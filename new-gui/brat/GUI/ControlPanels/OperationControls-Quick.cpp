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



inline int ItemRow( QListWidgetItem *item )
{
	return item->listWidget()->row( item );
}
inline int FindText( QListWidget *list, const char *text  )
{
    auto items = list->findItems( text, Qt::MatchExactly );			assert__( items.size() == 1 );
	return ItemRow( items[ 0 ] );
}
inline QString ItemText( QListWidget *list, int index  )
{
	auto *item = list->item( index );
	return item->text();
}


/////////////////////////////////////////////////////////////////////////////////
//						Launch Display Actions
/////////////////////////////////////////////////////////////////////////////////


CDataset* COperationControls::QuickDataset()
{
	const int dataset_index = mQuickDatasetsCombo->currentIndex();
	if ( dataset_index < 0 )
		return nullptr;

	return mWDataset->GetDataset( q2a( mQuickDatasetsCombo->itemText( dataset_index ) ) );
}

void COperationControls::HandleDatasetsChanged_Quick()
{
	int selected = mQuickDatasetsCombo->currentIndex();

	mQuickDatasetsCombo->blockSignals( true );
    FillCombo( mQuickDatasetsCombo, *mWDataset->GetDatasets(),

        -1, true,

        []( const CObMap::value_type &i ) -> const char*
        {
            return i.first.c_str();
        }
    );
	mQuickDatasetsCombo->blockSignals( false );

	mQuickDatasetsCombo->setCurrentIndex( selected < 0 ? 0 : selected );
}

