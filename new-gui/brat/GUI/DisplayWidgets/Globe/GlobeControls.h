#ifndef GUI_DISPLAY_WIDGETS_GLOBE_GLOBE_CONTROL_H
#define GUI_DISPLAY_WIDGETS_GLOBE_GLOBE_CONTROL_H


namespace osgViewer { class Viewer; }

namespace osgEarth {

	namespace Util {

		namespace Controls {

			class NavigationControl;
		}
	}
}


class CGlobeWidget;


//#ifdef HAVE_OSGEARTH_ELEVATION_QUERY
//#undef HAVE_OSGEARTH_ELEVATION_QUERY
//#endif



///////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Event Handlers
///////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////
// FlyToExtentHandler
//////////////////////////

class FlyToExtentHandler : public osgGA::GUIEventHandler
{
	CGlobeWidget *mGlobe = nullptr;
public:
	FlyToExtentHandler( CGlobeWidget* globe ) : mGlobe( globe ) { }

	bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa ) override;
};


//////////////////////////
// KeyboardControlHandler
//////////////////////////

class KeyboardControlHandler : public osgGA::GUIEventHandler
{
	osg::observer_ptr<osgEarth::Util::EarthManipulator> _manip;

public:
	KeyboardControlHandler( osgEarth::Util::EarthManipulator* manip ) : _manip( manip ) { }

	bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa ) override;
};


//////////////////////////
// QueryCoordinatesHandler
//////////////////////////

// An event handler that will print out the coordinates at the clicked point
#ifdef HAVE_OSGEARTH_ELEVATION_QUERY
#else
class QueryCoordinatesHandler : public osgGA::GUIEventHandler
{
public:
	QueryCoordinatesHandler( CGlobeWidget* globe, osgEarth::Util::ElevationManager* elevMan,
		const osgEarth::SpatialReference* mapSRS )
		:  mGlobe( globe ), _mapSRS( mapSRS ), _elevMan( elevMan ) { }

	bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );

	virtual osg::Vec3d getCoords( float x, float y, osgViewer::View* view, bool getElevation = false );

private:
	CGlobeWidget* mGlobe;
	osg::ref_ptr<const SpatialReference> _mapSRS;
	osg::ref_ptr<osgEarth::Util::ElevationManager> _elevMan;
};
#endif







///////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Controls
///////////////////////////////////////////////////////////////////////////////////////////////////////////

class CGlobeControls
{
	//static members

protected:
	static std::string smGlobeDir;

public:
	static const std::string& GlobeDir();

	// Must be called at startup, before 1st globe creation
	//
	static void SetControlsDirectory( const std::string &GlobeDir );


	//instance data
protected:

	CGlobeWidget *mGlobe = nullptr;
	osgViewer::Viewer *mOsgViewer = nullptr;
	osgEarth::Util::Controls::ControlCanvas *mControlCanvas = nullptr;

	ImageControl *mYawPitchWheel = nullptr;
	NavigationControl *mRotateCCW = nullptr;
	NavigationControl *mRotateCW = nullptr;
	NavigationControl *mRotateReset = nullptr;
	NavigationControl *mTiltUp = nullptr;
	NavigationControl *mTiltDown = nullptr;
	ImageControl *mMoveWheel = nullptr;
	NavigationControl *mMoveLeft = nullptr;
	NavigationControl *mMoveRight = nullptr;
	NavigationControl *mMoveUp = nullptr;
	NavigationControl *mMoveDown = nullptr;
	NavigationControl *mZoomHome = nullptr;
	ImageControl *mBackgroundGrp1 = nullptr;
	ImageControl *mZoomIn = nullptr;
	ImageControl *mZoomOut = nullptr;
	ImageControl *mBackgroundGrp2 = nullptr;
	ImageControl *mHome = nullptr;
	ImageControl *mRefresh = nullptr;
	ImageControl *mSync = nullptr;


	//construction / destruction
	
public:
	CGlobeControls( CGlobeWidget *globe );

	virtual ~CGlobeControls()
	{}	

	void AddControls();

	void RemoveControls();

private:
	template< typename CONTROL >
	void RemoveControl( CONTROL *&c );
};


#endif // GUI_DISPLAY_WIDGETS_GLOBE_GLOBE_CONTROL_H
