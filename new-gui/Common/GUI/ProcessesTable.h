#if !defined GUI_CONTROL_PANELS_PROCESSES_TABLE_H
#define GUI_CONTROL_PANELS_PROCESSES_TABLE_H


#include "ControlPanel.h"



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

	typedef long long uid_t;

	using log_file_t = void (COsProcess::*)( const std::string &msg );


	// instance data

protected:
	bool mSync = false;
	std::string mId;		//tasks
	std::string mAt;		//tasks
	std::string mName;		//tasks & operations
	std::string mStatus;	//tasks
	std::string mCmdLine;	//tasks & operations
	std::string mLogPath;	//tasks

	void *mUserData = nullptr;

	CLogFile mLog;
	log_file_t mLogPtr = nullptr;

public:
	COsProcess( bool sync, const std::string &name, QWidget *parent, const std::string &cmd, void *user_data = nullptr,
		const std::string &id = "",
		const std::string &at = "",
		const std::string &status = "",
		const std::string &log_file = ""
		);

	virtual ~COsProcess();


	// access

	void* UserData() const { return mUserData; }

	const std::string& CmdLine() const { return mCmdLine; };
	virtual void SetCmd( const std::string& value ) { mCmdLine = value; };

	virtual const std::string& Name() const { return mName; };
	virtual void SetName( const std::string& value ) { mName = value; };

	const std::string& Id() const { return mId; }
	const std::string& At() const { return mAt; }
	const std::string& Status() const { return mStatus; }
	const std::string& LogPath() const { return mLogPath; }

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


	// operate

	virtual void Kill();

	virtual void Execute( bool detached = false );


	void Log( const std::string &msg )
	{
		assert__( mLogPtr );

		(this->*mLogPtr)( msg );
	}

protected:
	void RealLog( const std::string &msg )
	{
		mLog.Log( msg );
	}

	void PseudoLog( const std::string & )
	{}
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

	friend class CSchedulerDialog;


public:
	enum EProcessColumn
	{
		eTaskUid,
		eProcessName,		eTaskName = eProcessName,
		eTaskStart,
		eTaskStatus,
		eProcessCmdLine,	eTaskCMD = eProcessCmdLine,
		eTaskLogFile,

		EProcessColumn_size
	};


protected:

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

	bool Add( bool with_gui, void *user_data, bool sync, bool allow_multiple, const std::string &original_name, const std::string &cmd,
		const std::string &id = "",
		const std::string &at = "",
		const std::string &status = "",
		const std::string &log_file = ""
		);
	bool Add( void *user_data, bool sync, bool allow_multiple, const std::string &original_name, const std::string &executable, const std::vector< std::string > &args )
	{
		std::string cmd = executable + " ";
		cmd += Vector2String( args, " " );
		return Add( true, user_data, sync, allow_multiple, original_name, cmd );
	}
	bool SilentAdd( void *user_data, bool sync, bool allow_multiple, const std::string &original_name, const std::string &cmd,
		const std::string &id = "",
		const std::string &at = "",
		const std::string &status = "",
		const std::string &log_file = ""
		)
	{
		return Add( false, user_data, sync, allow_multiple, original_name, cmd, id, at, status, log_file );	
	}


	void HideColumns( std::initializer_list< EProcessColumn > columns, bool hide = true );

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

	std::string FormatOutputMessage( const std::string &name, const std::string &b ) const
	{
		return b.empty() ? std::string() : ( name.empty() ? b : name + q2a( smSeparator ) + b );
	}
	QString FormatOutputMessage( const QString &name, const QByteArray &b ) const
	{
		return b.isEmpty() ? QString() : ( name.isEmpty() ? QString::fromLocal8Bit( b ) : name + smSeparator + QString::fromLocal8Bit( b ) );
	}
	QString FormatErrorMessage( COsProcess *process, const QString &msg ) const
	{
		return msg.isEmpty() ? QString() : ( process->Name().c_str() + smSeparator + "Process error occurred: " + msg );
	}
	QString FormatFinishedMessage( COsProcess *process, const QString &msg ) const
	{
		return msg.isEmpty() ? QString() : ( process->Name().c_str() + smSeparator + "Process execution finished reporting " + msg );
	}

	//... log helpers

	void LogInfo( COsProcess *process, const std::string &text );

	void LogWarn( COsProcess *process, const std::string &text );

	
	// Finish Handlers

	COsProcess* ProcessFinished( int exit_code, QProcess::ExitStatus exitStatus );

signals:
	void ProcessFinished( int exit_code, QProcess::ExitStatus exitStatus, bool sync, void *user_data );


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
