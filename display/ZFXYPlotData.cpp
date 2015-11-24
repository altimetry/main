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
    #pragma implementation "ZFXYPlotData.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <ctime>

#include "new-gui/Common/tools/Trace.h"
#include "new-gui/Common/tools/Exception.h"
#include "LatLonPoint.h"
using namespace brathl;

#include "vtkProperty.h"
#include "vtkProperty2D.h"
#include "vtkDoubleArray.h"
#include "vtkShortArray.h"
#include "vtkRenderer.h"
#include "vtkPointData.h"
#include "vtkStructuredGrid.h"
#include "vtkCellData.h"
#include "vtkFloatArray.h"
#include "vtkCellDataToPointData.h"
#include "vtkStripper.h"
#include "vtkTextProperty.h"
#include "vtkIdTypeArray.h"
#include "vtkCellArray.h"
#include "vtkMath.h"
#include "vtkCamera.h"


#include "vtkMaskPoints.h"
#include "vtkBMPReader.h"
#include "vtkTexture.h"
#include "vtkSurfaceReconstructionFilter.h"
#include "vtkReverseSense.h"
#include "vtkTexturedSphereSource.h"
#include "vtkPNMReader.h"
#include "vtkGeometryFilter.h"
#include "vtkPolyDataNormals.h"
#include "vtkConnectivityFilter.h"
#include "vtkEarthSource.h"

#include "vtkZFXYPlotFilter.h"

#include "vtkDataSetReader.h"
#include "vtkCastToConcrete.h"
#include "vtkRectilinearGrid.h"
#include "vtkRectilinearGridGeometryFilter.h"
#include "vtkPolyDataWriter.h"

#include "ZFXYPlotData.h"

//-------------------------------------------------------------
//------------------- CLUTZFXYRenderer class --------------------
//-------------------------------------------------------------

CLUTZFXYRenderer::CLUTZFXYRenderer()
{

  m_vtkRend = NULL;
  m_LUT = NULL;
  m_scalarBarActor = NULL;

  m_vtkRend = vtkRenderer::New();

  m_LUT = new CBratLookupTable();

  m_scalarBarActor = vtkScalarBarActor::New();
  m_scalarBarActor->SetLookupTable(m_LUT->GetLookupTable());
  m_scalarBarActor->SetOrientationToHorizontal();
  m_scalarBarActor->SetPosition(0.1, 0.1);
  m_scalarBarActor->SetPosition2(0.8, 0.9);
  m_scalarBarActor->SetNumberOfLabels(2);

  vtkTextProperty* txtProp = m_scalarBarActor->GetLabelTextProperty();
  vtkTextProperty* titleProp = m_scalarBarActor->GetTitleTextProperty();

  titleProp->SetColor(0.7, 0.7, 0.7);
  titleProp->ShadowOff();
  titleProp->SetFontFamilyToArial();
  titleProp->SetFontSize(8);
  titleProp->ItalicOff();
  titleProp->BoldOff();

  txtProp->SetColor(0.7, 0.7, 0.7);
  txtProp->ShadowOff();
  txtProp->SetFontFamilyToArial();
  txtProp->SetFontSize(8);
  txtProp->ItalicOff();
  txtProp->SetJustificationToCentered();
  txtProp->BoldOff();

  m_scalarBarActor->SetLabelFormat("%g");

  m_vtkRend->AddActor2D(m_scalarBarActor);


}

//----------------------------------------
CLUTZFXYRenderer::~CLUTZFXYRenderer()
{
  if (m_scalarBarActor != NULL)
  {
    m_scalarBarActor->Delete();
    m_scalarBarActor = NULL;
  }

  if (m_vtkRend != NULL)
  {
    m_vtkRend->Delete();
    m_vtkRend = NULL;
  }

  DeleteLUT();

}
//----------------------------------------
void CLUTZFXYRenderer::ResetTextActor(CZFXYPlotData* zfxyPlotData)
{
  std::string text = zfxyPlotData->GetDataTitle();
  text += "\t\t(" + zfxyPlotData->m_plotProperty.m_name;
  text += ")\t\t-\t\tUnit:\t" +  zfxyPlotData->GetDataUnitString();
  text += "\t" + zfxyPlotData->GetDataDateString();

  ResetTextActor(text);
}
//----------------------------------------
void CLUTZFXYRenderer::ResetTextActor(const std::string& text)
{
  m_scalarBarActor->SetTitle(text.c_str());
}
//----------------------------------------
void CLUTZFXYRenderer::DeleteLUT()
{
  if (m_LUT != NULL)
  {
    delete m_LUT;
    m_LUT = NULL;
  }
}

//----------------------------------------
void CLUTZFXYRenderer::SetLUT(CBratLookupTable* lut)
{
  if (lut == NULL)
  {
    return;
  }

  DeleteLUT();

  m_LUT = new CBratLookupTable(*lut);
  m_scalarBarActor->SetLookupTable(m_LUT->GetLookupTable());
}

//----------------------------------------
void CLUTZFXYRenderer::SetNumberOfLabels(int32_t n)
{
  if (m_scalarBarActor == NULL)
  {
    return;
  }
  m_scalarBarActor->SetNumberOfLabels(n);
}

//----------------------------------------
int32_t CLUTZFXYRenderer::GetNumberOfLabels()
{
  if (m_scalarBarActor == NULL)
  {
    return 0;
  }

  return m_scalarBarActor->GetNumberOfLabels();
}

//----------------------------------------
void CLUTZFXYRenderer::SetBackground(double r, double g, double b)
{
  if (m_vtkRend == NULL)
  {
    return;
  }
  if (m_scalarBarActor == NULL)
  {
    return;
  }
  m_vtkRend->SetBackground(r, g, b);
  m_scalarBarActor->GetLabelTextProperty()->SetColor(1-r, 1-g, 1-b);
  m_scalarBarActor->GetTitleTextProperty()->SetColor(1-r, 1-g, 1-b);

}


