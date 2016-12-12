/*
* This file is part of BRAT 
*
* BRAT is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* BRAT is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
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

    void CreateWidgets( const std::string& exprValue, const COperation *operation );
    void Wire();

    //Setup SqlTableDb
    void initializeModel(QStandardItemModel *model);
    QTableView *CreateTableView(QStandardItemModel *model, const QString &title);
    void addEntry(unsigned int i, QString& exp_name, QString& oUnit, QString& cUnit);

public:
    CShowInfoDialog( QWidget *parent, const CFormula *formula, const COperation *operation );

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
