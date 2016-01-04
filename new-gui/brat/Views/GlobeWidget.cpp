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
#include "new-gui/brat/Views/Globe/Globe.h"

#include "GlobeWidget.h"


osgEarth::QtGui::ViewerWidget* CGlobeWidget::CreateViewerWidget( const std::string &globeDir, QgsMapCanvas *theCanvas, QWidget *dialogParent )		//theCanvas = nullptr, QWidget *dialogParent = nullptr 
{
    Q_UNUSED( dialogParent );

	mGlobe = new GlobePlugin( nullptr, theCanvas );
	mOsgViewer = mGlobe->run( globeDir );						//can return null
	return new osgEarth::QtGui::ViewerWidget( mOsgViewer );	
}


CGlobeWidget::CGlobeWidget( QWidget *parent, const std::string &globeDir, QgsMapCanvas *theCanvas )		//theCanvas = nullptr, QWidget *dialogParent = nullptr 
	: base_t( parent )
{
	mGlobeViewerWidget = CreateViewerWidget( globeDir, theCanvas, parent );
	osg::Camera* camera = mOsgViewer->getCamera();
	osgQt::GraphicsWindowQt *gw = dynamic_cast<osgQt::GraphicsWindowQt*>( camera->getGraphicsContext() );
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
}


//virtual 
CGlobeWidget::~CGlobeWidget()
{
	delete mGlobeViewerWidget;
	mGlobeViewerWidget = nullptr;
	delete mGlobe;
	mGlobe = nullptr;
}


void CGlobeWidget::settings()
{
	mGlobe->settings();
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_GlobeWidget.cpp"
