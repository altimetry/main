/***************************************************************************
	globe.h

	Globe Plugin
	a QGIS plugin
	--------------------------------------
	Date                 : 08-Jul-2010
	Copyright            : (C) 2010 by Sourcepole
	Email                : info at sourcepole.ch
	***************************************************************************
	*                                                                         *
	*   This program is free software; you can redistribute it and/or modify  *
	*   it under the terms of the GNU General Public License as published by  *
	*   the Free Software Foundation; either version 2 of the License, or     *
	*   (at your option) any later version.                                   *
	*                                                                         *
	***************************************************************************/

#ifndef GRAPHICS_GLOBE_GLOBE_H
#define GRAPHICS_GLOBE_GLOBE_H

#include <QObject>

#if defined (WIN32) || defined(_WIN32)
#pragma warning ( disable: 4100 )           //unreferenced formal parameter
#endif

#include <osgEarth/Version>

#include "qgspoint.h"
#include <qgsmapcanvas.h>

#if defined (WIN32) || defined(_WIN32)
#pragma warning ( default: 4100 )           //unreferenced formal parameter
#endif



class QAction;
class QToolBar;
class QgisInterface;
class QgsGlobePluginDialog;


namespace osgViewer { class Viewer; }

namespace osgEarth { 

	class MapNode; class ElevationQuery; class ImageLayer;

	
	namespace QtGui { class ViewerWidget; } 

	namespace Drivers {	class QgsOsgEarthTileSource; }

	namespace Util { 
		
		class SkyNode; class VerticalScale; class ObjectLocator;

		namespace Controls {

			class ControlCanvas;
		}
	}
}



class GlobePlugin : public QObject
{
	Q_OBJECT

	friend class QgsGlobePluginDialog;

	QgsMapCanvas *mCanvas;

public:
	GlobePlugin( QgisInterface* theQgisInterface, QgsMapCanvas *theCanvas = nullptr, QWidget *dialogParent = nullptr );
	virtual ~GlobePlugin();

public slots:
	//! Show the dialog box
	osgViewer::Viewer* run( const std::string &imgDir );
	//! Show the settings dialog box
	void settings();
	//!  Reset globe
	void reset();

	//! Called when a new set of image layers has been received
	void imageLayersChanged();
	//! Called when a new set of elevation layers has been received
	void elevationLayersChanged();
	//! Set a different base map (QString::NULL will disable the base map)
	void setBaseMap( QString url );
	//! Called when the extents of the map change
	void setSkyParameters( bool enabled, const QDateTime& dateTime, bool autoAmbience );
	//! Called when the extents of the map change
	void extentsChanged();
	//! Sync globe extent to mapCanavas
	void syncExtent();
#if OSGEARTH_VERSION_GREATER_OR_EQUAL( 2, 5, 0 )
	//! Set vertical scale
	void setVerticalScale( double scale );
#endif

	//! called when a project has been read successfully
	void projectReady();
	//! called when a new project has been created successfully
	void blankProjectReady();
	//! called when the globe window is closed
	void setGlobeNotRunning();
	//! set the globe coordinates of a user right-click on the globe
	void setSelectedCoordinates( osg::Vec3d coords );
	//! get a coordinates vector
	osg::Vec3d getSelectedCoordinates();
	//! prints the ccordinates in a QMessageBox
	void showSelectedCoordinates();
	//! emits signal with current mouse coordinates
	void showCurrentCoordinates( double lon, double lat );
	//! get longitude of user right click
	double getSelectedLon();
	//! get latitude of user right click
	double getSelectedLat();
	//! get elevation of user right click
	double getSelectedElevation();

	//! Place an OSG model on the globe
	void placeNode( osg::Node* node, double lat, double lon, double alt = 0.0 );

	osgViewer::Viewer* osgViewer() { return mOsgViewer; }

	//! Recursive copy folder
	static void copyFolder( QString sourceFolder, QString destFolder );

private:
	//!  Setup map
	void setupMap();
	//!  Setup map controls
	void setupControls( const std::string &imgDir );

private:
	//! Checks if the globe is open
	//! Pointer to the QGIS interface object
	//femm    QgisInterface *mQGisIface;
	//!pointer to the qaction for this plugin
	//! OSG Viewer
	osgViewer::Viewer* mOsgViewer;

	//! Settings Dialog
	QgsGlobePluginDialog *mSettingsDialog;
	//! OSG root node
	osg::Group* mRootNode;
	//! Map node
	osgEarth::MapNode* mMapNode;
	//! Base layer
	osg::ref_ptr<osgEarth::ImageLayer> mBaseLayer;
	//! Sky node
	osg::ref_ptr<osgEarth::Util::SkyNode> mSkyNode;
#if OSGEARTH_VERSION_GREATER_OR_EQUAL( 2, 5, 0 )
	osg::ref_ptr<osgEarth::Util::VerticalScale> mVerticalScale;
#endif
	//! QGIS maplayer
	osgEarth::ImageLayer* mQgisMapLayer;
	//! Tile source
	osgEarth::Drivers::QgsOsgEarthTileSource* mTileSource;
	//! Control Canvas
	osgEarth::Util::Controls::ControlCanvas* mControlCanvas;
#ifdef HAVE_OSGEARTH_ELEVATION_QUERY
	//! Elevation manager
	osgEarth::ElevationQuery* mElevationManager;
	//! Object placer
	osgEarth::Util::ObjectLocator* mObjectPlacer;
#else
	//! Elevation manager
	osgEarth::Util::ElevationManager* mElevationManager;
	//! Object placer
	osgEarth::Util::ObjectPlacer* mObjectPlacer;
#endif
	//! tracks if the globe is open
	bool mIsGlobeRunning;
	//! coordinates of the right-clicked point on the globe
	double mSelectedLat, mSelectedLon, mSelectedElevation;

signals:
	//! emits current mouse position
	void xyCoordinates( const QgsPoint & p );
	//! emits position of right click on globe
	void newCoordinatesSelected( const QgsPoint & p );
};


#endif // GRAPHICS_GLOBE_GLOBE_H