//-------------------------------------------------------------
//------------------- CZFXYPlotProperty class --------------------
//-------------------------------------------------------------

CZFXYPlotProperty::CZFXYPlotProperty()
{

  m_opacity = 0.7;

  m_showPropertyPanel = true;
  m_showColorBar = true;
  m_showAnimationToolbar = false;

  m_numColorLabels = 5;

  setDefaultValue(m_minHeightValue);
  setDefaultValue(m_maxHeightValue);

  m_LUT = new CBratLookupTable();

  m_LUT->ExecMethodDefaultColorTable();

  setDefaultValue(m_xMax);
  setDefaultValue(m_xMin);
  setDefaultValue(m_yMax);
  setDefaultValue(m_yMin);

  m_xBase = 10.0;
  m_yBase = 10.0;

  setDefaultValue(m_xNumTicks);
  setDefaultValue(m_yNumTicks);

  m_xLog = false;
  m_yLog = false;

  m_withContour = false;
  m_solidColor = true;;

  m_withContourLabel = false;
  setDefaultValue(m_minContourValue);
  setDefaultValue(m_maxContourValue);
  m_numContour = 5;
  m_numContourLabel = 1;

  m_contourLabelSize = 10;
  m_contourLineWidth = 1.0;

  m_contourLineColor.Set(0.0, 0.0, 0.0);
  m_contourLineColor.Set(0.0, 0.0, 0.0);

  m_contourLabelFormat = "%-#.3g";

  m_withAnimation = false;


}

//----------------------------------------

CZFXYPlotProperty::CZFXYPlotProperty(const CZFXYPlotProperty& p)
{
  m_LUT = NULL;
  this->Copy(p);

}

//----------------------------------------
const CZFXYPlotProperty& CZFXYPlotProperty::operator=(const CZFXYPlotProperty& p)
{
  this->Copy(p);
  return *this;
}
//----------------------------------------
CZFXYPlotProperty::~CZFXYPlotProperty()
{
  DeleteLUT();
}

//----------------------------------------
void CZFXYPlotProperty::DeleteLUT()
{
  if (m_LUT != NULL)
  {
    delete m_LUT;
    m_LUT = NULL;
  }
}
//----------------------------------------
void CZFXYPlotProperty::Copy(const CZFXYPlotProperty& p)
{

  DeleteLUT();
  m_LUT = new CBratLookupTable(*(p.m_LUT));

  m_title = p.m_title;

  m_name = p.m_name;

  m_xLabel = p.m_xLabel;
  m_yLabel = p.m_yLabel;

  m_opacity = p.m_opacity;
  m_showPropertyPanel = p.m_showPropertyPanel;
  m_showColorBar = p.m_showColorBar;
  m_showAnimationToolbar = p.m_showAnimationToolbar;

  m_numColorLabels = p.m_numColorLabels;

  m_minHeightValue = p.m_minHeightValue;
  m_maxHeightValue = p.m_maxHeightValue;

  m_xMax = p.m_xMax;
  m_xMin = p.m_xMin;
  m_yMax = p.m_yMax;
  m_yMin = p.m_yMin;

  m_xBase = p.m_xBase;
  m_yBase = p.m_yBase;

  m_xLog = p.m_xLog;
  m_yLog = p.m_yLog;

  m_xNumTicks = p.m_xNumTicks;
  m_yNumTicks = p.m_yNumTicks;

  m_withContour = p.m_withContour;
  m_withContourLabel = p.m_withContourLabel;
  m_minContourValue = p.m_minContourValue;
  m_maxContourValue = p.m_maxContourValue;
  m_numContour = p.m_numContour;
  m_numContourLabel = p.m_numContourLabel;

  m_contourLabelSize = p.m_contourLabelSize;
  m_contourLineWidth = p.m_contourLineWidth;

  m_contourLineColor = p.m_contourLineColor;
  m_contourLabelColor = p.m_contourLabelColor;

  m_contourLabelFormat = p.m_contourLabelFormat;

  m_solidColor = p.m_solidColor;

  m_withAnimation = p.m_withAnimation;



}

//-------------------------------------------------------------
//------------------- CZFXYPlotData class --------------------
//-------------------------------------------------------------

CZFXYPlotData::CZFXYPlotData(wxWindow* parent, CZFXYPlot* plot, CPlotField* field)
{
  if (field == NULL)
  {
    return;
  }

  if (field->m_zfxyProps != NULL)
  {
    m_plotProperty = *field->m_zfxyProps;
  }

  m_currentMap = 0;

  m_colorBarRenderer = NULL;

  m_vtkMapper2D = vtkPolyDataMapper2D::New();
  m_vtkMapper2D->SetScalarModeToUseCellData();
  m_vtkMapper2D->UseLookupTableScalarRangeOn();


  m_vtkContourMapper2D = NULL;

  m_vtkActor2D = vtkActor2D::New();
  m_vtkActor2D->SetMapper(m_vtkMapper2D);
  m_vtkActor2D->PickableOn();

  m_vtkContourActor2D = NULL;

  m_LUT = NULL;

  m_vtkContourFilter = NULL;

  m_vtkContourLabelMapper = NULL;
  m_vtkContourLabelActor = NULL;

  m_vtkLabelContourData = NULL;
  m_vtkLabelContourPoints = NULL;
  m_vtkLabelContourPositions = NULL;

  m_vtkConnectivity = NULL;

  m_vtkVisiblePointsFilter = NULL;
  m_vtkVisibleSpherePointsFilter = NULL;
  m_vtkVisibleSpherePoints = NULL;
  m_vtkVisibleSpherePointsData = NULL;

  m_LUT = NULL;

  m_vtkRend = NULL;

  m_aborted = false;

  setDefaultValue(m_minhv);
  setDefaultValue(m_maxhv);

  setDefaultValue(m_xMin);
  setDefaultValue(m_xMax);

  setDefaultValue(m_yMin);
  setDefaultValue(m_yMax);

  Create(parent, &(field->m_internalFiles), field->m_name, plot);



}
//----------------------------------------
CZFXYPlotData::~CZFXYPlotData()
{
  if (m_colorBarRenderer != NULL)
  {
    delete m_colorBarRenderer;
    m_colorBarRenderer = NULL;
  }

  DeleteAll2D();

  DeleteVtkContourLabelObject();

  DeleteLUT();

}

