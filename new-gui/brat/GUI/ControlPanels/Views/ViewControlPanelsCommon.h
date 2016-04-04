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

	QLineEdit *mPlotTitle = nullptr;
	QLineEdit *mPlotType = nullptr;


	//construction / destruction

	void CreateWidgets();
public:
    CViewControlsPanelGeneral( QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~CViewControlsPanelGeneral()
	{}
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

public:
    // instance data members

    QListWidget *mPlotTypesList = nullptr;
	QComboBox *mLinkToPlot = nullptr;

protected:

	void CreateWidgets();
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

public:

protected:

	void CreateWidgets();
public:
    CViewControlsPanelGeneralMaps( QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~CViewControlsPanelGeneralMaps()
	{}
};



#endif	//GUI_CONTROL_PANELS_VIEW_CONTROL_PANELS_COMMON_H
