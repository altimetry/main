#include "new-gui/brat/stdafx.h"

#include "new-gui/brat/Workspaces/Workspace.h"

#include "new-gui/brat/Views/TextWidget.h"
#include "new-gui/brat/Views/2DPlotWidget.h"
#include "new-gui/brat/Views/3DPlotWidget.h"

#include "ControlsPanel.h"



/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//									Controls
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

CStackedWidget::CStackedWidget( QWidget *parent, const std::vector< PageInfo > &widgets, int iselected )		//iselected = 0 
	: base_t( parent )
{
    assert__( (size_t)iselected < widgets.size() );

	SetObjectName( this, "StackedWidget" );

	mGroup = new QButtonGroup( parent );

	for ( auto &w : widgets )
	{
		addWidget( w.mWidget );
		QAbstractButton *b = new QPushButton( w.mName.c_str(), parent );
		b->setCheckable( true );
        connect( b, SIGNAL( toggled( bool ) ), this, SLOT( buttonToggled( bool ) ) );
		mGroup->addButton( b );
	}

    SetCurrentIndex( iselected );
}



void CStackedWidget::SetCurrentIndex( int index )
{
    Button( index )->setChecked( true );
}


// slot
void CStackedWidget::buttonToggled( bool checked )
{
    QAbstractButton *b = qobject_cast< QAbstractButton* >( sender() );      assert__( b );
    if ( checked )
    {
        auto index = mGroup->buttons().indexOf( b );
        setCurrentIndex( index );
        emit PageChanged( index );
    }
}







/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//									Control Panels
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

