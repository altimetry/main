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
    #pragma implementation "BratScheduler.h"
#endif

// For compilers that support precompilation
#include <wx/wxprec.h>


#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// Include private headers

#include <locale.h>

//#ifdef WIN32
//  #include <crtdbg.h>
//#endif

#include <wx/mimetype.h>
#include <wx/ffile.h>

#include "Trace.h"
#include "List.h"
#include "Tools.h"
#include "Exception.h"
using namespace brathl;

#include "BratScheduler.h"
#include "SchedulerTaskConfig.h"
#include "RichTextFrame.h"
#include "DirTraverser.h"


// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

#ifdef WIN32
const std::string BRATHL_ICON_FILENAME = "BratIcon.ico";
#else
const std::string BRATHL_ICON_FILENAME = "BratIcon.bmp";
#endif

const wxString BRATGUI_APPNAME = "BratGui";
const wxString GROUP_COMMON = "Common";
const wxString ENTRY_USER_MANUAL = "UserManual";
const wxString ENTRY_USER_MANUAL_VIEWER = "UserManualViewer";

const int32_t CBratSchedulerApp::m_SCHEDULER_TIMER_INTERVAL = 30000;
const int32_t CBratSchedulerApp::m_CHECK_CONFIGFILE_TIMER_INTERVAL = 5000;

// WDR: class implementations


//------------------------------------------------------------------------------
// MainApp
//------------------------------------------------------------------------------

IMPLEMENT_APP(CBratSchedulerApp)

BEGIN_EVENT_TABLE(CBratSchedulerApp,wxApp)
END_EVENT_TABLE()

CBratSchedulerApp::CBratSchedulerApp()
{
  m_configXml = NULL;
  m_checker = NULL;
  m_frame = NULL;
  m_configBratGui = NULL;

}
//----------------------------------------
void CBratSchedulerApp::DeleteChecker()
{
  if (m_checker != NULL)
  {
    delete m_checker;
    m_checker = NULL;
  }

}
//----------------------------------------
bool CBratSchedulerApp::OnInit()
{
  const wxString name = wxString::Format("%s-%s", wxGetApp().GetAppName().c_str(), wxGetUserId().c_str());
  m_checker = new wxSingleInstanceChecker(name);
  if ( m_checker->IsAnotherRunning() )
  {
    wxString msg = wxString::Format("%s (user %s) is already running", wxGetApp().GetAppName().c_str(), wxGetUserId().c_str());
    //wxLogError(msg.c_str());
    wxMessageBox(msg.c_str(),
            		 "Information",
		             wxOK | wxCENTRE | wxICON_INFORMATION);

    DeleteChecker(); // OnExit() won't be called if we return false

    return false;
  }

  bool bOk = true;

  // Set verbose to true to allow LogInfo messages
  wxLog::SetVerbose();

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


  if (appPath != "")
  {
    if (getenv(BRATHL_ENVVAR) == NULL)
    {
      CTools::SetDataDirForExecutable(wxGetApp().argv[0]);
    }
  }

  if (!CTools::DirectoryExists(CTools::GetDataDir()))
  {
      std::cerr << "ERROR: " << CTools::GetDataDir() << " is not a valid directory" << std::endl;
      ::wxMessageBox(wxString(CTools::GetDataDir().c_str()) + " is not a valid directory -- BRAT cannot continue. \n\nAre you sure your " + BRATHL_ENVVAR + " environment variable is set correctly?", "BRAT ERROR");

      DeleteChecker(); // OnExit() won't be called if we return false

      return false;
  }


  std::string errorMsg;
  if (!CTools::LoadAndCheckUdUnitsSystem(errorMsg))
  {
      std::cerr << errorMsg << std::endl;
      ::wxMessageBox(errorMsg.c_str(), "BRAT ERROR");

      DeleteChecker(); // OnExit() won't be called if we return false

      return false;
  }

  try 
  {
    LoadConfigBratGui();
  }
  catch(CException &e) 
  {
    std::cerr << errorMsg << std::endl;
    wxMessageBox(wxString::Format("An error occured while loading BratGui configuration (CBratSchedulerApp::LoadConfigBratGui)\nNavive exception: %s", e.what()),
		                               "Warning",
		                               wxOK | wxCENTRE | wxICON_EXCLAMATION);
    
  }

  //----------------
  // Install listeners
  //----------------
  InstallEventListeners();

  m_schedulerTimerId = ::wxNewId();
  m_checkConfigFileTimerId = ::wxNewId();

  try
  {
    m_frame = new CSchedulerFrame( NULL, -1, BRATSCHEDULER_TITLE);

    CSchedulerTaskConfig* schedulerTaskConfig = CSchedulerTaskConfig::GetInstance();

    // After loading, tasks whose status is 'in progress' are considered as 'pending'
    // They have to be re-executed.
    ChangeProcessingToPending();

    m_checkConfigFileTimer.SetFileName(schedulerTaskConfig->GetFullFileName());
    m_checkConfigFileTimer.SetOwner(this, m_checkConfigFileTimerId);

    m_schedulerTimer.SetOwner(this, m_schedulerTimerId);
  }
  catch(CException &e) {
    wxMessageBox(wxString::Format("An error occured while loading Brat Scheduler configuration\nNavive exception: %s", e.what()),
		 "Warning",
		 wxOK | wxCENTRE | wxICON_EXCLAMATION);
  }

  if (m_frame == NULL)
  {
    DeleteChecker();
    return false;
  }

  m_frame->Show( TRUE );

  try
  {
    StartSchedulerTimer();
    StartCheckConfigFileTimer();
  }
  catch(CException &e)
  {
    wxMessageBox(wxString(e.what()),
	            	 "Warning",
		             wxOK | wxCENTRE | wxICON_EXCLAMATION);
  }

  m_frame->SetTitle();

  return true;
}

