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

#include "new-gui/Common/tools/Trace.h"
#include "new-gui/Common/tools/Exception.h"
#include "libbrathl/InternalFiles.h"
#include "libbrathl/Tools.h"

using namespace brathl;

#include "PlotBase.h"
#include "PlotField.h"
#include "WorldPlotData.h"
#include "ZFXYPlotData.h"



//-------------------------------------------------------------
//------------------- CPlotBase class --------------------
//-------------------------------------------------------------

//----------------------------------------
CPlotField* CPlotBase::GetPlotField( size_t index )
{
	assert__( index < mFields.size() );

    return mFields[ index ];
}
//----------------------------------------
CPlotField* CPlotBase::FindPlotField( const std::string& fieldName, bool *withContour, bool *withSolidColor )	// /withContour = nullptr, bool* withSolidColor = nullptr
{
	for ( auto *plotField : mFields )
	{
		if ( plotField->m_name == fieldName )
		{
			bool plotFieldWithContour = false;
			bool plotFieldWithSolidColor = true;

			if ( plotField->m_worldProps != nullptr )
			{
				plotFieldWithContour = plotField->m_worldProps->m_withContour;
				plotFieldWithSolidColor = plotField->m_worldProps->m_solidColor;
			}

			if ( plotField->m_zfxyProps != nullptr )
			{
				plotFieldWithContour = plotField->m_zfxyProps->m_withContour;
				plotFieldWithSolidColor = plotField->m_zfxyProps->m_solidColor;
			}

			if ( ( !withContour || *withContour == plotFieldWithContour ) &&
				( !withSolidColor || *withSolidColor == plotFieldWithSolidColor ) )
			{
				return plotField;
			}
		}
	}

	return nullptr;
}
//----------------------------------------
void CPlotBase::GetAllInternalFiles( CObArray& allInternalFiles )
{
	for ( auto *field : mFields )
	{
		if ( field != nullptr )
		{
			for ( CObArray::iterator itInternalFile = field->m_internalFiles.begin(); itInternalFile != field->m_internalFiles.end(); itInternalFile++ )
			{
				CInternalFiles* f = GetInternalFiles( *itInternalFile );
				allInternalFiles.Insert( f );
			}
		}
	}
}
//----------------------------------------
void CPlotBase::GetVar(const std::string& varName, CInternalFiles* file,
                     ExpressionValueDimensions* dimVal,
                     CExpressionValue* var)
{
  if (file->VarExists(varName) == false)
  {
    std::string msg = CTools::Format("CPlotBase::GetAxis -  unknown variable name: '%s'",
                                varName.c_str());
    CException e(msg, BRATHL_INCONSISTENCY_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);
  }

  // Get dim
  if (dimVal != nullptr)
  {
    file->GetVarDims(varName.c_str(), *dimVal);
    if ( (dimVal->size() <= 0) || (dimVal->size() > 2) )
    {
      std::string msg = CTools::Format("CPlotBase::GetAxis - '%s' axis -> number of dimensions must be 1 or 2 - Found : %ld",
                                   varName.c_str(), (long)dimVal->size());
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
    }
  }

  // Get values
  if (var != nullptr)
  {
    file->ReadVar(varName, *var, file->GetUnit(varName).GetText());
  }


}

//----------------------------------------
void CPlotBase::GetForcedAxisX(CInternalFiles* file,
                     ExpressionValueDimensions* dimVal,
                     CExpressionValue* varX)
{
  GetVar(m_forcedVarXName.c_str(), file, dimVal, varX);
}
//----------------------------------------
void CPlotBase::GetForcedAxisY(CInternalFiles* file,
                     ExpressionValueDimensions* dimVal,
                     CExpressionValue* varY)
{
  GetVar(m_forcedVarYName.c_str(), file, dimVal, varY);
}
