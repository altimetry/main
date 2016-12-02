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

#include "new-gui/Common/QtUtilsIO.h"
#include "common/tools/Trace.h"
#include "common/tools/Exception.h"
#include "libbrathl/Tools.h"
using namespace brathl;

#include "BratLookupTableSettings.h"
#include "BratLookupTable.h"



#if defined(BRAT_V3)

//static 
double const CBratLookupTable::smDefaultRangeValues = 0;

#else

static const QColor invalid_color = QColor( 255, 255, 255, 0 );


//static 
double const CBratLookupTable::smDefaultRangeValues = defaultValue< double >();





QLookupTable::QLookupTable() 
	: qwt_base_t()
	, qwt3d_base_t()
{
	mTableRange[0] = CBratLookupTable::smDefaultRangeValues;
	mTableRange[1] = CBratLookupTable::smDefaultRangeValues;
}



void QLookupTable::SetNumberOfTableValues(vtkIdType number)
{
	mTable.resize( number );
}

size_t QLookupTable::GetNumberOfTableValues() const
{ 
	return mTable.size(); 
}

//virtual 
void QLookupTable::SetTableRange( double m, double M )
{
	mTableRange[0] = m;
	mTableRange[1] = M;
}

const QColor& QLookupTable::GetTableValue( size_t indx) const
{
	assert__( (vtkIdType)indx >= 0 && indx < GetNumberOfTableValues() );

	return mTable[ indx ];
}

void QLookupTable::SetTableValue( size_t indx, double r, double g, double b, double a )		//a=1.0
{
	assert__( (vtkIdType)indx >= 0 && indx < GetNumberOfTableValues() );

	//double rgba[ 4 ];
	//rgba[ 0 ] = r; rgba[ 1 ] = g; rgba[ 2 ] = b; rgba[ 3 ] = a;

	QColor &c = mTable[ indx ];
	c.setAlpha( a * 255.0f + 0.5f );
	c.setRed( r * 255.0f + 0.5f );
	c.setGreen( g * 255.0f + 0.5f );
	c.setBlue( b * 255.0f + 0.5f );
}


size_t QLookupTable::GetIndex( double v ) const
{
	double maxIndex = mTable.size() - 1;
	double shift, scale;

	//if ( this->Scale == VTK_SCALE_LOG10 )
	//{   // handle logarithmic scale
	//	double logRange[ 2 ];
	//	vtkLookupTableLogRange( mTableRange, logRange );
	//	shift = -logRange[ 0 ];
	//	if ( logRange[ 1 ] <= logRange[ 0 ] )
	//	{
	//		scale = VTK_DOUBLE_MAX;
	//	}
	//	else
	//	{
	//		/* while this looks like the wrong scale, it is the correct scale
	//		 * taking into account the truncation to int that happens below. */
	//		scale = ( maxIndex + 1 ) / ( logRange[ 1 ] - logRange[ 0 ] );
	//	}
	//	v = vtkApplyLogScale( v, mTableRange, logRange );
	//}
	//else
	{   // plain old linear
		shift = - mTableRange[ 0 ];
		if ( mTableRange[ 1 ] <= mTableRange[ 0 ] )
		{
			scale = 1.0e+299;
		}
		else
		{
			/* while this looks like the wrong scale, it is the correct scale
			 * taking into account the truncation to int that happens below. */
			scale = ( maxIndex + 1 ) / ( mTableRange[ 1 ] - mTableRange[ 0 ] );
		}
	}

	// map to an index
	double findx = ( v + shift )*scale;
	if ( findx < 0 )
	{
		findx = 0;
	}
	if ( findx > maxIndex )
	{
		findx = maxIndex;
	}
	return static_cast<int>( findx );
}


////////////////
// QWT interface
////////////////

QLookupTable::QLookupTable( const QLookupTable&o ) 
	: qwt_base_t( o )
	, qwt3d_base_t( o )
	, mTable( o.mTable )
{
	mTableRange[0] = o.mTableRange[0] ;
	mTableRange[1] = o.mTableRange[1];
}


//virtual 
QwtColorMap *QLookupTable::copy() const
{
	return new QLookupTable( *this );
}

//Map a value of a given interval into a rgb value.
// param interval Range for the values
// param value Value
// return rgb value, corresponding to value
//
//virtual 
QRgb QLookupTable::rgb( const QwtDoubleInterval &interval, double value ) const
{
    Q_UNUSED( interval );

	if ( std::isnan( value ) )
		return invalid_color.rgba();

	return mTable[ GetIndex( value ) ].rgba();
}

//Map a value of a given interval into a color index
// param interval Range for the values
// param value Value
// return color index, corresponding to value
//
//virtual 
unsigned char QLookupTable::colorIndex( const QwtDoubleInterval &interval, double value ) const
{
    Q_UNUSED( interval );

    return (unsigned char) GetIndex( value );
}

//QColor color(const QwtDoubleInterval &, double value) const;

//virtual 
QVector<QRgb> QLookupTable::colorTable( const QwtDoubleInterval &interval ) const
{
	return qwt_base_t::colorTable( interval );
}



//////////////////
// QWT3D interface
//////////////////


//virtual 
Qwt3D::RGBA QLookupTable::operator()( double x, double y, double z ) const
{
	Q_UNUSED( x );		Q_UNUSED( y );

	auto const &c = std::isnan( z ) ? invalid_color : mTable[ GetIndex( z ) ];
	return Qwt3D::RGBA( c.red() / 255.0, c.green() / 255.0, c.blue() / 255.0, c.alpha() / 255.0 );
}


//virtual 
Qwt3D::ColorVector& QLookupTable::createVector( Qwt3D::ColorVector& vec )
{
	vec.clear();
	const size_t size = mTable.size();
	for ( size_t i = 0; i < size; i++ )
	{
		auto const &c = mTable[ i ];
		vec.push_back( Qwt3D::RGBA( c.red() / 255.0, c.green() / 255.0, c.blue() / 255.0, c.alpha() / 255.0 ) );
	}
	return vec;
}


