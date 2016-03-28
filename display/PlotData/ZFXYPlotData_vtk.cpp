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

#include <ctime>

#include "new-gui/Common/tools/Trace.h"
#include "new-gui/Common/tools/Exception.h"
#include "libbrathl/LatLonPoint.h"
using namespace brathl;

#if defined (__unix__)
#if defined (__DEPRECATED)          //avoid linux warning in vtk include
#undef __DEPRECATED
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"
#endif	// __unix__
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

#include "PlotData/vtkZFXYPlotFilter.h"

#include "vtkDataSetReader.h"
#include "vtkCastToConcrete.h"
#include "vtkRectilinearGrid.h"
#include "vtkRectilinearGridGeometryFilter.h"
#include "vtkPolyDataWriter.h"
#include "vtkLookupTable.h"

#include "ZFXYPlotData_vtk.h"

#if defined(BRAT_V3)
#include "wx/progdlg.h"
#endif


//-------------------------------------------------------------
//------------------- CLUTZFXYRenderer class --------------------
//-------------------------------------------------------------

CLUTZFXYRenderer::CLUTZFXYRenderer()
{

  m_vtkRend = nullptr;
  m_LUT = nullptr;
  m_scalarBarActor = nullptr;

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
  if (m_scalarBarActor != nullptr)
  {
    m_scalarBarActor->Delete();
    m_scalarBarActor = nullptr;
  }

  if (m_vtkRend != nullptr)
  {
    m_vtkRend->Delete();
    m_vtkRend = nullptr;
  }

  DeleteLUT();

}
//----------------------------------------
void CLUTZFXYRenderer::ResetTextActor(VTK_CZFXYPlotData* zfxyPlotData)
{
  std::string text = zfxyPlotData->GetDataTitle();
  text += "\t\t(" + zfxyPlotData->m_plotProperties.m_name;
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
  if (m_LUT != nullptr)
  {
    delete m_LUT;
    m_LUT = nullptr;
  }
}

//----------------------------------------
void CLUTZFXYRenderer::SetLUT(CBratLookupTable* lut)
{
  if (lut == nullptr)
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
  if (m_scalarBarActor == nullptr)
  {
    return;
  }
  m_scalarBarActor->SetNumberOfLabels(n);
}

//----------------------------------------
int32_t CLUTZFXYRenderer::GetNumberOfLabels()
{
  if (m_scalarBarActor == nullptr)
  {
    return 0;
  }

  return m_scalarBarActor->GetNumberOfLabels();
}

//----------------------------------------
void CLUTZFXYRenderer::SetBackground(double r, double g, double b)
{
  if (m_vtkRend == nullptr)
  {
    return;
  }
  if (m_scalarBarActor == nullptr)
  {
    return;
  }
  m_vtkRend->SetBackground(r, g, b);
  m_scalarBarActor->GetLabelTextProperty()->SetColor(1-r, 1-g, 1-b);
  m_scalarBarActor->GetTitleTextProperty()->SetColor(1-r, 1-g, 1-b);

}


//-------------------------------------------------------------
//------------------- VTK_CZFXYPlotData class --------------------
//-------------------------------------------------------------

