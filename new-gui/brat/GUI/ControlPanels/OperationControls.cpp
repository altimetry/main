#include "new-gui/brat/stdafx.h"

#include "libbrathl/TreeField.h"
#include "libbrathl/Field.h"

#include "new-gui/Common/QtUtils.h"

#include "ApplicationLogger.h"
#include "DataModels/Model.h"
#include "DataModels/Workspaces/Workspace.h"
#include "DataModels/Workspaces/Operation.h"
#include "GUI/ActionsTable.h"
#include "GUI/DisplayWidgets/TextWidget.h"
#include "GUI/DisplayEditors/MapEditor.h"
#include "GUI/DisplayEditors/PlotEditor.h"

#include "ProcessesTable.h"
#include "OperationControls.h"


Q_DECLARE_METATYPE( CMapTypeField::ETypeField )



CDataExpressionsTree::CDataExpressionsTree( QWidget *parent, QWidget *drag_source )
	: base_t( parent )
	, mDragSource( drag_source )
{
	SetObjectName( this, "DataExpressionsTree" );

    QStringList labels;
    labels << tr("Data Expressions");
    setHeaderLabels(labels);

    mGroupIcon.addPixmap( style()->standardPixmap( QStyle::SP_DirClosedIcon ), QIcon::Normal, QIcon::Off );
    mGroupIcon.addPixmap( style()->standardPixmap( QStyle::SP_DirOpenIcon ), QIcon::Normal, QIcon::On );
    mKeyIcon.addPixmap( style()->standardPixmap( QStyle::SP_FileIcon ) );

	QTreeWidgetItem *
	child = new QTreeWidgetItem( this );
	setItemWidget( child, 0, new QLabel("<b>X</b>", this ) );
	child->setData( 0, Qt::UserRole, QVariant::fromValue( CMapTypeField::eTypeOpAsX ) );
	child->setIcon( 0, mGroupIcon );


	child = new QTreeWidgetItem( this );
	setItemWidget( child, 0, new QLabel("<b>Y (optional)</b>", this ) );
	child->setData( 0, Qt::UserRole, QVariant::fromValue( CMapTypeField::eTypeOpAsY ) );
	child->setIcon( 0, mGroupIcon );

	child = new QTreeWidgetItem( this );
	setItemWidget( child, 0, new QLabel("<b>Data</b>", this ) );
	child->setData( 0, Qt::UserRole, QVariant::fromValue( CMapTypeField::eTypeOpAsField ) );
	child->setIcon( 0, mGroupIcon );

	child = new QTreeWidgetItem( this );
	child->setText( 0, "Selection criteria (optional)" );
	child->setData( 0, Qt::UserRole, QVariant::fromValue( CMapTypeField::eTypeOpAsSelect ) );
	child->setIcon( 0, mGroupIcon );

	viewport()->setAcceptDrops(true);
	setDropIndicatorShown(true);

	auto *root_item = invisibleRootItem();
    root_item->setFlags( root_item->flags() & ~Qt::ItemIsDropEnabled );
}


void CDataExpressionsTree::dragEnterEvent( QDragEnterEvent *event )
{
	if ( mDragSource == event->source() )
	{
		auto *item = itemAt( event->pos() );
		QModelIndex dropIndex = indexAt( event->pos() );
		if ( item && ( itemAt( dropIndex.row(), 0 ) == item ) /*&& !item->parent() && ( item->childCount() == 0 ) && item != invisibleRootItem()*/ )
		{
			event->acceptProposedAction();
			qDebug() << "dragEnterEvent " << item;
		}
	}
}

void CDataExpressionsTree::dropEvent( QDropEvent *event )
{
	if ( mDragSource == event->source() && event->dropAction() == Qt::CopyAction )
	{
		auto *item = itemAt( event->pos() );

		qDebug() << "dropEvent " << item;		// items are different !!!!

		if ( item && item != invisibleRootItem() && !item->parent() && item->childCount() == 0 && item != invisibleRootItem() )
		{
			//TODO if childCount == 1, replace 

			QString field = event->mimeData()->text();
			QTreeView::dropEvent( event );
			event->acceptProposedAction();
			return;
		}
	}

	event->ignore();
	return;
}




static const std::string ButtonDisplayTextNoOp = "View...";
static const std::string ButtonDisplayTextIsMap = "Map...";
static const std::string ButtonDisplayTextIsPlot = "Plot...";


