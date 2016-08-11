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

#include "new-gui/Common/+Utils.h"
#include "ApplicationLogger.h"

// InternalOutputStream is the destination output stream of CApplicationLoggerBase; it abouts to 
//	nothing in release builds. In debug builds, abouts to qDebug, or to file if QGIS_LOG_FILE 
//	is not empty.
//
// InternalOutputStream is a streamlined copy of QgsLogger, for applications that do not use QGIS
//	but where some logging uniformity with it is required.
//
// Define QGISDEBUG (typically, when DEBUG is defined) to activate logging, through macro 
//	InternalDebugMsg (sDebugLevel is assigned 1; if sDebugLevel is 0 or the message debug 
//	level is greater than sDebugLevel no logging is done).
// This is done in stdafx.h of both brat's GUI applications.

//Comments of the original class QgsLogger:
//
// QgsLogger is a class to print debug/warning/error messages to the console.
// The advantage of this class over iostream & co. is that the
// output can be controlled with environment variables:
// QGIS_DEBUG is an int describing what debug messages are written to the console.
// If the debug level of a message is <= QGIS_DEBUG, the message is written to the
// console. It the variable QGIS_DEBUG is not defined, it defaults to 1 for debug
// mode and to 0 for release mode
// QGIS_DEBUG_FILE may contain a file name. Only the messages from this file are
// printed (provided they have the right debuglevel). If QGIS_DEBUG_FILE is not
// set, messages from all files are printed
//
// QGIS_LOG_FILE may contain a file name. If set, all messages will be appended
// to this file rather than to stdout.
//
class InternalOutputStream
{
	// current debug level 
	static int sDebugLevel;
	static QString sLogFile;
	static QString sFileFilter;
	static QTime sTime;

	static void init()
	{
		if ( sDebugLevel != -999 )
			return;

		sTime.start();

		sLogFile = getenv( "QGIS_LOG_FILE" ) ? getenv( "QGIS_LOG_FILE" ) : "";
		sFileFilter = getenv( "QGIS_DEBUG_FILE" ) ? getenv( "QGIS_DEBUG_FILE" ) : "";
		sDebugLevel = getenv( "QGIS_DEBUG" ) ? atoi( getenv( "QGIS_DEBUG" ) ) :
#ifdef QGISDEBUG
			1
#else
			0
#endif
			;
	}

public:
	//Goes to qDebug.
	//@param msg the message to be printed
	//@param debuglevel
	//@param file file name where the message comes from
	//@param function function where the message comes from
	//@param line place in file where the message comes from
	static void debug( const QString& msg, int debuglevel = 1, const char* file = nullptr, const char* function = nullptr, int line = -1 )
	{
		init();

		if ( !file && !sFileFilter.isEmpty() && !sFileFilter.endsWith( file ) )
			return;

		if ( sDebugLevel == 0 || debuglevel > sDebugLevel )
			return;

		QString m = msg;
		if ( file )
		{
			if ( qApp && qApp->thread() != QThread::currentThread() )
			{
				m.prepend( QString( "[thread:0x%1] " ).arg( (qint64)QThread::currentThread(), 0, 16 ) );
			}

			m.prepend( QString( "[%1ms] " ).arg( sTime.elapsed() ) );
			sTime.restart();

			if ( function )
			{
				m.prepend( QString( " (%1) " ).arg( function ) );
			}

			if ( line != -1 )
			{
#ifndef _MSC_VER
				m.prepend( QString( ": %1:" ).arg( line ) );
#else
				m.prepend( QString( "(%1) :" ).arg( line ) );
#endif
			}

			m.prepend( file );
		}

		if ( sLogFile.isEmpty() )
		{
			qDebug( "%s", m.toLocal8Bit().constData() );
		}
		else
		{
			logMessageToFile( m );
		}
	}


    // Reads the environment variable QGIS_DEBUG and converts it to int. If QGIS_DEBUG is not set,
    // the function returns 1 if QGISDEBUG is defined and 0 if not
    static int debugLevel() { init(); return sDebugLevel; }

    static void setDebugLevel( int level ) { init(); sDebugLevel = level; }	//NOTE this is not in the original QGIS class

	// Logs the message passed in to the logfile defined in QGIS_LOG_FILE if any. *
	static void logMessageToFile( QString theMessage )
	{
		if ( sLogFile.isEmpty() )
			return;

		//Maybe more efficient to keep the file open for the life of qgis...
		QFile file( sLogFile );
		if ( !file.open( QIODevice::Append ) )
			return;
		file.write( theMessage.toLocal8Bit().constData() );
		file.write( "\n" );
		file.close();
	}
};

