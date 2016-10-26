/*
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
#include "stdafx.h"

#include "new-gui/Common/+Utils.h"
#include "new-gui/Common/+UtilsIO.h"
#include "new-gui/Common/QtUtilsIO.h"

#include "new-gui/Common/GUI/ApplicationUserPaths.h"

#include "TaskProcessor.h"




///////////////////////////////////////
///////////////////////////////////////
//			Singleton
///////////////////////////////////////
///////////////////////////////////////


//static 
const std::string CTasksProcessor::smFileName = "SchedulerTasksConfig";		//extension ".xml" must be added
//static 
bool CTasksProcessor::smFactoryCalled = false;
//static 
CTasksProcessor* CTasksProcessor::smInstance = nullptr;
//static 
std::function< CTasksProcessor*( const std::string &path, bool lockFile, bool unlockFile ) > CTasksProcessor::smFactory = nullptr;


//static 
CTasksProcessor* CTasksProcessor::CreateInstance( const std::string&scheduler_name, const CApplicationUserPaths &app_paths )
{
#if defined(BRAT_V3)
	static const std::string path = scheduler_name;	//HACK scheduler_name used as tasks file path
#else
	static const std::string path = app_paths.DefaultUserDataPath4Application( scheduler_name, true ) + "/" + smFileName + ".xml";
#endif
	static bool called = false;			assert__( !called );
	called = true;

    if ( !smFactory )
    {
        CreateFactory();
    }

    if ( !smInstance )
	{
		std::string error_msg;
		try
		{
            smInstance = smFactory( path, true, true );		//factory calls constructor (of course), which loads XML
		}
		catch ( CException& e )
		{
			error_msg = e.Message();
		}

		if ( !error_msg.empty() )
		{
			std::string msg = 
					"Unable to load Brat Scheduler configuration file '"
					+ path
					+ "'- Please, check XML syntax - Parser error: '"
					+ error_msg
					+ "'";

			throw CException( msg, BRATHL_ERROR );
		}
	}
	
	return GetInstance();		//return GetInstance( reload, lockFile, unlockFile );
}



///////////////////////////////////////
///////////////////////////////////////
//			Serialization
///////////////////////////////////////
///////////////////////////////////////



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


void TestLock( const QtLockedFile &lf )
{
#if defined(DEBUG) || defined(_DEBUG)
	int m = lf.lockMode();
	if ( m == 0 )
		qDebug() << "\n[*] You have no locks.";
	else if ( m == QFile::ReadOnly )
		qDebug() << "\n[*] You have a read lock.";
	else
	{
		assert__( m == QFile::ReadWrite || m == QFile::WriteOnly );
		qDebug() << "\n[*] You have a write or read/write lock.";
	}
#else
	Q_UNUSED( lf );
#endif
}


bool CTasksProcessor::Unlock( const std::string &lock_path )
{
	bool result = mLockedFile.unlock();
	if ( !result )
		LOG_WARN( "Could not unlock file " + lock_path );	//smLockedFile.flush();

	mLockedFile.close();
	return result;
}

bool CTasksProcessor::TryLock( const std::string &lock_path, QtLockedFile::OpenMode open_mode, QtLockedFile::LockMode lock_mode, bool block )
{
	bool result = false;
	if ( !mLockedFile.open( open_mode ) )
		LOG_WARN( "Locked file " + lock_path + " failed to open" );
	else
	{
		TestLock( mLockedFile );
		//
		if ( mLockedFile.lock( lock_mode, block ) )
		{
			result = true;
			qDebug() << "done!\n";
		}
		else
		{
			qDebug() << "not currently possible.";
		}
		qDebug() << "Blocking: " << ( block ? "yes" : "no" );
		//
		TestLock( mLockedFile );
	}
	return result;
}

bool CTasksProcessor::store( const std::string &path, bool block )
{
	std::string lock_path = q2a( mLockedFile.fileName() );		assert__( lock_path == m_fullFileName );
	bool lock = NormalizedPath( path ) == lock_path;
	bool result = !lock;
	{
		std::ofstream ofs( path, std::ofstream::out );			//do not use " | std::ofstream::trunc "
		if ( lock )
			result = TryLock( lock_path, QFile::ReadWrite, QtLockedFile::WriteLock, block );

		result = result && ::store( *this, ofs );
	}

	if ( lock )
		Unlock( lock_path );

	return result;	//return ::store( *this, path );
}

bool CTasksProcessor::load( const std::string &path, bool block )
{
	std::string lock_path = q2a( mLockedFile.fileName() );		assert__( lock_path == m_fullFileName );
	bool lock = NormalizedPath( path ) == lock_path;
	bool result = !lock;
	{
		std::ifstream ifs( path, std::ifstream::in );
		if ( lock )
			result = TryLock( lock_path, QFile::ReadOnly, QtLockedFile::ReadLock, block );

		result = result && ::load( *this, ifs );
	}

	if ( lock )
		Unlock( lock_path );

	return result;	//return ::load( *this, path );
}


//virtual 
bool CTasksProcessor::LockConfigFile( bool block, bool lockFile )	//lockFile = true 
{
    UNUSED( block );		UNUSED( lockFile );

	return true;
}
//virtual 
void CTasksProcessor::UnLockConfigFile( bool unlockFile )
{
    UNUSED( unlockFile );
}






template<>
struct serializer_traits< CTasksProcessor >
{
	typedef bratSchedulerConfig converter_t;

	static void
	serialize( ::xercesc::XMLFormatTarget& ft,
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


	static void
	serialize( ::std::ostream& os,		//::xercesc::XMLFormatTarget& ft,
						 const converter_t& x, 
						 const ::xml_schema::namespace_infomap& m = ::xml_schema::namespace_infomap (),
						 const ::std::string& e = "UTF-8",
						 ::xml_schema::flags f = 0)
	{
		::bratSchedulerConfig_( os, x, m, e, f );
	}

	static ::std::auto_ptr< converter_t >
	deserialize( ::std::istream& is, ::xml_schema::flags f, const ::xml_schema::properties& p = ::xml_schema::properties() )
	{
		return ::bratSchedulerConfig_( is, f, p );
	}
};


//virtual 
bool CTasksProcessor::SaveAllTasks()
{
    std::string ex;
    bool result = true;
	try {
		result = store( m_fullFileName, true );		//true: block until save can be done
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
bool CTasksProcessor::LoadAllTasks( bool block, std::string &xml_error_msg )
{
	// lambdas

	auto check_status = []( const CBratTask *task, EStatus expected_status )
	{
		auto status = task->GetStatus();
		if ( status == CBratTask::eBRAT_STATUS_ERROR || status == CBratTask::eBRAT_STATUS_WARNING )
			status = CBratTask::eBRAT_STATUS_ENDED;

		if ( status != expected_status )
			LOG_WARN( 
			"Task '" + task->GetName() + "' in " + CBratTask::TaskStatusToString( expected_status ) 
			+ " table is marked as " + CBratTask::TaskStatusToString( task->GetStatus() ) );
	};


	// function body

	RemoveMapBratTasks();

    bool result = true;

	try {
		if ( !IsFile( m_fullFileName ) )
		{
			result = store( m_fullFileName, true );
		}

		result = result && load( m_fullFileName, block );

#if defined(DEBUG) || defined(_DEBUG)

		if ( result && getenv( "S3ALTB_ROOT" ) )
		{
			std::string debug_path = std::string( getenv( "S3ALTB_ROOT" ) ) + "/project/dev/support/data/scheduler/" + smFileName + "_backup.xml";
			CTasksProcessor debug_tasks( debug_path );
			if ( result )
			{
				result = store( debug_path, false );				//false won't be used
				if ( result )
					result = debug_tasks.load( debug_path, true );
			}

			assert__( result && debug_tasks == *this );
		}

#endif

	}
	catch ( const xml_schema::exception& e )
	{
		translate_exception( xml_error_msg, e );
	    result = false;
	}

	if ( result )
	{
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
		{
			m_mapBratTask.Insert( pt.first, pt.second );		check_status( pt.second, CBratTask::eBRAT_STATUS_PENDING );
		}
		for ( auto &pt : mProcessingTasksMap )
		{
			m_mapBratTask.Insert( pt.first, pt.second );		check_status( pt.second, CBratTask::eBRAT_STATUS_PROCESSING );
		}
		for ( auto &pt : mEndedTasksMap )
		{
			m_mapBratTask.Insert( pt.first, pt.second );		check_status( pt.second, CBratTask::eBRAT_STATUS_ENDED );
		}
	}

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
			CBratTask* bratTaskToAdd = new CBratTask( *bratTaskNew );

			AddTaskToMap( bratTaskToAdd );

			m_mapNewBratTask.Insert( bratTaskToAdd->GetUid(), bratTaskToAdd );
		}
	}

	return true;
}


bool CTasksProcessor::ReloadOnlyNew( bool block, bool lockFile, bool unlockFile )	//bool lockFile = true, bool unlockFile = true 
{
	m_mapNewBratTask.RemoveAll();

	bool bOk = LockConfigFile( block, lockFile );

	if ( !bOk )
	{
		UnLockConfigFile( unlockFile );
		std::string msg = "Unable to load Brat Scheduler configuration file. Perhaps, it's used by another application - Try again later.";
		throw CException( msg, BRATHL_WARNING );
	}

	CTasksProcessor* schedulerTaskConfig = nullptr;
	bOk = true;

	std::string error_msg;
	try
	{
		schedulerTaskConfig = smFactory( m_fullFileName, false, false );		//false, false ignored
	}
	catch ( CException& e )
	{
		bOk = false;
		error_msg = e.Message().c_str();
	}

	if ( !bOk )
	{
		std::string msg;
		if ( error_msg.empty() )
		{
			std::string parserError;
			msg =
				"Unable to load Brat Scheduler configuration file '"
				+ m_fullFileName
				+ "' - Please, check XML syntax - Parser error: '"
				+ parserError
				+ "'";
		}
		else
		{
			msg = error_msg;
		}

		delete schedulerTaskConfig;
		schedulerTaskConfig = nullptr;

		this->UnLockConfigFile( unlockFile );

		throw CException( msg, BRATHL_ERROR );
	}

	AddNewTasksFromSibling( schedulerTaskConfig );

	delete schedulerTaskConfig;
	schedulerTaskConfig = nullptr;

	this->UnLockConfigFile( unlockFile );

	return bOk;
}


///////////////////////////////////////
///////////////////////////////////////
//				Access
///////////////////////////////////////
///////////////////////////////////////




CBratTask* CTasksProcessor::FindTaskFromMap( uid_t id )
{
	return m_mapBratTask.Find( id );
}


CBratTask::uid_t CTasksProcessor::GenerateId()
{
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

	std::string log_file_name =  "/BratTask_" + name + "_" + n2s( id ) + ".log";

	CBratTask *task = nullptr;
	if ( function )
	{
		const CBratTaskFunction *functionRef = CMapBratTaskFunction::GetInstance().Find( cmd_or_function );
		if ( functionRef == nullptr )
		{
			std::string msg = "Unable to create Brat task - Function '" + cmd_or_function + "' is unknown.";
			throw CException( msg, BRATHL_ERROR );
		}

        task = new CBratTask( id, name, *functionRef, at, CBratTask::eBRAT_STATUS_PENDING, log_dir + log_file_name );
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
				+ n2s( static_cast<int>( status ) )
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
	std::string idAsString = n2s( id );

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
			+ n2s( static_cast<int>( status ) )
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

	// v3 note: tasks will be deleted from mProcessingTasksMap by ChangeTaskStatus function
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


CTasksProcessor::EStatus CTasksProcessor::ChangeTaskStatus( uid_t id, EStatus new_status )
{
	CBratTask* bratTask = m_mapBratTask.Find( id );

	if ( bratTask == nullptr )
	{
		std::string msg =
			"Unable to update task status: task uid '"
			+ n2s( id )
			+ "' is not found (CTasksProcessor#ChangeTaskStatusFromMap).";

		throw CException( msg, BRATHL_ERROR );
	}

	EStatus current_status = bratTask->GetStatus();

	switch ( current_status )
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
				+ n2s( static_cast<int>( current_status ) )
				+ " ("
				+ bratTask->GetStatusAsString()
				+ ") not implemented in the function.";

			throw CException( msg, BRATHL_ERROR );
		}
	}

	bratTask->SetStatus( new_status );

	switch ( new_status )
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
				+ n2s( static_cast<int>( new_status ) )
				+ " ("
				+ CBratTask::TaskStatusToString( new_status )
				+ ") not implemented in the function.";

			throw CException( msg, BRATHL_ERROR );
		}
	}

	std::string msg = 
		"Task '"
		+ n2s( id )
		+ "' status has been changed from '"
		+ CBratTask::TaskStatusToString( current_status )
		+ "' to '"
		+ CBratTask::TaskStatusToString( new_status )
		+ "'.";

	LOG_INFO( msg );

	return current_status;
}
