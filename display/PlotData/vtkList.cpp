
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

#include <string> 

#include "new-gui/Common/tools/TraceLog.h" 
#include "new-gui/Common/tools/Exception.h" 
#include "../libbrathl/Tools.h" 
#include "vtkList.h" 


//-------------------------------------------------------------
//------------------- vtkObList class --------------------
//-------------------------------------------------------------

vtkObList::vtkObList(bool bDelete /*= true*/)
{
  m_bDelete = bDelete;
}

//----------------------------------------
/*
vtkObList::vtkObList(const vtkObList& lst)
      	  : listvtkobject(lst)
{
  m_bDelete = lst.m_bDelete;
}
*/
//----------------------------------------

vtkObList::~vtkObList()
{
    RemoveAll();
}

//----------------------------------------
/*
const vtkObList& vtkObList::operator =(const vtkObList& lst)
{
  
  RemoveAll();
  
  m_bDelete = lst.m_bDelete;

  Insert(lst);
    
  return *this;

    
}
*/
//----------------------------------------

/*
void vtkObList::Insert(const vtkObList& lst, bool bEnd )
{
  
  if (lst.empty())
  {
    return;
  }

  vtkObList::const_iterator it;

  for (it = lst.begin() ; it != lst.end() ; it++)
  {
    vtkObject* ob = *it;
    if (ob != NULL)
    {
      Insert(ob, bEnd);
    }
  }
    
  
}
*/
//----------------------------------------

void vtkObList::Insert(vtkObject* ob, bool bEnd /*= true*/)
{
  
  if (bEnd)
  {  
    listvtkobject::push_back(ob);
  }
  else
  {  
    listvtkobject::push_front(ob);
  }
    
  
}
//----------------------------------------
bool vtkObList::Erase(vtkObList::iterator it)
{
  if (it == end())
  {
    return false;
  }

  bool bOk = false;


  vtkObject* ob = *it;
  if (ob != NULL)
  {
    bOk = true;
    listvtkobject::erase(it);
    if (m_bDelete)
    {
      ob->Delete();
    }
  }

  return bOk;

}

//----------------------------------------
bool vtkObList::PopBack()
{
  vtkObList::iterator it = --end(); // --end() --> iterator of the last element

  return Erase(it);

}
//----------------------------------------

void vtkObList::RemoveAll()
{
  
  vtkObList::iterator it;

  if (m_bDelete)
  {
    for (it = begin() ; it != end() ; it++)
    {
      vtkObject* ob = *it;
      if (ob != NULL)
      {
        ob->Delete();
      }
    }
  }

  listvtkobject::clear();


}

//----------------------------------------
void vtkObList::Dump(std::ostream& fOut /* = std::cerr */)
{

  if (CTrace::IsTrace() == false)
   {
      return;
   }

   vtkObList::iterator it;
   int i = 0;

   fOut << "==> Dump a vtkObList Object at " << this << " with " <<  size() << " elements" << std::endl;

   for ( it = this->begin( ); it != this->end( ); it++ )
   {
     fOut << "vtkObList[" << i << "] at " << (*it) << std::endl;  
     //(*it)->Dump(fOut);
     i++;
   }

   fOut << "==> END Dump a vtkObList Object at " << this << " with " <<  size() << " elements" << std::endl;
    
 
   fOut << std::endl;
   

}


//-------------------------------------------------------------
//------------------- vtkObArray class --------------------
//-------------------------------------------------------------

vtkObArray::vtkObArray(bool bDelete /*= true*/)
{
  m_bDelete = bDelete;

}

//----------------------------------------
/*
vtkObArray::vtkObArray(const vtkObArray& vect)
      	  : arrayvtkobject(vect)
{
  m_bDelete = vect.m_bDelete;  
}
*/
//----------------------------------------

vtkObArray::~vtkObArray()
{
  RemoveAll();

}

//----------------------------------------
/*
const vtkObArray& vtkObArray::operator =(const vtkObArray& vect)
{
  
  RemoveAll();

  m_bDelete = vect.m_bDelete;

  Insert(vect);
    
  return *this;

    
}
*/
//----------------------------------------

/*
void vtkObArray::Insert(const vtkObArray& vect)
{
  
  if (vect.empty())
  {
    return;
  }
  
  vtkObArray::const_iterator it;

  for (it = vect.begin() ; it != vect.end() ; it++)
  {
    vtkObject* ob = *it;
    if (ob != NULL)
    {
      Insert(ob);
    }
  }
    
  
}
*/
//----------------------------------------

void vtkObArray::Insert(vtkObject* ob)
{
  
  arrayvtkobject::push_back(ob);
      
}
//----------------------------------------
vtkObArray::iterator vtkObArray::InsertAt(vtkObArray::iterator where, vtkObject* ob)
{
  
  return arrayvtkobject::insert(where, ob);
      
}

//----------------------------------------
vtkObArray::iterator vtkObArray::ReplaceAt(vtkObArray::iterator where, vtkObject* ob)
{
  Erase(where);
  return InsertAt(where, ob);     
}
//----------------------------------------
bool vtkObArray::PopBack()
{
  vtkObArray::iterator it = end();
  it--; // --end() --> iterator of the last element

  return Erase(it);

}
//----------------------------------------
bool vtkObArray::Erase(int32_t index)
{
  return Erase(begin() + index);
}
//----------------------------------------

