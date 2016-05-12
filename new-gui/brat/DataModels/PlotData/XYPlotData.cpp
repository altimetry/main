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

#include "new-gui/Common/tools/Trace.h"
#include "new-gui/Common/tools/Exception.h"
#include "libbrathl/ExternalFiles.h"
#include "libbrathl/ExternalFilesNetCDF.h"
#include "libbrathl/ExternalFilesFactory.h"
#include "libbrathl/Field.h"

using namespace brathl;

#if defined(BRAT_V3)
	#include "MapColor.h"
    #include "../support/code/legacy/display/PlotData/XYPlotData_vtk.h"
	#include "wxInterface.h"
	#include "wx/progdlg.h"
#endif


#include "ApplicationLogger.h"


#include "XYPlotData.h"



//-------------------------------------------------------------
//------------------- vtkDoubleArrayBrathl class --------------------
//-------------------------------------------------------------

//----------------------------------------------------------------------------
//vtkStandardNewMacro(vtkDoubleArrayBrathl);
//
////----------------------------------------------------------------------------
//void vtkDoubleArrayBrathl::PrintSelf(std::ostream& os, vtkIndent indent)
//{
//  this->vtkDoubleArray::PrintSelf(os,indent);
//}
////----------------------------------------------------------------------------
//
//void vtkDoubleArrayBrathl::ComputeRange( int comp )
//{
//	//---------------------------
//	vtkDoubleArray::ComputeRange( comp );
//	//---------------------------
//	double s, t;
//	vtkIdType numTuples;
//
//	if ( comp < 0 && this->NumberOfComponents == 1 )
//	{
//		comp = 0;
//	}
//
//	int idx = comp;
//	idx = ( idx<0 ) ? ( 4 ) : ( idx );
//
//	if ( ( this->GetMTime() > this->ComponentRangeComputeTime2[ idx ] ) )
//	{
//		numTuples=this->GetNumberOfTuples();
//		this->Range2[ 0 ] =  VTK_DOUBLE_MAX;
//		this->Range2[ 1 ] =  VTK_DOUBLE_MIN;
//
//		for ( vtkIdType i=0; i < numTuples; i++ )
//		{
//			if ( comp >= 0 )
//			{
//				s = this->GetComponent( i, comp );
//				if ( isDefaultValue( s ) )
//				{
//					//------------------
//					continue;
//					//------------------
//				}
//			}
//			else
//			{ // Compute range of std::vector magnitude.
//				s = 0.0;
//				for ( int j=0; j < this->NumberOfComponents; ++j )
//				{
//					t = this->GetComponent( i, j );
//					if ( isDefaultValue( t ) )
//					{
//						//------------------
//						continue;
//						//------------------
//					}
//					s += t*t;
//				}
//				s = sqrt( s );
//			}
//			if ( s < this->Range2[ 0 ] )
//			{
//				this->Range2[ 0 ] = s;
//			}
//			if ( s > this->Range2[ 1 ] )
//			{
//				this->Range2[ 1 ] = s;
//			}
//		}
//		this->ComponentRangeComputeTime2[ idx ].Modified();
//		this->ComponentRange2[ idx ][ 0 ] = this->Range2[ 0 ];
//		this->ComponentRange2[ idx ][ 1 ] = this->Range2[ 1 ];
//	}
//	else
//	{
//		this->Range2[ 0 ] = this->ComponentRange2[ idx ][ 0 ];
//		this->Range2[ 1 ] = this->ComponentRange2[ idx ][ 1 ];
//	}
//}
//
//
//
////-------------------------------------------------------------
////------------------- vtkDataArrayPlotData class
////-------------------------------------------------------------
//
////----------------------------------------------------------------------------
//vtkDataArrayPlotData::vtkDataArrayPlotData()
//{
//  this->XDataArray = nullptr;
//  this->YDataArray = nullptr;
//  this->ZDataArray = nullptr;
//  this->NumberOfItems = 0;
//}
//
////----------------------------------------------------------------------------
//vtkDataArrayPlotData::~vtkDataArrayPlotData()
//{
//  if (this->XDataArray != nullptr)
//    {
//    this->XDataArray->UnRegister(this);
//    }
//  if (this->YDataArray != nullptr)
//    {
//    this->YDataArray->UnRegister(this);
//    }
//  if (this->ZDataArray != nullptr)
//    {
//    this->ZDataArray->UnRegister(this);
//    }
//}
//
////----------------------------------------------------------------------------
//void vtkDataArrayPlotData::SetXDataArray( vtkDoubleArrayBrathl *dataArray )
//{
//	if ( this->XDataArray != dataArray )
//	{
//		if ( dataArray != nullptr )
//		{
//			dataArray->Register( this );
//		}
//		if ( this->XDataArray != nullptr )
//		{
//			this->XDataArray->UnRegister( this );
//		}
//		this->XDataArray = dataArray;
//		this->Modified();
//	}
//}
//
////----------------------------------------------------------------------------
//void vtkDataArrayPlotData::SetYDataArray(vtkDoubleArrayBrathl *dataArray)
//{
//  if (this->YDataArray != dataArray)
//    {
//    if (dataArray != nullptr)
//      {
//      dataArray->Register(this);
//      }
//    if (this->YDataArray != nullptr)
//      {
//      this->YDataArray->UnRegister(this);
//      }
//    this->YDataArray = dataArray;
//    this->Modified();
//    }
//}
//
////----------------------------------------------------------------------------
//void vtkDataArrayPlotData::SetZDataArray(vtkDoubleArrayBrathl *dataArray)
//{
//  if (this->ZDataArray != dataArray)
//    {
//    if (dataArray != nullptr)
//      {
//      dataArray->Register(this);
//      }
//    if (this->ZDataArray != nullptr)
//      {
//      this->ZDataArray->UnRegister(this);
//      }
//    this->ZDataArray = dataArray;
//    this->Modified();
//    }
//}
//
////----------------------------------------------------------------------------
//// NumberOfItems is set to the minimum number of tuples of all data arrays.
//// Only data arrays that are not null pointers are checked. If both the X, Y
//// and Z data arrays are null pointers then NumberOfItems is set to 0.
//void vtkDataArrayPlotData::Execute()
//{
//  vtkIdType numberOfTuples;
//
//  vtkDebugMacro(<<"Updating NumberOfItems");
//
//  this->NumberOfItems = -1;
//
//  if (this->XDataArray != nullptr)
//    {
//    numberOfTuples = this->XDataArray->GetNumberOfTuples();
//    if (this->NumberOfItems == -1)
//      {
//      this->NumberOfItems = numberOfTuples;
//      }
//    }
//  if (this->YDataArray != nullptr)
//    {
//    numberOfTuples = this->YDataArray->GetNumberOfTuples();
//    if (this->NumberOfItems == -1 || this->NumberOfItems > numberOfTuples)
//      {
//      this->NumberOfItems = numberOfTuples;
//      }
//    }
//  if (this->ZDataArray != nullptr)
//    {
//    numberOfTuples = this->ZDataArray->GetNumberOfTuples();
//    if (this->NumberOfItems == -1 || this->NumberOfItems > numberOfTuples)
//      {
//      this->NumberOfItems = numberOfTuples;
//      }
//    }
//  if (this->NumberOfItems == -1)
//    {
//    this->NumberOfItems = 0;
//    }
//
//  this->vtkPlotData::Execute();
//}
//
////----------------------------------------------------------------------------
//double vtkDataArrayPlotData::GetXValue(int i)
//{
//  if (this->XDataArray != nullptr)
//    {
//    return this->XDataArray->GetTuple1(i);
//    }
//  return 0.0;
//}
//
////----------------------------------------------------------------------------
//double vtkDataArrayPlotData::GetYValue(int i)
//{
//  if (this->YDataArray != nullptr)
//    {
//    return this->YDataArray->GetTuple1(i);
//    }
//  return 0.0;
//}
//
////----------------------------------------------------------------------------
//double vtkDataArrayPlotData::GetZValue(int i)
//{
//  if (this->ZDataArray != nullptr)
//    {
//    return this->ZDataArray->GetTuple1(i);
//    }
//  return 0.0;
//}
//
////----------------------------------------------------------------------------
//int vtkDataArrayPlotData::GetNumberOfItems()
//{
//  return this->NumberOfItems;
//}
//
////----------------------------------------------------------------------------
//unsigned long vtkDataArrayPlotData::GetMTime()
//{
//  unsigned long mTime = this->vtkPlotData::GetMTime();
//  unsigned long time;
//
//  vtkDebugMacro(<<"Returning modification time");
//
//  if (this->XDataArray != nullptr)
//    {
//    time = this->XDataArray->GetMTime();
//    mTime = time > mTime ? time : mTime;
//    }
//  if (this->YDataArray != nullptr)
//    {
//    time = this->YDataArray->GetMTime();
//    mTime = time > mTime ? time : mTime;
//    }
//  if (this->ZDataArray != nullptr)
//    {
//    time = this->ZDataArray->GetMTime();
//    mTime = time > mTime ? time : mTime;
//    }
//
//  return mTime;
//}
//
//
//
//
////-------------------------------------------------------------
////------------------- CPlotArray class --------------------
////-------------------------------------------------------------
//CPlotArray::CPlotArray()
//{
//  m_didCalculateRange = false;
//  setDefaultValue(m_min);
//  setDefaultValue(m_max);
//}
////----------------------------------------
//CPlotArray::~CPlotArray()
//{
//}
////----------------------------------------
//vtkDoubleArrayBrathl* CPlotArray::GetFrameData(size_t r)
//{
//  if (r >= m_rows.size())
//  {
//    r = m_rows.size() - 1;
//  }
//
//  vtkDoubleArrayBrathl* data = dynamic_cast<vtkDoubleArrayBrathl*>(m_rows.at(r));
//  if (data == nullptr)
//  {
//    std::string msg =
//            "ERROR in CPlotArray::GetFrameData: dynamic_cast<vtkDoubleArrayBrathl*>(m_rows.at("
//            + n2s<std::string>( r )
//            + ") returns nullptr pointer - "
//            "Plot Array (rows) seems to contain objects other than those of the class vtkDoubleArrayBrathl class";
//    CException e(msg, BRATHL_LOGIC_ERROR);
//    throw (e);
//  }
//
//  return data;
//
//}
////----------------------------------------
//vtkDoubleArrayBrathl* CPlotArray::GetFrameData(vtkObArray::iterator it)
//{
//  vtkDoubleArrayBrathl* data = dynamic_cast<vtkDoubleArrayBrathl*>(*it);
//  if (data == nullptr)
//  {
//    throw CException("ERROR in  GetFrameData(CObArray::iterator it) : dynamic_cast<vtkDoubleArrayBrathl*>(*it); returns nullptr pointer - "
//                 "CPlot Array seems to contain objects other than those of the class vtkDoubleArrayBrathl or derived class", BRATHL_LOGIC_ERROR);
//  }
//  return data;
//
//}
////----------------------------------------
//void CPlotArray::SetFrameData( uint32_t r, double* vect, int32_t size )
//{
//	const bool bCopy = 
//
//#if defined(BRAT_V3)
//		false;
//#else
//		true;
//#endif
//
//	double* newVect = nullptr;
//	if ( bCopy )
//	{
//		newVect = new double[ size ];
//		memcpy( newVect, vect, size * sizeof( *vect ) );
//	}
//	else
//	{
//		newVect = vect;
//	}
//	vtkDoubleArrayBrathl* vtkArray = vtkDoubleArrayBrathl::New();
//	// This method lets the user specify data to be held by the array.
//	// The array argument is a pointer to the data.
//	// size is the size of the array supplied by the user.
//	// Set save to 1 to keep the class from deleting the array when it cleans up or reallocates memory.
//	// The class uses the actual array provided; it does not copy the data from the suppled array
//	vtkArray->SetArray( newVect, size, 0 );
//
//	SetFrameData( r, vtkArray );
//}
////----------------------------------------
//void CPlotArray::SetFrameData( uint32_t r, vtkDoubleArrayBrathl* vect )
//{
//	//if (r == m_rows.size())
//	if ( r >= m_rows.size() )
//	{
//		m_rows.Insert( vect );
//	}
//	else if ( r < m_rows.size() )
//	{
//		m_rows.ReplaceAt( m_rows.begin() + r, vect );
//	}
//	else
//	{
//		std::string msg = CTools::Format( "ERROR in  CPlotArray::SetFrameData : index %d out-of-range : [0, %ld]",
//			r, (long)m_rows.size() );
//		CException e( msg, BRATHL_LOGIC_ERROR );
//		throw ( e );
//
//	}
//	m_didCalculateRange = false;
//}
////----------------------------------------
//void CPlotArray::GetRange(double& min, double& max, uint32_t frame)
//{
//
//  setDefaultValue(min);
//  setDefaultValue(max);
//
//  vtkDoubleArrayBrathl* data = GetFrameData(frame);
//  if (data == nullptr)
//  {
//    return;
//  }
//
//  min = data->GetRange2()[0];
//  max = data->GetRange2()[1];
//
//}
////----------------------------------------
//void CPlotArray::GetRange(double& min, double& max)
//{
//  if (m_didCalculateRange)
//  {
//    min = m_min;
//    max = m_max;
//    return;
//  }
//
//  vtkObArray::iterator it;
//  setDefaultValue(min);
//  setDefaultValue(max);
//
//  for (it = m_rows.begin(); it != m_rows.end() ; it++)
//  {
//    vtkDoubleArrayBrathl* data = GetFrameData(it);
//    double l = data->GetRange2()[0];
//    double h = data->GetRange2()[1];
//
//    //data->GetRange(l, h);
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
//  }
//
//  m_min = min;
//  m_max = max;
//
//  m_didCalculateRange = true;
//}
////----------------------------------------
//void CPlotArray::Normalize(int32_t value)
//{
//  double l = 0;
//  double h = 0;
//  GetRange(l, h);
//
//  int i =0;
//
//  vtkObArray::iterator it;
//
//  for (it = m_rows.begin(); it != m_rows.end() ; it++)
//  {
//    vtkDoubleArrayBrathl* data = GetFrameData(it);
//    for (i = 0 ; i < data->GetNumberOfTuples(); i++)
//    {
//      data->SetValue(i, ((data->GetValue(i) - l) / (h - l)) * value);
//    }
//  }
//  m_didCalculateRange = false;
//}

