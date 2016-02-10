#include "new-gui/brat/stdafx.h"

#include "new-gui/Common/QtUtilsIO.h"

#include "new-gui/brat/DataModels/Workspaces/Workspace.h"
#include "new-gui/brat/DataModels/Workspaces/Display.h"
#include "new-gui/brat/DataModels/CmdLineProcessor.h"
#include "display/PlotData/WPlot.h"

#include "new-gui/brat/GUI/ActionsTable.h"
#include "new-gui/brat/GUI/TabbedDock.h"
#include "new-gui/brat/GUI/ControlsPanel.h"
#include "new-gui/brat/GUI/ViewControlsPanel.h"
#include "new-gui/brat/Views/BratWidgets.h"
#include "new-gui/brat/Views/GlobeWidget.h"
#include "new-gui/brat/Views/TextWidget.h"

#include "ViewEditor.h"


/////////////////////////////////////////////////////////////////
//	TODO: move to proper place after using real projections
/////////////////////////////////////////////////////////////////

QActionGroup* ProjectionsActions( QWidget *parent )
{
	std::vector<EActionTag> ActionTags =
	{
		eAction_Projection1,
		eAction_Projection2,
		eAction_Projection3,
	};

	return CActionInfo::CreateActionsGroup( parent, ActionTags, true );
}







#define DE_NOT_IMPLEMENTED( checked )     FNOT_IMPLEMENTED( n2s<std::string>( checked ) + __func__ );


///////////////////////////////////////////////////////////////////////////////////////////
//	Base
///////////////////////////////////////////////////////////////////////////////////////////

void CAbstractViewEditor::FilterDisplays( bool with_maps )
{
	auto &displays = *mWksd->GetDisplays();
	std::string errors;
	for ( auto &display_entry : displays )
	{
		CmdLineProcessor *p = new CmdLineProcessor;
		CDisplay *display = dynamic_cast<CDisplay*>( display_entry.second );		assert__( display );
		try
		{
			p->Process( display->GetCmdFilePath() );
			if ( with_maps ? p->isZFLatLon() : !p->isZFLatLon() )
			{
				mCmdLineProcessors.push_back( p );
				continue;
			}
		}
		catch ( const CException &e )
		{
			errors += ( e.Message() + "\n" );
		}
		delete p;
	}

	if ( !errors.empty() )
		SimpleWarnBox( errors );
}



void CAbstractViewEditor::CreateWorkingDock()
{
	setDockOptions( dockOptions() & ~AnimatedDocks );	//TODO: DELETE AFTER CHECKING WINDOWS BLUE SCREENS

	//dock
	mWorkingDock = new CTabbedDock( QString( "View Panel" ) + "[*]", this );
	mWorkingDock->setMinimumSize( min_editor_dock_width, min_editor_dock_height );
	auto PreventActions = QDockWidget::DockWidgetClosable;							//QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable | 
	mWorkingDock->setFeatures( mWorkingDock->features() & ~PreventActions );

	addDockWidget( Qt::DockWidgetArea::BottomDockWidgetArea, mWorkingDock );

	CActionInfo::SetDockActionProperties( mWorkingDock, eAction_DisplayEditorDock );		//TODO: Remove this if the action is not added to the DisplayEditor tool-bar
}


void CAbstractViewEditor::CreateGraphicsBar()
{
    mToolBar = new QToolBar( "View ToolBar", this );
    mToolBar->setIconSize({icon_size,icon_size});

	// add button group

	QActionGroup *group = CActionInfo::CreateActionsGroup( this, { eAction_DisplayEditor2D, eAction_DisplayEditor3D, eAction_DisplayEditorLog }, false );

	mAction2D = group->actions()[0];														assert__( mAction2D->isCheckable() );
	connect( mAction2D, SIGNAL( toggled( bool ) ), this, SLOT( Handle2D_slot( bool ) ) );

	mAction3D = group->actions()[1];														assert__( mAction3D->isCheckable() );
	connect( mAction3D, SIGNAL( toggled( bool ) ), this, SLOT( Handle3D_slot( bool ) ) );

	mLogAction = group->actions()[2];														assert__( mLogAction->isCheckable() );
	connect( mLogAction, SIGNAL( toggled( bool ) ), this, SLOT( HandleLog_slot( bool ) ) );

	mToolBar->addActions( group->actions() );

	// add the bar

	mToolBar->setAllowedAreas( Qt::RightToolBarArea );
    addToolBar( Qt::RightToolBarArea, mToolBar );
}

