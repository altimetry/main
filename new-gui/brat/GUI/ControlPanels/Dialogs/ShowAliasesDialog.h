#if !defined GUI_CONTROLPANELS_DIALOGS_SHOWALIASESDIALOG_H
#define GUI_CONTROLPANELS_DIALOGS_SHOWALIASESDIALOG_H


#include <QDialog>

//class BBBB;


class CShowAliasesDialog : public QDialog
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

    /////////////////////////////
    //construction / destructuion
    /////////////////////////////

public:
    CShowAliasesDialog( QWidget *parent );

    virtual ~CShowAliasesDialog();

    /////////////////////////////
    // getters / setters / testers
    /////////////////////////////

    /////////////////////////////
    // remaining methods
    /////////////////////////////

};


#endif      // GUI_CONTROLPANELS_DIALOGS_SHOWALIASESDIALOG_H
