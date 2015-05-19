/*
* Copyright (C) 2005-2011 Collecte Localisation Satellites (CLS), France (31)
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
#include "Stl.h"
#include "Product.h"
//#include "List.h"
//#include "Exception.h"
//#include "Trace.h"
//#include "BratProcess.h"
//#include "BratProcessStats.h"

// When debugging changes all calls to “new” to be calls to “DEBUG_NEW” allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

using namespace brathl;
//using namespace processes;


/*
**
** Main program
**
*/
int main (int argc, char *argv[])
{
#ifdef WIN32
  #ifdef _DEBUG
    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
  #endif
#endif

  if (argc != 2)
  {
    cerr << "usage: " << argv[0] << " filename" << endl;
    return 2;
  }

  int32_t result = BRATHL_SUCCESS;

  string msg;

  bool bOk = true;

  if (getenv(BRATHL_ENVVAR) == NULL)
  {
    CTools::SetDataDirForExecutable(argv[0]);
  }

  if (!CTools::LoadAndCheckUdUnitsSystem(msg))
  {
    cerr << msg << endl;
    return BRATHL_ERROR;
  }
  try
  {

    string fileName = argv[1];
    CStringArray errors;
    CProduct::CheckAliases(fileName, errors);
    CStringArray::const_iterator it;

    for (it = errors.begin() ; it != errors.end() ; it++)
    {
      cout << *it << endl;
    }

    return result;
  }
  catch (CException &e)
  {
    cerr << "BRAT ERROR: " << e.what() << endl;
    return BRATHL_ERROR;
  }
  catch (exception &e)
  {
    cerr << "BRAT RUNTIME ERROR: " << e.what() << endl;
    return 254;
  }
  catch (...)
  {
    cerr << "BRAT FATAL ERROR: Unexpected error" << endl;
    return 255;
  }
}




