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
#ifndef DATA_MODELS_PLOT_DATA_ZFXYPLOTDATA_H
#define DATA_MODELS_PLOT_DATA_ZFXYPLOTDATA_H

#include "new-gui/Common/tools/Exception.h"

#include "libbrathl/brathl.h"

#include "libbrathl/BratObject.h"
#include "libbrathl/Tools.h"
#include "libbrathl/List.h"
#include "libbrathl/Date.h"
#include "libbrathl/Unit.h"

#include "libbrathl/InternalFiles.h"
#include "libbrathl/InternalFilesZFXY.h"

using namespace brathl;

#include "PlotValues.h"
#include "WidgetField.h"
#include "PlotColor.h"

#include "new-gui/brat/DataModels/PlotData/BratLookupTable.h"


class CZFXYPlot;



class CZFXYPlotField : public CZFXYValues, public CWidgetField
{
	////////////////////////////
	//			types
	////////////////////////////

	using base_t = CWidgetField;
	using values_base_t = CZFXYValues;

public:
	using values_base_t::GetXRange;
	using values_base_t::GetYRange;
	using values_base_t::CurrentFrame;

protected:
	using values_base_t::back;


	////////////////////////////
	//		instance data
	////////////////////////////


#if defined (BRAT_V3)
	bool m_aborted;
	CStringArray m_dataTitles;
public:
#endif

	////////////////////////////
	// construction /destruction
	////////////////////////////

	void Create( CZFXYPlot* plot );
public:
	CZFXYPlotField( CZFXYPlot* plot, CZFXYPlotProperties* field );

	virtual ~CZFXYPlotField()
	{}

	
	////////////////////////////
	//		access
	////////////////////////////

	virtual CZFXYPlotProperties* GetPlotProperties() override { return dynamic_cast< CZFXYPlotProperties* >( base_t::GetPlotProperties() ); }

	virtual size_t CurrentFrame() const override { return values_base_t::CurrentFrame(); }
};





#endif		//DATA_MODELS_PLOT_DATA_ZFXYPLOTDATA_H

