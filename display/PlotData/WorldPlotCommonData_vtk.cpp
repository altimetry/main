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

#include <ctime>

#include "new-gui/Common/tools/Trace.h"
#include "new-gui/Common/tools/Exception.h"
#include "libbrathl/LatLonPoint.h"
using namespace brathl;

#if defined (__DEPRECATED)          //avoid linux warning in vtk include
#undef __DEPRECATED
#endif
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
#include "vtkLookupTable.h"

#include "WorldPlotCommonData_vtk.h"
//#include "GeoMap.h"

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
void CLUTRenderer::ResetTextActor(CWorldPlotData* geoMap)
{
  std::string text = geoMap->GetDataTitle();
  text += "\t\t(" + geoMap->m_plotProperty.m_name;
  text += ")\t\t-\t\tUnit:\t" +  geoMap->GetDataUnitString();
  text += "\t" + geoMap->GetDataDateString();

  ResetTextActor(text);
}
//----------------------------------------
void CLUTRenderer::ResetTextActor(const std::string& text)
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
//------------------- CWorldPlotProperties class --------------------
//-------------------------------------------------------------

//CWorldPlotProperties::CWorldPlotProperties()
//{
//
//  //m_projection = "";
//  m_opacity = 0.7;
//  m_deltaRadius = 0.007;
//
//  m_showPropertyPanel = true;
//  m_showColorBar = true;
//  m_showAnimationToolbar = false;
//  //m_lineWidth = 1;
//  //m_pointSize = 2;
//  //m_color.Insert(0);
//  //m_color.Insert(0);
//  //m_color.Insert(1);
//
//  m_heightFactor = 0;
//  m_numColorLabels = 5;
//
//  //m_colorTable = NULL;
//
//  setDefaultValue(m_minHeightValue);
//  setDefaultValue(m_maxHeightValue);
//
//  //setDefaultValue(m_stipplepattern);
//  m_valueConversion = 0.0;
//  m_heightConversion = 0.0;
//
//  m_centerLongitude = 0.0;
//  m_centerLatitude = 0.0;
//
//  m_LUT = new CBratLookupTable();
//
//  m_LUT->ExecMethodDefaultColorTable();
//
//  m_zoom = false;
//  m_zoomLon1 = -180.0;
//  m_zoomLon2 = 180.0;
//  m_zoomLat1 = -90.0;
//  m_zoomLat2 = 90.0;
//
//  m_withContour = false;
//  m_solidColor = true;;
//  m_gridLabel = true;
//
//  m_withContourLabel = false;
//  setDefaultValue(m_minContourValue);
//  setDefaultValue(m_maxContourValue);
//  m_numContour = 5;
//  m_numContourLabel = 1;
//
//  m_contourLabelSize = 10;
//  m_contourLineWidth = 1.0;
//
//  m_contourLineColor.Set(0.0, 0.0, 0.0);
//  m_contourLineColor.Set(0.0, 0.0, 0.0);
//
//  m_contourLabelFormat = "%-#.3g";
//
//  m_withAnimation = false;
//
//  m_eastComponent = false;
//  m_northComponent = false;
//
//
//}
//
////----------------------------------------
//
//CWorldPlotProperties::CWorldPlotProperties(const CWorldPlotProperties& p)
//{
//  m_LUT = NULL;
//  this->Copy(p);
//
//}
//
////----------------------------------------
//const CWorldPlotProperties& CWorldPlotProperties::operator=(const CWorldPlotProperties& p)
//{
//  this->Copy(p);
//  return *this;
//}
////----------------------------------------
//CWorldPlotProperties::~CWorldPlotProperties()
//{
//  DeleteLUT();
//}
//
////----------------------------------------
//void CWorldPlotProperties::DeleteLUT()
//{
//  if (m_LUT != NULL)
//  {
//    delete m_LUT;
//    m_LUT = NULL;
//  }
//}
////----------------------------------------
//void CWorldPlotProperties::Copy(const CWorldPlotProperties& p)
//{
//
//  m_coastResolution = p.m_coastResolution;
//  m_projection = p.m_projection;
//
//  DeleteLUT();
//  m_LUT = new CBratLookupTable(*(p.m_LUT));
//
//  m_title = p.m_title;
//
//  m_name = p.m_name;
//
//  m_opacity = p.m_opacity;
//  m_deltaRadius = p.m_deltaRadius;
//  m_showPropertyPanel = p.m_showPropertyPanel;
//  m_showColorBar = p.m_showColorBar;
//  m_showAnimationToolbar = p.m_showAnimationToolbar;
//
//  m_heightFactor = p.m_heightFactor;
//  m_numColorLabels = p.m_numColorLabels;
//
//  m_minHeightValue = p.m_minHeightValue;
//  m_maxHeightValue = p.m_maxHeightValue;
//  m_valueConversion = p.m_valueConversion;
//  m_heightConversion = p.m_heightConversion;
//
//  m_centerLongitude = p.m_centerLongitude;
//  m_centerLatitude = p.m_centerLatitude;
//
//  m_zoom = p.m_zoom;
//  m_zoomLon1 = p.m_zoomLon1;
//  m_zoomLon2 = p.m_zoomLon2;
//  m_zoomLat1 = p.m_zoomLat1;
//  m_zoomLat2 = p.m_zoomLat2;
//
//  m_withContour = p.m_withContour;
//  m_withContourLabel = p.m_withContourLabel;
//  m_minContourValue = p.m_minContourValue;
//  m_maxContourValue = p.m_maxContourValue;
//  m_numContour = p.m_numContour;
//  m_numContourLabel = p.m_numContourLabel;
//
//  m_contourLabelSize = p.m_contourLabelSize;
//  m_contourLineWidth = p.m_contourLineWidth;
//
//  m_contourLineColor = p.m_contourLineColor;
//  m_contourLabelColor = p.m_contourLabelColor;
//
//  m_contourLabelFormat = p.m_contourLabelFormat;
//
//  m_solidColor = p.m_solidColor;
//
//  m_withAnimation = p.m_withAnimation;
//
//  m_eastComponent = p.m_eastComponent;
//  m_northComponent = p.m_northComponent;
//
//
//}
//
//
//-------------------------------------------------------------
//------------------- VTK_CWorldPlotCommonData class --------------------
//-------------------------------------------------------------

