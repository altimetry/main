#ifndef QSCHEDULERAPPLICATION
#define QSCHEDULERAPPLICATION

#include <QApplication>
#include <QString>

class QSchedulerApplication : public QApplication
{

    Q_OBJECT

    // types

    typedef QApplication base_t;

    // static data members

    // instance data members
    QString mUserManualViewer;
    QString mUserManual;

    // Methods

public:
    //explicit SchedulerDlg(QWidget *parent = 0);
    QSchedulerApplication(int &argc, char **argv, int flags = ApplicationFlags);
    QSchedulerApplication(int &argc, char **argv, bool GUIenabled, int = ApplicationFlags)
        : base_t(argc, argv, GUIenabled, ApplicationFlags)
    {}
    QSchedulerApplication(int &argc, char **argv, Type t, int = ApplicationFlags)
        : base_t(argc, argv, t, ApplicationFlags)
    {}

};




#endif // QSCHEDULERAPPLICATION

