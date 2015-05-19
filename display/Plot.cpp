/*
* Copyright (C) 2005 Collecte Localisation Satellites (CLS), France (31)
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
    #pragma implementation "Plot.h"
#endif

// For compilers that support precompilation
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "Trace.h"
#include "Tools.h"
#include "Exception.h"
#include "LatLonPoint.h"

using namespace brathl;


#include "BratDisplay.h"
#include "Plot.h"


//-------------------------------------------------------------
//------------------- CPlot class --------------------
//-------------------------------------------------------------

CPlot::CPlot(uint32_t groupNumber)
      : CPlotBase(groupNumber)
{
  Init();

}

//----------------------------------------
CPlot::~CPlot()
{

}
//----------------------------------------
void CPlot::Init()
{
  m_unitXConv = false;
  m_unitYConv = false;
}
//----------------------------------------
void CPlot::GetInfo()
{

  CUnit unitXRead;
  CUnit unitYRead;

  string unitXStr;
  string unitYStr;

  bool assignYUnit = true;
  bool assignYTitle = true;
  bool assignXUnit = true;
  bool assignXTitle = true;

  CObArray allInternalFiles(false);


  GetAllInternalFiles(allInternalFiles);
  int32_t nrFiles = allInternalFiles.size();

  //int32_t iField = 0;
  int32_t nrFields = m_fields.size();
  CObArray::iterator itField;
  CObArray::iterator itFile;

  CStringArray plotFieldNames;

  for (itField = m_fields.begin() ; itField != m_fields.end() ; itField++)
  {
    CPlotField* field = CPlotField::GetPlotField(*itField);
    string fieldName = (const char *)(field->m_name);

    plotFieldNames.InsertUnique(fieldName);

    if ((field->m_xyProps != NULL) && (m_title.IsEmpty()) )
    {
      m_title = field->m_xyProps->GetTitle();
    }

    for (itFile = field->m_internalFiles.begin() ; itFile != field->m_internalFiles.end() ; itFile++)
    {

      CInternalFiles* yfx = CPlot::GetInternalFiles(*itFile);
      //-----------------------------------
      // Get plot Title --> title of the first file
      //-----------------------------------
      if (m_title.IsEmpty())
      {
        m_title = yfx->GetTitle("").c_str();
      }

      //-----------------------------------
      // Get and control unit of X axis
      //-----------------------------------
      string varXName;
      GetAxisX(yfx, NULL, NULL, &varXName);

      unitXRead = yfx->GetUnit(varXName);


      if (assignXUnit)
      {
        m_unitX = unitXRead;
        unitXStr = m_unitX.AsString();
        m_unitXLabel = "\nUnit:\t" + wxString(m_unitX.GetText().c_str());
        assignXUnit = false;
      }
      else
      {
        string unitXReadStr = unitXRead.AsString();
        if (m_unitX.IsCompatible(unitXRead) == false)
        {
          string msg = CTools::Format("CPlot::CkeckUnits - In group field number %d, X field unit are not in the same way (not compatible)"
                                      "- Expected unit '%s' and found '%s' for axis X - File name is '%s'",
                                       m_groupNumber,
                                       unitXStr.c_str(),
                                       unitXReadStr.c_str(),
                                       yfx->GetName().c_str());
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
      string titleX;

      if (m_titleX.IsEmpty())
      {
        titleX = yfx->GetTitle(varXName);
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
      // Get and control unit of Y axis --> unit of each field
      //---------------------------------------------------------
      unitYRead = yfx->GetUnit(fieldName);

      if ( assignYUnit )
      {
        m_unitY = unitYRead;
        unitYStr = m_unitY.AsString();
        m_unitYLabel = "\nUnit:\t" + wxString(m_unitY.GetText().c_str());
        assignYUnit = false;
      }
      else
      {
        string unitYReadStr = unitYRead.AsString();
        if (m_unitY.IsCompatible(unitYRead) == false)
        {
          string msg = CTools::Format("CXYPlotData::Create - In group field number %d, Y field unit are not in the same way (not compatible)"
                                      "- Expected unit '%s' and found '%s' for axis Y - Field name is '%s' - File name is '%s'",
                                       m_groupNumber,
                                       unitYStr.c_str(),
                                       unitYReadStr.c_str(),
                                       fieldName.c_str(),
                                       yfx->GetName().c_str());
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

      string titleY;

      if ((nrFields == 1) && (nrFiles <= 1))
      {
        if (m_titleY.IsEmpty())
        {
          titleY = yfx->GetTitle(fieldName);
          if (titleY.empty())
          {
            titleY = fieldName;
          }
        }
      }

      if (assignYTitle)
      {
        m_titleY += titleY.c_str() + m_unitYLabel;
        assignYTitle = false;
      }


    } // end for (itFile = ...

  } //  end for (itField = ...

  //CTrace::GetInstance()->SetTraceLevel(5);
  //plotFieldNames.Dump(*(CTrace::GetInstance()->GetDumpContext()));

  if (plotFieldNames.size() > 0)
  {
    for (itFile = allInternalFiles.begin() ; itFile != allInternalFiles.end() ; itFile++)
    {
      CInternalFiles* yfx = CPlot::GetInternalFiles(*itFile);

      if (yfx == NULL)
      {
        continue;
      }

      CStringArray complement;

      yfx->GetComplementVars(plotFieldNames, complement);

      CStringArray::iterator itStringArray;
      for (itStringArray = complement.begin() ; itStringArray != complement.end(); itStringArray++)
      {
        CNetCDFVarDef* v1Def = yfx->GetNetCDFVarDef(plotFieldNames.at(0));
        CNetCDFVarDef* v2Def = yfx->GetNetCDFVarDef(*itStringArray);

        if ((v1Def == NULL) || (v2Def == NULL))
        {
          continue;
        }


        if (yfx->GetFile()->HaveEqualDimNames(plotFieldNames.at(0), *itStringArray))
        {
          m_nonPlotFieldNames.InsertUnique(*itStringArray);
        }

      }

      //complement.Dump(*(CTrace::GetInstance()->GetDumpContext()));


    }

    //m_nonPlotFieldNames.Dump(*(CTrace::GetInstance()->GetDumpContext()));
  }

/*


  for (iFile = 0 ; iFile < nrFiles ; iFile++)
  {
    CInternalFilesYFX* yfx = GetInternalFilesYFX(allInternalFiles.at(iFile));
    //-----------------------------------
    // Get plot Title --> title of the first file
    //-----------------------------------
    if (m_title.IsEmpty())
    {
      m_title = yfx->GetTitle("").c_str();
    }

    //-----------------------------------
    // Get and control unit of X axis
    //-----------------------------------
    string varXName;
    GetAxisX(yfx, NULL, NULL, &varXName);

    unitXRead = yfx->GetUnit(varXName);



    //--------------------------------------------------------
    // Get and control unit of Y axis --> unit of each field
    //---------------------------------------------------------

    //int32_t iField = 0;
    int32_t nrFields = m_fields.size();
    CObArray::iterator itField;

    for (itField = m_fields.begin() ; itField != m_fields.end() ; itField++)
    {

      if (assignXUnit)
      {
        m_unitX = unitXRead;
        unitXStr = m_unitX.AsString();
        m_unitXLabel = "\nUnit:\t" + wxString(m_unitX.GetText().c_str());
        assignXUnit = false;
      }
      else
      {
        string unitXReadStr = unitXRead.AsString();
        if (m_unitX.IsCompatible(unitXRead) == false)
        {
          string msg = CTools::Format("CPlot::CkeckUnits - In group field number %d, X field unit are not in the same way (not compatible)"
                                      "- Expected unit '%s' and found '%s' for axis X - File name is '%s'",
                                       m_groupNumber,
                                       unitXStr.c_str(),
                                       unitXReadStr.c_str(),
                                       yfx->GetName().c_str());
          CException e(msg, BRATHL_INCONSISTENCY_ERROR);
          CTrace::Tracer(e.what());
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
      string titleX;

      if (m_titleX.IsEmpty())
      {
        titleX = yfx->GetTitle(varXName);
        if (titleX.empty())
        {
          titleX = varXName;
        }
      }

      //if (iFile == 0)
      if (assignXTitle)
      {
        m_titleX += titleX.c_str() + m_unitXLabel;
        assignXTitle = false;
      }


      CPlotField* field = CPlotField::GetPlotField(*itField);
      string fieldName = field->m_name.c_str();

      unitYRead = yfx->GetUnit(fieldName);

      if ( assignYUnit )
      {
        m_unitY = unitYRead;
        unitYStr = m_unitY.AsString();
        m_unitYLabel = "\nUnit:\t" + wxString(m_unitY.GetText().c_str());
        assignYUnit = false;
      }
      else
      {
        string unitYReadStr = unitYRead.AsString();
        if (m_unitY.IsCompatible(unitYRead) == false)
        {
          string msg = CTools::Format("CXYPlotData::Create - In group field number %d, Y field unit are not in the same way (not compatible)"
                                      "- Expected unit '%s' and found '%s' for axis Y - Field name is '%s' - File name is '%s'",
                                       m_groupNumber,
                                       unitYStr.c_str(),
                                       unitYReadStr.c_str(),
                                       fieldName.c_str(),
                                       yfx->GetName().c_str());
          CException e(msg, BRATHL_INCONSISTENCY_ERROR);
          CTrace::Tracer(e.what());
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

      string titleY;

      if ((nrFields == 1) && (nrFiles <= 1))
      {
        if (m_titleY.IsEmpty())
        {
          titleY = yfx->GetTitle(fieldName);
          if (titleY.empty())
          {
            titleY = fieldName;
          }
        }
      }

      if (assignYTitle)
      {
        m_titleY += titleY.c_str() + m_unitYLabel;
        assignYTitle = false;
      }
    }
  }


*/
}
//----------------------------------------
CInternalFiles* CPlot::GetInternalFiles(CBratObject* ob, bool withExcept /* = true */)
{
  CInternalFiles* f = dynamic_cast<CInternalFiles*>(ob);
  if (f == NULL)
  {
    if (withExcept)
    {
      CException e("CPlot::GetInternalFiles -  dynamic_cast<CInternalFiles*>(ob) returns NULL",
                   BRATHL_LOGIC_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
    }

  }
  return f;
}

//----------------------------------------
CInternalFilesYFX* CPlot::GetInternalFilesYFX(CBratObject* ob)
{
  CInternalFilesYFX* yfx = dynamic_cast<CInternalFilesYFX*>(ob);
  if (yfx == NULL)
  {
    CException e("CPlot::GetInternalFilesYFX - Non-YFX data found - dynamic_cast<CInternalFilesYFX*>(ob) returns NULL",
                 BRATHL_LOGIC_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);

  }
  if (yfx->IsGeographic() == true)
  {
    CException e("CPlot::GetInternalFilesYFX  - a YFX data seems to be geographical  - yfx->IsGeographic() is true",
                 BRATHL_LOGIC_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);
  }

  return yfx;

}


/*
//----------------------------------------
CPlotField* CPlot::GetPlotField(const wxString& fieldName)
{

  CPlotField* field = dynamic_cast<CPlotField*>(m_fields.Exists(fieldName));
  return field;
}
*/



//----------------------------------------
void CPlot::GetAxisX(CInternalFiles* yfx,
                     ExpressionValueDimensions* dimVal,
                     CExpressionValue* varX,
                     string* varXName)
{


  if (!m_forcedVarXName.empty())
  {
    if (varXName != NULL)
    {
      *varXName = m_forcedVarXName.c_str();
    }

    GetForcedAxisX(yfx, dimVal, varX);
    return;
  }


  CStringArray axisNames;
  CStringArray::iterator is;
  //ExpressionValueDimensions dimVal;

  yfx->GetAxisVars(axisNames);

  if (axisNames.size() != 1)
  {
    string msg = CTools::Format("CPlot::GetAxisX -  wrong number of axis in file '%s' : %ld"
                                "Correct number is 1",
                                yfx->GetName().c_str(),
                                (long)axisNames.size());
    CException e(msg, BRATHL_INCONSISTENCY_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);
  }

  for (is = axisNames.begin(); is != axisNames.end(); is++)
  {
    // Get dim
    if (dimVal != NULL)
    {
      yfx->GetVarDims(*is, *dimVal);
      if ( (dimVal->size() <= 0) || (dimVal->size() > 2) )
      {
        string msg = CTools::Format("CPlot::GetAxisX - '%s' axis -> number of dimensions must be 1 or 2 - Found : %ld",
                                     (*is).c_str(), (long)dimVal->size());
        CException e(msg, BRATHL_INCONSISTENCY_ERROR);
        CTrace::Tracer("%s", e.what());
        throw (e);
      }
    }

    // Get values
    if (varX != NULL)
    {
      /*
      string wantedUnit;

      NetCDFVarKind varKind = yfx->GetVarKind(*is);
      if (varKind == Longitude)
      {
        wantedUnit = CLatLonPoint::m_DEFAULT_UNIT_LONGITUDE;
      }
      else if (varKind == Latitude)
      {
        wantedUnit = CLatLonPoint::m_DEFAULT_UNIT_LATITUDE;
      }
      else
      {
        CUnit unitX = yfx->GetUnit(*varXName);
        wantedUnit = unitX.GetText();
      }

      yfx->ReadVar(*is, *varX, wantedUnit);
      */
      yfx->ReadVar(*is, *varX, yfx->GetUnit(*is).GetText());
    }

    // Get name
    if (varXName != NULL)
    {
      *varXName = (*is);
    }
  }

}

