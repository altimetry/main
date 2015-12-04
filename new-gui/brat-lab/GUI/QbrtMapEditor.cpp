#include "../stdafx.h"

#include <QtGui>
#include <QColor>

#include <osgEarthQt/ViewerWidget>

#include <qgsproviderregistry.h>
#include <qgsvectordataprovider.h>
#include <qgsmaplayerregistry.h>
#include <qgsvectorlayer.h>

#include <qgsrubberband.h>
#include <qgsvertexmarker.h>
#include <qgscoordinatereferencesystem.h>
#include <qgsapplication.h>
#include <qgslinesymbollayerv2.h>
#include <qgsmarkersymbollayerv2.h>
//#include <qgsvectorfieldsymbollayer.h>

#include <qgslayertreeview.h>
#include <qgslayertreemodel.h>
#include <qgslayertreenode.h>
#include <qgslayertreeviewdefaultactions.h>
#include <qgslayertreegroup.h>

#include "qgsapplayertreeviewmenuprovider.h"

#include "new-gui/Common/QtFileUtils.h"

#include "new-gui/Common/tools/Trace.h"

#include "../System/BackServices.h"
#include "../System/CmdLineProcessor.h"
#include "../../display/PlotData/GeoMap.h"
#include "../../display/PlotData/WPlot.h"


#include "QbrtMapEditor.h"


void addLabelsLayer( QbrtMapEditor *mpMapCanvas );


//constants.py

auto TRACK_TYPE_ROAD    = "ROAD";
auto TRACK_TYPE_WALKING = "WALKING";
auto TRACK_TYPE_BIKE    = "BIKE";
auto TRACK_TYPE_HORSE   = "HORSE";

auto TRACK_DIRECTION_BOTH     = "BOTH";
auto TRACK_DIRECTION_FORWARD  = "FORWARD";
auto TRACK_DIRECTION_BACKWARD = "BACKWARD";

auto TRACK_STATUS_OPEN   = "OPEN";
auto TRACK_STATUS_CLOSED = "CLOSED";

// Various pixel widths:

auto ROAD_WIDTH  = 0.0001;  // Width of a road, in map units.
auto TRAIL_WIDTH = 0.00003; // Width of a trail, in map units.
auto POINT_SIZE  = 0.0004;  // Diameter of a start/end point, in map units.


#undef HAVE_VPRINTF
#undef HAVE_SNPRINTF
#undef HAVE_DIRECT_H
#define Py_CONFIG_H
#define MIN std::min
#include <qgsvectorfilewriter.h>
#include <qgsrasterlayer.h>
#include <qgsdatasourceuri.h>

void QbrtMapCanvas::setupDatabase()
{
	std::string cur_dir = getDirectoryFromPath( qApp->argv()[ 0 ] );
	std::string dbName = cur_dir + "/data" + "/tracks.sqlite";
	if ( !IsFile( dbName ) )
	{
		QgsFields fields;
		fields.append( QgsField( "id", QVariant::Int ) );
		fields.append( QgsField( "type", QVariant::String ) );
		fields.append( QgsField( "name", QVariant::String ) );
		fields.append( QgsField( "direction", QVariant::String ) );
		fields.append( QgsField( "status", QVariant::String ) );
		QgsCoordinateReferenceSystem crs( 4326, QgsCoordinateReferenceSystem::EpsgCrsId );
		QgsVectorFileWriter writer( 
			t2q( dbName ), 
			"utf-8", 
			fields,
			QGis::WKBLineString,
			&crs, "SQLite",
			QStringList( "SPATIALITE=YES" ) );

		if ( writer.hasError() != QgsVectorFileWriter::NoError )
			std::cout << "Error creating tracks database!" << std::endl;

		//del writer
	}
}

#include <qgsrulebasedrendererv2.h>

QgsSymbolV2* QbrtMapCanvas::createSymbol( double width, 
	const QColor &lineColor, const std::string &arrowColor, const std::string &status, const std::string &direction )
{
	auto qsm = []( const char *s1, const std::string &s2 )
	{
		QgsStringMap m;
		m.insert( s1, s2.c_str() );
		return m;
	};

	QgsSymbolV2 *s = QgsLineSymbolV2::createSimple( qsm( "", "" ) );
	s->deleteSymbolLayer( 0 ); // Remove default symbol layer.
	//s->setColor( lineColor );
	//s->setMapUnitScale()

	auto symbolLayer = new QgsSimpleLineSymbolLayerV2;
	//auto symbolLayer = QgsVectorFieldSymbolLayer::create( qsm( "", "" ) );
	symbolLayer->setWidth( width );
	symbolLayer->setWidthUnit( QgsSymbolV2::MapUnit );
	symbolLayer->setColor( QColor( lineColor ) );
	if ( status == TRACK_STATUS_CLOSED )
		symbolLayer->setPenStyle( Qt::DotLine );
	s->appendSymbolLayer( symbolLayer );

    if ( direction == TRACK_DIRECTION_FORWARD )
	{ }
    //        registry = QgsSymbolLayerV2Registry.instance()
    //        markerLineMetadata = registry.symbolLayerMetadata("MarkerLine")
    //        markerMetadata     = registry.symbolLayerMetadata("SimpleMarker")

    //        symbolLayer = markerLineMetadata.createSymbolLayer(
    //                            {'width': '0.26',
    //                             'color': arrowColor,
    //                             'rotate': '1',
    //                             'placement': 'interval',
    //                             'interval' : '20',
    //                             'offset': '0'})
    //        subSymbol = symbolLayer.subSymbol()
    //        subSymbol.deleteSymbolLayer(0)
    //        triangle = markerMetadata.createSymbolLayer(
    //                            {'name': 'filled_arrowhead',
    //                             'color': arrowColor,
    //                             'color_border': arrowColor,
    //                             'offset': '0,0',
    //                             'size': '3',
    //                             'outline_width': '0.5',
    //                             'output_unit': 'mapunit',
    //                             'angle': '0'})
    //        subSymbol.appendSymbolLayer(triangle)

    //        symbol.appendSymbolLayer(symbolLayer)
	else
	if ( direction == TRACK_DIRECTION_BACKWARD )
	{ }
    //        registry = QgsSymbolLayerV2Registry.instance()
    //        markerLineMetadata = registry.symbolLayerMetadata("MarkerLine")
    //        markerMetadata     = registry.symbolLayerMetadata("SimpleMarker")

    //        symbolLayer = markerLineMetadata.createSymbolLayer(
    //                            {'width': '0.26',
    //                             'color': arrowColor,
    //                             'rotate': '1',
    //                             'placement': 'interval',
    //                             'interval' : '20',
    //                             'offset': '0'})
    //        subSymbol = symbolLayer.subSymbol()
    //        subSymbol.deleteSymbolLayer(0)
    //        triangle = markerMetadata.createSymbolLayer(
    //                            {'name': 'filled_arrowhead',
    //                             'color': arrowColor,
    //                             'color_border': arrowColor,
    //                             'offset': '0,0',
    //                             'size': '3',
    //                             'outline_width': '0.5',
    //                             'output_unit': 'mapunit',
    //                             'angle': '180'})
    //        subSymbol.appendSymbolLayer(triangle)

    //        symbol.appendSymbolLayer(symbolLayer)


	return s;
}

void QbrtMapCanvas::setupMapLayers()
{
	std::string cur_dir = getDirectoryFromPath( qApp->argv()[ 0 ] );

	// Raster base layer.

	std::string filename = cur_dir + "/data" + "/basemap.tif";
	QgsRasterLayer *baseLayer = new QgsRasterLayer( t2q( filename ), "basemap" );
	QgsMapLayerRegistry::instance()->addMapLayer( baseLayer );
    mLayerSet.append(QgsMapCanvasLayer(baseLayer, TRUE));

	// Track layer, stored in an sqlite database.

	QgsDataSourceURI uri;
	uri.setDatabase( t2q( cur_dir + "/data" + "/tracks.sqlite" ) );
	uri.setDataSource( "", "tracks", "GEOMETRY" );

	QgsVectorLayer *trackLayer = new QgsVectorLayer( uri.uri(), "Tracks", "spatialite" );
	QgsMapLayerRegistry::instance()->addMapLayer( trackLayer );
	mLayerSet.append(QgsMapCanvasLayer( trackLayer, TRUE ) );

	// Shortest path layer, held in memory.

	QgsVectorLayer *shortestPathLayer = new QgsVectorLayer( "LineString?crs=EPSG:4326", "shortestPathLayer", "memory" );
	QgsMapLayerRegistry::instance()->addMapLayer( shortestPathLayer );
	mLayerSet.append( QgsMapCanvasLayer( shortestPathLayer, TRUE ) );

		// Start point layer, held in memory.

	QgsVectorLayer *startPointLayer = new QgsVectorLayer( "Point?crs=EPSG:4326", "startPointLayer", "memory" );
	QgsMapLayerRegistry::instance()->addMapLayer( startPointLayer );
	mLayerSet.append( QgsMapCanvasLayer( startPointLayer ) );

	// End point layer, held in memory.

	QgsVectorLayer *endPointLayer = new QgsVectorLayer( "Point?crs=EPSG:4326", "endPointLayer", "memory" );
	QgsMapLayerRegistry::instance()->addMapLayer( endPointLayer );
	mLayerSet.append( QgsMapCanvasLayer( endPointLayer ) );

	// Show the map layers.

	//mLayerSet.reverse(); // Vector layers sit in front of basemap.
	setLayerSet( mLayerSet );
	setExtent( baseLayer->extent() );


	////////////////////////////////////////
	// Create a renderer to show the tracks.
	////////////////////////////////////////

	QgsRuleBasedRendererV2::Rule *root_rule = new QgsRuleBasedRendererV2::Rule(nullptr);
	const char *things[] = { TRACK_TYPE_ROAD, TRACK_TYPE_WALKING, TRACK_TYPE_BIKE, TRACK_TYPE_HORSE };
	double width = 0.0;
	for ( std::string track_type : things )
	{
		if ( track_type == TRACK_TYPE_ROAD )
			width = ROAD_WIDTH;
		else
			width = TRAIL_WIDTH;

		auto lineColor = Qt::lightGray;
		std::string arrowColor = "dark gray";

		const char *more_things[] = { TRACK_STATUS_OPEN, TRACK_STATUS_CLOSED };
		for ( std::string track_status : more_things )
		{
			const char *yet_more_things[] = { TRACK_DIRECTION_BOTH, TRACK_DIRECTION_FORWARD, TRACK_DIRECTION_BACKWARD };
			for ( std::string track_direction : yet_more_things )
			{
				QgsSymbolV2 *symbol = createSymbol( width, lineColor, arrowColor, track_status, track_direction );
				auto expression = ( "(type=" + track_type + ") and (status=" + track_status + ") and (direction=" + track_direction + ")" );
				QgsRuleBasedRendererV2::Rule *rule = new QgsRuleBasedRendererV2::Rule ( symbol, 0, 0, t2q( expression ) );
				root_rule->appendChild( rule );
			}
		}
	}

	auto qsm = []( const char *s1, const char *s2 )
	{
		QgsStringMap m;
		m.insert( s1, s2 );
		return m;
	};
	QgsSymbolV2 *s = QgsLineSymbolV2::createSimple( qsm( "color", "black" ) );
	QgsRuleBasedRendererV2::Rule *rule = new QgsRuleBasedRendererV2::Rule( s, 0, 0, QString(), QString(), QString(), true );
	root_rule->appendChild( rule );
	trackLayer->setRendererV2( new QgsRuleBasedRendererV2( root_rule ) );

	// Create a renderer to show the shortest path.

	QgsLineSymbolV2 *ls = QgsLineSymbolV2::createSimple( qsm( "color", "blue" ) );
	ls->setWidth( ROAD_WIDTH );
	ls->setOutputUnit( QgsSymbolV2::MapUnit );
	shortestPathLayer->setRendererV2( new QgsSingleSymbolRendererV2( ls ) );

	// Create a renderer to show the start point.

	QgsMarkerSymbolV2 *ms = QgsMarkerSymbolV2::createSimple( qsm( "color", "green" ) );
	ms->setSize( POINT_SIZE );
	ms->setOutputUnit( QgsSymbolV2::MapUnit );
	startPointLayer->setRendererV2( new QgsSingleSymbolRendererV2( ms ) );

	// Create a renderer to show the end point.

	QgsMarkerSymbolV2 *symbol = QgsMarkerSymbolV2::createSimple( qsm( "color", "red" ) );
	symbol->setSize( POINT_SIZE );
	symbol->setOutputUnit( QgsSymbolV2::MapUnit );
	endPointLayer->setRendererV2( new QgsSingleSymbolRendererV2( symbol ) );
}