VTK_CZFXYPlotData::VTK_CZFXYPlotData( CZFXYPlot* plot, CPlotField* field )
	: base_t( plot, field )
{
	if ( field == nullptr )
	{
		return;
	}

	m_colorBarRenderer = nullptr;

	m_vtkMapper2D = vtkPolyDataMapper2D::New();
	m_vtkMapper2D->SetScalarModeToUseCellData();
	m_vtkMapper2D->UseLookupTableScalarRangeOn();


	m_vtkContourMapper2D = nullptr;

	m_vtkActor2D = vtkActor2D::New();
	m_vtkActor2D->SetMapper( m_vtkMapper2D );
	m_vtkActor2D->PickableOn();

	m_vtkContourActor2D = nullptr;

	m_vtkContourFilter = nullptr;

	m_vtkContourLabelMapper = nullptr;
	m_vtkContourLabelActor = nullptr;

	m_vtkLabelContourData = nullptr;
	m_vtkLabelContourPoints = nullptr;
	m_vtkLabelContourPositions = nullptr;

	m_vtkConnectivity = nullptr;

	m_vtkVisiblePointsFilter = nullptr;
	m_vtkVisibleSpherePointsFilter = nullptr;
	m_vtkVisibleSpherePoints = nullptr;
	m_vtkVisibleSpherePointsData = nullptr;

	m_LUT = nullptr;

	m_vtkRend = nullptr;

	/////////////////////////////////////////////////////
	//the following has the vtk related content of Create
	//using the values of v4 structures filled by the base
	/////////////////////////////////////////////////////

	m_contourLabelNeedUpdateOnWindow = false;
	m_contourLabelNeedUpdatePositionOnContour = false;

	m_colorBarRenderer = new CLUTZFXYRenderer();
	m_colorBarRenderer->GetVtkRenderer()->InteractiveOff();
	m_colorBarRenderer->SetNumberOfLabels( m_plotProperties.m_numColorLabels );

	m_finished = false;

	vtkZFXYPlotFilter* zfxyPlotFilter = nullptr;

	for ( uint32_t iMap = 0; iMap < m_nrMaps; iMap++ )
	{
		vtkDoubleArray* values = vtkDoubleArray::New();
		vtkShortArray* bitarray = vtkShortArray::New();
		vtkDoubleArray* yAxis = vtkDoubleArray::New();
		vtkDoubleArray* xAxis = vtkDoubleArray::New();

		for ( auto v : mMaps( iMap ).mXaxis )
			xAxis->InsertNextValue( v );

		for ( auto v : mMaps( iMap ).mYaxis )
			yAxis->InsertNextValue( v );

		for ( auto v : mMaps( iMap ).mValues )
			values->InsertNextValue( v );

		for ( auto v : mMaps( iMap ).mBits )
			bitarray->InsertNextValue( v );


		zfxyPlotFilter = vtkZFXYPlotFilter::New();

		zfxyPlotFilter->SetId( iMap );

		zfxyPlotFilter->SetValues( values );
		zfxyPlotFilter->SetBitarray( bitarray );
		zfxyPlotFilter->SetXDataArray( xAxis );
		zfxyPlotFilter->SetYDataArray( yAxis );

		zfxyPlotFilter->SetPlotWidth( mMaps( iMap ).mPlotWidth );
		zfxyPlotFilter->SetPlotHeight( mMaps( iMap ).mPlotHeight );

		zfxyPlotFilter->SetXDataRange( mMaps( iMap ).mMinX, mMaps( iMap ).mMaxX );
		zfxyPlotFilter->SetYDataRange( mMaps( iMap ).mMinY, mMaps( iMap ).mMaxY );

		m_zfxyPlotFilterList.Insert( zfxyPlotFilter );

		values->Delete();
		bitarray->Delete();
		xAxis->Delete();
		yAxis->Delete();
	}


	for ( uint32_t iMap = 0; iMap < m_nrMaps; iMap++ )
	{
		zfxyPlotFilter = dynamic_cast<vtkZFXYPlotFilter*>( m_zfxyPlotFilterList[ iMap ] );
		zfxyPlotFilter->SetMinMappedValue( mMaps( iMap ).mMinHeightValue );
		zfxyPlotFilter->SetMaxMappedValue( mMaps( iMap ).mMaxHeightValue );
	}

	SetLUT( m_plotProperties.m_LUT );

	m_LUT->GetLookupTable()->SetTableRange( m_plotProperties.m_minContourValue, m_plotProperties.m_maxContourValue );

	m_colorBarRenderer->SetLUT( m_LUT );


	zfxyPlotFilter = GetCurrentPlotData();

	SetInput( zfxyPlotFilter->GetOutput() );
}
//----------------------------------------
VTK_CZFXYPlotData::~VTK_CZFXYPlotData()
{
  if (m_colorBarRenderer != nullptr)
  {
    delete m_colorBarRenderer;
    m_colorBarRenderer = nullptr;
  }

  DeleteAll2D();

  DeleteVtkContourLabelObject();

  DeleteLUT();

}

