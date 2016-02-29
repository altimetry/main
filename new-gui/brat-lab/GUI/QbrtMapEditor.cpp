#include "new-gui/brat-lab/stdafx.h"

#include <QtGui>
#include <QColor>

#include "new-gui/Common/+UtilsIO.h"


#include <osgEarthQt/ViewerWidget>

#if defined (GL_LINES_ADJACENCY_EXT)
#undef GL_LINES_ADJACENCY_EXT
#endif
#if defined (GL_LINE_STRIP_ADJACENCY_EXT)
#undef GL_LINE_STRIP_ADJACENCY_EXT
#endif
#if defined (GL_TRIANGLES_ADJACENCY_EXT)
#undef GL_TRIANGLES_ADJACENCY_EXT
#endif
#if defined (GL_TRIANGLE_STRIP_ADJACENCY_EXT)
#undef GL_TRIANGLE_STRIP_ADJACENCY_EXT
#endif

#include <qgsproviderregistry.h>
#include <qgsvectordataprovider.h>
#include <qgsmaplayerregistry.h>
#include <qgsvectorlayer.h>

#include <qgsrubberband.h>
#include <qgsvertexmarker.h>
#include <qgscoordinatereferencesystem.h>
#include <qgsapplication.h>
#include <qgslinesymbollayerv2.h>
#include <qgsmarkersymbollayerv2.h>
//#include <qgsvectorfieldsymbollayer.h>

#include <qgslayertreeview.h>
#include <qgslayertreemodel.h>
#include <qgslayertreenode.h>
#include <qgslayertreeviewdefaultactions.h>
#include <qgslayertreegroup.h>

#include "qgsapplayertreeviewmenuprovider.h"

#include "new-gui/Common/QtUtilsIO.h"

#include "new-gui/Common/tools/Trace.h"

#include "new-gui/brat-lab/System/BackServices.h"
#include "new-gui/brat/DataModels/DisplayFilesProcessor.h"
#include "new-gui/brat/DataModels/PlotData/WorldPlotData.h"
#include "new-gui/brat/DataModels/PlotData/WorldPlot.h"


#include "new-gui/brat/GUI/DisplayWidgets/GlobeWidget.h"


#include "QbrtMapEditor.h"


void addLabelsLayer( QbrtMapEditor *mpMapCanvas );


//constants.py

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//template<>
//QgsMapLayerRegistry* QgsSingleton<QgsMapLayerRegistry>::sInstance;

#include <osgViewer/Viewer>
//#include "new-gui/brat/Graphics/Globe/Globe.h"

QbrtMapEditor::QbrtMapEditor(QWidget *parent) : base_t(parent), m_ToolEditor( false )
{
    isUntitled = true;

    setWindowIcon(QPixmap(":/images/globe.png"));
    setWindowTitle("[*]");
    setAttribute(Qt::WA_DeleteOnClose);

    //createLayerTreeView();
	mSplitter = new QSplitter( Qt::Vertical, this );
    mMapCanvas = new CBratMapView( mSplitter );
	setCentralWidget( mSplitter );

	createToolBar();
	createGlobe();
}

void QbrtMapEditor::createToolBar()
{
	QToolBar *ToolBar = new QToolBar( this );
	ToolBar->setIconSize( QSize( IconSize, IconSize ) );
	ToolBar->setWindowTitle( "Map ToolBar" );
	ToolBar->setAllowedAreas( Qt::RightToolBarArea | Qt::LeftToolBarArea );

	//ToolBar->setStyleSheet("QToolBar { border: 2px }");
	setStyleSheet( "QMainWindow{background-color: darkgray;border: 1px solid black;}" );

	::insertToolBar( this, ToolBar, Qt::RightToolBarArea );
	//addToolBarBreak();

	mAction_View_Globe = new QAction( "View Globe", ToolBar );								//starts unchecked: actions in button menu
	mAction_View_Globe->setObjectName( QString::fromUtf8( "mAction_View_Globe" ) );
	mAction_View_Globe->setCheckable( true );
	mAction_View_Globe->setToolTip( "Show Globe" );
	mAction_View_Globe->setChecked( false );
	mAction_View_Globe->setEnabled( false );
	connect( mAction_View_Globe, SIGNAL( toggled( bool ) ), this, SLOT( action_View_Globe( bool ) ) );
	QIcon icon4;
	icon4.addFile( QString::fromUtf8( ":/images/globe.png" ), QSize(), QIcon::Normal, QIcon::Off );
	mAction_View_Globe->setIcon( icon4 );
	ToolBar->addAction( mAction_View_Globe );

	ToolBar->addSeparator();

	// Render suppression tool bar widget
	//
	mRenderSuppressionAction = new QAction( tr( "Render" ), ToolBar );	//mRenderSuppressionAction->setOrientation( Qt::Vertical );
	mRenderSuppressionAction->setObjectName( "mRenderSuppressionAction" );
	mRenderSuppressionAction->setCheckable( true );
	mRenderSuppressionAction->setChecked( true );
	mRenderSuppressionAction->setWhatsThis( tr( "When checked, the map layers "
		"are rendered in response to map navigation commands and other "
		"events. When not checked, no rendering is done. This allows you "
		"to add a large number of layers and symbolize them before rendering." ) );
	mRenderSuppressionAction->setToolTip( tr( "Toggle map rendering" ) );
	QIcon icon3;
	icon3.addFile( QString::fromUtf8( ":/images/render.png" ), QSize(), QIcon::Normal, QIcon::Off );
	mRenderSuppressionAction->setIcon( icon3 );
	ToolBar->addAction( mRenderSuppressionAction );

	connect( mRenderSuppressionAction, SIGNAL( toggled( bool ) ), mMapCanvas, SLOT( setRenderFlag( bool ) ) );

	// Add progress
	//
	mProgressBar = new QProgressBar( ToolBar );
	ToolBar->addWidget( mProgressBar );
	mProgressBar->setTextVisible( false );
	mProgressBar->setObjectName( "mProgressBar" );
	mProgressBar->setMaximumHeight( IconSize );	  	//mProgressBar->setMaximumWidth( IconSize );
	mProgressBar->setOrientation( Qt::Vertical );
	mProgressBar->setSizePolicy( QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred );
	mProgressBar->hide();
	mProgressBar->setWhatsThis( tr( "Progress bar that displays the status "
		"of rendering layers and other time-intensive operations" ) );
	mProgressBarAction = qobject_cast<QWidgetAction*>( ToolBar->addWidget( mProgressBar ) );

	connect( mMapCanvas, SIGNAL( renderStarting() ), this, SLOT( canvasRefreshStarted() ) );
	connect( mMapCanvas, SIGNAL( mapCanvasRefreshed() ), this, SLOT( canvasRefreshFinished() ) );
}

