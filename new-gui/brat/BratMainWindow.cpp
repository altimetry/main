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
#include "stdafx.h"

#include <osg/Version>

#include "common/BratVersion.h"
#include "common/ccore-types.h"
#include "new-gui/Common/QtUtils.h"
#include "new-gui/Common/GUI/TextWidget.h"
#include "new-gui/Common/GUI/ProcessesTable.h"

#include "BratApplication.h"
#include "ApplicationSettingsDlg.h"
#include "BratLogger.h"

#include "DataModels/DisplayFilesProcessor.h"
#include "DataModels/Workspaces/Workspace.h"
#include "DataModels/Workspaces/WorkspaceSettings.h"
#include "DataModels/Filters/BratFilters.h"
#include "DataModels/PlotData/Plots.h"

#include "GUI/DisplayWidgets/GlobeWidget.h"
#include "GUI/DisplayEditors/MapEditor.h"
#include "GUI/DisplayEditors/PlotEditor.h"

#include "GUI/ActionsTable.h"
#include "GUI/WorkspaceDialog.h"
#include "GUI/WorkspaceElementsDialog.h"
#include "GUI/WorkspaceViewsDialog.h"
#include "GUI/OperationViewsDialog.h"
#include "GUI/ActiveViewsDialog.h"
#include "GUI/TabbedDock.h"
#include "GUI/ControlPanels/DatasetsBrowserControls.h"
#include "GUI/ControlPanels/DatasetsRadsBrowserControls.h"
#include "GUI/ControlPanels/BratFilterControls.h"
#include "GUI/ControlPanels/OperationControls.h"
#include "GUI/DisplayEditors/Dialogs/ExportImageDialog.h"

#include "BratMainWindow.h"



////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                            Main Window private settings keywords
////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static const std::string GROUP_MAIN_WINDOW =			"MainWindow";

static const std::string KEY_MAIN_WINDOW_GEOMETRY =		"geometry";
static const std::string KEY_MAIN_WINDOW_STATE =		"state";
static const std::string KEY_MAIN_WINDOW_MAXIMIZED =	"maximized";





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
QString CBratMainWindow::MakeWindowTitle( const QString &title )// = QString() //returns the application name if title is empty
{
	static const QString &app_title = qApp->applicationName()

#if defined (DEBUG) || defined(_DEBUG)

		+ build_configuration.c_str();

#endif
	;

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
		! mRecentFilesProcessor->WriteSettings() 
		||
		! AppSettingsWriteSection( GROUP_MAIN_WINDOW,
		
			k_v( KEY_MAIN_WINDOW_GEOMETRY, saveGeometry() ),
			k_v( KEY_MAIN_WINDOW_STATE, saveState() ),
			k_v( KEY_MAIN_WINDOW_MAXIMIZED, isMaximized() )
		) 
		)
		LOG_INFO( "Unable to save BRAT the main window settings." );

		mSettings.Sync();

	return mSettings.Status() == QSettings::NoError;
}


CControlPanel* CBratMainWindow::MakeWorkingPanel( ETabName tab )
{
	assert__( mProcessesTable && mDesktopManager );

	switch ( tab )
	{
		case eDataset:
			return new ControlsPanelType< eDataset >::type( mApp, mDesktopManager );
			break;
		case eRADS:
			return new ControlsPanelType< eRADS >::type( mApp, mDesktopManager );
			break;
		case eFilter:
			return new ControlsPanelType< eFilter >::type( mApp, mDesktopManager );
			break;
		case eOperations:
			return new ControlsPanelType< eOperations >::type( mApp, mDesktopManager, mProcessesTable );
			break;
		default:
			assert__( false );
	}

	return nullptr;
}


