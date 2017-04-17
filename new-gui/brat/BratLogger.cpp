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

#include <osgEarth/Notify>

#include "common/+Utils.h"
#include "BratLogger.h"


void TraceWrite( const QString& msg, int debuglevel, const char* file, const char* function, int line )	//debuglevel = 1, const char* file = nullptr, const char* function = nullptr, int line = -1 
{
	QgsLogger::debug( msg, debuglevel, file, function, line );
}



//static 
CBratLogger& CBratLogger::Instance()
{
	if ( !smInstance )
		smInstance = new CBratLogger();

	return *dynamic_cast< CBratLogger* >( smInstance );
}


//	Dynamic instance of the class is passed to osg::setNotifyHandler and automatically deleted by OSG
//
CBratLogger::CBratLogger() 
	: base_t()
{
	assert__( smInstance && smInstance == this );		//in strict C++ terms, smInstance not necessarily == this

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
CBratLogger::~CBratLogger()
{}


void CBratLogger::SetOsgStandardNotifier( bool standard )
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


void CBratLogger::SetOsgNotifyLevel( osg::NotifySeverity ns )
{ 
	osg::setNotifyLevel( ns );
	osgEarth::setNotifyLevel( ns );

	emit OsgLogLevelChanged( ns );
}


//virtual 
void CBratLogger::SetNotifyLevel( int level )
{ 
	SetOsgNotifyLevel( level_cast< osg::NotifySeverity >( level ) );
}




//	Captures qDebug, qCritical, qWarning, qFatal
//
void CBratLogger::QtMessageOutput( QtMsgType type, qt_raw_logtext_t msg, const QMessageLogContext *pcontext )	//pcontext = nullptr
{
	if ( !mEnabled )
		return;

    base_t::QtMessageOutput( type, msg, pcontext );

	QMutexLocker locker( &mQtHandlerMutex );

	switch ( type )
	{
		case QtDebugMsg:
			//QgsMessageLog::logMessage( msg, "Debug" );		infinite loop
			break;

		case QtWarningMsg:
        
#if defined(Q_OS_MAC)
			if ( !msg.startsWith( "QWidgetWindow" ) && !msg.startsWith( "QNSView" ) )   //Qt5 bugs; expecting next version
#endif
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


void CBratLogger::ProductionLogMsg( const QString &msg, QtMsgType level )
{
	// - Standard output: invokes QgsDebugMsg, which abouts to nothing in release builds. In debug builds, abouts 
	//		to qDebug, or to file if QGIS_LOG_FILE is not empty
	//
	// - GUI output: always emits, so the GUI always captures, debug and release builds
	//
	QgsMessageLog::logMessage( msg, "", level_cast< QgsMessageLog::MessageLevel >( level ) );

	//QgsDebugMsg( QString( "%1 %2[%3] %4" ).arg( QDateTime::currentDateTime().toString( Qt::ISODate ) ).arg( tag ).arg( level ).arg( message ) );	== QgsLogger::debug(QString(str), 1, __FILE__, __FUNCTION__, __LINE__)
	//QgsMessageLog::instance()->emitMessage( message, tag, level ); ==	emit messageReceived( message, tag, level );
}


//virtual override
void CBratLogger::notify( osg::NotifySeverity severity, const char *message )
{
	//qApp->processEvents();

	//std::unique_lock< std::recursive_mutex > lck{ mSignalsMutex };
	QMutexLocker locker( &mSignalsMutex );

	if ( mEnabled )
		emit OsgLogMessage( severity, message );
}


void CBratLogger::GuiLogMessage( QString message, QString tag, QgsMessageLog::MessageLevel level )
{
	Q_UNUSED( tag );

	//std::unique_lock< std::recursive_mutex > lck{ mSignalsMutex };
	QMutexLocker locker( &mSignalsMutex );

	if ( mEnabled )
		emit QgisLogMessage( level_cast< osg::NotifySeverity >( level ), message );
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_BratLogger.cpp"
