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

#include <QtOpenGL>

#include "process/BratEmbeddedPythonProcess.h"
#include "externals/hdf5/src/winxp_hack.h"

#if defined (WIN32) || defined(_WIN32)
#include "new-gui/Common/System/Service/qtservice_win.h"
#endif

#include "common/+Utils.h"
#include "common/+UtilsIO.h"
#include "new-gui/Common/QtUtils.h"
#include "new-gui/Common/DataModels/TaskProcessor.h"
#include "DataModels/Model.h"
#include "DataModels/Workspaces/Operation.h"
#include "GUI/LoginDialog.h"

#include "BratLogger.h"
#include "BratApplication.h"



/*****************************************************************************************/
/*****************************************************************************************/
/*****************************************************************************************/

//////////////////////////////////////////////////////////////////////////////////
//								CBratApplication
//////////////////////////////////////////////////////////////////////////////////

bool CBratApplication::smPrologueCalled = false;
CApplicationPaths *CBratApplication::smApplicationPaths = nullptr;



// Tries to create a QApplication on-the-fly to be able to use the
//	 GUI, since the only place we will call this is in main, where 
//	everything else has failed.
//
//static 
int CBratApplication::OffGuiErrorDialog( int error_type, char const *error_msg )
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
	
#if defined(_MSC_VER)
	if ( !CheckSETranslator() )
		LOG_TRACE( "WARNING: structured exceptions translator not assigned." );

	SetAbortSignal();
#endif
    
    qputenv( "QT_LOGGING_RULES", "qt.network.ssl.warning=false" );

	safe_debug_envar_msg( "QGIS_LOG_FILE" );
	safe_debug_envar_msg( "QGIS_DEBUG_FILE" );
	safe_debug_envar_msg( "QGIS_DEBUG" );


	// Application/Organization Names ////////////////////////////////////

	QCoreApplication::setApplicationName( BRAT_APPLICATION_NAME );
	QCoreApplication::setOrganizationName( ORGANIZATION_NAME );


	// Application Paths ////////////////////////////////////

    static CApplicationPaths brat_paths( argv[0], BRAT_APPLICATION_NAME );		// (*)
    if ( !brat_paths.IsValid() )
		throw CException( "One or more path directories are invalid:\n" + brat_paths.ErrorMsg() );

	smApplicationPaths = &brat_paths;	LOG_TRACEstd( smApplicationPaths->ToString() );


	// Check Windows stat function ////////////////////////////////////
	//
	CheckWindowsStat();


	// Qt MetaTypes ////////////////////////////////////

	RegisterAsQtTypes();

	
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


void CBratApplication::CheckWindowsStat()
{
#if defined (WIN32) || defined(_WIN32)

	if ( is_xp_32() )
	{
		LOG_TRACE( "Running in Windows XP 32 bits" );
	}
	struct stat s { brat_sign };
	if ( stat( "", &s ) == brat_sign )
		LOG_TRACE( "Using stat replacement" );
	struct stat fs { brat_sign };
	if ( fstat( 0, &fs ) == brat_sign )
		LOG_TRACE( "Using fstat replacement" );

#endif
}


// Called in Prologue; see Q_DECLARE_METATYPE after class declaration
// Linux prevents this before the Q_DECLARE_METATYPE 
//
//static 
void CBratApplication::RegisterAsQtTypes()
{
    qRegisterMetaType< ERadsNotification >();
}


// Remove all arguments starting with "--", because they are not
//  for direct brat use, only for its dependencies, such as Qt
//
QStringList CBratApplication::CleanArgumentsList() const
{
    QStringList args = arguments();
    args.removeFirst();
    
    int i = 0;
    while ( i < args.size() )
    {
        if ( args[i].startsWith( "--" ) )
            args.removeAt(i);
        else
            i++;
    }
    
    return args;
}

void CBratApplication::CheckRunMode()
{
	LOG_TRACE( "Operating mode check..." );			assert__( !mOperatingInDisplayMode );
	
    QStringList args = CleanArgumentsList();
    if ( args.isEmpty() )
        return;
    
    QString wkspc_dir = args[ 0 ];
    mOperatingInDisplayMode = !IsDir( wkspc_dir );	//no workspace, but there are command line arguments: let old BratDisplay ghost take the command
    mOperatingInInstantPlotSaveMode = IsDir( args[ 0 ] ) && args.size() >= 3 && args.size() <= 4;
}



