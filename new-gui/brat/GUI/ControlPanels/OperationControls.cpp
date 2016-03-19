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

#include "GUI/ControlPanels/Dialogs/DelayExecutionDialog.h"
#include "GUI/ControlPanels/Dialogs/ExportDialog.h"
#include "GUI/ControlPanels/Dialogs/FormulaDialog.h"
#include "GUI/ControlPanels/Dialogs/InsertAlgorithmDialog.h"
#include "GUI/ControlPanels/Dialogs/InsertFunctionDialog.h"
#include "GUI/ControlPanels/Dialogs/ShowAliasesDialog.h"
#include "GUI/ControlPanels/Dialogs/ShowInfoDialog.h"

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
QList<QAction*> COperationControls::CreateDataComputationActions( QObject *parent )
{
	QList<QAction*> actions;
	auto map = CMapDataMode::GetInstance();
	for ( CMapDataMode::const_iterator it = map.begin(); it != map.end(); it++ )
	{
		if ( !isDefaultValue( it->second ) )
		{
			auto *action = new QAction( it->first.c_str(), parent );
			action->setCheckable( true );
			actions << action;
		}
	}
	return actions;
}



QWidget* COperationControls::CreateQuickOperationsPage()
{
	auto mQuickOperationsPage = new QWidget( this );

	// I. Buttons row

	mAddVariable = CreateToolButton( "Add", ":/images/OSGeo/new.png", "Add a new variable" );
	mClearVariables = CreateToolButton( "Clear", ":/images/OSGeo/workspace-delete.png", "Clear selection" );
	mDisplayMapButton = CreateToolButton( ButtonDisplayTextIsMap, ":/images/themes/default/propertyicons/map_tools.png", "Edit operation map" );
	mDisplayPlotButton = CreateToolButton( ButtonDisplayTextIsPlot, ":/images/themes/default/histogram.png", "Edit operation plot" );

	mAddVariable->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
	mClearVariables->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
	mDisplayMapButton->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
	mDisplayPlotButton ->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );

	mAddVariable->setEnabled( false );
	mClearVariables->setEnabled( false );

	QWidget *buttons_row = CreateButtonRow( false, Qt::Horizontal, {
		mAddVariable, mClearVariables, nullptr, mDisplayMapButton, mDisplayPlotButton } );

	// II. Dataset

	mQuickDatasetsCombo = new QComboBox;
	mQuickDatasetsCombo->setToolTip( "Selected operation dataset" );
	auto *data_label = new QLabel( "Dataset" );
	data_label->setAlignment( Qt::AlignCenter );
	auto *data_group = CreateGroupBox( ELayoutType::Horizontal, 
	{
		LayoutWidgets( Qt::Vertical, { new QLabel( "Dataset" ), mQuickDatasetsCombo }, nullptr, s, m, m, m, m )
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
	QGroupBox *vars_group = CreateGroupBox( ELayoutType::Vertical, { mQuickVariablesList}, "Variables" , nullptr, 0, m, m, m, m );

	auto mVarTable = new QTableWidget;
	mVarTable->setColumnCount( 4 );
	QGroupBox *desc_group = CreateGroupBox( ELayoutType::Vertical, { mVarTable }, "Variable Description" , nullptr, 0, m, m, m, m );


	// Assemble in quick operations page

	LayoutWidgets( Qt::Vertical, 
	{ 
		buttons_row,
		data_group,
		CreateSplitter( nullptr, Qt::Vertical, { vars_group, desc_group } )
	}, 
	mQuickOperationsPage, 0, m, m, m, m );

	return mQuickOperationsPage;
}




QWidget* COperationControls::CreateAdancedOperationsPage()
{
	QWidget *mAdvancedOperationsPage = new QWidget( this );

	// I. Top Buttons Row

	mNewOperationButton = CreateToolButton( "", ":/images/OSGeo/new.png", "Create a new operation" );
    mRenameOperationButton = CreateToolButton( "", ":/images/OSGeo/edit.png", "Rename the selected operation" );
    mDuplicateOperationButton = CreateToolButton( "", ":/images/themes/default/mActionCopySelected.png", "Duplicate the selected operation" );
    mDeleteOperationButton = CreateToolButton( "", ":/images/OSGeo/workspace-delete.png", "Delete the selected operation" );

	mAdvancedDisplayButton = CreateToolButton( "", ":/images/OSGeo/execute.png", "Execute the operation and edit resulting view" );	//ButtonDisplayIconNoOpPath
	mAdvancedDisplayButton->setPopupMode( QToolButton::MenuButtonPopup );
    mDelayExecutionAction = new QAction( "Delay Execution...", this );
	mLaunchSchedulerAction = CActionInfo::CreateAction( this, eAction_Launch_Scheduler );
	mAdvancedDisplayButton->addAction( mDelayExecutionAction );
	mAdvancedDisplayButton->addAction( mLaunchSchedulerAction );

	QWidget *top_buttons_row = CreateButtonRow( false, Qt::Horizontal, 
	{ 
        mNewOperationButton, mRenameOperationButton, mDeleteOperationButton, mDuplicateOperationButton, nullptr, mAdvancedDisplayButton
	} );


	// II. Operations & Datasets

	mOperationsCombo = new QComboBox;		
	mOperationsCombo->setToolTip( "Selected operation" );

	//dataset list

	mAdvancedDatasetsCombo = new QComboBox;
	mAdvancedDatasetsCombo->setToolTip( "Selected operation dataset" );

	auto *ops_group = CreateGroupBox( ELayoutType::Horizontal, 
	{ 
		LayoutWidgets( Qt::Vertical, { new QLabel("Operation"), mOperationsCombo }, nullptr, s, m, m, m, m ), 
		LayoutWidgets( Qt::Vertical, { new QLabel("Dataset"), mAdvancedDatasetsCombo }, nullptr, s, m, m, m, m )
	}, 
	"", nullptr, s, m, m, m, m );


	// III. Expression Group

	// ...1. Fields & Field Description Group

	// - fields
	auto adv_fields_label = new QLabel( "Fields", mAdvancedOperationsPage );
	mAdvancedFieldsTree = new CFieldsTreeWidget( mAdvancedOperationsPage );
	QBoxLayout *adv_fields_vl = LayoutWidgets( Qt::Vertical, { adv_fields_label, mAdvancedFieldsTree }, nullptr, s, m, m, m, m );

	auto adv_desc_label = new QLabel( "Variable Description", mAdvancedOperationsPage );
	auto mAdvVarTable = new QTableWidget( mAdvancedOperationsPage );
	mAdvVarTable->setColumnCount( 4 );
	QBoxLayout *adv_desc_vl = LayoutWidgets( Qt::Vertical, { adv_desc_label, mAdvVarTable }, nullptr, 0, m, m, m, m );

	QGroupBox *fields_group = CreateGroupBox( ELayoutType::Horizontal, { adv_fields_vl, adv_desc_vl }, "", nullptr, s, m, m, m, m );


	// ...2. Expressions Tree(s)

#if defined(USE_2_EXPRESSION_TREES)
	//...map tree page
    //
	QWidget *page_1 = mDataExpressionsTree = mDataExpressionsTrees[eExpressionTreeMap] = 
		new CDataExpressionsTreeWidget( true, mAdvancedOperationsPage, mAdvancedFieldsTree );

	//plot tree page
	//
	QWidget *page_2 = mDataExpressionsTrees[eExpressionTreePlot] = 
		new CDataExpressionsTreeWidget( false, mAdvancedOperationsPage, mAdvancedFieldsTree );

    // Group Stack Widget Buttons & add Stack Widget itself
    //
    mDataExpressionsStack = new CStackedWidget( nullptr, 
	{
		{ page_1, "Map", "", ":/images/themes/default/propertyicons/map_tools.png", true },
		{ page_2, "Plot", "", ":/images/themes/default/histogram.png", true }
	} );

    mSwitchToMapButton = qobject_cast<stack_button_type*>( mDataExpressionsStack->Button( eExpressionTreeMap ) );
    mSwitchToPlotButton = qobject_cast<stack_button_type*>( mDataExpressionsStack->Button( eExpressionTreePlot ) );
	auto *data_expressions_buttons = CreateButtonRow( Qt::Vertical, { mSwitchToMapButton, mSwitchToPlotButton } );

    auto *data_expressions_group = CreateGroupBox( ELayoutType::Horizontal, { data_expressions_buttons, mDataExpressionsStack }, "", nullptr, s, m, m, m, m );

#else
	mDataExpressionsTree = new CDataExpressionsTreeWidget( true, mAdvancedOperationsPage, mAdvancedFieldsTree );

	mSwitchToMapButton = CreateToolButton( "Map", ":/images/themes/default/propertyicons/map_tools.png", "Click to change the type of the expression output" );
	mSwitchToPlotButton = CreateToolButton( "Plot", ":/images/themes/default/histogram.png", "Click to change the type of the expression output" );

	QWidget *data_expressions_buttons = CreateButtonRow( true, Qt::Vertical, 
	{ 
		mSwitchToMapButton, mSwitchToPlotButton
	} );

    //auto *data_expressions_group = CreateGroupBox( ELayoutType::Horizontal, { data_expressions_buttons, mDataExpressionsTree }, "", nullptr, s, m, m, m, m );
	QWidget *data_expressions_group = new QWidget;
    LayoutWidgets( Qt::Horizontal, { mDataExpressionsTree, data_expressions_buttons }, data_expressions_group, s, m, m, m, m );

#endif


	//...3. Buttons Row & Expression

	mInsertFunction = CreateToolButton( "", ":/images/alpha-numeric/1.png", "Insert function" );
	mInsertAlgorithm = CreateToolButton( "", ":/images/alpha-numeric/2.png", "Insert algorithm" );
	mInsertFormula = CreateToolButton( "", ":/images/alpha-numeric/3.png", "Insert formula" );
	mSaveAsFormula = CreateToolButton( "", ":/images/alpha-numeric/4.png", "Save as formula" );
	mDataComputationGroup = CreateActionGroup( this, CreateDataComputationActions( this ), true );
	mDataComputation = CreateMenuButton(  "", ":/images/alpha-numeric/b.png", "Set how data are stored/computed", mDataComputationGroup->actions() );
	mShowInfoButton = CreateToolButton( "", ":/images/OSGeo/page-info.png", "Show information" );
    mShowAliasesButton = CreateToolButton( "", ":/images/alpha-numeric/__e.png", "Show aliases" );

	mCheckSyntaxButton = CreateToolButton( "", ":/images/OSGeo/check.png", "Check syntax" );

	auto *expression_buttons = CreateButtonRow( false, Qt::Horizontal, 
	{ 
		mInsertFunction, mInsertFormula, mSaveAsFormula, mInsertAlgorithm, mDataComputation, nullptr, mShowInfoButton, mShowAliasesButton, mCheckSyntaxButton
	} );

	mExpressionTextWidget = new CTextWidget;
	mExpressionGroup = CreateGroupBox( ELayoutType::Vertical, { expression_buttons, mExpressionTextWidget }, "Expression", mAdvancedOperationsPage, 0, m, m, m, m );

	//....Split in Group

    static const QString SyncGroup("SyncGroup");
	mOperationExpressionsGroup = CreateCollapsibleGroupBox( 
		ELayoutType::Vertical, { 
			CreateSplitter( nullptr, Qt::Vertical, { fields_group, data_expressions_group, mExpressionGroup } )
		}, 
		"Expressions", mAdvancedOperationsPage, s, 4, 4, 4, 4 );
	mOperationExpressionsGroup->setCollapsed( false );
	mOperationExpressionsGroup->setSyncGroup( SyncGroup );
	//expression_group->setCheckable( true );


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
	QgsCollapsibleGroupBox *sampling_group = CreateCollapsibleGroupBox( ELayoutType::Horizontal, { adv_filter_vl, line, sampling_gridl }, 
        "Sampling", mAdvancedOperationsPage, s, 4, 4, 4, 4 );
	sampling_group->setCollapsed( true );
	sampling_group->setCheckable( true );
	sampling_group->setSyncGroup( SyncGroup );


    LayoutWidgets( Qt::Vertical,
	{ 
		top_buttons_row, 
		ops_group,
		mOperationExpressionsGroup,
		sampling_group,
		nullptr
	}, 
	mAdvancedOperationsPage, 0, m, m, m, m );

	return mAdvancedOperationsPage;
}


//	QUESTIONS: 
//	save workspace, not individual things...
//	quick can only be quick if an auto/default operation is created
//	view: determined by operation: distinction quick/advanced does not seem to stand

QWidget* COperationControls::CreateCommonWidgets( QAbstractButton *b1, QAbstractButton *b2 )
{
	//buttons row

	//mOperationFilterButton = CActionInfo::CreatePopupButton( eActionGroup_Filters, QList<QAction*>() /*mOperationFilterGroup->actions() */);
	mOperationFilterButton = CActionInfo::CreatePopupButton( eActionGroup_Filters, QList<QAction*>() /*mOperationFilterGroup->actions() */);
	mOperationFilterButton->setPopupMode( QToolButton::DelayedPopup );
	mOperationFilterButton->setCheckable( true );
	mOperationFilterButton->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );


	mExportOperationAction = new QAction( "Export...", this );
	mEditExportAsciiAction = new QAction( "Edit ASCII Export", this );
	mOperationExportButton = CreateMenuButton( "", ":/images/OSGeo/export.png", "Export operation", { mExportOperationAction, mEditExportAsciiAction } );

	mOperationStatisticsButton = CreateToolButton( "", ":/images/OSGeo/stats.png", "Generate statistics and show results saved in file" );

	mSplitPlotsButton = CreateToolButton( "", ":/images/alpha-numeric/__s.png", "Split plots" );
	mSplitPlotsButton->setCheckable( true );

	auto *buttons = CreateButtonRow( false, Qt::Horizontal, 
	{ 
		b1, b2, nullptr,
		mOperationFilterButton, mOperationExportButton, mOperationStatisticsButton, mSplitPlotsButton,		
	} );

	return buttons;
}


