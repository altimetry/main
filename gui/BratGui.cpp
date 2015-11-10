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
    #pragma implementation "BratGui.h"
#endif

// For compilers that support precompilation
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// Include private headers

#include <locale.h>

#include "wx/mimetype.h"

//#ifdef WIN32
//  #include <crtdbg.h>
//#endif


#include "Trace.h"
#include "List.h"
#include "Tools.h"
#include "Exception.h"
#include "InternalFilesFactory.h"
#include "InternalFiles.h"
#include "InternalFilesYFX.h"
#include "InternalFilesZFXY.h"
#include "Product.h"
#include "BratAlgorithmBase.h"
using namespace brathl;

#include "BratGui.h"
#include "DirTraverser.h"
#include "MapTypeDisp.h"

//#ifdef WIN32
//#include <crtdbg.h>
//#endif

#ifdef WIN32
const std::string BRATHL_ICON_FILENAME = "BratIcon.ico";
const wxString BRATCREATEYFX_EXE = "BratCreateYFX.exe";
const wxString BRATCREATEZFXY_EXE = "BratCreateZFXY.exe";
const wxString BRATDISPLAY_EXE = "BratDisplay.exe";
const wxString BRATEXPORTASCII_EXE = "BratExportAscii.exe";
const wxString BRATEXPORTGEOTIFF_EXE = "BratExportGeoTiff.exe";
const wxString BRATSHOWSTATS_EXE = "BratStats.exe";
const wxString BRATSCHEDULER_EXE = "BratScheduler.exe";
#else
const std::string BRATHL_ICON_FILENAME = "BratIcon.bmp";
const wxString BRATCREATEYFX_EXE = "BratCreateYFX";
const wxString BRATCREATEZFXY_EXE = "BratCreateZFXY";
const wxString BRATDISPLAY_EXE = "BratDisplay";
const wxString BRATEXPORTASCII_EXE = "BratExportAscii";
const wxString BRATEXPORTGEOTIFF_EXE = "BratExportGeoTiff";
const wxString BRATSHOWSTATS_EXE = "BratStats";
const wxString BRATSCHEDULER_EXE = "BratScheduler";
#endif

// WDR: class implementations


//------------------------------------------------------------------------------
// MainApp
//------------------------------------------------------------------------------

IMPLEMENT_APP(CBratGuiApp)

CBratGuiApp::CBratGuiApp()
{
  m_config = NULL;
  m_currentTree = NULL;
  //_CrtSetBreakAlloc(10);

}
//----------------------------------------
CWorkspace* CBratGuiApp::GetCurrentWorkspace()
{
  if (m_currentTree == NULL)
  {
    m_currentTree = &m_tree;
  }
  return m_currentTree->GetRootData();
}
//----------------------------------------
CWorkspaceDataset* CBratGuiApp::GetCurrentWorkspaceDataset()
{
  if (m_currentTree == NULL)
  {
    m_currentTree = &m_tree;
  }
  return dynamic_cast<CWorkspaceDataset*>(m_currentTree->FindObject((const char *)GetWorkspaceKey(WKS_DATASET_NAME).c_str()));
}
//----------------------------------------
CWorkspaceOperation* CBratGuiApp::GetCurrentWorkspaceOperation()
{
  if (m_currentTree == NULL)
  {
    m_currentTree = &m_tree;
  }
  return dynamic_cast<CWorkspaceOperation*>(m_currentTree->FindObject((const char *)GetWorkspaceKey(WKS_OPERATION_NAME).c_str()));
}
//----------------------------------------
CWorkspaceFormula* CBratGuiApp::GetCurrentWorkspaceFormula()
{
  if (m_currentTree == NULL)
  {
    m_currentTree = &m_tree;
  }
  return dynamic_cast<CWorkspaceFormula*>(m_currentTree->FindObject((const char *)GetWorkspaceKey(WKS_FORMULA_NAME).c_str()));
}
//----------------------------------------
CWorkspaceDisplay* CBratGuiApp::GetCurrentWorkspaceDisplay()
{
  if (m_currentTree == NULL)
  {
    m_currentTree = &m_tree;
  }
  return dynamic_cast<CWorkspaceDisplay*>(m_currentTree->FindObject((const char *)GetWorkspaceKey(WKS_DISPLAY_NAME).c_str()));
}
//----------------------------------------
wxString CBratGuiApp::GetWorkspaceKey(const wxString& subKey)
{
  CWorkspace* wks = GetCurrentWorkspace();

  if (wks == NULL)
  {
    return "";
  }

  return wks->GetKey() + CTreeWorkspace::m_keyDelimiter + subKey;
}