void QbrtMapCanvas::setProjection()
{
///////////////////////////////////////////////////////////
	//QgsMapRenderer r = 
	QgsCoordinateReferenceSystem crs3( 4326 );
	QgsCoordinateReferenceSystem crs4( 32633  );

	QgsCoordinateTransform xform( crs3, crs4 );

	// forward transformation: src -> dest
	//pt1 = xform.transform(QgsPoint(18,5))

	// inverse transformation: dest -> src
	//pt2 = xform.transform(pt1, QgsCoordinateTransform.ReverseTransform)

	//this->setTransform( xform );
	QgsApplication::setPrefixPath("L:\\lib\\Graphics\\QGIS\\default\\bin\\x64\\Debug", true);
	this->setCrsTransformEnabled( true );

	const QString theProjString = "";
	//GEOCRS_ID
	//QgsCoordinateReferenceSystem::createFromProj4( theProjString );

	QgsCoordinateReferenceSystem crs( 4326, QgsCoordinateReferenceSystem::PostgisCrsId );
	//crs.createFromProj4( "+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs" );

	QString wkt = "GEOGCS[\"WGS84\", DATUM[\"WGS84\", SPHEROID[\"WGS84\", 6378137.0, 298.257223563]],PRIMEM[\"Greenwich\", 0.0], UNIT[\"degree\",0.017453292519943295],""AXIS[\"Longitude\",EAST], AXIS[\"Latitude\",NORTH]]";
	static QgsCoordinateReferenceSystem crs2( wkt );

	setDestinationCrs( crs );
	//this->mapRenderer()->setDestinationCrs( crs );	deprecated
	//this->setO


	//GADM
///////////////////////////////////////////////////////////
}

void QbrtMapCanvas::drawLines()
{
	//To show a polyline

	QgsRubberBand *r = new QgsRubberBand( this, false );  // False = not a polygon
	//QgsPoint points[] = { QgsPoint( -1, -1 ), QgsPoint( 0, 1 ), QgsPoint( 1, -1 ) };

	//QgsPolyline poly( 3, QgsPoint( -1, -1 ) );
	//poly << QgsPoint( 0, 1 ) << QgsPoint( 1, -1 );

	//r->setToGeometry( QgsGeometry::fromPolyline( poly ), nullptr );

	r->setToGeometry( QgsGeometry::fromPolyline( QgsPolyline( 3, QgsPoint( -2, -2 ) ) << QgsPoint( 3, 3 ) << QgsPoint( 2, -2 ) ), nullptr );
	r->setColor( QColor( 255, 0, 255 ) );
	r->setWidth( 3 );

	//To show a polygon

	QgsRubberBand *polygon = new QgsRubberBand( this, true );  // True = a polygon
	//QgsPoint points[] = { QgsPoint( -1, -1 ), QgsPoint( 0, 1 ), QgsPoint( 1, -1 ) };

	polygon->setToGeometry( QgsGeometry::fromPolygon( QgsPolygon( 1, QgsPolyline( 3, QgsPoint( -1, -1 ) ) << QgsPoint( 1, 1 ) << QgsPoint( 1, -1 ) ) ), nullptr );

	//Note that points for polygon is not a plain list: in fact, it 
	//is a list of rings containing linear rings of the polygon: first 
	//ring is the outer border, further (optional) rings correspond to holes in the polygon.
	//
	//Rubber bands allow some customization, namely to change their color and line width

	polygon->setColor( QColor( 255, 0, 0 ) );
	polygon->setWidth( 3 );


	//The canvas items are bound to the canvas scene. To temporarily hide them (and show 
	//again, use the hide() and show() combo. To completely remove the item, you have 
	//to remove it from the scene of the canvas

//	scene()->removeItem( r );

	//(in C++ it's possible to just delete the item, however in Python del r would just 
	//delete the reference and the object will still exist as it is owned by the canvas)
	//Rubber band can be also used for drawing points, however QgsVertexMarker class is 
	//better suited for this (QgsRubberBand would only draw a rectangle around the desired 
	//point). How to use the vertex marker

	QgsVertexMarker *m = new QgsVertexMarker( this );
	m->setCenter( QgsPoint( -3, 3 ) );

	//This will draw a red cross on position [0,0]. It is possible to customize the icon type, size, color and pen width

	m->setColor( QColor( 0, 255, 0 ) );
	m->setIconSize( 5 );
	m->setIconType( QgsVertexMarker::ICON_BOX ); // or ICON_CROSS, ICON_X
	m->setPenWidth( 3 );
}

void QbrtMapCanvas::addGrid()
{
	//this->
	//QgsMapLayerRegistry *reg = QgsMapLayerRegistry::instance();
	//auto mr = mapRenderer();
	//auto qc = MapComposer::MapComposer( qmlr=reg, qmr=mr );
	////3. Now, we are going to create some variables to shorten some unusually long method and object names:
	//setGridAnnoPos = qc.composerMap.setGridAnnotationPosition;
	//setGridAnnoDir = qc.composerMap.setGridAnnotationDirection;
	//qcm = QgsComposerMap;
	////4. Then, we enable the grid, set the line spacing, and use solid lines for the grid:
	//qc.composerMap.setGridEnabled( True );
	//qc.composerMap.setGridIntervalX( .75 );
	//qc.composerMap.setGridIntervalY( .75 );
	//qc.composerMap.setGridStyle( qcm.Solid );
	////5. Next, we enable the annotation numbers for coordinates and set the decimal precision to 0 for whole numbers:
	//qc.composerMap.setShowGridAnnotation(True);
	//qc.composerMap.setGridAnnotationPrecision(0);
	////6. Now, we go around the map composition frame and define locations and directions
	////for each set of grid lines, using our shorter variable names from the previous steps:
	//setGridAnnoPos(qcm.OutsideMapFrame, qcm.Top);
	//setGridAnnoDir(qcm.Horizontal, qcm.Top);
	//setGridAnnoPos(qcm.OutsideMapFrame, qcm.Bottom);
	//setGridAnnoDir(qcm.Horizontal, qcm.Bottom);
	//setGridAnnoPos(qcm.OutsideMapFrame, qcm.Left);
	//setGridAnnoDir(qcm.Vertical, qcm.Left);
	//setGridAnnoPos(qcm.OutsideMapFrame, qcm.Right);
	//setGridAnnoDir(qcm.Vertical, qcm.Right);
	////7. Finally, we set some additional styling for the grid lines and annotations before adding the whole map to the overall composition:
	//qc.composerMap.setAnnotationFrameDistance(1);
	//qc.composerMap.setGridPenWidth(.2);
	//qc.composerMap.setGridPenColor(QColor(0, 0, 0));
	//qc.composerMap.setAnnotationFontColor(QColor(0, 0, 0));
	//qc.c.addComposerMap(qc.composerMap);
	////8. We output the composition to an image:
	////qc.output("/qgis_data/map.jpg", "jpg
}




#include <qgsvectordataprovider.h>
//
// QGIS Map tools
//
#include "qgsmaptoolpan.h"
#include "qgsmaptoolzoom.h"
//

//#include <qgsfieldmap.h> 
//
//Labelling related
//
#include <qgslabel.h>
#include <qgslabelattributes.h>
#include <qgsfield.h>

