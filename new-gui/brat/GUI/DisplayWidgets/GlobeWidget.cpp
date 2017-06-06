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

#if defined (WIN32) || defined(_WIN32)
#pragma warning ( disable: 4100 )           //unreferenced formal parameter
#endif


#include <osgGA/StateSetManipulator>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osgEarthQt/ViewerWidget>

#include <osgEarthUtil/GeodeticGraticule>
#include <osgEarthUtil/MGRSFormatter>
#include <osgEarthUtil/MGRSGraticule>
#include <osgEarthUtil/UTMGraticule>

#include <osgEarthUtil/MouseCoordsTool>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarthUtil/VerticalScale>
#include <osgEarthUtil/Sky>						//femm	SkyNode => Sky
#include <osgEarthUtil/AutoClipPlaneHandler>

#include <osgEarthDrivers/tms/TMSOptions>
#include <osgEarthDrivers/cache_filesystem/FileSystemCache>

using namespace osgEarth::Drivers;
using namespace osgEarth::Util;

#include <osgEarthUtil/Controls>
using namespace osgEarth::Util::Controls;

#include <qgsdistancearea.h>

#include "Globe/qgsosgearthtilesource.h"


#if defined (WIN32) || defined(_WIN32)
#pragma warning ( default: 4100 )           //unreferenced formal parameter
#endif



#include "Globe/GlobeControls.h"

#include "new-gui/Common/QtUtils.h"

#include "PSProcessing.h"
#include "MapWidget.h"
#include "GlobeWidget.h"




///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Globe
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//		Globe statics
///////////////////////////////////////////////////////////////////////////////////////////////////////////



void CGlobeWidget::SetOSGDirectories( const std::string &GlobeDir )
{
	CGlobeControls::SetControlsDirectory( GlobeDir );
}




///////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Globe construction - I. Graticule
///////////////////////////////////////////////////////////////////////////////////////////////////////////


struct ToggleGraticuleHandler : public ControlEventHandler
{
    ToggleGraticuleHandler( GraticuleNode* graticule ) : _graticule( graticule ) { }

    void onValueChanged( Control* control, bool value )
    {
        _graticule->setVisible( value );
    }

    GraticuleNode* _graticule;
};

struct OffsetGraticuleHandler : public ControlEventHandler
{
    OffsetGraticuleHandler( GraticuleNode* graticule, const osg::Vec2f& offset ) :
        _graticule( graticule ),
        _offset(offset)
    {
        //nop
    }

    void onClick( Control* control, const osg::Vec2f& pos, int mouseButtonMask )
    {
        _graticule->setCenterOffset( _graticule->getCenterOffset() + _offset );
    }

    osg::Vec2f _offset;
    GraticuleNode* _graticule;
};

void CGlobeWidget::ToggleGridEnabled( bool toggle )
{
	assert__( mGraticuleNode.get() );

	mGraticuleNode->setVisible( toggle );
}


GraticuleNode* CGlobeWidget::CreateGraticule()
{
    Formatter* formatter = 0L;
	
	bool isUTM = false;
	bool isMGRS = false;
	bool isGeodetic = false;

	GraticuleNode *gn = nullptr;

    if ( isUTM )
    {
        UTMGraticule* gr = new UTMGraticule( mMapNode );
        mRootNode->addChild( gr );
        formatter = new MGRSFormatter();
    }
    else if ( isMGRS )
    {
        MGRSGraticule* gr = new MGRSGraticule( mMapNode );
        mRootNode->addChild( gr );
        formatter = new MGRSFormatter();
    }
    else if ( isGeodetic )
    {
        GeodeticGraticule* gr = new GeodeticGraticule( mMapNode );
        GeodeticGraticuleOptions o = gr->getOptions();
        o.lineStyle()->getOrCreate<LineSymbol>()->stroke()->color().set(1,0,0,0);
        o.textStyle()->getOrCreate<TextSymbol>()->halo()->color().set(0,0,0,1);
        //o.textStyle()->getOrCreate<CharSymbol>()->font()->color().set(1,1,1,1);
		//const_cast< std::vector<GeodeticGraticuleOptions::Level>& >( o.levels() ).clear();
		//o.addLevel( 0.9, 0.1, -1 );
		//o.addLevel( 0.0, 0.0, -1 );
        gr->setOptions( o );
        mRootNode->addChild( gr );
        formatter = new LatLongFormatter();
    }
    else
    {
        GraticuleOptions o;
        o.color() = Color(0.7,0.7,0.7,1);
        o.labelColor() = Color(0,0,0,1);
		o.lineWidth() = 2.;
		o.gridLines() = 10;
		o.resolutions() = "10 5 2.5 1.25 1. 0.5 0.25 0.125 0.1 0.05 0.025 0.01 0.001";
        gn = new GraticuleNode( mMapNode, o );
		gn->setVisible( false );
    }

	return gn;
}

