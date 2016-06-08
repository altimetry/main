#include "stdafx.h"

// Include private headers

#include <locale>
#include <string>

#include <QMenuBar>

#include "libbrathl/Tools.h"

#include "new-gui/Common/BratVersion.h"
#include "new-gui/Common/QtUtils.h"
#include "new-gui/Common/ConsoleApplicationPaths.h"
#include "new-gui/Common/GUI/BasicLogShell.h"
#include "new-gui/Common/GUI/ProcessesTable.h"

#include "new-gui/Common/DataModels/TaskProcessor.h"

#include "SchedulerApplication.h"
#include "SchedulerLogger.h"
#include "SchedulerDialog.h"
#include "EditTasksFileDialog.h"


// When debugging changes all calls to new to be calls to DEBUG_NEW allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include <libbrathl/Win32MemLeaksAccurate.h>

#ifdef WIN32
const std::string BRATHL_ICON_FILENAME = "BratIcon.ico";
#else
const std::string BRATHL_ICON_FILENAME = "BratIcon.bmp";
#endif

//const std::string BRATGUI_APPNAME = "scheduler";
//const std::string GROUP_COMMON = "Common";
//const std::string ENTRY_USER_MANUAL = "UserManual";
//const std::string ENTRY_USER_MANUAL_VIEWER = "UserManualViewer";

//QString mUserManualViewer;
//std::string m_userManual;



/////////////////////////////////////////////////////////////////////////////////
//							Construction Helpers
/////////////////////////////////////////////////////////////////////////////////


template< typename... Args >
void CSchedulerDialog::SetTablesProperties( Args... args )
{
	// lambdas

    auto resize_columns = []( std::initializer_list< QTableWidget* > tables )
	{
        static const CProcessesTable::EProcessColumn resizable_columns[] = { CProcessesTable::eTaskUid, CProcessesTable::eTaskStart, CProcessesTable::eTaskStatus };

        for ( auto *t : tables )
		{
			for ( auto i : resizable_columns )
				t->resizeColumnToContents( i );
		}
	};


	// function body

	resize_columns( { mTablePendingTask, mProcessesTable->mProcessesList, mTableEndedTask } );

	QTableWidget *tables[] { args... }; 
    for ( QTableWidget *t : tables )
	{
		// Enabling sorting in all tables: pending, processing and end tasks.
		t->setSortingEnabled( true );

		// Changing selection behavior to whole row selection
		t->setSelectionBehavior( QAbstractItemView::SelectRows );

		// Setting alternating row colors
		t->setAlternatingRowColors( true );

        // Disable editable table items
        t->setEditTriggers( QAbstractItemView::NoEditTriggers );

		// Disable grid lines.
		t->setShowGrid( false );

		// select only one item
		t->setSelectionMode( QAbstractItemView::SingleSelection );	

		t->setMinimumWidth( ComputeTableWidth( t ) );
	}
};


//static 
QString CSchedulerDialog::MakeWindowTitle( const QString &title )// = QString() //returns the application name if title is empty
{
	static const QString &app_title = qApp->applicationName() + build_configuration.c_str();

	QString t = app_title;

	if ( !title.isEmpty() )
		t += " - " + title;

	return t + "[*]";
}


