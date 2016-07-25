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
#include "stdlib.h"
#include "math.h"
#include "BratContour.h"

//static 
const long_t CBratContour::invalid = std::numeric_limits<long_t>::max();


double TestFunction(double x,double y)
{  
	return 0.5*(cos(x+3.14/4)+sin(y+3.14/4)); 
};


inline bool DefaultSetCurrentValue( int current )
{
    UNUSED( current );

	return true;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBratContour::CBratContour()
{
	m_iColFir=m_iRowFir=32;
	m_iColSec=m_iRowSec=256;
	m_dDx=m_dDy=0;
	m_pFieldFcn=nullptr;
	m_pLimits[0]=m_pLimits[2]=0;
	m_pLimits[1]=m_pLimits[3]=5.;
	m_ppFnData=nullptr;

	// temporary stuff
	m_pFieldFcn=TestFunction;
	SetProgressInterface();
	m_vPlanes.resize(20);
	for ( size_t i=0; i < m_vPlanes.size(); i++ )
	{
		m_vPlanes[ i ]=( i - m_vPlanes.size() / 2.0 )*0.1;
	}
}


void CBratContour::SetProgressInterface( CProgressInterface *pI, int m, int M )		//pI = nullptr, int m = 0, int M = 0 
{
	mProgressInterface = pI;

	if ( mProgressInterface )
	{
		mProgressInterface->SetRange( m, M );
		mProgress = [this]( int current ) -> bool
		{
			if ( !mProgressInterface->SetCurrentValue( current ) )
				throw false;
			return true;
		};
	}
	else
		mProgress = DefaultSetCurrentValue;
}


CBratContour::~CBratContour()
{
	CleanMemory();
}

void CBratContour::InitMemory()
{
	if (!m_ppFnData)
	{
		m_ppFnData=new CFnStr*[m_iColSec+1];
		for (long_t i=0;i<m_iColSec+1;i++)
		{
			m_ppFnData[i]=nullptr;
		}
	}
}

void CBratContour::CleanMemory()
{
	if (m_ppFnData)
	{
		long_t i;
		for (i=0;i<m_iColSec+1;i++)
		{
			if ( m_ppFnData[ i ] )
				delete[] m_ppFnData[ i ];
		}
		delete[] m_ppFnData;
		m_ppFnData=nullptr;
	}
}

void CBratContour::Generate( bool compact )	//bool compact = true 
{
	UNUSED( compact );

	long_t i, j;
	long_t x3, x4, y3, y4, x, y, oldx3, xlow;
	const long_t cols=m_iColSec+1;
	const long_t rows=m_iRowSec+1;
	double xoff,yoff;
	
	// Initialize memory if needed
	InitMemory();

	m_dDx = (m_pLimits[1]-m_pLimits[0])/(double)(m_iColSec);
	xoff = m_pLimits[0];
	m_dDy = (m_pLimits[3]-m_pLimits[2])/(double)(m_iRowSec);
	yoff = m_pLimits[2];

	xlow = 0;
	oldx3 = 0;
	x3 = (cols-1)/m_iRowFir;
	x4 = ( 2*(cols-1) )/m_iRowFir;
	for (x = oldx3; x <= x4; x++) 
	{	  
		// allocate new columns needed
		if (x >= cols)
			break;
		if (m_ppFnData[x]==nullptr)
			m_ppFnData[x] = new CFnStr[rows];

		for (y = 0; y < rows; y++)
			FnctData(x,y)->m_sTopLen = -1;
	}


	y4 = 0;
	for (j = 0; j < m_iColFir; j++) 
	{
		y3 = y4;
		y4 = ((j+1)*(rows-1))/m_iColFir;
		Cntr1( oldx3, x3, y3, y4 );
	}

	mProgress( ProgressSoFar() + 1 );

	for (i = 1; i < m_iRowFir; i++) 
	{
		mProgress( ProgressSoFar() + 1 );

		y4 = 0;
		for (j = 0; j < m_iColFir; j++) 
		{
			y3 = y4;
			y4 = ((j+1)*(rows-1))/m_iColFir;
			Cntr1(x3, x4, y3, y4);
		}

		y4 = 0;
		for (j = 0; j < m_iColFir; j++) 
		{
			y3 = y4;
			y4 = ((j+1)*(rows-1))/m_iColFir;
			Pass2(oldx3,x3,y3,y4);
		}

		if (i < (m_iRowFir-1)) 
		{	 /* re-use columns no longer needed */
			oldx3 = x3;
			x3 = x4;
			x4 = ((i+2)*(cols-1))/m_iRowFir;
			for (x = x3+1; x <= x4; x++) 
			{
				if (xlow < oldx3) 
				{
					if (m_ppFnData[x])
						delete[] m_ppFnData[x];
					m_ppFnData[x] = m_ppFnData[xlow];
					m_ppFnData[ xlow ] = nullptr;
					xlow++;
				} 
				else
					if (m_ppFnData[x]==nullptr)
						m_ppFnData[x] = new CFnStr[rows];

				for (y = 0; y < rows; y++)
					FnctData(x,y)->m_sTopLen = -1;
			}
		}
	}

	mProgress( ProgressSoFar() + 1 );

	y4 = 0;
	for (j = 0; j < m_iColFir; j++) 
	{
		y3 = y4;
		y4 = ((j+1)*(rows-1))/m_iColFir;
		Pass2(x3,x4,y3,y4);
	}

	mProgress( ProgressSoFar() + 1 );
}

bool CBratContour::GenerateContours( CProgressInterface *pI, bool compact )	//i = nullptr, bool compact = true 
{
	bool result = false;
	try
	{
		SetProgressInterface( pI, 0, m_iRowFir + 3 );
		SetProgressLabel( "Computing contours..." );
		Generate( compact );
		result = true;
	}
	catch ( ... )
	{
	}
	SetProgressInterface();
	return result;
}


void CBratContour::Cntr1(long_t x1, long_t x2, long_t y1, long_t y2)
{
	double f11, f12, f21, f22, f33;
	long_t x3, y3, i, j;
	
	if ((x1 == x2) || (y1 == y2))	/* if not a real cell, punt */
		return;
	f11 = Field(x1, y1);
	f12 = Field(x1, y2);
	f21 = Field(x2, y1);
	f22 = Field(x2, y2);
	//if ( std::isnan( f11 ) || std::isnan( f12 ) || std::isnan( f21 ) || std::isnan( f22 ) )
	//	return;
	if ((x2 > x1+1) || (y2 > y1+1)) {	/* is cell divisible? */
		x3 = (x1+x2)/2;
		y3 = (y1+y2)/2;
		f33 = Field(x3, y3);
		//if ( std::isnan( f33 ) )
		//	return;
		i = j = 0;
		if (f33 < f11) i++; else if (f33 > f11) j++;
		if (f33 < f12) i++; else if (f33 > f12) j++;
		if (f33 < f21) i++; else if (f33 > f21) j++;
		if (f33 < f22) i++; else if (f33 > f22) j++;
		if ((i > 2) || (j > 2)) /* should we divide cell? */
		{	
			/* subdivide cell */
			Cntr1(x1, x3, y1, y3);
			Cntr1(x3, x2, y1, y3);
			Cntr1(x1, x3, y3, y2);
			Cntr1(x3, x2, y3, y2);
			return;
		}
	}
	/* install cell in array */
	FnctData(x1,y2)->m_sBotLen = FnctData(x1,y1)->m_sTopLen = x2-x1;
	FnctData(x2,y1)->m_sLeftLen = FnctData(x1,y1)->m_sRightLen = y2-y1;
}

void CBratContour::Pass2(long_t x1, long_t x2, long_t y1, long_t y2)
{
	long_t left, right, top, bot,old, iNew, i, j, x3, y3;
	double yy0, yy1, xx0, xx1, xx3, yy3;
	double v, f11, f12, f21, f22, f33, fold, fnew;
	double xoff=m_pLimits[0];
	double yoff=m_pLimits[2];
	
	if ((x1 == x2) || (y1 == y2))	/* if not a real cell, punt */
		return;
	f11 = FnctData(x1,y1)->m_dFnVal;
	f12 = FnctData(x1,y2)->m_dFnVal;
	f21 = FnctData(x2,y1)->m_dFnVal;
	f22 = FnctData(x2,y2)->m_dFnVal;
	if ((x2 > x1+1) || (y2 > y1+1)) /* is cell divisible? */
	{	
		x3 = (x1+x2)/2;
		y3 = (y1+y2)/2;
		f33 = FnctData(x3, y3)->m_dFnVal;
		i = j = 0;
		if (f33 < f11) i++; else if (f33 > f11) j++;
		if (f33 < f12) i++; else if (f33 > f12) j++;
		if (f33 < f21) i++; else if (f33 > f21) j++;
		if (f33 < f22) i++; else if (f33 > f22) j++;
		if ((i > 2) || (j > 2)) /* should we divide cell? */ 
		{	
			/* subdivide cell */
			Pass2(x1, x3, y1, y3);
			Pass2(x3, x2, y1, y3);
			Pass2(x1, x3, y3, y2);
			Pass2(x3, x2, y3, y2);
			return;
		}
	}

	for (i = 0; i < (long_t)m_vPlanes.size(); i++) 
	{
		v = m_vPlanes[i];
		j = 0;
		if (f21 > v) j++;
		if (f11 > v) j |= 2;
		if (f22 > v) j |= 4;
		if (f12 > v) j |= 010;
		if ((f11 > v) ^ (f12 > v)) 
		{
			if ((FnctData(x1,y1)->m_sLeftLen != 0) &&
				(FnctData(x1,y1)->m_sLeftLen < FnctData(x1,y1)->m_sRightLen)) 
			{
				old = y1;
				fold = f11;
				while (1) 
				{
					iNew = old+FnctData(x1,old)->m_sLeftLen;
					fnew = FnctData(x1,iNew)->m_dFnVal;
					if ((fnew > v) ^ (fold > v))
						break;
					old = iNew;
					fold = fnew;
				}
				yy0 = ((old-y1)+(iNew-old)*(v-fold)/(fnew-fold))/(y2-y1);
			} 
			else
				yy0 = (v-f11)/(f12-f11);

			left = (long_t)(y1+(y2-y1)*yy0+0.5);
		}
		if ((f21 > v) ^ (f22 > v)) 
		{
			if ((FnctData(x2,y1)->m_sRightLen != 0) &&
				(FnctData(x2,y1)->m_sRightLen < FnctData(x2,y1)->m_sLeftLen)) 
			{
				old = y1;
				fold = f21;
				while (1) 
				{
					iNew = old+FnctData(x2,old)->m_sRightLen;
					fnew = FnctData(x2,iNew)->m_dFnVal;
					if ((fnew > v) ^ (fold > v))
						break;
					old = iNew;
					fold = fnew;
				}
				yy1 = ((old-y1)+(iNew-old)*(v-fold)/(fnew-fold))/(y2-y1);
			} 
			else
				yy1 = (v-f21)/(f22-f21);

			right = (long_t)(y1+(y2-y1)*yy1+0.5);
		}
		if ((f21 > v) ^ (f11 > v)) 
		{
			if ((FnctData(x1,y1)->m_sBotLen != 0) &&
				(FnctData(x1,y1)->m_sBotLen < FnctData(x1,y1)->m_sTopLen)) {
				old = x1;
				fold = f11;
				while (1) {
					iNew = old+FnctData(old,y1)->m_sBotLen;
					fnew = FnctData(iNew,y1)->m_dFnVal;
					if ((fnew > v) ^ (fold > v))
						break;
					old = iNew;
					fold = fnew;
				}
				xx0 = ((old-x1)+(iNew-old)*(v-fold)/(fnew-fold))/(x2-x1);
			} 
			else
				xx0 = (v-f11)/(f21-f11);

			bot = (long_t)(x1+(x2-x1)*xx0+0.5);
		}
		if ((f22 > v) ^ (f12 > v)) 
		{
			if ((FnctData(x1,y2)->m_sTopLen != 0) &&
				(FnctData(x1,y2)->m_sTopLen < FnctData(x1,y2)->m_sBotLen)) {
				old = x1;
				fold = f12;
				while (1) {
					iNew = old+FnctData(old,y2)->m_sTopLen;
					fnew = FnctData(iNew,y2)->m_dFnVal;
					if ((fnew > v) ^ (fold > v))
						break;
					old = iNew;
					fold = fnew;
				}
				xx1 = ((old-x1)+(iNew-old)*(v-fold)/(fnew-fold))/(x2-x1);
			} 
			else
				xx1 = (v-f12)/(f22-f12);

			top = (long_t)(x1+(x2-x1)*xx1+0.5);
		}

		switch (j) 
		{
			case 7:
			case 010:
				ExportLine(i,x1,left,top,y2);
				break;
			case 5:
			case 012:
				ExportLine(i,bot,y1,top,y2);
				break;
			case 2:
			case 015:
				ExportLine(i,x1,left,bot,y1);
			break;
		case 4:
		case 013:
			ExportLine(i,top,y2,x2,right);
			break;
		case 3:
		case 014:
			ExportLine(i,x1,left,x2,right);
			break;
		case 1:
		case 016:
			ExportLine(i,bot,y1,x2,right);
			break;
		case 0:
		case 017:
			break;
		case 6:
		case 011:
			yy3 = (xx0*(yy1-yy0)+yy0)/(1.0-(xx1-xx0)*(yy1-yy0));
			xx3 = yy3*(xx1-xx0)+xx0;
			xx3 = x1+xx3*(x2-x1);
			yy3 = y1+yy3*(y2-y1);
			xx3 = xoff+xx3*m_dDx;
			yy3 = yoff+yy3*m_dDy;
			/////////////////////////////////
			double f = m_pFieldFcn(xx3, yy3);
			/////////////////////////////////
			if ( std::isnan( f ) )
			{
				ExportLine( i, invalid, invalid, invalid, invalid );
				ExportLine( i, invalid, invalid, invalid, invalid );
			}
			else
			if (f == v) 
			{
				ExportLine( i, bot, y1, top, y2 );
				ExportLine( i, x1, left, x2, right );
			} 
			else
			if (((f > v) && (f22 > v)) || ((f < v) && (f22 < v))) 
			{
				ExportLine( i, x1, left, top, y2 );
				ExportLine( i, bot, y1, x2, right );
			} 
			else 
			{
				ExportLine( i, x1, left, bot, y1 );
				ExportLine( i, top, y2, x2, right );
			}
		}
	}
}

double CBratContour::Field(long_t x, long_t y)	 /* evaluate funct if we must,	*/
{
	double x1, y1;
	
	if (FnctData(x,y)->m_sTopLen != -1)  /* is it already in the array */
		return(FnctData(x,y)->m_dFnVal);

	/* not in the array, create new array element */
	x1 = m_pLimits[0]+m_dDx*x;
	y1 = m_pLimits[2]+m_dDy*y;
	FnctData(x,y)->m_sTopLen = 0;
	FnctData(x,y)->m_sBotLen = 0;
	FnctData(x,y)->m_sRightLen = 0;
	FnctData(x,y)->m_sLeftLen = 0;
	///////////////////////////////////////////////////////
	return (FnctData(x,y)->m_dFnVal = m_pFieldFcn(x1, y1));
	///////////////////////////////////////////////////////
}

void CBratContour::SetPlanes(const std::vector<double>& vPlanes)
{	
	// cleaning memory
	CleanMemory();

	m_vPlanes = vPlanes;
};

void CBratContour::SetFieldFcn( const std::function<double (double x, double y)>& pFieldFcn ) 
{	
	m_pFieldFcn = pFieldFcn ;
};

void CBratContour::SetFirstGrid(long_t iCol, long_t iRow)
{
	m_iColFir=std::max(iCol,2ll);
	m_iRowFir=std::max(iRow,2ll);
}

void CBratContour::SetSecondaryGrid(long_t iCol, long_t iRow)
{
	// cleaning work matrices if allocated
	CleanMemory();

	m_iColSec=std::max(iCol,2ll);
	m_iRowSec=std::max(iRow,2ll);
}

void CBratContour::SetLimits(double pLimits[])
{
	assert__(pLimits[0]<pLimits[1]);
	assert__(pLimits[2]<pLimits[3]);
	for (long_t i=0;i<4;i++)
	{
		m_pLimits[i]=pLimits[i];
	}	
}

void CBratContour::GetLimits(double pLimits[])
{
	for (long_t i=0;i<4;i++)
	{
		pLimits[i]=m_pLimits[i];
	}
}
