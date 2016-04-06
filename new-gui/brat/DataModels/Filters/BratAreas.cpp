#include "new-gui/brat/stdafx.h"

#include "ApplicationLogger.h"

#include "BratAreas.h"



static const std::string VERTICES_KEY = "Vertices";
static const std::string REGIONS_KEY = "Regions";

static const std::string AREA_BASE_NAME = "UserArea";
static const std::string REGION_BASE_NAME = "UserRegion";





/////////////////////////////////////////////////////////////////////
//							CArea
/////////////////////////////////////////////////////////////////////


QStringList CArea::ToQStringList() const
{
	QStringList list;
	for ( auto const &vertex : *this )
	{
		list << n2q( vertex.lon() ) << n2q( vertex.lat() );
	}

	return list;
}


bool CArea::FromQStringList( const QStringList &list )
{
	const size_t size = list.size();
	if ( size == 0 || size % 2 != 0 )
		return false;

	clear();
	for ( size_t i = 0; i < size; )
	{
		AddVertex( list[ i ].toDouble(), list[ i + 1 ].toDouble() );
		i += 2;
	}

	return true;
}




/////////////////////////////////////////////////////////////////////
//							CBratAreas
/////////////////////////////////////////////////////////////////////



CArea* CBratAreas::Find( const std::string &name )
{
	auto it = mAreasMap.find( name );
	if ( it == mAreasMap.end() )
		return nullptr;

	return &it->second;
}


std::string CBratAreas::MakeNewName() const
{
	size_t i = mAreasMap.size();
	std::string key;

	do
	{
		key = AREA_BASE_NAME + "_" + n2s<std::string>( i++ );
	} 
	while ( Find( key ) != nullptr );

	return key;
}


bool CBratAreas::AddArea( const std::string &name )
{
	if ( Find( name ) )
		return false;

	mAreasMap.insert( { name, CArea( name ) }  );

	return true;
}


bool CBratAreas::AddArea( const CArea &area )
{
	if ( !AddArea( area.Name() ) )
		return false;

	mAreasMap[ area.Name() ] = area;

	return true;
}


bool CBratAreas::RenameArea( const std::string &name, const std::string &new_name )
{
	if ( !Find( name ) || Find( new_name ) )
		return false;

	CArea new_area = mAreasMap.at( name );
	if ( !DeleteArea( name ) )
    {
        LOG_TRACE( "Unexpected failure deleting area" );
		assert__( false );
    }

	new_area.Name() = new_name;

	mAreasMap.insert( { new_name, new_area } );

	return true;
}


bool CBratAreas::DeleteArea( const std::string &name )
{
	if ( !Find( name ) )
		return false;

	mAreasMap.erase( name );

	return true;
}


bool CBratAreas::Load()
{
	mAreasMap.clear();
	QStringList groups = mSettings.childGroups();
	for ( auto const &group : groups )
	{
		auto name = q2a( group );
		mAreasMap.insert( { name, CArea( name ) }  );
	}

	if ( Status() != QSettings::NoError )
		return false;

	for ( auto it = mAreasMap.begin(); it != mAreasMap.end(); it++ )
	{
		QStringList list;
		ReadSection( it->first,

			k_v( VERTICES_KEY,		&list )
		);

		if ( !it->second.FromQStringList( list ) )
			return false;
	}

	return mSettings.status() == QSettings::NoError;
}


bool CBratAreas::Save()
{
	Clear();

	for ( auto it = mAreasMap.cbegin(); it != mAreasMap.cend(); it++ )
	{
		WriteSection( it->first,

			k_v( VERTICES_KEY,		it->second.ToQStringList() )
		);
	}

	return mSettings.status() == QSettings::NoError;
}








/////////////////////////////////////////////////////////////////////
//						CBratRegions
/////////////////////////////////////////////////////////////////////



CRegion* CBratRegions::Find( const std::string &name )
{
	auto it = mRegionsMap.find( name );
	if ( it == mRegionsMap.end() )
		return nullptr;

	return &it->second;
}


std::string CBratRegions::MakeNewName() const
{
	size_t i = mRegionsMap.size();
	std::string key;

	do
	{
		key = REGION_BASE_NAME + "_" + n2s<std::string>( i++ );
	} 
	while ( Find( key ) != nullptr );

	return key;
}


// operations

bool CBratRegions::AddRegion( const std::string &name, const std::vector< std::string > &area_names )
{
	if ( Find( name ) )
		return false;

	mRegionsMap.insert( { name, CRegion( name, area_names ) }  );

	return true;
}


bool CBratRegions::AddRegion( const CRegion &region )
{
	return AddRegion( region.Name(), { region.begin(), region.end() } );
}


bool CBratRegions::RenameRegion( const std::string &name, const std::string &new_name )
{
	if ( !Find( name ) || Find( new_name ) )
		return false;

	CRegion new_region = mRegionsMap.at( name );
	if ( !DeleteRegion( name ) )
    {
        LOG_TRACE( "Unexpected failure deleting region" );
		assert__( false );
    }

	new_region.Name() = new_name;

	mRegionsMap.insert( { new_name, new_region } );

	return true;
}


bool CBratRegions::DeleteRegion( const std::string &name )
{
	if ( !Find( name ) )
		return false;

	mRegionsMap.erase( name );

	return true;
}



bool CBratRegions::Load()
{
	mRegionsMap.clear();
	QStringList groups = mSettings.childGroups();
	for ( auto const &group : groups )
	{
		auto name = q2a( group );
		mRegionsMap.insert( { name, CRegion( name ) }  );
	}

	if ( Status() != QSettings::NoError )
		return false;

	for ( auto it = mRegionsMap.begin(); it != mRegionsMap.end(); it++ )
	{
		QStringList list;
		ReadSection( it->first,

			k_v( REGIONS_KEY,		&list )
		);

		for ( auto &name : list )
			it->second.push_back( q2a( name ) );
	}

	return mSettings.status() == QSettings::NoError;
}


bool CBratRegions::Save()
{
	Clear();

	for ( auto it = mRegionsMap.cbegin(); it != mRegionsMap.cend(); it++ )
	{
		QStringList list;
		for ( auto &name : it->second )
			list << t2q( name );

		WriteSection( it->first,

			k_v( REGIONS_KEY,		list )
		);
	}

	return mSettings.status() == QSettings::NoError;
}
