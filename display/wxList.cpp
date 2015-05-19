
/*
* Copyright (C) 2005 Collecte Localisation Satellites (CLS), France (31)
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

#include "Stl.h" 

#include "TraceLog.h" 
#include "Exception.h" 
#include "Tools.h" 
#include "wxList.h" 


//-------------------------------------------------------------
//------------------- wxObList class --------------------
//-------------------------------------------------------------

wxObList::wxObList(bool bDelete /*= true*/)
{
  m_bDelete = bDelete;
}

//----------------------------------------
/*
wxObList::wxObList(const wxObList& lst)
      	  : listwxobject(lst)
{
  m_bDelete = lst.m_bDelete;
}
*/
//----------------------------------------

wxObList::~wxObList()
{
    RemoveAll();
}

//----------------------------------------
/*
const wxObList& wxObList::operator =(const wxObList& lst)
{
  
  RemoveAll();
  
  m_bDelete = lst.m_bDelete;

  Insert(lst);
    
  return *this;

    
}
*/
//----------------------------------------

/*
void wxObList::Insert(const wxObList& lst, bool bEnd )
{
  
  if (lst.empty())
  {
    return;
  }

  wxObList::const_iterator it;

  for (it = lst.begin() ; it != lst.end() ; it++)
  {
    wxObject* ob = *it;
    if (ob != NULL)
    {
      Insert(ob, bEnd);
    }
  }
    
  
}
*/
//----------------------------------------

void wxObList::Insert(wxObject* ob, bool bEnd /*= true*/)
{
  
  if (bEnd)
  {  
    listwxobject::push_back(ob);
  }
  else
  {  
    listwxobject::push_front(ob);
  }
    
  
}
//----------------------------------------
bool wxObList::Erase(wxObList::iterator it)
{
  if (it == end())
  {
    return false;
  }

  bool bOk = false;


  wxObject* ob = *it;
  if (ob != NULL)
  {
    bOk = true;
    listwxobject::erase(it);
    if (m_bDelete)
    {
      delete ob;
      ob = NULL;
    }
  }

  return bOk;

}

//----------------------------------------
bool wxObList::PopBack()
{
  wxObList::iterator it = --end(); // --end() --> iterator of the last element

  return Erase(it);

}
//----------------------------------------

void wxObList::RemoveAll()
{
  
  wxObList::iterator it;

  if (m_bDelete)
  {
    for (it = begin() ; it != end() ; it++)
    {
      wxObject* ob = *it;
      if (ob != NULL)
      {
        delete ob;
        ob = NULL;
      }
    }
  }

  listwxobject::clear();


}

//----------------------------------------
void wxObList::Dump(ostream& fOut /* = cerr */)
{

  if (CTrace::IsTrace() == false)
   {
      return;
   }

   wxObList::iterator it;
   int i = 0;

   fOut << "==> Dump a wxObList Object at " << this << " with " <<  size() << " elements" << endl;

   for ( it = this->begin( ); it != this->end( ); it++ )
   {
     fOut << "wxObList[" << i << "] at " << (*it) << endl;  
     //(*it)->Dump(fOut);
     i++;
   }

   fOut << "==> END Dump a wxObList Object at " << this << " with " <<  size() << " elements" << endl;
    
 
   fOut << endl;
   

}


//-------------------------------------------------------------
//------------------- wxObArray class --------------------
//-------------------------------------------------------------

wxObArray::wxObArray(bool bDelete /*= true*/)
{
  m_bDelete = bDelete;

}

//----------------------------------------
/*
wxObArray::wxObArray(const wxObArray& vect)
      	  : arraywxobject(vect)
{
  m_bDelete = vect.m_bDelete;  
}
*/
//----------------------------------------

wxObArray::~wxObArray()
{
  RemoveAll();

}

//----------------------------------------
/*
const wxObArray& wxObArray::operator =(const wxObArray& vect)
{
  
  RemoveAll();

  m_bDelete = vect.m_bDelete;

  Insert(vect);
    
  return *this;

    
}
*/
//----------------------------------------

/*
void wxObArray::Insert(const wxObArray& vect)
{
  
  if (vect.empty())
  {
    return;
  }
  
  wxObArray::const_iterator it;

  for (it = vect.begin() ; it != vect.end() ; it++)
  {
    wxObject* ob = *it;
    if (ob != NULL)
    {
      Insert(ob);
    }
  }
    
  
}
*/
//----------------------------------------

void wxObArray::Insert(wxObject* ob)
{
  
  arraywxobject::push_back(ob);
      
}
//----------------------------------------
wxObArray::iterator wxObArray::InsertAt(wxObArray::iterator where, wxObject* ob)
{
  
  return arraywxobject::insert(where, ob);
      
}

//----------------------------------------
wxObArray::iterator wxObArray::ReplaceAt(wxObArray::iterator where, wxObject* ob)
{
  Erase(where);
  return InsertAt(where, ob);     
}
//----------------------------------------
bool wxObArray::PopBack()
{
  wxObArray::iterator it = end();
  it--; // --end() --> iterator of the last element

  return Erase(it);

}
//----------------------------------------
bool wxObArray::Erase(int32_t index)
{
  return Erase(begin() + index);
}
//----------------------------------------

