#include "stdafx.h"

#include <QtOpenGL>

#include "new-gui/Common/+Utils.h"
#include "new-gui/Common/+UtilsIO.h"
#include "new-gui/Common/QtUtils.h"
#include "new-gui/Common/DataModels/TaskProcessor.h"
#include "DataModels/Workspaces/Operation.h"

#include "ApplicationLogger.h"
#include "CoreApplication.h"


template< class APPLICATION, class SETTINGS >
bool CCoreApplication< APPLICATION, SETTINGS >::smPrologueCalled = false;

template< class APPLICATION, class SETTINGS > 
CApplicationPaths *CCoreApplication< APPLICATION, SETTINGS >::smApplicationPaths = nullptr;

template< class APPLICATION, class SETTINGS > 
const PythonEngine *CCoreApplication< APPLICATION, SETTINGS >::sm_pe = nullptr;


// Tries to create a QApplication on-the-fly to able to use the
//	 GUI, since the only place we will call this is in main, where 
//	everything else has failed.
//
//static 
template< class APPLICATION, class SETTINGS > 
int CCoreApplication< APPLICATION, SETTINGS >::OffGuiErrorDialog( int error_type, char const *error_msg )
{
	int argc = 0;
    QApplication a( argc, nullptr );
    QMessageBox msg_abort;
    msg_abort.setText( QString("A fatal error has occurred.") );
    msg_abort.setInformativeText( error_msg );
    msg_abort.setStandardButtons( QMessageBox::Abort );
    msg_abort.setIcon( QMessageBox::Critical );
    msg_abort.exec();
    return error_type;
}


// Static "pre-constructor"
//
//	Ensure that Qt only begins after anything we want to do first,
//	like assigning/checking application paths or statically setting 
//	application style sheet.
//
template< class APPLICATION, class SETTINGS > 
void CCoreApplication< APPLICATION, SETTINGS >::Prologue( int argc, char *argv[] )
{
	// lambda

	auto safe_debug_envar_msg = []( const char *var )
	{
		const char *value = getenv( var );
		QString msg( var );
		msg += "=";
		LOG_TRACE( value ? msg + value : msg );
	};

	// function body

    assert__( !smPrologueCalled );    UNUSED(argc);

	if ( smPrologueCalled )
		throw CException( "CCoreApplication Prologue must be called only once." );

	LOG_TRACE( "prologue tasks.." );
	
#if defined(_MSC_VER)
	if ( !CheckSETranslator() )
		LOG_TRACE( "WARNING: structured exceptions translator not assigned." );

	SetAbortSignal();
#endif

	safe_debug_envar_msg( "QGIS_LOG_FILE" );
	safe_debug_envar_msg( "QGIS_DEBUG_FILE" );
	safe_debug_envar_msg( "QGIS_DEBUG" );


	// Application Pahs ////////////////////////////////////

    static CApplicationPaths brat_paths( argv[0] );		// (*)
    if ( !brat_paths.IsValid() )
		throw CException( "One or more path directories are invalid:\n" + brat_paths.GetErrorMsg() );

	smApplicationPaths = &brat_paths;


	// Python Engine ////////////////////////////////////

	try {
		std::wstring ws = q2w( brat_paths.mExecutableDir.c_str() ) + L"/Python";
		wchar_t *argv_buffer = new wchar_t[ ws.length() + 1 ];
		memcpy( argv_buffer, ws.c_str(), ( ws.length() + 1 ) * sizeof( wchar_t ) );
		sm_pe = PythonEngine::CreateInstance( argv_buffer );
    }
    catch ( const CException &e )
    {
        LOG_TRACEstd( e.Message() );
		sm_pe = nullptr;
    }
    catch ( ... )
    {
        LOG_TRACE( "Unknown exception caught creating embedded Python." );
		sm_pe = nullptr;
    }

    smPrologueCalled = true;

	LOG_TRACE( "prologue tasks finished." );
}
//(*) As a last resort, this would solve the problem of double version loading:
//
//      setenv( "QT_PLUGIN_PATH", nullptr, 1 );
//