//static 
QList<QAction*> COperationControls::CreateDataComputationActions( QObject *parent )
{
	QList<QAction*> actions;
	auto map = CMapDataMode::GetInstance();
	for ( CMapDataMode::const_iterator it = map.begin(); it != map.end(); it++ )
	{
		if ( !isDefaultValue( it->second ) )
		{
			actions << new QAction( it->first.c_str(), parent );
		}
	}
	return actions;
}




CDataExpressionsTree* COperationControls::CreateDataExpressionsTree( QWidget *parent, QWidget *drag_source )
{
	return new CDataExpressionsTree( parent, drag_source );
}






QWidget* COperationControls::CreateQuickOperationsPage()
{
	auto mQuickOperationsPage = new QWidget( this );

	// I. Saved Variables Group

	mQuickVariablesList = CreateBooleanList( nullptr, { { "SSH", true }, { "SWH" }, { "Winds" }, { "Sigma0", true } } );

	auto mAddVariable = new QPushButton( "Add Variable" );
	auto mClear = new QPushButton( "Clear" );
	QBoxLayout *buttons_vl = LayoutWidgets( Qt::Vertical, { mAddVariable, mClear } );

	QBoxLayout *variables_hl = LayoutWidgets( Qt::Horizontal, { mQuickVariablesList, buttons_vl }, nullptr, 0, m, m, m, m );

	auto desc_label = new QLabel( "Variable Description" );
	auto mVarTable = new QTableWidget;
	mVarTable->setColumnCount( 4 );
	QBoxLayout *desc_vl = LayoutWidgets( Qt::Vertical, { desc_label, mVarTable }, nullptr, 0, m, m, m, m );

	QGroupBox *vars_group = CreateGroupBox( ELayoutType::Vertical, { variables_hl, desc_vl }, "Saved Variables" , mQuickOperationsPage, 0, m, m, m, m );

	// IV. Assemble all groups in quick operations page

	LayoutWidgets( Qt::Vertical, 
	{ 
		vars_group, 
	}, 
	mQuickOperationsPage, 0, m, m, m, m );

	return mQuickOperationsPage;
}


QWidget* COperationControls::CreateAdancedOperationsPage()
{
	QWidget *mAdvancedOperationsPage = new QWidget( this );
	QWidget *tab_parent = mAdvancedOperationsPage;			// new QWidget( mAdvancedOperationsPage );	// mAdvancedOperationsPage;

	// I. Buttons row

	mInsertFunction = CreateToolButton( "", ":/images/alpha-numeric/1.png", "Insert function" );
	mInsertAlgorithm = CreateToolButton( "", ":/images/alpha-numeric/2.png", "Insert algorithm" );
	mInsertFormula = CreateToolButton( "", ":/images/alpha-numeric/3.png", "Insert formula" );
	mSaveAsFormula = CreateToolButton( "", ":/images/alpha-numeric/4.png", "Save as formula" );

    mShowAliasesButton = CreateToolButton( "", ":/images/alpha-numeric/__e.png", "Show aliases" );
	mCheckSyntaxButton = CreateToolButton( "", ":/images/OSGeo/check.png", "Check syntax" );
	mShowInfoButton = CreateToolButton( "", ":/images/OSGeo/page-info.png", "Show information" );

	mDataComputationGroup = CreateActionGroup( this, CreateDataComputationActions( this ), true );
	mDataComputation = CreateMenuButton(  "", ":/images/alpha-numeric/b.png", "Set how data are stored/computed", mDataComputationGroup->actions() );

	auto *formulas_l = LayoutWidgets( Qt::Horizontal, 
	{ 
		mInsertFunction, mInsertAlgorithm, mInsertFormula, mSaveAsFormula, mDataComputation,
		nullptr, mCheckSyntaxButton, mShowInfoButton, mShowAliasesButton
	}, 
	nullptr, s, m, m, m, m );


	// II. Fields Layout & Variable Description

	// - fields
	auto adv_fields_label = new QLabel( "Fields", tab_parent );
	mAdvancedFieldList = new QListWidget( tab_parent );
	mAdvancedFieldList->setDragEnabled( true );
	QBoxLayout *adv_fields_vl = LayoutWidgets( Qt::Vertical, { adv_fields_label, mAdvancedFieldList }, nullptr, s, m, m, m, m );

	auto adv_desc_label = new QLabel( "Variable Description", tab_parent );
	auto mAdvVarTable = new QTableWidget( tab_parent );
	mAdvVarTable->setColumnCount( 4 );
	QBoxLayout *adv_desc_vl = LayoutWidgets( Qt::Vertical, { adv_desc_label, mAdvVarTable }, nullptr, 0, m, m, m, m );

	QBoxLayout *fields_hl = LayoutWidgets( Qt::Horizontal, { adv_fields_vl, adv_desc_vl }, nullptr, s, m, m, m, m );
	

	// III. Data Expressions Layout

	mDataExpressionsTree = CreateDataExpressionsTree( tab_parent, mAdvancedFieldList );
	//QBoxLayout *adv_expressions_vl = LayoutWidgets( Qt::Vertical, { mDataExpressionsTree }, nullptr, s, m, m, m, m );


	// IV. Expression Group

	// expressions
	mExpressionTextWidget = new CTextWidget;
	auto exp_group = CreateGroupBox( ELayoutType::Horizontal, { mExpressionTextWidget }, "Expression", tab_parent, 0, m, m, m, m );

    static const QString SyncGroup("SyncGroup");
	QgsCollapsibleGroupBox *expression_group = CreateCollapsibleGroupBox( ELayoutType::Vertical, { formulas_l, fields_hl, mDataExpressionsTree, exp_group }, 
		"Expression", tab_parent, 0, m, m, m, m );
	//expression_group->setCollapsed( true );
	expression_group->setSyncGroup( SyncGroup );
	//expression_group->setCheckable( true );


	// V. Sampling Group

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
        "Sampling", tab_parent, 2, 4, 4, 4, 4 );
	sampling_group->setCollapsed( true );
	sampling_group->setCheckable( true );
	sampling_group->setSyncGroup( SyncGroup );


    LayoutWidgets( Qt::Vertical,
	{ 
		expression_group,
		sampling_group,
		nullptr
	}, 
	tab_parent, 0, m, m, m, m );

	return mAdvancedOperationsPage;
}


