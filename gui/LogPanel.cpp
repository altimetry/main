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
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "LogPanel.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#include <wx/ffile.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "Exception.h"
using namespace brathl;

#include "BratGui.h"
#include "LogPanel.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// CLogPanel
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(CLogPanel,wxPanel)

// WDR: event table for CLogPanel

BEGIN_EVENT_TABLE(CLogPanel,wxPanel)
    EVT_BUTTON( ID_LOG_CLEAR, CLogPanel::OnLogClear )
    EVT_IDLE(CLogPanel::OnIdle)
    EVT_TIMER(wxID_ANY, CLogPanel::OnTimer)
    EVT_LIST_ITEM_SELECTED( ID_TASKLIST, CLogPanel::OnTaskSelected )
    EVT_LIST_ITEM_DESELECTED( ID_TASKLIST, CLogPanel::OnTaskDeselected )
    EVT_BUTTON( ID_TASK_KILL, CLogPanel::OnKillTask )
END_EVENT_TABLE()

//----------------------------------------
CLogPanel::CLogPanel( wxWindow *parent, wxWindowID id,
                      const wxPoint &position, const wxSize& size, long style )
                  : wxPanel( parent, id, position, size, style )

{
  m_timerIdleWakeUp.SetOwner(this);

  wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

  LogInfoPanel(this, false, true);

  item0->Add( this, 0, wxGROW|wxALL, 5 );
  parent->SetSizer( item0 );

  CProcess::EvtProcessTermCommand(*this,
                                 (CProcessTermEventFunction)&CLogPanel::OnProcessTerm, NULL, this);

  EnableCtrl();
}

//----------------------------------------
CLogPanel::~CLogPanel()
{
  RemoveProcessAll();
}

//----------------------------------------
CProcess* CLogPanel::GetProcess(const wxString& name)
{
  return dynamic_cast<CProcess*>(m_processes.Exists((const char *)name.c_str()));
}
//----------------------------------------
CProcess* CLogPanel::GetProcess(CMapProcess::iterator it)
{
  return dynamic_cast<CProcess*>(it->second);
}

//----------------------------------------
wxString CLogPanel::GetProcessNewName(const wxString& baseName)
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
bool CLogPanel::AddProcess(CProcess *process, bool allowMultiple /* = false */)
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
        GetLogMess()->AppendText(msg);
        wxMessageBox(msg,
                    "Warning",
                    wxOK | wxICON_EXCLAMATION);
        return bOk;
      }
    }

    m_processes.Insert((const char *)processName.c_str(), process);

    bool isSync = ((process->GetExecuteFlags() & wxEXEC_SYNC) == wxEXEC_SYNC);
    if (isSync)
    {
      msg = wxString::Format("\n\n===> Synchronous Task '%s' started with command line below:<===\n'%s'\n\n"
                             "\n ==========> Please wait.... A report will display at the end of the task <==========\n\n",
                                process->GetName().c_str(),
                                process->GetCmd().c_str());
      GetLogMess()->AppendText(msg);
      GetTasklist()->InsertProcess(process);
    }

    //----------------
    process->Execute();
    //----------------

    if (!isSync)
    {

      msg = wxString::Format("\n\n===> Asynchronous Task '%s' (pid %d) started with command line below:<===\n'%s'\n\n",
                                process->GetName().c_str(),
                                process->GetPid(),
                                process->GetCmd().c_str());
      GetLogMess()->AppendText(msg);
      GetTasklist()->InsertProcess(process);
    }


  }
  catch (CException& e)
  {
    bOk = false;
    msg = wxString::Format("\n==> Unable to process task '%s'\na similar task is already running\n\nReason:\n'%s' <===\n",
                            process->GetName().c_str(),
                            e.what());
    GetLogMess()->AppendText(msg);
    wxMessageBox(msg,
                "Warning",
                wxOK | wxICON_EXCLAMATION);
  }

  return bOk;

}
//----------------------------------------
void CLogPanel::KillSelectedProcess()
{
  GetTaskKill()->Enable(false);

  int32_t selCount = GetTasklist()->GetSelectedItemCount();

  long item = GetTasklist()->GetNextItem(-1, wxLIST_NEXT_ALL,
                                         wxLIST_STATE_SELECTED);
  while ( item != -1 )
  {
    CProcess* process = (CProcess*)(GetTasklist()->GetItemData(item));
    KillProcess(process);
    item = GetTasklist()->GetNextItem(item, wxLIST_NEXT_ALL,
                                      wxLIST_STATE_SELECTED);
  }


  EnableCtrl();
}
//----------------------------------------
bool CLogPanel::KillProcess(CProcess *process)
{
  if (process == NULL)
  {
    return false;
  }
  int32_t result = process->Kill(wxSIGKILL);
  wxString msg;
  if (result == wxKILL_OK)
  {
    msg = wxString::Format("\n======================\nTask '%s' KILLED \n======================\n",
                            process->GetName().c_str());
  }
  else
  {
    msg = wxString::Format("\n======================\nTask '%s' (pid %d): UNABLE TO KILL --> return code %d\n======================\n",
                            process->GetName().c_str(),
                            process->GetPid(),
                            result);
  }

  GetLogMess()->AppendText(msg);
  return result == wxKILL_OK;
}
//----------------------------------------
bool CLogPanel::RemoveProcess(const wxString& name)
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
bool CLogPanel::RemoveProcessAll(bool deleteProcess)
{
  m_processes.SetDelete(deleteProcess);
  m_processes.RemoveAll();
  // we don't need to get idle events all the time any more
  m_timerIdleWakeUp.Stop();
  return true;
}
//----------------------------------------
void CLogPanel::EnableCtrl()
{
  bool enableKillTask = GetTasklist()->GetSelectedItemCount() > 0;
  GetTaskKill()->Enable(enableKillTask);
}

