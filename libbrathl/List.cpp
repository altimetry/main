
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

#include "Stl.h"  

#include "TraceLog.h" 
#include "Exception.h"
 
#include "List.h" 
using namespace brathl;

namespace brathl
{
const char* DUMP_FORMAT_DOUBLE = "%.15g";
//-------------------------------------------------------------
//------------------- CStringList class --------------------
//-------------------------------------------------------------



CStringList::CStringList()
{
  
}

//----------------------------------------

CStringList::CStringList(const CStringList& lst)
      	  : stringlist(lst)
{
  
}
//----------------------------------------

CStringList::CStringList(const stringlist& lst)
      	  : stringlist(lst)
{
  
}
//----------------------------------------

CStringList::CStringList(const CStringArray& vect)
{
  this->Insert(vect);  
}

//----------------------------------------

CStringList::CStringList(const stringarray& vect)
{
  this->Insert(vect);  
}

//----------------------------------------

CStringList::~CStringList()
{

}
//----------------------------------------
const CStringList& CStringList::operator =(const CStringList& lst)
{
  
  this->clear();

  Insert(lst);
    
  return *this;
   
}
//----------------------------------------
const CStringList& CStringList::operator =(const CStringArray& vect)
{
  
  this->clear();

  Insert(vect);
    
  return *this;
   
}
//----------------------------------------
const CStringList& CStringList::operator =(const stringarray& vect)
{
  
  this->clear();

  Insert(vect);
    
  return *this;
   
}
//----------------------------------------
const CStringList& CStringList::operator =(const stringlist& lst)
{
  
  this->clear();

  Insert(lst);
    
  return *this;
   
}
//----------------------------------------

void CStringList::Insert(const CStringArray& vect, bool bEnd /*= true*/)
{
  
  if (vect.empty())
  {
    return;
  }

  if (bEnd)
  {  
    stringlist::insert(this->end(), vect.begin(), vect.end());
  }
  else
  {  
    stringlist::insert(this->begin(), vect.begin(), vect.end());
  }
    
  
}
//----------------------------------------

void CStringList::Insert(const stringarray& vect, bool bEnd /*= true*/)
{
  if (vect.empty())
  {
    return;
  }
  
  if (bEnd)
  {  
    stringlist::insert(this->end(), vect.begin(), vect.end());
  }
  else
  {  
    stringlist::insert(this->begin(), vect.begin(), vect.end());
  }
    
  
}

//----------------------------------------

void CStringList::Insert(const CStringList& lst, bool bEnd /*= true*/)
{
  
  if (lst.empty())
  {
    return;
  }

  if (bEnd)
  {  
    stringlist::insert(this->end(), lst.begin(), lst.end());
  }
  else
  {  
    stringlist::insert(this->begin(), lst.begin(), lst.end());
  }
    
  
}

//----------------------------------------
void CStringList::Insert(const stringlist& lst, bool bEnd /*= true*/)
{
  if (lst.empty())
  {
    return;
  }
  
  if (bEnd)
  {  
    stringlist::insert(this->end(), lst.begin(), lst.end());
  }
  else
  {  
    stringlist::insert(this->begin(), lst.begin(), lst.end());
  }
    
  
}

//----------------------------------------

void CStringList::Insert(const string& str, bool bEnd /*= true*/)
{
  
  if (bEnd)
  {  
    stringlist::push_back(str);
  }
  else
  {  
    stringlist::push_front(str);
  }
    
  
}

//----------------------------------------
bool CStringList::Exists(const string& str) const
{
   CStringList::const_iterator it;

   for ( it = this->begin( ); it != this->end( ); it++ )
   {
     if (str.compare(*it) == 0)
     {
       return true;
     }
   }
    
  return false;  
}
//----------------------------------------
bool CStringList::ExistsNoCase(const string& str) const
{
   CStringList::const_iterator it;

   for ( it = this->begin( ); it != this->end( ); it++ )
   {
     if (CTools::CompareNoCase(str, *it))
     {
       return true;
     }
   }
    
  return false;  
}
//----------------------------------------
int32_t CStringList::FindIndex(const string& str, bool compareNoCase /* = false */) const
{
  CStringList::const_iterator it;
  int32_t i = -1;

  for (it = this->begin() ; it != this->end() ; it++ )
  {
    i++;
    if (compareNoCase)
    {
      if (CTools::CompareNoCase(str, *it))
      {
       return i;
      }
    }
    else
    {
      if (str.compare(*it) == 0)
      {
       return i;
      }
    }
  }
    
  return -1;  
}

//----------------------------------------

void CStringList::InsertUnique(const string& str, bool bEnd /*= true*/)
{
  if (this->Exists(str))
  {
    return;
  }

  this->Insert(str, bEnd);
  
}

//----------------------------------------
void CStringList::InsertUnique(const CStringList& lst, bool bEnd /*= true*/)
{
   CStringList::const_iterator it;

   for ( it = lst.begin( ); it != lst.end( ); it++ )
   {
     this->InsertUnique(*it, bEnd);
   }

}

//----------------------------------------
void CStringList::InsertUnique(const CStringArray& vect, bool bEnd /*= true*/)
{
   CStringArray::const_iterator it;

   for ( it = vect.begin( ); it != vect.end( ); it++ )
   {
     this->InsertUnique(*it, bEnd);
   }

}
//----------------------------------------
void CStringList::InsertUnique(const CStringArray* vect, bool bEnd /*= true*/)
{
  InsertUnique(*vect, bEnd);
  
}

//----------------------------------------
void CStringList::InsertUnique(const stringarray& vect, bool bEnd /*= true*/)
{
   vector<string>::const_iterator it;

   for ( it = vect.begin( ); it != vect.end( ); it++ )
   {
     this->InsertUnique(*it, bEnd);
   }

}
//----------------------------------------
void CStringList::InsertUnique(const stringlist& lst, bool bEnd /*= true*/)
{
   list<string>::const_iterator it;

   for ( it = lst.begin( ); it != lst.end( ); it++ )
   {
     this->InsertUnique(*it, bEnd);
   }

}
//----------------------------------------
void CStringList::RemoveAll()
{  
  this->clear();
}

//----------------------------------------
bool CStringList::Intersect(const CStringList& array, CStringList& intersect) const
{
  intersect.RemoveAll();

  CStringList::const_iterator itA;
  CStringList::const_iterator itB;

  for ( itA = this->begin( ); itA != this->end( ); itA++ )
  {
    for ( itB = array.begin( ); itB != array.end( ); itB++ )
    {
      if ( (*itA).compare((*itB)) == 0)
      {
        intersect.Insert((*itB));
      }
    }
  }

  return (!(intersect.empty()));
}
//----------------------------------------
bool CStringList::Complement(const CStringList& array, CStringList& complement) const
{
  CStringList intersect;
  complement.clear();
  
  if (Intersect(array, intersect) == false)
  {
    complement.Insert(array);
    return (!(complement.empty()));
  }


  CStringList::const_iterator itArray;
  CStringList::const_iterator itIntersect = intersect.begin();

  for ( itArray = array.begin(); itArray != array.end(); itArray++ )
  {
    bool found = false;
    for ( itIntersect = intersect.begin(); itIntersect != intersect.end(); itIntersect++ )
    {
      if ( (*itArray) == (*itIntersect) )
      {
        found = true;
        break;
      }
    }

    if (!found)
    {
      complement.Insert(*itArray);
    }

  }

  return (!(complement.empty()));
}


//----------------------------------------

void CStringList::ExtractStrings(const string& str, const char delim, bool bRemoveAll /* = true */) 
{

if (bRemoveAll == true)
  {
  this->clear();
  }

string sequence = str;

int32_t pos = sequence.find(delim);

uint32_t nLg;

while (sequence.empty() == false)
  {

  if (pos > 0)
    {
    this->Insert(sequence.substr(0, pos));
    }
  if (pos < 0)
    {
    this->Insert(sequence);
    sequence = "";
    }

  nLg = (sequence.length() - pos - 1);
  
  if (nLg > 0)
    {
    sequence = sequence.substr(pos + 1, nLg);
    }
  else
    {
    sequence = "";
    }

  pos = sequence.find(delim);
  }

return;
}


//----------------------------------------

void CStringList::ExtractStrings(const string& str, const string& delim, bool bRemoveAll /* = true */) 
{

if (bRemoveAll == true)
  {
  this->clear();
  }

string sequence = str;

int32_t pos = sequence.find(delim);

uint32_t nLg;

while (sequence.empty() == false)
  {

  if (pos > 0)
    {
    this->Insert(sequence.substr(0, pos));
    }
  if (pos < 0)
    {
    this->Insert(sequence);
    sequence = "";
    }

  nLg = (sequence.length() - pos - delim.length());
  
  if (nLg > 0)
    {
    sequence = sequence.substr(pos + 1, nLg);
    }
  else
    {
    sequence = "";
    }

  pos = sequence.find(delim);
  }

return;
}

//----------------------------------------

string CStringList::ToString(const string& delim /*= "," */, bool useBracket /*= true */) const
{
  string str;
  CStringList::const_iterator it;
  for ( it = this->begin( ); it != this->end( ); it++ )
  {
    str.append(*it);
    str.append(delim);
  }
  
  if (str.empty() == false)
  {
    str.erase(str.end() - delim.size(), str.end());
  }

  return str;
}
//----------------------------------------
void CStringList::Erase(const string& str) 
{
  
  CStringList::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    if (str == *it)
    {
      Erase(it);
      break;
    }
  }
 
}
//----------------------------------------

void CStringList::Erase(CStringList::iterator it)
{
  if (it == end())
  {
    return;
  }

  stringlist::erase(it);

}
//----------------------------------------
void CStringList::ExtractKeys(const string& str, const string& delim, bool bRemoveAll /* = true */) 
{

if (bRemoveAll == true)
  {
  this->clear();
  }

string sequence = str;

int32_t pos = sequence.find(delim, 0);

uint32_t beginSearch = 0;

while (true)
{
  if (pos > 0)
    {
    this->Insert(sequence.substr(0, pos));
    }
  if (pos < 0)
    {
    this->Insert(sequence);
    break;
    }

  beginSearch = pos + 1;
 
  if (beginSearch < sequence.length())
  {
    pos = sequence.find(delim, beginSearch);
  }
  else
  {
    pos = -1;
  }
}

return;
}

//----------------------------------------
void CStringList::Dump(ostream& fOut /* = cerr */) const
{

  if (CTrace::IsTrace() == false)
   {
      return;
   }

   CStringList::const_iterator it;
   int i = 0;

   fOut << "==> Dump a CStringList Object at "<< this << " with " <<  size() << " elements" << endl;

   for ( it = this->begin( ); it != this->end( ); it++ )
   {
     fOut << "CStringList[" << i << "]= " << *it << endl;  
     i++;
   }
    
   fOut << "==> END Dump a CStringList Object at "<< this << " with " <<  size() << " elements" << endl;

 
   fOut << endl;
   

}

//-------------------------------------------------------------
//------------------- CIntList class --------------------
//-------------------------------------------------------------



CIntList::CIntList()
{
  
}

//----------------------------------------

CIntList::CIntList(const CIntList& lst)
      	  : intlist(lst)
{
  
}

//----------------------------------------

CIntList::~CIntList()
{

}
//----------------------------------------


const CIntList& CIntList::operator =(const CIntList& lst)
{
  
  this->clear();

  Insert(lst);
    
  return *this;

    
}
//----------------------------------------


void CIntList::Insert(const CIntList& lst, bool bEnd /*= true*/)
{
  
  if (lst.empty())
  {
    return;
  }

  if (bEnd)
  {  
    intlist::insert(this->end(), lst.begin(), lst.end());
  }
  else
  {  
    intlist::insert(this->begin(), lst.begin(), lst.end());
  }
    
  
}
//----------------------------------------

void CIntList::Insert(const int value, bool bEnd /*= true*/)
{
  
  if (bEnd)
  {  
    intlist::push_back(value);
  }
  else
  {  
    intlist::push_front(value);
  }
    
  
}


//----------------------------------------
void CIntList::Dump(ostream& fOut /* = cerr */) const
{

  if (CTrace::IsTrace() == false)
   {
      return;
   }

   CIntList::const_iterator it;
   int i = 0;

   fOut << "==> Dump a CIntList Object at "<< this << " with " <<  size() << " elements" << endl;

   for ( it = this->begin( ); it != this->end( ); it++ )
   {
     fOut << "CIntList[" << i << "]= " << *it << endl;  
     i++;
   }
    
   fOut << "==> END Dump a CIntList Object at "<< this << " with " <<  size() << " elements" << endl;

 
   fOut << endl;
   

}


//-------------------------------------------------------------
//------------------- CObList class --------------------
//-------------------------------------------------------------

CObList::CObList(bool bDelete /*= true*/)
{
  m_bDelete = bDelete;
}

//----------------------------------------

CObList::CObList(const CObList& lst)
{
  *this = lst;
}

//----------------------------------------

CObList::~CObList()
{
    RemoveAll();
}

//----------------------------------------

const CObList& CObList::operator =(const CObList& lst)
{
  
  RemoveAll();
  
  m_bDelete = lst.m_bDelete;

  Insert(lst);
    
  return *this;

    
}

//----------------------------------------
void CObList::Insert(const CObList& lst, bool bEnd )
{
  if (lst.empty())
  {
    return;
  }

  CObList::const_iterator it;

  for (it = lst.begin() ; it != lst.end() ; it++)
  {
    CBratObject* ob = *it;
    if (ob != NULL)
    {
      Insert(ob->Clone(), bEnd);
    }
  }
      
}
//----------------------------------------

void CObList::Insert(CBratObject* ob, bool bEnd /*= true*/)
{
  
  if (bEnd)
  {  
    oblist::push_back(ob);
  }
  else
  {  
    oblist::push_front(ob);
  }
    
  
}
//----------------------------------------
bool CObList::Erase(CBratObject* ob)
{
  if (ob == NULL)
  {
    return false;
  }

  CObList::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    if (ob == *it)
    {
      return Erase(it);
    }
  }
 
  return false;
}

//----------------------------------------

bool CObList::Erase(CObList::iterator it)
{
  if (it == end())
  {
    return false;
  }

  bool bOk = false;


  CBratObject* ob = *it;
  if (ob != NULL)
  {
    bOk = true;
    oblist::erase(it);
    if (m_bDelete)
    {
      delete  ob;
    }
  }

  return bOk;

}

//----------------------------------------

bool CObList::PopBack()
{
  CObList::iterator it = --end(); // --end() --> iterator of the last element

  return Erase(it);

}
//----------------------------------------

void CObList::RemoveAll()
{
  
  CObList::iterator it;

  if (m_bDelete)
  {
    for (it = begin() ; it != end() ; it++)
    {
      CBratObject* ob = *it;
      if (ob != NULL)
      {
          delete  ob;
      }
    }
  }

  oblist::clear();


}

//----------------------------------------
void CObList::Dump(ostream& fOut /* = cerr */) const
{

  if (CTrace::IsTrace() == false)
  {
    return;
  }

  CObList::const_iterator it;
  int i = 0;

  fOut << "==> Dump a CObList Object at " << this << " with " <<  size() << " elements" << endl;

  for ( it = this->begin( ); it != this->end( ); it++ )
  {
    fOut << "CObList[" << i << "]= " << endl;  
    if ((*it) != NULL)
    {
      (*it)->Dump(fOut);
    }
    i++;
  }

  fOut << "==> END Dump a CObList Object at " << this << " with " <<  size() << " elements" << endl;


  fOut << endl;
   

}

//-------------------------------------------------------------
//------------------- CStringArray class --------------------
//-------------------------------------------------------------

CStringArray::CStringArray()
{
  
}

//----------------------------------------

CStringArray::CStringArray(const CStringArray& vect)
      	  : stringarray(vect)
{
  
}

//----------------------------------------

CStringArray::CStringArray(const stringarray& vect)
      	  : stringarray(vect)
{
  
}

//----------------------------------------

CStringArray::CStringArray(const CStringList& lst)
{
  this->Insert(lst);  
}


//----------------------------------------

CStringArray::CStringArray(const stringlist& lst)
{
  this->Insert(lst);  
}

//----------------------------------------

CStringArray::~CStringArray()
{

}

//----------------------------------------
bool CStringArray::operator ==(const CStringArray& vect)
{
  uint32_t count = this->size();

  if (count != vect.size())
  {
    return false;
  }

  if (count == 0)
  {
    return true;
  }

  bool isEqual = true;

  for (uint32_t i = 0 ; i < count ; i++)
  {
    if (this->at(i) != vect.at(i))
    {
      isEqual = false;
      break;
    }
  }

  return isEqual;    
}

//----------------------------------------

const CStringArray& CStringArray::operator =(const CStringArray& vect)
{
  
  this->clear();

  Insert(vect);
    
  return *this;

    
}

//----------------------------------------

const CStringArray& CStringArray::operator =(const CStringList& lst)
{
  
  this->clear();

  Insert(lst);
    
  return *this;

    
}
//----------------------------------------

const CStringArray& CStringArray::operator =(const stringarray& vect)
{
  
  this->clear();

  Insert(vect);
    
  return *this;

    
}

//----------------------------------------

const CStringArray& CStringArray::operator =(const stringlist& lst)
{
  
  this->clear();

  Insert(lst);
    
  return *this;

    
}

//----------------------------------------
void CStringArray::RemoveAll()
{
  
  this->clear();

}
//----------------------------------------

void CStringArray::Insert(const CStringArray& vect, bool bEnd /*= true*/)
{
  
  if (vect.empty())
  {
    return;
  }

  if (bEnd)
  {  
    stringarray::insert(this->end(), vect.begin(), vect.end());
  }
  else
  {  
    stringarray::insert(this->begin(), vect.begin(), vect.end());
  }    
  
}
//----------------------------------------

void CStringArray::Insert(const stringarray& vect, bool bEnd /*= true*/)
{
  if (vect.empty())
  {
    return;
  }
  
  if (bEnd)
  {  
    stringarray::insert(this->end(), vect.begin(), vect.end());
  }
  else
  {  
    stringarray::insert(this->begin(), vect.begin(), vect.end());
  }    
  
}
//----------------------------------------

void CStringArray::Insert(const string& str)
{
  
stringarray::push_back(str);    
  
}

//----------------------------------------

void CStringArray::Insert(const CStringList& lst)
{
 
   CStringList::const_iterator it;

   for ( it = lst.begin( ); it != lst.end( ); it++ )
   {
     this->Insert(*it);
   }
  
}

//----------------------------------------

void CStringArray::Insert(const CIntArray& vect)
{
 
  CIntArray::const_iterator it;

  for ( it = vect.begin( ); it != vect.end( ); it++ )
  {
    string str = CTools::IntToStr(*it);
    if (!(str.empty()))
    {
      this->Insert(str);
    }
  }
  
}

//----------------------------------------

void CStringArray::Insert(const stringlist& lst)
{
 
  list<string>::const_iterator it;

   for ( it = lst.begin( ); it != lst.end( ); it++ )
   {
     this->Insert(*it);
   }
  
}

//----------------------------------------
CStringArray::iterator CStringArray::InsertAt(CStringArray::iterator where, const string& str)
{ 
  return stringarray::insert(where, str);     
}

//----------------------------------------
CStringArray::iterator CStringArray::InsertAt(int32_t index, const string& str)
{ 
  if (index < 0)
  {
    return this->end();
  }

  if ((uint32_t)index >= this->size())
  {
    this->resize(index + 1);
    (*this)[index] = str;
    return this->begin() + index;
  }

  return stringarray::insert(this->begin() + index, str);     
}

//----------------------------------------
bool CStringArray::Erase(int32_t index)
{
  if (index < 0)
  {
    return false;
  }

  return Erase(this->begin() + index);   
}
//----------------------------------------
bool CStringArray::Erase(uint32_t index)
{
  return Erase(this->begin() + index);   
}
//----------------------------------------
bool CStringArray::Erase(CStringArray::iterator where)
{ 
  stringarray::erase(where);   
  return true;
}
//----------------------------------------
CStringArray::iterator CStringArray::ReplaceAt(uint32_t index, const string& str)
{
  (*this)[index] = str;
  return this->begin() + index;
}
//----------------------------------------
CStringArray::iterator CStringArray::ReplaceAt(int32_t index, const string& str)
{
  if (index < 0)
  {
    return this->end();
  }

  (*this)[index] = str;
  return this->begin() + index;

}
//----------------------------------------
CStringArray::iterator CStringArray::ReplaceAt(CStringArray::iterator where, const string& str)
{
  Erase(where);
  return InsertAt(where, str);     
}

//----------------------------------------
bool CStringArray::Exists(const string& str, bool compareNoCase /* = false */) const
{
  CStringArray::const_iterator it;

  for ( it = this->begin( ); it != this->end( ); it++ )
  {
    if (compareNoCase)
    {
      if (CTools::CompareNoCase(str, *it))
      {
        return true;
      }
    }
    else
    {
      if (str.compare(*it) == 0)
      {
        return true;
      }
    }
  }

  return false;  
}


//----------------------------------------
int32_t CStringArray::FindIndex(const string& str, bool compareNoCase /* = false */) const
{
  for (uint32_t i = 0 ; i < size() ; i++ )
  {
    if (compareNoCase)
    {
      if (CTools::CompareNoCase(str, this->at(i)))
      {
       return i;
      }
    }
    else
    {
      if (str.compare(this->at(i)) == 0)
      {
       return i;
      }
    }
  }
    
  return -1;  
}
//----------------------------------------
void CStringArray::FindIndexes(const string& str, CIntArray& indexes, bool compareNoCase /* = false */) const
{
  for (uint32_t i = 0 ; i < size() ; i++ )
  {
    if (compareNoCase)
    {
      if (CTools::CompareNoCase(str, this->at(i)))
      {
       indexes.Insert(i);
      }
    }
    else
    {
      if (str.compare(this->at(i)) == 0)
      {
       indexes.Insert(i);
      }
    }
  }
    
}
//----------------------------------------
void CStringArray::GetValues(const CIntArray& indexes, CStringArray& values) const
{
  CIntArray::const_iterator it;

  int32_t maxIndex =  this->size() - 1;

  for ( it = indexes.begin( ); it != indexes.end( ); it++ )
  {
    if (*it > maxIndex)
    {
      continue;
    }

    values.Insert(this->at(*it));
  }
    
}

