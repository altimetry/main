#include "new-gui/brat/stdafx.h"

#include "GUI/TabbedDock.h"
#include "DesktopManager.h"


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
//
/////////////////////////////////////////////////////////////////////////////////////

QWidget* CDesktopManagerSDI::CenterOnWidget( QWidget *const w, const QWidget *const parent )
{
    w->show();
    QApplication::processEvents();
    return CenterOnParentCenter( w, parent->mapToGlobal( parent->rect().center() ) );
}

//virtual 
CDesktopManagerSDI::desktop_child_t* CDesktopManagerSDI::AddSubWindow( QWidget *widget, Qt::WindowFlags flags )	//flags = 0 
{
	desktop_child_t *child = new desktop_child_t( this, flags );
    SetChildWindowTitle( child, widget );

    AddWidget( child, widget );
    widget->setParent( child );
    //if ( parentWidget()->width() / 3 * 2 > child->width() )	//TODO this risks showing too high: consider on the right of main working dock
    //    CenterOnWidget( child, mMap );
    //else
        CenterOnWidget( child, parentWidget() );

	connect( child, SIGNAL( closed( QWidget* ) ), this, SLOT( SubWindowClosed( QWidget* ) ) );

	mSubWindows.push_back( child );
	return child;
}


CDesktopManagerSDI::CDesktopManagerSDI( const CApplicationPaths &paths, QMainWindow *parent )
	: base_t( paths, parent )
{
	//QWidget *centralWidget = parent->centralWidget();
	//QGridLayout *centralLayout = new QGridLayout( centralWidget );
	//centralWidget->setLayout( centralLayout );
	//centralLayout->setContentsMargins( 0, 0, 0, 0 );

	// "theMapCanvas" used to find this canonical instance later
	//mMapCanvas = new QgsMapCanvas( centralWidget, "theMapCanvas" );
	//mMapCanvas->setWhatsThis( tr( "Map canvas. This is where raster and vector "
	//"layers are displayed when added to the map" ) );

	// set canvas color right away
	//int myRed = settings.value( "/qgis/default_canvas_color_red", 255 ).toInt();
	//int myGreen = settings.value( "/qgis/default_canvas_color_green", 255 ).toInt();
	//int myBlue = settings.value( "/qgis/default_canvas_color_blue", 255 ).toInt();
	//mMapCanvas->setCanvasColor( QColor( myRed, myGreen, myBlue ) );

	//centralLayout->addWidget( mMap, 0, 0, 2, 1 );

	mMap->setMinimumSize( min_main_window_width / 3 * 2, min_main_window_height / 3 * 2 );

#if defined TABBED_MANAGER

	addTab( mMap, "Map" );

#else

	const int margin = 11;

	auto frame = new QFrame( parent );
	LayoutWidgets( { frame }, this, 0, 0, 0, 0, 0 );
	frame->setFrameStyle( QFrame::Panel );
	frame->setFrameShadow( QFrame::Sunken );
	LayoutWidgets( Qt::Horizontal, { mMap }, frame, 0, margin, margin, margin, margin );

#endif
}



/////////////////////////////////////////////////////////////////////////////////////
//
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

CDesktopManagerMDI::CDesktopManagerMDI( const CApplicationPaths &paths, QMainWindow *parent )		//parent = nullptr 
	: base_t( paths, parent )
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
