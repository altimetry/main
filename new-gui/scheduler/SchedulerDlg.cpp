#include "stdafx.h"

// Include private headers

#include <locale>
#include <string>

#include <QMenuBar>

#include "new-gui/Common/+/QtUtils.h"

#include "TaskProcessor.h"


//#include "Trace.h"
//#include "List.h"
#include "libbrathl/Tools.h"
//#include "Exception.h"
//using namespace brathl;

//#include "BratScheduler.h"
//#include "SchedulerTaskConfig.h"
//#include "RichTextFrame.h"
//#include "DirTraverser.h"

#include "SchedulerDlg.h"

// When debugging changes all calls to new to be calls to DEBUG_NEW allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include <libbrathl/Win32MemLeaksAccurate.h>

#ifdef WIN32
const std::string BRATHL_ICON_FILENAME = "BratIcon.ico";
#else
const std::string BRATHL_ICON_FILENAME = "BratIcon.bmp";
#endif

//const std::string BRATGUI_APPNAME = "scheduler";
//const std::string GROUP_COMMON = "Common";
//const std::string ENTRY_USER_MANUAL = "UserManual";
//const std::string ENTRY_USER_MANUAL_VIEWER = "UserManualViewer";

//QString mUserManualViewer;
//std::string m_userManual;




SchedulerDlg::SchedulerDlg(QWidget *parent) : QDialog(parent)
	, mIsDialog( parent ? true : false )
{
    setupUi(this);

    if (mIsDialog)
    {
        setWindowTitle( "Brat Scheduler Dialog");
    }
    else
    {
        setWindowTitle( "Brat Scheduler");

		//RCCC (2015/11/14): Eliminate menu items in excess (left here only for "education" 
		//	purposes) and add the pertinent ones. Create event handlers (slots) for all 
		//	pertinent items even if they are left with empty bodies that simply display a 
		//	message (use "Stupid Item..." as reference and note that stupid items are not 
		//	pertinent. And that we do not want stupid things in our applications).
		//	Note also, seriously, that menu item names must end with "..." if they possibly 
		//	trigger a dialog box of any kind (the user is reassured that the action does 
		//	not take place immediately, there is yet another step that allows canceling).
		//
		//	Define the createMenuBar member function and move there all menu creation 
		//	functionality; the constructor simply invokes that function in this branch (the
		//	"else") of the if. In the "then" branch (mIsDialog is true), create button(s)
		//	to close the dialog and the respective actions (ok and cancel, or simply close:
		//	see in BratGui which of them make sense).
        //
        //	(2015/11/16)
        //  Is a maximize button possible? In a dialog? I didn't check, but please see if it is.
        //
        //	For dialogs, including dialog applications, the minimum size should be the
        //  opening one. Use setMinimumSize, or similar.

        //  Disregard the following notes, until we talk (I left them here to serve as
        //      reminders for me)
        //
        //      - generate tasks xml with arguments
		//
        //      - wxBratTools::wxStringTowxLongLong_t


        // 1. MenuBar
        QMenuBar *menuBar = new QMenuBar(this);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        //menuBar->setGeometry(QRect(0, 0, 957, 21));

        // 1.1. Menu File
        QMenu *menu_File = new QMenu(tr("&File"), menuBar);
        menu_File->setObjectName(QString::fromUtf8("menu_File"));
        menuBar->addAction(menu_File->menuAction());

        // 1.1.1. Action Exit
        QAction *action_Exit = new QAction(tr("&Exit"), this);
        action_Exit->setShortcut(QKeySequence::Quit);
        action_Exit->setStatusTip(tr("Quit BRAT Scheduler"));
        QObject::connect(action_Exit, SIGNAL(triggered()), this, SLOT(close()));
        menu_File->addAction(action_Exit);

        // 1.2. Menu View
        QMenu *menu_View = new QMenu(tr("&View"), menuBar);
        menu_View->setObjectName(QString::fromUtf8("menu_View"));
        menuBar->addAction(menu_View->menuAction());

        // 1.2.1. Action ViewConfig
        QAction *action_ViewConfig = new QAction(tr("&Configuration file..."), this);
        action_ViewConfig->setStatusTip(tr("Open task configuration file"));
        QObject::connect(action_ViewConfig, SIGNAL(triggered()), this, SLOT(action_ViewConfig_slot()));
        menu_View->addAction(action_ViewConfig);

        // 1.3. Menu Help
        QMenu *menu_Help = new QMenu(tr("&Help"), menuBar);
        menu_Help->setObjectName(QString::fromUtf8("menu_Help"));
        menuBar->addAction(menu_Help->menuAction());

        // 1.3.1. Action UserManual
        QAction *action_UserManual = new QAction(tr("&User's manual"), this);
        action_UserManual->setShortcut(QKeySequence::HelpContents);
        action_UserManual->setStatusTip(tr("Open user's manual"));
        QObject::connect(action_UserManual, SIGNAL(triggered()), this, SLOT(action_UserManual_slot()));
        menu_Help->addAction(action_UserManual);
        menu_Help->addSeparator();

        // 1.3.2. Action Help
        QAction *action_About = new QAction(tr("&About"), this);
        action_About->setShortcut(QKeySequence::WhatsThis);
        QObject::connect(action_About, SIGNAL(triggered()), this, SLOT(action_About_slot()));
        menu_Help->addAction(action_About);


        mVerticalLayout->setMenuBar(menuBar);

//        QToolBar *toolBar = new QToolBar();
//        gridLayout->addWidget(toolBar);
//        QAction *action1 = new QAction("Add", toolBar);
//        QAction *action2 = new QAction("Del", toolBar);

//        menuBar = new QMenuBar(this)
//        layout()->setMenuBar(menuBar);

          //QMainWindow* mainWindow = new QMainWindow();
    }

    // Single Instance Checker TODO!!!!!

    // To be sure that number have always a decimal point (and not a comma
    // or something else)
    setlocale(LC_NUMERIC, "C");
}