int InternalOutputStream::sDebugLevel = -999; // undefined value
QString InternalOutputStream::sFileFilter;
QString InternalOutputStream::sLogFile;
QTime InternalOutputStream::sTime;


#ifdef QGISDEBUG
#define InternalDebugMsg(str) InternalOutputStream::debug( QString(str), 1, __FILE__, __FUNCTION__, __LINE__ )
#else
#define InternalDebugMsg(str)
#endif



/////////////////////////////////////////////////////////////////////////////////////////////////////////
//										Application Logger
/////////////////////////////////////////////////////////////////////////////////////////////////////////


CApplicationLoggerBase *CApplicationLoggerBase::smInstance = nullptr;


//
//
void CApplicationLoggerBase::TraceWrite( const QString& msg, int debuglevel, const char* file, const char* function, int line ) //debuglevel = 1, const char* file = nullptr, const char* function = nullptr, int line = -1 
{
	InternalOutputStream::debug( msg, debuglevel, file, function, line );
}



// This is what Qt does in systems where it does not assign a specific handler
//
void DefaultQtMessageHandler( QtMsgType type, const char *msg )
{
    Q_UNUSED( type );

    fprintf( stderr, "%s\n", msg );
    fflush( stderr );
}


//static 
QtMsgHandler CApplicationLoggerBase::InstallQtMsgHandler( QtMsgHandler handler )
{
	QtMsgHandler original_handler = qInstallMsgHandler( handler );
    if (!original_handler)
        original_handler = &DefaultQtMessageHandler;

	return original_handler;
}



CApplicationLoggerBase::CApplicationLoggerBase() 
	: base_t()

	, mQtHandler( InstallQtMsgHandler( MessageOutputProxy ) )

	, mLogMsg( getenv( "QGIS_DEBUG" ) ? &CApplicationLoggerBase::CallFullLogMsg : &CApplicationLoggerBase::CallProductionLogMsg )

	, mSignalsMutex( QMutex::Recursive )

	, mQtHandlerMutex( QMutex::Recursive )
{
	assert__( !smInstance );
	smInstance = this;
}

//virtual 
CApplicationLoggerBase::~CApplicationLoggerBase()
{
	assert__( smInstance );
	smInstance = nullptr;
}


//virtual 
int CApplicationLoggerBase::NotifyLevel() const
{
	return QtCriticalMsg;
}


//virtual 
void CApplicationLoggerBase::SetEnabled( bool enable )
{ 
	mEnabled = enable;
	InternalOutputStream::setDebugLevel( enable ? 1 : 0 );
}



//	Captures qDebug, qCritical, qWarning, qFatal
//
void CApplicationLoggerBase::QtMessageOutput( QtMsgType type, const char *msg )
{
	if ( !mEnabled )
		return;

	//std::unique_lock< std::recursive_mutex > lck{ mQtHandlerMutex };
	QMutexLocker locker( &mQtHandlerMutex );

	mQtHandler( type, msg );
}


void CApplicationLoggerBase::FullLogMsg( const QString &msg, QtMsgType level )
{
	ProductionLogMsg( msg, level );

	// !QGISDEBUG means application is using release libraries, where InternalDebugMsg or QgsDebugMsg is dead.
	//	So, in debug builds, calling LOG_TRACE would be redundant with ProductionLog. And dangerous.
	//
#if !defined(QGISDEBUG)

	LOG_TRACE( QString( "%1 %2[%3] %4" ).arg( QDateTime::currentDateTime().toString( Qt::ISODate ) ).arg( "" ).arg( level ).arg( msg ) );
#endif
}


void CApplicationLoggerBase::ProductionLogMsg( const QString &msg, QtMsgType level )
{
	// - Standard output: invokes InternalDebugMsg, which abouts to nothing in release builds. In debug builds, abouts 
	//		to qDebug, or to file if QGIS_LOG_FILE is not empty
	//
	// - GUI output: always emits, so the GUI always captures, debug and release builds
	//
	//QgsMessageLog::logMessage( msg, "", qtlevel_cast< QgsMessageLog::MessageLevel >( level ) );

	//equivalent to QgsMessageLog::logMessage
	InternalDebugMsg( QString( "%1 %2[%3] %4" ).arg( QDateTime::currentDateTime().toString( Qt::ISODate ) ).arg( "" ).arg( level ).arg( msg ) );
	if ( mEnabled )
		emit QtLogMessage( level, msg );

	//InternalDebugMsg( QString( "%1 %2[%3] %4" ).arg( QDateTime::currentDateTime().toString( Qt::ISODate ) ).arg( tag ).arg( level ).arg( message ) );	== 
	//QgsMessageLog::instance()->emitMessage( message, tag, level ); ==	emit messageReceived( message, tag, level );

}

///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_ApplicationLogger.cpp"
