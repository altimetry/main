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
#if !defined(_BratAlgoFilterLanczos_h_)
#define _BratAlgoFilterLanczos_h_

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
//------------------- CBratAlgoFilterLanczos class --------------------
//-------------------------------------------------------------

/* ================================================================== 
The filtered value at point m (Y(m)) is :
                 m+l
                  S    a(i)*X(i)
                i=m-l
        Y(m) =  ----------------
                 m l
                  S   a(i)
                i=m-l

        with:            SIN(2*PI*f*ABS(i-m))   SIN(PI*ABS(i-m)/n)
               a(i) =    -------------------- * ------------------
               i#m           PI*ABS(i-m)          PI*ABS(i-m)/n


               a(m) = 2.f

f is the cutoff frequency
n is the width of the window

================================================================== */


class CBratAlgoFilterLanczos : public CBratAlgoFilterKernel 
{
public:
  /** Default contructor */
  CBratAlgoFilterLanczos();
  /** Copy contructor */
  CBratAlgoFilterLanczos(const CBratAlgoFilterLanczos	&copy);

  /** Destructor */
  virtual ~CBratAlgoFilterLanczos();


  virtual uint32_t GetNumInputParam() const override { return CBratAlgoFilterLanczos::m_INPUT_PARAMS; };
  
  virtual std::string GetInputParamDesc(uint32_t indexParam) const override {
        std::string value = "";
        switch (indexParam) 
        {
          case 0: value = CBratAlgoFilterKernel::m_VAR_PARAM_DESCR; break;
          case 1: value = "Window/region size (N): The size will be N x N for two-dimensionals data (e.g. gridded data). "
                          "The size will be N for one-dimensional data (e.g. along-track data)."
                          "The value must be odd. The parameter must be an odd constant value."; break;
          case 2: value = "The value of the cutoff period (number of data points). The frequency (1/cutoff) is the value at which the response passes from one to zero."; break;
          case 3: value = CBratAlgoFilterKernel::m_VALID_PARAM_DESCR; break;
          case 4: value = CBratAlgoFilterKernel::m_EXTRAPOLATE_PARAM_DESCR; break;
          default: value = CTools::Format("CBratAlgoFilterLanczos2D#GetInputParamDesc: out of range parameter index %d. Valid range is [0,%d].",
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
          default: throw CAlgorithmException(CTools::Format("CBratAlgoFilterLanczos2D#GetInputParamFormat: out of range parameter index %d. Valid range is [0,%d].",
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
          default: value = CTools::Format("CBratAlgoFilterLanczos2D#GetInputParamUnit: out of range parameter index %d. Valid range is [0,%d].",
                     indexParam, GetNumInputParam()); break;

        }
        return value;
  };


  /** Gets the unit of an output value returned by the 'Run' function.
   \param indexParam [in] : parameter index.
   */
  virtual std::string GetOutputUnit() const override { return "this unit depends on the input data (variable or Brat expression), but it is always a SI unit"; };

  virtual void CheckInputParams(CVectorBratAlgorithmParam& args);

  virtual void SetParamValues(CVectorBratAlgorithmParam& args);

  virtual std::string GetParamName(uint32_t indexParam) const override {
        std::string value = "";
        switch (indexParam) 
        {
          case 0: value = CBratAlgoFilterKernel::m_VAR_PARAM_NAME; break;
          case 1: value = "WindowLength"; break;
          case 2: value = "CutOff"; break;
          case 3: value = CBratAlgoFilterKernel::m_VALID_PARAM_NAME; break;
          case 4: value = CBratAlgoFilterKernel::m_EXTRAPOLATE_PARAM_NAME; break;
          default: value = CTools::Format("CBratAlgoFilterLanczos#GetParamName: out of range parameter index %d. Valid range is [0,%d].",
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
          case 3: break;
          case 4: value = 0.0; break;
          default: break;

        }
        return value;
  };


  /** Overloads operator '=' */
  CBratAlgoFilterLanczos& operator=(const CBratAlgoFilterLanczos &copy);

  /** Dump function */
  virtual void Dump(std::ostream& fOut = std::cerr);

  static double Lanczos(uint32_t support, double cutOffFrequency, double x);

 
protected:
  enum ComputeMode {ModeHorizontal, ModeVertical};

protected:

  void Init();
  void Set(const CBratAlgoFilterLanczos &o);
  //Save current values to previous values previous values.
  virtual void SetPreviousValues(bool fromProduct);
  virtual void SetNextValues();

  void SetBeginOfFile();
  void SetEndOfFile();

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

  double m_lanczos;

  uint32_t m_cutOffPeriod;
  double m_cutOffFrequency;


  CDoubleArray m_initialWeights;

  //CDoubleArray m_weights;

  static const uint32_t m_WINDOW_PARAM_INDEX;
  static const uint32_t m_CUTOFF_PERIOD_PARAM_INDEX;
  static const uint32_t m_VALID_PARAM_INDEX;
  static const uint32_t m_EXTRAPOLATE_PARAM_INDEX;

  static const uint32_t m_INPUT_PARAMS;


};


/** @} */
}

#endif // !defined(_BratAlgoFilterLanczos_h_)
