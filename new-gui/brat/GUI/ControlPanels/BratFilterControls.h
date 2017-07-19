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
#ifndef GUI_CONTROL_PANELS_DATASET_FILTER_CONTROLS_H
#define GUI_CONTROL_PANELS_DATASET_FILTER_CONTROLS_H

// File Index:
//
//	- Filter Control Panel
//


#include "DesktopControlPanel.h"

class CDataset;



/////////////////////////////////////////////////////////////////////////////////////
//								Dataset Filter
/////////////////////////////////////////////////////////////////////////////////////

class CBratFilterControls : public CDesktopControlsPanel
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

	Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	// types

	using base_t = CDesktopControlsPanel;

	//instance data

	//...buttons row

    QToolButton *mNewFilter = nullptr;
    QToolButton *mRenameFilter = nullptr;
    QToolButton *mDeleteFilter = nullptr;
    QToolButton *mSaveFilters = nullptr;

	//where

    QToolButton *mClearWhere = nullptr;
    QComboBox *mFiltersCombo = nullptr;
	QListWidget *mAreasListWidget = nullptr;
    QComboBox *mRegionsCombo = nullptr;

    QPushButton *mShowAllAreas = nullptr;
    QToolButton *mRegionSettings = nullptr;

    QToolButton *mNewArea = nullptr;
    QToolButton *mAddKML = nullptr;
    QToolButton *mAddMask = nullptr;
    QToolButton *mRenameArea = nullptr;
    QToolButton *mDeleteArea = nullptr;

	QLineEdit *mFirstCoordEdit = nullptr;	//point to the 1st of the following 4 in the Z-order
	QLineEdit *mMaxLatEdit = nullptr;
    QLineEdit *mMaxLonEdit = nullptr;
    QLineEdit *mMinLatEdit = nullptr;
    QLineEdit *mMinLonEdit = nullptr;

	QAction *mActionSelectFeatures = nullptr;
	QAction *mActionDeselectAll = nullptr;
	QToolButton *mMapSelectionButton = nullptr;

	QWidget *mWhereBox = nullptr;

	//when

    QToolButton *mClearWhen = nullptr;
	QRadioButton *mUseTimeRadio = nullptr;
	QRadioButton *mUseCyclePassRadio = nullptr;
	QDateTimeEdit *mStartTimeEdit = nullptr;
    QDateTimeEdit *mStopTimeEdit = nullptr;

    QLineEdit *mStartCycleEdit = nullptr;
    QLineEdit *mStopCycleEdit = nullptr;
    QLineEdit *mStartPassEdit = nullptr;
    QLineEdit *mStopPassEdit = nullptr;
    QGroupBox *mAbsoluteTimesBox = nullptr;

    QLineEdit *mRelativeStart = nullptr;
    QLineEdit *mRelativeStop = nullptr;
    QDateTimeEdit *mRefDateTimeEdit = nullptr;
    QCheckBox *mUseCurrentDateTime = nullptr;
    QGroupBox *mRelativeTimesBox = nullptr;

    QWidget *mWhenBox = nullptr;


	//...domain data

	CBratFilters &mBratFilters;
	CBratAreas &mBratAreas;
	CBratRegions &mBratRegions;
	CBratFilter *mFilter = nullptr;

	CWorkspaceOperation *mWOperation = nullptr;


	//construction / destruction

	void CreateWidgets();
	void Wire();
public:
	explicit CBratFilterControls( CBratApplication &app, CDesktopManagerBase *manager, QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~CBratFilterControls()
	{}	


	// overrides

	virtual void WorkspaceChanged() override;

	virtual void UpdatePanelSelectionChange() override;


	// operations

protected:

	bool ReloadFilters();
	void FillFiltersCombo();
    void FillRegionsCombo();
    void FillAreasList();
    void ShowOnlyAreasInRegion(int region_index);
    void SaveAllAreas();
    void ValidateAndStoreValue(QLineEdit *TextBox, int &ValueInFilter, int ParamDef, int min, int max);
    void updateDateWidgets();
    void updateCyclePassWidgets();
    void updateRelativeTimeWidgets();

	void EndAreaSelection();
	bool ValidateArea( double &min_lon, double &max_lon, double &min_lat, double &max_lat, QRectF *pbox = nullptr );

signals:
    void FiltersChanged();
    void FilterCompositionChanged( std::string filter_name );


public slots:
	void HandleNewFilter();
    void HandleRenameFilter();
	void HandleDeleteFilter();
	void HandleSaveFilters();

	void HandleFiltersCurrentIndexChanged( int index );
    void HandleRegionsCurrentIndexChanged( int index );
    void HandleAreasSelectionChanged();
	void HandleClearWhere();
	void HandleClearWhen();

    void HandleShowAllAreas();
    void HandleRegionSettings();

    void HandleNewArea();
    void HandleAddKML();
    void HandleAddMask();
    void HandleRenameArea();
    void HandleDeleteArea();
    void HandleAreaChecked(QListWidgetItem*area_item);
	void EnableRectangularSelection( bool enable );

	void HandleUseTimeToggled( bool toggled );
    void HandleStartDateTimeChanged(const QDateTime &start_datetime);
    void HandleStopDateTimeChanged (const QDateTime &stop_datetime);

	void HandleUseCyclePass( bool toggled );
	void HandleStartCycleChanged();
    void HandleStopCycleChanged();
    void HandleStartPassChanged();
    void HandleStopPassChanged();

    void HandleRelativeStartTimeChanged();
    void HandleRelativeStopTimeChanged();
    void HandleRelativeReferenceTimeChanged(const QDateTime &ref_datetime);

	void HandleWorkspaceChanged();

    void HandleCurrentLayerSelectionChanged( QRectF box = QRectF() );
	void HandleRelativeTimesBoxChecked( bool checked );
    void HandleCurrentDateTimeBoxChecked( bool checked );

};







#endif // GUI_CONTROL_PANELS_DATASET_FILTER_CONTROLS_H