//----------------------------------------
bool CBratGuiApp::OnInit()
{
#ifdef WIN32
  #ifdef _DEBUG
    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
  #endif
#endif

  // Register Brat algorithms
  CBratAlgorithmBase::RegisterAlgorithms();

  bool bOk = true;


  // To be sure that number have always a decimal point (and not a comma
  // or something else)
  setlocale(LC_NUMERIC, "C");

  m_execName.Assign(wxGetApp().argv[0]);

  wxString appPath = GetExecPathName();

  if (getenv(BRATHL_ENVVAR) == NULL)
  {
      // Note that this won't work on Mac OS X when you use './BratGui' from within the Contents/MacOS directory of
      // you .app bundle. The problem is that in that case Mac OS X will change the current working directory to the
      // location of the .app bundle and thus the calculation of absolute paths will break
      CTools::SetDataDirForExecutable(wxGetApp().argv[0]);
  }


  //wxFileName traceFileName;
  //traceFileName.AssignDir(wxGetApp().GetExecPathName());
  //traceFileName.SetFullName("GuiTrace.log");
  //traceFileName.Normalize();
  //CTrace::GetInstance(traceFileName.GetFullPath().c_str());

  //auto_ptr<CTrace>pTrace(CTrace::GetInstance(argc, argv));
  //pTrace->SetTraceLevel(5);

  if (appPath != "")
  {
    m_execYFXName.AssignDir(appPath);
    m_execZFXYName.AssignDir(appPath);
    m_execDisplayName.AssignDir(appPath);
    m_execExportAsciiName.AssignDir(appPath);
    m_execExportGeoTiffName.AssignDir(appPath);
    m_execShowStatsName.AssignDir(appPath);
    if (getenv(BRATHL_ENVVAR) == NULL)
    {
      CTools::SetDataDirForExecutable(wxGetApp().argv[0]);
    }
  }

  if (!CTools::DirectoryExists(CTools::GetDataDir()))
  {
      std::cerr << "ERROR: " << CTools::GetDataDir() << " is not a valid directory" << std::endl;
      ::wxMessageBox(wxString(CTools::GetDataDir().c_str()) + " is not a valid directory -- BRAT cannot continue. \n\nAre you sure your " + BRATHL_ENVVAR + " environment variable is set correctly?", "BRAT ERROR");
      return false;
  }


  std::string errorMsg;
  if (!CTools::LoadAndCheckUdUnitsSystem(errorMsg))
  {
      std::cerr << errorMsg << std::endl;
      ::wxMessageBox(errorMsg.c_str(), "BRAT ERROR");
      return false;
  }


  // Load aliases dictionnary
  CAliasesDictionary::LoadAliasesDictionary(&errorMsg, false);
  if (!(errorMsg.empty())) 
  {
    std::cerr << "WARNING: " << errorMsg << std::endl;
    wxMessageBox(errorMsg.c_str(),
		 "Warning",
		 wxOK | wxCENTRE | wxICON_EXCLAMATION);
  }

  m_execYFXName.SetFullName(BRATCREATEYFX_EXE);
  m_execYFXName.MakeAbsolute();

  m_execZFXYName.SetFullName(BRATCREATEZFXY_EXE);
  m_execZFXYName.MakeAbsolute();

  m_execDisplayName.SetFullName(BRATDISPLAY_EXE);
  m_execDisplayName.MakeAbsolute();

  m_execExportAsciiName.SetFullName(BRATEXPORTASCII_EXE);
  m_execExportAsciiName.MakeAbsolute();

  m_execExportGeoTiffName.SetFullName(BRATEXPORTGEOTIFF_EXE);
  m_execExportGeoTiffName.MakeAbsolute();

  m_execShowStatsName.SetFullName(BRATSHOWSTATS_EXE);
  m_execShowStatsName.MakeAbsolute();

  m_execBratSchedulerName.AssignDir(appPath);
  m_execBratSchedulerName.SetFullName(BRATSCHEDULER_EXE);
  m_execBratSchedulerName.MakeAbsolute();

  SetExecBratSchedulerName();

  CProduct::CodaInit();
  //CProduct::SetCodaReleaseWhenDestroy(false);

  m_config = new wxFileConfig(wxGetApp().GetAppName(), wxEmptyString, wxEmptyString, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);


  try {
    LoadConfig();
  }
  catch(CException &e) {
    wxMessageBox(wxString::Format("An error occured while loading Brat configuration (CBratGui::LoadConfig)\nNavive std::exception: %s", e.what()),
		 "Warning",
		 wxOK | wxCENTRE | wxICON_EXCLAMATION);
  }


  m_frame = new CGuiFrame( NULL, -1, BRATGUI_TITLE);

  m_frame->Show( TRUE );

  //if (m_lastWksPath.IsEmpty() == false)
//  {
  try {

    bOk = m_frame->OpenWorkspace(m_lastWksPath);
    if (bOk == false)
    {
      wxMessageBox(wxString::Format("Unable to load last used workspace located in '%s' ",
                                    m_lastWksPath.c_str()),
                   "Warning",
                    wxOK | wxCENTRE | wxICON_EXCLAMATION);

    }

  }
  catch(CException &e) {
    wxMessageBox(wxString(e.what()),
		 "Warning",
		 wxOK | wxCENTRE | wxICON_EXCLAMATION);
  }

    m_frame->SetTitle();
  //}

   GotoLastPageReached();

  return true;
}

