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
#if defined (__unix__) && defined(__DEPRECATED)
#define __DEPRECATED_DEFINED __DEPRECATED
#undef __DEPRECATED
#endif

#include <vtkLookupTable.h>



#include "wx/arrstr.h"
#include "wx/config.h"  // (to let wxWidgets choose a wxConfig class for your platform)
#include "wx/confbase.h"  // (base config class)
#include "wx/fileconf.h" // (wxFileConfig class)


#include "PlotData/BratLookupTable.h"
#include "display/BratDisplayApp.h"

#if defined (__unix__) && defined(__DEPRECATED_DEFINED)
#define __DEPRECATED __DEPRECATED_DEFINED
#endif



#include "new-gui/Common/tools/Trace.h"
#include "libbrathl/Tools.h"
#include "new-gui/Common/tools/Exception.h"
using namespace brathl;

#include "new-gui/brat/DataModels/PlotData/ColorPalleteNames.h"

//#include "ColorPalette.h"




#define brathlFmtEntryColorMacro(entry, index)	wxString::Format(wxString(CBratLookupTable::GROUP_COLOR()) + "%d/" + entry, index)




//-------------------------------------------------------------
//------------------- CCustomColor class --------------------
//-------------------------------------------------------------
CCustomColor::CCustomColor()
{
  Init();
}
//----------------------------------------
CCustomColor::CCustomColor(const CCustomColor& customColor)
{
  Init();
  m_vtkColor = customColor.m_vtkColor;
  m_xValue = customColor.m_xValue;

}

//----------------------------------------
CCustomColor::CCustomColor(const CPlotColor& vtkColor, int32_t xValue)
{
  Init();

  m_vtkColor = vtkColor;
  m_xValue = xValue;
}
//----------------------------------------
CCustomColor::~CCustomColor()
{

}


//----------------------------------------
void CCustomColor::Init()
{
  setDefaultValue(m_xValue);
}

//-------------------------------------------------------------
//------------------- CBratLookupTable class --------------------
//-------------------------------------------------------------

CBratLookupTable::CBratLookupTable()
    : m_nameToMethod(true)
{
  Init();
}

//----------------------------------------
CBratLookupTable::CBratLookupTable(const CBratLookupTable& lut)
{

  Init();
  m_cust.RemoveAll();
  DupCustMap(*(lut.GetCust()));

  m_grad.RemoveAll();
  DupGradMap(*(lut.GetGrad()));

  //m_std = lut.GetStd();
  m_currentFunction = lut.GetCurrentFunction();

  m_vtkLookupTable->SetNumberOfTableValues(lut.GetLookupTable()->GetNumberOfTableValues());
  m_vtkLookupTable->SetTableRange(lut.GetLookupTable()->GetTableRange());

  ExecCurveMethod(lut.GetCurve());

  /*
  CallbackVoid* method = static_cast< CallbackVoid* >(m_nameToMethod[methodName]);
  if (method == NULL)
  {
    std::string msg = CTools::Format("ERROR in CBratLookupTable::CBratLookupTable(CBratLookupTable& lut) : unknown methodname '%s'",
                                 methodName.c_str());
    CException e(msg, BRATHL_LOGIC_ERROR);
    CTrace::Tracer(e.what());
    Dump(*CTrace::GetDumpContext());
    throw (e);

  }

  method->execute();
*/

  Update();
}

//----------------------------------------

CBratLookupTable::~CBratLookupTable()
{
  if (m_vtkLookupTable != NULL)
  {
    m_vtkLookupTable->Delete();
    m_vtkLookupTable = NULL;
  }
}

//----------------------------------------
CBratLookupTable& CBratLookupTable::operator=(const CBratLookupTable& lut)
{

  m_cust.RemoveAll();
  DupCustMap(*(lut.GetCust()));

  m_grad.RemoveAll();
  DupGradMap(*(lut.GetGrad()));


  //m_std = lut.GetStd();
  m_currentFunction = lut.GetCurrentFunction();

  m_vtkLookupTable->SetNumberOfTableValues(lut.GetLookupTable()->GetNumberOfTableValues());
  m_vtkLookupTable->SetTableRange(lut.GetLookupTable()->GetTableRange());


  ExecCurveMethod(lut.GetCurve());
/*
  CallbackVoid* method = static_cast< CallbackVoid* >(m_nameToMethod[methodName]);
  if (method == NULL)
  {
    std::string msg = CTools::Format("ERROR in CBratLookupTable::CBratLookupTable(CBratLookupTable& lut) : unknown methodname '%s'",
                                 methodName.c_str());
    CException e(msg, BRATHL_LOGIC_ERROR);
    CTrace::Tracer(e.what());
    Dump(*CTrace::GetDumpContext());
    throw (e);

  }

  method->execute();
*/
  Update();

  return *this;
}