//----------------------------------------
void VTK_CZFXYPlotData::NewVtkContourLabelObject()
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
void VTK_CZFXYPlotData::DeleteVtkContourLabelObject()
{
  if (m_vtkVisiblePointsFilter != nullptr)
  {
    m_vtkVisiblePointsFilter->Delete();
    m_vtkVisiblePointsFilter = nullptr;
  }
  if (m_vtkVisibleSpherePointsFilter != nullptr)
  {
    m_vtkVisibleSpherePointsFilter->Delete();
    m_vtkVisibleSpherePointsFilter = nullptr;
  }
  if (m_vtkVisibleSpherePoints != nullptr)
  {
    m_vtkVisibleSpherePoints->Delete();
    m_vtkVisibleSpherePoints = nullptr;
  }
  if (m_vtkVisibleSpherePointsData != nullptr)
  {
    m_vtkVisibleSpherePointsData->Delete();
    m_vtkVisibleSpherePointsData = nullptr;
  }
  if (m_vtkConnectivity != nullptr)
  {
    m_vtkConnectivity->Delete();
    m_vtkConnectivity = nullptr;
  }
  if (m_vtkLabelContourPositions != nullptr)
  {
    m_vtkLabelContourPositions->Delete();
    m_vtkLabelContourPositions = nullptr;
  }
  if (m_vtkLabelContourPoints != nullptr)
  {
    m_vtkLabelContourPoints->Delete();
    m_vtkLabelContourPoints = nullptr;
  }
  if (m_vtkLabelContourData != nullptr)
  {
    m_vtkLabelContourData->Delete();
    m_vtkLabelContourData = nullptr;
  }

}


//----------------------------------------
void VTK_CZFXYPlotData::DeleteContourFilter()
{
  if (m_vtkContourFilter != nullptr)
  {
    m_vtkContourFilter->Delete();
    m_vtkContourFilter = nullptr;
  }

}

//----------------------------------------
void VTK_CZFXYPlotData::DeleteLUT()
{
  if (m_LUT != nullptr)
  {
    delete m_LUT;
    m_LUT = nullptr;
  }

}

//----------------------------------------
void VTK_CZFXYPlotData::DeleteAll2D()
{
  Delete2D();
  DeleteContour2D();

}

//----------------------------------------
void VTK_CZFXYPlotData::Delete2D()
{
  DeleteActor2D();
  DeleteMapper2D();
}

//----------------------------------------
void VTK_CZFXYPlotData::DeleteContour2D()
{
  DeleteContourActor2D();
  DeleteContourMapper2D();
  DeleteContourLabelActor();
  DeleteContourLabelMapper();
  DeleteContourFilter();
}
//----------------------------------------
void VTK_CZFXYPlotData::DeleteContourLabelActor()
{
  if (m_vtkContourLabelActor != nullptr)
  {
    m_vtkContourLabelActor->Delete();
    m_vtkContourLabelActor = nullptr;
  }
}

//----------------------------------------
void VTK_CZFXYPlotData::DeleteActor2D()
{
  if (m_vtkActor2D != nullptr)
  {
    m_vtkActor2D->Delete();
    m_vtkActor2D = nullptr;
  }
}

//----------------------------------------
void VTK_CZFXYPlotData::DeleteContourActor2D()
{
  if (m_vtkContourActor2D != nullptr)
  {
    m_vtkContourActor2D->Delete();
    m_vtkContourActor2D = nullptr;
  }
}

//----------------------------------------
void VTK_CZFXYPlotData::DeleteContourLabelMapper()
{
  if (m_vtkContourLabelMapper != nullptr)
  {
    m_vtkContourLabelMapper->Delete();
    m_vtkContourLabelMapper = nullptr;
  }
}

//----------------------------------------
void VTK_CZFXYPlotData::DeleteMapper2D()
{
  if (m_vtkMapper2D != nullptr)
  {
    m_vtkMapper2D->Delete();
    m_vtkMapper2D = nullptr;
  }

}
//----------------------------------------
void VTK_CZFXYPlotData::DeleteContourMapper2D()
{
  if (m_vtkContourMapper2D != nullptr)
  {
    m_vtkContourMapper2D->Delete();
    m_vtkContourMapper2D = nullptr;
  }

}
//----------------------------------------
bool VTK_CZFXYPlotData::HasActor2D()
{
  return ((m_vtkActor2D != nullptr) || (m_vtkContourActor2D != nullptr));
}

