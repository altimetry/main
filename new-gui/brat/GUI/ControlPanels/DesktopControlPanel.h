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
#if !defined GUI_CONTROL_PANELS_DESKTOP_CONTROL_PANEL_H
#define GUI_CONTROL_PANELS_DESKTOP_CONTROL_PANEL_H

#include "libbrathl/Product.h"

#include "new-gui/Common/GUI/ControlPanel.h"
#include "GUI/DesktopManager.h"
#include "GUI/StackedWidget.h"


class CWorkspaceDataset;
class CWorkspaceOperation;
class CWorkspaceDisplay;
class CModel;



/////////////////////////////////////////////////////////////////////////////////////
//					Control Panel Specialized for Desktop
/////////////////////////////////////////////////////////////////////////////////////


class CDesktopControlsPanel : public CControlPanel
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

	using base_t = CControlPanel;

protected:
	using stack_button_type = CStackedWidget::stack_button_type;


	//instance data

protected:
	CModel &mModel;
	CDesktopManagerBase *mDesktopManager = nullptr;

	//construction / destruction

public:
	explicit CDesktopControlsPanel( CModel &model, CDesktopManagerBase *manager, QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~CDesktopControlsPanel()
	{}	


	// To be called when this is in a widget container and its selection state changed
	//	- useful to 
	//
	virtual void SelectionChanged( bool selected ) = 0;


	////////////////////////////
	// GUI
	////////////////////////////


	static 
	QgsCollapsibleGroupBox* CreateCollapsibleGroupBox( ELayoutType o, const std::vector< QObject* > &v, const QString &title = "", QWidget *parent = nullptr,
		int spacing = smSpacing, int left = smLeft, int top = smTop, int right = smRight, int bottom = smBottom )
	{
		return ::CreateGroupBox< QgsCollapsibleGroupBox >( o, v, title, parent, spacing, left, top, right, bottom);
	}
};



#endif	//GUI_CONTROL_PANELS_DESKTOP_CONTROL_PANEL_H