void CSchedulerDialog::CreateMenuBar()
{
    // 1. MenuBar
    QMenuBar *menuBar = new QMenuBar(this);
    menuBar->setObjectName(QString::fromUtf8("menuBar"));
    //menuBar->setGeometry(QRect(0, 0, 957, 21));

    // 1.1. Menu File
    QMenu *menu_File = new QMenu(tr("&File"), menuBar);
    menu_File->setObjectName(QString::fromUtf8("menu_File"));
    menuBar->addAction(menu_File->menuAction());

    // 1.1.1. Action Exit
    QAction *action_Exit = new QAction(tr("&Exit"), this);
    action_Exit->setShortcut(QKeySequence::Quit);
    action_Exit->setStatusTip(tr("Quit BRAT Scheduler"));
    QObject::connect(action_Exit, SIGNAL(triggered()), this, SLOT(close()));
    menu_File->addAction(action_Exit);


    // 1.2. Menu Tools
    QMenu *menu_Tools = new QMenu(tr("&Tools"), menuBar);
    menu_Tools->setObjectName(QString::fromUtf8("menu_Tools"));
    menuBar->addAction(menu_Tools->menuAction());

    // 1.2.1. Action ViewConfig
    QAction *action_ViewConfig = new QAction(tr("&View Configuration..."), this);
    action_ViewConfig->setStatusTip(tr("Open task configuration file"));
    QObject::connect(action_ViewConfig, SIGNAL(triggered()), this, SLOT(action_ViewConfig_slot()));
    menu_Tools->addAction(action_ViewConfig);

#if defined (DEBUG) || defined(_DEBUG)
	QAction *action_Timers = new QAction(tr("Timers..."), this);
	action_Timers->setCheckable( true );
	action_Timers->setChecked( true );
    action_Timers->setStatusTip(tr("Start/Stop Timers"));
    QObject::connect( action_Timers, SIGNAL(toggled(bool)), this, SLOT(action_ToggleTimers_slot( bool )));
    menu_Tools->addAction(action_Timers);
#endif
	



    // 1.3. Menu Help
    QMenu *menu_Help = new QMenu(tr("&Help"), menuBar);
    menu_Help->setObjectName(QString::fromUtf8("menu_Help"));
    menuBar->addAction(menu_Help->menuAction());

    // 1.3.1. Action UserManual
    QAction *action_UserManual = new QAction(tr("&User's manual"), this);
    action_UserManual->setShortcut(QKeySequence::HelpContents);
    action_UserManual->setStatusTip(tr("Open user's manual"));
    QObject::connect(action_UserManual, SIGNAL(triggered()), this, SLOT(action_UserManual_slot()));
    menu_Help->addAction(action_UserManual);
    menu_Help->addSeparator();

    // 1.3.2. Action Help
    QAction *action_About = new QAction(tr("&About"), this);
    action_About->setShortcut(QKeySequence::WhatsThis);
    QObject::connect(action_About, SIGNAL(triggered()), this, SLOT(action_About_slot()));
    menu_Help->addAction(action_About);

    layout()->setMenuBar(menuBar);
}


void CSchedulerDialog::CreateLogTable()
{
	static std::string names[] =
	{
		"INFO",
		"WARN",
		"FATAL"
	};
    static const DEFINE_ARRAY_SIZE( names ); Q_UNUSED(names_size);

	mSeverityToColorTable.reserve( 3 );

	mSeverityToColorTable.insert( QtDebugMsg,		QColor( 0, 255, 0, 127 ) );
	mSeverityToColorTable.insert( QtWarningMsg,		QColor( 255, 255, 0, 127 ) );
	mSeverityToColorTable.insert( QtCriticalMsg,	QColor( 255, 0, 0, 127 ) );
		
	mSeverityToPromptTable.reserve( 3 );

	mSeverityToPromptTable.insert( QtDebugMsg,		"[INFO] " );
	mSeverityToPromptTable.insert( QtWarningMsg,	"[WARN] " );
	mSeverityToPromptTable.insert( QtCriticalMsg,	"[FATAL] ");

	mLogFrame = new CBasicLogShell( CSchedulerLogger::Instance(), std::vector< std::string >( &names[0], &names[names_size] ), 
		mSeverityToColorTable, mSeverityToPromptTable, this );
	mLogFrame->setObjectName( QString::fromUtf8( "mLogShell" ) );
	mLogFrameIndex = mTabWidget->addTab( mLogFrame, "Log" );

	connect( 
		&CSchedulerLogger::Instance(), SIGNAL( QtLogMessage( int, QString  ) ),
		mLogFrame, SLOT( QtNotifySlot( int, QString ) ), Qt::QueuedConnection );

	mLogFrame->Deactivate( false );

	LOG_INFO( "Logging initialized in " + qApp->applicationName().toStdString() );
}


