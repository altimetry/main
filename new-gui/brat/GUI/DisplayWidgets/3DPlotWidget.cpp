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

#include <QtGui>
#include <QColor>

#include <qwt3d_plot.h>
#include <qwt3d_function.h>


#include "libbrathl/Date.h"
using namespace brathl;

#include "common/+UtilsIO.h"
#include "new-gui/Common/QtUtils.h"
#include "DataModels/PlotData/PlotValues.h"
#include "DataModels/PlotData/BratLookupTable.h"

#include "PSProcessing.h"
#include "GUI/ActionsTable.h"

#include "3DPlotWidget.h"


// TODO delete this macro; if active, no operation is expected to work besides the simple example display
//#define TEST_EXAMPLES


//////////////////////////////////////////
//
//		Auxiliary Qwt3D Classes
//
//			- color
//			- function
//			- scale
//			- plot
//
//////////////////////////////////////////

//////////////////////////////////////////
//	color
//////////////////////////////////////////

class CBratColor : public Qwt3D::StandardColor
{
	// types

	using base_t = Qwt3D::StandardColor;

	//static members

	static double interpolate( double val, double y0, double x0, double y1, double x1 )
	{
		return ( val - x0 )*( y1 - y0 ) / ( x1 - x0 ) + y0;
	}

	static double blue( double grayscale )
	{
		if ( grayscale < -0.33 ) return 1.0;
		else if ( grayscale < 0.33 ) return interpolate( grayscale, 1.0, -0.33, 0.0, 0.33 );
		else return 0.0;
	}

	static double green( double grayscale )
	{
		if ( grayscale < -1.0 ) return 0.0; // unexpected grayscale value
		if ( grayscale < -0.33 ) return interpolate( grayscale, 0.0, -1.0, 1.0, -0.33 );
		else if ( grayscale < 0.33 ) return 1.0;
		else if ( grayscale <= 1.0 ) return interpolate( grayscale, 1.0, 0.33, 0.0, 1.0 );
		else return 1.0; // unexpected grayscale value
	}

	static double red( double grayscale )
	{
		if ( grayscale < -0.33 ) return 0.0;
		else if ( grayscale < 0.33 ) return interpolate( grayscale, 0.0, -0.33, 1.0, 0.33 );
		else return 1.0;
	}

	//construction / destruction

public:
#if (QWT3D_MINOR_VERSION > 2)
	CBratColor( Qwt3D::Curve *data, unsigned size = 100 ) 
#else
	CBratColor( Qwt3D::Plot3D *data, unsigned size = 100 ) 
#endif
		: base_t( data, size )
	{
		reset(size);
	}

	virtual ~CBratColor()
	{}

	void reset( unsigned size )
	{
		colors_ = Qwt3D::ColorVector( size );
		Qwt3D::RGBA elem;

		double dsize = size;

		for ( unsigned int i=0; i != size; ++i )
		{
			elem.r = red( i / dsize );
			elem.g = green( i / dsize );
			elem.b = blue( i / dsize );
			elem.a = 1.0;
			colors_[ i ] = elem;
		}
	}
};



//////////////////////////////////////////
//			function
//////////////////////////////////////////

struct CBrat3DFunction : public Qwt3D::Function
{
	const CZFXYPlotParameters *mPlotParameters = nullptr;

	double mxmin;
	double mxmax;
	double mymin;
	double mymax;
	double mzmin;
	double mzmax;

	bool mLogarithmic = false;

#if (QWT3D_MINOR_VERSION > 2)
	CBrat3DFunction( Qwt3D::Curve& pw ) 
#else
	CBrat3DFunction( SurfacePlot& pw ) 
#endif
		: Qwt3D::Function( pw )
	{}
	virtual ~CBrat3DFunction()
	{}


	inline double CorrectedValue( double value ) const
	{
		if ( !mLogarithmic )
			return value;

		return value == 0 ? 0 : log( value );
	}


	void SetSurface( const CZFXYPlotParameters &values )
	{
		assert__( values.mValues.size() == values.mBits.size() );
		assert__( values.mValues.size() == values.mXaxis.size() * values.mYaxis.size() );

		mPlotParameters = &values;
		mxmin = values.mMinX;
		mxmax = values.mMaxX;
		mymin = values.mMinY;
		mymax = values.mMaxY;

		setMesh( (int)mPlotParameters->mXaxis.size(), (int)mPlotParameters->mYaxis.size() );		//setMesh( xmax, ymax );
		setDomain( mxmin, mxmax, mymin, mymax );

		SetZRange( values.mMinHeightValue, values.mMaxHeightValue );
	}


	void SetZRange( double zmin, double zmax )
	{
		if ( isDefaultValue( zmin ) || isDefaultValue( zmax ) )
		{
			mzmin = 0;
			mzmax = 0;
		}
		else
		{
			mzmin = zmin;
			mzmax = zmax;
		}

		SetZRange();
	}


	void SetZRange()
	{
		setMinZ( CorrectedValue( mzmin ) );
		setMaxZ( CorrectedValue( mzmax ) );

		create();
	}


	bool Logarithmic() const { return mLogarithmic; }

	void SetLogarithmic( bool log )
	{
		if ( mLogarithmic == log )
			return;

		mLogarithmic = log;

		SetZRange();
	}


	void Ranges( double &xMin, double &xMax, double &yMin, double &yMax, double &zMin, double &zMax ) const
	{
		xMin = mxmin;
		xMax = mxmax;

		yMin = mymin;
		yMax = mymax;

		zMin = CorrectedValue( mzmin );
		zMax = CorrectedValue( mzmax );
	}


	//Qwt3D::Function interface

