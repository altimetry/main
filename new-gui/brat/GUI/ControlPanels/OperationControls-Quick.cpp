/*
* This file is part of BRAT 
*
* BRAT is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* BRAT is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include "new-gui/brat/stdafx.h"

#include "libbrathl/ProductNetCdf.h"

#include "BratLogger.h"

#include "DataModels/Model.h"
#include "DataModels/Workspaces/Workspace.h"
#include "DataModels/MapTypeDisp.h"

#include "new-gui/Common/GUI/TextWidget.h"
#include "GUI/ProgressDialog.h"
#include "GUI/DisplayEditors/MapEditor.h"
#include "GUI/DisplayEditors/PlotEditor.h"
#include "BratSettings.h"

#include "OperationControls.h"





// 1) change name from range_ku to range (range_ku and range always defined); range makes more sense

// 2) Add sla to quicks (alias sla in aliases.xml always defined where applicable)

// 3) New alias in aliases.xml: ocean_editing. Only applicable when ssh or sla are being calculated (otherwise ignore). If alias
//	ocean_editing exists and quick ssh or sla is being done, then apply alias in selection criteria. If alias ocean_editing  does 
//	not exist compute ssh and sla with empty selection criteria.


//static 
const std::vector<std::string> COperationControls::smQuickPredefinedVariableNames =
{
	"SSH",
	"SWH",
	"SLA",
	"Winds",
	"Sigma0",
	"Tracker Range",
};




/////////////////////////////////////////////////////////////////////////////////
//	Quick Operation Helpers
/////////////////////////////////////////////////////////////////////////////////

inline std::string QuickVariableAlias( COperationControls::EPredefinedVariables index )
{
	static std::vector< std::string > valiases =
	{
		"ssh",
		"swh",
        "sla",
		"winds",
		"sigma0",
        "range",
	};

	assert__( index < COperationControls::EPredefinedVariables_size );

	return valiases[ index ];
}


inline std::string QuickCriteriaAlias( COperationControls::EPredefinedSelectionCriteria index )
{
	static std::vector< std::string > valiases =
	{
		"ocean_editing",
	};

	assert__( index < COperationControls::EPredefinedSelectionCriteria_size );

	return valiases[ index ];
}



//static 
const std::string& COperationControls::QuickFindAliasValue( const CProductInfo &pi, EPredefinedVariables index )
{
    return pi.FindAliasValue( QuickVariableAlias( index ) );
}

//static 
const std::string& COperationControls::QuickFindAliasValue( const CProductInfo &pi, EPredefinedSelectionCriteria index )
{
    return pi.FindAliasValue( QuickCriteriaAlias( index ) );
}


CAliasInfo COperationControls::QuickFindField( const CProductInfo &pi, EPredefinedVariables index, bool &alias_used, std::string &field_error_msg )
{
    return pi.FindField( QuickVariableAlias( index ), mModel.Settings().mUseUnsupportedFields, alias_used, field_error_msg );
}

CAliasInfo COperationControls::QuickFindField( const CProductInfo &pi, EPredefinedSelectionCriteria index, bool &alias_used, std::string &field_error_msg )
{
    return pi.FindField( QuickCriteriaAlias( index ), mModel.Settings().mUseUnsupportedFields, alias_used, field_error_msg );
}


// Only works if a formula used the alias (name, not value) as description; which they don't, unless alias value 
//	and name are equal; this is for simplicity, to avoid having to use the product to retrieve the alias value
//
//static 
bool COperationControls::FormulaNameMatchesQuickAlias( const std::string &name, EPredefinedVariables index )
{
	return name == QuickVariableAlias( index );
}



std::string COperationControls::QuickDatasetSelectedName() const
{
	const int dataset_index = mQuickDatasetsCombo->currentIndex();
	if ( dataset_index < 0 )
		return "";

	return q2a( mQuickDatasetsCombo->itemText( dataset_index ) );
}
CDataset* COperationControls::QuickDatasetSelected() const
{
	return mWDataset ? mWDataset->GetDataset( QuickDatasetSelectedName() ) : nullptr;
}


// Checks fields according to quick operation formulas
//
void COperationControls::UpdateFieldsCheckState()
{
	//lambda

	auto set_gui_field = [this]( const std::string &name )
	{
		for ( int i = 0; i < EPredefinedVariables_size; ++i )
		{
			auto *item = mQuickVariablesList->item( i );
			if ( FormulaNameMatchesQuickAlias( name, (EPredefinedVariables)i ) )	//see not in FormulaMatchesQuickAlias
			{
				item->setFlags( item->flags() | Qt::ItemIsEnabled );
				item->setCheckState( Qt::Checked );
				//item->setData( Qt::UserRole, i );
			}
		}
	};


	//function body

	assert__( mQuickOperation );

	CMapFormula *formulas = mQuickOperation->GetFormulas();
	if ( !formulas )
		return;

	for ( CMapFormula::iterator it = formulas->begin(); it != formulas->end(); it++ )
	{
		CFormula* formula = mQuickOperation->GetFormula( it );
		if ( formula == nullptr )
			continue;

		switch ( formula->GetFieldType() )
		{
		case CMapTypeField::eTypeOpAsX:			assert__( formula->IsXType() || formula->IsLatLonDataType() );
			break;

		case CMapTypeField::eTypeOpAsY:			assert__( formula->IsYType() || formula->IsLatLonDataType() );
			break;

		case CMapTypeField::eTypeOpAsField:
			{
				//set_gui_field( formula->GetDescription() );
				set_gui_field( formula->GetName() );
			}
			break;

        default:
            break;
        }
	}
}


COperation* COperationControls::GetOrCreateEmptyQuickOperation()
{
	static const std::string opname = CWorkspaceOperation::QuickOperationName();

	if ( !mWOperation )
		return nullptr;

	// Retrieve quick operation from workspace, if any
	
	COperation *operation = mWOperation ? mWOperation->GetQuickOperation() : nullptr;
	if ( !operation )
	{
		if ( !mWOperation->InsertOperation( opname ) )
		{
			LOG_FATAL( "Unexpected v3 error...." );
			return nullptr;
		}
	}
	operation = mWOperation->GetOperation( opname );		assert__( operation );
	return operation;
}


/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
//	Quick Operation Entry Point
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

// Quick Operations entry point
//
//	If a workspace exists, even if empty, a quick operation is always created.
//
void COperationControls::HandleWorkspaceChanged_Quick()
{
	LOG_TRACEstd( "Quick operations page started handling signal to change workspace" );

	// 1. Retrieve /create, if any; only return null if mWOperation is null
	//
	mQuickOperation = GetOrCreateEmptyQuickOperation();	//always assign mQuickOperation if possible, and always do it first 

	// 2. Fill quick datasets
	//		- also: assigns dataset, enables all dataset dependent elements (fields and execution buttons, etc.)
	//		- can clear the operation formulas
	//
	mQuickInitializing = true;					//prevent clearing formulas. Ugly trick, but the simplest and quickest now
	HandleDatasetsChanged_Quick( nullptr );		//argument not used
	mQuickInitializing = false;

	if ( !mWOperation )
	{
		LOG_TRACEstd( "Quick operations page finished handling signal to change to NULL workspace" );
		return;
	}

	// 3. Always ensure quick operation existence
	//
	assert__( mQuickOperation );

	// 4. Select operation dataset in combo 
	//		- select dataset without signals (that is, no dataset assignment)
	//			- if recently created, and there are datasets, one was assigned
	//			- if existed, and there are datasets, one was assigned and/or selected
	//
	SelectOperationDatasetIndex( mQuickOperation, mQuickDatasetsCombo );

	// 5. Check fields according to quick operation formulas
	//		- assuming that this synchronization only needs to be done when setting 
	//			up a recently opened workspace:	the quick formulas are retrieved from 
	//			persistence (and reflected by this update) or are assigned by operation 
	//			execution according to fields check in the GUI. So, nothing in between 
	//			changes the quick formulas. If  dataset changes, all fields are unchecked.
	//
	UpdateFieldsCheckState();

	LOG_TRACEstd( "Quick operations page finished handling signal to change workspace" );
}


// - Fill datasets combo (no signals)
// - Select quick operation dataset
// - Trigger or call HandleSelectedDatasetChanged_Quick: 
//		- update dataset dependent GUI / data members, including dataset assignment if needed
// 
// - NOTE: connected by main window to datasets panel
// - triggered when whole datasets or a dataset composition changes; so when 
//		externally triggered, a workspace exists
//
void COperationControls::HandleDatasetsChanged_Quick( const CDataset *dataset )
{	
	Q_UNUSED( dataset );

	// 1. Refill datasets combo (no signals)

	mQuickDatasetsCombo->clear();
	if ( mWDataset )
	{
		mQuickDatasetsCombo->blockSignals( true );
		FillCombo( mQuickDatasetsCombo, *mWDataset->GetDatasets(),

			-1, true,

			[]( const CObMap::value_type &i ) -> const char*
			{
				return i.first.c_str();
			}
		);
		mQuickDatasetsCombo->blockSignals( false );
	}

	// 2. Select quick operation dataset (if any) OR automatically select (assign) one 

	int selected = -1;
	if ( mQuickOperation )
	{
		selected = mQuickDatasetsCombo->findText( mQuickOperation->OriginalDatasetName().c_str() );
		if ( selected < 0 && mQuickDatasetsCombo->count() > 0 )
			selected = 0;
	}

	// 3. Trigger or call HandleSelectedDatasetChanged_Quick: 
	//		- update in function of selected dataset (includes dataset assignment)

	if ( mQuickDatasetsCombo->currentIndex() == selected )
		HandleSelectedDatasetChanged_Quick( selected );
	else
		mQuickDatasetsCombo->setCurrentIndex( selected );
}


//	Update dataset dependent GUI/data elements/members
//	Clears operation formulas
//	Assign QuickOperation dataset, if any
//	Quit if there are no lon/lat fields
//	Enable quick data fields found in dataset
//	Call HandleVariableStateChanged_Quick: enable/disable execution buttons
//	ASSUMES: mQuickOperation is assigned if dataset_index >= 0
//
void COperationControls::HandleSelectedDatasetChanged_Quick( int dataset_index )
{
	LOG_TRACEstd( "Entering HandleSelectedDatasetChanged_Quick" );

	mQuickVariablesList->setEnabled( dataset_index >= 0 );			//items are enabled only if required field exists, regardless of whole list being enabled
	mQuickSelectionCriteriaCheck->setEnabled( dataset_index >= 0 );
	mOperationFilterButton_Quick->setEnabled( dataset_index >= 0 );	//assume that, if a dataset exists, it will be assigned and filtering makes sense

	//always disable execution in this function
	//	- enable (in HandleVariableStateChanged_Quick) only if valid dataset with required fields AND some are checked
	//
	mCanExecuteQuickOperation = false;
	mDisplayMapButton->setEnabled( mCanExecuteQuickOperation );
	mDisplayPlotButton->setEnabled( mCanExecuteQuickOperation );

	mQuickFieldDesc->clear();

	CDataset *dataset = QuickDatasetSelected();
	//TODO the 2nd branch of the conjunction prevents deleting formulas on initialization, but check if this branch is not undesirable when dataset composition changes
	if ( mQuickOperation && ( !dataset || ( dataset != mQuickOperation->OriginalDataset() ) ) )
		mQuickOperation->RemoveFormulas();

	if ( dataset_index < 0 )
	{
		if ( SelectedPanel() && !AdvancedMode() )
			UpdatePanelSelectionChange();
		LOG_TRACEstd( "Leaving HandleSelectedDatasetChanged_Quick with index < 0" );
		return;
	}

    WaitCursor wait;

	// 1. disable and check fields off
	//
	const int size = mQuickVariablesList->count();
	for ( int i = 0; i < size; ++i )
	{
		auto *item = mQuickVariablesList->item( i );
		item->setCheckState( Qt::Unchecked );					//triggers
		item->setFlags( item->flags() & ~Qt::ItemIsEnabled );
	}

	assert__( dataset );		assert__( mQuickOperation );

	// 2. re-assign dataset or filtered dataset 
	//		Here is the usage of mQuickInitializing ugly trick
	//
	if ( !mQuickInitializing )
		mQuickOperation->SetOriginalDataset( mWDataset, dataset->GetName() );


	// 3. check if dataset files are usable (have lon/lat fields); return if not
	//
	std::vector< std::string > dataset_files = dataset->GetFiles( true );	//TODO: IMPORTANT, REVIEW THIS true, put here because of rads
	bool has_lon_lat_fields = false;
	for ( auto const &path : dataset_files )
	{
		CProductInfo pi( dataset, path );
		if ( !pi.IsValid() )
			continue;

		std::string field_error_msg;
		bool lon_alias_used, lat_alias_used;
        std::pair<CAliasInfo, CAliasInfo> fields = pi.FindLonLatFields( mModel.Settings().mUseUnsupportedFields, lon_alias_used, lat_alias_used, field_error_msg );
		if ( !fields.first.Empty() && !fields.second.Empty() )
		{
			has_lon_lat_fields = true;
			break;
		}
	}

	bool has_selection_criteria = false;					//FIXME big hack see below (*)
	if ( has_lon_lat_fields )
	{
		// 4. enable quick data fields found in dataset
		//
		bool has_fields = false;
		for ( int i = 0; i < EPredefinedVariables_size; ++i )
		{
			auto *item = mQuickVariablesList->item( i );			assert__( item );
			for ( auto const &path : dataset_files )
			{
				CProductInfo pi( dataset, path );
				if ( !pi.IsValid() )
					continue;

				bool alias_used;
				std::string field_error_msg;
				EPredefinedVariables vi = (EPredefinedVariables)i;
				CAliasInfo alias_info = QuickFindField( pi, vi, alias_used, field_error_msg );
				const std::string expression = alias_info.Value();
				const CField *field = alias_info.Field();
				if ( !expression.empty() )
				{
					item->setFlags( item->flags() | Qt::ItemIsEnabled );
					has_fields = true;
                    std::string field_description = "[ %{" + QuickVariableAlias( vi ) + "}==" + expression + " ]";
					if ( field )
                        field_description += "\n\n => " + field->GetDescription();
                    item->setData( Qt::UserRole, t2q( field_description ) );
				}
				has_selection_criteria |= ( has_fields && !QuickFindAliasValue( pi, eOceanEditing ).empty() );		//TODO see above (*)
				if ( !expression.empty() )
					break;
			}
		}

		// 5. Call HandleVariableStateChanged_Quick to update execution buttons state
		//
		HandleVariableStateChanged_Quick( nullptr );
	}

	mQuickSelectionCriteriaCheck->setEnabled( has_selection_criteria );
	if ( !has_selection_criteria )
		mQuickSelectionCriteriaCheck->setChecked( false );

	if ( SelectedPanel() && !AdvancedMode() )
		UpdatePanelSelectionChange();

	LOG_TRACEstd( "Leaving HandleSelectedDatasetChanged_Quick with index >= 0" );
}


//	Update execution buttons state
//	NOTE: For simplicity reasons (not have to create and open product, etc.) mQuickOperation formulas are 
//		not updated here, only when the operation is executed; as a consequence, saving without executing 
//		will nor preserve the selected fields for the next application session.
//
void COperationControls::HandleVariableStateChanged_Quick( QListWidgetItem * )
{
	const int size = mQuickVariablesList->count();
	mCanExecuteQuickOperation = false;
	for ( int i = 0; i < size; ++i )
	{
		auto *item = mQuickVariablesList->item( i );
		if ( item->checkState() == Qt::Checked )
		{
			mCanExecuteQuickOperation = true;
			break;
		}
	}

	mDisplayMapButton->setEnabled( mCanExecuteQuickOperation );
	mDisplayPlotButton->setEnabled( mCanExecuteQuickOperation );

	if ( IsQuickOperationSelected() )
	{
		mAdvancedExecuteButton->setEnabled( mCanExecuteQuickOperation );
		mSwitchToMapButton->setEnabled( mCanExecuteQuickOperation );
		mSwitchToPlotButton->setEnabled( mCanExecuteQuickOperation );
	}
}


// Update field description
//
void COperationControls::HandleSelectedFieldChanged_Quick( int variable_index )
{
	mQuickFieldDesc->clear();
	if ( variable_index < 0 )
		return;

	auto *item = mQuickVariablesList->item( variable_index );
	mQuickFieldDesc->setText( item->data( Qt::UserRole ).toString() );
}


  //Insert("asField", eTypeOpAsField);
  //Insert("asX", eTypeOpAsX);
  //Insert("asY", eTypeOpAsY);


//bool CDisplayPanel::AddField( CDisplayData* data, bool preserveFieldProperties /* = false */ )
//{
//	if ( data == nullptr )
//	{
//		return false;
//	}
//	if ( m_display == nullptr )
//	{
//		return false;
//	}
//
//	int32_t typeDisplay = m_display->GetType();
//	int32_t typeDataToAdd = data->GetType();
//
//	if ( typeDisplay >= 0 )
//	{
//		if ( typeDataToAdd != typeDisplay )
//		{
//			wxMessageBox( wxString::Format( "The actual view type is '%s'. You can't add a '%s' data type,",
//				CMapTypeDisp::GetInstance().IdToName( typeDisplay ).c_str(),
//				CMapTypeDisp::GetInstance().IdToName( typeDataToAdd ).c_str() ),
//				"Warning",
//				wxOK | wxICON_HAND );
//			return false;
//		}
//	}
//
//	bool insert = false;
//	std::string dataKey;
//
//	dataKey = data->GetDataKey();
//
//
//	if ( m_display->ExistData( dataKey ) == true )
//	{
//		EnableCtrl();
//		return true;
//	}
//
//
//	CDisplayData* newdata = new CDisplayData( *data, wxGetApp().GetCurrentWorkspaceOperation() );
//
//	if ( ! preserveFieldProperties )
//	{
//		if ( wxGetApp().GetLastColorTable().IsEmpty() == false )
//		{
//			newdata->SetColorPalette( wxGetApp().GetLastColorTable().ToStdString() );
//		}
//		else
//		{
//			newdata->SetColorPalette( PALETTE_AEROSOL.c_str() );
//		}
//	}
//
//	insert = m_display->InsertData( GetDataKey( *data ).ToStdString(), newdata );
//
//	long indexInserted = GetDispDataSel()->InsertField( newdata );
//
//	GetDispType()->SetLabel( CMapTypeDisp::GetInstance().IdToName( m_display->GetType() ).c_str() );
//
//	CheckSelectedFields();
//
//	SetValueFieldGroup();
//
//	GetDispDataSel()->DeselectAll();
//	GetDispDataSel()->Select( indexInserted );
//
//	EnableCtrl();
//
//	return true;
//}


