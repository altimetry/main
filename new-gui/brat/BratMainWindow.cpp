#include "stdafx.h"

#include <osg/Version>

#include "new-gui/Common/BratVersion.h"
#include "new-gui/Common/ccore-types.h"
#include "new-gui/Common/QtUtils.h"
#include "new-gui/Common/ConfigurationKeywords.h"

#include "BratApplication.h"
#include "ApplicationSettingsDlg.h"
#include "ApplicationLogger.h"

#include "DataModels/DisplayFilesProcessor.h"
#include "DataModels/Workspaces/Workspace.h"
#include "DataModels/PlotData/ZFXYPlot.h"
#include "DataModels/PlotData/XYPlot.h"
#include "DataModels/PlotData/WorldPlot.h"


#include "GUI/ActionsTable.h"
#include "GUI/WorkspaceDialog.h"
#include "GUI/WorkspaceElementsDialog.h"
#include "GUI/TabbedDock.h"
#include "GUI/ControlPanels/ControlPanel.h"
#include "GUI/ControlPanels/DatasetBrowserControls.h"
#include "GUI/ControlPanels/DatasetFilterControls.h"
#include "GUI/ControlPanels/OperationControls.h"
#include "GUI/ControlPanels/ProcessesTable.h"
#include "GUI/DisplayWidgets/TextWidget.h"
#include "GUI/DisplayWidgets/GlobeWidget.h"
#include "GUI/DisplayEditors/MapEditor.h"
#include "GUI/DisplayEditors/PlotEditor.h"

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


CControlPanel* CBratMainWindow::MakeWorkingPanel( ETabName tab )
{
	assert__( mProcessesTable && mDesktopManager );

	switch ( tab )
	{
		case eDataset:
			return new ControlsPanelType< eDataset >::type( mDesktopManager );
			break;
		case eFilter:
			return new ControlsPanelType< eFilter >::type( mDesktopManager );
			break;
		case eOperations:
			return new ControlsPanelType< eOperations >::type( mProcessesTable, mDesktopManager );
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
	mMainWorkingDock->setObjectName("mMainWorkingDock");
	mMainWorkingDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );	//mMainWorkingDock->setMaximumWidth( max_main_dock_width );
	addDockWidget( Qt::LeftDockWidgetArea, mMainWorkingDock, Qt::Vertical );

	auto 
	tab = mMainWorkingDock->AddTab( MakeWorkingPanel( eDataset ), "Datasets" );			assert( mMainWorkingDock->TabIndex( tab ) == eDataset );
	mMainWorkingDock->SetTabToolTip( tab, "Dataset browser" );
    connect( this, SIGNAL( WorkspaceChanged( CWorkspaceDataset* ) ), WorkingPanel< eDataset >(), SLOT( HandleWorkspaceChanged( CWorkspaceDataset* ) ) );

	tab = mMainWorkingDock->AddTab( MakeWorkingPanel( eFilter ), "Filters" );	 		assert( mMainWorkingDock->TabIndex( tab ) == eFilter );
	mMainWorkingDock->SetTabToolTip( tab, "Dataset filter" );
	connect( this, SIGNAL( WorkspaceChanged( CWorkspaceDataset* ) ), WorkingPanel< eFilter >(), SLOT( HandleWorkspaceChanged( CWorkspaceDataset* ) ) );

	tab = mMainWorkingDock->AddTab( MakeWorkingPanel( eOperations ), "Operations" );		assert( mMainWorkingDock->TabIndex( tab ) == eOperations );
	mMainWorkingDock->SetTabToolTip( tab, "Quick or advanced operations"  );
	connect( this, SIGNAL( WorkspaceChanged( CModel* ) ), WorkingPanel< eOperations >(), SLOT( HandleSelectedWorkspaceChanged( CModel* ) ) );
	connect( WorkingPanel< eOperations >(), SIGNAL( SyncProcessExecution( bool ) ), this, SLOT( HandleSyncProcessExecution( bool ) ) );

	connect( WorkingPanel< eDataset >(), SIGNAL( CurrentDatasetChanged(CDataset*) ), WorkingPanel< eFilter >(), SLOT( HandleDatasetChanged(CDataset*) ) );

	LOG_TRACE( "Finished working dock construction." );
}


