#if !defined(AFX_LISTCONTOUR_H__84C3BBC0_6F70_416A_8194_9657813FF3AE__INCLUDED_)
#define AFX_LISTCONTOUR_H__84C3BBC0_6F70_416A_8194_9657813FF3AE__INCLUDED_

#include "stdafx.h"

#include "BratContour.h"

// a list of point index referring to the secondary grid
// Let i the index of a point,
typedef std::list<unsigned long> CLineStrip;
typedef std::list<CLineStrip*> CLineStripList;
typedef std::vector<CLineStripList> CLineStripListVector;

class CListContour : public CBratContour  
{
public:
	CListContour();
	virtual ~CListContour();

	// retreiving list of line strip for the i-th contour
    CLineStripList* GetLines(unsigned long iPlane)	{	assert__(iPlane>=0); assert__(iPlane<GetNPlanes());	return &m_vStripLists[iPlane]; }

	// Initializing memory
    virtual void InitMemory() override;
	// Cleaning memory and line strips
    virtual void CleanMemory() override;
	// Generate contour strips
	virtual void Generate( bool compact = true ) override;

	// Adding segment to line strips
	// See CBratContour::ExportLine for further details
    void ExportLine(int iPlane,int x1, int y1, int x2, int y2) override;

	// Basic algorithm to concatanate line strip. Not optimized at all !
	bool CompactStrips();
	/// debuggin
	void DumpPlane(unsigned long iPlane) const;

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