//----------------------------------------
int CBratGuiApp::OnExit()
{

  if (m_config != NULL)
  {
    SaveConfig();
    delete m_config;
    m_config = NULL;
  }

  m_tree.DeleteTree();

  CProduct::CodaRelease();

  return 0;
}
//----------------------------------------
void CBratGuiApp::SetExecBratSchedulerName()
{
  m_execBratSchedulerName.AssignDir(GetExecBratSchedulerPathName());
  m_execBratSchedulerName.SetFullName(BRATSCHEDULER_EXE);
  m_execBratSchedulerName.MakeAbsolute();

}
//----------------------------------------
bool CBratGuiApp::RemoveFile(const wxString& name)
{
  bool bOk = true;

  bOk = wxFileExists(name);
  if (bOk == false)
  {
    return true;
  }

  try
  {
    bOk = wxRemoveFile(name);
  }
  catch (...)
  {
    //Nothing to do
    bOk = false;
  }
  return bOk;
}
//----------------------------------------
bool CBratGuiApp::RenameFile(const wxString& oldName, const wxString& newName)
{
  bool bOk = true;

  bOk = wxFileExists(oldName);
  if (bOk == false)
  {
    return true;
  }

  try
  {
    bOk = wxRenameFile(oldName, newName);
  }
  catch (...)
  {
    //Nothing to do
    bOk = false;
  }

  return bOk;
}

