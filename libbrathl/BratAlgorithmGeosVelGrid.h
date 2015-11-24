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
#if !defined(_BratAlgorithmGeosVelGrid_h_)
#define _BratAlgorithmGeosVelGrid_h_

#include <string>

#include "new-gui/Common/tools/brathl_error.h" 
#include "brathl.h" 

#include "Tools.h" 
#include "BratAlgorithmGeosVel.h" 


using namespace brathl;

namespace brathl
{
//-------------------------------------------------------------
//------------------- CBratAlgorithmGeosVelGrid class --------------------
//-------------------------------------------------------------


/** \addtogroup Algorithm Algorithms classes
  @{ */

/** 
  Algorithm base class. 
*/

class CBratAlgorithmGeosVelGrid : public CBratAlgorithmGeosVel 
{
public:
  /** Default contructor */
  CBratAlgorithmGeosVelGrid();
  /** Copy contructor */
  CBratAlgorithmGeosVelGrid(const CBratAlgorithmGeosVelGrid	&copy);

  /** Destructor */
  virtual ~CBratAlgorithmGeosVelGrid();

  virtual uint32_t GetNumInputParam() { return CBratAlgorithmGeosVelGrid::m_INPUT_PARAMS; };
  
  virtual std::string GetInputParamDesc(uint32_t indexParam)  {
        std::string value = "";
        switch (indexParam) 
        {
          case 0: value = "Latitude field."; break;
          case 1: value = "Longitude field."; break;
          case 2: value = "Height: sea surface height variable (or expression) used to derive the geostrophic velocity component. "
                          "Typically, use sea surface height with respect to geoid (dynamic topography) or with respect to a mean sea surface (Sea level anomalies)."; break;
          case 3: value = "Limit at the Equator: Coriolis force does not apply at the Equator, so geostrophic velocity basic computation is not relevant there. "
                           "Set by default to +/- 5 degrees north. The parameter must be a constant value."; break;
          default: value = CTools::Format("CBratAlgorithmGeosVelGrid#GetInputParamDesc: out of range parameter index %d. Valid range is [0,%d].",
                     indexParam, GetNumInputParam()); break;


        }
        return value;
  };
  virtual CBratAlgorithmParam::bratAlgoParamTypeVal GetInputParamFormat(uint32_t indexParam) {
        CBratAlgorithmParam::bratAlgoParamTypeVal value = CBratAlgorithmParam::T_UNDEF;
        switch (indexParam) 
        {
          case 0: value = CBratAlgorithmParam::T_DOUBLE; break;
          case 1: value = CBratAlgorithmParam::T_DOUBLE; break;
          case 2: value = CBratAlgorithmParam::T_DOUBLE; break;
          case 3: value = CBratAlgorithmParam::T_DOUBLE; break;
          default: throw CAlgorithmException(CTools::Format("CBratAlgorithmGeosVelGrid#GetInputParamFormat: out of range parameter index %d. Valid range is [0,%d].",
                     indexParam, GetNumInputParam())); break;

        }
        return value;
  };
  /** Gets the unit of an input parameter :
   \param indexParam [in] : parameter index.
   */
  virtual std::string GetInputParamUnit(uint32_t indexParam) {
        std::string value = "";
        switch (indexParam) 
        {
          case 0: value = "degrees_north"; break;
          case 1: value = "degrees_east"; break;
          case 2: value = "m"; break;
          case 3: value = "degrees_north"; break;
          default: value = CTools::Format("CBratAlgorithmGeosVelGrid#GetInputParamUnit: out of range parameter index %d. Valid range is [0,%d].",
                     indexParam, GetNumInputParam()); break;

        }
        return value;
  };


  /** Gets the unit of an output value returned by the 'Run' function.
   \param indexParam [in] : parameter index.
   */
  virtual std::string GetOutputUnit() { return "m/s"; };

  virtual double Run(CVectorBratAlgorithmParam& args);

  virtual void CheckInputParams(CVectorBratAlgorithmParam& args);
  virtual void SetParamValues(CVectorBratAlgorithmParam& args);
  virtual std::string GetParamName(uint32_t indexParam) {
        std::string value = "";
        switch (indexParam) 
        {
          case 0: value = CBratAlgorithmGeosVel::m_LAT_PARAM_NAME; break;
          case 1: value = CBratAlgorithmGeosVel::m_LON_PARAM_NAME; break;
          case 2: value = "Height"; break;
          case 3: value = "EquatorLimit"; break;
          default: value = CTools::Format("CBratAlgorithmGeosVelGrid#GetParamName: out of range parameter index %d. Valid range is [0,%d].",
                     indexParam, GetNumInputParam()); break;

        }
        return value;
  };

  virtual double GetParamDefaultValue(uint32_t indexParam) {
        double value;
        setDefaultValue(value);
        switch (indexParam) 
        {
          case 0:  break;
          case 1:  break;
          case 2: break;
          case 3: value = 5.0; break;
          default: break;

        }
        return value;
  };

