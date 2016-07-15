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

#include <qgsproviderregistry.h>
#include <qgsmaplayerregistry.h>
#include <qgsvectorlayer.h>
#include <qgsvectordataprovider.h>

#include <qgsrubberband.h>

#include <qgslinesymbollayerv2.h>
#include <qgsmarkersymbollayerv2.h>
#include <qgssinglesymbolrendererv2.h>
#include <qgsfillsymbollayerv2.h>

// for addMemoryLayer
#include <qgsgraduatedsymbolrendererv2.h>
#include <qgssinglebandpseudocolorrenderer.h>
#include <qgsrastershader.h>

// not used so far
#include <qgsvectorcolorrampv2.h>


// for selection tools
#include <qgscursors.h>
#include "QGISapp/MapToolSelectUtils.h"
#include "QGISapp/MapToolSelectRectangle.h"
#include "QGISapp/MapToolSelectPolygon.h"
#include "QGISapp/MapToolMeasure.h"

// for grid
#include "QGISapp/MapDecorationGrid.h"


// to that write layer to shape file
#include <qgscoordinatetransform.h>
//qgsvectorfilewriter.h inclusion requires the following undefs
//
#undef HAVE_VPRINTF
#undef HAVE_DIRECT_H
#undef HAVE_SNPRINTF
//#include <qgsvectorfilewriter.h>
//////////////////////////////////////////////////////////////////////////
#include <qgsvectorfilewriter.h>


// from brat
#include "libbrathl/Date.h"

#include "new-gui/Common/+UtilsIO.h"
#include "new-gui/Common/QtUtils.h"
#include "new-gui/Common/tools/Trace.h"

#include "DataModels/PlotData/MapProjection.h"
#include "DataModels/PlotData/BratLookupTable.h"
#include "DataModels/PlotData/MapColor.h"
#include "DataModels/PlotData/PlotValues.h"
#include "DataModels/PlotData/Contour/ListContour.h"

#include "BratLogger.h"
#include "BratSettings.h"
#include "GUI/ActionsTable.h"

#include "MapTip.h"
#include "SublayersDialog.h"
#include "MapWidget.h"



// This method will load with OGR the layers  in parameter. It has been conceived to use the new URI
// format of the OGR provider so as to give precisions about which sublayer to load into QGIS. It is normally triggered by the
// sublayer selection dialog.
//
QgsVectorLayer* LoadOGRSublayer( QString layertype, QString uri, QString name )
{
    // The uri must contain the actual uri of the vectorLayer from which we are going to load the sub-layer.
	//
    QString fileName = QFileInfo( uri ).baseName();

    QString composedURI;
    QStringList elements = name.split( ":" );
    while ( elements.size() > 2 )
    {
        elements[0] += ":" + elements[1];
        elements.removeAt( 1 );
    }

    QString sub_name = elements.value( 0 );
    QString sub_type = elements.value( 1 );
    if ( sub_type == "any" )
    {
        sub_type = "";
        elements.removeAt( 1 );
    }

    if ( layertype != "GRASS" )
    {
        composedURI = uri + "|layername=" + sub_name;
    }
    else
    {
        composedURI = uri + "|layerindex=" + sub_name;
    }

    if ( !sub_type.isEmpty() )
    {
        composedURI += "|geometrytype=" + sub_type;
    }

    QgsDebugMsg( "Creating new vector layer using " + composedURI );

    name.replace( ":", " " );
    QgsVectorLayer *layer = new QgsVectorLayer( composedURI, name, "ogr", false );
    if ( !layer || !layer->isValid() )
    {
        LOG_WARN( "Invalid Data Source" + QString( "%1 is not a valid or recognized data source" ).arg( composedURI ) );
		delete layer;
		layer = nullptr;
    }

	return layer;
}


QgsVectorLayer* CMapWidget::AskUserForOGRSublayers( QWidget *parent, QgsVectorLayer *layer )
{
    assert__( layer );

    QStringList sublayers = layer->dataProvider()->subLayers();

    CSublayersDialog dlg( parent, sublayers );
    if ( dlg.exec() == QDialog::Accepted )
    {
        QString uri = layer->source();
        
		//the separator char & was changed to | to be compatible with URL for protocol drivers
		//
        if ( uri.contains( '|', Qt::CaseSensitive ) )
        {
            // If we get here, there are some options added to the filename.
            // A valid uri is of the form: filename&option1=value1&option2=value2,...
            // We want only the filename here, so we get the first part of the split.
			//
            QStringList theURIParts = uri.split( "|" );
            uri = theURIParts.at( 0 );
        }

		QString layertype = layer->dataProvider()->storageType();
        LOG_TRACE( "Layer type " + layertype );

        return LoadOGRSublayer( layertype, uri, dlg.selectionName() );
    }
	return nullptr;
}


bool CMapWidget::OpenLayer( QWidget *parent, const QString &path, QgsRectangle &bounding_box )
{
	QString src = path.trimmed();
	QString base;

	QFileInfo fi( src );
	base = fi.completeBaseName();	QgsDebugMsg( "completeBaseName: " + base );

    QgsVectorLayer *layer = new QgsVectorLayer( src, base, "ogr", true );
	if ( !layer )
	{
		LOG_WARN( "Could not create layer from " + path );
		return false;
	}

	bool result = layer->isValid();
	if ( result )
	{
        layer->setProviderEncoding( "System" );
        QStringList sublayers = layer->dataProvider()->subLayers();
        LOG_TRACE( QString( "got valid layer with %1 sub-layers" ).arg( sublayers.count() ) );

        // Show the sub-layers selection dialog so the user can confirm/select the sub-layer to actually load.
		//
        if ( sublayers.count() > 0 )
        {
            auto *sublayer = AskUserForOGRSublayers( parent, layer );
			if ( sublayer )
			{
				delete layer;
				layer = sublayer;
				bounding_box = layer->extent();
			}
        }
	}
	else
	{
		QString msg = tr( "%1 is not a valid or recognized data source" ).arg( src );
		LOG_WARN( "Invalid Data Source " + msg );
	}

	delete layer;

	return result;
}


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
//static
std::string CMapWidget::smURLRasterLayerPath;


