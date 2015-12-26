#include "stdafx.h"

#include <cassert>

#include <qtimer.h>

#include "new-gui/Common/ccore-types.h"
#include "new-gui/Common/QtUtils.h"

#include "Workspaces/Workspace.h"

#include "BratApplication.h"
#include "new-gui/brat/Views/TextWidget.h"
#include "new-gui/brat/GUI/TabbedDock.h"
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


CBratMainWindow::CBratMainWindow( QWidget *parent ) : QMainWindow( parent )
{
    setupUi( this );
	setMinimumSize( min_main_window_width, min_main_window_height );    //mdiArea->setViewMode(QMdiArea::SubWindowView);
    setWindowIcon( QIcon("://images/BratIcon.png") );
    setWindowTitle( makeWindowTitle( windowTitle() ) );     //set title sensitive to doc (un)modifications

	CreateDocks();
	mManager = new desktop_manager_t( this );

    // Menu File
    //
    menu_File->insertAction( action_Print, action_Close );    //replicate menu item here: it is handy...
    menu_File->insertSeparator( action_Print );

    // Menu File / Most recent files logic
    //
	// (...)

    setupPrintActions();

    // Aplication Settings
    //
    readSettings();

    #ifndef QT_NO_CLIPBOARD
    connect( QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(clipboardDataChanged()) );
    #endif

    // Menu View / ToolBars
    //
    menu_View->addAction( mMainToolBar->toggleViewAction() );
    menu_View->addAction( mMainToolsToolBar->toggleViewAction() );

    // Menu View / Docks editors (see also the toolbars section in this ctor)
    //
    CTextWidget *editors[] = { mOutputTextWidget, mToolTextWidget };
    QDockWidget *docks[] = { mOutputDock, mMainWorkingDock };
    size_t size = sizeof(editors) / sizeof(*editors);
    menu_View->addSeparator();
    for ( size_t i = 0; i < size; ++i )
    {
        editors[i]->setToolEditor( true );
        //connectAutoUpdateEditActions( editors[i] );
        menu_View->addAction( docks[i]->toggleViewAction() );
    }

    // Menu Tools
    //
    //connect( menu_Tools, SIGNAL(aboutToShow()), this, SLOT(UpdateToolsActions()) );

    // Status Bar
    //
	FillStatusBar();

    // Load CmdLine Files
    //
    //QTimer::singleShot( 0, this, SLOT( LoadCmdLineFiles() ) );
}


