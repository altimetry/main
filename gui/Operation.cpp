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
    #pragma implementation "Operation.h"
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
#include "BratProcess.h"
using namespace brathl;
using namespace processes;

#include "BratGui.h"
#include "Workspace.h"

#include "Operation.h"


//----------------------------------------

COperation::COperation(wxString name)
      : m_formulas(false)
{
  Init();
  m_name = name;

}
//----------------------------------------
COperation::COperation(COperation& o)
{
  Init();
  Copy(o);

}
//----------------------------------------

COperation::~COperation()
{
  DeleteSelect();
}
//----------------------------------------
COperation& COperation::operator=(COperation& o)
{
  Copy(o);
  return *this;
}
//----------------------------------------
void COperation::Init()
{
  m_product = NULL;
  m_dataset = NULL;

  m_select = new CFormula(ENTRY_SELECT, false);
  m_select->SetType(CMapTypeField::typeOpAsSelect);

  m_type = CMapTypeOp::typeOpYFX;
  m_dataMode = CMapDataMode::GetInstance().GetDefault();
  m_verbose = 2;
  m_exportAsciiDateAsPeriod = false;
  m_exportAsciiExpandArray = false;
  m_exportAsciiNoDataComputation = false;
  m_executeAgain = false;

  setDefaultValue(m_exportAsciiNumberPrecision);
}
//----------------------------------------
void COperation::DeleteSelect()
{

  if (m_select != NULL)
  {
    delete m_select;
    m_select = NULL;
  }
}
//----------------------------------------
bool COperation::IsSelect(CFormula* value)
{
  // if same pointer --> return
  return (m_select == value);
}
//----------------------------------------
bool COperation::IsZFXY()
{
  return (m_type == CMapTypeOp::typeOpZFXY);
}
//----------------------------------------
bool COperation::IsYFX()
{
  return (m_type == CMapTypeOp::typeOpYFX);
}
//----------------------------------------
bool COperation::IsMap()
{
  if (!IsZFXY())
  {
    return false;
  }

  CFormula* xFormula = GetFormula(CMapTypeField::typeOpAsX);
  CFormula* yFormula = GetFormula(CMapTypeField::typeOpAsY);

  if ((xFormula == NULL)  || (yFormula == NULL))
  {
    return false;
  }

  bool isMap = (xFormula->IsLonDataType() && yFormula->IsLatDataType()) ||
               (xFormula->IsLatDataType() && yFormula->IsLonDataType());

  return isMap;
}

//----------------------------------------
bool COperation::IsSelect(const wxString& name)
{
  return (name.CmpNoCase(ENTRY_SELECT) == 0);
}

//----------------------------------------
void COperation::SetSelect(CFormula* value)
{
  // if same pointer --> return
  if (IsSelect(value))
  {
    return;
  }

  DeleteSelect();

  m_select = value;
  m_select->SetName(ENTRY_SELECT);
}

//----------------------------------------
void COperation::Copy(COperation& o)
{
  m_name = o.m_name;

  m_dataset = NULL;
  if (o.m_dataset != NULL)
  {
    m_dataset = FindDataset(o.m_dataset->GetName());
  }

  m_record = o.m_record;
  m_product = o.m_product;

  if (o.m_select != NULL)
  {
    if (m_select != NULL)
    {
      *m_select = *(o.m_select);
    }
    else
    {
      m_select = new CFormula(*(o.m_select));
    }
  }

  m_formulas = o.m_formulas;

  m_type = o.m_type;
  m_dataMode = o.m_dataMode;
  m_exportAsciiDateAsPeriod = o.m_exportAsciiDateAsPeriod;

  InitOutput();
  SetCmdFile();

  InitExportAsciiOutput();
  SetExportAsciiCmdFile();


}

//----------------------------------------
void COperation::ClearLogFile()
{
  m_logFile.Clear();
}
//----------------------------------------
void COperation::SetLogFile(const wxString& value)
{
  m_logFile.Assign(value);
  m_logFile.Normalize();
}
//----------------------------------------
void COperation::SetLogFile()
{
  CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();

  m_logFile = m_output;
  m_logFile.SetExt(LOGFILE_EXTENSION);
  if (wks == NULL)
  {
    m_logFile.Normalize();
  }
  else
  {
    m_logFile.Normalize(wxPATH_NORM_ALL, wks->GetPathName());
  }

}

//----------------------------------------
bool COperation::CtrlLoessCutOff(wxString& msg)
{
  if (m_type != CMapTypeOp::typeOpZFXY)
  {
    return true;
  }

  CMapFormula::iterator it;

  bool hasCutOffX = false;
  bool hasCutOffY = false;
  bool hasFilter = false;
  wxString fields;

  for (it = m_formulas.begin() ; it != m_formulas.end() ; it++)
  {
    CFormula* value = dynamic_cast<CFormula*>(it->second);
    if (value == NULL)
    {
      continue;
    }

    if (value->GetType() == CMapTypeField::typeOpAsSelect)
    {
      continue;
    }

    if (value->GetType() == CMapTypeField::typeOpAsField)
    {
      if ( (isDefaultValue(value->GetFilter()) == false) &&
        (value->GetFilter() != CMapTypeFilter::filterNone) )
      {
        hasFilter = true;
        fields += wxString::Format("\t'%s'", value->GetName().c_str());
      }
    }
    if (value->GetType() == CMapTypeField::typeOpAsX)
    {
      if ( (isDefaultValue(value->GetLoessCutOff()) == false) &&
           (value->GetLoessCutOff() > 1) )
      {
        hasCutOffX = true;
      }
    }
    if (value->GetType() == CMapTypeField::typeOpAsY)
    {
      if ( (isDefaultValue(value->GetLoessCutOff()) == false) &&
           (value->GetLoessCutOff() > 1) )
      {
        hasCutOffY = true;
      }
    }
  }

  if (hasFilter)
  {
    msg += wxString::Format("At least a filter is defined for operation '%s'\non 'Data field(s)' \n%s\n",
                            GetName().c_str(),
                            fields.c_str());
    if (hasCutOffX == false)
    {
      msg += "No 'Loess cut-off' is defined for X field (or 'Loess-cut-off value is <= 1)\n";
    }
    if (hasCutOffY == false)
    {
      msg += "No 'Loess cut-off' is defined for Y field (or 'Loess-cut-off value is <= 1)\n";
    }
  }
  /*
  else
  {
    msg += wxString::Format("No filter is defined on 'Data field(s)' for operation '%s'.\n",
                            GetName().c_str());
    if (hasCutOffX)
    {
      msg += "A 'Loess cut-off' is defined for X field.\n";
    }
    if (hasCutOffY)
    {
      msg += "A 'Loess cut-off' is defined for Y field.\n";
    }
  }
*/
  bool bOk = true;

  if (hasFilter)
  {
   bOk = hasCutOffX && hasCutOffY;
  }
  /*
  else
  {
   bOk = (!hasCutOffX) && (!hasCutOffY);
  }
  */

  return bOk;
}
//----------------------------------------
bool COperation::UseDataset(const wxString& name)
{
  if (m_dataset == NULL)
  {
    return false;
  }
  return (m_dataset->GetName().CmpNoCase(name) == 0);
}

