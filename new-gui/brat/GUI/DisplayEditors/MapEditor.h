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
#ifndef GUI_DISPLAY_EDITORS_MAP_EDITOR_H
#define GUI_DISPLAY_EDITORS_MAP_EDITOR_H


#include "AbstractDisplayEditor.h"


class CGeoPlot;
class CWorldPlotData;
class CWorldPlotVelocityData;

class CBratMapView;
class CGlobeWidget;

struct CViewControlsPanelGeneralMaps;
struct CMapControlsPanelDataLayers;
struct CMapControlsPanelView;
class CMapWidget;


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
	CMapWidget *mMapView = nullptr;
	CGlobeWidget *mGlobeView = nullptr;
	bool mMapProcessing = false;
	bool mGlobeProcessing = false;
	bool mDisplaying2D = false;
	bool mDisplaying3D = false;
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
    QAction *mActionMapTips = nullptr;


	//...actions
	QToolButton *mToolProjection = nullptr;

	QActionGroup *mProjectionsGroup;

	//...tabs
	CMapControlsPanelDataLayers *mTabDataLayers = nullptr;
	CMapControlsPanelView *mTabView = nullptr;


	//...domain data: "current" pointers

	CWorldPlotData *mCurrentPlotData = nullptr;
	CWorldPlotVelocityData *mCurrentPlotVelocityData = nullptr;


	// construction /destruction

private:
	QActionGroup* CreateProjectionsActions();
	void ResetAndWireNewMap();
	void CreateWidgets();
	void WireMapActions();
	void Wire();

public:
    CMapEditor( CModel *model, const COperation *op, const std::string &display_name = "" );
    CMapEditor( CDisplayDataProcessor *proc, CGeoPlot* wplot );
    CMapEditor( CDisplayFilesProcessor *proc, CGeoPlot* wplot );

	virtual ~CMapEditor();

	// 

protected:

    virtual void closeEvent( QCloseEvent *event ) override;

	CViewControlsPanelGeneralMaps* TabGeneral();

	QgsVectorLayer* CreateCurrentDataLayer( bool add = true );

	void ResetMap();

	void KillGlobe();


	///////////////////////////////////////////////////////////
	// Virtual interface implementations
	///////////////////////////////////////////////////////////

	virtual bool IsMapEditor() const { return true; }

	virtual bool ResetViews( bool reset_2d, bool reset_3d, bool enable_2d, bool enable_3d ) override;

	virtual void Show2D( bool checked ) override;
	virtual void Show3D( bool checked ) override;
	virtual void Recenter() override;
	virtual void Export2Image( bool save_2d, bool save_3d, const std::string path2d, const std::string path3d, EImageExportType type ) override;

	virtual bool ChangeView() override;									//when display changes
	virtual bool UpdateCurrentPointers( int field_index ) override;		//when field changes
	virtual void SetPlotTitle() override;
	virtual void NewButtonClicked() override;
	virtual void RenameButtonClicked() override;
	virtual void DeleteButtonClicked() override;

	virtual void OperationChanged( int index ) override;

	virtual void OneClick() override;

	virtual bool Test() override;


	///////////////////////////////////////////////////////////
	// Handlers
	///////////////////////////////////////////////////////////

	//helper, not slot

	bool SetNumberOfContours( int field_index, std::pair< unsigned, unsigned > grid , unsigned contours );

protected slots:

	void HandleCurrentFieldChanged( int field_index );

	void HandleShowContourChecked( bool checked );
	void HandleContourColorSelected();
	void HandleNumberOfContoursChanged();
	void HandleContourPrecisionGridEditPressed();
	void HandleContourWidthChanged();
	void HandleShowSolidColorChecked( bool checked );
	void HandleColorTablesIndexChanged( int index );

	void HandleMagnitudeFactorEditEntered();

	void HandleProjection();
};





#endif			// GUI_DISPLAY_EDITORS_MAP_EDITOR_H
