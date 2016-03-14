#include "new-gui/brat/stdafx.h"

#include "libbrathl/ProductNetCdf.h"

#include "DataModels/Filters/BratFilters.h"
#include "DataModels/Workspaces/Dataset.h"

#include "ApplicationLogger.h"

#include "DatasetFilterControls.h"



/////////////////////////////////////////////////////////////////////////////////////
//							Dataset Filter Controls
/////////////////////////////////////////////////////////////////////////////////////


void CDatasetFilterControls::CreateWidgets()
{
    // I. Top buttons row
    //
	mNewFilter = CreateToolButton( "", ":/images/OSGeo/new.png", "Create a new filter" );
	mRenameFilter = CreateToolButton( "", ":/images/OSGeo/edit.png", "Rename the selected filter" );
	mDeleteFilter = CreateToolButton( "", ":/images/OSGeo/workspace-delete.png", "Delete the selected filter" );
	mSaveFilters = CreateToolButton( "", ":/images/OSGeo/save.png", "Save values in selected filter" );

    mFiltersCombo = new QComboBox;
    mFiltersCombo->setToolTip( "Select Filter" );

	QWidget *buttons_row = CreateButtonRow( false, Qt::Horizontal, { mNewFilter, mDeleteFilter, mSaveFilters, nullptr, new QLabel( "Selected Filter"), mFiltersCombo } );

	AddTopWidget( buttons_row );
	AddTopSpace( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding );


    // II. "Where" Description group
    //
	mClearWhere = CreateToolButton( "", ":/images/themes/default/clearInput.png", "Clear values" );
	auto *where_l = new QLabel( "Where" );
	QFont font = where_l->font();
	font.setBold( true );
	where_l->setFont( font );
	AddTopLayout( ELayoutType::Horizontal, { WidgetLine( nullptr, Qt::Horizontal ), where_l, WidgetLine( nullptr, Qt::Horizontal ), mClearWhere }, s, m, m, m, m );

    //    II.1 List of Areas
    QListWidget *areas_list = CreateBooleanList( this, { { "Lake Baikal", true }, { "Black Sea" }, { "User Area 1", true }, { "User Area 2" } } );
    areas_list->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    QGroupBox *areas_box = CreateGroupBox( ELayoutType::Horizontal, { areas_list }, "Areas", this );

    //    II.2 Buttons for region selection
    mSavedRegionsCombo = new QComboBox;
    mSavedRegionsCombo->setToolTip( "List of saved regions" );

    auto mKML     = new QPushButton( "KML" );
    auto mMask    = new QPushButton( "Mask" );

    QBoxLayout *buttons_regions = LayoutWidgets( Qt::Horizontal, { mSavedRegionsCombo, mKML, mMask } );

    //    II.3 Coordinates (max and min values)
    mMaxLatEdit = new QLineEdit(this);
    mMaxLonEdit = new QLineEdit(this);
    mMinLatEdit = new QLineEdit(this);
    mMinLonEdit = new QLineEdit(this);

    QBoxLayout *coord_values_l = LayoutWidgets( Qt::Vertical, {
                                             LayoutWidgets( Qt::Horizontal, { new QLabel( "Max Lat (deg)" ), mMaxLatEdit } ),
                                             LayoutWidgets( Qt::Horizontal, { new QLabel( "Min Lat (deg)" ), mMinLatEdit } )
                                               } );

    QBoxLayout *coord_values_r = LayoutWidgets( Qt::Vertical, {
                                             LayoutWidgets( Qt::Horizontal, { new QLabel( "Max Lon (deg)" ), mMaxLonEdit } ),
                                             LayoutWidgets( Qt::Horizontal, { new QLabel( "Min Lon (deg)" ), mMinLonEdit } )
                                               } );

    QBoxLayout *coord_values = LayoutWidgets( Qt::Horizontal, { coord_values_l, nullptr, coord_values_r }, nullptr );


    //    II.4 Adding previous widgets to this...

    QGroupBox *regions_coord = CreateGroupBox(ELayoutType::Vertical, {buttons_regions, coord_values} );
    regions_coord->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );

    mWhereBox = AddTopLayout( ELayoutType::Horizontal, { areas_box, regions_coord }, s, m, m, m, m );

	AddTopSpace( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding );


    // III. "When" Description group
    //
	auto *when_l = new QLabel( "When" );
	when_l->setFont( font );
	mClearWhen = CreateToolButton( "", ":/images/themes/default/clearInput.png", "Clear values" );
	AddTopLayout( ELayoutType::Horizontal, { WidgetLine( nullptr, Qt::Horizontal ), when_l, WidgetLine( nullptr, Qt::Horizontal ), mClearWhen }, s, m, m, m, m );

    //    III.1 Dates, Cycles and Pass (start and stop values)

    mStartTimeEdit  = new QDateTimeEdit;			mStartTimeEdit->setCalendarPopup(true);
    mStopTimeEdit  = new QDateTimeEdit;				mStopTimeEdit->setCalendarPopup(true);
    mStartCycleEdit = new QLineEdit(this);
    mStopCycleEdit  = new QLineEdit(this);
    mStartPassEdit  = new QLineEdit(this);
    mStopPassEdit   = new QLineEdit(this);

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

    mWhenBox = LayoutWidgets( Qt::Horizontal, {dates_box, nullptr, cycles_box, nullptr, pass_box});


    //   III.2 One-Click Time Filtering

	auto *one_click_title = 
		LayoutWidgets( Qt::Horizontal, { WidgetLine( nullptr, Qt::Horizontal ), new QLabel( "One-Click Time Filtering" ), WidgetLine( nullptr, Qt::Horizontal ) }, nullptr, s, m, m, m, m );

    //    Checkable menu items --> ATTENTION: are exclusive checkable menu items??
    auto last_month  = new QCheckBox( "Last Month" );
    auto last_year   = new QCheckBox( "Last Year" );
    auto last_cycle  = new QCheckBox( "Last Cycle" );
    auto *month_year_cycle_layout = LayoutWidgets( Qt::Vertical, {last_month, last_year, last_cycle} );

    QFrame *lineVertical_1 = WidgetLine( nullptr, Qt::Vertical );
    QFrame *lineVertical_2 = WidgetLine( nullptr, Qt::Vertical );

    auto mRelativeStart  = new QLineEdit(this);
    auto mRelativeStop   = new QLineEdit(this);
    QBoxLayout *relative_times = LayoutWidgets( Qt::Vertical, {
                                                 LayoutWidgets( Qt::Horizontal, { new QLabel( "Relative Start" ), mRelativeStart } ),
                                                 LayoutWidgets( Qt::Horizontal, { new QLabel( "Relative Stop" ),  mRelativeStop  } )
                                                } );

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

	AddTopSpace( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding );


    // IV. "Total Nb Records Selected" Description group
    //
    mTotalRecordsSelectedEdit = new QLineEdit( this );
	mTotalRecordsSelectedEdit->setReadOnly( true );
	mTotalRecordsSelectedEdit->setAlignment( Qt::AlignCenter );
    mTotalRecordsSelectedEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    QBoxLayout *TotalRecords_box = LayoutWidgets( Qt::Horizontal, { new QLabel( "Total Number of Records Selected" ), mTotalRecordsSelectedEdit });
    TotalRecords_box->setAlignment(Qt::AlignRight);

    //    Adding previous widgets to this...
    AddTopGroupBox( ELayoutType::Horizontal, { TotalRecords_box});


    // IV. a. v3 Inherited Selection Criteria... (TBD)
    //
    auto mDefineSelectionCriteria = new QPushButton( "Define..." );
    auto mApplySelectionCriteria = new QPushButton( "Apply..." );
    auto mShowSelectionReport = new QPushButton( "Report..." );

    AddTopGroupBox( ELayoutType::Horizontal, { mDefineSelectionCriteria, mApplySelectionCriteria, mShowSelectionReport }, "Selection Criteria" );

    AddTopWidget( WidgetLine( nullptr, Qt::Horizontal ) );


    AddTopSpace( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding );

	Wire();
}