//----------------------------------------
wxString COperation::GetFormulaNewName()
{

  int32_t i = m_formulas.size();
  wxString key;

  do
  {
    key = wxString::Format(WKS_EXPRESSION_NAME +"_%d", i + 1);
    i++;
  }
  while (m_formulas.Exists((const char *)key.c_str()) != NULL);

  return key;
}
//----------------------------------------
wxString COperation::GetFormulaNewName(const wxString& prefix)
{

  if (m_formulas.Exists((const char *)prefix.c_str()) == NULL)
  {
    return prefix;
  }

  int32_t i = 1;
  wxString key;

  do
  {
    key = wxString::Format(prefix +"_%d", i);
    i++;
  }
  while (m_formulas.Exists((const char *)key.c_str()) != NULL);

  return key;
}


//----------------------------------------
bool COperation::RenameFormula(CFormula* formula, const wxString& newName)
{
 bool bOk = m_formulas.RenameKey((const char *)formula->GetName().c_str(), (const char *)newName.c_str());
 if (bOk == false)
 {
   return false;
 }

 formula->SetName(newName);
 return true;
}

//----------------------------------------
wxString COperation::GetDatasetName()
{
  if (m_dataset == NULL)
  {
    return "";
  }

  return m_dataset->GetName();
}
//----------------------------------------
void COperation::GetFormulaNames(wxComboBox& combo)
{
  CMapFormula::iterator it;

  for (it = m_formulas.begin() ; it != m_formulas.end() ; it++)
  {
    combo.Append( (it->first).c_str());
  }
}


//----------------------------------------
CFormula* COperation::NewUserFormula(CField* field, int32_t typeField, bool addToMap, CProduct* product)
{
  if (field == NULL)
  {
    return NULL;

  }

  CFormula* formula =  NewUserFormula(field->GetName().c_str(), typeField, field->GetUnit().c_str(), addToMap, product);

  //formula->SetUnit(field->GetUnit().c_str());
  if (m_record.compare(field->GetRecordName().c_str()) != 0)
  {
    formula->SetDescription(field->GetFullNameWithRecord());
  }
  else
  {
    formula->SetDescription(field->GetFullName());
  }

  return formula;
}
//----------------------------------------
CFormula* COperation::NewUserFormula(const wxString& name, int32_t typeField, const wxString& strUnit, bool addToMap, CProduct* product)
{
  bool bOk = true;

  CFormula* formulaToReturn = NULL;

  wxString formulaName = (name.IsEmpty()) ? GetFormulaNewName() : GetFormulaNewName(name);

  CFormula formula(formulaName, false);
  formula.SetType(typeField);
  formula.SetUnit(strUnit, "", false);

  CUnit* unit = formula.GetUnit();

  formula.SetDataType(typeField, *unit, product);

  if (strUnit.IsEmpty())
  {
    formula.SetDefaultUnit();
  }

  formula.SetDefaultMinMax();

  if (addToMap)
  {
    bOk = AddFormula(formula);
    formulaToReturn = GetFormula(formulaName);
  }
  else
  {
    formulaToReturn = new CFormula(formula);
  }

  return formulaToReturn;
}
//----------------------------------------
bool COperation::AddFormula(CFormula& formula)
{
  CFormula* value = dynamic_cast<CFormula*>(m_formulas.Exists((const char *)formula.GetName().c_str()));
  if (value != NULL)
  {
    return false;
  }

  m_formulas.InsertUserDefined(formula);
  return true;
}

//----------------------------------------
bool COperation::DeleteFormula(const wxString& name)
{

  bool bOk = false;

  CFormula* formula = GetFormula(name);

  if (formula != NULL)
  {
    if (IsSelect(formula))
    {
      formula->SetDescription("");
      bOk = true;
    }
    else
    {
      bOk = m_formulas.Erase((const char *)name.c_str());
    }
  }
  else if (COperation::IsSelect(name))
  {
    m_select->SetDescription("");
    bOk = true;
  }

 return bOk;
}
//----------------------------------------
wxString COperation::GetCommentFormula(const wxString& name)
{
  return m_formulas.GetCommentFormula(name);
}
//----------------------------------------
void COperation::SetCommentFormula(const wxString& name, const wxString& value)
{
  m_formulas.SetCommentFormula(name, value);
}
//----------------------------------------
wxString COperation::GetDescFormula(const wxString& name, bool alias)
{
  return m_formulas.GetDescFormula(name, alias);
}
/*
//----------------------------------------
void COperation::SetDescFormula(const wxString& name, const wxString& value)
{
  m_formulas.SetDescFormula(name, value);
}
*/
//----------------------------------------
CFormula* COperation::GetFormula(const wxString& name)
{
  return dynamic_cast<CFormula*>(m_formulas.Exists((const char *)name.c_str()));
}
//----------------------------------------
CFormula* COperation::GetFormula(int32_t type)
{
  return m_formulas.GetFormula(type);
}
//----------------------------------------
CFormula* COperation::GetFormula(CMapFormula::iterator it)
{
  return dynamic_cast<CFormula*>(it->second);
}

