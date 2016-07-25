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
#if !defined(AFX_LISTCONTOUR_H__84C3BBC0_6F70_416A_8194_9657813FF3AE__INCLUDED_)
#define AFX_LISTCONTOUR_H__84C3BBC0_6F70_416A_8194_9657813FF3AE__INCLUDED_

#include "stdafx.h"

#include "BratContour.h"

// a list of point index referring to the secondary grid
// Let i the index of a point,
typedef std::list<size_t> CLineStrip;
typedef std::list<CLineStrip*> CLineStripList;
typedef std::vector<CLineStripList> CLineStripListVector;

class CListContour : public CBratContour
{
	using base_t = CBratContour;
public:
	CListContour();
	virtual ~CListContour();

	// retreiving list of line strip for the i-th contour
    CLineStripList* GetLines( size_t iPlane)	{	assert__(iPlane>=0); assert__(iPlane<GetNPlanes());	return &m_vStripLists[iPlane]; }

	// Initializing memory
    virtual void InitMemory() override;
	// Cleaning memory and line strips
    virtual void CleanMemory() override;
	// Generate contour strips
protected:
	virtual void Generate( bool compact = true ) override;
	virtual void SetProgressInterface( CProgressInterface *pI = nullptr, int m = 0, int M = 0 ) override;
public:

	// Adding segment to line strips
	// See CBratContour::ExportLine for further details
    void ExportLine(long_t iPlane,long_t x1, long_t y1, long_t x2, long_t y2) override;

	// Basic algorithm to concatanate line strip. Not optimized at all !
	bool CompactStrips();
	/// debuggin
	void DumpPlane( size_t iPlane ) const;

	// Area given by this function can be positive or negative depending on the winding direction of the contour.
	double Area(CLineStrip* Line);

	double EdgeWeight(CLineStrip* pLine, double R);
	bool   PrintContour(char *fname);
protected:
	// Merges pStrip1 with pStrip2 if they have a common end point
	bool MergeStrips(CLineStrip* pStrip1, CLineStrip* pStrip2);
	// Merges the two strips with a welding threshold.
	bool ForceMerge(CLineStrip* pStrip1, CLineStrip* pStrip2);
	// returns true if contour is touching boundary
	bool OnBoundary(CLineStrip* pStrip);

private:
	// array of line strips
	CLineStripListVector m_vStripLists; 
};


#endif // !defined(AFX_LISTCONTOUR_H__84C3BBC0_6F70_416A_8194_9657813FF3AE__INCLUDED_)
