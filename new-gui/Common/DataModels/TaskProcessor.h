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
#ifndef TASK_PROCESSOR_H
#define TASK_PROCESSOR_H

#include <functional>

#include "BratTask.h"
#include "new-gui/Common/ScheduledTasksList.hxx"
#include "new-gui/Common/XmlSerializer.h"


class CConsoleApplicationPaths;


// for serialization
//
class bratSchedulerConfig;


class CTasksProcessor
{
	///////////////////////////////////////
	//	types
	///////////////////////////////////////

public:
	typedef CBratTask::uid_t	uid_t;

protected:

	typedef CBratTask::EStatus	EStatus;

	///////////////////////////////////////
	//static members
	///////////////////////////////////////


	static const std::string smFileName;

	static bool mFactoryCalled;
    static CTasksProcessor* smInstance;
	static std::function<CTasksProcessor*( const std::string &filename, bool lockFile, bool unlockFile )> mFactory;


	//....construction

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

    static CTasksProcessor* GetInstance( const std::string* fileName, bool reload = false, bool lockFile = true, bool unlockFile = true );

public:
	static CTasksProcessor* GetInstance( bool reload = false, bool lockFile = true, bool unlockFile = true );

	static CTasksProcessor* CreateInstance( const CConsoleApplicationPaths &app_paths, bool reload = false, bool lockFile = true, bool unlockFile = true );

	static void DestroyInstance()
	{
		assert__( mFactoryCalled );
        delete smInstance;
        smInstance = nullptr;
	}



	///////////////////////////////////////
	//instance data
	///////////////////////////////////////

private:

	uid_t m_lastId;

protected:
	std::string m_fullFileName;

	bool m_reloadAll = false;

private:
	// Map for all tasks (the reference map)
	//
	CMapBratTask m_mapBratTask;

	// Map for pending tasks - Warning : don't delete object stored in it.
	//
	CMapBratTask mPendingTasksMap;

	// Map for processing tasks - Warning : don't delete object stored in it.
	//
	CMapBratTask mProcessingTasksMap;

	// Map for ended (or error) tasks - Warning : don't delete object stored in it.
	//
	CMapBratTask mEndedTasksMap;

	// Map used to stored new pending tasks when file is reloaded.
	// Tasks are just added as reference - Warning : don't delete object stored in it.
	//
	CMapBratTask m_mapNewBratTask;



	///////////////////////////////////////
	//construction / destruction				//- see also static section, singleton related
	///////////////////////////////////////

protected:

#if defined(BRAT_V3) || defined(DEBUG) || defined(_DEBUG)
	CTasksProcessor() :
		  mPendingTasksMap( false )
		, mProcessingTasksMap( false )
		, mEndedTasksMap( false )
		, m_mapNewBratTask( false )
	{}
#endif


//called by factory, also used by CSchedulerTaskConfig

	CTasksProcessor( const std::string &filename, bool lockFile = true, bool unlockFile = true ) : 
		  m_fullFileName( filename )
		, mPendingTasksMap( false )
		, mProcessingTasksMap( false )
		, mEndedTasksMap( false )
		, m_mapNewBratTask( false )
	{
		LoadAndCreateTasks( filename, lockFile, unlockFile );		//if not ... throw ???
	}

public:

	virtual ~CTasksProcessor()
	{}


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// brat interface - begin /////////////////////////////////////////////////////////////////////////////////////

	CBratTask* CreateTaskAsPending( bool function, CBratTask *parent, const std::string& cmd_or_function, CVectorBratAlgorithmParam& params, const QDateTime& at, const std::string& name, const std::string& log_dir );
	CBratTask* CreateCmdTaskAsPending( CBratTask *parent, const std::string& cmd, const QDateTime& at, const std::string& name, const std::string& log_dir );
	CBratTask* CreateFunctionTaskAsPending( CBratTask *parent, const std::string& function, CVectorBratAlgorithmParam& params, const QDateTime& at, const std::string& name, const std::string& log_dir );

	virtual bool LoadAllTasks();

	////////////////
	// Find

	CBratTask* FindTaskFromMap( uid_t id );

protected:

	void AddTask2Parent( uid_t parentId, CBratTask* task );

	bool RemoveTaskFromMap( uid_t id );


	// brat interface - end ///////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

	
public:

	//access 
	
	const std::string& SchedulerFilePath() const { return m_fullFileName; }



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


	///////////////////////////////////////
	//	serialization
	///////////////////////////////////////

protected:
	virtual bool LoadAndCreateTasks( const std::string& fileName, bool lockFile = true, bool unlockFile = true );

	bool LoadAndCreateTasks( bool lockFile = true, bool unlockFile = true )
	{
		return LoadAndCreateTasks( m_fullFileName, lockFile, unlockFile );
	}


	virtual bool AddNewTasksFromSibling( CTasksProcessor* sched );

	
public:

	// scheduler interface

	bool ReloadOnlyNew( //std::function<CTasksProcessor*(const std::string &filename, bool lockFile, bool unlockFile)> factory,
		bool lockFile = true, bool unlockFile = true );

	void GetMapPendingBratTaskToProcess( std::vector<uid_t> *tasks2process );	//const wxDateTime& dateref,  femm

	bool HasMapNewBratTask() const { return ( m_mapNewBratTask.size() > 0 ); }

	virtual bool SaveAllTasks();

public:

    CTasksProcessor& operator = ( const bratSchedulerConfig &o );		//used by global load

    bratSchedulerConfig& IOcopy( bratSchedulerConfig &oxml ) const;		//used by global store

protected:


	bool store( const std::string &path );

	bool load( const std::string &path );


	///////////////////////////////////////
	//			Locks
	///////////////////////////////////////

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
	//			CSchedulerTaskConfig methods
	/////////////////////////////////////////////////////

public:

	CMapBratTask* GetMapPendingBratTask() { return &mPendingTasksMap; }
	CMapBratTask* GetMapEndedBratTask() { return &mEndedTasksMap; }

protected:

    bool IsReloadAll() { return m_reloadAll; }


	////////////////
	// Add / Remove


	virtual void AddTaskToMap( CBratTask* task );

	virtual void RemoveMapBratTasks()
	{
		//wxCriticalSectionLocker locker( m_critSectMapBratTask );		//TODO

		mPendingTasksMap.RemoveAll();
		mProcessingTasksMap.RemoveAll();
		mEndedTasksMap.RemoveAll();
		m_mapNewBratTask.RemoveAll();

		m_mapBratTask.RemoveAll();
	}


	////////////////
	// 

	uid_t GenerateId();


#if defined(BRAT_V3)

	/////////////////////////////////////////////////////
	//			Accessors

public:
    virtual bool IsOk() const { return true; }

	const CMapBratTask* GetMapNewBratTask() const { return &m_mapNewBratTask; }


protected:
//were private
    CMapBratTask* GetMapBratTask() { return &m_mapBratTask; }
    CMapBratTask* GetMapProcessingBratTask() { return &mProcessingTasksMap; }



	/////////////////////////////////////////////////////
	//			Add / Remove


	void AddTask( const CMapBratTask* mapBratTask );


#endif	//BRAT_V3


	/////////////////////////////////////////////////////
	//			Change

public:
	// scheduler interface

	virtual EStatus ChangeTaskStatus( uid_t id, EStatus newStatus );

	bool ChangeProcessingToPending( CVectorBratTask& vectorTasks );

	bool RemoveTaskFromMap( CBratTask* task )
	{
		if ( !task )
			return false;

		return RemoveTaskFromMap( task->GetUid() );
	}
};



#endif // TASK_PROCESSOR_H