void VTK_CWorldPlotCommonData::Init() 
{

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


  NoTransform();
}
//----------------------------------------
VTK_CWorldPlotCommonData::~VTK_CWorldPlotCommonData()
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
void VTK_CWorldPlotCommonData::NewVtkContourLabelObject()
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
void VTK_CWorldPlotCommonData::DeleteVtkContourLabelObject()
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
void VTK_CWorldPlotCommonData::DeleteContourFilter()
{
  if (m_vtkContourFilter != NULL)
  {
    m_vtkContourFilter->Delete();
    m_vtkContourFilter = NULL;
  }

}
//----------------------------------------
void VTK_CWorldPlotCommonData::DeleteLUT()
{
  if (m_LUT != NULL)
  {
    delete m_LUT;
    m_LUT = NULL;
  }

}

//----------------------------------------
void VTK_CWorldPlotCommonData::DeleteAll3D()
{
  Delete3D();
  DeleteContour3D();
}

//----------------------------------------
void VTK_CWorldPlotCommonData::Delete3D()
{
  DeleteActor();
  DeleteMapper();
}

//----------------------------------------
void VTK_CWorldPlotCommonData::DeleteContour3D()
{
  DeleteContourActor();
  DeleteContourMapper();
  DeleteContourLabelActor();
  DeleteContourLabelMapper();
  DeleteContourFilter();
}

