#include "stdafx.h"

#include "new-gui/Common/tools/Trace.h"
#include "new-gui/Common/tools/Exception.h"
#include "CoreTypes.h"


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//						Numeric Array Types
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


const std::streamsize floatPrecision = 15;


//-------------------------------------------------------------
//------------------- CStringArray class --------------------
//-------------------------------------------------------------

////----------------------------------------
//CStringArray::CStringArray(const CStringList& lst)
//{
//  this->Insert(lst);  
//}
////----------------------------------------
//CStringArray::CStringArray(const stringlist& lst)
//{
//  this->Insert(lst);  
//}

//----------------------------------------
const CStringArray& CStringArray::operator =(const CStringArray& vect)
{
  
  this->clear();

  Insert(vect);
    
  return *this;    
}

////----------------------------------------
//const CStringArray& CStringArray::operator =(const CStringList& lst)
//{  
//  this->clear();
//
//  Insert(lst);
//    
//  return *this;    
//}
//----------------------------------------

const CStringArray& CStringArray::operator =(const stringarray& vect)
{
  
  this->clear();

  Insert(vect);
    
  return *this;

    
}

//----------------------------------------

const CStringArray& CStringArray::operator =( const std::list<std::string> & lst )
{
	this->clear();

	Insert( lst );

	return *this;
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

void CStringArray::Insert(const std::string& str)
{
  
stringarray::push_back(str);    
  
}

////----------------------------------------
//void CStringArray::Insert( const CStringList& lst )
//{
//	CStringList::const_iterator it;
//
//	for ( it = lst.begin(); it != lst.end(); it++ )
//	{
//		this->Insert( *it );
//	}
//}

//----------------------------------------
void CStringArray::Insert( const CIntArray& vect )
{
	for ( auto it = vect.begin(); it != vect.end(); it++ )
	{
		std::string str = n2s<std::string>( *it );
		if ( !( str.empty() ) )
		{
			this->Insert( str );
		}
	}
}

//----------------------------------------

void CStringArray::Insert( const std::list<std::string> & lst )
{
	std::list<std::string>::const_iterator it;

	for ( it = lst.begin(); it != lst.end(); it++ )
	{
		this->Insert( *it );
	}
}

//----------------------------------------
CStringArray::iterator CStringArray::InsertAt(CStringArray::iterator where, const std::string& str)
{ 
  return stringarray::insert(where, str);     
}

//----------------------------------------
CStringArray::iterator CStringArray::InsertAt(int32_t index, const std::string& str)
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
CStringArray::iterator CStringArray::ReplaceAt(uint32_t index, const std::string& str)
{
  (*this)[index] = str;
  return this->begin() + index;
}
//----------------------------------------
CStringArray::iterator CStringArray::ReplaceAt(int32_t index, const std::string& str)
{
  if (index < 0)
  {
    return this->end();
  }

  (*this)[index] = str;
  return this->begin() + index;

}
//----------------------------------------
CStringArray::iterator CStringArray::ReplaceAt(CStringArray::iterator where, const std::string& str)
{
  Erase(where);
  return InsertAt(where, str);     
}

//----------------------------------------
bool CStringArray::Exists(const std::string& str, bool compareNoCase /* = false */) const
{
  CStringArray::const_iterator it;

  for ( it = this->begin( ); it != this->end( ); it++ )
  {
    if (compareNoCase)
    {
      if (str_icmp( str, *it))
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
int32_t CStringArray::FindIndex(const std::string& str, bool compareNoCase /* = false */) const
{
  for (uint32_t i = 0 ; i < size() ; i++ )
  {
    if (compareNoCase)
    {
      if (str_icmp( str, this->at(i)))
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
void CStringArray::FindIndexes(const std::string& str, CIntArray& indexes, bool compareNoCase /* = false */) const
{
  for (uint32_t i = 0 ; i < size() ; i++ )
  {
    if (compareNoCase)
    {
      if (str_icmp( str, this->at(i)))
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
void CStringArray::GetValues( const CIntArray& indexes, CStringArray& values ) const
{
	const size_t maxIndex =  this->size() - 1;

	for ( auto it = indexes.begin(); it != indexes.end(); it++ )
	{
		if ( *it <= maxIndex )
			values.Insert( this->at( *it ) );
	}
}

//----------------------------------------
void CStringArray::GetValues(const CIntArray& indexes, std::string& values) const
{
  CStringArray arrayValues;

  GetValues(indexes, arrayValues);

  values = arrayValues.ToString("\n", false);
}

//----------------------------------------
bool CStringArray::Intersect(const std::string& str, CStringArray& intersect, bool compareNoCase /* = false */) const
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
        same = str_icmp( *itA, *itB );
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
bool CStringArray::Intersect(const std::string& str, CUIntArray& intersect, bool compareNoCase /* = false */) const
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
        same = str_icmp( this->at(iA), array.at(iB) );
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
bool CStringArray::Remove(const std::string& str, bool compareNoCase /* = false */)
{
  CStringArray array;
  array.Insert(str);
  return Remove(array, compareNoCase);
}
//----------------------------------------
bool CStringArray::Remove( const CStringArray& array, bool compareNoCase /* = false */ )
{
	CUIntArray intersect;

	bool bOk = Intersect( array, intersect, compareNoCase );
	if ( bOk )
	{
        for ( long i = intersect.size() - 1; i >= 0; i-- )
		{
			this->Erase( intersect.at( i ) );
		}
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
void CStringArray::Replace(const std::string& findString, const std::string& replaceBy, CStringArray& replaced, bool compareNoCase /* = false */, bool insertUnique /* = false */) const
{ 
  CStringArray findVect;
  findVect.Insert(findString);

  Replace(findVect, replaceBy, replaced, compareNoCase, insertUnique);

}

//----------------------------------------

std::string StringReplace( const std::string& str, const std::string& c, const std::string& replaceBy, bool compareNoCase = false )
{
	std::string strRet = str;

    //RCCC: pos is alwyas positive, etc, etc.

	auto pos = compareNoCase ? i_find( str, c ) : strRet.find( c );
	while ( pos >= 0 )
	{
		strRet.replace( pos, c.length(), replaceBy );
		if ( compareNoCase )
		{
			pos = i_find( str, c, pos + replaceBy.length() );
		}
		else
		{
			pos = strRet.find( c, pos + replaceBy.length() );
		}
	}

	return strRet;
}

void CStringArray::Replace( const CStringArray& findString, const std::string& replaceBy, CStringArray& replaced, bool compareNoCase /* = false */, bool insertUnique /* = false */ ) const
{
	if ( this == &replaced )
	{
		throw CException(
			"ERROR - CStringArray::Replace : this object (at memory address "
			+ hn2s<std::string>( ( void* )this )
			+ ") and parameter 'CStringArray& replaced' (at memory address "
			+ hn2s<std::string>( (void*)&replaced )
			+ ") are same objects. They have to be different.",
			BRATHL_LOGIC_ERROR );
	}

	replaced.RemoveAll();

	CStringArray findStringTmp( findString );

	//bool removed = findStringTmp.Remove(replaceBy, compareNoCase);

	if ( findStringTmp.size() <= 0 )
	{
		replaced.Insert( *this );
		return;
	}

	std::string thisStr = this->ToString( ",", false );
	std::string findStringStr = findStringTmp.ToString( ",", false );

	thisStr = StringReplace( thisStr, findStringStr, replaceBy, compareNoCase );

	replaced.ExtractStrings( thisStr, ",", true, insertUnique );
}
//----------------------------------------
void CStringArray::ExtractKeys( const std::string& str, const std::string& delim, bool bRemoveAll /* = true */ )
{
	if ( bRemoveAll )
		this->clear();

	std::string sequence = str;

	auto pos = (long)sequence.find( delim, 0 );

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
			pos = (long)sequence.find( delim, beginSearch );
		}
		else
		{
			pos = -1;
		}
	}
}
//----------------------------------------

void CStringArray::ExtractStrings( const std::string& str, const char delim, bool bRemoveAll /* = true */, bool insertUnique /* = false */ )
{
	if ( bRemoveAll )
		this->clear();

	std::string sequence = str;

	long pos = (long)sequence.find( delim );

	size_t nLg;

	while ( sequence.empty() == false )
	{

		if ( pos > 0 )
		{
			if ( insertUnique )
			{
				this->InsertUnique( sequence.substr( 0, pos ) );
			}
			else
			{
				this->Insert( sequence.substr( 0, pos ) );
			}

		}
		if ( pos < 0 )
		{
			if ( insertUnique )
			{
				this->InsertUnique( sequence );
			}
			else
			{
				this->Insert( sequence );
			}
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

		pos = (long)sequence.find( delim );
	}
}


//----------------------------------------

void CStringArray::ExtractStrings( const std::string& str, const std::string& delim, bool bRemoveAll /* = true */, bool insertUnique /* = false */ )
{
	if ( bRemoveAll )
		this->clear();

	std::string sequence = str;

	long pos = (long)sequence.find( delim );

	size_t nLg;

	while ( sequence.empty() == false )
	{

		if ( pos > 0 )
		{
			if ( insertUnique )
			{
				this->InsertUnique( sequence.substr( 0, pos ) );
			}
			else
			{
				this->Insert( sequence.substr( 0, pos ) );
			}
		}
		if ( pos < 0 )
		{
			if ( insertUnique )
			{
				this->InsertUnique( sequence );
			}
			else
			{
				this->Insert( sequence );
			}

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

		pos = (long)sequence.find( delim );
	}
}

//----------------------------------------

void CStringArray::InsertUnique(const std::string& str)
{
  if (this->Exists(str))
  {
    return;
  }

  this->Insert(str);
  
  return;  
}

////----------------------------------------
//void CStringArray::InsertUnique( const CStringList& lst )
//{
//	CStringList::const_iterator it;
//
//	for ( it = lst.begin(); it != lst.end(); it++ )
//	{
//		this->InsertUnique( *it );
//	}
//}

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
void CStringArray::InsertUnique( const stringarray& vect )
{
	for ( auto it = vect.begin(); it != vect.end(); it++ )
	{
		this->InsertUnique( *it );
	}
}

//----------------------------------------
void CStringArray::InsertUnique( const std::list<std::string> & lst )
{
	for ( auto it = lst.begin(); it != lst.end(); it++ )
	{
		this->InsertUnique( *it );
	}
}

//----------------------------------------
std::string CStringArray::ToString(const std::string& delim /*= "," */, bool useBracket /*= true */) const
{
  std::ostringstream myStream;

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
void CStringArray::Dump(std::ostream& fOut /* = std::cerr */) const
{

  if (CTrace::IsTrace() == false)
   {
      return;
   }

   CStringArray::const_iterator it;
   int i = 0;

   fOut << "==> Dump a CStringArray Object at "<< this << " with " <<  size() << " elements" << std::endl;

   for ( it = this->begin( ); it != this->end( ); it++ )
   {
     fOut << "CStringArray[" << i << "]= " << *it << std::endl;  
     i++;
   }
    
   fOut << "==> END Dump a CStringArray Object at "<< this << " with " <<  size() << " elements" << std::endl;

 
   fOut << std::endl;
   

}



//-------------------------------------------------------------
//------------------- CIntArray class --------------------
//-------------------------------------------------------------
CIntArray::iterator CIntArray::InsertAt( int32_t index, const int32_t value )
{
	assert__( index > -1 && index < size() );	//if it doesn't assert (exhaustive use cases), we can get rid of conditions below

	if ( index < 0 )
		return this->end();

	if ( (uint32_t)index >= this->size() )
	{
		this->resize( index + 1 );
		( *this )[ index ] = value;
		return this->begin() + index;
	}

	return int32array::insert( this->begin() + index, value );
}


CIntArray::iterator CIntArray::ReplaceAt( int32_t index, const int32_t value )
{
	assert__( index > -1 && index < size() );	//if it doesn't assert (exhaustive use cases), we can get rid of condition below

	if ( index < 0 )
		return this->end();

	//what if greater than size???

	( *this )[ index ] = value;
	return this->begin() + index;
}


bool CIntArray::Intersect( const CIntArray& array, CIntArray& intersect ) const
{
	intersect.clear();

	CIntArray::const_iterator itA;
	CIntArray::const_iterator itB;

	for ( itA = this->begin(); itA != this->end(); itA++ )
	{
		for ( itB = array.begin(); itB != array.end(); itB++ )
		{
			if ( ( *itA ) == ( *itB ) )
			{
				intersect.Insert( ( *itB ) );
			}
		}
	}

	return ( !( intersect.empty() ) );
}


std::string CIntArray::ToString( const std::string& delim /*= "," */, bool useBracket /*= true */ ) const
{
	std::ostringstream myStream;

	if ( useBracket )
	{
		myStream << "[";
	}

	for ( auto it = this->begin(); it != this->end(); it++ )
	{
		myStream << *it;
		if ( ( it + 1 ) != this->end() )
		{
			myStream << delim;
		}
	}
	if ( useBracket )
	{
		myStream << "]";
	}

	return myStream.str();
}


void CIntArray::Dump( std::ostream& fOut ) const	//fOut = std::cerr
{
	if ( CTrace::IsTrace() == false )
	{
		return;
	}

	CIntArray::const_iterator it;
	int i = 0;

	fOut << "==> Dump a CIntArray Object at " << this << " with " << size() << " elements" << std::endl;

	for ( it = this->begin(); it != this->end(); it++ )
	{
		fOut << "CIntArray[" << i << "]= " << *it << std::endl;
		i++;
	}

	fOut << "==> END Dump a CIntArray Object at " << this << " with " << size() << " elements" << std::endl;

	fOut << std::endl;
}





//-------------------------------------------------------------
//------------------- CInt64Array class --------------------
//-------------------------------------------------------------

//----------------------------------------
bool CInt64Array::operator ==( const CInt64Array& vect )
{
	size_t count = this->size();

	if ( count != vect.size() )
	{
		return false;
	}

	if ( count == 0 )
	{
		return true;
	}

	bool isEqual = true;

	for ( size_t i = 0; i < count; i++ )
	{
		if ( this->at( i ) != vect.at( i ) )
		{
			isEqual = false;
			break;
		}
	}

	return isEqual;
}
//----------------------------------------
const CInt64Array& CInt64Array::operator =( const CInt64Array& vect )
{

	this->clear();

	Insert( vect );

	return *this;


}
//----------------------------------------
void CInt64Array::Insert( const CInt64Array& vect, bool bEnd /*= true*/ )
{

	if ( vect.empty() )
	{
		return;
	}

	if ( bEnd )
	{
		int64array::insert( this->end(), vect.begin(), vect.end() );
	}
	else
	{
		int64array::insert( this->begin(), vect.begin(), vect.end() );
	}


}

//----------------------------------------
void CInt64Array::Insert( int64_t* vect, size_t length )
{
	for ( size_t i = 0; i < length; i++ )
	{
		Insert( vect[ i ] );
	}
}

//----------------------------------------

void CInt64Array::Insert( const int64_t value )
{

	int64array::push_back( value );

}

//----------------------------------------
CInt64Array::iterator CInt64Array::InsertAt( CInt64Array::iterator where, const int64_t value )
{
	return int64array::insert( where, value );
}
//----------------------------------------
CInt64Array::iterator CInt64Array::InsertAt( size_t index, const int64_t value )
{
	if ( index < 0 )
	{
		return this->end();
	}

	if ( index >= this->size() )
	{
		this->resize( index + 1 );
		( *this )[ index ] = value;
		return this->begin() + index;
	}

	return int64array::insert( this->begin() + index, value );
}


//----------------------------------------
bool CInt64Array::Erase( CInt64Array::iterator where )
{
	int64array::erase( where );
	return true;
}
//----------------------------------------
CInt64Array::iterator CInt64Array::ReplaceAt( size_t index, const int64_t value )
{
	( *this )[ index ] = value;
	return this->begin() + index;
}
//----------------------------------------
CInt64Array::iterator CInt64Array::ReplaceAt( CInt64Array::iterator where, const int64_t value )
{
	Erase( where );
	return InsertAt( where, value );
}

//----------------------------------------
bool CInt64Array::Intersect( const CInt64Array& array, CInt64Array& intersect ) const
{
	intersect.clear();

	CInt64Array::const_iterator itA;
	CInt64Array::const_iterator itB;

	for ( itA = this->begin(); itA != this->end(); itA++ )
	{
		for ( itB = array.begin(); itB != array.end(); itB++ )
		{
			if ( ( *itA ) == ( *itB ) )
			{
				intersect.Insert( ( *itB ) );
			}
		}
	}

	return ( !( intersect.empty() ) );
}

//----------------------------------------
void CInt64Array::IncrementValue( uint64_t incr /* = 1 */ )
{
	CInt64Array::iterator it;

	for ( it = this->begin(); it != this->end(); it++ )
	{
		( *it ) = ( *it ) + incr;
	}


}

//----------------------------------------
std::string CInt64Array::ToString( const std::string& delim /*= "," */, bool useBracket /*= true */ ) const
{
	std::ostringstream myStream;

	if ( useBracket )
	{
		myStream << "[";
	}


	CInt64Array::const_iterator it;
	for ( it = this->begin(); it != this->end(); it++ )
	{
		myStream << *it;
		if ( ( it + 1 ) != this->end() )
		{
			myStream << delim;
		}
	}
	if ( useBracket )
	{
		myStream << "]";
	}


	return myStream.str();
}
//----------------------------------------
int64_t* CInt64Array::ToArray()
{
	int64_t* newArray = new int64_t[ this->size() ];

	for ( size_t i = 0; i < this->size(); i++ )
	{
		newArray[ i ] = this->at( i );
	}


	return newArray;
}

//----------------------------------------
void CInt64Array::Dump( std::ostream& fOut /* = std::cerr */ ) const
{
	if ( CTrace::IsTrace() == false )
	{
		return;
	}

	CInt64Array::const_iterator it;
	int i = 0;

	fOut << "==> Dump a CInt64Array Object at " << this << " with " << size() << " elements" << std::endl;

	for ( it = this->begin(); it != this->end(); it++ )
	{
		fOut << "CInt64Array[" << i << "]= " << *it << std::endl;
		i++;
	}

	fOut << "==> END Dump a CInt64Array Object at " << this << " with " << size() << " elements" << std::endl;

	fOut << std::endl;
}



//-------------------------------------------------------------
//------------------- CUIntArray class --------------------
//-------------------------------------------------------------

//----------------------------------------
bool CUIntArray::operator ==( const CUIntArray& vect )
{
	size_t count = this->size();

	if ( count != vect.size() )
	{
		return false;
	}

	if ( count == 0 )
	{
		return true;
	}

	bool isEqual = true;

	for ( uint32_t i = 0; i < count; i++ )
	{
		if ( this->at( i ) != vect.at( i ) )
		{
			isEqual = false;
			break;
		}
	}

	return isEqual;
}

//----------------------------------------

const CUIntArray& CUIntArray::operator =( const CUIntArray& vect )
{

	this->clear();

	Insert( vect );

	return *this;


}
//----------------------------------------

void CUIntArray::Insert( CUIntArray* vect, bool bEnd /*= true*/ )
{
	Insert( *vect );
}

//----------------------------------------

void CUIntArray::Insert( const CUIntArray& vect, bool bEnd /*= true*/ )
{

	if ( vect.empty() )
	{
		return;
	}

	if ( bEnd )
	{
		uint32array::insert( this->end(), vect.begin(), vect.end() );
	}
	else
	{
		uint32array::insert( this->begin(), vect.begin(), vect.end() );
	}


}
//----------------------------------------
void CUIntArray::Insert( const std::vector<uint32_t>& vect, bool bEnd /*= true*/ )
{

	if ( vect.empty() )
	{
		return;
	}

	if ( bEnd )
	{
		uint32array::insert( this->end(), vect.begin(), vect.end() );
	}
	else
	{
		uint32array::insert( this->begin(), vect.begin(), vect.end() );
	}


}
//----------------------------------------
void CUIntArray::Insert( uint32_t* vect, size_t length )
{
	for ( size_t i = 0; i < length; i++ )
	{
		Insert( vect[ i ] );
	}
}

//----------------------------------------
void CUIntArray::Insert( const uint32_t value )
{

	uint32array::push_back( value );

}

//----------------------------------------
CUIntArray::iterator CUIntArray::InsertAt( CUIntArray::iterator where, const uint32_t value )
{
	return uint32array::insert( where, value );
}
//----------------------------------------
CUIntArray::iterator CUIntArray::InsertAt( int32_t index, const uint32_t value )
{
	if ( index < 0 )
	{
		return this->end();
	}

	if ( (uint32_t)index >= this->size() )
	{
		this->resize( index + 1 );
		( *this )[ index ] = value;
		return this->begin() + index;
	}

	return uint32array::insert( this->begin() + index, value );
}

//----------------------------------------
bool CUIntArray::Erase( CUIntArray::iterator where )
{
	uint32array::erase( where );
	return true;
}
//----------------------------------------
CUIntArray::iterator CUIntArray::ReplaceAt( int32_t index, const uint32_t value )
{
	if ( index < 0 )
	{
		return this->end();
	}

	( *this )[ index ] = value;
	return this->begin() + index;

}
//----------------------------------------
CUIntArray::iterator CUIntArray::ReplaceAt( CUIntArray::iterator where, const uint32_t value )
{
	Erase( where );
	return InsertAt( where, value );
}

//----------------------------------------
bool CUIntArray::Intersect( const CUIntArray& array, CUIntArray& intersect ) const
{
	intersect.clear();

	CUIntArray::const_iterator itA;
	CUIntArray::const_iterator itB;

	for ( itA = this->begin(); itA != this->end(); itA++ )
	{
		for ( itB = array.begin(); itB != array.end(); itB++ )
		{
			if ( ( *itA ) == ( *itB ) )
			{
				intersect.Insert( ( *itB ) );
			}
		}
	}

	return ( !( intersect.empty() ) );
}

//----------------------------------------
bool CUIntArray::Complement( const CUIntArray& array, CUIntArray& complement ) const
{
	CUIntArray intersect;
	complement.clear();

	if ( Intersect( array, intersect ) == false )
	{
		complement.Insert( array );
		return ( !( complement.empty() ) );
	}


	CUIntArray::const_iterator itArray;
	CUIntArray::const_iterator itIntersect= intersect.begin();

	for ( itArray = array.begin(); itArray != array.end(); itArray++ )
	{
		bool found = false;
		for ( itIntersect = intersect.begin(); itIntersect != intersect.end(); itIntersect++ )
		{
			if ( ( *itArray ) == ( *itIntersect ) )
			{
				found = true;
				break;
			}
		}

		if ( !found )
		{
			complement.Insert( *itArray );
		}

	}

	return ( !( complement.empty() ) );
}


//----------------------------------------
std::string CUIntArray::ToString( const std::string& delim /*= "," */, bool useBracket /*= true */ ) const
{
	std::ostringstream myStream;

	if ( useBracket )
	{
		myStream << "[";
	}

	CUIntArray::const_iterator it;
	for ( it = this->begin(); it != this->end(); it++ )
	{
		myStream << *it;
		if ( ( it + 1 ) != this->end() )
		{
			myStream << delim;
		}
	}
	if ( useBracket )
	{
		myStream << "]";
	}


	return myStream.str();

}
//----------------------------------------
uint32_t* CUIntArray::ToArray()
{

	uint32_t* newArray = new uint32_t[ this->size() ];

	for ( uint32_t i = 0; i < this->size(); i++ )
	{
		newArray[ i ] = this->at( i );
	}


	return newArray;

}
//----------------------------------------
int32_t* CUIntArray::ToIntArray()
{

	int32_t* newArray = new int32_t[ this->size() ];

	for ( uint32_t i = 0; i < this->size(); i++ )
	{
		newArray[ i ] = static_cast<int32_t>( this->at( i ) );
	}


	return newArray;

}
//----------------------------------------
size_t* CUIntArray::ToSizeTArray()
{

	size_t* newArray = new size_t[ this->size() ];

	for ( uint32_t i = 0; i < this->size(); i++ )
	{
		newArray[ i ] = static_cast<size_t>( this->at( i ) );
	}


	return newArray;

}
//----------------------------------------
uint32_t CUIntArray::GetProductValues() const
{
	uint32_t product = 1;

	if ( this->size() <= 0 )
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
void CUIntArray::Dump( std::ostream& fOut /* = std::cerr */ ) const
{

	if ( CTrace::IsTrace() == false )
	{
		return;
	}

	CUIntArray::const_iterator it;
	int i = 0;

	fOut << "==> Dump a CUIntArray Object at " << this << " with " << size() << " elements" << std::endl;

	for ( it = this->begin(); it != this->end(); it++ )
	{
		fOut << "CUIntArray[" << i << "]= " << *it << std::endl;
		i++;
	}

	fOut << "==> END Dump a CUIntArray Object at " << this << " with " << size() << " elements" << std::endl;


	fOut << std::endl;


}




//-------------------------------------------------------------
//------------------- CUInt64Array class --------------------
//-------------------------------------------------------------

//----------------------------------------
bool CUInt64Array::operator ==( const CUInt64Array& vect )
{
	size_t count = this->size();

	if ( count != vect.size() )
	{
		return false;
	}

	if ( count == 0 )
	{
		return true;
	}

	bool isEqual = true;

	for ( size_t i = 0; i < count; i++ )
	{
		if ( this->at( i ) != vect.at( i ) )
		{
			isEqual = false;
			break;
		}
	}

	return isEqual;
}
//----------------------------------------
const CUInt64Array& CUInt64Array::operator =( const CUInt64Array& vect )
{

	this->clear();

	Insert( vect );

	return *this;


}
//----------------------------------------
void CUInt64Array::Insert( const CUInt64Array& vect, bool bEnd /*= true*/ )
{

	if ( vect.empty() )
	{
		return;
	}

	if ( bEnd )
	{
		uint64array::insert( this->end(), vect.begin(), vect.end() );
	}
	else
	{
		uint64array::insert( this->begin(), vect.begin(), vect.end() );
	}


}
//----------------------------------------

void CUInt64Array::Insert( CUInt64Array* vect, bool bEnd /*= true*/ )
{
	Insert( *vect );
}

//----------------------------------------
void CUInt64Array::Insert( const std::vector<uint64_t>& vect, bool bEnd /*= true*/ )
{

	if ( vect.empty() )
	{
		return;
	}

	if ( bEnd )
	{
		uint64array::insert( this->end(), vect.begin(), vect.end() );
	}
	else
	{
		uint64array::insert( this->begin(), vect.begin(), vect.end() );
	}


}
//----------------------------------------
void CUInt64Array::Insert( uint64_t* vect, size_t length )
{
	for ( size_t i = 0; i < length; i++ )
	{
		Insert( vect[ i ] );
	}
}

//----------------------------------------
void CUInt64Array::Insert( const uint64_t value )
{

	uint64array::push_back( value );

}

//----------------------------------------
CUInt64Array::iterator CUInt64Array::InsertAt( CUInt64Array::iterator where, const uint64_t value )
{
	return uint64array::insert( where, value );
}
//----------------------------------------
CUInt64Array::iterator CUInt64Array::InsertAt( size_t index, const uint64_t value )
{
	if ( index < 0 )
	{
		return this->end();
	}

	if ( index >= this->size() )
	{
		this->resize( index + 1 );
		( *this )[ index ] = value;
		return this->begin() + index;
	}

	return uint64array::insert( this->begin() + index, value );
}


//----------------------------------------
bool CUInt64Array::Erase( CUInt64Array::iterator where )
{
	uint64array::erase( where );
	return true;
}
//----------------------------------------
CUInt64Array::iterator CUInt64Array::ReplaceAt( size_t index, const uint64_t value )
{
	( *this )[ index ] = value;
	return this->begin() + index;
}
//----------------------------------------
CUInt64Array::iterator CUInt64Array::ReplaceAt( CUInt64Array::iterator where, const uint64_t value )
{
	Erase( where );
	return InsertAt( where, value );
}

//----------------------------------------
bool CUInt64Array::Intersect( const CUInt64Array& array, CUInt64Array& intersect ) const
{
	intersect.clear();

	CUInt64Array::const_iterator itA;
	CUInt64Array::const_iterator itB;

	for ( itA = this->begin(); itA != this->end(); itA++ )
	{
		for ( itB = array.begin(); itB != array.end(); itB++ )
		{
			if ( ( *itA ) == ( *itB ) )
			{
				intersect.Insert( ( *itB ) );
			}
		}
	}

	return ( !( intersect.empty() ) );
}

//----------------------------------------
std::string CUInt64Array::ToString( const std::string& delim /*= "," */, bool useBracket /*= true */ ) const
{
	std::ostringstream myStream;

	if ( useBracket )
	{
		myStream << "[";
	}


	CUInt64Array::const_iterator it;
	for ( it = this->begin(); it != this->end(); it++ )
	{
		myStream << *it;
		if ( ( it + 1 ) != this->end() )
		{
			myStream << delim;
		}
	}
	if ( useBracket )
	{
		myStream << "]";
	}


	return myStream.str();

}
//----------------------------------------
uint64_t* CUInt64Array::ToArray()
{

	uint64_t* newArray = new uint64_t[ this->size() ];

	for ( size_t i = 0; i < this->size(); i++ )
	{
		newArray[ i ] = this->at( i );
	}


	return newArray;

}

//----------------------------------------
void CUInt64Array::Dump( std::ostream& fOut /* = std::cerr */ ) const
{

	if ( CTrace::IsTrace() == false )
	{
		return;
	}

	CUInt64Array::const_iterator it;
	int i = 0;

	fOut << "==> Dump a CUInt64Array Object at " << this << " with " << size() << " elements" << std::endl;

	for ( it = this->begin(); it != this->end(); it++ )
	{
		fOut << "CUInt64Array[" << i << "]= " << *it << std::endl;
		i++;
	}

	fOut << "==> END Dump a CUInt64Array Object at " << this << " with " << size() << " elements" << std::endl;


	fOut << std::endl;


}




//-------------------------------------------------------------
//------------------- CInt16Array class --------------------
//-------------------------------------------------------------

const CInt16Array& CInt16Array::operator =( const CInt16Array& vect )
{

	this->clear();

	Insert( vect );

	return *this;


}
//----------------------------------------
void CInt16Array::Insert( const CInt16Array& vect, bool bEnd /*= true*/ )
{

	if ( vect.empty() )
	{
		return;
	}

	if ( bEnd )
	{
		int16array::insert( this->end(), vect.begin(), vect.end() );
	}
	else
	{
		int16array::insert( this->begin(), vect.begin(), vect.end() );
	}


}

//----------------------------------------
void CInt16Array::Insert( int16_t* vect, size_t length )
{
	for ( size_t i = 0; i < length; i++ )
	{
		Insert( vect[ i ] );
	}
}
//----------------------------------------

void CInt16Array::Insert( const int16_t value )
{

	int16array::push_back( value );

}

//----------------------------------------
CInt16Array::iterator CInt16Array::InsertAt( CInt16Array::iterator where, const int16_t value )
{
	return int16array::insert( where, value );
}
//----------------------------------------
CInt16Array::iterator CInt16Array::InsertAt( int32_t index, const int16_t value )
{
	if ( index < 0 )
	{
		return this->end();
	}

	if ( (uint32_t)index >= this->size() )
	{
		this->resize( index + 1 );
		( *this )[ index ] = value;
		return this->begin() + index;
	}

	return int16array::insert( this->begin() + index, value );
}

//----------------------------------------
bool CInt16Array::Erase( CInt16Array::iterator where )
{
	int16array::erase( where );
	return true;
}
//----------------------------------------
CInt16Array::iterator CInt16Array::ReplaceAt( int32_t index, const int16_t value )
{
	if ( index < 0 )
	{
		return this->end();
	}

	( *this )[ index ] = value;
	return this->begin() + index;

}
//----------------------------------------
CInt16Array::iterator CInt16Array::ReplaceAt( CInt16Array::iterator where, const int16_t value )
{
	Erase( where );
	return InsertAt( where, value );
}

//----------------------------------------
bool CInt16Array::Intersect( const CInt16Array& array, CInt16Array& intersect ) const
{
	intersect.clear();

	CInt16Array::const_iterator itA;
	CInt16Array::const_iterator itB;

	for ( itA = this->begin(); itA != this->end(); itA++ )
	{
		for ( itB = array.begin(); itB != array.end(); itB++ )
		{
			if ( ( *itA ) == ( *itB ) )
			{
				intersect.Insert( ( *itB ) );
			}
		}
	}

	return ( !( intersect.empty() ) );
}


//----------------------------------------
std::string CInt16Array::ToString( const std::string& delim /*= "," */, bool useBracket /*= true */ ) const
{
	std::ostringstream myStream;

	if ( useBracket )
	{
		myStream << "[";
	}


	CInt16Array::const_iterator it;
	for ( it = this->begin(); it != this->end(); it++ )
	{
		myStream << *it;
		if ( ( it + 1 ) != this->end() )
		{
			myStream << delim;
		}
	}
	if ( useBracket )
	{
		myStream << "]";
	}


	return myStream.str();

}
//----------------------------------------
int16_t* CInt16Array::ToArray()
{

	int16_t* newArray = new int16_t[ this->size() ];

	for ( uint16_t i = 0; i < this->size(); i++ )
	{
		newArray[ i ] = this->at( i );
	}


	return newArray;

}

//----------------------------------------
void CInt16Array::Dump( std::ostream& fOut /* = std::cerr */ ) const
{

	if ( CTrace::IsTrace() == false )
	{
		return;
	}

	CInt16Array::const_iterator it;
	int i = 0;

	fOut << "==> Dump a CInt16Array Object at " << this << " with " << size() << " elements" << std::endl;

	for ( it = this->begin(); it != this->end(); it++ )
	{
		fOut << "CInt16Array[" << i << "]= " << *it << std::endl;
		i++;
	}

	fOut << "==> END Dump a CInt16Array Object at " << this << " with " << size() << " elements" << std::endl;


	fOut << std::endl;


}




//-------------------------------------------------------------
//------------------- CUInt16Array class --------------------
//-------------------------------------------------------------

//----------------------------------------
bool CUInt16Array::operator ==( const CUInt16Array& vect )
{
	size_t count = this->size();

	if ( count != vect.size() )
	{
		return false;
	}

	if ( count == 0 )
	{
		return true;
	}

	bool isEqual = true;

	for ( uint16_t i = 0; i < count; i++ )
	{
		if ( this->at( i ) != vect.at( i ) )
		{
			isEqual = false;
			break;
		}
	}

	return isEqual;
}

//----------------------------------------

const CUInt16Array& CUInt16Array::operator =( const CUInt16Array& vect )
{

	this->clear();

	Insert( vect );

	return *this;


}
//----------------------------------------

void CUInt16Array::Insert( CUInt16Array* vect, bool bEnd /*= true*/ )
{
	Insert( *vect );
}

//----------------------------------------

void CUInt16Array::Insert( const CUInt16Array& vect, bool bEnd /*= true*/ )
{

	if ( vect.empty() )
	{
		return;
	}

	if ( bEnd )
	{
		uint16array::insert( this->end(), vect.begin(), vect.end() );
	}
	else
	{
		uint16array::insert( this->begin(), vect.begin(), vect.end() );
	}


}
//----------------------------------------
void CUInt16Array::Insert( const std::vector<uint16_t>& vect, bool bEnd /*= true*/ )
{

	if ( vect.empty() )
	{
		return;
	}

	if ( bEnd )
	{
		uint16array::insert( this->end(), vect.begin(), vect.end() );
	}
	else
	{
		uint16array::insert( this->begin(), vect.begin(), vect.end() );
	}


}
//----------------------------------------
void CUInt16Array::Insert( uint16_t* vect, size_t length )
{
	for ( size_t i = 0; i < length; i++ )
	{
		Insert( vect[ i ] );
	}
}
//----------------------------------------

void CUInt16Array::Insert( const uint16_t value )
{

	uint16array::push_back( value );

}

//----------------------------------------
CUInt16Array::iterator CUInt16Array::InsertAt( CUInt16Array::iterator where, const uint16_t value )
{
	return uint16array::insert( where, value );
}
//----------------------------------------
CUInt16Array::iterator CUInt16Array::InsertAt( int32_t index, const uint16_t value )
{
	if ( index < 0 )
	{
		return this->end();
	}

	if ( (uint32_t)index >= this->size() )
	{
		this->resize( index + 1 );
		( *this )[ index ] = value;
		return this->begin() + index;
	}

	return uint16array::insert( this->begin() + index, value );
}

//----------------------------------------
bool CUInt16Array::Erase( CUInt16Array::iterator where )
{
	uint16array::erase( where );
	return true;
}
//----------------------------------------
CUInt16Array::iterator CUInt16Array::ReplaceAt( int32_t index, const uint16_t value )
{
	if ( index < 0 )
	{
		return this->end();
	}

	( *this )[ index ] = value;
	return this->begin() + index;

}
//----------------------------------------
CUInt16Array::iterator CUInt16Array::ReplaceAt( CUInt16Array::iterator where, const uint16_t value )
{
	Erase( where );
	return InsertAt( where, value );
}

//----------------------------------------
bool CUInt16Array::Intersect( const CUInt16Array& array, CUInt16Array& intersect ) const
{
	intersect.clear();

	CUInt16Array::const_iterator itA;
	CUInt16Array::const_iterator itB;

	for ( itA = this->begin(); itA != this->end(); itA++ )
	{
		for ( itB = array.begin(); itB != array.end(); itB++ )
		{
			if ( ( *itA ) == ( *itB ) )
			{
				intersect.Insert( ( *itB ) );
			}
		}
	}

	return ( !( intersect.empty() ) );
}

//----------------------------------------
bool CUInt16Array::Complement( const CUInt16Array& array, CUInt16Array& complement ) const
{
	CUInt16Array intersect;
	complement.clear();

	if ( Intersect( array, intersect ) == false )
	{
		complement.Insert( array );
		return ( !( complement.empty() ) );
	}


	CUInt16Array::const_iterator itArray;
	CUInt16Array::const_iterator itIntersect= intersect.begin();

	for ( itArray = array.begin(); itArray != array.end(); itArray++ )
	{
		bool found = false;
		for ( itIntersect = intersect.begin(); itIntersect != intersect.end(); itIntersect++ )
		{
			if ( ( *itArray ) == ( *itIntersect ) )
			{
				found = true;
				break;
			}
		}

		if ( !found )
		{
			complement.Insert( *itArray );
		}

	}

	return ( !( complement.empty() ) );
}


//----------------------------------------
std::string CUInt16Array::ToString( const std::string& delim /*= "," */, bool useBracket /*= true */ ) const
{
	std::ostringstream myStream;

	if ( useBracket )
	{
		myStream << "[";
	}

	CUInt16Array::const_iterator it;
	for ( it = this->begin(); it != this->end(); it++ )
	{
		myStream << *it;
		if ( ( it + 1 ) != this->end() )
		{
			myStream << delim;
		}
	}
	if ( useBracket )
	{
		myStream << "]";
	}


	return myStream.str();

}
//----------------------------------------
uint16_t* CUInt16Array::ToArray()
{

	uint16_t* newArray = new uint16_t[ this->size() ];

	for ( uint16_t i = 0; i < this->size(); i++ )
	{
		newArray[ i ] = this->at( i );
	}


	return newArray;

}
//----------------------------------------
int16_t* CUInt16Array::ToIntArray()
{

	int16_t* newArray = new int16_t[ this->size() ];

	for ( uint16_t i = 0; i < this->size(); i++ )
	{
		newArray[ i ] = static_cast<int16_t>( this->at( i ) );
	}


	return newArray;

}
//----------------------------------------
void CUInt16Array::Dump( std::ostream& fOut /* = std::cerr */ ) const
{

	if ( CTrace::IsTrace() == false )
	{
		return;
	}

	CUInt16Array::const_iterator it;
	int i = 0;

	fOut << "==> Dump a CUInt16Array Object at " << this << " with " << size() << " elements" << std::endl;

	for ( it = this->begin(); it != this->end(); it++ )
	{
		fOut << "CUInt16Array[" << i << "]= " << *it << std::endl;
		i++;
	}

	fOut << "==> END Dump a CUInt16Array Object at " << this << " with " << size() << " elements" << std::endl;


	fOut << std::endl;


}



//-------------------------------------------------------------
//------------------- CInt8Array class --------------------
//-------------------------------------------------------------

const CInt8Array& CInt8Array::operator =( const CInt8Array& vect )
{

	this->clear();

	Insert( vect );

	return *this;


}
//----------------------------------------
void CInt8Array::Insert( const CInt8Array& vect, bool bEnd /*= true*/ )
{

	if ( vect.empty() )
	{
		return;
	}

	if ( bEnd )
	{
		int8array::insert( this->end(), vect.begin(), vect.end() );
	}
	else
	{
		int8array::insert( this->begin(), vect.begin(), vect.end() );
	}


}
//----------------------------------------
void CInt8Array::Insert( int8_t* vect, size_t length )
{
	for ( size_t i = 0; i < length; i++ )
	{
		Insert( vect[ i ] );
	}
}

//----------------------------------------

void CInt8Array::Insert( const int8_t value )
{

	int8array::push_back( value );

}

//----------------------------------------
CInt8Array::iterator CInt8Array::InsertAt( CInt8Array::iterator where, const int8_t value )
{
	return int8array::insert( where, value );
}
//----------------------------------------
CInt8Array::iterator CInt8Array::InsertAt( int32_t index, const int8_t value )
{
	if ( index < 0 )
	{
		return this->end();
	}

	if ( (uint32_t)index >= this->size() )
	{
		this->resize( index + 1 );
		( *this )[ index ] = value;
		return this->begin() + index;
	}

	return int8array::insert( this->begin() + index, value );
}

//----------------------------------------
bool CInt8Array::Erase( CInt8Array::iterator where )
{
	int8array::erase( where );
	return true;
}
//----------------------------------------
CInt8Array::iterator CInt8Array::ReplaceAt( int32_t index, const int8_t value )
{
	if ( index < 0 )
	{
		return this->end();
	}

	( *this )[ index ] = value;
	return this->begin() + index;

}
//----------------------------------------
CInt8Array::iterator CInt8Array::ReplaceAt( CInt8Array::iterator where, const int8_t value )
{
	Erase( where );
	return InsertAt( where, value );
}

//----------------------------------------
bool CInt8Array::Intersect( const CInt8Array& array, CInt8Array& intersect ) const
{
	intersect.clear();

	CInt8Array::const_iterator itA;
	CInt8Array::const_iterator itB;

	for ( itA = this->begin(); itA != this->end(); itA++ )
	{
		for ( itB = array.begin(); itB != array.end(); itB++ )
		{
			if ( ( *itA ) == ( *itB ) )
			{
				intersect.Insert( ( *itB ) );
			}
		}
	}

	return ( !( intersect.empty() ) );
}


//----------------------------------------
std::string CInt8Array::ToString( const std::string& delim /*= "," */, bool useBracket /*= true */ ) const
{
	std::ostringstream myStream;

	if ( useBracket )
	{
		myStream << "[";
	}


	CInt8Array::const_iterator it;
	for ( it = this->begin(); it != this->end(); it++ )
	{
		myStream << *it;
		if ( ( it + 1 ) != this->end() )
		{
			myStream << delim;
		}
	}
	if ( useBracket )
	{
		myStream << "]";
	}


	return myStream.str();

}
//----------------------------------------
int8_t* CInt8Array::ToArray()
{

	int8_t* newArray = new int8_t[ this->size() ];

	for ( uint8_t i = 0; i < this->size(); i++ )
	{
		newArray[ i ] = this->at( i );
	}


	return newArray;

}

//----------------------------------------
void CInt8Array::Dump( std::ostream& fOut /* = std::cerr */ ) const
{

	if ( CTrace::IsTrace() == false )
	{
		return;
	}

	CInt8Array::const_iterator it;
	int i = 0;

	fOut << "==> Dump a CInt8Array Object at " << this << " with " << size() << " elements" << std::endl;

	for ( it = this->begin(); it != this->end(); it++ )
	{
		fOut << "CInt8Array[" << i << "]= " << *it << std::endl;
		i++;
	}

	fOut << "==> END Dump a CInt8Array Object at " << this << " with " << size() << " elements" << std::endl;


	fOut << std::endl;


}




//-------------------------------------------------------------
//------------------- CUInt8Array class --------------------
//-------------------------------------------------------------

bool CUInt8Array::operator ==( const CUInt8Array& vect )
{
	size_t count = this->size();

	if ( count != vect.size() )
	{
		return false;
	}

	if ( count == 0 )
	{
		return true;
	}

	bool isEqual = true;

	for ( uint8_t i = 0; i < count; i++ )
	{
		if ( this->at( i ) != vect.at( i ) )
		{
			isEqual = false;
			break;
		}
	}

	return isEqual;
}

//----------------------------------------

const CUInt8Array& CUInt8Array::operator =( const CUInt8Array& vect )
{

	this->clear();

	Insert( vect );

	return *this;


}
//----------------------------------------

void CUInt8Array::Insert( CUInt8Array* vect, bool bEnd /*= true*/ )
{
	Insert( *vect );
}

//----------------------------------------

void CUInt8Array::Insert( const CUInt8Array& vect, bool bEnd /*= true*/ )
{

	if ( vect.empty() )
	{
		return;
	}

	if ( bEnd )
	{
		uint8array::insert( this->end(), vect.begin(), vect.end() );
	}
	else
	{
		uint8array::insert( this->begin(), vect.begin(), vect.end() );
	}


}
//----------------------------------------
void CUInt8Array::Insert( const std::vector<uint8_t>& vect, bool bEnd /*= true*/ )
{

	if ( vect.empty() )
	{
		return;
	}

	if ( bEnd )
	{
		uint8array::insert( this->end(), vect.begin(), vect.end() );
	}
	else
	{
		uint8array::insert( this->begin(), vect.begin(), vect.end() );
	}


}
//----------------------------------------
void CUInt8Array::Insert( uint8_t* vect, size_t length )
{
	for ( size_t i = 0; i < length; i++ )
	{
		Insert( vect[ i ] );
	}
}

//----------------------------------------

void CUInt8Array::Insert( const uint8_t value )
{

	uint8array::push_back( value );

}

//----------------------------------------
CUInt8Array::iterator CUInt8Array::InsertAt( CUInt8Array::iterator where, const uint8_t value )
{
	return uint8array::insert( where, value );
}
//----------------------------------------
CUInt8Array::iterator CUInt8Array::InsertAt( int32_t index, const uint8_t value )
{
	if ( index < 0 )
	{
		return this->end();
	}

	if ( (uint32_t)index >= this->size() )
	{
		this->resize( index + 1 );
		( *this )[ index ] = value;
		return this->begin() + index;
	}

	return uint8array::insert( this->begin() + index, value );
}

//----------------------------------------
bool CUInt8Array::Erase( CUInt8Array::iterator where )
{
	uint8array::erase( where );
	return true;
}
//----------------------------------------
CUInt8Array::iterator CUInt8Array::ReplaceAt( int32_t index, const uint8_t value )
{
	if ( index < 0 )
	{
		return this->end();
	}

	( *this )[ index ] = value;
	return this->begin() + index;

}
//----------------------------------------
CUInt8Array::iterator CUInt8Array::ReplaceAt( CUInt8Array::iterator where, const uint8_t value )
{
	Erase( where );
	return InsertAt( where, value );
}

//----------------------------------------
bool CUInt8Array::Intersect( const CUInt8Array& array, CUInt8Array& intersect ) const
{
	intersect.clear();

	CUInt8Array::const_iterator itA;
	CUInt8Array::const_iterator itB;

	for ( itA = this->begin(); itA != this->end(); itA++ )
	{
		for ( itB = array.begin(); itB != array.end(); itB++ )
		{
			if ( ( *itA ) == ( *itB ) )
			{
				intersect.Insert( ( *itB ) );
			}
		}
	}

	return ( !( intersect.empty() ) );
}

//----------------------------------------
bool CUInt8Array::Complement( const CUInt8Array& array, CUInt8Array& complement ) const
{
	CUInt8Array intersect;
	complement.clear();

	if ( Intersect( array, intersect ) == false )
	{
		complement.Insert( array );
		return ( !( complement.empty() ) );
	}


	CUInt8Array::const_iterator itArray;
	CUInt8Array::const_iterator itIntersect= intersect.begin();

	for ( itArray = array.begin(); itArray != array.end(); itArray++ )
	{
		bool found = false;
		for ( itIntersect = intersect.begin(); itIntersect != intersect.end(); itIntersect++ )
		{
			if ( ( *itArray ) == ( *itIntersect ) )
			{
				found = true;
				break;
			}
		}

		if ( !found )
		{
			complement.Insert( *itArray );
		}

	}

	return ( !( complement.empty() ) );
}


//----------------------------------------
std::string CUInt8Array::ToString( const std::string& delim /*= "," */, bool useBracket /*= true */ ) const
{
	std::ostringstream myStream;

	if ( useBracket )
	{
		myStream << "[";
	}

	CUInt8Array::const_iterator it;
	for ( it = this->begin(); it != this->end(); it++ )
	{
		myStream << *it;
		if ( ( it + 1 ) != this->end() )
		{
			myStream << delim;
		}
	}
	if ( useBracket )
	{
		myStream << "]";
	}


	return myStream.str();

}
//----------------------------------------
uint8_t* CUInt8Array::ToArray()
{

	uint8_t* newArray = new uint8_t[ this->size() ];

	for ( uint8_t i = 0; i < this->size(); i++ )
	{
		newArray[ i ] = this->at( i );
	}


	return newArray;

}
//----------------------------------------
int8_t* CUInt8Array::ToIntArray()
{

	int8_t* newArray = new int8_t[ this->size() ];

	for ( uint8_t i = 0; i < this->size(); i++ )
	{
		newArray[ i ] = static_cast<int8_t>( this->at( i ) );
	}


	return newArray;

}
//----------------------------------------
void CUInt8Array::Dump( std::ostream& fOut /* = std::cerr */ ) const
{

	if ( CTrace::IsTrace() == false )
	{
		return;
	}

	CUInt8Array::const_iterator it;
	int i = 0;

	fOut << "==> Dump a CUInt8Array Object at " << this << " with " << size() << " elements" << std::endl;

	for ( it = this->begin(); it != this->end(); it++ )
	{
		fOut << "CUInt8Array[" << i << "]= " << *it << std::endl;
		i++;
	}

	fOut << "==> END Dump a CUInt8Array Object at " << this << " with " << size() << " elements" << std::endl;


	fOut << std::endl;


}




//-------------------------------------------------------------
//------------------- CFloatArray class --------------------
//-------------------------------------------------------------

void CFloatArray::RemoveAll()
{

	floatarray::clear();

}

//----------------------------------------


const CFloatArray& CFloatArray::operator =( const CFloatArray& vect )
{

	this->clear();

	Insert( vect );

	return *this;


}

//----------------------------------------
void CFloatArray::Insert( float* data, int32_t size )
{
	if ( size <= 0 )
	{
		return;
	}

	for ( int32_t i = 0; i < size; i++ )
	{
		this->Insert( data[ i ] );
	}
}
//----------------------------------------
void CFloatArray::Insert( const CFloatArray& vect, bool bEnd /*= true*/ )
{
	if ( vect.empty() )
	{
		return;
	}

	if ( bEnd )
	{
		floatarray::insert( this->end(), vect.begin(), vect.end() );
	}
	else
	{
		floatarray::insert( this->begin(), vect.begin(), vect.end() );
	}


}
//----------------------------------------
void CFloatArray::Insert( const CFloatArray& vect, int32_t first, int32_t last, bool bEnd /*= true*/ )
{
	if ( vect.empty() )
	{
		return;
	}

	if ( bEnd )
	{
		floatarray::insert( this->end(), vect.begin() + first, vect.begin() + last );
	}
	else
	{
		floatarray::insert( this->begin(), vect.begin() + first, vect.begin() + last );
	}


}
//----------------------------------------

void CFloatArray::Insert( const float value )
{

	floatarray::push_back( value );

}
//----------------------------------------
void CFloatArray::Insert( const int32_t value )
{

	floatarray::push_back( static_cast<float>( value ) );

}
//----------------------------------------
void CFloatArray::Insert( const uint32_t value )
{

	floatarray::push_back( static_cast<float>( value ) );

}

//----------------------------------------
CFloatArray::iterator CFloatArray::InsertAt( CFloatArray::iterator where, const float value )
{
	return floatarray::insert( where, value );
}
//----------------------------------------
CFloatArray::iterator CFloatArray::InsertAt( int32_t index, const float value )
{
	if ( index < 0 )
	{
		return this->end();
	}

	if ( (uint32_t)index >= this->size() )
	{
		this->resize( index + 1 );
		( *this )[ index ] = value;
		return this->begin() + index;
	}

	return floatarray::insert( this->begin() + index, value );
}

//----------------------------------------
bool CFloatArray::Erase( CFloatArray::iterator where )
{
	floatarray::erase( where );
	return true;
}
//----------------------------------------
CFloatArray::iterator CFloatArray::ReplaceAt( int32_t index, const float value )
{
	if ( index < 0 )
	{
		return this->end();
	}

	( *this )[ index ] = value;
	return this->begin() + index;

}
//----------------------------------------
CFloatArray::iterator CFloatArray::ReplaceAt( CFloatArray::iterator where, const float value )
{
	Erase( where );
	return InsertAt( where, value );
}
//----------------------------------------
bool CFloatArray::Intersect( const CFloatArray& array, CFloatArray& intersect ) const
{
	intersect.clear();

	CFloatArray::const_iterator itA;
	CFloatArray::const_iterator itB;

	for ( itA = this->begin(); itA != this->end(); itA++ )
	{
		for ( itB = array.begin(); itB != array.end(); itB++ )
		{
			if ( ( *itA ) == ( *itB ) )
			{
				intersect.Insert( ( *itB ) );
			}
		}
	}

	return ( !( intersect.empty() ) );
}

//----------------------------------------
float* CFloatArray::ToArray()
{

	float* newArray = new float[ this->size() ];

	for ( uint32_t i = 0; i < this->size(); i++ )
	{
		newArray[ i ] = this->at( i );
	}


	return newArray;

}

//----------------------------------------
std::string CFloatArray::ToString( const std::string& delim /*= "," */, bool useBracket /*= true */ ) const
{
	std::ostringstream myStream;

	if ( useBracket )
	{
		myStream << "[";
	}


	CFloatArray::const_iterator it;
	for ( it = this->begin(); it != this->end(); it++ )
	{
		myStream << *it;
		if ( ( it + 1 ) != this->end() )
		{
			myStream << delim;
		}
	}
	if ( useBracket )
	{
		myStream << "]";
	}


	return myStream.str();

}

//----------------------------------------
void CFloatArray::GetRange( float& min, float& max )
{
	setDefaultValue( min );
	setDefaultValue( max );

	CFloatArray::iterator it;

	for ( it = begin(); it != end(); it++ )
	{
		if ( it == begin() )
		{
			min = *it;
			max = *it;
		}
		else
		{
			min = ( min > *it ) ? *it : min;
			max = ( max > *it ) ? max : *it;
		}
	}
}

//----------------------------------------
void CFloatArray::Dump( std::ostream& fOut /* = std::cerr */ ) const
{

	if ( CTrace::IsTrace() == false )
	{
		return;
	}

	CFloatArray::const_iterator it;
	int i = 0;

	fOut << "==> Dump a CFloatArray Object at " << this << " with " << size() << " elements" << std::endl;

	for ( it = this->begin(); it != this->end(); it++ )
	{
		fOut << "CFloatArray[" << i << "]= " << n2s<std::string>( *it, floatPrecision ) << std::endl;
		i++;
	}

	fOut << "==> END Dump a CFloatArray Object at " << this << " with " << size() << " elements" << std::endl;


	fOut << std::endl;


}



//-------------------------------------------------------------
//------------------- CDoubleArray class --------------------
//-------------------------------------------------------------

//----------------------------------------

void CDoubleArray::RemoveAll()
{

	doublearray::clear();

}

//----------------------------------------


const CDoubleArray& CDoubleArray::operator =( const CDoubleArray& vect )
{

	this->clear();

	Insert( vect );

	return *this;


}
//----------------------------------------
void CDoubleArray::Insert( int32_t* data, int32_t size )
{
	if ( size <= 0 )
	{
		return;
	}

	for ( int32_t i = 0; i < size; i++ )
	{
		if ( isDefaultValue( static_cast<int32_t>( data[ i ] ) ) )
		{
			this->Insert( defaultValue<double>() );
		}
		else
		{
			this->Insert( data[ i ] );
		}
	}
}
//----------------------------------------
void CDoubleArray::Insert( uint32_t* data, int32_t size )
{
	if ( size <= 0 )
	{
		return;
	}

	for ( int32_t i = 0; i < size; i++ )
	{
		if ( isDefaultValue( static_cast<uint32_t>( data[ i ] ) ) )
		{
			this->Insert( defaultValue<double>() );
		}
		else
		{
			this->Insert( data[ i ] );
		}
	}
}
//----------------------------------------
void CDoubleArray::Insert( double* data, int32_t size )
{
	if ( size <= 0 )
	{
		return;
	}

	for ( int32_t i = 0; i < size; i++ )
	{
		this->Insert( data[ i ] );
	}
}
//----------------------------------------
void CDoubleArray::Insert( const CDoubleArray& vect, bool bEnd /*= true*/ )
{
	if ( vect.empty() )
	{
		return;
	}

	if ( bEnd )
	{
		doublearray::insert( this->end(), vect.begin(), vect.end() );
	}
	else
	{
		doublearray::insert( this->begin(), vect.begin(), vect.end() );
	}


}
//----------------------------------------
void CDoubleArray::Insert( const CUInt8Array& vect, bool bEnd /*= true*/ )
{
	if ( vect.empty() )
	{
		return;
	}

	for ( uint32_t i = 0; i < vect.size(); i++ )
	{
		if ( bEnd )
		{
			doublearray::insert( this->end(), static_cast<double>( vect.at( i ) ) );
		}
		else
		{
			doublearray::insert( this->begin(), static_cast<double>( vect.at( i ) ) );
		}
	}


}

//----------------------------------------
void CDoubleArray::Insert( const CInt8Array& vect, bool bEnd /*= true*/ )
{
	if ( vect.empty() )
	{
		return;
	}

	for ( uint32_t i = 0; i < vect.size(); i++ )
	{
		if ( bEnd )
		{
			doublearray::insert( this->end(), static_cast<double>( vect.at( i ) ) );
		}
		else
		{
			doublearray::insert( this->begin(), static_cast<double>( vect.at( i ) ) );
		}
	}


}

//----------------------------------------
void CDoubleArray::Insert( const CInt16Array& vect, bool bEnd /*= true*/ )
{
	if ( vect.empty() )
	{
		return;
	}

	for ( uint32_t i = 0; i < vect.size(); i++ )
	{
		if ( bEnd )
		{
			doublearray::insert( this->end(), static_cast<double>( vect.at( i ) ) );
		}
		else
		{
			doublearray::insert( this->begin(), static_cast<double>( vect.at( i ) ) );
		}
	}


}
//----------------------------------------
void CDoubleArray::Insert( const CIntArray& vect, bool bEnd /*= true*/ )
{
	if ( vect.empty() )
	{
		return;
	}

	for ( uint32_t i = 0; i < vect.size(); i++ )
	{
		if ( bEnd )
		{
			doublearray::insert( this->end(), static_cast<double>( vect.at( i ) ) );
		}
		else
		{
			doublearray::insert( this->begin(), static_cast<double>( vect.at( i ) ) );
		}
	}


}
//----------------------------------------
void CDoubleArray::Insert( const CFloatArray& vect, bool bEnd /*= true*/ )
{
	if ( vect.empty() )
	{
		return;
	}

	for ( uint32_t i = 0; i < vect.size(); i++ )
	{
		if ( bEnd )
		{
			doublearray::insert( this->end(), static_cast<double>( vect.at( i ) ) );
		}
		else
		{
			doublearray::insert( this->begin(), static_cast<double>( vect.at( i ) ) );
		}
	}


}

//----------------------------------------
void CDoubleArray::Insert( const CDoubleArray& vect, int32_t first, int32_t last, bool bEnd /*= true*/ )
{
	if ( vect.empty() )
	{
		return;
	}

	if ( bEnd )
	{
		doublearray::insert( this->end(), vect.begin() + first, vect.begin() + last );
	}
	else
	{
		doublearray::insert( this->begin(), vect.begin() + first, vect.begin() + last );
	}


}
//----------------------------------------
//void CDoubleArray::Insert( const CStringArray& vect, bool bEnd /*= true*/ )
//{
//	if ( vect.empty() )
//	{
//		return;
//	}
//
//	for ( uint32_t i = 0; i < vect.size(); i++ )
//	{
//		double value;
//
//		std::istringstream myStream( vect.at( i ) );
//
//		if ( !( myStream >> value ) )
//		{
//			CTools::SetDefaultValue( value );
//		}
//
//
//		if ( bEnd )
//		{
//			doublearray::insert( this->end(), value );
//		}
//		else
//		{
//			doublearray::insert( this->begin(), value );
//		}
//	}
//
//
//}
//----------------------------------------
void CDoubleArray::Insert( const std::string& vect, const std::string& delim /*= "," */, bool bEnd /*= true*/ )
{
	if ( vect.empty() )
	{
		return;
	}
	std::string value = vect;

	if ( value.at( 0 ) == '[' )
	{
		value.erase( 0, 1 );
	}

	if ( value.at( value.size() - 1 ) == ']' )
	{
		value.erase( value.size() - 1, 1 );
	}

	//TODO: RCCC

	//CStringArray stringArray;
	//stringArray.ExtractStrings( value, delim );

	//this->Insert( stringArray, bEnd );

	std::vector< std::string > v;
	String2Vector( v, value, delim );
	for ( auto s : v )
		push_back( s2n<double>( s ) );
}

//----------------------------------------

void CDoubleArray::Insert( const double value )
{

	doublearray::push_back( value );

}
//----------------------------------------
void CDoubleArray::Insert( const int32_t value )
{

	double doubleValue = static_cast<double>( value );
	doublearray::push_back( doubleValue );

}
//----------------------------------------
void CDoubleArray::Insert( const uint32_t value )
{

	double doubleValue = static_cast<double>( value );
	doublearray::push_back( doubleValue );

}
//----------------------------------------
void CDoubleArray::Insert( const int16_t value )
{

	double doubleValue = static_cast<double>( value );
	doublearray::push_back( doubleValue );

}
//----------------------------------------
void CDoubleArray::Insert( const uint16_t value )
{

	double doubleValue = static_cast<double>( value );
	doublearray::push_back( doubleValue );

}
//----------------------------------------
void CDoubleArray::Insert( const int8_t value )
{

	double doubleValue = static_cast<double>( value );
	doublearray::push_back( doubleValue );

}
//----------------------------------------
void CDoubleArray::Insert( const uint8_t value )
{

	double doubleValue = static_cast<double>( value );
	doublearray::push_back( doubleValue );

}

//----------------------------------------
CDoubleArray::iterator CDoubleArray::InsertAt( CDoubleArray::iterator where, const double value )
{
	return doublearray::insert( where, value );
}
//----------------------------------------
CDoubleArray::iterator CDoubleArray::InsertAt( int32_t index, const double value )
{
	if ( index < 0 )
	{
		return this->end();
	}

	if ( (uint32_t)index >= this->size() )
	{
		this->resize( index + 1 );
		( *this )[ index ] = value;
		return this->begin() + index;
	}

	return doublearray::insert( this->begin() + index, value );
}


//----------------------------------------
bool CDoubleArray::Erase( CDoubleArray::iterator where )
{
	doublearray::erase( where );
	return true;
}
//----------------------------------------
CDoubleArray::iterator CDoubleArray::ReplaceAt( int32_t index, const double value )
{
	if ( index < 0 )
	{
		return this->end();
	}

	( *this )[ index ] = value;
	return this->begin() + index;

}
//----------------------------------------
CDoubleArray::iterator CDoubleArray::ReplaceAt( CDoubleArray::iterator where, const double value )
{
	Erase( where );
	return InsertAt( where, value );
}
//----------------------------------------
bool CDoubleArray::Intersect( const CDoubleArray& array, CDoubleArray& intersect ) const
{
	intersect.clear();

	CDoubleArray::const_iterator itA;
	CDoubleArray::const_iterator itB;

	for ( itA = this->begin(); itA != this->end(); itA++ )
	{
		for ( itB = array.begin(); itB != array.end(); itB++ )
		{
			if ( ( *itA ) == ( *itB ) )
			{
				intersect.Insert( ( *itB ) );
			}
		}
	}

	return ( !( intersect.empty() ) );
}
//----------------------------------------
int32_t CDoubleArray::FindIndex( double value ) const
{
	int32_t index = -1;
	int32_t size = static_cast<int32_t>( this->size() );

	for ( int32_t i = 0; i < size; i++ )
	{
		if ( areEqual( value, this->at( i ) ) )
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

	double* newArray = new double[ this->size() ];

	for ( uint32_t i = 0; i < this->size(); i++ )
	{
		newArray[ i ] = this->at( i );
	}


	return newArray;

}


//----------------------------------------
std::string CDoubleArray::ToString( const std::string& delim /*= "," */, bool useBracket /*= true */ ) const
{
	std::ostringstream myStream;

	if ( useBracket )
	{
		myStream << "[";
	}


	CDoubleArray::const_iterator it;
	for ( it = this->begin(); it != this->end(); it++ )
	{
		myStream << *it;
		if ( ( it + 1 ) != this->end() )
		{
			myStream << delim;
		}
	}
	if ( useBracket )
	{
		myStream << "]";
	}


	return myStream.str();

}

//----------------------------------------
void CDoubleArray::GetRange( double& min, double& max )
{

	setDefaultValue( min );
	setDefaultValue( max );

	CDoubleArray::iterator it;

	for ( it = begin(); it != end(); it++ )
	{
		if ( it == begin() )
		{
			min = *it;
			max = *it;
		}
		else
		{
			min = ( min > *it ) ? *it : min;
			max = ( max > *it ) ? max : *it;
		}
	}

}
//----------------------------------------
bool CDoubleArray::operator ==( const CDoubleArray& vect )
{
	size_t count = this->size();

	if ( count != vect.size() )
	{
		return false;
	}

	if ( count == 0 )
	{
		return true;
	}

	bool isEqual = true;

	for ( uint32_t i = 0; i < count; i++ )
	{
		if ( ! areEqual( this->at( i ), vect.at( i ) ) )
		{
			isEqual = false;
			break;
		}
	}

	return isEqual;
}


//----------------------------------------
void CDoubleArray::Dump( std::ostream& fOut /* = std::cerr */ ) const
{

	if ( CTrace::IsTrace() == false )
	{
		return;
	}

	CDoubleArray::const_iterator it;
	int i = 0;

	fOut << "==> Dump a CDoubleArray Object at " << this << " with " << size() << " elements" << std::endl;

	for ( it = this->begin(); it != this->end(); it++ )
	{
		fOut << "CDoubleArray[" << i << "]= " << n2s<std::string>( *it, floatPrecision ) << std::endl;
		i++;
	}

	fOut << "==> END Dump a CDoubleArray Object at " << this << " with " << size() << " elements" << std::endl;


	fOut << std::endl;


}





////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//						Algorithm Parameters
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------
//------------------- CBratAlgorithmParam class --------------------
//-------------------------------------------------------------

//----------------------------------------
CBratAlgorithmParam::CBratAlgorithmParam()
{
	Init();
}

//----------------------------------------
CBratAlgorithmParam::CBratAlgorithmParam( const CBratAlgorithmParam &o )
{
	Init();
	Set( o );
}

//----------------------------------------
CBratAlgorithmParam::~CBratAlgorithmParam()
{
}

//----------------------------------------
const CBratAlgorithmParam& CBratAlgorithmParam::operator=( const CBratAlgorithmParam &o )
{
	if ( this == &o )
	{
		return *this;
	}

	Set( o );

	return *this;
}
//----------------------------------------
void CBratAlgorithmParam::Init()
{
	m_typeVal = CBratAlgorithmParam::T_UNDEF;
	setDefaultValue( m_valDouble );
	setDefaultValue( m_valFloat );
	setDefaultValue( m_valInt );
	setDefaultValue( m_valLong );
	m_valChar = '\0';
	m_vectValDouble.RemoveAll();
}

//----------------------------------------
void CBratAlgorithmParam::Set( const CBratAlgorithmParam &o )
{
	m_typeVal = o.m_typeVal;

	m_valDouble = o.m_valDouble;
	m_valFloat = o.m_valFloat;
	m_valInt = o.m_valInt;
	m_valLong = o.m_valLong;
	m_valString = o.m_valString;
	m_valChar = o.m_valChar;

	Copy( o.m_vectValDouble );
}
//----------------------------------------
void CBratAlgorithmParam::SetValue( double value, CBratAlgorithmParam::bratAlgoParamTypeVal type )
{
	switch ( type )
	{
		case CBratAlgorithmParam::T_INT:
		{
			int32_t v = static_cast<int32_t>( value );
			if ( isDefaultValue( value ) )
			{
				setDefaultValue( v );
			}
			SetValue( v );
			break;
		}
		case CBratAlgorithmParam::T_LONG:
		{
			int64_t v = static_cast<int64_t>( value );
			if ( isDefaultValue( value ) )
			{
				setDefaultValue( v );
			}
			SetValue( v );
			break;
		}
		case CBratAlgorithmParam::T_FLOAT:
		{
			float v = static_cast<float>( value );
			if ( isDefaultValue( value ) )
			{
				setDefaultValue( v );
			}
			SetValue( v );
			break;
		}
		case CBratAlgorithmParam::T_DOUBLE:
		{
			SetValue( value );
			break;
		}
		case CBratAlgorithmParam::T_CHAR:
		{
			SetValue( static_cast<char>( value ) );
			break;
		}
		default:
		{
			throw CException(
				"CBratAlgorithmParam#SetValue : Setting value from double to Brat algorithm type "
				+ n2s<std::string>( static_cast<int32_t>( type ) )
				+ " is not valid",
				BRATHL_LOGIC_ERROR );
			break;
		}
	}
}
//----------------------------------------
void CBratAlgorithmParam::SetValue( const std::string& value, CBratAlgorithmParam::bratAlgoParamTypeVal type )
{
	switch ( type )
	{
		case CBratAlgorithmParam::T_INT:
		case CBratAlgorithmParam::T_LONG:
		case CBratAlgorithmParam::T_FLOAT:
		case CBratAlgorithmParam::T_DOUBLE:
		case CBratAlgorithmParam::T_CHAR:
		{
			SetValue( s2n< double >( value ), type );
			break;
		}
		case CBratAlgorithmParam::T_STRING:
		{
			SetValue( value );
			break;
		}
		case CBratAlgorithmParam::T_VECTOR_DOUBLE:
		{
			SetValueAsDoubleArray( value );
			break;
		}
		default:
		{
			throw CException(
				"CBratAlgorithmParam#SetValue : Setting value from std::string to Brat algorithm type "
				+ n2s<std::string>( static_cast<int32_t>( type ) )
				+ " is not valid",
				BRATHL_LOGIC_ERROR );
			break;
		}
	}
}

//----------------------------------------
void CBratAlgorithmParam::SetValue( double value )
{
	m_typeVal = CBratAlgorithmParam::T_DOUBLE;
	m_valDouble = value;
}
//----------------------------------------
void CBratAlgorithmParam::SetValue( float value )
{
	m_typeVal = CBratAlgorithmParam::T_FLOAT;
	m_valFloat = value;
}
//----------------------------------------
void CBratAlgorithmParam::SetValue( int32_t value )
{
	m_typeVal = CBratAlgorithmParam::T_INT;
	m_valInt = value;
}
//----------------------------------------
void CBratAlgorithmParam::SetValue( int64_t value )
{
	m_typeVal = CBratAlgorithmParam::T_LONG;
	m_valLong = value;
}
//----------------------------------------
void CBratAlgorithmParam::SetValue( const std::string& value )
{
	m_typeVal = CBratAlgorithmParam::T_STRING;
	m_valString = value;
}

//----------------------------------------
void CBratAlgorithmParam::SetValue( unsigned char value )
{
	m_typeVal = CBratAlgorithmParam::T_CHAR;
	m_valChar = value;
}
//----------------------------------------
void CBratAlgorithmParam::SetValue( const CDoubleArray& value )
{
	m_typeVal = CBratAlgorithmParam::T_VECTOR_DOUBLE;
	Copy( value );
}

//----------------------------------------
void CBratAlgorithmParam::SetValueAsDoubleArray( const std::string& value, const std::string& delim /*= "," */ )
{
	m_typeVal = CBratAlgorithmParam::T_VECTOR_DOUBLE;
	m_vectValDouble.RemoveAll();
	m_vectValDouble.Insert( value );
}
//----------------------------------------
void CBratAlgorithmParam::Copy( const CDoubleArray& value )
{
	m_vectValDouble.RemoveAll();
	m_vectValDouble.Insert( value );
}
//----------------------------------------
void CBratAlgorithmParam::SetValue( double* value, int32_t nbValues )
{
	m_typeVal = CBratAlgorithmParam::T_VECTOR_DOUBLE;

	m_vectValDouble.RemoveAll();

	for ( int32_t i = 0; i < nbValues; i++ )
	{
		m_vectValDouble.Insert( value[ i ] );
	}
}
//----------------------------------------
std::string CBratAlgorithmParam::TypeValAsString( CBratAlgorithmParam::bratAlgoParamTypeVal type )
{
	std::string value;
	switch ( type )
	{
		case T_UNDEF: value = "undefined"; break;
		case T_INT: value = "integer"; break;
		case T_LONG: value = "long"; break;
		case T_FLOAT: value = "float"; break;
		case T_DOUBLE: value = "double"; break;
		case T_CHAR: value = "char"; break;
		case T_STRING: value = "std::string"; break;
		case T_VECTOR_DOUBLE: value = "array of double"; break;
		default: value =
			"CBratAlgorithmParam#TypeValAsString: unknown type "
			+ n2s<std::string>( static_cast<uint32_t>( type ) )
			+ ".";
			break;
	}
	return value;
}
//----------------------------------------
std::string CBratAlgorithmParam::GetValue() const
{
	std::string value;

	switch ( m_typeVal )
	{
		case CBratAlgorithmParam::T_INT:
		{
			value = n2s<std::string>( GetValueAsInt() );
			break;
		}
		case CBratAlgorithmParam::T_LONG:
		{
			value = n2s<std::string>( GetValueAsLong() );
			break;
		}
		case CBratAlgorithmParam::T_FLOAT:
		{
			value = n2s<std::string>( this->GetValueAsFloat(), floatPrecision );
			break;
		}
		case CBratAlgorithmParam::T_DOUBLE:
		{
			value = n2s<std::string>( this->GetValueAsDouble(), floatPrecision );
			break;
		}
		case CBratAlgorithmParam::T_CHAR:
		{
			value = this->GetValueAsChar();
			break;
		}
		case CBratAlgorithmParam::T_STRING:
		{
			value = this->GetValueAsString();
			break;
		}
		case CBratAlgorithmParam::T_VECTOR_DOUBLE:
		{
			value = this->GetValueAsVectDouble()->ToString();
			break;
		}
		default:
		{
			throw CException(
				"CBratAlgorithmParam#GetValue : GetValue value as std::string from Brat algorithm type "
				+ n2s<std::string>( static_cast<int32_t>( m_typeVal ) )
				+ " is not valid",
				BRATHL_LOGIC_ERROR );
			break;
		}
	}

	return value;
}

//----------------------------------------
void CBratAlgorithmParam::Dump( std::ostream& fOut /* = std::cerr */ )
{

	if ( CTrace::IsTrace() == false )
	{
		return;
	}

	fOut << "==> Dump a CBratAlgorithmParam Object at " << this << std::endl;
	fOut << "m_typeVal: " << m_typeVal << std::endl;
	fOut << "m_valDouble: " << m_valDouble << std::endl;
	fOut << "m_valInt: " << m_valInt << std::endl;
	fOut << "m_valFloat: " << m_valFloat << std::endl;
	fOut << "m_valString: " << m_valString << std::endl;
	fOut << "m_valChar: " << m_valChar << std::endl;
	fOut << "m_vectValDouble: " << std::endl;
	m_vectValDouble.Dump( fOut );
	fOut << "==> END Dump a CBratAlgorithmParam Object at " << this << " elements" << std::endl;


	fOut << std::endl;

}
//-------------------------------------------------------------
//------------------- CVectorBratAlgorithmParam class --------------------
//-------------------------------------------------------------

CVectorBratAlgorithmParam::CVectorBratAlgorithmParam()
{
	Init();
}


//----------------------------------------
CVectorBratAlgorithmParam::~CVectorBratAlgorithmParam()
{
	RemoveAll();
}
//----------------------------------------
void CVectorBratAlgorithmParam::Init()
{
}

//----------------------------------------
void CVectorBratAlgorithmParam::Insert( const CBratAlgorithmParam& o )
{
	vectorbratalgorithmparam::push_back( o );
}

//----------------------------------------
void CVectorBratAlgorithmParam::RemoveAll()
{
	vectorbratalgorithmparam::clear();

}
//----------------------------------------
void CVectorBratAlgorithmParam::Insert( double value, CBratAlgorithmParam::bratAlgoParamTypeVal type )
{
	CBratAlgorithmParam p;
	p.SetValue( value, type );
	this->Insert( p );
}
//----------------------------------------
void CVectorBratAlgorithmParam::Insert( const std::string& value, CBratAlgorithmParam::bratAlgoParamTypeVal type )
{
	CBratAlgorithmParam p;
	p.SetValue( value, type );
	this->Insert( p );
}

//----------------------------------------
void CVectorBratAlgorithmParam::Insert( double value )
{
	CBratAlgorithmParam p;
	p.SetValue( value );
	this->Insert( p );
}
//----------------------------------------
void CVectorBratAlgorithmParam::Insert( float value )
{
	CBratAlgorithmParam p;
	p.SetValue( value );
	this->Insert( p );
}
//----------------------------------------
void CVectorBratAlgorithmParam::Insert( int32_t value )
{
	CBratAlgorithmParam p;
	p.SetValue( value );
	this->Insert( p );
}
//----------------------------------------
void CVectorBratAlgorithmParam::Insert( const std::string& value )
{
	CBratAlgorithmParam p;
	p.SetValue( value );
	this->Insert( p );
}
//----------------------------------------
void CVectorBratAlgorithmParam::Insert( unsigned char value )
{
	CBratAlgorithmParam p;
	p.SetValue( value );
	this->Insert( p );
}
//----------------------------------------
void CVectorBratAlgorithmParam::Insert( const CDoubleArray& value )
{
	CBratAlgorithmParam p;
	p.SetValue( value );
	this->Insert( p );
}

//----------------------------------------
void CVectorBratAlgorithmParam::Insert( double* value, int32_t nbValues )
{
	CBratAlgorithmParam p;
	p.SetValue( value, nbValues );
	this->Insert( p );
}
//----------------------------------------
std::string CVectorBratAlgorithmParam::ToString( const std::string& delim /*= "," */, bool useBracket /*= true */ ) const
{
	std::ostringstream myStream;

	if ( useBracket )
	{
		myStream << "(";
	}

	//CVectorBratAlgorithmParam::iterator it;

	for ( auto it = cbegin(); it != cend(); it++ )
	{
		myStream << it->GetValue();
		if ( ( it + 1 ) != this->end() )
		{
			myStream << delim;
		}
	}

	if ( useBracket )
	{
		myStream << ")";
	}


	return myStream.str();



}
//----------------------------------------
void CVectorBratAlgorithmParam::Dump( std::ostream& fOut /* = std::cerr */ )
{

	if ( CTrace::IsTrace() == false )
	{
		return;
	}

	CVectorBratAlgorithmParam::iterator it;
	int i = 0;

	fOut << "==> Dump a CVectorBratAlgorithmParam Object at " << this << " with " << size() << " elements" << std::endl;

	for ( it = this->begin(); it != this->end(); it++ )
	{
		fOut << "CVectorBratAlgorithmParam[" << i << "]= " << std::endl;
		( *it ).Dump( fOut );
		i++;
	}

	fOut << "==> END Dump a CVectorBratAlgorithmParam Object at " << this << " with " << size() << " elements" << std::endl;


	fOut << std::endl;

}