//----------------------------------------
void CStringArray::GetValues(const CIntArray& indexes, string& values) const
{
  CStringArray arrayValues;

  GetValues(indexes, arrayValues);

  values = arrayValues.ToString("\n", false);
}

//----------------------------------------
bool CStringArray::Intersect(const string& str, CStringArray& intersect, bool compareNoCase /* = false */) const
{
  CStringArray array;
  array.Insert(str);

  return Intersect(array, intersect, compareNoCase);

}
//----------------------------------------
bool CStringArray::Intersect(const CStringArray& array, CStringArray& intersect, bool compareNoCase /* = false */) const
{
  intersect.RemoveAll();

  CStringArray::const_iterator itA;
  CStringArray::const_iterator itB;
  bool same = false;

  for ( itA = this->begin( ); itA != this->end( ); itA++ )
  {
    for ( itB = array.begin( ); itB != array.end( ); itB++ )
    {
      
      same = false;

      if (compareNoCase)
      {
        same = CTools::CompareNoCase(*itA, *itB);
      }
      else
      {
        same = ( (*itA).compare((*itB)) == 0);
      }

      if (same)
      {
        intersect.Insert((*itB));
      }
    }
  }

  return (!(intersect.empty()));
}
//----------------------------------------
bool CStringArray::Intersect(const string& str, CUIntArray& intersect, bool compareNoCase /* = false */) const
{
  CStringArray array;
  array.Insert(str);

  return Intersect(array, intersect, compareNoCase);
}
//----------------------------------------
bool CStringArray::Intersect(const CStringArray& array, CUIntArray& intersect, bool compareNoCase /* = false */) const
{
  intersect.RemoveAll();
  bool same = false;

  for (uint32_t iA = 0 ; iA < this->size() ; iA++ )
  {
    for (uint32_t iB = 0 ; iB < array.size() ; iB++ )
    {
      same = false;

      if (compareNoCase)
      {
        same = CTools::CompareNoCase(this->at(iA), array.at(iB));
      }
      else
      {
        same = ( this->at(iA).compare(array.at(iB)) == 0);
      }

      if (same)
      {
        intersect.Insert(iA);
      }
    }
  }

  return (!(intersect.empty()));
}

//----------------------------------------
bool CStringArray::Remove(const string& str, bool compareNoCase /* = false */)
{
  CStringArray array;
  array.Insert(str);
  return Remove(array, compareNoCase);
}
//----------------------------------------
bool CStringArray::Remove(const CStringArray& array, bool compareNoCase /* = false */)
{

  CUIntArray intersect;

  bool bOk = Intersect(array, intersect, compareNoCase);
  if (!bOk)
  {
    return bOk;
  }

  for (int32_t i = intersect.size() - 1; i >= 0; i-- )
  {
    this->Erase(intersect.at(i));
  }

  return bOk;
}

//----------------------------------------
bool CStringArray::Complement(const CStringArray& array, CStringArray& complement) const
{
  CStringArray intersect;
  complement.clear();
  
  if (Intersect(array, intersect) == false)
  {
    complement.Insert(array);
    return (!(complement.empty()));
  }


  CStringArray::const_iterator itArray;
  CStringArray::const_iterator itIntersect = intersect.begin();

  for ( itArray = array.begin(); itArray != array.end(); itArray++ )
  {
    bool found = false;
    for ( itIntersect = intersect.begin(); itIntersect != intersect.end(); itIntersect++ )
    {
      if ( (*itArray) == (*itIntersect) )
      {
        found = true;
        break;
      }
    }

    if (!found)
    {
      complement.Insert(*itArray);
    }

  }

  return (!(complement.empty()));
}

//----------------------------------------
void CStringArray::Replace(const string& findString, const string& replaceBy, CStringArray& replaced, bool compareNoCase /* = false */, bool insertUnique /* = false */) const
{ 
  CStringArray findVect;
  findVect.Insert(findString);

  Replace(findVect, replaceBy, replaced, compareNoCase, insertUnique);

}
//----------------------------------------
void CStringArray::Replace(const CStringArray& findString, const string& replaceBy, CStringArray& replaced, bool compareNoCase /* = false */, bool insertUnique /* = false */) const
{
  if (this == &replaced)
  {
    throw CException(CTools::Format("ERROR - CStringArray::Replace : this object (at memory address %p) and parameter 'CStringArray& replaced' (at memory address %p) are same objects. They have to be different.", (void*)this, (void*)(&replaced)), 
                   BRATHL_LOGIC_ERROR);
  }

  replaced.RemoveAll();

  CStringArray findStringTmp(findString);
  
  //bool removed = findStringTmp.Remove(replaceBy, compareNoCase);
  
  if (findStringTmp.size() <= 0)
  {
    replaced.Insert(*this);
    return;
  }

  string thisStr = this->ToString(",", false);
  string findStringStr = findStringTmp.ToString(",", false);

  thisStr = CTools::StringReplace(thisStr, findStringStr, replaceBy, compareNoCase);

  replaced.ExtractStrings(thisStr, ",", true, insertUnique);
/*
  CUIntArray intersectIndex;
  bool bIntersect = replaced.Intersect(findStringTmp, intersectIndex, compareNoCase);
 
  CUIntArray::const_iterator it;

  for (it = intersectIndex.begin() ; it != intersectIndex.end() ; it++)
  {
    if (it == intersectIndex.begin())
    {
      replaced.ReplaceAt(*it,  findStringTmp.at(0));
    }
    else
    {
      replaced.Erase(*it);
    }
  }
  */
//  replaced.Dump(*(CTrace::GetInstance()->GetDumpContext()));

  return;
/*
  CStringArray::const_iterator itThis;
  CStringArray::const_iterator itFindString;

  for (itThis = this->begin() ; itThis != this->end() ; itThis++)
  {
    for (itFindString = findString.begin() ; itFindString != findString.end() ; itFindString++)
    {
      bool same = false;
      
      string str = *itThis;

      if (compareNoCase)
      {
        same = CTools::CompareNoCase(*itThis, *itFindString);
      }
      else
      {
        same = (itThis->compare(*itFindString) == 0);
      }

      if (same)
      {
        str = replaceBy;
      }
      if (insertUnique)
      {
        replaced.InsertUnique(str);
      }
      else
      {
        replaced.Insert(str);
      }
    }
  }
  
  replaced.Dump(*(CTrace::GetInstance()->GetDumpContext()));

  for (itFindString = findString.begin() ; itFindString != findString.end() ; itFindString++)
  {
    int32_t indexFound = replaced.FindIndex(*itFindString, compareNoCase);
    if (indexFound >= 0)
    {
      replaced.Erase(replaced.begin() + indexFound);
    }
  }

  replaced.Dump(*(CTrace::GetInstance()->GetDumpContext()));
*/
}
//----------------------------------------
void CStringArray::ExtractKeys(const string& str, const string& delim, bool bRemoveAll /* = true */) 
{

if (bRemoveAll == true)
  {
  this->clear();
  }

string sequence = str;

int32_t pos = sequence.find(delim, 0);

uint32_t beginSearch = 0;

while (true)
{
  if (pos > 0)
    {
    this->Insert(sequence.substr(0, pos));
    }
  if (pos < 0)
    {
    this->Insert(sequence);
    break;
    }

  beginSearch = pos + 1;
 
  if (beginSearch < sequence.length())
  {
    pos = sequence.find(delim, beginSearch);
  }
  else
  {
    pos = -1;
  }
}

return;
}
//----------------------------------------

void CStringArray::ExtractStrings(const string& str, const char delim, bool bRemoveAll /* = true */ , bool insertUnique /* = false */) 
{

if (bRemoveAll == true)
  {
  this->clear();
  }

string sequence = str;

int32_t pos = sequence.find(delim);

uint32_t nLg;

while (sequence.empty() == false)
  {

  if (pos > 0)
    {
      if (insertUnique)
      {
        this->InsertUnique(sequence.substr(0, pos));
      }
      else
      {
        this->Insert(sequence.substr(0, pos));
      }

    }
  if (pos < 0)
    {
      if (insertUnique)
      {
        this->InsertUnique(sequence);
      }
      else
      {
        this->Insert(sequence);
      }
    sequence = "";
    }

  nLg = (sequence.length() - pos - 1);
  
  if (nLg > 0)
    {
    sequence = sequence.substr(pos + 1, nLg);
    }
  else
    {
    sequence = "";
    }

  pos = sequence.find(delim);
  }

return;
}


//----------------------------------------

void CStringArray::ExtractStrings(const string& str, const string& delim, bool bRemoveAll /* = true */, bool insertUnique /* = false */) 
{

if (bRemoveAll == true)
  {
  this->clear();
  }

string sequence = str;

int32_t pos = sequence.find(delim);

uint32_t nLg;

while (sequence.empty() == false)
  {

  if (pos > 0)
    {
      if (insertUnique)
      {
        this->InsertUnique(sequence.substr(0, pos));
      }
      else
      {
        this->Insert(sequence.substr(0, pos));
      }
    }
  if (pos < 0)
    {
      if (insertUnique)
      {
        this->InsertUnique(sequence);
      }
      else
      {
        this->Insert(sequence);
      }

    sequence = "";
    }

  nLg = (sequence.length() - pos - delim.length());
  
  if (nLg > 0)
    {
    sequence = sequence.substr(pos + 1, nLg);
    }
  else
    {
    sequence = "";
    }

  pos = sequence.find(delim);
  }

return;
}

//----------------------------------------

void CStringArray::InsertUnique(const string& str)
{
  if (this->Exists(str))
  {
    return;
  }

  this->Insert(str);
  
  return;  
}

//----------------------------------------

void CStringArray::InsertUnique(const CStringList& lst)
{
 
   CStringList::const_iterator it;

   for ( it = lst.begin( ); it != lst.end( ); it++ )
   {
     this->InsertUnique(*it);
   }
  
}

//----------------------------------------

void CStringArray::InsertUnique(const CStringArray& vect)
{
 
   CStringArray::const_iterator it;

   for ( it = vect.begin( ); it != vect.end( ); it++ )
   {
     this->InsertUnique(*it);
   }
  
}
//----------------------------------------
void CStringArray::InsertUnique(const CStringArray* vect)
{
  InsertUnique(*vect);
  
}

//----------------------------------------

void CStringArray::InsertUnique(const stringarray& vect)
{
 
   vector<string>::const_iterator it;

   for ( it = vect.begin( ); it != vect.end( ); it++ )
   {
     this->InsertUnique(*it);
   }
  
}

//----------------------------------------

void CStringArray::InsertUnique(const stringlist& lst)
{
 
   list<string>::const_iterator it;

   for ( it = lst.begin( ); it != lst.end( ); it++ )
   {
     this->InsertUnique(*it);
   }
  
}

//----------------------------------------
string CStringArray::ToString(const string& delim /*= "," */, bool useBracket /*= true */) const
{
  ostringstream myStream;

  if (useBracket)
  {
    myStream << "[";
  }


  CStringArray::const_iterator it;
  for ( it = this->begin(); it != this->end(); it++ )
  {
    myStream << *it;  
    if ((it+1) != this->end())
    {
      myStream << delim;  
    }
  }
  if (useBracket)
  {
    myStream << "]";
  }


  return myStream.str();

}




//----------------------------------------
void CStringArray::Dump(ostream& fOut /* = cerr */) const
{

  if (CTrace::IsTrace() == false)
   {
      return;
   }

   CStringArray::const_iterator it;
   int i = 0;

   fOut << "==> Dump a CStringArray Object at "<< this << " with " <<  size() << " elements" << endl;

   for ( it = this->begin( ); it != this->end( ); it++ )
   {
     fOut << "CStringArray[" << i << "]= " << *it << endl;  
     i++;
   }
    
   fOut << "==> END Dump a CStringArray Object at "<< this << " with " <<  size() << " elements" << endl;

 
   fOut << endl;
   

}



//-------------------------------------------------------------
//------------------- CIntArray class --------------------
//-------------------------------------------------------------

CIntArray::CIntArray()
{
  
}

//----------------------------------------

CIntArray::CIntArray(const CIntArray& vect)
      	  : int32array(vect)
{
  
}

//----------------------------------------

CIntArray::~CIntArray()
{

}
//----------------------------------------
bool CIntArray::operator ==(const CIntArray& vect)
{
  uint32_t count = this->size();

  if (count != vect.size())
  {
    return false;
  }

  if (count == 0)
  {
    return true;
  }

  bool isEqual = true;

  for (uint32_t i = 0 ; i < count ; i++)
  {
    if (this->at(i) != vect.at(i))
    {
      isEqual = false;
      break;
    }
  }

  return isEqual;    
}
//----------------------------------------
const CIntArray& CIntArray::operator =(const CIntArray& vect)
{
  
  this->clear();

  Insert(vect);
    
  return *this;

    
}
//----------------------------------------
void CIntArray::Insert(const CIntArray& vect, bool bEnd /*= true*/)
{
  
  if (vect.empty())
  {
    return;
  }

  if (bEnd)
  {  
    int32array::insert(this->end(), vect.begin(), vect.end());
  }
  else
  {  
    int32array::insert(this->begin(), vect.begin(), vect.end());
  }
    
  
}
//----------------------------------------
void CIntArray::Insert(const CStringArray& vect)
{
  
  CStringArray::const_iterator it;

  for ( it = vect.begin( ); it != vect.end( ); it++ )
  {
    int32_t value = CTools::StrToInt(*it);
    if (!(CTools::IsDefaultValue(value)))
    {
      this->Insert(value);
    }
  }
    
  
}

//----------------------------------------
void CIntArray::Insert(int32_t* vect, size_t length)
{
  for (size_t i = 0 ; i < length ; i++)
  {
    Insert(vect[i]);
  }
}

//----------------------------------------

void CIntArray::Insert(const int32_t value)
{
  
int32array::push_back(value);    
  
}

//----------------------------------------
CIntArray::iterator CIntArray::InsertAt(CIntArray::iterator where, const int32_t value)
{ 
  return int32array::insert(where, value);     
}
//----------------------------------------
CIntArray::iterator CIntArray::InsertAt(int32_t index, const int32_t value)
{ 
  if (index < 0)
  {
    return this->end();
  }

  if ((uint32_t)index >= this->size())
  {
    this->resize(index + 1);
    (*this)[index] = value;
    return this->begin() + index;
  }

  return int32array::insert(this->begin() + index, value);     
}


//----------------------------------------
bool CIntArray::Erase(CIntArray::iterator where)
{ 
  int32array::erase(where);   
  return true;
}
//----------------------------------------
CIntArray::iterator CIntArray::ReplaceAt(int32_t index, const int32_t value)
{
  if (index < 0)
  {
    return this->end();
  }

  (*this)[index] = value;
  return this->begin() + index;

}
//----------------------------------------
CIntArray::iterator CIntArray::ReplaceAt(CIntArray::iterator where, const int32_t value)
{
  Erase(where);
  return InsertAt(where, value);     
}

//----------------------------------------
bool CIntArray::Intersect(const CIntArray& array, CIntArray& intersect) const
{
  intersect.clear();

  CIntArray::const_iterator itA;
  CIntArray::const_iterator itB;

  for ( itA = this->begin( ); itA != this->end( ); itA++ )
  {
    for ( itB = array.begin( ); itB != array.end( ); itB++ )
    {
      if ( (*itA) == (*itB) )
      {
        intersect.Insert((*itB));
      }
    }
  }

  return (!(intersect.empty()));
}

//----------------------------------------
void CIntArray::IncrementValue(uint32_t incr /* = 1 */)
{
  CIntArray::iterator it;
 
  for (it = this->begin() ; it != this->end() ; it++)
  {
    (*it) = (*it) + incr;     
  }

  
}

//----------------------------------------
string CIntArray::ToString(const string& delim /*= "," */, bool useBracket /*= true */) const
{
  ostringstream myStream;

  if (useBracket)
  {
    myStream << "[";
  }


  CIntArray::const_iterator it;
  for ( it = this->begin(); it != this->end(); it++ )
  {
    myStream << *it;  
    if ((it+1) != this->end())
    {
      myStream << delim;  
    }
  }
  if (useBracket)
  {
    myStream << "]";
  }


  return myStream.str();

}
//----------------------------------------
int32_t* CIntArray::ToArray()
{

  int32_t* newArray = new int32_t[this->size()];

  for (uint32_t i = 0 ; i < this->size() ; i++ )
  {
    newArray[i] = this->at(i);
  }


  return newArray;

}

//----------------------------------------
void CIntArray::Dump(ostream& fOut /* = cerr */) const
{

  if (CTrace::IsTrace() == false)
   {
      return;
   }

   CIntArray::const_iterator it;
   int i = 0;

   fOut << "==> Dump a CIntArray Object at "<< this << " with " <<  size() << " elements" << endl;

   for ( it = this->begin( ); it != this->end( ); it++ )
   {
     fOut << "CIntArray[" << i << "]= " << *it << endl;  
     i++;
   }
    
   fOut << "==> END Dump a CIntArray Object at "<< this << " with " <<  size() << " elements" << endl;

 
   fOut << endl;
   

}


//-------------------------------------------------------------
//------------------- CInt64Array class --------------------
//-------------------------------------------------------------

CInt64Array::CInt64Array()
{
  
}

//----------------------------------------

CInt64Array::CInt64Array(const CInt64Array& vect)
      	  : int64array(vect)
{
  
}

//----------------------------------------

CInt64Array::~CInt64Array()
{

}
//----------------------------------------
bool CInt64Array::operator ==(const CInt64Array& vect)
{
  size_t count = this->size();

  if (count != vect.size())
  {
    return false;
  }

  if (count == 0)
  {
    return true;
  }

  bool isEqual = true;

  for (size_t i = 0 ; i < count ; i++)
  {
    if (this->at(i) != vect.at(i))
    {
      isEqual = false;
      break;
    }
  }

  return isEqual;    
}
//----------------------------------------
const CInt64Array& CInt64Array::operator =(const CInt64Array& vect)
{
  
  this->clear();

  Insert(vect);
    
  return *this;

    
}
//----------------------------------------
void CInt64Array::Insert(const CInt64Array& vect, bool bEnd /*= true*/)
{
  
  if (vect.empty())
  {
    return;
  }

  if (bEnd)
  {  
    int64array::insert(this->end(), vect.begin(), vect.end());
  }
  else
  {  
    int64array::insert(this->begin(), vect.begin(), vect.end());
  }
    
  
}
//----------------------------------------
void CInt64Array::Insert(const CStringArray& vect)
{
  
  CStringArray::const_iterator it;

  for ( it = vect.begin( ); it != vect.end( ); it++ )
  {
    int64_t value = CTools::StrToInt64(*it);
    if (!(CTools::IsDefaultValue(value)))
    {
      this->Insert(value);
    }
  }
    
  
}

//----------------------------------------
void CInt64Array::Insert(int64_t* vect, size_t length)
{
  for (size_t i = 0 ; i < length ; i++)
  {
    Insert(vect[i]);
  }
}

//----------------------------------------

void CInt64Array::Insert(const int64_t value)
{
  
int64array::push_back(value);    
  
}

//----------------------------------------
CInt64Array::iterator CInt64Array::InsertAt(CInt64Array::iterator where, const int64_t value)
{ 
  return int64array::insert(where, value);     
}
//----------------------------------------
CInt64Array::iterator CInt64Array::InsertAt(size_t index, const int64_t value)
{ 
  if (index < 0)
  {
    return this->end();
  }

  if (index >= this->size())
  {
    this->resize(index + 1);
    (*this)[index] = value;
    return this->begin() + index;
  }

  return int64array::insert(this->begin() + index, value);     
}


//----------------------------------------
bool CInt64Array::Erase(CInt64Array::iterator where)
{ 
  int64array::erase(where);   
  return true;
}
//----------------------------------------
CInt64Array::iterator CInt64Array::ReplaceAt(size_t index, const int64_t value)
{
  (*this)[index] = value;
  return this->begin() + index;
}
//----------------------------------------
CInt64Array::iterator CInt64Array::ReplaceAt(CInt64Array::iterator where, const int64_t value)
{
  Erase(where);
  return InsertAt(where, value);     
}

//----------------------------------------
bool CInt64Array::Intersect(const CInt64Array& array, CInt64Array& intersect) const
{
  intersect.clear();

  CInt64Array::const_iterator itA;
  CInt64Array::const_iterator itB;

  for ( itA = this->begin( ); itA != this->end( ); itA++ )
  {
    for ( itB = array.begin( ); itB != array.end( ); itB++ )
    {
      if ( (*itA) == (*itB) )
      {
        intersect.Insert((*itB));
      }
    }
  }

  return (!(intersect.empty()));
}

//----------------------------------------
void CInt64Array::IncrementValue(uint64_t incr /* = 1 */)
{
  CInt64Array::iterator it;
 
  for (it = this->begin() ; it != this->end() ; it++)
  {
    (*it) = (*it) + incr;     
  }

  
}

//----------------------------------------
string CInt64Array::ToString(const string& delim /*= "," */, bool useBracket /*= true */) const
{
  ostringstream myStream;

  if (useBracket)
  {
    myStream << "[";
  }


  CInt64Array::const_iterator it;
  for ( it = this->begin(); it != this->end(); it++ )
  {
    myStream << *it;  
    if ((it+1) != this->end())
    {
      myStream << delim;  
    }
  }
  if (useBracket)
  {
    myStream << "]";
  }


  return myStream.str();

}
//----------------------------------------
int64_t* CInt64Array::ToArray()
{

  int64_t* newArray = new int64_t[this->size()];

  for (size_t i = 0 ; i < this->size() ; i++ )
  {
    newArray[i] = this->at(i);
  }


  return newArray;

}

//----------------------------------------
void CInt64Array::Dump(ostream& fOut /* = cerr */) const
{

  if (CTrace::IsTrace() == false)
   {
      return;
   }

   CInt64Array::const_iterator it;
   int i = 0;

   fOut << "==> Dump a CInt64Array Object at "<< this << " with " <<  size() << " elements" << endl;

   for ( it = this->begin( ); it != this->end( ); it++ )
   {
     fOut << "CInt64Array[" << i << "]= " << *it << endl;  
     i++;
   }
    
   fOut << "==> END Dump a CInt64Array Object at "<< this << " with " <<  size() << " elements" << endl;

 
   fOut << endl;
   

}


//-------------------------------------------------------------
//------------------- CUIntArray class --------------------
//-------------------------------------------------------------

CUIntArray::CUIntArray()
{
  
}

