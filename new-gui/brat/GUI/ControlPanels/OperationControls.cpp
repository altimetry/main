#include "new-gui/brat/stdafx.h"

#include "libbrathl/TreeField.h"
#include "libbrathl/Field.h"

#include "new-gui/Common/QtUtils.h"

#include "ApplicationLogger.h"

#include "DataModels/Model.h"
#include "DataModels/Filters/BratFilters.h"
#include "DataModels/Workspaces/Workspace.h"
#include "DataModels/Workspaces/Operation.h"
#include "DataModels/Workspaces/Function.h"

#include "GUI/ActionsTable.h"
#include "GUI/DisplayWidgets/TextWidget.h"
#include "GUI/DisplayEditors/MapEditor.h"
#include "GUI/DisplayEditors/PlotEditor.h"

#include "Dialogs/DelayExecutionDialog.h"
#include "Dialogs/ExportDialog.h"
#include "Dialogs/ShowAliasesDialog.h"
#include "Dialogs/ShowInfoDialog.h"

#include "DataExpressionsTree.h"
#include "ProcessesTable.h"
#include "OperationControls.h"



//static 
const std::vector<std::string> COperationControls::smPredefinedVariables =
{
	"SSH",
	"SWH",
	"Winds",
	"Sigma0"
};



static const std::string ButtonDisplayTextNoOp = "View...";
static const std::string ButtonDisplayTextIsMap = "Map...";
static const std::string ButtonDisplayTextIsPlot = "Plot...";


static const std::string ButtonDisplayIconNoOpPath = ":/images/themes/default/propertyicons/display.svg";

QIcon ButtonDisplayIcon( const QString &path )
{
	QIcon icon;
	icon.addFile( path, QSize(), QIcon::Normal, QIcon::Off );
	return icon;
}
QIcon ButtonDisplayIconNoOp()
{
	return ButtonDisplayIcon( ButtonDisplayIconNoOpPath.c_str() );
}
QIcon ButtonDisplayIconIsMap()
{
	return ButtonDisplayIcon( ":/images/themes/default/propertyicons/map_tools.png" );
}
QIcon ButtonDisplayIconIsPlot()
{
	return ButtonDisplayIcon( ":/images/themes/default/histogram.png" );
}


//static 
QList<QAction*> COperationControls::CreateDataComputationActions()
{
	QList<QAction*> actions;
	auto map = CMapDataMode::GetInstance();
	for ( CMapDataMode::const_iterator it = map.begin(); it != map.end(); it++ )
	{
		if ( !isDefaultValue( it->second ) )
		{
			auto *action = new QAction( it->first.c_str(), nullptr );	//checkable, parent and name properties assigned by group creation function
			actions << action;
		}
	}
	return actions;
}



void COperationControls::CreateQuickOperationsPage()
{
	mQuickOperationsPage = new QWidget( this );

	// I. Buttons row

	mDisplayMapButton = CreateToolButton( ButtonDisplayTextIsMap, ":/images/themes/default/propertyicons/map_tools.png", "Edit operation map" );
	mDisplayPlotButton = CreateToolButton( ButtonDisplayTextIsPlot, ":/images/themes/default/histogram.png", "Edit operation plot" );
	mDisplayMapButton->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
	mDisplayPlotButton->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );

	QWidget *buttons_row = CreateButtonRow( false, Qt::Horizontal, { nullptr, mDisplayMapButton, mDisplayPlotButton, nullptr } );

	// II. Dataset &Filters

	mQuickDatasetsCombo = new QComboBox;
	mQuickDatasetsCombo->setToolTip( "Selected operation dataset" );

	mOperationFilterButton_Quick = CActionInfo::CreatePopupButton( eActionGroup_Filters_Quick, QList<QAction*>() );
	mOperationFilterButton_Quick->setCheckable( true );
	mOperationFilterButton_Quick->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );

	auto *data_group = CreateGroupBox( ELayoutType::Horizontal, 
	{
		nullptr,
		LayoutWidgets( Qt::Horizontal, { new QLabel( "Dataset" ), mQuickDatasetsCombo }, nullptr, s, m, m, m, m ),
		nullptr,
		LayoutWidgets( Qt::Horizontal, { new QLabel( "Filter" ), mOperationFilterButton_Quick }, nullptr, s, m, m, m, m ),
		nullptr,
	}, 
	"", nullptr, s, m, m, m, m );
	data_group->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Maximum );


	
	// III. Predefined Variables

	mQuickVariablesList = CreateBooleanList( nullptr, 
	{ 
		{ smPredefinedVariables[ eSSH ] }, 
		{ smPredefinedVariables[ eSWH ] }, 
		{ smPredefinedVariables[ eWinds ] }, 
		{ smPredefinedVariables[ eSigma0 ] }
	} );
	mQuickVariablesList->setSelectionMode( QAbstractItemView::NoSelection );

	mAddVariable = CreateToolButton( "Add", ":/images/OSGeo/new.png", "Add a new variable" );
	mClearVariables = CreateToolButton( "Clear", ":/images/OSGeo/workspace-delete.png", "Clear selection" );
	mAddVariable->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
	mClearVariables->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
	mAddVariable->setEnabled( false );
	mClearVariables->setEnabled( false );

	QWidget *variables_buttons = CreateButtonRow( true, Qt::Vertical, 
	{ 
		mAddVariable, mClearVariables
	} );

	QGroupBox *vars_group = CreateGroupBox( ELayoutType::Horizontal, { mQuickVariablesList, variables_buttons }, "Fields" , nullptr, 12, 10, 10, 10, 10 );

	mQuickFieldDesc = new CTextWidget;
	mQuickFieldDesc->SetReadOnlyEditor( true );
	QGroupBox *desc_group = CreateGroupBox( ELayoutType::Vertical, { mQuickFieldDesc }, "Field Description" , nullptr, 0, m, m, m, m );


	// Assemble in quick operations page

	LayoutWidgets( Qt::Vertical, 
	{ 
		buttons_row,
		data_group,
		CreateSplitter( nullptr, Qt::Vertical, { vars_group, desc_group } ),
		nullptr
	}, 
	mQuickOperationsPage, 0, m, m, m, m );
}




