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
#if !defined(_BratAlgorithmGeosVel_h_)
#define _BratAlgorithmGeosVel_h_

#include <string>

#include "new-gui/Common/tools/brathl_error.h" 
#include "brathl.h" 

#include "Tools.h" 
#include "BratAlgorithmBase.h" 

using namespace brathl;

namespace brathl
{

//-------------------------------------------------------------
//------------------- CBratAlgorithmGeosVel class --------------------
//-------------------------------------------------------------


/** \addtogroup Algorithm Algorithms classes
  @{ */

/** 
  Algorithm base class. 
*/

class CBratAlgorithmGeosVel : public CBratAlgorithmBase 
{
public:
  /** Default contructor */
  CBratAlgorithmGeosVel();
  /** Copy contructor */
  CBratAlgorithmGeosVel(const CBratAlgorithmGeosVel	&copy);

  /** Destructor */
  virtual ~CBratAlgorithmGeosVel();


  /** Overloads operator '=' */
  CBratAlgorithmGeosVel& operator=(const CBratAlgorithmGeosVel &copy);

  /** Dump function */
  virtual void Dump(std::ostream& fOut = std::cerr);
 
  //static double Exec(const char* function, const char *fmt, const type_union *arg);
  //static CBratAlgorithmGeosVel* GetNew(const char* algorithName);

  void EtoB(double lonPlane, double latPlane, double lon, double lat, double& betaX, double& betaY);
  void BtoE(double lonPlane, double latPlane, double betaX, double betaY, double& lon, double& lat);

protected:

  void Init();
  void Set(const CBratAlgorithmGeosVel &o);
  //Save current values to previous values previous values.
  virtual void SetPreviousValues(bool fromProduct);
  virtual void SetNextValues();
  virtual void ComputeCoriolis();

  void SetBeginOfFile();
  void SetEndOfFile();

protected:
  double m_earthRadius;
  double m_gravity;
  double m_omega;
  double m_beta;
  double m_degreeToRadianMutiplier;
  double m_p2;

  bool m_equatorTransition;
  bool m_equatorTransitionIsNext;

  double m_lat;
  double m_lon;
  
  double m_latPrev;
  double m_lonPrev;
  
  double m_latNext;
  double m_lonNext;

  double m_coriolis;

  CDoubleArray* m_latArray;
  CDoubleArray* m_lonArray;

  double m_velocity;

  static const std::string m_LAT_PARAM_NAME; 
  static const std::string m_LON_PARAM_NAME; 

};


/** @} */
}

#endif // !defined(_BratAlgorithmGeosVel_h_)
