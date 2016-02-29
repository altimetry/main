#include "new-gui/brat/stdafx.h"

#include "libbrathl/TreeField.h"
#include "libbrathl/Field.h"

#include "new-gui/Common/QtUtils.h"

#include "ApplicationLogger.h"
#include "DataModels/Model.h"
#include "DataModels/Workspaces/Workspace.h"
#include "DataModels/Workspaces/Operation.h"
#include "GUI/DisplayWidgets/TextWidget.h"
#include "GUI/DisplayEditors/MapEditor.h"
#include "GUI/DisplayEditors/PlotEditor.h"

#include "ProcessesTable.h"
#include "OperationControls.h"


QWidget* COperationControls::CreateQuickOperationsPage()
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



QWidget* COperationControls::CreateAdancedOperationsPage()
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
	mExpressionTextWidget = new CTextWidget;
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

	mAvancedDisplayButton = new QPushButton( "View" );
	auto mSplitPlots = new QCheckBox ( "Split Plots" );
	QGroupBox *views_group = CreateGroupBox( ELayoutType::Horizontal, { mAvancedDisplayButton, mSplitPlots }, "Views" , tab_parent, 6, 2, 2, 2, 2 );


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

QWidget* COperationControls::CreateCommonWidgets()
{
	const int m = 6;
	const int s = 4;

	mOperationsCombo = new QComboBox;		
	mOperationsCombo->setToolTip( "List of current workspace operations" );
	auto mNewOperation = new QPushButton( "New..." );
	auto mDeleteOperation = new QPushButton( "Delete..." );
    auto mRenameOperation = new QPushButton( "Rename..." );
	auto dlabel = new QLabel("Operation Dataset");
	mDatasetsCombo = new QComboBox;
	mDatasetsCombo->setToolTip( "Operation dataset" );

	auto mSaveOperation = new QPushButton( "Save..." );	// TODO confirm that save will only be done in the scope of whole workspace
	
	QFrame *frame = new QFrame;
	frame->setWindowTitle( "QFrame::Box" );
	frame->setFrameStyle( QFrame::Panel );
	frame->setFrameShadow( QFrame::Sunken );
	frame->setObjectName("OperationsFrame");
	frame->setStyleSheet("#OperationsFrame { border: 2px solid black; }");
	auto *grid_l = LayoutWidgets( { 
		mOperationsCombo, mNewOperation, mDeleteOperation, nullptr,
		mRenameOperation, dlabel, mDatasetsCombo, nullptr, 
		mSaveOperation
	}, nullptr, s, m, m, m, m );

	mExecuteButton = new QToolButton;
	mExecuteButton->setText( "Execute..." );
	QPixmap pix(":/images/alpha-numeric/0.png");
	QIcon icon(pix);	
	mExecuteButton->setIcon(icon);
	mExecuteButton->setIconSize(QSize(icon_size, icon_size));
	mExecuteButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	mExecuteButton->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );

    LayoutWidgets( Qt::Horizontal, { grid_l, mExecuteButton	}, frame, s, m, m, m, m );

	return frame;
}



//explicit 
COperationControls::COperationControls( CProcessesTable *processes_table, CDesktopManagerBase *manager, QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0 
	: base_t( manager, parent, f )
	, mProcessesTable( processes_table )
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

    Wire();
}