    virtual double operator()( double x, double y ) override
    {
		return CorrectedValue( mPlotParameters->value_v( x, y ) );		//mPlotParameters->value_s( x, y )
    }
};




//////////////////////////////////////////
//				Scale
//////////////////////////////////////////



template< class QWT3DSCALE, int DIGITS, int MINORS = 0 >
class CBrat3DScale : public  QWT3DSCALE
{
	//types

	using base_t = QWT3DSCALE;


	//statics

public:
	static const int smDefaultDigits = DIGITS;
	static const int smDefaultMinors = MINORS;		//log scale only accepts 2, 3, 5 and 9
	static const int smDefaultMajors = 5;


protected:
	//instance data

	int mDigits = smDefaultDigits;


	//construction / destruction

public:
	CBrat3DScale()
		: base_t()
	{}
	virtual ~CBrat3DScale()
	{}

    // getters / setters / testers

    //Number of digits on the scientific representation of the scale

    unsigned int GetNDigits() const { return mDigits; }
    void SetNDigits( int digits = -1 )
	{ 
		mDigits = digits <= 0 ? smDefaultDigits : digits; 
	}
};




class CBrat3DLinearScale : public CBrat3DScale< Qwt3D::LinearScale, 2 >
{
	//types

	using base_t = CBrat3DScale< Qwt3D::LinearScale, 2 >;

	//statics

protected:
	//instance data

	bool mIsDate = false;
    brathl_refDate mDateRef = REF19500101;

	//construction / destruction

public:
	CBrat3DLinearScale()
		: base_t()
	{
		setMinors( smDefaultMinors );
		setMajors( smDefaultMajors );
	}
	virtual ~CBrat3DLinearScale()
	{}

    // getters / setters / testers

    //date scale

    bool IsDate() const { return mIsDate; }
    void SetDate( bool date, brathl_refDate date_ref = REF19500101 )
	{ 
		mIsDate = date; 
        if (mIsDate)
        {
            mDateRef = date_ref;
        }
	}


protected:

	//qwt3d interface

	virtual Scale* clone() const override
	{
		CBrat3DLinearScale * new_nb = new CBrat3DLinearScale;
		new_nb->SetDate( mIsDate, mDateRef );
		new_nb->SetNDigits( mDigits );
		return new_nb;
	}

	virtual QString ticLabel( unsigned int idx ) const override
	{
		if ( idx < majors_p.size() )
		{
			if ( mIsDate )
			{
				auto value = majors_p[ idx ];
                CDate d( value, mDateRef );
				if ( !d.IsDefaultValue() )
					return QString( d.AsString().c_str() );
			}

            return n2qx( majors_p[ idx ], mDigits );
		}

		return QString( "" );
	}
};



class CBrat3DLogScale : public CBrat3DScale< Qwt3D::LogScale, 10, 2 >
{
	//types

	using base_t = CBrat3DScale< Qwt3D::LogScale, 10, 2 >;


	//statics

protected:

	//construction / destruction

public:
	CBrat3DLogScale()
		: base_t()
	{
		setMinors( smDefaultMinors );		//log scale only accepts 2, 3, 5 and 9
		setMajors( smDefaultMajors );
	}
	CBrat3DLogScale( const CBrat3DLogScale &o )
		: base_t( o )								//useless
	{
		*this = o;
	}
	virtual ~CBrat3DLogScale()
	{}


	CBrat3DLogScale& operator = ( const CBrat3DLogScale &o )
	{
		if ( this != &o )
		{
			base_t::operator=( o );
			setMinors( smDefaultMinors );
			setMajors( smDefaultMajors );
			SetNDigits( o.mDigits );
		}
		return *this;
	}

    // getters / setters / testers

	void SetLimits( double start, double stop )
	{
		double runningval = floor( start );
		if ( runningval >= stop )
			stop += 1.;

		if ( runningval <= start )
			start -= 1.;

		base_t::setLimits( start, stop );
		calculate();
	}

protected:

	//qwt3d interface

	virtual Scale* clone() const override
	{
		return new CBrat3DLogScale( *this );
	}

	virtual QString ticLabel( unsigned int idx ) const override
	{
		if ( idx < majors_p.size() )
		{
			double scale_log = pow( 10., majors_p[ idx ] );
            return n2qx( scale_log, mDigits );
		}

		return QString( "" );
	}
};





//////////////////////////////////////////
//			Qwt3D plot class
//////////////////////////////////////////


//static 
const std::vector< Qwt3D::AXIS > CBrat3DPlot::smXaxis = { Qwt3D::AXIS::X1, Qwt3D::AXIS::X2, Qwt3D::AXIS::X3, Qwt3D::AXIS::X4 };
//static 
const std::vector< Qwt3D::AXIS > CBrat3DPlot::smYaxis = { Qwt3D::AXIS::Y1, Qwt3D::AXIS::Y2, Qwt3D::AXIS::Y3, Qwt3D::AXIS::Y4 };
//static 
const std::vector< Qwt3D::AXIS > CBrat3DPlot::smZaxis = { Qwt3D::AXIS::Z1, Qwt3D::AXIS::Z2, Qwt3D::AXIS::Z3, Qwt3D::AXIS::Z4 };




//virtual 
void CBrat3DPlot::paintGL()
{
	base_t::paintGL();
}



CBrat3DPlot::CBrat3DPlot( const CPlotsGraphicParameters &plots_graphic_parameters, QWidget *pw )
	: base_t( pw )
#if defined (Q_OS_MAC)
    , mPlotsGraphicParameters( {
          plots_graphic_parameters.mFontName,
          plots_graphic_parameters.mAxisFontSize,
          plots_graphic_parameters.mAxisBold,
          plots_graphic_parameters.mTitleFontSize,
          plots_graphic_parameters.mTitleBold,
          plots_graphic_parameters.mSizeHint
          } )
