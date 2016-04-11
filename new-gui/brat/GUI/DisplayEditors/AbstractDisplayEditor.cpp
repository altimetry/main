#include "new-gui/brat/stdafx.h"

#include "new-gui/Common/QtUtils.h"
#include "ApplicationLogger.h"

#include "DataModels/Model.h"
#include "DataModels/Workspaces/Display.h"
#include "DataModels/DisplayFilesProcessor.h"
#include "DataModels/Workspaces/Workspace.h"
#include "DataModels/Workspaces/Display.h"

#include "GUI/ActionsTable.h"
#include "GUI/TabbedDock.h"
#include "GUI/ControlPanels/Views/ViewControlPanels.h"
#include "GUI/DisplayWidgets/TextWidget.h"

#include "AbstractDisplayEditor.h"




inline QToolBar* AddWidgets( QToolBar *toolbar, const std::vector< QObject* > &v )
{
	for ( auto ob : v )
	{
		if ( !ob )
		{
			toolbar->addSeparator();
			continue;
		}

		auto w = qobject_cast<QWidget*>( ob );		assert__( w );
		if ( w )
			toolbar->addWidget( w );
	}
	return toolbar;
}

void CAbstractDisplayEditor::CreateMainToolbar()
{
	QToolBar *toptoolbar = new QToolBar( "View ToolBar", this );
	//toptoolbar->setIconSize( { tool_icon_size, tool_icon_size } );
	toptoolbar->setAllowedAreas( Qt::TopToolBarArea );
	toptoolbar->setMovable( false );
	toptoolbar->layout()->setSpacing( 2 );

	auto loperations = new QLabel( "Operation" );
	QFont font = loperations->font();
	font.setBold( true );
	loperations->setFont( font );
	auto lfilters = new QLabel( "Filter" );
	auto ldataset = new QLabel( "Dataset" );
	mOperationsCombo = new QComboBox;
	mFilterLineEdit = new QLineEdit;		mFilterLineEdit->setReadOnly( true );		mFilterLineEdit->setAlignment( Qt::AlignHCenter );
	mFilterLineEdit->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );

	mDatasetName = new QLineEdit;
	mDatasetName->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );

	mOperationsCombo->setToolTip( "Operation" );
	mFilterLineEdit->setToolTip( "Filter" );
	mDatasetName->setToolTip( "Dataset" );

	mSaveOneClickButton = new QPushButton( "One Click" );
	QPixmap pix( t2q( CActionInfo::IconPath( eAction_One_Click ) ) );
	QIcon icon( pix );
	mSaveOneClickButton->setIcon( icon );
	mSaveOneClickButton->setDefault( false );
	mSaveOneClickButton->setAutoDefault( false );

	addToolBar( AddWidgets( toptoolbar,
	{
		loperations, mOperationsCombo, nullptr, ldataset, mDatasetName, nullptr, lfilters, mFilterLineEdit, nullptr, mSaveOneClickButton
	}
	) );
}


void CAbstractDisplayEditor::CreateWorkingDock()
{
	setDockOptions( dockOptions() & ~AnimatedDocks );	//TODO DELETE AFTER CHECKING WINDOWS BLUE SCREENS

	//dock
	assert__( mOperation );
	mWorkingDock = new CTabbedDock( "Display Properties", this );

	mWorkingDock->setMinimumSize( min_editor_dock_width, min_editor_dock_height );
	auto PreventActions = QDockWidget::DockWidgetClosable;								//QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable | 
	mWorkingDock->setFeatures( mWorkingDock->features() & ~PreventActions );
	mWorkingDock->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );

	addDockWidget( Qt::DockWidgetArea::BottomDockWidgetArea, mWorkingDock );

	CActionInfo::SetDockActionProperties( mWorkingDock, eAction_DisplayEditorDock );	//TODO: Remove this if the action is not added to the DisplayEditor tool-bar
}


