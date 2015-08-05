
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

#include "brathl_error.h" 
#include "brathl.h" 
//#define BRAT_INTERNAL

#include "Stl.h" 

#include "TraceLog.h" 
#include "Tools.h" 
#include "Exception.h" 

#include "Criteria.h" 
#include "CriteriaDatetime.h" 


using namespace brathl;



namespace brathl
{



//-------------------------------------------------------------
//------------------- CCriteriaDatetime class --------------------
//-------------------------------------------------------------




CCriteriaDatetime::CCriteriaDatetime() 
{
  Init();
}
//-------------------------------------------------------------

CCriteriaDatetime::CCriteriaDatetime(CCriteriaDatetime& c) 
{
  Init();
  Set(c);
}
//-------------------------------------------------------------

CCriteriaDatetime::CCriteriaDatetime(CCriteriaDatetime* c) 
{
  Init();
  Set(*c);
}

//-------------------------------------------------------------
CCriteriaDatetime::CCriteriaDatetime(CDatePeriod& datePeriod) 
{
  Init();
  Set(datePeriod);
}
//-------------------------------------------------------------
CCriteriaDatetime::CCriteriaDatetime(CDate& from, CDate& to) 
{
  Init();
  Set(from, to);
}
//-------------------------------------------------------------
CCriteriaDatetime::CCriteriaDatetime(const string& from, const string& to) 
{
  Init();
  Set(from, to);
}

//-------------------------------------------------------------
CCriteriaDatetime::CCriteriaDatetime(double from, double to) 
{
  Init();
  Set(from, to);
}

//-------------------------------------------------------------
CCriteriaDatetime::CCriteriaDatetime(const CStringArray& array) 
{
  Init();
  Set(array);

}


//----------------------------------------

CCriteriaDatetime::~CCriteriaDatetime()
{

}

//----------------------------------------

void CCriteriaDatetime::Init()
{
  m_key = CCriteria::DATETIME;
  
  SetDefaultValue();
}
//-------------------------------------------------------------
const CCriteriaDatetime& CCriteriaDatetime::operator=(CCriteriaDatetime& c)
{

  Set(c);

  return *this;
}

//-------------------------------------------------------------
void CCriteriaDatetime::Set(CCriteriaDatetime& c) 
{
  Set(c.m_datePeriod);

}

//----------------------------------------
string CCriteriaDatetime::GetAsText(const string& delimiter /* = CDatePeriod::m_delimiter */)
{
  return m_datePeriod.GetAsText(delimiter);
}

//----------------------------------------
CCriteriaDatetime* CCriteriaDatetime::GetCriteria(CBratObject* ob, bool withExcept /*= true*/)
{
  CCriteriaDatetime* criteria = dynamic_cast<CCriteriaDatetime*>(ob);
  if (withExcept)
  {
    if (criteria == NULL)
    {
      CException e("CCriteriaDatetime::GetCriteria - dynamic_cast<CCriteriaDatetime*>(ob) returns NULL"
                   "object seems not to be an instance of CCriteriaDatetime",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }
  
  return criteria;

}


//----------------------------------------
void CCriteriaDatetime::SetDefaultValue()
{
  m_datePeriod.SetDefaultValue();
}

//----------------------------------------
bool CCriteriaDatetime::IsDefaultValue()
{
  return m_datePeriod.IsDefaultValue();
}


//-------------------------------------------------------------
void CCriteriaDatetime::SetTo(CDate& to) 
{
  m_datePeriod.SetTo(to);
}
//-------------------------------------------------------------
void CCriteriaDatetime::SetTo(const string& strDate)
{
  m_datePeriod.SetTo(strDate);
}
//-------------------------------------------------------------
void CCriteriaDatetime::SetFrom(CDate& from) 
{
  m_datePeriod.SetFrom(from);
}

//-------------------------------------------------------------
void CCriteriaDatetime::SetFrom(const string& strDate)
{
  m_datePeriod.SetFrom(strDate);
}

//-------------------------------------------------------------
void CCriteriaDatetime::Set(CDate& from, CDate& to)
{
  m_datePeriod.Set(from, to);

}
//-------------------------------------------------------------
void CCriteriaDatetime::Set(CDatePeriod& datePeriod)
{
  m_datePeriod.Set(datePeriod);

}
//-------------------------------------------------------------
void CCriteriaDatetime::Set(double from, double to)
{
  m_datePeriod.Set(from, to);

}
//-------------------------------------------------------------
void CCriteriaDatetime::Set(const string& from, const string& to)
{
  m_datePeriod.Set(from, to);

}
//-------------------------------------------------------------
void CCriteriaDatetime::SetFromText(const string& values, const string& delimiter /* = CDatePeriod::m_delimiter */)
{
  CStringArray array;
  array.ExtractStrings(values, delimiter);

  m_datePeriod.Set(array);

}
//-------------------------------------------------------------

void CCriteriaDatetime::Set(const CStringArray& array) 
{
  m_datePeriod.Set(array);
}


//----------------------------------------
bool CCriteriaDatetime::Intersect(double otherFrom, double otherTo, CDatePeriod& intersect)
{
  CDatePeriod datePeriod(otherFrom, otherTo);
  
  return m_datePeriod.Intersect(datePeriod, intersect);
}
//----------------------------------------
bool CCriteriaDatetime::Intersect(CDatePeriod& datePeriod, CDatePeriod& intersect)
{
  
  return m_datePeriod.Intersect(datePeriod, intersect);
}

//----------------------------------------
void CCriteriaDatetime::Dump(ostream& fOut /* = cerr */)
{

  if (CTrace::IsTrace() == false)
  {
    return;
  }

  CCriteria::Dump(fOut);

  fOut << "==> Dump a CCriteriaDatetime Object at "<< this << endl;
  m_datePeriod.Dump(fOut);
  fOut << "==> END Dump a CCriteriaDatetime Object at "<< this << endl;

  fOut << endl;

}

} // end namespace
