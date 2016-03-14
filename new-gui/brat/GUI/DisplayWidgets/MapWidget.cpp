#include "new-gui/brat/stdafx.h"

#include <qgsproviderregistry.h>
#include <qgsmaplayerregistry.h>
#include <qgsvectorlayer.h>

#include <qgsrubberband.h>

#include <qgslinesymbollayerv2.h>
#include <qgsmarkersymbollayerv2.h>

// for addMemoryLayer
#include <qgsgraduatedsymbolrendererv2.h>
#include <qgssinglebandpseudocolorrenderer.h>
#include <qgsrastershader.h>

// for selection tools
#include <qgscursors.h>
#include "QGISapp/MapToolSelectUtils.h"
#include "QGISapp/MapToolSelectRectangle.h"
#include "QGISapp/MapToolSelectPolygon.h"
#include "QGISapp/MapToolMeasure.h"

// for grid
#include "QGISapp/MapDecorationGrid.h"


// from brat
#include "new-gui/Common/+UtilsIO.h"
#include "new-gui/Common/QtUtils.h"
#include "new-gui/Common/tools/Trace.h"

#include "ApplicationLogger.h"
#include "BratSettings.h"
#include "DataModels/PlotData/MapProjection.h"
#include "GUI/ActionsTable.h"

#include "MapWidget.h"




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		Map statics
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////
// Directories
//////////////


//static 
std::string CMapWidget::smQgisPluginsDir;
//static 
std::string CMapWidget::smVectorLayerPath;
//static 
std::string CMapWidget::smRasterLayerPath;


