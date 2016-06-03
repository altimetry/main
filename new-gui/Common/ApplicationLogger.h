#if !defined COMMON_APPLICATION_LOGGER_H
#define COMMON_APPLICATION_LOGGER_H

#include <QMutex>

#include "new-gui/Common/QtStringUtils.h"


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
//			does nothing. In debug builds it will always be on and always print.
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
//		- are defined through calls to CMyLoggerClass::Instance().LogMsg
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


//	The body of this function must be defined in a client application source file
//
//	In QGIS dependent applications, define it as 
//	{
//		QgsLogger::debug( msg, debuglevel, file, function, line );
//	}
//
//	In other applications, define it as
//	{
//		CApplicationLoggerBase::TraceWrite( msg, debuglevel, file, function, line );
//	}
//
void TraceWrite( const QString& msg, int debuglevel = 1, const char* file = nullptr, const char* function = nullptr, int line = -1 );


// Using the same definition of LOG_TRACE for release and debug builds, there 
//	is always a function call, even in release mode, and even if QGIS_DEBUG is
//	0 and messages are not printed.
//
// On the other hand, if QGIS_DEBUG is 1, this allows to still have access to trace
//	messages in release builds.
//
#define LOG_TRACE( msg )	TraceWrite(QString(msg), 1, __FILE__, __FUNCTION__, __LINE__)

// The same as LOG_TRACE, for std::strings. Macros cannot be overloaded...
//
#define LOG_TRACEstd( msg )	LOG_TRACE( t2q( msg ) )




// Using LOGGER::Instance() for calling the static CApplicationLoggerBase::LogMsg
//	to ensure singleton instantiation and that it's type is of the correct class

template< class LOGGER, typename STRING >
inline void log_info( const STRING &msg )
{
    LOGGER::Instance().LogMsg( msg, QtMsgType::QtDebugMsg );
}
template< class LOGGER >
inline void log_info( const std::string &msg )
{
    log_info< LOGGER >( msg.c_str() );
}


template< class LOGGER, typename STRING >
inline void log_warn( const STRING &msg )
{
    LOGGER::Instance().LogMsg( msg, QtMsgType::QtWarningMsg );
}
template< class LOGGER >
inline void log_warn( const std::string &msg )
{
    log_warn< LOGGER >( msg.c_str() );
}


template< class LOGGER, typename STRING >
inline void log_fatal( const STRING &msg )
{
    LOGGER::Instance().LogMsg( msg, QtMsgType::QtCriticalMsg );
}
template< class LOGGER >
inline void log_fatal( const std::string &msg )
{
    log_fatal< LOGGER >( msg.c_str() );
}


//
//#define LOG_INFO( msg ) log_info< CMyLoggerClass >( msg )
//#define LOG_WARN( msg ) log_warn< CMyLoggerClass >( msg )
//#define LOG_FATAL( msg ) log_fatal< CMyLoggerClass >( msg )
//



class CApplicationLoggerBase : public QObject, public non_copyable
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

    // types & friends

	using base_t = QObject;

	using log_msg_t = void (CApplicationLoggerBase::*)( const QString &msg, QtMsgType level );


protected:
	// static members

	static CApplicationLoggerBase *smInstance;

	static void MessageOutputProxy( QtMsgType type, const char *msg )
	{
		smInstance->QtMessageOutput( type, msg );
	}


	static QtMsgHandler InstallQtMsgHandler( QtMsgHandler handler );

public:

	// For default trace macro usage
	//
	static void TraceWrite( const QString& msg, int debuglevel = 1, const char* file = nullptr, const char* function = nullptr, int line = -1 );


	// For macros API usage (except trace)
	//
	//	- the pointer selects debug or production log method (in CApplicationLoggerBase ctor according to QGIS_DEBUG value)
	//	- the pointed methods simply make virtual calls (implicitly selecting base or derived class implementation)
	//
	static void LogMsg( const QString &msg, QtMsgType level )
	{
		(smInstance->*(smInstance->mLogMsg))( msg, level );
	}


	// instance data 

protected:

	log_msg_t				mLogMsg = nullptr;
	bool					mEnabled = true;
	QtMsgHandler			mQtHandler = nullptr;
    QMutex					mSignalsMutex;
	QMutex					mQtHandlerMutex;

	// construction / destruction

	CApplicationLoggerBase();

public:

	virtual ~CApplicationLoggerBase();


	// access

	virtual int NotifyLevel() const;

	virtual void SetNotifyLevel( int level )
	{
		Q_UNUSED( level );
	}

	virtual void SetEnabled( bool enable );


protected:

	// remaining member functions

private:
	void CallFullLogMsg( const QString &msg, QtMsgType level )
	{
		FullLogMsg( msg, level );
	}
	void CallProductionLogMsg( const QString &msg, QtMsgType level )
	{
		ProductionLogMsg( msg, level );
	}

protected:
	// calls ProductionLogMsg and traces if not in debug mode
	//
	virtual void FullLogMsg( const QString &msg, QtMsgType level );

	// to override (typically in QGIS application) for specialized info, warn, fatal
	//
	virtual void ProductionLogMsg( const QString &msg, QtMsgType level );


	// Qt callback: 
	//	Captures Qt's qDebug, qCritical, qWarning, qFatal
	//	- qDebug: reuses whatever Qt uses as output. Origin:
	//		1. issued directly by Qt code, brat code, etc.
	//
	//	- qCritical, qWarning, qFatal are redirected also to qDebug and to the GUI (via QGIS QgsMessageLog::logMessage)
	//
	//	enum QtMsgType { QtDebugMsg, QtWarningMsg, QtCriticalMsg, QtFatalMsg, QtSystemMsg = QtCriticalMsg };
	//
	virtual void QtMessageOutput( QtMsgType type, const char *msg );


public slots:

signals:

	// Log signals, meant for GUI display
	//	- use queued connections for these
	//
	void QtLogMessage( int severity, QString message );
};





#endif	//COMMON_APPLICATION_LOGGER_H
