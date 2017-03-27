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

#include <QLocalServer>
#include <QLocalSocket>

#include "new-gui/Common/QtUtils.h"
#include "new-gui/Common/System/OsProcess.h"

#include "simplecrypt.h"
#include "RadsService.h"
#include "RadsServiceLogger.h"
#include "RadsSharedParameters.h"



//#define WAIT_FOR_DEBUGGER

// Ensure that WAIT_FOR_DEBUGGER is always undefined in release builds
//
#if !defined (DEBUG) && !defined (_DEBUG)
#undef WAIT_FOR_DEBUGGER
#endif



//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//				Service Log Management
//
//		(must be called before any logging occurs;
//		should be called periodically after that)
//
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

bool InitializeServiceLogging( const std::string &log_path )
{
	static const auto log_to_file_env_var = CApplicationLoggerBase::LogToFileEnvVar().c_str();
	static const auto full_log_env_var = CApplicationLoggerBase::FullLogEnvVar().c_str();
	static const char *full_log_env_var_value = "1";

	// Ensure file logging and full log (as in debug mode), otherwise no log will be available
	//	because by design we do not want to capture signals from ProductionLog 

	bool log_ok = true;

	if ( qgetenv( log_to_file_env_var ).isEmpty() )
	{
		qputenv( log_to_file_env_var, log_path.c_str() );

		if ( qgetenv( log_to_file_env_var ).isEmpty() )
		{
			std::cerr << "Could not set " << log_to_file_env_var << std::endl;
			log_ok = false;
		}
		else
			std::cout << "Set " << log_to_file_env_var << " as " << log_path + " for logging to file." << std::endl;
	}


	if ( qgetenv( full_log_env_var ).isEmpty() )
	{
		qputenv( full_log_env_var, full_log_env_var_value );

		if ( qgetenv( full_log_env_var ).isEmpty() )
		{
			std::cerr << "Could not set " << full_log_env_var << std::endl;
			log_ok = false;
		}
		else
			std::cout << "Set " << full_log_env_var << " to " << full_log_env_var_value << " to enable log." << std::endl;
	}


	if ( !log_ok )
	{
		std::cerr << "Logging may not be available." << std::endl;
		return false;
	}

	return true;
}


