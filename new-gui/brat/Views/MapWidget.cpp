#include "new-gui/brat/stdafx.h"

#include <qgsproviderregistry.h>
#include <qgsmaplayerregistry.h>
#include <qgsvectorlayer.h>

#include <qgsrubberband.h>

#include <qgslinesymbollayerv2.h>
#include <qgsmarkersymbollayerv2.h>

// for addMemoryLayer
#include <qgsgraduatedsymbolrendererv2.h>
#include <qgssinglebandpseudocolorrenderer.h>
#include <qgsrastershader.h>

// for selection tools
#include <qgscursors.h>
#include "QGISapp/MapToolSelectUtils.h"
#include "QGISapp/MapToolSelectRectangle.h"
#include "QGISapp/MapToolSelectPolygon.h"
#include "QGISapp/MapToolMeasure.h"

// for grid
#include "QGISapp/MapDecorationGrid.h"


// from brat
#include "new-gui/Common/+UtilsIO.h"
#include "new-gui/Common/QtUtils.h"
#include "new-gui/Common/tools/Trace.h"

#include "new-gui/brat/BratSettings.h"
#include "new-gui/brat/GUI/ActionsTable.h"

#include "MapWidget.h"




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		Map statics
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//static 
std::string CMapWidget::smQgisPluginsDir;
//static 
std::string CMapWidget::smVectorLayerPath;
//static 
std::string CMapWidget::smRasterLayerPath;
//static 
std::string CMapWidget::smGlobeDir;


void CMapWidget::SetQGISDirectories( 
	const std::string &QgisPluginsDir, 
	const std::string &VectorLayerPath, 
	const std::string &RasterLayerPath,
	const std::string &GlobeDir )
{
	smQgisPluginsDir = QgisPluginsDir;
	smVectorLayerPath = VectorLayerPath;
	smRasterLayerPath = RasterLayerPath;
	smGlobeDir = GlobeDir;
}


