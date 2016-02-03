#if !defined APPLICATION_LOGGER_H
#define APPLICATION_LOGGER_H


#include <osg/Notify>
#include <qgsmessagelog.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// The log API is composed by
//
//	LOG_TRACE
//		- never displayed by the GUI.
//
//		- outputs to wherever Qt qDebug outputs, or to whichever file QGIS_LOG_FILE points to
//			if not empty. Never prints to both.
//
//		- directly calls QGIS QgsLogger::debug
//
//		- defined through a macro, to convey __FILE__, __FUNCTION__, __LINE__ information (defined
//			as a function, even inline, would always refer that function's file, name, line...)
//
//		- so far (see comments near the macro's definition), can be activated in release builds 
//			by defining QGIS_DEBUG to 1 in the execution environment. This means that a runtime
//			call is always made to the QgsLogger::debug, even if, as is the case in release, that
//			does nothing. In debug builds will always be on and always print.
//
//			So, LOG_TRACE should be used with care, because it has a weight in production, even if 
//			the impact is very small for a few well chosen places.
//
//	The remaining 3 interfaces are:
//
//	LOG_INFO
//		Information for the user to see in the Log GUI.
//
//	LOG_WARN
//		The same as LOG_INFO, for warnings.
//
//	LOG_FATAL
//		The same as LOG_INFO, for fatal messages. They should be issued only when it is certain that 
//		the application cannot continue.
//
//	These interfaces 
//		- always print to the GUI (as soon as available, but can be used anytime)
//		- also print to LOG_TRACE in debug builds
//		- also print to LOG_TRACE in release builds if QGIS_DEBUG is 1
//		- are defined through calls to CApplicationLogger::Instance().LogMsg
//
//	So, in release builds, they are useful for tracing purposes only if QGIS_DEBUG is on and the GUI 
//	is not (or not yet, or not already) available. This means that there is never a reason to use 
//	LOG_TRACE if one of these is used to convey the same information. On the other hand, they should
//	not be used for information without interest to the user.
//
//	LOG_DEBUG - not defined. Instead, qDebug is directly used, preferably only on a temporary basis,
//		being deleted after supporting the development task at hand. LOG_TRACE can also be used for
//		debugging purposes, on a temporary basis, for release builds not in production.
//
//	A note about OSG:
//		OSG notifications are only displayed in the GUI. In production the level is set to osg::NOTICE
//		and 
//
//	QGIS implementation notes:
//
//		QgsLogger::debug uses the following environment variables to define its behavior:
//
//			QGIS_LOG_FILE - log output file
//			QGIS_DEBUG_FILE - source file filter
//			QGIS_DEBUG - debug level; 0 or anything above this level prints nothing; 1 is the debug 
//						default. Messages with level <= QGIS_DEBUG are printed. The level parameter
//						in QGIS log functions signature is 1 by default, and brat doesn't use it, 
//						so by default in debug everything is printed, in release nothing is printed.
//
//		QGISDEBUG is a compile time macro (not to be confused with QGIS_DEBUG environment variable), 
//			undefined in release builds (both brat's and QGIS's) and with value 1 in debug builds.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////





class CApplicationLogger : public QObject, public osg::NotifyHandler, public non_copyable
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

    // types & friends

	using base_t = QObject;
	using osg_base_t = osg::NotifyHandler;

	using log_msg_t = void (CApplicationLogger::*)( const QString &msg, QgsMessageLog::MessageLevel level );


	// static data

	static CApplicationLogger *smInstance;

	static void MessageOutputProxy( QtMsgType type, const char *msg )
	{
		smInstance->MessageOutput( type, msg );
	}

public:

	static CApplicationLogger& Instance();


	static void LogMsg( const QString &msg, QgsMessageLog::MessageLevel level )
	{
		(smInstance->*(smInstance->mLogMsg))( msg, level );
	}

	// instance data 

