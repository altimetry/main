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
    #pragma implementation "WorldPlotData.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <ctime>

#include "Trace.h"
#include "Exception.h"
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
#include "vtkRegularPolygonSource.h"

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

#include "WorldPlotData.h"

//-------------------------------------------------------------
//------------------- CLUTRenderer class --------------------
//-------------------------------------------------------------

CLUTRenderer::CLUTRenderer()
{

  m_vtkRend = NULL;
  m_LUT = NULL;
  m_scalarBarActor = NULL;
  //m_txtProp = NULL;
  //m_titleProp = NULL;

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
CLUTRenderer::~CLUTRenderer()
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
void CLUTRenderer::ResetTextActor(CGeoMap* geoMap)
{
  string text = geoMap->GetDataTitle();
  text += "\t\t(" + geoMap->m_plotProperty.m_name;
  text += ")\t\t-\t\tUnit:\t" +  geoMap->GetDataUnitString();
  text += "\t" + geoMap->GetDataDateString();

  ResetTextActor(text);
}
//----------------------------------------
void CLUTRenderer::ResetTextActor(const string& text)
{
  m_scalarBarActor->SetTitle(text.c_str());
}
//----------------------------------------
void CLUTRenderer::DeleteLUT()
{
  if (m_LUT != NULL)
  {
    delete m_LUT;
    m_LUT = NULL;
  }
}

//----------------------------------------
void CLUTRenderer::SetLUT(CBratLookupTable* lut)
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
void CLUTRenderer::SetNumberOfLabels(int32_t n)
{
  if (m_scalarBarActor == NULL)
  {
    return;
  }
  m_scalarBarActor->SetNumberOfLabels(n);
}

//----------------------------------------
int32_t CLUTRenderer::GetNumberOfLabels()
{
  if (m_scalarBarActor == NULL)
  {
    return 0;
  }

  return m_scalarBarActor->GetNumberOfLabels();
}

//----------------------------------------
void CLUTRenderer::SetBackground(double r, double g, double b)
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
//------------------- CWorldPlotProperty class --------------------
//-------------------------------------------------------------

CWorldPlotProperty::CWorldPlotProperty()
{

  //m_projection = "";
  m_opacity = 0.7;
  m_deltaRadius = 0.007;

  m_showPropertyPanel = true;
  m_showColorBar = true;
  m_showAnimationToolbar = false;
  //m_lineWidth = 1;
  //m_pointSize = 2;
  //m_color.Insert(0);
  //m_color.Insert(0);
  //m_color.Insert(1);

  m_heightFactor = 0;
  m_numColorLabels = 5;

  //m_colorTable = NULL;

  CTools::SetDefaultValue(m_minHeightValue);
  CTools::SetDefaultValue(m_maxHeightValue);

  //CTools::SetDefaultValue(m_stipplepattern);
  m_valueConversion = 0.0;
  m_heightConversion = 0.0;

  m_centerLongitude = 0.0;
  m_centerLatitude = 0.0;

  m_LUT = new CBratLookupTable();

  m_LUT->ExecMethodDefaultColorTable();

  m_zoom = false;
  m_zoomLon1 = -180.0;
  m_zoomLon2 = 180.0;
  m_zoomLat1 = -90.0;
  m_zoomLat2 = 90.0;

  m_withContour = false;
  m_solidColor = true;;
  m_gridLabel = true;

  m_withContourLabel = false;
  CTools::SetDefaultValue(m_minContourValue);
  CTools::SetDefaultValue(m_maxContourValue);
  m_numContour = 5;
  m_numContourLabel = 1;

  m_contourLabelSize = 10;
  m_contourLineWidth = 1.0;

  m_contourLineColor.Set(0.0, 0.0, 0.0);
  m_contourLineColor.Set(0.0, 0.0, 0.0);

  m_contourLabelFormat = "%-#.3g";

  m_withAnimation = false;

  m_eastComponent = false;
  m_northComponent = false;


}

//----------------------------------------

CWorldPlotProperty::CWorldPlotProperty(const CWorldPlotProperty& p)
{
  m_LUT = NULL;
  this->Copy(p);

}

//----------------------------------------
const CWorldPlotProperty& CWorldPlotProperty::operator=(const CWorldPlotProperty& p)
{
  this->Copy(p);
  return *this;
}
//----------------------------------------
CWorldPlotProperty::~CWorldPlotProperty()
{
  DeleteLUT();
}

//----------------------------------------
void CWorldPlotProperty::DeleteLUT()
{
  if (m_LUT != NULL)
  {
    delete m_LUT;
    m_LUT = NULL;
  }
}
//----------------------------------------
void CWorldPlotProperty::Copy(const CWorldPlotProperty& p)
{

  m_coastResolution = p.m_coastResolution;
  m_projection = p.m_projection;

  DeleteLUT();
  m_LUT = new CBratLookupTable(*(p.m_LUT));

  m_title = p.m_title;

  m_name = p.m_name;

  m_opacity = p.m_opacity;
  m_deltaRadius = p.m_deltaRadius;
  m_showPropertyPanel = p.m_showPropertyPanel;
  m_showColorBar = p.m_showColorBar;
  m_showAnimationToolbar = p.m_showAnimationToolbar;

  m_heightFactor = p.m_heightFactor;
  m_numColorLabels = p.m_numColorLabels;

  m_minHeightValue = p.m_minHeightValue;
  m_maxHeightValue = p.m_maxHeightValue;
  m_valueConversion = p.m_valueConversion;
  m_heightConversion = p.m_heightConversion;

  m_centerLongitude = p.m_centerLongitude;
  m_centerLatitude = p.m_centerLatitude;

  m_zoom = p.m_zoom;
  m_zoomLon1 = p.m_zoomLon1;
  m_zoomLon2 = p.m_zoomLon2;
  m_zoomLat1 = p.m_zoomLat1;
  m_zoomLat2 = p.m_zoomLat2;

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

  m_eastComponent = p.m_eastComponent;
  m_northComponent = p.m_northComponent;


}



//-------------------------------------------------------------
//------------------- CWorldPlotData class --------------------
//-------------------------------------------------------------

CWorldPlotData::CWorldPlotData(CWorldPlotProperty* plotProperty)
{
  if (plotProperty != NULL)
  {
    m_plotProperty = *plotProperty;
  }

  m_colorBarRenderer = NULL;

  m_vtkActor = NULL;
  m_vtkContourActor = NULL;

  m_vtkActor2D = NULL;
  m_vtkContourActor2D = NULL;

  m_vtkMapper = NULL;
  m_vtkContourMapper = NULL;

  m_vtkMapper2D = NULL;
  m_vtkContourMapper2D = NULL;

  m_LUT = NULL;
  //m_dateString = "";

  m_vtkFilter = vtkProj2DFilter::New();
#ifdef WIN32
  // decrease interpolation distance on windows
  // because of inacurate calculations with VC 7.1
  GetVtkFilter()->SetInterpolationDistance(0.02);
#endif

  m_vtkTransform = vtkTransformPolyDataFilter::New();

  m_transform = vtkTransform::New();

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

  m_LUT = new CBratLookupTable(*m_plotProperty.m_LUT);

  m_vtkRend = NULL;

  m_aborted = false;

  m_is3D = false;

  m_projection = VTK_PROJ2D_3D;

  NoTransform();
}
//----------------------------------------
CWorldPlotData::~CWorldPlotData()
{
  if (m_colorBarRenderer != NULL)
  {
    delete m_colorBarRenderer;
    m_colorBarRenderer = NULL;
  }

  DeleteAll3D();
  DeleteAll2D();

  DeleteVtkContourLabelObject();

  if (m_vtkFilter != NULL)
  {
    m_vtkFilter->Delete();
    m_vtkFilter = NULL;
  }

  if (m_vtkTransform != NULL)
  {
    m_vtkTransform->Delete();
    m_vtkTransform = NULL;
  }

  if (m_transform != NULL)
  {
    m_transform->Delete();
    m_transform = NULL;
  }

  DeleteLUT();


}
//----------------------------------------
void CWorldPlotData::NewVtkContourLabelObject()
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
void CWorldPlotData::DeleteVtkContourLabelObject()
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
void CWorldPlotData::DeleteContourFilter()
{
  if (m_vtkContourFilter != NULL)
  {
    m_vtkContourFilter->Delete();
    m_vtkContourFilter = NULL;
  }

}
//----------------------------------------
void CWorldPlotData::DeleteLUT()
{
  if (m_LUT != NULL)
  {
    delete m_LUT;
    m_LUT = NULL;
  }

}

//----------------------------------------
void CWorldPlotData::DeleteAll3D()
{
  Delete3D();
  DeleteContour3D();
}

//----------------------------------------
void CWorldPlotData::Delete3D()
{
  DeleteActor();
  DeleteMapper();
}

//----------------------------------------
void CWorldPlotData::DeleteContour3D()
{
  DeleteContourActor();
  DeleteContourMapper();
  DeleteContourLabelActor();
  DeleteContourLabelMapper();
  DeleteContourFilter();
}

//----------------------------------------
void CWorldPlotData::DeleteAll2D()
{
  Delete2D();
  DeleteContour2D();

}
//----------------------------------------
void CWorldPlotData::Delete2D()
{
  DeleteActor2D();
  DeleteMapper2D();
}

//----------------------------------------
void CWorldPlotData::DeleteContour2D()
{
  DeleteContourActor2D();
  DeleteContourMapper2D();
  DeleteContourLabelActor();
  DeleteContourLabelMapper();
  DeleteContourFilter();
}

//----------------------------------------
void CWorldPlotData::DeleteActor()
{
  if (m_vtkActor != NULL)
  {
    m_vtkActor->Delete();
    m_vtkActor = NULL;
  }
}

//----------------------------------------
void CWorldPlotData::DeleteContourActor()
{
  if (m_vtkContourActor != NULL)
  {
    m_vtkContourActor->Delete();
    m_vtkContourActor = NULL;
  }
}
//----------------------------------------
void CWorldPlotData::DeleteContourLabelActor()
{
  if (m_vtkContourLabelActor != NULL)
  {
    m_vtkContourLabelActor->Delete();
    m_vtkContourLabelActor = NULL;
  }
}

//----------------------------------------
void CWorldPlotData::DeleteActor2D()
{
  if (m_vtkActor2D != NULL)
  {
    m_vtkActor2D->Delete();
    m_vtkActor2D = NULL;
  }
}

//----------------------------------------
void CWorldPlotData::DeleteContourActor2D()
{
  if (m_vtkContourActor2D != NULL)
  {
    m_vtkContourActor2D->Delete();
    m_vtkContourActor2D = NULL;
  }
}

//----------------------------------------
void CWorldPlotData::DeleteMapper()
{
  if (m_vtkMapper != NULL)
  {
    m_vtkMapper->Delete();
    m_vtkMapper = NULL;
  }
}

//----------------------------------------
void CWorldPlotData::DeleteContourMapper()
{
  if (m_vtkContourMapper != NULL)
  {
    m_vtkContourMapper->Delete();
    m_vtkContourMapper = NULL;
  }
}

//----------------------------------------
void CWorldPlotData::DeleteContourLabelMapper()
{
  if (m_vtkContourLabelMapper != NULL)
  {
    m_vtkContourLabelMapper->Delete();
    m_vtkContourLabelMapper = NULL;
  }
}

//----------------------------------------
void CWorldPlotData::DeleteMapper2D()
{
  if (m_vtkMapper2D != NULL)
  {
    m_vtkMapper2D->Delete();
    m_vtkMapper2D = NULL;
  }

}
//----------------------------------------
void CWorldPlotData::DeleteContourMapper2D()
{
  if (m_vtkContourMapper2D != NULL)
  {
    m_vtkContourMapper2D->Delete();
    m_vtkContourMapper2D = NULL;
  }

}
//----------------------------------------
bool CWorldPlotData::HasActor()
{
  return ((m_vtkActor != NULL) || (m_vtkContourActor != NULL));
}
//----------------------------------------
bool CWorldPlotData::HasActor2D()
{
  return ((m_vtkActor2D != NULL) || (m_vtkContourActor2D != NULL));
}
//----------------------------------------

void CWorldPlotData::NoTransform()
{
  m_transform->Identity();
  m_vtkTransform->SetTransform(m_transform);

}

//----------------------------------------
void CWorldPlotData::SetLUT(CBratLookupTable* lut)
{
  if (lut == NULL)
  {
    return;
  }

  if ( (m_vtkMapper == NULL) && (m_vtkMapper2D == NULL) )
  {
    return;
  }


  DeleteLUT();

  m_LUT = new CBratLookupTable(*lut);

  if (m_vtkMapper != NULL)
  {
    m_vtkMapper->SetLookupTable(m_LUT->GetLookupTable());
  }
  else if (m_vtkMapper2D != NULL)
  {
    m_vtkMapper2D->SetLookupTable(m_LUT->GetLookupTable());
  }
}

//----------------------------------------

void CWorldPlotData::SetProjection(int32_t proj)
{
  m_projection = proj;
  GetVtkFilter()->SetProjection(proj);

  //if ( (proj == VTK_PROJ2D_3D) && ((m_is3D == false) || (m_vtkActor == NULL)) )
  if ( (proj == VTK_PROJ2D_3D) && ((m_is3D == false) || (HasActor() == false)) )
  {
    DeleteAll2D();

    Set3D();

  }
  //else if ( (proj != VTK_PROJ2D_3D) && ((m_is3D == true) || (m_vtkActor2D == NULL)) )
  else if ( (proj != VTK_PROJ2D_3D) && ((m_is3D == true) || (HasActor2D() == false)) )
  {
    DeleteAll3D();

    Set2D();
  }
  else
  {
    return;
    //CException e("ERROR in CWorldPlotData::SetProjection : Mode is  neither 3D nor 2D", BRATHL_LOGIC_ERROR);
    //throw(e);
  }



}
//----------------------------------------
void CWorldPlotData::Set3D()
{
  Delete3D();

  m_is3D = true;

  m_vtkMapper = vtkPolyDataMapper::New();

  m_vtkMapper->SetInput(GetVtkFilter()->GetOutput());
  m_vtkMapper->SetScalarModeToUseCellData();
  m_vtkMapper->UseLookupTableScalarRangeOn();

  if (m_LUT != NULL)
  {
    m_vtkMapper->SetLookupTable(m_LUT->GetLookupTable());
  }

  m_vtkActor = vtkActor::New();
  m_vtkActor->SetMapper(m_vtkMapper);

  Update();

}
//----------------------------------------
void CWorldPlotData::Set2D()
{
  Delete2D();

  m_is3D = false;

  m_vtkTransform->SetInput(GetVtkFilter()->GetOutput());
  //m_vtkTransform->SetTransform(m_transform);

  vtkCoordinate* c = vtkCoordinate::New();
  c->SetCoordinateSystemToNormalizedViewport();


  m_vtkMapper2D = vtkPolyDataMapper2D::New();
  m_vtkMapper2D->SetInput(m_vtkTransform->GetOutput());
  m_vtkMapper2D->SetTransformCoordinate(c);

  m_vtkMapper2D->SetScalarModeToUseCellData();
  m_vtkMapper2D->UseLookupTableScalarRangeOn();

  if (m_LUT != NULL)
  {
    m_vtkMapper2D->SetLookupTable(m_LUT->GetLookupTable());
  }

  m_vtkActor2D = vtkActor2D::New();
  m_vtkActor2D->SetMapper(m_vtkMapper2D);

  c->Delete();
  c = NULL;

  Update();

}

//----------------------------------------
void CWorldPlotData::SetCenterLatitude(double lat)
{
  if (GetVtkFilter() != NULL)
  {
    GetVtkFilter()->SetCenterLatitude(lat);
  }
}

//----------------------------------------
void CWorldPlotData::SetCenterLongitude(double lon)
{
  if (GetVtkFilter() != NULL)
  {
    GetVtkFilter()->SetCenterLongitude(lon);
  }
}

//----------------------------------------
void CWorldPlotData::SetInput(vtkPolyData* output)
{
  if (GetVtkFilter() != NULL)
  {
    GetVtkFilter()->SetInput(output);
  }
}


//-------------------------------------------------------------
//------------------- CGeoGrid class --------------------
//-------------------------------------------------------------

CGeoGrid::CGeoGrid() : CWorldPlotData()
{

  m_source = vtkGeoGridSource::New();
  SetInput(m_source->GetOutput());

}

//----------------------------------------

CGeoGrid::~CGeoGrid()
{
  if (m_source != NULL)
  {
    m_source->Delete();
    m_source = NULL;
  }

}

//----------------------------------------
void CGeoGrid::SetProjection(int32_t proj)
{

  if (proj == VTK_PROJ2D_MERCATOR)
  {
    m_source->SetLatitudeLimitMagnitude(178.0);
    m_source->CreateParallelsForPolesOff();
  }
  else
  {
    m_source->SetLatitudeLimitMagnitude(180.0);
    m_source->CreateParallelsForPolesOn();
  }



  CWorldPlotData::SetProjection(proj);

  //m_source->Print(*CTrace::GetDumpContext());
}


//----------------------------------------
void CGeoGrid::Update()
{
  //m_source->Modified();
  m_source->Update();

  if ((m_is3D == true) && (m_vtkActor != NULL))
  {
    m_vtkActor->GetProperty()->SetOpacity(m_plotProperty.m_opacity);
    m_vtkActor->GetProperty()->SetLineWidth(.7);
  }
  else if ((m_is3D == false) && (m_vtkActor2D != NULL))
  {
    m_vtkActor2D->GetProperty()->SetOpacity(m_plotProperty.m_opacity);
    m_vtkActor2D->GetProperty()->SetLineWidth(.7);
  }
  else
  {
    return;
    //CException e("ERROR in CGeoGrid::Update() : Mode is  neither 3D nor 2D", BRATHL_LOGIC_ERROR);
    //throw(e);
  }

}

//----------------------------------------------------------------------------
bool CGeoGrid::FindDataCoords2D(vtkRenderer* ren, double value[2], int& xCoord, int& yCoord)
{
  return FindDataCoords2D(ren, value[0], value[1], xCoord, yCoord);
}

//----------------------------------------------------------------------------
bool CGeoGrid::FindDataCoords2D(vtkRenderer* ren, double xValue, double yValue, int& xCoord, int& yCoord)
{
  if (m_is3D)
  {
    return false;
  }

  if (ren == NULL)
  {
    return false;
  }

  if (m_vtkMapper2D == NULL)
  {
    return false;
  }

  if (m_transform == NULL)
  {
    return false;
  }

  vtkPolyData *outputSource = (vtkPolyData *) m_source->GetOutput();
  vtkPolyData *outputTransform = (vtkPolyData *) m_vtkTransform->GetOutput();

  if (outputSource == NULL)
  {
    return false;
  }
  if (outputTransform == NULL)
  {
    return false;
  }

  vtkCoordinate* c = vtkCoordinate::New();
  c->SetCoordinateSystemToNormalizedViewport();
  vtkIdType point = outputSource->FindPoint(xValue, yValue, 0);

  if ( (point < 0) || (point >= outputTransform->GetNumberOfPoints()))
  {
    c->Delete();
    return false;
  }

  int* iValue;

  vtkFloatingPointType* xyz = outputTransform->GetPoint(point);
  c->SetValue(xyz);
  //iValue = c->GetComputedDisplayValue (ren);
  //iValue = c->GetComputedLocalDisplayValue (ren);
  iValue = c->GetComputedViewportValue (ren);
  //iValue = c->GetComputedWorldValue (ren);
  xCoord = iValue[0];
  yCoord = iValue[1];

  c->Delete();

  return true;
}

//-------------------------------------------------------------
//------------------- CCoastLine class --------------------
//-------------------------------------------------------------

CCoastLine::CCoastLine(const string& fileName, int32_t maxLevel) : CWorldPlotData()
{

  m_GSHHSReader = vtkGSHHSReader::New();
  SetGSHHSReader(fileName, maxLevel);

  SetInput(m_GSHHSReader->GetOutput());


}

//----------------------------------------

CCoastLine::~CCoastLine()
{
  if (m_GSHHSReader != NULL)
  {
    m_GSHHSReader->Delete();
    m_GSHHSReader = NULL;
  }

}
//----------------------------------------
void CCoastLine::SetGSHHSReader(const string& fileName, int32_t maxLevel)
{
  if (m_GSHHSReader != NULL)
  {
    m_GSHHSReader->SetFileName(fileName.c_str());
    m_GSHHSReader->SetMaxLevel(maxLevel);
  }

}

//----------------------------------------

void CCoastLine::Update()
{

  if ((m_is3D == true) && (m_vtkActor != NULL))
  {
    //m_vtkMapper->SetInputConnection(normals2->GetOutputPort());

    m_vtkActor->GetProperty()->SetOpacity(m_plotProperty.m_opacity);
    m_vtkActor->GetProperty()->SetLineWidth(1.5);

  }
  else if ((m_is3D == false) && (m_vtkActor2D != NULL))
  {
    //m_vtkMapper2D->SetInputConnection(normals2->GetOutputPort());
    m_vtkActor2D->GetProperty()->SetOpacity(m_plotProperty.m_opacity);
    m_vtkActor2D->GetProperty()->SetLineWidth(1.5);
  }
  else
  {
    return;
    //CException e("ERROR in CCoastLine::Update() : Mode is  neither 3D nor 2D", BRATHL_LOGIC_ERROR);
    //throw(e);
  }

}



//-------------------------------------------------------------
//------------------- CGlobe class --------------------
//-------------------------------------------------------------

CGlobe::CGlobe() : CWorldPlotData()
{

  m_source = vtkSphereSource::New();
  m_source->SetRadius(1);
  m_source->SetPhiResolution(30);
  m_source->SetThetaResolution(60);

  m_vtkMapper = vtkPolyDataMapper::New();
  m_vtkMapper->SetInput(m_source->GetOutput());

}

//----------------------------------------

CGlobe::~CGlobe()
{
  if (m_source != NULL)
  {
    m_source->Delete();
    m_source = NULL;
  }

}

//----------------------------------------
void CGlobe::SetProjection(int32_t proj)
{
  if ( (proj == VTK_PROJ2D_3D) && (m_vtkActor == NULL) )
  {
    m_vtkActor = vtkActor::New();
    m_vtkActor->SetMapper(m_vtkMapper);
  }
  else if (proj != VTK_PROJ2D_3D)
  {
    DeleteActor();
  }

}

//-------------------------------------------------------------
//------------------- CGeoMap class --------------------
//-------------------------------------------------------------

//----------------------------------------
CGeoMap::CGeoMap(wxWindow* parent, CPlotField* field)
        : CWorldPlotData(field->m_worldProps)

{
  Create(parent, &(field->m_internalFiles), (const char *)(field->m_name));
}
//----------------------------------------
CGeoMap::CGeoMap(wxWindow* parent, CObArray* data, const string& fieldName, CWorldPlotProperty* plotProperty)
        : CWorldPlotData(plotProperty)

{
  Create(parent, data, fieldName);
}
//----------------------------------------
CGeoMap::~CGeoMap()
{
}

//----------------------------------------
// for base classes
CGeoMap::CGeoMap(wxWindow* parent, CWorldPlotProperty *property)
        : CWorldPlotData(property)
{
}

//----------------------------------------
void CGeoMap::Create(wxWindow* parent, CObArray* data, const string& fieldName)
{
  m_contourLabelNeedUpdateOnWindow = false;
  m_contourLabelNeedUpdatePositionOnContour = false;

  if (m_plotProperty.m_name.IsEmpty())
  {
    m_plotProperty.m_name = fieldName.c_str();
  }

  m_colorBarRenderer = new CLUTRenderer();
  m_colorBarRenderer->GetVtkRenderer()->InteractiveOff();

  m_colorBarRenderer->SetNumberOfLabels(m_plotProperty.m_numColorLabels);

  m_minhv = 0.0;
  m_maxhv = 0.0;

  m_finished = false;
  m_currentMap = 0;
  m_nrMaps = data->size();


  wxProgressDialog pd("Calculating Geomap Data", "", m_nrMaps, parent, wxPD_CAN_ABORT | wxPD_REMAINING_TIME | wxPD_SMOOTH| wxPD_APP_MODAL);
  pd.SetSize(-1,-1, 350, -1);

  bool firstValue = true;

  uint32_t iMap = 0;
  int32_t iX = 0;
  int32_t iY = 0;
  int32_t maxX = 0;
  int32_t maxY = 0;
  bool firstDimIsLat = false;

  vtkGeoMapFilter* geoMapFilter = NULL;
  //vtkVelocityGlyphFilter* geoMapFilter = NULL;
  double minHeightValue = 0;
  double maxHeightValue = 0;

  for (iMap = 0 ; iMap < m_nrMaps ; iMap++)
  {
    minHeightValue = m_plotProperty.m_minHeightValue;
    maxHeightValue = m_plotProperty.m_maxHeightValue;

    CExpressionValue varLat;
    CExpressionValue varLon;
    CExpressionValue varValue;
    NetCDFVarKind varKind;
    ExpressionValueDimensions varDimVal;
    CStringArray varDimNames;


    CInternalFilesZFXY* zfxy = dynamic_cast<CInternalFilesZFXY*>(data->at(iMap));
    if (zfxy == NULL)
    {
      CException e("CGeoMap - Non-geographical data found - dynamic_cast<CInternalFilesZFXY*>data->at(iMap) returns NULL",
                   BRATHL_LOGIC_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);

    }
    if (zfxy->IsGeographic() == false)
    {
      CException e("CGeoMap - Non-geographical data found - zfxy->IsGeographic() is false",
                   BRATHL_LOGIC_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);

    }


    //update ProgressDialog
    string msg = CTools::Format("Calculating Frame: %d of %d", iMap + 1, m_nrMaps);
    if (pd.Update(iMap, msg.c_str()) == false)
    {
      m_aborted = true;
      break;
    }


    int32_t mapWidth = 0;
    int32_t mapHeight = 0;

    GetMapLatLon(zfxy, mapWidth, mapHeight, varLat, varLon);

    // Get data unit
    CUnit* unit = new CUnit(zfxy->GetUnit(fieldName));
    m_dataUnits.Insert(unit);

    // Get data title
    string dataTitle = zfxy->GetTitle(fieldName);
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
      string msg = CTools::Format("CGeoMap - variable '%s' is not a kind of Data (%d) : %s",
                                   fieldName.c_str(), Data, CNetCDFFiles::VarKindToString(varKind).c_str());
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
    }

    zfxy->GetVarDims(fieldName, varDimVal);

    if (varDimVal.size() != 2)
    {
      string msg = CTools::Format("CGeoMap - variable '%s -> number of dimensions value not equal 2 : %ld",
                                   fieldName.c_str(), (long)varDimVal.size());
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
    }

    zfxy->GetVarDims(fieldName, varDimNames);

    if (varDimNames.size() != 2)
    {
      string msg = CTools::Format("CGeoMap - variable '%s -> number of dimensions name not equal 2 : %ld",
                                   fieldName.c_str(), (long)varDimNames.size());
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
    }

    if ( (zfxy->GetVarKind(varDimNames.at(0)) == Latitude) &&
         (zfxy->GetVarKind(varDimNames.at(1)) == Longitude) )
    {
      firstDimIsLat = true;
    }
    else if ( (zfxy->GetVarKind(varDimNames.at(0)) == Longitude) &&
              (zfxy->GetVarKind(varDimNames.at(1)) == Latitude) )
    {
      firstDimIsLat = false;
    }
    else
    {
      string msg = CTools::Format("CGeoMap - Inconsistency kind of axis : varDimNames|0] = '%s' (%s) and "
                                   "varDimNames|1] = '%s' (%s)",
                                   varDimNames.at(0).c_str(), CNetCDFFiles::VarKindToString(zfxy->GetVarKind(varDimNames.at(0))).c_str(),
                                   varDimNames.at(1).c_str(), CNetCDFFiles::VarKindToString(zfxy->GetVarKind(varDimNames.at(0))).c_str());
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);

    }
    maxX = mapWidth;
    maxY = mapHeight;


    vtkDoubleArray* values = vtkDoubleArray::New();
    vtkShortArray* bitarray = vtkShortArray::New();
    vtkShortArray* validMercatorLatitudes = vtkShortArray::New();
    vtkDoubleArray* longitudes = vtkDoubleArray::New();
    vtkDoubleArray* latitudes = vtkDoubleArray::New();

/*
    for (iLat = 0 ; iLat < m_mapHeight ; iLat++)
    {
      for (iLon = 0 ; iLon < m_mapWidth ; iLon++)
      {
*/

    for (iY = 0 ; iY < maxY ; iY++)
    {
      latitudes->InsertNextValue(varLat.GetValues()[iY]);
      for (iX = 0 ; iX < maxX ; iX++)
      {
        int32_t iTemp;
        if (firstDimIsLat)
        {
          iTemp = (iY * maxX) + iX;
        }
        else
        {
          iTemp = (iX * maxY) + iY;
        }

        int16_t validLatitude = (CTools::AreValidMercatorLatitude(varLat.GetValues()[iY]) ? 1 : 0);
        validMercatorLatitudes->InsertNextValue(validLatitude);

        double v = varValue.GetValues()[iTemp];
        if ( (CTools::IsNan(v) != 0) || CTools::IsDefaultValue(v))
        {

          values->InsertNextValue(0);
          bitarray->InsertNextValue(0);
          //bitarray->InsertNextValue(1);
        }
        else
        {
          //???if self.property.value_conversion: v = self.property.value_conversion(v)
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

    if (CTools::IsDefaultValue(minHeightValue))
    {
      minHeightValue = m_minhv;
    }
    if (CTools::IsDefaultValue(maxHeightValue))
    {
      maxHeightValue = m_maxhv;
    }

    for (iX = 0 ; iX < maxX ; iX++)
    {
      longitudes->InsertNextValue(CTools::NormalizeLongitude(-180.0,varLon.GetValues()[iX]));
      //longitudes->InsertNextValue(varLon.GetValues()[iX]);
    }


    geoMapFilter = vtkGeoMapFilter::New();
    geoMapFilter->SetValues(values);

    geoMapFilter->SetHeights(values);
    geoMapFilter->SetBitarray(bitarray);
    geoMapFilter->SetValidMercatorLatitudes(validMercatorLatitudes);
    geoMapFilter->SetLongitudes(longitudes);
    geoMapFilter->SetLatitudes(latitudes);

    // Compute offset to center points in their cells
    double longitudeOffset = CTools::Abs((longitudes->GetValue(1) - longitudes->GetValue(0)) / 2);
    double latitudeOffset = CTools::Abs((latitudes->GetValue(1) - latitudes->GetValue(0)) / 2);
    geoMapFilter->SetOffsetLatitude(latitudeOffset);
    geoMapFilter->SetOffsetLongitude(longitudeOffset);

    geoMapFilter->SetFactor(m_plotProperty.m_heightFactor);
    geoMapFilter->SetMapWidth(mapWidth);
    geoMapFilter->SetMapHeight(mapHeight);

    m_geoMapFilterList.Insert(geoMapFilter);

    values->Delete();
    bitarray->Delete();
    validMercatorLatitudes->Delete();
    longitudes->Delete();
    latitudes->Delete();
  }


  if (CTools::IsDefaultValue(m_plotProperty.m_minContourValue))
  {
    m_plotProperty.m_minContourValue = minHeightValue;
  }

  if (CTools::IsDefaultValue(m_plotProperty.m_maxContourValue))
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

    geoMapFilter = dynamic_cast<vtkGeoMapFilter*>(m_geoMapFilterList[iMap]);
    //geoMapFilter = dynamic_cast<vtkVelocityGlyphFilter*>(m_geoMapFilterList[iMap]);

    geoMapFilter->SetMinMappedValue(minHeightValue);
    geoMapFilter->SetMaxMappedValue(maxHeightValue);
  }


  m_LUT->GetLookupTable()->SetTableRange(minHeightValue, maxHeightValue);

  m_colorBarRenderer->SetLUT(m_LUT);


  geoMapFilter = dynamic_cast<vtkGeoMapFilter*>(m_geoMapFilterList[m_currentMap]);
  //geoMapFilter = dynamic_cast<vtkVelocityGlyphFilter*>(m_geoMapFilterList[m_currentMap]);
  if (geoMapFilter == NULL)
  {
    throw CException(CTools::Format("ERROR in CGeoMap ctor  : dynamic_cast<vtkGeoMapFilter*>(m_geoMapFilterList[%d]) returns NULL pointer - "
                     "m_geoMapFilterList list seems to contain objects other than those of the class vtkGeoMapFilter", m_currentMap),
                     BRATHL_LOGIC_ERROR);

  }

  SetInput(geoMapFilter->GetOutput());


}


//----------------------------------------
void CGeoMap::SetProjection(int32_t proj)
{

  CWorldPlotData::SetProjection(proj);
  //return;

  uint32_t iMap;
  vtkGeoMapFilter* geoMapFilter = NULL;

  for (iMap = 0 ; iMap < m_nrMaps ; iMap++)
  {
    geoMapFilter = dynamic_cast<vtkGeoMapFilter*>(m_geoMapFilterList[iMap]);
    if (geoMapFilter == NULL)
    {
      continue;
    }
    geoMapFilter->SetProjection(proj);
  }


}
//----------------------------------------
void CGeoMap::OnKeyframeChanged(uint32_t i)
{
  if (m_finished)
  {
    return;
  }
  if (i >= m_geoMapFilterList.size())
  {
    return;
  }

  if ( i != m_currentMap)
  {
    m_currentMap = i;
    vtkGeoMapFilter* geoMapFilter = dynamic_cast<vtkGeoMapFilter*>(m_geoMapFilterList[m_currentMap]);
    if (geoMapFilter == NULL)
    {
      throw CException(CTools::Format("ERROR in CGeoMap ctor ::SetProjection : dynamic_cast<vtkGeoMapFilter*>(m_geoMapFilterList[%d]) returns NULL pointer - "
                       "m_geoMapFilterList list seems to contain objects other than those of the class vtkGeoMapFilter", m_currentMap),
                       BRATHL_LOGIC_ERROR);

    }

    SetInput(geoMapFilter->GetOutput());

  }
}

//----------------------------------------
void CGeoMap::Close()
{
  m_finished= true;
}

//----------------------------------------
void CGeoMap::Update()
{
  if ((m_is3D == true) && (m_vtkActor != NULL))
  {
    m_vtkActor->GetProperty()->SetOpacity(m_plotProperty.m_opacity);
    m_vtkActor->GetProperty()->BackfaceCullingOn();
  }
  else if ((m_is3D == false) && (m_vtkActor2D != NULL))
  {
    m_vtkActor2D->GetProperty()->SetOpacity(m_plotProperty.m_opacity);
  }

}
//----------------------------------------
void CGeoMap::GetMapLatLon(CInternalFilesZFXY* zfxy,
                           int32_t& width, int32_t& height,
                           CExpressionValue& varLat, CExpressionValue& varLon)
{
  CStringArray axisNames;
  CStringArray::iterator is;
  ExpressionValueDimensions dimVal;

  width = -1;
  height = -1;

  zfxy->GetAxisVars(axisNames);

  for (is = axisNames.begin(); is != axisNames.end(); is++)
  {
    zfxy->GetVarDims(*is, dimVal);
    if (dimVal.size() != 1)
    {
      string msg = CTools::Format("CGeoMap::GetMapLatLon - '%s' axis -> number of dimensions not equal 1 : %ld",
                                   (*is).c_str(), (long)dimVal.size());
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
    }
    NetCDFVarKind varKind = zfxy->GetVarKind(*is);
    if (varKind == Longitude)
    {
      width = dimVal.at(0);
      zfxy->ReadVar(*is, varLon, CLatLonPoint::m_DEFAULT_UNIT_LONGITUDE);
    }
    else if (varKind == Latitude)
    {
      height = dimVal.at(0);
      zfxy->ReadVar(*is, varLat, CLatLonPoint::m_DEFAULT_UNIT_LATITUDE);
    }
  }

  if (width == -1)
    {
      string msg = CTools::Format("CGeoMap::GetMapLatLon - Longitude axis not found in input file '%s'",
                                   zfxy->GetName().c_str());
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
    }

  if (height == -1)
    {
      string msg = CTools::Format("CGeoMap::GetMapLatLon - Latitude axis not found in input file '%s'",
                                   zfxy->GetName().c_str());
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
    }

}

//----------------------------------------
string CGeoMap::GetDataDateString(uint32_t index)
{

  if (index >= m_dataDates.size())
  {
    return "";
    /*
    CException e(CTools::Format("CWorldPlotData GetDataDateString - index %d out-of-range - Max. index is %d",
                                index, m_dataDates.size()),
                 BRATHL_LOGIC_ERROR);
    CTrace::Tracer(e.what());
    throw (e);
    */

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
string CGeoMap::GetDataUnitString(uint32_t index)
{
  if (index >= m_dataUnits.size())
  {
    return "";
    /*
    CException e(CTools::Format("CWorldPlotData GetDataUnitString - index %d out-of-range - Max. index is %d",
                                index, m_dataUnits.size()),
                 BRATHL_LOGIC_ERROR);
    CTrace::Tracer(e.what());
    throw (e);
    */
  }

  CUnit* dataUnit = dynamic_cast<CUnit*>(m_dataUnits[index]);

  return dataUnit->GetText();
}
//----------------------------------------
void CGeoMap::Set3D()
{
  SetSolidColor3D();
  SetContour3D();
}

//----------------------------------------
void CGeoMap::SetSolidColor3D()
{
  if (m_plotProperty.m_solidColor)
  {
    CWorldPlotData::Set3D();
  }

}
//----------------------------------------
void CGeoMap::SetContour3D()
{
  DeleteContour3D();

  if (m_plotProperty.m_withContour == false)
  {
    return;
  }

  vtkGeoMapFilter* geoMapFilter = dynamic_cast<vtkGeoMapFilter*>(m_geoMapFilterList[m_currentMap]);
  if (geoMapFilter == NULL)
  {
    throw CException(CTools::Format("ERROR in CGeoMap::Set3D ::SetProjection : dynamic_cast<vtkGeoMapFilter*>(m_geoMapFilterList[%d]) returns NULL pointer - "
                     "m_geoMapFilterList list seems to contain objects other than those of the class vtkGeoMapFilter", m_currentMap),
                     BRATHL_LOGIC_ERROR);

  }

  vtkCellDataToPointData* c2p = vtkCellDataToPointData::New();
  c2p->SetInput(GetVtkFilter()->GetOutput());


  DeleteContourFilter();
  m_vtkContourFilter = vtkContourFilter::New();

  m_vtkContourFilter->SetInput(c2p->GetOutput());

  ContourGenerateValues();


  c2p->Delete();
  c2p = NULL;


  m_vtkContourMapper = vtkPolyDataMapper::New();
  m_vtkContourMapper->SetInput(m_vtkContourFilter->GetOutput());
  //m_vtkContourMapper->SetScalarRange(minValue, maxValue);
  m_vtkContourMapper->ScalarVisibilityOff();

  m_vtkContourActor = vtkActor::New();
  m_vtkContourActor->SetMapper(m_vtkContourMapper);

  SetContour3DProperties();

  CreateContourLabels();

}
//----------------------------------------
void CGeoMap::Set2D()
{
  SetSolidColor2D();
  SetContour2D();
}
//----------------------------------------
void CGeoMap::SetSolidColor2D()
{
  if (m_plotProperty.m_solidColor)
  {
    CWorldPlotData::Set2D();
  }

}
//----------------------------------------
void CGeoMap::SetContour2D()
{
  DeleteContour2D();

  if (m_plotProperty.m_withContour == false)
  {
    return;
  }


  vtkGeoMapFilter* geoMapFilter = dynamic_cast<vtkGeoMapFilter*>(m_geoMapFilterList[m_currentMap]);
  if (geoMapFilter == NULL)
  {
    throw CException(CTools::Format("ERROR in CGeoMap::Set2D ::SetProjection : dynamic_cast<vtkGeoMapFilter*>(m_geoMapFilterList[%d]) returns NULL pointer - "
                     "m_geoMapFilterList list seems to contain objects other than those of the class vtkGeoMapFilter", m_currentMap),
                     BRATHL_LOGIC_ERROR);

  }

  m_vtkTransform->SetInput(GetVtkFilter()->GetOutput());
  m_vtkTransform->SetTransform(m_transform);

  vtkCellDataToPointData* c2p = vtkCellDataToPointData::New();
  c2p->SetInput(m_vtkTransform->GetOutput());

  DeleteContourFilter();
  m_vtkContourFilter = vtkContourFilter::New();

  m_vtkContourFilter->SetInput(c2p->GetOutput());

  ContourGenerateValues();


  c2p->Delete();
  c2p = NULL;


  vtkCoordinate* c = vtkCoordinate::New();
  c->SetCoordinateSystemToNormalizedViewport();


  m_vtkContourMapper2D = vtkPolyDataMapper2D::New();
  m_vtkContourMapper2D->SetInput(m_vtkContourFilter->GetOutput());
  m_vtkContourMapper2D->SetTransformCoordinate(c);

  m_vtkContourMapper2D->ScalarVisibilityOff();

  m_vtkContourActor2D = vtkActor2D::New();
  m_vtkContourActor2D->SetMapper(m_vtkContourMapper2D);

  SetContour2DProperties();


  c->Delete();

  CreateContourLabels2D();
}
//----------------------------------------
void CGeoMap::SetContourLabelProperties()
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
void CGeoMap::ContourGenerateValues()
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
void CGeoMap::SetContour3DProperties()
{
  if (m_vtkContourActor == NULL)
  {
    return;
  }

  m_vtkContourActor->GetProperty()->SetColor(m_plotProperty.m_contourLineColor.Red(),
                                             m_plotProperty.m_contourLineColor.Green(),
                                             m_plotProperty.m_contourLineColor.Blue()
                                             );

  m_vtkContourActor->GetProperty()->SetLineWidth(m_plotProperty.m_contourLineWidth);

}
//----------------------------------------
void CGeoMap::SetContour2DProperties()
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
void CGeoMap::CreateContourLabels()
{
  long numpoints =0;

  if (m_plotProperty.m_withContour == false)
  {
    return;
  }

  numpoints = m_vtkContourFilter->GetOutput()->GetNumberOfPoints();
  if ( numpoints <= 0 )
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
  m_vtkConnectivity->ScalarConnectivityOff();
  m_vtkConnectivity->SetExtractionModeToSpecifiedRegions();

  m_vtkLabelContourPositions->SetPoints(m_vtkLabelContourPoints);
  m_vtkLabelContourPositions->GetPointData()->SetScalars(m_vtkLabelContourData);

  m_vtkVisiblePointsFilter->SetRenderer(m_vtkRend);
  m_vtkVisiblePointsFilter->SetInput(m_vtkLabelContourPositions);

  m_vtkVisibleSpherePointsFilter->SetInput(m_vtkVisiblePointsFilter->GetOutput());
  m_vtkVisibleSpherePointsFilter->SetExecuteMethod(CGeoMap::FindVisibleSpherePoints, static_cast<void*>(this));

  m_vtkContourLabelMapper->SetInput(m_vtkVisibleSpherePointsFilter->GetPolyDataOutput());

  m_vtkContourLabelActor = vtkActor2D::New();

  m_vtkContourLabelActor->SetMapper(m_vtkContourLabelMapper)  ;

  UpdateContourLabels();

}
//----------------------------------------
void CGeoMap::UpdateContourLabels()
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

  //Labels every contour.
  //If the data min/max is changed, this must be called manually to update the labels.

  m_vtkConnectivity->InitializeSpecifiedRegionList();
  m_vtkConnectivity->Update();
  int32_t nContours = m_vtkConnectivity->GetNumberOfExtractedRegions();

  //nlabels = int(self.options['Number of labels per contour']);

  int32_t nLabels = m_plotProperty.m_numContourLabel;


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

        labelPoints->InsertPoint(insertctr, pts->GetPoint(tpl));

        m_vtkLabelContourData->InsertValue(insertctr, data->GetTuple1(tpl));
        insertctr++;
      }
    }
  }


  //need to make render window update with new labels
  m_vtkLabelContourPositions->Modified();

  wxSetCursor(wxNullCursor);

}
//----------------------------------------
void CGeoMap::FindVisibleSpherePoints(void* arg)
{
  CGeoMap* geoMap = static_cast<CGeoMap*>(arg);
  if (geoMap == NULL)
  {
    return;
  }

  // Finds visible points on sphere based on cosine angle of points
  // relative to camera view vector.
  // Works on assumption that sphere origin is world coord (0,0,0).
  vtkPolyData* input = geoMap->GetVtkVisibleSpherePointsFilter()->GetPolyDataInput();
  vtkPolyData* output = geoMap->GetVtkVisibleSpherePointsFilter()->GetPolyDataOutput();
  vtkPoints* pts = input->GetPoints();
  int32_t npts = pts->GetNumberOfPoints();
  vtkDataArray* data = input->GetPointData()->GetScalars();
  geoMap->GetVtkVisibleSpherePoints()->Reset();
  geoMap->GetVtkVisibleSpherePointsData()->Reset();

  double* cameraPosition = geoMap->GetVtkRenderer()->GetActiveCamera()->GetPosition();
  double* cameraFocalPoint = geoMap->GetVtkRenderer()->GetActiveCamera()->GetFocalPoint();

  double fp[3];
  fp[0] =  cameraFocalPoint[0] - cameraPosition[0];
  fp[1] =  cameraFocalPoint[1] - cameraPosition[1];
  fp[2] =  cameraFocalPoint[2] - cameraPosition[2];

  for (int32_t i = 0 ; i < npts ; i++)
  {
    vtkFloatingPointType* xyz = input->GetPoint(i);
    // negative cosine angle denotes point normal facing viewer
    if (vtkMath::Dot(xyz, fp) < 0)
    {
      geoMap->GetVtkVisibleSpherePoints()->InsertNextPoint(xyz);
      geoMap->GetVtkVisibleSpherePointsData()->InsertNextValue(data->GetTuple1(i));
    }
  }
  output->CopyStructure(input);
  output->SetPoints(geoMap->GetVtkVisibleSpherePoints());
  output->GetPointData()->SetScalars(geoMap->GetVtkVisibleSpherePointsData());

}
//----------------------------------------
void CGeoMap::FindVisiblePlanePoints(void* arg)
{
  CGeoMap* geoMap = static_cast<CGeoMap*>(arg);
  if (geoMap == NULL)
  {
    return;
  }

  // Finds visible points on sphere based on cosine angle of points
  // relative to camera view vector.
  // Works on assumption that sphere origin is world coord (0,0,0).
  vtkPolyData* input = geoMap->GetVtkVisibleSpherePointsFilter()->GetPolyDataInput();
  vtkPolyData* output = geoMap->GetVtkVisibleSpherePointsFilter()->GetPolyDataOutput();
  vtkPoints* pts = input->GetPoints();
  int32_t npts = pts->GetNumberOfPoints();
  vtkDataArray* data = input->GetPointData()->GetScalars();
  geoMap->GetVtkVisibleSpherePoints()->Reset();
  geoMap->GetVtkVisibleSpherePointsData()->Reset();

  //vtkCoordinate* c = vtkCoordinate::New();
  //c->SetCoordinateSystemToNormalizedViewport();

  for (int32_t i = 0 ; i < npts ; i++)
  {
    vtkFloatingPointType* xyz = input->GetPoint(i);


    geoMap->GetVtkVisibleSpherePoints()->InsertNextPoint(xyz);
    //geoMap->GetVtkVisibleSpherePoints()->InsertNextPoint(xyzViewPort);
    geoMap->GetVtkVisibleSpherePointsData()->InsertNextValue(data->GetTuple1(i));
  }
  output->CopyStructure(input);
  output->SetPoints(geoMap->GetVtkVisibleSpherePoints());
  output->GetPointData()->SetScalars(geoMap->GetVtkVisibleSpherePointsData());

}

