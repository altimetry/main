/*
* This file is part of BRAT
*
* BRAT is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* BRAT is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include "new-gui/brat/stdafx.h"

#include "libbrathl/ProductNetCdf.h"

#include "DataModels/Model.h"
#include "DataModels/Workspaces/Workspace.h"
#include "DataModels/Filters/BratFilters.h"
#include "DataModels/Workspaces/Dataset.h"
#include "DataModels/PlotData/MapColor.h"

#include "GUI/ActionsTable.h"
#include "GUI/ControlPanels/Dialogs/RegionSettingsDialog.h"
#include "GUI/DisplayWidgets/MapWidget.h"

#include "BratLogger.h"
#include "BratSettings.h"

#include "BratFilterControls.h"



/////////////////////////////////////////////////////////////////////////////////////
//							Dataset Filter Controls
/////////////////////////////////////////////////////////////////////////////////////


void CBratFilterControls::CreateWidgets()
{
    // I. Top buttons row
    //
    mNewFilter    = CreateToolButton( "", ":/images/OSGeo/filter_new.png", "<b>Create filter...</b><br>Create a new filter" );
    mRenameFilter = CreateToolButton( "", ":/images/OSGeo/filter_edit.png", "<b>Rename filter...</b><br>Change the name of selected filter" );
    mDeleteFilter = CreateToolButton( "", ":/images/OSGeo/filter_delete.png", "<b>Delete filter...</b><br>Delete the selected filter" );
    mSaveFilters  = CreateToolButton( "", ":/images/OSGeo/filter_save.png", "<b>Save filters</b><br>Save filter parameters." );

    mFiltersCombo = new QComboBox;
    mFiltersCombo->setToolTip( "Select Filter" );

	auto *filters_label = new QLabel( "Selected Filter");
	filters_label->setStyleSheet( "font-weight: bold; color: black" );
    QWidget *buttons_row = CreateButtonRow( false, Qt::Horizontal, 
	{ 
		mNewFilter, mRenameFilter, mDeleteFilter, mSaveFilters, nullptr, filters_label, mFiltersCombo 
	} );

    AddTopWidget( buttons_row );
    AddTopSpace( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding );


    // II. "Where" Description group
    //
    mClearWhere = CreateToolButton( "", ":/images/OSGeo/filter_clearvalues.png", "Clear values" );
    auto *where_l = new QLabel( "Where" );
    QFont font = where_l->font();
    font.setBold( true );
    where_l->setFont( font );
    AddTopLayout( ELayoutType::Horizontal, { /*WidgetLine( nullptr, Qt::Horizontal ), */nullptr, where_l, /*WidgetLine( nullptr, Qt::Horizontal ),*/ nullptr, mClearWhere }, s, m, m, m, m );


    //    II.1 Buttons for region selection
    mShowAllAreas = new QPushButton( "Show all" );
	mShowAllAreas->setToolTip( "<b>All</b><br>Show all areas." );
    mShowAllAreas->setCheckable( true );

    mRegionsCombo = new QComboBox;
    mRegionsCombo->setToolTip( "List of saved regions" );

    mRegionSettings = CreateToolButton( "", ":/images/OSGeo/region_settings.png", "<b>Regions settings...</b><br>Configure regions properties." );

    QBoxLayout *regions_layout = LayoutWidgets( Qt::Horizontal, { mShowAllAreas, nullptr, new QLabel("Show region"), mRegionsCombo });


    //    II.2 Box of Areas with region buttons
    mAreasListWidget = new QListWidget( this );
    mAreasListWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    mNewArea = CreateToolButton( "", ":/images/OSGeo/area_new.png", "<b>Create area</b><br>Use the map selection mouse tool to define a new area" );
    mAddKML  = CreateToolButton( "", ":/images/OSGeo/area_kml_add.png", "<b>Add from KML...</b><br>Add area from KML file" );
    mAddMask = CreateToolButton( "", ":/images/OSGeo/area_mask_add.png", "<b>Add from mask</b><br>Add area from Mask file" );
    mRenameArea = CreateToolButton( "", ":/images/OSGeo/area_edit.png", "<b>Rename area</b><br>Change the name of selected area" );
    mDeleteArea = CreateToolButton( "", ":/images/OSGeo/area_remove.png", "<b>Delete area</b><br>Delete the selected area" );
    //mSaveArea = CreateToolButton( "", ":/images/OSGeo/area_save.png", "<b>Save area</b><br>Save values in selected area" );
    mAddMask->setVisible( false );		//TODO	MODIFY WHEN IMPLEMENTED

    QWidget *buttons_col = CreateButtonRow( false, Qt::Horizontal, { mNewArea, mAddKML, mAddMask, mRenameArea, mDeleteArea, nullptr, mRegionSettings } );

    QGroupBox *areas_box = CreateGroupBox( ELayoutType::Vertical, { buttons_col, mAreasListWidget, regions_layout }, "Areas && Regions", this, 2, 4, 4, 4, 4 );


    //    II.3 Coordinates (max and min values)
    mMaxLatEdit = new QLineEdit(this);
    mMaxLonEdit = new QLineEdit(this);
    mMinLatEdit = new QLineEdit(this);
    mMinLonEdit = new QLineEdit(this);

	SetReadOnlyEditor( mMaxLatEdit, true );
	SetReadOnlyEditor( mMaxLonEdit, true );
	SetReadOnlyEditor( mMinLatEdit, true );
	SetReadOnlyEditor( mMinLonEdit, true );

    QLabel *icon_north = new QLabel();
    icon_north->setPixmap( QPixmap(":/images/OSGeo/north-arrow.png") );

    QBoxLayout *coord_values = LayoutWidgets( Qt::Vertical, {
                                             //LayoutWidgets( Qt::Horizontal, { nullptr } ),
                                             LayoutWidgets( Qt::Horizontal, { nullptr, new QLabel( "Max Lat (deg)" ), nullptr } ),
                                             LayoutWidgets( Qt::Horizontal, { nullptr, mMaxLatEdit, nullptr } ),
                                             LayoutWidgets( Qt::Horizontal, { new QLabel( "Min Lon (deg)" ), nullptr, new QLabel( "Max Lon (deg)" ) } ),
                                             LayoutWidgets( Qt::Horizontal, { mMinLonEdit, icon_north, mMaxLonEdit } ),
                                             LayoutWidgets( Qt::Horizontal, { nullptr, new QLabel( "Min Lat (deg)" ), nullptr } ),
                                             LayoutWidgets( Qt::Horizontal, { nullptr, mMinLatEdit, nullptr } ),
                                             //LayoutWidgets( Qt::Horizontal, { nullptr } ),
                                               } );

	QToolBar *toolbar = new QToolBar;
	mSelectionButton = CMapWidget::CreateMapSelectionActions( toolbar, mActionSelectFeatures, mActionDeselectAll );
	toolbar->addAction( mActionDeselectAll );
	toolbar->insertWidget( mActionDeselectAll, mSelectionButton );
