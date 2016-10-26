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

#include "new-gui/Common/QtUtils.h"
#include "new-gui/Common/QtUtilsIO.h"
#include "new-gui/Common/System/OsProcess.h"

#include "simplecrypt.h"
#include "RadsService.h"
#include "RadsServiceLogger.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// temporary code/////////////////////////////////////////////////////////////////////////////////////////

int processArgs(int argc, char **argv)
{
	if (argc > 2) {
		QString arg1(argv[1]);
		if (arg1 == QLatin1String("-i") ||
			arg1 == QLatin1String("-install")) {
			if (argc > 2) {
				QString account;
				QString password;
				QString path(argv[2]);
				if (argc > 3)
					account = argv[3];
				if (argc > 4)
					password = argv[4];
				printf("The service %s installed.\n",
					(QtServiceController::install(path, account, password) ? "was" : "was not"));
				return 0;
			}
		} else {
			QString serviceName(argv[1]);
			QtServiceController controller(serviceName);
			QString option(argv[2]);
			if (option == QLatin1String("-u") ||
				option == QLatin1String("-uninstall")) {
				printf("The service \"%s\" %s uninstalled.\n",
					controller.serviceName().toLatin1().constData(),
					(controller.uninstall() ? "was" : "was not"));
				return 0;
			} else if (option == QLatin1String("-s") ||
				option == QLatin1String("-start")) {
				QStringList args;
				for (int i = 3; i < argc; ++i)
					args.append(QString::fromLocal8Bit(argv[i]));
				printf("The service \"%s\" %s started.\n",
					controller.serviceName().toLatin1().constData(),
					(controller.start(args) ? "was" : "was not"));
				return 0;
			} else if (option == QLatin1String("-t") ||
				option == QLatin1String("-terminate")) {
				printf("The service \"%s\" %s stopped.\n",
					controller.serviceName().toLatin1().constData(),
					(controller.stop() ? "was" : "was not"));
				return 0;
			} else if (option == QLatin1String("-p") ||
				option == QLatin1String("-pause")) {
				printf("The service \"%s\" %s paused.\n",
					controller.serviceName().toLatin1().constData(),
					(controller.pause() ? "was" : "was not"));
				return 0;
			} else if (option == QLatin1String("-r") ||
				option == QLatin1String("-resume")) {
				printf("The service \"%s\" %s resumed.\n",
					controller.serviceName().toLatin1().constData(),
					(controller.resume() ? "was" : "was not"));
				return 0;
			} else if (option == QLatin1String("-c") ||
				option == QLatin1String("-command")) {
				if (argc > 3) {
					QString codestr(argv[3]);
					int code = codestr.toInt();
					printf("The command %s sent to the service \"%s\".\n",
						(controller.sendCommand(code) ? "was" : "was not"),
						controller.serviceName().toLatin1().constData());
					return 0;
				}
			} else if (option == QLatin1String("-v") ||
				option == QLatin1String("-version")) {
				bool installed = controller.isInstalled();
				printf("The service\n"
					"\t\"%s\"\n\n", controller.serviceName().toLatin1().constData());
				printf(" and %s\n\n", (controller.isRunning() ? "running" : "not running"));
				if (installed) {
					printf("path: %s\n", controller.serviceFilePath().toLatin1().data());
					printf("description: %s\n", controller.serviceDescription().toLatin1().data());
					printf("startup: %s\n", controller.startupType() == QtServiceController::AutoStartup ? "Auto" : "Manual");
				}
				return 0;
			}
		}
	}
	printf("controller [-i PATH | SERVICE_NAME [-v | -u | -s | -t | -p | -r | -c CODE] | -h] [-w]\n\n"
		"\t-i(nstall) PATH\t: Install the service\n"
		"\t-v(ersion)\t: Print status of the service\n"
		"\t-u(ninstall)\t: Uninstall the service\n"
		"\t-s(tart)\t: Start the service\n"
		"\t-t(erminate)\t: Stop the service\n"
		"\t-p(ause)\t: Pause the service\n"
		"\t-r(esume)\t: Resume the service\n"
		"\t-c(ommand) CODE\t: Send a command to the service\n"
		"\t-h(elp)\t\t: Print this help info\n"
		"\t-w(ait)\t\t: Wait for keypress when done\n");
	return 0;
}