protected:

	log_msg_t				mLogMsg = nullptr;
	bool					mEnabled = true;
	QtMsgHandler			mQtHandler = nullptr;
	//std::recursive_mutex	mSignalsMutex;
	//std::recursive_mutex	mRecMutex;
    QMutex					mSignalsMutex;
	QMutex					mQtHandlerMutex;

	// construction / destruction

	CApplicationLogger();

public:

	virtual ~CApplicationLogger();


	// access

	void SetEnabled( bool enable ){ mEnabled = enable;	}

	osg::NotifySeverity OsgNotifyLevel() const
	{
		return osg::getNotifyLevel();
	}

	void SetOsgNotifyLevel( osg::NotifySeverity ns );

protected:

	// If standard is false, unassigns "this" as OSG notifier
	//
	void SetOsgStandardNotifier( bool standard );


	// remaining member functions

	void FullLog( const QString &msg, QgsMessageLog::MessageLevel level );
	void ProductionLog( const QString &msg, QgsMessageLog::MessageLevel level );


	// Qt callback: 
	//	Captures Qt's qDebug, qCritical, qWarning, qFatal
	//	- qDebug: reuses whatever Qt uses as output. Origin:
	//		1. issued directly by Qt code, brat code, etc.
	//		2. issued by QGIS QgsDebugMsg==QgsLogger::debug(==brat LOG_TRACE), if QGIS_LOG_FILE is empty (the default). 
	//			Otherwise goes to file, not qDebug, so not here.
	//		3. issued by QGIS QgsMessageLog::logMessage(==brat LOG_INFO, WARN, FATAL), which calls 2, (besides emitting)
	//
	//	- qCritical, qWarning, qFatal are redirected also to qDebug and to the GUI (via QGIS QgsMessageLog::logMessage)
	//
	void MessageOutput( QtMsgType type, const char *msg );


	// OSG callback: Capture OSG notify, emits OsgLogMessage for GUI display
	//
	virtual void notify( osg::NotifySeverity severity, const char *message ) override;

public slots:

	// QGIS callback: captures QGIS QgsMessageLog signal messages, emits QgisLogMessage for GUI display
	//
	void GuiLogMessage( QString message, QString tag = QString::null, QgsMessageLog::MessageLevel level = QgsMessageLog::WARNING );


signals:
    void OsgLogLevelChanged( int ns );

	// Log signals, meant for GUI display
	//	- use queued connections for these guys
	//
	void OsgLogMessage( int severity, QString message );
    void QgisLogMessage( int severity, QString message );
};







// Using the same definition for release and debug builds, there is always a function call, even in
//	release mode, and even if QGIS_DEBUG is 0 and messages are not printed.
//
// On the other hand, if QGIS_DEBUG is 1, this allows to still have access to trace
//	messages in release builds.
//
#define LOG_TRACE( msg )	QgsLogger::debug(QString(msg), 1, __FILE__, __FUNCTION__, __LINE__)

// The same as LOG_TRACE, for std::strings. Macros cannot be overloaded...
//
#define LOG_TRACEstd( msg )	LOG_TRACE( t2q( msg ) )



template< typename STRING >
inline void LOG_INFO( const STRING &msg )
{
    CApplicationLogger::Instance().LogMsg( msg, QgsMessageLog::INFO );
}
template<>
inline void LOG_INFO( const std::string &msg )
{
    LOG_INFO( msg.c_str() );
}


template< typename STRING >
inline void LOG_WARN( const STRING &msg )
{
    CApplicationLogger::Instance().LogMsg( msg, QgsMessageLog::WARNING );
}
template<>
inline void LOG_WARN( const std::string &msg )
{
    LOG_WARN( msg.c_str() );
}


template< typename STRING >
inline void LOG_FATAL( const STRING &msg )
{
    CApplicationLogger::Instance().LogMsg( msg, QgsMessageLog::CRITICAL );
}
template<>
inline void LOG_FATAL( const std::string &msg )
{
    LOG_FATAL( msg.c_str() );
}




#endif	//APPLICATION_LOGGER_H
