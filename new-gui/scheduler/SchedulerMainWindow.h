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
#ifndef SCHEDULER_DLG_H
#define SCHEDULER_DLG_H

#if defined(QT_VERSION) && (QT_VERSION < 0x050000)
#include <QtGui/QToolBar>
#else
#include <QtWidgets/QToolBar>
#endif

#include "ui_SchedulerMainWindow.h"
#include "new-gui/Common/DataModels/TaskProcessor.h"
#include "new-gui/Common/GUI/ProcessesTable.h"


class CApplicationUserPaths;
class CSchedulerApplication;
class CBasicLogShell;
class CProcessesTable;



class CBratTaskFunctionProcess : public COsProcess
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

	using base_t = COsProcess;


	// instance data

	CBratTask *mTask = nullptr;


	// construction / destruction 

	std::string FormatPseudoCmd( const CBratTask *task ) const;
public:
	CBratTaskFunctionProcess( CBratTask *task, bool sync, QWidget *parent, void *user_data );

	virtual ~CBratTaskFunctionProcess()
	{}


	// operations override

	virtual void Kill() override;

	virtual void Execute( bool detached = false ) override;

signals:

#if QT_VERSION >= 0x050000
	void started();				//has 1 parameter in qt5
#endif
};








class CSchedulerMainWindow : public QMainWindow, private Ui::CSchedulerMainWindow
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	// types

	using base_t = QMainWindow;

	using uid_t = CBratTask::uid_t;

	struct post_execution_handler_wrapper_t;

	using post_execution_handler_t = void(CSchedulerMainWindow::*)( int exit_code, QProcess::ExitStatus exitStatus, post_execution_handler_wrapper_t *phandler );

	struct post_execution_handler_wrapper_t
	{
		CBratTask *task;
		post_execution_handler_t next_post_execution_handler;
		int isubtask;
	};

    // static members

    static const auto smSCHEDULER_TIMER_INTERVAL = 30000;			//respecting v3 period
    static const auto smCHECK_CONFIGFILE_TIMER_INTERVAL = 5000;		//idem

	static QString MakeWindowTitle( const QString &title = QString() );	//returns the application name if title is empty


    // instance data members

	CProcessesTable *mProcessesTable = nullptr;
	int mProcessesTableIndex = -1;
	bool mSyncProcessExecuting = false;

    CBasicLogShell *mLogFrame = nullptr;
	int mLogFrameIndex = -1;
	QHash< int, QColor >	mSeverityToColorTable;
	QHash< int, QString >	mSeverityToPromptTable;

    const CApplicationUserPaths *mAppPaths;
	std::string mSchedulerFilePath;

	QTimer mSchedulerTimer;
	QTimer mCheckConfigFileTimer;
	QDateTime mLastCheck;

    QMutex mTasksMutex;


	// construction / destruction

    void CreateMenuBar();
    void CreateLogTable();
    void CreateProcessesTable();
	
	template< typename... Args > 
	void SetTablesProperties( Args... args );
    
	void UpdateTasksTables();

public:
    CSchedulerMainWindow( CSchedulerApplication &a, QWidget *parent = 0);

    virtual ~CSchedulerMainWindow();


	// operations

protected:
	void StartTimers();
	void StopTimers();
	void ConnectTimers();
	void DisconnectTimers();

	void SetItemProcessData( QTableWidget *table, int index, CBratTask *task );
	CBratTask* GetItemProcessData( QTableWidget *table, int index ) const;

    void UpdateTasksTable( const CMapBratTask &data, QTableWidget *table );

	bool RemoveTaskFromTable( QTableWidget *table, const CBratTask *task );
	bool RemoveTaskFromTableAndMap( QTableWidget *table, CBratTask *task );
	bool RemoveTasksFromTableAndMap( QTableWidget *table );
	void AddTaskToTable( QTableWidget *table, CBratTask *task );

	bool ExecuteAsync( CBratTask *task, int isubtask = - 1 );
	void AsyncTaskFinished( int exit_code, QProcess::ExitStatus exitStatus, post_execution_handler_wrapper_t *phandler );


	// signals / slots

    virtual void closeEvent( QCloseEvent *event ) override;

signals:
	void AsyncProcessExecution( bool executing );
	void SyncProcessExecution( bool executing );

public slots:

    void action_ToggleTimers_slot( bool checked );
    void action_ViewConfig_slot();
    void action_UserManual_slot();
    void action_About_slot();

	void SchedulerTimerTimeout();
	void CheckConfigFileTimerTimeout();

private slots:

	void HandlePeerMessage( const QString& );
	void HandleProcessFinished( int exit_code, QProcess::ExitStatus exitStatus, bool sync, void *user_data );

    void on_mRemovePendTaskButton_clicked();
    void on_mClearPendTaskButton_clicked();
    void on_mRemoveEndedTaskButton_clicked();
    void on_mClearEndedTaskButton_clicked();
    void on_mShowEndedLogButton_clicked();

	void HandleSelectedPendingProcessChanged();
	void HandleSelectedEndedProcessChanged();
};

#endif // SCHEDULER_DLG_H
