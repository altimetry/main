
#include <cstdio>
#include <cstdlib>
#include "Stl.h"
#include "List.h"
#include "Exception.h"
#include "FileParams.h"
#include "Product.h"
#include "Trace.h"
#include "InternalFiles.h"
#include "InternalFilesFactory.h"
#include "InternalFilesYFX.h"
#include "InternalFilesZFXY.h"

using namespace brathl;

int main (int argc, char *argv[])
{
  if (argc != 2)
  {
    cerr << "usage: " << argv[0] << " filename" << endl;
    return 2;
  }

  if (getenv(BRATHL_ENVVAR) == NULL)
  {
    CTools::SetDataDirForExecutable(argv[0]);
  }

  try
  {
//    auto_ptr<CTrace>pTrace(CTrace::GetInstance());

    vector<string>		Names;
    vector<string>		DimNames;
    ExpressionValueDimensions	DimVal;
    vector<string>::iterator	is;
    size_t			Index;
    CExpressionValue		VarValue;

    cout << "File name:" << endl << "\t" << argv[1] << endl;

    CInternalFiles &F	= *BuildExistingInternalFileKind(argv[1]);

    F.Open(ReadOnly);

    cout << "Type:" << endl << "\t" << F.GetType() << endl;
    cout << "Title:" << endl << "\t" << F.GetTitle("") << endl;

    F.GetVariables(Names);
    cout << "Variables:" << endl;
    for (is=Names.begin(); is != Names.end(); is++)
    {
      string Title = F.GetTitle(*is);
      cout << "\t" << *is;
      if (Title != "")
        cout << " (" << Title << ")";
      cout << ". Unit=" << F.GetUnit(*is).GetText() << endl;
    }

    cout << endl << "Dimensions:" << endl;
    F.GetAxisVars(Names);
    for (is=Names.begin(); is != Names.end(); is++)
    {
      F.GetVarDims(*is, DimNames);
      F.GetVarDims(*is, DimVal);
      cout << "\t" << *is << "(";
      for (Index=0; Index < DimNames.size(); Index++)
      {
	if (Index != 0) cout << ", ";
	cout << DimNames[Index] << ":" << DimVal[Index];
      }
      cout << "): Type=" << CNetCDFFiles::VarKindToString(F.GetVarKind(*is)) << endl;
    }

    for (is=Names.begin(); is != Names.end(); is++)
    {
      F.ReadVar(*is, VarValue, CUnit::m_UNIT_SI);
      F.GetVarDims(*is, DimVal);
      cout << endl << endl << "\t" << *is << "=" << VarValue.AsString() << endl;
    }

    cout << endl << "Data variables:" << endl;
    F.GetDataVars(Names);
    for (is=Names.begin(); is != Names.end(); is++)
    {
      F.GetVarDims(*is, DimNames);
      F.GetVarDims(*is, DimVal);
      cout << "\t" << *is << "(";
      for (Index=0; Index < DimNames.size(); Index++)
      {
	if (Index != 0) cout << ", ";
	cout << DimNames[Index] << ":" << DimVal[Index];
      }
      cout << "): Type=" << CNetCDFFiles::VarKindToString(F.GetVarKind(*is)) << endl;
    }

    for (is=Names.begin(); is != Names.end(); is++)
    {
      F.ReadVar(*is, VarValue, CUnit::m_UNIT_SI);
      F.GetVarDims(*is, DimVal);
      cout << endl << endl << "\t" << *is << "=" << VarValue.AsString() << endl;
    }

    cout << endl << endl << endl << "You can use ncdump command as well" << endl;
    delete &F;
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

