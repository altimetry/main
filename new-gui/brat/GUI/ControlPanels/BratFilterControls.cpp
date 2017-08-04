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

#include "GUI/ActionsTable.h"
#include "GUI/ControlPanels/Dialogs/RegionSettingsDialog.h"
#include "GUI/DisplayWidgets/MapWidget.h"

#include "BratLogger.h"
#include "BratApplication.h"

#include "BratFilterControls.h"



/////////////////////////////////////////////////////////////////////////////////////
//							Dataset Filter Controls
/////////////////////////////////////////////////////////////////////////////////////

static const QString cycle_pas_radio_text = "Use Cycle/Pass";


template< typename VALIDATOR >
struct ValidatorTraits;

template<>
struct ValidatorTraits< double >
{
	using type = QDoubleValidator;
};

//int decimals = 10:	digits after decimal point
// 
template< typename T >
inline typename ValidatorTraits<T>::type* CreateValidator( QObject *parent, T bottom, T top, int decimals = 10 );

template<>
inline typename ValidatorTraits<double>::type* CreateValidator( QObject *parent, double bottom, double top, int decimals )
{
	typename ValidatorTraits<double>::type *v = new ValidatorTraits<double>::type( bottom, top, decimals, parent );
	v->setNotation(ValidatorTraits<double>::type::StandardNotation);
	QLocale l( "C" );
	l.setNumberOptions(QLocale::RejectGroupSeparator);
	v->setLocale( l );

	return v;
}



