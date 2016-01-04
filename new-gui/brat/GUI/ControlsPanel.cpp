#include "new-gui/brat/stdafx.h"

#include "ControlsPanel.h"
#include "new-gui/brat/Views/TextWidget.h"
#include "new-gui/brat/Views/2DPlotWidget.h"
#include "new-gui/brat/Views/3DPlotWidget.h"


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
		mGroup->addButton( b );
	}

	setCurrentIndex( iselected );
	Button( iselected )->setChecked( true );

	connect( mGroup, SIGNAL( buttonClicked( QAbstractButton* ) ), this, SLOT( changePage( QAbstractButton* ) ) );
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



// parent = nullptr, spacing = smSpacing, left = smLeft, top = smTop, right = smRight, bottom = smBottom
//
QBoxLayout* CControlsPanel::LayoutWidgets( Qt::Orientation o, const std::vector< QObject* > &v, 
	QWidget *parent, int spacing, int left, int top, int right, int bottom  )			
{
	assert__( parent != this );

	QBoxLayout *main_l = CreateLayout( parent, o, spacing, left, top, right, bottom );
	for ( auto ob : v )
	{
		if ( !ob )
		{
			main_l->addItem( CreateSpace() );
			continue;
		}

		auto w = qobject_cast<QWidget*>( ob );
		auto l = qobject_cast<QLayout*>( ob );		assert__( w || l );
		if ( w )
			main_l->addWidget( w );
		else
			main_l->addLayout( l );
	}
	return main_l;
}



// parent = nullptr, spacing = smSpacing, left = smLeft, top = smTop, right = smRight, bottom = smBottom
//
QGridLayout* CControlsPanel::LayoutWidgets( const std::vector< QObject* > &v, 
	QWidget *parent, int spacing, int left, int top, int right, int bottom )
{
	assert__( parent != this );

	QGridLayout *main_l = CreateGridLayout( parent, spacing, left, top, right, bottom );
	int line = 0, col = 0;
	for ( auto ob : v )
	{
		if ( !ob )
		{
			++line;
			col = 0;
		}
		else
		{
			auto w = qobject_cast<QWidget*>( ob );
			auto l = qobject_cast<QLayout*>( ob );		assert__( w || l );
			if ( w )
				main_l->addWidget( w, line, col, 1, 1 );
			else
				main_l->addLayout( l, line, col, 1, 1 );

			++col;
		}
	}
	return main_l;
}


//	title = "", parent = nullptr, spacing = smSpacing, left = smLeft, top = smTop, right = smRight, bottom = smBottom
//
template< class GROUP_BOX >
GROUP_BOX* CControlsPanel::CreateGroupBox( ELayoutType o, const std::vector< QObject* > &v,
	 const QString &title, QWidget *parent, int spacing, int left, int top, int right, int bottom )		 
{

	auto group = new GROUP_BOX( title, parent );

	for ( auto ob : v )
	{
		if ( !ob )
			continue;

		// QObject and QWidget setParent are not polymorphic
		//
		auto w = qobject_cast<QWidget*>( ob );
		auto l = qobject_cast<QLayout*>( ob );		assert__( w || l );
		if ( w )
			w->setParent( group );
		else
			l->setParent( nullptr );
	}

	switch ( o )
	{
		case eHorizontal:
		case eVertical:
			LayoutWidgets( static_cast< Qt::Orientation >( o ), v, group, spacing, left, top, right, bottom  );
			break;
		case eGrid:
			LayoutWidgets( v, group, spacing, left, top, right, bottom  );
			break;
		default:
			assert__( false );
	}

	return group;
}

//	title = "", parent = nullptr, spacing = smSpacing, left = smLeft, top = smTop, right = smRight, bottom = smBottom
//
QGroupBox* CControlsPanel::CreateGroupBox( ELayoutType o, const std::vector< QObject* > &v,
	 const QString &title, QWidget *parent, int spacing, int left, int top, int right, int bottom )		 
{
	return CreateGroupBox< QGroupBox >( o, v, title, parent, spacing, left, top, right, bottom);
}