//static 
const std::string& CMapWidget::QgisPluginsDir()
{
	assert__( IsDir( smQgisPluginsDir ) );

	return smQgisPluginsDir;
}
//static 
const std::string& CMapWidget::VectorLayerPath()
{
	assert__( IsDir( smVectorLayerPath ) );

	return smVectorLayerPath;
}
//static 
const std::string& CMapWidget::RasterLayerPath()
{
	//no assert, not mandatory

	return smRasterLayerPath;
}
//static 
const std::string& CMapWidget::GlobeDir()
{
	assert__( IsDir( smGlobeDir ) );

	return smGlobeDir;
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Construction / Destruction
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



CMapWidget::CMapWidget( QWidget *parent )	//parent = nullptr
	: base_t(parent)
{
	//char *argv[] =
	//{
	//	"",
	//	"L:\\project\\dev\\source\\data\\stuff\\gshhs_l.b",
	//	//"L:\\project\\dev\\source\\data\\stuff\\gshhs_l.shp",
	//};

	//const int argc = sizeof(argv) / sizeof(*argv);

	//if ( !gshhs( argc, argv ) )
	//	std::cout << "error" << std::endl;

    //connect( document(), SIGNAL(modificationChanged(bool)), this, SLOT(setWindowModified(bool)));
    //if ( qobject_cast< QMainWindow* >( parent ) )
    //{
    //    //assert( isMainChild() );
    //    connect( this, SIGNAL( setCurrentFile( const QbrtMapEditor* )), parent, SLOT(setCurrentFile( const QbrtMapEditor* )) );
    //}

    QString myLayerBaseName     = "italy";
    QString myProviderName      = "ogr";

    // Instantiate Provider Registry
    QgsProviderRegistry *preg = QgsProviderRegistry::instance( t2q( smQgisPluginsDir ) );
	Q_UNUSED( preg );

	assert__( IsFile( smVectorLayerPath ) );
	//QString source, dest;
	//if ( !readFileFromResource( ":/maps/ne_10m_coastline/ne_10m_coastline.shp", dest, true ) )
	//	std::cout << "problem" << std::endl;
	//else
	//	qDebug() << dest;
		
	mMainLayer = AddOGRVectorLayer( t2q( smVectorLayerPath ) );
	mMainLayer->rendererV2()->symbols()[ 0 ]->setColor( "black" );

#if defined(Q_OS_WIN)
    if ( true )
	{
		if ( IsFile( smRasterLayerPath ) )
			mMainRasterLayer = AddRasterLayer( t2q( smRasterLayerPath ), "raster", "" );
	}
#endif


    //mMainLayer =  new QgsVectorLayer(mVectorLayerPath, myLayerBaseName, myProviderName);
    //QgsSingleSymbolRendererV2 *mypRenderer = new QgsSingleSymbolRendererV2(QgsSymbolV2::defaultSymbol(mMainLayer->geometryType()));

    //mMainLayer->setRendererV2(mypRenderer);    mMainLayer->isValid() ? qDebug("Layer is valid") : qDebug("Layer is NOT valid");

    //// Add the Vector Layer to the Layer Registry
    //QgsMapLayerRegistry::instance()->addMapLayer(mMainLayer, TRUE);
    //// Add the Layer to the Layer Set
    //mLayerSet.append(QgsMapCanvasLayer(mMainLayer, TRUE));

	//setupDatabase();
	//setupMapLayers();

	//4. We need the index for the population value:
	//i = tractLyr.fieldNameIndex('POPULAT11')
	//5. Now, we get our census layer's features as an iterator:
	//features = tractLyr.getFeatures()
	//6. We need a data provider for the memory layer so that we can edit it:
	
	//auto vpr = popLyr->dataProvider();

    setExtent(mMainLayer->extent());
    enableAntiAliasing(true);
    setCanvasColor(QColor(255, 255, 255));
    freeze(false);

    //// Set the Map Canvas Layer Set
    //setLayerSet(mLayerSet);

    setMinimumSize( min_globe_widget_width, min_globe_widget_height );    //setVisible(true);
    refresh();
}



//virtual
CMapWidget::~CMapWidget()
{
	assert( !isDrawing() );

	delete mSelectFeatures;
	delete mSelectPolygon;
	delete mMeasureDistance;
	delete mMeasureArea;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		Lines & Points
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


QgsRubberBand* CMapWidget::AddRBPoint( double lon, double lat, QColor color, QgsVectorLayer *layer )	//layer = nullptr  
{
	QgsRubberBand *r = new QgsRubberBand( this, QGis::Point );
	r->setToGeometry( QgsGeometry::fromPoint( QgsPoint( lon, lat ) ), layer );
	r->setColor( color );
	r->setIconSize( 1 );
	r->setWidth( 1 );

	return r;
}
QgsRubberBand* CMapWidget::AddRBLine( QgsPolyline points, QColor color, QgsVectorLayer *layer )	//layer = nullptr  
{
	QgsRubberBand *r = new QgsRubberBand( this, QGis::Line );			//deprecated: False = not a polygon
	r->setToGeometry( QgsGeometry::fromPolyline( points ), layer );
	r->setColor( color );
	r->setIconSize( 1 );
	r->setWidth( 1 );

	return r;
}

//	Note that you can use strings like "red" instead of Qt::red !!!
//
//static 
QgsSymbolV2* CMapWidget::createPointSymbol( double width, const QColor &color )
{
	auto qsm = []( const char *s1, const std::string &s2 )
	{
		QgsStringMap m;
		m.insert( s1, s2.c_str() );
		return m;
	};

	QgsSymbolV2 *s = QgsMarkerSymbolV2::createSimple( qsm( "","" ) );
	s->deleteSymbolLayer( 0 );		// Remove default symbol layer.
	//s->setColor( lineColor );
	//s->setMapUnitScale()

	auto symbolLayer = new QgsSimpleMarkerSymbolLayerV2;
	symbolLayer->setColor( color );
	symbolLayer->setSize( width );
	symbolLayer->setOutlineStyle( Qt::NoPen );
	s->appendSymbolLayer( symbolLayer );

	return s;
}

//	Note that you can use strings like "red" instead of Qt::red !!!
//
//static 
QgsSymbolV2* CMapWidget::createLineSymbol( double width, const QColor &color )
{
	auto qsm = []( const char *s1, const std::string &s2 )
	{
		QgsStringMap m;
		m.insert( s1, s2.c_str() );
		return m;
	};

	QgsSymbolV2 *s = QgsLineSymbolV2::createSimple( qsm( "", "" ) );
	s->deleteSymbolLayer( 0 ); // Remove default symbol layer.
	//s->setColor( lineColor );
	//s->setMapUnitScale()

	auto symbolLayer = new QgsSimpleLineSymbolLayerV2;
	symbolLayer->setWidth( width );
	symbolLayer->setWidthUnit( QgsSymbolV2::MapUnit );
	symbolLayer->setColor( QColor( color ) );
	s->appendSymbolLayer( symbolLayer );

	return s;
}

//static 
QgsFeatureList& CMapWidget::createPointFeature( QgsFeatureList &list, double lon, double lat, double value )
{
	QgsFields fields;
	fields.append( QgsField("height", QVariant::Double),  QgsFields::OriginProvider );
	QgsFeature *f = new QgsFeature( fields );
	f->setGeometry( QgsGeometry::fromPoint( QgsPoint( lon, lat ) ) );
	f->setAttribute( "height", value );					//not working
	list.append( *f );

	return list;
}

//static 
QgsFeatureList& CMapWidget::createLineFeature( QgsFeatureList &list, QgsPolyline points )
{
	auto line = QgsGeometry::fromPolyline( points );
	QgsFeature *f = new QgsFeature();
	f->setGeometry( line );
	list.append( *f );

	return list;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		Layers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void CMapWidget::SetCurrentLayer( QgsMapLayer *l )
{
	setCurrentLayer( l );
	QgsVectorLayer *vl = qobject_cast< QgsVectorLayer* >( l );
	if ( vl )
		connect( vl, SIGNAL( selectionChanged() ), this, SIGNAL( CurrentLayerSelectionChanged() ), Qt::QueuedConnection );
}


// by femm...
#if !defined(TRUE)
#define TRUE 1
#endif

QgsRasterLayer* CMapWidget::AddRasterLayer( const QString &layer_path, const QString &base_name, const QString &provider, QgsSymbolV2* symbol )	//symbol = nullptr
{
    UNUSED( symbol );

    static const std::string index_symbol = "#";
	static size_t index = 0;

	std::string name = q2a( base_name ) + index_symbol + n2s<std::string>(index++);
	auto l = provider.isEmpty() ? new QgsRasterLayer( layer_path, name.c_str() ) : new QgsRasterLayer( layer_path, name.c_str(), provider );
	//addRenderer( l, symbol );

	if ( l->isValid() ) 
	{
							qDebug( "Layer is valid" );

		// Add the Vector Layer to the Layer Registry
		QgsMapLayerRegistry::instance()->addMapLayer(l, TRUE);

		// Add the Layer to the Layer Set
		mLayerSet.append(QgsMapCanvasLayer(l, TRUE));

		// Set the Map Canvas Layer Set			//TODO: check if we need to setLayerSet every time
		setLayerSet(mLayerSet);

		//setCurrentLayer( l );

		return l;
	}
							qDebug( "Layer is NOT valid" );
	delete l;
	return nullptr;
}


QgsVectorLayer* CMapWidget::AddMemoryLayer( QgsSymbolV2* symbol )		//symbol = nullptr 
{
	return AddVectorLayer( "Point?crs=EPSG:4326&field=height:double&field=name:string(255)&index=yes", "mem", "memory", symbol );

	auto ml = AddVectorLayer( "Point?crs=EPSG:4326&field=height:double&field=name:string(255)&index=yes", "mem", "memory" );	// , symbol );

	auto myTargetField = "height";
	auto myRangeList = new QgsRangeList;
	auto myOpacity = 1;
	// Make our first symbol and range...
	auto myMin = 1491.0;
	auto myMax = 1491.5;
	auto myLabel = "Group 1";
	auto myColour = QColor( "#ffee00" );
	auto mySymbol1 = createPointSymbol( 0.5, myColour );
	mySymbol1->setAlpha( myOpacity );
	auto myRange1 = new QgsRendererRangeV2( myMin, myMax, mySymbol1, myLabel );
	myRangeList->append( *myRange1 );
	//now make another symbol and range...
	myMin = 1491.6;
	myMax = 1492.0;
	myLabel = "Group 2";
	myColour = QColor( "#00eeff" );
	auto mySymbol2 = createPointSymbol( 0.5, myColour );
	mySymbol2->setAlpha( myOpacity );
	auto myRange2 = new QgsRendererRangeV2( myMin, myMax, mySymbol2, myLabel );
	myRangeList->append( *myRange2 );
	auto myRenderer = new QgsGraduatedSymbolRendererV2( "", *myRangeList );
	myRenderer->setMode( QgsGraduatedSymbolRendererV2::Jenks );
	myRenderer->setClassAttribute( myTargetField );

	ml->setRendererV2( myRenderer );

	return ml;
}


QgsVectorLayer* CMapWidget::AddVectorLayer( const QString &layer_path, const QString &base_name, const QString &provider, QgsSymbolV2* symbol )	//symbol = nullptr 
{
	static const std::string index_symbol = "#";
	static size_t index = 0;

	std::string name = q2a( base_name ) + index_symbol + n2s<std::string>(index++);
	auto l = new QgsVectorLayer( layer_path, name.c_str(), provider );
	addRenderer( l, symbol );

	if ( l->isValid() ) {
							qDebug( "Layer is valid" );

		// Add the Vector Layer to the Layer Registry
		QgsMapLayerRegistry::instance()->addMapLayer(l, TRUE);

		// Add the Layer to the Layer Set
		mLayerSet.append(QgsMapCanvasLayer(l, TRUE));

		// Set the Map Canvas Layer Set			//TODO: check if we need to setLayerSet every time
		setLayerSet(mLayerSet);

		SetCurrentLayer( l );

		return l;
	}
							qDebug( "Layer is NOT valid" );
	delete l;
	return nullptr;
}


void CMapWidget::RemoveLayers()
{
	const int count = mLayerSet.size();

	while( mLayerSet.size() > 1 )
		mLayerSet.removeLast();

	if ( count > 1 )
	{
		setLayerSet( mLayerSet );
		SetCurrentLayer( mLayerSet.begin()->layer() );
		refresh();
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		Zoom
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//virtual 
void CMapWidget::keyPressEvent( QKeyEvent * e ) //override
{
	switch ( e->key() )
	{
		case Qt::Key_Home:			QgsDebugMsg( "Home" );

			zoomToFullExtent();
			break;
		default:
			base_t::keyPressEvent( e );
	}
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		Selection Tool
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//slots

void CMapWidget::EnableRectangularSelection( bool enable )
{
	enable ? setMapTool( mSelectFeatures ) : unsetMapTool( mSelectFeatures );
}																		 	
void CMapWidget::EnablePolygonalSelection( bool enable )
{
	enable ? setMapTool( mSelectPolygon ) : unsetMapTool( mSelectPolygon );
}
void CMapWidget::DeselectAll()
{
	// Turn off rendering to improve speed.
	bool renderFlagState = renderFlag();
	if ( renderFlagState )
		setRenderFlag( false );

	QMap<QString, QgsMapLayer*> layers = QgsMapLayerRegistry::instance()->mapLayers();
	for ( QMap<QString, QgsMapLayer*>::iterator it = layers.begin(); it != layers.end(); ++it )
	{
		QgsVectorLayer *vl = qobject_cast<QgsVectorLayer *>( it.value() );
		if ( !vl )
			continue;

		vl->removeSelection();
	}

	// Turn on rendering (if it was on previously)
	if ( renderFlagState )
		setRenderFlag( true );
}


//get selection info

QgsRectangle CMapWidget::CurrentLayerSelectedBox() const
{
	QgsRectangle box;
	QgsVectorLayer *l = QgsMapToolSelectUtils::getCurrentVectorLayer( const_cast<CMapWidget*>( this ) );
	if ( l )
		box = l->boundingBoxOfSelected();

	return box;
}


//create and insert widgets/actions

void CMapWidget::InsertMapSelectionActions( QToolBar *tb, QAction *before, bool with_separator )
{
	if ( !mSelectionButton )
		CreateSelectionWidgets( tb );

	QAction *a = nullptr;
	if ( with_separator )
	{
		a = CActionInfo::CreateAction( tb, eAction_Separator );
		tb->insertAction( before, a );
	}
	else
		a = before;

	tb->insertAction( a, mActionDeselectAll );
	a = tb->insertWidget( mActionDeselectAll, mSelectionButton );
}


void CMapWidget::AddMapSelectionActions( QToolBar *tb, bool with_separator )
{
	if ( !mSelectionButton )
		CreateSelectionWidgets( tb );

	if ( with_separator )
		tb->addAction( CActionInfo::CreateAction( tb, eAction_Separator ) );

	tb->addWidget( mSelectionButton );
	tb->addAction( mActionDeselectAll );
}


void CMapWidget::CreateSelectionWidgets( QToolBar *tb )
{
    mActionSelectFeatures = CActionInfo::CreateAction( tb, eAction_SelectFeatures );
	connect( mActionSelectFeatures, SIGNAL( toggled( bool ) ), this, SLOT( EnableRectangularSelection( bool ) ) );
	mSelectFeatures = new CMapToolSelectFeatures( this );
	mSelectFeatures->setAction( mActionSelectFeatures );

	mActionSelectPolygon = CActionInfo::CreateAction( tb, eAction_SelectPolygon );
	connect( mActionSelectPolygon, SIGNAL( toggled( bool ) ), this, SLOT( EnablePolygonalSelection( bool ) ) );
	mSelectPolygon = new CMapToolSelectPolygon( this );
	mSelectPolygon->setAction( mActionSelectPolygon );

	mActionDeselectAll = CActionInfo::CreateAction( tb, eAction_DeselectAll );
	connect( mActionDeselectAll, SIGNAL( triggered() ), this, SLOT( DeselectAll() ) );

	mSelectionButton = new QToolButton( tb );
	mSelectionButton->setPopupMode( QToolButton::MenuButtonPopup );
	mSelectionButton->addAction( mActionSelectFeatures );
	mSelectionButton->addAction( mActionSelectPolygon );
	mSelectionButton->setDefaultAction( mActionSelectFeatures );
	connect( mSelectionButton, SIGNAL( triggered( QAction * ) ), this, SLOT( ToolButtonTriggered( QAction * ) ) );
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		Measure Tool
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//slots

void CMapWidget::MeasureDistance( bool enable )
{
	enable ? setMapTool( mMeasureDistance ) : unsetMapTool( mMeasureDistance );
}

void CMapWidget::MeasureArea( bool enable )
{
	enable ? setMapTool( mMeasureArea ) : unsetMapTool( mMeasureArea );
}


//create and insert widgets/actions

void CMapWidget::InsertMapMeasureActions( QToolBar *tb, QAction *before, bool with_separator )
{
	if ( !mMeasureButton  )
		CreateMeasureWidgets( tb );

	QAction *a = nullptr;
	if ( with_separator )
	{
		a = CActionInfo::CreateAction( tb, eAction_Separator );
		tb->insertAction( before, a );
	}
	else
		a = before;

	a = tb->insertWidget( a, mMeasureButton );
}


void CMapWidget::AddMapMeasureActions( QToolBar *tb, bool with_separator )
{
	if ( !mMeasureButton  )
		CreateMeasureWidgets( tb );

	if ( with_separator )
		tb->addAction( CActionInfo::CreateAction( tb, eAction_Separator ) );

	tb->addWidget( mMeasureButton );
}

void CMapWidget::CreateMeasureWidgets( QToolBar *tb )
{
    mActionMeasure = CActionInfo::CreateAction( tb, eAction_MeasureLine );
	connect( mActionMeasure, SIGNAL( toggled( bool ) ), this, SLOT( MeasureDistance( bool ) ) );
	mMeasureDistance = new CMeasureTool( this, false );	//false -> distance
	mMeasureDistance->setAction( mActionMeasure );

	mActionMeasureArea = CActionInfo::CreateAction( tb, eAction_MeasureArea );
	connect( mActionMeasureArea, SIGNAL( toggled( bool ) ), this, SLOT( MeasureArea( bool ) ) );
	mMeasureArea = new CMeasureTool( this, true );		//true -> area
	mMeasureArea->setAction( mActionMeasureArea );

	mMeasureButton = new QToolButton( tb );
	mMeasureButton->setPopupMode( QToolButton::MenuButtonPopup );
	mMeasureButton->addAction( mActionMeasure );
	mMeasureButton->addAction( mActionMeasureArea );
	mMeasureButton->setDefaultAction( mActionMeasure );

	connect( mMeasureButton, SIGNAL( triggered( QAction * ) ), this, SLOT( ToolButtonTriggered( QAction * ) ) );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		All Tools
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void CMapWidget::ToolButtonTriggered( QAction *action )
{
	QToolButton *bt = qobject_cast<QToolButton *>( sender() );
	if ( !bt )
		return;

	bt->setDefaultAction( action );
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		Grid
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//slots

void CMapWidget::ToggleGridEnabled( bool toggle ) 
{ 
	mDecorationGrid->setEnabled( toggle ); 
	refresh();
}


//create and insert widgets/actions

void CMapWidget::CreateGridWidgets( QToolBar *tb )
{
	mActionDecorationGrid = CActionInfo::CreateAction( tb, eAction_DecorationGrid );
	mDecorationGrid = new CDecorationGrid( this );
	connect( this, SIGNAL( renderComplete( QPainter * ) ), mDecorationGrid, SLOT( render( QPainter * ) ) );
	connect( mActionDecorationGrid, SIGNAL( toggled( bool ) ), this, SLOT( ToggleGridEnabled( bool ) ) );

	mDecorationGrid->setEnabled( false ); 
	mActionDecorationGrid->setChecked( false );
}


void CMapWidget::AddGridActions( QToolBar *tb, bool with_separator )
{
	if ( !mDecorationGrid )
		CreateGridWidgets( tb );

	if ( with_separator )
		tb->addAction( CActionInfo::CreateAction( tb, eAction_Separator ) );

	tb->addAction( mActionDecorationGrid );
}
void CMapWidget::InsertGridActions( QToolBar *tb, QAction *before, bool with_separator )
{
	if ( !mDecorationGrid )
		CreateGridWidgets( tb );

	QAction *a = nullptr;
	if ( with_separator )
	{
		a = CActionInfo::CreateAction( tb, eAction_Separator );
		tb->insertAction( before, a );
	}
	else
		a = before;

	tb->insertAction( a, mActionDecorationGrid );
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		Rendering Interaction in Parent StatusBar
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// slots

void CMapWidget::ShowProgress( int theProgress, int theTotalSteps )
{
	if ( theProgress == theTotalSteps )
	{
		mProgressBar->reset();
		mProgressBar->hide();
	}
	else
	{
		//only call show if not already hidden to reduce flicker
		if ( !mProgressBar->isVisible() )
		{
			mProgressBar->show();
		}
		mProgressBar->setMaximum( theTotalSteps );
		mProgressBar->setValue( theProgress );

		if ( mProgressBar->maximum() == 0 )
		{
			// for busy indicator (when minimum equals to maximum) the oxygen Qt style (used in KDE)
			// has some issues and does not start busy indicator animation. This is an ugly fix
			// that forces creation of a temporary progress bar that somehow resumes the animations.
			// Caution: looking at the code below may introduce mild pain in stomach.
			if ( strcmp( QApplication::style()->metaObject()->className(), "Oxygen::Style" ) == 0 )
			{
				QProgressBar pb;
				pb.setAttribute( Qt::WA_DontShowOnScreen ); // no visual annoyance
				pb.setMaximum( 0 );
				pb.show();
				qApp->processEvents();
			}
		}
	}
}


void CMapWidget::RefreshStarted()
{
	ShowProgress( -1, 0 ); // trick to make progress bar show busy indicator
}

void CMapWidget::RefreshFinished()
{
	ShowProgress( 0, 0 ); // stop the busy indicator
}


//create and insert widgets

void CMapWidget::InsertStatusBarMapWidgets( QStatusBar *bar, int index )
{
	// Add a panel to the status bar for progress
	//
	mProgressBar = new QProgressBar( bar );
	mProgressBar->setObjectName( "mProgressBar" );
	mProgressBar->setMaximumWidth( 100 );
	mProgressBar->setWhatsThis( tr( "Progress bar that displays the status of time-intensive operations" ) );
	bar->insertPermanentWidget( index, mProgressBar, 1 );

	connect( this, SIGNAL( renderStarting() ), this, SLOT( RefreshStarted() ) );
	connect( this, SIGNAL( mapCanvasRefreshed() ), this, SLOT( RefreshFinished() ) );

	// Bumped the font up one point size since 8 was too
	// small on some platforms. A point size of 9 still provides
	// plenty of display space on 1024x768 resolutions
	QFont myFont( "Arial", 9 );
	// render suppression status bar widget
	//
	mRenderSuppressionCBox = new QCheckBox( tr( "Render" ), bar );
	mRenderSuppressionCBox->setObjectName( "mRenderSuppressionCBox" );
	mRenderSuppressionCBox->setChecked( true );
	mRenderSuppressionCBox->setFont( myFont );
	mRenderSuppressionCBox->setWhatsThis( tr( "When checked, the map layers "
		"are rendered in response to map navigation commands and other "
		"events. When not checked, no rendering is done. This allows you "
		"to add a large number of layers and symbolize them before rendering." ) );
	mRenderSuppressionCBox->setToolTip( tr( "Toggle map rendering" ) );
	bar->insertPermanentWidget( index, mRenderSuppressionCBox, 0 );

	connect( mRenderSuppressionCBox, SIGNAL( toggled( bool ) ), this, SLOT( setRenderFlag( bool ) ) );
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_MapWidget.cpp"
