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
    #pragma implementation "Workspace.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "brathl.h"

#include "new-gui/Common/tools/Trace.h"
#include "new-gui/Common/tools/Exception.h"


#include "BratGui.h"
#include "DirTraverser.h"
#include "Workspace.h"

const wxString CWorkspace::m_configName = "setup.ini";

//-------------------------------------------------------------
//------------------- CWorkspace class --------------------
//-------------------------------------------------------------
//----------------------------------------
CWorkspace::CWorkspace()
{
  Init();
}
//----------------------------------------
CWorkspace::CWorkspace(const wxString& name, const wxFileName& path, bool createPath)
{
  Init();

  SetName(name);
  SetPath(path, createPath);
}

//----------------------------------------
CWorkspace::CWorkspace(const wxString& name, const wxString& path, bool createPath)
{
  Init();

  SetName(name);
  SetPath(path, createPath);
}

//----------------------------------------
CWorkspace::~CWorkspace()
{
  DeleteConfigObj();

}
//----------------------------------------
void CWorkspace::Init()
{
  m_importBitSet = NULL;
  m_config = NULL;
  m_level = -1;
  m_ctrlDatasetFiles = true;
}
//----------------------------------------
wxString CWorkspace::GetRootKey()
{
  wxString result;

  CStringArray array;
  std::string key = (const char *)m_key.c_str();
  array.ExtractStrings(key, (const char *)CTreeWorkspace::m_keyDelimiter.c_str());

  if (array.size() >= 1)
  {
    result = array[0].c_str();
  }

  return result;
}

//----------------------------------------
void CWorkspace::DeleteConfigObj()
{
  if (m_config != NULL)
  {
    delete m_config;
    m_config = NULL;
  }
}
//----------------------------------------
bool CWorkspace::DeleteConfigFile()
{
  /*
  bool bOk = wxRemoveFile(m_configFileName.GetFullPath());
  if ( bOk == false)
  {
    wxMessageBox(wxString::Format("Unable to remove file '%s",
                                  m_configFileName.GetFullPath().c_str()),
                 "Warning",
                  wxOK | wxCENTRE | wxICON_EXCLAMATION);

  }

  return bOk;
  */
  bool bOk = m_config->DeleteAll();

  InitConfig();

  return bOk;

}
//----------------------------------------
void CWorkspace::InitConfig()
{
  m_configFileName.Assign(m_path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) +  CWorkspace::m_configName);
  DeleteConfigObj();
  m_config = new wxFileConfig(wxEmptyString, wxEmptyString, m_configFileName.GetFullPath(), wxEmptyString, wxCONFIG_USE_LOCAL_FILE);

}
//----------------------------------------
bool CWorkspace::Import(CWorkspace* wks)
{
  return true;
}
//----------------------------------------
bool CWorkspace::SaveConfig(bool flush)
{
  bool bOk = true;

  if (m_config == NULL)
  {
    return true;
  }

  bOk &= DeleteConfigFile();
  bOk &= SaveCommonConfig();

  if (flush)
  {
    m_config->Flush();
  }

  return bOk;
}
//----------------------------------------
bool CWorkspace::SaveCommonConfig(bool flush)
{
  bool bOk = true;

  if (m_config == NULL)
  {
    return true;
  }

  m_config->SetPath("/" + GROUP_GENERAL);
  bOk &= m_config->Write(ENTRY_WKSNAME, m_name);
  m_config->SetPath("/" + GROUP_GENERAL);
  bOk &= m_config->Write(ENTRY_WKSLEVEL, m_level);

  if (flush)
  {
    m_config->Flush();
  }

  return bOk;
}
//----------------------------------------
bool CWorkspace::LoadConfig()
{

  bool bOk = false;

  if (m_config == NULL)
  {
    return bOk;
  }

  bOk = LoadCommonConfig();

  return bOk;
}
//----------------------------------------
bool CWorkspace::LoadCommonConfig()
{

  bool bOk = false;

  if (m_config == NULL)
  {
    return bOk;
  }

  m_config->SetPath("/" + GROUP_GENERAL);
  m_name.Empty();

  bOk = true;

  bOk &= m_config->Read(ENTRY_WKSNAME, &m_name);
  bOk &= m_config->Read(ENTRY_WKSLEVEL, &m_level);

  return bOk;
}


//----------------------------------------
bool CWorkspace::SetPath(const wxFileName& path, bool createPath)
{
  bool bOk = true;

  m_path.AssignDir(path.GetPath());
  m_path.Normalize();

  if (m_path.IsDir() == false)
  {
    return false;
  }

  if ((m_path.DirExists() == false) && (createPath))
  {
    bOk = m_path.Mkdir(0777, wxPATH_MKDIR_FULL);
  }

  InitConfig();

  return bOk;
}

//----------------------------------------
bool CWorkspace::SetPath(const wxString& path, bool createPath)
{
  wxFileName pathTmp;
  if (path.IsEmpty())
  {
    return false;
  }
  pathTmp.AssignDir(path);
  return SetPath(pathTmp, createPath);
}
//----------------------------------------
bool CWorkspace::Rmdir()
{
  RmdirRecurse();

  return m_path.Rmdir();
}
//----------------------------------------
bool CWorkspace::RmdirRecurse()
{
  CDirTraverserDeleteFile traverserDelFile;
  CDirTraverserDeleteDir traverserDelDir;

  //path name given to wxDir is locked until wxDir object is deleted
  wxDir dir;

  dir.Open(GetPathName());
  dir.Traverse(traverserDelFile);

  dir.Open(GetPathName());
  dir.Traverse(traverserDelDir);
  return (dir.HasFiles() == false) && (dir.HasSubDirs() == false);
}
//----------------------------------------
void CWorkspace::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }


  fOut << "==> Dump a CWorkspace Object at "<< this << std::endl;
  fOut << "m_name = " << m_name << std::endl;
  fOut << "m_path object at = " << &m_path << std::endl;
  fOut << "m_path = " << m_path.GetPath() << std::endl;
  fOut << "m_configFileName object at = " << &m_configFileName << std::endl;
  fOut << "m_configFileName = " << m_configFileName.GetFullPath() << std::endl;
  fOut << "m_config object at = " << m_config << std::endl;

  fOut << "==> END Dump a CWorkspace Object at "<< this << std::endl;

  fOut << std::endl;

}


