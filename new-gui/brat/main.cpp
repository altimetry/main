#include "stdafx.h"

#include <QMainWindow>
#include <QStyle>
#include <QDesktopWidget>

#include "new-gui/QtInterface.h"
#include "new-gui/Common/QtUtils.h"
#include "new-gui/Common/QtFileUtils.h"

#include "Workspaces/Dataset.h"
#include "BratApplication.h"
#include "BratMainWindow.h"

/*	
	TODO: unit tests out of this

	std::string dir = "L:\\project\\workspaces\\S3A - Copy";			//l:/P/Q/R/xyz
	//std::string dir = "l:////P\\\\Q\\\\F//..\\R\\xyz/\\/\\\\//";		//l:/P/Q/R/xyz
	std::string path = "/A/B";

	//CleanPath( dir );
	//qDebug() << CannonicalPath( dir ).c_str();
	//qDebug() << CleanedPath( dir ).c_str();
	NormalizePath( dir );
	qDebug() << dir.c_str();

	MakeDirectory( dir + path );

	//NormalizePath( dir );
	//qDebug() << dir.c_str();

	//QDir d( directory.c_str() );
	//QDir d;
    //return d.rmpath( (dir + path).c_str() );
	DeleteDirectory( dir );

	//DeletePath( dir, path );

	return 0;
*/

#include "Views/MapWidget.h"
#include "Views/GlobeWidget.h"
#include "Views/2DPlotWidget.h"
#include "Views/3DPlotWidget.h"

#include "GUI/DisplayEditor.h"
#include "GUI/DesktopManager.h"

//#define MAP_WIDGET
//#define GLOBE_WIDGET
//#define PLOT_2D_WIDGET
//#define PLOT_3D_WIDGET
//#define DISPLAY_EDITORS
#define DESKTOP_MANAGERS


