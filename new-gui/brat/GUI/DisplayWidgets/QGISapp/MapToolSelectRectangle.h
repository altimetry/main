/***************************************************************************
    qgsmaptoolselectrectangle.h  -  map tool for selecting features by
                                 rectangle
    ---------------------
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

#ifndef VIEWS_QGISAPP_MAPTOOLRECTANGLE_H
#define VIEWS_QGISAPP_MAPTOOLRECTANGLE_H

#include <QRect>

#include <qgsmaptool.h>


class QPoint;
class QMouseEvent;
class QgsMapCanvas;
class QgsVectorLayer;
class QgsGeometry;
class QgsRubberBand;


class CMapToolSelectFeatures : public QgsMapTool
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

	Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

public:
	CMapToolSelectFeatures( QgsMapCanvas* canvas );

	//! Overridden mouse move event
	virtual void canvasMoveEvent( QMouseEvent * e ) override;

	//! Overridden mouse press event
	virtual void canvasPressEvent( QMouseEvent * e ) override;

	//! Overridden mouse release event
	virtual void canvasReleaseEvent( QMouseEvent * e ) override;

private:

	//! Flag to indicate a map canvas drag operation is taking place
	bool mDragging;

	//! stores actual select rect
	QRect mSelectRect;

	QgsRubberBand* mRubberBand;

	QColor mFillColor;

	QColor mBorderColour;
};

#endif
