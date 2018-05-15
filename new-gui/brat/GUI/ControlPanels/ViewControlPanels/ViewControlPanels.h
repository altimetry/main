/*
* This file is part of BRAT
*
* BRAT is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* BRAT is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#if !defined GUI_CONTROL_PANELS_VIEWS_VIEW_CONTROL_PANELS_H
#define GUI_CONTROL_PANELS_VIEWS_VIEW_CONTROL_PANELS_H


#include "../ColorMapWidget.h"
#include "ViewControlPanelsCommon.h"


class CDisplay;
class CTextWidget;
class CColorButton;



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
    //QGroupBox *mStatisticsOptions = nullptr;
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

	static const double smMaxScale;

public:

    // instance data members

	//CAxisTab *mX_axis = nullptr;
	//CAxisTab *mY_axis = nullptr;
	//CAxisTab *mZ_axis = nullptr;

    QLineEdit *mXAxisLabel = nullptr;
	QToolButton *mXAxisDimension = nullptr;
	QLineEdit *mXNbTicks  = nullptr;
    QLineEdit *mXNbDigits = nullptr;
	QDoubleSpinBox *mX2DScaleSpin = nullptr;
    QLineEdit *mXAxisMinMax = nullptr;
	QDoubleSpinBox *mX3DScaleSpin = nullptr;

    QLineEdit *mYAxisLabel = nullptr;
	QToolButton *mYAxisDimension = nullptr;
	QLineEdit *mYNbTicks  = nullptr;
    QLineEdit *mYNbDigits = nullptr;
	QDoubleSpinBox *mY2DScaleSpin = nullptr;
    QLineEdit *mYAxisMinMax = nullptr;
	QDoubleSpinBox *mY3DScaleSpin = nullptr;

    QLineEdit *mZAxisLabel = nullptr;
	QToolButton *mZAxisDimension = nullptr;
	QLineEdit *mZNbTicks  = nullptr;
    QLineEdit *mZNbDigits = nullptr;
	QDoubleSpinBox *mZ2DScaleSpin = nullptr;
    QLineEdit *mZAxisMinMax = nullptr;
	QDoubleSpinBox *mZ3DScaleSpin = nullptr;

    QCheckBox *mXLogScaleCheck = nullptr;
    QCheckBox *mYLogScaleCheck = nullptr;
    QCheckBox *mZLogScaleCheck = nullptr;

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

	CTextWidget *mInformation = nullptr;

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

	QGroupBox *mMagnitudeFactorGroupBox = nullptr;
	QLineEdit *mMagnitudeFactorEdit = nullptr;
	CColorMapWidget *mColorMapWidget = nullptr;

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
