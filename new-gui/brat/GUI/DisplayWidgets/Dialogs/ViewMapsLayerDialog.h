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
#ifndef GUI_DISPLAY_WIDGETS_DIALOGS_MapViewsLayerDialog_H
#define GUI_DISPLAY_WIDGETS_DIALOGS_MapViewsLayerDialog_H


#include <QDialog>

#include "../MapWidget.h"


class QRadioButton;
class QDialogButtonBox;



class CViewMapsLayerDialog : public QDialog
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

	using ELayerBaseType = CMapWidget::ELayerBaseType;


	//static members



	//instance data

	QRadioButton *mViewsUseVectorLayer = nullptr;
	QRadioButton *mViewsUseRasterLayer = nullptr;
    QRadioButton *mViewsUseRasterLayerWMS = nullptr;

    QDialogButtonBox *mButtonBox = nullptr;


	ELayerBaseType mLayerBaseType = ELayerBaseType::eVectorLayer;


	//construction / destruction
	// 

	void CreateWidgets( bool enable_uri, bool enable_wms_uri );
	void Wire();
public:
    explicit CViewMapsLayerDialog( ELayerBaseType type, bool enable_uri, bool enable_wms_uri, QWidget *parent );
	virtual ~CViewMapsLayerDialog();


	// access
	
	ELayerBaseType LayerBaseType() const
	{
		return mLayerBaseType;
	}


protected:
    bool ValidateAndAssign();


private slots:
    virtual void accept();

};

#endif // GUI_DISPLAY_WIDGETS_DIALOGS_MapViewsLayerDialog_H
