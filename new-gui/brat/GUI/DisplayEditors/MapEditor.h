#ifndef GUI_DISPLAY_EDITORS_MAP_EDITOR_H
#define GUI_DISPLAY_EDITORS_MAP_EDITOR_H


#include "AbstractDisplayEditor.h"


class CWPlot;
class CBratMapView;
class CGlobeWidget;
class CWorldPlotData;

struct CViewControlsPanelGeneralMaps;
struct CMapControlsPanelDataLayers;
struct CMapControlsPanelView;




/////////////////////////////////////////////////////////////////
//	TODO: move to proper place after using real projections
/////////////////////////////////////////////////////////////////

QActionGroup* ProjectionsActions( QWidget *parent );





/////////////////////////////////////////////////////////////////
//						Map Editor
/////////////////////////////////////////////////////////////////



class CMapEditor : public CAbstractDisplayEditor
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


	using base_t = CAbstractDisplayEditor;


	// instance data

	//map && globe
	CBratMapView *mMapView = nullptr;
	CGlobeWidget *mGlobeView = nullptr;
	//
	//...map/globe managed widgets/actions
	QProgressBar *mProgressBar = nullptr;
	QCheckBox *mRenderSuppressionCBox = nullptr;
	QToolButton *mCoordinatesFormatButton = nullptr;
	QString mCoordinatesFormat;
	QLineEdit *mCoordsEdit = nullptr;

	QAction *mActionMeasure = nullptr;
	QAction *mActionMeasureArea = nullptr;
	QToolButton *mMeasureButton = nullptr;
	QAction *mActionDecorationGrid = nullptr;


	//...actions
	QAction *mActionStatisticsMean = nullptr;
	QAction *mActionStatisticsStDev = nullptr;
	QAction *mActionStatisticsLinearRegression = nullptr;
	QToolButton *mToolProjection = nullptr;

	QActionGroup *mProjectionsGroup;

	//...tabs
	CMapControlsPanelDataLayers *mTabDataLayers = nullptr;
	CMapControlsPanelView *mTabView = nullptr;


	//...domain data

	std::vector< CWorldPlotData* > *mDataArrayMap;

	CWorldPlotProperties *mPropertiesMap = nullptr;


	// construction /destruction

private:
	void CreateMapActions();
	void CreateAndWireNewMap();
	void CreateWidgets();
	void WireMapActions();
	void Wire();

public:
    CMapEditor( CModel *model, const COperation *op, const std::string &display_name = "" );
    CMapEditor( CDisplayFilesProcessor *proc, CWPlot* wplot );

	virtual ~CMapEditor();

	// 

protected:

    virtual void closeEvent( QCloseEvent *event ) override;

	CViewControlsPanelGeneralMaps* TabGeneral();

	void ResetProperties( const CWorldPlotProperties *props, CWPlot *wplot );

	void KillGlobe();

	///////////////////////////////////////////////////////////
	// Virtual interface implementations
	///////////////////////////////////////////////////////////

	virtual bool IsMapEditor() const { return true; }

	virtual bool ResetViews( bool reset_2d, bool reset_3d ) override;

	virtual void Show2D( bool checked ) override;
	virtual void Show3D( bool checked ) override;
	virtual void Recenter() override;

	virtual bool ViewChanged() override;
	virtual void NewButtonClicked() override;
	virtual void RenameButtonClicked() override;
	virtual void DeleteButtonClicked() override;

	virtual void OperationChanged( int index ) override;

	virtual void OneClick() override;

	///////////////////////////////////////////////////////////
	// Handlers
	///////////////////////////////////////////////////////////

protected slots:

	void HandleStatisticsMean();
	void HandleStatisticsStDev();
	void HandleStatisticsLinearRegression();

	void HandleCurrentFieldChanged( int field_index );

	void HandleShowSolidColor( bool checked );

	void HandleProjection();
};





#endif			// GUI_DISPLAY_EDITORS_MAP_EDITOR_H