//----------------------------------------

CUIntArray::CUIntArray(const CUIntArray& vect)
      	  : uint32array(vect)
{
  
}

//----------------------------------------

CUIntArray::~CUIntArray()
{

}
//----------------------------------------
bool CUIntArray::operator ==(const CUIntArray& vect)
{
  uint32_t count = this->size();

  if (count != vect.size())
  {
    return false;
  }

  if (count == 0)
  {
    return true;
  }

  bool isEqual = true;

  for (uint32_t i = 0 ; i < count ; i++)
  {
    if (this->at(i) != vect.at(i))
    {
      isEqual = false;
      break;
    }
  }

  return isEqual;    
}

//----------------------------------------

const CUIntArray& CUIntArray::operator =(const CUIntArray& vect)
{
  
  this->clear();

  Insert(vect);
    
  return *this;

    
}
//----------------------------------------

void CUIntArray::Insert(CUIntArray* vect, bool bEnd /*= true*/)
{
  Insert(*vect); 
}

//----------------------------------------

void CUIntArray::Insert(const CUIntArray& vect, bool bEnd /*= true*/)
{
  
  if (vect.empty())
  {
    return;
  }

  if (bEnd)
  {  
    uint32array::insert(this->end(), vect.begin(), vect.end());
  }
  else
  {  
    uint32array::insert(this->begin(), vect.begin(), vect.end());
  }
    
  
}
//----------------------------------------
void CUIntArray::Insert(const vector<uint32_t>& vect, bool bEnd /*= true*/)
{
  
  if (vect.empty())
  {
    return;
  }

  if (bEnd)
  {  
    uint32array::insert(this->end(), vect.begin(), vect.end());
  }
  else
  {  
    uint32array::insert(this->begin(), vect.begin(), vect.end());
  }
    
  
}
//----------------------------------------
void CUIntArray::Insert(uint32_t* vect, size_t length)
{
  for (size_t i = 0 ; i < length ; i++)
  {
    Insert(vect[i]);
  }
}

//----------------------------------------
void CUIntArray::Insert(const uint32_t value)
{
  
uint32array::push_back(value);    
  
}

//----------------------------------------
CUIntArray::iterator CUIntArray::InsertAt(CUIntArray::iterator where, const uint32_t value)
{ 
  return uint32array::insert(where, value);     
}
//----------------------------------------
CUIntArray::iterator CUIntArray::InsertAt(int32_t index, const uint32_t value)
{ 
  if (index < 0)
  {
    return this->end();
  }

  if ((uint32_t)index >= this->size())
  {
    this->resize(index + 1);
    (*this)[index] = value;
    return this->begin() + index;
  }

  return uint32array::insert(this->begin() + index, value);     
}

//----------------------------------------
bool CUIntArray::Erase(CUIntArray::iterator where)
{ 
  uint32array::erase(where);   
  return true;
}
//----------------------------------------
CUIntArray::iterator CUIntArray::ReplaceAt(int32_t index, const uint32_t value)
{
  if (index < 0)
  {
    return this->end();
  }

  (*this)[index] = value;
  return this->begin() + index;

}
//----------------------------------------
CUIntArray::iterator CUIntArray::ReplaceAt(CUIntArray::iterator where, const uint32_t value)
{
  Erase(where);
  return InsertAt(where, value);     
}

//----------------------------------------
bool CUIntArray::Intersect(const CUIntArray& array, CUIntArray& intersect) const
{
  intersect.clear();

  CUIntArray::const_iterator itA;
  CUIntArray::const_iterator itB;

  for ( itA = this->begin( ); itA != this->end( ); itA++ )
  {
    for ( itB = array.begin( ); itB != array.end( ); itB++ )
    {
      if ( (*itA) == (*itB) )
      {
        intersect.Insert((*itB));
      }
    }
  }

  return (!(intersect.empty()));
}

//----------------------------------------
bool CUIntArray::Complement(const CUIntArray& array, CUIntArray& complement) const
{
  CUIntArray intersect;
  complement.clear();
  
  if (Intersect(array, intersect) == false)
  {
    complement.Insert(array);
    return (!(complement.empty()));
  }


  CUIntArray::const_iterator itArray;
  CUIntArray::const_iterator itIntersect= intersect.begin();

  for ( itArray = array.begin(); itArray != array.end(); itArray++ )
  {
    bool found = false;
    for ( itIntersect = intersect.begin(); itIntersect != intersect.end(); itIntersect++ )
    {
      if ( (*itArray) == (*itIntersect) )
      {
        found = true;
        break;
      }
    }

    if (!found)
    {
      complement.Insert(*itArray);
    }

  }

  return (!(complement.empty()));
}


//----------------------------------------
string CUIntArray::ToString(const string& delim /*= "," */, bool useBracket /*= true */) const
{
  ostringstream myStream;

  if (useBracket)
  {
    myStream << "[";
  }

  CUIntArray::const_iterator it;
  for ( it = this->begin(); it != this->end(); it++ )
  {
    myStream << *it;  
    if ((it+1) != this->end())
    {
      myStream << delim;  
    }
  }
  if (useBracket)
  {
    myStream << "]";
  }


  return myStream.str();

}
//----------------------------------------
uint32_t* CUIntArray::ToArray()
{

  uint32_t* newArray = new uint32_t[this->size()];

  for (uint32_t i = 0 ; i < this->size() ; i++ )
  {
    newArray[i] = this->at(i);
  }


  return newArray;

}
//----------------------------------------
int32_t* CUIntArray::ToIntArray()
{

  int32_t* newArray = new int32_t[this->size()];

  for (uint32_t i = 0 ; i < this->size() ; i++ )
  {
    newArray[i] = static_cast<int32_t>(this->at(i));
  }


  return newArray;

}
//----------------------------------------
size_t* CUIntArray::ToSizeTArray()
{

  size_t* newArray = new size_t[this->size()];

  for (uint32_t i = 0 ; i < this->size() ; i++ )
  {
    newArray[i] = static_cast<size_t>(this->at(i));
  }


  return newArray;

}
//----------------------------------------
uint32_t CUIntArray::GetProductValues() const
{
  uint32_t product = 1;
  
  if (this->size() <= 0)
  {
    product = 0;
  }

  CUIntArray::const_iterator it;
  for ( it = this->begin(); it != this->end(); it++ )
  {
    product	*= *it;
  }
  
  return product;
}

//----------------------------------------
void CUIntArray::Dump(ostream& fOut /* = cerr */) const
{

  if (CTrace::IsTrace() == false)
   {
      return;
   }

   CUIntArray::const_iterator it;
   int i = 0;

   fOut << "==> Dump a CUIntArray Object at "<< this << " with " <<  size() << " elements" << endl;

   for ( it = this->begin( ); it != this->end( ); it++ )
   {
     fOut << "CUIntArray[" << i << "]= " << *it << endl;  
     i++;
   }
    
   fOut << "==> END Dump a CUIntArray Object at "<< this << " with " <<  size() << " elements" << endl;

 
   fOut << endl;
   

}

//-------------------------------------------------------------
//------------------- CUInt64Array class --------------------
//-------------------------------------------------------------

CUInt64Array::CUInt64Array()
{
  
}

//----------------------------------------

CUInt64Array::CUInt64Array(const CUInt64Array& vect)
      	  : uint64array(vect)
{
  
}

//----------------------------------------

CUInt64Array::~CUInt64Array()
{

}
//----------------------------------------
bool CUInt64Array::operator ==(const CUInt64Array& vect)
{
  size_t count = this->size();

  if (count != vect.size())
  {
    return false;
  }

  if (count == 0)
  {
    return true;
  }

  bool isEqual = true;

  for (size_t i = 0 ; i < count ; i++)
  {
    if (this->at(i) != vect.at(i))
    {
      isEqual = false;
      break;
    }
  }

  return isEqual;    
}
//----------------------------------------
const CUInt64Array& CUInt64Array::operator =(const CUInt64Array& vect)
{
  
  this->clear();

  Insert(vect);
    
  return *this;

    
}
//----------------------------------------
void CUInt64Array::Insert(const CUInt64Array& vect, bool bEnd /*= true*/)
{
  
  if (vect.empty())
  {
    return;
  }

  if (bEnd)
  {  
    uint64array::insert(this->end(), vect.begin(), vect.end());
  }
  else
  {  
    uint64array::insert(this->begin(), vect.begin(), vect.end());
  }
    
  
}
//----------------------------------------

void CUInt64Array::Insert(CUInt64Array* vect, bool bEnd /*= true*/)
{
  Insert(*vect); 
}

//----------------------------------------
void CUInt64Array::Insert(const vector<uint64_t>& vect, bool bEnd /*= true*/)
{
  
  if (vect.empty())
  {
    return;
  }

  if (bEnd)
  {  
    uint64array::insert(this->end(), vect.begin(), vect.end());
  }
  else
  {  
    uint64array::insert(this->begin(), vect.begin(), vect.end());
  }
    
  
}
//----------------------------------------
void CUInt64Array::Insert(uint64_t* vect, size_t length)
{
  for (size_t i = 0 ; i < length ; i++)
  {
    Insert(vect[i]);
  }
}

//----------------------------------------
void CUInt64Array::Insert(const uint64_t value)
{
  
uint64array::push_back(value);    
  
}

//----------------------------------------
CUInt64Array::iterator CUInt64Array::InsertAt(CUInt64Array::iterator where, const uint64_t value)
{ 
  return uint64array::insert(where, value);     
}
//----------------------------------------
CUInt64Array::iterator CUInt64Array::InsertAt(size_t index, const uint64_t value)
{ 
  if (index < 0)
  {
    return this->end();
  }

  if (index >= this->size())
  {
    this->resize(index + 1);
    (*this)[index] = value;
    return this->begin() + index;
  }

  return uint64array::insert(this->begin() + index, value);     
}


//----------------------------------------
bool CUInt64Array::Erase(CUInt64Array::iterator where)
{ 
  uint64array::erase(where);   
  return true;
}
//----------------------------------------
CUInt64Array::iterator CUInt64Array::ReplaceAt(size_t index, const uint64_t value)
{
  (*this)[index] = value;
  return this->begin() + index;
}
//----------------------------------------
CUInt64Array::iterator CUInt64Array::ReplaceAt(CUInt64Array::iterator where, const uint64_t value)
{
  Erase(where);
  return InsertAt(where, value);     
}

//----------------------------------------
bool CUInt64Array::Intersect(const CUInt64Array& array, CUInt64Array& intersect) const
{
  intersect.clear();

  CUInt64Array::const_iterator itA;
  CUInt64Array::const_iterator itB;

  for ( itA = this->begin( ); itA != this->end( ); itA++ )
  {
    for ( itB = array.begin( ); itB != array.end( ); itB++ )
    {
      if ( (*itA) == (*itB) )
      {
        intersect.Insert((*itB));
      }
    }
  }

  return (!(intersect.empty()));
}

//----------------------------------------
string CUInt64Array::ToString(const string& delim /*= "," */, bool useBracket /*= true */) const
{
  ostringstream myStream;

  if (useBracket)
  {
    myStream << "[";
  }


  CUInt64Array::const_iterator it;
  for ( it = this->begin(); it != this->end(); it++ )
  {
    myStream << *it;  
    if ((it+1) != this->end())
    {
      myStream << delim;  
    }
  }
  if (useBracket)
  {
    myStream << "]";
  }


  return myStream.str();

}
//----------------------------------------
uint64_t* CUInt64Array::ToArray()
{

  uint64_t* newArray = new uint64_t[this->size()];

  for (size_t i = 0 ; i < this->size() ; i++ )
  {
    newArray[i] = this->at(i);
  }


  return newArray;

}

//----------------------------------------
void CUInt64Array::Dump(ostream& fOut /* = cerr */) const
{

  if (CTrace::IsTrace() == false)
   {
      return;
   }

   CUInt64Array::const_iterator it;
   int i = 0;

   fOut << "==> Dump a CUInt64Array Object at "<< this << " with " <<  size() << " elements" << endl;

   for ( it = this->begin( ); it != this->end( ); it++ )
   {
     fOut << "CUInt64Array[" << i << "]= " << *it << endl;  
     i++;
   }
    
   fOut << "==> END Dump a CUInt64Array Object at "<< this << " with " <<  size() << " elements" << endl;

 
   fOut << endl;
   

}


//-------------------------------------------------------------
//------------------- CInt16Array class --------------------
//-------------------------------------------------------------

CInt16Array::CInt16Array()
{
  
}

//----------------------------------------

CInt16Array::CInt16Array(const CInt16Array& vect)
      	  : int16array(vect)
{
  
}

//----------------------------------------

CInt16Array::~CInt16Array()
{

}
//----------------------------------------


const CInt16Array& CInt16Array::operator =(const CInt16Array& vect)
{
  
  this->clear();

  Insert(vect);
    
  return *this;

    
}
//----------------------------------------
void CInt16Array::Insert(const CInt16Array& vect, bool bEnd /*= true*/)
{
  
  if (vect.empty())
  {
    return;
  }

  if (bEnd)
  {  
    int16array::insert(this->end(), vect.begin(), vect.end());
  }
  else
  {  
    int16array::insert(this->begin(), vect.begin(), vect.end());
  }
    
  
}
//----------------------------------------
void CInt16Array::Insert(const CStringArray& vect)
{
  
  CStringArray::const_iterator it;

  for ( it = vect.begin( ); it != vect.end( ); it++ )
  {
    int16_t value = CTools::StrToInt(*it);
    if (!(CTools::IsDefaultValue(value)))
    {
      this->Insert(value);
    }
  }
    
  
}
//----------------------------------------
void CInt16Array::Insert(int16_t* vect, size_t length)
{
  for (size_t i = 0 ; i < length ; i++)
  {
    Insert(vect[i]);
  }
}
//----------------------------------------

void CInt16Array::Insert(const int16_t value)
{
  
int16array::push_back(value);    
  
}

//----------------------------------------
CInt16Array::iterator CInt16Array::InsertAt(CInt16Array::iterator where, const int16_t value)
{ 
  return int16array::insert(where, value);     
}
//----------------------------------------
CInt16Array::iterator CInt16Array::InsertAt(int32_t index, const int16_t value)
{ 
  if (index < 0)
  {
    return this->end();
  }

  if ((uint32_t)index >= this->size())
  {
    this->resize(index + 1);
    (*this)[index] = value;
    return this->begin() + index;
  }

  return int16array::insert(this->begin() + index, value);     
}

//----------------------------------------
bool CInt16Array::Erase(CInt16Array::iterator where)
{ 
  int16array::erase(where);   
  return true;
}
//----------------------------------------
CInt16Array::iterator CInt16Array::ReplaceAt(int32_t index, const int16_t value)
{
  if (index < 0)
  {
    return this->end();
  }

  (*this)[index] = value;
  return this->begin() + index;

}
//----------------------------------------
CInt16Array::iterator CInt16Array::ReplaceAt(CInt16Array::iterator where, const int16_t value)
{
  Erase(where);
  return InsertAt(where, value);     
}

//----------------------------------------
bool CInt16Array::Intersect(const CInt16Array& array, CInt16Array& intersect) const
{
  intersect.clear();

  CInt16Array::const_iterator itA;
  CInt16Array::const_iterator itB;

  for ( itA = this->begin( ); itA != this->end( ); itA++ )
  {
    for ( itB = array.begin( ); itB != array.end( ); itB++ )
    {
      if ( (*itA) == (*itB) )
      {
        intersect.Insert((*itB));
      }
    }
  }

  return (!(intersect.empty()));
}


//----------------------------------------
string CInt16Array::ToString(const string& delim /*= "," */, bool useBracket /*= true */) const
{
  ostringstream myStream;

  if (useBracket)
  {
    myStream << "[";
  }


  CInt16Array::const_iterator it;
  for ( it = this->begin(); it != this->end(); it++ )
  {
    myStream << *it;  
    if ((it+1) != this->end())
    {
      myStream << delim;  
    }
  }
  if (useBracket)
  {
    myStream << "]";
  }


  return myStream.str();

}
//----------------------------------------
int16_t* CInt16Array::ToArray()
{

  int16_t* newArray = new int16_t[this->size()];

  for (uint16_t i = 0 ; i < this->size() ; i++ )
  {
    newArray[i] = this->at(i);
  }


  return newArray;

}

//----------------------------------------
void CInt16Array::Dump(ostream& fOut /* = cerr */) const
{

  if (CTrace::IsTrace() == false)
   {
      return;
   }

   CInt16Array::const_iterator it;
   int i = 0;

   fOut << "==> Dump a CInt16Array Object at "<< this << " with " <<  size() << " elements" << endl;

   for ( it = this->begin( ); it != this->end( ); it++ )
   {
     fOut << "CInt16Array[" << i << "]= " << *it << endl;  
     i++;
   }
    
   fOut << "==> END Dump a CInt16Array Object at "<< this << " with " <<  size() << " elements" << endl;

 
   fOut << endl;
   

}


//-------------------------------------------------------------
//------------------- CUInt16Array class --------------------
//-------------------------------------------------------------

CUInt16Array::CUInt16Array()
{
  
}

//----------------------------------------

CUInt16Array::CUInt16Array(const CUInt16Array& vect)
      	  : uint16array(vect)
{
  
}

//----------------------------------------

CUInt16Array::~CUInt16Array()
{

}
//----------------------------------------
bool CUInt16Array::operator ==(const CUInt16Array& vect)
{
  uint16_t count = this->size();

  if (count != vect.size())
  {
    return false;
  }

  if (count == 0)
  {
    return true;
  }

  bool isEqual = true;

  for (uint16_t i = 0 ; i < count ; i++)
  {
    if (this->at(i) != vect.at(i))
    {
      isEqual = false;
      break;
    }
  }

  return isEqual;    
}

//----------------------------------------

const CUInt16Array& CUInt16Array::operator =(const CUInt16Array& vect)
{
  
  this->clear();

  Insert(vect);
    
  return *this;

    
}
//----------------------------------------

void CUInt16Array::Insert(CUInt16Array* vect, bool bEnd /*= true*/)
{
  Insert(*vect); 
}

//----------------------------------------

void CUInt16Array::Insert(const CUInt16Array& vect, bool bEnd /*= true*/)
{
  
  if (vect.empty())
  {
    return;
  }

  if (bEnd)
  {  
    uint16array::insert(this->end(), vect.begin(), vect.end());
  }
  else
  {  
    uint16array::insert(this->begin(), vect.begin(), vect.end());
  }
    
  
}
//----------------------------------------
void CUInt16Array::Insert(const vector<uint16_t>& vect, bool bEnd /*= true*/)
{
  
  if (vect.empty())
  {
    return;
  }

  if (bEnd)
  {  
    uint16array::insert(this->end(), vect.begin(), vect.end());
  }
  else
  {  
    uint16array::insert(this->begin(), vect.begin(), vect.end());
  }
    
  
}
//----------------------------------------
void CUInt16Array::Insert(uint16_t* vect, size_t length)
{
  for (size_t i = 0 ; i < length ; i++)
  {
    Insert(vect[i]);
  }
}
//----------------------------------------

void CUInt16Array::Insert(const uint16_t value)
{
  
uint16array::push_back(value);    
  
}

//----------------------------------------
CUInt16Array::iterator CUInt16Array::InsertAt(CUInt16Array::iterator where, const uint16_t value)
{ 
  return uint16array::insert(where, value);     
}
//----------------------------------------
CUInt16Array::iterator CUInt16Array::InsertAt(int32_t index, const uint16_t value)
{ 
  if (index < 0)
  {
    return this->end();
  }

  if ((uint32_t)index >= this->size())
  {
    this->resize(index + 1);
    (*this)[index] = value;
    return this->begin() + index;
  }

  return uint16array::insert(this->begin() + index, value);     
}

//----------------------------------------
bool CUInt16Array::Erase(CUInt16Array::iterator where)
{ 
  uint16array::erase(where);   
  return true;
}
//----------------------------------------
CUInt16Array::iterator CUInt16Array::ReplaceAt(int32_t index, const uint16_t value)
{
  if (index < 0)
  {
    return this->end();
  }

  (*this)[index] = value;
  return this->begin() + index;

}
//----------------------------------------
CUInt16Array::iterator CUInt16Array::ReplaceAt(CUInt16Array::iterator where, const uint16_t value)
{
  Erase(where);
  return InsertAt(where, value);     
}

//----------------------------------------
bool CUInt16Array::Intersect(const CUInt16Array& array, CUInt16Array& intersect) const
{
  intersect.clear();

  CUInt16Array::const_iterator itA;
  CUInt16Array::const_iterator itB;

  for ( itA = this->begin( ); itA != this->end( ); itA++ )
  {
    for ( itB = array.begin( ); itB != array.end( ); itB++ )
    {
      if ( (*itA) == (*itB) )
      {
        intersect.Insert((*itB));
      }
    }
  }

  return (!(intersect.empty()));
}

//----------------------------------------
bool CUInt16Array::Complement(const CUInt16Array& array, CUInt16Array& complement) const
{
  CUInt16Array intersect;
  complement.clear();
  
  if (Intersect(array, intersect) == false)
  {
    complement.Insert(array);
    return (!(complement.empty()));
  }


  CUInt16Array::const_iterator itArray;
  CUInt16Array::const_iterator itIntersect= intersect.begin();

  for ( itArray = array.begin(); itArray != array.end(); itArray++ )
  {
    bool found = false;
    for ( itIntersect = intersect.begin(); itIntersect != intersect.end(); itIntersect++ )
    {
      if ( (*itArray) == (*itIntersect) )
      {
        found = true;
        break;
      }
    }

    if (!found)
    {
      complement.Insert(*itArray);
    }

  }

  return (!(complement.empty()));
}