void COperationControls::HandleSelectedDatasetChanged_Quick( int dataset_index )
{
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
			CField *field = FindField( product, smPredefinedVariables[ i ], alias_used, field_error_msg );
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


///////////////////////////////////////////////////////////////////////////////////////////
bool COperationControls::CreateDisplayData( COperation *operation, CMapDisplayData &m_dataList )
{
	CInternalFiles *file = nullptr;
	CUIntArray displayTypes;
	CDisplay::GetDisplayType( operation, displayTypes, &file );
	if ( file == nullptr )
	{
		return false;
	}

	CStringArray names;
	names.RemoveAll();
	file->GetDataVars( names );

	for ( CUIntArray::iterator itDispType = displayTypes.begin(); itDispType != displayTypes.end(); itDispType++ )
	{
		for ( CStringArray::iterator itField = names.begin(); itField != names.end(); itField++ )
		{
			CStringArray varDimensions;
			file->GetVarDims( *itField, varDimensions );

			uint32_t nbDims = varDimensions.size();

			if ( nbDims > 2 )
			{
				continue;
			}

			if ( ( nbDims != 2 ) && ( ( *itDispType == CMapTypeDisp::eTypeDispZFXY )
				|| ( *itDispType == CMapTypeDisp::eTypeDispZFLatLon ) ) )
			{
				continue;
			}

			CDisplayData* displayData = new CDisplayData( operation );

			displayData->SetType( *itDispType );

			displayData->GetField()->SetName( *itField );

			std::string unit = file->GetUnit( *itField ).GetText();
			displayData->GetField()->SetUnit( unit );

			std::string comment = file->GetComment( *itField );
			std::string description = file->GetTitle( *itField );

			if ( !comment.empty() )
			{
				description += "." + comment;
			}

			displayData->GetField()->SetDescription( (const char *)description.c_str() );

			if ( nbDims >= 1 )
			{
				std::string dimName = varDimensions.at( 0 );
				displayData->GetX()->SetName( varDimensions.at( 0 ) );

				std::string unit = file->GetUnit( dimName ).GetText();
				displayData->GetX()->SetUnit( unit );

				displayData->GetX()->SetDescription( file->GetTitle( dimName ) );
			}

			if ( nbDims >= 2 )
			{
				std::string dimName = varDimensions.at( 1 );
				displayData->GetY()->SetName( varDimensions.at( 1 ) );

				std::string unit = file->GetUnit( dimName ).GetText();
				displayData->GetY()->SetUnit( unit );

				displayData->GetY()->SetDescription( file->GetTitle( dimName ) );
			}

			if ( nbDims >= 3 )
			{
				std::string dimName = varDimensions.at( 2 );
				displayData->GetZ()->SetName( varDimensions.at( 2 ) );

				std::string unit = file->GetUnit( dimName ).GetText();
				displayData->GetZ()->SetUnit( unit );

				displayData->GetZ()->SetDescription( file->GetTitle( dimName ) );
			}

			m_dataList.Insert( displayData->GetDataKey(), displayData, false );
		}
	}
	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////

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


//void CDisplayPanel::Execute()
//{
//	if ( m_display == nullptr )
//	{
//		return;
//	}
//
//	wxString msg;
//
//	bool bOk = Control( msg );
//
//	if ( !bOk )
//	{
//		wxMessageBox( msg, "Warning", wxOK | wxICON_EXCLAMATION );
//		return;
//	}
//
//	bOk = RefreshSelectedData();
//
//	if ( !bOk )
//	{
//		return;
//	}
//
//	BuildCmdFile();
//
//	wxGetApp().GotoLogPage();
//
//
//	CProcess* process = new CProcess( m_display->GetTaskName(),
//		wxGetApp().GetLogPanel(),
//		GetFullCmd(),
//		nullptr );
//
//	bOk = wxGetApp().GetLogPanel()->AddProcess( process, true );
//
//	if ( bOk == false )
//	{
//		delete process;
//		process = nullptr;
//	}
//}


void COperationControls::HandleQuickMap()
{
	WaitCursor wait;				assert__( mModel );

	std::string opname = mWOperation->GetOperationNewName();

	if ( !mWOperation->InsertOperation( opname ) )		//v4 this is weired: call to GetOperationNewName ensures not existing name
	{
		LOG_FATAL( "Unexpected v3 error...." );
		return;
	}

	mOperation = mWOperation->GetOperation( opname );
	mOperation->InitOutput( mWOperation );
	mOperation->InitExportAsciiOutput( mWOperation );
	mOperation->SetType( CMapTypeOp::eTypeOpZFXY );

	mOperation->SetDataset( QuickDataset() );
	mProduct = mOperation->GetDataset()->SetProduct();
	mOperation->SetProduct( mProduct );

	std::string operation_record = mOperation->GetRecord();

	std::vector< CField*> fields;
	bool alias_used;
	std::string field_error_msg;
	std::pair<CField*, CField*> lon_lat_fields = FindLonLatFields( mProduct, alias_used, field_error_msg );		assert__( lon_lat_fields.first && lon_lat_fields.second );

	std::string error_msg;
	if ( !mOperation->HasFormula() )
	{
		std::string field_record = lon_lat_fields.first->GetRecordName();
		mOperation->SetRecord( field_record );
		if ( mOperation->GetRecord().empty() && mProduct->IsNetCdf() )
			mOperation->SetRecord( CProductNetCdf::m_virtualRecordName );
	}
	CFormula* formula = mOperation->NewUserFormula( error_msg, lon_lat_fields.first, CMapTypeField::eTypeOpAsX, true, mProduct );
	mOperation->ComputeInterval( formula, error_msg );

	formula = mOperation->NewUserFormula( error_msg, lon_lat_fields.second, CMapTypeField::eTypeOpAsY, true, mProduct );
	mOperation->ComputeInterval( formula, error_msg );

  //Insert("asField", eTypeOpAsField);
  //Insert("asX", eTypeOpAsX);
  //Insert("asY", eTypeOpAsY);

	CField *field = nullptr;
	//for ( int i = 0; i < EPredefinedVariables_size; ++i )	//TODO this should be corrected to open products only once, not once per iteration
	//{
	//	field = FindField( mProduct, smPredefinedVariables[ i ], alias_used, field_error_msg );
	//	if ( field )
	//		fields.push_back( field );
	//}
	const int size = mQuickVariablesList->count();
	for ( int i = 0; i < size; ++i )
	{
		auto *item = mQuickVariablesList->item( i );
		if ( item->checkState() ==  Qt::Checked  )
		{
			field = FindField( mProduct, smPredefinedVariables[ i ], alias_used, field_error_msg );
			if ( field )
				fields.push_back( field );
		}
	}									 assert__( fields.size() > 0 );

	for ( auto *field : fields )
	{
		std::string field_record = field->GetRecordName();
		//if ( !mOperation->HasFormula() )
		//{
		//	mOperation->SetRecord( field_record );
		//	if ( mOperation->GetRecord().empty() && mProduct->IsNetCdf() )
		//		mOperation->SetRecord( CProductNetCdf::m_virtualRecordName );
		//}
		CFormula* formula = mOperation->NewUserFormula( error_msg, field, CMapTypeField::eTypeOpAsField, true, mProduct );
		if ( !error_msg.empty() )
			SimpleErrorBox( error_msg );
		//Add( theParentId, formula );

        Q_UNUSED( formula)
	}

	assert__( mOperation->IsMap() );

	if ( !Execute( true ) )
		return;

	//openTestFile( t2q( mManager->mPaths.mWorkspacesDir + R"(/newWP/Displays/DisplayDisplays_New.par)" ) );
}


void COperationControls::HandleQuickPlot()
{
	WaitCursor wait;				assert__( mModel );

	NOT_IMPLEMENTED;
	return;

	auto ed = new CPlotEditor( mModel, mOperation, "", mManager->parentWidget() );
    auto subWindow = mManager->AddSubWindow( ed );
    subWindow->show();

    //openTestFile( t2q( mManager->mPaths.mWorkspacesDir + R"(/newWP/Displays/DisplayDisplays_2.par)" ) );
}



//L:\project\workspaces\newWP\Displays\DisplayDisplays_New.par

void COperationControls::openTestFile( const QString &fileName )
{
	delete mCmdLineProcessor;
	mCmdLineProcessor = new CDisplayFilesProcessor;

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
					auto subWindow = mManager->AddSubWindow( ed );
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
					auto subWindow = mManager->AddSubWindow( ed );
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
					auto subWindow = mManager->AddSubWindow( ed );
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