//----------------------------------------
int32_t COperation::GetFormulaCountDataFields()
{
  return m_formulas.CountDataFields();
}
//----------------------------------------
bool COperation::HasFilters()
{
  return m_formulas.HasFilters();
}
/*
//----------------------------------------
wxString COperation::GetFormulaName()
{
  if (m_formula == NULL)
  {
    return "";
  }

  return m_formula->GetName();
}

//----------------------------------------
wxString COperation::GetFormulaDescription()
{
  if (m_formula == NULL)
  {
    return "";
  }

  return m_formula->GetDescription();
}
//----------------------------------------
void COperation::SetFormulaDesc(const wxString& value)
{
  if (m_formula == NULL)
  {
    return;
  }

  m_formula->SetDescription(value);
}
*/
//----------------------------------------
/*
void COperation::SetSelectDesc(const wxString& value)
{
  if (m_select == NULL)
  {
    return;
  }

  m_select->SetDescription(value);

}
*/
//----------------------------------------
wxString COperation::GetSelectName()
{
  if (m_select == NULL)
  {
    return "";
  }

  return m_select->GetName();
}
//----------------------------------------
wxString COperation::GetSelectDescription()
{
  if (m_select == NULL)
  {
    return "";
  }

  return m_select->GetDescription();
}
//----------------------------------------
bool COperation::SaveConfig(wxFileConfig* config)
{
  bool bOk = true;
  if (config == NULL)
  {
    return true;
  }

  config->SetPath("/" + m_name);

  if (m_dataset != NULL)
  {
    bOk &= config->Write(ENTRY_DSNAME,
                         m_dataset->GetName());
    bOk &= config->Write(ENTRY_RECORDNAME,
                         GetRecord());
  }
/*  else
  {
    bOk &= config->Write(ENTRY_DSNAME,
                         "");
  }
*/
  bOk &= config->Write(ENTRY_TYPE,
                       CMapTypeOp::GetInstance().IdToName(m_type));

  // Data mode is no more for a operation, but for each formula (data expression)
  //bOk &= config->Write(ENTRY_DATA_MODE,
  //                     CMapDataMode::GetInstance().IdToName(m_dataMode));

  m_select->SaveConfigDesc(config, config->GetPath());

  // Save relative path based on path operation workspace for output file
  //bOk &= config->Write(ENTRY_OUTPUT, GetOutputName());
  bOk &= config->Write(ENTRY_OUTPUT, GetOutputNameRelativeToWks());

  bOk &= config->Write(ENTRY_EXPORT_ASCII_OUTPUT, GetExportAsciiOutputNameRelativeToWks());

  // Warning after formulas Load config conig path has changed
  m_formulas.SaveConfig(config, false, GetName() );

  return bOk;


}
//----------------------------------------
bool COperation::LoadConfig(wxFileConfig* config)
{
  bool bOk = true;
  if (config == NULL)
  {
    return true;
  }

  config->SetPath("/" + m_name);

  wxString valueString;
  valueString = config->Read(ENTRY_DSNAME);
  m_dataset = FindDataset(valueString);

  valueString = config->Read(ENTRY_TYPE, (const char *)CMapTypeOp::GetInstance().IdToName(m_type).c_str());
  if (valueString.IsEmpty())
  {
    m_type = CMapTypeOp::typeOpYFX;
  }
  else
  {
    m_type = CMapTypeOp::GetInstance().NameToId(valueString);
  }

  valueString = config->Read(ENTRY_DATA_MODE, (const char *)CMapDataMode::GetInstance().IdToName(m_dataMode).c_str());
  if (valueString.IsEmpty())
  {
    m_dataMode = CMapDataMode::GetInstance().GetDefault();
  }
  else
  {
    m_dataMode = CMapDataMode::GetInstance().NameToId(valueString);
  }


  m_record = config->Read(ENTRY_RECORDNAME);

  if (m_record.IsEmpty())
  {
    m_record = CProductNetCdf::m_virtualRecordName.c_str();
  }
  valueString = config->Read(ENTRY_OUTPUT);
  if (valueString.IsEmpty() == false)
  {
    // Note that if the path to the output is in relative form,
    // SetOutput make it in absolute form based on workspace Operation path.
    SetOutput(valueString);
  }

  valueString = config->Read(ENTRY_EXPORT_ASCII_OUTPUT);
  if (valueString.IsEmpty() == false)
  {
    // Note that if the path to the output is in relative form,
    // SetOutput make it in absolute form based on workspace Operation path.
    SetExportAsciiOutput(valueString);
  }
  else
  {
    InitExportAsciiOutput();
  }

  m_select->LoadConfigDesc(config, config->GetPath());

  // Warning after formulas Load config conig path has changed
  m_formulas.LoadConfig(config, false, GetName());

  m_formulas.InitFormulaDataMode(m_dataMode);

  return true;

}

//----------------------------------------
CDataset* COperation::FindDataset(const wxString& datasetName)
{
  CWorkspaceDataset* wks = wxGetApp().GetCurrentWorkspaceDataset();
  if (wks == NULL)
  {
    return NULL;
  }

  return wks->GetDataset(datasetName);
}
//----------------------------------------
wxFileName* COperation::GetSystemCommand()
{
  switch (m_type)
  {
  case CMapTypeOp::typeOpYFX:
    return wxGetApp().GetExecYFXName();
    break;
  case CMapTypeOp::typeOpZFXY:
    return wxGetApp().GetExecZFXYName();
    break;
  default :
    return &m_cmdFile;
    break;
  }
}
//----------------------------------------
wxFileName* COperation::GetShowStatsSystemCommand()
{
  return wxGetApp().GetExecShowStatsName();
}
//----------------------------------------
wxFileName* COperation::GetExportAsciiSystemCommand()
{
  return wxGetApp().GetExecExportAsciiName();
}
//----------------------------------------
wxString COperation::GetFullCmd()
{
  return wxString::Format("\"%s\" \"%s\"",
                           GetSystemCommand()->GetFullPath().c_str(),
                           m_cmdFile.GetFullPath().c_str());
}
//----------------------------------------
wxString COperation::GetExportAsciiFullCmd()
{
  return wxString::Format("\"%s\" \"%s\"",
                           GetExportAsciiSystemCommand()->GetFullPath().c_str(),
                           m_exportAsciiCmdFile.GetFullPath().c_str());
}
//----------------------------------------
wxString COperation::GetShowStatsFullCmd()
{
  return wxString::Format("\"%s\" \"%s\"",
                           GetShowStatsSystemCommand()->GetFullPath().c_str(),
                           m_showStatsCmdFile.GetFullPath().c_str());
}


//----------------------------------------
wxString COperation::GetTaskName()
{
  //wxString value = m_cmdFile.GetName();
  if (m_cmdFile.GetName().IsEmpty())
  {
    return "NoName";
  }
  /*
  if (value.Last() == '.')
  {
    value.RemoveLast();
  }
  return value;
  */
  return m_cmdFile.GetName();
}
//----------------------------------------
wxString COperation::GetExportAsciiTaskName()
{
  //wxString value = m_cmdFile.GetName();
  if (m_exportAsciiCmdFile.GetName().IsEmpty())
  {
    return "NoName";
  }
  /*
  if (value.Last() == '.')
  {
    value.RemoveLast();
  }
  return value;
  */
  return m_exportAsciiCmdFile.GetName();
}

