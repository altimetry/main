/*
* Copyright (C) 2005 Collecte Localisation Satellites (CLS), France (31)
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
#include <typeinfo> 

#include "Stl.h"
#include "List.h"
#include "Exception.h"
#include "FileParams.h"
#include "Product.h"
#include "Trace.h"
#include "ProcessCommonTools.h"

using namespace brathl;
using namespace processes;


//----------------------------------------

int main (int argc, char *argv[])
{
  string			FileName;
  CStringArray			InputFiles;
  CProduct			*Product	= NULL;
  bool				Error		= false;
  bool				Help		= false;

  if (argc != 2)
    Error	= true;
  else if ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0))
    Help	= true;
  else
  {
    FileName.assign(argv[1]);

    if (! CTools::FileExists(FileName))
    {
      cerr << "ERROR: Data file '" << FileName << "' not found" << endl << endl;
      Error	= true;
    }
    InputFiles.push_back(FileName);
  }

  if (Error || Help)
  {
    cerr << "Usage : " << argv[0] << " [ -h | --help] FileName" << endl;
    cerr << "Where FileName is the name of a file to show" << endl;
  }

  if (Help)
    cerr << endl << "Displays the fields available for a product file" << endl;

  if (Error || Help)
    return 2;

  if (getenv(BRATHL_ENVVAR) == NULL)
  {
    CTools::SetDataDirForExecutable(argv[0]);
  }

  try
  {
//    auto_ptr<CTrace>pTrace(CTrace::GetInstance());

    // construct the product
    Product = CProduct::Construct(InputFiles);
    Product->Open(FileName);

    CTreeField	*Tree	= Product->GetTreeField();
    if (Tree->GetRoot() != NULL)
    {
      Tree->SetWalkDownRootPivot();    
      do
      {
	CField *field  = dynamic_cast<CField*>(Tree->GetWalkCurrent()->GetData()); 
	if (field == NULL)
	{
	  throw CException("ERROR at least one of the tree object is not a CField object",
			   BRATHL_INCONSISTENCY_ERROR);
	}

	if (typeid(*field) == typeid(CFieldRecord))
	  continue;
	if ((typeid(*field) == typeid(CFieldArray))  &&
	    (! field->IsFixedSize()))
	  continue;
	string Unit	= field->GetUnit();
	cout << CTools::Format("%-20s", field->GetRecordName().c_str())
	     << field->GetFullName();
 	if (Unit != "")
	  cout << " (" << Unit << ")";
	cout << endl;
      }
      while (Tree->SubTreeWalkDown());
    }

    delete Product;
    return 0;
  }
  catch (CException &e)
  {
    cerr << "BRAT ERROR: " << e.what() << endl;
    return 1;
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

