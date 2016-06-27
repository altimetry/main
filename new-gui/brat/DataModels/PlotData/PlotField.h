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

#ifndef DATA_MODELS_PLOT_DATA_PLOTFIELD_H
#define DATA_MODELS_PLOT_DATA_PLOTFIELD_H

#include "libbrathl/brathl.h"
#include "libbrathl/List.h"
using namespace brathl;

class CXYPlotProperties;
class CWorldPlotProperties;
struct CZFXYPlotProperties;
class CPlot;
class CWPlot;
class CZFXYPlot;

class CDisplayData;

namespace brathl {

	class CInternalFiles;
	class CInternalFilesYFX;
}





//-------------------------------------------------------------
//------------------- CPlotField class --------------------
//-------------------------------------------------------------

//  Class to manage field and their associated internal files 

class CPlotField : public CBratObject
{
	using base_t = CBratObject;

public:
	std::string m_name;
	CObArray m_internalFiles;

	CDisplayData *mDisplayData = nullptr;

	CXYPlotProperties *m_xyProps = nullptr;
	CWorldPlotProperties *m_worldProps = nullptr;
	CZFXYPlotProperties *m_zfxyProps = nullptr;

public:
	CPlotField( const std::string& name, CDisplayData *ddata = nullptr )
		: base_t()
		, m_name( name )
		, m_internalFiles( false )
		, mDisplayData( ddata )
		, m_xyProps( nullptr )
		, m_worldProps( nullptr )
		, m_zfxyProps( nullptr )
	{}

	virtual ~CPlotField()
	{}

	CInternalFiles* GetInternalFiles( int32_t index );
	CInternalFilesYFX* GetInternalFilesYFX( int32_t index );
};




// Base class for all CXXXPlotData hierarchy
//
class CPlotData : public CBratObject
{
	using base_t = CBratObject;

protected:
	std::vector<std::string> mFieldNames;

public:
	CPlotData( std::initializer_list< const std::string > names )
		: base_t()
	{
		for ( auto const &name : names )
			mFieldNames.push_back( name );
	}

	virtual ~CPlotData()
	{}


	const std::string& FieldName( size_t index ) const
	{
		assert__( index < mFieldNames.size() );

		return mFieldNames[ index ];
	}
};





#endif			//DATA_MODELS_PLOT_DATA_PLOTFIELD_H
