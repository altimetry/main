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
#include <cstdlib>
#include <cstdio>
#include <cstring> 
#include <typeinfo> 
#include <string> 

#include "new-gui/Common/tools/brathl_error.h" 
#include "brathl.h" 
//#define BRAT_INTERNAL

#include "new-gui/Common/tools/TraceLog.h" 
#include "Tools.h" 
#include "new-gui/Common/tools/Exception.h" 

#include "LatLonPoint.h"

namespace brathl
{

const char* CLatLonPoint::m_DEFAULT_UNIT_LONGITUDE = "degrees_east";
const char* CLatLonPoint::m_DEFAULT_UNIT_LATITUDE = "degrees_north";
const double CLatLonPoint::m_LONGITUDE_COMPARE_EPSILON = 1.0E-4;

//-------------------------------------------------------------
//------------------- CLatLonPoint class --------------------
//-------------------------------------------------------------
CLatLonPoint::CLatLonPoint() 
{
  setDefaultValue(m_lat);
  setDefaultValue(m_lon);
}

//----------------------------------------
CLatLonPoint::CLatLonPoint(CLatLonPoint &pt) 
{
  SetLatitude(pt.GetLatitude()); 
  SetLongitude(pt.GetLongitude()); 
}

//----------------------------------------
CLatLonPoint::CLatLonPoint(double lat,double lon) 
{
  SetLatitude(lat); 
  SetLongitude(lon); 
}
//----------------------------------------
CLatLonPoint::~CLatLonPoint() 
{
}
//----------------------------------------
const CLatLonPoint& CLatLonPoint::operator =(CLatLonPoint& pt)
{
  SetLatitude(pt.GetLatitude()); 
  SetLongitude(pt.GetLongitude()); 
  return *this;    
}

//----------------------------------------
bool CLatLonPoint::BetweenLon(double lon, double lonBeg, double lonEnd) 
{
  lonBeg = CLatLonPoint::LonNormal( lonBeg, lon); 
  lonEnd = CLatLonPoint::LonNormal( lonEnd, lon); 

  return ( lon >= lonBeg)&&( lon <= lonEnd);

}

//----------------------------------------
double CLatLonPoint::Range180(double lon) 
{

  return CLatLonPoint::LonNormal(lon);

}
//----------------------------------------

double CLatLonPoint::LonNormal360(double lon) 
{

  return CLatLonPoint::LonNormal(lon, 180.0 );
  //return CTools::NormalizeLongitude(0, lon);

}

//----------------------------------------
double CLatLonPoint::LonNormal360(double value, CUnit* unitIn)
{

  if (unitIn == NULL)
  {
    return CLatLonPoint::LonNormal360(value);
  }

  CUnit unit(CLatLonPoint::m_DEFAULT_UNIT_LONGITUDE);
  unit.SetConversionFrom(*unitIn);
  
  value = unit.Convert(value);

  value = CLatLonPoint::LonNormal360(value);
  
  unit	= unitIn->BaseUnit();
  unit.SetConversionTo(*unitIn);
  
  value	= unit.Convert(value);
  
  return value;

}
//----------------------------------------
double CLatLonPoint::LonNormalFrom(double lon, double from, CUnit* unitIn) 
{
  CUnit unit(CLatLonPoint::m_DEFAULT_UNIT_LONGITUDE);
  unit.SetConversionTo(*unitIn);
  
  double value = unit.Convert(180);

  return CLatLonPoint::LonNormal(lon, from + value, unitIn);

}

//----------------------------------------
double CLatLonPoint::LonNormalFrom(double lon, double from) 
{
  return CLatLonPoint::LonNormal(lon, from + 180.0);

}
//----------------------------------------
double CLatLonPoint::LonNormal(double lon, double center) 
{

  return CTools::NormalizeLongitude(center - 180, lon);
  //return center + CTools::Mod((lon - center),  360.0);

}
//----------------------------------------
double CLatLonPoint::LonNormal(double lon, double center, CUnit* unitIn) 
{

  if (unitIn == NULL)
  {
    return CLatLonPoint::LonNormal(lon, center);
  }

  CUnit unit(CLatLonPoint::m_DEFAULT_UNIT_LONGITUDE);
  unit.SetConversionFrom(*unitIn);
  
  lon = unit.Convert(lon);
  center = unit.Convert(center);

  lon = CLatLonPoint::LonNormal(lon, center);
  
  unit	= unitIn->BaseUnit();
  unit.SetConversionTo(*unitIn);
  
  lon	= unit.Convert(lon);
  
  return lon;

}
//----------------------------------------
double CLatLonPoint::LonNormal(double lon) 
{

  if (( lon < -180. ) || ( lon > 180. ))
  {
    return CLatLonPoint::LonNormal(lon, 0);
  }

  return lon;

}
//----------------------------------------
double CLatLonPoint::LatNormal(double lat, CUnit* unitIn) 
{
  if (unitIn == NULL)
  {
    return CLatLonPoint::LatNormal(lat);
  }

  CUnit unit(CLatLonPoint::m_DEFAULT_UNIT_LATITUDE);
  unit.SetConversionFrom(*unitIn);
  
  lat = unit.Convert(lat);

  lat = CLatLonPoint::LatNormal(lat);
  
  unit	= unitIn->BaseUnit();
  unit.SetConversionTo(*unitIn);
  
  lat	= unit.Convert(lat);
  
  return lat;

}

//----------------------------------------
double CLatLonPoint::LatNormal(double lat) 
{

  if (lat < -90.) 
  {
    return -90. ;
  } 

  if (lat > 90.)
  {
    return 90. ;
  }

  return lat;

}
 
//----------------------------------------
void CLatLonPoint::Set(CLatLonPoint& pt)
{
  SetLongitude(pt.GetLongitude()); 
  SetLatitude(pt.GetLatitude()); 

}
//----------------------------------------
void CLatLonPoint::Set(double lat,double lon) 
{
  SetLongitude(lon); 
  SetLatitude(lat); 

}

//----------------------------------------
void CLatLonPoint::Set(float lat,float lon) 
{
  SetLongitude(lon); 
  SetLatitude(lat); 

}

//----------------------------------------
void CLatLonPoint::SetLongitude(double lon) 
{
  if (isDefaultValue(lon) )
  {
    setDefaultValue(m_lon);
  }
  else
  {
    m_lon = CLatLonPoint::LonNormal(lon); 
  }
}

//----------------------------------------
void CLatLonPoint::SetLatitude(double lat) 
{
  if (isDefaultValue(lat))
  {
    setDefaultValue(m_lat);
  }
  else
  {
    m_lat = CLatLonPoint::LatNormal(lat); 
  }
}

//----------------------------------------
void CLatLonPoint::SetDefaultValue()
{
  setDefaultValue(m_lat);
  setDefaultValue(m_lon);
}


//----------------------------------------
/*
bool CLatLonPoint::Equals(CObject& obj) 
{
  if (typeid(obj) != typeid(CLatLonPoint))
  {
    return false;
  }

  CLatLonPoint &that = static_cast<CLatLonPoint&>(obj);

  return ( ( m_lat == that.GetLatitude()) && (m_lon== that.GetLongitude()) );

}
*/
//----------------------------------------
bool CLatLonPoint::Equals(CLatLonPoint& pt) 
{
  bool lonOk = CloseEnough(pt.GetLongitude(), m_lon);

  if (!lonOk) 
  {
    lonOk = CloseEnough(CLatLonPoint::LonNormal360(pt.GetLongitude()), CLatLonPoint::LonNormal360( m_lon)); 
  }

  return (lonOk && CloseEnough(pt.GetLatitude(), m_lat));

}
//----------------------------------------
bool CLatLonPoint::IsDefaultValue()
{
  return ((isDefaultValue(m_lat) == true) ||
          (isDefaultValue(m_lon) == true));

}

//----------------------------------------
bool CLatLonPoint::CloseEnough(double d1, double d2) 
{

  if (d1 != 0.0) 
  {
    return CTools::Abs((d1 - d2) / d1) < 1.0e-9;
  }
  if (d2 != 0.0)
  {
    return CTools::Abs((d1 - d2) / d2) < 1.0e-9;
  }

  return true;

}
//-------------------------------------------------------------
void CLatLonPoint::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }


  fOut << "==> Dump a CLatLonPoint Object at "<< this << std::endl;

  fOut << "m_lat = " << m_lat << std::endl;
  fOut << "m_lon = " << m_lon << std::endl;
  
  fOut << "==> END Dump a CLatLonPoint Object at "<< this << std::endl;

  fOut << std::endl;

}

} // end namespace