//----------------------------------------
void VTK_CWorldPlotCommonData::DeleteAll2D()
{
  Delete2D();
  DeleteContour2D();

}
//----------------------------------------
void VTK_CWorldPlotCommonData::Delete2D()
{
  DeleteActor2D();
  DeleteMapper2D();
}

//----------------------------------------
void VTK_CWorldPlotCommonData::DeleteContour2D()
{
  DeleteContourActor2D();
  DeleteContourMapper2D();
  DeleteContourLabelActor();
  DeleteContourLabelMapper();
  DeleteContourFilter();
}

//----------------------------------------
void VTK_CWorldPlotCommonData::DeleteActor()
{
  if (m_vtkActor != NULL)
  {
    m_vtkActor->Delete();
    m_vtkActor = NULL;
  }
}

//----------------------------------------
void VTK_CWorldPlotCommonData::DeleteContourActor()
{
  if (m_vtkContourActor != NULL)
  {
    m_vtkContourActor->Delete();
    m_vtkContourActor = NULL;
  }
}
//----------------------------------------
void VTK_CWorldPlotCommonData::DeleteContourLabelActor()
{
  if (m_vtkContourLabelActor != NULL)
  {
    m_vtkContourLabelActor->Delete();
    m_vtkContourLabelActor = NULL;
  }
}

//----------------------------------------
void VTK_CWorldPlotCommonData::DeleteActor2D()
{
  if (m_vtkActor2D != NULL)
  {
    m_vtkActor2D->Delete();
    m_vtkActor2D = NULL;
  }
}

//----------------------------------------
void VTK_CWorldPlotCommonData::DeleteContourActor2D()
{
  if (m_vtkContourActor2D != NULL)
  {
    m_vtkContourActor2D->Delete();
    m_vtkContourActor2D = NULL;
  }
}

//----------------------------------------
void VTK_CWorldPlotCommonData::DeleteMapper()
{
  if (m_vtkMapper != NULL)
  {
    m_vtkMapper->Delete();
    m_vtkMapper = NULL;
  }
}

//----------------------------------------
void VTK_CWorldPlotCommonData::DeleteContourMapper()
{
  if (m_vtkContourMapper != NULL)
  {
    m_vtkContourMapper->Delete();
    m_vtkContourMapper = NULL;
  }
}

//----------------------------------------
void VTK_CWorldPlotCommonData::DeleteContourLabelMapper()
{
  if (m_vtkContourLabelMapper != NULL)
  {
    m_vtkContourLabelMapper->Delete();
    m_vtkContourLabelMapper = NULL;
  }
}

//----------------------------------------
void VTK_CWorldPlotCommonData::DeleteMapper2D()
{
  if (m_vtkMapper2D != NULL)
  {
    m_vtkMapper2D->Delete();
    m_vtkMapper2D = NULL;
  }

}
//----------------------------------------
void VTK_CWorldPlotCommonData::DeleteContourMapper2D()
{
  if (m_vtkContourMapper2D != NULL)
  {
    m_vtkContourMapper2D->Delete();
    m_vtkContourMapper2D = NULL;
  }

}
//----------------------------------------
bool VTK_CWorldPlotCommonData::HasActor()
{
  return ((m_vtkActor != NULL) || (m_vtkContourActor != NULL));
}
//----------------------------------------
bool VTK_CWorldPlotCommonData::HasActor2D()
{
  return ((m_vtkActor2D != NULL) || (m_vtkContourActor2D != NULL));
}
//----------------------------------------

void VTK_CWorldPlotCommonData::NoTransform()
{
  m_transform->Identity();
  m_vtkTransform->SetTransform(m_transform);

}

//----------------------------------------
void VTK_CWorldPlotCommonData::SetLUT(CBratLookupTable* lut)
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