//----------------------------------------
string CUInt16Array::ToString(const string& delim /*= "," */, bool useBracket /*= true */) const
{
  ostringstream myStream;

  if (useBracket)
  {
    myStream << "[";
  }

  CUInt16Array::const_iterator it;
  for ( it = this->begin(); it != this->end(); it++ )
  {
    myStream << *it;  
    if ((it+1) != this->end())
    {
      myStream << delim;  
    }
  }
  if (useBracket)
  {
    myStream << "]";
  }


  return myStream.str();

}
//----------------------------------------
uint16_t* CUInt16Array::ToArray()
{

  uint16_t* newArray = new uint16_t[this->size()];

  for (uint16_t i = 0 ; i < this->size() ; i++ )
  {
    newArray[i] = this->at(i);
  }


  return newArray;

}
//----------------------------------------
int16_t* CUInt16Array::ToIntArray()
{

  int16_t* newArray = new int16_t[this->size()];

  for (uint16_t i = 0 ; i < this->size() ; i++ )
  {
    newArray[i] = static_cast<int16_t>(this->at(i));
  }


  return newArray;

}
//----------------------------------------
void CUInt16Array::Dump(ostream& fOut /* = cerr */) const
{

  if (CTrace::IsTrace() == false)
   {
      return;
   }

   CUInt16Array::const_iterator it;
   int i = 0;

   fOut << "==> Dump a CUInt16Array Object at "<< this << " with " <<  size() << " elements" << endl;

   for ( it = this->begin( ); it != this->end( ); it++ )
   {
     fOut << "CUInt16Array[" << i << "]= " << *it << endl;  
     i++;
   }
    
   fOut << "==> END Dump a CUInt16Array Object at "<< this << " with " <<  size() << " elements" << endl;

 
   fOut << endl;
   

}

//-------------------------------------------------------------
//------------------- CInt8Array class --------------------
//-------------------------------------------------------------

CInt8Array::CInt8Array()
{
  
}

//----------------------------------------

CInt8Array::CInt8Array(const CInt8Array& vect)
      	  : int8array(vect)
{
  
}

//----------------------------------------

CInt8Array::~CInt8Array()
{

}
//----------------------------------------


const CInt8Array& CInt8Array::operator =(const CInt8Array& vect)
{
  
  this->clear();

  Insert(vect);
    
  return *this;

    
}
//----------------------------------------
void CInt8Array::Insert(const CInt8Array& vect, bool bEnd /*= true*/)
{
  
  if (vect.empty())
  {
    return;
  }

  if (bEnd)
  {  
    int8array::insert(this->end(), vect.begin(), vect.end());
  }
  else
  {  
    int8array::insert(this->begin(), vect.begin(), vect.end());
  }
    
  
}
//----------------------------------------
void CInt8Array::Insert(const CStringArray& vect)
{
  
  CStringArray::const_iterator it;

  for ( it = vect.begin( ); it != vect.end( ); it++ )
  {
    int8_t value = CTools::StrToInt(*it);
    if (!(CTools::IsDefaultValue(value)))
    {
      this->Insert(value);
    }
  }
    
  
}
//----------------------------------------
void CInt8Array::Insert(int8_t* vect, size_t length)
{
  for (size_t i = 0 ; i < length ; i++)
  {
    Insert(vect[i]);
  }
}

//----------------------------------------

void CInt8Array::Insert(const int8_t value)
{
  
int8array::push_back(value);    
  
}

//----------------------------------------
CInt8Array::iterator CInt8Array::InsertAt(CInt8Array::iterator where, const int8_t value)
{ 
  return int8array::insert(where, value);     
}
//----------------------------------------
CInt8Array::iterator CInt8Array::InsertAt(int32_t index, const int8_t value)
{ 
  if (index < 0)
  {
    return this->end();
  }

  if ((uint32_t)index >= this->size())
  {
    this->resize(index + 1);
    (*this)[index] = value;
    return this->begin() + index;
  }

  return int8array::insert(this->begin() + index, value);     
}

//----------------------------------------
bool CInt8Array::Erase(CInt8Array::iterator where)
{ 
  int8array::erase(where);   
  return true;
}
//----------------------------------------
CInt8Array::iterator CInt8Array::ReplaceAt(int32_t index, const int8_t value)
{
  if (index < 0)
  {
    return this->end();
  }

  (*this)[index] = value;
  return this->begin() + index;

}
//----------------------------------------
CInt8Array::iterator CInt8Array::ReplaceAt(CInt8Array::iterator where, const int8_t value)
{
  Erase(where);
  return InsertAt(where, value);     
}

//----------------------------------------
bool CInt8Array::Intersect(const CInt8Array& array, CInt8Array& intersect) const
{
  intersect.clear();

  CInt8Array::const_iterator itA;
  CInt8Array::const_iterator itB;

  for ( itA = this->begin( ); itA != this->end( ); itA++ )
  {
    for ( itB = array.begin( ); itB != array.end( ); itB++ )
    {
      if ( (*itA) == (*itB) )
      {
        intersect.Insert((*itB));
      }
    }
  }

  return (!(intersect.empty()));
}


//----------------------------------------
string CInt8Array::ToString(const string& delim /*= "," */, bool useBracket /*= true */) const
{
  ostringstream myStream;

  if (useBracket)
  {
    myStream << "[";
  }


  CInt8Array::const_iterator it;
  for ( it = this->begin(); it != this->end(); it++ )
  {
    myStream << *it;  
    if ((it+1) != this->end())
    {
      myStream << delim;  
    }
  }
  if (useBracket)
  {
    myStream << "]";
  }


  return myStream.str();

}
//----------------------------------------
int8_t* CInt8Array::ToArray()
{

  int8_t* newArray = new int8_t[this->size()];

  for (uint8_t i = 0 ; i < this->size() ; i++ )
  {
    newArray[i] = this->at(i);
  }


  return newArray;

}

//----------------------------------------
void CInt8Array::Dump(ostream& fOut /* = cerr */) const
{

  if (CTrace::IsTrace() == false)
   {
      return;
   }

   CInt8Array::const_iterator it;
   int i = 0;

   fOut << "==> Dump a CInt8Array Object at "<< this << " with " <<  size() << " elements" << endl;

   for ( it = this->begin( ); it != this->end( ); it++ )
   {
     fOut << "CInt8Array[" << i << "]= " << *it << endl;  
     i++;
   }
    
   fOut << "==> END Dump a CInt8Array Object at "<< this << " with " <<  size() << " elements" << endl;

 
   fOut << endl;
   

}


//-------------------------------------------------------------
//------------------- CUInt8Array class --------------------
//-------------------------------------------------------------

CUInt8Array::CUInt8Array()
{
  
}

//----------------------------------------

CUInt8Array::CUInt8Array(const CUInt8Array& vect)
      	  : uint8array(vect)
{
  
}

//----------------------------------------

CUInt8Array::~CUInt8Array()
{

}
//----------------------------------------
bool CUInt8Array::operator ==(const CUInt8Array& vect)
{
  uint8_t count = this->size();

  if (count != vect.size())
  {
    return false;
  }

  if (count == 0)
  {
    return true;
  }

  bool isEqual = true;

  for (uint8_t i = 0 ; i < count ; i++)
  {
    if (this->at(i) != vect.at(i))
    {
      isEqual = false;
      break;
    }
  }

  return isEqual;    
}

//----------------------------------------

const CUInt8Array& CUInt8Array::operator =(const CUInt8Array& vect)
{
  
  this->clear();

  Insert(vect);
    
  return *this;

    
}
//----------------------------------------

void CUInt8Array::Insert(CUInt8Array* vect, bool bEnd /*= true*/)
{
  Insert(*vect); 
}

//----------------------------------------

void CUInt8Array::Insert(const CUInt8Array& vect, bool bEnd /*= true*/)
{
  
  if (vect.empty())
  {
    return;
  }

  if (bEnd)
  {  
    uint8array::insert(this->end(), vect.begin(), vect.end());
  }
  else
  {  
    uint8array::insert(this->begin(), vect.begin(), vect.end());
  }
    
  
}
//----------------------------------------
void CUInt8Array::Insert(const vector<uint8_t>& vect, bool bEnd /*= true*/)
{
  
  if (vect.empty())
  {
    return;
  }

  if (bEnd)
  {  
    uint8array::insert(this->end(), vect.begin(), vect.end());
  }
  else
  {  
    uint8array::insert(this->begin(), vect.begin(), vect.end());
  }
    
  
}
//----------------------------------------
void CUInt8Array::Insert(uint8_t* vect, size_t length)
{
  for (size_t i = 0 ; i < length ; i++)
  {
    Insert(vect[i]);
  }
}

//----------------------------------------

void CUInt8Array::Insert(const uint8_t value)
{
  
uint8array::push_back(value);    
  
}

//----------------------------------------
CUInt8Array::iterator CUInt8Array::InsertAt(CUInt8Array::iterator where, const uint8_t value)
{ 
  return uint8array::insert(where, value);     
}
//----------------------------------------
CUInt8Array::iterator CUInt8Array::InsertAt(int32_t index, const uint8_t value)
{ 
  if (index < 0)
  {
    return this->end();
  }

  if ((uint32_t)index >= this->size())
  {
    this->resize(index + 1);
    (*this)[index] = value;
    return this->begin() + index;
  }

  return uint8array::insert(this->begin() + index, value);     
}

//----------------------------------------
bool CUInt8Array::Erase(CUInt8Array::iterator where)
{ 
  uint8array::erase(where);   
  return true;
}
//----------------------------------------
CUInt8Array::iterator CUInt8Array::ReplaceAt(int32_t index, const uint8_t value)
{
  if (index < 0)
  {
    return this->end();
  }

  (*this)[index] = value;
  return this->begin() + index;

}
//----------------------------------------
CUInt8Array::iterator CUInt8Array::ReplaceAt(CUInt8Array::iterator where, const uint8_t value)
{
  Erase(where);
  return InsertAt(where, value);     
}

//----------------------------------------
bool CUInt8Array::Intersect(const CUInt8Array& array, CUInt8Array& intersect) const
{
  intersect.clear();

  CUInt8Array::const_iterator itA;
  CUInt8Array::const_iterator itB;

  for ( itA = this->begin( ); itA != this->end( ); itA++ )
  {
    for ( itB = array.begin( ); itB != array.end( ); itB++ )
    {
      if ( (*itA) == (*itB) )
      {
        intersect.Insert((*itB));
      }
    }
  }

  return (!(intersect.empty()));
}

//----------------------------------------
bool CUInt8Array::Complement(const CUInt8Array& array, CUInt8Array& complement) const
{
  CUInt8Array intersect;
  complement.clear();
  
  if (Intersect(array, intersect) == false)
  {
    complement.Insert(array);
    return (!(complement.empty()));
  }


  CUInt8Array::const_iterator itArray;
  CUInt8Array::const_iterator itIntersect= intersect.begin();

  for ( itArray = array.begin(); itArray != array.end(); itArray++ )
  {
    bool found = false;
    for ( itIntersect = intersect.begin(); itIntersect != intersect.end(); itIntersect++ )
    {
      if ( (*itArray) == (*itIntersect) )
      {
        found = true;
        break;
      }
    }

    if (!found)
    {
      complement.Insert(*itArray);
    }

  }

  return (!(complement.empty()));
}


//----------------------------------------
string CUInt8Array::ToString(const string& delim /*= "," */, bool useBracket /*= true */) const
{
  ostringstream myStream;

  if (useBracket)
  {
    myStream << "[";
  }

  CUInt8Array::const_iterator it;
  for ( it = this->begin(); it != this->end(); it++ )
  {
    myStream << *it;  
    if ((it+1) != this->end())
    {
      myStream << delim;  
    }
  }
  if (useBracket)
  {
    myStream << "]";
  }


  return myStream.str();

}
//----------------------------------------
uint8_t* CUInt8Array::ToArray()
{

  uint8_t* newArray = new uint8_t[this->size()];

  for (uint8_t i = 0 ; i < this->size() ; i++ )
  {
    newArray[i] = this->at(i);
  }


  return newArray;

}
//----------------------------------------
int8_t* CUInt8Array::ToIntArray()
{

  int8_t* newArray = new int8_t[this->size()];

  for (uint8_t i = 0 ; i < this->size() ; i++ )
  {
    newArray[i] = static_cast<int8_t>(this->at(i));
  }


  return newArray;

}
//----------------------------------------
void CUInt8Array::Dump(ostream& fOut /* = cerr */) const
{

  if (CTrace::IsTrace() == false)
   {
      return;
   }

   CUInt8Array::const_iterator it;
   int i = 0;

   fOut << "==> Dump a CUInt8Array Object at "<< this << " with " <<  size() << " elements" << endl;

   for ( it = this->begin( ); it != this->end( ); it++ )
   {
     fOut << "CUInt8Array[" << i << "]= " << *it << endl;  
     i++;
   }
    
   fOut << "==> END Dump a CUInt8Array Object at "<< this << " with " <<  size() << " elements" << endl;

 
   fOut << endl;
   

}


//-------------------------------------------------------------
//------------------- CFloatArray class --------------------
//-------------------------------------------------------------

CFloatArray::CFloatArray()
{

}

//----------------------------------------

CFloatArray::CFloatArray(const CFloatArray& vect)
      	  : floatarray(vect)
{
  
}

//----------------------------------------

CFloatArray::~CFloatArray()
{

}
//----------------------------------------

void CFloatArray::RemoveAll()
{
  
  floatarray::clear();

}

//----------------------------------------


const CFloatArray& CFloatArray::operator =(const CFloatArray& vect)
{
  
  this->clear();

  Insert(vect);
    
  return *this;

    
}

//----------------------------------------
void CFloatArray::Insert(float* data, int32_t size)
{
  if (size <= 0)
  {
    return;
  }

  for (int32_t i = 0 ; i < size ; i++)
  {
    this->Insert(data[i]);
  }
}
//----------------------------------------
void CFloatArray::Insert(const CFloatArray& vect, bool bEnd /*= true*/)
{
  if (vect.empty())
  {
    return;
  }

  if (bEnd)
  {  
    floatarray::insert(this->end(), vect.begin(), vect.end());
  }
  else
  {  
    floatarray::insert(this->begin(), vect.begin(), vect.end());
  }
    
  
}
//----------------------------------------
void CFloatArray::Insert(const CFloatArray& vect, int32_t first, int32_t last, bool bEnd /*= true*/)
{
  if (vect.empty())
  {
    return;
  }

  if (bEnd)
  {  
    floatarray::insert(this->end(), vect.begin() + first, vect.begin() + last);
  }
  else
  {  
    floatarray::insert(this->begin(), vect.begin() + first, vect.begin() + last);
  }
    
  
}
//----------------------------------------

void CFloatArray::Insert(const float value)
{
  
  floatarray::push_back(value);    
  
}
//----------------------------------------
void CFloatArray::Insert(const int32_t value)
{
  
  floatarray::push_back(static_cast<float>(value));    
  
}
//----------------------------------------
void CFloatArray::Insert(const uint32_t value)
{
  
  floatarray::push_back(static_cast<float>(value));    
  
}

//----------------------------------------
CFloatArray::iterator CFloatArray::InsertAt(CFloatArray::iterator where, const float value)
{ 
  return floatarray::insert(where, value);     
}
//----------------------------------------
CFloatArray::iterator CFloatArray::InsertAt(int32_t index, const float value)
{ 
  if (index < 0)
  {
    return this->end();
  }

  if ((uint32_t)index >= this->size())
  {
    this->resize(index + 1);
    (*this)[index] = value;
    return this->begin() + index;
  }

  return floatarray::insert(this->begin() + index, value);     
}

//----------------------------------------
bool CFloatArray::Erase(CFloatArray::iterator where)
{ 
  floatarray::erase(where);   
  return true;
}
//----------------------------------------
CFloatArray::iterator CFloatArray::ReplaceAt(int32_t index, const float value)
{
  if (index < 0)
  {
    return this->end();
  }

  (*this)[index] = value;
  return this->begin() + index;

}
//----------------------------------------
CFloatArray::iterator CFloatArray::ReplaceAt(CFloatArray::iterator where, const float value)
{
  Erase(where);
  return InsertAt(where, value);     
}
//----------------------------------------
bool CFloatArray::Intersect(const CFloatArray& array, CFloatArray& intersect) const
{
  intersect.clear();

  CFloatArray::const_iterator itA;
  CFloatArray::const_iterator itB;

  for ( itA = this->begin( ); itA != this->end( ); itA++ )
  {
    for ( itB = array.begin( ); itB != array.end( ); itB++ )
    {
      if ( (*itA) == (*itB) )
      {
        intersect.Insert((*itB));
      }
    }
  }

  return (!(intersect.empty()));
}

//----------------------------------------
float* CFloatArray::ToArray()
{

  float* newArray = new float[this->size()];

  for (uint32_t i = 0 ; i < this->size() ; i++ )
  {
    newArray[i] = this->at(i);
  }


  return newArray;

}

//----------------------------------------
string CFloatArray::ToString(const string& delim /*= "," */, bool useBracket /*= true */) const
{
  ostringstream myStream;

  if (useBracket)
  {
    myStream << "[";
  }


  CFloatArray::const_iterator it;
  for ( it = this->begin(); it != this->end(); it++ )
  {
    myStream << *it;  
    if ((it+1) != this->end())
    {
      myStream << delim;  
    }
  }
  if (useBracket)
  {
    myStream << "]";
  }


  return myStream.str();

}

//----------------------------------------
void CFloatArray::GetRange(float& min, float& max)
{

  CTools::SetDefaultValue(min);
  CTools::SetDefaultValue(max);
  
  CFloatArray::iterator it;

  for ( it = begin( ); it != end( ); it++ )
  {
    if (it == begin())
    {
      min = *it;
      max = *it;
    }
    else
    {
      min = (min > *it) ? *it : min;
      max = (max > *it) ? max : *it;      
    }
  }
 
}

//----------------------------------------
void CFloatArray::Dump(ostream& fOut /* = cerr */) const
{

  if (CTrace::IsTrace() == false)
   {
      return;
   }

   CFloatArray::const_iterator it;
   int i = 0;

   fOut << "==> Dump a CFloatArray Object at "<< this << " with " <<  size() << " elements" << endl;

   for ( it = this->begin( ); it != this->end( ); it++ )
   {
     fOut << "CFloatArray[" << i << "]= " << CTools::Format(DUMP_FORMAT_DOUBLE, (*it)) << endl;  
     i++;
   }
    
   fOut << "==> END Dump a CFloatArray Object at "<< this << " with " <<  size() << " elements" << endl;

 
   fOut << endl;
   

}

//-------------------------------------------------------------
//------------------- CDoubleArray class --------------------
//-------------------------------------------------------------

CDoubleArray::CDoubleArray()
{

}

//----------------------------------------

CDoubleArray::CDoubleArray(const CDoubleArray& vect)
      	  : doublearray(vect)
{
  
}

//----------------------------------------

CDoubleArray::~CDoubleArray()
{

}
//----------------------------------------

void CDoubleArray::RemoveAll()
{
  
  doublearray::clear();

}

//----------------------------------------


const CDoubleArray& CDoubleArray::operator =(const CDoubleArray& vect)
{
  
  this->clear();

  Insert(vect);
    
  return *this;

    
}
//----------------------------------------
void CDoubleArray::Insert(int32_t* data, int32_t size)
{
  if (size <= 0)
  {
    return;
  }

  for (int32_t i = 0 ; i < size ; i++)
  {
    if (CTools::IsDefaultValue(static_cast<int32_t>(data[i])))
    {
      this->Insert(CTools::m_defaultValueDOUBLE);
    }
    else
    {
     this->Insert(data[i]);
    }
  }
}
//----------------------------------------
void CDoubleArray::Insert(uint32_t* data, int32_t size)
{
  if (size <= 0)
  {
    return;
  }

  for (int32_t i = 0 ; i < size ; i++)
  {
    if (CTools::IsDefaultValue(static_cast<uint32_t>(data[i])))
    {
      this->Insert(CTools::m_defaultValueDOUBLE);
    }
    else
    {
     this->Insert(data[i]);
    }
  }
}
//----------------------------------------
void CDoubleArray::Insert(double* data, int32_t size)
{
  if (size <= 0)
  {
    return;
  }

  for (int32_t i = 0 ; i < size ; i++)
  {
    this->Insert(data[i]);
  }
}
//----------------------------------------
void CDoubleArray::Insert(const CDoubleArray& vect, bool bEnd /*= true*/)
{
  if (vect.empty())
  {
    return;
  }

  if (bEnd)
  {  
    doublearray::insert(this->end(), vect.begin(), vect.end());
  }
  else
  {  
    doublearray::insert(this->begin(), vect.begin(), vect.end());
  }
    
  
}
//----------------------------------------
void CDoubleArray::Insert(const CUInt8Array& vect, bool bEnd /*= true*/)
{
  if (vect.empty())
  {
    return;
  }

  for (uint32_t i = 0 ; i < vect.size() ; i++)
  {
    if (bEnd)
    {  
      doublearray::insert(this->end(), static_cast<double>(vect.at(i)));
    }
    else
    {  
      doublearray::insert(this->begin(), static_cast<double>(vect.at(i)));
    }
  }
    
  
}

//----------------------------------------
void CDoubleArray::Insert(const CInt8Array& vect, bool bEnd /*= true*/)
{
  if (vect.empty())
  {
    return;
  }

  for (uint32_t i = 0 ; i < vect.size() ; i++)
  {
    if (bEnd)
    {  
      doublearray::insert(this->end(), static_cast<double>(vect.at(i)));
    }
    else
    {  
      doublearray::insert(this->begin(), static_cast<double>(vect.at(i)));
    }
  }
    
  
}

//----------------------------------------
void CDoubleArray::Insert(const CInt16Array& vect, bool bEnd /*= true*/)
{
  if (vect.empty())
  {
    return;
  }

  for (uint32_t i = 0 ; i < vect.size() ; i++)
  {
    if (bEnd)
    {  
      doublearray::insert(this->end(), static_cast<double>(vect.at(i)));
    }
    else
    {  
      doublearray::insert(this->begin(), static_cast<double>(vect.at(i)));
    }
  }
    
  
}
//----------------------------------------
void CDoubleArray::Insert(const CIntArray& vect, bool bEnd /*= true*/)
{
  if (vect.empty())
  {
    return;
  }

  for (uint32_t i = 0 ; i < vect.size() ; i++)
  {
    if (bEnd)
    {  
      doublearray::insert(this->end(), static_cast<double>(vect.at(i)));
    }
    else
    {  
      doublearray::insert(this->begin(), static_cast<double>(vect.at(i)));
    }
  }
    
  
}
//----------------------------------------
void CDoubleArray::Insert(const CFloatArray& vect, bool bEnd /*= true*/)
{
  if (vect.empty())
  {
    return;
  }

  for (uint32_t i = 0 ; i < vect.size() ; i++)
  {
    if (bEnd)
    {  
      doublearray::insert(this->end(), static_cast<double>(vect.at(i)));
    }
    else
    {  
      doublearray::insert(this->begin(), static_cast<double>(vect.at(i)));
    }
  }
    
  
}

