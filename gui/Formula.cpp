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


#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "Formula.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "brathl.h"

#include "new-gui/Common/tools/Trace.h"
#include "Tools.h"
#include "new-gui/Common/tools/Exception.h"
using namespace brathl;

#include "BratProcess.h"
using namespace processes;

#include "BratGui.h"
#include "Workspace.h"
#include "Formula.h"

const char* CFormula::FMT_FLOAT_XY = "%-#.10g";
const char* CFormula::FMT_FLOAT_LATLON = "%-#.15g";


//-------------------------------------------------------------
//------------------- CFormula class --------------------
//-------------------------------------------------------------


//----------------------------------------

CFormula::CFormula()
{
  Init();

}
//----------------------------------------
CFormula::CFormula(const wxString& name, bool predefined, const wxString& description, const wxString& comment)
{
  Init();

  m_name = name;
  m_description = description;
  m_comment = comment;
  m_predefined = predefined;

}
//----------------------------------------
CFormula::CFormula(const std::string& name, bool predefined, const std::string& description, const std::string& comment)
{
  Init();

  m_name = name.c_str();
  m_description = description.c_str();
  m_comment = comment.c_str();
  m_predefined = predefined;
}
//----------------------------------------
CFormula::CFormula(const CFormula& formula)
{
  Init();
  Copy(formula);
}

//----------------------------------------

CFormula::~CFormula()
{
}

//----------------------------------------
CFormula& CFormula::operator=(const CFormula& formula)
{
  Copy(formula);
  return *this;
}
//----------------------------------------
void CFormula::Init()
{
  m_type = CMapTypeField::typeOpAsField;
  //m_dataType = CMapTypeData::typeOpData;
  m_dataType = -1;
  m_filter = CMapTypeFilter::filterNone;
  //m_step = DEFAULT_STEP_LATLON_ASSTRING;
  m_step = "";
  setDefaultValue(m_minValue);
  setDefaultValue(m_maxValue);
  setDefaultValue(m_interval);
  setDefaultValue(m_loessCutOff);

  m_dataMode = CMapDataMode::GetInstance().GetDefault();

  m_predefined = false;

}
//----------------------------------------
void CFormula::Copy(const CFormula& formula)
{
  m_name = formula.m_name;
  m_description = formula.m_description;
  m_comment = formula.m_comment;
  m_predefined = formula.m_predefined;
  m_type = formula.m_type;
  m_dataType = formula.m_dataType;
  m_filter = formula.m_filter;
  m_interval = formula.m_interval;
  m_loessCutOff = formula.m_loessCutOff;
  m_minValue = formula.m_minValue;
  m_maxValue = formula.m_maxValue;
  m_step = formula.m_step;
  m_title = formula.m_title;
  m_unit = formula.m_unit;
  m_dataMode = formula.m_dataMode;


}
//----------------------------------------
void CFormula::SetDefaultMinMax()
{
  setDefaultValue(m_minValue);
  setDefaultValue(m_maxValue);

  switch (m_dataType)
  {
  case CMapTypeData::typeOpLongitude :
    {
      SetMinValue(ConvertToFormulaUnit(-180));
      SetMaxValue(ConvertToFormulaUnit(180));
      break;
    }
  case CMapTypeData::typeOpLatitude :
    {
      SetMinValue(ConvertToFormulaUnit(-90));
      SetMaxValue(ConvertToFormulaUnit(90));
      break;
    }
  default :
    {
      break;
    }
  }

}

//----------------------------------------
wxString CFormula::GetMinValueAsText()
{
  wxString str;
  wxString format = GetFormatString();

  if (IsTimeDataType())
  {
    str = GetMinValueAsDateString(format);
  }
  else
  {
    str =  CTools::TrailingZeroesTrim( (const char *)wxString::Format(format, m_minValue).c_str() ).c_str();
  }

  return str;
}
//----------------------------------------
wxString CFormula::GetMaxValueAsText()
{
  wxString str;
  wxString format = GetFormatString();

  if (IsTimeDataType())
  {
    str = GetMaxValueAsDateString(format);
  }
  else
  {
    str = CTools::TrailingZeroesTrim( (const char *)wxString::Format(format, m_maxValue).c_str() ).c_str();
  }

  return str;
}
//----------------------------------------
wxString CFormula::GetFormatString()
{
  wxString format;

  if (IsLatLonDataType())
  {
    CUnit unit(m_unit.BaseUnit());
    if (unit == m_unit)
    {
      format = CFormula::FMT_FLOAT_LATLON;
    }
    else
    {
      format = CFormula::FMT_FLOAT_XY;
    }

  }
  else if (IsTimeDataType())
  {
    ; // "" as default format for date time
  }
  else
  {
    format = CFormula::FMT_FLOAT_XY;
  }

  return format;
}
//----------------------------------------
void CFormula::SetDataType(int32_t typeField, const CUnit& unit, CProduct* product)
{

  switch (typeField)
  {
  case CMapTypeField::typeOpAsX :
    {
      if (unit.IsDate())
      {
        SetDataType(CMapTypeData::typeOpT);
      }
      else if (unit.IsCompatible(CLatLonPoint::m_DEFAULT_UNIT_LONGITUDE))
      {
        SetDataType(CMapTypeData::typeOpLongitude);

        if (product != NULL)
        {
          if ( (product->IsLatitudeFieldName((const char *)m_name.c_str())) || (CTools::FindNoCase((const char *)m_name.c_str(), "lat") == 0) )
          {
            SetDataType(CMapTypeData::typeOpLatitude);
          }
        }
      }
      else
      {
        SetDataType(CMapTypeData::typeOpX);
      }

      break;
    }
  case CMapTypeField::typeOpAsY :
    {
      if (unit.IsDate())
      {
        SetDataType(CMapTypeData::typeOpT);
      }
      else if (unit.IsCompatible(CLatLonPoint::m_DEFAULT_UNIT_LATITUDE))
      {
        SetDataType(CMapTypeData::typeOpLatitude);
        if (product != NULL)
        {
          if ( (product->IsLongitudeFieldName((const char *)m_name.c_str()))  || (CTools::FindNoCase((const char *)m_name.c_str(), "lon") == 0) )
          {
            SetDataType(CMapTypeData::typeOpLongitude);
          }
        }

      }
      else
      {
        SetDataType(CMapTypeData::typeOpY);
      }
      break;
    }
  case CMapTypeField::typeOpAsField :
      SetDataType(CMapTypeData::typeOpData);
      break;
  case CMapTypeField::typeOpAsSelect :
      SetDataType(CMapTypeData::typeOpData);
      break;
  default :
      SetDataType(CMapTypeData::typeOpData);
      break;
  }

}

//----------------------------------------
wxString CFormula::GetFilterAsLabel()
{
  if (m_filter == CMapTypeFilter::filterNone)
  {
    return wxString::Format(FORMAT_FILTER_LABEL_NONE,
                             GetName().c_str());
  }
  else
  {
    return wxString::Format(FORMAT_FILTER_LABEL,
                            GetName().c_str(),
                            GetFilterAsString().c_str());

  }
}

//----------------------------------------
wxString CFormula::GetResolutionAsLabel(bool hasFilter)
{
  wxString loessCutOffLabel;
  wxString intervalLabel;

  if (hasFilter)
  {
    if (isDefaultValue(m_loessCutOff))
    {
      loessCutOffLabel = wxString::Format(FORMAT_LOESS_CUT_OFF_LABEL,
                                          "Not defined");
    }
    else
    {
      loessCutOffLabel = wxString::Format(FORMAT_LOESS_CUT_OFF_LABEL,
                                          GetLoessCutOffAsText().c_str());
    }
  }

  if (isDefaultValue(m_interval))
  {
    intervalLabel = wxString::Format(FORMAT_INTERVAL_LABEL,
                                          "Not defined");
  }
  else
  {
    intervalLabel = wxString::Format(FORMAT_INTERVAL_LABEL,
                                        GetIntervalAsText().c_str());
  }

  return wxString::Format(FORMAT_XY_RESOLUTION_LABEL,
            (isDefaultValue(m_minValue) ? "Not defined" : GetMinAsString().c_str()),
            (isDefaultValue(m_maxValue) ? "Not defined" : GetMaxAsString().c_str()),
            loessCutOffLabel.c_str(),
            GetStep().c_str(),
            intervalLabel.c_str());
}

//----------------------------------------
wxString CFormula::GetMinAsString()
{
  return GetMinValueAsText();
}

//----------------------------------------
wxString CFormula::GetMaxAsString()
{
  return GetMaxValueAsText();
}


