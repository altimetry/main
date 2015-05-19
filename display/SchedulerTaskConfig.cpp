/*
* Copyright (C) 2005-2010 Collecte Localisation Satellites (CLS), France (31)
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

#include <cerrno>

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "SchedulerTaskConfig.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/stdpaths.h"
#include "wx/filename.h"

#include "wxBratTools.h"

#include "SchedulerTaskConfig.h"

// When debugging changes all calls to “new” to be calls to “DEBUG_NEW” allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

// WDR: class implementations



wxCriticalSection CSchedulerTaskConfig::m_critSectSchedulerTaskConfigInstance;

wxString CSchedulerTaskConfig::m_CONFIG_APPNAME = "BratScheduler";

const wxString CSchedulerTaskConfig::m_CONFIG_FILE_NAME = "BratSchedulerTasksConfig.xml";
const wxString CSchedulerTaskConfig::m_LOG_REL_PATH = "Logs";

CSchedulerTaskConfig* CSchedulerTaskConfig::m_instance = NULL;
wxLogBuffer* CSchedulerTaskConfig::m_logBuffer = NULL;
wxLog* CSchedulerTaskConfig::m_initialLog = NULL;

const wxString CSchedulerTaskConfig::m_DEFAULT_TASK_NAME = "unnamed";

const wxString CSchedulerTaskConfig::m_ENDED_ELT = "endedTasks";
const wxString CSchedulerTaskConfig::m_PENDING_ELT = "pendingTasks";
const wxString CSchedulerTaskConfig::m_PROCESSING_ELT = "processingTasks";
const wxString CSchedulerTaskConfig::m_ROOT_ELT = "bratSchedulerConfig";
const wxString CSchedulerTaskConfig::m_TASK_ELT = "task";
const wxString CSchedulerTaskConfig::m_ARG_ELT = "arg";

const wxString CSchedulerTaskConfig::m_TASK_AT_ATTR = "at";
const wxString CSchedulerTaskConfig::m_TASK_CMD_ATTR = "cmd";
const wxString CSchedulerTaskConfig::m_TASK_FUNCTION_ATTR = "function";
const wxString CSchedulerTaskConfig::m_TASK_ID_ATTR = "uid";
const wxString CSchedulerTaskConfig::m_TASK_LOG_FILE_ATTR = "logFile";
const wxString CSchedulerTaskConfig::m_TASK_NAME_ATTR = "name";
const wxString CSchedulerTaskConfig::m_TASK_RETURN_CODE_ATTR = "returnCode";
const wxString CSchedulerTaskConfig::m_TASK_STATUS_ATTR = "status";
const wxString CSchedulerTaskConfig::m_TYPE_ATTR = "type";


//----------------------------------------------------------------------------
// SchedulerTaskConfig
//----------------------------------------------------------------------------

CSchedulerTaskConfig::CSchedulerTaskConfig()
{
  Init();
}

//----------------------------------------
CSchedulerTaskConfig::CSchedulerTaskConfig(const wxXmlDocument& doc)
                      : wxXmlDocument(doc)
{
  Init();
}

//----------------------------------------
CSchedulerTaskConfig::CSchedulerTaskConfig(const CSchedulerTaskConfig& o)                     
{
  Init();
  Set(o);
}
//----------------------------------------
CSchedulerTaskConfig::CSchedulerTaskConfig(const wxString& filename, bool lockFile /* = true */, bool unlockFile /* = true */, const wxString &encoding /* = "UTF-8" */)
{

  // Don't call wxXmlDocument(filename, encoding) because overriden 'Load' will be not called

  Init();
  m_fullFileName = filename;

  if ( !Load(filename, lockFile, unlockFile, encoding) )
  {
    DeleteRootNode();
  }

}
//----------------------------------------
CSchedulerTaskConfig::CSchedulerTaskConfig(wxInputStream& stream, const wxString &encoding /* = "UTF-8" */)
                      : wxXmlDocument(stream, encoding)
{
  Init();
}

//----------------------------------------
CSchedulerTaskConfig::~CSchedulerTaskConfig()
{
  DeleteConfigFileChecker();
}

//----------------------------------------
void CSchedulerTaskConfig::Init()
{
  m_mapPendingBratTask.SetDelete(false);
  m_mapProcessingBratTask.SetDelete(false);
  m_mapEndedBratTask.SetDelete(false);
  m_mapNewBratTask.SetDelete(false);

  m_configFileChecker = NULL;
  
  m_reloadAll = false;
}
//----------------------------------------
void CSchedulerTaskConfig::Set(const CSchedulerTaskConfig& o)                     
{
  wxXmlDocument::operator=(o);

  RemoveMapBratTasks();
  AddTask(&o.m_mapBratTask);


}
//----------------------------------------
CSchedulerTaskConfig& CSchedulerTaskConfig::operator=(const CSchedulerTaskConfig& o)                     
{
  if (this == &o)
  {
    return *this;
  }

  Set(o);
  return *this;

}
//----------------------------------------
wxString CSchedulerTaskConfig::GetUserDataDir()
{
  wxStandardPaths standardPaths;
  wxString dirName = standardPaths.GetUserDataDir();
  wxFileName dir;
  dir.AssignDir(dirName);
  dir.Normalize();
  if (!dir.DirExists())
  {
    dir.Mkdir(0777, wxPATH_MKDIR_FULL);
  }
  return dirName;

}
//----------------------------------------
wxString CSchedulerTaskConfig::GetUserConfigDir()
{
  wxStandardPaths standardPaths;
  wxString dirName = standardPaths.GetUserConfigDir();
  wxFileName dir;
  dir.AssignDir(dirName);
  dir.Normalize();
  if (!dir.DirExists())
  {
    dir.Mkdir(0777, wxPATH_MKDIR_FULL);
  }
  return dirName;

}
//----------------------------------------
wxString CSchedulerTaskConfig::GetUserConfigDir(const wxString& appName)
{
  wxString dirName = GetUserConfigDir();

  if (appName.IsEmpty())
  {
    return dirName;
  }

  dirName = wxString::Format("%s/%s", dirName.c_str(), appName.c_str());
  
  wxFileName dir;
  dir.AssignDir(dirName);
  dir.Normalize();
  if (!dir.DirExists())
  {
    dir.Mkdir(0777, wxPATH_MKDIR_FULL);
  }
  return dirName;

}
//----------------------------------------
wxString CSchedulerTaskConfig::GetConfigFilePath()
{
  return wxString::Format("%s/%s", CSchedulerTaskConfig::GetUserDataDir().c_str(), CSchedulerTaskConfig::m_CONFIG_FILE_NAME.c_str());
}
//----------------------------------------
wxString CSchedulerTaskConfig::GetConfigFilePath(const wxString& appName)
{
  if (appName.IsEmpty())
  {
    return GetConfigFilePath();
  }

  return wxString::Format("%s/%s", CSchedulerTaskConfig::GetUserConfigDir(appName).c_str(), CSchedulerTaskConfig::m_CONFIG_FILE_NAME.c_str());
}

