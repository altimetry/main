#if !defined GUI_CONTROLPANELS_DIALOGS_SELECTIONCRITERIADIALOG_H
#define GUI_CONTROLPANELS_DIALOGS_SELECTIONCRITERIADIALOG_H


#include <QDialog>

//class BBBB;


class CSelectionCriteriaDialog : public QDialog
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
    CSelectionCriteriaDialog( QWidget *parent );

    virtual ~CSelectionCriteriaDialog();

    /////////////////////////////
    // getters / setters / testers
    /////////////////////////////

    /////////////////////////////
    // remaining methods
    /////////////////////////////

};


#endif      // GUI_CONTROLPANELS_DIALOGS_SELECTIONCRITERIADIALOG_H
