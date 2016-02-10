/*
* 
*
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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "PlotBase.h"
#endif

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "libbrathl/InternalFiles.h"
#include "new-gui/Common/tools/Trace.h"
#include "libbrathl/Tools.h"
#include "new-gui/Common/tools/Exception.h"

using namespace brathl;

#include "PlotBase.h"
#include "PlotField.h"



//-------------------------------------------------------------
//------------------- CPlotBase class --------------------
//-------------------------------------------------------------

//----------------------------------------
CPlotField* CPlotBase::GetPlotField(int32_t index)
{
  int32_t nCount = (int32_t)m_fields.size();

  if ( (index < 0) || (index >= nCount))
  {
    std::string msg = CTools::Format("CPlotBase::GetPlotField - invalid index %d"
                                "- Valid range is [0, %d]",
                                index, nCount);
    CException e(msg,
                 BRATHL_LOGIC_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);
  }

  CPlotField* field = dynamic_cast<CPlotField*>(m_fields[index]);
  if (field == NULL)
  {
    CException e(CTools::Format("CPlotBase::GetPlotField - dynamic_cast<CPlotField*>(m_fields[%d]) returns NULL"
                                "- Valid range is [0, %d]"
                                "- object seems not to be an instance of CPlotField",
                                index, nCount),
                 BRATHL_LOGIC_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);

  }

  return field;
}
//----------------------------------------
CPlotField* CPlotBase::FindPlotField(const std::string& fieldName, bool* withContour, bool* withSolidColor )	// /withContour = NULL, bool* withSolidColor = NULL
{
  CObArray::iterator it;

  for (it = m_fields.begin() ; it != m_fields.end() ; it++)
  {
    CPlotField* plotField = CPlotField::GetPlotField(*it);
    if (plotField->m_name.compare(fieldName.c_str()) == 0)
    {
      //----------------------------------------------------
      if ((withContour == NULL) && (withSolidColor == NULL))
      //----------------------------------------------------
      {
        return plotField;
      }


      bool plotFieldWithContour = false;
      bool plotFieldWithSolidColor = true;

	  //femmTODO: uncommnet next 3 blocks
      //if (plotField->m_worldProps != NULL)
      //{
      //  plotFieldWithContour = plotField->m_worldProps->m_withContour;
      //  plotFieldWithSolidColor = plotField->m_worldProps->m_solidColor;
      //}

      //if (plotField->m_zfxyProps != NULL)
      //{
      //  plotFieldWithContour = plotField->m_zfxyProps->m_withContour;
      //  plotFieldWithSolidColor = plotField->m_zfxyProps->m_solidColor;
      //}

      //----------------------------------------------------
      if ((withContour != NULL) && (withSolidColor != NULL))
      //----------------------------------------------------
      {
        if ((plotFieldWithContour != *withContour) || (plotFieldWithSolidColor != *withSolidColor))
        {
          continue;
        }
        else
        {
          return plotField;
        }
      }

      //----------------------------------------------------
      if (withContour != NULL)
      //----------------------------------------------------
      {
        if (plotFieldWithContour != *withContour)
        {
          continue;
        }
        else
        {
          return plotField;
        }
      }

      //----------------------------------------------------
      if (withSolidColor != NULL)
      //----------------------------------------------------
      {
        if (plotFieldWithSolidColor != *withSolidColor)
        {
          continue;
        }
        else
        {
          return plotField;
        }
      }
    }
  }
  return NULL;
}
/*
//----------------------------------------
CPlotField* CPlotBase::FindPlotField(const wxString& fieldName, bool withContour)
{
  CPlotField* plotField = FindPlotField(fieldName);
  if (plotField == NULL)
  {
    return NULL;
  }

  if (plotField->m_worldProps != NULL)
  {
    if (plotField->m_worldProps->m_withContour != withContour)
    {
      return NULL;
    }
  }
  else if (plotField->m_zfxyProps != NULL)
  {
    if (plotField->m_zfxyProps->m_withContour != withContour)
    {
      return NULL;
    }
  }

  return plotField;
}
*/
//----------------------------------------
void CPlotBase::GetAllInternalFiles(CObArray& allInternalFiles)
{

  CObArray::iterator itField;
  CObArray::iterator itInternalFile;

  CPlotField* field = NULL;

  for (itField = m_fields.begin() ; itField != m_fields.end() ; itField++)
  {
    field = CPlotField::GetPlotField(*itField);
    if (field == NULL)
    {
      continue;
    }
    for (itInternalFile = field->m_internalFiles.begin() ; itInternalFile != field->m_internalFiles.end() ; itInternalFile++)
    {
      CInternalFiles* f = GetInternalFiles(*itInternalFile);
      allInternalFiles.Insert(f);
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
  if (dimVal != NULL)
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
  if (var != NULL)
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