//----------------------------------------
void CBratLookupTable::Init()
{

  UpFlank = NULL;
  DownFlank = NULL;

  m_vtkLookupTable = vtkLookupTable::New();

  m_vtkLookupTable->SetNumberOfTableValues(256);

  m_resetFunction = "Reset";

  m_currentFunction = m_resetFunction;

  m_curveNames.Insert((const char*)CURVE_LINEAR());
  m_curveNames.Insert((const char*)CURVE_SQRT());
  m_curveNames.Insert((const char*)CURVE_COSINUS());

  //m_std = m_resetFunction;

  m_defaultColorTable = PALETTE_AEROSOL.c_str();
  #define BRATHL_CALLBACK(Type, Name) static_cast<CBratObject *>(new Type(this, &CBratLookupTable::Name))
  m_nameToMethod.Insert(PALETTE_BLACKTOWHITE,			BRATHL_CALLBACK(CallbackVoid, BlackToWhite));
  m_nameToMethod.Insert(PALETTE_WHITETOBLACK,			BRATHL_CALLBACK(CallbackVoid, WhiteToBlack));
  m_nameToMethod.Insert(PALETTE_REDTOGREEN,			BRATHL_CALLBACK(CallbackVoid, RedToGreen));
  m_nameToMethod.Insert(PALETTE_GREENTORED,			BRATHL_CALLBACK(CallbackVoid, GreenToRed));
  m_nameToMethod.Insert(PALETTE_CLOUD,			BRATHL_CALLBACK(CallbackVoid, Cloud));
  m_nameToMethod.Insert(PALETTE_RAINBOW,			BRATHL_CALLBACK(CallbackVoid, Rainbow));
  m_nameToMethod.Insert(PALETTE_RAINBOW2,			BRATHL_CALLBACK(CallbackVoid, Rainbow2));
  m_nameToMethod.Insert(m_defaultColorTable,	BRATHL_CALLBACK(CallbackVoid, Aerosol));
  m_nameToMethod.Insert(PALETTE_AEROSOL2,	BRATHL_CALLBACK(CallbackVoid, Aerosol2));
  m_nameToMethod.Insert(PALETTE_OZONE,			BRATHL_CALLBACK(CallbackVoid, Ozone));
  m_nameToMethod.Insert(PALETTE_BLACKBODY,			BRATHL_CALLBACK(CallbackVoid, Blackbody));

  CObMap::iterator it;
  for (it = m_nameToMethod.begin() ; it != m_nameToMethod.end() ; it++)
  {
    m_colorTableList.push_back((it->first).c_str());
  }

  m_gradientFunction = "Gradient";
  m_customFunction = "Custom";
  m_nameToMethod.Insert(m_resetFunction,	BRATHL_CALLBACK(CallbackVoid, Reset));
  m_nameToMethod.Insert(m_gradientFunction,	BRATHL_CALLBACK(CallbackVoid, Gradient));
  m_nameToMethod.Insert(m_customFunction,	BRATHL_CALLBACK(CallbackVoid, Custom));

  m_nameToMethod.Insert("SetCurveLinear",	BRATHL_CALLBACK(CallbackVoid, SetCurveLinear));
  m_nameToMethod.Insert("SetCurveSQRT",		BRATHL_CALLBACK(CallbackVoid, SetCurveSQRT));
  m_nameToMethod.Insert("SetCurveCosinus",	BRATHL_CALLBACK(CallbackVoid, SetCurveCosinus));

  m_nameToMethod.Insert("LineUp",		BRATHL_CALLBACK(CallbackFlank, LineUp));
  m_nameToMethod.Insert("LineDown",		BRATHL_CALLBACK(CallbackFlank, LineDown));
  m_nameToMethod.Insert("SqrtUp",		BRATHL_CALLBACK(CallbackFlank, SqrtUp));
  m_nameToMethod.Insert("SqrtDown",		BRATHL_CALLBACK(CallbackFlank, SqrtDown));
  m_nameToMethod.Insert("CosUp",		BRATHL_CALLBACK(CallbackFlank, CosUp));
  m_nameToMethod.Insert("CosDown",		BRATHL_CALLBACK(CallbackFlank, CosDown));

  #undef BRATHL_CALLBACK

  SetCurveLinear();
  //ExecMethodDefaultColorTable();
}
//----------------------------------------
std::string CBratLookupTable::MethodToLabeledMethod(const std::string& method)
{
  CObMap::iterator it;
  std::string methodTmp = CTools::StringToLower(method);

  for (it = m_nameToMethod.begin() ; it != m_nameToMethod.end() ; it++)
  {
    std::string lowerMethod = CTools::StringToLower(it->first);
    if (methodTmp == lowerMethod)
    {
      return it->first;
    }
  }

  return "";
}
//----------------------------------------
std::string CBratLookupTable::CurveToLabeledCurve(const std::string& curve)
{
  CStringArray::iterator it;
  std::string curveTmp = CTools::StringToLower(curve);

  for (it = m_curveNames.begin() ; it != m_curveNames.end() ; it++)
  {
    std::string lowerCurve = CTools::StringToLower(*it);
    if (curveTmp == lowerCurve)
    {
      return *it;
    }
  }

  return "";
}

//----------------------------------------
bool CBratLookupTable::IsValidCurve(const std::string& curve)
{
  CStringArray::iterator it;
  for (it = m_curveNames.begin() ; it != m_curveNames.end() ; it++)
  {
    if (curve == *it)
    {
      return true;
    }
  }

  return false;

}

//----------------------------------------
bool CBratLookupTable::IsValidMethod(const std::string& methodName)
{
  CallbackVoid* method = static_cast< CallbackVoid* >(m_nameToMethod[methodName]);
  if (method == NULL)
  {
    return false;
  }

  return true;

}

//----------------------------------------
//femmDONE
//void CBratLookupTable::ExecMethod(const std::string& methodName)
//{
//  ExecMethod(std::string(methodName));
//}

//----------------------------------------
void CBratLookupTable::ExecMethod(const std::string& methodName)
{


  CallbackVoid* method = static_cast< CallbackVoid* >(m_nameToMethod[methodName]);
  if (method == NULL)
  {
    std::string msg = CTools::Format("ERROR in CBratLookupTable::ExecMethod : unknown methodname or not a CallbackVoid :'%s'",
                                 methodName.c_str());
    CException e(msg, BRATHL_LOGIC_ERROR);
    CTrace::Tracer("%s", e.what());
    Dump(*CTrace::GetDumpContext());
    throw (e);

  }

  try
  {
    method->execute();
  }
  catch (CException e)
  {
    std::string msg = CTools::Format("BRAT ERROR in CBratLookupTable::ExecMethod : std::exception while executing methodname :'%s'"
                                "\nNative Error : %s",
                                 methodName.c_str(),
                                 e.what());
    CException e2(msg, BRATHL_LOGIC_ERROR);
    CTrace::Tracer("%s",e2.what());
    Dump(*CTrace::GetDumpContext());
    throw (e2);
  }
  catch (std::exception e)
  {
    std::string msg = CTools::Format("BRAT RUNTIME ERROR in CBratLookupTable::ExecMethod : std::exception while executing methodname :'%s'"
                                "\nNative Error : %s",
                                 methodName.c_str(),
                                 e.what());
    CException e2(msg, BRATHL_LOGIC_ERROR);
    CTrace::Tracer("%s",e2.what());
    Dump(*CTrace::GetDumpContext());
    throw (e2);
  }
  catch (...)
  {
    std::string msg = CTools::Format("BRAT FATAL ERROR in CBratLookupTable::ExecMethod : std::exception while executing methodname :'%s'",
                                 methodName.c_str());
    CException e2(msg, BRATHL_LOGIC_ERROR);
    CTrace::Tracer("%s",e2.what());
    Dump(*CTrace::GetDumpContext());
    throw (e2);
  }


}

//----------------------------------------
void CBratLookupTable::ExecMethodDefaultColorTable()
{
  ExecMethod(m_defaultColorTable);
}
//----------------------------------------
void CBratLookupTable::ExecCurveMethod(const std::string& curve)
{
  std::string methodName = "SetCurve" + curve;

  ExecMethod(methodName);
}


//----------------------------------------
void CBratLookupTable::SetCurveUpDown(CallbackFlank* up, CallbackFlank* down)
{
  UpFlank = up;
  DownFlank = down;
}

//----------------------------------------
double CBratLookupTable::LineUp(double a, double b, double x)
{
  if ((x - 1) <= a)
  {
    return 0.0;
  }
  else if ((x + 1) >= b)
  {
    return 1.0;
  }
  else
  {
    return (x - a) / (b - a);
  }
}