#else
    , mPlotsGraphicParameters( plots_graphic_parameters )
#endif
	, mTimer( this )
	, mXDigits( CBrat3DLinearScale::smDefaultDigits )
	, mYDigits( CBrat3DLinearScale::smDefaultDigits )
	, mZDigits( CBrat3DLinearScale::smDefaultDigits )
{
	connect( &mTimer, SIGNAL( timeout() ), this, SLOT( rotate() ) );

	setBackgroundColor( Qwt3D::Qt2GL( ViewsDefaultBackgroundColor() ) );

	SetCoordinateStyle();	//call before CreateContextMenu
	CreateContextMenu();
	Reset();

	setSmoothMesh( true );
	enableLighting( true );

	setMeshLineWidth( 1 );
	coordinates()->setGridLinesColor( Qwt3D::RGBA( 0, 0, 0.5 ) );
	coordinates()->setLineWidth( 1 );
	coordinates()->setNumberColor( Qwt3D::RGBA( 0, 0.5, 0 ) );

	coordinates()->adjustNumbers( 5 );
	coordinates()->adjustLabels( 10 );

	setTitleFont( 
		t2q( mPlotsGraphicParameters.mFontName ), 
		mPlotsGraphicParameters.mTitleFontSize, 
		mPlotsGraphicParameters.mTitleBold ? QFont::Bold : QFont::Normal );	//11 in axes example

	coordinates()->setNumberFont( t2q( mPlotsGraphicParameters.mFontName ), 
		mPlotsGraphicParameters.mAxisFontSize );							//10 in axes example

	coordinates()->setLabelFont( 
		t2q( mPlotsGraphicParameters.mFontName ), 
		mPlotsGraphicParameters.mAxisFontSize, 
		mPlotsGraphicParameters.mAxisBold ? QFont::Bold : QFont::Normal );	//12 in axes example, & bold

    legend()->setTitleFont( 
                t2q( mPlotsGraphicParameters.mFontName ), 
                mPlotsGraphicParameters.mTitleFontSize, QFont::Normal );
    
    legend()->setAxisFont( t2q( mPlotsGraphicParameters.mFontName ), 
		mPlotsGraphicParameters.mAxisFontSize );

	const size_t size = coordinates()->axes.size();
	for ( unsigned i = 0; i != size; ++i )
	{
		coordinates()->axes[ i ].setMajors( CBrat3DLinearScale::smDefaultMajors );
		coordinates()->axes[ i ].setMinors( CBrat3DLinearScale::smDefaultMinors );
	}

	//set all to defaults except last 2
	//
	assignMouse( 
		Qwt3D::MouseState( Qt::LeftButton ), 
		Qwt3D::MouseState( Qt::LeftButton, Qt::ShiftModifier ), 
		Qwt3D::MouseState( Qt::LeftButton ),

		Qwt3D::MouseState( Qt::LeftButton, Qt::AltModifier ),
		Qwt3D::MouseState( Qt::LeftButton, Qt::AltModifier ),
		Qwt3D::MouseState( Qt::LeftButton, Qt::AltModifier | Qt::ShiftModifier ),

		Qwt3D::MouseState( Qt::LeftButton, Qt::AltModifier | Qt::ControlModifier ),
		Qwt3D::MouseState( Qt::MiddleButton ),		//replaces default Qwt3D::MouseState( Qt::LeftButton, Qt::ControlModifier ),
		Qwt3D::MouseState( Qt::MiddleButton )		//replaces default Qwt3D::MouseState( Qt::LeftButton, Qt::ControlModifier )
		);
}

//virtual 
CBrat3DPlot::~CBrat3DPlot()
{
	DestroyPointersAndContainer( mFunctions );
}



void CBrat3DPlot::AddSurface( const CZFXYPlotParameters &values )
{
	mFunctions.push_back( new CBrat3DFunction( *this ) );
	mFunctions.back()->SetSurface( values );
}




//title

void CBrat3DPlot::SetTitle( const QString &title )
{
	setTitle( title );

    updateGL();
}



//axis titles

const std::string& CBrat3DPlot::AxisTitle( Qwt3D::AXIS axis ) const
{
	if ( In( axis, smXaxis ) )
		return mXlabel;
	else
	if ( In( axis, smYaxis ) )
		return mYlabel;
	else
	if ( In( axis, smZaxis ) )
		return mZlabel;
	else
		assert__( false );

	return empty_string< std::string >();
}
bool CBrat3DPlot::SetAxisTitle( Qwt3D::AXIS axis, const std::string &title, std::string &data_member )
{
	//std::string clean_title = replace( title, "\n", " - " );
	//clean_title = replace( clean_title, "\t", "" );

	if ( title.empty() )
		return false;

	data_member = title;
	coordinates()->axes[ axis ].setLabelString( data_member.c_str() );

	return true;
}


bool CBrat3DPlot::SetXYAxisTitles( const std::string &xtitle, const std::string &ytitle )
{
	for ( auto axis : smXaxis )
	{
		if ( !SetAxisTitle( axis, xtitle, mXlabel ) )
			return false;
	}
	
	for ( auto axis : smYaxis )
	{
		if ( !SetAxisTitle( axis, ytitle, mYlabel ) )
			return false;
	}

	return true;
}

bool CBrat3DPlot::SetZAxisTitle( const std::string &ztitle )
{
	for ( auto axis : smZaxis )
	{
		if ( !SetAxisTitle( axis, ztitle, mZlabel ) )
			return false;
	}

	return true;
}


//axis ticks