//----------------------------------------
wxString CSchedulerTaskConfig::GetLogFilePath()
{
  wxString dirName = wxString::Format("%s/%s", CSchedulerTaskConfig::GetUserDataDir().c_str(), CSchedulerTaskConfig::m_LOG_REL_PATH.c_str());;
  wxFileName dir;
  dir.AssignDir(dirName);
  dir.Normalize();
  if (!dir.DirExists())
  {
    dir.Mkdir(0777, wxPATH_MKDIR_FULL);
  }
  return dirName;

}
//----------------------------------------
wxString CSchedulerTaskConfig::GetLogFilePath(const wxString& appName)
{
  if (appName.IsEmpty())
  {
    return GetLogFilePath();
  }
  wxString dirName = wxString::Format("%s/%s", CSchedulerTaskConfig::GetUserConfigDir(appName.c_str()).c_str(), CSchedulerTaskConfig::m_LOG_REL_PATH.c_str());;
  wxFileName dir;
  dir.AssignDir(dirName);
  dir.Normalize();
  if (!dir.DirExists())
  {
    dir.Mkdir(0777, wxPATH_MKDIR_FULL);
  }
  return dirName;

}
//----------------------------------------------------
void CSchedulerTaskConfig::PrepareSmartCleaner()
{
  // make a single entry point for creating the smart cleaner because there are multiple
  // GetInstance interfaces.
  //wxCriticalSectionLocker locker(CSchedulerTaskConfig::CSmartCleaner::m_critSectSmartCleanerInstance);
  //cout << "Enter PrepareSmartCleaner" << endl;
  static CSmartCleaner object(&CSchedulerTaskConfig::m_instance, &CSchedulerTaskConfig::m_logBuffer);
}
//----------------------------------------
CSchedulerTaskConfig* CSchedulerTaskConfig::GetInstance(bool reload /* = false */, bool lockFile /* = true */, bool unlockFile  /* = true */, const wxString& encoding /* = "UTF-8" */)
{
  wxString str = GetConfigFilePath(CSchedulerTaskConfig::m_CONFIG_APPNAME);
  return CSchedulerTaskConfig::GetInstance(&str, reload, lockFile, unlockFile, encoding);
}
//----------------------------------------------------
CSchedulerTaskConfig* CSchedulerTaskConfig::GetInstance(const wxString* fileName, bool reload /* = false */, bool lockFile /* = true */, bool unlockFile  /* = true */, const wxString& encoding /* = "UTF-8" */)
{
  wxCriticalSectionLocker locker(CSchedulerTaskConfig::m_critSectSchedulerTaskConfigInstance);
   //cout << "Enter GetInstance" << endl;
  if (CSchedulerTaskConfig::m_instance != NULL)
  {
    if (reload)
    {
      if (CSchedulerTaskConfig::m_instance->m_reloadAll)
      {
        CSchedulerTaskConfig::m_instance->Load(*fileName, lockFile, unlockFile, encoding);
      }
      else
      {
        // "Reload" function just load (add in memory) new defined tasks from the configuration file
        CSchedulerTaskConfig::m_instance->Reload(lockFile, unlockFile);
      }
    }
  }
  else 
  {
    //cout << "GetInstance" << endl;
    if (fileName != NULL)
    {      

      if (CSchedulerTaskConfig::m_logBuffer == NULL)
      {
        CSchedulerTaskConfig::m_logBuffer = new wxLogBuffer();
        CSchedulerTaskConfig::m_initialLog = wxLog::GetActiveTarget();
      }
      
      CreateXmlFile(*fileName);

      wxLog::SetActiveTarget(m_logBuffer);

      bool bOk = true;

      wxString errorMsg;
      try
      {
          CSchedulerTaskConfig::m_instance = new CSchedulerTaskConfig(*fileName, lockFile, unlockFile, encoding);
          bOk = CSchedulerTaskConfig::m_instance->IsOk();
      }
      catch(CException& e)
      {
        bOk = false;
        errorMsg = e.GetMessage().c_str();
      }

      PrepareSmartCleaner();
      
      if (!bOk)
      {
        wxString msg;
        if (errorMsg.IsEmpty())
        {
          wxString parserError = m_logBuffer->GetBuffer();
          msg = wxString::Format("Unable to load Brat Scheduler configuration file '%s' - Please, check XML syntax - Parser error: '%s'", fileName->c_str(), parserError.c_str());
        }
        else
        {
          msg = errorMsg;
        }

        wxLog::SetActiveTarget(CSchedulerTaskConfig::m_initialLog);
        
        throw CException(msg.c_str(), BRATHL_ERROR);
      }
      
      wxLog::SetActiveTarget(CSchedulerTaskConfig::m_initialLog);

    }
  }
   //cout << "Exit GetInstance" << endl;

  return CSchedulerTaskConfig::m_instance;
}
////----------------------------------------------------
//CSchedulerTaskConfig* CSchedulerTaskConfig::GetInstance(const string* fileName, bool reload /* = false */, const wxString& encoding /* = "UTF-8" */)
//{
//  if (fileName == NULL)
//  {
//    return GetInstance(fileName, reload encoding);
//  }
//  wxString fileNameTmp(fileName->c_str());
//
//  return GetInstance(&fileNameTmp, reload, encoding);
//}
//----------------------------------------------------
bool CSchedulerTaskConfig::CreateXmlFile(const wxString& fileName)
{
  if (::wxFileExists(fileName))
  {
    return true;
  }

  CSchedulerTaskConfig xml;
  xml.AddRootNode();
  xml.AddPendingTaskNode();
  xml.AddEndedTaskNode();
  
  return xml.Save(fileName);

}

////----------------------------------------------------
//wxMutexError CSchedulerTaskConfig::MutexLock()
//{
//  wxMutexError mutexError = m_mutex.Lock();
//  uint32_t i = 0;
//  while ((!m_mutex.IsOk()) && (i < 5))
//  {
//    wxSleep(1);
//    i++;
//    mutexError = m_mutex.Lock();
//  }
//
//  return mutexError;
//}
////----------------------------------------------------
//wxMutexError CSchedulerTaskConfig::MutexUnLock()
//{
//  wxMutexError mutexError = wxMUTEX_NO_ERROR;
//
//  if (m_mutex.IsOk())
//  {
//    mutexError = m_mutex.Unlock();
//  }
//
//  return mutexError;
//}
//----------------------------------------------------
void CSchedulerTaskConfig::DeleteRootNode()
{
  wxXmlNode* root = DetachRoot();
  wxDELETE(root);
}
//----------------------------------------------------
void CSchedulerTaskConfig::DeleteConfigFileChecker()
{
  if (m_configFileChecker != NULL)
  {
    wxCriticalSectionLocker locker(m_critSectConfigFileChecker);
    delete m_configFileChecker;
    m_configFileChecker = NULL;
  }

}
//----------------------------------------------------
void CSchedulerTaskConfig::NewConfigFileChecker(const wxString& fileName)
{
  DeleteConfigFileChecker();

  // remove non-word character
  wxString mutexName = CTools::Replace(fileName.c_str(), "\\W", "").c_str();
  mutexName.Lower();

  wxCriticalSectionLocker locker(m_critSectConfigFileChecker);
  m_configFileChecker = new wxSingleInstanceChecker(mutexName.c_str());
}
//----------------------------------------------------
bool CSchedulerTaskConfig::LockConfigFile(bool lockFile /* = true */)
{
  return LockConfigFile(m_fullFileName, lockFile);
}
//----------------------------------------------------
bool CSchedulerTaskConfig::LockConfigFile(const wxString& fileName, bool lockFile /* = true */)
{
  bool bOk = true;

  if (!lockFile)
  {
    return bOk;
  }

  NewConfigFileChecker(fileName);

  uint32_t i = 0;
  // Try to get exclusive access again
  while ((m_configFileChecker->IsAnotherRunning()) && (i < 10))
  {
    wxThread::Sleep(1000);
    i++;
    NewConfigFileChecker(fileName);
  }

  if (m_configFileChecker->IsAnotherRunning())
  {
    DeleteConfigFileChecker();
    bOk = false;
  }

  return bOk;
}
//----------------------------------------------------
void CSchedulerTaskConfig::UnLockConfigFile(bool unlockFile)
{
  if (!unlockFile)
  {
    return;
  }

  DeleteConfigFileChecker();
}
//----------------------------------------------------
void CSchedulerTaskConfig::RemoveMapBratTasks()
{
  wxCriticalSectionLocker locker(m_critSectMapBratTask);

  m_mapPendingBratTask.RemoveAll();
  m_mapProcessingBratTask.RemoveAll();
  m_mapEndedBratTask.RemoveAll();
  m_mapNewBratTask.RemoveAll();

  m_mapBratTask.RemoveAll();

}
//----------------------------------------------------
bool CSchedulerTaskConfig::ChangeProcessingToPending(CVectorBratTask& vectorTasks)
{
  CMapBratTask::const_iterator it;

  vectorTasks.SetDelete(false);

  // tasks will be deleted from m_mapProcessingBratTask by ChangeTaskStatus function
  // Don't change status within the loop, store tasks into a vecor and then change status from vector tasks
  for (it = m_mapProcessingBratTask.begin() ; it != m_mapProcessingBratTask.end() ; it++)
  {
    CBratTask* bratTask = it->second;
    if (bratTask == NULL)
    {
      continue;
    }

    vectorTasks.Insert(bratTask);

  }

  CVectorBratTask::const_iterator itVector;

  for (itVector = vectorTasks.begin() ; itVector != vectorTasks.end() ; itVector++)
  {
    CBratTask* bratTask = *itVector;
    if (bratTask == NULL)
    {
      continue;
    }

    ChangeTaskStatus(bratTask, CBratTask::BRAT_STATUS_PENDING);

  }

  return (vectorTasks.size() > 0);

}

