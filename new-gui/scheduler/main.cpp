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



#include "SchedulerApplication.h"
#include "SchedulerDialog.h"
#include "SchedulerLogger.h"


class CConsoleApplicationPaths;




int main( int argc, char *argv[] )
try {

#if defined(_MSC_VER)
	RestoreSystemSETranslator();
#endif

	LOG_TRACE( "\n*** Entering main." );

	LOG_INFO( "scheduler is starting..." );

	CSchedulerApplication a( argc, argv );
	CSchedulerDialog w( a, nullptr );
	w.show();
	auto result = a.exec();


	LOG_INFO( "scheduler is exiting with result " + n2s<std::string>( result ) + ".");

	LOG_TRACE( "\nLeaving main. ***" );
	return result;
}
catch ( const QtSingleApplicationException &e )
{
	LOG_WARN( e.Message() );
    return CSchedulerApplication::OffGuiErrorDialog( 0, e.Message().c_str() );
}
catch ( const CException &e )
{
	LOG_FATAL( e.Message() );
    return CSchedulerApplication::OffGuiErrorDialog( -1, e.Message().c_str() );
}
catch ( const std::exception &e )
{	
	LOG_FATAL( e.what() );
    return CSchedulerApplication::OffGuiErrorDialog( -2, e.what() );
}
catch ( ... )
{
	static const std::string msg( "Unknown exception caught in main. Not possible to retrieve error information." );
	LOG_FATAL( msg );
    return CSchedulerApplication::OffGuiErrorDialog( -3, msg.c_str() );
}