void CAbstractViewEditor::CreateStatusBar()
{
}

CAbstractViewEditor::CAbstractViewEditor( CWorkspaceDisplay *wksd, QWidget *parent )	//parent = nullptr, QWidget *view = nullptr ) 
	: base_t( parent )
	, mWksd( wksd )
{
	setAttribute( Qt::WA_DeleteOnClose );

	CreateWorkingDock();
	CreateGraphicsBar();
	CreateStatusBar();

	mMainSplitter = createSplitterIn( this, Qt::Vertical );
	setCentralWidget( mMainSplitter );
	mGraphicsSplitter = createSplitterIn( mMainSplitter, Qt::Horizontal );
	mLogText = new CTextWidget( this );
	mMainSplitter->addWidget( mLogText );
	mLogText->hide();
}


QWidget* CAbstractViewEditor::AddTab( QWidget *tab , const QString title )		// title = "" 
{
	return mWorkingDock->AddTab( tab, title );
}


bool CAbstractViewEditor::AddView( QWidget *view, bool view3D )
{
	if ( mGraphicsSplitter->count() == 2 )
		return false;

	//connect( view, SIGNAL( closed( QWidget* ) ), this, SLOT( SubWindowClosed( QWidget* ) ) );
	mGraphicsSplitter->addWidget( view );

	if ( view3D )
		mAction3D->setChecked( true );
	else
		mAction2D->setChecked( true );

	return true;
}


QAction* CAbstractViewEditor::AddToolBarAction( QObject *parent, EActionTag tag )
{
	QAction *a = CActionInfo::CreateAction( parent, tag );
	mToolBar->addAction( a );
	return a;
}


QAction* CAbstractViewEditor::AddToolBarSeparator()
{
	return mToolBar->addSeparator();
}

QToolButton* CAbstractViewEditor::AddMenuButton( EActionTag button_tag, const QList<QAction*> &actions )
{
	QToolButton *b = CActionInfo::CreateMenuButton( button_tag, actions );
	mToolBar->addWidget( b );
	return b;
}



void CAbstractViewEditor::Handle2D_slot( bool checked )
{
	Handle2D( checked );
}
void CAbstractViewEditor::Handle3D_slot( bool checked )
{
	Handle3D( checked );
}
void CAbstractViewEditor::HandleLog_slot( bool checked )
{
	mLogText->setVisible( checked );
}




///////////////////////////////////////////////////////////////////////////////////////////
//	Maps
///////////////////////////////////////////////////////////////////////////////////////////

void CMapEditor::WireMapActions()
{
	connect( mActionMeasureDistance, SIGNAL( triggered() ), this, SLOT( HandleMeasureDistance() ) );
	connect( mActionMeasureArea, SIGNAL( triggered() ), this, SLOT( HandleMeasureArea() ) );

	connect( mActionStatisticsMean, SIGNAL( triggered() ), this, SLOT( HandleStatisticsMean() ) );
	connect( mActionStatisticsStDev, SIGNAL( triggered() ), this, SLOT( StatisticsStDev() ) );
	connect( mActionStatisticsLinearRegression, SIGNAL( triggered() ), this, SLOT( HandleStatisticsLinearRegression() ) );

	for ( auto a : mProjectionsGroup->actions() )
	{
		assert__( a->isCheckable() );
		connect( a, SIGNAL( triggered() ), this, SLOT( HandleProjection() ) );
	}
}
void CMapEditor::CreateMapActions()
{
	AddToolBarSeparator();
	mActionMeasureDistance = AddToolBarAction( this, eAction_MapEditorDistance );
	mActionMeasureArea = AddToolBarAction( this, eAction_MapEditorArea );

	AddToolBarSeparator();
	mActionStatisticsMean = AddToolBarAction( this, eAction_MapEditorMean );
	mActionStatisticsStDev = AddToolBarAction( this, eAction_MapEditorStDev );
	mActionStatisticsLinearRegression = AddToolBarAction( this, eAction_MapEditorLinearRegression );
	AddToolBarSeparator();

	// add menu button for projections

	mProjectionsGroup = ProjectionsActions( this );
	mToolProjection = AddMenuButton( eActionGroup_Projections, mProjectionsGroup->actions() );

	WireMapActions();
}


