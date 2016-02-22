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

#include "new-gui/Common/tools/Trace.h"
#include "new-gui/Common/tools/Exception.h"
#include "libbrathl/ExternalFiles.h"
#include "libbrathl/ExternalFilesNetCDF.h"
#include "libbrathl/ExternalFilesFactory.h"
#include "libbrathl/Field.h"
#include <QDebug>

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
#include "vtkObjectFactory.h"

#if defined(BRAT_V3)
#include "new-gui/brat/DataModels/PlotData/MapColor.h"
#include "wxInterface.h"
#include "wx/progdlg.h"
#endif


#define register        //avoid clang error in qt header
#include "new-gui/brat/ApplicationLogger.h"

#include "XYPlotData_vtk.h"


//-------------------------------------------------------------
//------------------- vtkDoubleArrayBrathl class --------------------
//-------------------------------------------------------------

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkDoubleArrayBrathl);

//----------------------------------------------------------------------------
void vtkDoubleArrayBrathl::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->vtkDoubleArray::PrintSelf(os,indent);
}
//----------------------------------------------------------------------------

/*
Function description (assuming comp=0):
Long:
this object has an associated timestamping object that
dates the last time the object has been modifyed (according
 to the comment discription)
ComponentRangeComputeTime2[0] is a timestamp that dates the
laste time a Range2 has been computed. And only execute the
computation algorithm if the object has been modifyed later than
ComponentRangeComputeTime2[0].
Range2[0] is the lowerbound of the entire vtkDoubleArray
Range2[1] is the upperbound of the entire vtkDoubleArray.

Short:

Calculates the lower and upper bound for the vtkDoubleArray. Skips
values that are default. Only execute the max/min algo if there
are actual modifications to the object itself.
*/
void vtkDoubleArrayBrathl::ComputeRange( int comp )
{
	//---------------------------
	vtkDoubleArray::ComputeRange( comp );
	//---------------------------
	double s, t;
	vtkIdType numTuples;

	if ( comp < 0 && this->NumberOfComponents == 1 )
	{
		comp = 0;
	}

	int idx = comp;
	idx = ( idx<0 ) ? ( 4 ) : ( idx );

    //only update if the object has been modifyed
    //Update if it the object time is greater than
    // the timestamp the last computation on Range2
	if ( ( this->GetMTime() > this->ComponentRangeComputeTime2[ idx ] ) )
	{
		numTuples=this->GetNumberOfTuples();
		this->Range2[ 0 ] =  VTK_DOUBLE_MAX;
		this->Range2[ 1 ] =  VTK_DOUBLE_MIN;

		for ( vtkIdType i=0; i < numTuples; i++ )
		{
            //case comp is positive >=0
			if ( comp >= 0 )
			{
                //get the component of the i-th tuple and j-th element
                // if we use j=0 (comp=0) we are just accessing the
               // i-th element from the vtkdoublearray
				s = this->GetComponent( i, comp );
                // search for the next non-default value
                // on the array
				if ( isDefaultValue( s ) )
				{
					//------------------
					continue;
					//------------------
				}
			}
            //case comp is negative (-1)
			else
			{ // Compute range of std::vector magnitude.
				s = 0.0;
				for ( int j=0; j < this->NumberOfComponents; ++j )
				{
					t = this->GetComponent( i, j );
					if ( isDefaultValue( t ) )
					{
						//------------------
						continue;
						//------------------
					}
					s += t*t;
				}
				s = sqrt( s );
			}
            // we only get here if s is not a
            // default value
			if ( s < this->Range2[ 0 ] )
			{
				this->Range2[ 0 ] = s;
			}
			if ( s > this->Range2[ 1 ] )
			{
				this->Range2[ 1 ] = s;
			}
		}
		this->ComponentRangeComputeTime2[ idx ].Modified();
		this->ComponentRange2[ idx ][ 0 ] = this->Range2[ 0 ];
		this->ComponentRange2[ idx ][ 1 ] = this->Range2[ 1 ];
	}
	else
	{
		this->Range2[ 0 ] = this->ComponentRange2[ idx ][ 0 ];
		this->Range2[ 1 ] = this->ComponentRange2[ idx ][ 1 ];
	}
}



//-------------------------------------------------------------
//------------------- vtkDataArrayPlotData class
//-------------------------------------------------------------

//----------------------------------------------------------------------------
vtkDataArrayPlotData::vtkDataArrayPlotData()
{
  this->XDataArray = nullptr;
  this->YDataArray = nullptr;
  this->ZDataArray = nullptr;
  this->NumberOfItems = 0;
}

//----------------------------------------------------------------------------
vtkDataArrayPlotData::~vtkDataArrayPlotData()
{
  if (this->XDataArray != nullptr)
    {
    this->XDataArray->UnRegister(this);
    }
  if (this->YDataArray != nullptr)
    {
    this->YDataArray->UnRegister(this);
    }
  if (this->ZDataArray != nullptr)
    {
    this->ZDataArray->UnRegister(this);
    }
}

//----------------------------------------------------------------------------
void vtkDataArrayPlotData::SetXDataArray( vtkDoubleArrayBrathl *dataArray )
{
	if ( this->XDataArray != dataArray )
	{
		if ( dataArray != nullptr )
		{
			dataArray->Register( this );
		}
		if ( this->XDataArray != nullptr )
		{
			this->XDataArray->UnRegister( this );
		}
		this->XDataArray = dataArray;
		this->Modified();
	}
}