CBratApplication::CBratApplication( int &argc, char **argv, bool GUIenabled, QString customConfigPath )	//customConfigPath = QString() 

	: base_t( argc, argv, GUIenabled, customConfigPath.isEmpty() ? smApplicationPaths->mInternalDataDir.mPath.c_str() : customConfigPath )

	, mSettings( *smApplicationPaths, q2a( QCoreApplication::organizationName() ), q2a( QgsApplication::applicationName() ) )

	, mServiceController( RADS_SERVICE_NAME )

    , mSocket( new QLocalSocket( this ) )

	, mRadsServiceSettings( mSettings.BratPaths() )
{
	LOG_TRACE( "Starting application instance construction..." );

    assert__( smPrologueCalled );
	if ( !smPrologueCalled )
		throw CException( "CBratApplication Prologue must be called before application construction." );
	
	CheckRunMode();
	
	CreateSplash();


	// QGIS paths
	//
	setPkgDataPath( mSettings.BratPaths().mInternalDataDir.mPath.c_str() ); // returned by QgsApplication::srsDbFilePath();	variable: mPkgDataPath
	LOG_TRACE( "srsDbFilePath==" + srsDbFilePath() );
	LOG_TRACE( "showSettings==" + showSettings() );
	LOG_TRACE( "qgisSettingsDirPath==" + qgisSettingsDirPath() );
	LOG_TRACE( "qgisUserDbFilePath==" + qgisUserDbFilePath() );		//variable mConfigPath
    LOG_TRACEstd( "Qt plugins==" + mSettings.BratPaths().mQtPluginsDir.mPath );
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
	CTools::SetInternalDataDir( mSettings.BratPaths().mInternalDataDir );	assert__( IsDir( CTools::GetInternalDataDir() ) );


	// Register Brat algorithms
	//
	LOG_TRACE( "Registering algorithms..." );
	RegisterAlgorithms();


	// Check OpenGL
    //  ...setting osgEarth env. variable for VMs in mac
	//
	ShowSplash( "Checking OpenGL..." );
    if ( mSettings.OsgEarthUsePbufferTestForVms() )
        qputenv( "OSGEARTH_USE_PBUFFER_TEST", "1" );
        
	if ( mSettings.mCheckOpenGL )
	{
        CheckOpenGL( true );							//throws on failure
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
#if defined(BRAT_V3)
	if ( !mSettings.LoadConfigSelectionCriteria() )
	{		
		throw CException( "Error reading the configuration file." );
	}
#endif


    // (*) this can be statically set, but not statically 
	//	queried before ctor call (issues Qt assertion)
    //
#if defined (Q_OS_LINUX)
#if QT_VERSION >= 0x050000
    mDefaultAppStyle = t2q( mSettings.getNameOfStyle( QStyleFactory::create( "fusion" ), true ) );       //(*)
#else
    mDefaultAppStyle = t2q( mSettings.getNameOfStyle( new QCleanlooksStyle, true ) );	//(*)
#endif
#else
	mDefaultAppStyle = CurrentStyleName();											//(*)
#endif

    mSettings.setApplicationStyle( *this, mDefaultAppStyle );

	// Load XML processor and TasksProcessor
	//	- catch all: do not prevent brat from starting if scheduling is unavailable
	//
	ShowSplash( "Loading scheduled tasks processor." );

	const std::string xml_error = "ERROR: scheduled tasks processor could not be loaded. ";
	try
	{
		::xercesc::XMLPlatformUtils::Initialize();

		if ( !CTasksProcessor::GetInstance() )
			CTasksProcessor::CreateInstance( SCHEDULER_APPLICATION_NAME, mSettings.BratPaths() );

		LOG_TRACE( "Scheduled tasks processor loaded." );
	}
	catch ( const CException &e )
	{
		LOG_WARN( xml_error + e.Message() );
	}
	catch ( ... )
	{
		LOG_WARN( xml_error );
	}


	// Create application core domain data

	mModel = &CModel::CreateInstance( mSettings );


	// RADS rsync status signals
	//	- ResetRadsSocketConnection must be explicitly called to receive service notifications about the rsync status

    connect( mSocket, SIGNAL( readyRead() ), this, SLOT( HandleSocketReadyRead() ) );
	connect( mSocket, SIGNAL( error(QLocalSocket::LocalSocketError) ), this, SLOT( HandleLocalSocketError(QLocalSocket::LocalSocketError) ) );

	bool config_result = false;
	if ( IsFile( mRadsServiceSettings.FilePath() ) )
		config_result = mRadsServiceSettings.LoadConfig();
	else
	{
		config_result = mRadsServiceSettings.SaveConfig();		//save defaults for brat to read
	}
	if ( !config_result )
		LOG_WARN( "An error occurred accessing the RADS Service settings file '" + mRadsServiceSettings.FilePath() + "'" );


	//v4: remaining initialization in charge of the main window

	LOG_TRACE( "Finished application instance construction." );
}



//virtual 
CBratApplication::~CBratApplication()
{
    CProduct::CodaRelease();

    ::xercesc::XMLPlatformUtils::Terminate();
	CTasksProcessor::DestroyInstance();

    if ( !mSettings.SaveConfig() )
		LOG_TRACE( "Unable to save the BRAT application settings." );

    mSettings.Sync();

	CModel::DestroyInstance();

    if ( mSocket )
		mSocket->disconnectFromServer();
}




inline QString ServiceUserName()
{
#if defined (Q_OS_WIN) && defined(USE_LocalSystem_ACCOUNT)
	return "";			// --> LocalSystem will have different settings file
#else
#if defined (Q_OS_MAC)
	return UserName();
#else
	return UserName();
#endif
#endif
}


bool CBratApplication::InstallRadsService( QWidget *parent )
{
	static const QString rads_service_name( RADS_SERVICE_NAME );

	bool installed = mServiceController.isInstalled();
	if ( !installed ) 
	{
		const std::string license = 
std::string("<p align='left'>" ) +
"RADS was developed at TU Delft, and is now a joint venture between TU Delft, NOAA, and EUMETSAT (http://rads.tudelft.nl). "
"The use of RADS is only intended for academic and institutional purposes.</p>"
+ "<p align='left'>"                
"Please acknowledge or refer usage of RADS in any publication:"
+ "<ul>"
"<li>Either put in the reference list: "
"\"Remko Scharroo, Eric Leuliette, John Lillibridge, Deirdre Byrne, Marc Naeije, and Gary Mitchum. RADS: Consistent Multi-Mission Products. "
"In Symposium on 20 Years of Progress in Radar Altimetry, Venice, 20-28 September 2012, volume SP-710. ESA, 2013\";"
+"</li>"
"<li>or put in the acknowledgements: \"We thank the developers of the altimeter database RADS at TU Delft, NOAA, and EUMETSAT\".</li>"
+ "</ul>"
"\n\nBy clicking ok you acknowledge to have read and accepted the RADS license above."
+ "</p>";

		if ( SimpleConfirmation( license ) )
		{
			LoginDialog *dlg = new LoginDialog( "Install " + rads_service_name, parent );
			dlg->SetUsername( ServiceUserName(), false );
			if ( dlg->exec() == QDialog::Accepted )
			{
				QString account = dlg->Username();
				QString password = dlg->Password();
				QString path = smApplicationPaths->mRadsServicePath.c_str();
				installed = QtServiceController::install( path, account, password );

#if defined (WIN32) || defined(_WIN32)
				if ( installed )
				{
					const std::string std_account = q2a( account );
					if ( !HasLogOnAsServicePrivilege( std_account ) )
					{
						if ( GrantLogOnAsServicePrivilege( std_account ) )
							SimpleMsgBox( "The account '" + std_account + "' has been granted the Log On As A Service right." );
						else
							SimpleWarnBox( "Could not grant the Log On As A Service right to the account '" + std_account + 
								"'.\nIf you cannot start the service from BRAT, please use the Windows Services administrative tool." );
					}
				}
#endif
			}
		}
	}
	bool running = mServiceController.isRunning();

	LOG_TRACE( rads_service_name + " is " + ( installed ? "installed" : "not installed") + " and " + ( running ? "running" : "not running") );
	if ( installed ) 
	{
		LOG_TRACE( rads_service_name + " path: " + mServiceController.serviceFilePath() );
		LOG_TRACE( rads_service_name + " description: " + mServiceController.serviceDescription() );
		LOG_TRACE( rads_service_name + " startup: " + ( mServiceController.startupType() == QtServiceController::AutoStartup ? "Auto" : "Manual") );
	}

	return installed;
}


bool CBratApplication::UninstallRadsService()
{
	bool result = mServiceController.uninstall();
	if ( result && mSocket && mSocket->state() != mSocket->ConnectedState )
		mSocket->disconnectFromServer();
	return result;
}


bool CBratApplication::ResetRadsSocketConnection()
{
	if ( mServiceController.isInstalled() && mServiceController.isRunning() )
	{    
		mSocket->abort();
		mSocket->connectToServer( RADS_SHARED_MEMORY_KEY.c_str() );
		return true;
	}

	return false;
}


bool CBratApplication::StartRadsService()
{
	bool result = mServiceController.start();
	if ( result && mSocket )
	{
		QBratThread::sleep( 4 );    //give some time for isRunning to return an accurate value (that is, for the executable to start)
		ResetRadsSocketConnection();
	}
	return result;
}


bool CBratApplication::StopRadsService()
{
	mSocket->abort();
	bool result = mServiceController.stop();
	if ( result && mSocket )
	{
		QBratThread::sleep( 4 );    //same comment as for StartRadsService

		//TODO: check this: at least in Windows, the broadcast made by the service cleanup (rsync client) 
		//	does not arrive to clients... so, do it manually

		auto prev_rsync_status = mRsyncStatus;
		mRsyncStatus = eRsyncStopped;
		if ( prev_rsync_status != mRsyncStatus )
			emit RadsNotification( Convert( mRsyncStatus ) );
	}
	return result;
}


bool CBratApplication::PauseRadsService()
{
	return mServiceController.pause();
}


bool CBratApplication::ResumeRadsService()
{
	return mServiceController.resume();
}


bool CBratApplication::SendRadsServiceCommand( int code )
{
	return mServiceController.sendCommand( code );
}








bool CBratApplication::RegisterAlgorithms()
{
	std::string msg;
    try
    {
		bool result = CBratEmbeddedPythonProcess::LoadPythonEngine( mSettings.BratPaths().mPythonDir );
		LOG_TRACEstd( CBratEmbeddedPythonProcess::PythonMessages() );
		LOG_TRACE( "Finished registering python algorithms." );
        return result;
    }
    catch ( const CException &e )
    {
        msg = e.Message();
    }
    catch ( ... )
    {
        msg = "Unknown exception caught loading python algorithm.";
    }

	msg += ( "\n" + CBratEmbeddedPythonProcess::PythonMessages() );
	LOG_WARN( msg );
	SimpleErrorBox( msg );

    return false;
}



/////////
// Slots
/////////

//const std::string RSYNC_RUNNING_SIGN = "RUNNING";
//const std::string RSYNC_STOPPED_SIGN = "STOPPED";


bool CBratApplication::ReadSingleSocketMessage()
{
    QDataStream in( mSocket );
    in.setVersion(QDataStream::Qt_4_0);

    quint16 block_size = 0;

    if ( mSocket->bytesAvailable() < (int)sizeof(quint16) )
        return false;

    in >> block_size;

    LOG_TRACEstd( "About to read " + n2s<std::string>(block_size) + " bytes from RADS socket." );

    if ( in.atEnd() )
        return false;

    QString rads_msg;
    in >> rads_msg;

    auto prev_rsync_status = mRsyncStatus;
    auto notification = eNotificationUnknown;

    if ( rads_msg == RSYNC_STOPPED_SIGN.c_str() )
    {
        mRsyncStatus = Convert( ( notification = eNotificationRsyncStopped ) );
    }
    else
    if ( rads_msg == RSYNC_RUNNING_SIGN.c_str() )
    {
        mRsyncStatus = Convert( ( notification = eNotificationRsyncRunnig ) );
    }
    else
    if ( rads_msg == CONFIG_UPDATED_SIGN.c_str() )
    {
        notification = eNotificationConfigSaved;
        mRadsServiceSettings.LoadConfig();		//FIXME : clients should be notified of notification reading errors
    }
    else
    {
        mRsyncStatus = Convert( ( notification = eNotificationUnknown ) );
    }

    LOG_TRACE( "Read " + rads_msg + " from RADS socket." );

    if ( ( prev_rsync_status != mRsyncStatus ) || notification == eNotificationConfigSaved )
        emit RadsNotification( notification );

    return true;
}


void CBratApplication::HandleSocketReadyRead()
{
    //TODO: this a safety escape; ReadSingleSocketMessage never had
    //  issues, but was not tested enough when enclosed in a loop,
    //  and if a bug in the service triggers too many messages, we
    //  don't want brat to hang reading them. Conclusion: consider
    //  deleting after proper testing, or replace by proper excpetion
    //  mechanism.
    //
    static const size_t max_iterations = 10;

    size_t iterations = 0;
    while( ReadSingleSocketMessage() && iterations < max_iterations ) iterations++;
    LOG_TRACE( "Read " + n2q( iterations ) + " notification(s) from RADS service" );
}


void CBratApplication::HandleLocalSocketError( QLocalSocket::LocalSocketError error )
{
    mRsyncStatus = eRsyncUnknown;
    
	switch ( error )
	{
		case QLocalSocket::ServerNotFoundError:
			LOG_WARN( "The RADS socket host was not found. Please check the host name and port settings." );
			break;
            
		case QLocalSocket::ConnectionRefusedError:
			LOG_WARN( "The connection was refused by the peer. \
						Make sure the server is running, and check that the host name and port settings are correct." );
			break;
                    
		case QLocalSocket::PeerClosedError:
			break;
            
		default:
			LOG_WARN( "The following error occurred: " + mSocket->errorString() );
	}
}


void CBratApplication::UpdateSettings()
{
	mSettings.setApplicationStyle( *this, mDefaultAppStyle );
}



/////////
// Splash
/////////

void CBratApplication::EndSplash( QWidget *w )
{
	assert__( mSplash );

	mSplash->finish( w );
	delete mSplash;
	mSplash = nullptr;
}
void CBratApplication::ShowSplash( const std::string &msg, bool disable_events ) const		//disable_events = false 
{
	assert__( mSplash );

	LOG_TRACE( msg.c_str() );

	mSplash->showMessage( msg.c_str(), Qt::AlignHCenter | Qt::AlignBottom, Qt::white );
	if ( !disable_events )
		processEvents();

	//QBratThread::sleep( 1 );
}
void CBratApplication::CreateSplash()
{
	assert__( !mSplash );

	QPixmap pix( "://images/InstallSplashScreen.png" );		//://images/Create.jpg		://images/12-final.jpg
	if ( pix.isNull() )
		LOG_TRACE( "Null splash image" );
	mSplash = new QSplashScreen( pix );
    mSplash->setMask( pix.mask() );
	QIcon icon( "://../../../data/BratIcon.ico" );
	if ( icon.isNull() )
		LOG_TRACE( "Null splash icon" );	
	mSplash->setWindowIcon( icon );
	mSplash->show();

	mSplash->activateWindow();
	mSplash->raise();

#if defined (Q_OS_WIN)
	int exstyle = GetWindowLong( (HWND)mSplash->winId(), GWL_EXSTYLE );
	SetWindowLong( (HWND)mSplash->winId(), GWL_EXSTYLE, exstyle & ~WS_EX_TOOLWINDOW	);
#endif
}




//////////////////////////////////////////////////////////////////////////////////
//					Override system calls from 3rd parties
//////////////////////////////////////////////////////////////////////////////////


#if defined (Q_OS_LINUX)

#include <stdio.h>
#include <stdint.h>
#include <dlfcn.h>

void exit( int status )
{
    UNUSED( status );

    using exit_t = void (*)(int);
    static exit_t my_exit = NULL;
    printf("inside shared object...\n");
    if (!my_exit)
        my_exit = (exit_t)dlsym(RTLD_NEXT, "exit");  // returns lib function pointer

    throw CException("exit called");
}

#endif



////////////////////////////////////////////////
//
////////////////////////////////////////////////

#include "moc_BratApplication.cpp"
