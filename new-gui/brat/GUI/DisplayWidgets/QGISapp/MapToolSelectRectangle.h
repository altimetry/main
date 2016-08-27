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

#include "MapTools.h"


class QPoint;
class QMouseEvent;
class QgsMapCanvas;
class QgsVectorLayer;
class QgsGeometry;
class QgsRubberBand;


class CMapToolSelectFeatures : public QgsMapTool
{
	// types

#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

	Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	// instance data

	//! Flag to indicate a map canvas drag operation is taking place
	bool mDragging;

	//! stores actual select rect
	QRect mSelectRect;

	QgsRubberBand* mRubberBand;

	QColor mFillColor;

	QColor mBorderColour;

	// construction / destruction

public:
	CMapToolSelectFeatures( QgsMapCanvas* canvas );

	//! Overridden mouse move event
	virtual void canvasMoveEvent( map_tool_mouse_event_t *e ) override;

	//! Overridden mouse press event
	virtual void canvasPressEvent( map_tool_mouse_event_t *e ) override;

	//! Overridden mouse release event
	virtual void canvasReleaseEvent( map_tool_mouse_event_t *e ) override;

	void SetRubberBandSelection( double lonm, double lonM, double latm, double latM );
	void RemoveRubberBandSelection();														//TODO confirm it is really needed


protected:

	void CreateRubberBand();
	void DeleteRubberBand();


signals:

	void NewRubberBandSelection( QRectF bounding );
};

#endif
