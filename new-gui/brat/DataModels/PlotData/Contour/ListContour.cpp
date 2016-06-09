#include "new-gui/brat/stdafx.h"
#include "ListContour.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CListContour::CListContour()
	: CContour()
{}

CListContour::~CListContour()
{
	CleanMemory();
}

void CListContour::Generate()
{
	// generate line strips
	CContour::Generate();
	// compact strips
	CompactStrips();
}

void CListContour::InitMemory()
{
	CContour::InitMemory();
	
	CLineStripList::iterator pos;
	CLineStrip* pStrip;
	
	if (!m_vStripLists.empty())
	{
		unsigned long i;
		// reseting lists
		assert__(m_vStripLists.size() == GetNPlanes());
		for (i=0;i<GetNPlanes();i++)
		{
			for (pos = m_vStripLists[i].begin(); pos!=m_vStripLists[i].end() ; pos++)
			{
				pStrip=(*pos);
				assert__(pStrip);

				pStrip->clear();
				delete pStrip;
			}
			m_vStripLists[i].clear();
		}
	}
	else
		m_vStripLists.resize(GetNPlanes());
}

void CListContour::CleanMemory()
{
	CContour::CleanMemory();
	
	// reseting lists
//	assert__(m_vStripLists.size() == GetNPlanes());
	for (unsigned long i= 0; i< m_vStripLists.size(); i++ )
	{
		for (CLineStripList::iterator pos=m_vStripLists[i].begin(); pos!=m_vStripLists[i].end();pos++)
		{
			CLineStrip* pStrip=(*pos);			assert__(pStrip);
			pStrip->clear();
			delete pStrip;
		}
		m_vStripLists[i].clear();
	}
}

void CListContour::ExportLine(int iPlane,int x1, int y1, int x2, int y2)
{
	assert__( iPlane >= 0 );
	assert__( iPlane < (int)GetNPlanes() );
	
	// check that the two points are not at the beginning or end of the  some line strip
	unsigned long i1=y1*( m_iColSec + 1 ) + x1;
	unsigned long i2=y2*( m_iColSec + 1 ) + x2;
	
	CLineStrip* pStrip;
	bool added=false;
	for ( CLineStripList::iterator pos = m_vStripLists[ iPlane ].begin(); pos != m_vStripLists[ iPlane ].end() && !added; pos++ )
	{
		pStrip = *pos;										assert__( pStrip );
		// check if points are appendable to this strip
		if ( i1 == pStrip->front() )
		{
			pStrip->insert( pStrip->begin(), i2 );
			return;
		}
		if ( i1 == pStrip->back() )
		{
			pStrip->insert( pStrip->end(), i2 );
			return;
		}
		if ( i2 == pStrip->front() )
		{
			pStrip->insert( pStrip->begin(), i1 );
			return;
		}
		if ( i2 == pStrip->back() )
		{
			pStrip->insert( pStrip->end(), i1 );
			return;
		}
	}
	
	// segment was not part of any line strip, creating new one
	pStrip=new CLineStrip;
	pStrip->insert(pStrip->begin(),i1);
	pStrip->insert(pStrip->end(),i2);
	m_vStripLists[iPlane].insert(m_vStripLists[iPlane].begin(),pStrip);
}

bool CListContour::ForceMerge(CLineStrip* pStrip1, CLineStrip* pStrip2) 
{
	
	CLineStrip::iterator pos;
	CLineStrip::reverse_iterator rpos;
	
	if (pStrip2->empty())
		return false;
	
	double x[4], y[4], weldDist;
	int index;
	index = pStrip1->front();
	x[0] = GetXi(index);
	y[0] = GetYi(index);
	index = pStrip1->back();
	x[1] = GetXi(index);
	y[1] = GetYi(index);
	index = pStrip2->front();
	x[2] = GetXi(index);
	y[2] = GetYi(index);
	index = pStrip2->back();
	x[3] = GetXi(index);
	y[3] = GetYi(index);
	
	weldDist = 10*(m_dDx*m_dDx+m_dDy*m_dDy);
	
	if ((x[1]-x[2])*(x[1]-x[2])+(y[1]-y[2])*(y[1]-y[2])< weldDist)
    {
		for (pos=pStrip2->begin(); pos!=pStrip2->end();pos++)
		{
			index=(*pos);
			assert__(index>=0);
			pStrip1->insert(pStrip1->end(),index);
		}
		pStrip2->clear();
		return true;
    }
	
	if ((x[3]-x[0])*(x[3]-x[0])+(y[3]-y[0])*(y[3]-y[0])< weldDist)
    {
		for (rpos=pStrip2->rbegin(); rpos!=pStrip2->rend();rpos++)
		{
			index=(*rpos);
			assert__(index>=0);
			pStrip1->insert(pStrip1->begin(),index);
		}
		pStrip2->clear();
		return true;
    }
	
	if ((x[1]-x[3])*(x[1]-x[3])+(y[1]-y[3])*(y[1]-y[3])< weldDist)
    {
		for (rpos=pStrip2->rbegin(); rpos!=pStrip2->rend();rpos++)
		{
			index=(*rpos);
			assert__(index>=0);
			pStrip1->insert(pStrip1->end(),index);
		}
		pStrip2->clear();
		return true;
    }

	if ((x[0]-x[2])*(x[0]-x[2])+(y[0]-y[2])*(y[0]-y[2])< weldDist)
    {
		for (pos=pStrip2->begin(); pos!=pStrip2->end();pos++)
		{
			index=(*pos);
			assert__(index>=0);
			pStrip1->insert(pStrip1->begin(),index);
		}
		pStrip2->clear();
		return true;
    }

	return false;
}

