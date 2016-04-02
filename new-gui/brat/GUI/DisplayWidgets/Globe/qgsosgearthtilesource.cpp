#include "new-gui/brat/stdafx.h"

#if defined (WIN32) || defined(_WIN32)
#pragma warning ( disable: 4100 )           //unreferenced formal parameter
#endif

#include <osgEarth/TileSource>
#include <osgEarth/Registry>
#include <osgEarth/ImageUtils>

#include "qgsosgearthtilesource.h"		//must come before the ifdef just below

#ifdef USE_RENDERER
#else

#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

#include <qgsmaprenderersequentialjob.h>

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

#endif  // USE_RENDERER


#if defined (WIN32) || defined(_WIN32)
#pragma warning ( default: 4100 )           //unreferenced formal parameter
#endif


#if defined (__unix__)
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif



using namespace osgEarth;
using namespace osgEarth::Drivers;


QgsOsgEarthTileSource::QgsOsgEarthTileSource( QgsMapCanvas* theCanvas, const TileSourceOptions& options )	//options = TileSourceOptions()
	: TileSource( options )
	, mCanvas( theCanvas )
	, mCoordTransform( 0 )
#ifdef USE_RENDERER
	, mMapRenderer( 0 )
#endif
{
	setProfile( osgEarth::Registry::instance()->getGlobalGeodeticProfile() );

	QgsCoordinateReferenceSystem destCRS;
	destCRS.createFromOgcWmsCrs( GEO_EPSG_CRS_AUTHID );

	if ( mCanvas->mapSettings().destinationCrs().authid().compare( GEO_EPSG_CRS_AUTHID, Qt::CaseInsensitive ) != 0 )
	{
		// FIXME: crs from canvas or first layer?
		QgsCoordinateReferenceSystem srcCRS( mCanvas->mapSettings().destinationCrs() );
		QgsDebugMsg( QString( "transforming from %1 to %2" ).arg( srcCRS.authid() ).arg( destCRS.authid() ) );
		mCoordTransform = new QgsCoordinateTransform( srcCRS, destCRS );
	}
	else
	{
		mCoordTransform = 0;
	}

#ifdef USE_RENDERER
	mMapRenderer = new QgsMapRenderer();
	mMapRenderer->setDestinationCrs( destCRS );
	mMapRenderer->setProjectionsEnabled( true );
	mMapRenderer->setOutputUnits( mCanvas->mapRenderer()->outputUnits() );
	mMapRenderer->setMapUnits( QGis::Degrees );
#else
	mMapSettings.setDestinationCrs( destCRS );
	mMapSettings.setCrsTransformEnabled( true );
	mMapSettings.setMapUnits( QGis::Degrees );
#endif
}