void CMapWidget::SetQGISDirectories(const std::string &QgisPluginsDir,
    const std::string &VectorLayerPath,
    const std::string &RasterLayerPath ,
    const std::string &URLRasterLayerPath)
{
	smQgisPluginsDir = QgisPluginsDir;
	smVectorLayerPath = VectorLayerPath;
	smRasterLayerPath = RasterLayerPath;
    smURLRasterLayerPath = URLRasterLayerPath;
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
const std::string& CMapWidget::URLRasterLayerPath()
{
    //no assert, not mandatory

    return smURLRasterLayerPath;
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


#if defined (WIN32) || defined(_WIN32)
#pragma warning ( disable : 4996 )
#endif

void CMapWidget::Init()
{
    // Instantiate Provider Registry
	//
    QgsProviderRegistry *preg = QgsProviderRegistry::instance( t2q( smQgisPluginsDir ) );
	Q_UNUSED( preg );

    assert__( mLayerBaseType != eVectorLayer || IsFile( smVectorLayerPath ) );
    assert__( mLayerBaseType != eRasterLayer || IsFile( smRasterLayerPath ) );
		
    if ( mLayerBaseType == eVectorLayer )
    {
        QgsVectorLayer *l = AddOGRVectorLayer( t2q( smVectorLayerPath ) );
        l->rendererV2()->symbols()[ 0 ]->setColor( "black" );
        mMainLayer = l;
//        QgsVectorSimplifyMethod simp;
//        simp.setSimplifyHints( QgsVectorSimplifyMethod::NoSimplification);
//        l->setSimplifyMethod( simp );
    }
    else
    {
        std::string raster_url = mLayerBaseType == eRasterLayer ? smRasterLayerPath : smURLRasterLayerPath;
        //"/home/brat/s3-altb/project/dev/support/data/WebMapServices_xml_files/map_GoogleMaps_jpg.xml";
        std::string provider = mLayerBaseType == eRasterLayer ? "" : "wms";
        mMainRasterLayer = AddRasterLayer( t2q( raster_url ), "raster", t2q( provider ) );
		if ( mMainRasterLayer )
		{
			//auto crs = mMainRasterLayer->crs();
			//setMapUnits( crs.mapUnits() );
			//auto crs = mMainRasterLayer->crs();

            //mMainRasterLayer->setCrs( mapSettings().destinationCrs() );
            //setDestinationCrs( mMainRasterLayer->crs() );
        }
		else if ( mLayerBaseType == eRasterURL )
		{
			if ( !IsFile( smRasterLayerPath ) )
				throw CException( "Loading layer from URL or file failed. Cannot continue." );

			mLayerBaseType = eRasterLayer;
			mMainRasterLayer = AddRasterLayer( t2q( smRasterLayerPath ), "raster", "" );
			LOG_WARN( "Raster layer URL seems invalid. Tried to load the default layer instead." );
		}
		mMainLayer = mMainRasterLayer;
	}

	//addRasterLayer( "http://server.arcgisonline.com/arcgis/rest/services/ESRI_Imagery_World_2D/MapServer?f=json&pretty=true", "raster", "" );
	//crs=EPSG:900913&dpiMode=7&featureCount=10&format=image/png&layers=precipitation&styles=&url=http://wms.openweathermap.org/service




    /////////////////// TODO RCCC //////////////////////////////////
//    auto l = new QgsRasterLayer( QString("contextualWMSLegend=0&crs=EPSG:4326&dpiMode=all&featureCount=10&format=image/png&layers=bluemarble&styles=&url=http://disc1.sci.gsfc.nasa.gov/daac-bin/wms_airsnrt?layer%3DAIRS_SO2_A%26" /*"http://localhost:8080/geoserver/wfs?srsname=EPSG:23030&typename=union&version=1.0.0&request=GetFeature&service=WFS"*/  /*"url=http://kaart.maaamet.ee/wms/alus&format=image/png&layers=MA-ALUS&styles=&crs=EPSG:AUTO"*/),
//                                 QString("basemap"),
//                                 QString("wms"),
//                                 false
//                                 );


//    if ( l->isValid() )
//    {
//                            qDebug( "Layer is valid" );

//        // Add the Raster Layer to the Layer Registry
//        QgsMapLayerRegistry::instance()->addMapLayer(l, true);

//        // Add the Layer to the Layer Set
//        mLayerSet.append(QgsMapCanvasLayer(l, TRUE));

//        // Set the Map Canvas Layer Set			//TODO: check if we need to SetLayerSet every time
//        SetLayerSet(mLayerSet);
//    }
//    else
//    {
//        qDebug( "Layer is NOT valid, basemap not loaded" );
//        delete l;
//    }
    //////////////////////////////////////////////////////////////////


    //mMainLayer =  new QgsVectorLayer(mVectorLayerPath, myLayerBaseName, myProviderName);
    //QgsSingleSymbolRendererV2 *mypRenderer = new QgsSingleSymbolRendererV2(QgsSymbolV2::defaultSymbol(mMainLayer->geometryType()));

	//setupDatabase();
	//setupMapLayers();

	//4. We need the index for the population value:
	//i = tractLyr.fieldNameIndex('POPULAT11')
	//5. Now, we get our census layer's features as an iterator:
	//features = tractLyr.getFeatures()
	//6. We need a data provider for the memory layer so that we can edit it:
	//auto vpr = popLyr->dataProvider();

    setWheelAction( WheelZoomToMouseCursor, 1.1 );

    setExtent( mMainLayer->extent() );
    enableAntiAliasing( true );
    setCanvasColor( QColor( 0xFF, 0xFF, 0xF0, 255 ) );		//setCanvasColor(QColor(255, 255, 255));
    freeze( false );

	// projections
	//
	setCrsTransformEnabled( true );
    //mapRenderer()->setProjectionsEnabled( true );
	//SetProjection( PROJ2D_LAMBERT_AZIMUTHAL );
    ///// RCCC TODO - To use Google Maps TMS define 4326 as default projection after adding Google raster //////
    //mDefaultProjection = { 4326, QgsCoordinateReferenceSystem::EpsgCrsId }; // NOTE: comment the following line.
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
	mDefaultProjection = mMainLayer->crs();
	mDefaultProjection.validate();		   						assert__( mDefaultProjection.isValid() );
	if ( mDefaultProjection.mapUnits() == QGis::UnknownUnit )
		LOG_WARN("Base map layer has unknown units");
	//setDestinationCrs( mDefaultProjection );
	//mMainLayer->setCrs( QgsCoordinateReferenceSystem() );
	//SetDefaultProjection();

    //// Set the Map Canvas Layer Set
    //SetLayerSet(mLayerSet);

    setMinimumSize( min_globe_widget_width, min_globe_widget_height );    //setVisible(true);
	setParallelRenderingEnabled( false );
    refresh();

	
#if defined (DEBUG) || defined(_DEBUG)

	//context menu

	setContextMenuPolicy( Qt::ActionsContextMenu );
	mActionAddRaster = new QAction( "Add Raster", this );
	connect( mActionAddRaster, SIGNAL( triggered() ), this, SLOT( HandleAddRaster() ) );
	addAction( mActionAddRaster );
	
	mActionRemoveLayers = new QAction( "Remove Layers", this );
	connect( mActionRemoveLayers, SIGNAL( triggered() ), this, SLOT( HandleRemoveLayers() ) );
	addAction( mActionRemoveLayers );

#endif
}

#if defined (WIN32) || defined(_WIN32)
#pragma warning ( default : 4996 )
#endif


CMapWidget::CMapWidget( ELayerBaseType layer_base_type, QWidget *parent, bool with_tracks_layer )		//with_tracks_layer = false 
	: base_t( parent )
    , mLayerBaseType( layer_base_type )
{
	Init();
	if ( with_tracks_layer )
		PlotTrack( nullptr, nullptr, nullptr, 0, REF19500101 );		//Use ReadData ref date instead of product->GetRefDate();
}


//virtual
CMapWidget::~CMapWidget()
{
	freeze( true );
	QList<QgsMapCanvasLayer> emptyList;
	setLayerSet( emptyList );
	clearCache();
	//  QgsMapLayerRegistry::instance()->removeAllMapLayers();

	assert__( !isDrawing() );

	delete mSelectFeatures;
	delete mSelectPolygon;
	delete mMeasureDistance;
	delete mMeasureArea;
}


void CMapWidget::closeEvent( QCloseEvent *event )
{
	if ( isDrawing() ) 
	{
		event->ignore();
	}
	else {
		event->accept();
	}
}




////////////////////////////
//	Debug Context Menu 
////////////////////////////


void CMapWidget::HandleAddRaster()
{
    if ( mLayerBaseType != eVectorLayer )
		return;

	if ( IsFile( smRasterLayerPath ) )
		mMainRasterLayer = AddRasterLayer( t2q( smRasterLayerPath ), "raster", "" );
}

void CMapWidget::HandleRemoveLayers()
{
	RemoveLayers();
}


////////////////////////////
//		Projections
////////////////////////////

bool CMapWidget::SetDefaultProjection()
{
	return SetProjection( mDefaultProjection );
}


//void QgsProjectionSelector::loadCrsList( QSet<QString> *crsFilter )
//
bool CMapWidget::SetProjection( const QgsCoordinateReferenceSystem &crs )
{
	assert__( hasCrsTransformEnabled() );

	if ( crs.mapUnits() == QGis::UnknownUnit )
	{
		LOG_WARN( "Projection with unknown map units." );
		return false;
	}

	if ( !crs.isValid() )
	{
		LOG_WARN( "Invalid projection specification." );
		return false;
	}

	freeze();

	setDestinationCrs( crs );
	setMapUnits( crs.mapUnits() );

	if ( mDecorationGrid )
	{
		mDecorationGrid->setMapUnits( crs.mapUnits() );
		if ( !mDecorationGrid->enabled() )				//the grid can disable itself
			if ( mActionDecorationGrid )
				mActionDecorationGrid->setChecked( false );
	}

//    mMainLayer = (QgsVectorLayer*)mLayerSet.last().layer();
//    while( mLayerSet.size() > 0 )
//        mLayerSet.removeLast();
//    mLayerSet.append( QgsMapCanvasLayer( mMainLayer, true ) );


	//for ( auto &layer : mLayerSet )
	//{
	//	//layer.setVisible( true );
	//	auto *vlayer = qobject_cast<QgsVectorLayer*>( layer.layer() );
	//	if ( vlayer && vlayer != mMainLayer )
	//	{
	//		vlayer->setCrs( QgsCoordinateReferenceSystem().createFromProj4( crs.toProj4() ) );
	//		//vlayer->setCrs( crs );			//vlayer->setCoordinateSystem();
	//		//vlayer->updateExtents();
	//		vlayer->triggerRepaint();
	//	}
	//}
    //SetLayerSet( mLayerSet );

	//QgsCoordinateReferenceSystem crs = currentLayer()->crs();
	//freeze();
	//setDestinationCrs( crs );
	//if ( crs.mapUnits() != QGis::UnknownUnit )
	//{
	//	setMapUnits( crs.mapUnits() );
	//}

	//SetCurrentLayer( mLayerSet.last().layer() );

	freeze( false );

	//refresh();

	Home();

	return true;
}

bool CMapWidget::SetProjection( unsigned id )
{
	assert__( id != PROJ2D_3D );	//3D projections not supported in 2D maps...

	return SetProjection( CMapProjection::GetInstance()->IdToCRS( id ) );
}


void CMapWidget::Home()
{
	zoomToFullExtent();		//makes refresh()
}



//inline bool Save2PS( QWidget *w )
//{
//	QPrinter printer( QPrinter::HighResolution );
//	printer.setOutputFormat( QPrinter::PostScriptFormat );
//	printer.setOutputFileName( m_currentTopic + ".ps" );
//	qreal xmargin = contentRect.width()*0.01;
//	qreal ymargin = contentRect.height()*0.01;
//	printer.setPaperSize( 10 * contentRect.size()*1.02, QPrinter::DevicePixel );
//	printer.setPageMargins( xmargin, ymargin, xmargin, ymargin, QPrinter::DevicePixel );
//	QPainter painter;
//	painter.begin( &printer );
//	w->render( &painter/*, QPointF( 0, 0 ), contentRect */);
//	painter.end();
//
//	return true;
//}



bool CMapWidget::Save2Image( const QString &path, const QString &format, const QString &extension )
{
	while ( isDrawing() )
		qApp->processEvents();

	QString qpath = path;
	QString f = format.toLower();
	SetFileExtension( qpath, extension );
	if ( f == "ps" )
	{
		QPrinter printer( QPrinter::HighResolution );
		printer.setOutputFormat( QPrinter::PostScriptFormat );
		printer.setOutputFileName( qpath );
		//qreal xmargin = contentRect.width()*0.01;
		//qreal ymargin = contentRect.height()*0.01;
		//printer.setPaperSize( 10 * contentRect.size()*1.02, QPrinter::DevicePixel );
		//printer.setPageMargins( xmargin, ymargin, xmargin, ymargin, QPrinter::DevicePixel );
		QPainter painter;
		painter.begin( &printer );
		render( &painter/*, QPointF( 0, 0 ), contentRect */ );
		painter.end();
		return true;
	}


	QPixmap pix = QPixmap::grabWidget( this );
	if ( pix.isNull() )
	{
		return false;
	}

	return pix.save( qpath, q2a( format ).c_str() );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		Fields & Attributes
//
//	- Data
//	- Vector
//	- Time
//	- Contour
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////
//		Generic
/////////////////////////////


QgsFields::FieldOrigin field_origin = QgsFields::OriginUnknown;


//	field=name:type(length,precision)
//
inline QString LayerFieldTypeName( QVariant::Type type )
{
	switch ( type )
	{
		case QVariant::Double:
			return "double";
			break;

		case QVariant::Int:
			return "integer";
			break;

		case QVariant::String:
			return "string(120)";
			break;

		default:
			assert__( false );		//new type names must be added here
	}
	return "";
}

//example
//"Point?
	//field=Rec_No:integer
	//&field=Include:string(120)
	//&field=X:double



struct CFieldDefinition
{
	const QString key;
	const QVariant::Type qtype;
	const QString type;

	CFieldDefinition( const QString &k, QVariant::Type q )
		: key( k )
		, qtype( q )
		, type( LayerFieldTypeName( qtype ) )
	{}


	//	field=name:type(length,precision)
	//
	QString MakeLayerDef() const
	{
		return "&field=" + key + ":" + LayerFieldTypeName( qtype );
	}
};



static const CFieldDefinition data_def{ "height", QVariant( 0. ).type() };

static const CFieldDefinition ref_date_def{ "ref_date", QVariant( 0 ).type() };

static const CFieldDefinition magnitude_def{ "magnitude", QVariant( 0. ).type() };

static const CFieldDefinition angle_def{ "angle", QVariant( 0. ).type() };



//static 
const char* CMapWidget::ReferenceDateFieldKey()
{
	static std::string key = q2a( ref_date_def.key );
	return key.c_str();
}




using attrs_t = std::vector< std::pair< QString, QVariant > >;





inline QgsFields CreateFieldsFromLayerFields( const std::vector<const CFieldDefinition*> &layer_fields )
{
	QgsFields fields;
    for ( auto const *layer_field : layer_fields )
        fields.append( QgsField( layer_field->key, layer_field->qtype ), field_origin );

	return fields;
}



/////////////////////////////
//	...data
/////////////////////////////



inline std::vector<const CFieldDefinition*> LayerDataFields()
{
	static std::vector<const CFieldDefinition*> v{ &data_def };
	return v;
}
attrs_t CreateDataAttributes( double value )
{
	attrs_t attrs =
	{
		{ data_def.key, value },
	};

	return attrs;
}
inline QgsFields DataFields()
{
	static QgsFields fields = CreateFieldsFromLayerFields( LayerDataFields() );
	return fields;
}



/////////////////////////////
//	...vector
/////////////////////////////


inline std::vector<const CFieldDefinition*> LayerVectorFields()
{
    static std::vector<const CFieldDefinition*> v{ &magnitude_def, &angle_def };
	return v;
}
attrs_t CreateVectorAttributes( double angle, double magnitude )
{
	attrs_t attrs =
	{
        { magnitude_def.key, magnitude },
        { angle_def.key, angle }
    };

	return attrs;
}
inline QgsFields VectorFields()
{
	static QgsFields fields = CreateFieldsFromLayerFields( LayerVectorFields() );
	return fields;
}



/////////////////////////////
//	...time
/////////////////////////////


inline std::vector<const CFieldDefinition*> LayerTimeFields()
{
	static std::vector<const CFieldDefinition*> v{ &data_def, &ref_date_def };
	return v;
}
attrs_t CreateTimeAttributes( int ref_date, double value )
{
	attrs_t attrs =
	{
		{ data_def.key, value },
        { ref_date_def.key, ref_date },
	};

	return attrs;
}
inline QgsFields TimeFields()
{
	static QgsFields fields = CreateFieldsFromLayerFields( LayerTimeFields() );
	return fields;
}






////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		Features
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const double default_data_step = 0.333;
static const double track_symbol_width = 0.1;
static const int data_layer_tranparency = 10;

//static 
QgsFeatureList& CMapWidget::CreatePointDataFeature( QgsFeatureList &list, double lon, double lat, double value )
{
	QgsFeature *f = new QgsFeature( DataFields() );
	f->setGeometry( QgsGeometry::fromPoint( QgsPoint( lon, lat ) ) );

	const attrs_t &attrs = CreateDataAttributes( value );
	for ( auto ii = attrs.begin(); ii != attrs.end(); ++ii )
	{
		f->setAttribute( ii->first, ii->second );
	}

	list.append( *f );
	return list;
}


//static 
QgsFeatureList& CreateLineDataFeature( QgsFeatureList &list, double value, QgsPolyline points )
{
	QgsFeature *f = new QgsFeature( DataFields() );
	f->setGeometry( QgsGeometry::fromPolyline( points ) );

	const attrs_t &attrs = CreateDataAttributes( value );
    for ( auto ii = attrs.begin(); ii != attrs.end(); ++ii )
    {
        f->setAttribute( ii->first, ii->second );
    }

	list.append( *f );
	return list;
}


QgsFeatureList& CreatePointTrackFeature( QgsFeatureList &list, double lon, double lat, int ref_date, double value )
{
	QgsFeature *f = new QgsFeature( TimeFields() );
	f->setGeometry( QgsGeometry::fromPoint( QgsPoint( lon, lat ) ) );

	const attrs_t &attrs = CreateTimeAttributes( ref_date, value );
	for ( auto ii = attrs.begin(); ii != attrs.end(); ++ii )
	{
		f->setAttribute( ii->first, ii->second );
	}

	list.append( *f );
	return list;
}





//static
QgsFeatureList& CMapWidget::CreatePolygonDataFeature( QgsFeatureList &list, double lon, double lat, double value, double step_x, double step_y )
{
    QgsPolyline polyline;
    QgsPolygon polygon;

    double lon_max = lon + step_x/2;
    double lon_min = lon - step_x/2;
    double lat_min = lat - step_y/2;
    double lat_max = lat + step_y/2;

    lon_max = lon_max < 180 ? lon_max : 179.99;
    lon_min = lon_min > -180 ? lon_min : -179.99;
    lat_max = lat_max < 90 ? lat_max : 89.99;
    lat_min = lat_min > -90 ? lat_min : -89.99;

    polyline.append( QgsPoint( lon_min , lat_max ) );
    polyline.append( QgsPoint( lon_max , lat_max ) );
    polyline.append( QgsPoint( lon_max , lat_min ) );
    polyline.append( QgsPoint( lon_min , lat_min ) );

    polygon.append( polyline );

    QgsFeature *f = new QgsFeature( DataFields() );
    f->setGeometry( QgsGeometry::fromPolygon( polygon ) );

	const attrs_t &attrs = CreateDataAttributes( value );
    for ( auto ii = attrs.begin(); ii != attrs.end(); ++ii )
    {
        f->setAttribute( ii->first, ii->second );
    }

    list.append( *f );
    return list;
}


//static
QgsFeatureList& CMapWidget::CreateVectorFeature( QgsFeatureList &list, double lon, double lat, double angle, double magnitude  )
{
    QgsFeature *f = new QgsFeature( VectorFields() );
    f->setGeometry( QgsGeometry::fromPoint( QgsPoint( lon, lat ) ) );

	const attrs_t &attrs = CreateVectorAttributes( angle, magnitude );
	for ( auto ii = attrs.begin(); ii != attrs.end(); ++ii )
    {
        f->setAttribute( ii->first, ii->second );
    }

    list.append( *f );
    return list;
}


//static
void CMapWidget::RemoveFeatures( QgsVectorLayer *layer )
{
	//layer->selectAll();
	//layer->deleteSelectedFeatures();

	layer->dataProvider()->deleteFeatures( layer->allFeatureIds() );
	layer->updateExtents();
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		Symbols & RubberBand
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


inline QgsStringMap qsm( const char *s1, const std::string &s2 )
{
	QgsStringMap m;
	m.insert( s1, s2.c_str() );
	return m;
};


//static
QgsSymbolV2* CMapWidget::CreatePolygonSymbol( double width, const QColor &color )
{
	Q_UNUSED( width );

    //QgsSymbolV2 *s = QgsMarkerSymbolV2::createSimple( qsm( "","" ) );
    QgsFillSymbolV2 *s = QgsFillSymbolV2::createSimple( qsm( "","" ) );
    //qDebug() << "///// TYPE:" << s->type();
    s->deleteSymbolLayer( 0 );		// Remove default symbol layer.
    //s->setColor( color );

    //s->setMapUnitScale()

    //auto symbolLayer = new QgsSimpleMarkerSymbolLayerV2;
    auto symbolLayer = new QgsSimpleFillSymbolLayerV2;

    symbolLayer->setBorderColor( color );
	//symbolLayer->setBorderWidthUnit( QgsSymbolV2::Pixel );
	//symbolLayer->setBorderWidth( symbolLayer->borderWidth() / 2 );
    //symbolLayer->setBorderStyle( Qt::NoPen );
	//symbolLayer->setPenJoinStyle( Qt::SvgMiterJoin );
    //symbolLayer->setFillColor( color );		== setColor
    //symbolLayer->setOutlineColor( color );

    symbolLayer->setColor( color );

    //symbolLayer->setName("square");					//
    //symbolLayer->setSizeUnit( QgsSymbolV2::MM );	//
    //symbolLayer->setSize( width );
    //symbolLayer->setOutlineStyle( Qt::NoPen );

    s->appendSymbolLayer( symbolLayer );

    return s;
}


//static 
QgsSymbolV2* CMapWidget::CreateLineSymbol( double width, const QColor &color )
{
	Q_UNUSED( width );

	QgsSymbolV2 *s = QgsLineSymbolV2::createSimple( qsm( "", "" ) );
	s->deleteSymbolLayer( 0 ); // Remove default symbol layer.

	auto symbolLayer = new QgsSimpleLineSymbolLayerV2;
    symbolLayer->setColor( color );
	symbolLayer->setWidth( width );
	//symbolLayer->setWidthUnit( QgsSymbolV2::MapUnit );

	s->appendSymbolLayer( symbolLayer );

	return s;
}


//static 
QgsSymbolV2* CMapWidget::CreatePointSymbol( double width, const QColor &color )
{
    QgsSymbolV2 *s = QgsMarkerSymbolV2::createSimple( qsm( "","" ) );
    s->deleteSymbolLayer( 0 );		// Remove default symbol layer.
	//s->setColor( lineColor );
	//s->setMapUnitScale()

    auto symbolLayer = new QgsSimpleMarkerSymbolLayerV2;
    symbolLayer->setColor( color );
    symbolLayer->setName("circle");
    symbolLayer->setSizeUnit( QgsSymbolV2::MapUnit );
    symbolLayer->setSize( width );
    symbolLayer->setOutlineStyle( Qt::NoPen );

    s->appendSymbolLayer( symbolLayer );

    return s;
}


//static
QgsSymbolV2* CMapWidget::CreateArrowSymbol( const QColor &color )
{
    QgsSymbolV2 *s = QgsMarkerSymbolV2::createSimple( qsm( "","" ) );
    s->deleteSymbolLayer( 0 );		// Remove default symbol layer.
    //s->setColor( lineColor );
    //s->setMapUnitScale()

    auto symbolLayer = new QgsSimpleMarkerSymbolLayerV2;
    symbolLayer->setColor( color );
    symbolLayer->setName("arrow");					//
    //symbolLayer->setSizeUnit( QgsSymbolV2::MM );	//	RCCC
    symbolLayer->setSizeUnit( QgsSymbolV2::Pixel );	//
    //symbolLayer->setSize( width );
    //symbolLayer->removeDataDefinedProperties();
    symbolLayer->setDataDefinedProperty( "size", magnitude_def.key );	//  m->setDataDefinedProperty( "size", props["size_expression"] );
    symbolLayer->setDataDefinedProperty( "angle", angle_def.key );		//	m->setDataDefinedProperty( "angle", props["angle_expression"] );
    symbolLayer->setOutlineStyle( Qt::NoPen );

    s->appendSymbolLayer( symbolLayer );

    return s;
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		Layer Construction
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#if !defined(TRUE)
#define TRUE 1
#endif

static const std::string index_symbol = "#";

inline std::string CreateUniqueLayerName( const std::string &name )
{
	static const std::string base_name = "layer";
	static size_t index = 0;

	std::string layer_name = name;
	if ( layer_name.empty() )
		layer_name = base_name;

	layer_name += ( index_symbol + n2s<std::string>(index++) );

	return layer_name;
}
	

QString CMapWidget::ExtractLayerBaseName( const QString &qname )
{
	std::string name = q2a( qname );
	auto pos = name.find( index_symbol );

	if ( std::string::npos == pos )
		return "";

	return name.substr( 0, pos ).c_str();
}
	

#include <qgsvectorlayerrenderer.h>
class CVectorLayerRenderer : public QgsVectorLayerRenderer
{
	using base_t = QgsVectorLayerRenderer;
public:
	CVectorLayerRenderer( QgsVectorLayer *layer, QgsRenderContext &context )
		: base_t( layer, context )
	{}
	virtual ~CVectorLayerRenderer()
	{}

	virtual bool render() override
	{
		try{
            return base_t::render();
		}
		catch ( ... )
		{
		}
        return false;
	}
};

inline QgsVectorLayer* CreateVectorLayer( const std::string &name, const QString &layer_path, const QString &provider, QgsFeatureRendererV2 *renderer )	//renderer = nullptr 
{
	auto l = new QgsVectorLayer( layer_path, CreateUniqueLayerName( name ).c_str(), provider );

    if ( !renderer )
        renderer = CMapWidget::CreateRenderer( l );

    l->setRendererV2( renderer );

    if ( l->isValid() )
    {
		return l;
	}

	LOG_WARN( "Vector layer is NOT valid" );
	delete l;
	return nullptr;
}




//auto l = new QgsRasterLayer( //"url=http://localhost:8080/geoserver/wfs?srsname=EPSG:23030&typename=union&version=1.0.0&request=GetFeature&service=WFS",
						//	//QString("url=http://kaart.maaamet.ee/wms/alus&format=image/png&layers=MA-ALUS&styles=&crs=EPSG:AUTO"),
						//	 "contextualWMSLegend=0&crs=EPSG:4326&dpiMode=all&featureCount=10&format=image/png&layers=bluemarble&styles=&url=http://disc1.sci.gsfc.nasa.gov/daac-bin/wms_airsnrt?layer%3DAIRS_SO2_A%26",
						//	 /*"http://localhost:8080/geoserver/wfs?srsname=EPSG:23030&typename=union&version=1.0.0&request=GetFeature&service=WFS"*/  
						//	 /*"url=http://kaart.maaamet.ee/wms/alus&format=image/png&layers=MA-ALUS&styles=&crs=EPSG:AUTO",*/
//                             "basemap",
//                             "wms",
//                             false
//                             );
//
QgsRasterLayer* CMapWidget::AddRasterLayer( const QString &layer_path, const QString &base_name, const QString &provider, QgsSymbolV2* symbol )	//symbol = nullptr
{
    Q_UNUSED( symbol );

	std::string name = CreateUniqueLayerName( q2a( base_name ) );
    auto l = provider.isEmpty() ? 
		new QgsRasterLayer( layer_path, name.c_str() ) : 
		new QgsRasterLayer( layer_path, name.c_str(), provider );

	if ( l->isValid() ) 
	{
		// Add the Vector Layer to the Layer Registry
		QgsMapLayerRegistry::instance()->addMapLayer(l, TRUE);

		// Add the Layer to the Layer Set
		mLayerSet.append(QgsMapCanvasLayer(l, TRUE));

		// Set the Map Canvas Layer Set			//TODO: check if we need to SetLayerSet every time
		SetLayerSet();

		SetCurrentLayer( l );

		return l;
	}

	LOG_WARN( "Raster layer is NOT valid" );
	delete l;
	return nullptr;
}


QgsVectorLayer* CMapWidget::AddVectorLayer( QgsVectorLayer *l )
{
    if ( l && l->isValid() )
    {
		LOG_TRACE( "Layer is valid" );

		// Add the Vector Layer to the Layer Registry
		QgsMapLayerRegistry::instance()->addMapLayer( l, true );

		// Add the Layer to the Layer Set
		mLayerSet.append( QgsMapCanvasLayer( l, true ) );

		// Set the Map Canvas Layer Set
		SetLayerSet();

		SetCurrentLayer( l );
	}

	return l;
}


//QgsVectorLayer* CMapWidget::AddVectorLayer( const std::string &name, const QString &layer_path, const QString &provider, QgsFeatureRendererV2 *renderer )	//renderer = nullptr 
//{
//	static const std::string base_name = "vlayer";
//	static const std::string index_symbol = "#";
//	static size_t index = 0;
//
//	std::string layer_name = name;
//	if ( layer_name.empty() )
//		layer_name = base_name;
//
//	layer_name += ( index_symbol + n2s<std::string>(index++) );
//
//	auto l = new QgsVectorLayer( layer_path, layer_name.c_str(), provider );
//
//    if ( !renderer )
//        renderer = CreateRenderer( l );
//    l->setRendererV2( renderer );
//
//    if ( l->isValid() )
//    {
//							qDebug( "Layer is valid" );
//
//		// Add the Vector Layer to the Layer Registry
//		QgsMapLayerRegistry::instance()->addMapLayer( l, true );
//
//		// Add the Layer to the Layer Set
//		mLayerSet.append( QgsMapCanvasLayer( l, true ) );
//
//		// Set the Map Canvas Layer Set
//		SetLayerSet();
//
//		SetCurrentLayer( l );
//
//		return l;
//	}
//							LOG_WARN( "Layer is NOT valid" );
//	delete l;
//	return nullptr;
//}
//
//
QgsVectorLayer* CMapWidget::AddOGRVectorLayer( const QString &layer_path, QgsSymbolV2* symbol )		//symbol = nullptr 
{
	static const QString provider_name = "ogr";

	return AddVectorLayer( CreateVectorLayer( CreateUniqueLayerName( q2a( provider_name ) ), layer_path, provider_name, 
		symbol ? CreateRenderer( symbol ) : nullptr ) );
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////
//			Layer Geometry
///////////////////////////////////////////

enum ELayerGeometryType
{
	ePoint,	eLineString, ePolygon, eMultiPoint, eMultiLineString, eMultiPolygon, 
	
	ELayerGeometryType_size
};


inline std::vector<QString> LayerGeometryTypeNames()
{
	static const std::vector<QString> names =
	{
		"Point", "LineString", "Polygon", "MultiPoint", "MultiLineString", "MultiPolygon"
	};

	assert__( ELayerGeometryType_size == names.size() );

	return names;
}


inline QString LayerGeometryName( ELayerGeometryType type )
{
	static const std::vector<QString> names = LayerGeometryTypeNames();

	assert__( type >= 0 && type < ELayerGeometryType_size );

	return LayerGeometryTypeNames()[ type ];
}
	

//	URI examples
//
//		"Point?crs=epsg:4326&field=id:integer&field=name:string(20)&index=yes"
//
//		"Point?
//			field=Rec_No:integer
//			&field=Include:string(120)
//			&field=Label:string(120)
//			&field=X:double
//			&field=Y:double
//			&field=Z:double
//			&field=Height:double
//			&field=Project_Re:string(120)
//			&field=NCA:string(120)
//			&field=DayCrit:integer
//			&field=EveCrit:integer
//			&field=NightCrit:integer",
//	
//	experimenting with CRS
//
//		auto &crs = mapSettings().destinationCrs();
//		const QString scrs = mMainLayer->crs().toProj4();
//		const QString scrs = crs.toProj4();	
//
QgsVectorLayer* CreateMemoryLayer( ELayerGeometryType type, const std::string &name, const std::vector<const CFieldDefinition*> &field_defs, 
	QgsFeatureRendererV2 *renderer, QgsFeatureList &flist )
{
	static const QString base_name = "mem";
    static const QString provider = "memory";
    static const QString crs = "4326";
 
	QString layer_fields;
	for ( auto const *field_def : field_defs )
		layer_fields += field_def->MakeLayerDef();

	QString layer_path = LayerGeometryName( type ) + "?crs=EPSG:" + crs + layer_fields + "&index=yes";

	QgsVectorLayer *l = CreateVectorLayer( name, layer_path, provider, renderer );

	if ( l && !flist.empty() )
		l->dataProvider()->addFeatures( flist );	//l->updateExtents();

	return l;
}


bool ExportLayer( const QString &dir, QgsVectorLayer *l, QgsVectorFileWriter::SymbologyExport symbology = QgsVectorFileWriter::FeatureSymbology )
{
	const QString corrected_name = replace( replace( q2a( l->name() ), "/", "_" ), "\\", "_" ).c_str();
	const QString new_dir = dir + "/" + corrected_name;
	const QString path = new_dir + "/" + corrected_name + ".shp";
	const QString fileencoding( "System" );
	QgsCoordinateTransform *ct = new QgsCoordinateTransform( l->crs(), l->crs() );

	return
		MakeDirectory( q2a( new_dir ) ) &&
		QgsVectorFileWriter::writeAsVectorFormat( l,
			path,
			fileencoding,
			ct,
			"ESRI Shapefile",	// const QString& driverName = "ESRI Shapefile",
			false,				// bool onlySelected = false,
			0,					// QString *errorMessage = 0,
			QStringList(),		// const QStringList &datasourceOptions = QStringList(),
			QStringList(),		// const QStringList &layerOptions = QStringList(),
			false,				// bool skipAttributeCreation = false,
			0,					// QString *newFilename = 0,
			symbology,			// SymbologyExport symbologyExport = NoSymbology,
			1.0,				// double symbologyScale = 1.0,
			0					// const QgsRectangle* filterExtent = 0
			) == QgsVectorFileWriter::WriterError::NoError;
}

/*
QgsVectorLayer* CMapWidget::AddMemoryLayer( const std::string &name, QgsSymbolV2* symbol )		//name = "", symbol = nullptr 
{
	return AddMemoryLayer( false, name, CreateRenderer( symbol ) );
}
*/


//static 
template< class LAYER >
QgsSingleSymbolRendererV2* CMapWidget::CreateRenderer( LAYER *layer )
{
	return new QgsSingleSymbolRendererV2( QgsSymbolV2::defaultSymbol( layer->geometryType() ) );
}
	
//static 
QgsSingleSymbolRendererV2* CMapWidget::CreateRenderer( QgsSymbolV2* symbol )
{
	return new QgsSingleSymbolRendererV2( symbol );
}

//
//	auto target_field = "height";
//	auto range_list = new QgsRangeList;
//	auto opacity = 1;
//	// Make our first symbol and range...
//	auto myMin = 1491.0;
//	auto myMax = 1491.5;
//	auto myLabel = "Group 1";
//	auto myColour = QColor( "#ffee00" );
//	auto mySymbol1 = CreatePointSymbol( 0.5, myColour );
//	mySymbol1->setAlpha( opacity );
//	auto myRange1 = new QgsRendererRangeV2( myMin, myMax, mySymbol1, myLabel );
//	range_list->append( *myRange1 );
//	//now make another symbol and range...
//	myMin = 1491.6;
//	myMax = 1492.0;
//	myLabel = "Group 2";
//	myColour = QColor( "#00eeff" );
//	auto mySymbol2 = CreatePointSymbol( 0.5, myColour );
//	mySymbol2->setAlpha( opacity );
//	auto myRange2 = new QgsRendererRangeV2( myMin, myMax, mySymbol2, myLabel );
//	range_list->append( *myRange2 );
//	auto renderer = new QgsGraduatedSymbolRendererV2( "", *range_list );
//	renderer->setMode( QgsGraduatedSymbolRendererV2::Jenks );
//	renderer->setClassAttribute( target_field );
//
//static 
QgsGraduatedSymbolRendererV2* CMapWidget::CreateRenderer( const QString &target_field, double width, double m, double M, const QLookupTable *lut, create_symbol_t cs )
{
    static const auto opacity = 1.;
	//static const QColor color_map[] =
	//{
	//	QColor( "blue" ),
	//	QColor( "cyan" ),
	//	QColor( "green" ),
	//	QColor( "yellow" )
	//};
 //   static const DEFINE_ARRAY_SIZE( color_map );        Q_UNUSED(color_map_size);

	auto create_range = [&lut, &cs, &width]( double mx, double Mx, int index )
	{
		//int color_index = index;
		//if ( color_index >= color_map_size )
		//	color_index = 0;

		auto label = "Group " + n2s<std::string>( index + 1 );
		//auto symbol = CreatePointSymbol( width, color_map[ color_index ] );

        QColor c = lut->GetTableValue( lut->GetIndex( Mx ) );

		auto symbol = cs( width, c );
		symbol->setAlpha( opacity );
		return new QgsRendererRangeV2( mx, Mx, symbol, label.c_str() );
	};

	//function body

    auto range_list = new QgsRangeList;

	const double range = M - m;
    size_t color_map_size = lut->GetNumberOfTableValues();
    double step = range / color_map_size;

	double v = m;
    for ( size_t i = 0; i < color_map_size; ++i )
	{
	    range_list->append( *create_range( v, v + step, i ) );
		v += step;
	}

    auto renderer = new QgsGraduatedSymbolRendererV2( target_field, *range_list );
    renderer->setMode( QgsGraduatedSymbolRendererV2::EqualInterval );
	return renderer;
}


//static 
void CMapWidget::ChangeRenderer( QgsVectorLayer *layer, const QString &target_field, double width, double m, double M, const QLookupTable *lut, create_symbol_t cs )
{
	layer->setRendererV2( CreateRenderer( target_field, width, m, M, lut, cs ) );
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		Specialized Layers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/*
QgsVectorLayer* CMapWidget::AddDataLayer( const std::string &name, double width, double m, double M, const QLookupTable *lut, QgsFeatureList &flist )
{
	static const QString target_field = height_key;

	auto *l = AddMemoryLayer( polygon, name, CreateRenderer( target_field, width, m, M, lut,
		polygon ?
		&CMapWidget::CreatePolygonSymbol :
		&CMapWidget::CreatePointSymbol
		) );

	if ( l )
	{
		l->setLayerTransparency( 10 );				//TODO Magic Number
		l->dataProvider()->addFeatures( flist );
		//l->updateExtents();
		mDataLayers.push_back( l );
	}

	return l;


	////QgsVectorFileWriter::writeAsVectorFormat( l, "L:\\my.shp", "", &l->crs(), "ESRI Shapefile", false, 0, QStringList(), QStringList(), false, 0, QgsVectorFileWriter::SymbolLayerSymbology );

	//QgsFields fields;
	//fields.append( QgsField( height_key, QVariant::Type::Double ), field_origin );
	//fields.append( QgsField( ref_date_key, QVariant::Type::Int ), field_origin );
    //QgsVectorFileWriter r( "L:\\layer", "", fields, QGis::WkbType::WKBPoint, &l->crs(), "ESRI Shapefile", QStringList(), QStringList(), 0, QgsVectorFileWriter::SymbolLayerSymbology );
	//QgsFeatureRequest req;
	//QgsFeatureIterator it = l->dataProvider()->getFeatures( req );
	//QgsFeature f;
	//while ( it.nextFeature( f ) )
	//	r.addFeature( f );

	//if ( r.hasError() )
	//	qDebug() << r.errorMessage();

	//////////////////////////////////////////////////////////////////////////
//    QgsVectorColorRampV2 *myColorRamp = nullptr;
//    myColorRamp = QgsVectorGradientColorRampV2::create();

//    renderer = QgsGraduatedSymbolRendererV2::createRenderer(
//      qobject_cast<QgsVectorLayer *>(l), // QgsVectorLayer* vlayer,
//      target_field,                     // QString attrName,
//      10,                                // int classes,
//      QgsGraduatedSymbolRendererV2::EqualInterval, //Mode mode,
//      mySymbol4,        // QgsSymbolV2* symbol,
//      myColorRamp       // QgsVectorColorRampV2* ramp,)
//      //bool inverted = false,
//      //QgsRendererRangeV2LabelFormat legendFormat = QgsRendererRangeV2LabelFormat()
//      );
}
QgsVectorLayer* CMapWidget::AddContourDataLayer( const std::string &name, double m, double M, const QLookupTable *lut, size_t contours, QgsFeatureList &flist )
{
	static const QString target_field = height_key;

	static const QString base_name = "mem";
    static const QString provider = "memory";
 

	//return AddVectorLayer( name, R"-(L:\project\dev\source\data\empty\empty.shp)-", "ogr", renderer );


	//auto &crs = mapSettings().destinationCrs();

	//const QString s = mMainLayer->crs().toProj4();		//EPSG:4326
	const QString s = mapSettings().destinationCrs().toProj4();		//

	//"Point", "LineString", "Polygon", "MultiPoint", "MultiLineString", or "MultiPolygon".

	const QString prefix = "LineString";

	const QString layer_path =
        prefix + "?crs=EPSG:4326"

		+ QString( "&field=" ) + height_key + height_type
//		+ QString( "&field=" ) + ref_date_key + ref_date_type 

		+ "&index=yes";

	auto *l = AddVectorLayer( name, layer_path, provider, nullptr );

	Q_UNUSED( m ); Q_UNUSED( M ); Q_UNUSED( lut ); Q_UNUSED( contours );

	//auto *l = AddMemoryLayer( polygon, name, CreateRenderer( target_field, width, m, M, lut, contours, 
	//	polygon ?
	//	&CMapWidget::CreatePolygonSymbol :
	//	&CMapWidget::CreateLineSymbol
	//	) );

	if ( l )
	{
		l->dataProvider()->addFeatures( flist );
		mDataLayers.push_back( l );
	}

	return l;
}
*/

QgsVectorLayer* CMapWidget::AddDataLayer( const std::string &name, double symbol_width, double m, double M, const QLookupTable *lut, QgsFeatureList &flist,
	bool isdate, brathl_refDate date_ref )
{
	Q_UNUSED( m );	Q_UNUSED( M );	Q_UNUSED( lut );	Q_UNUSED( isdate );		Q_UNUSED( date_ref );

	QgsVectorLayer *l = CreateMemoryLayer( ePolygon, name, LayerDataFields(), 
		CreateRenderer( data_def.key, symbol_width, m, M, lut, &CMapWidget::CreatePolygonSymbol ), flist );
	//QgsVectorLayer *l = CreateMemoryLayer( ePoint, name, LayerDataFields(), 
	//	CreateRenderer( data_def.key, symbol_width, m, M, lut, &CreatePointSymbol ), flist );

	if ( isdate )
		l->setProperty( q2a( ref_date_def.key ).c_str(), date_ref );

	if ( l )
	{
		l->setLayerTransparency( data_layer_tranparency );
		mDataLayers.push_back( l );
		mCurrentDataLayer = l;
		mContourLayers.push_back( nullptr );
	}

	// Uncomment for writing layer to shape file
	//bool result = ExportLayer( "/home/brat/Downloads", l );	Q_UNUSED( result );
	//bool result = ExportLayer( "L:/layers", l );				Q_UNUSED( result );

	return AddVectorLayer( l );
}



QgsFeatureList& CreateCountourFeatures( QgsFeatureList &flist, const CMapPlotParameters &map, size_t ncontours )
{
	CListContour contours;

	//1. function

	contours.SetFieldFcn(
		[&map]( double x, double y ) -> double
	{
		return map.vvalue( x, y );
		//return map.nan_vvalue( x, y );
	} );

	//2. limits

	double pLimits[ 4 ] = { map.mMinX, map.mMaxX, map.mMinY, map.mMaxX };	//double pLimits[ 4 ] = { -180, 180, -90, 90 };
	contours.SetLimits( pLimits );

	//3. planes (contours)

	std::vector<double> vPlanes( ncontours );
	const auto vstep = ( map.mMaxHeightValue - map.mMinHeightValue ) / ncontours;
	for ( unsigned long i = 0; i < ncontours; i++ )
	{
		vPlanes[ i ] = map.mMaxHeightValue - i * vstep;		//vPlanes[ i ] = map.mMinHeightValue + i * vstep;
	}
	contours.SetPlanes( vPlanes );

	//4. grids

	const int factor = 5;
	contours.SetFirstGrid( factor * map.mXaxis.size(), factor * map.mYaxis.size() );
	contours.SetSecondaryGrid( factor * map.mXaxis.size(), factor * map.mYaxis.size() );
	contours.Generate();
	
	//5. results

	auto const nplanes = contours.GetNPlanes();									assert__( nplanes == ncontours );
	//const double r = 0.3;
	for ( size_t i = 0; i < nplanes; i++ )
	{
		auto pStripList = contours.GetLines( i );								assert__( pStripList );
		for ( auto pos=pStripList->begin(); pos != pStripList->end(); pos++ )
		{
			auto pStrip=( *pos );												assert__( pStrip );
			if ( pStrip->empty() )
				continue;

			QgsPolyline points;
			for ( auto pos2 = pStrip->begin(); pos2 != pStrip->end(); ++pos2 )
			{
				auto index = ( *pos2 );

				double x = contours.GetXi( index );
				double y = contours.GetYi( index );

				//double lon_max = x + default_data_step / 2;
				//double lon_min = x - default_data_step / 2;
				//double lat_min = y - default_data_step / 2;
				//double lat_max = y + default_data_step / 2;

				//x = lon_max < 180 ? x : 179.99;
				//x = lon_min > -180 ? x : -179.99;
				//y = lat_max < 90 ? lat_max : 89.99;
				//y = lat_min > -90 ? lat_min : -89.99;

				if ( x > -179.99 && y > -89.99 && x < 179.99 && y < 89.99 )
				//if ( x > map.mMinX && y > map.mMinY && x < map.mMaxX && y < map.mMaxY )
				//if ( true )
				//if ( x != std::numeric_limits<int>::max() && y != std::numeric_limits<int>::max() )
				//if ( true)
				{
					//if ( x < 0 )
					//	x -= ( default_data_step + fmod( x, default_data_step ) );
					//else
					//	x += fmod( x, default_data_step );

					//if ( y < 0 )
					//	y -= ( default_data_step + fmod( y, default_data_step ) );
					//else
					//	y += fmod( y, default_data_step );

					//glVertex2f((GLfloat)(pLimits[0]+x),(GLfloat)(pLimits[2]+y));
					points.append( QgsPoint( x, y ) );
					//points.append( QgsPoint(  std::round( x / r ) * r, std::round( y / r ) * r ) );
					//points.append( QgsPoint(  x / default_data_step * default_data_step, y / default_data_step * default_data_step ) );
				}
				else
					LOG_TRACEstd( "Point discarded " + n2s<std::string>(x) + ", " + n2s<std::string>(y) );
			}
			if ( points.size() == 1 )
			{
				//points.append( QgsPoint( std::max( points[ 0 ].x() - 1, xmin ), std::min( points[ 0 ].y() + 1, ymax ) ) );
				//points.append( QgsPoint( std::min( points[ 0 ].x() + 1, xmax ), std::max( points[ 0 ].y() - 1, ymin ) ) );
			}
			if ( points.size() > 1 )
				CreateLineDataFeature( flist, map.mMaxHeightValue - i * vstep, points );
		}
	}
	return flist;
}

//	Assumes that a corresponding data layer was already inserted
//
QgsVectorLayer* CMapWidget::AddContourLayer( size_t index, const std::string &name, unsigned width, QColor color, size_t ncontours, const CMapPlotParameters &map, const QLookupTable *lut )	//lut = nullptr  
{
	Q_UNUSED( lut );	Q_UNUSED( color );		// TODO while using lut is not decided

	QgsFeatureList flist;

	QgsVectorLayer *l = CreateMemoryLayer( eLineString, name, LayerDataFields(), 
		//CreateRenderer( data_def.key, 0.1, map.mMinHeightValue, map.mMaxHeightValue, lut, &CMapWidget::CreateLineSymbol ),
		CreateRenderer( CreateLineSymbol( width, color ) ), 
		CreateCountourFeatures( flist, map, ncontours ) );

	if ( l )
	{
		assert__( index < mContourLayers.size() );
		mContourLayers[ index ] = l;
	}

	return AddVectorLayer( l );
}


bool CMapWidget::IsArrowLayer( size_t index ) const
{
	assert__( index < mDataLayers.size() );

	return mDataLayers[ index ]->property( q2a( magnitude_def.key ).c_str() ) == magnitude_def.key;
}


QgsVectorLayer* CMapWidget::AddArrowDataLayer( const std::string &name, QgsFeatureList &flist )
{
	QgsVectorLayer *l = CreateMemoryLayer( ePoint, name, LayerVectorFields(), 
		CreateRenderer( CreateArrowSymbol( "black" ) ), flist );

	if ( l )
	{
		l->dataProvider()->addFeatures( flist );
		l->setProperty( q2a( magnitude_def.key ).c_str(), magnitude_def.key );
		mDataLayers.push_back( l );
        mContourLayers.push_back( nullptr );
    }

	// Uncomment for writing layer to shape file
	//bool result = ExportLayer( "/home/brat/Downloads", l, QgsVectorFileWriter::SymbolLayerSymbology );	Q_UNUSED( result );
	//bool result = ExportLayer( "L:/layers", l, QgsVectorFileWriter::SymbolLayerSymbology );				Q_UNUSED( result );

	return AddVectorLayer( l );
}


void CMapWidget::PlotTrack( const double *x, const double *y, const double *z, size_t size, brathl_refDate ref_date, QColor color )		//color = Qt::red 
{
	QgsFeatureList flist;
	for ( auto i = 0u; i < size; ++ i )
	{
		CreatePointTrackFeature( flist, x[ i ], y[ i ], ref_date, z[ i ] );
	}

	if ( mTracksLayer == nullptr )
	{
		mTracksLayer = AddVectorLayer( 
			CreateMemoryLayer( ePoint, "SatelliteTrack", LayerTimeFields(), CreateRenderer( CreatePointSymbol( track_symbol_width, color ) ), flist ) );
	}
	else
	{
		mTracksLayer->dataProvider()->addFeatures( flist );		
		mTracksLayer->updateExtents();
	}

	mTracksLayer->setProperty( q2a( ref_date_def.key ).c_str(), ref_date );	//for correction, because so far it is always ReadData ref date: REF19500101
}


void CMapWidget::RemoveTracksLayerFeatures()
{
	assert__( mTracksLayer != nullptr );

	RemoveFeatures( mTracksLayer );
}






////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		User Interaction
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		Layers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void CMapWidget::SetCurrentLayer( QgsMapLayer *l )
{
	LOG_TRACEstd( "Layer CRS = " + n2s<std::string>( l->crs().srsid() ) );
    setCurrentLayer( l );
	QgsVectorLayer *vl = qobject_cast< QgsVectorLayer* >( l );
	if ( vl )
		connect( vl, SIGNAL( selectionChanged() ), this, SIGNAL( CurrentLayerSelectionChanged() ), Qt::QueuedConnection );
}



template <typename T>
QList<T> reversed( const QList<T> & in ) 
{
	QList<T> result;
	result.reserve( in.size() ); // reserve is new in Qt 4.7
	std::reverse_copy( in.begin(), in.end(), std::back_inserter( result ) );
	return result;
}


void CMapWidget::SetLayerSet()
{
	QList < QgsMapCanvasLayer > list = mLayerSet;

	std::reverse( list.begin(), list.end() );

	setLayerSet( list );
}


QgsMapCanvasLayer* CMapWidget::FindCanvasLayer( QgsMapLayer *layer )
{
	for ( auto &cl : mLayerSet )
		if ( cl.layer() == layer )
			return &cl;

	return nullptr;
}


bool CMapWidget::IsLayerVisible( QgsMapLayer *layer ) const
{
	const QgsMapCanvasLayer *l = FindCanvasLayer( layer );
	if ( l )
		return l->isVisible();

	return false;
}
bool CMapWidget::SetLayerVisible( QgsVectorLayer *layer, bool show, bool render )
{
	QgsMapCanvasLayer *l = FindCanvasLayer( layer );
	if ( !l )
		return false;

	if ( show != l->isVisible() )
	{
		l->setVisible( show );
		SetLayerSet();
		SetCurrentLayer( l->layer() );
		if ( render )
			refresh();
	}

	return true;
}


bool CMapWidget::HasContourLayer( size_t index ) const
{
	assert__( index < mContourLayers.size() );

	return mContourLayers[ index ];
}


bool CMapWidget::IsDataLayerVisible( size_t index ) const
{
	assert__( index < mDataLayers.size() );

	return IsLayerVisible( mDataLayers[ index ] );
}
bool CMapWidget::IsContourLayerVisible( size_t index ) const
{
    assert__( index < mContourLayers.size() );

    return mContourLayers[ index ] && IsLayerVisible( mContourLayers[ index ] );
}

bool CMapWidget::SetDataLayerVisible( size_t index, bool show, bool render )
{
	assert__( index < mDataLayers.size() );

	if ( !SetLayerVisible( mDataLayers[ index ], show, render ) )
		return false;

	mCurrentDataLayer = nullptr;
	const size_t size = mDataLayers.size();
	for ( size_t i = 0; i < size; ++i )
		if ( IsDataLayerVisible( i ) )
			mCurrentDataLayer = mDataLayers[ i ];		//last added that is visible

	return true;
}
bool CMapWidget::SetContourLayerVisible( size_t index, bool show, bool render )
{
	assert__( index < mContourLayers.size() );

	return SetLayerVisible( mContourLayers[ index ], show, render );
}


void CMapWidget::SetMainLayerVisible( bool show )
{
	QgsMapCanvasLayer *cl = FindCanvasLayer( mMainLayer );
	if ( show != cl->isVisible() )
	{
		cl->setVisible( show );
		SetLayerSet();
		refresh();
	}
}


void CMapWidget::ChangeDataRenderer( size_t index, double width, double m, double M, const QLookupTable *lut )
{
	assert__( index < mDataLayers.size() );

	stopRendering();
	freeze( true );
	while ( isDrawing() )
		qApp->processEvents();

	ChangeRenderer( mDataLayers[ index ], data_def.key, width, m, M, lut, &CMapWidget::CreatePolygonSymbol );

	freeze( false );
	refresh();
}



void CMapWidget::SetNumberOfContours( size_t index, size_t ncontours, const CMapPlotParameters &map )
{
	assert__( index < mContourLayers.size() );

	QgsVectorLayer *l = mContourLayers[ index ];

	RemoveFeatures( l );

	QgsFeatureList flist;
	CreateCountourFeatures( flist, map, ncontours );
	if ( l && !flist.empty() )
	{
		l->dataProvider()->addFeatures( flist );
		l->updateExtents();
	}
}

void CMapWidget::SetContoursProperties( size_t index, QColor color, unsigned width )
{
	assert__( index < mContourLayers.size() );

	QgsVectorLayer *l = mContourLayers[ index ];			assert__( l );

	l->setRendererV2( CreateRenderer( CreateLineSymbol( width, color ) ) );

	refresh();
}



// don't (un)assign mCurrentXXX pointers here
//
void CMapWidget::RemoveLayers( bool render )		//render = false 
{
	const int count = mLayerSet.size();

	while( mLayerSet.size() > 1 )
		mLayerSet.removeLast();

	mDataLayers.clear();
	mContourLayers.clear();

	if ( count > 1 )
	{
		SetLayerSet();
		SetCurrentLayer( mLayerSet.begin()->layer() );
		if ( render )
			refresh();
	}
}

void CMapWidget::RemoveLayer( QgsMapLayer *layer, bool render )		//render = false 
{
	if ( !layer )
		return;

	if ( layer == mCurrentDataLayer )
		mCurrentDataLayer = nullptr;

	for ( auto &l : mDataLayers )
		if ( l == layer )
			mDataLayers.erase( std::find( mDataLayers.begin(), mDataLayers.end(), layer ) );

	for ( auto &l : mContourLayers )
		if ( l == layer )
			mContourLayers.erase( std::find( mContourLayers.begin(), mContourLayers.end(), layer ) );

	QList <QgsMapCanvasLayer> set = mLayerSet;
	RemoveLayers( false );
	for ( auto &cl : set )
		if ( cl.layer() != layer )
			mLayerSet.append( cl );

	QgsMapLayerRegistry::instance()->removeMapLayer( layer->id() );

	SetLayerSet();
	SetCurrentLayer( mLayerSet.begin()->layer() );		//deletes layer;

	if ( render )
		refresh();
}
void CMapWidget::RemoveTracksLayer()
{
	RemoveLayer( mTracksLayer, false );
	mTracksLayer = nullptr;

	refresh();
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
	setContextMenuPolicy( enable ? Qt::PreventContextMenu : Qt::ActionsContextMenu );
}																		 	
void CMapWidget::EnablePolygonalSelection( bool enable )
{
	assert__( mSelectPolygon );

	enable ? setMapTool( mSelectPolygon ) : unsetMapTool( mSelectPolygon );
	setContextMenuPolicy( enable ? Qt::PreventContextMenu : Qt::ActionsContextMenu );
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

QgsRectangle CMapWidget::CurrentLayerSelectedFeaturesBox() const
{
	QgsRectangle box;
	QgsVectorLayer *l = QgsMapToolSelectUtils::getCurrentVectorLayer( const_cast<CMapWidget*>( this ) );
	if ( l )
		box = l->boundingBoxOfSelected();

	return box;
}

void CMapWidget::SelectArea( double lonm, double lonM, double latm, double latM )
{
	if ( mSelectFeatures )
		mSelectFeatures->SetRubberBandSelection( lonm, lonM, latm, latM );
}
void CMapWidget::RemoveAreaSelection()
{
	if ( mSelectFeatures )
		mSelectFeatures->RemoveRubberBandSelection();
}


//create and insert widgets/actions

void CMapWidget::ConnectParentSelectionActions( QToolButton *selection_button, QAction *action_select_features, QAction *action_select_polygon, QAction *action_deselect_all )
{
	connect( action_select_features, SIGNAL( toggled( bool ) ), this, SLOT( EnableRectangularSelection( bool ) ) );
	mSelectFeatures = new CMapToolSelectFeatures( this );
	mSelectFeatures->setAction( action_select_features );
	connect( mSelectFeatures, SIGNAL( NewRubberBandSelection( QRectF ) ), this, SIGNAL( NewRubberBandSelection( QRectF ) ), Qt::QueuedConnection );

	if ( action_select_polygon )
	{
		connect( action_select_polygon, SIGNAL( toggled( bool ) ), this, SLOT( EnablePolygonalSelection( bool ) ) );
		mSelectPolygon = new CMapToolSelectPolygon( this );
		mSelectPolygon->setAction( action_select_polygon );
	}

	connect( action_deselect_all, SIGNAL( triggered() ), this, SLOT( DeselectAll() ) );

	connect( selection_button, SIGNAL( triggered( QAction * ) ), this, SLOT( ToolButtonTriggered( QAction * ) ) );
}


//static 
QToolButton* CMapWidget::CreateMapSelectionActions( QToolBar *tb, QAction *&action_select_features, QAction *&action_deselect_all )
{
    action_select_features = CActionInfo::CreateAction( tb, eAction_SelectFeatures );
	action_deselect_all = CActionInfo::CreateAction( tb, eAction_DeselectAll );

	QToolButton *selection = new QToolButton( tb );
	selection->addAction( action_select_features );
	selection->setDefaultAction( action_select_features );

	return selection;
}

//static 
QToolButton* CMapWidget::AddMapSelectionPolygon( QToolButton *selection, QToolBar *tb, QAction *&action_select_polygon )
{
	action_select_polygon = CActionInfo::CreateAction( tb, eAction_SelectPolygon );

	selection->setPopupMode( QToolButton::MenuButtonPopup );
	selection->addAction( action_select_polygon );

	return selection;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		Measure Tool
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//slots

void CMapWidget::MeasureDistance( bool enable )
{
	enable ? setMapTool( mMeasureDistance ) : unsetMapTool( mMeasureDistance );
	setContextMenuPolicy( enable ? Qt::PreventContextMenu : Qt::ActionsContextMenu );
}

void CMapWidget::MeasureArea( bool enable )
{
	enable ? setMapTool( mMeasureArea ) : unsetMapTool( mMeasureArea );
	setContextMenuPolicy( enable ? Qt::PreventContextMenu : Qt::ActionsContextMenu );
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






////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		Map Tips (see also ShowMouseCoordinate)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void CMapWidget::ToggleMapTips( bool checked )
{
	mMapTipsVisible = checked;

	if ( !mMapTipsVisible )
	{
		mpMapTipsTimer.stop();
	}
}


// The mpMapTipsTimer is reset everytime the mouse is moved
//
void CMapWidget::ConnectParentMapTipsAction( QAction *action_map_tips )
{
	mActionMapTips = action_map_tips;

	mMapTipsVisible = mActionMapTips->isChecked(); 

	connect( mActionMapTips, SIGNAL( toggled( bool ) ), this, SLOT( ToggleMapTips( bool ) ) );


	// connect the timer to the map tips slot
	connect( &mpMapTipsTimer, SIGNAL( timeout() ), this, SLOT( ShowMapTip() ) );

	// set the interval to 0.850 seconds - timer will be started next time the mouse moves
	mpMapTipsTimer.setInterval( 850 );

	// Create the map tips object
	mpMaptip = new CMapTip;
}


//static 
QAction* CMapWidget::CreateMapTipsAction( QToolBar *tb )
{
	QAction *map_tips = CActionInfo::CreateAction( tb, eAction_MapTips );
	map_tips->setChecked( false );
	return map_tips;
}


// Show the map tip using tool-tip (SLOT)
void CMapWidget::ShowMapTip()
{
	// Stop the timer while we look for a map tip
	mpMapTipsTimer.stop();

	// Only show tool-tip if the mouse is over the canvas
	if ( this->underMouse() )
	{
		QPoint myPointerPos = mouseLastXY();

		QgsMapLayer* layer = mTracksLayer ? mTracksLayer : ( mCurrentDataLayer ? mCurrentDataLayer : currentLayer() );
		if ( layer && IsLayerVisible( layer) )
		{
			// only process vector layers
			if ( layer->type() == QgsMapLayer::VectorLayer )
			{
				// Show the map tip if the map tips button is depressed
				if ( mMapTipsVisible )
				{
					mpMaptip->ShowMapTip( layer, mLastMapPosition, myPointerPos, this );
				}
			}
		}
		else
		{
			LOG_WARN( "Map tips require an active layer" );
		}
	}
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		Map Coordinates
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void CMapWidget::CoordinatesFormatChanged()
{
	QAction *action = qobject_cast<QAction*>( sender() );				assert__( action );

	mCoordinatesFormat = action->text();
}


void CMapWidget::ShowMouseDegreeCoordinates( const QgsPoint &geo, bool erase )			//erase = false 
{
    if ( erase )
	{
		mCoordsEdit->clear();
	}
	else
	{
		if ( mCoordinatesFormat == "DM" )
		{
			mCoordsEdit->setText( geo.toDegreesMinutes( mMousePrecisionDecimalPlaces ) );
		}
		else if ( mCoordinatesFormat == "DMS" )
		{
			mCoordsEdit->setText( geo.toDegreesMinutesSeconds( mMousePrecisionDecimalPlaces ) );
		}
		else
		{
			mCoordsEdit->setText( geo.toString( mMousePrecisionDecimalPlaces ) );
		}
	}
}


void CMapWidget::ShowMouseCoordinate( const QString s, bool erase )			//erase = false 
{
	if ( erase )
	{
		mCoordsEdit->clear();
	}
	else
	{
		mCoordsEdit->setText( s );
	}
}


void CMapWidget::ShowMouseCoordinate( const QgsPoint &p, bool erase )		//erase = false 
{
	if ( erase )
	{
		mCoordsEdit->clear();
		return;
	}

    // Map Tips /////////////////////////////////////////////////////////
    if ( mMapTipsVisible && ( mTracksLayer || !mDataLayers.empty() ) )
    {
      // store the point, we need it for when the map tips timer fires
      mLastMapPosition = p;

      // we use this slot to control the timer for map tips since it is fired each time
      // the mouse moves.
      if ( this->underMouse() )
      {
        // Clear the map tip (this is done conditionally)
        mpMaptip->Clear( this );
        // don't start the timer if the mouse is not over the map canvas
        mpMapTipsTimer.start();
        //QgsDebugMsg("Started map tips timer");
      }
    }
    /////////////////////////////////////////////////////////////////////


    QGis::UnitType unit = mapUnits();

	mActionDM->setEnabled( unit == QGis::Degrees );
	if ( !mActionDM->isEnabled() )
		mActionDM->setChecked( false );

	mActionDMS->setEnabled( unit == QGis::Degrees );
	if ( !mActionDMS->isEnabled() )
		mActionDMS->setChecked( false );

	mActionDecimal->setEnabled( unit != QGis::UnknownUnit );
	if ( !mActionDecimal->isEnabled() )
		mActionDecimal->setChecked( false );
	else
		mActionDecimal->setChecked( unit != QGis::Degrees );

	if ( unit == QGis::UnknownUnit )
		return;

	try {
		QgsPoint geo = p;
		if ( unit == QGis::Degrees )
		{
			if ( !mapSettings().destinationCrs().isValid() )
				return;

			if ( !mapSettings().destinationCrs().geographicFlag() )
			{
				QgsCoordinateTransform ct( mapSettings().destinationCrs(), QgsCoordinateReferenceSystem( GEOSRID ) );
				geo = ct.transform( p );
			}

			ShowMouseDegreeCoordinates( geo );
		}
		else
		{
			mCoordsEdit->setText( geo.toString( mMousePrecisionDecimalPlaces ) );
		}

#if defined(DEBUG) || defined(_DEBUG)
		QgsRectangle rect( QgsPoint( geo.x() - 1, geo.y() - 1 ), QgsPoint( geo.x() + 1, geo.y() + 1 ) );		//TODO check this "around point" rectangle
		DebugWriteTrackValue( rect );
		DebugWriteDataValue( rect );
#endif

		if ( mCoordsEdit->width() > mCoordsEdit->minimumWidth() )
		{
			mCoordsEdit->setMinimumWidth( mCoordsEdit->width() );
		}
	}
	catch ( ... )
	{
		mCoordsEdit->clear();
	}
}


void CMapWidget::UpdateMouseCoordinatePrecision()
{
	// Work out what mouse display precision to use. This only needs to
	// be when the zoom level changes.

	// Work out a suitable number of decimal places for the mouse
	// coordinates with the aim of always having enough decimal places
	// to show the difference in position between adjacent pixels.
	// Also avoid taking the log of 0.

	int dp = 0;							//do not compute with an unsigned
	if ( mapUnitsPerPixel() != 0.0 )
		dp = static_cast<int>( ceil( -1.0 * log10( mapUnitsPerPixel() ) ) );

	// Keep mMousePrecisionDecimalPlaces sensible
	if ( dp < 0 )
		dp = 0;
    if ( dp > 20 )
        dp = 20;

    mMousePrecisionDecimalPlaces = dp;			assert__( mMousePrecisionDecimalPlaces <= 20 );
}


//static 
void CMapWidget::CreateCoordinatesWidget( QStatusBar *bar, QLineEdit *&coords, QToolButton *&format_button )
{
	QFont myFont( "Arial", 9 );

	//coords status bar widget
	coords = new QLineEdit( QString(), bar );
	coords->setObjectName( "coords" );
	coords->setFont( myFont );
	coords->setMinimumWidth( 10 );
	coords->setMaximumWidth( 400 );
	coords->setMaximumHeight( 20 );
	coords->setContentsMargins( 0, 0, 0, 0 );
	coords->setAlignment( Qt::AlignCenter );
	SetReadOnlyEditor( coords, true );

	// Coordinates format possible values: "DM", "DMS", "Decimal"
	//
	QAction *actionDM = new QAction( "DM", nullptr );
	QAction *actionDMS = new QAction( "DMS", nullptr  );
	QAction *actionDecimal = new QAction( "Decimal", nullptr  );
	auto *group = CreateActionGroup( nullptr, { actionDM, actionDMS, actionDecimal }, true );
	format_button = CActionInfo::CreateMenuButton( eAction_MouseTrackingeCoordinatesFormat, group->actions() );
	format_button->setObjectName( "format" );
    format_button->setMaximumWidth( 40 );
    format_button->setMaximumHeight( 20 );
}


void CMapWidget::ConnectCoordinatesWidget( QLineEdit *&coords, QToolButton *format_button, const QString &format )	//format = "Decimal" 
{
	mCoordsEdit = coords;
	mCoordinatesFormatButton = format_button;
	auto list = mCoordinatesFormatButton->menu()->actions();
	mCoordinatesFormat = format;
	bool valid_format = false;
	for ( auto *a : list )
	{
		if ( format == a->text() )
		{
			a->setChecked( true );
			valid_format = true;
			mCoordinatesFormat = a->text();
		}
		connect( a, SIGNAL( triggered() ), this, SLOT( CoordinatesFormatChanged() ) );
	}
	if ( !valid_format )
	{
		auto *a = list[ 0 ];
		mCoordinatesFormat = a->text();
		a->setChecked( true );
	}
	mActionDM = list[ 0 ];			assert__( mActionDM->text() == "DM" );
	mActionDMS = list[ 1 ];			assert__( mActionDMS->text() == "DMS" );
	mActionDecimal = list[ 2 ];		assert__( mActionDecimal->text() == "Decimal" );


	connect( this, SIGNAL( xyCoordinates( const QgsPoint & ) ), this, SLOT( ShowMouseCoordinate( const QgsPoint & ) ) );
	connect( this, SIGNAL( scaleChanged( double ) ), this, SLOT( UpdateMouseCoordinatePrecision() ) );
}




//////////////////////////////////////////////////////////////////////////////////////////////////


void CMapWidget::DebugWriteTrackValue( QgsRectangle rect )
{
	if ( !mTracksLayer || isDrawing() )
		return;

	auto *provider = mTracksLayer->dataProvider();
	QgsFeatureRequest req;
	req.setFilterRect( rect );
	QgsFeatureIterator it = provider->getFeatures( req );
	QgsFeature f;
	if ( it.nextFeature( f ) )
	{
		auto v = f.attribute( data_def.key );
		auto rd = f.attribute( ref_date_def.key );
		if ( v.isValid() && rd.isValid() )
		{
			bool ok;
			brathl_refDate brd = (brathl_refDate)rd.toInt( &ok );
			if ( ok )
			{
				CDate d( v.toDouble( &ok ), brd );
				if ( ok && !d.IsDefaultValue() )
				{
					mCoordsEdit->setText( mCoordsEdit->text() + " - " + d.AsString().c_str() );
				}
			}
		}
	}
}
void CMapWidget::DebugWriteDataValue( QgsRectangle rect )
{
	if ( mDataLayers.empty() || isDrawing() )
		return;

	for ( auto *l : mDataLayers )
	{
		if ( !IsLayerVisible( l ) )
			continue;

		auto *provider = l->dataProvider();
		QgsFeatureRequest req;
		req.setFilterRect( rect );
		QgsFeatureIterator it = provider->getFeatures( req );
		QgsFeature f;
        if ( it.nextFeature( f ) && f.attributes().size() > 0 )
		{
			//TODO how do we know its a time variable?
			//brathl_refDate ref_date = (brathl_refDate)f.attribute( ref_date_key ).toInt();	CDate d( f.attribute( height_key ).toDouble(), ref_date );
			auto v = f.attribute( data_def.key );
			if ( v.isValid() )
			{
				bool ok;
				double d = v.toDouble( &ok );
				if ( ok && !isDefaultValue(d) )
					mCoordsEdit->setText( mCoordsEdit->text() + " - " + v.toString() );
			}
		}
	}
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_MapWidget.cpp"