//-------------------------------------------------------------
//------------------- CWorkspaceDataset class --------------------
//-------------------------------------------------------------
//----------------------------------------
CWorkspaceDataset::CWorkspaceDataset()
{
  Init();
}
//----------------------------------------
CWorkspaceDataset::CWorkspaceDataset(const wxString& name, const wxFileName& path, bool createPath)
      : CWorkspace(name, path, createPath)
{
  Init();
}

//----------------------------------------
CWorkspaceDataset::CWorkspaceDataset(const wxString& name, const wxString& path, bool createPath)
      : CWorkspace(name, path, createPath)
{
  Init();
}

//----------------------------------------
CWorkspaceDataset::~CWorkspaceDataset()
{
  Close();
  m_datasets.RemoveAll();
}
//----------------------------------------
void CWorkspaceDataset::Init()
{
  /*
  wxString value = GetDatasetNewName();
  m_datasets.Insert(value.c_str(), new CDataset(value.c_str()));
  value = GetDatasetNewName();
  m_datasets.Insert(value.c_str(), new CDataset(value.c_str()));
  value = GetDatasetNewName();
  m_datasets.Insert(value.c_str(), new CDataset(value.c_str()));
  */
}
//----------------------------------------
bool CWorkspaceDataset::Import(CWorkspace* wks)
{
  if (wks == NULL)
  {
    return true;
  }

  CWorkspaceDataset* wksToImport = dynamic_cast<CWorkspaceDataset*>(wks);
  if (wksToImport == NULL)
  {
    return true;
  }

  if (m_importBitSet == NULL)
  {
    return true;
  }

  if (m_importBitSet->m_bitSet.test(IMPORT_DATASET_INDEX) == false)
  {
    return true;
  }


  CObMap::iterator it;
  for (it = wksToImport->GetDatasets()->begin() ; it != wksToImport->GetDatasets()->end()  ; it++)
  {
    CDataset* datasetImport = dynamic_cast<CDataset*>(it->second);
    if (datasetImport == NULL)
    {

      wxMessageBox("ERROR in  CWorkspaceDataset::Import\ndynamic_cast<CDataset*>(it->second) returns NULL pointer"
                   "\nList seems to contain objects other than those of the class CDataset",
                   "Error",
                    wxOK | wxCENTRE | wxICON_ERROR);
      return false;
    }

    CDataset* dataset = GetDataset(datasetImport->GetName());
    if (dataset != NULL)
    {
      wxMessageBox(wxString::Format("Dataset to import '%s':\nUnable to process, a dataset with the same name already exists\n"
                                    "Import cancelled",
                                    datasetImport->GetName().c_str()),
                  "Warning",
                  wxOK | wxICON_EXCLAMATION);
      return false;
    }

    m_datasets.Insert((const char *)datasetImport->GetName().c_str(), new CDataset(*datasetImport));

  }

  return true;
}

//----------------------------------------
bool CWorkspaceDataset::CheckFiles()
{
  bool bOk = true;
  CObMap::iterator it;

  for (it = m_datasets.begin() ; it != m_datasets.end() ; it++)
  {
    CDataset* dataset = dynamic_cast<CDataset*>(it->second);
    if (dataset == NULL)
    {

      wxMessageBox("ERROR in  CWorkspaceDataset::CheckFiles\ndynamic_cast<CDataset*>(it->second) returns NULL pointer"
                   "\nList seems to contain objects other than those of the class CDataset",
                   "Error",
                    wxOK | wxCENTRE | wxICON_ERROR);
      return false;
    }

    try
    {
      dataset->GetProductList()->CheckFiles();
    }
    catch (CException& e)
    {
      bOk = false;
      wxMessageBox(wxString::Format("Dataset '%s':\nUnable to process files. Please apply correction\n\nReason:\n%s",
                                    dataset->GetName().c_str(),
                                    e.what()),
                  "Warning",
                  wxOK | wxICON_EXCLAMATION);

    }
  }

  return bOk;
}
//----------------------------------------
bool CWorkspaceDataset::SaveConfig(bool flush)
{
  bool bOk = true;

  if (m_config == NULL)
  {
    return true;
  }

  bOk &= DeleteConfigFile();

  bOk &= SaveCommonConfig();

  bOk &= SaveConfigDataset();


  if (flush)
  {
    m_config->Flush();
  }

  return bOk;

}
//----------------------------------------
bool CWorkspaceDataset::SaveConfigDataset()
{
  bool bOk = true;
  if (m_config == NULL)
  {
    return true;
  }

  wxString entry;

  CObMap::iterator it;

  int32_t index = 0;

  for (it = m_datasets.begin() ; it != m_datasets.end() ; it++)
  {
    index++;
    CDataset* dataset = dynamic_cast<CDataset*>(it->second);
    if (dataset == NULL)
    {

      wxMessageBox("ERROR in  CWorkspaceDataset::SaveConfigDataset\ndynamic_cast<CDataset*>(it->second) returns NULL pointer"
                   "\nList seems to contain objects other than those of the class CDataset",
                   "Error",
                    wxOK | wxCENTRE | wxICON_ERROR);
      return false;
    }

    m_config->SetPath("/" + GROUP_DATASETS);

    bOk &= m_config->Write(wxString::Format(ENTRY_DSNAME + "%d", index),
                           dataset->GetName());

    dataset->SaveConfig(m_config);
  }

  return bOk;
}