// Valid types
//		eTypeOpYFX,
//		eTypeOpZFXY
//
//Advanced operation creation sequence:
//
	// 1. create and insert operation
	// 2. Assign mCurrentOperation, init outputs, update data expressions tree
	// 3. Select data computation mode in GUI
	// 4. Select X in expression tree
	// 5. Assigns selected dataset
	// 6. Add new operation to GUI lits and select it (possibly triggers all handling operation change sequence)
	// 7. Select and re-assign operation record (v3 technique: amounts to update GUI with operation record and assign one to operation if none assigned)
//
//
COperation* COperationControls::CreateQuickOperation( CMapTypeOp::ETypeOp type )
{
	static const std::string opname = CWorkspaceOperation::QuickOperationName();

	WaitCursor wait;											assert__( mWRoot && QuickDatasetSelected() );

	// 1. create and insert operation

	COperation *operation = GetOrCreateEmptyQuickOperation();	assert__( operation );
	int current_index = mOperationsCombo->currentIndex();
	int quick_op_index = mOperationsCombo->findText( opname.c_str() );
	if ( quick_op_index == current_index )
		mOperationsCombo->setCurrentIndex( -1 );
	operation->Clear();							//TODO always recreates; simplify

	operation->InitOutputs( mWOperation );
	operation->SetType( type );

	const std::string filter_name = q2a( mOperationFilterButton_Quick->text() );
	if ( !filter_name.empty() )
	{
		auto *filter = mBratFilters.Find( filter_name );				assert__( filter );
		operation->SetFilter( filter );
	}
	operation->SetOriginalDataset( mWDataset, QuickDatasetSelectedName() );

	CProductInfo pi( operation->OriginalDataset() );
	if ( !pi.IsValid() )
	{
		std::string suggestion;
		if ( operation->Filter() )
			suggestion = "\nMaybe the operation's filter excluded all data files.";
		SimpleWarnBox( "There is no product for the operation to use." + suggestion );
		return nullptr;
	}
	//operation->SetProduct( product );	operation->InvalidateProduct();



	bool lon_alias_used, lat_alias_used;
	std::string field_error_msg;
    std::pair<CAliasInfo, CAliasInfo> lon_lat_fields = 
		pi.FindLonLatFields( mModel.Settings().mUseUnsupportedFields, lon_alias_used, lat_alias_used, field_error_msg );		assert__( !lon_lat_fields.first.Empty() && !lon_lat_fields.second.Empty() );

	if ( !operation->HasFormula() )
	{
		std::string field_record = lon_lat_fields.first.Field() ? lon_lat_fields.first.Field()->GetRecordName() : pi.AliasesRecord();
		operation->SetRecord( field_record );
		if ( operation->GetRecord().empty() && pi.IsNetCdf() )
			operation->SetRecord( CProductNetCdf::m_virtualRecordName );
	}

	std::string error_msg;
	CFormula* 
	formula = lon_lat_fields.first.Field() ?
		operation->NewUserFormula( error_msg, lon_lat_fields.first.Field(), CMapTypeField::eTypeOpAsX, true, pi ) :
		operation->NewUserFormula( error_msg, lon_name(), CMapTypeField::eTypeOpAsX, "", true );
	operation->ComputeInterval( formula, error_msg );

	formula = lon_lat_fields.second.Field() ?
		operation->NewUserFormula( error_msg, lon_lat_fields.second.Field(), CMapTypeField::eTypeOpAsY, true, pi ) :
		operation->NewUserFormula( error_msg, lat_name(), CMapTypeField::eTypeOpAsY, "", true );
	operation->ComputeInterval( formula, error_msg );

  //Insert("asField", eTypeOpAsField);
  //Insert("asX", eTypeOpAsX);
  //Insert("asY", eTypeOpAsY);

	std::vector< std::pair< std::string, std::string > > fields;

#if defined ONLY_USE_WITH_SLA_SSH
	bool add_selection_criteria = false;
#else
	bool add_selection_criteria = true;
#endif

	const int size = mQuickVariablesList->count();
	for ( int i = 0; i < size; ++i )
	{
		auto *item = mQuickVariablesList->item( i );
		if ( item->checkState() ==  Qt::Checked  )
		{
			EPredefinedVariables vi = (EPredefinedVariables)i;
			add_selection_criteria |= vi == eSSH || vi == eSLA;
			std::string expression = QuickFindAliasValue( pi, vi );
			if ( !expression.empty() )
				fields.push_back( { expression, QuickVariableAlias( vi ) } );
		}
	}									 assert__( fields.size() > 0 );

    add_selection_criteria = add_selection_criteria && mQuickSelectionCriteriaCheck->isChecked();

	for ( auto const &expression : fields )
    {
        std::string error_msg;
        CField *field = pi.FindFieldByName( expression.first, error_msg );
		//CField *field = product->FindFieldByName( expression.first, false, &error_msg );		//true: throw on failure
		error_msg.clear();
        CFormula *formula = operation->NewUserFormula( error_msg, expression.second, CMapTypeField::eTypeOpAsField, field ? field->GetUnit() : "", true );
		if ( error_msg.empty() )
			formula->SetDescription( expression.first );
		else
			SimpleWarnBox( error_msg );
	}

	std::string selection_criteria;
	if ( add_selection_criteria )
	{
		selection_criteria = QuickFindAliasValue( pi, eOceanEditing );
	}
	operation->GetSelect()->SetDescription( selection_criteria );

	return operation;
}