//----------------------------------------
double CBratLookupTable::LineDown(double a, double b, double x)
{
  if ((x - 1) <= a)
  {
    return 0.0;
  }
  else if ((x + 1) >= b)
  {
    return -1.0;
  }
  else
  {
    return -1 * ( x - a) / (b - a);
  }
}

//----------------------------------------
double CBratLookupTable::SqrtUp(double a, double b, double x)
{
  if ((x - 1) <= a)
  {
    return 0.0;
  }
  else if ((x + 1) >= b)
  {
    return 1.0;
  }
  else
  {
    return CTools::Sqrt( (x - a) / (b - a) );
  }
}

//----------------------------------------
double CBratLookupTable::SqrtDown(double a, double b, double x)
{
  if ((x - 1) <= a)
  {
    return 0.0;
  }
  else if ((x + 1) >= b)
  {
    return -1.0;
  }
  else
  {
    return (CTools::Sqrt( 1 - (x - a) / (b - a) ) - 1);
  }
}

//----------------------------------------
double CBratLookupTable::CosUp(double a, double b, double x)
{

  if ((x - 1) <= a)
  {
    return 0.0;
  }
  else if ((x + 1) >= b)
  {
    return 1.0;
  }
  else
  {
    return 1 - ((CTools::Cos(((x - a) / (b - a)) * M_PI ) + 1) / 2);
  }
}

//----------------------------------------
double CBratLookupTable::CosDown(double a, double b, double x)
{
  if ((x - 1) <= a)
  {
    return 0.0;
  }
  else if ((x + 1) >= b)
  {
    return -1.0;
  }
  else
  {
    return -1 + ((CTools::Cos(((x - a) / (b - a)) * M_PI) + 1) / 2);
  }
}

//----------------------------------------
void CBratLookupTable::BlackToWhite()
{

  m_cust.RemoveAll();
  m_grad.RemoveAll();

  //m_std = "BlackToWhite";
  m_currentFunction = PALETTE_BLACKTOWHITE;

  double u = static_cast<double>(m_vtkLookupTable->GetNumberOfTableValues());

  for (int32_t i = 0 ; i <  m_vtkLookupTable->GetNumberOfTableValues() ; i++)
  {
    double r, g, b;
    double di = static_cast<double>(i);
    r = g = b = UpFlank->execute(0, u, di);
    m_vtkLookupTable->SetTableValue(i, r, g, b, 1.0);
  }

}

void CBratLookupTable::Black()
{

  m_cust.RemoveAll();
  m_grad.RemoveAll();

  //m_std = "BlackToWhite";
  m_currentFunction = PALETTE_BLACKTOWHITE;

  double u = static_cast<double>(m_vtkLookupTable->GetNumberOfTableValues());

  for (int32_t i = 0 ; i <  m_vtkLookupTable->GetNumberOfTableValues() ; i++)
  {
    double r, g, b;
    double di = static_cast<double>(i);
    r = g = b = UpFlank->execute(0, u, di);
    m_vtkLookupTable->SetTableValue(i, 0.0, 0.0, 0.0, 1.0);
  }

}

//----------------------------------------
void CBratLookupTable::WhiteToBlack()
{

  m_cust.RemoveAll();
  m_grad.RemoveAll();

  //m_std = "WhiteToBlack";
  m_currentFunction = PALETTE_WHITETOBLACK;

  double u = static_cast<double>(m_vtkLookupTable->GetNumberOfTableValues());

  for (int32_t i = 0 ; i <  m_vtkLookupTable->GetNumberOfTableValues() ; i++)
  {
    double r, g, b;
    double di = static_cast<double>(i);
    r = g = b = 1 + DownFlank->execute(0, u, di);
    m_vtkLookupTable->SetTableValue(i, r, g, b, 1.0);
  }

}

//----------------------------------------
void CBratLookupTable::GreenToRed()
{

  m_cust.RemoveAll();
  m_grad.RemoveAll();
  //m_std = "GreenToRed";
  m_currentFunction = PALETTE_GREENTORED;

  double u = static_cast<double>(m_vtkLookupTable->GetNumberOfTableValues());

  for (int32_t i = 0 ; i <  m_vtkLookupTable->GetNumberOfTableValues() ; i++)
  {
    double r, g;
    double di = static_cast<double>(i);
    r = UpFlank->execute(0, u, di);
    g = 1 + DownFlank->execute(0, u, di);
    m_vtkLookupTable->SetTableValue(i, r, g, 0.0, 1.0);
  }

}

//----------------------------------------
void CBratLookupTable::RedToGreen()
{

  m_cust.RemoveAll();
  m_grad.RemoveAll();
  //m_std = "RedToGreen";
  m_currentFunction = PALETTE_REDTOGREEN;

  double u = static_cast<double>(m_vtkLookupTable->GetNumberOfTableValues());

  for (int32_t i = 0 ; i <  m_vtkLookupTable->GetNumberOfTableValues() ; i++)
  {
    double r, g;
    double di = static_cast<double>(i);
    r = 1 + DownFlank->execute(0, u, di);
    g = UpFlank->execute(0, u, di);
    m_vtkLookupTable->SetTableValue(i, r, g, 0.0, 1.0);
  }

}

//----------------------------------------
void CBratLookupTable::Cloud()
{

  m_cust.RemoveAll();
  m_grad.RemoveAll();
  //m_std = "Cloud";
  m_currentFunction = PALETTE_CLOUD;

  double u = static_cast<double>(m_vtkLookupTable->GetNumberOfTableValues());

  for (int32_t i = 0 ; i <  m_vtkLookupTable->GetNumberOfTableValues() ; i++)
  {
    double r, g, b, a;
    double di = static_cast<double>(i);
    r = 1 + DownFlank->execute(0, 2.0 * u, di);
    g = 1 + DownFlank->execute(0, 2.0 * u, di);
    b = 1 + DownFlank->execute(0, 2.0 * u, di);
    a = UpFlank->execute(0, 2.0 * u, i);
    m_vtkLookupTable->SetTableValue(i, r, g, b, a);
  }

}