//----------------------------------------------------------------------------
void vtkDataArrayPlotData::SetYDataArray(vtkDoubleArrayBrathl *dataArray)
{
  if (this->YDataArray != dataArray)
    {
    if (dataArray != nullptr)
      {
      dataArray->Register(this);
      }
    if (this->YDataArray != nullptr)
      {
      this->YDataArray->UnRegister(this);
      }
    this->YDataArray = dataArray;
    this->Modified();
    }
}

//----------------------------------------------------------------------------
void vtkDataArrayPlotData::SetZDataArray(vtkDoubleArrayBrathl *dataArray)
{
  if (this->ZDataArray != dataArray)
    {
    if (dataArray != nullptr)
      {
      dataArray->Register(this);
      }
    if (this->ZDataArray != nullptr)
      {
      this->ZDataArray->UnRegister(this);
      }
    this->ZDataArray = dataArray;
    this->Modified();
    }
}

//----------------------------------------------------------------------------
// NumberOfItems is set to the minimum number of tuples of all data arrays.
// Only data arrays that are not null pointers are checked. If both the X, Y
// and Z data arrays are null pointers then NumberOfItems is set to 0.
void vtkDataArrayPlotData::Execute()
{
  vtkIdType numberOfTuples;

  vtkDebugMacro(<<"Updating NumberOfItems");

  this->NumberOfItems = -1;

  if (this->XDataArray != nullptr)
    {
    numberOfTuples = this->XDataArray->GetNumberOfTuples();
    if (this->NumberOfItems == -1)
      {
      this->NumberOfItems = numberOfTuples;
      }
    }
  if (this->YDataArray != nullptr)
    {
    numberOfTuples = this->YDataArray->GetNumberOfTuples();
    if (this->NumberOfItems == -1 || this->NumberOfItems > numberOfTuples)
      {
      this->NumberOfItems = numberOfTuples;
      }
    }
  if (this->ZDataArray != nullptr)
    {
    numberOfTuples = this->ZDataArray->GetNumberOfTuples();
    if (this->NumberOfItems == -1 || this->NumberOfItems > numberOfTuples)
      {
      this->NumberOfItems = numberOfTuples;
      }
    }
  if (this->NumberOfItems == -1)
    {
    this->NumberOfItems = 0;
    }

  this->vtkPlotData::Execute();
}

//----------------------------------------------------------------------------
double vtkDataArrayPlotData::GetXValue(int i)
{
  if (this->XDataArray != nullptr)
    {
    return this->XDataArray->GetTuple1(i);
    }
  return 0.0;
}

//----------------------------------------------------------------------------
double vtkDataArrayPlotData::GetYValue(int i)
{
  if (this->YDataArray != nullptr)
    {
    return this->YDataArray->GetTuple1(i);
    }
  return 0.0;
}

//----------------------------------------------------------------------------
double vtkDataArrayPlotData::GetZValue(int i)
{
  if (this->ZDataArray != nullptr)
    {
    return this->ZDataArray->GetTuple1(i);
    }
  return 0.0;
}

//----------------------------------------------------------------------------
int vtkDataArrayPlotData::GetNumberOfItems()
{
  return this->NumberOfItems;
}

//----------------------------------------------------------------------------
unsigned long vtkDataArrayPlotData::GetMTime()
{
  unsigned long mTime = this->vtkPlotData::GetMTime();
  unsigned long time;

  vtkDebugMacro(<<"Returning modification time");

  if (this->XDataArray != nullptr)
    {
    time = this->XDataArray->GetMTime();
    mTime = time > mTime ? time : mTime;
    }
  if (this->YDataArray != nullptr)
    {
    time = this->YDataArray->GetMTime();
    mTime = time > mTime ? time : mTime;
    }
  if (this->ZDataArray != nullptr)
    {
    time = this->ZDataArray->GetMTime();
    mTime = time > mTime ? time : mTime;
    }

  return mTime;
}




