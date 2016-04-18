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

#include "new-gui/brat/DataModels/PlotData/BratLookupTable.h"

#include "WorldPlotData_vtk.h"

//-------------------------------------------------------------
//------------------- VTK_CWorldPlotData class --------------------
//-------------------------------------------------------------

void VTK_CWorldPlotData::Create( CObArray* data, const std::string& fieldName )
{
	// lambdas

	auto Crash = []( const std::string &msg, int error_type )
	{
		CException e( msg, error_type );
		CTrace::Tracer( "%s", e.what() );
		throw ( e );
	};

	// function body 

	m_colorBarRenderer = new CLUTRenderer();
	m_colorBarRenderer->GetVtkRenderer()->InteractiveOff();

	m_colorBarRenderer->SetNumberOfLabels( m_plotProperty.m_numColorLabels );

	
	for ( uint32_t iMap = 0; iMap < m_nrMaps; iMap++ )
	{
		vtkDoubleArray *values = vtkDoubleArray::New();
		vtkShortArray *bitarray = vtkShortArray::New();
		vtkShortArray *validMercatorLatitudes = vtkShortArray::New();
		vtkDoubleArray *longitudes = vtkDoubleArray::New();
		vtkDoubleArray *latitudes = vtkDoubleArray::New();

		for ( auto v : mMaps( iMap ).mValues )
			values->InsertNextValue( v );

		for ( auto v : mMaps( iMap ).mBits )
			bitarray->InsertNextValue( v );

		for ( auto v : mMaps( iMap ).mValidMercatorLatitudes )
			validMercatorLatitudes->InsertNextValue( v );

		for ( auto v : mMaps( iMap ).mXaxis )
			longitudes->InsertNextValue( v );

		for ( auto v : mMaps( iMap ).mYaxis )
			latitudes->InsertNextValue( v );


		vtkGeoMapFilter *geoMapFilter = vtkGeoMapFilter::New();
		geoMapFilter->SetValues( values );

		geoMapFilter->SetHeights( values );
		geoMapFilter->SetBitarray( bitarray );
		geoMapFilter->SetValidMercatorLatitudes( validMercatorLatitudes );
		geoMapFilter->SetLongitudes( longitudes );
		geoMapFilter->SetLatitudes( latitudes );

		geoMapFilter->SetOffsetLatitude( mMaps().mLatitudeOffset );
		geoMapFilter->SetOffsetLongitude( mMaps().mLongitudeOffset );

		geoMapFilter->SetFactor( m_plotProperty.m_heightFactor );
		geoMapFilter->SetMapWidth( mMaps().mPlotWidth );
		geoMapFilter->SetMapHeight( mMaps().mPlotHeight );

		m_geoMapFilterList.Insert( geoMapFilter );

		values->Delete();
		bitarray->Delete();
		validMercatorLatitudes->Delete();
		longitudes->Delete();
		latitudes->Delete();
	}
	

	for ( uint32_t iMap = 0; iMap < m_nrMaps; iMap++ )
	{
		vtkGeoMapFilter *geoMapFilter = dynamic_cast<vtkGeoMapFilter*>( m_geoMapFilterList[ iMap ] );		//geoMapFilter = dynamic_cast<vtkVelocityGlyphFilter*>(m_geoMapFilterList[iMap]);
		geoMapFilter->SetMinMappedValue( mMaps( iMap ).mMinHeightValue );
		geoMapFilter->SetMaxMappedValue( mMaps( iMap ).mMaxHeightValue );
	}


	//v4 note: as noted for ZFXY plots, using the last HeightValues is maybe an implicit
	//	assumption that only one iteration exists (only one map) in the first loop of this function
	//
	m_LUT->GetLookupTable()->SetTableRange( mMaps().mMinHeightValue, mMaps().mMaxHeightValue );	

	m_colorBarRenderer->SetLUT( m_LUT );


	vtkGeoMapFilter *geoMapFilter = dynamic_cast<vtkGeoMapFilter*>( m_geoMapFilterList[ m_currentMap ] );	//geoMapFilter = dynamic_cast<vtkVelocityGlyphFilter*>(m_geoMapFilterList[m_currentMap]);
	if ( geoMapFilter == NULL )
		Crash( CTools::Format( "ERROR in CWorldPlotData ctor  : dynamic_cast<vtkGeoMapFilter*>(m_geoMapFilterList[%d]) returns NULL pointer - " "m_geoMapFilterList std::list seems to contain objects other than those of the class vtkGeoMapFilter", m_currentMap ), BRATHL_LOGIC_ERROR );


	SetInput( geoMapFilter->GetOutput() );
}