//----------------------------------------
bool CWorkspaceDataset::LoadConfig()
{
  bool bOk = true;


  if (m_config == NULL)
  {
    return false;
  }

  bOk &= LoadCommonConfig();

  bOk &= LoadConfigDataset();


  return bOk;
}

//----------------------------------------
bool CWorkspaceDataset::LoadConfigDataset()
{
  bool bOk = true;
  if (m_config == NULL)
  {
    return true;
  }

  m_config->SetPath("/" + GROUP_DATASETS);

  long maxEntries = m_config->GetNumberOfEntries();
  wxString entry;
  wxString valueString;
  long i = 0;

  do
  {
    bOk = m_config->GetNextEntry(entry, i);
    if (bOk)
    {
      valueString = m_config->Read(entry);
      InsertDataset(valueString);
    }
  }
  while (bOk);

  CObMap::iterator it;

  int32_t index = 0;

  for (it = m_datasets.begin() ; it != m_datasets.end() ; it++)
  {
    index++;
    CDataset* dataset = dynamic_cast<CDataset*>(it->second);
    if (dataset == NULL)
    {

      wxMessageBox("ERROR in  CWorkspaceDataset::LoadConfigDataset\ndynamic_cast<CDataset*>(it->second) returns NULL pointer"
                   "\nList seems to contain objects other than those of the class CDataset",
                   "Error",
                    wxOK | wxCENTRE | wxICON_ERROR);
      return false;
    }

    dataset->LoadConfig(m_config);
    if (m_ctrlDatasetFiles)
    {
      bOk = dataset->CtrlFiles();
      if (bOk)
      {
        try
        {
          //Just to initialize 'product class' and 'product type'
          dataset->GetProductList()->CheckFiles(true);
          /*
          wxString msg = dataset->GetProductList()->GetMessage().c_str();
          if (!(msg.IsEmpty()))
          {
            wxMessageBox(msg,
                      "Warning",
                      wxOK | wxICON_INFORMATION);
          }
          */
        }
        catch (CException& e)
        {
          e.what(); // to avoid compilation warning
          /*
          bOk = false;
          wxMessageBox(wxString::Format("Unable to process files. Please apply correction\n\nReason:\n%s",
                                        e.what()),
                      "Warning",
                      wxOK | wxICON_EXCLAMATION);
        */

        }

      }
    }

    /*
    if (bOk == false)
    {
      return false;
    }
    */
  }

  return true;
}
//----------------------------------------
bool CWorkspaceDataset::RenameDataset(CDataset* dataset, const wxString& newName)
{

  if (dataset->GetName().Cmp(newName) == 0)
  {
    return true;
  }

  bool bOk = m_datasets.RenameKey((const char *)dataset->GetName().c_str(), (const char *)newName.c_str());
  if (bOk == false)
  {
   return false;
  }

  dataset->SetName(newName);

  return true;
}

//----------------------------------------
bool CWorkspaceDataset::DeleteDataset(CDataset* dataset)
{
 bool bOk = m_datasets.Erase((const char *)dataset->GetName().c_str());

 return bOk;
}

//----------------------------------------
wxString CWorkspaceDataset::GetDatasetNewName()
{
  int32_t i = m_datasets.size();
  wxString key;

  do
  {
    key = wxString::Format(WKS_DATASET_NAME +"_%d", i + 1);
    i++;
  }
  while (m_datasets.Exists((const char *)key.c_str()) != NULL);

  return key;
}
//----------------------------------------
void CWorkspaceDataset::Close()
{
  CObMap::iterator it;

  for (it = m_datasets.begin() ; it != m_datasets.end() ; it++)
  {
    CDataset* dataset = dynamic_cast<CDataset*>(it->second);
    if (dataset != NULL)
    {
      //dataset->DeleteProduct();
    }
  }
}

//----------------------------------------
int32_t CWorkspaceDataset::GetDatasetCount()
{
  return m_datasets.size();
}
//----------------------------------------
CDataset* CWorkspaceDataset::GetDataset(const wxString& name)
{
  return dynamic_cast<CDataset*>(m_datasets.Exists((const char *)name.c_str()));
}
//----------------------------------------
void CWorkspaceDataset::GetDatasetNames(wxArrayString& array)
{
  CObMap::iterator it;

  for (it = m_datasets.begin() ; it != m_datasets.end() ; it++)
  {
    //std::string value = it->first;
    array.Add( (it->first).c_str());
  }
}
//----------------------------------------
void CWorkspaceDataset::GetDatasetNames(wxComboBox& combo)
{
  CObMap::iterator it;

  for (it = m_datasets.begin() ; it != m_datasets.end() ; it++)
  {
    //std::string value = it->first;
    combo.Append( (it->first).c_str());
  }
}

//----------------------------------------
bool CWorkspaceDataset::InsertDataset(wxString& name)
{
  if (m_datasets.Exists((const char *)name.c_str()))
  {
    return false;
  }

  m_datasets.Insert((const char *)name.c_str(), new CDataset(name));
  return true;
}
//----------------------------------------
void CWorkspaceDataset::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  CWorkspace::Dump(fOut);

  fOut << "==> Dump a CWorkspaceDataset Object at "<< this << std::endl;

  fOut << "==> END Dump a CWorkspaceDataset Object at "<< this << std::endl;

  fOut << std::endl;

}

