#include "stdafx.h"

#include <QtOpenGL>

#include "new-gui/Common/+Utils.h"
#include "new-gui/Common/+UtilsIO.h"
#include "new-gui/Common/QtUtils.h"
#include "DataModels/Workspaces/Operation.h"

#include "ApplicationLogger.h"
#include "BratApplication.h"


bool CBratApplication::smPrologueCalled = false;
CApplicationPaths *CBratApplication::smApplicationPaths = nullptr;

// Tries to create a QApplication on-the-fly to able to use the
//	 GUI, since the only place we will call this is in main, where 
//	everything else has failed.
//
//static 
int CBratApplication::OffGuiErrorDialog( int error_type, char const *error_msg )
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


// Static "pre-constructor"
//
//	Ensure that Qt only begins after anything we want to do first,
//	like assigning/checking application paths or statically setting 
//	application style sheet.
//
void CBratApplication::Prologue( int argc, char *argv[] )
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
		throw CException( "CBratApplication Prologue must be called only once." );

	LOG_TRACE( "prologue tasks.." );
	
	safe_debug_envar_msg( "QGIS_LOG_FILE" );
	safe_debug_envar_msg( "QGIS_DEBUG_FILE" );
	safe_debug_envar_msg( "QGIS_DEBUG" );

    static CApplicationPaths brat_paths( argv[0] );		// (*)
    if ( !brat_paths.IsValid() )
		throw CException( "One or more path directories are invalid:\n" + brat_paths.GetErrorMsg() );

	smApplicationPaths = &brat_paths;
	
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
void CBratApplication::CheckOpenGL( bool extended )		//extended = false 
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


void CBratApplication::CheckRunMode()
{
	LOG_TRACE( "Operating mode check..." );			assert__( !mOperatingInDisplayMode );
	
    QStringList args = arguments();
    args.removeFirst();
	QString wkspc_dir;
	mOperatingInDisplayMode = !args.empty() && !IsDir( args[ 0 ] );	//no workspace, but there are command line arguments: let the old BratDisplay ghost take the command
}


CBratApplication::CBratApplication( int &argc, char **argv, bool GUIenabled, QString customConfigPath )	//customConfigPath = QString() 

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
		throw CException( "CBratApplication Prologue must be called before application construction." );

	
	CheckRunMode();

	
	CreateSplash();


	// OpenGL
	//
	ShowSplash( "Checking OpenGL..." );
    //CheckOpenGL();							//throws on failure
	LOG_TRACE( "OpenGL check successful." );


	// QGIS paths
	//
	setPkgDataPath( mSettings.BratPaths().mInternalDataDir.c_str() ); // returned by QgsApplication::srsDbFilePath();	variable: mPkgDataPath
	LOG_TRACE( "srsDbFilePath==" + srsDbFilePath() );
	LOG_TRACE( "showSettings==" + showSettings() );
	LOG_TRACE( "qgisSettingsDirPath==" + qgisSettingsDirPath() );
	LOG_TRACE( "qgisUserDbFilePath==" + qgisUserDbFilePath() );		//variable mConfigPath


	// Register Brat algorithms
	//
	RegisterAlgorithms();


	// To be sure that number have always a decimal point (and not a comma or something else)
	//
	setlocale( LC_NUMERIC, "C" );


	ShowSplash( "Loading application settings..." );

	// Load configuration - I
	//
    if ( !mSettings.LoadConfig() )
	{		
		throw CException( "Error reading the configuration file." );
	}


	// Keep v3 happy
	//
	CTools::SetInternalDataDir( mSettings.BratPaths().mInternalDataDir );	assert__( CTools::DirectoryExists( CTools::GetInternalDataDir() ) );


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

    //v4: remaining initialization in charge of the main window

	LOG_TRACE( "Finished application instance construction." );
}


//virtual 
CBratApplication::~CBratApplication()
{
    CProduct::CodaRelease();

    if ( !mSettings.SaveConfig() )
		LOG_TRACE( "Unable to save BRAT the application settings." );	// TODO log this

    mSettings.Sync();
}



