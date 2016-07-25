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
#if !defined(AFX_CONTOUR_H__45A84754_43CD_414A_80A0_9211F954B0DA__INCLUDED_)
#define AFX_CONTOUR_H__45A84754_43CD_414A_80A0_9211F954B0DA__INCLUDED_


#include "DataModels/ProgressInterface.h"


// A structure used internally by CBratContour
struct CFnStr {
    double m_dFnVal;
    short m_sLeftLen;
	short m_sRightLen;
	short m_sTopLen;
	short m_sBotLen;
};


using long_t = long long;


class CBratContour  
{
protected:

	static const long_t invalid;

public:
	CBratContour();
	virtual ~CBratContour();

	// Initialize memory. Called in Generate
	virtual void InitMemory();
	// Clean work arrays
	virtual void CleanMemory();
	// Generates contour
	// Before calling this functions you must
	//	1. derive the function ExportLine that is 
	//		supposed to draw/store the segment of the contour
	//	2. Set the function draw contour of. (using  SetFieldFn
	//		The function must be declared as follows
	//		double (*myF)(double x , double y);
protected:
	virtual void Generate( bool compact = true );	//compact parameter: femm 
public:
	bool GenerateContours( CProgressInterface *pI = nullptr, bool compact = true );

	// Set the dimension of the primary grid
	void SetFirstGrid(long_t iCol, long_t iRow);
	// Set the dimension of the base grid
	void SetSecondaryGrid(long_t iCol, long_t iRow);
	// Sets the region [left, right, bottom,top] to generate contour 
	void SetLimits(double pLimits[4]);
	// Sets the isocurve values
	void SetPlanes(const std::vector<double>& vPlanes);
	// Sets the pointer to the F(x,y) funtion
	void SetFieldFcn( const std::function<double (double x, double y)>& pFieldFcn );

	// Retrieve dimension of grids, contouring region and isocurve
    long_t GetColFir() const		{	return m_iColFir; }
    long_t GetRowFir() const		{	return m_iRowFir; }
    long_t GetColSec() const		{	return m_iColSec; }
    long_t GetRowSec() const		{	return m_iRowSec; }
	void GetLimits(double pLimits[4]);
    size_t GetNPlanes() const						{ return m_vPlanes.size(); }
    const std::vector<double>& GetPlanes() const	{	return m_vPlanes; }
    double GetPlane( size_t i )	const				{	assert__(i>=0); assert__(i<m_vPlanes.size()); return m_vPlanes[i]; }

	// For an indexed point i on the sec. grid, returns x(i)
    double GetXi(long_t i) const {	return m_pLimits[0] +  i%(m_iColSec+1)*(m_pLimits[1]-m_pLimits[0])/(double)( m_iColSec ); }
	// For an indexed point i on the fir. grid, returns y(i)
    double GetYi(long_t i) const { assert__(i>=0); return m_pLimits[2] +  i/(m_iColSec+1)*(m_pLimits[3]-m_pLimits[2])/(double)( m_iRowSec ); }

private:	//femm
	std::function< double (double x, double y) > m_pFieldFcn; // pointer to F(x,y) function
	CProgressInterface *mProgressInterface = nullptr;

protected:
	std::function< bool( int ) > mProgress; // pointer to SetCurrentValue( ) progress interface
	virtual void SetProgressInterface( CProgressInterface *pI = nullptr, int m = 0, int M = 0 );
	int ProgressSoFar() const { return mProgressInterface->CurrentValue(); }
	void SetProgressLabel( const std::string &label ) { return mProgressInterface->SetLabel( label ); }

protected:
	// Accessible variables
	std::vector<double> m_vPlanes;							// value of contour planes
	double m_pLimits[4];									// left, right, bottom, top
	long_t m_iColFir;											// primary	grid, number of columns
	long_t m_iRowFir;											// primary	grid, number of rows
	long_t m_iColSec;											// secondary grid, number of columns
	long_t m_iRowSec;											// secondary grid, number of rows

	// Protected function
	virtual void ExportLine(long_t iPlane, long_t x1, long_t y1, long_t x2, long_t y2) = 0; // plots a line from (x1,y1) to (x2,y2)

	// Work functions and variables
	double m_dDx;
	double m_dDy;

private:
	CFnStr** m_ppFnData;	// pointer to mesh parts
    CFnStr* FnctData(long_t i,long_t j)  {	return (m_ppFnData[i]+j); }
	double Field(long_t x, long_t y);	 /* evaluate funct if we must,	*/
	void Cntr1(long_t x1, long_t x2, long_t y1, long_t y2);
	void Pass2(long_t x1, long_t x2, long_t y1, long_t y2);	  /* draws the contour lines */
};

// A simple test function
double TestFunction(double x,double y);


#endif // !defined(AFX_CONTOUR_H__45A84754_43CD_414A_80A0_9211F954B0DA__INCLUDED_)