#endif		// BRAT_V3



//-------------------------------------------------------------
//------------------- CBratLookupTable class --------------------
//-------------------------------------------------------------

CBratLookupTable::CBratLookupTable()
	: m_nameToMethod( true )
{
	Init();
}

//----------------------------------------
CBratLookupTable::CBratLookupTable( const CBratLookupTable& lut )
{
	Init();
	m_cust.RemoveAll();
	DupCustMap( *( lut.GetCust() ) );

	m_grad.RemoveAll();
	DupGradMap( *( lut.GetGrad() ) );

	//m_std = lut.GetStd();
	m_currentFunction = lut.GetCurrentFunction();

	m_vtkLookupTable->SetNumberOfTableValues( lut.GetLookupTable()->GetNumberOfTableValues() );
	m_vtkLookupTable->SetTableRange( lut.GetLookupTable()->GetTableRange() );

	ExecCurveMethod( lut.GetCurve() );

	Update();
}

//----------------------------------------

CBratLookupTable::~CBratLookupTable()
{
  if (m_vtkLookupTable != nullptr)
  {
    m_vtkLookupTable->Delete();
    m_vtkLookupTable = nullptr;
  }
}

//----------------------------------------
CBratLookupTable& CBratLookupTable::operator = ( const CBratLookupTable &lut )
{
	m_cust.RemoveAll();
	DupCustMap( *( lut.GetCust() ) );

	m_grad.RemoveAll();
	DupGradMap( *( lut.GetGrad() ) );

	m_currentFunction = lut.GetCurrentFunction();

	m_vtkLookupTable->SetNumberOfTableValues( lut.GetLookupTable()->GetNumberOfTableValues() );
	m_vtkLookupTable->SetTableRange( lut.GetLookupTable()->GetTableRange() );

	ExecCurveMethod( lut.GetCurve() );

	Update();	// == ExecMethod( m_currentFunction );

	return *this;
}

// There is a lot of guessing here; drop any equality empirically not working
//
bool CBratLookupTable::operator == ( const CBratLookupTable &o ) const
{
	return
		m_currentFunction == o.m_currentFunction &&
		GetLookupTable()->GetNumberOfTableValues() == o.GetLookupTable()->GetNumberOfTableValues() &&
		GetLookupTable()->GetTableRange()[ 0 ] == o.GetLookupTable()->GetTableRange()[ 0 ] &&
		GetLookupTable()->GetTableRange()[ 1 ] == o.GetLookupTable()->GetTableRange()[ 1 ] &&
		m_curve == o.m_curve &&
		m_gradientFunction == o.m_gradientFunction &&
		m_customFunction == o.m_customFunction &&
		m_fileName == o.m_fileName &&

		true;
}