#if defined(ENABLE_POLYGON_SELECTION)
	CMapWidget::AddMapSelectionPolygon( mSelectionButton, mMainToolsToolBar, mActionSelectPolygon );
	mDesktopManager->Map()->ConnectParentSelectionActions( mSelectionButton, mActionSelectFeatures, mActionSelectPolygon, mActionDeselectAll );
#else
	mDesktopManager->Map()->ConnectParentSelectionActions( mSelectionButton, mActionSelectFeatures, nullptr, mActionDeselectAll );
#endif
	mSelectionButton->setAutoRaise( false );
	((QToolButton*)toolbar->widgetForAction( mActionDeselectAll ))->setAutoRaise( false );
	toolbar->setIconSize( QSize( tool_icon_size, tool_icon_size ) );
	toolbar->layout()->setSpacing( 2 );
	toolbar->layout()->setMargin( 2 );

	auto *coord_box = CreateGroupBox( ELayoutType::Vertical, 
	{ 
		toolbar,
		coord_values 
	}, "Area Selection", this, 2, 4, 4, 4, 4 );

    //    II.4 Adding previous widgets to this...
    QBoxLayout *areas_coord = LayoutWidgets( Qt::Horizontal, { areas_box, coord_box } );

    mWhereBox = AddTopGroupBox(  ELayoutType::Vertical, { areas_coord } );
    AddTopSpace( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding );


    // III. "When" Description group
    //
    auto *when_l = new QLabel( "When" );
    when_l->setFont( font );
    mClearWhen = CreateToolButton( "", ":/images/OSGeo/filter_clearvalues.png", "Clear values" );
    AddTopLayout( ELayoutType::Horizontal, { /*WidgetLine( nullptr, Qt::Horizontal )*/nullptr, when_l, /*WidgetLine( nullptr, Qt::Horizontal ),*/ nullptr, mClearWhen }, s, m, m, m, m );

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


    //AddTopSpace( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding );

    Wire();
}


