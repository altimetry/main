#include "new-gui/brat/stdafx.h"

#include "libbrathl/TreeField.h"
#include "libbrathl/Field.h"

#include "new-gui/Common/QtUtils.h"
#include "new-gui/Common/DataModels/TaskProcessor.h"
#include "new-gui/Common/GUI/ProcessesTable.h"
#include "new-gui/Common/GUI/TextWidget.h"

#include "BratLogger.h"

#include "DataModels/Model.h"
#include "DataModels/Filters/BratFilters.h"
#include "DataModels/Workspaces/Workspace.h"
#include "DataModels/Workspaces/Function.h"

#include "GUI/ActionsTable.h"
#include "GUI/DisplayEditors/MapEditor.h"
#include "GUI/DisplayEditors/PlotEditor.h"

#include "Dialogs/DelayExecutionDialog.h"
#include "Dialogs/ExportDialog.h"
#include "Dialogs/EditExportAsciiDialog.h"
#include "Dialogs/ShowInfoDialog.h"

#include "DataExpressionsTree.h"
#include "OperationControls.h"

#include "process/BratProcessExportAscii.h"



static const std::string ButtonDisplayTextNoOp = "View...";
static const std::string ButtonDisplayTextIsMap = "Map...";
static const std::string ButtonDisplayTextIsPlot = "Plot...";

static const QString Xresolution_labelFormat = "<b>X Resolution</b><br><font size='2'>expression: '%1'<br>unit: %2</font>";
static const QString Yresolution_labelFormat = "<b>Y Resolution</b><br><font size='2'>expression: '%1'<br>unit: %2</font>";
static const QString LonResolution_labelFormat = "<b>Lon Resolution</b><br><font size='2'>expression: '%1'<br>unit: %2</font>";
static const QString LatResolution_labelFormat = "<b>Lat Resolution</b><br><font size='2'>expression: '%1'<br>unit: %2</font>";

static const QString MinResolutionlabel   = "<font size='2'>Min.</font>";
static const QString MaxResolutionlabel   = "<font size='2'>Max.</font>";
static const QString StepResolutionlabel  = "<font size='2'>Step</font>";
static const QString LoessResolutionlabel = "<font size='2'>Loess Cut-Off</font>";
static const QString NbIntervalslabel     = "<font size='2'>Number of<br>Intervals</font>";

static const QString DataMaxMin_labelFormat = "<b>Min/Max data coverage</b><br>Gets the minimum and maximum '%1' coverage of the dataset.";

static const QString WarningResolutionIconPath = ":/images/OSGeo/warning.png";
static const std::string ButtonDisplayIconNoOpPath = ":/images/themes/default/propertyicons/display.svg";
static const std::string DateTimeFormat ="%Y-%m-%d %H:%M:%S";

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
void COperationControls::RemoveOperationFormulas( COperation *operation )
{
	while ( operation->GetFormulaCount() > 0 )
		operation->DeleteFormula( operation->GetFormulas()->begin()->first );
}