//----------------------------------------------------
void CSchedulerTaskConfig::ChangeTaskStatus(CBratTask* bratTask, CBratTask::bratTaskStatus newStatus)
{
  ChangeTaskStatus(bratTask->GetUidValue(), newStatus);
}
//----------------------------------------------------
void CSchedulerTaskConfig::ChangeTaskStatus(wxLongLong_t id, CBratTask::bratTaskStatus newStatus)
{
  ChangeTaskStatusFromXml(id, newStatus);
  CBratTask::bratTaskStatus oldStatus = ChangeTaskStatusFromMap(id, newStatus);
 
  wxLongLong idObj(id);
  wxString idAsString = idObj.ToString();
  wxLogInfo("Task '%s': status has been changed from '%s' to '%s'.", idAsString.c_str(), CBratTask::TaskStatusToString(oldStatus).c_str(), CBratTask::TaskStatusToString(newStatus).c_str());
}
//----------------------------------------------------
void CSchedulerTaskConfig::ChangeTaskStatusFromXml(wxLongLong_t id, CBratTask::bratTaskStatus newStatus)
{
  wxCriticalSectionLocker locker(m_critSectXmlConfigFile);

  wxLongLong idObj(id);
  wxString idAsString = idObj.ToString();

  wxXmlNode* node = FindTaskNode(id, this->GetRoot(), true);

  if (node == NULL)
  {
    wxString msg = wxString::Format("Unable to update task status: task uid '%s' is not found (CSchedulerTaskConfig#ChangeTaskStatusFromXml).",
                                    idAsString.c_str());

    throw CException(msg.c_str(), BRATHL_ERROR);

  }

  wxXmlNode* parent = node->GetParent();
  if (parent == NULL)
  {
    wxString msg = wxString::Format("Unable to update task status: task uid '%s' is has no Xml parent (CSchedulerTaskConfig#ChangeTaskStatusFromXml).",
                                    idAsString.c_str());

    throw CException(msg.c_str(), BRATHL_ERROR);
  }


  bool removed = parent->RemoveChild(node);

  if (!removed)
  {
    wxString msg = wxString::Format("Unable to remove Xml node for task uid '%s' (CSchedulerTaskConfig#ChangeTaskStatusFromXml).",
                                    idAsString.c_str());

    throw CException(msg.c_str(), BRATHL_ERROR);
  }



  switch (newStatus)
  {
    case CBratTask::BRAT_STATUS_PENDING:
    {
      parent = AddPendingTaskNode();
      break;
    }
    case CBratTask::BRAT_STATUS_PROCESSING:
    {
      parent = AddProcessingTaskNode();
      break;
    }
    case CBratTask::BRAT_STATUS_ENDED:
    case CBratTask::BRAT_STATUS_ERROR:
    case CBratTask::BRAT_STATUS_WARNING:
    {
      parent = AddEndedTaskNode();
      break;
    }
    default:
    {
      wxString msg = wxString::Format("ERROR in CSchedulerTaskConfig#ChangeTaskStatusFromXml - status %d (%s) not implemented in the function.", 
                                      static_cast<int32_t>(newStatus), CBratTask::TaskStatusToString(newStatus).c_str());

      throw CException(msg.c_str(), BRATHL_ERROR);
    }
  }

  if (parent == NULL)
  {
    wxString msg = wxString::Format("Unable to update task status: task uid '%s' is has no Xml parent for its new status %d (%s) (CSchedulerTaskConfig#ChangeTaskStatusFromXml).",
                                    idAsString.c_str(), static_cast<int32_t>(newStatus), CBratTask::TaskStatusToString(newStatus).c_str());

    throw CException(msg.c_str(), BRATHL_ERROR);
  }

  this->SetPropVal(node, CSchedulerTaskConfig::m_TASK_STATUS_ATTR, CBratTask::TaskStatusToString(newStatus));

  parent->AddChild(node);

}
//----------------------------------------------------
CBratTask::bratTaskStatus CSchedulerTaskConfig::ChangeTaskStatusFromMap(wxLongLong_t id, CBratTask::bratTaskStatus newStatus)
{
  wxCriticalSectionLocker locker(m_critSectMapBratTask);
    
  wxLongLong idObj(id);
  wxString idAsString = idObj.ToString();

  CBratTask* bratTask = m_mapBratTask.Find(id);
  
  if (bratTask == NULL)
  {
    wxString msg = wxString::Format("Unable to update task status: task uid '%s' is not found (CSchedulerTaskConfig#ChangeTaskStatusFromMap).",
                                    idAsString.c_str());

    throw CException(msg.c_str(), BRATHL_ERROR);

  }

  CBratTask::bratTaskStatus currentStatus = bratTask->GetStatus();

  switch (currentStatus)
  {
    case CBratTask::BRAT_STATUS_PENDING:
    {
      m_mapPendingBratTask.Remove(id);
      break;
    }
    case CBratTask::BRAT_STATUS_PROCESSING:
    {
      m_mapProcessingBratTask.Remove(id);
      break;
    }
    case CBratTask::BRAT_STATUS_ENDED:
    case CBratTask::BRAT_STATUS_ERROR:
    case CBratTask::BRAT_STATUS_WARNING:
    {
      m_mapEndedBratTask.Remove(id);
      break;
    }
    default:
    {
      wxString msg = wxString::Format("ERROR in CSchedulerTaskConfig#ChangeTaskStatusFromMap - status %d (%s) not implemented in the function.", 
                                      static_cast<int32_t>(currentStatus), bratTask->GetStatusAsString().c_str());

      throw CException(msg.c_str(), BRATHL_ERROR);
    }
  }

  bratTask->SetStatus(newStatus);

  switch (newStatus)
  {
    case CBratTask::BRAT_STATUS_PENDING:
    {
      m_mapPendingBratTask.Insert(id, bratTask);
      break;
    }
    case CBratTask::BRAT_STATUS_PROCESSING:
    {
      m_mapProcessingBratTask.Insert(id, bratTask);
      break;
    }
    case CBratTask::BRAT_STATUS_ENDED:
    case CBratTask::BRAT_STATUS_ERROR:
    case CBratTask::BRAT_STATUS_WARNING:
    {
      m_mapEndedBratTask.Insert(id, bratTask);
      break;
    }
    default:
    {
      wxString msg = wxString::Format("ERROR in CSchedulerTaskConfig#ChangeTaskStatusFromMap - status %d (%s) not implemented in the function.", 
                                      static_cast<int32_t>(newStatus), CBratTask::TaskStatusToString(newStatus).c_str());

      throw CException(msg.c_str(), BRATHL_ERROR);
    }
  }

  return currentStatus;

}
//----------------------------------------------------
void CSchedulerTaskConfig::LoadTasks()
{
  RemoveMapBratTasks();
  LoadPendingTasks();
  LoadProcessingTasks();
  LoadEndedTasks();
}
//----------------------------------------------------
bool CSchedulerTaskConfig::LoadTasks(wxXmlNode* node)
{
  if (node == NULL)
  {
    return false;
  }

  wxXmlNode* child = node->GetChildren();
  
  while (child != NULL) 
  {
    if (child->GetName().CmpNoCase(CSchedulerTaskConfig::m_TASK_ELT) != 0)
    {
      child = child->GetNext();
      continue;
    }

    CBratTask* bratTask = CreateBratTask(child);
    AddTaskToMap(bratTask);

    LoadSubordinateTasks(child, bratTask);

    child = child->GetNext();
  }

  return true;

}

//----------------------------------------------------
bool CSchedulerTaskConfig::LoadEndedTasks()
{
  LoadTasks(GetEndedTaskNode());

  return (m_mapEndedBratTask.size() > 0);
}