void CSchedulerDialog::CreateProcessesTable()
{

	mProcessesTable = new CProcessesTable( this );
	mProcessesTable->setObjectName( QString::fromUtf8( "mProcessesTable" ) );	//mTabWidget->removeTab( 1 );
	mProcessesTableIndex = mTabWidget->insertTab( 1, mProcessesTable, "Processing Tasks" );

	connect( mProcessesTable, SIGNAL( ProcessFinished( int, QProcess::ExitStatus, bool, void* ) ),
		this, SLOT( HandleProcessFinished( int, QProcess::ExitStatus, bool, void* ) ) );
}


void CSchedulerDialog::UpdateTasksTables()
{
	UpdateTasksTable( *CTasksProcessor::GetInstance()->GetMapPendingBratTask(), mTablePendingTask );
	//UpdateTasksTable(*CTasksProcessor::GetInstance()->GetMapProcessingBratTask(), tableProcTask);
	UpdateTasksTable( *CTasksProcessor::GetInstance()->GetMapEndedBratTask(), mTableEndedTask );
}



/////////////////////////////////////////////////////////////////////////////////
//							Constructor
/////////////////////////////////////////////////////////////////////////////////


CSchedulerDialog::CSchedulerDialog( CSchedulerApplication &a, QWidget *parent )
    : base_t( parent )
    , mAppPaths( a.ApplicationPaths() )
	, mTasksMutex( QMutex::Recursive )
{
    assert__( mAppPaths );

	a.setActivationWindow( this );
	connect( &a, SIGNAL( messageReceived( const QString& ) ), this, SLOT( HandlePeerMessage( const QString& ) ) );

	setupUi( this );

    setAttribute( Qt::WA_QuitOnClose, true );
    setWindowIcon( QIcon("://images/BratIcon.png") );
	setWindowTitle( MakeWindowTitle() );
	CreateMenuBar();
	SetMaximizableDialog( this );

	CreateLogTable();
	CreateProcessesTable();

    if ( !CTasksProcessor::GetInstance() )
        CTasksProcessor::CreateInstance( mAppPaths->mExecutableDir );
	mSchedulerFilePath = CTasksProcessor::GetInstance()->SchedulerFilePath();

    // v3 note: After loading, tasks whose status is 'in progress' are considered as 'pending'
    // They have to be re-executed.
    CVectorBratTask v( false );
    CTasksProcessor::GetInstance()->ChangeProcessingToPending( v );

	UpdateTasksTables();

	SetTablesProperties( mTablePendingTask, mProcessesTable->mProcessesList, mTableEndedTask );

	mTabWidget->setCurrentIndex( 0 );

	// Wire

	connect( mTablePendingTask, SIGNAL( itemSelectionChanged() ), this, SLOT( HandleSelectedPendingProcessChanged() ) );
	connect( mTableEndedTask, SIGNAL( itemSelectionChanged() ), this, SLOT( HandleSelectedEndedProcessChanged() ) );

	// start carousel

	connect( &mSchedulerTimer, SIGNAL( timeout() ), this, SLOT( SchedulerTimerTimeout() ) );
	connect( &mCheckConfigFileTimer, SIGNAL( timeout() ), this, SLOT( CheckConfigFileTimerTimeout() ) );

	StartTimers();
}


//virtual
CSchedulerDialog::~CSchedulerDialog()
{
}


void CSchedulerDialog::HandlePeerMessage( const QString &msg )
{
	LOG_INFO( msg );
}


void CSchedulerDialog::StartTimers()
{
	mSchedulerTimer.start( smSCHEDULER_TIMER_INTERVAL );
	mCheckConfigFileTimer.start( smCHECK_CONFIGFILE_TIMER_INTERVAL );
}


void CSchedulerDialog::StopTimers()
{
	mSchedulerTimer.stop();
	mCheckConfigFileTimer.stop();
}


void CSchedulerDialog::HandleProcessFinished( int exit_code, QProcess::ExitStatus exitStatus, bool sync, void *user_data )
{
	if ( sync )
		mSyncProcessExecuting = false;

	post_execution_handler_wrapper_t *handler = reinterpret_cast<post_execution_handler_wrapper_t*>( user_data );
	if ( handler && handler->next_post_execution_handler )
		( this->*(handler->next_post_execution_handler) )( exit_code, exitStatus, handler );

	if ( sync )
		emit SyncProcessExecution( false );

	delete handler;
}


