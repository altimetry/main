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

#ifndef DATA_MODELS_PLOT_DATA_VTKCOLOR_H
#define DATA_MODELS_PLOT_DATA_VTKCOLOR_H

#include <QColor>

#include "libbrathl/brathl.h"
#include "libbrathl/BratObject.h"
#include "libbrathl/Tools.h"
using namespace brathl;

//-------------------------------------------------------------
//------------------- CPlotColor class --------------------
//-------------------------------------------------------------

class CPlotColor : public CBratObject
{
public:
	CPlotColor();

	CPlotColor( double r, double g, double b, double a = 1 );
	CPlotColor( int r, int g, int b, int a = 255 )
	{
		Set( r, g, b, a );
	}
	CPlotColor( const CPlotColor &color );
	CPlotColor( const QColor &c )
	{
		*this = c;
	}

	~CPlotColor();

	const CPlotColor& operator = ( const CPlotColor& color );

	CPlotColor& operator = ( const QColor &c )
	{
		Set( c.red(), c.green(), c.blue(), c.alpha() );
		return *this;
	}
	

	bool operator ==( const CPlotColor& color );

	void Get( double& r, double& g, double& b, double& a ) const;
	void Get( uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a ) const;
	void Get( uint32_t& r, uint32_t& g, uint32_t& b, uint32_t& a ) const;
	void Get( int& r, int& g, int& b, int& a ) const;

	void Set( double r, double g, double b, double a = 1 );
	void Set( uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255 );
	void Set( uint32_t r, uint32_t g, uint32_t b, uint32_t a = 255 );
	void Set( int r, int g, int b, int a = 255 );
	void Set( const CPlotColor& color );

	bool Ok() const { return m_isInit; }
	void Reset();

	QColor GetQColor() const 
	{ 
		int r, g, b, a;
		Get( r, g, b, a );
		return QColor( r, g, b, a );
	}

	double Red() const { return m_r; }
	double Green() const { return m_g; }
	double Blue() const { return m_b; }
	double Alpha() const { return m_a; }

private:
	double m_r;
	double m_g;
	double m_b;
	double m_a;

	bool m_isInit;
};



// QColor <=> CPlotColor ///////////////////////////



template< typename COLOR_TO, typename COLOR_FROM >
COLOR_TO color_cast( COLOR_FROM s );


template<>
inline QColor color_cast< QColor >( CPlotColor c )
{
	return c.GetQColor();
}

template<>
inline CPlotColor color_cast< CPlotColor >( QColor qc )
{
	return CPlotColor( qc );
}




#endif			//DATA_MODELS_PLOT_DATA_VTKCOLOR_H