//----------------------------------------
void CDoubleArray::Insert(const CDoubleArray& vect, int32_t first, int32_t last, bool bEnd /*= true*/)
{
  if (vect.empty())
  {
    return;
  }

  if (bEnd)
  {  
    doublearray::insert(this->end(), vect.begin() + first, vect.begin() + last);
  }
  else
  {  
    doublearray::insert(this->begin(), vect.begin() + first, vect.begin() + last);
  }
    
  
}
//----------------------------------------
void CDoubleArray::Insert(const CStringArray& vect, bool bEnd /*= true*/)
{
  if (vect.empty())
  {
    return;
  }

  for (uint32_t i = 0 ; i < vect.size() ; i++)
  {
    double value; 

    istringstream myStream(vect.at(i));

    if (!(myStream>>value))
    {
      CTools::SetDefaultValue(value);
    }


    if (bEnd)
    {  
      doublearray::insert(this->end(), value);
    }
    else
    {  
      doublearray::insert(this->begin(), value);
    }
  }
    
  
}
//----------------------------------------
void CDoubleArray::Insert(const string& vect, const string& delim /*= "," */, bool bEnd /*= true*/)
{
  if (vect.empty())
  {
    return;
  }
  string value = vect;

  if (value.at(0) == '[')
  {
    value.erase(0, 1);
  }

  if (value.at(value.size() - 1) == ']')
  {
    value.erase(value.size() - 1, 1);
  }

  CStringArray stringArray;
  stringArray.ExtractStrings(value, delim);
  
  this->Insert(stringArray, bEnd);

}

//----------------------------------------

void CDoubleArray::Insert(const double value)
{
  
  doublearray::push_back(value);    
  
}
//----------------------------------------
void CDoubleArray::Insert(const int32_t value)
{
  
  double doubleValue = static_cast<double>(value);
  doublearray::push_back(doubleValue);    
  
}
//----------------------------------------
void CDoubleArray::Insert(const uint32_t value)
{
  
  double doubleValue = static_cast<double>(value);
  doublearray::push_back(doubleValue);    
  
}
//----------------------------------------
void CDoubleArray::Insert(const int16_t value)
{
  
  double doubleValue = static_cast<double>(value);
  doublearray::push_back(doubleValue);    
  
}
//----------------------------------------
void CDoubleArray::Insert(const uint16_t value)
{
  
  double doubleValue = static_cast<double>(value);
  doublearray::push_back(doubleValue);    
  
}
//----------------------------------------
void CDoubleArray::Insert(const int8_t value)
{
  
  double doubleValue = static_cast<double>(value);
  doublearray::push_back(doubleValue);    
  
}
//----------------------------------------
void CDoubleArray::Insert(const uint8_t value)
{
  
  double doubleValue = static_cast<double>(value);
  doublearray::push_back(doubleValue);    
  
}

//----------------------------------------
CDoubleArray::iterator CDoubleArray::InsertAt(CDoubleArray::iterator where, const double value)
{ 
  return doublearray::insert(where, value);     
}
//----------------------------------------
CDoubleArray::iterator CDoubleArray::InsertAt(int32_t index, const double value)
{ 
  if (index < 0)
  {
    return this->end();
  }

  if ((uint32_t)index >= this->size())
  {
    this->resize(index + 1);
    (*this)[index] = value;
    return this->begin() + index;
  }

  return doublearray::insert(this->begin() + index, value);     
}


//----------------------------------------
bool CDoubleArray::Erase(CDoubleArray::iterator where)
{ 
  doublearray::erase(where);   
  return true;
}
//----------------------------------------
CDoubleArray::iterator CDoubleArray::ReplaceAt(int32_t index, const double value)
{
  if (index < 0)
  {
    return this->end();
  }

  (*this)[index] = value;
  return this->begin() + index;

}
//----------------------------------------
CDoubleArray::iterator CDoubleArray::ReplaceAt(CDoubleArray::iterator where, const double value)
{
  Erase(where);
  return InsertAt(where, value);     
}
//----------------------------------------
bool CDoubleArray::Intersect(const CDoubleArray& array, CDoubleArray& intersect) const
{
  intersect.clear();

  CDoubleArray::const_iterator itA;
  CDoubleArray::const_iterator itB;

  for ( itA = this->begin( ); itA != this->end( ); itA++ )
  {
    for ( itB = array.begin( ); itB != array.end( ); itB++ )
    {
      if ( (*itA) == (*itB) )
      {
        intersect.Insert((*itB));
      }
    }
  }

  return (!(intersect.empty()));
}
//----------------------------------------
int32_t CDoubleArray::FindIndex(double value) const
{
  int32_t index = -1;
  int32_t size = static_cast<int32_t>(this->size());

  for ( int32_t i = 0 ; i < size ; i++ )
  {
    if (CTools::AreEqual(value, this->at(i)))
    {
      index = i;
      break;
    }
  }

  return index;
}
//----------------------------------------
double* CDoubleArray::ToArray()
{

  double* newArray = new double[this->size()];

  for (uint32_t i = 0 ; i < this->size() ; i++ )
  {
    newArray[i] = this->at(i);
  }


  return newArray;

}


//----------------------------------------
string CDoubleArray::ToString(const string& delim /*= "," */, bool useBracket /*= true */) const
{
  ostringstream myStream;

  if (useBracket)
  {
    myStream << "[";
  }


  CDoubleArray::const_iterator it;
  for ( it = this->begin(); it != this->end(); it++ )
  {
    myStream << *it;  
    if ((it+1) != this->end())
    {
      myStream << delim;  
    }
  }
  if (useBracket)
  {
    myStream << "]";
  }


  return myStream.str();

}

//----------------------------------------
void CDoubleArray::GetRange(double& min, double& max)
{

  CTools::SetDefaultValue(min);
  CTools::SetDefaultValue(max);
  
  CDoubleArray::iterator it;

  for ( it = begin( ); it != end( ); it++ )
  {
    if (it == begin())
    {
      min = *it;
      max = *it;
    }
    else
    {
      min = (min > *it) ? *it : min;
      max = (max > *it) ? max : *it;      
    }
  }
 
}
//----------------------------------------
bool CDoubleArray::operator ==(const CDoubleArray& vect)
{
  uint32_t count = this->size();

  if (count != vect.size())
  {
    return false;
  }

  if (count == 0)
  {
    return true;
  }

  bool isEqual = true;

  for (uint32_t i = 0 ; i < count ; i++)
  {
    if ( ! CTools::AreEqual(this->at(i), vect.at(i)) )
    {
      isEqual = false;
      break;
    }
  }

  return isEqual;    
}


//----------------------------------------
void CDoubleArray::Dump(ostream& fOut /* = cerr */) const
{

  if (CTrace::IsTrace() == false)
   {
      return;
   }

   CDoubleArray::const_iterator it;
   int i = 0;

   fOut << "==> Dump a CDoubleArray Object at "<< this << " with " <<  size() << " elements" << endl;

   for ( it = this->begin( ); it != this->end( ); it++ )
   {
     fOut << "CDoubleArray[" << i << "]= " << CTools::Format(DUMP_FORMAT_DOUBLE, (*it)) << endl;  
     i++;
   }
    
   fOut << "==> END Dump a CDoubleArray Object at "<< this << " with " <<  size() << " elements" << endl;

 
   fOut << endl;
   

}


//-------------------------------------------------------------
//------------------- CDoublePtrArray class --------------------
//-------------------------------------------------------------

CDoublePtrArray::CDoublePtrArray(bool bDelete /*= true*/)
{
  m_bDelete = bDelete;

}


//----------------------------------------

CDoublePtrArray::~CDoublePtrArray()
{
  RemoveAll();

}

//----------------------------------------

void CDoublePtrArray::Insert(DoublePtr ob)
{
  
  doubleptrarray::push_back(ob);
      
}

//----------------------------------------
CDoublePtrArray::iterator CDoublePtrArray::InsertAt(CDoublePtrArray::iterator where, DoublePtr ob)
{
  
  return doubleptrarray::insert(where, ob);
      
}

//----------------------------------------
bool CDoublePtrArray::PopBack()
{
  CDoublePtrArray::iterator it = end();
  it--; // --end() --> iterator of the last element

  return Erase(it);

}

//----------------------------------------
CDoublePtrArray::iterator CDoublePtrArray::ReplaceAt(CDoublePtrArray::iterator where, DoublePtr ob)
{
  Erase(where);
  return InsertAt(where, ob);     
}
//----------------------------------------
bool CDoublePtrArray::Erase(int32_t index)
{
  if (index < 0)
  {
    return false;
  }
  return Erase(begin() + index);
}
//----------------------------------------
bool CDoublePtrArray::Erase(CDoublePtrArray::iterator it)
{
  if (it == end())
  {
    return false;
  }

  bool bOk = false;

  DoublePtr ob = *it;
  if (ob != NULL)
  {
    bOk = true;

    doubleptrarray::erase(it);
    if (m_bDelete)
    {
      Delete(ob);
    }
  }

  return bOk;
}

//----------------------------------------

void CDoublePtrArray::RemoveAll()
{
  
  CDoublePtrArray::iterator it;

  if (m_bDelete)
  {
    for (it = begin() ; it != end() ; it++)
    {
      Delete(*it);
    }

  }

  doubleptrarray::clear();


}
//----------------------------------------
void CDoublePtrArray::Delete(DoublePtr matrix)
{
  if (matrix == NULL)
  {
    return;
  }
  
  delete []matrix;
}
//----------------------------------------
uint32_t CDoublePtrArray::GetMatrixDim(uint32_t row)
{

  if (row >= GetMatrixNumberOfDims())
  {
    throw CException(CTools::Format("ERROR in CDoublePtrArray::GetMatrixDim - Out of bound error on matrix size - row is %d, but matrix size is %d\n",
                                 row, 
                                 GetMatrixNumberOfDims()),
                     BRATHL_LOGIC_ERROR);

  }

  return m_matrixDims.at(row);
}

//----------------------------------------
DoublePtr CDoublePtrArray::NewMatrix(double initialValue /* = CTools::m_defaultValueDOUBLE */ )
{
  uint32_t dims = GetMatrixNumberOfDims();

  if (dims <= 0)
  {
    return NULL;
  }

  uint32_t nbData = m_matrixDims.GetProductValues();
  
  if (nbData <= 0)
  {
    return NULL;;
  }

  DoublePtr matrix = new double[nbData];

  for (uint32_t i = 0 ; i < nbData ; i++)
  {
    matrix[i] = initialValue;
  }

  return matrix;
}
//----------------------------------------
void CDoublePtrArray::Dump(ostream& fOut /* = cerr */) const
{

  if (CTrace::IsTrace() == false)
  {
    return;
  }

  CDoublePtrArray::const_iterator it;
  int i = 0;

  fOut << "==> Dump a CDoublePtrArray Object at "<< this << " with " <<  size() << " elements" << endl;

  for ( it = this->begin( ); it != this->end( ); it++ )
  {
    DoublePtr matrix = *it;
    if ( matrix != NULL)
    {
      uint32_t nbValues = m_matrixDims.GetProductValues();

      for (uint32_t i = 0 ; i < nbValues ; i++)
      {
        fOut << "CDoublePtrArray[" << i << "]= " << matrix[i] << endl;
      }
    }
    else
    {
      fOut << "NULL";
    }

    i++;
  }

  fOut << "==> END Dump a CDoublePtrArray Object at "<< this << " with " <<  size() << " elements" << endl;


  fOut << endl;

}


//-------------------------------------------------------------
//------------------- CArrayDoublePtrArray class --------------------
//-------------------------------------------------------------

CArrayDoublePtrArray::CArrayDoublePtrArray(bool bDelete /*= true*/)
{
  Init();
  m_bDelete = bDelete;

}
//----------------------------------------
CArrayDoublePtrArray::CArrayDoublePtrArray(const CArrayDoublePtrArray& a)
{
  Init();
  Set(a);
}

//----------------------------------------

CArrayDoublePtrArray::~CArrayDoublePtrArray()
{
  RemoveAll();
}
//----------------------------------------

void CArrayDoublePtrArray::Init()
{
  m_bDelete = true;

  CTools::SetDefaultValue(m_minValue);
  CTools::SetDefaultValue(m_maxValue);
}
//----------------------------------------

void CArrayDoublePtrArray::GetMinMaxValues(double& min, double& max, bool recalc /*= true*/)
{
  if ( ! recalc )
  {
    min = m_minValue;
    max = m_maxValue;
    return;
  }

  uint32_t nRows = this->size();
  uint32_t nCols = this->at(0).size();

  DoublePtr dataValues = NULL;

  for (uint32_t xPos = 0 ; xPos < nRows ; xPos++)
  {
    for (uint32_t yPos = 0 ; yPos < nCols ; yPos++)
    {
      dataValues = (*this)[xPos][yPos];
      
      if (dataValues == NULL)
      {
        continue;
      }

      uint32_t nbValues = m_matrixDims.GetProductValues();

      for (uint32_t indexValues = 0 ; indexValues < nbValues; indexValues++)
      {
         //---------------------------------------------
        // compute variable min and max
        //---------------------------------------------
        AdjustValidMinMax(dataValues[indexValues]);
      }

    }
  }

  min = m_minValue;
  max = m_maxValue;
}
//----------------------------------------
void CArrayDoublePtrArray::AdjustValidMinMax(double value)
{
  if (CTools::IsDefaultValue(value))
  {
    return;
  }

  if (CTools::IsDefaultValue(m_minValue))
  {
    m_minValue = value;
  }
  else if (m_minValue > value)
  {
    m_minValue = value;
  }

  if (CTools::IsDefaultValue(m_maxValue))
  {
    m_maxValue = value;
  }
  else if (m_maxValue < value)
  {
    m_maxValue = value;
  }
}

//----------------------------------------
void CArrayDoublePtrArray::ResizeRC(uint32_t nrows, uint32_t ncols)
{
  this->resize(nrows);

  if (nrows == 0 || ncols == 0)
  {
    throw CException(CTools::Format("ERROR in CArrayDoublePtrArray::ResizeRC: number of rows (%d) or columns (%d) must be > 0", 
                                    nrows, ncols), BRATHL_LOGIC_ERROR);
  }

  for (uint32_t i = 0; i < nrows; ++i)
  {
    this->at(i).resize(ncols, NULL);
  }

}
//----------------------------------------
void CArrayDoublePtrArray::Set(const CArrayDoublePtrArray& a)
{
  RemoveAll();

  m_bDelete = true;
  m_matrixDims = a.m_matrixDims;
  m_minValue = a.m_minValue;
  m_maxValue = a.m_maxValue;

  uint32_t nRows = a.size();
  uint32_t nCols = a.at(0).size();
  
  ResizeRC(nRows, nCols);

  uint32_t nbData =  m_matrixDims.GetProductValues();

  for (uint32_t i = 0 ; i < nRows ; i++)
  {
    for (uint32_t j = 0; j < nCols; j++)
    {
      DoublePtr aValues = a[i][j];
      if (aValues == NULL)
      {
        continue;
      }

      DoublePtr array = NewMatrix();

      memcpy(array, aValues, sizeof(*array)*nbData);
      (*this)[i][j] = array;
    }       
  }


}
//----------------------------------------
const CArrayDoublePtrArray& CArrayDoublePtrArray::operator =(const CArrayDoublePtrArray& a)
{
  Set(a);
  return *this;

}

//----------------------------------------

void CArrayDoublePtrArray::RemoveAll()
{
    
  CArrayDoublePtrArray::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    Remove(*it);
  }

  arraydoubleptrarray::clear();

}
//----------------------------------------
void CArrayDoublePtrArray::Remove(doubleptrarray& vect)
{

  doubleptrarray::iterator it;

  if (m_bDelete)
  {
    for (it = vect.begin() ; it != vect.end() ; it++)
    {
      Delete(*it);
    }

  }

  vect.clear();

}

//----------------------------------------
void CArrayDoublePtrArray::Delete(DoublePtr matrix)
{
  if (matrix == NULL)
  {
    return;
  }
  
  delete []matrix;
}

//----------------------------------------
uint32_t CArrayDoublePtrArray::GetMatrixDim(uint32_t row)
{

  if (row >= GetMatrixNumberOfDims())
  {
    throw CException(CTools::Format("ERROR in CArrayDoublePtrArray::GetMatrixColDim - Out of bound error on matrix size - row is %d, but matrix size is %d\n",
                                 row, 
                                 GetMatrixNumberOfDims()),
                     BRATHL_LOGIC_ERROR);

  }

  return m_matrixDims.at(row);
}
//----------------------------------------
void CArrayDoublePtrArray::InitMatrix(double initialValue /* = CTools::m_defaultValueDOUBLE */)
{

  uint32_t nRows = this->size();
  uint32_t nCols = this->at(0).size();

  for (uint32_t xPos = 0 ; xPos < nRows ; xPos++)
  {
    for (uint32_t yPos = 0 ; yPos < nCols ; yPos++)
    {
      (*this)[xPos][yPos] = NewMatrix(initialValue);
    }
  }
}

//----------------------------------------
DoublePtr CArrayDoublePtrArray::NewMatrix(double initialValue /* = CTools::m_defaultValueDOUBLE */ )
{
  uint32_t dims = GetMatrixNumberOfDims();

  if (dims <= 0)
  {
    return NULL;
  }

  uint32_t nbData = m_matrixDims.GetProductValues();
  
  if (nbData <= 0)
  {
    return NULL;;
  }

  DoublePtr matrix = NULL;

  try
  {
    matrix = new double[nbData];
  }
  catch (CException& e)
  {
    RemoveAll(); // free memory  to be able to allocate new for error msg
    throw CException(CTools::Format("ERROR in CArrayDoublePtrArray::NewMatrix. Reason: %s\n",
					  e.what()),
            BRATHL_ERROR);
  }
  catch (bad_alloc& e) // If memory allocation (new) failed...
  {
    RemoveAll(); // free memory  to be able to allocate new for error msg
    throw CMemoryException(CTools::Format("ERROR: CArrayDoublePtrArray::NewMatrix) - Unable to allocate new memory to store the result.\nNative error: '%s'\n"
                                           " or try to allocate more than your system architecture can ever handle",
                                  e.what()));
  }
  catch (...)
  {
    RemoveAll(); // free memory  to be able to allocate new for error msg
    throw CMemoryException(CTools::Format("ERROR: CArrayDoublePtrArray::NewMatrix(). It may be that it does not remain enough memory to allocate %ld values"
                                           " or try to allocate more than your system architecture can ever handle",
					  (long)nbData));
  }


  if (matrix == NULL)
  {
    RemoveAll(); // free memory  to be able to allocate new for error msg
    throw CMemoryException(CTools::Format("ERROR: CArrayDoublePtrArray::NewMatrix(). It may be that it does not remain enough memory to allocate %ld values"
                                           " or try to allocate more than your system architecture can ever handle",
					  (long)nbData),
            BRATHL_ERROR);

  }

  
  for (uint32_t i = 0 ; i < nbData ; i++)
  {
    matrix[i] = initialValue;
  }
  return matrix;
}

//----------------------------------------
void CArrayDoublePtrArray::Dump(ostream& fOut /* = cerr */) const
{

  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CArrayDoublePtrArray Object at "<< this << " with " <<  size() << " elements" << endl;

  for ( uint32_t row = 0 ; row < this->size() ; row++ )
  {

    for ( uint32_t col = 0 ; col < this->at(row).size() ; col++ )
    {

      fOut << "CArrayDoublePtrArray[" << row << "," << col << "]=" << endl;

      DoublePtr matrix = this->at(row).at(col);
      if ( matrix != NULL)
      {
        uint32_t nbValues = m_matrixDims.GetProductValues();

        for (uint32_t i = 0 ; i < nbValues ; i++)
        {
          fOut << "double array[" << i << "]= " << matrix[i] << endl;
        }
      }
      else
      {
        fOut << "NULL";
      }

      fOut << endl;

    }     
  }
     
  fOut << "==> END Dump a CArrayDoublePtrArray Object at "<< this << " with " <<  size() << " elements" << endl;


  fOut << endl;
   

}


//-------------------------------------------------------------
//------------------- CArrayDoubleArray class --------------------
//-------------------------------------------------------------

CArrayDoubleArray::CArrayDoubleArray()
{
  Init();

}
//----------------------------------------
CArrayDoubleArray::CArrayDoubleArray(const CArrayDoubleArray& a)
{
  Init();
  Set(a);
}

//----------------------------------------

CArrayDoubleArray::~CArrayDoubleArray()
{
  RemoveAll();
}
//----------------------------------------

void CArrayDoubleArray::Init()
{
  CTools::SetDefaultValue(m_minValue);
  CTools::SetDefaultValue(m_maxValue);
}
//----------------------------------------

void CArrayDoubleArray::GetMinMaxValues(double& min, double& max, bool recalc /*= true*/)
{
  if ( ! recalc )
  {
    min = m_minValue;
    max = m_maxValue;
    return;
  }

  uint32_t nRows = this->size();
  uint32_t nCols = this->at(0).size();

  double dataValue;

  for (uint32_t xPos = 0 ; xPos < nRows ; xPos++)
  {
    for (uint32_t yPos = 0 ; yPos < nCols ; yPos++)
    {
      dataValue = (*this)[xPos][yPos];

      AdjustValidMinMax(dataValue);      
    }
  }

  min = m_minValue;
  max = m_maxValue;
}
//----------------------------------------
void CArrayDoubleArray::AdjustValidMinMax(double value)
{
  if (CTools::IsDefaultValue(value))
  {
    return;
  }

  if (CTools::IsDefaultValue(m_minValue))
  {
    m_minValue = value;
  }
  else if (m_minValue > value)
  {
    m_minValue = value;
  }

  if (CTools::IsDefaultValue(m_maxValue))
  {
    m_maxValue = value;
  }
  else if (m_maxValue < value)
  {
    m_maxValue = value;
  }
}