void COperationControls::ResetSelectedFilter()
{
	std::string selected_filter_name;
	if ( mCurrentOperation )
		selected_filter_name = mCurrentOperation->FilterName();

	mOperationFilterButton->setChecked( false );			//triggers
	auto actions = mOperationFilterButton->actions();
	for ( auto a : actions )
	{
		std::string filter_name = q2a( a->text() );
		a->setChecked( filter_name == selected_filter_name );
		if ( a->isChecked() )
			//mOperationFilterButton->setDefaultAction( a );
			mOperationFilterButton->setChecked( true );
	}
}


void COperationControls::ResetFilterActions()
{
	mOperationFilterButton->setChecked( false );
	auto actions = mOperationFilterButton->actions();
	for ( auto a : actions )
		mOperationFilterButton->removeAction( a );

	auto const &filters = mBratFilters.FiltersMap();
	for ( auto const &filter : filters )
	{
		std::string filter_name = filter.first;
		QAction *a = new QAction( filter_name.c_str(), this );
		a->setCheckable( true );
		mOperationFilterButton->addAction( a );
		connect( a, SIGNAL( triggered() ), this, SLOT( HandleOperationFilter() ) );
	}
	CreateActionGroup( mOperationFilterButton, mOperationFilterButton->actions(), true );
	connect( mOperationFilterButton, SIGNAL( triggered( QAction * ) ), this, SLOT( HandleOperationFilterButton( QAction * ) ) );
	connect( mOperationFilterButton, SIGNAL( toggled( bool ) ), this, SLOT( HandleOperationFilterButtonToggled( bool ) ) );
	ResetSelectedFilter();
}

