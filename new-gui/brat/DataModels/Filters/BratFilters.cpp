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

#include "new-gui/Common/QtUtils.h"

#include "libbrathl/Product.h"
#include "libbrathl/ProductNetCdf.h"
#include "libbrathl/CyclePassConverter.h"

using namespace brathl;

#include "BratLogger.h"

#include "BratFilters.h"



static const std::string NAME_KEY = "Name";

static const std::string FILTER_AREAS_KEY = "FilterAreas";

static const std::string USING_RELATIVE_TIMES_KEY = "UsingRelativeTimes";
static const std::string USING_CYCLE_PASS_KEY = "UsingDateTime";
static const std::string STARTDATE_KEY = "StartDate";
static const std::string STOPDATE_KEY = "StopDate";

static const std::string STARTCYCLE_KEY = "StartCycle";
static const std::string STOPCYCLE_KEY = "StopCycle";
static const std::string STARTPASS_KEY = "StartPass";
static const std::string STOPPASS_KEY = "StopPass";

static const std::string RELATIVE_START_DAYS_KEY = "RelativeStartDays";
static const std::string RELATIVE_STOP_DAYS_KEY = "RelativeStopDays";
static const std::string USE_CURRENT_TIME_KEY = "UseCurrentTime";
static const std::string RELATIVE_REFERENCE_TIME = "RelativeReferenceTime";


static const std::string date_time_format = "yyyy/MM/dd T hh:mm:ss";

static const std::string FILTER_BASE_NAME = "Filter";

static const std::string DATASET_SELECTION_LOG_FILENAME = "DatasetSelection.log";



//////////////////////////////////////////////////////////////
//						Single Filter
//////////////////////////////////////////////////////////////

//static 
const bool CBratFilter::smUsingRelativeTimes = false;

//static 
const bool CBratFilter::smUsingCyclePass = false;

// Setting Brathl internal reference date year (1950)
//
//static
const QDateTime CBratFilter::smStartTime = QDateTime( QDate(1950, 1, 1), QTime(0, 0, 0) );

//static
const int CBratFilter::smStartCycle = defaultValue< int >();
//static
const int CBratFilter::smStopCycle = defaultValue< int >();
//static
const int CBratFilter::smStartPass = defaultValue< int >();
//static
const int CBratFilter::smStopPass = defaultValue< int >();

//static
const bool CBratFilter::smUseCurrentTime = true;
//static
const int CBratFilter::smRelativeStartDays = defaultValue< int >();
//static
const int CBratFilter::smRelativeStopDays = defaultValue< int >();



CBratFilter& CBratFilter::operator = ( const CBratFilter &o )
{
    if ( this != &o )
    {
        mName = o.mName;

        mAreaNames = o.mAreaNames;

		mUsingRelativeTimes = o.mUsingRelativeTimes;
		mUsingCyclePass = o.mUsingCyclePass;
        mStartTime = o.mStartTime;
        mStopTime = o.mStopTime;

        mStartCycle = o.mStartCycle;
        mStopCycle = o.mStopCycle;
        mStartPass = o.mStartPass;
        mStopPass = o.mStopPass;

        mRelativeStartDays = o.mRelativeStartDays;
        mRelativeStopDays = o.mRelativeStopDays;
    }
    return *this;
}



bool CBratFilter::FindArea( const std::string &name ) const
{
    return std::find( mAreaNames.begin(), mAreaNames.end(), name ) != mAreaNames.end();
}


bool CBratFilter::AddArea( const std::string &name )
{
    if ( FindArea( name ) )
        return false;

    mAreaNames.push_back( name );

    return true;
}


bool CBratFilter::RemoveArea( const std::string &name )
{
    if ( !FindArea( name ) )
        return false;

    auto it = std::find( mAreaNames.begin(), mAreaNames.end(), name );		assert__( it != mAreaNames.end() );

    mAreaNames.erase( it );

    return true;
}


