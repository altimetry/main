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

#ifndef __SchedulerTaskConfig_H__
#define __SchedulerTaskConfig_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "SchedulerTaskConfig.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "brathl.h"

#include "wx/snglinst.h" // (wxSingleInstanceChecker class)
#include <wx/xml/xml.h> // (wxXmlDocument, wxXmlNode, wxXmlProperty classes)

#include "Date.h"
#include "Tools.h"

using namespace brathl;

#include "BratTask.h"



// WDR: class declarations

//----------------------------------------------------------------------------
// CSchedulerTaskConfig
//----------------------------------------------------------------------------

class CSchedulerTaskConfig: public wxXmlDocument
{

protected:
   // constructors and destructors
  CSchedulerTaskConfig();

  CSchedulerTaskConfig(const wxXmlDocument& doc);


  CSchedulerTaskConfig(const wxString &filename, bool lockFile = true, bool unlockFile = true, const wxString &encoding = "UTF-8");

 	CSchedulerTaskConfig(wxInputStream &stream, const wxString &encoding = "UTF-8");

public:
  
  CSchedulerTaskConfig(const CSchedulerTaskConfig& doc);

  virtual ~CSchedulerTaskConfig ();

  CSchedulerTaskConfig& operator=(const CSchedulerTaskConfig& doc);

  const wxString& GetFullFileName() { return m_fullFileName; };

  virtual bool Save(const wxString& fileName, bool lockFile = true, bool unlockFile = true, int indentStep = 2);
  bool Save(bool lockFile = true, bool unlockFile = true, int indentStep = 2);
  bool Load(const wxString& fileName, bool lockFile = true, bool unlockFile = true, const wxString& encoding = wxT("UTF-8"), int flags = wxXMLDOC_NONE);
  bool Load(bool lockFile = true, bool unlockFile = true, const wxString& encoding = wxT("UTF-8"), int flags = wxXMLDOC_NONE);

  bool IsReloadAll() { return m_reloadAll; };
  void SetReloadAll(bool value) { m_reloadAll = value; };
  
  static bool IsLoaded() { return (CSchedulerTaskConfig::m_instance != NULL); };
  
  bool Reload(bool lockFile = true, bool unlockFile = true);

  wxXmlNode* GetPendingTaskNode();
  wxXmlNode* GetEndedTaskNode();
  wxXmlNode* GetProcessingTaskNode();

  wxXmlNode* AddRootNode();
  wxXmlNode* AddPendingTaskNode();
  wxXmlNode* AddProcessingTaskNode();
  wxXmlNode* AddEndedTaskNode();

  wxXmlNode* CreateTaskNodeAsPending(wxDateTime& at, const wxString& name = CSchedulerTaskConfig::m_DEFAULT_TASK_NAME);
  wxXmlNode* CreateArgNode(wxXmlNode* parent, CVectorBratAlgorithmParam& params);

  wxXmlNode* AddTaskAsPending(const wxString& cmd, wxDateTime& at, const wxString& name = CSchedulerTaskConfig::m_DEFAULT_TASK_NAME);
  wxXmlNode* AddTaskAsPending(wxXmlNode* parent, const wxString& cmd, wxDateTime& at, const wxString& name = CSchedulerTaskConfig::m_DEFAULT_TASK_NAME);

  wxXmlNode* AddTaskAsPending(wxXmlNode* parent, const wxString& function, CVectorBratAlgorithmParam& params, wxDateTime& at, const wxString& name = CSchedulerTaskConfig::m_DEFAULT_TASK_NAME);

  void AddTask(wxLongLong_t parentId, CBratTask* bratTask);

  void AddTaskToMap(CBratTask* bratTask);
  bool RemoveTaskFromMap(CBratTask* bratTask);
  bool RemoveTaskFromMap(wxLongLong_t id);

  wxXmlNode* FindNodeByName(const wxString& name, wxXmlNode* parent, bool allDepths = false);
  
  wxXmlNode* FindTaskNode(const wxLongLong_t& taskId, bool allDepths = false);
  wxXmlNode* FindTaskNode(const wxString& taskId, bool allDepths = false);
  wxXmlNode* FindTaskNode(const wxString& taskId, wxXmlNode* parent, bool allDepths = false);
  wxXmlNode* FindTaskNode(const wxLongLong_t& taskId, wxXmlNode* parent, bool allDepths = false);

  CBratTask* CreateBratTask(wxXmlNode* taskNode);

  void RemoveMapBratTasks();

  bool RemoveTask(wxXmlNode*& node);
  bool RemoveTask(const wxLongLong& id);
  bool RemoveTask(wxLongLong_t id);
  
