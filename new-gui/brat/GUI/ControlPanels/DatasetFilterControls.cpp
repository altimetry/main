#include "new-gui/brat/stdafx.h"

#include "libbrathl/ProductNetCdf.h"

#include "DataModels/Model.h"
#include "DataModels/Workspaces/Workspace.h"
#include "DataModels/Filters/BratFilters.h"
#include "DataModels/Workspaces/Dataset.h"
#include "DataModels/PlotData/MapColor.h"

#include "GUI/ControlPanels/Dialogs/RegionSettingsDialog.h"
#include "GUI/DisplayWidgets/MapWidget.h"

#include "BratLogger.h"

#include "DatasetFilterControls.h"



/////////////////////////////////////////////////////////////////////////////////////
//							Dataset Filter Controls
/////////////////////////////////////////////////////////////////////////////////////


void CDatasetFilterControls::CreateWidgets()
{
    // I. Top buttons row
    //
    mNewFilter    = CreateToolButton( "", ":/images/OSGeo/filter_new.png", "<b>Create filter...</b><br>Create a new filter" );
    mRenameFilter = CreateToolButton( "", ":/images/OSGeo/filter_edit.png", "<b>Rename filter...</b><br>Change the name of selected filter" );
    mDeleteFilter = CreateToolButton( "", ":/images/OSGeo/filter_delete.png", "<b>Delete filter...</b><br>Delete the selected filter" );
    mSaveFilters  = CreateToolButton( "", ":/images/OSGeo/filter_save.png", "<b>Save filters</b><br>Save filter parameters." );

    mFiltersCombo = new QComboBox;
    mFiltersCombo->setToolTip( "Select Filter" );

    QWidget *buttons_row = CreateButtonRow( false, Qt::Horizontal, { mNewFilter, mRenameFilter, mDeleteFilter, mSaveFilters, nullptr, new QLabel( "Selected Filter"), mFiltersCombo } );

	AddTopWidget( buttons_row );
	AddTopSpace( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding );


    // II. "Where" Description group
    //
    mClearWhere = CreateToolButton( "", ":/images/OSGeo/filter_clearvalues.png", "Clear values" );
	auto *where_l = new QLabel( "Where" );
	QFont font = where_l->font();
	font.setBold( true );
	where_l->setFont( font );
    AddTopLayout( ELayoutType::Horizontal, { WidgetLine( nullptr, Qt::Horizontal ), where_l, WidgetLine( nullptr, Qt::Horizontal ), mClearWhere }, s, m, m, m, m );


    //    II.1 Buttons for region selection
    mShowAllAreas = CreateToolButton( "Show all", "", "<b>All</b><br>Show all areas." );
    mShowAllAreas->setCheckable( true );

    mRegionsCombo = new QComboBox;
    mRegionsCombo->setToolTip( "List of saved regions" );

    mRegionSettings = CreateToolButton( "", ":/images/OSGeo/region_settings.png", "<b>Region settings...</b><br>Configure region properties." );

    QBoxLayout *regions_layout = LayoutWidgets( Qt::Horizontal, { nullptr, mShowAllAreas, nullptr, mRegionsCombo, mRegionSettings  });


    //    II.2 Box of Areas with region buttons
    mAreasListWidget = new QListWidget( this );
    mAreasListWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    mNewArea = CreateToolButton( "", ":/images/OSGeo/area_new.png", "<b>Create area</b><br>Use the map selection feature to create a new area" );
    mAddKML  = CreateToolButton( "", ":/images/OSGeo/area_kml_add.png", "<b>Add area</b><br>Add area from KML file" );
    mAddMask = CreateToolButton( "", ":/images/OSGeo/area_mask_add.png", "<b>Add area</b><br>Add area from Mask file" );
    mRenameArea = CreateToolButton( "", ":/images/OSGeo/area_edit.png", "<b>Rename area</b><br>Change the name of selected area" );
    mDeleteArea = CreateToolButton( "", ":/images/OSGeo/area_remove.png", "<b>Delete area</b><br>Delete the selected area" );
    //mSaveArea = CreateToolButton( "", ":/images/OSGeo/area_save.png", "<b>Save area</b><br>Save values in selected area" );
	mAddMask->setVisible( false );		//TODO	MODIFY WHEN IMPLEMENTED


    QWidget *buttons_col = CreateButtonRow( false, Qt::Vertical, { mNewArea, mAddKML, mAddMask, mRenameArea, mDeleteArea, nullptr } );
    QBoxLayout *areas_layout = LayoutWidgets( Qt::Horizontal, { buttons_col, mAreasListWidget } );

    QGroupBox *areas_box = CreateGroupBox( ELayoutType::Vertical, { regions_layout, areas_layout }, "Areas", this );


    //    II.3 Coordinates (max and min values)
    mMaxLatEdit = new QLineEdit(this);
    mMaxLonEdit = new QLineEdit(this);
    mMinLatEdit = new QLineEdit(this);
    mMinLonEdit = new QLineEdit(this);

    mMaxLatEdit->setReadOnly( true );
    mMaxLonEdit->setReadOnly( true );
    mMinLatEdit->setReadOnly( true );
    mMinLonEdit->setReadOnly( true );

    QPalette *palette_readOnly = new QPalette();
    palette_readOnly->setColor(QPalette::Base, Qt::gray );
    palette_readOnly->setColor(QPalette::Text,Qt::black);

    mMaxLatEdit->setPalette(*palette_readOnly);
    mMaxLonEdit->setPalette(*palette_readOnly);
    mMinLatEdit->setPalette(*palette_readOnly);
    mMinLonEdit->setPalette(*palette_readOnly);

    QLabel *icon_north = new QLabel();
    icon_north->setPixmap( QPixmap(":/images/OSGeo/north-arrow.png") );

    QBoxLayout *coord_values = LayoutWidgets( Qt::Vertical, {
                                             LayoutWidgets( Qt::Horizontal, { nullptr } ),
                                             LayoutWidgets( Qt::Horizontal, { nullptr, new QLabel( "Max Lat (deg)" ), nullptr } ),
                                             LayoutWidgets( Qt::Horizontal, { nullptr, mMaxLatEdit, nullptr } ),
                                             LayoutWidgets( Qt::Horizontal, { new QLabel( "Min Lon (deg)" ), nullptr, new QLabel( "Max Lon (deg)" ) } ),
                                             LayoutWidgets( Qt::Horizontal, { mMinLonEdit, icon_north, mMaxLonEdit } ),
                                             LayoutWidgets( Qt::Horizontal, { nullptr, new QLabel( "Min Lat (deg)" ), nullptr } ),
                                             LayoutWidgets( Qt::Horizontal, { nullptr, mMinLatEdit, nullptr } ),
                                             LayoutWidgets( Qt::Horizontal, { nullptr } ),
                                               } );


    //    II.4 Adding previous widgets to this...
    QBoxLayout *areas_coord = LayoutWidgets( Qt::Horizontal, { areas_box, coord_values});

    mWhereBox = AddTopGroupBox(  ELayoutType::Vertical, { areas_coord } );
    AddTopSpace( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding );


    // III. "When" Description group
    //
    auto *when_l = new QLabel( "When" );
	when_l->setFont( font );
    mClearWhen = CreateToolButton( "", ":/images/OSGeo/filter_clearvalues.png", "Clear values" );
	AddTopLayout( ELayoutType::Horizontal, { WidgetLine( nullptr, Qt::Horizontal ), when_l, WidgetLine( nullptr, Qt::Horizontal ), mClearWhen }, s, m, m, m, m );

    //    III.1 Dates, Cycles and Pass (start and stop values)
    QDateTime minDateTime( QDate(1950, 1, 1), QTime(0, 0, 0));

    mStartTimeEdit = new QDateTimeEdit();                      mStopTimeEdit = new QDateTimeEdit();
    mStartTimeEdit->setCalendarPopup(true);                    mStopTimeEdit->setCalendarPopup(true);
    mStartTimeEdit->setDisplayFormat("yyyy.MM.dd hh:mm:ss");   mStopTimeEdit->setDisplayFormat("yyyy.MM.dd hh:mm:ss");
    mStartTimeEdit->setMinimumDateTime( minDateTime );         mStopTimeEdit->setMinimumDateTime( minDateTime );

    static QRegExpValidator *textValidator = new QRegExpValidator( QRegExp("[0-9]+"), this ); // only numeric letters

    mStartCycleEdit = new QLineEdit(this);           mStopCycleEdit  = new QLineEdit(this);
    mStartCycleEdit->setValidator( textValidator );  mStopCycleEdit->setValidator( textValidator );

    mStartPassEdit = new QLineEdit(this);            mStopPassEdit = new QLineEdit(this);
    mStartPassEdit->setValidator( textValidator );   mStopPassEdit->setValidator( textValidator );


    QBoxLayout *dates_box = LayoutWidgets( Qt::Vertical, {
                                                 LayoutWidgets( Qt::Horizontal, { new QLabel( "Start Date" ), mStartTimeEdit } ),
                                                 LayoutWidgets( Qt::Horizontal, { new QLabel( "Stop Date" ),  mStopTimeEdit } )
                                                } );

    QBoxLayout *cycles_box = LayoutWidgets( Qt::Vertical, {
                                                 LayoutWidgets( Qt::Horizontal, { new QLabel( "Start Cycle" ), mStartCycleEdit } ),
                                                 LayoutWidgets( Qt::Horizontal, { new QLabel( "Stop Cycle" ),  mStopCycleEdit  } )
                                                } );

    QBoxLayout *pass_box = LayoutWidgets( Qt::Vertical, {
                                                 LayoutWidgets( Qt::Horizontal, { new QLabel( "Start Pass" ), mStartPassEdit } ),
                                                 LayoutWidgets( Qt::Horizontal, { new QLabel( "Stop Pass" ),  mStopPassEdit  } )
                                                } );

    //   III.2 One-Click Time Filtering

#if defined (ONE_CLICK_TIME_FILTERING)

	auto *one_click_title = 
		LayoutWidgets( Qt::Horizontal, { WidgetLine( nullptr, Qt::Horizontal ), new QLabel( "One-Click Time Filtering" ), WidgetLine( nullptr, Qt::Horizontal ) }, nullptr, s, m, m, m, m );

    //    Checkable menu items --> ATTENTION: are exclusive checkable menu items??
    auto last_month  = new QCheckBox( "Last Month" );
    auto last_year   = new QCheckBox( "Last Year" );
    auto last_cycle  = new QCheckBox( "Last Cycle" );
    auto *month_year_cycle_layout = LayoutWidgets( Qt::Vertical, {last_month, last_year, last_cycle} );

    QFrame *lineVertical_1 = WidgetLine( nullptr, Qt::Vertical );
    QFrame *lineVertical_2 = WidgetLine( nullptr, Qt::Vertical );

    auto reference_date       = new QCheckBox( "Reference Date" );
    auto reference_date_text  = new QDateEdit;			  reference_date_text->setCalendarPopup(true);
    QBoxLayout *refDateBox = LayoutWidgets( Qt::Vertical, { reference_date, reference_date_text} );

    //    Adding previous widgets to this...

    AddTopLayout( ELayoutType::Vertical, 
	{
        mWhenBox,
        one_click_title,
        LayoutWidgets( Qt::Horizontal, { month_year_cycle_layout, lineVertical_1, relative_times, lineVertical_2, refDateBox }
	) }, s, m, m, m, m );
#endif

	mAbsoluteTimesBox = CreateGroupBox(  ELayoutType::Horizontal, { dates_box, nullptr, cycles_box, nullptr, pass_box} );

    mRelativeStart  = new QLineEdit(this);
    mRelativeStop   = new QLineEdit(this);
    mRelativeStart->setToolTip( "Insert the number of days later than reference date (or earlier if number of days is negative)" );
    mRelativeStop->setToolTip( "Insert the number of days later than reference date (or earlier if number of days is negative)" );

    mRefDateTimeEdit = new QDateTimeEdit();
    mRefDateTimeEdit->setCalendarPopup(true);
    mRefDateTimeEdit->setDisplayFormat("yyyy.MM.dd hh:mm:ss");
    mRefDateTimeEdit->setMinimumDateTime( minDateTime );
    mRefDateTimeEdit->setToolTip( "Edit reference date" );

    mUseCurrentDateTime = new QCheckBox( "Current DateTime" );
    mUseCurrentDateTime->setLayoutDirection( Qt::RightToLeft );
    mUseCurrentDateTime->setToolTip( "Use current datetime as reference date" );
    mUseCurrentDateTime->setStyleSheet( "QCheckBox:checked{color: black;} QCheckBox:unchecked {color: grey;} QCheckBox:disabled {color: grey;}");

    QBoxLayout *days_box = LayoutWidgets( Qt::Vertical, {
                                                 LayoutWidgets( Qt::Horizontal, { new QLabel( "Start" ), mRelativeStart } ),
                                                 LayoutWidgets( Qt::Horizontal, { new QLabel( "Stop" ),  mRelativeStop } )
                                                } );

    QBoxLayout *RefDate_box = LayoutWidgets( Qt::Vertical, {
                                                 mUseCurrentDateTime,
                                                 mRefDateTimeEdit
                                                } );

    mRelativeTimesBox = CreateGroupBox( ELayoutType::Horizontal, { days_box, nullptr, RefDate_box}, "Use Relative Time (days)" );
	mRelativeTimesBox->setCheckable( true );

    //LayoutWidgets( Qt::Horizontal, {dates_box, nullptr, cycles_box, nullptr, pass_box}, mWhenBox );
    mWhenBox = AddTopGroupBox( ELayoutType::Vertical, 
	{ 
		mAbsoluteTimesBox,
		mRelativeTimesBox
	}
	);



	AddTopSpace( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding );
    AddTopWidget( WidgetLine( nullptr, Qt::Horizontal ) );


    // IV. "Total Nb Records Selected" Description group
    //
    mTotalRecordsSelectedEdit = new QLineEdit( this );
	mTotalRecordsSelectedEdit->setReadOnly( true );
	mTotalRecordsSelectedEdit->setAlignment( Qt::AlignCenter );
    mTotalRecordsSelectedEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    QBoxLayout *TotalRecords_box = LayoutWidgets( Qt::Horizontal, { new QLabel( "Total Number of Records Selected" ), mTotalRecordsSelectedEdit });
    TotalRecords_box->setAlignment(Qt::AlignRight);

    //    Adding previous widgets to this...
    QGroupBox *Records_topBox = AddTopGroupBox( ELayoutType::Horizontal, { TotalRecords_box});
    Records_topBox->setDisabled( true );


    AddTopSpace( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding );

	Wire();
}


