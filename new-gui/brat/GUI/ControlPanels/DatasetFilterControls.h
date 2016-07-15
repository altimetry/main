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

class CDatasetFilterControls : public CDesktopControlsPanel
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

    QToolButton *mShowAllAreas = nullptr;
    QToolButton *mRegionSettings = nullptr;

    QToolButton *mNewArea = nullptr;
    QToolButton *mAddKML = nullptr;
    QToolButton *mAddMask = nullptr;
    QToolButton *mRenameArea = nullptr;
    QToolButton *mDeleteArea = nullptr;

    QLineEdit *mMaxLatEdit = nullptr;
    QLineEdit *mMaxLonEdit = nullptr;
    QLineEdit *mMinLatEdit = nullptr;
    QLineEdit *mMinLonEdit = nullptr;

    QWidget *mWhereBox = nullptr;

	//when

    QToolButton *mClearWhen = nullptr;
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

	//
	QLineEdit *mTotalRecordsSelectedEdit = nullptr;

    CMapWidget *mMap = nullptr;

	bool mAutoSatelliteTrack = true;


	//...domain data

	CBratFilters &mBratFilters;
	CBratAreas &mBratAreas;
	CBratRegions &mBratRegions;
	CBratFilter *mFilter = nullptr;

	CWorkspaceDataset *mWDataset = nullptr;
	CWorkspaceOperation *mWOperation = nullptr;
    CDataset *mDataset = nullptr;


	//construction / destruction

	void CreateWidgets();
	void Wire();
public:
	explicit CDatasetFilterControls( CModel &model, CDesktopManagerBase *manager, QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~CDatasetFilterControls()
	{}	


	// access 

	bool AutoSatelliteTrack() const
	{
		return mAutoSatelliteTrack;
	}


	void SetAutoSatelliteTrack( bool track )
	{
		mAutoSatelliteTrack = track;
		HandleDatasetChanged( mDataset );
	}


	// operations

protected:

	bool ReloadFilters();
	void FillFiltersCombo();
    void FillRegionsCombo();
    void FillAreasList();
    void ShowOnlyAreasInRegion(int region_index);
    void SaveAllAreas();
    void Relative2AbsoluteTimes();
    void ValidateAndStoreValue(QLineEdit *TextBox, int &ValueInFilter, int ParamDef, int min, int max);
    void updateDateWidgets();
    void updateCyclePassWidgets();
    void updateRelativeTimeWidgets();


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

    void HandleStartDateTimeChanged(const QDateTime &start_datetime);
    void HandleStopDateTimeChanged (const QDateTime &stop_datetime);

    void HandleStartCycleChanged();
    void HandleStopCycleChanged();
    void HandleStartPassChanged();
    void HandleStopPassChanged();

    void HandleRelativeStartTimeChanged();
    void HandleRelativeStopTimeChanged();
    void HandleRelativeReferenceTimeChanged(const QDateTime &ref_datetime);

	void HandleWorkspaceChanged();
    void HandleDatasetChanged( CDataset *dataset );

	void HandleCurrentLayerSelectionChanged();
	void HandleCurrentLayerSelectionChanged( QRectF box );
	void HandleRelativeTimesBoxChecked( bool checked );
    void HandleCurrentDateTimeBoxChecked( bool checked );

};







#endif // GUI_CONTROL_PANELS_DATASET_FILTER_CONTROLS_H
