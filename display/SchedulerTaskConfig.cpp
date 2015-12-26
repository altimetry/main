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
#include <cerrno>

#include "new-gui/brat/stdafx.h"

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/stdpaths.h"
#include "wx/filename.h"

#include "wxBratTools.h"

#include "SchedulerTaskConfig.h"


// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"


// WDR: class implementations



wxCriticalSection CSchedulerTaskConfig::m_critSectSchedulerTaskConfigInstance;

std::string CSchedulerTaskConfig::m_CONFIG_APPNAME = "BratScheduler";

const std::string CSchedulerTaskConfig::m_CONFIG_FILE_NAME = "BratSchedulerTasksConfig.xml";
const std::string CSchedulerTaskConfig::m_LOG_REL_PATH = "Logs";

//CSchedulerTaskConfig* CSchedulerTaskConfig::mInstance = NULL;
wxLogBuffer* CSchedulerTaskConfig::m_logBuffer = NULL;
wxLog* CSchedulerTaskConfig::m_initialLog = NULL;

//const std::string CSchedulerTaskConfig::m_DEFAULT_TASK_NAME = "unnamed";

const std::string CSchedulerTaskConfig::m_ENDED_ELT = "endedTasks";
const std::string CSchedulerTaskConfig::m_PENDING_ELT = "pendingTasks";
const std::string CSchedulerTaskConfig::m_PROCESSING_ELT = "processingTasks";
const std::string CSchedulerTaskConfig::m_ROOT_ELT = "bratSchedulerConfig";
const std::string CSchedulerTaskConfig::m_TASK_ELT = "task";
const std::string CSchedulerTaskConfig::m_ARG_ELT = "arg";

const std::string CSchedulerTaskConfig::m_TASK_AT_ATTR = "at";
const std::string CSchedulerTaskConfig::m_TASK_CMD_ATTR = "cmd";
const std::string CSchedulerTaskConfig::m_TASK_FUNCTION_ATTR = "function";
const std::string CSchedulerTaskConfig::m_TASK_ID_ATTR = "uid";
const std::string CSchedulerTaskConfig::m_TASK_LOG_FILE_ATTR = "logFile";
const std::string CSchedulerTaskConfig::m_TASK_NAME_ATTR = "name";
const std::string CSchedulerTaskConfig::m_TASK_RETURN_CODE_ATTR = "returnCode";
const std::string CSchedulerTaskConfig::m_TASK_STATUS_ATTR = "status";
const std::string CSchedulerTaskConfig::m_TYPE_ATTR = "type";


//----------------------------------------------------------------------------
// SchedulerTaskConfig
//----------------------------------------------------------------------------

//----------------------------------------
CSchedulerTaskConfig::~CSchedulerTaskConfig()
{
  DeleteConfigFileChecker();
}