void COperationControls::CreateAdancedOperationsPage()
{
	mAdvancedOperationsPage = new QWidget( this );

	// I. Top Buttons Row

	mNewOperationButton = CreateToolButton( "", ":/images/OSGeo/new.png", "Create a new operation" );
    mRenameOperationButton = CreateToolButton( "", ":/images/OSGeo/edit.png", "Rename the selected operation" );
    mDuplicateOperationButton = CreateToolButton( "", ":/images/themes/default/mActionCopySelected.png", "Duplicate the selected operation" );
    mDeleteOperationButton = CreateToolButton( "", ":/images/OSGeo/workspace-delete.png", "Delete the selected operation" );

	mAdvancedExecuteButton = CreateToolButton( "Execute", ":/images/OSGeo/execute.png", "Execute the operation and edit resulting view" );
	mAdvancedExecuteButton->setPopupMode( QToolButton::MenuButtonPopup );
	mAdvancedExecuteButton->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
    mDelayExecutionAction = new QAction( "Delay Execution...", this );
	mLaunchSchedulerAction = CActionInfo::CreateAction( this, eAction_Launch_Scheduler );
	mAdvancedExecuteButton->addAction( mDelayExecutionAction );
	mAdvancedExecuteButton->addAction( mLaunchSchedulerAction );

	QWidget *top_buttons_row = CreateButtonRow( false, Qt::Horizontal, 
	{ 
        mNewOperationButton, mRenameOperationButton, mDeleteOperationButton, mDuplicateOperationButton, nullptr, mAdvancedExecuteButton
	} );


	// II. Operations & Datasets &Filters

	mOperationsCombo = new QComboBox;		
	mOperationsCombo->setToolTip( "Selected operation" );
	mOperationFilterButton_Advanced = CActionInfo::CreatePopupButton( eActionGroup_Filters_Advanced, QList<QAction*>() );
	mOperationFilterButton_Advanced->setCheckable( true );
	mOperationFilterButton_Advanced->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );


	//dataset list

	mAdvancedDatasetsCombo = new QComboBox;
	mAdvancedDatasetsCombo->setToolTip( "Selected operation dataset" );

	//auto *ops_group = CreateGroupBox( ELayoutType::Horizontal, 
	auto *ops_group = LayoutWidgets( Qt::Horizontal, 
	{ 
		LayoutWidgets( Qt::Horizontal, { new QLabel("Operation"), mOperationsCombo }, nullptr, s, m, m, m, m ), 
		nullptr,
		LayoutWidgets( Qt::Horizontal, { new QLabel("Dataset"), mAdvancedDatasetsCombo }, nullptr, s, m, m, m, m ),
		nullptr,
		LayoutWidgets( Qt::Horizontal, { new QLabel("Filter"), mOperationFilterButton_Advanced }, nullptr, s, m, m, m, m )
	}, 
	nullptr, s, m, m, m, m );


	// III. Expression Group

	// ...1. Fields & Field Description Group

	// - fields
	mAdvancedFieldsTree = new CFieldsTreeWidget( mAdvancedOperationsPage );
	mAdvancedFieldDesc = new CTextWidget;
	mAdvancedFieldDesc->SetReadOnlyEditor( true );
	mAdvancedFieldDesc->SetSizeHint( 40 * fontMetrics().width( 'x' ), mAdvancedFieldDesc->sizeHint().height() );

	QGroupBox *fields_group = CreateGroupBox( ELayoutType::Horizontal, 
	{ 
		CreateSplitter( nullptr, Qt::Horizontal, { mAdvancedFieldsTree, mAdvancedFieldDesc } )
	}
	, "", nullptr, s, m, m, m, m );


	// ...2. Expressions Tree(s)

	mDataExpressionsTree = new CDataExpressionsTreeWidget( mWFormula, mMapFormulaString, true, mAdvancedOperationsPage, mAdvancedFieldsTree );

	mSwitchToMapButton = CreateToolButton( "Map", ":/images/themes/default/propertyicons/map_tools.png", "Click to change the type of the expression output" );
	mSwitchToPlotButton = CreateToolButton( "Plot", ":/images/themes/default/histogram.png", "Click to change the type of the expression output" );

	QWidget *data_expressions_buttons = CreateButtonRow( true, Qt::Vertical, 
	{ 
		mSwitchToMapButton, mSwitchToPlotButton
	} );

    //auto *data_expressions_group = CreateGroupBox( ELayoutType::Horizontal, { data_expressions_buttons, mDataExpressionsTree }, "", nullptr, s, m, m, m, m );
	QWidget *data_expressions_group = new QWidget;
    LayoutWidgets( Qt::Horizontal, { mDataExpressionsTree, data_expressions_buttons }, data_expressions_group, 12, 10, 10, 10, 10 );


	//...3. Buttons Row & Expression

	mInsertFunction = CreateToolButton( "", ":/images/alpha-numeric/1.png", "Insert function" );
	mInsertAlgorithm = CreateToolButton( "", ":/images/alpha-numeric/2.png", "Insert algorithm" );
	mInsertFormula = CreateToolButton( "", ":/images/alpha-numeric/3.png", "Insert formula" );
	mSaveAsFormula = CreateToolButton( "", ":/images/alpha-numeric/4.png", "Save as formula" );
	mDataComputationGroup = CreateActionGroup( this, CreateDataComputationActions(), true );
	mDataComputation = CreateMenuButton(  "", ":/images/alpha-numeric/b.png", "Set how data are stored/computed", mDataComputationGroup->actions() );
	mShowInfoButton = CreateToolButton( "", ":/images/OSGeo/page-info.png", "Show information" );
    mShowAliasesButton = CreateToolButton( "", ":/images/alpha-numeric/__e.png", "Show aliases" );

	mCheckSyntaxButton = CreateToolButton( "", ":/images/OSGeo/check.png", "Check syntax" );

	auto *expression_buttons = CreateButtonRow( false, Qt::Horizontal, 
	{ 
		mInsertFunction, mInsertFormula, mSaveAsFormula, mInsertAlgorithm, mDataComputation, nullptr, mShowInfoButton, mShowAliasesButton, mCheckSyntaxButton
	} );

	mExpressionGroup = CreateGroupBox( ELayoutType::Vertical, 
	{ 
		expression_buttons, 
		LayoutWidgets( Qt::Horizontal, { mDataExpressionsTree->ExpressionTextWidget(), mDataExpressionsTree->AssignExpressionButton() }, nullptr, s + m, m, m, m, m ),
	}
	, "Expression", mAdvancedOperationsPage, 0, m, m, m, m );

	//....Split in Group

	mOperationExpressionsGroup = CreateGroupBox( 
		ELayoutType::Vertical, { 
			CreateSplitter( nullptr, Qt::Vertical, { fields_group, data_expressions_group, mExpressionGroup } )
		}, 
		"", mAdvancedOperationsPage, s, 4, 4, 4, 4 );


	// IV. Sampling Group

	auto adv_filter_label = new QLabel( "Filter" );
	auto mAdvFilter = new QComboBox;
	QBoxLayout *adv_filter_vl = LayoutWidgets( Qt::Vertical, { adv_filter_label, mAdvFilter }, nullptr, 0, m, m, m, m );
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
	mSamplingGroup = CreateCollapsibleGroupBox( ELayoutType::Horizontal, { adv_filter_vl, line, sampling_gridl }, 
        "Sampling", mAdvancedOperationsPage, s, 4, 4, 4, 4 );
	mSamplingGroup->setCollapsed( true );
    //static const QString SyncGroup("SyncGroup");
	//mSamplingGroup->setCheckable( true );
	//mSamplingGroup->setSyncGroup( SyncGroup );


    LayoutWidgets( Qt::Vertical,
	{ 
		top_buttons_row, 
		CreateGroupBox( ELayoutType::Horizontal, { ops_group }, "" ),
		mOperationExpressionsGroup,
		mSamplingGroup,
		nullptr
	}, 
	mAdvancedOperationsPage, 0, m, m, m, m );
}


//	QUESTIONS: 
//	view: determined by operation: distinction quick/advanced does not seem to stand
//
QWidget* COperationControls::CreateCommonWidgets( QAbstractButton *b1, QAbstractButton *b2 )
{
	//buttons row

	mExportOperationAction = new QAction( "Export...", this );
	mEditExportAsciiAction = new QAction( "Edit ASCII Export", this );
	mOperationExportButton = CreateMenuButton( "", ":/images/OSGeo/export.png", "Export operation", { mExportOperationAction, mEditExportAsciiAction } );

	mOperationStatisticsButton = CreateToolButton( "", ":/images/OSGeo/stats.png", "Generate statistics and show results saved in file" );

	mSplitPlotsButton = CreateToolButton( "", ":/images/alpha-numeric/__s.png", "Split plots" );
	mSplitPlotsButton->setCheckable( true );

	auto *buttons = CreateButtonRow( false, Qt::Horizontal, 
	{ 
		b1, b2, 
		nullptr,
		mOperationExportButton, mOperationStatisticsButton, mSplitPlotsButton,		
	} );

	return buttons;
}


///////////////////////////////////////////////////////////////////////////////////////////////
//			Filters
///////////////////////////////////////////////////////////////////////////////////////////////

