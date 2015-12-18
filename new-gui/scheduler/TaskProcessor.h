#ifndef TASK_PROCESSOR_H
#define TASK_PROCESSOR_H

#include <functional>

#include "BratTask.h"
#include "new-gui/Common/ScheduledTasksList.hxx"
#include "new-gui/Common/XmlSerializer.h"



class bratSchedulerConfig;


///////////////////////////////////////
//			CTasksProcessor
///////////////////////////////////////

class CTasksProcessor
{
	typedef std::vector<CBratTask*> tasks_set_t;

protected:

	typedef CBratTask::uid_t	uid_t;
	typedef CBratTask::Status	Status;

private:
	tasks_set_t mPendingTasks;
	tasks_set_t mProcessingTasks;
	tasks_set_t mEndedTasks;

	uid_t m_lastId;

protected:
	std::string m_fullFileName;

	static bool mFactoryCalled;
	static CTasksProcessor* mInstance;
	static std::function<CTasksProcessor*( const std::string &filename, bool lockFile, bool unlockFile )> mFactory;

	bool m_reloadAll = false;

private:
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

protected:
	template< class DERIVED = CTasksProcessor >
	static void CreateFactory()
	{
		mFactory = []( const std::string &filename, bool lockFile, bool unlockFile )
		{
			mFactoryCalled = true;
			return new DERIVED( filename, lockFile, unlockFile );
		};
	}

	CTasksProcessor() :
		  m_mapPendingBratTask( false )
		, m_mapProcessingBratTask( false )
		, m_mapEndedBratTask( false )
		, m_mapNewBratTask( false )
	{}

public:	//TODO: pass to private after transition
	CTasksProcessor( const std::string &filename, bool lockFile = true, bool unlockFile = true ) : 
		  m_fullFileName( filename )
		, m_mapPendingBratTask( false )
		, m_mapProcessingBratTask( false )
		, m_mapEndedBratTask( false )
		, m_mapNewBratTask( false )
	{
		LoadAndCreateTasks( filename, lockFile, unlockFile );		//if not ... throw ???
	}

public:
    static CTasksProcessor* GetInstance( const std::string* fileName, bool reload = false, bool lockFile = true, bool unlockFile = true );
	static CTasksProcessor* GetInstance( bool reload = false, bool lockFile = true, bool unlockFile = true )
	{
		const std::string path = std::string( getenv( "S3ALTB_ROOT") ) + "/project/dev/source/new-gui/scheduler/BratSchedulerTasksConfig.xml";
		//std::string fileName = GetConfigFilePath( CSchedulerTaskConfig::m_CONFIG_APPNAME );
        return GetInstance( &path, reload, lockFile, unlockFile );

		return mInstance;
	}

	static void DestroyInstance()
	{
		assert__( mFactoryCalled );
		delete mInstance;
		mInstance = nullptr;
	}


	virtual ~CTasksProcessor()
	{
		//destroyPointersAndContainer( mPendingTasks );
		//destroyPointersAndContainer( mProcessingTasks );
		//destroyPointersAndContainer( mEndedTasks );
	}


protected:

	bool operator == ( const CTasksProcessor &o ) const 
	{
		auto equal_vectors = []( const tasks_set_t &v1, const tasks_set_t &v2 )
		{
			const size_t size = v1.size();
			if ( size != v2.size() )
				return false;

			for ( size_t i = 0; i < size; ++i )
				if ( *v1[ i ] != *v2[ i ] )
					return false;

			return true;
		};

		return 
			equal_vectors( mPendingTasks, o.mPendingTasks ) &&
			equal_vectors( mProcessingTasks, o.mProcessingTasks ) &&
			equal_vectors( mEndedTasks, o.mEndedTasks );
	}


	///////////////
	//serialization

public:		//TODO pass to private after transition to new scheduler
	virtual bool LoadAndCreateTasks( const std::string& fileName, bool lockFile = true, bool unlockFile = true );
protected:
	bool LoadAndCreateTasks( bool lockFile = true, bool unlockFile = true )
	{
		return LoadAndCreateTasks( m_fullFileName, lockFile, unlockFile );
	}


	virtual bool AddNewTasksFromSibling( CTasksProcessor* sched );

	
public:		//TODO pass to private after transition to new scheduler

	bool ReloadOnlyNew( /*std::function<CTasksProcessor*(const std::string &filename, bool lockFile, bool unlockFile)> factory, */
		bool lockFile = true, bool unlockFile = true );

public:

    CTasksProcessor& operator = ( const bratSchedulerConfig &o );			//USED ON LOAD

    bratSchedulerConfig& IOcopy( bratSchedulerConfig &oxml ) const;			//USED ON STORE

protected:


	bool store( const std::string &path );

	bool load( const std::string &path );



	/////////////////////////////////////////////////////
	//			CSchedulerTaskConfig methods
	/////////////////////////////////////////////////////

	/////////////////////////////////////////////////////
	//			Accessors

public:
    virtual bool IsOk() const { return true; }

    bool IsReloadAll() { return m_reloadAll; }

	CMapBratTask* GetMapPendingBratTask() { return &m_mapPendingBratTask; }
	CMapBratTask* GetMapEndedBratTask() { return &m_mapEndedBratTask; }

	const CMapBratTask* GetMapNewBratTask() const { return &m_mapNewBratTask; }
	bool HasMapNewBratTask() const { return ( m_mapNewBratTask.size() > 0 ); }


	CBratTask* FindTaskFromMap( uid_t id );

	uid_t GenerateId();

	void GetMapPendingBratTaskToProcess(/*const wxDateTime& dateref,  femm*/std::vector<uid_t>* vectorBratTaskToProcess );


protected:
//were private
    CMapBratTask* GetMapBratTask() { return &m_mapBratTask; }
    CMapBratTask* GetMapProcessingBratTask() { return &m_mapProcessingBratTask; }



	/////////////////////////////////////////////////////
	//			Locks

protected:
	virtual bool LockConfigFile( bool lockFile = true )
	{
		// TODO

		return true;
	}
	virtual bool LockConfigFile( const std::string& fileName, bool lockFile = true )
	{
		// TODO

		return true;
	}
	virtual void UnLockConfigFile( bool unlockFile )
	{
		// TODO
	}



	/////////////////////////////////////////////////////
	//			Add / Remove

	virtual bool LoadAllTasks();


	virtual void AddTaskToMap( CBratTask* bratTask );

	void AddTask( uid_t parentId, CBratTask* bratTask );
	void AddTask( const CMapBratTask* mapBratTask );



	virtual void RemoveMapBratTasks()
	{
		//wxCriticalSectionLocker locker( m_critSectMapBratTask );		//TODO

		m_mapPendingBratTask.RemoveAll();
		m_mapProcessingBratTask.RemoveAll();
		m_mapEndedBratTask.RemoveAll();
		m_mapNewBratTask.RemoveAll();

		m_mapBratTask.RemoveAll();
	}


	bool RemoveTaskFromMap( CBratTask* bratTask )
	{
		if ( !bratTask )
			return false;

		return RemoveTaskFromMap( bratTask->GetUid() );
	}
	bool RemoveTaskFromMap( uid_t id );



	/////////////////////////////////////////////////////
	//			Change

	virtual Status ChangeTaskStatus( uid_t id, Status newStatus );
public:

	bool ChangeProcessingToPending( CVectorBratTask& vectorTasks );

};



#endif // TASK_PROCESSOR_H
