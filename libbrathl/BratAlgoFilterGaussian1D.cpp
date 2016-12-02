
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


#include "common/tools/TraceLog.h"
#include "common/tools/Exception.h"
#include "Product.h" 

#include "BratAlgoFilterGaussian1D.h" 

// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

using namespace brathl;



namespace brathl
{

//-------------------------------------------------------------
//------------------- CBratAlgoFilterGaussian1D class --------------------
//-------------------------------------------------------------

CBratAlgoFilterGaussian1D::CBratAlgoFilterGaussian1D()
{
  Init();

}
//----------------------------------------
CBratAlgoFilterGaussian1D::CBratAlgoFilterGaussian1D(const CBratAlgoFilterGaussian1D &copy)
{
  Init();

  Set(copy);

}
//----------------------------------------
void CBratAlgoFilterGaussian1D::Init()
{
  m_expectedTypes.Insert(static_cast<int32_t>(CBratAlgorithmParam::T_DOUBLE));

  setDefaultValue(m_dataWindowLength);

  SetBeginOfFile();
}

//----------------------------------------
void CBratAlgoFilterGaussian1D::Set(const CBratAlgoFilterGaussian1D &o)
{
  CBratAlgoFilterGaussian::Set(o);

  m_dataWindowLength = o.m_dataWindowLength;
}


//----------------------------------------
CBratAlgoFilterGaussian1D& CBratAlgoFilterGaussian1D::operator=(const CBratAlgoFilterGaussian1D &copy)
{
  if (this == &copy)
  {
    return *this;
  }

  Set(copy);

  return *this;
}

//----------------------------------------
void CBratAlgoFilterGaussian1D::SetBeginOfFile()
{
  CBratAlgoFilterGaussian::SetBeginOfFile();
}

//----------------------------------------
void CBratAlgoFilterGaussian1D::SetEndOfFile()
{
  CBratAlgoFilterGaussian::SetEndOfFile();

  setDefaultValue(m_varValueNext);
}
//----------------------------------------
double CBratAlgoFilterGaussian1D::Run(CVectorBratAlgorithmParam& args)
{
  int32_t iRecord = m_callerProduct->GetCurrentRecordNumber();

  if (iRecord == m_callerProductRecordPrev)
  {
    // Do nothing: data have been already computed
    return m_gaussian;
  }

  setDefaultValue(m_gaussian);
  
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
    return m_gaussian;
  }

  if (m_dataWindowLength == 1)
  {
    m_gaussian = m_varValue;
    PrepareReturn();
    return m_varValue;
  }

  // If 'default value' and no extrapolation then returns
  if (isDefaultValue(m_varValue) && (m_extrapolate == 0))
  {
    PrepareReturn();
    return m_gaussian;
  }

  uint32_t shiftSymmetry = 1;
  
  PrepareDataValues1D(shiftSymmetry);

  ComputeGaussian();

  PrepareReturn();

  return m_gaussian;
}

//----------------------------------------
double CBratAlgoFilterGaussian1D::ComputeGaussian()
{
  setDefaultValue(m_gaussian);

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
    return m_gaussian;
  }

  //SumWeights(ModeHorizontal, 0);
  m_gaussian = ApplyFilter(ModeHorizontal, 0);
 
#if _DEBUG_BRAT_ALGO    
  CTrace::Tracer(3, CTools::Format("m_rawDataWindow contains (%d elements):", m_rawDataWindow.size()));
  std::string str;
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
  CTrace::Tracer(3, CTools::Format("gaussian is: %f", m_gaussian));
#endif

  return m_gaussian;

}

//----------------------------------------
void CBratAlgoFilterGaussian1D::SetPreviousValues(bool fromProduct) 
{
  if (fromProduct)
  {
    m_varValuePrev = this->GetDataValue(CBratAlgoFilterGaussian1D::m_VAR_PARAM_INDEX);
  }
  else
  {
    CBratAlgoFilterGaussian::SetPreviousValues(fromProduct);
    m_varValuePrev = m_varValue;
  }

}

//----------------------------------------
void CBratAlgoFilterGaussian1D::SetNextValues() 
{
  m_varValueNext = this->GetDataValue(CBratAlgoFilterGaussian1D::m_VAR_PARAM_INDEX);
  m_rawDataWindow.Insert(m_varValueNext);
}


//----------------------------------------
void CBratAlgoFilterGaussian1D::CheckVarExpression(uint32_t index)
{  
    UNUSED(index);
}

//----------------------------------------
void CBratAlgoFilterGaussian1D::Dump(std::ostream& fOut /*= std::cerr*/)
{
   if (CTrace::IsTrace() == false)
   { 
      return;
   }

  fOut << "==> Dump a CBratAlgoFilterGaussian1D Object at "<< this << std::endl;
  CBratAlgoFilterGaussian::Dump(fOut);
  fOut << "==> END Dump a CBratAlgoFilterGaussian1D Object at "<< this << std::endl;

}

} // end namespace