//----------------------------------------
void VTK_CZFXYPlotData::SetLUT(CBratLookupTable* lut)
{
  if (lut == nullptr)
  {
    return;
  }

  DeleteLUT();

  m_LUT = new CBratLookupTable(*lut);

  if (m_vtkMapper2D != nullptr)
  {
    m_vtkMapper2D->SetLookupTable(m_LUT->GetLookupTable());
  }

}

//----------------------------------------
void VTK_CZFXYPlotData::SetInput(vtkPolyData* output)
{

  if (m_vtkMapper2D != nullptr)
  {
    m_vtkMapper2D->SetInput(output);
  }

}

/*
void VTK_CZFXYPlotData::Create( CObArray* data, const std::string& fieldName, CZFXYPlot* plot )
{
	m_contourLabelNeedUpdateOnWindow = false;
	m_contourLabelNeedUpdatePositionOnContour = false;


	if ( m_plotProperties.m_name.empty() )
	{
		m_plotProperties.m_name = fieldName.c_str();
	}

	if ( m_plotProperties.m_title.empty() )
	{
		m_plotProperties.m_title = plot->m_title;
	}


	if ( m_plotProperties.m_xLabel.empty() )
	{
		m_plotProperties.m_xLabel = plot->m_titleX;
	}
	else
	{
		std::string titleX = m_plotProperties.m_xLabel;
		titleX += plot->m_unitXLabel;
		m_plotProperties.m_xLabel = titleX;
	}

	if ( m_plotProperties.m_yLabel.empty() )
	{
		m_plotProperties.m_yLabel = plot->m_titleY;
	}
	else
	{
		std::string titleY = m_plotProperties.m_yLabel;
		titleY += plot->m_unitYLabel;
		m_plotProperties.m_yLabel = titleY;
	}

	m_unitX = plot->m_unitX;
	m_unitY = plot->m_unitY;

	m_colorBarRenderer = new CLUTZFXYRenderer();
	m_colorBarRenderer->GetVtkRenderer()->InteractiveOff();

	m_colorBarRenderer->SetNumberOfLabels( m_plotProperties.m_numColorLabels );


	m_finished = false;
	m_currentMap = 0;
	m_nrMaps = (uint32_t)data->size();

#if defined(BRAT_V3)		// TODO replace by callback device to display progress
	wxProgressDialog pd("Calculating Plot Data", "", m_nrMaps, nullptr, wxPD_CAN_ABORT | wxPD_REMAINING_TIME | wxPD_SMOOTH| wxPD_APP_MODAL);
	pd.SetSize(-1,-1, 350, -1);
#endif


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

	vtkZFXYPlotFilter* zfxyPlotFilter = nullptr;
	double minHeightValue = 0;
	double maxHeightValue = 0;

	for ( iMap = 0; iMap < m_nrMaps; iMap++ )
	{
		mMaps.AddMap();

		setDefaultValue( minXDataMapValue );
		setDefaultValue( maxYDataMapValue );
		setDefaultValue( minYDataMapValue );
		setDefaultValue( maxYDataMapValue );

		minHeightValue = m_plotProperties.m_minHeightValue;
		maxHeightValue = m_plotProperties.m_maxHeightValue;

		CExpressionValue varX;
		CExpressionValue varY;
		CExpressionValue varValue;
		NetCDFVarKind varKind;


		CInternalFiles* zfxy = dynamic_cast<CInternalFiles*>( data->at( iMap ) );
		if ( zfxy == nullptr )
		{
			CException e( "VTK_CZFXYPlotData ctor - dynamic_cast<CInternalFiles*>data->at(iMap) returns nullptr",
				BRATHL_LOGIC_ERROR );
			CTrace::Tracer( "%s", e.what() );
			throw ( e );

		}
		if ( zfxy->IsGeographic() )
		{
			CException e( "VTK_CZFXYPlotData ctor - Geographical data found - zfxy->IsGeographic() is true",
				BRATHL_LOGIC_ERROR );
			CTrace::Tracer( "%s", e.what() );
			throw ( e );

		}


#if defined(BRAT_V3)		// TODO replace by callback device to display progress
		//update ProgressDialog
		std::string msg = CTools::Format("Calculating Frame: %d of %d", iMap + 1, m_nrMaps);
		if (pd.Update(iMap, msg.c_str()) == false)
		{
			m_aborted = true;
			break;
		}
#endif

		int32_t mapWidth = 0;
		int32_t mapHeight = 0;

		uint32_t dimRangeX = 0;
		uint32_t dimRangeY = 1;

		std::string varXName;
		std::string varYName;

		plot->GetPlotWidthHeight( zfxy, fieldName, mapWidth, mapHeight, varX, varY, dimRangeX, dimRangeY, varXName, varYName );

		CUnit unitXRead;
		CUnit unitYRead;

		// Get and control unit of X axis
		// X units are compatible but not the same --> convert
		unitXRead = zfxy->GetUnit( varXName );
		if ( m_unitX.AsString() != unitXRead.AsString() )
		{
			plot->m_unitX.SetConversionFrom( unitXRead );
			plot->m_unitX.ConvertVector( varX.GetValues(), (int32_t)varX.GetNbValues() );
		}

		// Get and control unit of Y axis
		// Y units are compatible but not the same --> convert
		unitYRead = zfxy->GetUnit( varYName );
		if ( m_unitY.AsString() != unitYRead.AsString() )
		{
			plot->m_unitY.SetConversionFrom( unitYRead );
			plot->m_unitY.ConvertVector( varY.GetValues(), (int32_t)varY.GetNbValues() );
		}


		// Get data unit
		CUnit* unit = new CUnit( zfxy->GetUnit( fieldName ) );
		m_dataUnits.Insert( unit );

		// Get data title
		std::string dataTitle = zfxy->GetTitle( fieldName );
		if ( dataTitle.empty() )
		{
			dataTitle = m_plotProperties.m_name;
		}

		m_dataTitles.Insert( dataTitle );


		// read data
		zfxy->ReadVar( fieldName, varValue, unit->GetText() );
		varKind	= zfxy->GetVarKind( fieldName );


		if ( varKind != Data )
		{
			std::string msg = CTools::Format( "VTK_CZFXYPlotData ctor - variable '%s' is not a kind of Data (%d) : %s",
				fieldName.c_str(), Data, CNetCDFFiles::VarKindToString( varKind ).c_str() );
			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
			CTrace::Tracer( "%s", e.what() );
			throw ( e );
		}

		bool transpose = false;

		if ( ( dimRangeY == 0 ) &&
			( dimRangeX == 1 ) )
		{
			transpose = true;
		}

		maxX = mapWidth;
		maxY = mapHeight;


		vtkDoubleArray* values = vtkDoubleArray::New();
		vtkShortArray* bitarray = vtkShortArray::New();
		vtkDoubleArray* yAxis = vtkDoubleArray::New();
		vtkDoubleArray* xAxis = vtkDoubleArray::New();

		for ( iY = 0; iY < maxY; iY++ )
		{
			double valueY = varY.GetValues()[ iY ];
			yAxis->InsertNextValue( valueY );
			mMaps.AddY(valueY);

			if ( iY == 0 )
			{
				if ( iMap == 0 )
				{
					m_yMin = valueY;
					m_yMax = valueY;
				}

				minYDataMapValue = valueY;
				maxYDataMapValue = valueY;
			}
			else
			{
				if ( valueY < m_yMin )
				{
					m_yMin = valueY;
				}
				if ( valueY > m_yMax )
				{
					m_yMax = valueY;
				}
				if ( valueY < minYDataMapValue )
				{
					minYDataMapValue = valueY;
				}
				if ( valueY > maxYDataMapValue )
				{
					maxYDataMapValue = valueY;
				}
			}


			for ( iX = 0; iX < maxX; iX++ )
			{

				int32_t iTemp;
				if ( transpose )
				{
					iTemp = ( iY * maxX ) + iX;
				}
				else
				{
					iTemp = ( iX * maxY ) + iY;
				}

				double v = varValue.GetValues()[ iTemp ];
				if ( ( CTools::IsNan( v ) != 0 ) || isDefaultValue( v ) )
				{

					values->InsertNextValue( 0 );
					bitarray->InsertNextValue( 0 );

					mMaps.AddValue( 0.0 );
					mMaps.AddBit( false );
				}
				else
				{
					values->InsertNextValue( v );
					bitarray->InsertNextValue( 1 );

					mMaps.AddValue( v );
					mMaps.AddBit( true );

					if ( firstValue )
					{
						firstValue = false;
						m_minhv = v;
						m_maxhv = v;
					}
					else
					{
						if ( v < m_minhv )
						{
							m_minhv = v;
						}
						if ( v > m_maxhv )
						{
							m_maxhv = v;
						}
					}
				}
			}
		}

		if ( isDefaultValue( minHeightValue ) )
		{
			minHeightValue = m_minhv;
		}
		if ( isDefaultValue( maxHeightValue ) )
		{
			maxHeightValue = m_maxhv;
		}

		for ( iX = 0; iX < maxX; iX++ )
		{
			double valueX = varX.GetValues()[ iX ];
			xAxis->InsertNextValue( valueX );
			mMaps.AddX( valueX );

			if ( iX == 0 )
			{
				if ( iMap == 0 )
				{
					m_xMin = valueX;
					m_xMax = valueX;
				}

				minXDataMapValue = valueX;
				maxXDataMapValue = valueX;

			}
			else
			{
				if ( valueX < m_xMin )
				{
					m_xMin = valueX;
				}
				if ( valueX > m_xMax )
				{
					m_xMax = valueX;
				}
				if ( valueX < minXDataMapValue )
				{
					minXDataMapValue = valueX;
				}
				if ( valueX > maxXDataMapValue )
				{
					maxXDataMapValue = valueX;
				}
			}

		}

		zfxyPlotFilter = vtkZFXYPlotFilter::New();

		zfxyPlotFilter->SetId( iMap );

		zfxyPlotFilter->SetValues( values );
		zfxyPlotFilter->SetBitarray( bitarray );
		zfxyPlotFilter->SetXDataArray( xAxis );
		zfxyPlotFilter->SetYDataArray( yAxis );

		zfxyPlotFilter->SetPlotWidth( mapWidth );
		zfxyPlotFilter->SetPlotHeight( mapHeight );

		zfxyPlotFilter->SetXDataRange( minXDataMapValue, maxXDataMapValue );
		zfxyPlotFilter->SetYDataRange( minYDataMapValue, maxYDataMapValue );

		m_zfxyPlotFilterList.Insert( zfxyPlotFilter );

		values->Delete();
		bitarray->Delete();
		xAxis->Delete();
		yAxis->Delete();
	}


	if ( isDefaultValue( m_plotProperties.m_minContourValue ) )
	{
		m_plotProperties.m_minContourValue = minHeightValue;
	}

	if ( isDefaultValue( m_plotProperties.m_maxContourValue ) )
	{
		m_plotProperties.m_maxContourValue = maxHeightValue;
	}

#if defined(BRAT_V3)		// TODO replace by callback device to display progress
	pd.Destroy();
#endif

	if ( m_aborted )
	{
		return;
	}

	for ( iMap = 0; iMap < m_nrMaps; iMap++ )
	{
		zfxyPlotFilter = dynamic_cast<vtkZFXYPlotFilter*>( m_zfxyPlotFilterList[ iMap ] );
		zfxyPlotFilter->SetMinMappedValue( minHeightValue );
		zfxyPlotFilter->SetMaxMappedValue( maxHeightValue );
	}

	SetLUT( m_plotProperties.m_LUT );

	m_LUT->GetLookupTable()->SetTableRange( minHeightValue, maxHeightValue );

	m_colorBarRenderer->SetLUT( m_LUT );


	zfxyPlotFilter = GetCurrentPlotData();

	SetInput( zfxyPlotFilter->GetOutput() );
}
*/
//----------------------------------------
vtkZFXYPlotFilter*  VTK_CZFXYPlotData::GetCurrentPlotData()
{
  vtkZFXYPlotFilter* zfxyPlotFilter = dynamic_cast<vtkZFXYPlotFilter*>(m_zfxyPlotFilterList[m_currentMap]);
  if (zfxyPlotFilter == nullptr)
  {
    throw CException(CTools::Format("ERROR in VTK_CZFXYPlotData::GetCurrentPlotData : dynamic_cast<vtkZFXYPlotFilter*>(m_zfxyPlotFilterList[%d]) returns nullptr pointer - "
                     "m_zfxyPlotFilterList std::list seems to contain objects other than those of the class vtkZFXYPlotFilter", m_currentMap),
                     BRATHL_LOGIC_ERROR);

  }

  return zfxyPlotFilter;
}
//----------------------------------------
vtkZFXYPlotFilter*  VTK_CZFXYPlotData::GetPlotData(int32_t i)
{
  vtkZFXYPlotFilter* zfxyPlotFilter = dynamic_cast<vtkZFXYPlotFilter*>(m_zfxyPlotFilterList[i]);
  if (zfxyPlotFilter == nullptr)
  {
    throw CException(CTools::Format("ERROR in VTK_CZFXYPlotData::GetPlotData : dynamic_cast<vtkZFXYPlotFilter*>(m_zfxyPlotFilterList[%d]) returns nullptr pointer - "
                     "m_zfxyPlotFilterList std::list seems to contain objects other than those of the class vtkZFXYPlotFilter", i),
                     BRATHL_LOGIC_ERROR);

  }

  return zfxyPlotFilter;
}
//----------------------------------------
void VTK_CZFXYPlotData::OnKeyframeChanged(uint32_t i)
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
void VTK_CZFXYPlotData::SetContour2D()
{
  DeleteContour2D();

  if (m_plotProperties.m_withContour == false)
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
  c2p = nullptr;


  m_vtkContourMapper2D = vtkPolyDataMapper2D::New();
  m_vtkContourMapper2D->SetInput(m_vtkContourFilter->GetOutput());

  m_vtkContourMapper2D->ScalarVisibilityOff();

  m_vtkContourActor2D = vtkActor2D::New();
  m_vtkContourActor2D->SetMapper(m_vtkContourMapper2D);

  SetContour2DProperties();

  CreateContourLabels2D();
}

