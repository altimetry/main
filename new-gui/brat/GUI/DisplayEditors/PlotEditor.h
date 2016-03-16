#ifndef GUI_DISPLAY_EDITORS_PLOT_EDITOR_H
#define GUI_DISPLAY_EDITORS_PLOT_EDITOR_H


#include "AbstractDisplayEditor.h"


class CPlot;
class CZFXYPlot;

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
		eXYZ3D,
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
			"XYZ3D",
			"Histogram"
		};

		assert__( t < EPlotTypes_size );

		return names[ t ];
	}


private:

	// instance data

	CBrat2DPlotView *mPlot2DView = nullptr;
	CBrat3DPlotView *mPlot3DView = nullptr;

    QComboBox *mSelectPlotCombo = nullptr;
    QComboBox *mvarX       = nullptr;
    QComboBox *mvarY       = nullptr;
    QComboBox *mvarY2      = nullptr;
    QComboBox *mvarZ       = nullptr;
	CPlotControlsPanelCurveOptions *mTabCurveOptions = nullptr;	
	CPlotControlsPanelAxisOptions *mTabAxisOptions = nullptr;	


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
	virtual void ResetViews() override;

	virtual void Show2D( bool checked ) override;
	virtual void Show3D( bool checked ) override;

	virtual bool Refresh() override;
	virtual void NewButtonClicked() override;

	virtual void OperationChanged( int index ) override;

	virtual void OneClick() override;

protected slots:
	void HandleLogarithmicScaleX( bool log );
	void HandleLogarithmicScaleY( bool log );
	void HandleLogarithmicScaleZ( bool log );

    void HandleLineOptionsChecked( bool checked, int index );
    //void HandlePointOptionsChecked( bool checked, int index );
    void HandleCurveLineColorSelected( QColor new_color, int index );
    void HandleCurvePointColorSelected( QColor new_color, int index );
    void HandleOpacityChanged(int new_value, int index);
    void HandleWidthChanged(int new_value, int index);

    void HandleStipplePatternChanged(int pattern, int index);

    void HandleGlyphPatternChanged(int pattern, int index);

    void HandleGlyphSizeChanged(int new_value, int index);

    void HandleFillGlyphInteriorChanged(bool checked, int index);

    //
    //void OnUpdateCurveOptions( int index );



signals:

   void UpdatedCurrPlot(const CDisplayFilesProcessor*);
};





#endif			// GUI_DISPLAY_EDITORS_PLOT_EDITOR_H
