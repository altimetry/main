#if !defined GUI_DESKTOP_MANAGER_H
#define GUI_DESKTOP_MANAGER_H

#include <QMainWindow>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QGridLayout>

#include "new-gui/Common/QtUtils.h"
#include "new-gui/brat/Views/MapWidget.h"
#include "new-gui/brat/GUI/DisplayEditor.h"

class CTabbedDock;

using desktop_manager_base_t = QWidget;

/////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////


template< class SUB_WINDOW >
class CAbstractDesktopManager : public desktop_manager_base_t, non_copyable
{
	// types

	using base_t = desktop_manager_base_t;

protected:

	using desktop_child_t = SUB_WINDOW;

	//data

	CMapWidget *mMap = nullptr;

protected:

	//ctors/dtor

	CAbstractDesktopManager( QMainWindow *const parent = nullptr, Qt::WindowFlags f = 0 )
		: base_t( parent, f )
	{
		setAttribute( Qt::WA_DeleteOnClose );

        setObjectName(QString::fromUtf8("centralWidget"));

		mMap = new CMapWidget( this );

        parent->setCentralWidget( this );
	}

	static void SetChildWindowTitle( desktop_child_t *child, const QWidget *widget = nullptr )
	{
		QString title = widget ? widget->windowTitle() : "";
		if ( title.isEmpty() )
			title = "untitled";

        child->setWindowTitle( title + "[*]" );
	}

public:
	virtual ~CAbstractDesktopManager()
	{
		//delete mMap;
	}


	CMapWidget* Map() { return mMap; }

public:
	virtual desktop_child_t* AddSubWindow( QWidget *widget, Qt::WindowFlags flags = 0 ) = 0;

	virtual QList<desktop_child_t*> SubWindowList() = 0;

	virtual void CloseAllSubWindows() = 0;
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

public:
	explicit CSubWindow( QWidget *parent = nullptr, Qt::WindowFlags f = 0 )
		: base_t( parent, f )
	{
		setAttribute( Qt::WA_DeleteOnClose );
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




class CDesktopManager : public CAbstractDesktopManager< CSubWindow >
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

	QList<desktop_child_t*> mSubWindows;

public:
	//ctor/dtor

	CDesktopManager( QMainWindow *parent = nullptr )
		: base_t( parent )
	{
        QLayout *l = createLayout( this, Qt::Horizontal, 6, 11, 11, 11, 11 );
        l->addWidget( mMap );
	}

	virtual ~CDesktopManager()
	{}

public:
    inline QWidget* CenterOnWidget( QWidget *const w, const QWidget *const parent )
    {
//        auto r = QStyle::alignedRect( Qt::LeftToRight, Qt::AlignCenter, w->size(), parent->geometry() );
//        r.moveCenter(parent->geometry().center());
//        w->setGeometry(r);
//        return w;
        //return CenterOnRect( w, qApp->desktop()->availableGeometry(parent) ) ;

        //return CenterOnRect( w, parent->geometry() )  ;
        //w->move( QApplication::desktop()->screen()->rect().center() - w->rect().center() );
        //w->move( parent->rect().center() - w->rect().center() );

//        w->move(
//               parent->mapToGlobal( parent->geometry().topLeft() ) +
//               parent->mapToGlobal( parent->window()->geometry().center() ) - w->mapToGlobal( w->geometry().center() )
//            );

    //    auto r = QStyle::alignedRect( Qt::LeftToRight, Qt::AlignCenter, w->size(), parent->rect() );
    //    w->setGeometry( r );
        //w->setGeometry( QStyle::alignedRect( Qt::LeftToRight, Qt::AlignCenter, w->size(), parent->geometry() ) );

        return w;
    }

    virtual desktop_child_t* AddSubWindow( QWidget *widget, Qt::WindowFlags flags = 0 ) override
	{
        Q_UNUSED( flags);

        desktop_child_t *child = new desktop_child_t( this );
        SetChildWindowTitle( child, widget );

#if defined (_WIN32) || defined (WIN32)
		// Show maximize button in windows
		// If this is set in linux, it will not center the dialog over parent
        child->setWindowFlags( child->windowFlags() | Qt::Window );
#elif defined (Q_OS_MAC)
        // Qt::WindowStaysOnTopHint also works (too weel: stays on top of other apps also). Without this, we have the mac MDI mess...
        child->setWindowFlags( child->windowFlags() | Qt::Tool );
#endif

        addWidget( child, widget );
        widget->setParent( child );
        CenterOnWidget( child, mMap );
        //CenterOnScreen( child );

		connect( child, SIGNAL( closed( QWidget* ) ), this, SLOT( SubWindowClosed( QWidget* ) ) );
		mSubWindows.push_back( child );
		return child;
	}

	virtual QList<desktop_child_t*> SubWindowList() override
	{
		return mSubWindows;
	}

	virtual void CloseAllSubWindows() override
	{	
		//this also works on windows at least
		//for ( auto child : mSubWindows )
		//	child->close();
		foreach ( auto child, mSubWindows )
			child->close();
	}

protected slots:

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

	using base_t = CAbstractDesktopManager;

	using desktop_child_t = QMdiSubWindow;

protected:

	//data

private:
	QMdiArea *mMdiArea = nullptr;
	CTabbedDock *mMapDock = nullptr;

public:
	//ctor/dtor

	CDesktopManagerMDI( QMainWindow *parent = nullptr );

	virtual ~CDesktopManagerMDI()
	{}

public:
    desktop_child_t* AddSubWindow( QWidget *widget, Qt::WindowFlags flags = 0 ) override
	{
        desktop_child_t *child = mMdiArea->addSubWindow( widget, flags );
        SetChildWindowTitle( child, widget );
		return child;
	}

	virtual QList<desktop_child_t*> SubWindowList() override
	{
		return mMdiArea->subWindowList();
	}

	virtual void CloseAllSubWindows() override
	{
		return mMdiArea->closeAllSubWindows();
	}
};








#endif	//GUI_DESKTOP_MANAGER_H
