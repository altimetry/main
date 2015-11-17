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

#ifndef __ProcessingPanel_H__
#define __ProcessingPanel_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "ProcessingPanel.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "SchedulerTaskConfig.h"

#include "BratScheduler_wdr.h"

#include "Process.h"

#include "TaskListView.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// CProcessingPanel
//----------------------------------------------------------------------------

class CProcessingPanel: public wxPanel
{
public:
  // constructors and destructors
  CProcessingPanel( wxWindow *parent, wxWindowID id = -1,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxTAB_TRAVERSAL | wxNO_BORDER );
    
  virtual ~CProcessingPanel();

  CProcess* GetProcess(const wxString& name);
  CProcess* GetProcess(CMapProcess::iterator it);
  bool HasProcess() {return m_processes.size() > 0;};
  
  wxString GetProcessNewName(const wxString& baseName);

  bool AddProcess(CBratTaskProcess *process, bool allowMultiple = false);

  void KillAllProcesses();
  void KillSelectedProcess();
  bool KillProcess(CProcess *process);
  
  bool RemoveProcess(const wxString& name);
  bool RemoveProcessAll(bool deleteProcess = true);

  void EnableCtrl();
  void EnableCtrl(bool enable);    
  
  // WDR: method declarations for CProcessingPanel
  wxButton* GetKillProcessing()  { return (wxButton*) FindWindow( ID_KILL_PROCESSING ); }
  CTaskListView* GetListProcessing()  { return (CTaskListView*) FindWindow( ID_LIST_PROCESSING ); }
    
private:
  CMapProcess m_processes;
  wxObArray m_processToRemove;
  wxObArray m_processKilledToRemove;

  // the idle event wake up timer
  wxTimer m_timerIdleWakeUp;

    // WDR: member variable declarations for CProcessingPanel
    
private:
  void InstallEventListeners();
  void DeInstallEventListeners();

    // WDR: handler declarations for CProcessingPanel
  void OnTaskDeselected( wxListEvent &event );
  void OnTaskSelected( wxListEvent &event );
  void OnKill( wxCommandEvent &event );
  void OnProcessTerm(CProcessTermEvent& event);
  void OnIdle(wxIdleEvent& event);
  void OnTimer(wxTimerEvent& event);
  void OnBratTaskProcess(CBratTaskProcessEvent& event);
  void OnDestroy( wxWindowDestroyEvent &event );

private:
    DECLARE_CLASS(CProcessingPanel)
    DECLARE_EVENT_TABLE()
};




#endif
