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
    #pragma implementation "XYPlotData.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "Trace.h"
#include "Exception.h"
#include "ExternalFiles.h"
#include "ExternalFilesNetCDF.h"
#include "ExternalFilesFactory.h"
#include "Field.h"

using namespace brathl;

#include "vtkObjectFactory.h"

#include "MapColor.h"
#include "XYPlotData.h"


//-------------------------------------------------------------
//------------------- vtkDoubleArrayBrathl class --------------------
//-------------------------------------------------------------

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkDoubleArrayBrathl);

//----------------------------------------------------------------------------
vtkDoubleArrayBrathl::vtkDoubleArrayBrathl(vtkIdType numComp): vtkDoubleArray(numComp)
{
}

//----------------------------------------------------------------------------
vtkDoubleArrayBrathl::~vtkDoubleArrayBrathl()
{
}

//----------------------------------------------------------------------------
void vtkDoubleArrayBrathl::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkDoubleArray::PrintSelf(os,indent);
}
//----------------------------------------------------------------------------

void vtkDoubleArrayBrathl::ComputeRange(int comp)
{
  //---------------------------
  vtkDoubleArray::ComputeRange(comp);
  //---------------------------
  double s,t;
  vtkIdType numTuples;

  if (comp < 0 && this->NumberOfComponents == 1)
    {
    comp = 0;
    }

  int idx = comp;
  idx = (idx<0)?(4):(idx);

  if ( (this->GetMTime() > this->ComponentRangeComputeTime2[idx]) )
    {
    numTuples=this->GetNumberOfTuples();
    this->Range2[0] =  VTK_DOUBLE_MAX;
    this->Range2[1] =  VTK_DOUBLE_MIN;

    for (vtkIdType i=0; i<numTuples; i++)
      {
      if (comp >= 0)
        {
        s = this->GetComponent(i,comp);
        if (CTools::IsDefaultValue(s))
          {
          //------------------
          continue;
          //------------------
          }
        }
      else
        { // Compute range of vector magnitude.
        s = 0.0;
        for (int j=0; j < this->NumberOfComponents; ++j)
          {
          t = this->GetComponent(i,j);
          if (CTools::IsDefaultValue(t))
            {
            //------------------
            continue;
            //------------------
            }
          s += t*t;
          }
        s = sqrt(s);
        }
      if ( s < this->Range2[0] )
        {
        this->Range2[0] = s;
        }
      if ( s > this->Range2[1] )
        {
        this->Range2[1] = s;
        }
      }
    this->ComponentRangeComputeTime2[idx].Modified();
    this->ComponentRange2[idx][0] = this->Range2[0];
    this->ComponentRange2[idx][1] = this->Range2[1];
    }
  else
    {
    this->Range2[0] = this->ComponentRange2[idx][0];
    this->Range2[1] = this->ComponentRange2[idx][1];
    }
}


//-------------------------------------------------------------
//------------------- CRow class --------------------
//-------------------------------------------------------------
/*
CRow::CRow(const CDoubleArray& vect)
{
  SetValues(vect);
}
//----------------------------------------
CRow::~CRow()
{
}
void CRow::SetValues(const CDoubleArray& vect)
{
  m_values.RemoveAll();
  m_values.Insert(vect);

}
*/
/*
//-------------------------------------------------------------
//------------------- CRowDouble class --------------------
//-------------------------------------------------------------
CRowDouble::CRowDouble()
{
}
//----------------------------------------
CRowDouble::~CRowDouble()
{
}

*/

//-------------------------------------------------------------
//------------------- CPlotArray class --------------------
//-------------------------------------------------------------
CPlotArray::CPlotArray()
{
  m_didCalculateRange = false;
  CTools::SetDefaultValue(m_min);
  CTools::SetDefaultValue(m_max);
}
//----------------------------------------
CPlotArray::~CPlotArray()
{
}
//----------------------------------------
vtkDoubleArrayBrathl* CPlotArray::GetFrameData(uint32_t r)
{
  if (r >= m_rows.size())
  {
    r = m_rows.size() - 1;
  }

  vtkDoubleArrayBrathl* data = dynamic_cast<vtkDoubleArrayBrathl*>(m_rows.at(r));
  if (data == NULL)
  {
    string msg = CTools::Format("ERROR in CPlotArray::GetFrameData: dynamic_cast<vtkDoubleArrayBrathl*>(m_rows.at(%d) returns NULL pointer - "
                                "Plot Array (rows) seems to contain objects other than those of the class vtkDoubleArrayBrathl class", r);
    CException e(msg, BRATHL_LOGIC_ERROR);
    throw (e);
  }

  return data;

}
//----------------------------------------
vtkDoubleArrayBrathl* CPlotArray::GetFrameData(vtkObArray::iterator it)
{
  vtkDoubleArrayBrathl* data = dynamic_cast<vtkDoubleArrayBrathl*>(*it);
  if (data == NULL)
  {
    throw CException("ERROR in  GetFrameData(CObArray::iterator it) : dynamic_cast<vtkDoubleArrayBrathl*>(*it); returns NULL pointer - "
                 "CPlot Array seems to contain objects other than those of the class vtkDoubleArrayBrathl or derived class", BRATHL_LOGIC_ERROR);
  }
  return data;

}
//----------------------------------------
void CPlotArray::SetFrameData(uint32_t r, double* vect, int32_t size, bool bCopy)
{
  double* newVect = NULL;
  if (bCopy)
  {
    newVect = new double[size];
    memcpy(newVect, vect, size * sizeof(*vect));
  }
  else
  {
    newVect = vect;
  }
  vtkDoubleArrayBrathl* vtkArray = vtkDoubleArrayBrathl::New();
  // This method lets the user specify data to be held by the array.
  // The array argument is a pointer to the data.
  // size is the size of the array supplied by the user.
  // Set save to 1 to keep the class from deleting the array when it cleans up or reallocates memory.
  // The class uses the actual array provided; it does not copy the data from the suppled array
  vtkArray->SetArray(newVect, size, 0);

  SetFrameData(r, vtkArray);

}
//----------------------------------------
void CPlotArray::SetFrameData(uint32_t r, vtkDoubleArrayBrathl* vect)
{
  //if (r == m_rows.size())
  if (r >= m_rows.size())
  {
    m_rows.Insert(vect);
  }
  else if (r < m_rows.size())
  {
    m_rows.ReplaceAt(m_rows.begin() + r, vect);
  }
  else
  {
    string msg = CTools::Format("ERROR in  CPlotArray::SetFrameData : index %d out-of-range : [0, %ld]",
                                r, (long)m_rows.size());
    CException e(msg, BRATHL_LOGIC_ERROR);
    throw (e);

  }
  m_didCalculateRange = false;

}
//----------------------------------------
void CPlotArray::GetRange(double& min, double& max, uint32_t frame)
{

  CTools::SetDefaultValue(min);
  CTools::SetDefaultValue(max);

  vtkDoubleArrayBrathl* data = GetFrameData(frame);
  if (data == NULL)
  {
    return;
  }

  min = data->GetRange2()[0];
  max = data->GetRange2()[1];

}
//----------------------------------------
void CPlotArray::GetRange(double& min, double& max)
{
  if (m_didCalculateRange)
  {
    min = m_min;
    max = m_max;
    return;
  }

  vtkObArray::iterator it;
  CTools::SetDefaultValue(min);
  CTools::SetDefaultValue(max);

  for (it = m_rows.begin(); it != m_rows.end() ; it++)
  {
    vtkDoubleArrayBrathl* data = GetFrameData(it);
    double l = data->GetRange2()[0];
    double h = data->GetRange2()[1];

    //data->GetRange(l, h);

    if (CTools::IsDefaultValue(min))
    {
      min = l;
      max = h;
    }
    else
    {
      min = (min > l) ? l : min;
      max = (max > h) ? max : h;
    }
  }

  m_min = min;
  m_max = max;

  m_didCalculateRange = true;
}
//----------------------------------------
void CPlotArray::Normalize(int32_t value)
{
  double l = 0;
  double h = 0;
  GetRange(l, h);

  int i =0;

  vtkObArray::iterator it;

  for (it = m_rows.begin(); it != m_rows.end() ; it++)
  {
    vtkDoubleArrayBrathl* data = GetFrameData(it);
    for (i = 0 ; i < data->GetNumberOfTuples(); i++)
    {
      data->SetValue(i, ((data->GetValue(i) - l) / (h - l)) * value);
    }
  }
  m_didCalculateRange = false;
}