//-------------------------------------------------------------
//------------------- CXYPlotProperties class --------------------
//-------------------------------------------------------------


CXYPlotProperties::CXYPlotProperties( CXYPlotData* parent )
{
	m_focus = false;

	SetParent( parent );

#if defined(BRAT_V3)
	m_vtkProperty2D = vtkProperty2D::New();

	SetLineStipple( displayFULL );
	SetOpacityFactor( 1 / GetOpacity() );
#endif

	SetLineWidthFactor( 1.5 );

	SetLineWidth( 0.8 );

	SetLines( true );
	SetPoints( false );

	SetPointSize( 1.0 );

	SetOpacity( 0.6 );

	setDefaultValue( m_xMax );
	setDefaultValue( m_xMin );
	setDefaultValue( m_yMax );
	setDefaultValue( m_yMin );


	SetXLog( false );
	SetYLog( false );

	SetShowAnimationToolbar( false );
	SetShowPropertyPanel( true );

	SetLoop( false );

	SetNormalizeX( 0 );
	SetNormalizeY( 0 );

	SetFps( 30 );

	setDefaultValue( m_xNumTicks );
	setDefaultValue( m_yNumTicks );

	SetXBase( 10.0 );
	SetYBase( 10.0 );

    if (parent)
    {
        parent->GetXRange(m_xMin, m_xMax);
    }

	CPlotColor color = CMapColor::GetInstance().NextPrimaryColors();
	if ( color.Ok() )
	{
		SetColor( color );
	}
	else
	{
		SetColor( CPlotColor( 0.0, 0.0, 0.0 ) );
	}

	SetPointGlyph( displayCIRCLE_GLYPH );
	SetFilledPoint( true );

	SetLineWidthFactor( 1.5 );

	SetHide( false );

}

