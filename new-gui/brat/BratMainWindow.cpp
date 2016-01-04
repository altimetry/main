#include "stdafx.h"

#include <cassert>

#include <qtimer.h>

#include "new-gui/Common/ccore-types.h"
#include "new-gui/Common/QtUtils.h"
#include "new-gui/Common/ConfigurationKeywords.h"

#include "BratApplication.h"
#include "new-gui/brat/Views/TextWidget.h"
#include "new-gui/brat/GUI/TabbedDock.h"
#include "new-gui/brat/GUI/ControlsPanel.h"
#include "new-gui/brat/GUI/ActionsTable.h"

#include "BratMainWindow.h"




#if defined(BRAT_ARCHITECTURE_64)
    static const QString build_platform( " 64-bit" );
#else
	static const QString build_platform( " 32-bit" );
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



//    "    The window title must contain a "[*]" placeholder, which
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
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////


CBratMainWindow::CBratMainWindow( CApplicationSettings &settings ) 
	: QMainWindow( nullptr)
	, mSettings( settings )
{
	assert__( !smInstance );

    setupUi( this );
    setWindowIcon( QIcon("://images/BratIcon.png") );
    setWindowTitle( makeWindowTitle( windowTitle() ) );     //set title sensitive to doc (un)modifications

	mManager = new desktop_manager_t( this );
	CreateDocks();
	
#if !defined (DEBUG) && !defined(_DEBUG)
	action_Test->setVisible( false );
#endif


    // Menu File
    //
	connect( action_Close_All, SIGNAL(triggered()), mManager, SLOT(closeAllSubWindows()) );

    // Menu File / Most recent files logic
    //
	mRecentFilesSeparatorAction = menu_File->insertSeparator( action_Exit );

	mRecentFilesProcessor = new CRecentFilesProcessor( this, "Recent WorkSpaces", menu_File, action_Exit, GROUP_WKS_RECENT.c_str() );
	connect( mRecentFilesProcessor, SIGNAL( triggered( QAction* ) ), this, SLOT( openRecentWorkspace_triggered( QAction* ) ) );

	mRecentFilesSeparatorAction = menu_File->insertSeparator( action_Exit );


    // Application Settings
    //
	bool lastSessionGeometry = false;
	try {
		ReadSettings( lastSessionGeometry );
	}
	catch ( const CException &e )			// TODO: rather stupid catching the exception here...
	{
		SimpleWarnBox( std::string( "An error occurred while loading Brat configuration (CBratGui::LoadConfig)\nNative std::exception: " )
			+ e.what() );
	}

    // Menu View / ToolBars
    //
	if ( !menu_View->isEmpty() )
		menu_View->addSeparator();
    menu_View->addAction( mMainToolBar->toggleViewAction() );
    menu_View->addAction( mMainToolsToolBar->toggleViewAction() );

    // Menu View / Docks editors (see also the toolbars section in this ctor)
    //
    CTextWidget *editors[] = { mOutputTextWidget };
    QDockWidget *docks[] = { mOutputDock, mMainWorkingDock };
    size_t size = sizeof(editors) / sizeof(*editors);
    menu_View->addSeparator();
    for ( size_t i = 0; i < size; ++i )
    {
        editors[i]->setToolEditor( true );
        //connectAutoUpdateEditActions( editors[i] );
        menu_View->addAction( docks[i]->toggleViewAction() );
    }


    // Global Wiring
    //
    connect( menu_Window, SIGNAL(aboutToShow()), this, SLOT(UpdateWindowMenu()) );

    #ifndef QT_NO_CLIPBOARD
    connect( QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(clipboardDataChanged()) );
    #endif


    // Status Bar
    //
	FillStatusBar();


// Remaining old brat initialization tasks, in app ctor /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// load configuration : done above

	//////////////////////////////////////
	//		Load Last Workspace
	//////////////////////////////////////

	//* femm
	//if (m_lastWksPath.empty() == false)
	//  {
	try {
		if ( !OpenWorkspace( mSettings.m_lastWksPath.c_str() ) )
		{
			SimpleWarnBox( "Unable to load last used workspace located in '" + mSettings.m_lastWksPath + "' " );
		}
	}
	catch ( const CException &e ) 
	{
		SimpleWarnBox( e.what() );
	}


	//////////////////////////////////////
	//	Open last tab of previous session 
	//////////////////////////////////////

	//GotoLastPageReached();
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Position
    //
	if ( !lastSessionGeometry )
	{
		//mMainWorkingDock->adjustSize();
		//CenterOnScreen( this );		//calls adjustSize
		show();
		adjustSize();
		CenterOnScreen2( this );
	}

    // Load CmdLine Files
    //
    //QTimer::singleShot( 0, this, SLOT( LoadCmdLineFiles() ) );

	smInstance = this;
}


