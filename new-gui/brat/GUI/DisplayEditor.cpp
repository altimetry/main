#include "new-gui/brat/stdafx.h"

#include "new-gui/Common/QtUtilsIO.h"

#include "new-gui/brat/GUI/ActionsTable.h"
#include "new-gui/brat/GUI/TabbedDock.h"
#include "new-gui/brat/GUI/ControlsPanel.h"
#include "new-gui/brat/Views/GlobeWidget.h"
#include "new-gui/brat/Views/2DPlotWidget.h"
#include "new-gui/brat/Views/3DPlotWidget.h"
#include "new-gui/brat/Views/TextWidget.h"


#include "DisplayEditor.h"


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


void CAbstractDisplayEditor::CreateWorkingDock( CControlsPanel *controls )
{
	setDockOptions( dockOptions() & ~AnimatedDocks );	//TODO: DELETE AFTER CHECKING WINDOWS BLUE SCREENS

	//dock
	mWorkingDock = new CTabbedDock( QString( "View Panel" ) + "[*]", this );
	mWorkingDock->setMinimumSize( min_editor_dock_width, min_editor_dock_height );
	auto PreventActions = QDockWidget::DockWidgetClosable;							//QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable | 
	mWorkingDock->setFeatures( mWorkingDock->features() & ~PreventActions );

	mWorkingDock->AddTab( controls, "" );
	addDockWidget( Qt::DockWidgetArea::LeftDockWidgetArea, mWorkingDock );

	CActionInfo::SetDockActionProperties( mWorkingDock, eAction_DisplayEditorDock );		//TODO: Remove this if the action is not added to the DisplayEditor tool-bar
}


void CAbstractDisplayEditor::CreateGraphicsBar()
{
    mToolBar = new QToolBar( "View ToolBar", this );
    mToolBar->setIconSize( QSize( 16, 16 ) );

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

void CAbstractDisplayEditor::CreateStatusBar()
{
}

CAbstractDisplayEditor::CAbstractDisplayEditor( CControlsPanel *controls, QWidget *parent )	//parent = nullptr, QWidget *view = nullptr ) 
	: base_t( parent )
{
	setAttribute( Qt::WA_DeleteOnClose );

	CreateWorkingDock( controls );		//should be called by derived in their ctors
	CreateGraphicsBar();
	CreateStatusBar();

	mMainSplitter = createSplitterIn( this, Qt::Vertical );
	mGraphicsSplitter = createSplitterIn( mMainSplitter, Qt::Horizontal );
	mLogText = new CTextWidget( this );
	mMainSplitter->addWidget( mLogText );
	mLogText->hide();
}

//virtual 
CAbstractDisplayEditor::~CAbstractDisplayEditor()
{}

bool CAbstractDisplayEditor::AddView( QWidget *view, bool view3D )
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

QAction* CAbstractDisplayEditor::AddToolBarAction( QObject *parent, EActionTag tag )
{
	QAction *a = CActionInfo::CreateAction( parent, tag );
	mToolBar->addAction( a );
	return a;
}

QAction* CAbstractDisplayEditor::AddToolBarSeparator()
{
	return mToolBar->addSeparator();
}

QToolButton* CAbstractDisplayEditor::AddMenuButton( EActionTag button_tag, const QList<QAction*> &actions )
{
	QToolButton *b = CActionInfo::CreateMenuButton( button_tag, actions );
	mToolBar->addWidget( b );
	return b;
}



void CAbstractDisplayEditor::Handle2D_slot( bool checked )
{
	Handle2D( checked );
}
void CAbstractDisplayEditor::Handle3D_slot( bool checked )
{
	Handle3D( checked );
}
void CAbstractDisplayEditor::HandleLog_slot( bool checked )
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

CMapEditor::CMapEditor( QWidget *parent )		//parent = nullptr
	: base_t( new CMapViewControls, parent )
{
	CreateMapActions();

	mMap = new CMapWidget( this );
	mGlobe = new CGlobeWidget( this, ApplicationDirectories::instance().mGlobeDir, mMap );
	AddView( mMap, false );
	AddView( mGlobe, true );

	// According to the selected projection index (instead of hard-coded 0):
	//
	mProjectionsGroup->actions()[ 0 ]->setChecked( true );
}

//virtual 
CMapEditor::~CMapEditor()
{}


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

CPlotEditor::CPlotEditor( QWidget *parent ) 	//parent = nullptr
	: base_t( new CPlotViewControls, parent )
{
	mPlot2D = new C2DPlotWidget( ePlotDefault, this );
	mPlot3D = new C3DPlotWidget( this );
	AddView( mPlot2D, false );
	AddView( mPlot3D, true );
}

//virtual 
CPlotEditor::~CPlotEditor()
{}

//virtual 
void CPlotEditor::Handle2D( bool checked )
{
	mPlot2D->setVisible( checked );
}
//virtual 
void CPlotEditor::Handle3D( bool checked )
{
	mPlot3D->setVisible( checked );
}
//virtual 
void CPlotEditor::HandleLog( bool checked )
{
	mLogText->setVisible( checked );
}





///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_DisplayEditor.cpp"
