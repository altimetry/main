/***************************************************************************
    qgsmaptoolselectrectangle.cpp  -  map tool for selecting features by
                                   rectangle
    ----------------------
    begin                : January 2006
    copyright            : (C) 2006 by Martin Dobias
    email                : wonder.sk at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "new-gui/brat/stdafx.h"

#include <QMouseEvent>
#include <QRect>

#include <qgsrubberband.h>
#include <qgsmapcanvas.h>
#include <qgsmaptopixel.h>
#include <qgsvectorlayer.h>
#include <qgscursors.h>
#include <qgsgeometry.h>
#include <qgspoint.h>
#include <qgis.h>

#include "MapToolSelectUtils.h"
#include "MapToolSelectRectangle.h"


#define TEST_SELECTION

CMapToolSelectFeatures::CMapToolSelectFeatures( QgsMapCanvas* canvas )
	: QgsMapTool( canvas )
	, mDragging( false )
{
	mToolName = tr( "Select features" );
	QPixmap mySelectQPixmap = QPixmap( (const char **)select_cursor );
	mCursor = QCursor( mySelectQPixmap, 1, 1 );
	mRubberBand = nullptr;
	mFillColor = QColor( 254, 178, 76, 63 );
	mBorderColour = QColor( 254, 58, 29, 100 );
}


void CMapToolSelectFeatures::CreateRubberBand()
{
	DeleteRubberBand();
	mSelectRect.setRect( 0, 0, 0, 0 );
	mRubberBand = new QgsRubberBand( mCanvas, QGis::Polygon );
	mRubberBand->setFillColor( mFillColor );
	mRubberBand->setBorderColor( mBorderColour );
}

void CMapToolSelectFeatures::DeleteRubberBand()
{
	if (mRubberBand )
	{
		mRubberBand->reset( QGis::Polygon );
		delete mRubberBand;
		mRubberBand = nullptr;
		mDragging = false;
	}

#if defined(TEST_SELECTION)

	QgsVectorLayer* vlayer = QgsMapToolSelectUtils::getCurrentVectorLayer( mCanvas );
	if ( vlayer == nullptr )
	{
		return;
	}
	QgsFeatureIds newSelectedFeatures;
	vlayer->setSelectedFeatures( newSelectedFeatures );

#endif
}


void CMapToolSelectFeatures::canvasPressEvent( QMouseEvent *e )
{
	Q_UNUSED( e );

#if defined(TEST_SELECTION)

	DeleteRubberBand();
	emit NewRubberBandSelection( QRectF() );

#endif

	CreateRubberBand();
}


void CMapToolSelectFeatures::canvasMoveEvent( QMouseEvent *e )
{
	if ( e->buttons() != Qt::LeftButton )
		return;

	if ( !mDragging )
	{
		mDragging = true;
		mSelectRect.setTopLeft( e->pos() );
	}
	mSelectRect.setBottomRight( e->pos() );
	QgsMapToolSelectUtils::setRubberBand( mCanvas, mSelectRect, mRubberBand );
}


void CMapToolSelectFeatures::SetRubberBandSelection( double lonm, double lonM, double latm, double latM )
{
	CreateRubberBand();

	const QgsMapToPixel* t = mCanvas->getCoordinateTransform();

	QgsPoint p0 = t->transform( lonm, latm );
	QgsPoint p1 = t->transform( lonM, latM );

	mSelectRect = QRect( p0.toQPointF().toPoint(), p1.toQPointF().toPoint() );

	QgsMapToolSelectUtils::setRubberBand( mCanvas, mSelectRect, mRubberBand );
	QgsGeometry* selectGeom = mRubberBand->asGeometry();
	QgsMapToolSelectUtils::setSelectFeatures( mCanvas, selectGeom );
}


void CMapToolSelectFeatures::RemoveRubberBandSelection()
{
	DeleteRubberBand();
}



void CMapToolSelectFeatures::canvasReleaseEvent( QMouseEvent *e )
{
	QgsVectorLayer* vlayer = QgsMapToolSelectUtils::getCurrentVectorLayer( mCanvas );

#if defined(TEST_SELECTION)
	if ( mSelectRect.width() == 0 || mSelectRect.height() == 0 )
		vlayer = nullptr;
#endif

	if ( vlayer == nullptr )
	{
		DeleteRubberBand();
		return;
	}

	//if the user simply clicked without dragging a rect
	//we will fabricate a small 1x1 pix rect and then continue
	//as if they had dragged a rect
	if ( !mDragging )
	{
		QgsMapToolSelectUtils::expandSelectRectangle( mSelectRect, vlayer, e->pos() );
	}
	else
	{
		// Set valid values for rectangle's width and height
		if ( mSelectRect.width() == 1 )
		{
			mSelectRect.setLeft( mSelectRect.left() + 1 );
		}
		if ( mSelectRect.height() == 1 )
		{
			mSelectRect.setBottom( mSelectRect.bottom() + 1 );
		}
	}

	if ( mRubberBand )
	{
		QgsMapToolSelectUtils::setRubberBand( mCanvas, mSelectRect, mRubberBand );

		QgsGeometry* selectGeom = mRubberBand->asGeometry();
		if ( !mDragging )
		{
			bool doDifference = e->modifiers() & Qt::ControlModifier ? true : false;
			QgsMapToolSelectUtils::setSelectFeatures( mCanvas, selectGeom, false, doDifference, true );
		}
		else
			QgsMapToolSelectUtils::setSelectFeatures( mCanvas, selectGeom, e );


#if defined(TEST_SELECTION)

		emit NewRubberBandSelection( QgsMapToolSelectUtils::toLayerCoordinates( mCanvas, selectGeom, vlayer ).boundingBox().toRectF() );
#else
		DeleteRubberBand();
#endif
		delete selectGeom;
	}

	mDragging = false;
}





///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_MapToolSelectRectangle.cpp"