bool wxObArray::Erase(wxObArray::iterator it)
{
  if (it == end())
  {
    return false;
  }

  bool bOk = false;

  wxObject* ob = *it;
  if (ob != NULL)
  {
    bOk = true;
    arraywxobject::erase(it);
    if (m_bDelete)
    {
      delete ob;
      ob = NULL;
    }
  }

  return bOk;
}

//----------------------------------------

void wxObArray::RemoveAll()
{
  
  wxObArray::iterator it;

  if (m_bDelete)
  {
    for (it = begin() ; it != end() ; it++)
    {
      wxObject* ob = *it;
      if (ob != NULL)
      {
        delete ob;
        ob = NULL;
      }
    }
  }

  arraywxobject::clear();


}

//----------------------------------------
void wxObArray::Dump(ostream& fOut /* = cerr */)
{

  if (CTrace::IsTrace() == false)
   {
      return;
   }

   wxObArray::iterator it;
   int i = 0;

   fOut << "==> Dump a wxObArray Object ast "<< this << " with " <<  size() << " elements" << endl;

   for ( it = this->begin( ); it != this->end( ); it++ )
   {
     fOut << "wxObArray[" << i << "] at " << (*it) << endl;  
     //(*it)->Dump(fOut);
     i++;
   }

   fOut << "==> END Dump a wxObArray Object at "<< this << " with " <<  size() << " elements" << endl;
    
 
   fOut << endl;
   

}



//-------------------------------------------------------------
//------------------- wxObMap class --------------------
//-------------------------------------------------------------

wxObMap::wxObMap(bool bDelete /*= true*/)
{
  m_bDelete = bDelete;

}


//----------------------------------------
wxObMap::~wxObMap()
{
   RemoveAll();

}

//----------------------------------------

wxObject* wxObMap::Insert(const string& key, wxObject* ob, bool withExcept /* = true */)
{
  

  pair <wxObMap::iterator,bool> pairInsert;


  //If 'key' already exists --> pairInsert.second == false and
  // pairInsert.first then contains an iterator on the existing object
  // If 'key' does not exist --> pairInsert.second == true and
  // pairInsert.first then contains a iterator on the inserted object
  pairInsert = (mapwxobject::insert(wxObMap::value_type(key, ob)));

  if( (pairInsert.second == false) && (withExcept))
  {
    CException e("ERROR in wxObMap::Insert - try to insert an object that already exists and parameter withExcept is true", BRATHL_LOGIC_ERROR);
    throw(e);
  }
  
  wxObMap::iterator it = (pairInsert.first);
  return (*it).second;;

}
//----------------------------------------
/*
void wxObMap::Insert(const wxObMap& obMap, bool withExcept )
{
  
  wxObMap::const_iterator it;

  for (it = obMap.begin() ; it != obMap.end() ; it++)
  {
    this->Insert((*it).first, (*it).second, withExcept);
  }

}
*/
//----------------------------------------

wxObject* wxObMap::Exists(const string& key)
{
  wxObMap::iterator it = mapwxobject::find(key);
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

void wxObMap::RemoveAll()
{
  
  wxObMap::iterator it;

  if (m_bDelete)
  {
    for (it = begin() ; it != end() ; it++)
    {
      wxObject* ob = it->second;
      if (ob != NULL)
      {
        delete ob;
        ob = NULL;
      }
    }
  }

  mapwxobject::clear();

}
//----------------------------------------


bool wxObMap::Erase(wxObMap::iterator it)
{

  if (it == end())
  {
    return false;
  }

  bool bOk = false;

   wxObject *ob = it->second;
   if (ob != NULL)
   {
     bOk = true;
     mapwxobject::erase(it);
     if (m_bDelete)
     {
       delete ob;
       ob = NULL;
     }
   }

   return bOk;

}
//----------------------------------------


bool wxObMap::Erase(const string& key)
{

  wxObMap::iterator it;

  it = mapwxobject::find(key);

  return Erase(it);
  
}

//----------------------------------------

wxObject* wxObMap::operator[](const string& key)
{

  wxObject *ob = mapwxobject::operator[](key);
//   if (ob == NULL)
//   {
//     logic_error e("key not found");
//     throw e;
//   }

return  ob;
}

//----------------------------------------


void wxObMap::Dump(ostream& fOut /* = cerr */) 
{

   if (CTrace::IsTrace() == false)
   {
      return;
   }

   fOut << "==> Dump a wxObMap Object at "<< this << " with " <<  size() << " elements" << endl;
 
   wxObMap::iterator it;

   for (it = this->begin() ; it != this->end() ; it++)
   {
      wxObject *ob = it->second;
      fOut << "wxObMap Key is = " << (*it).first << endl;
      fOut << "wxObMap Value at " << ob << endl;
      //ob->Dump(fOut);
   }

   fOut << "==> END Dump a wxObMap Object at "<< this << " with " <<  size() << " elements" << endl;
  
}