CMapEditor::CMapEditor( const CmdLineProcessor *proc, CWPlot* wplot, QWidget *parent )		//parent = nullptr
	: base_t( nullptr, parent )

	, mTabGeneral( new CViewControlsPanelGeneral( this ) )
	, mTabData( new CMapControlsPanelEdit( this ) )
	, mTabView( new CMapControlsPanelOptions( this ) )
	, mTabPlots( new CViewControlsPanelPlots( this ) )
{
	AddTab( mTabGeneral, "General" );
	AddTab( mTabData, "Data" );
	AddTab( mTabView, "View" );



	CreateMapActions();

	mMap = new CBratMapWidget( this );
	mGlobe = new CGlobeWidget( this, CMapWidget::GlobeDir(), mMap );
	AddView( mMap, false );
	AddView( mGlobe, true );

	// According to the selected projection index (instead of hard-coded 0):
	//
	mProjectionsGroup->actions()[ 0 ]->setChecked( true );

	mMap->CreatePlot( proc, wplot );
}


CMapEditor::CMapEditor( CWorkspaceDisplay *wksd, QWidget *parent )		//parent = nullptr
	: base_t( wksd, parent )

	, mTabGeneral( new CViewControlsPanelGeneral( this ) )
	, mTabData( new CMapControlsPanelEdit( this ) )
	, mTabView( new CMapControlsPanelOptions( this ) )
	, mTabPlots( new CViewControlsPanelPlots( this ) )
{
	AddTab( mTabGeneral, "General" );
	AddTab( mTabData, "Edit" );
	AddTab( mTabView, "Options" );
	AddTab( mTabPlots, "Plots" );


    //    Set Header Label Texts
    mTabPlots->mPlotInfoTable->setRowCount(1);
    mTabPlots->mPlotInfoTable->setColumnCount(5);
    mTabPlots->mPlotInfoTable->setHorizontalHeaderLabels( QString("Plot name;Expression name;Unit;Operation name;Dataset").split(";") );


	CreateMapActions();

	mMap = new CBratMapWidget( this );
	mGlobe = new CGlobeWidget( this, CMapWidget::GlobeDir(), mMap );
	AddView( mMap, false );
	AddView( mGlobe, true );

	// According to the selected projection index (instead of hard-coded 0):
	//
	mProjectionsGroup->actions()[ 0 ]->setChecked( true );

	FilterDisplays( true );

	//auto &displays = *mWksd->GetDisplays();
	//std::string errors;
	//for ( auto &display_entry : displays )
	//{
	//	CmdLineProcessor *p = new CmdLineProcessor;
	//	CDisplay *display = dynamic_cast<CDisplay*>( display_entry.second );		assert__( display );
	//	try
	//	{
	//		p->Process( display->GetCmdFilePath() );
	//		if ( p->isZFLatLon() )
	//		{
	//			mCmdLineProcessors.push_back( p );
	//			continue;
	//		}
	//	}
	//	catch ( const CException &e )
	//	{
	//		errors += ( e.Message() + "\n" );
	//	}
	//	delete p;
	//}

	//if ( !errors.empty() )
	//	SimpleWarnBox( errors );

	for ( auto *cmd : mCmdLineProcessors )
	{
		mTabGeneral->mOpenAplot->addItem( t2q( cmd->ParamFile() ) );
	}
	mTabGeneral->mOpenAplot->setCurrentIndex( 0 );

    connect( mTabGeneral->mOpenAplot, SIGNAL( currentIndexChanged(int) ), this, SLOT( PlotChanged(int) ) );
}


