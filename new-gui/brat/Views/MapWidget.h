#ifndef GRAPHICS_MAP_WIDGET_H
#define GRAPHICS_MAP_WIDGET_H

#include <cassert>

#include <QMessageBox>

#include <qgsmapcanvas.h>
#include <qgssinglesymbolrendererv2.h>


//class CmdLineProcessor;
//class CWorldPlotData;
typedef QList<QgsFeature> QgsFeatureList;
typedef QVector<QgsPoint> QgsPolyline;
class QgsRubberBand;
class QgsRasterLayer;

//
// http://foodforsamurai.com/post/483440483/git-to-jira
// https://github.com/joyjit/git-jira-hook
//
// https://sites.google.com/site/midvattenpluginforqgis/download/dependencies
//
// http://doc.qt.io/QtDataVisualization/index.html
// http://plplot.sourceforge.net/index.php
// http://www.qcustomplot.com/
//


class CMapWidget : public QgsMapCanvas
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

    using base_t = QgsMapCanvas;

	QgsVectorLayer *mMainLayer = nullptr;
	QgsRasterLayer *mMainRasterLayer = nullptr;
    QList <QgsMapCanvasLayer> mLayerSet;

	void setupDatabase();
	void setupMapLayers();
	QgsSymbolV2* createSymbol( double width, const QColor &lineColor, const std::string &arrowColor, 
		const std::string &status = "", const std::string &direction = "" );

	void setProjection();
	void drawLines();
	void addLabelsLayer();
	void addGrid();

public:
	template< class LAYER >
	static QgsSingleSymbolRendererV2* addRenderer( LAYER *layer, QgsSymbolV2* symbol = nullptr )
	{
		QgsSingleSymbolRendererV2 *renderer = new QgsSingleSymbolRendererV2( 
			symbol ?
			symbol : QgsSymbolV2::defaultSymbol(layer->geometryType() ) );

		layer->setRendererV2( renderer );

		return renderer;
	}
	
	static QgsSymbolV2* createPointSymbol( double width, const QColor &color );

	static QgsSymbolV2* createLineSymbol( double width, const QColor &color );

	static QgsFeatureList& createPointFeature( QgsFeatureList &list, double lon, double lat, double value );

	static QgsFeatureList& createLineFeature( QgsFeatureList &list, QgsPolyline points );


protected:
	virtual void keyPressEvent( QKeyEvent * e ) override;

	QgsVectorLayer* addLayer( const QString &layer_path, const QString &base_name, const QString &provider, QgsSymbolV2* symbol = nullptr );

	QgsVectorLayer* addMemoryLayer( QgsSymbolV2* symbol = nullptr );

	QgsVectorLayer* addOGRLayer( const QString &layer_path, QgsSymbolV2* symbol = nullptr )
	{
		return addLayer( layer_path, "ogr", "ogr", symbol );
	}
	QgsRasterLayer* addRasterLayer( const QString &layer_path, const QString &base_name, const QString &provider, QgsSymbolV2* symbol = nullptr );


	QgsRubberBand* addRBPoint( double lon, double lat, QColor color, QgsVectorLayer *layer = nullptr );
	QgsRubberBand* addRBLine( QgsPolyline points, QColor color, QgsVectorLayer *layer = nullptr );


public:
    CMapWidget( QWidget *parent = 0 );

	virtual ~CMapWidget();

    QSize sizeHint() const override
    {
        return QSize(72 * fontMetrics().width('x'),
                     25 * fontMetrics().lineSpacing());
    }

    //void CreateWPlot( const CmdLineProcessor *proc, CWPlot* wplot );
	//void AddData( CWorldPlotData* data );
};





#endif			//	GRAPHICS_MAP_WIDGET_H