void CAbstractDisplayEditor::CreateGraphicsBar()
{
    mGraphicsToolBar = new QToolBar( "View ToolBar", this );
    mGraphicsToolBar->setIconSize({tool_icon_size,tool_icon_size});

	// add button group

	QActionGroup *group = CActionInfo::CreateActionGroup( this, { eAction_DisplayEditor2D, eAction_DisplayEditor3D }, mIsMapEditor );

	m2DAction = group->actions()[0];	assert__( m2DAction->isCheckable() );
	m3DAction = group->actions()[1];	assert__( m3DAction->isCheckable() );
	mRecenterAction = CActionInfo::CreateAction( this, eAction_Re_center );

	mGraphicsToolBar->addActions( group->actions() );
	mGraphicsToolBar->addAction( mRecenterAction );


	// add the bar

	mGraphicsToolBar->setAllowedAreas( Qt::RightToolBarArea );
    addToolBar( Qt::RightToolBarArea, mGraphicsToolBar );
}


void CAbstractDisplayEditor::CreateStatusBar()
{
    mStatusBar = new QStatusBar( this );
    mStatusBar->setObjectName(QString::fromUtf8("mStatusBar"));
    setStatusBar(mStatusBar);
}


template< class GENERAL_TAB >
void CAbstractDisplayEditor::CreateWidgets()
{
	setAttribute( Qt::WA_DeleteOnClose );

	if ( !mDisplayOnlyMode )
	{
		CreateMainToolbar();
	}
	CreateWorkingDock();
	CreateGraphicsBar();
	CreateStatusBar();

	mMainSplitter = CreateSplitterIn( this, Qt::Horizontal );
	mMainSplitter->setChildrenCollapsible( false );
	setCentralWidget( mMainSplitter );

	mTabGeneral = new GENERAL_TAB( this );
	AddTab( mTabGeneral, "General" );


	Wire();
}

// force template instantiation
//
template void CAbstractDisplayEditor::CreateWidgets< CViewControlsPanelGeneralMaps >();
template void CAbstractDisplayEditor::CreateWidgets< CViewControlsPanelGeneralPlots >();



void CAbstractDisplayEditor::Wire()
{
	//graphics tool-bar

	connect( m2DAction, SIGNAL( toggled( bool ) ), this, SLOT( Handle2D( bool ) ) );
	connect( m3DAction, SIGNAL( toggled( bool ) ), this, SLOT( Handle3D( bool ) ) );

	connect( mRecenterAction, SIGNAL( triggered() ), this, SLOT( HandleRecenter() ) );


	//tab general

	connect( mTabGeneral->mDisplaysCombo, SIGNAL( currentIndexChanged(int) ), this, SLOT( HandleViewChanged(int) ) );
	connect( mTabGeneral->mNewDisplayButton, SIGNAL( clicked() ), this, SLOT( HandleNewButtonClicked() ) );
	connect( mTabGeneral->mRenameDisplayButton, SIGNAL( clicked() ), this, SLOT( HandleRenameButtonClicked() ) );
	connect( mTabGeneral->mDeleteDisplayButton, SIGNAL( clicked() ), this, SLOT( HandleDeleteButtonClicked() ) );
    connect( mTabGeneral->mExecuteDisplay, SIGNAL( clicked() ), this, SLOT( RunButtonClicked() ) );

	connect( mTabGeneral->mPlotTitle, SIGNAL( returnPressed() ), this, SLOT( HandlePlotTitleEntered() ) );


	//main tool-bar

	connect( mOperationsCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( HandleOperationsIndexChanged( int ) ) );
	connect( mSaveOneClickButton, SIGNAL( clicked() ), this, SLOT( HandleOneClickClicked() ) );

	mDatasetName->setReadOnly( true );

	UpdateOperationsCombo();
}


