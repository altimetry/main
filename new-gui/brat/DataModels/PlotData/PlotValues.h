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
#ifndef DATAMODELS_PLOTDATA_PLOT_VALUES_H
#define DATAMODELS_PLOTDATA_PLOT_VALUES_H

#include <set>
#include <functional>

#include <qwt_data.h>
#include <qwt_raster_data.h>

#include "common/tools/CoreTypes.h"

#if defined (min)
#undef min
#endif
#if defined (max)
#undef max
#endif



static_assert( std::numeric_limits<double>::has_quiet_NaN, "Value nan of type double is undefined in the system." );
static_assert( std::numeric_limits<float>::has_quiet_NaN, "Value nan of type float is undefined in the system." );


template< typename FLOATING_POINT >
inline const FLOATING_POINT& Invalid()
{
	static const FLOATING_POINT invalid = std::numeric_limits< FLOATING_POINT >::quiet_NaN();
	return invalid;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//													2D
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//									Frame Container & Frame (nested)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


class CYFXValues : public QwtData
{
	//types

	friend class CQwtFitData;


	////////////////////////////
	//			Frame
	////////////////////////////


	struct CCurve
	{
		//instance data

		double *mX = nullptr;
		double *mY = nullptr;

		size_t mSizeX = 0;
		size_t mSizeY = 0;

        double mMinX = std::numeric_limits<double>::max();
        double mMaxX = std::numeric_limits<double>::lowest();

        double mMinY = std::numeric_limits<double>::max();
        double mMaxY = std::numeric_limits<double>::lowest();

		mutable double mIntercept_a = defaultValue<double>();
		mutable double mSlope_b = defaultValue<double>();


        //construction / destruction

		static CCurve Copy( const CCurve &o )
		{
			CCurve self;

			self.mX = new double[ o.mSizeX ];
			memcpy( self.mX, o.mX, o.mSizeX * sizeof( *o.mX ) );
			self.mSizeX = o.mSizeX;

			self.mY = new double[ o.mSizeY ];
			memcpy( self.mY, o.mY, o.mSizeY * sizeof( *o.mY ) );
			self.mSizeY = o.mSizeY;

			return self;
		}

	protected:
		CCurve() = default;

	public:
		CCurve( double *x, double *y, size_t size_x, size_t size_y ) :
			mX( x ), mY( y ), mSizeX( size_x ), mSizeY( size_y )
		{
			assert__( mX && mY );
		}

		~CCurve()
		{}

		void Reset()
		{
			delete[] mX;
			delete[] mY;
			mX = nullptr;
			mY = nullptr;
			mSizeX = 0;
			mSizeY = 0;
		}


		// access

		void GetXRange( double& min, double& max ) const
		{
			min = mMinX;
			max = mMaxX;
		}


		void GetYRange( double& min, double& max ) const
		{
			min = mMinY;
			max = mMaxY;
		}


		// NOTE: "to" is one after
		//
		template< class F >
		size_t GetDataCountIf( size_t from, size_t to, const F &f ) const
		{
			return std::count_if( &mY[ from ], &mY[ to ], f );
		}


		template< class F >
        size_t GetDataCountIf( const F &f ) const
		{
			return GetDataCountIf( 0, mSizeY, f );
		}

		//
		bool Fit() const;
	};


public:

	using CYFXPlotParameters = CCurve;


protected:
	//instance data

	std::vector< CCurve > mFrames;
	mutable int mCurrentFrame = 0;

    double mMinXValue = std::numeric_limits<double>::max();
    double mMaxXValue = std::numeric_limits<double>::lowest();

    double mMinYValue = std::numeric_limits<double>::max();
    double mMaxYValue = std::numeric_limits<double>::lowest();


    //construction / destruction

public:
	CYFXValues()
	{}

	CYFXValues( const CYFXValues &o )
	{
		*this = o;
	}


	CYFXValues& operator = ( const CYFXValues &o )
	{
		Reset();
		if ( this != &o )
		{
			for ( auto &curve : o.mFrames  )
				mFrames.push_back( CCurve::Copy( curve ) );

			mCurrentFrame = o.mCurrentFrame;

			mMinXValue = o.mMinXValue;
			mMaxXValue = o.mMaxXValue;

			mMinYValue = o.mMinYValue;
			mMaxYValue = o.mMaxYValue;
		}
		return *this;
	}

protected:
    void Reset()
	{
		for ( auto &frame : mFrames )
			frame.Reset();
	}

public:
	virtual ~CYFXValues()
	{
		Reset();
	}


	// access

	int CurrentFrame() const { return mCurrentFrame; }


	// Qwt interface

	virtual QwtData *copy() const override
	{
		return new CYFXValues( *this );
	}


	virtual size_t size() const override
	{
		assert__( mFrames[ mCurrentFrame ].mSizeX == mFrames[ mCurrentFrame ].mSizeY );

		return mFrames[ mCurrentFrame ].mSizeX;
	}


	virtual double x( size_t i ) const override
	{
		assert__( i < mFrames[ mCurrentFrame ].mSizeX );

		return mFrames[ mCurrentFrame ].mX[ i ];
	}


	virtual double y( size_t i ) const override
	{
		assert__( i < mFrames[ mCurrentFrame ].mSizeY );

		return mFrames[ mCurrentFrame ].mY[ i ];
	}


	// assignment


	void SetNextFrame( bool forward ) const
	{
		int incr = forward ? 1 : -1;
		mCurrentFrame += incr;

		if ( mCurrentFrame >= (int)mFrames.size() )
			mCurrentFrame = 0;

		if ( mCurrentFrame < 0 )
            mCurrentFrame = (int)mFrames.size() - 1;
	}


    void SetFrame( size_t iframe ) const
	{
		assert__( iframe < mFrames.size() );

        mCurrentFrame = (int)iframe;
	}


	void AddFrameData( double *x_, size_t sizex_, double *y_, size_t sizey_ )
	{
        mFrames.push_back( CCurve( x_, y_, sizex_, sizey_ ) );

        //compute curve range, working directly over the recently inserted CCurve object

		CCurve &c = mFrames.back();

		for ( size_t i = 0; i < c.mSizeX; ++i )
        {
			if ( isDefaultValue( c.mX[ i ] ) )
			{
				c.mX[ i ] = Invalid< double >();		//needed for our QWT partial functions plotting algorithm
				continue;
			}

            if ( c.mX[i] < c.mMinX )
            {
                c.mMinX = c.mX[i];
            }
            if ( c.mX[i] > c.mMaxX )
            {
                c.mMaxX = c.mX[i];
            }
        }

		for ( size_t i = 0; i < c.mSizeY; ++i )
		{
			if ( isDefaultValue( c.mY[ i ] ) )
			{
				c.mY[ i ] = Invalid< double >();		//needed for our QWT partial functions plotting algorithm
				continue;
			}

            if ( c.mY[ i ] < c.mMinY )
            {
                c.mMinY = c.mY[ i ];
            }
            if ( c.mY[ i ] > c.mMaxY )
            {
                c.mMaxY = c.mY[ i ];
            }
        }

        //compute overall range

        if ( c.mMaxX > mMaxXValue )
        {
            mMaxXValue = c.mMaxX;
        }

        if ( c.mMinX < mMinXValue )
        {
            mMinXValue = c.mMinX;
        }

        if ( c.mMaxY > mMaxYValue )
        {
            mMaxYValue = c.mMaxY;
        }

        if ( c.mMinY < mMinYValue )
        {
            mMinYValue = c.mMinY;
        }

    }



	//access


	template< class F >
	size_t GetDataCountIf( size_t from, size_t to, size_t iframe, const F &f ) const
	{
		assert__( iframe < mFrames.size() );

		auto const &frame = mFrames[ iframe ];

		return frame.GetDataCountIf( from, to, f ) ;
    }


	template< class F >
	size_t GetDataCountIf( size_t iframe, const F &f ) const
	{
		assert__( iframe < mFrames.size() );

		auto const &frame = mFrames[ iframe ];

		return frame.GetDataCountIf( f ) ;
    }


	void GetDataRange( double& min, double& max, size_t iframe ) const
	{
		return GetYRange( min, max, iframe );
    }
	void GetDataRange( double& min, double& max ) const
	{
		return GetYRange( min, max );
    }


	size_t GetDataSize( size_t iframe ) const
	{
		assert__( iframe < mFrames.size() );

		auto const &frame = mFrames[ iframe ];			assert__( frame.mSizeX == frame.mSizeY );

		return frame.mSizeY;
    }



	virtual void GetXRange( double& min, double& max, size_t iframe ) const
	{
		assert__( iframe < mFrames.size() );

		auto const &frame = mFrames[ iframe ];

		frame.GetXRange( min, max );
	}


	virtual void GetYRange( double& min, double& max, size_t iframe ) const
	{
		assert__( iframe < mFrames.size() );

		auto const &frame = mFrames[ iframe ];

		frame.GetYRange( min, max );
    }


	virtual void GetXRange( double& min, double& max ) const
	{
		min = mMinXValue;
		max = mMaxXValue;
	}


	virtual void GetYRange( double& min, double& max ) const
	{
		min = mMinYValue;
		max = mMaxYValue;
	}


	virtual size_t GetNumberOfFrames() const
	{
		return mFrames.size();
	}


#if defined(BRAT_V3)
	void GetFrameData( size_t iframe, const double *&x, size_t &sizex, const double *&y, size_t &sizey ) const
	{
		assert__( iframe < mFrames.size() );

		auto const &frame = mFrames[ iframe ];

		x = frame.mX;
		sizex = frame.mSizeX;
		y = frame.mY;
		sizey = frame.mSizeY;
	}
#endif
};




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//													Linear Fit
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


class CQwtFitData : public QwtData
{
	using base_t = QwtData;

	const CYFXValues &mData;

public:
	CQwtFitData( const CYFXValues &data )
		: base_t()
		, mData( data )
	{}

	CQwtFitData( CQwtFitData &o )
		: base_t( o )
		, mData( o.mData )
	{}


	//access 

	double slope() const
	{
		auto &curve = mData.mFrames[ mData.mCurrentFrame ];

		if ( !curve.Fit() )
			return Invalid< double >();		//needed for our QWT partial functions plotting algorithm

		return curve.mSlope_b;
	}

	double yintersect() const
	{
		auto &curve = mData.mFrames[ mData.mCurrentFrame ];

		if ( !curve.Fit() )
			return Invalid< double >();		//needed for our QWT partial functions plotting algorithm

		return curve.mIntercept_a;
	}

	bool viable()
	{
		auto &curve = mData.mFrames[ mData.mCurrentFrame ];

		return curve.Fit();
	}




	// Qwt interface

	virtual QwtData *copy() const override
	{
		return new CQwtFitData( mData );
	}


	virtual size_t size() const override
	{
		assert__( mData.mFrames[ mData.mCurrentFrame ].mSizeX == mData.mFrames[ mData.mCurrentFrame ].mSizeY );

		return mData.mFrames[ mData.mCurrentFrame ].mSizeX;
	}


	virtual double x( size_t i ) const override
	{
		assert__( i < mData.mFrames[ mData.mCurrentFrame ].mSizeX );

		return mData.mFrames[ mData.mCurrentFrame ].mX[ i ];
	}


	virtual double y( size_t i ) const override
	{
		assert__( i < mData.mFrames[ mData.mCurrentFrame ].mSizeY );

		auto &curve = mData.mFrames[ mData.mCurrentFrame ];

		if ( !curve.Fit() )
			return Invalid< double >();		//needed for our QWT partial functions plotting algorithm


		return curve.mX[ i ] * curve.mSlope_b + curve.mIntercept_a;
	}
};





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//													3D
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//													Frames
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct CGeneric3DlotParameters
{
	// instance data 

	std::vector< double>	mValues;
	std::vector< bool >		mBits;
	std::vector< double >	mXaxis;
	std::vector< double >	mYaxis;

	int mPlotWidth = 0;
	int mPlotHeight = 0;

	double mMinX = 0.;
	double mMaxX = 0.;

	double mMinY = 0.;
	double mMaxY = 0.;

	double mMinHeightValue = 0.;
	double mMaxHeightValue = 0.;

protected:
	mutable bool mOrdered = false;

	// In fact, ordered axis, such as these and std::set members in ZFXY derived 
	//	class, are redundant for ZFXY (NOT for maps!) because x and y data seem to 
	//	always come ordered, but at least serve to compare the efficiency of set and 
	//	vector containers std::distance

	mutable std::vector< double > mOrderedXaxis;
	mutable std::vector< double > mOrderedYaxis;

public:

	// axis assignment

	inline virtual void AddX( double value )
	{
		mXaxis.push_back( value );
	}


	inline virtual void AddY( double value )
	{
		mYaxis.push_back( value );
	}


	// nearest neighbor

protected:

	// ER
	//	- equal range iterator abstracting set and vector
	//	- used for computing index of value through std::distance

	template< class IT >
	static inline std::pair< IT, IT > ER( const std::set< double > &c, double v )
	{
		return c.equal_range( v );
	}

	template< class IT >
	static inline std::pair< IT, IT > ER( const std::vector< double > &c, double v  )
	{
		return std::equal_range( c.begin(), c.end(), v );
	}


	// The parameter "axis" exists only for ORDERED_CONTAINERs that do not define []; otherwise the "nearest_map" and
	// "axis" parameters can be used with the same argument. If different, it is critical that both containers have 
	//	the same elements, equally ordered
	//
	template< class ORDERED_CONTAINER, class AXIS >
	static size_t ComputeNearestIndex( double raster_v, const ORDERED_CONTAINER &nearest_map, const AXIS &axis )
	{
		auto pair = ER< typename ORDERED_CONTAINER::const_iterator >( nearest_map, raster_v );

		if ( pair.first == nearest_map.end() )			//return m;
			return 0;

		size_t index = std::distance( nearest_map.begin(), pair.first );
		if ( index == 0 )
			return index;

		if ( *pair.first - raster_v < raster_v - axis[ index - 1 ] )
			return index;

		return index - 1;
	}


	template< typename ORDERED_AXIS, typename AXIS >
	inline double compute_value( double ix, double iy, const double DEFAULT, const ORDERED_AXIS &oaxis_x, const ORDERED_AXIS &oaxis_y,
		const AXIS &axis_x, const AXIS &axis_y ) const
	{
		ix = ComputeNearestIndex( ix, oaxis_x, axis_x );
		iy = ComputeNearestIndex( iy, oaxis_y, axis_y );

		auto index = iy * mXaxis.size() + ix;							assert__( index >= 0 && index < mBits.size() );
		if ( index < 0 || index >= mBits.size() || !mBits.at( index ) )
			return DEFAULT;

		return mValues.at( index );
	}

public:

	// The "_v" suffix means: using equal_range iterator with std::vector for std::distance (index computation)
	//
	//	In the map case the ordered axis can have different values of the respective raw axis, and this leads
	//	to undetected errors, such as the nearest neighbor being always one before/after the correct index, 
	//	because the interval to compute the nearest value uses one value of each vector
	//
	// QWT and qwtplot3d support NANs, which should be used (std::numeric_limits<double>::quiet_NaN()) for absent values
	// Map contours do not support NANs
	//
	inline double value_v( double x, double y, double invalid = Invalid< double >() ) const
	{
		assert__( mOrdered );

		return compute_value( x, y, invalid, mOrderedXaxis, mOrderedYaxis, mOrderedXaxis, mOrderedYaxis );
	}


protected:

	inline bool CheckOrderedAxis( const std::vector< double > &ordered_axis, const std::vector< double > &unordered_axis ) const
	{
#if defined (DEBUG) || defined(_DEBUG)

		if ( ordered_axis.size() != unordered_axis.size() )
			return false;

		double m = std::numeric_limits< double >::lowest();
		for ( auto const &v : ordered_axis )
		{
			if ( v < m )
			{
				mOrdered = false;
				break;
			}
			m = v;
		}

#else
		UNUSED( ordered_axis );        UNUSED( unordered_axis );
#endif

		return true;
	}


public:

	virtual bool OrderAxes() = 0;


	// diagnostics

	// NOTE: "to" is one after
	
	template< class F >
	size_t GetDataCountIf( size_t from, size_t to, const F &f ) const
	{
		return std::count_if( mValues.begin() + from, mValues.begin() + to, f );
	}


	template< class F >
	size_t GetDataCountIf( const F &f ) const
	{
        return GetDataCountIf( 0, mValues.size(), f );
	}
};




struct CZFXYPlotParameters : public CGeneric3DlotParameters
{
	//types

	using base_t = CGeneric3DlotParameters;


	//instance data

protected:

	std::set< double > mXmap;
	std::set< double > mYmap;


public:

	//axis assignment

	inline virtual void AddX( double value ) override
	{
		base_t::AddX( value );
		mXmap.insert( value );
	}


	inline virtual void AddY( double value ) override
	{
		base_t::AddY( value );
		mYmap.insert( value );
	}


	// nearest neighbor


	// The "_s" suffix means: using std::set iterator for std::distance (index computation)
	// This is equivalent to base class value_v, and could be used instead. But here we test 
	//	std::set behavior
	//
	// qwtplot3d supports NANs, which should be used for absent values
	//
	inline double value_s( double x, double y, double invalid = Invalid< double >() ) const
	{
		return compute_value( x, y, invalid, mXmap, mYmap, mXaxis, mYaxis );
	}


protected:

	inline bool OrderAxis( const std::set< double > &ordered_set, std::vector< double > &ordered_axis, const std::vector< double > &axis ) const
	{
		// Filling the ordered vectors is mandatory to support the inherited functions value_v
		//
		for ( auto const &v : ordered_set )
			ordered_axis.push_back( v );

		return CheckOrderedAxis( ordered_axis, axis ) && ( ordered_axis == axis );
	}


public:

	inline virtual bool OrderAxes() override
	{
		if ( !mOrdered )
			mOrdered = 
			OrderAxis( mXmap, mOrderedXaxis, mXaxis ) && 
			OrderAxis( mYmap, mOrderedYaxis, mYaxis );

		return mOrdered;
	}
};





struct CMapPlotParameters : public CGeneric3DlotParameters
{
	//types

	using base_t = CGeneric3DlotParameters;

	//instance data

	std::vector< bool >	mValidMercatorLatitudes;
	std::vector<double>	mValuesEast;			//for velocity maps

	double mLongitudeOffset = 0.;
	double mLatitudeOffset = 0.;

private:
	double mMinX360 = 0.;
	double mMaxX360 = 0.;

	//axis assignment

	//	hide AddX

	using base_t::AddX;

public:

	inline void AddLongitude( double raw, double normalized )
	{
		base_t::AddX( normalized );
		mOrderedXaxis.push_back( raw );
	}

	inline virtual void AddY( double value ) override
	{
		base_t::AddY( value );
		mOrderedYaxis.push_back( value );
	}


	bool IsValidPoint( size_t index ) const
	{
		bool bOk = mBits[ index ];

		//	  if (Projection == VTK_PROJ2D_MERCATOR)
		//	  {
        //bOk = bOk && maps(0).mValidMercatorLatitudes[ i ];
		//	  }
		//
		return bOk;
	};


	double LonAt( size_t index ) const
	{
		assert__( index < mValues.size() );

		auto x = index % mXaxis.size();			assert__( x < mXaxis.size() );
		return mXaxis.at( x );
	}


	double LatAt( size_t index ) const
	{
		assert__( index < mValues.size() );

		auto y = index / mXaxis.size();			assert__( y < mYaxis.size() );
		return mYaxis.at( y );
	}


	double ValueAt( size_t index ) const
	{
		assert__( index < mValues.size() );

		return mValues[ index ] ;
	}


	double EastValueAt( size_t index ) const
	{
		assert__( index < mValuesEast.size() );

		return mValuesEast[ index ] ;
	}


	// nearest neighbor

	double MinX360() const
	{
		return mMinX360;
	}

	double MaxX360() const
	{
		return mMaxX360;
	}

public:

	inline virtual bool OrderAxes() override
	{
		if ( !mOrdered )
		{
			mOrdered =
				CheckOrderedAxis( mOrderedXaxis, mXaxis ) &&
				CheckOrderedAxis( mOrderedYaxis, mYaxis );

			mMinX360 = mOrderedXaxis.front();
			mMaxX360 = mOrderedXaxis.back();

			mMinX = mXaxis.front();
			mMaxX = mXaxis.back();

			mMinY = mYaxis.front();
			mMaxY = mYaxis.back();
		}

		return mOrdered;
	}
};





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//												Frame Containers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


template< class PARAMS >
class CGenericZFXYValues : public std::vector< PARAMS >, public QwtRasterData
{
	//types

public:

	using parameters_t = PARAMS;

	using base_t = std::vector<parameters_t>;
    using qwt_base_t = QwtRasterData;

    using base_t::back;
    using base_t::at;
    using base_t::push_back;
    using base_t::size;


	//instance data

protected:

	mutable size_t mCurrentFrame = 0;


	// construction / destruction

public:
	CGenericZFXYValues() 
		: base_t()
        , qwt_base_t()
	{}
	virtual ~CGenericZFXYValues()
	{}

	// access

	virtual size_t CurrentFrame() const { return mCurrentFrame; }


	void GetXRange( double &min, double &max, size_t iframe ) const
	{
		assert__( iframe < size() );	//v3 if ( frame >= size() )	{	frame = size() - 1;	}

        auto const &frame = at( iframe );

		min = frame.mMinX;
		max = frame.mMaxX;
	}

	void GetXRange( double& min, double& max ) const { GetXRange( min, max, mCurrentFrame ) ; }


	void GetYRange( double &min, double &max, size_t iframe ) const
	{
		assert__( iframe < size() );	// v3 if ( frame >= size() ){	frame = size() - 1;	}

        auto const &frame = at( iframe );

		min = frame.mMinY;
		max = frame.mMaxY;
	}

	void GetYRange( double& min, double& max ) const { GetYRange( min, max, mCurrentFrame ) ; }


	// used at least for diagnostics

	template< class F >
	size_t GetDataCountIf( size_t iframe, const F &f ) const
	{
        assert__( iframe < size() );

        auto const &frame = at( iframe );

		return frame.GetDataCountIf( f ) ;
    }


	void GetDataRange( double& min, double& max, size_t iframe ) const
	{
        assert__( iframe < size() );

        auto const &frame = at( iframe );

		min = frame.mMinHeightValue;
		max = frame.mMaxHeightValue;
    }


	void GetDataRange( double& min, double& max ) const
	{
		return GetDataRange( min, max, mCurrentFrame );
    }


	size_t GetDataSize( size_t iframe ) const
	{
        assert__( iframe < size() );

        auto const &frame = at( iframe );

		return frame.mValues.size();
    }



	//QwtRasterData interface

    virtual QwtRasterData *copy() const override
    {
        return new CGenericZFXYValues< parameters_t >( *this );
    }

    virtual QwtDoubleInterval range() const override
    {
		const parameters_t &frame = ( *this )[ mCurrentFrame ];
		return QwtDoubleInterval( frame.mMinHeightValue, frame.mMaxHeightValue );
    }


	virtual double value( double x, double y ) const override
    {
		const parameters_t &frame = ( *this )[ mCurrentFrame ];
        return frame.value_v( x, y );
    }


	// assignment


	void SetNextFrame() const
	{
		mCurrentFrame++;
		if ( mCurrentFrame >= size() )
			mCurrentFrame = 0;
	}


	inline void AddMap()
	{
		push_back( parameters_t() );
	}


	inline void AddValue( double value )
	{
		back().mValues.push_back( value );
	}


	void AddBit( bool bit )
	{
		back().mBits.push_back( bit );
	}


	inline virtual void AddY( double value )
	{
		back().AddY( value );
	}


	parameters_t& operator()()
	{
		return back();
	}

	const parameters_t& operator()( size_t i ) const
	{
		return at( i );
	}
};






struct CZFXYValues : CGenericZFXYValues< CZFXYPlotParameters >
{
	//types

	using parameters_t = CZFXYPlotParameters;

	using base_t = CGenericZFXYValues< parameters_t >;


	// construction / destruction

	CZFXYValues() 
		: base_t()
	{}
	virtual ~CZFXYValues()
	{}


	inline virtual void AddX( double value )
	{
		back().AddX( value );
	}
};






struct CMapValues : CGenericZFXYValues< CMapPlotParameters >
{
	//types

	using parameters_t = CMapPlotParameters;

	using base_t = CGenericZFXYValues< parameters_t >;


	// construction / destruction

	CMapValues() 
		: base_t()
	{}
	virtual ~CMapValues()
	{}



	inline void AddLongitude( double raw, double normalized )
	{
		back().AddLongitude( raw, normalized );
	}


	// operations

	inline void AddmValidMercatorLatitude( bool value )
	{
		back().mValidMercatorLatitudes.push_back( value );
	}


	inline void AddEastValue( double value )
	{
		back().mValuesEast.push_back( value );
	}
};



using CYFXPlotParameters = CYFXValues::CYFXPlotParameters;



#endif			// DATAMODELS_PLOTDATA_PLOT_VALUES_H
