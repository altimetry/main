#include "new-gui/brat/stdafx.h"

#include "new-gui/Common/QtUtils.h"
#include "ApplicationLogger.h"

#include "DataModels/Model.h"
#include "DataModels/Workspaces/Display.h"
#include "DataModels/DisplayFilesProcessor.h"
#include "DataModels/Workspaces/Workspace.h"
#include "DataModels/Workspaces/Display.h"

#include "GUI/ActionsTable.h"
#include "GUI/TabbedDock.h"
#include "GUI/ControlPanels/Views/ViewControlPanels.h"
#include "GUI/DisplayWidgets/TextWidget.h"

#include "AbstractDisplayEditor.h"





///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
//	Display Editors Base Class
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


inline QToolBar* AddWidgets( QToolBar *toolbar, const std::vector< QObject* > &v )
{
	for ( auto ob : v )
	{
		if ( !ob )
		{
			toolbar->addSeparator();
			continue;
		}

		auto w = qobject_cast<QWidget*>( ob );		assert__( w );
		if ( w )
			toolbar->addWidget( w );
	}
	return toolbar;
}

void CAbstractDisplayEditor::CreateWorkingDock()
{
	setDockOptions( dockOptions() & ~AnimatedDocks );	//TODO DELETE AFTER CHECKING WINDOWS BLUE SCREENS

	if ( !mDisplayOnlyMode )
	{
		QToolBar *toptoolbar = new QToolBar( "View ToolBar", this );
		toptoolbar->setIconSize({tool_icon_size,tool_icon_size});
		toptoolbar->setAllowedAreas( Qt::TopToolBarArea );
		toptoolbar->setMovable( false );

		auto loperations = new QLabel( "Operation" );
		auto lfilters = new QLabel( "Filter" );
		auto ldataset = new QLabel( "Dataset" );
		mOperationsCombo = new QComboBox;
		mFiltersCombo = new QComboBox;
		mDatasetName = new QLineEdit;
		mDatasetName->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );

		mOperationsCombo->setToolTip( "Operation" );
		mFiltersCombo->setToolTip( "Filter" );
		mDatasetName->setToolTip( "Dataset" );

		mSaveOneClickButton = new QPushButton( "One Click" );
		QPixmap pix( t2q( CActionInfo::IconPath( eAction_One_Click ) ) );
		QIcon icon( pix );
		mSaveOneClickButton->setIcon( icon );

		addToolBar( AddWidgets( toptoolbar,
		{ 
			loperations, mOperationsCombo, nullptr, lfilters, mFiltersCombo, nullptr, ldataset, mDatasetName, nullptr, mSaveOneClickButton
		} 
		) );
	}

	//dock
	assert__( mOperation );
	mWorkingDock = new CTabbedDock( mOperation->IsMap()? "Map Views" : "Plot Views", this );

	mWorkingDock->setMinimumSize( min_editor_dock_width, min_editor_dock_height );
	auto PreventActions = QDockWidget::DockWidgetClosable;								//QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable | 
	mWorkingDock->setFeatures( mWorkingDock->features() & ~PreventActions );

	addDockWidget( Qt::DockWidgetArea::BottomDockWidgetArea, mWorkingDock );

	CActionInfo::SetDockActionProperties( mWorkingDock, eAction_DisplayEditorDock );	//TODO: Remove this if the action is not added to the DisplayEditor tool-bar
}


void CAbstractDisplayEditor::CreateGraphicsBar()
{
    mToolBar = new QToolBar( "View ToolBar", this );
    mToolBar->setIconSize({tool_icon_size,tool_icon_size});

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

template< class GENERAL_TAB >
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

	mTabGeneral = new GENERAL_TAB( this );
	AddTab( mTabGeneral, "General" );
	mDisplaysCombo = mTabGeneral->mDisplaysCombo;

	Wire();
}

// force template instantiation
//
template void CAbstractDisplayEditor::CreateWidgets< CViewControlsPanelGeneralMaps >();
template void CAbstractDisplayEditor::CreateWidgets< CViewControlsPanelGeneralPlots >();



