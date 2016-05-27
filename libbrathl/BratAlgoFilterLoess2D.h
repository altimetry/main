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
#if !defined(_BratAlgoFilterLoess2D_h_)
#define _BratAlgoFilterLoess2D_h_

#include <string>

#include "new-gui/Common/tools/brathl_error.h" 
#include "brathl.h" 

#include "Tools.h" 
#include "BratAlgoFilterLoess.h" 


using namespace brathl;

namespace brathl
{
//-------------------------------------------------------------
//------------------- CBratAlgoFilterLoess2D class --------------------
//-------------------------------------------------------------


/** \addtogroup Algorithm Algorithms classes
  @{ */

/** 
  Algorithm base class. 
*/

class CBratAlgoFilterLoess2D : public CBratAlgoFilterLoess 
{
public:
  /** Default contructor */
  CBratAlgoFilterLoess2D();
  /** Copy contructor */
  CBratAlgoFilterLoess2D(const CBratAlgoFilterLoess2D	&copy);

  /** Destructor */
  virtual ~CBratAlgoFilterLoess2D();

  virtual std::string GetName() const override { return "BratAlgoFilterLoessGrid"; }

  virtual std::string GetDescription() const override { return "Loess filter (smoothing filter) for two-dimensionals data (e.g. gridded data) as input data source. "
                                           "Description to be completed... . "
                                           "The output value unit depends on the variable (data) filtered" ; }

  virtual uint32_t GetNumInputParam() const override { return CBratAlgoFilterLoess2D::m_INPUT_PARAMS; }
  
  virtual std::string GetInputParamDesc(uint32_t indexParam) const override {
        std::string value = "";
        switch (indexParam) 
        {
          case 0: value = CBratAlgoFilterKernel::m_VAR_PARAM_DESCR; break;
          case 1: value = "Window/region width (x). The parameter must be a constant odd value."; break;
          case 2: value = "Window/region height (y). The parameter must be a constant odd value."; break;
          case 3: value = CBratAlgoFilterKernel::m_VALID_PARAM_DESCR; break;
          case 4: value = CBratAlgoFilterKernel::m_EXTRAPOLATE_PARAM_DESCR; break;
          default: value = CTools::Format("CBratAlgoFilterLoess2D#GetInputParamDesc: out of range parameter index %d. Valid range is [0,%d].",
                     indexParam, GetNumInputParam()); break;


        }
        return value;
  };
  virtual CBratAlgorithmParam::bratAlgoParamTypeVal GetInputParamFormat(uint32_t indexParam) const override {
        CBratAlgorithmParam::bratAlgoParamTypeVal value = CBratAlgorithmParam::T_UNDEF;
        switch (indexParam) 
        {
          case 0: value = CBratAlgorithmParam::T_DOUBLE; break;
          case 1: value = CBratAlgorithmParam::T_INT; break;
          case 2: value = CBratAlgorithmParam::T_INT; break;
          case 3: value = CBratAlgorithmParam::T_INT; break;
          case 4: value = CBratAlgorithmParam::T_INT; break;
          default: throw CAlgorithmException(CTools::Format("CBratAlgoFilterLoess2D#GetInputParamFormat: out of range parameter index %d. Valid range is [0,%d].",
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
          case 0: value = "this unit depends on the input data (variable or Brat expression)"; break;
          case 1: value = "count"; break;
          case 2: value = "count"; break;
          case 3: value = "count"; break;
          case 4: value = "count"; break;
          default: value = CTools::Format("CBratAlgoFilterLoess2D#GetInputParamUnit: out of range parameter index %d. Valid range is [0,%d].",
                     indexParam, GetNumInputParam()); break;

        }
        return value;
  };


  /** Gets the unit of an output value returned by the 'Run' function.
   \param indexParam [in] : parameter index.
   */
  virtual std::string GetOutputUnit() const override { return "this unit depends on the input data (variable or Brat expression), but it is always a SI unit"; }

  virtual double Run(CVectorBratAlgorithmParam& args) override;

  virtual void CheckInputParams(CVectorBratAlgorithmParam& args) override;

  virtual void SetParamValues(CVectorBratAlgorithmParam& args);

  virtual std::string GetParamName(uint32_t indexParam) const override {
        std::string value = "";
        switch (indexParam) 
        {
          case 0: value = CBratAlgoFilterKernel::m_VAR_PARAM_NAME; break;
          case 1: value = "WindowWidth"; break;
          case 2: value = "WindowHeight"; break;
          case 3: value = CBratAlgoFilterKernel::m_VALID_PARAM_NAME; break;
          case 4: value = CBratAlgoFilterKernel::m_EXTRAPOLATE_PARAM_NAME; break;
          default: value = CTools::Format("CBratAlgoFilterLoess2D#GetParamName: out of range parameter index %d. Valid range is [0,%d].",
                     indexParam, GetNumInputParam()); break;

        }
        return value;
  };

  virtual double GetParamDefaultValue(uint32_t indexParam) const override {
        double value;
        setDefaultValue(value);
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
  CBratAlgoFilterLoess2D& operator=(const CBratAlgoFilterLoess2D &copy);

  virtual uint32_t GetDataWindowSize() override { return m_dataWindowHeight * m_dataWindowWidth; }

  /** Dump function */
  virtual void Dump(std::ostream& fOut = std::cerr) override;
 

protected:

  void Init();
  void Set(const CBratAlgoFilterLoess2D &copy);

  virtual void OpenProductFile() override;
  void CheckProduct();
  void CheckVarExpression(uint32_t index) override;


  virtual void SetBeginOfFile() override;
  virtual void SetEndOfFile() override;


  void PrepareDataWindow();
  void PrepareDataValues();

  double ComputeLoess();
  void ComputeInitialWeights();
  //uint32_t SumWeights();
  
  double ApplyFilter();

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


#endif // !defined(_BratAlgoFilterLoess2D_h_)