void SchedulerDlg::action_BlackAdder_slot()
{
	SimpleMsgBox("This message is false.");
}

void SchedulerDlg::functionWithCodeToMoveToApplicationClassConstructor()
{
	//	I. SingleInstanceChecker

	//	II. Logging

	// To be sure that number have always a decimal point (and not a comma or something else)
	//
	setlocale( LC_NUMERIC, "C" );

	//	III. Locate data directory
	/*
	if ( getenv( BRATHL_ENVVAR ) == NULL )
	{
		// Note that this won't work on Mac OS X when you use './BratGui' from within the Contents/MacOS directory of
		// you .app bundle. The problem is that in that case Mac OS X will change the current working directory to the
		// location of the .app bundle and thus the calculation of absolute paths will break
		CTools::SetDataDirForExecutable( wxGetApp().argv[ 0 ] );
	}


	if ( appPath != "" )
	{
		if ( getenv( BRATHL_ENVVAR ) == NULL )
		{
			CTools::SetDataDirForExecutable( wxGetApp().argv[ 0 ] );
		}
	}

	if ( !CTools::DirectoryExists( CTools::GetDataDir() ) )
	{
		std::cerr << "ERROR: " << CTools::GetDataDir() << " is not a valid directory" << std::endl;
		::wxMessageBox( std::string( CTools::GetDataDir().c_str() ) + " is not a valid directory -- BRAT cannot continue. \n\nAre you sure your " + BRATHL_ENVVAR + " environment variable is set correctly?", "BRAT ERROR" );

		DeleteChecker(); // OnExit() won't be called if we return false

		return false;
	}


	std::string errorMsg;
	if ( !CTools::LoadAndCheckUdUnitsSystem( errorMsg ) )
	{
		std::cerr << errorMsg << std::endl;
		::wxMessageBox( errorMsg.c_str(), "BRAT ERROR" );

		DeleteChecker(); // OnExit() won't be called if we return false

		return false;
	}

	try
	{
		LoadConfigBratGui();
	}
	catch ( CException &e )
	{
		std::cerr << errorMsg << std::endl;
		wxMessageBox( std::string::Format( "An error occured while loading BratGui configuration (CBratSchedulerApp::LoadConfigBratGui)\nNavive exception: %s", e.what() ),
			"Warning",
			wxOK | wxCENTRE | wxICON_EXCLAMATION );

	}

	//----------------
	// Install listeners
	//----------------
	InstallEventListeners();

	m_schedulerTimerId = ::wxNewId();
	m_checkConfigFileTimerId = ::wxNewId();
	
	try
	{
		//m_frame = new CSchedulerFrame( NULL, -1, BRATSCHEDULER_TITLE );

		CSchedulerTaskConfig* schedulerTaskConfig = CSchedulerTaskConfig::GetInstance();

		// After loading, tasks whose status is 'in progress' are considered as 'pending'
		// They have to be re-executed.
		ChangeProcessingToPending();

		m_checkConfigFileTimer.SetFileName( schedulerTaskConfig->GetFullFileName() );
		m_checkConfigFileTimer.SetOwner( this, m_checkConfigFileTimerId );

		m_schedulerTimer.SetOwner( this, m_schedulerTimerId );
	}
	catch ( CException &e ) {
		SimpleMsgBox( std::string::Format( "An error occured while loading Brat Scheduler configuration\nNavive exception: %s", e.what() ),
			"Warning",
			wxOK | wxCENTRE | wxICON_EXCLAMATION );
	}

	if ( m_frame == NULL )
	{
		DeleteChecker();
		return false;
	}

	m_frame->Show( TRUE );

	try
	{
		StartSchedulerTimer();
		StartCheckConfigFileTimer();
	}
	catch ( CException &e )
	{
		SimpleMsgBox( std::string( e.what() ), "Warning", wxOK | wxCENTRE | wxICON_EXCLAMATION );
	}

	//m_frame->SetTitle();
	*/

#if defined (WIN32)
    readTasks( "C:\\Users\\fernando\\AppData\\Roaming\\BratScheduler\\BratSchedulerTasksConfig.xml" );
#endif
}


///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_SchedulerDlg.cpp"