//	title = "", parent = nullptr, spacing = smSpacing, left = smLeft, top = smTop, right = smRight, bottom = smBottom
//
QgsCollapsibleGroupBox* CControlsPanel::CreateCollapsibleGroupBox( ELayoutType o, const std::vector< QObject* > &v,
	 const QString &title, QWidget *parent, int spacing, int left, int top, int right, int bottom )
{
	return CreateGroupBox< QgsCollapsibleGroupBox >( o, v, title, parent, spacing, left, top, right, bottom);
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
		case eHorizontal:
		case eVertical:
			l = LayoutWidgets( static_cast< Qt::Orientation >( o ), v, nullptr, spacing, left, top, right, bottom  );
			break;
		case eGrid:
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

	QGroupBox *page_1 = CreateGroupBox( eHorizontal, { mFilesList, buttons_vl }, "", nullptr, 0, 2, 2, 2, 2 );

	// - Page RADS browser
	//
	auto rads_label = new QLabel( "Missions" );
	QListWidget *mRadsList = CreateBooleanList( nullptr, { { "ERS1", true }, { "Jason1" }, { "CryoSat" } } );	//TODO, obviously
    //QBoxLayout *rads_vl =	
    LayoutWidgets( Qt::Vertical, { rads_label, mRadsList }, nullptr, 0, 2, 2, 2, 2 );

	QGroupBox *page_2 = CreateGroupBox( eVertical, { rads_label, mRadsList }, "", nullptr, 0, 2, 2, 2, 2 );

	// Group Stack Widget Buttons & add Stack Widget itself
	//
	mBrowserStakWidget = new CStackedWidget( nullptr, { { page_1, "Files" }, { page_2, "RADS" } } );

	m_BrowseFilesButton = qobject_cast<QPushButton*>( mBrowserStakWidget->Button( 0 ) );
	m_BrowseRadsButton = qobject_cast<QPushButton*>( mBrowserStakWidget->Button( 1 ) );
	QBoxLayout *brwose_buttons_hl = LayoutWidgets( Qt::Horizontal, { m_BrowseFilesButton, m_BrowseRadsButton }, nullptr, 0, 2, 2, 2, 2 );
	
	AddTopGroupBox( eVertical, { brwose_buttons_hl, mBrowserStakWidget }, "", 4, 4, 4, 4, 4 );


	// II. Variable Description group
	//
	auto mVarList = new QListWidget;		//mVarList->setMaximumWidth( 120 );
	auto mVarTable = new QTableWidget;		//mVarTable->setMaximumWidth( 120 );
	mVarTable->setColumnCount( 4 );
	QBoxLayout *vars_hl = LayoutWidgets( Qt::Horizontal, { mVarList, mVarTable }, nullptr );

	auto mVarText = new CTextWidget;
	mVarText->setPlainText("bla bla bla with the full....");

	AddTopGroupBox( eVertical, { vars_hl, mVarText }, "Variable Description", 4, 4, 4, 4, 4 );


	// III. Dataset name and buttons group
	//
	auto mDatasetName = new QLineEdit;	mDatasetName->setText( "Dataset Name" );
	auto mNewDataset = new QPushButton( "New" );
	auto mDeleteDataset = new QPushButton( "Delete" );
	auto mSaveDataset = new QPushButton( "Save" );

	auto mOpenDataset = new QPushButton( "Open..." );
	auto mDefineSelectionCriteria = new QPushButton( "Sel. Criteria..." );
	auto mCheckFiles = new QPushButton( "Check Files" );
	auto mShowSelectionReport = new QPushButton( "Sel. Report..." );

	AddTopGroupBox(  
		eGrid, {
			mDatasetName, mNewDataset, mDeleteDataset, mSaveDataset, nullptr,
			mOpenDataset, mDefineSelectionCriteria, mCheckFiles, mShowSelectionReport 
		}, 
		"", 0, 0, 0, 0, 0 
		);


	//connect( mBrowserStakWidget, SIGNAL( PageChanged( int ) ), this, SLOT( PageChanged( int ) ) );
	//
	// done with Browse Stack Widget
}


/////////////////////////////////////////////////////////////////////////////////////
//								Dataset Filter
/////////////////////////////////////////////////////////////////////////////////////

//w = 0, h = 0, hData = QSizePolicy::Expanding, QSizePolicy::Policy vData = QSizePolicy::Expanding 
//
QSpacerItem* CControlsPanel::CreateSpace( int w, int h, QSizePolicy::Policy hData, QSizePolicy::Policy vData )
{
	return new QSpacerItem( w, h, hData, vData );
}


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
	QListWidget *areas_list = CreateBooleanList( this, { { "Lake Baikal", true }, { "Black Seaa" }, { "User Area 1", true }, { "User Area 2" } } );
	QGroupBox *where_box = CreateGroupBox( eVertical, { areas_list }, "Where", this );

	AddTopWidget( where_box );
	//AddTopSpace( 20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding );
	AddTopSpace( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding );
}




