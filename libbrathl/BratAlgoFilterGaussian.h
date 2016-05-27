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
#if !defined(_BratAlgoFilterGaussian_h_)
#define _BratAlgoFilterGaussian_h_

#include "new-gui/Common/tools/brathl_error.h" 
#include "brathl.h" 

#include <string>

#include "Tools.h" 
#include "Product.h" 
#include "ProductNetCdf.h" 
#include "BratAlgoFilterKernel.h" 

using namespace brathl;

namespace brathl
{

//-------------------------------------------------------------
//------------------- CBratAlgoFilterGaussian class --------------------
//-------------------------------------------------------------


class CBratAlgoFilterGaussian : public CBratAlgoFilterKernel 
{
public:
  /** Default contructor */
  CBratAlgoFilterGaussian();
  /** Copy contructor */
  CBratAlgoFilterGaussian(const CBratAlgoFilterGaussian	&copy);

  /** Destructor */
  virtual ~CBratAlgoFilterGaussian();


  virtual uint32_t GetNumInputParam() const override { return CBratAlgoFilterGaussian::m_INPUT_PARAMS; };
  
  virtual std::string GetInputParamDesc(uint32_t indexParam) const override {
        std::string value = "";
        switch (indexParam) 
        {
          case 0: value = CBratAlgoFilterKernel::m_VAR_PARAM_DESCR; break;
          case 1: value = "Window/region size (N): The size will be N x N for two-dimensionals data (e.g. gridded data). "
                          "The size will be N for one-dimensional data (e.g. along-track data)."
                          "The value must be odd. The parameter must be an odd constant value."; break;
          case 2: value = "The standard deviation (sigma) of the distribution. "
                          "Set by default to 1. The parameter must be a constant value. "
                          "Usually, a Gaussian filter with sigma <= 1 is used to reduce noise."; break;
          case 3: value = "The coefficient of spreading to the left and right of the distribution. "
                          "Set by default to 3. The parameter must be a strictly positive constant value. "
                          "Usually in practice, the value used is 3 with sigma equals to 1. "
                          "The part of Gaussian distribution utilized is the range [(-3 x sigma), (3 x sigma)], the Gaussian distribution is truncated at points +/- (3 x sigma). "
                          "When the range is [(-3 x sigma), (3 x sigma)], the bell-shaped curve adjusts the corner values to 0.01. "; break;
          case 4: value = CBratAlgoFilterKernel::m_VALID_PARAM_DESCR; break;
          case 5: value = CBratAlgoFilterKernel::m_EXTRAPOLATE_PARAM_DESCR; break;
          default: value = CTools::Format("CBratAlgoFilterGaussian2D#GetInputParamDesc: out of range parameter index %d. Valid range is [0,%d].",
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
          case 2: value = CBratAlgorithmParam::T_DOUBLE; break;
          case 3: value = CBratAlgorithmParam::T_INT; break;
          case 4: value = CBratAlgorithmParam::T_INT; break;
          case 5: value = CBratAlgorithmParam::T_INT; break;
          default: throw CAlgorithmException(CTools::Format("CBratAlgoFilterGaussian2D#GetInputParamFormat: out of range parameter index %d. Valid range is [0,%d].",
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
          case 5: value = "count"; break;
          default: value = CTools::Format("CBratAlgoFilterGaussian2D#GetInputParamUnit: out of range parameter index %d. Valid range is [0,%d].",
                     indexParam, GetNumInputParam()); break;

        }
        return value;
  };


  /** Gets the unit of an output value returned by the 'Run' function.
   \param indexParam [in] : parameter index.
   */
  virtual std::string GetOutputUnit() const override { return "this unit depends on the input data (variable or Brat expression), but it is always a SI unit"; }

  virtual void CheckInputParams(CVectorBratAlgorithmParam& args) override;

  virtual void SetParamValues(CVectorBratAlgorithmParam& args);

  virtual std::string GetParamName(uint32_t indexParam) const override {
        std::string value = "";
        switch (indexParam) 
        {
          case 0: value = CBratAlgoFilterKernel::m_VAR_PARAM_NAME; break;
          case 1: value = "WindowLength"; break;
          case 2: value = "Sigma"; break;
          case 3: value = "Trunc"; break;
          case 4: value = CBratAlgoFilterKernel::m_VALID_PARAM_NAME; break;
          case 5: value = CBratAlgoFilterKernel::m_EXTRAPOLATE_PARAM_NAME; break;
          default: value = CTools::Format("CBratAlgoFilterGaussian#GetParamName: out of range parameter index %d. Valid range is [0,%d].",
                     indexParam, GetNumInputParam()); break;

        }
        return value;
  };

  virtual double GetParamDefaultValue(uint32_t indexParam) const override {
        double value;
        setDefaultValue(value);
        switch (indexParam) 
        {
          case 0: break;
          case 1: break;
          case 2: value = 1.0; break;
          case 3: value = 3.0; break;
          case 4: break;
          case 5: value = 0.0; break;
          default: break;

        }
        return value;
  };


  /** Overloads operator '=' */
  CBratAlgoFilterGaussian& operator=(const CBratAlgoFilterGaussian &copy);

  /** Dump function */
  virtual void Dump(std::ostream& fOut = std::cerr) override;
 
protected:
  enum ComputeMode {ModeHorizontal, ModeVertical};

protected:

  void Init();
  void Set(const CBratAlgoFilterGaussian &o);
  //Save current values to previous values previous values.
  virtual void SetPreviousValues(bool fromProduct) override;
  virtual void SetNextValues() override;

  virtual void SetBeginOfFile() override;
  virtual void SetEndOfFile() override;

  void PrepareReturn() ;

  virtual void ComputeInitialWeights();
  //virtual void SumWeights(ComputeMode mode, uint32_t from);
  
  double ApplyFilter(ComputeMode mode, uint32_t from);

  virtual void PrepareDataValues2DOneField(CExpressionValue& exprValue);
  virtual void PrepareDataValues2DComplexExpression(CExpressionValue& exprValue);
  virtual void PrepareDataValues2DComplexExpressionWithAlgo(CExpressionValue& exprValue);

  virtual void PrepareDataWindow(ComputeMode mode, const CDoubleArray& dataValue);
  virtual void PrepareDataWindowHorizontal(const CDoubleArray& dataValue);
  virtual void PrepareDataWindowVertical(const CDoubleArray& dataValue);

  virtual void PrepareDataReading2DX();
  virtual void PrepareDataReading2DY();

  void ResetRawDataWindow();

protected:

  double m_gaussian;

  double m_sigma;
  uint32_t m_trunc;

  CDoubleArray m_initialWeights;

  //CDoubleArray m_weights;

  static const uint32_t m_WINDOW_PARAM_INDEX;
  static const uint32_t m_TRUNC_PARAM_INDEX;
  static const uint32_t m_SIGMA_PARAM_INDEX;
  static const uint32_t m_VALID_PARAM_INDEX;
  static const uint32_t m_EXTRAPOLATE_PARAM_INDEX;

  static const uint32_t m_INPUT_PARAMS;


};


/** @} */
}

#endif // !defined(_BratAlgoFilterGaussian_h_)