bool CListContour::MergeStrips(CLineStrip* pStrip1, CLineStrip* pStrip2)
{
	CLineStrip::iterator pos;
	CLineStrip::reverse_iterator rpos;
	if (pStrip2->empty())
		return false;
	
	int index;

	// debugging stuff
	if (pStrip2->front()==pStrip1->front())
    {
		// retreiving first element
		pStrip2->pop_front();
		// adding the rest to strip1
		for (pos=pStrip2->begin(); pos!=pStrip2->end();pos++)
		{
			index=(*pos);
			assert__(index>=0);
			pStrip1->insert(pStrip1->begin(),index);
		}
		pStrip2->clear();
		return true;
    }
	
	if (pStrip2->front()==pStrip1->back())
    {
		pStrip2->pop_front();
		// adding the rest to strip1
		for (pos=pStrip2->begin(); pos!=pStrip2->end();pos++)
		{
			index=(*pos);
			assert__(index>=0);
			pStrip1->insert(pStrip1->end(),index);
		}
		pStrip2->clear();
		return true;
    }
	
	if (pStrip2->back()==pStrip1->front())
    {
		pStrip2->pop_back();
		// adding the rest to strip1
		for (rpos=pStrip2->rbegin(); rpos!=pStrip2->rend();rpos++)
		{
			index=(*rpos);
			assert__(index>=0);
			pStrip1->insert(pStrip1->begin(),index);
		}
		pStrip2->clear();
		return true;
    }
	
	if (pStrip2->back()==pStrip1->back())
    {
		pStrip2->pop_back();
		// adding the rest to strip1
		for (rpos=pStrip2->rbegin(); rpos!=pStrip2->rend();rpos++)
		{
			index=(*rpos);
			assert__(index>=0);
			pStrip1->insert(pStrip1->end(),index);
		}
		pStrip2->clear();
		return true;
    }
	
	return false;
}