//----------------------------------------

// WDR: handler implementations for CLogPanel

//----------------------------------------
void CLogPanel::OnKillTask( wxCommandEvent &event )
{
  KillSelectedProcess();
  event.Skip();
}

//----------------------------------------
void CLogPanel::OnTaskDeselected( wxListEvent &event )
{
  EnableCtrl();
  event.Skip();
}

//----------------------------------------
void CLogPanel::OnTaskSelected( wxListEvent &event )
{
  EnableCtrl();
  event.Skip();

}
//----------------------------------------
void CLogPanel::OnTimer(wxTimerEvent& event)
{
  wxWakeUpIdle();
}
//----------------------------------------
// input polling
void CLogPanel::OnIdle(wxIdleEvent& event)
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
        GetTasklist()->RemoveProcess(process);
        processToRemove.Insert(process);
        m_processToRemove.Insert(process);
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
}
//----------------------------------------
void CLogPanel::OnProcessTerm(CProcessTermEvent& event)
{
  wxString msg;
  if (event.m_exitcode == BRATHL_SUCCESS)
  {
    msg = wxString::Format("\n===> Task '%s' (pid %d) SUCCESSFULLY ENDED <===\n",
                                  event.m_name.c_str(),
                                  event.m_pid);

    if (wxGetApp().IsCurrentLogPage() && (event.m_type >= 0))
    {
      wxGetApp().GotoLastPageReached();
    }
  }
  else if (event.m_exitcode == BRATHL_WARNING)
  {
    msg = wxString::Format("\n===> Task '%s' (pid %d) ENDED WITH WARNINGS (code %d) - See messages above <===\n",
                                  event.m_name.c_str(),
                                  event.m_pid,
                                  event.m_exitcode);
    if (! wxGetApp().IsCurrentLogPage())
    {
      wxGetApp().GotoPage(LOG_PAGE_NAME);
    }
  }
  else
  {
    msg = wxString::Format("\n===> Task '%s' (pid %d) UNSUCCESSFULLY ENDED (code %d) - See messages above <===\n",
                                  event.m_name.c_str(),
                                  event.m_pid,
                                  event.m_exitcode);
    if (! wxGetApp().IsCurrentLogPage())
    {
      wxGetApp().GotoPage(LOG_PAGE_NAME);
    }
  }

  GetLogMess()->AppendText(msg);

  if (event.m_type != -1)
  {
    //wxPostEvent(wxGetApp().GetDisplayPanel(), event);
    wxPostEvent(wxGetApp().GetDisplayPanel(), event);
    wxPostEvent(wxGetApp().GetOperationPanel(), event);
  }

}

//----------------------------------------
void CLogPanel::OnLogClear( wxCommandEvent &event )
{
  GetLogMess()->Clear();
}
//----------------------------------------
void CLogPanel::LogFile( wxFileName* file )
{
  if (file == NULL)
  {
    return;
  }

  if (!file->IsOk())
  {
    return;
  }

  LogFile(file->GetFullPath());

}

//----------------------------------------
void CLogPanel::LogFile( const wxString& file )
{
  if (! wxFileExists(file))
  {
    return;
  }

  wxFFile wxffile(file);

  wxString content;
  wxffile.ReadAll(&content);

  GetLogMess()->AppendText(content);

}





