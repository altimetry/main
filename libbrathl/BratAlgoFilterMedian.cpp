
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


#include "TraceLog.h" 
#include "Exception.h" 

#include "BratAlgoFilterMedian.h" 

// When debugging changes all calls to “new” to be calls to “DEBUG_NEW” allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

using namespace brathl;



namespace brathl
{

//-------------------------------------------------------------
//------------------- CBratAlgoFilterMedian class --------------------
//-------------------------------------------------------------

CBratAlgoFilterMedian::CBratAlgoFilterMedian()
{
  Init();

}
//----------------------------------------
CBratAlgoFilterMedian::CBratAlgoFilterMedian(const CBratAlgoFilterMedian &copy)
{
  Init();

  Set(copy);

}

//----------------------------------------
CBratAlgoFilterMedian::~CBratAlgoFilterMedian()
{

}
//----------------------------------------
void CBratAlgoFilterMedian::Init()
{

  SetBeginOfFile();
}

//----------------------------------------
void CBratAlgoFilterMedian::Set(const CBratAlgoFilterMedian &o)
{
  CBratAlgoFilterKernel::Set(o);
  
  m_median = o.m_median;
}


//----------------------------------------
CBratAlgoFilterMedian& CBratAlgoFilterMedian::operator=(const CBratAlgoFilterMedian &copy)
{
  if (this == &copy)
  {
    return *this;
  }

  Set(copy);

  return *this;
}

//----------------------------------------
void CBratAlgoFilterMedian::SetBeginOfFile()
{
  CBratAlgoFilterKernel::SetBeginOfFile();
  
}

//----------------------------------------
void CBratAlgoFilterMedian::SetEndOfFile()
{
  CBratAlgoFilterKernel::SetEndOfFile();

}
//----------------------------------------
void CBratAlgoFilterMedian::SetPreviousValues(bool fromProduct) 
{
  CBratAlgoFilterKernel::SetPreviousValues(fromProduct);

}
//----------------------------------------
void CBratAlgoFilterMedian::SetNextValues() 
{
  CBratAlgoFilterKernel::SetNextValues();
}
//----------------------------------------
void CBratAlgoFilterMedian::PrepareReturn() 
{
  int32_t iRecord = m_callerProduct->GetCurrentRecordNumber();
  m_callerProductRecordPrev = iRecord;

  if (iRecord + 1 >= m_nProductRecords)
  {
    SetEndOfFile();
  }

}
//----------------------------------------
double CBratAlgoFilterMedian::ComputeMedian()
{
  CTools::SetDefaultValue(m_median);

  CDoubleArray::const_iterator it;
  m_dataWindow.RemoveAll();

  for (it = m_rawDataWindow.begin(); it != m_rawDataWindow.end(); it++)
  {
    double value = *it;
    if (!CTools::IsDefaultValue(value))
    {
      m_dataWindow.Insert(value);    
    }
  }

  if ((m_dataWindow.size() < m_validPts) || (m_dataWindow.size() <= 0))
  {
    return m_median;
  }

  // using default comparison (operator <):
  nth_element(m_dataWindow.begin(), m_dataWindow.begin() + m_dataWindow.size()/2, m_dataWindow.end());

  uint32_t middle = m_dataWindow.size() / 2;

  m_median = *(m_dataWindow.begin() + middle);

  if (CTools::IsEven(GetDataWindowSize()))
  {
    m_median = (*(m_dataWindow.begin() + (middle - 1)) + *(m_dataWindow.begin() + (middle))) / 2.0;
  }

  //Dump(*CTrace::GetDumpContext());
  
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

  CTrace::Tracer(3, CTools::Format("m_dataWindow contains (%d elements):", m_dataWindow.size()));
  str.clear();
  for (it = m_dataWindow.begin(); it != m_dataWindow.end(); it++)
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

  CTrace::Tracer(3, CTools::Format("median is: %f", m_median));
#endif

  return m_median;

}



//----------------------------------------
void CBratAlgoFilterMedian::Dump(std::ostream& fOut /*= std::cerr*/)
{
   if (CTrace::IsTrace() == false)
   { 
      return;
   }

  fOut << "==> Dump a CBratAlgoFilterMedian Object at "<< this << std::endl;
  CBratAlgoFilterKernel::Dump(fOut);

  fOut << "m_median: " << m_median << std::endl;

  fOut << "==> END Dump a CBratAlgoFilterMedian Object at "<< this << std::endl;

}


} // end namespace
