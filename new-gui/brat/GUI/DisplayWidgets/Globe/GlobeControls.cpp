#include "new-gui/brat/stdafx.h"

#include <osgViewer/Viewer>


#if defined (WIN32) || defined(_WIN32)
#pragma warning ( disable: 4100 )           //unreferenced formal parameter
#endif

#include <osgEarthUtil/EarthManipulator>

//using namespace osgEarth::Drivers;
using namespace osgEarth::Util;


#ifndef HAVE_OSGEARTHQT 
	//use back-ported controls if osgEarth <= 2.1
	#define USE_BACKPORTED_CONTROLS
#endif
#ifdef USE_BACKPORTED_CONTROLS
	#include "osgEarthUtil/Controls"
	using namespace osgEarth::Util::Controls21;
#else
	#include <osgEarthUtil/Controls>
	using namespace osgEarth::Util::Controls;
#endif

#if defined (WIN32) || defined(_WIN32)
#pragma warning ( default: 4100 )           //unreferenced formal parameter
#endif


#include "new-gui/Common/QtUtils.h"

#include "Globe.h"
#include "GlobeControls.h"


#define MOVE_OFFSET 0.05




///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Event Handlers  (for the Globe Widget)
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////
// KeyboardControlHandler
/////////////////////////

bool KeyboardControlHandler::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& /*aa*/ )
{
#if 0
	osgEarth::Util::EarthManipulator::Settings* _manipSettings = _manip->getSettings();
	_manip->getSettings()->bindKey( osgEarth::Util::EarthManipulator::ACTION_ZOOM_IN, osgGA::GUIEventAdapter::KEY_Space );
	//install default action bindings:
	osgEarth::Util::EarthManipulator::ActionOptions options;

	_manipSettings->bindKey( osgEarth::Util::EarthManipulator::ACTION_HOME, osgGA::GUIEventAdapter::KEY_Space );

	_manipSettings->bindMouse( osgEarth::Util::EarthManipulator::ACTION_PAN, osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON );

	// zoom as you hold the right button:
	options.clear();
	options.add( osgEarth::Util::EarthManipulator::OPTION_CONTINUOUS, true );
	_manipSettings->bindMouse( osgEarth::Util::EarthManipulator::ACTION_ROTATE, osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON, 0L, options );

	// zoom with the scroll wheel:
	_manipSettings->bindScroll( osgEarth::Util::EarthManipulator::ACTION_ZOOM_IN,  osgGA::GUIEventAdapter::SCROLL_DOWN );
	_manipSettings->bindScroll( osgEarth::Util::EarthManipulator::ACTION_ZOOM_OUT, osgGA::GUIEventAdapter::SCROLL_UP );

	// pan around with arrow keys:
	_manipSettings->bindKey( osgEarth::Util::EarthManipulator::ACTION_PAN_LEFT,  osgGA::GUIEventAdapter::KEY_Left );
	_manipSettings->bindKey( osgEarth::Util::EarthManipulator::ACTION_PAN_RIGHT, osgGA::GUIEventAdapter::KEY_Right );
	_manipSettings->bindKey( osgEarth::Util::EarthManipulator::ACTION_PAN_UP,    osgGA::GUIEventAdapter::KEY_Up );
	_manipSettings->bindKey( osgEarth::Util::EarthManipulator::ACTION_PAN_DOWN,  osgGA::GUIEventAdapter::KEY_Down );

	// double click the left button to zoom in on a point:
	options.clear();
	options.add( osgEarth::Util::EarthManipulator::OPTION_GOTO_RANGE_FACTOR, 0.4 );
	_manipSettings->bindMouseDoubleClick( osgEarth::Util::EarthManipulator::ACTION_GOTO, osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON, 0L, options );

	// double click the right button(or CTRL-left button) to zoom out to a point
	options.clear();
	options.add( osgEarth::Util::EarthManipulator::OPTION_GOTO_RANGE_FACTOR, 2.5 );
	_manipSettings->bindMouseDoubleClick( osgEarth::Util::EarthManipulator::ACTION_GOTO, osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON, 0L, options );
	_manipSettings->bindMouseDoubleClick( osgEarth::Util::EarthManipulator::ACTION_GOTO, osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON, osgGA::GUIEventAdapter::MODKEY_CTRL, options );

	_manipSettings->setThrowingEnabled( false );
	_manipSettings->setLockAzimuthWhilePanning( true );

	_manip->applySettings( _manipSettings );
#endif

	switch ( ea.getEventType() )
	{
		case( osgGA::GUIEventAdapter::KEYDOWN ) :
		{
			//move map
			if ( ea.getKey() == '4' )
			{
				_manip->pan( -MOVE_OFFSET, 0 );
			}
			if ( ea.getKey() == '6' )
			{
				_manip->pan( MOVE_OFFSET, 0 );
			}
			if ( ea.getKey() == '2' )
			{
				_manip->pan( 0, MOVE_OFFSET );
			}
			if ( ea.getKey() == '8' )
			{
				_manip->pan( 0, -MOVE_OFFSET );
			}
			//rotate
			if ( ea.getKey() == '/' )
			{
				_manip->rotate( MOVE_OFFSET, 0 );
			}
			if ( ea.getKey() == '*' )
			{
				_manip->rotate( -MOVE_OFFSET, 0 );
			}
			//tilt
			if ( ea.getKey() == '9' )
			{
				_manip->rotate( 0, MOVE_OFFSET );
			}
			if ( ea.getKey() == '3' )
			{
				_manip->rotate( 0, -MOVE_OFFSET );
			}
			//zoom
			if ( ea.getKey() == '-' )
			{
				_manip->zoom( 0, MOVE_OFFSET );
			}
			if ( ea.getKey() == '+' )
			{
				_manip->zoom( 0, -MOVE_OFFSET );
			}
			//reset
			if ( ea.getKey() == '5' )
			{
				//_manip->zoom( 0, 0 );
			}
			break;
		}

		default:
			break;
	}
	return false;
}



