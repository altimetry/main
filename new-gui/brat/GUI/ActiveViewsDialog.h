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
#if !defined GUI_ACTIVE_VIEWS_DIALOG_H
#define GUI_ACTIVE_VIEWS_DIALOG_H

#include <QDialog>

#include "DesktopManager.h"


class CActiveViewsDialog : public QDialog
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

	Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif


	/////////////////////////////
	//	Types
	/////////////////////////////

	using base_t = QDialog;


	/////////////////////////////
	//	Instance Data
	/////////////////////////////

	QListWidget *mViewsListWidget = nullptr;
	QPushButton *mCloseDialogButton = nullptr;
	QPushButton *mCloseWindowButton = nullptr;
	QPushButton *mSelectWindowButton = nullptr;

	QList< QWidget* > mSubWindows;

	CDesktopManagerBase *mDesktopManager = nullptr;

public:

	/////////////////////////////
	//	Construction /Destruction
	/////////////////////////////

private:

	void CreateWidgets();
	void Wire();

public:
	explicit CActiveViewsDialog( QWidget *parent, CDesktopManagerBase *manager );

    virtual ~CActiveViewsDialog();


	QList< QWidget* > SubWindows() { return mSubWindows; }


protected:
	virtual QSize sizeHint() const override;

	virtual void accept() override;

	void RefreshWindowsList();

protected slots:

	void HandleCloseWindow();
	void HandleSelectWindow();
};


#endif	//GUI_ACTIVE_VIEWS_DIALOG_H