//virtual 
CMapEditor::~CMapEditor()
{
	delete mGlobe;
	delete mMap;
}


//virtual 
void CMapEditor::Handle2D( bool checked )
{
	mMap->setVisible( checked );
}
//virtual 
void CMapEditor::Handle3D( bool checked )
{
	mGlobe->setVisible( checked );
}
//virtual 
void CMapEditor::HandleLog( bool checked )
{
	mLogText->setVisible( checked );
}


void CMapEditor::HandlePlotChanged( int index )
{
	if ( index < 0 )
		return;

	try
	{
		CmdLineProcessor *p = mCmdLineProcessors[ index ];		assert__( p->isZFLatLon() );
		for ( CObArray::const_iterator itGroup = p->plots().begin(); itGroup != p->plots().end(); itGroup++ )
		{
			CWPlot* wplot = dynamic_cast<CWPlot*>( *itGroup );
			if ( wplot == NULL )
				continue;

			mMap->CreatePlot( p, wplot );
		}
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
		SimpleErrorBox( "Unexpected error encountered" );
		throw;
	}
}




void CMapEditor::HandleMeasureDistance()
{
	NOT_IMPLEMENTED
}


void CMapEditor::HandleMeasureArea()
{
	NOT_IMPLEMENTED
}


void CMapEditor::HandleStatisticsMean()
{
	NOT_IMPLEMENTED
}


void CMapEditor::StatisticsStDev()
{
	NOT_IMPLEMENTED
}


void CMapEditor::HandleStatisticsLinearRegression()
{
	NOT_IMPLEMENTED
}


void CMapEditor::HandleProjection()
{
	auto a = qobject_cast<QAction*>( sender() );				assert__( a );
	auto index = mProjectionsGroup->actions().indexOf( a );		assert__( index >= 0 );

	SimpleMsgBox( "Not implemented: projection # " + n2s<std::string>( index ) );
}




///////////////////////////////////////////////////////////////////////////////////////////
//	Plots
///////////////////////////////////////////////////////////////////////////////////////////

void CPlotEditor::CreatePlotViews() 	//parent = nullptr
{
	AddTab( mTabGeneral, "General" );
	AddTab( mTabData, "Edit" );
	AddTab( mTabCurveOptions, "Curve Options" );
	AddTab( mTabAxisOptions, "Axis Options" );
	AddTab( mTabPlots, "Plots" );

    //    Set Header Label Texts
    mTabPlots->mPlotInfoTable->setRowCount(1);
    mTabPlots->mPlotInfoTable->setColumnCount(8);
    mTabPlots->mPlotInfoTable->setHorizontalHeaderLabels(
                QString("Plot name;Expression name;View type;Unit;dim.1;dim.2;Operation name;Dataset").split(";"));


	mPlot2D = new CBrat2DPlotWidget( this );
	mPlot3D = new CBrat3DPlotWidget( this );
	AddView( mPlot2D, false );
	AddView( mPlot3D, true );
}

void CPlotEditor::Wire()
{
    connect( mTabGeneral->mOpenAplot, SIGNAL( currentIndexChanged(int) ), this, SLOT( PlotChanged(int) ) );

	connect( mTabAxisOptions, SIGNAL( LogarithmicScaleX( bool ) ), this, SLOT( HandleLogarithmicScaleX( bool ) ) );
	connect( mTabAxisOptions, SIGNAL( LogarithmicScaleY( bool ) ), this, SLOT( HandleLogarithmicScaleY( bool ) ) );
	connect( mTabAxisOptions, SIGNAL( LogarithmicScaleZ( bool ) ), this, SLOT( HandleLogarithmicScaleZ( bool ) ) );
}