void CBratFilterControls::Wire()
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
CBratFilterControls::CBratFilterControls( CModel &model, CDesktopManagerBase *manager, QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0
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


bool CBratFilterControls::ReloadFilters()
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


void CBratFilterControls::FillFiltersCombo()
{
    mFiltersCombo->clear();

    auto const &filters = mBratFilters.FiltersMap();
    for ( auto const &filter : filters )
        mFiltersCombo->addItem( filter.first.c_str() );
}

void CBratFilterControls::FillRegionsCombo()
{
    mRegionsCombo->clear();

    auto &rmap = mBratRegions.RegionsMap();
    for ( auto &region_entry : rmap )
    {
        mRegionsCombo->addItem( region_entry.first.c_str() );
    }
}

void CBratFilterControls::FillAreasList()
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


void CBratFilterControls::ShowOnlyAreasInRegion(int region_index)
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


void CBratFilterControls::HandleCurrentLayerSelectionChanged( QRectF box /*= QRectF()*/ )
{
    // Clear Selected area in areas list
    mAreasListWidget->clearSelection();
    mRenameArea->setDisabled( true );
    mDeleteArea->setDisabled( true );

    // Check Lat and Lon values
    double MaxLat = box.bottom();
    double MinLat = box.top();
    double MaxLon = box.right();
    double MinLon = box.left();

    if ( MaxLat >   90 ){ MaxLat =  90; }
    if ( MaxLat <  -90 ){ MaxLat = -90; }

    if ( MinLat >   90 ){ MinLat =  90; }
    if ( MinLat <  -90 ){ MinLat = -90; }

    if ( MaxLon >  180 ){ MaxLon =  180; }
    if ( MaxLon < -180 ){ MaxLon = -180; }

    if ( MinLon >  180 ){ MinLon =  180; }
    if ( MinLon < -180 ){ MinLon = -180; }

    // Check if selection is valid
    bool invalid_selection = ( ( MaxLat == 0 && MinLat == 0 && MaxLon == 0 && MinLon == 0 ) || // No area selected
                               ( MaxLat == MinLat ||  MaxLon == MinLon ) );                    // Empty area

    mMaxLatEdit->setText( invalid_selection ? "" : n2q(MaxLat) );
    mMaxLonEdit->setText( invalid_selection ? "" : n2q(MaxLon) );
    mMinLatEdit->setText( invalid_selection ? "" : n2q(MinLat) );
    mMinLonEdit->setText( invalid_selection ? "" : n2q(MinLon) );

    // Disable button new area if selection is not valid
    mNewArea->setDisabled( invalid_selection );
}


//public slots:
void CBratFilterControls::HandleWorkspaceChanged()
{
	LOG_TRACEstd( "Filters tab started handling signal to change workspace" );

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

	LOG_TRACEstd( "Filters tab finished handling signal to change workspace" );
}


void CBratFilterControls::HandleNewFilter()
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


void CBratFilterControls::HandleRenameFilter()
{
    assert__( mFilter );

	CStringArray operation_names;
	if ( mWOperation->UseFilter( mFilter->Name(), &operation_names ) )
	{
		SimpleErrorBox( "Filter '" + mFilter->Name() + "' cannot be renamed. It is used by the following operations:\n\n" + operation_names.ToString("\n") );
		return;
	}

    auto result = ValidatedInputString( "Filter Name", mFilter->Name(), "Rename Filter..." );
    if ( !result.first )
        return;

    if ( !mBratFilters.RenameFilter( mFilter->Name(), result.second ) )
    {
        SimpleMsgBox( "A filter with same name already exists." );
        return;
    }

    FillFiltersCombo();
    mFiltersCombo->setCurrentIndex( mFiltersCombo->findText( result.second.c_str() ) );

    emit FiltersChanged();
}


void CBratFilterControls::HandleDeleteFilter()
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


void CBratFilterControls::HandleSaveFilters()
{
    assert__( mFilter );

    if ( !mBratFilters.Save() )
        SimpleWarnBox( "There was a problem saving filters to '" + mBratFilters.FilePath() + "'. Some information could be lost or damaged." );
}


void CBratFilterControls::HandleFiltersCurrentIndexChanged( int filter_index )
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


void CBratFilterControls::HandleRegionsCurrentIndexChanged( int region_index )
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


void CBratFilterControls::HandleAreasSelectionChanged()
{
    QListWidgetItem *item = mAreasListWidget->currentItem();

    mNewArea->setEnabled( item != nullptr ); // allowing to duplicate area
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


void CBratFilterControls::HandleClearWhere()
{
    mFilter->DeleteAllAreas();

    // Refresh areas list (checked/unchecked status)
    FillAreasList();

    // Update Max/Min Lat and Lon and refresh "NewArea" button status
    HandleCurrentLayerSelectionChanged();
}


void CBratFilterControls::HandleClearWhen()
{
    mFilter->SetDefaultValues();

    updateDateWidgets();
    updateCyclePassWidgets();
    updateRelativeTimeWidgets();
}


void CBratFilterControls::HandleShowAllAreas()
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

void CBratFilterControls::HandleRegionSettings()
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


void CBratFilterControls::HandleNewArea()
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

void CBratFilterControls::HandleAddKML()
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

void CBratFilterControls::HandleAddMask()
{
    // TODO
    BRAT_NOT_IMPLEMENTED;
    //Save all areas
//    SaveAllAreas();
}

void CBratFilterControls::HandleRenameArea()
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

void CBratFilterControls::HandleDeleteArea()
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


void CBratFilterControls::HandleAreaChecked(QListWidgetItem *area_item)
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


void CBratFilterControls::SaveAllAreas()
{
    if ( !mBratAreas.Save() )
        SimpleWarnBox( "There was a problem saving areas to '" + mBratAreas.FilePath() + "'. Some information could be lost or damaged." );
}



void CBratFilterControls::HandleStartDateTimeChanged(const QDateTime &start_datetime)
{
    mFilter->StartTime() = start_datetime;

    // When user selects a data, the cycle and pass are automatically deleted (Only user dates are used in When criteria)
    mFilter->SetDefaultCyclePassValues();
    updateDateWidgets();
    updateCyclePassWidgets();

    emit FilterCompositionChanged( mFilter->Name() );
}


void CBratFilterControls::HandleStopDateTimeChanged(const QDateTime &stop_datetime)
{
    mFilter->StopTime() = stop_datetime;

    // When user selects a data, the cycle and pass are automatically deleted (Only user dates are used in When criteria)
    mFilter->SetDefaultCyclePassValues();
    updateDateWidgets();
    updateCyclePassWidgets();

    emit FilterCompositionChanged( mFilter->Name() );
}


void CBratFilterControls::HandleStartCycleChanged()
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

    emit FilterCompositionChanged( mFilter->Name() );
}


void CBratFilterControls::HandleStopCycleChanged()
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

    emit FilterCompositionChanged( mFilter->Name() );
}