//----------------------------------------
void VTK_CWorldPlotData::SetProjection(int32_t proj)
{

  base_t::SetProjection(proj);
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
void VTK_CWorldPlotData::OnKeyframeChanged(uint32_t i)
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
      throw CException(CTools::Format("ERROR in VTK_CWorldPlotData ctor ::SetProjection : dynamic_cast<vtkGeoMapFilter*>(m_geoMapFilterList[%d]) returns NULL pointer - "
                       "m_geoMapFilterList std::list seems to contain objects other than those of the class vtkGeoMapFilter", m_currentMap),
                       BRATHL_LOGIC_ERROR);

    }

    SetInput(geoMapFilter->GetOutput());

  }
}

//----------------------------------------

//----------------------------------------
void VTK_CWorldPlotData::Update()
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
//void VTK_CWorldPlotData::GetMapLatLon(CInternalFilesZFXY* zfxy,
//                           int32_t& width, int32_t& height,
//                           CExpressionValue& varLat, CExpressionValue& varLon)
//{
//  CStringArray axisNames;
//  CStringArray::iterator is;
//  ExpressionValueDimensions dimVal;
//
//  width = -1;
//  height = -1;
//
//  zfxy->GetAxisVars(axisNames);
//
//  for (is = axisNames.begin(); is != axisNames.end(); is++)
//  {
//    zfxy->GetVarDims(*is, dimVal);
//    if (dimVal.size() != 1)
//    {
//      std::string msg = CTools::Format("VTK_CWorldPlotData::GetMapLatLon - '%s' axis -> number of dimensions not equal 1 : %ld",
//                                   (*is).c_str(), (long)dimVal.size());
//      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
//      CTrace::Tracer("%s", e.what());
//      throw (e);
//    }
//    NetCDFVarKind varKind = zfxy->GetVarKind(*is);
//    if (varKind == Longitude)
//    {
//      width = dimVal.at(0);
//      zfxy->ReadVar(*is, varLon, CLatLonPoint::m_DEFAULT_UNIT_LONGITUDE);
//    }
//    else if (varKind == Latitude)
//    {
//      height = dimVal.at(0);
//      zfxy->ReadVar(*is, varLat, CLatLonPoint::m_DEFAULT_UNIT_LATITUDE);
//    }
//  }
//
//  if (width == -1)
//    {
//      std::string msg = CTools::Format("VTK_CWorldPlotData::GetMapLatLon - Longitude axis not found in input file '%s'",
//                                   zfxy->GetName().c_str());
//      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
//      CTrace::Tracer("%s", e.what());
//      throw (e);
//    }
//
//  if (height == -1)
//    {
//      std::string msg = CTools::Format("VTK_CWorldPlotData::GetMapLatLon - Latitude axis not found in input file '%s'",
//                                   zfxy->GetName().c_str());
//      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
//      CTrace::Tracer("%s", e.what());
//      throw (e);
//    }
//
//}
//
//----------------------------------------
//std::string VTK_CWorldPlotData::GetDataDateString(uint32_t index)
//{
//
//  if (index >= m_dataDates.size())
//  {
//    return "";
//    /*
//    CException e(CTools::Format("base_t GetDataDateString - index %d out-of-range - Max. index is %d",
//                                index, m_dataDates.size()),
//                 BRATHL_LOGIC_ERROR);
//    CTrace::Tracer(e.what());
//    throw (e);
//    */
//
//  }
//
//  CDate* dataDate = dynamic_cast<CDate*>(m_dataDates[index]);
//  if (dataDate->IsDefaultValue() == false)
//  {
//    return CTools::Format(128, "Date:\t%s - Time:\t%s",
//                          dataDate->AsString("%d-%b-%Y", false).c_str(),
//                          dataDate->AsString("%H:%M:%S", true).c_str());
//  }
//
//  return "";
//}

