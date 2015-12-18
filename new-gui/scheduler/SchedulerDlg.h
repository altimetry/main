#ifndef SCHEDULER_DLG_H
#define SCHEDULER_DLG_H

#include <QtGui/QToolBar>

#include "ui_SchedulerDlg.h"
#include "TaskProcessor.h"


//// RCCC - TODO - Put in right place (here or in SchedulerApplication?)
// Creating QString with BratScheduler window title
#if defined(_WIN64) || defined(WIN64) || defined(__LP64__) || defined(__x86_64__)
    const QString qBRATSCHEDULER_DIALOG_BASE ("Brat Scheduler Dialog (64 bit)");
    const QString qBRATSCHEDULER_APP_BASE ("Brat Scheduler (64 bit)");
#else
    const QString qBRATSCHEDULER_DIALOG_BASE ("Brat Scheduler Dialog (32 bit)");
    const QString qBRATSCHEDULER_APP_BASE ("Brat Scheduler (32 bit)");
#endif
#if defined(DEBUG) || defined(_DEBUG)
    const QString qBRATSCHEDULER_DIALOG_TITLE = qBRATSCHEDULER_DIALOG_BASE + " [Debug]";
    const QString qBRATSCHEDULER_APP_TITLE = qBRATSCHEDULER_APP_BASE + " [Debug]";
#else
    const QString qBRATSCHEDULER_DIALOG_TITLE = qBRATSCHEDULER_DIALOG_BASE;
    const QString qBRATSCHEDULER_APP_TITLE = qBRATSCHEDULER_APP_BASE;
#endif


class SchedulerDlg : public QDialog, private Ui::SchedulerDlg
{
    Q_OBJECT

    // types

    // static data members

    static const auto smSCHEDULER_TIMER_INTERVAL = 30000;
    static const auto smCHECK_CONFIGFILE_TIMER_INTERVAL = 5000;

    // instance data members

    bool mIsDialog = false;

public:
    explicit SchedulerDlg(QWidget *parent = 0);

    void createMenuBar();
    void loadTasks(const CMapBratTask &data, QTableWidget* tableTasks);

private:
    bool ChangeProcessingToPending();

public slots:
    void action_ViewConfig_slot();
    void action_UserManual_slot();
    void action_About_slot();
};

#endif // SCHEDULER_DLG_H
