#include "stdafx.h"

#include "SchedulerApplication.h"
#include "new-gui/Common/ScheduledTasksList.hxx"
#include "new-gui/Common/XmlSerializer.h"
#include "new-gui/Common/DataModels/TaskProcessor.h"
#include "new-gui/Common/ConsoleApplicationPaths.h"

#if defined(MEM_LEAKS)
	#include "new-gui/Common/WinMemChecker.h"
	MemChecker MemChecker::instance;

	_CrtMemState CSchedulerApplication::FirstState;			//before all run-time allocations
	_CrtMemState CSchedulerApplication::BeforeRunState;		//after app creation, before running engine
	_CrtMemState CSchedulerApplication::AfterRunState;		//after running engine, before de-allocating
#endif

const CConsoleApplicationPaths *CSchedulerApplication::smApplicationPaths = nullptr;


 
// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "libbrathl/Win32MemLeaksAccurate.h"


// Tries to create a QApplication on-the-fly to able to use the
//	 GUI, since the only place we will call this is in main, where
//	everything else has failed.
//
//static
int CSchedulerApplication::OffGuiErrorDialog( int error_type, char const *error_msg )
{
    int argc = 0;
    QApplication a( argc, nullptr );
    QMessageBox msg_abort;
    msg_abort.setText( QString("A fatal error as ocurred.") );
    msg_abort.setInformativeText( error_msg );
    msg_abort.setStandardButtons( QMessageBox::Abort );
    msg_abort.setIcon( QMessageBox::Critical );
    msg_abort.exec();
    return error_type;
}



CSchedulerApplication::CSchedulerApplication(int &argc, char **argv, int flags)
    : base_t(argc, argv, flags)
{
    ::xercesc::XMLPlatformUtils::Initialize();

#if defined(MEM_LEAKS)
	_CrtMemCheckpoint( &FirstState );			//set first flag
#endif

    //	I. SingleInstanceChecker

    //	II. Logging

    // To be sure that number have always a decimal point (and not a comma or something else)
    //
    setlocale( LC_NUMERIC, "C" );

    //	III. Locate data directory

    static const CConsoleApplicationPaths brat_paths( argv[ 0 ] );
    if ( !brat_paths.IsValid() )
        throw CException( "One or more path directories are invalid:\n" + brat_paths.GetErrorMsg() );

    smApplicationPaths = &brat_paths;

    CTools::SetInternalDataDir( brat_paths.mInternalDataDir );

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

    //	IV. Units System

    std::string errorMsg;
    if ( !CTools::LoadAndCheckUdUnitsSystem( errorMsg ) )
    {
        std::cerr << errorMsg << std::endl;
        ::wxMessageBox( errorMsg.c_str(), "BRAT ERROR" );

        DeleteChecker(); // OnExit() won't be called if we return false

        return false;
    }

    //	V. Application Configuration

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

    //	V a wxWidgtes paraphernalia NOTTODO

    //----------------
    // Install listeners
    //----------------
    InstallEventListeners();

    //	VI. Load Tasks & Create Timers

    m_schedulerTimerId = ::wxNewId();
    m_checkConfigFileTimerId = ::wxNewId();

    try
    {
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        m_frame = new CSchedulerFrame( NULL, -1, BRATSCHEDULER_TITLE );			//tasks are loaded here by a certain CPendingPanel

		CTasksProcessor* schedulerTaskConfig = CTasksProcessor::GetInstance();

        // After loading, tasks whose status is 'in progress' are considered as 'pending'
        // They have to be re-executed.
        ChangeProcessingToPending();
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        m_checkConfigFileTimer.SetFileName( schedulerTaskConfig->GetFullFileName() );
        m_checkConfigFileTimer.SetOwner( this, m_checkConfigFileTimerId );

        m_schedulerTimer.SetOwner( this, m_schedulerTimerId );
    }
    catch ( CException &e ) {
        SimpleMsgBox( std::string::Format( "An error occured while loading Brat Scheduler configuration\nNavive exception: %s", e.what() ),
            "Warning",
            wxOK | wxCENTRE | wxICON_EXCLAMATION );
    }

    //	VIa. wxWidgtes paraphernalia NOTTODO

    if ( m_frame == NULL )
    {
        DeleteChecker();
        return false;
    }

    m_frame->Show( TRUE );

    //	VII. Start timers

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
}

//virtual
CSchedulerApplication::~CSchedulerApplication()
{
	//_CrtMemState state, CurrentState, AfterState, ComparisonState;
	//_CrtMemCheckpoint( &CurrentState );
	//
	//int _CrtMemDifference(
	//	_CrtMemState *stateDiff,
	//	const _CrtMemState *oldState,
	//	const _CrtMemState *newState
	//	);
	//
    ::xercesc::XMLPlatformUtils::Terminate();
	//_CrtMemCheckpoint( &AfterState );
	//_CrtMemDifference( &ComparisonState, &CurrentState, &AfterState );
	//_CrtMemDifference( &state, &ComparisonState, &FirstState );
	//_CrtMemDifference( &FirstState, &FirstState, &state );

	CTasksProcessor::DestroyInstance();
}







void CSchedulerApplication::dumpMemoryStatistics()
{
#if defined(MEM_LEAKS)

	if ( MemChecker::dumpStatistics( FirstState ) /*||		//all wasted memory 
		MemChecker::dumpStatistics( BeforeRunState )*/ )	//all memory wasted while running engine
        OutputDebugString( _T( "Possible Memory Leaks" ) );

#endif
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_SchedulerApplication.cpp"