void CBratFilterControls::HandleStartPassChanged()
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

    emit FilterCompositionChanged( mFilter->Name() );
}


void CBratFilterControls::HandleStopPassChanged()
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

    emit FilterCompositionChanged( mFilter->Name() );
}


void CBratFilterControls::HandleRelativeTimesBoxChecked( bool checked )
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


void CBratFilterControls::HandleRelativeStartTimeChanged()
{
    ValidateAndStoreValue( mRelativeStart,                // Text box                                                   -------------------------------------
                           mFilter->RelativeStartDays(),  // Filter value          Start  Stop    Ref    Start  Stop   | X1 and X2 are the number of days    |
                           mFilter->RelativeStopDays(),   // Default Param         __|_____|_______|_______|_____|__   | later [or earlier if it's negative] |
                           CTools::m_defaultValueINT32,   // Min                     |     |               |     |     | than Ref datetime)                  |
                           mFilter->RelativeStopDays() ); // Max (should be < X2)   -X1   -X2              X1    X2     -------------------------------------

    mFilter->Relative2AbsoluteTimes();

    updateRelativeTimeWidgets();
    updateDateWidgets();

    emit FilterCompositionChanged( mFilter->Name() );
}


void CBratFilterControls::HandleRelativeStopTimeChanged()
{
    ValidateAndStoreValue( mRelativeStop,                  // Text box                                                   -------------------------------------
                           mFilter->RelativeStopDays(),    // Filter value          Start  Stop    Ref    Start  Stop   | X1 and X2 are the number of days    |
                           mFilter->RelativeStartDays(),   // Default Param         __|_____|_______|_______|_____|__   | later [or earlier if it's negative] |
                           mFilter->RelativeStartDays(),   // Min (should be > X1)    |     |               |     |     | than Ref datetime)                  |
                           CTools::m_defaultValueINT32  ); // Max                    -X1   -X2              X1    X2     -------------------------------------

    mFilter->Relative2AbsoluteTimes();

    updateRelativeTimeWidgets();
    updateDateWidgets();

    emit FilterCompositionChanged( mFilter->Name() );
}


