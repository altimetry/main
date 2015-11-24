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

// Temporary hack to prevent the inclusion of Windows XML 
// headers, which collide with xerces
//
#if defined (WIN32) || defined (_WIN32)
//avoid later inclusion of Microsoft XML stuff, which causes name collisions with xerces
#define DOMDocument MsDOMDocument
#include <msxml.h>
#include <urlmon.h>
#undef DOMDocument
#endif


#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "ProcessingPanel.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "SchedulerTaskConfig.h"

#include "BratSchedulerApp.h"
#include "ProcessingPanel.h"

// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// CProcessingPanel
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(CProcessingPanel,wxPanel)

// WDR: event table for CProcessingPanel

BEGIN_EVENT_TABLE(CProcessingPanel,wxPanel)
    EVT_WINDOW_DESTROY(CProcessingPanel::OnDestroy )
    EVT_IDLE(CProcessingPanel::OnIdle)
    EVT_TIMER(wxID_ANY, CProcessingPanel::OnTimer)
    EVT_BUTTON( ID_KILL_PROCESSING, CProcessingPanel::OnKill )
    EVT_LIST_ITEM_SELECTED( ID_LIST_PROCESSING, CProcessingPanel::OnTaskSelected )
    EVT_LIST_ITEM_DESELECTED( ID_LIST_PROCESSING, CProcessingPanel::OnTaskDeselected )