//----------------------------------------------------
bool CSchedulerTaskConfig::LoadPendingTasks()
{
  LoadTasks(GetPendingTaskNode());

  return (m_mapPendingBratTask.size() > 0);
}
//----------------------------------------------------
bool CSchedulerTaskConfig::LoadProcessingTasks()
{
  LoadTasks(GetProcessingTaskNode());

  return (m_mapProcessingBratTask.size() > 0);
}

//----------------------------------------------------
bool CSchedulerTaskConfig::LoadSubordinateTasks(wxXmlNode* taskNode, CBratTask* bratTask)
{
  if (taskNode == NULL)
  {
    return false;
  }
  if (bratTask == NULL)
  {
    return false;
  }
  wxXmlNode* child = taskNode->GetChildren();
  
  while (child != NULL) 
  {
    if (child->GetName().CmpNoCase(CSchedulerTaskConfig::m_TASK_ELT) != 0)
    {
      child = child->GetNext();
      continue;
    }

    CBratTask* subordinateBratTask = CreateBratTask(child);
    bratTask->GetSubordinateTasks()->Insert(subordinateBratTask);
    
    LoadSubordinateTasks(child, subordinateBratTask);

    child = child->GetNext();
  }

  return (bratTask->GetSubordinateTasks()->size() > 0);
}
//----------------------------------------------------
bool CSchedulerTaskConfig::Load(const wxString& fileName, bool lockFile /* = true */, bool unlockFile /* = true */, const wxString& encoding /* = wxT("UTF-8") */, int flags /* = wxXMLDOC_NONE */)
{
  bool bOk = LockConfigFile(fileName, lockFile);

  if (!bOk)
  {
    UnLockConfigFile(unlockFile);
    wxString msg = "Unable to load Brat Scheduler configuration file. Perhaps, it's used by another application - Try again later.";
    throw CException(msg.c_str(), BRATHL_WARNING);
  }

  wxLog::SetActiveTarget(CSchedulerTaskConfig::m_initialLog);
  wxLogInfo("Loading '%s' ...", fileName.c_str());

  wxLog::SetActiveTarget(m_logBuffer);

  bOk = false;

  try
  {

    DeleteRootNode();
    
    bOk = wxXmlDocument::Load(fileName, encoding, flags);
    if (!bOk)
    {
      wxString parserError = m_logBuffer->GetBuffer();
      wxString msg = wxString::Format("Unable to load Brat Scheduler configuration file '%s' -  Native error: '%s'", fileName.c_str(), parserError.c_str());

      wxLog::SetActiveTarget(CSchedulerTaskConfig::m_initialLog);
      
      DeleteRootNode();

      throw CException(msg.c_str(), BRATHL_WARNING);

    }
  }
  catch(CException& e)
  {
    UnLockConfigFile(unlockFile);
    throw e;
  }
  catch(exception& e)
  {
    UnLockConfigFile(unlockFile);
    throw e;
  }
  catch(...)
  {
    UnLockConfigFile(unlockFile);
    throw CException("Unexpected error while loading Brat Scheduler configuration file - No Context and no message have been set for this error", BRATHL_ERROR);
  }

  wxLog::SetActiveTarget(CSchedulerTaskConfig::m_initialLog);

  if (bOk)
  {
    LoadTasks();
  }

  UnLockConfigFile(unlockFile);

  wxLogInfo("'%s' loaded.", fileName.c_str());

  return bOk;

}
//----------------------------------------------------
bool CSchedulerTaskConfig::Load(bool lockFile /* = true */, bool unlockFile  /* = true */, const wxString& encoding /* = wxT("UTF-8") */, int flags /* = wxXMLDOC_NONE */)
{
  return Load(m_fullFileName, lockFile, unlockFile, encoding, flags);
}

//----------------------------------------------------
bool CSchedulerTaskConfig::Reload(bool lockFile /* = true */, bool unlockFile  /* = true */)
{
  m_mapNewBratTask.RemoveAll();

  bool bOk = this->LockConfigFile(lockFile);

  if (!bOk)
  {
    UnLockConfigFile(unlockFile);
    wxString msg = "Unable to load Brat Scheduler configuration file. Perhaps, it's used by another application - Try again later.";
    throw CException(msg.c_str(), BRATHL_WARNING);
  }

  wxLog::SetActiveTarget(CSchedulerTaskConfig::m_initialLog);
  wxLogInfo("Re-loading '%s' ...", m_fullFileName.c_str());


  CSchedulerTaskConfig* schedulerTaskConfig = NULL;
  bOk = true;

  wxLog::SetActiveTarget(m_logBuffer);

  wxString errorMsg;
  try
  {
    schedulerTaskConfig = new CSchedulerTaskConfig(m_fullFileName, false, false, this->GetFileEncoding());
    bOk = schedulerTaskConfig->IsOk();
  }
  catch(CException& e)
  {
    bOk = false;
    errorMsg = e.GetMessage().c_str();
  }

  if (!bOk)
  {
    wxString msg;
    if (errorMsg.IsEmpty())
    {
      wxString parserError = m_logBuffer->GetBuffer();
      msg = wxString::Format("Unable to load Brat Scheduler configuration file '%s' - Please, check XML syntax - Parser error: '%s'", m_fullFileName.c_str(), parserError.c_str());
    }
    else
    {
      msg = errorMsg;
    }

    wxLog::SetActiveTarget(CSchedulerTaskConfig::m_initialLog);
        
    Delete(schedulerTaskConfig);

    this->UnLockConfigFile(unlockFile);

    throw CException(msg.c_str(), BRATHL_ERROR);
  }
      
  wxLog::SetActiveTarget(CSchedulerTaskConfig::m_initialLog);
  
  Synchronize(schedulerTaskConfig);

  Delete(schedulerTaskConfig);

  this->UnLockConfigFile(unlockFile);

  wxLogInfo("'%s' re-loaded.", m_fullFileName.c_str());

  return bOk;
}