//----------------------------------------
void CArrayDoubleArray::ResizeRC(uint32_t nrows, uint32_t ncols)
{
  this->resize(nrows);

  if (nrows == 0 || ncols == 0)
  {
    throw CException(CTools::Format("ERROR in CArrayDoubleArray::ResizeRC: number of rows (%d) or columns (%d) must be > 0", 
                                    nrows, ncols), BRATHL_LOGIC_ERROR);
  }

  for (uint32_t i = 0; i < nrows; ++i)
  {
    this->at(i).resize(ncols);
  }

}
//----------------------------------------
void CArrayDoubleArray::Set(const CArrayDoubleArray& a)
{
  RemoveAll();

  m_minValue = a.m_minValue;
  m_maxValue = a.m_maxValue;

  uint32_t nRows = a.size();
  uint32_t nCols = a.at(0).size();
  
  ResizeRC(nRows, nCols);

  for (uint32_t i = 0 ; i < nRows ; i++)
  {
    for (uint32_t j = 0; j < nCols; j++)
    {
      (*this)[i][j] = a[i][j];;
    }       
  }


}
//----------------------------------------
const CArrayDoubleArray& CArrayDoubleArray::operator =(const CArrayDoubleArray& a)
{
  Set(a);
  return *this;

}

//----------------------------------------

void CArrayDoubleArray::RemoveAll()
{
    
  CArrayDoubleArray::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    it->clear();
  }

  arraydoublearray::clear();

}

/*
//----------------------------------------
uint32_t CArrayDoubleArray::GetMatrixDim(uint32_t row)
{

  if (row >= GetMatrixNumberOfDims())
  {
    throw CException(CTools::Format("ERROR in CArrayDoubleArray::GetMatrixColDim - Out of bound error on matrix size - row is %d, but matrix size is %d\n",
                                 row, 
                                 GetMatrixNumberOfDims()),
                     BRATHL_LOGIC_ERROR);

  }

  return m_matrixDims.at(row);
}
*/
//----------------------------------------
void CArrayDoubleArray::InitMatrix(double initialValue /* = CTools::m_defaultValueDOUBLE */)
{

  uint32_t nRows = this->size();
  uint32_t nCols = this->at(0).size();

  for (uint32_t xPos = 0 ; xPos < nRows ; xPos++)
  {
    for (uint32_t yPos = 0 ; yPos < nCols ; yPos++)
    {
      (*this)[xPos][yPos] = initialValue;
    }
  }
}


//----------------------------------------
void CArrayDoubleArray::Dump(ostream& fOut /* = cerr */) const
{

  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CArrayDoubleArray Object at "<< this << " with " <<  this->size() << " rows" << endl;

  for ( uint32_t row = 0 ; row < this->size() ; row++ )
  {
    fOut << "==> CArrayDoubleArray Object at row " << row << " with " <<   this->at(row).size() << " cols" << endl;
    for ( uint32_t col = 0 ; col < this->at(row).size() ; col++ )
    {
      fOut << "CArrayDoubleArray[" << row << "," << col << "]=" << this->at(row).at(col) << endl;
    }     
  }
     
  fOut << "==> END Dump a CArrayDoubleArray Object at "<< this << " with " <<  this->size()  << " rows" << endl;


  fOut << endl;
   

}

//-------------------------------------------------------------
//------------------- CArrayStringMap class --------------------
//-------------------------------------------------------------

CArrayStringMap::CArrayStringMap()
{
  Init();

}
//----------------------------------------
CArrayStringMap::CArrayStringMap(const CArrayStringMap& a)
{
  Init();
  Set(a);
}

//----------------------------------------

CArrayStringMap::~CArrayStringMap()
{
  RemoveAll();
}
//----------------------------------------

void CArrayStringMap::Init()
{
}


//----------------------------------------
void CArrayStringMap::Set(const CArrayStringMap& a)
{
  RemoveAll();
  CArrayStringMap::const_iterator it;

  for (it = a.begin() ; it != a.end() ; it++)
  {
    Insert(it->first, it->second, false);
  }


}

//----------------------------------------
CStringArray* CArrayStringMap::Insert(const string& key, const CStringArray& array, bool withExcept /* = true */)
{
  

  pair <maparraystring::iterator,bool> pairInsert;
  //If 'key' already exists --> pairInsert.second == false and
  // pairInsert.first then contains an iterator on the existing object
  // If 'key' does not exist --> pairInsert.second == true and
  // pairInsert.first then contains a iterator on the inserted object

  pairInsert = (maparraystring::insert(CArrayStringMap::value_type(key, array)));

  if( (pairInsert.second == false) && (withExcept))
  {
    CException e("ERROR in CStringMap::Insert - try to insert a string that already exists and parameter withExcept is true", BRATHL_LOGIC_ERROR);
    throw(e);
  }
  
  CArrayStringMap::iterator it = (pairInsert.first);

  return &((*it).second);

}

//----------------------------------------

const CStringArray* CArrayStringMap::Exists(const string& key) const
{
  CArrayStringMap::const_iterator it = maparraystring::find(key);
  if (it == end())
  {
    return NULL;
  }
  else
  {
    return &((*it).second);
  }

}
//----------------------------------------


bool CArrayStringMap::Erase(CArrayStringMap::iterator it)
{

  if (it == end())
  {
    return false;
  }

  bool bOk = false;

  (it->second).RemoveAll();
  
  maparraystring::erase(it);

  return bOk;

}
//----------------------------------------


bool CArrayStringMap::Erase(const string& key)
{

  CArrayStringMap::iterator it;

  it = maparraystring::find(key);

  return Erase(it);
  
}


//----------------------------------------
const CArrayStringMap& CArrayStringMap::operator =(const CArrayStringMap& a)
{
  Set(a);
  return *this;

}

//----------------------------------------

void CArrayStringMap::RemoveAll()
{
    
  CArrayStringMap::iterator it;

  for (it = begin() ; it != end() ; it++)
  {

    (it->second).RemoveAll();
  }

  maparraystring::clear();

}


//----------------------------------------
void CArrayStringMap::Dump(ostream& fOut /* = cerr */) const
{

  if (CTrace::IsTrace() == false)
  {
    return;
  }

   fOut << "==> Dump a CArrayStringMap Object at "<< this << " with " <<  size() << " elements" << endl;
 
   CArrayStringMap::const_iterator it;

   for (it = this->begin() ; it != this->end() ; it++)
   {
      fOut << "CArrayStringMap Key is = " << (*it).first << endl;
      fOut << "CArrayStringMap Value is = " <<  endl;

      (it->second).Dump(fOut);
   }

   fOut << "==> END Dump a CArrayStringMap Object at "<< this << " with " <<  size() << " elements" << endl;
  

  fOut << endl;
   

}

//-------------------------------------------------------------
//------------------- CMatrix class --------------------
//-------------------------------------------------------------

//----------------------------------------
CMatrix::CMatrix()
{

} 
//----------------------------------------
CMatrix::CMatrix(const CMatrix& m)
{
  Set(m);
}
//----------------------------------------
CMatrix::~CMatrix()
{

} 

//----------------------------------------
void CMatrix::Set(const CMatrix& m)
{
  m_name = m.m_name;
}

//----------------------------------------
const CMatrix& CMatrix::operator =(const CMatrix& m)
{
  Set(m);
  return *this;

}

//----------------------------------------
void CMatrix::Dump(ostream& fOut /* = cerr */) const
{

  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CMatrix Object at "<< this
       << " with " <<  GetNumberOfRows() << " rows and " <<  GetNumberOfCols() << " columns" << endl;

  fOut << "m_name = "<< m_name << endl;
  fOut << "m_xName = "<< m_xName << endl;
  fOut << "m_yName = "<< m_yName << endl;

  fOut << "==> END Dump a CDoublePtrArray Object at "<< this
       << " with " <<  GetNumberOfRows() << " rows  and " <<  GetNumberOfCols() << " columns" << endl;


  fOut << endl;

}


//-------------------------------------------------------------
//------------------- CMatrixDoublePtr class --------------------
//-------------------------------------------------------------

//----------------------------------------
CMatrixDoublePtr::CMatrixDoublePtr(uint32_t nrows, uint32_t ncols)
      : CMatrix()
{
  m_data.ResizeRC(nrows, ncols);

} 
//----------------------------------------
CMatrixDoublePtr::CMatrixDoublePtr(const CMatrixDoublePtr& m)
      : CMatrix(m)
{
  Set(m);
}
//----------------------------------------
CMatrixDoublePtr::~CMatrixDoublePtr()
{

} 

//----------------------------------------
void CMatrixDoublePtr::Set(const CMatrixDoublePtr& m)
{
  CMatrix::Set(m);
  m_data = m.m_data;
}

//----------------------------------------
const CMatrixDoublePtr& CMatrixDoublePtr::operator =(const CMatrixDoublePtr& m)
{
  Set(m);
  return *this;

}
//----------------------------------------
DoublePtr CMatrixDoublePtr::operator() (unsigned row, unsigned col)
{
  if (row >= GetNumberOfRows() || col >= GetNumberOfCols())
  {
    throw CException(CTools::Format("ERROR in CMatrixDoublePtr getter: index of row/col (%d/%d) must be < %d/%d", 
                                    row, col, GetNumberOfRows(), GetNumberOfCols()), BRATHL_LOGIC_ERROR);
  }

  return m_data[row][col];
}

//----------------------------------------
DoublePtr CMatrixDoublePtr::operator() (unsigned row, unsigned col) const
{
  if (row >= GetNumberOfRows() || col >= GetNumberOfCols())
  {
    throw CException(CTools::Format("ERROR in CMatrixDoublePtr getter: index of row/col (%d/%d) must be < %d/%d", 
                                    row, col, GetNumberOfRows(), GetNumberOfCols()), BRATHL_LOGIC_ERROR);
  }

  return m_data[row][col];
}
//----------------------------------------
doubleptrarray& CMatrixDoublePtr::operator[](const uint32_t& row)
{
  if (row >= GetNumberOfRows() )
  {
    throw CException(CTools::Format("ERROR in CMatrixDoublePtr setter: index of row (%d) must be < %d", 
                                    row, GetNumberOfRows()), BRATHL_LOGIC_ERROR);
  }
  return m_data[row];

}

//----------------------------------------
const doubleptrarray& CMatrixDoublePtr::operator[](const uint32_t & row) const
{
  if (row >= GetNumberOfRows() )
  {
    throw CException(CTools::Format("ERROR in CMatrixDoublePtr setter: index of row (%d) must be < %d", 
                                    row, GetNumberOfRows()), BRATHL_LOGIC_ERROR);
  }
  return m_data[row];

}

//----------------------------------------
void CMatrixDoublePtr::Set(uint32_t& row, uint32_t& col, DoublePtr x)
{
  if (row >= GetNumberOfRows() || col >= GetNumberOfCols())
  {
    throw CException(CTools::Format("ERROR in CMatrixDoublePtr setter: index of row/col (%d/%d) must be < %d/%d > 0", 
                                    row, col, GetNumberOfRows(), GetNumberOfCols()), BRATHL_LOGIC_ERROR);
  }

  m_data[row][col] = x;
}
//----------------------------------------
void CMatrixDoublePtr::SetMatrixDimsData(uint32_t nbValues)
{
  m_data.GetMatrixDims()->RemoveAll();
  m_data.GetMatrixDims()->Insert(nbValues);

}
//----------------------------------------
void CMatrixDoublePtr::InitMatrixDimsData(const CUIntArray& matrixDims, double initialValue /* = CTools::m_defaultValueDOUBLE */)
{
  if (matrixDims.size() <= 0)
  {
    SetMatrixDimsData(1);
  }
  else
  {
    SetMatrixDimsData(matrixDims);
  }

  InitMatrix(initialValue);

}

//----------------------------------------
bool CMatrixDoublePtr::IsMatrixDataSet()
{
  return (m_data.GetMatrixDims()->size() > 0);
}
//----------------------------------------
void CMatrixDoublePtr::GetMinMaxValues(double& min, double& max)
{
  m_data.GetMinMaxValues(min, max);
}

//----------------------------------------
void CMatrixDoublePtr::ScaleDownData(double scaleFactor, double addOffset, double defaultValue /* = CTools::m_defaultValueDOUBLE */)
{
  uint32_t nRows = this->GetNumberOfRows();
  uint32_t nCols = this->GetNumberOfCols();

  for ( uint32_t row = 0 ; row < nRows ; row++ )
  {
    for ( uint32_t col = 0 ; col < nCols ; col++ )
    {
      DoublePtr matrix = this->At(row, col);
      if ( matrix == NULL)
      {
        continue;
      }

      uint32_t nbValues = GetMatrixNumberOfValuesData();

      for (uint32_t i = 0 ; i < nbValues ; i++)
      {
        double value = matrix[i];

        if (CTools::IsDefaultValue(value))
        {
          value = defaultValue;
        }
        else
        {
          value = ((value - addOffset) / scaleFactor);
        }

        matrix[i] = value;
      }
    }
  }

}
//----------------------------------------
void CMatrixDoublePtr::ScaleUpData(double scaleFactor, double addOffset, double defaultValue /* = CTools::m_defaultValueDOUBLE */)
{
  uint32_t nRows = this->GetNumberOfRows();
  uint32_t nCols = this->GetNumberOfCols();

  for ( uint32_t row = 0 ; row < nRows ; row++ )
  {
    for ( uint32_t col = 0 ; col < nCols ; col++ )
    {


      DoublePtr matrix = this->At(row, col);
      if ( matrix == NULL)
      {
        continue;
      }

      uint32_t nbValues = GetMatrixNumberOfValuesData();

      for (uint32_t i = 0 ; i < nbValues ; i++)
      {
        double value = matrix[i];

        if (value == defaultValue)
        {
          value = CTools::m_defaultValueDOUBLE;
        }
        else
        {
          value = ((value * scaleFactor) + addOffset);
        }

        matrix[i] = value;
      }
  
    }

  }

}

//----------------------------------------
void CMatrixDoublePtr::Dump(ostream& fOut /* = cerr */) const
{

  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CMatrixDoublePtr Object at "<< this
       << " with " <<  GetNumberOfRows() << " rows and " <<  GetNumberOfCols() << " columns" << endl;

  CMatrix::Dump(fOut);

  m_data.Dump(fOut);

  fOut << "==> END Dump a CDoublePtrArray Object at "<< this
       << " with " <<  GetNumberOfRows() << " rows  and " <<  GetNumberOfCols() << " columns" << endl;



  fOut << endl;

}

//----------------------------------------
//-------------------------------------------------------------
//------------------- CMatrixDouble class --------------------
//-------------------------------------------------------------

//----------------------------------------
CMatrixDouble::CMatrixDouble(uint32_t nrows, uint32_t ncols)
      : CMatrix()
{
  m_data.ResizeRC(nrows, ncols);

} 
//----------------------------------------
CMatrixDouble::CMatrixDouble(const CMatrixDouble& m)
      : CMatrix(m)
{
  Set(m);
}
//----------------------------------------
CMatrixDouble::~CMatrixDouble()
{

} 

//----------------------------------------
void CMatrixDouble::Set(const CMatrixDouble& m)
{
  CMatrix::Set(m);
  m_data = m.m_data;
}

//----------------------------------------
const CMatrixDouble& CMatrixDouble::operator =(const CMatrixDouble& m)
{
  Set(m);
  return *this;

}
//----------------------------------------
DoublePtr CMatrixDouble::operator() (unsigned row, unsigned col)
{
  if (row >= GetNumberOfRows() || col >= GetNumberOfCols())
  {
    throw CException(CTools::Format("ERROR in CMatrixDouble getter: index of row/col (%d/%d) must be < %d/%d", 
                                    row, col, GetNumberOfRows(), GetNumberOfCols()), BRATHL_LOGIC_ERROR);
  }

  return &m_data[row][col];
}

//----------------------------------------
DoublePtr CMatrixDouble::operator() (unsigned row, unsigned col) const
{
  if (row >= GetNumberOfRows() || col >= GetNumberOfCols())
  {
    throw CException(CTools::Format("ERROR in CMatrixDouble getter: index of row/col (%d/%d) must be < %d/%d", 
                                    row, col, GetNumberOfRows(), GetNumberOfCols()), BRATHL_LOGIC_ERROR);
  }

  return const_cast<DoublePtr const>(&m_data[row][col]);;
}
//----------------------------------------
doublearray& CMatrixDouble::operator[](const uint32_t& row)
{
  if (row >= GetNumberOfRows() )
  {
    throw CException(CTools::Format("ERROR in CMatrixDouble setter: index of row (%d) must be < %d", 
                                    row, GetNumberOfRows()), BRATHL_LOGIC_ERROR);
  }
  return m_data[row];

}

//----------------------------------------
const doublearray& CMatrixDouble::operator[](const uint32_t & row) const
{
  if (row >= GetNumberOfRows() )
  {
    throw CException(CTools::Format("ERROR in CMatrixDouble setter: index of row (%d) must be < %d", 
                                    row, GetNumberOfRows()), BRATHL_LOGIC_ERROR);
  }
  return m_data[row];

}

//----------------------------------------
void CMatrixDouble::Set(uint32_t& row, uint32_t& col, DoublePtr x)
{
  if (row >= GetNumberOfRows() || col >= GetNumberOfCols())
  {
    throw CException(CTools::Format("ERROR in CMatrixDouble setter: index of row/col (%d/%d) must be < %d/%d > 0", 
                                    row, col, GetNumberOfRows(), GetNumberOfCols()), BRATHL_LOGIC_ERROR);
  }

  m_data[row][col] = *x;
}
//----------------------------------------
void CMatrixDouble::GetMinMaxValues(double& min, double& max)
{
  m_data.GetMinMaxValues(min, max);
}

//----------------------------------------
void CMatrixDouble::ScaleDownData(double scaleFactor, double addOffset, double defaultValue /* = CTools::m_defaultValueDOUBLE */)
{
  uint32_t nRows = this->GetNumberOfRows();
  uint32_t nCols = this->GetNumberOfCols();

  for ( uint32_t row = 0 ; row < nRows ; row++ )
  {
    for ( uint32_t col = 0 ; col < nCols ; col++ )
    {
      double value = *(this->At(row, col));

      if (CTools::IsDefaultValue(value))
      {
        value = defaultValue;
      }
      else
      {
        value = ((value - addOffset) / scaleFactor);
      }

      this->Set(row, col, &value);
    }
  }

}
//----------------------------------------
void CMatrixDouble::ScaleUpData(double scaleFactor, double addOffset, double defaultValue /* = CTools::m_defaultValueDOUBLE */)
{
  uint32_t nRows = this->GetNumberOfRows();
  uint32_t nCols = this->GetNumberOfCols();

  for ( uint32_t row = 0 ; row < nRows ; row++ )
  {
    for ( uint32_t col = 0 ; col < nCols ; col++ )
    {
      double value = *(this->At(row, col));

      if (value == defaultValue)
      {
        value = CTools::m_defaultValueDOUBLE;
      }
      else
      {
        value = ((value * scaleFactor) + addOffset);
      }
    
      this->Set(row, col, &value);
  
    }

  }

}

//----------------------------------------
void CMatrixDouble::Dump(ostream& fOut /* = cerr */) const
{

  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CMatrixDouble Object at "<< this
       << " with " <<  GetNumberOfRows() << " rows and " <<  GetNumberOfCols() << " columns" << endl;

  CMatrix::Dump(fOut);

  m_data.Dump(fOut);

  fOut << "==> END Dump a CDoublePtrArray Object at "<< this
       << " with " <<  GetNumberOfRows() << " rows  and " <<  GetNumberOfCols() << " columns" << endl;



  fOut << endl;

}

//----------------------------------------
//-------------------------------------------------------------
//------------------- CDoubleArrayOb class --------------------
//-------------------------------------------------------------

CDoubleArrayOb::CDoubleArrayOb()
{

}

//----------------------------------------

CDoubleArrayOb::CDoubleArrayOb(const CDoubleArrayOb& vect)
      	  : CDoubleArray(vect)
{
  
}

//----------------------------------------

CDoubleArrayOb::~CDoubleArrayOb()
{
  RemoveAll();
}
//----------------------------------------
const CDoubleArrayOb& CDoubleArrayOb::operator =(const CDoubleArrayOb& vect)
{
  CDoubleArray::operator =(vect);    
 
  return *this;
}

//----------------------------------------
CBratObject* CDoubleArrayOb::Clone()
{
  return new CDoubleArrayOb(*this);
}
//----------------------------------------

void CDoubleArrayOb::Dump(ostream& fOut /* = cerr */) const
{

  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CDoubleArrayOb Object at "<< this << " with " <<  size() << " elements" << endl;
  
  CDoubleArray::Dump(fOut);

  fOut << "==> END Dump a CDoubleArrayOb Object at "<< this << " with " <<  size() << " elements" << endl;


  fOut << endl;
   

}



//-------------------------------------------------------------
//------------------- CObStack class --------------------
//-------------------------------------------------------------

CObStack::CObStack(bool bDelete /*= true*/)
{
  m_bDelete = bDelete;

}



//----------------------------------------

CObStack::~CObStack()
{
  RemoveAll();

}


//----------------------------------------

void CObStack::Push(CBratObject* ob)
{
  
  obstack::push(ob);
      
}
//----------------------------------------
void CObStack::Pop()
{
  
  obstack::pop();
      
}

//----------------------------------------
CBratObject* CObStack::Top()
{
  
  return obstack::top();
      
}

//----------------------------------------

void CObStack::RemoveAll()
{

  while (! this->empty()) 
  {
    if (m_bDelete)
    {
      CBratObject* ob = this->top();
      if (ob != NULL)
      {
        delete  ob;
      }
    }

    this->pop();
  }

}



//-------------------------------------------------------------
//------------------- CObArray class --------------------
//-------------------------------------------------------------

CObArray::CObArray(bool bDelete /*= true*/)
{
  m_bDelete = bDelete;

}


//----------------------------------------

CObArray::CObArray(const CObArray& vect)
{
  *this = vect;  
}

//----------------------------------------

CObArray::~CObArray()
{
  RemoveAll();

}

//----------------------------------------

const CObArray& CObArray::operator =(const CObArray& vect)
{
  
  RemoveAll();

  m_bDelete = vect.m_bDelete;

  Insert(vect);
    
  return *this;

    
}

//----------------------------------------

void CObArray::Insert(const CObArray& vect)
{
  if (vect.empty())
  {
    return;
  }
  
  CObArray::const_iterator it;

  for (it = vect.begin() ; it != vect.end() ; it++)
  {
    CBratObject* ob = *it;
    if (ob != NULL)
    {
      Insert(ob->Clone());
    }
  }
   
  
}

//----------------------------------------

void CObArray::Insert(CBratObject* ob)
{
  
  obarray::push_back(ob);
      
}

//----------------------------------------
CObArray::iterator CObArray::InsertAt(CObArray::iterator where, CBratObject* ob)
{
  
  return obarray::insert(where, ob);
      
}

