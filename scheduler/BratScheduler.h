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


#ifndef __BratScheduler_H__
#define __BratScheduler_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "BratScheduler.h"
#endif

// Include wxWindows' headers

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "wx/filename.h"
#include "wx/config.h"  // (to let wxWidgets choose a wxConfig class for your platform)
#include "wx/confbase.h"  // (base config class)
#include "wx/fileconf.h" // (wxFileConfig class)
#include "wx/xml/xml.h" // (wxXmlDocument, wxXmlNode, wxXmlProperty classes)
#include "wx/snglinst.h" // (wxSingleInstanceChecker class)

//#include "wx/msw/regconf.h" // (wxRegConfig class)

#include "List.h"

using namespace brathl;

//#include "BratScheduler_wdr.h"
#include "SchedulerTimer.h"
#include "SchedulerFrame.h"

const wxString BRATSCHEDULER_TITLE = "Brat Task Scheduler";
const wxString BRAT_USER_MANUAL = wxString::Format("brat_user_manual_%s.pdf", BRAT_VERSION);
const wxString BRAT_DOC_SUBDIR = "doc";

const wxString PENDING_PAGE_NAME = "Pending Tasks";
const wxString PROCESSING_PAGE_NAME = "Processing tasks";
const wxString ENDED_PAGE_NAME = "Ended Task";
const wxString LOG_PAGE_NAME = "Logs";


// WDR: class declarations

//----------------------------------------------------------------------------
// BratApp
//----------------------------------------------------------------------------

class CBratSchedulerApp: public wxApp
{
public:
  CBratSchedulerApp();
  
  virtual bool OnInit();
  virtual int OnExit();

  static void DetermineCharSize(wxWindow* wnd, int32_t& width, int32_t& height);

  wxBitmapType GetIconType();
  wxString GetIconFile();
  wxString GetIconFileName();

  wxNotebook* GetMainnotebook()  { return  m_frame->GetMainnotebook(); };
  CSchedulerFrame* GetSchedulerFrame()  { return  m_frame; };
  CSchedulerPanel* GetSchedulerPanel()  { return  m_frame->GetSchedulerPanel(); };

  CProcessingPanel* GetProcessingPanel()  { return  m_frame->GetProcessingPanel(); };
  CLogPanel* GetLogPanel()  { return  m_frame->GetLogPanel(); };
  CPendingPanel* GetPendingPanel()  { return  m_frame->GetPendingPanel(); };
  CEndedPanel* GetEndedPanel()  { return  m_frame->GetEndedPanel(); };
  
  CTaskListView* GetListPending()  { return  GetPendingPanel()->GetListPending(); };
  CTaskListView* GetListProcessing()  { return  GetProcessingPanel()->GetListProcessing(); };
  CTaskListView* GetListEnded()  { return  GetEndedPanel()->GetListEnded(); };

  wxLog* GetSchedulerLog();

  bool LoadTasks();

  void StartSchedulerTimer();
  void StopSchedulerTimer();

  void StartCheckConfigFileTimer();
  void StopCheckConfigFileTimer();
  
  void LoadSchedulerTaskConfig(bool quiet = false);
  
  void UnLockConfigFile();

  void ResetConfigFileChange();
  
  void AskUserToRestartApplication();

  wxString BuildUserManualPath();
  void ViewUserManual();

  wxString GetUserManual() {return m_userManual;};
  void SetUserManual(const wxString& value) {m_userManual = value;};

  int32_t GotoPage(int32_t pos);
  int32_t GotoPage(const wxString& name);
  
  wxString GetCurrentPageText();

  wxFileName* GetExecName() {return &m_execName;};
  
  wxString GetExecPathName(int32_t flags = wxPATH_GET_VOLUME, wxPathFormat format = wxPATH_NATIVE) 
#ifdef __WXMAC__
                        {return (m_execName.IsRelative() ? 
                                 m_execName.GetPath(flags, format) + "/BratScheduler.app/Contents/MacOS" :
                                 m_execName.GetPath(flags, format));};
#else
                        {return m_execName.GetPath(flags, format);};
#endif
  
  static wxString FindFile(const wxString& fileName );

  static void ViewFile(const wxString& fname, wxWindow* parent);
  static void ViewFileGeneric(const wxString& fname, wxWindow* parent);
  
  //static void ViewUserManual( const wxString& fileName);

  static void EvtFocus(wxWindow& window,  int32_t eventType, const wxFocusEventFunction& method,
                        wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
  static void EvtChar(wxWindow& window,  int32_t eventType, const wxCharEventFunction& method,
                        wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);


public:
  static const int32_t m_SCHEDULER_TIMER_INTERVAL;
  static const int32_t m_CHECK_CONFIGFILE_TIMER_INTERVAL;

protected:

  wxSingleInstanceChecker* m_checker;

  wxXmlDocument* m_configXml;

  CSchedulerFrame* m_frame;

  CSchedulerTimer m_schedulerTimer;
  CCheckFileChangeTimer m_checkConfigFileTimer;

  long m_schedulerTimerId;
  long m_checkConfigFileTimerId;

  wxFileName m_execName;

  wxString m_userManualViewer;
  wxString m_userManual;

  wxFileConfig* m_configBratGui;

protected:
  
  void DeleteChecker();
  bool ChangeProcessingToPending();

  void InstallEventListeners();
  void DeInstallEventListeners();

  void OnBratTaskProcess(CBratTaskProcessEvent& event);
  void OnProcessTerm(CProcessTermEvent& event);
  void OnCheckFileChange(CCheckFileChangeEvent& event);

  bool LoadConfigBratGui();
  void DeleteConfigBratGui();

  DECLARE_EVENT_TABLE()

};

DECLARE_APP(CBratSchedulerApp)

#endif