/////////////////////////
// FlyToExtentHandler
/////////////////////////

bool FlyToExtentHandler::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& /*aa*/ )
{
	if ( ea.getEventType() == ea.KEYDOWN && ea.getKey() == '1' )
	{
		mGlobe->syncExtent();
	}
	return false;
}


//////////////////////////
// QueryCoordinatesHandler
//////////////////////////

#ifdef HAVE_OSGEARTH_ELEVATION_QUERY
#else
bool QueryCoordinatesHandler::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{
	if ( ea.getEventType() == osgGA::GUIEventAdapter::MOVE )
	{
		osgViewer::View* view = static_cast<osgViewer::View*>( aa.asView() );
		osg::Vec3d coords = getCoords( ea.getX(), ea.getY(), view, false );
		mGlobe->showCurrentCoordinates( coords.x(), coords.y() );
	}
	if ( ea.getEventType() == osgGA::GUIEventAdapter::PUSH
		&& ea.getButtonMask() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON )
	{
		osgViewer::View* view = static_cast<osgViewer::View*>( aa.asView() );
		osg::Vec3d coords = getCoords( ea.getX(), ea.getY(), view, false );

		OE_NOTICE << "SelectedCoordinates set to:\nLon: " << coords.x() << " Lat: " << coords.y()
			<< " Ele: " << coords.z() << std::endl;

		mGlobe->setSelectedCoordinates( coords );

		if ( ea.getModKeyMask() == osgGA::GUIEventAdapter::MODKEY_CTRL )
			//ctrl + rightclick pops up a QMessageBox
		{
			mGlobe->showSelectedCoordinates();
		}
	}

	return false;
}

osg::Vec3d QueryCoordinatesHandler::getCoords( float x, float y, osgViewer::View* view, bool getElevation )
{
	osgUtil::LineSegmentIntersector::Intersections results;
	osg::Vec3d coords;
	if ( view->computeIntersections( x, y, results, 0x01 ) )
	{
		// find the first hit under the mouse:
		osgUtil::LineSegmentIntersector::Intersection first = *( results.begin() );
		osg::Vec3d point = first.getWorldIntersectPoint();

		// transform it to map coordinates:
		double lat_rad, lon_rad, height;
		_mapSRS->getEllipsoid()->convertXYZToLatLongHeight( point.x(), point.y(), point.z(), lat_rad, lon_rad, height );

		// query the elevation at the map point:
		double lon_deg = osg::RadiansToDegrees( lon_rad );
		double lat_deg = osg::RadiansToDegrees( lat_rad );
		double elevation = -99999;

		if ( getElevation )
		{
			osg::Matrixd out_mat;
			double query_resolution = 0.1; // 1/10th of a degree
			double out_resolution = 0.0;

			//TODO test elevation calculation
			//@see https://github.com/gwaldron/osgearth/blob/master/src/applications/osgearth_elevation/osgearth_elevation.cpp
			if ( _elevMan->getPlacementMatrix(
				lon_deg, lat_deg, 0,
				query_resolution, _mapSRS,
				//query_resolution, NULL,
				out_mat, elevation, out_resolution ) )
			{
				OE_NOTICE << "Elevation at " << lon_deg << ", " << lat_deg
					<< " is " << elevation << std::endl;
			}
			else
			{
				OE_NOTICE << "getElevation FAILED! at (" << lon_deg << ", "
					<< lat_deg << ")" << std::endl;
			}
		}

		coords = osg::Vec3d( lon_deg, lat_deg, elevation );
	}
	return coords;
}
#endif




