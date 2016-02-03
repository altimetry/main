#include "new-gui/brat/stdafx.h"

#include <qgsproviderregistry.h>
//#include <qgsvectordataprovider.h>
#include <qgsmaplayerregistry.h>
#include <qgsvectorlayer.h>

#include <qgsrubberband.h>

#include <qgslinesymbollayerv2.h>
#include <qgsmarkersymbollayerv2.h>

// for addMemoryLayer
#include <qgsgraduatedsymbolrendererv2.h>
#include <qgssinglebandpseudocolorrenderer.h>
#include <qgsrastershader.h>

#include "new-gui/Common/+UtilsIO.h"
#include "new-gui/Common/QtUtils.h"
#include "new-gui/Common/tools/Trace.h"

#include "new-gui/brat/BratSettings.h"

//#include "new-gui/brat-lab/System/BackServices.h"
//#include "new-gui/brat-lab/System/CmdLineProcessor.h"
//#include "display/PlotData/GeoMap.h"
//#include "display/PlotData/WPlot.h"

#include "MapWidget.h"


//static 
std::string CMapWidget::smQgisPluginsDir;
//static 
std::string CMapWidget::smVectorLayerPath;
//static 
std::string CMapWidget::smRasterLayerPath;
//static 
std::string CMapWidget::smGlobeDir;


void CMapWidget::SetQGISDirectories( 
	const std::string &QgisPluginsDir, 
	const std::string &VectorLayerPath, 
	const std::string &RasterLayerPath,
	const std::string &GlobeDir )
{
	smQgisPluginsDir = QgisPluginsDir;
	smVectorLayerPath = VectorLayerPath;
	smRasterLayerPath = RasterLayerPath;
	smGlobeDir = GlobeDir;
}