void CheckServiceLogging( const std::string &log_path )
{
	static const bool log_ok = InitializeServiceLogging( log_path );	UNUSED( log_ok  );		//ignore, user was already warned in the console
	static const signed log_files_time_to_live = 10;
	static const signed log_files_size_to_backup = 10 * 1024 * 1024;

	// Log file (re)creation

	if ( IsFile( log_path ) )
	{
		QFileInfo info( log_path.c_str() );
		if ( info.size() > log_files_size_to_backup )
		{
			std::string old_path = CreateUniqueFileName( log_path );			//old_path is a new path, to save old contents
			if ( old_path.empty() || !DuplicateFile( log_path, old_path ) )		//save "old" file
				std::cerr << "Could not create new log file. Trying to use existing one." << std::endl;
			else
				RemoveFile( log_path );
		}
	}

	// Old log files cleanup

	QDir sourceDir( GetDirectoryFromPath( log_path ).c_str() );
	QString filter = ( GetBaseFilenameFromPath( log_path ) + ".*" ).c_str();
	QStringList files = sourceDir.entryList( QStringList() << filter, QDir::Files );
	QDateTime log_files_end_date = QDateTime::currentDateTime().addDays( -log_files_time_to_live );
	const std::string log_ext = GetLastExtensionFromPath( log_path );
	for ( auto &file : files )
	{
		if ( log_ext == GetLastExtensionFromPath( q2a( file ) ) )
			continue;

		file = sourceDir.absolutePath() + "/" + file;
		QFileInfo info( file );
		if ( info.lastModified() <= log_files_end_date )
		{
			RemoveFile( file );
		}
	}
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////
// temporary code/////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////
//						Password
//////////////////////////////////////////////////////////


static const quint64 default_key = 6396014356587936538;

inline QString encrypt( const QString &to_encrypt, quint64 key = default_key )
{
	SimpleCrypt processSimpleCrypt( key );
	return processSimpleCrypt.encryptToString( to_encrypt );
}
inline QString decrypt( const QString &to_decrypt, quint64 key = default_key )
{
	SimpleCrypt processSimpleCrypt( key );
	return processSimpleCrypt.decryptToString( to_decrypt );
}
/*
QString to_decrypt = encrypt( "rat@tu1" );
qDebug() << "Encrypted rads_password to" << to_decrypt;
qDebug() << "Decrypted rads_password to" << decrypt( to_decrypt );

const quint64 key = 89473829;
QString to_decrypt = encrypt( "rat@tu1", key );
qDebug() << "Encrypted rads_password to" << to_decrypt;
qDebug() << "Decrypted rads_password to" << decrypt( to_decrypt, key );
////Set The Encryption And Decryption Key
//SimpleCrypt processSimpleCrypt(89473829);
//QString rads_password("rat@tu1");
////Encrypt
//QString rads_password_encrypted = processSimpleCrypt.encryptToString( rads_password );
//qDebug() << "Encrypted rads_password to" << rads_password_encrypted;
////Decrypt
//QString decrypt = processSimpleCrypt.decryptToString( rads_password_encrypted );
//qDebug() << "Decrypted rads_password to" << decrypt;
*/



// temporary code/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//						RadsClient
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////


CRadsClient::CRadsClient( CRadsSettings &settings, QObject *parent )	//parent = nullptr 
	: base_t( parent )
	, mSettings( settings ) 
	, mTimer( this )
	, mDisabled( false )
	, mRadsServerAddress( ReadRadsServerAddress( mSettings.RadsPaths().mRadsConfigurationFilePath ) )
    //, mSharedMemory( RADS_SHARED_MEMORY_KEY.c_str(), this )
	, mSocketServer( new QLocalServer( this ) )
{
	//setup

	const int periodic_check_in_seconds = mSettings.PeriodicCheckInMinutes() * 60;

	mSocketServer->setSocketOptions( QLocalServer::WorldAccessOption );

	if ( mSocketServer->listen( RADS_SHARED_MEMORY_KEY.c_str() ) )
		connect( mSocketServer, SIGNAL( newConnection() ), this, SLOT( HandleNewConnection() ) );
	else
		LOG_WARN( "Unable to start the RADS socket server: " + mSocketServer->errorString() );

	connect( &mTimer, &QTimer::timeout, this, &CRadsClient::HandleSynchronize );
	mTimer.start( periodic_check_in_seconds * 1000 );


	//startup (starting paused only if PAUSE_ON_START defined for diagnostic)

#if !defined (WAIT_FOR_DEBUGGER) && defined(PAUSE_ON_START)

	Pause();

#endif

	QTimer::singleShot( 0, this, &CRadsClient::HandleSynchronize );
}


//virtual 
CRadsClient::~CRadsClient()
{
	if ( mCurrentProcess )
	{
		CleanRsyncProcess( true );
		LOG_INFO( "Rads client is being deleted. A request was sent for existing rsync process to terminate." );
	}

	for ( auto *connection : mSocketConnections )
	{
		if ( !connection->isValid() )
			continue;

		connection->disconnectFromServer();
	}
}


//////////////////////////////////////////////////////////
//	RadsClient - what all this is about
//////////////////////////////////////////////////////////

void CRadsClient::Pause()
{
	mDisabled = true;
}


void CRadsClient::Resume()
{
	LOG_INFO( "RadsClient: resuming." );
	mDisabled = false;
	Synchronize();
}


void CRadsClient::DelaySaveConfig()
{
	if ( mSettings.HasFileLocked() )
	{
		QTimer::singleShot( 3000, this, &CRadsClient::DelaySaveConfig );
		LOG_INFO( "Configuration file is locked. Delaying save." );
	}
	else
	if ( mSettings.SaveConfig() )
		BroadcastRsyncStatusToSocketClients( CONFIG_UPDATED_SIGN.c_str() );
	else
		LOG_WARN( "RadsClient: Could not save the configuration to file." );
}


//for easy parameterless slot connect
void CRadsClient::ForceSynchronize()
{
	Synchronize( true );
}


void CRadsClient::ForceRsyncEnd()
{
	if ( mCurrentProcess )
	{
		//"Console applications on Windows that do not run an event loop, or whose event 
		//loop does not handle the WM_CLOSE message, can only be terminated by calling kill()."
		//
		LOG_INFO( "Received request to terminate any current synchronization." );
#if defined (Q_OS_MAC)
        mCurrentProcess->terminate();
#else
        mCurrentProcess->Kill();
#endif
		LOG_INFO( "The request to terminate any current synchronization was processed." );
	}
}




template< typename STRING >
inline STRING QuotePath( const STRING &path )
{
	return "\"" + path + "\"";
}


bool CRadsClient::Synchronize( bool force )		//force = false 
{
	// ensure log is prepared

	CheckServiceLogging( mSettings.LogFilePath() );

	// ensure schedule

	bool need_save_config = mSettings.CorrectLastSyncTime();					assert__( !need_save_config );
	if ( need_save_config )
	{
		LOG_WARN( "Unexpected invalid synchronization time. It was corrected." );
	}

	if ( force )
	{
		LOG_INFO( "Trying to start immediate data synchronization with RADS." );
		need_save_config = mSettings.SetNextSyncDateToday() || need_save_config;
	}

	// ensure updated settings in file, if previous steps changed them

	if ( need_save_config )
		DelaySaveConfig();


	// check if it is not time to synchronize or it should otherwise be skipped

	if ( QDate::currentDate() < mSettings.NextSyncDate() )
	{
		return true;
	}
    else
    if ( !force )
    {
        LOG_INFO( "Trying to start scheduled data synchronization with RADS." );
    }

	bool disable = !force && mDisabled;
	if ( mCurrentProcess || disable || !IsDir( mSettings.DownloadDirectory() ) || mSettings.MissionNames().size() == 0 )
	{
		std::string msg = "Skipping one data synchronization event. Reason(s):\n";
		if ( mCurrentProcess )
			msg += "-An rsync process is still executing.\n";
		if ( disable )
			msg += "-The service was temporarily paused.\n";
		if ( mSettings.DownloadDirectory().empty() )
			msg += "-No output directory was specified.\n";
		if ( !IsDir( mSettings.DownloadDirectory() ) )
			msg += "-Output directory does not exist.\n";
		if ( mSettings.MissionNames().size() == 0 )             //see (*) below before changing
			msg += "-No missions were specified.\n";

		LOG_INFO( msg );
		return false;
	}


	// time to synchronize...


	//return BroadcastRsyncStatusToSocketClients();


    const std::string pass_file_path( mSettings.RadsPaths().mInternalDataDir + "/rads_pass.txt" );
    QFile pass_file( pass_file_path.c_str() );
    pass_file.setPermissions( QFile::ReadOwner | QFile::WriteOwner );

	std::string cmd_line = QuotePath( mSettings.RadsPaths().mRsyncExecutablePath.mPath );
	cmd_line += " ";
	cmd_line += "-avrzR --del --password-file=";
    cmd_line += QuotePath( win2cygwin( pass_file_path ) );		//TODO encrypt/decode: write unencrypted to temporary file passed here
	cmd_line += " ";
	cmd_line += "--timeout=";
	cmd_line += n2s<std::string>( mSettings.TimeoutInSeconds() );
	cmd_line += " ";
	std::string src_missions;
    
    //Assuming mSettings.MissionNames() is not empty: //see (*) above before changing
    
#if defined(IMPL_WIN)
    
	for ( auto const &mission : mSettings.MissionNames() )
	{
		src_missions += ( mRadsServerAddress + "/" + mission + " " );
	}
    
#else
    
    auto v = String2Vector( mRadsServerAddress, std::string( "::" ) );
    src_missions += v[0] + "::\"";
    for ( auto const &mission : mSettings.MissionNames() )
	{
		src_missions += ( v[1] + "/" + mission + " " );
	}
    src_missions.back() = '\"';

#endif    
    
    cmd_line += src_missions;
	cmd_line += " ";
	cmd_line += QuotePath( win2cygwin( mSettings.DownloadDirectory() ) );

	const bool sync = false;
	mCurrentProcess = new COsProcess( sync, "", this, cmd_line );
	connect( mCurrentProcess, SIGNAL( readyReadStandardOutput() ),				this, SLOT( HandleUpdateOutput() ) );
	connect( mCurrentProcess, SIGNAL( readyReadStandardError() ),				this, SLOT( HandleUpdateOutput() ) );
	connect( mCurrentProcess, SIGNAL( finished( int, QProcess::ExitStatus ) ),	this, SLOT( HandleProcessFinished( int, QProcess::ExitStatus ) ) );
	connect( mCurrentProcess, SIGNAL( error( QProcess::ProcessError ) ),		this, SLOT( HandleRsyncError( QProcess::ProcessError ) ) );
	BroadcastRsyncStatusToSocketClients( RSYNC_RUNNING_SIGN.c_str() );
	mCurrentProcess->Execute();

	const QString msg = "A RADS data synchronization request was processed.\nCommand line: " + t2q( cmd_line );
	QtServiceBase::instance()->logMessage( msg );
	LOG_INFO( msg );
	DelaySaveConfig();

	return true;
}



//////////////////////////////////////////////////////////
//	RadsClient - rsync process handling
//////////////////////////////////////////////////////////


////////////////////////////////
// socket notifications handling
////////////////////////////////

void CRadsClient::HandleNewConnection()
{
	QLocalSocket *connection = mSocketServer->nextPendingConnection();
	mSocketConnections.push_back( connection );

	connect( connection, SIGNAL( disconnected() ), this, SLOT( HandleSocketDisconnected() ) );
	//connect( connection, SIGNAL( disconnected() ), connection, SLOT( deleteLater() ) );

	//new client needs immediate knowledge of current status
	//
	BroadcastRsyncStatusToSocketClients( mCurrentProcess ? RSYNC_RUNNING_SIGN.c_str() : RSYNC_STOPPED_SIGN.c_str() );

	LOG_WARN( "New socket connection received. " + connection->fullServerName() );
}


void CRadsClient::HandleSocketDisconnected()
{
	for ( auto it = mSocketConnections.begin(); it != mSocketConnections.end();  )
	{
		QLocalSocket *connection = *it;
		if ( connection->isValid() )
		{
			++it;
		}
		else
		{
			it = mSocketConnections.erase( it );	// it remains valid, pointing to next item
			connection->deleteLater();
			LOG_WARN( "Erased socket connection reference. " + connection->fullServerName() );
		}
	}
}


//slot
bool CRadsClient::BroadcastRsyncStatusToSocketClients( const char *msg )
{
	static bool test = false;

	if ( mSocketConnections.empty() )
		return true;

	if ( mBroadcasting )	//this should never happen; mBroadcasting is more a development device than a real sync. mechanism
	{
		QTimer::singleShot( 5000, this, [this, msg]() 
		{
			BroadcastRsyncStatusToSocketClients( msg );
		} );

		LOG_WARN( "Delaying socket notifications." );
		return false;
	}

	//test = !test;

    LOG_INFO( QString( "Broadcasting status..." ) + msg );

	mBroadcasting = true;

	QByteArray block;
	QDataStream out( &block, QIODevice::WriteOnly );
	out.setVersion( QDataStream::Qt_4_0 );
	out << (quint16)0;
	out << QString( msg );			   //QString, not raw char*, is critical
	out.device()->seek( 0 );
	out << (quint16)( block.size() - sizeof(quint16) );

    size_t need_cleanup = 0;		//apparently, a remote disconnection does not emit remotely...
    size_t notifications_sent = 0;
    for ( auto *connection : mSocketConnections )
	{
		if ( !connection->isValid() )
		{
            need_cleanup++;
			continue;
		}

		connection->write( block );
		connection->flush();
		//connection->disconnectFromServer();

        notifications_sent++;
    }

    LOG_INFO( "Number of connections notified == " + n2q( notifications_sent ) );

    if ( need_cleanup )
    {
        LOG_INFO( n2q( need_cleanup ) + " stale connections found: scheduling clean-up." );
        QTimer::singleShot( 5000, this, &CRadsClient::HandleSocketDisconnected );
    }

	mBroadcasting = false;

	LOG_INFO( "Finished broadcasting status." );

	return true;
}


//////////////////////
// cleanup helper
//////////////////////

void CRadsClient::CleanRsyncProcess( bool kill )		//kill = false 
{
	if ( mCurrentProcess )
	{
		if ( kill )
			mCurrentProcess->Kill();
		mCurrentProcess->deleteLater();
	}
	mCurrentProcess = nullptr;
	BroadcastRsyncStatusToSocketClients( RSYNC_STOPPED_SIGN.c_str() );
}


//////////////////////
// rsync process slots
//////////////////////

//slot
void CRadsClient::HandleRsyncError( QProcess::ProcessError error )
{
    auto message = "An error occurred with " + mSettings.RadsPaths().mRsyncExecutablePath.mPath + "\n" + q2a( COsProcess::ProcessErrorMessage( error ) );
	QtServiceBase::instance()->logMessage( message.c_str() );
	LOG_FATAL( message.c_str() );
	CleanRsyncProcess();		//TODO consider removing this: apparently, HandleProcessFinished is always called, even when HandleRsyncError is called
}


//slot
void CRadsClient::HandleProcessFinished( int exit_code, QProcess::ExitStatus exitStatus )
{
    // data & lambdas: rsync exit codes translation mechanism 
    
    struct 
    {
        const int code;
        const std::string msg;        
    }  
    rsync_exit_codes[] =
    {
        0,      "Success",
        1,      "Syntax or usage error",
        2,      "Protocol incompatibility",
        3,      "Errors selecting input/output files, dirs",
        4,      "Requested  action not supported: an attempt was made to manipulate 64-bit files on a platform "
        "that cannot support them; or an option was specified that is supported by the client and not by the server.",
        5,      "Error starting client-server protocol",
        6,      "Daemon unable to append to log-file",
        10,     "Error in socket I/O",
        11,     "Error in file I/O",
        12,     "Error in rsync protocol data stream",
        13,     "Errors with program diagnostics",
        14,     "Error in IPC code",
        20,     "Received SIGUSR1 or SIGINT",
        21,     "Some error returned by waitpid()",
        22,     "Error allocating core memory buffers",
        23,     "Partial transfer due to error",
        24,     "Partial transfer due to vanished source files",
        25,     "The --max-delete limit stopped deletions",
        30,     "Timeout in data send/receive",
        35,      "Timeout waiting for daemon connection"
    };
    
    
    const DEFINE_ARRAY_SIZE( rsync_exit_codes );
    
    
    auto translate_code = [&rsync_exit_codes, &rsync_exit_codes_size](int code ) -> std::string
    {
        std::string s = n2s<std::string>( code );
        
        for ( size_t i = 0; i <  rsync_exit_codes_size; ++i )
            if ( rsync_exit_codes[i].code == code )
                return s += ( "-" + rsync_exit_codes[i].msg );
        
        return s;
    };
    
    
    // function body
    
    COsProcess *process = qobject_cast<COsProcess*>( sender() );	Q_UNUSED(process);		assert__( process && ( !mCurrentProcess || mCurrentProcess == process ) );

	if (exitStatus == QProcess::CrashExit) 
	{
		const QString msg = "program crash";
		QtServiceBase::instance()->logMessage( msg );
		LOG_FATAL( msg );
	} 
	else if ( exit_code != 0 )	//this is ExitStatus::NormalExit, although error code != 0
	{
		const QString msg = t2q( "exit code " + translate_code( exit_code ) );
		QtServiceBase::instance()->logMessage( msg );
		LOG_WARN( msg );
	} 
	else 
	{
        mSettings.SetLastSyncTimeNow();
		auto next = mSettings.NextSyncDate();
		const QString msg = 
            "A RADS data synchronization request was successfully processed.\nNext synchronization scheduled for "
            + next.toString( CRadsSettingsBase::RsyncDateFormat() );
        QtServiceBase::instance()->logMessage( msg );
        LOG_INFO( msg );
    
        DelaySaveConfig();
	}

	CleanRsyncProcess();
}


void CRadsClient::HandleUpdateOutput()
{
	COsProcess *process = qobject_cast<COsProcess*>( sender() );			assert__( process && mCurrentProcess == process );

	// log errors to system logs
	//
	QString text = process->readAllStandardError();
	if ( !text.isEmpty() )
	{
		QtServiceBase::instance()->logMessage( text );
		LOG_WARN( text );
	}

	//don't flood system logs with rsync output
	//
	text = process->readAllStandardOutput();
	if ( !text.isEmpty() )
	{
		LOG_INFO( text );
	}
}





//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//						RadsService
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

CRadsService::CRadsService( int argc, char *argv[], const CApplicationStaticPaths &paths, bool auto_start )
    : base_t( argc, argv, RADS_SERVICE_NAME )
	, mSettings( paths )
{
	LOG_INFO( "Creating a service object. Configuration file: " + mSettings.FilePath() );

	setServiceDescription( "Synchronizes data between RADS sever and local BRAT data repository" );
	setServiceFlags( QtServiceBase::CanBeSuspended );
	setStartupType( auto_start ? QtServiceController::StartupType::AutoStartup : QtServiceController::StartupType::ManualStartup );

	LOG_INFO( "Created a service object." );
}


//virtual 
CRadsService::~CRadsService()
{
	if ( mRadsClient )
	{
		delete mRadsClient;
		LOG_WARN( "Deleted rads client object in service destructor." );
	}
}


//virtual 
int CRadsService::exec()
{
	LOG_INFO( "Calling CRadsService::base_t::exec()" );
	return base_t::exec();
}

#ifndef _WIN32
    //#include <stdio.h>
    //#include <unistd.h>

    bool RunningUnderDebugger() //prevents debugger from attaching
    {
//        FILE *fd = fopen("/tmp", "r");
//        bool under = fileno(fd) > 5;
//        fclose(fd);
//        return under;
        return false;
    }
#else
    bool RunningUnderDebugger()
    {
         return IsDebuggerPresent() == TRUE;
    }
#endif


//virtual 
void CRadsService::start()
{

#if defined (WAIT_FOR_DEBUGGER)
#if defined (DEBUG) || defined (_DEBUG)
#if defined (_MSC_VER)
	LOG_INFO( "Waiting for debugger to attach..." );
    while ( !RunningUnderDebugger() ) QBratThread::sleep( 10 );
	LOG_INFO( "Debugger attached." );
#else
    //while ( !RunningUnderDebugger() ) QBratThread::sleep( 10 );
    QBratThread::sleep( 15 );
#endif
#endif
#endif

	bool config_result = false;
	if ( IsFile( mSettings.FilePath() ) )
		config_result = mSettings.LoadConfig();
	else
		config_result = mSettings.SaveConfig();		//save defaults for brat to read
	if ( !config_result )
		LOG_WARN( "An error occurred accessing the configuration file." );

	QCoreApplication *app = application();

	LOG_INFO( "Entering CRadsService::start()" );

	if ( mRadsClient )
	{
		delete mRadsClient;
		LOG_WARN( "Deleted rads client object before starting." );
	}

	mRadsClient = new CRadsClient( mSettings, app );

	if ( !mRadsClient->IsListening() )
	{
		const QString msg = "Failed to setup rads client. Quiting...";
		logMessage( msg, QtServiceBase::Error );
		LOG_FATAL( msg );
		app->quit();
		return;
	}

    const bool auto_start = startupType() == QtServiceController::StartupType::AutoStartup;
    const std::string startup_type = auto_start ? "auto-start": "manual";
    LOG_INFO( "Service start-up type: " + startup_type + "." );
    if ( auto_start )
    {
        const std::string auto_start_enabled = AutoStartEnabled() ? "enabled": "disabled";
        LOG_INFO( "Auto-start is " + auto_start_enabled + "." );
    }
    
    
	LOG_INFO( "Leaving CRadsService::start() in expected return point." );
}


//virtual 
void CRadsService::stop()
{
	LOG_INFO( "Stop system command received." );
	delete mRadsClient;
	mRadsClient = nullptr;	
	base_t::stop();		//only because; it does nothing
}


//virtual 
void CRadsService::pause()
{
	QTimer::singleShot( 200, mRadsClient, &CRadsClient::Pause );
}


//virtual 
void CRadsService::resume()
{
	QTimer::singleShot( 200, mRadsClient, &CRadsClient::Resume );
}


//virtual 
void CRadsService::processCommand( int code )
{
	base_t::processCommand( code );			//just because; it does nothing

	switch ( code )
	{
		case eRadsService_ExecNow:
		{
			QTimer::singleShot( 200, mRadsClient, &CRadsClient::ForceSynchronize );
		}
		break;

		case eRadsService_StopExec:
		{
			QTimer::singleShot( 200, mRadsClient, &CRadsClient::ForceRsyncEnd );
		}
		break;

		case eRadsService_LockConfig:
		{
			mSettings.LockFile( true );
		}
		break;

		case eRadsService_UnlockConfig:
		{
			if ( !mSettings.LockFile( false ) )	//reloads parameters
                LOG_WARN( "Error reading RADS service configuration while processing the unlock command." );
		}
		break;

		default:
		{
			LOG_WARN( "The service received an INAVALID command code." );
		}
	}
}


//virtual 
void CRadsService::logMessage( const QString &message, MessageType type, int id, uint category, const QByteArray &data )		//type = Success, int id = 0, uint category = 0, const QByteArray &data = QByteArray()
{
	base_t::logMessage( message, type, id, category, data );
	LOG_INFO( "Service framework logMessage: " + message );
}




//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//						main
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

//#define LOCAL_DEBUG



int main( int argc, char *argv[] )
{
#if defined (LOCAL_DEBUG)	

	qDebug() << "Decrypted rads_password to" << decrypt( "AwKel0QzUiZmEkIw", 89473829 );				//AwKqGyUhayqpheog or AwIfrpCU3p8cMF+V with default_key?
	qDebug() << "Encrypted rads_password to" << encrypt( "rat@tu1", 89473829 );
	qDebug() << "Encrypted rads_password to" << encrypt( "rat@tu1", 89473829 );
	QString to_decrypt = encrypt( "rat@tu1", 89473829 );				//AwKqGyUhayqpheog with default_key
	qDebug() << "Encrypted rads_password to" << to_decrypt;
	qDebug() << "Decrypted rads_password to" << decrypt( to_decrypt, 89473829 );

#if !defined(Q_OS_WIN)
    // QtService stores service settings in SystemScope, which normally require root privileges.
    // To allow testing this example as non-root, we change the directory of the SystemScope settings file.
    QSettings::setPath(QSettings::NativeFormat, QSettings::SystemScope, QDir::tempPath());
    qWarning("(Example uses dummy settings file: %s/QtSoftware.conf)", QDir::tempPath().toLatin1().constData());
#endif
#endif

	// I. Application Paths

	const CApplicationStaticPaths brat_paths( argv[ 0 ], RADS_SERVICE_NAME );


	// II. Application Settings

	CRadsSettings settings( brat_paths );

	bool config_result = false;
	if ( IsFile( settings.FilePath() ) )
		config_result = settings.LoadConfig();
	else
		config_result = settings.SaveConfig();		//save defaults for brat to read


	// III. Application Log. Report starting conditions

	const std::string log_path = config_result ? settings.LogFilePath() : brat_paths.mRadsServicePanicLogFilePath.mPath;
	CheckServiceLogging( log_path );

	LOG_INFO( "" );		LOG_INFO( " *** Logging started *** " );

	if ( !config_result )
	{
		const std::string msg = "An error occurred accessing the configuration file.";
		LOG_WARN( msg );
		std::cerr << msg << std::endl;	//if log file could not be created, output also to console
	}

	std::string msg =  "Configuration file " + settings.FilePath() + ". Log messages will be written to " + log_path;
	LOG_INFO( msg );
	std::cout << msg << std::endl;

	std::ostringstream os;
	os << settings;
	msg = "Initial configuration:\n" + os.str();
	LOG_INFO( msg );
	std::cout << msg << std::endl;


	// IV. Service

#if defined (LOCAL_DEBUG)	
	QCoreApplication service( argc, argv );
	new CRadsClient( brat_paths );
	return service.exec();
#else

	CRadsService service( argc, argv, brat_paths, true );		//auto start
	return service.exec();

#endif
}




///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_RadsService.cpp"
