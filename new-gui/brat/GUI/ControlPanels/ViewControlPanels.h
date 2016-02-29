#if !defined GUI_CONTROL_PANELS_VIEW_CONTROL_PANELS_H
#define GUI_CONTROL_PANELS_VIEW_CONTROL_PANELS_H


#include "ControlPanel.h"

class CDisplay;


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
    QColor mCurrentColor = QColor();
    QWidget *mColorDisplay = nullptr;


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

struct CViewControlsPanelGeneral : public CControlPanel
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

    using base_t = CControlPanel;

    // instance data members

protected:
	const CDisplayFilesProcessor *mCmdLineProcessor = nullptr;
	std::vector< const CDisplay* > mDisplays;

	QPushButton *mNewPlot = nullptr;

	QPushButton *mRunPlot = nullptr;
    QComboBox *mOpenAplot = nullptr;

	void CreateWidgets();
	void Wire();
public:
    CViewControlsPanelGeneral( const CDisplayFilesProcessor *cmd_line_processor, QWidget *parent = nullptr, Qt::WindowFlags f = 0 );
    CViewControlsPanelGeneral( const std::vector< const CDisplay* > &displays, QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

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


struct CViewControlsPanelPlots : public CControlPanel
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

    using base_t = CControlPanel;

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


struct CPlotControlsPanelEdit : public CControlPanel
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

    using base_t = CControlPanel;

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

struct CPlotControlsPanelCurveOptions : public CControlPanel
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

    using base_t = CControlPanel;

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

struct CPlotControlsPanelAxisOptions : public CControlPanel
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

    using base_t = CControlPanel;

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


struct CMapControlsPanelEdit : public CControlPanel
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

    using base_t = CControlPanel;

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


struct CMapControlsPanelOptions : public CControlPanel
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

    using base_t = CControlPanel;

    // instance data members

    QTabWidget *ViewOptionsTabs = nullptr;

public:
    CMapControlsPanelOptions( QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~CMapControlsPanelOptions()
	{}
};



#endif	//GUI_CONTROL_PANELS_VIEW_CONTROL_PANELS_H
