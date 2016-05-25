#include "stdafx.h"

// Include private headers

#include <locale>
#include <string>

#include <QMenuBar>

#include "libbrathl/Tools.h"

#include "new-gui/Common/QtUtils.h"
#include "new-gui/Common/ConsoleApplicationPaths.h"

#include "new-gui/Common/DataModels/TaskProcessor.h"

#include "SchedulerDialog.h"

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
void CSchedulerDialog::SetupTables( Args... args )
{
	// lambdas

    auto resize_columns = []( std::initializer_list< QTableWidget* > tables )
	{
        static const ETaskListCol resizable_columns[] = { eTaskUid, eTaskStart, eTaskStatus };

        for ( auto *t : tables )
		{
			for ( auto i : resizable_columns )
				t->resizeColumnToContents( i );
		}
	};


	// function body

	resize_columns( { tablePendingTask, tableProcTask, tableEndTask } );

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
        t->setEditTriggers(QAbstractItemView::NoEditTriggers);

		// Disable grid lines.
		t->setShowGrid( false );

		t->setMinimumWidth( ComputeTableWidth( t ) );
	}
};


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

    // 1.2. Menu View
    QMenu *menu_Tools = new QMenu(tr("&Tools"), menuBar);
    menu_Tools->setObjectName(QString::fromUtf8("menu_Tools"));
    menuBar->addAction(menu_Tools->menuAction());

    // 1.2.1. Action ViewConfig
    QAction *action_ViewConfig = new QAction(tr("&View Configuration..."), this);
    action_ViewConfig->setStatusTip(tr("Open task configuration file"));
    QObject::connect(action_ViewConfig, SIGNAL(triggered()), this, SLOT(action_ViewConfig_slot()));
    menu_Tools->addAction(action_ViewConfig);

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


void CSchedulerDialog::LoadTasks()
{
	LoadTasks( *CTasksProcessor::GetInstance()->GetMapPendingBratTask(), tablePendingTask );
	//loadTasks(*CTasksProcessor::GetInstance()->GetMapProcessingBratTask(), tableProcTask);
	LoadTasks( *CTasksProcessor::GetInstance()->GetMapEndedBratTask(), tableEndTask );

	ChangeProcessingToPending();
}



/////////////////////////////////////////////////////////////////////////////////
//							Constructor
/////////////////////////////////////////////////////////////////////////////////


CSchedulerDialog::CSchedulerDialog( const CConsoleApplicationPaths *paths, QWidget *parent )
    : base_t( parent )
	, mIsDialog( parent ? true : false )
    , mAppPaths( paths )
{
    assert__( paths );

	setupUi( this );

	if ( mIsDialog )
	{
		setWindowTitle( qBRATSCHEDULER_DIALOG_TITLE );
	}
	else
	{
        setAttribute( Qt::WA_QuitOnClose, true );
        setWindowIcon( QIcon("://images/BratIcon.png") );
		setWindowTitle( qBRATSCHEDULER_APP_TITLE );
		CreateMenuBar();
		SetMaximizableDialog( this );
	}

    if ( !CTasksProcessor::GetInstance() )
        CTasksProcessor::CreateInstance( mAppPaths->mExecutableDir );

	LoadTasks();

	SetupTables( tablePendingTask, tableProcTask, tableEndTask );

	mTabWidget->setCurrentIndex( 0 );

	adjustSize();
	setMinimumSize( width(), height() );
}


//virtual
CSchedulerDialog::~CSchedulerDialog()
{
}


//virtual
void CSchedulerDialog::closeEvent( QCloseEvent *event )
{
    if ( !mIsDialog )
        QTimer::singleShot( 0, qApp, SLOT( quit() ) );

    base_t::closeEvent( event );
}


/////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////


void CSchedulerDialog::LoadTasks( const CMapBratTask &data, QTableWidget* tableTasks )
{
	tableTasks->setRowCount( (int)data.size() );

	int index = 0;
	for ( CMapBratTask::const_iterator it = data.begin(); it != data.end(); it++ )
	{
		CBratTask* bratTask = it->second;		assert__( bratTask != nullptr );

		tableTasks->setItem( index, eTaskUid, new QTableWidgetItem( bratTask->GetUidAsString().c_str() ) );
		tableTasks->setItem( index, eTaskName, new QTableWidgetItem( bratTask->GetName().c_str() ) );
		tableTasks->setItem( index, eTaskStart, new QTableWidgetItem( bratTask->GetAtAsString().c_str() ) );
		tableTasks->setItem( index, eTaskStatus, new QTableWidgetItem( bratTask->GetStatusAsString().c_str() ) );
		tableTasks->setItem( index, eTaskCMD, new QTableWidgetItem( bratTask->GetCmd().c_str() ) );
		tableTasks->setItem( index, eTaskLogFile, new QTableWidgetItem( bratTask->GetLogFile().c_str() ) );

		index++;
	}
}


bool CSchedulerDialog::ChangeProcessingToPending()
{
    CVectorBratTask vectorTasks(false);
    bool somethingHasChanged = CTasksProcessor::GetInstance()->ChangeProcessingToPending(vectorTasks);
    if (!somethingHasChanged)
    {
        return somethingHasChanged;
    }

    for (CVectorBratTask::const_iterator itVector = vectorTasks.begin() ; itVector != vectorTasks.end() ; itVector++)
    {
        CBratTask* bratTask = *itVector;
        if (bratTask == nullptr)
        {
            continue;
        }
    }

    LoadTasks( *CTasksProcessor::GetInstance()->GetMapPendingBratTask(), tablePendingTask );

    return somethingHasChanged;
}



void CSchedulerDialog::action_ViewConfig_slot()
{
    BRAT_MSG_NOT_IMPLEMENTED( "This should open Config file." );
}


void CSchedulerDialog::action_UserManual_slot()
{
    BRAT_MSG_NOT_IMPLEMENTED( "This should open User's manual." );
}


void CSchedulerDialog::action_About_slot()
{
    BRAT_MSG_NOT_IMPLEMENTED( "This should open About info." );
}


///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_SchedulerDialog.cpp"
