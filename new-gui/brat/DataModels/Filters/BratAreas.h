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
#if !defined DATA_MODELS_FILTERS_BRAT_AREAS_H
#define DATA_MODELS_FILTERS_BRAT_AREAS_H


#include "new-gui/Common/ApplicationSettings.h"



class CVertex : public std::pair< double, double >
{
	//types

	using base_t = std::pair< double, double >;


	//construction / destruction

public:
	CVertex( double lon, double lat )
		: base_t( lon, lat )
	{}

	virtual ~CVertex()
	{}


	//access

	double& lon() { return first; }
	double& lat() { return second; }

	double lon() const { return first; }
	double lat() const { return second; }
};





class CArea : public std::vector< CVertex >
{
	//types

	using base_t = std::vector< CVertex >;


	//instance data

	std::string mName;

	bool mPredefined = false;


    //JOFF -
    double mlat_min = std::numeric_limits<double>::max();
    double mlat_max = std::numeric_limits<double>::lowest();
    double mlon_min = std::numeric_limits<double>::max();
    double mlon_max = std::numeric_limits<double>::lowest();

	//construction / destruction

public:

	explicit CArea()		//TODO check compiler issue about deleting this ctor
	{}

	explicit CArea( const std::string &name, bool predefined = false )
		: mName( name ), mPredefined( predefined )
	{}

	//allows declarations like CArea area( "Lake Baikal", { { 24.3938, 57.7512 }, { -9.49747, 36.0065 } } )
	//
	CArea( const std::string &name, std::initializer_list< CVertex > vertices, bool predefined = false )
		: mName( name ), mPredefined( predefined )
	{
		for ( auto &v : vertices )
			AddVertex( v );
	}

	CArea( const CArea &o )
	{
		*this = o;
	}

	CArea& operator = ( const CArea &o )
	{
		if ( this != &o )
		{
			static_cast<base_t&>( *this ) = o;
			mName = o.mName;
			mPredefined = o.mPredefined;
            mlat_min = o.mlat_min;
            mlat_max = o.mlat_max;
            mlon_min = o.mlon_min;
            mlon_max = o.mlon_max;
		}
		return *this;
	}

	virtual ~CArea()
	{}


	//access

	bool operator == ( const CArea &o ) const
	{
		if ( mName != o.mName )
			return false;

		if ( mPredefined != o.mPredefined )
			return false;

		const size_t sz = size();
		if ( sz != o.size() )
			return false;

		for ( size_t i = 0; i < sz; ++i )
			if ( (*this)[i] != o[i] )
				return false;

		return true;
	}

	bool operator != ( const CArea &o ) const
	{
		return !( *this == o );
	}


	std::string& Name() { return mName; }

	const std::string& Name() const { return mName; }

    double GetLatMax() const {return mlat_max; }
    double GetLatMin() const {return mlat_min; }
    double GetLonMax() const {return mlon_max; }
    double GetLonMin() const {return mlon_min; }

	bool IsPredefined() const { return mPredefined; }



	//assignment

	void AddVertex( double lon, double lat )
	{
		push_back( CVertex( lon, lat ) );
        if (lon < mlon_min)
        {
            mlon_min = lon;
        }
        if (lon > mlon_max)
        {
            mlon_max = lon;
        }

        if (lat < mlat_min)
        {
            mlat_min = lat;
        }
        if (lat > mlat_max)
        {
            mlat_max = lat;
        }
	}


	void AddVertex( const CVertex &v )
	{
        push_back( v );
        double lon = v.lon();
        double lat = v.lat();

        if (lon < mlon_min)
        {
            mlon_min = lon;
        }
        if (lon > mlon_max)
        {
            mlon_max = lon;
        }

        if (lat < mlat_min)
        {
            mlat_min = lat;
        }
        if (lat > mlat_max)
        {
            mlat_max = lat;
        }
	}

	void RemoveVertex( size_t index )
	{
		assert__( index < size() );

		erase( begin() + index  );

        //recalc min/max

        double lon =0;
        double lat =0;
        mlat_min = std::numeric_limits<double>::max();
        mlat_max = std::numeric_limits<double>::lowest();
        mlon_min = std::numeric_limits<double>::max();
        mlon_max = std::numeric_limits<double>::lowest();
        size_t sz = size();

        for(unsigned int i=0;i<sz;i++)
        {
            lon = at(i).lon();
            lat = at(i).lat();
            if (lon < mlon_min)
            {
                mlon_min = lon;
            }
            if (lon > mlon_max)
            {
                mlon_max = lon;
            }

            if (lat < mlat_min)
            {
                mlat_min = lat;
            }
            if (lat > mlat_max)
            {
                mlat_max = lat;
            }
        }
        // actualization complete.
	}


	// persistence

	QStringList ToQStringList() const;

	bool FromQStringList( const QStringList &list );
};





//map str->Area;
class CBratAreas : public CFileSettings
{
	//types

	using base_t = CFileSettings;


	//instance data

	std::map< std::string, CArea > mAreasMap;

public:

	CBratAreas( const std::string &full_path ) 
		: base_t( full_path )
	{}

	virtual ~CBratAreas()
	{}


	bool operator == ( const CBratAreas &o ) const
	{
		const size_t size = mAreasMap.size();
		if ( size != o.mAreasMap.size() )
			return false;

		for ( auto &area_entry : mAreasMap )
		{
			if ( area_entry.second != o.mAreasMap.at( area_entry.first ) )
				return false;
		}

		return true;
	}

	bool operator != ( const CBratAreas &o ) const
	{
		return !( *this == o );
	}


	// access

    const std::map< std::string, CArea >& AreasMap() const { return mAreasMap; }

	CArea* Find( const std::string &name );

	const CArea* Find( const std::string &name ) const
	{
		return const_cast< CBratAreas* >( this )->Find( name );
	}

	std::string MakeNewName() const;


	// operations

	bool AddArea( const std::string &name );

	bool AddArea( const CArea &area );

	bool RenameArea( const std::string &name, const std::string &new_name );

	bool DeleteArea( const std::string &name );


	// persistence

	bool Load();

	bool Save();
};






class CRegion : public std::vector< std::string >
{
	//types

	using base_t = std::vector< std::string >;


	//instance data

	std::string mName;

public:
	explicit CRegion()		//TODO check compiler issue about deleting this ctor
	{}

	explicit CRegion( const std::string &name )
		: mName( name )
	{}

	CRegion( const std::string &name, const std::vector< std::string > &area_names )
		: base_t( area_names.begin(), area_names.end() )
		, mName( name )
	{}

	virtual ~CRegion()
	{}


	std::string& Name() { return mName; }

	const std::string& Name() const { return mName; }
};





class CBratRegions : public CFileSettings
{
	//types

	using base_t = CFileSettings;

	//instance data

	std::map< std::string, CRegion > mRegionsMap;

public:
	CBratRegions( const std::string &full_path )
		: base_t( full_path )
	{}

	virtual ~CBratRegions()
	{}


	// access

    const std::map< std::string, CRegion >& RegionsMap() const { return mRegionsMap; }

	CRegion* Find( const std::string &name );

	const CRegion* Find( const std::string &name ) const
	{
		return const_cast< CBratRegions* >( this )->Find( name );
	}

	std::string MakeNewName() const;


	// operations

	bool AddRegion( const std::string &name, const std::vector< std::string > &area_names );

	bool AddRegion( const CRegion &region );

	bool RenameRegion( const std::string &name, const std::string &new_name );

	bool DeleteRegion( const std::string &name );


	// persistence

	bool Load();

	bool Save();
};





#endif	//DATA_MODELS_FILTERS_BRAT_AREAS_H
