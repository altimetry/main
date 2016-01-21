#include "stdafx.h"

#include <cassert>

#include <qtimer.h>

#include "new-gui/Common/ccore-types.h"
#include "new-gui/Common/QtUtils.h"
#include "new-gui/Common/ConfigurationKeywords.h"

#include "BratApplication.h"
#include "ApplicationSettingsDlg.h"
#include "new-gui/brat/Views/TextWidget.h"
#include "new-gui/brat/GUI/TabbedDock.h"
#include "new-gui/brat/GUI/ControlsPanel.h"
#include "new-gui/brat/GUI/ActionsTable.h"
#include "new-gui/brat/Workspaces/Workspace.h"

#include "new-gui/brat/GUI/WorkspaceDialog.h"
#include "new-gui/brat/GUI/WorkspaceElementsDialog.h"

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
//											Construction Helpers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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

	mSettings.m_lastPageReached = n2s< std::string >( eDataset );
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
}


void CBratMainWindow::ProcessMenu()
{
    // Fill Actions Table
    //
	const QList< QMenu* > lst = mMainMenuBar->findChildren< QMenu* >();
	foreach( QMenu *m, lst )
	{
		qDebug() << m->title();
		foreach( QAction *a, m->actions() )
		{
			auto tag = CActionInfo::UpdateActionProperties( a );		assert__( tag < EActionTag::EActionTags_size );
			qDebug() << a->text() << tag;
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

    // Menu View / ToolBars
    //
	if ( !menu_View->isEmpty() )
		menu_View->addSeparator();
    menu_View->addAction( mMainToolBar->toggleViewAction() );
    menu_View->addAction( mMainToolsToolBar->toggleViewAction() );

    // Menu View / Docks editors (see also the tool-bars section in this ctor)
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
	connect( this, SIGNAL( WorkspaceChanged( const CModel* ) ), this, SLOT( UpdateWorkspaceUI( const CModel* ) ) );

#ifndef QT_NO_CLIPBOARD
    connect( QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(clipboardDataChanged()) );
#endif

#if !defined (DEBUG) && !defined(_DEBUG)
	action_Test->setVisible( false );
#endif

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



////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//											Constructor
////////////////////////////////////////////////////////////////////////////////////////////////////////////////


CBratMainWindow::CBratMainWindow( CApplicationSettings &settings ) 
	: QMainWindow( nullptr)
	, mSettings( settings )
{
	assert__( !smInstance );

    setupUi( this );
    setWindowIcon( QIcon("://images/BratIcon.png") );
    setWindowTitle( makeWindowTitle( windowTitle() ) );     //set title sensitive to doc (un)modifications
    mMainToolBar->setIconSize({icon_size,icon_size});
    mMainToolsToolBar->setIconSize({icon_size,icon_size});

    // Sub-Windows management
    //
	mManager = new desktop_manager_t( this );


    // Docked controls
    //
	CreateDocks();


    // Menu commands
    //
	ProcessMenu();


    // Settings from persistence
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


    // Status Bar
    //
	FillStatusBar();


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


    // Assign singleton just before triggering load operations and leave
    //
	smInstance = this;


    // Load command line or user settings referenced files 
    //
    QTimer::singleShot( 0, this, SLOT( LoadCmdLineFiles() ) );
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


bool CBratMainWindow::ReadSettings( bool &has_gui_settings )
{
	if ( !mRecentFilesProcessor->ReadSettings() )
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
	}

	return mSettings.Status() == QSettings::NoError;
}




bool CBratMainWindow::WriteSettings()
{
	assert__( !destroyed() );

    mSettings.m_lastPageReached = n2s< std::string >( mMainWorkingDock->SelectedTab() );
    mSettings.mAdvancedOperations = WorkingPanel< eOperations >()->AdvancedMode();
    const CWorkspace *wks = mModel.GetCurrentRootWorkspace();
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


bool CBratMainWindow::OkToContinue()
{
	if ( mModel.GetCurrentRootWorkspace() == nullptr )
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
	emit WorkspaceChanged( mModel.GetCurrentWorkspace< CWorkspaceDataset >() );
	emit WorkspaceChanged( mModel.GetCurrentWorkspace< CWorkspaceOperation >() );
	emit WorkspaceChanged( mModel.GetCurrentWorkspace< CWorkspaceDisplay >() );
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
	//return DoEmptyWorkspace();		// TODO DELLTE THIS AFTER DEBUGGING

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
	static QString last_path = t2q( ApplicationDirectories::instance().mExternalDataDir );

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
	static QString last_path = t2q( ApplicationDirectories::instance().mExternalDataDir );

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

    const CWorkspace *wks = mModel.GetCurrentRootWorkspace();
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
	static QString last_path = t2q( ApplicationDirectories::instance().mExternalDataDir );

	CWorkspace *wks = mModel.GetCurrentRootWorkspace();			assert__( wks != nullptr );


    bool bOk = false;               Q_UNUSED( bOk );

	mModel.ResetImportTree();

	//std::string wksPath = wxGetApp().GetCurrentWorkspace()->GetPathName();
	//wxFileName currentWksPath;
	//currentWksPath.AssignDir( wksPath );
	//currentWksPath.MakeAbsolute();

	std::string error_msg;
	if ( !mModel.SaveWorkspace( error_msg ) )	// TODO SaveWorkspace failures not verified in v3
	{
		SimpleErrorBox( error_msg );
		return;
	}

	CWorkspaceDialog dlg( this, CTreeWorkspace::eImport, wks, last_path, mModel );
	//wxGetApp().m_tree.GetImportBitSet()->m_bitSet.reset();

	if ( dlg.exec() != QDialog::Accepted )
		return;

	last_path = t2q( dlg.mPath );


	std::vector< std::string > formulasToImport;

	mModel.ResetImportBits();
	mModel.SetImportBits( 
	{ 
        { IMPORT_DATASET_INDEX,	dlg.mDatasets },
        { IMPORT_FORMULA_INDEX, dlg.mFormulas },
        { IMPORT_OPERATION_INDEX, dlg.mOperations },
        { IMPORT_DISPLAY_INDEX, dlg.mViews }
	} );


    CWorkspace* import_wks = new CWorkspace( dlg.mPath );		//CWorkspace* wks = new CWorkspace( dlg.GetWksName()->GetValue(), dlg.mPath, false );

    Q_UNUSED( import_wks );

	NOT_IMPLEMENTED
	return;

	/*
	wxGetApp().m_treeImport.SetCtrlDatasetFiles( wxGetApp().m_tree.GetImportBitSet()->m_bitSet.test( IMPORT_DATASET_INDEX ) );

	wxGetApp().CreateTree( wks, wxGetApp().m_treeImport );

	//-----------------------------------------
	wxGetApp().SetCurrentTree( &( wxGetApp().m_treeImport ) );
	//-----------------------------------------

	bOk = wxGetApp().m_treeImport.LoadConfig();

	if ( bOk == false )
	{
		wxGetApp().m_treeImport.DeleteTree();
		wxGetApp().m_tree.DeleteTree();

		bool oldValue = wxGetApp().m_tree.GetCtrlDatasetFiles();
		wxGetApp().m_tree.SetCtrlDatasetFiles( false );

		OpenWorkspace( wksPath );

		wxGetApp().m_tree.SetCtrlDatasetFiles( oldValue );

		return bOk;
	}

	CWorkspaceOperation* wksOpImport = wxGetApp().GetCurrentWorkspaceOperation();

	//-----------------------------------------
	wxGetApp().SetCurrentTree( &( wxGetApp().m_tree ) );
	//-----------------------------------------

	// Retrieve formula to Import
	CWorkspaceFormula* wksFormula = wxGetApp().GetCurrentWorkspaceFormula();

	dlg.GetCheckedFormulas( wksFormula->GetFormulasToImport() );


	bOk = wxGetApp().m_tree.Import( &( wxGetApp().m_treeImport ) );
	if ( bOk )
	{
		SaveWorkspace();

		CWorkspaceOperation* wksOp = wxGetApp().GetCurrentWorkspaceOperation();
		if ( ( wksOp != nullptr ) && ( wksOpImport != nullptr ) )
		{
			CDirTraverserCopyFile traverserCopyFile( wksOp->GetPath(), "nc" );
			//path name given to wxDir is locked until wxDir object is deleted
			wxDir dir;

			dir.Open( wksOpImport->GetPathName() );
			dir.Traverse( traverserCopyFile );
		}

	}

	wxGetApp().m_treeImport.DeleteTree();
	wxGetApp().m_tree.DeleteTree();

	bool oldValue = wxGetApp().m_tree.GetCtrlDatasetFiles();
	wxGetApp().m_tree.SetCtrlDatasetFiles( false );

	bOk = OpenWorkspace( wksPath );

	wxGetApp().m_tree.SetCtrlDatasetFiles( oldValue );

	return bOk;
	  */
}


void CBratMainWindow::on_action_Rename_Workspace_triggered()
{
	static QString last_path = t2q( ApplicationDirectories::instance().mExternalDataDir );

	CWorkspace *wks = mModel.GetCurrentRootWorkspace();
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
	static QString last_path = t2q( ApplicationDirectories::instance().mExternalDataDir );

	CWorkspace *wks = mModel.GetCurrentRootWorkspace();
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


void CBratMainWindow::on_action_Test_triggered()
{
#if defined (DEBUG) || defined(_DEBUG)

    std::string msg = "Main application settings file: " + mSettings.FilePath();
    SimpleMsgBox( msg );

    // NOT_IMPLEMENTED

#endif
}




/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
//							UPDATE STATE
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////


void CBratMainWindow::UpdateWorkspaceUI( const CModel *model )
{
	Q_UNUSED( model );

	const bool enable = mModel.GetCurrentRootWorkspace() != nullptr;

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