  bool ChangeProcessingToPending(CVectorBratTask& vectorTasks);
  
  void ChangeTaskStatus(CBratTask* bratTask, CBratTask::bratTaskStatus newStatus);
  void ChangeTaskStatus(wxLongLong_t id, CBratTask::bratTaskStatus newStatus);
  void ChangeTaskStatusFromXml(wxLongLong_t id, CBratTask::bratTaskStatus newStatus);
  CBratTask::bratTaskStatus ChangeTaskStatusFromMap(wxLongLong_t id, CBratTask::bratTaskStatus newStatus);

  void LoadTasks();
  
  bool LoadTasks(wxXmlNode* node);

  bool LoadPendingTasks();
  bool LoadEndedTasks();
  bool LoadProcessingTasks();

  bool LoadSubordinateTasks(wxXmlNode* taskNode, CBratTask* bratTask);
  
  bool SetPropVal(wxXmlNode* node, const wxString& propName, const wxString& value, bool allDepths = true);

  CBratTask* FindTaskFromMap(wxLongLong_t id);

  CMapBratTask* GetMapBratTask() { return &m_mapBratTask; };
  CMapBratTask* GetMapPendingBratTask() { return &m_mapPendingBratTask; };
  CMapBratTask* GetMapProcessingBratTask() { return &m_mapProcessingBratTask; };
  CMapBratTask* GetMapEndedBratTask() { return &m_mapEndedBratTask; };
  CMapBratTask* GetMapNewBratTask() { return &m_mapNewBratTask; };

  bool HasMapNewBratTask() { return (m_mapNewBratTask.size() > 0); };

  bool LockConfigFile(bool lockFile = true);
  bool LockConfigFile(const wxString& fileName, bool lockFile = true);
  void UnLockConfigFile(bool unlockFile);

  static bool LoadSchedulerTaskConfig(bool quiet = false);
  static bool SaveSchedulerTaskConfig(bool quiet = false);

  void GetMapPendingBratTaskToProcess(const wxDateTime& dateref,  vector<wxLongLong_t>* vectorBratTaskToProcess);

  //wxMutexError MutexLock();
  //wxMutexError MutexUnLock();

  static wxString GetConfigFilePath();
  static wxString GetConfigFilePath(const wxString& appName);

  static wxString GetLogFilePath();
  static wxString GetLogFilePath(const wxString& appName);

  static wxString GetUserDataDir();
  static wxString GetUserConfigDir();
  static wxString GetUserConfigDir(const wxString& appName);

  static bool IsPendingTaskNode(wxXmlNode* node);

  static wxString GetTaskIdAsString(wxXmlNode* node);
  static wxLongLong_t GetTaskId(wxXmlNode* node);

  static void ForceUnLockConfigFile();

  static CSchedulerTaskConfig* GetInstance(bool reload = false, bool lockFile = true, bool unlockFile = true, const wxString& encoding = "UTF-8");
  static CSchedulerTaskConfig* GetInstance(const wxString* fileName, bool reload = false , bool lockFile = true, bool unlockFile = true, const wxString& encoding = "UTF-8");
  //static CSchedulerTaskConfig* GetInstance(const string* fileName, bool reload = false, const wxString& encoding = "UTF-8");

  bool ParseDateTime(const wxString& value, wxDateTime& dt, wxString& error);


    // WDR: method declarations for SchedulerTaskConfig


public:

  static const wxString m_ENDED_ELT;
  static const wxString m_PENDING_ELT;
  static const wxString m_PROCESSING_ELT;
  static const wxString m_ROOT_ELT;
  static const wxString m_TASK_ELT;
  static const wxString m_ARG_ELT;

  static const wxString m_TASK_ID_ATTR;
  static const wxString m_TASK_CMD_ATTR;
  static const wxString m_TASK_FUNCTION_ATTR;
  static const wxString m_TASK_AT_ATTR;
  static const wxString m_TASK_LOG_FILE_ATTR;
  static const wxString m_TASK_NAME_ATTR;
  static const wxString m_TASK_RETURN_CODE_ATTR;
  static const wxString m_TASK_STATUS_ATTR;
  static const wxString m_TYPE_ATTR;

  static const wxString m_DEFAULT_TASK_NAME;;

  static wxString m_CONFIG_APPNAME;




protected:
  //----------------------------------------------------------------------------
  // CSmartCleaner
  //----------------------------------------------------------------------------
  
  class CSmartCleaner
  {
    // the cleaner stores the address of the instance pointer
    // for a singleton object. The CSmartCleaner MUST be
    // instanciated as a static object in a function, which
    // ensures it is destroyed after exit, but is safe if
    // the singleton object IS explicitly deleted.

