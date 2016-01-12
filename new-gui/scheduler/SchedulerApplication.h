#ifndef QSCHEDULER_APPLICATION
#define QSCHEDULER_APPLICATION

#include <QApplication>
#include <QString>


class QSchedulerApplication : public QApplication
{
    Q_OBJECT

    // types

    using base_t = QApplication;

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

#if defined( QT_VERSION) && (QT_VERSION < 050000)
    QSchedulerApplication(int &argc, char **argv, bool GUIenabled, int = ApplicationFlags)
        : base_t(argc, argv, GUIenabled, ApplicationFlags)
    {}

    QSchedulerApplication(int &argc, char **argv, Type t, int = ApplicationFlags)
        : base_t(argc, argv, t, ApplicationFlags)
    {}
#endif

    virtual ~QSchedulerApplication();

};




#endif // QSCHEDULER_APPLICATION