//----------------------------------------
void CBratLookupTable::Rainbow()
{

  m_cust.RemoveAll();
  m_grad.RemoveAll();
  //m_std = "Rainbow";
  m_currentFunction = PALETTE_RAINBOW;

  double u = (1.0/6) * static_cast<double>(m_vtkLookupTable->GetNumberOfTableValues());

  for (int32_t i = 0 ; i <  m_vtkLookupTable->GetNumberOfTableValues() ; i++)
  {
    double r, g, b;
    double di = static_cast<double>(i);
    r = 1 + DownFlank->execute(2.*u, 3.*u, di) + UpFlank->execute(4.*u, 6.*u, di);
    g = UpFlank->execute(0, 2.*u, di) + DownFlank->execute(3.*u, 4.*u, di);
    b = UpFlank->execute(3.*u, 4.*u, di);
    m_vtkLookupTable->SetTableValue(i, r, g, b, 1.0);
  }

}
//----------------------------------------
void CBratLookupTable::Rainbow2()
{

  m_cust.RemoveAll();
  m_grad.RemoveAll();
  //m_std = "Rainbow";
  m_currentFunction = PALETTE_RAINBOW2;

  double u = (1.0/6) * static_cast<double>(m_vtkLookupTable->GetNumberOfTableValues());

  int32_t j = m_vtkLookupTable->GetNumberOfTableValues() - 1;

  for (int32_t i = 0 ; i <  m_vtkLookupTable->GetNumberOfTableValues() ; i++)
  {
    double r, g, b;
    double di = static_cast<double>(i);
    r = 1 + DownFlank->execute(2.*u, 3.*u, di) + UpFlank->execute(4.*u, 6.*u, di);
    g = UpFlank->execute(0, 2.*u, di) + DownFlank->execute(3.*u, 4.*u, di);
    b = UpFlank->execute(3.*u, 4.*u, di);
    m_vtkLookupTable->SetTableValue(j, r, g, b, 1.0);
    j--;
  }

}

//----------------------------------------
void CBratLookupTable::Ozone()
{

  m_cust.RemoveAll();
  m_grad.RemoveAll();
  //m_std = "Ozone";
  m_currentFunction = PALETTE_OZONE;

  double u = (1.0/7) * static_cast<double>(m_vtkLookupTable->GetNumberOfTableValues());

  for (int32_t i = 0 ; i <  m_vtkLookupTable->GetNumberOfTableValues() ; i++)
  {
    double r, g, b;
    double di = static_cast<double>(i);
    r = UpFlank->execute(3*u, 4*u, di);
    g = UpFlank->execute(u, 2*u, di) + DownFlank->execute(4*u, 5*u, di) + UpFlank->execute(6*u, 7*u, di);
    b = UpFlank->execute(0, u, di) + DownFlank->execute(2*u, 3*u, di) + UpFlank->execute(5*u, 6*u, di);
    m_vtkLookupTable->SetTableValue(i, r, g, b, 1.0);
  }

}

//----------------------------------------
void CBratLookupTable::Blackbody()
{

  m_cust.RemoveAll();
  m_grad.RemoveAll();
  //m_std = "Blackbody";
  m_currentFunction = "Blackbody";

  double u = (1.0/3) * static_cast<double>(m_vtkLookupTable->GetNumberOfTableValues());

  for (int32_t i = 0 ; i <  m_vtkLookupTable->GetNumberOfTableValues() ; i++)
  {
    double r, g, b;
    double di = static_cast<double>(i);
    r = UpFlank->execute(0, u, di);
    g = UpFlank->execute(u, 2*u, di);
    b = UpFlank->execute(2*u, 3*u, di);
    m_vtkLookupTable->SetTableValue(i, r, g, b, 1.0);
  }

}

//----------------------------------------
void CBratLookupTable::Aerosol()
{

  m_cust.RemoveAll();
  m_grad.RemoveAll();
  //m_std = PALETTE_AEROSOL
  m_currentFunction = PALETTE_AEROSOL;

  double u = (1.0/4) * static_cast<double>(m_vtkLookupTable->GetNumberOfTableValues());

  for (int32_t i = 0 ; i <  m_vtkLookupTable->GetNumberOfTableValues() ; i++)
  {
    double r, g, b;
    double di = static_cast<double>(i);
    r = UpFlank->execute(2*u, 3*u, di);
    g = UpFlank->execute(0, 2*u, di) + DownFlank->execute(3*u, 4*u, di);
    b = SqrtUp(0, u, i) + DownFlank->execute(2*u, 3*u, di);
    m_vtkLookupTable->SetTableValue(i, r, g, b, 1.0);
  }

}

//----------------------------------------
void CBratLookupTable::Aerosol2()
{

  m_cust.RemoveAll();
  m_grad.RemoveAll();
  //m_std = PALETTE_AEROSOL
  m_currentFunction = PALETTE_AEROSOL2;

  double u = (1.0/4) * static_cast<double>(m_vtkLookupTable->GetNumberOfTableValues());
  int32_t j = m_vtkLookupTable->GetNumberOfTableValues() - 1;
  for (int32_t i = 0 ; i <  m_vtkLookupTable->GetNumberOfTableValues() ; i++)
  {
    double r, g, b;
    double di = static_cast<double>(i);
    r = UpFlank->execute(2*u, 3*u, di);
    g = UpFlank->execute(0, 2*u, di) + DownFlank->execute(3*u, 4*u, di);
    b = SqrtUp(0, u, i) + DownFlank->execute(2*u, 3*u, di);
    m_vtkLookupTable->SetTableValue(j, r, g, b, 1.0);
    j--;
  }

}



//----------------------------------------
void CBratLookupTable::Reset()
{

  m_cust.RemoveAll();
  m_grad.RemoveAll();
  //m_std = "Reset";
  m_currentFunction = m_resetFunction;

  for (int32_t i = 0 ; i <  m_vtkLookupTable->GetNumberOfTableValues() ; i++)
  {
    m_vtkLookupTable->SetTableValue(i, 0.0, 0.0, 0.0, 1.0);
  }

}

//----------------------------------------
void CBratLookupTable::SetCurveLinear()
{

  SetCurveUpDown(static_cast< CallbackFlank* >(m_nameToMethod["LineUp"]),
                       static_cast< CallbackFlank* >(m_nameToMethod["LineDown"]));

  m_curve = CURVE_LINEAR();

  Update();
}


//----------------------------------------
void CBratLookupTable::SetCurveSQRT()
{

  SetCurveUpDown(static_cast< CallbackFlank* >(m_nameToMethod["SqrtUp"]),
                       static_cast< CallbackFlank* >(m_nameToMethod["SqrtDown"]));

  m_curve = CURVE_SQRT();

  Update();
}
//----------------------------------------

