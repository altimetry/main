#ifndef GUI_DISPLAY_WIDGETS_MAP_WIDGET_H
#define GUI_DISPLAY_WIDGETS_MAP_WIDGET_H

#include <cassert>

#include <QMessageBox>

#if defined(OSG_GLDEFINES)	//avoid collisions of OPENGL macro definitions by OSG and QGIS
#define QGLSHADERPROGRAM_H
#endif

#include <qgsmapcanvas.h>



typedef QList<QgsFeature> QgsFeatureList;
typedef QVector<QgsPoint> QgsPolyline;
class QgsRubberBand;
class QgsRasterLayer;
class CDecorationGrid;
class QgsGraduatedSymbolRendererV2;

// TODO RCCC
class QgsMapTip;



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


//qgis.utils.iface.addRasterLayer("http://server.arcgisonline.com/arcgis/rest/services/ESRI_Imagery_World_2D/MapServer?f=json&pretty=true","raster")




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

public:

    enum ELayerBaseType
    {
        eVectorLayer,
        eRasterLayer,
        eRasterURL
    };


protected:

    //////////////////////////////////////
    //	static members
    //////////////////////////////////////

	//directories

	static std::string smQgisPluginsDir;
	static std::string smVectorLayerPath;
	static std::string smRasterLayerPath;
    static std::string smURLRasterLayerPath;

public:

	static const std::string& QgisPluginsDir();
	static const std::string& VectorLayerPath();
    static const std::string& RasterLayerPath();
    static const std::string& URLRasterLayerPath();

	// Must be called at startup, before 1st map creation
	//
	static void SetQGISDirectories( 
		const std::string &QgisPluginsDir, 
		const std::string &VectorLayerPath, 
        const std::string &RasterLayerPath,
        const std::string &URLRasterLayerPath );


	// See parent widget creation helpers near instance methods for 
	//	parent actions connection with this


public:
	template< class LAYER >
	static QgsSingleSymbolRendererV2* CreateRenderer( LAYER *layer );
	
	static QgsSingleSymbolRendererV2* CreateRenderer( QgsSymbolV2* symbol );

	static QgsGraduatedSymbolRendererV2* CreateRenderer( const QString &target_field, double width, double m, double M, size_t contours );

    ///////////////// TODO _ RCCC //////////////////////////////////////////
    static QgsGraduatedSymbolRendererV2* CreateRenderer_polygon( const QString &target_field, double m, double M, size_t contours );

    static QgsSymbolV2* CreatePolygonSymbol( const QColor &color );
    /////////////////////////////////////////////////////////////////////////
	
    static QgsSymbolV2* CreatePointSymbol( double width, const QColor &color );

	static QgsSymbolV2* createLineSymbol( double width, const QColor &color );

    static QgsFeatureList& CreatePointFeature( QgsFeatureList &list, double lon, double lat, const std::map<QString, QVariant> &attrs = std::map<QString, QVariant>() );

	static QgsFeatureList& CreatePointFeature( QgsFeatureList &list, double lon, double lat, double value );

    ///////////////// TODO _ RCCC //////////////////////////////////////////
    static QgsFeatureList& CreatePolygonFeature( QgsFeatureList &list, double lon, double lat, const std::map<QString, QVariant> &attrs = std::map<QString, QVariant>() );

    static QgsFeatureList& CreatePolygonFeature( QgsFeatureList &list, double lon, double lat, double value );
    /////////////////////////////////////////////////////////////////////////

	static QgsFeatureList& createLineFeature( QgsFeatureList &list, QgsPolyline points );


