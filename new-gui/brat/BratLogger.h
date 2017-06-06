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
#if !defined BRAT_LOGGER_H
#define BRAT_LOGGER_H

#include <QMutex>

#include <osg/Notify>

#include <qgsmessagelog.h>
#include <qgslogger.h>

#include "common/+Utils.h"
#include "new-gui/Common/ApplicationLoggerBase.h"


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
//		- are defined through calls to CBratLogger::Instance().LogMsg
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

//#undef LOG_TRACE
//#define LOG_TRACE( msg )




class CBratLogger : public CApplicationLoggerBase, public ApplicationLoggerInterface, public osg::NotifyHandler
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

	using base_t = CApplicationLoggerBase;
	using osg_base_t = osg::NotifyHandler;

	
	// static members

public:
	static CBratLogger& Instance();

	// instance data 

	std::vector< std::string >	mLevelNames;
	QHash< int, QColor >		mSeverityToColorTable;
	QHash< int, QString >		mSeverityToPromptTable;

protected:

	// construction / destruction

	CBratLogger();

public:
	virtual ~CBratLogger();


	// access

	osg::NotifySeverity OsgNotifyLevel() const
	{
		return osg::getNotifyLevel();
	}

	void SetOsgNotifyLevel( osg::NotifySeverity ns );


	// ApplicationLoggerInterface implementation

	virtual void SetEnabled( bool enable ) override
	{
		base_t::SetEnabled( enable );
	}

	virtual int NotifyLevel() const override { return OsgNotifyLevel(); }

	virtual void SetNotifyLevel( int level ) override;

	virtual void LogMessage( const QString &msg, QtMsgType level ) override
	{
		LogMsg( msg, level );
	}

	virtual const std::vector< std::string >& LevelNames() const override;

	virtual const QHash< int, QColor >& SeverityToColorTable() const override;

	virtual const QHash< int, QString >& SeverityToPromptTable() const override;



	// other

protected:

	// If standard is false, unassigns "this" as OSG notifier
	//
	void SetOsgStandardNotifier( bool standard );


	// remaining member functions

	virtual void ProductionLogMsg( const QString &msg, QtMsgType level ) override;


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
	//	enum QtMsgType { QtDebugMsg, QtWarningMsg, QtCriticalMsg, QtFatalMsg, QtSystemMsg = QtCriticalMsg };
	//
    virtual void QtMessageOutput( QtMsgType type, qt_raw_logtext_t msg, const QMessageLogContext *pcontext = nullptr ) override;


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
	//	- use queued connections for these
	//
	void OsgLogMessage( int severity, QString message );
    void QgisLogMessage( int severity, QString message );
};





static_assert( 
    QgsMessageLog::INFO == (int)QtMsgType::QtDebugMsg &&
    QgsMessageLog::WARNING == (int)QtMsgType::QtWarningMsg &&
    QgsMessageLog::CRITICAL == (int)QtMsgType::QtCriticalMsg

	, "QGIS and Qt log message types do not match." );


template< typename TO, typename FROM >
inline TO level_cast( FROM level );
//{
//	assert__( false );
//
//	return (TO)level;
//}

template<>
inline QgsMessageLog::MessageLevel level_cast< QgsMessageLog::MessageLevel >( QtMsgType level )
{
	if ( level == QtFatalMsg )
		level = QtCriticalMsg;

	return ( QgsMessageLog::MessageLevel )level;
}


enum ENotifySeverity 
{
	eALWAYS,
	eFATAL,
	eWARN,
	eNOTICE,
	eINFO,
	eDEBUG_INFO,
	eDEBUG_FP,

	ENotifySeverity_size
};



static_assert(

    eALWAYS == (int)osg::ALWAYS &&
    eFATAL == (int)osg::FATAL &&
    eWARN == (int)osg::WARN &&
    eNOTICE == (int)osg::NOTICE &&
    eINFO == (int)osg::INFO &&
    eDEBUG_INFO == (int)osg::DEBUG_INFO &&
    eDEBUG_FP == (int)osg::DEBUG_FP,

	"Discrepancy between logger levels and OSG notify levels"
	);


//enum MessageLevel
//{
//  INFO = 0,
//  WARNING = 1,
//  CRITICAL = 2
//};

template<>
inline osg::NotifySeverity level_cast< osg::NotifySeverity >( QgsMessageLog::MessageLevel level )
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

	return osg::FATAL;	//never reached
}


template<>
inline osg::NotifySeverity level_cast< osg::NotifySeverity >( int level )
{
	assert__( level >= osg::NotifySeverity::ALWAYS  && level <= osg::NotifySeverity::DEBUG_FP );

	return ( osg::NotifySeverity )level;
}



#define LOG_INFO( msg ) log_info< CBratLogger >( msg )
#define LOG_WARN( msg ) log_warn< CBratLogger >( msg )
#define LOG_FATAL( msg ) log_fatal< CBratLogger >( msg )



#endif	//BRAT_LOGGER_H
