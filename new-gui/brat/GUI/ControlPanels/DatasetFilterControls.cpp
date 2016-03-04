#include "new-gui/brat/stdafx.h"

#include "DataModels/Workspaces/Dataset.h"

#include "ApplicationLogger.h"

#include "DatasetFilterControls.h"



/////////////////////////////////////////////////////////////////////////////////////
//								Dataset Filter
/////////////////////////////////////////////////////////////////////////////////////



//explicit
CDatasetFilterControls::CDatasetFilterControls( CDesktopManagerBase *manager, QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0
    : base_t( manager, parent, f )
	, mMap( manager->Map() )
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

    auto mKML     = new QPushButton( "KML" );
    auto mMask    = new QPushButton( "Mask" );

    QBoxLayout *buttons_regions = LayoutWidgets( Qt::Horizontal, { mSavedRegionsCombo, mKML, mMask } );

    //    I.3 Coordinates (max and min values)
    mMaxLat = new QLineEdit(this);
    mMaxLon = new QLineEdit(this);
    mMinLat = new QLineEdit(this);
    mMinLon = new QLineEdit(this);

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
                        LayoutWidgets( Qt::Horizontal, { monthYearCycleGroup, lineVertical_1, relative_times, lineVertical_2, refDateBox }
						) },"When", 4, 4, 4, 4, 4 );


    // III. "Total Nb Records Selected" Description group
    //
    auto mTotalRecordsSelected  = new QLineEdit(this);
    mTotalRecordsSelected->setText("0");
    mTotalRecordsSelected->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    QBoxLayout *TotalRecords_box = LayoutWidgets( Qt::Horizontal, { new QLabel( "Total Nb Records selected" ), mTotalRecordsSelected });
    TotalRecords_box->setAlignment(Qt::AlignRight);

    //    Adding previous widgets to this...
    AddTopGroupBox( ELayoutType::Horizontal, { TotalRecords_box});


    // III. a. v3 Inherited Selection Criteria... (TBD)
    //
    auto mDefineSelectionCriteria = new QPushButton( "Define..." );
    auto mApplySelectionCriteria = new QPushButton( "Apply..." );
    auto mShowSelectionReport = new QPushButton( "Report..." );

    AddTopGroupBox( ELayoutType::Horizontal, { mDefineSelectionCriteria, mApplySelectionCriteria, mShowSelectionReport }, "Selection Criteria" );

    AddTopWidget( WidgetLine( nullptr, Qt::Horizontal ) );


    // IV. Filter name description group
    //
    auto mFilterName  = new QLineEdit(this);
    mFilterName ->setText("Filter Name");

    auto mNewFilter     = new QPushButton( "New Filter" );
    auto mDeleteFilter  = new QPushButton( "Delete Filter" );
    auto mSaveFilter    = new QPushButton( "Save Filter" );

    mOpenFilterCombo = new QComboBox;
    mOpenFilterCombo->setToolTip( "Open a Filter" );

    AddTopLayout( ELayoutType::Horizontal, {  mFilterName, mNewFilter, mDeleteFilter, mSaveFilter, mOpenFilterCombo } );


    AddTopSpace( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding );

	Wire();
}


void CDatasetFilterControls::Wire()
{
	connect( mMap, SIGNAL( CurrentLayerSelectionChanged() ), this, SLOT( HandleCurrentLayerSelectionChanged() ) );
}


void CDatasetFilterControls::HandleCurrentLayerSelectionChanged()
{
	QgsRectangle box = mMap->CurrentLayerSelectedBox();

	mMaxLat->setText( n2q( box.yMaximum() ) );
    mMaxLon->setText( n2q( box.xMaximum() ) );
    mMinLat->setText( n2q( box.yMinimum() ) );
    mMinLon->setText( n2q( box.xMinimum() ) );
}





//public slots:
void CDatasetFilterControls::HandleWorkspaceChanged( CWorkspaceDataset *wksd )
{
    mWks = wksd;
}