//----------------------------------------
int CBratSchedulerApp::OnExit()
{
  DeleteConfigBratGui();

  DeInstallEventListeners();

  StopCheckConfigFileTimer();
  StopSchedulerTimer();

  DeleteChecker();

  return 0;
}

//----------------------------------------
void CBratSchedulerApp::InstallEventListeners()
{
  CProcess::EvtProcessTermCommand(*this,
                                (CProcessTermEventFunction)&CBratSchedulerApp::OnProcessTerm, NULL, this);

  CBratTask::EvtBratTaskProcessCommand(*this,
                                 (CBratTaskProcessEventFunction)&CBratSchedulerApp::OnBratTaskProcess, NULL, this);

  CCheckFileChange::EvtCheckFileChangeCommand(*this,
                                (CCheckFileChangeEventFunction)&CBratSchedulerApp::OnCheckFileChange, NULL, this);
}
//----------------------------------------
void CBratSchedulerApp::DeInstallEventListeners()
{
  CProcess::DisconnectEvtProcessTermCommand(*this);

  CBratTask::DisconnectEvtBratTaskProcessCommand(*this);

  CCheckFileChange::DisconnectEvtCheckFileChangeCommand(*this);
}
//----------------------------------------
void CBratSchedulerApp::StartSchedulerTimer()
{
  m_schedulerTimer.Start(CBratSchedulerApp::m_SCHEDULER_TIMER_INTERVAL);
}
//----------------------------------------
void CBratSchedulerApp::StopSchedulerTimer()
{
  m_schedulerTimer.Stop();
}
//----------------------------------------
void CBratSchedulerApp::StartCheckConfigFileTimer()
{
  m_checkConfigFileTimer.Start(CBratSchedulerApp::m_CHECK_CONFIGFILE_TIMER_INTERVAL);
}
//----------------------------------------
void CBratSchedulerApp::StopCheckConfigFileTimer()
{
  m_checkConfigFileTimer.Stop();
}
//----------------------------------------
bool CBratSchedulerApp::ChangeProcessingToPending()
{
  CVectorBratTask vectorTasks(false);
  bool somethingHasChanged = CSchedulerTaskConfig::GetInstance()->ChangeProcessingToPending(vectorTasks);
  if (!somethingHasChanged)
  {
    return somethingHasChanged;
  }

  CVectorBratTask::const_iterator itVector;

  for (itVector = vectorTasks.begin() ; itVector != vectorTasks.end() ; itVector++)
  {
    CBratTask* bratTask = *itVector;
    if (bratTask == NULL)
    {
      continue;
    }
  }

  GetPendingPanel()->LoadTasks();

  return somethingHasChanged;
}
//----------------------------------------
wxLog* CBratSchedulerApp::GetSchedulerLog()
{
  if (m_frame == NULL)
  {
    return NULL;
  }
  if (m_frame->GetSchedulerPanel() == NULL)
  {
    return NULL;
  }

  return m_frame->GetSchedulerPanel()->GetActiveLog();
}

