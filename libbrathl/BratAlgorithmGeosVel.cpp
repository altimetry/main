
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


#include "common/tools/TraceLog.h"
#include "common/tools/Exception.h"
#include "Expression.h" 
#include "LatLonRect.h" 

#include "BratAlgorithmGeosVel.h" 

// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

using namespace brathl;


namespace brathl
{

	enum  brathl_global_constants
	{
		EARTH_ROTATION = 0,
		LIGHT_SPEED,
		EARTH_GRAVITY,
		EARTH_RADIUS,
		ELLIPSOID_PARAM
	};


	static double GetGlobalConstant( brathl_global_constants constantValue )
	{
		switch (constantValue)
		{
			case EARTH_ROTATION:
				return 360.0 / 86400.0;

			case LIGHT_SPEED:
				return 299792.458;

			case EARTH_GRAVITY:
				return 9.807;

			case EARTH_RADIUS:
				return 6378.1363;

			case ELLIPSOID_PARAM:
				return 298.257;
				/*
				case EARTH_RADIUS:
				switch (Inv_IdMission)
				{
				case E_MIS_ERS1:
				case E_MIS_ERS2:
				case E_MIS_Envisat:
				case E_MIS_ENN:
				case E_MIS_Geosat:
				case E_MIS_Cryosat1:
				case E_MIS_Icesat1:
				return 6378.137;

				case E_MIS_TP:
				case E_MIS_TPN:
				case E_MIS_Topex:
				case E_MIS_Poseidon:
				case E_MIS_Jason1:
				case E_MIS_J1N:
				case E_MIS_Jason2:
				case E_MIS_GFO:
				return 6378.1363;

				case E_MIS_Altika:
				abort();

				case E_MIS_inconnue:
				return dc_SUDV_DefReal8;

				default:
				assert(0);
				}
				break;

				case ELLIPSOID_PARAM:
				switch (Inv_IdMission)
				{
				case E_MIS_ERS1:
				case E_MIS_ERS2:
				case E_MIS_Envisat:
				case E_MIS_ENN:
				case E_MIS_Geosat:
				case E_MIS_Cryosat1:
				case E_MIS_Icesat1:
				return 298.257223563;

				case E_MIS_TP:
				case E_MIS_TPN:
				case E_MIS_Topex:
				case E_MIS_Poseidon:
				case E_MIS_Jason1:
				case E_MIS_J1N:
				case E_MIS_Jason2:
				case E_MIS_GFO:
				return 298.257;

				case E_MIS_Altika:
				abort();

				case E_MIS_inconnue:
				return dc_SUDV_DefReal8;

				default:
				assert(0);
				}
				*/
			default:
				CException e(CTools::Format("GetGeneralConstant - invalid/unknown input constant '%d'", constantValue),
					BRATHL_LOGIC_ERROR);
				throw (e);
		}
		CException e(CTools::Format("GetGeneralConstant - invalid/unknown input constant '%d'", constantValue),
			BRATHL_LOGIC_ERROR);
		throw (e);
		return 0;
	}



  const std::string CBratAlgorithmGeosVel::m_LAT_PARAM_NAME = "%{lat}";
  const std::string CBratAlgorithmGeosVel::m_LON_PARAM_NAME = "%{lon}";

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
  m_earthRadius = GetGlobalConstant(EARTH_RADIUS);
  m_gravity = GetGlobalConstant(EARTH_GRAVITY);
  m_omega = CTools::Deg2Rad(GetGlobalConstant(EARTH_ROTATION));
  m_beta = -m_gravity * m_earthRadius * 1000.0 / (2.0 * m_omega);
  m_degreeToRadianMutiplier = M_PI / 180.0;
  m_p2 = m_degreeToRadianMutiplier * m_earthRadius;

  setDefaultValue(m_lat);
  setDefaultValue(m_lon);

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

  setDefaultValue(m_latPrev);
  setDefaultValue(m_lonPrev);   
  setDefaultValue(m_latNext);
  setDefaultValue(m_lonNext);   
  setDefaultValue(m_coriolis);
  setDefaultValue(m_velocity);

  m_latArray = NULL;
  m_lonArray = NULL;


}

//----------------------------------------
void CBratAlgorithmGeosVel::SetEndOfFile()
{
  CBratAlgorithmBase::SetEndOfFile();

  setDefaultValue(m_latNext);
  setDefaultValue(m_lonNext);
  
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
  if (isDefaultValue(m_lon) || isDefaultValue(m_lat))
  {
    setDefaultValue(m_coriolis);
  }
  else
  {
    m_coriolis	= - m_gravity / (2.0 * m_omega * CTools::Sin( CTools::Deg2Rad(m_lat) ));
  }
}

//----------------------------------------
void CBratAlgorithmGeosVel::Dump(std::ostream& fOut /*= std::cerr*/)
{
   if (CTrace::IsTrace() == false)
   { 
      return;
   }

  fOut << "==> Dump a CBratAlgorithmGeosVel Object at "<< this << std::endl;
  CBratAlgorithmBase::Dump(fOut);

  fOut << "m_earthRadius: " << m_earthRadius << std::endl;
  fOut << "m_gravity: " << m_gravity << std::endl;
  fOut << "m_omega: " << m_omega << std::endl;
  fOut << "m_beta: " << m_beta << std::endl;
  fOut << "m_degreeToRadianMutiplier: " << m_degreeToRadianMutiplier << std::endl;
  fOut << "m_p2: " << m_p2 << std::endl;
  fOut << "m_equatorTransition: " << m_equatorTransition << std::endl;
  fOut << "m_equatorTransitionIsNext: " << m_equatorTransitionIsNext << std::endl;
  fOut << "m_lat: " << m_lat << std::endl;
  fOut << "m_lon: " << m_lon << std::endl;
  fOut << "m_latPrev: " << m_latPrev << std::endl;
  fOut << "m_lonPrev: " << m_lonPrev << std::endl;
  fOut << "m_latNext: " << m_latNext << std::endl;
  fOut << "m_lonNext: " << m_latNext << std::endl;
  fOut << "m_coriolis: " << m_coriolis << std::endl;
  fOut << "m_velocity: " << m_velocity << std::endl;
  fOut << "==> END Dump a CBratAlgorithmGeosVel Object at "<< this << std::endl;

}


} // end namespace
