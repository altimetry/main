
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

#include "brathl.h" 


#include "new-gui/Common/tools/TraceLog.h" 
#include "new-gui/Common/tools/Exception.h" 
#include "Product.h" 

#include "BratAlgoFilterMedian1D.h" 

// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

using namespace brathl;



namespace brathl
{

const uint32_t CBratAlgoFilterMedian1D::m_INPUT_PARAMS = 4;

const uint32_t CBratAlgoFilterMedian1D::m_WINDOW_PARAM_INDEX = 1;
const uint32_t CBratAlgoFilterMedian1D::m_VALID_PARAM_INDEX = 2;
const uint32_t CBratAlgoFilterMedian1D::m_EXTRAPOLATE_PARAM_INDEX = 3;

//-------------------------------------------------------------
//------------------- CBratAlgoFilterMedian1D class --------------------
//-------------------------------------------------------------

CBratAlgoFilterMedian1D::CBratAlgoFilterMedian1D()
{
  Init();

}
//----------------------------------------
CBratAlgoFilterMedian1D::CBratAlgoFilterMedian1D(const CBratAlgoFilterMedian1D &copy)
{
  Init();

  Set(copy);

}
//----------------------------------------
void CBratAlgoFilterMedian1D::Init()
{
  SetBeginOfFile();
}

//----------------------------------------
void CBratAlgoFilterMedian1D::Set(const CBratAlgoFilterMedian1D &o)
{
  CBratAlgoFilterMedian::Set(o);
}


//----------------------------------------
CBratAlgoFilterMedian1D& CBratAlgoFilterMedian1D::operator=(const CBratAlgoFilterMedian1D &copy)
{
  if (this == &copy)
  {
    return *this;
  }

  Set(copy);

  return *this;
}

//----------------------------------------
void CBratAlgoFilterMedian1D::SetBeginOfFile()
{
  CBratAlgoFilterMedian::SetBeginOfFile();
  }

//----------------------------------------
void CBratAlgoFilterMedian1D::SetEndOfFile()
{
  CBratAlgoFilterMedian::SetEndOfFile();
}
//----------------------------------------
double CBratAlgoFilterMedian1D::Run(CVectorBratAlgorithmParam& args)
{
  int32_t iRecord = m_callerProduct->GetCurrentRecordNumber();

  if (iRecord == m_callerProductRecordPrev)
  {
    // Do nothing: data have been already computed
    return m_median;
  }

  setDefaultValue(m_median);
  
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
    return m_median;
  }

  if (m_dataWindowLength == 1)
  {
    m_median = m_varValue;
    PrepareReturn();
    return m_median;
  }

  // If 'default value' and no extrapolation then returns
  if (isDefaultValue(m_varValue) && (m_extrapolate == 0))
  {
    PrepareReturn();
    return m_median;
  }

  uint32_t shiftSymmetry = 0;

  PrepareDataValues1D(shiftSymmetry);

  ComputeMedian();

  PrepareReturn();

  return m_median;
}
//----------------------------------------
void CBratAlgoFilterMedian1D::SetPreviousValues(bool fromProduct) 
{
  if (fromProduct)
  {
    m_varValuePrev = this->GetDataValue(CBratAlgoFilterMedian1D::m_VAR_PARAM_INDEX);
  }
  else
  {
    CBratAlgoFilterMedian::SetPreviousValues(fromProduct);
    m_varValuePrev = m_varValue;
  }

}

//----------------------------------------
void CBratAlgoFilterMedian1D::SetNextValues() 
{
  m_varValueNext = this->GetDataValue(CBratAlgoFilterMedian1D::m_VAR_PARAM_INDEX);
  m_rawDataWindow.Insert(m_varValueNext);
}