void CBratMainWindow::CreateWorkingDock()
{
	LOG_TRACE( "Starting working dock (controls panel) construction..." );

	mMainWorkingDock = new CTabbedDock( "Workspace Elements", this );
	mMainWorkingDock->setObjectName("mMainWorkingDock");
	mMainWorkingDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );	
	mMainWorkingDock->setMinimumWidth( min_main_working_dock_width );	
	addDockWidget( Qt::LeftDockWidgetArea, mMainWorkingDock, Qt::Vertical );
	connect( mMainWorkingDock, &CTabbedDock::TabSelected, this, &CBratMainWindow::TabSelected );

	LOG_TRACE( "Added mMainWorkingDock..." );

	auto 
	tab = mMainWorkingDock->AddTab( MakeWorkingPanel( eDataset ), "Datasets" );			assert__( mMainWorkingDock->TabIndex( tab ) == eDataset );
	mMainWorkingDock->SetTabToolTip( tab, "Dataset browser" );

	LOG_TRACE( "Added MakeWorkingPanel( eDataset )..." );

	tab = mMainWorkingDock->AddTab( MakeWorkingPanel( eRADS ), "RADS Datasets" );	 	assert__( mMainWorkingDock->TabIndex( tab ) == eRADS );
	mMainWorkingDock->SetTabToolTip( tab, "RADS Browser" );

	LOG_TRACE( "Added MakeWorkingPanel( eRADS )..." );

	tab = mMainWorkingDock->AddTab( MakeWorkingPanel( eFilter ), "Filters" );	 		assert__( mMainWorkingDock->TabIndex( tab ) == eFilter );
	mMainWorkingDock->SetTabToolTip( tab, "Dataset filter" );

	LOG_TRACE( "Added MakeWorkingPanel( eFilter )..." );

	tab = mMainWorkingDock->AddTab( MakeWorkingPanel( eOperations ), "Operations" );	assert__( mMainWorkingDock->TabIndex( tab ) == eOperations );
	mMainWorkingDock->SetTabToolTip( tab, "Quick or advanced operations"  );

	LOG_TRACE( "Added MakeWorkingPanel( eOperations )..." );

	connect( WorkingPanel< eOperations >(), SIGNAL( SyncProcessExecution( bool ) ), this, SLOT( HandleSyncProcessExecution( bool ) ) );
	connect( WorkingPanel< eOperations >(), SIGNAL( AsyncProcessExecution( bool ) ), this, SLOT( HandleAsyncProcessExecution( bool ) ) );

	action_Satellite_Tracks->setChecked( CDesktopControlsPanel::AutoDrawSatelliteTrack() );

	//notifications from datasets to operations
    connect( WorkingPanel< eDataset >(), SIGNAL( DatasetsChanged(const CDataset*) ), WorkingPanel< eOperations >(), SLOT( HandleDatasetsChanged_Quick(const CDataset*) ) );
    connect( WorkingPanel< eDataset >(), SIGNAL( DatasetsChanged(const CDataset*) ), WorkingPanel< eOperations >(), SLOT( HandleDatasetsChanged_Advanced(const CDataset*) ) );

	connect( WorkingPanel< eRADS >(), SIGNAL( DatasetsChanged(const CDataset*) ), WorkingPanel< eOperations >(), SLOT( HandleDatasetsChanged_Quick(const CDataset*) ) );
	connect( WorkingPanel< eRADS >(), SIGNAL( DatasetsChanged(const CDataset*) ), WorkingPanel< eOperations >(), SLOT( HandleDatasetsChanged_Advanced(const CDataset*) ) );

	//notifications from filters to operations
    connect( WorkingPanel< eFilter >(), SIGNAL( FiltersChanged() ),							WorkingPanel< eOperations >(), SLOT( HandleFiltersChanged() ) );
    connect( WorkingPanel< eFilter >(), SIGNAL( FilterCompositionChanged( std::string ) ),	WorkingPanel< eOperations >(), SLOT( HandleFilterCompositionChanged( std::string ) ) );


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

	mLogFrame = new CBasicLogShell( CBratLogger::Instance(), this );
	mLogFrame->setObjectName( QString::fromUtf8( "mLogShell" ) );
	mLogFrameIndex = mOutputDock->TabIndex( mOutputDock->AddTab( mLogFrame, "Log" ) );

	// wire to logger

	connect( 
		&CBratLogger::Instance(), SIGNAL( OsgLogLevelChanged( int ) ), 
		mLogFrame, SLOT( SetOsgLogLevel( int ) ), Qt::QueuedConnection );

	connect( 
		&CBratLogger::Instance(), SIGNAL( OsgLogMessage( int, QString ) ), 
		mLogFrame, SLOT( OsgNotifySlot( int, QString ) ), Qt::QueuedConnection );

	connect( 
		&CBratLogger::Instance(), SIGNAL( QgisLogMessage( int, QString  ) ),
		mLogFrame, SLOT( QtNotifySlot( int, QString ) ), Qt::QueuedConnection );

	//mLogFrame->SetOsgLogLevel( ns );	//also updates the application logger
	mLogFrame->Deactivate( false );

    LOG_INFO( "Qt version: " + std::string( QT_VERSION_STR ) );
	LOG_INFO( "Python version: " + std::string( PY_VERSION ) );
	LOG_INFO( "OSG version: " + std::string( osgGetVersion() ) + " - " + osgGetSOVersion() + " - " + osgGetLibraryName() );
	LOG_INFO( "osgEarth version: " + std::string( osgEarthGetVersion() ) + " - " + osgEarthGetSOVersion() + " - " + osgEarthGetLibraryName() );
	LOG_INFO( "QGIS version: " + std::string( q2a( QGis::QGIS_VERSION ) ) );	//q2a used for compatibility with QGIS 2.16.1
	if ( getenv("OSG_FILE_PATH") )
		LOG_INFO( "OSG_FILE_PATH = " + std::string( getenv("OSG_FILE_PATH") ) );
	else
		LOG_TRACE( "OSG_FILE_PATH not defined" );

	LOG_TRACEstd( "Initial OSG notification level: " + n2s( CBratLogger::Instance().OsgNotifyLevel() ) );

	const char *QGIS_LOG_FILE = getenv( "QGIS_LOG_FILE" );
	if ( QGIS_LOG_FILE )
		LOG_INFO( "QGIS_LOG_FILE = " + std::string( QGIS_LOG_FILE ) );

	const char *QGIS_DEBUG = getenv( "QGIS_DEBUG" );
	if ( QGIS_DEBUG )
		LOG_INFO( "QGIS_DEBUG = " + std::string( QGIS_DEBUG ) );


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

    // Link to Actions Table
    //
	CActionInfo::UpdateMenuActionsProperties( mMainMenuBar );


    // Menu File
    //
	connect( action_Close_All, SIGNAL(triggered()), mDesktopManager, SLOT(closeAllSubWindows()) );

#if !defined(DEBUG) || !defined(_DEBUG) 
	action_Close_Workspace->setVisible( false );
#endif

    // Menu File / Most recent files logic
    //
	mRecentFilesSeparatorAction = menu_File->insertSeparator( action_Exit );

	mRecentFilesProcessor = new CRecentFilesProcessor( this, "Recent Workspaces", menu_File, action_Exit );
	connect( mRecentFilesProcessor, SIGNAL( triggered( QAction* ) ), this, SLOT( openRecentWorkspace_triggered( QAction* ) ) );

	mRecentFilesSeparatorAction = menu_File->insertSeparator( action_Exit );

	if ( !mRecentFilesProcessor->ReadSettings() )
	{
		CException e( "Error reading the configuration file " + AppSettingsFilePath(), BRATHL_ERROR );
		throw e;
	}


    // mMainToolsToolBar: Map actions
    //
	QAction *after = action_Satellite_Tracks;

	mActionDecorationGrid = CMapWidget::CreateGridAction( mMainToolsToolBar );
	mMainToolsToolBar->insertAction( after, mActionDecorationGrid );
	mDesktopManager->Map()->ConnectParentGridAction( mActionDecorationGrid );

	mActionMapTips = CMapWidget::CreateMapTipsAction( mMainToolsToolBar );
	mMainToolsToolBar->insertAction( after, mActionMapTips );
	mDesktopManager->Map()->ConnectParentMapTipsAction( mActionMapTips );
	mActionMapTips->setChecked( true );


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

