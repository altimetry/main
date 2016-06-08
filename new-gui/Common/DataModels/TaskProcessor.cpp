#include "stdafx.h"

#include "new-gui/Common/+Utils.h"
#include "new-gui/Common/+UtilsIO.h"
#include "new-gui/Common/QtUtilsIO.h"

#include "TaskProcessor.h"




///////////////////////////////////////
///////////////////////////////////////
//			Singleton
///////////////////////////////////////
///////////////////////////////////////


//static 
const std::string CTasksProcessor::smFileName = "SchedulerTasksConfig";		//extension ".xml" must be added
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
		if ( fileName != nullptr )
		{
			//if ( m_logBuffer == nullptr )
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
                //smInstance = new CTasksProcessor( *fileName, lockFile, unlockFile );
                smInstance = mFactory( *fileName, lockFile, unlockFile );
                bOk = smInstance != nullptr;
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
	mPendingTasksMap.clear();
	mProcessingTasksMap.clear();
	mEndedTasksMap.clear();

	auto const &pending = oxml.pendingTasks().task();
	auto const &processing = oxml.processingTasks().task();
	auto const &ended = oxml.endedTasks().task();

	for ( auto ii = pending.begin(); ii != pending.end(); ++ii )
	{
		mPendingTasksMap.Insert( ii->uid(), new CBratTask(*ii) );
	}
	for ( auto ii = processing.begin(); ii != processing.end(); ++ii )
	{
		mProcessingTasksMap.Insert( ii->uid(), new CBratTask(*ii) );
	}
	for ( auto ii = ended.begin(); ii != ended.end(); ++ii )
	{
		mEndedTasksMap.Insert( ii->uid(), new CBratTask(*ii) );
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

	for ( auto ii = mPendingTasksMap.begin(); ii != mPendingTasksMap.end(); ++ii )
	{
		task t;
		pending.push_back( ii->second->IOcopy( t ) );
	}

	for ( auto ii = mProcessingTasksMap.begin(); ii != mProcessingTasksMap.end(); ++ii )
	{
		task t;
		processing.push_back( ii->second->IOcopy( t ) );
	}

	for ( auto ii = mEndedTasksMap.begin(); ii != mEndedTasksMap.end(); ++ii )
	{
		task t;
		ended.push_back( ii->second->IOcopy( t ) );
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
bool CTasksProcessor::SaveAllTasks()
{
    std::string ex;
    bool result = true;
	try {
		result = store( m_fullFileName );
	}
	catch ( const xml_schema::exception& e )
	{
		translate_exception( ex, e );
	}
	if ( !result )
		throw CException( ex, BRATHL_ERROR );

	return result;
}

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
			std::string debug_path = std::string( getenv( "S3ALTB_ROOT" ) ) + "/project/dev/source/new-gui/scheduler/" + smFileName + "_backup.xml";
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


	//CMapBratTask m_mapBratTask;
	//// Map for pending tasks - Warning : don't delete object stored in it.
	//CMapBratTask mPendingTasksMap;
	//// Map for processing tasks - Warning : don't delete object stored in it.
	//CMapBratTask mProcessingTasksMap;
	//// Map for ended (or error) tasks - Warning : don't delete object stored in it.
	//CMapBratTask mEndedTasksMap;
	//// Map used to stored new pending tasks when file is reloaded.
	//// Tasks are just added as reference - Warning : don't delete object stored in it.
	//CMapBratTask m_mapNewBratTask;

	for ( auto &pt : mPendingTasksMap )
		m_mapBratTask.Insert( pt.first, pt.second );
	for ( auto &pt : mProcessingTasksMap )
		m_mapBratTask.Insert( pt.first, pt.second );
	for ( auto &pt : mEndedTasksMap )
		m_mapBratTask.Insert( pt.first, pt.second );

	return result;
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
		if ( bratTask == nullptr )
		{
			//wxLogInfo( "Add task '%s' to pending std::list", bratTaskNew->GetUidAsString().c_str() );			// TODO

			//wxXmlNode* nodeNew = sched->FindTaskNode_xml( bratTaskNew->GetUid(), sched->GetPendingTasksNode_xml(), true );

			//if ( nodeNew == nullptr )
			//{
			//	std::string msg = 
			//		"Unable to find task id '"
			//		+ bratTaskNew->GetUidAsString()
			//		+ "' in Xml file while adding new tasks (CTasksProcessor::AddNewTask) ";
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

	//wxLog::SetActiveTarget( CTasksProcessor::m_initialLog );
	//wxLogInfo( "Re-loading '%s' ...", m_fullFileName.c_str() );
	//wxLog::SetActiveTarget( m_logBuffer );					   				// TODO

	CTasksProcessor* schedulerTaskConfig = nullptr;
	bOk = true;

	std::string errorMsg;
	try
	{
		//schedulerTaskConfig = new CTasksProcessor( m_fullFileName, false, false );		//assert__(this->GetFileEncoding() == encoding() );
		schedulerTaskConfig = mFactory( m_fullFileName, false, false );		//assert__(this->GetFileEncoding() == encoding() );
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

		//wxLog::SetActiveTarget( CTasksProcessor::m_initialLog );			// TODO

		delete schedulerTaskConfig;
		schedulerTaskConfig = nullptr;

		this->UnLockConfigFile( unlockFile );

		throw CException( msg, BRATHL_ERROR );
	}

	//wxLog::SetActiveTarget( CTasksProcessor::m_initialLog );	   			// TODO

	AddNewTasksFromSibling( schedulerTaskConfig );

	delete schedulerTaskConfig;
	schedulerTaskConfig = nullptr;

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


void CTasksProcessor::GetMapPendingBratTaskToProcess( std::vector<uid_t> *tasks2process )
{
	if ( !tasks2process )
		return;

	//wxCriticalSectionLocker locker( m_critSectMapBratTask );			//TODO

	for ( CMapBratTask::const_iterator it = mPendingTasksMap.begin(); it != mPendingTasksMap.end(); it++ )
	{
		CBratTask* bratTask = it->second;
		if ( !bratTask || bratTask->laterThanNow() )		//if (bratTask->GetAt() > dateRef)		femm
			continue;

		tasks2process->push_back( bratTask->GetUid() );
	}
}




///////////////////////////////////////
///////////////////////////////////////
//			Tasks Processing
///////////////////////////////////////
///////////////////////////////////////


///////////////////////////////////////
//			Add / Remove



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// brat interface - begin /////////////////////////////////////////////////////////////////////////////////////

CBratTask* CTasksProcessor::CreateTaskAsPending( bool function, CBratTask *parent, const std::string& cmd_or_function, CVectorBratAlgorithmParam& params, const QDateTime& at, const std::string& name, const std::string& log_dir )
{
	auto id = GenerateId();

	std::string log_file_name =  "/BratTask_" + name + "_" + n2s<std::string>( id ) + ".log";

	CBratTask *task = nullptr;
	if ( function )
	{
		CBratTaskFunction* functionRef = CMapBratTaskFunction::GetInstance().Find( cmd_or_function );
		if ( functionRef == nullptr )
		{
			std::string msg = "Unable to create Brat task - Function '" + cmd_or_function + "' is unknown.";
			throw CException( msg, BRATHL_ERROR );
		}

		CBratTaskFunction f = *functionRef;
        task = new CBratTask( id, name, f, at, CBratTask::eBRAT_STATUS_PENDING, log_dir + log_file_name );
        CVectorBratAlgorithmParam *fparams = task->GetBratTaskFunction()->GetParams();
		for ( CVectorBratAlgorithmParam::const_iterator it = params.begin(); it != params.end(); it++ )
		{
			fparams->Insert( *it );
		}
	}
	else
	{
		task = new CBratTask( id, name, cmd_or_function, at, CBratTask::eBRAT_STATUS_PENDING, log_dir + log_file_name );
	}


	//if ( IsPendingTasksElement( parent.n ) )
	if ( !parent )
	{
		AddTaskToMap( task );
	}
	else
	{
		AddTask2Parent( parent->GetUid(), task );
	}

	//if ( !CTasksProcessor::SaveSchedulerTaskConfig() )
	//	RemoveTaskFromSchedulerTaskConfig( r.n );

	if ( !SaveAllTasks() )
	{
		RemoveTaskFromMap( id );
		return nullptr;
	}

	return task;
}
CBratTask* CTasksProcessor::CreateCmdTaskAsPending( CBratTask *parent, const std::string &cmd, const QDateTime& at, const std::string& name, const std::string& log_dir )
{
	CVectorBratAlgorithmParam params;
	return CreateTaskAsPending( false, parent, cmd, params, at, name, log_dir );
}


CBratTask* CTasksProcessor::CreateFunctionTaskAsPending( CBratTask *parent, const std::string &function, CVectorBratAlgorithmParam& params, const QDateTime& at, const std::string& name, const std::string& log_dir )
{
    return CreateTaskAsPending( true, parent, function, params, at, name, log_dir );
}

// brat interface - end ///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



void CTasksProcessor::AddTaskToMap( CBratTask* bratTask )
{
	if ( !bratTask )
		return;

	//wxCriticalSectionLocker locker( m_critSectMapBratTask );		// TODO

	uid_t uid = bratTask->GetUid();
	m_mapBratTask.Insert( uid, bratTask );

	CBratTask::EStatus status = bratTask->GetStatus();
	switch ( status )
	{
		case CBratTask::eBRAT_STATUS_PENDING:
		{
			mPendingTasksMap.Insert( uid, bratTask );
			break;
		}
		case CBratTask::eBRAT_STATUS_PROCESSING:
		{
			mProcessingTasksMap.Insert( uid, bratTask );
			break;
		}
		case CBratTask::eBRAT_STATUS_ENDED:
		case CBratTask::eBRAT_STATUS_ERROR:
		case CBratTask::eBRAT_STATUS_WARNING:
		{
			mEndedTasksMap.Insert( uid, bratTask );
			break;
		}
		default:
		{
			std::string msg =
				"ERROR in CTasksProcessor#AddTaskToMap - status "
				+ n2s<std::string>( static_cast<int>( status ) )
				+ " ("
				+ bratTask->GetStatusAsString()
				+ ") not implemented in the function.";

			throw CException( msg, BRATHL_ERROR );
		}
	}
}


void CTasksProcessor::AddTask2Parent( uid_t parentId, CBratTask* bratTask )
{
	if ( !bratTask )
		return;

	//wxCriticalSectionLocker locker( m_critSectMapBratTask );			//TODO

	CBratTask* parentBratTask = m_mapBratTask.Find( parentId );
	if ( !parentBratTask )
		return;

	parentBratTask->GetSubordinateTasks()->Insert( bratTask );
}



#if defined(BRAT_V3)

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

#endif




bool CTasksProcessor::RemoveTaskFromMap( uid_t id )
{
	// ensure that no other thread accesses the id
	//wxCriticalSectionLocker locker( m_critSectMapBratTask );			//TODO

	std::string idAsString = n2s<std::string>( id );

	CBratTask* bratTask = m_mapBratTask.Find( id );
	if ( bratTask == nullptr )
	{
		return false;
	}

	CBratTask::EStatus status = bratTask->GetStatus();

	if ( status == CBratTask::eBRAT_STATUS_PROCESSING )
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

	mPendingTasksMap.Remove( id );
	mProcessingTasksMap.Remove( id );
	mEndedTasksMap.Remove( id );

	// Remove log file
	RemoveFile( bratTask->GetLogFile() );	//->GetFullPath()

	m_mapBratTask.Remove( id );

	return true;
}



///////////////////////////////////////
//			Change Status


//----------------------------------------------------
bool CTasksProcessor::ChangeProcessingToPending( CVectorBratTask& vectorTasks )
{
	vectorTasks.SetDelete( false );

	// tasks will be deleted from mProcessingTasksMap by ChangeTaskStatus function
	// Don't change status within the loop, store tasks into a vecor and then change status from std::vector tasks
	for ( CMapBratTask::const_iterator it = mProcessingTasksMap.begin(); it != mProcessingTasksMap.end(); it++ )
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

		ChangeTaskStatus( bratTask->GetUid(), CBratTask::eBRAT_STATUS_PENDING );
	}

	return vectorTasks.size() > 0;
}


CTasksProcessor::EStatus CTasksProcessor::ChangeTaskStatus( uid_t id, EStatus newStatus )
{
	//ChangeTaskStatusFromXml( id, newStatus );
	//CBratTask::EStatus currentStatus = ChangeTaskStatusFromMap( id, newStatus );

	//wxCriticalSectionLocker locker(m_critSectMapBratTask);		// TODO

	std::string idAsString = n2s<std::string>( id );

	CBratTask* bratTask = m_mapBratTask.Find( id );

	if ( bratTask == nullptr )
	{
		std::string msg =
			"Unable to update task status: task uid '"
			+ idAsString
			+ "' is not found (CTasksProcessor#ChangeTaskStatusFromMap).";

		throw CException( msg, BRATHL_ERROR );
	}

	EStatus currentStatus = bratTask->GetStatus();

	switch ( currentStatus )
	{
		case CBratTask::eBRAT_STATUS_PENDING:
		{
			mPendingTasksMap.Remove( id );
			break;
		}
		case CBratTask::eBRAT_STATUS_PROCESSING:
		{
			mProcessingTasksMap.Remove( id );
			break;
		}
		case CBratTask::eBRAT_STATUS_ENDED:
		case CBratTask::eBRAT_STATUS_ERROR:
		case CBratTask::eBRAT_STATUS_WARNING:
		{
			mEndedTasksMap.Remove( id );
			break;
		}
		default:
		{
			std::string msg =
				"ERROR in CTasksProcessor#ChangeTaskStatusFromMap - status "
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
		case CBratTask::eBRAT_STATUS_PENDING:
		{
			mPendingTasksMap.Insert( id, bratTask );
			break;
		}
		case CBratTask::eBRAT_STATUS_PROCESSING:
		{
			mProcessingTasksMap.Insert( id, bratTask );
			break;
		}
		case CBratTask::eBRAT_STATUS_ENDED:
		case CBratTask::eBRAT_STATUS_ERROR:
		case CBratTask::eBRAT_STATUS_WARNING:
		{
			mEndedTasksMap.Insert( id, bratTask );
			break;
		}
		default:
		{
			std::string msg =
				"ERROR in CTasksProcessor#ChangeTaskStatusFromMap - status "
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
