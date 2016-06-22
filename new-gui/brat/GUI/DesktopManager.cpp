#include "new-gui/brat/stdafx.h"

#include "GUI/DisplayWidgets/GlobeWidget.h"
#include "GUI/TabbedDock.h"
#include "DesktopManager.h"
#include "BratSettings.h"


/////////////////////////////////////////////////////////////////////////////////////
//						Desktop Managers Base
/////////////////////////////////////////////////////////////////////////////////////

#if defined TABBED_MANAGER
CDesktopManagerBase::CDesktopManagerBase( const CBratSettings &settings, QMainWindow *const parent )
	: base_t( parent )
#else
CDesktopManagerBase::CDesktopManagerBase( const CBratSettings &settings, QMainWindow *const parent, Qt::WindowFlags f )	//f = 0 
	: base_t( parent, f )
#endif
	, mPaths( settings.BratPaths() )
{
	setAttribute( Qt::WA_DeleteOnClose );

	setObjectName( QString::fromUtf8( "centralWidget" ) );

    mMap = new CMapWidget( settings.MainLayerBaseType(), this );

	parent->setCentralWidget( this );

	mMap->Home();
}


//virtual 
QWidget* CDesktopManagerBase::AddSubWindow( std::function< QWidget*( ) > f, Qt::WindowFlags flags )	//flags = 0 
{
	try
	{
		auto subWindow = AddSubWindow( f(), flags );
		subWindow->show();
		return subWindow;
	}
	catch ( const CException &e )
	{
		SimpleErrorBox( e.Message() );
	}
	catch ( ... )
	{
		LOG_FATAL( "Unknown error: could not add editor to desktop." );
	}
	return nullptr;
}







#if defined TESTS

inline QWidget* CenterOnRect( QWidget *const w, const QRect &r )
{
    w->setGeometry( QStyle::alignedRect( Qt::LeftToRight, Qt::AlignCenter, w->size(), r ) );

    return w;
}

inline QWidget* CenterOnScreen( QWidget *const w )
{
	assert__( qApp );

    //w->setGeometry( QStyle::alignedRect( Qt::LeftToRight, Qt::AlignCenter, w->size(), qApp->desktop()->availableGeometry() ) );
    //w->move( QApplication::desktop()->screen()->rect().center() - w->rect().center() );
    return CenterOnRect( w, qApp->desktop()->availableGeometry() )  ;

    //return w;
}

inline QWidget* CenterOnScreens( QWidget *const w )
{
	w->move( QApplication::desktop()->screen()->rect().center() - w->rect().center() );
}
inline QWidget* CenterOn1stScreen( QWidget *const w )
{
	assert__( qApp );

	w->adjustSize();
    w->setGeometry( QStyle::alignedRect( Qt::LeftToRight, Qt::AlignCenter, w->size(), qApp->desktop()->availableGeometry() ) );

    return w;
}
#endif

/////////////////////////////////////////////////////////////////////////////////////
//									SDI
/////////////////////////////////////////////////////////////////////////////////////

//////////////////////
//		SDI Sub-Window
//////////////////////


CSubWindow::CSubWindow( QWidget *widget, QWidget *parent, Qt::WindowFlags f )		//parent = nullptr, Qt::WindowFlags f = 0 
	: base_t( parent, f )
	, mWidget( widget )
{
	setAttribute( Qt::WA_DeleteOnClose );

#if defined (_WIN32) || defined (WIN32)
	// Show maximize button in windows
	// If this is set in linux, it will not center the dialog over parent
	setWindowFlags( ( windowFlags() & ~Qt::Dialog ) | Qt::Window | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint );
#elif defined (Q_OS_MAC)
	// Qt::WindowStaysOnTopHint also works (too weel: stays on top of other apps also). Without this, we have the mac MDI mess...
	setWindowFlags( ( windowFlags() & ~Qt::Dialog ) | Qt::Tool );
#endif

    AddWidget( this, mWidget );
    mWidget->setParent( this );
}



///////////////
//	SDI Manager
///////////////

QWidget* CDesktopManagerSDI::CenterOnWidget( QWidget *const w, const QWidget *const parent )
{
    w->show();
    QApplication::processEvents();
    return CenterOnParentCenter( w, parent->mapToGlobal( parent->rect().center() ) );
}