///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//		osgEarth Specialized Controls (for the CGlobeControls)
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////
// Base Class: NavigationControlHandler
/////////////


namespace osgEarth
{
	namespace Util
	{
#ifdef USE_BACKPORTED_CONTROLS
		namespace Controls21
#else
		namespace Controls
#endif
		{
			class NavigationControlHandler : public ControlEventHandler
			{
			public:
				virtual void onMouseDown( class Control* control, int mouseButtonMask ) { Q_UNUSED( control ); Q_UNUSED( mouseButtonMask ); }
				virtual void onClick( class Control* control, int mouseButtonMask, const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa ) { Q_UNUSED( control ); Q_UNUSED( mouseButtonMask ); Q_UNUSED( ea ); Q_UNUSED( aa ); }
				virtual void onClick( class Control* control, int mouseButtonMask ) override { Q_UNUSED( control ); Q_UNUSED( mouseButtonMask ); }
			};

			class NavigationControl : public ImageControl
			{
				osg::ref_ptr<const osgGA::GUIEventAdapter> _mouse_down_event;

			public:
				NavigationControl( osg::Image* image = 0L ) : ImageControl( image ), _mouse_down_event( NULL ) 
				{}

			protected:
				virtual bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, ControlContext& cx ) override;
			};
		}
	}
}



bool NavigationControl::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, ControlContext& cx )
{
	switch ( ea.getEventType() )
	{
		case osgGA::GUIEventAdapter::PUSH:
			_mouse_down_event = &ea;
			break;
		case osgGA::GUIEventAdapter::FRAME:
			if ( _mouse_down_event )
			{
				_mouse_down_event = &ea;
			}
			break;
		case osgGA::GUIEventAdapter::RELEASE:
			for ( ControlEventHandlerList::const_iterator i = _eventHandlers.begin(); i != _eventHandlers.end(); ++i )
			{
				NavigationControlHandler* handler = dynamic_cast<NavigationControlHandler*>( i->get() );
				if ( handler )
				{
					handler->onClick( this, ea.getButtonMask(), ea, aa );
				}
			}
			_mouse_down_event = NULL;
			break;
		default:
			/* ignore */
			;
	}
	if ( _mouse_down_event )
	{
		//OE_NOTICE << "NavigationControl::handle getEventType " << ea.getEventType() << std::endl;
		for ( ControlEventHandlerList::const_iterator i = _eventHandlers.begin(); i != _eventHandlers.end(); ++i )
		{
			NavigationControlHandler* handler = dynamic_cast<NavigationControlHandler*>( i->get() );
			if ( handler )
			{
				handler->onMouseDown( this, ea.getButtonMask() );
			}
		}
	}
	return Control::handle( ea, aa, cx );
}




///////////
// Pan
///////////

struct PanControlHandler : public NavigationControlHandler
{
	PanControlHandler( osgEarth::Util::EarthManipulator* manip, double dx, double dy ) : _manip( manip ), _dx( dx ), _dy( dy ) { }
	virtual void onMouseDown( Control* /*control*/, int /*mouseButtonMask*/ ) override
	{
		_manip->pan( _dx, _dy );
	}
private:
	osg::observer_ptr<osgEarth::Util::EarthManipulator> _manip;
	double _dx;
	double _dy;
};



///////////
// Rotate
///////////