/////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////

bool CSchedulerDialog::ExecuteAsync( CBratTask *task, int isubtask )	//, int isubtask = - 1
{
    CBratTask *child_task = isubtask < 0 ? task : task->GetSubordinateTasks()->at( isubtask );
    post_execution_handler_wrapper_t *handler = new post_execution_handler_wrapper_t
    {
        task, &CSchedulerDialog::AsyncTaskFinished, isubtask
    };

	if ( task != child_task )
	{
		child_task->SetStatus( CBratTask::eBRAT_STATUS_PROCESSING );
		child_task->SetLogFile( task->GetLogFile() );					//use parent log file
	}

    const bool sync = false;
    bool result = mProcessesTable->SilentAdd( handler, sync, false, child_task->GetName(), child_task->GetCmd(),
        child_task->GetUidAsString(),
        child_task->GetAtAsString(),
        child_task->GetStatusAsString(),
        child_task->GetLogFile() );

    mSyncProcessExecuting = result && sync;
    if ( !mSyncProcessExecuting )
        emit sync ? SyncProcessExecution( false ) : AsyncProcessExecution( false );
    else
    {
        if ( sync )
        {
            while ( mSyncProcessExecuting )
            {
                QBratThread::sleep( 1 );
                qApp->processEvents();
            }
        }
    }

    return result;
}
void CSchedulerDialog::AsyncTaskFinished( int exit_code, QProcess::ExitStatus exitStatus, post_execution_handler_wrapper_t *phandler )
{
	QMutexLocker locker( &mTasksMutex );

	const bool success =
		exit_code == 0 &&
		exitStatus == QProcess::NormalExit;

	CBratTask *task = phandler->task;
	CBratTask::EStatus status = success ? CBratTask::eBRAT_STATUS_ENDED : CBratTask::eBRAT_STATUS_WARNING;

    if ( success && task->HasSubordinateTasks() )
    {
        const size_t size = task->GetSubordinateTasks()->size();
        int isubtask = phandler->isubtask + 1;
        if ( isubtask < size )
        {
            ExecuteAsync( task, isubtask );
            return;
        }
    }


	try
	{
		mCheckConfigFileTimer.stop();

		mProcessesTable->setEnabled( false );
		mTab_EndedTasks->setEnabled( false );

		//LoadSchedulerTaskConfig(true);
		CTasksProcessor::GetInstance()->ReloadOnlyNew();

		CTasksProcessor::GetInstance()->ChangeTaskStatus( task->GetUid(), status );

		//bool saved = 
		CTasksProcessor::GetInstance()->SaveAllTasks();		//TODO error handling

		//    ResetConfigFileChange();

		InsertTaskInTable( mTableEndedTask, task );
		//
		//  }
		//  catch(CException& e)
		//  {
		//    UnLockConfigFile();
		//    bOk = false;
		//    wxLogError("%s",e.what());
		//
		//  }
		//
		mProcessesTable->setEnabled( true );
		mTab_EndedTasks->setEnabled( true );

		mCheckConfigFileTimer.start( smCHECK_CONFIGFILE_TIMER_INTERVAL );

		LOG_INFO( "Asynchronous process for task '" + task->GetName() + "' finished execution." );
	}
	catch ( const CException &e )
	{
		LOG_WARN( e.Message() );
	}
}


//void CProcessingPanel::OnBratTaskProcess( CBratTaskProcessEvent& event )
//{
//	CBratTask* task = CSchedulerTaskConfig::GetInstance()->FindTaskFromMap( event.m_uid );
//
//	if ( task == nullptr )
//	{
//		wxString msg = wxString::Format( "Unable to process task: task uid '%s' is not found.",
//			task->GetUidValueAsString().c_str() );
//		//wxMessageBox(msg,
//		//            "Error",
//		//            wxOK | wxICON_ERROR);
//
//		wxLogError( "%s", msg.c_str() );
//
//	}
//
//	task->SetStatus( CBratTask::BRAT_STATUS_PROCESSING );
//
//	CBratTaskProcess* process = new CBratTaskProcess( task, this );
//
//	this->AddProcess( process );
//}
//void CSchedulerDialog::AsyncSubtaskFinished( int exit_code, QProcess::ExitStatus exitStatus, post_execution_handler_wrapper_t *phandler )
void CSchedulerDialog::AsyncSubtaskFinished( int , QProcess::ExitStatus , post_execution_handler_wrapper_t * )
{

}