//	QUESTIONS: 
//	save workspace, not individual things...
//	quick can only be quick if an auto/default operation is created
//	view: determined by operation: distinction quick/advanced does not seem to stand

QWidget* COperationControls::CreateCommonWidgets()
{
	//buttons row

	mNewOperationButton = CreateToolButton( "", ":/images/OSGeo/new.png", "Create a new operation" );
    mRenameOperationButton = CreateToolButton( "", ":/images/OSGeo/edit.png", "Rename the selected operation" );
	// TODO confirm that save will only be done in the scope of whole workspace
	mDeleteOperationButton = CreateToolButton( "", ":/images/OSGeo/workspace-delete.png", "Delete the selected operation" );
	auto mSaveOperationButton = CreateToolButton( "", ":/images/OSGeo/save2.png", "Save the selected operation???????????????????????????????" );
	mSaveOperationButton->setEnabled( false );
	mSaveOperationButton->setVisible( false );

	mOperationFilterGroup = CreateActionGroup( this, { new QAction( "Filter 1", this ), new QAction( "Filter 2", this ), new QAction( "Filter 3", this ) }, true );
	mOperationFilterButton = CActionInfo::CreateMenuButton( eActionGroup_Filters, mOperationFilterGroup->actions() );

	mExportOperationAction = new QAction( "Export...", this );
	mEditExportAsciiAction = new QAction( "Edit ASCII Export", this );
	mOperationExportButton = CreateMenuButton( "", ":/images/OSGeo/export.png", "Export operation", { mExportOperationAction, mEditExportAsciiAction } );

	mOperationStatisticsButton = CreateToolButton( "", ":/images/OSGeo/stats.png", "Generate statistics and show results saved in file" );

	mExecuteButton = CreateToolButton( "Execute...", ":/images/OSGeo/execute.png", "Invoke the tool that executes the operation" );
	mExecuteButton->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
	mExecuteButton->setPopupMode( QToolButton::MenuButtonPopup );
	mDelayExecutionAction = new QAction( "Delay Execution", this );
	mLaunchSchedulerAction = CActionInfo::CreateAction( this, eAction_Launch_Scheduler );
	mExecuteButton->addAction( mDelayExecutionAction );
	mExecuteButton->addAction( mLaunchSchedulerAction );

	mDisplayButton = CreateToolButton( ButtonDisplayTextNoOp, ":/images/themes/default/propertyicons/display.svg", "View and edit operation plots" );
	mDisplayButton->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
	mDisplayButton->setPopupMode( QToolButton::MenuButtonPopup );
	mSplitPlotsAction = new QAction( "Split Plots", this );
	mSplitPlotsAction->setCheckable( true );
	mDisplayButton->addAction( mSplitPlotsAction );

	auto *buttons_l = LayoutWidgets( Qt::Horizontal
		, { mNewOperationButton, mRenameOperationButton, mDeleteOperationButton, mSaveOperationButton, 
		mOperationFilterButton, mOperationExportButton, mOperationStatisticsButton,
		nullptr, mDisplayButton, mExecuteButton }, 
		nullptr, s, m, m, m, m );


	//operation and dataset lists

	mDatasetsList = new QListWidget;
	mDatasetsList->setToolTip( "Selected operation dataset" );
	auto *data_l = LayoutWidgets( Qt::Vertical, { new QLabel("Operation Dataset"), mDatasetsList }, nullptr, 0, m, m, m, m );
	//mDatasetsList->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );

	mOperationsList = new QListWidget;		
	mOperationsList->setToolTip( "List of current workspace operations" );
	//mOperationsList->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );

	auto *lists_l = LayoutWidgets( Qt::Horizontal, { mOperationsList }, nullptr, s, m, m, m, m );


	//frame and wrap all

	QFrame *frame = new QFrame;
	frame->setWindowTitle( "QFrame::Box" );
	frame->setFrameStyle( QFrame::Panel );
	frame->setFrameShadow( QFrame::Sunken );
	frame->setObjectName("OperationsFrame");
	frame->setStyleSheet("#OperationsFrame { border: 1px solid black; }");
	//frame->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );

    auto *ops_l = LayoutWidgets( Qt::Horizontal, { lists_l, data_l }, nullptr, s, m, m, m, m );
	LayoutWidgets( Qt::Vertical, { buttons_l, ops_l }, frame, s, m, m, m, m );

	return frame;
}


