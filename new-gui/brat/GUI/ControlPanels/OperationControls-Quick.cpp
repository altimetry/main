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
#include "GUI/DisplayEditors/MapEditor.h"
#include "GUI/DisplayEditors/PlotEditor.h"

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
const std::string& COperationControls::QuickFindAliasValue( CProduct *product, EPredefinedVariables index )
{
	return FindAliasValue( product, QuickVariableAlias( index ) );
}

//static 
const std::string& COperationControls::QuickFindAliasValue( CProduct *product, EPredefinedSelectionCriteria index )
{
	return FindAliasValue( product, QuickCriteriaAlias( index ) );
}


//static
CField* COperationControls::QuickFindField( CProduct *product, EPredefinedVariables index, bool &alias_used, std::string &field_error_msg )
{
	return FindField( product, QuickVariableAlias( index ), alias_used, field_error_msg );
}
//static
CField* COperationControls::QuickFindField( CProduct *product, EPredefinedSelectionCriteria index, bool &alias_used, std::string &field_error_msg )
{
	return FindField( product, QuickCriteriaAlias( index ), alias_used, field_error_msg );
}



//static 
bool COperationControls::FormulaMatchesQuickAlias( const std::string &description, EPredefinedVariables index )
{
	return description == QuickVariableAlias( index );
}



CDataset* COperationControls::QuickDatasetSelected() const
{
	const int dataset_index = mQuickDatasetsCombo->currentIndex();
	if ( dataset_index < 0 )
		return nullptr;

	return mWDataset->GetDataset( q2a( mQuickDatasetsCombo->itemText( dataset_index ) ) );
}


// Checks fields according to quick operation formulas
//
void COperationControls::UpdateFieldsCheckState()
{
	//lambda

	auto set_field = [this]( const std::string &name )
	{
		for ( int i = 0; i < EPredefinedVariables_size; ++i )
		{
			auto *item = mQuickVariablesList->item( i );
			if ( FormulaMatchesQuickAlias( name, (EPredefinedVariables)i ) )	//aliases: assuming formulas use the alias as description
			{
				item->setFlags( item->flags() | Qt::ItemIsEnabled );
				item->setCheckState( Qt::Checked );
				item->setData( Qt::UserRole, i );
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
		case CMapTypeField::eTypeOpAsX:
			assert__( formula->IsXType() || formula->IsLatLonDataType() );
			break;
		case CMapTypeField::eTypeOpAsY:
			assert__( formula->IsYType() || formula->IsLatLonDataType() );
			break;
		case CMapTypeField::eTypeOpAsField:
			{
				set_field( formula->GetDescription() );
			}
			break;
        default:
            break;
        }
	}
}


COperation* COperationControls::CreateEmptyQuickOperation()
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
	// 1. Retrieve /create, if any; only return null if mWOperation is null
	//
	mQuickOperation = CreateEmptyQuickOperation();	//always assign mQuickOperation if possible, and always do it first 

	// 2. Fill quick datasets
	//		- also: assigns dataset, enables all dataset dependent elements (fields and execution buttons, etc.)
	//
	HandleDatasetsChanged_Quick( nullptr );

	if ( !mWOperation )
		return;

	// 3. Always ensure quick operation existence
	//
	assert__( mQuickOperation );

	// 4. Select operation dataset in combo 
	//		- no signals (*): 
	//			- if recently created, and there are datasets, one was assigned
	//			- if existed, and there are datasets, one was assigned and/or selected
	//
	SelectOperationDataset( mQuickOperation, mQuickDatasetsCombo, true );	//true == no signals (*)

	// 5. Check fields according to quick operation formulas
	//		- assuming that this synchronization only needs to be done when setting 
	//			up a recently opened workspace:	the quick formulas are retrieved from 
	//			persistence (and reflected by this update) or are assigned by operation 
	//			execution according to fields check in the GUI. So, nothing in between 
	//			changes the quick formulas. If  dataset changes, all fields are unchecked.
	//
	UpdateFieldsCheckState();
}