const CBratAreas& CBratFilter::Areas() const
{
    return CBratFilters::GetInstance().Areas();
}


// Returns false if the filter is empty (no areas)
//
// Take advantage of the previously calculated min, max on lon, lat.
// (FAST LDR)
//
bool CBratFilter::BoundingArea( double &lon1, double &lat1, double &lon2, double &lat2 ) const
{
	if ( mAreaNames.empty() )
	{
		lon1 = lat1 = lon2 = lat2 = defaultValue< double >();
		return false;
	}

	auto const &areas = Areas();	//returns a CBratAreas

	lon1 = std::numeric_limits<double>::max();
    lon2 = std::numeric_limits<double>::lowest();
    lat1 = std::numeric_limits<double>::max();
    lat2 = std::numeric_limits<double>::lowest();

    for ( auto const &name : mAreaNames )		//iterate over filter area names
    {
        auto const &area = areas.Find( name );	//find area object in the set of all areas; an area 'is a' vector

        if (area->GetLonMin() < lon1)
        {
            lon1= area->GetLonMin();
        }
        if (area->GetLonMax() > lon2)
        {
            lon2= area->GetLonMax();
        }
        if (area->GetLatMin() < lat1)
        {
            lat1= area->GetLatMin();
        }
        if (area->GetLatMax() > lat2)
        {
            lat2= area->GetLatMax();
        }
    }

	return true;
}


bool CBratFilter::GetTimeBounds( CDate &Start, CDate &Stop, const std::string &label_for_cycle_pass, std::string &error_msg ) const
{
    if ( UsingCyclePass() && !InvalidCyclePassValues() )
    {
        // 1- Uses Start/Stop Cycle/Pass defined by user

		const CCyclePassConverter &m = CCyclePassConverter::GetConverter( label_for_cycle_pass );
        if ( m.ErrorCode() == BRATHL_SUCCESS )
        {
            m.Convert( StartCycle(), StartPass(), Start);
            m.Convert( StopCycle(), StopPass(), Stop);
            
            qDebug() << "Cycle/Pass converted to " + t2q( Start.AsString() ) + " - " + t2q( Stop.AsString() );
            
            return true;
        }
        else // Mission not found
        {
            error_msg.append( "Unable to convert Cycle and Pass to date with '" + label_for_cycle_pass + "':\n" + m.ErrorMessages() );
            return false;
        }
    }
    else // 2- Uses Start and Stop datetimes defined by user
    {
        Start = BratStartTime();
        Stop  = BratStopTime();
        return true;
    }
}