CAbstractDisplayEditor::CAbstractDisplayEditor( bool map_editor, CModel *model, const COperation *op, const std::string &display_name )

	: base_t( nullptr )		//will be re-parented by desktop sub-window parent
	, mModel( model )
	, mWDataset( model->Workspace< CWorkspaceDataset >() )
	, mWOperation( model->Workspace< CWorkspaceOperation >() )
	, mWDisplay( model->Workspace< CWorkspaceDisplay >() )
	, mWFormula( model->Workspace< CWorkspaceFormula >() )
	, mOperation( op )
	, mDisplayOnlyMode( false )
	, mIsMapEditor( map_editor )
{
    assert__( mOperation && mWDataset && mWOperation && mWDisplay && mWFormula );       Q_UNUSED( display_name );

	FilterOperations();
	if ( !mOperation )
	{
		throw CException( "The requested operation " + op->GetName() + " has no associated displays. Please run the operation again" );
	}

	//IMPORTANT CreateWidgets() must be called by derived classes
}


// Constructor for brat display one mode
//
CAbstractDisplayEditor::CAbstractDisplayEditor( bool map_editor, const CDisplayFilesProcessor *proc )

	: base_t( nullptr )						//will be re-parented by desktop sub-window parent
	, mCurrentDisplayFilesProcessor( proc )
	, mDisplayOnlyMode( true )
	, mIsMapEditor( map_editor )
{
    assert__( !map_editor || proc->isZFLatLon() );                  Q_UNUSED(map_editor);   //for release builds
	assert__( map_editor || proc->isYFX() || proc->isZFXY() );

	mTabGeneral->mDisplaysCombo->addItem( t2q( mCurrentDisplayFilesProcessor->ParamFile() ) );
	mTabGeneral->mDisplaysCombo->setEnabled( false );
}


//virtual 
CAbstractDisplayEditor::~CAbstractDisplayEditor()
{
	delete mCurrentDisplayFilesProcessor;
}





QSize CAbstractDisplayEditor::sizeHint() const
{
	return QSize( 72 * fontMetrics().width( 'x' ),
		25 * fontMetrics().lineSpacing() );
}


int CAbstractDisplayEditor::DisplayIndex( const CDisplay *display ) const
{
	if ( !display )
		return -1;

	return std::find( mFilteredDisplays.begin(), mFilteredDisplays.end(), display ) - mFilteredDisplays.begin();
}


int CAbstractDisplayEditor::OperationIndex( const COperation *op ) const
{
	if ( !op )
		return -1;

	return std::find( mFilteredOperations.begin(), mFilteredOperations.end(), op ) - mFilteredOperations.begin();
}



QWidget* CAbstractDisplayEditor::AddTab( QWidget *tab, const QString title )		// title = "" 
{
	return mWorkingDock->AddTab( tab, title );
}


void CAbstractDisplayEditor::SelectTab( QWidget *tab )
{
	mWorkingDock->SelectTab( mWorkingDock->TabIndex( tab ) );
}


bool CAbstractDisplayEditor::AddView( QWidget *view, bool view3D )
{
	assert__( mMainSplitter->count() < 2 );

	if ( mMainSplitter->count() == 2 )
		return false;

	if ( view3D )
	{
		m3DAction->setEnabled( true );
		m3DAction->setChecked( true );
		mMainSplitter->insertWidget( 1, view );
	}
	else
	{
		m2DAction->setEnabled( true );
		m2DAction->setChecked( true );
		mMainSplitter->insertWidget( 0, view );
	}

	return true;
}

bool CAbstractDisplayEditor::RemoveView( QWidget *view, bool view3D, bool disable_action )
{
	const int count = mMainSplitter->count();
	if ( count == 0 )
		return false;

	if ( !view->close() )
		return false;
	delete view;			assert__( mMainSplitter->count() == count - 1 );

	if ( view3D )
	{
		m3DAction->blockSignals( true );
		m3DAction->setChecked( false );
		m3DAction->setDisabled( disable_action );
		m3DAction->blockSignals( false );
	}
	else
	{
		m2DAction->blockSignals( true );
		m2DAction->setChecked( false );
		m2DAction->setDisabled( disable_action );
		m2DAction->blockSignals( false );
	}

	return true;
}



