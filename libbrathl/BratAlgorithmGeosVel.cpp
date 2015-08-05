
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
#include "brathl.h" 


#include "TraceLog.h" 
#include "Exception.h" 
#include "Mission.h" 
#include "Expression.h" 
#include "LatLonRect.h" 

#include "BratAlgorithmGeosVel.h" 

// When debugging changes all calls to “new” to be calls to “DEBUG_NEW” allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

using namespace brathl;



namespace brathl
{

  const string CBratAlgorithmGeosVel::m_LAT_PARAM_NAME = "%{lat}";
  const string CBratAlgorithmGeosVel::m_LON_PARAM_NAME = "%{lon}";

//-------------------------------------------------------------
//------------------- CBratAlgorithmGeosVel class --------------------
//-------------------------------------------------------------

CBratAlgorithmGeosVel::CBratAlgorithmGeosVel()
{
  Init();

}
//----------------------------------------
CBratAlgorithmGeosVel::CBratAlgorithmGeosVel(const CBratAlgorithmGeosVel &copy)
{
  Init();

  Set(copy);

}

//----------------------------------------
CBratAlgorithmGeosVel::~CBratAlgorithmGeosVel()
{

}
//----------------------------------------
void CBratAlgorithmGeosVel::Init()
{
  m_earthRadius = CMission::GetGlobalConstant(EARTH_RADIUS);
  m_gravity = CMission::GetGlobalConstant(EARTH_GRAVITY);
  m_omega = CTools::Deg2Rad(CMission::GetGlobalConstant(EARTH_ROTATION));
  m_beta = -m_gravity * m_earthRadius * 1000.0 / (2.0 * m_omega);
  m_degreeToRadianMutiplier = M_PI / 180.0;
  m_p2 = m_degreeToRadianMutiplier * m_earthRadius;

  CTools::SetDefaultValue(m_lat);
  CTools::SetDefaultValue(m_lon);

  SetBeginOfFile();
}

//----------------------------------------
void CBratAlgorithmGeosVel::Set(const CBratAlgorithmGeosVel &o)
{
  CBratAlgorithmBase::Set(o);

  m_equatorTransition = o.m_equatorTransition;
  m_equatorTransitionIsNext = o.m_equatorTransitionIsNext;
  m_lat = o.m_lat;
  m_lon = o.m_lon;
  m_latPrev = o.m_latPrev;
  m_lonPrev = o.m_lonPrev;
  m_latNext = o.m_latNext;
  m_lonNext = o.m_lonNext;
  m_coriolis = o.m_coriolis;
  m_velocity = o.m_velocity;
  m_latArray = NULL;
  m_lonArray = NULL;


}


//----------------------------------------
CBratAlgorithmGeosVel& CBratAlgorithmGeosVel::operator=(const CBratAlgorithmGeosVel &copy)
{
  if (this == &copy)
  {
    return *this;
  }

  Set(copy);

  return *this;
}


//----------------------------------------
void CBratAlgorithmGeosVel::EtoB(double lonPlane, double latPlane, double lon, double lat, double& betaX, double& betaY)
{
  if ((latPlane > CLatLonRect::LATITUDE_MAX) || (latPlane < CLatLonRect::LATITUDE_MIN))
  {
    throw CAlgorithmException(CTools::Format("Error in CBratAlgorithmGeosVel::EtoB - Invalid latPlane value: %f - Valid range is [%f, %f]", latPlane, CLatLonRect::LATITUDE_MIN, CLatLonRect::LATITUDE_MAX), this->GetName(), BRATHL_LOGIC_ERROR);
  }

  if ((lat > CLatLonRect::LATITUDE_MAX) || (lat < CLatLonRect::LATITUDE_MIN))
  {
    throw CAlgorithmException(CTools::Format("Error in CBratAlgorithmGeosVel::EtoB - Invalid lat value: %f - Valid range is [%f, %f]", latPlane, CLatLonRect::LATITUDE_MIN, CLatLonRect::LATITUDE_MAX), this->GetName(), BRATHL_LOGIC_ERROR);
  }

  double lonPlaneTemp = CTools::NormalizeLongitude(0, lonPlane); 
  double lonTemp = CTools::NormalizeLongitude(0, lon); 

  betaX = lonTemp - lonPlaneTemp;
  if (betaX > 180.0) 
  {
    betaX -=360.0;
  } 
  else if (betaX < -180.0)
  {
    betaX +=360.0;
  } 
  else if (CTools::Abs(betaX - 180.0) < CTools::m_CompareEpsilon)
  {
    betaX = CTools::Abs(betaX);
  }
  
  betaX = m_p2 * betaX * CTools::Cos(m_degreeToRadianMutiplier * latPlane);
  betaY = m_p2 * (lat - latPlane);

}

//----------------------------------------
void CBratAlgorithmGeosVel::BtoE(double lonPlane, double latPlane, double betaX, double betaY, double& lon, double& lat)
{
  if ((latPlane > CLatLonRect::LATITUDE_MAX) || (latPlane < CLatLonRect::LATITUDE_MIN))
  {
    throw CAlgorithmException(CTools::Format("Error in CBratAlgorithmGeosVel::BtoE - Invalid latPlane value: %f - Valid range is [%f, %f]", latPlane, CLatLonRect::LATITUDE_MIN, CLatLonRect::LATITUDE_MAX), this->GetName(), BRATHL_LOGIC_ERROR);
  }

  double lonPlaneTemp = CTools::NormalizeLongitude(0, lonPlane); 

  lon = lonPlaneTemp + betaX / (m_p2 * CTools::Cos(m_degreeToRadianMutiplier * latPlane));

  if (lon > 360.0) 
  {
    lon -= 360.0;
  }

  if (lon < 0.0) 
  {
    lon += 360.0;
  }

  lat = latPlane + (betaY / m_p2);

  if ((lat > CLatLonRect::LATITUDE_MAX) || (lat < CLatLonRect::LATITUDE_MIN))
  {
    throw CAlgorithmException(CTools::Format("Error in CBratAlgorithmGeosVel::BtoE - Invalid lat value: %f - Valid range is [%f, %f]", latPlane, CLatLonRect::LATITUDE_MIN, CLatLonRect::LATITUDE_MAX), this->GetName(), BRATHL_LOGIC_ERROR);
  }
}

//----------------------------------------
void CBratAlgorithmGeosVel::SetBeginOfFile()
{
  CBratAlgorithmBase::SetBeginOfFile();

  m_equatorTransitionIsNext = false;
  m_equatorTransition = false;

  CTools::SetDefaultValue(m_latPrev);
  CTools::SetDefaultValue(m_lonPrev);   
  CTools::SetDefaultValue(m_latNext);
  CTools::SetDefaultValue(m_lonNext);   
  CTools::SetDefaultValue(m_coriolis);
  CTools::SetDefaultValue(m_velocity);

  m_latArray = NULL;
  m_lonArray = NULL;


}

//----------------------------------------
void CBratAlgorithmGeosVel::SetEndOfFile()
{
  CBratAlgorithmBase::SetEndOfFile();

  CTools::SetDefaultValue(m_latNext);
  CTools::SetDefaultValue(m_lonNext);
  
  m_equatorTransitionIsNext = false;

}
//----------------------------------------
void CBratAlgorithmGeosVel::SetPreviousValues(bool fromProduct) 
{
  CBratAlgorithmBase::SetPreviousValues(fromProduct);

  m_latPrev = m_lat;
  m_lonPrev = m_lon;

}
//----------------------------------------
void CBratAlgorithmGeosVel::SetNextValues() 
{
  CBratAlgorithmBase::SetNextValues();
}

//----------------------------------------
void CBratAlgorithmGeosVel::ComputeCoriolis() 
{
  if (CTools::IsDefaultValue(m_lon) || CTools::IsDefaultValue(m_lat))
  {
    CTools::SetDefaultValue(m_coriolis);
  }
  else
  {
    m_coriolis	= - m_gravity / (2.0 * m_omega * CTools::Sin( CTools::Deg2Rad(m_lat) ));
  }
}

//----------------------------------------
void CBratAlgorithmGeosVel::Dump(ostream& fOut /*= cerr*/)
{
   if (CTrace::IsTrace() == false)
   { 
      return;
   }

  fOut << "==> Dump a CBratAlgorithmGeosVel Object at "<< this << endl;
  CBratAlgorithmBase::Dump(fOut);

  fOut << "m_earthRadius: " << m_earthRadius << endl;
  fOut << "m_gravity: " << m_gravity << endl;
  fOut << "m_omega: " << m_omega << endl;
  fOut << "m_beta: " << m_beta << endl;
  fOut << "m_degreeToRadianMutiplier: " << m_degreeToRadianMutiplier << endl;
  fOut << "m_p2: " << m_p2 << endl;
  fOut << "m_equatorTransition: " << m_equatorTransition << endl;
  fOut << "m_equatorTransitionIsNext: " << m_equatorTransitionIsNext << endl;
  fOut << "m_lat: " << m_lat << endl;
  fOut << "m_lon: " << m_lon << endl;
  fOut << "m_latPrev: " << m_latPrev << endl;
  fOut << "m_lonPrev: " << m_lonPrev << endl;
  fOut << "m_latNext: " << m_latNext << endl;
  fOut << "m_lonNext: " << m_latNext << endl;
  fOut << "m_coriolis: " << m_coriolis << endl;
  fOut << "m_velocity: " << m_velocity << endl;
  fOut << "==> END Dump a CBratAlgorithmGeosVel Object at "<< this << endl;

}


} // end namespace
