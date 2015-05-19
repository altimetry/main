/*
* Copyright (C) 2008 Collecte Localisation Satellites (CLS), France (31)
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
#if !defined(_BratAlgoFilterMedian2D_h_)
#define _BratAlgoFilterMedian2D_h_

#include "brathl_error.h" 
#include "brathl.h" 

#include "Stl.h"

#include "Tools.h" 
#include "BratAlgoFilterMedian.h" 


using namespace brathl;

namespace brathl
{
//-------------------------------------------------------------
//------------------- CBratAlgoFilterMedian2D class --------------------
//-------------------------------------------------------------


/** \addtogroup Algorithm Algorithms classes
  @{ */

/** 
  Algorithm base class. 
*/

class CBratAlgoFilterMedian2D : public CBratAlgoFilterMedian 
{
public:
  /** Default contructor */
  CBratAlgoFilterMedian2D();
  /** Copy contructor */
  CBratAlgoFilterMedian2D(const CBratAlgoFilterMedian2D	&copy);

  /** Destructor */
  virtual ~CBratAlgoFilterMedian2D();

  virtual string GetName() { return "BratAlgoFilterMedianGrid"; };

  virtual string GetDescription() { return "Median filter for two-dimensionals data (e.g. gridded data) as input data source. "
                                           "Median filter is windowed filter of nonlinear class, which removes destructive noise while preserving edges. "
                                           "The output value unit depends on the variable (data) filtered" ; };

  virtual uint32_t GetNumInputParam() { return CBratAlgoFilterMedian2D::m_INPUT_PARAMS; };
  
  virtual string GetInputParamDesc(uint32_t indexParam)  {
        string value = "";
        switch (indexParam) 
        {
          case 0: value = CBratAlgoFilterKernel::m_VAR_PARAM_DESCR; break;
          case 1: value = "Window/region width (x). The parameter must be a constant value."; break;
          case 2: value = "Window/region height (y). The parameter must be a constant value."; break;
          case 3: value = CBratAlgoFilterKernel::m_VALID_PARAM_DESCR; break;
          case 4: value = CBratAlgoFilterKernel::m_EXTRAPOLATE_PARAM_DESCR; break;
          default: value = CTools::Format("CBratAlgoFilterMedian2D#GetInputParamDesc: out of range parameter index %d. Valid range is [0,%d].",
                     indexParam, GetNumInputParam()); break;


        }
        return value;
  };
  virtual CBratAlgorithmParam::bratAlgoParamTypeVal GetInputParamFormat(uint32_t indexParam) {
        CBratAlgorithmParam::bratAlgoParamTypeVal value = CBratAlgorithmParam::T_UNDEF;
        switch (indexParam) 
        {
          case 0: value = CBratAlgorithmParam::T_DOUBLE; break;
          case 1: value = CBratAlgorithmParam::T_INT; break;
          case 2: value = CBratAlgorithmParam::T_INT; break;
          case 3: value = CBratAlgorithmParam::T_INT; break;
          case 4: value = CBratAlgorithmParam::T_INT; break;
          default: throw CAlgorithmException(CTools::Format("CBratAlgoFilterMedian2D#GetInputParamFormat: out of range parameter index %d. Valid range is [0,%d].",
                     indexParam, GetNumInputParam())); break;

        }
        return value;
  };
  /** Gets the unit of an input parameter :
   \param indexParam [in] : parameter index.
   */
  virtual string GetInputParamUnit(uint32_t indexParam) {
        string value = "";
        switch (indexParam) 
        {
          case 0: value = "this unit depends on the input data (variable or Brat expression)"; break;
          case 1: value = "count"; break;
          case 2: value = "count"; break;
          case 3: value = "count"; break;
          case 4: value = "count"; break;
          default: value = CTools::Format("CBratAlgoFilterMedian2D#GetInputParamUnit: out of range parameter index %d. Valid range is [0,%d].",
                     indexParam, GetNumInputParam()); break;

        }
        return value;
  };


  /** Gets the unit of an output value returned by the 'Run' function.
   \param indexParam [in] : parameter index.
   */
  virtual string GetOutputUnit() { return "this unit depends on the input data (variable or Brat expression), but it is always a SI unit"; };

  virtual double Run(CVectorBratAlgorithmParam& args);

  virtual void CheckInputParams(CVectorBratAlgorithmParam& args);
  virtual void SetParamValues(CVectorBratAlgorithmParam& args);
  virtual string GetParamName(uint32_t indexParam) {
        string value = "";
        switch (indexParam) 
        {
          case 0: value = CBratAlgoFilterKernel::m_VAR_PARAM_NAME; break;
          case 1: value = "WindowWidth"; break;
          case 2: value = "WindowHeight"; break;
          case 3: value = CBratAlgoFilterKernel::m_VALID_PARAM_NAME; break;
          case 4: value = CBratAlgoFilterKernel::m_EXTRAPOLATE_PARAM_NAME; break;
          default: value = CTools::Format("CBratAlgoFilterMedian2D#GetParamName: out of range parameter index %d. Valid range is [0,%d].",
                     indexParam, GetNumInputParam()); break;

        }
        return value;
  };

  virtual double GetParamDefaultValue(uint32_t indexParam) {
        double value;
        CTools::SetDefaultValue(value);
        switch (indexParam) 
        {
          case 0:  break;
          case 1:  break;
          case 2:  break;
          case 3:  break;
          case 4: value = 0.0; break;
          default: break;

        }
        return value;
  };

    /** Overloads operator '=' */
  CBratAlgoFilterMedian2D& operator=(const CBratAlgoFilterMedian2D &copy);

  virtual uint32_t GetDataWindowSize() { return m_dataWindowHeight * m_dataWindowWidth; };

  /** Dump function */
  virtual void Dump(ostream& fOut = cerr);
 

protected:

  void Init();
  void Set(const CBratAlgoFilterMedian2D &copy);

  virtual void OpenProductFile();
  void CheckProduct();
  void CheckVarExpression(uint32_t index);


  void SetBeginOfFile();
  void SetEndOfFile();


  void PrepareDataWindow();
  void PrepareDataValues();
  
  double ComputeSingle();
  double ComputeMean();



protected:


  static const uint32_t m_WINDOW_WIDTH_PARAM_INDEX;
  static const uint32_t m_WINDOW_HEIGHT_PARAM_INDEX;
  static const uint32_t m_VALID_PARAM_INDEX;
  static const uint32_t m_EXTRAPOLATE_PARAM_INDEX;

  static const uint32_t m_INPUT_PARAMS;


};


/** @} */
}


#endif // !defined(_BratAlgoFilterMedian2D_h_)