void COperationControls::Wire()
{
	connect( mQuickMapButton, SIGNAL( clicked() ), this, SLOT( HandleQuickMap() ) );
	connect( mQuickPlotButton, SIGNAL( clicked() ), this, SLOT( HandleQuickPlot() ) );

	connect( mShowAliases, SIGNAL( clicked() ), this, SLOT( HandleShowAliases() ) );

	connect( mOperationsCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( HandleSelectedOperationChanged( int ) ) );
	connect( mDatasetsCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( HandleSelectedDatasetChanged( int ) ) );
	connect( mExecuteButton, SIGNAL( clicked() ), this, SLOT( HandleExecute() ) );

	connect( mAvancedDisplayButton, SIGNAL( clicked() ), this, SLOT( HandleDisplay() ) );
	//connect( mOperationsStakWidget, SIGNAL( PageChanged( int ) ), this, SLOT( PageChanged( int ) ) );

	//connect( &mTimer, SIGNAL( timeout() ), this, SLOT( HandleExecute() ) );
	//mTimer.start( 10000 );
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

	mOperationsCombo->clear();
	mDatasetsCombo->clear();

    mWRoot = mModel ? mModel->RootWorkspace() : nullptr;
    if ( !mModel || !mWRoot )
	{
		mWDataset = nullptr;
		mWOperation = nullptr;
		mWDisplay = nullptr;
		mWFormula = nullptr;

        //TODO clear all

		return;
	}

	mWDataset = mModel->Workspace< CWorkspaceDataset >();
	mWOperation = mModel->Workspace< CWorkspaceOperation >();
	mWDisplay = mModel->Workspace< CWorkspaceDisplay >();
    mWFormula = mModel->Workspace< CWorkspaceFormula >();		assert__( mWDataset && mWOperation && mWDisplay && mWFormula );

	mWFormula->GetFormulaNames( mMapFormulaString );


	mDatasetsCombo->blockSignals( true );
    FillCombo( mDatasetsCombo, *mWDataset->GetDatasets(),

        []( const CObMap::value_type &i ) -> const char*
        {
            return i.first.c_str();
        },
        -1, true
    );
	mDatasetsCombo->blockSignals( false );


    FillCombo( mOperationsCombo, *mWOperation->GetOperations(),

        []( const CObMap::value_type &i ) -> const char*
        {
            return i.first.c_str();
        },
        0, true
    );


#if defined (TESTING_GLOBE)

	//if ( mWDisplay )
	//	QuickMap();

#endif

}

void COperationControls::HandleDisplay()
{
	assert__( mOperation );

	if ( mOperation->IsMap() )
		HandleQuickMap();
	else
	if ( mOperation->IsZFXY() )
		HandleQuickPlot();
	else
	if ( mOperation->IsYFX() )
		HandleQuickPlot();
	else
		assert__( false );
}


void COperationControls::HandleQuickMap()
{
	WaitCursor wait;				assert__( mModel );

	auto ed = new CMapEditor( mModel, mOperation, mManager->parentWidget() );
	auto subWindow = mManager->AddSubWindow( ed );
	subWindow->show();

	//openTestFile( t2q( mManager->mPaths.mWorkspacesDir + R"(/newWP/Displays/DisplayDisplays_New.par)" ) );
}

void COperationControls::HandleQuickPlot()
{
	WaitCursor wait;				assert__( mModel );

    auto ed = new CPlotEditor( mModel, mOperation, mManager->parentWidget() );
    auto subWindow = mManager->AddSubWindow( ed );
    subWindow->show();

    //openTestFile( t2q( mManager->mPaths.mWorkspacesDir + R"(/newWP/Displays/DisplayDisplays_2.par)" ) );
}