void CDatasetFilterControls::Wire()
{
	connect( mNewFilter, SIGNAL( clicked() ), this, SLOT( HandleNewFilter() ) );
	connect( mRenameFilter, SIGNAL( clicked() ), this, SLOT( HandleRenameFilter() ) );
	connect( mDeleteFilter, SIGNAL( clicked() ), this, SLOT( HandleDeleteFilter() ) );
	connect( mSaveFilters, SIGNAL( clicked() ), this, SLOT( HandleSaveFilters() ) );

	connect( mFiltersCombo, SIGNAL( currentIndexChanged(int) ), this, SLOT( HandleFiltersCurrentIndexChanged(int) ) );

	connect( mClearWhere, SIGNAL( clicked() ), this, SLOT( HandleClearWhere() ) );
	connect( mClearWhen, SIGNAL( clicked() ), this, SLOT( HandleClearWhen() ) );

	connect( mMap, SIGNAL( CurrentLayerSelectionChanged() ), this, SLOT( HandleCurrentLayerSelectionChanged() ) );
}


//explicit
CDatasetFilterControls::CDatasetFilterControls( CDesktopManagerBase *manager, CBratFilters &brat_filters, QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0
    : base_t( manager, parent, f )
	, mMap( manager->Map() )
	, mBratFilters( brat_filters )
{
	CreateWidgets();

	if ( !ReloadFilters() )
	{
		setEnabled( false );
		LOG_WARN( "Brat filters cold not be loaded. Please check the filters configuration file " + mBratFilters.FilePath() );
	}
}


bool CDatasetFilterControls::ReloadFilters()
{
	if ( !mBratFilters.Load() )
	{
		return false;
	}
	FillFiltersCombo();
	return true;
}


void CDatasetFilterControls::FillFiltersCombo()
{
	mFiltersCombo->clear();

	auto const &filters = mBratFilters.FiltersMap();
	for ( auto const &filter : filters )
		mFiltersCombo->addItem( filter.first.c_str() );
}


void CDatasetFilterControls::HandleCurrentLayerSelectionChanged()
{
	QgsRectangle box = mMap->CurrentLayerSelectedBox();

	mMaxLatEdit->setText( n2q( box.yMaximum() ) );
    mMaxLonEdit->setText( n2q( box.xMaximum() ) );
    mMinLatEdit->setText( n2q( box.yMinimum() ) );
    mMinLonEdit->setText( n2q( box.xMinimum() ) );
}


//public slots:
void CDatasetFilterControls::HandleWorkspaceChanged( CWorkspaceDataset *wksd )
{
    mWks = wksd;
	mFilter = nullptr;
	mDataset = nullptr;
	mFiltersCombo->clear();
	if ( mWks )
		ReloadFilters();
}


void CDatasetFilterControls::HandleNewFilter()
{
	auto result = SimpleInputString( "Filter Name", mBratFilters.MakeNewName(), "New Filter" );
	if ( !result.first )
		return;

	if ( !mBratFilters.AddFilter( result.second ) )
		SimpleMsgBox( "A filter with same name already exists." );
	else
	{
		FillFiltersCombo();
		mFiltersCombo->setCurrentIndex( mFiltersCombo->findText( result.second.c_str() ) );

		// TODO Refresh all other pertinent widgets
	}
}
void CDatasetFilterControls::HandleRenameFilter()
{
	assert__( mFilter );

	auto result = SimpleInputString( "Filter Name", mFilter->Name(), "Rename Filter" );
	if ( !result.first )
		return;

	if ( !mBratFilters.RenameFilter( mFilter->Name(), result.second ) )
		SimpleMsgBox( "A filter with same name already exists." );
	else
	{
		FillFiltersCombo();
		mFiltersCombo->setCurrentIndex( mFiltersCombo->findText( result.second.c_str() ) );

		// TODO Refresh all other pertinent widgets
	}
}
void CDatasetFilterControls::HandleDeleteFilter()
{
	assert__( mFilter );

	if ( SimpleQuestion( "Are you sure you want to delete filter '" + mFilter->Name() + "' ?" ) )
	{
		if ( !mBratFilters.DeleteFilter( mFilter->Name() ) )
			SimpleErrorBox( "Filter '" + mFilter->Name() + "' was not found!" );
		else
		{
			FillFiltersCombo();
			mFiltersCombo->setCurrentIndex( 0 );
		}
	}
}
void CDatasetFilterControls::HandleSaveFilters()
{
	assert__( mFilter );

	// TODO validation

    mFilter->MinLon() = s2n< double >( q2a( mMaxLatEdit->text() ) );
    mFilter->MaxLon() = s2n< double >( q2a( mMaxLonEdit->text() ) );
    mFilter->MinLat() = s2n< double >( q2a( mMinLatEdit->text() ) );
    mFilter->MaxLat() = s2n< double >( q2a( mMinLonEdit->text() ) );

    mFilter->StartTime() = mStartTimeEdit->dateTime();
    mFilter->StopTime() = mStopTimeEdit->dateTime();

    mFilter->StartCycle() = s2n< int >( q2a( mStartCycleEdit->text() ) );
    mFilter->StopCycle() = s2n< int >( q2a( mStopCycleEdit->text() ) );
    mFilter->StartPass() = s2n< int >( q2a( mStartPassEdit->text() ) );
    mFilter->StopPass() = s2n< int >( q2a( mStopPassEdit->text() ) );

	if ( !mBratFilters.Save() )
		SimpleWarnBox( "There was a problem saving filters to '" + mBratFilters.FilePath() + "'. Some information could be lost or damaged." );
}


void CDatasetFilterControls::HandleFiltersCurrentIndexChanged( int filter_index ) 
{
	mRenameFilter->setEnabled( filter_index >= 0 );
	mDeleteFilter->setEnabled( filter_index >= 0 );
	mSaveFilters->setEnabled( filter_index >= 0 );
	mWhenBox->setEnabled( filter_index >= 0 );
	mWhereBox->setEnabled( filter_index >= 0 );

	if ( filter_index < 0 )
	{
		return;
	}

	std::string name = q2a( mFiltersCombo->itemText( filter_index ) );
	CBratFilter *filter = mBratFilters.Find( name );
	if ( filter == mFilter )
		return;

	mFilter = filter;

	mMinLonEdit->setText( n2s< std::string >( mFilter->MinLon() ).c_str() );
	mMaxLonEdit->setText( n2s< std::string >( mFilter->MaxLon() ).c_str() );
	mMinLatEdit->setText( n2s< std::string >( mFilter->MinLat() ).c_str() );
	mMaxLatEdit->setText( n2s< std::string >( mFilter->MaxLat() ).c_str() );

	mStartTimeEdit->setDateTime( mFilter->StartTime() );
    mStopTimeEdit->setDateTime( mFilter->StopTime() );

    mStartCycleEdit->setText( n2s< std::string >( mFilter->StartCycle() ).c_str() );
    mStopCycleEdit->setText( n2s< std::string >( mFilter->StopCycle() ).c_str() );

    mStartPassEdit->setText( n2s< std::string >( mFilter->StartPass() ).c_str() );
    mStopPassEdit->setText( n2s< std::string >( mFilter->StopPass() ).c_str() );
}


void CDatasetFilterControls::HandleClearWhere()
{
	NOT_IMPLEMENTED;
}


void CDatasetFilterControls::HandleClearWhen()
{
	NOT_IMPLEMENTED;
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



inline std::string MakeAlias( const std::string &aname )
{
	return "%{" + aname + "}";
}

static const std::string lon_name = "lon";
static const std::string lat_name = "lat";
static const std::string time_name = "time";

static const std::string lon_alias = MakeAlias( lon_name );
static const std::string lat_alias = MakeAlias( lat_name );
static const std::string time_alias = MakeAlias( time_name );

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
		can_use_alias ? lon_alias.c_str()	: lon_name.c_str(),
		can_use_alias ? lat_alias.c_str()	: lat_name.c_str(),
		can_use_alias ? time_alias.c_str()	: time_name.c_str()
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


CField* CControlPanel::FindField( CProduct *product, const std::string &name, bool &alias_used, std::string &field_error_msg )
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

	auto *alias = product->GetAlias( ToLowerCopy( name ) );
	if ( alias )
	{
		field = product->FindFieldByName( alias->GetValue(), false, &field_error_msg );		//true: throw on failure
		//guessing
		if ( !field && !record.empty() )
			field = product->FindFieldByName( alias->GetValue(), record, false );	//true: throw on failure
		if ( !field )
			field = product->FindFieldByInternalName( alias->GetValue(), false );	//true: throw on failure
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
	}
	return field;
}

std::pair<CField*, CField*> CControlPanel::FindLonLatFields( CProduct *product, bool &alias_used, std::string &field_error_msg )
{
	std::pair<CField*, CField*> fields;
	fields.first = FindField( product, lon_name, alias_used, field_error_msg );
	fields.second = FindField( product, lat_name, alias_used, field_error_msg );
	return fields;
}

CField* CControlPanel::FindTimeField( CProduct *product, bool &alias_used, std::string &field_error_msg )
{
	return FindField( product, time_name, alias_used, field_error_msg );
}



void CDatasetFilterControls::HandleDatasetChanged( CDataset *dataset )
{    
    //lambdas

    auto debug_log = []( const std::string &msg )
    {
        LOG_INFO( msg  );
        qApp->processEvents();
    };


    //function body

	mMap->RemoveLayers();
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
			CProduct *product = mDataset->SetProduct( path );
			total_records += product->GetNumberOfRecords();
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

			int ReturnCode = ReadTrack( alias_used, path, record, lonv, lon_dim, latv, lat_dim, timev, time_dim, time ? 3 : 2 );
			if ( ReturnCode == BRATHL_SUCCESS )
			{
				assert__( lon_dim == lat_dim );				
				assert__( !is_netcdf || ( lon_dim == expected_lon_dim && lat_dim == expected_lat_dim ) );

				debug_log( "Normalizing longitudes..." );

				for ( size_t i = 0; i < lon_dim; ++i )
					lonv[ i ] = CTools::NormalizeLongitude( -180.0, lonv[ i ] );

				debug_log( "About to plot..." );

				mMap->PlotTrack( lonv, latv, timev, lon_dim, QColor( 255, std::abs( (long)lonv[ 0 ] ) % 255, std::abs( (long)latv[ 0 ] ) % 255 ) );

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
