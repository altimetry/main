#if !defined GUI_CONTROL_PANELS_VIEWS_VIEW_CONTROL_PANELS_H
#define GUI_CONTROL_PANELS_VIEWS_VIEW_CONTROL_PANELS_H


#include "ViewControlPanelsCommon.h"

class CDisplay;


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//							Specialized Widgets
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
//	 Color Button (Builds a button for selecting a color)
////////////////////////////////////////////////////////////////

class CColorButton : public QToolButton
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
    using base_t = QToolButton;

    // static members
    static const QString smStyleSheet_colorButton;
    static const QString smInitColor;

    // instance data members
    QColor mCurrentColor;
    QWidget *mColorDisplay = nullptr;


public:
    CColorButton( QWidget *parent = nullptr );

    virtual ~CColorButton()
    {}

    // access


    // operations

    QColor GetColor()
    {
        return mCurrentColor;
    }


signals:

	void ColorChanged();


public slots:

    void InputColor()
	{
		SetColor( QColorDialog::getColor( mCurrentColor, parentWidget() ) );
	}

    void SetColor( QColor color )
    {
		if ( !color.isValid() )
			return;

        mCurrentColor = color;
        mColorDisplay->setStyleSheet( smStyleSheet_colorButton + mCurrentColor.name() );
		emit ColorChanged();
    }
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

    Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

    // types

    using base_t = QWidget;

    // instance data members



	//construction / destruction

    //void Wire();

public:

    QCheckBox *mLogScaleCheck = nullptr;
    QLineEdit *mAxisLabel = nullptr;
    QLineEdit *mNbTicks  = nullptr;
    QLineEdit *mBase     = nullptr;
    QLineEdit *mNbDigits = nullptr;

    QLineEdit *mAxisMax = nullptr;
    QLineEdit *mAxisMin = nullptr;
    QPushButton *mReset = nullptr;

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
//								Plot View Tabs
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////
//		Curve Options Tab					
////////////////////////////////////////

struct CPlotControlsPanelCurveOptions : public CControlPanel
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	//types

    using base_t = CControlPanel;


    // static members

    // instance data members

public:

    QListWidget *mFieldsList = nullptr;

    QGroupBox *mLineOptions = nullptr;
    QGroupBox *mPointOptions = nullptr;
    QGroupBox *mSpectrogramOptions = nullptr;

    CColorButton *mLineColorButton  = nullptr;
    CColorButton *mPointColorButton = nullptr;

    QComboBox *mStipplePattern = nullptr;
    QComboBox *mPointGlyph = nullptr;

    QLineEdit *mLineOpacityValue = nullptr;
    QCheckBox * mFillPointCheck = nullptr;

    QLineEdit *mLineWidthValue = nullptr;
    QLineEdit * mPointSizeValue = nullptr;

	QCheckBox *mShowContour = nullptr;
	QCheckBox *mShowSolidColor = nullptr;


	//construction / destruction

public:
    CPlotControlsPanelCurveOptions( QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

    virtual ~CPlotControlsPanelCurveOptions()
    {}
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

    Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

    // types

    using base_t = CControlPanel;

    // static members

public:

    // instance data members

	CAxisTab *mX_axis = nullptr;
	CAxisTab *mY_axis = nullptr;
	CAxisTab *mZ_axis = nullptr;

    QTabWidget *mAxisOptionsTabs = nullptr;

    //void Wire();
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


struct CMapControlsPanelDataLayers : public CControlPanel
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

public:
    // instance data members

    QListWidget *mFieldsList = nullptr;
    QCheckBox *mShowSolidColorCheck = nullptr;


public:
    CMapControlsPanelDataLayers( QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~CMapControlsPanelDataLayers()
	{}
};



////////////////////////////////////////
//		Map Options Tab
////////////////////////////////////////


struct CMapControlsPanelView : public CControlPanel
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

    QTabWidget *ViewOptionsTabs = nullptr;

	void CreateWidgets();
public:
    CMapControlsPanelView( QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~CMapControlsPanelView()
	{}
};



#endif	//GUI_CONTROL_PANELS_VIEWS_VIEW_CONTROL_PANELS_H
