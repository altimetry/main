#include "new-gui/brat/stdafx.h"

#include "new-gui/Common/QtUtilsIO.h"

#include "DataModels/DisplayFilesProcessor.h"
#include "DataModels/Workspaces/Display.h"
#include "DataModels/PlotData/MapProjection.h"
#include "DataModels/PlotData/WorldPlot.h"

#include "GUI/ActionsTable.h"
#include "GUI/ControlPanels/ViewControlPanels.h"
#include "GUI/DisplayWidgets/BratViews.h"
#include "GUI/DisplayWidgets/GlobeWidget.h"

#include "MapEditor.h"


void CMapEditor::CreateMapActions()
{
	AddToolBarSeparator();
	mActionStatisticsMean = AddToolBarAction( this, eAction_MapEditorMean );
	mActionStatisticsStDev = AddToolBarAction( this, eAction_MapEditorStDev );
	mActionStatisticsLinearRegression = AddToolBarAction( this, eAction_MapEditorLinearRegression );

	// add menu button for projections

	AddToolBarSeparator();
	mProjectionsGroup = ProjectionsActions( this );
	mToolProjection = AddMenuButton( eActionGroup_Projections, mProjectionsGroup->actions() );
}


void CMapEditor::CreateAndWireNewMap()
{
	if ( mMapView )
	{
		QWidget *p = mMapView;
		mMapView = nullptr;
		RemoveView( p, false );
	}
	mMapView = new CBratMapView( this );
	mMapView->ConnectParentRenderWidgets( mProgressBar, mRenderSuppressionCBox );
	mMapView->ConnectParentMeasureActions( mMeasureButton, mActionMeasure, mActionMeasureArea );
	mMapView->ConnectParentGridAction( mActionDecorationGrid );
	AddView( mMapView, false );
	mMapView->setVisible( false );
}

//create and insert widgets

void CMapEditor::CreateWidgets()
{
    //    Specialized Tabs
	mTabData = new CMapControlsPanelEdit( this );
	mTabView = new CMapControlsPanelOptions( this );
	mTabPlots = new CViewControlsPanelPlots( this );

	AddTab( mTabData, "Edit" );
	AddTab( mTabView, "Options" );
	AddTab( mTabPlots, "Plots" );

    //    Set Header Label Texts
    mTabPlots->mPlotInfoTable->setRowCount(1);
    mTabPlots->mPlotInfoTable->setColumnCount(5);
    mTabPlots->mPlotInfoTable->setHorizontalHeaderLabels( QString("Plot name;Expression name;Unit;Operation name;Dataset").split(";") );

	// map widget managed

	const int index = 0;
	CMapWidget::CreateRenderWidgets( statusBar(), mProgressBar, mRenderSuppressionCBox );
	statusBar()->insertPermanentWidget( index, mRenderSuppressionCBox, 0 );
	statusBar()->insertPermanentWidget( index, mProgressBar, 1 );

	mMeasureButton = CMapWidget::CreateMapMeasureActions( mToolBar, mActionMeasure, mActionMeasureArea );
	mToolBar->addAction( CActionInfo::CreateAction( mToolBar, eAction_Separator ) );
	mToolBar->addWidget( mMeasureButton );

	mActionDecorationGrid = CMapWidget::CreateGridAction( mToolBar );
	mToolBar->addAction( mActionDecorationGrid );

    //    Map
	ResetViews();				//creates map and connects map actions; hides it
	mMapView->setVisible( true );

	// Tool-bar actions

	CreateMapActions();

	Wire();
}

void CMapEditor::Wire()
{
	connect( mActionStatisticsMean, SIGNAL( triggered() ), this, SLOT( HandleStatisticsMean() ) );
	connect( mActionStatisticsStDev, SIGNAL( triggered() ), this, SLOT( HandleStatisticsStDev() ) );
	connect( mActionStatisticsLinearRegression, SIGNAL( triggered() ), this, SLOT( HandleStatisticsLinearRegression() ) );

	for ( auto a : mProjectionsGroup->actions() )
	{
		if ( a->isSeparator() )
			continue;

		assert__( a->isCheckable() );
		connect( a, SIGNAL( triggered() ), this, SLOT( HandleProjection() ) );
	}
}

CMapEditor::CMapEditor( CModel *model, COperation *operation, QWidget *parent )		//parent = nullptr
	: base_t( model, operation, parent )
{
	CreateWidgets();

#if defined (TESTING_GLOBE)

	Show3D( true );

#endif
}


CMapEditor::CMapEditor( CDisplayFilesProcessor *proc, CWPlot* wplot, QWidget *parent )		//parent = nullptr
	: base_t( true, proc, parent )
{
	CreateWidgets();

	mMapView->CreatePlot( proc->GetWorldPlotProperties( 0 ), wplot );
}


//virtual 
CMapEditor::~CMapEditor()
{
	delete mGlobeView;
	delete mMapView;
}


//virtual 
void CMapEditor::Show2D( bool checked )
{
	mMapView->setVisible( checked );
}
//virtual 
void CMapEditor::Show3D( bool checked )
{
	if ( checked && !mGlobeView )
	{
		WaitCursor wait;

		mGlobeView = new CGlobeWidget( this, mMapView, statusBar() );
		AddView( mGlobeView, true );
	}
	mGlobeView->setVisible( checked );
}


//virtual 
void CMapEditor::ResetViews()
{											 //TODO	TODO	TODO	TODO	TODO	CHECK/UNCHECK TOOLBAR
	if ( mGlobeView )
	{
		QWidget *p = mGlobeView;
		mGlobeView = nullptr;
		RemoveView( p, true );
	}
	CreateAndWireNewMap();
}