void QbrtMapEditor::action_View_Globe( bool visible )
{
	//assert( mGlobeViewerWidget );
	//mGlobeViewerWidget->setVisible( visible );
	////mMapCanvas->setVisible( !visible );

	assert__( mGlobeWidget );
	mGlobeWidget->setVisible( visible );
}

void QbrtMapEditor::createGlobe()
{
	//if ( !mGlobe /* && mSplitter*/)
	if ( !mGlobeWidget /* && mSplitter*/)
	{
        mGlobeWidget = new CGlobeWidget( this, mMapCanvas, statusBar() );
		//mGlobe = new GlobePlugin( nullptr, mMapCanvas );
		//mOsgViewer = mGlobe->run( ad.mGlobeDir );						//can return null

		//mGlobeViewerWidget = new osgEarth::QtGui::ViewerWidget( mOsgViewer );	//mGlobeViewerWidget->setGeometry( 100, 100, 1024, 800 );	//mGlobeViewerWidget->show();
		//osg::Camera* camera = mOsgViewer->getCamera();
		//osgQt::GraphicsWindowQt *gw = dynamic_cast<osgQt::GraphicsWindowQt*>( camera->getGraphicsContext() );
		//gw->getGLWidget()->setMinimumSize( 10, 10 );	//CRITICAL: prevent image from disappearing forever when height is 0

		//QSettings settings;

		//if ( settings.value( "/Plugin-Globe/anti-aliasing", true ).toBool() )
		//{
		//	QGLFormat glf = QGLFormat::defaultFormat();
		//	glf.setSampleBuffers( true );
		//	//bool aaLevelIsInt;
		//	//int aaLevel;
		//	//QString aaLevelStr = settings.value( "/Plugin-Globe/anti-aliasing-level", "" ).toString();
		//	//aaLevel = aaLevelStr.toInt( &aaLevelIsInt );
		//	//if ( aaLevelIsInt )
		//	//{
		//	//	glf.setSamples( aaLevel );
		//	//}
		//	mGlobeViewerWidget->setFormat( glf );
		//}

		//mSplitter->addWidget( mGlobeViewerWidget );
		mSplitter->addWidget( mGlobeWidget );

		if (mAction_View_Globe)
			mAction_View_Globe->setEnabled( true );
	}
}
void QbrtMapEditor::CreateWPlot( const CDisplayFilesProcessor *proc, CWPlot* wplot )
{
    mMapCanvas->CreatePlot( proc->GetWorldPlotProperties(0), wplot );
	createGlobe();
}