void COperationControls::Wire()
{
	//common

	ResetFilterActions();

	connect( mExportOperationAction, SIGNAL( triggered() ), this, SLOT( HandleExportOperation() ) );
	connect( mEditExportAsciiAction, SIGNAL( triggered() ), this, SLOT( HandleEditExportAscii() ) );
	connect( mOperationStatisticsButton, SIGNAL( clicked() ), this, SLOT( HandleOperationStatistics() ) );

	//...quick mQuickVariablesList

	connect( mQuickDatasetsCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( HandleSelectedDatasetChanged_Quick( int ) ) );
	connect( mQuickVariablesList, SIGNAL( currentRowChanged( int ) ), this, SLOT( HandleSelectedVariableChanged_Quick( int ) ) );
	connect( mQuickVariablesList, SIGNAL( itemChanged(QListWidgetItem*) ), this, SLOT( HandleVariableStateChanged_Quick(QListWidgetItem*) ) );

	connect( mDisplayMapButton, SIGNAL( clicked() ), this, SLOT( HandleQuickMap() ) );
	connect( mDisplayPlotButton, SIGNAL( clicked() ), this, SLOT( HandleQuickPlot() ) );	


	//...advanced

	connect( mNewOperationButton, SIGNAL( clicked() ), this, SLOT( HandleNewOperation() ) );
    connect( mRenameOperationButton, SIGNAL( clicked() ), this, SLOT( HandleRenameOperation() ) );
	connect( mDeleteOperationButton, SIGNAL( clicked() ), this, SLOT( HandleDeleteOperation() ) );
    connect( mDuplicateOperationButton, SIGNAL( clicked() ), this, SLOT( HandleDuplicateOperation() ) );

	connect( mAdvancedDisplayButton, SIGNAL( clicked() ), this, SLOT( HandleExecute() ) );
	connect( mDelayExecutionAction, SIGNAL( triggered() ), this, SLOT( HandleDelayExecution() ) );
	connect( mLaunchSchedulerAction, SIGNAL( triggered() ), this, SLOT( HandleLaunchScheduler() ) );

	connect( mOperationsCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( HandleSelectedOperationChanged( int ) ) );
	connect( mAdvancedDatasetsCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( HandleSelectedDatasetChanged_Advanced( int ) ) );

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

#if defined(USE_2_EXPRESSION_TREES)

	connect( mDataExpressionsStack, SIGNAL( PageChanged( int ) ), this, SLOT( HandleExpressionsTreePageChanged( int ) ) );
	connect( mDataExpressionsTrees[eExpressionTreeMap], SIGNAL( FormulaInserted( CFormula * ) ), this, SLOT( HandleFormulaInserted( CFormula * ) ) );
	connect( mDataExpressionsTrees[eExpressionTreePlot], SIGNAL( FormulaInserted( CFormula * ) ), this, SLOT( HandleFormulaInserted( CFormula * ) ) );

#else

	connect( mSwitchToMapButton, SIGNAL( clicked() ), this, SLOT( HandleSwitchExpressionType() ) );
	connect( mSwitchToPlotButton, SIGNAL( clicked() ), this, SLOT( HandleSwitchExpressionType() ) );

	connect( mDataExpressionsTree, SIGNAL( FormulaInserted( CFormula* ) ), this, SLOT( HandleFormulaInserted( CFormula* ) ) );
	connect( mDataExpressionsTree, SIGNAL( SelectedFormulaChanged( CFormula* ) ), this, SLOT( HandleSelectedFormulaChanged( CFormula* ) ) );

	mSwitchToMapButton->setChecked( true );

#endif

	mOperationExpressionsGroup->setCollapsed( false );


	//connect( &mTimer, SIGNAL( timeout() ), this, SLOT( HandleExecute() ) );
	//mTimer.start( 10000 );
}


