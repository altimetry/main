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
	};


	//instance data

	std::vector< CCurve > mFrames;
	mutable size_t mCurrentFrame = 0;

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


	void SetNextFrame() const
	{
		mCurrentFrame++;
		if ( mCurrentFrame >= mFrames.size() )
			mCurrentFrame = 0;
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


protected:
	static double nearest( double raster_x, const std::set<double> &nearest_map, double m, double M )
	{
		auto pair = nearest_map.equal_range( raster_x );		UNUSED( M );

		if ( pair.first == nearest_map.end() )
			return m;

		return *pair.first;
	}
	double nearest_x( double raster_x ) const
	{
		const parameters_t &frame = ( *this )[ mCurrentFrame ];
		return nearest( raster_x, frame.mXmap, frame.mMinX, frame.mMaxX );
	}
	double nearest_y( double raster_y ) const
	{
		const parameters_t &frame = ( *this )[ mCurrentFrame ];
		return nearest( raster_y, frame.mYmap, frame.mMinY, frame.mMaxY  );
	}


public:

	virtual double value( double x, double y ) const override
    {
		const parameters_t &frame = ( *this )[ mCurrentFrame ];

		x = nearest_x( x ) - frame.mMinX;
		y = nearest_y( y ) - frame.mMinY;

		auto index = y * frame.mXaxis.size() + x;										assert__( index >= 0 && index < frame.mBits.size() );
		if ( index < 0 || index >= frame.mBits.size() || !frame.mBits.at( index ) )
			return 0;		//rasters do not seem to support NANs std::numeric_limits<double>::quiet_NaN();

		return frame.mValues.at( index );
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
