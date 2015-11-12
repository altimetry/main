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
    #pragma implementation "SchedulerFrame.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif


#include "BratSchedulerApp.h"
#include "SchedulerFrame.h"
#include "SchedulerTaskConfig.h"

// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// CSchedulerFrame
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(CSchedulerFrame,wxFrame)

// WDR: event table for CSchedulerFrame

BEGIN_EVENT_TABLE(CSchedulerFrame,wxFrame)
    EVT_MENU( wxID_EXIT, CSchedulerFrame::OnQuit )
    EVT_MENU( wxID_ABOUT, CSchedulerFrame::OnAbout )
    EVT_CLOSE(CSchedulerFrame::OnCloseWindow)
    EVT_MENU( ID_HELP_USER_MANUEL, CSchedulerFrame::OnUserManual )
    EVT_MENU( ID_VIEW_XML_CONFIG, CSchedulerFrame::OnViewConfiguration )
END_EVENT_TABLE()

//----------------------------------------
CSchedulerFrame::CSchedulerFrame( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxFrame( parent, id, title, position, size, style )
{
  m_menuBar = NULL;

  CreateMainMenuBar();
    
  CreateStatusBar(1);
    
  m_schedulerPanel = new CSchedulerPanel(this, -1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER );
  wxBoxSizer * sizer = new wxBoxSizer(wxVERTICAL);

  sizer->Add(m_schedulerPanel, 1, wxEXPAND);
  SetSizer(sizer);

  // insert main window here
  //MainFrameContentFunc(this);

  SetMinSize(GetBestSize());
  SetSize(GetBestSize());

  try
  {
    if (!wxGetApp().GetIconFile().IsEmpty())
    {
      SetIcon(wxIcon(wxGetApp().GetIconFile(), ::wxGetApp().GetIconType()));
    }
    else
    {
      ::wxMessageBox(wxString::Format("WARNING: Unable to find Brat icon file %s",
                                      ::wxGetApp().GetIconFileName().c_str())
                     , "BRAT WARNING");
    }
  }
  catch(...)
  {
    // Do nothing
  }

  //CSchedulerTaskConfig* sched =  CSchedulerTaskConfig::GetInstance();
  //sched->LockConfigFile("C:\\Documents and Settings\\dearith\\Application Data\\BratInternalTests\\BratSchedulerTasksConfig.xml");

  //m_schedulerTimerId = ::wxNewId();
  //m_checkConfigFileTimerId = ::wxNewId();

  //m_checkConfigFileTimer.SetOwner(this, m_checkConfigFileTimerId);
  //m_schedulerTimer.SetOwner(this, m_schedulerTimerId);
  //m_schedulerTimer.Start(45000);
  //m_schedulerTimer.Start(30000);

  //sched->UnLockConfigFile();


}

//----------------------------------------
CSchedulerFrame::~CSchedulerFrame()
{
}

//----------------------------------------
void CSchedulerFrame::CreateMainMenuBar()
{
  m_menuBar = MainMenuBarFunc();

  SetMenuBar( m_menuBar );

}
//----------------------------------------
//bool CSchedulerFrame::LoadTasks()
//{
//  return true;
//}

// WDR: handler implementations for CSchedulerFrame
//----------------------------------------
void CSchedulerFrame::OnViewConfiguration( wxCommandEvent &event )
{
  wxString fname = CSchedulerTaskConfig::GetConfigFilePath(CSchedulerTaskConfig::m_CONFIG_APPNAME);
  CBratSchedulerApp::ViewFile(fname, this);
}


//----------------------------------------
void CSchedulerFrame::OnUserManual( wxCommandEvent &event )
{
  wxGetApp().ViewUserManual();
}

//----------------------------------------

#if defined(_WIN64) || defined(__LP64__) || defined(__x86_64__) 
	#define PROCESSOR_ARCH wxT("64 bit") 
#else
	#define PROCESSOR_ARCH wxT("32 bit") 
#endif

void CSchedulerFrame::OnAbout( wxCommandEvent &event )
{
  wxMessageDialog dialog( this, wxT("Welcome to BRAT ") wxT(BRAT_VERSION) wxT(" - ") PROCESSOR_ARCH wxT("\n(C)opyright ESA"),
      wxT("About BRAT"), wxOK|wxICON_INFORMATION );
  dialog.ShowModal();    
}

//----------------------------------------
void CSchedulerFrame::OnQuit( wxCommandEvent &event )
{
  Close();    
}
//----------------------------------------
void CSchedulerFrame::OnCloseWindow( wxCloseEvent &event )
{
  //GetFieldstreectrl()->DeInstallEventListeners();
  
  bool cancel = AskToExit();

  if (cancel)
  {
    event.Veto();
    return;
  }

  // if ! saved changes -> return
  Destroy();
}
//----------------------------------------
void CSchedulerFrame::SetTitle()
{
  wxString title = BRATSCHEDULER_TITLE;

  wxTopLevelWindow::SetTitle(title);
}
//----------------------------------------
bool CSchedulerFrame::AskToExit()
{
  bool cancel = false;

  //---------------------
  wxGetApp().StopSchedulerTimer();
  wxGetApp().StopCheckConfigFileTimer();
  //---------------------
  
  wxString msg;
  // Check if processes are running
  if (GetProcessingPanel()->HasProcess())
  {
    msg = "Some tasks are running. Are you sure you want to exit application ?\n"
          "Click Yes to kill running tasks and exit.\n"
          "Click No to continue.\n";
  }
  else
  {
    msg = "Do you want to exit application ?\n"
           "Click Yes to exit\n"
           "Click No to continue\n";
  }

  int32_t result = wxMessageBox(msg,
                                "Question",
                                 wxYES_NO | wxCENTRE | wxICON_QUESTION);

  cancel = (result == wxNO);

  if (cancel)
  {
    //---------------------
    wxGetApp().StartSchedulerTimer();
    wxGetApp().StartCheckConfigFileTimer();
    //---------------------
    return cancel;
  }

  GetProcessingPanel()->KillAllProcesses();

  return cancel;
}
//----------------------------------------




