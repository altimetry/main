#include "stdafx.h"

#include <osg/Version>

#include "new-gui/Common/BratVersion.h"
#include "new-gui/Common/ccore-types.h"
#include "new-gui/Common/QtUtils.h"
#include "new-gui/Common/ConfigurationKeywords.h"

#include "new-gui/brat/Workspaces/Workspace.h"

#include "new-gui/brat/Views/TextWidget.h"

#include "new-gui/brat/GUI/TabbedDock.h"
#include "new-gui/brat/GUI/ControlsPanel.h"
#include "new-gui/brat/GUI/ActionsTable.h"
#include "new-gui/brat/GUI/WorkspaceDialog.h"
#include "new-gui/brat/GUI/WorkspaceElementsDialog.h"

#include "BratApplication.h"
#include "ApplicationSettingsDlg.h"
#include "ApplicationLogger.h"

#include "BratMainWindow.h"



#if defined(BRAT_ARCHITECTURE_64)

    static const QString build_platform( " 64-bit" );

#elif defined(BRAT_ARCHITECTURE_32)

	static const QString build_platform( " 32-bit" );
#else
#error One of BRAT_ARCHITECTURE_32 or BRAT_ARCHITECTURE_64 must be defined
#endif


#if defined(_UNICODE) || defined(UNICODE)
    static const QString build_charset( " Un" );
#else
	static const QString build_charset( "" );
#endif


#if defined (DEBUG) || defined(_DEBUG)
    static const QString build_configuration = build_platform + " [DEBUG] " + build_charset;
#else
	static const QString build_configuration = build_platform + build_charset;
#endif


auto const PROCESSOR_ARCH =

#if defined(BRAT_ARCHITECTURE_64)
	"64 bit";
#else
	"32 bit";
#endif



////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                              Statics
////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//static 
CBratMainWindow *CBratMainWindow::smInstance = nullptr;



//    "The window title must contain a "[*]" placeholder, which
//    indicates where the '*' should appear. Normally, it should appear
//    right after the file name (e.g., "document1.txt[*] - Text
//    TextEditor")." Qt doc.
//
//static
QString CBratMainWindow::makeWindowTitle( const QString &title )// = QString() //returns the application name if title is empty
{
    static const QString &app_title = qApp->applicationName() + build_configuration;

    QString t = app_title;

    if ( !title.isEmpty() )
        t += " - " + title;

    return t + "[*]";
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//											Construction Helpers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool CBratMainWindow::ReadSettings( bool &has_gui_settings, bool &wasMaximized, QByteArray &geom, QByteArray &state )
{
	CAppSection section( GROUP_MAIN_WINDOW );
	has_gui_settings = !section.Keys().empty();

	if ( has_gui_settings )
	{
		if ( !AppSettingsReadSection( GROUP_MAIN_WINDOW,

			k_v( KEY_MAIN_WINDOW_GEOMETRY, &geom ),
			k_v( KEY_MAIN_WINDOW_STATE, &state ),
			k_v( KEY_MAIN_WINDOW_MAXIMIZED, &wasMaximized )
			) )

			return false;
	}

	return mSettings.Status() == QSettings::NoError;
}


bool CBratMainWindow::WriteSettings()
{
	assert__( !destroyed() );

    mSettings.m_lastPageReached = n2s< std::string >( mMainWorkingDock->SelectedTab() );
    mSettings.mAdvancedOperations = WorkingPanel< eOperations >()->AdvancedMode();
    const CWorkspace *wks = mModel.RootWorkspace();
    mSettings.m_lastWksPath = wks != nullptr ? wks->GetPath() : "";

	if (
		//! mSettings.SaveConfig(  )		TODO this is in charge of the application class but something is wrong here: the window assigns the parameters that the application saves???
		//||
		! mRecentFilesProcessor->WriteSettings() 
		||
		! AppSettingsWriteSection( GROUP_MAIN_WINDOW,
		
			k_v( KEY_MAIN_WINDOW_GEOMETRY, saveGeometry() ),
			k_v( KEY_MAIN_WINDOW_STATE, saveState() ),
			k_v( KEY_MAIN_WINDOW_MAXIMIZED, isMaximized() )
		) 
		)
		std::cout << "Unable to save BRAT the main window settings." << std::endl;	// TODO log this

		mSettings.Sync();

	return mSettings.Status() == QSettings::NoError;
}


CControlsPanel* CBratMainWindow::MakeWorkingPanel( ETabName tab )
{
	switch ( tab )
	{
		case eDataset:
			return new ControlsPanelType< eDataset >::type( mManager );
			break;
		case eFilter:
			return new ControlsPanelType< eFilter >::type( mManager );
			break;
		case eOperations:
			return new ControlsPanelType< eOperations >::type( mManager );
			break;
		default:
			assert__( false );
	}

	return nullptr;
}


void CBratMainWindow::CreateWorkingDock()
{
	LOG_TRACE( "Starting working dock (controls panel) construction..." );

	mMainWorkingDock = new CTabbedDock( "Main Working Dock", this );
	mMainWorkingDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );	//mMainWorkingDock->setMaximumWidth( max_main_dock_width );
	addDockWidget( Qt::LeftDockWidgetArea, mMainWorkingDock, Qt::Vertical );

	auto 
	tab = mMainWorkingDock->AddTab( MakeWorkingPanel( eDataset ), "Dataset" );			assert( mMainWorkingDock->TabIndex( tab ) == eDataset );
	mMainWorkingDock->SetTabToolTip( tab, "Dataset browser" );
	connect( this, SIGNAL( WorkspaceChanged( CWorkspaceDataset* ) ), WorkingPanel< eDataset >(), SLOT( WorkspaceChanged( CWorkspaceDataset* ) ) );

	tab = mMainWorkingDock->AddTab( MakeWorkingPanel( eFilter ), "Filter" );	 		assert( mMainWorkingDock->TabIndex( tab ) == eFilter );
	mMainWorkingDock->SetTabToolTip( tab, "Dataset filter" );
	connect( this, SIGNAL( WorkspaceChanged( CWorkspaceDataset* ) ), WorkingPanel< eFilter >(), SLOT( WorkspaceChanged( CWorkspaceDataset* ) ) );

	tab = mMainWorkingDock->AddTab( MakeWorkingPanel( eOperations ), "Operations" );	assert( mMainWorkingDock->TabIndex( tab ) == eOperations );
	mMainWorkingDock->SetTabToolTip( tab, "Quick or advanced operations"  );
	connect( this, SIGNAL( WorkspaceChanged( CWorkspaceOperation* ) ), WorkingPanel< eOperations >(), SLOT( WorkspaceChanged( CWorkspaceOperation* ) ) );
	connect( this, SIGNAL( WorkspaceChanged( CWorkspaceDisplay* ) ), WorkingPanel< eOperations >(), SLOT( WorkspaceChanged( CWorkspaceDisplay* ) ) );

	LOG_TRACE( "Finished working dock construction." );
}