void QbrtMapCanvas::addLabelsLayer()
{
	QString mVectorLayerPath         = "E:/VMachines/S3-ALTB/software/COTS/B/QGIS/QGIS-Code-Examples-master/3_basic_labelling/data";
	QString myLayerBaseName     = "test";
	QString myProviderName      = "ogr";

	QgsVectorLayer * mMainLayer = new QgsVectorLayer( mVectorLayerPath, myLayerBaseName, myProviderName );

	if ( mMainLayer->isValid() )
	{
		qDebug( "Layer is valid" );
	}
	else
	{
		qDebug( "Layer is NOT valid" );
		return;
	}

	//set up a renderer for the layer
	QgsSingleSymbolRendererV2 *mypRenderer = new QgsSingleSymbolRendererV2( QgsSymbolV2::defaultSymbol(mMainLayer->geometryType() ) );
	mMainLayer->setRendererV2( mypRenderer );

	//
	//set up labelling for the layer
	//

	//get the label instance associated with the layer
	QgsLabel * mypLabel;
	mypLabel = mMainLayer->label();
	//and the label attributes associated with the label
	QgsLabelAttributes * mypLabelAttributes;
	mypLabelAttributes = mypLabel->labelAttributes();

	//get the field list associated with the layer
	//we'll print the names out to console for diagnostic purposes
	QgsFields myFields = mMainLayer->dataProvider()->fields();
	for ( int i = 0; i < myFields.size(); i++ )
	{
		qDebug( "Field Name: " + QString( myFields[ i ].name() ).toLocal8Bit() );
	}
	//just use the last field's name in the fields list as the label field! 
	qDebug( "set label field to " + QString( myFields[ myFields.size() - 1 ].name() ).toLocal8Bit() );
	mypLabel->setLabelField( QgsLabel::Text, myFields.size() - 1 );
	//set the colour of the label text
	mypLabelAttributes->setColor( Qt::black );
	//create a 'halo' effect around each label so it
	//can still be read on dark backgrounds
	mypLabelAttributes->setBufferEnabled( true );
	mypLabelAttributes->setBufferColor( Qt::yellow );
	int myType = QgsLabelAttributes::PointUnits;
	mypLabelAttributes->setBufferSize( 1, myType );

	/*
	 * Here are a bunch of other things you can set based on values on a database field
	 * the second parameter in each case would be the field name from which the
	 * attribute can be retrieved.
	 mypLabel->setLabelField( QgsLabel::Family, "fontFamily" );
	 mypLabel->setLabelField( QgsLabel::Bold,  "fontIsBold" );
	 mypLabel->setLabelField( QgsLabel::Italic, "fontIsItalic"  );
	 mypLabel->setLabelField( QgsLabel::Underline, "fontIsUnderlined"  );
	 mypLabel->setLabelField( QgsLabel::Size, "fontSize" );
	 mypLabel->setLabelField( QgsLabel::BufferSize,"fontBufferSize" );
	 mypLabel->setLabelField( QgsLabel::XCoordinate, "labelX" );
	 mypLabel->setLabelField( QgsLabel::YCoordinate, "labelY");
	 mypLabel->setLabelField( QgsLabel::XOffset, "labelXOffset");
	 mypLabel->setLabelField( QgsLabel::YOffset, "labelYOffset");
	 mypLabel->setLabelField( QgsLabel::Alignment, "labelAlignment" );
	 mypLabel->setLabelField( QgsLabel::Angle, "labelAngle");
	 */

	//lastly we enable labelling!
	mMainLayer->enableLabels( true );

	// Add the Vector Layer to the Layer Registry
	QgsMapLayerRegistry::instance()->addMapLayer( mMainLayer, TRUE );

	// Add the Layer to the Layer Set
	//QList<QgsMapCanvasLayer> myLayerSet;
	mLayerSet.append( QgsMapCanvasLayer( mMainLayer ) );
	// set teh canvas to the extent of our layer
	setExtent( mMainLayer->extent() );
	// Set the Map Canvas Layer Set
	//mpMapCanvas->setLayerSet( mLayerSet );
}


//
#if defined(_WIN32)
	#if defined(_WIN64)
		#define PLATFORM_SUBDIR "x64"
	#else defined(_WIN32)
		#define PLATFORM_SUBDIR "Win32"
	#endif
#else
	#if defined(__LP64__) || defined(__x86_64__)
		#define PLATFORM_SUBDIR "x86_64"
	#else
		#define PLATFORM_SUBDIR "i386"
	#endif
#endif

#if defined(_DEBUG) || defined(DEBUG)
	#define CONFIG_SUBDIR "Debug"
#else
	#define CONFIG_SUBDIR "Release"
#endif

#if defined(_WIN32)
	#define QGIS_PLUGINS_SUBDIR "plugins"
#elif defined (__APPLE__)
    #define QGIS_PLUGINS_SUBDIR "QGIS.app/Contents/PlugIns/qgis"
#else
	#define QGIS_PLUGINS_SUBDIR "lib/qgis/plugins"
#endif

struct ApplicationDirectories
{
	std::string mBasePath;
	std::string mPlatform;
	std::string mConfiguration;

	std::string mQgisDir;
	std::string mQgisPluginsDir;

	std::string mExecutableDir;
	std::string mInternalDataDir;
	std::string mExternalDataDir;

	std::string mRasterLayerPath;
	std::string mVectorLayerPath;

	std::string mGlobeDir;

	static std::string computeBaseDirectory()
	{
		auto s3root = getenv( "S3ALTB_ROOT" );
		if ( s3root )
			return s3root;

		return std::string();
	}
	static std::string computeInternalDataDirectory( const std::string &ExecutableDir )
	{
		std::string InternalDataDir;
		auto s3data = getenv( "BRAT_DATA_DIR" );
		if ( s3data )
			InternalDataDir = s3data;
        else
        {
            InternalDataDir = getDirectoryFromPath( ExecutableDir );    //strip first parent directory (MacOS in mac)
        #if defined (__APPLE__)
            InternalDataDir = getDirectoryFromPath( InternalDataDir );  //strip Contents
            InternalDataDir = getDirectoryFromPath( InternalDataDir );  //strip brat.app
            InternalDataDir = getDirectoryFromPath( InternalDataDir );  //strip wherever brat.app is
        #endif
            InternalDataDir += "/data";
        }
        return InternalDataDir;
	}
private:
	ApplicationDirectories() :
		  mBasePath( computeBaseDirectory() )
		, mPlatform( PLATFORM_SUBDIR )
		, mConfiguration( CONFIG_SUBDIR )

		, mQgisDir( mBasePath + "/lib/Graphics/QGIS/default/bin/" + mPlatform + "/" + mConfiguration )
		, mQgisPluginsDir( mQgisDir + "/" + QGIS_PLUGINS_SUBDIR )

		, mExecutableDir( getDirectoryFromPath( qApp->argv()[ 0 ] ) )
		, mInternalDataDir( computeInternalDataDirectory( mExecutableDir ) )
		, mExternalDataDir( mBasePath + "/lib/data" )

		, mRasterLayerPath( mExternalDataDir + "/maps/NE1_HR_LC_SR_W_DR/NE1_HR_LC_SR_W_DR.tif" )
		, mVectorLayerPath( mExternalDataDir + "/maps/ne_10m_coastline/ne_10m_coastline.shp" )
	{
		//mGlobeDir = QDir::cleanPath( QgsApplication::pkgDataPath() + "/globe/gui" ).toStdString();
		//if ( QgsApplication::isRunningFromBuildDir() )
		//{
		//	mGlobeDir = QDir::cleanPath( QgsApplication::buildSourcePath() + "/src/plugins/globe/images/gui" ).toStdString();
		//}
		mGlobeDir = mExternalDataDir + "/globe/gui";
	}

public:
	static const ApplicationDirectories& instance()
	{
		static const ApplicationDirectories ad;
		if ( !ad.valid() )
			SimpleErrorBox("Some standard application directories or files are not valid.\nIt will continue but more or less serious errors are to be expected.\nUsers like you are a disgrace.");
		return ad;
	}