/////////////////////////////////////////////////////////////////////////////////////
//								Quick Operations
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

	QGroupBox *vars_group = CreateGroupBox( eVertical, { variables_hl, desc_vl }, "Saved Variables" , mQuickOperationsPage, 0, 2, 2, 2, 2 );


	// II. Views Group

	mQuickMapButton = new QPushButton( "Quick Map" );
	mQuickPlotButton = new QPushButton( "Quick Plot" );
	auto mSplitPlots = new QCheckBox( "Split Plots" );

	QGroupBox *views_group = CreateGroupBox( eHorizontal, { mQuickMapButton, mQuickPlotButton, mSplitPlots }, "Views" , mQuickOperationsPage, 6, 2, 2, 2, 2 );


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

	QGroupBox *all_group = CreateGroupBox( eVertical, { all_grid_l, mQuickOperationNname, quick_op_hl }, "", mQuickOperationsPage, 2, 2, 2, 2, 2 );

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

	QGroupBox *formulas_group = CreateGroupBox( eHorizontal, 
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
	auto exp_group = CreateGroupBox( eHorizontal, { mExpressionTextWidget }, "Expression", tab_parent, 0, 2, 2, 2, 2 );

	static const QString SyncGroup("SyncGroup");
	QgsCollapsibleGroupBox *expression_group = CreateCollapsibleGroupBox( eVertical, { formulas_group, fields_hl, adv_desc_vl, exp_group }, 
		"Expression", tab_parent, 0, 2, 2, 2, 2 );
	//expression_group->setCollapsed( true );
	expression_group->setSyncGroup( SyncGroup );
	//expression_group->setCheckable( true );



	// V. Data Computation Layout

	auto mDataComputation = new QComboBox( tab_parent );
	auto mCheckSyntax = new QPushButton( "Check Syntax", tab_parent );
	auto mShowInfo = new QPushButton( "Show Info", tab_parent );
	auto mShowAliases = new QPushButton( "Show Aliases", tab_parent );
	QGroupBox *data_compute_group = CreateGroupBox( eHorizontal, { mDataComputation, mCheckSyntax, mShowInfo, mShowAliases }, 
		"Data Computation", tab_parent, 2, 2, 2, 2, 2 );

	// VI. Sampling Group

	auto adv_filter_label = new QLabel( "Filter" );
	auto mAdvFilter = new QComboBox;
	QBoxLayout *adv_filter_vl = LayoutWidgets( Qt::Vertical, { adv_filter_label, mAdvFilter }, nullptr, 0, 2, 2, 2, 2 );
	QFrame *line = WidgetLine( nullptr, Qt::Vertical );

	QGridLayout *sampling_gridl = LayoutWidgets(
	{ 
		new QLabel( "" ),			new QLabel( "Step" ),	new QLabel( "Cut-Off" ),new QLabel( "Number of Intervals" ),	nullptr,
		new QLabel( "Latitude" ),	new QLineEdit,			new QLineEdit,			new QLabel( "3600"  ),					nullptr,
		new QLabel( "Longitude" ),	new QLineEdit,			new QLineEdit,			new QLabel( "540" ),					nullptr,
	},
	nullptr, 0, 2, 2, 2, 2 );
	QgsCollapsibleGroupBox *sampling_group = CreateCollapsibleGroupBox( eHorizontal, { adv_filter_vl, line, sampling_gridl }, 
		"Sampling", tab_parent, 0, 2, 2, 2, 2 );
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
	QGroupBox *views_group = CreateGroupBox( eHorizontal, { mQuickMap, mSplitPlots }, "Views" , tab_parent, 6, 2, 2, 2, 2 );

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

	QGroupBox *all_group = CreateGroupBox( eVertical, { all_grid_l, mQuickOperationNname, adv_op_hl }, "", tab_parent, 2, 2, 2, 2, 2 );

    LayoutWidgets( Qt::Vertical,
	{ 
		expression_group, //= CreateCollapsibleGroupBox( eVertical, { formulas_group, fields_hl, adv_desc_vl, exp_group }, 

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

	mOperationsStakWidget = new CStackedWidget( this, { { mQuickOperationsPage, "Quick" }, { mAdvancedOperationsPage, "Advanced" } } );

	// Layout Stack Widget Buttons & add Stack Widget itself
	
	QPushButton *m_QuickButton = qobject_cast<QPushButton*>( mOperationsStakWidget->Button( 0 ) );
	QPushButton *m_AdvancedButton = qobject_cast<QPushButton*>( mOperationsStakWidget->Button( 1 ) );

	AddTopLayout( eHorizontal, { m_QuickButton, m_AdvancedButton }, 0, 2, 2, 2, 2 );

	AddTopWidget( mOperationsStakWidget );

	WireQuickOperations();
}

void COperationsControls::WireQuickOperations()
{
	connect( mQuickMapButton, SIGNAL( clicked() ), this, SLOT( QuickMap() ) );
	connect( mQuickPlotButton, SIGNAL( clicked() ), this, SLOT( QuickPlot() ) );

	//connect( mOperationsStakWidget, SIGNAL( PageChanged( int ) ), this, SLOT( PageChanged( int ) ) );
	//
	// done with mOperationsStakWidget
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
//								Advanced Operations
/////////////////////////////////////////////////////////////////////////////////////

////explicit 
//CAdvancedOperationsControls::CAdvancedOperationsControls( QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0 
//	: base_t( parent, f )
//{
//
//}




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