//static 
void COperationControls::SelectOperationDataset( COperation *operation, QComboBox *combo, bool block_signals )
{
	if ( block_signals )
		combo->blockSignals( true );

	int index = operation ? combo->findText( operation->OriginalDatasetName().c_str() ) : 0;
	combo->setCurrentIndex( index );

	if ( block_signals )
		combo->blockSignals( false );
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


//static
QList<QAction*> COperationControls::CreateDataSmoothingActions()
{
    QList<QAction*> actions;
    auto map = CMapTypeFilter::GetInstance();
    for ( CMapTypeFilter::const_iterator it = map.begin(); it != map.end(); it++ )
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
	mQuickDatasetsCombo->setMinimumWidth( min_readable_combo_width );

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
		{ smQuickPredefinedVariableNames[ eSSH ] }, 
		{ smQuickPredefinedVariableNames[ eSWH ] }, 
		{ smQuickPredefinedVariableNames[ eWinds ] }, 
		{ smQuickPredefinedVariableNames[ eSigma0 ] },
		{ smQuickPredefinedVariableNames[ eRange ] }
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


    // IV. Help Info

    CTextWidget *help_text = new CTextWidget;	//use spaces and/or try to have lines without very dissimilar lengths
    help_text->SetHelpProperties(
        "To quickly execute an operation, select a dataset, check a field "
        "of interest in the Fields list, and click the Map or Plot buttons "
        "to see the result.\n\n"
        "You can convert the quick operation into a standard (advanced) one "
		"by duplicating it in \"Advanced\" mode. A new name, that you can "
		"change later, will automatically be assigned to the new operation."
        , 3, 6, Qt::AlignCenter, true );
	help_text->setMaximumHeight( QWIDGETSIZE_MAX );
    QGroupBox *help_group = CreateGroupBox( ELayoutType::Grid, { help_text }, "About quick operations", nullptr, 6, 6, 6, 6, 6 );
//    help_text->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Maximum );
//    help_group->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Maximum );


	// Assemble in quick operations page

	LayoutWidgets( Qt::Vertical, 
	{ 
		buttons_row,
		data_group,
        CreateSplitter( nullptr, Qt::Vertical, { vars_group, desc_group, help_group } )
        //nullptr
	}, 
	mQuickOperationsPage, 0, m, m, m, m );
}




void COperationControls::CreateAdvancedOperationsPage()
{
	mAdvancedOperationsPage = new QWidget( this );

	// I. Top Buttons Row

    mNewOperationButton = CreateToolButton( "", ":/images/OSGeo/operation_new.png", "Create a new operation" );
    mRenameOperationButton = CreateToolButton( "", ":/images/OSGeo/operation_edit.png", "Rename the selected operation" );
    mDuplicateOperationButton = CreateToolButton( "", ":/images/OSGeo/operation_duplicate.png", "Duplicate the selected operation" );
    mDeleteOperationButton = CreateToolButton( "", ":/images/OSGeo/operation_delete.png", "Delete the selected operation" );

	mAdvancedExecuteButton = CreateToolButton( "Execute", ":/images/OSGeo/execute.png", "Execute the operation and edit resulting view" );
	mAdvancedExecuteButton->setPopupMode( QToolButton::MenuButtonPopup );
	mAdvancedExecuteButton->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
    mDelayExecutionAction = new QAction( "Schedule Execution...", this );
	mLaunchSchedulerAction = CActionInfo::CreateAction( this, eAction_Launch_Scheduler );
	mAdvancedExecuteButton->addAction( mDelayExecutionAction );
	mAdvancedExecuteButton->addAction( mLaunchSchedulerAction );

	mExportOperationAction = new QAction( "Export...", this );
	mEditExportAsciiAction = new QAction( "Edit ASCII Export...", this );
	mCreateExportedDisplaysAction = new QAction( "(Re)Create Views...", this );
	mOperationExportButton = CreateMenuButton( "", ":/images/OSGeo/export.png", "Export operation", 
	{ 
		mExportOperationAction, mEditExportAsciiAction, mCreateExportedDisplaysAction 
	} );

	QWidget *top_buttons_row = CreateButtonRow( false, Qt::Horizontal, 
	{ 
        mNewOperationButton, mRenameOperationButton, mDeleteOperationButton, mDuplicateOperationButton, nullptr, mOperationExportButton, mAdvancedExecuteButton
	} );


	// II. Operations & Datasets &Filters

	mOperationsCombo = new QComboBox;
	mOperationsCombo->setMinimumWidth( min_readable_combo_width );
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

    mInsertFunction = CreateToolButton( "", ":/images/OSGeo/function.png", "Insert function" );
    mInsertAlgorithm = CreateToolButton( "", ":/images/OSGeo/algorithm.png", "Insert algorithm" );
    mInsertFormula = CreateToolButton( "", ":/images/OSGeo/expression.png", "Insert formula" );
    mSaveAsFormula = CreateToolButton( "", ":/images/OSGeo/expression_save.png", "Save as formula" );
	mDataComputationGroup = CreateActionGroup( this, CreateDataComputationActions(), true );
    mDataComputation = CreateMenuButton(  "", ":/images/OSGeo/processing.png", "Set how data are stored/computed", mDataComputationGroup->actions() );
    mDataComputation->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );

    mDataSmoothingGroup = CreateActionGroup( this, CreateDataSmoothingActions(), true );
    mDataSmoothing = CreateMenuButton(  "", ":/images/OSGeo/brat_smoothing.png", "Set how data are filled between tracks.\nDon't forget to set 'Loess cut-off' on sampling section.", mDataSmoothingGroup->actions() );
    mDataSmoothing->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );

	mShowInfoButton = CreateToolButton( "", ":/images/OSGeo/page-info.png", "Show information" );
    mShowAliasesButton = CreateToolButton( "", ":/images/OSGeo/aliases.png", "Show aliases" );

	mCheckSyntaxButton = CreateToolButton( "", ":/images/OSGeo/check.png", "Check syntax" );

	auto *expression_buttons = CreateButtonRow( false, Qt::Horizontal, 
	{ 
        mInsertFunction, mInsertFormula, mSaveAsFormula, mInsertAlgorithm, nullptr, mDataComputation, mDataSmoothing, nullptr, mShowInfoButton, mShowAliasesButton, mCheckSyntaxButton
	} );


	// ...4. Sampling Sub-Group

    //QFrame *line = WidgetLine( nullptr, Qt::Vertical );
    //line->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );

    auto min_label       = new QLabel( MinResolutionlabel );     min_label->setAlignment(Qt::AlignCenter);
    auto max_label       = new QLabel( MaxResolutionlabel );     max_label->setAlignment(Qt::AlignCenter);
    auto step_label      = new QLabel( StepResolutionlabel );    step_label->setAlignment(Qt::AlignCenter);
    auto cut_label       = new QLabel( LoessResolutionlabel );   cut_label->setAlignment(Qt::AlignCenter);
    auto intervals_label = new QLabel( NbIntervalslabel );       intervals_label->setAlignment(Qt::AlignCenter);

    mGetDataMaxMinX = CreateToolButton( "", ":/images/OSGeo/data_minmax_x.png", "Get min/max X coverage" );
    mGetDataMaxMinY = CreateToolButton( "", ":/images/OSGeo/data_minmax_y.png", "Get min/max Y coverage" );

    mXLonLabel = new QLabel("X Resolution");  mXLonLabel->setAlignment(Qt::AlignCenter);
    mYLatLabel = new QLabel("Y Resolution");  mYLatLabel->setAlignment(Qt::AlignCenter);
    mXLonMinValue = new QLineEdit;            mXLonMinValue->setAlignment(Qt::AlignCenter);
    mXLonMaxValue = new QLineEdit;            mXLonMaxValue->setAlignment(Qt::AlignCenter);
    mYLatMinValue = new QLineEdit;            mYLatMinValue->setAlignment(Qt::AlignCenter);
    mYLatMaxValue = new QLineEdit;            mYLatMaxValue->setAlignment(Qt::AlignCenter);
    mXLonStep = new QLineEdit;                mXLonStep->setAlignment(Qt::AlignCenter);
    mYLatStep = new QLineEdit;			      mYLatStep->setAlignment(Qt::AlignCenter);
    mXLonIntervalsLabel = new QLabel;         mXLonIntervalsLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    mYLatIntervalsLabel = new QLabel;         mYLatIntervalsLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    mXLonCutOff = new QLineEdit;		      mXLonCutOff->setAlignment(Qt::AlignCenter);
    mYLatCutOff = new QLineEdit;			  mYLatCutOff->setAlignment(Qt::AlignCenter);

    // VALIDATORS for min/max X/Y, step and cut off //
    // Explanation:
    //   [-0-9:.]+  -> means only hyphens(-), numbers(0-9), colons(:) and points(.); one-or-more (+) or zero-or-more (*) repetition of the characters within character class ([]).
    //     (^$)     -> allows empty strings (otherwise editingFinished signal is not triggered ). (^) and ($) mean beginning and end of string, respectively.
    QRegExpValidator *DegreeAndTimeValidator = new QRegExpValidator( QRegExp("[-0-9:. ]*"), this );
    QRegExpValidator *LoessCutValidator      = new QRegExpValidator( QRegExp("[0-9]+|(^$)"), this );
    QRegExpValidator *StepValidator          = new QRegExpValidator( QRegExp("[-+/0-9.]+|(^$)"), this );

    mXLonMinValue->setValidator( DegreeAndTimeValidator );
    mXLonMaxValue->setValidator( DegreeAndTimeValidator );
    mYLatMinValue->setValidator( DegreeAndTimeValidator );
    mYLatMaxValue->setValidator( DegreeAndTimeValidator );
    mXLonStep->setValidator( StepValidator );
    mYLatStep->setValidator( StepValidator );
    mXLonCutOff->setValidator( LoessCutValidator );
    mYLatCutOff->setValidator( LoessCutValidator );

    XiconWarning = new QLabel;
    YiconWarning = new QLabel;
    auto *Xintr_WarnIcon = LayoutWidgets( Qt::Horizontal, { mXLonIntervalsLabel, XiconWarning }, nullptr, s, m, m, m, m );
    auto *Yintr_WarnIcon = LayoutWidgets( Qt::Horizontal, { mYLatIntervalsLabel, YiconWarning }, nullptr, s, m, m, m, m );

	QGridLayout *sampling_gridl = LayoutWidgets(
	{ 
//        mXLonLabel,       mXLonMinValue, mXLonMaxValue, mXLonStep,  mXLonCutOff, mXLonIntervalsLabel, XiconWarning,     nullptr,
//        new QLabel,		min_label,     max_label,     step_label, cut_label,   intervals_label,     new QLabel, nullptr,
//        mYLatLabel,       mYLatMinValue, mYLatMaxValue, mYLatStep,  mYLatCutOff, mYLatIntervalsLabel, YiconWarning,     nullptr
            new QLabel,			mXLonLabel,      new QLabel,		mYLatLabel,      new QLabel,		nullptr,
            mGetDataMaxMinX,	mXLonMinValue,   min_label,         mYLatMinValue,   mGetDataMaxMinY,   nullptr,
            new QLabel,			mXLonMaxValue,   max_label,         mYLatMaxValue,   new QLabel,		nullptr,
            new QLabel,			mXLonStep,       step_label,        mYLatStep,       new QLabel,		nullptr,
            new QLabel,			Xintr_WarnIcon,  intervals_label,   Yintr_WarnIcon,  new QLabel,		nullptr,
	},
    nullptr, 2, 4, 4, 4, 2 );

    auto *CutOff_Layout = LayoutWidgets( Qt::Horizontal, { nullptr, mXLonCutOff, nullptr, cut_label, nullptr, mYLatCutOff, nullptr }, nullptr, s, m, m, m, m );

    mSamplingGroup = CreateCollapsibleGroupBox( ELayoutType::Vertical, { sampling_gridl, CutOff_Layout },
        "Sampling", mAdvancedOperationsPage, s, 4, 4, 4, 2 );
	mSamplingGroup->setCollapsed( true );
    //static const QString SyncGroup("SyncGroup");
	//mSamplingGroup->setCheckable( true );
	//mSamplingGroup->setSyncGroup( SyncGroup );    

	mExpressionGroup = CreateGroupBox( ELayoutType::Vertical, 
	{ 
		expression_buttons, 
		LayoutWidgets( Qt::Horizontal, 
		{ 
			mDataExpressionsTree->ExpressionTextWidget(), 
			mDataExpressionsTree->AssignExpressionButton() 
		}, 
		nullptr, s + m, m, m, m, m ),
		mSamplingGroup,
	}
	, "Expression", mAdvancedOperationsPage, 0, m, m, m, m );

	//....Split in Group

	mOperationExpressionsGroup = CreateGroupBox( 
		ELayoutType::Vertical, { 
			CreateSplitter( nullptr, Qt::Vertical, { fields_group, data_expressions_group, mExpressionGroup } )
		}, 
		"", mAdvancedOperationsPage, s, 4, 4, 4, 4 );


    LayoutWidgets( Qt::Vertical,
	{ 
		top_buttons_row, 
		CreateGroupBox( ELayoutType::Horizontal, { ops_group }, "" ),
		mOperationExpressionsGroup,
		//mSamplingGroup,
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

	mOperationStatisticsButton = CreateToolButton( "", ":/images/OSGeo/stats.png", "Generate statistics and save results in file" );

	mSplitPlotsButton = CreateToolButton( "", ":/images/alpha-numeric/__s.png", "Split plots" );
	mSplitPlotsButton->setCheckable( true );

	auto *buttons = CreateButtonRow( false, Qt::Horizontal, 
	{ 
		b1, b2, 
		nullptr,
		mOperationStatisticsButton, mSplitPlotsButton,		
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
			if ( filter_name == selected_filter_name )
			{
				button->setText( a->text() );
			}
		};

		button->blockSignals( true );						//avoid "internal" assignments
		button->setChecked( !button->text().isEmpty() );
		button->blockSignals( false );
	};

	reset_filter_selection( mOperationFilterButton_Advanced, mCurrentOperation );
	reset_filter_selection( mOperationFilterButton_Quick, mQuickOperation );
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


void COperationControls::HandleFilterCompositionChanged( std::string filter_name )	//triggered if filter areas change
{
	auto &operations = *mWOperation->GetOperations();
	for ( auto &operation_entry : operations )
	{
		COperation *operation = dynamic_cast<COperation*>( operation_entry.second );
		if ( operation->FilterName() == filter_name )
		{
			operation->ReapplyFilter();			//TODO this impacts dataset
		}
	}
}


void COperationControls::HandleFiltersChanged()		//always triggered by new, triggered by rename and delete if filter not being used
{
	ResetFilterActions();
}


bool COperationControls::AssignFilter( const CBratFilters &brat_filters, COperation *operation, const std::string &name )
{
	assert__( operation );

	const std::string &op_filter_name = operation->FilterName();

	if ( op_filter_name.empty() )
	{
		if ( !SimpleQuestion( "Are you sure you want to apply the filter '" + name + "' to the operation '" + operation->GetName() + "'?" ) )
		{
			return false;
		}
	}
	else
	{
		if ( op_filter_name != name &&
			!SimpleQuestion( "Operation '" + operation->GetName() + "' has associated filter '" + op_filter_name + "'. Are you sure you want to change it to '" + name + "'?" )
			)
		{
			return false;
		}
	}

	WaitCursor wait;

	auto *filter = brat_filters.Find( name );			assert__( filter );
	operation->SetFilter( filter );
	return true;
}


bool COperationControls::AssignAdvancedFilter( const std::string &name )
{
	return AssignFilter( mBratFilters, mCurrentOperation, name );
}
bool COperationControls::AssignQuickFilter( const std::string &name )
{
	return AssignFilter( mBratFilters, mQuickOperation, name );
}


bool COperationControls::RemoveFilter( COperation *operation, const std::string &name )
{
	assert__( operation );		Q_UNUSED( name );		//release builds

	const std::string &op_filter_name = operation->FilterName();	assert__( name == op_filter_name && !op_filter_name.empty() );	

	if ( !SimpleQuestion( "Are you sure you want to remove the filter '" + op_filter_name + "' from the operation '" + operation->GetName() + "'?" ) )
	{
		return false;
	}

	WaitCursor wait;

	operation->RemoveFilter();
	return true;
}


bool COperationControls::RemoveAdvancedFilter( const std::string &name )
{
    return RemoveFilter( mCurrentOperation, name );
}
bool COperationControls::RemoveQuickFilter( const std::string &name )
{
    return RemoveFilter( mQuickOperation, name );
}


void UpdateFilterButton( QToolButton *button, const COperation *operation )
{
	//ugly...
	button->blockSignals( true );
	button->setChecked( operation->Filter() );
	button->setText( operation->FilterName().c_str() );
	button->blockSignals( false );
}


void COperationControls::HandleOperationFilterButtonToggled_Advanced( bool toggled )
{
	assert__( mCurrentOperation );

	const std::string filter_name = q2a( mOperationFilterButton_Advanced->text() );

	if ( toggled )
	{
		AssignAdvancedFilter( filter_name );
	}
	else
	{
		RemoveAdvancedFilter( filter_name );
	}

	UpdateFilterButton( mOperationFilterButton_Advanced, mCurrentOperation );
	if ( mCurrentOperation == mQuickOperation )
		UpdateFilterButton( mOperationFilterButton_Quick, mQuickOperation );

	mDataExpressionsTree->InsertOperation( mCurrentOperation );
}


void COperationControls::HandleOperationFilterButtonToggled_Quick( bool toggled )
{
	assert__( mQuickOperation );

	const std::string filter_name = q2a( mOperationFilterButton_Quick->text() );

	if ( toggled )
	{
		AssignQuickFilter( filter_name );
	}
	else
	{
		RemoveQuickFilter( filter_name );
	}

	UpdateFilterButton( mOperationFilterButton_Quick, mQuickOperation );
	if ( mCurrentOperation == mQuickOperation )
		UpdateFilterButton( mOperationFilterButton_Advanced, mCurrentOperation );
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
void COperationControls::HandleOperationFilterButton_Quick( QAction *a )
{
	QToolButton *bt = qobject_cast<QToolButton *>( sender() );
	if ( !bt )
		return;

	mOperationFilterButton_Quick->setText( a->text() );
	HandleOperationFilterButtonToggled_Quick( true );
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
	connect( mCreateExportedDisplaysAction, SIGNAL( triggered() ), this, SLOT( HandleCreateExportedDisplays() ) );
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

    for ( auto *a : mDataSmoothingGroup->actions() )
    {
        connect( a, SIGNAL( triggered() ), this, SLOT( HandleDataSmoothing() ) );
    }

	connect( mProcessesTable, SIGNAL( ProcessFinished( int, QProcess::ExitStatus, bool, void* ) ),
		this, SLOT( HandleProcessFinished( int, QProcess::ExitStatus, bool, void* ) ) );

	connect( mSwitchToMapButton, SIGNAL( clicked() ), this, SLOT( HandleSwitchExpressionType() ) );
	connect( mSwitchToPlotButton, SIGNAL( clicked() ), this, SLOT( HandleSwitchExpressionType() ) );

	connect( mDataExpressionsTree, SIGNAL( FormulaInserted( CFormula* ) ), this, SLOT( HandleFormulaInserted( CFormula* ) ) );
	connect( mDataExpressionsTree, SIGNAL( SelectedFormulaChanged( CFormula* ) ), this, SLOT( HandleSelectedFormulaChanged( CFormula* ) ) );

	connect( mStackWidget, SIGNAL( PageChanged( int ) ), this, SLOT( HandlePageChanged( int ) ) );

    connect( mXLonMinValue, SIGNAL( editingFinished() ), this, SLOT( HandleXLonMin_changed() ) );
    connect( mXLonMaxValue, SIGNAL( editingFinished() ), this, SLOT( HandleXLonMax_changed() ) );
    connect( mYLatMinValue, SIGNAL( editingFinished() ), this, SLOT( HandleYLatMin_changed() ) );
    connect( mYLatMaxValue, SIGNAL( editingFinished() ), this, SLOT(HandleYLatMax_changed() ) );
    connect( mXLonStep, SIGNAL( editingFinished() ), this, SLOT(HandleXLonStep_changed() ) );
    connect( mYLatStep, SIGNAL( editingFinished() ), this, SLOT(HandleYLatStep_changed() ) );
    connect( mXLonCutOff, SIGNAL( editingFinished() ), this, SLOT(HandleXLoessCut_changed() ) );
    connect( mYLatCutOff, SIGNAL( editingFinished() ), this, SLOT(HandleYLoessCut_changed() ) );

    connect( mGetDataMaxMinX, SIGNAL( clicked() ), this, SLOT( HandleGetDataMinMaxX() ) );
    connect( mGetDataMaxMinY, SIGNAL( clicked() ), this, SLOT( HandleGetDataMinMaxY() ) );

	mProcessesTable->HideColumns( { CProcessesTable::eTaskUid, CProcessesTable::eTaskStart, CProcessesTable::eTaskStatus, CProcessesTable::eTaskLogFile } );

	mSwitchToMapButton->setChecked( true );

	//connect( &mTimer, SIGNAL( timeout() ), this, SLOT( HandleExecute() ) );
	//mTimer.start( 10000 );

	HandlePageChanged( AdvancedMode() ? eAdvanced : eQuick );	//this should be the last thing to do
}


COperationControls::COperationControls( CProcessesTable *processes_table, CModel &model, CDesktopManagerBase *manager, QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0 
	: base_t( model, manager, parent, f )
	, mProcessesTable( processes_table )
	, mBratFilters( mModel.BratFilters() )
{
	CreateQuickOperationsPage();		assert__( mQuickOperationsPage );
	CreateAdvancedOperationsPage();		assert__( mAdvancedOperationsPage );

    mStackWidget = new CStackedWidget( this, { 
		{ mQuickOperationsPage, "Quick", "", "", true }, 
		{ mAdvancedOperationsPage, "Advanced", "", "", true } 
	} );

	// Layout Stack Widget Buttons & add Stack Widget itself
	
    mQuickPageButton = qobject_cast<stack_button_type*>( mStackWidget->Button( 0 ) );
    mAdvancedPageButton = qobject_cast<stack_button_type*>( mStackWidget->Button( 1 ) );

	mCommonGroup = CreateCommonWidgets( mQuickPageButton, mAdvancedPageButton );

	AddTopLayout( ELayoutType::Vertical, {
		mCommonGroup,
		WidgetLine( nullptr, Qt::Horizontal ),
		mStackWidget,
		nullptr
	},
	s, m, m, m, m
	);


    Wire();
}



//virtual 
COperationControls::~COperationControls()
{
	delete mProduct;
	mProduct = nullptr;
}


bool COperationControls::AdvancedMode() const
{
    return mStackWidget->CurrentIndex() == eAdvanced;
}


void COperationControls::SetAdvancedMode( bool advanced )
{
	bool force = AdvancedMode() == advanced;
    mStackWidget->SetCurrentIndex( advanced ? eAdvanced : eQuick );
	if ( force )
		HandlePageChanged( mStackWidget->CurrentIndex() );
}


void COperationControls::HandlePageChanged( int index )
{
	EMode m = (EMode)index;

	//enable = hasOperation && hasDataset;
	//enableField = (GetDatasettreectrl()->GetSelection().IsOk() && (GetFieldstreectrl()->GetCount() > 0));
    //hasUserFormula = mCurrentOperation->HasFormula();
	//enableExecute = enable && enableField && hasUserFormula;

    //enableExportAsciiEdit = wxFile::Exists(mCurrentOperation->GetExportAsciiOutputName());

	COperation *operation = nullptr;
	switch ( m )
	{
		case eAdvanced:
		{
			operation = mCurrentOperation;
		}
			break;

		case eQuick:
		{
			operation = mQuickOperation;
		}
			break;

		default:
			assert__( false );
			break;
	}

	mOperationStatisticsButton->setEnabled( operation && operation->HasFormula() );
	mSplitPlotsButton->setEnabled( operation );
}



/////////////////////////////////////////////////////////////////////////////////
//	Operation Control Panel Helpers
/////////////////////////////////////////////////////////////////////////////////


// Fills datasets combo
//	- no signals: no side effects
//	- disables tab if there are no datasets
//	- selects mCurrentOperation dataset, if mCurrentOperation  not null
//	- selects suggested_index only if mCurrentOperation is null
//
void COperationControls::FillDatasets_Advanced( int suggested_index )
{
	mAdvancedDatasetsCombo->blockSignals( true );

	mAdvancedDatasetsCombo->clear();
	if ( mWDataset )
	{
		auto const &datasets = *mWDataset->GetDatasets();
		for ( auto dataset_entry : datasets )
		{
            const CDataset *dataset = dynamic_cast<CDataset*>( dataset_entry.second );		assert__( dataset );	Q_UNUSED( dataset );
			mAdvancedDatasetsCombo->addItem( dataset_entry.first.c_str() );
		}
	}
	mAdvancedDatasetsCombo->setCurrentIndex( suggested_index );
	const bool has_datasets = mAdvancedDatasetsCombo->count() > 0;
	if ( has_datasets )
	{
		SelectOperationDataset( mCurrentOperation, mAdvancedDatasetsCombo, false );	//false: signals are already blocked
	}

	mAdvancedDatasetsCombo->blockSignals( false );

	setEnabled( has_datasets );
}


bool COperationControls::IsQuickOperationSelected() const
{
	static const std::string quick_opname = CWorkspaceOperation::QuickOperationName();

	int quick_operation_index = mOperationsCombo->findText( quick_opname.c_str() );
	int operation_index = mOperationsCombo->currentIndex();
	return quick_operation_index != -1 && quick_operation_index == operation_index;
}


/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
//						Advanced Operation Entry Point
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////


void COperationControls::HandleWorkspaceChanged()
{
    mWDataset = nullptr;
    mWOperation = nullptr;
    mWDisplay = nullptr;
    mWFormula = nullptr;

	delete mProduct;
	mProduct = nullptr;

	// - I. Select NO operation 
	//		- this does not necessarily clear the datasets dependent widgets, but 
	//			- if changing to no workspace, everything is disabled and tab hidden behind others
	//			- if changing to another workspace, FillDatasets_Advanced and FillCombo( mOperationsCombo ...) setup everything
	//
	HandleWorkspaceChanged_Quick();				//reset quick page
	int index = mOperationsCombo->currentIndex();
	mOperationsCombo->clear();					// if index != -1, triggers HandleSelectedOperationChanged(-1 ) => SelectDataset( nullptr )
	if ( index < 0 )							
		HandleSelectedOperationChanged( -1 );	//force call to HandleSelectedOperationChanged(-1)


	// - II. Set core domain variables
	//
    mWRoot = mModel.RootWorkspace();
    if ( mWRoot )
	{
		mWDataset = mModel.Workspace< CWorkspaceDataset >();
		mWOperation = mModel.Workspace< CWorkspaceOperation >();
		mWDisplay = mModel.Workspace< CWorkspaceDisplay >();
		mWFormula = mModel.Workspace< CWorkspaceFormula >();		assert__( mWDataset && mWOperation && mWDisplay && mWFormula );
	}

	////////////////////////////////////////////////////////////////////
	// - III. Setup everything according to domain variables
	////////////////////////////////////////////////////////////////////

	// - No workspace? Disable everything
	//
    setEnabled( mWRoot != nullptr );

	// - III.A. Setup quick before advanced
	//
	HandleWorkspaceChanged_Quick();


	// - IV Setup advanced (datasets first, operations (and Formulas) last)

	// - IV.A. Datasets
	//
	FillDatasets_Advanced( mWRoot ? 0 : -1 );	//disables tab if !mWDataset || !mWDataset->GetDatasetCount() > 0 

	// - If no workspace, no operations or formulas, done
	//
	if ( !mWRoot )
	{
		return;
	}

	// - IV.B. Formulas	(no particular widgets involved)
	
	mWFormula->GetFormulaNames( mMapFormulaString );

	
	// - IV.C. Operations: fill (advanced) operation combo, selecting 1st operation
	//
    FillCombo( mOperationsCombo, *mWOperation->GetOperations(),

        0, true,

        []( const CObMap::value_type &i ) -> const char*
        {
            return i.first.c_str();
        }
    );
}


// A dataset change is almost like a workspace change; this is a slot answering
//	dataset signals
//
// This function is roughly equivalent to item IV in advanced entry 
//	point, when a workspace exists. So, it should be able to completely 
//	reset the tab
//
// NOTE connected by main window to datasets panel
//
void COperationControls::HandleDatasetsChanged_Advanced( CDataset *dataset )
{
    Q_UNUSED( dataset );

	FillDatasets_Advanced( mAdvancedDatasetsCombo->currentIndex() );

	if ( mCurrentOperation )
	{
		HandleSelectedOperationChanged( mOperationsCombo->currentIndex() );
	}
}


// for enabling / disabling widgets dependent of multiple state changes:
//
//	- operation existence
//	- formula existence
//	- operation file existence
//
void COperationControls::UpdateGUIState()
{
	mExportOperationAction->setEnabled( mCurrentOperation && !IsQuickOperationSelected() && mCurrentOperation->HasFormula() );
	mEditExportAsciiAction->setEnabled( mCurrentOperation && !IsQuickOperationSelected() && IsFile( mCurrentOperation->GetExportAsciiOutputPath() ) );
	mCreateExportedDisplaysAction->setEnabled( mCurrentOperation && !IsQuickOperationSelected() && IsFile( mCurrentOperation->GetOutputPath() ) );
	mOperationExportButton->setEnabled( mEditExportAsciiAction->isEnabled() || mExportOperationAction->isEnabled() || mCreateExportedDisplaysAction->isEnabled() );
}


// Assuming datasets setup in combo, this function should be enough to setup remaining tab
//
void COperationControls::HandleSelectedOperationChanged( int operation_index )	//ComboOperation( int currentOperation )
{
	// I. Enable/disable items independent of quick operation

	mSplitPlotsButton->setEnabled( operation_index >= 0 );

	mOperationExportButton->setEnabled( operation_index >= 0 );
	mOperationStatisticsButton->setEnabled( operation_index >= 0 );

	mDuplicateOperationButton->setEnabled( operation_index >= 0 );


	// II. Enable/disable items that can(not) relate with quick operation

	const bool quick_operation_selected = IsQuickOperationSelected();

	//the following 3 widgets are also controlled by quick operation code in HandleVariableStateChanged_Quick
	//
	mAdvancedExecuteButton->setEnabled( operation_index >= 0 && ( !quick_operation_selected || mCanExecuteQuickOperation ) );
	mSwitchToMapButton->setEnabled( operation_index >= 0 && ( !quick_operation_selected || mCanExecuteQuickOperation ) );
	mSwitchToPlotButton->setEnabled( operation_index >= 0 && ( !quick_operation_selected || mCanExecuteQuickOperation ) );

	mDeleteOperationButton->setEnabled( operation_index >= 0 && !quick_operation_selected );
	mRenameOperationButton->setEnabled( operation_index >= 0 && !quick_operation_selected );
	mOperationFilterButton_Advanced->setEnabled( operation_index >= 0 && !quick_operation_selected );
	mAdvancedDatasetsCombo->setEnabled( !quick_operation_selected );
	mDataExpressionsTree->setEnabled( operation_index >= 0 && !quick_operation_selected );
	mExpressionGroup->setEnabled( operation_index >= 0 && !quick_operation_selected );
	mSamplingGroup->setEnabled( operation_index >= 0 && !quick_operation_selected );


	// III. Setup operation: mCurrentOperation assignment, updates fields and expressions
	//
	//	- ensures a dataset is assigned, if datasets exist and mCurrentOperation is assigned

	// fields tree update

	if ( operation_index < 0 )
	{
		mCurrentOperation = nullptr;
		SelectDataset( nullptr );
	}
	else
	{
		// assign mCurrentOperation
		mCurrentOperation = mWOperation->GetOperation( q2a( mOperationsCombo->itemText( operation_index ) ) );		assert__( mCurrentOperation );
		const CDataset *dataset = mCurrentOperation->OriginalDataset();
		if ( dataset )
			SelectDataset( mCurrentOperation->OriginalDatasetName() );
		else
		if ( !quick_operation_selected )
		{
			auto new_dataset_name = q2a( mAdvancedDatasetsCombo->itemText( 0 ) );
			dataset = mWDataset->GetDataset( new_dataset_name );
			AssignDataset( dataset, false );
		}
	}

	// data expression tree update

	mDataExpressionsTree->InsertOperation( mCurrentOperation );
    bool can_be_map = mDataExpressionsTree->IsMap();
	if ( can_be_map )
		mSwitchToMapButton->setChecked( true );		//with auto-exclusive buttons, calling with false doesn't work
	else
		mSwitchToPlotButton->setChecked( true );	//idem

	//update filters

	ResetFilterSelection();

	UpdateGUIState();
}


bool COperationControls::SelectDataset( const std::string &dataset_name )
{
	return SelectDataset( mWDataset->GetDataset( dataset_name ) );
}
// Supports null dataset and mCurrentOperation; takes care dataset selection, without 
//	(persistent) domain assignments (assigns operation product)
//
// Assigns mCurrentOriginalDataset (with dataset parameter) and mProduct (only domain 
//	change, not persistent)
// Updates mAdvancedFieldsTree
//
// If dataset is not null
//	- Selects it in GUI
//	- Opens product (independently of mCurrentOperation assigned;
//		-  uses a filtered dataset if mCurrentOperation assigned
//
// If mCurrentOperation is not null
//	- Selects record (from operation) in fields tree and assigns selected record from fields tree to 
//		operation (apparently this corresponds to assigning the 1st one to operation if it has none assigned
//	
bool COperationControls::SelectDataset( const CDataset *dataset )
{
	// Assign mCurrentOriginalDataset
	//
	mCurrentOriginalDataset = dataset;
	delete mProduct;
	mProduct = nullptr;

	bool result = true;

	// Select dataset in GUI and assign mProduct
	//
	if ( dataset )
	{
		// Select dataset in GUI
		//
		int dataset_index = mAdvancedDatasetsCombo->findText( dataset->GetName().c_str() );
		if ( mAdvancedDatasetsCombo->currentIndex() != dataset_index )
		{
			mAdvancedDatasetsCombo->blockSignals( true );
			mAdvancedDatasetsCombo->setCurrentIndex( dataset_index );
			mAdvancedDatasetsCombo->blockSignals( false );
		}

		// Assign mProduct and mCurrentOperation->m_product, if mCurrentOperation assigned
		//
		std::string error_msg;
		try
		{
			const CDataset *filtered_dataset = mCurrentOperation ? const_cast<const COperation*>( mCurrentOperation )->Dataset() : mCurrentOriginalDataset;
			mProduct = filtered_dataset->OpenProduct();
			if ( mCurrentOperation )
				mCurrentOperation->SetProduct( mProduct );
		}
		catch ( CException& e )
		{
			error_msg = std::string( "Unable to process files.\nReason:\n" ) + e.what();
			mProduct = nullptr;
		}
		if ( mProduct == nullptr )
		{
			if ( !error_msg.empty() )
				SimpleErrorBox( error_msg );
			else
				LOG_WARN( "Unable to set Product.\nPerhaps dataset file list is empty or product file doesn't exist." );

			result = false;
		}
	}

	if ( mProduct != nullptr )
	{
		//TODO
		//m_productClass = m_product->GetProductClass().c_str();
		//m_productType = m_product->GetProductType().c_str();
	}

	// Update fields tree with new product
	//
	mAdvancedFieldsTree->InsertProduct( mProduct );

	if ( mCurrentOperation )
	{
		mAdvancedFieldsTree->SelectRecord( mCurrentOperation->GetRecord() );
		//SetCurrentRecord();
		mCurrentOperation->SetRecord( mAdvancedFieldsTree->GetCurrentRecord() );
	}

	return result;
}


// - Tries to assign dataset to operation if mCurrentOperation assigned
//		clearing operation dataset and product if null dataset
// - Tries to open dataset product
// - Keeps operation and (filtered) dataset association intact if anything above fails
// - Makes SelectDataset( dataset ) if succeeds
//
bool COperationControls::AssignDataset( const CDataset *dataset, bool changing_used_dataset )
{
	CProduct *new_product = nullptr;

	// Try to assign new dataset
	//
	if ( !dataset )
	{
		if ( mCurrentOperation )
		{
			mCurrentOperation->SetDataset( nullptr );
			mCurrentOperation->SetProduct( nullptr );
		}
	}
	else
	{
		const CDataset *filtered_dataset = dataset;
		const CDataset *saved_filtered_dataset = nullptr;
		if ( mCurrentOperation )
		{
			const CDataset *op_filtered_dataset = const_cast<const COperation*>( mCurrentOperation )->Dataset();
			saved_filtered_dataset = op_filtered_dataset ? new CDataset( *op_filtered_dataset ) : nullptr;
			mCurrentOperation->SetDataset( dataset );
            filtered_dataset = const_cast<const COperation*>( mCurrentOperation )->Dataset();
		}

		std::string error_msg;
		try
		{
			new_product = filtered_dataset->OpenProduct();
		}
		catch ( CException& e )
		{
			error_msg = std::string( "Unable to process files.\nReason:\n" ) + e.what();
			new_product = nullptr;
		}

		if ( new_product == nullptr )
		{
			if ( !error_msg.empty() )
				SimpleErrorBox( error_msg );
			else
				SimpleErrorBox( "Unable to set Product\nPerhaps dataset file list is empty or product file doesn't exist." );

			if ( mCurrentOperation )
			{
				mCurrentOperation->SetDataset( saved_filtered_dataset );
			}

			return false;
		}

		if ( changing_used_dataset && !CDataExpressionsTreeWidget::SelectRecord( this, mCurrentOperation, new_product ) )
		{
			SimpleErrorBox( "You have not selected a record name.\nDataset has not been changed.\nChanging operation dataset is canceled." );
			mCurrentOperation->SetDataset( saved_filtered_dataset );
			delete new_product;
			return false;
		}
	}

	delete new_product;

	// At this point, assume dataset assignment was OK. Update GUI and internal state

	SelectDataset( dataset );	//	mCurrentOriginalDataset = dataset;

	//TODO what is the meaning of the following lines ???
	//
	//wxTreeItemId rootIdFields = GetFieldstreectrl()->GetRootItem();
	//int nRecords = ( rootIdFields.IsOk() ) ? GetFieldstreectrl()->GetChildrenCount( rootIdFields, false ) : 0;
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
//if (!mCurrentOriginalDataset )
//	mCurrentOperation->SetDataset( nullptr );
//mCurrentOperation->SetProduct( mProduct );

	//////////////////////////////////////////////////////////////////SetCurrentDataset(); end

	//// v3 note If there is only one record in the dataset ==> select it
	//GetOperationRecord();		sets GetFieldstreectrl ("Fields" tree) with mCurrentOperation->GetRecord();
//mAdvancedFieldsTree->SelectRecord( mCurrentOperation->GetRecord() );
////SetCurrentRecord();
//mCurrentOperation->SetRecord( mAdvancedFieldsTree->GetCurrentRecord() );	

	//SetDataSetAndRecordLabel();

	//EnableCtrl();

	return true;
}


//	slot
//	If index is invalid, calls AssignDataset with null
//	If index is valid
//		- asks user confirmation if the operation assigned dataset is changing, and clears op record if yes
//		- retrieves dataset dataset_index form respective workspace, calls AssignDataset with it
//
//	Clears op formulas
//	At the end, updates mDataExpressionsTree
//
void COperationControls::HandleSelectedDatasetChanged_Advanced( int dataset_index )		   //OnDatasetSelChanging( wxTreeEvent &event )
{
	//lambdas

	auto cancel_dataset_change = [this]()
	{
		if ( mCurrentOperation )
			SelectOperationDataset( mCurrentOperation, mAdvancedDatasetsCombo, true );
	};


	//function body 

	CDataset *new_dataset = nullptr;
	bool changing_used_dataset = false;
	if ( dataset_index >= 0 )
	{
		auto new_dataset_name = q2a( mAdvancedDatasetsCombo->itemText( dataset_index ) );
		if ( mCurrentOperation )
		{
            std::string current_dataset_name = mCurrentOperation->OriginalDatasetName();						//TODO why cannot assert__( !mCurrentOperation->HasFormula() || !current_dataset_name.empty() );
			changing_used_dataset = mCurrentOperation->HasFormula() && new_dataset_name != current_dataset_name;
		}
		if ( changing_used_dataset )
		{
			std::string question =
				"Some data expressions are built from '"
				+ mCurrentOperation->OriginalDatasetName()
				+ "' dataset.\nDo you still want to change to '"
				+ new_dataset_name
				+ "' dataset ?";

			if ( !SimpleQuestion( question ) )
			{
				cancel_dataset_change();
				return;
			}

			mCurrentOperation->SetRecord( "" );
		}

		new_dataset = mWDataset->GetDataset( new_dataset_name );	//do not return if dataset is the same: content may have changed
	}

	if ( !AssignDataset( new_dataset, changing_used_dataset ) )
	{
		cancel_dataset_change();
	}
	else
	if ( mCurrentOperation )
	{
		RemoveOperationFormulas( mCurrentOperation );
	}

	mDataExpressionsTree->InsertOperation( mCurrentOperation );
}


// Selects data computation mode in GUI
//
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
	}											assert__( mUserFormula == formula);

	// if same pointer
	if ( mCurrentOperation->GetSelect() == formula )		//IsCriteriaSelection( formula )
	{
		return;
	}

	auto *action = mDataComputationGroup->findChild<QAction*>( QString( formula->GetDataModeAsString().c_str() ) );
	if ( action )
    {
		action->setChecked( true );
        mDataComputation->setText( action->text() );
    }
    else
        mDataComputation->setText( "" );
}

// Assigns data computation mode to mUserFormula using caller (action) id
// At the end calls SelectDataComputationMode (selects data computation mode in GUI)
//
void COperationControls::HandleDataComputation()
{
	assert__( mCurrentOperation && mUserFormula && mUserFormula == mDataExpressionsTree->SelectedFormula() );

	auto a = qobject_cast<QAction*>( sender() );	assert__( a );

	//CFormula *formula = mUserFormula;	// == GetCurrentFormula();	== GetOperationtreectrl()->GetCurrentFormula();
	if ( mUserFormula == nullptr )
	{
		return;
	}

	// if same pointer
	if ( mCurrentOperation->GetSelect() == mUserFormula )	//== IsCriteriaSelection( formula ) in old OperationPanel
	{
		return;
	}

	auto id   = CMapDataMode::GetInstance().NameToId( q2a( a->text() ) );
	mUserFormula->SetDataMode( id );

	SelectDataComputationMode();
}



// Selects data smoothing mode in GUI
//
void COperationControls::SelectDataSmoothingMode()		//from COperationPanel::GetDataMode()
{
    if ( mCurrentOperation == nullptr )
    {
        return;
    }

    CFormula* formula = mDataExpressionsTree->SelectedFormula();
    if ( formula == nullptr )
    {
        return;
    }											assert__( mUserFormula == formula);

    // if same pointer
    if ( mCurrentOperation->GetSelect() == formula )		//IsCriteriaSelection( formula )
    {
        return;
    }

    auto *action = mDataSmoothingGroup->findChild<QAction*>( QString( formula->GetFilterAsString().c_str() ) );
    if ( action )
    {
        action->setChecked( true );
        mDataSmoothing->setText( action->text() );
    }
    else
        mDataSmoothing->setText( "" );
}



// Assigns data smoothing mode to mUserFormula using caller (action) id
// At the end calls SelectDataSmoothingMode (selects data smoothing mode in GUI)
//
void COperationControls::HandleDataSmoothing()
{
    assert__( mCurrentOperation && mUserFormula && mUserFormula == mDataExpressionsTree->SelectedFormula() );

    auto a = qobject_cast<QAction*>( sender() );	assert__( a );

    //CFormula *formula = mUserFormula;	// == GetCurrentFormula();	== GetOperationtreectrl()->GetCurrentFormula();
    if ( mUserFormula == nullptr )
    {
        return;
    }

    // if same pointer
    if ( mCurrentOperation->GetSelect() == mUserFormula )	//== IsCriteriaSelection( formula ) in old OperationPanel
    {
        return;
    }

    auto id   = CMapTypeFilter::GetInstance().NameToId( q2a( a->text() ) );
    mUserFormula->SetFilter( id );

    SelectDataSmoothingMode();

    // To update Loess Cut off widgets
    // Check if has smoothing
    bool hasSmoothing = mCurrentOperation->HasV3Filters();
    mXLonCutOff->setEnabled( hasSmoothing );
    mYLatCutOff->setEnabled( hasSmoothing );
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    SAMPLING METHODS
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// At old Brat -> CResolutionDlg::Init().
void COperationControls::UpdateSamplingGroup()
{
    // Clear sampling widgets
    mXLonLabel->clear();
    mYLatLabel->clear();
    mXLonMinValue->clear();
    mYLatMinValue->clear();
    mXLonMaxValue->clear();
    mYLatMaxValue->clear();
    mXLonStep->clear();
    mYLatStep->clear();
    mXLonCutOff->clear();
    mYLatCutOff->clear();
    mYLatIntervalsLabel->clear();
    mXLonIntervalsLabel->clear();

	mSamplingGroup->setEnabled( false );
    mXLonCutOff->setEnabled( false );
    mYLatCutOff->setEnabled( false );

    if ( !mCurrentOperation || !mCurrentOperation->IsZFXY() )
		return;

    // Is ZFXY ///
    mXformula = mCurrentOperation->GetFormula( CMapTypeField::eTypeOpAsX );
    mYformula = mCurrentOperation->GetFormula( CMapTypeField::eTypeOpAsY );

    if ( mXformula == nullptr || mYformula == nullptr )
        return;

    mSamplingGroup->setEnabled( true );

    // Check if has smoothing
    bool hasSmoothing = mCurrentOperation->HasV3Filters();
    mXLonCutOff->setEnabled( hasSmoothing );
    mYLatCutOff->setEnabled( hasSmoothing );

    ////////////////////////////////////////////////////////////
    ///  Initialize values on text box, labels and tooltips  ///
    ////////////////////////////////////////////////////////////

    ///////////////////
    ///  X Formula  ///
    ///////////////////
    if ( mXformula )
	{
        QString xFormula_name = mXformula->GetName().c_str();
        QString xFormula_units = mXformula->GetUnitAsText().c_str();

        // X Label //
        if ( mXformula->IsLonDataType() && mYformula->IsLatDataType() )
            mXLonLabel->setText( LonResolution_labelFormat.arg( xFormula_name, xFormula_units ) );
        else
            mXLonLabel->setText( Xresolution_labelFormat.arg( xFormula_name, xFormula_units ) );

        // Min/Max X //
        if ( mXformula->IsTimeDataType() )
        {
            mXLonMinValue->setText( mXformula->GetMinValueAsDateString().c_str() );
            mXLonMaxValue->setText( mXformula->GetMaxValueAsDateString().c_str() );
        }
        else
        {
            mXLonMinValue->setText( mXformula->GetMinAsString().c_str() );
            mXLonMaxValue->setText( mXformula->GetMaxAsString().c_str() );
        }

        // Set Step and Interval values //
        mXLonStep->setText( mXformula->GetStep().c_str() );
        mXLonIntervalsLabel->setText( mXformula->GetIntervalAsText().c_str() );

        // Set ToolTips //
        mXLonMinValue->setToolTip( "Minimun value for '" + xFormula_name + "' field." );
        mXLonMaxValue->setToolTip( "Maximum value for '" + xFormula_name + "' field." );
        mGetDataMaxMinX->setToolTip( DataMaxMin_labelFormat.arg( xFormula_name ) );
        mXLonStep->setToolTip( "Step between consecutive values for '" + xFormula_name +  "' field.\n" +
                               "'Step' is correlated with 'Number of intervals'." );
        mXLonIntervalsLabel->setToolTip( "Number of intervals between min. and max value for '" + xFormula_name +
                                         "' field.\n'Number of intervals' is correlate with 'Step',"  );

        // Smoothing //
        if ( hasSmoothing )
        {
            mXLonCutOff->setText( isDefaultValue(mXformula->GetLoessCutOff()) ? "" : n2q(mXformula->GetLoessCutOff()) );
            mXLonCutOff->setToolTip( "Distance (in dots) for '" + xFormula_name +
                                     "' field where LOESS smoothing reaches 0 along X axis.\n" +
                                     "Must be an odd integer. If 1 or 0, Distance computation is disabled." );
        }
    }

    ///////////////////
    ///  Y Formula  ///
    ///////////////////
    if ( mYformula )
	{
        QString yFormula_name = mYformula->GetName().c_str();
        QString yFormula_units = mYformula->GetUnitAsText().c_str();

        // Y Label //
        if ( mYformula->IsLatDataType() && mXformula->IsLonDataType() )
            mYLatLabel->setText( LatResolution_labelFormat.arg( yFormula_name, yFormula_units ) );
        else
            mYLatLabel->setText( Yresolution_labelFormat.arg( yFormula_name, yFormula_units ) );

        // Min/Max Y //
        if ( mYformula->IsTimeDataType() )
        {
            mYLatMinValue->setText( mYformula->GetMinValueAsDateString().c_str() );
            mYLatMaxValue->setText( mYformula->GetMaxValueAsDateString().c_str() );
        }
        else
        {
            mYLatMinValue->setText( mYformula->GetMinAsString().c_str() );
            mYLatMaxValue->setText( mYformula->GetMaxAsString().c_str() );
        }

        // Set Step and Interval values //
        mYLatStep->setText( mYformula->GetStep().c_str() );
        mYLatIntervalsLabel->setText( mYformula->GetIntervalAsText().c_str() );

        // Set ToolTips //
        mYLatMinValue->setToolTip( "Minimun value for '" + yFormula_name + "' field." );
        mYLatMaxValue->setToolTip( "Maximum value for '" + yFormula_name + "' field." );
        mGetDataMaxMinY->setToolTip( DataMaxMin_labelFormat.arg( yFormula_name ) );
        mYLatStep->setToolTip( "Step between consecutive values for '" + yFormula_name +  "' field.\n" +
                               "'Step' is correlated with 'Number of intervals'." );
        mYLatIntervalsLabel->setToolTip( "Number of intervals between min. and max value for '" + yFormula_name +
                                         "' field.\n'Number of intervals' is correlated with 'Step',"  );

        // Smoothing //
        if ( hasSmoothing )
        {
            mYLatCutOff->setText( isDefaultValue(mYformula->GetLoessCutOff()) ? "" : n2q(mYformula->GetLoessCutOff()) );
            mYLatCutOff->setToolTip( "Distance (in dots) for '" + yFormula_name +
                                     "' field where LOESS smoothing reaches 0 along Y axis.\n" +
                                     "Must be an odd integer. If 1 or 0, Distance computation is disabled." );
        }
	}

    ///////////////////////////////////////////////////////
    /// Verify values, compute interval, update mformula //
    ValidateData();

    ///////////////////
    /// IMPROVEMENT ///
    // Case X or Y are TimeDateType, automatically sets the min/max data coverage of the dataset
    // Otherwise the user may create, by default, a operation with a huge time range from 2000 to current date...
    if ( mXformula->IsTimeDataType() )
        GetDataMinMax( mXformula );
    if ( mYformula->IsTimeDataType() )
        GetDataMinMax( mYformula );

}


// At old Brat -> CResolutionDlg::GetMinmax(CFormula* formula).
void COperationControls::GetDataMinMax(CFormula* formula)
{
    if ( formula == nullptr )
        return;

    CProduct* product = mCurrentOperation->GetProduct();
    if ( product == nullptr )
        return;

    WaitCursor wait;

    ///////////////////////////////
    /// Get product temp object  //
    ///////////////////////////////
    // Don't use product of operation object to read data
    // because reading reload fields info and then we get bad pointer
    // So, we use a CProduct temporary object (Old Brat comment)
    CProduct* productTmp = nullptr;

    QString errorMsg;
    bool is_Ok = true;
    try
    {
        /// RCCC TODO: Try to merge the following 2 lines (dummy)
        const COperation *currentOperation = mCurrentOperation;
        CProductList *currentProductList   = const_cast<CProductList*>( currentOperation->Dataset()->GetProductList() );

        productTmp = CProduct::Construct( *currentProductList );
    }
    catch ( CException &e )
    {
        errorMsg = e.what();
        is_Ok    = false;
    }
    catch ( std::exception &e )
    {
        errorMsg = e.what();
        is_Ok    = false;
    }
    catch ( ... )
    {
        errorMsg = " Unexpected error while getting product (runtime error).";
        is_Ok    = false;
    }

    if ( !is_Ok )
    {
        wait.Restore(); // restore wait cursor
        SimpleWarnBox( QString("Unable to get min/max values - Reason:\n%1").arg( errorMsg ) );
        return;
    }

    ///////////////////////////////
    /// Get product Min and Max  //
    ///////////////////////////////
    bool resultGetCoverage = false;
    try
    {
        if ( formula->IsTimeDataType() )  // Date time type
        {
            CDatePeriod datePeriod;
            resultGetCoverage = productTmp->GetDateMinMax( datePeriod );

            if ( formula->IsXType() )  // X formula
            {
                mXLonMinValue->setText( datePeriod.GetFrom().AsString().c_str() );
                mXLonMaxValue->setText( datePeriod.GetTo().AsString().c_str() );
            }
            else if ( formula->IsYType() )  // Y formula
            {
                mYLatMinValue->setText( datePeriod.GetFrom().AsString().c_str() );
                mYLatMaxValue->setText( datePeriod.GetTo().AsString().c_str() );
            }
        }
        else if ( formula->IsLatLonDataType() )  // Latitude/Longitude type
        {
            double latMin, latMax, lonMin, lonMax;
            resultGetCoverage = productTmp->GetLatLonMinMax( latMin, lonMin, latMax, lonMax );

            latMin = formula->ConvertToFormulaUnit( latMin );
            latMax = formula->ConvertToFormulaUnit( latMax );
            lonMin = formula->ConvertToFormulaUnit( lonMin );
            lonMax = formula->ConvertToFormulaUnit( lonMax );

            double valueMin = 0.0;
            double valueMax = 0.0;

            if ( formula->IsLonDataType() ) // Is Longitude
            {
                valueMin = lonMin;
                valueMax = lonMax;
            }
            else                            // Is Latitude
            {
                if ( !isDefaultValue( productTmp->GetForceLatMinCriteriaValue() ) ) // Has Min Lat (Ex: Jason2 Min Lat=-67)
                {
                    CExpression expr = std::string( formula->GetDescription( true,
                                                                             &mMapFormulaString,
                                                                             productTmp->GetAliasesAsString() ).c_str() );
                    productTmp->GetValueMinMax( expr,
                                                (const char *)mCurrentOperation->GetRecord().c_str(),
                                                valueMin,
                                                valueMax,
                                                *(formula->GetUnit() ));
                }
                else
                {
                    valueMin = latMin;
                    valueMax = latMax;
                }
            }
            if (formula->IsXType())
            {
                mXLonMinValue->setText( n2q(valueMin) );
                mXLonMaxValue->setText( n2q(valueMax) );
            }
            else if (formula->IsYType())
            {
                mYLatMinValue->setText( n2q(valueMin) );
                mYLatMaxValue->setText( n2q(valueMax) );
            }
        }

        if ( !resultGetCoverage ) // If failed in determining Max/min date or Lat/Lon or field has other type of info
        {
            double valueMin = 0.0;
            double valueMax = 0.0;
            CExpression expr = std::string( formula->GetDescription( true,
                                                                     &mMapFormulaString,
                                                                     productTmp->GetAliasesAsString()  ).c_str() );
            if ( formula->IsTimeDataType() )
            {
                CUnit unit;
                productTmp->GetValueMinMax( expr,
                                            (const char *)mCurrentOperation->GetRecord().c_str(),
                                            valueMin,
                                            valueMax,
                                            unit);

                CDatePeriod datePeriod( valueMin, valueMax );

                if ( formula->IsXType() )
                {
                    mXLonMinValue->setText( datePeriod.GetFrom().AsString().c_str() );
                    mXLonMaxValue->setText( datePeriod.GetTo().AsString().c_str()   );
                }
                else if ( formula->IsYType() )
                {
                    mYLatMinValue->setText( datePeriod.GetFrom().AsString().c_str() );
                    mYLatMaxValue->setText( datePeriod.GetTo().AsString().c_str() );
                }
            }
            else
            {
                productTmp->GetValueMinMax( expr,
                                            (const char *)mCurrentOperation->GetRecord().c_str(),
                                            valueMin,
                                            valueMax,
                                            *(formula->GetUnit()) );

                if ( formula->IsXType() )
                {
                    mXLonMinValue->setText( n2q(valueMin) );
                    mXLonMaxValue->setText( n2q(valueMax) );
                }
                else if ( formula->IsYType() )
                {
                    mYLatMinValue->setText( n2q(valueMin) );
                    mYLatMaxValue->setText( n2q(valueMax) );
                }
            }
        }
    }
    catch ( CException &e )
    {
        errorMsg = e.what();
        is_Ok    = false;
    }
    catch ( std::exception &e )
    {
        errorMsg = e.what();
        is_Ok    = false;
    }
    catch ( ... )
    {
        errorMsg = QString(" Unexpected error while getting product (runtime error).");
        is_Ok    = false;
    }

    if ( productTmp != nullptr )
    {
        delete productTmp;
        productTmp = nullptr;
    }

    if ( !is_Ok )
    {
        SimpleWarnBox( QString("Unable to get min/max values - Reason:\n%1").arg( errorMsg ) );
        return;
    }

    ///////////////////////////////////////////////////////
    /// Verify values, compute interval, update mformula //
    ///////////////////////////////////////////////////////
    ValidateData();
}


void COperationControls::ValidateData()
{
    VerifyMinMaxX();
    VerifyMinMaxY();
    ComputeXYInterval();
}


bool COperationControls::VerifyMinMaxX()
{
    if ( mXformula->IsTimeDataType() )
        return VerifyMinMaxAsDate( mXformula, mXLonMinValue, mXLonMaxValue );
    else
        return VerifyMinMax( mXformula, mXLonMinValue, mXLonMaxValue );
}

bool COperationControls::VerifyMinMaxY()
{
    if ( mYformula->IsTimeDataType() )
        return VerifyMinMaxAsDate( mYformula, mYLatMinValue, mYLatMaxValue );
    else
        return VerifyMinMax( mYformula, mYLatMinValue, mYLatMaxValue );
}


bool COperationControls::VerifyMinMaxAsDate( CFormula *formula, QLineEdit *LineEdit_Min, QLineEdit *LineEdit_Max )
{
    if ( mCurrentOperation == nullptr )
        return true;

    ////////////////////////////////////////////////////////////////////////
    /// GetCtrlMinMaxAsDate(formula, ctrlMin, ctrlMax) -> Old method BratV3
    ////////////////////////////////////////////////////////////////////////
    CProduct* product = mCurrentOperation->GetProduct();

    CDate dateMin, dateMax;
    double min = 0.0;
    double max = 0.0;

    dateMin.InitDateZero();

    if (product != nullptr)
    {
        dateMin.SetDate(0.0, product->GetRefDate());
    }

    dateMax.SetDateNow();

    GetValueAsDate( LineEdit_Min, min, dateMin.Value(), dateMin.Value(), dateMax.Value() );
    GetValueAsDate( LineEdit_Max, max, dateMax.Value(), dateMin.Value(), dateMax.Value() );

    // Set validated values into formula //
    formula->SetMinValue( min );
    formula->SetMaxValue( max );
    //////////////////////////////////////////////////

    if ( ( isDefaultValue(min) && isDefaultValue(max) ) )
        return true;

    if ( min >= max )
    {
        // Set default values into formula and LineEdit //
        formula->SetMinValue( dateMin.Value() );   LineEdit_Min->setText( dateMin.AsString( DateTimeFormat ).c_str() );
        formula->SetMaxValue( dateMax.Value() );   LineEdit_Max->setText( dateMax.AsString( DateTimeFormat ).c_str() );
        SimpleWarnBox( QString("'%1' minimum value must be strictly less than '%2' maximum value.").arg(
                                formula->GetName().c_str(),
                                formula->GetName().c_str() ) );
        return false;
    }

    return true;
}


void COperationControls::GetValueAsDate( QLineEdit *LineEdit_Axisvalue, double &seconds, double defValue, double min, double max )
{
    CDate dateTmp;

    //////////////////////////////////////////////////////////////////////////
    // GetValue(CDate& value, double defValue, double min, double max) -> Old BratV3
    ///////////////////////////////////////////////////////////////////////////
    try
    {
        QString str = LineEdit_Axisvalue->text();

        // ////////////////////////////////
        // Is Empty (value in line Edit) //
        // ////////////////////////////////
        if ( str.isEmpty() )
        {
            if ( isDefaultValue(defValue) )
                dateTmp.SetDefaultValue();
            else
                dateTmp = defValue;
        }
        else
        {
            int32_t result = dateTmp.SetDate( str.toStdString().c_str() );
            if ( result != BRATHL_SUCCESS )
                dateTmp.SetDefaultValue();

            // //////////////////////////////////
            // Is Default (value in line Edit) //
            // //////////////////////////////////
            if ( dateTmp.IsDefaultValue() && !isDefaultValue(defValue) )
                dateTmp = defValue;

            // /////////////////////////////////////////////////
            // Is outside Min/Max limits (value in line Edit) //
            // /////////////////////////////////////////////////
            if (  (!isDefaultValue(min) && dateTmp < min) ||
                  (!isDefaultValue(max) && dateTmp > max)    )
            {
                if ( isDefaultValue(defValue) )
                    dateTmp.SetDefaultValue();
                else
                    dateTmp = defValue;
            }
        }
    }
    catch (CException e)
    {
        dateTmp = defValue;
    }

    // Validated date is set on Line Edit and will be passed to formula object (in seconds)
    LineEdit_Axisvalue->setText( dateTmp.AsString( DateTimeFormat ).c_str() );
    seconds = dateTmp.Value();
}


bool COperationControls::VerifyMinMax( CFormula *formula, QLineEdit *LineEdit_Min, QLineEdit *LineEdit_Max )
{
    if ( mCurrentOperation == nullptr )
        return true;

    ////////////////////////////////////////////////////////////////////////
    /// GetCtrlMinMax(formula, ctrlMin, ctrlMax, minDefault, maxDefault) -> Old method BratV3
    ////////////////////////////////////////////////////////////////////////
    double minDefault = 0;
    double maxDefault = 0;
    double min = 0.0;
    double max = 0.0;

    if ( formula->IsLonDataType() )
    {
        formula->ConvertToFormulaUnit( -180, minDefault );
        formula->ConvertToFormulaUnit(  180, maxDefault );

        GetValue( LineEdit_Min, min, minDefault );
        GetValue( LineEdit_Max, max, maxDefault );
    }
    else if( formula->IsLatDataType() )
    {
        formula->ConvertToFormulaUnit( -90, minDefault );
        formula->ConvertToFormulaUnit(  90, maxDefault );

        GetValue( LineEdit_Min, min, minDefault, minDefault, maxDefault );
        GetValue( LineEdit_Max, max, maxDefault, minDefault, maxDefault );
    }
    else // Any other type besides Lon, Lat and time. Examples: alt, ssh...
    {
        setDefaultValue( minDefault );
        setDefaultValue( maxDefault );

        GetValue( LineEdit_Min, min, minDefault, minDefault, maxDefault );
        GetValue( LineEdit_Max, max, maxDefault, minDefault, maxDefault );

        if ( min == max ) // for variables than have only one value, min == max.
        {
            setDefaultValue( max );
            LineEdit_Max->setText( "" );
        }
    }

    // Set validated values into formula //
    formula->SetMinValue( min );
    formula->SetMaxValue( max );
    ///////////////////////////////////////////////////////////////////////

    if ( isDefaultValue(min) && isDefaultValue(max) )
        return true;

    // Chech if min>=max ////////////
    std::string errorMsg = "Resolution for '" + formula->GetName() + "': ";
    if ( !formula->CtrlMinMaxValue(errorMsg) )
    {
        // Set default values into formula and LineEdit //
        formula->SetMinValue( minDefault );
        formula->SetMaxValue( maxDefault );

        LineEdit_Min->setText( isDefaultValue(minDefault) ? "" : n2q(minDefault) );
        LineEdit_Max->setText( isDefaultValue(maxDefault) ? "" : n2q(maxDefault) );

        SimpleWarnBox( t2q(errorMsg) );
        return false;
    }

    return true;
}

void COperationControls::GetValue( QLineEdit *LineEdit_Axisvalue, double &value, double defValue, double min, double max )	//min = defaultValue<double>(), double max = defaultValue<double>() 
{
    CExpression expression;
    CExpressionValue exprValue;

    try
    {
        std::string str = q2a( LineEdit_Axisvalue->text() );

        // ////////////////////////////////
        // Is Empty (value in line Edit) //
        // ////////////////////////////////
        if ( str.empty() )
        {
            value = defValue;
            LineEdit_Axisvalue->setText( isDefaultValue(value) ? "" : n2q(value) );
            return;
        }

        if ( str.back() == '.' )
            str.append("0");

        expression.SetExpression( str );
        if ( expression.GetFieldNames()->size() != 0 )
            throw CException( "Fieldnames are not allowed for float parameter value", BRATHL_SYNTAX_ERROR );

        exprValue = expression.Execute();
        value = *( exprValue.GetValues() );

        // /////////////////////////////////////////////////
        // Is outside Min/Max limits (value in line Edit) //
        // /////////////////////////////////////////////////
        if ( (!isDefaultValue(min) && value < min) ||
             (!isDefaultValue(max) && value > max)    )
        {
            value = defValue;
            LineEdit_Axisvalue->setText( n2q(value) );
            return;
        }
    }
    catch (CException e)
    {
        value = defValue;
        LineEdit_Axisvalue->setText( isDefaultValue(value) ? "" : n2q(value) );
    }
}


void COperationControls::ComputeXYInterval()
{
    ComputeInterval( mXformula, mXLonIntervalsLabel, mXLonStep, XiconWarning);
    ComputeInterval( mYformula, mYLatIntervalsLabel, mYLatStep, YiconWarning);
}


bool COperationControls::ComputeInterval( CFormula *formula, QLabel *IntervalsLabel, QLineEdit* StepLineEdit, QLabel *IconWarning )
{
    // Compute interval of the formula
    std::string errorMsg;
    bool Ok = formula->ComputeInterval( errorMsg );
    //if ( !errorMsg.empty() && !Ok )
    //    SimpleWarnBox( errorMsg );

    // Update warning icon
    if ( Ok == false )
    {
        IconWarning->setPixmap( WarningResolutionIconPath );
        IconWarning->setToolTip( "Interval was round up or down\nto the nearest integer value." );
    }
    else
    {
        IconWarning->clear();
    }

    // Set number of Intervals
    int32_t interval = formula->GetInterval();
    if ( !isDefaultValue( interval ) )
    {
        IntervalsLabel->setText( n2q(interval) );
    }

    // Set step value
    std::string step = formula->GetStep();
    StepLineEdit->setText( t2q(step) );

    return Ok;
}


void COperationControls::HandleXLonMin_changed()
{
    VerifyMinMaxX();
    ComputeXYInterval();
}


void COperationControls::HandleXLonMax_changed()
{
    VerifyMinMaxX();
    ComputeXYInterval();
}

void COperationControls::HandleYLatMin_changed()
{
    VerifyMinMaxY();
    ComputeXYInterval();
}

void COperationControls::HandleYLatMax_changed()
{
    VerifyMinMaxY();
    ComputeXYInterval();
}


void COperationControls::HandleXLonStep_changed()
{
    QString step = mXLonStep->text();

    // Empty step or division by zero not allowed //
    static QRegExp re("[0-9]+[/][0.]+");
    if ( step.isEmpty() || re.exactMatch( step ) )
        step = t2q( mXformula->GetDefaultStep() );

    mXLonStep->setText( step );
    mXformula->SetStep( step.toStdString() );

    ComputeXYInterval();
}


void COperationControls::HandleYLatStep_changed()
{
    QString step = mYLatStep->text();

    // Empty step or division by zero not allowed //
    static QRegExp re("[0-9]+[/][0.]+");
    if ( step.isEmpty() || re.exactMatch( step ) )
        step = t2q( mYformula->GetDefaultStep() );

    mYLatStep->setText( step );
    mYformula->SetStep( step.toStdString() );

    ComputeXYInterval();
}


void COperationControls::HandleXLoessCut_changed()
{
    QString loess = mXLonCutOff->text();
    int loess_int = loess.toInt();

    if ( loess.isEmpty() )
    {
        mXformula->SetLoessCutOffDefault(); // distance computation disabled
    }
    else if ( loess_int % 2 == 0   ||   loess_int < 2 ) // Invalid! Loess_int is even or smaller than 2
    {
        mXLonCutOff->setText( "" );
        mXformula->SetLoessCutOffDefault();
        SimpleWarnBox( "Loess Cut-Off value must be an odd integer. If 0 or 1, distance computation is disabled." );
    }
    else // Valid! It's passed to formula
    {
        mXformula->SetLoessCutOff( loess_int );
    }
}


void COperationControls::HandleYLoessCut_changed()
{
    QString loess = mYLatCutOff->text();
    int loess_int = loess.toInt();

    if ( loess.isEmpty() )
    {
        mYformula->SetLoessCutOffDefault(); // distance computation disabled
    }
    else if ( loess_int % 2 == 0   ||   loess_int < 2 ) // Invalid! Loess_int is even or smaller than 2
    {
        mYLatCutOff->setText( "" );
        mYformula->SetLoessCutOffDefault();
        SimpleWarnBox( "Loess Cut-Off value must be an odd integer. If 0 or 1, distance computation is disabled." );
    }
    else // Valid! It's passed to formula
    {
        mYformula->SetLoessCutOff( loess_int );
    }
}


void COperationControls::HandleGetDataMinMaxX()
{
    // Sets min/max X data coverage
    GetDataMinMax( mXformula );
}

void COperationControls::HandleGetDataMinMaxY()
{
    // Sets min/max Y data coverage
    GetDataMinMax( mYformula );
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//						Field & Data Expressions
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


void COperationControls::HandleSelectedFieldChanged_Advanced()
{
	mAdvancedFieldDesc->setText( mAdvancedFieldsTree->GetCurrentFieldDescription().c_str() );
}


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
	//mExpressionGroup->setTitle( mUserFormula ? mUserFormula->GetName().c_str() : "" );
	mExpressionGroup->setTitle( mUserFormula ? mDataExpressionsTree->ParentItemTitle( formula ).c_str() : "No Expression Selected" );

    // Update Data computation and Smoothing parameters/buttons
    bool enable_data_Computation_Smooth = mUserFormula != nullptr &&
                                          mUserFormula->GetFieldType() == CMapTypeField::eTypeOpAsField;
    mDataComputation->setEnabled( enable_data_Computation_Smooth );
    mDataSmoothing->setEnabled(   enable_data_Computation_Smooth );
    SelectDataComputationMode();
    SelectDataSmoothingMode();

    // Update Sampling parameters/buttons
    bool enable_sampling =
		mUserFormula != nullptr &&
        //mUserFormula->GetFieldType() == CMapTypeField::eTypeOpAsField && // Enables only if Formula has a data field
        !COperation::IsSelect( mUserFormula->GetName() );

	mSamplingGroup->setEnabled( enable_sampling );
	mSamplingGroup->setCollapsed( mSamplingGroup->isCollapsed() || !enable_sampling );
	if ( enable_sampling )
		UpdateSamplingGroup();

	UpdateGUIState();
}


void COperationControls::HandleFormulaInserted( CFormula *formula )
{
    Q_UNUSED( formula );

	qDebug() << ( "Inserted formula " + ( formula ? formula->GetName() : "null" ) ).c_str();
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


void COperationControls::HandleCheckSyntax()
{
	assert__( mProduct );

	if ( mDataExpressionsTree->CheckSyntax( mProduct ) )
		LOG_WARN( "Data expression OK." );
}


void COperationControls::HandleShowInfo()
{
	assert__( mCurrentOperation && mUserFormula );
    
    CShowInfoDialog dlg( this, mUserFormula, mCurrentOperation );
	dlg.exec();
}

void COperationControls::HandleShowAliases()
{
	assert__( !mUserFormula || mUserFormula == mDataExpressionsTree->SelectedFormula() );

	mDataExpressionsTree->ShowAliases();
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

	// 1. create and insert operation

	std::string op_name = mWOperation->GetOperationNewName();
	if ( !mWOperation->InsertOperation( op_name ) )		//v4 this is weired: call to GetOperationNewName ensures not existing name
	{
		SimpleErrorBox( "Operation '" + op_name + "' already exists" );
		//GetOpnames()->SetStringSelection( op_name );
		//m_currentOperationIndex = GetOpnames()->GetSelection();

		//v3 continues in spite of warning
	}
	else
	{
		/////////////////////////////////////////////////////////////////////////////do this later mOperationsCombo->addItem( op_name.c_str() );
		//m_currentOperationIndex = GetOpnames()->Append( op_name );
		//GetOpnames()->SetSelection( m_currentOperationIndex );
	}


	// 2. Assign mCurrentOperation, init outputs, update data expressions tree

	//SetCurrentOperation();		//assigns   mCurrentOperation, makes GetOperationtreectrl()->Insert(mCurrentOperation);
	mCurrentOperation = mWOperation->GetOperation( op_name );
	mCurrentOperation->InitOutput( mWOperation );
	mCurrentOperation->InitExportAsciiOutput( mWOperation );
	mDataExpressionsTree->InsertOperation( mCurrentOperation );		//makes mExpressionTextWidget->setText( formula ? formula->GetDescription().c_str() : "" );

	
	// 3. Select data computation mode in GUI

	//GetDataMode();				//select value in GetOpDataMode combo
	SelectDataComputationMode();
    SelectDataSmoothingMode();

	//InitOperationOutputs();		//makes mCurrentOperation->InitOutput and mCurrentOperation->InitExportAsciiOutput
	//GetOperationOutputs();		//repeats calls above in case mCurrentOperation output strings are empty (but WHY? if the repetition fills them the same way??)
	//see above

	//GetOperationDataset();		//selects in datasets widget the operation dataset, if any
	//In fact, for new operations this does NOTHING in v3


	// 4. Select X in expression tree

	//GetOperationtreectrl()->SelectItem( GetOperationtreectrl()->GetXRootId() );		//selects the X root in the "Data expressions" tree widget
	mDataExpressionsTree->SelectX();

	//SetCurrentFormula();		//fills formula related fields: "Expression", "Unit" list, GetOptype combo, GetOpDataMode list, labels below "Expression" (SetResolutionLabels); see SetCurrentFormula
	//auto *formula = mDataExpressionsTree->SelectedFormula();
	//mExpressionTextWidget->setText( formula ? formula->GetDescription().c_str() : "" );	see above

	// 5. Assigns selected dataset and mProduct

	//SetCurrentDataset();		//sets the (real) operation dataset and formula
	mCurrentOperation->SetDataset( dataset );
	mCurrentOperation->SetProduct( mProduct );

	// 6. Add new operation to GUI lits and select it (possibly triggers all handling operation change sequence)

																			mOperationsCombo->addItem( op_name.c_str() );
																			mOperationsCombo->setCurrentIndex( mOperationsCombo->findText( op_name.c_str() ) );

	// 7. Select and re-assign operation record (v3 technique: amounts to update GUI with operation record and assign one to operation if none assigned)

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

//    mCurrentOperation = nullptr;
//    mUserFormula = nullptr;

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

    if ( !mWOperation->InsertOperation( op_name, mCurrentOperation, mWDataset ) )
    {
        SimpleMsgBox( "Operation '" + op_name + "' already exists" );
//        GetOpnames()->SetStringSelection(mCurrentOperation->GetName());
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

    auto result = ValidatedInputString( "Operation Name", mCurrentOperation->GetName(), "Rename Operation" );
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



void COperationControls::ExportGeoTIFFAsyncComputationFinished( int exit_code, QProcess::ExitStatus exitStatus, const COperation *operation )
{
	const bool success = 
		exit_code == 0 &&
		exitStatus == QProcess::NormalExit;

	if ( !success )
		return;

	LOG_INFO( "Asynchronous export GeoTIFF process for operation '" + operation->GetName() + "' finished execution." );
}

void COperationControls::ExportASCIIAsyncComputationFinished( int exit_code, QProcess::ExitStatus exitStatus, const COperation *operation )
{
	const bool success = 
		exit_code == 0 &&
		exitStatus == QProcess::NormalExit;

	if ( !success )
		return;

	LOG_INFO( "Asynchronous export ASCII process for operation '" + operation->GetName() + "' finished execution." );
}

/*
void COperationControls::ExportGeoTiff( std::string out_file, bool createKML, std::string colorTable, double rangeMin, double rangeMax )
{
	//wxGetApp().GotoLogPage();
	//file.Create( exportGeoTiffCmdFile.GetFullPath(), true, wxS_DEFAULT | wxS_IXUSR );

	std::string exportGeoTiffCmdFile = mWOperation->GetPath() + "/" + GetFilenameFromPath( out_file ) + EXPORTGEOTIFF_COMMANDFILE_EXTENSION;

	//std::string 
	//content = "\n#----- LOG -----\n\n";
	//content += ( kwVERBOSE + "=2\n" );
	//content += ( "\n#----- INPUT -----\n\n" );
	//content += ( kwFILE );
	//content += ( "=" );
	//content += ( mCurrentOperation->GetOutputPath() );
	//content += ( "\n" );
	//content += ( "\n#----- COLORTABLE -----\n\n" );
	//content += ( kwDISPLAY_COLORTABLE );
	//content += ( "=" );
	//content += ( colorTable );
	//content += ( "\n" );
	//if ( !isDefaultValue( rangeMin ) )
	//{
	//	content += ( kwDISPLAY_MINVALUE + "=" + n2s<std::string>( rangeMin ) + "\n" );
	//}
	//if ( !isDefaultValue( rangeMax ) )
	//{
	//	content += ( kwDISPLAY_MAXVALUE + "=" + n2s<std::string>( rangeMax ) + "\n" );
	//}
	//if ( createKML )
	//{
	//	std::string kmlOutputFile = out_file;
	//	SetFileExtension( out_file, "kml" );
	//	content += ( "\n#----- GOOGLE EARTH -----\n\n" );
	//	content += ( kwOUTPUT_KML );
	//	content += ( "=" );
	//	content += ( kmlOutputFile );
	//	content += ( "\n" );
	//	content += ( kwDISPLAY_LOGO_URL );
	//	content += ( "=" );
	//	content += ( mBratPaths.mInternalDataDir );
	//	content += ( "/BratLogo.png" );
	//	content += ( "\n" );
	//	content += ( kwFILETYPE );
	//	content += ( "=" );
	//	content += ( mProduct->GetProductClass().c_str() );
	//	content += ( " / " );
	//	content += ( mProduct->GetProductType().c_str() );
	//	content += ( "\n" );
	//	content += ( kwPRODUCT_LIST );
	//	content += ( "=" );
	//	content += ( mCurrentOperation->OriginalDataset()->GetProductList()->ToString( ", ", true ) );
	//	content += ( "\n" );
	//}
	//content += ( "\n#----- OUTPUT -----\n\n" );
	//content += ( kwOUTPUT );
	//content += ( "=" );
	//content += ( out_file );
	//content += ( "\n" );

	//std::string fullCommand = "\"" + COperation::GetExecExportGeoTiffName() + "\" \"" + exportGeoTiffCmdFile + "\"";

	//std::string outputFile_str = out_file;

	//CPipedProcess* process = new CPipedProcess( exportGeoTiffCmdFile.GetName(),
	//	wxGetApp().GetLogPanel(),
	//	fullCommand,
	//	wxGetApp().GetLogPanel()->GetLogMess(),
	//	&outputFile_str,
	//	mCurrentOperation->GetType() );

	//bool bOk = wxGetApp().GetLogPanel()->AddProcess( process );
	//if ( bOk == false )
	//{
	//	delete process;
	//	process = nullptr;
	//}

	//EnableCtrl();
}
*/


void COperationControls::HandleExportOperation()
{
	///////////////
	//lambdas
	///////////////

	auto DelayExportOperationAsGeoTiff = [this]( const QDateTime &at )
	{
		CBratTask *parent = Schedule( EExecutionType::eOperation, at );
		if ( parent == nullptr )
		{
			return;
		}
		Schedule( EExecutionType::eExportGeoTIFF, at, parent );
	};

	auto ExportOperationAsGeoTiff = [this]()
	{
		// v3: Always execute again
		if ( !ExecuteCurrentOperation( &COperationControls::OperationSyncExecutionFinished ) )
			return;

		Execute( EExecutionType::eExportGeoTIFF, mCurrentOperation, false, &COperationControls::ExportGeoTIFFAsyncComputationFinished );
	};


	auto DelayExportOperationAsNetCdf = [this]( const QDateTime &at )
	{
		CBratTask *parent = Schedule( EExecutionType::eOperation, at );
		if ( parent == nullptr )
		{
			return;
		}
		Schedule( EExecutionType::eOperation, at, parent );
	};

	auto ExportOperationAsNetCdf = [this]( const std::string &export_path )
	{
		if ( !ExecuteCurrentOperation( &COperationControls::OperationSyncExecutionFinished ) )
			return;

		const std::string output_path = mCurrentOperation->GetOutputPath( EExecutionType::eOperation );

		if ( !IsFile( output_path ) )
		{
			SimpleWarnBox(
				"File'"
				+ output_path
				+ "' doesn't exist - Please, look at the messages in the log panel"
				" and check if the operation has been correctly processed" );
		}
		else if ( !DuplicateFile( output_path, export_path ) )
		{
			SimpleWarnBox( "Unable to copy file '" + output_path + "' to '" + export_path + "'" );
		}
		else
		{
			SimpleMsgBox( "File has been exported to '" + export_path + "' " );
		}
	};


	auto DelayExportOperationAsAscii = [this]( const QDateTime &at )
	{
		CBratTask *parent = nullptr;

		// Exports operation with data computation, otherwise only dumps expressions
		if ( !mCurrentOperation->IsExportAsciiNoDataComputation() )
		{
			parent = Schedule( EExecutionType::eOperation, at );
			if ( parent == nullptr )
			{
				return;
			}
		}
		Schedule( EExecutionType::eExportASCII, at, parent );
	};

	auto ExportOperationAsAscii = [this]()
	{
		// Exports operation with data computation, otherwise only dumps expressions
		if ( !mCurrentOperation->IsExportAsciiNoDataComputation() )
		{
			if ( !ExecuteCurrentOperation( &COperationControls::OperationSyncExecutionFinished ) )
				return;

			if ( !IsFile( mCurrentOperation->GetOutputPath( EExecutionType::eOperation ) ) )
			{
				SimpleWarnBox(
					"File'"
					+ mCurrentOperation->GetOutputPath( EExecutionType::eOperation )
					+ "' doesn't exist - Please, look at the messages in the log panel"
					" and check if the operation has been correctly processed." );

				return;
			}
		}
		//ExportOperationAsAsciiDump == following
		Execute( EExecutionType::eExportASCII, mCurrentOperation, false, &COperationControls::ExportASCIIAsyncComputationFinished );
	};


	///////////////
	//function body
	///////////////

	assert__( mCurrentOperation && !IsQuickOperationSelected() );

    // Check if operation has errors
    std::string msg;
    bool operationOk = CheckOperation( mCurrentOperation, msg, &mMapFormulaString );
    if ( !operationOk )
    {
        SimpleErrorBox( "Operation '"
                        + mCurrentOperation->GetName()
                        + "' has some errors and process can't be achieved"
                        + ( msg.empty() ? "." : ":\n" )
                        + msg );
        return;
    }


    CExportDialog dlg( mModel.BratPaths().mInternalDataDir + "/BratLogo.png",  mWOperation, mCurrentOperation, &mMapFormulaString, this );
    if ( dlg.exec() == QDialog::Rejected )
        return;

 	switch ( dlg.ExportFormat() )
	{
		case CExportDialog::eASCII:
		{
			if ( dlg.DelayExecution() )
			{
				DelayExportOperationAsAscii( dlg.DateTime() );
			}
			else
			{
				ExportOperationAsAscii();
			}
		}
		break;


		case CExportDialog::eNETCDF:
		{
			if ( dlg.DelayExecution() )
			{
				DelayExportOperationAsNetCdf( dlg.DateTime() );
			}
			else
			{
				ExportOperationAsNetCdf( dlg.OutputPath() );
			}
		}
		break;


		case CExportDialog::eGEOTIFF:
		{
			if ( dlg.DelayExecution() )
			{
				DelayExportOperationAsGeoTiff( dlg.DateTime() );
			}
			else
			{
				ExportOperationAsGeoTiff();
			}
		}
		break;


		default:
			assert__( false );
		break;
	}
//    //------------------
//    // Remember last export filename and path -> OLD BRAT
//    //------------------
////    std::string filenameToSave = dlg.m_currentName;
////    filenameToSave.SetExt(CExportDlg::m_defaultExtensionAscii);
////    mCurrentOperation->SetExportAsciiOutput( filenameToSave.GetFullPath().ToStdString(), wxGetApp().GetCurrentWorkspaceOperation() );

////    mCurrentOperation->SetExecuteAgain(dlg.m_executeAgain);
////  //  mCurrentOperation->SetDelayExecution(dlg.m_delayExecution);


//    //------------------
//    // Export as Ascii
//    //------------------
//    if ( dlg.AsAscii() )
//    {
//        operation->SetExportAsciiDateAsPeriod( dlg.m_dateAsPeriod );
//        operation->SetExportAsciiExpandArray( dlg.m_expandArray );
//        operation->SetExportAsciiNoDataComputation( dlg.m_noDataComputation );
//        operation->SetExportAsciiNumberPrecision( dlg.m_asciiNumberPrecision );

//        if ( dlg.m_delayExecution )
//        {
//            //DelayExportOperationAsAscii( dlg.m_delayDlg );
//            BRAT_NOT_IMPLEMENTED;
//        }
//        else
//        {
//            ExportOperationAsAscii( operation );
//        }
//        return;
//    }


//    ///////////////// TO INTEGRATE /////////////////
////	operation->ClearLogFile();		//!sync
////	operation->InitShowStatsOutput( mWOperation );
//    if ( !operation->BuildShowStatsCmdFile( mWFormula, mWOperation ) )	//v3 didn't seem to care if this fails
//    {
//        SimpleErrorBox( "There was an error composing the command file.\nStatistics cannot be computed." );
//        return;
//    }

//    emit AsyncProcessExecution( true );
//    // ProcessesTable will display user messages for us, no need to report on false return
//    //
//    //bool result =
//    mProcessesTable->Add4Statistics( false, operation );
//    emit AsyncProcessExecution( false );
//    ///////////////// TO INTEGRATE /////////////////

//    //------------------
//    // Export as NetCdf
//    //------------------

//    //------------------
//    // Export as GeoTiff
//    //------------------

	UpdateGUIState();
}


void COperationControls::HandleEditExportAscii()
{
  //std::string title = std::string::Format("%s", mCurrentOperation->GetExportAsciiOutputName().c_str());
  //CRichTextFrame* frame = new CRichTextFrame(this, title);

  //wxFFile wxffile(mCurrentOperation->GetExportAsciiOutputName());

  //std::string content;
  //wxffile.ReadAll(&content);
  //frame->GetTextCtrl()->SetValue(content);
  ////frame->GetTextCtrl()->LoadFile(mCurrentOperation->GetExportAsciiOutputName());

  //frame->Show(true);
  //frame->GetTextCtrl()->SetInsertionPoint(0);

  //EnableCtrl();


	assert__( mCurrentOperation && !IsQuickOperationSelected() );

    CEditExportAsciiDialog dlg( mCurrentOperation, this );
    dlg.exec();
}


void COperationControls::HandleCreateExportedDisplays()
{
	assert__( mCurrentOperation && !IsQuickOperationSelected() );

	OperationSyncExecutionFinishedWithDisplay( 0, QProcess::NormalExit, mCurrentOperation );
}


void COperationControls::StatsAsyncComputationFinished( int exit_code, QProcess::ExitStatus exitStatus, const COperation *operation )
{
	const bool success = 
		exit_code == 0 &&
		exitStatus == QProcess::NormalExit;

	if ( !success )
		return;

	LOG_INFO( "Asynchronous statistics process for operation '" + operation->GetName() + "' finished execution." );
}
void COperationControls::HandleOperationStatistics()
{
	assert__( mCurrentOperation || mQuickOperation );

	Execute( EExecutionType::eStatistics, AdvancedMode() ? mCurrentOperation : mQuickOperation, false, &COperationControls::StatsAsyncComputationFinished );
}


void COperationControls::HandleDelayExecution()
{
	assert__( mCurrentOperation );

	QDateTime at = QDateTime::currentDateTime();
	std::string label = mCurrentOperation->GetTaskName( EExecutionType::eOperation );

    CDelayExecutionDialog dlg( label, at, this );
    if ( dlg.exec() == QDialog::Accepted )
    {
		mCurrentOperation->SetScheduledTaskName( label );
		Schedule( EExecutionType::eOperation, at );
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
	COsProcess *process = new COsProcess( false, "", this, mModel.BratPaths().mExecBratSchedulerName );
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
//							Operation Execution
/////////////////////////////////////////////////////////////////////////////////

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


bool COperationControls::MapRequested() const
{
    return mSwitchToMapButton->isChecked();
}


//slot
//
bool COperationControls::HandleExecute()
{
	if ( IsQuickOperationSelected() )
	{
		if ( MapRequested() )
			HandleQuickMap();
		else
			HandleQuickPlot();

		return true;
    }

	return ExecuteCurrentOperation();
}


//	Operation::Execute generates 
//		operation.par 
//		operation.nc
//
//	Display::Execute generates 
//		display.par
//
//

bool COperationControls::CreateOperationExecutionDisplays( std::string &to_display, int exit_code, QProcess::ExitStatus exitStatus, const COperation *operation )
{
	const bool success = 
		exit_code == 0 &&
		exitStatus == QProcess::NormalExit;

	if ( success )
	{
		assert__( operation == mCurrentOperation );		//should only fail if asynchronous process; requires refreshing all operations info

		auto displays = mModel.OperationDisplays( operation->GetName() );

		CMapDisplayData data_list;		//v3: dataList, pointer data member
		data_list.SetDelete( false );
		std::vector< CDisplay* > v = mWDisplay->CeateDisplays4Operation( mCurrentOperation, &data_list );
		if ( v.empty() )
			////display panel -> GetOperations();
			////display panel -> GetDispavailtreectrl()->InsertData( &m_dataList );
			//CMapDisplayData *m_dataList = new CMapDisplayData;
			//m_dataList->clear();
			//if ( !CreateDisplayData( mCurrentOperation, *m_dataList ) )	//CDisplay::GetDisplayType failed
		{
			SimpleErrorBox( "Could not retrieve operation output." );
		}
		else
		{
			for ( auto *display : displays )
			{
				display->UpdateDisplayData( &data_list, mWOperation );
				//if ( !display->AssignOperation( mCurrentOperation, true ) )
				//{
				//	SimpleErrorBox( "Could not retrieve operation output." );
				//	return;
				//}
			}
			//for ( auto &data : *m_dataList )
			//	display->InsertData( data.first, dynamic_cast<CDisplayData*>( data.second ) );

			to_display = v[ 0 ]->GetName();
		}
	}
	return success;
}


void COperationControls::OperationSyncExecutionFinished( int exit_code, QProcess::ExitStatus exitStatus, const COperation *operation )
{
	std::string to_display;
	if ( CreateOperationExecutionDisplays( to_display, exit_code, exitStatus, operation ) )
		LOG_INFO( "Execution of operation '" + operation->GetName() + "' finished." );
}


void COperationControls::OperationSyncExecutionFinishedWithDisplay( int exit_code, QProcess::ExitStatus exitStatus, const COperation *operation )
{
	std::string to_display;
	if ( CreateOperationExecutionDisplays( to_display, exit_code, exitStatus, operation ) )
		LaunchDisplay( to_display );
}


bool COperationControls::ExecuteCurrentOperation( post_execution_handler_t post_execution_handler )	//post_execution_handler = &COperationControls::OperationSyncExecutionFinishedWithDisplay
{
	assert__( mCurrentOperation );

	//v4
	if ( IsFile( mCurrentOperation->GetOutputPath( EExecutionType::eOperation ) ) )         //COperationPanel::RemoveOutput()
	{
		if ( !mCurrentOperation->RemoveOutput() )
		{
			SimpleWarnBox(
				"Unable to use file '"
				+ mCurrentOperation->GetOutputPath( EExecutionType::eOperation )
				+ "' \nIf it is opened by the operation in a display window, please close it and try again." );

			return false;
		}
	}

	bool result = Execute( COperation::eOperation, mCurrentOperation, true, post_execution_handler );

	UpdateGUIState();

	return result;
}




///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//				Generic External Process Execution for Operations
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


bool COperationControls::CheckOperation( COperation *operation, std::string& msg, const CStringMap* aliases )		//CtrlOperation
{
	return operation && operation->Control( mWFormula, msg, aliases );
}


void COperationControls::HandleProcessFinished( int exit_code, QProcess::ExitStatus exitStatus, bool sync, void *user_data )
{
	if ( sync )
		mSyncProcessExecuting = false;

	post_execution_handler_wrapper_t *handler = reinterpret_cast<post_execution_handler_wrapper_t*>( user_data );
	if ( handler && handler->post_execution_handler )
		( this->*(handler->post_execution_handler) )( exit_code, exitStatus, handler->operation );

	if ( sync )
		emit SyncProcessExecution( false );

	delete handler;
}


bool COperationControls::Execute( EExecutionType type, COperation *operation, bool sync, post_execution_handler_t post_execution_handler )
{
    //static int n = 0;

	// ProcessesTable will display user messages for us, no need to report on false return
	//
	//emit SyncProcessExecution( true );
	//mProcessesTable->Add( true, true, "brat-lab", "brat-lab", { "20", "ARG_0", "arg_1", "A_R_G" } );
	//emit SyncProcessExecution( false );
	//return;

	assert__( operation );

	//v4 checks 

    if ( MapRequested() )
    {
		if ( !operation->IsMap() )
		{
			SimpleErrorBox( "A map view was requested but the operation expressions are not suitable for a map.\n"
				"Map views require longitude, latitude and data fields." );
			return false;
		}
    }


	//v3 checks 

	std::string msg;
	bool operationOk = CheckOperation( operation, msg, &mMapFormulaString );
	if ( !operationOk )
	{
		SimpleWarnBox( 
			"Operation '"
			+ operation->GetName()
			+ "' has some errors and can't be executed"
			+ ( msg.empty() ? "." : ":\n" )
			+ msg );

		return false;
	}

	//wxGetApp().GotoLogPage();

	if ( sync )
	{
		operation->SetLogFile( mWOperation );
	}
	else
	{
		operation->ClearLogFile();
	}


	//BuildCmdFile(); == following 
	std::string error_msg;
	if ( !operation->BuildCmdFile( type, mWFormula, mWOperation, error_msg ) )	//v3 didn't seem to care if this fails
	{
		assert__( !error_msg.empty() );

		SimpleWarnBox( error_msg );
		return false;
	}

	/*
	CPipedProcess* process = new CPipedProcess( 

		operation->GetTaskName(),
		wxGetApp().GetLogPanel(),
		operation->GetFullCmd(),
		wxGetApp().GetLogPanel()->GetLogMess(),
		&operation->GetOutputPath(),				//used in remove file, must be complete path
		operation->GetType() 

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
		"A process for the operation '"
		+ operation->GetName()
		+ "' will be executed with the following command line\n\n"
		+ operation->GetFullCmd( type )
		+ "\n\nDo you want to proceed?" ) 
		)
		return false;


	// ProcessesTable will display user messages for us, no need to report on false return
	//
	emit sync ? SyncProcessExecution( true ) : AsyncProcessExecution( true );
	post_execution_handler_wrapper_t *handler = new post_execution_handler_wrapper_t{ operation, post_execution_handler };
	bool result = mProcessesTable->Add( true, handler, sync, false, operation->GetTaskName( type ), operation->GetFullCmd( type ) );
	mSyncProcessExecuting = sync && result;
	if ( !mSyncProcessExecuting )
		emit sync ? SyncProcessExecution( false ) : AsyncProcessExecution( false );
	else
	{
		if ( sync )
		{
			while ( mSyncProcessExecuting )
			{
				QBratThread::sleep( 1 );
				qApp->processEvents();
			}
		}
	}

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
		wxGetApp().GetLogPanel()->LogFile( std::string( operation->GetLogFile() ) );
	}


	BRAT_NOT_IMPLEMENTED
	*/

	return result;
}


CBratTask* COperationControls::Schedule( EExecutionType type, const QDateTime &at, CBratTask *parent )		//parent = nullptr 
{
	static auto *task_scheduler = CTasksProcessor::GetInstance();

	assert__( mCurrentOperation );

	const bool subordinated_task = parent != nullptr;
	const bool exporting_netcdf = subordinated_task && type == EExecutionType::eOperation;

	std::string msg;
	if ( !subordinated_task && !CheckOperation( mCurrentOperation, msg, &mMapFormulaString ) )
	{
		SimpleWarnBox( "Operation '" + mCurrentOperation->GetName() + "' has some errors and can't be execute:\n" + msg );
		return nullptr;
	}

	WaitCursor wait;

	//BuildCmdFile(); == following 
	std::string error_msg;
	if ( !exporting_netcdf && !mCurrentOperation->BuildCmdFile( type, mWFormula, mWOperation, error_msg ) )	//v3 didn't seem to care if this fails
	{
		assert__( !error_msg.empty() );

		wait.Restore();
		SimpleWarnBox( error_msg );
        return nullptr;
	}

	std::string task_label = mCurrentOperation->GetTaskName( type, true );		assert__( !task_label.empty() );

	// Save tasks uid because LoadSchedulerTaskConfig can reload all the file
	// and 'parent' will be invalid
	CBratTask::uid_t uid_saved = 0;
	if ( parent != nullptr )
	{
		uid_saved = parent->GetUid();
	}

	if ( !task_scheduler->LoadAllTasks() )
	{
		wait.Restore();
		SimpleErrorBox( "An error accessing the scheduled tasks file prevents delaying tasks execution." );
		return nullptr;
	}

	//restore parent
	if ( parent != nullptr )
		parent = task_scheduler->FindTaskFromMap( uid_saved );

	CBratTask *task = nullptr;
	try
	{
		std::string log_dir = GetDirectoryFromPath( mCurrentOperation->GetOutputPath( type ) );
		if ( exporting_netcdf )
		{
			CVectorBratAlgorithmParam params;
			params.Insert( mCurrentOperation->GetOutputPath() );
			params.Insert( mCurrentOperation->GetOutputPath( type ) );

			task = task_scheduler->CreateFunctionTaskAsPending( parent, CBratTaskFunction::m_TASK_FUNC_COPYFILE, params, at, task_label, log_dir );
		}
		else
		{
			task = task_scheduler->CreateCmdTaskAsPending( parent, mCurrentOperation->GetFullCmd( type ), at, task_label, log_dir );
		}
	}
	catch ( const CException &e )
	{
		SimpleErrorBox( e.Message() );
		return nullptr;
	}

	//if ( !SaveSchedulerTaskConfig() )
	//{
	//	RemoveTaskFromSchedulerTaskConfig( taskNode );
	//}

	return task;
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_OperationControls.cpp"