//----------------------------------------
wxString COperation::GetShowStatsTaskName()
{
  //wxString value = m_cmdFile.GetName();
  if (m_showStatsCmdFile.GetName().IsEmpty())
  {
    return "NoName";
  }
  /*
  if (value.Last() == '.')
  {
    value.RemoveLast();
  }
  return value;
  */
  return m_showStatsCmdFile.GetName();
}

//----------------------------------------
bool COperation::BuildCmdFile()
{
  m_cmdFile.Normalize();

  m_file.Create(m_cmdFile.GetFullPath(), true, wxS_DEFAULT | wxS_IXUSR);

  BuildCmdFileHeader();
  BuildCmdFileVerbose();
  BuildCmdFileGeneralProperties();
  BuildCmdFileAlias();
  BuildCmdFileDataset();
  BuildCmdFileSpecificUnit();
  BuildCmdFileFields();
  BuildCmdFileSelect();
  BuildCmdFileOutput();

  m_file.Close();
  return true;


}
//----------------------------------------
bool COperation::BuildExportAsciiCmdFile()
{
  m_exportAsciiCmdFile.Normalize();

  m_file.Create(m_exportAsciiCmdFile.GetFullPath(), true, wxS_DEFAULT | wxS_IXUSR);

  BuildExportAsciiCmdFileHeader();
  BuildCmdFileVerbose();
  BuildExportAsciiCmdFileGeneralProperties();
  BuildCmdFileAlias();

  if (m_exportAsciiNoDataComputation)
  {
    BuildCmdFileDataset();
  }
  else
  {
    BuildCmdFileFromOutputOperation();
  }

  BuildCmdFileSpecificUnit();
  BuildExportAsciiCmdFileFields();

  // If Export ascii is done from the output operation file, 
  // don't include the select expression in the command line,
  // because the select expression has already been applied 
  // and the fields contained in the select expression are not necessarily in the output file. 
  if (m_exportAsciiNoDataComputation)
  {
    BuildCmdFileSelect();
  }

  BuildExportAsciiCmdFileOutput();

  m_file.Close();
  return true;


}
//----------------------------------------
bool COperation::BuildShowStatsCmdFile()
{
  m_showStatsCmdFile.Normalize();

  m_file.Create(m_showStatsCmdFile.GetFullPath(), true, wxS_DEFAULT | wxS_IXUSR);

  BuildShowStatsCmdFileHeader();
  BuildCmdFileVerbose();
  BuildCmdFileAlias();
  BuildCmdFileDataset();
  BuildCmdFileSpecificUnit();
  BuildShowStatsCmdFileFields();
  BuildCmdFileSelect();
  BuildShowStatsCmdFileOutput();

  m_file.Close();
  return true;


}
//----------------------------------------
bool COperation::BuildShowStatsCmdFileHeader()
{
//  WriteComment("!/usr/bin/env " + GetShowStatsSystemCommand()->GetName());
  return true;

}

//----------------------------------------
bool COperation::BuildExportAsciiCmdFileHeader()
{
//  WriteComment("!/usr/bin/env " + GetExportAsciiSystemCommand()->GetName());
  return true;

}

//----------------------------------------
bool COperation::BuildCmdFileHeader()
{
//  WriteComment("!/usr/bin/env " + GetSystemCommand()->GetName());
  WriteComment("Type:" + CMapTypeOp::GetInstance().IdToName(m_type));
  return true;

}
//----------------------------------------
bool COperation::BuildExportAsciiCmdFileGeneralProperties()
{

  WriteEmptyLine();
  WriteComment("----- GENERAL PROPERTIES -----");
  WriteEmptyLine();

  wxString valueString = (m_exportAsciiDateAsPeriod ? "Yes" : "No");

  WriteLine(wxString(kwDATE_AS_PERIOD.c_str()) + "=" + valueString);

  valueString = (m_exportAsciiExpandArray ? "Yes" : "No");

  WriteLine(wxString(kwEXPAND_ARRAY.c_str()) + "=" + valueString);

  return true;

}
//----------------------------------------
bool COperation::BuildCmdFileGeneralProperties()
{

  WriteEmptyLine();
  WriteComment("----- GENERAL PROPERTIES -----");
  WriteEmptyLine();
  // Data mode is no more for a operation, but for each formula (data expression)
  //WriteLine(wxString::Format("DATA_MODE=%s", CMapDataMode::GetInstance().IdToName(m_dataMode).c_str()));

  return true;

}