#if defined (USE_DATA_VISUALIZATION)
	AddFutureActions( this, menu_Help, mSettings.BratPaths().mExecutableDir, mModel );
#endif

    // Global Wiring
    //
    connect( menu_Tools, SIGNAL(aboutToShow()), this, SLOT(UpdateToolsMenu()) );
    connect( menu_Window, SIGNAL(aboutToShow()), this, SLOT(UpdateWindowMenu()) );
	connect( this, SIGNAL( WorkspaceChanged() ), this, SLOT( WorkspaceChangedUpdateUI() ) );

#if !defined (DEBUG) && !defined(_DEBUG)
	action_Test->setVisible( false );
	action_One_Click->setVisible( false );			//TODO delete line when implemented
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

#if !defined(_DEBUG) && !defined(DEBUG)
	mRenderSuppressionCBox->setVisible( false );
	mRenderSuppressionCBox->setEnabled( false );
#endif

	mDesktopManager->Map()->ConnectParentRenderWidgets( mProgressBar, mRenderSuppressionCBox );

	//coords status bar widget
	CMapWidget::CreateCoordinatesWidget( statusBar(), mCoordsEdit, mCoordinatesFormat );
	statusBar()->addPermanentWidget( mCoordinatesFormat, 0 );
	statusBar()->addPermanentWidget( mCoordsEdit, 0 );
	mDesktopManager->Map()->ConnectCoordinatesWidget( mCoordsEdit, mCoordinatesFormat, "DMS" );

	mMessageButton = new QToolButton( statusBar() );
	mMessageButton->setAutoRaise( true );
	mMessageButton->setIcon( CBratApplication::getThemeIcon( "bubble.svg" ) );
	mMessageButton->setToolTip( tr( "Messages" ) );
	mMessageButton->setWhatsThis( tr( "Messages" ) );
	mMessageButton->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
	mMessageButton->setObjectName( "mMessageButton" );
	mMessageButton->setCheckable( true );
	statusBar()->addPermanentWidget( mMessageButton, 0 );

	connect( mMessageButton, SIGNAL( toggled( bool ) ), mOutputDock, SLOT( setVisible( bool ) ) );
	connect( mOutputDock, SIGNAL( visibilityChanged( bool ) ), this, SLOT( OutputDockVisibilityChanged( bool ) ) );


	LOG_TRACE( "Finished status-bar construction." );
}


