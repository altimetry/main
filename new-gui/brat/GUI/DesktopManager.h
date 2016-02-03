#if !defined GUI_DESKTOP_MANAGER_H
#define GUI_DESKTOP_MANAGER_H

#include <QMainWindow>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QGridLayout>

#include "new-gui/Common/QtUtils.h"
#include "new-gui/brat/Views/MapWidget.h"
#include "new-gui/brat/GUI/DisplayEditor.h"


class CTreeWorkspace;

class CTabbedDock;


//#define TABBED_MANAGER

#if defined TABBED_MANAGER
using desktop_manager_base_t = QTabWidget;
#else
using desktop_manager_base_t = QWidget;
#endif

/////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////


class CDesktopManagerBase : public desktop_manager_base_t, non_copyable
{
	// types

	using base_t = desktop_manager_base_t;

protected:

	//data

	CMapWidget *mMap = nullptr;
	CTabbedDock *mMapDock = nullptr;

protected:

	// construction / destruction

#if defined TABBED_MANAGER
	CDesktopManagerBase( QMainWindow *const parent )
		: base_t( parent )
#else
	CDesktopManagerBase( QMainWindow *const parent, Qt::WindowFlags f = 0 )
		: base_t( parent, f )
#endif
	{
		setAttribute( Qt::WA_DeleteOnClose );

        setObjectName(QString::fromUtf8("centralWidget"));

		mMap = new CMapWidget( this );

        parent->setCentralWidget( this );
	}

	static void SetChildWindowTitle( QWidget *child, const QWidget *widget = nullptr )
	{
		QString title = widget ? widget->windowTitle() : "";
		if ( title.isEmpty() )
			title = "untitled";

        child->setWindowTitle( title + "[*]" );
	}

public:
	virtual ~CDesktopManagerBase()
	{
		//delete mMap;
	}


	// access

	CMapWidget* Map() { return mMap; }


	CTabbedDock* MapDock() { return mMapDock; }


	virtual QList<QWidget*> SubWindowList() = 0;

public:
	virtual QWidget* AddSubWindow( QWidget *widget, Qt::WindowFlags flags = 0 ) = 0;

	virtual void CloseAllSubWindows() = 0;
};




template< class SUB_WINDOW >
class CAbstractDesktopManager : public CDesktopManagerBase
{
	// types

	using base_t = CDesktopManagerBase;

protected:

	using desktop_child_t = SUB_WINDOW;

	//data

protected:

	// construction / destruction

#if defined TABBED_MANAGER
	CAbstractDesktopManager( QMainWindow *const parent )
		: base_t( parent )
#else
	CAbstractDesktopManager( QMainWindow *const parent, Qt::WindowFlags f = 0 )
		: base_t( parent, f )
#endif
	{}


public:
	virtual ~CAbstractDesktopManager()
	{}
};




/////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////

class CSubWindow : public QDialog
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

		
	using base_t = QDialog;

	//data

public:
	explicit CSubWindow( QWidget *parent = nullptr, Qt::WindowFlags f = 0 )
		: base_t( parent, f )
	{
		setAttribute( Qt::WA_DeleteOnClose );

#if defined (_WIN32) || defined (WIN32)
		// Show maximize button in windows
		// If this is set in linux, it will not center the dialog over parent
		setWindowFlags( ( windowFlags() & ~Qt::Dialog ) | Qt::Window | Qt::WindowMaximizeButtonHint );
#elif defined (Q_OS_MAC)
		// Qt::WindowStaysOnTopHint also works (too weel: stays on top of other apps also). Without this, we have the mac MDI mess...
		setWindowFlags( ( windowFlags() & ~Qt::Dialog ) | Qt::Tool );
#endif

	}

	virtual ~CSubWindow()
	{}

	virtual void closeEvent( QCloseEvent *event ) override
	{
		  emit closed( this );
		  event->accept();
	}

signals:

	// qt moc does not recognize typedefs in receiver, so use known type as argument
	//
	void closed( QWidget *emitter );		
};




class CDesktopManagerSDI : public CAbstractDesktopManager< CSubWindow >
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	using base_t = CAbstractDesktopManager;

protected:

	//data

	QList<QWidget*> mSubWindows;

protected:

public:
	//ctor/dtor

	CDesktopManagerSDI( QMainWindow *parent );

	virtual ~CDesktopManagerSDI()
	{}

public:
	virtual QList<QWidget*> SubWindowList() override
	{
		return mSubWindows;
	}

	virtual desktop_child_t* AddSubWindow( QWidget *widget, Qt::WindowFlags flags = 0 ) override;

	virtual void CloseAllSubWindows() override
	{	
		//this also works on windows at least
		//for ( auto child : mSubWindows )
		//	child->close();
		foreach ( auto child, mSubWindows )
			child->close();
	}

protected:
	QWidget* CenterOnWidget( QWidget *const w, const QWidget *const parent );

	QWidget* AddTab( QWidget *tab_widget, const QString &title );

protected slots:
	void closeAllSubWindows()
	{
		CloseAllSubWindows();
	}

	void SubWindowClosed( QWidget *emitter )
	{
		desktop_child_t *child = qobject_cast< desktop_child_t* >( emitter );

		mSubWindows.removeOne( child );
	}
};





/////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////


//using desktop_manager_base_t = QMdiArea;
//using desktop_manager_child_t = QMdiSubWindow;

class CDesktopManagerMDI : public CAbstractDesktopManager< QMdiSubWindow >
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	using desktop_child_t = QMdiSubWindow;

	using base_t = CAbstractDesktopManager< desktop_child_t >;

protected:

	//data

private:
	QMdiArea *mMdiArea = nullptr;

	//construction / destruction

protected:

	void AddMDIArea( QWidget *parent );
public:

	CDesktopManagerMDI( QMainWindow *parent );

	virtual ~CDesktopManagerMDI()
	{}

	// access / operations

public:
    desktop_child_t* AddSubWindow( QWidget *widget, Qt::WindowFlags flags = 0 ) override
	{
        desktop_child_t *child = mMdiArea->addSubWindow( widget, flags );
        SetChildWindowTitle( child, widget );
		return child;
	}

	virtual QList<QWidget*> SubWindowList() override
	{
		QList<QWidget*> list;
        auto mdi_list = mMdiArea->subWindowList();
		for ( auto pchild : mdi_list )
			list << pchild;
		return list;
	}

	virtual void CloseAllSubWindows() override
	{
		return mMdiArea->closeAllSubWindows();
	}
};




//
///////////////////////////////////////////////////////////////////////////////////////
////					Default Application Desktop Type
///////////////////////////////////////////////////////////////////////////////////////
//
//
////#define BRAT_COMMON_DESKTOP
//
//
//#if defined(Q_OS_MACX) || defined(BRAT_COMMON_DESKTOP)
//    using desktop_manager_t = CDesktopManagerSDI;
//#else
//    using desktop_manager_t = CDesktopManagerMDI;
//#endif
//
//
//

#endif	//GUI_DESKTOP_MANAGER_H