//----------------------------------------
CXYPlotProperties::CXYPlotProperties(CXYPlotProperties& p)
{
  m_focus = false;
  this->Copy(p);
}

//----------------------------------------
const CXYPlotProperties& CXYPlotProperties::operator=( const CXYPlotProperties& p )
{
  this->Copy(p);
  return *this;
}

//----------------------------------------
CXYPlotProperties::~CXYPlotProperties()
{
#if defined(BRAT_V3)
	if ( m_vtkProperty2D != nullptr )
	{
		m_vtkProperty2D->Delete();
		m_vtkProperty2D = nullptr;
	}
#endif
}
//----------------------------------------
void CXYPlotProperties::Copy( const CXYPlotProperties& p )
{
	SetParent( p.GetParent() );

#if defined(BRAT_V3)
	m_vtkProperty2D = vtkProperty2D::New();

	SetLineStipple( p.GetLineStipple() );
	SetLineWidth( p.GetLineWidth() );
	SetOpacity( p.GetOpacity() );

#endif

	SetLineWidthFactor( p.GetLineWidthFactor() );
	m_focus = p.GetFocus();

	SetLines( p.GetLines() );
	SetPoints( p.GetPoints() );

	SetPointSize( p.GetPointSize() );

	SetXMax( p.GetXMax() );
	SetYMax( p.GetYMax() );

	SetXMin( p.GetXMin() );
	SetYMin( p.GetYMin() );

	SetXLog( p.GetXLog() );
	SetYLog( p.GetYLog() );

	SetShowAnimationToolbar( p.GetShowAnimationToolbar() );
	SetShowPropertyPanel( p.GetShowPropertyPanel() );

	SetLoop( p.GetLoop() );

	SetNormalizeX( p.GetNormalizeX() );
	SetNormalizeY( p.GetNormalizeY() );

	SetFps( p.GetFps() );

	SetXNumTicks( p.GetXNumTicks() );
	SetYNumTicks( p.GetYNumTicks() );

	SetXBase( p.GetXBase() );
	SetYBase( p.GetYBase() );

	SetColor( p.GetColor() );

	SetTitle( p.GetTitle() );
	SetName( p.GetName() );
	SetXAxis( p.GetXAxis() );
	SetXLabel( p.GetXLabel() );
	SetYLabel( p.GetYLabel() );

	SetPointGlyph( p.GetPointGlyph() );
	SetFilledPoint( p.GetFilledPoint() );

	SetOpacityFactor( p.GetOpacityFactor() );

	SetHide( p.GetHide() );

	mColor = p.mColor;
	mPointColor = p.mPointColor;
	mOpacity = p.mOpacity;
	mLineWidth = p.mLineWidth ;

#if !defined(BRAT_V3)

	mStipplePattern = p.mStipplePattern;
#endif

}
//----------------------------------------
void CXYPlotProperties::Update()
{
  SetLines();

  SetPoints();
  SetPointSize();

  SetPointGlyph();
  SetFilledPoint();

  SetNormalizeX();
  SetNormalizeY();

}

#if defined(BRAT_V3)
VTK_CXYPlotData* CXYPlotProperties::parent()
{
	return dynamic_cast<VTK_CXYPlotData*>( m_parent );
}
#endif

//----------------------------------------
bool CXYPlotProperties::HasParent()
{
  return m_parent != nullptr;
}

//----------------------------------------
void CXYPlotProperties::SetHide(bool value)
{
  m_hide = value;

  SetHide();

}
//----------------------------------------
void CXYPlotProperties::SetHide()
{
  if (!HasParent())
  {
    return;
  }

  if (m_hide)
  {
#if defined(BRAT_V3)
    parent()->GetVtkDataArrayPlotData()->PlotLinesOff();
    parent()->GetVtkDataArrayPlotData()->PlotPointsOff();
#endif
  }
  else
  {
    SetLines();
    SetPoints();
  }

}

//----------------------------------------
void CXYPlotProperties::SetLineWidth( double value )
{
	mLineWidth = value < 1.0 ? value*m_lineWidthFactor : value;		//v3 compatibility

#if defined(BRAT_V3)
	if ( value <= 0.0 )
	{
		value = 0.8;
	}
	m_vtkProperty2D->SetLineWidth( value );
#endif
}
//----------------------------------------
void CXYPlotProperties::SetLines()
{

  if (!HasParent())
  {
    return;
  }

#if defined(BRAT_V3)
  if (m_lines)
  {
    parent()->GetVtkDataArrayPlotData()->PlotLinesOn();
  }
  else
  {
    parent()->GetVtkDataArrayPlotData()->PlotLinesOff();
  }
#endif

}
//----------------------------------------
void CXYPlotProperties::SetLines(bool value)
{
  m_lines = value;

  SetLines();

}
//----------------------------------------
void CXYPlotProperties::SetPoints()
{

  if (!HasParent())
  {
    return;
  }

#if defined(BRAT_V3)
  if (m_points)
  {
    parent()->GetVtkDataArrayPlotData()->PlotPointsOn();
  }
  else
  {
    parent()->GetVtkDataArrayPlotData()->PlotPointsOff();
  }
#endif
  SetFilledPoint();

}