COperationControls::COperationControls( const CBratFilters &filters, CProcessesTable *processes_table, CDesktopManagerBase *manager, QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0 
	: base_t( manager, parent, f )
	, mProcessesTable( processes_table )
	, mBratFilters( filters )
{
	QWidget *mQuickOperationsPage = CreateQuickOperationsPage();
	QWidget *mAdvancedOperationsPage = CreateAdancedOperationsPage();

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
		mStackWidget,
		nullptr
	},
	s, m, m, m, m
	);


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

void COperationControls::HandleSelectedWorkspaceChanged( CModel *model )
{
	mModel = model;

	int index = mOperationsCombo->currentIndex();
	mOperationsCombo->clear();					//can trigger
	if ( index < 0 )
		HandleSelectedOperationChanged( -1 );

	index = mAdvancedDatasetsCombo->currentIndex();
	mAdvancedDatasetsCombo->clear();
	if ( index < 0 )
		HandleSelectedDatasetChanged_Advanced( -1 );

	mQuickDatasetsCombo->clear();			 //TODO handle quick

    mWRoot = mModel ? mModel->RootWorkspace() : nullptr;
    if ( mModel && mWRoot )
	{
		mWDataset = mModel->Workspace< CWorkspaceDataset >();
		mWOperation = mModel->Workspace< CWorkspaceOperation >();
		mWDisplay = mModel->Workspace< CWorkspaceDisplay >();
		mWFormula = mModel->Workspace< CWorkspaceFormula >();		assert__( mWDataset && mWOperation && mWDisplay && mWFormula );
	}
	else
	{
		mWDataset = nullptr;
		mWOperation = nullptr;
		mWDisplay = nullptr;
		mWFormula = nullptr;
	}

    setEnabled( mModel && mWDataset && mWDataset->GetDatasetCount() > 0 );

	if ( !mModel || !mWRoot )
	{
		return;
	}


	mWFormula->GetFormulaNames( mMapFormulaString );


	//fill both datasets combos

	mAdvancedDatasetsCombo->blockSignals( true );
    FillCombo( mAdvancedDatasetsCombo, *mWDataset->GetDatasets(),

        -1, true,											//TODO default to last opened

        []( const CObMap::value_type &i ) -> const char*
        {
            return i.first.c_str();
        }
    );
	mAdvancedDatasetsCombo->blockSignals( false );


    HandleDatasetsChanged_Quick(nullptr);


	//fill (advanced) operation combo

    FillCombo( mOperationsCombo, *mWOperation->GetOperations(),

        0, true,											//TODO default to last opened

        []( const CObMap::value_type &i ) -> const char*
        {
            return i.first.c_str();
        }
    );
}




void COperationControls::HandleDatasetsChanged_Advanced(CDataset *)
{
	LOG_WARN("HandleDatasetsChanged_Advanced");
}


bool COperationControls::TryDatasetChange( int index )
{
	Q_UNUSED( index );
	return true;
}



