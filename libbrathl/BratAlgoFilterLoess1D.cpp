
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
#include <algorithm>
#include <cmath>

#include "brathl.h" 


#include "TraceLog.h" 
#include "Exception.h" 
#include "Product.h" 

#include "BratAlgoFilterLoess1D.h" 

// When debugging changes all calls to “new” to be calls to “DEBUG_NEW” allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

using namespace brathl;



namespace brathl
{

const uint32_t CBratAlgoFilterLoess1D::m_INPUT_PARAMS = 4;

const uint32_t CBratAlgoFilterLoess1D::m_X_PARAM_INDEX = 1;
const uint32_t CBratAlgoFilterLoess1D::m_WINDOW_PARAM_INDEX = 2;
const uint32_t CBratAlgoFilterLoess1D::m_VALID_PARAM_INDEX = 3;
//const uint32_t CBratAlgoFilterLoess1D::m_EXTRAPOLATE_PARAM_INDEX = 4;

//-------------------------------------------------------------
//------------------- CBratAlgoFilterLoess1D class --------------------
//-------------------------------------------------------------

CBratAlgoFilterLoess1D::CBratAlgoFilterLoess1D()
{
  Init();

}
//----------------------------------------
CBratAlgoFilterLoess1D::CBratAlgoFilterLoess1D(const CBratAlgoFilterLoess1D &copy)
{
  Init();

  Set(copy);

}
//----------------------------------------
void CBratAlgoFilterLoess1D::Init()
{
  SetBeginOfFile();
}

//----------------------------------------
void CBratAlgoFilterLoess1D::Set(const CBratAlgoFilterLoess1D &o)
{
  CBratAlgoFilterLoess::Set(o);

  m_xValue = o.m_xValue;
  m_xValuePrev = o.m_xValuePrev;
  m_xValueNext = o.m_xValueNext;

  m_xDataWindow = o.m_xDataWindow;
  m_distances = o.m_distances;
  m_sortedDistances = o.m_sortedDistances;

}


//----------------------------------------
CBratAlgoFilterLoess1D& CBratAlgoFilterLoess1D::operator=(const CBratAlgoFilterLoess1D &copy)
{
  if (this == &copy)
  {
    return *this;
  }

  Set(copy);

  return *this;
}

//----------------------------------------
void CBratAlgoFilterLoess1D::SetBeginOfFile()
{
  CBratAlgoFilterLoess::SetBeginOfFile();

  CTools::SetDefaultValue(m_xValue);
  CTools::SetDefaultValue(m_xValueNext);
  CTools::SetDefaultValue(m_xValuePrev);

}

//----------------------------------------
void CBratAlgoFilterLoess1D::SetEndOfFile()
{
  CBratAlgoFilterLoess::SetEndOfFile();

  CTools::SetDefaultValue(m_xValueNext);
}

//----------------------------------------
void CBratAlgoFilterLoess1D::TreatLeftEdge1D(uint32_t shiftSymmetry, uint32_t index)
{
  CBratAlgoFilterLoess::TreatLeftEdge1D(shiftSymmetry, index);
  m_xDataWindow.InsertAt(m_xDataWindow.begin(), *(m_xDataWindow.begin() + shiftSymmetry + (index * 2)));
}
//----------------------------------------
void CBratAlgoFilterLoess1D::TreatRightEdge1D(uint32_t shiftSymmetry, uint32_t index)
{
  CBratAlgoFilterLoess::TreatRightEdge1D(shiftSymmetry, index);
  m_xDataWindow.InsertAt(m_xDataWindow.end(), *(m_xDataWindow.end() - (index * 2) - 1 - shiftSymmetry));
}
//----------------------------------------
void CBratAlgoFilterLoess1D::RemoveFirstItemDataWindow1D()
{
  CBratAlgoFilterLoess::RemoveFirstItemDataWindow1D();
  m_xDataWindow.Erase(m_xDataWindow.begin());
}
//----------------------------------------
void CBratAlgoFilterLoess1D::InsertCurrentValueDataWindow1D()
{
  CBratAlgoFilterLoess::InsertCurrentValueDataWindow1D();
  m_xDataWindow.Insert(m_xValue);
}
//----------------------------------------
double CBratAlgoFilterLoess1D::Run(CVectorBratAlgorithmParam& args)
{
  int32_t iRecord = m_callerProduct->GetCurrentRecordNumber();

  if (iRecord == m_callerProductRecordPrev)
  {
    // Do nothing: data have been already computed
    return m_loess;
  }

  CTools::SetDefaultValue(m_loess);
  
  OpenProductFile();

  // Gets the next record regarding to the current product record.
  // and save previous values.
  //this->GetNextData();

  // Get current parameter values (included expression value)
  SetParamValues(args);

  if (m_dataWindowLength > static_cast<uint32_t>(m_nProductRecords))
  {
    throw CAlgorithmException(CTools::Format("'%s' algorithm can't be applied because the window size parameter (%d) is greater than the number of data values (%d) within the data file (%s). "
                                             "You have to decrease the window size parameter", 
                                              this->GetName().c_str(), m_dataWindowLength, m_nProductRecords, m_currentFileName.c_str()),
                              this->GetName(), BRATHL_LOGIC_ERROR);
  }

  if (m_dataWindowLength < 1)
  {
    PrepareReturn();
    return m_loess;
  }

  if (m_dataWindowLength == 1)
  {
    m_loess = m_varValue;
    PrepareReturn();
    return m_loess;
  }

  // If 'default value' and no extrapolation then returns
  //if (CTools::IsDefaultValue(m_varValue) && (m_extrapolate == 0))
  // If 'default value' then returns
  if (CTools::IsDefaultValue(m_varValue))
  {
    PrepareReturn();
    return m_loess;
  }
 
  uint32_t shiftSymmetry = 0;

  PrepareDataValues1D(shiftSymmetry);

  ComputeLoess();

  PrepareReturn();

  return m_loess;
}


//----------------------------------------
double CBratAlgoFilterLoess1D::Tricube(double u, double t)
{
  if (CTools::IsDefaultValue(u))
  {
    return u;
  }
  if (CTools::IsDefaultValue(t))
  {
    return t;
  }

  // u is a distance and hence should be strictly positive.
  if (u < 0) 
  {
    throw CAlgorithmException(CTools::Format("'%s' algorithm can't be applied because an error has occcured the 'CBratAlgoFilterLoess1D::Tricube' function. "
                                              "Distance 'u' (%f) is strictly negative. This should not happen. "
                                              "\nRecord number is %d -  Distance max 't' is %f - Variable value is %f",
                                              this->GetName().c_str(), 
                                              u,  
                                              m_callerProduct->GetCurrentRecordNumber(), t, m_varValue),
                              this->GetName(), BRATHL_LOGIC_ERROR);
  }

  // 0 <= u < t
  if ( (0.0 <= u) && (u < t) )
  {
    // (1 - (u/t)^3)^3
    return pow( ( 1.0 - pow(u/t, 3)), 3 );
  }
  // u >= t
  else 
  {
    return 0.0;
  }

}
//----------------------------------------
void CBratAlgoFilterLoess1D::FitWLinear(const double *x, const uint32_t xstride,
                                           const double *w, const uint32_t wstride,
                                           const double *y, const uint32_t ystride,
                                           const uint32_t n,
                                           double *c0, double *c1,
                                           double *cov_00, double *cov_01, double *cov_11,
                                           double *chisq)
{

  // compute the weighted means and weighted deviations from the means 
  // wm denotes a "weighted mean", wm(f) = (sum_i w_i f_i) / (sum_i w_i)

  double W = 0.0;
  double wm_x = 0.0;
  double wm_y = 0.0;
  double wm_dx2 = 0.0;
  double wm_dxdy = 0.0;

  uint32_t i;

  for (i = 0; i < n; i++)
    {
      const double wi = w[i * wstride];

      if (CTools::IsDefaultValue(w[i * wstride]) || CTools::IsDefaultValue(x[i * xstride]) ||  CTools::IsDefaultValue(y[i * ystride]))
      {
        continue;
      }

      if (wi > 0.0)
        {
          W += wi;
          wm_x += (x[i * xstride] - wm_x) * (wi / W);
          wm_y += (y[i * ystride] - wm_y) * (wi / W);
        }
    }

  // reset the total weight 
  W = 0.0;                       

  for (i = 0; i < n; i++)
    {

      if (CTools::IsDefaultValue(w[i * wstride]) || CTools::IsDefaultValue(x[i * xstride]) ||  CTools::IsDefaultValue(y[i * ystride]))
      {
        continue;
      }

      const double wi = w[i * wstride];
      if (wi > 0)
        {
          const double dx = x[i * xstride] - wm_x;
          const double dy = y[i * ystride] - wm_y;

          W += wi;
          wm_dx2 += (dx * dx - wm_dx2) * (wi / W);
          wm_dxdy += (dx * dy - wm_dxdy) * (wi / W);
        }
    }

  // In terms of y = a + b * x 

  {
    double d2 = 0;
    double b = wm_dxdy / wm_dx2;
    double a = wm_y - wm_x * b;

    *c0 = a;
    *c1 = b;

    *cov_00 = (1 / W) * (1 + wm_x * wm_x / wm_dx2);
    *cov_11 = 1 / (W * wm_dx2);

    *cov_01 = -wm_x / (W * wm_dx2);

    // Compute chi^2 = \sum w_i (y_i - (a + b * x_i))^2 

    for (i = 0; i < n; i++)
    {
      if (CTools::IsDefaultValue(w[i * wstride]) || CTools::IsDefaultValue(x[i * xstride]) ||  CTools::IsDefaultValue(y[i * ystride]))
      {
        continue;
      }

        const double wi = w[i * wstride];

        if (wi > 0)
          {
            const double dx = x[i * xstride] - wm_x;
            const double dy = y[i * ystride] - wm_y;
            const double d = dy - b * dx;
            d2 += wi * d * d;
          }
    }

    *chisq = d2;
  }

}

//----------------------------------------
void CBratAlgoFilterLoess1D::FitLinearEst(const double x, const double c0, const double c1, 
                                          const double cov00, const double cov01, const double cov11,
                                          double *y, double *y_err)
{
  *y = c0 + c1 * x;
  *y_err = sqrt (cov00 + x * (2 * cov01 + cov11 * x));
}
//----------------------------------------
double CBratAlgoFilterLoess1D::ApplyFilter()
{
  uint32_t windowSize = GetDataWindowSize();

  // Compute the best-fit linear regression coefficients (c0, c1) of the model Y = c_0 + c_1 X for the weighted
  // dataset (x, y).
  double c0;
  double c1;
  double cov00;
  double cov01;
  double cov11;
  double chisq;
  double y;
  double yErr;

  CBratAlgoFilterLoess1D::FitWLinear(m_xDataWindow.data(), 1, m_initialWeights.data(), 1, m_rawDataWindow.data(), 1, windowSize, &c0, &c1, &cov00, &cov01, &cov11, &chisq);
 
  // Compute the fitted function y and its standard deviation yErr for the model Y = c_0 + c_1 X at the point x.
  CBratAlgoFilterLoess1D::FitLinearEst(m_xValue, c0, c1, cov00, cov01, cov11, &y, &yErr);

  return y;
}

//----------------------------------------
double CBratAlgoFilterLoess1D::ComputeLoess()
{
  CTools::SetDefaultValue(m_loess);

  uint32_t windowSize = GetDataWindowSize();

  m_distances.resize(windowSize);
  m_sortedDistances.resize(windowSize);
  m_initialWeights.resize(windowSize);

  // Compute distances.
  for (uint32_t i = 0; i < windowSize; i++)
  {
    if (CTools::IsDefaultValue(m_xDataWindow.at(i)))
    {
      CTools::SetDefaultValue(m_distances[i]); 
      continue;
    }

    m_distances[i] = CTools::Abs(m_xValue - m_xDataWindow.at(i));
  }
  
  m_sortedDistances = m_distances;
  // Sort distances in order from smallest to largest.
  std::sort(m_sortedDistances.begin(), m_sortedDistances.end());

  double maxDistance;
  CTools::SetDefaultValue(maxDistance);

  // Get max distance:
  // Last 'non default value' value of the sorted array.
  for (uint32_t i = windowSize - 1; i >= 0; i--)
  {
    if (!CTools::IsDefaultValue(m_sortedDistances.at(i)))
    {
      maxDistance = (m_sortedDistances.at(i)); 
      break;
    }
  }




  uint32_t nbMissingValue = 0;
  // Compute weights.
  for (uint32_t i = 0; i < windowSize; i++)
  {
    m_initialWeights[i] = this->Tricube(m_distances.at(i), maxDistance);

    if (CTools::IsDefaultValue(m_initialWeights.at(i)) && CTools::IsDefaultValue(m_rawDataWindow.at(i)))
    {
      nbMissingValue++;
    }
    else if (CTools::IsDefaultValue(m_initialWeights.at(i)))
    {
      nbMissingValue++;
    }
    else if (CTools::IsDefaultValue(m_rawDataWindow.at(i)))
    {
      nbMissingValue++;
    }

  }

  uint32_t nbValidPts = windowSize - nbMissingValue;

  if (nbValidPts < m_validPts)
  {
    return m_loess;
  }

  m_loess = ApplyFilter();


#if _DEBUG_BRAT_ALGO    
  std::string str;

  CDoubleArray::iterator it;

  CTrace::Tracer(3, CTools::Format("m_xDataWindow contains (%d elements):", m_xDataWindow.size()));
  str.clear();
  for (it = m_xDataWindow.begin(); it != m_xDataWindow.end(); it++)
  {
    if (CTools::IsDefaultValue(*it))
    {
      str.append("DV ");
    }
    else
    {
      str.append(CTools::Format("%f ", *it));
    }
  }

  CTrace::Tracer(3, str);

  CTrace::Tracer(3, CTools::Format("m_rawDataWindow contains (%d elements):", m_rawDataWindow.size()));
  str.clear();
  for (it = m_rawDataWindow.begin(); it != m_rawDataWindow.end(); it++)
  {
    if (CTools::IsDefaultValue(*it))
    {
      str.append("DV ");
    }
    else
    {
      str.append(CTools::Format("%f ", *it));
    }
  }

  CTrace::Tracer(3, str);

  CTrace::Tracer(3, CTools::Format("loess is: %f", m_loess));

  CTrace::Tracer(3, CTools::Format("m_distances contains (%d elements):", m_distances.size()));
  str.clear();
  for (it = m_distances.begin(); it != m_distances.end(); it++)
  {
    if (CTools::IsDefaultValue(*it))
    {
      str.append("DV ");
    }
    else
    {
      str.append(CTools::Format("%f ", *it));
    }
  }

  CTrace::Tracer(3, str);

  CTrace::Tracer(3, CTools::Format("maxDistance is %f:", maxDistance));
  
  str.clear();
  str.append("\ncomputed weights are: ");

  for (uint32_t i  = 0; i < windowSize; i++)
  {
	  str.append(CTools::Format("%f ", m_initialWeights[i]));
  }
  str.append("\n");

  CTrace::Tracer(str);
#endif

  return m_loess;

}

//----------------------------------------
void CBratAlgoFilterLoess1D::SetPreviousValues(bool fromProduct) 
{
  if (fromProduct)
  {
    m_varValuePrev = this->GetDataValue(CBratAlgoFilterLoess1D::m_VAR_PARAM_INDEX);
    m_xValuePrev = this->GetDataValue(CBratAlgoFilterLoess1D::m_X_PARAM_INDEX);
  }
  else
  {
    CBratAlgoFilterLoess::SetPreviousValues(fromProduct);
    m_varValuePrev = m_varValue;
    m_xValuePrev = m_xValue;
  }

}

//----------------------------------------
void CBratAlgoFilterLoess1D::SetNextValues() 
{
  m_varValueNext = this->GetDataValue(CBratAlgoFilterLoess1D::m_VAR_PARAM_INDEX);
  m_rawDataWindow.Insert(m_varValueNext);

  m_xValueNext = this->GetDataValue(CBratAlgoFilterLoess1D::m_X_PARAM_INDEX);
  m_xDataWindow.Insert(m_xValueNext);
}



//----------------------------------------
void CBratAlgoFilterLoess1D::CheckInputParams(CVectorBratAlgorithmParam& args) 
{
  CBratAlgorithmBase::CheckInputParams(args);

  for (uint32_t indexParam = 0 ; indexParam < CBratAlgoFilterLoess1D::m_INPUT_PARAMS ; indexParam++)
  {
    CheckInputTypeParams(indexParam, this->GetInputParamFormat(indexParam), args);
  }

}
//----------------------------------------
void CBratAlgoFilterLoess1D::SetParamValues(CVectorBratAlgorithmParam& args)
{    
  CheckInputParams(args);

  // Set variable
  m_varValue = args.at(CBratAlgoFilterLoess1D::m_VAR_PARAM_INDEX).GetValueAsDouble();
  //// Set only the first value, others values will be stored in the 'SetNextValues'
  //if (m_rawDataWindow.size() <= 0)
  //{
  //  m_rawDataWindow.Insert(m_varValue);
  //}

  // Set x param
  m_xValue = args.at(CBratAlgoFilterLoess1D::m_X_PARAM_INDEX).GetValueAsDouble();
  //if (m_xDataWindow.size() <= 0)
  //{
  //  m_xDataWindow.Insert(m_xValue);
  //}

  int32_t valueInt32;

  // Set data window size (once)
  if (CTools::IsDefaultValue(m_dataWindowLength))
  {
    this->CheckConstantParam(CBratAlgoFilterLoess1D::m_WINDOW_PARAM_INDEX);
    
    valueInt32 = args.at(CBratAlgoFilterLoess1D::m_WINDOW_PARAM_INDEX).GetValueAsInt();
    if (valueInt32 <= 0)
    {
      throw CAlgorithmException(CTools::Format("'%s' algorithm can't be applied because the window size parameter (%d) is not greater than 0. " 
                                               "You have to adjust the '%s' parameter.", 
                                                this->GetName().c_str(), valueInt32, 
                                                this->GetParamName(CBratAlgoFilterLoess1D::m_WINDOW_PARAM_INDEX).c_str()),
                                this->GetName(), BRATHL_LOGIC_ERROR);
    }

    m_dataWindowLength = valueInt32;
    if (CTools::IsDefaultValue(valueInt32))
    {
      m_dataWindowLength = 9;
    }
    if (CTools::IsEven(m_dataWindowLength))
    {
      throw CAlgorithmException(CTools::Format("'%s' algorithm can't be applied because the window size parameter (%d) is not an odd number." 
                                               "You have to adjust the '%s' parameter.", 
                                                this->GetName().c_str(), m_dataWindowLength,
                                                this->GetParamName(CBratAlgoFilterLoess1D::m_WINDOW_PARAM_INDEX).c_str()),
                                this->GetName(), BRATHL_LOGIC_ERROR);

    }
  }

  // Set valid points limit (once)
  if (CTools::IsDefaultValue(m_validPts))
  {
    this->CheckConstantParam(CBratAlgoFilterLoess1D::m_VALID_PARAM_INDEX);

    valueInt32 = args.at(CBratAlgoFilterLoess1D::m_VALID_PARAM_INDEX).GetValueAsInt();
    if (valueInt32 <= 0)
    {
      throw CAlgorithmException(CTools::Format("'%s' algorithm can't be applied because the '%s' parameter (%d) is less than or equal to 0. " 
                                               "You have to adjust the '%s'  parameter.", 
                                                this->GetName().c_str(), 
                                                this->GetParamName(CBratAlgoFilterLoess1D::m_VALID_PARAM_INDEX).c_str(),
                                                valueInt32, 
                                                this->GetParamName(CBratAlgoFilterLoess1D::m_VALID_PARAM_INDEX).c_str()),
                                this->GetName(), BRATHL_LOGIC_ERROR);
    }

    m_validPts = valueInt32;
    if (CTools::IsDefaultValue(valueInt32))
    {
      m_validPts = GetDataWindowSize()/2;
    }

    if (m_dataWindowLength < m_validPts)
    {
      throw CAlgorithmException(CTools::Format("'%s' algorithm can't be applied because the window size parameter (%d) is less than the minimum number of valid points parameter (%d). "
                                               "You have to adjust either the '%s' parameter or the '%s' parameter.", 
                                                this->GetName().c_str(), m_dataWindowLength, m_validPts,
                                                this->GetParamName(CBratAlgoFilterLoess1D::m_WINDOW_PARAM_INDEX).c_str(),
                                                this->GetParamName(CBratAlgoFilterLoess1D::m_VALID_PARAM_INDEX).c_str()),
                                this->GetName(), BRATHL_LOGIC_ERROR);
    }

  }

  //// Set extrapolate flag (once)
  //if (CTools::IsDefaultValue(m_extrapolate))
  //{
  //  SetParamValueExtrapolate(args, CBratAlgoFilterLoess1D::m_EXTRAPOLATE_PARAM_INDEX);
  //}

}
//----------------------------------------
void CBratAlgoFilterLoess1D::CheckVarExpression(uint32_t index)
{  

}

//----------------------------------------
void CBratAlgoFilterLoess1D::Dump(std::ostream& fOut /*= std::cerr*/)
{
   if (CTrace::IsTrace() == false)
   { 
      return;
   }

  fOut << "==> Dump a CBratAlgoFilterLoess1D Object at "<< this << std::endl;
  CBratAlgoFilterLoess::Dump(fOut);

  fOut << "m_xValue: " << m_xValue << std::endl;
  fOut << "m_xValueNext: " << m_xValueNext << std::endl;
  fOut << "m_xValuePrev: " << m_xValuePrev << std::endl;

  fOut << "m_xDataWindow: " << std::endl;
  m_xDataWindow.Dump(fOut);
  fOut << "m_distances: " << std::endl;
  m_distances.Dump(fOut);
  fOut << "m_sortedDistances: " << std::endl;
  m_sortedDistances.Dump(fOut);

  fOut << "==> END Dump a CBratAlgoFilterLoess1D Object at "<< this << std::endl;


}

} // end namespace