//-------------------------------------------------------------
//------------------- CWorkspaceFormula class --------------------
//-------------------------------------------------------------
//----------------------------------------
CWorkspaceFormula::CWorkspaceFormula()
{
  Init();
}
//----------------------------------------
CWorkspaceFormula::CWorkspaceFormula(const wxString& name, const wxFileName& path, bool createPath)
      : CWorkspace(name, path, createPath)
{
  Init();
}

//----------------------------------------
CWorkspaceFormula::CWorkspaceFormula(const wxString& name, const wxString& path, bool createPath)
      : CWorkspace(name, path, createPath)
{
  Init();
}

//----------------------------------------
CWorkspaceFormula::~CWorkspaceFormula()
{

}
//----------------------------------------
void CWorkspaceFormula::Init()
{
}
//----------------------------------------

void CWorkspaceFormula::AmendFormulas(const CStringArray& keys, CProduct* product, const wxString& record)
{
  m_formulas.Amend(keys, product, record);
  //SaveConfigPredefinedFormula();
  SaveConfigFormula();

}

//----------------------------------------
void CWorkspaceFormula::SetFormulasToImport(const wxArrayString& array)
{
  m_formulasToImport.Clear();
  m_formulasToImport = array;
}

//----------------------------------------
bool CWorkspaceFormula::IsFormulaToImport(const wxString& name)
{
  bool bOk = false;
  uint32_t n = m_formulasToImport.GetCount();

  for(uint32_t i = 0 ; i < n ; i++)
  {
    if (name.CmpNoCase(m_formulasToImport[i]) == 0)
    {
      bOk = true;
      break;
    }
  }

  return bOk;
}
//----------------------------------------
bool CWorkspaceFormula::Import(CWorkspace* wks)
{
  if (wks == NULL)
  {
    return true;
  }

  CWorkspaceFormula* wksToImport = dynamic_cast<CWorkspaceFormula*>(wks);
  if (wksToImport == NULL)
  {
    return true;
  }

  if (m_importBitSet == NULL)
  {
    return true;
  }

  if (m_importBitSet->m_bitSet.test(IMPORT_FORMULA_INDEX) == false)
  {
    return true;
  }


  int32_t countImport = m_formulasToImport.GetCount();

  CObMap::iterator it;
  for (it = wksToImport->GetFormulas()->begin() ; it != wksToImport->GetFormulas()->end()  ; it++)
  {
    CFormula* formulaImport = dynamic_cast<CFormula*>(it->second);
    if (formulaImport == NULL)
    {

      wxMessageBox("ERROR in  CWorkspaceFormula::Import\ndynamic_cast<CFormula*>(it->second) returns NULL pointer"
                   "\nList seems to contain objects other than those of the class CFormula",
                   "Error",
                    wxOK | wxCENTRE | wxICON_ERROR);
      return false;
    }
    if (formulaImport->IsPredefined())
    {
      continue;
    }

    wxString formulaNewName = formulaImport->GetName();

    // if import array is not empty : test if import or not
    // else import all formulas
    if (countImport > 0)
    {
      if (!IsFormulaToImport(formulaNewName))
      {
        continue;
      }
    }


    CFormula* formula = GetFormula(formulaImport->GetName());
    if (formula != NULL)
    {
      CFormula* formulaTmp = NULL;
      do
      {
        formulaNewName.Append("_from_");
        formulaNewName.Append(wksToImport->GetRootKey());
        formulaTmp = GetFormula(formulaNewName);

      } while (formulaTmp != NULL);


      wxMessageBox(wxString::Format("Import formula '%s':\n\nA formula with the same name already exists.\n\nImported formula has been renamed to:\n\n '%s'\n",
                                    formulaImport->GetName().c_str(),
                                    formulaNewName.c_str()),
                  "Warning",
                  wxOK | wxICON_EXCLAMATION);
    }

    CFormula* newFormula = new CFormula(*formulaImport);
    newFormula->SetName(formulaNewName);

    m_formulas.Insert((const char *)newFormula->GetName().c_str(), newFormula, false);

  }

  return true;
}

