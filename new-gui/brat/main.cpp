#include "stdafx.h"

#include <QMainWindow>
#include <QStyle>
#include <QDesktopWidget>

#include "new-gui/QtInterface.h"
#include "new-gui/Common/QtUtilsIO.h"

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
#include "GUI/norwegianwoodstyle.h"

//#define MAP_WIDGET
//#define GLOBE_WIDGET
//#define PLOT_2D_WIDGET
//#define PLOT_3D_WIDGET
//#define DISPLAY_EDITORS
//#define DESKTOP_MANAGERS


//void printf_( const char* s )
//{
//	while ( s && *s ) 
//	{
//		if ( *s == '%' && *++s != '%' )	// make sure that there wasn't meant to be more arguments; %% represents plain % in a format string
//			throw std::runtime_error( "invalid format: missing arguments" );
//
//		qDebug() << *s++;
//	}
//}
//
//template<typename T, typename... Args>				// note the "..."
//void printf_( const char* s, T value, Args... args )	// note the "..."
//{
//	while ( s && *s ) 
//	{
//		if ( *s == '%' && *++s != '%' ) 	// a format specifier (ignore which one it is)
//		{
//			qDebug() << value;				// use first non-format argument
//			printf_( ++s, args... ); 		// ``peel off'' first argument
//			return;
//		}
//		qDebug() << *s++;
//	}
//	throw std::runtime_error( "extra arguments provided to printf" );
//}
//
//
//#define kv std::make_pair
//
////template< typename... Types >
////using ttt = std::pair< std::string, Types... >;
//
//template< typename T >
//T t2q( T &a )
//{
//	return a;
//}
//
//bool WriteIni( int settings )
//{
//	return settings > 0;
//}
//template< typename T, typename... Types >
//bool WriteIni( int settings, const kv_t< T > &first, Types... args )
//{
//	//if( sizeof...( args ) == 0 )
//	//	return settings > 0;
//
//	qDebug() << first.first.c_str();
//	qDebug() << t2q( first.second );
//	return WriteIni( settings, args... );
//}

void f( kv_t< bool& > k )
{
	k.second = false;
}


int main( int argc, char *argv[] )
{
	//bool is = true;
	//kv_t< bool& > k = kv_t<bool&>( std::string(""), is );
	//f( k );
	//std::cout << k.first << std::endl;
	//std::cout << k.second << std::endl;

	//return 0;


	//auto a1 = kv_t< int >( "1", 14 );
	//auto a2 = kv_t< char >( "2", '1' );
	//auto a3 = kv_t< std::string >( "3", "rabbits" );

	//if ( !WriteIni( 
	//	13, 
	//	kv( std::string("1"), 14 ),
	//	kv( std::string("2"), '1' ),
	//	kv( std::string("3"), "rabbits" )
	//	) )
	////if ( !WriteIni< int, std::pair<std::string, int > >( 
	////	13, 
	////	{ std::string("picas"), 56 }, 
	////	{ std::string("pocas"), 9.0 }, 
	////	{ std::string("pucas"), 'c' }, 
	////	{ std::string("pacas"), "conch" } 
	////	) )
	//	throw std::runtime_error( "WriteIni returned false" );

	//printf_("fesm %d %s %g", 456, "countable", 3.14 );

	//return 0;

	//QCoreApplication::setOrganizationName( "ESA" );
	////QCoreApplication::setOrganizationDomain( "mysoft.com" );
	//QCoreApplication::setApplicationName( "brat" );
	//QSettings::setDefaultFormat( QSettings::IniFormat );
	//QSettings settings;
	//qDebug() << settings.format();
	//qDebug() << settings.fileName();
	//return 0;


    CBratApplication a( argc, argv, true );
	//QApplication::setStyle(new NorwegianWoodStyle);		//joke

	//CConfiguration* m_config = new CConfiguration( "L:\\project\\workspaces\\W1\\Datasets\\setup.ini" );
	//CConfiguration* m_config = new CConfiguration( "L:\\project\\workspaces\\S3A\\Datasets\\setup.ini" );
	//CDataset *data = new CDataset("Datasets_FM_1");
	//data->LoadConfig( m_config );

	CBratMainWindow w( a.Settings() );

	//QMainWindow w;
    //QWidget *central = nullptr;
	
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

    //CMapWidget *centralMap = w.mManager->Map();			//new CMapWidget( nullptr );
	central = w.mManager;		// new manager_t( /*centralMap, */&w );

	CAbstractDisplayEditor *editors[ 3 ] = { 
		new CMapEditor( nullptr ), 
		new CPlotEditor( nullptr ), 
		new CPlotEditor( nullptr ) 
		//new CMapEditor( nullptr ), 
	};
	editors[ 0 ]->AddView( new CMapWidget( editors[ 0 ] ) );
	editors[ 1 ]->AddView( new C2DPlotWidget( ePlotDefault, editors[ 1 ] ) );
	editors[ 2 ]->AddView( new C3DPlotWidget( editors[ 2 ] ) );
	//editors[ 3 ]->AddView( new CGlobeWidget( editors[ 3 ], ApplicationDirectories::instance().mGlobeDir, centralMap ) );
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