//----------------------------------------
void CZFXYPlotData::NewVtkContourLabelObject()
{
  DeleteVtkContourLabelObject();

  m_vtkLabelContourData = vtkDoubleArray::New();
  m_vtkLabelContourPoints = vtkPoints::New(VTK_DOUBLE);
  m_vtkLabelContourPositions = vtkPolyData::New();

  m_vtkConnectivity = vtkPolyDataConnectivityFilter::New();

  m_vtkVisiblePointsFilter = vtkSelectVisiblePoints::New();
  m_vtkVisibleSpherePointsFilter = vtkProgrammableFilter::New();
  m_vtkVisibleSpherePoints = vtkPoints::New(VTK_DOUBLE);
  m_vtkVisibleSpherePointsData = vtkDoubleArray::New();


}

//----------------------------------------
void CZFXYPlotData::DeleteVtkContourLabelObject()
{
  if (m_vtkVisiblePointsFilter != NULL)
  {
    m_vtkVisiblePointsFilter->Delete();
    m_vtkVisiblePointsFilter = NULL;
  }
  if (m_vtkVisibleSpherePointsFilter != NULL)
  {
    m_vtkVisibleSpherePointsFilter->Delete();
    m_vtkVisibleSpherePointsFilter = NULL;
  }
  if (m_vtkVisibleSpherePoints != NULL)
  {
    m_vtkVisibleSpherePoints->Delete();
    m_vtkVisibleSpherePoints = NULL;
  }
  if (m_vtkVisibleSpherePointsData != NULL)
  {
    m_vtkVisibleSpherePointsData->Delete();
    m_vtkVisibleSpherePointsData = NULL;
  }
  if (m_vtkConnectivity != NULL)
  {
    m_vtkConnectivity->Delete();
    m_vtkConnectivity = NULL;
  }
  if (m_vtkLabelContourPositions != NULL)
  {
    m_vtkLabelContourPositions->Delete();
    m_vtkLabelContourPositions = NULL;
  }
  if (m_vtkLabelContourPoints != NULL)
  {
    m_vtkLabelContourPoints->Delete();
    m_vtkLabelContourPoints = NULL;
  }
  if (m_vtkLabelContourData != NULL)
  {
    m_vtkLabelContourData->Delete();
    m_vtkLabelContourData = NULL;
  }

}


//----------------------------------------
void CZFXYPlotData::DeleteContourFilter()
{
  if (m_vtkContourFilter != NULL)
  {
    m_vtkContourFilter->Delete();
    m_vtkContourFilter = NULL;
  }

}

//----------------------------------------
void CZFXYPlotData::DeleteLUT()
{
  if (m_LUT != NULL)
  {
    delete m_LUT;
    m_LUT = NULL;
  }

}

//----------------------------------------
void CZFXYPlotData::DeleteAll2D()
{
  Delete2D();
  DeleteContour2D();

}

//----------------------------------------
void CZFXYPlotData::Delete2D()
{
  DeleteActor2D();
  DeleteMapper2D();
}

//----------------------------------------
void CZFXYPlotData::DeleteContour2D()
{
  DeleteContourActor2D();
  DeleteContourMapper2D();
  DeleteContourLabelActor();
  DeleteContourLabelMapper();
  DeleteContourFilter();
}
//----------------------------------------
void CZFXYPlotData::DeleteContourLabelActor()
{
  if (m_vtkContourLabelActor != NULL)
  {
    m_vtkContourLabelActor->Delete();
    m_vtkContourLabelActor = NULL;
  }
}

//----------------------------------------
void CZFXYPlotData::DeleteActor2D()
{
  if (m_vtkActor2D != NULL)
  {
    m_vtkActor2D->Delete();
    m_vtkActor2D = NULL;
  }
}

//----------------------------------------
void CZFXYPlotData::DeleteContourActor2D()
{
  if (m_vtkContourActor2D != NULL)
  {
    m_vtkContourActor2D->Delete();
    m_vtkContourActor2D = NULL;
  }
}

//----------------------------------------
void CZFXYPlotData::DeleteContourLabelMapper()
{
  if (m_vtkContourLabelMapper != NULL)
  {
    m_vtkContourLabelMapper->Delete();
    m_vtkContourLabelMapper = NULL;
  }
}

//----------------------------------------
void CZFXYPlotData::DeleteMapper2D()
{
  if (m_vtkMapper2D != NULL)
  {
    m_vtkMapper2D->Delete();
    m_vtkMapper2D = NULL;
  }

}
//----------------------------------------
void CZFXYPlotData::DeleteContourMapper2D()
{
  if (m_vtkContourMapper2D != NULL)
  {
    m_vtkContourMapper2D->Delete();
    m_vtkContourMapper2D = NULL;
  }

}
//----------------------------------------
bool CZFXYPlotData::HasActor2D()
{
  return ((m_vtkActor2D != NULL) || (m_vtkContourActor2D != NULL));
}
//----------------------------------------
std::string CZFXYPlotData::GetDataDateString(uint32_t index)
{

  if (index >= m_dataDates.size())
  {
    return "";

  }

  CDate* dataDate = dynamic_cast<CDate*>(m_dataDates[index]);
  if (dataDate->IsDefaultValue() == false)
  {
    return CTools::Format(128, "Date:\t%s - Time:\t%s",
                          dataDate->AsString("%d-%b-%Y", false).c_str(),
                          dataDate->AsString("%H:%M:%S", true).c_str());
  }

  return "";
}

