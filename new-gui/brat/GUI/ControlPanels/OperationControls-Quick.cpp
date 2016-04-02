#include "new-gui/brat/stdafx.h"

#include "libbrathl/ProductNetCdf.h"

#include "ApplicationLogger.h"

#include "DataModels/Model.h"
#include "DataModels/Workspaces/Workspace.h"
#include "DataModels/MapTypeDisp.h"

#include "GUI/DisplayEditors/MapEditor.h"
#include "GUI/DisplayEditors/PlotEditor.h"

//HAMMER SECTION - begin
#include "DataModels/DisplayFilesProcessor.h"
#include "DataModels/PlotData/ZFXYPlot.h"
#include "DataModels/PlotData/XYPlot.h"
#include "DataModels/PlotData/WorldPlot.h"
//HAMMER SECTION - end


#include "OperationControls.h"

//
//
//inline int ItemRow( QListWidgetItem *item )
//{
//	return item->listWidget()->row( item );
//}
//inline int FindText( QListWidget *list, const char *text  )
//{
//    auto items = list->findItems( text, Qt::MatchExactly );			assert__( items.size() == 1 );
//	return ItemRow( items[ 0 ] );
//}
//inline QString ItemText( QListWidget *list, int index  )
//{
//	auto *item = list->item( index );
//	return item->text();
//}
//


std::string PseudoAlias( const std::string &name )
{
	static std::map<std::string, std::string> alias_map =
	{
		{ COperationControls::smPredefinedVariables[ COperationControls::eSSH ], "ssha" },
		{ COperationControls::smPredefinedVariables[ COperationControls::eSWH ], "swh_ku" },
		{ COperationControls::smPredefinedVariables[ COperationControls::eSigma0 ], "sig0_ku" },
		{ COperationControls::smPredefinedVariables[ COperationControls::eWinds ], "wind_speed_alt" }
	};

	return alias_map[ name ];
}
//lon
//lat
//time
//ssha (isto é para o SSH)
//swh_ku (isto é para o SWH)
//sig0_ku (isto é para o sigma0)
//wind_speed_alt (isto é para Winds)
//range_ku (isto é para Tracker range)


/////////////////////////////////////////////////////////////////////////////////
//						
/////////////////////////////////////////////////////////////////////////////////


CDataset* COperationControls::QuickDataset()
{
	const int dataset_index = mQuickDatasetsCombo->currentIndex();
	if ( dataset_index < 0 )
		return nullptr;

	return mWDataset->GetDataset( q2a( mQuickDatasetsCombo->itemText( dataset_index ) ) );
}

void COperationControls::HandleDatasetsChanged_Quick( CDataset * )
{
	int selected = mQuickDatasetsCombo->currentIndex();

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

	if ( selected >= mQuickDatasetsCombo->count() )
		selected = 0;

	if ( mQuickDatasetsCombo->currentIndex() == selected )
		HandleSelectedDatasetChanged_Quick( selected );
	else
		mQuickDatasetsCombo->setCurrentIndex( selected );
}