void COperationControls::HandleSelectedOperationChanged( int operation_index )	//ComboOperation( int32_t currentOperation )
{
	mAdvancedDatasetsCombo->setEnabled( operation_index >= 0 );		// TODO also update state of all other...
	mAdvancedDisplayButton->setEnabled( operation_index >= 0 );
	mSplitPlotsButton->setEnabled( operation_index >= 0 );

	mOperationExportButton->setEnabled( operation_index >= 0 );
	mOperationFilterButton->setEnabled( operation_index >= 0 );
	mOperationStatisticsButton->setEnabled( operation_index >= 0 );

	mRenameOperationButton->setEnabled( operation_index >= 0 );
	mDuplicateOperationButton->setEnabled( operation_index >= 0 );
	mDeleteOperationButton->setEnabled( operation_index >= 0 );

	if ( operation_index < 0 )
	{
		mCurrentOperation = nullptr;
		//mDataset = nullptr;
		//mProduct = nullptr;
		//mUserFormula = nullptr;
	}
	else
	{
		mCurrentOperation = mWOperation->GetOperation( q2a( mOperationsCombo->itemText( operation_index ) ) )	;						assert__( mCurrentOperation );
		mAdvancedDatasetsCombo->setCurrentIndex( mAdvancedDatasetsCombo->findText( mCurrentOperation->GetDatasetName().c_str() ) );	//trigger signal
	}

	mDataExpressionsTree->InsertOperation( mCurrentOperation );

	ResetSelectedFilter();

	if ( mCurrentOperation == nullptr )
	{
		return;
	}

	CDataset *dataset = nullptr;
	//DatasetSelChanged(id);//////////////////////////////////////////////////////////////
	{
		//ClearFieldsInfo();

		dataset = mCurrentOperation->GetDataset();	 //GetDatasettreectrl()->GetDataset( id );
		assert__( dataset != nullptr );			//v4 can an operation have a nullptr dataset??? v3 tested and returned on nullptr; leave assert here until we understand

		std::string msg;
		try
		{
			mProduct = dataset->SetProduct();

			if ( mProduct != nullptr )
			{
				//TODO
				//m_productClass = mProduct->GetProductClass();
				//m_productType = mProduct->GetProductType();
			}
		}
		catch ( CException& e )
		{
			mProduct = nullptr;
			msg = std::string( "Unable to process files.\nReason:\n") + e.what();
		}

		if ( mProduct == nullptr )
		{
			if ( msg.empty() )
				msg = "Unable to set Product\nPerhaps dataset file list is empty or product file doesn't exist()";

			return;
		}


		mAdvancedFieldsTree->InsertProduct( mProduct );

		//	GetFieldstreectrl()->InsertProduct( mProduct );
		//wxTreeItemId rootIdFields = GetFieldstreectrl()->GetRootItem();
		//int32_t nRecords = rootIdFields.IsOk() ? GetFieldstreectrl()->GetChildrenCount( rootIdFields, false ) : 0;
		//if ( nRecords > 0 && nRecords <= 2 )
		//{
		//	wxTreeItemIdValue cookie;
		//	wxTreeItemId idChild = GetFieldstreectrl()->GetFirstChild( rootIdFields, cookie );
		//	if ( nRecords == 2 )
		//	{
		//		idChild = GetFieldstreectrl()->GetNextChild( rootIdFields, cookie );
		//	}
		//	GetFieldstreectrl()->Expand( idChild );
		//}
	}
	//////////////////////////////////////////////////////////////////////////////////////

	//m_currentDataset = id;
	////////////////////////////////////////////////////////////////////////////////////////SetCurrentDataset();
	{
		mCurrentOperation->SetDataset( dataset );
		mCurrentOperation->SetProduct( mProduct );
	}
	//// If there is only one record in the dataset ==> select it	
	//GetOperationRecord();			TODO ???//////////////////////////////////////COperationPanel::GetOperationRecord(bool select = true)
	//wxTreeItemId id;
	//if ( mCurrentOperation == nullptr )
	//	return id;
	//id = GetFieldstreectrl()->FindItem( mCurrentOperation->GetRecord() );
	//if ( select )
	//{
	//	m_currentRecord = id;
	//	// if no selection (current record < 0) and only one record in the list, select it
	//	if ( ( !m_currentRecord ) && ( GetFieldstreectrl()->GetRecordCount() == 1 ) )
	//	{
	//		m_currentRecord = GetFieldstreectrl()->GetFirstRecordId();
	//	}
	//	GetFieldstreectrl()->SelectItem( m_currentRecord );
	//	GetFieldstreectrl()->Expand( m_currentRecord );
	//}

	//SetCurrentRecord();			TODO ???/////////////////////////	bool COperationPanel::SetCurrentRecord()
	//{
	//	mCurrentOperation->SetRecord( GetFieldstreectrl()->GetCurrentRecord().ToStdString() );
	//}

	//SetDataSetAndRecordLabel();	TODO ???	///////////////////void COperationPanel::SetDataSetAndRecordLabel()
	//{
	//	if ( mProduct == nullptr )
	//	{
	//		GetOperationtreectrllabel()->SetLabel( "" );
	//		GetOpchangerecord()->Enable( false );
	//		return;
	//	}
	//	if ( mCurrentOperation == nullptr )
	//	{
	//		GetOperationtreectrllabel()->SetLabel( "" );
	//		GetOpchangerecord()->Enable( false );
	//		return;
	//	}

	//	std::string strFormat = "Product: %s/%s\nDataset: %s\nDefault record: %s";

	//	if ( CProductNetCdf::IsProductNetCdf( mProduct ) )
	//	{
	//		strFormat = "Product: %s/%s\nDataset: %s";
	//	}

	//	std::string str;
	//	std::string datasetName = "?";
	//	std::string recordName = "?";

	//	if ( !mCurrentOperation->GetDatasetName().empty() )
	//	{
	//		datasetName =  mCurrentOperation->GetDatasetName();
	//	}
	//	if ( !mCurrentOperation->GetRecord().empty() )
	//	{
	//		recordName =  mCurrentOperation->GetRecord();
	//	}

	//	if ( CProductNetCdf::IsProductNetCdf( mProduct ) )
	//	{
	//		GetOpchangerecord()->Enable( false );
	//		str.Printf( strFormat, m_productClass.c_str(), m_productType.c_str(), datasetName.c_str() );
	//	}
	//	else
	//	{
	//		GetOpchangerecord()->Enable( true );
	//		str.Printf( strFormat, m_productClass.c_str(), m_productType.c_str(), datasetName.c_str(), recordName.c_str() );
	//	}

	//	GetOperationtreectrllabel()->SetLabel( str );
	//}

	//EnableCtrl();					v4 this is the UpdateUI....
}




