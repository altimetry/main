#include "new-gui/brat/stdafx.h"

#include "new-gui/Common/QtUtils.h"

#include "DataModels/Model.h"
#include "DataModels/DisplayFilesProcessor.h"
#include "DataModels/Workspaces/Workspace.h"
#include "DataModels/Workspaces/Display.h"

#include "GUI/ActionsTable.h"
#include "GUI/TabbedDock.h"
#include "GUI/ControlPanels/ViewControlPanels.h"
#include "GUI/DisplayWidgets/TextWidget.h"

#include "AbstractDisplayEditor.h"





///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
//	Display Editors Base Class
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


void CAbstractDisplayEditor::CreateWorkingDock()
{
	setDockOptions( dockOptions() & ~AnimatedDocks );	//TODO DELETE AFTER CHECKING WINDOWS BLUE SCREENS

	if ( !mDisplayOnlyMode )
	{
		auto ldatasets = new QLabel( "Dataset" );		ldatasets->setAlignment( Qt::AlignCenter );
		auto lfilters = new QLabel( "Filter " );			lfilters->setAlignment( Qt::AlignCenter );
		auto loperations = new QLabel( "Operation" );		loperations->setAlignment( Qt::AlignCenter );
		mDatasetsCombo = new QComboBox;
		mFiltersCombo = new QComboBox;
		mOperationsCombo = new QComboBox;

		auto *wkspc_l = LayoutWidgets( { ldatasets, lfilters, loperations, nullptr, mDatasetsCombo, mFiltersCombo, mOperationsCombo }, nullptr, 6, 2, 2, 2, 2 );
		wkspc_l->setSizeConstraint( QLayout::SetNoConstraint );

		mSaveOneClickButton = new QPushButton( "Save One Click" );
		QPixmap pix( t2q( CActionInfo::IconPath( eAction_One_Click ) ) );
		QIcon icon( pix );
		mSaveOneClickButton->setIcon( icon );
		mSaveOneClickButton->setIconSize( QSize( icon_size, icon_size ) );

		auto *quick_l = LayoutWidgets( Qt::Horizontal, { nullptr, mSaveOneClickButton }, nullptr, 6, 2, 2, 2, 2 );

		mTopFrame = new QFrame;
		mTopFrame->setWindowTitle( "QFrame::Box" );
		mTopFrame->setFrameStyle( QFrame::Panel );
		mTopFrame->setFrameShadow( QFrame::Sunken );
		mTopFrame->setObjectName( "OperationsFrame" );
		mTopFrame->setStyleSheet( "#OperationsFrame { border: 1px solid black; }" );

		LayoutWidgets( { wkspc_l, quick_l }, mTopFrame, 6, 2, 2, 2, 2, 1, 2 );
	}

	//dock
	mWorkingDock = mDisplayOnlyMode ? 
		new CTabbedDock( QString( "View Panel" ) + "[*]", this ) :
		new CTabbedDock( mTopFrame, QString( "View Panel" ) + "[*]", this );

	mWorkingDock->setMinimumSize( min_editor_dock_width, min_editor_dock_height );
	auto PreventActions = QDockWidget::DockWidgetClosable;								//QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable | 
	mWorkingDock->setFeatures( mWorkingDock->features() & ~PreventActions );

	addDockWidget( Qt::DockWidgetArea::BottomDockWidgetArea, mWorkingDock );

	CActionInfo::SetDockActionProperties( mWorkingDock, eAction_DisplayEditorDock );	//TODO: Remove this if the action is not added to the DisplayEditor tool-bar
}


void CAbstractDisplayEditor::CreateGraphicsBar()
{
    mToolBar = new QToolBar( "View ToolBar", this );
    mToolBar->setIconSize({icon_size,icon_size});

	// add button group

	QActionGroup *group = CActionInfo::CreateActionGroup( this, { eAction_DisplayEditor2D, eAction_DisplayEditor3D, eAction_DisplayEditorLog }, false );

	m2DAction = group->actions()[0];	assert__( m2DAction->isCheckable() );
	m3DAction = group->actions()[1];	assert__( m3DAction->isCheckable() );
	mLogAction = group->actions()[2];	assert__( mLogAction->isCheckable() );

	mToolBar->addActions( group->actions() );

	// add the bar

	mToolBar->setAllowedAreas( Qt::RightToolBarArea );
    addToolBar( Qt::RightToolBarArea, mToolBar );
}