void COperationControls::HandleExecute()
{
    //static int n = 0;

	// ProcessesTable will display user messages for us, no need to report on false return
	//
	//emit SyncProcessExecution( true );
	//mProcessesTable->Add( true, true, "brat-lab", "brat-lab", { "20", "ARG_0", "arg_1", "A_R_G" } );
	//emit SyncProcessExecution( false );
	//return;

	assert__( mOperation );

	auto CtrlOperation = [this]( CWorkspaceFormula *wks, std::string& msg, bool basicControl, const CStringMap* aliases )
	{
		if ( mOperation && mOperation->Control( wks, msg, basicControl, aliases ) )
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
			+ mOperation->GetName()
			+ "' has some errors and can't be executed"
			+ ( msg.empty() ? "." : ":\n" )
			+ msg );

		return;
	}

	//wxGetApp().GotoLogPage();

	const bool wait = false;			//TODO replace by function parameter

	if ( wait )
	{
		mOperation->SetLogFile( mWOperation );
	}
	else
	{
		mOperation->ClearLogFile();
	}

	//BuildCmdFile(); == following 
	std::string error_msg;
	if ( !mOperation->BuildCmdFile( mWFormula, mWOperation, error_msg ) )	//v3 didn't seem to care if this fails
	{
		assert__( !error_msg.empty() );

		SimpleWarnBox( error_msg );
		return;
	}

	/*
	CPipedProcess* process = new CPipedProcess( 

		mOperation->GetTaskName(),
		wxGetApp().GetLogPanel(),
		mOperation->GetFullCmd(),
		wxGetApp().GetLogPanel()->GetLogMess(),
		&mOperation->GetOutputPath(),				//used in remove file, must be complete path
		mOperation->GetType() 

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
			msg = wxString::Format( "\n\n===> Synchronous Task '%s' started with command line below:<===\n'%s'\n\n"
				"\n ==========> Please wait.... A report will display at the end of the task <==========\n\n",
				process->GetName().c_str(),
				process->GetCmd().c_str() );
			GetLogMess()->AppendText( msg );
			GetTasklist()->InsertProcess( process );
		}

		*/

	// ProcessesTable will display user messages for us, no need to report on false return
	//
	emit SyncProcessExecution( true );
	mProcessesTable->Add( false, false, mOperation->GetTaskName(), mOperation->GetFullCmd() );
	emit SyncProcessExecution( false );

	/*
		//----------------
		process->Execute();
		//----------------

		if ( !isSync )
		{

			msg = wxString::Format( "\n\n===> Asynchronous Task '%s' (pid %d) started with command line below:<===\n'%s'\n\n",
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
		wxGetApp().GetLogPanel()->LogFile( wxFileName( mOperation->GetLogFile() ) );
	}


	NOT_IMPLEMENTED
	*/
}


