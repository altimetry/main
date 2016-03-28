#include "new-gui/brat/stdafx.h"

#include <QtGui>
#include <QColor>


#if defined (WIN32) || defined(_WIN32)
#pragma warning ( disable: 4100 )           //unreferenced formal parameter
#endif

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

#include <qgslayertreenode.h>
#include <qgslayertreeviewdefaultactions.h>

#if defined (WIN32) || defined(_WIN32)
#pragma warning ( default: 4100 )           //unreferenced formal parameter
#endif


#include "new-gui/Common/QtUtilsIO.h"
#include "new-gui/Common/tools/Trace.h"

#include "new-gui/Common/+UtilsIO.h"
#include "new-gui/Common/QtUtils.h"


#include "MapWidget.h"


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


//qgsvectorfilewriter.h inclusion requires the following undefs
//
#undef HAVE_VPRINTF
#undef HAVE_DIRECT_H
#undef HAVE_SNPRINTF
//#define Py_CONFIG_H
//#if !defined MIN
//#define MIN std::min
//#endif
#include <qgsvectorfilewriter.h>
#include <qgsrasterlayer.h>
#include <qgsdatasourceuri.h>

void CMapWidget::setupDatabase()
{
    std::string cur_dir = GetDirectoryFromPath( qApp->argv()[ 0 ] );
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

QgsSymbolV2* CMapWidget::createSymbol( double width, 
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

void CMapWidget::setupMapLayers()
{
    std::string cur_dir = GetDirectoryFromPath( qApp->argv()[ 0 ] );

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

void CMapWidget::setProjection()
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

void CMapWidget::drawLines()
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

void CMapWidget::addGrid()
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
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

#include <qgsmaptoolpan.h>
#include <qgsmaptoolzoom.h>

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif
//

//#include <qgsfieldmap.h> 
//
//Labelling related
//
#include <qgslabel.h>
#include <qgslabelattributes.h>
#include <qgsfield.h>

void CMapWidget::addLabelsLayer()
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
