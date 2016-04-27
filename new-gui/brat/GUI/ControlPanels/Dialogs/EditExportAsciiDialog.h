#if !defined GUI_CONTROLPANELS_DIALOGS_EXPORT_ASCII_DIALOG_H
#define GUI_CONTROLPANELS_DIALOGS_EXPORT_ASCII_DIALOG_H


#include <QDialog>


#include "new-gui/brat/DataModels/Workspaces/Operation.h"


class CEditExportAsciiDialog : public QDialog
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
    CEditExportAsciiDialog( QWidget *parent );

    virtual ~CEditExportAsciiDialog();

    /////////////////////////////
    // getters / setters / testers
    /////////////////////////////

    /////////////////////////////
    // Operations
    /////////////////////////////

protected:
    virtual void accept() override;

};



#endif      // GUI_CONTROLPANELS_DIALOGS_EXPORT_ASCII_DIALOG_H
