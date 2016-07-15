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
#ifndef DATAMODELS_PLOTDATA_WORLDPLOT_DATA_H
#define DATAMODELS_PLOTDATA_WORLDPLOT_DATA_H


#include "libbrathl/brathl.h"

#include "libbrathl/InternalFilesZFXY.h"

using namespace brathl;

#include "PlotValues.h"
#include "MapProjection.h"
#include "PlotData.h"




class CWorldPlotData : public CMapValues, public CPlotData
{
	////////////////////////////
	//			types
	////////////////////////////

	using base_t = CPlotData;
	using values_base_t = CMapValues;

protected:
	using values_base_t::back;


	////////////////////////////
	//			statics
	////////////////////////////

protected:

	static void GetMapLatLon( CInternalFilesZFXY* zfxy,	int32_t& width, int32_t& height, CExpressionValue& varLat, CExpressionValue& varLon );


	////////////////////////////
	//		instance data
	////////////////////////////

	int32_t m_projection;

	//bool m_finished;


	///////////////////////////////
	// construction / destruction
	///////////////////////////////


	CWorldPlotData( CWorldPlotProperties *north_field, CWorldPlotProperties *east_field )
		: base_t( north_field, east_field )
		, values_base_t()
		, m_projection( PROJ2D_3D )
	{}

public:
	CWorldPlotData( CWorldPlotProperties *field )
		: base_t( { field } )
		, values_base_t()
		, m_projection( PROJ2D_3D )
	{
		Create();
	}

	virtual ~CWorldPlotData()
	{}

protected:
	virtual void Create();

public:

	///////////////////////////////
	//			access
	///////////////////////////////

	virtual CWorldPlotProperties* GetPlotProperties() override { return dynamic_cast< CWorldPlotProperties* >( base_t::GetPlotProperties() ); }

	virtual size_t CurrentFrame() const override { return values_base_t::CurrentFrame(); }


	virtual void SetProjection( int32_t proj )
	{
		m_projection = proj;
	}

#if defined (BRAT_V3)
	bool m_aborted = false;
	CStringArray mDataTitles;			//filled, not used in v4

protected:
	// v3 note: for base classes
	//
    CWorldPlotData( CWorldPlotProperties &plotProperty )
		: base_t( {} )
		, values_base_t()
		, m_projection( PROJ2D_3D )
	{}
#endif

};




//-------------------------------------------------------------
//------------------- CWorldPlotVelocityData class --------------------
//-------------------------------------------------------------

class CWorldPlotVelocityData : public CWorldPlotData
{
	using base_t = CWorldPlotData;

protected:

	double m_minhvN;
	double m_maxhvN;

	double m_minhvE;
	double m_maxhvE;

   //only for vtk derived classes (globe, etc)
   //
#if defined(BRAT_V3)
   CWorldPlotVelocityData( CWorldPlotProperties &plotProperty )
	   :base_t( plotProperty )
   {}

	CWorldPlotVelocityData( CWorldPlotProperties* field )
		:base_t( field )
	{}
#endif

	void Create( const std::vector< CInternalFiles* > &north_files, const std::vector< CInternalFiles* > &east_files );
public:

	// The order of fields matters
	//
	CWorldPlotVelocityData( CWorldPlotProperties* northField, CWorldPlotProperties* eastField )
		: base_t( northField, eastField )										 //v3 note: this is just to compile
	{
		Create( northField->InternalFiles(), eastField->InternalFiles() );
	}

	virtual ~CWorldPlotVelocityData()
	{}
};


#endif		//DATAMODELS_PLOTDATA_WORLDPLOT_DATA_H