//----------------------------------------
int32_t CBratGuiApp::GotoPage(int32_t pos)
{
  return GetMainnotebook()->SetSelection(pos);
}
//----------------------------------------
int32_t CBratGuiApp::GotoLastPageReached()
{
  if (m_lastPageReached.IsEmpty())
  {
    return -1;
  }

  return GotoPage(m_lastPageReached);
}
//----------------------------------------
int32_t CBratGuiApp::GotoPage(const wxString& name)
{
  for (uint32_t i = 0 ; i < GetMainnotebook()->GetPageCount() ; i++)
  {
    if (GetMainnotebook()->GetPageText(i).CmpNoCase(name) == 0)
    {
      return GotoPage(i);
    }
  }

  return -1;
}
//----------------------------------------
wxString CBratGuiApp::GetCurrentPageText()
{
  int32_t idx = GetMainnotebook()->GetSelection();

  wxString result;

  if ( idx == wxNOT_FOUND)
  {
    return result;
  }


  return GetMainnotebook()->GetPageText(idx);
}
//----------------------------------------
bool CBratGuiApp::IsCurrentLogPage()
{
  return (GetCurrentPageText().CmpNoCase(LOG_PAGE_NAME) == 0);
}
//----------------------------------------
bool CBratGuiApp::CanDeleteOperation(const wxString& name, CStringArray* displayNames /*= NULL*/)
{
  bool canDelete = true;
  CWorkspaceDisplay* wks = GetCurrentWorkspaceDisplay();
  if (wks == NULL)
  {
    return true;
  }

  canDelete &= (wks->UseOperation(name, displayNames) == false);

  return canDelete;
}
//----------------------------------------
bool CBratGuiApp::CanDeleteDisplay(const wxString& name)
{
  bool canDelete = true;

  return canDelete;
}
//----------------------------------------
bool CBratGuiApp::CanDeleteDataset(const wxString& name, CStringArray* operationNames /*= NULL*/)
{
  bool canDelete = true;
  CWorkspaceOperation* wks = GetCurrentWorkspaceOperation();
  if (wks == NULL)
  {
    return true;
  }

  canDelete &= (wks->UseDataset(name, operationNames) == false);

  return canDelete;
}
//----------------------------------------
void CBratGuiApp::DetermineCharSize(wxWindow* wnd, int32_t& width, int32_t& height)
{
  wxStaticText w(wnd, -1, "W", wxDefaultPosition, wxSize(-1,-1));
  w.GetSize(&width, &height);
//  w.Destroy();
}
//----------------------------------------
wxString CBratGuiApp::BuildUserManualPath()
{
  wxFileName fileName;

  fileName.AssignDir(wxGetApp().GetExecPathName());
  fileName.AppendDir("..");
  fileName.AppendDir(BRAT_DOC_SUBDIR);
  fileName.SetFullName(BRAT_USER_MANUAL);
  fileName.Normalize();

  return fileName.GetFullPath();
}
//----------------------------------------
void CBratGuiApp::ViewUserManual()
{
  wxFileName fileName;

  if (m_userManual.IsEmpty())
  {
    //m_userManual = CBratGuiApp::FindFile(BRAT_USER_MANUAL);
    m_userManual = BuildUserManualPath();
  }
  else
  {
    fileName.Assign(m_userManual);
    wxString fullName = fileName.GetFullName();

    bool versionIsOk= (fullName.Cmp(BRAT_USER_MANUAL) == 0);

    if (!versionIsOk)
    {
      m_userManual = BuildUserManualPath();
    }
  }

  if (m_userManual.IsEmpty() || (!wxFileExists(m_userManual)))
  {
      m_userManual = wxFileSelector(wxString::Format("Where is the file %s ?",  BRAT_USER_MANUAL.c_str()),
                                      wxGetCwd(),
                                      BRAT_USER_MANUAL);
  }

  if (m_userManual.IsEmpty())
  {
    wxMessageBox(wxString::Format("User's manual '%s' is not found. Not path to the file has been selected.",
                                  BRAT_USER_MANUAL.c_str()),
                 "Information",
                  wxOK | wxCENTRE | wxICON_INFORMATION);
    return;

  }
  if (wxFileExists(m_userManual) == false)
  {
    wxMessageBox(wxString::Format("User's manual '%s' doesn't exist.",
                                  m_userManual.c_str()),
                 "Warning",
                  wxOK | wxCENTRE | wxICON_EXCLAMATION);
    return;

  }

  wxString ext = m_userManual.AfterLast(_T('.'));
  wxFileType *ft = wxTheMimeTypesManager->GetFileTypeFromExtension(ext);
  if ( ft == NULL )
  {
    //wxMessageBox(wxString::Format("There is no program associated with '%s' file. "
    //                              "Brat can't open the User Manual from '%s'. "
    //                              "Check your system configuration and/or try to open the User Manuel manually (outside Brat)",
    //                              ext.c_str(),
    //                              m_userManual.c_str()),
    //             "Warning",
    //              wxOK | wxCENTRE | wxICON_EXCLAMATION);

    if (m_userManualViewer.IsEmpty())
    {
      wxMessageBox(wxString::Format("No default '%s' viewer found."
                                       "Please click OK and then select an executable program to view '%s'",
                                       ext.c_str(),
                                       m_userManual.c_str()),
                    "Warning",
                    wxOK | wxCENTRE | wxICON_EXCLAMATION);

      wxFileName currentDir;
      currentDir.AssignDir(".");
      currentDir.Normalize();

      wxString wildcards = wxString::Format(_T("All files (%s)|%s"),
                                          wxFileSelectorDefaultWildcardStr,
                                          wxFileSelectorDefaultWildcardStr
                                          );

      wxFileDialog execDialog(NULL, wxString::Format("Choose a '%s' viewer executable", ext.c_str()),
                              currentDir.GetPath(), wxEmptyString, wildcards, wxFD_CHANGE_DIR|wxFD_FILE_MUST_EXIST);

      if (execDialog.ShowModal() == wxID_OK) 
      {
        m_userManualViewer = execDialog.GetPath();
      }
    }
    
    if (!(m_userManualViewer.IsEmpty()))
    {
      wxString cmd = m_userManualViewer;
      cmd.Append(" ");
      cmd.Append(m_userManual);
      ::wxExecute(cmd);
    }

    return;
  }

  wxString desc;
  ft->GetDescription(&desc);

  wxString cmd;
  bool bOk = ft->GetOpenCommand(&cmd,
                               wxFileType::MessageParameters(m_userManual, wxEmptyString) );
  delete ft;
  ft = NULL;

  if (bOk == false)
  {
    wxMessageBox(wxString::Format("No command available (%s) to open '%s'",
                                  desc.c_str(),
                                  m_userManual.c_str()),
                 "Warning",
                  wxOK | wxCENTRE | wxICON_EXCLAMATION);

    return;
  }
  
  ::wxExecute(cmd);

};