//----------------------------------------------------
void CSchedulerTaskConfig::Delete(CSchedulerTaskConfig*& schedulerTaskConfig)
{
  if (schedulerTaskConfig != NULL)
  {
    delete schedulerTaskConfig;
    schedulerTaskConfig = NULL;
  }
}
//----------------------------------------------------
bool CSchedulerTaskConfig::Synchronize(CSchedulerTaskConfig* sched)
{
  if (sched == NULL)
  {
    return false;
  }

  bool bOk = true;
  bOk &= AddNewTask(sched);

  return bOk;
}
//----------------------------------------------------
void CSchedulerTaskConfig::AddTask(const CMapBratTask* mapBratTask)
{
  if (mapBratTask == NULL)
  {
    return;
  }

  CMapBratTask::const_iterator it;
  for (it = mapBratTask->begin() ; it != mapBratTask->end() ; it++)
  {
    wxLongLong_t id = it->first;
    CBratTask* bratTaskNew = it->second;
    if (bratTaskNew == NULL)
    {
      continue;
    }

    AddTaskToMap(new CBratTask(*bratTaskNew));
  }

}
//----------------------------------------------------
bool CSchedulerTaskConfig::AddNewTask(CSchedulerTaskConfig* sched)
{
  if (sched == NULL)
  {
    return false;
  }

  CMapBratTask::const_iterator it;
  for (it = sched->m_mapBratTask.begin() ; it != sched->m_mapBratTask.end() ; it++)
  {
    wxLongLong_t id = it->first;
    CBratTask* bratTaskNew = it->second;
    if (bratTaskNew == NULL)
    {
      continue;
    }
    CBratTask* bratTask = m_mapBratTask.Find(id);
    if (bratTask == NULL)
    {
      wxLogInfo("Add task '%s' to pending list", bratTaskNew->GetUidValueAsString().c_str());
      
      wxXmlNode* nodeNew = sched->FindTaskNode(bratTaskNew->GetUidValue(), sched->GetPendingTaskNode(), true);

      if (nodeNew == NULL)
      {
        wxString msg = wxString::Format("Unable to find task id '%s' in Xml file while adding new tasks (CSchedulerTaskConfig::AddNewTask) ", bratTaskNew->GetUidValueAsString().c_str());
        throw CException(msg.c_str(), BRATHL_ERROR);
      }

      wxXmlNode* taskNodeToAdd = new wxXmlNode(*nodeNew);

      wxXmlNode* pending = AddPendingTaskNode();
      pending->AddChild(taskNodeToAdd);

      CBratTask* bratTaskToAdd = new CBratTask(*bratTaskNew);
      
      AddTaskToMap(bratTaskToAdd);

      m_mapNewBratTask.Insert(bratTaskToAdd->GetUidValue(), bratTaskToAdd);
    }
  }

  return true;
}
//----------------------------------------------------
bool CSchedulerTaskConfig::Save(const wxString& fileName, bool lockFile /* = true */, bool unlockFile  /* = true */, int indentStep /* = 2 */)
{
  //wxMutexError mutexError = MutexLock();

  bool bOk = LockConfigFile(fileName, lockFile);

  wxLogInfo("Saving '%s' ...", fileName.c_str());

  if (!bOk)
  {
    //wxString msg = wxString::Format("Unable to save Brat Scheduler configuration file. Perhaps, it's used by another application - Try again later (internal code is: %d - internal reason is: %s", static_cast<int32_t>(mutexError), MutexErrorToString(mutexError).c_str());
    wxString msg = "Unable to save Brat Scheduler configuration file. Perhaps, it's used by another application - Try again later.";
    throw CException(msg.c_str(), BRATHL_ERROR);
    //::wxMessageBox(msg,
    //               "Warning",
    //                wxOK | wxCENTRE | wxICON_EXCLAMATION);
    //return bOk;
  }

  wxLog::SetActiveTarget(m_logBuffer);

  try
  {

    bOk = wxXmlDocument::Save(fileName, indentStep);
    if (!bOk)
    {
      wxString parserError = m_logBuffer->GetBuffer();
      wxString msg = wxString::Format("Unable to save Brat Scheduler configuration file '%s' -  Native error: '%s'", fileName.c_str(), parserError.c_str());

      wxLog::SetActiveTarget(CSchedulerTaskConfig::m_initialLog);

      throw CException(msg.c_str(), BRATHL_ERROR);

    }
  }
  catch(CException& e)
  {
    //MutexUnLock();
    UnLockConfigFile(unlockFile);
    throw e;
  }
  catch(exception& e)
  {
    //MutexUnLock();
    UnLockConfigFile(unlockFile);
    throw e;
  }
  catch(...)
  {
    //MutexUnLock();
    UnLockConfigFile(unlockFile);
    throw CException("Unexpected error while saving Brat Scheduler configuration file - No Context and no message have been set for this error", BRATHL_ERROR);
  }

  wxLog::SetActiveTarget(CSchedulerTaskConfig::m_initialLog);

  UnLockConfigFile(unlockFile);

  wxLogInfo("'%s' saved ...", fileName.c_str());

  return bOk;
  
}
//----------------------------------------------------
bool CSchedulerTaskConfig::Save(bool lockFile /* = true */, bool unlockFile  /* = true */, int indentStep /* = 2 */)
{  
  return CSchedulerTaskConfig::Save(m_fullFileName, lockFile, unlockFile, indentStep);
}

//----------------------------------------------------
wxXmlNode* CSchedulerTaskConfig::GetPendingTaskNode()
{
  return FindNodeByName(CSchedulerTaskConfig::m_PENDING_ELT, this->GetRoot(), false);
}
//----------------------------------------------------
wxXmlNode* CSchedulerTaskConfig::GetProcessingTaskNode()
{
  return FindNodeByName(CSchedulerTaskConfig::m_PROCESSING_ELT, this->GetRoot(), false);
}
//----------------------------------------------------
wxXmlNode* CSchedulerTaskConfig::GetEndedTaskNode()
{
  return FindNodeByName(CSchedulerTaskConfig::m_ENDED_ELT, this->GetRoot(), false);
}
//----------------------------------------------------
wxXmlNode* CSchedulerTaskConfig::FindNodeByName(const wxString& name, wxXmlNode* parent, bool allDepths /* = false */)
{
  if (parent == NULL)
  {
    return NULL;
  }

  
  //wxXmlNode* node = NULL;
  wxXmlNode* child =  parent->GetChildren();

  while (child != NULL)
  {
    if (child->GetName().CmpNoCase(name) == 0)
    {
      break;
    }
    if ((allDepths) && (child->GetChildren() != NULL))
    {
      wxXmlNode* newChild = FindNodeByName(name, child, allDepths);
      
      if (newChild != NULL)
      {
        child = newChild;
        break;
      }
    }

    child = child->GetNext();
  }

  return child;

}
//----------------------------------------------------
wxXmlNode* CSchedulerTaskConfig::FindTaskNode(const wxLongLong_t& taskId, bool allDepths /* = false */)
{
  return FindTaskNode(taskId, this->GetRoot(), allDepths);
}
//----------------------------------------------------
wxXmlNode* CSchedulerTaskConfig::FindTaskNode(const wxString& taskId, bool allDepths /* = false */)
{
  return FindTaskNode(wxBratTools::wxStringTowxLongLong_t(taskId), this->GetRoot(), allDepths);
}
//----------------------------------------------------
wxXmlNode* CSchedulerTaskConfig::FindTaskNode(const wxString& taskId, wxXmlNode* parent, bool allDepths /* = false */)
{
  return FindTaskNode(wxBratTools::wxStringTowxLongLong_t(taskId), this->GetRoot(), allDepths);
}
//----------------------------------------------------
wxXmlNode* CSchedulerTaskConfig::FindTaskNode(const wxLongLong_t& taskId, wxXmlNode* parent, bool allDepths /* = false */)
{
  if (parent == NULL)
  {
    return NULL;
  }

  wxXmlNode* child =  parent->GetChildren();
  wxString value;

  while (child != NULL)
  {
    if (child->GetName().CmpNoCase(CSchedulerTaskConfig::m_TASK_ELT) == 0)
    {
      bool bOk = child->GetPropVal(CSchedulerTaskConfig::m_TASK_ID_ATTR, &value);
      if (bOk)
      {
        if (wxBratTools::wxStringTowxLongLong_t(value) == taskId)
        {
          break;
        }
      }
    }

    if ((allDepths) && (child->GetChildren() != NULL))
    {
      wxXmlNode* newChild = FindTaskNode(taskId, child, allDepths);
      
      if (newChild != NULL)
      {
        child = newChild;
        break;
      }
    }

    child = child->GetNext();
  }

  return child;

}
//----------------------------------------------------
wxLongLong CSchedulerTaskConfig::GenerateId()
{
  // ensure that no other thread accesses the id
  //wxCriticalSectionLocker locker(m_criSectionMapBratTask);
  m_lastId = wxDateTime::UNow().GetValue();
  //cout << "Enter generate id " << m_lastId.GetValue() << endl;
  CBratTask* bratTask = m_mapBratTask.Find(m_lastId.GetValue());

  while (bratTask != NULL)
  {
    m_lastId++;
    //cout << "Loop generate id " << m_lastId.GetValue() << endl;
    bratTask = m_mapBratTask.Find(m_lastId.GetValue());
  }

  return m_lastId;

}
////----------------------------------------------------
//wxString CSchedulerTaskConfig::GetLastIdAsString(bool generateNewId)
//{
//  if (generateNewId)
//  {
//    GenerateId();
//  }
//
//  return m_lastId.ToString();
//}
////----------------------------------------------------
//wxLongLong_t CSchedulerTaskConfig::GetLastIdAsLongLong(bool generateNewId)
//{
//  if (generateNewId)
//  {
//    GenerateId();
//  }
//
//  return m_lastId.GetValue();
//}