void CBratLookupTable::SetCurveCosinus()
{
  SetCurveUpDown(static_cast< CallbackFlank* >(m_nameToMethod["CosUp"]),
                       static_cast< CallbackFlank* >(m_nameToMethod["CosDown"]));

  m_curve = CURVE_COSINUS();

  Update();
}
//----------------------------------------
int32_t CBratLookupTable::GetMaxCustomXValue()
{
  int32_t max = -1;

  CObArray::iterator it;

  if (m_cust.empty() == false)
  {
    it = m_cust.end() - 1;
    CCustomColor* customColor = dynamic_cast<CCustomColor*>(*it);
    if (customColor == NULL)
    {
      CException e("ERROR in CBratLookupTable::GetMaxCustomXValue - at least one of the array custom color object is not a CCustomColor object", BRATHL_LOGIC_ERROR);
      throw(e);
    }
    max = customColor->GetXValue();
  }

  /*
  for (it = m_cust.begin() ; it != m_cust.end() ; it++)
  {
    CCustomColor* customColor = dynamic_cast<CCustomColor*>(*it);
    if (customColor == NULL)
    {
      CException e("ERROR in CBratLookupTable::GetMaxCustomXValue - at least one of the array custom color object is not a CCustomColor object", BRATHL_LOGIC_ERROR);
      throw(e);
    }
    max = (max < customColor->GetXValue()) ? customColor->GetXValue() : max;
  }
*/

  return max;
}

//----------------------------------------
void CBratLookupTable::FacetsCorrection()
{
  // empty array --> nothing to do
  if (m_cust.empty())
  {
    return;
  }

  if (m_cust.size() < 2)
  {
    std::string msg = CTools::Format("ERROR in CBratLookupTable::FacetsCorrection() - size of custom color array is less 2 : %ld",
                                (long)m_cust.size());
    CException e(msg, BRATHL_LOGIC_ERROR);
    throw(e);
  }

  CCustomColor* cMin = dynamic_cast<CCustomColor*>(*(m_cust.begin()));
  CCustomColor* cMax = dynamic_cast<CCustomColor*>(*(m_cust.end() - 1));
  if ( (cMin == NULL) || (cMax == NULL) )
  {
    CException e("ERROR in CBratLookupTable::FacetsCorrection - at least one of the array custom color object is not a CCustomColor object", BRATHL_LOGIC_ERROR);
    throw(e);
  }
  // min value correction
  if (cMin->GetXValue() != 0)
  {
    cMin->SetXValue(0);
  }

  // max value correction
  if (cMax->GetXValue() != m_vtkLookupTable->GetNumberOfTableValues())
  {
    cMax->SetXValue(m_vtkLookupTable->GetNumberOfTableValues());
  }


  //now we are sure that :
  // m_cust size is >= 2;
  // min value != max value

  // duplicate value correction
  CObArray::iterator it = m_cust.begin();

  do
  {
    CCustomColor* c1 = dynamic_cast<CCustomColor*>(*it);
    CCustomColor* c2 = dynamic_cast<CCustomColor*>(*(it+1));
    if ( (c1 == NULL) || (c2 == NULL))
    {
      CException e("ERROR in CBratLookupTable::FacetsCorrection - at least one of the array custom color object is not a CCustomColor object", BRATHL_LOGIC_ERROR);
      throw(e);
    }

    if (c1->GetXValue() == c2->GetXValue())
    {
      //delete duplication value (c2)
      m_cust.Erase(it+1);
      c2 = NULL;
      it = m_cust.begin();
    }
    else
    {
      it++;
    }

  }
  while (it != m_cust.end() - 1);

}
//----------------------------------------
void CBratLookupTable::SetFacets(int32_t f)
{
  int32_t old = m_vtkLookupTable->GetNumberOfTableValues();

  m_vtkLookupTable->SetNumberOfTableValues(f);

  CObArray::iterator it;

  for (it = m_cust.begin() ; it != m_cust.end() ; it++)
  {
    CCustomColor* customColor = dynamic_cast<CCustomColor*>(*it);
    if (customColor == NULL)
    {
      CException e("ERROR in CBratLookupTable::SetFacets - at least one of the array custom color object is not a CCustomColor object", BRATHL_LOGIC_ERROR);
      throw(e);
    }
    int32_t newXValue = static_cast<int32_t>(static_cast<double>(customColor->GetXValue()) *
                                             static_cast<double>(f) / static_cast<double>(old));
    customColor->SetXValue(newXValue);
  }

  FacetsCorrection();

  Update();
}
//----------------------------------------
void CBratLookupTable::Update()
{
  ExecMethod(m_currentFunction);
}
//----------------------------------------
void CBratLookupTable::Gradient(const CPlotColor& c1, const CPlotColor& c2)
{
  m_cust.RemoveAll();
  m_grad.RemoveAll();
  m_grad.Insert(new CPlotColor(c1));
  m_grad.Insert(new CPlotColor(c2));

  Gradient();
}

//----------------------------------------
void CBratLookupTable::Gradient()
{
  m_cust.RemoveAll();

  if (m_grad.size() != 2)
  {
    std::string msg = CTools::Format("ERROR in CBratLookupTable::Gradient() - size of color array (must contains min/max) not equal 2 : %ld",
                                (long)m_grad.size());
    CException e(msg, BRATHL_LOGIC_ERROR);
    throw(e);
  }
  CPlotColor* c1 = dynamic_cast<CPlotColor*>(m_grad.at(0));
  CPlotColor* c2 = dynamic_cast<CPlotColor*>(m_grad.at(1));
  if ( (c1 == NULL) || (c2 == NULL))
  {
    CException e("ERROR in CBratLookupTable::Gradient() - at least one of the color object is not a CPlotColor object", BRATHL_LOGIC_ERROR);
    throw(e);
  }

  //m_std = m_resetFunction;
  m_currentFunction = m_gradientFunction;

  DrawGradient(*c1, *c2, 0, m_vtkLookupTable->GetNumberOfTableValues());

}


//----------------------------------------
void CBratLookupTable::Custom(const CCustomColor& c1, const CCustomColor& c2)
{
  m_grad.RemoveAll();

  m_cust.RemoveAll();
  m_cust.Insert(new CCustomColor(c1));
  m_cust.Insert(new CCustomColor(c2));

  Custom();
}

//----------------------------------------
void CBratLookupTable::Custom(const CObArray& cust)
{
  m_grad.RemoveAll();

  m_cust.RemoveAll();
  DupCustMap(cust);

  Custom();

}

//----------------------------------------
void CBratLookupTable::Custom()
{
  m_grad.RemoveAll();

  if (m_cust.size() < 2)
  {
    std::string msg = CTools::Format("ERROR in CBratLookupTable::Custom() - size of custom color array is less 2 : %ld",
                                (long)m_cust.size());
    CException e(msg, BRATHL_LOGIC_ERROR);
    throw(e);
  }

  //m_std = m_resetFunction;
  m_currentFunction = m_customFunction;
  int32_t maxXValue = GetMaxCustomXValue();
  if (maxXValue < 0)
  {
    std::string msg = CTools::Format("ERROR in CBratLookupTable::Custom() - max of X value is negative value : %d",
                                  maxXValue);
    CException e(msg, BRATHL_LOGIC_ERROR);
    throw(e);
  }

  int32_t u = static_cast<int32_t>(static_cast<double>(m_vtkLookupTable->GetNumberOfTableValues())
                                   / static_cast<double>(maxXValue));
  CObArray::iterator it;

  for (it = m_cust.begin() ; it != m_cust.end() - 1 ; it++)
  {
    CCustomColor* c1 = dynamic_cast<CCustomColor*>(*it);
    CCustomColor* c2 = dynamic_cast<CCustomColor*>(*(it+1));
    if ( (c1 == NULL) || (c2 == NULL))
    {
      CException e("ERROR in CBratLookupTable::Custom - at least one of the array custom color object is not a CCustomColor object", BRATHL_LOGIC_ERROR);
      throw(e);
    }

    DrawGradient(*(c1->GetVtkColor()), *(c2->GetVtkColor()),  c1->GetXValue() * u, c2->GetXValue() * u);

  }

}