//-------------------------------------------------------------
//------------------- CXYPlotProperty class --------------------
//-------------------------------------------------------------


CXYPlotProperty::CXYPlotProperty(CXYPlotData* parent)
{
  this->Init();

  m_vtkProperty2D = vtkProperty2D::New();

  SetParent(parent);

  SetLineStipple(displayFULL);

  SetLineWidth(0.8);


  SetLines(true);
  SetPoints(false);

  SetPointSize(1.0);

  SetOpacity(0.6);
  SetOpacityFactor(1 / GetOpacity());

  CTools::SetDefaultValue(m_xMax);
  CTools::SetDefaultValue(m_xMin);
  CTools::SetDefaultValue(m_yMax);
  CTools::SetDefaultValue(m_yMin);


  SetXLog(false);
  SetYLog(false);

  SetShowAnimationToolbar(false);
  SetShowPropertyPanel(true);

  SetLoop(false);

  SetNormalizeX(0);
  SetNormalizeY(0);

  SetFps(30);

  CTools::SetDefaultValue(m_xNumTicks);
  CTools::SetDefaultValue(m_yNumTicks);

  SetXBase(10.0);
  SetYBase(10.0);

  CVtkColor vtkColor = CMapColor::GetInstance().NextPrimaryColors();
  if (vtkColor.Ok())
  {
    SetColor(vtkColor);
  }
  else
  {
    SetColor(CVtkColor(0.0, 0.0, 0.0));
  }

  SetPointGlyph(displayCIRCLE_GLYPH);
  SetFilledPoint(true);

  SetLineWidthFactor(1.5);

  SetHide(false);
}

//----------------------------------------
CXYPlotProperty::CXYPlotProperty(CXYPlotProperty& p)
{
  this->Init();
  this->Copy(p);
}

//----------------------------------------
const CXYPlotProperty& CXYPlotProperty::operator=(CXYPlotProperty& p)
{
  this->Copy(p);
  return *this;
}

//----------------------------------------
CXYPlotProperty::~CXYPlotProperty()
{
  if (m_vtkProperty2D != NULL)
  {
    m_vtkProperty2D->Delete();
    m_vtkProperty2D = NULL;
  }

}
//----------------------------------------
void CXYPlotProperty::Init()
{
  m_focus = false;
}
//----------------------------------------
void CXYPlotProperty::Copy(CXYPlotProperty& p)
{

  m_vtkProperty2D = vtkProperty2D::New();

  SetParent(p.GetParent());

  SetLineStipple(p.GetLineStipple());

  SetLineWidth(p.GetLineWidth());
  SetLineWidthFactor(p.GetLineWidthFactor());
  m_focus = p.GetFocus();

  SetLines(p.GetLines());
  SetPoints(p.GetPoints());

  SetPointSize(p.GetPointSize());

  SetOpacity(p.GetOpacity());

  SetXMax(p.GetXMax());
  SetYMax(p.GetYMax());

  SetXMin(p.GetXMin());
  SetYMin(p.GetYMin());

  SetXLog(p.GetXLog());
  SetYLog(p.GetYLog());

  SetShowAnimationToolbar(p.GetShowAnimationToolbar());
  SetShowPropertyPanel(p.GetShowPropertyPanel());

  SetLoop(p.GetLoop());

  SetNormalizeX(p.GetNormalizeX());
  SetNormalizeY(p.GetNormalizeY());

  SetFps(p.GetFps());

  SetXNumTicks(p.GetXNumTicks());
  SetYNumTicks(p.GetYNumTicks());

  SetXBase(p.GetXBase());
  SetYBase(p.GetYBase());

  SetColor(p.GetColor());

  SetTitle(p.GetTitle());
  SetName(p.GetName());
  SetXAxis(p.GetXAxis());
  SetXLabel(p.GetXLabel());
  SetYLabel(p.GetYLabel());

  SetPointGlyph(p.GetPointGlyph());
  SetFilledPoint(p.GetFilledPoint());

  SetOpacityFactor(p.GetOpacityFactor());

  SetHide(p.GetHide());

//  SetInputFile(p.GetInputFile());

}
//----------------------------------------
void CXYPlotProperty::Update()
{
  SetLines();

  SetPoints();
  SetPointSize();

  SetPointGlyph();
  SetFilledPoint();

  SetNormalizeX();
  SetNormalizeY();

}
//----------------------------------------
bool CXYPlotProperty::HasParent()
{
  return m_parent != NULL;
}

//----------------------------------------
void CXYPlotProperty::SetHide(bool value)
{
  m_hide = value;

  SetHide();

}
//----------------------------------------
void CXYPlotProperty::SetHide()
{
  if (!HasParent())
  {
    return;
  }

  if (m_hide)
  {
    m_parent->GetVtkDataArrayPlotData()->PlotLinesOff();
    m_parent->GetVtkDataArrayPlotData()->PlotPointsOff();
  }
  else
  {
    SetLines();
    SetPoints();
  }

}

//----------------------------------------
void CXYPlotProperty::SetLineWidth(double value)
{
  if (value <= 0.0)
  {
    value = 0.8;
  }
  m_vtkProperty2D->SetLineWidth(value);
}
//----------------------------------------
void CXYPlotProperty::SetLines()
{

  if (!HasParent())
  {
    return;
  }

  if (m_lines)
  {
    m_parent->GetVtkDataArrayPlotData()->PlotLinesOn();
  }
  else
  {
    m_parent->GetVtkDataArrayPlotData()->PlotLinesOff();
  }

}
//----------------------------------------
void CXYPlotProperty::SetLines(bool value)
{
  m_lines = value;

  SetLines();

}
//----------------------------------------
void CXYPlotProperty::SetPoints()
{

  if (!HasParent())
  {
    return;
  }

  if (m_points)
  {
    m_parent->GetVtkDataArrayPlotData()->PlotPointsOn();
  }
  else
  {
    m_parent->GetVtkDataArrayPlotData()->PlotPointsOff();
  }

  SetFilledPoint();

}

//----------------------------------------
void CXYPlotProperty::SetPoints(bool value)
{
  m_points = value;

  SetPoints();


}

//----------------------------------------
void CXYPlotProperty::SetPointSize()
{
  if (!HasParent())
  {
    return;
  }

  m_parent->GetVtkDataArrayPlotData()->SetGlyphSize(m_pointSize / 100.0);

}

//----------------------------------------
void CXYPlotProperty::SetPointSize(double value)
{
  if (value <= 0.0)
  {
    value = 1.0;
  }
  m_pointSize = value;

  SetPointSize();

}

//----------------------------------------
void CXYPlotProperty::SetPointGlyph()
{

  if (!HasParent())
  {
    return;
  }
  if (m_points == false)
  {
    return;
  }

  m_parent->GetVtkDataArrayPlotData()->GetGlyphSource()->SetGlyphType(m_glyphType);

}
//----------------------------------------
void CXYPlotProperty::SetPointGlyph(PointGlyph value)
{
  m_glyphType = value;

  SetPointGlyph();
}
//----------------------------------------
void CXYPlotProperty::SetFilledPoint()
{

  if (!HasParent())
  {
    return;
  }
  if (m_points == false)
  {
    return;
  }

  if (m_filledPoint)
  {
    m_parent->GetVtkDataArrayPlotData()->GetGlyphSource()->FilledOn();
  }
  else
  {
    m_parent->GetVtkDataArrayPlotData()->GetGlyphSource()->FilledOff();
  }

}
//----------------------------------------
void CXYPlotProperty::SetFilledPoint(bool value)
{
  m_filledPoint = value;

  SetFilledPoint();
}

//----------------------------------------
void CXYPlotProperty::SetOpacity(double value)
{
  if (value > 1.0)
  {
    value = 1.0;
  }

  if (value <= 0.0)
  {
    value = 0.6;
  }

  m_opacityFactor = 1 / value;

  m_vtkProperty2D->SetOpacity(value);
}

//----------------------------------------
void CXYPlotProperty::SetName(const wxString& value)
{
  m_name = value;

  if (!HasParent())
  {
    return;
  }
  m_parent->GetVtkDataArrayPlotData()->SetPlotLabel(value.c_str());

}
//----------------------------------------
void CXYPlotProperty::SetXLog(bool value)
{
  m_xLog = value;

  if ((m_xMin <= 0) && (value))
  {
    string msg = CTools::Format("ERROR in CXYPlotProperty::SetXLog : X range [%g, %g] contains value <= 0 - no logarithmic axis possible",
                                 m_xMin, m_xMax);
    CException e(msg, BRATHL_LOGIC_ERROR);
    throw (e);
  }
}
//----------------------------------------
void CXYPlotProperty::SetYLog(bool value)
{
  m_yLog = value;

  if ((m_yMin <= 0) && (value))
  {
    string msg = CTools::Format("ERROR in CXYPlotProperty::SetYLog : Y range [%g, %g] contains value <= 0 - no logarithmic axis possible",
                                 m_yMin, m_yMax);
    CException e(msg, BRATHL_LOGIC_ERROR);
    throw (e);
  }
}

