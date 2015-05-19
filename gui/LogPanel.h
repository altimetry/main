/*
* Copyright (C) 2005 Collecte Localisation Satellites (CLS), France (31)
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
#ifndef __LogPanel_H__
#define __LogPanel_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "LogPanel.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "BratGui_wdr.h"

#include "Process.h"
#include "TaskList.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// CLogPanel
//----------------------------------------------------------------------------

class CLogPanel: public wxPanel
{
public:
  // constructors and destructors
  CLogPanel( wxWindow *parent, wxWindowID id = -1,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxTAB_TRAVERSAL | wxNO_BORDER );
  
  virtual ~CLogPanel();
  
  CProcess* GetProcess(const wxString& name);
  CProcess* GetProcess(CMapProcess::iterator it);
  bool HasProcess() {return m_processes.size() > 0;};
  
  wxString GetProcessNewName(const wxString& baseName);

  bool AddProcess(CProcess *process, bool allowMultiple = false);
  void KillSelectedProcess();
  bool KillProcess(CProcess *process);
  bool RemoveProcess(const wxString& name);
  bool RemoveProcessAll(bool deleteProcess = true);


  void EnableCtrl();

  void LogFile( wxFileName* file );
  void LogFile( const wxString& file );


  // WDR: method declarations for CLogPanel
    wxButton* GetTaskKill()  { return (wxButton*) FindWindow( ID_TASK_KILL ); }
    CTaskList* GetTasklist()  { return (CTaskList*) FindWindow( ID_TASKLIST ); }
  wxButton* GetLogClear()  { return (wxButton*) FindWindow( ID_LOG_CLEAR ); }
  wxTextCtrl* GetLogMess()  { return (wxTextCtrl*) FindWindow( ID_LOG_MESS ); }
    
private:
  CMapProcess m_processes;
  wxObArray m_processToRemove;

  // the idle event wake up timer
  wxTimer m_timerIdleWakeUp;

  // WDR: member variable declarations for CLogPanel
    
private:
  // WDR: handler declarations for CLogPanel
    void OnKillTask( wxCommandEvent &event );
    void OnTaskDeselected( wxListEvent &event );
    void OnTaskSelected( wxListEvent &event );
  void OnLogClear( wxCommandEvent &event );
  void OnProcessTerm(CProcessTermEvent& event);
  void OnIdle(wxIdleEvent& event);
  void OnTimer(wxTimerEvent& event);

private:
    DECLARE_CLASS(CLogPanel)
    DECLARE_EVENT_TABLE()
};




#endif