int main_example(int argc, char **argv)
{
#if !defined(Q_OS_WIN)
	// QtService stores service settings in SystemScope, which normally require root privileges.
	// To allow testing this example as non-root, we change the directory of the SystemScope settings file.
	QSettings::setPath(QSettings::NativeFormat, QSettings::SystemScope, QDir::tempPath());
	qWarning("(Example uses dummy settings file: %s/QtSoftware.conf)", QDir::tempPath().toLatin1().constData());
#endif

	int result = processArgs(argc, argv);

	if (QString::fromLocal8Bit(argv[argc-1]) == QLatin1String("-w") ||
		QString::fromLocal8Bit(argv[argc-1]) == QLatin1String("-wait")) {
		printf("\nPress Enter to continue...");
		QFile input;
		input.open(stdin, QIODevice::ReadOnly);
		input.readLine();
		printf("\n");
	}

	return result;
}

// temporary code/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//						Password
//////////////////////////////////////////////////////////
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



//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//						RadsService
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////


void CRadsService::start()
{
	DEBUG_BREAK;

	QCoreApplication *app = application();

	if ( mDaemon )
	{
		delete mDaemon;
		LOG_WARN( "Deleted existing rads-client daemon before starting." );
	}

	mDaemon = new CRadsDaemon( mPaths, app );

	if (!mDaemon->IsListening() )
	{
		const QString msg = "Failed to setup rads-client daemon. Quiting...";
		logMessage( msg, QtServiceBase::Error );
		LOG_FATAL( msg );
		app->quit();
	}
}




//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//						RadsFaemon
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

//virtual 
CRadsDaemon::~CRadsDaemon()
{
	if ( mCurrentProcess )
	{
		mCurrentProcess->Kill();
		mCurrentProcess->deleteLater();
		LOG_INFO( "Rads client is being deleted. A request was sent for existing rsync process to terminate." );
	}
}


void CRadsDaemon::RsyncError( QProcess::ProcessError error )
{
	auto message = "An error occurred launching " + mPaths.mRsyncExecutablePath + "\n" + q2a( COsProcess::ProcessErrorMessage( error ) );
	QtServiceBase::instance()->logMessage( message.c_str() );
	LOG_FATAL( message.c_str() );
}


void CRadsDaemon::HandleProcessFinished( int exit_code, QProcess::ExitStatus exitStatus )
{
	COsProcess *process = qobject_cast<COsProcess*>( sender() );			assert__( process && mCurrentProcess == process );

	if (exitStatus == QProcess::CrashExit) 
	{
		const QString msg = "program crash";
		QtServiceBase::instance()->logMessage( msg );
		LOG_FATAL( msg );
	} 
	else if ( exit_code != 0 )	//this is ExitStatus::NormalExit, although error code != 0
	{
		const QString msg = t2q( "exit code " + n2s<std::string>( exit_code ) );
		QtServiceBase::instance()->logMessage( msg );
		LOG_WARN( msg );
	} 
	else 
	{
		LOG_INFO( "success" );
	}

	mCurrentProcess->deleteLater();
	mCurrentProcess = nullptr;
}


void CRadsDaemon::HandleUpdateOutput()
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