//----------------------------------------
double CFormula::GetStepAsDouble()
{
  CExpression expression;
  CExpressionValue exprValue;
  double result = 0.0;

  SetStepToDefaultAsNecessary();

  wxString errorMsg;

  bool bOk = CFormula::SetExpression(m_step, expression, errorMsg);

  if (!bOk)
  {
    wxMessageBox(errorMsg,
                 "Warning",
                  wxOK | wxCENTRE | wxICON_EXCLAMATION);
    SetStepToDefault();
    CFormula::SetExpression(m_step, expression, errorMsg);
  }

  if (expression.GetFieldNames()->size() != 0)
  {
    wxMessageBox("Fieldnames are not allowed for step in formula",
                 "Warning",
                  wxOK | wxCENTRE | wxICON_EXCLAMATION);
    SetStepToDefault();
    CFormula::SetExpression(m_step, expression, errorMsg);
  }

  try
  {
    exprValue = expression.Execute();
    result = *(exprValue.GetValues());
  }
  catch (CException& e)
  {
    wxMessageBox(wxString::Format("Formula '%s':\nSyntax for step ('%s') is not valid\nReason:\n%s",
                                  m_name.c_str(),
                                  m_step.c_str(),
                                  e.what()),
                "Warning",
                wxOK | wxICON_EXCLAMATION);

    result = CTools::m_defaultValueDOUBLE;
  }
  catch (std::exception& e)
  {
    wxMessageBox(wxString::Format("Formula '%s':\nSyntax for step ('%s') is not valid\nReason:\n%s",
                                  m_name.c_str(),
                                  m_step.c_str(),
                                  e.what()),
                "Warning",
                wxOK | wxICON_EXCLAMATION);

    result = CTools::m_defaultValueDOUBLE;
  }
  catch (...)
  {
    wxMessageBox(wxString::Format("Formula '%s':\nSyntax for step ('%s') is not valid\n",
                                  m_name.c_str(),
                                  m_step.c_str()),
                "Warning",
                wxOK | wxICON_EXCLAMATION);

    result = CTools::m_defaultValueDOUBLE;
  }

  return result;
}
//----------------------------------------
double CFormula::GetStepAsDouble(const wxString& step)
{
  CExpression expression;
  CExpressionValue exprValue;
  double result = 0.0;
  if (step.IsEmpty())
  {
   return CTools::m_defaultValueDOUBLE;
  }

  wxString errorMsg;

  bool bOk = CFormula::SetExpression(step, expression, errorMsg);

  if (!bOk)
  {
    wxMessageBox(errorMsg,
                 "Warning",
                  wxOK | wxCENTRE | wxICON_EXCLAMATION);
    CFormula::SetExpression(DEFAULT_STEP_GENERAL_ASSTRING, expression, errorMsg);
  }

  if (expression.GetFieldNames()->size() != 0)
  {
    wxMessageBox("Fieldnames are not allowed for step in formula",
                 "Warning",
                  wxOK | wxCENTRE | wxICON_EXCLAMATION);
    CFormula::SetExpression(DEFAULT_STEP_GENERAL_ASSTRING, expression, errorMsg);
  }

  try
  {

    exprValue = expression.Execute();
    result = *(exprValue.GetValues());
  }
  catch (CException& e)
  {
    wxMessageBox(wxString::Format("Syntax for step ('%s') is not valid\nReason:\n%s",
                                  step.c_str(),
                                  e.what()),
                "Warning",
                wxOK | wxICON_EXCLAMATION);

    result = CTools::m_defaultValueDOUBLE;
  }
  catch (std::exception& e)
  {
    wxMessageBox(wxString::Format("Syntax for step ('%s') is not valid\nReason:\n%s",
                                  step.c_str(),
                                  e.what()),
                "Warning",
                wxOK | wxICON_EXCLAMATION);

    result = CTools::m_defaultValueDOUBLE;
  }
  catch (...)
  {
    wxMessageBox(wxString::Format("Syntax for step ('%s') is not valid\n",
                                  step.c_str()),
                "Warning",
                wxOK | wxICON_EXCLAMATION);

    result = CTools::m_defaultValueDOUBLE;
  }

  return result;


}
//----------------------------------------
wxString CFormula::GetDefaultUnit()
{
  return GetDefaultUnit(m_dataType);
}
//----------------------------------------
wxString CFormula::GetDefaultUnit(int32_t dataType)
{
  CUnit unit;

  switch (dataType)
  {
    case CMapTypeData::typeOpData:
      //unit = DEFAULT_UNIT_COUNT.c_str();
      break;
    case CMapTypeData::typeOpLongitude:
      unit = CLatLonPoint::m_DEFAULT_UNIT_LONGITUDE;
      break;
    case CMapTypeData::typeOpLatitude:
      unit = CLatLonPoint::m_DEFAULT_UNIT_LATITUDE;
      break;
    case CMapTypeData::typeOpT:
      unit = CDate::m_DEFAULT_UNIT_SECOND;
      break;
    default:
      break;
  }

  return unit.GetText().c_str();

}

//----------------------------------------
bool CFormula::CheckExpressionUnits(const wxString& exprStr, const wxString& record, const wxString& strUnitExpr, CProduct* product, wxString& errorMsg)
{
  bool bOk = true;

  if (product == NULL)
  {
    return bOk;
  }


  CExpression expr;
  bOk &= CFormula::SetExpression(exprStr, expr, errorMsg);

  if (!bOk)
  {
    return bOk;
  }

  if (!expr.HasFieldNames())
  {
    return bOk;
  }

  if (expr.HasConstants())
  {
    return bOk;
  }
  if (expr.GetNbFieldNames() > 1)
  {
    return bOk;
  }
  //Expression contains only one field

  wxString fieldName = expr.GetFieldNames()->at(0).c_str();

  CField* field = product->FindFieldByName((const char *)fieldName.c_str(), (const char *)record.c_str(), false, NULL, true);

  if (field == NULL)
  {
    return bOk;
  }

  CUnit unitExpr;

  try
  {
    unitExpr = (const char *)strUnitExpr.c_str();
  }
  catch (CException& e)
  {
    errorMsg.Append(wxString::Format("The unit of the expression ('%s') is not valid.\nReason\n'%s'",
                                  strUnitExpr.c_str(),
                                  e.what()));
    return false;
  }

  if (CFormula::IsDefaultUnitData(unitExpr))
  {
    return bOk;
  }

  std::string msgErrorUnits;
  if (! unitExpr.IsCompatible(field->GetUnit(), &msgErrorUnits) )
  {
    if (!msgErrorUnits.empty())
    {
      errorMsg.Append(wxString::Format("%s", msgErrorUnits.c_str()));
    }
    else
    {
      errorMsg.Append(wxString::Format("The unit of the field '%s': '%s' is not compatible with the unit of the expression: '%s'\n",
                                        fieldName.c_str(),
                                        field->GetUnit().c_str(),
                                        strUnitExpr.c_str()));
    }
    bOk = false;

  }

  return bOk;
}
//----------------------------------------
bool CFormula::CheckFieldNames(const CExpression& expr, const wxString& record, CProduct* product, CStringArray& fieldNamesNotFound)
{
  if (product == NULL)
  {
    return true;
  }

  return product->CheckFieldNames(expr, (const char *)record.c_str(), fieldNamesNotFound);
}

//----------------------------------------
bool CFormula::CheckFieldNames(const wxString& exprStr, const wxString& record, CProduct* product, wxString& exprStrOut, wxString& errorMsg)
{
  bool bOk = true;

  CExpression expr;
  bOk &= CFormula::SetExpression(exprStr, expr, errorMsg);

  if (!bOk)
  {
    return bOk;
  }

  CExpression exprOut;
  bOk &= CFormula::CheckFieldNames(expr, record, product, exprOut, errorMsg);

  try
  {
    exprStrOut = exprOut.AsString().c_str();
  }
  catch (CException& e)
  {
    errorMsg.Append(wxString::Format("Syntax is not valid\nReason:\n%s",
                                  e.what()));
    bOk = false;

  }

  return bOk;
}
//----------------------------------------
bool CFormula::CheckFieldNames(const CExpression& expr, const wxString& record, CProduct* product, CExpression& exprOut, wxString& errorMsg)
{
  if (product == NULL)
  {
    return true;
  }

  try
  {
    exprOut = expr;
  }
  catch (CException& e)
  {
    errorMsg.Append(wxString::Format("Syntax is not valid\nReason:\n%s",
                                  e.what()));
    return false;

  }


  CStringArray fieldNamesNotFound;
  //Try to correct lower/upper case error in field names.
  product->ReplaceNamesCaseSensitive(expr, fieldNamesNotFound, exprOut);

  //Add record name to field names
  std::string errorString;
  bool recordNameAdded = product->AddRecordNameToField(expr, (const char *)record.c_str(), exprOut, errorString);

  if (!recordNameAdded)
  {
    errorMsg.Append("\n");
    errorMsg.Append(errorString.c_str());
    errorMsg.Append("\n");
  }


  fieldNamesNotFound.RemoveAll();
  bool bOk = CFormula::CheckFieldNames(exprOut, record, product, fieldNamesNotFound);


/*
  CStringArray fieldNamesNotFound;
  bool bOk = CFormula::CheckFieldNames(expr, record, product, fieldNamesNotFound);

  //Try to correct lower/upper case error in field names.

  if (!bOk)
  {
    product->ReplaceNamesCaseSensitive(expr, fieldNamesNotFound, exprOut);
    fieldNamesNotFound.RemoveAll();
    bOk = CFormula::CheckFieldNames(exprOut, record, product, fieldNamesNotFound);
  }

  std::string errorString;

  bool recordNameAdded = product->AddRecordNameToField(expr, record.c_str(), exprOut, errorString);

  if (!recordNameAdded)
  {
    errorMsg.Append("\n");
    errorMsg.Append(errorString.c_str());
    errorMsg.Append("\n");
  }

  fieldNamesNotFound.RemoveAll();
  bOk = CFormula::CheckFieldNames(exprOut, record, product, fieldNamesNotFound);
*/
  if (!bOk)
  {
    errorMsg.Append("Fields below are unknown:\n\n");
    CStringArray::const_iterator it;

    for (it = fieldNamesNotFound.begin() ; it != fieldNamesNotFound.end() ; it++)
    {
      errorMsg.Append("\t");
      errorMsg.Append((*it).c_str());
      errorMsg.Append("\n");
    }
  }

  return bOk;
}
//----------------------------------------
bool CFormula::CheckExpression(const wxString& value, const wxString& record, wxString& errorMsg, wxString* strUnitExpr,
                               const CStringMap* aliases /* = NULL*/, CProduct* product /* = NULL*/, wxString* valueOut /*= NULL*/)
{

  wxString tmp = value;
  tmp.Trim(true);
  tmp.Trim(false);
  if (tmp.IsEmpty())
  {
    errorMsg.Append("Data expression is empty. You have to set it.");
    return false;
  }


  bool bOk = true;
  std::string str = (const char *)value.c_str();
  uint32_t numberVarsExpanded = 0;

  // Group fields aliases and formulas aliases in a single map.
  // Add fields aliases first then formulas aliases
  // If aliases are duplicated (fields / formulas): fields aliases will be overidden by formulas aliases
  //CStringMap allAliases;
  //CProduct::GroupAliases(product, aliases, allAliases);

  const CStringMap* fieldAliases = CProduct::GetAliasesAsString(product);


  if ((aliases != NULL) || (fieldAliases != NULL))
  {

    CStringArray aliasesFound;

    try
    {
      CTools::FindAliases((const char *)value.c_str(), aliasesFound, true, "%", true, aliases, fieldAliases, true);
    }
    catch(CException& e)
    {
      errorMsg.Append(wxString::Format("Unable to find aliases (formulas and fields) from:\n%s\n.\nReason\n'%s'",
                                    value.c_str(),
                                    e.what()));
      return false;
    }
    //CTrace::GetInstance()->SetTraceLevel(5);
    //aliasesFound.Dump();

    CStringArray aliasesFoundUnique;
    aliasesFoundUnique.InsertUnique(aliasesFound);

      CWorkspaceFormula* wks = wxGetApp().GetCurrentWorkspaceFormula();
      if (wks != NULL)
      {
        wks->AmendFormulas(aliasesFoundUnique, product, record);
      }

    // Expand fields aliases and formulas aliases
    std::string errorString;
    str = CTools::ExpandVariables((const char *)value.c_str(), aliases, fieldAliases, true, '%', &numberVarsExpanded, false, &errorString);
    if (!errorString.empty())
    {
      errorMsg.Append(wxString::Format("Unable to expand aliases (formulas and fields) from:\n%s\n.\nReason\n'%s'",
                                    value.c_str(),
                                    errorString.c_str()));    
      return false;
    }

  }

  //CExpression expr;
  //bOk &= CFormula::SetExpression(str.c_str(), expr, errorMsg);

  //CExpression exprOut;
  wxString expStrOut;
  bOk &= CFormula::CheckFieldNames(str.c_str(), record, product, expStrOut, errorMsg);

  if (bOk)
  {
    if (valueOut != NULL)
    {
      if (numberVarsExpanded == 0)
      {
        *valueOut = expStrOut;
      }
      else
      {
        //Try to correct lower/upper case error in field names.
        std::string out;
        product->ReplaceNamesCaseSensitive((const char *)value.c_str(), out);

        //Add record name to field names
        std::string out2 = out;
        std::string errorString;
        product->AddRecordNameToField(out2, (const char *)record.c_str(), out, errorString);


        *valueOut = out.c_str();
      }

    }
  }
  if ((bOk) && (strUnitExpr != NULL))
  {
    bOk &= CFormula::CheckExpressionUnits(expStrOut, record, *strUnitExpr, product, errorMsg);
  }


  return bOk;
}