void CAbstractDisplayEditor::CreateStatusBar()
{
    mStatusBar = new QStatusBar( this );
    mStatusBar->setObjectName(QString::fromUtf8("mStatusBar"));
    setStatusBar(mStatusBar);
}

void CAbstractDisplayEditor::CreateWidgets()
{
	setAttribute( Qt::WA_DeleteOnClose );

	CreateWorkingDock();
	CreateGraphicsBar();
	CreateStatusBar();

	mMainSplitter = CreateSplitterIn( this, Qt::Vertical );
	setCentralWidget( mMainSplitter );
	mGraphicsSplitter = CreateSplitterIn( mMainSplitter, Qt::Horizontal );
	mLogText = new CTextWidget( this );
	mMainSplitter->addWidget( mLogText );
	mLogText->hide();

	mTabGeneral = new CViewControlsPanelGeneral( { mDisplays.begin(), mDisplays.end() }, this );
	AddTab( mTabGeneral, "General" );

	Wire();
}

void CAbstractDisplayEditor::Wire()
{
	connect( m2DAction, SIGNAL( toggled( bool ) ), this, SLOT( Handle2D( bool ) ) );
	connect( m3DAction, SIGNAL( toggled( bool ) ), this, SLOT( Handle3D( bool ) ) );
	connect( mLogAction, SIGNAL( toggled( bool ) ), this, SLOT( HandleLog( bool ) ) );

	connect( mTabGeneral, SIGNAL( CurrentDisplayIndexChanged(int) ), this, SLOT( HandlePlotChanged(int) ) );
    connect( mTabGeneral, SIGNAL( RunButtonClicked(int) ), this, SLOT( HandlePlotChanged(int) ) );

	connect( mTabGeneral, SIGNAL( NewButtonClicked() ), this, SLOT( HandleNewButtonClicked() ) );

	connect( mDatasetsCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( HandleDatasetsIndexChanged( int ) ) );
	connect( mFiltersCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( HandleFiltersIndexChanged( int ) ) );
	connect( mSaveOneClickButton, SIGNAL( clicked() ), this, SLOT( HandleOneClickClicked() ) );

	mDatasetsCombo->setEnabled( false );
}


CAbstractDisplayEditor::CAbstractDisplayEditor( CModel *model, COperation *operation, QWidget *parent )		//QWidget *parent = nullptr ) 
	: base_t( parent )
	, mWDataset( model->Workspace< CWorkspaceDataset >() )
	, mWOperation( model->Workspace< CWorkspaceOperation >() )
	, mWDisplay( model->Workspace< CWorkspaceDisplay >() )
	, mWFormula( model->Workspace< CWorkspaceFormula >() )
	, mOperation( operation )
	, mDisplayOnlyMode( false )
{
	assert__( mWDataset && mWOperation && mWDisplay && mWFormula );

	const bool map_editor = mOperation->IsMap();

	FilterDisplays( map_editor );
	CreateWidgets();
}

CAbstractDisplayEditor::CAbstractDisplayEditor( bool map_editor, const CDisplayFilesProcessor *proc, QWidget *parent )	//QWidget *parent = nullptr ) 
	: base_t( parent )
	, mCurrentDisplayFilesProcessor( proc )
	, mDisplayOnlyMode( true )
{
    assert__( !map_editor || proc->isZFLatLon() );                  Q_UNUSED(map_editor);   //for release builds
	assert__( map_editor || proc->isYFX() || proc->isZFXY() );

	CreateWidgets();
}


//virtual 
CAbstractDisplayEditor::~CAbstractDisplayEditor()
{
	delete mCurrentDisplayFilesProcessor;
}



QSize CAbstractDisplayEditor::sizeHint() const
{
	return QSize( 72 * fontMetrics().width( 'x' ),
		25 * fontMetrics().lineSpacing() );
}



QWidget* CAbstractDisplayEditor::AddTab( QWidget *tab , const QString title )		// title = "" 
{
	return mWorkingDock->AddTab( tab, title );
}


bool CAbstractDisplayEditor::AddView( QWidget *view, bool view3D )
{
	if ( mGraphicsSplitter->count() == 2 )
		return false;

	if ( view3D )
	{
		m3DAction->setChecked( true );
		mGraphicsSplitter->insertWidget( 1, view );
	}
	else
	{
		m2DAction->setChecked( true );
		mGraphicsSplitter->insertWidget( 0, view );
	}

	return true;
}