void CDatasetFilterControls::Wire()
{
    // Filter buttons //
    connect( mNewFilter, SIGNAL( clicked() ), this, SLOT( HandleNewFilter() ) );
	connect( mRenameFilter, SIGNAL( clicked() ), this, SLOT( HandleRenameFilter() ) );
	connect( mDeleteFilter, SIGNAL( clicked() ), this, SLOT( HandleDeleteFilter() ) );
	connect( mSaveFilters, SIGNAL( clicked() ), this, SLOT( HandleSaveFilters() ) );
	connect( mFiltersCombo, SIGNAL( currentIndexChanged(int) ), this, SLOT( HandleFiltersCurrentIndexChanged(int) ) );

    // Where widgets //
    connect( mRegionsCombo, SIGNAL( currentIndexChanged(int) ), this, SLOT( HandleRegionsCurrentIndexChanged(int) ) );
    connect( mAreasListWidget, SIGNAL( itemSelectionChanged() ), this, SLOT( HandleAreasSelectionChanged() ) );
    connect( mAreasListWidget, SIGNAL( itemChanged(QListWidgetItem*) ), this, SLOT( HandleAreaChecked(QListWidgetItem*) ) );

	connect( mClearWhere, SIGNAL( clicked() ), this, SLOT( HandleClearWhere() ) );

    connect( mShowAllAreas, SIGNAL( clicked() ), this, SLOT( HandleShowAllAreas() ) );
    connect( mRegionSettings, SIGNAL( clicked() ), this, SLOT( HandleRegionSettings() ) );

    connect( mNewArea, SIGNAL( clicked() ), this, SLOT( HandleNewArea() ) );
    connect( mAddKML, SIGNAL( clicked() ), this, SLOT( HandleAddKML() ) );
    connect( mAddMask, SIGNAL( clicked() ), this, SLOT( HandleAddMask() ) );
    connect( mRenameArea, SIGNAL( clicked() ), this, SLOT( HandleRenameArea() ) );
    connect( mDeleteArea, SIGNAL( clicked() ), this, SLOT( HandleDeleteArea() ) );

    // When widgets
	connect( mClearWhen, SIGNAL( clicked() ), this, SLOT( HandleClearWhen() ) );
    connect( mStartTimeEdit, SIGNAL( dateTimeChanged(const QDateTime&) ), this, SLOT( HandleStartDateTimeChanged(const QDateTime&) ) );
    connect( mStopTimeEdit, SIGNAL( dateTimeChanged(const QDateTime&) ), this, SLOT( HandleStopDateTimeChanged(const QDateTime&) ) );

    connect( mStartCycleEdit, SIGNAL( textEdited(const QString &) ), this, SLOT( HandleStartCycleChanged() ) );
    connect( mStopCycleEdit,  SIGNAL( textEdited(const QString &) ), this, SLOT( HandleStopCycleChanged() ) );
    connect( mStartPassEdit,  SIGNAL( textEdited(const QString &) ), this, SLOT( HandleStartPassChanged() ) );
    connect( mStopPassEdit,   SIGNAL( textEdited(const QString &) ), this, SLOT( HandleStopPassChanged() ) );

    connect( mRelativeTimesBox, SIGNAL( toggled( bool ) ), this, SLOT( HandleRelativeTimesBoxChecked( bool ) ) );
    mRelativeTimesBox->setChecked( false );

    connect( mRelativeStart,   SIGNAL( textEdited(const QString &) ), this, SLOT( HandleRelativeStartTimeChanged() ) );
    connect( mRelativeStop,    SIGNAL( textEdited(const QString &) ), this, SLOT( HandleRelativeStopTimeChanged() ) );
    connect( mRefDateTimeEdit, SIGNAL( dateTimeChanged(const QDateTime&) ), this, SLOT( HandleRelativeReferenceTimeChanged(const QDateTime&) ) );

    connect( mUseCurrentDateTime, SIGNAL( toggled( bool ) ), this, SLOT( HandleCurrentDateTimeBoxChecked( bool ) ) );

	connect( mMap, SIGNAL( NewRubberBandSelection( QRectF ) ), this, SLOT( HandleCurrentLayerSelectionChanged( QRectF ) ) );
}


