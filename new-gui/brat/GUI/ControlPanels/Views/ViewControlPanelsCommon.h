#if !defined GUI_CONTROL_PANELS_VIEW_CONTROL_PANELS_COMMON_H
#define GUI_CONTROL_PANELS_VIEW_CONTROL_PANELS_COMMON_H


#include "../ControlPanel.h"

class CDisplay;

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//								General Tab
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////
//		Common General Tab					
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

public:
	QLayout *mViewsLayout = nullptr;
	QToolButton *mNewDisplayButton = nullptr;
    QToolButton *mRenameDisplayButton = nullptr;
	QToolButton *mDeleteDisplayButton = nullptr;
	QToolButton *mExecuteDisplay = nullptr;
    QComboBox *mDisplaysCombo = nullptr;

	//construction / destruction

	void CreateWidgets();
	void Wire();
public:
    CViewControlsPanelGeneral( QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

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
//		Plots General Tab
////////////////////////////////////////

struct CViewControlsPanelGeneralPlots : public CViewControlsPanelGeneral
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

    using base_t = CViewControlsPanelGeneral;

    // instance data members

    QComboBox *mSelectPlotCombo = nullptr;
    QComboBox *mvarX       = nullptr;
    QComboBox *mvarY       = nullptr;
    QComboBox *mvarY2      = nullptr;
    QComboBox *mvarZ       = nullptr;

    QComboBox *mLinkToPlot = nullptr;

protected:

	void CreateWidgets();
	void Wire();
public:
    CViewControlsPanelGeneralPlots( QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~CViewControlsPanelGeneralPlots()
	{}
};



////////////////////////////////////////
//		Maps General Tab
////////////////////////////////////////

struct CViewControlsPanelGeneralMaps : public CViewControlsPanelGeneral
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

    using base_t = CViewControlsPanelGeneral;

    // instance data members

    QComboBox *mData = nullptr;

protected:

	void CreateWidgets();
	void Wire();
public:
    CViewControlsPanelGeneralMaps( QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~CViewControlsPanelGeneralMaps()
	{}
};





/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//								Plots Table Tab					
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


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







#endif	//GUI_CONTROL_PANELS_VIEW_CONTROL_PANELS_COMMON_H