//virtual 
void CMapEditor::NewButtonClicked()
{
	NOT_IMPLEMENTED
}

void CMapEditor::PlotChanged( int index )
{
	if ( index < 0 )
		return;

	WaitCursor wait;

	try
	{
		ResetViews();

		std::vector< CWPlot* > wplots;
		if ( mDisplay->IsZLatLonType() )
			wplots = GetDisplayPlots< CWPlot >( mDisplay );			assert__( wplots.size() && mCurrentDisplayFilesProcessor );

		for ( auto *wplot : wplots )
		{
			assert__( wplot != nullptr );

			mMapView->CreatePlot( mCurrentDisplayFilesProcessor->GetWorldPlotProperties( 0 ), wplot );
		}

		mMapView->setVisible( true );

		return;
	}
	catch ( CException &e )
	{
		SimpleErrorBox( e.what() );
		throw;
	}
	catch ( std::exception &e )
	{
		SimpleErrorBox( e.what() );
		throw;
	}
	catch ( ... )
	{
		SimpleErrorBox( "Unexpected error trying to create a map plot." );
	}

	throw;
}




void CMapEditor::HandleStatisticsMean()
{
	NOT_IMPLEMENTED
}


void CMapEditor::HandleStatisticsStDev()
{
	NOT_IMPLEMENTED
}


void CMapEditor::HandleStatisticsLinearRegression()
{
	NOT_IMPLEMENTED
}


///////////////////
//	Map Projections
///////////////////

static const unsigned default_projection_id = (unsigned)-1;


QActionGroup* ProjectionsActions( QWidget *parent )
{
	static const std::vector< std::pair< EActionTag, unsigned > > ProjIdsMap =
	{
		{ eAction_Projection_LAMBERT_CYLINDRICAL,	PROJ2D_LAMBERT_CYLINDRICAL   },
		{ eAction_Projection_PLATE_CAREE,			PROJ2D_PLATE_CAREE			 },
		{ eAction_Projection_MOLLWEIDE,				PROJ2D_MOLLWEIDE			 },
		{ eAction_Projection_ROBINSON,				PROJ2D_ROBINSON				 },
		{ eAction_Projection_LAMBERT_AZIMUTHAL,		PROJ2D_LAMBERT_AZIMUTHAL	 },
		{ eAction_Projection_AZIMUTHAL_EQUIDISTANT, PROJ2D_AZIMUTHAL_EQUIDISTANT },
		{ eAction_Projection_MERCATOR,				PROJ2D_MERCATOR				 },
		{ eAction_Projection_ORTHO,					PROJ2D_ORTHO				 },
		{ eAction_Projection_NEAR_SIGHTED,			PROJ2D_NEAR_SIGHTED			 },
		{ eAction_Projection_STEREOGRAPHIC,			PROJ2D_STEREOGRAPHIC		 },
		{ eAction_Separator,						default_projection_id		 },		//any id, it doesn't matter
		{ eAction_Projection_Default,				default_projection_id		 },
	};

	static size_t size = 0;

	static const std::vector< EActionTag > ActionTags =
	{
		ProjIdsMap[size++].first,
		ProjIdsMap[size++].first,
		ProjIdsMap[size++].first,
		ProjIdsMap[size++].first,
		ProjIdsMap[size++].first,
		ProjIdsMap[size++].first,
		ProjIdsMap[size++].first,
		ProjIdsMap[size++].first,
		ProjIdsMap[size++].first,
		ProjIdsMap[size++].first,
		ProjIdsMap[size++].first,
		ProjIdsMap[size++].first,
	};


	QActionGroup *g = CActionInfo::CreateActionGroup( parent, ActionTags, true );
	auto actions = g->actions();

	// CMapProjection::3D is not in ActionTags, default_projection_id and separator are not in CMapProjection
	//
	assert__( size == ActionTags.size() && size == CMapProjection::GetInstance()->size() + 1 && size == ProjIdsMap.size() && size == actions.size() );

	for ( size_t i = 0; i < size; ++i )
	{
		auto *a = actions[ (int) i ];
		if ( a->isSeparator() )
			continue;

		auto id = ProjIdsMap[ i ].second;
		if ( (unsigned)id == (unsigned)default_projection_id )
			a->setChecked( true );
		else
		{
			a->setData( QVariant::fromValue( id ) );
			a->setText( a->text() + " - C.R.S. " + CMapProjection::GetInstance()->IdToCRS( id ).description() );
		}
	}

	return g;
}

void CMapEditor::HandleProjection()
{
	assert__( mMapView );

	auto a = qobject_cast<QAction*>( sender() );				assert__( a && mProjectionsGroup->actions().indexOf( a ) >= 0 );

	unsigned proj_id = a->data().toUInt();

	if ( proj_id == PROJ2D_3D )
		Show3D( true );
	else
	if ( proj_id == default_projection_id )
		mMapView->SetDefaultProjection();
	else
		mMapView->SetProjection( proj_id );
}


//virtual 
void CMapEditor::DatasetsIndexChanged( int index )
{
    Q_UNUSED(index);

	NOT_IMPLEMENTED
}
//virtual 
void CMapEditor::FiltersIndexChanged( int index )
{
    Q_UNUSED(index);

    NOT_IMPLEMENTED
}
//virtual 
void CMapEditor::OneClick()
{
	NOT_IMPLEMENTED
}




///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_MapEditor.cpp"