//----------------------------------------
std::string CSchedulerTaskConfig::GetUserDataDir()
{
  std::string dirName = wxStandardPaths::Get().GetUserDataDir().ToStdString();
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
std::string CSchedulerTaskConfig::GetUserConfigDir()
{
  std::string dirName = wxStandardPaths::Get().GetUserConfigDir().ToStdString();
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
std::string CSchedulerTaskConfig::GetUserConfigDir(const std::string& appName)
{
  std::string dirName = GetUserConfigDir();

  if (appName.empty())
  {
    return dirName;
  }

  dirName = dirName + "/" + appName;
  
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
std::string CSchedulerTaskConfig::GetLogFilePath()
{
  std::string dirName = CSchedulerTaskConfig::GetUserDataDir() + "/" + CSchedulerTaskConfig::m_LOG_REL_PATH;
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
std::string CSchedulerTaskConfig::GetLogFilePath(const std::string& appName)
{
  if (appName.empty())
  {
    return GetLogFilePath();
  }
  std::string dirName = CSchedulerTaskConfig::GetUserConfigDir(appName.c_str()) + "/" + CSchedulerTaskConfig::m_LOG_REL_PATH;
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
  //cout << "Enter PrepareSmartCleaner" << std::endl;
  static CSmartCleaner object(&CSchedulerTaskConfig::mInstance, &CSchedulerTaskConfig::m_logBuffer);
}
//----------------------------------------
CSchedulerTaskConfig* CSchedulerTaskConfig::GetInstance( bool reload, bool lockFile, bool unlockFile )		//bool reload = false, bool lockFile, bool unlockFile
{
	std::string fileName = GetConfigFilePath( CSchedulerTaskConfig::m_CONFIG_APPNAME );
	return GetInstance( &fileName, reload, lockFile, unlockFile );
}
//----------------------------------------------------
CSchedulerTaskConfig* CSchedulerTaskConfig::GetInstance( const std::string* fileName, bool reload /* = false */, bool lockFile /* = true */, bool unlockFile )
{
	wxCriticalSectionLocker locker( m_critSectSchedulerTaskConfigInstance );
	
	if ( !mFactory )
	{
		CreateFactory< CSchedulerTaskConfig >();
	}

    if ( mInstance )
		return dynamic_cast<CSchedulerTaskConfig*>( base_t::GetInstance( fileName, reload, lockFile, unlockFile ) );
//	{
//		if ( reload )
//		{
//			if ( mInstance->IsReloadAll() )
//			{
//				mInstance->LoadAndCreateTasks( *fileName, lockFile, unlockFile );
//			}
//			else
//			{
//				// "Reload" function just load (add in memory) new defined tasks from the configuration file
//				mInstance->ReloadOnlyNew(
///*					[]( const std::string &filename, bool lockFile, bool unlockFile )
//					{ 
//						return new CTasksProcessor( filename, lockFile, unlockFile ); 
//					}, */
//					lockFile, unlockFile );
//			}
//		}
//	}
	else
	{
		if ( fileName != NULL )
		{
			if ( m_logBuffer == NULL )
			{
				m_logBuffer = new wxLogBuffer();
				m_initialLog = wxLog::GetActiveTarget();
			}

			CreateXmlFile( *fileName );

			wxLog::SetActiveTarget( m_logBuffer );

			bool bOk = true;
			std::string errorMsg;
			try
			{
                //mInstance = new CSchedulerTaskConfig( *fileName, lockFile, unlockFile );
                mInstance = dynamic_cast<CSchedulerTaskConfig*>( base_t::GetInstance( fileName, reload, lockFile, unlockFile ) );
                bOk = mInstance && mInstance->IsOk();
			}
			catch ( CException& e )
			{
				bOk = false;
				errorMsg = e.GetMessage().c_str();
			}

			PrepareSmartCleaner();

			if ( !bOk )
			{
				std::string msg;
				if ( errorMsg.empty() )
				{
                    std::string parserError = m_logBuffer->GetBuffer().ToStdString();
					msg = 
						"Unable to load Brat Scheduler configuration file '"
						+ *fileName
						+ "'- Please, check XML syntax - Parser error: '"
						+ parserError
						+ "'";
				}
				else
				{
					msg = errorMsg;
				}

				wxLog::SetActiveTarget( m_initialLog );

				throw CException( msg, BRATHL_ERROR );
			}

			wxLog::SetActiveTarget( m_initialLog );
		}
	}

    return dynamic_cast<CSchedulerTaskConfig*>( mInstance );
}
////----------------------------------------------------
//CSchedulerTaskConfig* CSchedulerTaskConfig::GetInstance(const std::string* fileName, bool reload /* = false */, const std::string& encoding /* = "UTF-8" */)
//{
//  if (fileName == NULL)
//  {
//    return GetInstance(fileName, reload encoding);
//  }
//  std::string fileNameTmp(fileName->c_str());
//
//  return GetInstance(&fileNameTmp, reload, encoding);
//}
//----------------------------------------------------
bool CSchedulerTaskConfig::CreateXmlFile(const std::string& fileName)
{
  if (::wxFileExists(fileName))
  {
    return true;
  }

  CSchedulerTaskConfig xml;
  xml.GetOrAddRootNode_xml();
  xml.GetOrAddPendingTasksElement_xml();
  xml.GetOrAddEndedTasksElement_xml();
  
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
void CSchedulerTaskConfig::NewConfigFileChecker(const std::string& fileName)
{
  DeleteConfigFileChecker();

  // remove non-word character
  std::string mutexName = replace(fileName, "\\W", "").c_str();
  toLower( mutexName );

  wxCriticalSectionLocker locker(m_critSectConfigFileChecker);
  m_configFileChecker = new wxSingleInstanceChecker(mutexName.c_str());
}
//----------------------------------------------------
bool CSchedulerTaskConfig::LockConfigFile(const std::string& fileName, bool lockFile /* = true */)
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
	wxCriticalSectionLocker locker( m_critSectMapBratTask );

	base_t::RemoveMapBratTasks();
}
//----------------------------------------------------
CBratTask::Status CSchedulerTaskConfig::ChangeTaskStatus( wxLongLong_t id, CBratTask::Status newStatus )
{
	wxCriticalSectionLocker locker( m_critSectXmlConfigFile );

	wxLongLong idObj( id );
    std::string idAsString = idObj.ToString().ToStdString();

	wxXmlNode* node = FindTaskNode_xml( id, this->GetRoot_xml(), true );

	if ( node == NULL )
	{
		std::string msg =
			"Unable to update task status: task uid '"
			+ idAsString
			+ "' is not found (CSchedulerTaskConfig#ChangeTaskStatusFromXml).";

		throw CException( msg, BRATHL_ERROR );
	}

	wxXmlNode* parent = node->GetParent();
	if ( parent == NULL )
	{
		std::string msg =
			"Unable to update task status: task uid '"
			+ idAsString
			+ "' is has no Xml parent (CSchedulerTaskConfig#ChangeTaskStatusFromXml).";

		throw CException( msg, BRATHL_ERROR );
	}


	bool removed = parent->RemoveChild( node );

	if ( !removed )
	{
		std::string msg =
			"Unable to remove Xml node for task uid '"
			+ idAsString
			+ "' (CSchedulerTaskConfig#ChangeTaskStatusFromXml).";

		throw CException( msg, BRATHL_ERROR );
	}

	switch ( newStatus )
	{
		case CBratTask::e_BRAT_STATUS_PENDING:
		{
			parent = GetOrAddPendingTasksElement_xml();
			break;
		}
		case CBratTask::e_BRAT_STATUS_PROCESSING:
		{
			parent = GetOrAddProcessingTasksElement_xml();
			break;
		}
		case CBratTask::e_BRAT_STATUS_ENDED:
		case CBratTask::e_BRAT_STATUS_ERROR:
		case CBratTask::e_BRAT_STATUS_WARNING:
		{
			parent = GetOrAddEndedTasksElement_xml();
			break;
		}
		default:
		{
			std::string msg =
				"ERROR in CSchedulerTaskConfig#ChangeTaskStatusFromXml - status "
				+ n2s<std::string>( static_cast<int>( newStatus ) )
				+ " ("
				+ CBratTask::TaskStatusToString( newStatus )
				+ ") not implemented in the function.";

			throw CException( msg, BRATHL_ERROR );
		}
	}

	if ( parent == NULL )
	{
		std::string msg =
			"Unable to update task status: task uid '"
			+ idAsString
			+ "' has no Xml parent for its new status "
			+ n2s<std::string>( static_cast<int>( newStatus ) )
			+ " ("
			+ CBratTask::TaskStatusToString( newStatus )
			+ ") (CSchedulerTaskConfig#ChangeTaskStatusFromXml).";

		throw CException( msg, BRATHL_ERROR );
	}

	this->SetPropVal( node, CSchedulerTaskConfig::m_TASK_STATUS_ATTR, CBratTask::TaskStatusToString( newStatus ) );

	parent->AddChild( node );

	CBratTask::Status oldStatus = base_t::ChangeTaskStatus( id, newStatus );

	wxLogInfo( "Task '%s': status has been changed from '%s' to '%s'.", idAsString.c_str(), CBratTask::TaskStatusToString( oldStatus ).c_str(), CBratTask::TaskStatusToString( newStatus ).c_str() );

	return oldStatus;
}
//----------------------------------------------------
bool CSchedulerTaskConfig::LoadAllTasks()
{
	if ( !base_t::LoadAllTasks() )
		return false;

	LoadTasks( GetProcessingTasksNode_xml() );
	LoadTasks( GetProcessingTasksNode_xml() );
	LoadTasks( GetEndedTasksNode_xml() );

	return true;
}
//----------------------------------------------------
bool CSchedulerTaskConfig::LoadSubordinateTasks( wxXmlNode* taskNode, CBratTask* bratTask )
{
	if ( !taskNode || !bratTask )
		return false;

	wxXmlNode* child = taskNode->GetChildren();

	while ( child != NULL )
	{
		if ( child->GetName().CmpNoCase( CSchedulerTaskConfig::m_TASK_ELT ) != 0 )
		{
			child = child->GetNext();
			continue;
		}

		CBratTask* bratTask = CreateBratTask( child );
		bratTask->GetSubordinateTasks()->Insert( bratTask );

		LoadSubordinateTasks( child, bratTask );

		child = child->GetNext();
	}

	return ( bratTask->GetSubordinateTasks()->size() > 0 );
}
//----------------------------------------------------
bool CSchedulerTaskConfig::LoadTasks( wxXmlNode* node )
{
	if ( !node )
		return false;

	wxXmlNode* child = node->GetChildren();

	while ( child != NULL )
	{
		if ( child->GetName().CmpNoCase( CSchedulerTaskConfig::m_TASK_ELT ) != 0 )
		{
			child = child->GetNext();
			continue;
		}

		CBratTask* bratTask = CreateBratTask( child );
		AddTaskToMap( bratTask );

		LoadSubordinateTasks( child, bratTask );

		child = child->GetNext();
	}

	return true;
}
//----------------------------------------------------
bool CSchedulerTaskConfig::LoadAndCreateTasks( const std::string& fileName, bool lockFile, bool unlockFile )		//lockFile = true, bool unlockFile = true, const std::string& encoding = wxT("UTF-8"), int flags = wxXMLDOC_NONE
{
	if ( !LockConfigFile( fileName, lockFile ) )
	{
		UnLockConfigFile( unlockFile );
		std::string msg = "Unable to load Brat Scheduler configuration file. Perhaps, it's used by another application - Try again later.";
		throw CException( msg, BRATHL_WARNING );
	}

	wxLog::SetActiveTarget( m_initialLog );
	wxLogInfo( "Loading '%s' ...", fileName.c_str() );
	wxLog::SetActiveTarget( m_logBuffer );

	bool bOk = false;

	try
	{
		delete DetachRoot_xml();

		bOk = mdoc.Load( fileName, encoding(), wxXMLDOC_NONE );
		if ( !bOk )
		{
            std::string parserError = m_logBuffer->GetBuffer().ToStdString();
			std::string msg = 
				"Unable to load Brat Scheduler configuration file '"
				+ fileName
				+ "' -  Native error: '"
				+ parserError
				+ "'";
			wxLog::SetActiveTarget( m_initialLog );

			delete DetachRoot_xml();

			throw CException( msg, BRATHL_WARNING );
		}
	}
    catch ( const CException& )
	{
		UnLockConfigFile( unlockFile );
		throw;
	}
    catch ( const std::exception& )
	{
		UnLockConfigFile( unlockFile );
		throw;
	}
	catch ( ... )
	{
		UnLockConfigFile( unlockFile );
		throw CException( "Unexpected error while loading Brat Scheduler configuration file - No Context and no message have been set for this error", BRATHL_ERROR );
	}

	wxLog::SetActiveTarget( m_initialLog );

	//if ( bOk )			this member function being called only by the constructor, the base class is assumed to already have done this
	//	LoadAllTasks();

	UnLockConfigFile( unlockFile );

	wxLogInfo( "'%s' loaded.", fileName.c_str() );

	return bOk;
}
//----------------------------------------------------
bool CSchedulerTaskConfig::AddNewTasksFromSibling( CTasksProcessor* tp )
{
	if ( !base_t::AddNewTasksFromSibling( tp ) )
		return false;

	CSchedulerTaskConfig *sched = dynamic_cast< CSchedulerTaskConfig* >( tp );		assert__(sched);

	auto new_tasks_map = *GetMapNewBratTask();
	for ( CMapBratTask::const_iterator it = new_tasks_map.begin(); it != new_tasks_map.end(); it++ )
	{
		wxLongLong_t id = it->first;
		CBratTask* bratTaskNew = it->second;		assert__( bratTaskNew );

		//CBratTask* bratTask = m_mapBratTask.Find( id );
		//if ( bratTask == NULL )
		//{
			wxLogInfo( "Add task '%s' to pending std::list", bratTaskNew->GetUidAsString().c_str() );

			wxXmlNode* nodeNew = sched->FindTaskNode_xml( bratTaskNew->GetUid(), sched->GetPendingTasksNode_xml(), true );

			if ( nodeNew == NULL )
			{
				std::string msg = 
					"Unable to find task id '"
					+ bratTaskNew->GetUidAsString()
					+ "' in Xml file while adding new tasks (CSchedulerTaskConfig::AddNewTask) ";
				throw CException( msg, BRATHL_ERROR );
			}

			wxXmlNode* taskNodeToAdd = new wxXmlNode( *nodeNew );

			wxXmlNode* pending = GetOrAddPendingTasksElement_xml();
			pending->AddChild( taskNodeToAdd );

			//CBratTask* bratTaskToAdd = new CBratTask( *bratTaskNew );

			//AddTaskToMap( bratTaskToAdd );

			//m_mapNewBratTask.Insert( bratTaskToAdd->GetUid(), bratTaskToAdd );
		//}
	}

	return true;
}
//----------------------------------------------------
bool CSchedulerTaskConfig::Save(const std::string& fileName, bool lockFile /* = true */, bool unlockFile  /* = true */, int indentStep /* = 2 */)
{
  //wxMutexError mutexError = MutexLock();

  bool bOk = LockConfigFile(fileName, lockFile);

  wxLogInfo("Saving '%s' ...", fileName.c_str());

  if (!bOk)
  {
    //std::string msg = std::string::Format("Unable to save Brat Scheduler configuration file. Perhaps, it's used by another application - Try again later (internal code is: %d - internal reason is: %s", static_cast<int32_t>(mutexError), MutexErrorToString(mutexError).c_str());
    std::string msg = "Unable to save Brat Scheduler configuration file. Perhaps, it's used by another application - Try again later.";
    throw CException(msg, BRATHL_ERROR);
    //::wxMessageBox(msg,
    //               "Warning",
    //                wxOK | wxCENTRE | wxICON_EXCLAMATION);
    //return bOk;
  }

  wxLog::SetActiveTarget(m_logBuffer);

  try
  {

    bOk = mdoc.Save(fileName, indentStep);
    if (!bOk)
    {
      std::string parserError = m_logBuffer->GetBuffer().ToStdString();
      std::string msg = 
		  "Unable to save Brat Scheduler configuration file '"
		  + fileName
		  + "' -  Native error: '"
		  + parserError
		  + "'";

      wxLog::SetActiveTarget(CSchedulerTaskConfig::m_initialLog);

      throw CException(msg, BRATHL_ERROR);
    }
  }
  catch(CException& e)
  {
    //MutexUnLock();
    UnLockConfigFile(unlockFile);
    throw e;
  }
  catch(std::exception& e)
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
wxXmlNode* CSchedulerTaskConfig::GetPendingTasksNode_xml()
{
  return FindNodeByName_xml(CSchedulerTaskConfig::m_PENDING_ELT);		//, this->GetRoot(), false
}
//----------------------------------------------------
wxXmlNode* CSchedulerTaskConfig::GetProcessingTasksNode_xml()
{
  return FindNodeByName_xml(CSchedulerTaskConfig::m_PROCESSING_ELT);	//, this->GetRoot(), false
}
//----------------------------------------------------
wxXmlNode* CSchedulerTaskConfig::GetEndedTasksNode_xml()
{
  return FindNodeByName_xml(CSchedulerTaskConfig::m_ENDED_ELT);	//, this->GetRoot(), false
}
//----------------------------------------------------
wxXmlNode* CSchedulerTaskConfig::FindNodeByName_xml( const std::string& name )	//, wxXmlNode* parent, bool allDepths
{
	wxXmlNode *parent = GetRoot_xml();
	if ( !parent )
		return NULL;

	wxXmlNode* child =  parent->GetChildren();
	while ( child != NULL )
	{
		if ( child->GetName().CmpNoCase( name ) == 0 )
			break;

		//if ( ( allDepths ) && ( child->GetChildren() != NULL ) )	//function always called with allDepths false
		//{
		//	wxXmlNode* newChild = FindNodeByName( name, child/*, allDepths */);

		//	if ( newChild != NULL )
		//	{
		//		child = newChild;
		//		break;
		//	}
		//}

		child = child->GetNext();
	}
	return child;
}
//----------------------------------------------------
wxXmlNode* CSchedulerTaskConfig::FindTaskNode_xml(const wxLongLong_t& taskId, bool allDepths /* = false */)
{
  return FindTaskNode_xml(taskId, this->GetRoot_xml(), allDepths);
}
//----------------------------------------------------
wxXmlNode* CSchedulerTaskConfig::FindTaskNode_xml(const std::string& taskId, bool allDepths /* = false */)
{
  return FindTaskNode_xml(wxBratTools::wxStringTowxLongLong_t(taskId), this->GetRoot_xml(), allDepths);
}
//----------------------------------------------------
wxXmlNode* CSchedulerTaskConfig::FindTaskNode_xml(const std::string& taskId, wxXmlNode* parent, bool allDepths /* = false */)
{
    UNUSED(parent);

  return FindTaskNode_xml(wxBratTools::wxStringTowxLongLong_t(taskId), this->GetRoot_xml(), allDepths);
}

#if defined (_MSC_VER)
#pragma warning( disable : 4996 )		//deprecation
#endif

//----------------------------------------------------
wxXmlNode* CSchedulerTaskConfig::FindTaskNode_xml( const wxLongLong_t& taskId, wxXmlNode* parent, bool allDepths /* = false */ )
{
	if ( parent == NULL )
		return NULL;

	wxXmlNode* child =  parent->GetChildren();

	while ( child != NULL )
	{
		if ( child->GetName().CmpNoCase( CSchedulerTaskConfig::m_TASK_ELT ) == 0 )
		{
			wxString value;
			bool bOk = child->GetPropVal( CSchedulerTaskConfig::m_TASK_ID_ATTR, &value );
			if ( bOk )
			{
				if ( wxBratTools::wxStringTowxLongLong_t( value ) == taskId )
				{
					break;
				}
			}
		}

		if ( ( allDepths ) && ( child->GetChildren() != NULL ) )
		{
			wxXmlNode* newChild = FindTaskNode_xml( taskId, child, allDepths );

			if ( newChild != NULL )
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
//
// femm: If r.t is nullptr, means failure
//
TaskRet CSchedulerTaskConfig::AddTaskAsPending( const std::string& cmd, wxDateTime& at, const std::string& name )
{
	// ensure that no other thread accesses the id
	//wxCriticalSectionLocker locker(m_critSectMapBratTask);

	TaskRet r;
	r.n = GetOrAddPendingTasksElement_xml();
	if ( r.n )
		return AddTaskAsPending( r, cmd, at, name );

	return r;
}
//----------------------------------------------------
TaskRet CSchedulerTaskConfig::AddTaskAsPending( TaskRet parent, const std::string& cmd, wxDateTime& at, const std::string& name )
{
	if ( !parent.n )
		return AddTaskAsPending( cmd, at, name );

	TaskRet r;
	r.n = CreateTaskNodeAsPending_xml( at, name, m_TASK_CMD_ATTR, cmd );
	if ( !r.n )
		return r;
	parent.n->AddChild( r.n );

	// ensure that no other thread accesses the tasks' map
	//wxCriticalSectionLocker locker(m_critSectMapBratTask);

	r.t = CreateBratTask( r.n );
	if ( IsPendingTasksElement( parent.n ) )
	{
		AddTaskToMap( r.t );
	}
	else
	{
		AddTask( GetTaskId( parent.n ), r.t );		assert__( GetTaskId( parent.n ) == parent.t->GetUid() );
	}

	return r;
}
//----------------------------------------------------
//femm changed name
TaskRet CSchedulerTaskConfig::AddFunctionTaskAsPending( TaskRet parent, const std::string& function, CVectorBratAlgorithmParam& params, wxDateTime& at, const std::string& name )
{
	if ( !parent.n )
		return TaskRet();

	TaskRet r;
	r.n = CreateTaskNodeAsPending_xml( at, name, m_TASK_FUNCTION_ATTR, function, &params );
	if ( !r.n )
		return r;
	parent.n->AddChild( r.n );

	// ensure that no other thread accesses the tasks' map
	//wxCriticalSectionLocker locker(m_critSectMapBratTask);

	r.t = CreateBratTask( r.n );
	if ( IsPendingTasksElement( parent.n ) )
	{
		AddTaskToMap( r.t );
	}
	else
	{
		AddTask( GetTaskId( parent.n ), r.t );		assert__( GetTaskId( parent.n ) == parent.t->GetUid() );
	}

	return r;
}

//----------------------------------------------------
wxXmlNode* CSchedulerTaskConfig::CreateTaskNodeAsPending_xml( wxDateTime& at, const std::string& name, const std::string &property, const std::string &value, 
	const CVectorBratAlgorithmParam *params )		//const CVectorBratAlgorithmParam *params = nullptr 
{
	// ensure that no other thread accesses 
	wxCriticalSectionLocker locker( m_critSectXmlConfigFile );

	wxXmlNode* taskNode = new wxXmlNode( wxXML_ELEMENT_NODE, CSchedulerTaskConfig::m_TASK_ELT );
	wxLongLong id = GenerateId();

	taskNode->AddProperty( CSchedulerTaskConfig::m_TASK_ID_ATTR, id.ToString() );

	if ( !name.empty() )
		taskNode->AddProperty( CSchedulerTaskConfig::m_TASK_NAME_ATTR, name );

	std::string fullLogFileName =
        CSchedulerTaskConfig::GetLogFilePath( m_CONFIG_APPNAME )
		+ "/"
        + "BratTask_" + name + "_" + id.ToString().ToStdString() + ".log";

	taskNode->AddProperty( CSchedulerTaskConfig::m_TASK_LOG_FILE_ATTR,	fullLogFileName );
	taskNode->AddProperty( CSchedulerTaskConfig::m_TASK_AT_ATTR,		at.Format( CBratTask::formatISODateTime() ) );
	taskNode->AddProperty( CSchedulerTaskConfig::m_TASK_STATUS_ATTR,	CBratTask::TaskStatusToString( CBratTask::e_BRAT_STATUS_PENDING ) );	//m_BRAT_STATUS_PENDING_LABEL
	taskNode->AddProperty( property,									value );

	if ( params )
	{
		for ( CVectorBratAlgorithmParam::const_iterator it = params->begin(); it != params->end(); it++ )
		{
			wxXmlNode* argNode = new wxXmlNode( wxXML_ELEMENT_NODE, CSchedulerTaskConfig::m_ARG_ELT );
			argNode->AddProperty( CSchedulerTaskConfig::m_TYPE_ATTR, it->GetTypeValAsString().c_str() );
			wxXmlNode* argTextNode = new wxXmlNode( wxXML_TEXT_NODE, "" );
			argNode->AddChild( argTextNode );
			try
			{
				argTextNode->SetContent( it->GetValue().c_str() );
			}
			catch ( CException& e )
			{
				argTextNode->SetContent( e.GetMessage().c_str() );
			}

			taskNode->AddChild( argNode );
		}
	}

	return taskNode;
}
//----------------------------------------------------
bool CSchedulerTaskConfig::IsPendingTasksElement( wxXmlNode* node )
{
	if ( !node )
		return false;

	return ( node->GetName().CmpNoCase( CSchedulerTaskConfig::m_PENDING_ELT ) == 0 );
}
//----------------------------------------------------
std::string CSchedulerTaskConfig::GetTaskIdAsString(wxXmlNode* node)
{
  if (node == NULL || node->GetName().CmpNoCase(CSchedulerTaskConfig::m_TASK_ELT) != 0)
    return "";

  wxString value;
  bool bOk = node->GetPropVal(CSchedulerTaskConfig::m_TASK_ID_ATTR, &value);
  UNUSED( value );

  return value.ToStdString();
}
//----------------------------------------------------
CBratTask::uid_t CSchedulerTaskConfig::GetTaskId(wxXmlNode* node)
{
  std::string value = GetTaskIdAsString(node);
    
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

  if (uid.empty())
  {
    return false;
  }

  wxString status;
  bOk = node->GetPropVal(CSchedulerTaskConfig::m_TASK_STATUS_ATTR, &status);

  //if (status.CmpNoCase(CBratTask::m_BRAT_STATUS_PROCESSING_LABEL) == 0)
  if ( CBratTask::StringToTaskStatus( status.ToStdString() ) == CBratTask::e_BRAT_STATUS_PROCESSING )
  {
      std::string msg = 
		  "Task id '"
          + uid.ToStdString()
		  + "' has status '"
          + status.ToStdString()
		  + "'. It can't be removed.";

      throw CException(msg, BRATHL_ERROR);
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
bool CSchedulerTaskConfig::RemoveTask(wxLongLong_t id)
{
  wxXmlNode* root = this->GetRoot_xml();

  wxXmlNode* node = FindTaskNode_xml(id, root, true);

  // Warning: node is set to NULL if removed
  return RemoveTask(node);
}
//----------------------------------------------------
wxXmlNode* CSchedulerTaskConfig::GetOrAddRootNode_xml()
{
	wxXmlNode* root = this->GetRoot_xml();
	if ( !root )
	{
		root = new wxXmlNode( wxXML_ELEMENT_NODE, m_ROOT_ELT );
		SetRoot_xml( root );
	}
	return root;
}
//----------------------------------------------------
wxXmlNode* CSchedulerTaskConfig::GetOrAddTasksElement_xml( const std::string &elt )
{
	wxXmlNode* node = FindNodeByName_xml(elt);
	if ( !node )
	{
		node = new wxXmlNode( wxXML_ELEMENT_NODE, elt );
		wxXmlNode* root = this->GetOrAddRootNode_xml();
		root->AddChild( node );
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

  CBratTask* bratTask = new CBratTask;

  wxString value;
  bool bOk = taskNode->GetPropVal(CSchedulerTaskConfig::m_TASK_ID_ATTR, &value);
  if (!bOk)
  {
    std::string msg = 
		"Unable to create Brat task - Task's id is empty - Please check all '"
		+ CSchedulerTaskConfig::m_TASK_ID_ATTR
		+ "' attributes of the '"
		+ CSchedulerTaskConfig::m_TASK_ELT
		+ "' elements in the Brat Scheduler configuration file '"
		+ m_fullFileName
		+ "'.";

    delete bratTask;
    bratTask = NULL;

    throw CException(msg, BRATHL_ERROR);
  }

  bratTask->SetUid(value.ToStdString());
  bOk = taskNode->GetPropVal(CSchedulerTaskConfig::m_TASK_NAME_ATTR, &value);
  if (bOk)
  {
    bratTask->SetName(value.ToStdString());
  }

  bOk = taskNode->GetPropVal(CSchedulerTaskConfig::m_TASK_CMD_ATTR, &value);
  if (bOk)
  {
    bratTask->SetCmd(value.ToStdString());
  }
  else
  {
    bOk = taskNode->GetPropVal(CSchedulerTaskConfig::m_TASK_FUNCTION_ATTR, &value);
    if (bOk)
    {
      CBratTaskFunction* functionRef = CMapBratTaskFunction::GetInstance().Find(value.ToStdString());
      if (functionRef == NULL)
      {
        std::string msg = 
			"Unable to create Brat task - Function '"
            + value.ToStdString()
			+ "' is unknown.\nPlease check '"
			+ CSchedulerTaskConfig::m_TASK_FUNCTION_ATTR
			+ "' attribute of the '"
			+ CSchedulerTaskConfig::m_TASK_ELT
			+ "' element for task id '"
			+ bratTask->GetUidAsString()
			+ "' in the Brat Scheduler configuration file '"
			+ m_fullFileName
			+ "'.";

        delete bratTask;
        bratTask = NULL;

        throw CException(msg, BRATHL_ERROR);
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
            params->Insert(argTextNode->GetContent().ToStdString());
          }
        }

        argNode = argNode->GetNext();
      }

    }

  }
  if (!bOk)
  {
    std::string msg = "Unable to create Brat task - Neither Task's command-line nor task's function have been set.\nPlease check "
        + CSchedulerTaskConfig::m_TASK_CMD_ATTR
		+ " attribute of the "
        + CSchedulerTaskConfig::m_TASK_ELT
		+ " element for task id "
		+ bratTask->GetUidAsString()
		+ " in the Brat Scheduler configuration file "
        + m_fullFileName
		+ ".";

    delete bratTask;
    bratTask = NULL;

    throw CException(msg, BRATHL_ERROR);
  }

  bOk = taskNode->GetPropVal(CSchedulerTaskConfig::m_TASK_AT_ATTR, &value);
  if (!bOk)
  {
    std::string msg = 
		"Unable to create Brat task - Task's scheduled date/time is empty - Please check '"
		+ CSchedulerTaskConfig::m_TASK_AT_ATTR
		+ "' attribute of the '"
		+ CSchedulerTaskConfig::m_TASK_ELT
		+ "' element for task id '"
		+ bratTask->GetUidAsString()
		+ "' in the Brat Scheduler configuration file '"
		+ m_fullFileName
		+ "'.";

    delete bratTask;
    bratTask = NULL;

    throw CException(msg, BRATHL_ERROR);
  }

  wxDateTime dt;
  std::string error;

  bOk = ParseDateTime(value.ToStdString(), dt, error);
  if (!bOk)
  {
    std::string msg = 
		"Unable to parse date '"
        + value.ToStdString()
		+ "' for Brat Scheduler task id "
		+ bratTask->GetUidAsString()
		+ "(cmd is '"
		+ bratTask->GetCmd()
		+ "')";
    
    delete bratTask;
    bratTask = NULL;
    
    throw CException(msg, BRATHL_ERROR);
  }

  bratTask->SetAt(value.ToStdString());		//bratTask->SetAt(dt);		femm

  bOk = taskNode->GetPropVal(CSchedulerTaskConfig::m_TASK_STATUS_ATTR, &value);
  if (!bOk)
  {
    //femm value = CBratTask::m_BRAT_STATUS_PENDING_LABEL;
    value = CBratTask::TaskStatusToString( CBratTask::e_BRAT_STATUS_PENDING );
    //std::string msg = std::string::Format("Unable to create Brat task - Task's scheduled status is empty - "
    //  "Please check '%s' attribute of the '%s' element for task id '%s' in the Brat Scheduler configuration file '%s'.",
    //  CSchedulerTaskConfig::m_TASK_STATUS_ATTR.c_str(),
    //  CSchedulerTaskConfig::m_TASK_ELT.c_str(),
    //  bratTask->GetUid().ToString().c_str(),
    //  m_fullFileName.c_str());

    //delete bratTask;
    //bratTask = NULL;

    //throw CException(msg.c_str(), BRATHL_ERROR);
  }

  bratTask->SetStatus(value.ToStdString());

  bOk = taskNode->GetPropVal(CSchedulerTaskConfig::m_TASK_LOG_FILE_ATTR, &value);
  if (!bOk)
  {
    std::string msg = 
		"Unable to create Brat task - Log file is empty - Please check '"
		+ CSchedulerTaskConfig::m_TASK_LOG_FILE_ATTR
		+ "' attribute of the '"
		+ CSchedulerTaskConfig::m_TASK_ELT
		+ "' element for task id '"
		+ bratTask->GetUidAsString()
		+ "' in the Brat Scheduler configuration file '"
		+ m_fullFileName
		+ "'.";

    delete bratTask;
    bratTask = NULL;

    throw CException(msg, BRATHL_ERROR);
  }

  bratTask->SetLogFile(value.ToStdString());

  return bratTask;
}

//----------------------------------------
bool CSchedulerTaskConfig::ParseDateTime(const std::string& value, wxDateTime& dt, std::string& error) 
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
bool CSchedulerTaskConfig::LoadAllSchedulerTaskConfig( bool quiet /* = false */ )
{
	bool loaded = false;
	bool cancelled = false;

	do
	{
		try
		{
			bool reload = true;		//femm
			GetInstance( reload, true, false );
			loaded = true;
		}
		catch ( const CException& e )
		{
			//-----------------------
			if ( quiet )
			{
				ForceUnLockConfigFile();
				throw;
			}
			//-----------------------

			loaded = false;
			std::string msg = e.GetMessage() + "\n Try again ?";
			cancelled = wxMessageBox( msg, "Warning", wxYES_NO | wxCENTRE | wxICON_EXCLAMATION ) != wxYES;
		}
	} while ( !loaded && !cancelled );

	if ( !loaded )
	{
		std::string msg = "Operation canceled";
        //int32_t result =
                wxMessageBox( msg, "Warning", wxOK | wxCENTRE | wxICON_EXCLAMATION );

		ForceUnLockConfigFile();
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
      std::string msg = e.GetMessage() + "\n Try again ?";
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

    std::string msg = "Operation canceled";
    //int32_t result =
            wxMessageBox(msg,
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
bool CSchedulerTaskConfig::SetPropVal(wxXmlNode* node, const std::string& propName, const std::string& value, bool allDepths /* = true */)
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

#if defined (_MSC_VER)
#pragma warning( default : 4996 )
#endif
