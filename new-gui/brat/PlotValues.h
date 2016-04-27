#ifndef DATAMODELS_PLOTDATA_PLOT_VALUES_H
#define DATAMODELS_PLOTDATA_PLOT_VALUES_H

#include <set>

#include <qwt_data.h>
#include <qwt_raster_data.h>

#include "new-gui/Common/tools/CoreTypes.h"

#if defined (min)
#undef min
#endif
#if defined (max)
#undef max
#endif



static_assert( std::numeric_limits<double>::has_quiet_NaN, "Value nan of type double is undefined in the system." );
static_assert( std::numeric_limits<float>::has_quiet_NaN, "Value nan of type float is undefined in the system." );


class CQwtArrayPlotData : public QwtData
{
	//types

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
			return GetDataCountIf( 0, mSizeY, f ) ;
		}
	};


	//instance data

	std::vector< CCurve > mFrames;
	mutable int mCurrentFrame = 0;

    double mMinXValue = std::numeric_limits<double>::max();
    double mMaxXValue = std::numeric_limits<double>::lowest();

    double mMinYValue = std::numeric_limits<double>::max();
    double mMaxYValue = std::numeric_limits<double>::lowest();


    //construction / destruction

public:
	CQwtArrayPlotData()
	{}

	CQwtArrayPlotData( const CQwtArrayPlotData &o )
	{
		*this = o;
	}


	CQwtArrayPlotData& operator = ( const CQwtArrayPlotData &o )
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
	virtual ~CQwtArrayPlotData()
	{
		Reset();
	}


	// access

	int CurrentFrame() const { return mCurrentFrame; }


	// Qwt interface

	virtual QwtData *copy() const override
	{
		return new CQwtArrayPlotData( *this );
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
				c.mX[ i ] = std::numeric_limits< double >::quiet_NaN();		//needed for our QWT partial functions plotting algorithm
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
				c.mY[ i ] = std::numeric_limits< double >::quiet_NaN();		//needed for our QWT partial functions plotting algorithm
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


	size_t GetDataSize( size_t iframe ) const
	{
		assert__( iframe < mFrames.size() );

		auto const &frame = mFrames[ iframe ];			assert__( frame.mSizeX == frame.mSizeY );

		return frame.mSizeY;
    }



	void GetXRange( double& min, double& max, size_t iframe ) const
	{
        // is our frame within our bounds?
		assert__( iframe < mFrames.size() );

        // gets the associated frame
		auto const &frame = mFrames[ iframe ];

		frame.GetXRange( min, max );
	}


	void GetYRange( double& min, double& max, size_t iframe ) const
	{
		assert__( iframe < mFrames.size() );

		auto const &frame = mFrames[ iframe ];

		frame.GetYRange( min, max );
    }


	void GetXRange( double& min, double& max ) const
	{
		min = mMinXValue;
		max = mMaxXValue;
	}


	void GetYRange( double& min, double& max ) const
	{
		min = mMinYValue;
		max = mMaxYValue;
	}


	size_t GetNumberOfFrames() const
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






struct C3DPlotParameters
{
	std::vector<double>	mValues;
	std::vector<bool>	mBits;
	std::vector<double>	mXaxis;
	std::vector<double>	mYaxis;

	std::set< double > mXmap;
	std::set< double > mYmap;

	int mPlotWidth = 0;
	int mPlotHeight = 0;

	double mMinX = 0.;
	double mMaxX = 0.;

	double mMinY = 0.;
	double mMaxY = 0.;

	double mMinHeightValue = 0.;
	double mMaxHeightValue = 0.;


protected:
	static size_t nearest( double raster_x, const std::set<double> &nearest_map, const std::vector<double> &axis )
	{
		auto pair = nearest_map.equal_range( raster_x );

		if ( pair.first == nearest_map.end() )
			//return m;
			return 0;

		size_t index = std::distance( nearest_map.begin(), pair.first );
		if ( index == 0 )
			return index;

		if ( *pair.first - raster_x < raster_x - axis[ index - 1 ] )
			return index;

		return index - 1;
	}
	size_t nearest_x( double raster_x ) const
	{
		return nearest( raster_x, mXmap, mXaxis );
	}
	size_t nearest_y( double raster_y ) const
	{
		return nearest( raster_y, mYmap, mYaxis );
	}

public:

	double value( double x, double y ) const
	{
		x = nearest_x( x );
		y = nearest_y( y );

		auto index = y * mXaxis.size() + x;								assert__( index >= 0 && index < mBits.size() );
		if ( index < 0 || index >= mBits.size() || !mBits.at( index ) )
			return 0;		//rasters do not seem to support NANs std::numeric_limits<double>::quiet_NaN();

		return mValues.at( index );
	}

	// NOTE: "to" is one after
	//
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





struct CWorldPlotParameters : public C3DPlotParameters
{
	std::vector< bool >	mValidMercatorLatitudes;
	std::vector<double>	mValuesEast;				//for velocity maps

	double mLongitudeOffset = 0.;
	double mLatitudeOffset = 0.;
};



template< class PARAMS >
struct CGeneric3DPlotInfo : public std::vector< PARAMS >, public QwtRasterData
{
	//types

	using parameters_t = PARAMS;

	using base_t = std::vector<parameters_t>;
    using qwt_base_t = QwtRasterData;

    using base_t::back;
    using base_t::at;
    using base_t::push_back;
    using base_t::size;


	//instance data

	mutable size_t mCurrentFrame = 0;


	// construction / destruction

	CGeneric3DPlotInfo() 
		: base_t()
        , qwt_base_t()
	{}
	virtual ~CGeneric3DPlotInfo()
	{}


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
        return new CGeneric3DPlotInfo< parameters_t >( *this );
    }

    virtual QwtDoubleInterval range() const override
    {
		const parameters_t &frame = ( *this )[ mCurrentFrame ];
		return QwtDoubleInterval( frame.mMinHeightValue, frame.mMaxHeightValue );
    }


	virtual double value( double x, double y ) const override
    {
		const parameters_t &frame = ( *this )[ mCurrentFrame ];
        return frame.value( x, y );
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
		push_back(parameters_t());
	}


	inline void AddValue( double value )
	{
		back().mValues.push_back( value );
	}


	void AddBit( bool bit )
	{
		back().mBits.push_back( bit );
	}


	inline void AddX( double value )
	{
		back().mXaxis.push_back( value );
		back().mXmap.insert( value );
	}


	inline void AddY( double value )
	{
		back().mYaxis.push_back( value );
		back().mYmap.insert( value );
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




struct CWorldPlotInfo : CGeneric3DPlotInfo< CWorldPlotParameters >
{
	//types

	using parameters_t = CWorldPlotParameters;

	using base_t = CGeneric3DPlotInfo< parameters_t >;


	// construction / destruction

	CWorldPlotInfo() 
		: base_t()
	{}
	virtual ~CWorldPlotInfo()
	{}


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




using C3DPlotInfo = CGeneric3DPlotInfo< C3DPlotParameters >;





#endif			// DATAMODELS_PLOTDATA_PLOT_VALUES_H