//----------------------------------------
void VTK_CZFXYPlotData::SetContourLabelProperties()
{
  if (m_vtkContourLabelMapper == nullptr)
  {
    return;
  }

  vtkTextProperty* text = m_vtkContourLabelMapper->GetLabelTextProperty();

  text->SetFontFamilyToArial();
  text->SetFontSize(m_plotProperties.m_contourLabelSize);
  text->ShadowOff();
  text->ItalicOff();
  text->SetJustificationToCentered();
  text->SetColor(m_plotProperties.m_contourLabelColor.Red(),
                 m_plotProperties.m_contourLabelColor.Green(),
                 m_plotProperties.m_contourLabelColor.Blue()
                 );

  m_vtkContourLabelMapper->SetLabelFormat(m_plotProperties.m_contourLabelFormat.c_str());
  m_vtkContourLabelMapper->SetLabelModeToLabelScalars();

}

//----------------------------------------
void VTK_CZFXYPlotData::ContourGenerateValues()
{
  if (m_vtkContourFilter == nullptr)
  {
    return;
  }

  m_vtkContourFilter->GenerateValues(m_plotProperties.m_numContour,
                                     m_plotProperties.m_minContourValue,
                                     m_plotProperties.m_maxContourValue
                                     );
  m_contourLabelNeedUpdateOnWindow = true;

}
//----------------------------------------
void VTK_CZFXYPlotData::SetContour2DProperties()
{
  if (m_vtkContourActor2D == nullptr)
  {
    return;
  }

  m_vtkContourActor2D->GetProperty()->SetColor(m_plotProperties.m_contourLineColor.Red(),
                                             m_plotProperties.m_contourLineColor.Green(),
                                             m_plotProperties.m_contourLineColor.Blue()
                                             );

  m_vtkContourActor2D->GetProperty()->SetLineWidth(m_plotProperties.m_contourLineWidth);

}

