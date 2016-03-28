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

#ifndef GUI_DISPLAY_WIDGETS_GLOBE_GLOBE_H
#define GUI_DISPLAY_WIDGETS_GLOBE_GLOBE_H

#include <QObject>

#if defined (WIN32) || defined(_WIN32)
#pragma warning ( disable: 4100 )           //unreferenced formal parameter
#endif

#include <osgEarth/Version>
#include <osgEarthUtil/GraticuleNode>

static_assert( OSGEARTH_VERSION_GREATER_OR_EQUAL( 2, 5, 0 ), "osgEarth version not supported." );


#if defined (WIN32) || defined(_WIN32)
#pragma warning ( default: 4100 )           //unreferenced formal parameter
#endif


#include <qgsmapcanvas.h>




//#ifdef HAVE_OSGEARTH_ELEVATION_QUERY
//#undef HAVE_OSGEARTH_ELEVATION_QUERY
//#endif

#define GLOBE_DIALOG_ENABLED


class QAction;
class QToolBar;
class QgisInterface;
class QgsGlobePluginDialog;
class CRootUpdateCallback;
class CGlobeControls;

namespace osgViewer { class Viewer; }

namespace osgEarth { 

	class MapNode; class ElevationQuery; class ImageLayer;

	
	namespace QtGui { class ViewerWidget; } 

	namespace Drivers {	class QgsOsgEarthTileSource; }

	namespace Util { 
		
		class SkyNode; class VerticalScale; class ObjectLocator; class GraticuleNode;

		namespace Controls {

			class ControlCanvas;
		}
	}
}







class CGlobePlugin : public QObject
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

	Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif


#if defined (GLOBE_DIALOG_ENABLED)

	friend class QgsGlobePluginDialog;

	//! Settings Dialog
	QgsGlobePluginDialog *mSettingsDialog = nullptr;

public:
	//! Show the settings dialog box
	void settings();

	//! called when a project has been read successfully
	void projectReady();

	//! called when a new project has been created successfully
	void blankProjectReady();

#else

public:
		void settings(){}

#endif

private:

	friend class CRootUpdateCallback;


	////////////////////////////
	//static members
	////////////////////////////

public:
	// Must be called at startup, before 1st globe creation
	//
	static void SetOSGDirectories( const std::string &GlobeDir );

protected:

	////////////////////////////
	//instance data
	////////////////////////////

	//...OSG Viewer

	osgViewer::Viewer* mOsgViewer = nullptr;

	QgsMapCanvas *mCanvas = nullptr;

	CRootUpdateCallback *mRootUpdateCallback = nullptr;


	//...scene graph nodes

	//! OSG root node
	osg::Group *mRootNode = nullptr;

	//! Map node
	osgEarth::MapNode *mMapNode = nullptr;

	//! Sky node
	osg::ref_ptr<osgEarth::Util::SkyNode> mSkyNode;


	//...layers

	//! Base layer
	osg::ref_ptr<osgEarth::ImageLayer> mBaseLayer;

	osg::ref_ptr<osgEarth::Util::VerticalScale> mVerticalScale;		//OSGEARTH_VERSION_GREATER_OR_EQUAL( 2, 5, 0 )

	//! QGIS map layer
	osgEarth::ImageLayer *mQgisMapLayer = nullptr;

	//! Tile source
	osgEarth::Drivers::QgsOsgEarthTileSource *mTileSource = nullptr;


	//...interaction control

	//! Control Canvas
	osgEarth::Util::Controls::ControlCanvas *mControlCanvas = nullptr;

	osg::ref_ptr<osgEarth::Util::GraticuleNode> mGraticuleNode = nullptr;

	CGlobeControls *mControls = nullptr;


#ifdef HAVE_OSGEARTH_ELEVATION_QUERY
	//! Elevation manager
	osgEarth::ElevationQuery *mElevationManager = nullptr;
	//! Object placer
	osgEarth::Util::ObjectLocator *mObjectPlacer = nullptr;
#else
	//! Elevation manager
	osgEarth::Util::ElevationManager *mElevationManager = nullptr;
	//! Object placer
	osgEarth::Util::ObjectPlacer *mObjectPlacer = nullptr;
#endif

	//...coordinates

	//! coordinates of the right-clicked point on the globe
	double mSelectedLat = 0., mSelectedLon = 0., mSelectedElevation = 0.;


	////////////////////////////
	//construction / destruction
	////////////////////////////

public:
	CGlobePlugin( QgsMapCanvas *the_canvas, QStatusBar *sb = nullptr, QWidget *dialogParent = nullptr );
	virtual ~CGlobePlugin();

	//access

	osgViewer::Viewer* osgViewer() { return mOsgViewer; }


	void RemoveLayers();

public slots:
	//!  Setup map controls
	void SetupControls( bool set );

	void ToggleGridEnabled( bool toggle );


	////////////////////////////
	//original methods
	////////////////////////////

	//! Called when a new set of image layers has been received
	void imageLayersChanged();

	//! Called when a new set of elevation layers has been received
	void elevationLayersChanged();

	//! Set a different base map (QString::NULL will disable the base map)
	void setBaseMap( QString url );

	//! Called when the extents of the map change
	void extentsChanged();

    //! Called when the extents of the map change
    void setSkyParameters( bool sky, const QDateTime& date_itime, bool auto_ambience );

	//! Sync globe extent to mapCanavas
	void syncExtent();

	//! Set vertical scale
	void setVerticalScale( double scale );		//OSGEARTH_VERSION_GREATER_OR_EQUAL( 2, 5, 0 )

	void SetSkyParameters( bool sky );			//OSGEARTH_VERSION_GREATER_OR_EQUAL( 2, 5, 0 )

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

private:
	//!  Setup map
	void setupMap();

	void ImageLayersChanged();

signals:
	//! emits current mouse position
	void xyCoordinates( const QgsPoint & p );

	//! emits position of right click on globe
	void newCoordinatesSelected( const QgsPoint & p );
};


#endif // GUI_DISPLAY_WIDGETS_GLOBE_GLOBE_H