//can be called from another thread
//
osg::Image* QgsOsgEarthTileSource::createImage( const TileKey& key, ProgressCallback* progress )
{
	//while ( mCanvas->isDrawing() )
	//	qApp->processEvents();

	if ( mStop || mCanvas->isDrawing() )
		return nullptr;

	QString kname = key.str().c_str();
	kname.replace( '/', '_' );

	Q_UNUSED( progress );

	//Get the extents of the tile
	int tileSize = getPixelsPerTile();
	if ( tileSize <= 0 )
	{
		QgsDebugMsg( "Tile size too small." );
		return ImageUtils::createEmptyImage();
	}

	mRendering = true;

	QgsRectangle viewExtent = mCanvas->fullExtent();
	if ( mCoordTransform )
	{
		QgsDebugMsg( QString( "vext0:%1" ).arg( viewExtent.toString( 5 ) ) );
		viewExtent = mCoordTransform->transformBoundingBox( viewExtent );
	}

	QgsDebugMsg( QString( "vext1:%1" ).arg( viewExtent.toString( 5 ) ) );

	double xmin, ymin, xmax, ymax;
	key.getExtent().getBounds( xmin, ymin, xmax, ymax );
	QgsRectangle tileExtent( xmin, ymin, xmax, ymax );

	QgsDebugMsg( QString( "text0:%1" ).arg( tileExtent.toString( 5 ) ) );
	if ( !viewExtent.intersects( tileExtent ) )
	{
		QgsDebugMsg( QString( "earth tile key:%1 ext:%2: NO INTERSECT" ).arg( kname ).arg( tileExtent.toString( 5 ) ) );
		mRendering = false;
		return ImageUtils::createEmptyImage();
	}

#ifdef USE_RENDERER

	QImage *qImage = createQImage( tileSize, tileSize );
	if ( !qImage )
	{
		QgsDebugMsg( QString( "earth tile key:%1 ext:%2: EMPTY IMAGE" ).arg( kname ).arg( tileExtent.toString( 5 ) ) );
		mRendering = false;
		return ImageUtils::createEmptyImage();
	}

	mMapRenderer->setLayerSet( mCanvas->mapRenderer()->layerSet() );
	mMapRenderer->setOutputSize( QSize( tileSize, tileSize ), qImage->logicalDpiX() );
	mMapRenderer->setExtent( tileExtent );

	QPainter thePainter( qImage );
	mMapRenderer->render( &thePainter );

#else

	mMapSettings.setLayers( mCanvas->mapSettings().layers() );
	mMapSettings.setOutputSize( QSize( tileSize, tileSize ) );
	mMapSettings.setOutputDpi( QgsApplication::desktop()->logicalDpiX() );
	mMapSettings.setExtent( tileExtent );
	//mMapSettings.setBackgroundColor( QColor( 0, 0, 0, 0 ) );		//femm commented out, and it seems correct

	QgsMapRendererSequentialJob job( mMapSettings );
	job.start();
	job.waitForFinished();

	QImage *qImage = new QImage( job.renderedImage() );
	if ( !qImage )
	{
		QgsDebugMsg( QString( "earth tile key:%1 ext:%2: EMPTY IMAGE" ).arg( kname ).arg( tileExtent.toString( 5 ) ) );
		mRendering = false;
		return ImageUtils::createEmptyImage();
	}

	Q_ASSERT( qImage->logicalDpiX() == QgsApplication::desktop()->logicalDpiX() );
	Q_ASSERT( qImage->format() == QImage::Format_ARGB32_Premultiplied );

#endif		//USE_RENDERER

	QgsDebugMsg( QString( "earth tile key:%1 ext:%2" ).arg( kname ).arg( tileExtent.toString( 5 ) ) );
#if 0
	qImage->save( QString( "/tmp/tile-%1.png" ).arg( kname ) );
#endif

	osg::ref_ptr<osg::Image> image = new osg::Image;

	//The pixel format is always RGBA to support transparency
	image->setImage( tileSize, tileSize, 1, 4, // width, height, depth, pixelFormat?
		GL_BGRA, GL_UNSIGNED_BYTE, //Why not GL_RGBA - Qt bug?
		qImage->bits(),
		osg::Image::NO_DELETE, 1 );

	image->flipVertical();

	mRendering = false;

	//Create a transparent image if we don't have an image
	if ( !image.valid() )
	{
		QgsDebugMsg( "image is invalid" );
		return ImageUtils::createEmptyImage();
	}

	QgsDebugMsg( "returning image" );
	return image.release();
}



#ifdef USE_RENDERER

QImage* QgsOsgEarthTileSource::createQImage( int width, int height ) const
{
	if ( width < 0 || height < 0 )
		return 0;

	QImage *qImage = 0;

	//is format jpeg?
	bool jpeg = false;
	//transparent parameter
	bool transparent = true;

	//use alpha channel only if necessary because it slows down performance
	if ( transparent && !jpeg )
	{
		qImage = new QImage( width, height, QImage::Format_ARGB32_Premultiplied );
		qImage->fill( 0 );
	}
	else
	{
		qImage = new QImage( width, height, QImage::Format_RGB32 );
		qImage->fill( qRgb( 255, 255, 255 ) );
	}

	if ( !qImage )
		return 0;

	//apply DPI parameter if present.
#if 0
	int dpm = dpi / 0.0254;
	qImage->setDotsPerMeterX( dpm );
	qImage->setDotsPerMeterY( dpm );
#endif
	return qImage;
}

#endif		//USE_RENDERER