void CBratMainWindow::CreateWorkingDock()
{
	mMainWorkingDock = new CTabbedDock( "Main Working Dock", this );
	mMainWorkingDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
	mMainWorkingDock->setMaximumWidth( max_main_dock_width );
	addDockWidget( Qt::LeftDockWidgetArea, mMainWorkingDock );

	// sample widgets: to delete

	mToolTextWidget = new CTextWidget;
	mToolTextWidget->setObjectName( QString::fromUtf8( "mToolTextWidget" ) );

	QTreeWidget *Output_treeWidget_3 = new QTreeWidget;
	QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
	__qtreewidgetitem->setText( 0, QString::fromUtf8( "1" ) );
	Output_treeWidget_3->setHeaderItem( __qtreewidgetitem );
	Output_treeWidget_3->setObjectName( QString::fromUtf8( "Output_treeWidget_3" ) );

	// sample adding docked tabs content: to delete

	auto 
	tab = mMainWorkingDock->AddTab( mToolTextWidget, "Browse" );
	mMainWorkingDock->SetTabToolTip( tab, "Dataset Browser" );
	tab = mMainWorkingDock->AddTab( Output_treeWidget_3, "Filter" );
	mMainWorkingDock->SetTabToolTip( tab, "Dataset Filter" );
	tab = mMainWorkingDock->AddTab( new CTextWidget, "Quick" );
	mMainWorkingDock->SetTabToolTip( tab, "Quick Operations"  );
	tab = mMainWorkingDock->AddTab( new CTextWidget, "Advanced" );
	mMainWorkingDock->SetTabToolTip( tab, "Advanced Operations" );
}
void CBratMainWindow::CreateOutputDock()
{
	mOutputDock = new CTabbedDock( "Output", this );
	mOutputDock->setAllowedAreas( Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea );
	mOutputDock->setMaximumHeight( max_out_window_height );

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
    // Docks (see also the Docks editors section in this ctor)
    //
    setDockOptions( dockOptions() | VerticalTabs );     //not set in designer

    setCorner( Qt::TopLeftCorner, Qt::LeftDockWidgetArea );
    setCorner( Qt::TopRightCorner, Qt::RightDockWidgetArea );
    setCorner( Qt::BottomRightCorner, Qt::RightDockWidgetArea );
    setCorner( Qt::BottomLeftCorner, Qt::LeftDockWidgetArea );		//Qt::BottomDockWidgetArea

	CreateWorkingDock();
	CreateOutputDock();
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


//virtual 
CBratMainWindow::~CBratMainWindow()
{
}


void CBratMainWindow::readSettings()
{
	// If not reading geometry from settings file:
	CenterOnScreen( this );

//    static QSettings& settings = m_bs.GetAppSettings();
//
//    settings.beginGroup( "MainWindow" );
//
//    restoreGeometry( settings.value("geometry").toByteArray() );
//    restoreState( settings.value("state").toByteArray() );
//
//    m_recentFiles = settings.value("recentFiles").toStringList();
//    updateRecentFileActions();
//
////    bool showGrid = settings.value("showGrid", true).toBool();
////    showGridAction->setChecked(showGrid);
//
////    bool autoRecalc = settings.value("autoRecalc", true).toBool();
////    autoRecalcAction->setChecked(autoRecalc);
//
//    settings.endGroup();
}


void CBratMainWindow::writeSettings()
{
//    static QSettings& settings = m_bs.GetAppSettings();
//
//    settings.beginGroup( "MainWindow" );
//    settings.setValue( "geometry", saveGeometry() );
//    settings.setValue( "state", saveState() );
//
//    settings.setValue( "recentFiles", m_recentFiles );
//
////    settings.setValue("showGrid", showGridAction->isChecked());
////    settings.setValue("autoRecalc", autoRecalcAction->isChecked());
//    settings.endGroup();
}


/////////////////////////////////////////////////////////////////////////
//                      OPEN/ INPUT ACTIONS
/////////////////////////////////////////////////////////////////////////

void CBratMainWindow::on_action_New_triggered()
{
	NOT_IMPLEMENTED
}

void CBratMainWindow::XYPlot()
{
	/*
	CObArray::iterator itGroup;

	wxSize size = wxDefaultSize;
	wxPoint pos = wxDefaultPosition;

	for ( itGroup = m_plots.begin(); itGroup != m_plots.end(); itGroup++ )
	{
		CPlot* plot = dynamic_cast<CPlot*>( *itGroup );
		if ( plot == nullptr )
		{
			continue;
		}

		CWindowHandler::GetSizeAndPosition( size, pos );

		CreateXYPlot( plot, size, pos );
	}
	*/
}
void CBratMainWindow::ZFXYPlot()
{
	/*
	CObArray::iterator itGroup;

	wxSize size = wxDefaultSize;
	wxPoint pos = wxDefaultPosition;

	for ( itGroup = m_plots.begin(); itGroup != m_plots.end(); itGroup++ )
	{
		CZFXYPlot* zfxyplot = dynamic_cast<CZFXYPlot*>( *itGroup );
		if ( zfxyplot == nullptr )
		{
			continue;
		}

		CWindowHandler::GetSizeAndPosition( size, pos );

		CreateZFXYPlot( zfxyplot, size, pos );
	}
	*/
}


void CBratMainWindow::CreateTree( CWorkspace* root, CTreeWorkspace& tree )
{
	tree.DeleteTree();

	// Set tree root
	tree.SetRoot( root->GetName(), root, true );

	//WARNING : the sequence of workspaces object creation is significant, because of the interdependence of them

	std::string 

	//FIRSTLY - Create "Datasets" branch
	path = root->GetPath() + "/" + CWorkspaceDataset::NAME;
	CWorkspaceDataset* wksDataSet = new CWorkspaceDataset( CWorkspaceDataset::NAME, path );
	tree.AddChild( wksDataSet->GetName(), wksDataSet );

	//SECOND Create "Formulas" branch
	path = root->GetPath() + "/" + CWorkspaceFormula::NAME;
	std::string errorMsg;
	CWorkspaceFormula* wksFormula = new CWorkspaceFormula( errorMsg, CWorkspaceFormula::NAME, path );
	if ( !errorMsg.empty() )
		SimpleWarnBox( errorMsg );
	tree.AddChild( wksFormula->GetName(), wksFormula );

	//THIRDLY - Create "Operations" branch
	path = root->GetPath() + "/" + CWorkspaceOperation::NAME;
	CWorkspaceOperation* wksOperation = new CWorkspaceOperation( CWorkspaceOperation::NAME, path );
	tree.AddChild( wksOperation->GetName(), wksOperation );

	//FOURTHLY -  Create "Displays" branch
	path = root->GetPath() + "/" + CWorkspaceDisplay::NAME;
	CWorkspaceDisplay* wksDisplay = new CWorkspaceDisplay( CWorkspaceDisplay::NAME, path );
	tree.AddChild( wksDisplay->GetName(), wksDisplay );
}

template< class WKSPC >
WKSPC* CBratMainWindow::GetCurrentWorkspace()
{
	if ( mCurrentTree == nullptr )
		mCurrentTree = &mTree;

	CWorkspace* wks = GetCurrentRootWorkspace();
	std::string workspaceKey = !wks ? "" : ( wks->GetKey() + CWorkspace::m_keyDelimiter + WKSPC::NAME );

	return dynamic_cast< WKSPC* >( mCurrentTree->FindObject( workspaceKey ) );
}

CWorkspace* CBratMainWindow::GetCurrentRootWorkspace()
{
	if ( mCurrentTree == nullptr )
		mCurrentTree = &mTree;

	return mCurrentTree->GetRootData();
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
void CBratMainWindow::AddWorkspaceToHistory( const std::string& name )
{
    UNUSED( name );

    //m_menuBar->Enable( ID_MENU_FILE_RECENT, true );
	//m_wksHistory.AddFileToHistory( name );
}

void CBratMainWindow::SetTitle( CWorkspace *wks )	//= nullptr 
{
	if ( !wks )
		wks = GetCurrentRootWorkspace();

	setWindowTitle( makeWindowTitle( t2q( wks ? wks->GetName() : "" ) ) );
}

void CBratMainWindow::DoEmptyWorkspace()
{
	mTree.DeleteTree();
	ResetWorkspace();
	EnableCtrlWorkspace();
	SetTitle();
}

bool CBratMainWindow::OpenWorkspace( const QString &qpath )
{
	auto path = q2a( qpath );
	CWorkspace* wks = new CWorkspace( GetFilenameFromPath( path ), path );

	CreateTree( wks );
	CWorkspace *failed_wks = nullptr;
	std::string errorMsg;
	if ( !mTree.LoadConfig( failed_wks, 
		GetCurrentWorkspace<CWorkspaceDataset>(), 
		GetCurrentWorkspace<CWorkspaceOperation>(), 
		GetCurrentWorkspace<CWorkspaceDisplay>(), errorMsg ) )
	{
		SimpleWarnBox( errorMsg + "\nUnable to load workspace '" + ( failed_wks ? failed_wks->GetName() : "" ) + "'." );
		DoEmptyWorkspace();
		return false;
	}

	ResetWorkspace();

	LoadWorkspace();

	EnableCtrlWorkspace();

	if ( wks )
		AddWorkspaceToHistory( wks->GetPath() );

	SetTitle( wks );
    //CTextWidget *editor = CTextWidget::OpenWorkspace( fileName, this );
    //if ( editor )
    //    addEditor( editor );

	return true;
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


///////////////////////////////////////////////////////////////////////////////////////////
//                                  TOOLS->OPTIONS ACTIONS
///////////////////////////////////////////////////////////////////////////////////////////

void CBratMainWindow::on_action_Options_triggered()
{
	NOT_IMPLEMENTED
    //SettingsDlg dlg( this );
    //dlg.exec();
}


/////////////////////////////////////////////////////////////////////////
//                      CLOSE / OUTPUT ACTIONS
/////////////////////////////////////////////////////////////////////////


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
		writeSettings();
		event->accept();
	}
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



///////////////////////////////////////////////////////////////////////////////////////////
//                                  HELP ACTIONS
///////////////////////////////////////////////////////////////////////////////////////////


void CBratMainWindow::on_action_About_triggered()
{
    QMessageBox::about(this, tr("About BRAT"), QString( "Welcome to BRAT " ) + BRAT_VERSION + " - " + PROCESSOR_ARCH + "\n(C)opyright CNES/ESA" );
}


///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_BratMainWindow.cpp"