//----------------------------------------
void CXYPlotProperties::SetPoints( bool value )
{
	m_points = value;

	SetPoints();
}

//----------------------------------------
void CXYPlotProperties::SetPointSize()
{
	if ( !HasParent() )
	{
		return;
	}

#if defined(BRAT_V3)
	parent()->GetVtkDataArrayPlotData()->SetGlyphSize( m_pointSize / 100.0 );
#endif

}

//----------------------------------------
void CXYPlotProperties::SetPointSize(double value)
{
  if (value <= 0.0)
  {
    value = 1.0;
  }
  m_pointSize = value;

  SetPointSize();
}

//----------------------------------------
void CXYPlotProperties::SetPointGlyph()
{
	if ( !HasParent() )
	{
		return;
	}
	if ( m_points == false )
	{
		return;
	}

#if defined(BRAT_V3)
	parent()->GetVtkDataArrayPlotData()->GetGlyphSource()->SetGlyphType( m_glyphType );
#endif
}
//----------------------------------------
void CXYPlotProperties::SetPointGlyph( EPointGlyph value )
{
	m_glyphType = value;

	SetPointGlyph();
}
//----------------------------------------
void CXYPlotProperties::SetFilledPoint()
{
	if ( !HasParent() )
	{
		return;
	}
	if ( m_points == false )
	{
		return;
	}

#if defined(BRAT_V3)
	if ( m_filledPoint )
	{
		parent()->GetVtkDataArrayPlotData()->GetGlyphSource()->FilledOn();
	}
	else
	{
		parent()->GetVtkDataArrayPlotData()->GetGlyphSource()->FilledOff();
	}
#endif
}
//----------------------------------------
void CXYPlotProperties::SetFilledPoint( bool value )
{
	m_filledPoint = value;

	SetFilledPoint();
}

//----------------------------------------
void CXYPlotProperties::SetOpacity( double value )
{
	if ( value > 1.0 )
	{
		value = 1.0;
	}

	if ( value <= 0.0 )
	{
		value = 0.6;
	}

	m_opacityFactor = 1 / value;

#if defined(BRAT_V3)
	m_vtkProperty2D->SetOpacity( value );
#endif
}

//----------------------------------------
void CXYPlotProperties::SetName( const std::string& value )
{
	m_name = value;

	if ( !HasParent() )
	{
		return;
	}
#if defined(BRAT_V3)
	parent()->GetVtkDataArrayPlotData()->SetPlotLabel( value.c_str() );
#endif
}
//----------------------------------------
void CXYPlotProperties::SetXLog(bool value)
{
  m_xLog = value;

  if ((m_xMin <= 0) && (value))
  {
    std::string msg = CTools::Format("ERROR in CXYPlotProperties::SetXLog : X range [%g, %g] contains value <= 0 - no logarithmic axis possible",
                                 m_xMin, m_xMax);
    CException e(msg, BRATHL_LOGIC_ERROR);
    throw (e);
  }
}
//----------------------------------------
void CXYPlotProperties::SetYLog(bool value)
{
  m_yLog = value;

  if ((m_yMin <= 0) && (value))
  {
    std::string msg = CTools::Format("ERROR in CXYPlotProperties::SetYLog : Y range [%g, %g] contains value <= 0 - no logarithmic axis possible",
                                 m_yMin, m_yMax);
    CException e(msg, BRATHL_LOGIC_ERROR);
    throw (e);
  }
}

//----------------------------------------
void CXYPlotProperties::SetFocus( bool value )
{

	if ( ( m_focus == false ) && ( value == true ) )
	{
		m_focus = value;

#if defined(BRAT_V3)
		SetLineWidth( GetLineWidth() * m_lineWidthFactor );
#endif
	}
	else if ( ( m_focus == true ) && ( value == false ) )
	{
		m_focus = value;

#if defined(BRAT_V3)
		SetLineWidth( GetLineWidth() / m_lineWidthFactor );
#endif
	}
}

//----------------------------------------
void CXYPlotProperties::SetNormalizeX()
{
	if ( !HasParent() )
	{
		return;
	}
	if ( m_normalizeX <= 0 )
	{
		return;
	}
	m_parent->NormalizeX( m_normalizeX );
}


//----------------------------------------
void CXYPlotProperties::SetNormalizeX(int32_t value)
{
  m_normalizeX = value;

  SetNormalizeX();
}


//----------------------------------------
void CXYPlotProperties::SetNormalizeY()
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
void CXYPlotProperties::SetNormalizeY(int32_t value)
{
  m_normalizeY = value;

  SetNormalizeY();
}

//----------------------------------------
CPlotColor CXYPlotProperties::GetColor() const
{
#if defined(BRAT_V3)

	CPlotColor c( m_vtkProperty2D->GetColor()[ 0 ],
		m_vtkProperty2D->GetColor()[ 1 ],
		m_vtkProperty2D->GetColor()[ 2 ] );

	assert__( c == mColor );

	return c;

#else

	return mColor;
#endif

}
//----------------------------------------
void CXYPlotProperties::GetColor( CPlotColor& color ) const
{
	color = GetColor();
}
//----------------------------------------
void CXYPlotProperties::SetColor( const CPlotColor& color )
{
	mColor = color;

#if defined(BRAT_V3)
	m_vtkProperty2D->SetColor( color.Red(),
		color.Green(),
		color.Blue() );
#endif

}

//-------------------------------------------------------------
//------------------- CXYPlotData class --------------------
//-------------------------------------------------------------

CXYPlotData::CXYPlotData( CPlot* plot, int32_t iField )
{
	m_plotProperty.SetParent( this );

	m_currentFrame = 0;
	m_length = 0;

	m_aborted = false;

	CPlotField* field = plot->GetPlotField( iField );

	if ( field->m_xyProps != nullptr )
	{
		m_plotProperty = *field->m_xyProps;

	}

	CInternalFiles* yfx = field->GetInternalFiles( 0 );

	Create( yfx, plot, iField );
}