void CAbstractDisplayEditor::Wire()
{
	connect( m2DAction, SIGNAL( toggled( bool ) ), this, SLOT( Handle2D( bool ) ) );
	connect( m3DAction, SIGNAL( toggled( bool ) ), this, SLOT( Handle3D( bool ) ) );
	connect( mLogAction, SIGNAL( toggled( bool ) ), this, SLOT( HandleLog( bool ) ) );

	connect( mTabGeneral, SIGNAL( CurrentDisplayIndexChanged(int) ), this, SLOT( HandleViewChanged(int) ) );
    connect( mTabGeneral, SIGNAL( RunButtonClicked(int) ), this, SLOT( HandleViewChanged(int) ) );

	connect( mTabGeneral, SIGNAL( NewButtonClicked() ), this, SLOT( HandleNewButtonClicked() ) );

	connect( mOperationsCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( HandleOperationsIndexChanged( int ) ) );
	connect( mFiltersCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( HandleFiltersIndexChanged( int ) ) );
	connect( mSaveOneClickButton, SIGNAL( clicked() ), this, SLOT( HandleOneClickClicked() ) );

	mDatasetName->setReadOnly( true );

	mOperationsCombo->blockSignals( true );
	FillCombo( mOperationsCombo, mFilteredOperations,

		-1, true,

		[]( const COperation *po ) -> const char*
		{
			return po->GetName().c_str();
		}
	);
	mOperationsCombo->blockSignals( false );
}


CAbstractDisplayEditor::CAbstractDisplayEditor( CModel *model, const COperation *op, const std::string &display_name, QWidget *parent )		//display_name = "", parent = nullptr ) 
	: base_t( parent )
	, mModel( model )
	, mWDataset( model->Workspace< CWorkspaceDataset >() )
	, mWOperation( model->Workspace< CWorkspaceOperation >() )
	, mWDisplay( model->Workspace< CWorkspaceDisplay >() )
	, mWFormula( model->Workspace< CWorkspaceFormula >() )
	, mOperation( op )
	, mDisplayOnlyMode( false )
{
    assert__( mOperation && mWDataset && mWOperation && mWDisplay && mWFormula );       Q_UNUSED( display_name );

	FilterOperations( mOperation->IsMap() );
	if ( !mOperation )
	{
		throw CException( "The requested operation " + op->GetName() + " has no associated displays. Please run the operation again" );
	}

	//IMPORTANT CreateWidgets() must be called by derived classes
}


