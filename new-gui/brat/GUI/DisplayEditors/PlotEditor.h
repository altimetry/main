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
#ifndef GUI_DISPLAY_EDITORS_PLOT_EDITOR_H
#define GUI_DISPLAY_EDITORS_PLOT_EDITOR_H

#include "DataModels/PlotData/ZFXYPlotData.h"
#include "DataModels/PlotData/XYPlotData.h"
#include "DataModels/PlotData/MapColor.h"

#include "AbstractDisplayEditor.h"

class C2DPlotWidget;
class C3DPlotWidget;

struct CViewControlsPanelGeneralPlots;
struct CPlotControlsPanelCurveOptions;
struct CPlotControlsPanelAxisOptions;
struct CPlotControlsPanelAnimateOptions;




/////////////////////////////////////////////////////////////////
//						Plot Editor
/////////////////////////////////////////////////////////////////

class CPlotEditor : public CAbstractDisplayEditor
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

public:

	enum EPlotType
	{
		eXY, 
		eXYZ,
		eHistogram,

		EPlotTypes_size
	};


	static std::string PlotTypeName( EPlotType t )
	{
		static const std::string names[ EPlotTypes_size ] =
		{
			"XY", 
			"XYZ",
			"Histogram"
		};

		assert__( t < EPlotTypes_size );

		return names[ t ];
	}


private:

	// instance data

	C2DPlotWidget *mPlot2DView = nullptr;
	C3DPlotWidget *mPlot3DView = nullptr;

	CPlotControlsPanelCurveOptions *mTabCurveOptions = nullptr;	
	CPlotControlsPanelAxisOptions *mTabAxisOptions = nullptr;
	CPlotControlsPanelAnimateOptions *mTabAnimateOptions = nullptr;

	EPlotType mPlotType = EPlotTypes_size;		//start with invalid value


	//...domain data

	CXYPlotData *mCurrentPlotDataYFX = nullptr;
	bool mMultiFrame = false;					//	bool mPanelMultiFrame;	2 variables in v3: one in the window, the other in the panel

	CZFXYPlotData *mCurrentPlotDataZFXY = nullptr;


	// construction /destruction

private:
    void CreateWidgets();
    void Wire();

public:
    CPlotEditor( CModel *model, const COperation *op, const std::string &display_name = "" );
    CPlotEditor( const CDisplayDataProcessor *proc, CYFXPlot* plot );
    CPlotEditor( const CDisplayDataProcessor *proc, CZFXYPlot* plot );
    CPlotEditor( const CDisplayFilesProcessor *proc, CYFXPlot* plot );
    CPlotEditor( const CDisplayFilesProcessor *proc, CZFXYPlot* plot );

	virtual ~CPlotEditor();


protected:
	
	// remaining operations

protected:
	
	CViewControlsPanelGeneralPlots* TabGeneral();
	void UpdatePlotType( EPlotType type, bool select );

	void Recreate2DPlots();
	void Recreate3DPlots( bool build2d, bool build3d );

	double CreateHistogram();

	void SetAnimationDescr( int frame );		//from v3 CXYPlotPanel::SetAnimationDescr


	///////////////////////////////////////////////////////////
	// Virtual interface implementations
	///////////////////////////////////////////////////////////

	virtual bool IsMapEditor() const { return false; }

	virtual bool ResetViews( bool reset_2d, bool reset_3d, bool enable_2d, bool enable_3d ) override;

	virtual void Show2D( bool checked ) override;
	virtual void Show3D( bool checked ) override;
	virtual void Recenter() override;
	virtual void Export2Image( bool save_2d, bool save_3d, const std::string path2d, const std::string path3d, EImageExportType type ) override;

	virtual bool ChangeView() override;					 				//when display changes
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

	//not slots
    bool HandleAxisLabelChanged( QLineEdit *label_edit, std::string &xtitle, std::string &ytitle, std::string &ztitle, std::string &new_title );

protected slots:

	void HandlePlotTypeChanged( int );

	//axis handlers ///////////////////////////////////////////

    //...labels
    void HandleXAxisLabelChanged();
    void HandleYAxisLabelChanged();
    void HandleZAxisLabelChanged();

    //...ticks
    void HandleXNbTicksChanged();
    void HandleYNbTicksChanged();
    void HandleZNbTicksChanged();

    //...digits
    void HandleXAxisNbDigitsChanged();
    void HandleYAxisNbDigitsChanged();
    void HandleZAxisNbDigitsChanged();

	//...scale
	void HandleX2DScaleSpinChanged( double );
	void HandleY2DScaleSpinChanged( double );
	void HandleX3DScaleSpinChanged( double );
	void HandleY3DScaleSpinChanged( double );
	void HandleZ3DScaleSpinChanged( double );

	void Handle2DScaleChanged( int iaxis, double factor, QString range );
	void Handle3DScaleChanged( double xVal, double yVal, double zVal );


	//...log
	//void HandleLogarithmicScaleX( bool log );
	//void HandleLogarithmicScaleY( bool log );
	void HandleLogarithmicScaleZ( bool log );


	//curve handlers //////////////////////////////////////////

	//...select field
	void HandleCurrentFieldChanged( int index );

	//...spectrogram //////////////////////////////////////////

	void HandleShowContourChecked( bool checked );
	void HandleShowSolidColorChecked( bool checked );
	void HandleColorTablesIndexChanged( int index );
	void HandleNumberOfContoursChanged();


	//...histogram //////////////////////////////////////////

	void HandleNumberOfBinsChanged();


	//...line/points //////////////////////////////////////////

	//...enable/disable line/points
    void HandleLineOptionsChecked( bool checked );
    void HandlePointOptionsChecked( bool checked );

	//...color
	void HandleCurveLineColorSelected();
	void HandleCurvePointColorSelected();

	//...pattern/glyph
	void HandleStipplePatternChanged( int pattern );
	void HandleGlyphPatternChanged( int pattern );

	//...opacity/fill point
	void HandleCurveLineOpacityEntered();
	void HandleFillGlyphInterior( bool checked );

	//...width/size
	void HandleCurveLineWidthEntered();
	void HandleCurveGlyphWidthEntered();


	//animation handlers //////////////////////////////////////////

	void HandleAnimateButtonToggled( bool toggled );
	void HandleStopAnimationButton();
	void HandleAnimateLoop( bool );
	void HandleAnimateBackwardToggled( bool );
	void HandleAnimateForwardToggled( bool );
	void AnimationStopped();
	void HandleFrameIndexSpinChanged( int i );
};





#endif			// GUI_DISPLAY_EDITORS_PLOT_EDITOR_H