//----------------------------------------
void CWorkspaceFormula::AddFormula(CFormula& formula)
{
  m_formulas.InsertUserDefined(formula);
}
//----------------------------------------
void CWorkspaceFormula::RemoveFormula(const wxString& name)
{
  m_formulas.Erase((const char *)name.c_str());
}
//----------------------------------------
wxString CWorkspaceFormula::GetCommentFormula(const wxString& name)
{
  return m_formulas.GetCommentFormula(name);
}
//----------------------------------------
wxString CWorkspaceFormula::GetDescFormula(const wxString& name, bool alias)
{
  return m_formulas.GetDescFormula(name, alias);
}
//----------------------------------------
CFormula* CWorkspaceFormula::GetFormula(const wxString& name)
{
  return dynamic_cast<CFormula*>(m_formulas.Exists((const char *)name.c_str()));
}
//----------------------------------------
CFormula* CWorkspaceFormula::GetFormula(CMapFormula::iterator it)
{
  return dynamic_cast<CFormula*>(it->second);
}
//----------------------------------------
void CWorkspaceFormula::GetFormulaNames(CStringMap& stringMap, bool getPredefined /* = true*/, bool getUser /* = true*/)
{
  CMapFormula::iterator it;
  stringMap.RemoveAll();

  for (it = m_formulas.begin() ; it != m_formulas.end() ; it++)
  {
    CFormula* formula = GetFormula(it);
    if (formula != NULL)
    {
      if ( ( getPredefined && formula->IsPredefined() ) || ( getUser && !formula->IsPredefined() ) )
      {
        stringMap.Insert((const char *)formula->GetName().c_str(), (const char *)formula->GetDescription().c_str());
      }
    }
  }
}
//----------------------------------------
void CWorkspaceFormula::GetFormulaNames(wxArrayString& array, bool getPredefined /* = true*/, bool getUser /* = true*/)
{
  CMapFormula::iterator it;

  for (it = m_formulas.begin() ; it != m_formulas.end() ; it++)
  {
    CFormula* formula = GetFormula(it);
    if (formula != NULL)
    {
      if ( ( getPredefined && formula->IsPredefined() ) || ( getUser && !formula->IsPredefined() ) )
      {
        array.Add( (it->first).c_str());
      }
    }
  }
}
//----------------------------------------
void CWorkspaceFormula::GetFormulaNames(wxComboBox& combo, bool getPredefined /* = true*/, bool getUser /* = true*/)
{
  CMapFormula::iterator it;

  for (it = m_formulas.begin() ; it != m_formulas.end() ; it++)
  {
    CFormula* formula = GetFormula(it);
    if (formula != NULL)
    {
      if ( ( getPredefined && formula->IsPredefined() ) || ( getUser && !formula->IsPredefined() ) )
      {
        combo.Append( (it->first).c_str());
      }
    }
  }
}
//----------------------------------------
void CWorkspaceFormula::GetFormulaNames(wxListBox& lb, bool getPredefined /* = true*/, bool getUser /* = true*/)
{
  CMapFormula::iterator it;

  for (it = m_formulas.begin() ; it != m_formulas.end() ; it++)
  {
    CFormula* formula = GetFormula(it);
    if (formula != NULL)
    {
      if ( ( getPredefined && formula->IsPredefined() ) || ( getUser && !formula->IsPredefined() ) )
      {
        lb.Append( (it->first).c_str());
      }
    }
  }
}


//----------------------------------------
bool CWorkspaceFormula::SaveConfig(bool flush)
{
  bool bOk = true;

  if (m_config == NULL)
  {
    return true;
  }

  bOk &= DeleteConfigFile();

  bOk &= SaveCommonConfig();
  bOk &= SaveConfigFormula();

  if (flush)
  {
    m_config->Flush();
  }

  return bOk;

}
//----------------------------------------
bool CWorkspaceFormula::SaveConfigFormula()
{
  bool bOk = true;
  if (m_config == NULL)
  {
    return true;
  }

  m_formulas.SaveConfig(m_config, false);

  return bOk;
}
//----------------------------------------
bool CWorkspaceFormula::SaveConfigPredefinedFormula()
{

  wxFileName formulaPath;
  formulaPath.Assign(CTools::GetDataDir().c_str(), CMapFormula::m_predefFormulaFile);
  formulaPath.Normalize();

  wxFileConfig* config = new wxFileConfig(wxEmptyString, wxEmptyString, formulaPath.GetFullPath(), wxEmptyString, wxCONFIG_USE_LOCAL_FILE);

  bool bOk = config->DeleteAll();

  bOk &= m_formulas.SaveConfig(config, true);

  delete config;
  config = NULL;

  return bOk;
}
//----------------------------------------
bool CWorkspaceFormula::LoadConfig()
{
  bool bOk = false;

  if (m_config == NULL)
  {
    return bOk;
  }

  bOk = LoadCommonConfig();
  bOk &= LoadConfigFormula();


  return bOk;
}
//----------------------------------------
bool CWorkspaceFormula::LoadConfigFormula()
{
  bool bOk = true;
  if (m_config == NULL)
  {
    return true;
  }

  m_formulas.InsertUserDefined(m_config);

  return true;
}
//----------------------------------------
void CWorkspaceFormula::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  CWorkspace::Dump(fOut);

  fOut << "==> Dump a CWorkspaceFormula Object at "<< this << std::endl;

  fOut << "==> END Dump a CWorkspaceFormula Object at "<< this << std::endl;

  fOut << std::endl;

}

//-------------------------------------------------------------
//------------------- CWorkspaceOperation class --------------------
//-------------------------------------------------------------
//----------------------------------------
CWorkspaceOperation::CWorkspaceOperation()
{
  Init();
}
//----------------------------------------
CWorkspaceOperation::CWorkspaceOperation(const wxString& name, const wxFileName& path, bool createPath)
      : CWorkspace(name, path, createPath)
{
  Init();
}

//----------------------------------------
CWorkspaceOperation::CWorkspaceOperation(const wxString& name, const wxString& path, bool createPath)
      : CWorkspace(name, path, createPath)
{
  Init();
}

//----------------------------------------
CWorkspaceOperation::~CWorkspaceOperation()
{

  m_operations.RemoveAll();
}
//----------------------------------------
void CWorkspaceOperation::Init()
{
}
//----------------------------------------
bool CWorkspaceOperation::Import(CWorkspace* wks)
{
  if (wks == NULL)
  {
    return true;
  }

  CWorkspaceOperation* wksToImport = dynamic_cast<CWorkspaceOperation*>(wks);
  if (wksToImport == NULL)
  {
    return true;
  }

  if (m_importBitSet == NULL)
  {
    return true;
  }

  if (m_importBitSet->m_bitSet.test(IMPORT_OPERATION_INDEX) == false)
  {
    return true;
  }


  CObMap::iterator it;
  for (it = wksToImport->GetOperations()->begin() ; it != wksToImport->GetOperations()->end()  ; it++)
  {
    COperation* operationImport = dynamic_cast<COperation*>(it->second);
    if (operationImport == NULL)
    {

      wxMessageBox("ERROR in  CWorkspaceOperation::Import\ndynamic_cast<COperation*>(it->second) returns NULL pointer"
                   "\nList seems to contain objects other than those of the class COperation",
                   "Error",
                    wxOK | wxCENTRE | wxICON_ERROR);
      return false;
    }

    COperation* operation = GetOperation(operationImport->GetName());
    if (operation != NULL)
    {
      wxMessageBox(wxString::Format("Operation to import '%s':\nUnable to process, an operation with the same name already exists\n"
                                    "Import cancelled",
                                    operationImport->GetName().c_str()),
                  "Warning",
                  wxOK | wxICON_EXCLAMATION);
      return false;
    }

    m_operations.Insert((const char *)operationImport->GetName().c_str(), new COperation(*operationImport));

  }

  return true;
}