//----------------------------------------
void CXYPlotProperty::SetFocus(bool value)
{

  if ( (m_focus == false) && (value == true) )
  {
    m_focus = value;
    //SetOpacity(GetOpacity() * m_opacityFactor);
    //m_vtkProperty2D->SetOpacity(GetOpacity() * m_opacityFactor);
    SetLineWidth(GetLineWidth() * m_lineWidthFactor);
  }
  else if ( (m_focus == true) && (value == false) )
  {
    m_focus = value;
    //SetOpacity(GetOpacity() / m_opacityFactor);
    //m_vtkProperty2D->SetOpacity(GetOpacity() / m_opacityFactor);
    SetLineWidth(GetLineWidth() / m_lineWidthFactor);
  }

}

//----------------------------------------
void CXYPlotProperty::SetNormalizeX()
{
  if (!HasParent())
  {
    return;
  }
  if (m_normalizeX <= 0)
  {
    return;
  }
  m_parent->NormalizeX(m_normalizeX);

}


//----------------------------------------
void CXYPlotProperty::SetNormalizeX(int32_t value)
{
  m_normalizeX = value;

  SetNormalizeX();
}


//----------------------------------------
void CXYPlotProperty::SetNormalizeY()
{
  if (!HasParent())
  {
    return;
  }

  if (m_normalizeY <= 0)
  {
    return;
  }

  m_parent->NormalizeY(m_normalizeY);

}
//----------------------------------------
void CXYPlotProperty::SetNormalizeY(int32_t value)
{
  m_normalizeY = value;

  SetNormalizeY();

}

//----------------------------------------
CVtkColor CXYPlotProperty::GetColor()
{
  return CVtkColor(m_vtkProperty2D->GetColor()[0],
                   m_vtkProperty2D->GetColor()[1],
                   m_vtkProperty2D->GetColor()[2]);
}
//----------------------------------------
void CXYPlotProperty::GetColor(CVtkColor& vtkColor)
{
  vtkColor = GetColor();
}
//----------------------------------------
void CXYPlotProperty::SetColor(const CVtkColor& vtkColor)
{
  m_vtkProperty2D->SetColor(vtkColor.Red(),
                            vtkColor.Green(),
                            vtkColor.Blue());
}
//----------------------------------------
void CXYPlotProperty::SetColor(const wxColour& color)
{
  CVtkColor vtkColor(color);

  m_vtkProperty2D->SetColor(vtkColor.Red(),
                            vtkColor.Green(),
                            vtkColor.Blue());
}
//----------------------------------------
void CXYPlotProperty::SetColor(wxColourData& colorData, int32_t indexCustomColor)
{
  if (indexCustomColor < 0)
  {
    SetColor(colorData.GetColour());
  }
  else
  {
    SetColor(colorData.GetCustomColour(indexCustomColor));
  }
}

//-------------------------------------------------------------
//------------------- CXYPlotData class --------------------
//-------------------------------------------------------------

CXYPlotData::CXYPlotData(const wxString& name, CXYPlotProperty* plotProperty)
{
  Init();

  if (plotProperty != NULL)
  {
    m_plotProperty = *plotProperty;

  }


  //m_name = name;

}

//----------------------------------------

