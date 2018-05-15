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
#if !defined GUI_CONTROLPANELS_DIALOGS_EDIT_FILTER_FIELDS_DIALOG_H
#define GUI_CONTROLPANELS_DIALOGS_EDIT_FILTER_FIELDS_DIALOG_H


#include <QDialog>


class COperation;



class CEditFilterFieldsDialog : public QDialog
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

	QLineEdit *mLongitudeLineEdit = nullptr;
	QLineEdit *mLatitudeLineEdit = nullptr;
	QLineEdit *mTimeLineEdit = nullptr;
	QPushButton *mResetPushButton = nullptr;

    QDialogButtonBox *mButtonBox = nullptr;

	//...domain data
	
	COperation *mOperation = nullptr;


    /////////////////////////////
    //construction / destruction
    /////////////////////////////

    void CreateWidgets();
    void Wire();

public:
    CEditFilterFieldsDialog( COperation *operation, QWidget *parent );

    virtual ~CEditFilterFieldsDialog();

    /////////////////////////////
    // getters / setters / testers
    /////////////////////////////

    /////////////////////////////
    // Operations
    /////////////////////////////

protected:
	void Reset();

	virtual void accept() override;

protected slots:

	void HandleResetPushButton();
};


#endif      // GUI_CONTROLPANELS_DIALOGS_EDIT_FILTER_FIELDS_DIALOG_H
