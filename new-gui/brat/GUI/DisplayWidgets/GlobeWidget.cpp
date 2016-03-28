#include "new-gui/brat/stdafx.h"

#if defined (WIN32) || defined(_WIN32)
#pragma warning ( disable: 4100 )           //unreferenced formal parameter
#endif

#include <osgViewer/Viewer>
#include <osgEarthQt/ViewerWidget>

#if defined (WIN32) || defined(_WIN32)
#pragma warning ( default: 4100 )           //unreferenced formal parameter
#endif


#include "new-gui/Common/QtUtils.h"
#include "GUI/DisplayWidgets/Globe/Globe.h"

#include "GlobeWidget.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		Globe statics
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void CGlobeWidget::SetOSGDirectories( const std::string &GlobeDir )
{
	CGlobePlugin::SetOSGDirectories( GlobeDir );
}



class CGlobeViewerWidget : public osgEarth::QtGui::ViewerWidget
{
	using base_t = osgEarth::QtGui::ViewerWidget;

public:
	CGlobeViewerWidget( osgViewer::ViewerBase *viewer )
		: base_t( viewer )
	{}
	
	virtual ~CGlobeViewerWidget()
	{
		Pause();
	}


	void Pause()
	{
		_timer.stop();
	}

	void Resume()
	{
		_timer.start(20);
	}

	bool IsPaused() const
	{
		return !_timer.isActive();
	}
};


void CGlobeWidget::CanvasStarted()
{
	mGlobeViewerWidget->Pause();
}
void CGlobeWidget::CanvasFinished()
{
	if ( mLayersChanging )
	{
		mGlobe->imageLayersChanged();
		mLayersChanging = false;
	}
	mGlobeViewerWidget->Resume();
	mLayersChanging = true;
}
void CGlobeWidget::ImageLayersChanged()
{
	mLayersChanging = true;
	bool paused = mGlobeViewerWidget->IsPaused();
	if (!paused)
		mGlobeViewerWidget->Pause();
	//if (!paused)
	//	mGlobeViewerWidget->Resume();
	//mLayersChanging = false;
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		Globe Construction
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


CGlobeWidget::CGlobeWidget( QWidget *parent, QgsMapCanvas *the_canvas, QStatusBar *sb )		//sb = nullptr 
	: base_t( parent )
{
	mGlobe = new CGlobePlugin( the_canvas, sb );
	mOsgViewer = mGlobe->osgViewer();										assert__( mOsgViewer );
	mGlobeViewerWidget = new CGlobeViewerWidget( mOsgViewer );	

	osg::Camera* camera = mOsgViewer->getCamera();
	osgQt::GraphicsWindowQt *gw = dynamic_cast< osgQt::GraphicsWindowQt* >( camera->getGraphicsContext() );
	gw->getGLWidget()->setMinimumSize( 10, 10 );	//CRITICAL: prevent image from disappearing forever when height is 0

	QSettings settings;

	if ( settings.value( "/Plugin-Globe/anti-aliasing", true ).toBool() )
	{
		QGLFormat glf = QGLFormat::defaultFormat();
		glf.setSampleBuffers( true );
		//bool aaLevelIsInt;
		//int aaLevel;
		//QString aaLevelStr = settings.value( "/Plugin-Globe/anti-aliasing-level", "" ).toString();
		//aaLevel = aaLevelStr.toInt( &aaLevelIsInt );
		//if ( aaLevelIsInt )
		//{
		//	glf.setSamples( aaLevel );
		//}
		mGlobeViewerWidget->setFormat( glf );
	}
	AddWidget( this, mGlobeViewerWidget );

	setMinimumSize( min_globe_widget_width, min_globe_widget_height );



	connect( the_canvas, SIGNAL( renderStarting() ), this, SLOT( CanvasStarted() ) );
	connect( the_canvas, SIGNAL( mapCanvasRefreshed() ), this, SLOT( CanvasFinished() ) );
	connect( the_canvas, SIGNAL( layersChanged() ), this, SLOT( ImageLayersChanged() ) );


	//context menu

	setContextMenuPolicy( Qt::ActionsContextMenu );

	mActionSettingsDialog = new QAction( "Settings Dialog...", this );
	connect( mActionSettingsDialog, SIGNAL( triggered() ), mGlobe, SLOT( settings() ) );
	addAction( mActionSettingsDialog );

	mActionSky = new QAction( "Sky", this );
	connect( mActionSky, SIGNAL( toggled( bool ) ), mGlobe, SLOT( SetSkyParameters( bool ) ) );
	mActionSky->setCheckable( true );
	mActionSky->setChecked( false );
	addAction( mActionSky );	

	mActionSetupControls = new QAction( "Globe Controls", this );
	connect( mActionSetupControls, SIGNAL( toggled( bool ) ), mGlobe, SLOT( SetupControls( bool ) ) );
	mActionSetupControls->setCheckable( true );
	mActionSetupControls->setChecked( false );
	addAction( mActionSetupControls );	
}


//virtual 
CGlobeWidget::~CGlobeWidget()
{
	delete mGlobeViewerWidget;
	mGlobeViewerWidget = nullptr;
	delete mGlobe;
	mGlobe = nullptr;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void CGlobeWidget::RemoveLayers()
{
	mGlobe->RemoveLayers();
}


void CGlobeWidget::HandleSettings()
{
	mGlobe->settings();
}
void CGlobeWidget::HandleSky( bool toggled )
{
	mGlobe->SetSkyParameters( toggled );
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_GlobeWidget.cpp"
