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
#ifndef GUI_DISPLAY_WIDGETS_MAP_TIP_H
#define GUI_DISPLAY_WIDGETS_MAP_TIP_H

//class QgsMapLayer;
//class CMapWidget;
//class QPoint;
//class QString;
//
//#include "qgsfeature.h"

// A map tip is a class to display a tip on a map canvas
// when a mouse is hovered over a feature.
//
class CMapTip : public QObject
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

	Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

    //	types & friends

    using base_t = QObject;


	// instance data

	// ...flag to indicate if a map tip is currently being displayed
	//
	bool mMapTipVisible = false;

	// ...last point on the map canvas when the map tip timer fired. This point is in widget pixel coordinates
	//
	QPoint mLastPosition;

	// construction / destruction

public:
	CMapTip( QObject *parent = nullptr )
		: base_t( parent )
		, mMapTipVisible( false )
	{}

	virtual ~CMapTip()
	{}

	// Show a map tip at a given point on the map canvas
	// - layer a qgis vector map layer pointer that will
	//         be used to provide the attribute data for the map tip.
	// - theMapPosition a reference to the position of the cursor
	//         in map coordinates.
	//  - thePixelPosition a reference to the position of the cursor
	//         in pixel coordinates.
	//  - map_canvas the map canvas that owns the layer (can be equal to dest)
    //  - dest the widget on which the tip is drawn
	// 
	void ShowMapTip( QgsMapLayer *layer, QgsPoint & map_position, const QPoint &pixel_position, CMapWidget *map_canvas, QWidget *dest );

	// Clear the current map tip if it exists 
	//	- map_canvas the canvas from which the tip should be cleared.
	//
	void Clear();

private:
	// Fetch the feature to use for the map tip text. Only the first feature in the search radius is used

	QString FetchFeature( QgsMapLayer * layer, QgsPoint & theMapPosition, CMapWidget *map_canvas );
};


#endif // GUI_DISPLAY_WIDGETS_MAP_TIP_H