//----------------------------------------
void CBratLookupTable::Init()
{
	//mUpFlank = nullptr;
	//mDownFlank = nullptr;

	m_vtkLookupTable = QLookupTable::New();

	m_vtkLookupTable->SetNumberOfTableValues( 256 );

	//m_resetFunction = "Reset";

	m_currentFunction = m_resetFunction;

	m_curveNames.Insert( CURVE_LINEAR() );
	m_curveNames.Insert( CURVE_SQRT() );
	m_curveNames.Insert( CURVE_COSINUS() );

	//mDefaultColorTable = PALETTE_AEROSOL.c_str();

	m_nameToMethod.Insert( PALETTE_BLACKTOWHITE,	static_cast<CBratObject *>( new CallbackVoid( this, &CBratLookupTable::BlackToWhite ) ) );
	m_nameToMethod.Insert( PALETTE_WHITETOBLACK,	static_cast<CBratObject *>( new CallbackVoid( this, &CBratLookupTable::WhiteToBlack ) ) );
	m_nameToMethod.Insert( PALETTE_REDTOGREEN,		static_cast<CBratObject *>( new CallbackVoid( this, &CBratLookupTable::RedToGreen ) ) );
	m_nameToMethod.Insert( PALETTE_GREENTORED,		static_cast<CBratObject *>( new CallbackVoid( this, &CBratLookupTable::GreenToRed ) ) );
	m_nameToMethod.Insert( PALETTE_CLOUD,			static_cast<CBratObject *>( new CallbackVoid( this, &CBratLookupTable::Cloud ) ) );
	m_nameToMethod.Insert( PALETTE_RAINBOW,			static_cast<CBratObject *>( new CallbackVoid( this, &CBratLookupTable::Rainbow ) ) );
	m_nameToMethod.Insert( PALETTE_RAINBOW2,		static_cast<CBratObject *>( new CallbackVoid( this, &CBratLookupTable::Rainbow2 ) ) );
	m_nameToMethod.Insert( mDefaultColorTable,		static_cast<CBratObject *>( new CallbackVoid( this, &CBratLookupTable::Aerosol ) ) );
	m_nameToMethod.Insert( PALETTE_AEROSOL2,		static_cast<CBratObject *>( new CallbackVoid( this, &CBratLookupTable::Aerosol2 ) ) );
	m_nameToMethod.Insert( PALETTE_OZONE,			static_cast<CBratObject *>( new CallbackVoid( this, &CBratLookupTable::Ozone ) ) );
	m_nameToMethod.Insert( PALETTE_BLACKBODY,		static_cast<CBratObject *>( new CallbackVoid( this, &CBratLookupTable::Blackbody ) ) );

	for ( CObMap::iterator it = m_nameToMethod.begin(); it != m_nameToMethod.end(); it++ )
	{
		m_colorTableList.push_back( ( it->first ).c_str() );
	}

	m_gradientFunction = "Gradient";
	m_customFunction = "Custom";
	m_nameToMethod.Insert( m_resetFunction,		static_cast<CBratObject *>( new CallbackVoid( this, &CBratLookupTable::Reset ) ) );
	m_nameToMethod.Insert( m_gradientFunction,	static_cast<CBratObject *>( new CallbackVoid( this, &CBratLookupTable::Gradient ) ) );
	m_nameToMethod.Insert( m_customFunction,	static_cast<CBratObject *>( new CallbackVoid( this, &CBratLookupTable::Custom ) ) );

	m_nameToMethod.Insert( "SetCurveLinear",	static_cast<CBratObject *>( new CallbackVoid( this, &CBratLookupTable::SetCurveLinear ) ) );
	m_nameToMethod.Insert( "SetCurveSQRT",		static_cast<CBratObject *>( new CallbackVoid( this, &CBratLookupTable::SetCurveSQRT ) ) );
	m_nameToMethod.Insert( "SetCurveCosinus",	static_cast<CBratObject *>( new CallbackVoid( this, &CBratLookupTable::SetCurveCosinus ) ) );

	m_nameToMethod.Insert( "LineUp",			static_cast<CBratObject *>( new CallbackFlank( this, &CBratLookupTable::LineUp ) ) );
	m_nameToMethod.Insert( "LineDown",			static_cast<CBratObject *>( new CallbackFlank( this, &CBratLookupTable::LineDown ) ) );
	m_nameToMethod.Insert( "SqrtUp",			static_cast<CBratObject *>( new CallbackFlank( this, &CBratLookupTable::SqrtUp ) ) );
	m_nameToMethod.Insert( "SqrtDown",			static_cast<CBratObject *>( new CallbackFlank( this, &CBratLookupTable::SqrtDown ) ) );
	m_nameToMethod.Insert( "CosUp",				static_cast<CBratObject *>( new CallbackFlank( this, &CBratLookupTable::CosUp ) ) );
	m_nameToMethod.Insert( "CosDown",			static_cast<CBratObject *>( new CallbackFlank( this, &CBratLookupTable::CosDown ) ) );

	SetCurveLinear();
	//ExecMethodDefaultColorTable();
}
//----------------------------------------
std::string CBratLookupTable::MethodToLabeledMethod( const std::string& method ) const
{
	std::string methodTmp = CTools::StringToLower( method );

	for ( CObMap::const_iterator it = m_nameToMethod.begin(); it != m_nameToMethod.end(); it++ )
	{
		std::string lowerMethod = CTools::StringToLower( it->first );
		if ( methodTmp == lowerMethod )
		{
			return it->first;
		}
	}

	return "";
}
//----------------------------------------
std::string CBratLookupTable::CurveToLabeledCurve( const std::string& curve ) const
{
	std::string curveTmp = CTools::StringToLower( curve );

	for ( CStringArray::const_iterator it = m_curveNames.begin(); it != m_curveNames.end(); it++ )
	{
		std::string lowerCurve = CTools::StringToLower( *it );
		if ( curveTmp == lowerCurve )
		{
			return *it;
		}
	}

	return "";
}

//----------------------------------------
bool CBratLookupTable::IsValidCurve(const std::string& curve)
{
  CStringArray::iterator it;
  for (it = m_curveNames.begin() ; it != m_curveNames.end() ; it++)
  {
    if (curve == *it)
    {
      return true;
    }
  }

  return false;

}

//----------------------------------------
bool CBratLookupTable::IsValidMethod(const std::string& methodName)
{
  CallbackVoid* method = static_cast< CallbackVoid* >(m_nameToMethod[methodName]);
  if (method == nullptr)
  {
    return false;
  }

  return true;

}

//----------------------------------------
void CBratLookupTable::ExecMethod( const std::string& methodName )
{
	CallbackVoid* method = static_cast<CallbackVoid*>( m_nameToMethod[ methodName ] );
	if ( method == nullptr )
	{
		std::string msg = CTools::Format( "ERROR in CBratLookupTable::ExecMethod : unknown methodname or not a CallbackVoid :'%s'",
			methodName.c_str() );
		CException e( msg, BRATHL_LOGIC_ERROR );
		CTrace::Tracer( "%s", e.what() );
		Dump( *CTrace::GetDumpContext() );
		throw ( e );

	}

	try
	{
		method->execute();
	}
	catch ( CException e )
	{
		std::string msg = CTools::Format( "BRAT ERROR in CBratLookupTable::ExecMethod : std::exception while executing methodname :'%s'"
			"\nNative Error : %s",
			methodName.c_str(),
			e.what() );
		CException e2( msg, BRATHL_LOGIC_ERROR );
		CTrace::Tracer( "%s", e2.what() );
		Dump( *CTrace::GetDumpContext() );
		throw ( e2 );
	}
	catch ( std::exception e )
	{
		std::string msg = CTools::Format( "BRAT RUNTIME ERROR in CBratLookupTable::ExecMethod : std::exception while executing methodname :'%s'"
			"\nNative Error : %s",
			methodName.c_str(),
			e.what() );
		CException e2( msg, BRATHL_LOGIC_ERROR );
		CTrace::Tracer( "%s", e2.what() );
		Dump( *CTrace::GetDumpContext() );
		throw ( e2 );
	}
	catch ( ... )
	{
		std::string msg = CTools::Format( "BRAT FATAL ERROR in CBratLookupTable::ExecMethod : std::exception while executing methodname :'%s'",
			methodName.c_str() );
		CException e2( msg, BRATHL_LOGIC_ERROR );
		CTrace::Tracer( "%s", e2.what() );
		Dump( *CTrace::GetDumpContext() );
		throw ( e2 );
	}
}

//----------------------------------------
void CBratLookupTable::ExecMethodDefaultColorTable()
{
  ExecMethod(mDefaultColorTable);
}
//----------------------------------------
void CBratLookupTable::ExecCurveMethod(const std::string& curve)
{
  std::string methodName = "SetCurve" + curve;

  ExecMethod(methodName);
}


