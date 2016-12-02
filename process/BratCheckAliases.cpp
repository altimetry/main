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
#include <cstdlib>
#include <string>
#include "Product.h"

#include "common/ApplicationStaticPaths.h"

// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

using namespace brathl;
//using namespace processes;


static const char *app_name = "BratCheckAliases";

/*
**
** Main program
**
*/
int main( int argc, char *argv[] )
{
#ifdef WIN32
#ifdef _DEBUG
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
#endif

	if ( argc != 2 )
	{
		std::cerr << "usage: " << argv[ 0 ] << " filename" << std::endl;
		return 2;
	}

	int32_t result = BRATHL_SUCCESS;

	std::string msg;

	bool bOk = true;

	const CApplicationStaticPaths brat_paths( argv[ 0 ], app_name );

	CTools::SetInternalDataDir( brat_paths.mInternalDataDir );

	if ( !CTools::LoadAndCheckUdUnitsSystem( msg ) )
	{
		std::cerr << msg << std::endl;
		return BRATHL_ERROR;
	}
	try
	{

		std::string fileName = argv[ 1 ];
		CStringArray errors;
		CProduct::CheckAliases( fileName, errors );
		CStringArray::const_iterator it;

		for ( it = errors.begin(); it != errors.end(); it++ )
		{
			std::cout << *it << std::endl;
		}

		return result;
	}
	catch ( CException &e )
	{
		std::cerr << "BRAT ERROR: " << e.what() << std::endl;
		return BRATHL_ERROR;
	}
	catch ( std::exception &e )
	{
		std::cerr << "BRAT RUNTIME ERROR: " << e.what() << std::endl;
		return 254;
	}
	catch ( ... )
	{
		std::cerr << "BRAT FATAL ERROR: Unexpected error" << std::endl;
		return 255;
	}
}