QAction* CAbstractDisplayEditor::AddToolBarAction( QObject *parent, EActionTag tag )
{
	QAction *a = CActionInfo::CreateAction( parent, tag );
	mGraphicsToolBar->addAction( a );
	return a;
}


QAction* CAbstractDisplayEditor::AddToolBarSeparator()
{
	return mGraphicsToolBar->addSeparator();
}


QToolButton* CAbstractDisplayEditor::AddMenuButton( EActionTag button_tag, const QList<QAction*> &actions )
{
	QToolButton *b = CActionInfo::CreateMenuButton( button_tag, actions );
	mGraphicsToolBar->addWidget( b );
	return b;
}




/////////////////////////////
//	Handlers
/////////////////////////////


// Assuming Start is called after this
//
void CAbstractDisplayEditor::UpdateOperationsCombo()
{
	mOperationsCombo->blockSignals( true );
	FillCombo( mOperationsCombo, mFilteredOperations,

		-1, true,

		[]( const COperation *po ) -> const char*
		{
			return po->GetName().c_str();
		}
	);
	mOperationsCombo->blockSignals( false );
}


void CAbstractDisplayEditor::UpdateDisplaysCombo( int index )
{
	FillCombo( mTabGeneral->mDisplaysCombo, mFilteredDisplays,

		index, true,

		[]( const CDisplay *disp ) -> const char*
		{
			return disp->GetName().c_str();
		}
	);
}


void CAbstractDisplayEditor::HandleOperationsIndexChanged( int index )
{
	if ( index < 0 )
	{
		mDatasetName->clear();
		mFilterLineEdit->clear();
	}
	else
	{
		assert__( mWOperation );
		const COperation *op = mWOperation->GetOperation( q2a( mOperationsCombo->currentText() ) );		
		if ( op == mOperation )
			LOG_TRACE( "Operation already assigned before handling index change." );

		mOperation = op;
		std::string dataset_name = mOperation->OriginalDatasetName();
		mDatasetName->setText( t2q( dataset_name ) );
		mFilterLineEdit->setText( mOperation->Filter() ? t2q( mOperation->Filter()->Name() ) : "" );
	}
	
	FilterDisplays();
	UpdateDisplaysCombo( mRequestedDisplayIndex );
	mRequestedDisplayIndex = 0;
	OperationChanged( index );
}


bool CAbstractDisplayEditor::Start( const std::string &display_name )
{
	FilterDisplays();
	mRequestedDisplayIndex = 0;
	if ( !display_name.empty() )
	{
		CDisplay *display = mWDisplay->GetDisplay( display_name );		assert__( display );
		mRequestedDisplayIndex = DisplayIndex( display );		  		assert__( mRequestedDisplayIndex >= 0 );
	}
	int index = OperationIndex( mOperation );

	if ( mOperationsCombo->currentIndex() == index )
		HandleOperationsIndexChanged( index );
	else
		mOperationsCombo->setCurrentIndex( index );

	return index >= 0;
}


void CAbstractDisplayEditor::RunButtonClicked()
{
	int index = mTabGeneral->mDisplaysCombo->currentIndex();
	if ( index >= 0 )
		HandleViewChanged( index );
}


void CAbstractDisplayEditor::HandlePlotTitleEntered()
{
	if ( !mDisplay )
		return;

	mDisplay->SetTitle( q2a( mTabGeneral->mPlotTitle->text() ) );
	SetWindowTitle();
}


void CAbstractDisplayEditor::SetWindowTitle()
{
	assert__( mDisplay );

	QWidget *parent = parentWidget();
	if ( !parent )						//editor is being created
		parent = this;					//when re-parented by the desktop sub-window, this one uses the child title

	if ( !mDisplay->GetTitle().empty() )
		parent->setWindowTitle( ( mDisplay->GetTitle() + " [" + mOperation->GetName() + "]" ).c_str() );
	else
		parent->setWindowTitle( ( mDisplay->GetName() + " [" + mOperation->GetName() + "]"  ).c_str() );
}


