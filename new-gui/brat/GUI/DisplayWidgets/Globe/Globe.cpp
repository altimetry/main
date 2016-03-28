/***************************************************************************
    globe_plugin.cpp

    Globe Plugin
    a QGIS plugin
     --------------------------------------
    Date                 : 08-Jul-2010
    Copyright            : (C) 2010 by Sourcepole
    Email                : info at sourcepole.ch
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "new-gui/brat/stdafx.h"

//#include <cmath>

//#include <QAction>
//#include <QDir>
//#include <QToolBar>
//#include <QMessageBox>

#if defined (WIN32) || defined(_WIN32)
#pragma warning ( disable: 4100 )           //unreferenced formal parameter
#endif

#include <osg/MatrixTransform>
#include <osg/ShapeDrawable>
#include <osgGA/StateSetManipulator>
#include <osgGA/GUIEventHandler>
#include <osgGA/TrackballManipulator>
#include <osgDB/ReadFile>
#include <osgDB/Registry>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#ifdef HAVE_OSGEARTHQT
#include <osgEarthQt/ViewerWidget>
#else
#include "osgEarthQt/ViewerWidget"
#endif
#include <osgEarth/Notify>
#include <osgEarth/Map>
#include <osgEarth/MapNode>
#include <osgEarth/TileSource>
#include <osgEarthUtil/Sky>						//femm	SkyNode => Sky
#include <osgEarthUtil/AutoClipPlaneHandler>
#include <osgEarthUtil/MouseCoordsTool>
#include <osgEarthDrivers/gdal/GDALOptions>
#include <osgEarthDrivers/tms/TMSOptions>

#include <osgEarth/Version>

#include <osgEarthDrivers/cache_filesystem/FileSystemCache>		//OSGEARTH_VERSION_GREATER_OR_EQUAL( 2, 2, 0 )

#include <osgEarthUtil/VerticalScale>							//OSGEARTH_VERSION_GREATER_OR_EQUAL( 2, 5, 0 )

using namespace osgEarth::Drivers;
using namespace osgEarth::Util;
//
//#include <osgEarth/ImageLayer>
#include <osgEarthUtil/EarthManipulator>

#ifndef HAVE_OSGEARTHQT 
	//use back-ported controls if osgEarth <= 2.1
	#define USE_BACKPORTED_CONTROLS
#endif
#ifdef USE_BACKPORTED_CONTROLS
	#include "osgEarthUtil/Controls"
	using namespace osgEarth::Util::Controls21;
#else
	#include <osgEarthUtil/Controls>
	using namespace osgEarth::Util::Controls;
#endif

//#ifdef HAVE_OSGEARTH_ELEVATION_QUERY
//	#include <osgEarth/ElevationQuery>
//	#include <osgEarthUtil/ObjectLocator>
//#else
//	#include <osgEarthUtil/ElevationManager>
//	#include <osgEarthUtil/ObjectPlacer>
//#endif

#include <qgisinterface.h>
#include <qgisgui.h>
#include <qgslogger.h>
#include <qgsapplication.h>
#include <qgsmapcanvas.h>
#include <qgsfeature.h>
#include <qgsgeometry.h>
#include <qgspoint.h>
#include <qgsdistancearea.h>

#include "globe_plugin_dialog.h"
#include "qgsosgearthtilesource.h"


#if defined (WIN32) || defined(_WIN32)
#pragma warning ( default: 4100 )           //unreferenced formal parameter
#endif


#include "new-gui/Common/+Utils.h"

#include "GlobeUpdateCallback.h"
#include "GlobeControls.h"
#include "Globe.h"



///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Globe
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//		Globe statics
///////////////////////////////////////////////////////////////////////////////////////////////////////////



void CGlobePlugin::SetOSGDirectories( const std::string &GlobeDir )
{
	CGlobeControls::SetControlsDirectory( GlobeDir );
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Globe construction - I. function invoked by the constructor, in their call order
///////////////////////////////////////////////////////////////////////////////////////////////////////////


void CGlobePlugin::setBaseMap( QString url )
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

//#include <osgGA/StateSetManipulator>
#include <osgEarthUtil/MouseCoordsTool>
#include <osgEarthUtil/MGRSFormatter>
#include <osgEarthUtil/LatLongFormatter>

#include <osgEarthUtil/GeodeticGraticule>
#include <osgEarthUtil/MGRSGraticule>
#include <osgEarthUtil/UTMGraticule>

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

void CGlobePlugin::ToggleGridEnabled( bool toggle )
{
	assert__( mGraticuleNode.get() );

	mGraticuleNode->setVisible( toggle );
}


void CGlobePlugin::setupMap()
{
	QSettings settings;
	QString cacheDirectory = settings.value( "cache/directory", QgsApplication::qgisSettingsDirPath() + "cache" ).toString();

	FileSystemCacheOptions cacheOptions;					//OSGEARTH_VERSION_GREATER_OR_EQUAL( 2, 2, 0 )
	cacheOptions.rootPath() = cacheDirectory.toStdString();	//OSGEARTH_VERSION_GREATER_OR_EQUAL( 2, 2, 0 )

	MapOptions mapOptions;
	mapOptions.cache() = cacheOptions;
	osgEarth::Map *map = new osgEarth::Map( mapOptions );

	//Default image layer
	/*
	GDALOptions driverOptions;
	driverOptions.url() = QDir::cleanPath( QgsApplication::pkgDataPath() + "/globe/world.tif" ).toStdString();
	ImageLayerOptions layerOptions( "world", driverOptions );
	map->addImageLayer( new osgEarth::ImageLayer( layerOptions ) );
	*/

	MapNodeOptions nodeOptions;
	//nodeOptions.proxySettings() =
	//nodeOptions.enableLighting() = false;

	//LoadingPolicy loadingPolicy( LoadingPolicy::MODE_SEQUENTIAL );
	TerrainOptions terrainOptions;
	//terrainOptions.loadingPolicy() = loadingPolicy;
	//terrainOptions.compositingTechnique() = TerrainOptions::COMPOSITING_MULTITEXTURE_FFP; femm
	//terrainOptions.lodFallOff() = 6.0;
	nodeOptions.setTerrainOptions( terrainOptions );

	// The MapNode will render the Map object in the scene graph.
	mMapNode = new osgEarth::MapNode( map, nodeOptions );

	const bool enable_base_map = false;	//don't load
	if ( settings.value( "/Plugin-Globe/baseLayerEnabled", enable_base_map ).toBool() )
	{
		setBaseMap( settings.value( "/Plugin-Globe/baseLayerURL", "http://readymap.org/readymap/tiles/1.0.0/7/" ).toString() );
	}

	mRootNode = new osg::Group();

	//osg::Node* gridNode = osgDB::readNodeFile( R"(L:\lib\Graphics\OSG\osgEarth\default\source\tests\_ridicule.earth)" );
 //   if ( gridNode )
	//	mRootNode->addChild( gridNode );

	mRootNode->addChild( mMapNode );
    Formatter* formatter = 0L;
	
	bool isUTM = false;
	bool isMGRS = false;
	bool isGeodetic = false;

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
        mGraticuleNode = new GraticuleNode( mMapNode, o );
        mRootNode->addChild( mGraticuleNode );
		mGraticuleNode->setVisible( false );
    }

    //VBox* vbox = new VBox();
    //ControlCanvas::get( mOsgViewer )->addControl( vbox );
    //LabelControl* readout = new LabelControl();
    //vbox->addControl( readout );

    //if (graticuleNode)
    //{
    //    HBox* toggleBox = vbox->addControl( new HBox() );
    //    toggleBox->setChildSpacing( 5 );
    //    CheckBoxControl* toggleCheckBox = new CheckBoxControl( true );
    //    toggleCheckBox->addEventHandler( new ToggleGraticuleHandler( graticuleNode ) );
    //    toggleBox->addControl( toggleCheckBox );
    //    LabelControl* labelControl = new LabelControl( "Show Graticule" );
    //    labelControl->setFontSize( 24.0f );
    //    toggleBox->addControl( labelControl  );

    //    HBox* offsetBox = vbox->addControl( new HBox() );
    //    offsetBox->setChildSpacing( 5 );
    //    osg::Vec4 activeColor(1,.3,.3,1);

    //    offsetBox->addControl(new LabelControl("Adjust Labels"));

    //    double adj = 10.0;
    //    LabelControl* left = new LabelControl("Left");
    //    left->addEventHandler(new OffsetGraticuleHandler(graticuleNode, osg::Vec2f(-adj, 0.0)) );
    //    offsetBox->addControl(left);
    //    left->setActiveColor(activeColor);

    //    LabelControl* right = new LabelControl("Right");
    //    right->addEventHandler(new OffsetGraticuleHandler(graticuleNode, osg::Vec2f(adj, 0.0)) );
    //    offsetBox->addControl(right);
    //    right->setActiveColor(activeColor);

    //    LabelControl* down = new LabelControl("Down");
    //    down->addEventHandler(new OffsetGraticuleHandler(graticuleNode, osg::Vec2f(0.0, -adj)) );
    //    offsetBox->addControl(down);
    //    down->setActiveColor(activeColor);

    //    LabelControl* up = new LabelControl("Up");
    //    up->addEventHandler(new OffsetGraticuleHandler(graticuleNode, osg::Vec2f(0.0, adj)) );
    //    offsetBox->addControl(up);
    //    up->setActiveColor(activeColor);


    //}
    //MouseCoordsTool* tool = new MouseCoordsTool( mMapNode );
    //tool->addCallback( new MouseCoordsLabelCallback(readout, formatter) );
    //mOsgViewer->addEventHandler( tool );





	// Add layers to the map
	if ( mCanvas )			//femm
	{
		ImageLayersChanged();
		elevationLayersChanged();
	}

	// model placement utils
