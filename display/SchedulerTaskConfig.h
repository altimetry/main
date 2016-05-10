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

#ifndef __SchedulerTaskConfig_H__
#define __SchedulerTaskConfig_H__

#include "new-gui/brat/stdafx.h"

#if defined (__APPLE__)
    #if defined (Q_FORWARD_DECLARE_OBJC_CLASS)
        #undef Q_FORWARD_DECLARE_OBJC_CLASS
    #endif
    #define Q_FORWARD_DECLARE_OBJC_CLASS( x )
    #include <qdebug.h>
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "brathl.h"

#include "wx/snglinst.h" // (wxSingleInstanceChecker class)
#include <wx/xml/xml.h> // (wxXmlDocument, wxXmlNode, wxXmlProperty classes)

#include "Date.h"
#include "Tools.h"

#include "new-gui/scheduler/TaskProcessor.h"

using namespace brathl;




// WDR: class declarations

//----------------------------------------------------------------------------
// CSchedulerTaskConfig
//----------------------------------------------------------------------------

//Auxiliary structure to replace xml nodes

struct PseudoXml
{
	std::vector< CBratTask* > ended;
	std::vector< CBratTask* > pending;
	std::vector< CBratTask* > processing;
};

struct TaskRet
{
	wxXmlNode *n = nullptr;
	CBratTask *t = nullptr;
};



class CSchedulerTaskConfig : public CTasksProcessor
{
	typedef CTasksProcessor base_t;

	//wxXmlDocument sub-object

	wxXmlDocument mdoc;

    virtual bool IsOk() const override { return GetRoot_xml() != NULL; }

    // Returns encoding of document (may be empty).
    // Note: this is the encoding original file was saved in, *not* the
    // encoding of in-memory representation!
    const std::string GetFileEncoding() const { return mdoc.GetFileEncoding().ToStdString(); }

    // Returns root node of the document.
	wxXmlNode *GetRoot_xml() const { return mdoc.GetRoot(); }
    wxXmlNode *DetachRoot_xml() { return mdoc.DetachRoot(); }
	void SetRoot_xml( wxXmlNode *node ) { return mdoc.SetRoot( node ); }


	//wxXmlDocument replacer

	PseudoXml mxml;

	////////////////////////////////////////////////////////////////////////

	//wxMutex m_mutex;
	// m_configFileChecker is use to lock and get exclusive access to the configuration file.
	// In the wxWoidgets documentation, we cant find that :
	// "...A critical section object is used for exactly the same purpose as a wxMutex.
	// The only difference is that under Windows platform critical sections are only visible inside one process,
	// while mutexes may be shared among processes, so using critical sections is slightly more efficient."
	// But wxMutex use 'unnamed' mutex for Windows, so in this case mutex are only intra-process and not inter-process.
	// To bypass this, we use wxSingleInstanceChecker interface to  lock and get exclusive access to the configuration file,
	// because under Windows wxSingleInstanceChecker implementation use 'named' mutex.
	wxSingleInstanceChecker* m_configFileChecker = nullptr;

	// constructors
public:
	CSchedulerTaskConfig() : base_t()
	{
		assert__( !m_reloadAll && !m_configFileChecker );
	}

	CSchedulerTaskConfig( const std::string &filename, bool lockFile = true, bool unlockFile = true ) :
		base_t( filename, lockFile, unlockFile )
	{
		assert__( !m_reloadAll && !m_configFileChecker );

		// Don't call wxXmlDocument(filename, encoding) because overriden 'Load' will be not called

		if ( !LoadAndCreateTasks( filename, lockFile, unlockFile ) )
			delete DetachRoot_xml();
	}

	static const std::string m_TASK_ID_ATTR;			  //femm: moved from below
public:

	static CSchedulerTaskConfig* GetInstance( bool reload = false, bool lockFile = true, bool unlockFile = true );

	static bool LoadAllSchedulerTaskConfig( bool quiet = false );
	static bool SaveSchedulerTaskConfig( bool quiet = false );

	//dtor

	virtual ~CSchedulerTaskConfig();

	TaskRet AddTaskAsPending( const std::string& cmd, wxDateTime& at, const std::string& name );
	TaskRet AddTaskAsPending( TaskRet parent, const std::string& cmd, wxDateTime& at, const std::string& name );
	//femm changed name
	TaskRet AddFunctionTaskAsPending( TaskRet parent, const std::string& function, CVectorBratAlgorithmParam& params, wxDateTime& at, const std::string& name );

