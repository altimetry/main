#if !defined GUI_CONTROLS_PANEL_H
#define GUI_CONTROLS_PANEL_H

#include "new-gui/brat/GUI/DesktopManager.h"


// File Index:
//
//	- Individual Controls 
//	- GUI Control Panels
//	- Graphics Control Panels



class QgsCollapsibleGroupBox;


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//									Controls
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

struct CStackedWidget : public QStackedWidget
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

	using base_t = QStackedWidget;
public:

	struct PageInfo
	{
		QWidget *mWidget;
		std::string mName;
	};

	// data

protected:
	QButtonGroup *mGroup = nullptr;

public:
	CStackedWidget( QWidget *parent, const std::vector< PageInfo > &widgets, int iselected = 0 );

	virtual ~CStackedWidget()
	{}

	// access

	QAbstractButton* Button( int index ) const
	{
		return mGroup->buttons().at( index );
	}


	// operations

signals:
	void PageChanged( int i );

protected slots:

	void changePage( QAbstractButton *b )
	{
		auto index = mGroup->buttons().indexOf( b );
		setCurrentIndex( index );
		emit PageChanged( index );
	}
};





/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//									Control Panels
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

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

	///////
	//types
	///////

	using base_t = controls_panel_base_t;

protected:
	// For use with CreateBooleanList
	//
	struct BooleanListItemInfo
	{
		std::string mName;
		bool mChecked;

		BooleanListItemInfo( const std::string &name, bool checked = false )
			: mName( name ), mChecked( checked )
		{}
	};

	// For use with CreateGroupBox
	//
	enum ELayoutType
	{
		eHorizontal = Qt::Horizontal,
		eVertical = Qt::Vertical,
		eGrid,

		eELayoutType_size
	};

	////////////////
	// static data
	////////////////

	static const int smSpacing = 6;
	static const int smLeft = 2;
	static const int smTop = 2;
	static const int smRight = 2;
	static const int smBottom = 2;


	////////////////
	// instance data
	////////////////

private:
	QVBoxLayout *mMainLayout = nullptr;

	////////////////////////////
	//construction / destruction
	////////////////////////////

public:
	explicit CControlsPanel( QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~CControlsPanel()
	{}	

protected:
	////////////////////////////
	// access 
	////////////////////////////

	////////////////////////////
	// operations
	////////////////////////////

	QBoxLayout* LayoutWidgets( Qt::Orientation o, const std::vector< QObject* > &v, QWidget *parent = nullptr, 
		int spacing = smSpacing, int left = smLeft, int top = smTop, int right = smRight, int bottom = smBottom );

	QGridLayout* LayoutWidgets( const std::vector< QObject* > &v, QWidget *parent = nullptr, 
		int spacing = smSpacing, int left = smLeft, int top = smTop, int right = smRight, int bottom = smBottom );

private:
	template< class GROUP_BOX >
	GROUP_BOX* CreateGroupBox( ELayoutType o, const std::vector< QObject* > &v, const QString &title = "", QWidget *parent = nullptr,
		int spacing = smSpacing, int left = smLeft, int top = smTop, int right = smRight, int bottom = smBottom );

protected:
	QGroupBox* CreateGroupBox( ELayoutType o, const std::vector< QObject* > &v, const QString &title = "", QWidget *parent = nullptr,
		int spacing = smSpacing, int left = smLeft, int top = smTop, int right = smRight, int bottom = smBottom );

	QgsCollapsibleGroupBox* CreateCollapsibleGroupBox( ELayoutType o, const std::vector< QObject* > &v, const QString &title = "", QWidget *parent = nullptr,
		int spacing = smSpacing, int left = smLeft, int top = smTop, int right = smRight, int bottom = smBottom );

	static QListWidget* CreateBooleanList( QWidget *parent, std::initializer_list< BooleanListItemInfo > il );

	QSpacerItem* CreateSpace( int w = 0, int h = 0, QSizePolicy::Policy hData = QSizePolicy::Expanding, QSizePolicy::Policy vData = QSizePolicy::Expanding );


	QSpacerItem* AddTopSpace( int w, int h, QSizePolicy::Policy hData = QSizePolicy::Minimum, QSizePolicy::Policy vData = QSizePolicy::Minimum );

	QObject* AddTopWidget( QObject *ob );

	QLayout* AddTopLayout( ELayoutType o, const std::vector< QObject* > &v, 
		int spacing = smSpacing, int left = smLeft, int top = smTop, int right = smRight, int bottom = smBottom );

	QGroupBox* AddTopGroupBox( ELayoutType o, const std::vector< QObject* > &v, const QString &title = "",
		int spacing = smSpacing, int left = smLeft, int top = smTop, int right = smRight, int bottom = smBottom );
};




