#if !defined GUI_CONTROLPANELS_DIALOGS_INSERTALGORITHMDIALOG_H
#define GUI_CONTROLPANELS_DIALOGS_INSERTALGORITHMDIALOG_H


#include <QDialog>

//class BBBB;


class CInsertAlgorithmDialog : public QDialog
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

	QListWidget *mAlgorithmsList = nullptr;
	CTextWidget *mDescriptionTextWidget = nullptr;
	QTableWidget *mParametersTable = nullptr;
	QLineEdit *mOutputUnitTextWidget = nullptr;

    QDialogButtonBox *mButtonBox = nullptr;

	//...domain data

	CBratAlgorithmBase *mCurrentAlgorithm = nullptr;


    /////////////////////////////
    //construction / destruction
    /////////////////////////////
private:

    void CreateWidgets();
    void Wire();

public:
    CInsertAlgorithmDialog( QWidget *parent );

    virtual ~CInsertAlgorithmDialog();

    /////////////////////////////
    // getters / setters / testers
    /////////////////////////////

	const CBratAlgorithmBase *CurrentAlgorithm() const { return mCurrentAlgorithm; }


    /////////////////////////////
    // Operations
    /////////////////////////////

protected:
	void FillAlgorithmsList();

    virtual void accept() override;


protected slots:

	void HandleAlgorithmsListSelection( int index );
};


#endif      // GUI_CONTROLPANELS_DIALOGS_INSERTALGORITHMDIALOG_H
