#if !defined GUI_CONTROL_PANELS_PROCESSES_TABLE_H
#define GUI_CONTROL_PANELS_PROCESSES_TABLE_H


#include "ControlPanel.h"



class COperation;


/////////////////////////////////////////////////////////////////////////////////
//							Brat Process Class
/////////////////////////////////////////////////////////////////////////////////


class COsProcess : public QProcess
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

	Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	// types & friends

	using base_t = QProcess;


	// instance data

protected:
	bool mSync = false;
	std::string mName;
	std::string mCmdLine;

	int m_running;
	bool mKilled;

	std::string mOutput;
	void *mUserData = nullptr;

	int mExecuteFlags;

	long mCurrentPid;

	const COperation *mOperation = nullptr;

public:
	COsProcess( const COperation *operation, bool sync, const std::string& name, QWidget *parent, const std::string& cmd, void *user_data = nullptr, const std::string *output = nullptr );

	virtual ~COsProcess();

	const std::string& GetCmd() const { return mCmdLine; };

	const COperation* Operation() const { return mOperation; }

	void* UserData() const { return mUserData; }


	virtual void Kill();

	virtual void Execute();

	long long Pid() const
	{
		return
#if defined (WIN32) || defined(_WIN32)
			pid()->dwProcessId;
#else
            pid();
#endif
	}

	std::string PidString() const {	return n2s<std::string>( Pid() ); }

	//

	virtual void SetCmd( const std::string& value ) { mCmdLine = value; };



	virtual const std::string& GetName() const { return mName; };
	virtual void SetName( const std::string& value ) { mName = value; };


	int GetExecuteFlags() { return mExecuteFlags; };
	void SetExecuteFlags( int value ) { mExecuteFlags = value; };

	// instead of overriding this virtual function we might as well process the
	// event from it in the frame class - this might be more convenient in some
	// cases
	//virtual void OnTerminate( int pid, int status );

	//virtual void OnTerminate( int status );

	virtual bool HasInput() { return false; };

	bool IsRunning() { return m_running > 0; };
	bool IsEnded() { return m_running < 0; };
	bool IsReady() { return m_running == 0; };
	bool IsKilled() { return mKilled; };

	int AskForPid();

	//static void EvtProcessTermCommand( wxEvtHandler& evtHandler, const CProcessTermEventFunction& method,
	//	wxObject* userData = nullptr, wxEvtHandler* eventSink = nullptr );

	//static void DisconnectEvtProcessTermCommand( wxEvtHandler& evtHandler );

protected:
	//virtual int ExecuteSync();
	//virtual int ExecuteAsync();
};





/////////////////////////////////////////////////////////////////////////////////
//						Processes Table Widget
/////////////////////////////////////////////////////////////////////////////////


class CProcessesTable : public CControlPanel
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

	Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	// types & friends

	using base_t = CControlPanel;

	enum EProcessColumn
	{
		eProcessName,
		eProcessCmdLine,

		EProcessColumn_size
	};


	// static data

	static const QString smSeparator;

public:

	static const QString& ProcessErrorMessage( QProcess::ProcessError error );


protected:

	// instance data

	QTableWidget *mProcessesList = nullptr;
	QPushButton *mKillButton = nullptr;

	std::vector< COsProcess* > mProcesses;


	// construction / destruction

	void CreateWidgets();
	void Wire();

	void KillAll();
public:
	CProcessesTable( QWidget *parent = nullptr );

	virtual ~CProcessesTable();


	// Add is called by the client...

	bool Add( void *user_data, bool sync, bool allow_multiple, const std::string &original_name, const std::string &cmd, const COperation *operation = nullptr );
	bool Add( void *user_data, bool sync, bool allow_multiple, const std::string &original_name, const std::string &executable, const std::vector< std::string > &args )
	{
		std::string cmd = executable + " ";
		cmd += Vector2String( args, " " );
		return Add( user_data, sync, allow_multiple, original_name, cmd );
	}


	bool Empty() const { return mProcessesList->rowCount() == 0; }


protected:

	// ...Remove is called by the slots processing COsProcess signals

	void Remove( COsProcess *process );


	// List manipulation helpers

	void FillList();

	void SetItemProcessData( int index, COsProcess *process );

	COsProcess* GetItemProcessData( int index ) const;

	int FindProcess( const std::string &name ) const;

	std::string MakeProcessNewName( const std::string &name ) const;



	// Process I/O helpers

	QString ReadStdError( COsProcess *process );
	QString ReadStdOut( COsProcess *process );

	QString FormatOutputMessage( const QString &name, const QByteArray &b ) const
	{
		return b.isEmpty() ? QString() : ( name + smSeparator + QString::fromLocal8Bit( b ) );
	}
	QString FormatErrorMessage( COsProcess *process, const QString &msg ) const
	{
		return msg.isEmpty() ? QString() : ( process->GetName().c_str() + smSeparator + "Process error occurred: " + msg );
	}
	QString FormatFinishedMessage( COsProcess *process, const QString &msg ) const
	{
		return msg.isEmpty() ? QString() : ( process->GetName().c_str() + smSeparator + "Process execution finished reporting " + msg );
	}


	COsProcess* ProcessFinished( int exit_code, QProcess::ExitStatus exitStatus );

signals:
	void ProcessFinished( int exit_code, QProcess::ExitStatus exitStatus, const COperation *operation, bool sync, void *user_data );


private slots :

	// Kill

	void HandleSelectedProcessChanged();
	void HandleKill();

	// QProcess report handling

	void UpdateOutput();
	void AsyncProcessFinished( int exit_code, QProcess::ExitStatus exitStatus );
	void SyncProcessFinished( int exit_code, QProcess::ExitStatus exitStatus );
	void ProcessError( QProcess::ProcessError error );
};




#endif	//GUI_CONTROL_PANELS_PROCESSES_TABLE_H