//----------------------------------------
std::string CZFXYPlotData::GetDataUnitString(uint32_t index)
{
  if (index >= m_dataUnits.size())
  {
    return "";
  }

  CUnit* dataUnit = dynamic_cast<CUnit*>(m_dataUnits[index]);

  return dataUnit->GetText();
}

//----------------------------------------
void CZFXYPlotData::SetLUT(CBratLookupTable* lut)
{
  if (lut == NULL)
  {
    return;
  }

  DeleteLUT();

  m_LUT = new CBratLookupTable(*lut);

  if (m_vtkMapper2D != NULL)
  {
    m_vtkMapper2D->SetLookupTable(m_LUT->GetLookupTable());
  }

}

//----------------------------------------
void CZFXYPlotData::SetInput(vtkPolyData* output)
{

  if (m_vtkMapper2D != NULL)
  {
    m_vtkMapper2D->SetInput(output);
  }

}

//----------------------------------------
void CZFXYPlotData::Create(wxWindow* parent, CObArray* data, const std::string& fieldName, CZFXYPlot* plot)
{
  m_contourLabelNeedUpdateOnWindow = false;
  m_contourLabelNeedUpdatePositionOnContour = false;


  if (m_plotProperty.m_name.IsEmpty())
  {
    m_plotProperty.m_name = fieldName.c_str();
  }

  if (m_plotProperty.m_title.IsEmpty())
  {
    m_plotProperty.m_title = plot->m_title;
  }


  if (m_plotProperty.m_xLabel.IsEmpty())
  {
    m_plotProperty.m_xLabel = plot->m_titleX;
  }
  else
  {
    wxString titleX = m_plotProperty.m_xLabel;
    titleX += plot->m_unitXLabel;
    m_plotProperty.m_xLabel = titleX;
  }

  if (m_plotProperty.m_yLabel.IsEmpty())
  {
    m_plotProperty.m_yLabel = plot->m_titleY;
  }
  else
  {
    wxString titleY = m_plotProperty.m_yLabel;
    titleY += plot->m_unitYLabel;
    m_plotProperty.m_yLabel = titleY;
  }

  m_unitX = plot->m_unitX;
  m_unitY = plot->m_unitY;

  m_colorBarRenderer = new CLUTZFXYRenderer();
  m_colorBarRenderer->GetVtkRenderer()->InteractiveOff();

  m_colorBarRenderer->SetNumberOfLabels(m_plotProperty.m_numColorLabels);

  /*
  m_minhv = 0.0;
  m_maxhv = 0.0;

  m_xMin = 0.0;
  m_xMax = 0.0;

  m_yMin = 0.0;
  m_yMax = 0.0;
  */

  m_finished = false;
  m_currentMap = 0;
  m_nrMaps = data->size();
  wxProgressDialog pd("Calculating Plot Data", "", m_nrMaps, parent, wxPD_CAN_ABORT | wxPD_REMAINING_TIME | wxPD_SMOOTH| wxPD_APP_MODAL);
  pd.SetSize(-1,-1, 350, -1);

  bool firstValue = true;

  uint32_t iMap = 0;
  int32_t iX = 0;
  int32_t iY = 0;
  int32_t maxX = 0;
  int32_t maxY = 0;

  double minXDataMapValue = 0;
  double maxXDataMapValue = 0;

  double minYDataMapValue = 0;
  double maxYDataMapValue = 0;

  vtkZFXYPlotFilter* zfxyPlotFilter = NULL;
  double minHeightValue = 0;
  double maxHeightValue = 0;

  for (iMap = 0 ; iMap < m_nrMaps ; iMap++)
  {
    setDefaultValue(minXDataMapValue);
    setDefaultValue(maxYDataMapValue);
    setDefaultValue(minYDataMapValue);
    setDefaultValue(maxYDataMapValue);

    minHeightValue = m_plotProperty.m_minHeightValue;
    maxHeightValue = m_plotProperty.m_maxHeightValue;

    CExpressionValue varX;
    CExpressionValue varY;
    CExpressionValue varValue;
    NetCDFVarKind varKind;


    CInternalFiles* zfxy = dynamic_cast<CInternalFiles*>(data->at(iMap));
    if (zfxy == NULL)
    {
      CException e("CZFXYPlotData ctor - dynamic_cast<CInternalFiles*>data->at(iMap) returns NULL",
                   BRATHL_LOGIC_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);

    }
    if (zfxy->IsGeographic())
    {
      CException e("CZFXYPlotData ctor - Geographical data found - zfxy->IsGeographic() is true",
                   BRATHL_LOGIC_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);

    }


    //update ProgressDialog
    std::string msg = CTools::Format("Calculating Frame: %d of %d", iMap + 1, m_nrMaps);
    if (pd.Update(iMap, msg.c_str()) == false)
    {
      m_aborted = true;
      break;
    }


    int32_t mapWidth = 0;
    int32_t mapHeight = 0;

    uint32_t dimRangeX = 0;
    uint32_t dimRangeY = 1;

    std::string varXName;
    std::string varYName;

    plot->GetPlotWidthHeight(zfxy, fieldName, mapWidth, mapHeight, varX, varY, dimRangeX, dimRangeY, varXName, varYName);

    CUnit unitXRead;
    CUnit unitYRead;

    // Get and control unit of X axis
    // X units are compatible but not the same --> convert
    unitXRead = zfxy->GetUnit(varXName);
    if (m_unitX.AsString() != unitXRead.AsString())
    {
      plot->m_unitX.SetConversionFrom(unitXRead);
      plot->m_unitX.ConvertVector(varX.GetValues(), varX.GetNbValues());
    }

    // Get and control unit of Y axis
    // Y units are compatible but not the same --> convert
    unitYRead = zfxy->GetUnit(varYName);
    if (m_unitY.AsString() != unitYRead.AsString())
    {
      plot->m_unitY.SetConversionFrom(unitYRead);
      plot->m_unitY.ConvertVector(varY.GetValues(), varY.GetNbValues());
    }


    // Get data unit
    CUnit* unit = new CUnit(zfxy->GetUnit(fieldName));
    m_dataUnits.Insert(unit);

    // Get data title
    std::string dataTitle = zfxy->GetTitle(fieldName);
    if (dataTitle.empty())
    {
      dataTitle = m_plotProperty.m_name;
    }

    m_dataTitles.Insert(dataTitle);


    // read data
    zfxy->ReadVar(fieldName, varValue, unit->GetText());
    varKind	= zfxy->GetVarKind(fieldName);


    if (varKind != Data)
    {
      std::string msg = CTools::Format("CZFXYPlotData ctor - variable '%s' is not a kind of Data (%d) : %s",
                                   fieldName.c_str(), Data, CNetCDFFiles::VarKindToString(varKind).c_str());
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
    }

    bool transpose = false;

    if ( (dimRangeY == 0) &&
         (dimRangeX == 1) )
    {
      transpose = true;
    }

    maxX = mapWidth;
    maxY = mapHeight;


    vtkDoubleArray* values = vtkDoubleArray::New();
    vtkShortArray* bitarray = vtkShortArray::New();
    vtkDoubleArray* yAxis = vtkDoubleArray::New();
    vtkDoubleArray* xAxis = vtkDoubleArray::New();

    for (iY = 0 ; iY < maxY ; iY++)
    {
      double valueY = varY.GetValues()[iY];
      yAxis->InsertNextValue(valueY);

      if (iY == 0)
      {
        if (iMap == 0)
        {
          m_yMin = valueY;
          m_yMax = valueY;
        }

        minYDataMapValue = valueY;
        maxYDataMapValue = valueY;
      }
      else
      {
        if (valueY < m_yMin)
        {
          m_yMin = valueY;
        }
        if (valueY > m_yMax)
        {
          m_yMax = valueY;
        }
        if (valueY < minYDataMapValue)
        {
          minYDataMapValue = valueY;
        }
        if (valueY > maxYDataMapValue)
        {
          maxYDataMapValue = valueY;
        }
      }


      for (iX = 0 ; iX < maxX ; iX++)
      {

        int32_t iTemp;
        if (transpose)
        {
          iTemp = (iY * maxX) + iX;
        }
        else
        {
          iTemp = (iX * maxY) + iY;
        }

        double v = varValue.GetValues()[iTemp];
        if ( (CTools::IsNan(v) != 0) || isDefaultValue(v))
        {

          values->InsertNextValue(0);
          bitarray->InsertNextValue(0);
        }
        else
        {
          values->InsertNextValue(v);

          bitarray->InsertNextValue(1);

	        if (firstValue)
          {
            firstValue = false;
            m_minhv = v;
            m_maxhv = v;
          }
          else
          {
            if (v < m_minhv)
            {
              m_minhv = v;
            }
            if (v > m_maxhv)
            {
              m_maxhv = v;
            }
          }
        }
      }
    }

    if (isDefaultValue(minHeightValue))
    {
      minHeightValue = m_minhv;
    }
    if (isDefaultValue(maxHeightValue))
    {
      maxHeightValue = m_maxhv;
    }

    for (iX = 0 ; iX < maxX ; iX++)
    {
      double valueX = varX.GetValues()[iX];
      xAxis->InsertNextValue(valueX);

      if (iX == 0)
      {
        if (iMap == 0)
        {
          m_xMin = valueX;
          m_xMax = valueX;
        }

        minXDataMapValue = valueX;
        maxXDataMapValue = valueX;

      }
      else
      {
        if (valueX < m_xMin)
        {
          m_xMin = valueX;
        }
        if (valueX > m_xMax)
        {
          m_xMax = valueX;
        }
        if (valueX < minXDataMapValue)
        {
          minXDataMapValue = valueX;
        }
        if (valueX > maxXDataMapValue)
        {
          maxXDataMapValue = valueX;
        }
      }

    }

    zfxyPlotFilter = vtkZFXYPlotFilter::New();

    zfxyPlotFilter->SetId(iMap);

    zfxyPlotFilter->SetValues(values);
    zfxyPlotFilter->SetBitarray(bitarray);
    zfxyPlotFilter->SetXDataArray(xAxis);
    zfxyPlotFilter->SetYDataArray(yAxis);

    zfxyPlotFilter->SetPlotWidth(mapWidth);
    zfxyPlotFilter->SetPlotHeight(mapHeight);

    zfxyPlotFilter->SetXDataRange(minXDataMapValue, maxXDataMapValue);
    zfxyPlotFilter->SetYDataRange(minYDataMapValue, maxYDataMapValue);

    m_zfxyPlotFilterList.Insert(zfxyPlotFilter);

    values->Delete();
    bitarray->Delete();
    xAxis->Delete();
    yAxis->Delete();
  }


  if (isDefaultValue(m_plotProperty.m_minContourValue))
  {
    m_plotProperty.m_minContourValue = minHeightValue;
  }

  if (isDefaultValue(m_plotProperty.m_maxContourValue))
  {
    m_plotProperty.m_maxContourValue = maxHeightValue;
  }

  pd.Destroy();

  if (m_aborted)
  {
    return;
  }

  for (iMap = 0 ; iMap < m_nrMaps ; iMap++)
  {
    zfxyPlotFilter = dynamic_cast<vtkZFXYPlotFilter*>(m_zfxyPlotFilterList[iMap]);
    zfxyPlotFilter->SetMinMappedValue(minHeightValue);
    zfxyPlotFilter->SetMaxMappedValue(maxHeightValue);
  }

  SetLUT(m_plotProperty.m_LUT);

  m_LUT->GetLookupTable()->SetTableRange(minHeightValue, maxHeightValue);

  m_colorBarRenderer->SetLUT(m_LUT);


  zfxyPlotFilter = GetCurrentPlotData();

  SetInput(zfxyPlotFilter->GetOutput());


}
//----------------------------------------
vtkZFXYPlotFilter*  CZFXYPlotData::GetCurrentPlotData()
{
  vtkZFXYPlotFilter* zfxyPlotFilter = dynamic_cast<vtkZFXYPlotFilter*>(m_zfxyPlotFilterList[m_currentMap]);
  if (zfxyPlotFilter == NULL)
  {
    throw CException(CTools::Format("ERROR in CZFXYPlotData::GetCurrentPlotData : dynamic_cast<vtkZFXYPlotFilter*>(m_zfxyPlotFilterList[%d]) returns NULL pointer - "
                     "m_zfxyPlotFilterList std::list seems to contain objects other than those of the class vtkZFXYPlotFilter", m_currentMap),
                     BRATHL_LOGIC_ERROR);

  }

  return zfxyPlotFilter;
}
//----------------------------------------
vtkZFXYPlotFilter*  CZFXYPlotData::GetPlotData(int32_t i)
{
  vtkZFXYPlotFilter* zfxyPlotFilter = dynamic_cast<vtkZFXYPlotFilter*>(m_zfxyPlotFilterList[i]);
  if (zfxyPlotFilter == NULL)
  {
    throw CException(CTools::Format("ERROR in CZFXYPlotData::GetPlotData : dynamic_cast<vtkZFXYPlotFilter*>(m_zfxyPlotFilterList[%d]) returns NULL pointer - "
                     "m_zfxyPlotFilterList std::list seems to contain objects other than those of the class vtkZFXYPlotFilter", i),
                     BRATHL_LOGIC_ERROR);

  }

  return zfxyPlotFilter;
}
//----------------------------------------
void CZFXYPlotData::GetXRange(double& min, double& max, uint32_t frame)
{
  if (frame >= m_zfxyPlotFilterList.size())
  {
    frame = m_zfxyPlotFilterList.size() - 1;
  }

  vtkZFXYPlotFilter* zfxyPlotFilter =  CZFXYPlotData::GetPlotData(frame);

  zfxyPlotFilter->GetXDataRange(min, max);

}
//----------------------------------------
void CZFXYPlotData::GetYRange(double& min, double& max, uint32_t frame)
{
  if (frame >= m_zfxyPlotFilterList.size())
  {
    frame = m_zfxyPlotFilterList.size() - 1;
  }

  vtkZFXYPlotFilter* zfxyPlotFilter =  CZFXYPlotData::GetPlotData(frame);

  zfxyPlotFilter->GetYDataRange(min, max);

}
//----------------------------------------
void CZFXYPlotData::OnKeyframeChanged(uint32_t i)
{
  if (m_finished)
  {
    return;
  }
  if (i >= m_zfxyPlotFilterList.size())
  {
    return;
  }

  if ( i != m_currentMap)
  {
    m_currentMap = i;
    vtkZFXYPlotFilter* zfxyPlotFilter = GetCurrentPlotData();

    SetInput(zfxyPlotFilter->GetOutput());
  }
}

