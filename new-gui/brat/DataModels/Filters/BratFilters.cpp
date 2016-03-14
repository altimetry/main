#include "new-gui/brat/stdafx.h"

#include "libbrathl/Product.h"

using namespace brathl;

#include "ApplicationLogger.h"

#include "BratFilters.h"



static const std::string NAME_KEY = "Name";

static const std::string MINLON_KEY = "MinLon";
static const std::string MAXLON_KEY = "MaxLon";
static const std::string MINLAT_KEY = "MinLat";
static const std::string MAXLAT_KEY = "MaxLat";

static const std::string STARTDATE_KEY = "StartDate";
static const std::string STOPDATE_KEY = "StopDate";

static const std::string STARTCYCLE_KEY = "StartCycle";
static const std::string STOPCYCLE_KEY = "StopCycle";
static const std::string STARTPASS_KEY = "StartPass";
static const std::string STOPPASS_KEY = "StopPass";


static const std::string date_time_format = "yyyy/MM/dd T hh:mm:ss";

static const std::string FILTER_BASE_NAME = "Filter";

static const std::string DATASET_SELECTION_LOG_FILENAME = "DatasetSelection.log";



//static 
std::vector< CRegion > CBratFilter::mRegions;




CBratFilter& CBratFilter::operator = ( const CBratFilter &o )
{
	if ( this != &o )
	{
		mName = o.mName;

		mMaxLon = o.mMaxLon;
		mMaxLon = o.mMaxLon;
		mMinLat = o.mMinLat;
		mMaxLat = o.mMaxLat;

		mStartTime = o.mStartTime;
		mStopTime = o.mStopTime;

		mStartCycle = o.mStartCycle;
		mStopCycle = o.mStopCycle;
		mStartPass = o.mStartPass;
		mStopPass = o.mStopPass;
	}
	return *this;
}


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
	for ( auto it = mFiltersMap.cbegin(); it != mFiltersMap.cend(); it++ )
	{
		WriteSection( it->first,

			k_v( MINLON_KEY,		it->second.MinLon() ),
			k_v( MAXLON_KEY,		it->second.MaxLon() ),
			k_v( MINLAT_KEY,		it->second.MinLat() ),
			k_v( MAXLAT_KEY,		it->second.MaxLat() ),

			k_v( STARTDATE_KEY,		it->second.StartTime().toString( t2q( date_time_format ) ) ),
			k_v( STOPDATE_KEY,		it->second.StopTime().toString( t2q( date_time_format ) ) ),

			k_v( STARTCYCLE_KEY,	it->second.StartCycle() ),
			k_v( STOPCYCLE_KEY,		it->second.StopCycle() ),
			k_v( STARTPASS_KEY,		it->second.StartPass() ),
			k_v( STOPPASS_KEY,		it->second.StopPass() )
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
		std::string stime;
		std::string etime;
		ReadSection( it->first,

			k_v( MINLON_KEY,		&it->second.MinLon() ),
			k_v( MAXLON_KEY,		&it->second.MaxLon() ),
			k_v( MINLAT_KEY,		&it->second.MinLat() ),
			k_v( MAXLAT_KEY,		&it->second.MaxLat() ),

			k_v( STARTDATE_KEY,		&stime ),
			k_v( STOPDATE_KEY,		&etime ),

			k_v( STARTCYCLE_KEY,	&it->second.StartCycle() ),
			k_v( STOPCYCLE_KEY,		&it->second.StopCycle() ),
			k_v( STARTPASS_KEY,		&it->second.StartPass() ),
			k_v( STOPPASS_KEY,		&it->second.StopPass() )
		);

		it->second.StartTime() = QDateTime::fromString( stime.c_str(), t2q( date_time_format ) );
		it->second.StopTime() = QDateTime::fromString( etime.c_str(), t2q( date_time_format ) );
	}

	return mSettings.status() == QSettings::NoError;
}


bool CBratFilters::Apply( const std::string &name, const CStringList& files_in, CStringList& files_out )
{
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

		CProduct* product_ref = dynamic_cast< CProduct* >( CMapProduct::GetInstance().Exists( p->GetLabel() ) );
		if ( product_ref == nullptr )
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