//----------------------------------------
bool CBratGuiApp::SaveConfig(bool flush)
{
  bool bOk = true;
  if (m_config == NULL)
  {
    return bOk;
  }

  m_lastWksPath.Empty();

  CWorkspace* wks = GetCurrentWorkspace();

  if (wks != NULL)
  {
    m_lastWksPath = wks->GetPathName();
  }

  //------------------------------------
  m_config->SetPath("/" + GROUP_COMMON);
  //------------------------------------

  bOk &= m_config->Write(ENTRY_USER_MANUAL, m_userManual);
  if (!m_userManualViewer.IsEmpty())
  {
    bOk &= m_config->Write(ENTRY_USER_MANUAL_VIEWER, m_userManualViewer);
  }
  bOk &= m_config->Write(ENTRY_LAST_DATA_PATH, m_lastDataPath);
  bOk &= m_config->Write(ENTRY_LAST_PAGE_REACHED, m_lastPageReached);
  

  //------------------------------------
  m_config->SetPath("/" + GROUP_WKS);
  //------------------------------------
  bOk &= m_config->Write(ENTRY_LAST, m_lastWksPath);

  if (m_lastColorTable.IsEmpty() == false)
  {
    //------------------------------------
    m_config->SetPath("/" + GROUP_COLORTABLE);
    //------------------------------------
    bOk &= m_config->Write(ENTRY_LAST, m_lastColorTable);
  }

  bOk &= SaveConfigSelectionCriteria(false);

  if (flush)
  {
    m_config->Flush();
  }


  return bOk;

}

//----------------------------------------
bool CBratGuiApp::SaveConfigSelectionCriteria(bool flush)
{
  bool bOk = true;
  if (m_config == NULL)
  {
    return bOk;
  }


  wxString configPath;
  CMapProduct& mapProductInstance = CMapProduct::GetInstance();

  CObMap::iterator it;
  for (it = mapProductInstance.begin() ; it != mapProductInstance.end() ; it++)
  {
    CProduct* product = dynamic_cast<CProduct*>(it->second);
    if (product == NULL)
    {
      continue;
    }
    if (!product->HasCriteriaInfo())
    {
      continue;
    }

    configPath.Empty();
    configPath.Append("/");
    configPath.Append(GROUP_SEL_CRITERIA);
    configPath.Append(product->GetLabel().c_str());

    wxString value;

    //------------------------------------
    m_config->SetPath(configPath);
    //------------------------------------

    if (product->IsSetLatLonCriteria())
    {
      value = product->GetLatLonCriteria()->GetAsText().c_str();
      bOk &= m_config->Write(ENTRY_LAT_LON, value);
    }
    if (product->IsSetDatetimeCriteria())
    {
      value = product->GetDatetimeCriteria()->GetAsText().c_str();
      bOk &= m_config->Write(ENTRY_DATETIME, value);
    }
    if (product->IsSetCycleCriteria())
    {
      value = product->GetCycleCriteria()->GetAsText().c_str();
      bOk &= m_config->Write(ENTRY_CYCLE, value);
    }
    if (product->IsSetPassIntCriteria())
    {
      value = product->GetPassIntCriteria()->GetAsText().c_str();
      bOk &= m_config->Write(ENTRY_PASS_NUMBER, value);
    }
    if (product->IsSetPassStringCriteria())
    {
      value = product->GetPassStringCriteria()->GetAsText().c_str();
      bOk &= m_config->Write(ENTRY_PASS_STRING, value);
    }
  }

  return bOk;

}
//----------------------------------------
bool CBratGuiApp::LoadConfig()
{
  bool bOk = true;
  if (m_config == NULL)
  {
    return bOk;
  }

  m_config->SetPath("/" + GROUP_COMMON);

  bOk &= m_config->Read(ENTRY_USER_MANUAL, &m_userManual);
  bOk &= m_config->Read(ENTRY_USER_MANUAL_VIEWER, &m_userManualViewer);
  bOk &= m_config->Read(ENTRY_LAST_DATA_PATH, &m_lastDataPath);
  bOk &= m_config->Read(ENTRY_LAST_PAGE_REACHED, &m_lastPageReached, DATASETS_PAGE_NAME);

  m_config->SetPath("/" + GROUP_WKS);

  bOk &= m_config->Read(ENTRY_LAST, &m_lastWksPath);

  m_config->SetPath("/" + GROUP_COLORTABLE);

  bOk &= m_config->Read(ENTRY_LAST, &m_lastColorTable);

  bOk &= LoadConfigSelectionCriteria();

  return bOk;

}