void CBratFilterControls::CreateWidgets()
{
    // I. Top buttons row
    //
    mNewFilter    = CreateToolButton( "", ":/images/OSGeo/filter_new.png", CActionInfo::FormatTip( "Create filter...\nCreate a new filter" ) );
    mRenameFilter = CreateToolButton( "", ":/images/OSGeo/filter_edit.png", CActionInfo::FormatTip( "Rename filter...\nChange the name of selected filter" ) );
    mDeleteFilter = CreateToolButton( "", ":/images/OSGeo/filter_delete.png", CActionInfo::FormatTip( "Delete filter...\nDelete the selected filter" ) );
    mSaveFilters  = CreateToolButton( "", ":/images/OSGeo/filter_save.png", CActionInfo::FormatTip( "Save filters\nSave filter parameters." ) );

    mFiltersCombo = new QComboBox;
    mFiltersCombo->setToolTip( "Select Filter" );

	auto *filters_label = new QLabel( "Selected Filter");
	filters_label->setStyleSheet( "font-weight: bold; color: black" );
    QWidget *buttons_row = CreateButtonRow( false, Qt::Horizontal, 
	{ 
		mNewFilter, mRenameFilter, mDeleteFilter, mSaveFilters, nullptr, filters_label, mFiltersCombo 
	} );

    AddTopWidget( buttons_row );
    //AddTopSpace( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding );


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

    mRegionSettings = CreateToolButton( "", ":/images/OSGeo/region_settings.png", CActionInfo::FormatTip( "Regions settings...\nConfigure regions properties." ) );

    QBoxLayout *regions_layout = LayoutWidgets( Qt::Vertical, 
    { 
        LayoutWidgets( { new QLabel("Show region only"), mRegionsCombo } ),
        mShowAllAreas
    });


    //    II.2 Box of Areas with region buttons
    mAreasListWidget = new QListWidget( this );
    mAreasListWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
	mAreasListWidget->setToolTip("Checking an area adds it to the currently selected filter");

    mNewArea = CreateToolButton( "", ":/images/OSGeo/area_new.png", CActionInfo::FormatTip( "Create area\nUse the area selection tool to enable creating a new area" ) );
    mAddKML  = CreateToolButton( "", ":/images/OSGeo/area_kml_add.png", CActionInfo::FormatTip( "Add from KML...\nAdd area from KML file" ) );
    mAddMask = CreateToolButton( "", ":/images/OSGeo/area_mask_add.png", CActionInfo::FormatTip( "Add from mask\nAdd area from Mask file" ) );
    mRenameArea = CreateToolButton( "", ":/images/OSGeo/area_edit.png", CActionInfo::FormatTip( "Rename area\nChange the name of selected area" ) );
    mDeleteArea = CreateToolButton( "", ":/images/OSGeo/area_remove.png", CActionInfo::FormatTip( "Delete area\nDelete the selected area" ) );
    //mSaveArea = CreateToolButton( "", ":/images/OSGeo/area_save.png", "<b>Save area</b><br>Save values in selected area" );
    mAddMask->setVisible( false );		//TODO	MODIFY WHEN IMPLEMENTED

	mNewArea->setEnabled( false );

    QWidget *buttons_col = CreateButtonRow( false, Qt::Horizontal, { mNewArea, mAddKML, mAddMask, mRenameArea, mDeleteArea, nullptr, mRegionSettings } );

    QGroupBox *areas_box = CreateGroupBox( ELayoutType::Vertical, { buttons_col, mAreasListWidget, regions_layout }, "Areas && Regions", this, 2, 4, 4, 4, 4 );


    //    II.3 Coordinates (max and min values)
    //    Re-assign mFirstCoordEdit to the 1st of the following 4 coordinate editors
    //    The order of creation is the Z-order for manual editing, so do NOT change it 
    //    unless the Z-order needs to be changed too.
    //    
	mMaxLatEdit = new QLineEdit( this );				mFirstCoordEdit = mMaxLatEdit;
    mMaxLonEdit = new QLineEdit(this);
    mMinLatEdit = new QLineEdit(this);
    mMinLonEdit = new QLineEdit(this);

	static QDoubleValidator *lon_validator = CreateValidator< double >( this, -180., 180. );
	static QDoubleValidator *lat_validator = CreateValidator< double >( this, -90., 90. );

	mMaxLatEdit->setValidator( lat_validator );  mMinLatEdit->setValidator( lat_validator );
	mMaxLonEdit->setValidator( lon_validator );  mMinLonEdit->setValidator( lon_validator );

	SetReadOnlyEditor( mMaxLatEdit, true );
	SetReadOnlyEditor( mMaxLonEdit, true );
	SetReadOnlyEditor( mMinLatEdit, true );
	SetReadOnlyEditor( mMinLonEdit, true );

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

	QToolBar *toolbar = new QToolBar;
	mMapSelectionButton = CMapWidget::CreateMapSelectionActions( toolbar, mActionSelectFeatures, mActionDeselectAll );
	toolbar->addAction( mActionDeselectAll );
	toolbar->insertWidget( mActionDeselectAll, mMapSelectionButton );
#if defined(ENABLE_POLYGON_SELECTION)
	CMapWidget::AddMapSelectionPolygon( mMapSelectionButton, mMainToolsToolBar, mActionSelectPolygon );
	mDesktopManager->Map()->ConnectParentSelectionActions( mMapSelectionButton, mActionSelectFeatures, mActionSelectPolygon, mActionDeselectAll );
#else
	mDesktopManager->Map()->ConnectParentSelectionActions( mMapSelectionButton, mActionSelectFeatures, nullptr, mActionDeselectAll );
#endif
	mMapSelectionButton->setAutoRaise( false );
	((QToolButton*)toolbar->widgetForAction( mActionDeselectAll ))->setAutoRaise( false );
	toolbar->setIconSize( QSize( tool_icon_size, tool_icon_size ) );
	toolbar->layout()->setSpacing( 2 );
	toolbar->layout()->setMargin( 2 );

	auto *coord_box = CreateGroupBox( ELayoutType::Vertical, 
	{ 
		toolbar,
		coord_values 
	}, "New Area Selection", this, 2, 4, 4, 4, 4 );

    //    II.4 Adding previous widgets to this...
    QBoxLayout *areas_coord = LayoutWidgets( Qt::Horizontal, { coord_box, areas_box } );

    mWhereBox = AddTopGroupBox(  ELayoutType::Vertical, { areas_coord } );
    //AddTopSpace( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding );


    // III. "When" Description group
    //
    auto *when_l = new QLabel( "When" );
    when_l->setFont( font );
    mClearWhen = CreateToolButton( "", ":/images/OSGeo/filter_clearvalues.png", "Clear values" );
    AddTopLayout( ELayoutType::Horizontal, { /*WidgetLine( nullptr, Qt::Horizontal )*/nullptr, when_l, /*WidgetLine( nullptr, Qt::Horizontal ),*/ nullptr, mClearWhen }, s, m, m, m, m );

    //    III.1 Dates, Cycles and Pass (start and stop values)
    QDateTime minDateTime( QDate(1950, 1, 1), QTime(0, 0, 0));

	mUseTimeRadio = new QRadioButton( "Use Dates");
	mUseCyclePassRadio = new QRadioButton( cycle_pas_radio_text );

	mStartTimeEdit = new QDateTimeEdit();								mStopTimeEdit = new QDateTimeEdit();
    mStartTimeEdit->setCalendarPopup(true);								mStopTimeEdit->setCalendarPopup(true);
    mStartTimeEdit->setDisplayFormat("yyyy.MM.dd hh:mm:ss");			mStopTimeEdit->setDisplayFormat("yyyy.MM.dd hh:mm:ss");
    mStartTimeEdit->setMinimumDateTime( minDateTime );					mStopTimeEdit->setMinimumDateTime( minDateTime );

	// Any date-time edit is good; we found no other way to do this
	// 
	CBratFilter::SetRelativeTimesValidationRange( mStartTimeEdit->minimumDateTime(), mStartTimeEdit->maximumDateTime() );


    static QRegExpValidator *textValidator = new QRegExpValidator( QRegExp("[0-9]+"), this ); // only numeric letters

    mStartCycleEdit = new QLineEdit(this);           mStopCycleEdit  = new QLineEdit(this);
    mStartCycleEdit->setValidator( textValidator );  mStopCycleEdit->setValidator( textValidator );

    mStartPassEdit = new QLineEdit(this);            mStopPassEdit = new QLineEdit(this);
    mStartPassEdit->setValidator( textValidator );   mStopPassEdit->setValidator( textValidator );


    QBoxLayout *dates_box = LayoutWidgets( Qt::Vertical, 
	{
		LayoutWidgets( Qt::Horizontal, { new QLabel( "Start" ), mStartTimeEdit } ),
		LayoutWidgets( Qt::Horizontal, { new QLabel( "Stop" ),  mStopTimeEdit } )
	} );

    QBoxLayout *cycles_box = LayoutWidgets( Qt::Vertical, 
	{
        LayoutWidgets( Qt::Horizontal, { new QLabel( "Start Cycle" ), mStartCycleEdit } ),
        LayoutWidgets( Qt::Horizontal, { new QLabel( "Stop Cycle" ),  mStopCycleEdit  } )
    } );

    QBoxLayout *pass_box = LayoutWidgets( Qt::Vertical, 
	{
        LayoutWidgets( Qt::Horizontal, { new QLabel( "Start Pass" ), mStartPassEdit } ),
        LayoutWidgets( Qt::Horizontal, { new QLabel( "Stop Pass" ),  mStopPassEdit  } )
    } );


    mAbsoluteTimesBox = CreateGroupBox(  ELayoutType::Horizontal, 
	{ 
		LayoutWidgets( Qt::Vertical, 
		{
			mUseTimeRadio,						
			dates_box
		} )
		,
		nullptr, 
		LayoutWidgets( Qt::Vertical, 
		{
			mUseCyclePassRadio,
			LayoutWidgets( Qt::Horizontal, 
			{
				cycles_box, nullptr, pass_box
			} )
		} )
	} 
	);


	mRelativeStart = new QLineEdit(this);
    mRelativeStop = new QLineEdit(this);
    mRelativeStart->setToolTip( "Insert the number of days later than reference date (or earlier if number of days is negative)" );
    mRelativeStop->setToolTip( "Insert the number of days later than reference date (or earlier if number of days is negative)" );

	mRelativeStart->setValidator( new QIntValidator );
	mRelativeStop->setValidator( new QIntValidator );

    mRefDateTimeEdit = new QDateTimeEdit();
    mRefDateTimeEdit->setCalendarPopup(true);
    mRefDateTimeEdit->setDisplayFormat("yyyy.MM.dd hh:mm:ss");
    mRefDateTimeEdit->setMinimumDateTime( minDateTime );
    mRefDateTimeEdit->setToolTip( "Edit reference date" );

    mUseCurrentDateTime = new QCheckBox( "Current DateTime" );
    //mUseCurrentDateTime->setLayoutDirection( Qt::RightToLeft );
    mUseCurrentDateTime->setToolTip( "Use current datetime as reference date" );
    mUseCurrentDateTime->setStyleSheet( "QCheckBox:checked{color: black;} QCheckBox:unchecked {color: grey;} QCheckBox:disabled {color: grey;}");

	mStartTimeEdit->setCalendarPopup(false);							mStopTimeEdit->setCalendarPopup(false);

	QBoxLayout *days_l = LayoutWidgets( Qt::Vertical, {
		LayoutWidgets( Qt::Horizontal, { new QLabel( "Start" ), mRelativeStart } ),
		LayoutWidgets( Qt::Horizontal, { new QLabel( "Stop" ),  mRelativeStop } )
	} );

	//auto *RefDate_box = CreateGroupBox( ELayoutType::Vertical, {
	auto *RefDate_box = LayoutWidgets( Qt::Vertical, { mUseCurrentDateTime, mRefDateTimeEdit }, nullptr, 2,2,2,2,2 );
	RefDate_box->setAlignment( Qt::AlignCenter );

	mRelativeTimesBox = CreateGroupBox( ELayoutType::Horizontal, { days_l, nullptr, RefDate_box }, "Use Relative Time (days)" );
    mRelativeTimesBox->setCheckable( true );
	mRelativeTimesBox->setAlignment( Qt::AlignCenter );

    mWhenBox = AddTopGroupBox( ELayoutType::Vertical,
    {
        mAbsoluteTimesBox,
        mRelativeTimesBox
    }
    );



    AddTopSpace( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding );

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

	connect( mActionSelectFeatures, &QAction::toggled, this, &CBratFilterControls::EnableRectangularSelection );

	connect( mMap, SIGNAL( NewRubberBandSelection( QRectF ) ), this, SLOT( HandleCurrentLayerSelectionChanged( QRectF ) ) );

	// When widgets
	connect( mClearWhen, SIGNAL( clicked() ), this, SLOT( HandleClearWhen() ) );

	connect( mUseTimeRadio, SIGNAL( toggled( bool ) ), this, SLOT( HandleUseTimeToggled( bool ) ) );
    connect( mStartTimeEdit, SIGNAL( dateTimeChanged(const QDateTime&) ), this, SLOT( HandleStartDateTimeChanged(const QDateTime&) ) );
    connect( mStopTimeEdit, SIGNAL( dateTimeChanged(const QDateTime&) ), this, SLOT( HandleStopDateTimeChanged(const QDateTime&) ) );

	connect( mUseCyclePassRadio, SIGNAL( toggled( bool ) ), this, SLOT( HandleUseCyclePass( bool ) ) );
	connect( mStartCycleEdit, SIGNAL( textEdited(const QString &) ), this, SLOT( HandleStartCycleChanged() ) );
    connect( mStopCycleEdit,  SIGNAL( textEdited(const QString &) ), this, SLOT( HandleStopCycleChanged() ) );
    connect( mStartPassEdit,  SIGNAL( textEdited(const QString &) ), this, SLOT( HandleStartPassChanged() ) );
    connect( mStopPassEdit,   SIGNAL( textEdited(const QString &) ), this, SLOT( HandleStopPassChanged() ) );
	mUseCyclePassRadio->setChecked( true );	//only to force state change to false (and calling handler) by next line
	mUseTimeRadio->setChecked( true );

    connect( mRelativeTimesBox, SIGNAL( toggled( bool ) ), this, SLOT( HandleRelativeTimesBoxChecked( bool ) ) );
    mRelativeTimesBox->setChecked( false );

    //connect( mRelativeStart,   SIGNAL( textEdited(const QString &) ), this, SLOT( HandleRelativeStartTimeChanged() ) );
    //connect( mRelativeStop,    SIGNAL( textEdited(const QString &) ), this, SLOT( HandleRelativeStopTimeChanged() ) );
	connect( mRelativeStart,   SIGNAL( editingFinished() ), this, SLOT( HandleRelativeStartTimeChanged() ) );
	connect( mRelativeStop,    SIGNAL( editingFinished() ), this, SLOT( HandleRelativeStopTimeChanged() ) );

    connect( mRefDateTimeEdit, SIGNAL( dateTimeChanged(const QDateTime&) ), this, SLOT( HandleRelativeReferenceTimeChanged(const QDateTime&) ) );

    connect( mUseCurrentDateTime, SIGNAL( toggled( bool ) ), this, SLOT( HandleCurrentDateTimeBoxChecked( bool ) ) );
}


