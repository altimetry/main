#ifndef GUI_DISPLAY_WIDGETS_GLOBE_WIDGET_H
#define GUI_DISPLAY_WIDGETS_GLOBE_WIDGET_H

class QgsMapCanvas;
class CGlobePlugin;
class CGlobeViewerWidget;

namespace osgViewer { class Viewer; } 
namespace osgEarth { namespace QtGui { class ViewerWidget; } }


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

	//static members

public:
	// Must be called at startup, before 1st globe creation
	//
	static void SetOSGDirectories( const std::string &GlobeDir );

protected:

	//instance data

	CGlobeViewerWidget *mGlobeViewerWidget = nullptr;
	osgViewer::Viewer *mOsgViewer = nullptr;
	CGlobePlugin *mGlobe = nullptr;
	bool mLayersChanging = false;

	//...context menu
	//
	QAction *mActionSettingsDialog = nullptr;
	QAction *mActionSky = nullptr;
	QAction *mActionSetupControls = nullptr;


	//construction / destruction

public:
	CGlobeWidget( QWidget *parent, QgsMapCanvas *the_canvas, QStatusBar *sb = nullptr );

	virtual ~CGlobeWidget();

	//remaining

	void RemoveLayers();


    QSize sizeHint() const override
    {
        return QSize(72 * fontMetrics().width('x'),
                     25 * fontMetrics().lineSpacing());
    }

public slots:

	void HandleSettings();

	void CanvasStarted();
	void CanvasFinished();
	void ImageLayersChanged();


protected slots:

	void HandleSky( bool toggled );
};


#endif			//	GUI_DISPLAY_WIDGETS_GLOBE_WIDGET_H