void CAbstractDisplayEditor::HandleViewChanged( int index )
{
	CDisplay *display = index < 0 ? nullptr : mFilteredDisplays[ index ];
	delete mCurrentDisplayFilesProcessor;
	mCurrentDisplayFilesProcessor = nullptr;
	
	//if ( display == mDisplay )
	//	return;					the display is the same but the properties could have changed

	mDisplay = display;

	if ( !mDisplay )
		return;

	assert__( mDisplay );

	SetWindowTitle();
	mTabGeneral->mPlotTitle->setText( mDisplay->GetTitle().c_str() );
	CMapTypeDisp::ETypeDisp type = mDisplay->GetType();
	mTabGeneral->mPlotType->setText( CMapTypeDisp::GetInstance().IdToName( type ).c_str() );

	std::string msg;
	if ( !ControlSolidColor() || !ControlVectorComponents( msg ) )	//== CDisplayPanel::Control(std::string& msg)
	{
		SimpleErrorBox( msg );

		//TODO de-select display in widget list 

		index = -1;		//TODO de-select display in widget list ???

		return;
	}
	else
	{
		if ( !RefreshDisplayData() )
		{
			return;
		}

		if ( !mDisplay->BuildCmdFile( msg, !IsMapEditor() ) )
		{
			assert__( !msg.empty() );

			SimpleErrorBox( msg );

			index = -1;		//TODO de-select display in widget list ???

			return;
		}
	}

	ViewChanged();
}


void CAbstractDisplayEditor::Handle2D( bool checked )
{
	if ( checked || m3DAction->isChecked() )
		Show2D( checked );
	else
		m2DAction->setChecked( true );
}


void CAbstractDisplayEditor::Handle3D( bool checked )
{
	if ( checked || m2DAction->isChecked() )
		Show3D( checked );
	else
		m3DAction->setChecked( true );
}


void CAbstractDisplayEditor::HandleRecenter()
{
	Recenter();
}



/////////////////////////////
//	Display Objects Edition
/////////////////////////////


void CAbstractDisplayEditor::HandleNewButtonClicked()
{
	assert__( mOperation && mWDisplay );
	
    /*int current_display = */mTabGeneral->mDisplaysCombo->currentIndex();

	//FillProjList();
	//FillPaletteList();
	//GetDispNames()->Enable( true );

	CDisplay *new_display = nullptr;

	std::string display_name = mWDisplay->GetDisplayNewName();
	if ( !mWDisplay->InsertDisplay( display_name ) )
	{
		SimpleErrorBox( "Display '" + display_name + "' already exists" );		//TODO After call to GetDisplayNewName(), this is stupid and should be an assert
		return;
	}
	else
	{
		//new_display = GetDispNames()->Append( display_name );
		//GetDispNames()->SetSelection( m_currentDisplay );
	}

	//SetCurrentDisplay();		//assigns mDisplay using m_currentDisplay

	//InitDisplayOutput();		//makes mDisplay->InitOutput( wksd );
	//GetDisplayOutput();			//oddly, makes InitOutput( wksd ); in case output is empty

	new_display = mWDisplay->GetDisplay( display_name );
	new_display->InitOutput( mWDisplay );

	if ( !new_display->AssignOperation( mOperation ) )
	////display panel -> GetOperations();
	////display panel -> GetDispavailtreectrl()->InsertData( &m_dataList );
	//CMapDisplayData *m_dataList = new CMapDisplayData;
	//m_dataList->clear();
	//if ( !CreateDisplayData( mCurrentOperation, *m_dataList ) )	//CDisplay::GetDisplayType failed
	{
		SimpleErrorBox( "Could not retrieve operation output." );
		mWDisplay->DeleteDisplay( new_display );
		return;	
	}

	//GetDispTitle()->Clear();

	//ClearSelectedDataList();

	//ShowGeneralProperties();
	//ShowFieldProperties();

	//LoadAvailableData();		//makes GetOperations();

	//GetDispDataSel()->SetDisplay( mDisplay );

	//EnableCtrl();

	Start( display_name );

	NewButtonClicked();		//for derived classes notification
}


