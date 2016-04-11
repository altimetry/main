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

#if !defined( BRAT_V3)
#include <qgscoordinatereferencesystem.h>
#endif


#include "libbrathl/Tools.h"
#include "new-gui/Common/tools/Exception.h"
using namespace brathl;


#include "MapProjection.h"
#include "DataModels/PlotData/WorldPlotCommonData.h"


///////////////////////////
//	v3 singleton technique
///////////////////////////

class CSmartCleanerMapProj
{
public:
	CSmartCleanerMapProj( CMapProjection * pObject = nullptr ) : m_pObject( pObject ) { }
	virtual ~CSmartCleanerMapProj()
	{
		if ( m_pObject != nullptr )
		{
			delete m_pObject;
		}
	}
	void SetObject( CMapProjection * pObject )
	{
		m_pObject = pObject;
	}
	CMapProjection * GetObject()
	{
		return m_pObject;
	}
private:
	CMapProjection * m_pObject;
};





////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Coordinate Reference Systems
////////////////////////////////////////////////////////////////////////////////////////////////////////////



#if !defined( BRAT_V3)

using crs_entry_t = std::pair< unsigned, QgsCoordinateReferenceSystem >;
using crs_map_t = const std::vector< crs_entry_t >;



crs_entry_t MakeCRSEntryFromId( unsigned id )
{
	QgsCoordinateReferenceSystem invalid;

	switch ( id )
	{
		case PROJ2D_LAMBERT_CYLINDRICAL:
            return { id, { 54003, QgsCoordinateReferenceSystem::EpsgCrsId } };	//World_Miller_Cylindrical
			break;

		case PROJ2D_PLATE_CAREE:
			return { id, { 32662, QgsCoordinateReferenceSystem::EpsgCrsId } };		//WGS 84 / Plate Carree
			break;

		case PROJ2D_MOLLWEIDE:
			return { id, { 54009, QgsCoordinateReferenceSystem::EpsgCrsId } };		//World_Mollweide
			break;

		case PROJ2D_ROBINSON:
			return { id, { 54030, QgsCoordinateReferenceSystem::EpsgCrsId } };		//World_Robinson
			break;

		case PROJ2D_LAMBERT_AZIMUTHAL:
			return { id, { 102017, QgsCoordinateReferenceSystem::EpsgCrsId } };		//North_Pole_Lambert_Azimuthal_Equal_Area
			break;

		case PROJ2D_AZIMUTHAL_EQUIDISTANT:
            return { id, { 54032, QgsCoordinateReferenceSystem::EpsgCrsId } };		//World_Azimuthal_Equidistant
			break;

		case PROJ2D_3D:			//invalid CRS, not 2D
			break;

		case PROJ2D_MERCATOR:
			return { id, { 3785, QgsCoordinateReferenceSystem::EpsgCrsId } };		//Popular .... Mercator
			break;

		case PROJ2D_ORTHO:
			return { id, { 102020, QgsCoordinateReferenceSystem::EpsgCrsId } };		//???? South_Pole_Lambert_Azimuthal_Equal_Area
			break;

		case PROJ2D_NEAR_SIGHTED:
			return { id, { 53031, QgsCoordinateReferenceSystem::EpsgCrsId } };		//??? Sphere_Two_point_Equidistant 
			break;

		case PROJ2D_STEREOGRAPHIC:
			return { id, { 53026, QgsCoordinateReferenceSystem::EpsgCrsId } };		//Sphere_Stereographic, 54026-World_Stereographic
			break;

		case PROJ2D_TMERCATOR:	//not used
			break;

		default:
			assert__( false );
	}

	return crs_entry_t( id, invalid );
}

#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	CMapProjection class
////////////////////////////////////////////////////////////////////////////////////////////////////////////



CMapProjection* CMapProjection::m_instance = nullptr;
CSmartCleanerMapProj	CMapProjection::m_SmartCleaner;

CMapProjection* CMapProjection::GetInstance()
{
	if ( m_instance == nullptr )
		m_instance = new CMapProjection;

	return m_instance;
}