protected:
    //////////////////////////////////////
    //	instance data
    //////////////////////////////////////

	QgsMapLayer *mMainLayer = nullptr;
    ELayerBaseType mLayerBaseType = eVectorLayer;
	QgsVectorLayer *mTracksLayer = nullptr;
	std::vector< QgsVectorLayer* > mDataLayers;
	QgsRasterLayer *mMainRasterLayer = nullptr;
    QList <QgsMapCanvasLayer> mLayerSet;

	QgsMapTool *mSelectFeatures = nullptr;
	QgsMapTool *mSelectPolygon = nullptr;

	QgsMapTool *mMeasureDistance = nullptr;
	QgsMapTool *mMeasureArea = nullptr;
	QAction *mActionDecorationGrid = nullptr;	//we need this reference to parent action for enabling/disabling
	CDecorationGrid* mDecorationGrid = nullptr;

	//reference to parent widgets
	//
	QProgressBar *mProgressBar = nullptr;
	QCheckBox *mRenderSuppressionCBox = nullptr;
	QLineEdit *mCoordsEdit = nullptr;
	unsigned int mMousePrecisionDecimalPlaces = 0;
	QToolButton *mCoordinatesFormatButton = nullptr;
	QString mCoordinatesFormat;
	QAction *mActionDM = nullptr;
	QAction *mActionDMS = nullptr;
	QAction *mActionDecimal = nullptr;



	//...context menu
	//
	QAction *mActionAddRaster = nullptr;
	QAction *mActionRemoveLayers = nullptr;


	//...domain

	QgsCoordinateReferenceSystem mDefaultProjection;


    // TODO: RCCC - Create Map Tips. ///////////////////////////
    // Timer for map tips
    QTimer *mpMapTipsTimer = nullptr;
    // Maptip object
    QgsMapTip *mpMaptip = nullptr;
    // Point of last mouse position in map coordinates (used with MapTips)
    QgsPoint mLastMapPosition;
    // Flag to indicate if maptips are on or off
    bool mMapTipsVisible;
    /////////////////////////////////////////////////////////////


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
	void setProjectionExp();	//experimental
	//void CopyStyle( QgsMapLayer * sourceLayer );
	//void PasteStyle( QgsMapLayer * destinationLayer );
	QgsMapLayer* DuplicateLayer( QgsMapLayer *layer );


	void CreateSelectionWidgets( QToolBar *tb );
	void CreateGridWidgets( QToolBar *tb );
	void Init();		
public:
    CMapWidget( QWidget *parent );
    explicit CMapWidget( ELayerBaseType layer_base_type , QWidget *parent );

	virtual ~CMapWidget();


    //////////////////////////////////////
    //	GUI operations
    //////////////////////////////////////

	static QToolButton* CreateMapSelectionActions( QToolBar *tb, QAction *&action_select_features, QAction *&action_select_polygon, QAction *&action_deselect_all );
	void ConnectParentSelectionActions( QToolButton *selection_button, QAction *action_select_features, QAction *action_select_polygon, QAction *action_deselect_all );

	static QToolButton* CreateMapMeasureActions( QToolBar *tb, QAction *&action_measure_distance, QAction *&action_measure_area );
	void ConnectParentMeasureActions( QToolButton *measure_button, QAction *action_measure_distance, QAction *action_measure_area );

	static QAction* CreateGridAction( QToolBar *tb );
	void ConnectParentGridAction( QAction *action_grid );

	static void CreateRenderWidgets( QStatusBar *bar, QProgressBar *&progress, QCheckBox *&box );
	void ConnectParentRenderWidgets( QProgressBar *bar, QCheckBox *box );

	// Coordinates format possible values: "DM", "DMS", "Decimal"
	//
	static void CreateCoordinatesWidget( QStatusBar *bar, QLineEdit *&coords, QToolButton *&format_button );
	void ConnectCoordinatesWidget( QLineEdit *&coords, QToolButton *format_button, const QString &format = "Decimal" );


	const QString& CoordinatesFormat() const { return mCoordinatesFormat; }


    //////////////////////////////////////
    //	public operations
    //////////////////////////////////////


	bool IsLayerVisible( size_t index ) const;
	bool SetLayerVisible( size_t index, bool show );


	QgsRectangle CurrentLayerSelectedBox() const;

	void RemoveLayers( bool render = false );
	
	void RemoveLayer( QgsMapLayer *layer, bool render = false );

	void RemoveTracksLayer();
	

	void Home();

	QgsCoordinateReferenceSystem DefaultProjection() const { return mDefaultProjection; }

	bool SetDefaultProjection();

	bool SetProjection( unsigned id );

    void PlotTrack( const double *x, const double *y, const double *z, size_t size, brathl_refDate ref_date, QColor color = Qt::red );


    // TODO - RCCC Map tips    //////////////
    void createMapTips();

    // Toggle map tips on/off
    void toggleMapTips();
    /////////////////////////////////////////


