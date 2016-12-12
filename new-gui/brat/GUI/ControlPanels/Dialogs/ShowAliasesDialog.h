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
#if !defined GUI_CONTROLPANELS_DIALOGS_SHOWALIASESDIALOG_H
#define GUI_CONTROLPANELS_DIALOGS_SHOWALIASESDIALOG_H


#include <QDialog>


class COperation;
class CFormula;



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

	enum EColums
	{
		eValue,
		eSyntax,
		eDescription,
		eSelect,

		EColums_size
	};

    /////////////////////////////
    // static data
    /////////////////////////////

    /////////////////////////////
    // instance data
    /////////////////////////////

	QTableWidget *mAliasesTable = nullptr;
	QLabel *mHeaderLabel = nullptr;
	QLabel *mFooterLabel = nullptr;
	std::vector<QCheckBox*> mSelected;

    QDialogButtonBox *mButtonBox = nullptr;


    /////////////////////////////
    //construction / destruction
    /////////////////////////////
private:

    void CreateWidgets();
    void Wire();

public:
    CShowAliasesDialog( QWidget *parent, const COperation* operation, CFormula* formula );

    virtual ~CShowAliasesDialog();

    /////////////////////////////
    // getters / setters / testers
    /////////////////////////////


	const std::vector<QCheckBox*>& CheckedItems() const { return mSelected; }

	std::string AliasSyntax( int row ) const 
	{ 
		return q2a( mAliasesTable->item( row, eSyntax )->text() );
	}


    /////////////////////////////
    // Operations
    /////////////////////////////

protected:
	QWidget* MakeSelectCell();

    virtual void accept() override;

};


#endif      // GUI_CONTROLPANELS_DIALOGS_SHOWALIASESDIALOG_H