unsigned int CBrat3DPlot::XAxisNbTicks() const
{
	return const_cast<CBrat3DPlot*>( this )->coordinates()->axes[ Qwt3D::AXIS::X1 ].majors();
}
void CBrat3DPlot::SetXAxisTicks( unsigned int nticks )
{
    for ( auto axis : smXaxis )
    {
        coordinates()->axes[ axis ].setMajors( nticks );
    }
    updateGL();
}


unsigned int CBrat3DPlot::YAxisNbTicks() const
{
	return const_cast<CBrat3DPlot*>( this )->coordinates()->axes[ Qwt3D::AXIS::Y1 ].majors();
}
void CBrat3DPlot::SetYAxisTicks( unsigned int nticks )
{
    for ( auto axis : smYaxis )
    {
        coordinates()->axes[ axis ].setMajors( nticks );
    }
    updateGL();
}


unsigned int CBrat3DPlot::ZAxisNbTicks() const
{
	return const_cast<CBrat3DPlot*>( this )->coordinates()->axes[ Qwt3D::AXIS::Z1 ].majors();
}
void CBrat3DPlot::SetZAxisTicks( unsigned int nticks )
{
    for ( auto axis : smZaxis )
    {
        coordinates()->axes[ axis ].setMajors( nticks );
    }

	if ( legend() )
		legend()->setMajors( coordinates()->axes[ smZaxis[0] ].majors() );

    updateGL();
}


//animation

void CBrat3DPlot::Animate( int updateinterval )		//updateinterval = 200 
{
	if ( !mAnimateAction->isChecked() || updateinterval == 0 )
	{
		mTimer.stop();
	}
	else
	{
		mTimer.start( updateinterval );
	}
}

void CBrat3DPlot::rotate()
{
	int prec = 3;

	setRotation(
			(int(prec*xRotation() + 2) % (360*prec))/double(prec),
			(int(prec*yRotation() + 2) % (360*prec))/double(prec),
			(int(prec*zRotation() + 2) % (360*prec))/double(prec)
			);
}


// digits / scale / date

void CBrat3DPlot::SetDigits( int &this_digits, const std::vector< Qwt3D::AXIS > &axis_ids, bool islog, bool isdate, int digits, brathl_refDate date_ref )
{
	assert__( !islog || !isdate );			//old code: "assert__( this_digits != -1 ); //can only be assigned once"; why?

	this_digits = isdate ? -1 : digits;
	for ( auto axis : axis_ids )
    {
		Qwt3D::Scale *s = nullptr;
		if ( islog )
		{
			CBrat3DLogScale *slog = new CBrat3DLogScale;
			slog->SetNDigits( this_digits );
			double start, stop;
            coordinates()->axes[ axis ].limits( start, stop );
			slog->SetLimits( start, stop );
			s = slog;
		}
		else
		{
			CBrat3DLinearScale *slinear = new CBrat3DLinearScale;        //s->SetLog(is_log);
			slinear->SetDate( isdate, date_ref );
			slinear->SetNDigits( this_digits );
			s = slinear;
		}

        coordinates()->axes[ axis ].setScale( s );
		if ( islog )
		{
			coordinates()->axes[ axis ].setScaling( true );
			coordinates()->axes[ axis ].draw();
		}
    }

    updateGL();
}

void CBrat3DPlot::SetXDigits( bool isdate, int digits, brathl_refDate date_ref )      //date_ref = REF19500101
{
    SetDigits( mXDigits, smXaxis, false, isdate, digits, date_ref );
}

void CBrat3DPlot::SetYDigits( bool isdate, int digits, brathl_refDate date_ref )      //date_ref = REF19500101
{
    SetDigits( mYDigits, smYaxis, false, isdate, digits, date_ref );
}

void CBrat3DPlot::SetZDigits( bool isdate, int digits, brathl_refDate date_ref )      //date_ref = REF19500101
{
    SetDigits( mZDigits, smZaxis, LogarithmicScaleZ(), isdate, digits, date_ref );
}



//scale

bool CBrat3DPlot::LogarithmicScaleZ() const
{
	assert__( mFunctions.size() == 1 );

	auto const *function = mFunctions.back();		assert__( function );

	return function->Logarithmic();
}

void CBrat3DPlot::SetLogarithmicScaleZ( bool log )
{
	for ( auto *f : mFunctions )
	{
		assert__( f );

		f->SetLogarithmic( log );
	}

    SetXDigits( XisDateTime(), mXDigits );          //functions setZRange rebuild whole coordinate system...
    SetYDigits( YisDateTime(), mYDigits );          //functions setZRange rebuild whole coordinate system...
    SetZDigits( !log && ZisDateTime(), mZDigits );	//the only that is partially supported
}


void CBrat3DPlot::FunctionRanges( double &xMin, double &xMax, double &yMin, double &yMax, double &zMin, double &zMax ) const
{
	assert__( mFunctions.size() == 1 );

	auto const *function = mFunctions.back();		assert__( function );

	function->Ranges( xMin, xMax, yMin, yMax, zMin, zMax );
}



void CBrat3DPlot::Scale( double &xVal, double &yVal, double &zVal )
{
	xVal = xScale();
	yVal = yScale();
	zVal = zScale();
}



template< typename T >
inline int MTicLength( T &t )
{
	double m, M;
	t.ticLength( M, m );
	return M;
}
template< typename T >
inline int mTicLength( T &t )
{
	double m, M;
	t.ticLength( M, m );
	return m;
}