//----------------------------------------
bool COperation::BuildCmdFileAlias()
{
  CMapFormula::iterator it;

  CWorkspaceFormula* wks = wxGetApp().GetCurrentWorkspaceFormula();
  if (wks == NULL)
  {
    return false;
  }

  CMapFormula* formulas = wks->GetFormulas();

  WriteEmptyLine();
  WriteComment("----- ALIAS -----");

  for (it = formulas->begin() ; it != formulas->end() ; it++)
  {
    CFormula* value = dynamic_cast<CFormula*>(it->second);
    if (value == NULL)
    {
      continue;
    }
    WriteLine(wxString(kwALIAS_NAME.c_str()) + wxString("=") + wxString(value->GetName().c_str()));
    WriteLine(wxString(kwALIAS_VALUE.c_str()) + wxString("=") + wxString(value->GetDescription(true)));
  }

  return true;

}
//----------------------------------------
bool COperation::BuildCmdFileSpecificUnit()
{
  CMapFormula::iterator it;

  CDataset* dataset = GetDataset();
  if (dataset == NULL)
  {
    return false;
  }


  WriteEmptyLine();
  WriteComment("----- SPECIFIC UNIT -----");

  CStringMap* fieldSpecificUnit = dataset->GetFieldSpecificUnits();
  CStringMap::iterator itMap;

  for (itMap = fieldSpecificUnit->begin() ; itMap != fieldSpecificUnit->end() ; itMap++)
  {
    WriteLine( wxString::Format("%s=%s", kwUNIT_ATTR_NAME.c_str(), (itMap->first).c_str()));
    WriteLine( wxString::Format("%s=%s", kwUNIT_ATTR_VALUE.c_str(), (itMap->second).c_str()));

  }

  return true;

}
//----------------------------------------
bool COperation::BuildCmdFileFromOutputOperation()
{

  WriteEmptyLine();
  WriteComment("----- DATASET -----");
  WriteEmptyLine();
  WriteLine(wxString(kwRECORD.c_str()) + "=" + CProductNetCdf::m_virtualRecordName.c_str());
  WriteEmptyLine();
  WriteLine(wxString(kwFILE.c_str()) + "=" + this->GetOutputName());

  return true;

}
//----------------------------------------
bool COperation::BuildCmdFileDataset()
{

  WriteEmptyLine();
  WriteComment("----- DATASET -----");
  WriteEmptyLine();
  WriteLine(wxString(kwRECORD.c_str()) + "=" + m_record);
  WriteEmptyLine();
  wxArrayString array;
  m_dataset->GetFiles(array);
  for (uint32_t i = 0 ; i < array.GetCount() ; i++)
  {
    WriteLine(wxString(kwFILE.c_str()) + "=" + array[i]);
  }
  return true;

}
//----------------------------------------
bool COperation::BuildCmdFileFields()
{
  wxString valueString;

  WriteEmptyLine();
  WriteComment("----- FIELDS -----");
  WriteEmptyLine();

  CMapFormula::iterator it;

  for (it = m_formulas.begin() ; it != m_formulas.end() ; it++)
  {
    CFormula* value = dynamic_cast<CFormula*>(it->second);
    if (value == NULL)
    {
      continue;
    }
    WriteEmptyLine();
    WriteComment(value->GetComment(true));
    WriteLine(value->GetFieldPrefix()  + "=" + value->GetDescription(true));
    WriteLine(value->GetFieldPrefix()  + "_NAME=" + value->GetName());
    WriteLine(value->GetFieldPrefix()  + "_TYPE=" + value->GetDataTypeAsString());
    WriteLine(value->GetFieldPrefix()  + "_UNIT=" + value->GetUnitAsText());

    if (value->IsFieldType())
    {
      WriteLine(value->GetFieldPrefix()  + "_DATA_MODE=" + wxString(value->GetDataModeAsString().c_str()));
    }

    valueString = (value->GetTitle().IsEmpty()) ? value->GetName() : value->GetTitle();
    WriteLine(value->GetFieldPrefix()  + "_TITLE=" + valueString);

    valueString = (value->GetComment().IsEmpty()) ? "DV" : value->GetComment();
    WriteLine(value->GetFieldPrefix()  + "_COMMENT=" + valueString);

    BuildCmdFileFieldsSpecificZFXY(value);

  }

  return true;

}
//----------------------------------------
bool COperation::BuildExportAsciiCmdFileFields()
{
  wxString valueString;

  WriteEmptyLine();
  WriteComment("----- FIELDS -----");
  WriteEmptyLine();

  CMapFormula::iterator it;

  for (it = m_formulas.begin() ; it != m_formulas.end() ; it++)
  {
    CFormula* value = dynamic_cast<CFormula*>(it->second);
    if (value == NULL)
    {
      continue;
    }
    WriteEmptyLine();
    WriteComment(value->GetComment(true));

    // If export operation  is not a dump export,
    // Field name in not the field name of the source data file
    // but the field name of the operation result file (intermediate netcdf file).
    if (m_exportAsciiNoDataComputation)
    {
      // Just a dump --> get the name of the source file
      WriteLine(value->GetExportAsciiFieldPrefix()  + "=" + value->GetDescription(true));
    }
    else
    {
      // Not a dump --> get the name of operation file (intermediate netcdf file).
      WriteLine(value->GetExportAsciiFieldPrefix()  + "=" + value->GetName());
    }

    WriteLine(value->GetExportAsciiFieldPrefix()  + "_NAME=" + value->GetName());
    WriteLine(value->GetExportAsciiFieldPrefix()  + "_UNIT=" + value->GetUnitAsText());

    if (! isDefaultValue(m_exportAsciiNumberPrecision) )
    {
      WriteLine(wxString::Format("%s_FORMAT=%d", value->GetExportAsciiFieldPrefix().c_str(), m_exportAsciiNumberPrecision));
    }
  }

  return true;

}
//----------------------------------------
bool COperation::BuildShowStatsCmdFileFields()
{
  wxString valueString;

  WriteEmptyLine();
  WriteComment("----- FIELDS -----");
  WriteEmptyLine();

  CMapFormula::iterator it;

  for (it = m_formulas.begin() ; it != m_formulas.end() ; it++)
  {
    CFormula* value = dynamic_cast<CFormula*>(it->second);
    if (value == NULL)
    {
      continue;
    }
    if (value->GetType() != CMapTypeField::typeOpAsField)
    {
      continue;
    }
    WriteEmptyLine();
    WriteComment(value->GetComment(true));
    WriteLine(value->GetExportAsciiFieldPrefix()  + "=" + value->GetDescription(true));
    //WriteLine(value->GetExportAsciiFieldPrefix()  + "_NAME=" + value->GetName());
    WriteLine(value->GetExportAsciiFieldPrefix()  + "_UNIT=" + value->GetUnitAsText());
  }

  return true;

}