//----------------------------------------
bool CFormula::SetExpression(const char* value, CExpression& expr, wxString& errorMsg)
{
  wxString str(value);
  return SetExpression(str, expr, errorMsg);

}
//----------------------------------------
bool CFormula::SetExpression(const std::string& value, CExpression& expr, wxString& errorMsg)
{
  wxString str(value.c_str());
  return SetExpression(str, expr, errorMsg);

}
//----------------------------------------
bool CFormula::SetExpression(const wxString& value, CExpression& expr, wxString& errorMsg)
{
  bool bOk = true;

  try
  {
    expr.SetExpression((const char *)value.c_str());
  }
  catch (CException& e)
  {
    errorMsg.Append(wxString::Format("Syntax is not valid\nReason:\n%s",
                                  e.what()));
    bOk = false;

  }

  return bOk;
}
//----------------------------------------
bool CFormula::GetFields(CStringArray& fields, wxString& errorMsg, const CStringMap* aliases /* = NULL*/, const CStringMap* fieldAliases /*= NULL*/)
{
  CExpression expr;

  bool bOk = CFormula::SetExpression(GetDescription(true, aliases, fieldAliases), expr, errorMsg);
  if (!bOk)
  {
    return bOk;
  }
  // Copy fields names contained in expression
  fields = *(expr.GetFieldNames());

  return bOk;
}

//----------------------------------------
bool CFormula::CheckExpression(wxString& errorMsg, const wxString& record,
                               const CStringMap* aliases /* = NULL*/, CProduct* product /* = NULL*/, wxString* valueOut /*= NULL*/)
{

  wxString stringExpr = GetDescription(true);

  // Empty expression is allowed for selection criteria
  if ((this->GetType() == CMapTypeField::typeOpAsSelect) && (stringExpr.IsEmpty()))
  {
    return true;
  }

  wxString str;
  wxString valueOutTemp;
  wxString strUnit = m_unit.AsString(false).c_str();

  bool bOk = CFormula::CheckExpression(stringExpr, record, str, &strUnit, aliases, product, &valueOutTemp);

  valueOutTemp = valueOutTemp.Trim();

  if (bOk && (stringExpr.Cmp(valueOutTemp) != 0) && valueOutTemp.IsEmpty())
  {
    SetDescription(valueOutTemp);
  }

  if (valueOut != NULL)
  {
    *valueOut = GetDescription(true);
  }

  if (!bOk)
  {
    errorMsg.Append(wxString::Format("\nFormula '%s':\n\t%s\n", m_name.c_str(), str.c_str()));
  }

  return bOk;
}

//----------------------------------------
double CFormula::LonNormal360(double value)
{

  CUnit unit((const char *)GetDefaultUnit(CMapTypeData::typeOpLongitude).c_str());
  unit.SetConversionFrom(m_unit);

  value = unit.Convert(value);
  value = CLatLonPoint::LonNormal360(value);

  unit	= m_unit.BaseUnit();
  unit.SetConversionTo(m_unit);

  value	= unit.Convert(value);

  return value;

}
//----------------------------------------

bool CFormula::ControlResolution(wxString& errorMsg)
{
  bool bOk = true;

  if (!IsXYType())
  {
    return bOk;
  }

  wxString str;


  if (isDefaultValue(m_minValue))
  {
    str.Append("\n\tMin. value is not defined.");
    bOk &= false;
  }

  if (isDefaultValue(m_maxValue))
  {
    str.Append("\n\tMax. value is not defined.");
    bOk &= false;
  }

  if (isDefaultValue(m_interval))
  {
    str.Append("\n\tNumber of intervals is not defined.");
    bOk &= false;
  }

  if (!isDefaultValue(m_minValue) || !isDefaultValue(m_maxValue))
  {
    bOk &= CtrlMinMaxValue(errorMsg);
  }

  if (!bOk)
  {
    errorMsg.Append(wxString::Format("\nFormula '%s' - Errors on resolution:%s\n", m_name.c_str(), str.c_str()));
  }

  return bOk;
}
//----------------------------------------
bool CFormula::CtrlMinMaxValue(wxString& errorMsg)
{
  bool bOk = true;
  double min = m_minValue;
  double max = m_maxValue;

  if (IsLonDataType())
  {
    if (min >= max)
    {
      max = LonNormal360(max);
      //formula.SetMaxValue(max);
      //ctrlMax->SetValue(max);
    }
  }
  if (min >= max)
  {
    errorMsg.Append("\n\tMinimun value must be strictly less than Maximum value."),
    bOk = false;
  }

  return bOk;
}