//----------------------------------------
void CBratLookupTable::SetCurveUpDown(CallbackFlank* up, CallbackFlank* down)
{
  mUpFlank = up;
  mDownFlank = down;
}

//----------------------------------------
double CBratLookupTable::LineUp(double a, double b, double x)
{
  if ((x - 1) <= a)
  {
    return 0.0;
  }
  else if ((x + 1) >= b)
  {
    return 1.0;
  }
  else
  {
    return (x - a) / (b - a);
  }
}

//----------------------------------------
double CBratLookupTable::LineDown(double a, double b, double x)
{
  if ((x - 1) <= a)
  {
    return 0.0;
  }
  else if ((x + 1) >= b)
  {
    return -1.0;
  }
  else
  {
    return -1 * ( x - a) / (b - a);
  }
}

//----------------------------------------
double CBratLookupTable::SqrtUp(double a, double b, double x)
{
  if ((x - 1) <= a)
  {
    return 0.0;
  }
  else if ((x + 1) >= b)
  {
    return 1.0;
  }
  else
  {
    return CTools::Sqrt( (x - a) / (b - a) );
  }
}

//----------------------------------------
double CBratLookupTable::SqrtDown(double a, double b, double x)
{
  if ((x - 1) <= a)
  {
    return 0.0;
  }
  else if ((x + 1) >= b)
  {
    return -1.0;
  }
  else
  {
    return (CTools::Sqrt( 1 - (x - a) / (b - a) ) - 1);
  }
}

//----------------------------------------
double CBratLookupTable::CosUp(double a, double b, double x)
{

  if ((x - 1) <= a)
  {
    return 0.0;
  }
  else if ((x + 1) >= b)
  {
    return 1.0;
  }
  else
  {
    return 1 - ((CTools::Cos(((x - a) / (b - a)) * M_PI ) + 1) / 2);
  }
}

//----------------------------------------
double CBratLookupTable::CosDown(double a, double b, double x)
{
  if ((x - 1) <= a)
  {
    return 0.0;
  }
  else if ((x + 1) >= b)
  {
    return -1.0;
  }
  else
  {
    return -1 + ((CTools::Cos(((x - a) / (b - a)) * M_PI) + 1) / 2);
  }
}

//----------------------------------------
void CBratLookupTable::BlackToWhite()
{
  m_cust.RemoveAll();
  m_grad.RemoveAll();

  m_currentFunction = PALETTE_BLACKTOWHITE;

  double u = static_cast<double>(m_vtkLookupTable->GetNumberOfTableValues());

  for (size_t i = 0 ; i <  m_vtkLookupTable->GetNumberOfTableValues() ; i++)
  {
    double r, g, b;
    double di = static_cast<double>(i);
    r = g = b = mUpFlank->execute(0, u, di);
    m_vtkLookupTable->SetTableValue(i, r, g, b, 1.0);
  }

}

void CBratLookupTable::Black()
{
  m_cust.RemoveAll();
  m_grad.RemoveAll();

  m_currentFunction = PALETTE_BLACKTOWHITE;

  double u = static_cast<double>(m_vtkLookupTable->GetNumberOfTableValues());

  for (size_t i = 0 ; i <  m_vtkLookupTable->GetNumberOfTableValues() ; i++)
  {
    double r, g, b;
    double di = static_cast<double>(i);
    r = g = b = mUpFlank->execute(0, u, di);
    m_vtkLookupTable->SetTableValue(i, 0.0, 0.0, 0.0, 1.0);
  }

}

//----------------------------------------
void CBratLookupTable::WhiteToBlack()
{
  m_cust.RemoveAll();
  m_grad.RemoveAll();

  m_currentFunction = PALETTE_WHITETOBLACK;

  double u = static_cast<double>(m_vtkLookupTable->GetNumberOfTableValues());

  for (size_t i = 0 ; i <  m_vtkLookupTable->GetNumberOfTableValues() ; i++)
  {
    double r, g, b;
    double di = static_cast<double>(i);
    r = g = b = 1 + mDownFlank->execute(0, u, di);
    m_vtkLookupTable->SetTableValue(i, r, g, b, 1.0);
  }

}

//----------------------------------------
void CBratLookupTable::GreenToRed()
{
  m_cust.RemoveAll();
  m_grad.RemoveAll();
  m_currentFunction = PALETTE_GREENTORED;

  double u = static_cast<double>(m_vtkLookupTable->GetNumberOfTableValues());

  for (size_t i = 0 ; i <  m_vtkLookupTable->GetNumberOfTableValues() ; i++)
  {
    double r, g;
    double di = static_cast<double>(i);
    r = mUpFlank->execute(0, u, di);
    g = 1 + mDownFlank->execute(0, u, di);
    m_vtkLookupTable->SetTableValue(i, r, g, 0.0, 1.0);
  }
}

//----------------------------------------
void CBratLookupTable::RedToGreen()
{
  m_cust.RemoveAll();
  m_grad.RemoveAll();
  m_currentFunction = PALETTE_REDTOGREEN;

  double u = static_cast<double>(m_vtkLookupTable->GetNumberOfTableValues());

  for (size_t i = 0 ; i <  m_vtkLookupTable->GetNumberOfTableValues() ; i++)
  {
    double r, g;
    double di = static_cast<double>(i);
    r = 1 + mDownFlank->execute(0, u, di);
    g = mUpFlank->execute(0, u, di);
    m_vtkLookupTable->SetTableValue(i, r, g, 0.0, 1.0);
  }
}

