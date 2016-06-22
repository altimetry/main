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




#endif      // GUI_CONTROLPANELS_DIALOGS_FORMULADIALOG_H