//----------------------------------------
bool CFormula::IsFieldType()
{
  return (GetType() == CMapTypeField::typeOpAsField);
}
//----------------------------------------
bool CFormula::IsXType()
{
  return (GetType() == CMapTypeField::typeOpAsX);
}
//----------------------------------------
bool CFormula::IsYType()
{
  return (GetType() == CMapTypeField::typeOpAsY);
}
//----------------------------------------
bool CFormula::IsXYType()
{
  return ( IsXType() ||
           IsYType() );
}
//----------------------------------------
bool CFormula::IsDataTypeSet()
{
  return (GetDataType() >= 0);
}
//----------------------------------------
bool CFormula::IsLatLonDataType()
{
  return ( IsLonDataType() ||
           IsLatDataType() );
}
//----------------------------------------
bool CFormula::IsLonDataType()
{
  return (GetDataType() == CMapTypeData::typeOpLongitude);
}
//----------------------------------------
bool CFormula::IsLatDataType()
{
  return (GetDataType() == CMapTypeData::typeOpLatitude);
}
//----------------------------------------
bool CFormula::IsTimeDataType()
{
  return (GetDataType() == CMapTypeData::typeOpT);
}
//----------------------------------------
bool CFormula::IsXYLatLon()
{
  return IsXYType() && IsLatLonDataType();
}
//----------------------------------------
bool CFormula::IsXYTime()
{
  return IsXYType() && IsTimeDataType();
}
//----------------------------------------
bool CFormula::ControlUnitConsistency(wxString& errorMsg)
{
  bool bOk = IsUnitCompatible();
  if (!bOk)
  {

    errorMsg.Append(wxString::Format("\nFormula '%s': \nUnit '%s' is not compatible with type '%s'.\nYou have to change either unit, or type\n",
                                  m_name.c_str(),
                                  GetUnitAsText().c_str(),
                                  GetDataTypeAsString().c_str()));
  }

  return bOk;
}
//----------------------------------------
bool CFormula::IsDefaultUnitData()
{
  return CFormula::IsDefaultUnitData(m_unit);
}
//----------------------------------------
bool CFormula::IsDefaultUnitData(const CUnit& unit)
{
  return CFormula::IsDefaultUnitData(unit.GetText().c_str());
}
//----------------------------------------
bool CFormula::IsDefaultUnitData(const wxString& unitText)
{
  return ( unitText.IsEmpty() || (unitText.CmpNoCase(DEFAULT_UNIT_COUNT) == 0) );
}
//----------------------------------------
bool CFormula::IsUnitCompatible()
{
  return IsUnitCompatible(m_dataType, m_unit);
}
//----------------------------------------
bool CFormula::IsUnitCompatible(const CUnit& unit)
{
  return IsUnitCompatible(m_dataType, unit);
}
//----------------------------------------
bool CFormula::IsUnitCompatible(const wxString& unitText)
{
  return IsUnitCompatible(m_dataType, unitText);
}
//----------------------------------------
bool CFormula::IsUnitCompatible(int32_t dataType, const CUnit& unit)
{
  bool bOk = true;

  //To avoid compilation error on Linux : 2 steps
  std::string defaultUnit = (const char *)GetDefaultUnit(dataType).c_str();
  CUnit unitDataType = defaultUnit;

  if (CFormula::IsDefaultUnitData(unitDataType))
  {
    return bOk;
  }

  try
  {
    bOk = unitDataType.IsCompatible(unit);
  }
  catch (CException& e)
  {
    e.what(); // to avoid warning compilation
    bOk = false;
  }

  return bOk;
}
//----------------------------------------
bool CFormula::IsUnitCompatible(int32_t dataType, const wxString& unitText)
{
  CUnit unit((const char *)GetDefaultUnit(dataType).c_str());

  return CFormula::IsUnitCompatible(dataType, unit);
}

//----------------------------------------
bool CFormula::IsDefaultUnit()
{
//  wxString unit(GetDefaultUnit(m_dataType).c_str());
//  return unit.CmpNoCase(m_unit.GetText().c_str()) == 0;
    return CFormula::IsDefaultUnit(m_unit.GetText().c_str(), m_dataType);
}

//----------------------------------------
bool CFormula::IsDefaultUnit(const char * value, int32_t dataType)
{

  wxString unit(GetDefaultUnit(dataType).c_str());

  return unit.CmpNoCase(value) == 0;
}
//----------------------------------------
void CFormula::SetDefaultUnit()
{

  m_unit = (const char *)GetDefaultUnit(m_dataType).c_str();

  SetStepToDefaultAsNecessary();

/*
  switch (m_dataType)
  {
    case CMapTypeData::typeOpData:
      m_unit = DEFAULT_UNIT_COUNT.c_str();
      break;
    case CMapTypeData::typeOpLongitude:
      m_unit = DEFAULT_UNIT_LONGITUDE.c_str();
      break;
    case CMapTypeData::typeOpLatitude:
      m_unit = DEFAULT_UNIT_LATITUDE.c_str();
      break;
    default:
      break;
  }
*/
}
//----------------------------------------
void CFormula::SetUnit(const wxString& value, const wxString& defaultValue /*= ""*/, bool withMsg /*= true*/, bool convertMinMax /*= false*/)
{
  if (convertMinMax)
  {
    ConvertToMinMaxFormulaBaseUnit();
  }

  try
  {
    if (value.IsEmpty())
    {
      SetDefaultUnit();
    }
    else
    {
      m_unit = (const char *)value.c_str();
    }
  }
  catch (CException& e)
  {
    if (withMsg)
    {
      wxMessageBox(wxString::Format("Formula '%s' - Invalid unit '%s'\nReason\n'%s'",
                                    m_name.c_str(),
                                    value.c_str(),
                                    e.what()),
                  "Warning",
                  wxOK | wxICON_EXCLAMATION);
    }
    m_unit = (const char *)defaultValue.c_str();
  }

  SetStepToDefaultAsNecessary();

  if (convertMinMax)
  {
    ConvertToMinMaxFormulaUnit();
  }

}
//----------------------------------------
void CFormula::SetUnit(const CUnit& value, const wxString& defaultValue /*= ""*/, bool withMsg /*= true*/, bool convertMinMax /*= false*/)
{
  if (convertMinMax)
  {
    if (!IsTimeDataType())
    {
      ConvertToMinMaxFormulaBaseUnit();
    }
  }

  try
  {
    m_unit = value;
  }
  catch (CException& e)
  {
    if (withMsg)
    {
      wxMessageBox(wxString::Format("Formula '%s' - Invalid unit '%s'\nReason\n'%s'",
                                    m_name.c_str(),
                                    value.GetText().c_str(),
                                    e.what()),
                  "Warning",
                  wxOK | wxICON_EXCLAMATION);
    }
    m_unit = (const char *)defaultValue.c_str();
  }

  SetStepToDefaultAsNecessary();

  if (convertMinMax)
  {
    if (!IsTimeDataType())
    {
      ConvertToMinMaxFormulaUnit();
    }
  }

}
//----------------------------------------
wxString CFormula::GetFieldPrefix(int32_t type)
{
  wxString result;
  switch (type)
  {
    case CMapTypeField::typeOpAsField:
      result = "FIELD";
      break;
    case CMapTypeField::typeOpAsX:
      result = "X";
      break;
    case CMapTypeField::typeOpAsY:
      result = "Y";
      break;
    default:
      result = "FIRLD";
      break;
  }
  return result;

}
//----------------------------------------
wxString CFormula::GetFieldPrefix()
{
  return GetFieldPrefix(m_type);
}
//----------------------------------------
wxString CFormula::GetExportAsciiFieldPrefix()
{
  return GetFieldPrefix(CMapTypeField::typeOpAsField);
}

//----------------------------------------
bool CFormula::LoadConfigDesc(wxFileConfig* config, const wxString& path)
{
  bool bOk = true;
  if (config == NULL)
  {
    return true;
  }

  config->SetPath("/" + path);

  m_description = config->Read(m_name, m_description);

  return true;
}
//----------------------------------------
bool CFormula::LoadConfig(wxFileConfig* config, const wxString& pathSuff)
{
  bool bOk = true;
  wxString valueString;

  if (config == NULL)
  {
    return true;
  }
  wxString path = m_name;
  if (pathSuff.IsEmpty() == false)
  {
    path += "_" + pathSuff;
  }

  config->SetPath("/" + path);

  m_description = config->Read(ENTRY_DEFINE, m_description);
  m_comment = config->Read(ENTRY_COMMENT, m_comment);

  valueString = config->Read(ENTRY_UNIT, m_unit.GetText().c_str());
  SetUnit(valueString);

  valueString = config->Read(ENTRY_FIELDTYPE, GetTypeAsString());
  if (valueString.IsEmpty())
  {
    m_type = CMapTypeField::typeOpAsField;
  }
  else
  {
    m_type = CMapTypeField::GetInstance().NameToId(valueString);
  }

  valueString = config->Read(ENTRY_DATATYPE, GetDataTypeAsString());
  if (valueString.IsEmpty())
  {
    m_dataType = CMapTypeData::typeOpData;
  }
  else
  {
    m_dataType = CMapTypeData::GetInstance().NameToId(valueString);
  }

  m_title = config->Read(ENTRY_TITLE, m_title);

  valueString = config->Read(ENTRY_FILTER, GetFilterAsString());
  if (valueString.IsEmpty())
  {
    m_filter = CMapTypeFilter::filterNone;
  }
  else
  {
    m_filter = CMapTypeFilter::GetInstance().NameToId(valueString);
  }
  if (IsTimeDataType())
  {
    valueString = config->Read(ENTRY_MINVALUE);
    SetMinValueFromDateString(valueString);

    valueString = config->Read(ENTRY_MAXVALUE);
    SetMaxValueFromDateString(valueString);
  }
  else
  {
    config->Read(ENTRY_MINVALUE, &m_minValue, CTools::m_defaultValueDOUBLE);
    config->Read(ENTRY_MAXVALUE, &m_maxValue, CTools::m_defaultValueDOUBLE);
  }
  
  // 3.3.1 note: wxWidgets asserts if value >= INT_MAX, but CTools::m_defaultValueINT32 is INT_MAX, so,
  // if value is not read and the default is used, we have a failed assertion. This was not changed, to
  // avoid unknown implications.

  config->Read(ENTRY_INTERVAL, &m_interval, CTools::m_defaultValueINT32);		
  m_step = config->Read(ENTRY_STEP, DEFAULT_STEP_GENERAL_ASSTRING);
  if (m_step.IsEmpty())
  {
    m_step = DEFAULT_STEP_GENERAL_ASSTRING;
  }

  config->Read(ENTRY_LOESSCUTOFF, &m_loessCutOff, CTools::m_defaultValueINT32);

  valueString = config->Read(ENTRY_DATA_MODE);
  if (valueString.IsEmpty())
  {
    m_dataMode = -1;
  }
  else
  {
    m_dataMode = CMapDataMode::GetInstance().NameToId(valueString);
  }

  return true;
}
//----------------------------------------
bool CFormula::SaveConfigDesc(wxFileConfig* config, const wxString& path)
{
  bool bOk = true;
  if (config == NULL)
  {
    return true;
  }

  config->SetPath("/" + path);

  bOk &= config->Write(m_name, GetDescription(true));
  //bOk &= config->Write(ENTRY_COMMENT, m_comment);

  return true;
}

