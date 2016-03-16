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

    /////////////////////////////
    //construction / destructuion
    /////////////////////////////

public:
    CInsertAlgorithmDialog( QWidget *parent );

    virtual ~CInsertAlgorithmDialog();

    /////////////////////////////
    // getters / setters / testers
    /////////////////////////////

    /////////////////////////////
    // remaining methods
    /////////////////////////////

};


#endif      // GUI_CONTROLPANELS_DIALOGS_INSERTALGORITHMDIALOG_H