void CBratMainWindow::OutputDockVisibilityChanged( bool visible )
{ 
	//We can't use
	//connect( mOutputDock, SIGNAL( visibilityChanged( bool ) ), mMessageButton, SLOT( setChecked( bool ) ) );
	//because visibilityChanged is triggered when window is minimized, not when is restored

	if ( !isMinimized() )
		mMessageButton->setChecked( visible );
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//											Remote Counter
////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static const QUrl counter_management_url( "http://www.altimetry.info/wp-admin/" );
static const QUrl counter_url( "http://www.altimetry.info/test-page/" );


//static 
RemoteCounter *RemoteCounter::smRemoteCounter = nullptr;


#if defined(USE_WEB_ENGINE)

//https://user:password@youtu.be/-Lt0Zka0nII														//not working with any url
//static const QUrl counter_management_url( "https://www.youtube.com/my_videos?o=U" );
//static const QUrl counter_original_url( "https://youtu.be/-Lt0Zka0nII" );							//original: not working as is
//static const QUrl counter_url( "https://www.youtube.com/watch?v=-Lt0Zka0nII&feature=youtu.be" );	//works with unlisted



RemoteCounter::RemoteCounter() 
	: base_t( nullptr )
{
	assert__( smRemoteCounter == nullptr );

	setAttribute( Qt::WA_DeleteOnClose );
	connect( this, &QWebEngineView::loadFinished, [this]( bool success ) {
		if ( !success )
			LOG_WARN( "RemoteCounter failure" );
		QTimer::singleShot( 10000, this, &QWidget::close );
	} );
	setVisible( false );
	smRemoteCounter = this;
}
//virtual 
RemoteCounter::~RemoteCounter()
{
	assert__( smRemoteCounter != nullptr );
	LOG_INFO( "Deleted remote counter." );
	smRemoteCounter = nullptr;
}


RemoteCounterPage::RemoteCounterPage( const std::string &user, const std::string &pass, QWebEngineProfile *profile, QObject *parent )
	: QWebEnginePage(profile, parent)
	, mUser( user )
	, mPass( pass )
{
	connect(this, &QWebEnginePage::authenticationRequired, this, &RemoteCounterPage::handleAuthenticationRequired);
	connect(this, &QWebEnginePage::proxyAuthenticationRequired, this, &RemoteCounterPage::handleProxyAuthenticationRequired);
}
//virtual 
RemoteCounterPage::~RemoteCounterPage()
{
	LOG_INFO( "Deleted remote counter page." );
}

void RemoteCounterPage::FillAuth( QAuthenticator *auth )
{
	auth->setUser( mUser.c_str() );
	auth->setPassword( mPass.c_str() );
}

bool RemoteCounterPage::certificateError( const QWebEngineCertificateError &error )
{
	QWidget *mainWindow = view()->window();
	QMessageBox::critical( mainWindow, tr("Certificate Error"), error.errorDescription() );
	return false;
}

void RemoteCounterPage::handleAuthenticationRequired( const QUrl &, QAuthenticator *auth )
{
	FillAuth( auth );
}

void RemoteCounterPage::handleProxyAuthenticationRequired( const QUrl &, QAuthenticator *auth, const QString & )
{
	FillAuth( auth );
}

void RemoteCounter::RemoteCount()
{
	RemoteCounterPage *webPage = new RemoteCounterPage( "brat.helpdesk@esa.int", "Deimos2015", QWebEngineProfile::defaultProfile(), this );
	setPage( webPage );
	page()->load( counter_url );
}

void RemoteCount()
{
	RemoteCounter *webview = new RemoteCounter;
	webview->RemoteCount();
}


#else



RemoteCounter::RemoteCounter()
{
    assert__( !smRemoteCounter );
	connect( &mManager, SIGNAL( finished(QNetworkReply*) ), SLOT( downloadFinished(QNetworkReply*) ) );
    smRemoteCounter = this;
}

//virtual 
RemoteCounter::~RemoteCounter()
{
	assert__( smRemoteCounter != nullptr );
	LOG_TRACE( "Deleted remote counter." );
	smRemoteCounter = nullptr;
}


void RemoteCounter::Count()
{
	QNetworkRequest request( counter_url );
	//request.setRawHeader( "Authorization", "Basic " + QByteArray( QString("brat:Deimos2015").toLatin1() ).toBase64() );
	QNetworkReply *reply = mManager.get( request );

	connect( reply, SIGNAL( sslErrors(QList<QSslError>) ), SLOT( sslErrors(QList<QSslError>) ) );

	mCurrentReplies.append(reply);
}


void RemoteCounter::sslErrors(const QList<QSslError> &sslErrors)
{
	foreach (const QSslError &error, sslErrors)
	{
		LOG_TRACE( "SSL error: " + error.errorString() );
	}
}


void RemoteCounter::downloadFinished(QNetworkReply *reply)
{
	if ( reply->error() ) 
    {
		LOG_TRACE( "Remote count failed: " +reply->errorString() );
	} 
    else 
    {
//        QFile file(filename);
//        if (!file.open(QIODevice::WriteOnly)) {
//            fprintf(stderr, "Could not open %s for writing: %s\n",
//                qPrintable(filename),
//                qPrintable(file.errorString()));
//            return false;
//        }
//        file.write(
                    reply->readAll();
//                    );
//        file.close();
	}

	mCurrentReplies.removeAll(reply);
	reply->deleteLater();
    deleteLater();
}


void RemoteCount()
{
	static const char *S3ALTB_ROOT = getenv( "S3ALTB_ROOT" );
	static const std::string dev_tag_file =
		S3ALTB_ROOT ?
		std::string( S3ALTB_ROOT ) + "/project/dev/support/dev_tag.txt" :
		"";

	RemoteCounter *rc = new RemoteCounter;

	if ( !IsFile( dev_tag_file ) )
    	rc->Count();
}


#endif




////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//											Constructor
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////


CBratMainWindow::CBratMainWindow( CBratApplication &app ) 

	: QMainWindow( nullptr)

	, mApp( app )

	, mSettings( mApp.Settings() )

	, mModel( mApp.DataModel() )

{
	LOG_TRACE( "Starting main window construction..." );

	assert__( !smInstance );
    assert__( mSettings.BratPaths().IsValid() );

    // Assign singleton just before triggering load operations and leave
    //
    smInstance = this;
    SetApplicationWindow( smInstance );


    mApp.ShowSplash( "Creating main window..." );

	connect( this, SIGNAL( SettingsUpdated() ), &mApp, SLOT( UpdateSettings() ) );

	CMapWidget::SetQGISDirectories(
		mSettings.BratPaths().mQgisPluginsDir,
		mSettings.BratPaths().mVectorLayerPath,
        mSettings.BratPaths().RasterLayerPath(),
        mSettings.BratPaths().URLRasterLayerPath() );

	CGlobeWidget::SetOSGDirectories( mSettings.BratPaths().mGlobeDir );

	CWorkspaceSettings::InitializeCommonData( mSettings.BratPaths(), mApp.RadsServiceSettings() );

    setupUi( this );
    setWindowIcon( QIcon("://images/BratIcon.png") );
    setWindowTitle( MakeWindowTitle( windowTitle() ) );     //set title sensitive to doc (un)modifications
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
		SimpleWarnBox( std::string( "An error occurred while loading Brat configuration: " ) + e.what() );
	}


    // Sub-Windows management
    //
	if ( mSettings.mDesktopManagerSdi )
		mDesktopManager = new CDesktopManagerSDI( mSettings, this );
	else
		mDesktopManager = new CDesktopManagerMDI( mSettings, this );


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
    }


	mApp.ShowSplash( "Loading workspace..." );


    // Load command line or user settings referenced files 
    //
	// Let any exceptions occurred while initializing leak to main and terminate,
	//	unless locally decided otherwise;
	// LeakExceptions is set to false by LoadCmdLineFiles
	//
	mApp.LeakExceptions( true );	
	if ( !mApp.SplashAvailable() )
		QTimer::singleShot( 100, this, SLOT( LoadCmdLineFiles() ) );
	else
		LoadCmdLineFiles();

	mApp.EndSplash( this );


    // RADS service
    //
	if ( mSettings.mDisplayRadsInstallInformation && !mApp.RadsServiceController().isInstalled() )
	{
        static const std::string options_name =
#if defined (Q_OS_MAC)
        "Preferences"
#else
        "Options"
#endif
        ;
		QTimer::singleShot( 0, this, []() {
			SimpleMsgBox( "For proper use and update of RADS datasets, the RADS service should be installed on your system.\n\n\
The service can be installed and configured from the " + options_name + " dialog." );
		}
		);
		mSettings.mDisplayRadsInstallInformation = false;
	}

    mApp.ResetRadsSocketConnection();
    
    
    RemoteCount();


	LOG_TRACE( "Finished main window construction." );
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
		QStringList args = mApp.CleanArgumentsList();
		QString wkspc_dir;
		if ( !args.empty() )
			wkspc_dir = args[ 0 ];

        // parameters for instant pot save
        bool start_in_instant_plot_save_mode = mApp.OperatingInInstantPlotSaveMode();
        std::string instant_display_name, instant_path;
        bool instant_is_3d = false;                                             //defaults to 2D
        CDisplayData::EImageExportType instant_type = CDisplayData::ePng;       //format defaults to png

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
        else    //a workspace was passed in the command line; check remaining arguments for a plot save
        if ( start_in_instant_plot_save_mode )
        {
            start_in_instant_plot_save_mode =
                    CheckInstantPlotSave( args, wkspc_dir, instant_display_name, instant_path, instant_is_3d, instant_type );
        }

		if ( wkspc_dir.isEmpty() )
		{
			LOG_TRACE( "No workspace required, starting without one." );

			DoNoWorkspace();	//no workspace given and the user does not want to LoadLastWorkspaceAtStartUp
		}
		else
		{
			LOG_TRACE( "Opening required workspace: " + wkspc_dir );

			// let any exceptions leak to main
			//	- on failure other than exception, OpenWorkspace makes DoNoWorkspace

			if ( !OpenWorkspace( q2a( wkspc_dir ) ) )
			{
				SimpleWarnBox( "Unable to load the required workspace located in '" + wkspc_dir + "' " );
			}
			else
			{
				//v3: GotoLastPageReached();

				LOG_TRACE( "Selecting last used working panel." );

				mMainWorkingDock->SelectTab( compute_last_page_reached() );
				WorkingPanel<eOperations>()->SetAdvancedMode( mSettings.mAdvancedOperations );

				if ( start_in_instant_plot_save_mode )
                {
					LOG_TRACE( "Starting in instant plot save mode." );

					StartInInstantPlotSaveMode( instant_display_name, instant_path, instant_is_3d, instant_type );
                }
			}
		}
	}

	LOG_TRACE( "Finished command line arguments processing." );
}