//----------------------------------------------------
wxXmlNode* CSchedulerTaskConfig::AddTaskAsPending(const wxString& cmd, wxDateTime& at, const wxString& name /* = CSchedulerTaskConfig::m_DEFAULT_TASK_NAME */ )
{
  // ensure that no other thread accesses the id
  //wxCriticalSectionLocker locker(m_critSectMapBratTask);

  wxXmlNode* pending = AddPendingTaskNode();
  if (pending == NULL)
  {
    return NULL;
  }

  return AddTaskAsPending(pending, cmd, at, name);
}
//----------------------------------------------------
wxXmlNode* CSchedulerTaskConfig::AddTaskAsPending(wxXmlNode* parent, const wxString& cmd, wxDateTime& at, const wxString& name /* = CSchedulerTaskConfig::m_DEFAULT_TASK_NAME */ )
{
  if (parent == NULL)
  {
    return AddTaskAsPending(cmd, at, name);
  }

  wxXmlNode* taskNode = CreateTaskNodeAsPending(at, name);
  
  if (taskNode == NULL)
  {
    return NULL;
  }

  // ensure that no other thread accesses the tasks' map
  //wxCriticalSectionLocker locker(m_critSectMapBratTask);
  
  taskNode->AddProperty(CSchedulerTaskConfig::m_TASK_CMD_ATTR, cmd);
  
  parent->AddChild(taskNode);

  CBratTask* bratTask = CreateBratTask(taskNode);
  
  if (CSchedulerTaskConfig::IsPendingTaskNode(parent))
  {
    AddTaskToMap(bratTask);
  }
  else
  {
    AddTask(CSchedulerTaskConfig::GetTaskId(parent), bratTask);
  }

  return taskNode;

}
//----------------------------------------------------
wxXmlNode* CSchedulerTaskConfig::AddTaskAsPending(wxXmlNode* parent, const wxString& function, CVectorBratAlgorithmParam& params, wxDateTime& at, const wxString& name /* = CSchedulerTaskConfig::m_DEFAULT_TASK_NAME */ )
{
  if (parent == NULL)
  {
    return NULL;
  }

  wxXmlNode* taskNode = CreateTaskNodeAsPending(at, name);
  
  if (taskNode == NULL)
  {
    return NULL;
  }

  // ensure that no other thread accesses the tasks' map
  //wxCriticalSectionLocker locker(m_critSectMapBratTask);
  
  taskNode->AddProperty(CSchedulerTaskConfig::m_TASK_FUNCTION_ATTR, function);
 
  CreateArgNode(taskNode, params);

  parent->AddChild(taskNode);

  CBratTask* bratTask = CreateBratTask(taskNode);
  
  if (CSchedulerTaskConfig::IsPendingTaskNode(parent))
  {
    AddTaskToMap(bratTask);
  }
  else
  {
    AddTask(CSchedulerTaskConfig::GetTaskId(parent), bratTask);
  }

  return taskNode;


}

//----------------------------------------------------
wxXmlNode* CSchedulerTaskConfig::CreateTaskNodeAsPending(wxDateTime& at, const wxString& name /* = CSchedulerTaskConfig::m_DEFAULT_TASK_NAME */ )
{

  // ensure that no other thread accesses 
  wxCriticalSectionLocker locker(m_critSectXmlConfigFile);
  
  wxXmlNode* taskNode = new wxXmlNode(wxXML_ELEMENT_NODE, CSchedulerTaskConfig::m_TASK_ELT);
  wxLongLong id = GenerateId();
  
  
  taskNode->AddProperty(CSchedulerTaskConfig::m_TASK_ID_ATTR, id.ToString());

  if (!name.IsEmpty())
  {
    taskNode->AddProperty(CSchedulerTaskConfig::m_TASK_NAME_ATTR, name);
  }

  wxString fullLogFileName = wxString::Format("%s/BratTask_%s_%s.log", CSchedulerTaskConfig::GetLogFilePath(m_CONFIG_APPNAME).c_str(), name.c_str(), id.ToString().c_str());
  taskNode->AddProperty(CSchedulerTaskConfig::m_TASK_LOG_FILE_ATTR, fullLogFileName);
  
  taskNode->AddProperty(CSchedulerTaskConfig::m_TASK_AT_ATTR, at.Format(CBratTask::FormatISODateTime));
  
  taskNode->AddProperty(CSchedulerTaskConfig::m_TASK_STATUS_ATTR, CBratTask::m_BRAT_STATUS_PENDING_LABEL);

  return taskNode;

}
//----------------------------------------------------
wxXmlNode* CSchedulerTaskConfig::CreateArgNode(wxXmlNode* parent, CVectorBratAlgorithmParam& params)
{

  if (parent == NULL)
  {
    return NULL;
  }

  CVectorBratAlgorithmParam::iterator it;

  for(it = params.begin() ; it != params.end() ; it++)
  {
    wxXmlNode* argNode = new wxXmlNode(wxXML_ELEMENT_NODE, CSchedulerTaskConfig::m_ARG_ELT);
    argNode->AddProperty(CSchedulerTaskConfig::m_TYPE_ATTR, it->GetTypeValAsString().c_str());
    wxXmlNode* argTextNode = new wxXmlNode(wxXML_TEXT_NODE, "");
    argNode->AddChild(argTextNode);
    try
    {
      argTextNode->SetContent(it->GetValue().c_str());
    }
    catch(CException& e)
    {
      argTextNode->SetContent(e.GetMessage().c_str());
    }
  
    parent->AddChild(argNode);
  }
   
  return parent;

}
//----------------------------------------------------
bool CSchedulerTaskConfig::IsPendingTaskNode(wxXmlNode* node)
{
  if (node == NULL)
  {
    return false;
  }

  return (node->GetName().CmpNoCase(CSchedulerTaskConfig::m_PENDING_ELT) == 0);
}
//----------------------------------------------------
wxString CSchedulerTaskConfig::GetTaskIdAsString(wxXmlNode* node)
{
  wxString value;

  if (node == NULL)
  {
    return value;
  }

  if (node->GetName().CmpNoCase(CSchedulerTaskConfig::m_TASK_ELT) != 0)
  {
    return value;
  }

  bool bOk = node->GetPropVal(CSchedulerTaskConfig::m_TASK_ID_ATTR, &value);

  if (!bOk)
  {
    return value;
  }

  return value;
}
//----------------------------------------------------
wxLongLong_t CSchedulerTaskConfig::GetTaskId(wxXmlNode* node)
{
  wxString value = GetTaskIdAsString(node);
    
  return wxBratTools::wxStringTowxLongLong_t(value);
}
//----------------------------------------------------
bool CSchedulerTaskConfig::RemoveTask(wxXmlNode*& node)
{
  // ensure that no other thread accesses the id
  wxCriticalSectionLocker locker(m_critSectXmlConfigFile);

  if (node == NULL)
  {
    return false;
  }

  if (node->GetName().CmpNoCase(CSchedulerTaskConfig::m_TASK_ELT) != 0)
  {
    return false;
  }

  wxString uid;

  bool bOk = node->GetPropVal(CSchedulerTaskConfig::m_TASK_ID_ATTR, &uid);

  if (!bOk)
  {
    return false;
  }

  if (uid.IsEmpty())
  {
    return false;
  }

  wxString status;

  bOk = node->GetPropVal(CSchedulerTaskConfig::m_TASK_STATUS_ATTR, &status);

  if (status.CmpNoCase(CBratTask::m_BRAT_STATUS_PROCESSING_LABEL) == 0)
  {
      wxString msg = wxString::Format("Task id '%s' has status '%s'. It can't be removed.", 
                                      uid.c_str(),
                                      status.c_str());

      throw CException(msg.c_str(), BRATHL_ERROR);
  }

  wxXmlNode* parent = node->GetParent();
  if (parent == NULL)
  {
    return false;
  }

  bool removed = parent->RemoveChild(node);
  delete node;
  node = NULL;

  if (removed)
  {
    RemoveTaskFromMap(wxBratTools::wxStringTowxLongLong_t(uid));
  }

  return removed;
}
//----------------------------------------------------
bool CSchedulerTaskConfig::RemoveTask(const wxLongLong& id)
{
  return RemoveTask(id.GetValue());
}
//----------------------------------------------------
bool CSchedulerTaskConfig::RemoveTask(wxLongLong_t id)
{
  wxXmlNode* root = this->GetRoot();

  wxXmlNode* node = FindTaskNode(id, root, true);

  // Warning: node is set to NULL if removed
  return RemoveTask(node);

}
//----------------------------------------------------
bool CSchedulerTaskConfig::RemoveTaskFromMap(CBratTask* bratTask)
{
  if (bratTask == NULL)
  {
    return false;
  }

  return RemoveTaskFromMap(bratTask->GetUidValue());
}