//----------------------------------------
int32_t CBratSchedulerApp::GotoPage(int32_t pos)
{
  return GetMainnotebook()->SetSelection(pos);
}
//----------------------------------------
int32_t CBratSchedulerApp::GotoPage(const wxString& name)
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
wxString CBratSchedulerApp::GetCurrentPageText()
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
void CBratSchedulerApp::DetermineCharSize(wxWindow* wnd, int32_t& width, int32_t& height)
{
  wxStaticText w(wnd, -1, "W", wxDefaultPosition, wxSize(-1,-1));
  w.GetSize(&width, &height);
}
//----------------------------------------
wxBitmapType CBratSchedulerApp::GetIconType()
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
wxString CBratSchedulerApp::GetIconFile()
{
  return CTools::FindDataFile((const char *)GetIconFileName().c_str()).c_str();
}
//----------------------------------------
wxString CBratSchedulerApp::GetIconFileName()
{
  return BRATHL_ICON_FILENAME.c_str();
}


//----------------------------------------
wxString CBratSchedulerApp::FindFile(const wxString& fileName )
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
void CBratSchedulerApp::OnCheckFileChange(CCheckFileChangeEvent& event)
{
  bool bOk = true;

  try
  {
    //---------------------
    StopCheckConfigFileTimer();
    //---------------------

    GetPendingPanel()->EnableCtrl(false);

    m_checkConfigFileTimer.SetFileDirty(false);

    CSchedulerTaskConfig::LoadSchedulerTaskConfig(true);
    UnLockConfigFile();
  }
  catch(CException& e)
  {
    UnLockConfigFile();
    bOk = false;
    wxLogError("%s",e.what());

  }

  GetPendingPanel()->EnableCtrl(true);

  //---------------------
  StartCheckConfigFileTimer();
  //---------------------

  if (!bOk)
  {
    return;
  }

  CCheckFileChangeEvent ev(wxID_ANY, event.m_checkFileChange);
  wxPostEvent(this->GetPendingPanel(), ev);
}
//----------------------------------------
void CBratSchedulerApp::OnBratTaskProcess(CBratTaskProcessEvent& event)
{
  bool bOk = true;

  try
  {
    //---------------------
    StopCheckConfigFileTimer();
    //---------------------

    GetPendingPanel()->EnableCtrl(false);
    GetProcessingPanel()->EnableCtrl(false);

    LoadSchedulerTaskConfig(true);

    GetListPending()->RemoveTasksFromList(event.m_uid);
    CSchedulerTaskConfig::GetInstance()->ChangeTaskStatus(event.m_uid, CBratTask::BRAT_STATUS_PROCESSING);

    bool saved = CSchedulerTaskConfig::SaveSchedulerTaskConfig(true);

    ResetConfigFileChange();
  }
  catch(CException& e)
  {
    UnLockConfigFile();
    bOk = false;
    wxLogError("%s",e.what());

  }

  GetProcessingPanel()->EnableCtrl(true);
  GetPendingPanel()->EnableCtrl(true);

  //---------------------
  StartCheckConfigFileTimer();
  //---------------------

  if (!bOk)
  {
    return;
  }

  CBratTaskProcessEvent ev(wxID_ANY, event.m_uid);
  wxPostEvent(this->GetProcessingPanel(), ev);

}

//----------------------------------------
void CBratSchedulerApp::OnProcessTerm(CProcessTermEvent& event)
{
  bool bOk = true;

  try
  {
    //---------------------
    StopCheckConfigFileTimer();
    //---------------------

    GetProcessingPanel()->EnableCtrl(false);
    GetEndedPanel()->EnableCtrl(false);

    LoadSchedulerTaskConfig(true);

    GetListProcessing()->RemoveTasksFromList(event.m_taskUid);
    CBratTask::bratTaskStatus bratTaskStatus = CBratTask::BRAT_STATUS_ERROR;

    if (event.m_exitcode == BRATHL_SUCCESS)
    {
      bratTaskStatus = CBratTask::BRAT_STATUS_ENDED;
    }
    else if (event.m_exitcode == BRATHL_WARNING)
    {
      bratTaskStatus = CBratTask::BRAT_STATUS_WARNING;
    }

    CSchedulerTaskConfig::GetInstance()->ChangeTaskStatus(event.m_taskUid, bratTaskStatus);

    bool saved = CSchedulerTaskConfig::SaveSchedulerTaskConfig(true);

    ResetConfigFileChange();

    CBratTask* bratTask = CSchedulerTaskConfig::GetInstance()->FindTaskFromMap(event.m_taskUid);

    if (bratTask == NULL)
    {
      wxString msg = wxString::Format("Unable to add task to '%s' task list: task uid '%s' is not found.",
                                      ENDED_PAGE_NAME.c_str(),
                                      bratTask->GetUidValueAsString().c_str());
      wxLogError("%s",msg.c_str());

    }

    GetListEnded()->InsertTask(bratTask);

  }
  catch(CException& e)
  {
    UnLockConfigFile();
    bOk = false;
    wxLogError("%s",e.what());

  }

  GetProcessingPanel()->EnableCtrl(true);
  GetEndedPanel()->EnableCtrl(true);

  //---------------------
  StartCheckConfigFileTimer();
  //---------------------

  if (!bOk)
  {
    return;
  }

}

