/***************************************************************************
qgsmaptoolselectpolygon.h  -  map tool for selecting features by polygon
---------------------
begin                : May 2010
copyright            : (C) 2010 by Jeremy Palmer
email                : jpalmer at linz dot govt dot nz
***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#ifndef VIEWS_QGISAPP_QGSMAPTOOLSELECTPOLYGON_H
#define VIEWS_QGISAPP_QGSMAPTOOLSELECTPOLYGON_H

#include "qgsmaptool.h"

class QgsMapCanvas;
class QgsRubberBand;


class CMapToolSelectPolygon : public QgsMapTool
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
	CMapToolSelectPolygon( QgsMapCanvas* canvas );

	virtual ~CMapToolSelectPolygon();

	//! Overridden mouse move event
	virtual void canvasMoveEvent( QMouseEvent * e ) override;

	//! Overridden mouse press event
	virtual void canvasPressEvent( QMouseEvent * e ) override;

private:

	//! used for storing all of the maps point for the polygon
	QgsRubberBand* mRubberBand;

	QColor mFillColor;

	QColor mBorderColour;
};

#endif				//VIEWS_QGISAPP_QGSMAPTOOLSELECTPOLYGON_H