// Argument label_for_cycle_pass must be the value returned by CProduct::GetLabelForCyclePass()
//
std::string CBratFilter::GetSelectionCriteriaExpression( const std::string &label_for_cycle_pass ) const
{
	//types

	using lon_normalize_t = double(*)(double);

	//statics

	// With abs_min_lon == 0: using longitudes range [0,360]
	// With abs_min_lon == -180: using longitudes range [-180,180]

	static const double abs_min_lon = 0;
	static const double abs_max_lon = abs_min_lon + 360;

	static lon_normalize_t lon_normalize = abs_min_lon == -180 ? 
		static_cast<lon_normalize_t>( &CLatLonPoint::LonNormal ) : 
		static_cast<lon_normalize_t>( &CLatLonPoint::LonNormal360 );

	static const std::string abs_min_lon_string = n2s( abs_min_lon );


	//nested lambdas

	// Creates the expression for 'is_bounded' function, which checks if a value x is 
	//	included between two values (min/max). The result is a string like 
	//
	//	"is_bounded( min, x, max )"
	//
	// used to translate filter parameters into selection criteria formula expressions.
	//
	auto is_bounded = []( double min, const std::string &arg_with_alias, double max ) -> std::string 
	{
		return
			"is_bounded( " 
			+ std::to_string( min ) + ", "
			+ arg_with_alias + ", "
			+ std::to_string( max ) + 
			" )";
	};

	
	// is_bounded (see above) for latitudes
	//
	auto lat_is_bounded = [&is_bounded]( double min, double max ) -> std::string 
	{
		return is_bounded( min, lat_alias(), max );
	};


	// is_bounded (see above) for normalized longitudes
	//
	auto lon_is_bounded = [&is_bounded]( double min, double max ) -> std::string 
	{
		auto lon_normalize_formula = []( const std::string &lon ) -> std::string 
		{
			return "deg_normalize( " + abs_min_lon_string + ", " + lon + " )";
		};

		return is_bounded( min, lon_normalize_formula( lon_alias() ), max );
	};


	//function body

	std::string expression;


	// LAT/LON filtering expression

    if ( !mAreaNames.empty() )
    {
        auto const &areas = Areas();

        // Iterate over filter areas names
		//
        for ( auto const &name : mAreaNames )
        {
            auto const &area  = areas.Find( name );

            double LatMin = area->GetLatMin();
            double LatMax = area->GetLatMax();

			//	We assume that all products have longitude fields in degrees east units.
			//	We use a longitude default range (see function statics section above) and 
			//	build the selection formula in order to normalize to this range the raw 
			//	longitudes read from product.
			//
			//	Examples of [-180, 180] range : Reaper, ...
			//	Examples of [0,360] range: Cryosat, Jason1&2, GRID_DOTS_MERCATOR, ...
			//
			double LonMin = lon_normalize( area->GetLonMin() );
			double LonMax = lon_normalize( area->GetLonMax() );

			if ( area->GetLonMin() != area->GetLonMax() && LonMin == LonMax )
            {
                LonMin = abs_min_lon;
                LonMax = abs_max_lon;
            }

            if ( !expression.empty() ) {  expression += " || ";  }

			// Brat expression:	(is_bounded(LatMin,Lat,LatMax) && is_bounded(LonMin,Lon,LonMax)) || ...
			//        which is:                             (AREA1)                              OR (AREA2) ....
			//
			expression += "(";
			expression += lat_is_bounded( LatMin, LatMax );
			expression += " && ";

			if ( LonMin > LonMax )
			{
				// Area crosses the line of longitude = 0
				expression += "(";
				expression += lon_is_bounded( LonMin, abs_max_lon );
				expression += " || ";
				expression += lon_is_bounded( abs_min_lon, LonMax );
				expression += ")";
			}
			else
			{
				expression += lon_is_bounded( LonMin, LonMax );
			}
			expression += ")";
        }
    }

    
	// TIME filtering expression

    CDate Start, Stop;
    std::string error_msg;
	if ( GetTimeBounds( Start, Stop, label_for_cycle_pass, error_msg ) )
    {
		// Although some products have time fields in seconds since 2000-01-01,in selection 
		// criteria formula the user shall insert time values in seconds since 1950-01-01.
		//
        double start_seconds, stop_seconds;
        Start.Convert2Second( start_seconds ); 
        Stop.Convert2Second( stop_seconds );   

        if ( !expression.empty() )
        {
            expression = "(" + expression + ")" + " && ";   //  ((AREA1) || (AREA2)) && ... (TIME_PERIOD)
        }
        expression += is_bounded( start_seconds, time_alias(), stop_seconds );
    }

    if (expression.empty()) { LOG_INFO( "Filter '" + mName + "' applied no selection criteria expression." ); }
    else                    { LOG_INFO( "Filter '" + mName + "' translated to a selection criteria expression:\n" + expression );}

    return expression;
}


// The 1st boolean return is the usual result
// The 2nd boolean return is true only if user canceled
// Whenever 2nd is true, 1st must be false
//
std::pair< bool, bool > CBratFilter::Apply( const CStringList& files_in, CStringList& files_out, std::string& error_msg, CProgressInterface *progress ) const
{
	auto result = CBratFilters::GetInstance().Apply( mName, files_in, files_out, error_msg, progress );	//variable created only for assertion

	assert__( !result.second || !result.first );

	return result;
}