//explicit
CDatasetFilterControls::CDatasetFilterControls( CModel &model, CDesktopManagerBase *manager, QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0
    : base_t( model, manager, parent, f )
	, mMap( manager->Map() )
	, mBratFilters( mModel.BratFilters() )
	, mBratAreas( mBratFilters.Areas() )
	, mBratRegions( mBratFilters.Regions() )
{
	CreateWidgets();

    if ( !ReloadFilters() )
	{
		setEnabled( false );
		LOG_WARN( "Brat filters cold not be loaded. Please check the filter configuration files." );
	}
}


bool CDatasetFilterControls::ReloadFilters()
{
	if ( !mModel.BratFiltersValid() )
	{
        return false;
	}

	//CArea area1( "Lake Baikal", { { 24.3938, 57.7512 }, { -9.49747, 36.0065 } } );
	//CArea area2( "Loch Ness", { { 4.8543, 79.7512 }, { -19.723452, -12.98705 } } );
	//mBratAreas.AddArea( area1 );		//this is a boolean function; deal with error if it fails
	//mBratAreas.AddArea( area2 );		//this is a boolean, deal with error if fails
	//mBratAreas.Save();				//this is a boolean, deal with error if fails

	//CRegion r1( "Grande Lisboa", { area1.Name(), area2.Name() } );
	//CRegion r2( "Lakes", { area2.Name(), area1.Name() } );
	//mBratRegions.AddRegion( r1 );	//this is a boolean, can fail
	//mBratRegions.AddRegion( r2 );	//this is a boolean, can fail
	//mBratRegions.Save();			//this is a boolean, can fail

    HandleFiltersCurrentIndexChanged( -1 ); // for updating all buttons and sections status

	FillFiltersCombo();
    FillRegionsCombo();

    // Default behavior: show all areas
    HandleShowAllAreas();

    //FillAreasList();

	return true;
}


void CDatasetFilterControls::FillFiltersCombo()
{
	mFiltersCombo->clear();

	auto const &filters = mBratFilters.FiltersMap();
	for ( auto const &filter : filters )
		mFiltersCombo->addItem( filter.first.c_str() );
}

void CDatasetFilterControls::FillRegionsCombo()
{
    mRegionsCombo->clear();

    auto &rmap = mBratRegions.RegionsMap();
    for ( auto &region_entry : rmap )
    {
        mRegionsCombo->addItem( region_entry.first.c_str() );
    }
}

void CDatasetFilterControls::FillAreasList()
{
    mAreasListWidget->clear();
    HandleAreasSelectionChanged(); // for updating buttons status

    // Fill all areas
    auto &amap = mBratAreas.AreasMap();
    for ( auto &area_entry : amap )
    {
        auto &area = area_entry.second;
        QListWidgetItem* item = new QListWidgetItem;
        item->setText( t2q(area.Name())  );
        item->setFlags( item->flags() | Qt::ItemIsUserCheckable );
        item->setCheckState( mFilter->FindArea(area.Name()) ? Qt::Checked : Qt::Unchecked );
        mAreasListWidget->addItem( item );
    }

    // Sort items (ascending order)
    mAreasListWidget->sortItems();

}


void CDatasetFilterControls::ShowOnlyAreasInRegion(int region_index)
{
    // Show areas of current region, hide remaining ones.
    std::string name = q2a( mRegionsCombo->itemText( region_index ) );
    CRegion *region = mBratRegions.Find( name );

    // 1. Dummy method!!
    // 1.1 Hide all areas
    for (int area_index = 0; area_index < mAreasListWidget->count(); ++area_index )
    {
        QListWidgetItem* area_item = mAreasListWidget->item( area_index );
        area_item->setHidden( true );
    }

    // 1.2 Unhide areas that belong to region
    for( auto area_name : *region )
    {
        auto area_list = mAreasListWidget->findItems( area_name.c_str(), Qt::MatchExactly );
        if (area_list.count() > 0 )
        {
            area_list.first()->setHidden( false );
        }
    }
}


void CDatasetFilterControls::HandleCurrentLayerSelectionChanged( QRectF box /*= QRectF()*/ )
{
    mMaxLatEdit->setText( n2q(  box.bottom() >  90  ?   90 : box.bottom() ) );
    mMaxLonEdit->setText( n2q(  box.right()  > 180  ?  180 : box.right()  ) );
    mMinLatEdit->setText( n2q(  box.top()    < -90  ?  -90 : box.top()    ) );
    mMinLonEdit->setText( n2q(  box.left()   < -180 ? -180 : box.left()   ) );

    // Disable button new area if there is no selection
    mNewArea->setDisabled( box.bottom() == 0 && box.right() == 0 &&
                           box.top() == 0    && box.left()  == 0     );
}


//public slots:
void CDatasetFilterControls::HandleWorkspaceChanged()
{
    auto *root = mModel.RootWorkspace();
    if ( root )
	{
		mWDataset = mModel.Workspace< CWorkspaceDataset >();
		mWOperation = mModel.Workspace< CWorkspaceOperation >();
	}
	else
	{
		mWDataset = nullptr;
		mWOperation = nullptr;
	}
	mFilter = nullptr;
	mDataset = nullptr;
	mFiltersCombo->clear();
	if ( root )
		ReloadFilters();
}


void CDatasetFilterControls::HandleNewFilter()
{
    auto result = ValidatedInputString( "Filter Name", mBratFilters.MakeNewName(), "New Filter..." );
	if ( !result.first )
		return;

	if ( !mBratFilters.AddFilter( result.second ) )
		SimpleMsgBox( "A filter with same name already exists." );
	else
	{
		FillFiltersCombo();
		mFiltersCombo->setCurrentIndex( mFiltersCombo->findText( result.second.c_str() ) );

	    emit FiltersChanged();
	}
}


void CDatasetFilterControls::HandleRenameFilter()
{
	assert__( mFilter );

    auto result = ValidatedInputString( "Filter Name", mFilter->Name(), "Rename Filter..." );
	if ( !result.first )
		return;

	if ( !mBratFilters.RenameFilter( mFilter->Name(), result.second ) )
	{
		SimpleMsgBox( "A filter with same name already exists." );
		return;
	}

	CStringArray operation_names;
	if ( mWOperation->UseFilter( mFilter->Name(), &operation_names ) )
	{
		SimpleErrorBox( "Filter '" + mFilter->Name() + "' cannot be renamed. It is used by the following operations:\n\n" + operation_names.ToString("\n") );
		return;
	}

	FillFiltersCombo();
	mFiltersCombo->setCurrentIndex( mFiltersCombo->findText( result.second.c_str() ) );

	emit FiltersChanged();
}


void CDatasetFilterControls::HandleDeleteFilter()
{
	assert__( mFilter );

	if ( SimpleQuestion( "Are you sure you want to delete filter '" + mFilter->Name() + "' ?" ) )
	{
		CStringArray operation_names;
		if ( mWOperation->UseFilter( mFilter->Name(), &operation_names ) )
		{
			SimpleErrorBox( "Filter '" + mFilter->Name() + "' cannot be deleted. It is used by the following operations:\n\n" + operation_names.ToString( "\n" ) );
			return;
		}

        if ( !mBratFilters.DeleteFilter( mFilter->Name() ) )
        {
            SimpleErrorBox( "Filter '" + mFilter->Name() + "' was not found!" );
            return;
        }

        FillFiltersCombo();
        mFiltersCombo->setCurrentIndex( 0 );

        emit FiltersChanged();
	}
}


void CDatasetFilterControls::HandleSaveFilters()
{
    assert__( mFilter );

	if ( !mBratFilters.Save() )
		SimpleWarnBox( "There was a problem saving filters to '" + mBratFilters.FilePath() + "'. Some information could be lost or damaged." );
}


void CDatasetFilterControls::HandleFiltersCurrentIndexChanged( int filter_index ) 
{
	mRenameFilter->setEnabled( filter_index >= 0 );
	mDeleteFilter->setEnabled( filter_index >= 0 );
	mSaveFilters->setEnabled( filter_index >= 0 );
    mWhereBox->setEnabled( filter_index >= 0 );
    mWhenBox->setEnabled( filter_index >= 0 );
    mClearWhere->setEnabled( filter_index >= 0 );
    mClearWhen->setEnabled( filter_index >= 0 );

	if ( filter_index < 0 )
	{
        return;
    }

	std::string name = q2a( mFiltersCombo->itemText( filter_index ) );
	CBratFilter *filter = mBratFilters.Find( name );
	if ( filter == mFilter )
		return;

	mFilter = filter;

    // Refresh areas list (checked/unchecked status)
    FillAreasList();

    // Update Max/Min Lat and Lon and refresh "NewArea" button status
    HandleCurrentLayerSelectionChanged();

    // Check/Uncheck Relative Time section and Updates When section
    mRelativeTimesBox->setChecked( mFilter->UsingRelativeTimes() );
    updateDateWidgets();
    updateCyclePassWidgets();
    updateRelativeTimeWidgets();
}


void CDatasetFilterControls::HandleRegionsCurrentIndexChanged( int region_index )
{
    if ( region_index < 0 )
    {
        return;
    }

    mShowAllAreas->setChecked( false );
    ShowOnlyAreasInRegion( region_index );

//    std::string name = q2a( mRegionsCombo->itemText( region_index ) );
//    CRegion *region = mBratRegions.Find( name );
//    Q_UNUSED( region );

}


void CDatasetFilterControls::HandleAreasSelectionChanged()
{
    QListWidgetItem *item = mAreasListWidget->currentItem();

    mRenameArea->setEnabled( item != nullptr );
    mDeleteArea->setEnabled( item != nullptr );
    mMap->RemoveAreaSelection(); // clean the map selection

    if ( item == nullptr )
    {
        return;
    }

    // Fill Min, Max values of Lon and lat
    CArea *area = mBratAreas.Find( item->text().toStdString() );

    mMinLonEdit->setText( n2q( area->GetLonMin() ) );
    mMaxLonEdit->setText( n2q( area->GetLonMax()) );
    mMinLatEdit->setText( n2q( area->GetLatMin() ) );
    mMaxLatEdit->setText( n2q( area->GetLatMax() ) );

	mMap->SelectArea( area->GetLonMin(), area->GetLonMax(), area->GetLatMin(), area->GetLatMax() );
}


void CDatasetFilterControls::HandleClearWhere()
{
    mFilter->DeleteAllAreas();

    // Refresh areas list (checked/unchecked status)
    FillAreasList();

    // Update Max/Min Lat and Lon and refresh "NewArea" button status
    HandleCurrentLayerSelectionChanged();
}


void CDatasetFilterControls::HandleClearWhen()
{
    mFilter->SetDefaultValues();

    updateDateWidgets();
    updateCyclePassWidgets();
    updateRelativeTimeWidgets();
}


void CDatasetFilterControls::HandleShowAllAreas()
{
    mRegionsCombo->setCurrentIndex( -1 );
    mShowAllAreas->setChecked( true );

    // Unhide all areas
    for (int area_index = 0; area_index < mAreasListWidget->count(); ++area_index )
    {
        QListWidgetItem* area_item = mAreasListWidget->item( area_index );
        area_item->setHidden( false );
    }
}

void CDatasetFilterControls::HandleRegionSettings()
{
    CRegionSettingsDialog dlg( this, mBratRegions, mBratAreas );
    if ( dlg.exec() == QDialog::Rejected )
    {
        // Fill with new user regions
        FillRegionsCombo();

        // Default behaviour: show all areas
        HandleShowAllAreas();
    }
}


void CDatasetFilterControls::HandleNewArea()
{
    auto result = ValidatedInputString( "Area Name", mBratAreas.MakeNewName(), "New Area..." );
    if ( !result.first )
        return;

    if ( !mBratAreas.AddArea( result.second ) )
        SimpleMsgBox( "A area with same name already exists." );
    else
    {
        // Add all vertex of selection to new area
        CArea *area = mBratAreas.Find( result.second );

        double lat_max = s2n< double >( q2a( mMaxLatEdit->text() ) );
        double lat_min = s2n< double >( q2a( mMinLatEdit->text() ) );
        double lon_max = s2n< double >( q2a( mMaxLonEdit->text() ) );
        double lon_min = s2n< double >( q2a( mMinLonEdit->text() ) );

        area->AddVertex( lon_min, lat_max );      area->AddVertex( lon_max, lat_max );
        area->AddVertex( lon_min, lat_min );      area->AddVertex( lon_max, lat_min );

        // Save all areas
        SaveAllAreas();

        // Add new area to areas list
        QListWidgetItem* item = new QListWidgetItem;
        item->setText( t2q( result.second ) );
        item->setFlags( item->flags() | Qt::ItemIsUserCheckable );
        item->setCheckState( Qt::Unchecked );
        mAreasListWidget->addItem( item );
        mAreasListWidget->setCurrentItem( item ); //mAreasListWidget->findItems( result.second.c_str(), Qt::MatchExactly ).first() );
    }
}

void CDatasetFilterControls::HandleAddKML()
{
	static std::string kml_path = mModel.BratPaths().UserDataDirectory();
	
    // Ask user to choose a KML file
	QString path = BrowseFileWithExtension( this, "Open from KML File", kml_path.c_str(), "kml", "Keyhole Markup Language" );

	if ( !path.isEmpty() )
	{
        kml_path = q2a( path );  // save path for next usages

        // Open Layer and get bounding_box
		QgsRectangle bounding_box;
		if ( !CMapWidget::OpenLayer( this, path, bounding_box ) )
			return;

		LOG_WARN( bounding_box.toString() );
        if ( bounding_box.isEmpty() )
            return;

        // Ask user to provide an area name
        auto result = ValidatedInputString( "Area Name", mBratAreas.MakeNewName(), "New Area from KML..." );
        if ( !result.first )
            return;

        if ( !mBratAreas.AddArea( result.second ) )
        {
            SimpleMsgBox( "A area with same name already exists." );
            return;
        }

        // Add all vertex of bounding_box to new area and show vertex values
        CArea *area = mBratAreas.Find( result.second );

        double lat_max = bounding_box.yMaximum();
        double lat_min = bounding_box.yMinimum();
        double lon_max = bounding_box.xMaximum();
        double lon_min = bounding_box.xMinimum();

        area->AddVertex( lon_min, lat_max );      area->AddVertex( lon_max, lat_max );
        area->AddVertex( lon_min, lat_min );      area->AddVertex( lon_max, lat_min );

        mMaxLatEdit->setText( n2q( lat_max ) );
        mMaxLonEdit->setText( n2q( lon_max ) );
        mMinLatEdit->setText( n2q( lat_min ) );
        mMinLonEdit->setText( n2q( lon_min ) );

        // Save all areas
        SaveAllAreas();

        // Add new area to areas list
        QListWidgetItem* item = new QListWidgetItem;
        item->setText( t2q( result.second ) );
        item->setFlags( item->flags() | Qt::ItemIsUserCheckable );
        item->setCheckState( Qt::Unchecked );
        mAreasListWidget->addItem( item );
        mAreasListWidget->setCurrentItem( item ); //mAreasListWidget->findItems( result.second.c_str(), Qt::MatchExactly ).first() );
	}
}

void CDatasetFilterControls::HandleAddMask()
{
    // TODO
    BRAT_NOT_IMPLEMENTED;
    //Save all areas
//    SaveAllAreas();
}

void CDatasetFilterControls::HandleRenameArea()
{
    std::string area_name = mAreasListWidget->currentItem()->text().toStdString();

    // Check if area is used by any filter
    auto const &filters = mBratFilters.FiltersMap();
    for ( auto const &filter : filters )
        if ( filter.second.FindArea( area_name ) )
        {
            SimpleWarnBox( "Cannot rename area '" + area_name +  "', it is used by filter '" + filter.first + "'." );
            return;
        }

    // Rename area
    auto result = ValidatedInputString( "Area Name", area_name, "Rename Area..." );
    if ( !result.first )
        return;

    if ( !mBratAreas.RenameArea( area_name, result.second ) )
        SimpleMsgBox( "A area with same name already exists." );
    else
    {
        QListWidgetItem *area_item = mAreasListWidget->currentItem();
        area_item->setText( result.second.c_str() );

        //Save all areas
        SaveAllAreas();
    }
}

void CDatasetFilterControls::HandleDeleteArea()
{
    std::string area_name = mAreasListWidget->currentItem()->text().toStdString();


    if ( SimpleQuestion( "Are you sure you want to delete area '" + area_name + "' ?" ) )
    {
        // Check if area is used by any filter
        auto const &filters = mBratFilters.FiltersMap();
        for ( auto const &filter : filters )
            if ( filter.second.FindArea( area_name ) )
            {
                SimpleWarnBox( "Cannot delete area '" + area_name +  "', it is used by filter '" + filter.first + "'." );
                return;
            }

        // Delete area
        if ( !mBratAreas.DeleteArea( area_name ) )
            SimpleErrorBox( "Area '" + area_name + "' was not found!" );
        else
        {
            delete mAreasListWidget->currentItem();
            mAreasListWidget->setCurrentRow(0);

            //Save all areas
            SaveAllAreas();
        }
    }
}


void CDatasetFilterControls::HandleAreaChecked(QListWidgetItem *area_item)
{
    if ( area_item->checkState() == Qt::Checked )
    {
        mFilter->AddArea( area_item->text().toStdString()  );
    }
    else
    {
        mFilter->RemoveArea( area_item->text().toStdString() );
    }

	emit FilterCompositionChanged( mFilter->Name() );
}


void CDatasetFilterControls::SaveAllAreas()
{
    if ( !mBratAreas.Save() )
        SimpleWarnBox( "There was a problem saving areas to '" + mBratAreas.FilePath() + "'. Some information could be lost or damaged." );
}



void CDatasetFilterControls::HandleStartDateTimeChanged(const QDateTime &start_datetime)
{
    mFilter->StartTime() = start_datetime;

    // When user selects a data, the cycle and pass are automatically deleted (Only user dates are used in When criteria)
    mFilter->SetDefaultCyclePassValues();
    updateDateWidgets();
    updateCyclePassWidgets();
}


void CDatasetFilterControls::HandleStopDateTimeChanged(const QDateTime &stop_datetime)
{
    mFilter->StopTime() = stop_datetime;

    // When user selects a data, the cycle and pass are automatically deleted (Only user dates are used in When criteria)
    mFilter->SetDefaultCyclePassValues();
    updateDateWidgets();
    updateCyclePassWidgets();
}


void CDatasetFilterControls::HandleStartCycleChanged()
{
    ValidateAndStoreValue( mStartCycleEdit,              // Text box
                           mFilter->StartCycle(),        // Filter value
                           mFilter->StopCycle(),         // Default Param
                           CTools::m_defaultValueINT32,  // Min
                           mFilter->StopCycle()  );      // Max (should be < StopCycle)

    // When user selects a cycle, the dates are automatically deleted (Only user cycle/pass are used in When criteria) 
    mFilter->SetDefaultDateValues();
    updateDateWidgets();
    updateCyclePassWidgets();
}


void CDatasetFilterControls::HandleStopCycleChanged()
{
    ValidateAndStoreValue( mStopCycleEdit,                // Text box
                           mFilter->StopCycle(),          // Filter value
                           mFilter->StartCycle(),         // Default Param
                           mFilter->StartCycle(),         // Min (should be > StartCycle)
                           CTools::m_defaultValueINT32 ); // Max

    // When user selects a cycle, the dates are automatically deleted (Only user cycle/pass are used in When criteria)
    mFilter->SetDefaultDateValues();
    updateDateWidgets();
    updateCyclePassWidgets();
}


void CDatasetFilterControls::HandleStartPassChanged()
{
    ValidateAndStoreValue( mStartPassEdit,               // Text box
                           mFilter->StartPass(),         // Filter value
                           mFilter->StopPass(),          // Default Param
                           CTools::m_defaultValueINT32,  // Min
                           mFilter->StopPass()  );       // Max (should be < StopPass)

    // When user selects a cycle, the dates are automatically deleted (Only user cycle/pass are used in When criteria)
    mFilter->SetDefaultDateValues();
    updateDateWidgets();
    updateCyclePassWidgets();
}


void CDatasetFilterControls::HandleStopPassChanged()
{
    ValidateAndStoreValue( mStopPassEdit,                 // Text box
                           mFilter->StopPass(),           // Filter value
                           mFilter->StartPass(),          // Default Param
                           mFilter->StartPass(),          // Min (should be > StartPass)
                           CTools::m_defaultValueINT32 ); // Max

    // When user selects a cycle, the dates are automatically deleted (Only user cycle/pass are used in When criteria)
    mFilter->SetDefaultDateValues();
    updateDateWidgets();
    updateCyclePassWidgets();
}


void CDatasetFilterControls::HandleRelativeTimesBoxChecked( bool checked )
{
    mAbsoluteTimesBox->setEnabled( !checked );

    if ( mFilter )
    {
        if ( !checked )
        {
            mFilter->DisableRelativeTimes();
        }
        else
        {
            // When user selects a relative data, Cycle&Pass are automatically deleted (Only user dates are used in When criteria)
            mFilter->SetDefaultCyclePassValues();

            // For updating current DateTime
            if ( mFilter->UseCurrentTime() ) { mFilter->RelativeReferenceTime() = QDateTime::currentDateTime(); }
            mRefDateTimeEdit->setDateTime( mFilter->RelativeReferenceTime() );
        }
        updateDateWidgets();
        updateCyclePassWidgets();
        updateRelativeTimeWidgets();
    }
}


void CDatasetFilterControls::HandleRelativeStartTimeChanged()
{
    ValidateAndStoreValue( mRelativeStart,                // Text box                                                   -------------------------------------
                           mFilter->RelativeStartDays(),  // Filter value          Start  Stop    Ref    Start  Stop   | X1 and X2 are the number of days    |
                           mFilter->RelativeStopDays(),   // Default Param         __|_____|_______|_______|_____|__   | later [or earlier if it's negative] |
                           CTools::m_defaultValueINT32,   // Min                     |     |               |     |     | than Ref datetime)                  |
                           mFilter->RelativeStopDays() ); // Max (should be < X2)   -X1   -X2              X1    X2     -------------------------------------

    mFilter->Relative2AbsoluteTimes();

    updateRelativeTimeWidgets();
    updateDateWidgets();
}


void CDatasetFilterControls::HandleRelativeStopTimeChanged()
{
    ValidateAndStoreValue( mRelativeStop,                  // Text box                                                   -------------------------------------
                           mFilter->RelativeStopDays(),    // Filter value          Start  Stop    Ref    Start  Stop   | X1 and X2 are the number of days    |
                           mFilter->RelativeStartDays(),   // Default Param         __|_____|_______|_______|_____|__   | later [or earlier if it's negative] |
                           mFilter->RelativeStartDays(),   // Min (should be > X1)    |     |               |     |     | than Ref datetime)                  |
                           CTools::m_defaultValueINT32  ); // Max                    -X1   -X2              X1    X2     -------------------------------------

    mFilter->Relative2AbsoluteTimes();

    updateRelativeTimeWidgets();
    updateDateWidgets();
}


void CDatasetFilterControls::HandleCurrentDateTimeBoxChecked( bool checked )
{
    mRefDateTimeEdit->setEnabled( !checked );

    if ( mFilter )
    {
        mFilter->UseCurrentTime() = checked;

        // Re-calculate start and stop dates using new reference date
        mFilter->Relative2AbsoluteTimes();
        updateDateWidgets();
        updateRelativeTimeWidgets();
    }
}


void CDatasetFilterControls::HandleRelativeReferenceTimeChanged(const QDateTime &ref_datetime)
{
    mFilter->RelativeReferenceTime() = ref_datetime;

    // Re-calculate start and stop dates using new reference date
    mFilter->Relative2AbsoluteTimes();
    updateDateWidgets();
}


// ---------------------------------------------------------------------------------------
// This method is used to validate the (start/stop) Cycle, Pass and Relative time values.
// The value in TextBox is validated and set into filter object.
// Case the value is outside the min/max limits, the ParamDef is automatically assigned.
void CDatasetFilterControls::ValidateAndStoreValue(QLineEdit *TextBox, int &ValueInFilter, int ParamDef, int min, int max)
{
    int new_value;
    QString value_str = TextBox->text();

    if ( value_str.isEmpty() )
        setDefaultValue( new_value );
    else
    {
        new_value = value_str.toInt();

        if ( (!isDefaultValue(min)  &&  new_value < min) ||
             (!isDefaultValue(max)  &&  new_value > max)    )
            new_value = ParamDef;
    }

    TextBox->setText( isDefaultValue(new_value) ? "" : n2q(new_value) );
    ValueInFilter = new_value;
}


void CDatasetFilterControls::updateDateWidgets()
{
    // BLOCK SIGNALS //
    // Signals are blocked, otherwise HandleStartDateTimeChanged or HandleStopDateTimeChanged are called
    mStartTimeEdit->blockSignals( true );
    mStopTimeEdit->blockSignals( true );

    // UPDATE start/stop dates and max/min allowed
    mStartTimeEdit->setMaximumDateTime( mFilter->StopTime() ); // Stop_datetime defines the maximum allowed start_datetime
    mStopTimeEdit->setMinimumDateTime( mFilter->StartTime() ); // Start_datetime defines the minimum allowed stop_datetime
    mStartTimeEdit->setDateTime( mFilter->StartTime() );
    mStopTimeEdit->setDateTime( mFilter->StopTime() );

    // SET READ_ONLY PALETTE
    bool markAsReadOnly = ( !isDefaultValue(mFilter->StartCycle()) &
                            !isDefaultValue(mFilter->StopCycle())  &
                            !isDefaultValue(mFilter->StartPass())  &
                            !isDefaultValue(mFilter->StopPass())
                           );

    QPalette *palette_readOnly = new QPalette();
    if ( markAsReadOnly )
    {
        palette_readOnly->setColor(QPalette::Base, Qt::gray );
        palette_readOnly->setColor(QPalette::Text,Qt::black);
    }
    mStartTimeEdit->setPalette( *palette_readOnly );
    mStopTimeEdit->setPalette( *palette_readOnly );

    // UNBLOCK SIGNALS //
    mStartTimeEdit->blockSignals( false );
    mStopTimeEdit->blockSignals( false );
}


void CDatasetFilterControls::updateCyclePassWidgets()
{
    // UPDATE start/stop Cycle and Pass
    mStartCycleEdit->setText( isDefaultValue(mFilter->StartCycle()) ? "" : n2q(mFilter->StartCycle()) );
    mStopCycleEdit->setText(  isDefaultValue(mFilter->StopCycle())  ? "" : n2q(mFilter->StopCycle()) );
    mStartPassEdit->setText( isDefaultValue(mFilter->StartPass()) ? "" : n2q(mFilter->StartPass()) );
    mStopPassEdit->setText(  isDefaultValue(mFilter->StopPass())  ? "" : n2q(mFilter->StopPass()) );

    // SET READ_ONLY PALETTE
    bool markAsReadOnly = ( isDefaultValue(mFilter->StartCycle()) ||
                            isDefaultValue(mFilter->StopCycle())  ||
                            isDefaultValue(mFilter->StartPass())  ||
                            isDefaultValue(mFilter->StopPass())
                           );
    QPalette *palette_readOnly = new QPalette();
    if ( markAsReadOnly )
    {
        palette_readOnly->setColor(QPalette::Base, Qt::gray );
        palette_readOnly->setColor(QPalette::Text,Qt::black);
    }
    mStartCycleEdit->setPalette( *palette_readOnly );
    mStopCycleEdit->setPalette( *palette_readOnly );
    mStartPassEdit->setPalette( *palette_readOnly );
    mStopPassEdit->setPalette( *palette_readOnly );
}

void CDatasetFilterControls::updateRelativeTimeWidgets()
{
    // UPDATE start/stop Relative Times
    mRelativeStart->setText( isDefaultValue(mFilter->RelativeStartDays()) ? "" : n2q(mFilter->RelativeStartDays()) );
    mRelativeStop->setText(  isDefaultValue(mFilter->RelativeStopDays() ) ? "" : n2q(mFilter->RelativeStopDays()) );
    mRefDateTimeEdit->setDateTime( mFilter->RelativeReferenceTime() );

    // SET Checked/Unchecked
    mUseCurrentDateTime->setChecked( mFilter->UseCurrentTime() );
}


//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//					Satellite Tracks Processing TODO change place???
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

//CAliasesDictionary* GetAliasesDictionary()
//{
//	try
//	{
//		return CAliasesDictionary::GetInstance();
//	}
//	catch ( CException& e )
//	{
//		LOG_WARN( "The aliases dictionary can't be loaded properly - Native Error is " + std::string( e.what() ) );
//	}
//	return nullptr;
//}
//
//std::string FindDefaultRecord( const CProduct *product )
//{
//	static CAliasesDictionary *ad = GetAliasesDictionary();
//	static const std::string default_record = "data";
//
//	assert__( product );
//
//	if ( ad )
//	{
//		CXmlNode* node = ad->FindProductNode( product->GetProductClass(), false );
//		if ( node != nullptr )
//		{
//			CObArray defaultRecordsArray;
//			ad->GetDefaultRecords( node, defaultRecordsArray );
//			for ( CObArray::const_iterator it = defaultRecordsArray.begin(); it != defaultRecordsArray.end(); it++ )
//			{
//				CDefaultRecord* dr = dynamic_cast<CDefaultRecord*>( *it );
//				if ( dr && ! dr->GetName().empty() )
//					return dr->GetName();
//			}
//		}
//	}
//
//	return default_record;
//}



// Here is an expression using the  alias 'swh': %{sig_wave_height} -->  'swh' field in tha aliases dictionary (Jason-1)
//Expressions[1]	= "%{sig_wave_height}";
//Units[1]		= "count";
//
// Here is an expression calling the 'BratAlgoFilterMedianAtp' algorithm
//Expressions[1] = "exec(\"BratAlgoFilterMedianAtp\", %{sig_wave_height}, 7, 1, 0)";
//Units[1]		= "count";
//
int ReadTrack( bool can_use_alias, const std::string &path, const std::string &record, double *&x, size_t &sizex, double *&y, size_t &sizey, double *&z, size_t &sizez, int nfields )
{
    static const char *Units[] = { "count", "count", "second" };
	static const int default_nfields = 3;

	const char *Expressions[] = 
	{ 
		can_use_alias ? lon_alias().c_str()	: lon_name().c_str(),
		can_use_alias ? lat_alias().c_str()	: lat_name().c_str(),
		can_use_alias ? time_alias().c_str()	: time_name().c_str()
	};
	char *Names[] = { const_cast<char*>( path.c_str() ) };

    double	*Data[ default_nfields ]	= { nullptr, nullptr, nullptr };
    int32_t	Sizes[ default_nfields ]	= { -1, -1, -1 };

    size_t	ActualSize;
	//int nfields = default_nfields;


    int ReturnCode = CProduct::ReadData(
                1, Names,
                record.c_str(),		//"data",                
                nullptr,			//"latitude > 20 && latitude < 30",
                nfields,
                const_cast< char** >( Expressions ),
                const_cast< char** >( Units ),
                Data,
                Sizes,
                &ActualSize,
                1,
                0,
                defaultValue< double >()
                );

    LOG_TRACEstd( "Return code          : " + n2s<std::string>( ReturnCode ) );
    LOG_TRACEstd( "Actual number of data: " + n2s<std::string>( ActualSize ) );

    x = Data[ 0 ];
    y = Data[ 1 ];
    z = Data[ 2 ];

    sizez = sizex = sizey = ActualSize;

    return ReturnCode;
}


//static 
const std::string& CDesktopControlsPanel::FindAliasValue( CProduct *product, const std::string &alias_name )
{
	auto *alias = product->GetAlias( alias_name );
	if ( !alias )
		alias = product->GetAlias( ToLowerCopy( alias_name ) );	//TODO confirm aliases case sensitiveness

	if ( alias )
	{
		return alias->GetValue();
	}

	return empty_string< std::string >();
}

//static
CField* CDesktopControlsPanel::FindField( CProduct *product, const std::string &name, bool &alias_used, std::string &field_error_msg )
{
	std::string record;
	if ( product->IsNetCdfOrNetCdfCFProduct() )
		//record = CProductNetCdf::m_virtualRecordName;		//this is done for COperation; should we do it HERE????
		record = "";
	else
	{
		auto *aliases = product->GetAliases();
		if ( aliases )
			record = aliases->GetRecord();
	}

	CField *field = nullptr;
	alias_used = true;

	std::string value = FindAliasValue( product, name );
	if ( !value.empty() )
	{
		field = product->FindFieldByName( value, false, &field_error_msg );		//true: throw on failure
		//guessing
		if ( !field && !record.empty() )
			field = product->FindFieldByName( value, record, false );	//true: throw on failure
		if ( !field )
			field = product->FindFieldByInternalName( value, false );	//true: throw on failure
	}

	if ( !field )
	{
		alias_used = false;
		field = product->FindFieldByName( name, false, &field_error_msg );		//true: throw on failure
		//still guessing
		if ( !field && !record.empty() )
			field = product->FindFieldByName( name, record, false );	//true: throw on failure
		if ( !field )
			field = product->FindFieldByInternalName( name, false );	//true: throw on failure
		if (!field)
			field = product->FindFieldByName( ToLowerCopy( name ), false, &field_error_msg );
	}
	return field;
}


//static
std::pair<CField*, CField*> CDesktopControlsPanel::FindLonLatFields( CProduct *product, bool &alias_used, std::string &field_error_msg )
{
	std::pair<CField*, CField*> fields;
	fields.first = FindField( product, lon_name(), alias_used, field_error_msg );
	fields.second = FindField( product, lat_name(), alias_used, field_error_msg );
	return fields;
}


//static
CField* CDesktopControlsPanel::FindTimeField( CProduct *product, bool &alias_used, std::string &field_error_msg )
{
	return FindField( product, time_name(), alias_used, field_error_msg );
}



void CDatasetFilterControls::HandleDatasetChanged( CDataset *dataset )
{    
    static CMapColor &mc = CMapColor::GetInstance();        Q_UNUSED(mc);

    //lambdas

    auto debug_log = []( const std::string &msg )
    {
        LOG_TRACEstd( msg  );
        qApp->processEvents();
    };


    //function body

	mMap->RemoveTracksLayerFeatures();
	mTotalRecordsSelectedEdit->setText( "" );

    if ( !mAutoSatelliteTrack || !dataset || mDataset != dataset )
    {
        if ( mDataset == dataset )
            return;

        mDataset = dataset;
        if ( !mDataset || !mAutoSatelliteTrack )
            return;
    }

	std::string error_msg;
	int total_records = -1;

	mMap->setRenderFlag( false );
    std::vector< std::string > paths;    mDataset->GetFiles( paths );
	for ( auto &path : paths )
	{
		WaitCursor wait;
		try
		{
            CProduct *product = mDataset->OpenProduct( path );
            //total_records += product->GetNumberOfRecords();
			bool skip_iteration = !product || !product->HasAliases();
			if ( skip_iteration )
			{
				LOG_WARN( "Aliases not supported: reading file " + path );
			}
			const bool is_netcdf = product->IsNetCdfOrNetCdfCFProduct();

			std::string record;
			if ( is_netcdf )
				record = CProductNetCdf::m_virtualRecordName;		//TODO this is done for COperation; should we do it HERE????
			else
			{
				auto *aliases = product->GetAliases();
				if ( aliases )
					record = aliases->GetRecord();
			}

			std::string field_error_msg;
			bool alias_used;
			std::pair<CField*, CField*> fields = FindLonLatFields( product, alias_used, field_error_msg );
			CField *lon = fields.first;
			CField *lat = fields.second;
			if ( !lon || !lat )
			{
				skip_iteration = true;
				LOG_WARN( field_error_msg + " - File " + path );
			}
			CField *time = FindTimeField( product, alias_used, field_error_msg );

            auto expected_lon_dim = lon ? lon->GetDim()[ 0 ] : 0;
			auto expected_lat_dim = lat ? lat->GetDim()[ 0 ] : 0;
            auto expected_time_dim = time ? time->GetDim()[ 0 ] : 0;                Q_UNUSED( expected_time_dim );
			if ( expected_lon_dim != expected_lat_dim )
			{
				skip_iteration = true;
				LOG_WARN( "Different latitude/longitude dimensions in file " + path );
			}
			if ( !time )
			{
				skip_iteration = true;
				LOG_WARN( "No time data found in file " + path + "; cannot display track." );
			}

			LOG_WARN( 
				"%{lat}==" + ( lat ? lat->GetName() : "" ) +
				" %{lon}==" + ( lon ? lon->GetName() : "" ) +
				" %{time}==" + ( time ? time->GetName() : "") );

			auto ref_date = REF19500101;	//Use ReadData ref date instead of product->GetRefDate();
            delete product;
            if ( skip_iteration )
            {
                continue;
            }

            /*
            p->Open( q2a( path ), "data" );
			CStringList FieldsToRead;
			FieldsToRead.push_back("lat");
			FieldsToRead.push_back("lon");
            p->SetListFieldToRead( FieldsToRead, false );
            // Get the number of record for the default record name (set in Open method of CProduct above)
			int32_t nRecords = p->GetNumberOfRecords();

			for ( int32_t iRecord = 0; iRecord < nRecords; iRecord++ )
            {
			//Read fields for the record name  (list of field and record name are set in Open method of CProduct above)
			p->ReadBratRecord( iRecord );
			}
			*/

            size_t lon_dim = 0;
			size_t lat_dim = 0;
			size_t time_dim = 0;
			double *lonv = nullptr;
			double *latv = nullptr;
			double *timev = nullptr;

			debug_log( "About to read variables from file " + path );

            //https://decs.deimos.com.pt/pages/viewpage.action?spaceKey=S3ALTB&title=Aliases+Products+IDs

			int ReturnCode = ReadTrack( alias_used, path, record, lonv, lon_dim, latv, lat_dim, timev, time_dim, time ? 3 : 2 );
			if ( ReturnCode == BRATHL_SUCCESS )
			{
				assert__( lon_dim == lat_dim );				
				assert__( !is_netcdf || ( lon_dim == expected_lon_dim && lat_dim == expected_lat_dim ) );

				debug_log( "Normalizing longitudes..." );

				for ( size_t i = 0; i < lon_dim; ++i )
					lonv[ i ] = CTools::NormalizeLongitude( -180.0, lonv[ i ] );

				debug_log( "About to plot..." );

				mMap->PlotTrack( lonv, latv, timev, lon_dim, ref_date, Qt::red );	// <QColor>( mc.NextPrimaryColors() ) );

				debug_log( "Finished plotting..." );
			}
			else
				error_msg += ( "\n\nError reading " + path + ".\nReturn code: " + n2s<std::string>( ReturnCode ) );

			free( lonv );
			free( latv );
		}
		catch ( const CException &e )
		{
			error_msg += ( "\n\n" + path + " caused error: " + e.Message() );
		}
		catch ( ... )
		{
			error_msg += ( "\n\nUnknown error reading file " + path );
		}
	}

	mMap->setRenderFlag( true );
	if ( total_records >= 0 )
		mTotalRecordsSelectedEdit->setText( n2s<std::string>( total_records ).c_str() );

	if ( !error_msg.empty() )
	{
		LOG_WARN( mDataset ? 
			( "Problems reading satellite tracks from " + mDataset->GetName() + ":\n" + error_msg )
			: error_msg );
	}
}





///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_DatasetFilterControls.cpp"