void CBratApplication::EndSplash( QWidget *w )
{
	assert__( mSplash );

	mSplash->finish( w );
	delete mSplash;
	mSplash = nullptr;
}
void CBratApplication::ShowSplash( const std::string &msg, bool disable_events ) const		//disable_events = false 
{
	class I : public QThread
	{
	public:
		static void sleep( unsigned long secs ) 
		{
			QThread::sleep( secs );
		}

		static void msleep( unsigned long msecs ) 
		{
			QThread::msleep( msecs );
		}

		static void usleep( unsigned long usecs ) 
		{
			QThread::usleep( usecs );
		}
	};


	assert__( mSplash );

	mSplash->showMessage( msg.c_str(), Qt::AlignHCenter | Qt::AlignBottom, Qt::white );
	if ( !disable_events )
		processEvents();

	//I::sleep( 1 );
}
void CBratApplication::CreateSplash()
{
	assert__( !mSplash );

	QPixmap myPixmap( "://images/InstallSplashScreen.png" );		//://images/Create.jpg		://images/12-final.jpg
	if ( myPixmap.isNull() )
		LOG_TRACE( "Null splash image" );
	mSplash = new QSplashScreen( myPixmap );
    mSplash->setMask( myPixmap.mask() );
    mSplash->show();
}



void CBratApplication::UpdateSettings()
{
	mSettings.setApplicationStyle( *this, mDefaultAppStyle );
}



void CBratApplication::RegisterAlgorithms()
{
	static const std::string file_prefix = "BratAlgorithm-";
	static const std::string file_suffix = ".py";

	//types

	struct python_base_creator : public base_creator
	{
		using base_t = base_creator;

		const std::string mFilePath;
		const std::string mClassName;

		python_base_creator( const std::string file_path, const std::string &class_name ) 
			: base_t( nullptr )
			, mFilePath( file_path )
			, mClassName( class_name )
		{}

		virtual ~python_base_creator()
		{}

		virtual CBratAlgorithmBase* operator()( void )
		{
			return new PyAlgo( mFilePath, mClassName );
		}
	};


	//function body

	//load C++ algorithms

	CBratAlgorithmBase::RegisterAlgorithms();

	//load python algorithms

	//const std::string python_algorithms_path = mSettings.BratPaths().mExternalDataDir + "/python";
	//try
	//{
	//	QDir sourceDir( python_algorithms_path.c_str() );
	//	QStringList files = sourceDir.entryList( QDir::Files );
	//	auto &registry = CBratAlgorithmBaseRegistry::GetInstance();
	//	for ( int i = 0; i < files.count(); i++ )
	//	{
	//		auto file_name = q2a( files[ i ] );
	//		if ( StartsWith( file_name, file_prefix ) && EndsWith( file_name, file_suffix ) )
	//		{
	//			auto path = python_algorithms_path + "/" + file_name;
	//			auto class_name = file_name.substr( file_prefix.length() );
	//			class_name = class_name.substr( 0, class_name.length() - file_suffix.length() );				//qDebug() << files[ i ];
	//			registry.Add( new python_base_creator( path, class_name ) );
	//		}
	//	}
	//}
	//catch ( const CException &e )
	//{
	//	LOG_WARN( e.Message() );
	//	SimpleErrorBox( e.Message() );
	//}
	//catch ( ... )
	//{
	//	static const std::string msg(
	//		"Unknown exception caught loading python algorithm.\nPlease make sure that the python file name matches the pattern "
	//		+ file_prefix + "<python-class-name>" + file_suffix );
	//	LOG_WARN( msg );
	//	SimpleErrorBox( msg );
	//}
}


////////////////////////////////////////////////
////////////////////////////////////////////////
//			PythonEngine Singleton
////////////////////////////////////////////////
////////////////////////////////////////////////

//const PythonEngine* PyAlgo::sm_pe = &PythonEngine::Instance();




////////////////////////////////////////////////
//
////////////////////////////////////////////////

#include "moc_BratApplication.cpp"