void CBratFilter::Relative2AbsoluteTimes()
{
    // Check if relative times are default values
    if ( isDefaultValue(mRelativeStartDays) && isDefaultValue(mRelativeStopDays) )
        return;
    else if ( isDefaultValue(mRelativeStartDays) )
        mRelativeStartDays = mRelativeStopDays;
    else if ( isDefaultValue(mRelativeStopDays) )
        mRelativeStopDays = mRelativeStartDays;

    // Calculate Absolute Start and Stop times
    if ( mUseCurrentTime ){ mRelativeReferenceTime = QDateTime::currentDateTime(); }

    mStartTime = mRelativeReferenceTime.addDays( mRelativeStartDays );
    mStopTime  = mRelativeReferenceTime.addDays( mRelativeStopDays );
}


void CBratFilter::SetDefaultValues()
{
	mUsingRelativeTimes = false;	//absolute always valid (if not using cycle-pass)
	mUsingCyclePass = false;		//date-times always valid

    SetDefaultDateValues();

    SetDefaultCyclePassValues();

    SetDefaultRelativeDays();
}


void CBratFilter::SetDefaultDateValues()
{
	//set to valid values that filter nothing

	// Setting Brathl internal reference date year (1950): see smStartTime definition
    mStartTime = smStartTime;
    mStopTime  = QDateTime::currentDateTime();
}

bool CBratFilter::InvalidCyclePassValues() const
{
	return 
		mStartCycle == smStartCycle ||
		mStopCycle == smStopCycle ||
		mStartPass == smStartPass ||
		mStopPass == smStopPass;
}

void CBratFilter::SetDefaultCyclePassValues()
{
	//set to all invalid values

    mStartCycle = smStartCycle;
    mStopCycle = smStopCycle;
    mStartPass = smStartPass;
    mStopPass = smStopPass;
}

void CBratFilter::SetDefaultRelativeDays()
{
	//set to invalid values
	//
	mRelativeStartDays = smRelativeStartDays;
    mRelativeStopDays = smRelativeStopDays;

    mUseCurrentTime = smUseCurrentTime;
    mRelativeReferenceTime = QDateTime::currentDateTime();
}


//////////////////////////////////////////////////////////////
//						All Filters
//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////
// statics


CBratFilters *CBratFilters::smInstance = nullptr;


//static
const std::string& CBratFilters::FindAliasValue( CProduct *product, const std::string &alias_name )
{
	auto *alias = product->GetAlias( alias_name );
	if ( !alias )
		alias = product->GetAlias( ToLowerCopy( alias_name ) );	//TODO confirm aliases case sensitiveness

	if ( alias )
	{
		return alias->GetValue();
	}

	return empty_string();
}

// The std::string pair element is the alias value, if the name has a corresponding alias
// The CField pair element means is the corresponding variable in product, if found and the alias values is not a formula
//
//static
CAliasInfo CBratFilters::FindField( CProduct *product, const std::string &name, bool try_unsupported, bool &alias_used, std::string &field_error_msg )
{
	std::string record;
	if ( product->IsNetCdfOrNetCdfCFProduct() )
		record = CProductNetCdf::m_virtualRecordName;
	else
	{
		auto *aliases = product->GetAliases();
		if ( aliases )
			record = aliases->GetRecord();
	}

	CAliasInfo alias_info { empty_string(), nullptr };
	alias_used = true;

	alias_info.Value() = FindAliasValue( product, name );
	if ( !alias_info.Value().empty() )
	{
		alias_info.Field() = product->FindFieldByName( alias_info.Value(), false, &field_error_msg );		//true: throw on failure

																											//guessing ?

		if ( /*try_unsupported && */!alias_info.Field() && !record.empty() )
			alias_info.Field() = product->FindFieldByName( alias_info.Value(), record, false );	//true: throw on failure
		if ( !alias_info.Field() )
			alias_info.Field() = product->FindFieldByInternalName( alias_info.Value(), false );	//idem
	}

	//guessing

	if ( try_unsupported && !alias_info.Field() )
	{
		alias_used = false;
		alias_info.Field() = product->FindFieldByName( name, false, &field_error_msg );		//true: throw on failure
		if ( !alias_info.Field() && !record.empty() )
			alias_info.Field() = product->FindFieldByName( name, record, false );			//idem
		if ( !alias_info.Field() )
			alias_info.Field() = product->FindFieldByInternalName( name, false );			//idem
		if (!alias_info.Field() )
			alias_info.Field() = product->FindFieldByName( ToLowerCopy( name ), false, &field_error_msg );
	}

	return alias_info;
}