#ifdef HAVE_OSGEARTH_ELEVATION_QUERY
#else
	mElevationManager = new osgEarth::Util::ElevationManager( mMapNode->getMap() );
	mElevationManager->setTechnique( osgEarth::Util::ElevationManager::TECHNIQUE_GEOMETRIC );
	mElevationManager->setMaxTilesToCache( 50 );

	mObjectPlacer = new osgEarth::Util::ObjectPlacer( mMapNode );

	// place 3D model on point layer
	if ( mSettingsDialog->modelLayer() && !mSettingsDialog->modelPath().isEmpty() )
	{
		osg::Node* model = osgDB::readNodeFile( mSettingsDialog->modelPath().toStdString() );
		if ( model )
		{
			QgsVectorLayer* layer = mSettingsDialog->modelLayer();
			QgsFeatureIterator fit = layer->getFeatures( QgsFeatureRequest().setSubsetOfAttributes( QgsAttributeList() ) ); //TODO: select only visible features
			QgsFeature feature;
			while ( fit.nextFeature( feature ) )
			{
				QgsPoint point = feature.geometry()->asPoint();
				placeNode( model, point.y(), point.x() );
			}
		}
	}
#endif

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Globe construction - II. structures created by the constructor
///////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PrintCoordsToStatusBar : public osgEarth::Util::MouseCoordsTool::Callback
{
	QStatusBar* mStatusBar = nullptr;

public:
	PrintCoordsToStatusBar( QStatusBar *sb ) : mStatusBar( sb ) { }

	void set( const GeoPoint& p, osg::View* view, MapNode* mapNode )
	{
		std::string str = osgEarth::Stringify() << p.y() << ", " << p.x();
		mStatusBar->showMessage( QString( str.c_str() ) );
	}

	void reset( osg::View* view, MapNode* mapNode )
	{
		mStatusBar->showMessage( QString( "out of range" ) );
	}
};