//----------------------------------------
bool CObArray::PopBack()
{
  CObArray::iterator it = end();
  it--; // --end() --> iterator of the last element

  return Erase(it);

}

//----------------------------------------
CObArray::iterator CObArray::ReplaceAt(CObArray::iterator where, CBratObject* ob)
{
  Erase(where);
  return InsertAt(where, ob);     
}
//----------------------------------------
bool CObArray::Erase(int32_t index)
{
  if (index < 0)
  {
    return false;
  }
  return Erase(begin() + index);
}
//----------------------------------------
bool CObArray::Erase(CBratObject* ob)
{
  if (ob == NULL)
  {
    return false;
  }

  CObArray::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    if (ob == *it)
    {
      return Erase(it);
    }
  }
 
  return false;
}
//----------------------------------------
bool CObArray::Erase(CObArray::iterator it)
{
  if (it == end())
  {
    return false;
  }

  bool bOk = false;

  CBratObject* ob = *it;
  if (ob != NULL)
  {
    bOk = true;
    obarray::erase(it);
    if (m_bDelete)
    {
      delete  ob;
    }
  }

  return bOk;
}

//----------------------------------------

void CObArray::RemoveAll()
{
  
  CObArray::iterator it;

  if (m_bDelete)
  {
    for (it = begin() ; it != end() ; it++)
    {
      CBratObject* ob = *it;
      if (ob != NULL)
      {
        delete  ob;
      }
    }
  }

  obarray::clear();


}

//----------------------------------------
void CObArray::Dump(ostream& fOut /* = cerr */) const
{

  if (CTrace::IsTrace() == false)
  {
    return;
  }

  CObArray::const_iterator it;
  int i = 0;

  fOut << "==> Dump a CObArray Object at "<< this << " with " <<  size() << " elements" << endl;

  for ( it = this->begin( ); it != this->end( ); it++ )
  {
    fOut << "CObArray[" << i << "]= " << endl;  
    if ((*it) != NULL)
    {
     (*it)->Dump(fOut);
    }
    i++;
  }

  fOut << "==> END Dump a CObArray Object at "<< this << " with " <<  size() << " elements" << endl;


  fOut << endl;


}

//-------------------------------------------------------------
//------------------- CObArrayOb class --------------------
//-------------------------------------------------------------

CObArrayOb::CObArrayOb(bool bDelete /*= true*/)
		: CObArray(bDelete)
{

}

//----------------------------------------

CObArrayOb::CObArrayOb(const CObArrayOb& lst)
{
  *this = lst;
}


//----------------------------------------

CObArrayOb::~CObArrayOb()
{
  RemoveAll();

}

//----------------------------------------
CBratObject* CObArrayOb::Clone()
{
  return new CObArrayOb(*this);
}

//----------------------------------------
const CObArrayOb& CObArrayOb::operator =(const CObArrayOb& vect)
{
  CObArray::operator =(vect);    
 
  return *this;
}


//----------------------------------------
void CObArrayOb::Dump(ostream& fOut /* = cerr */) const
{

  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CObArrayOb Object at "<< this << " with " <<  size() << " elements" << endl;

  CObArray::Dump(fOut);

  fOut << "==> END Dump a CObArrayOb Object at "<< this << " with " <<  size() << " elements" << endl;


  fOut << endl;


}



//-------------------------------------------------------------
//------------------- CStringMap class --------------------
//-------------------------------------------------------------

CStringMap::CStringMap()
{

}


//----------------------------------------
CStringMap::~CStringMap()
{
   RemoveAll();

}

//----------------------------------------
void CStringMap::Insert(const CStringMap& strmap, bool withExcept /* = true */)
{
  
   CStringMap::const_iterator it;
   for (it = strmap.begin() ; it != strmap.end() ; it++)
   {
     Insert((*it).first, (*it).second, withExcept);
   }


}

//----------------------------------------
string CStringMap::Insert(const string& key, const string& str, bool withExcept /* = true */)
{
  

  pair <mapstring::iterator,bool> pairInsert;
  //If 'key' already exists --> pairInsert.second == false and
  // pairInsert.first then contains an iterator on the existing object
  // If 'key' does not exist --> pairInsert.second == true and
  // pairInsert.first then contains a iterator on the inserted object

  pairInsert = (mapstring::insert(CStringMap::value_type(key, str)));

  if( (pairInsert.second == false) && (withExcept))
  {
    CException e("ERROR in CStringMap::Insert - try to insert a string that already exists and parameter withExcept is true", BRATHL_LOGIC_ERROR);
    throw(e);
  }
  
  CStringMap::iterator it = (pairInsert.first);

  return (*it).second;

}

//----------------------------------------

string CStringMap::Exists(const string& key) const
{
  CStringMap::const_iterator it = mapstring::find(key);
  if (it == end())
  {
    return "";
  }
  else
  {
    return (*it).second;
  }

}
//----------------------------------------
string CStringMap::IsValue(const string& value)
{
  CStringMap::iterator it;
  
  for(it = begin() ; it != end() ; it++)
  {
    if ((*it).second == value)
    {
      return (*it).first;
    }
  }

  return "";
}

//----------------------------------------

void CStringMap::RemoveAll()
{
  
  mapstring::clear();

}
//----------------------------------------


bool CStringMap::Erase(CStringMap::iterator it)
{

  if (it == end())
  {
    return false;
  }

  bool bOk = false;

  string str = it->second;
  if (str.empty() == false)
  {
    bOk = true;
    mapstring::erase(it);
  }

  return bOk;

}
//----------------------------------------


bool CStringMap::Erase(const string& key)
{

  CStringMap::iterator it;

  it = mapstring::find(key);

  return Erase(it);
  
}

//----------------------------------------
void CStringMap::GetKeys(CStringArray& keys, bool bRemoveAll /* = true */) const
{
  
  if (bRemoveAll) 
  {
    keys.clear();
  }

  CStringMap::const_iterator it;
  for (it = begin() ; it != end() ; it++)
  {
    keys.Insert(it->first);
  }

}
//----------------------------------------


void CStringMap::Dump(ostream& fOut /* = cerr */) const
{

   if (CTrace::IsTrace() == false)
   {
      return;
   }

   fOut << "==> Dump a CStringMap Object at "<< this << " with " <<  size() << " elements" << endl;
 
   CStringMap::const_iterator it;

   for (it = this->begin() ; it != this->end() ; it++)
   {
      string str = it->second;
      fOut << "CStringMap Key is = " << (*it).first << endl;
      fOut << "CStringMap Value is = " << (*it).second << endl;
   }

   fOut << "==> END Dump a CStringMap Object at "<< this << " with " <<  size() << " elements" << endl;
  
}


//-------------------------------------------------------------
//------------------- CIntMap class --------------------
//-------------------------------------------------------------

CIntMap::CIntMap()
{

}


//----------------------------------------
CIntMap::~CIntMap()
{
   RemoveAll();

}

//----------------------------------------

int32_t CIntMap::Insert(const string& key, int32_t value, bool withExcept /* = true */)
{
  

  pair <mapint::iterator,bool> pairInsert;
  //If 'key' already exists --> pairInsert.second == false and
  // pairInsert.first then contains an iterator on the existing object
  // If 'key' does not exist --> pairInsert.second == true and
  // pairInsert.first then contains a iterator on the inserted object

  pairInsert = (mapint::insert(CIntMap::value_type(key, value)));

  if( (pairInsert.second == false) && (withExcept))
  {
    CException e("ERROR in CIntMap::Insert - try to insert an integer value that already exists and parameter withExcept is true", BRATHL_LOGIC_ERROR);
    throw(e);
  }
  
  CIntMap::iterator it = (pairInsert.first);

  return (*it).second;

}

//----------------------------------------

int32_t CIntMap::Exists(const string& key) const
{
  CIntMap::const_iterator it = mapint::find(key);
  if (it == end())
  {
    return CTools::m_defaultValueINT32;
  }
  else
  {
    return (*it).second;
  }

}
//----------------------------------------
void CIntMap::Insert(const CIntMap& m, bool bRemoveAll /* = true */, bool withExcept /* = true */)
{
  if (bRemoveAll) 
  {
    RemoveAll();
  }

  CIntMap::const_iterator it;
  for (it = m.begin() ; it != m.end() ; it++)
  {
    Insert(it->first, it->second, withExcept);
  }
}
//----------------------------------------
void CIntMap::Insert(const CStringArray& keys, const CIntArray& values, bool bRemoveAll /* = true */, bool withExcept /* = true */)
{  
  uint32_t nbKeys = keys.size();
  uint32_t nbValues = values.size();


  if (nbKeys != nbValues)
  {
    CException e(CTools::Format("ERROR - CIntMap::Insert - number of keys %d is different of number of values %d: Arrays have not the same size: ",
                                nbKeys,
                                nbValues),
                BRATHL_LOGIC_ERROR);
    throw (e);
  }

  if (bRemoveAll) 
  {
    RemoveAll();
  }

  for (uint32_t i = 0 ; i < nbKeys ; i++)
  {
    Insert(keys[i], values[i], withExcept);
  }

}

//----------------------------------------

void CIntMap::RemoveAll()
{
  
  mapint::clear();

}
//----------------------------------------


bool CIntMap::Erase(CIntMap::iterator it)
{

  if (it == end())
  {
    return false;
  }

  bool bOk = false;

  int32_t value = it->second;
  if (CTools::IsDefaultValue(value) == false)
  {
    bOk = true;
    mapint::erase(it);
  }

  return bOk;

}
//----------------------------------------


bool CIntMap::Erase(const string& key)
{

  CIntMap::iterator it;

  it = mapint::find(key);

  return Erase(it);
  
}

//----------------------------------------

int32_t CIntMap::operator[](const string& key)
{

  //int32_t value = mapint::operator[](key);
  return Exists(key);

}

//----------------------------------------


void CIntMap::Dump(ostream& fOut /* = cerr */) const 
{

   if (CTrace::IsTrace() == false)
   {
      return;
   }

   fOut << "==> Dump a CIntMap Object at "<< this << " with " <<  size() << " elements" << endl;
 
   CIntMap::const_iterator it;

   for (it = this->begin() ; it != this->end() ; it++)
   {
      fOut << "CIntMap Key is = " << (*it).first << endl;
      fOut << "CIntMap Value is = " << (*it).second << endl;
   }

   fOut << "==> END Dump a CIntMap Object at "<< this << " with " <<  size() << " elements" << endl;
  
}

//-------------------------------------------------------------
//------------------- CUIntMap class --------------------
//-------------------------------------------------------------

CUIntMap::CUIntMap()
{

}


//----------------------------------------
CUIntMap::~CUIntMap()
{
   RemoveAll();

}

//----------------------------------------
void CUIntMap::Insert(const CUIntMap& m, bool bRemoveAll /* = true */, bool withExcept /* = true */)
{
  if (bRemoveAll) 
  {
    RemoveAll();
  }

  CUIntMap::const_iterator it;
  for (it = m.begin() ; it != m.end() ; it++)
  {
    Insert(it->first, it->second, withExcept);
  }
}

//----------------------------------------
void CUIntMap::Insert(const CStringArray& keys, uint32_t initValue, bool bRemoveAll /* = true */, bool withExcept /* = true */)
{
  if (bRemoveAll) 
  {
    RemoveAll();
  }

  CStringArray::const_iterator it;
  for (it = keys.begin() ; it != keys.end() ; it++)
  {
    Insert(*it, initValue, withExcept);
  }
}
//----------------------------------------
void CUIntMap::Insert(const CStringArray& keys, const CUIntArray& values, bool bRemoveAll /* = true */, bool withExcept /* = true */)
{  
  uint32_t nbKeys = keys.size();
  uint32_t nbValues = values.size();


  if (nbKeys != nbValues)
  {
    CException e(CTools::Format("ERROR - CUIntMap::Insert - number of keys %d is different of number of values %d: Arrays have not the same size: ",
                                nbKeys,
                                nbValues),
                BRATHL_LOGIC_ERROR);
    throw (e);
  }

  if (bRemoveAll) 
  {
    RemoveAll();
  }

  for (uint32_t i = 0 ; i < nbKeys ; i++)
  {
    Insert(keys[i], values[i], withExcept);
  }

}
//----------------------------------------
void CUIntMap::Insert(const CStringArray& keys, bool bRemoveAll /* = true */, bool withExcept /* = true */)
{  

  if (bRemoveAll) 
  {
    RemoveAll();
  }

  uint32_t nbKeys = keys.size();

  for (uint32_t i = 0 ; i < nbKeys ; i++)
  {
    Insert(keys[i], i, withExcept);
  }

}

//----------------------------------------

uint32_t CUIntMap::Insert(const string& key, uint32_t value, bool withExcept /* = true */)
{
  

  pair <mapuint::iterator,bool> pairInsert;
  //If 'key' already exists --> pairInsert.second == false and
  // pairInsert.first then contains an iterator on the existing object
  // If 'key' does not exist --> pairInsert.second == true and
  // pairInsert.first then contains a iterator on the inserted object

  pairInsert = (mapuint::insert(CUIntMap::value_type(key, value)));

  if( (pairInsert.second == false) && (withExcept))
  {
    CException e("ERROR in CUIntMap::Insert - try to insert an integer value that already exists and parameter withExcept is true", BRATHL_LOGIC_ERROR);
    throw(e);
  }
  
  CUIntMap::iterator it = (pairInsert.first);

  return (*it).second;

}
//----------------------------------------
void CUIntMap::GetKeys(CStringArray& keys, bool bRemoveAll /* = true */)
{
  
  if (bRemoveAll) 
  {
    keys.clear();
  }

  CUIntMap::iterator it;
  for (it = begin() ; it != end() ; it++)
  {
    keys.Insert(it->first);
  }

}
//----------------------------------------

uint32_t CUIntMap::Exists(const string& key) const
{
  CUIntMap::const_iterator it = mapuint::find(key);
  if (it == end())
  {
    return CTools::m_defaultValueUINT32;
  }
  else
  {
    return (*it).second;
  }

}
//----------------------------------------

void CUIntMap::RemoveAll()
{
  
  mapuint::clear();

}
//----------------------------------------


bool CUIntMap::Erase(CUIntMap::iterator it)
{

  if (it == end())
  {
    return false;
  }

  bool bOk = false;

  uint32_t value = it->second;
  if (CTools::IsDefaultValue(value) == false)
  {
    bOk = true;
    mapuint::erase(it);
  }

  return bOk;

}
//----------------------------------------


bool CUIntMap::Erase(const string& key)
{

  CUIntMap::iterator it;

  it = mapuint::find(key);

  return Erase(it);
  
}

//----------------------------------------

uint32_t CUIntMap::operator[](const string& key)
{

  //uint32_t value = mapuint::operator[](key);
  return Exists(key);

}

//----------------------------------------


void CUIntMap::Dump(ostream& fOut /* = cerr */) const 
{

   if (CTrace::IsTrace() == false)
   {
      return;
   }

   fOut << "==> Dump a CUIntMap Object at "<< this << " with " <<  size() << " elements" << endl;
 
   CUIntMap::const_iterator it;

   for (it = this->begin() ; it != this->end() ; it++)
   {
      fOut << "CUIntMap Key is = " << (*it).first << endl;
      fOut << "CUIntMap Value is = " << (*it).second << endl;
   }

   fOut << "==> END Dump a CUIntMap Object at "<< this << " with " <<  size() << " elements" << endl;
  
}
//-------------------------------------------------------------
//------------------- CDoubleMap class --------------------
//-------------------------------------------------------------

CDoubleMap::CDoubleMap()
{

}


//----------------------------------------
CDoubleMap::~CDoubleMap()
{
   RemoveAll();

}

//----------------------------------------

double CDoubleMap::Insert(const string& key, double value, bool withExcept /* = true */)
{
  

  pair <mapdouble::iterator,bool> pairInsert;
  //If 'key' already exists --> pairInsert.second == false and
  // pairInsert.first then contains an iterator on the existing object
  // If 'key' does not exist --> pairInsert.second == true and
  // pairInsert.first then contains a iterator on the inserted object

  pairInsert = (mapdouble::insert(CDoubleMap::value_type(key, value)));

  if( (pairInsert.second == false) && (withExcept))
  {
    CException e("ERROR in CDoubleMap::Insert - try to insert an double value that already exists and parameter withExcept is true", BRATHL_LOGIC_ERROR);
    throw(e);
  }
  
  CDoubleMap::iterator it = (pairInsert.first);

  return (*it).second;

}

//----------------------------------------

double CDoubleMap::Exists(const string& key) const
{
  CDoubleMap::const_iterator it = mapdouble::find(key);
  if (it == end())
  {
    return CTools::m_defaultValueDOUBLE;
  }
  else
  {
    return (*it).second;
  }

}
//----------------------------------------

void CDoubleMap::RemoveAll()
{
  
  mapdouble::clear();

}
//----------------------------------------


bool CDoubleMap::Erase(CDoubleMap::iterator it)
{

  if (it == end())
  {
    return false;
  }

  bool bOk = false;

  double value = it->second;
  if (CTools::IsDefaultValue(value) == false)
  {
    bOk = true;
    mapdouble::erase(it);
  }

  return bOk;

}
//----------------------------------------


bool CDoubleMap::Erase(const string& key)
{

  CDoubleMap::iterator it;

  it = mapdouble::find(key);

  return Erase(it);
  
}

//----------------------------------------

double CDoubleMap::operator[](const string& key)
{

  //double value = mapdouble::operator[](key);
  return Exists(key);

}

//----------------------------------------


void CDoubleMap::Dump(ostream& fOut /* = cerr */) const 
{

   if (CTrace::IsTrace() == false)
   {
      return;
   }

   fOut << "==> Dump a CDoubleMap Object at "<< this << " with " <<  size() << " elements" << endl;
 
   CDoubleMap::const_iterator it;

   for (it = this->begin() ; it != this->end() ; it++)
   {
      fOut << "CDoubleMap Key is = " << (*it).first << endl;
      fOut << "CDoubleMap Value is = " << CTools::Format(DUMP_FORMAT_DOUBLE, (*it).second) << endl;
   }

   fOut << "==> END Dump a CDoubleMap Object at "<< this << " with " <<  size() << " elements" << endl;
  
}
//-------------------------------------------------------------
//------------------- CObMap class --------------------
//-------------------------------------------------------------

CObMap::CObMap(bool bDelete /*= true*/)
{
  m_bDelete = bDelete;

}

//----------------------------------------

CObMap::CObMap(const CObMap& lst)
{
  *this = lst;
}


//----------------------------------------
CObMap::~CObMap()
{
   RemoveAll();

}

//----------------------------------------

CBratObject* CObMap::Insert(const string& key, CBratObject* ob, bool withExcept /* = true */)
{
  

  pair <CObMap::iterator,bool> pairInsert;


  //If 'key' already exists --> pairInsert.second == false and
  // pairInsert.first then contains an iterator on the existing object
  // If 'key' does not exist --> pairInsert.second == true and
  // pairInsert.first then contains a iterator on the inserted object
  pairInsert = (mapobject::insert(CObMap::value_type(key, ob)));

  if( (pairInsert.second == false) && (withExcept))
  {
    CException e("ERROR in CObMap::Insert - try to insert an object that already exists and parameter withExcept is true", BRATHL_LOGIC_ERROR);
    throw(e);
  }
  
  CObMap::iterator it = (pairInsert.first);
  return (*it).second;;

}

//----------------------------------------
void CObMap::Insert(const CObMap& obMap, bool withExcept)
{
  
  if (obMap.empty())
  {
    return;
  }


  CObMap::const_iterator it;

  for (it = obMap.begin() ; it != obMap.end() ; it++)
  {
    CBratObject* ob = (*it).second;
    if (ob != NULL)
    {
      this->Insert((*it).first, ob->Clone(), withExcept);
    }

  }

}

//----------------------------------------

const CObMap& CObMap::operator =(const CObMap& obMap)
{
  
  RemoveAll();

  m_bDelete = obMap.m_bDelete;

  Insert(obMap);
    
  return *this;

    
}

//----------------------------------------

CBratObject* CObMap::Exists(const string& key) const
{
  CObMap::const_iterator it = mapobject::find(key);
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
void CObMap::GetKeys(CStringArray& keys, bool bRemoveAll /* = true */, bool bUnique /* = false */)
{
  
  if (bRemoveAll) 
  {
    keys.clear();
  }

  CObMap::iterator it;
  for (it = begin() ; it != end() ; it++)
  {
    if (bUnique)
    {
      keys.InsertUnique(it->first);
    }
    else
    {
      keys.Insert(it->first);
    }
  }

}
//----------------------------------------
void CObMap::GetKeys(CStringList& keys, bool bRemoveAll /* = true */, bool bUnique /* = false */)
{
  
  if (bRemoveAll) 
  {
    keys.clear();
  }

  CObMap::iterator it;
  for (it = begin() ; it != end() ; it++)
  {
    if (bUnique)
    {
      keys.InsertUnique(it->first);
    }
    else
    {
      keys.Insert(it->first);
    }

  }

}
//----------------------------------------

bool CObMap::RenameKey(const string& oldKey, const string& newKey)
{
  CObMap::iterator itOld = mapobject::find(oldKey);
  if (itOld == end())
  {
    return false;
  }

  CObMap::iterator itNew = mapobject::find(newKey);
  if (itNew != end())
  {
    return false;
  }

  CBratObject *ob = itOld->second;
  mapobject::erase(itOld);

  Insert(newKey, ob);

  return true;
}
//----------------------------------------

void CObMap::ToArray(CObArray& obArray)
{
  
  CObMap::const_iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CBratObject* ob = it->second;
    if (ob != NULL)
    {
      obArray.Insert(ob);
    }
  }

}
//----------------------------------------

void CObMap::RemoveAll()
{
  
  CObMap::iterator it;

  if (m_bDelete)
  {
    for (it = begin() ; it != end() ; it++)
    {
      CBratObject* ob = it->second;
      if (ob != NULL)
      {
        delete  ob;
      }
    }
  }

  mapobject::clear();

}
//----------------------------------------


bool CObMap::Erase(CObMap::iterator it)
{

  if (it == end())
  {
    return false;
  }

  bool bOk = false;

   CBratObject *ob = it->second;
   if (ob != NULL)
   {
     bOk = true;
     mapobject::erase(it);
    if (m_bDelete)
    {
      delete  ob;
    }
   }

   return bOk;

}
//----------------------------------------