//----------------------------------------
bool COperation::BuildCmdFileFieldsSpecificZFXY(CFormula* value)
{

  // if operation is not X/Y as Lat/Lon or Lon/Lat, force to default value
  /*
  if (!IsMap())
  {
    value->SetFilterDefault();
    value->SetMinValueDefault();
    value->SetMaxValueDefault();
    value->SetIntervalDefault();
    value->SetLoessCutOffDefault();
  }
*/
  wxString valueString;

  if (m_type != CMapTypeOp::typeOpZFXY)
  {
    return false;
  }

  if (value == NULL)
  {
    return false;
  }

  valueString = (value->GetFilter() == CMapTypeFilter::filterNone) ? "DV" : value->GetFilterAsString();
  WriteLine(value->GetFieldPrefix()  + "_FILTER=" + valueString);

  if (value->IsXYType())
  {
    value->ComputeInterval(false);


    if (value->IsTimeDataType())
    {
      valueString = (isDefaultValue(value->GetMinValue())) ? "DV" : value->GetMinValueAsDateString();
      WriteLine(value->GetFieldPrefix()  + "_MIN=" + valueString);

      valueString = (isDefaultValue(value->GetMaxValue())) ? "DV" : value->GetMaxValueAsDateString();
      WriteLine(value->GetFieldPrefix()  + "_MAX=" + valueString);
    }
    else
    {
      valueString = (isDefaultValue(value->GetMinValue())) ? "DV" : value->GetMinValueAsText();
      WriteLine(value->GetFieldPrefix()  + "_MIN=" + valueString);

      valueString = (isDefaultValue(value->GetMaxValue())) ? "DV" : value->GetMaxValueAsText();
      WriteLine(value->GetFieldPrefix()  + "_MAX=" + valueString);
    }

    valueString = (isDefaultValue(value->GetInterval())) ? "DV" : value->GetIntervalAsText();
    WriteLine(value->GetFieldPrefix()  + "_INTERVALS=" + valueString);

    valueString = (isDefaultValue(value->GetLoessCutOff())) ? "DV" : value->GetLoessCutOffAsText();
    WriteLine(value->GetFieldPrefix()  + "_LOESS_CUTOFF=" + valueString);
  }

  return true;

}
//----------------------------------------
bool COperation::BuildCmdFileSelect()
{

  WriteEmptyLine();
  WriteComment("----- SELECT -----");
  WriteEmptyLine();
  if (m_select->GetDescription(true).IsEmpty() == false)
  {
    WriteLine(wxString(kwSELECT.c_str()) + "=" + m_select->GetDescription(true));
  }

  return true;

}
//----------------------------------------
bool COperation::BuildCmdFileOutput()
{

  WriteEmptyLine();
  WriteComment("----- OUTPUT -----");
  WriteEmptyLine();
  if (m_output.GetFullPath().IsEmpty())
  {
    InitOutput();
  }
  WriteLine(wxString(kwOUTPUT.c_str()) + "=" + m_output.GetFullPath());

  return true;

}
//----------------------------------------
bool COperation::BuildExportAsciiCmdFileOutput()
{

  WriteEmptyLine();
  WriteComment("----- OUTPUT -----");
  WriteEmptyLine();
  if (m_exportAsciiOutput.GetFullPath().IsEmpty())
  {
    InitExportAsciiOutput();
  }
  WriteLine(wxString(kwOUTPUT.c_str()) + "=" + m_exportAsciiOutput.GetFullPath());

  return true;

}
//----------------------------------------
bool COperation::BuildShowStatsCmdFileOutput()
{

  WriteEmptyLine();
  WriteComment("----- OUTPUT -----");
  WriteEmptyLine();
  if (m_showStatsOutput.GetFullPath().IsEmpty())
  {
    InitShowStatsOutput();
  }
  WriteLine(wxString(kwOUTPUT.c_str()) + "=" + m_showStatsOutput.GetFullPath());

  return true;

}


//----------------------------------------
bool COperation::BuildCmdFileVerbose()
{

  WriteEmptyLine();
  WriteComment("----- LOG -----");
  WriteEmptyLine();
  WriteLine(wxString::Format("%s=%d", kwVERBOSE.c_str(), m_verbose));

  if (m_logFile.IsOk())
  {
    WriteLine(wxString::Format("%s=%s", kwLOGFILE.c_str(), m_logFile.GetFullPath().c_str()));
  }

  return true;

}

//----------------------------------------
bool COperation::WriteComment(const wxString& text)
{
  WriteLine("#" + text);
  return true;
}
//----------------------------------------
bool COperation::WriteLine(const wxString& text)
{
  if (m_file.IsOpened() == false)
  {
    return false;
  }
  m_file.Write(text + "\n");
  return true;
}
//----------------------------------------
bool COperation::WriteEmptyLine()
{
  if (m_file.IsOpened() == false)
  {
    return false;
  }
  m_file.Write("\n");
  return true;
}

//----------------------------------------
void COperation::SetShowStatsOutput(const wxFileName& value)
{
  CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();

  m_showStatsOutput = value;

  if (wks == NULL)
  {
    m_showStatsOutput.Normalize();
  }
  else
  {
    m_showStatsOutput.Normalize(wxPATH_NORM_ALL, wks->GetPathName());
  }

  SetShowStatsCmdFile();
}
//----------------------------------------
void COperation::SetShowStatsOutput(const wxString& value)
{
  CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();

  m_showStatsOutput.Assign(value);

  if (wks == NULL)
  {
    m_showStatsOutput.Normalize();
  }
  else
  {
    m_showStatsOutput.Normalize(wxPATH_NORM_ALL, wks->GetPathName());
  }

  SetShowStatsCmdFile();
}
//----------------------------------------
void COperation::SetExportAsciiOutput(const wxFileName& value)
{
  CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();

  m_exportAsciiOutput = value;

  if (wks == NULL)
  {
    m_exportAsciiOutput.Normalize();
  }
  else
  {
    m_exportAsciiOutput.Normalize(wxPATH_NORM_ALL, wks->GetPathName());
  }

  SetExportAsciiCmdFile();
}
//----------------------------------------
void COperation::SetExportAsciiOutput(const wxString& value)
{
  CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();

  m_exportAsciiOutput.Assign(value);

  if (wks == NULL)
  {
    m_exportAsciiOutput.Normalize();
  }
  else
  {
    m_exportAsciiOutput.Normalize(wxPATH_NORM_ALL, wks->GetPathName());
  }

  SetExportAsciiCmdFile();
}

//----------------------------------------
void COperation::SetOutput(const wxFileName& value)
{
  CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();

  m_output = value;

  if (wks == NULL)
  {
    m_output.Normalize();
  }
  else
  {
    m_output.Normalize(wxPATH_NORM_ALL, wks->GetPathName());
  }

  SetCmdFile();
}
//----------------------------------------
void COperation::SetOutput(const wxString& value)
{
  CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();

  m_output.Assign(value);

  if (wks == NULL)
  {
    m_output.Normalize();
  }
  else
  {
    m_output.Normalize(wxPATH_NORM_ALL, wks->GetPathName());
  }

  SetCmdFile();
}

//----------------------------------------
void COperation::SetCmdFile()
{
  CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();

  m_cmdFile = m_output;
  m_cmdFile.SetExt(COMMANDFILE_EXTENSION);
  if (wks == NULL)
  {
    m_cmdFile.Normalize();
  }
  else
  {
    m_cmdFile.Normalize(wxPATH_NORM_ALL, wks->GetPathName());
  }

}
//----------------------------------------
void COperation::SetExportAsciiCmdFile()
{
  CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();

  m_exportAsciiCmdFile.AssignDir(wks->GetPathName());
  m_exportAsciiCmdFile.SetFullName(m_exportAsciiOutput.GetFullName());
  m_exportAsciiCmdFile.SetExt(EXPORTASCII_COMMANDFILE_EXTENSION);
  if (wks == NULL)
  {
    m_exportAsciiCmdFile.Normalize();
  }
  else
  {
    m_exportAsciiCmdFile.Normalize(wxPATH_NORM_ALL, wks->GetPathName());
  }

}
//----------------------------------------
void COperation::SetShowStatsCmdFile()
{
  CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();

  m_showStatsCmdFile.AssignDir(wks->GetPathName());
  m_showStatsCmdFile.SetFullName(m_showStatsOutput.GetFullName());
  m_showStatsCmdFile.SetExt(SHOWSTAT_COMMANDFILE_EXTENSION);
  if (wks == NULL)
  {
    m_showStatsCmdFile.Normalize();
  }
  else
  {
    m_showStatsCmdFile.Normalize(wxPATH_NORM_ALL, wks->GetPathName());
  }

}