class GlobeViewer : public osgViewer::Viewer
{
	using base_t = osgViewer::Viewer;

public:

	GlobeViewer()
		: base_t()
	{
		//connect( &m_timer, SIGNAL( timeout() ), this, SLOT( update() ) );
		//m_timer.start( m_gps.TimerInterval() );
	}

	virtual int run()
	{
		return base_t::run();
	}
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Globe constructor / destruction
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CGlobePlugin::CGlobePlugin( QgsMapCanvas *the_canvas, QStatusBar *sb, QWidget *dialogParent )		//sb = nullptr, QWidget *dialogParent = nullptr 
	: mCanvas( the_canvas )
{
	assert__( !mOsgViewer );
	assert__( !mRootNode );
	assert__( !mMapNode );
	assert__( !mBaseLayer );
	assert__( !mQgisMapLayer );
	assert__( !mTileSource );
	assert__( !mControlCanvas );
	assert__( !mElevationManager );
	assert__( !mObjectPlacer );
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

#if defined (GLOBE_DIALOG_ENABLED)

	if ( !dialogParent )
		if ( mCanvas )
			dialogParent = mCanvas->nativeParentWidget();

	mSettingsDialog = new QgsGlobePluginDialog( dialogParent, this, QgisGui::ModalDialogFlags );		//v4: dialogParent was theQgisInterface->mainWindow()
	//v4: moved from initGUI
	connect( mSettingsDialog, SIGNAL( elevationDatasourcesChanged() ), this, SLOT( elevationLayersChanged() ) );

#endif

	//ex-run() member function

	mOsgViewer = new GlobeViewer();	// osgViewer::Viewer();
	mOsgViewer->setThreadingModel( osgViewer::Viewer::SingleThreaded );

	// install the programmable manipulator.
	//
	osgEarth::Util::EarthManipulator* manip = new osgEarth::Util::EarthManipulator();
	mOsgViewer->setCameraManipulator( manip );

	setupMap();		//creates the root node! and attaches the map node

	// v4 setSkyParameters was called here

	// create a surface to house the controls
	//
	mControlCanvas = ControlCanvas::get( mOsgViewer );		//OSGEARTH_VERSION_GREATER_OR_EQUAL( 2, 1, 1 )
	mRootNode->addChild( mControlCanvas );

	mRootUpdateCallback = new CRootUpdateCallback( this );
	mRootNode->setUpdateCallback( mRootUpdateCallback );

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
	mOsgViewer->getDatabasePager()->setDoPreCompile( true );


//#ifdef GLOBE_OSG_STANDALONE_VIEWER  mOsgViewer->run();	#endif

	// Set a home viewpoint
	manip->setHomeViewpoint(
		//osgEarth::Util::Viewpoint( osg::Vec3d( -90, 0, 0 ), 0.0, -90.0, 2e7 ), femm
		osgEarth::Util::Viewpoint( nullptr, -90, 0, 0, 0.0, -90.0, 2e7 ),
		1.0 );


	// add our handlers
	mOsgViewer->addEventHandler( new FlyToExtentHandler( this ) );
	mOsgViewer->addEventHandler( new KeyboardControlHandler( manip ) );

	if ( sb )
	{
		MouseCoordsTool* tool = new MouseCoordsTool( mMapNode );
		tool->addCallback( new PrintCoordsToStatusBar( sb ) );
		mOsgViewer->addEventHandler( tool );
	}


#ifndef HAVE_OSGEARTH_ELEVATION_QUERY
	mOsgViewer->addEventHandler( new QueryCoordinatesHandler( this, mElevationManager,
		mMapNode->getMap()->getProfile()->getSRS() )
		);
#endif

	mControls = new CGlobeControls( this );

	//// Connect actions
	//connect( mQActionPointer, SIGNAL( triggered() ), this, SLOT( run() ) );
	//connect( mQActionSettingsPointer, SIGNAL( triggered() ), this, SLOT( settings() ) );
	//connect( mQActionUnload, SIGNAL( triggered() ), this, SLOT( reset() ) );

	//// Add the icon to the toolbar
	//mQGisIface->addToolBarIcon( mQActionPointer );

	////Add menu
	//mQGisIface->addPluginToMenu( tr( "&Globe" ), mQActionPointer );
	//mQGisIface->addPluginToMenu( tr( "&Globe" ), mQActionSettingsPointer );
	//mQGisIface->addPluginToMenu( tr( "&Globe" ), mQActionUnload );

	connect( mCanvas, SIGNAL( extentsChanged() ),			this, SLOT( extentsChanged() ) );
	//connect( mCanvas, SIGNAL( layersChanged() ),			this, SLOT( imageLayersChanged() ) );

	//connect( mSettingsDialog, SIGNAL( elevationDatasourcesChanged() ),	this, SLOT( elevationLayersChanged() ) );
	//connect( mQGisIface->mainWindow(), SIGNAL( projectRead() ),			this, SLOT( projectReady() ) );
	//connect( mQGisIface, SIGNAL( newProjectCreated() ),					this, SLOT( blankProjectReady() ) );
	connect( this, SIGNAL( xyCoordinates( const QgsPoint & ) ),			mCanvas, SIGNAL( xyCoordinates( const QgsPoint & ) ) );

	connect( mCanvas, SIGNAL( GridEnabled( bool ) ),	this, SLOT( ToggleGridEnabled( bool ) ) ); 

	mOsgViewer->getCamera()->setClearColor( osg::Vec4( 1.0, 1.0, 0xD8/255.0, 0.5 ) );
    //mOsgViewer->getCamera()->setClearMask( mOsgViewer->getCamera()->getClearMask() | GL_COLOR_BUFFER_BIT );
	//mOsgViewer->getCamera()->setClearColor(osg::Vec4(0.,0.,0.,0.));
	
}



//destructor
CGlobePlugin::~CGlobePlugin()
{
	delete mControls;
	//delete mTileSource;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Slots
///////////////////////////////////////////////////////////////////////////////////////////////////////////


void CGlobePlugin::SetSkyParameters( bool sky )
{
	setSkyParameters( sky, QDateTime(), true );
}

void CGlobePlugin::SetupControls( bool set )
{
	assert__( mControls );

	set ? mControls->AddControls() : mControls->RemoveControls();
}




#if defined (GLOBE_DIALOG_ENABLED)

void CGlobePlugin::settings()
{
	mSettingsDialog->updatePointLayers();
	if ( mSettingsDialog->exec() )
	{
		//viewer stereo settings set by mSettingsDialog and stored in QSettings
	}
}

void CGlobePlugin::projectReady()
{
	blankProjectReady();
	mSettingsDialog->readElevationDatasources();
}

void CGlobePlugin::blankProjectReady()
{ //needs at least http://trac.osgeo.org/qgis/changeset/14452
	mSettingsDialog->resetElevationDatasources();
}

#endif




///////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////


void CGlobePlugin::showCurrentCoordinates( double lon, double lat )
{
  // show x y on status bar
  //OE_NOTICE << "lon: " << lon << " lat: " << lat <<std::endl;
  QgsPoint coord = QgsPoint( lon, lat );
  emit xyCoordinates( coord );
}

void CGlobePlugin::setSelectedCoordinates( osg::Vec3d coords )
{
  mSelectedLon = coords.x();
  mSelectedLat = coords.y();
  mSelectedElevation = coords.z();
  QgsPoint coord = QgsPoint( mSelectedLon, mSelectedLat );
  emit newCoordinatesSelected( coord );
}

osg::Vec3d CGlobePlugin::getSelectedCoordinates()
{
  osg::Vec3d coords = osg::Vec3d( mSelectedLon, mSelectedLat, mSelectedElevation );
  return coords;
}

void CGlobePlugin::showSelectedCoordinates()
{
  QString lon, lat, elevation;
  lon.setNum( mSelectedLon );
  lat.setNum( mSelectedLat );
  elevation.setNum( mSelectedElevation );
  QMessageBox m;
  m.setText( "selected coordinates are:\nlon: " + lon + "\nlat: " + lat + "\nelevation: " + elevation );
  m.exec();
}

double CGlobePlugin::getSelectedLon()
{
  return mSelectedLon;
}

double CGlobePlugin::getSelectedLat()
{
  return mSelectedLat;
}

double CGlobePlugin::getSelectedElevation()
{
  return mSelectedElevation;
}

void CGlobePlugin::syncExtent()
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

	osgEarth::Util::Viewpoint viewpoint( nullptr, extent.center().x(), extent.center().y(), 0.0, 0.0, -90.0, distance );
	//osgEarth::Util::Viewpoint viewpoint( osg::Vec3d( extent.center().x(), extent.center().y(), 0.0 ), 0.0, -90.0, distance ); //femm
	manip->setViewpoint( viewpoint, 4.0 );
}


void CGlobePlugin::setVerticalScale( double value )		//OSGEARTH_VERSION_GREATER_OR_EQUAL( 2, 5, 0 )
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



void CGlobePlugin::extentsChanged()
{
	assert__( mCanvas );

	QgsDebugMsg( "extentsChanged: " + mCanvas->extent().toString() );
}


void CGlobePlugin::RemoveLayers()
{
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
}


void CGlobePlugin::ImageLayersChanged()
{
	assert__( mCanvas );

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
	mTileSource->initialize( "", 0 );
	ImageLayerOptions options( "QGIS" );

	options.cachePolicy() = CachePolicy::NO_CACHE;				//OSGEARTH_VERSION_GREATER_OR_EQUAL( 2, 2, 0 )

	mQgisMapLayer = new ImageLayer( options, mTileSource );
	map->addImageLayer( mQgisMapLayer );
}


void CGlobePlugin::imageLayersChanged()
{
	ImageLayersChanged();		//mRootUpdateCallback->AssignImageLayersChanged();


	//assert__( mCanvas );

	//QgsDebugMsg( "imageLayersChanged: Globe Running, executing" );

	//osg::ref_ptr<Map> map = mMapNode->getMap();
	//if ( map->getNumImageLayers() > 1 )
	//{
	//	mOsgViewer->getDatabasePager()->clear();
	//}

	////remove QGIS layer
	//if ( mQgisMapLayer )
	//{
	//	QgsDebugMsg( "removeMapLayer" );
	//	map->removeImageLayer( mQgisMapLayer );
	//}

	////add QGIS layer
	//QgsDebugMsg( "addMapLayer" );
	//mTileSource = new QgsOsgEarthTileSource( mCanvas );
	//mTileSource->initialize( "", 0 );
	//ImageLayerOptions options( "QGIS" );

	//options.cachePolicy() = CachePolicy::NO_CACHE;				//OSGEARTH_VERSION_GREATER_OR_EQUAL( 2, 2, 0 )

	//mQgisMapLayer = new ImageLayer( options, mTileSource );
	//map->addImageLayer( mQgisMapLayer );
}


void CGlobePlugin::elevationLayersChanged()
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

#if defined (GLOBE_DIALOG_ENABLED)