struct RotateControlHandler : public NavigationControlHandler
{
	RotateControlHandler( osgEarth::Util::EarthManipulator* manip, double dx, double dy ) : _manip( manip ), _dx( dx ), _dy( dy ) { }
	virtual void onMouseDown( Control* /*control*/, int /*mouseButtonMask*/ ) override
	{
		if ( 0 == _dx && 0 == _dy )
		{
			_manip->setRotation( osg::Quat() );
		}
		else
		{
			_manip->rotate( _dx, _dy );
		}
	}
private:
	osg::observer_ptr<osgEarth::Util::EarthManipulator> _manip;
	double _dx;
	double _dy;
};



///////////
// Zoom
///////////

struct ZoomControlHandler : public NavigationControlHandler
{
	ZoomControlHandler( osgEarth::Util::EarthManipulator* manip, double dx, double dy ) : _manip( manip ), _dx( dx ), _dy( dy ) { }
	virtual void onMouseDown( Control* /*control*/, int /*mouseButtonMask*/ ) override
	{
		_manip->zoom( _dx, _dy );
	}
private:
	osg::observer_ptr<osgEarth::Util::EarthManipulator> _manip;
	double _dx;
	double _dy;
};



///////////
// Home
///////////

struct HomeControlHandler : public NavigationControlHandler
{
	HomeControlHandler( osgEarth::Util::EarthManipulator* manip ) : _manip( manip ) { }
	virtual void onClick( Control* /*control*/, int /*mouseButtonMask*/, const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa ) override
	{
		_manip->home( ea, aa );
	}
private:
	osg::observer_ptr<osgEarth::Util::EarthManipulator> _manip;
};



///////////
// Refresh
///////////

struct RefreshControlHandler : public ControlEventHandler
{
	RefreshControlHandler( CGlobePlugin* globe ) : mGlobe( globe ) { }
	virtual void onClick( Control* /*control*/, int /*mouseButtonMask*/ ) override
	{
		mGlobe->imageLayersChanged();
	}
private:
	CGlobePlugin* mGlobe;
};



/////////////
// SyncExtent
/////////////

struct SyncExtentControlHandler : public ControlEventHandler
{
	SyncExtentControlHandler( CGlobePlugin* globe ) : mGlobe( globe ) { }
	virtual void onClick( Control* /*control*/, int /*mouseButtonMask*/ ) override
	{
		mGlobe->syncExtent();
	}
private:
	CGlobePlugin* mGlobe;
};









///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//		CGlobeControls
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////
//		CGlobeControls statics
///////////////////////////////////////////////////////////////////////////////////////////////////////////


//static 
std::string CGlobeControls::smGlobeDir;


void CGlobeControls::SetControlsDirectory( const std::string &GlobeDir )
{
	smGlobeDir = GlobeDir;
}


