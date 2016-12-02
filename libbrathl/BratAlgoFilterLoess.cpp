
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


#include "common/tools/TraceLog.h"
#include "common/tools/Exception.h"

#include "BratAlgoFilterLoess.h" 

// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

using namespace brathl;



namespace brathl
{

//-------------------------------------------------------------
//------------------- CBratAlgoFilterLoess class --------------------
//-------------------------------------------------------------

CBratAlgoFilterLoess::CBratAlgoFilterLoess()
{
  Init();

}
//----------------------------------------
CBratAlgoFilterLoess::CBratAlgoFilterLoess(const CBratAlgoFilterLoess &copy)
{
  Init();

  Set(copy);

}

//----------------------------------------
CBratAlgoFilterLoess::~CBratAlgoFilterLoess()
{

}
//----------------------------------------
void CBratAlgoFilterLoess::Init()
{

  SetBeginOfFile();
}

//----------------------------------------
void CBratAlgoFilterLoess::Set(const CBratAlgoFilterLoess &o)
{
  CBratAlgoFilterKernel::Set(o);

  m_loess = o.m_loess;
  m_initialWeights = o.m_initialWeights;
  //m_weights = o.m_weights;

}


//----------------------------------------
CBratAlgoFilterLoess& CBratAlgoFilterLoess::operator=(const CBratAlgoFilterLoess &copy)
{
  if (this == &copy)
  {
    return *this;
  }

  Set(copy);

  return *this;
}

//----------------------------------------
void CBratAlgoFilterLoess::SetBeginOfFile()
{
  CBratAlgoFilterKernel::SetBeginOfFile();
  
}

//----------------------------------------
void CBratAlgoFilterLoess::SetEndOfFile()
{
  CBratAlgoFilterKernel::SetEndOfFile();

}
//----------------------------------------
void CBratAlgoFilterLoess::SetPreviousValues(bool fromProduct) 
{
  CBratAlgoFilterKernel::SetPreviousValues(fromProduct);

}
//----------------------------------------
void CBratAlgoFilterLoess::SetNextValues() 
{
  CBratAlgoFilterKernel::SetNextValues();
}
//----------------------------------------
void CBratAlgoFilterLoess::PrepareReturn() 
{
  int32_t iRecord = m_callerProduct->GetCurrentRecordNumber();
  m_callerProductRecordPrev = iRecord;

  if (iRecord + 1 >= m_nProductRecords)
  {
    SetEndOfFile();
  }

}

//----------------------------------------
void CBratAlgoFilterLoess::Dump(std::ostream& fOut /*= std::cerr*/)
{
   if (CTrace::IsTrace() == false)
   { 
      return;
   }

  fOut << "==> Dump a CBratAlgoFilterLoess Object at "<< this << std::endl;
  CBratAlgoFilterKernel::Dump(fOut);

  fOut << "m_initialWeights: " << std::endl;
  m_initialWeights.Dump();
  fOut << "m_loess: " << m_loess << std::endl;

  fOut << "==> END Dump a CBratAlgoFilterLoess Object at "<< this << std::endl;

}


} // end namespace
