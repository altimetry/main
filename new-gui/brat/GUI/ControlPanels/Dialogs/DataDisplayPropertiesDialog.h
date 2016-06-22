#if !defined GUI_CONTROLPANELS_DIALOGS_DATA_DISPLAY_PROPERTIES_DIALOG_H
#define GUI_CONTROLPANELS_DIALOGS_DATA_DISPLAY_PROPERTIES_DIALOG_H


#include <QDialog>
#include "DataModels/Workspaces/Operation.h"


class CDataDisplayPropertiesDialog : public QDialog
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

    QRadioButton *mNorthComponent = nullptr;
    QRadioButton *mEastComponent  = nullptr;
    QGroupBox *mVectorBox  = nullptr;
    QDialogButtonBox *mButtonBox = nullptr;


    // domain data

    CFormula *mFormula = nullptr;
    COperation *mOperation = nullptr;


    /////////////////////////////
    //construction / destruction
    /////////////////////////////
private:

    void CreateWidgets();
    void Wire();

public:
    CDataDisplayPropertiesDialog( QWidget *parent, CFormula *formula, COperation *operation );

    virtual ~CDataDisplayPropertiesDialog();

    /////////////////////////////
    // getters / setters / testers
    /////////////////////////////

    /////////////////////////////
    // Operations
    /////////////////////////////

protected:
    virtual void accept() override;

};


#endif      // GUI_CONTROLPANELS_DIALOGS_DATA_DISPLAY_PROPERTIES_DIALOG_H