void CBratMainWindow::CreateWorkingDock()
{
	mMainWorkingDock = new CTabbedDock( "Main Working Dock", this );
	mMainWorkingDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
	//mMainWorkingDock->setMaximumWidth( max_main_dock_width );
	addDockWidget( Qt::LeftDockWidgetArea, mMainWorkingDock, Qt::Vertical );

	auto 
	tab = mMainWorkingDock->AddTab( new CDatasetBrowserControls( mManager ), "Dataset" );
	mMainWorkingDock->SetTabToolTip( tab, "Dataset browser" );
	tab = mMainWorkingDock->AddTab( new CDatasetFilterControls( mManager ), "Filter" );
	mMainWorkingDock->SetTabToolTip( tab, "Dataset filter" );
	tab = mMainWorkingDock->AddTab( new COperationsControls( mManager ), "Operations" );
	mMainWorkingDock->SetTabToolTip( tab, "Quick or advanced operations"  );
}
void CBratMainWindow::CreateOutputDock()
{
	mOutputDock = new CTabbedDock( "Output", this );
	mOutputDock->setAllowedAreas( Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea );
	//mOutputDock->setMaximumHeight( max_out_window_height );
	addDockWidget( Qt::BottomDockWidgetArea, mOutputDock );

	// sample widgets: to delete

	mOutputTextWidget = new CTextWidget;
	mOutputTextWidget->setObjectName( QString::fromUtf8( "mOutputTextWidget" ) );

	QTreeWidget *Output_treeWidget_3 = new QTreeWidget;
	QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
	__qtreewidgetitem->setText( 0, QString::fromUtf8( "1" ) );
	Output_treeWidget_3->setHeaderItem( __qtreewidgetitem );
	Output_treeWidget_3->setObjectName( QString::fromUtf8( "Output_treeWidget_3" ) );

	QFrame *Log_frame_3 = new QFrame;
	Log_frame_3->setObjectName( QString::fromUtf8( "Log_frame_3" ) );
	Log_frame_3->setStyleSheet( QString::fromUtf8( "" ) );
	Log_frame_3->setFrameShape( QFrame::StyledPanel );
	Log_frame_3->setFrameShadow( QFrame::Raised );

	// sample adding docked tabs content: to delete

	mOutputDock->AddTab( mOutputTextWidget, "Main" );
	mOutputDock->AddTab( Output_treeWidget_3, "Tree" );
	mOutputDock->AddTab( Log_frame_3, "Log" );
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

	//mMainWorkingDock->adjustSize();
	//mOutputDock->adjustSize();
}


void CBratMainWindow::FillStatusBar()
{
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
}


void CBratMainWindow::LoadCmdLineFiles()
{
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
            OpenWorkspace(arg);

    //InitialUpdateActions();
}


