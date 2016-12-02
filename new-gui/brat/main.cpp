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

#include "ApplicationPaths.h"

#include "BratApplication.h"
#include "BratMainWindow.h"
#include "BratLogger.h"



//_NO_DEBUG_HEAP=1


const std::string mRadsServerAddress = "radsuser@corads.tudelft.nl::rads/data";
        // /usr/bin/rsync -avrzR --del --password-file="/Users/brat/dev/bin/Qt-5.7.0/brat/x86_64/Debug/brat.app/Contents/MacOS/data/rads_pass.txt" radsuser@corads.tudelft.nl::'rads/data/c2 rads/data/j2'  "/Volumes/ext_My_Passport/user-data/rads"

int main( int argc, char *argv[] )
try {
    
    
//    std::string src_missions;
//    auto v = String2Vector( mRadsServerAddress, std::string( "::" ) );
//    src_missions += v[0] + "::" + "'";
//    std::vector<std::string> MissionNames = { "c2" };
//    for ( auto const &mission : MissionNames )
//	{
//		src_missions += ( v[1] + "/" + mission + " " );
//	}
//    src_missions.back() = '\'';
    
//    std::cout << src_missions << std::endl;

//    return 0;

	//CBratLogger::Instance().SetEnabled( false );

	LOG_TRACE( "\n*** Entering main." );

	CBratApplication::Prologue( argc, argv );

	LOG_INFO( "brat is starting..." );

    CBratApplication a( argc, argv, true );
	CBratMainWindow w( a );

	int result = 0;
	if ( a.OperatingInInstantPlotSaveMode() )
	{
		LOG_TRACE( "\nOperating in plot-save mode." );
	}
	else
	{
        w.show();
		result = a.exec();
	}

	LOG_INFO( "brat is exiting with result " + n2s( result ) + ".");

	LOG_TRACE( "\nLeaving main. ***" );
	return result;
}
catch ( const CException &e )
{
	LOG_FATAL( e.Message() );
    return CBratApplication::OffGuiErrorDialog( -1, e.Message().c_str() );
}
catch ( const std::exception &e )
{	
	LOG_FATAL( e.what() );
    return CBratApplication::OffGuiErrorDialog( -2, e.what() );
}
catch ( ... )
{
	static const std::string msg( "Unknown exception caught in main. Not possible to retrieve error information." );
	LOG_FATAL( msg );
    return CBratApplication::OffGuiErrorDialog( -3, msg.c_str() );
}
