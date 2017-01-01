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
#include "new-gui/brat/stdafx.h"

#include "DesktopControlPanel.h"



/////////////////////////////////////////////////////////////////////////////////////
//					Control Panel Specialized for Desktop
/////////////////////////////////////////////////////////////////////////////////////



//explicit 
CDesktopControlsPanel::CDesktopControlsPanel( CModel &model, CDesktopManagerBase *manager, QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0 
	: base_t( parent, f ), mModel( model ), mDesktopManager( manager )
{}


//virtual 
void CDesktopControlsPanel::ChangePanelSelection( bool selected )
{
	mSelectedPanel = selected;
	UpdatePanelSelectionChange();
}


///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_DesktopControlPanel.cpp"