//----------------------------------------
//std::string VTK_CWorldPlotData::GetDataUnitString(uint32_t index)
//{
//  if (index >= m_dataUnits.size())
//  {
//    return "";
//    /*
//    CException e(CTools::Format("base_t GetDataUnitString - index %d out-of-range - Max. index is %d",
//                                index, m_dataUnits.size()),
//                 BRATHL_LOGIC_ERROR);
//    CTrace::Tracer(e.what());
//    throw (e);
//    */
//  }
//
//  CUnit* dataUnit = dynamic_cast<CUnit*>(m_dataUnits[index]);
//
//  return dataUnit->GetText();
//}
//----------------------------------------
void VTK_CWorldPlotData::SetContour3D()
{
  DeleteContour3D();

  if (m_plotProperty.m_withContour == false)
  {
    return;
  }

  vtkGeoMapFilter* geoMapFilter = dynamic_cast<vtkGeoMapFilter*>(m_geoMapFilterList[m_currentMap]);
  if (geoMapFilter == NULL)
  {
    throw CException(CTools::Format("ERROR in VTK_CWorldPlotData::Set3D ::SetProjection : dynamic_cast<vtkGeoMapFilter*>(m_geoMapFilterList[%d]) returns NULL pointer - "
                     "m_geoMapFilterList std::list seems to contain objects other than those of the class vtkGeoMapFilter", m_currentMap),
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
void VTK_CWorldPlotData::SetContour2D()
{
  DeleteContour2D();

  if (m_plotProperty.m_withContour == false)
  {
    return;
  }


  vtkGeoMapFilter* geoMapFilter = dynamic_cast<vtkGeoMapFilter*>(m_geoMapFilterList[m_currentMap]);
  if (geoMapFilter == NULL)
  {
    throw CException(CTools::Format("ERROR in VTK_CWorldPlotData::Set2D ::SetProjection : dynamic_cast<vtkGeoMapFilter*>(m_geoMapFilterList[%d]) returns NULL pointer - "
                     "m_geoMapFilterList std::list seems to contain objects other than those of the class vtkGeoMapFilter", m_currentMap),
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
void VTK_CWorldPlotData::SetContourLabelProperties()
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
void VTK_CWorldPlotData::ContourGenerateValues()
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
void VTK_CWorldPlotData::SetContour3DProperties()
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
void VTK_CWorldPlotData::SetContour2DProperties()
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
void VTK_CWorldPlotData::CreateContourLabels()
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
  m_vtkVisibleSpherePointsFilter->SetExecuteMethod(VTK_CWorldPlotData::FindVisibleSpherePoints, static_cast<void*>(this));

  m_vtkContourLabelMapper->SetInput(m_vtkVisibleSpherePointsFilter->GetPolyDataOutput());

  m_vtkContourLabelActor = vtkActor2D::New();

  m_vtkContourLabelActor->SetMapper(m_vtkContourLabelMapper)  ;

  UpdateContourLabels();

}
//----------------------------------------
void VTK_CWorldPlotData::UpdateContourLabels()
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

  //femm TODO wxSetCursor(*wxHOURGLASS_CURSOR);

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

	//femm TODO wxSetCursor(wxNullCursor);

}
//----------------------------------------
void VTK_CWorldPlotData::FindVisibleSpherePoints(void* arg)
{
  VTK_CWorldPlotData* geoMap = static_cast<VTK_CWorldPlotData*>(arg);
  if (geoMap == NULL)
  {
    return;
  }

  // Finds visible points on sphere based on cosine angle of points
  // relative to camera view std::vector.
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
void VTK_CWorldPlotData::FindVisiblePlanePoints(void* arg)
{
  VTK_CWorldPlotData* geoMap = static_cast<VTK_CWorldPlotData*>(arg);
  if (geoMap == NULL)
  {
    return;
  }

  // Finds visible points on sphere based on cosine angle of points
  // relative to camera view std::vector.
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
void VTK_CWorldPlotData::CreateContourLabels2D()
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
  m_vtkVisibleSpherePointsFilter->SetExecuteMethod(VTK_CWorldPlotData::FindVisiblePlanePoints, static_cast<void*>(this));

 // m_vtkContourLabelMapper->SetInput(m_vtkLabelContourPositions);
  m_vtkContourLabelMapper->SetInput(m_vtkVisibleSpherePointsFilter->GetPolyDataOutput());

  m_vtkContourLabelActor = vtkActor2D::New();

  m_vtkContourLabelActor->SetMapper(m_vtkContourLabelMapper);

  UpdateContourLabels2D();

}

//----------------------------------------
void VTK_CWorldPlotData::UpdateContourLabels2D()
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

  	//femm TODO wxSetCursor(*wxHOURGLASS_CURSOR);

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

  //femm TODO wxSetCursor(wxNullCursor);
  m_contourLabelNeedUpdateOnWindow = false;
}


//----------------------------------------
void VTK_CWorldPlotData::SetContourLabels2DPosition()
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

  //femm TODO wxSetCursor(*wxHOURGLASS_CURSOR);

  int32_t nLabels = m_plotProperty.m_numContourLabel;
  int32_t nContours =  (uint32_t)m_labelPts.size();

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

  //femm TODO wxSetCursor(wxNullCursor);
}








//-------------------------------------------------------------
//------------------- VTK_CWorldPlotVelocityData class --------------------
//-------------------------------------------------------------

//----------------------------------------

VTK_CWorldPlotVelocityData::VTK_CWorldPlotVelocityData( CPlotField* fieldNorth, CPlotField* fieldEast )
	: VTK_CWorldPlotData( fieldNorth, fieldEast )
{
	m_simple2DProjFilter = vtkProj2DFilter::New();

	// this is just to avoid interpolation and generation of intermediate points
	GetVtkFilter()->SetInterpolationDistance( 10 );
    IsGlyph = false;
	m_arrowSource = NULL;
	m_glyph = NULL;

	Create( &( fieldNorth->m_internalFiles ), &( fieldEast->m_internalFiles ), fieldNorth->m_name, fieldEast->m_name );
}
//----------------------------------------
VTK_CWorldPlotVelocityData::~VTK_CWorldPlotVelocityData()
{
    if ( m_simple2DProjFilter )
    m_simple2DProjFilter->Delete();
    if ( m_arrowSource )
        m_arrowSource->Delete();
    if ( m_glyph )
        m_glyph->Delete();
}

//----------------------------------------
void VTK_CWorldPlotVelocityData::Create( CObArray* northData, CObArray* eastData, const std::string& fieldNameNorth, const std::string& fieldNameEast )
{
	////////////////auto Crash = []( const std::string &msg, int error_type )
	////////////////{
	////////////////	CException e( msg, error_type );
	////////////////	CTrace::Tracer( "%s", e.what() );
	////////////////	throw ( e );
	////////////////};

	////////////////m_contourLabelNeedUpdateOnWindow = false;
	////////////////m_contourLabelNeedUpdatePositionOnContour = false;

	////////////////m_plotProperty.m_name = fieldNameEast + "/" + fieldNameNorth;

	m_colorBarRenderer = new CLUTRenderer();
	m_colorBarRenderer->GetVtkRenderer()->InteractiveOff();
	m_colorBarRenderer->SetNumberOfLabels( m_plotProperty.m_numColorLabels );

	////////////////m_finished = false;
	////////////////m_currentMap = 0;

	//////////////// // check both are of the same size
	//////////////// m_nrMaps = (uint32_t)northData->size();
	//////////////// if ( northData->size() != eastData->size() )
	////////////////	Crash("VTK_CWorldPlotVelocityData - North and East data of different size!", BRATHL_LOGIC_ERROR);

	//////////////// //femm TODO - begin
	//////////////// //wxProgressDialog pd("Calculating Geomap Data", "", m_nrMaps, parent, wxPD_CAN_ABORT | wxPD_REMAINING_TIME | wxPD_SMOOTH| wxPD_APP_MODAL);
	//////////////// //pd.SetSize(-1,-1, 350, -1);
	//////////////// //femm TODO - end

	//////////////// bool firstValue = true;

	//////////////// //double minHeightValue = 0;
	//////////////// //double maxHeightValue = 0;

	/*
	for ( uint32_t iMap = 0; iMap < m_nrMaps; iMap++ )
	{
		mMaps.AddMap();

		mMaps().mMinHeightValue = m_plotProperty.m_minHeightValue;
		mMaps().mMaxHeightValue = m_plotProperty.m_maxHeightValue;

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
		CInternalFilesZFXY* zfxy = dynamic_cast<CInternalFilesZFXY*>( northData->at( iMap ) );
		CInternalFilesZFXY* zfxyEast = dynamic_cast<CInternalFilesZFXY*>( eastData->at( iMap ) );


		if ( zfxy == NULL || zfxy == NULL )
			Crash( "VTK_CWorldPlotVelocityData ctor - Non-geographical data found - dynamic_cast<CInternalFilesZFXY*>data->at(iMap) returns NULL", BRATHL_LOGIC_ERROR );


		if ( zfxy->IsGeographic() == false || zfxyEast->IsGeographic() == false )
			Crash( "VTK_CWorldPlotVelocityData ctor - Non-geographical data found - IsGeographic() is false", BRATHL_LOGIC_ERROR );

		//femm TODO - begin
		//update ProgressDialog
		//std::string msg = CTools::Format("Calculating Frame: %d of %d", iMap + 1, m_nrMaps);
		//if (pd.Update(iMap, msg.c_str()) == false)
		//{
		//  m_aborted = true;
		//  break;
		//}
		//femm TODO - end


		int32_t mapWidth = 0;
		int32_t mapHeight = 0;

		GetMapLatLon( zfxy, mMaps().mPlotWidth, mMaps().mPlotHeight, varLat, varLon );

		// Get data units
		CUnit* unitNorth = new CUnit( zfxy->GetUnit( fieldNameNorth ) );
		CUnit* unitEast = new CUnit( zfxyEast->GetUnit( fieldNameEast ) );


		if ( *unitNorth != *unitEast )
			Crash( CTools::Format( "VTK_CWorldPlotVelocityData ctor - Unequal Units (%s != %s)", "d", "d" ), BRATHL_INCONSISTENCY_ERROR );

		m_dataUnits.Insert( unitNorth );		//v3 original comment m_dataUnits.Insert(unitEast);

		m_dataTitles.Insert( m_plotProperty.m_name );


		// read data
		zfxy->ReadVar( fieldNameNorth, varValueNorth, unitNorth->GetText() );
		varKindNorth	= zfxy->GetVarKind( fieldNameNorth );

		zfxy->ReadVar( fieldNameEast, varValueEast, unitEast->GetText() );
		// this may need to be changed
		varKindEast	= zfxyEast->GetVarKind( fieldNameEast );


		if ( varKindNorth != Data || varKindEast != varKindNorth )
			Crash(CTools::Format( "VTK_CWorldPlotVelocityData ctor - variables '%s', '%s' are not a kind of Data (%d) : %s", fieldNameNorth.c_str(), fieldNameEast.c_str(), Data, CNetCDFFiles::VarKindToString( varKindNorth ).c_str() ), BRATHL_INCONSISTENCY_ERROR);


		zfxy->GetVarDims( fieldNameNorth, varDimValNorth );
		zfxyEast->GetVarDims( fieldNameEast, varDimValEast );

		if ( varDimValNorth.size() != 2 || varDimValNorth.size() != varDimValEast.size() )
			Crash( CTools::Format( "VTK_CWorldPlotVelocityData ctor - variables '%s or %s -> number of dimensions value not equal 2 : %ld, %ld", fieldNameNorth.c_str(), fieldNameEast.c_str(), (long)varDimValNorth.size(), (long)varDimValEast.size() ), BRATHL_INCONSISTENCY_ERROR );

		zfxy->GetVarDims( fieldNameNorth, varDimNamesNorth );
		zfxyEast->GetVarDims( fieldNameEast, varDimNamesEast );

		if ( varDimNamesNorth.size() != 2 || varDimNamesEast.size() != varDimNamesNorth.size() )
			Crash( CTools::Format( "VTK_CWorldPlotVelocityData ctor - variable '%s  or %s-> number of dimensions name not equal 2 : %ld, %ld", fieldNameNorth.c_str(), fieldNameEast.c_str(), (long)varDimNamesNorth.size(), (long)varDimNamesEast.size() ), BRATHL_INCONSISTENCY_ERROR);

		// v3 original comment TODO (cornejo#1#): should we check this with the East velocity as well??
		bool firstDimIsLat = false;
		if ( ( zfxy->GetVarKind( varDimNamesNorth.at( 0 ) ) == Latitude ) &&
			( zfxy->GetVarKind( varDimNamesNorth.at( 1 ) ) == Longitude ) )
		{
			firstDimIsLat = true;
		}
		else if ( ( zfxy->GetVarKind( varDimNamesNorth.at( 0 ) ) == Longitude ) &&
			( zfxy->GetVarKind( varDimNamesNorth.at( 1 ) ) == Latitude ) )
		{
			firstDimIsLat = false;
		}
		else
			Crash( CTools::Format( "VTK_CWorldPlotVelocityData ctor - Inconsistency kind of axis : varDimNames|0] = '%s' (%s) and "
				"varDimNames|1] = '%s' (%s)",
				varDimNamesNorth.at( 0 ).c_str(), CNetCDFFiles::VarKindToString( zfxy->GetVarKind( varDimNamesNorth.at( 0 ) ) ).c_str(),
				varDimNamesNorth.at( 1 ).c_str(), CNetCDFFiles::VarKindToString( zfxy->GetVarKind( varDimNamesNorth.at( 0 ) ) ).c_str() ), BRATHL_INCONSISTENCY_ERROR );
				
		const int32_t maxX = mMaps().mPlotWidth;
		const int32_t maxY = mMaps().mPlotHeight;


		//////////////////////////////////////vtkDoubleArray* valuesNorth = vtkDoubleArray::New();
		//////////////////////////////////////vtkDoubleArray* valuesEast = vtkDoubleArray::New();
		//////////////////////////////////////vtkShortArray* bitarray = vtkShortArray::New();
		//////////////////////////////////////vtkShortArray* validMercatorLatitudes = vtkShortArray::New();
		//////////////////////////////////////vtkDoubleArray* longitudes = vtkDoubleArray::New();
		//////////////////////////////////////vtkDoubleArray* latitudes = vtkDoubleArray::New();

		for ( int32_t iY = 0; iY < maxY; iY++ )
		{
			//////////////latitudes->InsertNextValue( varLat.GetValues()[ iY ] );
			mMaps.AddY( varLat.GetValues()[ iY ] );

			for ( int32_t iX = 0; iX < maxX; iX++ )
			{
				int32_t iTemp;
				if ( firstDimIsLat )
					iTemp = ( iY * maxX ) + iX;
				else
					iTemp = ( iX * maxY ) + iY;

				int16_t validLatitude = ( CTools::AreValidMercatorLatitude( varLat.GetValues()[ iY ] ) ? 1 : 0 );
				////////////////////////////validMercatorLatitudes->InsertNextValue( validLatitude );
				mMaps.AddmValidMercatorLatitude( validLatitude != 0 );
				
				double vN = varValueNorth.GetValues()[ iTemp ];
				double vE = varValueEast.GetValues()[ iTemp ];

				// invalid values?
				if ( ( CTools::IsNan( vN ) != 0 ) || ( CTools::IsNan( vE ) != 0 ) || isDefaultValue( vN ) || isDefaultValue( vE ) )
				{
					//////////////////////////////valuesNorth->InsertNextValue( 0 );
					//////////////////////////////valuesEast->InsertNextValue( 0 );
					//////////////////////////////bitarray->InsertNextValue( 0 );
					mMaps.AddBit( false );
					mMaps.AddValue( 0.0 );
					mMaps.AddEastValue( 0.0 );
				}
				else
				{
					mMaps.AddBit( true );
					mMaps.AddValue( vN );
					mMaps.AddEastValue( vE );
					
					double magnitude = sqrt( vE*vE + vN*vN );
					
					/////////////////////////////////////valuesNorth->InsertNextValue( vN );
					/////////////////////////////////////valuesEast->InsertNextValue( vE );
					/////////////////////////////////////bitarray->InsertNextValue( 1 );
					
					// this is just to figure out minimums and maximums
					if ( firstValue )
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
						if ( vN < m_minhvN )
						{
							m_minhvN = vN;
						}
						if ( vN > m_maxhvN )
						{
							m_maxhvN = vN;
						}


						if ( vE < m_minhvE )
						{
							m_minhvE = vE;
						}
						if ( vE > m_maxhvE )
						{
							m_maxhvE = vE;
						}

						if ( magnitude < m_minhv )
						{
							m_minhv = magnitude;
						}
						if ( magnitude > m_maxhv )
						{
							m_maxhv = magnitude;
						}

					}
				}
			}
		}

		if ( isDefaultValue( mMaps().mMinHeightValue ) )
			mMaps().mMinHeightValue = m_minhvN < m_minhvE ? m_minhvN : m_minhvE;
		if ( isDefaultValue( mMaps().mMaxHeightValue ) )
			mMaps().mMaxHeightValue = m_minhvN > m_minhvE ? m_minhvN : m_minhvE;

		*/

	
	for ( auto iMap = 0; iMap < m_nrMaps; iMap++ )
	{
		vtkDoubleArray* valuesNorth = vtkDoubleArray::New();
		vtkDoubleArray* valuesEast = vtkDoubleArray::New();
		vtkShortArray* bitarray = vtkShortArray::New();
		vtkShortArray* validMercatorLatitudes = vtkShortArray::New();
		vtkDoubleArray* longitudes = vtkDoubleArray::New();
		vtkDoubleArray* latitudes = vtkDoubleArray::New();

		for ( auto v : mMaps( iMap ).mValues )
			valuesNorth->InsertNextValue( v );

		for ( auto v : mMaps( iMap ).mValuesEast )
			valuesEast->InsertNextValue( v );

		for ( auto v : mMaps( iMap ).mBits )
			bitarray->InsertNextValue( v );

		for ( auto v : mMaps( iMap ).mValidMercatorLatitudes )
			validMercatorLatitudes->InsertNextValue( v );

		for ( auto v : mMaps( iMap ).mXaxis )
			longitudes->InsertNextValue( v );

		for ( auto v : mMaps( iMap ).mYaxis )
			latitudes->InsertNextValue( v );

		//////////////////////for ( int32_t iX = 0; iX < mMaps().mPlotWidth; iX++ ){
			//////////////////////longitudes->InsertNextValue( CTools::NormalizeLongitude( -180.0, varLon.GetValues()[ iX ] ) );			//v3 original comment longitudes->InsertNextValue(varLon.GetValues()[iX]);
			//////////////////////mMaps.AddX(CTools::NormalizeLongitude( -180.0, varLon.GetValues()[ iX ] ));
		//////////////////////}

		vtkVelocityGlyphFilter* geoMapFilter = vtkVelocityGlyphFilter::New();
		geoMapFilter->SetValuesNorth( valuesNorth );
		geoMapFilter->SetValuesEast( valuesEast );

		geoMapFilter->SetMaxVN( m_maxhvN );
		geoMapFilter->SetMinVN( m_minhvN );
		geoMapFilter->SetMaxVE( m_maxhvE );
		geoMapFilter->SetMinVE( m_minhvE );		//v3 original comment geoMapFilter->SetMinV(m_minhv);
		geoMapFilter->SetMaxV( m_maxhv );

		// TODO (cornejo#1#): what should we use here.  Magnitude??
		geoMapFilter->SetHeights( valuesNorth );

		geoMapFilter->SetBitarray( bitarray );
		geoMapFilter->SetValidMercatorLatitudes( validMercatorLatitudes );
		geoMapFilter->SetLongitudes( longitudes );
		geoMapFilter->SetLatitudes( latitudes );

		//geoMapFilter->SetOffsetLatitude(1);
		//geoMapFilter->SetOffsetLongitude(1);

		geoMapFilter->SetFactor( m_plotProperty.m_heightFactor );
		geoMapFilter->SetMapWidth( mMaps().mPlotWidth );
		geoMapFilter->SetMapHeight( mMaps().mPlotHeight );

		m_geoMapFilterList.Insert( geoMapFilter );

		valuesNorth->Delete();
		valuesEast->Delete();

		bitarray->Delete();
		validMercatorLatitudes->Delete();
		longitudes->Delete();
		latitudes->Delete();
	}


	/////////////////////// if ( isDefaultValue( m_plotProperty.m_minContourValue ) )
	/////////////////////// 	m_plotProperty.m_minContourValue = mMaps().mMinHeightValue;

	/////////////////////// if ( isDefaultValue( m_plotProperty.m_maxContourValue ) )
	/////////////////////// 	m_plotProperty.m_maxContourValue = mMaps().mMaxHeightValue;

	///////////////////////  //femm TODO - begin
	///////////////////////  //  pd.Destroy();
	///////////////////////  //femm TODO - begin

	/////////////////////// if ( m_aborted )
	/////////////////////// 	return;

	for ( auto iMap = 0; iMap < m_nrMaps; iMap++ )
	{
		//v3 original comment geoMapFilter = dynamic_cast<vtkGeoMapFilter*>(m_geoMapFilterList[iMap]);
		vtkGeoMapFilter* gMapFilter = dynamic_cast<vtkGeoMapFilter*>( m_geoMapFilterList[ iMap ] );
		if ( gMapFilter == NULL )
		{
			continue;
		}

		gMapFilter->SetMinMappedValue( mMaps( iMap ).mMinHeightValue );
		gMapFilter->SetMaxMappedValue( mMaps( iMap ).mMaxHeightValue );
	}


	m_LUT->GetLookupTable()->SetTableRange( 0, m_maxhv );
	m_LUT->Black();
	// no LUT displayed for this
	//m_colorBarRenderer->SetLUT(m_LUT);

	vtkVelocityGlyphFilter *geoMapFilter = dynamic_cast<vtkVelocityGlyphFilter*>( m_geoMapFilterList[ m_currentMap ] );
	if ( geoMapFilter == NULL )
	{
		throw CException( CTools::Format( "ERROR in VTK_CWorldPlotVelocityData ctor  : dynamic_cast<vtkVelocityGlyphFilter*>(m_geoMapFilterList[%d]) returns NULL pointer - "
			"m_geoMapFilterList std::list seems to contain objects other than those of the class vtkVelocityGlyphFilter", m_currentMap ), BRATHL_LOGIC_ERROR );
		return;
	}

	m_geoMapFilter = geoMapFilter;
	m_geoMapFilter->SetIsGlyph( IsGlyph );
	SetInput( geoMapFilter->GetOutput() );
}


vtkProj2DFilter* VTK_CWorldPlotVelocityData::GetVtkFilter()
{
     return m_simple2DProjFilter;
}


void VTK_CWorldPlotVelocityData::SetIsGlyph(bool val)
{
    IsGlyph = val;
    m_geoMapFilter->SetIsGlyph(val);

    if ( Is3D() )
        Set3D();
    else
        Set2D();

}

void VTK_CWorldPlotVelocityData::Set2D()
{
    if ( IsGlyph )
        Set2DGlyphs();
    else
        base_t::Set2D();
}

void VTK_CWorldPlotVelocityData::Set3D()
{

    if ( IsGlyph )
        Set3DGlyphs();
    else
        base_t::Set3D();
}

void VTK_CWorldPlotVelocityData::Set2DGlyphs()
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


void VTK_CWorldPlotVelocityData::Set3DGlyphs()
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
