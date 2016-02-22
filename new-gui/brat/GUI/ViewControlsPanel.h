#if !defined GUI_VIEW_CONTROLS_PANEL_H
#define GUI_VIEW_CONTROLS_PANEL_H


#include "ControlsPanel.h"


////////////////////////////////////////////////////////////////
//	 Color Button (Builds a button for selecting a color)
////////////////////////////////////////////////////////////////
class CColorButton : public QPushButton
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

    // types
    using base_t = QPushButton;

    // static members
    static const QString smStyleSheet_colorButton;
    static const QString smInitColor;

    // instance data members
    QColor      mCurrentColor = QColor();
    QWidget     *mColorDisplay = nullptr;


public:
    explicit CColorButton(QWidget *parent = nullptr);

    virtual ~CColorButton()
    {}

    // access


    // operations
    public slots:
    void SetColor();

};



////////////////////////////////////////////////////////////////
//	 Axis Tab (used to build the tab page with axis options)
////////////////////////////////////////////////////////////////
class CAxisTab : public QWidget
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

    // types

    using base_t = QWidget;

    // instance data members

    QCheckBox    *mLogScaleCheck = nullptr;

	//construction / destruction

	void Wire();

public:
    CAxisTab( QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

    virtual ~CAxisTab()
    {}

    //

signals:
	void LogarithmicScale( bool log );

protected slots:

};



/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//							View Common Tabs
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////
//			General Tab					
////////////////////////////////////////

struct CViewControlsPanelGeneral : public CControlsPanel
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

    // types

    using base_t = CControlsPanel;

    // instance data members

protected:
	const std::vector< DisplayFilesProcessor* >	&mCmdLineProcessors;

//		QComboBox *mSelectDataset   = nullptr;
//		QComboBox *mApplyFilter     = nullptr;
//		QComboBox *mSelectOperation = nullptr;

	QPushButton *mNewPlot = nullptr;

	QPushButton *mRunPlot = nullptr;
    QComboBox *mOpenAplot = nullptr;

	void Wire();
public:
    CViewControlsPanelGeneral( const std::vector< DisplayFilesProcessor* > &cmd_line_processors, QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~CViewControlsPanelGeneral()
	{}

signals:
	void NewButtonClicked();
	void CurrentDisplayIndexChanged( int index );
	void RunButtonClicked( int index );

public slots:
	void RunButtonClicked();
};



////////////////////////////////////////
//			Plots Tab					
////////////////////////////////////////


struct CViewControlsPanelPlots : public CControlsPanel
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

    // types

    using base_t = CControlsPanel;

    // instance data members

public:
	QTableWidget *mPlotInfoTable = nullptr;

public:
    CViewControlsPanelPlots( QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~CViewControlsPanelPlots()
	{}
};






/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//								Plot View Tabs
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////
//			Curve Edit/Data Tab					
////////////////////////////////////////


struct CPlotControlsPanelEdit : public CControlsPanel
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

    // types

    using base_t = CControlsPanel;

    // static members

    // instance data members

    QComboBox *mSelectplot = nullptr;
    QComboBox *mvarX       = nullptr;
    QComboBox *mvarY       = nullptr;
    QComboBox *mvarY2      = nullptr;
    QComboBox *mvarZ       = nullptr;

    QComboBox *mLinkToPlot = nullptr;

public:
    CPlotControlsPanelEdit( QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

    virtual ~CPlotControlsPanelEdit()
    {}
};




////////////////////////////////////////
//		Curve Options Tab					
////////////////////////////////////////

struct CPlotControlsPanelCurveOptions : public CControlsPanel
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

    // types

    using base_t = CControlsPanel;

    // static members

    // instance data members

    CColorButton *mLineColorButton  = nullptr;
    CColorButton *mPointColorButton = nullptr;

    QComboBox *mStipplePattern = nullptr;
    QComboBox *mPointGlyph = nullptr;

	QGroupBox *mLineOptions = nullptr;
	QGroupBox *mPointOptions = nullptr;

	//construction / destruction

	void Wire();
public:
    CPlotControlsPanelCurveOptions( QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

    virtual ~CPlotControlsPanelCurveOptions()
    {}

    // access


    // operations

signals:
	void LineOptionsChecked( bool checked );
	void PointOptionsChecked( bool checked );


protected slots:
	void HandleLineOptionsChecked( bool checked );
	void HandlePointOptionsChecked( bool checked );
};



////////////////////////////////////////
//		Axis Options Tab					
////////////////////////////////////////

struct CPlotControlsPanelAxisOptions : public CControlsPanel
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

    // types

    using base_t = CControlsPanel;

    // static members

    // instance data members

	CAxisTab *mX_axis = nullptr;
	CAxisTab *mY_axis = nullptr;
	CAxisTab *mZ_axis = nullptr;

    QTabWidget *mAxisOptionsTabs = nullptr;

	void Wire();
public:
    CPlotControlsPanelAxisOptions( QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

    virtual ~CPlotControlsPanelAxisOptions()
    {}

signals:
	void LogarithmicScaleX( bool log );
	void LogarithmicScaleY( bool log );
	void LogarithmicScaleZ( bool log );
};



/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//								Map View Tabs
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////
//			Map Edit Tab					
////////////////////////////////////////


struct CMapControlsPanelEdit : public CControlsPanel
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

    // types

    using base_t = CControlsPanel;

    // instance data members

    QWidget *mAddData    = nullptr;
    QComboBox *mDataLayer  = nullptr;

public:
    CMapControlsPanelEdit( QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~CMapControlsPanelEdit()
	{}
};



////////////////////////////////////////
//		Map Options Tab
////////////////////////////////////////


struct CMapControlsPanelOptions : public CControlsPanel
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

    // types

    using base_t = CControlsPanel;

    // instance data members

    QTabWidget *ViewOptionsTabs = nullptr;

public:
    CMapControlsPanelOptions( QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~CMapControlsPanelOptions()
	{}
};



#endif	//GUI_VIEW_CONTROLS_PANEL_H