	QSettings settings;
	QString cacheDirectory = settings.value( "cache/directory", QgsApplication::qgisSettingsDirPath() + "cache" ).toString();
	QTableWidget* table = mSettingsDialog->elevationDatasources();
	for ( int i = 0; i < table->rowCount(); ++i )
	{
		QString type = table->item( i, 0 )->text();
		//bool cache = table->item( i, 1 )->checkState();
		QString uri = table->item( i, 2 )->text();
		ElevationLayer* layer = 0;

		if ( "Raster" == type )
		{
			GDALOptions options;
			options.url() = uri.toStdString();
			layer = new osgEarth::ElevationLayer( uri.toStdString(), options );
		}
		else if ( "TMS" == type )
		{
			TMSOptions options;
			options.url() = uri.toStdString();
			layer = new osgEarth::ElevationLayer( uri.toStdString(), options );
		}
		map->addElevationLayer( layer );

		//if ( !cache || type == "Worldwind" ) layer->setCache( 0 ); //no tms cache for worldwind (use worldwind_cache)
	}
#endif		//	GLOBE_DIALOG_ENABLED

	double scale = QgsProject::instance()->readDoubleEntry( "Globe-Plugin", "/verticalScale", 1 );	//OSGEARTH_VERSION_GREATER_OR_EQUAL( 2, 5, 0 )
	setVerticalScale( scale );																		//OSGEARTH_VERSION_GREATER_OR_EQUAL( 2, 5, 0 )
}



