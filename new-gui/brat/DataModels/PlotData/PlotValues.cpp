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

#include "PlotValues.h"

//#include <algorithm>
//#include <iostream>
#include <numeric>
//#include <vector>

double slope( const std::vector<double>& x, const std::vector<double>& y )
{
	const auto n    = x.size();
	const auto s_x  = std::accumulate( x.begin(), x.end(), 0.0 );
	const auto s_y  = std::accumulate( y.begin(), y.end(), 0.0 );
	const auto s_xx = std::inner_product( x.begin(), x.end(), x.begin(), 0.0 );
	const auto s_xy = std::inner_product( x.begin(), x.end(), y.begin(), 0.0 );
	const auto a    = ( n * s_xy - s_x * s_y ) / ( n * s_xx - s_x * s_x );
	return a;
}


struct Point
{
	double _x, _y;
};

struct Line
{
	double _slope, _yInt;
	double getYforX( double x ) {
		return _slope*x + _yInt;
	}
	// Construct line from points
	bool fitPoints( const std::vector<Point> &pts )
	{
		size_t nPoints = pts.size();
		if ( nPoints < 2 ) {
			// Fail: infinitely many lines passing through this single point
			return false;
		}
		double sumX=0, sumY=0, sumXY=0, sumX2=0;
		for ( size_t i=0; i < nPoints; i++ ) {
			sumX += pts[ i ]._x;
			sumY += pts[ i ]._y;
			sumXY += pts[ i ]._x * pts[ i ]._y;
			sumX2 += pts[ i ]._x * pts[ i ]._x;
		}
		double xMean = sumX / nPoints;
		double yMean = sumY / nPoints;
		double denominator = sumX2 - sumX * xMean;
		// You can tune the eps (1e-7) below for your specific task
		if ( std::fabs( denominator ) < 1e-7 ) {
			// Fail: it seems a vertical line
			return false;
		}
		_slope = ( sumXY - sumX * yMean ) / denominator;
		_yInt = yMean - _slope * xMean;
		return true;
	}
};



struct StdFitab 
{
	template<class T>
	static inline T SQR(const T a) {return a*a;}

	const size_t ndata;
	double intercept_a, slope_b, siga, sigb, chi2, sigdat;
	const std::vector<double> &x, &y;

	StdFitab( const std::vector<double> &xx, const std::vector<double> &yy )
		: ndata( xx.size() ), x( xx ), y( yy ), chi2( 0. ), sigdat( 0. ) 
	{
		double sx=0., sy=0., st2=0., t;
		slope_b = 0.0;
		for ( size_t i=0; i < ndata; i++ )
		{
			sx += x[ i ];
			sy += y[ i ];
		}
		const double ss = ndata;
		const double sxoss = sx / ss;
		for ( size_t i=0; i < ndata; i++ ) 
		{
			t=x[ i ] - sxoss;
			st2 += t*t;
			slope_b += t*y[ i ];
		}
		slope_b /= st2;
		intercept_a = ( sy - sx * slope_b ) / ss;
		siga=sqrt( ( 1.0 + sx*sx / ( ss*st2 ) ) / ss );
		sigb=sqrt( 1.0 / st2 );
		for ( size_t i=0; i < ndata; i++ ) 
			chi2 += SQR( y[ i ] - intercept_a - slope_b * x[ i ] );
		if ( ndata > 2 ) 
			sigdat=sqrt( chi2 / ( ndata - 2 ) );
		siga *= sigdat;
		sigb *= sigdat;
	}
};


struct Fitab 
{
	template<class T>
	static inline T SQR(const T a) {return a*a;}

	double intercept_a, slope_b, siga, sigb, chi2, sigdat;
	const double *x = nullptr;
	const double *y = nullptr;

	Fitab( const size_t ndata, const double *xx, const double *yy )
		: x( xx ), y( yy ), chi2( 0. ), sigdat( 0. ) 
	{
		double sx=0., sy=0., st2=0., t;
		slope_b = 0.0;
		for ( size_t i=0; i < ndata; i++ )
		{
			sx += x[ i ];
			sy += y[ i ];
		}
		const double ss = ndata;
		const double sxoss = sx / ss;
		for ( size_t i=0; i < ndata; i++ ) 
		{
			t=x[ i ] - sxoss;
			st2 += t*t;
			slope_b += t*y[ i ];
		}
		slope_b /= st2;
		intercept_a = ( sy - sx * slope_b ) / ss;
		siga=sqrt( ( 1.0 + sx*sx / ( ss*st2 ) ) / ss );
		sigb=sqrt( 1.0 / st2 );
		for ( size_t i=0; i < ndata; i++ ) 
			chi2 += SQR( y[ i ] - intercept_a - slope_b * x[ i ] );
		if ( ndata > 2 ) 
			sigdat=sqrt( chi2 / ( ndata - 2 ) );
		siga *= sigdat;
		sigb *= sigdat;
	}
};


 //   //std::vector<double> x{6, 5, 11, 7, 5, 4, 4};
 //   //std::vector<double> y{2, 3, 9, 1, 8, 7, 5};
 //   //qDebug() << slope(x, y) << '\n';  // outputs 0.305556

	//std::vector<double> x { 1.5, 2.4, 3.2, 4.8,  5.0, 7.0,  8.43 };
	//std::vector<double> y { 3.5, 5.3, 7.7, 6.2, 11.0, 9.5, 10.27 };

	////std::vector<double> x { 1., 1., 1, 1., 1., 1., 1. };
 ////   std::vector<double> y { 1., 2., 3., 4., 4., 5., 6. };

 //   qDebug() << slope(x, y) << '\n';


	//Fitab fit( x, y );				// y = a + bx
 //   qDebug() << fit.intercept_a;	//Intercept				- 3.46212
 //   qDebug() << fit.slope_b;		//Slope					- 0.904273
 //   qDebug() << fit.chi2;
 //   qDebug() << fit.sigdat;
 //   qDebug() << fit.siga;
 //   qDebug() << fit.sigb;
	////qDebug() << fit.coefficient 	//Regression coefficient- 0.808257



bool CYFXValues::CCurve::Fit() const
{
	assert__( mX && mY && mSizeX == mSizeY );

	if ( isDefaultValue( mSlope_b ) )
	{
		Fitab fit( mSizeX, mX, mY );

		mIntercept_a = fit.intercept_a;
		mSlope_b = fit.slope_b;
	}

	return !std::isnan( mSlope_b ) && !std::isnan( mIntercept_a );
}
