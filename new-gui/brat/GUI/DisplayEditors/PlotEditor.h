#ifndef GUI_DISPLAY_EDITORS_PLOT_EDITOR_H
#define GUI_DISPLAY_EDITORS_PLOT_EDITOR_H


#include "AbstractDisplayEditor.h"
#include "DataModels/PlotData/MapColor.h"


class CPlot;
class CZFXYPlot;
class CXYPlotDataCollection;

class CBrat2DPlotView;
class CBrat3DPlotView;

struct CViewControlsPanelGeneralPlots;
struct CPlotControlsPanelCurveOptions;
struct CPlotControlsPanelAxisOptions;




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
		eXYY,
		eXYZ,
		eHistogram,

		EPlotTypes_size
	};


	static std::string PlotTypeName( EPlotType t )
	{
		static const std::string names[ EPlotTypes_size ] =
		{
			"XY", 
			"XYY",
			"XYZ",
			"Histogram"
		};

		assert__( t < EPlotTypes_size );

		return names[ t ];
	}


private:

	// instance data

	CBrat2DPlotView *mPlot2DView = nullptr;
	CBrat3DPlotView *mPlot3DView = nullptr;

	CPlotControlsPanelCurveOptions *mTabCurveOptions = nullptr;	
	CPlotControlsPanelAxisOptions *mTabAxisOptions = nullptr;	

	EPlotType mPlotType = EPlotTypes_size;		//start with invalid value


	//...domain data

	CXYPlotDataCollection *mDataArrayXY = nullptr;
	CXYPlotProperties *mPropertiesXY = nullptr;

	CObArray *mDataArrayZFXY_2D = nullptr;
	CZFXYPlotProperties *mPropertiesZFXY_2D = nullptr;
	CObArray *mDataArrayZFXY_3D = nullptr;
	CZFXYPlotProperties *mPropertiesZFXY_3D = nullptr;



	// construction /destruction

private:
    void CreateWidgets();
    void Wire();

public:
    CPlotEditor( CModel *model, const COperation *op, const std::string &display_name = "", QWidget *parent = nullptr );
    CPlotEditor( const CDisplayFilesProcessor *proc, CPlot* plot, QWidget *parent = nullptr );
    CPlotEditor( const CDisplayFilesProcessor *proc, CZFXYPlot* plot, QWidget *parent = nullptr );

	virtual ~CPlotEditor();


	// remaining operations

protected:
	
	bool Refresh( EPlotType type );

	CViewControlsPanelGeneralPlots* TabGeneral();
	void UpdatePlotType( EPlotType type, bool select );

	void Reset2DProperties( const CXYPlotProperties *props, CPlot *plot );
	void Reset2DProperties( const CZFXYPlotProperties *props, CZFXYPlot *zfxyplot );



	///////////////////////////////////////////////////////////
	// Virtual interface implementations
	///////////////////////////////////////////////////////////

	virtual bool IsMapEditor() const { return false; }

	virtual bool ResetViews( bool enable_2d, bool enable_3d ) override;

	virtual void Show2D( bool checked ) override;
	virtual void Show3D( bool checked ) override;

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

	void HandlePlotTypeChanged( int );

	//axis handlers ///////////////////////////////////////////

	void HandleLogarithmicScaleX( bool log );
	void HandleLogarithmicScaleY( bool log );
	void HandleLogarithmicScaleZ( bool log );

	//curve handlers //////////////////////////////////////////

	//...select field
	void HandleCurrentFieldChanged( int index );

	//...spectrogram //////////////////////////////////////////

	void HandleShowContourChecked( bool checked );
	void HandleShowSolidColorChecked( bool checked );

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
};





#endif			// GUI_DISPLAY_EDITORS_PLOT_EDITOR_H