void CAbstractDisplayEditor::HandleRenameButtonClicked()
{
    assert__( mDisplay );

    auto result = SimpleInputString( "View Name", mDisplay->GetName(), "Rename View" );
	if ( !result.first )
		return;

    if ( !mWDisplay->RenameDisplay( mDisplay, result.second.c_str() ) )
    {
        SimpleMsgBox( "Unable to rename view '"
                        + mDisplay->GetName()
                        + "' by '"
                        + result.second
                        + "'\nPerhaps view already exists, " );
    }
    else
    {
        mTabGeneral->mDisplaysCombo->setItemText( mTabGeneral->mDisplaysCombo->currentIndex(), mDisplay->GetName().c_str() );

        std::string old_output = mDisplay->GetOutputFilename();
        const bool output_exists = IsFile( old_output );
        mDisplay->InitOutput( mWDisplay );
        if ( output_exists )
        {
			RemoveFile( old_output );
        }

		//RunButtonClicked(); why???

        //TODO a big one: emit DisplayModified( mDisplay );

		RenameButtonClicked();	//for derived classes notification
    }
}


void CAbstractDisplayEditor::HandleDeleteButtonClicked()
{
	assert__( mOperation && mDisplay && mWDisplay );

    if ( !SimpleQuestion( "Are you sure to delete view '" + mDisplay->GetName() + "' ?" ) )
         return;

	std::string display_name = mDisplay->GetName();
	if ( !mWDisplay->DeleteDisplay( mDisplay ) )
	{
		SimpleErrorBox( "Unable to delete display '" + mDisplay->GetName() + "'" );
		return;
	}

	//check if there are any displays left for the selected operation

	const COperation *to_select = nullptr;
	std::string new_display_name;
	auto displays = FilterDisplays( mOperation );
	if ( displays.size() > 0 )
	{
		to_select = mOperation;
		new_display_name = displays[ 0 ]->GetName();
	}

	//... if there are no more displays, look for another operation with valid displays

	if ( to_select == nullptr )
	{
		mFilteredOperations.erase( std::find( mFilteredOperations.begin(), mFilteredOperations.end(), mOperation ) );
		mOperation = nullptr;
		UpdateOperationsCombo();
		for ( auto *op : mFilteredOperations )
		{
			displays = FilterDisplays( op );
			if ( displays.size() > 0 )
			{
				to_select = op;
				new_display_name = displays[ 0 ]->GetName();
				break;
			}
		}
	}

	if ( to_select == nullptr )
	{		
		SimpleMsgBox( "There are no suitable views to display in this editor. Closing..." );		assert__( new_display_name.empty() );
		QTimer::singleShot( 1000, parentWidget(), SLOT( close() ) );
		return;
	}

	mOperation = to_select;

	Start( new_display_name );


	//mDisplay = NULL;
	//m_currentData = NULL;

	//GetDispNames()->Delete( m_currentDisplay );

	//m_currentDisplay = ( GetDispNames()->GetCount() > 0 ) ? 0 : -1;

	//GetDispNames()->SetSelection( m_currentDisplay );

	//SetCurrentDisplay();

	//LoadAvailableData();
	//LoadSelectedData();

	//ShowGeneralProperties();
	//ShowFieldProperties();

	//GetDisplayOutput();


	//EnableCtrl();


	DeleteButtonClicked();	//for derived classes notification
}







/////////////////////////////
//	Domain Processing Helpers
/////////////////////////////