//static 
const std::string& CMapWidget::QgisPluginsDir()
{
	assert__( IsDir( smQgisPluginsDir ) );

	return smQgisPluginsDir;
}
//static 
const std::string& CMapWidget::VectorLayerPath()
{
	assert__( IsDir( smVectorLayerPath ) );

	return smVectorLayerPath;
}
//static 
const std::string& CMapWidget::RasterLayerPath()
{
	//no assert, not mandatory

	return smRasterLayerPath;
}
//static 
const std::string& CMapWidget::GlobeDir()
{
	assert__( IsDir( smGlobeDir ) );

	return smGlobeDir;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


CMapWidget::CMapWidget( QWidget *parent )	//parent = nullptr
	: base_t(parent)
{
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
    QgsProviderRegistry *preg = QgsProviderRegistry::instance( t2q( smQgisPluginsDir ) );
	Q_UNUSED( preg );

	assert__( IsFile( smVectorLayerPath ) );
	//QString source, dest;
	//if ( !readFileFromResource( ":/maps/ne_10m_coastline/ne_10m_coastline.shp", dest, true ) )
	//	std::cout << "problem" << std::endl;
	//else
	//	qDebug() << dest;
		
	mMainLayer = addOGRLayer( t2q( smVectorLayerPath ) );
	mMainLayer->rendererV2()->symbols()[ 0 ]->setColor( "black" );

	if ( false )
	{
		if ( IsFile( smRasterLayerPath ) )
			mMainRasterLayer = addRasterLayer( t2q( smRasterLayerPath ), "raster", "" );
	}

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

    setMinimumSize( min_globe_widget_width, min_globe_widget_height );    //setVisible(true);
    refresh();
}

// by femm...
#if !defined(TRUE)
#define TRUE 1
#endif

QgsRasterLayer* CMapWidget::addRasterLayer( const QString &layer_path, const QString &base_name, const QString &provider, QgsSymbolV2* symbol )	//symbol = nullptr
{
    UNUSED( symbol );

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

QgsVectorLayer* CMapWidget::addLayer( const QString &layer_path, const QString &base_name, const QString &provider, QgsSymbolV2* symbol )	//symbol = nullptr 
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

QgsRubberBand* CMapWidget::addRBPoint( double lon, double lat, QColor color, QgsVectorLayer *layer )	//layer = nullptr  
{
	QgsRubberBand *r = new QgsRubberBand( this, QGis::Point );
	r->setToGeometry( QgsGeometry::fromPoint( QgsPoint( lon, lat ) ), layer );
	r->setColor( color );
	r->setIconSize( 1 );
	r->setWidth( 1 );

	return r;
}
QgsRubberBand* CMapWidget::addRBLine( QgsPolyline points, QColor color, QgsVectorLayer *layer )	//layer = nullptr  
{
	QgsRubberBand *r = new QgsRubberBand( this, QGis::Line );			//deprecated: False = not a polygon
	r->setToGeometry( QgsGeometry::fromPolyline( points ), layer );
	r->setColor( color );
	r->setIconSize( 1 );
	r->setWidth( 1 );

	return r;
}

QgsVectorLayer* CMapWidget::addMemoryLayer( QgsSymbolV2* symbol )		//symbol = nullptr 
{
	return addLayer( "Point?crs=EPSG:4326&field=height:double&field=name:string(255)&index=yes", "mem", "memory", symbol );

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
}

//	Note that you can use strings like "red" instead of Qt::red !!!
//
//static 
QgsSymbolV2* CMapWidget::createPointSymbol( double width, const QColor &color )
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
QgsSymbolV2* CMapWidget::createLineSymbol( double width, const QColor &color )
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
QgsFeatureList& CMapWidget::createPointFeature( QgsFeatureList &list, double lon, double lat, double value )
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
QgsFeatureList& CMapWidget::createLineFeature( QgsFeatureList &list, QgsPolyline points )
{
	auto line = QgsGeometry::fromPolyline( points );
	QgsFeature *f = new QgsFeature();
	f->setGeometry( line );
	list.append( *f );

	return list;
}



//virtual
CMapWidget::~CMapWidget()
{
	assert( !isDrawing() );
}

//virtual 
void CMapWidget::keyPressEvent( QKeyEvent * e ) //override
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

//void CMapWidget::CreateWPlot( const CmdLineProcessor *proc, CWPlot* wplot )
//{
//	QSize size;
//	QPoint pos;
//
//	wplot->GetInfo();
//
//	CWorldPlotProperty* wPlotProperty = proc->GetWorldPlotProperty( 0 );
//	UNUSED( wPlotProperty );
//
//	//TODO CWorldPlotFrame* frame = new CWorldPlotFrame( NULL, -1, title, wPlotProperty, pos, size );
//
//	// for geostrophic velocity
//	CPlotField * northField =NULL;
//	CPlotField * eastField =NULL;
//	for ( CObArray::iterator itField = wplot->m_fields.begin(); itField != wplot->m_fields.end(); itField++ )
//	{
//		CPlotField* field = CPlotField::GetPlotField( *itField );
//
//		if ( field->m_internalFiles.empty() )
//			continue;
//
//		if ( field->m_worldProps->m_northComponent && northField == NULL ) {
//			northField = field;
//			continue;
//		}
//		else
//		if ( field->m_worldProps->m_eastComponent && eastField == NULL ) {
//			eastField = field;
//			continue;
//		}
//
//		// otherwise just add it as regular data
//		CGeoMap *geoMap = new CGeoMap( field );
//		AddData( geoMap );
//	}
//
//	// we have a Vector Plot!
//	if ( northField != NULL && eastField != NULL ) {
//
//		CGeoVelocityMap *gvelocityMap = new CGeoVelocityMap( northField, eastField );
//		gvelocityMap->SetIsGlyph( true );
//		AddData( gvelocityMap );
//	}
//	else if ( northField != eastField ) {
//		CException e( "CBratDisplayApp::CreateWPlot - incomplete std::vector plot components", BRATHL_INCONSISTENCY_ERROR );
//		CTrace::Tracer( "%s", e.what() );
//		throw ( e );
//	}
//}
////void CWorldPlotRenderer::AddData( CWorldPlotData* pdata )
////{
////	pdata->SetRenderer( m_vtkRend );
////
////	m_actors.Insert( pdata );
////	m_transformations->AddItem( pdata->GetTransform() );
////	pdata->GetVtkTransform()->SetTransform( pdata->GetTransform() );
////
////	ResetTextActor();
////}

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
//void CMapWidget::AddData( CWorldPlotData* pdata )
//{
//	CGeoMap* geoMap = dynamic_cast<CGeoMap*>( pdata );
//
//	auto IsValidPoint = [&geoMap]( int32_t i )
//	{
//		bool bOk = geoMap->bits[ i ];
//
//		//	  if (Projection == VTK_PROJ2D_MERCATOR)
//		//	  {
//		bOk &= geoMap->valids[ i ];
//		//	  }
//		//
//		return bOk;
//	};
//
//
//	auto const size = geoMap->vals.size();
//	QgsFeatureList flist;
//
//#if defined (USE_POINTS)	//(**)
//
//	for ( auto i = 0u; i < size; ++ i )
//	{
//		if ( !IsValidPoint( i ) )
//			continue;
//
//		auto x = i % geoMap->lons.size(); // ( x * geoMap->lats.size() ) + i;
//		auto y = i / geoMap->lons.size(); // ( x * geoMap->lats.size() ) + i;
//
//#if defined (USE_FEATURES) //(***)
//		createPointFeature( flist, geoMap->lons.at( x ), geoMap->lats.at( y ), geoMap->vals[ i ] );
//		//createPointFeature( flist, geoMap->lons.at( x ), geoMap->lats.at( y ), QColor( 0, (unsigned char)(geoMap->vals[ i ]), 0 ) );
//#else
//		addRBPoint( geoMap->lons.at( x ), geoMap->lats.at( y ), QColor( (long)(geoMap->vals[ i ]) ), mMainLayer );
//#endif
//	}
//
//#if defined (USE_FEATURES)
//	auto memL = addMemoryLayer( createPointSymbol( 0.5, Qt::red ) );	//(*)	//note that you can use strings like "red" instead!!!
//	memL->dataProvider()->addFeatures( flist );
//	//memL->updateExtents();
//	//refresh();
//#endif
//
//	return;
//
//#else		//(**)
//
//	QgsPolyline points;
//	for ( auto i = 0; i < size; ++ i ) 
//	{
//		if ( !IsValidPoint(i) )
//			continue;
//
//		auto x = i % geoMap->lons.size();
//		auto y = i / geoMap->lons.size();
//
//		points.append( QgsPoint( geoMap->lons.at( x ), geoMap->lats.at( y ) ) );
//	}
//#if !defined (USE_FEATURES) //(***)
//	auto memL = addMemoryLayer( createLineSymbol( 0.5, Qt::red ) );	//(*)	//note that you can use strings like "red" instead!!!
//	createLineFeature( flist, points );						
//	memL->dataProvider()->addFeatures( flist );				
//	//memL->updateExtents();
//	//refresh();
//#else
//	addRBLine( points, QColor( 0, 255, 0 ), mMainLayer );	
//#endif
//
//	return;
//
//#endif
//
//	//femm: This is CWorldPlotPanel::AddData
//
//	//femm: the important part
//	//if ( pdata->GetColorBarRenderer() != NULL )
//	//	m_vtkWidget->GetRenderWindow()->AddRenderer( pdata->GetColorBarRenderer()->GetVtkRenderer() );
//	//m_plotRenderer->AddData( pdata );
//
//
//	//femm: the less important part
//	//CGeoMap* geoMap = dynamic_cast<CGeoMap*>( pdata );
//	//if ( geoMap != NULL )
//	//{
//	//	wxString textLayer = wxString::Format( "%s", geoMap->GetDataName().c_str() );
//
//	//	m_plotPropertyTab->GetLayerChoice()->Append( textLayer, static_cast<void*>( geoMap ) );
//	//	m_plotPropertyTab->SetCurrentLayer( 0 );
//	//}
//
//	//int32_t nFrames = 1;
//	//if ( geoMap != NULL )
//	//	nFrames = geoMap->GetNrMaps();
//
//	//m_animationToolbar->SetMaxFrame( nFrames );
//}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_MapWidget.cpp"
