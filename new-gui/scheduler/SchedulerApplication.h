#ifndef QSCHEDULER_APPLICATION
#define QSCHEDULER_APPLICATION

#include "stdafx.h"


class CConsoleApplicationPaths;





class CSchedulerApplication : public QtSingleApplication
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

    using base_t = QtSingleApplication;

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

    static int OffGuiErrorDialog( int error_code, char const *error_msg );

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

protected slots:

};




#endif // QSCHEDULER_APPLICATION
