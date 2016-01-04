#ifndef GUI_DISPLAY_EDITOR_H
#define GUI_DISPLAY_EDITOR_H


#include <QMainWindow>

class CTabbedDock;
class CControlsPanel;
class CMapWidget;
class CGlobeWidget;
class C2DPlotWidget;
class C3DPlotWidget;
class CTextWidget;

enum EActionTag : int;


/////////////////////////////////////////////////////////////////
//	TODO: move to proper place after using real projections
/////////////////////////////////////////////////////////////////

QActionGroup* ProjectionsActions( QWidget *parent );




/////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////

class CAbstractDisplayEditor : public QMainWindow
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

	QToolBar *mToolBar = nullptr;
	std::vector< QWidget > mGraphics;

	CTabbedDock *mWorkingDock = nullptr;
	QSplitter *mMainSplitter = nullptr;
	QSplitter *mGraphicsSplitter = nullptr;

protected:
	QAction *mAction2D = nullptr;
	QAction *mAction3D = nullptr;
	QAction *mLogAction = nullptr;

	CTextWidget *mLogText = nullptr;

private:
	void CreateWorkingDock( CControlsPanel *controls );
	void CreateGraphicsBar();
	void CreateStatusBar();

public:
    CAbstractDisplayEditor( CControlsPanel *controls, QWidget *parent = nullptr );

	virtual ~CAbstractDisplayEditor();

protected:
	QSize sizeHint() const override
    {
        return QSize(72 * fontMetrics().width('x'),
                     25 * fontMetrics().lineSpacing());
    }

	bool AddView( QWidget *view, bool view3D );

	QAction* AddToolBarAction( QObject *parent, EActionTag tag );
	QAction* AddToolBarSeparator();
	QToolButton* AddMenuButton( EActionTag button_tag, const QList<QAction*> &actions );

	virtual void Handle2D( bool checked ) = 0;
	virtual void Handle3D( bool checked ) = 0;
	virtual void HandleLog( bool checked ) = 0;

protected slots:
	void Handle2D_slot( bool checked );
	void Handle3D_slot( bool checked );
	void HandleLog_slot( bool checked );
};



/////////////////////////////////////////////////////////////////
//						Map Editor
/////////////////////////////////////////////////////////////////

class CMapEditor : public CAbstractDisplayEditor
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif


	using base_t = CAbstractDisplayEditor;

	CMapWidget *mMap = nullptr;
	CGlobeWidget *mGlobe = nullptr;

	QAction *mActionMeasureDistance = nullptr;
	QAction *mActionMeasureArea = nullptr;
	QAction *mActionStatisticsMean = nullptr;
	QAction *mActionStatisticsStDev = nullptr;
	QAction *mActionStatisticsLinearRegression = nullptr;
	QToolButton *mToolProjection = nullptr;

	QActionGroup *mProjectionsGroup;

	void CreateMapActions();
	void WireMapActions();
public:
    CMapEditor( QWidget *parent = nullptr );

	virtual ~CMapEditor();

protected:
	virtual void Handle2D( bool checked ) override;
	virtual void Handle3D( bool checked ) override;
	virtual void HandleLog( bool checked ) override;

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

class CPlotEditor : public CAbstractDisplayEditor
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	using base_t = CAbstractDisplayEditor;

	C2DPlotWidget *mPlot2D = nullptr;
	C3DPlotWidget *mPlot3D = nullptr;

public:
    CPlotEditor( QWidget *parent = nullptr );

	virtual ~CPlotEditor();

protected:
	virtual void Handle2D( bool checked ) override;
	virtual void Handle3D( bool checked ) override;
	virtual void HandleLog( bool checked ) override;
};





#endif			// GUI_DISPLAY_EDITOR_H
