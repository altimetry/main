/*
*  This file is part of BRAT.
*
*    BRAT is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    BRAT is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with BRAT.  If not, see <http://www.gnu.org/licenses/>.
*
*/

#include <cstdio>
#include <cstdlib>
#include <string>
#include "List.h"
#include "new-gui/Common/tools/Exception.h"
#include "FileParams.h"
#include "Product.h"
#include "new-gui/Common/tools/Trace.h"
#include "InternalFiles.h"
#include "InternalFilesFactory.h"
#include "InternalFilesYFX.h"
#include "InternalFilesZFXY.h"

using namespace brathl;

int main (int argc, char *argv[])
{
  if (argc != 2)
  {
    std::cerr << "usage: " << argv[0] << " filename" << std::endl;
    return 2;
  }

  if (getenv(BRATHL_ENVVAR) == NULL)
  {
    CTools::SetDataDirForExecutable(argv[0]);
  }

  try
  {
//    auto_ptr<CTrace>pTrace(CTrace::GetInstance());

    std::vector<std::string>		Names;
    std::vector<std::string>		DimNames;
    ExpressionValueDimensions	DimVal;
    std::vector<std::string>::iterator	is;
    size_t			Index;
    CExpressionValue		VarValue;

    std::cout << "File name:" << std::endl << "\t" << argv[1] << std::endl;

    CInternalFiles &F	= *BuildExistingInternalFileKind(argv[1]);

    F.Open(ReadOnly);

    std::cout << "Type:" << std::endl << "\t" << F.GetType() << std::endl;
    std::cout << "Title:" << std::endl << "\t" << F.GetTitle("") << std::endl;

    F.GetVariables(Names);
    std::cout << "Variables:" << std::endl;
    for (is=Names.begin(); is != Names.end(); is++)
    {
      std::string Title = F.GetTitle(*is);
      std::cout << "\t" << *is;
      if (Title != "")
        std::cout << " (" << Title << ")";
      std::cout << ". Unit=" << F.GetUnit(*is).GetText() << std::endl;
    }

    std::cout << std::endl << "Dimensions:" << std::endl;
    F.GetAxisVars(Names);
    for (is=Names.begin(); is != Names.end(); is++)
    {
      F.GetVarDims(*is, DimNames);
      F.GetVarDims(*is, DimVal);
      std::cout << "\t" << *is << "(";
      for (Index=0; Index < DimNames.size(); Index++)
      {
	if (Index != 0) std::cout << ", ";
	std::cout << DimNames[Index] << ":" << DimVal[Index];
      }
      std::cout << "): Type=" << CNetCDFFiles::VarKindToString(F.GetVarKind(*is)) << std::endl;
    }

    for (is=Names.begin(); is != Names.end(); is++)
    {
      F.ReadVar(*is, VarValue, CUnit::m_UNIT_SI);
      F.GetVarDims(*is, DimVal);
      std::cout << std::endl << std::endl << "\t" << *is << "=" << VarValue.AsString() << std::endl;
    }

    std::cout << std::endl << "Data variables:" << std::endl;
    F.GetDataVars(Names);
    for (is=Names.begin(); is != Names.end(); is++)
    {
      F.GetVarDims(*is, DimNames);
      F.GetVarDims(*is, DimVal);
      std::cout << "\t" << *is << "(";
      for (Index=0; Index < DimNames.size(); Index++)
      {
	if (Index != 0) std::cout << ", ";
	std::cout << DimNames[Index] << ":" << DimVal[Index];
      }
      std::cout << "): Type=" << CNetCDFFiles::VarKindToString(F.GetVarKind(*is)) << std::endl;
    }

    for (is=Names.begin(); is != Names.end(); is++)
    {
      F.ReadVar(*is, VarValue, CUnit::m_UNIT_SI);
      F.GetVarDims(*is, DimVal);
      std::cout << std::endl << std::endl << "\t" << *is << "=" << VarValue.AsString() << std::endl;
    }

    std::cout << std::endl << std::endl << std::endl << "You can use ncdump command as well" << std::endl;
    delete &F;
    return 0;
  }
  catch (CException &e)
  {
    std::cerr << "BRAT ERROR: " << e.what() << std::endl;
    return 1;
  }
  catch (std::exception &e)
  {
    std::cerr << "BRAT RUNTIME ERROR: " << e.what() << std::endl;
    return 254;
  }
  catch (...)
  {
    std::cerr << "BRAT FATAL ERROR: Unexpected error" << std::endl;
    return 255;
  }
}