bool CBratMainWindow::ReadSettings( bool &has_gui_settings )
{
	if ( !mSettings.LoadConfig() || !mRecentFilesProcessor->ReadSettings() )
	{
		CException e( "Error reading the configuration file", BRATHL_ERROR );
		throw e;
	}

	CAppSection section( GROUP_MAIN_WINDOW );
	has_gui_settings = !section.Keys().empty();

	if ( has_gui_settings )
	{
		bool wasMaximized = false;
		QByteArray geom, state;

		if ( AppSettingsReadSection( GROUP_MAIN_WINDOW,

			k_v( KEY_MAIN_WINDOW_GEOMETRY, &geom ),
			k_v( KEY_MAIN_WINDOW_STATE, &state ),
			k_v( KEY_MAIN_WINDOW_MAXIMIZED, &wasMaximized )
		) )
		{
			restoreGeometry( geom );
			restoreState( state );
			if ( wasMaximized )
				setWindowState( windowState() | Qt::WindowMaximized );
		}

		mMainWorkingDock->ReadSettings( GROUP_MAIN_WINDOW );
	}

	return mSettings.Status() == QSettings::NoError;
}

bool CBratMainWindow::WriteSettings()
{
	assert__( !destroyed() );

	if (
		! mSettings.SaveConfig( GetCurrentRootWorkspace() ) 
		||
		! mRecentFilesProcessor->WriteSettings() 
		||
		! AppSettingsWriteSection( GROUP_MAIN_WINDOW,
		
			k_v( KEY_MAIN_WINDOW_GEOMETRY, saveGeometry() ),
			k_v( KEY_MAIN_WINDOW_STATE, saveState() ),
			k_v( KEY_MAIN_WINDOW_MAXIMIZED, isMaximized() )
		) 
		||
		! mMainWorkingDock->WriteSettings( GROUP_MAIN_WINDOW )
		)
		std::cout << "Unable to save BRAT the application settings." << std::endl;	// TODO: log this

	mSettings.Sync();

	return mSettings.Status() == QSettings::NoError;
}


void CBratMainWindow::closeEvent( QCloseEvent *event )
{
	//auto list = mdiArea->subWindowList();
	//auto const size = list.size();
	//for ( int j = 0; j < size; j++ )
	//{
	//	CEditorBase *pe = dynamic_cast<CEditorBase*>( list[ j ]->widget() );
	//	if ( !pe->okToContinue() )
	//	{
	//		event->ignore();
	//		return;
	//	}
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
//                      WORKSPACE ACTIONS
/////////////////////////////////////////////////////////////////////////

void CBratMainWindow::on_action_New_triggered()
{
	NOT_IMPLEMENTED
}


void CBratMainWindow::on_action_Open_triggered()
{
    static QString lastPath;	// TODO: defaults from directories processor

    QString path = BrowseDirectory( this, "Select Workspace", lastPath );
	if ( !path.isEmpty() )
	{
		lastPath = QFileInfo( path ).absoluteDir().path();
		OpenWorkspace( path );
	}
    //CTextWidget *editor = CTextWidget::open(this);
    //if (editor)
    //    addEditor(editor);
}


void CBratMainWindow::on_action_Save_triggered()
{
	NOT_IMPLEMENTED
    //if (activeTextEditor())
    //    activeTextEditor()->save();
}


void CBratMainWindow::on_action_Save_As_triggered()
{
    NOT_IMPLEMENTED
    //if (activeTextEditor())
    //    activeTextEditor()->saveAs();
}


void CBratMainWindow::openRecentWorkspace_triggered( QAction *action )
{
    Q_UNUSED( action )

    NOT_IMPLEMENTED

	//openRecentEditorFile< TextEditor >( action );
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


void CBratMainWindow::on_action_Zom_Out_triggered()
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
	NOT_IMPLEMENTED
}


void CBratMainWindow::on_action_Launch_Scheduler_triggered()
{
	NOT_IMPLEMENTED
}


void CBratMainWindow::on_action_Options_triggered()
{
	NOT_IMPLEMENTED
    //SettingsDlg dlg( this );
    //dlg.exec();
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


void CBratMainWindow::on_action_Test_triggered()
{
#if defined (DEBUG) || defined(_DEBUG)
	NOT_IMPLEMENTED
#endif
}




///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_BratMainWindow.cpp"