//----------------------------------------
void CZFXYPlotData::Close()
{
  m_finished= true;
}

//----------------------------------------
void CZFXYPlotData::SetContour2D()
{
  DeleteContour2D();

  if (m_plotProperty.m_withContour == false)
  {
    return;
  }


  vtkZFXYPlotFilter* zfxyPlotFilter = GetCurrentPlotData();

  vtkCellDataToPointData* c2p = vtkCellDataToPointData::New();
  c2p->SetInput(zfxyPlotFilter->GetOutput());


  DeleteContourFilter();
  m_vtkContourFilter = vtkContourFilter::New();

  m_vtkContourFilter->SetInput(c2p->GetOutput());

  ContourGenerateValues();


  c2p->Delete();
  c2p = NULL;


  m_vtkContourMapper2D = vtkPolyDataMapper2D::New();
  m_vtkContourMapper2D->SetInput(m_vtkContourFilter->GetOutput());

  m_vtkContourMapper2D->ScalarVisibilityOff();

  m_vtkContourActor2D = vtkActor2D::New();
  m_vtkContourActor2D->SetMapper(m_vtkContourMapper2D);

  SetContour2DProperties();

  CreateContourLabels2D();
}

//----------------------------------------
void CZFXYPlotData::SetContourLabelProperties()
{
  if (m_vtkContourLabelMapper == NULL)
  {
    return;
  }

  vtkTextProperty* text = m_vtkContourLabelMapper->GetLabelTextProperty();

  text->SetFontFamilyToArial();
  text->SetFontSize(m_plotProperty.m_contourLabelSize);
  text->ShadowOff();
  text->ItalicOff();
  text->SetJustificationToCentered();
  text->SetColor(m_plotProperty.m_contourLabelColor.Red(),
                 m_plotProperty.m_contourLabelColor.Green(),
                 m_plotProperty.m_contourLabelColor.Blue()
                 );

  m_vtkContourLabelMapper->SetLabelFormat(m_plotProperty.m_contourLabelFormat.c_str());
  m_vtkContourLabelMapper->SetLabelModeToLabelScalars();

}