int main( int argc, char *argv[] )
{
    CBratApplication a(argc, argv, true);

	//CConfiguration* m_config = new CConfiguration( "L:\\project\\workspaces\\W1\\Datasets\\setup.ini" );
	//CConfiguration* m_config = new CConfiguration( "L:\\project\\workspaces\\S3A\\Datasets\\setup.ini" );
	//CDataset *data = new CDataset("Datasets_FM_1");
	//data->LoadConfig( m_config );

    CBratMainWindow w;

	//QMainWindow w;
	QWidget *central = nullptr;
	
#ifdef MAP_WIDGET
	// Map Widget

	central = new CMapWidget( &w );
#endif

#ifdef GLOBE_WIDGET
	// Globe Widget

	central = new CGlobeWidget( &w, ApplicationDirectories::instance().mGlobeDir, dynamic_cast< CMapWidget* >( central ) );
#endif

#ifdef PLOT_2D_WIDGET
	// 2D Plot Widget

	central = new C2DPlotWidget( &w );
#endif

#ifdef PLOT_3D_WIDGET
	// 3D Plot Widget

	central = new C3DPlotWidget( &w );
#endif

#ifdef DISPLAY_EDITORS
	// Display Editors

	QWidget *parent = nullptr;
    //central = new QMdiArea( &w );
    //central = new QScrollArea( &w );
	//central = new QFrame( &w );
    //central = new QWidget( &w );
    //QHBoxLayout *hl = new QHBoxLayout( central );
	//delete central->layout();
	//central->setLayout( nullptr);
    //central = &w;

	typedef QDialog sub_window_t;
    central = new CMapWidget( &w );
    for ( int i = 0; i < 4; ++i)
    {
        sub_window_t *dlg1 = new sub_window_t( central );
        dlg1->setWindowFlags( dlg1->windowFlags() | Qt::WindowStaysOnTopHint);     //mac only: works
        //dlg1->setWindowFlags( dlg1->windowFlags() | Qt::Tool );     //mac only, also works
		parent = dlg1;
        dlg1->setWindowTitle( QString( t2q( n2s<std::string>( i ) ) ) + "[*]" );
		CAbstractDisplayEditor *d1 = nullptr;
		if ( i % 2 )
		{
			d1 = new CMapEditor( parent );
			if ( i == 1 )
				d1->AddView( new CGlobeWidget( d1, ApplicationDirectories::instance().mGlobeDir, dynamic_cast<CMapWidget*>( central ) ) );
			else
				d1->AddView( new CMapWidget( d1 ) );
		}
		else
		{
			d1 = new CPlotEditor( parent );
			if ( i == 2 )
				d1->AddView( new C2DPlotWidget( ePlotDefault, d1 ) );
			else
				d1->AddView( new C3DPlotWidget( d1 ) );
		}
        addWidget( dlg1, d1 );
        //dlg1->setVisible( true );
    }

    //for ( int i = 0; i < 3; ++i)
    //{
    //    QDockWidget *dock1 = new QDockWidget( &w );								//note that docks already have layouts and Qt warns about that
    //    dock1->setWindowTitle( QString( t2q( n2s<std::string>( i ) ) ) + "[*]" );
    //    dock1->setAllowedAreas( Qt::DockWidgetArea::RightDockWidgetArea );
    //    w.addDockWidget( Qt::DockWidgetArea::RightDockWidgetArea, dock1 );
    //    //QWidget *w1 = new CGlobeWidget( &w, ApplicationDirectories::instance().mGlobeDir, dynamic_cast< CMapWidget* >( central ) ) ;
    //    QWidget *w1 = new C3DPlotWidget( &w ) ;
    //    dock1->setWidget( new CMapEditor( parent, w1 ) );
    //    dock1->setVisible( true );
    //}

    parent = central;
//    QWidget *w1 = new CGlobeWidget( &w, ApplicationDirectories::instance().mGlobeDir, dynamic_cast< CMapWidget* >( central ) ) ;
//    QWidget *w2 = new CGlobeWidget( &w, ApplicationDirectories::instance().mGlobeDir, dynamic_cast< CMapWidget* >( central ) ) ;
//    QWidget *w3 = new CGlobeWidget( &w, ApplicationDirectories::instance().mGlobeDir, dynamic_cast< CMapWidget* >( central ) ) ;

    //QWidget *w1 = new CDisplayEditor( parent, new C3DPlotWidget( &w ) );

    //parent = new QDialog( &w );
    //((QMdiArea*)central)->addSubWindow( w1 );
	//parent = new QDialog( &w );
    //((QMdiArea*)central)->addSubWindow( w2 );
	//parent = new QDialog( &w );
    //((QMdiArea*)central)->addSubWindow( w3 );

//    new CDisplayEditor( parent, new C3DPlotWidget( &w ) );
//    new CDisplayEditor( parent, new C3DPlotWidget( &w ) );
//    new CDisplayEditor( parent, new C3DPlotWidget( &w ) );

//    new CDisplayEditor( parent, w1 );
//    new CDisplayEditor( parent, w2 );
//    new CDisplayEditor( parent, w3 );

//    hl->addWidget( new CDisplayEditor( parent, w1 ) );
//    hl->addWidget( new CDisplayEditor( parent, w2 ) );
//    hl->addWidget( new CDisplayEditor( parent, w3 ) );
//    central->setLayout(hl);

    //central = nullptr;

#endif

#ifdef DESKTOP_MANAGERS

    //w.showMaximized();
    w.show();

	//typedef CDesktopManagerMDI manager_t;

	CMapWidget *centralMap = w.mManager->Map();			//new CMapWidget( nullptr );
	central = w.mManager;		// new manager_t( /*centralMap, */&w );

	CAbstractDisplayEditor *editors[ 4 ] = { 
		new CMapEditor( nullptr ), 
		new CMapEditor( nullptr ), 
		new CPlotEditor( nullptr ), 
		new CPlotEditor( nullptr ) 
	};
	editors[ 0 ]->AddView( new CGlobeWidget( editors[ 0 ], ApplicationDirectories::instance().mGlobeDir, centralMap ) );
	editors[ 1 ]->AddView( new CMapWidget( editors[ 1 ] ) );
	editors[ 2 ]->AddView( new C2DPlotWidget( ePlotDefault, editors[ 2 ] ) );
	editors[ 3 ]->AddView( new C3DPlotWidget( editors[ 3 ] ) );
	for ( auto ed : editors )
	{
		auto subWindow = qobject_cast<decltype( w.mManager )>( central )->AddSubWindow( ed );
		subWindow->show();
	}

#else

	w.show();

#endif

	//if ( central && central != &w )
	//	w.setCentralWidget( central );
 //	
 //   CenterOnScreen( &w )->show();
	//QObjectList widgetList = central->children();
	//for ( auto ii = widgetList.begin(); ii != widgetList.end(); ++ii )
	//{
	//	QWidget *w = qobject_cast<decltype(w)>( *ii );
	//	if (w)
	//		w->setVisible( true );
	//}

    return a.exec();
}