void CGlobeWidget::setBaseMap( QString url )
{
	if ( mMapNode )
	{
		mMapNode->getMap()->removeImageLayer( mBaseLayer );
		if ( url.isNull() )
		{
			mBaseLayer = 0;
		}
		else
		{
			TMSOptions imagery;
			imagery.url() = url.toStdString();
			mBaseLayer = new ImageLayer( "Imagery", imagery );
			mMapNode->getMap()->insertImageLayer( mBaseLayer, 0 );
		}
	}
}

void CGlobeWidget::SetupMap( bool enable_base_map )	//it needs renderer in tile source
{
	QString cacheDirectory = QgsApplication::qgisSettingsDirPath() + "cache";
	FileSystemCacheOptions cacheOptions;
	cacheOptions.rootPath() = cacheDirectory.toStdString();

	MapOptions mapOptions;
	mapOptions.cache() = cacheOptions;
	osgEarth::Map *map = new osgEarth::Map( mapOptions );

	MapNodeOptions nodeOptions;
	TerrainOptions terrainOptions;
	nodeOptions.setTerrainOptions( terrainOptions );

	// The MapNode will render the Map object in the scene graph.
	mMapNode = new osgEarth::MapNode( map, nodeOptions );

	if ( enable_base_map )
	{
		setBaseMap( "http://readymap.org/readymap/tiles/1.0.0/7/" );
	}

	mRootNode->addChild( mMapNode );

	mGraticuleNode = CreateGraticule();
	mRootNode->addChild( mGraticuleNode );

	// Add layers to the map

	imageLayersChanged();

    // And what remains of
#if defined(USE_DATA_VISUALIZAAION)
    elevationLayersChanged();
#else
    assert__( mMapNode->getMap()->getNumElevationLayers() == 0 );

    ElevationLayerVector list;
    map->getElevationLayers( list );		assert__( list.size() == 0 );

    double scale = 1.;
    setVerticalScale( scale );
#endif
}



//else if ( "Worldwind" == type )
//elevationPath->setText( "http://tileservice.worldwindcentral.com/getTile?bmng.topo.bathy.200401" );
//else if ( "TMS" == type )
//elevationPath->setText( "http://readymap.org/readymap/tiles/1.0.0/9/" );