//----------------------------------------
bool CWorkspaceOperation::RenameOperation(COperation* operation, const wxString& newName)
{
  if (operation->GetName().Cmp(newName) == 0)
  {
    return true;
  }

  bool bOk = m_operations.RenameKey((const char *)operation->GetName().c_str(), (const char *)newName.c_str());
  if (bOk == false)
  {
   return false;
  }

  operation->SetName(newName);

  return true;
}


//----------------------------------------
int32_t CWorkspaceOperation::GetOperationCount()
{
  return m_operations.size();
}
//----------------------------------------
COperation* CWorkspaceOperation::GetOperation(const wxString& name)
{
  return dynamic_cast<COperation*>(m_operations.Exists((const char *)name.c_str()));
}

//----------------------------------------
bool CWorkspaceOperation::SaveConfig(bool flush)
{
  bool bOk = true;

  if (m_config == NULL)
  {
    return true;
  }

  bOk &= DeleteConfigFile();

  bOk &= SaveCommonConfig();

  bOk &= SaveConfigOperation();

  if (flush)
  {
    m_config->Flush();
  }

  return bOk;
}

//----------------------------------------
bool CWorkspaceOperation::SaveConfigOperation()
{
  bool bOk = true;
  if (m_config == NULL)
  {
    return true;
  }

  wxString entry;

  CObMap::iterator it;

  int32_t index = 0;

  for (it = m_operations.begin() ; it != m_operations.end() ; it++)
  {
    index++;
    COperation* operation = dynamic_cast<COperation*>(it->second);
    if (operation == NULL)
    {

      wxMessageBox("ERROR in  CWorkspaceOperation::SaveConfigOperation\ndynamic_cast<COperation*>(it->second) returns NULL pointer"
                   "\nList seems to contain objects other than those of the class COperation",
                   "Error",
                    wxOK | wxCENTRE | wxICON_ERROR);
      return false;
    }

    m_config->SetPath("/" + GROUP_OPERATIONS);

    bOk &= m_config->Write(wxString::Format(ENTRY_OPNAME + "%d", index),
                           operation->GetName());

    operation->SaveConfig(m_config);
  }

  return bOk;
}

//----------------------------------------
bool CWorkspaceOperation::LoadConfig()
{
  bool bOk = true;

  if (m_config == NULL)
  {
    return bOk;
  }

  bOk &= LoadCommonConfig();

  bOk &= LoadConfigOperation();

  return bOk;
}

//----------------------------------------
bool CWorkspaceOperation::LoadConfigOperation()
{
  bool bOk = true;
  if (m_config == NULL)
  {
    return true;
  }

  m_config->SetPath("/" + GROUP_OPERATIONS);

  long maxEntries = m_config->GetNumberOfEntries();
  wxString entry;
  wxString valueString;
  long i = 0;

  do
  {
    bOk = m_config->GetNextEntry(entry, i);
    if (bOk)
    {
      valueString = m_config->Read(entry);
      InsertOperation(valueString);
    }
  }
  while (bOk);

  CObMap::iterator it;

  int32_t index = 0;

  for (it = m_operations.begin() ; it != m_operations.end() ; it++)
  {
    index++;
    COperation* operation = dynamic_cast<COperation*>(it->second);
    if (operation == NULL)
    {

      wxMessageBox("ERROR in  CWorkspaceOperation::LoadConfigOperation\ndynamic_cast<COperation*>(it->second) returns NULL pointer"
                   "\nList seems to contain objects other than those of the class COperation",
                   "Error",
                    wxOK | wxCENTRE | wxICON_ERROR);
      return false;
    }

    operation->LoadConfig(m_config);
  }

  return true;
}
//----------------------------------------
wxString CWorkspaceOperation::GetOperationNewName()
{
  int32_t i = m_operations.size();
  wxString key;

  do
  {
    key = wxString::Format(WKS_OPERATION_NAME +"_%d", i + 1);
    i++;
  }
  while (m_operations.Exists((const char *)key.c_str()) != NULL);

  return key;
}
//----------------------------------------
wxString CWorkspaceOperation::GetOperationCopyName(const wxString& baseName)
{
  int32_t i = 0;
  wxString key;

  do
  {
    if (i == 0)
    {
      key = wxString::Format(baseName +"_%s", "copy");
    }
    else
    {
      key = wxString::Format(baseName +"_%s%d", "copy", i);
    }
    i++;
  }
  while (m_operations.Exists((const char *)key.c_str()) != NULL);

  return key;
}