//explicit
CBratFilterControls::CBratFilterControls( CBratApplication &app, CDesktopManagerBase *manager, QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0
    : base_t( app, manager, parent, f )
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


//virtual 
void CBratFilterControls::UpdatePanelSelectionChange()
{
	if ( !isEnabled() )
		return;

	if ( SelectedPanel() )
		HandleAreasSelectionChanged();
	else
	{
		EndAreaSelection();

		RemoveAreaSelectionFromMap();
	}
}


//virtual 
void CBratFilterControls::WorkspaceChanged() //override
{
	base_t::WorkspaceChanged();

	auto *root = mModel.RootWorkspace();
	if ( root )
	{
		mWOperation = mModel.Workspace< CWorkspaceOperation >();
	}
	else
	{
		mWOperation = nullptr;
	}
	mFilter = nullptr;
	mFiltersCombo->clear();
	if ( root )
		ReloadFilters();
}


//public slots:
void CBratFilterControls::HandleWorkspaceChanged()
{
	LOG_TRACEstd( "Filters tab started handling signal to change workspace" );

	WorkspaceChanged();

	LOG_TRACEstd( "Filters tab finished handling signal to change workspace" );
}


void CBratFilterControls::HandleNewFilter()
{
    auto result = SimpleInputStringValidated( "Filter Name", mBratFilters.MakeNewName(), "New Filter..." );
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

    auto result = SimpleInputStringValidated( "Filter Name", mFilter->Name(), "Rename Filter..." );
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




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///											Space Related Functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void CBratFilterControls::HandleClearWhere()
{
	mFilter->DeleteAllAreas();

	// Refresh areas list (checked/unchecked status)
	FillAreasList();

	// Update Max/Min Lat and Lon and refresh "NewArea" button status
	HandleCurrentLayerSelectionChanged();
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

	mFilter = filter;										assert__( mFilter );

	mUseTimeRadio->setChecked( !mFilter->UsingCyclePass() );
	mUseCyclePassRadio->setChecked( mFilter->UsingCyclePass() );

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
	//if ( item && !item->isSelected() )
	//	item = nullptr;	

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

	SelectAreaInMap( area->GetLonMin(), area->GetLonMax(), area->GetLatMin(), area->GetLatMax() );
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


bool CBratFilterControls::ValidateArea( double &min_lon, double &max_lon, double &min_lat, double &max_lat, QRectF *pbox )	//pbox = nullptr  
{
	// nested functions

	auto to_double = []( const QString &s )
	{
		bool ok;
		double n = s.toDouble( &ok );
		return ok ? n : 0.;
	};


	// function body

	// Normalize values (swaps if min > max)
	// 
	QgsRectangle rect( 
		to_double( mMinLonEdit->text() ), 
		to_double( mMinLatEdit->text() ), 
		to_double( mMaxLonEdit->text() ), 
		to_double( mMaxLatEdit->text() )
	);


	QRectF box = rect.toRectF();

	max_lat = box.bottom();
	min_lat = box.top();
	max_lon = box.right();
	min_lon = box.left();

	if ( pbox )
		*pbox = box;

	return CArea::Validate( min_lon, max_lon, min_lat, max_lat );
}


void CBratFilterControls::HandleNewArea()
{
	EndAreaSelection();

	double lat_max, lat_min, lon_max, lon_min;
	if ( !ValidateArea( lon_min, lon_max, lat_min, lat_max ) )
	{
		SimpleErrorBox( "The specified area is not valid.\nPlease make sure that it is not empty and that all values are within the allowed ranges." );
	}
	else
	{
		auto result = SimpleInputStringValidated( "Area Name", mBratAreas.MakeNewName(), "New Area..." );
		if ( !result.first )
			return;

		if ( !mBratAreas.AddArea( result.second ) )
		{
			SimpleErrorBox( "A area with same name already exists." );
		}
		else
		{
			// Add all vertex of selection to new area
			CArea *area = mBratAreas.Find( result.second );

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
}


void CBratFilterControls::EndAreaSelection()
{
	mMapSelectionButton->setChecked( false );
	mActionSelectFeatures->setChecked( false );
}


// slot
// answers to map tool selection, namely mActionSelectFeatures
// 
void CBratFilterControls::EnableRectangularSelection( bool enable )
{
	SetReadOnlyEditor( mMaxLatEdit, !enable );
	SetReadOnlyEditor( mMinLatEdit, !enable );
	SetReadOnlyEditor( mMaxLonEdit, !enable );
	SetReadOnlyEditor( mMinLonEdit, !enable );

	if ( enable )
	{
		mFirstCoordEdit->setFocus();
	}
	else
	{
		double max_lat, min_lat, max_lon, min_lon;
		QRectF box;
		if ( ValidateArea( min_lon, max_lon, min_lat, max_lat, &box ) )
			SelectAreaInMap( min_lon, max_lon, min_lat, max_lat );

		HandleCurrentLayerSelectionChanged( box );
	}

	mNewArea->setEnabled( enable );
}


void CBratFilterControls::HandleCurrentLayerSelectionChanged( QRectF box )	// = QRectF()
{
	// Clear Selected area in areas list
	mAreasListWidget->clearSelection();		//triggers HandleAreasSelectionChanged (if not already cleared)
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
	// 
	// - do not clear invalid values for the user to have the chance to manually correct 
	// the wrong ones without having to write again the right ones
	// 
	if ( CArea::Validate( MinLon, MaxLon, MinLat, MaxLat ) )
	{
		mMaxLatEdit->setText( n2q( MaxLat ) );
		mMaxLonEdit->setText( n2q( MaxLon ) );
		mMinLatEdit->setText( n2q( MinLat ) );
		mMinLonEdit->setText( n2q( MinLon ) );
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
        auto result = SimpleInputStringValidated( "Area Name", mBratAreas.MakeNewName(), "New Area from KML..." );
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
    auto result = SimpleInputStringValidated( "Area Name", area_name, "Rename Area..." );
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
        mFilter->AddArea( q2a( area_item->text() ) );
    }
    else
    {
        mFilter->RemoveArea( q2a( area_item->text() ) );
    }

    emit FilterCompositionChanged( mFilter->Name() );
}


void CBratFilterControls::SaveAllAreas()
{
    if ( !mBratAreas.Save() )
        SimpleWarnBox( "There was a problem saving areas to '" + mBratAreas.FilePath() + "'. Some information could be lost or damaged." );
}






///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///											Time Related Functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void CBratFilterControls::HandleClearWhen()
{
	mFilter->SetDefaultValues();

	mAbsoluteTimesBox->setChecked( !mFilter->UsingRelativeTimes() );
	mRelativeTimesBox->setChecked( mFilter->UsingRelativeTimes() );
	mUseTimeRadio->setChecked( !mFilter->UsingCyclePass() );
	mUseCyclePassRadio->setChecked( mFilter->UsingCyclePass() );

	// Events triggered by the previous assignments, or if Clear is 
	// clicked directly from a (relative) time widget, the finished 
	// signal being also triggered, the filter is reassigned and 
	// the default values are lost
	// 
	mFilter->SetDefaultValues();

	updateDateWidgets();
	updateCyclePassWidgets();
	updateRelativeTimeWidgets();
}




////////////////////
///	Absolute Times	
////////////////////

void CBratFilterControls::HandleUseTimeToggled( bool toggled )
{
	mStartTimeEdit->setEnabled( toggled );
	mStopTimeEdit->setEnabled( toggled );

	if ( mFilter )
	{
		mFilter->EnableCyclePass( false );		//updateCyclePassWidgets();
	}
}


void CBratFilterControls::HandleUseCyclePass( bool toggled )
{
	mStartCycleEdit->setEnabled( toggled );
	mStopCycleEdit->setEnabled( toggled );
	mStartPassEdit->setEnabled( toggled );
	mStopPassEdit->setEnabled( toggled );

	if ( mFilter )
	{
		mFilter->EnableCyclePass( true ); 		//updateCyclePassWidgets();
	}
}


void CBratFilterControls::HandleStartDateTimeChanged(const QDateTime &start_datetime)
{
	assert__( mFilter && !mFilter->UsingCyclePass() );

	if ( !mFilter->UsingRelativeTimes() && ( start_datetime.isNull() || !start_datetime.isValid() ) )
	{
		SimpleErrorBox( "Invalid date-time start." );
		mStartTimeEdit->setDateTime( mFilter->StartTime() );
		return;
	}

	mFilter->StartTime() = start_datetime;

    updateDateWidgets();
    updateCyclePassWidgets();

    emit FilterCompositionChanged( mFilter->Name() );
}


void CBratFilterControls::HandleStopDateTimeChanged(const QDateTime &stop_datetime)
{
	assert__( mFilter && !mFilter->UsingCyclePass() );

	if ( !mFilter->UsingRelativeTimes() && ( stop_datetime.isNull() || !stop_datetime.isValid() ) )
	{
		SimpleErrorBox( "Invalid date-time stop." );
		mStopTimeEdit->setDateTime( mFilter->StopTime() );
		return;
	}

	mFilter->StopTime() = stop_datetime;

    updateDateWidgets();
    updateCyclePassWidgets();

    emit FilterCompositionChanged( mFilter->Name() );
}


void CBratFilterControls::HandleStartCycleChanged()
{
	assert__( mFilter && mFilter->UsingCyclePass() );

    ValidateAndStoreValue( mStartCycleEdit,              // Text box
                           mFilter->StartCycle(),        // Filter value
                           mFilter->StopCycle(),         // Default Param
                           CTools::m_defaultValueINT32,  // Min
                           mFilter->StopCycle()  );      // Max (should be < StopCycle)

    updateDateWidgets();
    updateCyclePassWidgets();

    emit FilterCompositionChanged( mFilter->Name() );
}


void CBratFilterControls::HandleStopCycleChanged()
{
	assert__( mFilter && mFilter->UsingCyclePass() );

	ValidateAndStoreValue( mStopCycleEdit,                // Text box
                           mFilter->StopCycle(),          // Filter value
                           mFilter->StartCycle(),         // Default Param
                           mFilter->StartCycle(),         // Min (should be > StartCycle)
                           CTools::m_defaultValueINT32 ); // Max

    updateDateWidgets();
    updateCyclePassWidgets();

    emit FilterCompositionChanged( mFilter->Name() );
}


void CBratFilterControls::HandleStartPassChanged()
{
	assert__( mFilter && mFilter->UsingCyclePass() );

	ValidateAndStoreValue( mStartPassEdit,               // Text box
                           mFilter->StartPass(),         // Filter value
                           mFilter->StopPass(),          // Default Param
                           CTools::m_defaultValueINT32,  // Min
                           mFilter->StopPass()  );       // Max (should be < StopPass)

    updateDateWidgets();
    updateCyclePassWidgets();

    emit FilterCompositionChanged( mFilter->Name() );
}


void CBratFilterControls::HandleStopPassChanged()
{
	assert__( mFilter && mFilter->UsingCyclePass() );

	ValidateAndStoreValue( mStopPassEdit,                 // Text box
                           mFilter->StopPass(),           // Filter value
                           mFilter->StartPass(),          // Default Param
                           mFilter->StartPass(),          // Min (should be > StartPass)
                           CTools::m_defaultValueINT32 ); // Max

    updateDateWidgets();
    updateCyclePassWidgets();

    emit FilterCompositionChanged( mFilter->Name() );
}



////////////////////
///	Relative Times	
////////////////////


void CBratFilterControls::HandleRelativeTimesBoxChecked( bool checked )
{
	mAbsoluteTimesBox->setEnabled( !checked );
	if (checked)
		mRelativeStart->setFocus();

	if ( mFilter )
	{
		// Updates current DateTime if necessary
		// 			
		mFilter->EnableRelativeTimes( checked );
		if ( checked )
		{
			mRefDateTimeEdit->setDateTime( mFilter->RelativeReferenceTime() );
		}
		updateDateWidgets();
		updateCyclePassWidgets();
		updateRelativeTimeWidgets();
	}
}


void CBratFilterControls::HandleRelativeStartTimeChanged()
{
	QString value_str = mRelativeStart->text();
	int new_start_value = value_str.isEmpty() ? mFilter->RelativeStartDays() : value_str.toInt(),
		new_stop_value = mFilter->RelativeStopDays();

	new_start_value += CBratFilter::DaysOverflow( mFilter->RelativeReferenceTime().addDays( new_start_value) );
	new_stop_value = std::max( new_start_value, new_stop_value  );

    //                                      -------------------------------------
    // Start  Stop    Ref    Start  Stop   | X1 and X2 are the number of days    |
    // __|_____|_______|_______|_____|__   | later [or earlier if it's negative] |
    //   |     |               |     |     | than Ref date-time)                 |
    //  -X1   -X2              X1    X2     -------------------------------------

	bool result = mFilter->UpdateRelative2AbsoluteTimes( new_start_value, new_stop_value, mFilter->UseCurrentTime(), mFilter->RelativeReferenceTime() );	assert__( result );

    updateRelativeTimeWidgets();
    updateDateWidgets();

    emit FilterCompositionChanged( mFilter->Name() );
}


void CBratFilterControls::HandleRelativeStopTimeChanged()
{
	QString value_str = mRelativeStop->text();
	int new_start_value = mFilter->RelativeStartDays(), 
		new_stop_value = value_str.isEmpty() ? mFilter->RelativeStopDays() : value_str.toInt();

	new_stop_value += CBratFilter::DaysOverflow( mFilter->RelativeReferenceTime().addDays( new_stop_value) );
	new_start_value = std::min( new_start_value, new_stop_value  );

	//                                       -------------------------------------
    //  Start  Stop    Ref    Start  Stop   | X1 and X2 are the number of days    |
    //  __|_____|_______|_______|_____|__   | later [or earlier if it's negative] |
    //    |     |               |     |     | than Ref date-time)                  |
    //   -X1   -X2              X1    X2     -------------------------------------

	bool result = mFilter->UpdateRelative2AbsoluteTimes( new_start_value, new_stop_value, mFilter->UseCurrentTime(), mFilter->RelativeReferenceTime() );	assert__( result );

    updateRelativeTimeWidgets();
    updateDateWidgets();

    emit FilterCompositionChanged( mFilter->Name() );
}


void CBratFilterControls::HandleCurrentDateTimeBoxChecked( bool checked )
{
    mRefDateTimeEdit->setEnabled( !checked );

    if ( mFilter )
    {
        // Re-calculate start and stop dates using new reference date
        //
		bool result = mFilter->UpdateRelative2AbsoluteTimes( mFilter->RelativeStartDays(), mFilter->RelativeStopDays(), checked, mFilter->RelativeReferenceTime() );	assert__( result );
		updateRelativeTimeWidgets();
		updateDateWidgets();

	    emit FilterCompositionChanged( mFilter->Name() );
	}
}


void CBratFilterControls::HandleRelativeReferenceTimeChanged( const QDateTime &ref_datetime )
{
	if ( !mFilter->UseCurrentTime() && ( ref_datetime.isNull() || !ref_datetime.isValid() ) )
	{
		SimpleErrorBox( "Invalid date-time reference." );
		mRefDateTimeEdit->setDateTime( mFilter->RelativeReferenceTime() );
		return;
	}

	QDateTime dt = CBratFilter::CorrectDateTime( ref_datetime );

    // Re-calculate start and stop dates using new reference date
    // 
	bool result = mFilter->UpdateRelative2AbsoluteTimes( mFilter->RelativeStartDays(), mFilter->RelativeStopDays(), mFilter->UseCurrentTime(), dt );	assert__( result );
	updateRelativeTimeWidgets();		//TODO: DELETE THIS OR CHECK 
	updateDateWidgets();

    emit FilterCompositionChanged( mFilter->Name() );
}


////////////////////
///
////////////////////

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
    // BLOCK SIGNALS
    // 
    // Signals are blocked, otherwise HandleStartDateTimeChanged or HandleStopDateTimeChanged are called
    // 
    mStartTimeEdit->blockSignals( true );
    mStopTimeEdit->blockSignals( true );

    // UPDATE max/min allowed
    // 
    //  Stop_datetime defines the maximum allowed start_datetime
    //  
	mStartTimeEdit->setMaximumDateTime( mFilter->StopTime() );		assert__( CBratFilter::CorrectDateTime( mFilter->StopTime() ) == mFilter->StopTime() );

	// Start_datetime defines the minimum allowed stop_datetime
	// 
    mStopTimeEdit->setMinimumDateTime( mFilter->StartTime() );		assert__( CBratFilter::CorrectDateTime( mFilter->StartTime() ) == mFilter->StartTime() );


	// UPDATE start/stop
	// 
	mStartTimeEdit->setDateTime( mFilter->StartTime() );
    mStopTimeEdit->setDateTime( mFilter->StopTime() );


    // UNBLOCK SIGNALS
    // 
    mStartTimeEdit->blockSignals( false );
    mStopTimeEdit->blockSignals( false );
}


void CBratFilterControls::updateCyclePassWidgets()
{
	assert__( mFilter );

    // UPDATE start/stop Cycle and Pass
    mStartCycleEdit->setText( isDefaultValue(mFilter->StartCycle()) ? "" : n2q(mFilter->StartCycle()) );
    mStopCycleEdit->setText(  isDefaultValue(mFilter->StopCycle())  ? "" : n2q(mFilter->StopCycle()) );
    mStartPassEdit->setText( isDefaultValue(mFilter->StartPass()) ? "" : n2q(mFilter->StartPass()) );
    mStopPassEdit->setText(  isDefaultValue(mFilter->StopPass())  ? "" : n2q(mFilter->StopPass()) );

    // SET READ_ONLY PALETTE
    const bool markAsWarning = mFilter->InvalidCyclePassValues();
    QPalette *palette_warning = new QPalette();
	QString text = cycle_pas_radio_text;
	if ( markAsWarning )
    {
        //palette_warning->setColor(QPalette::Base, Qt::lightGray );
        palette_warning->setColor(QPalette::Text,Qt::red);
		text += " (4 values required)";
    }
    mStartCycleEdit->setPalette( *palette_warning );
    mStopCycleEdit->setPalette( *palette_warning );
    mStartPassEdit->setPalette( *palette_warning );
    mStopPassEdit->setPalette( *palette_warning );
	mUseCyclePassRadio->setText( text );
}

void CBratFilterControls::updateRelativeTimeWidgets()
{
	assert__( mFilter );

	// Signal blocking only to avoid redundant events; if all is
	// accurate (inside filters and in the relative time widgets
	// processing) signal blocking can be commented out and all
	// the rest remains the same. Can serve as test for later
	// changes.
	// 
	mRelativeStart->blockSignals( true );
	mRelativeStop->blockSignals( true );
	mRefDateTimeEdit->blockSignals( true );

	int start = mFilter->RelativeStartDays();
	mRelativeStart->setText( isDefaultValue( start ) ? "" : n2q( start ) );

	int stop = mFilter->RelativeStopDays();
	mRelativeStop->setText( isDefaultValue( stop ) ? "" : n2q( stop ) );

	QDateTime dt = mFilter->RelativeReferenceTime();
	mRefDateTimeEdit->setDateTime( dt );

    mUseCurrentDateTime->setChecked( mFilter->UseCurrentTime() );

	mRelativeStart->blockSignals( false );
	mRelativeStop->blockSignals( false );
	mRefDateTimeEdit->blockSignals( false );
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_BratFilterControls.cpp"
