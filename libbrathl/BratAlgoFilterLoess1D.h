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
#if !defined(_BratAlgoFilterLoess1D_h_)
#define _BratAlgoFilterLoess1D_h_

#include <string>

#include "common/tools/brathl_error.h"
#include "brathl.h" 

#include "Tools.h" 
#include "BratAlgoFilterLoess.h" 


using namespace brathl;

namespace brathl
{
//-------------------------------------------------------------
//------------------- CBratAlgoFilterLoess1D class --------------------
//-------------------------------------------------------------


/** \addtogroup Algorithm Algorithms classes
  @{ */

/** 
  Algorithm base class. 
*/

class CBratAlgoFilterLoess1D : public CBratAlgoFilterLoess 
{
public:
  /** Default contructor */
  CBratAlgoFilterLoess1D();
  /** Copy contructor */
  CBratAlgoFilterLoess1D(const CBratAlgoFilterLoess1D	&copy);

  /** Destructor */
  virtual ~CBratAlgoFilterLoess1D() {};

  virtual std::string GetName() const override { return "BratAlgoFilterLoessAtp"; }
  
  virtual std::string GetDescription() const override { return "Loess filter (smoothing filter) for one-dimensional data (e.g. along-track data) as input data source. "
                                           "Description to be completed... . "
                                           "The output value unit depends on the variable (data) filtered" ; }
  
  virtual uint32_t GetNumInputParam() const override { return CBratAlgoFilterLoess1D::m_INPUT_PARAMS; }
  
  virtual std::string GetInputParamDesc(uint32_t indexParam) const override {
        std::string value = "";
        switch (indexParam) 
        {
          case 0: value = CBratAlgoFilterKernel::m_VAR_PARAM_DESCR; break;
          case 1: value = "The input data (X values) used to compute weights."; break;
          case 2: value = "Window/region size. The parameter must be a constant value."; break;
          case 3: value = CBratAlgoFilterKernel::m_VALID_PARAM_DESCR; break;
          //case 4: value = CBratAlgoFilterKernel::m_EXTRAPOLATE_PARAM_DESCR; break;
          default: value = CTools::Format("CBratAlgoFilterLoess1D#GetInputParamDesc: out of range parameter index %d. Valid range is [0,%d].",
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
          case 2: value = CBratAlgorithmParam::T_INT; break;
          case 3: value = CBratAlgorithmParam::T_INT; break;
          //case 4: value = CBratAlgorithmParam::T_INT; break;
          default: throw CAlgorithmException(CTools::Format("CBratAlgoFilterLoess1D#GetInputParamFormat: out of range parameter index %d. Valid range is [0,%d].",
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
          case 1: value = "this unit depends on the input data (variable or Brat expression)"; break;
          case 2: value = "count"; break;
          case 3: value = "count"; break;
          //case 4: value = "count"; break;
          default: value = CTools::Format("CBratAlgoFilterLoess1D#GetInputParamUnit: out of range parameter index %d. Valid range is [0,%d].",
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
          case 1: value = "X"; break;
          case 2: value = "WindowLength"; break;
          case 3: value = CBratAlgoFilterKernel::m_VALID_PARAM_NAME; break;
          //case 4: value = CBratAlgoFilterKernel::m_EXTRAPOLATE_PARAM_NAME; break;
          default: value = CTools::Format("CBratAlgoFilterLoess1D#GetParamName: out of range parameter index %d. Valid range is [0,%d].",
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
          case 2: break;
          case 3: break;
          //case 4: value = 0.0; break;
          default: break;

        }
        return value;
  };

    /** Overloads operator '=' */
  CBratAlgoFilterLoess1D& operator=(const CBratAlgoFilterLoess1D &copy);

  virtual uint32_t GetDataWindowSize() override { return m_dataWindowLength; }

  /** Dump function */
  virtual void Dump(std::ostream& fOut = std::cerr) override;
 
protected:

  void Init();
  void Set(const CBratAlgoFilterLoess1D &copy);
    
  virtual void SetPreviousValues(bool fromProduct) override;
  virtual void SetNextValues() override;

  virtual void CheckVarExpression(uint32_t index) override;

  virtual void SetBeginOfFile() override;
  virtual void SetEndOfFile() override;

  virtual void TreatLeftEdge1D(uint32_t shiftSymmetry, uint32_t index) override;
  virtual void TreatRightEdge1D(uint32_t shiftSymmetry, uint32_t index) override;
  virtual void RemoveFirstItemDataWindow1D() override;
  virtual void InsertCurrentValueDataWindow1D() override;

  double ComputeLoess();

  double Tricube(double u, double t);
  double ApplyFilter();

  // This function computes the best-fit linear regression coefficients (c0,c1) of the model Y = c_0 + c_1 X for the weighted datasets (x, y), 
  // two vectors of length n with strides xstride and ystride. The vector w, of length n and stride wstride, 
  // specifies the weight of each datapoint. The weight is the reciprocal of the variance for each datapoint in y.
  // The covariance matrix for the parameters (c0, c1) is estimated from weighted data and returned 
  // via the parameters (cov00, cov01, cov11). The weighted sum of squares of the residuals from the best-fit line, 
  // \chi^2, is returned in chisq. 
  // From gnu gsl library : http://www.gnu.org/s/gsl/manual/html_node/Linear-regression.html


  void FitWLinear(const double *x, const uint32_t xstride,
                  const double *w, const uint32_t wstride,
                  const double *y, const uint32_t ystride,
                  const uint32_t n,
                  double *c0, double *c1,
                  double *cov_00, double *cov_01, double *cov_11,
                  double *chisq);

  // This function uses the best-fit linear regression coefficients c0,c1 and their estimated covariance cov00,cov01,cov11 
  // to compute the fitted function y and its standard deviation y_err for the model Y = c_0 + c_1 X at the point x. 
  // From gnu gsl library : http://www.gnu.org/s/gsl/manual/html_node/Linear-regression.html
  void FitLinearEst (const double x, const double c0, const double c1, 
                     const double cov00, const double cov01, const double cov11,
                     double *y, double *y_err);

protected:

  CDoubleArray m_distances;
  CDoubleArray m_sortedDistances;

  double m_xValue;
  double m_xValueNext;
  double m_xValuePrev;

  CDoubleArray m_xDataWindow;

  static const uint32_t m_WINDOW_PARAM_INDEX;
  static const uint32_t m_VALID_PARAM_INDEX;
  static const uint32_t m_EXTRAPOLATE_PARAM_INDEX;
  static const uint32_t m_X_PARAM_INDEX;

  static const uint32_t m_INPUT_PARAMS;


};


/** @} */
}


#endif // !defined(_BratAlgoFilterLoess1D_h_)