void VTK_CWorldPlotCommonData::SetProjection( int32_t proj )
{
	base_t::SetProjection( proj );			//  m_projection = proj;

	GetVtkFilter()->SetProjection( proj );

	//if ( (proj == VTK_PROJ2D_3D) && ((m_is3D == false) || (m_vtkActor == NULL)) )
	if ( ( proj == VTK_PROJ2D_3D ) && ( ( m_is3D == false ) || ( HasActor() == false ) ) )
	{
		DeleteAll2D();

		Set3D();

	}
	//else if ( (proj != VTK_PROJ2D_3D) && ((m_is3D == true) || (m_vtkActor2D == NULL)) )
	else if ( ( proj != VTK_PROJ2D_3D ) && ( ( m_is3D == true ) || ( HasActor2D() == false ) ) )
	{
		DeleteAll3D();

		Set2D();
	}
	else
	{
		return;
		//CException e("ERROR in VTK_CWorldPlotCommonData::SetProjection : Mode is  neither 3D nor 2D", BRATHL_LOGIC_ERROR);
		//throw(e);
	}
}
//----------------------------------------
void VTK_CWorldPlotCommonData::Set3D()
{
	Delete3D();

	base_t::Set3D();			//m_is3D = true;

	m_vtkMapper = vtkPolyDataMapper::New();

	m_vtkMapper->SetInput( GetVtkFilter()->GetOutput() );
	m_vtkMapper->SetScalarModeToUseCellData();
	m_vtkMapper->UseLookupTableScalarRangeOn();

	if ( m_LUT != NULL )
	{
		m_vtkMapper->SetLookupTable( m_LUT->GetLookupTable() );
	}

	m_vtkActor = vtkActor::New();
	m_vtkActor->SetMapper( m_vtkMapper );

	Update();
}
//----------------------------------------
void VTK_CWorldPlotCommonData::Set2D()
{
	Delete2D();

	base_t::Set2D();		//m_is3D = false;

	m_vtkTransform->SetInput( GetVtkFilter()->GetOutput() );
	//m_vtkTransform->SetTransform(m_transform);

	vtkCoordinate* c = vtkCoordinate::New();
	c->SetCoordinateSystemToNormalizedViewport();


	m_vtkMapper2D = vtkPolyDataMapper2D::New();
	m_vtkMapper2D->SetInput( m_vtkTransform->GetOutput() );
	m_vtkMapper2D->SetTransformCoordinate( c );

	m_vtkMapper2D->SetScalarModeToUseCellData();
	m_vtkMapper2D->UseLookupTableScalarRangeOn();

	if ( m_LUT != NULL )
	{
		m_vtkMapper2D->SetLookupTable( m_LUT->GetLookupTable() );
	}

	m_vtkActor2D = vtkActor2D::New();
	m_vtkActor2D->SetMapper( m_vtkMapper2D );

	c->Delete();
	c = NULL;

	Update();
}

//----------------------------------------
void VTK_CWorldPlotCommonData::SetCenterLatitude(double lat)
{
  if (GetVtkFilter() != NULL)
  {
    GetVtkFilter()->SetCenterLatitude(lat);
  }
}

//----------------------------------------
void VTK_CWorldPlotCommonData::SetCenterLongitude(double lon)
{
  if (GetVtkFilter() != NULL)
  {
    GetVtkFilter()->SetCenterLongitude(lon);
  }
}

//----------------------------------------
void VTK_CWorldPlotCommonData::SetInput(vtkPolyData* output)
{
  if (GetVtkFilter() != NULL)
  {
    GetVtkFilter()->SetInput(output);
  }
}


//-------------------------------------------------------------
//------------------- CGeoGrid class --------------------
//-------------------------------------------------------------

CGeoGrid::CGeoGrid() : VTK_CWorldPlotCommonData()
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



  VTK_CWorldPlotCommonData::SetProjection(proj);

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

CCoastLine::CCoastLine(const std::string& fileName, int32_t maxLevel) : VTK_CWorldPlotCommonData()
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
void CCoastLine::SetGSHHSReader(const std::string& fileName, int32_t maxLevel)
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

CGlobe::CGlobe() : VTK_CWorldPlotCommonData()
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