//-------------------------------------------------------------
//------------------- CPlotArray class --------------------
//-------------------------------------------------------------
CPlotArray::CPlotArray()
{
  m_didCalculateRange = false;
  setDefaultValue(m_min);
  setDefaultValue(m_max);
}
//----------------------------------------
CPlotArray::~CPlotArray()
{
}
//----------------------------------------
vtkDoubleArrayBrathl* CPlotArray::GetFrameData(size_t r)
{
  if (r >= m_rows.size())
  {
    r = m_rows.size() - 1;
  }

  vtkDoubleArrayBrathl* data = dynamic_cast<vtkDoubleArrayBrathl*>(m_rows.at(r));
  if (data == nullptr)
  {
    std::string msg =
            "ERROR in CPlotArray::GetFrameData: dynamic_cast<vtkDoubleArrayBrathl*>(m_rows.at("
            + n2s<std::string>( r )
            + ") returns nullptr pointer - "
            "Plot Array (rows) seems to contain objects other than those of the class vtkDoubleArrayBrathl class";
    CException e(msg, BRATHL_LOGIC_ERROR);
    throw (e);
  }

  return data;

}
//----------------------------------------
vtkDoubleArrayBrathl* CPlotArray::GetFrameData(vtkObArray::iterator it)
{
  vtkDoubleArrayBrathl* data = dynamic_cast<vtkDoubleArrayBrathl*>(*it);
  if (data == nullptr)
  {
    throw CException("ERROR in  GetFrameData(CObArray::iterator it) : dynamic_cast<vtkDoubleArrayBrathl*>(*it); returns nullptr pointer - "
                 "CPlot Array seems to contain objects other than those of the class vtkDoubleArrayBrathl or derived class", BRATHL_LOGIC_ERROR);
  }
  return data;

}
//----------------------------------------
void CPlotArray::SetFrameData( uint32_t r, double* vect, int32_t size )
{
	const bool bCopy = 

#if defined(BRAT_V3)
		false;
#else
		true;
#endif

	double* newVect = nullptr;
	if ( bCopy )
	{
		newVect = new double[ size ];
		memcpy( newVect, vect, size * sizeof( *vect ) );
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
	vtkArray->SetArray( newVect, size, 0 );

	SetFrameData( r, vtkArray );
}
//----------------------------------------
void CPlotArray::SetFrameData( uint32_t r, vtkDoubleArrayBrathl* vect )
{
	//if (r == m_rows.size())
	if ( r >= m_rows.size() )
	{
		m_rows.Insert( vect );
	}
	else if ( r < m_rows.size() )
	{
		m_rows.ReplaceAt( m_rows.begin() + r, vect );
	}
	else
	{
		std::string msg = CTools::Format( "ERROR in  CPlotArray::SetFrameData : index %d out-of-range : [0, %ld]",
			r, (long)m_rows.size() );
		CException e( msg, BRATHL_LOGIC_ERROR );
		throw ( e );

	}
	m_didCalculateRange = false;
}
//----------------------------------------
void CPlotArray::GetRange(double& min, double& max, uint32_t frame)
{

  setDefaultValue(min);
  setDefaultValue(max);

  vtkDoubleArrayBrathl* data = GetFrameData(frame);
  if (data == nullptr)
  {
    return;
  }

  min = data->GetRange2()[0];
  max = data->GetRange2()[1];

}
//----------------------------------------
/*
Description:
CPlotArray object contains a list of several
vtkDoubleArrayBrathl. This function updates the general
lower and upper bound m_min and m_max. By the end of this
function m_min should be the least value on all elements
on each vtkDoubleArrayBrathl and m_max should be the greatest
of these values.
*/
void CPlotArray::GetRange(double& min, double& max)
{
  if (m_didCalculateRange)
  { // we calculated the range recently
    min = m_min;
    max = m_max;
    return;
  }

  //we did not calculate the range recently
  vtkObArray::iterator it;
  setDefaultValue(min);
  setDefaultValue(max);

  //process every row of plotarray
  //CplotArray contains a list of several
  // vtkadoublearrays
  for (it = m_rows.begin(); it != m_rows.end() ; it++)
  {
    //fectch the i-th vtkdoublearray
    vtkDoubleArrayBrathl* data = GetFrameData(it);
    //that vtkdoublearrays' lowerbound
    double l = data->GetRange2()[0];
    //that vtkdoublearrays' upperbound
    double h = data->GetRange2()[1];

    //this was already commented
    //data->GetRange(l, h);


    if (isDefaultValue(min))
    {
        //if our min is a default
      //min is current vtkdoublearray's lowerbound
      min = l;
      //max is the current vtkdoublearray's upperbound
      max = h;
    }
    else
    {
        //otherwise:
        //basically here we calculate(and update) the
        //newer general upperbound (min, max)

      min = (min > l) ? l : min;
      max = (max > h) ? max : h;
    }
  }

  //update these values (m_min, m_max)
  m_min = min;
  m_max = max;

  //set this boolean as true, we calculated this range
  m_didCalculateRange = true;
}
//----------------------------------------
void CPlotArray::Normalize( int32_t value )
{
	double l = 0;
	double h = 0;
	GetRange( l, h );

	for ( vtkObArray::iterator it = m_rows.begin(); it != m_rows.end(); it++ )
	{
		vtkDoubleArrayBrathl* data = GetFrameData( it );
		for ( int i = 0; i < data->GetNumberOfTuples(); i++ )
		{
			data->SetValue( i, ( ( data->GetValue( i ) - l ) / ( h - l ) ) * value );
		}
	}
	m_didCalculateRange = false;
}

////-------------------------------------------------------------
////------------------- CXYPlotProperties class --------------------
////-------------------------------------------------------------
//
//
//VTK_CXYPlotData* CXYPlotProperties::parent()
//{
//	return dynamic_cast< VTK_CXYPlotData* >( m_parent );
//}
////virtual 
//void CXYPlotProperties::SetName()
//{
//	parent()->GetVtkDataArrayPlotData()->SetPlotLabel( m_name.c_str() );
//}
//
//
//
//CXYPlotProperties::CXYPlotProperties(VTK_CXYPlotData* parent)
//{
//  this->Init();
//
//  m_vtkProperty2D = vtkProperty2D::New();
//
//
//  SetLineStipple(displayFULL);
//
//  SetLineWidth(0.8);
//
//
//
//
//  SetOpacity(0.6);
//  SetOpacityFactor(1 / GetOpacity());
//
//
//  CPlotColor vtkColor = CMapColor::GetInstance().NextPrimaryColors();
//  if (vtkColor.Ok())
//  {
//    SetColor(vtkColor);
//  }
//  else
//  {
//    SetColor(CPlotColor(0.0, 0.0, 0.0));
//  }
//
//  SetPointGlyph(displayCIRCLE_GLYPH);
//  SetFilledPoint(true);
//
//  SetLineWidthFactor(1.5);
//
//  SetHide(false);
//}
//
////----------------------------------------
//CXYPlotProperties::CXYPlotProperties(CXYPlotProperties& p)
//{
//  this->Init();
//  this->Copy(p);
//}
//
////----------------------------------------
//const CXYPlotProperties& CXYPlotProperties::operator=(const CXYPlotProperties& p)
//{
//  this->Copy(p);
//  return *this;
//}
//
////----------------------------------------
//CXYPlotProperties::~CXYPlotProperties()
//{
//  if (m_vtkProperty2D != nullptr)
//  {
//    m_vtkProperty2D->Delete();
//    m_vtkProperty2D = nullptr;
//  }
//
//}
////----------------------------------------
//void CXYPlotProperties::Copy(const CXYPlotProperties& p)
//{
//
//  m_vtkProperty2D = vtkProperty2D::New();
//
//
//  SetLineStipple(p.GetLineStipple());
//
//  SetLineWidth(p.GetLineWidth());
//
//  SetLines(p.GetLines());
//  SetPoints(p.GetPoints());
//
//  SetPointSize(p.GetPointSize());
//
//  SetOpacity(p.GetOpacity());
//
//  SetColor(p.GetColor());
//
//  SetName(p.GetName());
//
//  SetPointGlyph(p.GetPointGlyph());
//  SetFilledPoint(p.GetFilledPoint());
//
//  SetOpacityFactor(p.GetOpacityFactor());
//
//  SetHide(p.GetHide());
//
//
//}
////----------------------------------------
//void CXYPlotProperties::Update()
//{
//  SetLines();
//
//  SetPoints();
//  SetPointSize();
//
//  SetPointGlyph();
//  SetFilledPoint();
//
//  SetNormalizeX();
//  SetNormalizeY();
//
//}
////----------------------------------------
//bool CXYPlotProperties::HasParent()
//{
//  return m_parent != nullptr;
//}
////----------------------------------------
//void CXYPlotProperties::SetHide()
//{
//  if (!HasParent())
//  {
//    return;
//  }
//
//  if (m_hide)
//  {
//    parent()->GetVtkDataArrayPlotData()->PlotLinesOff();
//    parent()->GetVtkDataArrayPlotData()->PlotPointsOff();
//  }
//  else
//  {
//    SetLines();
//    SetPoints();
//  }
//
//}
//
////----------------------------------------
//void CXYPlotProperties::SetLineWidth(double value)
//{
//  if (value <= 0.0)
//  {
//    value = 0.8;
//  }
//  m_vtkProperty2D->SetLineWidth(value);
//}
////----------------------------------------
//void CXYPlotProperties::SetLines()
//{
//
//  if (!HasParent())
//  {
//    return;
//  }
//
//  if (m_lines)
//  {
//    parent()->GetVtkDataArrayPlotData()->PlotLinesOn();
//  }
//  else
//  {
//    parent()->GetVtkDataArrayPlotData()->PlotLinesOff();
//  }
//
//}
////----------------------------------------
//void CXYPlotProperties::SetPoints()
//{
//
//  if (!HasParent())
//  {
//    return;
//  }
//
//  if (m_points)
//  {
//    parent()->GetVtkDataArrayPlotData()->PlotPointsOn();
//  }
//  else
//  {
//    parent()->GetVtkDataArrayPlotData()->PlotPointsOff();
//  }
//
//  SetFilledPoint();
//
//}
////----------------------------------------
//void CXYPlotProperties::SetPointSize()
//{
//  if (!HasParent())
//  {
//    return;
//  }
//
//  parent()->GetVtkDataArrayPlotData()->SetGlyphSize(m_pointSize / 100.0);
//
//}
////----------------------------------------
//void CXYPlotProperties::SetPointGlyph()
//{
//
//  if (!HasParent())
//  {
//    return;
//  }
//  if (m_points == false)
//  {
//    return;
//  }
//
//  parent()->GetVtkDataArrayPlotData()->GetGlyphSource()->SetGlyphType(m_glyphType);
//
//}
////----------------------------------------
//void CXYPlotProperties::SetFilledPoint()
//{
//
//  if (!HasParent())
//  {
//    return;
//  }
//  if (m_points == false)
//  {
//    return;
//  }
//
//  if (m_filledPoint)
//  {
//    parent()->GetVtkDataArrayPlotData()->GetGlyphSource()->FilledOn();
//  }
//  else
//  {
//    parent()->GetVtkDataArrayPlotData()->GetGlyphSource()->FilledOff();
//  }
//
//}
////----------------------------------------
//void CXYPlotProperties::SetOpacity(double value)
//{
//  if (value > 1.0)
//  {
//    value = 1.0;
//  }
//
//  if (value <= 0.0)
//  {
//    value = 0.6;
//  }
//
//  m_opacityFactor = 1 / value;
//
//  m_vtkProperty2D->SetOpacity(value);
//}
//
//

//-------------------------------------------------------------
//------------------- VTK_CXYPlotData class --------------------
//-------------------------------------------------------------

VTK_CXYPlotData::VTK_CXYPlotData( CPlot* plot, int32_t iField )
	: base_t( plot, iField )
{
	m_vtkDataArrayPlotData = vtkDataArrayPlotData::New();

	Create();
}

//----------------------------------------
VTK_CXYPlotData::~VTK_CXYPlotData()
{
	if ( m_vtkDataArrayPlotData != nullptr )
	{
		m_vtkDataArrayPlotData->Delete();
		m_vtkDataArrayPlotData = nullptr;
	}
}
//----------------------------------------
//void VTK_CXYPlotData::Create( CInternalFiles* yfx, CPlot* plot, int32_t iField )
//{
//	std::string varXName;
//
//	CUnit unitXRead;
//	CUnit unitYRead;
//
//	std::string unitXStr;
//	std::string unitYStr;
//
//	if ( m_plotProperty.GetTitle().empty() )
//	{
//		m_plotProperty.SetTitle( plot->m_title );
//	}
//
//	if ( m_plotProperty.GetXLabel().empty() )
//	{
//		m_plotProperty.SetXLabel( plot->m_titleX );
//	}
//	else
//	{
//		std::string titleX = m_plotProperty.GetXLabel();
//		titleX += plot->m_unitXLabel;
//		m_plotProperty.SetXLabel( titleX );
//	}
//
//	if ( m_plotProperty.GetYLabel().empty() )
//	{
//		m_plotProperty.SetYLabel( plot->m_titleY );
//	}
//	else
//	{
//		std::string titleY = m_plotProperty.GetYLabel();
//		titleY += plot->m_unitYLabel;
//		m_plotProperty.SetYLabel( titleY );
//	}
//
//	m_unitX = plot->m_unitX;
//	m_unitY = plot->m_unitY;
//
//	CExpressionValue varX;
//
//	ExpressionValueDimensions dimValX;
//
//	NetCDFVarKind varKind;
//	CStringArray varDimNames;
//
//
//	// Get X axis data
//	plot->GetAxisX( yfx, &dimValX, &varX, &varXName );
//
//	// Get and control unit of X axis
//	// X units are compatible but not the same --> convert
//	unitXRead = yfx->GetUnit( varXName );
//	if ( m_unitX.AsString() != unitXRead.AsString() )
//	{
//		plot->m_unitX.SetConversionFrom( unitXRead );
//		plot->m_unitX.ConvertVector( varX.GetValues(), (int32_t)varX.GetNbValues() );
//	}
//
//	CExpressionValue varY;
//	std::string fieldName = plot->GetPlotField( iField )->m_name.c_str();
//
//	// Set name of the data
//
//	if ( GetName().empty() )
//	{
//		std::string str = yfx->GetTitle( fieldName ) + " (" + fieldName + ")";
//		SetName( str.c_str() );
//	}
//
//	// Read Y data dimension
//	ExpressionValueDimensions dimValY;
//
//	yfx->GetVarDims( fieldName, dimValY );
//	if ( ( dimValY.size() <= 0 ) || ( dimValY.size() > 2 ) )
//	{
//		std::string msg = CTools::Format( "VTK_CXYPlotData::Create - '%s' axis -> number of dimensions  must be 1 or 2 - Found : %ld",
//			fieldName.c_str(), (long)dimValY.size() );
//		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
//		LOG_TRACE( e.what() );
//		throw ( e );
//	}
//
//	// Read Y data
//	unitYRead = yfx->GetUnit( fieldName );
//	yfx->ReadVar( fieldName, varY, unitYRead.GetText() );
//	varKind	= yfx->GetVarKind( fieldName );
//	/************************DEDEDE
//	if (varKind != Data)
//	{
//	std::string msg = CTools::Format("VTK_CXYPlotData::Create - variable '%s' is not a kind of Data (%d) : %s",
//	fieldName.c_str(), Data, CNetCDFFiles::VarKindToString(varKind).c_str());
//	CException e(msg, BRATHL_INCONSISTENCY_ERROR);
//	CTrace::Tracer("%s", e.what());
//	throw (e);
//	}
//	*/
//	// Y units are compatible but not the same --> convert
//	if ( m_unitY.AsString() != unitYRead.AsString() )
//	{
//		plot->m_unitY.SetConversionFrom( unitYRead );
//		plot->m_unitY.ConvertVector( varY.GetValues(), (int32_t)varY.GetNbValues() );
//	}
//
//	CStringArray commonDims;
//	bool intersect = yfx->GetCommonVarDims( varXName, fieldName, commonDims );
//
//	if ( !intersect )
//	{
//		std::string msg = CTools::Format( "VTK_CXYPlotData::Create - variables '%s' and '%s' have no common dimension",
//			fieldName.c_str(), varXName.c_str() );
//		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
//		LOG_TRACE( e.what() );
//		throw ( e );
//
//	}
//
//	CStringArray dimXNames;
//	CStringArray dimYNames;
//	yfx->GetVarDims( varXName, dimXNames );
//	yfx->GetVarDims( fieldName, dimYNames );
//
//	if ( ( commonDims.size() == dimXNames.size() ) && ( commonDims.size() == dimYNames.size() ) && ( dimXNames != dimYNames ) )
//	{
//		std::string msg = CTools::Format( "VTK_CXYPlotData::Create - variables '%s' (dim %s) and '%s' (dim %s) must have the same dimension fields",
//			fieldName.c_str(), dimYNames.ToString().c_str(), varXName.c_str(), dimXNames.ToString().c_str() );
//		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
//		LOG_TRACE( e.what() );
//		throw ( e );
//
//	}
//
//
//	CUIntArray xCommonDimIndex;
//	CUIntArray yCommonDimIndex;
//
//	for ( uint32_t iIntersect = 0; iIntersect < commonDims.size(); iIntersect++ )
//	{
//		int32_t dimIndex = -1;
//
//		dimIndex = yfx->GetVarDimIndex( varXName, commonDims.at( iIntersect ) );
//		if ( dimIndex < 0 )
//		{
//			std::string msg = CTools::Format( "VTK_CXYPlotData::Create - variables '%s' - dim '%s' not found ",
//				varXName.c_str(), commonDims.at( iIntersect ).c_str() );
//			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
//			LOG_TRACE( e.what() );
//			throw ( e );
//		}
//
//		xCommonDimIndex.Insert( dimIndex );
//
//		dimIndex = yfx->GetVarDimIndex( fieldName, commonDims.at( iIntersect ) );
//		if ( dimIndex < 0 )
//		{
//			std::string msg = CTools::Format( "VTK_CXYPlotData::Create - variables '%s' - dim '%s' not found ",
//				fieldName.c_str(), commonDims.at( iIntersect ).c_str() );
//			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
//			LOG_TRACE( e.what() );
//			throw ( e );
//		}
//
//		yCommonDimIndex.Insert( dimIndex );
//	}
//
//
//	CStringArray complementDims;
//	bool complement = false;
//
//	if ( dimXNames.size() < dimYNames.size() )
//	{
//		complement = yfx->GetComplementVarDims( varXName, fieldName, complementDims );
//	}
//	else
//	{
//		complement = yfx->GetComplementVarDims( fieldName, varXName, complementDims );
//	}
//
//
//	CExternalFilesNetCDF* externalFile = dynamic_cast<CExternalFilesNetCDF*> ( BuildExistingExternalFileKind( yfx->GetName() ) );
//
//	if ( externalFile != nullptr )
//	{
//		externalFile->Open();
//	}
//
//	if ( ( complement ) && ( externalFile != nullptr ) )
//	{
//		// Read 'Complement dim' var data
//		CFieldNetCdf* field = externalFile->GetFieldNetCdf( complementDims.at( 0 ), false );
//		if ( field != nullptr )
//		{
//			m_fieldComplement = *field;
//			CUnit unit = yfx->GetUnit( field->GetName() );
//
//			yfx->ReadVar( field->GetName(), m_varComplement, unit.GetText() );
//		}
//	}
//
//
//	if ( externalFile != nullptr )
//	{
//		// Read other var data (non plot data)
//		//CStringArray::const_iterator itStringArray;
//		uint32_t iOtherVars = 0;
//
//		m_otherVars.RemoveAll();
//		//m_otherVars.resize(plot->m_nonPlotFieldNames.size());
//
//
//		for ( iOtherVars = 0; iOtherVars < plot->m_nonPlotFieldNames.size(); iOtherVars++ )
//		{
//
//			CFieldNetCdf* field = externalFile->GetFieldNetCdf( plot->m_nonPlotFieldNames.at( iOtherVars ), false );
//			if ( field != nullptr )
//			{
//				m_otherFields.Insert( new CFieldNetCdf( *field ) );
//
//				CUnit unit = yfx->GetUnit( field->GetName() );
//
//				CExpressionValue* exprValue = new CExpressionValue();
//
//				//yfx->ReadVar(field->GetName(), m_otherVars[iOtherVars], unit.GetText());
//				yfx->ReadVar( field->GetName(), *exprValue, unit.GetText() );
//
//				m_otherVars.Insert( exprValue );
//			}
//
//		}
//
//	}
//
//	//m_otherFields.Dump(*(CTrace::GetInstance()->GetDumpContext()));
//
//
//	if ( externalFile != nullptr )
//	{
//		externalFile->Close();
//		delete externalFile;
//		externalFile = nullptr;
//	}
//
//
//
//	SetData( varX.GetValues(), dimValX, varY.GetValues(), dimValY, xCommonDimIndex, yCommonDimIndex );
//
//	m_plotProperty.SetParent( this );
//	m_plotProperty.Update();
//}
//----------------------------------------
void VTK_CXYPlotData::SetRawData( CPlotArray* x, CPlotArray* y )
{

	if ( x->GetNumberOfFrames() != y->GetNumberOfFrames() )
	{
        std::string msg =
                "ERROR in VTK_CXYPlotData::SetRawData : Number of frames in PlotArray must be the same for all data - "
                "nb frames for x : "
                + n2s<std::string>(x->GetNumberOfFrames())
                + " - nb frames for y : "
                + n2s<std::string>(y->GetNumberOfFrames());

		CException e( msg, BRATHL_LOGIC_ERROR );
		throw ( e );

	}

	m_rawData.Insert( x );
	m_rawData.Insert( y );
	m_length = (int32_t)x->GetNumberOfFrames();

	Update();
}
//----------------------------------------
CPlotArray* VTK_CXYPlotData::GetRawData( int32_t i )
{
	CPlotArray* data = dynamic_cast<CPlotArray*>( m_rawData.at( i ) );
	if ( data == nullptr )
	{
		throw CException( "ERROR in  VTK_CXYPlotData::GetRawData : dynamic_cast<CPlotArray*>m_rawData.at(i);; returns nullptr pointer - "
			"XYPlotData  seems to contain objects other than those of the class CPlotArray or derived class", BRATHL_LOGIC_ERROR );
	}
#if ( defined (DEBUG) || defined(_DEBUG) ) && defined(DUMP_VALUES)

	vtkDoubleArrayBrathl* tmp_ptr = data->GetFrameData( 0 );
	vtkIdType capacity = tmp_ptr->Capacity();

	{
		ofstream myfile ("C:/example.txt");

		if (!myfile.is_open())
		{
			return NULL;
		}

		myfile << "these are y values:\n";

		for(int j=0;j<capacity;j++)
		{

			double s= tmp_ptr->GetComponent(j,0);
			s=s+s-s;
			myfile << "double number " << j << " has the value of: " << s << "\n";

		}
		//s = this->GetComponent( i, comp );
	}

#endif

	return data;
}

//----------------------------------------
void VTK_CXYPlotData::GetXRange(double& min, double& max, uint32_t frame)
{
  GetRawData(0)->GetRange(min, max, frame);
}
//----------------------------------------
void VTK_CXYPlotData::GetYRange(double& min, double& max, uint32_t frame)
{
  GetRawData(1)->GetRange(min, max, frame);
}
//----------------------------------------
/*
 * Gets the x-axis plot min and max range
*/
void VTK_CXYPlotData::GetXRange(double& min, double& max)
{
    //here we get the CPlotArray data relative to the
    //x axis and we calculate its general bounds
    //x_min and x_max.
  GetRawData(0)->GetRange(min, max);
}
//----------------------------------------
void VTK_CXYPlotData::GetYRange(double& min, double& max)
{
  GetRawData(1)->GetRange(min, max);
}

//----------------------------------------
int32_t VTK_CXYPlotData::GetNumberOfFrames()
{
  return (int32_t)GetRawData(0)->GetNumberOfFrames();
}
//----------------------------------------
void VTK_CXYPlotData::NormalizeX(int32_t value)
{
  GetRawData(0)->Normalize(value);
}
//----------------------------------------
void VTK_CXYPlotData::NormalizeY(int32_t value)
{
  GetRawData(1)->Normalize(value);
}
//----------------------------------------
void VTK_CXYPlotData::Normalize(int32_t value)
{
  NormalizeX(value);
  NormalizeY(value);
}

//----------------------------------------
void VTK_CXYPlotData::Update()
{
	m_vtkDataArrayPlotData->SetDataArrays( GetRawData( 0 )->GetFrameData( m_currentFrame ), GetRawData( 1 )->GetFrameData( m_currentFrame ) );
	//mQwtPlotData.SetDataArrays( GetRawData( 0 )->GetFrameData( m_currentFrame ), GetRawData( 1 )->GetFrameData( m_currentFrame ) );
}

//-------------------------------------------------------------
//------------------- CXYPlotDataMulti class --------------------
//-------------------------------------------------------------

//void VTK_CXYPlotData::SetData(
//	double* xArray, const CUIntArray& xDims,
//	double* yArray, const CUIntArray& yDims,
//	const CUIntArray& xCommonDimIndex,
//	const CUIntArray& yCommonDimIndex
//	)
void VTK_CXYPlotData::Create()
{
	assert__( m_rawData.size() == 0  );

	CPlotArray* xPlotArray = nullptr;
	CPlotArray* yPlotArray = nullptr;

	if ( m_rawData.size() == 0 )
	{
		xPlotArray = new CPlotArray();
		yPlotArray = new CPlotArray();
	}
	else
	{
		xPlotArray = GetRawData( 0 );
		yPlotArray = GetRawData( 1 );
	}


	const size_t maxFrames = mQwtPlotData.GetNumberOfFrames();

	for ( size_t i = 0; i < maxFrames; i++ )
	{

        auto framex = i + xPlotArray->GetNumberOfFrames();  UNUSED(framex);
        auto framey = i + xPlotArray->GetNumberOfFrames();  UNUSED(framey);

		assert__( framex == framey );


		size_t xDim, yDim;
		const double *xArray = nullptr, *yArray = nullptr;
		mQwtPlotData.GetFrameData( i, xArray, xDim, yArray, yDim );

		double *newX = (double*)malloc( xDim * sizeof( *newX ) );
		memcpy( newX, xArray, xDim * sizeof( *xArray ) );

		double *newY = (double*)malloc( yDim * sizeof( *newY ) );
		memcpy( newY, yArray, yDim * sizeof( *yArray ) );


		xPlotArray->SetFrameData( i + (int32_t)xPlotArray->GetNumberOfFrames(), newX, xDim );
		yPlotArray->SetFrameData( i + (int32_t)yPlotArray->GetNumberOfFrames(), newY, yDim );
	}

	SetRawData( xPlotArray, yPlotArray );

	m_plotProperty.Update();
}

////-------------------------------------------------------------
////------------------- CXYPlotDataCollection class --------------------
////-------------------------------------------------------------
//
////----------------------------------------
//VTK_CXYPlotData* CXYPlotDataCollection::Get(CObArray::iterator it )
//{
//  VTK_CXYPlotData* data = dynamic_cast<VTK_CXYPlotData*>(*it);
//  if (data == nullptr)
//  {
//    throw CException("ERROR in  CXYPlotDataCollection::Get : dynamic_cast<VTK_CXYPlotData*>(*it); returns nullptr pointer - "
//                 "XYPlotData Collection seems to contain objects other than those of the class VTK_CXYPlotData or derived class", BRATHL_LOGIC_ERROR);
//  }
//  return data;
//}
//
////----------------------------------------
//VTK_CXYPlotData* CXYPlotDataCollection::Get(int32_t index)
//{
//  if ( (index < 0) || (static_cast<uint32_t>(index) >= this->size()) )
//  {
//    std::string msg = CTools::Format("ERROR in  CXYPlotDataCollection::Get : index %d out-of-range "
//                                "Valid range is [0, %ld]",
//                                index,
//                                (long)this->size());
//    throw CException(msg, BRATHL_LOGIC_ERROR);
//
//  }
//
//  VTK_CXYPlotData* data = dynamic_cast<VTK_CXYPlotData*>(this->at(index));
//  if (data == nullptr)
//  {
//    throw CException("ERROR in  CXYPlotDataCollection::Get : dynamic_cast<VTK_CXYPlotData*>(this->at(index)); returns nullptr pointer - "
//                 "XYPlotData Collection seems to contain objects other than those of the class VTK_CXYPlotData or derived class", BRATHL_LOGIC_ERROR);
//  }
//  return data;
//}
////----------------------------------------
//CXYPlotProperties* CXYPlotDataCollection::GetPlotProperties(int32_t index)
//{
//  VTK_CXYPlotData* data = Get(index);
//
//  return data->GetPlotProperties();
//}
//
////----------------------------------------
//void CXYPlotDataCollection::OnFrameChange(int32_t f)
//{
//  CObArray::iterator it;
//
//  for (it = begin(); it != end() ; it++)
//  {
//    Get(it)->OnFrameChange(f);
//  }
//
//  m_currentFrame = f;
//
//
//}
//
////----------------------------------------
//void CXYPlotDataCollection::GetXRange(double& min, double& max, uint32_t frame)
//{
//  CObArray::iterator it;
//  setDefaultValue(min);
//  setDefaultValue(max);
//  for (it = begin(); it != end() ; it++)
//  {
//    VTK_CXYPlotData* data = Get(it);
//    if (data == nullptr)
//    {
//      continue;
//    }
//    double l = 0;
//    double h = 0;
//
//    CXYPlotProperties* props = data->GetPlotProperties();
//
//    data->GetXRange(l, h, frame);
//
//    if (isDefaultValue(props->GetXMin()) == false)
//    {
//      l = props->GetXMin();
//    }
//    if (isDefaultValue(props->GetXMax()) == false)
//    {
//      h = props->GetXMax();
//    }
//
//    if (isDefaultValue(min))
//    {
//      min = l;
//      max = h;
//    }
//    else
//    {
//      min = (min > l) ? l : min;
//      max = (max > h) ? max : h;
//    }
//
//  }
//
//}
////----------------------------------------
//void CXYPlotDataCollection::GetYRange(double& min, double& max, uint32_t frame)
//{
//  CObArray::iterator it;
//  setDefaultValue(min);
//  setDefaultValue(max);
//
//  for (it = begin(); it != end() ; it++)
//  {
//    VTK_CXYPlotData* data = Get(it);
//    double l = 0;
//    double h = 0;
//
//    CXYPlotProperties* props = data->GetPlotProperties();
//
//    data->GetYRange(l, h, frame);
//
//    if (isDefaultValue(props->GetYMin()) == false)
//    {
//      l = props->GetYMin();
//    }
//    if (isDefaultValue(props->GetYMax()) == false)
//    {
//      h = props->GetYMax();
//    }
//
//    if (isDefaultValue(min))
//    {
//      min = l;
//      max = h;
//    }
//    else
//    {
//      min = (min > l) ? l : min;
//      max = (max > h) ? max : h;
//    }
//
//  }
//}
////----------------------------------------
//void CXYPlotDataCollection::GetXRange(double& min, double& max)
//{
//  CObArray::iterator it;
//  setDefaultValue(min);
//  setDefaultValue(max);
//  for (it = begin(); it != end() ; it++)
//  {
//    VTK_CXYPlotData* data = Get(it);
//    double l = 0;
//    double h = 0;
//
//    CXYPlotProperties* props = data->GetPlotProperties();
//
//    data->GetXRange(l, h);
//
//    if (isDefaultValue(props->GetXMin()) == false)
//    {
//      l = props->GetXMin();
//    }
//    if (isDefaultValue(props->GetXMax()) == false)
//    {
//      h = props->GetXMax();
//    }
//
//    if (isDefaultValue(min))
//    {
//      min = l;
//      max = h;
//    }
//    else
//    {
//      min = (min > l) ? l : min;
//      max = (max > h) ? max : h;
//    }
//
//  }
//
//}
//
////----------------------------------------
//void CXYPlotDataCollection::GetYRange(double& min, double& max)
//{
//  CObArray::iterator it;
//  setDefaultValue(min);
//  setDefaultValue(max);
//
//  for (it = begin(); it != end() ; it++)
//  {
//    VTK_CXYPlotData* data = Get(it);
//    double l = 0;
//    double h = 0;
//
//    CXYPlotProperties* props = data->GetPlotProperties();
//
//    data->GetYRange(l, h);
//
//    if (isDefaultValue(props->GetYMin()) == false)
//    {
//      l = props->GetYMin();
//    }
//    if (isDefaultValue(props->GetYMax()) == false)
//    {
//      h = props->GetYMax();
//    }
//
//    if (isDefaultValue(min))
//    {
//      min = l;
//      max = h;
//    }
//    else
//    {
//      min = (min > l) ? l : min;
//      max = (max > h) ? max : h;
//    }
//
//  }
//}
////----------------------------------------
//bool CXYPlotDataCollection::ShowPropertyMenu()
//{
//  CObArray::iterator it;
//
//  for (it = begin(); it != end() ; it++)
//  {
//    VTK_CXYPlotData* data = Get(it);
//
//    CXYPlotProperties* props = data->GetPlotProperties();
//
//    if (props->GetShowPropertyPanel())
//    {
//      return true;
//    }
//  }
//
//  return false;
//
//}
//
////----------------------------------------
//void CXYPlotDataCollection::GetNames( std::vector<std::string>& names )
//{
//	for ( CObArray::iterator it = begin(); it != end(); it++ )
//	{
//		VTK_CXYPlotData* data = Get( it );
//		names.push_back( data->GetName() );
//	}
//}