void COperationControls::ResetFilterSelection()
{
	auto reset_filter_selection = []( QToolButton *button, const COperation *operation )
	{
		std::string selected_filter_name;
		if ( operation && operation->Filter() )
			selected_filter_name = operation->FilterName();

		button->setText( "" );

		auto actions = button->actions();
		for ( auto a : actions )
		{
			std::string filter_name = q2a( a->text() );
			a->setChecked( filter_name == selected_filter_name );
			if ( a->isChecked() )
			{
				button->setText( a->text() );
			}
		};

		button->setChecked( !button->text().isEmpty() );
	};

	reset_filter_selection( mOperationFilterButton_Advanced, mCurrentOperation );
	reset_filter_selection( mOperationFilterButton_Quick, QuickOperation() );
}


void COperationControls::ResetFilterActions()
{
	auto reset_filter_actions = [this]( QToolButton *button, const CBratFilters &brat_filters )
	{
		auto actions = button->actions();
		for ( auto a : actions )
			button->removeAction( a );

		auto const &filters = brat_filters.FiltersMap();
		for ( auto const &filter : filters )
		{
			std::string filter_name = filter.first;
			QAction *a = CActionInfo::CreateAction( this, eAction_Item_Filters );
			a->setText( filter_name.c_str() );
			a->setToolTip( filter_name.c_str() );
			button->addAction( a );
		}
	};

	reset_filter_actions( mOperationFilterButton_Advanced, mBratFilters );
	connect( mOperationFilterButton_Advanced, SIGNAL( triggered( QAction * ) ), this, SLOT( HandleOperationFilterButton_Advanced( QAction * ) ) );
	connect( mOperationFilterButton_Advanced, SIGNAL( toggled( bool ) ), this, SLOT( HandleOperationFilterButtonToggled_Advanced( bool ) ) );

	reset_filter_actions( mOperationFilterButton_Quick, mBratFilters );
	connect( mOperationFilterButton_Quick, SIGNAL( triggered( QAction * ) ), this, SLOT( HandleOperationFilterButton_Quick( QAction * ) ) );
	connect( mOperationFilterButton_Quick, SIGNAL( toggled( bool ) ), this, SLOT( HandleOperationFilterButtonToggled_Quick( bool ) ) );

	ResetFilterSelection();
}


void COperationControls::HandleFilterCompositionChanged( std::string filter_name )
{
	auto &operations = *mWOperation->GetOperations();
	for ( auto &operation_entry : operations )
	{
		COperation *operation = dynamic_cast<COperation*>( operation_entry.second );
		if ( operation->FilterName() == filter_name )
		{
			operation->ReapplyFilter();
		}
	}
}


void COperationControls::HandleFiltersChanged()
{
	ResetFilterActions();
}


bool COperationControls::AssignFilter( const std::string &name )
{
	assert__( mCurrentOperation );

	const std::string &op_filter_name = mCurrentOperation->FilterName();

	if ( op_filter_name.empty() )
	{
		if ( !SimpleQuestion( "Are you sure you want to apply the filter '" + name + "' to the operation '" + mCurrentOperation->GetName() + "'?" ) )
		{
			return false;
		}
	}
	else
	{
		if ( op_filter_name != name &&
			!SimpleQuestion( "Operation '" + mCurrentOperation->GetName() + "' has associated filter '" + op_filter_name + "'. Are you sure you want to change it to '" + name + "'?" )
			)
		{
			return false;
		}
	}

	WaitCursor wait;

	auto *filter = mBratFilters.Find( name );			assert__( filter );
	mCurrentOperation->SetFilter( filter );
	return true;
}


bool COperationControls::RemoveFilter( const std::string &name )
{
	assert__( mCurrentOperation );		Q_UNUSED( name );		//release builds

	const std::string &op_filter_name = mCurrentOperation->FilterName();	assert__( name == op_filter_name );	

	if ( !op_filter_name.empty() && 
		!SimpleQuestion( "Are you sure you want to remove the filter '" + op_filter_name + "' from the operation '" + mCurrentOperation->GetName() + "'?" ) )
	{
		return false;
	}

	WaitCursor wait;

	mCurrentOperation->RemoveFilter();
	return true;
}


void COperationControls::HandleOperationFilterButton_Quick( QAction * )
{
}
void COperationControls::HandleOperationFilterButtonToggled_Quick( bool )
{
}


void COperationControls::HandleOperationFilterButtonToggled_Advanced( bool toggled )
{
	assert__( mCurrentOperation );

	const std::string filter_name = q2a( mOperationFilterButton_Advanced->text() );

	if ( toggled )
	{
		AssignFilter( filter_name );
	}
	else
	{
		RemoveFilter( filter_name );
	}

	//ugly...
	mOperationFilterButton_Advanced->blockSignals( true );
	mOperationFilterButton_Advanced->setChecked( mCurrentOperation->Filter() );
	mOperationFilterButton_Advanced->setText( mCurrentOperation->FilterName().c_str() );
	mOperationFilterButton_Advanced->blockSignals( false );

	mDataExpressionsTree->InsertOperation( mCurrentOperation );
}


void COperationControls::HandleOperationFilterButton_Advanced( QAction *a )
{
	QToolButton *bt = qobject_cast<QToolButton *>( sender() );
	if ( !bt )
		return;

	mOperationFilterButton_Advanced->setText( a->text() );
	HandleOperationFilterButtonToggled_Advanced( true );
	//qDebug() << "HandleOperationFilterButton==" << a->text();
}




///////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////


void COperationControls::Wire()
{
	//common

	ResetFilterActions();

	connect( mExportOperationAction, SIGNAL( triggered() ), this, SLOT( HandleExportOperation() ) );
	connect( mEditExportAsciiAction, SIGNAL( triggered() ), this, SLOT( HandleEditExportAscii() ) );
	connect( mOperationStatisticsButton, SIGNAL( clicked() ), this, SLOT( HandleOperationStatistics() ) );

	//...quick mQuickVariablesList

	connect( mQuickDatasetsCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( HandleSelectedDatasetChanged_Quick( int ) ) );
	connect( mQuickVariablesList, SIGNAL( currentRowChanged( int ) ), this, SLOT( HandleSelectedFieldChanged_Quick( int ) ) );
	connect( mQuickVariablesList, SIGNAL( itemChanged(QListWidgetItem*) ), this, SLOT( HandleVariableStateChanged_Quick(QListWidgetItem*) ) );

	connect( mDisplayMapButton, SIGNAL( clicked() ), this, SLOT( HandleQuickMap() ) );
	connect( mDisplayPlotButton, SIGNAL( clicked() ), this, SLOT( HandleQuickPlot() ) );	


	//...advanced

	connect( mNewOperationButton, SIGNAL( clicked() ), this, SLOT( HandleNewOperation() ) );
    connect( mRenameOperationButton, SIGNAL( clicked() ), this, SLOT( HandleRenameOperation() ) );
	connect( mDeleteOperationButton, SIGNAL( clicked() ), this, SLOT( HandleDeleteOperation() ) );
    connect( mDuplicateOperationButton, SIGNAL( clicked() ), this, SLOT( HandleDuplicateOperation() ) );

	connect( mAdvancedExecuteButton, SIGNAL( clicked() ), this, SLOT( HandleExecute() ) );
	connect( mDelayExecutionAction, SIGNAL( triggered() ), this, SLOT( HandleDelayExecution() ) );
	connect( mLaunchSchedulerAction, SIGNAL( triggered() ), this, SLOT( HandleLaunchScheduler() ) );

	connect( mOperationsCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( HandleSelectedOperationChanged( int ) ) );
	connect( mAdvancedDatasetsCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( HandleSelectedDatasetChanged_Advanced( int ) ) );
	connect( mAdvancedFieldsTree, SIGNAL( itemSelectionChanged() ), this, SLOT( HandleSelectedFieldChanged_Advanced() ) );

	connect( mInsertFunction, SIGNAL( clicked() ), this, SLOT( HandleInsertFunction() ) );
	connect( mInsertAlgorithm, SIGNAL( clicked() ), this, SLOT( HandleInsertAlgorithm() ) );
	connect( mInsertFormula, SIGNAL( clicked() ), this, SLOT( HandleInsertFormula() ) );
	connect( mSaveAsFormula, SIGNAL( clicked() ), this, SLOT( HandleSaveAsFormula() ) );

	connect( mShowAliasesButton, SIGNAL( clicked() ), this, SLOT( HandleShowAliases() ) );
	connect( mCheckSyntaxButton, SIGNAL( clicked() ), this, SLOT( HandleCheckSyntax() ) );
	connect( mShowInfoButton, SIGNAL( clicked() ), this, SLOT( HandleShowInfo() ) );

	for ( auto *a : mDataComputationGroup->actions() )
	{
		connect( a, SIGNAL( triggered() ), this, SLOT( HandleDataComputation() ) );
	}

	connect( mProcessesTable, SIGNAL( ProcessFinished( int, QProcess::ExitStatus, const COperation* ) ),
		this, SLOT( HandleProcessFinished( int, QProcess::ExitStatus, const COperation* ) ) );

	connect( mSwitchToMapButton, SIGNAL( clicked() ), this, SLOT( HandleSwitchExpressionType() ) );
	connect( mSwitchToPlotButton, SIGNAL( clicked() ), this, SLOT( HandleSwitchExpressionType() ) );

	connect( mDataExpressionsTree, SIGNAL( FormulaInserted( CFormula* ) ), this, SLOT( HandleFormulaInserted( CFormula* ) ) );
	connect( mDataExpressionsTree, SIGNAL( SelectedFormulaChanged( CFormula* ) ), this, SLOT( HandleSelectedFormulaChanged( CFormula* ) ) );

	mSwitchToMapButton->setChecked( true );

	//connect( &mTimer, SIGNAL( timeout() ), this, SLOT( HandleExecute() ) );
	//mTimer.start( 10000 );
}