	bool valid() const 
	{ 
		return 
			!mBasePath.empty() && !mInternalDataDir.empty() &&
			IsDir( mBasePath ) && IsDir( mInternalDataDir ) &&
			IsFile( mRasterLayerPath) && IsFile( mVectorLayerPath ); 
	}
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//template<>
//QgsMapLayerRegistry* QgsSingleton<QgsMapLayerRegistry>::sInstance;

QbrtMapCanvas::QbrtMapCanvas(QWidget *parent) : base_t(parent)
{
	static const ApplicationDirectories &ad = ApplicationDirectories::instance();

	//char *argv[] =
	//{
	//	"",
	//	"L:\\project\\dev\\source\\data\\stuff\\gshhs_l.b",
	//	//"L:\\project\\dev\\source\\data\\stuff\\gshhs_l.shp",
	//};

	//const int argc = sizeof(argv) / sizeof(*argv);

	//if ( !gshhs( argc, argv ) )
	//	std::cout << "error" << std::endl;

    //connect( document(), SIGNAL(modificationChanged(bool)), this, SLOT(setWindowModified(bool)));
    //if ( qobject_cast< QMainWindow* >( parent ) )
    //{
    //    //assert( isMainChild() );
    //    connect( this, SIGNAL( setCurrentFile( const QbrtMapEditor* )), parent, SLOT(setCurrentFile( const QbrtMapEditor* )) );
    //}

    QString myLayerBaseName     = "italy";
    QString myProviderName      = "ogr";

    // Instantiate Provider Registry
    QgsProviderRegistry *preg = QgsProviderRegistry::instance( t2q( ad.mQgisPluginsDir ) );
	Q_UNUSED( preg );

	//QString source, dest;
	//if ( !readFileFromResource( ":/maps/ne_10m_coastline/ne_10m_coastline.shp", dest, true ) )
	//	std::cout << "problem" << std::endl;
	//else
	//	qDebug() << dest;
	
	mMainLayer = addOGRLayer( t2q( ad.mVectorLayerPath ) );
	mMainLayer->rendererV2()->symbols()[ 0 ]->setColor( "black" );

	mMainRasterLayer = addRasterLayer( t2q( ad.mRasterLayerPath ), "raster", "" );

    //mMainLayer =  new QgsVectorLayer(mVectorLayerPath, myLayerBaseName, myProviderName);
    //QgsSingleSymbolRendererV2 *mypRenderer = new QgsSingleSymbolRendererV2(QgsSymbolV2::defaultSymbol(mMainLayer->geometryType()));

    //mMainLayer->setRendererV2(mypRenderer);    mMainLayer->isValid() ? qDebug("Layer is valid") : qDebug("Layer is NOT valid");

    //// Add the Vector Layer to the Layer Registry
    //QgsMapLayerRegistry::instance()->addMapLayer(mMainLayer, TRUE);
    //// Add the Layer to the Layer Set
    //mLayerSet.append(QgsMapCanvasLayer(mMainLayer, TRUE));

	//setupDatabase();
	//setupMapLayers();

	//4. We need the index for the population value:
	//i = tractLyr.fieldNameIndex('POPULAT11')
	//5. Now, we get our census layer's features as an iterator:
	//features = tractLyr.getFeatures()
	//6. We need a data provider for the memory layer so that we can edit it:
	
	//auto vpr = popLyr->dataProvider();

    setExtent(mMainLayer->extent());
    enableAntiAliasing(true);
    setCanvasColor(QColor(255, 255, 255));
    freeze(false);

    //// Set the Map Canvas Layer Set
    //setLayerSet(mLayerSet);

    setVisible(true);
    refresh();
}

#include <osgViewer/Viewer>
#include "../Globe/globe_plugin.h"

QbrtMapEditor::QbrtMapEditor(QWidget *parent) : base_t(parent), m_ToolEditor( false )
{
    isUntitled = true;

    setWindowIcon(QPixmap(":/images/globe.png"));
    setWindowTitle("[*]");
    setAttribute(Qt::WA_DeleteOnClose);

    //createLayerTreeView();
	mSplitter = new QSplitter( Qt::Vertical, this );
	mMapCanvas = new QbrtMapCanvas( mSplitter );
	setCentralWidget( mSplitter );

	createToolBar();
	createGlobe();
}

void QbrtMapEditor::createToolBar()
{
	QToolBar *ToolBar = new QToolBar( this );
	ToolBar->setIconSize( QSize( IconSize, IconSize ) );
	ToolBar->setWindowTitle( "Map ToolBar" );
	ToolBar->setAllowedAreas( Qt::RightToolBarArea | Qt::LeftToolBarArea );

	//ToolBar->setStyleSheet("QToolBar { border: 2px }");
	setStyleSheet( "QMainWindow{background-color: darkgray;border: 1px solid black;}" );

	::insertToolBar( this, ToolBar, Qt::RightToolBarArea );
	//addToolBarBreak();

	mAction_View_Globe = new QAction( "View Globe", ToolBar );								//starts unchecked: actions in button menu
	mAction_View_Globe->setObjectName( QString::fromUtf8( "mAction_View_Globe" ) );
	mAction_View_Globe->setCheckable( true );
	mAction_View_Globe->setToolTip( "Show Globe" );
	mAction_View_Globe->setChecked( false );
	mAction_View_Globe->setEnabled( false );
	connect( mAction_View_Globe, SIGNAL( toggled( bool ) ), this, SLOT( action_View_Globe( bool ) ) );
	QIcon icon4;
	icon4.addFile( QString::fromUtf8( ":/images/globe.png" ), QSize(), QIcon::Normal, QIcon::Off );
	mAction_View_Globe->setIcon( icon4 );
	ToolBar->addAction( mAction_View_Globe );

	ToolBar->addSeparator();

	// Render suppression tool bar widget
	//
	mRenderSuppressionAction = new QAction( tr( "Render" ), ToolBar );	//mRenderSuppressionAction->setOrientation( Qt::Vertical );
	mRenderSuppressionAction->setObjectName( "mRenderSuppressionAction" );
	mRenderSuppressionAction->setCheckable( true );
	mRenderSuppressionAction->setChecked( true );
	mRenderSuppressionAction->setWhatsThis( tr( "When checked, the map layers "
		"are rendered in response to map navigation commands and other "
		"events. When not checked, no rendering is done. This allows you "
		"to add a large number of layers and symbolize them before rendering." ) );
	mRenderSuppressionAction->setToolTip( tr( "Toggle map rendering" ) );
	QIcon icon3;
	icon3.addFile( QString::fromUtf8( ":/images/render.png" ), QSize(), QIcon::Normal, QIcon::Off );
	mRenderSuppressionAction->setIcon( icon3 );
	ToolBar->addAction( mRenderSuppressionAction );

	connect( mRenderSuppressionAction, SIGNAL( toggled( bool ) ), mMapCanvas, SLOT( setRenderFlag( bool ) ) );

	// Add progress
	//
	mProgressBar = new QProgressBar( ToolBar );
	ToolBar->addWidget( mProgressBar );
	mProgressBar->setTextVisible( false );
	mProgressBar->setObjectName( "mProgressBar" );
	mProgressBar->setMaximumHeight( IconSize );	  	//mProgressBar->setMaximumWidth( IconSize );
	mProgressBar->setOrientation( Qt::Vertical );
	mProgressBar->setSizePolicy( QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred );
	mProgressBar->hide();
	mProgressBar->setWhatsThis( tr( "Progress bar that displays the status "
		"of rendering layers and other time-intensive operations" ) );
	mProgressBarAction = qobject_cast<QWidgetAction*>( ToolBar->addWidget( mProgressBar ) );

	connect( mMapCanvas, SIGNAL( renderStarting() ), this, SLOT( canvasRefreshStarted() ) );
	connect( mMapCanvas, SIGNAL( mapCanvasRefreshed() ), this, SLOT( canvasRefreshFinished() ) );
}

void QbrtMapEditor::action_View_Globe( bool visible )
{
	assert( mGlobeViewerWidget );
	mGlobeViewerWidget->setVisible( visible );
	//mMapCanvas->setVisible( !visible );
}

void QbrtMapEditor::createGlobe()
{
	static const ApplicationDirectories &ad = ApplicationDirectories::instance();

	if ( !mGlobe /* && mSplitter*/)
	{
		mGlobe = new GlobePlugin( nullptr, mMapCanvas );
		mOsgViewer = mGlobe->run( ad.mGlobeDir );						//can return null

		mGlobeViewerWidget = new osgEarth::QtGui::ViewerWidget( mOsgViewer );	//mGlobeViewerWidget->setGeometry( 100, 100, 1024, 800 );	//mGlobeViewerWidget->show();
		osg::Camera* camera = mOsgViewer->getCamera();
		osgQt::GraphicsWindowQt *gw = dynamic_cast<osgQt::GraphicsWindowQt*>( camera->getGraphicsContext() );
		gw->getGLWidget()->setMinimumSize( 10, 10 );	//CRITICAL: prevent image from disappearing forever when height is 0

		QSettings settings;

		if ( settings.value( "/Plugin-Globe/anti-aliasing", true ).toBool() )
		{
			QGLFormat glf = QGLFormat::defaultFormat();
			glf.setSampleBuffers( true );
			//bool aaLevelIsInt;
			//int aaLevel;
			//QString aaLevelStr = settings.value( "/Plugin-Globe/anti-aliasing-level", "" ).toString();
			//aaLevel = aaLevelStr.toInt( &aaLevelIsInt );
			//if ( aaLevelIsInt )
			//{
			//	glf.setSamples( aaLevel );
			//}
			mGlobeViewerWidget->setFormat( glf );
		}

		mSplitter->addWidget( mGlobeViewerWidget );

		if (mAction_View_Globe)
			mAction_View_Globe->setEnabled( true );
	}
}
void QbrtMapEditor::CreateWPlot( const CmdLineProcessor *proc, CWPlot* wplot )
{
	mMapCanvas->CreateWPlot( proc, wplot );
	createGlobe();
}

void QbrtMapEditor::canvasRefreshStarted()
{
	showProgress( -1, 0 ); // trick to make progress bar show busy indicator
}
void QbrtMapEditor::canvasRefreshFinished()
{
	showProgress( 0, 0 ); // stop the busy indicator
}
// slot to update the progress bar in the status bar
void QbrtMapEditor::showProgress( int theProgress, int theTotalSteps )
{
	if ( theProgress == theTotalSteps )
	{
		mProgressBar->reset();
		mProgressBarAction->setVisible(false);
	}
	else
	{
		//only call show if not already hidden to reduce flicker
		if ( !mProgressBar->isVisible() )
		{
			mProgressBarAction->setVisible(true);
		}
		mProgressBar->setMaximum( theTotalSteps );
		mProgressBar->setValue( theProgress );

		if ( mProgressBar->maximum() == 0 )
		{
			// for busy indicator (when minimum equals to maximum) the oxygen Qt style (used in KDE)
			// has some issues and does not start busy indicator animation. This is an ugly fix
			// that forces creation of a temporary progress bar that somehow resumes the animations.
			// Caution: looking at the code below may introduce mild pain in stomach.
			if ( strcmp( QApplication::style()->metaObject()->className(), "Oxygen::Style" ) == 0 )
			{
				QProgressBar pb;
				pb.setAttribute( Qt::WA_DontShowOnScreen ); // no visual annoyance
				pb.setMaximum( 0 );
				pb.show();
				qApp->processEvents();
			}
		}
	}
}



//virtual
QbrtMapEditor::~QbrtMapEditor()
{
	delete mGlobeViewerWidget;
	mGlobeViewerWidget = nullptr;
	delete mGlobe;
	mGlobe = nullptr;
}

void QbrtMapEditor::setupLayerTreeViewFromSettings()
{
	QSettings s;

	QgsLayerTreeModel* model = mLayerTreeView->layerTreeModel();
	model->setFlag( QgsLayerTreeModel::ShowRasterPreviewIcon, s.value( "/qgis/createRasterLegendIcons", false ).toBool() );

	QFont fontLayer, fontGroup;
	fontLayer.setBold( s.value( "/qgis/legendLayersBold", true ).toBool() );
	fontGroup.setBold( s.value( "/qgis/legendGroupsBold", false ).toBool() );
	model->setLayerTreeNodeFont( QgsLayerTreeNode::NodeLayer, fontLayer );
	model->setLayerTreeNodeFont( QgsLayerTreeNode::NodeGroup, fontGroup );
}

void QbrtMapEditor::createLayerTreeView()
{
	//no code originally commented out in this function

	mLayerTreeView = new QgsLayerTreeView( this );
	mLayerTreeView->setObjectName( "theLayerTreeView" ); // "theLayerTreeView" used to find this canonical instance later

	mLayerTreeView->setWhatsThis( tr( "Map legend that displays all the layers currently on the map canvas. Click on the check box to turn a layer on or off. Double click on a layer in the legend to customize its appearance and set other properties." ) );

	mLayerTreeDock = new QDockWidget( tr( "Layers" ), this );
	mLayerTreeDock->setObjectName( "Layers" );
	mLayerTreeDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

	QgsLayerTreeModel* model = new QgsLayerTreeModel( QgsProject::instance()->layerTreeRoot(), this );
#ifdef ENABLE_MODELTEST
	new ModelTest( model, this );
#endif
	model->setFlag( QgsLayerTreeModel::AllowNodeReorder );
	model->setFlag( QgsLayerTreeModel::AllowNodeRename );
	model->setFlag( QgsLayerTreeModel::AllowNodeChangeVisibility );
	model->setFlag( QgsLayerTreeModel::ShowLegendAsTree );
	model->setAutoCollapseLegendNodes( 10 );

	mLayerTreeView->setModel( model );
	mLayerTreeView->setMenuProvider( new QgsAppLayerTreeViewMenuProvider( mLayerTreeView, this ) );

	setupLayerTreeViewFromSettings();

	connect( mLayerTreeView, SIGNAL( doubleClicked( QModelIndex ) ), this, SLOT( layerTreeViewDoubleClicked( QModelIndex ) ) );
	connect( mLayerTreeView, SIGNAL( currentLayerChanged( QgsMapLayer* ) ), this, SLOT( activeLayerChanged( QgsMapLayer* ) ) );
	connect( mLayerTreeView->selectionModel(), SIGNAL( currentChanged( QModelIndex, QModelIndex ) ), this, SLOT( updateNewLayerInsertionPoint() ) );
	//connect( QgsProject::instance()->layerTreeRegistryBridge(), SIGNAL( addedLayersToLayerTree( QList<QgsMapLayer*> ) ),
	//	this, SLOT( autoSelectAddedLayer( QList<QgsMapLayer*> ) ) );

	// add group tool button
	QToolButton* btnAddGroup = new QToolButton;
	btnAddGroup->setAutoRaise( true );
	btnAddGroup->setIcon( QIcon( ":/images/actions/mActionFolder.png" ) );
	btnAddGroup->setToolTip( tr( "Add Group" ) );
	connect( btnAddGroup, SIGNAL( clicked() ), mLayerTreeView->defaultActions(), SLOT( addGroup() ) );

	// visibility groups tool button
	QToolButton* btnVisibilityPresets = new QToolButton;
	btnVisibilityPresets->setAutoRaise( true );
	btnVisibilityPresets->setToolTip( tr( "Manage Layer Visibility" ) );
	btnVisibilityPresets->setIcon( QIcon( ":/images/actions/mActionShowAllLayers.png" ) );
	btnVisibilityPresets->setPopupMode( QToolButton::InstantPopup );
	//btnVisibilityPresets->setMenu( QgsVisibilityPresets::instance()->menu() );

	// filter legend tool button
	//mBtnFilterLegend = new QToolButton;
	//mBtnFilterLegend->setAutoRaise( true );
	//mBtnFilterLegend->setCheckable( true );
	//mBtnFilterLegend->setToolTip( tr( "Filter Legend By Map Content" ) );
	//mBtnFilterLegend->setIcon( QgsApplication::getThemeIcon( "/mActionFilter.png" ) );
	//connect( mBtnFilterLegend, SIGNAL( clicked() ), this, SLOT( toggleFilterLegendByMap() ) );

	// expand / collapse tool buttons
	QToolButton* btnExpandAll = new QToolButton;
	btnExpandAll->setAutoRaise( true );
	btnExpandAll->setIcon( QIcon( ":/images/actions/mActionExpandTree.png" ) );
	btnExpandAll->setToolTip( tr( "Expand All" ) );
	connect( btnExpandAll, SIGNAL( clicked() ), mLayerTreeView, SLOT( expandAll() ) );
	QToolButton* btnCollapseAll = new QToolButton;
	btnCollapseAll->setAutoRaise( true );
	btnCollapseAll->setIcon( QIcon( ":/images/actions/mActionCollapseTree.png" ) );
	btnCollapseAll->setToolTip( tr( "Collapse All" ) );
	connect( btnCollapseAll, SIGNAL( clicked() ), mLayerTreeView, SLOT( collapseAll() ) );

	//QToolButton* btnRemoveItem = new QToolButton;
	//btnRemoveItem->setDefaultAction( this->mActionRemoveLayer );
	//btnRemoveItem->setAutoRaise( true );

	QHBoxLayout* toolbarLayout = new QHBoxLayout;
	toolbarLayout->setContentsMargins( QMargins( 5, 0, 5, 0 ) );
	toolbarLayout->addWidget( btnAddGroup );
	toolbarLayout->addWidget( btnVisibilityPresets );
	//toolbarLayout->addWidget( mBtnFilterLegend );
	toolbarLayout->addWidget( btnExpandAll );
	toolbarLayout->addWidget( btnCollapseAll );
	//toolbarLayout->addWidget( btnRemoveItem );
	toolbarLayout->addStretch();

	QVBoxLayout* vboxLayout = new QVBoxLayout;
	vboxLayout->setMargin( 0 );
	vboxLayout->addLayout( toolbarLayout );
	vboxLayout->addWidget( mLayerTreeView );

	QWidget* w = new QWidget;
	w->setLayout( vboxLayout );
	mLayerTreeDock->setWidget( w );
	addDockWidget( Qt::LeftDockWidgetArea, mLayerTreeDock );

	//mLayerTreeCanvasBridge = new QgsLayerTreeMapCanvasBridge( QgsProject::instance()->layerTreeRoot(), mMapCanvas, this );
	//connect( QgsProject::instance(), SIGNAL( writeProject( QDomDocument& ) ), mLayerTreeCanvasBridge, SLOT( writeProject( QDomDocument& ) ) );
	//connect( QgsProject::instance(), SIGNAL( readProject( QDomDocument ) ), mLayerTreeCanvasBridge, SLOT( readProject( QDomDocument ) ) );

	//bool otfTransformAutoEnable = QSettings().value( "/Projections/otfTransformAutoEnable", true ).toBool();
	//mLayerTreeCanvasBridge->setAutoEnableCrsTransform( otfTransformAutoEnable );

	//mMapLayerOrder = new QgsCustomLayerOrderWidget( mLayerTreeCanvasBridge, this );
	//mMapLayerOrder->setObjectName( "theMapLayerOrder" );

	//mMapLayerOrder->setWhatsThis( tr( "Map layer list that displays all layers in drawing order." ) );
	//mLayerOrderDock = new QDockWidget( tr( "Layer order" ), this );
	//mLayerOrderDock->setObjectName( "LayerOrder" );
	//mLayerOrderDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

	//mLayerOrderDock->setWidget( mMapLayerOrder );
	//addDockWidget( Qt::LeftDockWidgetArea, mLayerOrderDock );
	//mLayerOrderDock->hide();
}

void QbrtMapEditor::activeLayerChanged( QgsMapLayer* layer )
{
	if ( mMapCanvas )
		mMapCanvas->setCurrentLayer( layer );
}

void QbrtMapEditor::removeLayer()
{
	//no code originally commented out in this function

	if ( !mLayerTreeView )
		return;

	foreach( QgsMapLayer * layer, mLayerTreeView->selectedLayers() )
	{
		QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer*>( layer );
		if ( vlayer && vlayer->isEditable() /*&& !toggleEditing( vlayer, true ) */)
			return;
	}

	QList<QgsLayerTreeNode*> selectedNodes = mLayerTreeView->selectedNodes( true );

	//validate selection
	if ( selectedNodes.isEmpty() )
	{
		//messageBar()->pushMessage( tr( "No legend entries selected" ),
		//	tr( "Select the layers and groups you want to remove in the legend." ),
		//	QgsMessageBar::INFO, messageTimeout() );
		return;
	}

	bool promptConfirmation = QSettings().value( "qgis/askToDeleteLayers", true ).toBool();
	//display a warning
	if ( promptConfirmation && QMessageBox::warning( this, tr( "Remove layers and groups" ), tr( "Remove %n legend entries?", "number of legend items to remove", selectedNodes.count() ), QMessageBox::Ok | QMessageBox::Cancel ) == QMessageBox::Cancel )
	{
		return;
	}

	foreach( QgsLayerTreeNode* node, selectedNodes )
	{
		QgsLayerTreeGroup* parentGroup = qobject_cast<QgsLayerTreeGroup*>( node->parent() );
		if ( parentGroup )
			parentGroup->removeChildNode( node );
	}

	//showStatusMessage( tr( "%n legend entries removed.", "number of removed legend entries", selectedNodes.count() ) );

	mMapCanvas->refresh();
}


QgsRasterLayer* QbrtMapCanvas::addRasterLayer( const QString &layer_path, const QString &base_name, const QString &provider, QgsSymbolV2* symbol )	//symbol = nullptr
{
	static const std::string index_symbol = "#";
	static size_t index = 0;

	std::string name = q2a( base_name ) + index_symbol + n2s<std::string>(index++);
	auto l = provider.isEmpty() ? new QgsRasterLayer( layer_path, name.c_str() ) : new QgsRasterLayer( layer_path, name.c_str(), provider );
	//addRenderer( l, symbol );

	if ( l->isValid() ) {
							qDebug( "Layer is valid" );

		// Add the Vector Layer to the Layer Registry
		QgsMapLayerRegistry::instance()->addMapLayer(l, TRUE);

		// Add the Layer to the Layer Set
		mLayerSet.append(QgsMapCanvasLayer(l, TRUE));

		// Set the Map Canvas Layer Set			//TODO: check if we need to setLayerSet every time
		setLayerSet(mLayerSet);

		return l;
	}
							qDebug( "Layer is NOT valid" );
	delete l;
	return nullptr;
}

QgsVectorLayer* QbrtMapCanvas::addLayer( const QString &layer_path, const QString &base_name, const QString &provider, QgsSymbolV2* symbol )	//symbol = nullptr 
{
	static const std::string index_symbol = "#";
	static size_t index = 0;

	std::string name = q2a( base_name ) + index_symbol + n2s<std::string>(index++);
	auto l = new QgsVectorLayer( layer_path, name.c_str(), provider );
	addRenderer( l, symbol );

	if ( l->isValid() ) {
							qDebug( "Layer is valid" );

		// Add the Vector Layer to the Layer Registry
		QgsMapLayerRegistry::instance()->addMapLayer(l, TRUE);

		// Add the Layer to the Layer Set
		mLayerSet.append(QgsMapCanvasLayer(l, TRUE));

		// Set the Map Canvas Layer Set			//TODO: check if we need to setLayerSet every time
		setLayerSet(mLayerSet);

		return l;
	}
							qDebug( "Layer is NOT valid" );
	delete l;
	return nullptr;
}

QgsRubberBand* QbrtMapCanvas::addRBPoint( double lon, double lat, QColor color, QgsVectorLayer *layer )	//layer = nullptr  
{
	QgsRubberBand *r = new QgsRubberBand( this, QGis::Point );
	r->setToGeometry( QgsGeometry::fromPoint( QgsPoint( lon, lat ) ), layer );
	r->setColor( color );
	r->setIconSize( 1 );
	r->setWidth( 1 );

	return r;
}
QgsRubberBand* QbrtMapCanvas::addRBLine( QgsPolyline points, QColor color, QgsVectorLayer *layer )	//layer = nullptr  
{
	QgsRubberBand *r = new QgsRubberBand( this, QGis::Line );			//deprecated: False = not a polygon
	r->setToGeometry( QgsGeometry::fromPolyline( points ), layer );
	r->setColor( color );
	r->setIconSize( 1 );
	r->setWidth( 1 );

	return r;
}

#include <qgsgraduatedsymbolrendererv2.h>
#include <qgssinglebandpseudocolorrenderer.h>
#include <qgsrastershader.h>

QgsVectorLayer* QbrtMapCanvas::addMemoryLayer( QgsSymbolV2* symbol )		//symbol = nullptr 
{
	auto ml = addLayer( "Point?crs=EPSG:4326&field=height:double&field=name:string(255)&index=yes", "mem", "memory" );	// , symbol );

	auto myTargetField = "height";
	auto myRangeList = new QgsRangeList;
	auto myOpacity = 1;
	// Make our first symbol and range...
	auto myMin = 1491.0;
	auto myMax = 1491.5;
	auto myLabel = "Group 1";
	auto myColour = QColor( "#ffee00" );
	auto mySymbol1 = createPointSymbol( 0.5, myColour );
	mySymbol1->setAlpha( myOpacity );
	auto myRange1 = new QgsRendererRangeV2( myMin, myMax, mySymbol1, myLabel );
	myRangeList->append( *myRange1 );
	//now make another symbol and range...
	myMin = 1491.6;
	myMax = 1492.0;
	myLabel = "Group 2";
	myColour = QColor( "#00eeff" );
	auto mySymbol2 = createPointSymbol( 0.5, myColour );
	mySymbol2->setAlpha( myOpacity );
	auto myRange2 = new QgsRendererRangeV2( myMin, myMax, mySymbol2, myLabel );
	myRangeList->append( *myRange2 );
	auto myRenderer = new QgsGraduatedSymbolRendererV2( "", *myRangeList );
	myRenderer->setMode( QgsGraduatedSymbolRendererV2::Jenks );
	myRenderer->setClassAttribute( myTargetField );

	ml->setRendererV2( myRenderer );

	return ml;

	//return addLayer( "Point?crs=EPSG:4326&field=height:double&field=name:string(255)&index=yes", "mem", "memory", symbol );
}

//	Note that you can use strings like "red" instead of Qt::red !!!
//
//static 
QgsSymbolV2* QbrtMapCanvas::createPointSymbol( double width, const QColor &color )
{
	auto qsm = []( const char *s1, const std::string &s2 )
	{
		QgsStringMap m;
		m.insert( s1, s2.c_str() );
		return m;
	};

	QgsSymbolV2 *s = QgsMarkerSymbolV2::createSimple( qsm( "","" ) );
	s->deleteSymbolLayer( 0 );		// Remove default symbol layer.
	//s->setColor( lineColor );
	//s->setMapUnitScale()

	auto symbolLayer = new QgsSimpleMarkerSymbolLayerV2;
	symbolLayer->setColor( color );
	symbolLayer->setSize( width );
	symbolLayer->setOutlineStyle( Qt::NoPen );
	s->appendSymbolLayer( symbolLayer );

	return s;
}

//	Note that you can use strings like "red" instead of Qt::red !!!
//
//static 
QgsSymbolV2* QbrtMapCanvas::createLineSymbol( double width, const QColor &color )
{
	auto qsm = []( const char *s1, const std::string &s2 )
	{
		QgsStringMap m;
		m.insert( s1, s2.c_str() );
		return m;
	};

	QgsSymbolV2 *s = QgsLineSymbolV2::createSimple( qsm( "", "" ) );
	s->deleteSymbolLayer( 0 ); // Remove default symbol layer.
	//s->setColor( lineColor );
	//s->setMapUnitScale()

	auto symbolLayer = new QgsSimpleLineSymbolLayerV2;
	symbolLayer->setWidth( width );
	symbolLayer->setWidthUnit( QgsSymbolV2::MapUnit );
	symbolLayer->setColor( QColor( color ) );
	s->appendSymbolLayer( symbolLayer );

	return s;
}

//static 
QgsFeatureList& QbrtMapCanvas::createPointFeature( QgsFeatureList &list, double lon, double lat, double value )
{
	QgsFields fields;
	fields.append( QgsField("height", QVariant::Double),  QgsFields::OriginProvider );
	QgsFeature *f = new QgsFeature( fields );
	f->setGeometry( QgsGeometry::fromPoint( QgsPoint( lon, lat ) ) );
	f->setAttribute( "height", value );					//not working
	list.append( *f );

	return list;
}

//static 
QgsFeatureList& QbrtMapCanvas::createLineFeature( QgsFeatureList &list, QgsPolyline points )
{
	auto line = QgsGeometry::fromPolyline( points );
	QgsFeature *f = new QgsFeature();
	f->setGeometry( line );
	list.append( *f );

	return list;
}



//virtual
QbrtMapCanvas::~QbrtMapCanvas()
{
	assert( !isDrawing() );
}

//virtual 
void QbrtMapCanvas::keyPressEvent( QKeyEvent * e ) //override
{
	switch ( e->key() )
	{
		case Qt::Key_Home:			QgsDebugMsg( "Home" );

			zoomToFullExtent();
			break;
		default:
			base_t::keyPressEvent( e );
	}
}

void QbrtMapEditor::globeSettings()
{
	assert( mGlobe );

	mGlobe->settings();
}

void QbrtMapCanvas::CreateWPlot( const CmdLineProcessor *proc, CWPlot* wplot )
{
	QSize size;
	QPoint pos;

	wplot->GetInfo();

	CWorldPlotProperty* wPlotProperty = proc->GetWorldPlotProperty( 0 );
	UNUSED( wPlotProperty );

	//TODO CWorldPlotFrame* frame = new CWorldPlotFrame( NULL, -1, title, wPlotProperty, pos, size );

	// for geostrophic velocity
	CPlotField * northField =NULL;
	CPlotField * eastField =NULL;
	for ( CObArray::iterator itField = wplot->m_fields.begin(); itField != wplot->m_fields.end(); itField++ )
	{
		CPlotField* field = CPlotField::GetPlotField( *itField );

		if ( field->m_internalFiles.empty() )
			continue;

		if ( field->m_worldProps->m_northComponent && northField == NULL ) {
			northField = field;
			continue;
		}
		else
		if ( field->m_worldProps->m_eastComponent && eastField == NULL ) {
			eastField = field;
			continue;
		}

		// otherwise just add it as regular data
		CGeoMap *geoMap = new CGeoMap( field );
		AddData( geoMap );
	}

	// we have a Vector Plot!
	if ( northField != NULL && eastField != NULL ) {

		CGeoVelocityMap *gvelocityMap = new CGeoVelocityMap( northField, eastField );
		gvelocityMap->SetIsGlyph( true );
		AddData( gvelocityMap );
	}
	else if ( northField != eastField ) {
		CException e( "CBratDisplayApp::CreateWPlot - incomplete std::vector plot components", BRATHL_INCONSISTENCY_ERROR );
		CTrace::Tracer( "%s", e.what() );
		throw ( e );
	}
}
//void CWorldPlotRenderer::AddData( CWorldPlotData* pdata )
//{
//	pdata->SetRenderer( m_vtkRend );
//
//	m_actors.Insert( pdata );
//	m_transformations->AddItem( pdata->GetTransform() );
//	pdata->GetVtkTransform()->SetTransform( pdata->GetTransform() );
//
//	ResetTextActor();
//}

#define USE_POINTS		//(**)
#define USE_FEATURES	//(***)
/*
230 108	120 2

	89	 86
	18	 19
		 15
		  2
   107	122
	 1
*/  
//
//(*)	Using memory layer because: "OGR error creating feature 0: CreateFeature : unsupported operation on a read-only datasource."
//(**)	Using point and not line features because: 
//			1) (If using main layer and "this" shapefile) "Feature creation error (OGR error: Attempt to write non-linestring (POINT) geometry to ARC type shapefile.)"
//			2) There is no algorithm to "bezier" the points collection to an spline
//			3) cannot color the values over a line (unless with another layer, a point layer, but then, why the line layer?)
//(***) Using features and not rubberbands because these are not projected in the globe
//
void QbrtMapCanvas::AddData( CWorldPlotData* pdata )
{
	CGeoMap* geoMap = dynamic_cast<CGeoMap*>( pdata );

	auto IsValidPoint = [&geoMap]( int32_t i )
	{
		bool bOk = geoMap->bits[ i ];

		//	  if (Projection == VTK_PROJ2D_MERCATOR)
		//	  {
		bOk &= geoMap->valids[ i ];
		//	  }
		//
		return bOk;
	};


	auto const size = geoMap->vals.size();
	QgsFeatureList flist;

#if defined (USE_POINTS)	//(**)

	for ( auto i = 0u; i < size; ++ i )
	{
		if ( !IsValidPoint( i ) )
			continue;

		auto x = i % geoMap->lons.size(); // ( x * geoMap->lats.size() ) + i;
		auto y = i / geoMap->lons.size(); // ( x * geoMap->lats.size() ) + i;

#if defined (USE_FEATURES) //(***)
		createPointFeature( flist, geoMap->lons.at( x ), geoMap->lats.at( y ), geoMap->vals[ i ] );
		//createPointFeature( flist, geoMap->lons.at( x ), geoMap->lats.at( y ), QColor( 0, (unsigned char)(geoMap->vals[ i ]), 0 ) );
#else
		addRBPoint( geoMap->lons.at( x ), geoMap->lats.at( y ), QColor( (long)(geoMap->vals[ i ]) ), mMainLayer );
#endif
	}

#if defined (USE_FEATURES)
	auto memL = addMemoryLayer( createPointSymbol( 0.5, Qt::red ) );	//(*)	//note that you can use strings like "red" instead!!!
	memL->dataProvider()->addFeatures( flist );
	//memL->updateExtents();
	//refresh();
#endif

	return;

#else		//(**)

	QgsPolyline points;
	for ( auto i = 0; i < size; ++ i ) 
	{
		if ( !IsValidPoint(i) )
			continue;

		auto x = i % geoMap->lons.size();
		auto y = i / geoMap->lons.size();

		points.append( QgsPoint( geoMap->lons.at( x ), geoMap->lats.at( y ) ) );
	}
#if !defined (USE_FEATURES) //(***)
	auto memL = addMemoryLayer( createLineSymbol( 0.5, Qt::red ) );	//(*)	//note that you can use strings like "red" instead!!!
	createLineFeature( flist, points );						
	memL->dataProvider()->addFeatures( flist );				
	//memL->updateExtents();
	//refresh();
#else
	addRBLine( points, QColor( 0, 255, 0 ), mMainLayer );	
#endif

	return;

#endif

	//femm: This is CWorldPlotPanel::AddData

	//femm: the important part
	//if ( pdata->GetColorBarRenderer() != NULL )
	//	m_vtkWidget->GetRenderWindow()->AddRenderer( pdata->GetColorBarRenderer()->GetVtkRenderer() );
	//m_plotRenderer->AddData( pdata );


	//femm: the less important part
	//CGeoMap* geoMap = dynamic_cast<CGeoMap*>( pdata );
	//if ( geoMap != NULL )
	//{
	//	wxString textLayer = wxString::Format( "%s", geoMap->GetDataName().c_str() );

	//	m_plotPropertyTab->GetLayerChoice()->Append( textLayer, static_cast<void*>( geoMap ) );
	//	m_plotPropertyTab->SetCurrentLayer( 0 );
	//}

	//int32_t nFrames = 1;
	//if ( geoMap != NULL )
	//	nFrames = geoMap->GetNrMaps();

	//m_animationToolbar->SetMaxFrame( nFrames );
}



void QbrtMapEditor::ToolEditor( bool tool )
{
    m_ToolEditor = tool;
    setWindowModified( false );
    disconnect( this, SLOT(setWindowModified(bool)) );
}

//////////////////////////////////////////////////////////////////////////
//                          ACCESS ACTIONS
//////////////////////////////////////////////////////////////////////////


//bool QbrtMapEditor::isEmpty() const
//{
//    return ::isEmpty( this );
//}

bool QbrtMapEditor::isMDIChild() const
{
    SimpleMsgBox( "bool QbrtMapEditor::isMDIChild() const \n Not implemented yet..." );
    return false;
}

//QString QbrtMapEditor::getSelectedText()
//{
//    return ::getSelectedText( this );
//}


//////////////////////////////////////////////////////////////////////////
//                          UPDATE ACTIONS
//////////////////////////////////////////////////////////////////////////


//this is very nice but does not free us from manually updating the ations in the Edit update menu function
//
//void QbrtMapEditor::connectSaveAvailableAction( QAction *pa )      {    connect( document(), SIGNAL(modificationChanged(bool)), pa, SLOT(setEnabled(bool)));}

//void QbrtMapEditor::connectUndoAvailableAction( QAction *pa )      {    connect( document(), SIGNAL( undoAvailable(bool)), pa, SLOT(setEnabled(bool)) );}
//void QbrtMapEditor::connectRedoAvailableAction( QAction *pa )      {    connect( document(), SIGNAL( redoAvailable(bool)), pa, SLOT(setEnabled(bool)) );}
void QbrtMapEditor::connectCutAvailableAction( QAction *pa )       {    connect( this, SIGNAL( copyAvailable(bool)), pa, SLOT(setEnabled(bool)) );}
void QbrtMapEditor::connectCopyAvailableAction( QAction *pa )      {    connect( this, SIGNAL( copyAvailable(bool)), pa, SLOT(setEnabled(bool)) );}
void QbrtMapEditor::connectDeleteSelAvailableAction( QAction *pa ) {    connect( this, SIGNAL( copyAvailable(bool)), pa, SLOT(setEnabled(bool)) );}

void  QbrtMapEditor::focusInEvent ( QFocusEvent * event )
{
    base_t::focusInEvent ( event );
    //if ( m_ToolEditor )
        emit toolWindowActivated( this );
}

//////////////////////////////////////////////////////////////////////////
//                              FILE ACTIONS
//////////////////////////////////////////////////////////////////////////


enum EFileType {
    e_txt,
    e_html,
    e_odt,
    e_unknown
};

inline EFileType TypeFromExtension( const QString &fileName )
{
    if ( fileName.endsWith(".odt", Qt::CaseInsensitive) )       //Qt does not read, write only option
        return e_odt;
    if (    fileName.endsWith(".htm", Qt::CaseInsensitive)
         || fileName.endsWith(".html", Qt::CaseInsensitive)
         || fileName.endsWith(".rtf", Qt::CaseInsensitive)      //mightBeRichText returns false for this (...) and Qt really reads it as text
         )
        return e_html;
    if ( fileName.endsWith(".txt", Qt::CaseInsensitive) )       //default on write
        return e_txt;
    return e_unknown;
}

void QbrtMapEditor::documentWasModified()
{
    setWindowModified(true);
}

QString QbrtMapEditor::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}


