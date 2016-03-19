#if !defined GUI_CONTROLPANELS_DIALOGS_FORMULADIALOG_H
#define GUI_CONTROLPANELS_DIALOGS_FORMULADIALOG_H


#include <QDialog>



class CWorkspaceFormula;


class CFormulaDialog : public QDialog
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

	QListWidget *mFormulasListWidget = nullptr;
	QPushButton *mRemoveButton = nullptr;
	QLineEdit *mUniLineEdit = nullptr;
	QTextEdit *mFormulaContent = nullptr;
	QCheckBox *mAsAliasCheck = nullptr;
	bool mAsAlias = false;

    QDialogButtonBox *mButtonBox = nullptr;


	//...domain data

	CWorkspaceFormula *mWFormula = nullptr;
	const CFormula *mCurrentFormula = nullptr;


    /////////////////////////////
    //construction / destruction
    /////////////////////////////
private:

    void CreateWidgets();
    void Wire();

public:
    CFormulaDialog( CWorkspaceFormula *wkspc, QWidget *parent );

    virtual ~CFormulaDialog();

    /////////////////////////////
    // getters / setters / testers
    /////////////////////////////

	bool AsAlias() const { return mAsAlias; }

	const CFormula* CurrentFormula() const { return mCurrentFormula; }


    /////////////////////////////
    // Operations
    /////////////////////////////

protected:
	void FillFormulaList();

    virtual void accept() override;


protected slots:

	void HandleSelectedFormulaChanged( int index );
	void HandleAsAliasCheckToggled( bool toggled );
	void HandleRemoveButtonClicked();
};




class CSaveAsFormula : public QDialog
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

    QDialogButtonBox *mButtonBox = nullptr;


    /////////////////////////////
    //construction / destruction
    /////////////////////////////
private:

    void CreateWidgets();
    void Wire();

public:
    CSaveAsFormula( QWidget *parent );

    virtual ~CSaveAsFormula();

    /////////////////////////////
    // getters / setters / testers
    /////////////////////////////

    /////////////////////////////
    // Operations
    /////////////////////////////

protected:
    virtual void accept() override;

};


#endif      // GUI_CONTROLPANELS_DIALOGS_FORMULADIALOG_H