END_EVENT_TABLE()
//----------------------------------------
CProcessingPanel::CProcessingPanel( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxPanel( parent, id, position, size, style )
{

  m_timerIdleWakeUp.SetOwner(this);

  wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

  //----------------------------
  wxSizer* sizer = ProcessingListPanel( this, false, true );
  //----------------------------

  item0->Add( this, 1, wxGROW|wxALL, 5 );

  parent->SetSizer( item0 );

  InstallEventListeners();

}
//----------------------------------------
CProcessingPanel::~CProcessingPanel()
{
  RemoveProcessAll();
}
//----------------------------------------
void CProcessingPanel::InstallEventListeners()
{
  CProcess::EvtProcessTermCommand(*this,
                                (CProcessTermEventFunction)&CProcessingPanel::OnProcessTerm, NULL, this);


  CSchedulerTaskConfig::EvtBratTaskProcessCommand(*this,
                                 (CBratTaskProcessEventFunction)&CProcessingPanel::OnBratTaskProcess, NULL, this);
}
//----------------------------------------
void CProcessingPanel::DeInstallEventListeners()
{
  CProcess::DisconnectEvtProcessTermCommand(*this);
  CSchedulerTaskConfig::DisconnectEvtBratTaskProcessCommand(*this);
}

//----------------------------------------
void CProcessingPanel::OnDestroy( wxWindowDestroyEvent &event )
{
  DeInstallEventListeners();
}
//----------------------------------------
void CProcessingPanel::OnTimer(wxTimerEvent& event)
{
  wxWakeUpIdle();
}

//----------------------------------------
// input polling
void CProcessingPanel::OnIdle(wxIdleEvent& event)
{
  CMapProcess::iterator it;
  wxObArray processToRemove(false);
  CProcess* process = NULL;

  for (it = m_processes.begin() ; it != m_processes.end() ; it++)
  {
    process = GetProcess(it);
    if (process != NULL)
    {
      if ( (process->IsKilled()) || (process->IsEnded()) )
      {
        processToRemove.Insert(process);

        if ( process->IsKilled() )
        {
          //Under Linux: killed process objects have to be delete on exit application
          // Otherwise wxHandleProcessTermination crashes (invalid pointer)
          m_processKilledToRemove.Insert(process);
        }
        else
        {
          m_processToRemove.Insert(process);
        }
      }
      else if ( process->HasInput() )
      {
          event.RequestMore();
      }
    }
  }

  for (uint32_t i = 0 ; i < processToRemove.size() ; i ++)
  {
    process = dynamic_cast<CProcess*>(processToRemove[i]);
    RemoveProcess(process->GetName());
    //delete process;
    process = NULL;
  }

  m_processToRemove.RemoveAll();
}

//----------------------------------------
CProcess* CProcessingPanel::GetProcess(const wxString& name)
{
  return dynamic_cast<CProcess*>(m_processes.Exists((const char *)name.c_str()));
}
//----------------------------------------
CProcess* CProcessingPanel::GetProcess(CMapProcess::iterator it)
{
  return dynamic_cast<CProcess*>(it->second);
}

//----------------------------------------
wxString CProcessingPanel::GetProcessNewName(const wxString& baseName)
{
  int32_t i = 0;
  wxString key;

  do
  {
    key = wxString::Format(baseName +"_#%d", i + 1);
    i++;
  }
  while (m_processes.Exists((const char *)key.c_str()) != NULL);

  return key;
}


//----------------------------------------
bool CProcessingPanel::AddProcess(CBratTaskProcess *process, bool allowMultiple /* = false */)
{
  wxString msg;

  if (process == NULL)
  {
    return false;
  }

  bool bOk = true;
  if ( HasProcess() == false )
  {
    // we want to start getting the timer events to ensure that a
    // steady stream of idle events comes in -- otherwise we
    // wouldn't be able to poll the child process input
    m_timerIdleWakeUp.Start(100);
  }
  //else: the timer is already running

  wxString processName = process->GetName();

  try
  {
    if (m_processes.Exists((const char *)process->GetName().c_str()) != NULL)
    {
      if (allowMultiple)
      {
        processName = GetProcessNewName(processName);
        process->SetName(processName);
      }
      else
      {
        bOk = false;
        msg = wxString::Format("\n==> Unable to process task '%s'. A similar task is already running.<===\n",
                                process->GetName().c_str());
        wxLogWarning("%s",msg.c_str());
        return bOk;
      }
    }

    m_processes.Insert((const char *)processName.c_str(), process);

    bool isSync = ((process->GetExecuteFlags() & wxEXEC_SYNC) == wxEXEC_SYNC);
    //if (isSync)
    //{
    //  msg = wxString::Format("===> Synchronous Task '%s' started with command line below:<===\n'%s'",
    //                            process->GetName().c_str(),
    //                            process->GetCmd().c_str());
    //  wxLogInfo(msg);
    //}

    //----------------
    process->Execute();
    //----------------

    if (!isSync)
    {
    //  msg = wxString::Format("===> Asynchronous Task '%s' (pid %d) started with command line below:<===\n'%s'",
    //                            process->GetName().c_str(),
    //                            process->GetPid(),
    //                            process->GetCmd().c_str());
    //  wxLogInfo(msg);
      GetListProcessing()->InsertTask(process->GetBratTask());
    }


  }
  catch (CException& e)
  {
    bOk = false;
    msg = wxString::Format("==> Unable to process task '%s'.\nReason:\n'%s' <===",
                            process->GetName().c_str(),
                            e.what());
    wxLogError("%s",msg.c_str());
  }

  return bOk;

}
//----------------------------------------
void CProcessingPanel::KillAllProcesses()
{
  GetListProcessing()->SelectAll();
  KillSelectedProcess();
}
//----------------------------------------
void CProcessingPanel::KillSelectedProcess()
{
  GetKillProcessing()->Enable(false);

  int32_t selCount = GetListProcessing()->GetSelectedItemCount();

  long item = GetListProcessing()->GetNextItem(-1, wxLIST_NEXT_ALL,
                                         wxLIST_STATE_SELECTED);
  while ( item != -1 )
  {
    CBratTask* bratTask = (CBratTask*)(GetListProcessing()->GetItemData(item));
    if (bratTask != NULL)
    {
      // Process name is Brat Task uid
      CProcess* process = GetProcess(bratTask->GetUidAsString());
      if (process != NULL)
      {
        KillProcess(process);
      }
    }
    item = GetListProcessing()->GetNextItem(item, wxLIST_NEXT_ALL,
                                      wxLIST_STATE_SELECTED);
  }


  EnableCtrl();
}
//----------------------------------------
bool CProcessingPanel::KillProcess(CProcess *process)
{
  if (process == NULL)
  {
    return false;
  }
  int32_t result = process->Kill(wxSIGKILL);
  wxString msg;
  if (result == wxKILL_OK)
  {
    msg = wxString::Format("====> TASK '%s' KILLED <====",
                            process->GetName().c_str());
  }
  else
  {
    msg = wxString::Format("====> TASK '%s' (pid %ld): UNABLE TO KILL --> return code %d <====",
                            process->GetName().c_str(),
                            process->GetPid(),
                            result);
  }

  wxLogInfo("%s",msg.c_str());
  return result == wxKILL_OK;
}
//----------------------------------------
bool CProcessingPanel::RemoveProcess(const wxString& name)
{
  m_processes.Erase((const char *)name.c_str());
  if ( HasProcess() == false )
  {
      // we don't need to get idle events all the time any more
      m_timerIdleWakeUp.Stop();
  }
  return true;
}
//----------------------------------------
bool CProcessingPanel::RemoveProcessAll(bool deleteProcess)
{
  m_processes.SetDelete(deleteProcess);
  m_processes.RemoveAll();
  // we don't need to get idle events all the time any more
  m_timerIdleWakeUp.Stop();
  return true;
}
//----------------------------------------
void CProcessingPanel::EnableCtrl()
{
  bool enableKillTask = GetListProcessing()->GetSelectedItemCount() > 0;
  GetKillProcessing()->Enable(enableKillTask);
}
//----------------------------------------
void CProcessingPanel::EnableCtrl(bool enable)
{
  GetKillProcessing()->Enable(enable);
  GetListProcessing()->Enable(enable);
}

//----------------------------------------
void CProcessingPanel::OnBratTaskProcess(CBratTaskProcessEvent& event)
{
  CBratTask* bratTask = CSchedulerTaskConfig::GetInstance()->FindTaskFromMap(event.m_uid);

  if (bratTask == NULL)
  {
    wxString msg = wxString::Format("Unable to process task: task uid '%s' is not found.",
                                    bratTask->GetUidAsString().c_str());
    //wxMessageBox(msg,
    //            "Error",
    //            wxOK | wxICON_ERROR);

    wxLogError("%s",msg.c_str());

  }

  bratTask->SetStatus(CBratTask::e_BRAT_STATUS_PROCESSING);

  CBratTaskProcess* process = new CBratTaskProcess(bratTask,
                                                  this);


  this->AddProcess(process);


}

//----------------------------------------
void CProcessingPanel::OnProcessTerm(CProcessTermEvent& event)
{
  //wxString msg;
  //if (event.m_exitcode == BRATHL_SUCCESS)
  //{
  //  msg = wxString::Format("\n===> Task '%s' (pid %d) SUCCESSFULLY ENDED <===\n",
  //                                event.m_name.c_str(),
  //                                event.m_pid);
  //  wxLogInfo(msg);
  //}
  //else if (event.m_exitcode == BRATHL_WARNING)
  //{
  //  msg = wxString::Format("\n===> Task '%s' (pid %d) ENDED WITH WARNINGS (code %d) - See messages above <===\n",
  //                                event.m_name.c_str(),
  //                                event.m_pid,
  //                                event.m_exitcode);
  //  wxLogWarning(msg);
  //}
  //else
  //{
  //  msg = wxString::Format("\n===> Task '%s' (pid %d) UNSUCCESSFULLY ENDED (code %d) - See messages above <===\n",
  //                                event.m_name.c_str(),
  //                                event.m_pid,
  //                                event.m_exitcode);
  //  wxLogError(msg);
  //}

  wxPostEvent(&wxGetApp(), event);

}

//----------------------------------------
// WDR: handler implementations for CProcessingPanel

//----------------------------------------
void CProcessingPanel::OnTaskDeselected( wxListEvent &event )
{
  EnableCtrl();
  event.Skip();
}

//----------------------------------------
void CProcessingPanel::OnTaskSelected( wxListEvent &event )
{
  EnableCtrl();
  event.Skip();

}
//----------------------------------------
void CProcessingPanel::OnKill( wxCommandEvent &event )
{
  KillSelectedProcess();
  event.Skip();

}

//----------------------------------------



