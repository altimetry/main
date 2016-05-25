#if !defined GUI_CONTROLPANELS_DIALOGS_DELAYEXECUTIONDIALOG_H
#define GUI_CONTROLPANELS_DIALOGS_DELAYEXECUTIONDIALOG_H


#include <QDialog>




class CDelayExecutionDialog : public QDialog
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

    /////////////////////////////
    //	types
    /////////////////////////////

    using base_t = QDialog;

    /////////////////////////////
    // static data
    /////////////////////////////

    /////////////////////////////
    // instance data
    /////////////////////////////

	QDateTimeEdit *mExecDateEdit = nullptr;
	QLineEdit *mTaskLabelLineEdit = nullptr;

    QDialogButtonBox *mButtonBox = nullptr;

	std::string &mTaskLabel;
	QDateTime &mDateTime;

    /////////////////////////////
    //construction / destruction
    /////////////////////////////
private:

    void CreateWidgets();
    void Wire();

public:
    CDelayExecutionDialog( std::string &label, QDateTime &dt, QWidget *parent );

    virtual ~CDelayExecutionDialog();

    /////////////////////////////
    // getters / setters / testers
    /////////////////////////////

    /////////////////////////////
    // Operations
    /////////////////////////////

protected:
    virtual void accept() override;

};


#endif      // GUI_CONTROLPANELS_DIALOGS_DELAYEXECUTIONDIALOG_H