CXYPlotData::~CXYPlotData()
{

  m_otherVars.RemoveAll();

  if (m_vtkDataArrayPlotData != NULL)
  {
    m_vtkDataArrayPlotData->Delete();
    m_vtkDataArrayPlotData = NULL;
  }

}
//----------------------------------------
/*
CXYPlotData* CXYPlotData::Construct(int32_t dimValSize)
{
  CXYPlotData *plotData = NULL;

  if (dimVal.size() == 2)
  {
    plotData = new CXYPlotDataMulti;
  }
  else if (dimVal.size() == 1)
  {
    plotData = new CXYPlotDataSingle;
  }
  else
  {
    string msg = CTools::Format("CXYPlotData::Construct - Don't know how to create plot data - dimensions size of axis X : %d in file %s",
                                 dimVal.size(),
                                 f->GetName().c_str());
    CProductException e(msg, BRATHL_INCONSISTENCY_ERROR);
    CTrace::Tracer(e.what());
    throw (e);
  }

  return plotData;

}
*/
/*
//----------------------------------------
void CXYPlotData::Create(CObArray* data, CFieldGroup* fieldGroup)
{
  //CExpressionValue* varX = NULL;
  //CExpressionValue* varY = NULL;

  string varXName;

  int32_t iPlot = 0;
  int32_t nrPlots = data->size();

  CUnit unitXRead;
  CUnit unitYRead;

  string unitXStr;
  string unitYStr;

  //CObArray exprValuesX;
  //CObArray exprValuesY;

  for (iPlot = 0 ; iPlot < nrPlots ; iPlot++)
  {
    CExpressionValue varX;
    ExpressionValueDimensions dimValX;
    NetCDFVarKind varKind;
    CStringArray varDimNames;

    CInternalFilesYFX* yfx = dynamic_cast<CInternalFilesYFX*>(data->at(iPlot));
    if (yfx == NULL)
    {
      CException e("CXYPlotData::Create - Non-YFX data found - dynamic_cast<CInternalFilesYFX*>m_internalData.at(0) returns NULL",
                   BRATHL_LOGIC_ERROR);
      CTrace::Tracer(e.what());
      throw (e);

    }
    if (yfx->IsGeographic() == true)
    {
      CException e("CXYPlotData::Create - a YFX data seems to be geographical  - yfx->IsGeographic() is true",
                   BRATHL_LOGIC_ERROR);
      CTrace::Tracer(e.what());
      throw (e);

    }


    // Get X axis data
    GetAxisX(yfx, dimValX, varX, varXName);

    ////////exprValuesX.Insert(varX);

    // Get and control unit of X axis
    unitXRead = yfx->GetUnit(varXName);

    if (iPlot == 0)
    {
      m_name = yfx->GetTitle("").c_str();
      m_unitX = unitXRead;
      unitXStr = m_unitX.AsString();
    }
    else
    {
      string unitXReadStr = unitXRead.AsString();
      if ( unitXStr.compare(unitXReadStr) != 0)
      {
        string msg = CTools::Format("CXYPlotData::Create - In group field number %d, field unit are not in the same way "
                                    "- Expected unit '%s' and found '%s' for axis X",
                                     fieldGroup->m_groupNumber,
                                     unitXStr.c_str(),
                                     unitXReadStr.c_str());
        CException e(msg, BRATHL_INCONSISTENCY_ERROR);
        CTrace::Tracer(e.what());
        throw (e);

      }
    }
    // Get title of plot
    string titlePlot;

    if (m_plotProperty.GetTitle().IsEmpty())
    {
      titlePlot = m_name.c_str();
      m_plotProperty.SetTitle(m_name);
    }

    // Get title of X axis
    string titleX;

    if (m_plotProperty.GetXLabel().IsEmpty())
    {
      titleX = yfx->GetTitle(varXName);
    }

    if (titleX.empty())
    {
      titleX = varXName;
    }

    titleX += "\nUnit:\t" + m_unitX.GetText();
    m_plotProperty.SetXLabel(titleX.c_str());


    int32_t iField = 0;
    int32_t nrFields = fieldGroup->m_fields.size();

    for (iField = 0 ; iField < nrFields ; iField++)
    {
      CExpressionValue varY;
      string fieldName = fieldGroup->m_fields[iField].GetFieldNames()[0];

      // read data dimension
      ExpressionValueDimensions dimValY;

      yfx->GetVarDims(fieldName, dimValY);
      if ( (dimValY.size() <= 0) || (dimValY.size() > 2) )
      {
        string msg = CTools::Format("CXYPlotData::Create - '%s' axis -> number of dimensions  must be 1 or 2 - Found : %d",
                                     fieldName.c_str(), dimValY.size());
        CException e(msg, BRATHL_INCONSISTENCY_ERROR);
        CTrace::Tracer(e.what());
        throw (e);
      }

       // read data
      yfx->ReadVar(fieldName, varY, varKind);

      if (varKind != Data)
      {
        string msg = CTools::Format("CXYPlotData::Create - variable '%s' is not a kind of Data (%d) : %d",
                                     fieldName.c_str(), Data, varKind);
        CException e(msg, BRATHL_INCONSISTENCY_ERROR);
        CTrace::Tracer(e.what());
        throw (e);
      }


      ////////////exprValuesY.Insert(varY);

      // Get data unit
      unitYRead = yfx->GetUnit(fieldName);

      if ( (iPlot == 0) && (iField == 0) )
      {
        m_unitY = unitYRead;
        unitYStr = m_unitY.AsString();
      }
      else
      {
        string unitYReadStr = unitYRead.AsString();
        if ( unitYStr.compare(unitYReadStr) != 0)
        {
          string msg = CTools::Format("CXYPlotData::Create - In group field number %d, field unit are not in the same way "
                                      "- Expected unit '%s' and found '%s' for axis X",
                                       fieldGroup->m_groupNumber,
                                       unitYStr.c_str(),
                                       unitYReadStr.c_str());
          CException e(msg, BRATHL_INCONSISTENCY_ERROR);
          CTrace::Tracer(e.what());
          throw (e);

        }
      }
      // Get title of Y axis (as possible)
      string titleY;

      if (nrFields == 1)
      {
        if (m_plotProperty.GetYLabel().IsEmpty())
        {
          titleY = yfx->GetTitle(fieldName);
        }

        if (titleY.empty())
        {
          titleY = fieldName;
        }
      }


      if ( (iPlot == 0) && (iField == 0) )
      {
        titleY += "\nUnit:\t" + m_unitY.GetText();
        titleY = CTools::StringReplace(titleY, ' ', '\n');
        m_plotProperty.SetYLabel(titleY.c_str());
      }


      SetData(varX.GetValues(), dimValX, varY.GetValues(), dimValY);
    }


  }


}
*/
/*
//----------------------------------------
void CXYPlotData::Create(CObArray* data, CPlot* plot, int32_t iField)
{
  string varXName;

  int32_t iFile = 0;
  int32_t nrFiles = data->size();

  CUnit unitXRead;
  CUnit unitYRead;

  string unitXStr;
  string unitYStr;

  if (m_plotProperty.GetTitle().IsEmpty())
  {
    m_plotProperty.SetTitle(plot->m_title);
  }

  if (m_plotProperty.GetXLabel().IsEmpty())
  {
    m_plotProperty.SetXLabel(plot->m_titleX);
  }
  else
  {
    wxString titleX = m_plotProperty.GetXLabel();
    titleX += plot->m_unitXLabel;
    m_plotProperty.SetXLabel(titleX);
  }

  if (m_plotProperty.GetYLabel().IsEmpty())
  {
    m_plotProperty.SetYLabel(plot->m_titleY);
  }
  else
  {
    wxString titleY = m_plotProperty.GetYLabel();
    titleY += plot->m_unitYLabel;
    m_plotProperty.SetYLabel(titleY);
  }

  m_unitX = plot->m_unitX;
  m_unitY = plot->m_unitY;

  for (iFile = 0 ; iFile < nrFiles ; iFile++)
  {
    CExpressionValue varX;
    ExpressionValueDimensions dimValX;
    NetCDFVarKind varKind;
    CStringArray varDimNames;

    CInternalFilesYFX* yfx = plot->GetInternalFilesYFX(data->at(iFile));

    // Get X axis data
    plot->GetAxisX(yfx, &dimValX, &varX, &varXName);

    // X units are compatible but not the same --> convert
    unitXRead = yfx->GetUnit(varXName);
    if (m_unitX.AsString() != unitXRead.AsString())
    {
      plot->m_unitX.SetConversionFrom(unitXRead);
      plot->m_unitX.ConvertVector(varX.GetValues(), varX.GetNbValues());
    }

    // Get and control unit of X axis
    unitXRead = yfx->GetUnit(varXName);

    CExpressionValue varY;
    string fieldName = plot->GetPlotField(iField)->m_name.c_str();

    // Set name of the data
    if (iFile == 0)
    {
      if (GetName().IsEmpty())
      {
        string str = yfx->GetTitle(fieldName) + " (" + fieldName + ")";
        SetName(str.c_str());
      }
    }


    // Read Y data dimension
    ExpressionValueDimensions dimValY;

    yfx->GetVarDims(fieldName, dimValY);
    if ( (dimValY.size() <= 0) || (dimValY.size() > 2) )
    {
      string msg = CTools::Format("CXYPlotData::Create - '%s' axis -> number of dimensions  must be 1 or 2 - Found : %d",
                                   fieldName.c_str(), dimValY.size());
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      CTrace::Tracer(e.what());
      throw (e);
    }

     // Read Y data
    yfx->ReadVar(fieldName, varY);
    varKind	= yfx->GetVarKind(fieldName);

    if (varKind != Data)
    {
      string msg = CTools::Format("CXYPlotData::Create - variable '%s' is not a kind of Data (%d) : %s",
                                   fieldName.c_str(), Data, CNetCDFFiles::VarKindToString(varKind).c_str());
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      CTrace::Tracer(e.what());
      throw (e);
    }

    // Y units are compatible but not the same --> convert
     unitYRead = yfx->GetUnit(fieldName);
    if (m_unitY.AsString() != unitYRead.AsString())
    {
      plot->m_unitY.SetConversionFrom(unitYRead);
      plot->m_unitY.ConvertVector(varY.GetValues(), varY.GetNbValues());
    }



    SetData(varX.GetValues(), dimValX, varY.GetValues(), dimValY);

    m_plotProperty.Update();

  }


}
*/
//----------------------------------------
void CXYPlotData::Create(CInternalFiles* yfx, CPlot* plot, int32_t iField)
{
  string varXName;

  CUnit unitXRead;
  CUnit unitYRead;

  string unitXStr;
  string unitYStr;

  if (m_plotProperty.GetTitle().IsEmpty())
  {
    m_plotProperty.SetTitle(plot->m_title);
  }

  if (m_plotProperty.GetXLabel().IsEmpty())
  {
    m_plotProperty.SetXLabel(plot->m_titleX);
  }
  else
  {
    wxString titleX = m_plotProperty.GetXLabel();
    titleX += plot->m_unitXLabel;
    m_plotProperty.SetXLabel(titleX);
  }

  if (m_plotProperty.GetYLabel().IsEmpty())
  {
    m_plotProperty.SetYLabel(plot->m_titleY);
  }
  else
  {
    wxString titleY = m_plotProperty.GetYLabel();
    titleY += plot->m_unitYLabel;
    m_plotProperty.SetYLabel(titleY);
  }

  m_unitX = plot->m_unitX;
  m_unitY = plot->m_unitY;

  CExpressionValue varX;

  ExpressionValueDimensions dimValX;

  NetCDFVarKind varKind;
  CStringArray varDimNames;


  // Get X axis data
  plot->GetAxisX(yfx, &dimValX, &varX, &varXName);

  // Get and control unit of X axis
  // X units are compatible but not the same --> convert
  unitXRead = yfx->GetUnit(varXName);
  if (m_unitX.AsString() != unitXRead.AsString())
  {
    plot->m_unitX.SetConversionFrom(unitXRead);
    plot->m_unitX.ConvertVector(varX.GetValues(), varX.GetNbValues());
  }

  CExpressionValue varY;
  string fieldName = (const char *)(plot->GetPlotField(iField)->m_name);

  // Set name of the data

  if (GetName().IsEmpty())
  {
    string str = yfx->GetTitle(fieldName) + " (" + fieldName + ")";
    SetName(str.c_str());
  }

  // Read Y data dimension
  ExpressionValueDimensions dimValY;

  yfx->GetVarDims(fieldName, dimValY);
  if ( (dimValY.size() <= 0) || (dimValY.size() > 2) )
  {
    string msg = CTools::Format("CXYPlotData::Create - '%s' axis -> number of dimensions  must be 1 or 2 - Found : %ld",
                                 fieldName.c_str(), (long)dimValY.size());
    CException e(msg, BRATHL_INCONSISTENCY_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);
  }

   // Read Y data
  unitYRead = yfx->GetUnit(fieldName);
  yfx->ReadVar(fieldName, varY, unitYRead.GetText());
  varKind	= yfx->GetVarKind(fieldName);
  /************************DEDEDE
  if (varKind != Data)
  {
    string msg = CTools::Format("CXYPlotData::Create - variable '%s' is not a kind of Data (%d) : %s",
                                 fieldName.c_str(), Data, CNetCDFFiles::VarKindToString(varKind).c_str());
    CException e(msg, BRATHL_INCONSISTENCY_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);
  }
*/
  // Y units are compatible but not the same --> convert
  if (m_unitY.AsString() != unitYRead.AsString())
  {
    plot->m_unitY.SetConversionFrom(unitYRead);
    plot->m_unitY.ConvertVector(varY.GetValues(), varY.GetNbValues());
  }

  CStringArray commonDims;
  bool intersect = yfx->GetCommonVarDims(varXName, fieldName, commonDims);

  if (!intersect)
  {
    string msg = CTools::Format("CXYPlotData::Create - variables '%s' and '%s' have no common dimension",
                                 fieldName.c_str(), varXName.c_str());
    CException e(msg, BRATHL_INCONSISTENCY_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);

  }

  CStringArray dimXNames;
  CStringArray dimYNames;
  yfx->GetVarDims(varXName, dimXNames);
  yfx->GetVarDims(fieldName, dimYNames);

  if ((commonDims.size() == dimXNames.size()) && (commonDims.size() == dimYNames.size()) && (dimXNames != dimYNames))
  {
    string msg = CTools::Format("CXYPlotData::Create - variables '%s' (dim %s) and '%s' (dim %s) must have the same dimension fields",
                                 fieldName.c_str(), dimYNames.ToString().c_str(), varXName.c_str(), dimXNames.ToString().c_str());
    CException e(msg, BRATHL_INCONSISTENCY_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);

  }


  CUIntArray xCommonDimIndex;
  CUIntArray yCommonDimIndex;

  for (uint32_t iIntersect = 0 ; iIntersect < commonDims.size() ; iIntersect++)
  {
    int32_t dimIndex = -1;

    dimIndex = yfx->GetVarDimIndex(varXName, commonDims.at(iIntersect));
    if (dimIndex < 0)
    {
      string msg = CTools::Format("CXYPlotData::Create - variables '%s' - dim '%s' not found ",
                                   varXName.c_str(), commonDims.at(iIntersect).c_str());
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
    }

    xCommonDimIndex.Insert(dimIndex);

    dimIndex = yfx->GetVarDimIndex(fieldName, commonDims.at(iIntersect));
    if (dimIndex < 0)
    {
      string msg = CTools::Format("CXYPlotData::Create - variables '%s' - dim '%s' not found ",
                                   fieldName.c_str(), commonDims.at(iIntersect).c_str());
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
    }

    yCommonDimIndex.Insert(dimIndex);
  }


  CStringArray complementDims;
  bool complement = false;

  if (dimXNames.size() < dimYNames.size())
  {
    complement = yfx->GetComplementVarDims(varXName, fieldName, complementDims);
  }
  else
  {
    complement = yfx->GetComplementVarDims(fieldName, varXName, complementDims);
  }


  CExternalFilesNetCDF* externalFile = dynamic_cast<CExternalFilesNetCDF*> (BuildExistingExternalFileKind(yfx->GetName()));

  if (externalFile != NULL)
  {
    externalFile->Open();
  }

  if ((complement) && (externalFile != NULL))
  {
    // Read 'Complement dim' var data
    CFieldNetCdf* field = externalFile->GetFieldNetCdf(complementDims.at(0), false);
    if (field != NULL)
    {
      m_fieldComplement = *field;
      CUnit unit = yfx->GetUnit(field->GetName());

      yfx->ReadVar(field->GetName(), m_varComplement, unit.GetText());
    }
  }


  if (externalFile != NULL)
  {
    // Read other var data (non plot data)
    //CStringArray::const_iterator itStringArray;
    uint32_t iOtherVars = 0;

    m_otherVars.RemoveAll();
    //m_otherVars.resize(plot->m_nonPlotFieldNames.size());


    for (iOtherVars = 0 ; iOtherVars < plot->m_nonPlotFieldNames.size(); iOtherVars++)
    {

      CFieldNetCdf* field = externalFile->GetFieldNetCdf(plot->m_nonPlotFieldNames.at(iOtherVars), false);
      if (field != NULL)
      {
        m_otherFields.Insert(new CFieldNetCdf(*field));

        CUnit unit = yfx->GetUnit(field->GetName());

        CExpressionValue* exprValue = new CExpressionValue();

        //yfx->ReadVar(field->GetName(), m_otherVars[iOtherVars], unit.GetText());
        yfx->ReadVar(field->GetName(), *exprValue, unit.GetText());

        m_otherVars.Insert(exprValue);
      }

    }

  }

  //m_otherFields.Dump(*(CTrace::GetInstance()->GetDumpContext()));


  if (externalFile != NULL)
  {
    externalFile->Close();
    delete externalFile;
    externalFile = NULL;
  }



  SetData(varX.GetValues(), dimValX, varY.GetValues(), dimValY, xCommonDimIndex, yCommonDimIndex);

  m_plotProperty.SetParent(this);
  m_plotProperty.Update();


}
//----------------------------------------
void CXYPlotData::Init()
{
  m_plotProperty.SetParent(this);

  m_vtkDataArrayPlotData = vtkDataArrayPlotData::New();

  m_currentFrame = 0;
  m_length = 0;

  m_aborted = false;


}

