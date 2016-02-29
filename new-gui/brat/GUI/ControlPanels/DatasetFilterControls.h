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

	using base_t = CDesktopControlsPanel;

	//instance data

    QComboBox *mSavedRegionsCombo = nullptr;
    QComboBox *mOpenFilterCombo = nullptr;

    QLineEdit *mMaxLat = nullptr;
    QLineEdit *mMaxLon = nullptr;
    QLineEdit *mMinLat = nullptr;
    QLineEdit *mMinLon = nullptr;


    CBratMapView *mMap = nullptr;

	//...domain data

	CWorkspaceDataset *mWks = nullptr;


	//construction / destruction

	void Wire();
public:
	explicit CDatasetFilterControls( CDesktopManagerBase *manager, QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~CDatasetFilterControls()
	{}	

	// access 


	// operations

public slots:
	void WorkspaceChanged( CWorkspaceDataset *wksd );
	void DatasetChanged( CDataset *dataset );
	void FileChanged( const QString &path );

	void CurrentLayerSelectionChanged();
};







#endif // GUI_CONTROL_PANELS_DATASET_FILTER_CONTROLS_H