//----------------------------------------
wxString COperation::GetOutputNameRelativeToWks()
{
  CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();
  if (wks == NULL)
  {
    return GetOutputName();
  }

  wxFileName relative = m_output;
  relative.MakeRelativeTo(wks->GetPathName());
  return relative.GetFullPath();

}
//----------------------------------------
wxString COperation::GetExportAsciiOutputNameRelativeToWks()
{
  CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();
  if (wks == NULL)
  {
    return GetExportAsciiOutputName();
  }

  wxFileName relative = m_exportAsciiOutput;
  relative.MakeRelativeTo(wks->GetPathName());
  return relative.GetFullPath();

}
//----------------------------------------
wxString COperation::GetShowStatsOutputNameRelativeToWks()
{
  CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();
  if (wks == NULL)
  {
    return GetShowStatsOutputName();
  }

  wxFileName relative = m_showStatsOutput;
  relative.MakeRelativeTo(wks->GetPathName());
  return relative.GetFullPath();

}
//----------------------------------------
void COperation::InitOutput()
{
  CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();
  if (wks == NULL)
  {
    return;
  }

  wxString output = wks->GetPathName(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR)
                                     + "Create"
                                     + GetName() + ".nc";
  SetOutput(output);

}
//----------------------------------------
void COperation::InitShowStatsOutput()
{
  CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();
  if (wks == NULL)
  {
    return;
  }

  wxString output = wks->GetPathName(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR)
                                     + "Stats"
                                     + GetName() + ".txt";
  SetShowStatsOutput(output);

}
//----------------------------------------
void COperation::InitExportAsciiOutput()
{
  CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();
  if (wks == NULL)
  {
    return;
  }

  wxString output = wks->GetPathName(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR)
                                     + "ExportAscii"
                                     + GetName() + ".txt";
  SetExportAsciiOutput(output);

}
//----------------------------------------
bool COperation::RemoveOutput()
{
  bool bOk = CBratGuiApp::RemoveFile(GetOutputName());

  CBratGuiApp::RemoveFile(GetCmdFileFullName());

  return bOk;
}
//----------------------------------------
bool COperation::RenameOutput(const wxString& oldName)
{
  bool bOk = CBratGuiApp::RenameFile(oldName, GetOutputName());

  wxFileName oldCmdFile;
  oldCmdFile.Assign(oldName);
  oldCmdFile.SetExt(COMMANDFILE_EXTENSION);

  //RenameFile(oldCmdFile.GetFullPath(), GetCmdFile());
  CBratGuiApp::RemoveFile(oldCmdFile.GetFullPath());
  return bOk;
}

//----------------------------------------
bool COperation::ComputeInterval(const wxString& formulaName, bool showMsg)
{
  CFormula* f = dynamic_cast<CFormula*>(m_formulas.Exists((const char *)formulaName.c_str()));

  return ComputeInterval(f, showMsg);

}
//----------------------------------------
bool COperation::ComputeInterval(CFormula* f, bool showMsg)
{
  if (f == NULL)
  {
    return false;
  }
  if (!IsZFXY())
  {
    return true;
  }

  if (!f->IsXYType())
  {
    return true;
  }

  return f->ComputeInterval(showMsg);
}

//----------------------------------------
bool COperation::GetXExpression(CExpression& expr, wxString& errorMsg, const CStringMap* aliases /* = NULL*/)
{
  bool bOk = true;

  CFormula* formula = GetFormula(CMapTypeField::typeOpAsX);

  if (formula != NULL)
  {
    const CStringMap* fieldAliases = NULL;
    if (m_product != NULL)
    {
      fieldAliases = m_product->GetAliasesAsString();
    }

    bOk &= CFormula::SetExpression(formula->GetDescription(true, aliases, fieldAliases), expr, errorMsg);
  }

  return bOk;

}

//----------------------------------------
bool COperation::GetYExpression(CExpression& expr, wxString& errorMsg, const CStringMap* aliases /* = NULL*/)
{
  bool bOk = true;

  CFormula* formula = GetFormula(CMapTypeField::typeOpAsY);

  if (formula != NULL)
  {
    const CStringMap* fieldAliases = NULL;
    if (m_product != NULL)
    {
      fieldAliases = m_product->GetAliasesAsString();
    }

    bOk &= CFormula::SetExpression(formula->GetDescription(true, aliases, fieldAliases), expr, errorMsg);
  }

  return bOk;

}