void CSchedulerDialog::SchedulerTimerTimeout()
{
	QMutexLocker locker( &mTasksMutex );

	try
	{
		std::vector<CTasksProcessor::uid_t> tasks2process;
		CTasksProcessor::GetInstance()->GetMapPendingBratTaskToProcess( &tasks2process );

		for ( auto ii = tasks2process.begin(); ii != tasks2process.end(); ii++ )
		{
			CBratTask *task = CTasksProcessor::GetInstance()->FindTaskFromMap( *ii );		assert__( task->GetUid() == *ii );

			//CBratSchedulerApp::OnBratTaskProcess

			mCheckConfigFileTimer.stop();

			mTab_PendingTasks->setEnabled( false );
			mProcessesTable->setEnabled( false );
			//tab_EndedTasks;

			if ( CTasksProcessor::GetInstance()->ReloadOnlyNew() && CTasksProcessor::GetInstance()->HasMapNewBratTask() )		//LoadSchedulerTaskConfig(true);
			{
				//call to CPendingPanel::OnCheckFileChange(CCheckFileChangeEvent& event)
				UpdateTasksTable( *CTasksProcessor::GetInstance()->GetMapPendingBratTask(), mTablePendingTask );
			}

			CTasksProcessor::GetInstance()->ChangeTaskStatus( task->GetUid(), CBratTask::eBRAT_STATUS_PROCESSING );
			RemoveTaskFromTable( mTablePendingTask, task );
			HandleSelectedPendingProcessChanged();

			//bool saved = 
			CTasksProcessor::GetInstance()->SaveAllTasks();		//TODO error handling

			//ResetConfigFileChange();

			//call CProcessingPanel::OnBratTaskProcess(CBratTaskProcessEvent& event)
			if ( task == nullptr )
			{
				std::string msg = "Unable to process task: task uid '" + task->GetUidAsString() + "' is not found.";
				LOG_WARN( msg );
			}
            else

			////	CBratTaskProcess* process = new CBratTaskProcess( task, this );
			////	this->AddProcess( process );
			//post_execution_handler_wrapper_t *handler = new post_execution_handler_wrapper_t{ task, &CSchedulerDialog::AsyncTaskFinished, -1 };
			//const bool sync = false;
			//bool result = mProcessesTable->SilentAdd( handler, sync, false, task->GetName(), task->GetCmd(),
			//	task->GetUidAsString(),
			//	task->GetAtAsString(),
			//	task->GetStatusAsString(),
			//	task->GetLogFile() );

			//mSyncProcessExecuting = result && sync;
			//if ( !mSyncProcessExecuting )
			//	emit sync ? SyncProcessExecution( false ) : AsyncProcessExecution( false );
			//else
			//{
			//	if ( sync )
			//	{
			//		while ( mSyncProcessExecuting )
			//		{
			//			QBratThread::sleep( 1 );
			//			qApp->processEvents();
			//		}
			//	}
			//}

			if ( !ExecuteAsync( task ) )
			{
				CTasksProcessor::GetInstance()->ChangeTaskStatus( task->GetUid(), CBratTask::eBRAT_STATUS_PENDING );
				InsertTaskInTable( mTablePendingTask, task );
				LOG_WARN( "Task " + task->GetName() + " changed to pending." );
			}

			mTab_PendingTasks->setEnabled( true );
			mProcessesTable->setEnabled( true );
			mCheckConfigFileTimer.start( smCHECK_CONFIGFILE_TIMER_INTERVAL );
		}
	}
	catch ( const CException &e )
	{
		LOG_WARN( e.Message() );
	}
}
void CSchedulerDialog::CheckConfigFileTimerTimeout()
{
	QMutexLocker locker( &mTasksMutex );

	try
	{
		QDateTime most_recent = QFileInfo( mSchedulerFilePath.c_str() ).lastModified();
		if ( most_recent > mLastCheck )									  //CBratSchedulerApp::OnCheckFileChange
		{
			mCheckConfigFileTimer.stop();
			mLastCheck = most_recent;

			if ( CTasksProcessor::GetInstance()->ReloadOnlyNew() )
				UpdateTasksTable( *CTasksProcessor::GetInstance()->GetMapPendingBratTask(), mTablePendingTask );

			mCheckConfigFileTimer.start( smCHECK_CONFIGFILE_TIMER_INTERVAL );
		}
	}
	catch ( const CException &e )
	{
		LOG_WARN( e.Message() );
	}
}


