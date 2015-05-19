
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


using namespace brathl;



namespace brathl
{



//-------------------------------------------------------------
//------------------- CCriteria class --------------------
//-------------------------------------------------------------


CCriteria::CCriteria() 
{

  Init();

}



//----------------------------------------

CCriteria::~CCriteria()
{

}

//----------------------------------------

void CCriteria::Init()
{
  m_key = CCriteria::UNKNOWN;
}
//----------------------------------------
CCriteria* CCriteria::GetCriteria(CBratObject* ob, bool withExcept /*= true*/)
{
  CCriteria* criteria = dynamic_cast<CCriteria*>(ob);
  if (withExcept)
  {
    if (criteria == NULL)
    {
      CException e("CCriteria::GetCriteria - dynamic_cast<CCriteria*>(ob) returns NULL"
                   "object seems not to be an instance of CCriteria",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }
  
  return criteria;

}
//-------------------------------------------------------------

void CCriteria::Adjust(CIntArray& array) 
{
  switch (array.size()) 
  {
    case 1: 
    {
      array.Insert(array.at(0)); 
      break;
    }
    case 0: 
    {
      return; 
    }
    default:
    {
      break;
    }

  }

  if (!(CTools::IsDefaultValue(array.at(0))) && !(CTools::IsDefaultValue(array.at(1)))) 
  {
    if (array.at(0) > array.at(1))
    {
      int32_t tmp = array.at(0);
      array.ReplaceAt(array.begin(), array.at(1));
      array.ReplaceAt(array.begin() + 1, tmp);
    }
  } 
  else if ((CTools::IsDefaultValue(array.at(0))) && !(CTools::IsDefaultValue(array.at(1)))) 
  {
    array.ReplaceAt(array.begin(), array.at(1));
  } 
  else if (!(CTools::IsDefaultValue(array.at(0))) && (CTools::IsDefaultValue(array.at(1)))) 
  {
    array.ReplaceAt(array.begin() + 1 , array.at(0));
  }

}

//----------------------------------------
void CCriteria::Dump(ostream& fOut /* = cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }
  fOut << "==> Dump a CCriteria Object at "<< this << endl;
  
  fOut << "m_key "<< m_key << endl;
  
  fOut << "==> END Dump a CCriteria Object at "<< this << endl;

  fOut << endl;

}

} // end namespace