//----------------------------------------
bool CBratGuiApp::LoadConfigSelectionCriteria()
{
  bool bOk = true;
  if (m_config == NULL)
  {
    return bOk;
  }


  try {

    CMapProduct& mapProductInstance = CMapProduct::GetInstance();
    mapProductInstance.AddCriteriaToProducts();

    wxString configPath;

    CObMap::iterator it;
    for (it = mapProductInstance.begin() ; it != mapProductInstance.end() ; it++)
    {
      CProduct* product = dynamic_cast<CProduct*>(it->second);
      if (product == NULL)
      {
        continue;
      }
      if (!product->HasCriteriaInfo())
      {
        continue;
      }

      configPath.Empty();
      configPath.Append("/");
      configPath.Append(GROUP_SEL_CRITERIA);
      configPath.Append(product->GetLabel().c_str());

      wxString value;

      //------------------------------------
      m_config->SetPath(configPath);
      //------------------------------------

      if (product->HasLatLonCriteria())
      {
        value = m_config->Read(ENTRY_LAT_LON);
        if (!value.IsEmpty())
        {
          product->GetLatLonCriteria()->Set((const char *)value.c_str());
        }
      }
      if (product->HasDatetimeCriteria())
      {
        value = m_config->Read(ENTRY_DATETIME);
        if (!value.IsEmpty())
        {
          product->GetDatetimeCriteria()->SetFromText((const char *)value.c_str());
        }
      }
      if (product->HasCycleCriteria())
      {
        value = m_config->Read(ENTRY_CYCLE);
        if (!value.IsEmpty())
        {
          product->GetCycleCriteria()->SetFromText((const char *)value.c_str());
        }
      }
      if (product->HasPassIntCriteria())
      {
        value = m_config->Read(ENTRY_PASS_NUMBER);
        if (!value.IsEmpty())
        {
          product->GetPassIntCriteria()->SetFromText((const char *)value.c_str());
        }
      }
      if (product->HasPassStringCriteria())
      {
        value = m_config->Read(ENTRY_PASS_STRING);
        if (!value.IsEmpty())
        {
          product->GetPassStringCriteria()->Set((const char *)value.c_str());
        }
      }
    }
  }
  catch(CException &e) {
    wxString msg = wxString::Format("An error occured while loading criteria selection configuration (CBratGuiApp::LoadConfigSelectionCriteria)\nNavive execption: %s", e.what());
    throw CException((const char *)msg.c_str(), e.error());
  }

  return bOk;

}
//----------------------------------------
void CBratGuiApp::CreateTree(CWorkspace* root)
{
  CreateTree(root, m_tree);
}
//----------------------------------------
void CBratGuiApp::CreateTree(CWorkspace* root, CTreeWorkspace& tree)
{
  wxFileName path;

  tree.DeleteTree();

  // Set tree root
  tree.SetRoot(root->GetName(), root, true);

  //WARNING : the sequence of workspaces object creation is significant, because of the interdependence of them

  //FIRSTLY - Create "Datasets" branch
  path = root->GetPath();
  path.AppendDir(WKS_DATASET_NAME);

  CWorkspaceDataset* wksDataSet = new CWorkspaceDataset(WKS_DATASET_NAME, path);

  tree.AddChild(wksDataSet->GetName(), wksDataSet);

  //SECOND Create "Formulas" branch
  path = root->GetPath();
  path.AppendDir(WKS_FORMULA_NAME);

  CWorkspaceFormula* wksFormula = new CWorkspaceFormula(WKS_FORMULA_NAME, path);

  tree.AddChild(wksFormula->GetName(), wksFormula);

  //THIRDLY - Create "Operations" branch
  path = root->GetPath();
  path.AppendDir(WKS_OPERATION_NAME);

  CWorkspaceOperation* wksOperation = new CWorkspaceOperation(WKS_OPERATION_NAME, path);

  tree.AddChild( wksOperation->GetName(), wksOperation);

  //FOURTHLY -  Create "Displays" branch
  path = root->GetPath();
  path.AppendDir(WKS_DISPLAY_NAME);

  CWorkspaceDisplay* wksDisplay = new CWorkspaceDisplay(WKS_DISPLAY_NAME, path);

  tree.AddChild(wksDisplay->GetName(), wksDisplay);

}