//virtual
void CSchedulerDialog::closeEvent( QCloseEvent *event )
{
	QTimer::singleShot( 0, qApp, SLOT( quit() ) );

    base_t::closeEvent( event );
}


/////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////


Q_DECLARE_METATYPE( CBratTask* )

void CSchedulerDialog::SetItemProcessData( QTableWidget *table, int index, CBratTask *task )
{
	QTableWidgetItem *widgetItem = table->item( index, CProcessesTable::eProcessCmdLine ); 
	widgetItem->setData( Qt::UserRole, QVariant::fromValue( task ) );
}

CBratTask* CSchedulerDialog::GetItemProcessData( QTableWidget *table, int index ) const
{
	QTableWidgetItem *widgetItem = table->item( index, CProcessesTable::eProcessCmdLine );
	return widgetItem->data( Qt::UserRole ).value< CBratTask* >();
}


bool CSchedulerDialog::RemoveTaskFromTable( QTableWidget *table, const CBratTask *task )
{
	const int size = table->rowCount();
	for ( int index = 0; index < size; ++index )
	{
		CBratTask *item_task = GetItemProcessData( table, index );		assert__( item_task );
		if ( item_task == task )
		{
			table->removeRow( index );
			return true;
		}
	}
	return false;
}


void CSchedulerDialog::InsertTaskInTable( QTableWidget *table, CBratTask *task )
{
	int index = table->rowCount();
	table->setRowCount( index + 1 );

	table->setItem( index, CProcessesTable::eTaskUid, new QTableWidgetItem( task->GetUidAsString().c_str() ) );
	table->setItem( index, CProcessesTable::eTaskName, new QTableWidgetItem( task->GetName().c_str() ) );
	table->setItem( index, CProcessesTable::eTaskStart, new QTableWidgetItem( task->GetAtAsString().c_str() ) );
	table->setItem( index, CProcessesTable::eTaskStatus, new QTableWidgetItem( task->GetStatusAsString().c_str() ) );
	table->setItem( index, CProcessesTable::eTaskCMD, new QTableWidgetItem( task->GetCmd().c_str() ) );
	table->setItem( index, CProcessesTable::eTaskLogFile, new QTableWidgetItem( task->GetLogFile().c_str() ) );

	SetItemProcessData( table, index, task );
}


void CSchedulerDialog::UpdateTasksTable( const CMapBratTask &data, QTableWidget *table )
{
	table->setRowCount( 0 );
	for ( CMapBratTask::const_iterator it = data.begin(); it != data.end(); it++ )
	{
		CBratTask *task = it->second;		assert__( task != nullptr );
		InsertTaskInTable( table, task );
	}

	HandleSelectedPendingProcessChanged();
	HandleSelectedEndedProcessChanged();

	LOG_INFO( "Tasks loaded." );
}


void CSchedulerDialog::HandleSelectedPendingProcessChanged()
{
	mRemovePendTaskButton->setEnabled( mTablePendingTask->currentRow() >= 0 );
	mClearPendTaskButton->setEnabled( mTablePendingTask->rowCount() > 0 );
}
void CSchedulerDialog::HandleSelectedEndedProcessChanged()
{
	mShowEndedLogButton->setEnabled( mTableEndedTask->currentRow() >= 0 );
	mRemoveEndedTaskButton->setEnabled( mTableEndedTask->currentRow() >= 0 );
	mClearEndedTaskButton->setEnabled( mTableEndedTask->rowCount() > 0 );
}


////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