// args[0] - workspace directory
// args[1] - display name
// args[2] - full file path; picture format will be taken from file extension
// args[3] - "3" if a 3D image is required;
//
bool CBratMainWindow::CheckInstantPlotSave( const QStringList &args, const QString &wkspc_dir, std::string &display_name,
                                            std::string &path, bool &is_3d, CDisplayData::EImageExportType &type )
{
	//try to infer if the user is attempting to plot without the GUI; failure here is not an error

    if ( !IsDir( wkspc_dir ) )
        return false;

    if ( args.size() < 3 || args.size() > 4 )
        return false;

    display_name = q2a( args[ 1 ] );
    path = q2a( args[ 2 ] );
    is_3d = args.size() == 4 && args[ 3 ] == "3";

	//from here on it is reasonable to assume that the user is attempting to plot without the GUI

	std::string dir = GetDirectoryFromPath( path );
	if ( !IsDir( dir ) )
	{
		LOG_FATAL( "Output directory must exist." );
		return false;
	}

    std::string extension = GetLastExtensionFromPath( path );
    type = CDisplayData::String2ImageType( extension );
	bool valid_type = type != CDisplayData::EImageExportType_size;
	if ( !valid_type )
		LOG_FATAL( "Extension type not supported." );

    return valid_type;
}


void CBratMainWindow::StartInInstantPlotSaveMode( const std::string &display_name, const std::string &path, bool is_3d, CDisplayData::EImageExportType type)
{
    LOG_TRACE( "Starting in instant plot save mode." );

    CDisplay *d = mModel.Workspace< CWorkspaceDisplay >()->GetDisplay( display_name );
    if ( !d )
    {
        LOG_FATAL( "Could not find view '" + display_name + "' in workspace." + mModel.RootWorkspace()->GetName() );
    }
    else
    if ( is_3d && d->IsYFXType() )
    {
        LOG_FATAL( "View '" + display_name + "' does not support 3D format." );
        //qDebug() << "View '" + t2q( display_name ) + "' does not support 3D format.";
    }
    else
    {
        QWidget *w = OpenDisplay( d, false );
        CSubWindow *sub_window = nullptr;
        CAbstractDisplayEditor *ed = nullptr;
        if ( w )
        {
            sub_window = dynamic_cast<CSubWindow*>( w );
            if ( sub_window )
                ed = dynamic_cast<CAbstractDisplayEditor*>( sub_window->Widget() );
        }
        if ( !w || !sub_window || !ed )
        {
            LOG_FATAL( "Could not create view." );
        }
		else
		{
			if ( is_3d )
			{
				ed->Handle3D( true );
				if ( d->IsZLatLonType() )
					mApp.processEvents();
			}

			ed->Export2Image( !is_3d, is_3d, path, path, type );
			//w->setVisible( false );
		}
    }

    LOG_TRACE( "Exiting from n instant plot save mode." );

	mDesktopManager->CloseAllSubWindows();
	//exit( 0 );
    StopDisplayMode();//                    abort();
}


void CBratMainWindow::StopDisplayMode()
{
    assert__( mApp.OperatingInDisplayMode() || mApp.OperatingInInstantPlotSaveMode() );

	CActionInfo::TriggerAction( eAction_Exit );
}