//----------------------------------------
wxBitmapType CBratGuiApp::GetIconType()
{
  //return wxBITMAP_TYPE_BMP;
  //return wxBITMAP_TYPE_JPEG;
  //return wxBITMAP_TYPE_GIF;
#ifdef WIN32
  return wxBITMAP_TYPE_ICO;
#else
  return wxBITMAP_TYPE_BMP;
#endif
}
  //----------------------------------------
wxString CBratGuiApp::GetIconFile()
{
  return CTools::FindDataFile((const char *)GetIconFileName().c_str()).c_str();
}
//----------------------------------------
wxString CBratGuiApp::GetIconFileName()
{
  return BRATHL_ICON_FILENAME.c_str();
}

//----------------------------------------
void CBratGuiApp::CStringArrayToWxArray(CStringArray& from, wxArrayString& to)
{
  CStringArray::iterator it;
  for (it = from.begin() ; it != from.end() ; it++)
  {
    to.Add((*it).c_str());
  }
}
//----------------------------------------
void CBratGuiApp::CStringListToWxArray(CStringList& from, wxArrayString& to)
{
  CStringList::iterator it;
  for (it = from.begin() ; it != from.end() ; it++)
  {
    to.Add((*it).c_str());
  }
}
//----------------------------------------
void CBratGuiApp::CProductListToWxArray(CProductList& from, wxArrayString& to)
{
  CProductList::iterator it;
  for (it = from.begin() ; it != from.end() ; it++)
  {
    to.Add((*it).c_str());
  }
}
//----------------------------------------
wxString CBratGuiApp::FindFile(const wxString& fileName )
{
  wxString fileNameFound;

  CDirTraverserExistFile traverserExistFile(fileName);

  wxFileName path;
  if (wxGetApp().GetExecPathName().IsEmpty())
  {

    wxString fileTmp = wxFileSelector(wxString::Format("Where is the file %s ?",  fileName.c_str()),
                                      wxGetCwd(),
                                      fileName);
    if (fileTmp.IsEmpty())
    {
      return "";
    }

    return fileTmp;
  }

  path.AssignDir(wxGetApp().GetExecPathName());

  wxArrayString dirs = path.GetDirs();

  for (int32_t i = dirs.GetCount() - 1 ; i > 0 ; i--)
  {
    path.RemoveDir(i);
  }

  wxDir dir;

  dir.Open(path.GetFullPath());
  dir.Traverse(traverserExistFile);

  if (traverserExistFile.m_found)
  {
    fileNameFound = traverserExistFile.m_fileName.GetFullPath();
  }
  return fileNameFound;
}