void CBratMainWindow::CreateOutputDock()
{
	static bool initialized = false;				assert__(!initialized);

	LOG_TRACE( "Starting output dock construction..." );

	initialized = true;

	mOutputDock = new CTabbedDock( "Output", this );
	mOutputDock->setObjectName("mOutputDock");
	mOutputDock->setAllowedAreas( Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea );
	addDockWidget( Qt::BottomDockWidgetArea, mOutputDock );

	//Log Tab

	mLogFrame = new CLogShell( this );
	mLogFrame->setObjectName( QString::fromUtf8( "mLogShell" ) );
	mLogFrameIndex = mOutputDock->TabIndex( mOutputDock->AddTab( mLogFrame, "Log" ) );

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


	//Processes Tab

	mProcessesTable = new CProcessesTable( this );
	mProcessesTable->setObjectName( QString::fromUtf8( "mProcessesTable" ) );
	mProcessesTableIndex = mOutputDock->TabIndex( mOutputDock->AddTab( mProcessesTable, "Processes" ) );


	LOG_TRACE( "Finished output dock construction." );
}


void CBratMainWindow::CreateDocks()
{
	assert__( mDesktopManager );

    // Docks (see also the Docks editors section in this ctor)
    //
    setDockOptions( dockOptions() | VerticalTabs );     //not set in designer

    setCorner( Qt::TopLeftCorner, Qt::LeftDockWidgetArea );
    setCorner( Qt::TopRightCorner, Qt::RightDockWidgetArea );
    setCorner( Qt::BottomRightCorner, Qt::RightDockWidgetArea );
    setCorner( Qt::BottomLeftCorner, Qt::LeftDockWidgetArea );		//Qt::BottomDockWidgetArea

	CreateOutputDock();		//must be called before CreateWorkingDock
	CreateWorkingDock();

	if ( mDesktopManager->MapDock() )
		addDockWidget( Qt::LeftDockWidgetArea, mDesktopManager->MapDock() );
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
	connect( action_Close_All, SIGNAL(triggered()), mDesktopManager, SLOT(closeAllSubWindows()) );

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


    // mMainToolsToolBar: Map actions
    //
	mSelectionButton = CMapWidget::CreateMapSelectionActions( mMainToolsToolBar, mActionSelectFeatures, mActionSelectPolygon, mActionDeselectAll );
	QAction *a = CActionInfo::CreateAction( mMainToolsToolBar, eAction_Separator );
	mMainToolsToolBar->insertAction( action_Graphic_Settings, a );
	mMainToolsToolBar->insertAction( a, mActionDeselectAll );
	mMainToolsToolBar->insertWidget( mActionDeselectAll, mSelectionButton );
	mDesktopManager->Map()->ConnectParentSelectionActions( mSelectionButton, mActionSelectFeatures, mActionSelectPolygon, mActionDeselectAll );

	mActionDecorationGrid = CMapWidget::CreateGridAction( mMainToolsToolBar );
	mMainToolsToolBar->insertAction( action_Graphic_Settings, mActionDecorationGrid );
	mDesktopManager->Map()->ConnectParentGridAction( mActionDecorationGrid );


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
	connect( this, SIGNAL( WorkspaceChanged( CModel* ) ), this, SLOT( WorkspaceChangedUpdateUI( CModel* ) ) );

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
	//
	const int index = 0;

	CMapWidget::CreateRenderWidgets( statusBar(), mProgressBar, mRenderSuppressionCBox );
	statusBar()->insertPermanentWidget( index, mRenderSuppressionCBox, 0 );
	statusBar()->insertPermanentWidget( index, mProgressBar, 1 );

	mDesktopManager->Map()->ConnectParentRenderWidgets( mProgressBar, mRenderSuppressionCBox );


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


CBratMainWindow::CBratMainWindow( CBratApplication &app ) 

	: QMainWindow( nullptr)

	, mApp( app )

	, mSettings( mApp.Settings() )

	, mModel( CModel::CreateInstance( mSettings.BratPaths() ) )

{
	LOG_TRACE( "Starting main window construction..." );

	assert__( !smInstance );
    assert__( mSettings.BratPaths().IsValid()==true );

	mApp.ShowSplash( "Creating main window..." );

	connect( this, SIGNAL( SettingsUpdated() ), &mApp, SLOT( UpdateSettings() ) );

	CMapWidget::SetQGISDirectories(
		mSettings.BratPaths().mQgisPluginsDir,
		mSettings.BratPaths().mVectorLayerPath,
		mSettings.BratPaths().RasterLayerPath() );

	CGlobeWidget::SetOSGDirectories( mSettings.BratPaths().mGlobeDir );

	CWorkspaceSettings::SetApplicationPaths( mSettings.BratPaths() );

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
		mDesktopManager = new CDesktopManagerSDI( mSettings.BratPaths(), this );
	else
		mDesktopManager = new CDesktopManagerMDI( mSettings.BratPaths(), this );


    // Docked controls
    //
	CreateDocks();


    // Menu commands
    //
	ProcessMenu();


    // Status Bar
    //
	FillStatusBar();


	mApp.ShowSplash( "Restoring layout..." );

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


	mApp.ShowSplash( "Loading display or workspace..." );


    // Load command line or user settings referenced files 
    //
	// Let any exceptions occurred while initializing leak to main and terminate,
	//	unless locally decided otherwise;
	// LeakExceptions is set to false by LoadCmdLineFiles
	//
	mApp.LeakExceptions( true );	
	if ( !mApp.SplashAvailable() )
		QTimer::singleShot( 0, this, SLOT( LoadCmdLineFiles() ) );
	else
		LoadCmdLineFiles();

	mApp.EndSplash( this );

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
	//lambdas

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


	// function body

	LOG_TRACE( "Starting command line arguments parse." );
	
	mApp.LeakExceptions( false );	//resetting to normal behavior; if initialization crashes this has no effect anyway

	if ( mApp.OperatingInDisplayMode() )
	{
		LOG_TRACE( "Starting in simple display mode." );

		if ( StartDisplayMode() )
			LOG_TRACE( "Successfully started in simple display mode." );
	}
	else
	{
		QStringList args = QCoreApplication::arguments();
		args.removeFirst();
		QString wkspc_dir;
		if ( !args.empty() )
			wkspc_dir = args[ 0 ];

		// try to find out which workspace to load
		//	- if one was passed in the command line, it takes precedence over the mLoadLastWorkspaceAtStartUp option
		//	- if none was passed and LoadLastWorkspaceAtStartUp is false, invalidate the workspace string
		//
		if ( wkspc_dir.isEmpty() )
		{
			if ( mSettings.mLoadLastWorkspaceAtStartUp )
			{
				wkspc_dir = mSettings.m_lastWksPath.c_str();
			}
			else
				wkspc_dir.clear();
		}

		if ( wkspc_dir.isEmpty() )
		{
			LOG_TRACE( "No workspace required, starting without one." );

			DoNoWorkspace();	//no workspace given and the user does not want to LoadLastWorkspaceAtStartUp
		}
		else
		{
			// let any exceptions leak to main
			//	- on failure other than exception, OpenWorkspace makes DoNoWorkspace

			if ( !OpenWorkspace( q2a( wkspc_dir ) ) )
			{
				SimpleWarnBox( "Unable to load the required workspace located in '" + wkspc_dir + "' " );
			}
			else
			{
				//v3: GotoLastPageReached();

				mMainWorkingDock->SelectTab( compute_last_page_reached() );
				WorkingPanel<eOperations>()->SetAdvancedMode( mSettings.mAdvancedOperations );
			}
		}
	}

	LOG_TRACE( "Finished command line arguments processing." );
}


void CBratMainWindow::StopDisplayMode()
{
	assert__( mApp.OperatingInDisplayMode() );

	CActionInfo::TriggerAction( eAction_Exit );
}

bool CBratMainWindow::StartDisplayMode()
{
    QStringList args = QCoreApplication::arguments();
	DoNoWorkspace();
	setVisible( false );
	CDisplayFilesProcessor p;
	if ( p.Process( args ) )
	{
		std::vector< CAbstractDisplayEditor* > editors;

		connect( mDesktopManager, SIGNAL( AllSubWindowsClosed() ), this, SLOT( StopDisplayMode() ) );

		auto &plots = p.plots();
		if ( p.isZFLatLon() )		// =================================== WorldPlot()
		{
			for ( auto &plot : plots )
			{
				CWPlot* wplot = dynamic_cast<CWPlot*>( plot );
				if ( wplot == nullptr )
					continue;

				editors.push_back( new CMapEditor( &p, wplot, this ) );
			}
		}
		else if ( p.isYFX() )		// =================================== XYPlot();
		{
			for ( auto &plot : plots )
			{
				CPlot* yfxplot = dynamic_cast<CPlot*>( plot );
				if ( yfxplot == nullptr )
					continue;

				editors.push_back( new CPlotEditor( &p, yfxplot, this ) );
			}
		}
		else if ( p.isZFXY() )		// =================================== ZFXYPlot();
		{
			for ( auto &plot : plots )
			{
				CZFXYPlot* zfxyplot = dynamic_cast<CZFXYPlot*>( plot );
				if ( zfxyplot == nullptr )
					continue;

				editors.push_back( new CPlotEditor( &p, zfxyplot, this ) );
			}
		}
		else
		{
			CException e( "CBratDisplayApp::OnInit - Only World Plot Data, XY Plot Data and ZFXY Plot Data are implemented", BRATHL_UNIMPLEMENT_ERROR );
			LOG_TRACE( e.what() );
			throw e;
		}

		for ( auto *editor : editors )
		{
			auto subWindow = mDesktopManager->AddSubWindow( editor );
			subWindow->show();
		}
		return true;
	}

	return false;
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
	static const std::string processes_question =
		"There are processes launched by BRAT still running.\n"
		"If you exit they will be terminated now without finishing and information can be lost.\n"
		"Do you still want to exit?";

	if ( !mProcessesTable->Empty() && !SimpleQuestion( processes_question ) )
	{
		event->ignore();
		return;
	}

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

	mDesktopManager->CloseAllSubWindows();
	if ( !mDesktopManager->SubWindowList().isEmpty() ) 
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



/////////////////////////////////////////////////////////////////////////
//                      WORKSPACE ACTIONS
/////////////////////////////////////////////////////////////////////////

bool CBratMainWindow::DoNoWorkspace()
{
	mModel.Reset();
	mDesktopManager->CloseAllSubWindows();
	setWindowTitle( makeWindowTitle( QString() ) );
    EmitWorkspaceChanged();
	return true;
}

void CBratMainWindow::SetCurrentWorkspace( const CWorkspace *wks )
{
	assert__( wks );

	mDesktopManager->CloseAllSubWindows();
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
	//return DoNoWorkspace();		// TODO DELETE THIS AFTER DEBUGGING

	if ( path.empty() )
	{
		DoNoWorkspace();
	}
	else
	{
		std::string error_msg;
		CWorkspace* wks = mModel.LoadWorkspace( path, error_msg );
		if ( !error_msg.empty() )
		{
			assert__( !wks );

			SimpleWarnBox( error_msg );
			DoNoWorkspace();
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



void CBratMainWindow::on_action_Close_Workspace_triggered()
{
	DoNoWorkspace();
}


bool CBratMainWindow::SaveWorkspace()
{
	std::string error_msg;
	bool result = mModel.SaveWorkspace( error_msg );
    const CWorkspace *wks = mModel.RootWorkspace();
	if ( result )
	{
		LOG_TRACEstd( wks->GetName() + " saved." );
	}
	else
	{
		SimpleWarnBox( error_msg );
		LOG_TRACEstd( wks->GetName() + " save failed: " + error_msg );
		return false;
	}

	return result;
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
	mDesktopManager->Map()->zoomIn();
}


void CBratMainWindow::on_action_Zoom_Out_triggered()
{
	mDesktopManager->Map()->zoomOut();
}


void CBratMainWindow::on_action_Re_center_triggered()
{
	mDesktopManager->Map()->zoomToFullExtent();
}


void CBratMainWindow::on_action_Refresh_Map_triggered()
{
	mDesktopManager->Map()->refresh();
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
	action_Close_All->setEnabled( !mDesktopManager->SubWindowList().empty() );
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
//	if ( thread_finished )
//	{
//		thread_finished = false;
//		thread_d.start();
//		thread_l1.start();
//		thread_l2.start();
//	}
//	else
//	{
//		thread_finished = true;
//		thread_d.terminate();
//		thread_l1.terminate();
//		thread_l2.terminate();

//		thread_d.wait();
//		thread_l1.wait();
//		thread_l2.wait();
//	}
//	//for ( ;; )
//	//{
//	//	qDebug() << "hello from GUI thread " << qApp->thread()->currentThreadId();
//	//	qApp->processEvents();
//	//	//QTest::qSleep( 1000 );
//	//}

//	/*

#if defined (DEBUG) || defined(_DEBUG)

    std::string
    msg = "Main application settings file: " + mSettings.FilePath();
    msg += "\nCurrent Working Directory: " + q2a( QDir::currentPath() );
    msg += "\n\n";
    msg += mSettings.BratPaths().ToString();
    SimpleMsgBox( msg );

    // NOT_IMPLEMENTED

#endif
    //*/
}



/////////////////////////////////////////////////////////////////////////
//                      STATUS-BAR ACTIONS
/////////////////////////////////////////////////////////////////////////





/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
//							UPDATE STATE
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////


void CBratMainWindow::WorkspaceChangedUpdateUI( CModel *model )
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


void CBratMainWindow::EnableMapSelectionActions( bool enable )
{
	if ( mSelectionButton )
	{
		mSelectionButton->setEnabled( enable );
		mActionDeselectAll->setEnabled( enable );
	}
}


void CBratMainWindow::HandleSyncProcessExecution( bool executing )
{
	CActionInfo::UpdateActionsState( {

		eAction_Open,
		eAction_New,
		eAction_CloseWorkspace,
		eAction_Save,
		eAction_Save_As,
		eAction_Import_Workspace,
		eAction_Rename_Workspace,
		eAction_Delete_Workspace,

		eAction_Options,

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

	}, !executing );


	mRecentFilesProcessor->SetEnabled( !executing );
	EnableMapSelectionActions( !executing );
	//mDesktopManager->Map()->EnableMapMeasureActions( !executing );
	mMainWorkingDock->setEnabled( !executing );
	mDesktopManager->setEnabled( !executing );

	if ( executing )
		mOutputDock->SelectTab( mLogFrameIndex );
}


///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_BratMainWindow.cpp"