//----------------------------------------
void CBratSchedulerApp::LoadSchedulerTaskConfig(bool quiet /* = false */)
{
  CSchedulerTaskConfig::LoadSchedulerTaskConfig(true);

  if (CSchedulerTaskConfig::GetInstance()->HasMapNewBratTask())
  {
    CCheckFileChangeEvent ev(wxID_ANY);
    wxPostEvent(this->GetPendingPanel(), ev);
  }

}
//----------------------------------------
void CBratSchedulerApp::UnLockConfigFile()
{

  CSchedulerTaskConfig::ForceUnLockConfigFile();

}
//----------------------------------------
void CBratSchedulerApp::AskUserToRestartApplication()
{
  wxString msg = "An unrecoverable error has occurred.\nYou have to restart application (exit then start again).\n"
                 "Sorry for the inconvenience";

    wxMessageBox(msg.c_str(),
		              "Warning",
		              wxOK | wxCENTRE | wxICON_EXCLAMATION);
}
//----------------------------------------
void CBratSchedulerApp::ResetConfigFileChange()
{
  m_checkConfigFileTimer.GetCheckFileChange()->SetFileDirty(false);
  m_checkConfigFileTimer.GetCheckFileChange()->GetFileCurrentMod();
  m_checkConfigFileTimer.GetCheckFileChange()->SetFileDirty(false);
}
//----------------------------------------
void CBratSchedulerApp::ViewFile(const wxString& fname, wxWindow* parent)
{

  wxString ext = fname.AfterLast(_T('.'));
  wxFileType *ft = wxTheMimeTypesManager->GetFileTypeFromExtension(ext);
  if ( ft == NULL )
  {
    ViewFileGeneric(fname, parent);
    return;
  }

  wxString desc;
  ft->GetDescription(&desc);

  wxString cmd;
  bool bOk = ft->GetOpenCommand(&cmd,
                               wxFileType::MessageParameters(fname, wxEmptyString) );
  delete ft;
  ft = NULL;

  if (bOk == false)
  {
    ViewFileGeneric(fname, parent);
    return;
  }

  ::wxExecute(cmd);
}
//----------------------------------------
void CBratSchedulerApp::ViewFileGeneric(const wxString& fname, wxWindow* parent)
{
  CRichTextFrame* frame = new CRichTextFrame(parent, fname);



  wxString content;
  wxFFile wxffile(fname);
  wxffile.ReadAll(&content);

  frame->GetTextCtrl()->SetValue(content);

  frame->Show(true);
  frame->GetTextCtrl()->SetInsertionPoint(0);
  frame->GetTextCtrl()->ShowPosition(0);


}

//----------------------------------------
wxString CBratSchedulerApp::BuildUserManualPath()
{
   wxFileName fileName;

  fileName.AssignDir(wxGetApp().GetExecPathName());
  fileName.AppendDir(BRAT_DOC_SUBDIR);
  fileName.SetFullName(BRAT_USER_MANUAL);

  return fileName.GetFullPath();
}
//----------------------------------------
void CBratSchedulerApp::ViewUserManual()
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

}
//----------------------------------------
void CBratSchedulerApp::DeleteConfigBratGui()
{
  if (m_configBratGui != NULL)
  {
    delete m_configBratGui;
    m_configBratGui = NULL;
  }
}

//----------------------------------------
bool CBratSchedulerApp::LoadConfigBratGui()
{
  DeleteConfigBratGui();

  m_configBratGui = new wxFileConfig(BRATGUI_APPNAME, wxEmptyString, wxEmptyString, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);

  bool bOk = true;
  if (m_configBratGui == NULL)
  {
    return bOk;
  }

  m_configBratGui->SetPath("/" + GROUP_COMMON);

  bOk &= m_configBratGui->Read(ENTRY_USER_MANUAL, &m_userManual);
  bOk &= m_configBratGui->Read(ENTRY_USER_MANUAL_VIEWER, &m_userManualViewer);


  DeleteConfigBratGui();

  return bOk;

}
//----------------------------------------
void CBratSchedulerApp::EvtFocus(wxWindow& window,  int32_t eventType, const wxFocusEventFunction& method,
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
void CBratSchedulerApp::EvtChar(wxWindow& window,  int32_t eventType, const wxCharEventFunction& method,
                                wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(eventType,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}
