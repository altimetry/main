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

#include "Process.h"

#include "new-gui/scheduler/BratTask.h"

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/txtstrm.h"
#include "wx/wfstream.h"
#include "wx/thread.h"
#include "wx/numdlg.h"

// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

DEFINE_EVENT_TYPE(wxEVT_PROCESS_TERM)


// WDR: class implementations

//-------------------------------------------------------------
//------------------- CProcess class --------------------
//-------------------------------------------------------------
CProcess::CProcess(const wxString& name, wxWindow *parent, const wxString& cmd, const std::string *output, int32_t type)
    : wxProcess(parent), m_cmd(cmd)
{
  m_name = name;
  m_parent = parent;
  if (output != NULL)
  {
    m_output = *output;
  }
  m_pid = 0;
  m_currentPid = 0;
  m_running = 0;
  m_killed = false;
  m_type = type;



  m_executeFlags = wxEXEC_ASYNC;

}

//----------------------------------------
CProcess::~CProcess()
{
  try
  {
    if (m_killed)
    {
      wxRemoveFile(m_output);
    }
  }
  catch (...)
  {
    //Nothing to do
  }


}
//----------------------------------------
int32_t CProcess::Execute()
{
  bool isSync = ((m_executeFlags & wxEXEC_SYNC) == wxEXEC_SYNC);

  int32_t result = -1;

  if (isSync)
  {
    result = ExecuteSync();
  }
  else
  {
    result = ExecuteAsync();
  }

  return result;
}
//----------------------------------------
int32_t CProcess::ExecuteSync()
{
  int32_t result = wxExecute(m_cmd, m_executeFlags, this);
  m_running = -1;
  OnTerminate(result);
  return result;
}
//----------------------------------------
int32_t CProcess::ExecuteAsync()
{
  m_pid = wxExecute(m_cmd, m_executeFlags, this);
  m_running = m_pid;
  return m_pid;
}