//----------------------------------------
void CBratLookupTable::Cloud()
{
  m_cust.RemoveAll();
  m_grad.RemoveAll();
  m_currentFunction = PALETTE_CLOUD;

  double u = static_cast<double>(m_vtkLookupTable->GetNumberOfTableValues());

  for (size_t i = 0 ; i <  m_vtkLookupTable->GetNumberOfTableValues() ; i++)
  {
    double r, g, b, a;
    double di = static_cast<double>(i);
    r = 1 + mDownFlank->execute(0, 2.0 * u, di);
    g = 1 + mDownFlank->execute(0, 2.0 * u, di);
    b = 1 + mDownFlank->execute(0, 2.0 * u, di);
    a = mUpFlank->execute(0, 2.0 * u, i);
    m_vtkLookupTable->SetTableValue(i, r, g, b, a);
  }
}

//----------------------------------------
void CBratLookupTable::Rainbow()
{
  m_cust.RemoveAll();
  m_grad.RemoveAll();
  m_currentFunction = PALETTE_RAINBOW;

  double u = (1.0/6) * static_cast<double>(m_vtkLookupTable->GetNumberOfTableValues());

  for (size_t i = 0 ; i <  m_vtkLookupTable->GetNumberOfTableValues() ; i++)
  {
    double r, g, b;
    double di = static_cast<double>(i);
    r = 1 + mDownFlank->execute(2.*u, 3.*u, di) + mUpFlank->execute(4.*u, 6.*u, di);
    g = mUpFlank->execute(0, 2.*u, di) + mDownFlank->execute(3.*u, 4.*u, di);
    b = mUpFlank->execute(3.*u, 4.*u, di);
    m_vtkLookupTable->SetTableValue(i, r, g, b, 1.0);
  }
}
//----------------------------------------
void CBratLookupTable::Rainbow2()
{

  m_cust.RemoveAll();
  m_grad.RemoveAll();
  m_currentFunction = PALETTE_RAINBOW2;

  double u = (1.0/6) * static_cast<double>(m_vtkLookupTable->GetNumberOfTableValues());

  size_t j = m_vtkLookupTable->GetNumberOfTableValues() - 1;

  for (size_t i = 0 ; i <  m_vtkLookupTable->GetNumberOfTableValues() ; i++)
  {
    double r, g, b;
    double di = static_cast<double>(i);
    r = 1 + mDownFlank->execute(2.*u, 3.*u, di) + mUpFlank->execute(4.*u, 6.*u, di);
    g = mUpFlank->execute(0, 2.*u, di) + mDownFlank->execute(3.*u, 4.*u, di);
    b = mUpFlank->execute(3.*u, 4.*u, di);
    m_vtkLookupTable->SetTableValue(j, r, g, b, 1.0);
    j--;
  }
}

//----------------------------------------
void CBratLookupTable::Ozone()
{
  m_cust.RemoveAll();
  m_grad.RemoveAll();
  m_currentFunction = PALETTE_OZONE;

  double u = (1.0/7) * static_cast<double>(m_vtkLookupTable->GetNumberOfTableValues());

  for (size_t i = 0 ; i <  m_vtkLookupTable->GetNumberOfTableValues() ; i++)
  {
    double r, g, b;
    double di = static_cast<double>(i);
    r = mUpFlank->execute(3*u, 4*u, di);
    g = mUpFlank->execute(u, 2*u, di) + mDownFlank->execute(4*u, 5*u, di) + mUpFlank->execute(6*u, 7*u, di);
    b = mUpFlank->execute(0, u, di) + mDownFlank->execute(2*u, 3*u, di) + mUpFlank->execute(5*u, 6*u, di);
    m_vtkLookupTable->SetTableValue(i, r, g, b, 1.0);
  }
}

//----------------------------------------
void CBratLookupTable::Blackbody()
{
  m_cust.RemoveAll();
  m_grad.RemoveAll();
  m_currentFunction = "Blackbody";

  double u = (1.0/3) * static_cast<double>(m_vtkLookupTable->GetNumberOfTableValues());

  for (size_t i = 0 ; i <  m_vtkLookupTable->GetNumberOfTableValues() ; i++)
  {
    double r, g, b;
    double di = static_cast<double>(i);
    r = mUpFlank->execute(0, u, di);
    g = mUpFlank->execute(u, 2*u, di);
    b = mUpFlank->execute(2*u, 3*u, di);
    m_vtkLookupTable->SetTableValue(i, r, g, b, 1.0);
  }
}

//----------------------------------------
void CBratLookupTable::Aerosol()
{
  m_cust.RemoveAll();
  m_grad.RemoveAll();
  m_currentFunction = PALETTE_AEROSOL;

  double u = (1.0/4) * static_cast<double>(m_vtkLookupTable->GetNumberOfTableValues());

  for (size_t i = 0 ; i <  m_vtkLookupTable->GetNumberOfTableValues() ; i++)
  {
    double r, g, b;
    double di = static_cast<double>(i);
    r = mUpFlank->execute(2*u, 3*u, di);
    g = mUpFlank->execute(0, 2*u, di) + mDownFlank->execute(3*u, 4*u, di);
    b = SqrtUp(0, u, i) + mDownFlank->execute(2*u, 3*u, di);
    m_vtkLookupTable->SetTableValue(i, r, g, b, 1.0);
  }
}

//----------------------------------------
void CBratLookupTable::Aerosol2()
{
  m_cust.RemoveAll();
  m_grad.RemoveAll();
  m_currentFunction = PALETTE_AEROSOL2;

  double u = (1.0/4) * static_cast<double>(m_vtkLookupTable->GetNumberOfTableValues());
  size_t j = m_vtkLookupTable->GetNumberOfTableValues() - 1;
  for (size_t i = 0 ; i <  m_vtkLookupTable->GetNumberOfTableValues() ; i++)
  {
    double r, g, b;
    double di = static_cast<double>(i);
    r = mUpFlank->execute(2*u, 3*u, di);
    g = mUpFlank->execute(0, 2*u, di) + mDownFlank->execute(3*u, 4*u, di);
    b = SqrtUp(0, u, i) + mDownFlank->execute(2*u, 3*u, di);
    m_vtkLookupTable->SetTableValue(j, r, g, b, 1.0);
    j--;
  }
}



