#include "new-gui/brat/stdafx.h"

#include "new-gui/brat/GUI/TabbedDock.h"
#include "DesktopManager.h"


CDesktopManagerMDI::CDesktopManagerMDI( QMainWindow *parent )		//parent = nullptr 
	: base_t( parent )
{
    QGridLayout *gridLayout = new QGridLayout( this );
    gridLayout->setSpacing(6);
    gridLayout->setContentsMargins(11, 11, 11, 11);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    mMdiArea = new QMdiArea( this );
    mMdiArea->setObjectName(QString::fromUtf8("mdiArea"));
    mMdiArea->setMinimumSize(QSize(500, 400));
    mMdiArea->setViewMode( QMdiArea::SubWindowView );
    gridLayout->addWidget( mMdiArea, 0, 0, 1, 1 );

	mMapDock = new CTabbedDock( "Main Map", this );
	mMapDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
	mMapDock->setMaximumWidth( max_main_dock_width );
	parent->addDockWidget( Qt::LeftDockWidgetArea, mMapDock );

	mMapDock->AddTab( mMap, "Navigator" );

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