//----------------------------------------
bool CWorkspaceOperation::InsertOperation(wxString& name)
{
  if (m_operations.Exists((const char *)name.c_str()))
  {
    return false;
  }

  m_operations.Insert((const char *)name.c_str(), new COperation(name));
  return true;
}
//----------------------------------------
bool CWorkspaceOperation::InsertOperation(wxString& name, COperation* operationToCopy)
{
  if (m_operations.Exists((const char *)name.c_str()))
  {
    return false;
  }

  // Create and copy the operation
  COperation* newOperation = new COperation(*operationToCopy);

  // Set the correct names
  newOperation->SetName(name);
  newOperation->InitOutput();
  newOperation->InitExportAsciiOutput();

  m_operations.Insert((const char *)name.c_str(), newOperation);
  return true;
}
//----------------------------------------
void CWorkspaceOperation::GetOperationNames(wxArrayString& array)
{
  CObMap::iterator it;

  for (it = m_operations.begin() ; it != m_operations.end() ; it++)
  {
    //std::string value = it->first;
    array.Add( (it->first).c_str());
  }
}
//----------------------------------------
void CWorkspaceOperation::GetOperationNames(wxComboBox& combo)
{
  CObMap::iterator it;

  for (it = m_operations.begin() ; it != m_operations.end() ; it++)
  {
    //std::string value = it->first;
    combo.Append( (it->first).c_str());
  }
}
//----------------------------------------
bool CWorkspaceOperation::UseDataset(const wxString& name, CStringArray* operationNames /*= NULL*/)
{
  CObMap::iterator it;
  bool useDataset = false;
  for (it = m_operations.begin() ; it != m_operations.end() ; it++)
  {
    COperation* operation = dynamic_cast<COperation*>(it->second);
    if (operation == NULL)
    {

      wxMessageBox("ERROR in  CWorkspaceOperation::UseDataset\ndynamic_cast<COperation*>(it->second) returns NULL pointer"
                   "\nList seems to contain objects other than those of the class COperation",
                   "Error",
                    wxOK | wxCENTRE | wxICON_ERROR);
      return false;
    }

    bool useIt = operation->UseDataset(name);
    useDataset |= useIt;

    if (operationNames != NULL)
    {
      if (useIt)
      {
        operationNames->Insert((const char *)operation->GetName().c_str());
      }

    }
    else
    {
      if (useDataset)
      {
        break;
      }
    }
  }

  return useDataset;
}
//----------------------------------------
bool CWorkspaceOperation::DeleteOperation(COperation* operation)
{
 bool bOk = m_operations.Erase((const char *)operation->GetName().c_str());

 return bOk;
}

//----------------------------------------
void CWorkspaceOperation::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  CWorkspace::Dump(fOut);

  fOut << "==> Dump a CWorkspaceOperation Object at "<< this << std::endl;

  fOut << "==> END Dump a CWorkspaceOperation Object at "<< this << std::endl;

  fOut << std::endl;

}

//-------------------------------------------------------------
//------------------- CWorkspaceDisplay class --------------------
//-------------------------------------------------------------
//----------------------------------------
CWorkspaceDisplay::CWorkspaceDisplay()
{
  Init();
}
//----------------------------------------
CWorkspaceDisplay::CWorkspaceDisplay(const wxString& name, const wxFileName& path, bool createPath)
      : CWorkspace(name, path, createPath)
{
  Init();
}

//----------------------------------------
CWorkspaceDisplay::CWorkspaceDisplay(const wxString& name, const wxString& path, bool createPath)
      : CWorkspace(name, path, createPath)
{
  Init();
}

//----------------------------------------
CWorkspaceDisplay::~CWorkspaceDisplay()
{
  m_displays.RemoveAll();

}
//----------------------------------------
void CWorkspaceDisplay::Init()
{
}
//----------------------------------------
bool CWorkspaceDisplay::Import(CWorkspace* wks)
{
  if (wks == NULL)
  {
    return true;
  }

  CWorkspaceDisplay* wksToImport = dynamic_cast<CWorkspaceDisplay*>(wks);
  if (wksToImport == NULL)
  {
    return true;
  }

  if (m_importBitSet == NULL)
  {
    return true;
  }

  if (m_importBitSet->m_bitSet.test(IMPORT_DISPLAY_INDEX) == false)
  {
    return true;
  }


  CObMap::iterator it;
  for (it = wksToImport->GetDisplays()->begin() ; it != wksToImport->GetDisplays()->end()  ; it++)
  {
    CDisplay* displayImport = dynamic_cast<CDisplay*>(it->second);
    if (displayImport == NULL)
    {

      wxMessageBox("ERROR in  CWorkspaceDisplay::Import\ndynamic_cast<CDisplay*>(it->second) returns NULL pointer"
                   "\nList seems to contain objects other than those of the class CDisplay",
                   "Error",
                    wxOK | wxCENTRE | wxICON_ERROR);
      return false;
    }

    CDisplay* display = GetDisplay(displayImport->GetName());
    if (display != NULL)
    {
      wxMessageBox(wxString::Format("Operation to import '%s':\nUnable to process, an operation with the same name already exists\n"
                                    "Import cancelled",
                                    displayImport->GetName().c_str()),
                  "Warning",
                  wxOK | wxICON_EXCLAMATION);
      return false;
    }

    m_displays.Insert((const char *)displayImport->GetName().c_str(), new CDisplay(*displayImport));

  }

  return true;
}
//----------------------------------------
bool CWorkspaceDisplay::UseOperation(const wxString& name, CStringArray* displayNames /*= NULL*/)
{
  CObMap::iterator it;
  bool useOperation = false;

  for (it = m_displays.begin() ; it != m_displays.end() ; it++)
  {
    CDisplay* display = dynamic_cast<CDisplay*>(it->second);
    if (display == NULL)
    {

      wxMessageBox("ERROR in  CWorkspaceDisplay::UseOperation\ndynamic_cast<CDisplay*>(it->second) returns NULL pointer"
                   "\nList seems to contain objects other than those of the class CDisplay",
                   "Error",
                    wxOK | wxCENTRE | wxICON_ERROR);
      return false;
    }

    bool useIt = display->UseOperation(name);
    useOperation |= useIt;

    if (displayNames != NULL)
    {
      if (useIt)
      {
        displayNames->Insert((const char *)display->GetName().c_str());
      }

    }
    else
    {
      if (useOperation)
      {
        break;
      }
    }
  }

  return useOperation;
}
//----------------------------------------
int32_t CWorkspaceDisplay::GetDisplayCount()
{
  return m_displays.size();
}
//----------------------------------------
CDisplay* CWorkspaceDisplay::GetDisplay(const wxString& name)
{
  return dynamic_cast<CDisplay*>(m_displays.Exists((const char *)name.c_str()));
}

