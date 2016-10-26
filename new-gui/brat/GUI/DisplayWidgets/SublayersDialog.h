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
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA	 02110-1301, USA.
*/
#ifndef GUI_DISPLAY_WIDGETS_SUB_LAYERS_DIALOG_H
#define GUI_DISPLAY_WIDGETS_SUB_LAYERS_DIALOG_H

#include <QDialog>


class CSublayersDialog : public QDialog
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	//types

	using base_t = QDialog;

protected:

	//instance data

	QTreeWidget *mLayersTable = nullptr;


	//construction / destruction

	void CreateWidgets();
public:
    CSublayersDialog( QWidget* parent, QStringList list, QString delim = ":" , Qt::WindowFlags fl = 0 );
    virtual ~CSublayersDialog();

	//access

    // Returns selected layer. If there are more layers with the same name,
    // geometry type is appended separated by semicolon, example: <layer>:<geometryType>
	//
    QString selectionName();

protected:
	virtual void  accept() override;

public slots:

    int exec()
#if QT_VERSION >= 0x050000
    override;
#else
    ;
#endif

};

#endif      //GUI_DISPLAY_WIDGETS_SUB_LAYERS_DIALOG_H
