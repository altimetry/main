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
#if !defined GUI_CONTROLPANELS_DIALOGS_SELECT_RECORD_DIALOG_H
#define GUI_CONTROLPANELS_DIALOGS_SELECT_RECORD_DIALOG_H


#include <QDialog>

namespace brathl 
{
	class CProduct;
}



class CSelectRecordDialog : public QDialog
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

	QListWidget *mRecordsList = nullptr;

    QDialogButtonBox *mButtonBox = nullptr;

	std::string mSelectedRecord;


	//...domain data

	CProduct *mProduct = nullptr;


    /////////////////////////////
    //construction / destruction
    /////////////////////////////
private:

    void CreateWidgets();
    void Wire();

public:
    CSelectRecordDialog( QWidget *parent, CProduct *product  );

    virtual ~CSelectRecordDialog();

    /////////////////////////////
    // getters / setters / testers
    /////////////////////////////


	const std::string& SelectedRecord() const { return mSelectedRecord; }

    /////////////////////////////
    // Operations
    /////////////////////////////

protected:
    virtual void accept() override;

protected slots:

	void HandleRecordsListSelection( int index );

};


#endif      // GUI_CONTROLPANELS_DIALOGS_SELECT_RECORD_DIALOG_H
