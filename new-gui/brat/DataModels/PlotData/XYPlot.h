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

#ifndef DATA_MODELS_PLOT_DATA_XYPLOT_H
#define DATA_MODELS_PLOT_DATA_XYPLOT_H

#include "libbrathl/brathl.h"
#include "libbrathl/List.h"
using namespace brathl;

#include "PlotField.h"
#include "PlotBase.h"

//-------------------------------------------------------------
//------------------- CPlot class --------------------
//-------------------------------------------------------------

class CPlot : public CPlotBase
{
public:
	CPlot( uint32_t groupNumber = 0 );
	virtual ~CPlot();


	virtual void GetInfo() override;

	void GetAxisX( CInternalFiles* yfx,
		ExpressionValueDimensions* dimVal,
		CExpressionValue* varX,
		std::string* varXName );

	virtual CInternalFiles* GetInternalFiles( CBratObject* ob, bool withExcept = true ) override;
	static CInternalFilesYFX* GetInternalFilesYFX( CBratObject* ob );

public:
	virtual const std::string& BaseTitle() const override
	{
		static const std::string s = "BRAT Y=F(X) Plot";
		return s;
	}
};

#endif		//DATA_MODELS_PLOT_DATA_XYPLOT_H