//----------------------------------------
void CGeoMap::CreateContourLabels2D()
{
  if (m_plotProperty.m_withContour == false)
  {
    return;
  }

  //m_vtkContourFilter->Update();
  NewVtkContourLabelObject();

  //Must be called after adding all actors to the renderer which will
  //make some of these labels invisible, i.e. a sphere's actor will
  //hide those labels on the rear half of the sphere.

  m_vtkContourLabelMapper = vtkLabeledDataMapper::New();

  SetContourLabelProperties();

  m_vtkConnectivity->SetInput(m_vtkContourFilter->GetOutput());
  m_vtkConnectivity->ScalarConnectivityOff();
  m_vtkConnectivity->SetExtractionModeToSpecifiedRegions();

  m_vtkLabelContourPositions->SetPoints(m_vtkLabelContourPoints);
  m_vtkLabelContourPositions->GetPointData()->SetScalars(m_vtkLabelContourData);


  m_vtkVisiblePointsFilter->SetRenderer(m_vtkRend);
  m_vtkVisiblePointsFilter->SetInput(m_vtkLabelContourPositions);


  m_vtkVisibleSpherePointsFilter->SetInput(m_vtkVisiblePointsFilter->GetOutput());
  m_vtkVisibleSpherePointsFilter->SetExecuteMethod(CGeoMap::FindVisiblePlanePoints, static_cast<void*>(this));

 // m_vtkContourLabelMapper->SetInput(m_vtkLabelContourPositions);
  m_vtkContourLabelMapper->SetInput(m_vtkVisibleSpherePointsFilter->GetPolyDataOutput());

  m_vtkContourLabelActor = vtkActor2D::New();

  m_vtkContourLabelActor->SetMapper(m_vtkContourLabelMapper);

  UpdateContourLabels2D();

}