/*
//----------------------------------------
void CXYPlotData::GetAxisX(CInternalFilesYFX* yfx,
                           ExpressionValueDimensions& dimVal,
                           CExpressionValue& varX,
                           string& varXName)
{
  CStringArray axisNames;
  CStringArray::iterator is;
  //ExpressionValueDimensions dimVal;

  yfx->GetAxisVars(axisNames);

  if (axisNames.size() != 1)
  {
    string msg = CTools::Format("CXYPlotData::GetAxisX -  wrong number of axis in file '%s' : %d"
                                "Correct number is 1",
                                yfx->GetName().c_str(),
                                axisNames.size());
    CException e(msg, BRATHL_INCONSISTENCY_ERROR);
    CTrace::Tracer(e.what());
    throw (e);
  }

  for (is = axisNames.begin(); is != axisNames.end(); is++)
  {
    yfx->GetVarDims(*is, dimVal);
    if ( (dimVal.size() <= 0) || (dimVal.size() > 2) )
    {
      string msg = CTools::Format("CXYPlotData::GetAxisX - '%s' axis -> number of dimensions must be 1 or 2 - Found : %d",
                                   (*is).c_str(), dimVal.size());
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      CTrace::Tracer(e.what());
      throw (e);
    }
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

    varXName = (*is);
    yfx->ReadVar(*is, varX, varKind, wantedUnit);
  }

}
*/

//----------------------------------------
void CXYPlotData::OnFrameChange(int32_t f)
{

  m_currentFrame = (f < m_length - 1) ? f : m_length - 1;
  m_currentFrame = (m_currentFrame > 0) ? m_currentFrame : 0;

  Update();

}
//----------------------------------------
void CXYPlotData::SetRawData(CPlotArray* x, CPlotArray* y)
{

  if ( x->GetNumberOfFrames() != y->GetNumberOfFrames() )
  {
    string msg = CTools::Format("ERROR in CXYPlotData::SetRawData : Number of frames in PlotArray must be the same for all data - "
                                "nb frames for x : %d - nb frames for y : %d",
                                x->GetNumberOfFrames(), y->GetNumberOfFrames());
    CException e(msg, BRATHL_LOGIC_ERROR);
    throw (e);

  }

  m_rawData.Insert(x);
  m_rawData.Insert(y);
  m_length = x->GetNumberOfFrames();

  Update();

}
//----------------------------------------
CPlotArray* CXYPlotData::GetRawData(int32_t i)
{
  CPlotArray* data = dynamic_cast<CPlotArray*>(m_rawData.at(i));
  if (data == NULL)
  {
    throw CException("ERROR in  CXYPlotData::GetRawData : dynamic_cast<CPlotArray*>m_rawData.at(i);; returns NULL pointer - "
                 "XYPlotData  seems to contain objects other than those of the class CPlotArray or derived class", BRATHL_LOGIC_ERROR);
  }
  return data;

}

//----------------------------------------
void CXYPlotData::GetXRange(double& min, double& max, uint32_t frame)
{
  GetRawData(0)->GetRange(min, max, frame);
}
//----------------------------------------
void CXYPlotData::GetYRange(double& min, double& max, uint32_t frame)
{
  GetRawData(1)->GetRange(min, max, frame);
}
//----------------------------------------
void CXYPlotData::GetXRange(double& min, double& max)
{
  GetRawData(0)->GetRange(min, max);
}
//----------------------------------------
void CXYPlotData::GetYRange(double& min, double& max)
{
  GetRawData(1)->GetRange(min, max);
}

