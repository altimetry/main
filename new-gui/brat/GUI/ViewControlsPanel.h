#if !defined GUI_VIEW_CONTROLS_PANEL_H
#define GUI_VIEW_CONTROLS_PANEL_H

#include "ControlsPanel.h"


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//								Graphics Panels
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


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

    // static members


    // instance data members

    QPushButton *mAxisColorButton = nullptr;
    QColor      mAxisColor = QColor();


public:
    explicit CAxisTab( QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

    virtual ~CAxisTab()
    {}

    // access


    // operations
    protected slots:
    void SetAxisColor();

};



/////////////////////////////////////////////////////////////////////////////////////
//								Plot View
/////////////////////////////////////////////////////////////////////////////////////

class CPlotViewControls : public CControlsPanel
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

    QComboBox *mSelectDataset   = nullptr;
    QComboBox *mApplyFilter     = nullptr;
    QComboBox *mSelectOperation = nullptr;

    QComboBox *mOpenAplot  = nullptr;
    QComboBox *mSelectplot = nullptr;
    QComboBox *mvarX       = nullptr;
    QComboBox *mvarY       = nullptr;
    QComboBox *mvarY2      = nullptr;
    QComboBox *mvarZ       = nullptr;

    QComboBox *mLinkToPlot = nullptr;

    QPushButton *mLineColorButton = nullptr;
    QColor      mLineColor = QColor();

    QPushButton *mPointColorButton = nullptr;
    QColor      mPointColor = QColor();

    QComboBox *mStipplePattern = nullptr;
    QComboBox *mPointGlyph     = nullptr;

    QTabWidget *axisOptionsTabs = nullptr;


public:
    explicit CPlotViewControls( QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

    virtual ~CPlotViewControls()
    {}

    // access


    // operations
    protected slots:
    void SetLineColor();
    void SetPointColor();


};




/////////////////////////////////////////////////////////////////////////////////////
//								Map View
/////////////////////////////////////////////////////////////////////////////////////

class CMapViewControls : public CControlsPanel
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

    QComboBox *mSelectDataset   = nullptr;
    QComboBox *mApplyFilter     = nullptr;
    QComboBox *mSelectOperation = nullptr;

    QComboBox *mOpenAplot  = nullptr;
    QComboBox *mAddData    = nullptr;

    QComboBox *mDataLayer  = nullptr;

    QTabWidget *ViewOptionsTabs = nullptr;

public:
    explicit CMapViewControls( QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

    virtual ~CMapViewControls()
    {}

    // access


    // operations

};







#endif	//GUI_VIEW_CONTROLS_PANEL_H
