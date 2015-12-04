
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
#include "brathl.h" 


#include "new-gui/Common/tools/TraceLog.h" 
#include "new-gui/Common/tools/Exception.h" 
#include "Product.h" 

#include "BratAlgoFilterLanczos1D.h" 

// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

using namespace brathl;



namespace brathl
{

//-------------------------------------------------------------
//------------------- CBratAlgoFilterLanczos1D class --------------------
//-------------------------------------------------------------

CBratAlgoFilterLanczos1D::CBratAlgoFilterLanczos1D()
{
  Init();

}
//----------------------------------------
CBratAlgoFilterLanczos1D::CBratAlgoFilterLanczos1D(const CBratAlgoFilterLanczos1D &copy)
{
  Init();

  Set(copy);

}
//----------------------------------------
void CBratAlgoFilterLanczos1D::Init()
{
  m_expectedTypes.Insert(static_cast<int32_t>(CBratAlgorithmParam::T_DOUBLE));

  setDefaultValue(m_dataWindowLength);

  SetBeginOfFile();
}

//----------------------------------------
void CBratAlgoFilterLanczos1D::Set(const CBratAlgoFilterLanczos1D &o)
{
  CBratAlgoFilterLanczos::Set(o);

  m_dataWindowLength = o.m_dataWindowLength;
}


//----------------------------------------
CBratAlgoFilterLanczos1D& CBratAlgoFilterLanczos1D::operator=(const CBratAlgoFilterLanczos1D &copy)
{
  if (this == &copy)
  {
    return *this;
  }

  Set(copy);

  return *this;
}

//----------------------------------------
void CBratAlgoFilterLanczos1D::SetBeginOfFile()
{
  CBratAlgoFilterLanczos::SetBeginOfFile();
}

//----------------------------------------
void CBratAlgoFilterLanczos1D::SetEndOfFile()
{
  CBratAlgoFilterLanczos::SetEndOfFile();

  setDefaultValue(m_varValueNext);
}
//----------------------------------------
double CBratAlgoFilterLanczos1D::Run(CVectorBratAlgorithmParam& args)
{
  int32_t iRecord = m_callerProduct->GetCurrentRecordNumber();

  if (iRecord == m_callerProductRecordPrev)
  {
    // Do nothing: data have been already computed
    return m_lanczos;
  }

  setDefaultValue(m_lanczos);
  
  OpenProductFile();

  // Gets the next record regarding to the current product record.
  // and save previous values.
  //this->GetNextData();

  // Get current parameter values (included expression value)
  SetParamValues(args);

  //// Set only the first value, others values will be stored in the 'SetNextValues'
  //if (m_rawDataWindow.size() <= 0)
  //{
  //  m_rawDataWindow.Insert(m_varValue);
  //}


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
    return m_lanczos;
  }

  if (m_dataWindowLength == 1)
  {
    m_lanczos = m_varValue;
    PrepareReturn();
    return m_varValue;
  }

  // If 'default value' and no extrapolation then returns
  if (isDefaultValue(m_varValue) && (m_extrapolate == 0))
  {
    PrepareReturn();
    return m_lanczos;
  }

  uint32_t shiftSymmetry = 1;

  PrepareDataValues1D(shiftSymmetry);
  
  ComputeLanczos();

  PrepareReturn();

  return m_lanczos;
}

//----------------------------------------
double CBratAlgoFilterLanczos1D::ComputeLanczos()
{
  setDefaultValue(m_lanczos);

  size_t nbValues = m_rawDataWindow.size();

  size_t nbValidPoints = nbValues;
  CDoubleArray::const_iterator it;
  for (it = m_rawDataWindow.begin(); it != m_rawDataWindow.end(); it++)
  {
    if (isDefaultValue(*it))
    {
      nbValidPoints--;    
    }
  }

  if (nbValidPoints < m_validPts)
  {
    return m_lanczos;
  }

  //SumWeights(ModeHorizontal, 0);
  m_lanczos = ApplyFilter(ModeHorizontal, 0);
 
#if _DEBUG_BRAT_ALGO    
  CTrace::Tracer(3, "===> WINDOW IS :");

  std::string str;
  for (uint32_t i = 0 ; i < m_dataWindowLength ; i++)
  {
    double value = m_rawDataWindow[i];
    if (isDefaultValue(value))
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

  CTrace::Tracer(3, CTools::Format("lanczos is: %f", m_lanczos));
#endif

  return m_lanczos;

}
//----------------------------------------
void CBratAlgoFilterLanczos1D::SetPreviousValues(bool fromProduct) 
{
  if (fromProduct)
  {
    m_varValuePrev = this->GetDataValue(CBratAlgoFilterLanczos1D::m_VAR_PARAM_INDEX);
  }
  else
  {
    CBratAlgoFilterLanczos::SetPreviousValues(fromProduct);
    m_varValuePrev = m_varValue;
  }

}

//----------------------------------------
void CBratAlgoFilterLanczos1D::SetNextValues() 
{
  m_varValueNext = this->GetDataValue(CBratAlgoFilterLanczos1D::m_VAR_PARAM_INDEX);
  m_rawDataWindow.Insert(m_varValueNext);
}


//----------------------------------------
void CBratAlgoFilterLanczos1D::CheckVarExpression(uint32_t index)
{  
    UNUSED(index);
}

//----------------------------------------
void CBratAlgoFilterLanczos1D::Dump(std::ostream& fOut /*= std::cerr*/)
{
   if (CTrace::IsTrace() == false)
   { 
      return;
   }

  fOut << "==> Dump a CBratAlgoFilterLanczos1D Object at "<< this << std::endl;
  CBratAlgoFilterLanczos::Dump(fOut);
  fOut << "==> END Dump a CBratAlgoFilterLanczos1D Object at "<< this << std::endl;

}

} // end namespace