// Adaptation from boxes Qt demo
//
//static 
template< class APPLICATION, class SETTINGS > 
void CCoreApplication< APPLICATION, SETTINGS >::CheckOpenGL( bool extended )		//extended = false 
{
	//	lambdas

	auto matchString = [](const char *extensionString, const char *subString) -> bool 
	{
		int subStringLength = strlen(subString);
		return (strncmp(extensionString, subString, subStringLength) == 0)
			&& ((extensionString[subStringLength] == ' ') || (extensionString[subStringLength] == '\0'));
	};


	//	function body

	if ( ( QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_1_5 ) == 0 ) 
	{
		throw CException(
			"OpenGL version 1.5 or higher is required to run brat.\n"
			"The program will now exit." );
	}

	if ( extended )
	{
		QGLFormat f( QGL::SampleBuffers );
		QGLWidget widget( f );
		widget.makeCurrent();

		const char *extensionString = reinterpret_cast<const char *>( glGetString( GL_EXTENSIONS ) );
		const char *p = extensionString;

		const int GL_EXT_FBO = 1;
		const int GL_ARB_VS = 2;
		const int GL_ARB_FS = 4;
		const int GL_ARB_SO = 8;
		const int GL_ALL = GL_EXT_FBO | GL_ARB_VS | GL_ARB_FS | GL_ARB_SO;
		int extensions = 0;

		while ( *p ) {
			if ( matchString( p, "GL_EXT_framebuffer_object" ) )
				extensions |= GL_EXT_FBO;
			else if ( matchString( p, "GL_ARB_vertex_shader" ) )
				extensions |= GL_ARB_VS;
			else if ( matchString( p, "GL_ARB_fragment_shader" ) )
				extensions |= GL_ARB_FS;
			else if ( matchString( p, "GL_ARB_shader_objects" ) )
				extensions |= GL_ARB_SO;
			while ( ( *p != ' ' ) && ( *p != '\0' ) )
				++p;
			if ( *p == ' ' )
				++p;
		}

		if ( extensions != GL_ALL ) 
		{
			throw CException(
				"The OpenGL extensions required to run brat are missing.\n"
				"The program will now exit." );
		}
	}
}


template< class APPLICATION, class SETTINGS > 
void CCoreApplication< APPLICATION, SETTINGS >::CheckRunMode()
{
	LOG_TRACE( "Operating mode check..." );			assert__( !mOperatingInDisplayMode );
	
    QStringList args = arguments();
    args.removeFirst();
	QString wkspc_dir;
	mOperatingInDisplayMode = !args.empty() && !IsDir( args[ 0 ] );	//no workspace, but there are command line arguments: let the old BratDisplay ghost take the command
}


template< class APPLICATION, class SETTINGS > 
CCoreApplication< APPLICATION, SETTINGS >::CCoreApplication( int &argc, char **argv, bool GUIenabled, QString customConfigPath )	//customConfigPath = QString() 

	: base_t( argc, argv, GUIenabled, customConfigPath.isEmpty() ? smApplicationPaths->mInternalDataDir.c_str() : customConfigPath )

	, mSettings( *smApplicationPaths, "ESA", q2t< std::string >( QgsApplication::applicationName() ) )

