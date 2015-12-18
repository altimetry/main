#include "stdafx.h"

// Include private headers

#include <locale>
#include <string>

#include <QMenuBar>

#include "new-gui/Common/QtUtils.h"

#include "TaskProcessor.h"


//#include "Trace.h"
//#include "List.h"
#include "libbrathl/Tools.h"
//#include "Exception.h"
//using namespace brathl;

//#include "BratScheduler.h"
//#include "SchedulerTaskConfig.h"
//#include "RichTextFrame.h"
//#include "DirTraverser.h"

#include "SchedulerDlg.h"

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


/// RCCC - TODO - Organize or insert in the rigth place ////
// Enum task list columns
enum ETaskListCol{
    eTaskUid,
    eTaskName,
    eTaskStart,
    eTaskStatus,
    eTaskCMD,
    eTaskLogFile,
    eTaskListCol_size
};


SchedulerDlg::SchedulerDlg(QWidget *parent) : QDialog(parent)
	, mIsDialog( parent ? true : false )
{
    setupUi(this);

    if (mIsDialog)
    {
        setWindowTitle( qBRATSCHEDULER_DIALOG_TITLE );
    }
    else
    {
        setWindowTitle( qBRATSCHEDULER_APP_TITLE );
        createMenuBar();

		//RCCC (2015/11/14): Eliminate menu items in excess (left here only for "education" 
		//	purposes) and add the pertinent ones. Create event handlers (slots) for all 
		//	pertinent items even if they are left with empty bodies that simply display a 
		//	message (use "Stupid Item..." as reference and note that stupid items are not 
		//	pertinent. And that we do not want stupid things in our applications).
		//	Note also, seriously, that menu item names must end with "..." if they possibly 
		//	trigger a dialog box of any kind (the user is reassured that the action does 
		//	not take place immediately, there is yet another step that allows canceling).
		//
		//	Define the createMenuBar member function and move there all menu creation 
		//	functionality; the constructor simply invokes that function in this branch (the
		//	"else") of the if. In the "then" branch (mIsDialog is true), create button(s)
		//	to close the dialog and the respective actions (ok and cancel, or simply close:
		//	see in BratGui which of them make sense).
        //
        //	(2015/11/16)
        //  Is a maximize button possible? In a dialog? I didn't check, but please see if it is.
        //
        //	For dialogs, including dialog applications, the minimum size should be the
        //  opening one. Use setMinimumSize, or similar.
        //
        //  Disregard the following notes, until we talk (I left them here to serve as
        //      reminders for me)
        //
        //      - generate tasks xml with arguments
		//
        //      - wxBratTools::wxStringTowxLongLong_t
        //
        ///// SOME LESSONS:
        //
        //    std::vector<int> v;
        //    const size_t size = v.size();
        //    for ( size_t i = 0; i < size; ++i)
        //    {
        //        auto task = v[i];
        //
        //    }
        //    std::vector<CBratTask*> v2;
        //    for (std::vector<CBratTask*>::const_iterator it = v2.begin(); it != v2.end(); it++)
        //    {
        //        qDebug() << *it;
        //
        //        //it->->GetUidAsString();
        //
        //        (**it).GetUidAsString();
        //        (*it)->GetUidAsString();
        //
        //    }
        //      QString s = t2q(std::string("sdjfhgi"));
        //      std::string s2 = q2t<std::string>(QString("sdjfhgi"));
        //      auto s3 = n2s<std::string>(890);
        //      auto n = s2n<double>(std::string("789") );
        //
        //      for ( auto &t : data )
        //      {
        //          qDebug() << t.first;
        //          qDebug() << t.second->GetUidAsString().c_str();
        //      }
        ///////////////////////////////////////////////////
    }

    //// RCCC - TODO - Put in right place
    // Enabling sorting in all tables: pending, processing and end tasks.
    tablePendingTask->setSortingEnabled(true);
    tableProcTask->setSortingEnabled(true);
    tableEndTask->setSortingEnabled(true);

    // Changing selection behaviour to whole row selection
    tablePendingTask->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableProcTask->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableEndTask->setSelectionBehavior(QAbstractItemView::SelectRows);

    // Setting alternating row colors
    tablePendingTask->setAlternatingRowColors(true);
    tableProcTask->setAlternatingRowColors(true);
    tableEndTask->setAlternatingRowColors(true);

    //tablePendingTask->setEditTriggers();

    // Disable grid lines.
    tablePendingTask->setShowGrid(false);
    tableProcTask->setShowGrid(false);
    tableEndTask->setShowGrid(false);

    loadTasks(*CTasksProcessor::GetInstance()->GetMapPendingBratTask(),          tablePendingTask);
    //loadTasks(*CTasksProcessor::GetInstance()->GetMapProcessingBratTask(), tableProcTask);
    loadTasks(*CTasksProcessor::GetInstance()->GetMapEndedBratTask(),            tableEndTask);

    ChangeProcessingToPending();
}

void SchedulerDlg::createMenuBar()
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

    mVerticalLayout->setMenuBar(menuBar);
}

void SchedulerDlg::loadTasks(const CMapBratTask &data, QTableWidget* tableTasks)
{
    tableTasks->setRowCount(data.size());

    size_t index = 0;
    for (CMapBratTask::const_iterator it = data.begin(); it != data.end(); it++)
    {
      CBratTask* bratTask = it->second;
      //// InsertTask(bratTask);////
      if (bratTask == NULL)
      {
        //return -1;
      }

      tableTasks->setItem(index, eTaskUid,     new QTableWidgetItem(bratTask->GetUidAsString().c_str()) );
      tableTasks->setItem(index, eTaskName,    new QTableWidgetItem(bratTask->GetName().c_str()));
      tableTasks->setItem(index, eTaskStart,   new QTableWidgetItem(bratTask->GetAtAsString().c_str()));
      tableTasks->setItem(index, eTaskStatus,  new QTableWidgetItem(bratTask->GetStatusAsString().c_str()));
      tableTasks->setItem(index, eTaskCMD,     new QTableWidgetItem(bratTask->GetCmd().c_str()));
      tableTasks->setItem(index, eTaskLogFile, new QTableWidgetItem(bratTask->GetLogFile().c_str()));

      index++;
    }
}


bool SchedulerDlg::ChangeProcessingToPending()
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
        if (bratTask == NULL)
        {
            continue;
        }
    }

    loadTasks(*CTasksProcessor::GetInstance()->GetMapPendingBratTask(),          tablePendingTask);

    return somethingHasChanged;
}



void SchedulerDlg::action_ViewConfig_slot()
{
    SimpleMsgBox("This should open Config file.");
}

void SchedulerDlg::action_UserManual_slot()
{
    SimpleMsgBox("This should open User's manual.");
}

void SchedulerDlg::action_About_slot()
{
    SimpleMsgBox("This should open About info.");
}


///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_SchedulerDlg.cpp"
