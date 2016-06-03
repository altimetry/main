#ifndef SCHEDULER_DLG_H
#define SCHEDULER_DLG_H

#if defined(QT_VERSION) && (QT_VERSION < 0x050000)
#include <QtGui/QToolBar>
#else
#include <QtWidgets/QToolBar>
#endif

#include "ui_SchedulerDialog.h"
#include "new-gui/Common/DataModels/TaskProcessor.h"


class CConsoleApplicationPaths;
class CBasicLogShell;


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


class CSchedulerDialog : public QDialog, private Ui::CSchedulerDialog
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

	using base_t = QDialog;

	enum ETaskListCol
	{
		eTaskUid,
		eTaskName,
		eTaskStart,
		eTaskStatus,
		eTaskCMD,
		eTaskLogFile,
		eTaskListCol_size
	};


    // static data members

    static const auto smSCHEDULER_TIMER_INTERVAL = 30000;
    static const auto smCHECK_CONFIGFILE_TIMER_INTERVAL = 5000;

    // instance data members

    CBasicLogShell *mLogFrame = nullptr;
	int mLogFrameIndex = -1;
	QHash< int, QColor >	mSeverityToColorTable;
	QHash< int, QString >	mSeverityToPromptTable;

    bool mIsDialog = false;
    const CConsoleApplicationPaths *mAppPaths;



	// construction / destruction

    void CreateMenuBar();
	
	template< typename... Args > 
	void SetupTables( Args... args );
    
	void LoadTasks();

public:
    CSchedulerDialog( const CConsoleApplicationPaths *paths, QWidget *parent = 0);

    virtual ~CSchedulerDialog();


	// operations

protected:
    void LoadTasks(const CMapBratTask &data, QTableWidget* tableTasks);
    bool ChangeProcessingToPending();

	// signals / slots

    virtual void closeEvent( QCloseEvent *event ) override;

public slots:
    void action_ViewConfig_slot();
    void action_UserManual_slot();
    void action_About_slot();
};

#endif // SCHEDULER_DLG_H