void COperationControls::SelectOperation( const std::string &name, bool select_map )
{
	COperation *operation = mWOperation->GetOperation( name );			Q_UNUSED( operation );		//release builds

	int new_index = mOperationsCombo->findText( name.c_str() );
	if ( new_index == -1 )
	{
		mOperationsCombo->addItem( name.c_str() );
		int current_index = mOperationsCombo->currentIndex();
		new_index = mOperationsCombo->findText( name.c_str() );
		if ( current_index == new_index )
			mOperationsCombo->setCurrentIndex( -1 );
	}
	mOperationsCombo->setCurrentIndex( new_index );

	assert__( mCurrentOperation == operation && mCurrentOriginalDataset == operation->OriginalDataset() );

	if ( select_map )
		mSwitchToMapButton->setChecked( true );
	else
		mSwitchToPlotButton->setChecked( true );
}


void COperationControls::HandleQuickMap()
{
	assert__( mWRoot );

	COperation *operation = CreateQuickOperation( CMapTypeOp::eTypeOpZFXY );
	if ( !operation )
	{
		return;
	}
	assert__( operation->IsMap() );

	SelectOperation( operation->GetName(), true );		assert__( operation == mCurrentOperation );
	
	ExecuteCurrentOperation();
}


void COperationControls::HandleQuickPlot()
{
	assert__( mWRoot );

	COperation *operation = CreateQuickOperation( CMapTypeOp::eTypeOpZFXY );
	if ( !operation )
	{
		return;
	}
	assert__( operation->IsMap() );

	SelectOperation( operation->GetName(), false );		assert__( operation == mCurrentOperation );

	ExecuteCurrentOperation();
}