// - Fill datasets combo (no signals)
// - Select quick operation dataset
// - Trigger or call HandleSelectedDatasetChanged_Quick: 
//		- update dataset dependent GUI / data members, including dataset assignment if needed
// 
// - connected by main window to datasets panel
// - triggered when whole datasets or a dataset composition changes; so when 
//		externally triggered, a workspace exists
//
void COperationControls::HandleDatasetsChanged_Quick( CDataset * )
{	
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

	if ( mQuickOperation )
		RemoveOperationFormulas( mQuickOperation );

    if ( dataset_index < 0 )
        return;

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

	// 2. assign dataset
	//
	CDataset *dataset = QuickDatasetSelected();			assert__( dataset );		assert__( mQuickOperation );
	mQuickOperation->SetDataset( dataset );

	// 3. check if dataset files are usable (have lon/lat fields); return if not
	//
	std::vector<std::string> dataset_files;
	dataset->GetFiles( dataset_files );
	bool has_lon_lat_fields = false;
	for ( auto const &path : dataset_files )
	{
		CProduct *product = dataset->OpenProduct( path );
		if ( !product )
			continue;

		std::string field_error_msg;
		bool alias_used;
		std::pair<CField*, CField*> fields = FindLonLatFields( product, alias_used, field_error_msg );
		delete product;
		if ( fields.first && fields.second )
		{
			has_lon_lat_fields = true;
			break;
		}
	}

	bool has_selection_criteria = false;					//TODO big hack see below (*)
	if ( has_lon_lat_fields )
	{
		// 4. enable quick data fields found in dataset
		//
		bool has_fields = false;
		for ( int i = 0; i < EPredefinedVariables_size; ++i )	//TODO maybe this should be corrected to open products only once, not once per iteration
		{
			auto *item = mQuickVariablesList->item( i );			assert__( item );
			for ( auto const &path : dataset_files )
			{
				CProduct *product = dataset->OpenProduct( path );
				if ( !product )
					continue;

				bool alias_used;
				std::string field_error_msg;
				EPredefinedVariables vi = (EPredefinedVariables)i;
				std::string expression = QuickFindAliasValue( product, vi );
				CField *field = QuickFindField( product, vi, alias_used, field_error_msg );
				//if ( field )
				if ( !expression.empty() )
				{
					item->setFlags( item->flags() | Qt::ItemIsEnabled );
					has_fields = true;
					if ( field )
						item->setData( Qt::UserRole, t2q( field->GetDescription() ) );
				}
				has_selection_criteria |= ( has_fields && !QuickFindAliasValue( product, eOceanEditing ).empty() );		//TODO see above (*)
				delete product;
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
}


// Update execution buttons state
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
	// 5. Assigns selected dataset and mProduct
	// 6. Add new operation to GUI lits and select it (possibly triggers all handling operation change sequence)
	// 7. Select and re-assign operation record (v3 technique: amounts to update GUI with operation record and assign one to operation if none assigned)
//
//
COperation* COperationControls::CreateQuickOperation( CMapTypeOp::ETypeOp type )
{
	static const std::string opname = CWorkspaceOperation::QuickOperationName();

	WaitCursor wait;											assert__( mWRoot && QuickDatasetSelected() );

	// 1. create and insert operation

	COperation *operation = CreateEmptyQuickOperation();		assert__( operation );
	if ( operation )
	{
		int current_index = mOperationsCombo->currentIndex();
		int quick_op_index = mOperationsCombo->findText( opname.c_str() );
		if ( quick_op_index == current_index )
			mOperationsCombo->setCurrentIndex( -1 );
		operation->Clear();
	}

	operation->InitOutputs( mWOperation );	//operation->InitExportAsciiOutput( mWOperation );
	operation->SetType( type );

	operation->SetDataset( QuickDatasetSelected() );
	CProduct *product = const_cast<const COperation*>( operation )->Dataset()->OpenProduct();
	operation->SetProduct( product );

	std::string operation_record = operation->GetRecord();

	bool alias_used;
	std::string field_error_msg;
	std::pair<CField*, CField*> lon_lat_fields = FindLonLatFields( product, alias_used, field_error_msg );		assert__( lon_lat_fields.first && lon_lat_fields.second );

	std::string error_msg;
	if ( !operation->HasFormula() )
	{
		std::string field_record = lon_lat_fields.first->GetRecordName();
		operation->SetRecord( field_record );
		if ( operation->GetRecord().empty() && product->IsNetCdf() )
			operation->SetRecord( CProductNetCdf::m_virtualRecordName );
	}
	CFormula* formula = operation->NewUserFormula( error_msg, lon_lat_fields.first, CMapTypeField::eTypeOpAsX, true, product );
	operation->ComputeInterval( formula, error_msg );

	formula = operation->NewUserFormula( error_msg, lon_lat_fields.second, CMapTypeField::eTypeOpAsY, true, product );
	operation->ComputeInterval( formula, error_msg );

  //Insert("asField", eTypeOpAsField);
  //Insert("asX", eTypeOpAsX);
  //Insert("asY", eTypeOpAsY);

	std::vector< std::string > fields;

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
			std::string expression = QuickFindAliasValue( product, vi );
			if ( !expression.empty() )
				fields.push_back( expression );
		}
	}									 assert__( fields.size() > 0 );

	add_selection_criteria &= mQuickSelectionCriteriaCheck->isChecked();

	for ( auto const &expression : fields )
	{
		std::string error_msg;
		CFormula *formula = operation->NewUserFormula( error_msg, operation->GetFormulaNewName(), CMapTypeField::eTypeOpAsField, "", true );
		if ( error_msg.empty() )
			formula->SetDescription( expression );
		else
			SimpleWarnBox( error_msg );
	}

	std::string selection_criteria;
	if ( add_selection_criteria )
	{
		selection_criteria = QuickFindAliasValue( product, eOceanEditing );
	}
	operation->GetSelect()->SetDescription( selection_criteria );

	return operation;
}


//COperation* COperationControls::CreateQuickOperation( CMapTypeOp::ETypeOp type )
//{
//	static const std::string opname = CWorkspaceOperation::QuickOperationName();
//
//	WaitCursor wait;											assert__( mWRoot && QuickDatasetSelected() );
//
//	// 1. create and insert operation
//
//	COperation *operation = CreateEmptyQuickOperation();		assert__( operation );
//	if ( operation )
//	{
//		int current_index = mOperationsCombo->currentIndex();
//		int quick_op_index = mOperationsCombo->findText( opname.c_str() );
//		if ( quick_op_index == current_index )
//			mOperationsCombo->setCurrentIndex( -1 );
//		operation->Clear();
//	}
//
//	operation->InitOutput( mWOperation );
//	operation->InitExportAsciiOutput( mWOperation );
//	operation->SetType( type );
//
//	operation->SetDataset( QuickDatasetSelected() );
//	CProduct *product = const_cast<const COperation*>( operation )->Dataset()->OpenProduct();
//	operation->SetProduct( product );
//
//	std::string operation_record = operation->GetRecord();
//
//	std::vector< CField*> fields;
//	bool alias_used;
//	std::string field_error_msg;
//	std::pair<CField*, CField*> lon_lat_fields = FindLonLatFields( product, alias_used, field_error_msg );		assert__( lon_lat_fields.first && lon_lat_fields.second );
//
//	std::string error_msg;
//	if ( !operation->HasFormula() )
//	{
//		std::string field_record = lon_lat_fields.first->GetRecordName();
//		operation->SetRecord( field_record );
//		if ( operation->GetRecord().empty() && product->IsNetCdf() )
//			operation->SetRecord( CProductNetCdf::m_virtualRecordName );
//	}
//	CFormula* formula = operation->NewUserFormula( error_msg, lon_lat_fields.first, CMapTypeField::eTypeOpAsX, true, product );
//	operation->ComputeInterval( formula, error_msg );
//
//	formula = operation->NewUserFormula( error_msg, lon_lat_fields.second, CMapTypeField::eTypeOpAsY, true, product );
//	operation->ComputeInterval( formula, error_msg );
//
//  //Insert("asField", eTypeOpAsField);
//  //Insert("asX", eTypeOpAsX);
//  //Insert("asY", eTypeOpAsY);
//
//	CField *field = nullptr;
//	const int size = mQuickVariablesList->count();
//	bool add_selection_criteria = false;
//	for ( int i = 0; i < size; ++i )
//	{
//		auto *item = mQuickVariablesList->item( i );
//		if ( item->checkState() ==  Qt::Checked  )
//		{
//			EPredefinedVariables vi = (EPredefinedVariables)i;
//			add_selection_criteria |= vi == eSSH || vi == eSLA;
//			field = QuickFindField( product, vi, alias_used, field_error_msg );
//			if ( field )
//				fields.push_back( field );
//		}
//	}									 assert__( fields.size() > 0 );
//
//	for ( auto *field : fields )
//	{
//		std::string field_record = field->GetRecordName();
//		if ( !operation->HasFormula() )
//		{
//		//	operation->SetRecord( field_record );
//		//	if ( operation->GetRecord().empty() && product->IsNetCdf() )
//		//		operation->SetRecord( CProductNetCdf::m_virtualRecordName );
//		}
//		CFormula* formula = operation->NewUserFormula( error_msg, field, CMapTypeField::eTypeOpAsField, true, product );
//		if ( !error_msg.empty() )
//			SimpleErrorBox( error_msg );
//		//Add( theParentId, formula );
//
//        Q_UNUSED( formula)
//	}
//
//	std::string selection_criteria;
//	if ( add_selection_criteria )
//	{
//		selection_criteria = QuickFindAliasValue( product, eOceanEditing );
//	}
//	operation->GetSelect()->SetDescription( selection_criteria );
//
//	return operation;
//}


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