//static 
const std::string& CGlobeControls::GlobeDir()
{
	assert__( IsDir( smGlobeDir ) );

	return smGlobeDir;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////
//		CGlobeControls Construction
///////////////////////////////////////////////////////////////////////////////////////////////////////////

#define ENABLE_HOME_BUTTON 1


template< typename CONTROL >
void CGlobeControls::RemoveControl( CONTROL *&c )
{
	mControlCanvas->removeControl( c );
	c = nullptr;
}


void CGlobeControls::RemoveControls()
{
	assert__( mControlCanvas );

	RemoveControl( mYawPitchWheel );
	RemoveControl( mRotateCCW );
	RemoveControl( mRotateCW );
	RemoveControl( mRotateReset );
	RemoveControl( mTiltUp );
	RemoveControl( mTiltDown );
	RemoveControl( mMoveWheel );
	RemoveControl( mMoveLeft );
	RemoveControl( mMoveRight );
	RemoveControl( mMoveUp );
	RemoveControl( mMoveDown );
	RemoveControl( mZoomHome );
	RemoveControl( mBackgroundGrp1 );
	RemoveControl( mZoomIn );
	RemoveControl( mZoomOut );
	RemoveControl( mBackgroundGrp2 );
	RemoveControl( mHome );
	RemoveControl( mRefresh );
	RemoveControl( mSync );
}



void CGlobeControls::AddControls()
{
	osgEarth::Util::EarthManipulator *manip = dynamic_cast< osgEarth::Util::EarthManipulator* >( mOsgViewer->getCameraManipulator() );

	assert__( manip );

	int imgLeft = 16;
	int imgTop = 20;

	if ( !mYawPitchWheel )
	{
		osg::Image* yawPitchWheelImg = osgDB::readImageFile( smGlobeDir + "/YawPitchWheel.png" );
		mYawPitchWheel = new ImageControl( yawPitchWheelImg );
		mYawPitchWheel->setPosition( imgLeft, imgTop );
	}
	mControlCanvas->addControl( mYawPitchWheel );

	//ROTATE CONTROLS
	if ( !mRotateCCW )
	{
		mRotateCCW = new NavigationControl();
		mRotateCCW->setHeight( 22 );
		mRotateCCW->setWidth( 20 );
		mRotateCCW->setPosition( imgLeft + 0, imgTop + 18 );
		mRotateCCW->addEventHandler( new RotateControlHandler( manip, MOVE_OFFSET, 0 ) );
	}
	mControlCanvas->addControl( mRotateCCW );

	if ( !mRotateCW )
	{
		mRotateCW = new NavigationControl();
		mRotateCW->setHeight( 22 );
		mRotateCW->setWidth( 20 );
		mRotateCW->setPosition( imgLeft + 36, imgTop + 18 );
		mRotateCW->addEventHandler( new RotateControlHandler( manip, -MOVE_OFFSET, 0 ) );
	}
	mControlCanvas->addControl( mRotateCW );

	if ( !mRotateReset )
	{
		//Rotate Reset
		mRotateReset = new NavigationControl();
		mRotateReset->setHeight( 22 );
		mRotateReset->setWidth( 16 );
		mRotateReset->setPosition( imgLeft + 20, imgTop + 18 );
		mRotateReset->addEventHandler( new RotateControlHandler( manip, 0, 0 ) );
	}
	mControlCanvas->addControl( mRotateReset );

	//TILT CONTROLS
	if ( !mTiltUp )
	{
		mTiltUp = new NavigationControl();
		mTiltUp->setHeight( 19 );
		mTiltUp->setWidth( 24 );
		mTiltUp->setPosition( imgLeft + 20, imgTop + 0 );
		mTiltUp->addEventHandler( new RotateControlHandler( manip, 0, MOVE_OFFSET ) );
	}
	mControlCanvas->addControl( mTiltUp );

	if ( !mTiltDown )
	{
		mTiltDown = new NavigationControl();
		mTiltDown->setHeight( 19 );
		mTiltDown->setWidth( 24 );
		mTiltDown->setPosition( imgLeft + 16, imgTop + 36 );
		mTiltDown->addEventHandler( new RotateControlHandler( manip, 0, -MOVE_OFFSET ) );
	}
	mControlCanvas->addControl( mTiltDown );

	// -------

	imgTop = 80;

	if ( !mMoveWheel )
	{
		osg::Image* moveWheelImg = osgDB::readImageFile( smGlobeDir + "/MoveWheel.png" );
		mMoveWheel = new ImageControl( moveWheelImg );
		mMoveWheel->setPosition( imgLeft, imgTop );
	}
	mControlCanvas->addControl( mMoveWheel );

	//MOVE CONTROLS
	if ( !mMoveLeft )
	{
		mMoveLeft = new NavigationControl();
		mMoveLeft->setHeight( 22 );
		mMoveLeft->setWidth( 20 );
		mMoveLeft->setPosition( imgLeft + 0, imgTop + 18 );
		mMoveLeft->addEventHandler( new PanControlHandler( manip, -MOVE_OFFSET, 0 ) );
	}
	mControlCanvas->addControl( mMoveLeft );

	if ( !mMoveRight )
	{
		mMoveRight = new NavigationControl();
		mMoveRight->setHeight( 22 );
		mMoveRight->setWidth( 20 );
		mMoveRight->setPosition( imgLeft + 36, imgTop + 18 );
		mMoveRight->addEventHandler( new PanControlHandler( manip, MOVE_OFFSET, 0 ) );
	}
	mControlCanvas->addControl( mMoveRight );

	if ( !mMoveUp )
	{
		mMoveUp = new NavigationControl();
		mMoveUp->setHeight( 19 );
		mMoveUp->setWidth( 24 );
		mMoveUp->setPosition( imgLeft + 20, imgTop + 0 );
		mMoveUp->addEventHandler( new PanControlHandler( manip, 0, MOVE_OFFSET ) );
	}
	mControlCanvas->addControl( mMoveUp );

	if ( !mMoveDown )
	{
		mMoveDown = new NavigationControl();
		mMoveDown->setHeight( 19 );
		mMoveDown->setWidth( 24 );
		mMoveDown->setPosition( imgLeft + 16, imgTop + 36 );
		mMoveDown->addEventHandler( new PanControlHandler( manip, 0, -MOVE_OFFSET ) );
	}
	mControlCanvas->addControl( mMoveDown );

	//Zoom Reset
	if ( !mZoomHome )
	{
		mZoomHome = new NavigationControl();
		mZoomHome->setHeight( 22 );
		mZoomHome->setWidth( 16 );
		mZoomHome->setPosition( imgLeft + 20, imgTop + 18 );
		mZoomHome->addEventHandler( new HomeControlHandler( manip ) );
	}
	mControlCanvas->addControl( mZoomHome );

	// -------

	osg::Image* backgroundImg = osgDB::readImageFile( smGlobeDir + "/button-background.png" );

	if ( !mBackgroundGrp1 )
	{
		mBackgroundGrp1 = new ImageControl( backgroundImg );
		imgTop = imgTop + 62;
		mBackgroundGrp1->setPosition( imgLeft + 12, imgTop );
	}
	mControlCanvas->addControl( mBackgroundGrp1 );

	if ( !mZoomIn )
	{
		osg::Image* plusImg = osgDB::readImageFile( smGlobeDir + "/zoom-in.png" );
		mZoomIn = new NavigationControl( plusImg );
		mZoomIn->setPosition( imgLeft + 12 + 3, imgTop + 3 );
		mZoomIn->addEventHandler( new ZoomControlHandler( manip, 0, -MOVE_OFFSET ) );
	}
	mControlCanvas->addControl( mZoomIn );

	if ( !mZoomOut )
	{
		osg::Image* minusImg = osgDB::readImageFile( smGlobeDir + "/zoom-out.png" );
		mZoomOut = new NavigationControl( minusImg );
		mZoomOut->setPosition( imgLeft + 12 + 3, imgTop + 3 + 23 + 2 );
		mZoomOut->addEventHandler( new ZoomControlHandler( manip, 0, MOVE_OFFSET ) );
	}
	mControlCanvas->addControl( mZoomOut );

	// -------

	if ( !mBackgroundGrp2 )
	{
		mBackgroundGrp2 = new ImageControl( backgroundImg );
		imgTop = imgTop + 60;
		mBackgroundGrp2->setPosition( imgLeft + 12, imgTop );
	}
	mControlCanvas->addControl( mBackgroundGrp2 );

	//Zoom Reset
#if ENABLE_HOME_BUTTON
	if ( !mHome )
	{
		osg::Image* homeImg = osgDB::readImageFile( smGlobeDir + "/zoom-home.png" );
		mHome = new NavigationControl( homeImg );
		mHome->setPosition( imgLeft + 12 + 3, imgTop + 2 );
		imgTop = imgTop + 23 + 2;
		mHome->addEventHandler( new HomeControlHandler( manip ) );
	}
	mControlCanvas->addControl( mHome );
#endif

	//refresh layers
	if ( !mRefresh )
	{
		osg::Image* refreshImg = osgDB::readImageFile( smGlobeDir + "/refresh-view.png" );
		mRefresh = new NavigationControl( refreshImg );
		mRefresh->setPosition( imgLeft + 12 + 3, imgTop + 3 );
		imgTop = imgTop + 23 + 2;
		mRefresh->addEventHandler( new RefreshControlHandler( mGlobe ) );
	}
	mControlCanvas->addControl( mRefresh );

	//Sync Extent
	if ( !mSync )
	{
		osg::Image* syncImg = osgDB::readImageFile( smGlobeDir + "/sync-extent.png" );
		mSync = new NavigationControl( syncImg );
		mSync->setPosition( imgLeft + 12 + 3, imgTop + 2 );
		mSync->addEventHandler( new SyncExtentControlHandler( mGlobe ) );
	}
	mControlCanvas->addControl( mSync );
}


CGlobeControls::CGlobeControls( CGlobePlugin *globe ) 
	: mGlobe( globe )
	, mOsgViewer( globe->osgViewer() )
	, mControlCanvas( ControlCanvas::get( mOsgViewer ) )		//OSGEARTH_VERSION_GREATER_OR_EQUAL( 2, 1, 1 )
{}


////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////