//static
std::pair<CAliasInfo, CAliasInfo> CBratFilters::FindLonLatFields( CProduct *product, bool try_unsupported, bool &lon_alias_used, bool &lat_alias_used, 
	std::string &field_error_msg )
{
	std::pair<CAliasInfo, CAliasInfo> fields;

	fields.first = FindField( product, lon_name(), try_unsupported, lon_alias_used, field_error_msg );

	std::string msg;
	fields.second = FindField( product, lat_name(), try_unsupported, lat_alias_used, msg );

	if ( !field_error_msg.empty() )
		field_error_msg += "\n";
	field_error_msg += msg;

	return fields;
}


//static
CAliasInfo CBratFilters::FindTimeField( CProduct *product, bool try_unsupported, bool &alias_used, std::string &field_error_msg )
{
	return FindField( product, time_name(), try_unsupported, alias_used, field_error_msg );
}




//////////////////////////////////////////////////////////////
// Filter Application


bool CBratFilters::Translate2SelectionCriteria( const std::string &label_for_cycle_pass, CProduct *product_ref, const std::string &name, std::string &error_msg ) const
{
	auto const *filter = Find( name );

	assert__( filter && product_ref );

	if ( !product_ref->HasCriteriaInfo() )
	{
		error_msg.append( "No criteria information found in product type '" + product_ref->GetLabel() + "'." );
		return false;
	}


	//  LatLon Criteria

	if ( product_ref->HasLatLonCriteria() && !filter->AreaNames().empty() )
	{
		double lon1, lat1, lon2, lat2;
		if ( filter->BoundingArea( lon1, lat1, lon2, lat2 ) )
		{
			product_ref->GetLatLonCriteria()->Set( lat1, lon1, lat2, lon2 );  // double latLow, double lonLow, double latHigh, double lonHigh

			LOG_INFO( "Filter '" + filter->Name() + "' applied a LatLonCriteria, Lat=[" + std::to_string( lat1 ) + "; " + std::to_string( lat2 ) +
				"] and Lon=[" + std::to_string( lon1 ) + "; " + std::to_string( lon2 ) + "]." );
		}
		else
		{
			LOG_INFO( "Filter '" + filter->Name() + "' areas list is empty. No space filtering applied." );
		}
	}

	
	// DateTime (and Cycle/Pass) Criteria

	if ( product_ref->HasDatetimeCriteria() )
	{
		CDate Start, Stop;

		if ( filter->GetTimeBounds( Start, Stop, label_for_cycle_pass, error_msg ) )
		{
			// Start and Stop.Value() contains the date in a number of seconds since internal reference date, ie 1950.
			product_ref->GetDatetimeCriteria()->Set( Start.Value(), Stop.Value() );
			LOG_INFO("Filter '" + filter->Name() + "' applied a DateTimeCriteria from: " + Start.AsString() + " to: " + Stop.AsString()
				+ ".\n(product type/label: '" + label_for_cycle_pass + "')");
		}
		else
		{
			// Unable to convert Cycle and Pass to date: no mission found for product type
			return false;
		}
	}

	/// NOT USED ON BRAT V.4 /// Cycle/Pass criteria are applied as datetime criteria, therefore the old criteria are not used.
	
	//	Next 2 criteria assignments (cycle and pass) assume that cycle and pass can be used like simple integers as they come from the GUI
	//if ( product_ref->HasCycleCriteria() )
	//{
	//product_ref->GetCycleCriteria()->Set( filter->StartCycle(), filter->StopCycle() );
	//}
	//if ( product_ref->HasPassIntCriteria() )
	//{
	//product_ref->GetPassIntCriteria()->Set( filter->StartPass(), filter->StopPass() );
	//}
	////	Apparently Cryosat uses this criterion; is it simply composed by integer passes converted to string??? I wonder...
	////	If not, we still don't have GUI support for this
	//if ( product_ref->HasPassStringCriteria() )
	//{
	////product_ref->GetPassStringCriteria()->Set( comma separated strings );
	//}

	///////////

	return true;
}