void QbrtMapEditor::canvasRefreshStarted()
{
	showProgress( -1, 0 ); // trick to make progress bar show busy indicator
}
void QbrtMapEditor::canvasRefreshFinished()
{
	showProgress( 0, 0 ); // stop the busy indicator
}
// slot to update the progress bar in the status bar
void QbrtMapEditor::showProgress( int theProgress, int theTotalSteps )
{
	if ( theProgress == theTotalSteps )
	{
		mProgressBar->reset();
		mProgressBarAction->setVisible(false);
	}
	else
	{
		//only call show if not already hidden to reduce flicker
		if ( !mProgressBar->isVisible() )
		{
			mProgressBarAction->setVisible(true);
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



//virtual
QbrtMapEditor::~QbrtMapEditor()
{
	delete mGlobeWidget;

	//delete mGlobeViewerWidget;
	//mGlobeViewerWidget = nullptr;
	//delete mGlobe;
	//mGlobe = nullptr;
}


#include <qgsproject.h>

void QbrtMapEditor::setupLayerTreeViewFromSettings()
{
	QSettings s;

	QgsLayerTreeModel* model = mLayerTreeView->layerTreeModel();
	model->setFlag( QgsLayerTreeModel::ShowRasterPreviewIcon, s.value( "/qgis/createRasterLegendIcons", false ).toBool() );

	QFont fontLayer, fontGroup;
	fontLayer.setBold( s.value( "/qgis/legendLayersBold", true ).toBool() );
	fontGroup.setBold( s.value( "/qgis/legendGroupsBold", false ).toBool() );
	model->setLayerTreeNodeFont( QgsLayerTreeNode::NodeLayer, fontLayer );
	model->setLayerTreeNodeFont( QgsLayerTreeNode::NodeGroup, fontGroup );
}

void QbrtMapEditor::createLayerTreeView()
{
	//no code originally commented out in this function

	mLayerTreeView = new QgsLayerTreeView( this );
	mLayerTreeView->setObjectName( "theLayerTreeView" ); // "theLayerTreeView" used to find this canonical instance later

	mLayerTreeView->setWhatsThis( tr( "Map legend that displays all the layers currently on the map canvas. Click on the check box to turn a layer on or off. Double click on a layer in the legend to customize its appearance and set other properties." ) );

	mLayerTreeDock = new QDockWidget( tr( "Layers" ), this );
	mLayerTreeDock->setObjectName( "Layers" );
	mLayerTreeDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

	QgsLayerTreeModel* model = new QgsLayerTreeModel( QgsProject::instance()->layerTreeRoot(), this );
#ifdef ENABLE_MODELTEST
	new ModelTest( model, this );
#endif
	model->setFlag( QgsLayerTreeModel::AllowNodeReorder );
	model->setFlag( QgsLayerTreeModel::AllowNodeRename );
	model->setFlag( QgsLayerTreeModel::AllowNodeChangeVisibility );
	model->setFlag( QgsLayerTreeModel::ShowLegendAsTree );
	model->setAutoCollapseLegendNodes( 10 );

	mLayerTreeView->setModel( model );
	mLayerTreeView->setMenuProvider( new QgsAppLayerTreeViewMenuProvider( mLayerTreeView, this ) );

	setupLayerTreeViewFromSettings();

	connect( mLayerTreeView, SIGNAL( doubleClicked( QModelIndex ) ), this, SLOT( layerTreeViewDoubleClicked( QModelIndex ) ) );
	connect( mLayerTreeView, SIGNAL( currentLayerChanged( QgsMapLayer* ) ), this, SLOT( activeLayerChanged( QgsMapLayer* ) ) );
	connect( mLayerTreeView->selectionModel(), SIGNAL( currentChanged( QModelIndex, QModelIndex ) ), this, SLOT( updateNewLayerInsertionPoint() ) );
	//connect( QgsProject::instance()->layerTreeRegistryBridge(), SIGNAL( addedLayersToLayerTree( QList<QgsMapLayer*> ) ),
	//	this, SLOT( autoSelectAddedLayer( QList<QgsMapLayer*> ) ) );

	// add group tool button
	QToolButton* btnAddGroup = new QToolButton;
	btnAddGroup->setAutoRaise( true );
	btnAddGroup->setIcon( QIcon( ":/images/actions/mActionFolder.png" ) );
	btnAddGroup->setToolTip( tr( "Add Group" ) );
	connect( btnAddGroup, SIGNAL( clicked() ), mLayerTreeView->defaultActions(), SLOT( addGroup() ) );

	// visibility groups tool button
	QToolButton* btnVisibilityPresets = new QToolButton;
	btnVisibilityPresets->setAutoRaise( true );
	btnVisibilityPresets->setToolTip( tr( "Manage Layer Visibility" ) );
	btnVisibilityPresets->setIcon( QIcon( ":/images/actions/mActionShowAllLayers.png" ) );
	btnVisibilityPresets->setPopupMode( QToolButton::InstantPopup );
	//btnVisibilityPresets->setMenu( QgsVisibilityPresets::instance()->menu() );

	// filter legend tool button
	//mBtnFilterLegend = new QToolButton;
	//mBtnFilterLegend->setAutoRaise( true );
	//mBtnFilterLegend->setCheckable( true );
	//mBtnFilterLegend->setToolTip( tr( "Filter Legend By Map Content" ) );
	//mBtnFilterLegend->setIcon( QgsApplication::getThemeIcon( "/mActionFilter.png" ) );
	//connect( mBtnFilterLegend, SIGNAL( clicked() ), this, SLOT( toggleFilterLegendByMap() ) );

	// expand / collapse tool buttons
	QToolButton* btnExpandAll = new QToolButton;
	btnExpandAll->setAutoRaise( true );
	btnExpandAll->setIcon( QIcon( ":/images/actions/mActionExpandTree.png" ) );
	btnExpandAll->setToolTip( tr( "Expand All" ) );
	connect( btnExpandAll, SIGNAL( clicked() ), mLayerTreeView, SLOT( expandAll() ) );
	QToolButton* btnCollapseAll = new QToolButton;
	btnCollapseAll->setAutoRaise( true );
	btnCollapseAll->setIcon( QIcon( ":/images/actions/mActionCollapseTree.png" ) );
	btnCollapseAll->setToolTip( tr( "Collapse All" ) );
	connect( btnCollapseAll, SIGNAL( clicked() ), mLayerTreeView, SLOT( collapseAll() ) );

	//QToolButton* btnRemoveItem = new QToolButton;
	//btnRemoveItem->setDefaultAction( this->mActionRemoveLayer );
	//btnRemoveItem->setAutoRaise( true );

	QHBoxLayout* toolbarLayout = new QHBoxLayout;
	toolbarLayout->setContentsMargins( QMargins( 5, 0, 5, 0 ) );
	toolbarLayout->addWidget( btnAddGroup );
	toolbarLayout->addWidget( btnVisibilityPresets );
	//toolbarLayout->addWidget( mBtnFilterLegend );
	toolbarLayout->addWidget( btnExpandAll );
	toolbarLayout->addWidget( btnCollapseAll );
	//toolbarLayout->addWidget( btnRemoveItem );
	toolbarLayout->addStretch();

	QVBoxLayout* vboxLayout = new QVBoxLayout;
	vboxLayout->setMargin( 0 );
	vboxLayout->addLayout( toolbarLayout );
	vboxLayout->addWidget( mLayerTreeView );

	QWidget* w = new QWidget;
	w->setLayout( vboxLayout );
	mLayerTreeDock->setWidget( w );
	addDockWidget( Qt::LeftDockWidgetArea, mLayerTreeDock );

	//mLayerTreeCanvasBridge = new QgsLayerTreeMapCanvasBridge( QgsProject::instance()->layerTreeRoot(), mMapCanvas, this );
	//connect( QgsProject::instance(), SIGNAL( writeProject( QDomDocument& ) ), mLayerTreeCanvasBridge, SLOT( writeProject( QDomDocument& ) ) );
	//connect( QgsProject::instance(), SIGNAL( readProject( QDomDocument ) ), mLayerTreeCanvasBridge, SLOT( readProject( QDomDocument ) ) );

	//bool otfTransformAutoEnable = QSettings().value( "/Projections/otfTransformAutoEnable", true ).toBool();
	//mLayerTreeCanvasBridge->setAutoEnableCrsTransform( otfTransformAutoEnable );

	//mMapLayerOrder = new QgsCustomLayerOrderWidget( mLayerTreeCanvasBridge, this );
	//mMapLayerOrder->setObjectName( "theMapLayerOrder" );

	//mMapLayerOrder->setWhatsThis( tr( "Map layer list that displays all layers in drawing order." ) );
	//mLayerOrderDock = new QDockWidget( tr( "Layer order" ), this );
	//mLayerOrderDock->setObjectName( "LayerOrder" );
	//mLayerOrderDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

	//mLayerOrderDock->setWidget( mMapLayerOrder );
	//addDockWidget( Qt::LeftDockWidgetArea, mLayerOrderDock );
	//mLayerOrderDock->hide();
}

void QbrtMapEditor::activeLayerChanged( QgsMapLayer* layer )
{
	if ( mMapCanvas )
		mMapCanvas->setCurrentLayer( layer );
}

void QbrtMapEditor::removeLayer()
{
	//no code originally commented out in this function

	if ( !mLayerTreeView )
		return;

	foreach( QgsMapLayer * layer, mLayerTreeView->selectedLayers() )
	{
		QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer*>( layer );
		if ( vlayer && vlayer->isEditable() /*&& !toggleEditing( vlayer, true ) */)
			return;
	}

	QList<QgsLayerTreeNode*> selectedNodes = mLayerTreeView->selectedNodes( true );

	//validate selection
	if ( selectedNodes.isEmpty() )
	{
		//messageBar()->pushMessage( tr( "No legend entries selected" ),
		//	tr( "Select the layers and groups you want to remove in the legend." ),
		//	QgsMessageBar::INFO, messageTimeout() );
		return;
	}

	bool promptConfirmation = QSettings().value( "qgis/askToDeleteLayers", true ).toBool();
	//display a warning
	if ( promptConfirmation && QMessageBox::warning( this, tr( "Remove layers and groups" ), tr( "Remove %n legend entries?", "number of legend items to remove", selectedNodes.count() ), QMessageBox::Ok | QMessageBox::Cancel ) == QMessageBox::Cancel )
	{
		return;
	}

	foreach( QgsLayerTreeNode* node, selectedNodes )
	{
		QgsLayerTreeGroup* parentGroup = qobject_cast<QgsLayerTreeGroup*>( node->parent() );
		if ( parentGroup )
			parentGroup->removeChildNode( node );
	}

	//showStatusMessage( tr( "%n legend entries removed.", "number of removed legend entries", selectedNodes.count() ) );

	mMapCanvas->refresh();
}

void QbrtMapEditor::globeSettings()
{
	//assert__( mGlobe );
	assert__( mGlobeWidget );

	mGlobeWidget->HandleSettings();
}

//void QbrtMapCanvas::CreateWPlot( const CDisplayFilesProcessor *proc, CWPlot* wplot )
//{
//	QSize size;
//	QPoint pos;

//	wplot->GetInfo();

//	CWorldPlotProperty* wPlotProperty = proc->GetWorldPlotProperty( 0 );
//	UNUSED( wPlotProperty );

//	//TODO CWorldPlotFrame* frame = new CWorldPlotFrame( NULL, -1, title, wPlotProperty, pos, size );

//	// for geostrophic velocity
//	CPlotField * northField =NULL;
//	CPlotField * eastField =NULL;
//	for ( CObArray::iterator itField = wplot->m_fields.begin(); itField != wplot->m_fields.end(); itField++ )
//	{
//		CPlotField* field = CPlotField::GetPlotField( *itField );

//		if ( field->m_internalFiles.empty() )
//			continue;

//		if ( field->m_worldProps->m_northComponent && northField == NULL ) {
//			northField = field;
//			continue;
//		}
//		else
//		if ( field->m_worldProps->m_eastComponent && eastField == NULL ) {
//			eastField = field;
//			continue;
//		}

//		// otherwise just add it as regular data
//		CGeoMap *geoMap = new CGeoMap( field );
//		AddData( geoMap );
//	}

//	// we have a Vector Plot!
//	if ( northField != NULL && eastField != NULL ) {

//		CGeoVelocityMap *gvelocityMap = new CGeoVelocityMap( northField, eastField );
//		gvelocityMap->SetIsGlyph( true );
//		AddData( gvelocityMap );
//	}
//	else if ( northField != eastField ) {
//		CException e( "CBratDisplayApp::CreateWPlot - incomplete std::vector plot components", BRATHL_INCONSISTENCY_ERROR );
//		CTrace::Tracer( "%s", e.what() );
//		throw ( e );
//	}
//}
//void CWorldPlotRenderer::AddData( CWorldPlotData* pdata )
//{
//	pdata->SetRenderer( m_vtkRend );
//
//	m_actors.Insert( pdata );
//	m_transformations->AddItem( pdata->GetTransform() );
//	pdata->GetVtkTransform()->SetTransform( pdata->GetTransform() );
//
//	ResetTextActor();
//}

#define USE_POINTS		//(**)
#define USE_FEATURES	//(***)
/*
230 108	120 2

	89	 86
	18	 19
		 15
		  2
   107	122
	 1
*/  
//
//(*)	Using memory layer because: "OGR error creating feature 0: CreateFeature : unsupported operation on a read-only datasource."
//(**)	Using point and not line features because: 
//			1) (If using main layer and "this" shapefile) "Feature creation error (OGR error: Attempt to write non-linestring (POINT) geometry to ARC type shapefile.)"
//			2) There is no algorithm to "bezier" the points collection to an spline
//			3) cannot color the values over a line (unless with another layer, a point layer, but then, why the line layer?)
//(***) Using features and not rubberbands because these are not projected in the globe
//
//void QbrtMapCanvas::AddData( CWorldPlotData* pdata )
//{
//	CGeoMap* geoMap = dynamic_cast<CGeoMap*>( pdata );

//	auto IsValidPoint = [&geoMap]( int32_t i )
//	{
//		bool bOk = geoMap->bits[ i ];

//		//	  if (Projection == VTK_PROJ2D_MERCATOR)
//		//	  {
//		bOk &= geoMap->valids[ i ];
//		//	  }
//		//
//		return bOk;
//	};


//	auto const size = geoMap->vals.size();
//	QgsFeatureList flist;

//#if defined (USE_POINTS)	//(**)

//	for ( auto i = 0u; i < size; ++ i )
//	{
//		if ( !IsValidPoint( i ) )
//			continue;

//		auto x = i % geoMap->lons.size(); // ( x * geoMap->lats.size() ) + i;
//		auto y = i / geoMap->lons.size(); // ( x * geoMap->lats.size() ) + i;

//#if defined (USE_FEATURES) //(***)
//		createPointFeature( flist, geoMap->lons.at( x ), geoMap->lats.at( y ), geoMap->vals[ i ] );
//		//createPointFeature( flist, geoMap->lons.at( x ), geoMap->lats.at( y ), QColor( 0, (unsigned char)(geoMap->vals[ i ]), 0 ) );
//#else
//		addRBPoint( geoMap->lons.at( x ), geoMap->lats.at( y ), QColor( (long)(geoMap->vals[ i ]) ), mMainLayer );
//#endif
//	}

//#if defined (USE_FEATURES)
//	auto memL = addMemoryLayer( createPointSymbol( 0.5, Qt::red ) );	//(*)	//note that you can use strings like "red" instead!!!
//	memL->dataProvider()->addFeatures( flist );
//	//memL->updateExtents();
//	//refresh();
//#endif

//	return;

//#else		//(**)

//	QgsPolyline points;
//	for ( auto i = 0; i < size; ++ i )
//	{
//		if ( !IsValidPoint(i) )
//			continue;

//		auto x = i % geoMap->lons.size();
//		auto y = i / geoMap->lons.size();

//		points.append( QgsPoint( geoMap->lons.at( x ), geoMap->lats.at( y ) ) );
//	}
//#if !defined (USE_FEATURES) //(***)
//	auto memL = addMemoryLayer( createLineSymbol( 0.5, Qt::red ) );	//(*)	//note that you can use strings like "red" instead!!!
//	createLineFeature( flist, points );
//	memL->dataProvider()->addFeatures( flist );
//	//memL->updateExtents();
//	//refresh();
//#else
//	addRBLine( points, QColor( 0, 255, 0 ), mMainLayer );
//#endif

//	return;

//#endif

//	//femm: This is CWorldPlotPanel::AddData

//	//femm: the important part
//	//if ( pdata->GetColorBarRenderer() != NULL )
//	//	m_vtkWidget->GetRenderWindow()->AddRenderer( pdata->GetColorBarRenderer()->GetVtkRenderer() );
//	//m_plotRenderer->AddData( pdata );


//	//femm: the less important part
//	//CGeoMap* geoMap = dynamic_cast<CGeoMap*>( pdata );
//	//if ( geoMap != NULL )
//	//{
//	//	wxString textLayer = wxString::Format( "%s", geoMap->GetDataName().c_str() );

//	//	m_plotPropertyTab->GetLayerChoice()->Append( textLayer, static_cast<void*>( geoMap ) );
//	//	m_plotPropertyTab->SetCurrentLayer( 0 );
//	//}

//	//int32_t nFrames = 1;
//	//if ( geoMap != NULL )
//	//	nFrames = geoMap->GetNrMaps();

//	//m_animationToolbar->SetMaxFrame( nFrames );
//}



void QbrtMapEditor::ToolEditor( bool tool )
{
    m_ToolEditor = tool;
    setWindowModified( false );
    disconnect( this, SLOT(setWindowModified(bool)) );
}

//////////////////////////////////////////////////////////////////////////
//                          ACCESS ACTIONS
//////////////////////////////////////////////////////////////////////////


//bool QbrtMapEditor::isEmpty() const
//{
//    return ::isEmpty( this );
//}

bool QbrtMapEditor::isMDIChild() const
{
    SimpleMsgBox( "bool QbrtMapEditor::isMDIChild() const \n Not implemented yet..." );
    return false;
}

//QString QbrtMapEditor::getSelectedText()
//{
//    return ::getSelectedText( this );
//}


//////////////////////////////////////////////////////////////////////////
//                          UPDATE ACTIONS
//////////////////////////////////////////////////////////////////////////


//this is very nice but does not free us from manually updating the ations in the Edit update menu function
//
//void QbrtMapEditor::connectSaveAvailableAction( QAction *pa )      {    connect( document(), SIGNAL(modificationChanged(bool)), pa, SLOT(setEnabled(bool)));}

//void QbrtMapEditor::connectUndoAvailableAction( QAction *pa )      {    connect( document(), SIGNAL( undoAvailable(bool)), pa, SLOT(setEnabled(bool)) );}
//void QbrtMapEditor::connectRedoAvailableAction( QAction *pa )      {    connect( document(), SIGNAL( redoAvailable(bool)), pa, SLOT(setEnabled(bool)) );}
void QbrtMapEditor::connectCutAvailableAction( QAction *pa )       {    connect( this, SIGNAL( copyAvailable(bool)), pa, SLOT(setEnabled(bool)) );}
void QbrtMapEditor::connectCopyAvailableAction( QAction *pa )      {    connect( this, SIGNAL( copyAvailable(bool)), pa, SLOT(setEnabled(bool)) );}
void QbrtMapEditor::connectDeleteSelAvailableAction( QAction *pa ) {    connect( this, SIGNAL( copyAvailable(bool)), pa, SLOT(setEnabled(bool)) );}

void  QbrtMapEditor::focusInEvent ( QFocusEvent * event )
{
    base_t::focusInEvent ( event );
    //if ( m_ToolEditor )
        emit toolWindowActivated( this );
}

//////////////////////////////////////////////////////////////////////////
//                              FILE ACTIONS
//////////////////////////////////////////////////////////////////////////


enum EFileType {
    e_txt,
    e_html,
    e_odt,
    e_unknown
};

inline EFileType TypeFromExtension( const QString &fileName )
{
    if ( fileName.endsWith(".odt", Qt::CaseInsensitive) )       //Qt does not read, write only option
        return e_odt;
    if (    fileName.endsWith(".htm", Qt::CaseInsensitive)
         || fileName.endsWith(".html", Qt::CaseInsensitive)
         || fileName.endsWith(".rtf", Qt::CaseInsensitive)      //mightBeRichText returns false for this (...) and Qt really reads it as text
         )
        return e_html;
    if ( fileName.endsWith(".txt", Qt::CaseInsensitive) )       //default on write
        return e_txt;
    return e_unknown;
}

void QbrtMapEditor::documentWasModified()
{
    setWindowModified(true);
}

QString QbrtMapEditor::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}


/////////////////////////////////////////////////////////////////////////
// FILE ACTIONS - OPEN/ INPUT
/////////////////////////////////////////////////////////////////////////



void QbrtMapEditor::setCurrentFile( const QString &fileName )
{
    curFile = fileName;

    isUntitled = false;
    windowMenuAction()->setText(strippedName(curFile));
//    document()->setModified(false);
    setWindowTitle(strippedName(curFile) + "[*]");
    setWindowModified(false);
    emit setCurrentFile( this );
}


void QbrtMapEditor::newFile()
{
    static int documentNumber = 1;

    curFile = tr("document%1.txt").arg(documentNumber);
    setWindowTitle(curFile + "[*]");
    windowMenuAction()->setText(curFile);
    isUntitled = true;
    ++documentNumber;
}


bool QbrtMapEditor::readFile( const QString &fileName )
{
    if (!QFile::exists( fileName ))
        return false;

	assert( false );
	
	throw;
}


QbrtMapEditor *QbrtMapEditor::openFile(const QString &fileName, QWidget *parent)
{
    QbrtMapEditor *editor = new QbrtMapEditor( parent );
    if (editor->readFile( fileName )) {
        editor->setCurrentFile( fileName );
        return editor;
    } else {
        delete editor;
        return 0;
    }
}

bool QbrtMapEditor::reOpen()
{
    if ( !IsUntitled() && readFile( curFile ) )
        setCurrentFile( curFile );
    else
        return false;
    return true;
}

QbrtMapEditor *QbrtMapEditor::open(QWidget *parent)
{
    static const TBackServices &m_bs = GetBackServices();

    QString fileName = QFileDialog::getOpenFileName( parent, tr("Open"), m_bs.GetDataPath(),
                                                     tr("Expressions (*.mm *.txt);;HTML-Files (*.htm *.html);;All Files (*)") );
    if (fileName.isEmpty())
        return 0;

    return openFile(fileName, parent);
}


/////////////////////////////////////////////////////////////////////////
// FILE ACTIONS - CLOSE / OUTPUT ACTIONS
/////////////////////////////////////////////////////////////////////////

bool QbrtMapEditor::writeFile( const QString &fileName )
{
    //writing, the old way
    //
//    QFile file(fileName);
//    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
//        QMessageBox::warning(this, tr("MDI QbrtMapEditor"),
//                             tr("Cannot write file %1:\n%2.")
//                             .arg(file.fileName())
//                             .arg(file.errorString()));
//        return false;
//    }

    QTextDocumentWriter writer( fileName );
    bool success = false;
    {
        WaitCursor wait;

        //writing, the old way
        //
        //QTextStream out(&file);
        //out << toPlainText();

        if ( TypeFromExtension( fileName ) == e_html )
            writer.setFormat( "HTML" );
        else
        if ( TypeFromExtension( fileName ) == e_odt )
            writer.setFormat( "ODF" );
        else
        if ( TypeFromExtension( fileName ) == e_txt )
            writer.setFormat( "plaintext" );
        else
            writer.setFormat( "plaintext" );

        //===> success = writer.write( document() );
    }
    if ( !success )
        QMessageBox::warning(this, tr("MDI QbrtMapEditor"),
                             tr("Cannot write file %1:\n%2.")
                             .arg( fileName )
                             .arg( writer.device()->errorString() ) );
    return success;
}


bool QbrtMapEditor::saveFile(const QString &fileName)
{
    if (writeFile(fileName))
    {
        setCurrentFile(fileName);
        return true;
    }
    return false;
}


bool QbrtMapEditor::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As..."), curFile,
                                                    tr("Expressions (*.mm *.txt);;HTML-Files (*.htm *.html);;All Files (*);;ODF files (*.odt)"));
    if ( fileName.isEmpty() )
        return false;

    if ( TypeFromExtension( fileName ) == e_unknown )
        fileName += ".txt"; // default
        //fileName += ".odt"; // default

    return saveFile( fileName );
}