void CBrat3DPlot::SetScale( double xVal, double yVal, double zVal )
{
	setScale( xVal, yVal, zVal );

	for ( auto axis : smXaxis )
		coordinates()->axes[ axis ].setTicLength( 2, 1 );
	for ( auto axis : smYaxis )
		coordinates()->axes[ axis ].setTicLength( 2, 1 );
	for ( auto axis : smZaxis )
		coordinates()->axes[ axis ].setTicLength( 2, 1 );
}


void CBrat3DPlot::Reset()
{
	setRotation( 30, 0, 15 );
	setScale( 1, 1, 1 );
	setShift( 0.1, 0, 0 );
	setZoom( 0.8 );
}


//color / style

void CBrat3DPlot::StandardColor( bool standard )
{
	if ( standard )
		setDataColor( new Qwt3D::StandardColor( this ) );
	else
#if (QWT3D_MINOR_VERSION > 2)
		setDataColor( new CBratColor( curve() ) );
#else
		setDataColor( new CBratColor( this ) );
#endif
	updateData();
}


void CBrat3DPlot::SetPlotStyle( Qwt3D::PLOTSTYLE style )		//style = Qwt3D::FILLEDMESH
{
	setPlotStyle( style );
	updateData();
	updateGL();
}

void CBrat3DPlot::PlotStyle()
{
	auto a = qobject_cast<QAction*>( sender() );		assert__( a );
	auto index = mStyleGroup->actions().indexOf( a );	assert__( index >= 0 && a->data().toInt() == index );

    Q_UNUSED( index );

	SetPlotStyle( static_cast<Qwt3D::PLOTSTYLE>( a->data().toInt() ) );
}


void CBrat3DPlot::SetCoordinateStyle( Qwt3D::COORDSTYLE style )		//style = Qwt3D::FRAME
{
	setCoordinateStyle( style );
}

void CBrat3DPlot::CoordinateStyle()
{
	auto a = qobject_cast<QAction*>( sender() );					assert__( a );
	auto index = mCoordinateStyleGroup->actions().indexOf( a );		assert__( index >= 0 && a->data().toInt() == index );

    Q_UNUSED( index );

    SetCoordinateStyle( static_cast<Qwt3D::COORDSTYLE>( a->data().toInt() ) );
}




void CBrat3DPlot::SetColorMap( QLookupTable *pcolor_map )
{
	setDataColor( pcolor_map );
	updateData();
	updateGL();

	double start = pcolor_map->CurrentMinValue(), stop = pcolor_map->CurrentMaxValue();
	//coordinates()->axes[ Qwt3D::AXIS::Z1 ].limits( start,stop );

#if (QWT3D_MINOR_VERSION > 2)
	curve()->legend()->setLimits( start, stop );
#else
	legend()->setLimits( start, stop );
	legend()->setMajors( coordinates()->axes[ Qwt3D::AXIS::Z1 ].majors() );
#endif

	if ( !isDefaultValue( start ) && !isDefaultValue( stop ) && start != defaultValueDOUBLE && stop != defaultValueDOUBLE )
		showColorLegend( true );
}




//test menu