// The 1st boolean return is the usual result
// The 2nd boolean return is true only if user canceled
// Whenever 2nd is true, 1st must be false
//
std::pair< bool, bool > CBratFilters::Apply( const std::string &name, const CStringList& files_in, CStringList& files_out, std::string& error_msg, CProgressInterface *progress ) const
{
#if defined(BRAT_V3)
	return true;
#endif

	bool user_canceled = false;

	auto *filter = Find( name );		assert__( filter );
	if ( !filter )
		return{ false, user_canceled };

	CProduct* product = nullptr;
	bool filter_applied_complete = true;

	try
	{
		if ( files_in.size() > 0 )
			product = CProduct::Construct( *files_in.begin() );
		if ( !product )
		{
			error_msg.append( "Perhaps the dataset is empty." );
			return{ false, user_canceled };
		}

		product->GetProductList().clear();
		product->GetProductList().Insert( files_in );


		// 1. Check if product_ref is valid

		CMapProduct mapProduct;
		mapProduct.AddCriteriaToProducts();
		CProduct *product_ref = dynamic_cast< CProduct* >( mapProduct.Exists( product->GetLabel() ) );	//CMapProduct::GetInstance()

		if ( product_ref == nullptr )
		{
			error_msg.append( "Unable to get criteria information for product type '" + product->GetLabel() + " - "
				+ product->GetProductType() + "'." );
			return{ false, user_canceled };
		}


		// 2. Check if product has all required aliases

		// NOTE: This step can be deleted if we ensure that GetSelectionCriteriaExpression()
		// finds the field names case the aliases are not available (RCCC TODO: to be discussed)
		std::string lon_alias = FindAliasValue( product, lon_name() );
		std::string lat_alias = FindAliasValue( product, lat_name() );
		std::string time_alias = FindAliasValue( product, time_name() );

		if ( lon_alias.empty() || lat_alias.empty() || time_alias.empty() )
		{
			error_msg.append( "All required aliases (latitude, longitude and time) were not found for product type '"
				+ product->GetLabel() + " - "  + product->GetProductType()
				+ "'.\nPlease add missing information to aliases file." );
			return{ false, user_canceled };
		}


		// 3. Check if product has Lon, Lat and Time fields (required to filter data by location and time)

		std::string field_error_msg;

		CField *lon = product->FindFieldByName( lon_alias, false, &field_error_msg );
		if ( !lon )
		{
			error_msg.append( "No longitude data found in product." );
			return{ false, user_canceled };
		}

		CField *lat = product->FindFieldByName( lat_alias, false, &field_error_msg );
		if ( !lat )
		{
			error_msg.append( "No latitude data found in product." );
			return{ false, user_canceled };
		}

		CField *time = product->FindFieldByName( time_alias, false, &field_error_msg );
		if ( !time )
		{
			error_msg.append( "No time data found in product." );
			return{ false, user_canceled };
		}


		// 4. Translate Filter to selection criteria

		if ( !Translate2SelectionCriteria( product->GetLabelForCyclePass(), product_ref, name, error_msg ) )
		{
			// Unable to translate to selection criteria (no criteria info, unknown mission...)
			return{ false, user_canceled };
		}


		// 5. Add and apply Criteria (old Dataset Selection Criteria in Brat v.3) //
		product->AddCriteria( product_ref );
		std::string log_path = files_in.size() > 5000 ? empty_string() : mWorkspacesPath + "/" + DATASET_SELECTION_LOG_FILENAME;
		//files_out = files_in;
		if ( !product->ApplyCriteria( files_out, progress, log_path ) )
		{
			user_canceled = progress->Cancelled();

			error_msg.append( 
				user_canceled ? 
				"" :					//the user already knows what happened
				( log_path.empty() 	? 
					"An error occurred applying filter. Too much files to create a log file." : 
					( "An error occurred applying filter. Please see the log file " + log_path ) 
					)
			);

			filter_applied_complete = false;
		}
		else
		if ( files_out.size() < 1 )
		{
			error_msg.append( "No data available for filter criteria." );
			filter_applied_complete = false;
		}
		else
		{
			LOG_INFO( n2s( files_in.size() - files_out.size() ) + " files were excluded by filter '" + name + "'" );
			LOG_INFO( n2s( files_out.size() ) + " files remain for processing" );
		}
	}
	catch ( CException e )
	{
		filter_applied_complete = false;
	}
	catch ( ... )
	{
		filter_applied_complete = false;
	}


	delete product;

	return{ filter_applied_complete, user_canceled };
}