void CBratMainWindow::CreateOutputDock()
{
	static bool initialized = false;				assert__(!initialized);

	LOG_TRACE( "Starting output dock construction..." );

	initialized = true;

	mOutputDock = new CTabbedDock( "Output", this );
	mOutputDock->setAllowedAreas( Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea );
	addDockWidget( Qt::BottomDockWidgetArea, mOutputDock );

	mLogFrame = new CLogShell( this );
	mLogFrame->setObjectName( QString::fromUtf8( "mLogShell" ) );

	mOutputDock->AddTab( mLogFrame, "Log" );

	// wire to logger

	connect( 
		&CApplicationLogger::Instance(), SIGNAL( OsgLogLevelChanged( int ) ), 
		mLogFrame, SLOT( SetOsgLogLevel( int ) ), Qt::QueuedConnection );

	connect( 
		&CApplicationLogger::Instance(), SIGNAL( OsgLogMessage( int, QString ) ), 
		mLogFrame, SLOT( OsgNotifySlot( int, QString ) ), Qt::QueuedConnection );

	connect( 
		&CApplicationLogger::Instance(), SIGNAL( QgisLogMessage( int, QString  ) ),
		mLogFrame, SLOT( QtNotifySlot( int, QString ) ), Qt::QueuedConnection );

	//mLogFrame->SetOsgLogLevel( ns );	//also updates the application logger
	mLogFrame->Deactivate( false );

	LOG_INFO( "OSG initialized in " + qApp->applicationName().toStdString() );
	LOG_INFO( "OSG version: " + std::string( osgGetVersion() ) + " - " + osgGetSOVersion() + " - " + osgGetLibraryName() );
	if ( getenv("OSG_FILE_PATH") )
		LOG_INFO( "OSG_FILE_PATH = " + std::string( getenv("OSG_FILE_PATH") ) );
	else
		LOG_INFO( "OSG_FILE_PATH not defined" );

	LOG_INFO( "Initial OSG notification level: " + n2s<std::string>( CApplicationLogger::Instance().OsgNotifyLevel() ) );

	LOG_TRACE( "Finished output dock construction." );
}