	wxXmlNode* FindTaskNode_xml( const std::string& taskId, bool allDepths = false );

    static bool IsLoaded() { return smInstance != NULL; }

	void setReloadAll( bool value ) { m_reloadAll = value; }

	bool RemoveTask( wxXmlNode*& node );

	//used only by scheduler

	static std::string m_CONFIG_APPNAME;

	virtual Status ChangeTaskStatus( uid_t id, Status newStatus ) override;

	bool RemoveTask( const wxLongLong& id )
	{
		return RemoveTask( id.GetValue() );
	}
	bool RemoveTask( wxLongLong_t id );

	const std::string& GetFullFileName() { return m_fullFileName; }

	static std::string GetConfigFilePath( const std::string& appName )
	{
		if ( appName.empty() )
			return GetConfigFilePath();

		return CSchedulerTaskConfig::GetUserConfigDir( appName ) + "/" + CSchedulerTaskConfig::m_CONFIG_FILE_NAME;
	}

	static void ForceUnLockConfigFile();

private:
	static std::string GetConfigFilePath()
	{
		return CSchedulerTaskConfig::GetUserDataDir() + "/" + CSchedulerTaskConfig::m_CONFIG_FILE_NAME;
	}


	bool Save( const std::string& fileName, bool lockFile = true, bool unlockFile = true, int indentStep = 2 );
	bool Save( bool lockFile = true, bool unlockFile = true, int indentStep = 2 );
	virtual bool LoadAndCreateTasks( const std::string& fileName, bool lockFile = true, bool unlockFile = true ) override;

	wxXmlNode* GetPendingTasksNode_xml();
	wxXmlNode* GetEndedTasksNode_xml();
	wxXmlNode* GetProcessingTasksNode_xml();

	wxXmlNode* GetOrAddRootNode_xml();
	wxXmlNode* GetOrAddTasksElement_xml( const std::string &elt );
	wxXmlNode* GetOrAddPendingTasksElement_xml() { return GetOrAddTasksElement_xml( m_PENDING_ELT ); }
	wxXmlNode* GetOrAddProcessingTasksElement_xml() { return GetOrAddTasksElement_xml( m_PROCESSING_ELT ); }
	wxXmlNode* GetOrAddEndedTasksElement_xml() { return GetOrAddTasksElement_xml( m_ENDED_ELT ); }

	wxXmlNode* CreateTaskNodeAsPending_xml( wxDateTime& at, const std::string& name, const std::string &property, const std::string &value, const CVectorBratAlgorithmParam *params = nullptr );

	wxXmlNode* FindNodeByName_xml( const std::string& name);	//, wxXmlNode* parent, bool allDepths = false

	wxXmlNode* FindTaskNode_xml( const wxLongLong_t& taskId, bool allDepths = false );
	wxXmlNode* FindTaskNode_xml( const std::string& taskId, wxXmlNode* parent, bool allDepths = false );
	wxXmlNode* FindTaskNode_xml( const wxLongLong_t& taskId, wxXmlNode* parent, bool allDepths = false );

	CBratTask* CreateBratTask( wxXmlNode* taskNode );

	virtual void RemoveMapBratTasks() override;

	virtual bool LoadAllTasks() override;
	bool LoadTasks( wxXmlNode* node );
	bool LoadSubordinateTasks( wxXmlNode* taskNode, CBratTask* bratTask );

	bool SetPropVal( wxXmlNode* node, const std::string& propName, const std::string& value, bool allDepths = true );


	virtual bool LockConfigFile( bool lockFile = true ) override
	{
		return LockConfigFile( m_fullFileName, lockFile );
	}
	virtual bool LockConfigFile( const std::string& fileName, bool lockFile = true ) override;
	virtual void UnLockConfigFile( bool unlockFile ) override;

	//wxMutexError MutexLock();
	//wxMutexError MutexUnLock();

	static std::string GetLogFilePath();
	static std::string GetLogFilePath( const std::string& appName );

	static std::string GetUserDataDir();
	static std::string GetUserConfigDir();
	static std::string GetUserConfigDir( const std::string& appName );