void COperationControls::HandleSelectedDatasetChanged_Advanced( int dataset_index )		   //OnDatasetSelChanging( wxTreeEvent &event )
{
	//lambdas

	auto cancel_dataset_change = [this]()
	{
		mAdvancedDatasetsCombo->setCurrentIndex( mAdvancedDatasetsCombo->findText( mCurrentOperation->GetDatasetName().c_str() ) );
	};


	//function body 

	if ( dataset_index < 0 )
	{
		return;
	}

	auto new_datset_name = q2a( mAdvancedDatasetsCombo->itemText( dataset_index ) );
	CDataset *new_dataset = mWDataset->GetDataset( new_datset_name );
	CProduct* product = nullptr;
	if ( new_datset_name != mCurrentOperation->GetDatasetName() )
	{
		std::string question =
			"Some data expressions are built from '"
			+ mCurrentOperation->GetDatasetName()
			+ "' dataset.\nIf you want to use another dataset, make sure that field names in data expressions are the same, or don't forget to change them.\nChange to '"
			+ new_datset_name
			+ "' dataset ?";

		if ( !SimpleQuestion( question ) )
		{
			cancel_dataset_change();
			return;
		}

		//mCurrentOperation->SetRecord( "" );
		std::string error_msg;
		try
		{
			product = new_dataset->SetProduct();
		}
		catch ( CException& e )
		{
			error_msg = std::string( "Unable to process files.\nReason:\n" ) + e.what(),
			product = nullptr;
		}
		if ( product == nullptr )
		{
			if ( !error_msg.empty() )
				SimpleErrorBox( error_msg );
			else
				SimpleErrorBox( "Unable to set Product\nPerhaps dataset file list is empty or product file doesn't exist." );

			cancel_dataset_change();
			return;
		}

		if ( !mDataExpressionsTree->SelectRecord( product ) )
		{
			SimpleErrorBox( "You have not selected a record name.\nDataset has not been changed.\nChanging operation dataset is canceled." );
			cancel_dataset_change();
			delete product;
			return;
		}
	}

	return;

	mDataset = new_dataset;

	//OnDatasetSelChanging( wxTreeEvent &event )

	//wxTreeItemId id = event.GetItem();

	//if (!id)
	//{
	//  event.Veto();
	//  return;
	//}
	//// Apparently some versions of the MS common controls does not send the events
	//// when a tree item is selected programatically, so wxWidgets sends it.  Other
	//// versions of the common controls do send the events and so you get it twice.
	//if (m_currentDataset == id)
	//{
	//  if (GetFieldstreectrl()->GetRecordCount() > 0)
	//  {
	//    SetCurrentDataset();
	//    event.Veto();
	//    return;
	//  }
	//}

	//if (mCurrentOperation == nullptr)
	//{
	//  event.Veto();
	//  return;
	//}

	//CDataset* dataset = GetDatasettreectrl()->GetDataset(id);
	//if (dataset == nullptr)
	//{
	//  event.Veto();
	//  return;
	//}


	//std::string datasetNameOperation =  mCurrentOperation->GetDatasetName();
	//std::string datasetName = dataset->GetName();
	//bool resetDataExpression = mCurrentOperation->HasFormula() && (datasetNameOperation.CmpNoCase(datasetName) != 0);

	//if (resetDataExpression)
	//{
	//  int32_t result =  wxMessageBox(std::string::Format("Some data expressions are built from '%s' dataset.\n"
	//                                            "If you want to use another dataset, make sure that field names in data expressions are the same, or don't forget to change them.\n"
	//                                            "Change to '%s' dataset ?",
	//                                                  mCurrentOperation->GetDatasetName().c_str(),
	//                                                  GetDatasettreectrl()->GetItemText(id).c_str()),
	//                               "Changing operation dataset",
	//                                wxYES_NO | wxCENTRE | wxICON_QUESTION);

	//  if (result != wxYES)
	//  {
	//    event.Veto();
	//    return;
	//  }

	//  mCurrentOperation->SetRecord("");

	//  CProduct* product = nullptr;

	//  try
	//  {
	//    product = dataset->SetProduct();
	//  }
	//  catch (CException& e)
	//  {
	//    wxMessageBox(std::string::Format("Unable to process files.\nReason:\n%s",
	//                                  e.what()),
	//                "Warning",
	//                wxOK | wxICON_EXCLAMATION);

	//    product = nullptr;
	//  }

	//  if (product == nullptr)
	//  {
	//    wxMessageBox("Unable to set Product\nPerhaps dataset file list is empty or or product file doesn't exist()",
	//                "Warning",
	//                wxOK | wxICON_EXCLAMATION);

	//    event.Veto();
	//    return;
	//  }

	//  bool bOk = GetOperationtreectrl()->SelectRecord(product);

	//  delete product;
	//  product = nullptr;

	//  if (!bOk)
	//  {
	//    wxMessageBox("You have not selected a record name.\n"
	//                 "Dataset has not been changed.\n",
	//                 "Changing operation dataset is cancelled",
	//                  wxOK | wxCENTRE | wxICON_WARNING);

	//    event.Veto();
	//    return;
	//  }

	//}

	//////////////////////////////////////////////////////////////////void COperationPanel::OnDatasetSelChanged( wxTreeEvent &event )

	//DatasetSelChanged( id );
	/////////////////////////////////////////////////////////////////////////////////////DatasetSelChanged(id);

	delete mProduct;		//deletes product, inserts nullptr in GetFieldstreectrl
	mProduct = product;
	if ( mProduct != nullptr )
	{
		//TODO
		//m_productClass = m_product->GetProductClass().c_str();
		//m_productType = m_product->GetProductType().c_str();
	}

	mAdvancedFieldsTree->InsertProduct( mProduct );

	//TODO uncomment ans translate all the following lines
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
	//////////////////////////////////////////////////////////////////////////////////////


	//m_currentDataset = id;
	//SetCurrentDataset();

	//// If there is only one record in the dataset ==> select it
	//GetOperationRecord();
	//SetCurrentRecord();

	//SetDataSetAndRecordLabel();


	//EnableCtrl();
}


//std::string COperationControls::FindOperation1stDisplayName() const
//{
//	std::string name;
//	if ( mCurrentOperation )
//	{
//		auto displays = mModel->OperationDisplays( mCurrentOperation->GetName() );
//		if ( displays.size() > 0 )
//			name = displays[ 0 ]->GetName();
//	}
//	return name;
//}


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
	if ( mCurrentOperation->GetSelect() == formula )			//IsCriteriaSelection( formula )
	{
		return;
	}


	auto *action = mDataComputationGroup->findChild<QAction*>( formula->GetDataModeAsString().c_str() );
	if ( action )
		action->setChecked( true );
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//		Data Expressions Tree
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if defined(USE_2_EXPRESSION_TREES)

void COperationControls::HandleExpressionsTreePageChanged( int index )
{
	assert__( index >= 0 && index < EExpressionTrees_size );

    mDataExpressionsTree = mDataExpressionsTrees[ index ];

	//TODO ???
}

#else

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


#endif


void COperationControls::HandleSelectedFormulaChanged( CFormula *formula )
{
	mUserFormula = formula;
	mExpressionGroup->setEnabled( mUserFormula != nullptr );
	mDataComputation->setEnabled( mUserFormula != nullptr && !COperation::IsSelect( mUserFormula->GetName() ) );
	//mExpressionGroup->setTitle( mUserFormula ? mUserFormula->GetName().c_str() : "" );
	mExpressionGroup->setTitle( mUserFormula ? mDataExpressionsTree->ParentItemTitle( formula ).c_str() : "No Expression Selected" );
}


void COperationControls::HandleFormulaInserted( CFormula *formula )
{
    Q_UNUSED( formula );

	//std::string	error_msg;
	//mCurrentOperation->ComputeInterval( error_msg );

	//if ( !error_msg.empty() )
	//	SimpleErrorBox( error_msg );
}