//----------------------------------------
void CBratLookupTable::Reset()
{
  m_cust.RemoveAll();
  m_grad.RemoveAll();
  m_currentFunction = m_resetFunction;

  for (size_t i = 0 ; i <  m_vtkLookupTable->GetNumberOfTableValues() ; i++)
  {
    m_vtkLookupTable->SetTableValue(i, 0.0, 0.0, 0.0, 1.0);
  }
}

//----------------------------------------
void CBratLookupTable::SetCurveLinear()
{

  SetCurveUpDown(static_cast< CallbackFlank* >(m_nameToMethod["LineUp"]),
                       static_cast< CallbackFlank* >(m_nameToMethod["LineDown"]));

  m_curve = CURVE_LINEAR();

  Update();
}


//----------------------------------------
void CBratLookupTable::SetCurveSQRT()
{

  SetCurveUpDown(static_cast< CallbackFlank* >(m_nameToMethod["SqrtUp"]),
                       static_cast< CallbackFlank* >(m_nameToMethod["SqrtDown"]));

  m_curve = CURVE_SQRT();

  Update();
}
//----------------------------------------

void CBratLookupTable::SetCurveCosinus()
{
  SetCurveUpDown(static_cast< CallbackFlank* >(m_nameToMethod["CosUp"]),
                       static_cast< CallbackFlank* >(m_nameToMethod["CosDown"]));

  m_curve = CURVE_COSINUS();

  Update();
}
//----------------------------------------
int32_t CBratLookupTable::GetMaxCustomXValue()
{
  int32_t max = -1;

  CObArray::iterator it;

  if (m_cust.empty() == false)
  {
    it = m_cust.end() - 1;
    CCustomColor* customColor = dynamic_cast<CCustomColor*>(*it);
    if (customColor == nullptr)
    {
      CException e("ERROR in CBratLookupTable::GetMaxCustomXValue - at least one of the array custom color object is not a CCustomColor object", BRATHL_LOGIC_ERROR);
      throw(e);
    }
    max = customColor->GetXValue();
  }

  /*
  for (it = m_cust.begin() ; it != m_cust.end() ; it++)
  {
    CCustomColor* customColor = dynamic_cast<CCustomColor*>(*it);
    if (customColor == nullptr)
    {
      CException e("ERROR in CBratLookupTable::GetMaxCustomXValue - at least one of the array custom color object is not a CCustomColor object", BRATHL_LOGIC_ERROR);
      throw(e);
    }
    max = (max < customColor->GetXValue()) ? customColor->GetXValue() : max;
  }
*/

  return max;
}

//----------------------------------------
void CBratLookupTable::FacetsCorrection()
{
  // empty array --> nothing to do
  if (m_cust.empty())
  {
    return;
  }

  if (m_cust.size() < 2)
  {
    std::string msg = CTools::Format("ERROR in CBratLookupTable::FacetsCorrection() - size of custom color array is less 2 : %ld",
                                (long)m_cust.size());
    CException e(msg, BRATHL_LOGIC_ERROR);
    throw(e);
  }

  CCustomColor* cMin = dynamic_cast<CCustomColor*>(*(m_cust.begin()));
  CCustomColor* cMax = dynamic_cast<CCustomColor*>(*(m_cust.end() - 1));
  if ( (cMin == nullptr) || (cMax == nullptr) )
  {
    CException e("ERROR in CBratLookupTable::FacetsCorrection - at least one of the array custom color object is not a CCustomColor object", BRATHL_LOGIC_ERROR);
    throw(e);
  }
  // min value correction
  if (cMin->GetXValue() != 0)
  {
    cMin->SetXValue(0);
  }

  // max value correction
  if (cMax->GetXValue() != m_vtkLookupTable->GetNumberOfTableValues())
  {
    cMax->SetXValue((int)m_vtkLookupTable->GetNumberOfTableValues());
  }


  //now we are sure that :
  // m_cust size is >= 2;
  // min value != max value

  // duplicate value correction
  CObArray::iterator it = m_cust.begin();

  do
  {
    CCustomColor* c1 = dynamic_cast<CCustomColor*>(*it);
    CCustomColor* c2 = dynamic_cast<CCustomColor*>(*(it+1));
    if ( (c1 == nullptr) || (c2 == nullptr))
    {
      CException e("ERROR in CBratLookupTable::FacetsCorrection - at least one of the array custom color object is not a CCustomColor object", BRATHL_LOGIC_ERROR);
      throw(e);
    }

    if (c1->GetXValue() == c2->GetXValue())
    {
      //delete duplication value (c2)
      m_cust.Erase(it+1);
      c2 = nullptr;
      it = m_cust.begin();
    }
    else
    {
      it++;
    }

  }
  while (it != m_cust.end() - 1);

}
//----------------------------------------
void CBratLookupTable::SetFacets(int32_t f)
{
  size_t old = m_vtkLookupTable->GetNumberOfTableValues();

  m_vtkLookupTable->SetNumberOfTableValues(f);

  CObArray::iterator it;

  for (it = m_cust.begin() ; it != m_cust.end() ; it++)
  {
    CCustomColor* customColor = dynamic_cast<CCustomColor*>(*it);
    if (customColor == nullptr)
    {
      CException e("ERROR in CBratLookupTable::SetFacets - at least one of the array custom color object is not a CCustomColor object", BRATHL_LOGIC_ERROR);
      throw(e);
    }
    int32_t newXValue = static_cast<int32_t>(static_cast<double>(customColor->GetXValue()) *
                                             static_cast<double>(f) / static_cast<double>(old));
    customColor->SetXValue(newXValue);
  }

  FacetsCorrection();

  Update();
}
//----------------------------------------
void CBratLookupTable::Update()
{
  ExecMethod(m_currentFunction);
}
//----------------------------------------
void CBratLookupTable::Gradient(const CPlotColor& c1, const CPlotColor& c2)
{
  m_cust.RemoveAll();
  m_grad.RemoveAll();
  m_grad.Insert(new CPlotColor(c1));
  m_grad.Insert(new CPlotColor(c2));

  Gradient();
}

