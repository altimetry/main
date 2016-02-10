#if !defined GUI_CONTROLS_PANEL_H
#define GUI_CONTROLS_PANEL_H

#include "new-gui/brat/GUI/DesktopManager.h"

// File Index:
//
//	- Individual Controls 
//	- GUI Control Panels
//	- Graphics Control Panels



class QgsCollapsibleGroupBox;
class CWorkspaceDataset;
class CWorkspaceOperation;
class CWorkspaceDisplay;


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

    int CurrentIndex() const
    {
        return currentIndex();
    }


    void SetCurrentIndex( int index );



    QAbstractButton* Button( int index ) const
	{
		return mGroup->buttons().at( index );
	}


	// operations

signals:

	void PageChanged( int i );


protected slots:

    void buttonToggled( bool checked );
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

	////////////////
	// static data
	////////////////

	static const int smSpacing = default_spacing;
	static const int smLeft = default_left;
	static const int smTop = default_top;
	static const int smRight = default_right;
	static const int smBottom = default_bottom;


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
		int spacing = smSpacing, int left = smLeft, int top = smTop, int right = smRight, int bottom = smBottom )
	{
		assert__( parent != this );

		return ::LayoutWidgets( o, v, parent, spacing, left, top, right, bottom );
	}

	QGridLayout* LayoutWidgets( const std::vector< QObject* > &v, QWidget *parent = nullptr,
		int spacing = smSpacing, int left = smLeft, int top = smTop, int right = smRight, int bottom = smBottom )
	{
		assert__( parent != this );

		return ::LayoutWidgets( v, parent, spacing, left, top, right, bottom );
	}

	static 
	QGroupBox* CreateGroupBox( ELayoutType o, const std::vector< QObject* > &v, const QString &title = "", QWidget *parent = nullptr,
		int spacing = smSpacing, int left = smLeft, int top = smTop, int right = smRight, int bottom = smBottom )
	{
		return ::CreateGroupBox< QGroupBox >( o, v, title, parent, spacing, left, top, right, bottom );
	}

	static 
	QgsCollapsibleGroupBox* CreateCollapsibleGroupBox( ELayoutType o, const std::vector< QObject* > &v, const QString &title = "", QWidget *parent = nullptr,
		int spacing = smSpacing, int left = smLeft, int top = smTop, int right = smRight, int bottom = smBottom )
	{
		return ::CreateGroupBox< QgsCollapsibleGroupBox >( o, v, title, parent, spacing, left, top, right, bottom);
	}

	static QListWidget* CreateBooleanList( QWidget *parent, std::initializer_list< BooleanListItemInfo > il );



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
	CDesktopManagerBase *mManager = nullptr;

public:
	explicit CDesktopControlsPanel( CDesktopManagerBase *manager, QWidget *parent = nullptr, Qt::WindowFlags f = 0 )
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

    QListWidget *mFilesList = nullptr;
	QComboBox *mDatasetsCombo = nullptr;

	CWorkspaceDataset *mWks = nullptr;


	//construction / destruction

    void Wire();

public:
	explicit CDatasetBrowserControls( CDesktopManagerBase *manager, QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~CDatasetBrowserControls()
	{}

	// access 


	// operations

public slots:
	void WorkspaceChanged( CWorkspaceDataset *wksd );
    void DatasetChanged(int currentIndex);

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

    QComboBox *mSavedRegionsCombo = nullptr;
    QComboBox *mOpenFilterCombo   = nullptr;

	CWorkspaceDataset *mWks = nullptr;

public:
	explicit CDatasetFilterControls( CDesktopManagerBase *manager, QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~CDatasetFilterControls()
	{}	

	// access 


	// operations

public slots:
	void WorkspaceChanged( CWorkspaceDataset *wksd );
};




/////////////////////////////////////////////////////////////////////////////////////
//									Operations
/////////////////////////////////////////////////////////////////////////////////////

//HAMMER SECTION
class CmdLineProcessor;
//HAMMER SECTION


class COperationsControls : public CDesktopControlsPanel
{
	//HAMMER SECTION
	CmdLineProcessor *mCmdLineProcessor = nullptr;
	void openTestFile( const QString &fileName );
	//HAMMER SECTION

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

public:

    enum EMode
    {
        eQuick,
        eAdvanced
    };


protected:

	// data

    CStackedWidget *mStackWidget = nullptr;

	QPushButton *mQuickMapButton = nullptr;
	QPushButton *mQuickPlotButton = nullptr;

	CWorkspaceOperation *mWkso = nullptr;
	CWorkspaceDisplay *mWksd = nullptr;

	// construction / destruction

    void WireOperations();
	QWidget* CreateQuickOperationsPage();
	QWidget* CreateAdancedOperationsPage();

public:
	explicit COperationsControls( CDesktopManagerBase *manager, QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~COperationsControls()
	{}	

	// access 

    bool AdvancedMode() const;

    void SetAdvancedMode( bool advanced ) const;


	// operations

public slots:
	void WorkspaceChanged( CWorkspaceOperation *wksdo );
    void WorkspaceChanged( CWorkspaceDisplay *wksd );

protected slots:
	void QuickMap();
	void QuickPlot();
};











#endif	//GUI_CONTROLS_PANEL_H
