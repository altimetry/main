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

#include "new-gui/Common/ApplicationPaths.h"

#include "BratApplication.h"
#include "BratMainWindow.h"
#include "BratLogger.h"

#include "GUI/norwegianwoodstyle.h"

#include "DataModels/Filters/BratFilters.h"

//_NO_DEBUG_HEAP=1

int main( int argc, char *argv[] )
try {
	//CBratLogger::Instance().SetEnabled( false );

#if defined(_MSC_VER)
	RestoreSystemSETranslator();
#endif

	LOG_TRACE( "\n*** Entering main." );

	//  In Linux there must be a strict destruction order; 1. application
	//  window -> 2. application -> 3. other. (In Windows there are no visible
	//  problems with arbitrary order). So, the application cannot be
	//  dynamically deleted in main if the window is in the stack, because
	//  then it would be destructed before the end of the block, that is,
	//  before the window. Also, the application cannot be a singleton
	//  hidden in a static function of the application class, because then
	//  it risks to be destructed after some other globals it uses (this
	//  is a guess based on crashes in some icon cache when a static
	//  application object is destructed).
	//
	//  Conclusion: there is no good way of hiding the prologue inside
	//  a singleton "static ctor": if the application object is local static.
	//  violates sequence 2->3; if it is a dynamic pointer to be returned and
	//  destructed before the main block ends, violates 1->2.
	//  The following is the cleanest and clearest code to ensure the right
	//  construction/destruction order and at the same time ensure that
	//  we can do things before Qt.

	CBratApplication::Prologue( argc, argv );

	LOG_INFO( "brat is starting..." );

    CBratApplication a( argc, argv, true );    //QApplication::setStyle(new NorwegianWoodStyle);		//joke
	CBratMainWindow w( a );

	int result = 0;
	if ( a.OperatingInInstantPlotSaveMode() )
	{
		//a.processEvents();
		//result = a.exec();
	}
	else
	{
        w.show();
		result = a.exec();
	}

	LOG_INFO( "brat is exiting with result " + n2s<std::string>( result ) + ".");

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
