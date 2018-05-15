/***************************************************************************
	qgsmeasuretool.h  -  map tool for measuring distances and areas
	---------------------
	begin                : April 2007
	copyright            : (C) 2007 by Martin Dobias
	email                : wonder.sk at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef VIEWS_QGISAPP_QGSMEASURESTOOL_H
#define VIEWS_QGISAPP_QGSMEASURESTOOL_H

#include "qgsmaptool.h"

#include "MapTools.h"

class QgsDistanceArea;
class QgsMapCanvas;
class CMeasuresToolDialog;
class QgsRubberBand;



class CMeasuresTool : public QgsMapTool
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

	CMeasuresTool( QgsMapCanvas* canvas, bool measureArea );

	~CMeasuresTool();

	virtual Flags flags() const override { return QgsMapTool::AllowZoomRect; }

	//! returns whether measuring distance or area
	bool measureArea() { return mMeasureArea; }

	//! When we have added our last point, and not following
	bool done() { return mDone; }

	//! Reset and start new
	void restart();

	//! Add new point
	void addPoint( const QgsPoint &point );

	//! Returns reference to array of the points
	const QList<QgsPoint>& points();

	// Inherited from QgsMapTool

	//! Mouse move event for overriding
	virtual void canvasMoveEvent( map_tool_mouse_event_t *e ) override;

	//! Mouse press event for overriding
	virtual void canvasPressEvent( map_tool_mouse_event_t *e ) override;

	//! Mouse release event for overriding
	virtual void canvasReleaseEvent( map_tool_mouse_event_t *e ) override;

	//! called when set as currently active map tool
	virtual void activate() override;

	//! called when map tool is being deactivated
	virtual void deactivate() override;

	virtual void keyPressEvent( QKeyEvent* e ) override;

public slots:
	//! updates the projections we're using
	void updateSettings();

protected:

	QList<QgsPoint> mPoints;

	CMeasuresToolDialog *mDialog;

	//! Rubberband widget tracking the lines being drawn
	QgsRubberBand *mRubberBand;

	//! Rubberband widget tracking the added nodes to line
	QgsRubberBand *mRubberBandPoints;

	//! indicates whether we're measuring distances or areas
	bool mMeasureArea;

	//! indicates whether we've just done a right mouse click
	bool mDone;

	//! indicates whether we've recently warned the user about having the wrong
	// project projection
	bool mWrongProjectProjection;

	//! Destination CoordinateReferenceSystem used by the MapCanvas
	QgsCoordinateReferenceSystem mDestinationCrs;

	//! Returns the snapped (map) coordinate
	//@param p (pixel) coordinate
	QgsPoint snapPoint( const QPoint& p );

	/** Removes the last vertex from mRubberBand*/
	void undo();
};

#endif		//VIEWS_QGISAPP_QGSMEASURESTOOL_H
