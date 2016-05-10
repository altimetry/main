#include "stdafx.h"

#include "new-gui/Common/+Utils.h"
#include "new-gui/Common/+UtilsIO.h"

#include "TaskProcessor.h"




///////////////////////////////////////
///////////////////////////////////////
//			Singleton
///////////////////////////////////////
///////////////////////////////////////


//static 
bool CTasksProcessor::mFactoryCalled = false;
//static 
CTasksProcessor* CTasksProcessor::smInstance = nullptr;
//static 
std::function<CTasksProcessor*( const std::string &filename, bool lockFile, bool unlockFile )> CTasksProcessor::mFactory = nullptr;



CTasksProcessor* CTasksProcessor::GetInstance( const std::string* fileName, bool reload /* = false */, bool lockFile /* = true */, bool unlockFile )
{
    if ( !mFactory )
    {
        CreateFactory();
    }
    //wxCriticalSectionLocker locker( m_critSectSchedulerTaskConfigInstance );			// TODO
	
    if ( smInstance )
	{
		if ( reload )
		{
            if ( smInstance->IsReloadAll() )
			{
                smInstance->LoadAndCreateTasks( *fileName, lockFile, unlockFile );
			}
			else
			{
				// "Reload" function just load (add in memory) new defined tasks from the configuration file
                smInstance->ReloadOnlyNew( lockFile, unlockFile );
			}
		}
	}
	else
	{
		if ( fileName != NULL )
		{
			//if ( m_logBuffer == NULL )
			//{
			//	m_logBuffer = new wxLogBuffer();
			//	m_initialLog = wxLog::GetActiveTarget();
			//}													// TODO

			//CreateXmlFile( *fileName );

			// wxLog::SetActiveTarget( m_logBuffer );			// TODO

			bool bOk = true;
			std::string errorMsg;
			try
			{
                //smInstance = new CSchedulerTaskConfig( *fileName, lockFile, unlockFile );
                smInstance = mFactory( *fileName, lockFile, unlockFile );
                bOk = smInstance->IsOk();
			}
			catch ( CException& e )
			{
				bOk = false;
				errorMsg = e.Message().c_str();
			}

			//PrepareSmartCleaner();										  		//TODO ???

			if ( !bOk )
			{
				std::string msg;
				if ( errorMsg.empty() )
				{
					std::string parserError;	// = m_logBuffer->GetBuffer();		//TODO
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

				//wxLog::SetActiveTarget( m_initialLog );							//TODO

				throw CException( msg, BRATHL_ERROR );
			}

			//wxLog::SetActiveTarget( m_initialLog );	 							//TODO
		}
	}

    return smInstance;
}


///////////////////////////////////////
///////////////////////////////////////
//			Serialization
///////////////////////////////////////
///////////////////////////////////////


bool CTasksProcessor::LoadAndCreateTasks( const std::string& fileName, bool lockFile, bool unlockFile )		//lockFile = true, bool unlockFile = true, const std::string& encoding = wxT("UTF-8"), int flags = wxXMLDOC_NONE
{
	if ( !LockConfigFile( fileName, lockFile ) )
	{
		UnLockConfigFile( unlockFile );
		std::string msg = "Unable to load Brat Scheduler configuration file. Perhaps, it's used by another application - Try again later.";
		throw CException( msg, BRATHL_WARNING );
	}

	//wxLog::SetActiveTarget( m_initialLog );				//TODO
	//wxLogInfo( "Loading '%s' ...", fileName.c_str() );
	//wxLog::SetActiveTarget( m_logBuffer );

	bool bOk = false;

	//try
	//{
	//	delete DetachRoot_xml();

	//	bOk = mdoc.Load( fileName, encoding(), wxXMLDOC_NONE );
	//	if ( !bOk )
	//	{
	//		std::string parserError = m_logBuffer->GetBuffer();
	//		std::string msg =
	//			"Unable to load Brat Scheduler configuration file '"
	//			+ fileName
	//			+ "' -  Native error: '"
	//			+ parserError
	//			+ "'";
	//		wxLog::SetActiveTarget( m_initialLog );

	//		delete DetachRoot_xml();

	//		throw CException( msg, BRATHL_WARNING );
	//	}
	//}
	//catch ( const CException& )
	//{
	//	UnLockConfigFile( unlockFile );
	//	throw;
	//}
	//catch ( const std::exception& )
	//{
	//	UnLockConfigFile( unlockFile );
	//	throw;
	//}
	//catch ( ... )
	//{
	//	UnLockConfigFile( unlockFile );
	//	throw CException( "Unexpected error while loading Brat Scheduler configuration file - No Context and no message have been set for this error", BRATHL_ERROR );
	//}

	//wxLog::SetActiveTarget( m_initialLog );	 				//TODO

	//if ( bOk )
	bOk = LoadAllTasks();

	UnLockConfigFile( unlockFile );

	//wxLogInfo( "'%s' loaded.", fileName.c_str() );			//TODO

	return bOk;
}



CTasksProcessor& CTasksProcessor::operator = ( const bratSchedulerConfig &oxml )			//USED ON LOAD
{
	mPendingTasks.clear();
	mProcessingTasks.clear();
	mEndedTasks.clear();

	auto const &pending = oxml.pendingTasks().task();
	auto const &processing = oxml.processingTasks().task();
	auto const &ended = oxml.endedTasks().task();

	for ( auto ii = pending.begin(); ii != pending.end(); ++ii )
	{
		mPendingTasks.push_back( new CBratTask(*ii) );
	}
	for ( auto ii = processing.begin(); ii != processing.end(); ++ii )
	{
		mProcessingTasks.push_back( new CBratTask(*ii) );
	}
	for ( auto ii = ended.begin(); ii != ended.end(); ++ii )
	{
		mEndedTasks.push_back( new CBratTask(*ii) );
	}
	return *this;
}

bratSchedulerConfig& CTasksProcessor::IOcopy( bratSchedulerConfig &oxml ) const			//USED ON STORE
{
	oxml.pendingTasks( bratSchedulerConfig::pendingTasks_type() );
	oxml.processingTasks( bratSchedulerConfig::processingTasks_type() );
	oxml.endedTasks( bratSchedulerConfig::endedTasks_type() );

	auto &pending = oxml.pendingTasks().task();
	auto &processing = oxml.processingTasks().task();
	auto &ended = oxml.endedTasks().task();

	for ( auto ii = mPendingTasks.begin(); ii != mPendingTasks.end(); ++ii )
	{
		task t;
		pending.push_back( (*ii)->IOcopy( t ) );
	}

	for ( auto ii = mProcessingTasks.begin(); ii != mProcessingTasks.end(); ++ii )
	{
		task t;
		processing.push_back( (*ii)->IOcopy( t ) );
	}

	for ( auto ii = mEndedTasks.begin(); ii != mEndedTasks.end(); ++ii )
	{
		task t;
		ended.push_back( (*ii)->IOcopy( t ) );
	}

	return oxml;
}


bool CTasksProcessor::store( const std::string &path )
{
	return ::store( *this, path );
}

bool CTasksProcessor::load( const std::string &path )
{
	return ::load( *this, path );
}




template<>
struct serializer_traits< CTasksProcessor >
{
	typedef bratSchedulerConfig converter_t;

	static void
	serialize(::xercesc::XMLFormatTarget& ft,
						 const converter_t& x, 
						 const ::xml_schema::namespace_infomap& m = ::xml_schema::namespace_infomap (),
						 const ::std::string& e = "UTF-8",
						 ::xml_schema::flags f = 0)
	{
		::bratSchedulerConfig_( ft, x, m, e, f );
	}

	static ::std::auto_ptr< converter_t >
	deserialize( const ::std::string& u, ::xml_schema::flags f, const ::xml_schema::properties& p = ::xml_schema::properties() )
	{
		return ::bratSchedulerConfig_( u, f, p );
	}
};


//virtual 
bool CTasksProcessor::LoadAllTasks()
{
	RemoveMapBratTasks();

    std::string ex;
    bool result = true;

	try {
		if ( !IsFile( m_fullFileName ) )
		{
			result = store( m_fullFileName );
		}

		result = result && load( m_fullFileName );

#if defined(DEBUG) || defined(_DEBUG)

		CTasksProcessor debug_tasks;
		if ( result )
		{
			std::string debug_path = std::string( getenv( "S3ALTB_ROOT" ) ) + "/project/dev/source/new-gui/scheduler/BratSchedulerTasksConfig_backup.xml";
			result = store( debug_path );
			if ( result )
				result = debug_tasks.load( debug_path );
		}

		assert__( result && debug_tasks == *this );

#endif

	}
	catch ( const xml_schema::exception& e )
	{
		translate_exception( ex, e );
	}
	if ( !result )
		throw CException( ex, BRATHL_ERROR );

	//TODO: populate this

	for ( auto *pt : mPendingTasks )
		AddTaskToMap( pt );
	for ( auto *pt : mProcessingTasks )
		AddTaskToMap( pt );
	for ( auto *pt : mEndedTasks )
		AddTaskToMap( pt );

	mPendingTasks.clear();
	mProcessingTasks.clear();
	mEndedTasks.clear();

	//CMapBratTask m_mapBratTask;
	//// Map for pending tasks - Warning : don't delete object stored in it.
	//CMapBratTask m_mapPendingBratTask;
	//// Map for processing tasks - Warning : don't delete object stored in it.
	//CMapBratTask m_mapProcessingBratTask;
	//// Map for ended (or error) tasks - Warning : don't delete object stored in it.
	//CMapBratTask m_mapEndedBratTask;
	//// Map used to stored new pending tasks when file is reloaded.
	//// Tasks are just added as reference - Warning : don't delete object stored in it.
	//CMapBratTask m_mapNewBratTask;

	return result;

	//LoadPendingTasks();
	//LoadProcessingTasks();
	//LoadEndedTasks();
}



//----------------------------------------------------
bool CTasksProcessor::AddNewTasksFromSibling( CTasksProcessor* sched )
{
	if ( !sched )
		return false;

	for ( CMapBratTask::const_iterator it = sched->m_mapBratTask.begin(); it != sched->m_mapBratTask.end(); it++ )
	{
		uid_t id = it->first;
		CBratTask* bratTaskNew = it->second;
		if ( !bratTaskNew )
			continue;

		CBratTask* bratTask = m_mapBratTask.Find( id );
		if ( bratTask == NULL )
		{
			//wxLogInfo( "Add task '%s' to pending std::list", bratTaskNew->GetUidAsString().c_str() );			// TODO

			//wxXmlNode* nodeNew = sched->FindTaskNode_xml( bratTaskNew->GetUid(), sched->GetPendingTasksNode_xml(), true );

			//if ( nodeNew == NULL )
			//{
			//	std::string msg = 
			//		"Unable to find task id '"
			//		+ bratTaskNew->GetUidAsString()
			//		+ "' in Xml file while adding new tasks (CSchedulerTaskConfig::AddNewTask) ";
			//	throw CException( msg, BRATHL_ERROR );
			//}

			//wxXmlNode* taskNodeToAdd = new wxXmlNode( *nodeNew );
			//wxXmlNode* pending = GetOrAddPendingTasksElement_xml();
			//pending->AddChild( taskNodeToAdd );

			CBratTask* bratTaskToAdd = new CBratTask( *bratTaskNew );

			AddTaskToMap( bratTaskToAdd );

			m_mapNewBratTask.Insert( bratTaskToAdd->GetUid(), bratTaskToAdd );
		}
	}

	return true;
}


bool CTasksProcessor::ReloadOnlyNew( /*std::function<CTasksProcessor*(const std::string &filename, bool lockFile, bool unlockFile)> factory, */
	bool lockFile, bool unlockFile )	// lockFile = true, bool unlockFile = true
{
	m_mapNewBratTask.RemoveAll();

	bool bOk = LockConfigFile( lockFile );

	if ( !bOk )
	{
		UnLockConfigFile( unlockFile );
		std::string msg = "Unable to load Brat Scheduler configuration file. Perhaps, it's used by another application - Try again later.";
		throw CException( msg, BRATHL_WARNING );
	}

	//wxLog::SetActiveTarget( CSchedulerTaskConfig::m_initialLog );
	//wxLogInfo( "Re-loading '%s' ...", m_fullFileName.c_str() );
	//wxLog::SetActiveTarget( m_logBuffer );					   				// TODO

	CTasksProcessor* schedulerTaskConfig = NULL;
	bOk = true;

	std::string errorMsg;
	try
	{
		//schedulerTaskConfig = new CTasksProcessor( m_fullFileName, false, false );		//assert__(this->GetFileEncoding() == encoding() );
		schedulerTaskConfig = mFactory( m_fullFileName, false, false );		//assert__(this->GetFileEncoding() == encoding() );
		bOk = schedulerTaskConfig->IsOk();
	}
	catch ( CException& e )
	{
		bOk = false;
		errorMsg = e.Message().c_str();
	}

	if ( !bOk )
	{
		std::string msg;
		if ( errorMsg.empty() )
		{
			std::string parserError; //= m_logBuffer->GetBuffer();				// TODO
			msg =
				"Unable to load Brat Scheduler configuration file '"
				+ m_fullFileName
				+ "' - Please, check XML syntax - Parser error: '"
				+ parserError
				+ "'";
		}
		else
		{
			msg = errorMsg;
		}

		//wxLog::SetActiveTarget( CSchedulerTaskConfig::m_initialLog );			// TODO

		delete schedulerTaskConfig;
		schedulerTaskConfig = NULL;

		this->UnLockConfigFile( unlockFile );

		throw CException( msg, BRATHL_ERROR );
	}

	//wxLog::SetActiveTarget( CSchedulerTaskConfig::m_initialLog );	   			// TODO

	AddNewTasksFromSibling( schedulerTaskConfig );

	delete schedulerTaskConfig;
	schedulerTaskConfig = NULL;

	this->UnLockConfigFile( unlockFile );

	//wxLogInfo( "'%s' re-loaded.", m_fullFileName.c_str() );					// TODO

	return bOk;
}


///////////////////////////////////////
///////////////////////////////////////
//				Access
///////////////////////////////////////
///////////////////////////////////////




CBratTask* CTasksProcessor::FindTaskFromMap( uid_t id )
{
	// ensure that no other thread accesses the id
	//wxCriticalSectionLocker locker(m_critSectMapBratTask);			//TODO

	return m_mapBratTask.Find( id );
}


CBratTask::uid_t CTasksProcessor::GenerateId()
{
	// ensure that no other thread accesses the id
	//wxCriticalSectionLocker locker(m_criSectionMapBratTask);	femm: commented out in the original

	//m_lastId = wxDateTime::UNow().GetValue();
	m_lastId = QDateTime::currentDateTime().toMSecsSinceEpoch();	//cout << "Enter generate id " << m_lastId.GetValue() << std::endl;

	CBratTask* bratTask = m_mapBratTask.Find( m_lastId );

	while ( bratTask )
		bratTask = m_mapBratTask.Find( ++m_lastId );				//cout << "Loop generate id " << m_lastId.GetValue() << std::endl;

	return m_lastId;
}


void CTasksProcessor::GetMapPendingBratTaskToProcess( std::vector<uid_t>* vectorBratTaskToProcess )
{
	if ( !vectorBratTaskToProcess )
		return;

	//wxCriticalSectionLocker locker( m_critSectMapBratTask );			//TODO

	for ( CMapBratTask::const_iterator it = m_mapPendingBratTask.begin(); it != m_mapPendingBratTask.end(); it++ )
	{
		CBratTask* bratTask = it->second;
		if ( !bratTask || bratTask->laterThanNow() )		//if (bratTask->GetAt() > dateRef)		femm
			continue;

		vectorBratTaskToProcess->push_back( bratTask->GetUid() );
	}
}






///////////////////////////////////////
///////////////////////////////////////
//			Tasks Processing
///////////////////////////////////////
///////////////////////////////////////


///////////////////////////////////////
//			Add / Remove



void CTasksProcessor::AddTaskToMap( CBratTask* bratTask )
{
	if ( !bratTask )
		return;

	//wxCriticalSectionLocker locker( m_critSectMapBratTask );		// TODO

	uid_t uid = bratTask->GetUid();
	m_mapBratTask.Insert( uid, bratTask );

	CBratTask::Status status = bratTask->GetStatus();
	switch ( status )
	{
		case CBratTask::e_BRAT_STATUS_PENDING:
		{
			m_mapPendingBratTask.Insert( uid, bratTask );
			break;
		}
		case CBratTask::e_BRAT_STATUS_PROCESSING:
		{
			m_mapProcessingBratTask.Insert( uid, bratTask );
			break;
		}
		case CBratTask::e_BRAT_STATUS_ENDED:
		case CBratTask::e_BRAT_STATUS_ERROR:
		case CBratTask::e_BRAT_STATUS_WARNING:
		{
			m_mapEndedBratTask.Insert( uid, bratTask );
			break;
		}
		default:
		{
			std::string msg =
				"ERROR in CSchedulerTaskConfig#AddTaskToMap - status "
				+ n2s<std::string>( static_cast<int>( status ) )
				+ " ("
				+ bratTask->GetStatusAsString()
				+ ") not implemented in the function.";

			throw CException( msg, BRATHL_ERROR );
		}
	}
}


void CTasksProcessor::AddTask( uid_t parentId, CBratTask* bratTask )
{
	if ( !bratTask )
		return;

	//wxCriticalSectionLocker locker( m_critSectMapBratTask );			//TODO

	CBratTask* parentBratTask = m_mapBratTask.Find( parentId );
	if ( !parentBratTask )
		return;

	parentBratTask->GetSubordinateTasks()->Insert( bratTask );
}



void CTasksProcessor::AddTask( const CMapBratTask* mapBratTask )
{
	if ( !mapBratTask )
		return;

	for ( CMapBratTask::const_iterator it = mapBratTask->begin(); it != mapBratTask->end(); it++ )
	{
		CBratTask* bratTaskNew = it->second;		//wxLongLong_t id = it->first;
		if ( !bratTaskNew )
			continue;

		AddTaskToMap( new CBratTask( *bratTaskNew ) );
	}
}





bool CTasksProcessor::RemoveTaskFromMap( uid_t id )
{
	// ensure that no other thread accesses the id
	//wxCriticalSectionLocker locker( m_critSectMapBratTask );			//TODO

	std::string idAsString = n2s<std::string>( id );

	CBratTask* bratTask = m_mapBratTask.Find( id );
	if ( bratTask == NULL )
	{
		return false;
	}

	CBratTask::Status status = bratTask->GetStatus();

	if ( status == CBratTask::e_BRAT_STATUS_PROCESSING )
	{
		std::string msg =
			"Task id '"
			+ idAsString
			+ "' has status "
			+ n2s<std::string>( static_cast<int>( status ) )
			+ " ("
			+ bratTask->GetStatusAsString()
			+ "). It can't be removed.";

		throw CException( msg, BRATHL_ERROR );
	}

	m_mapPendingBratTask.Remove( id );
	m_mapProcessingBratTask.Remove( id );
	m_mapEndedBratTask.Remove( id );

	// Remove log file
	//wxBratTools::RemoveFile( bratTask->GetLogFile() );	//->GetFullPath()	//TODO//TODO//TODO//TODO//TODO//TODO//TODO//TODO//TODO//TODO//TODO//TODO//TODO//TODO

	m_mapBratTask.Remove( id );

	return true;
}



///////////////////////////////////////
//			Change Status

//----------------------------------------------------
bool CTasksProcessor::ChangeProcessingToPending( CVectorBratTask& vectorTasks )
{
	vectorTasks.SetDelete( false );

	// tasks will be deleted from m_mapProcessingBratTask by ChangeTaskStatus function
	// Don't change status within the loop, store tasks into a vecor and then change status from std::vector tasks
	for ( CMapBratTask::const_iterator it = m_mapProcessingBratTask.begin(); it != m_mapProcessingBratTask.end(); it++ )
	{
		CBratTask* bratTask = it->second;
		if ( !bratTask )
			continue;

		vectorTasks.Insert( bratTask );
	}

	for ( CVectorBratTask::const_iterator itVector = vectorTasks.begin(); itVector != vectorTasks.end(); itVector++ )
	{
		CBratTask* bratTask = *itVector;
		if ( !bratTask )
			continue;

		ChangeTaskStatus( bratTask->GetUid(), CBratTask::e_BRAT_STATUS_PENDING );
	}

	return vectorTasks.size() > 0;
}


CTasksProcessor::Status CTasksProcessor::ChangeTaskStatus( uid_t id, Status newStatus )
{
	//ChangeTaskStatusFromXml( id, newStatus );
	//CBratTask::Status currentStatus = ChangeTaskStatusFromMap( id, newStatus );

	//wxCriticalSectionLocker locker(m_critSectMapBratTask);		// TODO

	std::string idAsString = n2s<std::string>( id );

	CBratTask* bratTask = m_mapBratTask.Find( id );

	if ( bratTask == NULL )
	{
		std::string msg =
			"Unable to update task status: task uid '"
			+ idAsString
			+ "' is not found (CSchedulerTaskConfig#ChangeTaskStatusFromMap).";

		throw CException( msg, BRATHL_ERROR );
	}

	Status currentStatus = bratTask->GetStatus();

	switch ( currentStatus )
	{
		case CBratTask::e_BRAT_STATUS_PENDING:
		{
			m_mapPendingBratTask.Remove( id );
			break;
		}
		case CBratTask::e_BRAT_STATUS_PROCESSING:
		{
			m_mapProcessingBratTask.Remove( id );
			break;
		}
		case CBratTask::e_BRAT_STATUS_ENDED:
		case CBratTask::e_BRAT_STATUS_ERROR:
		case CBratTask::e_BRAT_STATUS_WARNING:
		{
			m_mapEndedBratTask.Remove( id );
			break;
		}
		default:
		{
			std::string msg =
				"ERROR in CSchedulerTaskConfig#ChangeTaskStatusFromMap - status "
				+ n2s<std::string>( static_cast<int>( currentStatus ) )
				+ " ("
				+ bratTask->GetStatusAsString()
				+ ") not implemented in the function.";

			throw CException( msg, BRATHL_ERROR );
		}
	}

	bratTask->SetStatus( newStatus );

	switch ( newStatus )
	{
		case CBratTask::e_BRAT_STATUS_PENDING:
		{
			m_mapPendingBratTask.Insert( id, bratTask );
			break;
		}
		case CBratTask::e_BRAT_STATUS_PROCESSING:
		{
			m_mapProcessingBratTask.Insert( id, bratTask );
			break;
		}
		case CBratTask::e_BRAT_STATUS_ENDED:
		case CBratTask::e_BRAT_STATUS_ERROR:
		case CBratTask::e_BRAT_STATUS_WARNING:
		{
			m_mapEndedBratTask.Insert( id, bratTask );
			break;
		}
		default:
		{
			std::string msg =
				"ERROR in CSchedulerTaskConfig#ChangeTaskStatusFromMap - status "
				+ n2s<std::string>( static_cast<int>( newStatus ) )
				+ " ("
				+ CBratTask::TaskStatusToString( newStatus )
				+ ") not implemented in the function.";

			throw CException( msg, BRATHL_ERROR );
		}
	}

	return currentStatus;

	// TODO
	//
	//wxLongLong idObj( id );
	//std::string idAsString = idObj.ToString();
	//wxLogInfo( "Task '%s': status has been changed from '%s' to '%s'.", idAsString.c_str(), CBratTask::TaskStatusToString( currentStatus ).c_str(), CBratTask::TaskStatusToString( newStatus ).c_str() );
}