protected:
	void SetLayerSet();

	void SetMainLayerVisible( bool show );

	bool IsLayerVisible( QgsVectorLayer *layer ) const;


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

	QgsMapCanvasLayer* FindCanvasLayer( QgsMapLayer *layer );	
	const QgsMapCanvasLayer* FindCanvasLayer( QgsMapLayer *layer ) const
	{
        return const_cast<CMapWidget*>( this )->FindCanvasLayer( layer );
	}

	QgsMapCanvasLayer* FindCanvasLayer( size_t index );	
	const QgsMapCanvasLayer* FindCanvasLayer( size_t index ) const
	{
        return const_cast<CMapWidget*>( this )->FindCanvasLayer( index );
	}

	const QgsMapCanvasLayer* FindCanvasLayer( const std::string &name ) const
	{
        return const_cast<CMapWidget*>( this )->FindCanvasLayer( name );
	}
	

	bool SetProjection( const QgsCoordinateReferenceSystem &proj );

	QgsVectorLayer* AddDataLayer( const std::string &name, double width, double m, double M, size_t contours, QgsFeatureList &flist );

    //////////////////////////////// TODO - RCCC /////////////////////////////////////
    QgsVectorLayer* AddDataLayer_Polygon(const std::string &name, double m, double M, size_t contours, QgsFeatureList &flist );
    ///////////////////////////////////////////////////////////////////////////////////

protected:

	QgsVectorLayer* AddVectorLayer( const std::string &name, const QString &layer_path, const QString &provider, QgsFeatureRendererV2 *renderer = nullptr );

	QgsVectorLayer* AddMemoryLayer( const std::string &name, QgsFeatureRendererV2 *renderer );
	QgsVectorLayer* AddMemoryLayer( const std::string &name = "", QgsSymbolV2* symbol = nullptr );

    //////////////////////////////// TODO - RCCC /////////////////////////////////////
    QgsVectorLayer* AddMemoryLayer_polygon( const std::string &name, QgsFeatureRendererV2 *renderer );
    ///////////////////////////////////////////////////////////////////////////////////

	QgsVectorLayer* AddOGRVectorLayer( const QString &layer_path, QgsSymbolV2* symbol = nullptr );


	QgsRasterLayer* AddRasterLayer( const QString &layer_path, const QString &base_name, const QString &provider, QgsSymbolV2* symbol = nullptr );


	QgsRubberBand* AddRBPoint( double lon, double lat, QColor color, QgsVectorLayer *layer = nullptr );
	QgsRubberBand* AddRBLine( QgsPolyline points, QColor color, QgsVectorLayer *layer = nullptr );

	void WriteTrackValue( QgsRectangle rect );
	void WriteDataValue( QgsRectangle rect );

protected:

    virtual void closeEvent( QCloseEvent *event ) override;

signals:
	void CurrentLayerSelectionChanged();
	void GridEnabled( bool enabled );


public slots:

	void ShowMouseCoordinate( const QgsPoint & p, bool erase = false );
	void ShowMouseCoordinate( const QString s, bool erase = false );
	void ShowMouseDegreeCoordinates( const QgsPoint & p, bool erase = false );

    // TODO - RCCC Map tips    //////////////
    // Show the maptip using tooltip
    void showMapTip();
    /////////////////////////////////////////

protected slots:

	void EnableRectangularSelection( bool enable );
	void EnablePolygonalSelection( bool enable );
	void DeselectAll();

	void MeasureDistance( bool enable );
	void MeasureArea( bool enable );

	void ToolButtonTriggered( QAction *action );

	void ToggleGridEnabled( bool toggle );

	//parent's status-bar

	void ShowProgress( int theProgress, int theTotalSteps );
	void RefreshStarted();
	void RefreshFinished();

	//void ShowMouseCoordinate( const QgsPoint & p );		see above
	void UpdateMouseCoordinatePrecision();
	void CoordinatesFormatChanged();

	//context menu

	void HandleAddRaster();
	void HandleRemoveLayers();
};







#endif			//	GUI_DISPLAY_WIDGETS_MAP_WIDGET_H
