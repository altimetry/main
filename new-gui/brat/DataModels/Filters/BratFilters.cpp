#include "new-gui/brat/stdafx.h"

#include "new-gui/Common/QtUtils.h"

#include "libbrathl/Product.h"
#include "libbrathl/Mission.h"

using namespace brathl;

#include "BratLogger.h"

#include "BratFilters.h"



static const std::string NAME_KEY = "Name";

static const std::string FILTER_AREAS_KEY = "FilterAreas";

static const std::string STARTDATE_KEY = "StartDate";
static const std::string STOPDATE_KEY = "StopDate";

static const std::string STARTCYCLE_KEY = "StartCycle";
static const std::string STOPCYCLE_KEY = "StopCycle";
static const std::string STARTPASS_KEY = "StartPass";
static const std::string STOPPASS_KEY = "StopPass";


static const std::string date_time_format = "yyyy/MM/dd T hh:mm:ss";

static const std::string FILTER_BASE_NAME = "Filter";

static const std::string DATASET_SELECTION_LOG_FILENAME = "DatasetSelection.log";



//////////////////////////////////////////////////////////////
//						Single Filter
//////////////////////////////////////////////////////////////


CBratFilter& CBratFilter::operator = ( const CBratFilter &o )
{
    if ( this != &o )
    {
        mName = o.mName;

        mAreaNames = o.mAreaNames;

        mStartTime = o.mStartTime;
        mStopTime = o.mStopTime;

        mStartCycle = o.mStartCycle;
        mStopCycle = o.mStopCycle;
        mStartPass = o.mStartPass;
        mStopPass = o.mStopPass;
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


// Here in our algo we simply take advantage of the previously calculated min, max on lon, lat.
// (FAST LDR)
//
void CBratFilter::BoundingArea( double &lon1, double &lat1, double &lon2, double &lat2 ) const
{
    auto const &areas = Areas(); //returns a CBratAreas

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
}


bool CBratFilter::GetTimeBounds( CDate &Start, CDate &Stop, const std::string &product_label ) const
{
    if ( !isDefaultValue( StartCycle() ) && !isDefaultValue( StopCycle() ) &&
         !isDefaultValue( StartPass() )  && !isDefaultValue( StopPass() )     )
    {
        // 1- Uses Start/Stop Cycle/Pass defined by user
        CMission m( product_label );

        if ( m.CtrlMission() == BRATHL_SUCCESS )
        {
            m.Convert( StartCycle(), StartPass(), Start);
            m.Convert( StopCycle(), StopPass(), Stop);
            return true;
        }
        else // Mission not found       TODO add string parameter for error messages
        {
            LOG_WARN( "Filter '" + Name() + "' was not applied!\nUnable to convert Cycle and Pass to date: no mission found for product '"
                            + product_label + "'." );
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


//////// RCCC TODO /////////////////////////////////////
std::string CBratFilter::GetSelectionCriteriaExpression( const std::string product_label ) const
{
    std::string expression;

    // LAT/LON filtering expression //
    if ( !mAreaNames.empty() )
    {
        double lon1, lat1, lon2, lat2;
        BoundingArea( lon1, lat1, lon2, lat2 );

        lon1 = CLatLonPoint::LonNormal360( lon1 );  // NOTE: We assume that all products have longitude fields in degrees east units,
        lon2 = CLatLonPoint::LonNormal360( lon2 );  // i.e, in the [0, 360] range. (Examples: Cryosat, Jason1, Jason2, GRID_DOTS_MERCATOR... )

        expression += func_is_bounded( lat1, lat_alias(), lat2 );
        expression += " && ";
        expression += func_is_bounded( lon1, lon_alias(), lon2 );
    }
    // TIME filtering expression //
    CDate Start, Stop;
    if ( GetTimeBounds( Start, Stop, product_label ) )
    {
        double start_seconds, stop_seconds;
        Start.Convert2Second( start_seconds ); // NOTE: Although some products have time fields in seconds since 2000-01-01,
        Stop.Convert2Second( stop_seconds );   // in selection criteria expression the user shall insert time values in seconds since 1950-01-01.

        expression.empty()  ?  expression += ""  :  expression += " && ";
        expression += func_is_bounded( start_seconds, time_alias(), stop_seconds );
    }

    if (expression.empty()) { LOG_INFO( "Filter '" + mName + "' applied no selection criteria expression." ); }
    else                    { LOG_INFO( "Filter '" + mName + "' translated to a selection criteria expression:\n" + expression );}

    return expression;
}
//////////////////////////////////////////////////


bool CBratFilter::Apply( const CStringList& files_in, CStringList& files_out ) const
{
	return CBratFilters::GetInstance().Apply( mName, files_in, files_out );
}


void CBratFilter::setDefaultValues()
{
    // Setting Brathl internal reference date year (1950)
    mStartTime = QDateTime( QDate(1950, 1, 1), QTime(0, 0, 0) );
    mStopTime  = QDateTime::currentDateTime();

    setDefaultValue( mStartCycle );
    setDefaultValue( mStopCycle );
    setDefaultValue( mStartPass );
    setDefaultValue( mStopPass );
}


void CBratFilter::setDefaultDateValues()
{
    // Setting Brathl internal reference date year (1950)
    mStartTime = QDateTime( QDate(1950, 1, 1), QTime(0, 0, 0) );
    mStopTime  = QDateTime::currentDateTime();
}

void CBratFilter::setDefaultCyclePassValues()
{
    setDefaultValue( mStartCycle );
    setDefaultValue( mStopCycle );
    setDefaultValue( mStartPass );
    setDefaultValue( mStopPass );
}



//////////////////////////////////////////////////////////////
//						All Filters
//////////////////////////////////////////////////////////////


CBratFilters *CBratFilters::smInstance = nullptr;



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

            k_v( STARTDATE_KEY,		filter.StartTime().toString( t2q( date_time_format ) ) ),
            k_v( STOPDATE_KEY,		filter.StopTime().toString( t2q( date_time_format ) ) ),

            k_v( STARTCYCLE_KEY,	filter.StartCycle() ),
            k_v( STOPCYCLE_KEY,		filter.StopCycle() ),
            k_v( STARTPASS_KEY,		filter.StartPass() ),
            k_v( STOPPASS_KEY,		filter.StopPass() )
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
        std::string stime;
        std::string etime;
        auto &filter = it->second;

        ReadSection( it->first,

            k_v( FILTER_AREAS_KEY,	&list ),

            k_v( STARTDATE_KEY,		&stime ),
            k_v( STOPDATE_KEY,		&etime ),

            k_v( STARTCYCLE_KEY,	&filter.StartCycle() ),
            k_v( STOPCYCLE_KEY,		&filter.StopCycle() ),
            k_v( STARTPASS_KEY,		&filter.StartPass() ),
            k_v( STOPPASS_KEY,		&filter.StopPass() )
        );

        auto &areas = filter.mAreaNames;
        areas.clear();
        for ( auto &name : list )
        {
            areas.push_back( q2a( name ) );
        }

        filter.StartTime() = QDateTime::fromString( stime.c_str(), t2q( date_time_format ) );
        filter.StopTime() = QDateTime::fromString( etime.c_str(), t2q( date_time_format ) );
    }

    return mSettings.status() == QSettings::NoError && Areas().Load() && Regions().Load();
}


bool CBratFilters::Translate2SelectionCriteria( CProduct *product_ref, const std::string &name ) const
{
	auto const *filter = Find( name );

	assert__( filter && product_ref );

	if ( !product_ref->HasCriteriaInfo() )
		return false;

    ///////////////////////
    ///  LatLon Criteria //
    ///////////////////////
    if ( product_ref->HasLatLonCriteria() && !filter->AreaNames().empty() )
	{
		double lon1, lat1, lon2, lat2;
		filter->BoundingArea( lon1, lat1, lon2, lat2 );
        product_ref->GetLatLonCriteria()->Set( lat1, lon1, lat2, lon2 );  // double latLow, double lonLow, double latHigh, double lonHigh

        LOG_INFO("Filter '" + filter->Name() + "' applied a LatLonCriteria, Lat=[" + std::to_string(lat1) + "; " + std::to_string(lat2) +
                                                                     "] and Lon=[" + std::to_string(lon1) + "; " + std::to_string(lon2) + "].");
	}

    ////////////////////////
    /// DateTime Criteria //
    ////////////////////////
    if ( product_ref->HasDatetimeCriteria() )
	{
        CDate Start, Stop;

        if ( filter->GetTimeBounds( Start, Stop, product_ref->GetLabel() ) )
        {
            // Start and Stop.Value() contains the date in a number of seconds since internal reference date, ie 1950.
            product_ref->GetDatetimeCriteria()->Set( Start.Value(), Stop.Value() );
            LOG_INFO("Filter '" + filter->Name() + "' applied a DateTimeCriteria from: " + Start.AsString() + " to: " + Stop.AsString()
                     + ".\n(product type/label: '" + product_ref->GetLabel() + "')");
        }
	}

    /// NOT USED ON BRAT V.4 /// Cycle/Pass criteria are applied as datetime criteria, therefore the old criteria are not used.
    //	Next 2 criteria assignments (cycle and pass) assume that cycle and pass can be used like simple integers as they come from the GUI
//    if ( product_ref->HasCycleCriteria() )
//    {
//        product_ref->GetCycleCriteria()->Set( filter->StartCycle(), filter->StopCycle() );
//    }
//    if ( product_ref->HasPassIntCriteria() )
//    {
//        product_ref->GetPassIntCriteria()->Set( filter->StartPass(), filter->StopPass() );
//    }
	//	Apparently Cryosat uses this criterion; is it simply composed by integer passes converted to string??? I wonder...
	//	If not, we still don't have GUI support for this
//	if ( product_ref->HasPassStringCriteria() )
//	{
//		//product_ref->GetPassStringCriteria()->Set( comma separated strings );
//	}

	return true;
}


bool CBratFilters::Apply( const std::string &name, const CStringList& files_in, CStringList& files_out ) const
{
#if defined(BRAT_V3)
	return true;
#endif

    auto *filter = Find( name );
    if ( !filter )
        return false;

    CProduct* p = nullptr;
    bool result = true;

    try
    {
        p = CProduct::Construct( *files_in.begin() );
        if ( !p )
            return false;

        p->GetProductList().clear();
        p->GetProductList().Insert( files_in );

		CMapProduct mapProduct;
		mapProduct.AddCriteriaToProducts();

        CProduct *product_ref = dynamic_cast< CProduct* >( mapProduct.Exists( p->GetLabel() ) );	//CMapProduct::GetInstance()
        if ( /*product_ref == nullptr ||*/ !Translate2SelectionCriteria( product_ref, name ) )
        {
            result = false;
        }
        else
        {
			p->AddCriteria( product_ref );

			std::string log_path = mInternalDataPath + "/" + DATASET_SELECTION_LOG_FILENAME;

			p->ApplyCriteria( files_out, log_path );
        }
    }
    catch ( CException e )
    {
        result = false;
    }
    catch ( ... )
    {
        result = false;
    }

    if ( !result )
    {
        files_out.Insert( files_in );
    }

    delete p;

    return result;
}