void COperationControls::HandleInsertFunction()
{
	assert__( mUserFormula );

	CInsertFunctionDialog dlg( this );
	if ( dlg.exec() == QDialog::Accepted )
	{
		std::string name = dlg.ResultFunction();
		std::string function_syntax = CMapFunction::GetInstance().GetSyntaxFunc( name );
		
		mExpressionTextWidget->insertPlainText( function_syntax.c_str() );
		mUserFormula->SetDescription( q2a( mExpressionTextWidget->toPlainText() ) );
		mDataExpressionsTree->FormulaChanged( mUserFormula );
	}
}


std::string COperationControls::GetOpunit()		//TODO how to deal/assign with field units
{
	assert__( mUserFormula );

	return mUserFormula->GetUnitAsText();
}


void COperationControls::HandleInsertAlgorithm()
{
	assert__( mCurrentOperation );

	CInsertAlgorithmDialog dlg( this );
	if ( dlg.exec() == QDialog::Accepted )
	{
		mExpressionTextWidget->insertPlainText( dlg.CurrentAlgorithm()->GetSyntax().c_str() );
		mUserFormula->SetDescription( q2a( mExpressionTextWidget->toPlainText() ) );				////SetTextFormula();

		//TODO how to deal/assign with field units		
		//SetUnitText();
	}
}


void COperationControls::HandleInsertFormula()
{
	CFormulaDialog dlg( mWFormula, this );
	if ( dlg.exec() == QDialog::Accepted )
	{
		//GetFormulaSyntax( GetOptextform(), GetOpunit() );
		std::string name = dlg.CurrentFormula()->GetName();
		std::string syntax = mWFormula->GetDescFormula( name, dlg.AsAlias() );
		mExpressionTextWidget->insertPlainText( syntax.c_str() );
		//TODO how to deal/assign with field units		
		//	unitCtrl->SetValue( dlg.m_formula->GetUnitAsText() );
		mUserFormula->SetDescription( q2a( mExpressionTextWidget->toPlainText() ) );				////SetTextFormula();

		//TODO how to deal/assign with field units		
		//SetUnitText();
	}
}


void COperationControls::HandleSaveAsFormula()
{
    CSaveAsFormula dlg( this );
    if ( dlg.exec() == QDialog::Accepted )
    {
        NOT_IMPLEMENTED;
    }
}
void COperationControls::HandleDataComputation()
{
	assert__( mCurrentOperation != nullptr );

	auto a = qobject_cast<QAction*>( sender() );	assert__( a );

	MSG_NOT_IMPLEMENTED( q2a( a->text() ) );
	return;

	CFormula *formula = nullptr;	// TODO DODODODODODODODODODODODODO GetCurrentFormula();
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


bool COperationControls::CheckSyntax()
{
	assert__( mCurrentOperation );

	std::string value = q2a( mExpressionTextWidget->toPlainText() );
	std::string value_out;
	bool result = false;
	std::string msg;

	if ( mCurrentOperation->IsSelect( mUserFormula ) )
	{
		result = CFormula::CheckExpression( mWFormula, value.c_str(), mCurrentOperation->GetRecord(), msg, nullptr, &mMapFormulaString, mProduct, &value_out );
		if ( !result )
			SimpleWarnBox( msg );
	}
	else
	{
		std::string unit;// = GetOpunit()->GetValue();	//TODO	TODO	TODO	TODO	TODO	TODO	TODO	TODO	TODO	TODO	TODO	
		result = CFormula::CheckExpression( mWFormula, value.c_str(), mCurrentOperation->GetRecord(), msg, &unit, &mMapFormulaString, mProduct, &value_out );
	}

	if ( ! value_out.empty() )
	{
		if ( value != value_out )
		{
			mExpressionTextWidget->setPlainText( value_out.c_str() );
		}
	}

	if ( !result )
	{
		mExpressionTextWidget->setFocus();
	}

	return result;
}


void COperationControls::HandleCheckSyntax()
{
	CheckSyntax();
}


void COperationControls::HandleShowInfo()
{
    CShowInfoDialog dlg( this );
    if ( dlg.exec() == QDialog::Accepted )
    {
        NOT_IMPLEMENTED;
    }
}

void COperationControls::HandleShowAliases()
{
    CShowAliasesDialog dlg( this );
    if ( dlg.exec() == QDialog::Accepted )
    {
        NOT_IMPLEMENTED;
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
	mDataExpressionsTree->InsertOperation( mCurrentOperation );

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
	auto *formula = mDataExpressionsTree->SelectedFormula();
	mExpressionTextWidget->setText( formula ? formula->GetDescription().c_str() : "" );

	//SetCurrentDataset();		//sets the (real) operation dataset and formula
	int index = mAdvancedDatasetsCombo->currentIndex();
	auto *dataset = index >= 0 ? mWDataset->GetDataset( q2a( mAdvancedDatasetsCombo->itemText( index ) ) ) : nullptr;
	mCurrentOperation->SetDataset( dataset );
	mCurrentOperation->SetProduct( mProduct );

																					mOperationsCombo->addItem( op_name.c_str() );
																					mOperationsCombo->setCurrentIndex( mOperationsCombo->findText( op_name.c_str() ) );

	// v3 note: If there is only one record in the dataset ==> select it
	//GetOperationRecord();			//sets GetFieldstreectrl ("Fields" tree) with mCurrentOperation->GetRecord();
	mAdvancedFieldsTree->SelectRecord( mCurrentOperation->GetRecord() );

	//SetCurrentRecord();				//mCurrentOperation->SetRecord( GetFieldstreectrl()->GetCurrentRecord().ToStdString() );
	//mCurrentOperation->SetRecord( mAdvancedFieldsTree->GetCurrentRecord() );		//isn't this stupid???


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
                if ( !mModel->SaveWorkspace( error_msg ) )
                {
                    assert__( !error_msg.empty() );

                    SimpleWarnBox( error_msg );
                }
            }

            emit OperationModified( mCurrentOperation );
        }
    }
}