//----------------------------------------
int32_t CXYPlotData::GetNumberOfFrames()
{
  return GetRawData(0)->GetNumberOfFrames();
}
//----------------------------------------
void CXYPlotData::NormalizeX(int32_t value)
{
  GetRawData(0)->Normalize(value);
}
//----------------------------------------
void CXYPlotData::NormalizeY(int32_t value)
{
  GetRawData(1)->Normalize(value);
}
//----------------------------------------
void CXYPlotData::Normalize(int32_t value)
{
  NormalizeX(value);
  NormalizeY(value);
}

//----------------------------------------
void CXYPlotData::Update()
{
  m_vtkDataArrayPlotData->SetXDataArray(GetRawData(0)->GetFrameData(m_currentFrame));
  m_vtkDataArrayPlotData->SetYDataArray(GetRawData(1)->GetFrameData(m_currentFrame));
}

//-------------------------------------------------------------
//------------------- CXYPlotDataSingle class --------------------
//-------------------------------------------------------------
/*
CXYPlotDataSingle::CXYPlotDataSingle(const wxString& name)
                : CXYPlotData(name)
{

}
//----------------------------------------
CXYPlotDataSingle::~CXYPlotDataSingle()
{

}
//----------------------------------------
void CXYPlotDataSingle::SetData(double* xArray, const vector<uint32_t>& xDims,  double* yArray, const vector<uint32_t>& yDims, bool bCopy)
{
  SetData(xArray, xDims[0], yArray, yDims[0], bCopy);
}
//----------------------------------------
void CXYPlotDataSingle::SetData(double* xArray, const CUIntArray& xDims,  double* yArray, const CUIntArray& yDims, bool bCopy)
{
  SetData(xArray, xDims[0], yArray, yDims[0], bCopy);
}

//----------------------------------------
void CXYPlotDataSingle::SetData(double* xArray, int32_t xSize,  double* yArray, int32_t ySize, bool bCopy)
{
  if (GetName().IsEmpty())
  {
    SetName("Dataset");
  }
  double* newX = NULL;
  double* newY = NULL;
  if (bCopy)
  {
    newX = new double[xSize];
    memcpy(newX, xArray, xSize * sizeof(*xArray));
    newY = new double[ySize];
    memcpy(newY, yArray, ySize * sizeof(*yArray));
  }
  else
  {
    newX = xArray;
    newY = yArray;
  }
  vtkDoubleArrayBrathl* vtkXArray = vtkDoubleArrayBrathl::New();
  vtkDoubleArrayBrathl* vtkYArray = vtkDoubleArrayBrathl::New();
  // This method lets the user specify data to be held by the array.
  // The array argument is a pointer to the data.
  // size is the size of the array supplied by the user.
  // Set save to 1 to keep the class from deleting the array when it cleans up or reallocates memory.
  // The class uses the actual array provided; it does not copy the data from the suppled array
  vtkXArray->SetArray(newX, xSize, 0);
  vtkYArray->SetArray(newY, ySize, 0);

  SetData(vtkXArray, vtkYArray);

}
//----------------------------------------
void CXYPlotDataSingle::SetData(vtkDoubleArrayBrathl* xArray, vtkDoubleArrayBrathl* yArray)
{

  if (GetName().IsEmpty())
  {
    SetName("Dataset");
  }

  CPlotArray* xPlotArray = new CPlotArray();
  xPlotArray->SetFrameData(0, xArray);

  CPlotArray* yPlotArray = new CPlotArray();
  yPlotArray->SetFrameData(0, yArray);

  SetRawData(xPlotArray, yPlotArray);
}
*/
//-------------------------------------------------------------
//------------------- CXYPlotDataMulti class --------------------
//-------------------------------------------------------------

