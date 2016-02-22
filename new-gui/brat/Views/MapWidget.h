#ifndef GRAPHICS_MAP_WIDGET_H
#define GRAPHICS_MAP_WIDGET_H

#include <cassert>

#include <QMessageBox>

#include <qgsmapcanvas.h>
#include <qgssinglesymbolrendererv2.h>



typedef QList<QgsFeature> QgsFeatureList;
typedef QVector<QgsPoint> QgsPolyline;
class QgsRubberBand;
class QgsRasterLayer;
class CDecorationGrid;


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


//	ShapeFiles
//
// Tiago - I
//	http://www.naturalearthdata.com/downloads/10m-raster-data/10m-cross-blend-hypso/
//	http://www.naturalearthdata.com/tag/world-file/
//	http://www.bluemarblegeo.com/products/world-map-data.php
//	http://www.gadm.org/version2

// Tiago - II
//	http://www.gadm.org/
//	http://ec.europa.eu/eurostat/web/gisco/overview
//	http://www.statsilk.com/maps/download-free-shapefile-maps
//	http://cloudmade.com/






class CMapWidget : public QgsMapCanvas
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

	Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

    //////////////////////////////////////
    //	types & friends
    //////////////////////////////////////

    using base_t = QgsMapCanvas;


    //////////////////////////////////////
    //	static members
    //////////////////////////////////////

	static std::string smQgisPluginsDir;
	static std::string smVectorLayerPath;
	static std::string smRasterLayerPath;
	static std::string smGlobeDir;

public:

	static const std::string& QgisPluginsDir();
	static const std::string& VectorLayerPath();
	static const std::string& RasterLayerPath();	//not required, used if exists on disk
	static const std::string& GlobeDir();

	// Must be called at startup, before 1st map creation
	//
	static void SetQGISDirectories( 
		const std::string &QgisPluginsDir, 
		const std::string &VectorLayerPath, 
		const std::string &RasterLayerPath,
		const std::string &GlobeDir );

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
    //////////////////////////////////////
    //	instance data
    //////////////////////////////////////

	QgsVectorLayer *mMainLayer = nullptr;
	QgsRasterLayer *mMainRasterLayer = nullptr;
    QList <QgsMapCanvasLayer> mLayerSet;

	QgsMapTool *mSelectFeatures = nullptr;
	QAction *mActionSelectFeatures = nullptr;
	QgsMapTool *mSelectPolygon = nullptr;
	QAction *mActionSelectPolygon = nullptr;
	QAction *mActionDeselectAll = nullptr;
	QToolButton *mSelectionButton = nullptr;

	QgsMapTool *mMeasureDistance = nullptr;
	QAction *mActionMeasure = nullptr;
	QgsMapTool *mMeasureArea = nullptr;
	QAction *mActionMeasureArea = nullptr;
	QToolButton *mMeasureButton = nullptr;

	QProgressBar *mProgressBar = nullptr;
	QCheckBox *mRenderSuppressionCBox = nullptr;

	CDecorationGrid* mDecorationGrid = nullptr;
	QAction *mActionDecorationGrid = nullptr;


    //////////////////////////////////////
    //	construction / destruction
    //////////////////////////////////////


	void setupDatabase();		//experimental
	void setupMapLayers();		//experimental
	QgsSymbolV2* createSymbol( double width, const QColor &lineColor, const std::string &arrowColor, const std::string &status = "", 
		const std::string &direction = "" );		//experimental
	void drawLines();			//experimental
	void addLabelsLayer();		//experimental
	void addGrid();				//experimental

	void setProjection();
	void CreateMeasureWidgets( QToolBar *tb );
	void CreateSelectionWidgets( QToolBar *tb );
	void CreateGridWidgets( QToolBar *tb );

public:
    CMapWidget( QWidget *parent = nullptr );

	virtual ~CMapWidget();


    //////////////////////////////////////
	// access instance data
    //////////////////////////////////////
	
	//QAction* ActionSelectFeatures() { return mActionSelectFeatures; }
	//QAction* ActionSelectPolygon() { return mActionSelectPolygon; }
	//QToolButton *SelectionButton() { return mSelectionButton; }
	//QAction* ActionDeselectAll() { return mActionDeselectAll; }

	//QAction* ActionMeasure() { return mActionMeasure; }
	//QAction* ActionMeasureArea() { return  mActionMeasureArea; }
	//QToolButton *MeasureButton() { return mMeasureButton; }


    //////////////////////////////////////
    //	GUI operations
    //////////////////////////////////////

	void InsertMapSelectionActions( QToolBar *tb, QAction *before, bool with_separator );
	void AddMapSelectionActions( QToolBar *tb, bool with_separator );

	void InsertMapMeasureActions( QToolBar *tb, QAction *before, bool with_separator );
	void AddMapMeasureActions( QToolBar *tb, bool with_separator );

	void InsertGridActions( QToolBar *tb, QAction *before, bool with_separator );
	void AddGridActions( QToolBar *tb, bool with_separator );

	void InsertStatusBarMapWidgets( QStatusBar *bar, int index );


    //////////////////////////////////////
    //	operations
    //////////////////////////////////////

	QgsRectangle CurrentLayerSelectedBox() const;

	void RemoveLayers();


protected:

    //////////////////////////////////////
    //	overrides
    //////////////////////////////////////

    QSize sizeHint() const override
    {
        return QSize(72 * fontMetrics().width('x'),
                     25 * fontMetrics().lineSpacing());
    }

	virtual void keyPressEvent( QKeyEvent * e ) override;

    //////////////////////////////////////
    //	protected layer operations
    //////////////////////////////////////

	void SetCurrentLayer( QgsMapLayer *l );	

	QgsVectorLayer* AddVectorLayer( const QString &layer_path, const QString &base_name, const QString &provider, QgsSymbolV2* symbol = nullptr );

	QgsVectorLayer* AddMemoryLayer( QgsSymbolV2* symbol = nullptr );

	QgsVectorLayer* AddOGRVectorLayer( const QString &layer_path, QgsSymbolV2* symbol = nullptr )
	{
		return AddVectorLayer( layer_path, "ogr", "ogr", symbol );
	}
	QgsRasterLayer* AddRasterLayer( const QString &layer_path, const QString &base_name, const QString &provider, QgsSymbolV2* symbol = nullptr );


	QgsRubberBand* AddRBPoint( double lon, double lat, QColor color, QgsVectorLayer *layer = nullptr );
	QgsRubberBand* AddRBLine( QgsPolyline points, QColor color, QgsVectorLayer *layer = nullptr );

signals:
	void CurrentLayerSelectionChanged();

public slots:

protected slots:

	void EnableRectangularSelection( bool enable );
	void EnablePolygonalSelection( bool enable );
	void DeselectAll();

	void MeasureDistance( bool enable );
	void MeasureArea( bool enable );

	void ToolButtonTriggered( QAction *action );

	void ToggleGridEnabled( bool toggle );

	void ShowProgress( int theProgress, int theTotalSteps );
	void RefreshStarted();
	void RefreshFinished();
};







#endif			//	GRAPHICS_MAP_WIDGET_H