/////////////////////////////////////////////////////////////////////////
// FILE ACTIONS - OPEN/ INPUT
/////////////////////////////////////////////////////////////////////////



void QbrtMapEditor::setCurrentFile( const QString &fileName )
{
    curFile = fileName;

    isUntitled = false;
    windowMenuAction()->setText(strippedName(curFile));
//    document()->setModified(false);
    setWindowTitle(strippedName(curFile) + "[*]");
    setWindowModified(false);
    emit setCurrentFile( this );
}


void QbrtMapEditor::newFile()
{
    static int documentNumber = 1;

    curFile = tr("document%1.txt").arg(documentNumber);
    setWindowTitle(curFile + "[*]");
    windowMenuAction()->setText(curFile);
    isUntitled = true;
    ++documentNumber;
}


bool QbrtMapEditor::readFile( const QString &fileName )
{
    if (!QFile::exists( fileName ))
        return false;

	assert( false );
	
	throw;
}


QbrtMapEditor *QbrtMapEditor::openFile(const QString &fileName, QWidget *parent)
{
    QbrtMapEditor *editor = new QbrtMapEditor( parent );
    if (editor->readFile( fileName )) {
        editor->setCurrentFile( fileName );
        return editor;
    } else {
        delete editor;
        return 0;
    }
}