//----------------------------------------
bool CFormula::SaveConfigPredefined(wxFileConfig* config, const wxString& pathSuff)
{
  bool bOk = true;
  if (config == NULL)
  {
    return true;
  }
  wxString path = m_name;
  if (pathSuff.IsEmpty() == false)
  {
    path += "_" + pathSuff;
  }
  config->SetPath("/" + path);

  bOk &= config->Write(ENTRY_DEFINE, GetDescription(true));
  bOk &= config->Write(ENTRY_COMMENT, GetComment(true));
  bOk &= config->Write(ENTRY_UNIT, m_unit.GetText().c_str());

  return true;
}
//----------------------------------------
bool CFormula::SaveConfig(wxFileConfig* config, const wxString& pathSuff)
{
  if (m_predefined)
  {
    return SaveConfigPredefined(config, pathSuff);
  }

  bool bOk = true;
  if (config == NULL)
  {
    return true;
  }
  wxString path = m_name;
  if (pathSuff.IsEmpty() == false)
  {
    path += "_" + pathSuff;
  }
  config->SetPath("/" + path);

  bOk &= config->Write(ENTRY_DEFINE, GetDescription(true));
  bOk &= config->Write(ENTRY_COMMENT, GetComment(true));
  bOk &= config->Write(ENTRY_UNIT, m_unit.GetText().c_str());
  bOk &= config->Write(ENTRY_FIELDTYPE, GetTypeAsString());
  bOk &= config->Write(ENTRY_DATATYPE, GetDataTypeAsString());
  bOk &= config->Write(ENTRY_TITLE, m_title);
  bOk &= config->Write(ENTRY_FILTER, GetFilterAsString());
  if (isDefaultValue(m_minValue) == false)
  {
    if (IsTimeDataType())
    {
      bOk &= config->Write(ENTRY_MINVALUE, GetMinValueAsDateString());
    }
    else
    {
      bOk &= config->Write(ENTRY_MINVALUE, CTools::Format("%.15g", m_minValue).c_str());
    }
  }
  if (isDefaultValue(m_maxValue) == false)
  {
    if (IsTimeDataType())
    {
      bOk &= config->Write(ENTRY_MAXVALUE, GetMaxValueAsDateString());
    }
    else
    {
      bOk &= config->Write(ENTRY_MAXVALUE, CTools::Format("%.15g", m_maxValue).c_str());
    }
  }
  if (isDefaultValue(m_interval) == false)
  {
    bOk &= config->Write(ENTRY_INTERVAL, m_interval);
  }
  if (m_step.IsEmpty() == false)
  {
    bOk &= config->Write(ENTRY_STEP, m_step);
  }
  if (isDefaultValue(m_loessCutOff) == false)
  {
    bOk &= config->Write(ENTRY_LOESSCUTOFF, m_loessCutOff);
  }
  bOk &= config->Write(ENTRY_DATA_MODE,
                       GetDataModeAsString());

  return true;
}
//----------------------------------------
wxString CFormula::GetDescription(bool removeCRLF /* = false */, const CStringMap* formulaAliases /* = NULL*/, const CStringMap* fieldAliases /* = NULL*/)
{
  if (removeCRLF == false)
  {
    return m_description;
  }

  wxString str = m_description;
  str.Replace("\n", " ");
  
  if ((formulaAliases != NULL) || (fieldAliases != NULL)) 
  {
    str = (const char *)CTools::ExpandVariables((const char *)str.c_str(), formulaAliases, fieldAliases, true, '%', NULL, false).c_str();
  }

  return str;
}
//----------------------------------------
wxString CFormula::GetComment(bool removeCRLF)
{
  if (removeCRLF == false)
  {
    return m_comment;
  }

  wxString str = m_comment;
  str.Replace("\n", " ");
  return str;
}
//----------------------------------------
wxString CFormula::GetAlias()
{
  return wxString::Format("%%{%s}", m_name.c_str());
}

//----------------------------------------
void CFormula::SetMinValue(CDate& d)
{
  if (d.IsDefaultValue())
  {
    setDefaultValue(m_minValue);
    return;
  }

  m_minValue = d.Value();
}
//----------------------------------------
void CFormula::SetMinValueFromDateString(const wxString& value)
{
  CDate d(value);
  SetMinValue(d);
}
//----------------------------------------
void CFormula::SetMaxValue(CDate& d)
{
  if (d.IsDefaultValue())
  {
    setDefaultValue(m_maxValue);
    return;
  }

  m_maxValue = d.Value();
}
//----------------------------------------
void CFormula::SetMaxValueFromDateString(const wxString& value)
{
  CDate d(value);
  SetMaxValue(d);
}
//----------------------------------------
void CFormula::ConvertToMinMaxFormulaBaseUnit()
{
  if (!IsTimeDataType())
  {
    m_minValue = ConvertToFormulaBaseUnit(m_minValue);
    m_maxValue = ConvertToFormulaBaseUnit(m_maxValue);
  }

  ComputeInterval();
}
//----------------------------------------
void CFormula::ConvertToMinMaxFormulaUnit()
{
  if (!IsTimeDataType())
  {
    m_minValue = ConvertToFormulaUnit(m_minValue);
    m_maxValue = ConvertToFormulaUnit(m_maxValue);
  }

  ComputeInterval();
}
//----------------------------------------
bool CFormula::ConvertToFormulaBaseUnit(double in, double& out)
{
  out = m_unit.ConvertToBaseUnit(in);

  return !isDefaultValue(out);;

}
//----------------------------------------
double CFormula::ConvertToFormulaBaseUnit(double in)
{
  double out;
  ConvertToFormulaBaseUnit(in, out);

  return out;
}

//----------------------------------------
bool CFormula::ConvertToFormulaUnit(double in, double& out)
{
  out = m_unit.ConvertToUnit(in);

  return !isDefaultValue(out);;
}
//----------------------------------------
double CFormula::ConvertToFormulaUnit(double in)
{
  double out;
  ConvertToFormulaUnit(in, out);
  return out;
}