bool CObMap::Erase(const string& key)
{

  CObMap::iterator it = mapobject::find(key);

  return Erase(it);
  
}

//----------------------------------------

CBratObject* CObMap::operator[](const string& key)
{

  CBratObject *ob = mapobject::operator[](key);
//   if (ob == NULL)
//   {
//     logic_error e("key not found");
//     throw e;
//   }

return  ob;
}

//----------------------------------------


void CObMap::Dump(ostream& fOut /* = cerr */) const 
{

  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CObMap Object at "<< this << " with " <<  size() << " elements" << endl;

  CObMap::const_iterator it;

  for (it = this->begin() ; it != this->end() ; it++)
  {
    CBratObject *ob = it->second;
    fOut << "CObMap Key is = " << (*it).first << endl;
    fOut << "CObMap Value is = " << endl;
    if ( ob != NULL)
    {
      ob->Dump(fOut);
    }
  }

  fOut << "==> END Dump a CObMap Object at "<< this << " with " <<  size() << " elements" << endl;
  
}

//-------------------------------------------------------------
//------------------- CObIntMap class --------------------
//-------------------------------------------------------------

CObIntMap::CObIntMap(bool bDelete /*= true*/)
{
  m_bDelete = bDelete;

}


//----------------------------------------
CObIntMap::~CObIntMap()
{
   RemoveAll();

}

//----------------------------------------

CBratObject* CObIntMap::Insert(int32_t key, CBratObject* ob, bool withExcept /* = true */)
{
  

  pair <CObIntMap::iterator,bool> pairInsert;


  //If 'key' already exists --> pairInsert.second == false and
  // pairInsert.first then contains an iterator on the existing object
  // If 'key' does not exist --> pairInsert.second == true and
  // pairInsert.first then contains a iterator on the inserted object
  pairInsert = (mapintobject::insert(CObIntMap::value_type(key, ob)));

  if( (pairInsert.second == false) && (withExcept))
  {
    CException e("ERROR in CObIntMap::Insert - try to insert an object that already exists and parameter withExcept is true", BRATHL_LOGIC_ERROR);
    throw(e);
  }
  
  CObIntMap::iterator it = (pairInsert.first);
  return (*it).second;;

}

//----------------------------------------
void CObIntMap::Insert(const CObIntMap& obMap, bool withExcept)
{
  
  if (obMap.empty())
  {
    return;
  }


  CObIntMap::const_iterator it;

  for (it = obMap.begin() ; it != obMap.end() ; it++)
  {
    CBratObject* ob = (*it).second;
    if (ob != NULL)
    {
      this->Insert((*it).first, ob->Clone(), withExcept);
    }

  }

}

//----------------------------------------

const CObIntMap& CObIntMap::operator =(const CObIntMap& obMap)
{
  
  RemoveAll();

  m_bDelete = obMap.m_bDelete;

  Insert(obMap);
    
  return *this;
   
}

//----------------------------------------

CBratObject* CObIntMap::Exists(int32_t key) const
{
  CObIntMap::const_iterator it = mapintobject::find(key);
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
void CObIntMap::GetKeys(CIntArray& keys, bool bRemoveAll /* = true */)
{
  
  if (bRemoveAll) 
  {
    keys.clear();
  }

  CObIntMap::iterator it;
  for (it = begin() ; it != end() ; it++)
  {
    keys.Insert(it->first);
  }

}
//----------------------------------------

bool CObIntMap::RenameKey(int32_t oldKey, int32_t newKey)
{
  CObIntMap::iterator itOld = mapintobject::find(oldKey);
  if (itOld == end())
  {
    return false;
  }

  CObIntMap::iterator itNew = mapintobject::find(newKey);
  if (itNew != end())
  {
    return false;
  }

  CBratObject *ob = itOld->second;
  mapintobject::erase(itOld);

  Insert(newKey, ob);

  return true;
}
//----------------------------------------

void CObIntMap::RemoveAll()
{
  
  CObIntMap::iterator it;

  if (m_bDelete)
  {
    for (it = begin() ; it != end() ; it++)
    {
      CBratObject* ob = it->second;
      if (ob != NULL)
      {
        delete  ob;
      }
    }
  }

  mapintobject::clear();

}
//----------------------------------------


bool CObIntMap::Erase(CObIntMap::iterator it)
{

  if (it == end())
  {
    return false;
  }

  bool bOk = false;

   CBratObject *ob = it->second;
   if (ob != NULL)
   {
     bOk = true;
     mapintobject::erase(it);
    if (m_bDelete)
    {
      delete  ob;
    }
   }

   return bOk;

}
//----------------------------------------


bool CObIntMap::Erase(int32_t key)
{

  CObIntMap::iterator it;

  it = mapintobject::find(key);

  return Erase(it);
  
}

//----------------------------------------

CBratObject* CObIntMap::operator[](int32_t key)
{

  CBratObject *ob = mapintobject::operator[](key);
//   if (ob == NULL)
//   {
//     logic_error e("key not found");
//     throw e;
//   }

return  ob;
}

//----------------------------------------


void CObIntMap::Dump(ostream& fOut /* = cerr */) const 
{

  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CObIntMap Object at "<< this << " with " <<  size() << " elements" << endl;

  CObIntMap::const_iterator it;

  for (it = this->begin() ; it != this->end() ; it++)
  {
    CBratObject *ob = it->second;
    fOut << "CObIntMap Key is = " << (*it).first << endl;
    fOut << "CObIntMap Value is = " << endl;
    if ( ob != NULL)
    {
      ob->Dump(fOut);
    }
  }

   fOut << "==> END Dump a CObIntMap Object at "<< this << " with " <<  size() << " elements" << endl;
  
}

//-------------------------------------------------------------
//------------------- CObDoubleMap class --------------------
//-------------------------------------------------------------

CObDoubleMap::CObDoubleMap(bool bDelete /*= true*/)
{
  m_bDelete = bDelete;

}


//----------------------------------------
CObDoubleMap::~CObDoubleMap()
{
   RemoveAll();

}

//----------------------------------------

CBratObject* CObDoubleMap::Insert(double key, CBratObject* ob, bool withExcept /* = true */)
{
  

  pair <CObDoubleMap::iterator,bool> pairInsert;


  //If 'key' already exists --> pairInsert.second == false and
  // pairInsert.first then contains an iterator on the existing object
  // If 'key' does not exist --> pairInsert.second == true and
  // pairInsert.first then contains a iterator on the inserted object
  pairInsert = (mapdoubleobject::insert(CObDoubleMap::value_type(key, ob)));

  if( (pairInsert.second == false) && (withExcept))
  {
    CException e("ERROR in CObDoubleMap::Insert - try to insert an object that already exists and parameter withExcept is true", BRATHL_LOGIC_ERROR);
    throw(e);
  }
  
  CObDoubleMap::iterator it = (pairInsert.first);
  return (*it).second;;

}

//----------------------------------------

const CObDoubleMap& CObDoubleMap::operator =(const CObDoubleMap& obMap)
{
  
  RemoveAll();

  m_bDelete = obMap.m_bDelete;

  Insert(obMap);
    
  return *this;
   
}

//----------------------------------------
void CObDoubleMap::Insert(const CObDoubleMap& obMap, bool withExcept)
{
  
  if (obMap.empty())
  {
    return;
  }


  CObDoubleMap::const_iterator it;

  for (it = obMap.begin() ; it != obMap.end() ; it++)
  {
    CBratObject* ob = (*it).second;
    if (ob != NULL)
    {
      this->Insert((*it).first, ob->Clone(), withExcept);
    }

  }

}


//----------------------------------------

CBratObject* CObDoubleMap::Exists(double key) const
{
  CObDoubleMap::const_iterator it = mapdoubleobject::find(key);
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
void CObDoubleMap::GetKeys(CDoubleArray& keys, bool bRemoveAll /* = true */)
{
  
  if (bRemoveAll) 
  {
    keys.clear();
  }

  CObDoubleMap::iterator it;
  for (it = begin() ; it != end() ; it++)
  {
    keys.Insert(it->first);
  }

}
//----------------------------------------

bool CObDoubleMap::RenameKey(double oldKey, double newKey)
{
  CObDoubleMap::iterator itOld = mapdoubleobject::find(oldKey);
  if (itOld == end())
  {
    return false;
  }

  CObDoubleMap::iterator itNew = mapdoubleobject::find(newKey);
  if (itNew != end())
  {
    return false;
  }

  CBratObject *ob = itOld->second;
  mapdoubleobject::erase(itOld);

  Insert(newKey, ob);

  return true;
}
//----------------------------------------

void CObDoubleMap::RemoveAll()
{
  
  CObDoubleMap::iterator it;

  if (m_bDelete)
  {
    for (it = begin() ; it != end() ; it++)
    {
      CBratObject* ob = it->second;
      if (ob != NULL)
      {
        delete  ob;
      }
    }
  }

  mapdoubleobject::clear();

}
//----------------------------------------


bool CObDoubleMap::Erase(CObDoubleMap::iterator it)
{

  if (it == end())
  {
    return false;
  }

  bool bOk = false;

   CBratObject *ob = it->second;
   if (ob != NULL)
   {
     bOk = true;
     mapdoubleobject::erase(it);
    if (m_bDelete)
    {
      delete  ob;
    }
   }

   return bOk;

}
//----------------------------------------


bool CObDoubleMap::Erase(double key)
{

  CObDoubleMap::iterator it;

  it = mapdoubleobject::find(key);

  return Erase(it);
  
}

//----------------------------------------

CBratObject* CObDoubleMap::operator[](double key)
{

  CBratObject *ob = mapdoubleobject::operator[](key);
//   if (ob == NULL)
//   {
//     logic_error e("key not found");
//     throw e;
//   }

return  ob;
}

//----------------------------------------


void CObDoubleMap::Dump(ostream& fOut /* = cerr */) const 
{

  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CObDoubleMap Object at "<< this << " with " <<  size() << " elements" << endl;

  CObDoubleMap::const_iterator it;

  for (it = this->begin() ; it != this->end() ; it++)
  {
    CBratObject *ob = it->second;
    fOut << "CObDoubleMap Key is = " << CTools::Format(DUMP_FORMAT_DOUBLE, (*it).first) << endl;
    fOut << "CObDoubleMap Value is = " << endl;
    if ( ob != NULL)
    {
      ob->Dump(fOut);
    }
  }

  fOut << "==> END Dump a CObDoubleMap Object at "<< this << " with " <<  size() << " elements" << endl;
  
}

//-------------------------------------------------------------
//------------------- CDoublePtrDoubleMap class --------------------
//-------------------------------------------------------------

CDoublePtrDoubleMap::CDoublePtrDoubleMap(bool bDelete /*= true*/)
{
  m_bDelete = bDelete;

}
//----------------------------------------
CDoublePtrDoubleMap::CDoublePtrDoubleMap(const CUIntArray& matrixDims, bool bDelete /*= true*/)
{
  m_bDelete = bDelete;
  SetMatrixDims(matrixDims);

}


//----------------------------------------
CDoublePtrDoubleMap::~CDoublePtrDoubleMap()
{
   RemoveAll();

}

//----------------------------------------
uint32_t CDoublePtrDoubleMap::GetMatrixColDim(uint32_t row)
{

  if (row >= GetMatrixNumberOfRows())
  {
    throw CException(CTools::Format("ERROR in CDoublePtrDoubleMap::GetMatrixColDim - Out of bound error on matrix size - row is %d, but matrix size is %d\n",
                                 row, 
                                 GetMatrixNumberOfRows()),
                     BRATHL_LOGIC_ERROR);

  }

  return m_matrixDims.at(row);
}

//----------------------------------------
DoublePtr* CDoublePtrDoubleMap::NewMatrix(double initialValue /* = CTools::m_defaultValueDOUBLE */ )
{
  uint32_t rows = GetMatrixNumberOfRows();

  if (rows <= 0)
  {
    return NULL;
  }

  DoublePtr* matrix = NULL;

  try
  {
    matrix = new DoublePtr[rows];
  }
  catch (CException& e)
  {
    throw CException(CTools::Format("ERROR in CDoublePtrDoubleMap::NewMatrix. Reason: %s\n",
					  e.what()),
            BRATHL_ERROR);
  }
  catch (bad_alloc& e) // If memory allocation (new) failed...
  {
    RemoveAll(); // free memory  to be able to allocate new for error msg
    throw CMemoryException(CTools::Format("ERROR: CDoublePtrDoubleMap::NewMatrix) - Unable to allocate new memory to store the result.\nNative error: '%s'\n"
                                           " or try to allocate more than your system architecture can ever handle",
                                  e.what()));
  }
  catch (...)
  {
    RemoveAll(); // free memory  to be able to allocate new for error msg
    throw CException(CTools::Format("ERROR: CDoublePtrDoubleMap::NewMatrix(). It may be that it does not remain enough memory to allocate %ld values"
                                           " or try to allocate more than your system architecture can ever handle",
					  (long)rows),
            BRATHL_ERROR);
  }


  if (matrix == NULL)
  {
    RemoveAll(); // free memory  to be able to allocate new for error msg
    throw CMemoryException(CTools::Format("ERROR: CDoublePtrDoubleMap::NewMatrix(). It may be that it does not remain enough memory to allocate %ld values"
                                           " or try to allocate more than your system architecture can ever handle",
					   (long)rows),
            BRATHL_ERROR);

  }
 
  uint32_t j = 0;
  uint32_t k = 0;
/*
  for (j = 0; j < rows; j++)
  {
    matrix[j] = NULL;
  }
*/
  for (j = 0; j < rows; j++)
  {
    uint32_t cols = m_matrixDims[j];
    matrix[j] = new double[ cols ];

    for (k = 0 ; k < cols ; k++)
    {
      matrix[j][k] = initialValue;
    }
  }

  return matrix;
}
//----------------------------------------
DoublePtr* CDoublePtrDoubleMap::Insert(double key, double initialValue /* = CTools::m_defaultValueDOUBLE */)
{
  DoublePtr* matrix = NewMatrix(initialValue);

  if (matrix != NULL)
  {
    matrix = Insert(key, matrix, true);
  }

  return matrix;

}
//----------------------------------------
DoublePtr* CDoublePtrDoubleMap::Insert(double key, DoublePtr* ob, bool withExcept /* = true */)
{
  

  pair <CDoublePtrDoubleMap::iterator,bool> pairInsert;


  //If 'key' already exists --> pairInsert.second == false and
  // pairInsert.first then contains an iterator on the existing object
  // If 'key' does not exist --> pairInsert.second == true and
  // pairInsert.first then contains a iterator on the inserted object
  pairInsert = (mapdoubledoubleptr::insert(CDoublePtrDoubleMap::value_type(key, ob)));

  if( (pairInsert.second == false) && (withExcept))
  {
    CException e("ERROR in CDoublePtrDoubleMap::Insert - try to insert an object that already exists and parameter withExcept is true", BRATHL_LOGIC_ERROR);
    throw(e);
  }
  
  CDoublePtrDoubleMap::iterator it = (pairInsert.first);
  return (*it).second;;

}


//----------------------------------------

DoublePtr* CDoublePtrDoubleMap::Exists(double key) const
{
  CDoublePtrDoubleMap::const_iterator it = mapdoubledoubleptr::find(key);
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
void CDoublePtrDoubleMap::GetKeys(CDoubleArray& keys, bool bRemoveAll /* = true */)
{
  
  if (bRemoveAll) 
  {
    keys.clear();
  }

  CDoublePtrDoubleMap::iterator it;
  for (it = begin() ; it != end() ; it++)
  {
    keys.Insert(it->first);
  }

}
//----------------------------------------

bool CDoublePtrDoubleMap::RenameKey(double oldKey, double newKey)
{
  CDoublePtrDoubleMap::iterator itOld = mapdoubledoubleptr::find(oldKey);
  if (itOld == end())
  {
    return false;
  }

  CDoublePtrDoubleMap::iterator itNew = mapdoubledoubleptr::find(newKey);
  if (itNew != end())
  {
    return false;
  }

  DoublePtr* ob = itOld->second;
  mapdoubledoubleptr::erase(itOld);

  Insert(newKey, ob);

  return true;
}

//----------------------------------------
void CDoublePtrDoubleMap::Delete(DoublePtr* matrix)
{
  if (matrix == NULL)
  {
    return;
  }

  // Note that deletion is the opposite order of allocation:
  for (uint32_t i = GetMatrixNumberOfRows() ; i > 0; --i)
  {
    if (matrix[i-1] != NULL)
    {
      delete []matrix[i-1];
      matrix[i-1] = NULL;
    }
  }

  delete[] matrix;
}
//----------------------------------------

void CDoublePtrDoubleMap::RemoveAll()
{
  
  CDoublePtrDoubleMap::iterator it;

  if (m_bDelete)
  {
    for (it = begin() ; it != end() ; it++)
    {
      Delete(it->second);
    }
  }

  mapdoubledoubleptr::clear();

}
//----------------------------------------
bool CDoublePtrDoubleMap::Erase(CDoublePtrDoubleMap::iterator it)
{

  if (it == end())
  {
    return false;
  }

  bool bOk = false;

   DoublePtr* matrix = it->second;
   if (matrix != NULL)
   {
     bOk = true;
     mapdoubledoubleptr::erase(it);
    if (m_bDelete)
    {
      Delete(matrix);
    }
   }

   return bOk;

}
//----------------------------------------


bool CDoublePtrDoubleMap::Erase(double key)
{

  CDoublePtrDoubleMap::iterator it;

  it = mapdoubledoubleptr::find(key);

  return Erase(it);
  
}

//----------------------------------------

DoublePtr* CDoublePtrDoubleMap::operator[](double key)
{

  DoublePtr* ob = mapdoubledoubleptr::operator[](key);
//   if (ob == NULL)
//   {
//     logic_error e("key not found");
//     throw e;
//   }

return  ob;
}

//----------------------------------------


void CDoublePtrDoubleMap::Dump(ostream& fOut /* = cerr */) const 
{

  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CDoublePtrDoubleMap Object at "<< this << " with " <<  size() << " elements" << endl;

  CDoublePtrDoubleMap::const_iterator it;

  for (it = this->begin() ; it != this->end() ; it++)
  {
    DoublePtr* matrix = it->second;
    fOut << "CDoublePtrDoubleMap Key is = " << CTools::Format(DUMP_FORMAT_DOUBLE, (*it).first) << endl;
    fOut << "CDoublePtrDoubleMap Value is = " << endl;
    if ( matrix != NULL)
    {
     // Here's how to access the (i,j) element:
     for (uint32_t i = 0 ; i < GetMatrixNumberOfRows() ; i++)
     {
       fOut << "Matrix(" << i << ", j = 0 to " << m_matrixDims[i] << ") = "; 
       for (uint32_t j = 0; j < m_matrixDims[i]; j++) 
       {
         fOut << matrix[i][j] << " ";
       }

       fOut << endl;
     }
    }
  }

  fOut << "==> END Dump a CDoublePtrDoubleMap Object at "<< this << " with " <<  size() << " elements" << endl;
  
}


//-------------------------------------------------------------
//------------------- CPtrMap class --------------------
//-------------------------------------------------------------

CPtrMap::CPtrMap(bool bDelete /*= true*/)
{
  m_bDelete = bDelete;

}


//----------------------------------------
CPtrMap::~CPtrMap()
{
   RemoveAll();

}

//----------------------------------------

void* CPtrMap::Insert(const string& key, void* ptr, bool withExcept /* = true */)
{
  

  pair <CPtrMap::iterator,bool> pairInsert;


  //If 'key' already exists --> pairInsert.second == false and
  // pairInsert.first then contains an iterator on the existing object
  // If 'key' does not exist --> pairInsert.second == true and
  // pairInsert.first then contains a iterator on the inserted object
  pairInsert = (mapptr::insert(CPtrMap::value_type(key, ptr)));

  if( (pairInsert.second == false) && (withExcept))
  {
    CException e("ERROR in CPtrMap::Insert - try to insert an object that already exists and parameter withExcept is true", BRATHL_LOGIC_ERROR);
    throw(e);
  }
  
  CPtrMap::iterator it = (pairInsert.first);
  return (*it).second;;

}
//----------------------------------------

void CPtrMap::Insert(const CPtrMap& ptrMap, bool withExcept /* = true */)
{
  
  if (ptrMap.empty())
  {
    return;
  }

  CPtrMap::const_iterator it;

  for (it = ptrMap.begin() ; it != ptrMap.end() ; it++)
  {
    this->Insert((*it).first, (*it).second, withExcept);
  }

}

//----------------------------------------

void* CPtrMap::Exists(const string& key) const
{
  CPtrMap::const_iterator it = mapptr::find(key);
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

void CPtrMap::RemoveAll()
{
  
  CPtrMap::iterator it;

  if (m_bDelete)
  {
    for (it = begin() ; it != end() ; it++)
    {
      if (it->second != NULL)
      {
        free(it->second);
      }
    }
  }

  mapptr::clear();

}
//----------------------------------------


bool CPtrMap::Erase(CPtrMap::iterator it)
{

  if (it == end())
  {
    return false;
  }

  bool bOk = false;

   void *ptr = it->second;
   if (ptr != NULL)
   {
     bOk = true;
     mapptr::erase(it);
    if (m_bDelete)
    {
      free(ptr);
    }
   }

   return bOk;

}
//----------------------------------------


bool CPtrMap::Erase(const string& key)
{

  CPtrMap::iterator it;

  it = mapptr::find(key);

  return Erase(it);
  
}

//----------------------------------------

void* CPtrMap::operator[](const string& key)
{

  void *ptr = mapptr::operator[](key);
//   if (ob == NULL)
//   {
//     logic_error e("key not found");
//     throw e;
//   }

return  ptr;
}

//----------------------------------------


void CPtrMap::Dump(ostream& fOut /* = cerr */) const 
{

   if (CTrace::IsTrace() == false)
   {
      return;
   }

   fOut << "==> Dump a CPtrMap Object at "<< this << " with " <<  size() << " elements" << endl;
 
   CPtrMap::const_iterator it;

   for (it = this->begin() ; it != this->end() ; it++)
   {
      fOut << "CPtrMap Key is = " << (*it).first << endl;
      fOut << "CPtrMap Value is = " << (*it).second << endl;
   }

   fOut << "==> END Dump a (*it).first <<  Object at "<< this << " with " <<  size() << " elements" << endl;
  
}


} // end namespace
