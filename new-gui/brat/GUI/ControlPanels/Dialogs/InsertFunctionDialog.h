#if !defined GUI_CONTROLPANELS_DIALOGS_INSERTFUNCTIONDIALOG_H
#define GUI_CONTROLPANELS_DIALOGS_INSERTFUNCTIONDIALOG_H


#include <QDialog>

//class BBBB;


class CInsertFunctionDialog : public QDialog
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

	CTextWidget *mHelpText = nullptr;
    QDialogButtonBox *mButtonBox = nullptr;
	QComboBox *mFunctionCategories = new QComboBox;
	QListWidget *mFunctionList = new QListWidget;

	std::string mResultFunction;		//dialog output in Ok case

    /////////////////////////////
    //construction / destructuion
    /////////////////////////////

	void Setup();
	void CreateWidgets();
public:
    CInsertFunctionDialog( QWidget *parent );

    virtual ~CInsertFunctionDialog();

    /////////////////////////////
    // getters / setters / testers
    /////////////////////////////

	std::string ResultFunction() const { return mResultFunction; }


    /////////////////////////////
    // remaining methods
    /////////////////////////////
protected:
	void FillFunctionList( int category );

	
	virtual void  accept() override;


protected slots:

	void HandleFunctionCategoriesIndexChanged( int index );
	void HandleFunctionListRowChanged( int index );
};


#endif      // GUI_CONTROLPANELS_DIALOGS_INSERTFUNCTIONDIALOG_H
