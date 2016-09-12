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

#include "SchedulerApplication.h"
#include "new-gui/Common/ScheduledTasksList.hxx"
#include "new-gui/Common/XmlSerializer.h"
#include "new-gui/Common/DataModels/TaskProcessor.h"
#include "new-gui/Common/GUI/ApplicationUserPaths.h"



bool CSchedulerApplication::smPrologueCalled = false;
const CApplicationUserPaths *CSchedulerApplication::smApplicationPaths = nullptr;


// Static "pre-constructor"
//
//	Ensure that Qt only begins after anything we want to do first,
//	like assigning/checking application paths or statically setting 
//	application style sheet.
//
void CSchedulerApplication::Prologue( int argc, char *argv[], const char *app_name )
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

	safe_debug_envar_msg( "QGIS_LOG_FILE" );
	safe_debug_envar_msg( "QGIS_DEBUG_FILE" );
	safe_debug_envar_msg( "QGIS_DEBUG" );


	// Application/Organization Names ////////////////////////////////////

	QCoreApplication::setApplicationName( app_name );
	QCoreApplication::setOrganizationName( ORGANIZATION_NAME );


	// Application Paths - a critical first thing to do ////////////////////////////////////

    static const CApplicationUserPaths brat_paths( argv[ 0 ], app_name );
    if ( !brat_paths.IsValid() )
        throw CException( "One or more path directories are invalid:\n" + brat_paths.GetErrorMsg() );

    smApplicationPaths = &brat_paths;	LOG_TRACEstd( smApplicationPaths->ToString() );

    smPrologueCalled = true;

	LOG_TRACE( "prologue tasks finished." );
}



// Tries to create a QApplication on-the-fly to able to use the
//	 GUI, since the only place we will call this is in main, where
//	everything else has failed.
//
//static
int CSchedulerApplication::OffGuiErrorDialog( int error_code, char const *error_msg )
{
    int argc = 0;
    QApplication a( argc, nullptr );
    QMessageBox msg_abort;
    msg_abort.setText( error_code == 0 ? QString("Warning:") : QString("A fatal error as occurred.") );
    msg_abort.setInformativeText( error_msg );
    msg_abort.setStandardButtons( error_code == 0 ? QMessageBox::Close : QMessageBox::Abort );
    msg_abort.setIcon( error_code == 0 ? QMessageBox::Warning : QMessageBox::Critical );
    msg_abort.exec();
    return error_code;
}



// (*) only for debugging, do not pass an id as 1st argument to be less 
// restrictive (allowing different executable file instances to run)
//
CSchedulerApplication::CSchedulerApplication( int &argc, char **argv )
    : base_t( "brat-scheduler", argc, argv )							//(*)
{
    assert__( smPrologueCalled );
	if ( !smPrologueCalled )
		throw CException( "CBratApplication Prologue must be called before application construction." );


    //	0. SingleInstanceChecker

	if ( isRunning() )
	{
		QString msg = QString("Quiting duplicated scheduler instance %1.").arg( QCoreApplication::applicationPid() );
		bool sentok = sendMessage( msg, 2000 );
		QString rep( "Another instance is running. Exiting...." );
		rep += sentok ? " Message sent ok." : " Message sending failed; the other instance may be frozen.";
		LOG_WARN( rep );
		throw QtSingleApplicationException( "Scheduler is already running." );		//dtor won't be called
	}


    //	I. XML

    ::xercesc::XMLPlatformUtils::Initialize();


    //	II. Locale

    // v3 note: To be sure that number have always a decimal point (and not a comma or something else)
    //
    setlocale( LC_NUMERIC, "C" );


    //	III. Locate data directory

    CTools::SetInternalDataDir( smApplicationPaths->mInternalDataDir );
}



//virtual
CSchedulerApplication::~CSchedulerApplication()
{
    ::xercesc::XMLPlatformUtils::Terminate();

	CTasksProcessor::DestroyInstance();
}




///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_SchedulerApplication.cpp"