//----------------------------------------------------
CBratTask* CSchedulerTaskConfig::FindTaskFromMap(wxLongLong_t id)
{
  // ensure that no other thread accesses the id
  wxCriticalSectionLocker locker(m_critSectMapBratTask);
  
  return m_mapBratTask.Find(id);

}
//----------------------------------------------------
bool CSchedulerTaskConfig::RemoveTaskFromMap(wxLongLong_t id)
{
  // ensure that no other thread accesses the id
  wxCriticalSectionLocker locker(m_critSectMapBratTask);
  
  wxLongLong idObj(id);
  wxString idAsString = idObj.ToString();

  CBratTask* bratTask = m_mapBratTask.Find(id);
  if (bratTask == NULL)
  {
    return false;
  }

  CBratTask::bratTaskStatus status = bratTask->GetStatus();

  if (status == CBratTask::BRAT_STATUS_PROCESSING)
  {
      wxString msg = wxString::Format("Task id '%s' has status %d (%s). It can't be removed.", 
                                      idAsString.c_str(),
                                      static_cast<int32_t>(status), 
                                      bratTask->GetStatusAsString().c_str());

      throw CException(msg.c_str(), BRATHL_ERROR);
  }

  m_mapPendingBratTask.Remove(id);
  m_mapProcessingBratTask.Remove(id);
  m_mapEndedBratTask.Remove(id);
  
  // Remove log file
  wxBratTools::RemoveFile(bratTask->GetLogFile()->GetFullPath());

  m_mapBratTask.Remove(id);


  return true;
}
//----------------------------------------------------
void CSchedulerTaskConfig::AddTask(wxLongLong_t parentId, CBratTask* bratTask)
{

  if (bratTask == NULL)
  {
    return;
  }

  wxCriticalSectionLocker locker(m_critSectMapBratTask);
  CBratTask* parentBratTask = m_mapBratTask.Find(parentId);
  if (parentBratTask == NULL)
  {
    return;
  }

  parentBratTask->GetSubordinateTasks()->Insert(bratTask);

}
//----------------------------------------------------
void CSchedulerTaskConfig::AddTaskToMap(CBratTask* bratTask)
{
  if (bratTask == NULL)
  {
    return;
  }

  wxLongLong_t uid = bratTask->GetUidValue();
  CBratTask::bratTaskStatus status = bratTask->GetStatus();

  wxCriticalSectionLocker locker(m_critSectMapBratTask);
  m_mapBratTask.Insert(uid, bratTask);
  switch (status)
  {
    case CBratTask::BRAT_STATUS_PENDING:
    {
      m_mapPendingBratTask.Insert(uid, bratTask);
      break;
    }
    case CBratTask::BRAT_STATUS_PROCESSING:
    {
      m_mapProcessingBratTask.Insert(uid, bratTask);
      break;
    }
    case CBratTask::BRAT_STATUS_ENDED:
    case CBratTask::BRAT_STATUS_ERROR:
    case CBratTask::BRAT_STATUS_WARNING:
    {
      m_mapEndedBratTask.Insert(uid, bratTask);
      break;
    }
    default:
    {
      wxString msg = wxString::Format("ERROR in CSchedulerTaskConfig#AddTaskToMap - status %d (%s) not implemented in the function.", 
                                      static_cast<int32_t>(status), bratTask->GetStatusAsString().c_str());

      throw CException(msg.c_str(), BRATHL_ERROR);
    }
  }

}
//----------------------------------------------------
wxXmlNode* CSchedulerTaskConfig::AddRootNode()
{
  wxXmlNode* root = this->GetRoot();
  if (root == NULL)
  {
    root = new wxXmlNode(wxXML_ELEMENT_NODE, CSchedulerTaskConfig::m_ROOT_ELT);
    this->SetRoot(root);
  }

  return root;
}
//----------------------------------------------------
wxXmlNode* CSchedulerTaskConfig::AddPendingTaskNode()
{
  wxXmlNode* root = this->AddRootNode();

  wxXmlNode* node = GetPendingTaskNode();
  if (node == NULL)
  {
    node = new wxXmlNode(wxXML_ELEMENT_NODE, CSchedulerTaskConfig::m_PENDING_ELT);
    root->AddChild(node);
  }

  return node;
}
//----------------------------------------------------
wxXmlNode* CSchedulerTaskConfig::AddProcessingTaskNode()
{
  wxXmlNode* root = this->AddRootNode();

  wxXmlNode* node = GetProcessingTaskNode();
  if (node == NULL)
  {
    node = new wxXmlNode(wxXML_ELEMENT_NODE, CSchedulerTaskConfig::m_PROCESSING_ELT);
    root->AddChild(node);
  }

  return node;
}
//----------------------------------------------------
wxXmlNode* CSchedulerTaskConfig::AddEndedTaskNode()
{
  wxXmlNode* root = this->AddRootNode();

  wxXmlNode* node = GetEndedTaskNode();
  if (node == NULL)
  {
    node = new wxXmlNode(wxXML_ELEMENT_NODE, CSchedulerTaskConfig::m_ENDED_ELT);
    root->AddChild(node);
  }

  return node;
}
//----------------------------------------
CBratTask* CSchedulerTaskConfig::CreateBratTask(wxXmlNode* taskNode) 
{
  if (taskNode == NULL)
  {
    return NULL;
  }

  CBratTask* bratTask = new CBratTask();

  wxString value;
  bool bOk = true;

  bOk = taskNode->GetPropVal(CSchedulerTaskConfig::m_TASK_ID_ATTR, &value);
  if (!bOk)
  {
    wxString msg = wxString::Format("Unable to create Brat task - Task's id is empty - "
      "Please check all '%s' attributes of the '%s' elements in the Brat Scheduler configuration file '%s'.",
      CSchedulerTaskConfig::m_TASK_ID_ATTR.c_str(),
      CSchedulerTaskConfig::m_TASK_ELT.c_str(),
      m_fullFileName.c_str());

    delete bratTask;
    bratTask = NULL;

    throw CException(msg.c_str(), BRATHL_ERROR);
  }

  bratTask->SetUid(value);

  bOk = taskNode->GetPropVal(CSchedulerTaskConfig::m_TASK_NAME_ATTR, &value);
  if (bOk)
  {
    bratTask->SetName(value);
  }

  bOk = taskNode->GetPropVal(CSchedulerTaskConfig::m_TASK_CMD_ATTR, &value);
  if (bOk)
  {
    bratTask->SetCmd(value);
  }
  else
  {
    bOk = taskNode->GetPropVal(CSchedulerTaskConfig::m_TASK_FUNCTION_ATTR, &value);
    if (bOk)
    {
      CBratTaskFunction* functionRef = CMapBratTaskFunction::GetInstance().Find(value);
      if (functionRef == NULL)
      {
        wxString msg = wxString::Format("Unable to create Brat task - Function '%s' is unknown"
          "Please check '%s' attribute of the '%s' element for task id '%s' in the Brat Scheduler configuration file '%s'.",
          value.c_str(),
          CSchedulerTaskConfig::m_TASK_FUNCTION_ATTR.c_str(),
          CSchedulerTaskConfig::m_TASK_ELT.c_str(),
          bratTask->GetUidValueAsString().c_str(),
          m_fullFileName.c_str());

        delete bratTask;
        bratTask = NULL;

        throw CException(msg.c_str(), BRATHL_ERROR);
      }

      bratTask->SetBratTaskFunction(functionRef);
      
      wxXmlNode* argNode = taskNode->GetChildren();
      CVectorBratAlgorithmParam* params = bratTask->GetBratTaskFunction()->GetParams();
      while (argNode != NULL)
      {
        if (argNode->GetName().CmpNoCase(CSchedulerTaskConfig::m_ARG_ELT) == 0)
        {
          wxXmlNode* argTextNode = argNode->GetChildren();
          if (argTextNode != NULL)
          {
            params->Insert(argTextNode->GetContent().c_str());
          }
        }

        argNode = argNode->GetNext();
      }

    }

  }
  if (!bOk)
  {
    wxString msg = wxString::Format("Unable to create Brat task - Neither Task's command-line nor task's function have been set"
      "Please check '%s' attribute of the '%s' element for task id '%s' in the Brat Scheduler configuration file '%s'.",
      CSchedulerTaskConfig::m_TASK_CMD_ATTR.c_str(),
      CSchedulerTaskConfig::m_TASK_ELT.c_str(),
      bratTask->GetUid().ToString().c_str(),
      m_fullFileName.c_str());

    delete bratTask;
    bratTask = NULL;

    throw CException(msg.c_str(), BRATHL_ERROR);
  }

  bOk = taskNode->GetPropVal(CSchedulerTaskConfig::m_TASK_AT_ATTR, &value);
  if (!bOk)
  {
    wxString msg = wxString::Format("Unable to create Brat task - Task's scheduled date/time is empty - "
      "Please check '%s' attribute of the '%s' element for task id '%s' in the Brat Scheduler configuration file '%s'.",
      CSchedulerTaskConfig::m_TASK_AT_ATTR.c_str(),
      CSchedulerTaskConfig::m_TASK_ELT.c_str(),
      bratTask->GetUid().ToString().c_str(),
      m_fullFileName.c_str());

    delete bratTask;
    bratTask = NULL;

    throw CException(msg.c_str(), BRATHL_ERROR);
  }

  wxDateTime dt;
  wxString error;

  bOk = ParseDateTime(value, dt, error);
  if (!bOk)
  {
    wxString msg = wxString::Format("Unable to parse date '%s' for Brat Scheduler task id %s (cmd is '%s')", value.c_str(), bratTask->GetUid().ToString().c_str(), bratTask->GetCmd().c_str());
    
    delete bratTask;
    bratTask = NULL;
    
    throw CException(msg.c_str(), BRATHL_ERROR);
  }

  bratTask->SetAt(dt);

  bOk = taskNode->GetPropVal(CSchedulerTaskConfig::m_TASK_STATUS_ATTR, &value);
  if (!bOk)
  {
    value = CBratTask::m_BRAT_STATUS_PENDING_LABEL;
    //wxString msg = wxString::Format("Unable to create Brat task - Task's scheduled status is empty - "
    //  "Please check '%s' attribute of the '%s' element for task id '%s' in the Brat Scheduler configuration file '%s'.",
    //  CSchedulerTaskConfig::m_TASK_STATUS_ATTR.c_str(),
    //  CSchedulerTaskConfig::m_TASK_ELT.c_str(),
    //  bratTask->GetUid().ToString().c_str(),
    //  m_fullFileName.c_str());

    //delete bratTask;
    //bratTask = NULL;

    //throw CException(msg.c_str(), BRATHL_ERROR);
  }

  bratTask->SetStatus(value);

  bOk = taskNode->GetPropVal(CSchedulerTaskConfig::m_TASK_LOG_FILE_ATTR, &value);
  if (!bOk)
  {
    wxString msg = wxString::Format("Unable to create Brat task - Log file is empty - "
      "Please check '%s' attribute of the '%s' element for task id '%s' in the Brat Scheduler configuration file '%s'.",
      CSchedulerTaskConfig::m_TASK_LOG_FILE_ATTR.c_str(),
      CSchedulerTaskConfig::m_TASK_ELT.c_str(),
      bratTask->GetUid().ToString().c_str(),
      m_fullFileName.c_str());

    delete bratTask;
    bratTask = NULL;

    throw CException(msg.c_str(), BRATHL_ERROR);
  }

  bratTask->SetLogFile(value);


  return bratTask;
}
//----------------------------------------
bool CSchedulerTaskConfig::ParseDateTime(const wxString& value, wxDateTime& dt, wxString& error) 
{
  bool bOk = true;

  wxLog::SetActiveTarget(CSchedulerTaskConfig::m_logBuffer);

  const wxChar* result = dt.ParseDateTime(value);
  if (result == NULL)
  {
    error = m_logBuffer->GetBuffer();
    bOk = false;
  }

  wxLog::SetActiveTarget(CSchedulerTaskConfig::m_initialLog);
  return bOk;

}
//----------------------------------------
bool CSchedulerTaskConfig::LoadSchedulerTaskConfig(bool quiet /* = false */)
{
  bool loaded = false;
  bool cancelled = false;

  do
  {
    try
    {
      CSchedulerTaskConfig::GetInstance(true, true, false);
      loaded = true;
    }
    catch(CException& e)
    {
      //-----------------------
      if (quiet)
      {
        CSchedulerTaskConfig::ForceUnLockConfigFile();

        throw e;
      }
      //-----------------------

      loaded = false;
      wxString msg = wxString::Format("%s\n Try again ?", e.GetMessage().c_str());
      int32_t result = wxMessageBox(msg,
                     "Warning",
                      wxYES_NO | wxCENTRE | wxICON_EXCLAMATION);
      
      if (result != wxYES)
      {
        cancelled = true;;
      }

    }
  } while (!loaded && !cancelled);
  
  if (!loaded)
  {    
    wxString msg = "Operation cancelled";
    int32_t result = wxMessageBox(msg,
                   "Warning",
                    wxOK | wxCENTRE | wxICON_EXCLAMATION);
    
    CSchedulerTaskConfig::ForceUnLockConfigFile();

  }

  return loaded;

}
//----------------------------------------
bool CSchedulerTaskConfig::SaveSchedulerTaskConfig(bool quiet /* = false */)
{
  bool saved = false;
  bool cancelled = false;
  
  do
  {
    try
    {
      saved = CSchedulerTaskConfig::GetInstance()->Save(false, true);    
    }
    catch(CException& e)
    {
      //-----------------------
      if (quiet)
      {
        CSchedulerTaskConfig::ForceUnLockConfigFile();
        throw e;
      }
      wxString msg = wxString::Format("%s\n Try again ?", e.GetMessage().c_str());
      int32_t result = wxMessageBox(msg,
                     "Warning",
                      wxYES_NO | wxCENTRE | wxICON_EXCLAMATION);
      
      if (result != wxYES)
      {
        cancelled = true;;
      }

    }
  } while (!saved && !cancelled);
  

  if (!saved)
  {
    CSchedulerTaskConfig::ForceUnLockConfigFile();

    wxString msg = "Operation cancelled";
    int32_t result = wxMessageBox(msg,
                   "Warning",
                    wxOK | wxCENTRE | wxICON_EXCLAMATION);
  }

  return saved;

}
//----------------------------------------
void CSchedulerTaskConfig::ForceUnLockConfigFile()
{
  if (CSchedulerTaskConfig::IsLoaded())
  {
    CSchedulerTaskConfig::GetInstance()->UnLockConfigFile(true);
  }

}
//----------------------------------------
void CSchedulerTaskConfig::GetMapPendingBratTaskToProcess(const wxDateTime& dateRef, vector<wxLongLong_t>* vectorBratTaskToProcess)
{
  if (vectorBratTaskToProcess == NULL)
  {
    return;
  }

  wxCriticalSectionLocker locker(m_critSectMapBratTask);

  CMapBratTask::const_iterator it;

  for (it = m_mapPendingBratTask.begin() ; it != m_mapPendingBratTask.end() ; it++)
  {
    CBratTask* bratTask = it->second;
    if (bratTask == NULL)
    {
      continue;
    }

    if (bratTask->GetAt() > dateRef)
    {
      continue;
    }
    
    vectorBratTaskToProcess->push_back(bratTask->GetUidValue());

  }

}
//----------------------------------------
bool CSchedulerTaskConfig::SetPropVal(wxXmlNode* node, const wxString& propName, const wxString& value, bool allDepths /* = true */)
{
  bool bOk = false;

  if (node == NULL)
  {
    return bOk;
  }

  wxXmlProperty *prop = node->GetProperties();

  while (prop)
  {
      if (prop->GetName() == propName)
      {
          prop->SetValue(value);
          bOk = true;
          break;
      }

      prop = prop->GetNext();
  }

  if (allDepths)
  {
    this->SetPropVal(node->GetChildren(), propName, value, allDepths);
  }

  return bOk;

  //if (bOk)
  //{
  //  return bOk;
  //}

  //node->AddProperty(propName, value);

  //return true;
}

//----------------------------------------