// TODO This seems to make sense only in old views tab; the question remains: how to 
//	keep functionality there
// 
bool CAbstractDisplayEditor::RefreshDisplayData()			//CDisplayPanel::RefreshSelectedData()
{
	assert__( mDisplay );

	//CMapDisplayData* availableData = GetDispavailtreectrl()->GetMapDisplayData();
	//CMapDisplayData* selectedData = mDisplay->GetData();

	//CObArray dispSelNotFound, dispSelToRefresh;

	//int oldDisplayType = mDisplay->GetType();
	//int newDisplayType = -1;

	//CUIntArray displayTypes;

	//const CDisplayData* firstNewSelData = nullptr;

	//for ( CMapDisplayData::const_iterator itSel = selectedData->begin(); itSel != selectedData->end(); itSel ++ )
	//{
	//	CDisplayData* dataSel = selectedData->GetDisplayData( itSel );
	//	if ( dataSel == nullptr )
	//	{
	//		continue;
	//	}

	//	const CDisplayData* dataAvailable = nullptr;

	//	if ( firstNewSelData == nullptr )
	//	{
	//		dataAvailable = availableData->GetDisplayData( dataSel->GetDataKey() );

	//		if ( dataAvailable == nullptr )
	//		{
	//			dataSel->GetAvailableDisplayTypes( displayTypes );

	//			for ( CUIntArray::iterator itDispType = displayTypes.begin(); itDispType != displayTypes.end(); itDispType++ )
	//			{
	//				dataAvailable = availableData->GetDisplayData( dataSel->GetDataKey( *itDispType ) );
	//				if ( dataAvailable != nullptr )
	//				{
	//					break;
	//				}
	//			}
	//		}
	//	}
	//	else
	//	{
	//		dataAvailable = availableData->GetDisplayData( dataSel->GetDataKey( firstNewSelData->GetType() ) );
	//	}

	//	if ( dataAvailable == nullptr )
	//	{
	//		dispSelNotFound.Insert( new CDisplayData( *dataSel, mWOperation ) );
	//		continue;
	//	}

	//	if ( firstNewSelData == nullptr )
	//	{
	//		firstNewSelData = dataAvailable;
	//		newDisplayType = dataAvailable->GetType();
	//	}

	//	CDisplayData* newDisplayData = new CDisplayData( *dataAvailable, mWOperation );
	//	newDisplayData->CopyFieldUserProperties( *dataSel );

	//	dispSelToRefresh.Insert( newDisplayData );
	//}

	//GetDispDataSel()->DeselectAll();
	//m_currentData = nullptr;

	//std::string msg = "Warnings about view '" + mDisplay->GetName() + "':\n\n";

	bool bOk = true;
	//for ( CObArray::const_iterator it = dispSelNotFound.begin(); it != dispSelNotFound.end(); it ++ )
	//{
	//	CDisplayData* data = dynamic_cast<CDisplayData*>( *it );

	//	if ( newDisplayType >= 0 )
	//	{
	//		data->SetType( newDisplayType );
	//	}
	//	else
	//	{
	//		data->SetType( oldDisplayType );
	//	}

	//	bOk = false;

	//	msg += "In operation '" + data->GetOperation()->GetName() + "', field's name '" + data->GetField()->GetName() + "' doesn't exist anymore.\n";
	//}


	//selectedData->RemoveAll();
	//GetDispDataSel()->InsertDisplay( nullptr );
	//GetDispDataSel()->SetDisplay( mDisplay );

	//if ( newDisplayType >= 0 )
	//{
	//	mDisplay->SetType( newDisplayType );
	//}

	//bOk &= AddField( dispSelToRefresh, true );
	//bOk &= AddField( dispSelNotFound, true );

	////GetDispDataSel()->InsertDisplay(mDisplay);

	//if ( dispSelToRefresh.size() <= 0 )
	//{
	//	msg += "\nAll the expressions to display don't exist anymore.\n";

	//}

	//if ( newDisplayType > 0 && ( oldDisplayType != newDisplayType ) )
	//{
	//	bOk = false;
	//	msg +=  "\nView type has changed from '"
	//		+ CMapTypeDisp::GetInstance().IdToName( oldDisplayType )
	//		+ "' to '"
	//		+ CMapTypeDisp::GetInstance().IdToName( newDisplayType )
	//		+ "'\n\n";
	//}


	//ShowFieldProperties();

	//if ( !bOk )
	//{
	//	SimpleErrorBox( msg );
	//}

	return bOk;
}