//----------------------------------------
void CGeoMap::UpdateContourLabels2D()
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

  //Labels every contour.
  //If the data min/max is changed, this must be called manually to update the labels.

  m_vtkConnectivity->InitializeSpecifiedRegionList();
  m_vtkConnectivity->Update();
  int32_t nContours = m_vtkConnectivity->GetNumberOfExtractedRegions();
  //int32_t nContours = m_vtkContourFilter->GetOutput()->GetNumberOfLines();

  //nlabels = int(self.options['Number of labels per contour']);

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
  //int32_t* iValue = NULL;
  //vtkCoordinate* c = m_vtkMapper2D->GetTransformCoordinate();
  vtkCoordinate* c = vtkCoordinate::New();
  c->SetCoordinateSystemToNormalizedViewport();

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

        //labelPoints->InsertPoint(insertctr, pts->GetPoint(tpl));
        labelPoints->InsertPoint(insertctr, xyzViewPort);

        m_vtkLabelContourData->InsertValue(insertctr, data->GetTuple1(tpl));
        insertctr++;
      }
    }
  }

  c->Delete();

  //need to make render window update with new labels
  m_vtkLabelContourPositions->Modified();

  wxSetCursor(wxNullCursor);
  m_contourLabelNeedUpdateOnWindow = false;
}


