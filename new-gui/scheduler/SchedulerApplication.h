#ifndef QSCHEDULER_APPLICATION
#define QSCHEDULER_APPLICATION

#include <QApplication>
#include <QString>

class CConsoleApplicationPaths;





class CSchedulerApplication : public QApplication
{
    Q_OBJECT

    // types

    using base_t = QApplication;

    // static data members

    static const CConsoleApplicationPaths *smApplicationPaths;

protected:

#if defined(MEM_LEAKS)
    static _CrtMemState FirstState;			//before all run-time allocations
    static _CrtMemState BeforeRunState;		//after app creation, before running engine
    static _CrtMemState AfterRunState;		//after running engine, before de-allocating
#endif

    // static member functions

public:
    static void dumpMemoryStatistics();

    static int OffGuiErrorDialog( int error_type, char const *error_msg );

    static const CConsoleApplicationPaths* ApplicationPaths()
    {
        return smApplicationPaths;
    }

private:

    // instance data members

    QString mUserManualViewer;
    QString mUserManual;

    // Ctors / Dtor

public:
    CSchedulerApplication( int &argc, char **argv, int flags = ApplicationFlags );

#if defined( QT_VERSION) && (QT_VERSION < 050000)
    CSchedulerApplication(int &argc, char **argv, bool GUIenabled, int = ApplicationFlags)
        : base_t(argc, argv, GUIenabled, ApplicationFlags)
    {}

    CSchedulerApplication(int &argc, char **argv, Type t, int = ApplicationFlags)
        : base_t(argc, argv, t, ApplicationFlags)
    {}
#endif

    virtual ~CSchedulerApplication();

};




#endif // QSCHEDULER_APPLICATION
