
/*
* 
*
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

#include <cstdlib>
#include <cstdio>
#include <cstring> 
#include <string> 

#include "new-gui/Common/tools/brathl_error.h" 
#include "brathl.h" 
//#define BRAT_INTERNAL

#include "new-gui/Common/tools/TraceLog.h" 
#include "Tools.h" 
#include "new-gui/Common/tools/Exception.h" 

#include "Criteria.h" 
#include "CriteriaLatLon.h" 


using namespace brathl;



namespace brathl
{



//-------------------------------------------------------------
//------------------- CCriteriaLatLon class --------------------
//-------------------------------------------------------------


CCriteriaLatLon::CCriteriaLatLon() 
{
  Init();
}
//-------------------------------------------------------------

CCriteriaLatLon::CCriteriaLatLon(CCriteriaLatLon& c) 
{
  Init();
  Set(c); 
}

//-------------------------------------------------------------

CCriteriaLatLon::CCriteriaLatLon(CCriteriaLatLon* c) 
{
  Init();
  Set(*c); 
}

//-------------------------------------------------------------

CCriteriaLatLon::CCriteriaLatLon(CLatLonRect& latLonRect) 
{
  Init();
  Set( latLonRect ); 
}

//-------------------------------------------------------------

CCriteriaLatLon::CCriteriaLatLon(CLatLonPoint& latLonLow, CLatLonPoint& latLonHigh) 
{
  Init();
  Set(latLonLow, latLonHigh); 
}
//-------------------------------------------------------------

CCriteriaLatLon::CCriteriaLatLon(CLatLonPoint& p1, double deltaLat, double deltaLon) 
{
  Init();
  Set(p1, deltaLat, deltaLon);
}

//-------------------------------------------------------------

CCriteriaLatLon::CCriteriaLatLon(double latLow,double lonLow,double latHigh,double lonHigh) 
{
  Init();
  Set(latLow, lonLow, latHigh, lonHigh); 
}

//-------------------------------------------------------------

CCriteriaLatLon::CCriteriaLatLon(const std::string& latLow, const std::string& lonLow, const std::string& latHigh, const std::string& lonHigh) 
{
  Init();
  Set(latLow, lonLow, latHigh, lonHigh); 
}

//-------------------------------------------------------------

CCriteriaLatLon::CCriteriaLatLon(const CStringArray& array) 
{
  m_latLonRect.Set(array);
}

//----------------------------------------

CCriteriaLatLon::~CCriteriaLatLon()
{

}

//----------------------------------------

void CCriteriaLatLon::Init()
{
  m_key = CCriteria::LATLON;
  
  SetDefaultValue();

}
//----------------------------------------
const CCriteriaLatLon& CCriteriaLatLon::operator=(CCriteriaLatLon& c)
{

  Set(c);

  return *this;
}
//-------------------------------------------------------------
void CCriteriaLatLon::Set(CCriteriaLatLon& c) 
{
  Set(c.m_latLonRect);
}

//----------------------------------------
std::string CCriteriaLatLon::GetAsText(const std::string& delimiter /* = CLatLonRect::m_delimiter */)
{
  return m_latLonRect.GetAsText(delimiter);
}
//----------------------------------------
CCriteriaLatLon* CCriteriaLatLon::GetCriteria(CBratObject* ob, bool withExcept /*= true*/)
{
  CCriteriaLatLon* criteria = dynamic_cast<CCriteriaLatLon*>(ob);
  if (withExcept)
  {
    if (criteria == NULL)
    {
      CException e("CCriteriaLatLon::GetCriteria - dynamic_cast<CCriteriaLatLon*>(ob) returns NULL"
                   "object seems not to be an instance of CCriteriaLatLon",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }
  
  return criteria;

}

//-------------------------------------------------------------

void CCriteriaLatLon::Set(CLatLonRect& latLonRect) 
{
  m_latLonRect.Set(latLonRect); 
}
//-------------------------------------------------------------

void CCriteriaLatLon::Set(CLatLonPoint& latLonLow, CLatLonPoint& latLonHigh) 
{
  m_latLonRect.Set(latLonLow, latLonHigh);
}

//-------------------------------------------------------------
void CCriteriaLatLon::Set(CLatLonPoint& p1, double deltaLat, double deltaLon) 
{
  m_latLonRect.Set(p1, deltaLat, deltaLon);
}


//-------------------------------------------------------------

void CCriteriaLatLon::Set(double latLow, double lonLow, double latHigh, double lonHigh) 
{

  m_latLonRect.Set(latLow, lonLow, latHigh, lonHigh);

}

//-------------------------------------------------------------

void CCriteriaLatLon::Set(const std::string& latLow, const std::string& lonLow, const std::string& latHigh, const std::string& lonHigh) 
{
  m_latLonRect.Set(latLow, lonLow, latHigh, lonHigh);
}
//-------------------------------------------------------------
void CCriteriaLatLon::Set(const std::string& latLonRect, const std::string& delimiter /* = CLatLonRect::m_delimiter */)
{
  m_latLonRect.Set(latLonRect, delimiter);

}
//-------------------------------------------------------------

double CCriteriaLatLon::GetMinOrMaxLon(double lon1, double lon2, bool wantMin) 
{
 double midpoint = (lon1 + lon2)/2 ;
 lon1 = CLatLonPoint::LonNormal(lon1, midpoint); 
 lon2 = CLatLonPoint::LonNormal(lon2, midpoint); 

 return  (wantMin? CTools::Min(lon1, lon2) : CTools::Max(lon1, lon2));

}

//----------------------------------------
bool CCriteriaLatLon::Intersect(CLatLonRect& clip, CLatLonRect& intersect)
{
  return m_latLonRect.Intersect(clip, intersect);
}
//----------------------------------------
void CCriteriaLatLon::SetDefaultValue()
{
  m_latLonRect.SetDefaultValue();
}

//----------------------------------------
bool CCriteriaLatLon::IsDefaultValue()
{
  return m_latLonRect.IsDefaultValue();
}


//----------------------------------------
void CCriteriaLatLon::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  CCriteria::Dump(fOut);

  fOut << "==> Dump a CCriteriaLatLon Object at "<< this << std::endl;
  m_latLonRect.Dump(fOut);
  fOut << "==> END Dump a CCriteriaLatLon Object at "<< this << std::endl;

  fOut << std::endl;

}

} // end namespace
