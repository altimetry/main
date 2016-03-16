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

    QColor GetColor()
    {
        return mCurrentColor;
    }

    public slots:
    void SetColor();

    void SetInternalColor(QColor new_color)
    {
        mCurrentColor = new_color;
        mColorDisplay->setStyleSheet(smStyleSheet_colorButton + mCurrentColor.name());
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

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

    // types

    using base_t = CControlPanel;

    // static members

    // instance data members

    QListWidget *mPlotList = nullptr;

    CColorButton *mLineColorButton  = nullptr;
    CColorButton *mPointColorButton = nullptr;

    QComboBox *mStipplePattern = nullptr;
    QComboBox *mPointGlyph = nullptr;

    QGroupBox *mLineOptions = nullptr;
    QGroupBox *mPointOptions = nullptr;

    QLineEdit *mLineOpacityValue = nullptr;
    QLineEdit *mLineWidthValue = nullptr;


    QLineEdit * mPointSizeValue = nullptr;

    QCheckBox * mFillPointCheck = nullptr;

	//construction / destruction

	void Wire();
public:
    CPlotControlsPanelCurveOptions( QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

    virtual ~CPlotControlsPanelCurveOptions()
    {}

    // access


    // operations

    void AddNewPlot2DName(const QString plot_name);

    void SetPlotTypeLine()
    {
        mLineOptions->setChecked(true);
        mPointOptions->setChecked(false);
    }

    void SetPlotTypeDot()
    {
        mPointOptions->setChecked(true);
        mLineOptions->setChecked(false);
    }

    void SetLineColor(const QColor new_color)
    {
        mLineColorButton->SetInternalColor(new_color);
    }

    void SetPointColor(const QColor new_color)
    {
        mPointColorButton->SetInternalColor(new_color);
    }

    void SetLineOpacity(const QString new_opacity)
    {
        mLineOpacityValue->setText(new_opacity);
    }

    void SetLineWidth(int new_width)
    {

        mLineWidthValue->setText(QString::number(new_width));
    }

    void SetPointSize(const QString new_size)
    {
        mPointSizeValue->setText(new_size);
    }

    void SetFillPointCheck(const bool query)
    {
        mFillPointCheck->setChecked(query);
    }

    void SetStipplePattern(int pattern)
    {
        int c_sz = mStipplePattern->count();
        if (pattern < 0)
        {
            mStipplePattern->setCurrentIndex(c_sz-1);
            return;
        }

        mStipplePattern->setCurrentIndex(pattern);
    }

    void SetGlyphPattern(int pattern)
    {
        int c_sz = this->mPointGlyph->count();
        if (pattern < 0)
        {
            mPointGlyph->setCurrentIndex(c_sz-1);
            return;
        }

        mPointGlyph->setCurrentIndex(pattern);
    }

    void SetGlyphSize(QString pattern_size)
    {
        mPointSizeValue->setText(pattern_size);
    }

    // access


    // operations

signals:
    void LineOptionsChecked( bool checked, int index );
    void PointOptionsChecked( bool checked, int index );

    void CurveLineColorSelected( QColor new_color, int index );
    void CurvePointColorSelected( QColor new_color, int index );

    void CurrCurveChanged(int index);

    //emit changed opacity
    void CurveLineOpacityEntered(int opacity, int index);
    //width value
    void CurveLineWidthEntered(int width, int index);

    void StipplePatternChanged(int pattern, int index);

    void GlyphPatternChanged(int pattern, int index);


    void CurveGlyphWidthEntered(int pixels, int index);

    void FillGlyphInterior(bool checked, int index);

protected slots:
    void HandleLineOptionsChecked( bool checked );
    void HandlePointOptionsChecked( bool checked );
    //User selected a new curve color:
    void HandleCurveLineColorSelected();
    void HandleCurvePointColorSelected();
    // changed opacity
    void HandleCurveLineOpacityEntered();
    //changed line width
    void HandleCurveLineWidthEntered();

    void HandleNewPlot( const CDisplayFilesProcessor* curr_proc);
    void HandleCurrCurveChanged(int index);
    // changed plot glyph
    void HandleStipplePatternChanged(int pattern);

    void HandleGlyphPatternChanged(int pattern);

    void HandleCurveGlyphWidthEntered();

    void HandleFillGlyphInterior(bool checked);
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


struct CMapControlsPanelDataLayers : public CControlPanel
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

public:
    // instance data members

    QComboBox *mDataLayer  = nullptr;

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

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

    // types

    using base_t = CControlPanel;

    // instance data members

    QTabWidget *ViewOptionsTabs = nullptr;

public:
    CMapControlsPanelView( QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~CMapControlsPanelView()
	{}
};



#endif	//GUI_CONTROL_PANELS_VIEWS_VIEW_CONTROL_PANELS_H