void COperationControls::HandleOperationFilterButtonToggled( bool toggled )
{
	//assert__( mCurrentOperation );
	if ( !mCurrentOperation )
		return;

	const std::string &op_filter_name = mCurrentOperation->FilterName();
	if ( !toggled )
	{
		if ( !op_filter_name.empty() && !SimpleQuestion( "Are you sure you want to remove the filter '" + op_filter_name + "' from the operation '" + mCurrentOperation->GetName() + "'?" ) )
		{
			mOperationFilterButton->setChecked( true );
			return;
		}

		mCurrentOperation->SetFilterName( "" );

		mOperationFilterButton->setText( "" );
		auto actions = mOperationFilterButton->actions();
		for ( auto *a : actions )
			a->setChecked( false );
	}
}
void COperationControls::HandleOperationFilterButton( QAction *a )
{
	qDebug() << a->text();
}
void COperationControls::HandleOperationFilter()
{
	assert__( mCurrentOperation );

	const std::string &op_filter_name = mCurrentOperation->FilterName();
	std::string filter_name;
	QAction *a = nullptr;

	//if ( !mOperationFilterButton->isChecked() )
	//{
	//	//if ( !op_filter_name.empty() && !SimpleQuestion( "Are you sure you want to remove the filter '" + op_filter_name + "' from the operation '" + mCurrentOperation->GetName() + "'?" ) )
	//		return;
	//}
	//else
	{
		a = qobject_cast<QAction*>( sender() );	assert__( a );
		filter_name = q2a( a->text() );

	qDebug() << a->isChecked();

		if ( op_filter_name.empty() )
		{
			if ( !SimpleQuestion( "Are you sure you want to apply the filter '" + filter_name + "' to the operation '" + mCurrentOperation->GetName() + "'?" ) )
			{
				a->setChecked( false );
				return;
			}
		}
		else
		{
			if ( op_filter_name != filter_name &&
				!SimpleQuestion( "Operation '" + mCurrentOperation->GetName() + "' has associated filter '" + op_filter_name + "'. Are you sure you want to change it to '" + filter_name + "'?" )
				)
			{
				a->setChecked( false );
				return;
			}
		}
	}

	//mOperationFilterButton->setDefaultAction( a );
	//mOperationFilterButton->setChecked( true );
	mOperationFilterButton->setText( filter_name.c_str() );
	mOperationFilterButton->setChecked( true );
	mCurrentOperation->SetFilterName( filter_name );
}


void COperationControls::HandleExportOperation()
{
    CExportDialog dlg( this );
    if ( dlg.exec() == QDialog::Accepted )
    {
        NOT_IMPLEMENTED;
    }
}
void COperationControls::HandleEditExportAscii()
{
    CEditExportAsciiDialog dlg( this );
    if ( dlg.exec() == QDialog::Accepted )
    {
        NOT_IMPLEMENTED;
    }
}
void COperationControls::HandleOperationStatistics()
{
	NOT_IMPLEMENTED;
}
void COperationControls::HandleDelayExecution()
{
    CDelayExecutionDialog dlg( this );
    if ( dlg.exec() == QDialog::Accepted )
    {
        NOT_IMPLEMENTED;
    }
}
void COperationControls::HandleLaunchScheduler()
{
	NOT_IMPLEMENTED;
}





/////////////////////////////////////////////////////////////////////////////////
//						Launch Display Actions
/////////////////////////////////////////////////////////////////////////////////


void COperationControls::LaunchDisplay()
{
	assert__( mCurrentOperation );

//	if ( mCurrentOperation->IsMap() )
//	{
//		auto ed = new CMapEditor( mModel, mCurrentOperation, "", mManager->parentWidget() );
//		auto subWindow = mManager->AddSubWindow( ed );
//		subWindow->show();
//	}
//	else
//	if ( mCurrentOperation->IsZFXY() || mCurrentOperation->IsYFX() )
//	{
//		auto ed = new CPlotEditor( mModel, mCurrentOperation, "", mManager->parentWidget() );
//		auto subWindow = mManager->AddSubWindow( ed );
//		subWindow->show();
//	}
//	else
//		assert__( false );


    if ( IsMap() )
    {
        assert__( mCurrentOperation->IsMap() || mCurrentOperation->IsZFXY() );

        auto ed = new CMapEditor( mModel, mCurrentOperation, "", mManager->parentWidget() );
        auto subWindow = mManager->AddSubWindow( ed );
        subWindow->show();
    }
    else
    if ( mCurrentOperation->IsZFXY() || mCurrentOperation->IsYFX() )
    {
        auto ed = new CPlotEditor( mModel, mCurrentOperation, "", mManager->parentWidget() );
        auto subWindow = mManager->AddSubWindow( ed );
        subWindow->show();
    }
    else
        assert__( false );
}


/////////////////////////////////////////////////////////////////////////////////
//								Execute
/////////////////////////////////////////////////////////////////////////////////

bool COperationControls::IsMap() const
{
    return mSwitchToMapButton->isChecked();
}

void COperationControls::HandleProcessFinished( int exit_code, QProcess::ExitStatus exitStatus, const COperation *operation )
{
	const bool success = 
		exit_code == 0 &&
		exitStatus == QProcess::NormalExit;

	if ( !success )
		return;

	if ( operation != mCurrentOperation )	//should only happen if asynchronous process; requires refreshing all operations info
		return;			


	std::string dname = mWDisplay->GetDisplayNewName();
	mWDisplay->InsertDisplay( dname );
	CDisplay *display = mWDisplay->GetDisplay( dname );
	display->InitOutput( mWDisplay );

	//display panel -> GetOperations();
	//display panel -> GetDispavailtreectrl()->InsertData( &m_dataList );
	CMapDisplayData *m_dataList = new CMapDisplayData;
	m_dataList->clear();
	if ( !CreateDisplayData( mCurrentOperation, *m_dataList ) )	//CDisplay::GetDisplayType failed
	{
		SimpleErrorBox( "Could not retrieve operation output." );
		return;
	
	}
	for ( auto &data : *m_dataList )
		display->InsertData( data.first, dynamic_cast<CDisplayData*>( data.second ) );

	LaunchDisplay();
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

	auto CtrlOperation = [this]( CWorkspaceFormula *wks, std::string& msg, bool basicControl, const CStringMap* aliases )
	{
		if ( mCurrentOperation && mCurrentOperation->Control( wks, msg, basicControl, aliases ) )
		{
			if ( mUserFormula != nullptr )
			{
				mExpressionTextWidget->setText( mUserFormula->GetDescription().c_str() );		//mExpressionTextWidget is v3 GetOptextform()
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


	NOT_IMPLEMENTED
	*/

	return result;
}


///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_OperationControls.cpp"
