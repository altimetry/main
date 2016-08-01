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
#ifndef DATA_MODELS_PLOT_DATA_XYPLOTDATA_H
#define DATA_MODELS_PLOT_DATA_XYPLOTDATA_H

#include <cmath>

#include "new-gui/Common/tools/Exception.h"
#include "new-gui/Common/+Utils.h"

#include "libbrathl/brathl.h"
#include "libbrathl/BratObject.h"
#include "libbrathl/Tools.h"
#include "libbrathl/List.h"
#include "libbrathl/Date.h"

#include "libbrathl/InternalFiles.h"
#include "libbrathl/InternalFilesZFXY.h"
#include "libbrathl/InternalFilesYFX.h"
using namespace brathl;

#include "WidgetField.h"
#include "PlotValues.h"


class CPlotColor;
class CYFXPlot;


//-------------------------------------------------------------
//------------------- CXYPlotField class --------------------
//-------------------------------------------------------------

class CXYPlotField : public CYFXValues, public CWidgetField
{
	using base_t = CWidgetField;
	using values_base_t = CYFXValues;

protected:

	CExpressionValue m_varComplement;
	CFieldNetCdf m_fieldComplement;

	CObArray m_otherVars;
	CObArray m_otherFields;


#if defined (BRAT_V3)
	bool m_aborted;
#endif

public:
	CXYPlotField( CYFXPlot *plot, size_t iField );

	virtual ~CXYPlotField();

protected:
	void Create( CYFXPlot *plot );

	virtual void SetData( double* xArray, const CUIntArray& xDims,
		double* yArray, const CUIntArray& yDims,
		const CUIntArray& xCommonDimIndex,
		const CUIntArray& yCommonDimIndex );

public:

	virtual CXYPlotProperties* GetPlotProperties() override { return dynamic_cast< CXYPlotProperties* >( base_t::GetPlotProperties() ); }

	virtual size_t CurrentFrame() const override { return values_base_t::CurrentFrame(); }

	void SetXLog( bool value );
	void SetYLog( bool value );

    const CExpressionValue& GetVarComplement() const { return m_varComplement; }
    CFieldNetCdf& GetFieldComplement() { return m_fieldComplement; }

    const CObArray& GetOtherVars() const { return m_otherVars; }
    const CObArray& GetOtherFields() const { return m_otherFields; }
};


#endif			//DATA_MODELS_PLOT_DATA_XYPLOTDATA_H
