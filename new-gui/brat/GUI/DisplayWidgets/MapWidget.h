/*
* This file is part of BRAT 
*
* BRAT is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* BRAT is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
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
class QLookupTable;
struct CMapPlotParameters;
class CMapTip;



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

	friend class CMapTip;


	typedef QgsSymbolV2* (*create_symbol_t)( double width, const QColor &color );

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

	static const char* ReferenceDateFieldKey();

	static QString ExtractLayerBaseName( const QString &qname );

    static QgsVectorLayer* AskUserForOGRSublayers( QWidget *parent, QgsVectorLayer *layer );
    static bool OpenLayer( QWidget *parent, const QString &path, QgsRectangle &bounding_box );


	template< class LAYER >
	static QgsSingleSymbolRendererV2* CreateRenderer( LAYER *layer );
	
    static QgsFeatureList& CreatePointDataFeature( QgsFeatureList &list, double lon, double lat, double value);

    static QgsFeatureList& CreatePolygonDataFeature(QgsFeatureList &list, double lon, double lat, double value, double step_x, double step_y);

    static QgsFeatureList& CreateVectorFeature( QgsFeatureList &list, double lon, double lat, double angle, double magnitude );

protected:

	static QgsSingleSymbolRendererV2* CreateRenderer( QgsSymbolV2* symbol );

	static QgsGraduatedSymbolRendererV2* CreateRenderer( const QString &target_field, double width, double m, double M, const QLookupTable *lut, create_symbol_t cs );

	static void ChangeRenderer( QgsVectorLayer *layer, const QString &target_field, double width, double m, double M, const QLookupTable *lut, create_symbol_t cs );


    static QgsSymbolV2* CreatePolygonSymbol( double width, const QColor &color );
	
    static QgsSymbolV2* CreatePointSymbol( double width, const QColor &color );

	static QgsSymbolV2* CreateLineSymbol( double width, const QColor &color );

    ///////////////// TODO _ RCCC //////////////////////////////////////////
    static QgsSymbolV2* CreateArrowSymbol(const QColor &color );
    /////////////////////////////////////////////////////////////////////////


protected:
    //////////////////////////////////////
    //	instance data
    //////////////////////////////////////

	QgsMapLayer *mMainLayer = nullptr;
    ELayerBaseType mLayerBaseType = eVectorLayer;
	QgsVectorLayer *mTracksLayer = nullptr;
	QgsVectorLayer *mCurrentDataLayer = nullptr;		//last added that is visible
	std::vector< QgsVectorLayer* > mDataLayers;
	std::vector< QgsVectorLayer* > mContourLayers;
	QgsRasterLayer *mMainRasterLayer = nullptr;
    QList <QgsMapCanvasLayer> mLayerSet;

	QgsMapTool *mSelectFeatures = nullptr;
	QgsMapTool *mSelectPolygon = nullptr;

	QgsMapTool *mMeasureDistance = nullptr;
	QgsMapTool *mMeasureArea = nullptr;
	QAction *mActionDecorationGrid = nullptr;	//we need this reference to parent action for enabling/disabling
	CDecorationGrid *mDecorationGrid = nullptr;

    // TODO: RCCC - Create Map Tips. ///////////////////////////
    // Maptip object
    CMapTip *mpMaptip = nullptr;
    // Point of last mouse position in map coordinates (used with MapTips)
    QgsPoint mLastMapPosition;

    QAction *mActionMapTips = nullptr;			//we need this reference to parent action for enabling/disabling
    bool mMapTipsVisible = false;
    QTimer mpMapTipsTimer;


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

	static QAction* CreateMapTipsAction( QToolBar *tb );
	void ConnectParentMapTipsAction( QAction *action_map_tips );

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


	bool IsDataLayerVisible( size_t index ) const;
	bool IsContourLayerVisible( size_t index ) const;
	bool SetDataLayerVisible( size_t index, bool show, bool render );
	bool SetContourLayerVisible( size_t index, bool show, bool render );
	bool HasContourLayer( size_t index ) const;

	void ChangeDataRenderer( size_t index, double width, double m, double M, const QLookupTable *lut );


	QgsRectangle CurrentLayerSelectedBox() const;

	void RemoveLayers( bool render = false );
	
	void RemoveLayer( QgsMapLayer *layer, bool render = false );

	void RemoveTracksLayer();
	

	void Home();

	bool Save2Image( const QString &path, const QString &format, const QString &extension );


    void SetNumberOfContours( size_t index, size_t ncontours, const CMapPlotParameters &map );

    void SetContoursProperties( size_t index, QColor color, unsigned width );


	QgsCoordinateReferenceSystem DefaultProjection() const { return mDefaultProjection; }

	bool SetDefaultProjection();

	bool SetProjection( unsigned id );



	QgsVectorLayer* AddPolygonDataLayer( const std::string &name, double m, double M, const QLookupTable *lut, QgsFeatureList &flist )
	{
		return AddDataLayer( true, name, 0., m, M, lut, flist );
	}

	QgsVectorLayer* AddDataLayer( const std::string &name, double symbol_width, double m, double M, const QLookupTable *lut, QgsFeatureList &flist,
		bool isdate, brathl_refDate date_ref );
    
	QgsVectorLayer* AddContourLayer( size_t index, const std::string &name, unsigned width, QColor color, const QLookupTable *lut, size_t ncontours, const CMapPlotParameters &map );

    QgsVectorLayer* AddArrowDataLayer(const std::string &name, QgsFeatureList &flist );

    void PlotTrack( const double *x, const double *y, const double *z, size_t size, brathl_refDate ref_date, QColor color = Qt::red );


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

	void SetLayerSet();

	void SetMainLayerVisible( bool show );

	bool IsLayerVisible( QgsMapLayer *layer ) const;
	bool SetLayerVisible( QgsVectorLayer *layer, bool show, bool render );

	void SetCurrentLayer( QgsMapLayer *l );	

	QgsMapCanvasLayer* FindCanvasLayer( QgsMapLayer *layer );	
	const QgsMapCanvasLayer* FindCanvasLayer( QgsMapLayer *layer ) const
	{
        return const_cast<CMapWidget*>( this )->FindCanvasLayer( layer );
	}

	const QgsMapCanvasLayer* FindCanvasLayer( const std::string &name ) const
	{
        return const_cast<CMapWidget*>( this )->FindCanvasLayer( name );
	}
	

	bool SetProjection( const QgsCoordinateReferenceSystem &proj );

	QgsVectorLayer* AddDataLayer( bool polygon, const std::string &name, double width, double m, double M, const QLookupTable *lut, QgsFeatureList &flist );


protected:

	//QgsVectorLayer* AddVectorLayer( const std::string &name, const QString &layer_path, const QString &provider, QgsFeatureRendererV2 *renderer = nullptr );
	QgsVectorLayer* AddVectorLayer( QgsVectorLayer *l );

	//QgsVectorLayer* AddMemoryLayer( bool polygon, const std::string &name, QgsFeatureRendererV2 *renderer );
	//QgsVectorLayer* AddMemoryLayer( const std::string &name = "", QgsSymbolV2* symbol = nullptr );


	QgsVectorLayer* AddOGRVectorLayer( const QString &layer_path, QgsSymbolV2* symbol = nullptr );


	QgsRasterLayer* AddRasterLayer( const QString &layer_path, const QString &base_name, const QString &provider, QgsSymbolV2* symbol = nullptr );


	QgsRubberBand* AddRBPoint( double lon, double lat, QColor color, QgsVectorLayer *layer = nullptr );
	QgsRubberBand* AddRBLine( QgsPolyline points, QColor color, QgsVectorLayer *layer = nullptr );

	void DebugWriteTrackValue( QgsRectangle rect );
	void DebugWriteDataValue( QgsRectangle rect );

protected:

    virtual void closeEvent( QCloseEvent *event ) override;

signals:
	void CurrentLayerSelectionChanged();
	void GridEnabled( bool enabled );


public slots:

	void ShowMouseCoordinate( const QgsPoint & p, bool erase = false );
	void ShowMouseCoordinate( const QString s, bool erase = false );
	void ShowMouseDegreeCoordinates( const QgsPoint & p, bool erase = false );

protected slots:

	void EnableRectangularSelection( bool enable );
	void EnablePolygonalSelection( bool enable );
	void DeselectAll();

	void MeasureDistance( bool enable );
	void MeasureArea( bool enable );

	void ToolButtonTriggered( QAction *action );

	void ToggleGridEnabled( bool toggle );

    void ToggleMapTips( bool checked );
    void ShowMapTip();

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