{
	LOG_TRACE( "Starting application instance construction..." );

// v4
//#ifdef WIN32
//#ifdef _DEBUG
//	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
//#endif
//#endif

    assert__( smPrologueCalled );
	if ( !smPrologueCalled )
		throw CException( "CCoreApplication Prologue must be called before application construction." );

	
	CheckRunMode();

	
	CreateSplash();


	// QGIS paths
	//
	setPkgDataPath( mSettings.BratPaths().mInternalDataDir.c_str() ); // returned by QgsApplication::srsDbFilePath();	variable: mPkgDataPath
	LOG_TRACE( "srsDbFilePath==" + srsDbFilePath() );
	LOG_TRACE( "showSettings==" + showSettings() );
	LOG_TRACE( "qgisSettingsDirPath==" + qgisSettingsDirPath() );
	LOG_TRACE( "qgisUserDbFilePath==" + qgisUserDbFilePath() );		//variable mConfigPath
    LOG_TRACEstd( "Qt plugins==" + mSettings.BratPaths().mQtPluginsDir );
    LOG_TRACEstd( "QGIS plugins==" + mSettings.BratPaths().mQgisPluginsDir );


	// To be sure that number have always a decimal point (and not a comma or something else)
	//
	setlocale( LC_NUMERIC, "C" );


	ShowSplash( "Loading application settings..." );

	// Load configuration - I
	//
    if ( !mSettings.LoadConfig() )
	{		
		throw CException( "Error reading the configuration. Settings file: " + mSettings.FilePath() );
	}


	// Keep v3 happy
	//
	CTools::SetInternalDataDir( mSettings.BratPaths().mInternalDataDir );	assert__( CTools::DirectoryExists( CTools::GetInternalDataDir() ) );


	// Register Brat algorithms
	//
	LOG_TRACE( "Registering algorithms..." );
	RegisterAlgorithms();


	// OpenGL
	//
	ShowSplash( "Checking OpenGL..." );
	if ( mSettings.mCheckOpenGL )
	{
		CheckOpenGL();							//throws on failure
		LOG_TRACE( "OpenGL check successful." );
	}


	// Use application paths to initialize COperation internal path references
	//
	COperation::SetExecNames( mSettings.BratPaths() );


	// Load physical units system
	//
	std::string errorMsg;
	if ( !CTools::LoadAndCheckUdUnitsSystem( errorMsg ) )
	{
		std::cerr << errorMsg << std::endl;
		SimpleErrorBox( errorMsg.c_str() );
		throw false;
	}


	// Load aliases dictionary
	//
	CAliasesDictionary::LoadAliasesDictionary( &errorMsg, false );
	if ( !errorMsg.empty() )
	{
		std::cerr << "WARNING: " << errorMsg << std::endl;
		SimpleWarnBox( errorMsg );
	}

	
	// CODA - can only be done after SetInternalDataDir
	//
	assert__( CProduct::GetRefCount() <= 0 );
	//
	CProduct::CodaInit();


	// Load configuration - II - can only be done after CodaInit
	//
    if ( !mSettings.LoadConfigSelectionCriteria() )
	{		
		throw CException( "Error reading the configuration file." );
	}


    // (*) this can be statically set, but not statically 
	//	queried before ctor call (issues Qt assertion)
    //
#if defined (Q_OS_LINUX)
    mDefaultAppStyle = t2q( mSettings.getNameOfStyle( new QCleanlooksStyle, true ) );	//(*)
#else
	mDefaultAppStyle = getCurrentStyleName();											//(*)
#endif

    mSettings.setApplicationStyle( *this, mDefaultAppStyle );

	// Load XML processor and TasksProcessor
	//
	ShowSplash( "Loading scheduled tasks processor." );

    ::xercesc::XMLPlatformUtils::Initialize();

    if ( !CTasksProcessor::GetInstance() )
        CTasksProcessor::CreateInstance( mSettings.BratPaths().mExecutableDir );

	LOG_TRACE( "Scheduled tasks processor loaded." );


    //v4: remaining initialization in charge of the main window

	LOG_TRACE( "Finished application instance construction." );
}


//virtual 
template< class APPLICATION, class SETTINGS > 
CCoreApplication< APPLICATION, SETTINGS >::~CCoreApplication()
{
    CProduct::CodaRelease();

    ::xercesc::XMLPlatformUtils::Terminate();
	CTasksProcessor::DestroyInstance();

    if ( !mSettings.SaveConfig() )
		LOG_TRACE( "Unable to save BRAT the application settings." );

    mSettings.Sync();
}



template< class APPLICATION, class SETTINGS > 
void CCoreApplication< APPLICATION, SETTINGS >::EndSplash( QWidget *w )
{
	assert__( mSplash );

	mSplash->finish( w );
	delete mSplash;
	mSplash = nullptr;
}
template< class APPLICATION, class SETTINGS > 
void CCoreApplication< APPLICATION, SETTINGS >::ShowSplash( const std::string &msg, bool disable_events ) const		//disable_events = false 
{
	assert__( mSplash );

	LOG_TRACE( msg.c_str() );

	mSplash->showMessage( msg.c_str(), Qt::AlignHCenter | Qt::AlignBottom, Qt::white );
	if ( !disable_events )
		processEvents();

	//QBratThread::sleep( 1 );
}
template< class APPLICATION, class SETTINGS > 
void CCoreApplication< APPLICATION, SETTINGS >::CreateSplash()
{
	assert__( !mSplash );

	QPixmap myPixmap( "://images/InstallSplashScreen.png" );		//://images/Create.jpg		://images/12-final.jpg
	if ( myPixmap.isNull() )
		LOG_TRACE( "Null splash image" );
	mSplash = new QSplashScreen( myPixmap );
    mSplash->setMask( myPixmap.mask() );
    mSplash->show();
}
