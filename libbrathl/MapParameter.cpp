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

#include "common/tools/brathl_error.h"
#include "brathl.h" 

#include "common/tools/Trace.h"
#include "common/tools/TraceLog.h"
#include "MapParameter.h"
using namespace brathl;

namespace brathl
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMapParameter::CMapParameter()
{
  
}


//----------------------------------------
CMapParameter::~CMapParameter()
{
   RemoveAll();

}

//----------------------------------------

CParameter* CMapParameter::Insert(const std::string& key, const std::string& value)
{
  

  std::pair <CMapParameter::iterator,bool> pairInsert;


  // Si la cle existe deja --> pairInsert.second == false et 
  // pairInsert.first contient alors une refrence sur l'objet  existant      
  // Si la cle n'existe pas --> pairInsert.second == true et 
  // pairInsert.first contient alors une refrence sur l'objet insere   
  
  CParameter *parameter = NULL;

  CMapParameter::iterator it = map_parameter::find(key);
  if (it != end())
  {
    parameter = (*it).second;
    parameter->AddValue(value.c_str());
  }
  else
  {
    parameter = new CParameter(key, value);  
    pairInsert = (map_parameter::insert(CMapParameter::value_type(key, parameter)));
  }


  return parameter;


}
//----------------------------------------

CParameter* CMapParameter::Exists(const std::string& key)
{
  CMapParameter::iterator it = map_parameter::find(key);
  if (it == end())
  {
    return NULL;
  }
  else
  {
    return (*it).second;
  }

}

//----------------------------------------

void CMapParameter::RemoveAll()
{
  
  CMapParameter::iterator iteratorParameter;

  for (iteratorParameter = begin() ; iteratorParameter != end() ; iteratorParameter++)
  {
    CParameter *pParameterTmp = iteratorParameter->second;
    if (pParameterTmp != NULL)
    {
      delete  pParameterTmp;
    }
  }

  map_parameter::clear();


}
//----------------------------------------


bool CMapParameter::Erase(CMapParameter::iterator iteratorParameter)
//CMapParameter::iterator CMapParameter::Erase(CMapParameter::iterator iteratorParameter)
{

   CParameter *pParameterTmp = iteratorParameter->second;
   // Sur Dec erase retourne void et non Iterator
   //CMapParameter::iterator it=  (map_parameter::erase(iteratorParameter));
   if (pParameterTmp != NULL)
   {
     delete  pParameterTmp;
     map_parameter::erase(iteratorParameter);
   }


   //return it;
   return true;

}
//----------------------------------------


bool CMapParameter::Erase(const std::string& key)
{

   CMapParameter::iterator iteratorParameter;

   iteratorParameter = map_parameter::find(key);

   if (iteratorParameter != end())
   {
      //CMapParameter::iterator it = Erase(iteratorParameter);
      Erase(iteratorParameter);
      return true;
   }

   return false;
  
}

//----------------------------------------

CParameter* CMapParameter::operator[](const std::string key)
{

  CParameter *p = map_parameter::operator[](key);
//   if (p == NULL)
//   {
//     logic_error e("key not found");
//     throw e;
//   }

return  p;
}

//----------------------------------------


void CMapParameter::Dump(std::ostream& fOut /* = std::cerr */) 
{

   if (CTrace::IsTrace() == false)
   {
      return;
   }

   fOut << "==> Dump a CMapParameter Object at "<< this << " with " <<  size() << " elements" << std::endl;
 
   CMapParameter::iterator iteratorParameter;

   for (iteratorParameter = this->begin() ; iteratorParameter != this->end() ; iteratorParameter++)
   {
      CParameter *pParameterTmp = iteratorParameter->second;
      fOut << "CMapParameter Key is = " << (*iteratorParameter).first << std::endl;
      fOut << "CMapParameter Value is = " << std::endl;
      pParameterTmp->Dump(fOut);
   }

   fOut << "==> END Dump a CMapParameter Object at "<< this << " with " <<  size() << " elements" << std::endl;
  
}


}    
    