//----------------------------------------
bool CFormula::ComputeInterval(bool showMsg)
{
  bool bOk = true;

  if (!IsXYType())
  {
    return true;
  }

/*
  if (IsTimeDataType())
  {
    bOk = ComputeIntervalAsDate(showMsg);
  }
  else
  {
    bOk = ComputeIntervalAsDouble(showMsg);
  }

  return bOk;
*/

  setDefaultValue(m_interval);

  if (!IsXYType())
  {
    return true;
  }

  if (isDefaultValue(m_minValue) && isDefaultValue(m_maxValue))
  {
    return true;
  }

  double interval = 0.0;
  double intervalTmp = 0.0;
  double step = GetStepAsDouble();

  if (isDefaultValue(step))
  {
    return true;
  }

  double minValueDefault =  0.0;
  double maxValueDefault =  1.0;

  if (IsLonDataType())
  {
    ConvertToFormulaUnit(-180, minValueDefault);
    ConvertToFormulaUnit(180, maxValueDefault);
  }
  else if (IsLatDataType())
  {
    ConvertToFormulaUnit(-90, minValueDefault);
    ConvertToFormulaUnit(90, maxValueDefault);
  }
  else if (IsTimeDataType())
  {
    ConvertToFormulaUnit(0, minValueDefault);
    ConvertToFormulaUnit(CDate::m_secInDay, maxValueDefault);
  }

  if (isDefaultValue(m_minValue))
  {
    m_minValue = minValueDefault;
  }
  if (isDefaultValue(m_maxValue))
  {
    m_maxValue = maxValueDefault;
  }

  if (step < 0)
  {
    step = -(step);
    m_step.Replace("-", "");
  }

  if (isZero(step))
  {
    SetStepToDefault();

    step = CFormula::GetStepAsDouble(m_step);
  }

/*
  CUnit	unit	= m_unit.BaseUnit();
  unit.SetConversionTo(m_unit);
  double minValue	= unit.Convert(m_minValue);
  double maxValue	= unit.Convert(m_maxValue);
*/
  double minValue	= m_minValue;
  double maxValue	= m_maxValue;

  if (IsLonDataType())
  {
    if (minValue >= maxValue)
    {
      maxValue	= LonNormal360(maxValue);
    }
  }
  else if (IsTimeDataType())
  {
    CUnit	unit	= m_unit.BaseUnit();
    unit.SetConversionTo(m_unit);
    minValue	= unit.Convert(m_minValue);
    maxValue	= unit.Convert(m_maxValue);
  }


  //intervalTmp = ((m_maxValue - m_minValue) / step) / CDate::m_secInDay;
  intervalTmp = ((maxValue - minValue) / step);
  //interval = CTools::Round( ((m_maxValue - m_minValue) / step) / CDate::m_secInDay);
  interval = CTools::Round( ((maxValue - minValue) / step));

  m_interval = static_cast<int32_t>( CTools::Int(interval) );

  if (m_interval <= 0)
  {
    m_interval = 1;
  }


  if ( (showMsg) && (areEqual( interval, intervalTmp ) == false) )
  {
    wxMessageBox(wxString::Format("Formula '%s':\nInterval was round up or down to the nearest integer value.",
                                  m_name.c_str()),
                 "Information",
                  wxOK | wxCENTRE | wxICON_INFORMATION);
  }

  return areEqual( interval, intervalTmp );

}
/*
//----------------------------------------
bool CFormula::ComputeIntervalAsDouble(bool showMsg)
{
  setDefaultValue(m_interval);

  if (!IsXYType())
  {
    return true;
  }
  if (isDefaultValue(m_minValue) && isDefaultValue(m_maxValue))
  {
    return true;
  }

  double interval = 0.0;
  double intervalTmp = 0.0;
  double step = GetStepAsDouble();

  if (isDefaultValue(step))
  {
    return true;
  }

  double minValueDefault =  0.0;
  double maxValueDefault =  0.0;

  if (IsLonDataType())
  {
    minValueDefault = -180 : -90;
    maxValueDefault = 180 : 90;
  }
  else if (IsLatDataType())
  {
    minValueDefault = -90;
    maxValueDefault = 90;
  }

  if (isDefaultValue(m_minValue))
  {
    m_minValue = minValueDefault;
  }
  if (isDefaultValue(m_maxValue))
  {
    m_maxValue = maxValueDefault;
  }

  if (step < 0)
  {
    step = -(step);
    m_step.Replace("-", "");
  }

  if (isZero(step))
  {
    SetStepToDefault();

    step = CFormula::GetStepAsDouble(m_step);
  }

  intervalTmp = (m_maxValue - m_minValue) / step;
  interval = CTools::Round( (m_maxValue - m_minValue) / step );

  m_interval = static_cast<int32_t>( CTools::Int(interval) );

  if (m_interval <= 0)
  {
    m_interval = 1;
  }

  if ( (showMsg) && (areEqual( interval, intervalTmp ) == false) )
  {
    wxMessageBox(wxString::Format("Formula '%s':\nInterval was round up or down to the nearest integer value.",
                                  m_name.c_str()),
                 "Information",
                  wxOK | wxCENTRE | wxICON_INFORMATION);
  }

  return areEqual( interval, intervalTmp );
}
*/
/*
//----------------------------------------
bool CFormula::ComputeIntervalAsDate(bool showMsg)
{
  setDefaultValue(m_interval);

  if (!IsXYType())
  {
    return true;
  }

  if (isDefaultValue(m_minValue) && isDefaultValue(m_maxValue))
  {
    return true;
  }

  double interval = 0.0;
  double intervalTmp = 0.0;
  double step = GetStepAsDouble();

  if (isDefaultValue(step))
  {
    return true;
  }

  double minValueDefault =  0.0;
  double maxValueDefault =  CDate::m_secInDay;

  if (isDefaultValue(m_minValue))
  {
    m_minValue = minValueDefault;
  }
  if (isDefaultValue(m_maxValue))
  {
    m_maxValue = maxValueDefault;
  }

  if (step < 0)
  {
    step = -(step);
    m_step.Replace("-", "");
  }

  if (isZero(step))
  {
    SetStepToDefault();

    step = CFormula::GetStepAsDouble(m_step);
  }


  CUnit	unit	= m_unit.BaseUnit();
  unit.SetConversionTo(m_unit);
  double minValue	= unit.Convert(m_minValue);
  double maxValue	= unit.Convert(m_maxValue);

  //intervalTmp = ((m_maxValue - m_minValue) / step) / CDate::m_secInDay;
  intervalTmp = ((maxValue - minValue) / step);
  //interval = CTools::Round( ((m_maxValue - m_minValue) / step) / CDate::m_secInDay);
  interval = CTools::Round( ((maxValue - minValue) / step));

  m_interval = static_cast<int32_t>( CTools::Int(interval) );

  if (m_interval <= 0)
  {
    m_interval = 1;
  }

  if ( (showMsg) && (areEqual( interval, intervalTmp ) == false) )
  {
    wxMessageBox(wxString::Format("Formula '%s':\nInterval was round up or down to the nearest integer value.",
                                  m_name.c_str()),
                 "Information",
                  wxOK | wxCENTRE | wxICON_INFORMATION);
  }

  return areEqual( interval, intervalTmp );
}
*/
//----------------------------------------
void CFormula::SetStepToDefaultAsNecessary()
{
  if (m_step.IsEmpty())
  {
    SetStepToDefault();
  }
}
//----------------------------------------
void CFormula::SetStepToDefault()
{
  m_step = GetDefaultStep();
}
//----------------------------------------
wxString CFormula::GetDefaultStep()
{
  if (!IsDataTypeSet())
  {
    return "";
  }

  if (IsLatLonDataType())
  {
    return DEFAULT_STEP_LATLON_ASSTRING;
  }
  else if (IsTimeDataType())
  {
    return DEFAULT_STEP_TIME_ASSTRING;
  }
  return DEFAULT_STEP_GENERAL_ASSTRING;
}

//----------------------------------------
wxString CFormula::GetAsDateString(double seconds, const wxString& format /*= ""*/)
{
  CDate d;
  d.SetDate(seconds);

  if (d.IsDefaultValue())
  {
    return "";
  }
  return d.AsString((const char *)format.c_str()).c_str();

}



//----------------------------------------
void CFormula::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }


  fOut << "==> Dump a CFormula Object at "<< this << std::endl;
  fOut << "m_name " << m_name;
  fOut << "m_description " << m_description;
  fOut << "==> END Dump a CFormula Object at "<< this << std::endl;

  fOut << std::endl;

}

//-------------------------------------------------------------
//------------------- CMapFormula class --------------------
//-------------------------------------------------------------
wxString CMapFormula::m_predefFormulaFile = "Formulas.ini";

CMapFormula::CMapFormula(bool withPredefined)
{
  Init();
  if (withPredefined)
  {
    InsertPredefined();
  }
}

//----------------------------------------
CMapFormula::CMapFormula(CMapFormula& m)
{
  Init();
  Copy(m);
}

//----------------------------------------
CMapFormula::~CMapFormula()
{
  DeleteConfig();
}

//----------------------------------------
CMapFormula& CMapFormula::operator=(CMapFormula& m)
{
  Copy(m);
  return *this;
}
//----------------------------------------
void CMapFormula::Init()
{
  m_config = NULL;
}
//----------------------------------------
void CMapFormula::Copy(CMapFormula& m)
{
  RemoveAll();

  CMapFormula::iterator it;

  for (it = m.begin() ; it != m.end() ; it++)
  {
    CFormula* value = dynamic_cast<CFormula*>(it->second);
    if (value == NULL)
    {
      continue;
    }

    Insert((const char *)value->GetName().c_str(), new CFormula(*value));
  }

}

/*
//----------------------------------------
CMapFormula& CMapFormula::GetInstance()
{
 static CMapFormula instance;

 return instance;
}
*/
//----------------------------------------
void CMapFormula::DeleteConfig()
{
  if (m_config != NULL)
  {
    delete m_config;
    m_config = NULL;
  }

}
//----------------------------------------
bool CMapFormula::ValidName(const std::string& name)
{
  return ValidName(name.c_str());
}
//----------------------------------------
bool CMapFormula::ValidName(const char* name)
{
  CFormula* value = dynamic_cast<CFormula*>(Exists(name));
  return (value != NULL);
}

//----------------------------------------
bool CMapFormula::LoadConfig(bool predefined)
{
  return LoadConfig(m_config, predefined);
}
//----------------------------------------
bool CMapFormula::LoadConfig(wxFileConfig* config, bool predefined, const wxString& pathSuff)
{
  bool bOk = true;

  if (config == NULL)
  {
    return false;
  }

  wxString path = GROUP_FORMULAS;
  if (pathSuff.IsEmpty() == false)
  {
    path += "_" + pathSuff;
  }
  config->SetPath("/" + path);

  long maxEntries = config->GetNumberOfEntries();
  wxString entry;
  wxString valueString;
  wxString formulaName;
  long i = 0;

  do
  {
    bOk = config->GetNextEntry(entry, i);
    if (bOk)
    {
      valueString = config->Read(entry);

      if (pathSuff.IsEmpty() == false)
      {
        formulaName = valueString.Left(valueString.Length() - pathSuff.Length() - 1);
      }
      else
      {
        formulaName = valueString;
      }
      CFormula* value = dynamic_cast<CFormula*>(Exists((const char *)formulaName.c_str()));
      if (value != NULL)
      {
        Erase((const char *)formulaName.c_str());
      }

      Insert((const char *)formulaName.c_str(), new CFormula(formulaName, predefined));
    }
  }
  while (bOk);

  CMapFormula::iterator it;

  int32_t index = 0;

  for (it = begin() ; it != end() ; it++)
  {
    index++;
    CFormula* formula = dynamic_cast<CFormula*>(it->second);
    if (formula == NULL)
    {

      wxMessageBox("ERROR in  CMapFormula::LoadConfig\ndynamic_cast<CFormula*>(it->second) returns NULL pointer"
                   "\nList seems to contain objects other than those of the class CFormula",
                   "Error",
                    wxOK | wxCENTRE | wxICON_ERROR);
      return false;
    }

    formula->LoadConfig(config, pathSuff);
  }


  return true;

}

