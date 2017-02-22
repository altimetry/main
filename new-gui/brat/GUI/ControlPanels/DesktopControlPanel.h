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


class CBratApplication;
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


	struct CPendingTracks
	{
		const CDataset *mDataset;
		bool mForceRedraw;
	};


protected:

	using stack_button_type = CStackedWidget::stack_button_type;


	//static members

private:

	static CPendingTracks *smPendingTrack;

	static bool smAutoSatelliteTrack;
	static const CDataset *smDataset;
	static bool smSomeTrackDisplayed;
	static int smTotalRecords;


protected:

	static const CDataset* CurrentMapDataset()
	{
		return smDataset;
	}


public:

	// Setter must redraw map; so, it must be instance member,
	//	for instance, to access the widget's parent
	//
	static bool AutoDrawSatelliteTrack()
	{
		return smAutoSatelliteTrack;
	}


	//instance data

private:
	bool mSelectedPanel = false;					

protected:
	CBratApplication &mApp;
	CModel &mModel;
	CDesktopManagerBase *mDesktopManager = nullptr;
	CMapWidget *mMap = nullptr;


	//construction / destruction

public:
	explicit CDesktopControlsPanel( CBratApplication &app, CDesktopManagerBase *manager, QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~CDesktopControlsPanel();


	// access 

	//...satellite tracks

	void SetAutoDrawSatelliteTrack( bool track )
	{
		smAutoSatelliteTrack = track;
		DrawDatasetTracks( smDataset, false );
	}

	// force_redraw: 
	//	- always redraw if dataset exists, even if equal to the cached smDataset
	//	- always clear if dataset is null, even if equal to the cached smDataset (that is, smDataset is also null)
	//	(if smAutoSatelliteTrack is false, it has no effect)
	//
	// Returns true if tracks were refreshed (even if reading data issued errors and nothing was drawn)
	// Returns false if 
	//	- tracks were only cleared and/or there was nothing to refresh (no new data to read)
	//	- function was busy and dataset saved to draw tracks later
	//
	bool DrawDatasetTracks( const CDataset *dataset, bool force_redraw );


	//...panel selection

	bool SelectedPanel() const { return mSelectedPanel; }

	// To be called when this is in a widget container and its selection state changed
	//
	void ChangePanelSelection( bool selected );


protected:

	// This a pure function BUT it has a (base class) implementation
	// Derived classes must call base class WorkspaceChanged() whenever they receive 
	//	the respective signal. 
	//
	virtual void WorkspaceChanged() = 0;


	// Derived classes must call SelectedPanel() in overrider methods to know their current
	//	selection status
	//
	virtual void UpdatePanelSelectionChange() = 0;


	int TotalRecords() const { return smTotalRecords; }


	void SelectAreaInMap( double lonm, double lonM, double latm, double latM );


	void RemoveAreaSelectionFromMap();


	void UpdateMapTitle();

public:

	////////////////////////////
	// GUI
	////////////////////////////


	static 
	QgsCollapsibleGroupBox* CreateCollapsibleGroupBox( ELayoutType o, const std::vector< QObject* > &v, const QString &title = "", QWidget *parent = nullptr,
		int spacing = smSpacing, int left = smLeft, int top = smTop, int right = smRight, int bottom = smBottom )
	{
		return ::CreateGroupBox< QgsCollapsibleGroupBox >( o, v, title, parent, spacing, left, top, right, bottom);
	}


protected slots:

	void ProcessPendingTracks();
};



#endif	//GUI_CONTROL_PANELS_DESKTOP_CONTROL_PANEL_H