bool CSchedulerDialog::RemoveTasksFromTableAndMap( QTableWidget *table )
{
	QMutexLocker locker( &mTasksMutex );

	try
	{
		const int size = table->rowCount();
		for ( int index = 0; index < size; ++index )
		{
			CBratTask *task = GetItemProcessData( table, index );		assert__( task );
			CTasksProcessor::GetInstance()->RemoveTaskFromMap( task );
		}
		table->setRowCount( 0 );
		return CTasksProcessor::GetInstance()->SaveAllTasks();
	}
	catch ( const CException &e )
	{
		SimpleErrorBox( e.Message() );
	}
	return false;
}


bool CSchedulerDialog::RemoveTaskFromTableAndMap( QTableWidget *table, CBratTask *task )
{
	QMutexLocker locker( &mTasksMutex );

	try
	{
		RemoveTaskFromTable( table, task );
		CTasksProcessor::GetInstance()->RemoveTaskFromMap( task );
		return CTasksProcessor::GetInstance()->SaveAllTasks();
	}
	catch ( const CException &e )
	{
		SimpleErrorBox( e.Message() );
	}
	return false;
}


bool AskConfirmation()
{
	return SimpleQuestion( "Are you sure you want to remove the task(s)?" );
}


void CSchedulerDialog::on_mRemovePendTaskButton_clicked()
{
	int index = mTablePendingTask->currentRow();
	if ( index < 0 || !AskConfirmation() )
		return;

	CBratTask *task = GetItemProcessData( mTablePendingTask, index );	assert__( task );
	RemoveTaskFromTableAndMap( mTablePendingTask, task );
	HandleSelectedPendingProcessChanged();			
}

void CSchedulerDialog::on_mClearPendTaskButton_clicked()
{
	if ( !AskConfirmation() )
		return;

	RemoveTasksFromTableAndMap( mTablePendingTask );
	HandleSelectedPendingProcessChanged();			
}

void CSchedulerDialog::on_mRemoveEndedTaskButton_clicked()
{
	int index = mTableEndedTask->currentRow();
	if ( index < 0 || !AskConfirmation() )
		return;

	CBratTask *task = GetItemProcessData( mTableEndedTask, index );		assert__( task );
	RemoveTaskFromTableAndMap( mTableEndedTask, task );
	HandleSelectedEndedProcessChanged();			
}

void CSchedulerDialog::on_mClearEndedTaskButton_clicked()
{
	if ( !AskConfirmation() )
		return;

	RemoveTasksFromTableAndMap( mTableEndedTask );
	HandleSelectedEndedProcessChanged();			
}


////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////


// Ended Tasks Tab
//
void CSchedulerDialog::on_mShowEndedLogButton_clicked()
{
	int index = mTableEndedTask->currentRow();
	if ( index < 0 )
		return;

	CBratTask *task = GetItemProcessData( mTableEndedTask, index );		assert__( task );
	auto const &path = task->GetLogFile();
	if ( !IsFile( path ) )
	{
		SimpleErrorBox( "Log file for task " + task->GetName() + " not found:\n" + path );
		return;
	}

    CEditTasksFileDialog dlg( task->GetName(), path, this );
    dlg.exec();
}


void CSchedulerDialog::action_ViewConfig_slot()
{
    CEditTasksFileDialog dlg( "", CTasksProcessor::GetInstance()->SchedulerFilePath(), this );
    dlg.exec();
}


void CSchedulerDialog::action_UserManual_slot()
{
	if ( !QDesktopServices::openUrl( QUrl( mAppPaths->mUserManualPath.c_str() ) ) )
		SimpleErrorBox( "Could not open " + mAppPaths->mUserManualPath );
}


void CSchedulerDialog::action_About_slot()
{
	QMessageBox::about(this, tr("About Scheduler"), QString( "Welcome to BRAT Scheduler " ) + BRAT_VERSION + " - " + PROCESSOR_ARCH + "\n(C)opyright CNES/ESA" );
}


void CSchedulerDialog::action_ToggleTimers_slot( bool checked )
{
	checked ? StartTimers() : StopTimers();
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_SchedulerDialog.cpp"
