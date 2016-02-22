#ifndef DATAMODELS_PLOTDATA_PLOT_VALUES_H
#define DATAMODELS_PLOTDATA_PLOT_VALUES_H


#include <qwt_data.h>

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

	// The x values depend on its index and the y values
	// can be calculated from the corresponding x value.
	// So we don't need to store the values.
	// Such an implementation is slower because every point
	// has to be recalculated for every replot, but it demonstrates how
	// QwtData can be used.

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
struct CGeneric3DPlotInfo : public std::vector< PARAMS >
{
	//types

	using parameters_t = PARAMS;

	using base_t = std::vector<parameters_t>;

    using base_t::back;
    using base_t::at;
    using base_t::push_back;



	// construction / destruction

	CGeneric3DPlotInfo()
	{}
	virtual ~CGeneric3DPlotInfo()
	{}


	// operations

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
	}


	inline void AddY( double value )
	{
		back().mYaxis.push_back( value );
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
