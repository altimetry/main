#if !defined GUI_CONTROL_PANELS_VIEWS_VIEW_CONTROL_PANELS_H
#define GUI_CONTROL_PANELS_VIEWS_VIEW_CONTROL_PANELS_H


#include "ViewControlPanelsCommon.h"

class CDisplay;
class CTextWidget;
class CBratLookupTable;


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//							Specialized Widgets
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

class CColorMapLabel : public QLabel
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

	using base_t = QLabel;

	//data

	const CBratLookupTable *mLut = nullptr;

public:
	CColorMapLabel( QWidget *parent = nullptr );

	virtual ~CColorMapLabel()
	{}

	void SetLUT( const CBratLookupTable *lut );

protected:

	virtual void resizeEvent( QResizeEvent * ) override;
};





class CColorMapWidget : public QWidget
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

	friend struct CPlotControlsPanelCurveOptions;
	friend struct CMapControlsPanelDataLayers;


	//data

	QCheckBox *mShowContour = nullptr;
	QCheckBox *mShowSolidColor = nullptr;
	CColorMapLabel *mColorMapLabel = nullptr;
	QComboBox *mColorTables = nullptr;

	CBratLookupTable *mLut = nullptr;

	//construction / destruction

public:
    CColorMapWidget( QWidget *parent = nullptr );

    virtual ~CColorMapWidget()
    {}

    // access

	void SetShowContour( bool checked );

	void SetShowSolidColor( bool checked );

	void SetLUT( CBratLookupTable *lut );

	QString itemText( int index )
	{
		return mColorTables->itemText( index );
	}

signals:
	void CurrentIndexChanged( int );
	void ShowContourToggled( bool );
	void ShowSolidColorToggled( bool );

protected slots:

void HandleColorTablesIndexChanged( int index );
};



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

public:

    QCheckBox *mLogScaleCheck = nullptr;
    QLineEdit *mAxisLabel = nullptr;
    QLineEdit *mNbTicks  = nullptr;
    QLineEdit *mNbDigits = nullptr;
	QDoubleSpinBox *m2DScaleSpin = nullptr;
    QLineEdit *mAxisMax = nullptr;
	QDoubleSpinBox *m3DScaleSpin = nullptr;

    QLineEdit *mAxisMin = nullptr;

	//construction / destruction

    CAxisTab( QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

    virtual ~CAxisTab()
    {}
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

	QWidget *mBox2D = nullptr;
	QWidget *mBox3D = nullptr;
	QWidget *mBoxHistogram = nullptr;

    QGroupBox *mLineOptions = nullptr;
    QGroupBox *mPointOptions = nullptr;
    QGroupBox *mSpectrogramOptions = nullptr;
	QGroupBox *mHistogramOptions = nullptr;

    CColorButton *mLineColorButton  = nullptr;
    CColorButton *mPointColorButton = nullptr;

    QComboBox *mStipplePattern = nullptr;
    QComboBox *mPointGlyph = nullptr;

    QLineEdit *mLineOpacityValue = nullptr;
    QCheckBox *mFillPointCheck = nullptr;

    QLineEdit *mLineWidthValue = nullptr;
    QLineEdit *mPointSizeValue = nullptr;

	CColorMapWidget *mColorMapWidget = nullptr;

    QLineEdit *mNumberOfBins = nullptr;


	//construction / destruction

public:
    CPlotControlsPanelCurveOptions( QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

    virtual ~CPlotControlsPanelCurveOptions()
    {}

	void SwitchTo2D();
	void SwitchTo3D();
	void SwitchToHistogram( bool hide3d );

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

	//CAxisTab *mX_axis = nullptr;
	//CAxisTab *mY_axis = nullptr;
	//CAxisTab *mZ_axis = nullptr;

	//QDoubleSpinBox *mXScaleSpin = nullptr; 
	//QDoubleSpinBox *mYScaleSpin = nullptr; 
	//QDoubleSpinBox *mZScaleSpin = nullptr; 

    QLineEdit *mXAxisLabel = nullptr;
    QLineEdit *mXNbTicks  = nullptr;
    QLineEdit *mXNbDigits = nullptr;
	QDoubleSpinBox *mX2DScaleSpin = nullptr;
    QLineEdit *mXAxisMinMax = nullptr;
	QDoubleSpinBox *mX3DScaleSpin = nullptr;

    QLineEdit *mYAxisLabel = nullptr;
    QLineEdit *mYNbTicks  = nullptr;
    QLineEdit *mYNbDigits = nullptr;
	QDoubleSpinBox *mY2DScaleSpin = nullptr;
    QLineEdit *mYAxisMinMax = nullptr;
	QDoubleSpinBox *mY3DScaleSpin = nullptr;

    QLineEdit *mZAxisLabel = nullptr;
    QLineEdit *mZNbTicks  = nullptr;
    QLineEdit *mZNbDigits = nullptr;
	QDoubleSpinBox *mZ2DScaleSpin = nullptr;
    QLineEdit *mZAxisMinMax = nullptr;
	QDoubleSpinBox *mZ3DScaleSpin = nullptr;

protected:
#if defined USE_AXIS_TABS
	QTabWidget *mAxisOptionsTabs = nullptr;
#else
#endif

    //void Wire();
public:
    CPlotControlsPanelAxisOptions( QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

    virtual ~CPlotControlsPanelAxisOptions()
    {}

	void SelectTab( int index );


	void SwitchTo2D();

	void SwitchTo3D();
};





////////////////////////////////////////
//		Animation Options Tab					
////////////////////////////////////////

struct CPlotControlsPanelAnimateOptions : public CControlPanel
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
	QToolButton *mAnimateButton = nullptr; 
	QToolButton *mStopButton = nullptr; 
	QToolButton *mMoveForwardButton = nullptr; 
	QToolButton *mMoveBackwardButton = nullptr; 
	QToolButton *mLoopButton = nullptr; 
	QLineEdit *mSpeedEdit = nullptr; 
	QSpinBox *mFrameIndexSpin = nullptr; 

	QTextEdit *mInformation = nullptr;

public:

	//construction / destruction

public:
    CPlotControlsPanelAnimateOptions( QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

    virtual ~CPlotControlsPanelAnimateOptions()
    {}

    void SetRange( unsigned int max );


	void SwitchTo2D();

	void SwitchTo3D();

public slots:
	
	void HandleFrameChanged( size_t iframe );	//to update mFrameIndexSpin

protected slots:
	
	void HandleAnimateButtonToggled( bool toggled );
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

	CColorMapWidget *mColorMapWidget = nullptr;

    QLineEdit *mMinRange = nullptr;
    QLineEdit *mMaxRange = nullptr;
    QPushButton *mReset = nullptr;

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