//----------------------------------------
void CBratLookupTable::Gradient()
{
  m_cust.RemoveAll();

  if (m_grad.size() != 2)
  {
    std::string msg = CTools::Format("ERROR in CBratLookupTable::Gradient() - size of color array (must contains min/max) not equal 2 : %ld",
                                (long)m_grad.size());
    CException e(msg, BRATHL_LOGIC_ERROR);
    throw(e);
  }
  CPlotColor* c1 = dynamic_cast<CPlotColor*>(m_grad.at(0));
  CPlotColor* c2 = dynamic_cast<CPlotColor*>(m_grad.at(1));
  if ( (c1 == nullptr) || (c2 == nullptr))
  {
    CException e("ERROR in CBratLookupTable::Gradient() - at least one of the color object is not a CPlotColor object", BRATHL_LOGIC_ERROR);
    throw(e);
  }

  //m_std = m_resetFunction;
  m_currentFunction = m_gradientFunction;

  DrawGradient(*c1, *c2, 0, (int)m_vtkLookupTable->GetNumberOfTableValues());

}


//----------------------------------------
void CBratLookupTable::Custom(const CCustomColor& c1, const CCustomColor& c2)
{
  m_grad.RemoveAll();

  m_cust.RemoveAll();
  m_cust.Insert(new CCustomColor(c1));
  m_cust.Insert(new CCustomColor(c2));

  Custom();
}

//----------------------------------------
void CBratLookupTable::Custom(const CObArray& cust)
{
  m_grad.RemoveAll();

  m_cust.RemoveAll();
  DupCustMap(cust);

  Custom();

}

//----------------------------------------
void CBratLookupTable::Custom()
{
  m_grad.RemoveAll();

  if (m_cust.size() < 2)
  {
    std::string msg = CTools::Format("ERROR in CBratLookupTable::Custom() - size of custom color array is less 2 : %ld",
                                (long)m_cust.size());
    CException e(msg, BRATHL_LOGIC_ERROR);
    throw(e);
  }

  //m_std = m_resetFunction;
  m_currentFunction = m_customFunction;
  int32_t maxXValue = GetMaxCustomXValue();
  if (maxXValue < 0)
  {
    std::string msg = CTools::Format("ERROR in CBratLookupTable::Custom() - max of X value is negative value : %d",
                                  maxXValue);
    CException e(msg, BRATHL_LOGIC_ERROR);
    throw(e);
  }

  int32_t u = static_cast<int32_t>(static_cast<double>(m_vtkLookupTable->GetNumberOfTableValues())
                                   / static_cast<double>(maxXValue));
  CObArray::iterator it;

  for (it = m_cust.begin() ; it != m_cust.end() - 1 ; it++)
  {
    CCustomColor* c1 = dynamic_cast<CCustomColor*>(*it);
    CCustomColor* c2 = dynamic_cast<CCustomColor*>(*(it+1));
    if ( (c1 == nullptr) || (c2 == nullptr))
    {
      CException e("ERROR in CBratLookupTable::Custom - at least one of the array custom color object is not a CCustomColor object", BRATHL_LOGIC_ERROR);
      throw(e);
    }

    DrawGradient(*(c1->GetColor()), *(c2->GetColor()),  c1->GetXValue() * u, c2->GetXValue() * u);

  }

}

//----------------------------------------

void CBratLookupTable::DrawGradient(const CPlotColor& c1, const CPlotColor& c2, int32_t i1, int32_t i2)
{
  double d1 = static_cast<double>(i1);
  double d2 = static_cast<double>(i2);

  for (int32_t i = i1 ; i < i2 ; i++)
  {
    double r, g, b, a;
    double di = static_cast<double>(i);
    r =   c1.Red()
        + c1.Red() * mDownFlank->execute(d1, d2, di)
        + c2.Red() * mUpFlank->execute(d1, d2, di);
    r = (r > 1) ? 1 : r;

    g =  c1.Green()
       + c1.Green() * mDownFlank->execute(d1, d2, di)
       + c2.Green() * mUpFlank->execute(d1, d2, di);
    g = (g > 1) ? 1 : g;

    b =  c1.Blue()
       + c1.Blue() * mDownFlank->execute(d1, d2, di)
       + c2.Blue() * mUpFlank->execute(d1, d2, di);
    b = (b > 1) ? 1 : b;

    a =  c1.Alpha()
       + c1.Alpha() * mDownFlank->execute(d1, d2, di)
       + c2.Alpha() * mUpFlank->execute(d1, d2, di);
    a = (a > 1) ? 1 : a;

    m_vtkLookupTable->SetTableValue(i, r, g, b, a);
  }
}

//----------------------------------------
void CBratLookupTable::DupCustMap(const CObArray& cust)
{
  CBratLookupTable::DupCustMap(cust, m_cust);
}

//----------------------------------------
void CBratLookupTable::DupCustMap(const CObArray& custSrc, CObArray& custDest)
{
  CObArray::const_iterator it;

  for (it = custSrc.begin() ; it != custSrc.end() ; it++)
  {
    CCustomColor* customColor = dynamic_cast<CCustomColor*>(*it);
    custDest.Insert(new CCustomColor(*customColor));
  }

}