void CGlobePlugin::setSkyParameters( bool sky, const QDateTime& date_time, bool auto_ambience )
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

		mSkyNode->setMoonVisible( fancy );
		mSkyNode->setStarsVisible( fancy );
		mSkyNode->setSunVisible( fancy );

		//mSkyNode->setLighting( osg::StateAttribute::OFF );
		//mSkyNode->getSunLight()->setSpecular(osg::Vec4(1.,1.,1.,1.));
		//mSkyNode->getSunLight()->setAmbient(osg::Vec4(1.,1.,1.,1.));
		//mSkyNode->setNodeMask(0);

		mSkyNode->attach( mOsgViewer );
			
		mRootUpdateCallback->AssignNodeToAdd( mSkyNode );		//mRootNode->addChild( mSkyNode );
	}
	else
	{
		mRootUpdateCallback->AssignNodeToDelete( mSkyNode );	// mRootNode->removeChild( mSkyNode );
	}
}


void CGlobePlugin::placeNode( osg::Node* node, double lat, double lon, double alt /*= 0.0*/ )
{
#ifdef HAVE_OSGEARTH_ELEVATION_QUERY
	Q_UNUSED( node );
	Q_UNUSED( lat );
	Q_UNUSED( lon );
	Q_UNUSED( alt );
#else
	// get elevation
	double elevation = 0.0;
	double resolution = 0.0;
	mElevationManager->getElevation( lon, lat, 0, NULL, elevation, resolution );

	// place model
	osg::Matrix mat;
	mObjectPlacer->createPlacerMatrix( lat, lon, elevation + alt, mat );

	osg::MatrixTransform* mt = new osg::MatrixTransform( mat );
	mt->addChild( node );
	mRootNode->addChild( mt );
#endif
}




////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////

#include "moc_Globe.cpp"