class CDesktopControlsPanel : public CControlsPanel
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

protected:

protected:
	desktop_manager_t *mManager = nullptr;

public:
	explicit CDesktopControlsPanel( desktop_manager_t *manager, QWidget *parent = nullptr, Qt::WindowFlags f = 0 )
		: base_t( parent, f ), mManager( manager )
	{}

	virtual ~CDesktopControlsPanel()
	{}	

	// access 

	// operations
};




/////////////////////////////////////////////////////////////////////////////////////
//								Dataset Browser 
/////////////////////////////////////////////////////////////////////////////////////

class CDatasetBrowserControls : public CDesktopControlsPanel
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	using base_t = CDesktopControlsPanel;

	CStackedWidget *mBrowserStakWidget = nullptr;
	QPushButton *m_BrowseFilesButton = nullptr;
	QPushButton *m_BrowseRadsButton = nullptr;

	//construction / destruction

	void Wire()
	{}
public:
	explicit CDatasetBrowserControls( desktop_manager_t *manager, QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~CDatasetBrowserControls()
	{}

	// access 


	// operations

protected slots:
	void PageChanged( int index );
};


/////////////////////////////////////////////////////////////////////////////////////
//								Dataset Filter
/////////////////////////////////////////////////////////////////////////////////////

class CDatasetFilterControls : public CDesktopControlsPanel
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	using base_t = CDesktopControlsPanel;

public:
	explicit CDatasetFilterControls( desktop_manager_t *manager, QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~CDatasetFilterControls()
	{}	

	// access 


	// operations

};




/////////////////////////////////////////////////////////////////////////////////////
//								Quick Operations
/////////////////////////////////////////////////////////////////////////////////////

class COperationsControls : public CDesktopControlsPanel
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

	using base_t = CDesktopControlsPanel;

	// data

	CStackedWidget *mOperationsStakWidget = nullptr;

	QPushButton *mQuickMapButton = nullptr;
	QPushButton *mQuickPlotButton = nullptr;

	// construction / destruction

	void WireQuickOperations();
	QWidget* CreateQuickOperationsPage();
	QWidget* CreateAdancedOperationsPage();
public:
	explicit COperationsControls( desktop_manager_t *manager, QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~COperationsControls()
	{}	

	// access 


	// operations

protected slots:
	void QuickMap();
	void QuickPlot();
};




/////////////////////////////////////////////////////////////////////////////////////
//								Advanced Operations
/////////////////////////////////////////////////////////////////////////////////////

//class CAdvancedOperationsControls : public CControlsPanel
//{
//#if defined (__APPLE__)
//#pragma clang diagnostic push
//#pragma clang diagnostic ignored "-Winconsistent-missing-override"
//#endif
//
//    Q_OBJECT
//
//#if defined (__APPLE__)
//#pragma clang diagnostic pop
//#endif
//
//	using base_t = CControlsPanel;
//
//public:
//	explicit CAdvancedOperationsControls( QWidget *parent = nullptr, Qt::WindowFlags f = 0 );
//
//	virtual ~CAdvancedOperationsControls()
//	{}	
//
//	// access 
//
//
//	// operations
//
//};




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