//----------------------------------------
int32_t CProcess::AskForPid()
{
  return wxGetNumberFromUser(_T("For this task you have to specify the process to kill"),
                                 _T("Enter PID:"),
                                 _T("Please specify the process to kill"),
                                 -1,
                                 // we need the full unsigned int range
                                 -INT_MAX, INT_MAX,
                                 NULL);
}
//----------------------------------------
int32_t CProcess::Kill(wxSignal sig)
{
  int32_t result = wxKILL_NO_PROCESS;

  int32_t pid = 0;

  if (wxProcess::Exists(m_pid) )
  {
    pid = m_pid;
  }
  else if (wxProcess::Exists(m_currentPid) )
  {
    pid = m_currentPid;
  }


  // with SYNC execute, pid is not set by wxExecute, so we can't get it.
  // Ask for the user the PID
  if ( !wxProcess::Exists(pid) )
  {
    pid = AskForPid();
    if (pid <= 0)
    {
      return result;
    }
  }


  try
  {
    result = wxProcess::Kill(pid, sig);
    if (result == wxKILL_OK)
    {
      m_killed = true;
      m_running = -1;
    }
  }
  catch (...)
  {
    //Nothing to do
  }

  return result;
}
//----------------------------------------
void CProcess::OnTerminate(int status)
{
  m_running = -1;
  if (m_killed)
  {
    return;
  }

  CProcessTermEvent ev(m_parent->GetId(), m_name, m_type, GetPid(), status);
  wxPostEvent(m_parent, ev);

}
//----------------------------------------
void CProcess::OnTerminate(int pid, int status)
{
  m_running = -1;
  if (m_killed)
  {
    return;
  }

  CProcessTermEvent ev(m_parent->GetId(), m_name, m_type, pid, status);
  wxPostEvent(m_parent, ev);

}
//----------------------------------------
void CProcess::EvtProcessTermCommand(wxEvtHandler& evtHandler, const CProcessTermEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  evtHandler.Connect(wxEVT_PROCESS_TERM,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}
//----------------------------------------
void CProcess::DisconnectEvtProcessTermCommand(wxEvtHandler& evtHandler)
{
  evtHandler.Disconnect(wxEVT_PROCESS_TERM);
}
//-------------------------------------------------------------
//------------------- CPipedProcess class --------------------
//-------------------------------------------------------------
CPipedProcess::CPipedProcess( const wxString& name, wxWindow *parent, const wxString& cmd, wxTextCtrl* logCtrl, const std::string *output, int32_t type )
	: CProcess( name, parent, cmd, output, type )
{
	m_logCtrl = logCtrl;
	m_logFile = NULL;
	m_logFileName = NULL;
	Redirect();
}

//----------------------------------------
CPipedProcess::~CPipedProcess()
{
  DeleteLogFile();
}
//----------------------------------------
void CPipedProcess::DeleteLogFile()
{
  if (m_logFileName != NULL)
  {
    delete m_logFileName;
    m_logFileName = NULL;
  }
  if (m_logFile != NULL)
  {
    // destructor close the file
    delete m_logFile;
    m_logFile = NULL;
  }

}

//----------------------------------------
void CPipedProcess::SetLogFile(const wxString& value)
{
  DeleteLogFile();

  m_logFileName = new wxFileName(value);
  m_logFileName->Normalize();

  m_logFile = new wxFile(m_logFileName->GetFullPath().c_str(), wxFile::write_append);

  if (!m_logFile->IsOpened())
  {
    wxLogError("Unable to open log file '%s'", m_logFileName->GetFullPath().c_str());
  }
}
//----------------------------------------
void CPipedProcess::SetLogFile(const wxFileName& value)
{
  SetLogFile(value.GetFullPath());
}
//----------------------------------------
void CPipedProcess::SetLogFile(wxFileName* value)
{
  if (value == NULL)
  {
    return;
  }

  SetLogFile(value->GetFullPath());
}
//----------------------------------------
void CPipedProcess::WriteToLogFile(const wxString& msg)
{
  if (m_logFile == NULL)
  {
    return;
  }

  if (!m_logFile->IsOpened())
  {
    return;
  }

  m_logFile->Write(msg);
  m_logFile->Flush();

}
//----------------------------------------
bool CPipedProcess::HasInput()
{
  bool hasInput = false;

  if ( IsInputAvailable() )
  {
    wxTextInputStream tis(*GetInputStream());

    // this assumes that the output is always line buffered
    wxString msg = wxString::Format("Task '%s': %s \n",
                                    m_name.c_str(),
                                    tis.ReadLine().c_str());

    //msg << m_name << _T(" (stdout): ") << tis.ReadLine();

    if (m_logCtrl != NULL)
    {
      m_logCtrl->AppendText(msg);
    }

    WriteToLogFile(msg);

    hasInput = true;
  }

  if ( IsErrorAvailable() )
  {
    wxTextInputStream tis(*GetErrorStream());

    // this assumes that the output is always line buffered
    wxString msg = wxString::Format("Task '%s': %s \n",
                                    m_name.c_str(),
                                    tis.ReadLine().c_str());
    //msg << m_name << _T(" (stderr): ") << tis.ReadLine();
    if (m_logCtrl != NULL)
    {
      m_logCtrl->AppendText(msg);
    }

    WriteToLogFile(msg);

    hasInput = true;
  }

  return hasInput;
}
//----------------------------------------
void CPipedProcess::OnTerminate(int status)
{
    // show the rest of the output
    while ( HasInput() )
        ;

    CProcess::OnTerminate(status);
}

//----------------------------------------
void CPipedProcess::OnTerminate(int pid, int status)
{
    // show the rest of the output
    while ( HasInput() )
        ;

    CProcess::OnTerminate(pid, status);
}

//-------------------------------------------------------------
//------------------- CBratTaskProcess class --------------------
//-------------------------------------------------------------
CBratTaskProcess::CBratTaskProcess(CBratTask* bratTask, wxWindow *parent)
      : CPipedProcess("", parent, "")
{
  if (bratTask == NULL)
  {
    throw CException("Unable to create CBratTaskProcess instance: bratTask is null", BRATHL_ERROR);
  }

  m_bratTask = bratTask;

  SetName(m_bratTask->GetUidAsString());

  SetLogFile(m_bratTask->GetLogFile());

  m_subordinateTaskIndex = -1;

  InstallEventListeners();

}

//----------------------------------------
CBratTaskProcess::~CBratTaskProcess()
{
  DeInstallEventListeners();
}

//----------------------------------------
void CBratTaskProcess::InstallEventListeners()
{
  CProcess::EvtProcessTermCommand(*this,
                                (CProcessTermEventFunction)&CBratTaskProcess::OnProcessTerm, NULL, this);

}
//----------------------------------------
void CBratTaskProcess::DeInstallEventListeners()
{
  CProcess::DisconnectEvtProcessTermCommand(*this);
}
//----------------------------------------
bool CBratTaskProcess::HasInput()
{
  bool hasInput = false;

  if ( IsInputAvailable() )
  {
    wxTextInputStream tis(*GetInputStream());

    // this assumes that the output is always line buffered
    wxString msg = wxString::Format("%s \n",
                                    tis.ReadLine().c_str());


    if (m_logCtrl != NULL)
    {
      m_logCtrl->AppendText(msg);
    }

    WriteToLogFile(msg);

    hasInput = true;
  }

  if ( IsErrorAvailable() )
  {
    wxTextInputStream tis(*GetErrorStream());

    // this assumes that the output is always line buffered
    wxString msg = wxString::Format("%s \n",
                                    tis.ReadLine().c_str());

    if (m_logCtrl != NULL)
    {
      m_logCtrl->AppendText(msg);
    }

    WriteToLogFile(msg);

    hasInput = true;
  }

  return hasInput;
}
//----------------------------------------
int32_t CBratTaskProcess::Execute()
{
  //int32_t result = -1;

  if (m_bratTask->HasFunction())
  {
    return ExecuteFunction(m_bratTask);
  }

  return ExecuteAsync();
}
//----------------------------------------
int32_t CBratTaskProcess::ExecuteSync()
{
  throw CException("CBratTaskProcess::ExecuteSync is not implemented", BRATHL_UNIMPLEMENT_ERROR);
}
//----------------------------------------
int32_t CBratTaskProcess::ExecuteAsync()
{
  if (m_bratTask == NULL)
  {
    throw CException("Unable to execute CBratTaskProcess instance: bratTask is null", BRATHL_ERROR);
  }

  SetCmd(m_bratTask->GetCmd());


  int32_t pid = CPipedProcess::ExecuteAsync();

  wxString msg = wxString::Format("====> TASK '%s' (pid %ld) - STARTED ON %s WITH COMMAND LINE BELOW:<===\n'%s'",
                                  m_name.c_str(),
                                  m_pid,
                                  wxDateTime::UNow().Format(CBratTask::formatISODateTime()).c_str(),
                                  GetCmd().c_str());
  wxLogInfo("%s", msg.c_str());
  WriteToLogFile(msg);
  WriteToLogFile("\n");

  return pid;
}
//----------------------------------------
int32_t CBratTaskProcess::ExecuteAsyncSubordinateTasks()
{
  if (m_bratTask == NULL)
  {
    throw CException("Unable to execute CBratTaskProcess instance: bratTask is null", BRATHL_ERROR);
  }

  int32_t countSubTask = static_cast<int32_t>(m_bratTask->GetSubordinateTasks()->size());
  if ((m_subordinateTaskIndex < 0) || (m_subordinateTaskIndex  >= countSubTask))
  {
    wxString msg = wxString::Format("Unable to execute sub-task index %d - Max. index is %d", m_subordinateTaskIndex, countSubTask);
    throw CException(msg.ToStdString(), BRATHL_ERROR);
  }

  CBratTask* subTask = m_bratTask->GetSubordinateTasks()->at(m_subordinateTaskIndex);
  if (subTask == NULL)
  {
    throw CException("Unable to execute CBratTaskProcess instance: subTask is null", BRATHL_ERROR);
  }

  if (subTask->HasFunction())
  {
    return ExecuteFunction(subTask);
  }

  SetCmd(subTask->GetCmd());

  m_currentPid = wxExecute(m_cmd, m_executeFlags, this);

  wxString msg = wxString::Format("===> SUB-TASK '%s' (pid %ld) STARTED ON %s WITH COMMAND LINE BELOW:<===\n'%s'",
                          subTask->GetUidAsString().c_str(),
                          m_currentPid,
                          wxDateTime::UNow().Format(CBratTask::formatISODateTime()).c_str(),
                          GetCmd().c_str());

  wxLogInfo("%s", msg.c_str());
  WriteToLogFile(msg);
  WriteToLogFile("\n");

  return m_currentPid;

}
//----------------------------------------
int32_t CBratTaskProcess::ExecuteFunction(CBratTask* bratTask)
{
  if (bratTask == NULL)
  {
    throw CException("Unable to execute function: bratTask is null (CBratTaskProcess#ExecuteFunction)", BRATHL_ERROR);
  }

  if (!bratTask->HasFunction())
  {
    throw CException("Unable to execute function: bratTask has no function (CBratTaskProcess#ExecuteFunction)", BRATHL_ERROR);
  }

  int32_t status = BRATHL_SUCCESS;
  try
  {
    const CBratTaskFunction* function = bratTask->GetBratTaskFunction();
    if (function == NULL)
    {
      throw CException("Function is NULL (CBratTaskProcess#ExecuteFunction)", BRATHL_ERROR);
    }

    std::string params;
    function->GetParamsAsString(params);

    wxString msg = wxString::Format("===> SUB-TASK '%s' STARTED ON %s WITH FUNCTION BELOW:<===\n'%s%s'",
                            bratTask->GetUidAsString().c_str(),
                            wxDateTime::UNow().Format(CBratTask::formatISODateTime()).c_str(),
                            function->GetName().c_str(),
                            params.c_str());

    wxLogInfo("%s", msg.c_str());
    WriteToLogFile(msg);
    WriteToLogFile("\n");

    //-------------------------
    bratTask->ExecuteFunction();
    //-------------------------

    //msg = wxString::Format("===> SUB-TASK '%s' SUCCESSFULLY ENDED ON %s <===",
    //                        bratTask->GetUidAsString().c_str(),
    //                        wxDateTime::UNow().Format(CBratTask::FormatISODateTime).c_str());
    //wxLogInfo(msg);
    //WriteToLogFile(msg);
    //WriteToLogFile("\n");
  }
  catch(CException& e)
  {
    status = BRATHL_ERROR;
    wxString msg = wxString::Format("===> SUB-TASK '%s' - ERROR: <===\n%s",
                            bratTask->GetUidAsString().c_str(),
                            e.what());

    wxLogInfo("%s", msg.c_str());
    WriteToLogFile(msg);
    WriteToLogFile("\n");
  }

  CProcessTermEvent ev(wxID_ANY, bratTask->GetUidAsString(), m_type, 0, status);
  wxPostEvent(this, ev);

  return 0;

}
//----------------------------------------
void CBratTaskProcess::OnProcessTerm(CProcessTermEvent& event)
{
  OnTerminate(event.m_pid, event.m_exitcode);

}
//----------------------------------------
void CBratTaskProcess::OnTerminate(int status)
{
    UNUSED(status);

   throw CException("CBratTaskProcess::OnTerminate(int status) is not implemented", BRATHL_UNIMPLEMENT_ERROR);

}
//----------------------------------------
void CBratTaskProcess::OnTerminate(int pid, int status)
{
  // show the rest of the output
  while ( HasInput() )
      ;


  if (m_subordinateTaskIndex >= 0)
  {
    OnTerminateReport(pid, status, m_bratTask->GetSubordinateTasks()->at(m_subordinateTaskIndex), "SUB-TASK");
  }

  if (!m_killed &&
      ((status == BRATHL_SUCCESS) || (status == BRATHL_WARNING)) &&
      m_bratTask->HasSubordinateTasks())
  {
    int32_t countSubTask = static_cast<int32_t>(m_bratTask->GetSubordinateTasks()->size());

    m_subordinateTaskIndex++;

    if (m_subordinateTaskIndex < countSubTask)
    {
      ExecuteAsyncSubordinateTasks();
      return;
    }
  }

  OnTerminateReport(m_pid, status, m_bratTask);

  m_logFile->Flush();
  m_logFile->Close();

  DeInstallEventListeners();

  CProcessTermEvent ev(m_parent->GetId(), m_name, m_bratTask->GetUid(), m_type, m_pid, status);
  wxPostEvent(m_parent, ev);

}

//----------------------------------------
void CBratTaskProcess::OnTerminateReport(int pid, int status, CBratTask* bratTask, const wxString& taskLabel /* = "TASK" */)
{
  wxString taskUid;

  if (bratTask != NULL)
  {
    taskUid = bratTask->GetUidAsString();
  }

  wxString msg;

  wxString nowAsString = wxDateTime::UNow().Format(CBratTask::formatISODateTime());

  m_running = -1;

  if (m_killed)
  {
    msg = wxString::Format("===> %s '%s' (pid %ld) KILLED BY THE USER ON %s <===",
                                  taskLabel.c_str(),
                                  taskUid.c_str(),
                                  (long int)pid,
                                  nowAsString.c_str());
  }
  else
  {
    if (status == BRATHL_SUCCESS)
    {
      msg = wxString::Format("===> %s '%s' (pid %ld) SUCCESSFULLY ENDED ON %s <===",
                                    taskLabel.c_str(),
                                    taskUid.c_str(),
                                    (long int)pid,
                                    nowAsString.c_str());
    }
    else if (status == BRATHL_WARNING)
    {
      msg = wxString::Format("===> %s '%s' (pid %ld) ENDED WITH WARNINGS (code %d) ON %s - SEE LOG FILE <===",
                                    taskLabel.c_str(),
                                    taskUid.c_str(),
                                    (long int)pid,
                                    status,
                                    nowAsString.c_str());
    }
    else
    {
      msg = wxString::Format("===> %s '%s' (pid %ld) UNSUCCESSFULLY ENDED (code %d) ON %s - SEE LOG FILE <===",
                                    taskLabel.c_str(),
                                    taskUid.c_str(),
                                    (long int)pid,
                                    status,
                                    nowAsString.c_str());
    }
  }

  wxLogInfo("%s", msg.c_str());
  WriteToLogFile(msg);
  WriteToLogFile("\n");

}

//-------------------------------------------------------------
//------------------- CMapProcess class --------------------
//-------------------------------------------------------------


CMapProcess::CMapProcess(bool bDelete)
      : wxObMap(bDelete)
{
}


//----------------------------------------
CMapProcess::~CMapProcess()
{
}
//----------------------------------------
bool CMapProcess::ValidName(const std::string& name)
{
  return ValidName(name.c_str());
}
//----------------------------------------
bool CMapProcess::ValidName(const char* name)
{
  CProcess* value = dynamic_cast<CProcess*>(Exists(name));
  return (value != NULL);
}

//----------------------------------------
void CMapProcess::NamesToArrayString(wxArrayString& array)
{
  CMapProcess::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CProcess* value = dynamic_cast<CProcess*>(it->second);
    if (value != NULL)
    {
      array.Add(value->GetName());
    }
  }

}
//----------------------------------------
void CMapProcess::NamesToComboBox(wxComboBox& combo)
{
  CMapProcess::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CProcess* value = dynamic_cast<CProcess*>(it->second);
    if (value != NULL)
    {
      combo.Append(value->GetName());
    }
  }

}

//----------------------------------------