bool CListContour::CompactStrips()
{
	CLineStrip* pStrip;
	CLineStrip* pStripBase;
	unsigned long i;
	CLineStripList::iterator pos,pos2;
	CLineStripList newList;
	bool again, changed;	
	
	const double weldDist = 10*(m_dDx*m_dDx+m_dDy*m_dDy);

	assert__(m_vStripLists.size() == GetNPlanes());
	for (i=0;i<GetNPlanes();i++)
	{
		again=true;
		while(again)
		{
			// REPEAT COMPACT PROCESS UNTILL LAST PROCESS MAKES NO CHANGE
			
			again=false;
			// building compacted list
			assert__(newList.empty());
			for (pos=m_vStripLists[i].begin(); pos!=m_vStripLists[i].end();pos++)
			{
				pStrip=(*pos);
				for (pos2=newList.begin(); pos2!=newList.end();pos2++)
				{
					pStripBase=(*pos2);
					changed=MergeStrips(pStripBase,pStrip);
					if (changed)
						again=true;
					if (pStrip->empty())
						break;
				}
				if (pStrip->empty())
					delete pStrip;
				else
					newList.insert(newList.begin(),pStrip);
			}
			
			
			// deleting old list
			m_vStripLists[i].clear();
			// Copying all
			for (pos2=newList.begin(); pos2 != newList.end(); pos2++)
			{
				pStrip=(*pos2);
				CLineStrip::iterator pos1 = pStrip->begin(),pos3;
				while (pos1!=pStrip->end()) 
				{
					pos3 = pos1; 
					pos3++;
					if ( pos3 != pStrip->end() && (*pos1) == (*pos3))
						pStrip->erase(pos3);
					else 
						pos1++;
				}
				
				//if (!(pStrip->front()==pStrip->back() && pStrip->size()==2))
				if (pStrip->size()!=1) 	
					m_vStripLists[i].insert(m_vStripLists[i].begin(),pStrip );
				else 
					delete pStrip;
			}
			// emptying temp list
			newList.clear();
			
		} // OF WHILE(AGAIN) (LAST COMPACT PROCESS MADE NO CHANGES)
		
	
		if (m_vStripLists[i].empty())
			continue;
		///////////////////////////////////////////////////////////////////////
		// compact more
		int Nstrip,j,index,count;

		Nstrip = m_vStripLists[i].size(); 
		std::vector<bool> closed(Nstrip);
		double x,y;

		// First let's find the open and closed lists in m_vStripLists
		for(pos2 = m_vStripLists[i].begin(), j=0, count=0; pos2 != m_vStripLists[i].end(); pos2++, j++) 
		{
			pStrip = (*pos2);

			// is it open ?
			if (pStrip->front() != pStrip->back()) 
			{
				index = pStrip->front();
				x = GetXi(index); y = GetYi(index);
				index = pStrip->back();
				x -= GetXi(index); y -= GetYi(index);
				
				// is it "almost closed" ?
				if ( x*x+y*y < weldDist)
					closed[j] = true; 
				else 
				{ 
					closed[j] = false; 
					// updating not closed counter...
					count ++; 
				}
			} 
			else 
				closed[j] = true;
		}
		
		// is there any open strip ?
		if (count > 1) 
		{ 
			// Merge the open strips into NewList
			pos = m_vStripLists[i].begin();
			for(j=0;j<Nstrip;j++) 
			{
				if (closed[j] == false ) 
				{
					pStrip = (*pos);
					newList.insert(newList.begin(),pStrip);	
					pos = m_vStripLists[i].erase(pos);
				}
				else 
					pos ++;
			}
			
			// are they open strips to process ?
			while(newList.size()>1) 
			{
				pStripBase = newList.front();
				
				// merge the rest to pStripBase
				again = true;
				while (again) 
				{
					again = false;
					pos = newList.begin(); 
					for(pos++; pos!=newList.end();) 
					{
						pStrip = (*pos);
						changed = ForceMerge(pStripBase,pStrip);
						if (changed) 
						{ 
							again = true;
							delete pStrip;
							pos = newList.erase(pos);
						} 
						else 
							pos ++;
					}		      
				} // while(again)
				
				index = pStripBase->front();
				x = GetXi(index); y = GetYi(index);
				index = pStripBase->back();
				x -= GetXi(index); y -= GetYi(index);
				
				// if pStripBase is closed or not
				if (x*x+y*y < weldDist) 
				{
					m_vStripLists[i].insert(m_vStripLists[i].begin(),pStripBase);
					newList.pop_front();
				} 
				else 
				{			
					if (OnBoundary(pStripBase)) 
					{
						LOG_WARN("# open strip ends on boundary, continue.\n");
						m_vStripLists[i].insert(m_vStripLists[i].begin(),pStripBase);
						newList.pop_front();
					} 
					else 
					{
						LOG_WARN("unpaird open strip at 1!");
						return false;
					}
				}
			} // while(newList.size()>1);


			if (newList.size() ==1) 
			{
				pStripBase = newList.front();
				if (OnBoundary(pStripBase)) 
				{
					LOG_WARN("# open strip ends on boundary, continue.\n");
					m_vStripLists[i].insert(m_vStripLists[i].begin(),pStripBase);
					newList.pop_front();
				} 
				else 
				{
					LOG_WARN("unpaird open strip at 2!");
					DumpPlane(i);
					return false;
				}
			}
			
			newList.clear();
			
		} 
		else if (count == 1) 
		{
			pos = m_vStripLists[i].begin();
			for(j=0;j<Nstrip;j++) 
			{
				if (closed[j] == false ) 
				{
					pStripBase = (*pos);
					break;
				}
				pos ++;
			}
			if (OnBoundary(pStripBase)) 
			{
				LOG_WARN("# open strip ends on boundary, continue.\n");
			} 
			else 
			{
				LOG_WARN("unpaird open strip at 3!");
				DumpPlane(i);
				return false;
			}
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////
	}

	return true;
}


bool CListContour::OnBoundary(CLineStrip* pStrip) 
{
	bool e1,e2;

	int index = pStrip->front();
	double x = GetXi(index), y = GetYi(index);
	if (x==m_pLimits[0] || x == m_pLimits[1] || 
		y == m_pLimits[2] || y == m_pLimits[3])
		e1 = true;
	else 
		e1 = false;
	
	index = pStrip->back();
	x = GetXi(index); y = GetYi(index);
	if (x==m_pLimits[0] || x == m_pLimits[1] || 
		y == m_pLimits[2] || y == m_pLimits[3])
		e2 = true;
	else 
		e2 = false;
	
	return (e1 && e2);
}

void CListContour::DumpPlane(unsigned long iPlane) const
{
	CLineStripList::const_iterator pos;
	unsigned long i;
	CLineStrip* pStrip;
	
	assert__(iPlane>=0);
	assert__(iPlane<GetNPlanes());
	LOG_WARN("Level : %d"),GetPlane(iPlane);
	
	LOG_WARN("Number of strips : %d\r\n"),m_vStripLists[iPlane].size();
	LOG_WARN("i np start end xstart ystart xend yend\r\n");
	for (pos = m_vStripLists[iPlane].begin(), i=0; pos != m_vStripLists[iPlane].end(); pos++, i++)
	{
		pStrip=*pos;
		assert__(pStrip);
        LOG_WARN(
                    n2s<std::string>(i) + " " +
                    n2s<std::string>(pStrip->size()) + " " +
                    n2s<std::string>(pStrip->front()) + " " +
                    n2s<std::string>(pStrip->back()) + " " +
                    n2s<std::string>(GetXi(pStrip->front())) + " " +
                    n2s<std::string>(GetYi(pStrip->front())) + " " +
                    n2s<std::string>(GetXi(pStrip->back())) + " " +
                    n2s<std::string>(GetYi(pStrip->back())) + "\r\n"
                    );
	}
}

double CListContour::Area(CLineStrip* Line) 
{
	// if Line is not closed, return 0;
	
	double Ar = 0, x, y, x0,y0,x1, y1;
	int index;
	
	CLineStrip::iterator pos;
	pos = Line->begin();
	index = (*pos);
	x0 = x =  GetXi(index);
	y0 = y =  GetYi(index);
	
	pos ++;
	
	for(;pos!=Line->end();pos++) 
	{
		index = (*pos);  
		x1 = GetXi(index);
		y1 = GetYi(index);
		// Ar += (x1-x)*(y1+y);
		Ar += (y1-y)*(x1+x)-(x1-x)*(y1+y);
		x = x1;
		y = y1;  
		
	}
	
	
	//Ar += (x0-x)*(y0+y);
	Ar += (y0-y)*(x0+x)-(x0-x)*(y0+y);
	// if not closed curve, return 0;
	if ((x0-x)*(x0-x) + (y0-y)*(y0-y)>20*(m_dDx*m_dDx+m_dDy*m_dDy)) 
	{
		Ar = 0;
		LOG_WARN("# open curve!\n");
	}
	//else   Ar /= -2;
	else Ar/=4;
	// result is \int ydex/2 alone the implicit direction.
	return Ar;
}

double CListContour:: EdgeWeight(CLineStrip* pLine, double R) 
{
	CLineStrip::iterator pos;
	int count = 0,index;
	double x,y;
	for(pos = pLine->begin(); pos!=pLine->end(); pos++) 
	{
		index = (*pos);
		x = GetXi(index); 
		y = GetYi(index);
		if (fabs(x) > R || fabs(y) > R) 
			count ++;
	}
	return (double)count/pLine->size();
}

bool::CListContour::PrintContour(char *fname) 
{
	std::ofstream file(fname);
	if (!file) 
	{ 
		LOG_WARN("cannot open output file.\n"); 
		return false; 
	}

	file << std::setprecision(10);
	
	unsigned long i, index;
	CLineStrip* pStrip;
	CLineStrip::iterator pos2;
	CLineStripList::iterator pos;
	
	for(i=0;i<GetNPlanes();i++) {
		for(pos = m_vStripLists[i].begin();pos!=m_vStripLists[i].end();pos++) 
		{
			pStrip = (*pos);
			for(pos2 = pStrip->begin();pos2!=pStrip->end();pos2++) 
			{
				index = (*pos2);
				file << GetXi(index)<<"\t"<<GetYi(index)<<"\n";
			}
			file<<"\n";
		}
	}
	file.close();
	return true;
	
}