void COperationControls::HandleSelectedDatasetChanged_Quick( int dataset_index )
{
	mQuickVariablesList->setEnabled( dataset_index >= 0 );

    if ( dataset_index < 0 )
        return;

    WaitCursor wait;

	const int size = mQuickVariablesList->count();
	for ( int i = 0; i < size; ++i )
	{
		auto *item = mQuickVariablesList->item( i );
		item->setCheckState( Qt::Unchecked );						 //triggers
		item->setFlags( item->flags() &~ ( Qt::ItemIsEnabled ) );
	}
	mDisplayMapButton->setEnabled( false );
	mDisplayPlotButton->setEnabled( false );

	CDataset *dataset = QuickDataset();			assert__( dataset );
	std::vector<std::string> paths;
	dataset->GetFiles( paths );

	bool has_lon_lat_fields = false;
	for ( auto const &path : paths )
	{
		CProduct *product = dataset->SetProduct( path );
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
	if ( !has_lon_lat_fields )
		return;


	bool has_fields = false;
	for ( int i = 0; i < EPredefinedVariables_size; ++i )	//TODO this should be corrected to open products only once, not once per iteration
	{
		auto *item = mQuickVariablesList->item( i );
		for ( auto const &path : paths )
		{
			CProduct *product = dataset->SetProduct( path );
			if ( !product )
				continue;

			bool alias_used;
			std::string field_error_msg;
			CField *field = FindField( product, PseudoAlias( smPredefinedVariables[ i ] ), alias_used, field_error_msg );
			delete product;
			if ( field )
			{
				item->setFlags( item->flags() | Qt::ItemIsEnabled );
				item->setCheckState( Qt::Checked );
				has_fields = true;
				break;
			}
		}
	}
	mDisplayMapButton->setEnabled( has_fields );
	mDisplayPlotButton->setEnabled( has_fields );
}



void COperationControls::HandleVariableStateChanged_Quick( QListWidgetItem * )
{
	const int size = mQuickVariablesList->count();
	bool enable = false;
	for ( int i = 0; i < size; ++i )
	{
		auto *item = mQuickVariablesList->item( i );
		if ( item->checkState() == Qt::Checked )
		{
			enable = true;
			break;
		}
	}
	mDisplayMapButton->setEnabled( enable );
	mDisplayPlotButton->setEnabled( enable );
}

void COperationControls::HandleSelectedVariableChanged_Quick( int )
{
	LOG_WARN("HandleSelectedVariableChanged_Quick");
}


//void CDisplayPanel::GetOperations( int32_t type /*= -1*/ )
//{
//	UNUSED( type );
//
//	m_dataList.RemoveAll();
//
//	CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();
//	if ( wks == nullptr )
//	{
//		return;
//	}
//
//	CObMap* operations = wks->GetOperations();
//	if ( operations == nullptr )
//	{
//		return;
//	}
//
//
//	CInternalFiles *file = nullptr;
//	CUIntArray displayTypes;
//
//	CStringArray names;
//
//	CObMap::iterator it;
//
//	for ( it = operations->begin(); it != operations->end(); it++ )
//	{
//
//		COperation* operation = dynamic_cast<COperation*>( it->second );
//		if ( operation == nullptr )
//		{
//			continue;
//		}
//
//		displayTypes.RemoveAll();
//
//		CDisplay::GetDisplayType( operation, displayTypes, &file );
//
//		if ( file == nullptr )
//		{
//			continue;
//		}
//
//		names.RemoveAll();
//
//		file->GetDataVars( names );
//
//		CUIntArray::iterator itDispType;
//		CStringArray::iterator itField;
//		CDisplayData* displayData;
//
//		for ( itDispType = displayTypes.begin(); itDispType != displayTypes.end(); itDispType++ )
//		{
//			for ( itField = names.begin(); itField != names.end(); itField++ )
//			{
//				CStringArray varDimensions;
//				file->GetVarDims( *itField, varDimensions );
//
//				uint32_t nbDims = varDimensions.size();
//
//				if ( nbDims > 2 )
//				{
//					continue;
//				}
//
//				if ( ( nbDims != 2 ) && ( ( *itDispType == CMapTypeDisp::eTypeDispZFXY )
//					|| ( *itDispType == CMapTypeDisp::eTypeDispZFLatLon ) ) )
//				{
//					continue;
//				}
//
//				displayData = new CDisplayData( operation );
//
//				displayData->SetType( *itDispType );
//
//				displayData->GetField()->SetName( *itField );
//
//				std::string unit = file->GetUnit( *itField ).GetText();
//				displayData->GetField()->SetUnit( unit );
//
//				wxString comment = file->GetComment( *itField ).c_str();
//
//				wxString description = file->GetTitle( *itField ).c_str();
//
//				if ( !comment.IsEmpty() )
//				{
//					description += "." + comment;
//				}
//
//				displayData->GetField()->SetDescription( (const char *)description.c_str() );
//
//				if ( nbDims >= 1 )
//				{
//					std::string dimName = varDimensions.at( 0 );
//					displayData->GetX()->SetName( varDimensions.at( 0 ) );
//
//					std::string unit = file->GetUnit( dimName ).GetText();
//					displayData->GetX()->SetUnit( unit );
//
//					displayData->GetX()->SetDescription( file->GetTitle( dimName ) );
//				}
//
//				if ( nbDims >= 2 )
//				{
//					std::string dimName = varDimensions.at( 1 );
//					displayData->GetY()->SetName( varDimensions.at( 1 ) );
//
//					std::string unit = file->GetUnit( dimName ).GetText();
//					displayData->GetY()->SetUnit( unit );
//
//					displayData->GetY()->SetDescription( file->GetTitle( dimName ) );
//				}
//
//				if ( nbDims >= 3 )
//				{
//					std::string dimName = varDimensions.at( 2 );
//					displayData->GetZ()->SetName( varDimensions.at( 2 ) );
//
//					std::string unit = file->GetUnit( dimName ).GetText();
//					displayData->GetZ()->SetUnit( unit );
//
//					displayData->GetZ()->SetDescription( file->GetTitle( dimName ) );
//				}
//
//				m_dataList.Insert( (const char *)GetDataKey( *displayData ).c_str(), displayData, false );
//			}
//
//		}
//
//		delete file;
//		file = nullptr;
//	}
//	delete file;
//	file = nullptr;
//}


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
COperation* COperationControls::CreateQuickOperation( CMapTypeOp::ETypeOp type )
{
	static const std::string opname = CWorkspaceOperation::QuickOperationName();

	WaitCursor wait;				assert__( mWRoot );

	COperation *operation = mWOperation->GetOperation( opname );
	if ( operation )
	{
		int current_index = mOperationsCombo->currentIndex();
		int op_index = mOperationsCombo->findText( opname.c_str() );
		if ( op_index == current_index )
			mOperationsCombo->setCurrentIndex( -1 );
		operation->Clear();
	}
	else
	if ( !mWOperation->InsertOperation( opname ) )		//v4 this is weired: call to GetOperationNewName ensures not existing name
	{
		LOG_FATAL( "Unexpected v3 error...." );
		return nullptr;
	}
	operation = mWOperation->GetOperation( opname );		assert__( operation );

	operation->InitOutput( mWOperation );
	operation->InitExportAsciiOutput( mWOperation );
	operation->SetType( type );

	operation->SetDataset( QuickDataset() );
	CProduct *product = operation->GetDataset()->SetProduct();
	operation->SetProduct( product );

	std::string operation_record = operation->GetRecord();

	std::vector< CField*> fields;
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

	CField *field = nullptr;
	const int size = mQuickVariablesList->count();
	for ( int i = 0; i < size; ++i )
	{
		auto *item = mQuickVariablesList->item( i );
		if ( item->checkState() ==  Qt::Checked  )
		{
			field = FindField( product, PseudoAlias( smPredefinedVariables[ i ] ), alias_used, field_error_msg );
			if ( field )
				fields.push_back( field );
		}
	}									 assert__( fields.size() > 0 );

	for ( auto *field : fields )
	{
		std::string field_record = field->GetRecordName();
		if ( !operation->HasFormula() )
		{
		//	operation->SetRecord( field_record );
		//	if ( operation->GetRecord().empty() && product->IsNetCdf() )
		//		operation->SetRecord( CProductNetCdf::m_virtualRecordName );
		}
		CFormula* formula = operation->NewUserFormula( error_msg, field, CMapTypeField::eTypeOpAsField, true, product );
		if ( !error_msg.empty() )
			SimpleErrorBox( error_msg );
		//Add( theParentId, formula );

        Q_UNUSED( formula)
	}

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

	assert__( mCurrentOperation == operation && mDataset == operation->GetDataset() );

	if ( select_map )
		mSwitchToMapButton->setChecked( true );
	else
		mSwitchToPlotButton->setChecked( true );
}


void COperationControls::HandleQuickMap()
{
	WaitCursor wait;				assert__( mWRoot );

	COperation *operation = CreateQuickOperation( CMapTypeOp::eTypeOpZFXY );
	if ( !operation )
	{
		return;
	}
	assert__( operation->IsMap() );

	SelectOperation( operation->GetName(), true );

	if ( !Execute( true ) )
		return;

	//openTestFile( t2q( mDesktopManager->mPaths.mWorkspacesDir + R"(/newWP/Displays/DisplayDisplays_New.par)" ) );
}


void COperationControls::HandleQuickPlot()
{
	WaitCursor wait;				assert__( mWRoot );

	COperation *operation = CreateQuickOperation( CMapTypeOp::eTypeOpZFXY );
	if ( !operation )
	{
		return;
	}
	assert__( operation->IsMap() );

	SelectOperation( operation->GetName(), false );

	if ( !Execute( true ) )
		return;

	//openTestFile( t2q( mDesktopManager->mPaths.mWorkspacesDir + R"(/newWP/Displays/DisplayDisplays_New.par)" ) );
}



//L:\project\workspaces\newWP\Displays\DisplayDisplays_New.par

void COperationControls::openTestFile( const QString &fileName )
{
	delete mCmdLineProcessor;
	mCmdLineProcessor = new CDisplayFilesProcessor( false );

	const std::string s = q2a( fileName );
	const char *argv[] = { "", s.c_str() };
	try
	{
		if ( mCmdLineProcessor->Process( 2, argv ) )
		{
			auto &plots = mCmdLineProcessor->plots();
			if ( mCmdLineProcessor->isZFLatLon() )		// =================================== WorldPlot
			{
				for ( auto &plot : plots )
				{
					CWPlot* wplot = dynamic_cast<CWPlot*>( plot );
					if ( wplot == nullptr )
						continue;

					auto ed = new CMapEditor( mCmdLineProcessor, wplot, this );
					auto subWindow = mDesktopManager->AddSubWindow( ed );
					subWindow->show();
				}
			}
			else if ( mCmdLineProcessor->isYFX() )		// =================================== XYPlot();
			{
				for ( auto &plot : plots )
				{
					CPlot* yfxplot = dynamic_cast<CPlot*>( plot );
					if ( yfxplot == nullptr )
						continue;

					auto ed = new CPlotEditor( mCmdLineProcessor, yfxplot, this );
					auto subWindow = mDesktopManager->AddSubWindow( ed );
					subWindow->show();
				}
			}
			else if ( mCmdLineProcessor->isZFXY() )		// =================================== ZFXYPlot();
			{
				for ( auto &plot : plots )
				{
					CZFXYPlot* zfxyplot = dynamic_cast<CZFXYPlot*>( plot );
					if ( zfxyplot == nullptr )
						continue;

					auto ed = new CPlotEditor( mCmdLineProcessor, zfxyplot, this );
					auto subWindow = mDesktopManager->AddSubWindow( ed );
					subWindow->show();
				}
			}
			else
			{
				CException e( "CBratDisplayApp::OnInit - Only World Plot Data, XY Plot Data and ZFXY Plot Data are implemented", BRATHL_UNIMPLEMENT_ERROR );
				LOG_TRACE( e.what() );
				throw e;
			}
		}
	}
	catch ( CException &e )
	{
		SimpleErrorBox( e.what() );
		throw;
	}
	catch ( std::exception &e )
	{
		SimpleErrorBox( e.what() );
		throw;
	}
	catch ( ... )
	{
		SimpleErrorBox( "Unexpected error encountered" );
		throw;
	}
}
//HAMMER SECTION