void CBratFilterControls::HandleCurrentDateTimeBoxChecked( bool checked )
{
    mRefDateTimeEdit->setEnabled( !checked );

    if ( mFilter )
    {
        mFilter->UseCurrentTime() = checked;

        // Re-calculate start and stop dates using new reference date
        mFilter->Relative2AbsoluteTimes();
        updateDateWidgets();
        updateRelativeTimeWidgets();

	    emit FilterCompositionChanged( mFilter->Name() );
	}
}


void CBratFilterControls::HandleRelativeReferenceTimeChanged(const QDateTime &ref_datetime)
{
    mFilter->RelativeReferenceTime() = ref_datetime;

    // Re-calculate start and stop dates using new reference date
    mFilter->Relative2AbsoluteTimes();
    updateDateWidgets();

    emit FilterCompositionChanged( mFilter->Name() );
}


// ---------------------------------------------------------------------------------------
// This method is used to validate the (start/stop) Cycle, Pass and Relative time values.
// The value in TextBox is validated and set into filter object.
// Case the value is outside the min/max limits, the ParamDef is automatically assigned.
void CBratFilterControls::ValidateAndStoreValue(QLineEdit *TextBox, int &ValueInFilter, int ParamDef, int min, int max)
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


void CBratFilterControls::updateDateWidgets()
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


void CBratFilterControls::updateCyclePassWidgets()
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

	SetReadOnlyEditor( mStartCycleEdit, markAsReadOnly );
	SetReadOnlyEditor( mStopCycleEdit, markAsReadOnly );
	SetReadOnlyEditor( mStartPassEdit, markAsReadOnly );
	SetReadOnlyEditor( mStopPassEdit, markAsReadOnly );
}

void CBratFilterControls::updateRelativeTimeWidgets()
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



enum AliasIndex
{
	eAliasIndexLon,
	eAliasIndexLat,
	eAliasIndexTime,
};