//----------------------------------------

void CBratLookupTable::DrawGradient(const CPlotColor& c1, const CPlotColor& c2, int32_t i1, int32_t i2)
{
  double d1 = static_cast<double>(i1);
  double d2 = static_cast<double>(i2);

  for (int32_t i = i1 ; i < i2 ; i++)
  {
    double r, g, b, a;
    double di = static_cast<double>(i);
    r =   c1.Red()
        + c1.Red() * DownFlank->execute(d1, d2, di)
        + c2.Red() * UpFlank->execute(d1, d2, di);
    r = (r > 1) ? 1 : r;

    g =  c1.Green()
       + c1.Green() * DownFlank->execute(d1, d2, di)
       + c2.Green() * UpFlank->execute(d1, d2, di);
    g = (g > 1) ? 1 : g;

    b =  c1.Blue()
       + c1.Blue() * DownFlank->execute(d1, d2, di)
       + c2.Blue() * UpFlank->execute(d1, d2, di);
    b = (b > 1) ? 1 : b;

    a =  c1.Alpha()
       + c1.Alpha() * DownFlank->execute(d1, d2, di)
       + c2.Alpha() * UpFlank->execute(d1, d2, di);
    a = (a > 1) ? 1 : a;

    m_vtkLookupTable->SetTableValue(i, r, g, b, a);
  }
}

//----------------------------------------
void CBratLookupTable::DupCustMap(const CObArray& cust)
{
  CBratLookupTable::DupCustMap(cust, m_cust);
}

//----------------------------------------
void CBratLookupTable::DupCustMap(const CObArray& custSrc, CObArray& custDest)
{
  CObArray::const_iterator it;

  for (it = custSrc.begin() ; it != custSrc.end() ; it++)
  {
    CCustomColor* customColor = dynamic_cast<CCustomColor*>(*it);
    custDest.Insert(new CCustomColor(*customColor));
  }

}