//----------------------------------------
bool CWorkspaceDisplay::SaveConfig(bool flush)
{
  bool bOk = true;

  if (m_config == NULL)
  {
    return true;
  }

  bOk &= DeleteConfigFile();

  bOk &= SaveCommonConfig();
  bOk &= SaveConfigDisplay();

  if (flush)
  {
    m_config->Flush();
  }

  return bOk;

}
//----------------------------------------
bool CWorkspaceDisplay::SaveConfigDisplay()
{
  bool bOk = true;
  if (m_config == NULL)
  {
    return true;
  }

  wxString entry;

  CObMap::iterator it;

  int32_t index = 0;

  for (it = m_displays.begin() ; it != m_displays.end() ; it++)
  {
    index++;
    CDisplay* display = dynamic_cast<CDisplay*>(it->second);
    if (display == NULL)
    {

      wxMessageBox("ERROR in  CWorkspaceDisplay::m_displays\ndynamic_cast<CDisplay*>(it->second) returns NULL pointer"
                   "\nList seems to contain objects other than those of the class CDisplay",
                   "Error",
                    wxOK | wxCENTRE | wxICON_ERROR);
      return false;
    }

    m_config->SetPath("/" + GROUP_DISPLAY);

    bOk &= m_config->Write(wxString::Format(ENTRY_DISPLAYNAME + "%d", index),
                           display->GetName());

    display->SaveConfig(m_config);
  }

  return bOk;
}

//----------------------------------------
bool CWorkspaceDisplay::LoadConfig()
{
  bool bOk = true;

  if (m_config == NULL)
  {
    return bOk;
  }

  bOk &= LoadCommonConfig();
  bOk &= LoadConfigDisplay();

  return bOk;
}
//----------------------------------------
bool CWorkspaceDisplay::LoadConfigDisplay()
{
  bool bOk = true;
  if (m_config == NULL)
  {
    return true;
  }

  m_config->SetPath("/" + GROUP_DISPLAY);

  long maxEntries = m_config->GetNumberOfEntries();
  wxString entry;
  wxString valueString;
  long i = 0;

  do
  {
    bOk = m_config->GetNextEntry(entry, i);
    if (bOk)
    {
      valueString = m_config->Read(entry);
      InsertDisplay(valueString);
    }
  }
  while (bOk);

  CObMap::iterator it;

  int32_t index = 0;

  for (it = m_displays.begin() ; it != m_displays.end() ; it++)
  {
    index++;
    CDisplay* display = dynamic_cast<CDisplay*>(it->second);
    if (display == NULL)
    {

      wxMessageBox("ERROR in  CWorkspaceDisplay::LoadConfigOperation\ndynamic_cast<CDisplay*>(it->second) returns NULL pointer"
                   "\nList seems to contain objects other than those of the class CDisplay",
                   "Error",
                    wxOK | wxCENTRE | wxICON_ERROR);
      return false;
    }

    display->LoadConfig(m_config);
  }

  return true;
}
//----------------------------------------
bool CWorkspaceDisplay::RenameDisplay(CDisplay* display, const wxString& newName)
{
  if (display->GetName().Cmp(newName) == 0)
  {
    return true;
  }

  bool bOk = m_displays.RenameKey((const char *)display->GetName().c_str(), (const char *)newName.c_str());
  if (bOk == false)
  {
   return false;
  }

  display->SetName(newName);

  return true;
}

//----------------------------------------
wxString CWorkspaceDisplay::GetDisplayNewName()
{
  int32_t i = m_displays.size();
  wxString key;

  do
  {
    key = wxString::Format(WKS_DISPLAY_NAME +"_%d", i + 1);
    i++;
  }
  while (m_displays.Exists((const char *)key.c_str()) != NULL);

  return key;
}
//----------------------------------------
bool CWorkspaceDisplay::InsertDisplay(wxString& name)
{
  if (m_displays.Exists((const char *)name.c_str()))
  {
    return false;
  }

  m_displays.Insert((const char *)name.c_str(), new CDisplay(name));
  return true;
}
//----------------------------------------
void CWorkspaceDisplay::GetDisplayNames(wxArrayString& array)
{
  CObMap::iterator it;

  for (it = m_displays.begin() ; it != m_displays.end() ; it++)
  {
    //std::string value = it->first;
    array.Add( (it->first).c_str());
  }
}
//----------------------------------------
void CWorkspaceDisplay::GetDisplayNames(wxComboBox& combo)
{
  CObMap::iterator it;

  for (it = m_displays.begin() ; it != m_displays.end() ; it++)
  {
    //std::string value = it->first;
    combo.Append( (it->first).c_str());
  }
}
//----------------------------------------
bool CWorkspaceDisplay::DeleteDisplay(CDisplay* display)
{
 bool bOk = m_displays.Erase((const char *)display->GetName().c_str());

 return bOk;
}


//----------------------------------------
void CWorkspaceDisplay::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  CWorkspace::Dump(fOut);

  fOut << "==> Dump a CWorkspaceDisplay Object at "<< this << std::endl;

  fOut << "==> END Dump a CWorkspaceDisplay Object at "<< this << std::endl;

  fOut << std::endl;

}