bool QbrtMapEditor::reOpen()
{
    if ( !IsUntitled() && readFile( curFile ) )
        setCurrentFile( curFile );
    else
        return false;
    return true;
}

QbrtMapEditor *QbrtMapEditor::open(QWidget *parent)
{
    static const TBackServices &m_bs = GetBackServices();

    QString fileName = QFileDialog::getOpenFileName( parent, tr("Open"), m_bs.GetDataPath(),
                                                     tr("Expressions (*.mm *.txt);;HTML-Files (*.htm *.html);;All Files (*)") );
    if (fileName.isEmpty())
        return 0;

    return openFile(fileName, parent);
}


/////////////////////////////////////////////////////////////////////////
// FILE ACTIONS - CLOSE / OUTPUT ACTIONS
/////////////////////////////////////////////////////////////////////////

bool QbrtMapEditor::writeFile( const QString &fileName )
{
    //writing, the old way
    //
//    QFile file(fileName);
//    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
//        QMessageBox::warning(this, tr("MDI QbrtMapEditor"),
//                             tr("Cannot write file %1:\n%2.")
//                             .arg(file.fileName())
//                             .arg(file.errorString()));
//        return false;
//    }

    QTextDocumentWriter writer( fileName );
    bool success = false;
    {
        WaitCursor wait;

        //writing, the old way
        //
        //QTextStream out(&file);
        //out << toPlainText();

        if ( TypeFromExtension( fileName ) == e_html )
            writer.setFormat( "HTML" );
        else
        if ( TypeFromExtension( fileName ) == e_odt )
            writer.setFormat( "ODF" );
        else
        if ( TypeFromExtension( fileName ) == e_txt )
            writer.setFormat( "plaintext" );
        else
            writer.setFormat( "plaintext" );

        //===> success = writer.write( document() );
    }
    if ( !success )
        QMessageBox::warning(this, tr("MDI QbrtMapEditor"),
                             tr("Cannot write file %1:\n%2.")
                             .arg( fileName )
                             .arg( writer.device()->errorString() ) );
    return success;
}