void CGlobeWidget::elevationLayersChanged()
{
    QgsDebugMsg( "elevationLayersChanged: Globe Running, executing" );
    osg::ref_ptr<Map> map = mMapNode->getMap();

    if ( map->getNumElevationLayers() > 1 )
    {
        mOsgViewer->getDatabasePager()->clear();
    }

    // Remove elevation layers
    ElevationLayerVector list;
    map->getElevationLayers( list );
    for ( ElevationLayerVector::iterator i = list.begin(); i != list.end(); ++i )
    {
        map->removeElevationLayer( *i );
    }

    // Add elevation layers
    QSettings settings;
    QString cacheDirectory = settings.value( "cache/directory", QgsApplication::qgisSettingsDirPath() + "cache" ).toString();
    ElevationLayer *layer = nullptr;
//    QTableWidget* table = mSettingsDialog->elevationDatasources();
//    for ( int i = 0; i < table->rowCount(); ++i )
//    {
//        QString type = table->item( i, 0 )->text();
//        //bool cache = table->item( i, 1 )->checkState();
//        QString uri = table->item( i, 2 )->text();
//        ElevationLayer* layer = 0;

//        if ( "Raster" == type )
//        {
//            GDALOptions options;
//            options.url() = uri.toStdString();
//            layer = new osgEarth::ElevationLayer( uri.toStdString(), options );
//        }
//        else if ( "TMS" == type )
//        {

    std::string p2 = "http://readymap.org/readymap/tiles/1.0.0/9/";
    //std::string p2 = "http://tileservice.worldwindcentral.com/getTile?bmng.topo.bathy.200401";
	//std::string p2 = "http://tileservice.worldwindcentral.com/getTile?interface=map&version=1&dataset=bmng.topo.bathy.200401&level=0&x=0&y=0";

            TMSOptions options;
            options.url() = p2;       //uri.toStdString();
            layer = new osgEarth::ElevationLayer( p2/*uri.toStdString()*/, options );
//        }
        map->addElevationLayer( layer );

        //if ( !cache || type == "Worldwind" ) layer->setCache( 0 ); //no tms cache for worldwind (use worldwind_cache)
//    }
#if OSGEARTH_VERSION_GREATER_OR_EQUAL( 2, 5, 0 )
    //double scale = QgsProject::instance()->readDoubleEntry( "Globe-Plugin", "/verticalScale", 1 );
    setVerticalScale( 10./*scale */);
#endif
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Globe construction - II. structures created by the constructor
///////////////////////////////////////////////////////////////////////////////////////////////////////////


struct PrintCoordsToStatusBar : public osgEarth::Util::MouseCoordsTool::Callback
{
	using base_t = osgEarth::Util::MouseCoordsTool::Callback;

	CMapWidget *mCanvas = nullptr;

public:
	PrintCoordsToStatusBar( CMapWidget *the_canvas ) 
		: base_t()
		, mCanvas( the_canvas ) 
	{}

	virtual ~PrintCoordsToStatusBar()
	{}

	virtual void set( const GeoPoint &p, osg::View *view, MapNode *mapNode ) override
	{
		if ( !mCanvas->mapSettings().destinationCrs().isValid() )
			return;

		std::string str = osgEarth::Stringify() << p.y() << ", " << p.x();
		//mCoordsEdit->setText( QString( str.c_str() ) );
		//qDebug() << str.c_str();
		//mCanvas->ShowMouseCoordinate( QgsPoint( p.x(), p.y() ) );
		mCanvas->ShowMouseDegreeCoordinates( QgsPoint( p.x(), p.y() ), false, true );
		//mCanvas->ShowMouseCoordinate( QString( str.c_str() ) );
	}

	virtual void reset( osg::View* view, MapNode* mapNode ) override
	{
		//mCoordsEdit->setText( QString( "out of range" ) );
		mCanvas->ShowMouseCoordinate( "", true );
	}
};


class CBratMouseCoordsTool : public MouseCoordsTool
{
	using base_t = MouseCoordsTool;

	QPoint mLastMousePoint;

public:
	explicit CBratMouseCoordsTool( CMapWidget *the_canvas, MapNode *mapNode, LabelControl *label = nullptr, Formatter *formatter = nullptr )
		:base_t( mapNode, label, formatter )
	{
		addCallback( new PrintCoordsToStatusBar( the_canvas ) );
	}

    virtual ~CBratMouseCoordsTool()
	{}


	const QPoint& LastMousePoint() const { return mLastMousePoint; }


	virtual bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa ) override
	{
		if ( ea.getEventType() == ea.MOVE || ea.getEventType() == ea.DRAG )
		{
			mLastMousePoint = QCursor::pos();
			//mLastMousePoint.setX( std::round( ea.getX() ) );
			//mLastMousePoint.setY( std::round( ea.getY() ) );
		}
		return base_t::handle( ea, aa );
	}
};





class GlobeViewer : public osgViewer::Viewer
{
	using base_t = osgViewer::Viewer;

public:

	GlobeViewer()
		: base_t()
	{}

	virtual int run()
	{
		return base_t::run();
	}
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		Globe Construction
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


class CGlobeViewerWidget : public osgEarth::QtGui::ViewerWidget
{
	using base_t = osgEarth::QtGui::ViewerWidget;

	QgsMapCanvas *mCanvas = nullptr;

	bool mPainting = false;
	bool mPaused = false;

public:
	CGlobeViewerWidget( QgsMapCanvas *the_canvas, osgViewer::ViewerBase *viewer )
		: base_t( viewer )
		, mCanvas( the_canvas )
		, mPainting( false )
		, mPaused( false )
	{
		assert__( Viewer() == viewer );
	}
	