//----------------------------------------
bool CMapFormula::SaveConfig(wxFileConfig* config, bool predefined, const wxString& pathSuff)
{
  bool bOk = true;

  if (config == NULL)
  {
    return false;
  }


  int32_t index = 0;
  CMapFormula::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    wxString path = GROUP_FORMULAS;
    if (pathSuff.IsEmpty() == false)
    {
      path += "_" + pathSuff;
    }
    config->SetPath("/" + path);

    CFormula* formula = dynamic_cast<CFormula*>(it->second);

    if (formula->IsPredefined() != predefined)
    {
      continue;
    }

    if (formula != NULL)
    {
      index++;
      wxString formulaName = formula->GetName();
      if (pathSuff.IsEmpty() == false)
      {
        formulaName += "_" + pathSuff;
      }

      config->Write(wxString::Format(ENTRY_FORMULA + "%d", index),
                    formulaName);

      formula->SaveConfig(config, pathSuff);

    }

  }

  return true;

}
//----------------------------------------
void CMapFormula::InsertPredefined()
{
  DeleteConfig();

  wxFileName formulaPath;
  formulaPath.Assign(CTools::GetDataDir().c_str(), CMapFormula::m_predefFormulaFile);
  formulaPath.Normalize();

  m_config = new wxFileConfig(wxEmptyString, wxEmptyString, formulaPath.GetFullPath(), wxEmptyString, wxCONFIG_USE_LOCAL_FILE);

  LoadConfig(true);

}
//----------------------------------------
void CMapFormula::InsertUserDefined(CFormula* formula)
{
  std::string name = (const char *)formula->GetName().c_str();

  CFormula* value = dynamic_cast<CFormula*>(Exists(name));
  if (value != NULL)
  {
    int32_t result = wxMessageBox(wxString::Format("Formula '%s' already exists\nContent:\n%s\nReplace by:\n%s?",
                                                    value->GetName().c_str(),
                                                    value->GetDescription().c_str(),
                                                    formula->GetDescription().c_str()),
                                 "Warning",
                                  wxYES_NO | wxCENTRE | wxICON_QUESTION);

    if (result == wxYES)
    {
      value->SetDescription(formula->GetDescription());
      value->SetComment(formula->GetComment());
    }
  }
  else
  {
    Insert(name, formula);
  }

}
//----------------------------------------
void CMapFormula::InsertUserDefined(CFormula& formula)
{
  std::string name = (const char *)formula.GetName().c_str();

  CFormula* value = dynamic_cast<CFormula*>(Exists(name));
  if (value != NULL)
  {
    if (value->IsPredefined())
    {
      wxMessageBox(wxString::Format("Formula '%s' already exists and is a predefined formula.\n"
                                    "You are not allowed to replace predefined formula.",
                                    value->GetName().c_str()),
                   "Warning",
                    wxOK | wxCENTRE | wxICON_INFORMATION);

    }
    else
    {
      int32_t result = wxMessageBox(wxString::Format("Formula '%s' already exists\nContent:\n%s\nReplace by:\n%s?",
                                                      value->GetName().c_str(),
                                                      value->GetDescription().c_str(),
                                                      formula.GetDescription().c_str()),
                                   "Warning",
                                    wxYES_NO | wxCENTRE | wxICON_QUESTION);

      if (result == wxYES)
      {
        value->SetDescription(formula.GetDescription());
        value->SetComment(formula.GetComment());
      }
    }
  }
  else
  {
    CFormula* formulaToInsert = new CFormula(formula);
    Insert(name, formulaToInsert);
  }

}
//----------------------------------------
void CMapFormula::InsertUserDefined(const wxFileName& fileName)
{
  DeleteConfig();

  m_config = new wxFileConfig(wxEmptyString, wxEmptyString, fileName.GetFullPath(), wxEmptyString, wxCONFIG_USE_LOCAL_FILE);

  LoadConfig(false);
}
//----------------------------------------
void CMapFormula::InsertUserDefined(wxFileConfig* config)
{
  LoadConfig(config, false);
}
//----------------------------------------
wxString CMapFormula::GetDescFormula(const wxString& name, bool alias)
{
  CFormula* value = dynamic_cast<CFormula*>(Exists((const char *)name.c_str()));
  if (value == NULL)
  {
    return "";
  }

  wxString text;
  if (alias)
  {
    text =  value->GetAlias();
  }
  else
  {
    text = value->GetDescription();
  }

  return text;
}
/*
//----------------------------------------
bool CMapFormula::SetDescFormula(const wxString& name, const wxString& description)
{
  CFormula* value = dynamic_cast<CFormula*>(Exists(name.c_str()));
  if (value == NULL)
  {
    return false;
  }

  value->SetDescription(description);
  return true;
}
*/
//----------------------------------------
wxString CMapFormula::GetCommentFormula(const wxString& name)
{
  CFormula* value = dynamic_cast<CFormula*>(Exists((const char *)name.c_str()));
  if (value == NULL)
  {
    return "";
  }

  return value->GetComment();
}
//----------------------------------------
bool CMapFormula::SetCommentFormula(const wxString& name, const wxString& comment)
{
  CFormula* value = dynamic_cast<CFormula*>(Exists((const char *)name.c_str()));
  if (value == NULL)
  {
    return false;
  }

  value->SetComment(comment);
  return true;
}

//----------------------------------------
CFormula* CMapFormula::GetFormula(int32_t type)
{
  CMapFormula::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CFormula* value = dynamic_cast<CFormula*>(it->second);
    if (value == NULL)
    {
      continue;
    }

    if (value->GetType() == type)
    {
      return value;
    }
  }

  return NULL;
}
//----------------------------------------
void CMapFormula::InitFormulaDataMode(int32_t dataMode)
{
  CMapFormula::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CFormula* value = dynamic_cast<CFormula*>(it->second);
    if (value == NULL)
    {
      continue;
    }

    if (value->GetDataMode() < 0)
    {
      value->SetDataMode(dataMode);
    }
  }

}

//----------------------------------------
void CMapFormula::NamesToArrayString(wxArrayString& array)
{
  CMapFormula::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CFormula* value = dynamic_cast<CFormula*>(it->second);
    if (value != NULL)
    {
      array.Add(value->GetName());
    }
  }

}
//----------------------------------------
void CMapFormula::NamesToComboBox(wxComboBox& combo)
{
  CMapFormula::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CFormula* value = dynamic_cast<CFormula*>(it->second);
    if (value != NULL)
    {
      combo.Append(value->GetName());
    }
  }

}
//----------------------------------------
int32_t CMapFormula::CountDataFields()
{
  CMapFormula::iterator it;

  int32_t count = 0;

  for (it = begin() ; it != end() ; it++)
  {
    CFormula* value = dynamic_cast<CFormula*>(it->second);
    if (value != NULL)
    {
      if (value->GetType() == CMapTypeField::typeOpAsField)
      {
        count++;
      }
    }
  }

  return count;

}
//----------------------------------------

bool CMapFormula::HasFilters()
{
  CMapFormula::iterator it;

  bool hasFilter = false;

  for (it = begin() ; it != end() ; it++)
  {
    CFormula* value = dynamic_cast<CFormula*>(it->second);
    if (value != NULL)
    {
      if (value->GetType() == CMapTypeField::typeOpAsField)
      {
        if (value->GetFilter() != CMapTypeFilter::filterNone)
        {
          hasFilter = true;
          break;
        }
      }
    }
  }

  return hasFilter;
}
//----------------------------------------

void CMapFormula::Amend(const CStringArray& keys, CProduct* product, const wxString& record)
{
  if (product == NULL)
  {
    return;
  }

  bool bOk = true;

  CStringArray::const_iterator itKey;

  for (itKey = keys.begin() ; itKey != keys.end() ; itKey++)
  {
    CFormula* value = dynamic_cast<CFormula*>(this->Exists(*itKey));
    if (value == NULL)
    {
      continue;
    }

    std::string out;
    wxString errorMsg;
    wxString str = value->GetDescription(true);

    product->ReplaceNamesCaseSensitive((const char *)str.c_str(), out);

    if (out.empty())
    {
      continue;
    }

    if (str.Cmp(out.c_str()) != 0)
    {
      value->SetDescription(out);
    }

  }
}


//-------------------------------------------------------------
//------------------- CMapTypeFilter class --------------------
//-------------------------------------------------------------

CMapTypeFilter::CMapTypeFilter()
{
  Insert("NONE", filterNone);
  Insert("LOESS_SMOOTH", filterLoessSmooth);
  Insert("LOESS_EXTRAPOLATE", filterLoessExtrapolate);
  Insert("LOESS", filterLoess);
}

//----------------------------------------
CMapTypeFilter::~CMapTypeFilter()
{

}

//----------------------------------------
CMapTypeFilter& CMapTypeFilter::GetInstance()
{
 static CMapTypeFilter instance;

 return instance;
}

//----------------------------------------
bool CMapTypeFilter::ValidName(const std::string& name)
{
  return ValidName(name.c_str());
}
//----------------------------------------
bool CMapTypeFilter::ValidName(const char* name)
{
  uint32_t value = Exists(name);
  return (!isDefaultValue(value));
}

//----------------------------------------

wxString CMapTypeFilter::IdToName(uint32_t id)
{
  CMapTypeFilter::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    uint32_t value = it->second;
    if (value == id)
    {
      return (it->first).c_str();
    }
  }

  return "";
}


//----------------------------------------
uint32_t CMapTypeFilter::NameToId(const wxString& name)
{
  return Exists((const char *)name.c_str());
}

//----------------------------------------
void CMapTypeFilter::NamesToArrayString(wxArrayString& array)
{
  CMapTypeFilter::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    uint32_t value = it->second;
    if (!isDefaultValue(value))
    {
      array.Add( (it->first).c_str());
    }
  }

}
//----------------------------------------
void CMapTypeFilter::NamesToComboBox(wxComboBox& combo)
{
  CMapTypeFilter::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    uint32_t value = it->second;
    if (!isDefaultValue(value))
    {
      //combo.Insert((it->first).c_str(), value);
      combo.Append( (it->first).c_str());
    }
  }

}


//-------------------------------------------------------------
//------------------- CMapTypeData class --------------------
//-------------------------------------------------------------