    /** Overloads operator '=' */
  CBratAlgorithmGeosVelGrid& operator=(const CBratAlgorithmGeosVelGrid &copy);

  /** Dump function */
  virtual void Dump(std::ostream& fOut = std::cerr);
 

protected:

  void Init();
  void Set(const CBratAlgorithmGeosVelGrid &copy);
  virtual void DeleteFieldNetCdf();
  virtual void DeleteProduct();

  virtual double ComputeVelocity() = 0;
  bool PrepareComputeVelocity();
  
  virtual void OpenProductFile();
  void CheckProduct();
  void CheckVarExpression(uint32_t index);

  void CheckLatLonExpression(uint32_t index);

  void CheckEquatorLimit();

  void SetBeginOfFile();
  void SetEndOfFile();

  void GetVarCacheExpressionValue(int32_t minIndexLat, int32_t maxIndexLat, int32_t minIndexLon, int32_t maxIndexLon);

  double GetVarExpressionValueCache(int32_t indexLat, int32_t indexLon);
  double GetVarExpressionValue(int32_t indexLat, int32_t indexLon);

  void GetLongitudes();
  void GetLatitudes();
  int32_t GetLongitudeIndex(double value);
  int32_t GetLatitudeIndex(double value);

  uint32_t GetLonDimRange(CFieldNetCdf* field);
  uint32_t GetLatDimRange(CFieldNetCdf* field);

  double ComputeMean();
  double ComputeSingle();

  virtual void PrepareDataValues2DOneField(CExpressionValue& exprValue);
  virtual void PrepareDataValues2DComplexExpression(CExpressionValue& exprValue);
  virtual void PrepareDataValues2DComplexExpressionWithAlgo(CExpressionValue& exprValue);
  virtual void PrepareDataReading2D(int32_t minIndexLat, int32_t maxIndexLat, int32_t minIndexLon, int32_t maxIndexLon);
  virtual void PrepareDataReading2D(int32_t indexLat, int32_t indexLon);
  
protected:

  double m_varValue;
  double m_varValueW;
  double m_varValueE;
  double m_varValueN;
  double m_varValueS;

  double m_equatorLimit;

  bool m_allLongitudes;

  double m_lonMin;
  double m_lonMax;

  int32_t m_indexLon;
  int32_t m_indexLat;

  int32_t m_varDimLatIndex;
  int32_t m_varDimLonIndex;

  CDoubleArray m_longitudes;
  CDoubleArray m_latitudes;

  CFieldNetCdf* m_fieldLon;
  CFieldNetCdf* m_fieldLat;

  CExpressionValue m_rawDataCache;
  
  static const uint32_t m_LAT_PARAM_INDEX;
  static const uint32_t m_LON_PARAM_INDEX;
  static const uint32_t m_VAR_PARAM_INDEX;
  static const uint32_t m_EQUATOR_LAT_LIMIT_INDEX;
  
  static const uint32_t m_INPUT_PARAMS;


};



//-------------------------------------------------------------
//------------------- CBratAlgorithmGeosVelGridU class --------------------
//-------------------------------------------------------------


/** \addtogroup Algorithm Algorithms classes
  @{ */

/** 
  Algorithm base class. 
*/

class CBratAlgorithmGeosVelGridU : public CBratAlgorithmGeosVelGrid 
{
public:
  /** Default contructor */
  CBratAlgorithmGeosVelGridU();
  /** Copy contructor */
  CBratAlgorithmGeosVelGridU(const CBratAlgorithmGeosVelGridU	&copy);

  /** Destructor */
  virtual ~CBratAlgorithmGeosVelGridU();

  virtual std::string GetName() const override { return "BratAlgoGeosVelGridU"; };
  
  virtual std::string GetDescription() { return "Geostrophic velocity Zonal component (U) computation from gridded data"; };

  /** Dump function */
  virtual void Dump(std::ostream& fOut = std::cerr);

protected:

  void Init();

  double ComputeVelocity();

};


//-------------------------------------------------------------
//------------------- CBratAlgorithmGeosVelGridV class --------------------
//-------------------------------------------------------------


/** \addtogroup Algorithm Algorithms classes
  @{ */

/** 
  Algorithm base class. 
*/

class CBratAlgorithmGeosVelGridV : public CBratAlgorithmGeosVelGrid 
{
public:
  /** Default contructor */
  CBratAlgorithmGeosVelGridV();
  /** Copy contructor */
  CBratAlgorithmGeosVelGridV(const CBratAlgorithmGeosVelGridV	&copy);

  /** Destructor */
  virtual ~CBratAlgorithmGeosVelGridV();

  virtual std::string GetName() const override { return "BratAlgoGeosVelGridV"; };
  
  virtual std::string GetDescription() { return "Geostrophic velocity Meridional component (V) computation from gridded data"; };
  

  /** Dump function */
  virtual void Dump(std::ostream& fOut = std::cerr);

protected:

  void Init();

  double ComputeVelocity();

};



/** @} */
}


#endif // !defined(_BratAlgorithmGeosVelGrid_h_)