void CBrat3DPlot::CreateContextMenu()
{
	QList<QAction*> style_actions_list;

	auto create_style_item = [ this, &style_actions_list ]( const std::string &name, const std::string tip, Qwt3D::PLOTSTYLE style )
	{
		auto a = new QAction( name.c_str(), mStyleGroup );
		a->setCheckable( true );
#if (QWT3D_MINOR_VERSION > 2)
		a->setChecked( curve()->plotStyle() == style );
#else
		a->setChecked( plotStyle() == style );
#endif
		a->setToolTip( tip.c_str() );
		a->setData( style );
		style_actions_list << a;
		connect( a, SIGNAL( triggered() ), this, SLOT( PlotStyle() ) );
	};



	setContextMenuPolicy( Qt::ActionsContextMenu );

#if defined (DEBUG) || defined (_DEBUG)
	mAnimateAction = new QAction( "Animate", this );
	mAnimateAction->setCheckable( true );
	mAnimateAction->setChecked( false );

    mStandardColorAction = new QAction( "Standard Color", this );
	mStandardColorAction->setCheckable( true );
	mStandardColorAction->setChecked( true );

    mHomeAction = new QAction( "Reset", this );

    addAction( mAnimateAction );
	addAction( mStandardColorAction );
    addAction( CActionInfo::CreateAction( this, eAction_Separator) );
    addAction( mHomeAction );
    addAction( CActionInfo::CreateAction( this, eAction_Separator) );

    connect( mAnimateAction,		SIGNAL(toggled(bool)),	this, SLOT( Animate() ) );
    connect( mStandardColorAction,	SIGNAL(toggled(bool)),	this, SLOT( StandardColor(bool) ) );
    connect( mHomeAction,			SIGNAL(triggered()),	this, SLOT( Reset() ) );
#endif

	mStyleGroup = new QActionGroup( this );
	mStyleGroup->setExclusive( true );

#if defined (DEBUG) || defined (_DEBUG)
	create_style_item( "Qwt3D::NOPLOT", "No visible data", Qwt3D::NOPLOT );
	create_style_item( "Qwt3D::WIREFRAME", "Wire-frame style", Qwt3D::WIREFRAME );
	create_style_item( "Qwt3D::HIDDENLINE", "Hidden Line style", Qwt3D::HIDDENLINE );
	create_style_item( "Qwt3D::FILLED", "Color filled polygons w/o edges", Qwt3D::FILLED );
	create_style_item( "Qwt3D::FILLEDMESH", "Color filled polygons w/ separately colored edges", Qwt3D::FILLEDMESH );
#else
	create_style_item( "Color Filled", "Color filled polygons w/o edges", Qwt3D::FILLED );
	create_style_item( "Color Filled Mesh", "Color filled polygons w/ separately colored edges", Qwt3D::FILLEDMESH );
#endif

	mCoordinateStyleGroup = new QActionGroup( this );
	mStyleGroup->setExclusive( true );

	QList<QAction*> coord_style_actions_list;
	auto create_coordinate_style_item = [ this, &coord_style_actions_list ]( const std::string &name, const std::string tip, Qwt3D::COORDSTYLE style )
	{
		auto a = new QAction( name.c_str(), mCoordinateStyleGroup );
		a->setCheckable( true );
		a->setChecked( coordinates()->style() == style );
		a->setToolTip( tip.c_str() );
		a->setData( style );
		coord_style_actions_list << a;
		connect( a, SIGNAL( triggered() ), this, SLOT( CoordinateStyle() ) );
	};
#if defined (DEBUG) || defined (_DEBUG)
	create_coordinate_style_item( "Qwt3D::NOCOORD", "Coordinate system is not visible", Qwt3D::NOCOORD );
	create_coordinate_style_item( "Qwt3D::BOX", "Boxed", Qwt3D::BOX );
	create_coordinate_style_item( "Qwt3D::FRAME", "Frame - 3 visible axes", Qwt3D::FRAME );
#else
	create_coordinate_style_item( "No Coordinates", "Coordinate system is not visible", Qwt3D::NOCOORD );
	create_coordinate_style_item( "Boxed Coordinates", "Boxed", Qwt3D::BOX );
	create_coordinate_style_item( "Frame Coordinates", "Frame - 3 visible axes", Qwt3D::FRAME );
#endif

	addActions( style_actions_list );
    addAction( CActionInfo::CreateAction( this, eAction_Separator) );
	addActions( coord_style_actions_list );
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//						Main Class
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////





C3DPlotWidget::C3DPlotWidget( const CPlotsGraphicParameters &plots_graphic_parameters, QWidget *parent ) 
	: base_t( parent )
	, graphic_parameters_base_t( plots_graphic_parameters )
//	, m_SizeHint( DefaultSizeHint( this ) )
{
	setWindowIcon( QPixmap( ":/3.png" ) );
	setWindowTitle( "[*]" );
	setAttribute( Qt::WA_DeleteOnClose );

	//same as QwtPlot (2D)

    setFrameStyle( QFrame::Panel | QFrame::Sunken );
    setLineWidth(2);
    setMidLineWidth(0);

	QPalette pal( palette() ); 
	pal.setColor( QPalette::Background, ViewsDefaultBackgroundColor() );	//the real effect is done in the plot class
	setAutoFillBackground( true );
	setPalette( pal );
}


C3DPlotWidget::~C3DPlotWidget()
{}


void C3DPlotWidget::PushPlot( const CZFXYPlotParameters &values, Qwt3D::Color *pcolor_map )
{
#if defined (TEST_EXAMPLES)
	return;
#endif

	mCurrentPlot = new CBrat3DPlot( *this, this );
	mSurfacePlots.push_back( mCurrentPlot );
	AddWidget( mCurrentPlot );
	
	connect( mCurrentPlot, SIGNAL( scaleChanged( double, double, double ) ), this, SLOT( HandleScaleChanged( double, double, double ) ) );

	mCurrentPlot->AddSurface( values );

	mCurrentPlot->setDataColor( pcolor_map );

	CorrectScale();
}


void C3DPlotWidget::SetColorMap( QLookupTable *pcolor_map )
{
	assert__( mSurfacePlots.size() );

	CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );

	p->SetColorMap( pcolor_map );
}




void C3DPlotWidget::SetCurrentPlot( int index )
{
	assert__( size_t( index ) <mSurfacePlots.size() );

	mCurrentPlot = dynamic_cast<CBrat3DPlot*>( mSurfacePlots[ index ] );

	mStackedLayout->setCurrentIndex( index );								assert__( mCurrentPlot == mStackedLayout->currentWidget() );
}


//title

void C3DPlotWidget::SetPlotTitle( const std::string &title )
{
	assert__( mSurfacePlots.size() );

	setWindowTitle( title.c_str() );

	const size_t size = mStackedLayout->count();
	for ( size_t i = 0; i < size; ++i )
	{
		auto *plot = mStackedLayout->widget( i );
		CBrat3DPlot *p = dynamic_cast<CBrat3DPlot*>( plot );		assert__( p );
		p->SetTitle( windowTitle() );
	}
}




// persistence (image only)

//static 
bool C3DPlotWidget::Save2ps( CBrat3DPlot *p, const QString &path )	//Supported
{
	QString qpath = path;
	QString f = "PS";
	SetFileExtension( qpath, f );
	return p->saveVector( qpath, f, Qwt3D::VectorWriter::NATIVE, Qwt3D::VectorWriter::NOSORT );
}
//static 
bool C3DPlotWidget::Save2unsupported( CBrat3DPlot *p, const QString &path, const QString &format )
{
	QPixmap pix = QPixmap::grabWidget( p );
	if ( pix.isNull() )
	{
		return false;
	}

	QString qpath = path;
	QString f = format;
	SetFileExtension( qpath, f );
	if ( p->saveVector( qpath, f, Qwt3D::VectorWriter::NATIVE, Qwt3D::VectorWriter::NOSORT ) || 
		p->save( qpath, f ) || 
		pix.save( qpath, f.toStdString().c_str() ) )
		return true;

	f = format.toLower();
	SetFileExtension( qpath, f );
	return  
		p->saveVector( qpath, f, Qwt3D::VectorWriter::NATIVE, Qwt3D::VectorWriter::NOSORT ) || 
		p->save( qpath, f ) || 
		pix.save( qpath, f.toStdString().c_str() );
}
//static 
bool C3DPlotWidget::Save2svg( CBrat3DPlot *p, const QString &path )	//NOT supported
{
	return Save2unsupported( p, path, "SVG" );
}
//static 
bool C3DPlotWidget::Save2gif( CBrat3DPlot *p, const QString &path )
{
	return Save2unsupported( p, path, "GIF" );
}