//----------------------------------------
void CBratLookupTable::DupGradMap(const CObArray& grad)
{
  CObArray::const_iterator it;

  for (it = grad.begin() ; it != grad.end() ; it++)
  {
    CPlotColor* vtkColor = dynamic_cast<CPlotColor*>(*it);
    m_grad.Insert(new CPlotColor(*vtkColor));
  }

}
//----------------------------------------
void CBratLookupTable::SaveToFile(const std::string& fileName)
{
  bool bOk = true;
  m_fileName = fileName;
  try
  {
    ::wxRemoveFile(m_fileName);
  }
  catch (...)
  {
    //do nothing
  }

  wxFileConfig file(wxGetApp().GetAppName(), wxEmptyString, m_fileName, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);

//  file.DeleteAll();

  bOk &= file.Write(ENTRY_NUMCOLORS(), static_cast<int>(m_vtkLookupTable->GetNumberOfTableValues()));
  bOk &= file.Write(ENTRY_CURRENTFCT(), m_currentFunction.c_str());
  bOk &= file.Write(ENTRY_CURVE(), m_curve.c_str());
  //bOk &= file.Write(ENTRY_STD, m_std.c_str());
  if (bOk == false)
  {
    CException e(CTools::Format("ERROR in CBratLookupTable::SaveToFile - Can't write file %s", m_fileName.c_str()),
                 BRATHL_LOGIC_ERROR);
    throw(e);
  }

  SaveGradToFile(file);

  SaveCustToFile(file);
}
//----------------------------------------
void CBratLookupTable::SaveCustToFile(wxFileConfig& file)
{
  bool bOk = true;

  if (m_cust.size() <= 0)
  {
    return;
  }

  if (m_cust.size() < 2)
  {
    std::string msg = CTools::Format("ERROR in CBratLookupTable::SaveCustToFile() - size of custom color array is less 2 : %ld",
                                (long)m_cust.size());
    CException e(msg, BRATHL_LOGIC_ERROR);
    throw(e);
  }


  CObArray::const_iterator it;

  for (it = m_cust.begin() ; it != m_cust.end() ; it++)
  {
    CCustomColor* customColor = dynamic_cast<CCustomColor*>(*it);
    if (customColor == NULL)
    {
      CException e("ERROR in CBratLookupTable::SaveCustToFile - at least one of the array custom color object is not a CCustomColor object", BRATHL_LOGIC_ERROR);
      throw(e);
    }

    CPlotColor* vtkColor = customColor->GetVtkColor();
    if (vtkColor == NULL)
    {
      CException e("ERROR in CBratLookupTable::SaveCustToFile - In Custom Color object there isn't a CPlotColor object", BRATHL_LOGIC_ERROR);
      throw(e);
    }


    bOk &= file.Write(brathlFmtEntryColorMacro("R", it - m_cust.begin()), vtkColor->Red());
    bOk &= file.Write(brathlFmtEntryColorMacro("G", it - m_cust.begin()), vtkColor->Green());
    bOk &= file.Write(brathlFmtEntryColorMacro("B", it - m_cust.begin()), vtkColor->Blue());
    bOk &= file.Write(brathlFmtEntryColorMacro("A", it - m_cust.begin()), vtkColor->Alpha());
    bOk &= file.Write(brathlFmtEntryColorMacro("Value", it - m_cust.begin()), customColor->GetXValue());
  }

  if (bOk == false)
  {
    CException e(CTools::Format("ERROR in CBratLookupTable::SaveToFile - Can't write file %s",
                                m_fileName.c_str()),
                 BRATHL_LOGIC_ERROR);
    throw(e);
  }

}
//----------------------------------------
void CBratLookupTable::SaveGradToFile(wxFileConfig& file)
{
  bool bOk = true;

  if (m_grad.size() <= 0)
  {
    return;
  }
  if (m_grad.size() != 2)
  {
    std::string msg = CTools::Format("ERROR in CBratLookupTable::SaveGradToFile() - size of color array (must contains min/max) not equal 2 : %ld",
                                (long)m_grad.size());
    CException e(msg, BRATHL_LOGIC_ERROR);
    throw(e);
  }

  CPlotColor* c1 = dynamic_cast<CPlotColor*>(m_grad.at(0));
  CPlotColor* c2 = dynamic_cast<CPlotColor*>(m_grad.at(1));
  if ( (c1 == NULL) || (c2 == NULL))
  {
    CException e("ERROR in CBratLookupTable::SaveGradToFile() - at least one of the color object is not a CPlotColor object", BRATHL_LOGIC_ERROR);
    throw(e);
  }

  bOk &= file.Write(brathlFmtEntryColorMacro("R", 0), c1->Red());
  bOk &= file.Write(brathlFmtEntryColorMacro("G", 0), c1->Green());
  bOk &= file.Write(brathlFmtEntryColorMacro("B", 0), c1->Blue());
  bOk &= file.Write(brathlFmtEntryColorMacro("A", 0), c1->Alpha());
  bOk &= file.Write(brathlFmtEntryColorMacro("Value", 0), 0);

  bOk &= file.Write(brathlFmtEntryColorMacro("R", 1), c2->Red());
  bOk &= file.Write(brathlFmtEntryColorMacro("G", 1), c2->Green());
  bOk &= file.Write(brathlFmtEntryColorMacro("B", 1), c2->Blue());
  bOk &= file.Write(brathlFmtEntryColorMacro("A", 1), c2->Alpha());
  bOk &= file.Write(brathlFmtEntryColorMacro("Value", 1), static_cast<int>(m_vtkLookupTable->GetNumberOfTableValues()));

  if (bOk == false)
  {
    CException e(CTools::Format("ERROR in CBratLookupTable::SaveToFile - Can't write file %s",
                                m_fileName.c_str()),
                 BRATHL_LOGIC_ERROR);
    throw(e);
  }
}
//----------------------------------------
void CBratLookupTable::HandleLoadError(bool bOk, const std::string& entry)
{
  if (bOk == false)
  {
    CException e(CTools::Format("ERROR in CBratLookupTable::LoadFromFile - Can't read '%s' from file %s",
                                entry.c_str(), m_fileName.c_str()),
                 BRATHL_LOGIC_ERROR);
    throw(e);

  }

}
//----------------------------------------
void CBratLookupTable::HandleLoadColorError(bool bOk, const std::string& entry, double& color)
{
  HandleLoadError(bOk, entry);

  if (color < 0.0)
  {
    ::wxMessageBox(CTools::Format("Invalid value %f for '%s'\n Value will bet set to 0", color, entry.c_str()),
                   "Warning",
                    wxOK | wxCENTRE | wxICON_EXCLAMATION);
    color = 0.0;
  }

  if (color > 1.0)
  {
    ::wxMessageBox(CTools::Format("Invalid value %f for '%s'\n Value will bet set to 1", color, entry.c_str()),
                   "Warning",
                    wxOK | wxCENTRE | wxICON_EXCLAMATION);
    color = 1.0;
  }
}
//----------------------------------------
bool CBratLookupTable::LoadFromFile(const std::string& fileName)
{
  bool bOk = true;

  CBratLookupTable* lutBackup = new CBratLookupTable(*this);

  m_fileName = fileName;

  wxFileConfig file(wxGetApp().GetAppName(), wxEmptyString, m_fileName, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);

  int32_t valueLong;
  wxString valueString;

  bOk = file.Read(ENTRY_NUMCOLORS(), &valueLong);
  HandleLoadError(bOk, (const char*)ENTRY_NUMCOLORS());
  valueLong = (valueLong > 65536) ? 65536 : valueLong;
  m_vtkLookupTable->SetNumberOfTableValues(valueLong);

  //bOk = file.Read(ENTRY_STD, valueString);
  //HandleLoadError(bOk, ENTRY_STD);
  //m_std = valueString.c_str();

  bOk = file.Read(ENTRY_CURVE(), &valueString);
  HandleLoadError(bOk, (const char*)ENTRY_CURVE());
  m_curve = valueString.c_str();

  //if ( (m_curve != CURVE_LINEAR()) || (m_curve != CURVE_SQRT()) || (m_curve != CURVE_COSINUS()) )
  if ( IsValidCurve(m_curve) == false )
  {
    ::wxMessageBox(CTools::Format("Unknown entry value '%s' for '%s' in file '%s'\n Linear value will be set",
                                    m_curve.c_str(), (const char*)ENTRY_CURVE(), m_fileName.c_str()),
                   "Warning",
                    wxOK | wxCENTRE | wxICON_EXCLAMATION);

    m_curve = CURVE_LINEAR();

  }

  bOk = file.Read(ENTRY_CURRENTFCT(), &valueString);
  HandleLoadError(bOk, (const char*)ENTRY_CURRENTFCT());
  m_currentFunction = valueString.c_str();


  if (m_currentFunction == m_customFunction)
  {
    bOk = LoadCustFromFile(file);
  }
  else if (m_currentFunction == m_gradientFunction)
  {
    bOk = LoadGradFromFile(file);
  }
  else
  {
    ::wxMessageBox(CTools::Format("Unknown entry value '%s' for '%s' in file '%s'\n Color table will not be loaded",
                                    m_currentFunction.c_str(), (const char*)ENTRY_CURRENTFCT(), m_fileName.c_str()),
                   "Warning",
                    wxOK | wxCENTRE | wxICON_EXCLAMATION);
    bOk = false;

  }

  if (bOk == false)
  {
    *this = *lutBackup;
  }

  delete lutBackup;
  lutBackup = NULL;

  ExecMethod(m_currentFunction);

  ExecCurveMethod(m_curve);

  return bOk;
}
//----------------------------------------
bool CBratLookupTable::LoadGradFromFile(wxFileConfig& file)
{
  bool bOk = true;
  m_grad.RemoveAll();
  m_cust.RemoveAll();

  wxString group;
  long dummy;
  file.SetPath("/");

  bOk = file.GetFirstGroup(group, dummy);

  double r, g, b, a;
  r = g = b = a = 0.0;
  int32_t xValue = 0;

  bool isColorDef = false;
  bool bError = false;

  while (bOk)
  {
    isColorDef =  (GROUP_COLOR().CmpNoCase(group.Left(GROUP_COLOR().Length())) == 0);
    if (isColorDef)
    {
      bool bOkEntry = true;
      bOkEntry = file.Read(group + "/R", &r);
      HandleLoadColorError(bOkEntry, (const char*)group, r);
      bOkEntry = file.Read(group + "/G", &g);
      HandleLoadColorError(bOkEntry, (const char*)group, g);
      bOkEntry = file.Read(group + "/B", &b);
      HandleLoadColorError(bOkEntry, (const char*)group, b);
      bOkEntry = file.Read(group + "/A", &a);
      HandleLoadColorError(bOkEntry, (const char*)group, a);
      bOkEntry = file.Read(group + "/Value", &xValue);
      HandleLoadError(bOkEntry, (const char*)group);
      if ( (xValue != 0 ) && (xValue != m_vtkLookupTable->GetNumberOfTableValues() ))
      {
        std::string msg = CTools::Format("Invalid value %d for '%s/Value' \n Color table will not be loaded",
                                        xValue,
                                        (const char*)group);
        ::wxMessageBox(msg,
                       "Warning",
                        wxOK | wxCENTRE | wxICON_EXCLAMATION);
        bError = true;
        //----------
        break;
        //----------
      }


      CPlotColor* vtkColor = new CPlotColor(r, g, b, a);
      if (xValue == 0)
      {
        m_grad.InsertAt(m_grad.begin(), vtkColor);
      }
      else
      {
        m_grad.InsertAt(m_grad.end(), vtkColor);
      }

    }

    bOk = file.GetNextGroup(group, dummy);
  }

  if (bError)
  {
    m_grad.RemoveAll();
  }


  if (m_grad.size() <= 0)
  {
    return false;
  }

  if (m_grad.size() != 2)
  {
    ::wxMessageBox(CTools::Format("There are %ld color definitions in file '%s'\n File must contains only 2 (min/max)\n"
                                    "Color table will not be loaded", (long)m_grad.size(), m_fileName.c_str()),
               "Warning",
                wxOK | wxCENTRE | wxICON_EXCLAMATION);
    bError = true;
  }

  return (bError == false);

}
//----------------------------------------
bool CBratLookupTable::LoadCustFromFile(wxFileConfig& file)
{

  bool bOk = true;
  m_grad.RemoveAll();
  m_cust.RemoveAll();

  wxString group;
  long dummy;
  file.SetPath("/");

  bOk = file.GetFirstGroup(group, dummy);

  double r, g, b, a;
  r = g = b = a = 0.0;
  int32_t xValue = 0;

  bool isColorDef = false;
  bool bError = false;

  while (bOk)
  {
    isColorDef =  (GROUP_COLOR().CmpNoCase(group.Left(GROUP_COLOR().Length())) == 0);
    if (isColorDef)
    {
      bool bOkEntry = true;
      bOkEntry = file.Read(group + "/R", &r);
      HandleLoadColorError(bOkEntry, (const char*)group, r);
      bOkEntry = file.Read(group + "/G", &g);
      HandleLoadColorError(bOkEntry, (const char*)group, g);
      bOkEntry = file.Read(group + "/B", &b);
      HandleLoadColorError(bOkEntry, (const char*)group, b);
      bOkEntry = file.Read(group + "/A", &a);
      HandleLoadColorError(bOkEntry, (const char*)group, a);
      bOkEntry = file.Read(group + "/Value", &xValue);
      HandleLoadError(bOkEntry, (const char*)group);
      if ( (xValue < 0 ) || (xValue > m_vtkLookupTable->GetNumberOfTableValues() ))
      {
        ::wxMessageBox(CTools::Format("Invalid value %d for '%s'\n"
                                        "Correct range is [%d,%d]\n"
                                        "Color table will not be loaded",
                                         xValue, (const char*)group, 0, static_cast<int>(m_vtkLookupTable->GetNumberOfTableValues()) ),
                       "Warning",
                        wxOK | wxCENTRE | wxICON_EXCLAMATION);
        bError = true;
        //----------
        break;
        //----------
      }


      CCustomColor* c = new CCustomColor(CPlotColor(r, g, b, a), xValue);

        std::string warning;
        InsertCustomColor(c, warning);
    }

    bOk = file.GetNextGroup(group, dummy);
  }

  if (bError)
  {
    m_cust.RemoveAll();
  }


  if (m_cust.size() <= 0)
  {
    return false;
  }

  if (m_cust.size() < 2)
  {
    ::wxMessageBox(CTools::Format("There are %ld color definitions in file '%s'\n File must contains at least 2\n"
                                    "Color table will not be loaded", (long)m_cust.size(), m_fileName.c_str()),
               "Warning",
                wxOK | wxCENTRE | wxICON_EXCLAMATION);
    bError = true;
  }

  return (bError == false);

}