bool CBratMainWindow::StartDisplayMode()
{
    QStringList args = QCoreApplication::arguments();   //TODO Qt and old brat args begin with "--"
	DoNoWorkspace();
	setVisible( false );
	CDisplayFilesProcessor p( false );
	if ( p.Process( args ) )
	{
		std::vector< CAbstractDisplayEditor* > editors;

		connect( mDesktopManager, SIGNAL( AllSubWindowsClosed() ), this, SLOT( StopDisplayMode() ) );

		auto &plots = p.BuildPlots();
		if ( p.isZFLatLon() )		// =================================== WorldPlot()
		{
			for ( auto &plot : plots )
			{
				CGeoPlot* wplot = dynamic_cast<CGeoPlot*>( plot );
				if ( wplot == nullptr )
					continue;

				editors.push_back( new CMapEditor( &p, wplot ) );
			}
		}
		else if ( p.isYFX() )		// =================================== XYPlot();
		{
			for ( auto &plot : plots )
			{
				CYFXPlot* yfxplot = dynamic_cast<CYFXPlot*>( plot );
				if ( yfxplot == nullptr )
					continue;

				editors.push_back( new CPlotEditor( &p, yfxplot ) );
			}
		}
		else if ( p.isZFXY() )		// =================================== ZFXYPlot();
		{
			for ( auto &plot : plots )
			{
				CZFXYPlot* zfxyplot = dynamic_cast<CZFXYPlot*>( plot );
				if ( zfxyplot == nullptr )
					continue;

				editors.push_back( new CPlotEditor( &p, zfxyplot ) );
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
		base_t::tr( "Do you want to save any workspace changes?" ),
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


    if ( mApp.OperatingInDisplayMode() || mApp.OperatingInInstantPlotSaveMode() )
    {
		mDesktopManager->CloseAllSubWindows();
        event->accept();
        return;
    }

	if ( !mProcessesTable->Empty() && !SimpleQuestion( processes_question ) )
	{
		event->ignore();
		return;
	}

	if ( !OkToContinue() )
	{
		event->ignore();
		return;
	}

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
    SetApplicationWindow( nullptr );

	if ( RemoteCounter::smRemoteCounter )
	{
		const QString msg = "RemoteCounter is NOT null";
		qDebug() << msg;
		LOG_TRACE( msg );
	}
}





////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//											Access
////////////////////////////////////////////////////////////////////////////////////////////////////////////////


template< CBratMainWindow::ETabName INDEX >
CBratMainWindow::TabType< INDEX >* CBratMainWindow::WorkingPanel()
{
    return dynamic_cast< TabType< INDEX >* >( mMainWorkingDock->TabWidget( INDEX ) );
}


/////////////////////////////////////////////////////////////////////////
//                      LINKS to GUI
/////////////////////////////////////////////////////////////////////////


void CBratMainWindow::EmitWorkspaceChanged()
{
	emit WorkspaceChanged();

	WorkingPanel< eDataset >()->HandleWorkspaceChanged( mModel.Workspace< CWorkspaceDataset >() );
	WorkingPanel< eRADS >()->HandleWorkspaceChanged( mModel.Workspace< CWorkspaceDataset >() );
	WorkingPanel< eFilter >()->HandleWorkspaceChanged(/* mModel.Workspace< CWorkspaceOperation >() */);
	WorkingPanel< eOperations >()->HandleWorkspaceChanged( /*mModel.Workspace< CWorkspaceDisplay >()*/ );
}


void CBratMainWindow::TabSelected( int index )
{
	std::vector< std::pair < CDesktopControlsPanel*, bool > > v;

	v.push_back( { WorkingPanel< eDataset >(), index == eDataset } );
	v.push_back( { WorkingPanel< eRADS >(), index == eRADS } );
	v.push_back( { WorkingPanel< eFilter >(), index == eFilter } );
	v.push_back( { WorkingPanel< eOperations >(), index == eOperations } );

	for ( auto const &pair : v )
		if ( !pair.second )
			pair.first->ChangePanelSelection( pair.second );

	for ( auto const &pair : v )
		if ( pair.second )
			pair.first->ChangePanelSelection( pair.second );
}



/////////////////////////////////////////////////////////////////////////
//                      WORKSPACE ACTIONS
/////////////////////////////////////////////////////////////////////////

bool CBratMainWindow::DoNoWorkspace()
{
	mModel.Reset();
	mDesktopManager->CloseAllSubWindows();
	setWindowTitle( MakeWindowTitle( QString() ) );
    EmitWorkspaceChanged();
	return true;
}

void CBratMainWindow::SetCurrentWorkspace( const CWorkspace *wks )
{
	assert__( wks );

	mDesktopManager->CloseAllSubWindows();
	EmitWorkspaceChanged();
	auto previous_tab = mMainWorkingDock->SelectedTab();
	mMainWorkingDock->SelectTab( ETabName::eDataset );
	if ( previous_tab == ETabName::eDataset )			//signal not emitted in this case, so, force selection notification
		TabSelected( ETabName::eDataset );

	QString cur_file = t2q( wks->GetPath() );
    QString shownName;

    if ( !wks->GetName().empty() )
    {
        shownName = t2q( wks->GetName() );
        //
        // Most recent files logic
        //
		mRecentFilesProcessor->SetCurrentFile( cur_file );
	}

	setWindowTitle( MakeWindowTitle( shownName ) );

    setWindowModified( false );
}


bool CBratMainWindow::OpenWorkspace( const std::string &path )
{
	WaitCursor wait;

	if ( path.empty() )
	{
		DoNoWorkspace();
	}
    else
    if ( !OkToContinue() )
    {
        return true;
    }
    else
	{
		std::string error_msg;
		CWorkspace *wks = mModel.LoadWorkspace( path, error_msg );
		if ( !error_msg.empty() )
		{
			if ( wks )
				LOG_WARN( error_msg );		//can be only warnings about (missing) filters, recovered file references, etc.
			else
			{
				SimpleErrorBox( error_msg );
				DoNoWorkspace();
				return false;
			}
		}

		SetCurrentWorkspace( wks );
	}

	return true;
}

void CBratMainWindow::on_action_New_triggered()
{
    static QString last_path = t2q( mSettings.BratPaths().WorkspacesDirectory() );

    if ( !OkToContinue() )
    {
        return;
    }

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
    static QString last_path = t2q( mSettings.BratPaths().WorkspacesDirectory() );

	CWorkspaceDialog dlg( this, CTreeWorkspace::eOpen, nullptr, last_path, mModel );
	if ( dlg.exec() == QDialog::Accepted )
	{
		if ( OpenWorkspace( dlg.mPath ) )
			last_path = t2q( GetDirectoryFromPath( dlg.mPath ) );
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
		LOG_INFO( wks->GetName() + " saved." );
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


void CBratMainWindow::on_action_Import_Workspace_triggered()
{
    static QString last_path = t2q( mSettings.BratPaths().WorkspacesDirectory() );

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
    static QString last_path = t2q( mSettings.BratPaths().WorkspacesDirectory() );

	CWorkspace *wks = mModel.RootWorkspace();			assert__( wks != nullptr );

	CWorkspaceDialog dlg( this, CTreeWorkspace::eRename, wks, last_path, mModel );
	if ( dlg.exec() == QDialog::Accepted )
	{
		last_path = t2q( dlg.mPath );

		wks->SetName( dlg.mName );
		QString shownName = t2q( wks->GetName() );
		setWindowTitle( MakeWindowTitle( shownName ) );
	}
}


void CBratMainWindow::on_action_Delete_Workspace_triggered()
{
    static QString last_path = t2q( mSettings.BratPaths().WorkspacesDirectory() );

	CWorkspace *wks = mModel.RootWorkspace();			assert__( wks != nullptr );

	CWorkspaceDialog dlg( this, CTreeWorkspace::eDelete, wks, last_path, mModel );	// TODO is it worth showing the dialog? All widgets are dimmed except the delete
	if ( dlg.exec() == QDialog::Accepted )
	{
		last_path = t2q( dlg.mPath );

		DoNoWorkspace();

		if ( !DeleteDirectory( dlg.mPath ) )
			SimpleWarnBox( "A system error occurred deleting " + dlg.mPath + ".\nPlease check if the workspace directory was fully deleted.");

		mRecentFilesProcessor->DeleteEntry( dlg.mPath.c_str() );

		WorkingPanel< ETabName::eFilter >()->DrawDatasetTracks( nullptr, false );
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
	mDesktopManager->Map()->ZoomIn();
}


void CBratMainWindow::on_action_Zoom_Out_triggered()
{
	mDesktopManager->Map()->ZoomOut();
}


void CBratMainWindow::on_action_Re_center_triggered()
{
	mDesktopManager->Map()->zoomToFullExtent();
}


void CBratMainWindow::on_action_Refresh_Map_triggered()
{
	mDesktopManager->Map()->refresh();
}


void CBratMainWindow::on_action_Satellite_Tracks_toggled( bool checked )
{
	WorkingPanel< eFilter >()->SetAutoDrawSatelliteTrack( checked );	//could be any tab panel
}

void CBratMainWindow::on_action_Save_Map_Image_triggered()
{
	static std::string path2d = mSettings.BratPaths().UserDataDirectory() + "/BratMainMap_2D";		//extension is added by dialog
	static std::string path3d = mSettings.BratPaths().UserDataDirectory() + "/BratMainMap_3D";		//NOT used

    CExportImageDialog dlg( true, false, path2d, path3d, this );
    if ( dlg.exec() == QDialog::Accepted )
    {
		path2d = dlg.OutputFileName2D();		assert__( dlg.Save2D() );

		QString extension = CDisplayData::ImageType2String( dlg.OutputFileType() ).c_str();
		QString format = dlg.OutputFileType() == CDisplayData::ePnm ? "ppm" : extension;

		if ( !mDesktopManager->Map()->Save2Image( path2d.c_str(), format, extension ) )
			SimpleWarnBox( "There was an error saving file " + path2d );
    }
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
	BRAT_NOT_IMPLEMENTED
}


void CBratMainWindow::on_action_Workspace_Tree_triggered()
{
	CWorkspaceElementsDialog dlg( this, mModel.Tree() );
	dlg.exec();
}


QWidget* CBratMainWindow::OpenDisplay( CDisplay *display, bool maps_as_plots )
{
	return mDesktopManager->AddSubWindow( 
		
		[this, &maps_as_plots, &display]() -> QWidget*
			{
				auto v = display->GetOperations();					assert__( v.size() > 0 );

				if ( maps_as_plots || !display->IsZLatLonType() )
				{
					return new plot_editor_t( &mModel, v[ 0 ], display->GetName() );
				}
				else
				{
					return new CMapEditor( &mModel, v[ 0 ], display->GetName() );
				}
			}		
		);
}


void CBratMainWindow::on_action_Operation_Views_triggered()
{
	COperation *operation = WorkingPanel< eOperations >()->CurrentOperation();
	if ( !operation )
		operation = WorkingPanel< eOperations >()->QuickOperation();

	COperationViewsDialog dlg( this, *operation, mModel );

	if ( dlg.exec() == QDialog::Accepted )
	{
		auto *display = dlg.SelectedDisplay();
		if ( !display )
			return;

		OpenDisplay( display, dlg.DisplayMapsAsPlots() );
	}
}


void CBratMainWindow::on_action_Launch_Scheduler_triggered()
{
	WorkingPanel<eOperations>()->LaunchScheduler();
}


void CBratMainWindow::on_action_Options_triggered()
{
    CApplicationSettingsDlg dlg( mApp, this );
	if ( dlg.exec() == QDialog::Accepted )
		emit SettingsUpdated();
}


void CBratMainWindow::UpdateToolsMenu()
{
	COperation *operation = WorkingPanel< eOperations >()->CurrentOperation();
	if ( !operation )
		operation = WorkingPanel< eOperations >()->QuickOperation();

	action_Operation_Views->setEnabled( operation && mModel.OperationDisplays( operation->GetName() ).size() > 0 );
}




/////////////////////////////////////////////////////////////////////////
//                      WINDOW ACTIONS
/////////////////////////////////////////////////////////////////////////



void CBratMainWindow::on_action_Views_List_triggered()
{
	CActiveViewsDialog dlg( this, mDesktopManager );
	dlg.exec();
}


void CBratMainWindow::on_action_Open_View_triggered()
{
	CWorkspaceViewsDialog dlg( this, mModel );

	if ( dlg.exec() == QDialog::Accepted )
	{
		auto *display = dlg.SelectedDisplay();				assert__( display );
		OpenDisplay( display, dlg.DisplayMapsAsPlots() );
	}
}


//////////////////////////////////////////
//NOTE: Close All connected in constructor
//////////////////////////////////////////


void CBratMainWindow::UpdateWindowMenu()
{
	bool has_sub_windows = !mDesktopManager->SubWindowList().empty();
	action_Close_All->setEnabled( has_sub_windows );
	action_Views_List->setEnabled( has_sub_windows );
	action_Open_View->setEnabled( 
		mModel.Workspace< CWorkspaceDisplay >() &&
		mModel.Workspace< CWorkspaceDisplay >()->GetDisplays() && 
		mModel.Workspace< CWorkspaceDisplay >()->GetDisplays()->size() > 0 );
}



/////////////////////////////////////////////////////////////////////////
//                      HELP ACTIONS
/////////////////////////////////////////////////////////////////////////


void CBratMainWindow::on_action_About_triggered()
{
	SimpleAboutBoxWithBuildDate( BRAT_VERSION_STRING, PROCESSOR_ARCH, "CNES/ESA" );
}


void CBratMainWindow::on_action_User_s_Manual_triggered()
{
	if ( !SimpleSystemOpenFile( mSettings.BratPaths().mUserManualPath ) )
		SimpleErrorBox( "Could not open " + mSettings.BratPaths().mUserManualPath.mPath );
}


void CBratMainWindow::on_action_Youtube_Video_Tutorials_triggered()
{
    static const std::string s = "https://www.youtube.com/playlist?list=PLWLKr3OylZcHbPKlAsmRLQp6aqAq2NChr";
	static const bool is_xp = QSysInfo::windowsVersion() == QSysInfo::WV_XP || QSysInfo::windowsVersion() == QSysInfo::WV_2003;	//WV_2003: xp x64
	static const std::string default_firefox_location =

	//Trying defaults of Firefox installation in XP

#if defined(BRAT_ARCHITECTURE_64)
		"C:\\Program Files (x86)\\Mozilla Firefox\\firefox.exe";
#else
		"C:\\Program Files\\Mozilla Firefox\\firefox.exe";
#endif

	if ( is_xp && IsFile( default_firefox_location ) )
	{
		system( ( "\"" + default_firefox_location + "\" " + s ).c_str() );
	}
	else
	{
		if ( !SimpleSystemOpenURL( s ) )
			SimpleErrorBox( "Could not open " + s );
	}
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TEST CODE SECTION ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined (DEBUG) || defined(_DEBUG)
#include "BratMainWindowDevTests.inc"
#endif

void CBratMainWindow::on_action_Test_triggered()
{
#if defined (DEBUG) || defined(_DEBUG)
	TestPyAlgo();
#endif
}
// TEST CODE SECTION ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





/////////////////////////////////////////////////////////////////////////
//                      STATUS-BAR ACTIONS
/////////////////////////////////////////////////////////////////////////





/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
//							UPDATE STATE
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////




void CBratMainWindow::WorkspaceChangedUpdateUI()
{
	const bool enable = mModel.RootWorkspace() != nullptr;

	CActionInfo::UpdateActionsState( {

		eAction_Save,
		eAction_Import_Workspace,
		eAction_Rename_Workspace,
		eAction_Delete_Workspace,

		eAction_One_Click,
		eAction_Workspace_Tree,

		eAction_Operation_Views,

		eAction_Views_List,
		eAction_Close_All,

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


void CBratMainWindow::HandleAsyncProcessExecution( bool executing )
{
	if ( executing )
	{
		mOutputDock->SelectTab( mLogFrameIndex );
		mOutputDock->setVisible( true );
	}
}


void CBratMainWindow::HandleSyncProcessExecution( bool executing )
{
	CActionInfo::UpdateActionsState( {

		eAction_Open,
		eAction_New,
		eAction_CloseWorkspace,
		eAction_Save,
		eAction_Import_Workspace,
		eAction_Rename_Workspace,
		eAction_Delete_Workspace,

		eAction_One_Click,
		eAction_Workspace_Tree,
		eAction_Operation_Views,
		eAction_Options,

		eAction_Views_List,
		eAction_Close_All,

	}, !executing );


	mRecentFilesProcessor->SetEnabled( !executing );
	EnableMapSelectionActions( !executing );
	//mDesktopManager->Map()->EnableMapMeasureActions( !executing );
	mMainWorkingDock->setEnabled( !executing );
	mDesktopManager->setEnabled( !executing );

	if ( executing )
	{
		mOutputDock->SelectTab( mLogFrameIndex );
		mOutputDock->setVisible( true );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_BratMainWindow.cpp"