//static 
void C3DPlotWidget::Save2All( CBrat3DPlot *p, const QString &path )
{
	Save2ps( p, path );		//supported
	Save2svg( p, path );	//NOT supported
	Save2gif( p, path );	//NOT supported

	QString qpath = path;
	QString 
	f = "tif";
	SetFileExtension( qpath, f );	//OK
	p->savePixmap( qpath, f );

	f = "bmp";
	SetFileExtension( qpath, f );	//OK
	p->savePixmap( qpath, f );

	f = "jpg";
	SetFileExtension( qpath, f );	//OK
	p->savePixmap( qpath, f );

	f = "png";
	SetFileExtension( qpath, f );	//OK
	p->savePixmap( qpath, f );

	f = "ppm";						//OK
	SetFileExtension( qpath, QString( "pnm" ) );
	p->save( qpath, f );
}

// Supported (and required) formats:
//
//enum EImageExportType
//{
//	eTif,
//	eBmp,
//	eJpg,
//	ePng,
//	ePnm,
//};
//
bool C3DPlotWidget::Save2Image( const QString &path, const QString &format, const QString &extension )
{
	assert__( mSurfacePlots.size() );

	CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );

//#if defined (_DEBUG) || defined(DEBUG)
//	Save2All( p, path );
//#endif

	QString qpath = path;
	QString f = format.toLower();
	if ( f == "ps" )
	{
#if QT_VERSION < 0x050000
		return Save2ps( p, path );
#else
		QTemporaryFile file;
		if ( file.open() )
		{
			const QString &in_path = file.fileName();
			file.close();
			return
				Save2Image( in_path, "jpg", "" )
				&&
				Convert2PostScript( q2a( in_path ), q2a( path ), q2a( windowTitle() ) );
		}
#endif
	}

	if (!extension.isEmpty() )
		SetFileExtension( qpath, extension );
	return p->savePixmap( qpath, f );
}




//style

void C3DPlotWidget::SetStyle( bool show_contour, bool show_mesh )
{
	assert__( mCurrentPlot );

	Qwt3D::PLOTSTYLE style = Qwt3D::WIREFRAME;

	if ( show_contour )
	{
		if ( show_mesh )
			style = Qwt3D::FILLEDMESH;
		else
			style = Qwt3D::HIDDENLINE;
	}
	else
	{
		if ( show_mesh )
			style = Qwt3D::FILLED;
		else
			style = Qwt3D::NOPLOT;
	}

	mCurrentPlot->SetPlotStyle( style );
}

bool C3DPlotWidget::HasMesh() const
{
	assert__( mCurrentPlot );

	auto style = mCurrentPlot->plotStyle();

	return style == Qwt3D::FILLEDMESH || style == Qwt3D::HIDDENLINE;
}

void C3DPlotWidget::ShowMesh( bool show )
{
	SetStyle( show, HasSolidColor() );
}


bool C3DPlotWidget::HasSolidColor() const
{
	assert__( mCurrentPlot );

	auto style = mCurrentPlot->plotStyle();

	return style == Qwt3D::FILLEDMESH || style == Qwt3D::FILLED;
}

void C3DPlotWidget::ShowSolidColor( bool show )
{
	SetStyle( HasMesh(), show );
}


void C3DPlotWidget::ShowMesh( int index, bool show )
{
	assert__( size_t( index ) <mSurfacePlots.size() );

	mCurrentPlot = dynamic_cast<CBrat3DPlot*>( mSurfacePlots[ index ] );

	ShowMesh( show );
}

void C3DPlotWidget::ShowSolidColor( int index, bool show )
{
	assert__( size_t( index ) <mSurfacePlots.size() );

	mCurrentPlot = dynamic_cast<CBrat3DPlot*>( mSurfacePlots[ index ] );

	ShowSolidColor( show );
}




//axis


//titles

void C3DPlotWidget::CurrentAxisTitles( std::string &xtitle, std::string &ytitle, std::string &ztitle ) const
{
    assert__( mSurfacePlots.size() );

    const CBrat3DPlot *p = dynamic_cast< const CBrat3DPlot* >( mCurrentPlot );		assert__( p );

	p->AxisTitles( xtitle, ytitle, ztitle );
}


bool C3DPlotWidget::SetAllXYAxisTitles( const std::string &xtitle, const std::string &ytitle )
{
    assert__( mSurfacePlots.size() );

#if defined (TEST_EXAMPLES)
	return;
#endif

	for ( auto *plot : mSurfacePlots )
	{
		CBrat3DPlot *p = dynamic_cast<CBrat3DPlot*>( plot );		assert__( p );
		if ( !p->SetXYAxisTitles( xtitle, ytitle ) )
			return false;
	}

	return true;
}

bool C3DPlotWidget::SetZAxisTitle( size_t index, const std::string &ztitle )
{
    assert__( index < mSurfacePlots.size() );

#if defined (TEST_EXAMPLES)
	return;
#endif

    CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mSurfacePlots[index] );		assert__( p );

	return p->SetZAxisTitle( ztitle );
}




// ticks

unsigned int C3DPlotWidget::XAxisTicks() const
{
    assert__( mSurfacePlots.size() );
    const CBrat3DPlot *p = dynamic_cast< const CBrat3DPlot* >( mCurrentPlot );		assert__( p );
    return p->XAxisNbTicks();
}
void C3DPlotWidget::SetXAxisTicks( unsigned int nbticks )
{
    assert__( mSurfacePlots.size() );
    CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );
	p->SetXAxisTicks( nbticks );
}


