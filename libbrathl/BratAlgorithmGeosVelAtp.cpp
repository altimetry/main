
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
#include "Product.h" 

#include "BratAlgorithmGeosVelAtp.h" 

// When debugging changes all calls to “new” to be calls to “DEBUG_NEW” allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

using namespace brathl;



namespace brathl
{

const uint32_t CBratAlgorithmGeosVelAtp::m_INPUT_PARAMS = 3;

const uint32_t CBratAlgorithmGeosVelAtp::m_LAT_PARAM_INDEX = 0;
const uint32_t CBratAlgorithmGeosVelAtp::m_LON_PARAM_INDEX = 1;
const uint32_t CBratAlgorithmGeosVelAtp::m_VAR_PARAM_INDEX = 2;

//-------------------------------------------------------------
//------------------- CBratAlgorithmGeosVelAtp class --------------------
//-------------------------------------------------------------

CBratAlgorithmGeosVelAtp::CBratAlgorithmGeosVelAtp()
{
  Init();

}
//----------------------------------------
CBratAlgorithmGeosVelAtp::CBratAlgorithmGeosVelAtp(const CBratAlgorithmGeosVelAtp &copy)
{
  Init();

  Set(copy);

}
//----------------------------------------
void CBratAlgorithmGeosVelAtp::Init()
{
  CTools::SetDefaultValue(m_varValue);
  SetBeginOfFile();
}

//----------------------------------------
void CBratAlgorithmGeosVelAtp::Set(const CBratAlgorithmGeosVelAtp &o)
{
  CBratAlgorithmGeosVel::Set(o);
  m_varValue = o.m_varValue;
  m_varValuePrev = o.m_varValuePrev;
  m_varValueNext = o.m_varValueNext;
  m_gap = o.m_gap;


}


//----------------------------------------
CBratAlgorithmGeosVelAtp& CBratAlgorithmGeosVelAtp::operator=(const CBratAlgorithmGeosVelAtp &copy)
{
  if (this == &copy)
  {
    return *this;
  }

  Set(copy);

  return *this;
}

//----------------------------------------
void CBratAlgorithmGeosVelAtp::SetBeginOfFile()
{
  CBratAlgorithmGeosVel::SetBeginOfFile();
  
  m_gap = 0.0;
  CTools::SetDefaultValue(m_varValuePrev);
  CTools::SetDefaultValue(m_varValueNext);

}

//----------------------------------------
void CBratAlgorithmGeosVelAtp::SetEndOfFile()
{
  CBratAlgorithmGeosVel::SetEndOfFile();

  CTools::SetDefaultValue(m_varValueNext);

}
//----------------------------------------
double CBratAlgorithmGeosVelAtp::Run(CVectorBratAlgorithmParam& args)
{
  int32_t iRecord = m_callerProduct->GetCurrentRecordNumber();

  if (iRecord == m_callerProductRecordPrev)
  {
    // Do nothing: data have been already computed
    return m_velocity;
  }

  // Gets the next record regarding to the current product record.
  // and save previous values.
  this->GetNextData();
  
  SetParamValues(args);

  SetGap();

  SetEquatorTransition();

  ComputeCoriolis();


  // Compute geostrophic velocity.
  ComputeVelocity();

  m_callerProductRecordPrev = iRecord;


  return m_velocity;
}
//----------------------------------------
void CBratAlgorithmGeosVelAtp::SetPreviousValues(bool fromProduct) 
{
  if (fromProduct)
  {
    m_latPrev = this->GetDataValue(CBratAlgorithmGeosVelAtp::m_LAT_PARAM_INDEX);
    m_lonPrev = this->GetDataValue(CBratAlgorithmGeosVelAtp::m_LON_PARAM_INDEX);
    m_varValuePrev = this->GetDataValue(CBratAlgorithmGeosVelAtp::m_VAR_PARAM_INDEX);
  }
  else
  {
    CBratAlgorithmGeosVel::SetPreviousValues(fromProduct);
    m_varValuePrev = m_varValue;
  }

}

//----------------------------------------
void CBratAlgorithmGeosVelAtp::SetNextValues() 
{
  m_latNext = this->GetDataValue(CBratAlgorithmGeosVelAtp::m_LAT_PARAM_INDEX);
  m_lonNext = this->GetDataValue(CBratAlgorithmGeosVelAtp::m_LON_PARAM_INDEX);
  m_varValueNext = this->GetDataValue(CBratAlgorithmGeosVelAtp::m_VAR_PARAM_INDEX);
}

//----------------------------------------
double CBratAlgorithmGeosVelAtp::ComputeVelocity() 
{
  Dump(*CTrace::GetDumpContext());

  if (m_equatorTransition)
  {
    return ComputeVelocityEquator();
  }
  else
  {
    return ComputeVelocityOutsideEquator();
  }
}

//----------------------------------------
double CBratAlgorithmGeosVelAtp::ComputeVelocityEquator() 
{
  CTools::SetDefaultValue(m_velocity);
  if (CTools::IsDefaultValue(m_varValuePrev) || CTools::IsDefaultValue(m_varValue) || CTools::IsDefaultValue(m_varValueNext))
  {
    return m_velocity;
  }

  // Compute distance on either side of the point closest to the equator

  double dxAfter;
  double dyAfter;

  this->EtoB(m_lon,
		     m_lat,
		     m_lonNext,
		     m_latNext,
		     dxAfter,
		     dyAfter);

  double dxBefore;
  double dyBefore;

	this->EtoB(m_lon,
		     m_lat,
		     m_lonPrev,
		     m_latPrev,
		     dxBefore,
		     dyBefore);

#if _DEBUG_BRAT_ALGO    
  CTrace::Tracer(3, "Algorithm '%s' - Compute velocity at equator: dxAfter=%f, dyAfter=%f, dxBefore=%f, dyBefore=%f,", this->GetName().c_str(),
             dxAfter, dyAfter, dxBefore, dyBefore);
#endif

  double dxAfterSqr = CTools::Sqr(dxAfter);
  double dyAfterSqr = CTools::Sqr(dyAfter);
  double dxBeforeSqr = CTools::Sqr(dxBefore);
  double dyBeforeSqr = CTools::Sqr(dyBefore);

  if (CTools::IsDefaultValue(dxAfterSqr) || CTools::IsDefaultValue(dyAfterSqr) || 
      CTools::IsDefaultValue(dxBeforeSqr) || CTools::IsDefaultValue(dyBeforeSqr))
  {
    return m_velocity;
  }

  double distance = (CTools::Sqrt(dxBeforeSqr + dyBeforeSqr)
			             + CTools::Sqrt(dxAfterSqr + dyAfterSqr))
                 * 1000.0;

  double distanceSqr = CTools::Sqr(distance);
  if (CTools::IsDefaultValue(distanceSqr))
  {
    return m_velocity;
  }

	m_velocity = (m_beta * ( (m_varValueNext + m_varValuePrev) - (2.0 * m_varValue) ) ) / distanceSqr;

#if _DEBUG_BRAT_ALGO    
  CTrace::Tracer(3, "Algorithm '%s' - Compute velocity at equator: distance=%f, velocity=%f", this->GetName().c_str(),
             distance, m_velocity);
#endif
  return m_velocity;


}
//----------------------------------------
double CBratAlgorithmGeosVelAtp::ComputeVelocityOutsideEquator() 
{
  CTools::SetDefaultValue(m_velocity);
  if (CTools::IsDefaultValue(m_varValue) || CTools::IsDefaultValue(m_lon))
  {
    return m_velocity;
  }

  double dxBefore = 0.0;
  double dyBefore = 0.0;
  bool hasPrevious = (!CTools::IsDefaultValue(m_latPrev) && !CTools::IsDefaultValue(m_lonPrev) && !CTools::IsDefaultValue(m_varValuePrev));
  double varValuePrev = m_varValuePrev;
//  	PositionAvant	= Index-1;
  if (hasPrevious)
  {
	  this->EtoB(m_lon,
		       m_lat,
		       m_lonPrev,
		       m_latPrev,
		       dxBefore,
		       dyBefore);

  }
  else
  {
    varValuePrev = m_varValue;
  }

  double dxAfter = 0.0;
  double dyAfter = 0.0;

  bool hasNext = (!CTools::IsDefaultValue(m_latNext) && !CTools::IsDefaultValue(m_lonNext) && !CTools::IsDefaultValue(m_varValueNext));
  double varValueNext = m_varValueNext;

  if (hasNext)
  {
    this->EtoB(m_lon,
		       m_lat,
		       m_lonNext,
		       m_latNext,
		       dxAfter,
		       dyAfter);
  }
  else
  {
    varValueNext = m_varValue;
  }


#if _DEBUG_BRAT_ALGO    
  CTrace::Tracer(3, "Algorithm '%s' - Compute velocity outside the equator: dxAfter=%f, dyAfter=%f, dxBefore=%f, dyBefore=%f,", this->GetName().c_str(),
             dxAfter, dyAfter, dxBefore, dyBefore);
#endif

  double dxAfterSqr = CTools::Sqr(dxAfter);
  double dyAfterSqr = CTools::Sqr(dyAfter);
  double dxBeforeSqr = CTools::Sqr(dxBefore);
  double dyBeforeSqr = CTools::Sqr(dyBefore);

  if (CTools::IsDefaultValue(dxAfterSqr) || CTools::IsDefaultValue(dyAfterSqr) || 
      CTools::IsDefaultValue(dxBeforeSqr) || CTools::IsDefaultValue(dyBeforeSqr))
  {
    return m_velocity;
  }

  double distance = (CTools::Sqrt(dxBeforeSqr + dyBeforeSqr)
			             + CTools::Sqrt(dxAfterSqr + dyAfterSqr))
                 * 1000.0;

  if (distance == 0.0)
  {
    return m_velocity;
  }

  m_velocity = ((GetTrackDirection() * (varValueNext - varValuePrev)) * m_coriolis) / distance;

#if _DEBUG_BRAT_ALGO    
  CTrace::Tracer(3, "Algorithm '%s' - Compute velocity outside the equator: distance=%f, velocity=%f", this->GetName().c_str(),
             distance, m_velocity);
#endif

  return m_velocity;
}
//----------------------------------------
void CBratAlgorithmGeosVelAtp::SetGap() 
{
  m_gap	= 0.0;

  if (CTools::IsDefaultValue(m_lat) || CTools::IsDefaultValue(m_latNext)) 
  {
    return;
  }

  if (CTools::Abs(m_latNext - m_lat) > CTools::Abs(m_gap))
  {
    m_gap	= m_latNext - m_lat;
  }
}

//----------------------------------------
void CBratAlgorithmGeosVelAtp::SetEquatorTransition() 
{

  if (m_equatorTransitionIsNext)
  {
    m_equatorTransitionIsNext = false;
    m_equatorTransition = true;
    return;
  }

  m_equatorTransition	= false;
  m_equatorTransitionIsNext = false;

  if (CTools::IsDefaultValue(m_lat) || CTools::IsDefaultValue(m_latNext)) 
  {
    return;
  }

  if ((m_lat * m_latNext) <= 0.0)
  {
    // Keep the nearest point from the equator
    if (CTools::Abs(m_lat) < CTools::Abs(m_latNext))
    {
      m_equatorTransition = true;
    }
    else
    {
      m_equatorTransitionIsNext = true;
    }
  }

}
 
//----------------------------------------
void CBratAlgorithmGeosVelAtp::CheckInputParams(CVectorBratAlgorithmParam& args) 
{
  CBratAlgorithmBase::CheckInputParams(args);

  for (uint32_t indexParam = 0 ; indexParam < CBratAlgorithmGeosVelAtp::m_INPUT_PARAMS ; indexParam++)
  {
    CheckInputTypeParams(indexParam, this->GetInputParamFormat(indexParam), args);
  }


}
//----------------------------------------
void CBratAlgorithmGeosVelAtp::SetParamValues(CVectorBratAlgorithmParam& args)
{    
  CheckInputParams(args);

  // Set latitude
  m_lat = args.at(CBratAlgorithmGeosVelAtp::m_LAT_PARAM_INDEX).GetValueAsDouble();
  // Set longitude
  m_lon = args.at(CBratAlgorithmGeosVelAtp::m_LON_PARAM_INDEX).GetValueAsDouble();
  // Set variable
  m_varValue = args.at(CBratAlgorithmGeosVelAtp::m_VAR_PARAM_INDEX).GetValueAsDouble();
}

//----------------------------------------
void CBratAlgorithmGeosVelAtp::Dump(std::ostream& fOut /*= std::cerr*/)
{
   if (CTrace::IsTrace() == false)
   { 
      return;
   }

  fOut << "==> Dump a CBratAlgorithmGeosVelAtp Object at "<< this << std::endl;
  CBratAlgorithmGeosVel::Dump(fOut);
  fOut << "m_varValue: " << m_varValue << std::endl;
  fOut << "m_varValuePrev: " << m_varValuePrev << std::endl;
  fOut << "m_varValueNext: " << m_varValueNext << std::endl;
  fOut << "m_gap: " << m_gap << std::endl;
  fOut << "==> END Dump a CBratAlgorithmGeosVelAtp Object at "<< this << std::endl;

}

} // end namespace
