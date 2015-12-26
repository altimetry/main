#if !defined GUI_CONTROLS_PANEL_H
#define GUI_CONTROLS_PANEL_H


using controls_panel_base_t = QWidget;


class CControlsPanel : public controls_panel_base_t
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	using base_t = controls_panel_base_t;

	QVBoxLayout *mMainLayout = nullptr;


public:
	explicit CControlsPanel( QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~CControlsPanel()
	{}	

	// access 


	// operations

};




/////////////////////////////////////////////////////////////////////////////////////
//								Dataset Browser 
/////////////////////////////////////////////////////////////////////////////////////

class CDatasetBrowserControls : public CControlsPanel
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	using base_t = CControlsPanel;

public:
	explicit CDatasetBrowserControls( QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~CDatasetBrowserControls()
	{}

	// access 


	// operations

};


/////////////////////////////////////////////////////////////////////////////////////
//								Dataset Filter
/////////////////////////////////////////////////////////////////////////////////////

class CDatasetFilterControls : public CControlsPanel
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	using base_t = CControlsPanel;

public:
	explicit CDatasetFilterControls( QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~CDatasetFilterControls()
	{}	

	// access 


	// operations

};




/////////////////////////////////////////////////////////////////////////////////////
//								Quick Operations
/////////////////////////////////////////////////////////////////////////////////////

class CQuickOperationsControls : public CControlsPanel
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	using base_t = CControlsPanel;

public:
	explicit CQuickOperationsControls( QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~CQuickOperationsControls()
	{}	

	// access 


	// operations

};




/////////////////////////////////////////////////////////////////////////////////////
//								Advanced Operations
/////////////////////////////////////////////////////////////////////////////////////

class CAdvancedOperationsControls : public CControlsPanel
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	using base_t = CControlsPanel;

public:
	explicit CAdvancedOperationsControls( QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~CAdvancedOperationsControls()
	{}	

	// access 


	// operations

};




/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//								Graphics Panels
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

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

	using base_t = CControlsPanel;

public:
	explicit CPlotViewControls( QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~CPlotViewControls()
	{}	

	// access 


	// operations

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

	using base_t = CControlsPanel;

public:
	explicit CMapViewControls( QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~CMapViewControls()
	{}	

	// access 


	// operations

};







#endif	//GUI_CONTROLS_PANEL_H
