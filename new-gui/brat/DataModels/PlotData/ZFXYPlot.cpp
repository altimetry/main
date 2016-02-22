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

#include "new-gui/Common/+Utils.h"
#include "new-gui/Common/tools/Trace.h"
#include "new-gui/Common/tools/Exception.h"

#include "libbrathl/Tools.h"
#include "libbrathl/InternalFiles.h"
#include "libbrathl/InternalFilesYFX.h"
#include "libbrathl/InternalFilesZFXY.h"
using namespace brathl;



#include "ZFXYPlot.h"


//-------------------------------------------------------------
//------------------- CZFXYPlot class --------------------
//-------------------------------------------------------------
CZFXYPlot::CZFXYPlot( uint32_t groupNumber )
	: CPlotBase( groupNumber )
{}


//----------------------------------------
CZFXYPlot::~CZFXYPlot()
{

}

//----------------------------------------
void CZFXYPlot::GetInfo()
{

  CUnit unitXRead;
  CUnit unitYRead;

  std::string unitXStr;
  std::string unitYStr;

  bool assignYUnit = true;
  bool assignYTitle = true;
  bool assignXUnit = true;
  bool assignXTitle = true;

  CObArray allInternalFiles(false);


  GetAllInternalFiles(allInternalFiles);

  CStringArray varDimsRef;

  CObArray::iterator itField;
  CObArray::iterator itFile;

  // Check dimensions of the plotted variables
  for (itField = m_fields.begin() ; itField != m_fields.end() ; itField++)
  {
    CPlotField* field = CPlotField::GetPlotField(*itField);
    std::string fieldName = field->m_name;

    CInternalFiles* zfxy = CZFXYPlot::GetInternalFiles(*(field->m_internalFiles.begin()));

    CStringArray varDimNames;
    zfxy->GetVarDims(fieldName, varDimNames );

    if (varDimNames.size() != 2)
    {
      std::string msg = CTools::Format("CZFXYPlot::GetInfo - '%s' variable -> number of dimensions must be equal to 2 - Found : %ld",
                                   fieldName.c_str(), (long)varDimNames.size());
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
    }

    if (varDimsRef.size() <= 0)
    {
      varDimsRef.Insert(varDimNames);
    }


    if (varDimsRef != varDimNames)
    {
      std::string msg = "CZFXYPlot::GetInfo - All variables to be plotted don't have the same dimensions";
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);

    }

  }


  std::string varXName = varDimsRef.at(0);
  std::string varYName = varDimsRef.at(1);

  if (!m_forcedVarXName.empty())
  {
     varXName = m_forcedVarXName;
  }

  if (!m_forcedVarYName.empty())
  {
     varYName = m_forcedVarYName;
  }



  //
  for (itField = m_fields.begin() ; itField != m_fields.end() ; itField++)
  {
    CPlotField* field = CPlotField::GetPlotField(*itField);
    std::string fieldName = field->m_name;

    if ((field->m_xyProps != NULL) && (m_title.empty()) )
    {
	  //femmTODO: uncomment next line
      //m_title = field->m_xyProps->GetTitle();
    }

    for (itFile = field->m_internalFiles.begin() ; itFile != field->m_internalFiles.end() ; itFile++)
    {
      CInternalFiles* zfxy = CZFXYPlot::GetInternalFiles(*itFile);

      //-----------------------------------
      // Get plot Title --> title of the first file
      //-----------------------------------
      if (m_title.empty())
      {
        m_title = zfxy->GetTitle("").c_str();
      }

      //-----------------------------------
      // Get and control unit of X axis
      //-----------------------------------

      unitXRead = zfxy->GetUnit(varXName);

      if (assignXUnit)
      {
        m_unitX = unitXRead;
        unitXStr = m_unitX.AsString();
        m_unitXLabel = std::string("\nUnit:\t") + m_unitX.GetText().c_str();
        assignXUnit = false;
      }
      else
      {
        std::string unitXReadStr = unitXRead.AsString();
        if (m_unitX.IsCompatible(unitXRead) == false)
        {
          std::string msg = CTools::Format("CZFXYPlot::GetInfo - In group field number %d, X field unit are not in the same way (not compatible)"
                                      "- Expected unit '%s' and found '%s' for axis X - File name is '%s'",
                                       m_groupNumber,
                                       unitXStr.c_str(),
                                       unitXReadStr.c_str(),
                                       zfxy->GetName().c_str());
          CException e(msg, BRATHL_INCONSISTENCY_ERROR);
          CTrace::Tracer("%s", e.what());
          throw (e);

        }
        if (unitXStr.compare(unitXReadStr) != 0)
        {
          m_unitXConv = true;
        }
      }
      //-----------------------------------
      // Get title of X axis
      //-----------------------------------
      std::string titleX;

      if (m_titleX.empty())
      {
        titleX = zfxy->GetTitle(varXName);
        if (titleX.empty())
        {
          titleX = varXName;
        }
      }

      if (assignXTitle)
      {
        m_titleX += titleX.c_str() + m_unitXLabel;
        assignXTitle = false;
      }

      //--------------------------------------------------------
      // Get and control unit of Y axis
      //---------------------------------------------------------
      unitYRead = zfxy->GetUnit(varYName);

      if ( assignYUnit )
      {
        m_unitY = unitYRead;
        unitYStr = m_unitY.AsString();
        m_unitYLabel = std::string("\nUnit:\t") + m_unitY.GetText().c_str();
        assignYUnit = false;
      }
      else
      {
        std::string unitYReadStr = unitYRead.AsString();
        if (m_unitY.IsCompatible(unitYRead) == false)
        {
          std::string msg = CTools::Format("CZFXYPlot::GetInfo - In group field number %d, Y field unit are not in the same way (not compatible)"
                                      "- Expected unit '%s' and found '%s' for axis Y - Field name is '%s' - File name is '%s'",
                                       m_groupNumber,
                                       unitYStr.c_str(),
                                       unitYReadStr.c_str(),
                                       fieldName.c_str(),
                                       zfxy->GetName().c_str());
          CException e(msg, BRATHL_INCONSISTENCY_ERROR);
          CTrace::Tracer("%s", e.what());
          throw (e);

        }
        if (unitYStr.compare(unitYReadStr) != 0)
        {
          m_unitYConv = true;
        }
      }

      //-----------------------------------
      // Get title of Y axis (as possible)
      //-----------------------------------

      std::string titleY;

      if (m_titleY.empty())
      {
        titleY = zfxy->GetTitle(varYName);
        if (titleY.empty())
        {
          titleY = varYName;
        }
      }

      if (assignYTitle)
      {
        m_titleY += titleY.c_str() + m_unitYLabel;
        assignYTitle = false;
      }


    } // end for (itFile = ...

  } //  end for (itField = ...


}
//----------------------------------------
void CZFXYPlot::GetPlotWidthHeight(CInternalFiles* zfxy,
                           const std::string& fieldName,
                           int32_t& width, int32_t& height,
                           CExpressionValue& varX, CExpressionValue& varY,
                           uint32_t& dimRangeX, uint32_t& dimRangeY,
                           std::string& varXName, std::string& varYName)
{
  bool bOk = true;

  width = -1;
  height = -1;

  CNetCDFVarDef* netCDFVarDef = zfxy->GetNetCDFVarDef(fieldName);
  CStringArray fieldDimNames;
  netCDFVarDef->GetNetCdfDimNames(fieldDimNames);

  CObArray* netCDFDims = netCDFVarDef->GetNetCDFDims();

  if (netCDFDims->size() != 2)
  {
    std::string msg = CTools::Format("CZFXYPlot::GetPlotWidthHeight - '%s' field -> number of dimensions not equal to 2 : %ld",
                                fieldName.c_str(), (long)netCDFDims->size());
    throw CException(msg, BRATHL_INCONSISTENCY_ERROR);
  }

  if (! m_forcedVarXName.empty())
  {
    bOk = netCDFVarDef->HasCommonDims(m_forcedVarXName.c_str());
    if (!bOk)
    {
      std::string msg = CTools::Format("CZFXYPlot::GetPlotWidthHeight - '%s' dimension doesn't exist for the field '%s'",
                                  m_forcedVarXName.c_str(), fieldName.c_str());
      throw CException(msg, BRATHL_INCONSISTENCY_ERROR);
    }
  }

  if (! m_forcedVarYName.empty())
  {
    bOk = netCDFVarDef->HasCommonDims(m_forcedVarYName.c_str());
    if (!bOk)
    {
      std::string msg = CTools::Format("CZFXYPlot::GetPlotWidthHeight - '%s' dimension doesn't exist for the field '%s'",
                                  m_forcedVarYName.c_str(), fieldName.c_str());
      throw CException(msg, BRATHL_INCONSISTENCY_ERROR);
    }
  }

  if ((! m_forcedVarXName.empty()) && (! m_forcedVarYName.empty()))
  {
	if ( str_icmp( m_forcedVarXName, m_forcedVarYName ) )
    {
      m_forcedVarYName = "";
    }

  }

  CStringArray complement;

  if ((m_forcedVarXName.empty()) && (! m_forcedVarYName.empty()))
  {
    complement.RemoveAll();
    netCDFVarDef->GetComplementDims(m_forcedVarYName.c_str(), complement);

    if (complement.size() != 1)
    {
      std::string msg = CTools::Format("CZFXYPlot::GetPlotWidthHeight - ERROR - Field '%s' : X dimension not found (Y "
                                  "dimension is '%s'), field dimensions are '%s'", fieldName.c_str(),
                                  m_forcedVarYName.c_str(), fieldDimNames.ToString().c_str());
      throw CException(msg, BRATHL_INCONSISTENCY_ERROR);
    }

    m_forcedVarXName = complement.at(0).c_str();

  }

  if ((! m_forcedVarXName.empty()) && (m_forcedVarYName.empty()))
  {
    complement.RemoveAll();
    netCDFVarDef->GetComplementDims(m_forcedVarXName.c_str(), complement);

    if (complement.size() != 1)
    {
      std::string msg = CTools::Format("CZFXYPlot::GetPlotWidthHeight - ERROR - Field '%s' : Y dimension not found (X "
                                  "dimension is '%s'), field dimensions are '%s'", fieldName.c_str(),
                                  m_forcedVarXName.c_str(), fieldDimNames.ToString().c_str());
      throw CException(msg, BRATHL_INCONSISTENCY_ERROR);
    }

    m_forcedVarYName = complement.at(0).c_str();

  }


  CStringArray dimNames;

  CObArray::iterator itOb;

  for (itOb = netCDFDims->begin(); itOb != netCDFDims->end(); itOb++)
  {
    CNetCDFDimension* netCDFDim = dynamic_cast<CNetCDFDimension*>(*itOb);
    dimNames.Insert(netCDFDim->GetName());
  }

  if ((! m_forcedVarXName.empty()) && (! m_forcedVarYName.empty()))
  {
    dimNames.RemoveAll();
    dimNames.Insert(m_forcedVarXName.c_str());
    dimNames.Insert(m_forcedVarYName.c_str());
  }


  for (uint32_t indexDim = 0; indexDim < dimNames.size(); indexDim++)
  {
    //CNetCDFDimension* netCDFDim = dynamic_cast<CNetCDFDimension*>(*it);
    CNetCDFCoordinateAxis* coordinateAxis = dynamic_cast<CNetCDFCoordinateAxis*>(zfxy->GetNetCDFVarDef(dimNames.at(indexDim)));
    CNetCDFDimension* netCDFDim = netCDFVarDef->GetNetCDFDim(dimNames.at(indexDim));

    if (coordinateAxis == NULL)
    {
      std::string msg = CTools::Format("CZFXYPlot::GetPlotWidthHeight - '%s' axis is not found or not a coordinate axis.",
                                   dimNames.at(indexDim).c_str());
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
    }

    uint32_t nDims = (uint32_t)coordinateAxis->GetDims()->size();

    if (nDims != 1)
    {
      std::string msg = CTools::Format("CZFXYPlot::GetPlotWidthHeight - '%s' axis -> number of dimensions not equal to 1 : %ld",
                                   coordinateAxis->GetName().c_str(), (long)nDims);
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
    }

    uint32_t dimRange =  netCDFVarDef->GetNetCDFDimRange(coordinateAxis->GetName());
    if (indexDim == 0)
    {
      dimRangeX = dimRange;
      width = netCDFDim->GetLength();
      varXName = netCDFDim->GetName(),
      zfxy->ReadVar(netCDFDim->GetName(), varX, coordinateAxis->GetUnit()->GetText());
    }
    else
    {
      dimRangeY = dimRange;
      height = netCDFDim->GetLength();
      varYName = netCDFDim->GetName(),
      zfxy->ReadVar(netCDFDim->GetName(), varY, coordinateAxis->GetUnit()->GetText());
    }

  }

  if (width == -1)
    {
      std::string msg = CTools::Format("CZFXYPlot::GetPlotWidthHeight - X axis not found in input file '%s'",
                                   zfxy->GetName().c_str());
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
    }

  if (height == -1)
    {
      std::string msg = CTools::Format("CZFXYPlot::GetPlotWidthHeight - Y axis not found in input file '%s'",
                                   zfxy->GetName().c_str());
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
    }

}
//----------------------------------------
CInternalFiles* CZFXYPlot::GetInternalFiles(CBratObject* ob, bool withExcept /* = true */)
{
  CInternalFiles* f = dynamic_cast<CInternalFiles*>(ob);
  if (f == NULL)
  {
    if (withExcept)
    {
      CException e("CZFXYPlot::GetInternalFiles -  dynamic_cast<CInternalFiles*>(ob) returns NULL",
                   BRATHL_LOGIC_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
    }

  }
  return f;
}
//----------------------------------------
CInternalFilesZFXY* CZFXYPlot::GetInternalFilesZFXY(CBratObject* ob, bool withExcept /* = true */)
{
  CInternalFilesZFXY* zfxy = dynamic_cast<CInternalFilesZFXY*>(ob);
  if (zfxy == NULL)
  {
    if (withExcept)
    {
      CException e("CZFXYPlot::GetInternalFilesZFXY - Non-ZFXY data found - dynamic_cast<CInternalFilesZFXY*>(ob) returns NULL",
                   BRATHL_LOGIC_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
    }

  }
  if (zfxy->IsGeographic() == true)
  {
    if (withExcept)
    {
      CException e("CZFXYPlot::GetInternalFilesZFXY  - a ZFXY data seems to be geographical  - zfxy->IsGeographic() is true",
                   BRATHL_LOGIC_ERROR);
      CTrace::Tracer("%s", e.what());
      throw e;
    }
  }

  return zfxy;

}
//----------------------------------------
CInternalFilesYFX* CZFXYPlot::GetInternalFilesYFX(CBratObject* ob, bool withExcept /* = true */)
{
  CInternalFilesYFX* zfxy = dynamic_cast<CInternalFilesYFX*>(ob);
  if (zfxy == NULL)
  {
    if (withExcept)
    {
      CException e("CZFXYPlot::GetInternalFilesYFX - Non-ZFXY data found - dynamic_cast<CInternalFilesYFX*>(ob) returns NULL",
                   BRATHL_LOGIC_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
    }

  }

  return zfxy;

}





