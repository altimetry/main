#if !defined GUI_CONTROLPANELS_DIALOGS_SHOWINFODIALOG_H
#define GUI_CONTROLPANELS_DIALOGS_SHOWINFODIALOG_H


#include <QDialog>
#include "DataModels/Workspaces/Operation.h"


class CShowInfoDialog : public QDialog
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
    QStandardItemModel *mInfoTable  = nullptr;
    QTableView * mTableView = nullptr;

    /////////////////////////////
    //construction / destruction
    /////////////////////////////
private:

    void CreateWidgets( const std::string& exprValue, COperation *operation );
    void Wire();

    //Setup SqlTableDb
    void initializeModel(QStandardItemModel *model);
    QTableView *CreateTableView(QStandardItemModel *model, const QString &title);
    void addEntry(unsigned int i, QString& exp_name, QString& oUnit, QString& cUnit);

public:
    CShowInfoDialog( QWidget *parent, const CFormula *formula, COperation *operation );

    virtual ~CShowInfoDialog();

    /////////////////////////////
    // getters / setters / testers
    /////////////////////////////

    /////////////////////////////
    // Operations
    /////////////////////////////

protected:
    virtual void accept() override;

};


#endif      // GUI_CONTROLPANELS_DIALOGS_SHOWINFODIALOG_H
