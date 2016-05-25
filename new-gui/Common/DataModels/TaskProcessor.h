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
	//tasks_set_t mPendingTasks;
	//tasks_set_t mProcessingTasks;
	//tasks_set_t mEndedTasks;

	uid_t m_lastId;

protected:
	std::string m_fullFileName;

	static bool mFactoryCalled;
    static CTasksProcessor* smInstance;
	static std::function<CTasksProcessor*( const std::string &filename, bool lockFile, bool unlockFile )> mFactory;

	bool m_reloadAll = false;

private:
	// Map for all tasks (the reference map)
	CMapBratTask m_mapBratTask;
	// Map for pending tasks - Warning : don't delete object stored in it.
	CMapBratTask mPendingTasksMap;
	// Map for processing tasks - Warning : don't delete object stored in it.
	CMapBratTask mProcessingTasksMap;
	// Map for ended (or error) tasks - Warning : don't delete object stored in it.
	CMapBratTask mEndedTasksMap;
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
		  mPendingTasksMap( false )
		, mProcessingTasksMap( false )
		, mEndedTasksMap( false )
		, m_mapNewBratTask( false )
	{}

public:	//TODO: pass to private after transition
	CTasksProcessor( const std::string &filename, bool lockFile = true, bool unlockFile = true ) : 
		  m_fullFileName( filename )
		, mPendingTasksMap( false )
		, mProcessingTasksMap( false )
		, mEndedTasksMap( false )
		, m_mapNewBratTask( false )
	{
		LoadAndCreateTasks( filename, lockFile, unlockFile );		//if not ... throw ???
	}

protected:
    static CTasksProcessor* GetInstance( const std::string* fileName, bool reload = false, bool lockFile = true, bool unlockFile = true );

public:
    static CTasksProcessor* GetInstance( bool reload = false, bool lockFile = true, bool unlockFile = true )
    {
        if ( !smInstance || smInstance->m_fullFileName.empty())
            return nullptr;

        return GetInstance( &smInstance->m_fullFileName, reload, lockFile, unlockFile );
    }
    static CTasksProcessor* CreateInstance( const std::string &exec_path, bool reload = false, bool lockFile = true, bool unlockFile = true )
	{
        const std::string path = exec_path + "/BratSchedulerTasksConfig.xml";

        return GetInstance( &path, reload, lockFile, unlockFile );
	}

	static void DestroyInstance()
	{
		assert__( mFactoryCalled );
        delete smInstance;
        smInstance = nullptr;
	}


	virtual ~CTasksProcessor()
	{
		//destroyPointersAndContainer( mPendingTasks );
		//destroyPointersAndContainer( mProcessingTasks );
		//destroyPointersAndContainer( mEndedTasks );
	}


	// brat interface - begin /////////////////////////////////////////////////////////////////////////////////////

	CBratTask* CreateTaskAsPending( bool function, CBratTask *parent, const std::string& cmd_or_function, CVectorBratAlgorithmParam& params, const QDateTime& at, const std::string& name, const std::string& log_dir );
	CBratTask* CreateCmdTaskAsPending( CBratTask *parent, const std::string& cmd, const QDateTime& at, const std::string& name, const std::string& log_dir );
	CBratTask* CreateFunctionTaskAsPending( CBratTask *parent, const std::string& function, CVectorBratAlgorithmParam& params, const QDateTime& at, const std::string& name, const std::string& log_dir );


	// brat interface - end ///////////////////////////////////////////////////////////////////////////////////////

protected:

	bool operator == ( const CTasksProcessor &o ) const 
	{
		auto equal_maps = []( const CMapBratTask &v1, const CMapBratTask &v2 )
		{
			const size_t size = v1.size();
			if ( size != v2.size() )
				return false;

			for ( auto const &entry : v1 )
			{
				auto it = v2.find( entry.first );
				if ( it == v2.end() )
					return false;

				if ( *it->second != *entry.second )
					return false;
			}

			return true;
		};

		return 
			equal_maps( mPendingTasksMap, o.mPendingTasksMap ) &&
			equal_maps( mProcessingTasksMap, o.mProcessingTasksMap ) &&
			equal_maps( mEndedTasksMap, o.mEndedTasksMap );
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

	CMapBratTask* GetMapPendingBratTask() { return &mPendingTasksMap; }
	CMapBratTask* GetMapEndedBratTask() { return &mEndedTasksMap; }

	const CMapBratTask* GetMapNewBratTask() const { return &m_mapNewBratTask; }
	bool HasMapNewBratTask() const { return ( m_mapNewBratTask.size() > 0 ); }


	CBratTask* FindTaskFromMap( uid_t id );

	uid_t GenerateId();

	void GetMapPendingBratTaskToProcess(/*const wxDateTime& dateref,  femm*/std::vector<uid_t>* vectorBratTaskToProcess );


protected:
//were private
    CMapBratTask* GetMapBratTask() { return &m_mapBratTask; }
    CMapBratTask* GetMapProcessingBratTask() { return &mProcessingTasksMap; }



	/////////////////////////////////////////////////////
	//			Locks

protected:
	virtual bool LockConfigFile( bool lockFile = true )
	{
        UNUSED( lockFile );

		// TODO

		return true;
	}
	virtual bool LockConfigFile( const std::string& fileName, bool lockFile = true )
	{
        UNUSED( fileName );         UNUSED( lockFile );

        // TODO

		return true;
	}
	virtual void UnLockConfigFile( bool unlockFile )
	{
        UNUSED( unlockFile );

        // TODO
	}



	/////////////////////////////////////////////////////
	//			Add / Remove

public:
	virtual bool LoadAllTasks();
	virtual bool SaveAllTasks();

protected:

	virtual void AddTaskToMap( CBratTask* bratTask );

	void AddTask2Parent( uid_t parentId, CBratTask* bratTask );
	void AddTask( const CMapBratTask* mapBratTask );



	virtual void RemoveMapBratTasks()
	{
		//wxCriticalSectionLocker locker( m_critSectMapBratTask );		//TODO

		mPendingTasksMap.RemoveAll();
		mProcessingTasksMap.RemoveAll();
		mEndedTasksMap.RemoveAll();
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
