#include "stdafx.h"


#include "new-gui/QtInterface.h"
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




CBratApplication::CBratApplication( int &argc, char **argv, bool GUIenabled, QString customConfigPath )	//customConfigPath = QString() 

	: base_t( argc, argv, GUIenabled, customConfigPath )

	, mSettings( *smApplicationPaths, "ESA", q2t< std::string >( QgsApplication::applicationName() ) )

{
	LOG_TRACE( "Starting application instance construction..." );

	LOG_TRACE( showSettings() );
	LOG_TRACE( qgisSettingsDirPath() );

// v4
//#ifdef WIN32
//#ifdef _DEBUG
//	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
//#endif
//#endif

    assert__( smPrologueCalled );
	if ( !smPrologueCalled )
		throw CException( "CBratApplication Prologue must be called before application construction." );

	// Register Brat algorithms
	//
	CBratAlgorithmBase::RegisterAlgorithms();


	// To be sure that number have always a decimal point (and not a comma or something else)
	//
	setlocale( LC_NUMERIC, "C" );


	// Load configuration
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

	
	// CODA
	//
	CProduct::CodaInit();	//CProduct::SetCodaReleaseWhenDestroy(false);	//v3 commented out 


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
		std::cout << "Unable to save BRAT the application settings." << std::endl;	// TODO log this

    mSettings.Sync();
}



void CBratApplication::UpdateSettings()
{
	bool result = true;
	//const TApplicationOptions &current = getAppOptions();		assert__( current.m_DefaultAppStyle == options.m_DefaultAppStyle );		//this field is set by application ctor and so far there is no reason besides error to request it's change
	//bool needStyleUpdate = 
	//	current.m_AppStyle != options.m_AppStyle ||
	//	current.m_UseDefaultStyle != options.m_UseDefaultStyle ||

	//	current.m_CustomAppStyleSheet != options.m_CustomAppStyleSheet ||
	//	current.m_NoStyleSheet != options.m_NoStyleSheet;

    if ( /*needStyleUpdate && */!mSettings.setApplicationStyle( *this, mDefaultAppStyle /*options */) )	//if necessary (e.g. error) setApplicationStyle corrects options to reflect actually changed settings; so far (08/2014) this only happens if style sheet resource not found
		result = false;

	// getPrivateAppOptions() = options;

	//return result;
}




////////////////////////////////////////////////
//
////////////////////////////////////////////////

#include "moc_BratApplication.cpp"