void COperationControls::HandleSelectedOperationChanged( int operation_index )
{
	mExecuteButton->setEnabled( operation_index >= 0 );				// TODO also update state of all other...
	mDatasetsCombo->setEnabled( operation_index >= 0 );
	mAvancedDisplayButton->setEnabled( operation_index >= 0 );

	mQuickMapButton->setEnabled( false );			//must be corrected after operation set, if any
	mQuickPlotButton->setEnabled( false );

	if ( operation_index < 0 )
	{
		mOperation = nullptr;
		return;
	}

	mOperation = mWOperation->GetOperation( q2a( mOperationsCombo->itemText( operation_index ) ) );			assert__( mOperation );
	int dataset_index = mDatasetsCombo->findText( mOperation->GetDatasetName().c_str() );
	mDatasetsCombo->setCurrentIndex( dataset_index );	//trigger signal

	mQuickMapButton->setEnabled( mOperation->IsMap() );
	mQuickPlotButton->setEnabled( !mOperation->IsMap() );

	CDataset *dataset = nullptr;
	//DatasetSelChanged(id);//////////////////////////////////////////////////////////////
	{
		//ClearFieldsInfo();

		dataset = mOperation->GetDataset();	 //GetDatasettreectrl()->GetDataset( id );
		assert__( dataset != nullptr );			//v4 can an operation have a nullptr dataset??? v3 tested and returned on nullptr; leave assert here until we understand

		std::string msg;
		try
		{
			mProduct = dataset->SetProduct();

			if ( mProduct != nullptr )
			{
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
				msg = "Unable to set Product\nPerhaps dataset file list is empty or or product file doesn't exist()";

			delete mProduct;
			mProduct = nullptr;
			//DeleteProduct();
			return;
		}
		//	GetFieldstreectrl()->InsertProduct( mProduct );		TODO display fields
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
		mOperation->SetDataset( dataset );
		mOperation->SetProduct( mProduct );
	}
	//// If there is only one record in the dataset ==> select it	
	//GetOperationRecord();			TODO ???//////////////////////////////////////COperationPanel::GetOperationRecord(bool select = true)
	//wxTreeItemId id;
	//if ( mOperation == nullptr )
	//	return id;
	//id = GetFieldstreectrl()->FindItem( mOperation->GetRecord() );
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
	//	mOperation->SetRecord( GetFieldstreectrl()->GetCurrentRecord().ToStdString() );
	//}

	//SetDataSetAndRecordLabel();	TODO ???	///////////////////void COperationPanel::SetDataSetAndRecordLabel()
	//{
	//	if ( mProduct == nullptr )
	//	{
	//		GetOperationtreectrllabel()->SetLabel( "" );
	//		GetOpchangerecord()->Enable( false );
	//		return;
	//	}
	//	if ( mOperation == nullptr )
	//	{
	//		GetOperationtreectrllabel()->SetLabel( "" );
	//		GetOpchangerecord()->Enable( false );
	//		return;
	//	}

	//	wxString strFormat = "Product: %s/%s\nDataset: %s\nDefault record: %s";

	//	if ( CProductNetCdf::IsProductNetCdf( mProduct ) )
	//	{
	//		strFormat = "Product: %s/%s\nDataset: %s";
	//	}

	//	wxString str;
	//	wxString datasetName = "?";
	//	wxString recordName = "?";

	//	if ( !mOperation->GetDatasetName().empty() )
	//	{
	//		datasetName =  mOperation->GetDatasetName();
	//	}
	//	if ( !mOperation->GetRecord().empty() )
	//	{
	//		recordName =  mOperation->GetRecord();
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





void COperationControls::HandleSelectedDatasetChanged( int dataset_index )	
{
	if ( dataset_index < 0 )
	{
		return;
	}

	auto new_datset_name = q2a( mDatasetsCombo->itemText( dataset_index ) );
	if ( new_datset_name != mOperation->GetDatasetName() )
	{
		std::string question =
			"Some data expressions are built from '"
			+ mOperation->GetDatasetName()
			+ "' dataset.\nIf you want to use another dataset, make sure that field names in data expressions are the same, or don't forget to change them.\nChange to '"
			+ new_datset_name
			+ "' dataset ?";

		if ( !SimpleQuestion( question ) )
		{
			mDatasetsCombo->setCurrentIndex( mDatasetsCombo->findText( mOperation->GetDatasetName().c_str() ) );
			return;
		}

		NOT_IMPLEMENTED;
		return;
	}

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

  //if (mOperation == nullptr)
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


  //wxString datasetNameOperation =  mOperation->GetDatasetName();
  //wxString datasetName = dataset->GetName();
  //bool resetDataExpression = mOperation->HasFormula() && (datasetNameOperation.CmpNoCase(datasetName) != 0);

  //if (resetDataExpression)
  //{
  //  int32_t result =  wxMessageBox(wxString::Format("Some data expressions are built from '%s' dataset.\n"
  //                                            "If you want to use another dataset, make sure that field names in data expressions are the same, or don't forget to change them.\n"
  //                                            "Change to '%s' dataset ?",
  //                                                  mOperation->GetDatasetName().c_str(),
  //                                                  GetDatasettreectrl()->GetItemText(id).c_str()),
  //                               "Changing operation dataset",
  //                                wxYES_NO | wxCENTRE | wxICON_QUESTION);

  //  if (result != wxYES)
  //  {
  //    event.Veto();
  //    return;
  //  }

  //  mOperation->SetRecord("");

  //  CProduct* product = nullptr;

  //  try
  //  {
  //    product = dataset->SetProduct();
  //  }
  //  catch (CException& e)
  //  {
  //    wxMessageBox(wxString::Format("Unable to process files.\nReason:\n%s",
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

}







void COperationControls::HandleShowAliases()
{
	//if ( mOperation == nullptr || mProduct == nullptr )
	//	return;

	//std::string title = "Show '" + mProduct->GetProductClassType() + "' aliases...";

	//CFormula* formula = GetOperationtreectrl()->GetCurrentFormula();
	//bool hasOpFieldSelected = formula != nullptr;

	//CAliasInfoDlg dlg( this, -1, title, mOperation, formula );

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
#include "DataModels/DisplayFilesProcessor.h"
#include "DataModels/PlotData/ZFXYPlot.h"
#include "DataModels/PlotData/XYPlot.h"
#include "DataModels/PlotData/WorldPlot.h"


//L:\project\workspaces\newWP\Displays\DisplayDisplays_New.par

void COperationControls::openTestFile( const QString &fileName )
{
	delete mCmdLineProcessor;
	mCmdLineProcessor = new CDisplayFilesProcessor;

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

					auto ed = new CPlotEditor( mCmdLineProcessor, yfxplot, this );
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

					auto ed = new CPlotEditor( mCmdLineProcessor, zfxyplot, this );
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

#include "moc_OperationControls.cpp"