CXYPlotDataMulti::CXYPlotDataMulti(const wxString& name, CXYPlotProperty* plotProperty)
              : CXYPlotData(name, plotProperty)
{

}
//----------------------------------------
CXYPlotDataMulti::~CXYPlotDataMulti()
{

}
/*
//----------------------------------------
void CXYPlotDataMulti::SetData(double* xArray, const vector<uint32_t>& xDims,  double* yArray, const vector<uint32_t>& yDims, bool bCopy)
{
  CUIntArray dimsX;
  dimsX.Insert(xDims);
  CUIntArray dimsY;
  dimsY.Insert(yDims);

  SetData(xArray, dimsX, yArray, dimsY, bCopy);
}
*/
//----------------------------------------
void CXYPlotDataMulti::SetData(double* xArray, const CUIntArray& xDims,  double* yArray, const CUIntArray& yDims,
                               const CUIntArray& xCommonDimIndex,
                               const CUIntArray& yCommonDimIndex,
                               bool bCopy)
{
  if (GetName().IsEmpty())
  {
    SetName("Dataset");
  }

  CUIntArray xDimIndex;
  CUIntArray yDimIndex;

  uint32_t iDimIndex = 0;

  for (iDimIndex = 0 ; iDimIndex  < xDims.size() ; iDimIndex++)
  {
    xDimIndex.Insert(iDimIndex);
  }

  for (iDimIndex = 0 ; iDimIndex  < yDims.size() ; iDimIndex++)
  {
    yDimIndex.Insert(iDimIndex);
  }

  double* newX = NULL;
  double* newY = NULL;

  if ( (xDims.size() <= 0) || (yDims.size() <= 0) )
  {
    string msg = CTools::Format("ERROR in CXYPlotDataMulti::SetData : At least one of Arrays is 0 dimensional  -"
                                "xArray dim. is %ld and yArray dim. is %ld",
                                (long)xDims.size(), (long)yDims.size());
    CException e(msg, BRATHL_INCONSISTENCY_ERROR);
    throw (e);

  }

  if ( (xCommonDimIndex.size() <= 0) || (yCommonDimIndex.size() <= 0) )
  {
    string msg = CTools::Format("ERROR in CXYPlotDataMulti::SetData : No common dimension have been set."
                                "xArray dim. is %ld and yArray dim. is %ld - "
                                "xCommonDimIndex dim. is %ld and yCommonDimIndex dim. is %ld",
                                (long)xDims.size(), (long)yDims.size(),
                                (long)xCommonDimIndex.size(), (long)yCommonDimIndex.size());
    CException e(msg, BRATHL_INCONSISTENCY_ERROR);
    throw (e);

  }
/*
  if ( xDims.at(xDims.size() - 1) != yDims.at(yDims.size() - 1) )
  {
    string msg = CTools::Format("ERROR in CXYPlotDataMulti::SetData : Array dimensions don't match  -"
                                "xArray dim. is %d  xArray last dim. value is %d"
                                "yArray dim. is %d  yArray last dim. value is %d",
                                xDims.size(), xDims.at(xDims.size() - 1),
                                yDims.size(), yDims.at(yDims.size() - 1));
    CException e(msg, BRATHL_INCONSISTENCY_ERROR);
    throw (e);

  }
*/

  if ( (xDims.size() > 2) || (yDims.size() > 2) )
  {
    string msg = CTools::Format("ERROR in CXYPlotDataMulti::SetData : Arrays must be 2 dimensional or less -"
                                "xArray dim. is %ld and yArray dim. is %ld",
                                (long)xDims.size(), (long)yDims.size());
    CException e(msg, BRATHL_INCONSISTENCY_ERROR);
    throw (e);

  }

  if ( (xCommonDimIndex.size() > 2) || (yCommonDimIndex.size() > 2) )
  {
    string msg = CTools::Format("ERROR in CXYPlotDataMulti::SetData : Common dimension arrays must be 2 dimensional or less -"
                                "xCommonDimIndex dim. is %ld and yCommonDimIndex dim. is %ld",
                                (long)xCommonDimIndex.size(), (long)yCommonDimIndex.size());
    CException e(msg, BRATHL_INCONSISTENCY_ERROR);
    throw (e);


  }

  if ( xCommonDimIndex.size() != yCommonDimIndex.size() )
  {
    string msg = CTools::Format("ERROR in CXYPlotDataMulti::SetData : Common dimension arrays have not the same dimension -"
                                "xCommonDimIndex dim. is %ld and yCommonDimIndex dim. is %ld",
                                (long)xCommonDimIndex.size(), (long)yCommonDimIndex.size());
    CException e(msg, BRATHL_INCONSISTENCY_ERROR);
    throw (e);

  }

  /*
  CUIntArray intersect;

  if (xCommonDimIndex.Intersect(yCommonDimIndex, intersect) == false)
  {
    string msg = CTools::Format("ERROR in CXYPlotDataMulti::SetData : Common dimension arrays have no intersection -"
                                "xCommonDimIndex dim. is %d, values are %s and yCommonDimIndex dim. is %d, values are %s",
                                xCommonDimIndex.size(), xCommonDimIndex.ToString().c_str(), yCommonDimIndex.size(), xCommonDimIndex.ToString().c_str());
    CException e(msg, BRATHL_INCONSISTENCY_ERROR);
    throw (e);

  }
  */
/*
  int32_t commonDim = -1;

  for (uint32_t iXDim = 0 ; iXDim < xDims.size() ; iXDim++)
  {

    for (uint32_t iYDim = 0 ; iYDim < yDims.size() ; iYDim++)
    {
      if (xDims.at(iXDim) == yDims.at(iYDim))
      {
        commonDim = iXDim;
        break;
      }
    }

    if (commonDim >= 0)
    {
      break;
    }
  }

  if ( commonDim < 0 )
  {

    string msg = CTools::Format("ERROR in CXYPlotDataMulti::SetData : Array dimensions don't match  -"
                                "xArray dim. is %d  xArray dim. values are %s"
                                "yArray dim. is %d  yArray dim. values are %s",
                                xDims.size(), (xDims.ToString()).c_str(),
                                yDims.size(), yDims.ToString().c_str());
    CException e(msg, BRATHL_INCONSISTENCY_ERROR);
    throw (e);

  }

  int32_t maxFrames = 0;


  if ((xDims.size() == 1) && (yDims.size() == 1))
  {
    maxFrames = 1;
  }
  else if (xDims.size() == 1)
  {
    maxFrames = yDims.at(0);
  }
  else if (yDims.size() == 1)
  {
    maxFrames = xDims.at(0);
  }
  else
  {
    maxFrames = (xDims.at(0) > yDims.at(0)) ? yDims.at(0) : xDims.at(0);
  }
*/

  int32_t maxFrames = 0;
  bool hasComplement = false;
  CUIntArray complementX;
  CUIntArray complementY;

  if (xCommonDimIndex.size() == 1)
  {
    if ((xDims.size() == 1) && (yDims.size() == 1))
    {
      maxFrames = 1;
    }
    else if (xDims.size() == 1)
    {
      hasComplement = yCommonDimIndex.Complement(yDimIndex, complementX);
      if (!hasComplement)
      {
        string msg = CTools::Format("ERROR in CXYPlotDataMulti::SetData : No X complement have been found:"
                                "xCommonDimIndex dim. is %ld, values are %s and yCommonDimIndex dim. is %ld, values are %s -"
                                "xDimIndex dim. is %ld, values are %s and yDimIndex dim. is %ld, values are %s",
                                (long)xCommonDimIndex.size(), xCommonDimIndex.ToString().c_str(),
                                (long)yCommonDimIndex.size(), xCommonDimIndex.ToString().c_str(),
                                (long)xDimIndex.size(), xDimIndex.ToString().c_str(),
                                (long)yDimIndex.size(), yDimIndex.ToString().c_str()
                                );
        CException e(msg, BRATHL_INCONSISTENCY_ERROR);
        throw (e);
      }
      maxFrames = yDims.at(complementX.at(0));
    }
    else if (yDims.size() == 1)
    {
      hasComplement = xCommonDimIndex.Complement(xDimIndex, complementY);
      if (!hasComplement)
      {
        string msg = CTools::Format("ERROR in CXYPlotDataMulti::SetData : No Y complement have been found:"
                                "xCommonDimIndex dim. is %ld, values are %s and yCommonDimIndex dim. is %ld, values are %s"
                                "xDimIndex dim. is %ld, values are %s and yDimIndex dim. is %ld, values are %s",
                                (long)xCommonDimIndex.size(), xCommonDimIndex.ToString().c_str(),
                                (long)yCommonDimIndex.size(), xCommonDimIndex.ToString().c_str(),
                                (long)xDimIndex.size(), xDimIndex.ToString().c_str(),
                                (long)yDimIndex.size(), yDimIndex.ToString().c_str()
                                );
        CException e(msg, BRATHL_INCONSISTENCY_ERROR);
        throw (e);
      }
      maxFrames = xDims.at(complementY.at(0));
    }
    else
    {
      string msg = CTools::Format("ERROR in CXYPlotDataMulti::SetData : X and Y dimension must be equals:"
                                "xCommonDimIndex dim. is %ld, values are %s and yCommonDimIndex dim. is %ld, values are %s"
                                "xDimIndex dim. is %ld, values are %s and yDimIndex dim. is %ld, values are %s",
                                (long)xCommonDimIndex.size(), xCommonDimIndex.ToString().c_str(),
                                (long)yCommonDimIndex.size(), xCommonDimIndex.ToString().c_str(),
                                (long)xDimIndex.size(), xDimIndex.ToString().c_str(),
                                (long)yDimIndex.size(), yDimIndex.ToString().c_str()
                                );
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      throw (e);
    }
  }
  else
  {
    hasComplement = yCommonDimIndex.Complement(yDimIndex, complementX);
    hasComplement |= xCommonDimIndex.Complement(xDimIndex, complementY);
    if (hasComplement)
    {
      string msg = CTools::Format("ERROR in CXYPlotDataMulti::SetData : X and Y must have the same dimension fields:"
                                "xCommonDimIndex dim. is %ld, values are %s and yCommonDimIndex dim. is %ld, values are %s"
                                "xDimIndex dim. is %ld, values are %s and yDimIndex dim. is %ld, values are %s",
                                (long)xCommonDimIndex.size(), xCommonDimIndex.ToString().c_str(),
                                (long)yCommonDimIndex.size(), xCommonDimIndex.ToString().c_str(),
                                (long)xDimIndex.size(), xDimIndex.ToString().c_str(),
                                (long)yDimIndex.size(), yDimIndex.ToString().c_str()
                                );
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      throw (e);
    }
    else if (xCommonDimIndex != yCommonDimIndex)
    {
      string msg = CTools::Format("ERROR in CXYPlotDataMulti::SetData : X and Y dimension must be equals:"
                                "xCommonDimIndex dim. is %ld, values are %s and yCommonDimIndex dim. is %ld, values are %s"
                                "xDimIndex dim. is %ld, values are %s and yDimIndex dim. is %ld, values are %s",
                                (long)xCommonDimIndex.size(), xCommonDimIndex.ToString().c_str(),
                                (long)yCommonDimIndex.size(), xCommonDimIndex.ToString().c_str(),
                                (long)xDimIndex.size(), xDimIndex.ToString().c_str(),
                                (long)yDimIndex.size(), yDimIndex.ToString().c_str()
                                );
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      throw (e);
    }

    maxFrames = 1;

  }


  wxProgressDialog* pd = new wxProgressDialog("Calculating Frames...",
                                               "",
                                               maxFrames, NULL,
                                               wxPD_CAN_ABORT | wxPD_REMAINING_TIME | wxPD_APP_MODAL);
  pd->SetSize(-1,-1, 350, -1);

  CPlotArray* xPlotArray = NULL;
  CPlotArray* yPlotArray = NULL;

  if (m_rawData.size() == 0)
  {
    xPlotArray = new CPlotArray();
    yPlotArray = new CPlotArray();
  }
  else
  {
    xPlotArray = GetRawData(0);
    yPlotArray = GetRawData(1);
  }


  int32_t i = 0;

  for (i = 0; i < maxFrames ; i++)
  {
    int32_t xDim = -1;
    int32_t yDim = -1;

    string msg = CTools::Format("Calculating Frame: %d of %d", i + 1, maxFrames);
    if (pd->Update(i, msg.c_str()) == false)
    {
      m_aborted = true;
      break;
    }

    if (xDims.size() == 1)
    {
      xDim = xDims[0];
      //newX = new double[xDim];
      newX = (double*) malloc(xDim * sizeof(*newX));
      memcpy(newX, xArray, xDim * sizeof(*xArray));
    }
    else
    {
      if (xCommonDimIndex.size() == 1)
      {
        xDim = xDims[xCommonDimIndex.at(0)];
        //newX = new double[xDim];
        newX = (double*) malloc(xDim * sizeof(*newX));
        if (xCommonDimIndex.at(0) == 1)
        {
          memcpy(newX, xArray + (i * xDim) , xDim * sizeof(*xArray));
        }
        else
        {
          for (int32_t j = 0 ; j < xDim ; j++)
          {
            newX[j] = xArray[i + (j * maxFrames)];
          }
        }
      }
      else
      {
        xDim = xDims[xCommonDimIndex.at(0)] * xDims[xCommonDimIndex.at(1)] ;
        //newX = new double[xDim];
        newX = (double*) malloc(xDim * sizeof(*newX));
        memcpy(newX, xArray + (i * xDim) , xDim * sizeof(*xArray));
      }

    }

    if (yDims.size() == 1)
    {
      yDim = yDims[0];
      //newY = new double[yDim];
      newY = (double*) malloc(yDim * sizeof(*newY));
      memcpy(newY, yArray, yDim * sizeof(*yArray));
    }
    else
    {
      if (yCommonDimIndex.size() == 1)
      {
        yDim = yDims[yCommonDimIndex.at(0)];
        //newY = new double[yDim];
        newY = (double*) malloc(yDim * sizeof(*newY));
        if (yCommonDimIndex.at(0) == 1)
        {
          memcpy(newY, yArray + (i * yDim) , yDim * sizeof(*yArray));
        }
        else
        {
          for (int32_t j = 0 ; j < yDim ; j++)
          {
            newY[j] = yArray[i +  (j * maxFrames)];
          }
        }
      }
      else
      {
        yDim = yDims[yCommonDimIndex.at(0)] * yDims[yCommonDimIndex.at(1)] ;
        //newY = new double[yDim];
        newY = (double*) malloc(yDim * sizeof(*newY));
        memcpy(newY, yArray + (i * yDim) , yDim * sizeof(*yArray));
      }
    }

    xPlotArray->SetFrameData(i + xPlotArray->GetNumberOfFrames(), newX, xDim, false);
    yPlotArray->SetFrameData(i + yPlotArray->GetNumberOfFrames(), newY, yDim, false);

  }

  pd->Destroy();

  if (m_aborted)
  {
    return;
  }

  SetRawData(xPlotArray, yPlotArray);


}
//-------------------------------------------------------------
//------------------- CXYPlotDataCollection class --------------------
//-------------------------------------------------------------