//originally based on vtkProj2DFilter.h from VISAN
//
CMapProjection::CMapProjection()
{
	//projection constants are declared in vtkProj2DFilter.h
	Insert( "Plate Caree", PROJ2D_PLATE_CAREE );
	Insert( "Robinson", PROJ2D_ROBINSON );
	Insert( "Mollweide", PROJ2D_MOLLWEIDE );
	Insert( "Lambert Cylindrical", PROJ2D_LAMBERT_CYLINDRICAL );
	Insert( "Lambert Azimuthal", PROJ2D_LAMBERT_AZIMUTHAL );
	Insert( "Azimuthal Equidistant", PROJ2D_AZIMUTHAL_EQUIDISTANT );
	Insert( "Mercator", PROJ2D_MERCATOR );
	Insert( "Orthographic", PROJ2D_ORTHO );
	Insert( "Near-Sided Perspective", PROJ2D_NEAR_SIGHTED );
	Insert( "Stereographic", PROJ2D_STEREOGRAPHIC );
	//Insert("Transverse Mercator", VTK_PROJ2D_TMERCATOR);  // this projection is weird..
	Insert( "3D", PROJ2D_3D );

	m_SmartCleaner.SetObject( this );
}


CMapProjection::~CMapProjection()
{
	// Step 1 - Set m_instance to nullptr when the object is deleted

	CMapProjection::m_instance = nullptr;

	// Step 2 - Set nullptr pointer to smart cleaner

	m_SmartCleaner.SetObject( nullptr );
}


#if !defined( BRAT_V3)
const QgsCoordinateReferenceSystem& CMapProjection::IdToCRS( unsigned id )
{
	// lambda

	auto validate_projections = []( const crs_map_t &m ) -> bool 
	{
		for ( auto const &crs : m )
			if ( crs.first != PROJ2D_3D && !crs.second.isValid() )
				return false;

		return true;
	};


	// function body

	static const crs_entry_t invalid;
	static const crs_map_t m =
	{
		MakeCRSEntryFromId( PROJ2D_LAMBERT_CYLINDRICAL ),
		MakeCRSEntryFromId( PROJ2D_PLATE_CAREE ),
		MakeCRSEntryFromId( PROJ2D_MOLLWEIDE ),
		MakeCRSEntryFromId( PROJ2D_ROBINSON ),
		MakeCRSEntryFromId( PROJ2D_LAMBERT_AZIMUTHAL ),
		MakeCRSEntryFromId( PROJ2D_AZIMUTHAL_EQUIDISTANT ),
		MakeCRSEntryFromId( PROJ2D_3D ),
		MakeCRSEntryFromId( PROJ2D_MERCATOR ),
		MakeCRSEntryFromId( PROJ2D_ORTHO ),
		MakeCRSEntryFromId( PROJ2D_NEAR_SIGHTED ),
		MakeCRSEntryFromId( PROJ2D_STEREOGRAPHIC ),
		//MakeCRSEntryFromId( PROJ2D_TMERCATOR ),
	};

	static const bool valid_projections = validate_projections( m );		assert__( valid_projections && ( m.size() == CMapProjection::GetInstance()->size() ) );

	for ( auto &crs : m )
		if ( crs.first == id )
			return crs.second;

	return invalid.second;
};
#endif



bool CMapProjection::ValidName( const std::string& name ) const
{
	return ValidName( name.c_str() );
}


bool CMapProjection::ValidName( const char* name ) const
{
	return ( !isDefaultValue( Exists( name ) ) );
}


std::string CMapProjection::NameToLabeledName( const std::string& name ) const
{
	std::string nameTmp = CTools::StringToLower( name );

	for ( CMapProjection::const_iterator it = begin(); it != end(); it++ )
	{
		std::string lowerName = CTools::StringToLower( it->first );
		if ( nameTmp == lowerName )
		{
			return it->first;
		}
	}

	return "";
}


std::string CMapProjection::IdToName( unsigned id ) const
{
	for ( CMapProjection::const_iterator it = begin(); it != end(); it++ )
	{
		if ( it->second == id )
		{
			return it->first;
		}
	}

	return "";
}


unsigned CMapProjection::NameToId( const char* name ) const
{
	return Exists( name );
}


unsigned CMapProjection::NameToId( const std::string& name ) const
{
	return Exists( name );
}


unsigned CMapProjection::Exists( const std::string& key ) const
{
	return  CUIntMap::Exists( key );
}


unsigned CMapProjection::operator[](const std::string& key) const
{
   return Exists(key);
}