//----------------------------------------
void CBratAlgoFilterMedian1D::CheckInputParams(CVectorBratAlgorithmParam& args) 
{
  CBratAlgorithmBase::CheckInputParams(args);

  for (uint32_t indexParam = 0 ; indexParam < CBratAlgoFilterMedian1D::m_INPUT_PARAMS ; indexParam++)
  {
    CheckInputTypeParams(indexParam, this->GetInputParamFormat(indexParam), args);
  }

}
//----------------------------------------
void CBratAlgoFilterMedian1D::SetParamValues(CVectorBratAlgorithmParam& args)
{    
  CheckInputParams(args);

  // Set variable
  m_varValue = args.at(CBratAlgoFilterMedian1D::m_VAR_PARAM_INDEX).GetValueAsDouble();
  //// Set only the first value, others values will be stored in the 'SetNextValues'
  //if (m_rawDataWindow.size() <= 0)
  //{
  //  m_rawDataWindow.Insert(m_varValue);
  //}

  int32_t valueInt32;

  // Set data window size (once)
  if (isDefaultValue(m_dataWindowLength))
  {
    this->CheckConstantParam(CBratAlgoFilterMedian1D::m_WINDOW_PARAM_INDEX);
    
    valueInt32 = args.at(CBratAlgoFilterMedian1D::m_WINDOW_PARAM_INDEX).GetValueAsInt();
    if (valueInt32 <= 0)
    {
      throw CAlgorithmException(CTools::Format("'%s' algorithm can't be applied because the window size parameter (%d) is not greater than 0. " 
                                               "You have to adjust the '%s' parameter.", 
                                                this->GetName().c_str(), valueInt32, 
                                                this->GetParamName(CBratAlgoFilterMedian1D::m_WINDOW_PARAM_INDEX).c_str()),
                                this->GetName(), BRATHL_LOGIC_ERROR);
    }

    m_dataWindowLength = valueInt32;
    if (isDefaultValue(valueInt32))
    {
      m_dataWindowLength = 9;
    }
  }

  // Set valid points limit (once)
  if (isDefaultValue(m_validPts))
  {
    this->CheckConstantParam(CBratAlgoFilterMedian1D::m_VALID_PARAM_INDEX);

    valueInt32 = args.at(CBratAlgoFilterMedian1D::m_VALID_PARAM_INDEX).GetValueAsInt();
    if (valueInt32 <= 0)
    {
      throw CAlgorithmException(CTools::Format("'%s' algorithm can't be applied because the '%s' parameter (%d) is less than or equal to 0. " 
                                               "You have to adjust the '%s'  parameter.", 
                                                this->GetName().c_str(), 
                                                this->GetParamName(CBratAlgoFilterMedian1D::m_VALID_PARAM_INDEX).c_str(),
                                                valueInt32, 
                                                this->GetParamName(CBratAlgoFilterMedian1D::m_VALID_PARAM_INDEX).c_str()),
                                this->GetName(), BRATHL_LOGIC_ERROR);
    }

    m_validPts = valueInt32;
    if (isDefaultValue(valueInt32))
    {
      m_validPts = GetDataWindowSize()/2;
    }

    if (GetDataWindowSize() < m_validPts)
    {
      throw CAlgorithmException(CTools::Format("'%s' algorithm can't be applied because the window size parameter (%d) is less than the minimum number of valid points parameter (%d). "
                                               "You have to adjust either the '%s' parameter or the '%s' parameter.", 
                                                this->GetName().c_str(), GetDataWindowSize(), m_validPts,
                                                this->GetParamName(CBratAlgoFilterMedian1D::m_WINDOW_PARAM_INDEX).c_str(),
                                                this->GetParamName(CBratAlgoFilterMedian1D::m_VALID_PARAM_INDEX).c_str()),
                                this->GetName(), BRATHL_LOGIC_ERROR);
    }


  }

  // Set extrapolate flag (once)
  if (isDefaultValue(m_extrapolate))
  {
    SetParamValueExtrapolate(args, CBratAlgoFilterMedian1D::m_EXTRAPOLATE_PARAM_INDEX);
  }

}
//----------------------------------------
void CBratAlgoFilterMedian1D::CheckVarExpression(uint32_t index)
{  

}

//----------------------------------------
void CBratAlgoFilterMedian1D::Dump(std::ostream& fOut /*= std::cerr*/)
{
   if (CTrace::IsTrace() == false)
   { 
      return;
   }

  fOut << "==> Dump a CBratAlgoFilterMedian1D Object at "<< this << std::endl;
  CBratAlgoFilterMedian::Dump(fOut);
  fOut << "==> END Dump a CBratAlgoFilterMedian1D Object at "<< this << std::endl;

}

} // end namespace