//----------------------------------------
void CGeoMap::SetContourLabels2DPosition()
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
  //int32_t* iValue = NULL;
  //vtkCoordinate* c = m_vtkMapper2D->GetTransformCoordinate();
  vtkCoordinate* c = vtkCoordinate::New();
  c->SetCoordinateSystemToNormalizedViewport();

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

        //labelPoints->InsertPoint(insertctr, pts->GetPoint(tpl));
        labelPoints->InsertPoint(insertctr, xyzViewPort);

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

//----------------------------------------
void CGeoMap::Update2D()
{
  UpdateContourLabels2D();
  //SetContourLabels2DPosition();
}







//-------------------------------------------------------------
//------------------- CGeoVelocityMap class --------------------
//-------------------------------------------------------------

//----------------------------------------

CGeoVelocityMap::CGeoVelocityMap(wxWindow* parent, CPlotField* fieldNorth, CPlotField* fieldEast)
        : CGeoMap(parent, fieldNorth->m_worldProps) // this is just to compile

{
  m_simple2DProjFilter = vtkProj2DFilter::New();

  // this is just to avoid interpolation and generation of intermediate points
  GetVtkFilter()->SetInterpolationDistance(10);
/*
  IsGlyph = false;
  m_headSource = NULL;
  m_barrowSource = NULL;
  m_arrowSource = NULL;
  m_glyph = NULL;
 m_headGlyph = NULL;
 */
  IsGlyph = false;
    m_arrowSource = NULL;
    m_glyph = NULL;

  Create(parent, &(fieldNorth->m_internalFiles), &(fieldEast->m_internalFiles), (const char *)(fieldNorth->m_name),
         (const char *)(fieldEast->m_name));
}
//----------------------------------------
CGeoVelocityMap::CGeoVelocityMap(wxWindow* parent, CObArray* northData, CObArray* eastData, const string& fieldNameNorth, const string& fieldNameEast, CWorldPlotProperty* plotProperty)
        : CGeoMap(parent, plotProperty) // this is just to compile

