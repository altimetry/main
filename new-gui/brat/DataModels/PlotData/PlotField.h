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
	void Init();
public:
	CPlotField( const std::string& name );
	virtual ~CPlotField();

	static CPlotField* GetPlotField( CBratObject* ob );

	CInternalFiles* GetInternalFiles( int32_t index );
	CInternalFilesYFX* GetInternalFilesYFX( int32_t index );

	std::string m_name;
	CObArray m_internalFiles;

	CXYPlotProperties* m_xyProps;
	CWorldPlotProperties* m_worldProps;
	CZFXYPlotProperties* m_zfxyProps;
};


#endif			//DATA_MODELS_PLOT_DATA_PLOTFIELD_H