//////////////////////////////////////////////////////////////////////////////////////////////
//		Satellite Tracks Processing TODO change place???
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
int ReadTrack( const std::string &path, const std::string &record, double *&x, size_t &sizex, double *&y, size_t &sizey )
{
    static const char *Expressions[] = { "lon", "lat" };
    static const char *Units[] = { "count", "count" };

	char *Names[] = { const_cast<char*>( path.c_str() ) };

    double	*Data[ 2 ]	= { nullptr, nullptr };
    int32_t	Sizes[ 2 ]	= { -1, -1 };

    size_t	ActualSize;


    int ReturnCode = CProduct::ReadData(
                1, Names,
                record.c_str(),		//"data",                
                nullptr,			//"latitude > 20 && latitude < 30",
                2,
                const_cast< char** >( Expressions ),
                const_cast< char** >( Units ),
                Data,
                Sizes,
                &ActualSize,
                0,
                0,
                defaultValue< double >()
                );

    LOG_TRACEstd( "Return code          : " + n2s<std::string>( ReturnCode ) );
    LOG_TRACEstd( "Actual number of data: " + n2s<std::string>( ActualSize ) );

    x = Data[ 0 ];
    y = Data[ 1 ];

    sizex = sizey = ActualSize;

    return ReturnCode;
}




void CDatasetFilterControls::HandleDatasetChanged( CDataset *dataset )
{    
    //statics

    static const std::string lon_alias = "lon";
    static const std::string lat_alias = "lat";

    //lambdas

    auto debug_log = []( const std::string &msg )
    {
        LOG_INFO( msg  );
        qApp->processEvents();
    };


    //function body

    if ( !dataset || mDataset != dataset )
    {
        mMap->RemoveLayers();

        if ( mDataset == dataset )
            return;

        mDataset = dataset;
        if ( !mDataset )
            return;
    }

	std::string error_msg;

    std::vector< std::string > paths;    mDataset->GetFiles( paths );
	for ( auto &path : paths )
	{
		WaitCursor wait;
		try
		{
			CProduct *product = mDataset->SetProduct( path );
			bool skip_iteration = !product || !product->HasAliases();
			if ( skip_iteration )
			{
				LOG_WARN( "Aliases not supported reading file " + path );
			}
			const bool is_netcdf = product->IsNetCdfOrNetCdfCFProduct();

			std::string field_error_msg;
			CField *lon = product->FindFieldByName( lon_alias, false, &field_error_msg );	//true: throw on failure
			CField *lat = product->FindFieldByName( lat_alias, false, &field_error_msg );
			if ( !lon || !lat )
			{
				skip_iteration = true;
				LOG_WARN( field_error_msg + " - File " + path );
			}

			auto expected_lon_dim = lon ? lon->GetDim()[ 0 ] : 0;
			auto expected_lat_dim = lat ? lat->GetDim()[ 0 ] : 0;
			if ( expected_lon_dim != expected_lat_dim )
			{
				skip_iteration = true;
				LOG_WARN( "Different latitude/longitude dimensions in file " + path );
			}

			std::string record;
			if ( !is_netcdf )
			{
				auto *aliases = product->GetAliases();
				if ( aliases )
					record = aliases->GetRecord();
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
			double *lonv = nullptr;
			double *latv = nullptr;

			debug_log( "About to read variables from file " + path );

			int ReturnCode = ReadTrack( path, record, lonv, lon_dim, latv, lat_dim );
			if ( ReturnCode == BRATHL_SUCCESS )
			{
				assert__( lon_dim == lat_dim );				
				assert__( !is_netcdf || ( lon_dim == expected_lon_dim && lat_dim == expected_lat_dim ) );

				debug_log( "Normalizing longitudes..." );

				for ( size_t i = 0; i < lon_dim; ++i )
					lonv[ i ] = CTools::NormalizeLongitude( -180.0, lonv[ i ] );

				debug_log( "About to plot..." );

				mMap->PlotTrack( lonv, latv, lon_dim, QColor( 255, std::abs( (long)lonv[ 0 ] ) % 255, std::abs( (long)latv[ 0 ] ) % 255 ) );

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

	if ( !error_msg.empty() )
	{
		SimpleErrorBox( mDataset ? 
			( "Problems reading satellite tracks from " + mDataset->GetName() + ":\n" + error_msg )
			: error_msg );
	}
}







///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_DatasetFilterControls.cpp"