//////////////////////////////////////////////////////////////
// Filter Management


CBratFilter* CBratFilters::Find( const std::string &name )
{
    auto it = mFiltersMap.find( name );
    if ( it == mFiltersMap.end() )
        return nullptr;

    return &it->second;
}


std::string CBratFilters::MakeNewName() const
{
    size_t i = mFiltersMap.size();
    std::string key;

    do
    {
        key = FILTER_BASE_NAME + "_" + n2s<std::string>( i++ );
    }
    while ( Find( key ) != nullptr );

    return key;
}


bool CBratFilters::AddFilter( const std::string &name )
{
    if ( Find( name ) )
        return false;

    mFiltersMap.insert( { name, CBratFilter( name ) }  );

    return true;
}


bool CBratFilters::RenameFilter( const std::string &name, const std::string &new_name )
{
    if ( !Find( name ) || Find( new_name ) )
        return false;

    CBratFilter new_f = mFiltersMap.at( name );
    if ( !DeleteFilter( name ) )
    {
        LOG_TRACE( "Unexpected failure deleting filter" );
        assert__( false );
    }

    new_f.Name() = new_name;

    mFiltersMap.insert( { new_name, new_f } );

    return true;
}


bool CBratFilters::DeleteFilter( const std::string &name )
{
    if ( !Find( name ) )
        return false;

    mFiltersMap.erase( name );

    return true;
}



//////////////////////////////////////////////////////////////
// All Filters - filter persistence


bool CBratFilters::Save()
{
    Clear();

    for ( auto it = mFiltersMap.cbegin(); it != mFiltersMap.cend(); it++ )
    {
        auto const &filter = it->second;
        QStringList list;
        auto const &areas = filter.AreaNames();
        for ( auto &name : areas )
            list << t2q( name );

        WriteSection( it->first,

            k_v( FILTER_AREAS_KEY,	list ),

			k_v( USING_RELATIVE_TIMES_KEY,	filter.mUsingRelativeTimes ),

			k_v( USING_CYCLE_PASS_KEY,		filter.mUsingCyclePass ),
            k_v( STARTDATE_KEY,				filter.StartTime().toString( t2q( date_time_format ) ) ),
            k_v( STOPDATE_KEY,				filter.StopTime().toString( t2q( date_time_format ) ) ),

            k_v( STARTCYCLE_KEY,			filter.StartCycle() ),
            k_v( STOPCYCLE_KEY,				filter.StopCycle() ),
            k_v( STARTPASS_KEY,				filter.StartPass() ),
            k_v( STOPPASS_KEY,				filter.StopPass() ),

            k_v( RELATIVE_START_DAYS_KEY,	filter.RelativeStartDays() ),
            k_v( RELATIVE_STOP_DAYS_KEY,	filter.RelativeStopDays() ),
            k_v( USE_CURRENT_TIME_KEY,		filter.UseCurrentTime() ),
            k_v( RELATIVE_REFERENCE_TIME,	filter.RelativeReferenceTime().toString( t2q( date_time_format ) ) )
        );
    }

    return mSettings.status() == QSettings::NoError;
}