//----------------------------------------
void VTK_CZFXYPlotData::FindVisiblePlanePoints(void* arg)
{
  VTK_CZFXYPlotData* zfxyPlotData = static_cast<VTK_CZFXYPlotData*>(arg);
  if (zfxyPlotData == nullptr)
  {
    return;
  }

  // Finds visible points on sphere based on cosine angle of points
  // relative to camera view std::vector.
  // Works on assumption that sphere origin is world coord (0,0,0).
  vtkPolyData* input = zfxyPlotData->GetVtkVisibleSpherePointsFilter()->GetPolyDataInput();
  vtkPolyData* output = zfxyPlotData->GetVtkVisibleSpherePointsFilter()->GetPolyDataOutput();
  vtkPoints* pts = input->GetPoints();
  if (pts == nullptr)
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
void VTK_CZFXYPlotData::CreateContourLabels2D()
{
  if (m_plotProperties.m_withContour == false)
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
  m_vtkVisibleSpherePointsFilter->SetExecuteMethod(VTK_CZFXYPlotData::FindVisiblePlanePoints, static_cast<void*>(this));

  m_vtkContourLabelMapper->SetInput(m_vtkVisibleSpherePointsFilter->GetPolyDataOutput());

  m_vtkContourLabelActor = vtkActor2D::New();

  m_vtkContourLabelActor->SetMapper(m_vtkContourLabelMapper);

  UpdateContourLabels2D();

}

//----------------------------------------
void VTK_CZFXYPlotData::UpdateContourLabels2D()
{
  if (m_plotProperties.m_withContour == false)
  {
    return;
  }

  if (m_plotProperties.m_withContourLabel == false)
  {
    return;
  }

  if (m_vtkContourLabelActor == nullptr)
  {
    return;
  }

  //wxSetCursor(*wxHOURGLASS_CURSOR);

  //Labels every contour.
  //If the data min/max is changed, this must be called manually to update the labels.

  m_vtkConnectivity->InitializeSpecifiedRegionList();
  m_vtkConnectivity->Update();
  int32_t nContours = m_vtkConnectivity->GetNumberOfExtractedRegions();

  int32_t nLabels = m_plotProperties.m_numContourLabel;


  m_vtkLabelContourPositions->Reset();
  vtkPoints* labelPoints = m_vtkLabelContourPositions->GetPoints();
  labelPoints->Allocate(nLabels * nContours, 1);

  m_vtkLabelContourData->SetNumberOfValues(nLabels * nContours);
  int32_t insertctr = 0;

  // Set evil seed (initial seed)
  srand( (unsigned)time( nullptr ) );

  m_labelPts.RemoveAll();
  m_labelIds.RemoveAll();
  m_labelData.RemoveAll();

  vtkPoints* pts = nullptr;
  vtkIdTypeArray* ids = nullptr;
  vtkDataArray* data = nullptr;

  vtkFloatingPointType* xyz = nullptr;
  vtkFloatingPointType* xyzViewPort = nullptr;

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
void VTK_CZFXYPlotData::SetContourLabels2DPosition()
{
  if (m_plotProperties.m_withContour == false)
  {
    return;
  }

  if (m_plotProperties.m_withContourLabel == false)
  {
    return;
  }

  if (m_vtkContourLabelActor == nullptr)
  {
    return;
  }

  wxSetCursor(*wxHOURGLASS_CURSOR);

  int32_t nLabels = m_plotProperties.m_numContourLabel;
  int32_t nContours =  m_labelPts.size();

  m_vtkLabelContourPositions->Reset();
  vtkPoints* labelPoints = m_vtkLabelContourPositions->GetPoints();
  labelPoints->Allocate(nLabels * nContours, 1);

  m_vtkLabelContourData->SetNumberOfValues(nLabels * nContours);
  int32_t insertctr = 0;

  // Set evil seed (initial seed)
  srand( (unsigned)time( nullptr ) );

  vtkPoints* pts = nullptr;
  vtkIdTypeArray* ids = nullptr;
  vtkDataArray* data = nullptr;

  vtkFloatingPointType* xyz = nullptr;
  vtkFloatingPointType* xyzViewPort = nullptr;

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