bool QbrtMapEditor::save()
{
    if (isUntitled) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

void QbrtMapEditor::closeEvent(QCloseEvent *event)
{
    if (okToContinue()) {
        event->accept();
    } else {
        event->ignore();
    }
}

bool QbrtMapEditor::okToContinue()
{
    //if (curFile.startsWith(QLatin1String(":/")))
    //    return true;

	if ( mMapCanvas )
		mMapCanvas->stopRendering();

	return !mMapCanvas || !mMapCanvas->isDrawing();

    //if ( document()->isModified() ) {
    //    int r = QMessageBox::warning(this, tr("MDI QbrtMapEditor"),
    //                    tr("File %1 has been modified.\n"
    //                       "Do you want to save your changes?")
    //                    .arg(strippedName(curFile)),
    //                    QMessageBox::Yes | QMessageBox::No
    //                    | QMessageBox::Cancel);
    //    if (r == QMessageBox::Yes) {
    //        return save();
    //    } else if (r == QMessageBox::Cancel) {
    //        return false;
    //    }
    //}
    return true;
}



//////////////////////////////////////////////////////////////////////////
//                              ??? ACTIONS
//////////////////////////////////////////////////////////////////////////


QSize QbrtMapEditor::sizeHint() const
{
    return QSize(72 * fontMetrics().width('x'),
                 25 * fontMetrics().lineSpacing());
}



//////////////////////////////////////////////////////////////////////////
//                              FORMAT ACTIONS
//////////////////////////////////////////////////////////////////////////


//void QbrtMapEditor::mergeFormatOnWordOrSelection( const QTextCharFormat &format )
//{
//    QTextCursor cursor = textCursor();
//    if (!cursor.hasSelection())
//        cursor.select( QTextCursor::WordUnderCursor );
//    cursor.mergeCharFormat( format );
//    mergeCurrentCharFormat( format );
//}
//
//void QbrtMapEditor::toBold( bool bold )
//{
//    QTextCharFormat fmt;
//    fmt.setFontWeight( bold ? QFont::Bold : QFont::Normal );
//    mergeFormatOnWordOrSelection( fmt );
//}
//
//void QbrtMapEditor::toItalic( bool italic )
//{
//    QTextCharFormat fmt;
//    fmt.setFontItalic( italic );
//    mergeFormatOnWordOrSelection( fmt );
//}
//
//void QbrtMapEditor::toUnderline( bool underline )
//{
//    QTextCharFormat fmt;
//    fmt.setFontUnderline( underline );
//    mergeFormatOnWordOrSelection( fmt );
//}

//bool QbrtMapEditor::selectFont()
//{
//    bool ok;
//    QFont f = QFontDialog::getFont( &ok, currentFont(), this ); // or font(), for all text
//    if (ok) {
//        setCurrentFont( f );                                    // or setFont(), for all text
//    }
//    return ok;
//}
//
//bool QbrtMapEditor::selectColor()
//{
//    QColor col = QColorDialog::getColor( textColor(), this );
//    if ( !col.isValid() )
//        return false;
//    QTextCharFormat fmt;
//    fmt.setForeground(col);
//    mergeFormatOnWordOrSelection( fmt );
//    return true;
//}
//
//void QbrtMapEditor::toFontFamily( const QString &f )
//{
//    QTextCharFormat fmt;
//    fmt.setFontFamily(f);
//    mergeFormatOnWordOrSelection( fmt );
//}
//
//void QbrtMapEditor::toFontSize( const QString &p )
//{
//    qreal pointSize = p.toFloat();
//    if (p.toFloat() > 0) {
//        QTextCharFormat fmt;
//        fmt.setFontPointSize(pointSize);
//        mergeFormatOnWordOrSelection( fmt );
//    }
//}
//
//void QbrtMapEditor::toListStyle( QTextListFormat::Style style/* = QTextListFormat::ListDisc */)
//{
//    QTextCursor cursor = textCursor();
//
//    if ( style != QTextListFormat::ListStyleUndefined )
//    {
//        cursor.beginEditBlock();
//        QTextBlockFormat blockFmt = cursor.blockFormat();
//        QTextListFormat listFmt;
//        if (cursor.currentList())
//        {
//            listFmt = cursor.currentList()->format();
//        } else {
//            listFmt.setIndent(blockFmt.indent() + 1);
//            blockFmt.setIndent(0);
//            cursor.setBlockFormat(blockFmt);
//        }
//        listFmt.setStyle(style);
//        cursor.createList(listFmt);
//        cursor.endEditBlock();
//    } else {
//        // ####
//        QTextBlockFormat bfmt;
//        bfmt.setObjectIndex(-1);
//        cursor.mergeBlockFormat(bfmt);
//    }
//}


//////////////////////////////////////////////////////////////////////////
//                              PRINT ACTIONS
//////////////////////////////////////////////////////////////////////////


//void QbrtMapEditor::PrintToPdf()
//{
//#ifndef QT_NO_PRINTER
////! [0]
//    QString fileName = QFileDialog::getSaveFileName(this, "Export PDF", QString(), "*.pdf");
//    if (!fileName.isEmpty()) {
//        if (QFileInfo(fileName).suffix().isEmpty())
//            fileName.append(".pdf");
//        QPrinter printer(QPrinter::HighResolution);
//        printer.setOutputFormat(QPrinter::PdfFormat);
//        printer.setOutputFileName(fileName);
//        document()->print(&printer);
//    }
////! [0]
//#endif
//}
//
//void QbrtMapEditor::PrintToPrinter()
//{
//#ifndef QT_NO_PRINTER
//    QPrinter printer(QPrinter::HighResolution);
//    QPrintDialog *dlg = new QPrintDialog(&printer, this);
//    if (textCursor().hasSelection())
//        dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);
//    dlg->setWindowTitle(tr("Print Document"));
//    if (dlg->exec() == QDialog::Accepted) {
//        print(&printer);
//    }
//    delete dlg;
//#endif
//}
//
//void QbrtMapEditor::PrintPreview()
//{
//#ifndef QT_NO_PRINTER
//    QPrinter printer(QPrinter::HighResolution);
//    QPrintPreviewDialog preview(&printer, this);
//    connect(&preview, SIGNAL(paintRequested(QPrinter*)), SLOT(printPreviewSlot(QPrinter*)));
//    preview.exec();
//#endif
//}

//void QbrtMapEditor::printPreviewSlot(QPrinter *printer)
//{
//#ifdef QT_NO_PRINTER
//    Q_UNUSED(printer);
//#else
//    print(printer);
//#endif
//}
//


#define _POSIX_SOURCE 1        /* GSHHS code is POSIX compliant */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

//#ifdef WIN32
//#pragma warning( disable : 4996 )
//#endif

#ifndef M_PI
#define M_PI          3.14159265358979323846
#endif

#ifndef SEEK_CUR    /* For really ancient systems */
#define SEEK_CUR 1
#endif

#define GSHHS_DATA_RELEASE    7    /* For v2.0 data set */
#define GSHHS_DATA_VERSION    "2.0"    /* For v2.0 data set */
#define GSHHS_PROG_VERSION    "1.12"

#define GSHHS_SCL    1.0e-6    /* Convert micro-degrees to degrees */

/* For byte swapping on little-endian systems (GSHHS is defined to be bigendian) */

#define swabi4(i4) (((i4) >> 24) + (((i4) >> 8) & 65280) + (((i4) & 65280) << 8) + (((i4) & 255) << 24))

const char * file2 = "L:\\project\\dev\\source\\data\\stuff\\ascii.dat";


struct GSHHS {    /* Global Self-consistent Hierarchical High-resolution Shorelines */
    int id;        /* Unique polygon id number, starting at 0 */
    int n;        /* Number of points in this polygon */
    int flag;    /* = level + version << 8 + greenwich << 16 + source << 24 + river << 25 */
    /* flag contains 5 items, as follows:
     * low byte:    level = flag & 255: Values: 1 land, 2 lake, 3 island_in_lake, 4 pond_in_island_in_lake
     * 2nd byte:    version = (flag >> 8) & 255: Values: Should be 7 for GSHHS release 7
     * 3rd byte:    greenwich = (flag >> 16) & 1: Values: Greenwich is 1 if Greenwich is crossed
     * 4th byte:    source = (flag >> 24) & 1: Values: 0 = CIA WDBII, 1 = WVS
     * 4th byte:    river = (flag >> 25) & 1: Values: 0 = not set, 1 = river-lake and level = 2
     */
    int west, east, south, north;    /* min/max extent in micro-degrees */
    int area;    /* Area of polygon in 1/10 km^2 */
    int area_full;    /* Area of original full-resolution polygon in 1/10 km^2 */
    int container;    /* Id of container polygon that encloses this polygon (-1 if none) */
    int ancestor;    /* Id of ancestor polygon in the full resolution set that was the source of this polygon (-1 if none) */
};

struct    TPOINT {    /* Each lon, lat pair is stored in micro-degrees in 4-byte integer format */
    int    x;
    int    y;
};


bool gshhs(int argc, char **argv)
{
    double w, e, s, n, area, f_area, lon, lat;
    char source, kind[2] = {'P', 'L'}, c = '>', *file = NULL;
    const char *name[2] = {"polygon", "line"};
    char container[800], ancestor[800];
    FILE *fp = NULL;
    FILE *fp2 = NULL;
    int k, line, max_east = 270000000, info, single, error, ID, flip;
    int  OK, level, version, greenwich, river, src, msformat = 0;
    struct    TPOINT p;
    struct GSHHS h;

    info = single = error = ID = 0;
    for (k = 1; k < argc; k++) {
        if (argv[k][0] == '-') {    /* Option switch */
            switch (argv[k][1]) {
                case 'L':
                    info = 1;
                    break;
                case 'M':
                    msformat = 1;
                    break;
                case 'I':
                    single = 1;
                    ID = atoi (&argv[k][2]);
                    break;
                default:
                    error++;
                    break;
            }
        }
        else
            file = argv[k];
    }

    if (!file) {
        fprintf (stderr, "gshhs: No data file given!\n");
        error++;
    }
    if (argc == 1 || error) {
        fprintf (stderr, "gshhs %s - ASCII export of GSHHS %s data\n", GSHHS_PROG_VERSION, GSHHS_DATA_VERSION);
        fprintf (stderr, "usage:  gshhs gshhs_[f|h|i|l|c].b [-I<id>] [-L] [-M] > ascii.dat\n");
        fprintf (stderr, "-L will only list headers (no data output)\n");
        fprintf (stderr, "-I only output data for polygon number <id> [Default is all polygons]\n");
        fprintf (stderr, "-M will write '>' at start of each segment header [P or L]\n");
        return false;
    }

    if ((fp = fopen (file, "rb")) == NULL ) {
        fprintf (stderr, "gshhs:  Could not find file %s.\n", file);
        return false;
    }

    if ((fp2 = fopen (file2, "w")) == NULL ) {
        fprintf (stderr, "gshhs:  Could not create file %s.\n", file2);
        return false;
    }

    size_t n_read = fread ((void *)&h, (size_t)sizeof (struct GSHHS), (size_t)1, fp);
    version = (h.flag >> 8) & 255;
	flip = ( version != GSHHS_DATA_RELEASE );    /* Take as sign that byte-swabbing is needed */

    while (n_read == 1) {
        if (flip) {
            h.id = swabi4 ((unsigned int)h.id);
            h.n  = swabi4 ((unsigned int)h.n);
            h.west  = swabi4 ((unsigned int)h.west);
            h.east  = swabi4 ((unsigned int)h.east);
            h.south = swabi4 ((unsigned int)h.south);
            h.north = swabi4 ((unsigned int)h.north);
            h.area  = swabi4 ((unsigned int)h.area);
            h.area_full  = swabi4 ((unsigned int)h.area_full);
            h.flag  = swabi4 ((unsigned int)h.flag);
            h.container  = swabi4 ((unsigned int)h.container);
            h.ancestor  = swabi4 ((unsigned int)h.ancestor);
        }
        level = h.flag & 255;                /* Level is 1-4 */
        version = (h.flag >> 8) & 255;            /* Version is 1-7 */
        greenwich = (h.flag >> 16) & 1;            /* Greenwich is 0 or 1 */
        src = (h.flag >> 24) & 1;            /* Greenwich is 0 (WDBII) or 1 (WVS) */
        river = (h.flag >> 25) & 1;            /* River is 0 (not river) or 1 (is river) */
        w = h.west  * GSHHS_SCL;            /* Convert from microdegrees to degrees */
        e = h.east  * GSHHS_SCL;
        s = h.south * GSHHS_SCL;
        n = h.north * GSHHS_SCL;
        source = (src == 1) ? 'W' : 'C';        /* Either WVS or CIA (WDBII) pedigree */
        if (river) source = tolower ((int)source);    /* Lower case c means river-lake */
        line = (h.area) ? 0 : 1;            /* Either Polygon (0) or Line (1) (if no area) */
        area = 0.1 * h.area;                /* Now im km^2 */
        f_area = 0.1 * h.area_full;                /* Now im km^2 */

        OK = (!single || h.id == ID);

        if (!msformat) c = kind[line];
        if (OK) {
            if (line)
                fprintf(fp2, "%c %6d%8d%2d%2c%10.5f%10.5f%10.5f%10.5f\n", c, h.id, h.n, level, source, w, e, s, n);
            else {
                (h.container == -1) ? sprintf (container, "-") : sprintf (container, "%6d", h.container);
                (h.ancestor == -1) ? sprintf (ancestor, "-") : sprintf (ancestor, "%6d", h.ancestor);
                fprintf(fp2, "%c %6d%8d%2d%2c%13.3f%13.3f%10.5f%10.5f%10.5f%10.5f %s %s\n", c, h.id, h.n, level, source, area, f_area, w, e, s, n, container, ancestor);
            }
        }

        if (info || !OK) {    /* Skip data, only want headers */
            fseek (fp, (long)(h.n * sizeof(struct TPOINT)), SEEK_CUR);
        }
        else {
            for (k = 0; k < h.n; k++) {

                if (fread ((void *)&p, (size_t)sizeof(struct TPOINT), (size_t)1, fp) != 1) {
                    fprintf (stderr, "gshhs:  Error reading file %s for %s %d, point %d.\n", argv[1], name[line], h.id, k);
					break;	// return false;
                }
                if (flip) {
                    p.x = swabi4 ((unsigned int)p.x);
                    p.y = swabi4 ((unsigned int)p.y);
                }
                lon = p.x * GSHHS_SCL;
                if ((greenwich && p.x > max_east) || (h.west > 180000000)) lon -= 360.0;
                lat = p.y * GSHHS_SCL;
                fprintf(fp2, "%11.6f%11.6f\n", lon, lat);
            }
        }
        max_east = 180000000;    /* Only Eurasia needs 270 */
        n_read = fread((void *)&h, (size_t)sizeof (struct GSHHS), (size_t)1, fp);
    }

    fclose (fp);
    fclose (fp2);

    return true;
}


///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_QbrtMapEditor.cpp"
