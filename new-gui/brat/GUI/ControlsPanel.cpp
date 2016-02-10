#include "new-gui/brat/stdafx.h"

#include "new-gui/brat/ApplicationLogger.h"

#include "new-gui/brat/DataModels/Workspaces/Workspace.h"

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


//	title = "", parent = nullptr, spacing = smSpacing, left = smLeft, top = smTop, right 00000= smRight, bottom = smBottom
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
CDatasetBrowserControls::CDatasetBrowserControls( CDesktopManagerBase *manager, QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0 
	: base_t( manager, parent, f )
{
	// I. Browse Stack Widget; 2 pages: files and rads

	// - Page Files browser
	//
    mFilesList = new QListWidget;

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
    QBoxLayout *browse_buttons_hl = LayoutWidgets( Qt::Horizontal, { m_BrowseFilesButton, m_BrowseRadsButton }, nullptr, 0, 2, 2, 2, 2 );
	
    AddTopGroupBox( ELayoutType::Vertical, { browse_buttons_hl, mBrowserStakWidget }, "", 4, 4, 4, 4, 4 );


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

    Wire();
}

void CDatasetBrowserControls::Wire()
{
    connect( mDatasetsCombo, SIGNAL( currentIndexChanged(int) ), this, SLOT( DatasetChanged(int) ) );
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

    // Fill ComboBox with Datasets list
	mDatasetsCombo->clear();
    if (wksd)
    {
        FillCombo( mDatasetsCombo, *mWks->GetDatasets(),

            []( const CObMap::value_type &i ) -> const char*
            {
                return i.first.c_str();
            },
            0, true
        );

        // Change Dataset and perform required changes
        // TODO: Why is the following line unnecessary??
        //DatasetChanged();
    }
}


void CDatasetBrowserControls::DatasetChanged( int currentIndex )
{
    // Clear list of files
    mFilesList->clear();

    // Assert index of selected Dataset
    assert__( currentIndex == mDatasetsCombo->currentIndex() );

    // If not empty or a Dataset is selected
    if (currentIndex > -1)
    {
        // Get current Dataset
        const CObMap &rDatasets   = *mWks->GetDatasets();
        CObMap::const_iterator it = rDatasets.begin();
        std::advance(it, currentIndex);
        CDataset *currentDataset  = dynamic_cast< CDataset* >( it->second );   assert__(currentDataset);

        // Fill FileList with files of current Dataset
        std::vector<std::string> sFilesName;
        currentDataset->GetFiles(sFilesName);

        QIcon fileIcon = QIcon(":/images/OSGeo/db.png");
        int index = 0;
        for (auto it : sFilesName)
        {
            QListWidgetItem *file = new QListWidgetItem( QString(it.c_str()) );
            //file->setToolTip();
            file->setIcon( fileIcon );

            mFilesList->insertItem(index, file);
            index++;
        }
    }
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
CDatasetFilterControls::CDatasetFilterControls( CDesktopManagerBase *manager, QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0 
	: base_t( manager, parent, f )
{

    // I. "Where" Description group
    //
    //    I.1 List of Areas
    QListWidget *areas_list = CreateBooleanList( this, { { "Lake Baikal", true }, { "Black Sea" }, { "User Area 1", true }, { "User Area 2" } } );
    areas_list->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    QGroupBox *areas_box = CreateGroupBox( ELayoutType::Horizontal, { areas_list }, "Areas", this );


    //    I.2 Buttons for region selection
    mSavedRegionsCombo = new QComboBox;
    mSavedRegionsCombo->setToolTip( "List of saved regions" );

    auto mBox     = new QPushButton( "Box" );
    auto mPolygon = new QPushButton( "Polygon" );
    auto mKML     = new QPushButton( "KML" );
    auto mMask    = new QPushButton( "Mask" );

    QBoxLayout *buttons_regions = LayoutWidgets( Qt::Horizontal, { mSavedRegionsCombo, mBox, mPolygon, mKML, mMask } );

    //    I.3 Coordinates (max and min values)
    auto mMaxLat = new QLineEdit(this);
    auto mMaxLon = new QLineEdit(this);
    auto mMinLat = new QLineEdit(this);
    auto mMinLon = new QLineEdit(this);

    mMaxLat->setText("0.0");
    mMaxLon->setText("0.0");
    mMinLat->setText("0.0");
    mMinLon->setText("0.0");

    QBoxLayout *coord_values_l = LayoutWidgets( Qt::Vertical, {
                                             LayoutWidgets( Qt::Horizontal, { new QLabel( "Max Lat (deg)" ), mMaxLat } ),
                                             LayoutWidgets( Qt::Horizontal, { new QLabel( "Min Lat (deg)" ), mMinLat } )
                                               } );

    QBoxLayout *coord_values_r = LayoutWidgets( Qt::Vertical, {
                                             LayoutWidgets( Qt::Horizontal, { new QLabel( "Max Lon (deg)" ), mMaxLon } ),
                                             LayoutWidgets( Qt::Horizontal, { new QLabel( "Min Lon (deg)" ), mMinLon } )
                                               } );

    QBoxLayout *coord_values = LayoutWidgets( Qt::Horizontal, { coord_values_l, nullptr, coord_values_r }, nullptr );


    //    I.4 Adding previous widgets to this...
    QGroupBox *regions_coord = CreateGroupBox(ELayoutType::Vertical, {buttons_regions, coord_values} );
    regions_coord->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    AddTopGroupBox( ELayoutType::Horizontal, { areas_box, regions_coord }, "Where", 4, 4, 4, 4, 4 );


    // II. "When" Description group
    //
    //    II.1 Dates, Cycles and Pass (start and stop values)
    auto mStartDate  = new QLineEdit(this);
    auto mStopDate   = new QLineEdit(this);
    auto mStartCycle = new QLineEdit(this);
    auto mStopCycle  = new QLineEdit(this);
    auto mStartPass  = new QLineEdit(this);
    auto mStopPass   = new QLineEdit(this);

    mStartDate->setText("yyyy/mm/dd T hh:mm");
    mStopDate->setText("yyyy/mm/dd T hh:mm");
    mStartCycle->setText("0");
    mStopCycle->setText("0");
    mStartPass->setText("0");
    mStopPass->setText("0");

    QBoxLayout *dates_box = LayoutWidgets( Qt::Vertical, {
                                                 LayoutWidgets( Qt::Horizontal, { new QLabel( "Start Date" ), mStartDate } ),
                                                 LayoutWidgets( Qt::Horizontal, { new QLabel( "Stop Date" ),  mStopDate  } )
                                                } );

    QBoxLayout *cycles_box = LayoutWidgets( Qt::Vertical, {
                                                 LayoutWidgets( Qt::Horizontal, { new QLabel( "Start Cycle" ), mStartCycle } ),
                                                 LayoutWidgets( Qt::Horizontal, { new QLabel( "Stop Cycle" ),  mStopCycle  } )
                                                } );

    QBoxLayout *pass_box = LayoutWidgets( Qt::Vertical, {
                                                 LayoutWidgets( Qt::Horizontal, { new QLabel( "Start Pass" ), mStartPass } ),
                                                 LayoutWidgets( Qt::Horizontal, { new QLabel( "Stop Pass" ),  mStopPass  } )
                                                } );

    QBoxLayout *dateCyclePassValues_box = LayoutWidgets( Qt::Horizontal, {dates_box, nullptr, cycles_box, nullptr, pass_box});


    //   II.2 One-Click Time Filtering
    QFrame *lineHorizontal = WidgetLine( nullptr, Qt::Horizontal );
    auto one_click_label  = new QLabel( "One-Click Time Filtering" );

    //    Checkable menu items --> ATTENTION: are exclusive checkable menu items??
    auto last_month  = new QCheckBox( "Last Month" );
    auto last_year   = new QCheckBox( "Last Year" );
    auto last_cycle  = new QCheckBox( "Last Cycle" );
    QGroupBox *monthYearCycleGroup = CreateGroupBox(ELayoutType::Vertical, {last_month, last_year, last_cycle} );

    QFrame *lineVertical_1 = WidgetLine( nullptr, Qt::Vertical );
    QFrame *lineVertical_2 = WidgetLine( nullptr, Qt::Vertical );

    auto mRelativeStart  = new QLineEdit(this);
    auto mRelativeStop   = new QLineEdit(this);
    mRelativeStart->setText("0");
    mRelativeStop ->setText("0");
    QBoxLayout *relative_times = LayoutWidgets( Qt::Vertical, {
                                                 LayoutWidgets( Qt::Horizontal, { new QLabel( "Relative Start" ), mRelativeStart } ),
                                                 LayoutWidgets( Qt::Horizontal, { new QLabel( "Relative Stop" ),  mRelativeStop  } )
                                                } );

    auto reference_date       = new QCheckBox( "Reference Date" );
    auto reference_date_text  = new QLineEdit(this);
    reference_date_text->setText("yyyy/mm/dd");
    QBoxLayout *refDateBox = LayoutWidgets( Qt::Vertical, { reference_date, reference_date_text} );

    //    Adding previous widgets to this...
    AddTopGroupBox( ELayoutType::Vertical, {
                        dateCyclePassValues_box,
                        lineHorizontal,
                        one_click_label,
                        LayoutWidgets( Qt::Horizontal, {monthYearCycleGroup, lineVertical_1, relative_times, lineVertical_2, refDateBox})
                                            },"When", 4, 4, 4, 4, 4 );


    // III. "Total Nb Records Selected" Description group
    //
    auto mTotalRecordsSelected  = new QLineEdit(this);
    mTotalRecordsSelected->setText("0");
    mTotalRecordsSelected->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    QBoxLayout *TotalRecords_box = LayoutWidgets( Qt::Horizontal, { new QLabel( "Total Nb Records selected" ), mTotalRecordsSelected });
    TotalRecords_box->setAlignment(Qt::AlignRight);

    //    Adding previous widgets to this...
    AddTopGroupBox( ELayoutType::Horizontal, { TotalRecords_box});


    // IV. Filter name description group
    //
    auto mFilterName  = new QLineEdit(this);
    mFilterName ->setText("Filter Name");

    auto mNewFilter     = new QPushButton( "New Filter" );
    auto mDeleteFilter  = new QPushButton( "Delete Filter" );
    auto mSaveFilter    = new QPushButton( "Save Filter" );

    mOpenFilterCombo = new QComboBox;
    mOpenFilterCombo->setToolTip( "Open a Filter" );

    QBoxLayout *buttons_filter = LayoutWidgets( Qt::Horizontal, {  mFilterName, mNewFilter, mDeleteFilter, mSaveFilter, mOpenFilterCombo } );

    //    Adding previous widgets to this...
    AddTopGroupBox( ELayoutType::Horizontal, { WidgetLine( nullptr, Qt::Vertical ), buttons_filter });


    //AddTopWidget( where_box );
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
COperationsControls::COperationsControls( CDesktopManagerBase *manager, QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0 
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
	WaitCursor wait;

	auto ed = new CMapEditor( mWksd, this );
	auto subWindow = mManager->AddSubWindow( ed );
	subWindow->show();

	//openTestFile( t2q( mManager->mPaths.mWorkspacesDir + R"(/newWP/Displays/DisplayDisplays_New.par)" ) );
}

void COperationsControls::QuickPlot()
{
	WaitCursor wait;

    auto ed = new CPlotEditor( mWksd, nullptr, (CPlot*)nullptr, this );
    auto subWindow = mManager->AddSubWindow( ed );
    subWindow->show();

    //openTestFile( t2q( mManager->mPaths.mWorkspacesDir + R"(/newWP/Displays/DisplayDisplays_2.par)" ) );
}






//HAMMER SECTION
#include "new-gui/brat/DataModels/CmdLineProcessor.h"
#include "display/PlotData/WPlot.h"
#include "display/PlotData/Plot.h"
#include "display/PlotData/ZFXYPlot.h"


//L:\project\workspaces\newWP\Displays\DisplayDisplays_New.par

void COperationsControls::openTestFile( const QString &fileName )
{
	delete mCmdLineProcessor;
	mCmdLineProcessor = new CmdLineProcessor;

	const std::string s = q2a( fileName );
	const char *argv[] = { "", s.c_str() };
	try
	{
		if ( mCmdLineProcessor->Process( 2, argv ) )
		{
			if ( mCmdLineProcessor->isZFLatLon() )		// =================================== WorldPlot
			{
				for ( CObArray::const_iterator itGroup = mCmdLineProcessor->plots().begin(); itGroup != mCmdLineProcessor->plots().end(); itGroup++ )
				{
					CWPlot* wplot = dynamic_cast<CWPlot*>( *itGroup );
					if ( wplot == NULL )
						continue;

					auto ed = new CMapEditor( mCmdLineProcessor, wplot, this );
					auto subWindow = mManager->AddSubWindow( ed );
					subWindow->show();
				}
			}
			else if ( mCmdLineProcessor->isYFX() )		// =================================== XYPlot();
			{
				for ( CObArray::const_iterator itGroup = mCmdLineProcessor->plots().begin(); itGroup != mCmdLineProcessor->plots().end(); itGroup++ )
				{
					CPlot* plot = dynamic_cast<CPlot*>( *itGroup );
					if ( plot == NULL )
						continue;

					auto ed = new CPlotEditor( mWksd, mCmdLineProcessor, plot, this );
					auto subWindow = mManager->AddSubWindow( ed );
					subWindow->show();
				}
			}
			else if ( mCmdLineProcessor->isZFXY() )		// =================================== ZFXYPlot();
			{
				for ( CObArray::const_iterator itGroup = mCmdLineProcessor->plots().begin(); itGroup != mCmdLineProcessor->plots().end(); itGroup++ )
				{
					CZFXYPlot* zfxyplot = dynamic_cast<CZFXYPlot*>( *itGroup );
					if ( zfxyplot == NULL )
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