//----------------------------------------
void CZFXYPlotData::ContourGenerateValues()
{
  if (m_vtkContourFilter == NULL)
  {
    return;
  }

  m_vtkContourFilter->GenerateValues(m_plotProperty.m_numContour,
                                     m_plotProperty.m_minContourValue,
                                     m_plotProperty.m_maxContourValue
                                     );
  m_contourLabelNeedUpdateOnWindow = true;

}
//----------------------------------------
void CZFXYPlotData::SetContour2DProperties()
{
  if (m_vtkContourActor2D == NULL)
  {
    return;
  }

  m_vtkContourActor2D->GetProperty()->SetColor(m_plotProperty.m_contourLineColor.Red(),
                                             m_plotProperty.m_contourLineColor.Green(),
                                             m_plotProperty.m_contourLineColor.Blue()
                                             );

  m_vtkContourActor2D->GetProperty()->SetLineWidth(m_plotProperty.m_contourLineWidth);

}

//----------------------------------------
void CZFXYPlotData::FindVisiblePlanePoints(void* arg)
{
  CZFXYPlotData* zfxyPlotData = static_cast<CZFXYPlotData*>(arg);
  if (zfxyPlotData == NULL)
  {
    return;
  }

  // Finds visible points on sphere based on cosine angle of points
  // relative to camera view std::vector.
  // Works on assumption that sphere origin is world coord (0,0,0).
  vtkPolyData* input = zfxyPlotData->GetVtkVisibleSpherePointsFilter()->GetPolyDataInput();
  vtkPolyData* output = zfxyPlotData->GetVtkVisibleSpherePointsFilter()->GetPolyDataOutput();
  vtkPoints* pts = input->GetPoints();
  if (pts == NULL)
  {
    return;
  }
  int32_t npts = pts->GetNumberOfPoints();
  vtkDataArray* data = input->GetPointData()->GetScalars();
  zfxyPlotData->GetVtkVisibleSpherePoints()->Reset();
  zfxyPlotData->GetVtkVisibleSpherePointsData()->Reset();


  for (int32_t i = 0 ; i < npts ; i++)
  {
    vtkFloatingPointType* xyz = input->GetPoint(i);

    zfxyPlotData->GetVtkVisibleSpherePoints()->InsertNextPoint(xyz);
    zfxyPlotData->GetVtkVisibleSpherePointsData()->InsertNextValue(data->GetTuple1(i));
  }
  output->CopyStructure(input);
  output->SetPoints(zfxyPlotData->GetVtkVisibleSpherePoints());
  output->GetPointData()->SetScalars(zfxyPlotData->GetVtkVisibleSpherePointsData());

}