//----------------------------------------
bool COperation::ControlXYDataFields(wxString& errorMsg, const CStringMap* aliases /* = NULL*/)
{
  bool bOk = true;

  CExpression xExpr;
  CExpression yExpr;
  CExpression dataExpr;

  /*
  CFormula* xFormula = GetFormula(CMapTypeField::typeOpAsX);

  if (xFormula != NULL)
  {
    bOk &= CFormula::SetExpression(xFormula->GetDescription(true, aliases), xExpr, errorMsg);
  }

  CFormula* yFormula = GetFormula(CMapTypeField::typeOpAsY);

  if (yFormula != NULL)
  {
    bOk &= CFormula::SetExpression(yFormula->GetDescription(true, aliases), yExpr, errorMsg);
  }
  */
  const CStringMap* fieldAliases = NULL;
  if (m_product != NULL)
  {
    fieldAliases = m_product->GetAliasesAsString();
  }

  bOk &= GetXExpression(xExpr, errorMsg, aliases);
  bOk &= GetYExpression(yExpr, errorMsg, aliases);

  CMapFormula::iterator it;

  for (it = m_formulas.begin() ; it != m_formulas.end() ; it++)
  {
    CFormula* value = dynamic_cast<CFormula*>(it->second);
    if (value == NULL)
    {
      continue;
    }

    switch (value->GetType())
    {

    //-----------------------------
    case CMapTypeField::typeOpAsX:
    case CMapTypeField::typeOpAsY:
    //-----------------------------
      continue;
      break;
    //-----------------------------
    case CMapTypeField::typeOpAsField:
    //-----------------------------
      bOk &= CFormula::SetExpression(value->GetDescription(true, aliases, fieldAliases), dataExpr, errorMsg);

      CStringArray intersect;

      bool bIntersect = xExpr.GetFieldNames()->Intersect(*(dataExpr.GetFieldNames()), intersect, true);

      if (bIntersect)
      {
        errorMsg.append("\nCommon fields between X expression and data expressions are not allowed : common field(s) is (are) :\n");
        errorMsg.append(intersect.ToString("\n", false).c_str());
        bOk = false;
      }

      intersect.RemoveAll();

      bIntersect = yExpr.GetFieldNames()->Intersect(*(dataExpr.GetFieldNames()), intersect, true);

      if (bIntersect)
      {
        errorMsg.append("\nCommon fields between Y expression and data expressions are not allowed : common field(s) is (are) :\n");
        errorMsg.append(intersect.ToString("\n", false).c_str());
        bOk = false;
      }

      break;
    }

  }

  return bOk;
}
//----------------------------------------
bool COperation::ControlResolution(wxString& errorMsg)
{
  bool bOk = true;

  if (this->GetType() != CMapTypeOp::typeOpZFXY)
  {
    return bOk;
  }


  CFormula* xFormula = GetFormula(CMapTypeField::typeOpAsX);

  if (xFormula != NULL)
  {
    bOk &= xFormula->ControlResolution(errorMsg);
  }

  CFormula* yFormula = GetFormula(CMapTypeField::typeOpAsY);

  if (yFormula != NULL)
  {
    bOk &= yFormula->ControlResolution(errorMsg);
  }

  return bOk;
}
//----------------------------------------
bool COperation::ControlDimensions(CFormula* formula, wxString& errorMsg,  const CStringMap* aliases /* = NULL*/)
{
  if (formula == NULL)
  {
    return true;
  }

  /*
  if ((!formula->IsXType()) && (!formula->IsYType()))
  {
    return true;
  }
*/
  if (m_product == NULL)
  {
    return true;
  }

  std::string msg;

  wxString stringExpr = formula->GetDescription(true, aliases, m_product->GetAliasesAsString());

  if (stringExpr.IsEmpty())
  {
    return true;
  }


  CUIntArray commonDimensions;
  bool bOk = m_product->HasCompatibleDims( (const char *)stringExpr.c_str(), (const char *)m_record.c_str(), msg, true, &commonDimensions);

  errorMsg.Append(msg.c_str());

  return bOk;

  if (formula->GetType() != CMapTypeField::typeOpAsSelect)
  {
    return bOk;
  }

  if (commonDimensions.size() <= 0)
  {
    return bOk;
  }

  CStringArray axesFields;

  CStringArray fields;

  CFormula* xFormula = GetFormula(CMapTypeField::typeOpAsX);

  if (xFormula != NULL)
  {
    fields.RemoveAll();

    bOk &= xFormula->GetFields(fields, errorMsg, aliases, m_product->GetAliasesAsString());

    axesFields.Insert(fields);
  }

  CFormula* yFormula = GetFormula(CMapTypeField::typeOpAsY);

  if (yFormula != NULL)
  {
    fields.RemoveAll();

    bOk &= yFormula->GetFields(fields, errorMsg, aliases, m_product->GetAliasesAsString());

    axesFields.Insert(fields);
  }


  fields.RemoveAll();

  bOk &= formula->GetFields(fields, errorMsg, aliases, m_product->GetAliasesAsString());

  CStringArray complement;

  axesFields.Complement(fields, complement);

  if (complement.size() <= 0)
  {
    return bOk;
  }

  errorMsg.Append(wxString::Format("\nFields as array are not allowed in selection criteria expression unless they are present in X and/or Y axes.\nWrong field(s) is (are):\n%s\n",
                                   complement.ToString("\n", false).c_str() ) );
  bOk = false;

  return bOk;

}

//----------------------------------------
bool COperation::Control(wxString& msg, bool basicControl /* = false */, const CStringMap* aliases /* = NULL*/)
{
  CMapFormula::iterator it;
  int32_t xCount = 0;
  int32_t yCount = 0;
  int32_t fieldCount = 0;
  int32_t errorCount = 0;
  bool bOk = true;

  for (it = m_formulas.begin() ; it != m_formulas.end() ; it++)
  {
    CFormula* value = dynamic_cast<CFormula*>(it->second);
    if (value == NULL)
    {
      continue;
    }

    switch (value->GetType())
    {
    case CMapTypeField::typeOpAsX:
      xCount++;
      break;
    case CMapTypeField::typeOpAsY:
      yCount++;
      break;
    case CMapTypeField::typeOpAsField:
      fieldCount++;
      break;
    }
    bOk = value->CheckExpression(msg, m_record, aliases, m_product);
    if (!bOk)
    {
      errorCount++;
    }
    else
    {
      bOk = ControlDimensions(value, msg, aliases);
      if (!bOk)
      {
        errorCount++;
      }
    }

    if (!basicControl)
    {
      bOk = value->ControlUnitConsistency(msg);
      if (!bOk)
      {
        errorCount++;
      }
    }
  }


  bOk = m_select->CheckExpression(msg, m_record, aliases, m_product);
  if (!bOk)
  {
    errorCount++;
  }
  else
  {
    bOk = ControlDimensions(m_select, msg, aliases);
    if (!bOk)
    {
      errorCount++;
    }
  }


  if (!basicControl)
  {
    if (xCount == 0)
    {
      msg.Append(wxString::Format("\nThere is no 'X field' for operation '%s'.",
                                    this->GetName().c_str()));
      errorCount++;
    }

    if ( (yCount == 0) && (this->GetType() == CMapTypeOp::typeOpZFXY) )
    {
      msg.Append(wxString::Format("\nThere is no 'Y field' for operation '%s'.",
                                    this->GetName().c_str()));
      errorCount++;
    }
    if (fieldCount == 0)
    {
      msg.Append(wxString::Format("\nThere is no 'Data field' for operation '%s'.",
                                    this->GetName().c_str()));
      errorCount++;
    }

    if (CtrlLoessCutOff(msg) == false)
    {
      errorCount++;
    }


    if (ControlResolution(msg) == false)
    {
      errorCount++;
    }

    //if (ControlXYDataFields(msg, aliases) == false)
    //{
    //  errorCount++;
    //}
  }

  return (errorCount == 0);
}


//----------------------------------------
void COperation::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }


  fOut << "==> Dump a COperation Object at "<< this << std::endl;

  m_dataset->Dump(fOut);
  fOut << "==> END Dump a COperation Object at "<< this << std::endl;

  fOut << std::endl;

}