    protected :
      CSchedulerTaskConfig **m_pAddressOfSchedulerTaskConfigPtr;
      wxLogBuffer **m_pAddressOfSchedulerTaskConfigLogger;


    public :
      CSmartCleaner( CSchedulerTaskConfig **addressOfSchedulerTaskConfigPtr, wxLogBuffer **addressOfSchedulerTaskConfigLogger) 
        : m_pAddressOfSchedulerTaskConfigPtr( addressOfSchedulerTaskConfigPtr ),
          m_pAddressOfSchedulerTaskConfigLogger( addressOfSchedulerTaskConfigLogger ) 
      {
      }
      
      virtual ~CSmartCleaner()
      {
        //cout << "Enter ~CSmartCleaner" << endl;

        if( *m_pAddressOfSchedulerTaskConfigLogger != NULL )
        {
          delete *m_pAddressOfSchedulerTaskConfigLogger;
          *m_pAddressOfSchedulerTaskConfigLogger = NULL;
        }

        if( *m_pAddressOfSchedulerTaskConfigPtr != NULL )
        {
          delete *m_pAddressOfSchedulerTaskConfigPtr;
          *m_pAddressOfSchedulerTaskConfigPtr = NULL;

        }
      }

      CSchedulerTaskConfig * GetObject() {return *m_pAddressOfSchedulerTaskConfigPtr;}
      wxLogBuffer * GetObjectLogger() {return *m_pAddressOfSchedulerTaskConfigLogger;}
        
  };

protected:

  bool m_reloadAll;

  wxString m_fullFileName;
  wxDateTime m_dateTimeFullFileNameMod;

  // Map for all tasks (the reference map)
  CMapBratTask m_mapBratTask;
  // Map for pending tasks - Warning : don't delete object stored in it.
  CMapBratTask m_mapPendingBratTask;
  // Map for processing tasks - Warning : don't delete object stored in it.
  CMapBratTask m_mapProcessingBratTask;
  // Map for ended (or error) tasks - Warning : don't delete object stored in it.
  CMapBratTask m_mapEndedBratTask;
  // Map used to stored new pending tasks when file is reloaded.
  // Tasks are just added as reference - Warning : don't delete object stored in it.
  CMapBratTask m_mapNewBratTask;

  wxLongLong m_lastId;

  static const wxString m_CONFIG_FILE_NAME;
  static const wxString m_LOG_REL_PATH;


  static CSchedulerTaskConfig* m_instance;
  static wxLogBuffer* m_logBuffer;

  // Initial reference of the wxLog object - Don't delete
  static wxLog* m_initialLog;

  static wxCriticalSection m_critSectSchedulerTaskConfigInstance;

  wxCriticalSection m_critSectXmlConfigFile;
  wxCriticalSection m_critSectMapBratTask;

  wxCriticalSection m_critSectConfigFileChecker;

  //wxMutex m_mutex;
  // m_configFileChecker is use to lock and get exclusive access to the configuration file.
  // In the wxWoidgets documentation, we cant find that :
  // "...A critical section object is used for exactly the same purpose as a wxMutex.
  // The only difference is that under Windows platform critical sections are only visible inside one process,
  // while mutexes may be shared among processes, so using critical sections is slightly more efficient."
  // But wxMutex use 'unnamed' mutex for Windows, so in this case mutex are only intra-process and not inter-process.
  // To bypass this, we use wxSingleInstanceChecker interface to  lock and get exclusive access to the configuration file,
  // because under Windows wxSingleInstanceChecker implementation use 'named' mutex.
  wxSingleInstanceChecker* m_configFileChecker;

    // WDR: member variable declarations for SchedulerTaskConfig
    
protected:
    void Init();
    void Set(const CSchedulerTaskConfig& o);

    void NewConfigFileChecker(const wxString& fileName);
    void DeleteConfigFileChecker();
    
    void DeleteRootNode();

    void Delete(CSchedulerTaskConfig*& schedulerTaskConfig);

    bool Synchronize(CSchedulerTaskConfig* sched);
    
    bool AddNewTask(CSchedulerTaskConfig* sched);
    
    void AddTask(const CMapBratTask* mapBratTask);

    wxLongLong GenerateId();
    //wxString GetLastIdAsString(bool generateNewId);
    //wxLongLong_t GetLastIdAsLongLong(bool generateNewId);
    

    static bool CreateXmlFile(const wxString& fileName);

    static void PrepareSmartCleaner();

    // WDR: handler declarations for SchedulerTaskConfig

};






#endif