//virtual 
CDesktopManagerSDI::desktop_child_t* CDesktopManagerSDI::AddSubWindow( QWidget *widget, Qt::WindowFlags flags )	//flags = 0 
{
	desktop_child_t *child = new desktop_child_t( widget, this, flags );
    SetChildWindowTitle( child, widget );

    //if ( parentWidget()->width() / 3 * 2 > child->width() )	//TODO this risks showing too high: consider on the right of main working dock
    //    CenterOnWidget( child, mMap );
    //else
        CenterOnWidget( child, parentWidget() );

	connect( child, SIGNAL( closed( QWidget* ) ), this, SLOT( SubWindowClosed( QWidget* ) ) );

	mSubWindows.push_back( child );
	return child;
}


CDesktopManagerSDI::CDesktopManagerSDI( const CBratSettings &settings, QMainWindow *parent )
	: base_t( settings, parent )
{

#if defined TABBED_MANAGER

	mMap->setMinimumSize( min_main_window_width / 3 * 2, min_main_window_height / 3 * 2 );
	addTab( mMap, "Map" );

#else

	mSplitter = CreateSplitterIn( this, Qt::Horizontal );
	mSplitter->setFrameStyle( QFrame::Panel );
	mSplitter->setFrameShadow( QFrame::Sunken );
	mSplitter->setMinimumSize( min_main_window_width / 3 * 2, min_main_window_height / 3 * 2 );
	mSplitter->addWidget( mMap );
	//mGlobeView = new CGlobeWidget( this, mMap, parent->statusBar() );
	//mSplitter->addWidget( mGlobeView );

	//QList<int> sizes;
	//sizes.append( min_main_window_width );
	//sizes.append( 0 );

	//mSplitter->setSizes( sizes );
	layout()->setContentsMargins( 4, 4, 4, 4 );

#endif
}



/////////////////////////////////////////////////////////////////////////////////////
//									MDI
/////////////////////////////////////////////////////////////////////////////////////


void CDesktopManagerMDI::AddMDIArea( QWidget *parent )
{
    QGridLayout *gridLayout = new QGridLayout( parent );
    gridLayout->setSpacing(6);
    gridLayout->setContentsMargins(11, 11, 11, 11);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    mMdiArea = new QMdiArea( this );
    mMdiArea->setObjectName(QString::fromUtf8("mdiArea"));
    mMdiArea->setMinimumSize(QSize(500, 400));
    mMdiArea->setViewMode( QMdiArea::SubWindowView );
    gridLayout->addWidget( mMdiArea, 0, 0, 1, 1 );
}

CDesktopManagerMDI::CDesktopManagerMDI( const CBratSettings &settings, QMainWindow *parent )		//parent = nullptr 
	: base_t( settings, parent )
{
#if defined TABBED_MANAGER
	QWidget *tab = new QWidget();
	AddMDIArea( tab );
	addTab( tab, "MDI" );
	addTab( mMap, "Map" );
#else
	AddMDIArea( this );

	mMapDock = new CTabbedDock( "Main Map", this );
	mMapDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );	//mMapDock->setMaximumWidth( max_main_dock_width );
	mMapDock->AddTab( mMap, "Navigator" );

#endif

	//QImage newBackground(":/images/world.png");
	//mdiArea->setBackground( newBackground );

	//// MDI sub-windows logic
	////
	//SubWindows_separatorAction = menu_Window->addSeparator();
	//windowActionGroup = new QActionGroup(this);
	//connect( mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(UpdateEditorsActions()) ); //UpdateEditorsActions => UpdateFileActions
	//connect( mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(InitialUpdateActions()) );
	////
	////it seems the above line is not enough... well, keep testing...
	////
	//connect( menu_File, SIGNAL(aboutToShow()), this, SLOT(UpdateFileActions()) );
	//connect( menu_Edit, SIGNAL(aboutToShow()), this, SLOT(UpdateEditorsActions()) );
	//connect( menu_Window, SIGNAL(aboutToShow()), this, SLOT(UpdateFileActions()) );
}




///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_DesktopManager.cpp"