bool CAbstractDisplayEditor::RemoveView( QWidget *view, bool view3D )
{
	const int count = mGraphicsSplitter->count();
	if ( count == 0 )
		return false;

	delete view;				assert__( mGraphicsSplitter->count() == count - 1 );

	if ( view3D )
	{
		m3DAction->blockSignals( true );
		m3DAction->setChecked( false );
		m3DAction->blockSignals( false );
	}
	else
	{
		m2DAction->blockSignals( true );
		m2DAction->setChecked( false );
		m2DAction->blockSignals( false );
	}

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



void CAbstractDisplayEditor::HandlePlotChanged( int index )
{
	mDisplay = index < 0 ? nullptr : mDisplays[ index ];
	delete mCurrentDisplayFilesProcessor;
	mCurrentDisplayFilesProcessor = nullptr;
	
	if ( mDisplay )
	{
		std::string msg;
		if ( !ControlSolidColor() || !ControlVectorComponents( msg ) )
		{
			SimpleErrorBox( msg );

			//TODO de-select display in widget list 

			index = -1;
		}
		else
		{
			// TODO for a complete re-implementation of CDisplayPanel::Execute() here
			//	is missing (after really checking) a call to a gigantic CDisplayPanel::RefreshSelectedData();
			//
			//if ( !RefreshSelectedData() )
			//{
			//	return;
			//}

			if ( !mDisplay->BuildCmdFile( msg) )
			{
				assert__( !msg.empty() );

				SimpleErrorBox( msg );

				//TODO de-select display in widget list 

				index = -1;
			}
		}
	}

	PlotChanged( index );
}


void CAbstractDisplayEditor::Handle2D( bool checked )
{
	Show2D( checked );
}


void CAbstractDisplayEditor::Handle3D( bool checked )
{
	Show3D( checked );
}


void CAbstractDisplayEditor::HandleLog( bool checked )
{
	mLogText->setVisible( checked );
}



/////////////////////////////
//	Domain Processing Helpers
/////////////////////////////

void CAbstractDisplayEditor::FilterDisplays( bool with_maps )
{
	auto const &displays = *mWDisplay->GetDisplays();
	std::string errors;
	for ( auto const &display_entry : displays )
	{
		CDisplay *display = dynamic_cast<CDisplay*>( display_entry.second );		assert__( display );
		try
		{
			if ( with_maps ? display->IsZLatLonType() : !display->IsZLatLonType() )
			{
				mDisplays.push_back( display );
				continue;
			}
		}
		catch ( const CException &e )
		{
			errors += ( e.Message() + "\n" );
		}
	}

	if ( !errors.empty() )
		SimpleWarnBox( errors );
}


bool CAbstractDisplayEditor::ControlSolidColor()
{
	assert__( mDisplay != nullptr );

	CMapDisplayData* selectedData =  mDisplay->GetDataSelected();

	for ( CMapDisplayData::const_iterator itSel = selectedData->begin(); itSel != selectedData->end(); itSel ++ )
	{
		CDisplayData* dataSel = selectedData->GetDisplayData( itSel );
		if ( dataSel == nullptr )
			continue;

		if ( !dataSel->IsSolidColor() && !dataSel->IsContour() )
			dataSel->SetSolidColor( true );
	}
	return true;
}


bool CAbstractDisplayEditor::ControlVectorComponents( std::string& msg )
{
	assert__( mDisplay != nullptr );

	bool xcomponent = false;
	bool ycomponent = false;

	CMapDisplayData* selectedData =  mDisplay->GetDataSelected();

	for ( CMapDisplayData::const_iterator itSel = selectedData->begin(); itSel != selectedData->end(); itSel ++ )
	{
		CDisplayData* dataSel = selectedData->GetDisplayData( itSel );
		if ( dataSel == nullptr )
			continue;

		if ( dataSel->IsEastComponent() ) 
		{
			if ( xcomponent ) 
			{
				msg = "More than one field selected as East Component!";
				return false;
			}
			xcomponent = true;
		}

		if ( dataSel->IsNorthComponent() ) 
		{
			if ( ycomponent ) 
			{
				msg = "More than one field selected as North Component!";
				return false;
			}
			ycomponent = true;
		}
	}

	if ( xcomponent == ycomponent )
		return true;

	msg = "Both East and North fields must be selected for a Vector Plot!";

	return false;
}





///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_AbstractDisplayEditor.cpp"
