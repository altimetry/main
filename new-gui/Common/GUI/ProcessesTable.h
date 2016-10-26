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
#if !defined COMMON_GUI_PROCESSES_TABLE_H
#define COMMON_GUI_PROCESSES_TABLE_H


#include "new-gui/Common/System/OsProcess.h"

#include "ControlPanel.h"




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

	friend class CSchedulerMainWindow;


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
	static const QString smTitleBegin;
	static const QString smTitleEnd;


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

	bool Add( COsProcess *process, bool with_gui, bool sync, bool allow_multiple );

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
	QString FormatErrorMessage( const QString &msg ) const
	{
		return msg.isEmpty() ? QString() : ( "Process error occurred: " + msg );
	}
	QString FormatFinishedMessage( const QString &msg ) const
	{
		return msg.isEmpty() ? QString() : ( "\n" + smTitleBegin + "Process execution finished reporting " + msg + smTitleEnd + "\n" );
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

	void HandleUpdateOutput();
	void HandleAsyncProcessFinished( int exit_code, QProcess::ExitStatus exitStatus );
	void HandleSyncProcessFinished( int exit_code, QProcess::ExitStatus exitStatus );
	void HandleProcessError( QProcess::ProcessError error );
};




#endif	//COMMON_GUI_PROCESSES_TABLE_H
