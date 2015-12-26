#ifndef GRAPHICS_GLOBE_WIDGET_H
#define GRAPHICS_GLOBE_WIDGET_H

class QgsMapCanvas;
class GlobePlugin;

namespace osgViewer { class Viewer; } 
namespace osgEarth { namespace QtGui { class ViewerWidget; } }


class CGlobeWidget : public QWidget
{

#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif


	using base_t = QWidget;

	osgEarth::QtGui::ViewerWidget *mGlobeViewerWidget = nullptr;
	osgViewer::Viewer *mOsgViewer = nullptr;
	GlobePlugin *mGlobe = nullptr;

private:
	osgEarth::QtGui::ViewerWidget* CreateViewerWidget( const std::string &globeDir, QgsMapCanvas *theCanvas = nullptr, QWidget *dialogParent = nullptr );

public:
	CGlobeWidget( QWidget *parent, const std::string &globeDir, QgsMapCanvas *theCanvas = nullptr );

	virtual ~CGlobeWidget();

    QSize sizeHint() const override
    {
        return QSize(72 * fontMetrics().width('x'),
                     25 * fontMetrics().lineSpacing());
    }

	//! Show the settings dialog box
	void settings();
};


#endif			//	GRAPHICS_GLOBE_WIDGET_H
