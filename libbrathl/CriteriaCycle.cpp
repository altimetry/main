
/*
* Copyright (C) 2008 Collecte Localisation Satellites (CLS), France (31)
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

#include "brathl_error.h" 
#include "brathl.h" 
//#define BRAT_INTERNAL

#include "Stl.h" 

#include "TraceLog.h" 
#include "Tools.h" 
#include "Exception.h" 

#include "Criteria.h" 
#include "CriteriaCycle.h" 


using namespace brathl;



namespace brathl
{
  const string FORMAT_INT_CYCLE = "%d";

  const string CCriteriaCycle::m_delimiter = " ";  

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
CCriteriaCycle::CCriteriaCycle(const string& from, const string& to) 
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
string CCriteriaCycle::GetAsText(const string& delimiter /* = CCriteriaCycle::m_delimiter */)
{
  string format = FORMAT_INT_CYCLE;
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
  CTools::SetDefaultValue(m_from);
  CTools::SetDefaultValue(m_to);

}


//----------------------------------------
bool CCriteriaCycle::IsDefaultValue()
{
  return (CTools::IsDefaultValue(m_from) || CTools::IsDefaultValue(m_to));
}

//-------------------------------------------------------------
void CCriteriaCycle::Adjust() 
{
  if (!(CTools::IsDefaultValue(m_from)) && !(CTools::IsDefaultValue(m_to))) 
  {
    if (m_from > m_to)
    {
      int32_t tmp = m_from;
      m_from = m_to;
      m_to = tmp;
    }
  } 
  else if ((CTools::IsDefaultValue(m_from)) && !(CTools::IsDefaultValue(m_to))) 
  {
    m_from = m_to;
  } 
  else if (!(CTools::IsDefaultValue(m_from)) && (CTools::IsDefaultValue(m_to))) 
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
void CCriteriaCycle::SetTo(const string& to)
{
  m_to = CTools::StrToInt(to);

  Adjust();
}
//-------------------------------------------------------------
void CCriteriaCycle::SetFrom(int32_t from) 
{
  m_from = from;

  Adjust();
}

//-------------------------------------------------------------
void CCriteriaCycle::SetFrom(const string& from)
{
  m_from = CTools::StrToInt(from);

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
void CCriteriaCycle::Set(const string& from, const string& to)
{
  Set(CTools::StrToInt(from), CTools::StrToInt(to));

}
//-------------------------------------------------------------
void CCriteriaCycle::SetFromText(const string& values, const string& delimiter /* = CCriteriaCycle::m_delimiter */)
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
  
  if ((this->IsDefaultValue()) || (CTools::IsDefaultValue(otherFrom)) || (CTools::IsDefaultValue(otherTo))) {
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
bool CCriteriaCycle::Intersect(const string& from, const string& to, CStringArray& intersect)
{
  return Intersect(CTools::StrToInt(from), CTools::StrToInt(to), intersect);

}
//----------------------------------------
bool CCriteriaCycle::Intersect(const string& from, const string& to, CIntArray& intersect)
{
  return Intersect(CTools::StrToInt(from), CTools::StrToInt(to), intersect);

}

//----------------------------------------
void CCriteriaCycle::Dump(ostream& fOut /* = cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  CCriteriaCycle::Dump(fOut);

  fOut << "==> Dump a CCriteriaCycle Object at "<< this << endl;
  fOut << "m_from "<< m_from << endl;
  fOut << "m_to "<< m_to << endl;
  fOut << "==> END Dump a CCriteriaCycle Object at "<< this << endl;

  fOut << endl;

}

} // end namespace
