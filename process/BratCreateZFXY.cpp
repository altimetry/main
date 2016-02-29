/*
* 
*
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
#include <cstdio>
#include <string>
#include "List.h"
#include "new-gui/Common/tools/Exception.h"
#include "FileParams.h"
#include "Tools.h"
#include "Field.h"
#include "Product.h"
#include "ProductNetCdf.h"
#include "new-gui/Common/tools/Trace.h"
#include "new-gui/Common/ConsoleApplicationPaths.h"
#include "BratProcess.h"
#include "BratProcessZFXY.h"
#include "InternalFilesZFXY.h"

// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

using namespace brathl;
using namespace processes;

//----------------------------------------
//----------------------------------------
//----------------------------------------
int main( int argc, char *argv[] )
{
#ifdef WIN32
#ifdef _DEBUG
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
#endif

	int32_t result = BRATHL_SUCCESS;

	std::string msg;

	bool bOk = true;

	const CConsoleApplicationPaths brat_paths( argv[ 0 ] );

	CTools::SetInternalDataDir( brat_paths.mInternalDataDir );

	if ( !CTools::LoadAndCheckUdUnitsSystem( msg ) )
	{
		std::cerr << msg << std::endl;
		return BRATHL_ERROR;
	}

	CBratProcessZFXY*  bratProcess = new CBratProcessZFXY();

	if ( bratProcess->CheckCommandLineOptions( argc, argv ) )
	{
		return BRATHL_ERROR;
	}

	try
	{
		//    auto_ptr<CTrace>pTrace(CTrace::GetInstance());

		bOk = bratProcess->Initialize( msg );
		if ( !bOk )
		{
			CTrace::Tracer( 1, msg );
			delete bratProcess;
			bratProcess = NULL;
			return BRATHL_ERROR;
		}

		result = bratProcess->Execute( msg );

		if ( result != BRATHL_SUCCESS )
		{
			CTrace::Tracer( 1, msg );
		}


		delete bratProcess;
		bratProcess = NULL;

		return result;
	}
	catch ( CException &e )
	{
		std::cerr << "BRAT ERROR: " << e.what() << std::endl;
		delete bratProcess;
		bratProcess = NULL;
		return BRATHL_ERROR;
	}
	catch ( std::exception &e )
	{
		std::cerr << "BRAT RUNTIME ERROR: " << e.what() << std::endl;
		delete bratProcess;
		bratProcess = NULL;
		return 254;
	}
	catch ( ... )
	{
		std::cerr << "BRAT FATAL ERROR: Unexpected error" << std::endl;
		delete bratProcess;
		bratProcess = NULL;
		return 255;
	}


}