void CBratMainWindow::CreateDocks()
{
	assert__( mManager );

    // Docks (see also the Docks editors section in this ctor)
    //
    setDockOptions( dockOptions() | VerticalTabs );     //not set in designer

    setCorner( Qt::TopLeftCorner, Qt::LeftDockWidgetArea );
    setCorner( Qt::TopRightCorner, Qt::RightDockWidgetArea );
    setCorner( Qt::BottomRightCorner, Qt::RightDockWidgetArea );
    setCorner( Qt::BottomLeftCorner, Qt::LeftDockWidgetArea );		//Qt::BottomDockWidgetArea

	CreateWorkingDock();
	CreateOutputDock();

	if ( mManager->MapDock() )
		addDockWidget( Qt::LeftDockWidgetArea, mManager->MapDock() );
}


void CBratMainWindow::ProcessMenu()
{
	LOG_TRACE( "Starting main menu processing..." );

    // Fill Actions Table
    //
	const QList< QMenu* > lst = mMainMenuBar->findChildren< QMenu* >();
	foreach( QMenu *m, lst )
	{
		qDebug() << m->title();
		foreach( QAction *a, m->actions() )
		{
            qDebug() << a->text();
            auto tag = CActionInfo::UpdateActionProperties( a );		assert__( tag < EActionTag::EActionTags_size );
            qDebug() << tag;
		}
	}

    // Menu File
    //
	connect( action_Close_All, SIGNAL(triggered()), mManager, SLOT(closeAllSubWindows()) );

    // Menu File / Most recent files logic
    //
	mRecentFilesSeparatorAction = menu_File->insertSeparator( action_Exit );

	mRecentFilesProcessor = new CRecentFilesProcessor( this, "Recent WorkSpaces", menu_File, action_Exit, GROUP_WKS_RECENT.c_str() );
	connect( mRecentFilesProcessor, SIGNAL( triggered( QAction* ) ), this, SLOT( openRecentWorkspace_triggered( QAction* ) ) );

	mRecentFilesSeparatorAction = menu_File->insertSeparator( action_Exit );

	if ( !mRecentFilesProcessor->ReadSettings() )
	{
		CException e( "Error reading the configuration file", BRATHL_ERROR );		// TODO which file
		throw e;
	}

    // Menu View / ToolBars
    //
	if ( !menu_View->isEmpty() )
		menu_View->addSeparator();
    menu_View->addAction( mMainToolBar->toggleViewAction() );
    menu_View->addAction( mMainToolsToolBar->toggleViewAction() );

    // Menu View / Docks editors (see also the tool-bars section in this ctor)
    //
    QDockWidget *docks[] = { mOutputDock, mMainWorkingDock };
    menu_View->addSeparator();
    for ( auto dock : docks )
    {
        menu_View->addAction( dock->toggleViewAction() );
    }

    // Global Wiring
    //
    connect( menu_Window, SIGNAL(aboutToShow()), this, SLOT(UpdateWindowMenu()) );
	connect( this, SIGNAL( WorkspaceChanged( const CModel* ) ), this, SLOT( UpdateWorkspaceUI( const CModel* ) ) );

#ifndef QT_NO_CLIPBOARD
    connect( QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(clipboardDataChanged()) );
#endif

#if !defined (DEBUG) && !defined(_DEBUG)
	action_Test->setVisible( false );
#endif

	LOG_TRACE( "Finished main menu processing." );
}