//explicit 
CControlsPanel::CControlsPanel( QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0 
	: base_t( parent, f )
{
	mMainLayout = static_cast< decltype( mMainLayout ) >( CreateLayout( this, Qt::Vertical, smSpacing, smLeft, smTop, smRight, smBottom ) );
}


QListWidget* CControlsPanel::CreateBooleanList( QWidget *parent, std::initializer_list< BooleanListItemInfo > il )
{
	QListWidget *l = new QListWidget( parent );
	for ( auto &i : il )
	{
		QListWidgetItem* item = new QListWidgetItem( t2q( i.mName ), l );
		item->setFlags( item->flags() | Qt::ItemIsUserCheckable );		
		item->setCheckState( i.mChecked ? Qt::Checked : Qt::Unchecked );
	}
	return l;
}


QObject* CControlsPanel::AddTopWidget( QObject *ob )
{
	auto w = qobject_cast<QWidget*>( ob );
	auto l = qobject_cast<QLayout*>( ob );		assert__( w || l );
	if ( w )
		mMainLayout->addWidget( w );
	else
		mMainLayout->addLayout( l );

	return ob;
}


//	spacing = smSpacing, left = smLeft, top = smTop, right = smRight, bottom = smBottom
//
QLayout* CControlsPanel::AddTopLayout( ELayoutType o, const std::vector< QObject* > &v, int spacing, int left, int top, int right, int bottom )		 
{
	QLayout *l = nullptr;
	switch ( o )
	{
		case ELayoutType::Horizontal:
		case ELayoutType::Vertical:
			l = LayoutWidgets( static_cast< Qt::Orientation >( o ), v, nullptr, spacing, left, top, right, bottom  );
			break;
		case ELayoutType::Grid:
			l = LayoutWidgets( v, nullptr, spacing, left, top, right, bottom  );
			break;
		default:
			assert__( false );
	}	
	AddTopWidget( l );

	return l;
}


//	title = "", parent = nullptr, spacing = smSpacing, left = smLeft, top = smTop, right = smRight, bottom = smBottom
//
QGroupBox* CControlsPanel::AddTopGroupBox( ELayoutType o, const std::vector< QObject* > &v, const QString &title, 
	int spacing, int left, int top, int right, int bottom )		 
{
	QGroupBox *group = CreateGroupBox( o, v, title, this, spacing, left, top, right, bottom  );
	AddTopWidget( group );
	return group;
}



/////////////////////////////////////////////////////////////////////////////////////
//								Dataset Browser 
/////////////////////////////////////////////////////////////////////////////////////

void CDatasetBrowserControls::PageChanged( int index )
{
	qDebug() << index;
}


//explicit 
CDatasetBrowserControls::CDatasetBrowserControls( desktop_manager_t *manager, QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0 
	: base_t( manager, parent, f )
{
	// I. Browse Stack Widget; 2 pages: files and rads

	// - Page Files browser
	//
	auto mFilesList = new QListWidget;

	auto mAddFiles = new QPushButton( "Add Files" );
	auto mAddDir = new QPushButton( "Add Dir" );
	auto mUp = new QPushButton( "Up" );
	auto mDown = new QPushButton( "Down" );			//QPushButton *mSort = new QPushButton( "Sort", page_1 );	by QListWidget ?
	auto mRemove = new QPushButton( "Remove" );
	auto mClear = new QPushButton( "Clear" );
	QBoxLayout *buttons_vl = LayoutWidgets( Qt::Vertical, { mAddFiles, mAddDir, mUp, mDown, /*mSort, */mRemove, mClear } );

	QGroupBox *page_1 = CreateGroupBox( ELayoutType::Horizontal, { mFilesList, buttons_vl }, "", nullptr, 0, 2, 2, 2, 2 );

	// - Page RADS browser
	//
	auto rads_label = new QLabel( "Missions" );
	QListWidget *mRadsList = CreateBooleanList( nullptr, { { "ERS1", true }, { "Jason1" }, { "CryoSat" } } );	//TODO, obviously
    //QBoxLayout *rads_vl =	
    LayoutWidgets( Qt::Vertical, { rads_label, mRadsList }, nullptr, 0, 2, 2, 2, 2 );

	QGroupBox *page_2 = CreateGroupBox( ELayoutType::Vertical, { rads_label, mRadsList }, "", nullptr, 0, 2, 2, 2, 2 );

	// Group Stack Widget Buttons & add Stack Widget itself
	//
	mBrowserStakWidget = new CStackedWidget( nullptr, { { page_1, "Files" }, { page_2, "RADS" } } );

	m_BrowseFilesButton = qobject_cast<QPushButton*>( mBrowserStakWidget->Button( 0 ) );
	m_BrowseRadsButton = qobject_cast<QPushButton*>( mBrowserStakWidget->Button( 1 ) );
	QBoxLayout *brwose_buttons_hl = LayoutWidgets( Qt::Horizontal, { m_BrowseFilesButton, m_BrowseRadsButton }, nullptr, 0, 2, 2, 2, 2 );
	
	AddTopGroupBox( ELayoutType::Vertical, { brwose_buttons_hl, mBrowserStakWidget }, "", 4, 4, 4, 4, 4 );


	// II. Variable Description group
	//
	auto mVarList = new QListWidget;		//mVarList->setMaximumWidth( 120 );
	auto mVarTable = new QTableWidget;		//mVarTable->setMaximumWidth( 120 );
	mVarTable->setColumnCount( 4 );
	QBoxLayout *vars_hl = LayoutWidgets( Qt::Horizontal, { mVarList, mVarTable }, nullptr );

	auto mVarText = new CTextWidget;
	mVarText->setPlainText("bla bla bla with the full....");

	AddTopGroupBox( ELayoutType::Vertical, { vars_hl, mVarText }, "Variable Description", 4, 4, 4, 4, 4 );


	// III. Dataset name and buttons group
	//
	mDatasetsCombo = new QComboBox;
	mDatasetsCombo->setToolTip( "List of current workspace datasets" );
	auto mNewDataset = new QPushButton( "New" );
	auto mDeleteDataset = new QPushButton( "Delete" );
	auto mSaveDataset = new QPushButton( "Save" );

	auto mOpenDataset = new QPushButton( "Open..." );
	auto mDefineSelectionCriteria = new QPushButton( "Sel. Criteria..." );
	auto mCheckFiles = new QPushButton( "Check Files" );
	auto mShowSelectionReport = new QPushButton( "Sel. Report..." );

	AddTopGroupBox(  
		ELayoutType::Grid, {
			mDatasetsCombo, mNewDataset, mDeleteDataset, mSaveDataset, nullptr,
			mOpenDataset, mDefineSelectionCriteria, mCheckFiles, mShowSelectionReport 
		}, 
		"", 0, 0, 0, 0, 0 
		);


	//connect( mBrowserStakWidget, SIGNAL( PageChanged( int ) ), this, SLOT( PageChanged( int ) ) );
	//
	// done with Browse Stack Widget
}



const QString& qidentity( const QString &s ){  return s; }

template < 
    typename COMBO, typename CONTAINER, typename FUNC = decltype( qidentity )
>
inline void FillCombo( COMBO *c, const CONTAINER &items, const FUNC &f, int selected = 0, bool enabled = true )
{
	for ( auto i : items ) 
	{
		c->addItem( f( i ) );
	}
	c->setCurrentIndex( selected );
	c->setEnabled( enabled );
}

void CDatasetBrowserControls::WorkspaceChanged( CWorkspaceDataset *wksd )
{
	mWks = wksd;

	mDatasetsCombo->clear();
    if (wksd)
        FillCombo( mDatasetsCombo, *mWks->GetDatasets(),

            []( const CObMap::value_type &i ) -> const char*
            {
                return i.first.c_str();
            },
            0, true
        );
}


/////////////////////////////////////////////////////////////////////////////////////
//								Dataset Filter
/////////////////////////////////////////////////////////////////////////////////////


//hData = QSizePolicy::Minimum, QSizePolicy::Policy vData = QSizePolicy::Minimum
//
QSpacerItem* CControlsPanel::AddTopSpace( int w, int h, QSizePolicy::Policy hData, QSizePolicy::Policy vData )
{
	auto spacer = CreateSpace( w, h, hData, vData );
	mMainLayout->addItem( spacer );
	return spacer;
}



//explicit 
CDatasetFilterControls::CDatasetFilterControls( desktop_manager_t *manager, QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0 
	: base_t( manager, parent, f )
{
	QListWidget *areas_list = CreateBooleanList( this, { { "Lake Baikal", true }, { "Black Sea" }, { "User Area 1", true }, { "User Area 2" } } );
	QGroupBox *where_box = CreateGroupBox( ELayoutType::Vertical, { areas_list }, "Where", this );

	AddTopWidget( where_box );
	//AddTopSpace( 20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding );
	AddTopSpace( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding );
}

//public slots:
void CDatasetFilterControls::WorkspaceChanged( CWorkspaceDataset *wksd )
{
	mWks = wksd;
}





/////////////////////////////////////////////////////////////////////////////////////
//									Operations
/////////////////////////////////////////////////////////////////////////////////////

QWidget* COperationsControls::CreateQuickOperationsPage()
{
	auto mQuickOperationsPage = new QWidget( this );

	// I. Saved Variables Group

	QListWidget *mSavedVariablesList = CreateBooleanList( nullptr, { { "SSH", true }, { "SWH" }, { "Winds" }, { "Sigma0", true } } );	//TODO, obviously

	auto mAddVariable = new QPushButton( "Add Variable" );
	auto mClear = new QPushButton( "Clear" );
	auto mExport = new QPushButton( "Export" );
	auto mEditAsciiExport = new QPushButton( "Ascii Export..." );	//QPushButton *mSort = new QPushButton( "Sort", page_1 );	by qlistwidget ?
	auto mStatistics = new QPushButton( "Statistics..." );
	QBoxLayout *buttons_vl = LayoutWidgets( Qt::Vertical, { mAddVariable, mClear, mExport, mEditAsciiExport, mStatistics } );

	QBoxLayout *variables_hl = LayoutWidgets( Qt::Horizontal, { mSavedVariablesList, buttons_vl }, nullptr, 0, 2, 2, 2, 2 );

	auto desc_label = new QLabel( "Variable Description" );
	auto mVarTable = new QTableWidget;
	mVarTable->setColumnCount( 4 );
	QBoxLayout *desc_vl = LayoutWidgets( Qt::Vertical, { desc_label, mVarTable }, nullptr, 0, 2, 2, 2, 2 );

	QGroupBox *vars_group = CreateGroupBox( ELayoutType::Vertical, { variables_hl, desc_vl }, "Saved Variables" , mQuickOperationsPage, 0, 2, 2, 2, 2 );


	// II. Views Group

	mQuickMapButton = new QPushButton( "Quick Map" );
	mQuickPlotButton = new QPushButton( "Quick Plot" );
	auto mSplitPlots = new QCheckBox( "Split Plots" );

	QGroupBox *views_group = CreateGroupBox( ELayoutType::Horizontal, { mQuickMapButton, mQuickPlotButton, mSplitPlots }, "Views" , mQuickOperationsPage, 6, 2, 2, 2, 2 );


	// III. Dataset & Filter & Operation Group

	auto mQuickDatasets = new QComboBox;
	auto mQuickFilters = new QComboBox;
	auto mSelectedDatasetName = new QLineEdit;
	auto mSelectedFilterName = new QLineEdit;
	QGridLayout *all_grid_l = LayoutWidgets( { mQuickDatasets, mQuickFilters, nullptr, mSelectedDatasetName, mSelectedFilterName }, nullptr, 6, 2, 2, 2, 2 );

	auto mQuickOperationNname = new QLineEdit;
	auto mNewQuickOperation = new QPushButton( "New" );
	auto mDeleteQuickOperation = new QPushButton( "Delete" );
	auto mSaveQuickOperation = new QPushButton( "Save" );
	auto mOpenQuickOperation = new QComboBox;
	QBoxLayout *quick_op_hl = LayoutWidgets( Qt::Horizontal, { mNewQuickOperation,
		mDeleteQuickOperation, mSaveQuickOperation, mOpenQuickOperation }, nullptr, 2, 2, 2, 2, 2 );

	QGroupBox *all_group = CreateGroupBox( ELayoutType::Vertical, { all_grid_l, mQuickOperationNname, quick_op_hl }, "", mQuickOperationsPage, 2, 2, 2, 2, 2 );

	// IV. Assemble all groups in quick operations page

	LayoutWidgets( Qt::Vertical, 
	{ 
		vars_group, 
		views_group, 
		all_group 
	}, 
	mQuickOperationsPage, 0, 2, 2, 2, 2 );

	return mQuickOperationsPage;
}


QWidget* COperationsControls::CreateAdancedOperationsPage()
{
	QWidget *mAdvancedOperationsPage = new QWidget( this );
	QWidget *tab_parent = mAdvancedOperationsPage;	// new QWidget( mAdvancedOperationsPage );	// mAdvancedOperationsPage;

	// I. Formulas Group

	auto mInsertFunction = new QPushButton( "Function" );
	auto mInsertAlgorithm = new QPushButton( "Algorithm" );
	auto mInsertFormula = new QPushButton( "Formula" );
	auto mSaveAsFormula = new QPushButton( "Save As Formula" );

	QGroupBox *formulas_group = CreateGroupBox( ELayoutType::Horizontal, 
	{ 
		mInsertFunction, mInsertAlgorithm, mInsertFormula, mSaveAsFormula 
	}, 
	"", tab_parent, 2, 2, 2, 2, 2 );


	// II. Fields Layout

	// - fields
	auto adv_fields_label = new QLabel( "Fields", tab_parent );
	auto mAdvancedFields = new QListWidget( tab_parent );
	QBoxLayout *adv_fields_vl = LayoutWidgets( Qt::Vertical, { adv_fields_label, mAdvancedFields }, nullptr, 2, 2, 2, 2, 2 );

	// - data expressions
	auto data_expressions_label = new QLabel( "Data Expressions", tab_parent );
	auto mAdvancedDataExpressions = new QTreeWidget( tab_parent );
	auto qtreewidgetitem = new QTreeWidgetItem();
	qtreewidgetitem->setText( 0, QString::fromUtf8( "1" ) );
	mAdvancedDataExpressions->setHeaderItem( qtreewidgetitem );
	SetObjectName( mAdvancedDataExpressions, "treeWidget" );
	QBoxLayout *adv_expressions_vl = LayoutWidgets( Qt::Vertical, { data_expressions_label, mAdvancedDataExpressions }, nullptr, 2, 2, 2, 2, 2 );

	QBoxLayout *fields_hl = LayoutWidgets( Qt::Horizontal, { adv_fields_vl, adv_expressions_vl }, nullptr, 2, 2, 2, 2, 2 );
	

	// III. Variable Description Layout

	auto adv_desc_label = new QLabel( "Variable Description", tab_parent );
	auto mAdvVarTable = new QTableWidget( tab_parent );
	mAdvVarTable->setColumnCount( 4 );
	QBoxLayout *adv_desc_vl = LayoutWidgets( Qt::Vertical, { adv_desc_label, mAdvVarTable }, nullptr, 0, 2, 2, 2, 2 );


	// IV. Expression Group

	// expressions
	auto mExpressionTextWidget = new CTextWidget;
	auto exp_group = CreateGroupBox( ELayoutType::Horizontal, { mExpressionTextWidget }, "Expression", tab_parent, 0, 2, 2, 2, 2 );

	static const QString SyncGroup("SyncGroup");
	QgsCollapsibleGroupBox *expression_group = CreateCollapsibleGroupBox( ELayoutType::Vertical, { formulas_group, fields_hl, adv_desc_vl, exp_group }, 
		"Expression", tab_parent, 0, 2, 2, 2, 2 );
	//expression_group->setCollapsed( true );
	expression_group->setSyncGroup( SyncGroup );
	//expression_group->setCheckable( true );


	// V. Data Computation Layout

	auto mDataComputation = new QComboBox( tab_parent );
	auto mCheckSyntax = new QPushButton( "Check Syntax", tab_parent );
	auto mShowInfo = new QPushButton( "Show Info", tab_parent );
	auto mShowAliases = new QPushButton( "Show Aliases", tab_parent );
	QGroupBox *data_compute_group = CreateGroupBox( ELayoutType::Horizontal, { mDataComputation, mCheckSyntax, mShowInfo, mShowAliases }, 
		"Data Computation", tab_parent, 2, 2, 2, 2, 2 );


	// VI. Sampling Group

	auto adv_filter_label = new QLabel( "Filter" );
	auto mAdvFilter = new QComboBox;
	QBoxLayout *adv_filter_vl = LayoutWidgets( Qt::Vertical, { adv_filter_label, mAdvFilter }, nullptr, 0, 2, 2, 2, 2 );
	QFrame *line = WidgetLine( nullptr, Qt::Vertical );
    auto step_label = new QLabel( "Step" );                         step_label->setAlignment(Qt::AlignCenter);
    auto cut_label = new QLabel( "Cut-Off" );                       cut_label->setAlignment(Qt::AlignCenter);
    auto intervals_label = new QLabel( "Number of Intervals" );     intervals_label->setAlignment(Qt::AlignCenter);
    auto mLatIntervalsLabel = new QLabel( "3600"  );                mLatIntervalsLabel->setAlignment(Qt::AlignCenter);
    auto mLonIntervalsLabel = new QLabel( "540" );                  mLonIntervalsLabel->setAlignment(Qt::AlignCenter);

	QGridLayout *sampling_gridl = LayoutWidgets(
	{ 
        new QLabel( "" ),			step_label,     cut_label,      intervals_label,        nullptr,
        new QLabel( "Latitude" ),	new QLineEdit,	new QLineEdit,	mLatIntervalsLabel,		nullptr,
        new QLabel( "Longitude" ),	new QLineEdit,	new QLineEdit,	mLonIntervalsLabel,     nullptr,
	},
    nullptr, 2, 4, 4, 4, 4 );
	QgsCollapsibleGroupBox *sampling_group = CreateCollapsibleGroupBox( ELayoutType::Horizontal, { adv_filter_vl, line, sampling_gridl }, 
        "Sampling", tab_parent, 2, 4, 4, 4, 4 );
	sampling_group->setCollapsed( true );
	sampling_group->setCheckable( true );
	sampling_group->setSyncGroup( SyncGroup );


	// VII. Other Advanced Layout

	auto mAdvComputeStatistics = new QPushButton( "Statistics", tab_parent );
	auto mDelayExecution = new QPushButton( "Delay Execution", tab_parent );
	auto mScheduler = new QPushButton( "Scheduler", tab_parent );
	auto mAdvExport = new QPushButton( "Export", tab_parent );
	auto mAdvEditAsciiExport = new QPushButton( "Edit ASCII Export", tab_parent );
	auto adv_other_hl = LayoutWidgets( { mAdvComputeStatistics, mDelayExecution, mScheduler, nullptr, mAdvExport, mAdvEditAsciiExport }, 
		nullptr, 0, 2, 2, 2, 2 );


	// VIII. Views Group

	auto mQuickMap = new QPushButton( "Quick View" );
	auto mSplitPlots = new QCheckBox ( "Split Plots" );
	QGroupBox *views_group = CreateGroupBox( ELayoutType::Horizontal, { mQuickMap, mSplitPlots }, "Views" , tab_parent, 6, 2, 2, 2, 2 );


	// IX. Dataset & Filter Group

	auto mAdvDatasets = new QComboBox;
	auto mAdvFilters = new QComboBox;
	auto mSelectedDatasetName = new QLineEdit;
	auto mSelectedFilterName = new QLineEdit;
	QGridLayout *all_grid_l = LayoutWidgets( { mAdvDatasets, mAdvFilters, nullptr, mSelectedDatasetName, mSelectedFilterName }, nullptr, 6, 2, 2, 2, 2 );

	auto mQuickOperationNname = new QLineEdit( tab_parent );
	auto mNewQuickOperation = new QPushButton( "New", tab_parent );
	auto mDeleteQuickOperation = new QPushButton( "Delete", tab_parent );
	auto mSaveQuickOperation = new QPushButton( "Save", tab_parent );
	auto mOpenQuickOperation = new QComboBox( tab_parent );
	QBoxLayout *adv_op_hl = LayoutWidgets( Qt::Horizontal, { mNewQuickOperation, 
		mDeleteQuickOperation, mSaveQuickOperation, mOpenQuickOperation }, nullptr, 2, 2, 2, 2, 2 );

	QGroupBox *all_group = CreateGroupBox( ELayoutType::Vertical, { all_grid_l, mQuickOperationNname, adv_op_hl }, "", tab_parent, 2, 2, 2, 2, 2 );

    LayoutWidgets( Qt::Vertical,
	{ 
		expression_group, //= CreateCollapsibleGroupBox( ELayoutType::Vertical, { formulas_group, fields_hl, adv_desc_vl, exp_group }, 

		//formulas_group, 
		//fields_hl, 
		//adv_desc_vl, 
		//exp_group, 
		data_compute_group,
		//nullptr,
		sampling_group,
		adv_other_hl,
		views_group,
		all_group,
		nullptr
	}, 
	tab_parent, 0, 2, 2, 2, 2 );
	/*
	QScrollArea *scrollA = new QScrollArea( mAdvancedOperationsPage );
	//QVBoxLayout *toplay = new QVBoxLayout( mAdvancedOperationsPage );
    //toplay->addWidget( scrollA );

	AddWidget( mAdvancedOperationsPage, scrollA );
	mAdvancedOperationsPage->layout()->setSpacing( 0 );
	mAdvancedOperationsPage->layout()->setContentsMargins( 0,0,0,0 );

    scrollA->setWidgetResizable(true);
    scrollA->setWidget( tab_parent );
	scrollA->setFrameShape(QFrame::NoFrame);

	//scrollA->resize( scrollA->sizeHint() );
	  */
	//AddWidget( mAdvancedOperationsPage, tab_parent );
	return mAdvancedOperationsPage;
}




//explicit 
COperationsControls::COperationsControls( desktop_manager_t *manager, QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0 
	: base_t( manager, parent, f )
{
	QWidget *mQuickOperationsPage = CreateQuickOperationsPage();
	QWidget *mAdvancedOperationsPage = CreateAdancedOperationsPage();

    mStackWidget = new CStackedWidget( this, { { mQuickOperationsPage, "Quick" }, { mAdvancedOperationsPage, "Advanced" } } );

	// Layout Stack Widget Buttons & add Stack Widget itself
	
    QPushButton *m_QuickButton = qobject_cast<QPushButton*>( mStackWidget->Button( 0 ) );
    QPushButton *m_AdvancedButton = qobject_cast<QPushButton*>( mStackWidget->Button( 1 ) );

	AddTopLayout( ELayoutType::Horizontal, { m_QuickButton, m_AdvancedButton }, 0, 2, 2, 2, 2 );

    AddTopWidget( mStackWidget );

    WireOperations();
}

void COperationsControls::WireOperations()
{
	connect( mQuickMapButton, SIGNAL( clicked() ), this, SLOT( QuickMap() ) );
	connect( mQuickPlotButton, SIGNAL( clicked() ), this, SLOT( QuickPlot() ) );

	//connect( mOperationsStakWidget, SIGNAL( PageChanged( int ) ), this, SLOT( PageChanged( int ) ) );
	//
	// done with mOperationsStakWidget
}


bool COperationsControls::AdvancedMode() const
{
    return mStackWidget->CurrentIndex() == eAdvanced;
}

void COperationsControls::SetAdvancedMode( bool advanced ) const
{
    return mStackWidget->SetCurrentIndex( advanced ? eAdvanced : eQuick );
}


void COperationsControls::WorkspaceChanged( CWorkspaceOperation *wksdo )
{
	mWkso = wksdo;
}
void COperationsControls::WorkspaceChanged( CWorkspaceDisplay *wksd )
{
	mWksd = wksd;
}




void COperationsControls::QuickMap()
{
	auto ed = new CMapEditor( this );
	auto subWindow = mManager->AddSubWindow( ed );
	subWindow->show();
}

void COperationsControls::QuickPlot()
{
	auto ed = new CPlotEditor( this );
	auto subWindow = mManager->AddSubWindow( ed );
	subWindow->show();
}







/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//								Graphics Panels
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////
//								Plot View
/////////////////////////////////////////////////////////////////////////////////////

//explicit 
CPlotViewControls::CPlotViewControls( QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0 
	: base_t( parent, f )
{

}




/////////////////////////////////////////////////////////////////////////////////////
//								Map View
/////////////////////////////////////////////////////////////////////////////////////

//explicit 
CMapViewControls::CMapViewControls( QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0 
	: base_t( parent, f )
{

}






///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_ControlsPanel.cpp"