//----------------------------------------
void CBratGuiApp::GetDisplayType(COperation* operation, CUIntArray& displayTypes, CInternalFiles** pf /* = NULL */)
{
  displayTypes.RemoveAll();

  if (operation == NULL)
  {
    return;
  }
/*
  CInternalFiles *file = NULL;
  CInternalFilesYFX *yfx = NULL;
  CInternalFilesZFXY *zfxy = NULL;

  try
  {
    file = BuildExistingInternalFileKind(operation->GetOutputName().c_str());
    file->Open(ReadOnly);

  }
  catch (CException& e)
  {
    e.what();
    if (file != NULL)
    {
      delete file;
      file = NULL;
    }

    return -1;
  }

  switch (operation->GetType())
  {
    case CMapTypeOp::typeOpYFX :
      yfx = dynamic_cast<CInternalFilesYFX*>(file);
      break;
    case CMapTypeOp::typeOpZFXY :
      zfxy = dynamic_cast<CInternalFilesZFXY*>(file);
      break;
    default  :
      break;
  }

  int32_t ret = -1;

  if ( (yfx != NULL) || (zfxy != NULL) )
  {

    if (yfx != NULL)
    {
      ret = CMapTypeDisp::typeDispYFX;
    }
    else if (zfxy != NULL)
    {
      if (zfxy->IsGeographic())
      {
        ret = CMapTypeDisp::typeDispZFLatLon;
      }
      else
      {
        ret = CMapTypeDisp::typeDispZFXY;
      }
    }
  }

  if (file != NULL)
  {
    file->Close();
    delete file;
    file = NULL;
  }
*/

  CInternalFiles* f = CInternalFiles::Create((const char *)operation->GetOutputName().c_str(), true, false);

  if (CInternalFiles::IsYFXFile(f))
  {
    displayTypes.Insert(CMapTypeDisp::typeDispYFX);
  }

  if (CInternalFiles::IsZFLatLonFile(f))
  {
    displayTypes.Insert(CMapTypeDisp::typeDispZFLatLon);
  }

  if (CInternalFiles::IsZFXYFile(f))
  {
    displayTypes.Insert(CMapTypeDisp::typeDispZFXY);
  }

  if (pf == NULL)
  {
    CTools::DeleteObject(f);
    f = NULL;
  }
  else
  {
    *pf = f;
  }

}


//----------------------------------------
bool CBratGuiApp::IsNewViewEnable()
{
  bool enable = (wxGetApp().GetCurrentWorkspace() != NULL);
  CWorkspaceOperation* wksOp = wxGetApp().GetCurrentWorkspaceOperation();

  enable &= (wksOp != NULL);
  if (enable)
  {
    enable &= wksOp->HasOperation();
  }

  return enable;
}
//----------------------------------------
bool CBratGuiApp::IsDeleteViewEnable()
{
  return (wxGetApp().GetCurrentWorkspace() != NULL)
                && (GetGuiPanel()->GetDisplayPanel()->HasDisplay());
}
//----------------------------------------
bool CBratGuiApp::IsNewDatasetEnable()
{
  return (wxGetApp().GetCurrentWorkspace() != NULL);
}
//----------------------------------------
bool CBratGuiApp::IsDeleteDatasetEnable()
{
  return (wxGetApp().GetCurrentWorkspace() != NULL)
                && (GetGuiPanel()->GetDatasetPanel()->HasDataset());
}
//----------------------------------------
bool CBratGuiApp::IsNewOperationEnable()
{
  bool enable = (wxGetApp().GetCurrentWorkspace() != NULL);
  CWorkspaceDataset* wksDataset = wxGetApp().GetCurrentWorkspaceDataset();

  enable &= (wksDataset != NULL);
  if (enable)
  {
    enable &= wksDataset->HasDataset();
  }

  return enable;
}
//----------------------------------------
bool CBratGuiApp::IsDeleteOperationEnable()
{
  return (wxGetApp().GetCurrentWorkspace() != NULL)
                && (GetGuiPanel()->GetOperationPanel()->HasOperation());
}

//----------------------------------------
void CBratGuiApp::GetRecordNames( wxArrayString& array, CProduct* product )
{
  CStringArray arrayTmp;

  GetRecordNames( arrayTmp, product);

  CBratGuiApp::CStringArrayToWxArray(arrayTmp, array);

}
//----------------------------------------
void CBratGuiApp::GetRecordNames( CStringArray& array, CProduct* product )
{
  if (product == NULL)
  {
    return;
  }

  product->GetRecords(array);

}

//----------------------------------------
void CBratGuiApp::GetRecordNames(wxComboBox& combo, CProduct* product)
{
  CStringArray array;

  GetRecordNames( array, product );

  CStringArray::iterator it;

  for (it = array.begin() ; it != array.end() ; it++)
  {
    combo.Append( (*it).c_str() );
  }
}
//----------------------------------------
void CBratGuiApp::GetRecordNames(wxListBox& listBox, CProduct* product)
{
  CStringArray array;

  GetRecordNames( array, product );

  CStringArray::iterator it;

  for (it = array.begin() ; it != array.end() ; it++)
  {
    listBox.Append( (*it).c_str() );
  }
}
//----------------------------------------

void CBratGuiApp::EvtFocus(wxWindow& window,  int32_t eventType, const wxFocusEventFunction& method,
                                wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(eventType,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}

//----------------------------------------
void CBratGuiApp::EvtChar(wxWindow& window,  int32_t eventType, const wxCharEventFunction& method,
                                wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(eventType,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}