{

    m_simple2DProjFilter = vtkProj2DFilter::New();//vtkSimpleLineProjFilter::New();

    // this is just to avoid interpolation and generation of intermediate points
    GetVtkFilter()->SetInterpolationDistance(10);
/*
    IsGlyph = false;
    m_headSource = NULL;
    m_barrowSource = NULL;
    m_glyph = NULL;
    m_headGlyph = NULL;
*/
    IsGlyph = false;
    m_arrowSource = NULL;
    m_glyph = NULL;

    Create(parent, northData, eastData, fieldNameNorth, fieldNameEast);
}
//----------------------------------------
CGeoVelocityMap::~CGeoVelocityMap()
{
    /*if ( m_simple2DProjFilter )
    m_simple2DProjFilter->Delete();

    if ( m_headSource )
        m_headSource->Delete();

    if ( m_barrowSource )
        m_barrowSource->Delete();

    if ( m_headGlyph )
        m_headGlyph->Delete();

    if ( m_glyph )
        m_glyph->Delete();
        */

    if ( m_simple2DProjFilter )
    m_simple2DProjFilter->Delete();
    if ( m_arrowSource )
        m_arrowSource->Delete();
    if ( m_glyph )
        m_glyph->Delete();
}

//----------------------------------------
void CGeoVelocityMap::Create(wxWindow* parent, CObArray* northData, CObArray* eastData, const string& fieldNameNorth, const string& fieldNameEast)
{

  m_contourLabelNeedUpdateOnWindow = false;
  m_contourLabelNeedUpdatePositionOnContour = false;

  //if (m_plotProperty.m_name.IsEmpty())
  //{
    m_plotProperty.m_name = wxString(fieldNameEast.c_str()) + wxString("/") + wxString(fieldNameNorth.c_str());
  //}

  m_colorBarRenderer = new CLUTRenderer();
  m_colorBarRenderer->GetVtkRenderer()->InteractiveOff();

  m_colorBarRenderer->SetNumberOfLabels(m_plotProperty.m_numColorLabels);

  m_finished = false;
  m_currentMap = 0;

  // check both are of the same size
  m_nrMaps = northData->size();
  if ( northData->size() != eastData->size() )
  {
      CException e("CGeoVelocityMap - North and East data of different size!",
                   BRATHL_LOGIC_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
  }

  wxProgressDialog pd("Calculating Geomap Data", "", m_nrMaps, parent, wxPD_CAN_ABORT | wxPD_REMAINING_TIME | wxPD_SMOOTH| wxPD_APP_MODAL);
  pd.SetSize(-1,-1, 350, -1);

  bool firstValue = true;

  uint32_t iMap = 0;
  int32_t iX = 0;
  int32_t iY = 0;
  int32_t maxX = 0;
  int32_t maxY = 0;
  bool firstDimIsLat = false;

  double minHeightValue = 0;
  double maxHeightValue = 0;

  vtkVelocityGlyphFilter* geoMapFilter = NULL;
  for (iMap = 0 ; iMap < m_nrMaps ; iMap++)
  {
    minHeightValue = m_plotProperty.m_minHeightValue;
    maxHeightValue = m_plotProperty.m_maxHeightValue;

    CExpressionValue varLat;
    CExpressionValue varLon;
    CExpressionValue varValueNorth;
    CExpressionValue varValueEast;
    NetCDFVarKind varKindNorth;
    NetCDFVarKind varKindEast;
    ExpressionValueDimensions varDimValNorth;
    ExpressionValueDimensions varDimValEast;
    CStringArray varDimNamesNorth;
    CStringArray varDimNamesEast;


    // check the file for North data
    CInternalFilesZFXY* zfxy = dynamic_cast<CInternalFilesZFXY*>(northData->at(iMap));
    CInternalFilesZFXY* zfxyEast = dynamic_cast<CInternalFilesZFXY*>(eastData->at(iMap));


    if (zfxy == NULL || zfxy == NULL)
    {
      CException e("CGeoVelocityMap ctor - Non-geographical data found - dynamic_cast<CInternalFilesZFXY*>data->at(iMap) returns NULL",
                   BRATHL_LOGIC_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
    }


    if (zfxy->IsGeographic() == false || zfxyEast->IsGeographic() == false )
    {
      CException e("CGeoVelocityMap ctor - Non-geographical data found - IsGeographic() is false",
                   BRATHL_LOGIC_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
    }

    //update ProgressDialog
    string msg = CTools::Format("Calculating Frame: %d of %d", iMap + 1, m_nrMaps);
    if (pd.Update(iMap, msg.c_str()) == false)
    {
      m_aborted = true;
      break;
    }


    int32_t mapWidth = 0;
    int32_t mapHeight = 0;

    GetMapLatLon(zfxy, mapWidth, mapHeight, varLat, varLon);

    // Get data units
    CUnit* unitNorth = new CUnit(zfxy->GetUnit(fieldNameNorth));
    CUnit* unitEast = new CUnit(zfxyEast->GetUnit(fieldNameEast));


    if ( *unitNorth != *unitEast )
    {
        string msg = CTools::Format("CGeoVelocityMap ctor - Unequal Units (%s != %s)", /*unitNorth->AsString(), unitEast->AsString()*/"d","d" );
        CException e(msg,
                   BRATHL_INCONSISTENCY_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
    }

    m_dataUnits.Insert(unitNorth);
    //m_dataUnits.Insert(unitEast);

    m_dataTitles.Insert(m_plotProperty.m_name.c_str() );


    // read data
    zfxy->ReadVar(fieldNameNorth, varValueNorth, unitNorth->GetText());
    varKindNorth	= zfxy->GetVarKind(fieldNameNorth);

    zfxy->ReadVar(fieldNameEast, varValueEast, unitEast->GetText());
    // this may need to be changed
    varKindEast	= zfxyEast->GetVarKind(fieldNameEast);


    if (varKindNorth != Data || varKindEast != varKindNorth)
    {
      string msg = CTools::Format("CGeoVelocityMap ctor - variables '%s', '%s' are not a kind of Data (%d) : %s",
                                   fieldNameNorth.c_str(), fieldNameEast.c_str(), Data, CNetCDFFiles::VarKindToString(varKindNorth).c_str());
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
    }


    zfxy->GetVarDims(fieldNameNorth, varDimValNorth);
    zfxyEast->GetVarDims(fieldNameEast, varDimValEast);

    if (varDimValNorth.size() != 2 || varDimValNorth.size() != varDimValEast.size() )
    {
      string msg = CTools::Format("CGeoVelocityMap ctor - variables '%s or %s -> number of dimensions value not equal 2 : %ld, %ld",
                                   fieldNameNorth.c_str(), fieldNameEast.c_str(), (long)varDimValNorth.size(), (long)varDimValEast.size());
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
    }

    zfxy->GetVarDims(fieldNameNorth, varDimNamesNorth);
    zfxyEast->GetVarDims(fieldNameEast, varDimNamesEast);

    if (varDimNamesNorth.size() != 2 || varDimNamesEast.size() != varDimNamesNorth.size())
    {
      string msg = CTools::Format("CGeoVelocityMap ctor - variable '%s  or %s-> number of dimensions name not equal 2 : %ld, %ld",
                                   fieldNameNorth.c_str(), fieldNameEast.c_str(), (long)varDimNamesNorth.size(), (long)varDimNamesEast.size());
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
    }

    // TODO (cornejo#1#): should we check this with the East velocity as well??

    if ( (zfxy->GetVarKind(varDimNamesNorth.at(0)) == Latitude) &&
         (zfxy->GetVarKind(varDimNamesNorth.at(1)) == Longitude) )
    {
      firstDimIsLat = true;
    }
    else if ( (zfxy->GetVarKind(varDimNamesNorth.at(0)) == Longitude) &&
              (zfxy->GetVarKind(varDimNamesNorth.at(1)) == Latitude) )
    {
      firstDimIsLat = false;
    }
    else
    {
      string msg = CTools::Format("CGeoVelocityMap ctor - Inconsistency kind of axis : varDimNames|0] = '%s' (%s) and "
                                   "varDimNames|1] = '%s' (%s)",
                                   varDimNamesNorth.at(0).c_str(), CNetCDFFiles::VarKindToString(zfxy->GetVarKind(varDimNamesNorth.at(0))).c_str(),
                                   varDimNamesNorth.at(1).c_str(), CNetCDFFiles::VarKindToString(zfxy->GetVarKind(varDimNamesNorth.at(0))).c_str());
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);

    }
    maxX = mapWidth;
    maxY = mapHeight;


    vtkDoubleArray* valuesNorth = vtkDoubleArray::New();
    vtkDoubleArray* valuesEast = vtkDoubleArray::New();
    vtkShortArray* bitarray = vtkShortArray::New();
    vtkShortArray* validMercatorLatitudes = vtkShortArray::New();
    vtkDoubleArray* longitudes = vtkDoubleArray::New();
    vtkDoubleArray* latitudes = vtkDoubleArray::New();

    for (iY = 0 ; iY < maxY ; iY++)
    {
      latitudes->InsertNextValue(varLat.GetValues()[iY]);
      for (iX = 0 ; iX < maxX ; iX++)
      {
        int32_t iTemp;
        if (firstDimIsLat)
        {
          iTemp = (iY * maxX) + iX;
        }
        else
        {
          iTemp = (iX * maxY) + iY;
        }

        int16_t validLatitude = (CTools::AreValidMercatorLatitude(varLat.GetValues()[iY]) ? 1 : 0);
        validMercatorLatitudes->InsertNextValue(validLatitude);

        double vN = varValueNorth.GetValues()[iTemp];
        double vE = varValueEast.GetValues()[iTemp];

        // invalid values?
        if ( (CTools::IsNan(vN) != 0) || (CTools::IsNan(vE) != 0) ||
                        CTools::IsDefaultValue(vN) || CTools::IsDefaultValue(vE))
        {

          valuesNorth->InsertNextValue(0);
          valuesEast->InsertNextValue(0);
          bitarray->InsertNextValue(0);
          //bitarray->InsertNextValue(1);
        }
        else
        {
            double magnitude = sqrt(vE*vE + vN*vN);

          //???if self.property.value_conversion: v = self.property.value_conversion(v)
          valuesNorth->InsertNextValue(vN);
          valuesEast->InsertNextValue(vE);

          bitarray->InsertNextValue(1);

          // this is just to figure out minimums and maximums
	      if (firstValue)
          {
            firstValue = false;
            m_minhvN = vN;
            m_maxhvN = vN;

            m_minhvE = vE;
            m_maxhvE = vE;

            m_maxhv = magnitude;
            m_minhv = magnitude;

          }
          else
          {
            if (vN < m_minhvN)
            {
              m_minhvN = vN;
            }
            if (vN > m_maxhvN)
            {
              m_maxhvN = vN;
            }


            if (vE < m_minhvE)
            {
              m_minhvE = vE;
            }
            if (vE > m_maxhvE)
            {
              m_maxhvE = vE;
            }

            if (magnitude < m_minhv)
            {
              m_minhv = magnitude;
            }
            if (magnitude > m_maxhv)
            {
              m_maxhv = magnitude;
            }

          }
        }
      }
    }

    if (CTools::IsDefaultValue(minHeightValue))
    {
      minHeightValue = m_minhvN < m_minhvE ? m_minhvN : m_minhvE;
    }
    if (CTools::IsDefaultValue(maxHeightValue))
    {
      maxHeightValue = m_minhvN > m_minhvE ? m_minhvN : m_minhvE;
    }

    for (iX = 0 ; iX < maxX ; iX++)
    {
      longitudes->InsertNextValue(CTools::NormalizeLongitude(-180.0,varLon.GetValues()[iX]));
      //longitudes->InsertNextValue(varLon.GetValues()[iX]);
    }

    geoMapFilter = vtkVelocityGlyphFilter::New();
    geoMapFilter->SetValuesNorth(valuesNorth);
    geoMapFilter->SetValuesEast(valuesEast);

    geoMapFilter->SetMaxVN(m_maxhvN);
    geoMapFilter->SetMinVN(m_minhvN);
    geoMapFilter->SetMaxVE(m_maxhvE);
    geoMapFilter->SetMinVE(m_minhvE);
    //geoMapFilter->SetMinV(m_minhv);
    geoMapFilter->SetMaxV(m_maxhv);

    // TODO (cornejo#1#): what should we use here.  Magnitude??
    geoMapFilter->SetHeights(valuesNorth);

    geoMapFilter->SetBitarray(bitarray);
    geoMapFilter->SetValidMercatorLatitudes(validMercatorLatitudes);
    geoMapFilter->SetLongitudes(longitudes);
    geoMapFilter->SetLatitudes(latitudes);

    //geoMapFilter->SetOffsetLatitude(1);
    //geoMapFilter->SetOffsetLongitude(1);

    geoMapFilter->SetFactor(m_plotProperty.m_heightFactor);
    geoMapFilter->SetMapWidth(mapWidth);
    geoMapFilter->SetMapHeight(mapHeight);

    m_geoMapFilterList.Insert(geoMapFilter);

    valuesNorth->Delete();
    valuesEast->Delete();

    bitarray->Delete();
    validMercatorLatitudes->Delete();
    longitudes->Delete();
    latitudes->Delete();
  }


  if (CTools::IsDefaultValue(m_plotProperty.m_minContourValue))
  {
    m_plotProperty.m_minContourValue = minHeightValue;
  }

  if (CTools::IsDefaultValue(m_plotProperty.m_maxContourValue))
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
    //geoMapFilter = dynamic_cast<vtkGeoMapFilter*>(m_geoMapFilterList[iMap]);
    vtkGeoMapFilter* gMapFilter = dynamic_cast<vtkGeoMapFilter*>(m_geoMapFilterList[iMap]);
    if ( gMapFilter == NULL )
    {
        continue;
    }

    gMapFilter->SetMinMappedValue(minHeightValue);
    gMapFilter->SetMaxMappedValue(maxHeightValue);
  }


  m_LUT->GetLookupTable()->SetTableRange(0, m_maxhv);
  m_LUT->Black();
  // no LUT displayed for this
  //m_colorBarRenderer->SetLUT(m_LUT);

  geoMapFilter = dynamic_cast<vtkVelocityGlyphFilter*>(m_geoMapFilterList[m_currentMap]);
  if (geoMapFilter == NULL)
  {
    throw CException(CTools::Format("ERROR in CGeoVelocityMap ctor  : dynamic_cast<vtkVelocityGlyphFilter*>(m_geoMapFilterList[%d]) returns NULL pointer - "
                     "m_geoMapFilterList list seems to contain objects other than those of the class vtkVelocityGlyphFilter", m_currentMap),
                     BRATHL_LOGIC_ERROR);
    return;
  }

 m_geoMapFilter = geoMapFilter;
 m_geoMapFilter->SetIsGlyph(IsGlyph);
 SetInput(geoMapFilter->GetOutput());

}


vtkProj2DFilter* CGeoVelocityMap::GetVtkFilter()
{
     return m_simple2DProjFilter;
}


void CGeoVelocityMap::SetIsGlyph(bool val)
{
    IsGlyph = val;
    m_geoMapFilter->SetIsGlyph(val);

    if ( Is3D() )
        Set3D();
    else
        Set2D();

}

void CGeoVelocityMap::Set2D()
{
    if ( IsGlyph )
        Set2DGlyphs();
    else
        CGeoMap::Set2D();
}

void CGeoVelocityMap::Set3D()
{

    if ( IsGlyph )
        Set3DGlyphs();
    else
        CGeoMap::Set3D();
}

void CGeoVelocityMap::Set2DGlyphs()
{

  m_is3D = false;
  vtkPolyData *projOutput =GetVtkFilter()->GetOutput();
  // this code is commented, but needed for glyphing
  m_pointMask = vtkMaskPoints::New();
  m_barrowSource = vtkBratArrowSource::New();

  m_vtkTransform->SetInput(projOutput);
  m_glyph = vtkGlyph3D::New();
  m_glyph->SetInput(m_vtkTransform->GetOutput());
  m_glyph->SetSource(m_barrowSource->GetOutput());
  m_glyph->SetScaleModeToScaleByVector();
  m_glyph->OrientOn();

  vtkCoordinate* c = vtkCoordinate::New();
  c->SetCoordinateSystemToNormalizedViewport();
  m_vtkMapper2D = vtkPolyDataMapper2D::New();
  m_vtkMapper2D->SetInput(m_glyph->GetOutput());
  m_vtkMapper2D->SetTransformCoordinate(c);
  m_vtkMapper2D->SetScalarModeToUseCellData();
  m_vtkMapper2D->UseLookupTableScalarRangeOn();

  m_vtkActor2D = vtkActor2D::New();
  m_vtkActor2D->SetMapper(m_vtkMapper2D);
  m_vtkActor2D->GetProperty()->SetColor(0.0,0,0);
  c->Delete();
  c = NULL;
  Update2D();
  SetContour2D();
}


void CGeoVelocityMap::Set3DGlyphs()
{

  m_is3D = true;
  vtkPolyData *projOutput =GetVtkFilter()->GetOutput();
  // this fixes returning to 3D...
  m_transform->Identity();
  m_vtkTransform->SetTransform(m_transform);
  m_vtkTransform->SetInput(projOutput);
  m_barrowSource = vtkBratArrowSource::New();

  m_glyph = vtkGlyph3D::New();
  m_glyph->SetInput(m_vtkTransform->GetOutput());
  m_glyph->SetSource(m_barrowSource->GetOutput());
  m_glyph->SetScaleModeToScaleByVector();
  m_glyph->OrientOn();
  m_glyph->SetScaleFactor(0.018);
  m_vtkMapper = vtkPolyDataMapper::New();
  m_vtkMapper->SetInput(m_glyph->GetOutput());
  m_vtkMapper->SetScalarModeToUseCellData();
  m_vtkMapper->UseLookupTableScalarRangeOn();

  m_vtkActor = vtkActor::New();
  m_vtkActor->SetMapper(m_vtkMapper);
  m_vtkActor->GetProperty()->SetColor(0.0,0.0,0.0);
  Update();
  SetContour3D();
}