bool CRadsDaemon::Synchronize()
{
	if ( mCurrentProcess || mDisabled )
		return false;

	//September 2016:
	//server: corads.tudelft.nl
	//user: radsuser
	//pass. rat@tu1

	std::string cmd_line = "\"" + mPaths.mRsyncExecutablePath + "\"";
	cmd_line += " ";
	cmd_line += "-avr --password-file=";
	cmd_line += win2cygwin( mPaths.mInternalDataDir + "/rads_pass.txt" );		//TODO encrypt/decode: write unencrypted to temporary file passed here
	cmd_line += " ";
	cmd_line += "--del radsuser@corads.tudelft.nl::rads/data/j2/a/c000";		//TODO get from configuration
	cmd_line += " ";
	cmd_line += win2cygwin( mPaths.mInternalDataDir + "/j2" );					//TODO get from configuration

	const bool sync = false;
	mCurrentProcess = new COsProcess( sync, "", this, cmd_line );
	connect( mCurrentProcess, SIGNAL( readyReadStandardOutput() ),				this, SLOT( HandleUpdateOutput() ) );
	connect( mCurrentProcess, SIGNAL( readyReadStandardError() ),				this, SLOT( HandleUpdateOutput() ) );
	connect( mCurrentProcess, SIGNAL( finished( int, QProcess::ExitStatus ) ),	this, SLOT( HandleProcessFinished( int, QProcess::ExitStatus ) ) );
	connect( mCurrentProcess, SIGNAL( error( QProcess::ProcessError ) ),		this, SLOT( RsyncError( QProcess::ProcessError ) ) );
	mCurrentProcess->Execute();

	const QString msg = "A RADS data synchronization request was processed.";
	QtServiceBase::instance()->logMessage( msg );
	LOG_INFO( msg );

	return true;
}



//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//						main
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

//#define LOCAL_DEBUG


int main( int argc, char *argv[] )
{
	DEBUG_BREAK;

#if defined (LOCAL_DEBUG)	

	qDebug() << "Decrypted rads_password to" << decrypt( "AwKel0QzUiZmEkIw", 89473829 );				//AwKqGyUhayqpheog or AwIfrpCU3p8cMF+V with default_key?
	qDebug() << "Encrypted rads_password to" << encrypt( "rat@tu1", 89473829 );
	qDebug() << "Encrypted rads_password to" << encrypt( "rat@tu1", 89473829 );
	QString to_decrypt = encrypt( "rat@tu1", 89473829 );				//AwKqGyUhayqpheog with default_key
	qDebug() << "Encrypted rads_password to" << to_decrypt;
	qDebug() << "Decrypted rads_password to" << decrypt( to_decrypt, 89473829 );
#endif

#if !defined(Q_OS_WIN)
	// QtService stores service settings in SystemScope, which normally require root privileges.
	// To allow testing this example as non-root, we change the directory of the SystemScope settings file.
	QSettings::setPath(QSettings::NativeFormat, QSettings::SystemScope, QDir::tempPath());
	qWarning("(Example uses dummy settings file: %s/QtSoftware.conf)", QDir::tempPath().toLatin1().constData());
#endif

	const CApplicationStaticPaths brat_paths( argv[ 0 ], RADS_SERVICE_NAME );

	const std::string log_path = brat_paths.mExecutableDir + "/RadsServiceLog.txt";
	qputenv( "QGIS_LOG_FILE", log_path.c_str() );

	if ( IsFile( log_path ) )
	{
		QFileInfo info( log_path.c_str() );
		if ( info.size() > 10000 )
		{
			std::string old_path = CreateUniqueFileName( log_path );			//old_path is a new path, to save old contents
			if ( old_path.empty() || !DuplicateFile( log_path, old_path ) )		//save "old" file
				LOG_WARN( "Could not create new log file. Trying to use existing one." );
			else
				RemoveFile( log_path );
		}
	}


#if defined (LOCAL_DEBUG)	
	QCoreApplication service( argc, argv );
	new CRadsDaemon( brat_paths );
	return service.exec();
#else

	CRadsService service( argc, argv, brat_paths );
	return service.exec();
#endif
}




///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_RadsService.cpp"
