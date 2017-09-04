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
	//setAttribute( Qt::WA_DeleteOnClose );		//the question is, why was this here in previous versions

	setObjectName( QString::fromUtf8( "centralWidget" ) );

    setWindowTitle( "Desktop Manager" );        //for diagnostic reasons

    mMap = new CMapWidget( settings.VectorSimplifyMethod(), settings.MainLayerBaseType(), this, true );

	parent->setCentralWidget( this );

	connect( mMap, &CMapWidget::windowTitleChanged, this, &CDesktopManagerBase::HandleMapWindowTitleChanged );

	mMap->Home();
}


//slot
void CDesktopManagerBase::HandleMapWindowTitleChanged( const QString &title )
{
	if ( title.isEmpty() )
		SetMapTitle( " " );		//hack: do not use empty string to always show map box
	else
		SetMapTitle( title );
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
    static const bool is_gnome = IsGnomeDesktop();

	setAttribute( Qt::WA_DeleteOnClose );

#if defined (_WIN32) || defined (WIN32)
	// Show maximize button in windows
	// If this is set in linux, it will not center the dialog over parent
    setWindowFlags( ( windowFlags() & ~Qt::Dialog ) | Qt::Window | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint );
#elif defined (Q_OS_LINUX)
    // Show buttons in gnome
    // In Debian 8 the user must also call gnome-tweak-tool from the command line and then:
    //  - select Windows | Titlebar Buttons
    //  - click ON for maximize and minimize buttons
    //
    if ( is_gnome )
    {
        setWindowFlags( ( windowFlags() & ~Qt::Dialog ) | Qt::Window | Qt::WindowCloseButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint );
#if defined (DEBUG) || defined(_DEBUG)
        LOG_INFO("Running in Gnome desktop");
#endif
    }
#elif defined (Q_OS_MAC)
    // Qt::WindowStaysOnTopHint also works (too well: stays on top of other apps also). Without this, we have the mac MDI mess...
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

	mMap->setMinimumSize(  min_globe_widget_width, min_globe_widget_height );
	addTab( mMap, "Map" );

#else

	mSplitter = CreateSplitterIn( this, Qt::Horizontal );
	mSplitter->setFrameStyle( QFrame::Panel );
	mSplitter->setFrameShadow( QFrame::Sunken );
	mSplitter->setMinimumSize( min_globe_widget_width, min_globe_widget_height );
	mMapBox = CreateGroupBox( ELayoutType::Horizontal, { mMap }, "Dataset", nullptr, 0, 0, 0, 0, 0 );
	mMapBox->setStyleSheet("QGroupBox { font-weight: bold; } ");
	mMapBox->setAlignment( Qt::AlignCenter );
	mSplitter->addWidget( mMapBox );
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