bool vtkObArray::Erase(vtkObArray::iterator it)
{
  if (it == end())
  {
    return false;
  }

  bool bOk = false;

  vtkObject* ob = *it;
  if (ob != NULL)
  {
    bOk = true;
    arrayvtkobject::erase(it);
    if (m_bDelete)
    {
      ob->Delete();
    }
  }

  return bOk;
}

//----------------------------------------

void vtkObArray::RemoveAll()
{
  
  vtkObArray::iterator it;

  if (m_bDelete)
  {
    for (it = begin() ; it != end() ; it++)
    {
      vtkObject* ob = *it;
      if (ob != NULL)
      {
        ob->Delete();
      }
    }
  }

  arrayvtkobject::clear();


}

//----------------------------------------
void vtkObArray::Dump(std::ostream& fOut /* = std::cerr */)
{

  if (CTrace::IsTrace() == false)
   {
      return;
   }

   vtkObArray::iterator it;
   int i = 0;

   fOut << "==> Dump a vtkObArray Object ast "<< this << " with " <<  size() << " elements" << std::endl;

   for ( it = this->begin( ); it != this->end( ); it++ )
   {
     fOut << "vtkObArray[" << i << "] at " << (*it) << std::endl;  
     //(*it)->Dump(fOut);
     i++;
   }

   fOut << "==> END Dump a vtkObArray Object at "<< this << " with " <<  size() << " elements" << std::endl;
    
 
   fOut << std::endl;
   

}



//-------------------------------------------------------------
//------------------- vtkObMap class --------------------
//-------------------------------------------------------------

vtkObMap::vtkObMap(bool bDelete /*= true*/)
{
  m_bDelete = bDelete;

}


//----------------------------------------
vtkObMap::~vtkObMap()
{
   RemoveAll();

}

//----------------------------------------

vtkObject* vtkObMap::Insert(const std::string& key, vtkObject* ob, bool withExcept /* = true */)
{
  

  std::pair <vtkObMap::iterator,bool> pairInsert;


  //If 'key' already exists --> pairInsert.second == false and
  // pairInsert.first then contains an iterator on the existing object
  // If 'key' does not exist --> pairInsert.second == true and
  // pairInsert.first then contains a iterator on the inserted object
  pairInsert = (mapvtkobject::insert(vtkObMap::value_type(key, ob)));

  if( (pairInsert.second == false) && (withExcept))
  {
    CException e("ERROR in vtkObMap::Insert - try to insert an object that already exists and parameter withExcept is true", BRATHL_LOGIC_ERROR);
    throw(e);
  }
  
  vtkObMap::iterator it = (pairInsert.first);
  return (*it).second;;

}
//----------------------------------------
/*
void vtkObMap::Insert(const vtkObMap& obMap, bool withExcept )
{
  
  vtkObMap::const_iterator it;

  for (it = obMap.begin() ; it != obMap.end() ; it++)
  {
    this->Insert((*it).first, (*it).second, withExcept);
  }

}
*/
//----------------------------------------

vtkObject* vtkObMap::Exists(const std::string& key)
{
  vtkObMap::iterator it = mapvtkobject::find(key);
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

void vtkObMap::RemoveAll()
{
  
  vtkObMap::iterator it;

  if (m_bDelete)
  {
    for (it = begin() ; it != end() ; it++)
    {
      vtkObject* ob = it->second;
      if (ob != NULL)
      {
        ob->Delete();
      }
    }
  }

  mapvtkobject::clear();

}
//----------------------------------------


bool vtkObMap::Erase(vtkObMap::iterator it)
{

  if (it == end())
  {
    return false;
  }

  bool bOk = false;

   vtkObject *ob = it->second;
   if (ob != NULL)
   {
     bOk = true;
     mapvtkobject::erase(it);
     if (m_bDelete)
     {
      ob->Delete();
     }
   }

   return bOk;

}
//----------------------------------------


bool vtkObMap::Erase(const std::string& key)
{

  vtkObMap::iterator it;

  it = mapvtkobject::find(key);

  return Erase(it);
  
}

//----------------------------------------

vtkObject* vtkObMap::operator[](const std::string& key)
{

  vtkObject *ob = mapvtkobject::operator[](key);
//   if (ob == NULL)
//   {
//     logic_error e("key not found");
//     throw e;
//   }

return  ob;
}

//----------------------------------------


void vtkObMap::Dump(std::ostream& fOut /* = std::cerr */) 
{

   if (CTrace::IsTrace() == false)
   {
      return;
   }

   fOut << "==> Dump a vtkObMap Object at "<< this << " with " <<  size() << " elements" << std::endl;
 
   vtkObMap::iterator it;

   for (it = this->begin() ; it != this->end() ; it++)
   {
      vtkObject *ob = it->second;
      fOut << "vtkObMap Key is = " << (*it).first << std::endl;
      fOut << "vtkObMap Value at " << ob << std::endl;
      //ob->Dump(fOut);
   }

   fOut << "==> END Dump a vtkObMap Object at "<< this << " with " <<  size() << " elements" << std::endl;
  
}