void CBratMainWindow::FillStatusBar()
{
	LOG_TRACE( "Starting status-bar construction..." );

	//remove borders from children under Windows
	//statusBar()->setStyleSheet( "QStatusBar::item {border: none;}" );

	// Add a panel to the status bar for the scale, coords and progress
	// And also rendering suppression checkbox
	mProgressBar = new QProgressBar( statusBar() );
	mProgressBar->setObjectName( "mProgressBar" );
	mProgressBar->setMaximumWidth( 100 );
	//mProgressBar->hide();
	mProgressBar->setWhatsThis( tr( "Progress bar that displays the status of time-intensive operations" ) );
	statusBar()->addPermanentWidget( mProgressBar, 1 );

	connect( mManager->Map(), SIGNAL( renderStarting() ), this, SLOT( CanvasRefreshStarted() ) );
	connect( mManager->Map(), SIGNAL( mapCanvasRefreshed() ), this, SLOT( CanvasRefreshFinished() ) );

	FillStatusBarExperimental();

	mMessageButton = new QToolButton( statusBar() );
	mMessageButton->setAutoRaise( true );
	mMessageButton->setIcon( CBratApplication::getThemeIcon( "bubble.svg" ) );
	mMessageButton->setToolTip( tr( "Messages" ) );
	mMessageButton->setWhatsThis( tr( "Messages" ) );
	mMessageButton->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
	mMessageButton->setObjectName( "mMessageButton" );
	//mMessageButton->setMaximumHeight( mScaleLabel->height() );
	mMessageButton->setCheckable( true );
	statusBar()->addPermanentWidget( mMessageButton, 0 );

	connect( mMessageButton, SIGNAL( toggled( bool ) ), mOutputDock, SLOT( setVisible( bool ) ) );
	connect( mOutputDock, SIGNAL( visibilityChanged( bool ) ), mMessageButton, SLOT( setChecked( bool ) ) );

	LOG_TRACE( "Finished status-bar construction." );
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//											Constructor
////////////////////////////////////////////////////////////////////////////////////////////////////////////////


CBratMainWindow::CBratMainWindow( CBratApplication &a ) 

	: QMainWindow( nullptr)

	, mSettings( a.Settings() )

	, mModel( CModel::CreateInstance( mSettings.BratPaths() ) )

{
	LOG_TRACE( "Starting main window construction..." );

	assert__( !smInstance );
    assert__( mSettings.BratPaths().IsValid()==true );

	connect( this, SIGNAL( SettingsUpdated() ), &a, SLOT( UpdateSettings() ) );

    CMapWidget::SetQGISDirectories(
                mSettings.BratPaths().mQgisPluginsDir,
                mSettings.BratPaths().mVectorLayerPath,
                mSettings.BratPaths().RasterLayerPath(),
                mSettings.BratPaths().mGlobeDir );

    setupUi( this );
    setWindowIcon( QIcon("://images/BratIcon.png") );
    setWindowTitle( makeWindowTitle( windowTitle() ) );     //set title sensitive to doc (un)modifications
    mMainToolBar->setIconSize({icon_size,icon_size});
    mMainToolsToolBar->setIconSize({icon_size,icon_size});

    // Settings from persistence
    //
	bool lastSessionGeometry = false;
	bool wasMaximized = false;
	QByteArray geom, state;
	try {
		ReadSettings( lastSessionGeometry, wasMaximized, geom, state );	//TODO use boolean return value, even if only to throw
	}
    catch ( const CException &e )			// TODO rather stupid catching the exception here...
	{
		SimpleWarnBox( std::string( "An error occurred while loading Brat configuration (CBratGui::LoadConfig)\nNative std::exception: " )
			+ e.what() );
	}


    // Sub-Windows management
    //
	if ( mSettings.mDesktopManagerSdi )
		mManager = new CDesktopManagerSDI( this );
	else
		mManager = new CDesktopManagerMDI( this );


    // Docked controls
    //
	CreateDocks();


    // Menu commands
    //
	ProcessMenu();


    // Status Bar
    //
	FillStatusBar();


    // Apply remaining stored settings
    //
	if ( lastSessionGeometry )
	{
		restoreGeometry( geom );
		restoreState( state );
		if ( wasMaximized )
			setWindowState( windowState() | Qt::WindowMaximized );
	}
    // Position
    //
	else	//if ( !lastSessionGeometry )
	{
        show();
        QApplication::processEvents();
        CenterOnScreen( this );
        //CenterOnScreen2( this );
        //CenterOnScreen3( this );
    }


    // Assign singleton just before triggering load operations and leave
    //
	smInstance = this;


    // Load command line or user settings referenced files 
    //
    QTimer::singleShot( 0, this, SLOT( LoadCmdLineFiles() ) );

	LOG_TRACE( "Finished main window construction." );
}





////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//											Access
////////////////////////////////////////////////////////////////////////////////////////////////////////////////


template< CBratMainWindow::ETabName INDEX >
CBratMainWindow::TabType< INDEX >* CBratMainWindow::WorkingPanel()
{
    return dynamic_cast< TabType< INDEX >* >( mMainWorkingDock->TabWidget( INDEX ) );
}





void CBratMainWindow::LoadCmdLineFiles()
{
    auto compute_last_page_reached = [this]() -> ETabName
    {
        //  from BratGui.h
        //
        static const std::string DATASETS_PAGE_NAME = "Datasets";
        static const std::string OPERATIONS_PAGE_NAME = "Operations";
        static const std::string DISPLAY_PAGE_NAME = "Views";
        static const std::string LOG_PAGE_NAME = "Logs";


        // 1st try old values

        if ( mSettings.m_lastPageReached == DATASETS_PAGE_NAME || mSettings.m_lastPageReached == LOG_PAGE_NAME )
            return eDataset;

        if ( mSettings.m_lastPageReached == OPERATIONS_PAGE_NAME || mSettings.m_lastPageReached == DISPLAY_PAGE_NAME )
            return eOperations;


        // 2nd try v4 values

        ETabName tab = eDataset;

        try
        {
            tab = static_cast< ETabName >( s2n< int >( mSettings.m_lastPageReached ) );
        }
        catch( const std::invalid_argument &e )
        { Q_UNUSED(e); }

        return tab;
    };

	// TODO decide here if (when to) process the command line or the 
	//	last saved workspace

	//////////////////////////////////////
	//	if ( Load Last Workspace ) then :
	//////////////////////////////////////

	if ( !mSettings.mLoadLastWorkspaceAtStartUp )
		DoEmptyWorkspace();
	else
	{
		try {
			if ( !OpenWorkspace( mSettings.m_lastWksPath.c_str() ) )
			{
				SimpleWarnBox( "Unable to load last used workspace located in '" + mSettings.m_lastWksPath + "' " );
			}
			else
			if ( !mSettings.m_lastWksPath.empty() )
			{
				//GotoLastPageReached();

				mMainWorkingDock->SelectTab( compute_last_page_reached() );
				WorkingPanel<eOperations>()->SetAdvancedMode( mSettings.mAdvancedOperations );
			}
		}
		catch ( const CException &e )
		{
			SimpleWarnBox( e.what() );
		}
	}

    return;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    QStringList args = QCoreApplication::arguments();
    args.removeFirst();
    if ( !args.empty() && args[0] == "m" )
        args.removeFirst();
	//femmTODO: uncomment or review appropriate actions with cmd line passed files
	if ( args.isEmpty() )
	{
		on_action_New_triggered();
		//mdiArea->activateNextSubWindow();
	}
    else
        foreach (QString arg, args)
            OpenWorkspace( q2a( arg ) );

    //InitialUpdateActions();
}


bool CBratMainWindow::OkToContinue()
{
	if ( mModel.RootWorkspace() == nullptr )
		return true;

	int r = 
		QMessageBox::warning( this, base_t::windowTitle(),
		base_t::tr( "Do you want to save your changes?" ),
		QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel );

	if ( r == QMessageBox::Yes ) 
	{
		return SaveWorkspace();
	}
	else 
	if ( r == QMessageBox::Cancel ) 
	{
		return false;
	}

	return true;
}



void CBratMainWindow::closeEvent( QCloseEvent *event )
{
	//auto list = mdiArea->subWindowList();
	//auto const size = list.size();
	//for ( int j = 0; j < size; j++ )
	//{
	//	CEditorBase *pe = dynamic_cast<CEditorBase*>( list[ j ]->widget() );
		//if ( !pe->okToContinue() )
		if ( !OkToContinue() )
		{
			event->ignore();
			return;
		}
	//}

	////mStopped = true;
	////if ( QThreadPool::globalInstance()->activeThreadCount() )
	////	QThreadPool::globalInstance()->waitForDone();

	mManager->CloseAllSubWindows();
	if ( !mManager->SubWindowList().isEmpty() ) 
	{
		event->ignore();
	}
	else {
		WriteSettings();
		event->accept();
	}
}


//virtual 
CBratMainWindow::~CBratMainWindow()
{
	assert__( smInstance );

	smInstance = nullptr;

	delete mMainWorkingDock;
	delete mOutputDock;
}





/////////////////////////////////////////////////////////////////////////
//                      LINKS to GUI
/////////////////////////////////////////////////////////////////////////


void CBratMainWindow::EmitWorkspaceChanged()
{
	emit WorkspaceChanged( &mModel );
	emit WorkspaceChanged( mModel.Workspace< CWorkspaceDataset >() );
	emit WorkspaceChanged( mModel.Workspace< CWorkspaceOperation >() );
	emit WorkspaceChanged( mModel.Workspace< CWorkspaceDisplay >() );
}



void CBratMainWindow::EnableCtrlWorkspace()
{
	//m_guiPanel->GetDatasetPanel()->EnableCtrl();
	//m_guiPanel->GetOperationPanel()->EnableCtrl();
	//m_guiPanel->GetDisplayPanel()->EnableCtrl();
}
void CBratMainWindow::LoadWorkspace()
{
	//m_guiPanel->GetDatasetPanel()->LoadDataset();
	//m_guiPanel->GetOperationPanel()->LoadOperation();
	//m_guiPanel->GetDisplayPanel()->LoadDisplay();
}
void CBratMainWindow::ResetWorkspace()
{
	//m_guiPanel->GetDatasetPanel()->Reset();
	//m_guiPanel->GetOperationPanel()->Reset();
	//m_guiPanel->GetDisplayPanel()->Reset();

	//m_guiPanel->GetDatasetPanel()->ClearAll();
	//m_guiPanel->GetOperationPanel()->ClearAll();
	//m_guiPanel->GetDisplayPanel()->ClearAll();
}



/////////////////////////////////////////////////////////////////////////
//                      WORKSPACE ACTIONS
/////////////////////////////////////////////////////////////////////////

bool CBratMainWindow::DoEmptyWorkspace()
{
	mModel.Reset();
	ResetWorkspace();
	EnableCtrlWorkspace();
	setWindowTitle( makeWindowTitle( QString() ) );
	EmitWorkspaceChanged();
	return true;
}

void CBratMainWindow::SetCurrentWorkspace( const CWorkspace *wks )
{
	assert__( wks );

	ResetWorkspace();
	LoadWorkspace();
	EnableCtrlWorkspace();


    mMainWorkingDock->SelectTab( ETabName::eDataset );
    EmitWorkspaceChanged();

	QString curFile = t2q( wks->GetPath() );
    QString shownName;

    if ( !wks->GetName().empty() )
    {
        shownName = t2q( wks->GetName() );
        //
        // Most recent files logic
        //
		mRecentFilesProcessor->setCurrentFile( curFile );
	}

	setWindowTitle( makeWindowTitle( shownName ) );

    setWindowModified( false );
}


bool CBratMainWindow::OpenWorkspace( const std::string &path )
{
	//return DoEmptyWorkspace();		// TODO DELETE THIS AFTER DEBUGGING

	if ( path.empty() )
	{
		DoEmptyWorkspace();
	}
	else
	{
		std::string error_msg;
		CWorkspace* wks = mModel.LoadWorkspace( path, error_msg );
		if ( !error_msg.empty() )
		{
			assert__( !wks );

			SimpleWarnBox( error_msg );
			DoEmptyWorkspace();
			return false;
		}

		SetCurrentWorkspace( wks );
	}

	return true;
}

void CBratMainWindow::on_action_New_triggered()
{
    static QString last_path = t2q( mSettings.BratPaths().WorkspacesPath() );

	CWorkspaceDialog dlg( this, CTreeWorkspace::eNew, nullptr, last_path, mModel );
	if ( dlg.exec() == QDialog::Accepted )
	{
		last_path = t2q( dlg.mPath );

		std::string error_msg;
		CWorkspace* wks = mModel.CreateWorkspace( dlg.mName, dlg.mPath, error_msg );
		if ( error_msg.empty() )
			mModel.SaveWorkspace( error_msg );
		if ( !error_msg.empty() )
			SimpleWarnBox( error_msg );

		if ( wks != nullptr )
			SetCurrentWorkspace( wks );
	}
}


void CBratMainWindow::on_action_Open_triggered()
{
    static QString last_path = t2q( mSettings.BratPaths().WorkspacesPath() );

	CWorkspaceDialog dlg( this, CTreeWorkspace::eOpen, nullptr, last_path, mModel );
	if ( dlg.exec() == QDialog::Accepted )
	{
		last_path = t2q( dlg.mPath );
		OpenWorkspace( dlg.mPath );
	}
}


bool CBratMainWindow::SaveWorkspace()
{
	std::string error_msg;
	if ( !mModel.SaveWorkspace( error_msg ) )						// TODO save failed but life goes on???
	{
		SimpleWarnBox( error_msg );
		return false;
	}

    const CWorkspace *wks = mModel.RootWorkspace();
	if ( wks != nullptr )
	{
		SetCurrentWorkspace( wks );
		std::cout << wks->GetName() + " saved." << std::endl;		// TODO log this
	}
	return true;
}

void CBratMainWindow::on_action_Save_triggered()
{
	SaveWorkspace();
}


void CBratMainWindow::on_action_Save_As_triggered()		//Save As implies (better, would imply) moving a directory...
{
    NOT_IMPLEMENTED
    //if (activeTextEditor())
    //    activeTextEditor()->saveAs();
}


void CBratMainWindow::on_action_Import_Workspace_triggered()
{
    static QString last_path = t2q( mSettings.BratPaths().WorkspacesPath() );

	CWorkspace *wks = mModel.RootWorkspace();			assert__( wks != nullptr );

	CWorkspaceDialog dlg( this, CTreeWorkspace::eImport, wks, last_path, mModel );
	if ( dlg.exec() != QDialog::Accepted )
		return;

	last_path = t2q( dlg.mPath );

	std::string error_msg;
	if ( !mModel.ImportWorkspace( dlg.mPath, dlg.mDatasets, dlg.mFormulas, dlg.mOperations, dlg.mViews, dlg.mImportFormulas, error_msg ) )
		SimpleErrorBox( error_msg );
	else
		SetCurrentWorkspace( mModel.RootWorkspace() );	//so far is the same as wks, but implementation must be free to change
}


void CBratMainWindow::on_action_Rename_Workspace_triggered()
{
    static QString last_path = t2q( mSettings.BratPaths().WorkspacesPath() );

	CWorkspace *wks = mModel.RootWorkspace();
	if ( wks == nullptr )											//TODO: update actions should have prevented this
	{
		SimpleWarnBox( "There is no current workspace opened" );		
		return;
	}

	CWorkspaceDialog dlg( this, CTreeWorkspace::eRename, wks, last_path, mModel );
	if ( dlg.exec() == QDialog::Accepted )
	{
		last_path = t2q( dlg.mPath );

		NOT_IMPLEMENTED
	}
}


void CBratMainWindow::on_action_Delete_Workspace_triggered()
{
    static QString last_path = t2q( mSettings.BratPaths().WorkspacesPath() );

	CWorkspace *wks = mModel.RootWorkspace();
	if ( wks == nullptr )											//TODO: update actions should have prevented this
	{
		SimpleWarnBox( "There is no current workspace opened" );
		return;
	}

	CWorkspaceDialog dlg( this, CTreeWorkspace::eDelete, wks, last_path, mModel );	// TODO is it worth showing the dialog? All widgets are dimmed except the delete
	if ( dlg.exec() == QDialog::Accepted )
	{
		last_path = t2q( dlg.mPath );

		NOT_IMPLEMENTED
	}
}


void CBratMainWindow::openRecentWorkspace_triggered( QAction *action )
{
	if ( action )
		OpenWorkspace( q2a( action->data().toString() ) );
}


//////////////////////////////////////////////
// NOTE: Action exit automatically connected 
//	in designer; see the closeEvent method
//////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////
//						EDIT ACTIONS
/////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////
// NOTE: All edit actions are experimental
//////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
//						VIEW ACTIONS
/////////////////////////////////////////////////////////////////////////

void CBratMainWindow::on_action_Zoom_In_triggered()
{
	mManager->Map()->zoomIn();
}


void CBratMainWindow::on_action_Zoom_Out_triggered()
{
	mManager->Map()->zoomOut();
}


void CBratMainWindow::on_action_Re_center_triggered()
{
	mManager->Map()->zoomToFullExtent();
}


void CBratMainWindow::on_action_Refresh_Map_triggered()
{
	mManager->Map()->refresh();
}


void CBratMainWindow::on_action_Graphic_Settings_triggered()
{
	NOT_IMPLEMENTED
}


void CBratMainWindow::on_action_Save_Map_Image_triggered()
{
	NOT_IMPLEMENTED
}


void CBratMainWindow::on_action_Full_Screen_triggered()
{
	QTimer::singleShot( 1000, this, isFullScreen() ? SLOT( showNormal() ) : SLOT( showFullScreen() ) );
}




/////////////////////////////////////////////////////////////////////////
//                      TOOLS ACTIONS
/////////////////////////////////////////////////////////////////////////


void CBratMainWindow::on_action_One_Click_triggered()
{
	NOT_IMPLEMENTED
}


void CBratMainWindow::on_action_Workspace_Tree_triggered()
{
	CWorkspaceElementsDialog dlg( this, mModel.Tree() );
	dlg.exec();
}


void CBratMainWindow::on_action_Launch_Scheduler_triggered()
{
	NOT_IMPLEMENTED
}


void CBratMainWindow::on_action_Options_triggered()
{
    CApplicationSettingsDlg dlg( mSettings, this );
	if ( dlg.exec() == QDialog::Accepted )
		emit SettingsUpdated();

	NOT_IMPLEMENTED
}


/////////////////////////////////////////////////////////////////////////
//                      WINDOW ACTIONS
/////////////////////////////////////////////////////////////////////////


void CBratMainWindow::on_action_Close_triggered()
{
	NOT_IMPLEMENTED
}

//////////////////////////////////////////
//NOTE: Close All connected in constructor
//////////////////////////////////////////

void CBratMainWindow::on_action_Tile_triggered()
{
	NOT_IMPLEMENTED
}


void CBratMainWindow::on_action_Cascade_triggered()
{
	NOT_IMPLEMENTED
}


void CBratMainWindow::on_action_Next_triggered()
{
	NOT_IMPLEMENTED
}


void CBratMainWindow::on_action_Previous_triggered()

{
	NOT_IMPLEMENTED
}


void CBratMainWindow::UpdateWindowMenu()
{
	action_Close_All->setEnabled( !mManager->SubWindowList().empty() );
}



/////////////////////////////////////////////////////////////////////////
//                      HELP ACTIONS
/////////////////////////////////////////////////////////////////////////


void CBratMainWindow::on_action_About_triggered()
{
	QMessageBox::about(this, tr("About BRAT"), QString( "Welcome to BRAT " ) + BRAT_VERSION + " - " + PROCESSOR_ARCH + "\n(C)opyright CNES/ESA" );
}


static bool thread_finished = true;

class DebugThread : public QThread
{
    //Q_OBJECT
private:
	void run()
	{
		for ( ;; )
		{
			LOG_TRACEstd( "DEBUG hello from worker thread " + n2s<std::string>( thread()->currentThreadId() ) );
			msleep( 1000 );
			if ( thread_finished )
				return;
		}
	}
};
class LogThread : public QThread
{
    //Q_OBJECT
private:
	void run()
	{
		int i = 0;
		for ( ;; )
		{
			switch ( ++i % 3 )
			{
				case 0:
					LOG_INFO( "LOG hello from worker thread " + n2s<std::string>( thread()->currentThreadId() ) );
					break;
				case 1:
					LOG_WARN( "LOG hello from worker thread " + n2s<std::string>( thread()->currentThreadId() ) );
					break;
				case 2:
					LOG_FATAL( "LOG hello from worker thread " + n2s<std::string>( thread()->currentThreadId() ) );
					break;
			}
			msleep( 10 );
			if ( thread_finished )
				return;
		}
	}
};


static DebugThread thread_d;
static LogThread thread_l1, thread_l2;


void CBratMainWindow::on_action_Test_triggered()
{
	if ( thread_finished )
	{
		thread_finished = false;
		thread_d.start();
		thread_l1.start();
		thread_l2.start();
	}
	else
	{
		thread_finished = true;
		thread_d.terminate();
		thread_l1.terminate();
		thread_l2.terminate();

		thread_d.wait();
		thread_l1.wait();
		thread_l2.wait();
	}
	//for ( ;; )
	//{
	//	qDebug() << "hello from GUI thread " << qApp->thread()->currentThreadId();
	//	qApp->processEvents();
	//	//QTest::qSleep( 1000 );
	//}

	/*

#if defined (DEBUG) || defined(_DEBUG)

    std::string
    msg = "Main application settings file: " + mSettings.FilePath();
    msg += "\nCurrent Working Directory: " + q2a( QDir::currentPath() );
    msg += "\n\n";
    msg += mSettings.BratPaths().ToString();
    SimpleMsgBox( msg );

    // NOT_IMPLEMENTED

#endif
	*/
}



/////////////////////////////////////////////////////////////////////////
//                      STATUS-BAR ACTIONS
/////////////////////////////////////////////////////////////////////////


void CBratMainWindow::ShowProgress( int theProgress, int theTotalSteps )
{
	if ( theProgress == theTotalSteps )
	{
		mProgressBar->reset();
		mProgressBar->hide();
	}
	else
	{
		//only call show if not already hidden to reduce flicker
		if ( !mProgressBar->isVisible() )
		{
			mProgressBar->show();
		}
		mProgressBar->setMaximum( theTotalSteps );
		mProgressBar->setValue( theProgress );

		if ( mProgressBar->maximum() == 0 )
		{
			// for busy indicator (when minimum equals to maximum) the oxygen Qt style (used in KDE)
			// has some issues and does not start busy indicator animation. This is an ugly fix
			// that forces creation of a temporary progress bar that somehow resumes the animations.
			// Caution: looking at the code below may introduce mild pain in stomach.
			if ( strcmp( QApplication::style()->metaObject()->className(), "Oxygen::Style" ) == 0 )
			{
				QProgressBar pb;
				pb.setAttribute( Qt::WA_DontShowOnScreen ); // no visual annoyance
				pb.setMaximum( 0 );
				pb.show();
				qApp->processEvents();
			}
		}
	}
}


void CBratMainWindow::CanvasRefreshStarted()
{
	ShowProgress( -1, 0 ); // trick to make progress bar show busy indicator
}

void CBratMainWindow::CanvasRefreshFinished()
{
	ShowProgress( 0, 0 ); // stop the busy indicator
}




/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
//							UPDATE STATE
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////


void CBratMainWindow::UpdateWorkspaceUI( const CModel *model )
{
	Q_UNUSED( model );

	const bool enable = mModel.RootWorkspace() != nullptr;

	CActionInfo::UpdateActionsState( {

		eAction_Save,
		eAction_Save_As,
		eAction_Import_Workspace,
		eAction_Rename_Workspace,
		eAction_Delete_Workspace,

		eAction_One_Click,
		eAction_Workspace_Tree,
		eAction_Operations,
		eAction_Dataset,
		eAction_Filter,

		eAction_Cut,
		eAction_Copy,
		eAction_Paste,
		eAction_Undo,
		eAction_Redo,
		eAction_Delete,
		eAction_Select_All,

	}, enable );


	mMainWorkingDock->SelectTab( eDataset );
	mMainWorkingDock->setEnabled( enable );
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_BratMainWindow.cpp"