CXYPlotDataCollection::CXYPlotDataCollection()
{
  Init();
}

//----------------------------------------

CXYPlotDataCollection::~CXYPlotDataCollection()
{

}
//----------------------------------------
void CXYPlotDataCollection::Init()
{
  m_zoomToDataRange = false;
  m_currentFrame = 0;
}
//----------------------------------------
CXYPlotData* CXYPlotDataCollection::Get(CObArray::iterator it )
{
  CXYPlotData* data = dynamic_cast<CXYPlotData*>(*it);
  if (data == NULL)
  {
    throw CException("ERROR in  CXYPlotDataCollection::Get : dynamic_cast<CXYPlotData*>(*it); returns NULL pointer - "
                 "XYPlotData Collection seems to contain objects other than those of the class CXYPlotData or derived class", BRATHL_LOGIC_ERROR);
  }
  return data;
}

//----------------------------------------
CXYPlotData* CXYPlotDataCollection::Get(int32_t index)
{
  if ( (index < 0) || (static_cast<uint32_t>(index) >= this->size()) )
  {
    string msg = CTools::Format("ERROR in  CXYPlotDataCollection::Get : index %d out-of-range "
                                "Valid range is [0, %ld]",
                                index,
                                (long)this->size());
    throw CException(msg, BRATHL_LOGIC_ERROR);

  }

  CXYPlotData* data = dynamic_cast<CXYPlotData*>(this->at(index));
  if (data == NULL)
  {
    throw CException("ERROR in  CXYPlotDataCollection::Get : dynamic_cast<CXYPlotData*>(this->at(index)); returns NULL pointer - "
                 "XYPlotData Collection seems to contain objects other than those of the class CXYPlotData or derived class", BRATHL_LOGIC_ERROR);
  }
  return data;
}
//----------------------------------------
CXYPlotProperty* CXYPlotDataCollection::GetPlotProperty(int32_t index)
{
  CXYPlotData* data = Get(index);

  return data->GetPlotProperty();
}

//----------------------------------------
void CXYPlotDataCollection::OnFrameChange(int32_t f)
{
  CObArray::iterator it;

  for (it = begin(); it != end() ; it++)
  {
    Get(it)->OnFrameChange(f);
  }

  m_currentFrame = f;


}

//----------------------------------------
void CXYPlotDataCollection::GetXRange(double& min, double& max, uint32_t frame)
{
  CObArray::iterator it;
  CTools::SetDefaultValue(min);
  CTools::SetDefaultValue(max);
  for (it = begin(); it != end() ; it++)
  {
    CXYPlotData* data = Get(it);
    if (data == NULL)
    {
      continue;
    }
    double l = 0;
    double h = 0;

    CXYPlotProperty* props = data->GetPlotProperty();

    data->GetXRange(l, h, frame);

    if (CTools::IsDefaultValue(props->GetXMin()) == false)
    {
      l = props->GetXMin();
    }
    if (CTools::IsDefaultValue(props->GetXMax()) == false)
    {
      h = props->GetXMax();
    }

    if (CTools::IsDefaultValue(min))
    {
      min = l;
      max = h;
    }
    else
    {
      min = (min > l) ? l : min;
      max = (max > h) ? max : h;
    }

  }

}
//----------------------------------------
void CXYPlotDataCollection::GetYRange(double& min, double& max, uint32_t frame)
{
  CObArray::iterator it;
  CTools::SetDefaultValue(min);
  CTools::SetDefaultValue(max);

  for (it = begin(); it != end() ; it++)
  {
    CXYPlotData* data = Get(it);
    double l = 0;
    double h = 0;

    CXYPlotProperty* props = data->GetPlotProperty();

    data->GetYRange(l, h, frame);

    if (CTools::IsDefaultValue(props->GetYMin()) == false)
    {
      l = props->GetYMin();
    }
    if (CTools::IsDefaultValue(props->GetYMax()) == false)
    {
      h = props->GetYMax();
    }

    if (CTools::IsDefaultValue(min))
    {
      min = l;
      max = h;
    }
    else
    {
      min = (min > l) ? l : min;
      max = (max > h) ? max : h;
    }

  }
}
//----------------------------------------
void CXYPlotDataCollection::GetXRange(double& min, double& max)
{
  CObArray::iterator it;
  CTools::SetDefaultValue(min);
  CTools::SetDefaultValue(max);
  for (it = begin(); it != end() ; it++)
  {
    CXYPlotData* data = Get(it);
    double l = 0;
    double h = 0;

    CXYPlotProperty* props = data->GetPlotProperty();

    data->GetXRange(l, h);

    if (CTools::IsDefaultValue(props->GetXMin()) == false)
    {
      l = props->GetXMin();
    }
    if (CTools::IsDefaultValue(props->GetXMax()) == false)
    {
      h = props->GetXMax();
    }

    if (CTools::IsDefaultValue(min))
    {
      min = l;
      max = h;
    }
    else
    {
      min = (min > l) ? l : min;
      max = (max > h) ? max : h;
    }

  }

}

//----------------------------------------
void CXYPlotDataCollection::GetYRange(double& min, double& max)
{
  CObArray::iterator it;
  CTools::SetDefaultValue(min);
  CTools::SetDefaultValue(max);

  for (it = begin(); it != end() ; it++)
  {
    CXYPlotData* data = Get(it);
    double l = 0;
    double h = 0;

    CXYPlotProperty* props = data->GetPlotProperty();

    data->GetYRange(l, h);

    if (CTools::IsDefaultValue(props->GetYMin()) == false)
    {
      l = props->GetYMin();
    }
    if (CTools::IsDefaultValue(props->GetYMax()) == false)
    {
      h = props->GetYMax();
    }

    if (CTools::IsDefaultValue(min))
    {
      min = l;
      max = h;
    }
    else
    {
      min = (min > l) ? l : min;
      max = (max > h) ? max : h;
    }

  }
}
//----------------------------------------
bool CXYPlotDataCollection::ShowPropertyMenu()
{
  CObArray::iterator it;

  for (it = begin(); it != end() ; it++)
  {
    CXYPlotData* data = Get(it);

    CXYPlotProperty* props = data->GetPlotProperty();

    if (props->GetShowPropertyPanel())
    {
      return true;
    }
  }

  return false;

}

//----------------------------------------
void CXYPlotDataCollection::GetNames(wxArrayString& names)
{
  CObArray::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CXYPlotData* data = Get(it);
    names.Add(data->GetName());
  }
}

// WDR: class implementations



