#include <cstdlib>
#include <cstdio>
#include <cstring> 

#include "brathl_error.h" 
#include "brathl.h" 
//#define BRAT_INTERNAL

#include <string> 

#include "TraceLog.h" 
#include "Tools.h" 
#include "Exception.h" 

#include "LatLonRect.h"

namespace brathl
{
//-------------------------------------------------------------
//------------------- CLatLonRect class --------------------
//-------------------------------------------------------------

const std::string CLatLonRect::LATITUDE_MIN_STR = "-90";
const std::string CLatLonRect::LATITUDE_MAX_STR = "90";
const std::string CLatLonRect::LONGITUDE_MIN_STR = "-180";
const std::string CLatLonRect::LONGITUDE_MAX_STR = "180";

const double CLatLonRect::LATITUDE_MIN = -90.0;
const double CLatLonRect::LATITUDE_MAX = 90.0;
const double CLatLonRect::LONGITUDE_MIN = -180.0;
const double CLatLonRect::LONGITUDE_MAX = 180.0;

const std::string CLatLonRect::m_delimiter = " ";

//-------------------------------------------------------------

CLatLonRect::CLatLonRect() 
{
  Init();
  CLatLonPoint pt(CLatLonRect::LATITUDE_MIN , CLatLonRect::LONGITUDE_MIN ); 
  Set(pt, 180 , 360 ); 
}

//-------------------------------------------------------------
CLatLonRect::CLatLonRect(CLatLonPoint& p1, double deltaLat, double deltaLon) 
{

  Init();
  Set(p1, deltaLat, deltaLon);

}
//-------------------------------------------------------------

CLatLonRect::CLatLonRect(CLatLonPoint& left, CLatLonPoint& right) 
{
  Init();
  Set(left, right);
}

//-------------------------------------------------------------

CLatLonRect::CLatLonRect(CLatLonRect& r) 
{
  Init();
  Set(r);
}

//-------------------------------------------------------------
CLatLonRect::CLatLonRect(CLatLonRect* r) 
{
  Init();
  Set(*r);
}

//----------------------------------------
CLatLonRect::~CLatLonRect() 
{

}
//-------------------------------------------------------------
CLatLonRect::CLatLonRect(double latLow, double lonLow, double latHigh, double lonHigh)
{
  Init();
  Set(latLow, lonLow, latHigh, lonHigh);
}
//-------------------------------------------------------------
CLatLonRect::CLatLonRect(const std::string& latLow, const std::string& lonLow, const std::string& latHigh, const std::string& lonHigh) 
{
  Init();
  Set(latLow, lonLow, latHigh, lonHigh);

}


//-------------------------------------------------------------

void CLatLonRect::Init() 
{
  
  SetDefaultValue();

  m_crossDateline = false;
  m_allLongitude = false;

}


//-------------------------------------------------------------
void CLatLonRect::Reset()
{
  CLatLonRect latLonRect;

  *this = latLonRect;
}

//-------------------------------------------------------------
void CLatLonRect::GetAsString(CStringArray& array)
{
  array.Insert(CTools::Format(30, FORMAT_FLOAT_LATLON.c_str(), m_lowerLeft.GetLatitude()));
  array.Insert(CTools::Format(30, FORMAT_FLOAT_LATLON.c_str(), m_lowerLeft.GetLongitude()));
  array.Insert(CTools::Format(30, FORMAT_FLOAT_LATLON.c_str(), m_upperRight.GetLatitude()));
  array.Insert(CTools::Format(30, FORMAT_FLOAT_LATLON.c_str(), m_upperRight.GetLongitude()));
}
//-------------------------------------------------------------
std::string CLatLonRect::GetAsText(const std::string& delimiter /* = CLatLonRect::m_delimiter*/)
{
  std::string format = FORMAT_FLOAT_LATLON;
  format.append(delimiter);
  format.append(FORMAT_FLOAT_LATLON);
  format.append(delimiter);
  format.append(FORMAT_FLOAT_LATLON);
  format.append(delimiter);
  format.append(FORMAT_FLOAT_LATLON);

  return CTools::Format(format.c_str(), m_lowerLeft.GetLatitude(),
                         m_lowerLeft.GetLongitude(),
                         m_upperRight.GetLatitude(),
                         m_upperRight.GetLongitude());
}

//-------------------------------------------------------------
CLatLonPoint CLatLonRect::GetUpperLeftPoint()
{
  CLatLonPoint latLonPoint(m_upperRight.GetLatitude(), m_lowerLeft.GetLongitude()); 
  return  latLonPoint;
}
//-------------------------------------------------------------
CLatLonPoint CLatLonRect::GetLowerRightPoint()
{ 
  CLatLonPoint latLonPoint(m_lowerLeft.GetLatitude(), m_upperRight.GetLongitude());
  return latLonPoint;
}

//-------------------------------------------------------------
/*
void CLatLonRect::DeleteLatLonPoint() 
{
  if (m_lowerLeft != NULL) 
  {
    delete m_lowerLeft;
    m_lowerLeft = NULL;
  }
  if (m_upperRight != NULL) 
  {
    delete m_upperRight;
    m_upperRight = NULL;
  }

}
*/

//----------------------------------------
const CLatLonRect& CLatLonRect::operator =(CLatLonRect& r)
{
  Set(r);    
  return *this;    
}

//-------------------------------------------------------------
void CLatLonRect::Set(CLatLonRect& r) 
{
  Set(r.GetLowerLeftPoint(), r.GetUpperRightPoint().GetLatitude() - r.GetLowerLeftPoint().GetLatitude(), r.GetWidth()); 
}
//-------------------------------------------------------------
void CLatLonRect::Set(const std::string& latLonRect, const std::string& delimiter /* = CLatLonRect::m_delimiter */)
{
  CStringArray array;

  array.ExtractStrings(latLonRect, delimiter);

  Set(array);


}
//-------------------------------------------------------------
void CLatLonRect::Set(const CStringArray& array)
{
  switch (array.size()) {
  case 4:
      Set(array.at(0), array.at(1), array.at(2), array.at(3));
      break;
  case 3:
      Set(array.at(0), array.at(1), array.at(2), CLatLonRect::LONGITUDE_MAX_STR);
      break;
  case 2:
      Set(array.at(0), array.at(1), CLatLonRect::LATITUDE_MAX_STR, CLatLonRect::LONGITUDE_MAX_STR);
      break;
  case 1:
      Set(array.at(0), CLatLonRect::LONGITUDE_MIN_STR, CLatLonRect::LATITUDE_MAX_STR, CLatLonRect::LONGITUDE_MAX_STR);
      break;
  case 0:
      // set a LatLonRect that covers the whole world.
      Reset();
      break;
  default:
      Set(array.at(0), array.at(1), array.at(2), array.at(3));
      break;
  }
    

}
//-------------------------------------------------------------

void CLatLonRect::Set(CLatLonPoint& left, CLatLonPoint& right) 
{
  if (left.IsDefaultValue() && right.IsDefaultValue())
  {
    SetDefaultValue();
    return;
  }
  if (!left.IsDefaultValue() && right.IsDefaultValue())
  {
    double lat = ( CTools::IsDefaultValue(right.GetLatitude()) ? 90 : right.GetLatitude());
    right.Set(lat, 360 + left.GetLongitude() - 0.001);
  }
  if (left.IsDefaultValue() && !right.IsDefaultValue())
  {
    double lat = ( CTools::IsDefaultValue(left.GetLatitude()) ? -90 : left.GetLatitude());
    left.Set(lat, right.GetLongitude() - 0.001);
  }

  double deltaLon = 0.0;

  if (right.GetLongitude() >= left.GetLongitude())
  {
    deltaLon = right.GetLongitude() - left.GetLongitude();
  }
  else
  {
    deltaLon  = 360 - CTools::Abs(left.GetLongitude() - right.GetLongitude());
  }

  Set(left, right.GetLatitude() - left.GetLatitude(), 
    CLatLonPoint::LonNormal360(deltaLon)); 
}

//-------------------------------------------------------------
void CLatLonRect::Set(CLatLonPoint& p1, double deltaLat, double deltaLon) 
{
  if (p1.IsDefaultValue())
  {
    SetDefaultValue();
    return;
  }

  double lonmax;
  double lonmin; 

  double latmin = CTools::Min(p1.GetLatitude(), p1.GetLatitude() + deltaLat);
  double latmax = CTools::Max(p1.GetLatitude(), p1.GetLatitude() + deltaLat);
 
  double lonpt = p1.GetLongitude();

  if (deltaLon > 0)  
  {
    lonmin = lonpt; 
    lonmax = lonpt+ deltaLon; 
    m_crossDateline = (lonmax > 180.0 ); 
  }
  else
  {
    lonmax = lonpt; 
    lonmin = lonpt+ deltaLon; 
    m_crossDateline = lonmin<180.0 ; 
  }

// DeleteLatLonPoint();

 m_lowerLeft.Set(latmin, lonmin);
 m_upperRight.Set(latmax, lonmax);
 
 // these are an alternative way to view the longitude range
 m_width = CTools::Abs(deltaLon);
 m_lon0 = CLatLonPoint::LonNormal(p1.GetLongitude() + (deltaLon/2));
 m_allLongitude = (m_width >= 360.0);
 
}



//-------------------------------------------------------------
void CLatLonRect::Set(double latLow, double lonLow , double latHigh , double lonHigh)
{
  if (  CTools::IsDefaultValue(latLow) &&
        CTools::IsDefaultValue(lonLow) &&
        CTools::IsDefaultValue(latHigh) &&
        CTools::IsDefaultValue(lonHigh)   )
  {
    SetDefaultValue();
  }

  if (!(CTools::IsDefaultValue(lonLow)) && (CTools::IsDefaultValue(lonHigh)))
  {
    lonHigh = 360 + lonLow - 0.001;
  }
  if ((CTools::IsDefaultValue(lonLow)) && !(CTools::IsDefaultValue(lonHigh)))
  {
    lonLow = lonHigh - 0.001;
  }

  if (CTools::IsDefaultValue(latLow))
  {
    latLow = CLatLonRect::LATITUDE_MIN;
  }
  if (CTools::IsDefaultValue(lonLow))
  {
    lonLow = CLatLonRect::LONGITUDE_MIN;
  }
  if (CTools::IsDefaultValue(latHigh))
  {
    latHigh = CLatLonRect::LATITUDE_MAX;
  }
  if (CTools::IsDefaultValue(lonHigh))
  {
    lonHigh = CLatLonRect::LONGITUDE_MAX;
  }

  // to fix problem with CLatLonRect width computation
  if (CTools::Abs(lonHigh - lonLow) >= 360.0) 
  {
    CLatLonPoint p1(latLow, LONGITUDE_MIN);
    CLatLonPoint p2(latHigh, LONGITUDE_MAX);
  
    Set(p1, p2);
  }
  else
  {
    CLatLonPoint p1(latLow, lonLow);
    CLatLonPoint p2(latHigh, lonHigh);
   
    Set(p1, p2);
  }

}
//-------------------------------------------------------------
void CLatLonRect::Set(const std::string& latLow, const std::string& lonLow, const std::string& latHigh, const std::string& lonHigh) 
{
// to fix problem with LatLonrect width computation : unconvert longitude with no normalization here.
  Set(CTools::UnconvertLat(latLow), CTools::UnconvertLon(lonLow, false), CTools::UnconvertLat(latHigh),
                CTools::UnconvertLon(lonHigh, false));

}


//-------------------------------------------------------------
bool CLatLonRect::Contains(double lat, double lon) 
{
  if (IsDefaultValue())
  {
    return false;
  }

  // check lat first
  if ((lat < m_lowerLeft.GetLatitude())
          || (lat > m_upperRight.GetLatitude())) 
  {
    return false;
  }

  if (m_allLongitude) 
  {
    return true;
  }

  if (m_crossDateline)
  {
    double lonMax = m_upperRight.GetLongitude();
    if (lonMax > 180)
    {
      lonMax = -180 + (lonMax - 180);
    }
    // bounding box crosses the +/- 180 seam
    return ((lon >= m_lowerLeft.GetLongitude())
            || (lon <= lonMax));
  }
  else
  {
    // check "normal" lon case
    return ((lon >= m_lowerLeft.GetLongitude())
            && (lon <= m_upperRight.GetLongitude()));
  }
}

//-------------------------------------------------------------

void CLatLonRect::Extend(CLatLonPoint& p) 
{
    if (IsDefaultValue())
    {
      Set(p, p);
      return;
    }

    if (Contains(p)) {
      return;
    }

    double lat = p.GetLatitude();
    double lon = p.GetLongitude();

    // lat is easy to deal with
    if (lat > m_upperRight.GetLatitude()) 
    {
      m_upperRight.SetLatitude(lat);
    }
    if (lat < m_lowerLeft.GetLatitude()) 
    {
      m_lowerLeft.SetLatitude(lat);
    }

    // lon is uglier
    if (m_allLongitude) 
    {
      ; // do nothing
    }
    else if (m_crossDateline) 
    {

      // bounding box crosses the +/- 180 seam
      double d1 = lon - m_upperRight.GetLongitude();
      double d2 = m_lowerLeft.GetLongitude() - lon;
      if ((d1 > 0.0) && (d2 > 0.0)) 
      {  // needed ?
        if (d1 > d2)
        {
          m_lowerLeft.SetLongitude(lon);
        }
        else 
        {
          m_upperRight.SetLongitude(lon);
        }
      }

    } 
    else 
    {

      // normal case
      if (lon > m_upperRight.GetLongitude()) {
        if (lon - m_upperRight.GetLongitude()
                > m_lowerLeft.GetLongitude() - lon + 360) {
          m_crossDateline = true;
          m_lowerLeft.SetLongitude(lon);
        } 
        else
        {
          m_upperRight.SetLongitude(lon);
        }
      } 
      else if (lon < m_lowerLeft.GetLongitude()) 
      {
        if (m_lowerLeft.GetLongitude() - lon
                > lon + 360.0 - m_upperRight.GetLongitude()) 
        {
          m_crossDateline = true;
          m_upperRight.SetLongitude(lon);
        } 
        else
        {
          m_lowerLeft.SetLongitude(lon);
        }
      }
    }

    // recalc delta, center
    m_width = m_upperRight.GetLongitude() - m_lowerLeft.GetLongitude();
    m_lon0 = (m_upperRight.GetLongitude() + m_lowerLeft.GetLongitude()) / 2;
    
    if (m_crossDateline) 
    {
      m_width += 360;
      m_lon0 -= 180;
    }

    m_allLongitude |= (m_width >= 360.0);
}

//-------------------------------------------------------------

void CLatLonRect::Extend(CLatLonRect& r) 
{
  if (IsDefaultValue())
  {
    Set(r);
    return;
  }

  // lat is easy
  double latMin = r.GetLatMin();
  double latMax = r.GetLatMax();

  if (latMax > m_upperRight.GetLatitude()) {
    m_upperRight.SetLatitude(latMax);
  }
  if (latMin < m_lowerLeft.GetLatitude()) {
    m_lowerLeft.SetLatitude(latMin);
  }

  // lon is uglier
  if (m_allLongitude)
  {
    return;
  }

  // everything is reletive to current LonMin
  double lonMin = GetLonMin();
  double lonMax = GetLonMax();

  double nlonMin = CLatLonPoint::LonNormal( r.GetLonMin(), lonMin);
  double nlonMax = nlonMin + r.GetWidth();
  lonMin = CTools::Min(lonMin, nlonMin);
  lonMax = CTools::Max(lonMax, nlonMax);

  m_width = lonMax - lonMin;
  m_allLongitude = m_width >= 360.0;
  if (m_allLongitude) 
  {
    m_width = 360.0;
    lonMin = -180.0;
  }
  else 
  {
    lonMin = CLatLonPoint::LonNormal(lonMin);
  }

  m_lowerLeft.SetLongitude(lonMin);
  m_upperRight.SetLongitude(lonMin + m_width);
  m_lon0 = lonMin + m_width / 2;
  m_crossDateline = m_lowerLeft.GetLongitude() > m_upperRight.GetLongitude();


}

//-------------------------------------------------------------

bool CLatLonRect::Intersect(CLatLonRect& clip, CLatLonRect& intersect) 
{
  double latMin = CTools::Max(GetLatMin(), clip.GetLatMin());
  double latMax = CTools::Min(GetLatMax(), clip.GetLatMax());
  double deltaLat = latMax - latMin;
  if (deltaLat < 0) 
  {
    return false;
  }

  // lon as always is a pain : if not intersection, try +/- 360
  double lon1min = GetLonMin();
  double lon1max = GetLonMax();
  double lon2min = clip.GetLonMin();
  double lon2max = clip.GetLonMax();

  if (!Intersect(lon1min, lon1max, lon2min, lon2max)) 
  {
     lon2min = clip.GetLonMin() + 360;
     lon2max = clip.GetLonMax() + 360;
     if (!Intersect(lon1min, lon1max, lon2min, lon2max)) 
     {
       lon2min = clip.GetLonMin() - 360;
       lon2max = clip.GetLonMax() - 360;
     }
  }

  // we did our best to find an intersection
  double lonMin = CTools::Max(lon1min, lon2min);
  double lonMax = CTools::Min(lon1max, lon2max);
  double deltaLon = lonMax - lonMin;
  if (deltaLon < 0) 
  {
    return false;
  }
  
  CLatLonPoint pt(latMin, lonMin);
  intersect.Set(pt, deltaLat, deltaLon);
  return true;

}

//-------------------------------------------------------------

bool CLatLonRect::Intersect(double min1,double max1,double min2,double max2) 
{
  double min = CTools::Max( min1, min2);
  double max = CTools::Min( max1, max2);

  return  (min < max);

}

//----------------------------------------
void CLatLonRect::SetDefaultValue()
{
  m_lowerLeft.SetDefaultValue();
  m_upperRight.SetDefaultValue();
}


//----------------------------------------
bool CLatLonRect::IsDefaultValue()
{
  return ((m_upperRight.IsDefaultValue()) ||
          (m_lowerLeft.IsDefaultValue()));

}
//----------------------------------------
std::string CLatLonRect::AsString(const std::string& format /*= ""*/)
{
  std::string theFormat;
  std::string formatRef = format;
  if (formatRef.empty())
  {
    formatRef = "%-#.5g";
  }
  theFormat = "Latitude min.: " + formatRef;
  std::string str = CTools::Format(theFormat.c_str(), GetLatMin());
  str.append("/");
  theFormat = "Longitude min.: " + formatRef;
  str.append(CTools::Format(theFormat.c_str(), GetLonMin()));
  str.append("/");
  theFormat = "Latitude max.: " + formatRef;
  str.append(CTools::Format(theFormat.c_str(), GetLatMax()));
  str.append("/");
  theFormat = "Longitude max.: " + formatRef;
  str.append(CTools::Format(theFormat.c_str(), GetLonMax()));

  return str;
}

//-------------------------------------------------------------
void CLatLonRect::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }


  fOut << "==> Dump a CLatLonRect Object at "<< this << std::endl;

  fOut << "m_allLongitude = " << m_allLongitude << std::endl;
  fOut << "m_crossDateline = " << m_crossDateline << std::endl;
  fOut << "m_lon0 = " << m_lon0 << std::endl;
  fOut << "m_width = " << m_width << std::endl;

  fOut << "m_lowerLeft at " << &m_lowerLeft << std::endl;
  m_lowerLeft.Dump(fOut);
  fOut << "m_upperRight at " << &m_upperRight << std::endl;
  m_upperRight.Dump(fOut);

  fOut << "==> END Dump a CLatLonRect Object at "<< this << std::endl;

  fOut << std::endl;

}

}