CAbstractDisplayEditor::CAbstractDisplayEditor( bool map_editor, const CDisplayFilesProcessor *proc, QWidget *parent )	//QWidget *parent = nullptr ) 
	: base_t( parent )
	, mCurrentDisplayFilesProcessor( proc )
	, mDisplayOnlyMode( true )
{
    assert__( !map_editor || proc->isZFLatLon() );                  Q_UNUSED(map_editor);   //for release builds
	assert__( map_editor || proc->isYFX() || proc->isZFXY() );

	mDisplaysCombo->addItem( t2q( mCurrentDisplayFilesProcessor->ParamFile() ) );
	mDisplaysCombo->setEnabled( false );
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


int CAbstractDisplayEditor::DisplayIndex( const CDisplay *display ) const
{
	if ( !display )
		return -1;

	return std::find( mFilteredDisplays.begin(), mFilteredDisplays.end(), display ) - mFilteredDisplays.begin();
}


int CAbstractDisplayEditor::OperationIndex( const COperation *op ) const
{
	if ( !op )
		return -1;

	return std::find( mFilteredOperations.begin(), mFilteredOperations.end(), op ) - mFilteredOperations.begin();
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


///////////////////////////////////////////////////////////////////////////////////////////////////////

void CAbstractDisplayEditor::UpdateDisplaysCombo( int index )
{
	mDisplaysCombo->clear();

	FillCombo( mDisplaysCombo, mFilteredDisplays,

		index, true,

		[]( const CDisplay *disp ) -> const char*
		{
			return disp->GetName().c_str();
		}
	);
}


void CAbstractDisplayEditor::HandleOperationsIndexChanged( int index )
{
	if ( index < 0 )
	{
		mDatasetName->clear();
	}
	else
	{
		assert__( mWOperation );
		const COperation *op = mWOperation->GetOperation( q2a( mOperationsCombo->currentText() ) );		
		if ( op == mOperation )
			LOG_TRACE( "Operation already assigned before handling index change." );

		mOperation = op;
		const CDataset *dataset = mOperation->GetDataset();
		mDatasetName->setText( t2q( dataset->GetName() ) );
	}
	
	FilterDisplays();
	UpdateDisplaysCombo( mRequestedDisplayIndex );
	mRequestedDisplayIndex = 0;
	OperationChanged( index );
}


bool CAbstractDisplayEditor::Start( const std::string &display_name )
{
	mRequestedDisplayIndex = 0;
	if ( !display_name.empty() )
	{
		CDisplay *display = mWDisplay->GetDisplay( display_name );		assert__( display );
		mRequestedDisplayIndex = DisplayIndex( display );		  		assert__( mRequestedDisplayIndex >= 0 );
	}
	int index = OperationIndex( mOperation );
    int save_mRequestedDisplayIndex = mRequestedDisplayIndex;           Q_UNUSED( save_mRequestedDisplayIndex );    //release builds
	mOperationsCombo->setCurrentIndex( index );							assert__( mDisplaysCombo->currentIndex() == mRequestedDisplayIndex );
	return index >= 0;
}


void CAbstractDisplayEditor::HandleViewChanged( int index )
{
	CDisplay *display = index < 0 ? nullptr : mFilteredDisplays[ index ];
	delete mCurrentDisplayFilesProcessor;
	mCurrentDisplayFilesProcessor = nullptr;
	
	if ( display == mDisplay )
		return;

	mDisplay = display;

	if ( !mDisplay )
		return;

	assert__( mDisplay );

	setWindowTitle( mDisplay->GetName().c_str() );

	std::string msg;
	if ( !ControlSolidColor() || !ControlVectorComponents( msg ) )
	{
		SimpleErrorBox( msg );

		//TODO de-select display in widget list 

		index = -1;		//TODO de-select display in widget list ???

		return;
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

			index = -1;		//TODO de-select display in widget list ???

			return;
		}
	}

	Refresh();
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

void CAbstractDisplayEditor::FilterDisplays()
{
	assert__( mOperation );

	mFilteredDisplays.clear();

	const bool with_maps = mOperation->IsMap();

	auto displays = mModel->OperationDisplays( mOperation->GetName() );
	std::string errors;
	for ( auto display : displays )
	{
		try
		{
			if ( with_maps ? display->IsZLatLonType() : !display->IsZLatLonType() )
			{
				auto v = display->GetOperations();
				if ( v.size() == 0 )
					LOG_WARN( "View " + display->GetName() + " does not reference any operation." );
				else
					mFilteredDisplays.push_back( mWDisplay->GetDisplay( display->GetName() ) );
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

//  - called by constructor
//	- can cancel mOperation assignment if it does not have associated views
//	- 
//
void CAbstractDisplayEditor::FilterOperations( bool with_maps )
{
	assert__( mFilteredOperations.empty() );	// to be called by constructor

	auto const &operations = *mWOperation->GetOperations();
	std::string errors;
	for ( auto const &operation_entry : operations )
	{
		const COperation *operation = dynamic_cast<COperation *>( operation_entry.second );		assert__( operation );
		try
		{
			if ( with_maps ? operation->IsMap() : !operation->IsMap() )
			{
				auto v = mModel->OperationDisplays( operation->GetName() );
				if ( v.size() == 0 )
				{
					LOG_WARN( "Operation " + operation->GetName() + " is not referenced by any view. Try to run the operation again." );
					if ( mOperation == operation )
						mOperation = nullptr;
				}
				else
					mFilteredOperations.push_back( operation );
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

	CMapDisplayData* selectedData =  mDisplay->GetData();

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

	CMapDisplayData* selectedData =  mDisplay->GetData();

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