bool CBratFilters::Load()
{
    mFiltersMap.clear();
    QStringList groups = mSettings.childGroups();
    for ( auto const &group : groups )
    {
        auto name = q2a( group );
        mFiltersMap.insert( { name, CBratFilter( name ) }  );
    }

    if ( Status() != QSettings::NoError )
        return false;

    for ( auto it = mFiltersMap.begin(); it != mFiltersMap.end(); it++ )
    {
        QStringList list;
        std::string stime, etime, relative_ref_time;
        auto &filter = it->second;

        ReadSection( it->first,

            k_v( FILTER_AREAS_KEY,			&list ),

			k_v( USING_RELATIVE_TIMES_KEY,	&filter.mUsingRelativeTimes,	CBratFilter::smUsingRelativeTimes ),

			k_v( USING_CYCLE_PASS_KEY,		&filter.mUsingCyclePass,		CBratFilter::smUsingCyclePass ),
			k_v( STARTDATE_KEY,				&stime ),
            k_v( STOPDATE_KEY,				&etime ),

            k_v( STARTCYCLE_KEY,			&filter.StartCycle(),			CBratFilter::smStartCycle ),
            k_v( STOPCYCLE_KEY,				&filter.StopCycle(),			CBratFilter::smStopCycle ),
            k_v( STARTPASS_KEY,				&filter.StartPass(),			CBratFilter::smStartPass ),
            k_v( STOPPASS_KEY,				&filter.StopPass(),				CBratFilter::smStopPass ),

            k_v( RELATIVE_START_DAYS_KEY,	&filter.RelativeStartDays(),	CBratFilter::smRelativeStartDays ),
            k_v( RELATIVE_STOP_DAYS_KEY,	&filter.RelativeStopDays(),		CBratFilter::smRelativeStopDays ),
            k_v( USE_CURRENT_TIME_KEY,		&filter.UseCurrentTime(),		CBratFilter::smUseCurrentTime ),
            k_v( RELATIVE_REFERENCE_TIME,	&relative_ref_time )
        );

        auto &areas = filter.mAreaNames;
        areas.clear();
        for ( auto &name : list )
        {
            areas.push_back( q2a( name ) );
        }

        filter.StartTime() = stime.empty() ? CBratFilter::smStartTime : QDateTime::fromString( stime.c_str(), t2q( date_time_format ) );
        filter.StopTime() = etime.empty() ?  QDateTime::currentDateTime() : QDateTime::fromString( etime.c_str(), t2q( date_time_format ) );
        filter.RelativeReferenceTime() = relative_ref_time.empty() ?
                                                QDateTime::currentDateTime() :
                                                QDateTime::fromString( relative_ref_time.c_str(), t2q( date_time_format ) );

        // Re-calculate Start and Stop times relative to system current time //
        if ( filter.UsingRelativeTimes() && filter.UseCurrentTime() )
        {
            filter.StartTime() = QDateTime::currentDateTime().addDays( filter.RelativeStartDays() );
            filter.StopTime()  = QDateTime::currentDateTime().addDays( filter.RelativeStopDays() );
        }
    }

    return mSettings.status() == QSettings::NoError && Areas().Load() && Regions().Load();
}
