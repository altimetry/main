
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
#include <cmath>

#include "brathl.h" 


#include "TraceLog.h" 
#include "Exception.h" 

#include "BratAlgoFilterGaussian.h" 

// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

using namespace brathl;



namespace brathl
{

const uint32_t CBratAlgoFilterGaussian::m_INPUT_PARAMS = 6;

const uint32_t CBratAlgoFilterGaussian::m_WINDOW_PARAM_INDEX = 1;
const uint32_t CBratAlgoFilterGaussian::m_SIGMA_PARAM_INDEX = 2;
const uint32_t CBratAlgoFilterGaussian::m_TRUNC_PARAM_INDEX = 3;
const uint32_t CBratAlgoFilterGaussian::m_VALID_PARAM_INDEX = 4;
const uint32_t CBratAlgoFilterGaussian::m_EXTRAPOLATE_PARAM_INDEX = 5;


//-------------------------------------------------------------
//------------------- CBratAlgoFilterGaussian class --------------------
//-------------------------------------------------------------

CBratAlgoFilterGaussian::CBratAlgoFilterGaussian()
{
  Init();

}
//----------------------------------------
CBratAlgoFilterGaussian::CBratAlgoFilterGaussian(const CBratAlgoFilterGaussian &copy)
{
  Init();

  Set(copy);

}

//----------------------------------------
CBratAlgoFilterGaussian::~CBratAlgoFilterGaussian()
{

}
//----------------------------------------
void CBratAlgoFilterGaussian::Init()
{
  CTools::SetDefaultValue(m_sigma);
  CTools::SetDefaultValue(m_trunc);

  SetBeginOfFile();
}

//----------------------------------------
void CBratAlgoFilterGaussian::Set(const CBratAlgoFilterGaussian &o)
{
  CBratAlgoFilterKernel::Set(o);

  m_sigma = o.m_sigma;
  m_trunc = o.m_trunc;
  m_gaussian = o.m_gaussian;
  m_initialWeights = o.m_initialWeights;
  //m_weights = o.m_weights;
}


//----------------------------------------
CBratAlgoFilterGaussian& CBratAlgoFilterGaussian::operator=(const CBratAlgoFilterGaussian &copy)
{
  if (this == &copy)
  {
    return *this;
  }

  Set(copy);

  return *this;
}

//----------------------------------------
void CBratAlgoFilterGaussian::SetBeginOfFile()
{
  CBratAlgoFilterKernel::SetBeginOfFile();

  CTools::SetDefaultValue(m_gaussian);  
}

//----------------------------------------
void CBratAlgoFilterGaussian::SetEndOfFile()
{
  CBratAlgoFilterKernel::SetEndOfFile();

}
//----------------------------------------
void CBratAlgoFilterGaussian::SetPreviousValues(bool fromProduct) 
{
  CBratAlgoFilterKernel::SetPreviousValues(fromProduct);

}
//----------------------------------------
void CBratAlgoFilterGaussian::SetNextValues() 
{
  CBratAlgoFilterKernel::SetNextValues();
}
//----------------------------------------
void CBratAlgoFilterGaussian::PrepareReturn() 
{
  int32_t iRecord = m_callerProduct->GetCurrentRecordNumber();
  m_callerProductRecordPrev = iRecord;

  if (iRecord + 1 >= m_nProductRecords)
  {
    SetEndOfFile();
  }

}
//----------------------------------------
void CBratAlgoFilterGaussian::CheckInputParams(CVectorBratAlgorithmParam& args) 
{
  CBratAlgorithmBase::CheckInputParams(args);

  for (uint32_t indexParam = 0 ; indexParam < CBratAlgoFilterGaussian::m_INPUT_PARAMS ; indexParam++)
  {
    if (indexParam == CBratAlgoFilterGaussian::m_VAR_PARAM_INDEX)
    {
      CheckInputTypeParams(indexParam, m_expectedTypes, args);
      continue;
    }
    CheckInputTypeParams(indexParam, this->GetInputParamFormat(indexParam), args);
  }

  CBratAlgorithmParam* algoParam = &(args.at(CBratAlgoFilterGaussian::m_VAR_PARAM_INDEX));
  if (algoParam->GetTypeVal() == CBratAlgorithmParam::T_VECTOR_DOUBLE)
  {
    m_varValueArray = algoParam->GetValueAsVectDouble();
  }

}
//----------------------------------------
void CBratAlgoFilterGaussian::SetParamValues(CVectorBratAlgorithmParam& args)
{    
  CheckInputParams(args);

  // Set variable
  m_varValue = args.at(CBratAlgoFilterGaussian::m_VAR_PARAM_INDEX).GetValueAsDouble();

  int32_t valueInt32;

  // Set data window size (once)
  if (CTools::IsDefaultValue(m_dataWindowLength))
  {
    this->CheckConstantParam(CBratAlgoFilterGaussian::m_WINDOW_PARAM_INDEX);

    valueInt32 = args.at(CBratAlgoFilterGaussian::m_WINDOW_PARAM_INDEX).GetValueAsInt();
    if (valueInt32 <= 0)
    {
      throw CAlgorithmException(CTools::Format("'%s' algorithm can't be applied because the window size parameter (%d) is not greater than 0. " 
                                               "You have to adjust the '%s' parameter.", 
                                                this->GetName().c_str(), valueInt32, 
                                                this->GetParamName(CBratAlgoFilterGaussian::m_WINDOW_PARAM_INDEX).c_str()),
                                this->GetName(), BRATHL_LOGIC_ERROR);
    }

    m_dataWindowLength = valueInt32;
    if (CTools::IsDefaultValue(valueInt32))
    {
      m_dataWindowLength = 3;
    }
    if (CTools::IsEven(m_dataWindowLength))
    {
      throw CAlgorithmException(CTools::Format("'%s' algorithm can't be applied because the window size parameter (%d) is not an odd number." 
                                               "You have to adjust the '%s' parameter.", 
                                                this->GetName().c_str(), m_dataWindowLength,
                                                this->GetParamName(CBratAlgoFilterGaussian::m_WINDOW_PARAM_INDEX).c_str()),
                                this->GetName(), BRATHL_LOGIC_ERROR);

    }

    m_dataWindowWidth = m_dataWindowLength;
    m_dataWindowHeight = m_dataWindowLength;
  }

  // Set valid points limit (once)
  if (CTools::IsDefaultValue(m_validPts))
  {
    this->CheckConstantParam(CBratAlgoFilterGaussian::m_VALID_PARAM_INDEX);

    valueInt32 = args.at(CBratAlgoFilterGaussian::m_VALID_PARAM_INDEX).GetValueAsInt();
    if (valueInt32 <= 0)
    {
      throw CAlgorithmException(CTools::Format("'%s' algorithm can't be applied because the '%s' parameter (%d) is less than or equal to 0. " 
                                               "You have to adjust the '%s'  parameter.", 
                                                this->GetName().c_str(), 
                                                this->GetParamName(CBratAlgoFilterGaussian::m_VALID_PARAM_INDEX).c_str(),
                                                valueInt32, 
                                                this->GetParamName(CBratAlgoFilterGaussian::m_VALID_PARAM_INDEX).c_str()),
                                this->GetName(), BRATHL_LOGIC_ERROR);
    }

    m_validPts = valueInt32;
    if (CTools::IsDefaultValue(valueInt32))
    {
      m_validPts = GetDataWindowSize()/2;
    }

    if (GetDataWindowSize() < m_validPts)
    {
      throw CAlgorithmException(CTools::Format("'%s' algorithm can't be applied because the window size %d (%dx%d) is less than the minimum number of valid points parameter (%d). "
                                               "You have to adjust either the '%s' parameter or the '%s' parameter.", 
                                               this->GetName().c_str(), GetDataWindowSize(), m_dataWindowLength, m_dataWindowLength, m_validPts,
                                               this->GetParamName(CBratAlgoFilterGaussian::m_WINDOW_PARAM_INDEX).c_str(),
                                               this->GetParamName(CBratAlgoFilterGaussian::m_VALID_PARAM_INDEX).c_str()),
                                this->GetName(), BRATHL_LOGIC_ERROR);
    }
  }
  // Set trunc (once)
  if (CTools::IsDefaultValue(m_trunc))
  {
    this->CheckConstantParam(CBratAlgoFilterGaussian::m_TRUNC_PARAM_INDEX);

    valueInt32 = args.at(CBratAlgoFilterGaussian::m_TRUNC_PARAM_INDEX).GetValueAsInt();
    if (valueInt32 <= 0)
    {
      throw CAlgorithmException(CTools::Format("'%s' algorithm can't be applied because the '%s' parameter (%d) is not greater than 0. " 
                                               "You have to adjust the '%s'  parameter.", 
                                                this->GetName().c_str(), 
                                                this->GetParamName(CBratAlgoFilterGaussian::m_TRUNC_PARAM_INDEX).c_str(),
                                                valueInt32, 
                                                this->GetParamName(CBratAlgoFilterGaussian::m_TRUNC_PARAM_INDEX).c_str()),
                                this->GetName(), BRATHL_LOGIC_ERROR);
    }

    m_trunc = valueInt32;
    if (CTools::IsDefaultValue(valueInt32))
    {
      this->GetParamDefValue(CBratAlgoFilterGaussian::m_TRUNC_PARAM_INDEX, m_trunc);
    }
  }
  
  // Set sigma (once)
  if (CTools::IsDefaultValue(m_sigma))
  {
    this->CheckConstantParam(CBratAlgoFilterGaussian::m_SIGMA_PARAM_INDEX);
    m_sigma = args.at(CBratAlgoFilterGaussian::m_SIGMA_PARAM_INDEX).GetValueAsDouble();
    if (CTools::IsDefaultValue(m_sigma))
    {
      this->GetParamDefValue(CBratAlgoFilterGaussian::m_SIGMA_PARAM_INDEX, m_sigma);
    }
  }

  // Set extrapolate flag (once)
  if (CTools::IsDefaultValue(m_extrapolate))
  {
    SetParamValueExtrapolate(args, CBratAlgoFilterGaussian::m_EXTRAPOLATE_PARAM_INDEX);
  }


}
//----------------------------------------
void CBratAlgoFilterGaussian::PrepareDataValues2DComplexExpressionWithAlgo(CExpressionValue& exprValue)
{
  CBratAlgoFilter::PrepareDataValues2DComplexExpressionWithAlgo(exprValue, CBratAlgoFilterKernel::m_VAR_PARAM_INDEX);
}
//----------------------------------------
void CBratAlgoFilterGaussian::PrepareDataValues2DComplexExpression(CExpressionValue& exprValue)
{
  CBratAlgoFilter::PrepareDataValues2DComplexExpression(exprValue, CBratAlgoFilterKernel::m_VAR_PARAM_INDEX);

}
//----------------------------------------
void CBratAlgoFilterGaussian::PrepareDataValues2DOneField(CExpressionValue& exprValue)
{
  CBratAlgoFilter::PrepareDataValues2DOneField(exprValue, CBratAlgoFilterKernel::m_VAR_PARAM_INDEX);

}

//----------------------------------------
void CBratAlgoFilterGaussian::ResetRawDataWindow()
{
  m_rawDataWindow.RemoveAll();

  m_rawDataWindow.resize(m_dataWindowLength);

  std::fill(m_rawDataWindow.begin(), m_rawDataWindow.end(), CTools::m_defaultValueDOUBLE); 

}
//----------------------------------------
void CBratAlgoFilterGaussian::PrepareDataWindow(ComputeMode mode, const CDoubleArray& dataValue)
{
  // Fill data window with data values
  ResetRawDataWindow();

  if (mode == ModeHorizontal)
  {
#if _DEBUG_BRAT_ALGO    
    CTrace::Tracer(3, "===> Mode Horizontal  :");
#endif
    // ---------------
    // Mode Horizontal
    // ---------------
    PrepareDataWindowHorizontal(dataValue);
  }
  else
  {
#if _DEBUG_BRAT_ALGO    
    CTrace::Tracer(3, "===> Mode Vertical  :");
#endif
    // ---------------
    // Mode Vertical
    // ---------------
    PrepareDataWindowVertical(dataValue);
  }


#if _DEBUG_BRAT_ALGO    
  CTrace::Tracer(3, CTools::Format("===> m_varValue is %f", m_varValue));
  
  CTrace::Tracer(3, "===> WINDOW IS :");

  std::string str;
  for (uint32_t i = 0 ; i < m_dataWindowLength ; i++)
  {
    double value = m_rawDataWindow[i];
    if (CTools::IsDefaultValue(value))
    {
      str.append("DV ");
    }
    else
    {
      str.append(CTools::Format("%f ", value));
    }

  }

  CTrace::Tracer(3, str);

  CTrace::Tracer(3, "===> END WINDOW <=======");
#endif

}

//----------------------------------------
void CBratAlgoFilterGaussian::PrepareDataWindowHorizontal(const CDoubleArray& dataValue)
{
  int32_t offset = m_addHeightTop;
  // Copy all height (y) for this width
  std::copy(dataValue.begin(),
          dataValue.begin() + m_countY, 
          m_rawDataWindow.begin() + offset);


  // Treating left width edge
  // Data are extended symmetrically.

  for (int32_t w = 0 ; w < m_addHeightTop ; w++)
  {
    m_rawDataWindow[offset - w - 1]  = m_rawDataWindow[offset + w + 1];
  }

  // Treating right width edge
  // Data are extended symmetrically.
  offset = m_addHeightBottom;

  for (int32_t w = 0 ; w < m_addHeightBottom ; w++)
  {
    m_rawDataWindow[m_countY + w]  = m_rawDataWindow[m_countY - 2 - w];
  }

}

//----------------------------------------
void CBratAlgoFilterGaussian::PrepareDataWindowVertical(const CDoubleArray& dataValue)
{

  int32_t offset = m_addWidthLeft;
  uint32_t sizeData = dataValue.size();

  for (uint32_t i = 0 ; i < sizeData ; i++)
  {
    //int32_t from = i * m_countY;
    m_rawDataWindow[offset + i] = dataValue[i];
	}

  // Treating left width edge
  // Data are extended symmetrically.

  for (int32_t w = 0 ; w < m_addWidthLeft ; w++)
  {
    m_rawDataWindow[offset - w - 1]  = m_rawDataWindow[offset + w + 1];
  }

  // Treating right width edge
  // Data are extended symmetrically.
  offset = m_addWidthRight;

  for (int32_t w = 0 ; w < m_addWidthRight ; w++)
  {
    m_rawDataWindow[sizeData + w]  = m_rawDataWindow[sizeData - 2 - w];
  }

}

//----------------------------------------
void CBratAlgoFilterGaussian::ComputeInitialWeights()
{
  // Compute just once 
  if (m_initialWeights.size() > 0)
  {
    // Already computed
    return;
  }

  //uint32_t windowSize = GetDataWindowSize();
  uint32_t windowSize = m_dataWindowLength;

  m_initialWeights.resize(windowSize);

  uint32_t halfWindow = static_cast<uint32_t>(windowSize) / 2;
  
  // Set central weight
  m_initialWeights[halfWindow] = 1.0;
  
  // Set the rest of weights
  for (uint32_t i = 1; i < halfWindow + 1; i++)
  {
	  // Set support point
    double x = static_cast<double>(m_trunc) * m_sigma * static_cast<double>(i) / static_cast<double>(halfWindow);
	  
#if _DEBUG_BRAT_ALGO    
    CTrace::Tracer(CTools::Format("x = %f and exp = %f", x, exp(-x * x / (2. * m_sigma * m_sigma))));
#endif
    // Set corresponding symmetric weights
	  m_initialWeights[halfWindow + i] = exp(-x * x / (2. * m_sigma * m_sigma));
	  m_initialWeights[halfWindow - i] = m_initialWeights[halfWindow + i];
  }

#if _DEBUG_BRAT_ALGO    

  std::string str;
  str.append("initial weights are : ");

  for (uint32_t i  = 0; i < windowSize; i++)
  {
	  str.append(CTools::Format("%f ",  m_initialWeights[i]));
  }
  str.append("\n");

  CTrace::Tracer(str);

#endif

}

////----------------------------------------
//void CBratAlgoFilterGaussian::SumWeights(ComputeMode mode, uint32_t from)
//{
//  ComputeInitialWeights();
//
//  CUIntArray missingValue;
//  missingValue.resize(m_initialWeights.size());
//  m_weights.resize(m_initialWeights.size());
//
//  // Sum weight (exclude 'default value')
//  double sumWeight = 0.0;
//
//  for (uint32_t i = 0; i < m_dataWindowLength; i++)
//  {
//    if (CTools::IsDefaultValue(m_rawDataWindow.at(i)))
//    {
//      missingValue[i] = 1;
//    }
//    else
//    {
//  	  sumWeight += m_initialWeights[i];
//      missingValue[i] = 0;
//    }
//  }
//
//  // Scale weight
//  for (uint32_t i = 0; i < m_dataWindowLength; i++)
//  {
//    if (missingValue.at(i) == 1)
//    {
//      CTools::SetDefaultValue(m_weights[i]);
//    }
//    else
//    {
//      m_weights[i] = m_initialWeights[i] / sumWeight;
//    }
//  }
//
//  //std::string str;
//  //if (mode == ModeHorizontal)
//  //{
//  //  str.append("Mode Horizontal: ");
//  //}
//  //else
//  //{
//  //  str.append("Mode Vertical: ");
//  //}
//  //str.append("computed weights are: ");
//
//  //for (uint32_t i  = 0; i < m_dataWindowLength; i++)
//  //{
//	 // str.append(CTools::Format("%f ", m_weights[i]));
//  //}
//  //str.append("\n");
//
//  //CTrace::Tracer(str);
//}
//----------------------------------------
double CBratAlgoFilterGaussian::ApplyFilter(ComputeMode mode, uint32_t from)
{
  ComputeInitialWeights();

  //m_weights.resize(m_initialWeights.size());

  // Sum weight (exclude 'default value')
  double sumWeight = 0.0;

	//   Apply filter - calculate weighted mean
	double sum = 0.0;
  for (uint32_t i = 0; i < m_dataWindowLength; i++)
  {
    if (CTools::IsDefaultValue(m_rawDataWindow.at(i)))
    {
      continue;
    }

    sumWeight += m_initialWeights[i];

    sum += m_initialWeights.at(i) * m_rawDataWindow.at(i);
  }

#if _DEBUG_BRAT_ALGO    
  std::string str;
  if (mode == ModeHorizontal)
  {
    str.append("Mode Horizontal: ");
  }
  else
  {
    str.append("Mode Vertical: ");
  }
  str.append("computed weights are: ");

  for (uint32_t i  = 0; i < m_dataWindowLength; i++)
  {
	  str.append(CTools::Format("%f ", m_initialWeights[i]));
  }
  str.append("\n");
  str.append(CTools::Format("sumWeight is: %f \n", sumWeight));

  CTrace::Tracer(str);
#endif

  if (sumWeight == 0.0)
  {
    return sum;
  }

	return sum / sumWeight;
}

//----------------------------------------
void CBratAlgoFilterGaussian::PrepareDataReading2DX()
{
  //CBratAlgoFilterKernel::PrepareDataReading2D();

  uint32_t halfWindowWidth = m_dataWindowWidth/2;
  uint32_t halfWindowHeight = m_dataWindowHeight/2;

  m_countY = 0;
  m_countX = 0;
  m_addHeightBottom = 0;
  m_addHeightTop = 0;
  m_addWidthRight = 0;
  m_addWidthLeft = 0;
  m_gapWidth = 0;
  m_gapHeight = 0;

  if (CTools::IsEven(m_dataWindowWidth))
  {
    m_gapWidth = 1;
  }

  if (CTools::IsEven(m_dataWindowHeight))
  {
    m_gapHeight = 1;
  }


  // Set number of data to read for 2D fields
  m_field2DAsRef = NULL;

  CObMap::const_iterator itMap;
  for (itMap = m_fieldVarsCaller.begin(); itMap != m_fieldVarsCaller.end() ; itMap++)
  { 
    CFieldNetCdf* fieldCaller = CExternalFiles::GetFieldNetCdf(itMap->second);
    
    int32_t nbDims = fieldCaller->GetNbDims();
    if (nbDims != 2)
    {
      continue;
    }

    //fieldCaller->Dump(*CTrace::GetDumpContext());
  
    uint32_t indexX = fieldCaller->GetDimsIndexArray()[0];
    uint32_t indexY = fieldCaller->GetDimsIndexArray()[1];

    uint32_t dimX = fieldCaller->GetDimAt(0);
    uint32_t dimY = fieldCaller->GetDimAt(1);

    if (m_dataWindowWidth > dimX)
    {
      throw CAlgorithmException(CTools::Format("'%s' algorithm can't be applied because the window width parameter (%d) is greater than the X-dimension value (%d) of the variable '%s' within the data file (%s). "
                                               "You have to decrease the window width parameter", 
                                                this->GetName().c_str(), m_dataWindowWidth, dimX, itMap->first.c_str(), m_currentFileName.c_str()),
                                this->GetName(), BRATHL_LOGIC_ERROR);
    }

    if (m_dataWindowHeight > dimY)
    {
      throw CAlgorithmException(CTools::Format("'%s' algorithm can't be applied because the window height parameter (%d) is greater than the Y-dimension value (%d) of the variable '%s' within the data file (%s). "
                                               "You have to decrease the window width parameter", 
                                                this->GetName().c_str(), m_dataWindowHeight, dimY, itMap->first.c_str(), m_currentFileName.c_str()),
                                this->GetName(), BRATHL_LOGIC_ERROR);
    }
    
    CFieldNetCdf* field = CExternalFiles::GetFieldNetCdf(m_fieldVars.Exists(fieldCaller->GetName()));
    if (field->GetDimsIndexArray() == NULL)
    {
      field->NewDimIndexArray();
    }

    if (m_field2DAsRef == NULL)
    {
      m_field2DAsRef = field;
    }


    m_addWidthRight = 0;
    m_addWidthLeft = 0;
    int32_t minIndexX = 0;
    m_countX = m_dataWindowWidth;
    //int32_t minIndexX = indexX + incrX;
    //m_countX = countX;

    if (indexX < halfWindowWidth) 
    {
      m_addWidthLeft = halfWindowWidth - indexX;
      m_countX -= m_addWidthLeft;
    }
    else 
    {
      minIndexX = indexX - halfWindowWidth;
      if ((indexX + halfWindowWidth) >= dimX)
      {
        m_addWidthRight = (halfWindowWidth + indexX + 1) - dimX;
        m_countX -= m_addWidthRight;
      }
    }

    int32_t minIndexY = 0;
    m_countY = m_dataWindowHeight;
    m_addHeightBottom = 0;
    m_addHeightTop = 0;

    if (indexY < halfWindowHeight) 
    {
      m_addHeightTop = (halfWindowHeight - indexY);
      m_countY -= m_addHeightTop;
    }
    else 
    {
      minIndexY = indexY - halfWindowHeight;
      if ((indexY + halfWindowHeight) >= dimY)
      {
        m_addHeightBottom = (halfWindowHeight + indexY + 1) - dimY;
        m_countY -= m_addHeightBottom;
      }
    }
    
    // Read data from Netcdf file
    field->GetDimsIndexArray()[0] = minIndexX;
    field->GetDimsCountArray()[0] = m_countX;
    
    field->GetDimsIndexArray()[1] = minIndexY;
    field->GetDimsCountArray()[1] = m_countY;
    
  }

  // Set number of data to read for 1D fields
  for (itMap = m_fieldVars.begin(); itMap != m_fieldVars.end() ; itMap++)
  { 
    CFieldNetCdf* field = CExternalFiles::GetFieldNetCdf(itMap->second);
    
    int32_t nbDims = field->GetNbDims();
    if (nbDims != 1)
    {
      continue;
    }

    if (field->GetDimsIndexArray() == NULL)
    {
      field->NewDimIndexArray();
    }

    CFieldNetCdf* fieldTemp = CExternalFiles::GetFieldNetCdf(m_fieldDependOnXDim.Exists(itMap->first), false);
    if (fieldTemp != NULL)
    {
      field->GetDimsIndexArray()[0] = m_field2DAsRef->GetDimsIndexArray()[0];
      field->GetDimsCountArray()[0] = m_field2DAsRef->GetDimsCountArray()[0];
      continue;
    }

    fieldTemp = CExternalFiles::GetFieldNetCdf(m_fieldDependOnYDim.Exists(itMap->first), false);
    if (fieldTemp != NULL)
    {
      field->GetDimsIndexArray()[0] = m_field2DAsRef->GetDimsIndexArray()[1];
      field->GetDimsCountArray()[0] = m_field2DAsRef->GetDimsCountArray()[1];
    }

  }


}

//----------------------------------------
void CBratAlgoFilterGaussian::PrepareDataReading2DY()
{
  uint32_t halfWindowHeight = m_dataWindowHeight/2;

  m_countY = 0;
  m_countX = 0;
  m_addHeightBottom = 0;
  m_addHeightTop = 0;
  m_addWidthRight = 0;
  m_addWidthLeft = 0;
  m_gapWidth = 0;
  m_gapHeight = 0;

  if (CTools::IsEven(m_dataWindowWidth))
  {
    m_gapWidth = 1;
  }

  if (CTools::IsEven(m_dataWindowHeight))
  {
    m_gapHeight = 1;
  }


  // Set number of data to read for 2D fields
  m_field2DAsRef = NULL;

  CObMap::const_iterator itMap;
  for (itMap = m_fieldVarsCaller.begin(); itMap != m_fieldVarsCaller.end() ; itMap++)
  { 
    CFieldNetCdf* fieldCaller = CExternalFiles::GetFieldNetCdf(itMap->second);
    
    int32_t nbDims = fieldCaller->GetNbDims();
    if (nbDims != 2)
    {
      continue;
    }

    //fieldCaller->Dump(*CTrace::GetDumpContext());
  
    uint32_t indexX = fieldCaller->GetDimsIndexArray()[0];
    uint32_t indexY = fieldCaller->GetDimsIndexArray()[1];

    if (indexX == 1)
    {
      std::string msg = "qsdsqd";
    }

    uint32_t dimX = fieldCaller->GetDimAt(0);
    uint32_t dimY = fieldCaller->GetDimAt(1);

    if (m_dataWindowWidth > dimX)
    {
      throw CAlgorithmException(CTools::Format("'%s' algorithm can't be applied because the window width parameter (%d) is greater than the X-dimension value (%d) of the variable '%s' within the data file (%s). "
                                               "You have to decrease the window width parameter", 
                                                this->GetName().c_str(), m_dataWindowWidth, dimX, itMap->first.c_str(), m_currentFileName.c_str()),
                                this->GetName(), BRATHL_LOGIC_ERROR);
    }

    if (m_dataWindowHeight > dimY)
    {
      throw CAlgorithmException(CTools::Format("'%s' algorithm can't be applied because the window height parameter (%d) is greater than the Y-dimension value (%d) of the variable '%s' within the data file (%s). "
                                               "You have to decrease the window width parameter", 
                                                this->GetName().c_str(), m_dataWindowHeight, dimY, itMap->first.c_str(), m_currentFileName.c_str()),
                                this->GetName(), BRATHL_LOGIC_ERROR);
    }
    
    CFieldNetCdf* field = CExternalFiles::GetFieldNetCdf(m_fieldVars.Exists(fieldCaller->GetName()));
    if (field->GetDimsIndexArray() == NULL)
    {
      field->NewDimIndexArray();
    }

    if (m_field2DAsRef == NULL)
    {
      m_field2DAsRef = field;
    }


    m_addWidthRight = 0;
    m_addWidthLeft = 0;
    //int32_t minIndexX = 0;
    //m_countX = m_dataWindowWidth;
    int32_t minIndexX = indexX;
    m_countX = 1;


    int32_t minIndexY = 0;
    m_countY = m_dataWindowHeight;
    m_addHeightBottom = 0;
    m_addHeightTop = 0;

    if (indexY < halfWindowHeight) 
    {
      m_addHeightTop = (halfWindowHeight - indexY);
      m_countY -= m_addHeightTop;
    }
    else 
    {
      minIndexY = indexY - halfWindowHeight;
      if ((indexY + halfWindowHeight) >= dimY)
      {
        m_addHeightBottom = (halfWindowHeight + indexY + 1) - dimY;
        m_countY -= m_addHeightBottom;
      }
    }
    
    // Read data from Netcdf file
    field->GetDimsIndexArray()[0] = minIndexX;
    field->GetDimsCountArray()[0] = m_countX;
    
    field->GetDimsIndexArray()[1] = minIndexY;
    field->GetDimsCountArray()[1] = m_countY;
    
  }

  // Set number of data to read for 1D fields
  for (itMap = m_fieldVars.begin(); itMap != m_fieldVars.end() ; itMap++)
  { 
    CFieldNetCdf* field = CExternalFiles::GetFieldNetCdf(itMap->second);
    
    int32_t nbDims = field->GetNbDims();
    if (nbDims != 1)
    {
      continue;
    }

    if (field->GetDimsIndexArray() == NULL)
    {
      field->NewDimIndexArray();
    }

    CFieldNetCdf* fieldTemp = CExternalFiles::GetFieldNetCdf(m_fieldDependOnXDim.Exists(itMap->first), false);
    if (fieldTemp != NULL)
    {
      field->GetDimsIndexArray()[0] = m_field2DAsRef->GetDimsIndexArray()[0];
      field->GetDimsCountArray()[0] = m_field2DAsRef->GetDimsCountArray()[0];
      continue;
    }

    fieldTemp = CExternalFiles::GetFieldNetCdf(m_fieldDependOnYDim.Exists(itMap->first), false);
    if (fieldTemp != NULL)
    {
      field->GetDimsIndexArray()[0] = m_field2DAsRef->GetDimsIndexArray()[1];
      field->GetDimsCountArray()[0] = m_field2DAsRef->GetDimsCountArray()[1];
    }

  }


}



//----------------------------------------
void CBratAlgoFilterGaussian::Dump(std::ostream& fOut /*= std::cerr*/)
{
   if (CTrace::IsTrace() == false)
   { 
      return;
   }

  fOut << "==> Dump a CBratAlgoFilterGaussian Object at "<< this << std::endl;
  CBratAlgoFilterKernel::Dump(fOut);

  fOut << "m_gaussian: " << m_gaussian << std::endl;
  fOut << "m_initialWeights: " << std::endl;
  m_initialWeights.Dump();
  //fOut << "m_weights: " << std::endl;
  //m_weights.Dump();

  fOut << "==> END Dump a CBratAlgoFilterGaussian Object at "<< this << std::endl;

}


} // end namespace
