#include "stdafx.h"

#include "new-gui/Common/+Utils.h"
#include "ApplicationLogger.h"



CApplicationLogger *CApplicationLogger::smInstance = nullptr;


//static 
CApplicationLogger& CApplicationLogger::Instance()
{
	if ( !smInstance )
		smInstance = new CApplicationLogger();

	return *smInstance;
}


// This is wat Qt does in systems where it does not assign a specific handler
//
void DefaultQtMessageHandler( QtMsgType type, const char *msg )
{
    Q_UNUSED( type );

    fprintf( stderr, "%s\n", msg );
    fflush( stderr );
}


//	Dynamic instance of the class is passed to osg::setNotifyHandler and automatically deleted by OSG
//
CApplicationLogger::CApplicationLogger() 

	: mQtHandler( qInstallMsgHandler( MessageOutputProxy ) )

	, mLogMsg( getenv( "QGIS_DEBUG" ) ? &CApplicationLogger::FullLog : &CApplicationLogger::ProductionLog )

	, mSignalsMutex( QMutex::Recursive )

	, mQtHandlerMutex( QMutex::Recursive )
{
	assert__( !smInstance );
	smInstance = this;

    if (!mQtHandler)
        mQtHandler = &DefaultQtMessageHandler;

	// Capture OSG info in notify member function
	//
	SetOsgStandardNotifier( false );

	// Capture QGIS MessageLogs in GuiLogMessage
	//
	connect( 
		QgsMessageLog::instance(), SIGNAL( messageReceived( QString, QString, QgsMessageLog::MessageLevel ) ),
		this, SLOT( GuiLogMessage( QString, QString, QgsMessageLog::MessageLevel ) ) );
}

//virtual 
CApplicationLogger::~CApplicationLogger()
{
	assert__( smInstance );
	smInstance = nullptr;
}


void CApplicationLogger::SetOsgStandardNotifier( bool standard )
{
	if ( standard )
	{
#if defined(WIN32)
		osg::setNotifyHandler( new osg::WinDebugNotifyHandler );
#else
		osg::setNotifyHandler( new osg::StandardNotifyHandler );
#endif	
	}
	else 
	{
		static osg::NotifySeverity ns = osg::WARN;

		SetOsgNotifyLevel( ns );

		// Capture OSG info in notify member function
		//
		osg::setNotifyHandler( this );		
		osg::getNotifyHandler()->ref();		//prevent auto deletion by OSG
	}
}


void CApplicationLogger::SetOsgNotifyLevel( osg::NotifySeverity ns )
{ 
	osg::setNotifyLevel( ns );
	emit OsgLogLevelChanged( ns );
}




//	Captures qDebug, qCritical, qWarning, qFatal
//
void CApplicationLogger::MessageOutput( QtMsgType type, const char *msg )
{
	//std::unique_lock< std::recursive_mutex > lck{ mQtHandlerMutex };
	QMutexLocker locker( &mQtHandlerMutex );

	mQtHandler( type, msg );

	switch ( type )
	{
		case QtDebugMsg:
			//QgsMessageLog::logMessage( msg, "Debug" );		infinite loop
			break;

		case QtWarningMsg:
			QgsMessageLog::logMessage( msg, "Warning" );		//redirects recursively here (as QtDebugMsg) and to GUI
			break;

		case QtCriticalMsg:	//=	QtSystemMsg
			QgsMessageLog::logMessage( msg, "Critical" );		//redirects recursively here (as QtDebugMsg) and to GUI
			break;

		case QtFatalMsg:
			QgsMessageLog::logMessage( msg, "Fatal" );			//redirects recursively here (as QtDebugMsg) and to GUI
			break;
			//abort();
		default:
			assert__( false );
	}
}


void CApplicationLogger::FullLog( const QString &msg, QgsMessageLog::MessageLevel level )
{
	ProductionLog( msg, level );

	// !QGISDEBUG means brat is using QGIS release libraries, where QgsDebugMsg is dead.
	//	So, in debug builds, calling LOG_TRACE would be redundant with ProductionLog. And dangerous.
	//
#if !defined(QGISDEBUG)

	LOG_TRACE( QString( "%1 %2[%3] %4" ).arg( QDateTime::currentDateTime().toString( Qt::ISODate ) ).arg( "" ).arg( level ).arg( msg ) );
#endif
}


void CApplicationLogger::ProductionLog( const QString &msg, QgsMessageLog::MessageLevel level )
{
	// - Standard output: invokes QgsDebugMsg, which abouts to nothing in release builds. In debug builds, abouts 
	//		to qDebug, or to file if QGIS_LOG_FILE is not empty
	//
	// - GUI output: always emits, so the GUI always captures, debug and release builds
	//
	QgsMessageLog::logMessage( msg, "", level );
}


//enum NotifySeverity 
//{
//    ALWAYS=0,
//    FATAL=1,
//    WARN=2,
//    NOTICE=3,
//    INFO=4,
//    DEBUG_INFO=5,
//    DEBUG_FP=6
//};

//enum MessageLevel
//{
//  INFO = 0,
//  WARNING = 1,
//  CRITICAL = 2
//};

inline int Qsg2OsgLevel( QgsMessageLog::MessageLevel level )
{
	switch (level)
	{
		case QgsMessageLog::INFO:
			return osg::ALWAYS;
			break;

		case QgsMessageLog::WARNING:
			return osg::WARN;
			break;

		case QgsMessageLog::CRITICAL:
			return osg::FATAL;
			break;

		default:
			assert__( false );
	}

	return -1;	//never reached
}


//virtual override
void CApplicationLogger::notify( osg::NotifySeverity severity, const char *message )
{
	//qApp->processEvents();

	//std::unique_lock< std::recursive_mutex > lck{ mSignalsMutex };
	QMutexLocker locker( &mSignalsMutex );

	if ( mEnabled )
		emit OsgLogMessage( severity, message );
}


void CApplicationLogger::GuiLogMessage( QString message, QString tag, QgsMessageLog::MessageLevel level )
{
	Q_UNUSED( tag );

	//std::unique_lock< std::recursive_mutex > lck{ mSignalsMutex };
	QMutexLocker locker( &mSignalsMutex );

	if ( mEnabled )
		emit QgisLogMessage( Qsg2OsgLevel( level ), message );
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_ApplicationLogger.cpp"