//----------------------------------------
CXYPlotData::~CXYPlotData()
{
	m_otherVars.RemoveAll();
}
//----------------------------------------
void CXYPlotData::Create( CInternalFiles* yfx, CPlot* plot, int32_t iField )
{
	std::string varXName;

	CUnit unitXRead;
	CUnit unitYRead;

	std::string unitXStr;
	std::string unitYStr;

	if ( m_plotProperty.GetTitle().empty() )
	{
		m_plotProperty.SetTitle( plot->m_title );
	}

	if ( m_plotProperty.GetXLabel().empty() )
	{
		m_plotProperty.SetXLabel( plot->m_titleX );
	}
	else
	{
		std::string titleX = m_plotProperty.GetXLabel();
		titleX += plot->m_unitXLabel;
		m_plotProperty.SetXLabel( titleX );
	}

	if ( m_plotProperty.GetYLabel().empty() )
	{
		m_plotProperty.SetYLabel( plot->m_titleY );
	}
	else
	{
		std::string titleY = m_plotProperty.GetYLabel();
		titleY += plot->m_unitYLabel;
		m_plotProperty.SetYLabel( titleY );
	}

	m_unitX = plot->m_unitX;
	m_unitY = plot->m_unitY;

	CExpressionValue varX;

	ExpressionValueDimensions dimValX;

	NetCDFVarKind varKind;
	CStringArray varDimNames;


	// Get X axis data
	plot->GetAxisX( yfx, &dimValX, &varX, &varXName );

	// Get and control unit of X axis
	// X units are compatible but not the same --> convert
	unitXRead = yfx->GetUnit( varXName );
	if ( m_unitX.AsString() != unitXRead.AsString() )
	{
		plot->m_unitX.SetConversionFrom( unitXRead );
		plot->m_unitX.ConvertVector( varX.GetValues(), (int32_t)varX.GetNbValues() );
	}

	CExpressionValue varY;
	std::string fieldName = plot->GetPlotField( iField )->m_name.c_str();

	// Set name of the data

	if ( GetName().empty() )
	{
		std::string str = yfx->GetTitle( fieldName ) + " (" + fieldName + ")";
		SetName( str.c_str() );
	}

	// Read Y data dimension
	ExpressionValueDimensions dimValY;

	yfx->GetVarDims( fieldName, dimValY );
	if ( ( dimValY.size() <= 0 ) || ( dimValY.size() > 2 ) )
	{
		std::string msg = CTools::Format( "CXYPlotData::Create - '%s' axis -> number of dimensions  must be 1 or 2 - Found : %ld",
			fieldName.c_str(), (long)dimValY.size() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		LOG_TRACE( e.what() );
		throw ( e );
	}

	// Read Y data
	unitYRead = yfx->GetUnit( fieldName );
	yfx->ReadVar( fieldName, varY, unitYRead.GetText() );
	varKind	= yfx->GetVarKind( fieldName );
	/************************DEDEDE
	if (varKind != Data)
	{
	std::string msg = CTools::Format("CXYPlotData::Create - variable '%s' is not a kind of Data (%d) : %s",
	fieldName.c_str(), Data, CNetCDFFiles::VarKindToString(varKind).c_str());
	CException e(msg, BRATHL_INCONSISTENCY_ERROR);
	CTrace::Tracer("%s", e.what());
	throw (e);
	}
	*/
	// Y units are compatible but not the same --> convert
	if ( m_unitY.AsString() != unitYRead.AsString() )
	{
		plot->m_unitY.SetConversionFrom( unitYRead );
		plot->m_unitY.ConvertVector( varY.GetValues(), (int32_t)varY.GetNbValues() );
	}

	CStringArray commonDims;
	bool intersect = yfx->GetCommonVarDims( varXName, fieldName, commonDims );

	if ( !intersect )
	{
		std::string msg = CTools::Format( "CXYPlotData::Create - variables '%s' and '%s' have no common dimension",
			fieldName.c_str(), varXName.c_str() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		LOG_TRACE( e.what() );
		throw ( e );

	}

	CStringArray dimXNames;
	CStringArray dimYNames;
	yfx->GetVarDims( varXName, dimXNames );
	yfx->GetVarDims( fieldName, dimYNames );

	if ( ( commonDims.size() == dimXNames.size() ) && ( commonDims.size() == dimYNames.size() ) && ( dimXNames != dimYNames ) )
	{
		std::string msg = CTools::Format( "CXYPlotData::Create - variables '%s' (dim %s) and '%s' (dim %s) must have the same dimension fields",
			fieldName.c_str(), dimYNames.ToString().c_str(), varXName.c_str(), dimXNames.ToString().c_str() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		LOG_TRACE( e.what() );
		throw ( e );

	}


	CUIntArray xCommonDimIndex;
	CUIntArray yCommonDimIndex;

	for ( uint32_t iIntersect = 0; iIntersect < commonDims.size(); iIntersect++ )
	{
		int32_t dimIndex = -1;

		dimIndex = yfx->GetVarDimIndex( varXName, commonDims.at( iIntersect ) );
		if ( dimIndex < 0 )
		{
			std::string msg = CTools::Format( "CXYPlotData::Create - variables '%s' - dim '%s' not found ",
				varXName.c_str(), commonDims.at( iIntersect ).c_str() );
			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
			LOG_TRACE( e.what() );
			throw ( e );
		}

		xCommonDimIndex.Insert( dimIndex );

		dimIndex = yfx->GetVarDimIndex( fieldName, commonDims.at( iIntersect ) );
		if ( dimIndex < 0 )
		{
			std::string msg = CTools::Format( "CXYPlotData::Create - variables '%s' - dim '%s' not found ",
				fieldName.c_str(), commonDims.at( iIntersect ).c_str() );
			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
			LOG_TRACE( e.what() );
			throw ( e );
		}

		yCommonDimIndex.Insert( dimIndex );
	}


	CStringArray complementDims;
	bool complement = false;

	if ( dimXNames.size() < dimYNames.size() )
	{
		complement = yfx->GetComplementVarDims( varXName, fieldName, complementDims );
	}
	else
	{
		complement = yfx->GetComplementVarDims( fieldName, varXName, complementDims );
	}


	CExternalFilesNetCDF* externalFile = dynamic_cast<CExternalFilesNetCDF*> ( BuildExistingExternalFileKind( yfx->GetName() ) );

	if ( externalFile != nullptr )
	{
		externalFile->Open();
	}

	if ( ( complement ) && ( externalFile != nullptr ) )
	{
		// Read 'Complement dim' var data
		CFieldNetCdf* field = externalFile->GetFieldNetCdf( complementDims.at( 0 ), false );
		if ( field != nullptr )
		{
			m_fieldComplement = *field;
			CUnit unit = yfx->GetUnit( field->GetName() );

			yfx->ReadVar( field->GetName(), m_varComplement, unit.GetText() );
		}
	}


	if ( externalFile != nullptr )
	{
		// Read other var data (non plot data)
		//CStringArray::const_iterator itStringArray;
		uint32_t iOtherVars = 0;

		m_otherVars.RemoveAll();
		//m_otherVars.resize(plot->m_nonPlotFieldNames.size());


		for ( iOtherVars = 0; iOtherVars < plot->m_nonPlotFieldNames.size(); iOtherVars++ )
		{

			CFieldNetCdf* field = externalFile->GetFieldNetCdf( plot->m_nonPlotFieldNames.at( iOtherVars ), false );
			if ( field != nullptr )
			{
				m_otherFields.Insert( new CFieldNetCdf( *field ) );

				CUnit unit = yfx->GetUnit( field->GetName() );

				CExpressionValue* exprValue = new CExpressionValue();

				//yfx->ReadVar(field->GetName(), m_otherVars[iOtherVars], unit.GetText());
				yfx->ReadVar( field->GetName(), *exprValue, unit.GetText() );

				m_otherVars.Insert( exprValue );
			}

		}

	}

	//m_otherFields.Dump(*(CTrace::GetInstance()->GetDumpContext()));


	if ( externalFile != nullptr )
	{
		externalFile->Close();
		delete externalFile;
		externalFile = nullptr;
	}



	SetData( varX.GetValues(), dimValX, varY.GetValues(), dimValY, xCommonDimIndex, yCommonDimIndex );

	m_plotProperty.SetParent( this );
}


//-------------------------------------------------------------
//------------------- ex-CXYPlotDataMulti class --------------------
//-------------------------------------------------------------

void CXYPlotData::SetData( 
	double* xArray, const CUIntArray& xDims, 
	double* yArray, const CUIntArray& yDims,
	const CUIntArray& xCommonDimIndex,
	const CUIntArray& yCommonDimIndex
	)
{
	if ( GetName().empty() )
	{
		SetName( "Dataset" );
	}

	CUIntArray xDimIndex;
	CUIntArray yDimIndex;

	uint32_t iDimIndex = 0;

	for ( iDimIndex = 0; iDimIndex < xDims.size(); iDimIndex++ )
	{
		xDimIndex.Insert( iDimIndex );
	}

	for ( iDimIndex = 0; iDimIndex < yDims.size(); iDimIndex++ )
	{
		yDimIndex.Insert( iDimIndex );
	}

	double* newX = nullptr;
	double* newY = nullptr;

	if ( ( xDims.size() <= 0 ) || ( yDims.size() <= 0 ) )
	{
		std::string msg = CTools::Format( "ERROR in CXYPlotDataMulti::SetData : At least one of Arrays is 0 dimensional  -"
			"xArray dim. is %ld and yArray dim. is %ld",
			(long)xDims.size(), (long)yDims.size() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		throw ( e );

	}

	if ( ( xCommonDimIndex.size() <= 0 ) || ( yCommonDimIndex.size() <= 0 ) )
	{
		std::string msg = CTools::Format( "ERROR in CXYPlotDataMulti::SetData : No common dimension have been set."
			"xArray dim. is %ld and yArray dim. is %ld - "
			"xCommonDimIndex dim. is %ld and yCommonDimIndex dim. is %ld",
			(long)xDims.size(), (long)yDims.size(),
			(long)xCommonDimIndex.size(), (long)yCommonDimIndex.size() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		throw ( e );

	}

	if ( ( xDims.size() > 2 ) || ( yDims.size() > 2 ) )
	{
		std::string msg = CTools::Format( "ERROR in CXYPlotDataMulti::SetData : Arrays must be 2 dimensional or less -"
			"xArray dim. is %ld and yArray dim. is %ld",
			(long)xDims.size(), (long)yDims.size() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		throw ( e );

	}

	if ( ( xCommonDimIndex.size() > 2 ) || ( yCommonDimIndex.size() > 2 ) )
	{
		std::string msg = CTools::Format( "ERROR in CXYPlotDataMulti::SetData : Common dimension arrays must be 2 dimensional or less -"
			"xCommonDimIndex dim. is %ld and yCommonDimIndex dim. is %ld",
			(long)xCommonDimIndex.size(), (long)yCommonDimIndex.size() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		throw ( e );


	}

	if ( xCommonDimIndex.size() != yCommonDimIndex.size() )
	{
		std::string msg = CTools::Format( "ERROR in CXYPlotDataMulti::SetData : Common dimension arrays have not the same dimension -"
			"xCommonDimIndex dim. is %ld and yCommonDimIndex dim. is %ld",
			(long)xCommonDimIndex.size(), (long)yCommonDimIndex.size() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		throw ( e );

	}

	int32_t maxFrames = 0;
	bool hasComplement = false;
	CUIntArray complementX;
	CUIntArray complementY;

	if ( xCommonDimIndex.size() == 1 )
	{
		if ( ( xDims.size() == 1 ) && ( yDims.size() == 1 ) )
		{
			maxFrames = 1;
		}
		else if ( xDims.size() == 1 )
		{
			hasComplement = yCommonDimIndex.Complement( yDimIndex, complementX );
			if ( !hasComplement )
			{
				std::string msg = CTools::Format( "ERROR in CXYPlotDataMulti::SetData : No X complement have been found:"
					"xCommonDimIndex dim. is %ld, values are %s and yCommonDimIndex dim. is %ld, values are %s -"
					"xDimIndex dim. is %ld, values are %s and yDimIndex dim. is %ld, values are %s",
					(long)xCommonDimIndex.size(), xCommonDimIndex.ToString().c_str(),
					(long)yCommonDimIndex.size(), xCommonDimIndex.ToString().c_str(),
					(long)xDimIndex.size(), xDimIndex.ToString().c_str(),
					(long)yDimIndex.size(), yDimIndex.ToString().c_str()
					);
				CException e( msg, BRATHL_INCONSISTENCY_ERROR );
				throw ( e );
			}
			maxFrames = yDims.at( complementX.at( 0 ) );
		}
		else if ( yDims.size() == 1 )
		{
			hasComplement = xCommonDimIndex.Complement( xDimIndex, complementY );
			if ( !hasComplement )
			{
				std::string msg = CTools::Format( "ERROR in CXYPlotDataMulti::SetData : No Y complement have been found:"
					"xCommonDimIndex dim. is %ld, values are %s and yCommonDimIndex dim. is %ld, values are %s"
					"xDimIndex dim. is %ld, values are %s and yDimIndex dim. is %ld, values are %s",
					(long)xCommonDimIndex.size(), xCommonDimIndex.ToString().c_str(),
					(long)yCommonDimIndex.size(), xCommonDimIndex.ToString().c_str(),
					(long)xDimIndex.size(), xDimIndex.ToString().c_str(),
					(long)yDimIndex.size(), yDimIndex.ToString().c_str()
					);
				CException e( msg, BRATHL_INCONSISTENCY_ERROR );
				throw ( e );
			}
			maxFrames = xDims.at( complementY.at( 0 ) );
		}
		else
		{
			std::string msg = CTools::Format( "ERROR in CXYPlotDataMulti::SetData : X and Y dimension must be equals:"
				"xCommonDimIndex dim. is %ld, values are %s and yCommonDimIndex dim. is %ld, values are %s"
				"xDimIndex dim. is %ld, values are %s and yDimIndex dim. is %ld, values are %s",
				(long)xCommonDimIndex.size(), xCommonDimIndex.ToString().c_str(),
				(long)yCommonDimIndex.size(), xCommonDimIndex.ToString().c_str(),
				(long)xDimIndex.size(), xDimIndex.ToString().c_str(),
				(long)yDimIndex.size(), yDimIndex.ToString().c_str()
				);
			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
			throw ( e );
		}
	}
	else
	{
		hasComplement = yCommonDimIndex.Complement( yDimIndex, complementX );
		hasComplement |= xCommonDimIndex.Complement( xDimIndex, complementY );
		if ( hasComplement )
		{
			std::string msg = CTools::Format( "ERROR in CXYPlotDataMulti::SetData : X and Y must have the same dimension fields:"
				"xCommonDimIndex dim. is %ld, values are %s and yCommonDimIndex dim. is %ld, values are %s"
				"xDimIndex dim. is %ld, values are %s and yDimIndex dim. is %ld, values are %s",
				(long)xCommonDimIndex.size(), xCommonDimIndex.ToString().c_str(),
				(long)yCommonDimIndex.size(), xCommonDimIndex.ToString().c_str(),
				(long)xDimIndex.size(), xDimIndex.ToString().c_str(),
				(long)yDimIndex.size(), yDimIndex.ToString().c_str()
				);
			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
			throw ( e );
		}
		else if ( xCommonDimIndex != yCommonDimIndex )
		{
			std::string msg = CTools::Format( "ERROR in CXYPlotDataMulti::SetData : X and Y dimension must be equals:"
				"xCommonDimIndex dim. is %ld, values are %s and yCommonDimIndex dim. is %ld, values are %s"
				"xDimIndex dim. is %ld, values are %s and yDimIndex dim. is %ld, values are %s",
				(long)xCommonDimIndex.size(), xCommonDimIndex.ToString().c_str(),
				(long)yCommonDimIndex.size(), xCommonDimIndex.ToString().c_str(),
				(long)xDimIndex.size(), xDimIndex.ToString().c_str(),
				(long)yDimIndex.size(), yDimIndex.ToString().c_str()
				);
			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
			throw ( e );
		}

		maxFrames = 1;

	}

#if defined BRAT_V3		// TODO: some callback device to display progress

	wxProgressDialog* pd = new wxProgressDialog("Calculating Frames...",
		"",
		maxFrames, nullptr,
		wxPD_CAN_ABORT | wxPD_REMAINING_TIME | wxPD_APP_MODAL);
	pd->SetSize(-1,-1, 350, -1);

#endif

	int32_t i = 0;

	for ( i = 0; i < maxFrames; i++ )
	{
		int32_t xDim = -1;
		int32_t yDim = -1;

#if defined BRAT_V3		// TODO: some callback device to display progress
		std::string msg = CTools::Format( "Calculating Frame: %d of %d", i + 1, maxFrames );
		if (pd->Update(i, msg.c_str()) == false)
		{
			m_aborted = true;
			break;
		}
#endif

		if ( xDims.size() == 1 )
		{
			xDim = xDims[ 0 ];
			newX = new double[xDim];
			memcpy( newX, xArray, xDim * sizeof( *xArray ) );
		}
		else
		{
			if ( xCommonDimIndex.size() == 1 )
			{
				xDim = xDims[ xCommonDimIndex.at( 0 ) ];
				newX = new double[xDim];
				if ( xCommonDimIndex.at( 0 ) == 1 )
				{
					memcpy( newX, xArray + ( i * xDim ), xDim * sizeof( *xArray ) );
				}
				else
				{
					for ( int32_t j = 0; j < xDim; j++ )
					{
						newX[ j ] = xArray[ i + ( j * maxFrames ) ];
					}
				}
			}
			else
			{
				xDim = xDims[ xCommonDimIndex.at( 0 ) ] * xDims[ xCommonDimIndex.at( 1 ) ];
				newX = new double[xDim];
				memcpy( newX, xArray + ( i * xDim ), xDim * sizeof( *xArray ) );
			}

		}

		if ( yDims.size() == 1 )
		{
			yDim = yDims[ 0 ];
			newY = new double[yDim];
			memcpy( newY, yArray, yDim * sizeof( *yArray ) );
		}
		else
		{
			if ( yCommonDimIndex.size() == 1 )
			{
				yDim = yDims[ yCommonDimIndex.at( 0 ) ];
				newY = new double[yDim];
				if ( yCommonDimIndex.at( 0 ) == 1 )
				{
					memcpy( newY, yArray + ( i * yDim ), yDim * sizeof( *yArray ) );
				}
				else
				{
					for ( int32_t j = 0; j < yDim; j++ )
					{
						newY[ j ] = yArray[ i + ( j * maxFrames ) ];
					}
				}
			}
			else
			{
				yDim = yDims[ yCommonDimIndex.at( 0 ) ] * yDims[ yCommonDimIndex.at( 1 ) ];
				newY = new double[yDim];
				memcpy( newY, yArray + ( i * yDim ), yDim * sizeof( *yArray ) );
			}
		}

		mQwtPlotData.AddFrameData( newX, xDim, newY, yDim );
	}

#if defined BRAT_V3		// TODO: some callback device to display progress

	pd->Destroy();
#endif

	if ( m_aborted )
	{
		return;
	}
}

//----------------------------------------
void CXYPlotData::GetXRange( double& min, double& max, uint32_t frame )
{
	//TODO check if this is what we want

	mQwtPlotData.GetXRange( min, max, frame );

	////////////////////////////////////GetRawData( 0 )->GetRange( min, max, frame );
}
//----------------------------------------
void CXYPlotData::GetYRange( double& min, double& max, uint32_t frame )
{
    mQwtPlotData.GetYRange( min, max, frame );

	//////////////////////////////////GetRawData( 1 )->GetRange( min, max, frame );
}
//----------------------------------------
void CXYPlotData::GetXRange( double& min, double& max )
{
	mQwtPlotData.GetXRange( min, max );

	////////////////////////////////GetRawData( 0 )->GetRange( min, max );
}
//----------------------------------------
void CXYPlotData::GetYRange( double& min, double& max )
{
	mQwtPlotData.GetYRange( min, max );

    ///////////////////////////////////////////GetRawData( 1 )->GetRange( min, max );
}

//----------------------------------------
int32_t CXYPlotData::GetNumberOfFrames()
{
	return mQwtPlotData.GetNumberOfFrames();

	/////////////////////return (int32_t)GetRawData( 0 )->GetNumberOfFrames();
}
//----------------------------------------
void CXYPlotData::NormalizeX( int32_t value )
{
	//TODO

    UNUSED( value );

	/////////////////////////////////GetRawData( 0 )->Normalize( value );
}
//----------------------------------------
void CXYPlotData::NormalizeY( int32_t value )
{
	//TODO

    UNUSED( value );

    //////////////////////////////////////GetRawData( 1 )->Normalize( value );
}
//----------------------------------------
void CXYPlotData::Normalize( int32_t value )
{
	NormalizeX( value );
	NormalizeY( value );
}


#if defined(BRAT_V3)

void CXYPlotData::OnFrameChange(int32_t f)
{

  m_currentFrame = (f < m_length - 1) ? f : m_length - 1;
  m_currentFrame = (m_currentFrame > 0) ? m_currentFrame : 0;

  Update();

}

void CXYPlotData::Update()
{
    //m_vtkDataArrayPlotData->SetDataArrays( GetRawData( 0 )->GetFrameData( m_currentFrame ), GetRawData( 1 )->GetFrameData( m_currentFrame ) );
}

#endif


//-------------------------------------------------------------
//------------------- CXYPlotDataCollection class --------------------
//-------------------------------------------------------------

//----------------------------------------
#if defined(BRAT_V3)
using xy_plot_data_t = VTK_CXYPlotData;
#else
using xy_plot_data_t = CXYPlotData;
#endif

xy_plot_data_t* CXYPlotDataCollection::Get(CObArray::iterator it )
{
  xy_plot_data_t* data = dynamic_cast<xy_plot_data_t*>(*it);
  if (data == nullptr)
  {
    throw CException("ERROR in  CXYPlotDataCollection::Get : dynamic_cast<CXYPlotData*>(*it); returns nullptr pointer - "
                 "XYPlotData Collection seems to contain objects other than those of the class CXYPlotData or derived class", BRATHL_LOGIC_ERROR);
  }
  return data;
}

//----------------------------------------
xy_plot_data_t* CXYPlotDataCollection::Get(int32_t index)
{
  if ( (index < 0) || (static_cast<uint32_t>(index) >= this->size()) )
  {
    std::string msg = CTools::Format("ERROR in  CXYPlotDataCollection::Get : index %d out-of-range "
                                "Valid range is [0, %ld]",
                                index,
                                (long)this->size());
    throw CException(msg, BRATHL_LOGIC_ERROR);

  }

  xy_plot_data_t* data = dynamic_cast<xy_plot_data_t*>(this->at(index));
  if (data == nullptr)
  {
    throw CException("ERROR in  CXYPlotDataCollection::Get : dynamic_cast<CXYPlotData*>(this->at(index)); returns nullptr pointer - "
                 "XYPlotData Collection seems to contain objects other than those of the class CXYPlotData or derived class", BRATHL_LOGIC_ERROR);
  }
  return data;
}
//----------------------------------------
CXYPlotProperties* CXYPlotDataCollection::GetPlotProperties(int32_t index)
{
  CXYPlotData* data = Get(index);

  return data->GetPlotProperties();
}


void CXYPlotDataCollection::AddData( CBratObject* ob )
{
	base_t::Insert( ob );

	CXYPlotData *pdata = dynamic_cast<CXYPlotData*>( ob );		assert__( pdata );
	auto *properties = pdata->GetPlotProperties();				assert__( properties );

	if ( isDefaultValue( properties->GetXMin() ) || isDefaultValue( properties->GetXMax() ) )
	{
		double xrMin, xrMax;

		GetXRange( xrMin, xrMax );
		properties->SetXMin( xrMin );
		properties->SetXMax( xrMax );
	}

	if ( isDefaultValue( properties->GetYMin() ) || isDefaultValue( properties->GetYMax() ) )
	{
		double yrMin, yrMax;
		GetYRange( yrMin, yrMax );
		properties->SetYMin( yrMin );
		properties->SetYMax( yrMax );
	}
}



#if defined(BRAT_V3)

void CXYPlotDataCollection::OnFrameChange(int32_t f)
{
  CObArray::iterator it;

  for (it = begin(); it != end() ; it++)
  {
    Get(it)->OnFrameChange(f);
  }

  m_currentFrame = f;
}

#endif

//----------------------------------------
void CXYPlotDataCollection::GetXRange(double& min, double& max, uint32_t frame)
{
  CObArray::iterator it;
  setDefaultValue(min);
  setDefaultValue(max);
  for (it = begin(); it != end() ; it++)
  {
    CXYPlotData* data = Get(it);
    if (data == nullptr)
    {
      continue;
    }
    double l = 0;
    double h = 0;

    CXYPlotProperties* props = data->GetPlotProperties();

    data->GetXRange(l, h, frame);

    if (isDefaultValue(props->GetXMin()) == false)
    {
      l = props->GetXMin();
    }
    if (isDefaultValue(props->GetXMax()) == false)
    {
      h = props->GetXMax();
    }

    if (isDefaultValue(min))
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
  setDefaultValue(min);
  setDefaultValue(max);

  for (it = begin(); it != end() ; it++)
  {
    CXYPlotData* data = Get(it);
    double l = 0;
    double h = 0;

    CXYPlotProperties* props = data->GetPlotProperties();

    data->GetYRange(l, h, frame);

    if (isDefaultValue(props->GetYMin()) == false)
    {
      l = props->GetYMin();
    }
    if (isDefaultValue(props->GetYMax()) == false)
    {
      h = props->GetYMax();
    }

    if (isDefaultValue(min))
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
/*
 * This function is calculating the general minimum along each frame.
 * however if CXYPlotProperties props already contains values for
 * m_xMin and m_xMax, it uses those instead.
 */
void CXYPlotDataCollection::GetXRange(double& min, double& max)
{
  CObArray::iterator it;
  setDefaultValue(min);
  setDefaultValue(max);
  for (it = begin(); it != end() ; it++)
  {
    CXYPlotData* data = Get(it);
    double l = 0;
    double h = 0;

    CXYPlotProperties* props = data->GetPlotProperties();

    //computes the X range (lower and upper)
    //notice that the function receives a pointer to
    // l and h
    data->GetXRange(l, h);

    //XYPlotProperties contains two CPlotArrays
    //m_xMin does not have an initial value yet
    if (isDefaultValue(props->GetXMin()) == false)
    {
      //only set l if xMin already contains an initial value
        //that is non default
      l = props->GetXMin();
    }
    //same for m_xMax
    if (isDefaultValue(props->GetXMax()) == false)
    {
      h = props->GetXMax();
    }

    //min has not been initialized
    if (isDefaultValue(min))
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
  setDefaultValue(min);
  setDefaultValue(max);

  for (it = begin(); it != end() ; it++)
  {
    CXYPlotData* data = Get(it);
    double l = 0;
    double h = 0;

    CXYPlotProperties* props = data->GetPlotProperties();

    data->GetYRange(l, h);

    if (isDefaultValue(props->GetYMin()) == false)
    {
      l = props->GetYMin();
    }
    if (isDefaultValue(props->GetYMax()) == false)
    {
      h = props->GetYMax();
    }

    if (isDefaultValue(min))
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

    CXYPlotProperties* props = data->GetPlotProperties();

    if (props->GetShowPropertyPanel())
    {
      return true;
    }
  }

  return false;

}

//----------------------------------------
void CXYPlotDataCollection::GetNames( std::vector<std::string>& names )
{
	for ( CObArray::iterator it = begin(); it != end(); it++ )
	{
		CXYPlotData* data = Get( it );
		names.push_back( data->GetName() );
	}
}