	static bool IsPendingTasksElement( wxXmlNode* node );

	static std::string GetTaskIdAsString( wxXmlNode* node );
	static uid_t GetTaskId( wxXmlNode* node );

	static CSchedulerTaskConfig* GetInstance( const std::string* fileName, bool reload = false, bool lockFile = true, bool unlockFile = true );
	//static CSchedulerTaskConfig* GetInstance(const std::string* fileName, bool reload = false, const std::string& encoding = "UTF-8");

	bool ParseDateTime( const std::string& value, wxDateTime& dt, std::string& error );


	// WDR: method declarations for SchedulerTaskConfig

	static const std::string m_ENDED_ELT;
	static const std::string m_PENDING_ELT;
	static const std::string m_PROCESSING_ELT;
	static const std::string m_ROOT_ELT;
	static const std::string m_TASK_ELT;
	static const std::string m_ARG_ELT;

	//static const std::string m_TASK_ID_ATTR;			  //femm: see above in public section
	static const std::string m_TASK_CMD_ATTR;
	static const std::string m_TASK_FUNCTION_ATTR;
	static const std::string m_TASK_AT_ATTR;
	static const std::string m_TASK_LOG_FILE_ATTR;
	static const std::string m_TASK_NAME_ATTR;
	static const std::string m_TASK_RETURN_CODE_ATTR;
	static const std::string m_TASK_STATUS_ATTR;
	static const std::string m_TYPE_ATTR;

//	static const std::string m_DEFAULT_TASK_NAME;

	static const std::string& encoding()
	{
		static const std::string e( "UTF-8" );
		return e;
	}

private:
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

	private:
		CTasksProcessor **m_pAddressOfSchedulerTaskConfigPtr;
		wxLogBuffer **m_pAddressOfSchedulerTaskConfigLogger;


	public:
		CSmartCleaner( CTasksProcessor **addressOfSchedulerTaskConfigPtr, wxLogBuffer **addressOfSchedulerTaskConfigLogger )
			: m_pAddressOfSchedulerTaskConfigPtr( addressOfSchedulerTaskConfigPtr ),
			m_pAddressOfSchedulerTaskConfigLogger( addressOfSchedulerTaskConfigLogger )
		{
		}

		virtual ~CSmartCleaner()
		{
			//cout << "Enter ~CSmartCleaner" << std::endl;

			if ( *m_pAddressOfSchedulerTaskConfigLogger != NULL )
			{
				delete *m_pAddressOfSchedulerTaskConfigLogger;
				*m_pAddressOfSchedulerTaskConfigLogger = NULL;
			}

			if ( *m_pAddressOfSchedulerTaskConfigPtr != NULL )
			{
				delete *m_pAddressOfSchedulerTaskConfigPtr;
				*m_pAddressOfSchedulerTaskConfigPtr = NULL;

			}
		}

		CSchedulerTaskConfig* GetObject() { return dynamic_cast<CSchedulerTaskConfig*>( *m_pAddressOfSchedulerTaskConfigPtr ); }
		wxLogBuffer * GetObjectLogger() { return *m_pAddressOfSchedulerTaskConfigLogger; }
	};

private:

	wxDateTime m_dateTimeFullFileNameMod;

	static const std::string m_CONFIG_FILE_NAME;
	static const std::string m_LOG_REL_PATH;

	static wxLogBuffer* m_logBuffer;

	// Initial reference of the wxLog object - Don't delete
	static wxLog* m_initialLog;

	static wxCriticalSection m_critSectSchedulerTaskConfigInstance;

	wxCriticalSection m_critSectXmlConfigFile;
	wxCriticalSection m_critSectMapBratTask;

	wxCriticalSection m_critSectConfigFileChecker;

	// WDR: member variable declarations for SchedulerTaskConfig

private:

	void NewConfigFileChecker( const std::string& fileName );
	void DeleteConfigFileChecker();

	virtual bool AddNewTasksFromSibling( CTasksProcessor* sched ) override;

	//std::string GetLastIdAsString(bool generateNewId);
	//wxLongLong_t GetLastIdAsLongLong(bool generateNewId);


	static bool CreateXmlFile( const std::string& fileName );

	static void PrepareSmartCleaner();

	// WDR: handler declarations for SchedulerTaskConfig
};






#endif
