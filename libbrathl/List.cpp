
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
#include <sstream> 

#include "new-gui/Common/tools/brathl_error.h" 
#include "brathl.h" 

#include <string>  

#include "new-gui/Common/tools/TraceLog.h" 
#include "new-gui/Common/tools/Exception.h"
 
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

void CStringList::Insert(const std::string& str, bool bEnd /*= true*/)
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
bool CStringList::Exists(const std::string& str) const
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
bool CStringList::ExistsNoCase(const std::string& str) const
{
   CStringList::const_iterator it;

   for ( it = this->begin( ); it != this->end( ); it++ )
   {
     if (str_icmp(str, *it))
     {
       return true;
     }
   }
    
  return false;  
}
//----------------------------------------
int32_t CStringList::FindIndex(const std::string& str, bool compareNoCase /* = false */) const
{
  CStringList::const_iterator it;
  int32_t i = -1;

  for (it = this->begin() ; it != this->end() ; it++ )
  {
    i++;
    if (compareNoCase)
    {
      if (str_icmp(str, *it))
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

void CStringList::InsertUnique(const std::string& str, bool bEnd /*= true*/)
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
   std::vector<std::string>::const_iterator it;

   for ( it = vect.begin( ); it != vect.end( ); it++ )
   {
     this->InsertUnique(*it, bEnd);
   }

}
//----------------------------------------
void CStringList::InsertUnique(const stringlist& lst, bool bEnd /*= true*/)
{
   std::list<std::string>::const_iterator it;

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

void CStringList::ExtractStrings( const std::string& str, const char delim, bool bRemoveAll /* = true */ )
{

	if ( bRemoveAll == true )
	{
		this->clear();
	}

	std::string sequence = str;

	size_t pos = sequence.find( delim );

	size_t nLg;

	while ( sequence.empty() == false )
	{

		if ( pos > 0 )
		{
			this->Insert( sequence.substr( 0, pos ) );
		}
		if ( pos < 0 )
		{
			this->Insert( sequence );
			sequence = "";
		}

		nLg = ( sequence.length() - pos - 1 );

		if ( nLg > 0 )
		{
			sequence = sequence.substr( pos + 1, nLg );
		}
		else
		{
			sequence = "";
		}

		pos = sequence.find( delim );
	}
}


//----------------------------------------

void CStringList::ExtractStrings( const std::string& str, const std::string& delim, bool bRemoveAll /* = true */ )
{
	if ( bRemoveAll == true )
	{
		this->clear();
	}

	std::string sequence = str;

	size_t pos = sequence.find( delim );

	size_t nLg;

	while ( sequence.empty() == false )
	{
		if ( pos > 0 )
		{
			this->Insert( sequence.substr( 0, pos ) );
		}
		if ( pos < 0 )
		{
			this->Insert( sequence );
			sequence = "";
		}

		nLg = ( sequence.length() - pos - delim.length() );

		if ( nLg > 0 )
		{
			sequence = sequence.substr( pos + 1, nLg );
		}
		else
		{
			sequence = "";
		}

		pos = sequence.find( delim );
	}
}

//----------------------------------------

std::string CStringList::ToString(const std::string& delim /*= "," */, bool useBracket /*= true */) const
{
    UNUSED(useBracket);

  std::string str;
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
void CStringList::Erase(const std::string& str) 
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
void CStringList::ExtractKeys( const std::string& str, const std::string& delim, bool bRemoveAll /* = true */ )
{
	if ( bRemoveAll == true )
	{
		this->clear();
	}

	std::string sequence = str;

	size_t pos = sequence.find( delim, 0 );

	size_t beginSearch = 0;

	while ( true )
	{
		if ( pos > 0 )
		{
			this->Insert( sequence.substr( 0, pos ) );
		}
		if ( pos < 0 )
		{
			this->Insert( sequence );
			break;
		}

		beginSearch = pos + 1;

		if ( beginSearch < sequence.length() )
		{
			pos = sequence.find( delim, beginSearch );
		}
		else
		{
			pos = -1;
		}
	}
}

//----------------------------------------
void CStringList::Dump(std::ostream& fOut /* = std::cerr */) const
{

  if (CTrace::IsTrace() == false)
   {
      return;
   }

   CStringList::const_iterator it;
   int i = 0;

   fOut << "==> Dump a CStringList Object at "<< this << " with " <<  size() << " elements" << std::endl;

   for ( it = this->begin( ); it != this->end( ); it++ )
   {
     fOut << "CStringList[" << i << "]= " << *it << std::endl;  
     i++;
   }
    
   fOut << "==> END Dump a CStringList Object at "<< this << " with " <<  size() << " elements" << std::endl;

 
   fOut << std::endl;
   

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
void CIntList::Dump(std::ostream& fOut /* = std::cerr */) const
{

  if (CTrace::IsTrace() == false)
   {
      return;
   }

   CIntList::const_iterator it;
   int i = 0;

   fOut << "==> Dump a CIntList Object at "<< this << " with " <<  size() << " elements" << std::endl;

   for ( it = this->begin( ); it != this->end( ); it++ )
   {
     fOut << "CIntList[" << i << "]= " << *it << std::endl;  
     i++;
   }
    
   fOut << "==> END Dump a CIntList Object at "<< this << " with " <<  size() << " elements" << std::endl;

 
   fOut << std::endl;
   

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
void CObList::Dump(std::ostream& fOut /* = std::cerr */) const
{

  if (CTrace::IsTrace() == false)
  {
    return;
  }

  CObList::const_iterator it;
  int i = 0;

  fOut << "==> Dump a CObList Object at " << this << " with " <<  size() << " elements" << std::endl;

  for ( it = this->begin( ); it != this->end( ); it++ )
  {
    fOut << "CObList[" << i << "]= " << std::endl;  
    if ((*it) != NULL)
    {
      (*it)->Dump(fOut);
    }
    i++;
  }

  fOut << "==> END Dump a CObList Object at " << this << " with " <<  size() << " elements" << std::endl;


  fOut << std::endl;
   

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
  size_t dims = GetMatrixNumberOfDims();

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
void CDoublePtrArray::Dump(std::ostream& fOut /* = std::cerr */) const
{

  if (CTrace::IsTrace() == false)
  {
    return;
  }

  CDoublePtrArray::const_iterator it;
  int i = 0;

  fOut << "==> Dump a CDoublePtrArray Object at "<< this << " with " <<  size() << " elements" << std::endl;

  for ( it = this->begin( ); it != this->end( ); it++ )
  {
    DoublePtr matrix = *it;
    if ( matrix != NULL)
    {
      uint32_t nbValues = m_matrixDims.GetProductValues();

      for (uint32_t i = 0 ; i < nbValues ; i++)
      {
        fOut << "CDoublePtrArray[" << i << "]= " << matrix[i] << std::endl;
      }
    }
    else
    {
      fOut << "NULL";
    }

    i++;
  }

  fOut << "==> END Dump a CDoublePtrArray Object at "<< this << " with " <<  size() << " elements" << std::endl;


  fOut << std::endl;

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

  setDefaultValue(m_minValue);
  setDefaultValue(m_maxValue);
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

  size_t nRows = this->size();
  size_t nCols = this->at(0).size();

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
  if (isDefaultValue(value))
  {
    return;
  }

  if (isDefaultValue(m_minValue))
  {
    m_minValue = value;
  }
  else if (m_minValue > value)
  {
    m_minValue = value;
  }

  if (isDefaultValue(m_maxValue))
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

  size_t nRows = a.size();
  size_t nCols = a.at(0).size();
  
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

  size_t nRows = this->size();
  size_t nCols = this->at(0).size();

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
  size_t dims = GetMatrixNumberOfDims();

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
  catch (std::bad_alloc& e) // If memory allocation (new) failed...
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
void CArrayDoublePtrArray::Dump(std::ostream& fOut /* = std::cerr */) const
{

  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CArrayDoublePtrArray Object at "<< this << " with " <<  size() << " elements" << std::endl;

  for ( uint32_t row = 0 ; row < this->size() ; row++ )
  {

    for ( uint32_t col = 0 ; col < this->at(row).size() ; col++ )
    {

      fOut << "CArrayDoublePtrArray[" << row << "," << col << "]=" << std::endl;

      DoublePtr matrix = this->at(row).at(col);
      if ( matrix != NULL)
      {
        uint32_t nbValues = m_matrixDims.GetProductValues();

        for (uint32_t i = 0 ; i < nbValues ; i++)
        {
          fOut << "double array[" << i << "]= " << matrix[i] << std::endl;
        }
      }
      else
      {
        fOut << "NULL";
      }

      fOut << std::endl;

    }     
  }
     
  fOut << "==> END Dump a CArrayDoublePtrArray Object at "<< this << " with " <<  size() << " elements" << std::endl;


  fOut << std::endl;
   

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
  setDefaultValue(m_minValue);
  setDefaultValue(m_maxValue);
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

  size_t nRows = this->size();
  size_t nCols = this->at(0).size();

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
  if (isDefaultValue(value))
  {
    return;
  }

  if (isDefaultValue(m_minValue))
  {
    m_minValue = value;
  }
  else if (m_minValue > value)
  {
    m_minValue = value;
  }

  if (isDefaultValue(m_maxValue))
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

  size_t nRows = a.size();
  size_t nCols = a.at(0).size();
  
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

  size_t nRows = this->size();
  size_t nCols = this->at(0).size();

  for (uint32_t xPos = 0 ; xPos < nRows ; xPos++)
  {
    for (uint32_t yPos = 0 ; yPos < nCols ; yPos++)
    {
      (*this)[xPos][yPos] = initialValue;
    }
  }
}


//----------------------------------------
void CArrayDoubleArray::Dump(std::ostream& fOut /* = std::cerr */) const
{

  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CArrayDoubleArray Object at "<< this << " with " <<  this->size() << " rows" << std::endl;

  for ( uint32_t row = 0 ; row < this->size() ; row++ )
  {
    fOut << "==> CArrayDoubleArray Object at row " << row << " with " <<   this->at(row).size() << " cols" << std::endl;
    for ( uint32_t col = 0 ; col < this->at(row).size() ; col++ )
    {
      fOut << "CArrayDoubleArray[" << row << "," << col << "]=" << this->at(row).at(col) << std::endl;
    }     
  }
     
  fOut << "==> END Dump a CArrayDoubleArray Object at "<< this << " with " <<  this->size()  << " rows" << std::endl;


  fOut << std::endl;
   

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
CStringArray* CArrayStringMap::Insert(const std::string& key, const CStringArray& array, bool withExcept /* = true */)
{
  

  std::pair <maparraystring::iterator,bool> pairInsert;
  //If 'key' already exists --> pairInsert.second == false and
  // pairInsert.first then contains an iterator on the existing object
  // If 'key' does not exist --> pairInsert.second == true and
  // pairInsert.first then contains a iterator on the inserted object

  pairInsert = (maparraystring::insert(CArrayStringMap::value_type(key, array)));

  if( (pairInsert.second == false) && (withExcept))
  {
    CException e("ERROR in CStringMap::Insert - try to insert a std::string that already exists and parameter withExcept is true", BRATHL_LOGIC_ERROR);
    throw(e);
  }
  
  CArrayStringMap::iterator it = (pairInsert.first);

  return &((*it).second);

}

//----------------------------------------

const CStringArray* CArrayStringMap::Exists(const std::string& key) const
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


bool CArrayStringMap::Erase(const std::string& key)
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
void CArrayStringMap::Dump(std::ostream& fOut /* = std::cerr */) const
{

  if (CTrace::IsTrace() == false)
  {
    return;
  }

   fOut << "==> Dump a CArrayStringMap Object at "<< this << " with " <<  size() << " elements" << std::endl;
 
   CArrayStringMap::const_iterator it;

   for (it = this->begin() ; it != this->end() ; it++)
   {
      fOut << "CArrayStringMap Key is = " << (*it).first << std::endl;
      fOut << "CArrayStringMap Value is = " <<  std::endl;

      (it->second).Dump(fOut);
   }

   fOut << "==> END Dump a CArrayStringMap Object at "<< this << " with " <<  size() << " elements" << std::endl;
  

  fOut << std::endl;
   

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
void CMatrix::Dump(std::ostream& fOut /* = std::cerr */) const
{

  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CMatrix Object at "<< this
       << " with " <<  GetNumberOfRows() << " rows and " <<  GetNumberOfCols() << " columns" << std::endl;

  fOut << "m_name = "<< m_name << std::endl;
  fOut << "m_xName = "<< m_xName << std::endl;
  fOut << "m_yName = "<< m_yName << std::endl;

  fOut << "==> END Dump a CDoublePtrArray Object at "<< this
       << " with " <<  GetNumberOfRows() << " rows  and " <<  GetNumberOfCols() << " columns" << std::endl;


  fOut << std::endl;

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
  size_t nRows = this->GetNumberOfRows();
  size_t nCols = this->GetNumberOfCols();

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

        if (isDefaultValue(value))
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
  size_t nRows = this->GetNumberOfRows();
  size_t nCols = this->GetNumberOfCols();

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
void CMatrixDoublePtr::Dump(std::ostream& fOut /* = std::cerr */) const
{

  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CMatrixDoublePtr Object at "<< this
       << " with " <<  GetNumberOfRows() << " rows and " <<  GetNumberOfCols() << " columns" << std::endl;

  CMatrix::Dump(fOut);

  m_data.Dump(fOut);

  fOut << "==> END Dump a CDoublePtrArray Object at "<< this
       << " with " <<  GetNumberOfRows() << " rows  and " <<  GetNumberOfCols() << " columns" << std::endl;



  fOut << std::endl;

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
  size_t nRows = this->GetNumberOfRows();
  size_t nCols = this->GetNumberOfCols();

  for ( uint32_t row = 0 ; row < nRows ; row++ )
  {
    for ( uint32_t col = 0 ; col < nCols ; col++ )
    {
      double value = *(this->At(row, col));

      if (isDefaultValue(value))
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
  size_t nRows = this->GetNumberOfRows();
  size_t nCols = this->GetNumberOfCols();

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
void CMatrixDouble::Dump(std::ostream& fOut /* = std::cerr */) const
{

  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CMatrixDouble Object at "<< this
       << " with " <<  GetNumberOfRows() << " rows and " <<  GetNumberOfCols() << " columns" << std::endl;

  CMatrix::Dump(fOut);

  m_data.Dump(fOut);

  fOut << "==> END Dump a CDoublePtrArray Object at "<< this
       << " with " <<  GetNumberOfRows() << " rows  and " <<  GetNumberOfCols() << " columns" << std::endl;



  fOut << std::endl;

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

void CDoubleArrayOb::Dump(std::ostream& fOut /* = std::cerr */) const
{

  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CDoubleArrayOb Object at "<< this << " with " <<  size() << " elements" << std::endl;
  
  CDoubleArray::Dump(fOut);

  fOut << "==> END Dump a CDoubleArrayOb Object at "<< this << " with " <<  size() << " elements" << std::endl;


  fOut << std::endl;
   

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
void CObArray::Dump(std::ostream& fOut /* = std::cerr */) const
{

  if (CTrace::IsTrace() == false)
  {
    return;
  }

  CObArray::const_iterator it;
  int i = 0;

  fOut << "==> Dump a CObArray Object at "<< this << " with " <<  size() << " elements" << std::endl;

  for ( it = this->begin( ); it != this->end( ); it++ )
  {
    fOut << "CObArray[" << i << "]= " << std::endl;  
    if ((*it) != NULL)
    {
     (*it)->Dump(fOut);
    }
    i++;
  }

  fOut << "==> END Dump a CObArray Object at "<< this << " with " <<  size() << " elements" << std::endl;


  fOut << std::endl;


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
void CObArrayOb::Dump(std::ostream& fOut /* = std::cerr */) const
{

  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CObArrayOb Object at "<< this << " with " <<  size() << " elements" << std::endl;

  CObArray::Dump(fOut);

  fOut << "==> END Dump a CObArrayOb Object at "<< this << " with " <<  size() << " elements" << std::endl;


  fOut << std::endl;


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
std::string CStringMap::Insert(const std::string& key, const std::string& str, bool withExcept /* = true */)
{
  

  std::pair <mapstring::iterator,bool> pairInsert;
  //If 'key' already exists --> pairInsert.second == false and
  // pairInsert.first then contains an iterator on the existing object
  // If 'key' does not exist --> pairInsert.second == true and
  // pairInsert.first then contains a iterator on the inserted object

  pairInsert = (mapstring::insert(CStringMap::value_type(key, str)));

  if( (pairInsert.second == false) && (withExcept))
  {
    CException e("ERROR in CStringMap::Insert - try to insert a std::string that already exists and parameter withExcept is true", BRATHL_LOGIC_ERROR);
    throw(e);
  }
  
  CStringMap::iterator it = (pairInsert.first);

  return (*it).second;

}

//----------------------------------------

std::string CStringMap::Exists(const std::string& key) const
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
std::string CStringMap::IsValue(const std::string& value)
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

  std::string str = it->second;
  if (str.empty() == false)
  {
    bOk = true;
    mapstring::erase(it);
  }

  return bOk;

}
//----------------------------------------


bool CStringMap::Erase(const std::string& key)
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


void CStringMap::Dump(std::ostream& fOut /* = std::cerr */) const
{

   if (CTrace::IsTrace() == false)
   {
      return;
   }

   fOut << "==> Dump a CStringMap Object at "<< this << " with " <<  size() << " elements" << std::endl;
 
   CStringMap::const_iterator it;

   for (it = this->begin() ; it != this->end() ; it++)
   {
      std::string str = it->second;
      fOut << "CStringMap Key is = " << (*it).first << std::endl;
      fOut << "CStringMap Value is = " << (*it).second << std::endl;
   }

   fOut << "==> END Dump a CStringMap Object at "<< this << " with " <<  size() << " elements" << std::endl;
  
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

int32_t CIntMap::Insert(const std::string& key, int32_t value, bool withExcept /* = true */)
{
  

  std::pair <mapint::iterator,bool> pairInsert;
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

int32_t CIntMap::Exists(const std::string& key) const
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
  size_t nbKeys = keys.size();
  size_t nbValues = values.size();


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
  if (isDefaultValue(value) == false)
  {
    bOk = true;
    mapint::erase(it);
  }

  return bOk;

}
//----------------------------------------


bool CIntMap::Erase(const std::string& key)
{

  CIntMap::iterator it;

  it = mapint::find(key);

  return Erase(it);
  
}

//----------------------------------------

int32_t CIntMap::operator[](const std::string& key)
{

  //int32_t value = mapint::operator[](key);
  return Exists(key);

}

//----------------------------------------


void CIntMap::Dump(std::ostream& fOut /* = std::cerr */) const 
{

   if (CTrace::IsTrace() == false)
   {
      return;
   }

   fOut << "==> Dump a CIntMap Object at "<< this << " with " <<  size() << " elements" << std::endl;
 
   CIntMap::const_iterator it;

   for (it = this->begin() ; it != this->end() ; it++)
   {
      fOut << "CIntMap Key is = " << (*it).first << std::endl;
      fOut << "CIntMap Value is = " << (*it).second << std::endl;
   }

   fOut << "==> END Dump a CIntMap Object at "<< this << " with " <<  size() << " elements" << std::endl;
  
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
  size_t nbKeys = keys.size();
  size_t nbValues = values.size();


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

  size_t nbKeys = keys.size();

  for (uint32_t i = 0 ; i < nbKeys ; i++)
  {
    Insert(keys[i], i, withExcept);
  }

}

//----------------------------------------

uint32_t CUIntMap::Insert(const std::string& key, uint32_t value, bool withExcept /* = true */)
{
  

  std::pair <mapuint::iterator,bool> pairInsert;
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

uint32_t CUIntMap::Exists(const std::string& key) const
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
  if (isDefaultValue(value) == false)
  {
    bOk = true;
    mapuint::erase(it);
  }

  return bOk;

}
//----------------------------------------


bool CUIntMap::Erase(const std::string& key)
{

  CUIntMap::iterator it;

  it = mapuint::find(key);

  return Erase(it);
  
}

//----------------------------------------

uint32_t CUIntMap::operator[](const std::string& key)
{

  //uint32_t value = mapuint::operator[](key);
  return Exists(key);

}

//----------------------------------------


void CUIntMap::Dump(std::ostream& fOut /* = std::cerr */) const 
{

   if (CTrace::IsTrace() == false)
   {
      return;
   }

   fOut << "==> Dump a CUIntMap Object at "<< this << " with " <<  size() << " elements" << std::endl;
 
   CUIntMap::const_iterator it;

   for (it = this->begin() ; it != this->end() ; it++)
   {
      fOut << "CUIntMap Key is = " << (*it).first << std::endl;
      fOut << "CUIntMap Value is = " << (*it).second << std::endl;
   }

   fOut << "==> END Dump a CUIntMap Object at "<< this << " with " <<  size() << " elements" << std::endl;
  
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

double CDoubleMap::Insert(const std::string& key, double value, bool withExcept /* = true */)
{
  

  std::pair <mapdouble::iterator,bool> pairInsert;
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

double CDoubleMap::Exists(const std::string& key) const
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
  if (isDefaultValue(value) == false)
  {
    bOk = true;
    mapdouble::erase(it);
  }

  return bOk;

}
//----------------------------------------


bool CDoubleMap::Erase(const std::string& key)
{

  CDoubleMap::iterator it;

  it = mapdouble::find(key);

  return Erase(it);
  
}

//----------------------------------------

double CDoubleMap::operator[](const std::string& key)
{

  //double value = mapdouble::operator[](key);
  return Exists(key);

}

//----------------------------------------


void CDoubleMap::Dump(std::ostream& fOut /* = std::cerr */) const 
{

   if (CTrace::IsTrace() == false)
   {
      return;
   }

   fOut << "==> Dump a CDoubleMap Object at "<< this << " with " <<  size() << " elements" << std::endl;
 
   CDoubleMap::const_iterator it;

   for (it = this->begin() ; it != this->end() ; it++)
   {
      fOut << "CDoubleMap Key is = " << (*it).first << std::endl;
      fOut << "CDoubleMap Value is = " << CTools::Format(DUMP_FORMAT_DOUBLE, (*it).second) << std::endl;
   }

   fOut << "==> END Dump a CDoubleMap Object at "<< this << " with " <<  size() << " elements" << std::endl;
  
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

CBratObject* CObMap::Insert(const std::string& key, CBratObject* ob, bool withExcept /* = true */)
{
  

  std::pair <CObMap::iterator,bool> pairInsert;


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

CBratObject* CObMap::Exists(const std::string& key) const
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

bool CObMap::RenameKey(const std::string& oldKey, const std::string& newKey)
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


bool CObMap::Erase(const std::string& key)
{

  CObMap::iterator it = mapobject::find(key);

  return Erase(it);
  
}

//----------------------------------------

CBratObject* CObMap::operator[](const std::string& key)
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


void CObMap::Dump(std::ostream& fOut /* = std::cerr */) const 
{

  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CObMap Object at "<< this << " with " <<  size() << " elements" << std::endl;

  CObMap::const_iterator it;

  for (it = this->begin() ; it != this->end() ; it++)
  {
    CBratObject *ob = it->second;
    fOut << "CObMap Key is = " << (*it).first << std::endl;
    fOut << "CObMap Value is = " << std::endl;
    if ( ob != NULL)
    {
      ob->Dump(fOut);
    }
  }

  fOut << "==> END Dump a CObMap Object at "<< this << " with " <<  size() << " elements" << std::endl;
  
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
  

  std::pair <CObIntMap::iterator,bool> pairInsert;


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


void CObIntMap::Dump(std::ostream& fOut /* = std::cerr */) const 
{

  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CObIntMap Object at "<< this << " with " <<  size() << " elements" << std::endl;

  CObIntMap::const_iterator it;

  for (it = this->begin() ; it != this->end() ; it++)
  {
    CBratObject *ob = it->second;
    fOut << "CObIntMap Key is = " << (*it).first << std::endl;
    fOut << "CObIntMap Value is = " << std::endl;
    if ( ob != NULL)
    {
      ob->Dump(fOut);
    }
  }

   fOut << "==> END Dump a CObIntMap Object at "<< this << " with " <<  size() << " elements" << std::endl;
  
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
  

  std::pair <CObDoubleMap::iterator,bool> pairInsert;


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


void CObDoubleMap::Dump(std::ostream& fOut /* = std::cerr */) const 
{

  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CObDoubleMap Object at "<< this << " with " <<  size() << " elements" << std::endl;

  CObDoubleMap::const_iterator it;

  for (it = this->begin() ; it != this->end() ; it++)
  {
    CBratObject *ob = it->second;
    fOut << "CObDoubleMap Key is = " << CTools::Format(DUMP_FORMAT_DOUBLE, (*it).first) << std::endl;
    fOut << "CObDoubleMap Value is = " << std::endl;
    if ( ob != NULL)
    {
      ob->Dump(fOut);
    }
  }

  fOut << "==> END Dump a CObDoubleMap Object at "<< this << " with " <<  size() << " elements" << std::endl;
  
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
  size_t rows = GetMatrixNumberOfRows();

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
  catch (std::bad_alloc& e) // If memory allocation (new) failed...
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
  

  std::pair <CDoublePtrDoubleMap::iterator,bool> pairInsert;


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
  for (size_t i = GetMatrixNumberOfRows() ; i > 0; --i)
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


void CDoublePtrDoubleMap::Dump(std::ostream& fOut /* = std::cerr */) const 
{

  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CDoublePtrDoubleMap Object at "<< this << " with " <<  size() << " elements" << std::endl;

  CDoublePtrDoubleMap::const_iterator it;

  for (it = this->begin() ; it != this->end() ; it++)
  {
    DoublePtr* matrix = it->second;
    fOut << "CDoublePtrDoubleMap Key is = " << CTools::Format(DUMP_FORMAT_DOUBLE, (*it).first) << std::endl;
    fOut << "CDoublePtrDoubleMap Value is = " << std::endl;
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

       fOut << std::endl;
     }
    }
  }

  fOut << "==> END Dump a CDoublePtrDoubleMap Object at "<< this << " with " <<  size() << " elements" << std::endl;
  
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

void* CPtrMap::Insert(const std::string& key, void* ptr, bool withExcept /* = true */)
{
  

  std::pair <CPtrMap::iterator,bool> pairInsert;


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

void* CPtrMap::Exists(const std::string& key) const
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


bool CPtrMap::Erase(const std::string& key)
{

  CPtrMap::iterator it;

  it = mapptr::find(key);

  return Erase(it);
  
}

//----------------------------------------

void* CPtrMap::operator[](const std::string& key)
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


void CPtrMap::Dump(std::ostream& fOut /* = std::cerr */) const 
{

   if (CTrace::IsTrace() == false)
   {
      return;
   }

   fOut << "==> Dump a CPtrMap Object at "<< this << " with " <<  size() << " elements" << std::endl;
 
   CPtrMap::const_iterator it;

   for (it = this->begin() ; it != this->end() ; it++)
   {
      fOut << "CPtrMap Key is = " << (*it).first << std::endl;
      fOut << "CPtrMap Value is = " << (*it).second << std::endl;
   }

   fOut << "==> END Dump a (*it).first <<  Object at "<< this << " with " <<  size() << " elements" << std::endl;
  
}


} // end namespace