// Here is an expression using the  alias 'swh': %{sig_wave_height} -->  'swh' field in tha aliases dictionary (Jason-1)
//Expressions[1]	= "%{sig_wave_height}";
//Units[1]		= "count";
//
// Here is an expression calling the 'BratAlgoFilterMedianAtp' algorithm
//Expressions[1] = "exec(\"BratAlgoFilterMedianAtp\", %{sig_wave_height}, 7, 1, 0)";
//Units[1]		= "count";
//
int ReadTrack( const std::vector< unsigned char > &can_use_alias, 
	const std::string &path, const std::string &record, double *&x, size_t &sizex, double *&y, size_t &sizey, double *&z, size_t &sizez, int nfields = -1 )
{
	static const int default_nfields = 3;
	static const char *Units[ default_nfields ] = { "count", "count", "second" };

    const char *Expressions[] =
    {
        can_use_alias[eAliasIndexLon]	? lon_alias().c_str()	: lon_name().c_str(),
        can_use_alias[eAliasIndexLat]	? lat_alias().c_str()	: lat_name().c_str(),
        can_use_alias[eAliasIndexTime]	? time_alias().c_str()	: time_name().c_str()
    };
    char *Names[] = { const_cast<char*>( path.c_str() ) };

    double	*Data[ default_nfields ]	= { nullptr, nullptr, nullptr };
    int32_t	Sizes[ default_nfields ]	= { -1, -1, -1 };

    size_t	ActualSize;
    if ( nfields < 0 )
		nfields = default_nfields;


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



void CBratFilterControls::HandleDatasetChanged( CDataset *dataset )
{
    static CMapColor &mc = CMapColor::GetInstance();        Q_UNUSED(mc);
    static const std::string unknown( "???" );

    //lambdas

    auto debug_log = []( const std::string &msg )
    {
        LOG_TRACEstd( msg  );
        qApp->processEvents();
    };


    //function body

    mMap->RemoveTracksLayerFeatures( true );
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
    std::vector< std::string > paths = mDataset->GetFiles();
    for ( auto &path : paths )
    {
        WaitCursor wait;

		bool is_netcdf = false, skip_iteration = false;
        std::vector< unsigned char > alias_used = { false, false, false };

		std::string record, info;

		auto ref_date = REF19500101;	//Use ReadData ref date instead of product->GetRefDate();

		long expected_lon_dim = 0, expected_lat_dim = 0; auto expected_time_dim = 0;                Q_UNUSED( expected_time_dim );

		{
			CProductInfo product_info( mDataset, path );
			if ( !product_info.IsValid() )
			{
				error_msg += ( "\n\n" + path + " caused error: " + product_info.ErrorMessages() );
				continue;
			}

			//total_records += product_info->GetNumberOfRecords();
			skip_iteration = !product_info.HasAliases();
			if ( skip_iteration )
			{
				LOG_WARN( "Aliases not supported: reading file " + path );
			}
			is_netcdf = product_info.IsNetCdfOrNetCdfCF();

			record = product_info.Record();

			std::string lonlat_error_msg, time_error_msg;
			std::pair<CField*, CField*> fields = product_info.FindLonLatFields( mModel.Settings().mUseUnsupportedFields, 
				(bool&)alias_used[ eAliasIndexLon ], (bool&)alias_used[ eAliasIndexLat ], lonlat_error_msg );
			CField *lon = fields.first;
			CField *lat = fields.second;
			CField *time = product_info.FindTimeField( mModel.Settings().mUseUnsupportedFields, (bool&)alias_used[ eAliasIndexTime ], time_error_msg );
			if ( !lon || !lat || !time )
			{
				skip_iteration = true;				
				LOG_WARN( "File - " + path + "\n" + replace( lonlat_error_msg, "\n", " - " ) + " - " + time_error_msg );
			}

			expected_lon_dim = lon ? lon->GetDim()[ 0 ] : 0;
			expected_lat_dim = lat ? lat->GetDim()[ 0 ] : 0;
			expected_time_dim = time ? time->GetDim()[ 0 ] : 0;
			if ( expected_lon_dim != expected_lat_dim )
			{
				skip_iteration = true;
				LOG_WARN( "Different latitude/longitude dimensions in file " + path );
			}

			info =
				( alias_used[ eAliasIndexLon ] ?	lon_alias() : lon_name() ) +	"==" + ( ( lon ? lon->GetName() : unknown ) + " " ) +
				( alias_used[ eAliasIndexLat ] ?	lat_alias() : lat_name() ) +	"==" + ( ( lat ? lat->GetName() : unknown ) + " " ) +
				( alias_used[ eAliasIndexTime ] ?	time_alias() : time_name() ) +	"==" + ( ( time ? time->GetName() : unknown ) );

		}	//delete product;


        if ( skip_iteration )
        {
			LOG_WARN( info );
			continue;
        }

		LOG_INFO( info );

        size_t lon_dim = 0;
        size_t lat_dim = 0;
        size_t time_dim = 0;
        double *lonv = nullptr;
        double *latv = nullptr;
        double *timev = nullptr;

        debug_log( "About to read variables from file " + path );

        //https://decs.deimos.com.pt/pages/viewpage.action?spaceKey=S3ALTB&title=Aliases+Products+IDs

        int ReturnCode = ReadTrack( alias_used, path, record, lonv, lon_dim, latv, lat_dim, timev, time_dim );
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

#include "moc_BratFilterControls.cpp"
