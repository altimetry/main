#ifndef GUI_DISPLAY_EDITORS_PLOT_EDITOR_H
#define GUI_DISPLAY_EDITORS_PLOT_EDITOR_H


#include "AbstractDisplayEditor.h"


class CPlot;
class CZFXYPlot;

class CBrat2DPlotView;
class CBrat3DPlotView;

struct CPlotControlsPanelEdit;
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


	// instance data

	CBrat2DPlotView *mPlot2DView = nullptr;
	CBrat3DPlotView *mPlot3DView = nullptr;

	CViewControlsPanelGeneral *mTabGeneral = nullptr;
	CPlotControlsPanelEdit *mTabData = nullptr;	
	CPlotControlsPanelCurveOptions *mTabCurveOptions = nullptr;	
	CPlotControlsPanelAxisOptions *mTabAxisOptions = nullptr;	
	CViewControlsPanelPlots *mTabPlots = nullptr;


	// construction /destruction

private:
    void CreateWidgets();
    void Wire();

public:
    CPlotEditor( CModel *model, COperation *operation, QWidget *parent = nullptr );
    CPlotEditor( const CDisplayFilesProcessor *proc, CPlot* plot, QWidget *parent = nullptr );
    CPlotEditor( const CDisplayFilesProcessor *proc, CZFXYPlot* plot, QWidget *parent = nullptr );

	virtual ~CPlotEditor();


	// remaining operations

protected:
	virtual void ResetViews() override;

	virtual void Show2D( bool checked ) override;
	virtual void Show3D( bool checked ) override;

	virtual void PlotChanged( int index ) override;
	virtual void NewButtonClicked() override;

	virtual void DatasetsIndexChanged( int index ) override;
	virtual void FiltersIndexChanged( int index ) override;

	virtual void OneClick() override;

protected slots:
	void HandleLogarithmicScaleX( bool log );
	void HandleLogarithmicScaleY( bool log );
	void HandleLogarithmicScaleZ( bool log );

	void HandleLineOptionsChecked( bool checked );
	void HandlePointOptionsChecked( bool checked );
};





#endif			// GUI_DISPLAY_EDITORS_PLOT_EDITOR_H