void COperationControls::Wire()
{
	//common

	connect( mOperationsList, SIGNAL( currentRowChanged( int ) ), this, SLOT( HandleSelectedOperationChanged( int ) ) );
	connect( mDatasetsList, SIGNAL( currentRowChanged( int ) ), this, SLOT( HandleSelectedDatasetChanged( int ) ) );

	connect( mNewOperationButton, SIGNAL( clicked() ), this, SLOT( HandleNewOperation() ) );
    connect( mRenameOperationButton, SIGNAL( clicked() ), this, SLOT( HandleRenameOperation() ) );
	connect( mDeleteOperationButton, SIGNAL( clicked() ), this, SLOT( HandleDeleteOperation() ) );
	for ( auto *a : mOperationFilterGroup->actions() )
		connect( a, SIGNAL( triggered() ), this, SLOT( HandleOperationFilter() ) );
	connect( mExportOperationAction, SIGNAL( triggered() ), this, SLOT( HandleExportOperation() ) );
	connect( mEditExportAsciiAction, SIGNAL( triggered() ), this, SLOT( HandleEditExportAscii() ) );
	connect( mOperationStatisticsButton, SIGNAL( clicked() ), this, SLOT( HandleOperationStatistics() ) );

	connect( mDisplayButton, SIGNAL( clicked() ), this, SLOT( HandleLaunchDisplay() ) );

	connect( mExecuteButton, SIGNAL( clicked() ), this, SLOT( HandleExecute() ) );
	connect( mDelayExecutionAction, SIGNAL( triggered() ), this, SLOT( HandleDelayExecution() ) );
	connect( mLaunchSchedulerAction, SIGNAL( triggered() ), this, SLOT( HandleLaunchScheduler() ) );

	//remaining

	//...advanced
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

	//connect( mOperationsStakWidget, SIGNAL( PageChanged( int ) ), this, SLOT( PageChanged( int ) ) );

	//connect( &mTimer, SIGNAL( timeout() ), this, SLOT( HandleExecute() ) );
	//mTimer.start( 10000 );
}


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

	AddTopLayout( ELayoutType::Vertical, {
		mCommonGroup,
		LayoutWidgets( Qt::Horizontal, { m_QuickButton, m_AdvancedButton }, nullptr, s, m, m, m, m ),
		mStackWidget,
		nullptr
	},
	s, m, m, m, m
	);

 //   AddTopWidget( mCommonGroup );

	//AddTopLayout( ELayoutType::Horizontal, { m_QuickButton, m_AdvancedButton }, 0, 2, 2, 2, 2 );

 //   AddTopWidget( mStackWidget );

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

	mOperationsList->clear();
	mDatasetsList->clear();

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


	mDatasetsList->blockSignals( true );
    FillList( mDatasetsList, *mWDataset->GetDatasets(),

        -1, true,											//TODO default to last opened

        []( const CObMap::value_type &i ) -> const char*
        {
            return i.first.c_str();
        }
    );
	mDatasetsList->blockSignals( false );


    FillList( mOperationsList, *mWOperation->GetOperations(),

        0, true,											//TODO default to last opened

        []( const CObMap::value_type &i ) -> const char*
        {
            return i.first.c_str();
        }
    );



