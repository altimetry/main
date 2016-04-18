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
#ifndef DATA_MODELS_PLOT_DATA_MAPPROJECTION_H
#define DATA_MODELS_PLOT_DATA_MAPPROJECTION_H


#include "libbrathl/brathl.h"
#include "libbrathl/List.h"
using namespace brathl;


class CSmartCleanerMapProj;



#if defined (BRAT_V3)
#include "new-gui/brat/DataModels/PlotData/BratLookupTable.h"
#include "display/PlotData/vtkProj2DFilter.h"
// (Pseudo) Cylindrical projections:
// - Lambert Cylindrical Equal Area
#define PROJ2D_LAMBERT_CYLINDRICAL      VTK_PROJ2D_LAMBERT_CYLINDRICAL
// - Plate Caree
#define PROJ2D_PLATE_CAREE              VTK_PROJ2D_PLATE_CAREE
// - Mollweide
#define PROJ2D_MOLLWEIDE                VTK_PROJ2D_MOLLWEIDE
// - Robinson
#define PROJ2D_ROBINSON                 VTK_PROJ2D_ROBINSON

// Azimuthal projections:
// - Lambert Azimuthal
#define PROJ2D_LAMBERT_AZIMUTHAL        VTK_PROJ2D_LAMBERT_AZIMUTHAL
// - Azimuthal Equidistant
#define PROJ2D_AZIMUTHAL_EQUIDISTANT    VTK_PROJ2D_AZIMUTHAL_EQUIDISTANT

// 3D projections
#define PROJ2D_3D                       VTK_PROJ2D_3D

// Mercator projections
#define PROJ2D_MERCATOR                 VTK_PROJ2D_MERCATOR

// Apian Globular
#define PROJ2D_ORTHO                    VTK_PROJ2D_ORTHO

// Near-Sighted Perspective
#define PROJ2D_NEAR_SIGHTED             VTK_PROJ2D_NEAR_SIGHTED

// Gnomonic Projection also called central or azimuthal centrographic
#define PROJ2D_STEREOGRAPHIC            VTK_PROJ2D_STEREOGRAPHIC

// transverse mercator
#define PROJ2D_TMERCATOR				VTK_PROJ2D_TMERCATOR
#else

// (Pseudo) Cylindrical projections:
// - Lambert Cylindrical Equal Area
#define PROJ2D_LAMBERT_CYLINDRICAL      1
// - Plate Caree
#define PROJ2D_PLATE_CAREE              2
// - Mollweide
#define PROJ2D_MOLLWEIDE                3
// - Robinson
#define PROJ2D_ROBINSON                 4

// Azimuthal projections:
// - Lambert Azimuthal
#define PROJ2D_LAMBERT_AZIMUTHAL        5
// - Azimuthal Equidistant
#define PROJ2D_AZIMUTHAL_EQUIDISTANT    6

// 3D projections
#define PROJ2D_3D                       7

// Mercator projections
#define PROJ2D_MERCATOR                 8

// Apian Globular
#define PROJ2D_ORTHO                    9

// Near-Sighted Perspective
#define PROJ2D_NEAR_SIGHTED             10

// Gnomonic Projection also called central or azimuthal centrographic
#define PROJ2D_STEREOGRAPHIC            11

// transverse mercator
#define PROJ2D_TMERCATOR				12

#endif




class CMapProjection : public CUIntMap
{
	/**
	* unique instance of the class
	*/
	static CMapProjection *m_instance;

	static CSmartCleanerMapProj	m_SmartCleaner;

public:
	static CMapProjection* GetInstance();

public:
	// CIntMap ctor

	CMapProjection();

	// CIntMap dtor

	virtual ~CMapProjection();

	bool ValidName( const char* name ) const;
	bool ValidName( const std::string& name )  const;

	std::string IdToName( unsigned id )  const;
#if !defined (BRAT_V3)
	const QgsCoordinateReferenceSystem& IdToCRS( unsigned id );
#endif

	unsigned NameToId( const char* name )  const;
	unsigned NameToId( const std::string& name )  const;

	std::string NameToLabeledName( const std::string& name )  const;

	virtual unsigned Exists( const std::string& key )  const;
	virtual unsigned operator[]( const std::string& key )  const;
};



#endif		//	DATA_MODELS_PLOT_DATA_MAPPROJECTION_H