COperationControls::COperationControls( CProcessesTable *processes_table, CModel &model, CDesktopManagerBase *manager, QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0 
	: base_t( model, manager, parent, f )
	, mProcessesTable( processes_table )
	, mBratFilters( mModel.BratFilters() )
{
	CreateQuickOperationsPage();		assert__( mQuickOperationsPage );
	CreateAdancedOperationsPage();		assert__( mAdvancedOperationsPage );

    mStackWidget = new CStackedWidget( this, { 
		{ mQuickOperationsPage, "Quick", "", "", true }, 
		{ mAdvancedOperationsPage, "Advanced", "", "", true } 
	} );

	// Layout Stack Widget Buttons & add Stack Widget itself
	
    stack_button_type *quick_button = qobject_cast<stack_button_type*>( mStackWidget->Button( 0 ) );
    stack_button_type *advanced_button = qobject_cast<stack_button_type*>( mStackWidget->Button( 1 ) );

	mCommonGroup = CreateCommonWidgets( quick_button, advanced_button );

	AddTopLayout( ELayoutType::Vertical, {
		mCommonGroup,
		WidgetLine( nullptr, Qt::Horizontal ),
		mStackWidget,
		nullptr
	},
	s, m, m, m, m
	);


	mSamplingGroup->setEnabled( false );		//TODO delete when implemented

    Wire();
}


bool COperationControls::AdvancedMode() const
{
    return mStackWidget->CurrentIndex() == eAdvanced;
}

void COperationControls::SetAdvancedMode( bool advanced ) const
{
    return mStackWidget->SetCurrentIndex( advanced ? eAdvanced : eQuick );
}




///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

void COperationControls::HandleWorkspaceChanged()
{
    mWDataset = nullptr;
    mWOperation = nullptr;
    mWDisplay = nullptr;
    mWFormula = nullptr;

	int index = mOperationsCombo->currentIndex();
	mOperationsCombo->clear();					//can trigger
	if ( index < 0 )
		HandleSelectedOperationChanged( -1 );

    mWRoot = mModel.RootWorkspace();
    if ( mWRoot )
	{
		mWDataset = mModel.Workspace< CWorkspaceDataset >();
		mWOperation = mModel.Workspace< CWorkspaceOperation >();
		mWDisplay = mModel.Workspace< CWorkspaceDisplay >();
		mWFormula = mModel.Workspace< CWorkspaceFormula >();		assert__( mWDataset && mWOperation && mWDisplay && mWFormula );
	}

    setEnabled( mWRoot != nullptr );

	HandleDatasetsChanged_Advanced( nullptr );			//disables tab if !mWDataset || !mWDataset->GetDatasetCount() > 0 

	HandleWorkspaceChanged_Quick();


	if ( !mWRoot )
	{
		return;
	}


	mWFormula->GetFormulaNames( mMapFormulaString );


	//fill (advanced) operation combo

    FillCombo( mOperationsCombo, *mWOperation->GetOperations(),

        0, true,

        []( const CObMap::value_type &i ) -> const char*
        {
            return i.first.c_str();
        }
    );
}


// NOTE connected by main window to datasets panel
//
void COperationControls::HandleDatasetsChanged_Advanced( CDataset *dataset )
{
    Q_UNUSED( dataset );

	mAdvancedDatasetsCombo->blockSignals( true );

	mAdvancedDatasetsCombo->clear();
	if ( mWDataset )
	{
		auto const &datasets = *mWDataset->GetDatasets();
		for ( auto dataset_entry : datasets )
		{
			const CDataset *dataset = dynamic_cast<CDataset*>( dataset_entry.second );		assert__( dataset );
			if ( !dataset->IsEmpty() )
				mAdvancedDatasetsCombo->addItem( dataset_entry.first.c_str() );
		}
	}
	int index = -1;
	mAdvancedDatasetsCombo->setCurrentIndex( -1 );
	const bool has_datasets = mAdvancedDatasetsCombo->count() > 0;
	if ( has_datasets )
	{
		index = mCurrentOperation ? mAdvancedDatasetsCombo->findText( mCurrentOperation->OriginalDatasetName().c_str() ) : 0;
		mAdvancedDatasetsCombo->setCurrentIndex( index );
	}

	mAdvancedDatasetsCombo->blockSignals( false );

	HandleSelectedDatasetChanged_Advanced( index );		//if -1, clear mDataset, mProduct and fields tree
	if ( mCurrentOperation )
	{
		HandleSelectedOperationChanged( mOperationsCombo->currentIndex() );		//TODO does not seem to be necessary
	}
	setEnabled( has_datasets );
}



void COperationControls::HandleSelectedOperationChanged( int operation_index )	//ComboOperation( int32_t currentOperation )
{
	mAdvancedExecuteButton->setEnabled( operation_index >= 0 );
	mSplitPlotsButton->setEnabled( operation_index >= 0 );

	mOperationExportButton->setEnabled( operation_index >= 0 );
	//mOperationFilterButton->setEnabled( operation_index >= 0 );
	mOperationStatisticsButton->setEnabled( operation_index >= 0 );

	mRenameOperationButton->setEnabled( operation_index >= 0 );
	mDuplicateOperationButton->setEnabled( operation_index >= 0 );
	mDeleteOperationButton->setEnabled( operation_index >= 0 );

	mSwitchToMapButton->setEnabled( operation_index >= 0 );
	mSwitchToPlotButton->setEnabled( operation_index >= 0 );
	mDataExpressionsTree->setEnabled( operation_index >= 0 );
	//mSamplingGroup->setEnabled( operation_index >= 0 );			TODO delete comment marks when implemented

	if ( operation_index < 0 )
	{
		mCurrentOperation = nullptr;
	}
	else
	{
		mCurrentOperation = mWOperation->GetOperation( q2a( mOperationsCombo->itemText( operation_index ) ) )	;					assert__( mCurrentOperation );
		int dataset_index = mAdvancedDatasetsCombo->findText( mCurrentOperation->OriginalDatasetName().c_str() );
		if ( mAdvancedDatasetsCombo->currentIndex() != dataset_index )
			mAdvancedDatasetsCombo->setCurrentIndex( dataset_index );	//trigger signal
		else
			HandleSelectedDatasetChanged_Advanced( dataset_index );
	}

	mDataExpressionsTree->InsertOperation( mCurrentOperation );

	ResetFilterSelection();
}


bool COperationControls::AssignDatasetAndProduct( const CDataset *dataset, bool changing_used_dataset )
{
	CProduct* new_product = nullptr;

	if ( mCurrentOperation && dataset )
	{
        const CDataset *op_filtered_dataset = const_cast<const COperation*>( mCurrentOperation )->Dataset();
        const CDataset *saved_filtered_dataset = op_filtered_dataset ? new CDataset( *op_filtered_dataset ) : nullptr;
		mCurrentOperation->SetDataset( dataset );
		const CDataset *filtered_dataset = const_cast<const COperation*>( mCurrentOperation )->Dataset();
		std::string error_msg;
		try
		{
			new_product = filtered_dataset->OpenProduct();
		}
		catch ( CException& e )
		{
			error_msg = std::string( "Unable to process files.\nReason:\n" ) + e.what(),
			new_product = nullptr;
		}
		if ( new_product == nullptr )
		{
			if ( !error_msg.empty() )
				SimpleErrorBox( error_msg );
			else
				SimpleErrorBox( "Unable to set Product\nPerhaps dataset file list is empty or product file doesn't exist." );

			mCurrentOperation->SetDataset( saved_filtered_dataset );
			return false;
		}

		if ( changing_used_dataset && !mDataExpressionsTree->SelectRecord( new_product ) )
		{
			SimpleErrorBox( "You have not selected a record name.\nDataset has not been changed.\nChanging operation dataset is canceled." );
			mCurrentOperation->SetDataset( saved_filtered_dataset );
			delete new_product;
			return false;
		}
	}

	mCurrentDataset = dataset;

	//////////////////////////////////////////////////////////////////void COperationPanel::OnDatasetSelChanged( wxTreeEvent &event ) - begin
	//DatasetSelChanged( id );////////////////////////////////////////void COperationPanel::DatasetSelChanged(id);

	delete mProduct;		//deletes product, inserts nullptr in GetFieldstreectrl
	mProduct = new_product;
	if ( mProduct != nullptr )
	{
		//TODO
		//m_productClass = m_product->GetProductClass().c_str();
		//m_productType = m_product->GetProductType().c_str();
	}

	mAdvancedFieldsTree->InsertProduct( mProduct );
	if ( !mCurrentOperation )
		return true;

	//TODO what is the meaning of the following lines ???
	//
	//wxTreeItemId rootIdFields = GetFieldstreectrl()->GetRootItem();
	//int32_t nRecords = ( rootIdFields.IsOk() ) ? GetFieldstreectrl()->GetChildrenCount( rootIdFields, false ) : 0;
	//if ( ( nRecords > 0 ) && ( nRecords <= 2 ) )
	//{
	//	wxTreeItemIdValue cookie;
	//	wxTreeItemId idChild = GetFieldstreectrl()->GetFirstChild( rootIdFields, cookie );
	//	if ( nRecords == 2 )
	//	{
	//		idChild = GetFieldstreectrl()->GetNextChild( rootIdFields, cookie );
	//	}
	//	GetFieldstreectrl()->Expand( idChild );
	//}
	//////////////////////////////////////////////////////////////////DatasetSelChanged(id); end
	//m_currentDataset = id;
	//SetCurrentDataset();
	//////////////////////////////////////////////////////////////////SetCurrentDataset(); begin
	if (!mCurrentDataset )
		mCurrentOperation->SetDataset( nullptr );
	mCurrentOperation->SetProduct( mProduct );
	//////////////////////////////////////////////////////////////////SetCurrentDataset(); end

	//// v3 note If there is only one record in the dataset ==> select it
	//GetOperationRecord();		sets GetFieldstreectrl ("Fields" tree) with mCurrentOperation->GetRecord();
	mAdvancedFieldsTree->SelectRecord( mCurrentOperation->GetRecord() );
	//SetCurrentRecord();
	mCurrentOperation->SetRecord( mAdvancedFieldsTree->GetCurrentRecord() );	

	//SetDataSetAndRecordLabel();

	//EnableCtrl();

	return true;
}


void COperationControls::HandleSelectedDatasetChanged_Advanced( int dataset_index )		   //OnDatasetSelChanging( wxTreeEvent &event )
{
	//lambdas

	auto cancel_dataset_change = [this]()
	{
		if ( mCurrentOperation )
			mAdvancedDatasetsCombo->setCurrentIndex( mAdvancedDatasetsCombo->findText( mCurrentOperation->OriginalDatasetName().c_str() ) );
	};


	//function body 

	CDataset *new_dataset = nullptr;
	bool changing_used_dataset = false;
	if ( dataset_index >= 0 )
	{
		auto new_dataset_name = q2a( mAdvancedDatasetsCombo->itemText( dataset_index ) );
		if ( mCurrentOperation )
		{
            std::string current_dataset_name = mCurrentOperation->OriginalDatasetName();						//TODO why cannot assert assert__( !mCurrentOperation->HasFormula() || !current_dataset_name.empty() );
			changing_used_dataset = mCurrentOperation->HasFormula() && new_dataset_name != current_dataset_name;
		}
		if ( changing_used_dataset )
		{
			std::string question =
				"Some data expressions are built from '"
				+ mCurrentOperation->OriginalDatasetName()
				+ "' dataset.\nIf you want to use another dataset, make sure that field names in data expressions are the same, or don't forget to change them.\nChange to '"
				+ new_dataset_name
				+ "' dataset ?";

			if ( !SimpleQuestion( question ) )
			{
				cancel_dataset_change();
				return;
			}

			mCurrentOperation->SetRecord( "" );

			while ( mCurrentOperation->GetFormulaCount() > 0 )
				mCurrentOperation->DeleteFormula( mCurrentOperation->GetFormulas()->begin()->first );

			mDataExpressionsTree->InsertOperation( mCurrentOperation );
		}

		new_dataset = mWDataset->GetDataset( new_dataset_name );	//do not return if dataset is the same: content may have changed
		if ( mCurrentOperation )
			mCurrentOperation->RemoveFilter();
	}

	if ( !AssignDatasetAndProduct( new_dataset, changing_used_dataset ) )
		cancel_dataset_change();
}



void COperationControls::SelectDataComputationMode()		//from COperationPanel::GetDataMode()
{
	if ( mCurrentOperation == nullptr )
	{
		return;
	}

	CFormula* formula = mDataExpressionsTree->SelectedFormula();
	if ( formula == nullptr )
	{
		return;
	}

	// if same pointer
	if ( mCurrentOperation->GetSelect() == formula )		//IsCriteriaSelection( formula )
	{
		return;
	}

	auto *action = mDataComputationGroup->findChild<QAction*>( QString( formula->GetDataModeAsString().c_str() ) );
	if ( action )
		action->setChecked( true );
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//				Data Expressions Operations
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void COperationControls::HandleSwitchExpressionType()
{
    QAbstractButton *b = qobject_cast< QAbstractButton* >( sender() );
	bool map = mSwitchToMapButton->isChecked();
	if ( map == mDataExpressionsTree->IsMap() )
		return;

	std::string error_msg;
	if ( !mDataExpressionsTree->CanSwitchType( error_msg ) )
	{
		SimpleErrorBox( error_msg );
		b->setChecked( false );
	}
	else
		mDataExpressionsTree->SwitchType();
}


void COperationControls::HandleSelectedFormulaChanged( CFormula *formula )
{
	mUserFormula = formula;
	mExpressionGroup->setEnabled( mUserFormula != nullptr );
	mDataComputation->setEnabled( mUserFormula != nullptr && !COperation::IsSelect( mUserFormula->GetName() ) );
	//mExpressionGroup->setTitle( mUserFormula ? mUserFormula->GetName().c_str() : "" );
	mExpressionGroup->setTitle( mUserFormula ? mDataExpressionsTree->ParentItemTitle( formula ).c_str() : "No Expression Selected" );
	SelectDataComputationMode();
}


void COperationControls::HandleFormulaInserted( CFormula *formula )
{
    Q_UNUSED( formula );

	LOG_INFO( "Inserted formula " + ( formula ? formula->GetName() : "null" ) );
	//std::string	error_msg;
	//mCurrentOperation->ComputeInterval( error_msg );

	//if ( !error_msg.empty() )
	//	SimpleErrorBox( error_msg );
}


void COperationControls::HandleInsertFunction()
{
	mDataExpressionsTree->InsertFunction();
}


std::string COperationControls::GetOpunit()		//TODO how to deal/assign with field units
{
	assert__( mUserFormula );

	return mUserFormula->GetUnitAsText();
}

void COperationControls::HandleInsertAlgorithm()
{
	assert__( mCurrentOperation );

	mDataExpressionsTree->InsertAlgorithm();

	//TODO how to deal/assign with field units		
	//SetUnitText();
}

void COperationControls::HandleInsertFormula()
{
	mDataExpressionsTree->InsertFormula();
}

void COperationControls::HandleSaveAsFormula()
{
	mDataExpressionsTree->SaveAsFormula();
}


void COperationControls::HandleDataComputation()
{
	assert__( mCurrentOperation && mUserFormula && mUserFormula == mDataExpressionsTree->SelectedFormula() );

	auto a = qobject_cast<QAction*>( sender() );	assert__( a );

	CFormula *formula = mUserFormula;	// TODO DODODODODODODODODODODODODO GetCurrentFormula();
	if ( formula == nullptr )
	{
		return;
	}

	// if same pointer
	if ( mCurrentOperation->GetSelect() == formula )	//== IsCriteriaSelection( formula ) in old OperationPanel
	{
		return;
	}

	auto id   = CMapDataMode::GetInstance().NameToId( q2a( a->text() ) );
	formula->SetDataMode( id );
}


void COperationControls::HandleCheckSyntax()
{
	assert__( mProduct );

	mDataExpressionsTree->CheckSyntax( mProduct );
}


void COperationControls::HandleShowInfo()
{
    CShowInfoDialog dlg( this );
    if ( dlg.exec() == QDialog::Accepted )
    {
        BRAT_NOT_IMPLEMENTED;
    }
}

void COperationControls::HandleShowAliases()
{
    CShowAliasesDialog dlg( this );
    if ( dlg.exec() == QDialog::Accepted )
    {
        BRAT_NOT_IMPLEMENTED;
    }

    //if ( mCurrentOperation == nullptr || mProduct == nullptr )
	//	return;

	//std::string title = "Show '" + mProduct->GetProductClassType() + "' aliases...";

	//CFormula* formula = GetOperationtreectrl()->GetCurrentFormula();
	//bool hasOpFieldSelected = formula != nullptr;

	//CAliasInfoDlg dlg( this, -1, title, mCurrentOperation, formula );

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




///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////



void COperationControls::HandleNewOperation()
{
	assert__( mWOperation );

	int dataset_index = mAdvancedDatasetsCombo->currentIndex();
	CDataset *dataset = nullptr;
	if ( dataset_index >= 0 )
	{
		dataset = mWDataset->GetDataset( q2a( mAdvancedDatasetsCombo->itemText( dataset_index ) ) );		assert__( dataset );
	}
	if ( dataset_index < 0 || dataset->IsEmpty() )
	{
		SimpleErrorBox( "Cannot create an operation without a dataset or with an empty one.\nPlease, select a valid dataset first." );
		return;
	}

	WaitCursor wait;

	//FillFormulaList();			//fills m_mapFormulaString (a CStringMap) with GetCurrentWorkspaceFormula()->GetFormulaNames(  );
	//FillDataTypeList();			//fills combo box "Type" (GetOptype) with CMapTypeData values (not in v4)
	//FillDataModeList();			//fills combo box "Data Computation" (GetOpDataMode) with CMapDataMode values (IN v4): see CreateDataComputationActions; can't understand why they did this for all new operation

	//GetOpnames()->Enable( true );

	//create and insert operation

	std::string op_name = mWOperation->GetOperationNewName();
	if ( !mWOperation->InsertOperation( op_name ) )		//v4 this is weired: call to GetOperationNewName ensures not existing name
	{
		SimpleErrorBox( "Operation '" + op_name + "' already exists" );
		//GetOpnames()->SetStringSelection( op_name );
		//m_currentOperationIndex = GetOpnames()->GetSelection();
	}
	else
	{
		/////////////////////////////////////////////////////////////////////////////do this later mOperationsCombo->addItem( op_name.c_str() );
		//m_currentOperationIndex = GetOpnames()->Append( op_name );
		//GetOpnames()->SetSelection( m_currentOperationIndex );
	}

	//SetCurrentOperation();		//assigns   mCurrentOperation, makes GetOperationtreectrl()->Insert(mCurrentOperation);
	mCurrentOperation = mWOperation->GetOperation( op_name );
	mCurrentOperation->InitOutput( mWOperation );
	mCurrentOperation->InitExportAsciiOutput( mWOperation );
	mDataExpressionsTree->InsertOperation( mCurrentOperation );		//makes mExpressionTextWidget->setText( formula ? formula->GetDescription().c_str() : "" );

	//GetDataMode();				//select value in GetOpDataMode combo
	SelectDataComputationMode();

	//InitOperationOutputs();		//makes mCurrentOperation->InitOutput and mCurrentOperation->InitExportAsciiOutput
	//GetOperationOutputs();		//repeats calls above in case mCurrentOperation output strings are empty (but WHY? if the repetition fills them the same way??)
	//see above

	//GetOperationDataset();		//selects in datasets widget the operation dataset, if any
	//In fact, for new operations this does NOTHING in v3

	//GetOperationtreectrl()->SelectItem( GetOperationtreectrl()->GetXRootId() );		//selects the X root in the "Data expressions" tree widget
	mDataExpressionsTree->SelectX();

	//SetCurrentFormula();		//fills formula related fields: "Expression", "Unit" list, GetOptype combo, GetOpDataMode list, labels below "Expression" (SetResolutionLabels); see SetCurrentFormula
	//auto *formula = mDataExpressionsTree->SelectedFormula();
	//mExpressionTextWidget->setText( formula ? formula->GetDescription().c_str() : "" );	see above

	//SetCurrentDataset();		//sets the (real) operation dataset and formula
	mCurrentOperation->SetDataset( dataset );
	mCurrentOperation->SetProduct( mProduct );

																					mOperationsCombo->addItem( op_name.c_str() );
																					mOperationsCombo->setCurrentIndex( mOperationsCombo->findText( op_name.c_str() ) );

	// v3 note: If there is only one record in the dataset ==> select it
	
	mAdvancedFieldsTree->SelectRecord( mCurrentOperation->GetRecord() );		//GetOperationRecord();		sets GetFieldstreectrl ("Fields" tree) with mCurrentOperation->GetRecord();
	mCurrentOperation->SetRecord( mAdvancedFieldsTree->GetCurrentRecord() );	//SetCurrentRecord();	//doesn't this return the record just assigned ???	

	/*
	SetDataSetAndRecordLabel();		//sets blue labels: 

	EnableCtrl();

	wxGetApp().GetDisplayPanel()->EnableCtrl();		//update display tab
	*/
}



//bool CBratGuiApp::CanDeleteOperation( const std::string& name, CStringArray* displayNames /*= nullptr*/ )
//{
//    bool canDelete = true;
//    CWorkspaceDisplay* wks = GetCurrentWorkspaceDisplay();
//    if ( wks == nullptr )
//    {
//        return true;
//    }

//    std::string errorMsg;
//    canDelete &= ( wks->UseOperation( name.ToStdString(), errorMsg, displayNames ) == false );
//    if ( !errorMsg.empty() )
//        wxMessageBox( errorMsg, "Error", wxOK | wxCENTRE | wxICON_ERROR );

//    return canDelete;
//}



void COperationControls::HandleDeleteOperation()
{
    assert__( mCurrentOperation );

    if ( !SimpleQuestion( "Are you sure to delete operation '" + mCurrentOperation->GetName() + "' ?" ) )
         return;

    //  Check display dependencies
    //
    {
        CStringArray display_names;
        std::string error_msg;
        bool can_delete = !mWDisplay->UseOperation( mCurrentOperation->GetName(), error_msg, &display_names );       assert__( error_msg.empty() );
        if ( !can_delete )
        {
            std::string str = display_names.ToString( "\n", false );
            SimpleWarnBox( "Unable to delete operation '"
                          + mCurrentOperation->GetName()
                          + "'.\nIt is used by the views below:\n"
                          + str
                          + "\n" );
            return;
        }
    }

    //

    if ( IsFile(mCurrentOperation->GetOutputPath() ) )         //COperationPanel::RemoveOutput()
    {

        if ( !SimpleQuestion( "Do you want to delete output file\n'"
                             + mCurrentOperation->GetOutputPath()
                             + "'\nlinked to this operation ?" ) )
        {
            return;
        }

        if ( !mCurrentOperation->RemoveOutput() )
        {
            SimpleWarnBox(
                        "Unable to delete file '"
                        + mCurrentOperation->GetOutputPath()
                        + "' \nYou have to delete it by yourself" );
        }
    }


    std::string op_name = mCurrentOperation->GetName();

    if ( !mWOperation->DeleteOperation( mCurrentOperation ) )
    {
        SimpleErrorBox( "Unable to delete operation '"
                        + mCurrentOperation->GetName()
                        + "'" );
        return;
    }

//    m_operation = nullptr;
//    m_userFormula = nullptr;

    int current = mOperationsCombo->currentIndex();     assert__( current == mOperationsCombo->findText( op_name.c_str() ) );
    mOperationsCombo->removeItem( current );
    int index = -1;
    if ( mOperationsCombo->count() > 0 )
        index = current > 0 ? current -1 : current;
    mOperationsCombo->setCurrentIndex( index );

    //SetCurrentOperation();		//assigns   mCurrentOperation, makes GetOperationtreectrl()->Insert(mCurrentOperation);

    //formula->GetDataModeAsString()

    //GetDataMode();

    //InitOperationOutputs();
    //GetOperationOutputs();

    //GetOperationDataset();

    //GetOperationtreectrl()->SelectItem(GetOperationtreectrl()->GetXRootId());

    //SetCurrentFormula();		//fills formula related fields: "Expression", "Unit" list, GetOptype combo, GetOpDataMode list, labels below "Expression" (SetResolutionLabels); see SetCurrentFormula

    //SetCurrentDataset();		//sets the (real) operation dataset and formula


    //GetOperationRecord();       //sets GetFieldstreectrl ("Fields" tree) with mCurrentOperation->GetRecord();            //TODO ????
    //SetCurrentRecord();         //mCurrentOperation->SetRecord( GetFieldstreectrl()->GetCurrentRecord().ToStdString() );

    //SetDataSetAndRecordLabel();		//sets blue labels:

//    EnableCtrl();

//    CDeleteOperationEvent ev(GetId(), opName);
//    wxPostEvent(GetParent(), ev);

//    EnableCtrl();
}



void COperationControls::HandleDuplicateOperation()
{
    assert__( mCurrentOperation );

    std::string op_name = mWOperation->GetOperationCopyName( mCurrentOperation->GetName() );

    if ( !mWOperation->InsertOperation(op_name, mCurrentOperation, mWDataset ) )
    {
        SimpleMsgBox( "Operation '" + op_name + "' already exists" );
//        GetOpnames()->SetStringSelection(m_operation->GetName());
//        m_currentOperationIndex = GetOpnames()->GetSelection();
    }
    else
    {
        mOperationsCombo->addItem( op_name.c_str() );
        int index = mOperationsCombo->findText( op_name.c_str() );       assert__( index >= 0 );
        mOperationsCombo->setCurrentIndex( index );
        //GetOpnames()->SetSelection(m_currentOperationIndex);
    }
}



void COperationControls::HandleRenameOperation()
{
    assert__( mCurrentOperation );

    auto result = SimpleInputString( "Operation Name", mCurrentOperation->GetName(), "Rename Operation" );
    if ( result.first )
    {
        if ( !mWOperation->RenameOperation( mCurrentOperation, result.second.c_str() ) )
        {
            SimpleMsgBox( "Unable to rename operation '"
                          + mCurrentOperation->GetName()
                          + "' by '"
                          + result.second
                          + "'\nPerhaps operation already exists, " );
        }
        else
        {
            mOperationsCombo->setItemText( mOperationsCombo->currentIndex(), mCurrentOperation->GetName().c_str() );

            std::string old_output = mCurrentOperation->GetOutputPath();
            const bool output_exists = IsFile( old_output );

            mCurrentOperation->InitExportAsciiOutput( mWOperation );
            mCurrentOperation->InitOutput( mWOperation );

            if ( !mCurrentOperation->RenameOutput( old_output ) )
            {
                SimpleWarnBox( "Unable to rename file '"
                               + old_output
                               + "' by '"
                               + mCurrentOperation->GetOutputPath()
                               + "'\nYou have to rename it by yourself" );
            }

            if ( output_exists ) // old file has been renamed, so we have to save ALL workspaces to keep consistency
            {
                std::string error_msg;
                if ( !mModel.SaveWorkspace( error_msg ) )
                {
                    assert__( !error_msg.empty() );

                    SimpleWarnBox( error_msg );
                }
            }

            emit OperationModified( mCurrentOperation );
        }
    }
}


void COperationControls::HandleExportOperation()
{
    CExportDialog dlg( this );
    if ( dlg.exec() == QDialog::Accepted )
    {
        BRAT_NOT_IMPLEMENTED;
    }
}
void COperationControls::HandleEditExportAscii()
{
    CEditExportAsciiDialog dlg( this );
    if ( dlg.exec() == QDialog::Accepted )
    {
        BRAT_NOT_IMPLEMENTED;
    }
}
void COperationControls::HandleOperationStatistics()
{
	BRAT_NOT_IMPLEMENTED;
}
void COperationControls::HandleDelayExecution()
{
    CDelayExecutionDialog dlg( this );
    if ( dlg.exec() == QDialog::Accepted )
    {
        BRAT_NOT_IMPLEMENTED;
    }
}


void COperationControls::SchedulerProcessError( QProcess::ProcessError error )
{
	auto message = "An error occurred launching scheduler application: " + CProcessesTable::ProcessErrorMessage( error );
	SimpleErrorBox( message );
	LOG_WARN( message );
}
void COperationControls::HandleLaunchScheduler()
{
	COsProcess *process = new COsProcess( nullptr, false, "", this, mModel.BratPaths().mExecBratSchedulerName );
    connect( process, SIGNAL( error( QProcess::ProcessError ) ), this, SLOT( SchedulerProcessError( QProcess::ProcessError ) ) );
	process->Execute();
}





/////////////////////////////////////////////////////////////////////////////////
//						Launch Display Actions
/////////////////////////////////////////////////////////////////////////////////


void COperationControls::LaunchDisplay( const std::string &display_name )
{
	assert__( mCurrentOperation );

    if ( MapRequested() )
    {
        assert__( mCurrentOperation->IsMap() );

        auto ed = new CMapEditor( &mModel, mCurrentOperation, display_name );
        auto subWindow = mDesktopManager->AddSubWindow( ed );
        subWindow->show();
    }
    else
    //if ( mCurrentOperation->IsZFXY() || mCurrentOperation->IsYFX() )
    {
        auto ed = new CPlotEditor( &mModel, mCurrentOperation, display_name );
        auto subWindow = mDesktopManager->AddSubWindow( ed );
        subWindow->show();
    }
    //else
    //    assert__( false );
}


/////////////////////////////////////////////////////////////////////////////////
//								Execute
/////////////////////////////////////////////////////////////////////////////////

bool COperationControls::MapRequested() const
{
    return mSwitchToMapButton->isChecked();
}

//	Operation::Execute generates 
//		operation.par 
//		operation.nc
//
//	Display::Execute generates 
//		display.par
//
//
void COperationControls::HandleProcessFinished( int exit_code, QProcess::ExitStatus exitStatus, const COperation *operation )
{
	const bool success = 
		exit_code == 0 &&
		exitStatus == QProcess::NormalExit;

	if ( !success )
		return;

	if ( operation != mCurrentOperation )	//should only happen if asynchronous process; requires refreshing all operations info
		return;			


	std::string display_name = mWDisplay->GetDisplayNewName();
	mWDisplay->InsertDisplay( display_name );
	CDisplay *display = mWDisplay->GetDisplay( display_name );
	display->InitOutput( mWDisplay );
	if (!display->AssignOperation( mCurrentOperation ) )
	////display panel -> GetOperations();
	////display panel -> GetDispavailtreectrl()->InsertData( &m_dataList );
	//CMapDisplayData *m_dataList = new CMapDisplayData;
	//m_dataList->clear();
	//if ( !CreateDisplayData( mCurrentOperation, *m_dataList ) )	//CDisplay::GetDisplayType failed
	{
		SimpleErrorBox( "Could not retrieve operation output." );
		return;	
	}
	auto displays = mModel.OperationDisplays( operation->GetName() );
	for ( auto *display : displays )
	{
		if ( !display->AssignOperation( mCurrentOperation, true ) )
		{
			SimpleErrorBox( "Could not retrieve operation output." );
			return;
		}
	}
	//for ( auto &data : *m_dataList )
	//	display->InsertData( data.first, dynamic_cast<CDisplayData*>( data.second ) );

	LaunchDisplay( display_name );
}

bool COperationControls::HandleExecute()
{
	return Execute( true );
}

///////////////////////////////////////////////////////////////////
//
//	TODO BIG one
//
//	CRITICAL: check if operation is selected in any editor and 
//		close it. Emit signal, at least after operation execution, 
//		so that all display editors refresh their internal data.
//
//	This happens for all destructive operations, so a method must
//	be created that collects all currently selected operations and 
//	prevents operating on it if it is selected.
//
//	Furthermore, in the display editors a device must be implemented, 
//	and made compatible with existing execution flow, and tested, 
//	and what not, to refresh all internal data... gosh...
//
///////////////////////////////////////////////////////////////////

bool COperationControls::Execute( bool sync )
{
    //static int n = 0;

	// ProcessesTable will display user messages for us, no need to report on false return
	//
	//emit SyncProcessExecution( true );
	//mProcessesTable->Add( true, true, "brat-lab", "brat-lab", { "20", "ARG_0", "arg_1", "A_R_G" } );
	//emit SyncProcessExecution( false );
	//return;

	assert__( mCurrentOperation );

	//v4 checks 

    if ( MapRequested() )
    {
		if ( !mCurrentOperation->IsMap() )
		{
			SimpleErrorBox( "A map view was requested but the operation expressions are not suitable for a map.\n"
				"Map views require longitude, latitude and data fields." );
			return false;
		}
    }


	//v3 checks 

	auto CtrlOperation = [this]( CWorkspaceFormula *wks, std::string& msg, bool basicControl, const CStringMap* aliases )
	{
		if ( mCurrentOperation && mCurrentOperation->Control( wks, msg, basicControl, aliases ) )
		{
			if ( mUserFormula != nullptr )
			{
				mDataExpressionsTree->ExpressionTextWidget()->setText( mUserFormula->GetDescription().c_str() );		//mExpressionTextWidget is v3 GetOptextform()
			}
			return true;
		}

		return false;
	};


	std::string msg;
	bool operationOk = CtrlOperation( mWFormula, msg, false, &mMapFormulaString );
	if ( !operationOk )
	{
		SimpleWarnBox( 
			"Operation '"
			+ mCurrentOperation->GetName()
			+ "' has some errors and can't be executed"
			+ ( msg.empty() ? "." : ":\n" )
			+ msg );

		return false;
	}

	//wxGetApp().GotoLogPage();

	if ( sync )
	{
		mCurrentOperation->SetLogFile( mWOperation );
	}
	else
	{
		mCurrentOperation->ClearLogFile();
	}

	//v4
	if ( IsFile( mCurrentOperation->GetOutputPath() ) )         //COperationPanel::RemoveOutput()
	{
		if ( !mCurrentOperation->RemoveOutput() )
		{
			SimpleWarnBox(
				"Unable to delete file '"
				+ mCurrentOperation->GetOutputPath()
				+ "' \nYou have to delete it by yourself" );

			return false;
		}
	}

	//BuildCmdFile(); == following 
	std::string error_msg;
	if ( !mCurrentOperation->BuildCmdFile( mWFormula, mWOperation, error_msg ) )	//v3 didn't seem to care if this fails
	{
		assert__( !error_msg.empty() );

		SimpleWarnBox( error_msg );
		return false;
	}

	/*
	CPipedProcess* process = new CPipedProcess( 

		mCurrentOperation->GetTaskName(),
		wxGetApp().GetLogPanel(),
		mCurrentOperation->GetFullCmd(),
		wxGetApp().GetLogPanel()->GetLogMess(),
		&mCurrentOperation->GetOutputPath(),				//used in remove file, must be complete path
		mCurrentOperation->GetType() 

		);


	if ( wait )
	{
		process->SetExecuteFlags( wxEXEC_SYNC | wxEXEC_NODISABLE );
	}

	bool bOk = wxGetApp().GetLogPanel()->AddProcess( process );
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		bool isSync = ( ( process->GetExecuteFlags() & wxEXEC_SYNC ) == wxEXEC_SYNC );
		if ( isSync )
		{
			msg = std::string::Format( "\n\n===> Synchronous Task '%s' started with command line below:<===\n'%s'\n\n"
				"\n ==========> Please wait.... A report will display at the end of the task <==========\n\n",
				process->GetName().c_str(),
				process->GetCmd().c_str() );
			GetLogMess()->AppendText( msg );
			GetTasklist()->InsertProcess( process );
		}

		*/

	if ( !SimpleQuestion(
		"Operation '"
		+ mCurrentOperation->GetName()
		+ "' will be executed with the following command line\n\n"
		+ mCurrentOperation->GetFullCmd()
		+ "\n\nDo you want to proceed?" ) 
		)
		return false;

	// ProcessesTable will display user messages for us, no need to report on false return
	//
	emit SyncProcessExecution( true );
	bool result = mProcessesTable->Add( sync, false, mCurrentOperation );
	emit SyncProcessExecution( false );

	/*
		//----------------
		process->Execute();
		//----------------

		if ( !isSync )
		{

			msg = std::string::Format( "\n\n===> Asynchronous Task '%s' (pid %d) started with command line below:<===\n'%s'\n\n",
				process->GetName().c_str(),
				process->GetPid(),
				process->GetCmd().c_str() );
			GetLogMess()->AppendText( msg );
			GetTasklist()->InsertProcess( process );
		}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if ( bOk == false )
	{
		delete process;
		process = nullptr;
	}

	if ( wait )
	{
		wxGetApp().GetLogPanel()->LogFile( wxFileName( mCurrentOperation->GetLogFile() ) );
	}


	BRAT_NOT_IMPLEMENTED
	*/

	return result;
}



void COperationControls::HandleSelectedFieldChanged_Advanced()
{
	mAdvancedFieldDesc->setText( mAdvancedFieldsTree->GetCurrentFieldDescription().c_str() );
}

///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_OperationControls.cpp"
