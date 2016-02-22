#include "new-gui/brat/stdafx.h"

#include "new-gui/brat/ApplicationLogger.h"

#include "new-gui/brat/DataModels/Workspaces/Workspace.h"

#include "new-gui/brat/Views/TextWidget.h"
#include "new-gui/brat/Views/2DPlotWidget.h"
#include "new-gui/brat/Views/3DPlotWidget.h"

#include "ControlsPanel.h"

#include "libbrathl/TreeField.h"
#include "libbrathl/Field.h"


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


//	title = "", parent = nullptr, spacing = smSpacing, left = smLeft, top = smTop, right 00000= smRight, bottom = smBottom
//
QGroupBox* CControlsPanel::AddTopGroupBox( ELayoutType o, const std::vector< QObject* > &v, const QString &title, 
	int spacing, int left, int top, int right, int bottom )		 
{
	QGroupBox *group = CreateGroupBox( o, v, title, this, spacing, left, top, right, bottom  );
	AddTopWidget( group );
	return group;
}


QSpacerItem* CControlsPanel::AddTopSpace( int w, int h, QSizePolicy::Policy hData, QSizePolicy::Policy vData )
{
    auto spacer = CreateSpace( w, h, hData, vData );
    mMainLayout->addItem( spacer );
    return spacer;
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


	/* III. Dataset & Filter & Operation Group

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
	  */
	// IV. Assemble all groups in quick operations page

	LayoutWidgets( Qt::Vertical, 
	{ 
		vars_group, 
		views_group, 
		//all_group 
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
	mShowAliases = new QPushButton( "Show Aliases", tab_parent );
	QGroupBox *data_compute_group = CreateGroupBox( ELayoutType::Horizontal, { mDataComputation, mCheckSyntax, mShowInfo, mShowAliases }, 
		"Data Computation", tab_parent, 2, 2, 2, 2, 2 );


	// VI. Sampling Group

	auto adv_filter_label = new QLabel( "Filter" );
	auto mAdvFilter = new QComboBox;
	QBoxLayout *adv_filter_vl = LayoutWidgets( Qt::Vertical, { adv_filter_label, mAdvFilter }, nullptr, 0, 2, 2, 2, 2 );
	QFrame *line = WidgetLine( nullptr, Qt::Vertical );
	line->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
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


	/* IX. Dataset & Filter Group

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
	  */
    LayoutWidgets( Qt::Vertical,
	{ 
		expression_group,

		data_compute_group,
		//nullptr,
		sampling_group,
		adv_other_hl,
		views_group,
		//all_group,
		nullptr
	}, 
	tab_parent, 0, 2, 2, 2, 2 );

	return mAdvancedOperationsPage;
}

QWidget* COperationsControls::CreateCommonWidgets()
{
	const int m = 6;
	const int s = 4;

	auto mOperationsCombo = new QComboBox;		
	mOperationsCombo->setToolTip( "List of current workspace operations" );
	auto mNewOperation = new QPushButton( "New..." );
	auto mDeleteOperation = new QPushButton( "Delete..." );
    auto mRenameOperation = new QPushButton( "Rename..." );

	auto mSaveOperation = new QPushButton( "Save..." );	// TODO confirm that save will only be done in the scope of whole workspace
	
	QFrame *frame = new QFrame;
	frame->setWindowTitle( "QFrame::Box" );
	frame->setFrameStyle( QFrame::Panel );
	frame->setFrameShadow( QFrame::Sunken );
	frame->setObjectName("OperationsFrame");
	frame->setStyleSheet("#OperationsFrame { border: 2px solid black; }");
	LayoutWidgets( { 
		mOperationsCombo, mNewOperation, mDeleteOperation, mRenameOperation, nullptr,
		mSaveOperation
	}, frame, s, m, m, m, m );

	return frame;
}



//explicit 
COperationsControls::COperationsControls( CDesktopManagerBase *manager, QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0 
	: base_t( manager, parent, f )
{
	mCommonGroup = CreateCommonWidgets();


	QWidget *mQuickOperationsPage = CreateQuickOperationsPage();
	QWidget *mAdvancedOperationsPage = CreateAdancedOperationsPage();

    mStackWidget = new CStackedWidget( this, { { mQuickOperationsPage, "Quick" }, { mAdvancedOperationsPage, "Advanced" } } );

	// Layout Stack Widget Buttons & add Stack Widget itself
	
    QPushButton *m_QuickButton = qobject_cast<QPushButton*>( mStackWidget->Button( 0 ) );
    QPushButton *m_AdvancedButton = qobject_cast<QPushButton*>( mStackWidget->Button( 1 ) );

    AddTopWidget( mCommonGroup );

	AddTopLayout( ELayoutType::Horizontal, { m_QuickButton, m_AdvancedButton }, 0, 2, 2, 2, 2 );

    AddTopWidget( mStackWidget );

    WireOperations();
}

void COperationsControls::WireOperations()
{
	connect( mQuickMapButton, SIGNAL( clicked() ), this, SLOT( QuickMap() ) );
	connect( mQuickPlotButton, SIGNAL( clicked() ), this, SLOT( QuickPlot() ) );

	connect( mShowAliases, SIGNAL( clicked() ), this, SLOT( ShowAliases() ) );

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
	WaitCursor wait;

	auto ed = new CMapEditor( mWksd, mManager->parentWidget() );
	auto subWindow = mManager->AddSubWindow( ed );
	subWindow->show();

	//openTestFile( t2q( mManager->mPaths.mWorkspacesDir + R"(/newWP/Displays/DisplayDisplays_New.par)" ) );
}

void COperationsControls::QuickPlot()
{
	WaitCursor wait;

    auto ed = new CPlotEditor( mWksd, nullptr, (CPlot*)nullptr, mManager->parentWidget() );
    auto subWindow = mManager->AddSubWindow( ed );
    subWindow->show();

    //openTestFile( t2q( mManager->mPaths.mWorkspacesDir + R"(/newWP/Displays/DisplayDisplays_2.par)" ) );
}


void COperationsControls::ShowAliases()
{
	//if ( m_operation == NULL || m_product == NULL )
	//	return;

	//std::string title = "Show '" + m_product->GetProductClassType() + "' aliases...";

	//CFormula* formula = GetOperationtreectrl()->GetCurrentFormula();
	//bool hasOpFieldSelected = formula != NULL;

	//CAliasInfoDlg dlg( this, -1, title, m_operation, formula );

	//dlg.ShowModal();

	//if ( hasOpFieldSelected )
	//{
	//	wxGrid* aliasesGrid = dlg.GetAliasinfoGrid();
	//	std::string text;
	//	int32_t numRows = aliasesGrid->GetNumberRows();

	//	bool hasSelectedItems = false;

	//	for ( int32_t i = 0; i < numRows; i++ )
	//	{
	//		std::string isSeleted = aliasesGrid->GetCellValue( i, CAliasInfoDlg::USE_COL );
	//		if ( isSeleted.compare( "1" ) == 0 )
	//		{
	//			hasSelectedItems = true;
	//			text.Append( aliasesGrid->GetCellValue( i, CAliasInfoDlg::SYNTAX_COL ) );
	//			text.Append( " " );
	//		}

	//	}
	//	if ( hasSelectedItems )
	//	{
	//		GetOptextform()->WriteText( text );
	//		SetTextFormula();
	//	}
	//}
}





//HAMMER SECTION
#include "new-gui/brat/DataModels/DisplayFilesProcessor.h"
#include "new-gui/brat/DataModels/PlotData/ZFXYPlot.h"
#include "new-gui/brat/DataModels/PlotData/XYPlot.h"
#include "new-gui/brat/DataModels/PlotData/WorldPlot.h"


//L:\project\workspaces\newWP\Displays\DisplayDisplays_New.par

void COperationsControls::openTestFile( const QString &fileName )
{
	delete mCmdLineProcessor;
	mCmdLineProcessor = new DisplayFilesProcessor;

	const std::string s = q2a( fileName );
	const char *argv[] = { "", s.c_str() };
	try
	{
		if ( mCmdLineProcessor->Process( 2, argv ) )
		{
			auto &plots = mCmdLineProcessor->plots();
			if ( mCmdLineProcessor->isZFLatLon() )		// =================================== WorldPlot
			{
				for ( auto &plot : plots )
				{
					CWPlot* wplot = dynamic_cast<CWPlot*>( plot );
					if ( wplot == nullptr )
						continue;

					auto ed = new CMapEditor( mCmdLineProcessor, wplot, this );
					auto subWindow = mManager->AddSubWindow( ed );
					subWindow->show();
				}
			}
			else if ( mCmdLineProcessor->isYFX() )		// =================================== XYPlot();
			{
				for ( auto &plot : plots )
				{
					CPlot* yfxplot = dynamic_cast<CPlot*>( plot );
					if ( yfxplot == nullptr )
						continue;

					auto ed = new CPlotEditor( mWksd, mCmdLineProcessor, yfxplot, this );
					auto subWindow = mManager->AddSubWindow( ed );
					subWindow->show();
				}
			}
			else if ( mCmdLineProcessor->isZFXY() )		// =================================== ZFXYPlot();
			{
				for ( auto &plot : plots )
				{
					CZFXYPlot* zfxyplot = dynamic_cast<CZFXYPlot*>( plot );
					if ( zfxyplot == nullptr )
						continue;

					auto ed = new CPlotEditor( mWksd, mCmdLineProcessor, zfxyplot, this );
					auto subWindow = mManager->AddSubWindow( ed );
					subWindow->show();
				}
			}
			else
			{
				CException e( "CBratDisplayApp::OnInit - Only World Plot Data, XY Plot Data and ZFXY Plot Data are implemented", BRATHL_UNIMPLEMENT_ERROR );
				LOG_TRACE( e.what() );
				throw e;
			}
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
//HAMMER SECTION





///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_ControlsPanel.cpp"