//----------------------------------------
int32_t CBratLookupTable::InsertCustomColor(CCustomColor *color, std::string &warning )
{
  if (m_cust.size() <= 0)
  {
    m_cust.Insert(color);
    return 0;
  }

  CCustomColor* cMin = dynamic_cast<CCustomColor*>(*(m_cust.begin()));
  CCustomColor* cMax = dynamic_cast<CCustomColor*>(*(m_cust.end() - 1));
  if ( (cMin == NULL) || (cMax == NULL) )
  {
    CException e("ERROR in CBratLookupTable::InsertCustomColor - at least one of the array custom color object is not a CCustomColor object", BRATHL_LOGIC_ERROR);
    throw(e);
  }


  bool duplicate = false;
  CObArray::iterator it;

  for (it = m_cust.begin() ; it != m_cust.end() ; it++)
  {
    CCustomColor* c = dynamic_cast<CCustomColor*>(*it);
    if (c == NULL)
    {
      CException e("ERROR in CBratLookupTable::InsertCustomColor - at least one of the array custom color object is not a CCustomColor object", BRATHL_LOGIC_ERROR);
      throw(e);
    }

    if (c->GetXValue() > color->GetXValue())
    {
      duplicate = false;
      break;
    }
    else if (c->GetXValue() == color->GetXValue())
    {
      duplicate = true;
      break;
    }
  }

  if (duplicate)
  {
	  warning = "There are duplicate color definitions (value " + color->GetStringXValue() + ") in file " + m_fileName + "\n Color definition will be ignored";
               //"Warning",
               // wxOK | wxCENTRE | wxICON_EXCLAMATION);
	  return -1;
  }

  CObArray::iterator itInserted = m_cust.InsertAt(it, color);

  CObArray::difference_type diff ;
  diff = distance (m_cust.begin ( ) , itInserted );

  return diff;


}
//----------------------------------------
void CBratLookupTable::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }


  fOut << "==> Dump a CBratLookupTable Object at "<< this << std::endl;


  fOut << "m_currentFunction = " << m_currentFunction << std::endl;
  fOut << "m_curve " << m_curve << std::endl;
//  fOut << "m_std = " << m_std << std::endl;
  fOut << "m_vtkLookupTable at " << m_vtkLookupTable << std::endl;

  m_nameToMethod.Dump(fOut);


  fOut << "==> END Dump a CBratLookupTable Object at "<< this << std::endl;

  fOut << std::endl;

}