CMapTypeData::CMapTypeData()
{
  Insert("Data", typeOpData);
  Insert("Longitude", typeOpLongitude);
  Insert("Latitude", typeOpLatitude);
  Insert("X", typeOpX);
  Insert("Y", typeOpY);
  Insert("Z", typeOpZ);
  Insert("Date", typeOpT);
}

//----------------------------------------
CMapTypeData::~CMapTypeData()
{

}

//----------------------------------------
CMapTypeData& CMapTypeData::GetInstance()
{
 static CMapTypeData instance;

 return instance;
}

//----------------------------------------
bool CMapTypeData::ValidName(const std::string& name)
{
  return ValidName(name.c_str());
}
//----------------------------------------
bool CMapTypeData::ValidName(const char* name)
{
  uint32_t value = Exists(name);
  return (!isDefaultValue(value));
}

//----------------------------------------

wxString CMapTypeData::IdToName(uint32_t id)
{
  CMapTypeData::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    uint32_t value = it->second;
    if (value == id)
    {
      return (it->first).c_str();
    }
  }

  return "";
}


//----------------------------------------
uint32_t CMapTypeData::NameToId(const wxString& name)
{
  return Exists((const char *)name.c_str());
}

//----------------------------------------
void CMapTypeData::NamesToArrayString(wxArrayString& array, bool noData)
{
  CMapTypeData::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    uint32_t value = it->second;
    if ( (value == typeOpData) && (noData) )
    {
      continue;
    }
    if (!isDefaultValue(value))
    {
      array.Add( (it->first).c_str());
    }
  }

}
//----------------------------------------
void CMapTypeData::NamesToComboBox(wxComboBox& combo, bool noData)
{
  CMapTypeData::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    uint32_t value = it->second;
    if ( (value == typeOpData) && (noData) )
    {
      continue;
    }
    if (!isDefaultValue(value))
    {
      //combo.Insert((it->first).c_str(), value);
      combo.Append( (it->first).c_str());
    }
  }

}

//-------------------------------------------------------------
//------------------- CMapTypeOp class --------------------
//-------------------------------------------------------------

CMapTypeOp::CMapTypeOp()
{
  Insert("Y=F(X)", typeOpYFX);
  Insert("Z=F(X,Y)", typeOpZFXY);
}

//----------------------------------------
CMapTypeOp::~CMapTypeOp()
{

}

//----------------------------------------
CMapTypeOp& CMapTypeOp::GetInstance()
{
 static CMapTypeOp instance;

 return instance;
}

//----------------------------------------
bool CMapTypeOp::ValidName(const std::string& name)
{
  return ValidName(name.c_str());
}
//----------------------------------------
bool CMapTypeOp::ValidName(const char* name)
{
  uint32_t value = Exists(name);
  return (!isDefaultValue(value));
}
//----------------------------------------

wxString CMapTypeOp::IdToName(uint32_t id)
{
  CMapTypeOp::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    uint32_t value = it->second;
    if (value == id)
    {
      return (it->first).c_str();
    }
  }

  return "";
}


//----------------------------------------
uint32_t CMapTypeOp::NameToId(const wxString& name)
{
  return Exists((const char *)name.c_str());
}

//----------------------------------------
void CMapTypeOp::NamesToArrayString(wxArrayString& array)
{
  CMapTypeOp::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    uint32_t value = it->second;
    if (!isDefaultValue(value))
    {
      array.Add( (it->first).c_str());
    }
  }

}
//----------------------------------------
void CMapTypeOp::NamesToComboBox(wxComboBox& combo)
{
  CMapTypeOp::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    uint32_t value = it->second;
    if (!isDefaultValue(value))
    {
      //combo.Insert((it->first).c_str(), value);
      combo.Append( (it->first).c_str());
    }
  }

}
/*
//-------------------------------------------------------------
//------------------- CMapTypeDisp class --------------------
//-------------------------------------------------------------

CMapTypeDisp::CMapTypeDisp()
{
  Insert("Y=F(X)", typeDispYFX);
  Insert("Z=F(Lat,Lon)", typeDispZFLatLon);
  Insert("Z=F(X,Y)", typeDispZFXY);
}

//----------------------------------------
CMapTypeDisp::~CMapTypeDisp()
{

}

//----------------------------------------
CMapTypeDisp& CMapTypeDisp::GetInstance()
{
 static CMapTypeDisp instance;

 return instance;
}

//----------------------------------------
bool CMapTypeDisp::ValidName(const std::string& name)
{
  return ValidName(name.c_str());
}
//----------------------------------------
bool CMapTypeDisp::ValidName(const char* name)
{
  uint32_t value = Exists(name);
  return (!isDefaultValue(value));
}

//----------------------------------------

wxString CMapTypeDisp::IdToName(uint32_t id)
{
  CMapTypeDisp::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    uint32_t value = it->second;
    if (value == id)
    {
      return (it->first).c_str();
    }
  }

  return "";
}


//----------------------------------------
uint32_t CMapTypeDisp::NameToId(const wxString& name)
{
  return Exists(name.c_str());
}

//----------------------------------------
void CMapTypeDisp::NamesToArrayString(wxArrayString& array)
{
  CMapTypeDisp::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    uint32_t value = it->second;
    if (!isDefaultValue(value))
    {
      array.Add( (it->first).c_str());
    }
  }

}
//----------------------------------------
void CMapTypeDisp::NamesToComboBox(wxComboBox& combo)
{
  CMapTypeDisp::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    uint32_t value = it->second;
    if (!isDefaultValue(value))
    {
      //combo.Insert((it->first).c_str(), value);
      combo.Append( (it->first).c_str());
    }
  }

}
//----------------------------------------

wxString CMapTypeDisp::Enum()
{
  CMapTypeDisp::iterator it;

  wxString result;

  for (it = begin() ; it != end() ; it++)
  {
    result.Append(wxString::Format("%s ==> %d  ", (it->first).c_str(), (it->second)));
  }

  return result;
}

*/
//-------------------------------------------------------------
//------------------- CMapTypeField class --------------------
//-------------------------------------------------------------

CMapTypeField::CMapTypeField()
{
  Insert("asField", typeOpAsField);
  Insert("asX", typeOpAsX);
  Insert("asY", typeOpAsY);
}

//----------------------------------------
CMapTypeField::~CMapTypeField()
{

}

//----------------------------------------
CMapTypeField& CMapTypeField::GetInstance()
{
 static CMapTypeField instance;

 return instance;
}

//----------------------------------------
bool CMapTypeField::ValidName(const std::string& name)
{
  return ValidName(name.c_str());
}
//----------------------------------------
bool CMapTypeField::ValidName(const char* name)
{
  uint32_t value = Exists(name);
  return (!isDefaultValue(value));
}

//----------------------------------------

wxString CMapTypeField::IdToName(uint32_t id)
{
  CMapTypeField::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    uint32_t value = it->second;
    if (value == id)
    {
      return (it->first).c_str();
    }
  }

  return "";
}


//----------------------------------------
uint32_t CMapTypeField::NameToId(const wxString& name)
{
  return Exists((const char *)name.c_str());
}

//----------------------------------------
void CMapTypeField::NamesToArrayString(wxArrayString& array)
{
  CMapTypeField::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    uint32_t value = it->second;
    if (!isDefaultValue(value))
    {
      array.Add( (it->first).c_str());
    }
  }

}
//----------------------------------------
void CMapTypeField::NamesToComboBox(wxComboBox& combo)
{
  CMapTypeField::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    uint32_t value = it->second;
    if (!isDefaultValue(value))
    {
      //combo.Insert((it->first).c_str(), value);
      combo.Append( (it->first).c_str());
    }
  }

}
//-------------------------------------------------------------
//------------------- CMapDataMode class --------------------
//-------------------------------------------------------------

CMapDataMode::CMapDataMode()
{
  for (int32_t mode = CBratProcess::pctFIRST; mode <= CBratProcess::pctLAST; mode++)
  {
    Insert(CBratProcess::DataModeStr(CBratProcess::MergeDataMode(mode)), mode);
  }
}

//----------------------------------------

CMapDataMode::~CMapDataMode()
{

}

//----------------------------------------
CMapDataMode& CMapDataMode::GetInstance()
{
 static CMapDataMode instance;

 return instance;
}

//----------------------------------------
bool CMapDataMode::ValidName(const std::string& name)
{
  return ValidName(name.c_str());
}
//----------------------------------------
bool CMapDataMode::ValidName(const char* name)
{
  uint32_t value = Exists(name);
  return (!isDefaultValue(value));
}

//----------------------------------------

wxString CMapDataMode::IdToName(uint32_t id)
{
  CMapDataMode::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    uint32_t value = it->second;
    if (value == id)
    {
      return (it->first).c_str();
    }
  }

  return "";
}


//----------------------------------------
uint32_t CMapDataMode::NameToId(const wxString& name)
{
  return Exists((const char *)name.c_str());
}

//----------------------------------------
void CMapDataMode::NamesToArrayString(wxArrayString& array)
{
  CMapDataMode::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    uint32_t value = it->second;
    if (!isDefaultValue(value))
    {
      array.Add( (it->first).c_str());
    }
  }

}
//----------------------------------------
void CMapDataMode::NamesToComboBox(wxComboBox& combo)
{
  CMapDataMode::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    uint32_t value = it->second;
    if (!isDefaultValue(value))
    {
      //combo.Insert((it->first).c_str(), value);
      combo.Append( (it->first).c_str());
    }
  }

}

//----------------------------------------
uint32_t CMapDataMode::GetDefault()
{
  return CBratProcess::pctMEAN;
}

//----------------------------------------
wxString CMapDataMode::GetDefaultAsString()
{
  return IdToName(GetDefault());
}

