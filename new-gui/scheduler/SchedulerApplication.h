#ifndef QSCHEDULER_APPLICATION
#define QSCHEDULER_APPLICATION

#include <QApplication>
#include <QString>


class QSchedulerApplication : public QApplication
{
    Q_OBJECT

    // types

    typedef QApplication base_t;

    // static data members

#if defined(MEM_LEAKS)
    static _CrtMemState FirstState;			//before all run-time allocations
    static _CrtMemState BeforeRunState;		//after app creation, before running engine
    static _CrtMemState AfterRunState;		//after running engine, before de-allocating
#endif

    // static member functions

public:
    static void dumpMemoryStatistics();

private:

    // instance data members

    QString mUserManualViewer;
    QString mUserManual;

    // Ctors / Dtor

public:
    QSchedulerApplication( int &argc, char **argv, int flags = ApplicationFlags );

    QSchedulerApplication(int &argc, char **argv, bool GUIenabled, int = ApplicationFlags)
        : base_t(argc, argv, GUIenabled, ApplicationFlags)
    {}

    QSchedulerApplication(int &argc, char **argv, Type t, int = ApplicationFlags)
        : base_t(argc, argv, t, ApplicationFlags)
    {}

    virtual ~QSchedulerApplication();

};




#endif // QSCHEDULER_APPLICATION