	virtual ~CGlobeViewerWidget()
	{
		Stop();
	}


	bool Painting() const
	{
		return mPainting;
	}

	void Stop()
	{
		mPaused = true;
		_timer.stop();
		Viewer()->getDatabasePager()->cancel();
		_viewer->setDone( true );
	}

	void Pause()
	{
		if ( mPaused )
			return;
		mPaused = true;
		_timer.stop();
		PausePager();
	}

	void Resume()
	{
		if ( !mPaused )
			return;
		mPaused = false;
		_timer.start(20);
		ResumePager();
	}

protected:
	void PausePager()
	{
		Viewer()->getDatabasePager()->setDatabasePagerThreadPause( true );
	}
	void ResumePager()
	{
		Viewer()->getDatabasePager()->setDatabasePagerThreadPause( false );
	}

	osgViewer::Viewer* Viewer()
	{
		return dynamic_cast< osgViewer::Viewer* >( _viewer.get() );
	}

	bool IsPaused() const
	{
		assert__( ( mPaused && !_timer.isActive() || !mPaused && _timer.isActive() ) );

		return mPaused;
	}

	virtual void paintEvent( QPaintEvent *event ) override
	{
		mPainting = true;

		//	qDebug() << Viewer()->getDatabasePager()->isRunning(); alway reports true

		PausePager();

		if ( mPaused )
		{
			return;
		}

		bool map_frozen = false;
		if ( mCanvas->isDrawing() )
		{
			return;
		}
		else
		{
			mCanvas->stopRendering();
			map_frozen = mCanvas->isFrozen();
			if ( !map_frozen )
				mCanvas->freeze( true );
			ResumePager();
		}

		base_t::paintEvent( event );

		PausePager();
		mCanvas->freeze( map_frozen );

		mPainting = false;
	}
};


bool CGlobeWidget::Rendering() const
{
	return mGlobeViewerWidget->Painting();
}
void CGlobeWidget::Pause()
{
	mGlobeViewerWidget->Pause();
}
void CGlobeWidget::Resume( bool layers_changed )
{
	if ( layers_changed )
		ChangeImageLayers();		// imageLayersChanged();
	mGlobeViewerWidget->Resume();
}


bool CGlobeWidget::ScheduleClose()
{
	mCloseScheduled = true;
	mTileSource->mStop = true;
	mGlobeViewerWidget->Stop();
	QTimer::singleShot( 1, this, SLOT( close() ) );
	return !mGlobeViewerWidget->Painting();
}


void CGlobeWidget::Home()
{
	mOsgViewer->getCameraManipulator()->home( 0. );	//argument is not used
}


bool CGlobeWidget::Save2Image( const QString &path, const QString &format, const QString &extension )
{
	while ( mCanvas->isDrawing() && Rendering() )			//this is very uncertain...
		qApp->processEvents();

	static const bool with_alpha = true;

	QString qpath = path;
	QString f = format.toLower();
	if (!extension.isEmpty())
		SetFileExtension( qpath, extension );
	QImage img = mGlobeViewerWidget->grabFrameBuffer( with_alpha );
	if ( f == "ps" )
	{
#if QT_VERSION < 0x050000
        QPrinter printer;
        printer.setOutputFormat( QPrinter::PostScriptFormat );
        printer.setPaperSize( QSizeF( 80, 80 ), QPrinter::Millimeter );
        printer.setCreator( BRAT_VERSION_STRING );
        printer.setOrientation( QPrinter::Landscape );

        printer.setOutputFileName( qpath );
        printer.setColorMode( QPrinter::Color );

		QString docName = windowTitle();
		if ( !docName.isEmpty() )
		{
			docName.replace( QRegExp( QString::fromLatin1( "\n" ) ), tr( " -- " ) );
			printer.setDocName( docName );
		}

		QPainter painter( &printer );
		painter.drawPixmap( 0, 0, -1, -1, QPixmap::fromImage( img ) );	//pixmap.save( qpath, "SVG" );		//fails

		return true;
#else
		QTemporaryFile file;
		if (file.open()) 
		{
			const QString &in_path = file.fileName();
			file.close();
			return 
				Save2Image( in_path, "jpg", "" )
				&&
				Convert2PostScript( q2a( in_path ), q2a( path ), q2a( windowTitle() ) );
		}
#endif

	}

	return img.save( qpath, q2a( format ).c_str(), 100 );	//int quality = -1
}




//virtual 
CGlobeWidget::~CGlobeWidget()
{
}


CGlobeWidget::CGlobeWidget( QWidget *parent, CMapWidget *the_canvas )
	: base_t( parent )
	, mCanvas( the_canvas )
{
	assert__( !mOsgViewer );
	assert__( !mRootNode );
	assert__( !mMapNode );
	assert__( !mBaseLayer );
	assert__( !mQgisMapLayer );
	assert__( !mTileSource );
	assert__( !mControlCanvas );
	assert__( !mSelectedLat );
	assert__( !mSelectedLon );
	assert__( !mSelectedElevation );

	// update path to OSG plug-ins on Mac OS X
	//
#ifdef Q_OS_MACX
	if ( !getenv( "OSG_LIBRARY_PATH" ) )
	{
		// OSG_PLUGINS_PATH value set by CMake option
		QString ogsPlugins( OSG_PLUGINS_PATH );
		QString bundlePlugins = QgsApplication::pluginPath() + "/../osgPlugins";
		if ( QFile::exists( bundlePlugins ) )
		{
			// add internal osg plugin path if bundled osg
			ogsPlugins = bundlePlugins;
		}
		if ( QFile::exists( ogsPlugins ) )
		{
			osgDB::Registry::instance()->setLibraryFilePathList( QDir::cleanPath( ogsPlugins ).toStdString() );
		}
	}
#endif

	mRootNode = new osg::Group();		//from setupMap

	mOsgViewer = new GlobeViewer();	// osgViewer::Viewer();
	mOsgViewer->setThreadingModel( osgViewer::Viewer::SingleThreaded );

	// install the programmable manipulator.
	//
	osgEarth::Util::EarthManipulator *manip = new osgEarth::Util::EarthManipulator();
	manip->getSettings()->bindScroll( osgEarth::Util::EarthManipulator::ACTION_ZOOM_OUT,  osgGA::GUIEventAdapter::SCROLL_DOWN ); 
	manip->getSettings()->bindScroll( osgEarth::Util::EarthManipulator::ACTION_ZOOM_IN, osgGA::GUIEventAdapter::SCROLL_UP ); 
	mOsgViewer->setCameraManipulator( manip );

	SetupMap( false );		//creates and attaches the map node, graticule, layers...


	// v4 setSkyParameters was called here
	SetSkyParameters( true );

	// create a surface to house the controls
	//
	mControlCanvas = ControlCanvas::get( mOsgViewer );		//OSGEARTH_VERSION_GREATER_OR_EQUAL( 2, 1, 1 )
	mRootNode->addChild( mControlCanvas );


	//	Root node
	//
	mOsgViewer->setSceneData( mRootNode );

	mOsgViewer->addEventHandler( new osgViewer::StatsHandler() );
	mOsgViewer->addEventHandler( new osgViewer::WindowSizeHandler() );
	mOsgViewer->addEventHandler( new osgViewer::ThreadingHandler() );
	mOsgViewer->addEventHandler( new osgViewer::LODScaleHandler() );
	mOsgViewer->addEventHandler( new osgGA::StateSetManipulator( mOsgViewer->getCamera()->getOrCreateStateSet() ) );

	mOsgViewer->getCamera()->addCullCallback( new AutoClipPlaneCullCallback( mMapNode ) );	//NOT OSGEARTH_VERSION_LESS_THAN( 2, 2, 0 )

	// osgEarth benefits from pre-compilation of GL objects in the pager. In newer versions of
	// OSG, this activates OSG's IncrementalCompileOpeartion in order to avoid frame breaks.
	//
	mOsgViewer->getDatabasePager()->setDoPreCompile( true );				//ATTENTION: original is "true"
	qDebug() << mOsgViewer->getDatabasePager()->getNumDatabaseThreads();	//2
	//mOsgViewer->getDatabasePager()->setUpThreads( 0, 0 );					//leaves 1 thread

	// Set a home viewpoint
	//	- changed from the original: osgEarth::Util::Viewpoint( osg::Vec3d( -90, 0, 0 ), 0.0, -90.0, 2e7 )

	manip->setHomeViewpoint( osgEarth::Util::Viewpoint( nullptr, 0, 0, 0, 0.0, -90.0, 2e7 ), 1.0 );

	// add our handlers
	mOsgViewer->addEventHandler( new FlyToExtentHandler( this ) );
	mOsgViewer->addEventHandler( new KeyboardControlHandler( manip ) );

	mMouseCoordsTool = new CBratMouseCoordsTool( mCanvas, mMapNode );
	mOsgViewer->addEventHandler( mMouseCoordsTool );


	mControls = new CGlobeControls( this );
#if defined(USE_DATA_VISUALIZAAION)
	SetupControls( true );
#else
	SetupControls( false );
#endif

	mGlobeViewerWidget = new CGlobeViewerWidget( mCanvas, mOsgViewer );	

	osg::Camera* camera = mOsgViewer->getCamera();
	osgQt::GraphicsWindowQt *gw = dynamic_cast< osgQt::GraphicsWindowQt* >( camera->getGraphicsContext() );
	gw->getGLWidget()->setMinimumSize( 10, 10 );	//CRITICAL: prevent image from disappearing forever when height is 0

	//anti-aliasing
	{
		QGLFormat glf = QGLFormat::defaultFormat();
		glf.setSampleBuffers( true );
		mGlobeViewerWidget->setFormat( glf );
	}
	AddWidget( this, mGlobeViewerWidget );

	Wire();
}


//////////////// Globe Tips /////////////////////////////////////////////////////////////////////////////
//	TODO: for this to work, searchRadiusMU for globe layer would have to be found and passed to CMapTip
//	- activate in CMapWidget::SetNextTip
//
#include "MapTip.h"
void CGlobeWidget::HandleMapTipTriggerd( CMapTip *maptip, QgsMapLayer *layer, QgsPoint &map_position )
{
	if ( underMouse() )
	{
		maptip->ShowMapTip( layer, map_position, mapFromGlobal( mMouseCoordsTool->LastMousePoint() ), mCanvas, this );
	}
}
//
//////////////// Globe Tips /////////////////////////////////////////////////////////////////////////////



void CGlobeWidget::Unwire()
{
	disconnect( this, SIGNAL( xyCoordinates( const QgsPoint & ) ),	mCanvas, SIGNAL( xyCoordinates( const QgsPoint & ) ) );
	disconnect( mCanvas, SIGNAL( GridEnabled( bool ) ),			this, SLOT( ToggleGridEnabled( bool ) ) ); 

	disconnect( mCanvas, SIGNAL( extentsChanged() ),			this, SLOT( extentsChanged() ) );

	disconnect( mCanvas, SIGNAL( layersChanged() ),				this, SLOT( imageLayersChanged() ) );

	disconnect( mCanvas, SIGNAL( MapTipTriggerd( CMapTip *, QgsMapLayer *, QgsPoint &) ),	this, SLOT( HandleMapTipTriggerd( CMapTip *, QgsMapLayer *, QgsPoint &) ) );

	//connect( mCanvas, SIGNAL( renderStarting() ), this, SLOT( CanvasStarted() ) );
	//connect( mCanvas, SIGNAL( mapCanvasRefreshed() ), this, SLOT( CanvasFinished() ) );
}

void CGlobeWidget::Wire()
{
	connect( this, SIGNAL( xyCoordinates( const QgsPoint & ) ),	mCanvas, SIGNAL( xyCoordinates( const QgsPoint & ) ) );
	connect( mCanvas, SIGNAL( GridEnabled( bool ) ),			this, SLOT( ToggleGridEnabled( bool ) ) ); 

	connect( mCanvas, SIGNAL( extentsChanged() ),				this, SLOT( extentsChanged() ) );
	connect( mCanvas, SIGNAL( layersChanged() ),				this, SLOT( imageLayersChanged() ) );

	connect( mCanvas, SIGNAL( MapTipTriggerd( CMapTip *, QgsMapLayer *, QgsPoint &) ),	this, SLOT( HandleMapTipTriggerd( CMapTip *, QgsMapLayer *, QgsPoint &) ) );

	//connect( mCanvas, SIGNAL( renderStarting() ), this, SLOT( CanvasStarted() ) );
	//connect( mCanvas, SIGNAL( mapCanvasRefreshed() ), this, SLOT( CanvasFinished() ) );
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Slots
///////////////////////////////////////////////////////////////////////////////////////////////////////////



void CGlobeWidget::extentsChanged()
{
	assert__( mCanvas );

	QgsDebugMsg( "extentsChanged: " + mCanvas->extent().toString() );
}


void CGlobeWidget::ChangeImageLayers()
{
	if ( mCloseScheduled )
		return;

	if ( mTileSource && mTileSource->mRendering )
	{
		imageLayersChanged();
		return;
	}

	QgsDebugMsg( "imageLayersChanged: Globe Running, executing" );

	osg::ref_ptr<Map> map = mMapNode->getMap();
	if ( map->getNumImageLayers() > 1 )
	{
		mOsgViewer->getDatabasePager()->clear();
	}

	//remove QGIS layer
	if ( mQgisMapLayer )
	{
		QgsDebugMsg( "removeMapLayer" );
		map->removeImageLayer( mQgisMapLayer );
	}

	//add QGIS layer
	QgsDebugMsg( "addMapLayer" );
	mTileSource = new QgsOsgEarthTileSource( mCanvas );

	ImageLayerOptions options( "QGIS" );
	options.cachePolicy() = CachePolicy::NO_CACHE;
	mQgisMapLayer = new ImageLayer( options, mTileSource );

	map->addImageLayer( mQgisMapLayer );
}

void CGlobeWidget::imageLayersChanged()
{
	if ( mCloseScheduled )
		return;

	if ( mTileSource )
	{
		mTileSource->mStop = true;
		QTimer::singleShot( 1, this, SLOT( ChangeImageLayers() ) );
	}
	else
		ChangeImageLayers();
}


void CGlobeWidget::HandleSky( bool toggled )
{
	SetSkyParameters( toggled );
}

void CGlobeWidget::SetSkyParameters( bool sky )
{
	setSkyParameters( sky, QDateTime(), true );
}


void CGlobeWidget::SetupControls( bool set )
{
	assert__( mControls );

	set ? mControls->AddControls() : mControls->RemoveControls();
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////


void CGlobeWidget::showCurrentCoordinates( double lon, double lat )
{
  // show x y on status bar
  //OE_NOTICE << "lon: " << lon << " lat: " << lat <<std::endl;
  QgsPoint coord = QgsPoint( lon, lat );
  emit xyCoordinates( coord );
}

void CGlobeWidget::setSelectedCoordinates( osg::Vec3d coords )
{
  mSelectedLon = coords.x();
  mSelectedLat = coords.y();
  mSelectedElevation = coords.z();
  QgsPoint coord = QgsPoint( mSelectedLon, mSelectedLat );
  emit newCoordinatesSelected( coord );
}

osg::Vec3d CGlobeWidget::getSelectedCoordinates()
{
  osg::Vec3d coords = osg::Vec3d( mSelectedLon, mSelectedLat, mSelectedElevation );
  return coords;
}

void CGlobeWidget::showSelectedCoordinates()
{
  QString lon, lat, elevation;
  lon.setNum( mSelectedLon );
  lat.setNum( mSelectedLat );
  elevation.setNum( mSelectedElevation );
  QMessageBox m;
  m.setText( "selected coordinates are:\nlon: " + lon + "\nlat: " + lat + "\nelevation: " + elevation );
  m.exec();
}

double CGlobeWidget::getSelectedLon()
{
  return mSelectedLon;
}

double CGlobeWidget::getSelectedLat()
{
  return mSelectedLat;
}

double CGlobeWidget::getSelectedElevation()
{
  return mSelectedElevation;
}

void CGlobeWidget::syncExtent()
{
	assert__( mCanvas );

	const QgsMapSettings &mapSettings = mCanvas->mapSettings();
	QgsRectangle extent = mCanvas->extent();

	long epsgGlobe = 4326;
	QgsCoordinateReferenceSystem globeCrs;
	globeCrs.createFromOgcWmsCrs( QString( "EPSG:%1" ).arg( epsgGlobe ) );

	// transform extent to WGS84
	if ( mapSettings.destinationCrs().authid().compare( QString( "EPSG:%1" ).arg( epsgGlobe ), Qt::CaseInsensitive ) != 0 )
	{
		QgsCoordinateReferenceSystem srcCRS( mapSettings.destinationCrs() );
		QgsCoordinateTransform* coordTransform = new QgsCoordinateTransform( srcCRS, globeCrs );
		extent = coordTransform->transformBoundingBox( extent );
		delete coordTransform;
	}

	osgEarth::Util::EarthManipulator* manip = dynamic_cast<osgEarth::Util::EarthManipulator*>( mOsgViewer->getCameraManipulator() );
	//rotate earth to north and perpendicular to camera
	manip->setRotation( osg::Quat() );

	QgsDistanceArea dist;

	dist.setSourceCrs( globeCrs );
	dist.setEllipsoidalMode( true );
	dist.setEllipsoid( "WGS84" );

	QgsPoint ll = QgsPoint( extent.xMinimum(), extent.yMinimum() );
	QgsPoint ul = QgsPoint( extent.xMinimum(), extent.yMaximum() );
	double height = dist.measureLine( ll, ul );

	//camera viewing angle
	double viewAngle = 30;
	//camera distance
	double distance = height / tan( viewAngle * osg::PI / 180 ); //c = b*cotan(B(rad))

	OE_NOTICE << "map extent: " << height << " camera distance: " << distance << std::endl;

	//osgEarth::Util::Viewpoint viewpoint( osg::Vec3d( extent.center().x(), extent.center().y(), 0.0 ), 0.0, -90.0, distance ); //femm

	osgEarth::Util::Viewpoint viewpoint( nullptr, extent.center().x(), extent.center().y(), 0.0, 0.0, -90.0, distance );
	manip->setViewpoint( viewpoint, 4.0 );
}


void CGlobeWidget::setVerticalScale( double value )		//OSGEARTH_VERSION_GREATER_OR_EQUAL( 2, 5, 0 )
{
  if ( mMapNode )
  {
    if ( !mVerticalScale.get() || mVerticalScale->getScale() != value )
    {
      mMapNode->getTerrainEngine()->removeEffect( mVerticalScale );
      mVerticalScale = new osgEarth::Util::VerticalScale();
      mVerticalScale->setScale( value );
      mMapNode->getTerrainEngine()->addEffect( mVerticalScale );
    }
  }
}





void CGlobeWidget::setSkyParameters( bool sky, const QDateTime& date_time, bool auto_ambience )
{
	assert__( mRootNode );
	if ( sky )
	{
		// Create if not yet done
		if ( !mSkyNode.get() )
		{
			SkyOptions options;
			options.ambient() = auto_ambience ? 1.0 : 0.0;
			mSkyNode = SkyNode::create( options, mMapNode );
		}

#if OSGEARTH_VERSION_GREATER_OR_EQUAL( 2, 4, 0 )
		//mSkyNode->setAutoAmbience( autoAmbience );	//femm
#else
		Q_UNUSED( autoAmbience );
#endif
		mSkyNode->setDateTime( osgEarth::DateTime( 
			date_time.date().year()
			, date_time.date().month()
			, date_time.date().day()
			, date_time.time().hour() + date_time.time().minute() / 60.0 ) 
			);

		//mSkyNode->setSunPosition( osg::Vec3(0,-1,0) );

		bool fancy = false;

        mSkyNode->setMoonVisible( true );
		mSkyNode->setStarsVisible( true );
		mSkyNode->setSunVisible( fancy );

		//mSkyNode->setLighting( osg::StateAttribute::OFF );
		//mSkyNode->getSunLight()->setSpecular(osg::Vec4(1.,1.,1.,1.));
		//mSkyNode->getSunLight()->setAmbient(osg::Vec4(1.,1.,1.,1.));
		//mSkyNode->setNodeMask(0);

		mSkyNode->attach( mOsgViewer );
			
		mRootNode->addChild( mSkyNode );
	}
	else
	{
		mRootNode->removeChild( mSkyNode );
	}
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_GlobeWidget.cpp"
