#ifndef GUI_DISPLAY_WIDGETS_GLOBE_WIDGET_H
#define GUI_DISPLAY_WIDGETS_GLOBE_WIDGET_H


#if defined (WIN32) || defined(_WIN32)
#pragma warning ( disable: 4100 )           //unreferenced formal parameter
#endif

#include <osgEarth/Version>
#include <osgEarthUtil/Sky>					//femm	SkyNode => Sky
#include <osgEarthUtil/GraticuleNode>

static_assert( OSGEARTH_VERSION_GREATER_OR_EQUAL( 2, 5, 0 ), "osgEarth version not supported." );


#if defined (WIN32) || defined(_WIN32)
#pragma warning ( default: 4100 )           //unreferenced formal parameter
#endif



class QgsMapCanvas;
class CGlobeViewerWidget;


class QAction;
class QToolBar;
class QgisInterface;
class QgsGlobePluginDialog;
class CRootUpdateCallback;
class CGlobeControls;


namespace osgViewer { class Viewer; }


namespace osg { 

	class Group;  

	template<class T>
	class ref_ptr; 
}


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





class CGlobeWidget : public QWidget
{

#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

	Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif


	//types

	using base_t = QWidget;

	friend class CRootUpdateCallback;



	//static members

public:
	// Must be called at startup, before 1st globe creation
	//
	static void SetOSGDirectories( const std::string &GlobeDir );


protected:

	////////////////////////////
	//instance data
	////////////////////////////

	//...OSG Viewer

	osgViewer::Viewer *mOsgViewer = nullptr;
	CGlobeViewerWidget *mGlobeViewerWidget = nullptr;
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

	osg::ref_ptr<osgEarth::Util::VerticalScale> mVerticalScale;

	//! QGIS map layer
	osgEarth::ImageLayer *mQgisMapLayer = nullptr;

	//! Tile source
	osgEarth::Drivers::QgsOsgEarthTileSource *mTileSource = nullptr;


	//...interaction control

	//! Control Canvas
	osgEarth::Util::Controls::ControlCanvas *mControlCanvas = nullptr;

	osg::ref_ptr<osgEarth::Util::GraticuleNode> mGraticuleNode = nullptr;

	CGlobeControls *mControls = nullptr;


	//...coordinates

	//! coordinates of the right-clicked point on the globe
	double mSelectedLat = 0., mSelectedLon = 0., mSelectedElevation = 0.;


	//...context menu
	//
	QAction *mActionSettingsDialog = nullptr;
	QAction *mActionSky = nullptr;
	QAction *mActionSetupControls = nullptr;


	////////////////////////////
	//construction / destruction
	////////////////////////////

	//!  Setup map
	void SetupMap();
	osgEarth::Util::GraticuleNode* CreateGraticule();
	void Wire();
	void Unwire();
public:
	CGlobeWidget( QWidget *parent, QgsMapCanvas *the_canvas, QStatusBar *sb = nullptr );

	virtual ~CGlobeWidget();

	bool ScheduleClose();

	//access

	osgViewer::Viewer* osgViewer() { return mOsgViewer; }


signals:
	//! emits current mouse position
	void xyCoordinates( const QgsPoint & p );

	//! emits position of right click on globe
	void newCoordinatesSelected( const QgsPoint & p );


public slots:

	////////////////////////////
	//original methods
	////////////////////////////

	//! Called when a new set of image layers has been received
	void imageLayersChanged();

	//! Called when a new set of elevation layers has been received
	//void elevationLayersChanged();

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

	//!  Setup map controls
	void SetupControls( bool set );


protected slots:

	void ToggleGridEnabled( bool toggle );

    void HandleSky( bool toggled );

	void CanvasStarted();
	void CanvasFinished();

	void ChangeImageLayers();

protected:

	QSize sizeHint() const override
    {
        return QSize(72 * fontMetrics().width('x'), 25 * fontMetrics().lineSpacing());
    }

};


#endif			//	GUI_DISPLAY_WIDGETS_GLOBE_WIDGET_H
