#if !defined SCHEDULER_EDIT_TASKS_FILE_DIALOG_H
#define SCHEDULER_EDIT_TASKS_FILE_DIALOG_H


#include <QDialog>


class CEditTasksFileDialog : public QDialog
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

	CTextWidget *mTextEdit = nullptr;
    QDialogButtonBox *mButtonBox = nullptr;

	const std::string mTaskName;
	const std::string mPath;


    /////////////////////////////
    //construction / destruction
    /////////////////////////////
private:

    void CreateWidgets();
    void Wire();

public:
    CEditTasksFileDialog( const std::string &task_name, const std::string &path, QWidget *parent );

    virtual ~CEditTasksFileDialog();

    /////////////////////////////
    // getters / setters / testers
    /////////////////////////////

    /////////////////////////////
    // Operations
    /////////////////////////////

protected:
    virtual void accept() override;

};



#endif      // SCHEDULER_EDIT_TASKS_FILE_DIALOG_H
