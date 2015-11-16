// Include private headers

#include <locale>
#include <string>

#include <QMenuBar>

//#include "Trace.h"
//#include "List.h"
//#include "Tools.h"
//#include "Exception.h"
//using namespace brathl;

//#include "BratScheduler.h"
//#include "SchedulerTaskConfig.h"
//#include "RichTextFrame.h"
//#include "DirTraverser.h"


// When debugging changes all calls to new to be calls to DEBUG_NEW allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

#ifdef WIN32
const std::string BRATHL_ICON_FILENAME = "BratIcon.ico";
#else
const std::string BRATHL_ICON_FILENAME = "BratIcon.bmp";
#endif

//const wxString BRATGUI_APPNAME = "scheduler";
//const wxString GROUP_COMMON = "Common";
//const wxString ENTRY_USER_MANUAL = "UserManual";
//const wxString ENTRY_USER_MANUAL_VIEWER = "UserManualViewer";




#include "schedulerdlg.h"

SchedulerDlg::SchedulerDlg(QWidget *parent) :
    QDialog(parent), mIsDialog( parent ? true : false )
{
    setupUi(this);

    if (mIsDialog)
    {
        setWindowTitle( "Brat Scheduler Dialog");
    }
    else
    {
        setWindowTitle( "Brat Scheduler");

        QVBoxLayout *mMainLayout = new QVBoxLayout( this );

        QMenuBar *menuBar = new QMenuBar(this);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 957, 21));
        QMenu *menu_File = new QMenu(menuBar);
        menu_File->setObjectName(QString::fromUtf8("menu_File"));
        QMenu *menuNew_Plot = new QMenu(menu_File);
        menuNew_Plot->setObjectName(QString::fromUtf8("menuNew_Plot"));
        QMenu *menu_Edit = new QMenu(menuBar);
        menu_Edit->setObjectName(QString::fromUtf8("menu_Edit"));
        QMenu *menu_Format = new QMenu(menu_Edit);
        menu_Format->setObjectName(QString::fromUtf8("menu_Format"));
        QMenu *menu_Window = new QMenu(menuBar);
        menu_Window->setObjectName(QString::fromUtf8("menu_Window"));
        QMenu *menu_Help = new QMenu(menuBar);
        menu_Help->setObjectName(QString::fromUtf8("menu_Help"));
        QMenu *menu_View = new QMenu(menuBar);
        menu_View->setObjectName(QString::fromUtf8("menu_View"));
        QMenu *menu_Tools = new QMenu(menuBar);
        menu_Tools->setObjectName(QString::fromUtf8("menu_Tools"));

        menuBar->addAction(menu_File->menuAction());
        menuBar->addAction(menu_Edit->menuAction());
        menuBar->addAction(menu_View->menuAction());
        menuBar->addAction(menu_Tools->menuAction());
        menuBar->addAction(menu_Window->menuAction());
        menuBar->addAction(menu_Help->menuAction());

        QAction *action_New = new QAction(this);
        QObject::connect(action_New, SIGNAL(triggered()), this, SLOT(close()));

        menu_File->addAction(action_New);

        mMainLayout->addWidget(menuBar);
        //setMenuBar(menuBar);

        //mMainLayout->addLayout(gridLayout);
        mMainLayout->addWidget(tabWidget);
        setLayout(mMainLayout);


//        QToolBar *toolBar = new QToolBar();
//        gridLayout->addWidget(toolBar);
//        QAction *action1 = new QAction("Add", toolBar);
//        QAction *action2 = new QAction("Del", toolBar);

//        menuBar = new QMenuBar(this)
//        layout()->setMenuBar(menuBar);

          //QMainWindow* mainWindow = new QMainWindow();
    }

    // Single Instance Checker TODO!!!!!

    // To be sure that number have always a decimal point (and not a comma
    // or something else)
    setlocale(LC_NUMERIC, "C");
    

}
