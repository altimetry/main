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
#if !defined(_BratAlgorithmGeosVelAtp_h_)
#define _BratAlgorithmGeosVelAtp_h_

#include <string>

#include "new-gui/Common/tools/brathl_error.h" 
#include "brathl.h" 

#include "Tools.h" 
#include "BratAlgorithmGeosVel.h" 


using namespace brathl;

namespace brathl
{
//-------------------------------------------------------------
//------------------- CBratAlgorithmGeosVelAtp class --------------------
//-------------------------------------------------------------


/** \addtogroup Algorithm Algorithms classes
  @{ */

/** 
  Algorithm base class. 
*/

class CBratAlgorithmGeosVelAtp : public CBratAlgorithmGeosVel 
{
public:
  /** Default contructor */
  CBratAlgorithmGeosVelAtp();
  /** Copy contructor */
  CBratAlgorithmGeosVelAtp(const CBratAlgorithmGeosVelAtp	&copy);

  /** Destructor */
  virtual ~CBratAlgorithmGeosVelAtp() {}

  virtual std::string GetName() const override { return "BratAlgoGeosVelAtp"; }
  
  virtual std::string GetDescription() const override { return "Geostrophic velocity computation for along-track data; result is the value of the geostrophic velocity component perpendicular to the track."; }
  
  virtual uint32_t GetNumInputParam() const override { return CBratAlgorithmGeosVelAtp::m_INPUT_PARAMS; }
  
  virtual std::string GetInputParamDesc(uint32_t indexParam) const override {
        std::string value = "";
        switch (indexParam) 
        {
          case 0: value = "Latitude field."; break;
          case 1: value = "Longitude field."; break;
          case 2: value = "Height: sea surface height variable (or expression) used to derive the geostrophic velocity component. "
                          "Typically, use sea surface height with respect to geoid (dynamic topography) or with respect to a mean sea surface (Sea level anomalies)."; break;
          default: value = CTools::Format("CBratAlgorithmGeosVelAtp#GetInputParamDesc: out of range parameter index %d. Valid range is [0,%d].",
                     indexParam, GetNumInputParam()); break;


        }
        return value;
  };
  virtual CBratAlgorithmParam::bratAlgoParamTypeVal GetInputParamFormat(uint32_t indexParam) const override {        
        CBratAlgorithmParam::bratAlgoParamTypeVal value = CBratAlgorithmParam::T_UNDEF;
        switch (indexParam) 
        {
          case 0: value = CBratAlgorithmParam::T_DOUBLE; break;
          case 1: value = CBratAlgorithmParam::T_DOUBLE; break;
          case 2: value = CBratAlgorithmParam::T_DOUBLE; break;
          default: throw CAlgorithmException(CTools::Format("CBratAlgorithmGeosVelAtp#GetInputParamFormat: out of range parameter index %d. Valid range is [0,%d].",
                     indexParam, GetNumInputParam())); break;

        }
        return value;
  };
  /** Gets the unit of an input parameter :
   \param indexParam [in] : parameter index.
   */
  virtual std::string GetInputParamUnit(uint32_t indexParam) const override {
        std::string value = "";
        switch (indexParam) 
        {
          case 0: value = "degrees_north"; break;
          case 1: value = "degrees_east"; break;
          case 2: value = "m"; break;
          default: value = CTools::Format("CBratAlgorithmGeosVelAtp#GetInputParamUnit: out of range parameter index %d. Valid range is [0,%d].",
                     indexParam, GetNumInputParam()); break;

        }
        return value;
  };


  /** Gets the unit of an output value returned by the 'Run' function.
   \param indexParam [in] : parameter index.
   */
  virtual std::string GetOutputUnit() const override { return "m/s"; }

  virtual double Run(CVectorBratAlgorithmParam& args) override;

  virtual void CheckInputParams(CVectorBratAlgorithmParam& args) override;

  virtual void SetParamValues(CVectorBratAlgorithmParam& args);

  virtual std::string GetParamName(uint32_t indexParam) const override {
        std::string value = "";
        switch (indexParam) 
        {
          case 0: value = CBratAlgorithmGeosVel::m_LAT_PARAM_NAME; break;
          case 1: value = CBratAlgorithmGeosVel::m_LON_PARAM_NAME; break;
          case 2: value = "Height"; break;
          default: value = CTools::Format("CBratAlgorithmGeosVelAtp#GetParamName: out of range parameter index %d. Valid range is [0,%d].",
                     indexParam, GetNumInputParam()); break;

        }
        return value;
  };

  double GetTrackDirection() { return (m_gap >= 0.0 ? 1.0 : -1.0); }

    /** Overloads operator '=' */
  CBratAlgorithmGeosVelAtp& operator=(const CBratAlgorithmGeosVelAtp &copy);

  /** Dump function */
  virtual void Dump(std::ostream& fOut = std::cerr) override;
 
protected:

  void Init();
  void Set(const CBratAlgorithmGeosVelAtp &copy);
  
  double ComputeVelocity();
  double ComputeVelocityEquator();
  double ComputeVelocityOutsideEquator();
  
  virtual void SetPreviousValues(bool fromProduct) override;
  virtual void SetNextValues() override;

  void SetEquatorTransition();
  void SetGap();

  virtual void SetBeginOfFile() override;
  virtual void SetEndOfFile() override;



protected:

  double m_varValue;
  double m_varValuePrev;
  double m_varValueNext;

  double m_gap;

  static const uint32_t m_LAT_PARAM_INDEX;
  static const uint32_t m_LON_PARAM_INDEX;
  static const uint32_t m_VAR_PARAM_INDEX;
  
  static const uint32_t m_INPUT_PARAMS;


};


/** @} */
}


#endif // !defined(_BratAlgorithmGeosVelAtp_h_)