unsigned int C3DPlotWidget::YAxisTicks() const
{
    assert__( mSurfacePlots.size() );
    const CBrat3DPlot *p = dynamic_cast< const CBrat3DPlot* >( mCurrentPlot );		assert__( p );
    return p->YAxisNbTicks();
}
void C3DPlotWidget::SetYAxisTicks( unsigned int nbticks )
{
    assert__( mSurfacePlots.size() );
    CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );
	p->SetYAxisTicks( nbticks );
}

unsigned int C3DPlotWidget::ZAxisTicks() const
{
    assert__( mSurfacePlots.size() );
    const CBrat3DPlot *p = dynamic_cast< const CBrat3DPlot* >( mCurrentPlot );		assert__( p );
    return p->ZAxisNbTicks();
}
void C3DPlotWidget::SetZAxisTicks( unsigned int nbticks )
{
    assert__( mSurfacePlots.size() );
    CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );
	p->SetZAxisTicks( nbticks );
}




// digits / date

int C3DPlotWidget::XAxisDigits() const
{
    assert__( mSurfacePlots.size() );
    CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );
	return p->XDigits();
}
int C3DPlotWidget::YAxisDigits() const
{
    assert__( mSurfacePlots.size() );
    CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );
	return p->YDigits();
}
int C3DPlotWidget::ZAxisDigits() const
{
    assert__( mSurfacePlots.size() );
    CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );
	return p->ZDigits();
}

bool C3DPlotWidget::XAxisIsDateTime() const
{
    assert__( mSurfacePlots.size() );
    CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );
	return p->XisDateTime();
}
bool C3DPlotWidget::YAxisIsDateTime() const
{
    assert__( mSurfacePlots.size() );
    CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );
	return p->YisDateTime();
}
bool C3DPlotWidget::ZAxisIsDateTime() const
{
    assert__( mSurfacePlots.size() );
    CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );
	return p->ZisDateTime();
}


void C3DPlotWidget::SetXAxisDigits( bool isdate, unsigned int digits, brathl_refDate date_ref )       //date_ref = REF19500101
{
    assert__( mSurfacePlots.size() );
    CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );
	p->SetXDigits( isdate, digits, date_ref );
}

void C3DPlotWidget::SetYAxisDigits( bool isdate, unsigned int digits, brathl_refDate date_ref )       //date_ref = REF19500101
{
    assert__( mSurfacePlots.size() );
    CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );
	p->SetYDigits( isdate, digits, date_ref );
}

void C3DPlotWidget::SetZAxisDigits( bool isdate, unsigned int digits, brathl_refDate date_ref )       //date_ref = REF19500101
{
    assert__( mSurfacePlots.size() );
    CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );
	p->SetZDigits( isdate, digits, date_ref );
}




// scale

void C3DPlotWidget::HandleScaleChanged( double xVal, double yVal, double zVal )
{
	emit ScaleChanged( xVal, yVal, zVal );
}


bool C3DPlotWidget::AxisZLogarithmicScale() const
{
	assert__( mSurfacePlots.size() );

	CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );

	return p->LogarithmicScaleZ();
}

void C3DPlotWidget::SetAxisZLogarithmicScale( bool log )
{
	assert__( mSurfacePlots.size() );

	CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );

	p->SetLogarithmicScaleZ( log );

	CorrectScale();
}



void C3DPlotWidget::Scale( double &xVal, double &yVal, double &zVal )
{
    assert__( mSurfacePlots.size() );
    CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );
    p->Scale( xVal, yVal, zVal );
}
void C3DPlotWidget::SetScale( double xVal, double yVal, double zVal )
{
    assert__( mSurfacePlots.size() );
    CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );
    p->SetScale( xVal, yVal, zVal );
}


void C3DPlotWidget::CorrectScale()
{
	double xMin;
	double xMax;
	double yMin;
	double yMax;
	double zMin;
	double zMax;

	mCurrentPlot->FunctionRanges( xMin, xMax, yMin, yMax, zMin, zMax );

	if ( !isDefaultValue( zMin ) && !isDefaultValue( zMax ) )
	{
		double xrange = xMax - xMin;			assert__( xrange > 0 );
		double yrange = yMax - yMin;			assert__( yrange > 0 );
		double zrange = zMax - zMin;			assert__( zrange >= 0 );	//open an exception to allow zMin == zMax
		if ( zrange == 0. )
			zrange = std::numeric_limits<double>::lowest();

		double range_max = std::max( xrange, std::max( yrange, zrange ) );

		double xscale = range_max / xrange ;
		double yscale = range_max / yrange ;
		double zscale = range_max / zrange ;

		SetScale( xscale, yscale, zscale );
		mCurrentPlot->setViewportShift( 0., 0. );
		mCurrentPlot->setZoom( 0.5 );
	}
}




//interaction

void C3DPlotWidget::Home()
{
	assert__( mSurfacePlots.size() );

	CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );

	p->Reset();

	CorrectScale();
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////


void C3DPlotWidget::AddWidget( QWidget *w )
{
	mStackedLayout = dynamic_cast< QStackedLayout* >( layout() );
	if ( !mStackedLayout )
	{
		mStackedLayout = new QStackedLayout( this );
		setLayout( mStackedLayout );
	}
	mStackedLayout->addWidget( w );
	mStackedLayout->setCurrentWidget( w );
}



QSize C3DPlotWidget::sizeHint() const
{
    return mSizeHint;
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_3DPlotWidget.cpp"