#if defined (TESTING_GLOBE)

	//if ( mWDisplay )
	//	QuickMap();

#endif

}

#if defined ItemText
#undef ItemText
#endif

inline int ItemRow( QListWidgetItem *item )
{
	return item->listWidget()->row( item );
}
inline int FindText( QListWidget *list, const char *text  )
{
    auto items = list->findItems( text, Qt::MatchExactly );			assert__( items.size() == 1 );
	return ItemRow( items[ 0 ] );
}
inline QString ItemText( QListWidget *list, int index  )
{
	auto *item = list->item( index );
	return item->text();
}


void COperationControls::HandleSelectedOperationChanged( int operation_index )
{
	mExecuteButton->setEnabled( operation_index >= 0 );				// TODO also update state of all other...
	mDatasetsList->setEnabled( operation_index >= 0 );
	mDisplayButton->setEnabled( operation_index >= 0 );

	mDisplayButton->setText( ButtonDisplayTextNoOp.c_str() );

	if ( operation_index < 0 )
	{
		mOperation = nullptr;
		return;
	}

	mOperation = mWOperation->GetOperation( q2a( mOperationsList->item( operation_index )->text() ) );			assert__( mOperation );
	//auto &items = mDatasetsList->findItems( mOperation->GetDatasetName().c_str(), Qt::MatchExactly );			assert__( items.size() == 1 );
	mDatasetsList->setCurrentRow( FindText( mDatasetsList, mOperation->GetDatasetName().c_str() ) );	//trigger signal

	mDisplayButton->setText( mOperation->IsMap() ? ButtonDisplayTextIsMap.c_str() : ButtonDisplayTextIsPlot.c_str() );

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

		FillFieldList();	//TODO THIS IS WRONG: IT IS A TREE

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

	auto new_datset_name = q2a( ItemText( mDatasetsList, dataset_index ) );
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
			mDatasetsList->setCurrentRow( FindText( mDatasetsList, mOperation->GetDatasetName().c_str() ) );
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


void COperationControls::FillFieldList()
{
    if ( mDatasetsList->currentRow() < 0 )
        return;

	CDataset *current_dataset = mOperation->GetDataset();		assert__( current_dataset->GetName() == q2a( ItemText( mDatasetsList, mDatasetsList->currentRow() ) ) );

    Q_UNUSED( current_dataset );

	assert__( mProduct );
    try
    {
        // GetDictlist()->InsertProduct(m_product); //////////////////////////////////////////////
        CTreeField* tree = mProduct->GetTreeField();

        for ( auto const *object : *tree )
        {
            const CField *field  = static_cast< const CField* >( object );			assert__( dynamic_cast< const CField*>( object ) != nullptr );

            // InsertField(field); ===========================================
            auto const &type_id = typeid( *field );

            if ( type_id == typeid( CFieldIndex )
                ||  type_id == typeid( CFieldRecord )
                || ( ( type_id == typeid( CFieldArray ) ) && ( field->IsFixedSize() == false ) )
                || field->IsVirtual() )
            {
                continue;
            }

            QListWidgetItem *item = new QListWidgetItem;

            item->setData(Qt::AccessibleTextRole,        field->GetName().c_str());
            item->setData(Qt::AccessibleDescriptionRole, field->GetDescription().c_str());
            item->setText( const_cast<CField*>( field )->GetFullName().c_str() );

            mAdvancedFieldList->addItem( item );
            // =================================================================

			//if ( startsWith( const_cast<CField*>( field )->GetFullName(), std::string( "lat" ) ) )
			//{
			//	CFieldNetCdf *f = dynamic_cast<CFieldNetCdf*>( const_cast<CField*>( field ) );
			//	if ( f && f->IsFixedSize() )
			//		if ( f->IsFixedSize() )
			//		{
			//			double *data = f->GetValuesAsArray();
			//		}
			//}
        }
        // ////////////////////////////////////////////////////////////////////////////////////

        //if ( product != nullptr )
        //{
        //    mFileDescGroup->setEnabled(product->IsNetCdf());

        //    mFileDesc->setEnabled( product->IsNetCdf() );
        //    mFileDesc->setText( QString("Product : ") +
        //                        product->GetProductClass().c_str() +
        //                        QString(" / ") +
        //                        product->GetProductType().c_str() +
        //                        QString("\n------------------------------------------------------\n") +
        //                        product->GetDescription().c_str() );
        //}
    }
    catch ( CException& e )
    {
        //UNUSED( e );
        SimpleErrorBox(e.Message());
        //wxMessageBox(wxString::Format("Unable to process files.\nReason:\n%s",
        //                              e.what()),  "Warning",  wxOK | wxICON_EXCLAMATION);
        //this->SetCursor(wxNullCursor);
    }
}




void COperationControls::HandleNewOperation()
{
	NOT_IMPLEMENTED;
}
void COperationControls::HandleDeleteOperation()
{
	NOT_IMPLEMENTED;
}
void COperationControls::HandleRenameOperation()
{
	NOT_IMPLEMENTED;
}
void COperationControls::HandleOperationFilter()
{
	auto a = qobject_cast<QAction*>( sender() );	assert__( a );

	MSG_NOT_IMPLEMENTED( q2a( a->text() ) );
}
void COperationControls::HandleExportOperation()
{
	NOT_IMPLEMENTED;
}
void COperationControls::HandleEditExportAscii()
{
	NOT_IMPLEMENTED;
}
void COperationControls::HandleOperationStatistics()
{
	NOT_IMPLEMENTED;
}
void COperationControls::HandleDelayExecution()
{
	NOT_IMPLEMENTED;
}
void COperationControls::HandleLaunchScheduler()
{
	NOT_IMPLEMENTED;
}

void COperationControls::HandleInsertFunction()
{
	NOT_IMPLEMENTED;
}
void COperationControls::HandleInsertAlgorithm()
{
	NOT_IMPLEMENTED;
}
void COperationControls::HandleInsertFormula()
{
	NOT_IMPLEMENTED;
}
void COperationControls::HandleSaveAsFormula()
{
	NOT_IMPLEMENTED;
}
void COperationControls::HandleDataComputation()
{
	assert__( mOperation != nullptr );

	auto a = qobject_cast<QAction*>( sender() );	assert__( a );

	MSG_NOT_IMPLEMENTED( q2a( a->text() ) );
	return;

	CFormula *formula = nullptr;	// TODO DODODODODODODODODODODODODO GetCurrentFormula();
	if ( formula == nullptr )
	{
		return;
	}

	// if same pointer
	if ( mOperation->GetSelect() == formula )	//== IsCriteriaSelection( formula ) in old OperationPanel
	{
		return;
	}

	auto id   = CMapDataMode::GetInstance().NameToId( q2a( a->text() ) );
	formula->SetDataMode( id );
}
void COperationControls::HandleCheckSyntax()
{
	NOT_IMPLEMENTED;
}
void COperationControls::HandleShowInfo()
{
	NOT_IMPLEMENTED;
}

void COperationControls::HandleShowAliases()
{
    NOT_IMPLEMENTED

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
	NOT_IMPLEMENTED;
}







/////////////////////////////////////////////////////////////////////////////////
//						Launch Display Actions
/////////////////////////////////////////////////////////////////////////////////


void COperationControls::HandleLaunchDisplay()
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



/////////////////////////////////////////////////////////////////////////////////
//								Execute
/////////////////////////////////////////////////////////////////////////////////



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

	if ( !SimpleQuestion(
		"Operation '"
		+ mOperation->GetName()
		+ "' will be executed with the following command line\n\n"
		+ mOperation->GetFullCmd()
		+ "\n\nDo you want to proceed?" ) 
		)
		return;

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