CPlotEditor::CPlotEditor( CWorkspaceDisplay *wksd, const CmdLineProcessor *proc, CPlot* plot, QWidget *parent ) 	//parent = nullptr
	: base_t( wksd, parent )

	, mTabGeneral( new CViewControlsPanelGeneral( this ) )
	, mTabData( new CPlotControlsPanelEdit( this ) )
	, mTabCurveOptions( new CPlotControlsPanelCurveOptions( this ) )
	, mTabAxisOptions( new CPlotControlsPanelAxisOptions( this ) )
	, mTabPlots( new CViewControlsPanelPlots( this ) )
{
    Q_UNUSED(proc);     Q_UNUSED(plot);         //mPlot2D->CreatePlot( proc, plot );

    CreatePlotViews();

	FilterDisplays( false );

	for ( auto *cmd : mCmdLineProcessors )
	{
		mTabGeneral->mOpenAplot->addItem( t2q( cmd->ParamFile() ) );
	}
	mTabGeneral->mOpenAplot->setCurrentIndex( 0 );

	Wire();
}

CPlotEditor::CPlotEditor( CWorkspaceDisplay *wksd, const CmdLineProcessor *proc, CZFXYPlot* plot, QWidget *parent ) 	//parent = nullptr
	: base_t( wksd, parent )

	, mTabGeneral( new CViewControlsPanelGeneral( this ) )
	, mTabData( new CPlotControlsPanelEdit( this ) )
	, mTabCurveOptions( new CPlotControlsPanelCurveOptions( this ) )
	, mTabAxisOptions( new CPlotControlsPanelAxisOptions( this ) )
	, mTabPlots( new CViewControlsPanelPlots( this ) )
{
    CreatePlotViews();
    Q_UNUSED(proc);     Q_UNUSED(plot);     //mPlot3D->CreatePlot( proc, plot );

	FilterDisplays( false );

	for ( auto *cmd : mCmdLineProcessors )
	{
		mTabGeneral->mOpenAplot->addItem( t2q( cmd->ParamFile() ) );
	}
	mTabGeneral->mOpenAplot->setCurrentIndex( 0 );

	Wire();
}




//virtual 
CPlotEditor::~CPlotEditor()
{}

//virtual 
void CPlotEditor::Handle2D( bool checked )
{
	assert__( mPlot2D );

	mPlot2D->setVisible( checked );
}
//virtual 
void CPlotEditor::Handle3D( bool checked )
{
	assert__( mPlot3D );

	mPlot3D->setVisible( checked );
}
//virtual 
void CPlotEditor::HandleLog( bool checked )
{
	assert__( mLogText );

	mLogText->setVisible( checked );
}

//virtual 
void CPlotEditor::HandlePlotChanged( int index )
{
	WaitCursor wait;

	if ( index < 0 )
		return;

	try
	{
		CmdLineProcessor *p = mCmdLineProcessors[ index ];		assert__( !p->isZFLatLon() );
		if ( p->isYFX() )		// =================================== XYPlot();
		{
			for ( CObArray::const_iterator itGroup = p->plots().begin(); itGroup != p->plots().end(); itGroup++ )
			{
				CPlot* plot = dynamic_cast<CPlot*>( *itGroup );
				if ( plot == NULL )
					continue;

				mPlot2D->CreatePlot( p, plot );
			}
		}
		else if ( p->isZFXY() )		// =================================== ZFXYPlot();
		{
			for ( CObArray::const_iterator itGroup = p->plots().begin(); itGroup != p->plots().end(); itGroup++ )
			{
				CZFXYPlot* zfxyplot = dynamic_cast<CZFXYPlot*>( *itGroup );
				if ( zfxyplot == NULL )
					continue;

				mPlot3D->CreatePlot( p, zfxyplot );
			}
		}
		else
			assert__( false );
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
		SimpleErrorBox( "Unexpected error encountered" );
		throw;
	}
}


void CPlotEditor::HandleLogarithmicScaleX( bool log )
{
	assert__( mPlot3D );

	WaitCursor wait;

	mPlot3D->SetXLogarithmicScale( log );
}

void CPlotEditor::HandleLogarithmicScaleY( bool log )
{
	assert__( mPlot3D );

	WaitCursor wait;

	mPlot3D->SetYLogarithmicScale( log );
}

void CPlotEditor::HandleLogarithmicScaleZ( bool log )
{
	assert__( mPlot3D );

	WaitCursor wait;

	mPlot3D->SetZLogarithmicScale( log );
}




///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_ViewEditor.cpp"
