#ifndef GUI_CONTROL_PANELS_DATASET_FILTER_CONTROLS_H
#define GUI_CONTROL_PANELS_DATASET_FILTER_CONTROLS_H

// File Index:
//
//	- Filter Control Panel
//


#include "ControlPanel.h"

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
    QComboBox *mSavedRegionsCombo = nullptr;
    QComboBox *mFiltersCombo = nullptr;

    QLineEdit *mMaxLatEdit = nullptr;
    QLineEdit *mMaxLonEdit = nullptr;
    QLineEdit *mMinLatEdit = nullptr;
    QLineEdit *mMinLonEdit = nullptr;

	QLayout *mWhereBox = nullptr;

	//when

    QToolButton *mClearWhen = nullptr;
    QDateTimeEdit *mStartTimeEdit = nullptr;
    QDateTimeEdit *mStopTimeEdit = nullptr;

    QLineEdit *mStartCycleEdit = nullptr;
    QLineEdit *mStopCycleEdit = nullptr;
    QLineEdit *mStartPassEdit = nullptr;
    QLineEdit *mStopPassEdit = nullptr;

	QLayout *mWhenBox = nullptr;

	//

	QLineEdit *mTotalRecordsSelectedEdit = nullptr;


    CBratMapView *mMap = nullptr;

	bool mAutoSatelliteTrack = true;


	//...domain data

	CBratFilters &mBratFilters;
	CBratFilter *mFilter = nullptr;

	CWorkspaceDataset *mWks = nullptr;
    CDataset *mDataset = nullptr;


	//construction / destruction

	void CreateWidgets();
	void Wire();
public:
	explicit CDatasetFilterControls( CDesktopManagerBase *manager, CBratFilters &brat_filters, QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

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

public slots:
	void HandleNewFilter();
	void HandleRenameFilter();
	void HandleDeleteFilter();
	void HandleSaveFilters();

	void HandleFiltersCurrentIndexChanged( int index );
	void HandleClearWhere();
	void HandleClearWhen();


	void HandleWorkspaceChanged( CWorkspaceDataset *wksd );
    void HandleDatasetChanged( CDataset *dataset );

	void HandleCurrentLayerSelectionChanged();
};







#endif // GUI_CONTROL_PANELS_DATASET_FILTER_CONTROLS_H