//----------------------------------------
void CZFXYPlotData::CreateContourLabels2D()
{
  if (m_plotProperty.m_withContour == false)
  {
    return;
  }

  NewVtkContourLabelObject();

  //Must be called after adding all actors to the renderer which will
  //make some of these labels invisible, i.e. a sphere's actor will
  //hide those labels on the rear half of the sphere.

  m_vtkContourLabelMapper = vtkLabeledDataMapper::New();

  SetContourLabelProperties();

  m_vtkConnectivity->SetInput(m_vtkContourFilter->GetOutput());
/*
  vtkPolyDataWriter* rectWriter = vtkPolyDataWriter::New();
  rectWriter->SetInput(m_vtkContourFilter->GetOutput());
  rectWriter->SetFileName("testWriter.vtk");
  rectWriter->SetFileTypeToASCII();
  rectWriter->Write();
  rectWriter->Delete();
*/
  m_vtkConnectivity->ScalarConnectivityOff();
  m_vtkConnectivity->SetExtractionModeToSpecifiedRegions();

  m_vtkLabelContourPositions->SetPoints(m_vtkLabelContourPoints);
  m_vtkLabelContourPositions->GetPointData()->SetScalars(m_vtkLabelContourData);

  m_vtkVisiblePointsFilter->SetRenderer(m_vtkRend);
  m_vtkVisiblePointsFilter->SetInput(m_vtkLabelContourPositions);

  m_vtkVisibleSpherePointsFilter->SetInput(m_vtkVisiblePointsFilter->GetOutput());
  m_vtkVisibleSpherePointsFilter->SetExecuteMethod(CZFXYPlotData::FindVisiblePlanePoints, static_cast<void*>(this));

  m_vtkContourLabelMapper->SetInput(m_vtkVisibleSpherePointsFilter->GetPolyDataOutput());

  m_vtkContourLabelActor = vtkActor2D::New();

  m_vtkContourLabelActor->SetMapper(m_vtkContourLabelMapper);

  UpdateContourLabels2D();

}

