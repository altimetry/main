#ifndef GUI_DISPLAY_EDITOR_H
#define GUI_DISPLAY_EDITOR_H


#include <QMainWindow>

class CTabbedDock;
class CControlsPanel;
class CBratMapWidget;
class CGlobeWidget;
class CBrat2DPlotWidget;
class CBrat3DPlotWidget;
class CTextWidget;

struct CViewControlsPanelGeneral;
struct CViewControlsPanelPlots;

struct CMapControlsPanelEdit;
struct CMapControlsPanelOptions;
struct CPlotControlsPanelEdit;	
struct CPlotControlsPanelCurveOptions;
struct CPlotControlsPanelAxisOptions;


enum EActionTag : int;


class CWorkspaceDisplay;
class CmdLineProcessor;
class CWPlot;
class CPlot;
class CZFXYPlot;


/////////////////////////////////////////////////////////////////
//	TODO: move to proper place after using real projections
/////////////////////////////////////////////////////////////////

QActionGroup* ProjectionsActions( QWidget *parent );




/////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////

class CAbstractViewEditor : public QMainWindow
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	using base_t = QMainWindow;


	// instance data

	QToolBar *mToolBar = nullptr;
	std::vector< QWidget > mGraphics;

	CTabbedDock *mWorkingDock = nullptr;
	QSplitter *mMainSplitter = nullptr;
	QSplitter *mGraphicsSplitter = nullptr;

protected:
	// domain data

	CWorkspaceDisplay					*mWksd = nullptr;
	std::vector< CmdLineProcessor* >	mCmdLineProcessors;

protected:
	QAction *mAction2D = nullptr;
	QAction *mAction3D = nullptr;
	QAction *mLogAction = nullptr;

	CTextWidget *mLogText = nullptr;

private:
	void CreateWorkingDock();
	void CreateGraphicsBar();
	void CreateStatusBar();

public:
    CAbstractViewEditor( CWorkspaceDisplay *wksd, QWidget *parent = nullptr );

	virtual ~CAbstractViewEditor()
	{}

protected:
	QSize sizeHint() const override
    {
        return QSize(72 * fontMetrics().width('x'),
                     25 * fontMetrics().lineSpacing());
    }

	QWidget* AddTab( QWidget *tab, const QString title = "" );

	bool AddView( QWidget *view, bool view3D );

	QAction* AddToolBarAction( QObject *parent, EActionTag tag );
	QAction* AddToolBarSeparator();
	QToolButton* AddMenuButton( EActionTag button_tag, const QList<QAction*> &actions );

	virtual void Handle2D( bool checked ) = 0;
	virtual void Handle3D( bool checked ) = 0;
	virtual void HandleLog( bool checked ) = 0;

	virtual void HandlePlotChanged( int index ) = 0;

	// domain

	void FilterDisplays( bool with_maps );


protected slots:
	void Handle2D_slot( bool checked );
	void Handle3D_slot( bool checked );
	void HandleLog_slot( bool checked );

	void PlotChanged( int index )
	{
		HandlePlotChanged( index );
	}
};



/////////////////////////////////////////////////////////////////
//						Map Editor
/////////////////////////////////////////////////////////////////



class CMapEditor : public CAbstractViewEditor
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif


	using base_t = CAbstractViewEditor;

	CBratMapWidget *mMap = nullptr;
	CGlobeWidget *mGlobe = nullptr;

	QAction *mActionMeasureDistance = nullptr;
	QAction *mActionMeasureArea = nullptr;
	QAction *mActionStatisticsMean = nullptr;
	QAction *mActionStatisticsStDev = nullptr;
	QAction *mActionStatisticsLinearRegression = nullptr;
	QToolButton *mToolProjection = nullptr;

	QActionGroup *mProjectionsGroup;

	CViewControlsPanelGeneral *mTabGeneral = nullptr;
	CMapControlsPanelEdit *mTabData = nullptr;
	CMapControlsPanelOptions *mTabView = nullptr;
	CViewControlsPanelPlots *mTabPlots = nullptr;

	// construction /destruction

	void CreateMapActions();
	void WireMapActions();
public:
    CMapEditor( const CmdLineProcessor *proc, CWPlot* wplot, QWidget *parent = nullptr );
    CMapEditor( CWorkspaceDisplay *wksd, QWidget *parent = nullptr );

	virtual ~CMapEditor();

	// 

protected:
	virtual void Handle2D( bool checked ) override;
	virtual void Handle3D( bool checked ) override;
	virtual void HandleLog( bool checked ) override;

	virtual void HandlePlotChanged( int index ) override;

protected slots:

	void HandleMeasureDistance();
	void HandleMeasureArea();

	void HandleStatisticsMean();
	void StatisticsStDev();
	void HandleStatisticsLinearRegression();

	void HandleProjection();
};




/////////////////////////////////////////////////////////////////
//						Plot Editor
/////////////////////////////////////////////////////////////////

class CPlotEditor : public CAbstractViewEditor
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	using base_t = CAbstractViewEditor;

	CBrat2DPlotWidget *mPlot2D = nullptr;
	CBrat3DPlotWidget *mPlot3D = nullptr;

	CViewControlsPanelGeneral *mTabGeneral = nullptr;
	CPlotControlsPanelEdit *mTabData = nullptr;	
	CPlotControlsPanelCurveOptions *mTabCurveOptions = nullptr;	
	CPlotControlsPanelAxisOptions *mTabAxisOptions = nullptr;	
	CViewControlsPanelPlots *mTabPlots = nullptr;

	// construction /destruction

    void Wire();
    void CreatePlotViews();
public:
    CPlotEditor( CWorkspaceDisplay *wksd, const CmdLineProcessor *proc, CPlot* plot, QWidget *parent = nullptr );
    CPlotEditor( CWorkspaceDisplay *wksd, const CmdLineProcessor *proc, CZFXYPlot* plot, QWidget *parent = nullptr );

	virtual ~CPlotEditor();

	//

protected:
	virtual void Handle2D( bool checked ) override;
	virtual void Handle3D( bool checked ) override;
	virtual void HandleLog( bool checked ) override;

	virtual void HandlePlotChanged( int index ) override;

protected slots:
	void HandleLogarithmicScaleX( bool log );
	void HandleLogarithmicScaleY( bool log );
	void HandleLogarithmicScaleZ( bool log );
};





#endif			// GUI_DISPLAY_EDITOR_H
