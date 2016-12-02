
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

#include <cstdlib>
#include <cstdio>
#include <cstring> 

#include "common/tools/brathl_error.h"
#include "brathl.h" 
//#define BRAT_INTERNAL

#include <string> 

#include "common/tools/TraceLog.h"
#include "Tools.h" 
#include "common/tools/Exception.h"

#include "Criteria.h" 
#include "CriteriaCycle.h" 


using namespace brathl;



namespace brathl
{
  const std::string FORMAT_INT_CYCLE = "%d";

  const std::string CCriteriaCycle::m_delimiter = " ";  

//-------------------------------------------------------------
//------------------- CCriteriaCycle class --------------------
//-------------------------------------------------------------


CCriteriaCycle::CCriteriaCycle() 
{
  Init();
}

//-------------------------------------------------------------
CCriteriaCycle::CCriteriaCycle(CCriteriaCycle& c) 
{
  Init();
  Set(c);
}
//-------------------------------------------------------------
CCriteriaCycle::CCriteriaCycle(CCriteriaCycle* c) 
{
  Init();
  Set(*c);
}

//-------------------------------------------------------------
CCriteriaCycle::CCriteriaCycle(int32_t from, int32_t to) 
{
  Init();
  Set(from, to);
}
//-------------------------------------------------------------
CCriteriaCycle::CCriteriaCycle(const std::string& from, const std::string& to) 
{
  Init();
  Set(from, to);
}
//-------------------------------------------------------------
CCriteriaCycle::CCriteriaCycle(const CStringArray& array) 
{
  Init();
  Set(array);

}
//----------------------------------------
CCriteriaCycle::~CCriteriaCycle()
{

}
//-------------------------------------------------------------
void CCriteriaCycle::Init()
{
  m_key = CCriteria::CYCLE;

  SetDefaultValue();
}
//----------------------------------------
const CCriteriaCycle& CCriteriaCycle::operator=(CCriteriaCycle& c)
{

  Set(c);

  return *this;
}

//----------------------------------------
std::string CCriteriaCycle::GetAsText(const std::string& delimiter /* = CCriteriaCycle::m_delimiter */)
{
  std::string format = FORMAT_INT_CYCLE;
  format.append(delimiter);
  format.append(FORMAT_INT_CYCLE);

  return CTools::Format(format.c_str(), m_from, m_to);

}

//----------------------------------------
CCriteriaCycle* CCriteriaCycle::GetCriteria(CBratObject* ob, bool withExcept /*= true*/)
{
  CCriteriaCycle* criteria = dynamic_cast<CCriteriaCycle*>(ob);
  if (withExcept)
  {
    if (criteria == NULL)
    {
      CException e("CCriteriaCycle::GetCriteria - dynamic_cast<CCriteriaCycle*>(ob) returns NULL"
                   "object seems not to be an instance of CCriteriaCycle",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }
  
  return criteria;

}
//----------------------------------------
void CCriteriaCycle::SetDefaultValue()
{
  setDefaultValue(m_from);
  setDefaultValue(m_to);

}


//----------------------------------------
bool CCriteriaCycle::IsDefaultValue()
{
  return (isDefaultValue(m_from) || isDefaultValue(m_to));
}

//-------------------------------------------------------------
void CCriteriaCycle::Adjust() 
{
  if (!(isDefaultValue(m_from)) && !(isDefaultValue(m_to))) 
  {
    if (m_from > m_to)
    {
      int32_t tmp = m_from;
      m_from = m_to;
      m_to = tmp;
    }
  } 
  else if ((isDefaultValue(m_from)) && !(isDefaultValue(m_to))) 
  {
    m_from = m_to;
  } 
  else if (!(isDefaultValue(m_from)) && (isDefaultValue(m_to))) 
  {
    m_to = m_from;
  }

}
//-------------------------------------------------------------
void CCriteriaCycle::SetTo(int32_t to) 
{
  m_to = to;

  Adjust();

}
//-------------------------------------------------------------
void CCriteriaCycle::SetTo(const std::string& to)
{
  m_to = CTools::StrToInt32(to);

  Adjust();
}
//-------------------------------------------------------------
void CCriteriaCycle::SetFrom(int32_t from) 
{
  m_from = from;

  Adjust();
}

//-------------------------------------------------------------
void CCriteriaCycle::SetFrom(const std::string& from)
{
  m_from = CTools::StrToInt32(from);

  Adjust();
}
//-------------------------------------------------------------
void CCriteriaCycle::Set(CCriteriaCycle& c) 
{
  Set(c.m_from, c.m_to);

}

//-------------------------------------------------------------
void CCriteriaCycle::Set(int32_t from, int32_t to)
{
  SetFrom(from);
  SetTo(to);

}
//-------------------------------------------------------------
void CCriteriaCycle::Set(const std::string& from, const std::string& to)
{
  Set(CTools::StrToInt32(from), CTools::StrToInt32(to));

}
//-------------------------------------------------------------
void CCriteriaCycle::SetFromText(const std::string& values, const std::string& delimiter /* = CCriteriaCycle::m_delimiter */)
{
  CStringArray array;
  array.ExtractStrings(values, delimiter);

  Set(array);

}

//-------------------------------------------------------------

void CCriteriaCycle::Set(const CStringArray& array) 
{
  switch (array.size()) 
  {
    case 2: 
    {
      Set(array.at(0), array.at(1)); 
      break;
    }
    case 1: 
    {
      Set(array.at(0), array.at(0)); 
      break;
    }
    default:
    {
      break;
    }

  }
}
//----------------------------------------
bool CCriteriaCycle::Intersect(CStringArray& array, CStringArray& intersect)
{

  CIntArray arrayIn;
  arrayIn.Insert(array);
  CIntArray arrayTmp;
  bool intersectFlag = Intersect(arrayIn, arrayTmp);
  if (intersectFlag) 
  {
    intersect.Insert(array);
  }

  return intersectFlag;

}
//----------------------------------------
bool CCriteriaCycle::Intersect(CStringArray& array, CIntArray& intersect)
{

  CIntArray arrayIn;
  arrayIn.Insert(array);
  bool intersectFlag = Intersect(arrayIn, intersect);
  if (intersectFlag) 
  {
    intersect.Insert(array);
  }

  return intersectFlag;

}
//----------------------------------------
bool CCriteriaCycle::Intersect(CIntArray& array, CStringArray& intersect)
{

  CIntArray arrayTmp;
  bool intersectFlag = Intersect(array, arrayTmp);
  if (intersectFlag) 
  {
    intersect.Insert(array);
  }

  return intersectFlag;
  
}
//----------------------------------------
bool CCriteriaCycle::Intersect(CIntArray& array, CIntArray& intersect)
{
  bool intersectFlag = false;

  switch (array.size()) 
  {
    case 2: 
    {
      intersectFlag = Intersect(array.at(0), array.at(1), intersect); 
      break;
    }
    case 1: 
    {
      intersectFlag = Intersect(array.at(0), array.at(0), intersect); 
      break;
    }
    default:
    {
      break;
    }

  }

  return intersectFlag;
}

//----------------------------------------
bool CCriteriaCycle::Intersect(int32_t otherFrom, int32_t otherTo, CStringArray& intersect)
{

  CIntArray array;
  bool intersectFlag = Intersect(otherFrom, otherTo, array);
  if (intersectFlag) 
  {
    intersect.Insert(array);
  }

  return intersectFlag;
}

//----------------------------------------
bool CCriteriaCycle::Intersect(int32_t otherFrom, int32_t otherTo, CIntArray& intersect)
{
  
  if ((this->IsDefaultValue()) || (isDefaultValue(otherFrom)) || (isDefaultValue(otherTo))) {
      return false;
  }

  //  this    !-------------!
  //  other                    !-----!
  // OR
  //  this                 !-------------!
  //  other       !-----!
  if ((m_to < otherFrom) || (otherTo < m_from))
  {
    return false;
  }


  //  this    !------------......
  //  other          !-----......
  if (m_from <= otherFrom)  
  {
    intersect.Insert(otherFrom);
  }
  //  this          !---------......
  //  other     !-------------......
  else
  {
    intersect.Insert(m_from);
  }

  //  this    ....--------!
  //  other   ....-------------!
  if (m_to <= otherTo)  
  {
    intersect.Insert(m_to);
  }
  //  this    ....-------------!
  //  other   ....--------!
  else
  {
    intersect.Insert(otherTo);
  }

  return true;
}

//----------------------------------------
bool CCriteriaCycle::Intersect(double otherFrom, double otherTo, CIntArray& intersect)
{
  return Intersect(static_cast<int32_t>(otherFrom), static_cast<int32_t>(otherTo), intersect);
}

//----------------------------------------
bool CCriteriaCycle::Intersect(const std::string& from, const std::string& to, CStringArray& intersect)
{
  return Intersect(CTools::StrToInt32(from), CTools::StrToInt32(to), intersect);

}
//----------------------------------------
bool CCriteriaCycle::Intersect(const std::string& from, const std::string& to, CIntArray& intersect)
{
  return Intersect(CTools::StrToInt32(from), CTools::StrToInt32(to), intersect);

}

//----------------------------------------
void CCriteriaCycle::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  CCriteriaCycle::Dump(fOut);

  fOut << "==> Dump a CCriteriaCycle Object at "<< this << std::endl;
  fOut << "m_from "<< m_from << std::endl;
  fOut << "m_to "<< m_to << std::endl;
  fOut << "==> END Dump a CCriteriaCycle Object at "<< this << std::endl;

  fOut << std::endl;

}

} // end namespace