bool QbrtMapEditor::saveFile(const QString &fileName)
{
    if (writeFile(fileName))
    {
        setCurrentFile(fileName);
        return true;
    }
    return false;
}


bool QbrtMapEditor::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As..."), curFile,
                                                    tr("Expressions (*.mm *.txt);;HTML-Files (*.htm *.html);;All Files (*);;ODF files (*.odt)"));
    if ( fileName.isEmpty() )
        return false;

    if ( TypeFromExtension( fileName ) == e_unknown )
        fileName += ".txt"; // default
        //fileName += ".odt"; // default

    return saveFile( fileName );
}


bool QbrtMapEditor::save()
{
    if (isUntitled) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

void QbrtMapEditor::closeEvent(QCloseEvent *event)
{
    if (okToContinue()) {
        event->accept();
    } else {
        event->ignore();
    }
}

bool QbrtMapEditor::okToContinue()
{
    //if (curFile.startsWith(QLatin1String(":/")))
    //    return true;

	if ( mMapCanvas )
		mMapCanvas->stopRendering();

	return !mMapCanvas || !mMapCanvas->isDrawing();

    //if ( document()->isModified() ) {
    //    int r = QMessageBox::warning(this, tr("MDI QbrtMapEditor"),
    //                    tr("File %1 has been modified.\n"
    //                       "Do you want to save your changes?")
    //                    .arg(strippedName(curFile)),
    //                    QMessageBox::Yes | QMessageBox::No
    //                    | QMessageBox::Cancel);
    //    if (r == QMessageBox::Yes) {
    //        return save();
    //    } else if (r == QMessageBox::Cancel) {
    //        return false;
    //    }
    //}
    return true;
}



//////////////////////////////////////////////////////////////////////////
//                              ??? ACTIONS
//////////////////////////////////////////////////////////////////////////


QSize QbrtMapEditor::sizeHint() const
{
    return QSize(72 * fontMetrics().width('x'),
                 25 * fontMetrics().lineSpacing());
}



//////////////////////////////////////////////////////////////////////////
//                              FORMAT ACTIONS
//////////////////////////////////////////////////////////////////////////


//void QbrtMapEditor::mergeFormatOnWordOrSelection( const QTextCharFormat &format )
//{
//    QTextCursor cursor = textCursor();
//    if (!cursor.hasSelection())
//        cursor.select( QTextCursor::WordUnderCursor );
//    cursor.mergeCharFormat( format );
//    mergeCurrentCharFormat( format );
//}
//
//void QbrtMapEditor::toBold( bool bold )
//{
//    QTextCharFormat fmt;
//    fmt.setFontWeight( bold ? QFont::Bold : QFont::Normal );
//    mergeFormatOnWordOrSelection( fmt );
//}
//
//void QbrtMapEditor::toItalic( bool italic )
//{
//    QTextCharFormat fmt;
//    fmt.setFontItalic( italic );
//    mergeFormatOnWordOrSelection( fmt );
//}
//
//void QbrtMapEditor::toUnderline( bool underline )
//{
//    QTextCharFormat fmt;
//    fmt.setFontUnderline( underline );
//    mergeFormatOnWordOrSelection( fmt );
//}

//bool QbrtMapEditor::selectFont()
//{
//    bool ok;
//    QFont f = QFontDialog::getFont( &ok, currentFont(), this ); // or font(), for all text
//    if (ok) {
//        setCurrentFont( f );                                    // or setFont(), for all text
//    }
//    return ok;
//}
//
//bool QbrtMapEditor::selectColor()
//{
//    QColor col = QColorDialog::getColor( textColor(), this );
//    if ( !col.isValid() )
//        return false;
//    QTextCharFormat fmt;
//    fmt.setForeground(col);
//    mergeFormatOnWordOrSelection( fmt );
//    return true;
//}
//
//void QbrtMapEditor::toFontFamily( const QString &f )
//{
//    QTextCharFormat fmt;
//    fmt.setFontFamily(f);
//    mergeFormatOnWordOrSelection( fmt );
//}
//
//void QbrtMapEditor::toFontSize( const QString &p )
//{
//    qreal pointSize = p.toFloat();
//    if (p.toFloat() > 0) {
//        QTextCharFormat fmt;
//        fmt.setFontPointSize(pointSize);
//        mergeFormatOnWordOrSelection( fmt );
//    }
//}
//
//void QbrtMapEditor::toListStyle( QTextListFormat::Style style/* = QTextListFormat::ListDisc */)
//{
//    QTextCursor cursor = textCursor();
//
//    if ( style != QTextListFormat::ListStyleUndefined )
//    {
//        cursor.beginEditBlock();
//        QTextBlockFormat blockFmt = cursor.blockFormat();
//        QTextListFormat listFmt;
//        if (cursor.currentList())
//        {
//            listFmt = cursor.currentList()->format();
//        } else {
//            listFmt.setIndent(blockFmt.indent() + 1);
//            blockFmt.setIndent(0);
//            cursor.setBlockFormat(blockFmt);
//        }
//        listFmt.setStyle(style);
//        cursor.createList(listFmt);
//        cursor.endEditBlock();
//    } else {
//        // ####
//        QTextBlockFormat bfmt;
//        bfmt.setObjectIndex(-1);
//        cursor.mergeBlockFormat(bfmt);
//    }
//}


//////////////////////////////////////////////////////////////////////////
//                              PRINT ACTIONS
//////////////////////////////////////////////////////////////////////////


//void QbrtMapEditor::PrintToPdf()
//{
//#ifndef QT_NO_PRINTER
////! [0]
//    QString fileName = QFileDialog::getSaveFileName(this, "Export PDF", QString(), "*.pdf");
//    if (!fileName.isEmpty()) {
//        if (QFileInfo(fileName).suffix().isEmpty())
//            fileName.append(".pdf");
//        QPrinter printer(QPrinter::HighResolution);
//        printer.setOutputFormat(QPrinter::PdfFormat);
//        printer.setOutputFileName(fileName);
//        document()->print(&printer);
//    }
////! [0]
//#endif
//}
//
//void QbrtMapEditor::PrintToPrinter()
//{
//#ifndef QT_NO_PRINTER
//    QPrinter printer(QPrinter::HighResolution);
//    QPrintDialog *dlg = new QPrintDialog(&printer, this);
//    if (textCursor().hasSelection())
//        dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);
//    dlg->setWindowTitle(tr("Print Document"));
//    if (dlg->exec() == QDialog::Accepted) {
//        print(&printer);
//    }
//    delete dlg;
//#endif
//}
//
//void QbrtMapEditor::PrintPreview()
//{
//#ifndef QT_NO_PRINTER
//    QPrinter printer(QPrinter::HighResolution);
//    QPrintPreviewDialog preview(&printer, this);
//    connect(&preview, SIGNAL(paintRequested(QPrinter*)), SLOT(printPreviewSlot(QPrinter*)));
//    preview.exec();
//#endif
//}

//void QbrtMapEditor::printPreviewSlot(QPrinter *printer)
//{
//#ifdef QT_NO_PRINTER
//    Q_UNUSED(printer);
//#else
//    print(printer);
//#endif
//}
//


#define _POSIX_SOURCE 1        /* GSHHS code is POSIX compliant */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

//#ifdef WIN32
//#pragma warning( disable : 4996 )
//#endif

#ifndef M_PI
#define M_PI          3.14159265358979323846
#endif

#ifndef SEEK_CUR    /* For really ancient systems */
#define SEEK_CUR 1
#endif

#define GSHHS_DATA_RELEASE    7    /* For v2.0 data set */
#define GSHHS_DATA_VERSION    "2.0"    /* For v2.0 data set */
#define GSHHS_PROG_VERSION    "1.12"

#define GSHHS_SCL    1.0e-6    /* Convert micro-degrees to degrees */

/* For byte swapping on little-endian systems (GSHHS is defined to be bigendian) */

#define swabi4(i4) (((i4) >> 24) + (((i4) >> 8) & 65280) + (((i4) & 65280) << 8) + (((i4) & 255) << 24))

const char * file2 = "L:\\project\\dev\\source\\data\\stuff\\ascii.dat";


struct GSHHS {    /* Global Self-consistent Hierarchical High-resolution Shorelines */
    int id;        /* Unique polygon id number, starting at 0 */
    int n;        /* Number of points in this polygon */
    int flag;    /* = level + version << 8 + greenwich << 16 + source << 24 + river << 25 */
    /* flag contains 5 items, as follows:
     * low byte:    level = flag & 255: Values: 1 land, 2 lake, 3 island_in_lake, 4 pond_in_island_in_lake
     * 2nd byte:    version = (flag >> 8) & 255: Values: Should be 7 for GSHHS release 7
     * 3rd byte:    greenwich = (flag >> 16) & 1: Values: Greenwich is 1 if Greenwich is crossed
     * 4th byte:    source = (flag >> 24) & 1: Values: 0 = CIA WDBII, 1 = WVS
     * 4th byte:    river = (flag >> 25) & 1: Values: 0 = not set, 1 = river-lake and level = 2
     */
    int west, east, south, north;    /* min/max extent in micro-degrees */
    int area;    /* Area of polygon in 1/10 km^2 */
    int area_full;    /* Area of original full-resolution polygon in 1/10 km^2 */
    int container;    /* Id of container polygon that encloses this polygon (-1 if none) */
    int ancestor;    /* Id of ancestor polygon in the full resolution set that was the source of this polygon (-1 if none) */
};

struct    TPOINT {    /* Each lon, lat pair is stored in micro-degrees in 4-byte integer format */
    int    x;
    int    y;
};


bool gshhs(int argc, char **argv)
{
    double w, e, s, n, area, f_area, lon, lat;
    char source, kind[2] = {'P', 'L'}, c = '>', *file = NULL;
    const char *name[2] = {"polygon", "line"};
    char container[800], ancestor[800];
    FILE *fp = NULL;
    FILE *fp2 = NULL;
    int k, line, max_east = 270000000, info, single, error, ID, flip;
    int  OK, level, version, greenwich, river, src, msformat = 0;
    struct    TPOINT p;
    struct GSHHS h;

    info = single = error = ID = 0;
    for (k = 1; k < argc; k++) {
        if (argv[k][0] == '-') {    /* Option switch */
            switch (argv[k][1]) {
                case 'L':
                    info = 1;
                    break;
                case 'M':
                    msformat = 1;
                    break;
                case 'I':
                    single = 1;
                    ID = atoi (&argv[k][2]);
                    break;
                default:
                    error++;
                    break;
            }
        }
        else
            file = argv[k];
    }

    if (!file) {
        fprintf (stderr, "gshhs: No data file given!\n");
        error++;
    }
    if (argc == 1 || error) {
        fprintf (stderr, "gshhs %s - ASCII export of GSHHS %s data\n", GSHHS_PROG_VERSION, GSHHS_DATA_VERSION);
        fprintf (stderr, "usage:  gshhs gshhs_[f|h|i|l|c].b [-I<id>] [-L] [-M] > ascii.dat\n");
        fprintf (stderr, "-L will only list headers (no data output)\n");
        fprintf (stderr, "-I only output data for polygon number <id> [Default is all polygons]\n");
        fprintf (stderr, "-M will write '>' at start of each segment header [P or L]\n");
        return false;
    }

    if ((fp = fopen (file, "rb")) == NULL ) {
        fprintf (stderr, "gshhs:  Could not find file %s.\n", file);
        return false;
    }

    if ((fp2 = fopen (file2, "w")) == NULL ) {
        fprintf (stderr, "gshhs:  Could not create file %s.\n", file2);
        return false;
    }

    size_t n_read = fread ((void *)&h, (size_t)sizeof (struct GSHHS), (size_t)1, fp);
    version = (h.flag >> 8) & 255;
	flip = ( version != GSHHS_DATA_RELEASE );    /* Take as sign that byte-swabbing is needed */

    while (n_read == 1) {
        if (flip) {
            h.id = swabi4 ((unsigned int)h.id);
            h.n  = swabi4 ((unsigned int)h.n);
            h.west  = swabi4 ((unsigned int)h.west);
            h.east  = swabi4 ((unsigned int)h.east);
            h.south = swabi4 ((unsigned int)h.south);
            h.north = swabi4 ((unsigned int)h.north);
            h.area  = swabi4 ((unsigned int)h.area);
            h.area_full  = swabi4 ((unsigned int)h.area_full);
            h.flag  = swabi4 ((unsigned int)h.flag);
            h.container  = swabi4 ((unsigned int)h.container);
            h.ancestor  = swabi4 ((unsigned int)h.ancestor);
        }
        level = h.flag & 255;                /* Level is 1-4 */
        version = (h.flag >> 8) & 255;            /* Version is 1-7 */
        greenwich = (h.flag >> 16) & 1;            /* Greenwich is 0 or 1 */
        src = (h.flag >> 24) & 1;            /* Greenwich is 0 (WDBII) or 1 (WVS) */
        river = (h.flag >> 25) & 1;            /* River is 0 (not river) or 1 (is river) */
        w = h.west  * GSHHS_SCL;            /* Convert from microdegrees to degrees */
        e = h.east  * GSHHS_SCL;
        s = h.south * GSHHS_SCL;
        n = h.north * GSHHS_SCL;
        source = (src == 1) ? 'W' : 'C';        /* Either WVS or CIA (WDBII) pedigree */
        if (river) source = tolower ((int)source);    /* Lower case c means river-lake */
        line = (h.area) ? 0 : 1;            /* Either Polygon (0) or Line (1) (if no area) */
        area = 0.1 * h.area;                /* Now im km^2 */
        f_area = 0.1 * h.area_full;                /* Now im km^2 */

        OK = (!single || h.id == ID);

        if (!msformat) c = kind[line];
        if (OK) {
            if (line)
                fprintf(fp2, "%c %6d%8d%2d%2c%10.5f%10.5f%10.5f%10.5f\n", c, h.id, h.n, level, source, w, e, s, n);
            else {
                (h.container == -1) ? sprintf (container, "-") : sprintf (container, "%6d", h.container);
                (h.ancestor == -1) ? sprintf (ancestor, "-") : sprintf (ancestor, "%6d", h.ancestor);
                fprintf(fp2, "%c %6d%8d%2d%2c%13.3f%13.3f%10.5f%10.5f%10.5f%10.5f %s %s\n", c, h.id, h.n, level, source, area, f_area, w, e, s, n, container, ancestor);
            }
        }

        if (info || !OK) {    /* Skip data, only want headers */
            fseek (fp, (long)(h.n * sizeof(struct TPOINT)), SEEK_CUR);
        }
        else {
            for (k = 0; k < h.n; k++) {

                if (fread ((void *)&p, (size_t)sizeof(struct TPOINT), (size_t)1, fp) != 1) {
                    fprintf (stderr, "gshhs:  Error reading file %s for %s %d, point %d.\n", argv[1], name[line], h.id, k);
					break;	// return false;
                }
                if (flip) {
                    p.x = swabi4 ((unsigned int)p.x);
                    p.y = swabi4 ((unsigned int)p.y);
                }
                lon = p.x * GSHHS_SCL;
                if ((greenwich && p.x > max_east) || (h.west > 180000000)) lon -= 360.0;
                lat = p.y * GSHHS_SCL;
                fprintf(fp2, "%11.6f%11.6f\n", lon, lat);
            }
        }
        max_east = 180000000;    /* Only Eurasia needs 270 */
        n_read = fread((void *)&h, (size_t)sizeof (struct GSHHS), (size_t)1, fp);
    }

    fclose (fp);
    fclose (fp2);

    return true;
}


///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_QbrtMapEditor.cpp"
