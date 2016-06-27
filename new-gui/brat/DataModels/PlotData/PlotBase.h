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
#ifndef DATA_MODELS_PLOT_DATA_PLOTBASE_H
#define DATA_MODELS_PLOT_DATA_PLOTBASE_H


#include "libbrathl/brathl.h"
#include "libbrathl/Expression.h"
using namespace brathl;


namespace brathl {

	class CInternalFiles;
}

class CPlotField;


//-------------------------------------------------------------
//------------------- CPlotBase class --------------------
//-------------------------------------------------------------
/** 
  A plot object management base class

 \version 1.0
*/
class CPlotBase : public CBratObject
{
public:
	uint32_t m_groupNumber;

	std::string m_title;

	std::vector<CPlotField*> mFields;
	CStringArray m_nonPlotFieldNames;

	std::string m_titleX;
	std::string m_titleY;

	CUnit m_unitX;
	CUnit m_unitY;

	bool m_unitXConv;
	bool m_unitYConv;


	std::string m_forcedVarXName;
	std::string m_forcedVarYName;

	std::string m_unitXLabel;
	std::string m_unitYLabel;

public:
	CPlotBase( uint32_t groupNumber = 0 )

		: m_groupNumber( groupNumber )
		, m_unitXConv( false )
		, m_unitYConv( false )
	{}

	virtual ~CPlotBase()
	{}

	virtual CInternalFiles* GetInternalFiles( CBratObject* ob, bool withExcept = true ) = 0;
	virtual void GetInfo() = 0;
	virtual const std::string& BaseTitle() const = 0;

	virtual std::string MakeTitle()
	{
		std::string titleTmp = m_title;
		titleTmp = CTools::SlashesDecode( titleTmp );

		replace( titleTmp, "\n", "-" );
		replace( titleTmp, "\t", " " );

		return BaseTitle() + " - " + titleTmp + n2s< std::string >( m_groupNumber );
	}

	CPlotField* GetPlotField( size_t index );

	CPlotField* FindPlotField( const std::string& fieldName, bool* withContour = NULL, bool* withSolidColor = NULL );

	void GetAllInternalFiles( CObArray& allInternalFiles );

	void SetForcedVarXname( const std::string& value ) { m_forcedVarXName = value; }
	const std::string& GetForcedVarXname() { return m_forcedVarXName; }

	void SetForcedVarYname( const std::string& value ) { m_forcedVarYName = value; }
	const std::string& GetForcedVarYname() { return m_forcedVarYName; }

	virtual void GetVar( const std::string& varName, CInternalFiles* file, ExpressionValueDimensions* dimVal, CExpressionValue* var );

	virtual void GetForcedAxisX( CInternalFiles* file, ExpressionValueDimensions* dimVal, CExpressionValue* varX );
	virtual void GetForcedAxisY( CInternalFiles* file, ExpressionValueDimensions* dimVal, CExpressionValue* varY );
};

#endif		//DATA_MODELS_PLOT_DATA_PLOTBASE_H
