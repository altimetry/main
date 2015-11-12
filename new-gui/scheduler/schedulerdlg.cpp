// Include private headers

#include <locale>
#include <string>

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

//const wxString BRATGUI_APPNAME = "BratGui";
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
    }

    // Single Instance Checker TODO!!!!!

    // To be sure that number have always a decimal point (and not a comma
    // or something else)
    setlocale(LC_NUMERIC, "C");
    

}
