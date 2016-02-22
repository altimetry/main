#ifndef GUI_DISPLAY_EDITOR_H
#define GUI_DISPLAY_EDITOR_H


#include <QMainWindow>

class CWorkspaceTabbedDock;
class CGlobeWidget;
class CTextWidget;

struct CViewControlsPanelGeneral;
struct CViewControlsPanelPlots;

struct CMapControlsPanelEdit;
struct CMapControlsPanelOptions;
struct CPlotControlsPanelEdit;	
struct CPlotControlsPanelCurveOptions;
struct CPlotControlsPanelAxisOptions;

class CWorkspaceDisplay;


enum EActionTag : int;



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

	Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	using base_t = QMainWindow;


	// instance data

	CWorkspaceTabbedDock *mWorkingDock = nullptr;
	QSplitter *mMainSplitter = nullptr;
	QSplitter *mGraphicsSplitter = nullptr;

	CViewControlsPanelGeneral *mTabGeneral = nullptr;

protected:
	QToolBar *mToolBar = nullptr;
    QStatusBar *mStatusBar = nullptr;

	QAction *mAction2D = nullptr;
	QAction *mAction3D = nullptr;
	QAction *mLogAction = nullptr;

	CTextWidget *mLogText = nullptr;

protected:

	// domain data

	CWorkspaceDisplay						*mWksd = nullptr;
	std::vector< DisplayFilesProcessor* >	mCmdLineProcessors;

	//construction / destruction

private:
	void CreateWorkingDock();
	void CreateGraphicsBar();
	void CreateStatusBar();
	void CreateWidgets();
	void Wire();

protected:
	CAbstractViewEditor( bool map_editor, DisplayFilesProcessor *proc, QWidget *parent = nullptr );
    CAbstractViewEditor( bool map_editor, CWorkspaceDisplay *wksd, QWidget *parent = nullptr );

public:
	virtual ~CAbstractViewEditor()
	{}

	//remaining member functions

protected:
	QSize sizeHint() const override
    {
        return QSize(72 * fontMetrics().width('x'),
                     25 * fontMetrics().lineSpacing());
    }

	QWidget* AddTab( QWidget *tab, const QString title = "" );

	bool AddView( QWidget *view, bool view3D );
	bool RemoveView( QWidget *view, bool view3D );

	QAction* AddToolBarAction( QObject *parent, EActionTag tag );
	QAction* AddToolBarSeparator();
	QToolButton* AddMenuButton( EActionTag button_tag, const QList<QAction*> &actions );

	virtual void Handle2D( bool checked ) = 0;
	virtual void Handle3D( bool checked ) = 0;
	virtual void HandleLog( bool checked ) = 0;

	virtual void HandlePlotChanged( int index ) = 0;
	virtual void HandleNewButtonClicked() = 0;

	// domain

	void FilterDisplays( bool with_maps );


protected slots:
	void NewButtonClicked()
	{
		HandleNewButtonClicked();
	}

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

	CBratMapView *mMap = nullptr;
	CGlobeWidget *mGlobe = nullptr;

	QAction *mActionStatisticsMean = nullptr;
	QAction *mActionStatisticsStDev = nullptr;
	QAction *mActionStatisticsLinearRegression = nullptr;
	QToolButton *mToolProjection = nullptr;

	QActionGroup *mProjectionsGroup;

	CMapControlsPanelEdit *mTabData = nullptr;
	CMapControlsPanelOptions *mTabView = nullptr;
	CViewControlsPanelPlots *mTabPlots = nullptr;

	// construction /destruction

private:
	void CreateMapActions();
	void CreateWidgets();
	void Wire();

public:
    CMapEditor( DisplayFilesProcessor *proc, CWPlot* wplot, QWidget *parent = nullptr );
    CMapEditor( CWorkspaceDisplay *wksd, QWidget *parent = nullptr );

	virtual ~CMapEditor();

	// 

protected:
	virtual void Handle2D( bool checked ) override;
	virtual void Handle3D( bool checked ) override;
	virtual void HandleLog( bool checked ) override;

	virtual void HandlePlotChanged( int index ) override;
	virtual void HandleNewButtonClicked() override;

protected slots:

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

	CBrat2DPlotView *mPlot2D = nullptr;
	CBrat3DPlotView *mPlot3D = nullptr;

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
    CPlotEditor( CWorkspaceDisplay *wksd, const DisplayFilesProcessor *proc, CPlot* plot, QWidget *parent = nullptr );
    CPlotEditor( CWorkspaceDisplay *wksd, const DisplayFilesProcessor *proc, CZFXYPlot* plot, QWidget *parent = nullptr );

	virtual ~CPlotEditor();

	//

protected:
	virtual void Handle2D( bool checked ) override;
	virtual void Handle3D( bool checked ) override;
	virtual void HandleLog( bool checked ) override;

	virtual void HandlePlotChanged( int index ) override;
	virtual void HandleNewButtonClicked() override;

protected slots:
	void HandleLogarithmicScaleX( bool log );
	void HandleLogarithmicScaleY( bool log );
	void HandleLogarithmicScaleZ( bool log );

	void HandleLineOptionsChecked( bool checked );
	void HandlePointOptionsChecked( bool checked );
};





#endif			// GUI_DISPLAY_EDITOR_H