void CAbstractDisplayEditor::FilterDisplays()
{
	assert__( mOperation );

	mFilteredDisplays = FilterDisplays( mOperation );
}


std::vector< CDisplay* >  CAbstractDisplayEditor::FilterDisplays( const COperation *operation )
{
	std::vector< CDisplay* > filtered_displays;

	auto displays = mModel->OperationDisplays( operation->GetName() );
	std::string errors;
	for ( auto display : displays )
	{
		try
		{
			if ( !mIsMapEditor || display->IsZLatLonType() )
			{
				auto v = display->GetOperations();
				if ( v.size() == 0 )
					LOG_WARN( "View " + display->GetName() + " does not reference any operation." );
				else
					filtered_displays.push_back( mWDisplay->GetDisplay( display->GetName() ) );
			}
		}
		catch ( const CException &e )
		{
			errors += ( e.Message() + "\n" );
		}
	}

	if ( !errors.empty() )
		SimpleWarnBox( errors );

	return filtered_displays;
}

//  - called by constructor and delete display
//	- can cancel mOperation assignment if it does not have associated views
//	- 
//
void CAbstractDisplayEditor::FilterOperations()
{
	mFilteredOperations.clear();

	auto const &operations = *mWOperation->GetOperations();
	std::string errors;
	for ( auto const &operation_entry : operations )
	{
		const COperation *operation = dynamic_cast<COperation *>( operation_entry.second );		assert__( operation );
		try
		{
			if ( !mIsMapEditor || operation->IsMap() )
			{
				auto v = mModel->OperationDisplays( operation->GetName() );
				if ( v.size() == 0 )
				{
					LOG_WARN( "Operation " + operation->GetName() + " is not referenced by any view. Try to run the operation again." );
					if ( mOperation == operation )
						mOperation = nullptr;
				}
				else
					mFilteredOperations.push_back( operation );
			}
		}
		catch ( const CException &e )
		{
			errors += ( e.Message() + "\n" );
		}
	}

	if ( !errors.empty() )
		SimpleWarnBox( errors );
}



bool CAbstractDisplayEditor::ControlSolidColor()
{
	assert__( mDisplay != nullptr );

	CMapDisplayData* selectedData =  mDisplay->GetData();

	for ( CMapDisplayData::const_iterator itSel = selectedData->begin(); itSel != selectedData->end(); itSel ++ )
	{
		CDisplayData* dataSel = selectedData->GetDisplayData( itSel );
		if ( dataSel == nullptr )
			continue;

		if ( !dataSel->IsSolidColor() && !dataSel->IsContour() )
			dataSel->SetSolidColor( true );
	}
	return true;
}


bool CAbstractDisplayEditor::ControlVectorComponents( std::string& msg )
{
	assert__( mDisplay != nullptr );

	bool xcomponent = false;
	bool ycomponent = false;

	CMapDisplayData* selectedData =  mDisplay->GetData();

	for ( CMapDisplayData::const_iterator itSel = selectedData->begin(); itSel != selectedData->end(); itSel ++ )
	{
		CDisplayData* dataSel = selectedData->GetDisplayData( itSel );
		if ( dataSel == nullptr )
			continue;

		if ( dataSel->IsEastComponent() ) 
		{
			if ( xcomponent ) 
			{
				msg = "More than one field selected as East Component!";
				return false;
			}
			xcomponent = true;
		}

		if ( dataSel->IsNorthComponent() ) 
		{
			if ( ycomponent ) 
			{
				msg = "More than one field selected as North Component!";
				return false;
			}
			ycomponent = true;
		}
	}

	if ( xcomponent == ycomponent )
		return true;

	msg = "Both East and North fields must be selected for a Vector Plot!";

	return false;
}





///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_AbstractDisplayEditor.cpp"
