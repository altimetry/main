
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

#include <string> 

#include "TraceLog.h" 
#include "Tools.h" 
#include "Exception.h" 

#include "DatePeriod.h" 


using namespace brathl;



namespace brathl
{


const std::string CDatePeriod::m_delimiter = "/";

//-------------------------------------------------------------
//------------------- CDatePeriod class --------------------
//-------------------------------------------------------------




CDatePeriod::CDatePeriod() 
{
  Init();
}
//-------------------------------------------------------------
CDatePeriod::CDatePeriod(CDatePeriod& datePeriod) 
{
  Init();
  Set(datePeriod);
}

//-------------------------------------------------------------
CDatePeriod::CDatePeriod(CDate& from, CDate& to) 
{
  Init();
  SetTo(to);
  SetFrom(from);
}
//-------------------------------------------------------------
CDatePeriod::CDatePeriod(const std::string& from, const std::string& to) 
{
  Init();
  Set(from, to);
}
//-------------------------------------------------------------

CDatePeriod::CDatePeriod(double from, double to)
{
  Init();
  Set(from, to);

}

//-------------------------------------------------------------
CDatePeriod::CDatePeriod(const CStringArray& array) 
{
  Init();
  Set(array);

}

//----------------------------------------
CDatePeriod::~CDatePeriod()
{

}

//----------------------------------------
const CDatePeriod& CDatePeriod::operator =(CDatePeriod& datePeriod)
{
  Set(datePeriod);    
  return *this;    
}

//----------------------------------------
void CDatePeriod::Init()
{
  m_withMuSecond = false;

  SetDefaultValue();
}


//----------------------------------------
void CDatePeriod::SetDefaultValue()
{
  m_from.SetDefaultValue();
  m_to.SetDefaultValue();
}


//-------------------------------------------------------------
void CDatePeriod::Set(double from, double to)
{
  CDate begin(from);
  CDate end(to);

  Set(begin, end);

}

//-------------------------------------------------------------
void CDatePeriod::Set(CDatePeriod& datePeriod) 
{
  SetTo(datePeriod.GetTo());
  SetFrom(datePeriod.GetFrom());
}
//-------------------------------------------------------------
void CDatePeriod::SetTo(CDate& to) 
{
  m_to = to;
  Adjust();
}
//-------------------------------------------------------------
void CDatePeriod::SetTo(const std::string& strDate)
{
  int32_t result = m_to.SetDate(strDate.c_str());
  if (result != BRATHL_SUCCESS)
  {
    throw CException(CTools::Format("ERROR in CDatePeriod::SetTo - Invalid date value : %s",
					     strDate.c_str()),
			      BRATHL_SYNTAX_ERROR);
  }

  Adjust();
}
//-------------------------------------------------------------
void CDatePeriod::SetFrom(CDate& from) 
{
  m_from = from;
  Adjust();
}

//-------------------------------------------------------------
void CDatePeriod::SetFrom(const std::string& strDate)
{
  int32_t result = m_from.SetDate(strDate.c_str());
  if (result != BRATHL_SUCCESS)
  {
    throw CException(CTools::Format("ERROR in CDatePeriod::SetFrom - Invalid date value : %s",
					     strDate.c_str()),
			      BRATHL_SYNTAX_ERROR);
  }

  Adjust();
}

//-------------------------------------------------------------
void CDatePeriod::Set(const std::string& from, const std::string& to)
{
  SetFrom(from);
  SetTo(to);

}
//-------------------------------------------------------------
void CDatePeriod::Set(CDate& from, CDate& to)
{
  SetFrom(from);
  SetTo(to);

}
//-------------------------------------------------------------

void CDatePeriod::Set(const CStringArray& array) 
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

//-------------------------------------------------------------
void CDatePeriod::Adjust() 
{
  if (!(m_from.IsDefaultValue()) && !(m_to.IsDefaultValue())) 
  {
    if (m_from > m_to)
    {
      CDate tmp = m_from;
      m_from = m_to;
      m_to = tmp;
    }
  } 
  else if ((m_from.IsDefaultValue()) && !(m_to.IsDefaultValue())) 
  {
    m_from = m_to;
  } 
  else if (!(m_from.IsDefaultValue()) && (m_to.IsDefaultValue())) 
  {
    m_to = m_from;
  }

}
//----------------------------------------
bool CDatePeriod::IsDefaultValue()
{
  return ((m_from.IsDefaultValue()) ||
          (m_to.IsDefaultValue()));

}



//----------------------------------------
bool CDatePeriod::Intersect(CDatePeriod& datePeriod, CDatePeriod& intersect)
{
  return Intersect(datePeriod.GetFrom(), datePeriod.GetTo(), intersect);
}
//----------------------------------------
bool CDatePeriod::Intersect(CDate& otherFrom, CDate& otherTo, CDatePeriod& intersect)
{
  if ((this->IsDefaultValue()) || (otherFrom.IsDefaultValue()) || (otherTo.IsDefaultValue())) {
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
    intersect.SetFrom(otherFrom);
  }
  //  this          !---------......
  //  other     !-------------......
  else
  {
    intersect.SetFrom(m_from);
  }

  //  this    ....--------!
  //  other   ....-------------!
  if (m_to <= otherTo)  
  {
    intersect.SetTo(m_to);
  }
  //  this    ....-------------!
  //  other   ....--------!
  else
  {
    intersect.SetTo(otherTo);
  }

  return true;


}
//----------------------------------------
bool CDatePeriod::Union(CDatePeriod& datePeriod)
{
  CDatePeriod unionDate;
  
  bool result = Union(datePeriod, unionDate);
  
  Set(unionDate); 
  
  return result;
}
//----------------------------------------
bool CDatePeriod::Union(CDate& otherFrom, CDate& otherTo)
{
  CDatePeriod unionDate;
  
  bool result = Union(otherFrom, otherTo, unionDate);
  
  Set(unionDate); 
  
  return result;

}
//----------------------------------------
bool CDatePeriod::Union(CDatePeriod& datePeriod, CDatePeriod& unionDate)
{
  return Union(datePeriod.GetFrom(), datePeriod.GetTo(), unionDate);
}
//----------------------------------------
bool CDatePeriod::Union(CDate& otherFrom, CDate& otherTo, CDatePeriod& unionDate)
{
   unionDate = *this;

  if ((otherFrom.IsDefaultValue()) || (otherTo.IsDefaultValue())) {
      return false;
  }

  if (this->IsDefaultValue()) {
      unionDate.SetFrom(otherFrom);
      unionDate.SetTo(otherTo);
      return true;
  }

  if (otherFrom < m_from)
  {
    unionDate.SetFrom(otherFrom);
  }
  else
  {
    unionDate.SetFrom(m_from);
  }

  if (otherTo > m_to)
  {
    unionDate.SetTo(otherTo);
  }
  else
  {
    unionDate.SetTo(m_to);
  }


  return true;

}
//----------------------------------------
std::string CDatePeriod::GetAsText(const std::string& delimiter /* = CDatePeriod::m_delimiter */)
{
  std::string str = GetFromAsText();
  str.append(delimiter);
  str.append(GetToAsText());
  return str;
}
//----------------------------------------
std::string CDatePeriod::GetFromAsText()
{
  return m_from.AsString(m_format, m_withMuSecond);
}
//----------------------------------------
std::string CDatePeriod::GetToAsText()
{
  return m_to.AsString(m_format, m_withMuSecond);
}
//----------------------------------------
std::string CDatePeriod::AsString(const std::string& format /*= ""*/, bool withMuSecond /* = false */)
{
  std::string str = m_from.AsString(format, withMuSecond);
  str.append("/");
  str.append(m_to.AsString(format, withMuSecond));
  return str;
}
//----------------------------------------
void CDatePeriod::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CDatePeriod Object at "<< this << std::endl;
  fOut << "==> m_from  at "<< &m_from << std::endl;
  m_from.Dump(fOut);
  fOut << "==> m_to  at "<< &m_to << std::endl;
  m_to.Dump(fOut);
  fOut << "==> END Dump a CDatePeriod Object at "<< this << std::endl;

  fOut << std::endl;

}

} // end namespace