//----------------------------------------
void CBratLookupTable::DupGradMap(const CObArray& grad)
{
  CObArray::const_iterator it;

  for (it = grad.begin() ; it != grad.end() ; it++)
  {
    CPlotColor* color = dynamic_cast<CPlotColor*>(*it);
    m_grad.Insert(new CPlotColor(*color));
  }

}
//----------------------------------------
void CBratLookupTable::SaveToFile( const std::string& file_name )
{
	m_fileName = file_name;
	try
	{
		RemoveFile( m_fileName );
	}
	catch ( ... )
	{
		//do nothing
	}

	//wxFileConfig file( wxGetApp().GetAppName(), wxEmptyString, m_fileName, wxEmptyString, wxCONFIG_USE_LOCAL_FILE );
	CBratLookupTableSettings file( m_fileName );

	file.SaveToFile( *this );

    //bOk = bOk && file.Write( ENTRY_NUMCOLORS(), static_cast<int>( m_vtkLookupTable->GetNumberOfTableValues() ) );
    //bOk = bOk && file.Write( ENTRY_CURRENTFCT(), m_currentFunction.c_str() );
    //bOk = bOk && file.Write( ENTRY_CURVE(), m_curve.c_str() );
	//if ( bOk == false )
	//{
	//	CException e( CTools::Format( "ERROR in CBratLookupTable::SaveToFile - Can't write file %s", m_fileName.c_str() ),
	//		BRATHL_LOGIC_ERROR );
	//	throw( e );
	//}

	//SaveGradToFile( file );

	//SaveCustToFile( file );
}
//----------------------------------------
//void CBratLookupTable::SaveCustToFile(CBratLookupTableSettings& file)
//{
//	file.SaveCustToFile( *this );
//}
////----------------------------------------
//void CBratLookupTable::SaveGradToFile(CBratLookupTableSettings& file)
//{
//	file.SaveGradToFile( *this );
//}
//----------------------------------------
void CBratLookupTable::HandleLoadError(bool bOk, const std::string& entry)
{
  if (bOk == false)
  {
    CException e(CTools::Format("ERROR in CBratLookupTable::LoadFromFile - Can't read '%s' from file %s",
                                entry.c_str(), m_fileName.c_str()),
                 BRATHL_LOGIC_ERROR);
    throw(e);

  }

}
//----------------------------------------
void CBratLookupTable::HandleLoadColorError( std::string &error_msg, bool bOk, const std::string& entry, double& color )
{
	HandleLoadError( bOk, entry );

	if ( color < 0.0 )
	{
		error_msg =
			"Invalid value "
			+ n2s<std::string>( color )
			+ " for '"
			+ entry
			+ "'\n Value will bet set to 0";

		color = 0.0;
	}

	if ( color > 1.0 )
	{
		error_msg =
			"Invalid value "
			+ n2s<std::string>( color )
			+ " for '"
			+ entry
			+ "'\n Value will bet set to 1";

		color = 1.0;
	}
}
//----------------------------------------
bool CBratLookupTable::LoadFromFile( std::string &error_msg, const std::string& file_name )
{
	m_fileName = file_name;

	//wxFileConfig file( wxGetApp().GetAppName(), wxEmptyString, m_fileName, wxEmptyString, wxCONFIG_USE_LOCAL_FILE );
	CBratLookupTableSettings file( m_fileName );

	return file.LoadFromFile( error_msg, *this );
}
////----------------------------------------
//bool CBratLookupTable::LoadGradFromFile(CBratLookupTableSettings& file)
//{
//	return file.LoadGradFromFile( *this );
//}
////----------------------------------------
//bool CBratLookupTable::LoadCustFromFile(CBratLookupTableSettings& file)
//{
//	return file.LoadCustFromFile( *this );
//}

//----------------------------------------
int32_t CBratLookupTable::InsertCustomColor(CCustomColor *color, std::string &warning )
{
  if (m_cust.size() <= 0)
  {
    m_cust.Insert(color);
    return 0;
  }

  CCustomColor* cMin = dynamic_cast<CCustomColor*>(*(m_cust.begin()));
  CCustomColor* cMax = dynamic_cast<CCustomColor*>(*(m_cust.end() - 1));
  if ( (cMin == nullptr) || (cMax == nullptr) )
  {
    CException e("ERROR in CBratLookupTable::InsertCustomColor - at least one of the array custom color object is not a CCustomColor object", BRATHL_LOGIC_ERROR);
    throw(e);
  }


  bool duplicate = false;
  CObArray::iterator it;

  for (it = m_cust.begin() ; it != m_cust.end() ; it++)
  {
    CCustomColor* c = dynamic_cast<CCustomColor*>(*it);
    if (c == nullptr)
    {
      CException e("ERROR in CBratLookupTable::InsertCustomColor - at least one of the array custom color object is not a CCustomColor object", BRATHL_LOGIC_ERROR);
      throw(e);
    }

    if (c->GetXValue() > color->GetXValue())
    {
      duplicate = false;
      break;
    }
    else if (c->GetXValue() == color->GetXValue())
    {
      duplicate = true;
      break;
    }
  }

  if (duplicate)
  {
	  warning = "There are duplicate color definitions (value " + color->GetStringXValue() + ") in file " + m_fileName + "\n Color definition will be ignored";
               //"Warning",
               // wxOK | wxCENTRE | wxICON_EXCLAMATION);
	  return -1;
  }

  CObArray::iterator itInserted = m_cust.InsertAt(it, color);

  CObArray::difference_type diff ;
  diff = distance (m_cust.begin ( ) , itInserted );

  return diff;


}
//----------------------------------------
void CBratLookupTable::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }


  fOut << "==> Dump a CBratLookupTable Object at "<< this << std::endl;


  fOut << "m_currentFunction = " << m_currentFunction << std::endl;
  fOut << "m_curve " << m_curve << std::endl;
//  fOut << "m_std = " << m_std << std::endl;
  fOut << "m_vtkLookupTable at " << m_vtkLookupTable << std::endl;

  m_nameToMethod.Dump(fOut);


  fOut << "==> END Dump a CBratLookupTable Object at "<< this << std::endl;

  fOut << std::endl;

}