void CMapWidget::SetQGISDirectories( 
	const std::string &QgisPluginsDir, 
	const std::string &VectorLayerPath, 
	const std::string &RasterLayerPath )
{
	smQgisPluginsDir = QgisPluginsDir;
	smVectorLayerPath = VectorLayerPath;
	smRasterLayerPath = RasterLayerPath;
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




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Construction / Destruction
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  //mProjectSrsId = mMapCanvas->mapSettings().destinationCrs().srsid();

  //QgsProjectionSelector *projectionSelector;
  //QgsDebugMsg( "Read project CRSID: " + QString::number( mProjectSrsId ) );
  //projectionSelector->setSelectedCrsId( mProjectSrsId );


  //  long myCRSID = projectionSelector->selectedCrsId();
  //if ( myCRSID )
  //{
  //  QgsCoordinateReferenceSystem srs( myCRSID, QgsCoordinateReferenceSystem::InternalCrsId );
  //  mMapCanvas->setDestinationCrs( srs );
  //  QgsDebugMsg( QString( "Selected CRS " ) + srs.description() );
  //  // write the currently selected projections _proj string_ to project settings
  //  QgsDebugMsg( QString( "SpatialRefSys/ProjectCRSProj4String: %1" ).arg( projectionSelector->selectedProj4String() ) );
  //  QgsProject::instance()->writeEntry( "SpatialRefSys", "/ProjectCRSProj4String", projectionSelector->selectedProj4String() );
  //  QgsProject::instance()->writeEntry( "SpatialRefSys", "/ProjectCRSID", ( int ) projectionSelector->selectedCrsId() );
  //  QgsProject::instance()->writeEntry( "SpatialRefSys", "/ProjectCrs", projectionSelector->selectedAuthId() );

  //  // Set the map units to the projected coordinates if we are projecting
  //  if ( isProjected() )
  //  {
  //    // If we couldn't get the map units, default to the value in the
  //    // projectproperties dialog box (set above)
  //    if ( srs.mapUnits() != QGis::UnknownUnit )
  //      mMapCanvas->setMapUnits( srs.mapUnits() );
  //  }

  //  if ( cbxProjectionEnabled->isChecked() )
  //  {
  //    // mark selected projection for push to front
  //    projectionSelector->pushProjectionToFront();
  //  }
  //}

//
//void QgsProjectProperties::on_cbxProjectionEnabled_toggled( bool onFlyEnabled )
//{
//  QString measureOnFlyState = tr( "Measure tool (CRS transformation: %1)" );
//  QString unitsOnFlyState = tr( "Canvas units (CRS transformation: %1)" );
//  if ( !onFlyEnabled )
//  {
//    // reset projection to default
//    const QMap<QString, QgsMapLayer*> &mapLayers = QgsMapLayerRegistry::instance()->mapLayers();
//
//    if ( mMapCanvas->currentLayer() )
//    {
//      mLayerSrsId = mMapCanvas->currentLayer()->crs().srsid();
//    }
//    else if ( mapLayers.size() > 0 )
//    {
//      mLayerSrsId = mapLayers.begin().value()->crs().srsid();
//    }
//    else
//    {
//      mLayerSrsId = mProjectSrsId;
//    }
//    mProjectSrsId = mLayerSrsId;
//    projectionSelector->setSelectedCrsId( mLayerSrsId );
//
//    QgsCoordinateReferenceSystem srs( mLayerSrsId, QgsCoordinateReferenceSystem::InternalCrsId );
//    //set radio button to crs map unit type
//    QGis::UnitType units = srs.mapUnits();
//
//    radMeters->setChecked( units == QGis::Meters );
//    radFeet->setChecked( units == QGis::Feet );
//    radNMiles->setChecked( units == QGis::NauticalMiles );
//    radDegrees->setChecked( units == QGis::Degrees );
//
//    // unset ellipsoid
//    mEllipsoidIndex = 0;
//
//    btnGrpMeasureEllipsoid->setTitle( measureOnFlyState.arg( tr( "OFF" ) ) );
//    btnGrpMapUnits->setTitle( unitsOnFlyState.arg( tr( "OFF" ) ) );
//  }
//  else
//  {
//    if ( !mLayerSrsId )
//    {
//      mLayerSrsId = projectionSelector->selectedCrsId();
//    }
//    projectionSelector->setSelectedCrsId( mProjectSrsId );
//
//    btnGrpMeasureEllipsoid->setTitle( measureOnFlyState.arg( tr( "ON" ) ) );
//    btnGrpMapUnits->setTitle( unitsOnFlyState.arg( tr( "ON" ) ) );
//  }
//
//  setMapUnitsToCurrentProjection();
//
//  // Enable/Disable selector and update tool-tip
//  updateEllipsoidUI( mEllipsoidIndex ); // maybe already done by setMapUnitsToCurrentProjection
//
//}

//
//void QgsProjectProperties::setMapUnitsToCurrentProjection()
//{
//  long myCRSID = projectionSelector->selectedCrsId();
//  if ( !isProjected() || !myCRSID )
//    return;
//
//  QgsCoordinateReferenceSystem srs( myCRSID, QgsCoordinateReferenceSystem::InternalCrsId );
//  //set radio button to crs map unit type
//  QGis::UnitType units = srs.mapUnits();
//
//  radMeters->setChecked( units == QGis::Meters );
//  radFeet->setChecked( units == QGis::Feet );
//  radNMiles->setChecked( units == QGis::NauticalMiles );
//  radDegrees->setChecked( units == QGis::Degrees );
//
//  // attempt to reset the projection ellipsoid according to the srs
//  int myIndex = 0;
//  for ( int i = 0; i < mEllipsoidList.length(); i++ )
//  {
//    if ( mEllipsoidList[ i ].acronym == srs.ellipsoidAcronym() )
//    {
//      myIndex = i;
//      break;
//    }
//  }
//  updateEllipsoidUI( myIndex );
//}



CMapWidget::CMapWidget( QWidget *parent )	//parent = nullptr
	: base_t(parent)
{
    // Instantiate Provider Registry
	//
    QgsProviderRegistry *preg = QgsProviderRegistry::instance( t2q( smQgisPluginsDir ) );
	Q_UNUSED( preg );

	assert__( IsFile( smVectorLayerPath ) );
	//QString source, dest;
	//if ( !readFileFromResource( ":/maps/ne_10m_coastline/ne_10m_coastline.shp", dest, true ) )
	//	std::cout << "problem" << std::endl;
	//else
	//	qDebug() << dest;
		
	mMainLayer = AddOGRVectorLayer( t2q( smVectorLayerPath ) );
	mMainLayer->rendererV2()->symbols()[ 0 ]->setColor( "black" );

	//addRasterLayer( "http://server.arcgisonline.com/arcgis/rest/services/ESRI_Imagery_World_2D/MapServer?f=json&pretty=true", "raster", "" );

    //QString myLayerBaseName     = "italy";
    //QString myProviderName      = "ogr";

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
    setCanvasColor( QColor( 0xFF, 0xFF, 0xF0, 255 ) );		//setCanvasColor(QColor(255, 255, 255));
    freeze(false);

	// projections
	//
	mDefaultProjection = mMainLayer->crs();
	setCrsTransformEnabled( true );

    //// Set the Map Canvas Layer Set
    //setLayerSet(mLayerSet);

    setMinimumSize( min_globe_widget_width, min_globe_widget_height );    //setVisible(true);
    refresh();

	
	//context menu

	setContextMenuPolicy( Qt::ActionsContextMenu );
	mActionAddRaster = new QAction( "Add Raster", this );
	connect( mActionAddRaster, SIGNAL( triggered() ), this, SLOT( HandleAddRaster() ) );
	addAction( mActionAddRaster );
	
	mActionRemoveLayers = new QAction( "Remove Layers", this );
	connect( mActionRemoveLayers, SIGNAL( triggered() ), this, SLOT( HandleRemoveLayers() ) );
	addAction( mActionRemoveLayers );
	
	mActionNextProjection = new QAction( "Next Projection", this );
	connect( mActionNextProjection, SIGNAL( triggered() ), this, SLOT( HandleNextProjection() ) );
	addAction( mActionNextProjection );
	
	mActionDefaultProjection = new QAction( "Default Projection", this );
	connect( mActionDefaultProjection, SIGNAL( triggered() ), this, SLOT( HandleDefaultProjection() ) );
	addAction( mActionDefaultProjection );
}


//////////////
// Projections
//////////////

void CMapWidget::SetDefaultProjection()
{
	SetProjection( mDefaultProjection );
}


void CMapWidget::SetProjection( const QgsCoordinateReferenceSystem &crs )
{
	setDestinationCrs( crs );
	if ( crs.mapUnits() == QGis::UnknownUnit )
		LOG_WARN( "Projection with unknown map units." );
	else
	{
		mDecorationGrid->setMapUnits( crs.mapUnits() );
		setMapUnits( crs.mapUnits() );
		if ( !mDecorationGrid->enabled() )				//the grid can disable itself 
			if ( mActionDecorationGrid )
				mActionDecorationGrid->setChecked( false );
	}
}

void CMapWidget::SetProjection( unsigned id )
{
	assert__( id != PROJ2D_3D );	//3D projections not supported in 2D maps...

	SetProjection( CMapProjection::GetInstance()->IdToCRS( id ) );
}

void CMapWidget::HandleDefaultProjection()
{
	SetDefaultProjection();
}


void CMapWidget::HandleNextProjection()
{
	//if ( crs->isValid() )
	//{
	//	if ( !this->hasCrsTransformEnabled() )
	//		SimpleErrorBox("ahhhhhhhhhhhhhhhhhhhhh");
	//	SimpleMsgBox( crs->description() );
	//	setDestinationCrs( *crs );
 //       setMapUnits( crs->mapUnits() );
	//}
	//else
	//	SimpleErrorBox("chissa");
//void QgsProjectionSelector::loadCrsList( QSet<QString> *crsFilter )
//{
//  if ( mProjListDone )
//    return;
//
//  // convert our Coordinate Reference System filter into the SQL expression
//  QString sqlFilter = ogcWmsCrsFilterAsSqlExpression( crsFilter );
//
//  // Create the top-level nodes for the list view of projections
//  // Make in an italic font to distinguish them from real projections
//  //
//  // Geographic coordinate system node
//  mGeoList = new QTreeWidgetItem( lstCoordinateSystems, QStringList( tr( "Geographic Coordinate Systems" ) ) );
//
//  QFont fontTemp = mGeoList->font( 0 );
//  fontTemp.setItalic( true );
//  fontTemp.setBold( true );
//  mGeoList->setFont( 0, fontTemp );
//  mGeoList->setIcon( 0, QIcon( QgsApplication::activeThemePath() + "geographic.png" ) );
//
//  // Projected coordinate system node
//  mProjList = new QTreeWidgetItem( lstCoordinateSystems, QStringList( tr( "Projected Coordinate Systems" ) ) );
//
//  fontTemp = mProjList->font( 0 );
//  fontTemp.setItalic( true );
//  fontTemp.setBold( true );
//  mProjList->setFont( 0, fontTemp );
//  mProjList->setIcon( 0, QIcon( QgsApplication::activeThemePath() + "transformed.png" ) );
//
//  //bail out in case the projections db does not exist
//  //this is necessary in case the pc is running linux with a
//  //read only filesystem because otherwise sqlite will try
//  //to create the db file on the fly
//
//  if ( !QFileInfo( mSrsDatabaseFileName ).exists() )
//  {
//    mProjListDone = true;
//    return;
//  }
//
//  // open the database containing the spatial reference data
//  sqlite3 *database;
//  int rc = sqlite3_open_v2( mSrsDatabaseFileName.toUtf8().data(), &database, SQLITE_OPEN_READONLY, NULL );
//  if ( rc )
//  {
//    // XXX This will likely never happen since on open, sqlite creates the
//    //     database if it does not exist.
//    showDBMissingWarning( mSrsDatabaseFileName );
//    return;
//  }
//
//  const char *tail;
//  sqlite3_stmt *stmt;
//  // Set up the query to retrieve the projection information needed to populate the list
//  //note I am giving the full field names for clarity here and in case someone
//  //changes the underlying view TS
//  QString sql = QString( "select description, srs_id, upper(auth_name||':'||auth_id), is_geo, name, parameters, deprecated from vw_srs where %1 order by name,description" )
//                .arg( sqlFilter );
//
//  rc = sqlite3_prepare( database, sql.toUtf8(), sql.toUtf8().length(), &stmt, &tail );
//  // XXX Need to free memory from the error msg if one is set
//  if ( rc == SQLITE_OK )
//  {
//    QTreeWidgetItem *newItem;
//    // Cache some stuff to speed up creating of the list of projected
//    // spatial reference systems
//    QString previousSrsType( "" );
//    QTreeWidgetItem* previousSrsTypeNode = 0;
//
//    while ( sqlite3_step( stmt ) == SQLITE_ROW )
//    {
//      // check to see if the srs is geographic
//      int isGeo = sqlite3_column_int( stmt, 3 );
//      if ( isGeo )
//      {
//        // this is a geographic coordinate system
//        // Add it to the tree (field 0)
//        newItem = new QTreeWidgetItem( mGeoList, QStringList( QString::fromUtf8(( char * )sqlite3_column_text( stmt, 0 ) ) ) );
//
//        // display the authority name (field 2) in the second column of the list view
//        newItem->setText( AUTHID_COLUMN, QString::fromUtf8(( char * )sqlite3_column_text( stmt, 2 ) ) );
//
//        // display the qgis srs_id (field 1) in the third column of the list view
//        newItem->setText( QGIS_CRS_ID_COLUMN, QString::fromUtf8(( char * )sqlite3_column_text( stmt, 1 ) ) );
//      }
//      else
//      {
//        // This is a projected srs
//        QTreeWidgetItem *node;
//        QString srsType = QString::fromUtf8(( char* )sqlite3_column_text( stmt, 4 ) );
//        // Find the node for this type and add the projection to it
//        // If the node doesn't exist, create it
//        if ( srsType == previousSrsType )
//        {
//          node = previousSrsTypeNode;
//        }
//        else
//        { // Different from last one, need to search
//          QList<QTreeWidgetItem*> nodes = lstCoordinateSystems->findItems( srsType, Qt::MatchExactly | Qt::MatchRecursive, NAME_COLUMN );
//          if ( nodes.count() == 0 )
//          {
//            // the node doesn't exist -- create it
//            // Make in an italic font to distinguish them from real projections
//            node = new QTreeWidgetItem( mProjList, QStringList( srsType ) );
//            QFont fontTemp = node->font( 0 );
//            fontTemp.setItalic( true );
//            node->setFont( 0, fontTemp );
//          }
//          else
//          {
//            node = nodes.first();
//          }
//          // Update the cache.
//          previousSrsType = srsType;
//          previousSrsTypeNode = node;
//        }
//        // add the item, setting the projection name in the first column of the list view
//        newItem = new QTreeWidgetItem( node, QStringList( QString::fromUtf8(( char * )sqlite3_column_text( stmt, 0 ) ) ) );
//        // display the authority id (field 2) in the second column of the list view
//        newItem->setText( AUTHID_COLUMN, QString::fromUtf8(( char * )sqlite3_column_text( stmt, 2 ) ) );
//        // display the qgis srs_id (field 1) in the third column of the list view
//        newItem->setText( QGIS_CRS_ID_COLUMN, QString::fromUtf8(( char * )sqlite3_column_text( stmt, 1 ) ) );
//        // expand also parent node
//        newItem->parent()->setExpanded( true );
//      }
//
//      // display the qgis deprecated in the user data of the item
//      newItem->setData( 0, Qt::UserRole, QString::fromUtf8(( char * )sqlite3_column_text( stmt, 6 ) ) );
//      newItem->setHidden( cbxHideDeprecated->isChecked() );
//    }
//    mProjList->setExpanded( true );
//  }
//
//  // close the sqlite3 statement
//  sqlite3_finalize( stmt );
//  // close the database
//  sqlite3_close( database );
//
//  mProjListDone = true;
//}
}

void CMapWidget::HandleAddRaster()
{
	if ( IsFile( smRasterLayerPath ) )
		mMainRasterLayer = AddRasterLayer( t2q( smRasterLayerPath ), "raster", "" );
}

void CMapWidget::HandleRemoveLayers()
{
	RemoveLayers();
}




//virtual
CMapWidget::~CMapWidget()
{
	assert( !isDrawing() );

	delete mSelectFeatures;
	delete mSelectPolygon;
	delete mMeasureDistance;
	delete mMeasureArea;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		Lines & Points
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


QgsRubberBand* CMapWidget::AddRBPoint( double lon, double lat, QColor color, QgsVectorLayer *layer )	//layer = nullptr  
{
	QgsRubberBand *r = new QgsRubberBand( this, QGis::Point );
	r->setToGeometry( QgsGeometry::fromPoint( QgsPoint( lon, lat ) ), layer );
	r->setColor( color );
	r->setIconSize( 1 );
	r->setWidth( 1 );

	return r;
}
QgsRubberBand* CMapWidget::AddRBLine( QgsPolyline points, QColor color, QgsVectorLayer *layer )	//layer = nullptr  
{
	QgsRubberBand *r = new QgsRubberBand( this, QGis::Line );			//deprecated: False = not a polygon
	r->setToGeometry( QgsGeometry::fromPolyline( points ), layer );
	r->setColor( color );
	r->setIconSize( 1 );
	r->setWidth( 1 );

	return r;
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



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		Layers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void CMapWidget::SetCurrentLayer( QgsMapLayer *l )
{
	LOG_INFO( "Layer CRS = " + n2s<std::string>( l->crs().srsid() ) );
	setCurrentLayer( l );
	QgsVectorLayer *vl = qobject_cast< QgsVectorLayer* >( l );
	if ( vl )
		connect( vl, SIGNAL( selectionChanged() ), this, SIGNAL( CurrentLayerSelectionChanged() ), Qt::QueuedConnection );
}


// by femm...
#if !defined(TRUE)
#define TRUE 1
#endif

QgsRasterLayer* CMapWidget::AddRasterLayer( const QString &layer_path, const QString &base_name, const QString &provider, QgsSymbolV2* symbol )	//symbol = nullptr
{
    UNUSED( symbol );

    static const std::string index_symbol = "#";
	static size_t index = 0;

	std::string name = q2a( base_name ) + index_symbol + n2s<std::string>(index++);
	auto l = provider.isEmpty() ? new QgsRasterLayer( layer_path, name.c_str() ) : new QgsRasterLayer( layer_path, name.c_str(), provider );
	//addRenderer( l, symbol );

	if ( l->isValid() ) 
	{
							qDebug( "Layer is valid" );

		// Add the Vector Layer to the Layer Registry
		QgsMapLayerRegistry::instance()->addMapLayer(l, TRUE);

		// Add the Layer to the Layer Set
		mLayerSet.append(QgsMapCanvasLayer(l, TRUE));

		// Set the Map Canvas Layer Set			//TODO: check if we need to setLayerSet every time
		setLayerSet(mLayerSet);

		//setCurrentLayer( l );

		return l;
	}
							qDebug( "Layer is NOT valid" );
	delete l;
	return nullptr;
}


QgsVectorLayer* CMapWidget::AddMemoryLayer( QgsSymbolV2* symbol )		//symbol = nullptr 
{
	return AddVectorLayer( "Point?crs=EPSG:4326&field=height:double&field=name:string(255)&index=yes", "mem", "memory", symbol );

	auto ml = AddVectorLayer( "Point?crs=EPSG:4326&field=height:double&field=name:string(255)&index=yes", "mem", "memory" );	// , symbol );

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


QgsVectorLayer* CMapWidget::AddVectorLayer( const QString &layer_path, const QString &base_name, const QString &provider, QgsSymbolV2* symbol )	//symbol = nullptr 
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

		SetCurrentLayer( l );

		return l;
	}
							qDebug( "Layer is NOT valid" );
	delete l;
	return nullptr;
}


void CMapWidget::RemoveLayers()
{
	const int count = mLayerSet.size();

	while( mLayerSet.size() > 1 )
		mLayerSet.removeLast();

	if ( count > 1 )
	{
		setLayerSet( mLayerSet );
		SetCurrentLayer( mLayerSet.begin()->layer() );
		refresh();
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		Zoom
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		Selection Tool
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//slots

void CMapWidget::EnableRectangularSelection( bool enable )
{
	enable ? setMapTool( mSelectFeatures ) : unsetMapTool( mSelectFeatures );
}																		 	
void CMapWidget::EnablePolygonalSelection( bool enable )
{
	enable ? setMapTool( mSelectPolygon ) : unsetMapTool( mSelectPolygon );
}
void CMapWidget::DeselectAll()
{
	// Turn off rendering to improve speed.
	bool renderFlagState = renderFlag();
	if ( renderFlagState )
		setRenderFlag( false );

	QMap<QString, QgsMapLayer*> layers = QgsMapLayerRegistry::instance()->mapLayers();
	for ( QMap<QString, QgsMapLayer*>::iterator it = layers.begin(); it != layers.end(); ++it )
	{
		QgsVectorLayer *vl = qobject_cast<QgsVectorLayer *>( it.value() );
		if ( !vl )
			continue;

		vl->removeSelection();
	}

	// Turn on rendering (if it was on previously)
	if ( renderFlagState )
		setRenderFlag( true );
}


//get selection info

QgsRectangle CMapWidget::CurrentLayerSelectedBox() const
{
	QgsRectangle box;
	QgsVectorLayer *l = QgsMapToolSelectUtils::getCurrentVectorLayer( const_cast<CMapWidget*>( this ) );
	if ( l )
		box = l->boundingBoxOfSelected();

	return box;
}


//create and insert widgets/actions

void CMapWidget::ConnectParentSelectionActions( QToolButton *selection_button, QAction *action_select_features, QAction *action_select_polygon, QAction *action_deselect_all )
{
	connect( action_select_features, SIGNAL( toggled( bool ) ), this, SLOT( EnableRectangularSelection( bool ) ) );
	mSelectFeatures = new CMapToolSelectFeatures( this );
	mSelectFeatures->setAction( action_select_features );

	connect( action_select_polygon, SIGNAL( toggled( bool ) ), this, SLOT( EnablePolygonalSelection( bool ) ) );
	mSelectPolygon = new CMapToolSelectPolygon( this );
	mSelectPolygon->setAction( action_select_polygon );

	connect( action_deselect_all, SIGNAL( triggered() ), this, SLOT( DeselectAll() ) );

	connect( selection_button, SIGNAL( triggered( QAction * ) ), this, SLOT( ToolButtonTriggered( QAction * ) ) );
}


//static 
QToolButton* CMapWidget::CreateMapSelectionActions( QToolBar *tb, QAction *&action_select_features, QAction *&action_select_polygon, QAction *&action_deselect_all )
{
    action_select_features = CActionInfo::CreateAction( tb, eAction_SelectFeatures );
	action_select_polygon = CActionInfo::CreateAction( tb, eAction_SelectPolygon );
	action_deselect_all = CActionInfo::CreateAction( tb, eAction_DeselectAll );

	QToolButton *selection = new QToolButton( tb );
	selection->setPopupMode( QToolButton::MenuButtonPopup );
	selection->addAction( action_select_features );
	selection->addAction( action_select_polygon );
	selection->setDefaultAction( action_select_features );

	return selection;
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		Measure Tool
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//slots

void CMapWidget::MeasureDistance( bool enable )
{
	enable ? setMapTool( mMeasureDistance ) : unsetMapTool( mMeasureDistance );
}

void CMapWidget::MeasureArea( bool enable )
{
	enable ? setMapTool( mMeasureArea ) : unsetMapTool( mMeasureArea );
}


//create and insert widgets/actions

void 
CMapWidget::ConnectParentMeasureActions( QToolButton *measure_button, QAction *action_measure_distance, QAction *action_measure_area )
{
	mMeasureDistance = new CMeasureTool( this, false );	//false -> distance
	mMeasureDistance->setAction( action_measure_distance );

	mMeasureArea = new CMeasureTool( this, true );		//true -> area
	mMeasureArea->setAction( action_measure_area );

	connect( action_measure_distance, SIGNAL( toggled( bool ) ), this, SLOT( MeasureDistance( bool ) ) );
	connect( action_measure_area, SIGNAL( toggled( bool ) ), this, SLOT( MeasureArea( bool ) ) );
	connect( measure_button, SIGNAL( triggered( QAction * ) ), this, SLOT( ToolButtonTriggered( QAction * ) ) );
}

//static 
QToolButton* CMapWidget::CreateMapMeasureActions( QToolBar *tb, QAction *&action_measure_distance, QAction *&action_measure_area )
{
    action_measure_distance = CActionInfo::CreateAction( tb, eAction_MeasureLine );
	action_measure_area = CActionInfo::CreateAction( tb, eAction_MeasureArea );

	QToolButton* measure = new QToolButton( tb );
	measure->setPopupMode( QToolButton::MenuButtonPopup );
	measure->addAction( action_measure_distance );
	measure->addAction( action_measure_area );
	measure->setDefaultAction( action_measure_distance );

	return measure;
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		All Tools
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void CMapWidget::ToolButtonTriggered( QAction *action )
{
	QToolButton *bt = qobject_cast<QToolButton *>( sender() );
	if ( !bt )
		return;

	bt->setDefaultAction( action );
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		Grid
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//slots

void CMapWidget::ToggleGridEnabled( bool toggle ) 
{ 
	mDecorationGrid->setEnabled( toggle ); 
	refresh();
	emit GridEnabled( toggle );
}



//create and connect widgets/actions

void CMapWidget::ConnectParentGridAction( QAction *action_grid )
{
	mActionDecorationGrid = action_grid;

	mDecorationGrid = new CDecorationGrid( this );
	mDecorationGrid->setEnabled( action_grid->isChecked() ); 

	connect( this, SIGNAL( renderComplete( QPainter * ) ), mDecorationGrid, SLOT( render( QPainter * ) ) );
	connect( action_grid, SIGNAL( toggled( bool ) ), this, SLOT( ToggleGridEnabled( bool ) ) );
}

//static 
QAction* CMapWidget::CreateGridAction( QToolBar *tb )
{
	QAction *agrid = CActionInfo::CreateAction( tb, eAction_DecorationGrid );
	agrid->setChecked( false );
	return agrid;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		Rendering Interaction with Parent Widgets
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// slots

void CMapWidget::ShowProgress( int theProgress, int theTotalSteps )
{
	assert__( mProgressBar );

	if ( theProgress == theTotalSteps )
	{
		mProgressBar->reset();
		mProgressBar->hide();
	}
	else
	{
		//only call show if not already hidden to reduce flicker
		if ( !mProgressBar->isVisible() )
		{
			mProgressBar->show();
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


void CMapWidget::RefreshStarted()
{
	ShowProgress( -1, 0 ); // trick to make progress bar show busy indicator
}

void CMapWidget::RefreshFinished()
{
	ShowProgress( 0, 0 ); // stop the busy indicator
}



//create and connect widgets/actions

void CMapWidget::ConnectParentRenderWidgets( QProgressBar *bar, QCheckBox *box )
{
	mProgressBar = bar;
	mRenderSuppressionCBox = box;

	connect( this, SIGNAL( renderStarting() ), this, SLOT( RefreshStarted() ) );
	connect( this, SIGNAL( mapCanvasRefreshed() ), this, SLOT( RefreshFinished() ) );
	connect( mRenderSuppressionCBox, SIGNAL( toggled( bool ) ), this, SLOT( setRenderFlag( bool ) ) );
}

//static 
void CMapWidget::CreateRenderWidgets( QStatusBar *bar, QProgressBar *&progress, QCheckBox *&box )
{
	progress = new QProgressBar( bar );
	progress->setObjectName( "mProgressBar" );
	progress->setMaximumWidth( 100 );
	progress->setWhatsThis( tr( "Progress bar that displays the status of time-intensive operations" ) );

	// Bumped the font up one point size since 8 was too
	// small on some platforms. A point size of 9 still provides
	// plenty of display space on 1024x768 resolutions
	QFont myFont( "Arial", 9 );
	// render suppression status bar widget
	//
	box = new QCheckBox( tr( "Render" ), bar );
	box->setObjectName( "mRenderSuppressionCBox" );
	box->setChecked( true );
	box->setFont( myFont );
	box->setWhatsThis( tr( "When checked, the map layers "
		"are rendered in response to map navigation commands and other "
		"events. When not checked, no rendering is done. This allows you "
		"to add a large number of layers and symbolize them before rendering." ) );
	box->setToolTip( tr( "Toggle map rendering" ) );
}




///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_MapWidget.cpp"