//----------------------------------------
void CZFXYPlotData::UpdateContourLabels2D()
{
  if (m_plotProperty.m_withContour == false)
  {
    return;
  }

  if (m_plotProperty.m_withContourLabel == false)
  {
    return;
  }

  if (m_vtkContourLabelActor == NULL)
  {
    return;
  }

  //wxSetCursor(*wxHOURGLASS_CURSOR);

  //Labels every contour.
  //If the data min/max is changed, this must be called manually to update the labels.

  m_vtkConnectivity->InitializeSpecifiedRegionList();
  m_vtkConnectivity->Update();
  int32_t nContours = m_vtkConnectivity->GetNumberOfExtractedRegions();

  int32_t nLabels = m_plotProperty.m_numContourLabel;


  m_vtkLabelContourPositions->Reset();
  vtkPoints* labelPoints = m_vtkLabelContourPositions->GetPoints();
  labelPoints->Allocate(nLabels * nContours, 1);

  m_vtkLabelContourData->SetNumberOfValues(nLabels * nContours);
  int32_t insertctr = 0;

  // Set evil seed (initial seed)
  srand( (unsigned)time( NULL ) );

  m_labelPts.RemoveAll();
  m_labelIds.RemoveAll();
  m_labelData.RemoveAll();

  vtkPoints* pts = NULL;
  vtkIdTypeArray* ids = NULL;
  vtkDataArray* data = NULL;

  vtkFloatingPointType* xyz = NULL;
  vtkFloatingPointType* xyzViewPort = NULL;

  vtkCoordinate* c = vtkCoordinate::New();
  c->SetCoordinateSystemToViewport();

  int32_t nTuples = 0;


  int32_t i = 0;
  int32_t j = 0;

  if (nLabels > 0)
  {
    for (i = 0 ; i < nContours ; i++)
    {
      m_vtkConnectivity->InitializeSpecifiedRegionList();
      // this extracts each contour line as a PolyData
      m_vtkConnectivity->AddSpecifiedRegion(i);
      m_vtkConnectivity->Update();

      pts = m_vtkConnectivity->GetOutput()->GetPoints();
      ids = m_vtkConnectivity->GetOutput()->GetLines()->GetData();
      data = m_vtkConnectivity->GetOutput()->GetPointData()->GetScalars();


      vtkPoints* ptsTemp = vtkPoints::New(VTK_DOUBLE);
      vtkIdTypeArray* idsTemp = vtkIdTypeArray::New();
      vtkDoubleArray* dataTemp = vtkDoubleArray::New();

      ptsTemp->DeepCopy(pts);
      idsTemp->DeepCopy(ids);
      dataTemp->DeepCopy(data);

      m_labelPts.Insert(ptsTemp);
      m_labelIds.Insert(idsTemp);
      m_labelData.Insert(dataTemp);

      nTuples = ids->GetNumberOfTuples();
      // don't label short simple lines; they can appear as points
      if (nTuples < 2)
      {
        continue;
      }

      int32_t randNum = static_cast<int32_t>( nTuples * rand() / (RAND_MAX + 1.0) );
      int32_t stride = static_cast<int32_t>(nTuples / (3*nLabels));

      for (j = 0 ; j < nLabels ; j++)
      {
        int32_t iD = (j*3*stride + randNum*3 + 1) % nTuples;

        int32_t tpl = static_cast<int32_t>(ids->GetTuple1(iD));

        xyz = pts->GetPoint(tpl);
        c->SetValue(xyz);
        xyzViewPort = c->GetComputedWorldValue(m_vtkRend);
        labelPoints->InsertPoint(insertctr, xyzViewPort);

        m_vtkLabelContourData->InsertValue(insertctr, data->GetTuple1(tpl));
        insertctr++;
      }
    }
  }

  c->Delete();

  //need to make render window update with new labels
  m_vtkLabelContourPositions->Modified();

  /*
  //int npts = labelPoints->GetNumberOfPoints();

  //m_vtkVisiblePointsFilter->Update();
  //m_vtkVisibleSpherePointsFilter->Update();
  vtkPolyDataWriter* rectWriter = vtkPolyDataWriter::New();
  rectWriter->SetInput(m_vtkVisiblePointsFilter->GetOutput());
  rectWriter->SetFileName("testWriter.vtk");
  rectWriter->SetFileTypeToASCII();
  rectWriter->Write();
  rectWriter->Delete();
*/


  //wxSetCursor(wxNullCursor);
  m_contourLabelNeedUpdateOnWindow = false;
}

/*
//----------------------------------------
void CZFXYPlotData::SetContourLabels2DPosition()
{
  if (m_plotProperty.m_withContour == false)
  {
    return;
  }

  if (m_plotProperty.m_withContourLabel == false)
  {
    return;
  }

  if (m_vtkContourLabelActor == NULL)
  {
    return;
  }

  wxSetCursor(*wxHOURGLASS_CURSOR);

  int32_t nLabels = m_plotProperty.m_numContourLabel;
  int32_t nContours =  m_labelPts.size();

  m_vtkLabelContourPositions->Reset();
  vtkPoints* labelPoints = m_vtkLabelContourPositions->GetPoints();
  labelPoints->Allocate(nLabels * nContours, 1);

  m_vtkLabelContourData->SetNumberOfValues(nLabels * nContours);
  int32_t insertctr = 0;

  // Set evil seed (initial seed)
  srand( (unsigned)time( NULL ) );

  vtkPoints* pts = NULL;
  vtkIdTypeArray* ids = NULL;
  vtkDataArray* data = NULL;

  vtkFloatingPointType* xyz = NULL;
  vtkFloatingPointType* xyzViewPort = NULL;

  vtkCoordinate* c = vtkCoordinate::New();
  c->SetCoordinateSystemToViewport();

  int32_t nTuples = 0;


  int32_t i = 0;
  int32_t j = 0;

  if (nLabels > 0)
  {
    for (i = 0 ; i < nContours ; i++)
    {

      pts = vtkPoints::SafeDownCast(m_labelPts[i]);
      ids = vtkIdTypeArray::SafeDownCast(m_labelIds[i]);
      data = vtkDataArray::SafeDownCast(m_labelData[i]);

      nTuples = ids->GetNumberOfTuples();
      // don't label short simple lines; they can appear as points
      if (nTuples < 2)
      {
        continue;
      }

      int32_t randNum = static_cast<int32_t>( nTuples * rand() / (RAND_MAX + 1.0) );
      int32_t stride = static_cast<int32_t>(nTuples / (3*nLabels));

      for (j = 0 ; j < nLabels ; j++)
      {
        int32_t iD = (j*3*stride + randNum*3 + 1) % nTuples;

        int32_t tpl = static_cast<int32_t>(ids->GetTuple1(iD));

        xyz = pts->GetPoint(tpl);
        c->SetValue(xyz);
        xyzViewPort = c->GetComputedWorldValue(m_vtkRend);

        labelPoints->InsertPoint(insertctr, xyzViewPort);
        //labelPoints->InsertPoint(insertctr, xyz);

        m_vtkLabelContourData->InsertValue(insertctr, data->GetTuple1(tpl));
        insertctr++;
      }
    }
  }

  c->Delete();

  //need to make render window update with new labels
  m_vtkLabelContourPositions->Modified();

  wxSetCursor(wxNullCursor);
}
*/

