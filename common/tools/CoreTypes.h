/*
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
#if !defined(ALGORITHM_PARAMS_H)
#define ALGORITHM_PARAMS_H


#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <limits>
#include <math.h>

#include "common/+Utils.h" 
#include "common/ccore-types.h" 
#include "common/tools/brathl_error.h" 



////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//							Numeric Constants
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


////////////////////
//	Default Values	
////////////////////

#if defined(max)
#undef max
#endif


//	!!! IMPORTANT !!! THESE CONSTANTS MUST BE DELETED (or encapsulated in the case of floating points) AFTER TESTS

const int8_t m_defaultValueINT8 = 0x7F;
const int8_t defaultValueINT8 = std::numeric_limits<int8_t>::max();

// default values for unsigned int 8 bits
const uint8_t m_defaultValueUINT8 = 0xFFU;
const uint8_t defaultValueUINT8 = std::numeric_limits<uint8_t>::max();

// default values for int 16 bits
const int16_t m_defaultValueINT16 = 0x7FFF;
const int16_t defaultValueINT16 = std::numeric_limits<int16_t>::max();

// default values for unsigned int 16 bits
const uint16_t m_defaultValueUINT16 = 0xFFFFU;
const uint16_t defaultValueUINT16 = std::numeric_limits<uint16_t>::max();


// default values for int 32 bits
const int32_t m_defaultValueINT32 = 0x7FFFFFFF;
const int32_t defaultValueINT32 = std::numeric_limits<int32_t>::max();

// default values for unsigned int 32 bits
const uint32_t m_defaultValueUINT32 = 0xFFFFFFFFU;
const uint32_t defaultValueUINT32 = std::numeric_limits<uint32_t>::max();

// default values for int 64 bits
#ifdef WIN32
const int64_t m_defaultValueINT64 = 0x7FFFFFFFFFFFFFFF;
#else
const int64_t m_defaultValueINT64 = 0x7FFFFFFFFFFFFFFFLL;
#endif
const int64_t defaultValueINT64 = std::numeric_limits<int64_t>::max();

// default values for unsigned int 64 bits
#ifdef WIN32
const uint64_t m_defaultValueUINT64 = 0xFFFFFFFFFFFFFFFFU;
#else
const uint64_t m_defaultValueUINT64 = 0xFFFFFFFFFFFFFFFFULL;
#endif
const uint64_t defaultValueUINT64 = std::numeric_limits<uint64_t>::max();


// default values for double
const double DEFAULT_VALUE_DOUBLE = 18446744073709551616.0;
const double defaultValueDOUBLE = std::numeric_limits<double>::max();

// default values for float
const float DEFAULT_VALUE_FLOAT = 18446744073709551616.0F;
const float defaultValueFLOAT = std::numeric_limits<float>::max();



// get defaults

template< typename T > 
inline 
T defaultValue()
{
	assert__( defaultValueINT8  == m_defaultValueINT8 );
	assert__( defaultValueUINT8 == m_defaultValueUINT8 );
	assert__( defaultValueINT16 == m_defaultValueINT16 );
	assert__( defaultValueUINT16 == m_defaultValueUINT16 );

	assert__( defaultValueINT32 == m_defaultValueINT32 );
	assert__( defaultValueUINT32 == m_defaultValueUINT32 );
	assert__( defaultValueINT64 == m_defaultValueINT64 );
	assert__( defaultValueUINT64 == m_defaultValueUINT64 );

	return std::numeric_limits< T >::max();
}
template<>
inline 
float defaultValue()
{
	assert__( defaultValueFLOAT != DEFAULT_VALUE_FLOAT );

	return DEFAULT_VALUE_FLOAT;
}
template<>
inline 
double defaultValue()
{
	assert__( defaultValueDOUBLE != DEFAULT_VALUE_DOUBLE );

	return DEFAULT_VALUE_DOUBLE;
}

// test defaults

template< typename T > 
inline 
bool isDefaultValue( const T &n )
{
	return n == defaultValue< T >();
}

// set defaults

template< typename T >
inline 
T setDefaultValue( T &n )
{
	return ( n = defaultValue<T>() );
}
template<>
inline 
float setDefaultValue( float &n )
{
	return ( n = defaultValue<float>() );
}
template<>
inline 
double setDefaultValue( double &n )
{
	return ( n = defaultValue<double>() );
}




////////////////////////////////////////
//	Floating Point Reference Constants
////////////////////////////////////////


// maximum difference used to declare that two real values are equal
const double CompareEpsilon = 1.0E-70;


inline bool isZero( double X )
{
    return fabs( X ) < CompareEpsilon;
}


inline bool areEqual( double X, double Y, double epsilon )
{
	static const double doubleDefault = defaultValue< double >();

	if ( X == doubleDefault )
	{
		return Y == doubleDefault;
	}
	if ( Y == doubleDefault )
	{
		return false;
	}

	return fabs( X - Y ) < epsilon;
}


inline bool areEqual( double X, double Y )
{
	return areEqual( X, Y, CompareEpsilon );
}




////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//						Numeric Array Types
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------
//------------------- CStringArray class --------------------
//-------------------------------------------------------------


  
/** \addtogroup tools Tools
  @{ */

/** 
  An array (std::vector) of strings management class.


 \version 1.0
*/

class CIntArray;
class CUIntArray;

class CStringArray : public std::vector<std::string>
{
public:
	/// Empty CStringArray ctor
	CStringArray()
	{}

	/** Creates new CStringArray object from another CStringList
	  \param std::list [in] : std::list to be copied */
        CStringArray( const CStringArray& v ) : std::vector<std::string>( v )
	{}
        CStringArray( const std::vector<std::string>& v ): std::vector<std::string>( v )
	{}

	/// Destructor
	virtual ~CStringArray()
	{}

	virtual bool Exists( const std::string& str, bool compareNoCase = false ) const;

	virtual int32_t FindIndex( const std::string& str, bool compareNoCase = false ) const;
	virtual void FindIndexes( const std::string& str, CIntArray& indexes, bool compareNoCase = false ) const;

	virtual void GetValues( const CIntArray& indexes, CStringArray& values ) const;
	virtual void GetValues( const CIntArray& indexes, std::string& values ) const;

	virtual void Insert( const CStringArray& vect, bool bEnd = true );
	virtual void Insert( const std::string& str );
        virtual void Insert( const std::vector<std::string>& vect, bool bEnd = true );
	virtual void Insert( const CIntArray& vect );
	virtual void Insert(const std::list<std::string> &lst);

	virtual CStringArray::iterator InsertAt( CStringArray::iterator where, const std::string& str );
	virtual CStringArray::iterator InsertAt( int32_t index, const std::string& str );

	virtual CStringArray::iterator ReplaceAt( int32_t index, const std::string& str );
	virtual CStringArray::iterator ReplaceAt( uint32_t index, const std::string& str );
	virtual CStringArray::iterator ReplaceAt( CStringArray::iterator where, const std::string& str );
	virtual bool Erase( CStringArray::iterator it );
	virtual bool Erase( int32_t index );
	virtual bool Erase( uint32_t index );

	virtual bool Remove( const std::string& array, bool compareNoCase = false );
	virtual bool Remove( const CStringArray& array, bool compareNoCase = false );

	/** Remove all elements and clear the std::list*/
	virtual void RemoveAll() { clear(); }

	virtual void ExtractStrings( const std::string& str, const char delim, bool bRemoveAll = true, bool insertUnique = false );
	virtual void ExtractStrings( const std::string& str, const std::string& delim, bool bRemoveAll = true, bool insertUnique = false );

	virtual void ExtractKeys( const std::string& str, const std::string& delim, bool bRemoveAll = true );

	virtual bool Intersect( const std::string& str, CStringArray& intersect, bool compareNoCase = false ) const;
	virtual bool Intersect( const CStringArray& array, CStringArray& intersect, bool compareNoCase = false ) const;
	virtual bool Intersect( const std::string& str, CUIntArray& intersect, bool compareNoCase = false ) const;
	virtual bool Intersect( const CStringArray& array, CUIntArray& intersect, bool compareNoCase = false ) const;
	virtual bool Complement( const CStringArray& array, CStringArray& complement ) const;

	virtual void Replace( const CStringArray& findString, const std::string& replaceBy, CStringArray& replaced, bool compareNoCase = false, bool insertUnique = false ) const;
	virtual void Replace( const std::string& findString, const std::string& replaceBy, CStringArray& replaced, bool compareNoCase = false, bool insertUnique = false ) const;

        virtual void InsertUnique( const std::string& str );
	virtual void InsertUnique( const CStringArray* vect );
	virtual void InsertUnique( const CStringArray& vect );
        virtual void InsertUnique( const std::vector<std::string>& vect );
	virtual void InsertUnique(const std::list<std::string> &lst);

	virtual std::string ToString( const std::string& delim = ",", bool useBracket = true ) const;

	/** Inequality operator overload
		Array are unequal if they are not equal*/
        virtual bool operator !=( const CStringArray& vect ) { return !( *this == vect ); }

	/** Copy a new CStringArray to the object */
	virtual const CStringArray& operator= (const std::list<std::string> & lst);

	///Dump fonction
	virtual void Dump( std::ostream& fOut = std::cerr ) const;
};


//-------------------------------------------------------------
//------------------- CIntArray class --------------------
//-------------------------------------------------------------


/** \addtogroup tools Tools
  @{ */

/**
  An array (std::vector) of ints management class.


  \version 1.0
  */
class CIntArray : public std::vector<int32_t>
{
	typedef std::vector< int32_t > int32array;

public:

	/// Empty CIntArray ctor
	CIntArray()
	{}

	/** Creates new CIntArray object from another CStringList
	  \param std::list [in] : std::list to be copied */
	CIntArray( const CIntArray& vect ) : int32array(vect)
	{}

	/// Destructor
	virtual ~CIntArray()
	{}

	virtual void Insert( const CIntArray& vect, bool bEnd = true )
	{
		if ( !vect.empty() )
			int32array::insert( bEnd ? end() : begin(), vect.begin(), vect.end() );
	}

	virtual void Insert(const CStringArray& vect)
	{
		for ( auto it = vect.begin(); it != vect.end(); it++ )
		{
			int32_t value = s2n<int32_t>( *it );
			if ( !( isDefaultValue( value ) ) )
			{
				this->Insert( value );
			}
		}
	}

	void Insert( int32_t* vect, size_t length )
	{
		for ( size_t i = 0; i < length; i++ )
			Insert( vect[ i ] );
	}

	virtual void Insert( const int32_t value ){ int32array::push_back(value); }

	virtual CIntArray::iterator InsertAt( CIntArray::iterator where, const int32_t value ){ return int32array::insert(where, value); }

	virtual CIntArray::iterator InsertAt( int32_t index, const int32_t value );

	virtual CIntArray::iterator ReplaceAt( CIntArray::iterator where, const int32_t value )
	{
		Erase( where );
		return InsertAt( where, value );
	}
	virtual CIntArray::iterator ReplaceAt( int32_t index, const int32_t value );

	virtual bool Erase( CIntArray::iterator where )
	{
		int32array::erase( where );
		return true;
	}

	virtual bool Intersect( const CIntArray& array, CIntArray& intersect ) const;

	virtual void IncrementValue( uint32_t incr = 1 )
	{
		for ( auto it = begin(); it != end(); it++ )
			( *it ) += incr;
	}

	virtual int32_t* ToArray() const
	{
		int32_t* newArray = new int32_t[ this->size() ];

		for ( uint32_t i = 0; i < this->size(); i++ )
		{
			newArray[ i ] = this->at( i );
		}

		return newArray;
	}

	virtual void RemoveAll() { this->clear(); }

	virtual std::string ToString( const std::string& delim = ",", bool useBracket = true ) const;

	///Dump fonction
	virtual void Dump( std::ostream& fOut = std::cerr ) const;
};


//-------------------------------------------------------------
//------------------- CInt64Array class --------------------
//-------------------------------------------------------------


/** \addtogroup tools Tools
  @{ */

/**
  An array (std::vector) of ints management class.


  \version 1.0
  */
class CInt64Array : public std::vector<int64_t>
{
	typedef std::vector<int64_t> int64array;

public:
	/// Empty CInt64Array ctor
	CInt64Array()
	{}

	/** Creates new CInt64Array object from another CInt64Array
	  \param v [in] : std::vector to be copied */
	CInt64Array( const CInt64Array& v ) : int64array( v )
	{}

	// Destructor
	virtual ~CInt64Array()
	{}

	virtual void Insert( const CInt64Array& vect, bool bEnd = true );

	virtual void Insert( int64_t* vect, size_t length );

	virtual void Insert( const int64_t value );
	virtual CInt64Array::iterator InsertAt( CInt64Array::iterator where, const int64_t value );
	virtual CInt64Array::iterator InsertAt( size_t index, const int64_t value );

	virtual CInt64Array::iterator ReplaceAt( CInt64Array::iterator where, const int64_t value );
	virtual CInt64Array::iterator ReplaceAt( size_t index, const int64_t value );

	virtual bool Erase( CInt64Array::iterator it );

	virtual bool Intersect( const CInt64Array& array, CInt64Array& intersect ) const;

	virtual void IncrementValue( uint64_t incr = 1 );

	virtual std::string ToString( const std::string& delim = ",", bool useBracket = true ) const;

	virtual int64_t* ToArray();

        virtual void RemoveAll() { this->clear(); }

	///Dump fonction
	virtual void Dump( std::ostream& fOut = std::cerr ) const;
};



//-------------------------------------------------------------
//------------------- CUIntArray class --------------------
//-------------------------------------------------------------


/** \addtogroup tools Tools
  @{ */

/**
  An array (std::vector) of ints management class.


  \version 1.0
  */
class CUIntArray : public std::vector<uint32_t>
{
	typedef std::vector<uint32_t> uint32array;

public:

	/// Empty CUIntArray ctor
	CUIntArray()
	{}

	/** Creates new CUIntArray object from another CStringList
	  \param std::list [in] : std::list to be copied */
	CUIntArray( const CUIntArray& v ) : uint32array( v )
	{}

	/// Destructor
	virtual ~CUIntArray()
	{}

	virtual void Insert( CUIntArray* vect, bool bEnd = true );
	virtual void Insert( const CUIntArray& vect, bool bEnd = true );
	virtual void Insert( const std::vector<uint32_t>& vect, bool bEnd = true );
	virtual void Insert( uint32_t* vect, size_t length );

	virtual void Insert( const uint32_t value );

	virtual CUIntArray::iterator InsertAt( CUIntArray::iterator where, const uint32_t value );
	virtual CUIntArray::iterator InsertAt( int32_t index, const uint32_t value );

	virtual CUIntArray::iterator ReplaceAt( CUIntArray::iterator where, const uint32_t value );
	virtual CUIntArray::iterator ReplaceAt( int32_t index, const uint32_t value );

	virtual bool Erase( CUIntArray::iterator it );

	virtual bool Intersect( const CUIntArray& array, CUIntArray& intersect ) const;
	virtual bool Complement( const CUIntArray& array, CUIntArray& complement ) const;

	virtual std::string ToString( const std::string& delim = ",", bool useBracket = true ) const;

	virtual uint32_t* ToArray();
	virtual int32_t* ToIntArray();
	virtual size_t* ToSizeTArray();

        virtual void RemoveAll() { this->clear(); }

	uint32_t GetProductValues() const;

	///Dump fonction
	virtual void Dump( std::ostream& fOut = std::cerr ) const;
};


//-------------------------------------------------------------
//------------------- CUInt64Array class --------------------
//-------------------------------------------------------------


/** \addtogroup tools Tools
  @{ */

/**
  An array (std::vector) of ints management class.


  \version 1.0
  */
class CUInt64Array : public std::vector<uint64_t>
{
	typedef std::vector<uint64_t> uint64array;

public:
	/// Empty CUIntArray ctor
	CUInt64Array()
	{}

	/** Creates new CUInt64Array object from another CStringList
	  \param std::list [in] : std::list to be copied */
	CUInt64Array( const CUInt64Array& v ) : uint64array( v )
	{}

	/// Destructor
	virtual ~CUInt64Array()
	{}

	virtual void Insert( CUInt64Array* vect, bool bEnd = true );
	virtual void Insert( const CUInt64Array& vect, bool bEnd = true );
	virtual void Insert( const std::vector<uint64_t>& vect, bool bEnd = true );
	virtual void Insert( uint64_t* vect, size_t length );

	virtual void Insert( const uint64_t value );

	virtual CUInt64Array::iterator InsertAt( CUInt64Array::iterator where, const uint64_t value );
	virtual CUInt64Array::iterator InsertAt( size_t index, const uint64_t value );

	virtual CUInt64Array::iterator ReplaceAt( CUInt64Array::iterator where, const uint64_t value );
	virtual CUInt64Array::iterator ReplaceAt( size_t index, const uint64_t value );

	virtual bool Erase( CUInt64Array::iterator it );

	virtual bool Intersect( const CUInt64Array& array, CUInt64Array& intersect ) const;

	virtual std::string ToString( const std::string& delim = ",", bool useBracket = true ) const;

	virtual uint64_t* ToArray();

        virtual void RemoveAll() { this->clear(); }

	uint64_t GetProductValues() const;

	///Dump fonction
	virtual void Dump( std::ostream& fOut = std::cerr ) const;
};

//-------------------------------------------------------------
//------------------- CInt16Array class --------------------
//-------------------------------------------------------------


/** \addtogroup tools Tools
  @{ */

/**
  An array (std::vector) of ints management class.


  \version 1.0
  */
class CInt16Array : public std::vector<int16_t>
{
	typedef std::vector<int16_t> int16array;

public:
	/// Empty CInt16Array ctor
	CInt16Array()
	{}

	/** Creates new CInt16Array object from another CStringList
	  \param std::list [in] : std::list to be copied */
	CInt16Array( const CInt16Array& v ) : int16array( v )
	{}

	/// Destructor
	virtual ~CInt16Array()
	{}

	virtual void Insert( const CInt16Array& vect, bool bEnd = true );

	virtual void Insert( int16_t* vect, size_t length );

	virtual void Insert( const int16_t value );
	virtual CInt16Array::iterator InsertAt( CInt16Array::iterator where, const int16_t value );
	virtual CInt16Array::iterator InsertAt( int32_t index, const int16_t value );

	virtual CInt16Array::iterator ReplaceAt( CInt16Array::iterator where, const int16_t value );
	virtual CInt16Array::iterator ReplaceAt( int32_t index, const int16_t value );

	virtual bool Erase( CInt16Array::iterator it );

	virtual bool Intersect( const CInt16Array& array, CInt16Array& intersect ) const;

	virtual std::string ToString( const std::string& delim = ",", bool useBracket = true ) const;

	virtual int16_t* ToArray();

        virtual void RemoveAll() { this->clear(); }

	///Dump fonction
	virtual void Dump( std::ostream& fOut = std::cerr ) const;
};

//-------------------------------------------------------------
//------------------- CUInt16Array class --------------------
//-------------------------------------------------------------


/** \addtogroup tools Tools
  @{ */

/**
  An array (std::vector) of ints management class.


  \version 1.0
  */
class CUInt16Array : public std::vector<uint16_t>
{
	typedef std::vector<uint16_t> uint16array;
public:
	/// Empty CUInt16Array ctor
	CUInt16Array()
	{}

	/** Creates new CUInt16Array object from another CStringList
	  \param std::list [in] : std::list to be copied */
	CUInt16Array( const CUInt16Array& v ) : uint16array( v )
	{}

	/// Destructor
	virtual ~CUInt16Array()
	{}

	virtual void Insert( CUInt16Array* vect, bool bEnd = true );
	virtual void Insert( const CUInt16Array& vect, bool bEnd = true );
	virtual void Insert( const std::vector<uint16_t>& vect, bool bEnd = true );

	virtual void Insert( uint16_t* vect, size_t length );

	virtual void Insert( const uint16_t value );

	virtual CUInt16Array::iterator InsertAt( CUInt16Array::iterator where, const uint16_t value );
	virtual CUInt16Array::iterator InsertAt( int32_t index, const uint16_t value );

	virtual CUInt16Array::iterator ReplaceAt( CUInt16Array::iterator where, const uint16_t value );
	virtual CUInt16Array::iterator ReplaceAt( int32_t index, const uint16_t value );

	virtual bool Erase( CUInt16Array::iterator it );

	virtual bool Intersect( const CUInt16Array& array, CUInt16Array& intersect ) const;
	virtual bool Complement( const CUInt16Array& array, CUInt16Array& complement ) const;

	virtual std::string ToString( const std::string& delim = ",", bool useBracket = true ) const;

	virtual uint16_t* ToArray();
	virtual int16_t* ToIntArray();

        virtual void RemoveAll() { this->clear(); }

	///Dump fonction
	virtual void Dump( std::ostream& fOut = std::cerr ) const;
};


//-------------------------------------------------------------
//------------------- CInt8Array class --------------------
//-------------------------------------------------------------


/** \addtogroup tools Tools
  @{ */

/**
  An array (std::vector) of ints management class.


  \version 1.0
  */
class CInt8Array : public std::vector<int8_t>
{
	typedef std::vector<int8_t> int8array;

public:
	/// Empty CInt8Array ctor
	CInt8Array()
	{}

	/** Creates new CInt8Array object from another CStringList
	  \param std::list [in] : std::list to be copied */
	CInt8Array( const CInt8Array& vect ) : int8array(vect)
	{}

	/// Destructor
	virtual ~CInt8Array()
	{}

	virtual void Insert( const CInt8Array& vect, bool bEnd = true );

	virtual void Insert( int8_t* vect, size_t length );

	virtual void Insert( const int8_t value );
	virtual CInt8Array::iterator InsertAt( CInt8Array::iterator where, const int8_t value );
	virtual CInt8Array::iterator InsertAt( int32_t index, const int8_t value );

	virtual CInt8Array::iterator ReplaceAt( CInt8Array::iterator where, const int8_t value );
	virtual CInt8Array::iterator ReplaceAt( int32_t index, const int8_t value );

	virtual bool Erase( CInt8Array::iterator it );

	virtual bool Intersect( const CInt8Array& array, CInt8Array& intersect ) const;

	virtual std::string ToString( const std::string& delim = ",", bool useBracket = true ) const;

	virtual int8_t* ToArray();

        virtual void RemoveAll() { this->clear(); }

	///Dump fonction
	virtual void Dump( std::ostream& fOut = std::cerr ) const;
};



//-------------------------------------------------------------
//------------------- CUInt8Array class --------------------
//-------------------------------------------------------------


/** \addtogroup tools Tools
  @{ */

/**
  An array (std::vector) of ints management class.


  \version 1.0
  */
class CUInt8Array : public std::vector<uint8_t>
{
	typedef std::vector<uint8_t> uint8array;

public:
	/// Empty CUInt8Array ctor
	CUInt8Array()
	{}

	/** Creates new CUInt8Array object from another CStringList
	  \param std::list [in] : std::list to be copied */
	CUInt8Array( const CUInt8Array& vect ): uint8array( vect )
	{}

	/// Destructor
	virtual ~CUInt8Array()
	{}

	virtual void Insert( CUInt8Array* vect, bool bEnd = true );
	virtual void Insert( const CUInt8Array& vect, bool bEnd = true );
	virtual void Insert( const std::vector<uint8_t>& vect, bool bEnd = true );

	virtual void Insert( uint8_t* vect, size_t length );

	virtual void Insert( const uint8_t value );

	virtual CUInt8Array::iterator InsertAt( CUInt8Array::iterator where, const uint8_t value );
	virtual CUInt8Array::iterator InsertAt( int32_t index, const uint8_t value );

	virtual CUInt8Array::iterator ReplaceAt( CUInt8Array::iterator where, const uint8_t value );
	virtual CUInt8Array::iterator ReplaceAt( int32_t index, const uint8_t value );

	virtual bool Erase( CUInt8Array::iterator it );

	virtual bool Intersect( const CUInt8Array& array, CUInt8Array& intersect ) const;
	virtual bool Complement( const CUInt8Array& array, CUInt8Array& complement ) const;

	virtual std::string ToString( const std::string& delim = ",", bool useBracket = true ) const;

	virtual uint8_t* ToArray();
	virtual int8_t* ToIntArray();

        virtual void RemoveAll() { this->clear(); }

	///Dump fonction
	virtual void Dump( std::ostream& fOut = std::cerr ) const;
};


//-------------------------------------------------------------
//------------------- CFloatArray class --------------------
//-------------------------------------------------------------

/** \addtogroup tools Tools
  @{ */

/**
  An array (std::vector) of float management class.


  \version 1.0
  */
class CFloatArray : public std::vector<float>
{
	typedef std::vector<float> floatarray;

public:
	/// Empty CFloatArray ctor
	CFloatArray()
	{}

	/** Creates new CFloatArray object from another CFloatArray
	  \param vect [in] : array to be copied */
	CFloatArray( const CFloatArray& v ): floatarray( v )
	{}

	/// Destructor
	virtual ~CFloatArray()
	{}

	/** Inserts an array of float at the end of the array
	  \param data [in] : array to be copied
	  \param size [in] : array size to be copied */
	virtual void Insert( float* data, int32_t size );

	/** Inserts a CFloatArray
	  \param vect [in] : array to be copied
	  \param bEnd [in] : insert values  at the end if true, at the beginning if false */
	virtual void Insert( const CFloatArray& vect, bool bEnd = true );

	/** Inserts a partial CFloatArray
	  \param vect [in] : array to be copied
	  \param first [in] : the position of the first element in the range of elements to be copied.
	  \param last [in] : the position of the first element beyond the range of elements to be copied.
	  \param bEnd [in] : insert values at the end if true, at the beginning if false */
	virtual void Insert( const CFloatArray& vect, int32_t first, int32_t last, bool bEnd = true );

	virtual void Insert( const float value );
	virtual void Insert( const int32_t value );
	virtual void Insert( const uint32_t value );

	/** Remove all elements and clear the std::list*/
	virtual void RemoveAll();

	virtual CFloatArray::iterator InsertAt( CFloatArray::iterator where, const float value );
	virtual CFloatArray::iterator InsertAt( int32_t index, const float value );

	virtual CFloatArray::iterator ReplaceAt( CFloatArray::iterator where, const float value );
	virtual CFloatArray::iterator ReplaceAt( int32_t index, const float value );

	virtual bool Erase( CFloatArray::iterator it );

	virtual bool Intersect( const CFloatArray& array, CFloatArray& intersect ) const;

	virtual std::string ToString( const std::string& delim = ",", bool useBracket = true ) const;

	void GetRange( float& min, float& max );

	float* ToArray();

	///Dump fonction
	virtual void Dump( std::ostream& fOut = std::cerr ) const;




};
//-------------------------------------------------------------
//------------------- CDoubleArray class --------------------
//-------------------------------------------------------------

/** \addtogroup tools Tools
  @{ */

/**
  An array (std::vector) of double management class.


  \version 1.0
  */
class CDoubleArray : public std::vector<double>
{
	typedef std::vector<double> doublearray;

public:
	/// Empty CDoubleArray ctor
	CDoubleArray()
	{}

	/** Creates new CDoubleArray object from another CDoubleArray
	  \param vect [in] : array to be copied */
	CDoubleArray( const CDoubleArray& v ): doublearray( v )
	{}

	/// Destructor
	virtual ~CDoubleArray()
	{}

	/** Inserts an array of double at the end of the array
	  \param data [in] : array to be copied
	  \param size [in] : array size to be copied */
	virtual void Insert( double* data, int32_t size );

	virtual void Insert( int32_t* data, int32_t size );
	virtual void Insert( uint32_t* data, int32_t size );

	/** Inserts a CDoubleArray
	  \param vect [in] : array to be copied
	  \param bEnd [in] : insert values  at the end if true, at the beginning if false */
	virtual void Insert( const CDoubleArray& vect, bool bEnd = true );

	/** Inserts a partial CDoubleArray
	  \param vect [in] : array to be copied
	  \param first [in] : the position of the first element in the range of elements to be copied.
	  \param last [in] : the position of the first element beyond the range of elements to be copied.
	  \param bEnd [in] : insert values at the end if true, at the beginning if false */
	virtual void Insert( const CDoubleArray& vect, int32_t first, int32_t last, bool bEnd = true );

	virtual void Insert( const CUInt8Array& vect, bool bEnd = true );
	virtual void Insert( const CInt8Array& vect, bool bEnd = true );
	virtual void Insert( const CInt16Array& vect, bool bEnd = true );
	virtual void Insert( const CIntArray& vect, bool bEnd = true );

	virtual void Insert( const CFloatArray& vect, bool bEnd = true );

	virtual void Insert( const std::string& vect, const std::string& delim = ",", bool bEnd = true );

	virtual void Insert( const double value );
	virtual void Insert( const int32_t value );
	virtual void Insert( const uint32_t value );
	virtual void Insert( const int16_t value );
	virtual void Insert( const uint16_t value );
	virtual void Insert( const int8_t value );
	virtual void Insert( const uint8_t value );

	/** Remove all elements and clear the std::list*/
	virtual void RemoveAll();

	virtual CDoubleArray::iterator InsertAt( CDoubleArray::iterator where, const double value );
	virtual CDoubleArray::iterator InsertAt( int32_t index, const double value );

	virtual CDoubleArray::iterator ReplaceAt( CDoubleArray::iterator where, const double value );
	virtual CDoubleArray::iterator ReplaceAt( int32_t index, const double value );

	virtual bool Erase( CDoubleArray::iterator it );

	virtual bool Intersect( const CDoubleArray& array, CDoubleArray& intersect ) const;

	virtual int32_t FindIndex( double value ) const;

	virtual std::string ToString( const std::string& delim = ",", bool useBracket = true ) const;

	void GetRange( double& min, double& max );

	double* ToArray();

	///Dump fonction
	virtual void Dump( std::ostream& fOut = std::cerr ) const;

	// Explicitly override data() as it is not a standard C++ stdlib method
	const double *data() const
	{
		return &front();
	}

};



////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//						Algorithm Parameters
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------
//------------------- CBratAlgorithmParam class --------------------
//-------------------------------------------------------------
class CBratAlgorithmParam
{
public:
	typedef enum {
		T_UNDEF = 0,
		T_INT,
		T_LONG,
		T_FLOAT,
		T_DOUBLE,
		T_CHAR,
		T_STRING,
		T_VECTOR_DOUBLE,
	} bratAlgoParamTypeVal;

protected:

	bratAlgoParamTypeVal m_typeVal;

	double m_valDouble;
	float m_valFloat;
	int32_t m_valInt;
	int64_t m_valLong;
	std::string m_valString;
	unsigned char m_valChar;
	CDoubleArray m_vectValDouble;


public:
	CBratAlgorithmParam();

	CBratAlgorithmParam( const CBratAlgorithmParam &o );

	virtual ~CBratAlgorithmParam();

        bratAlgoParamTypeVal GetTypeVal() { return m_typeVal; }
	std::string GetTypeValAsString() const { return TypeValAsString( m_typeVal ); }

	std::string GetValue() const;

        double GetValueAsDouble() const { return m_valDouble; }
        int32_t GetValueAsInt() const { return m_valInt; }
        int64_t GetValueAsLong() const { return m_valLong; }
        float GetValueAsFloat() const { return m_valFloat; }
        std::string GetValueAsString() const { return m_valString; }
        unsigned char GetValueAsChar() const { return m_valChar; }
        const CDoubleArray* GetValueAsVectDouble() const { return &m_vectValDouble; }
        CDoubleArray* GetValueAsVectDouble() { return &m_vectValDouble; }

	void SetValue( double value );
	void SetValue( float value );
	void SetValue( int32_t value );
	void SetValue( int64_t value );
	void SetValue( const std::string& value );
	void SetValue( unsigned char value );
	void SetValue( const CDoubleArray& value );
	void SetValue( double* value, int32_t nbValues );

	void SetValue( double value, CBratAlgorithmParam::bratAlgoParamTypeVal type );
	void SetValue( const std::string& value, CBratAlgorithmParam::bratAlgoParamTypeVal type );

	void SetValueAsDoubleArray( const std::string& value, const std::string& delim = "," );

        bratAlgoParamTypeVal TypeVal( bratAlgoParamTypeVal type ) { return type; }

	static std::string TypeValAsString( bratAlgoParamTypeVal type );

	virtual void Dump( std::ostream& fOut = std::cerr );

	const CBratAlgorithmParam& operator=( const CBratAlgorithmParam &o );
	//virtual bool operator==(const CBratAlgorithmParam& o) { return true; };


protected:
	void Set( const CBratAlgorithmParam &o );
	void Init();
	void Copy( const CDoubleArray& value );
};


typedef std::vector<CBratAlgorithmParam> vectorbratalgorithmparam;

//-------------------------------------------------------------
//------------------- CVectorBratAlgorithmParam class --------------------
//-------------------------------------------------------------


class CVectorBratAlgorithmParam : public vectorbratalgorithmparam
{
public:
	CVectorBratAlgorithmParam();
	virtual ~CVectorBratAlgorithmParam();

	virtual void Insert( const CBratAlgorithmParam& o );
	virtual void RemoveAll();

	void Insert( double value );
	void Insert( float value );
	void Insert( int32_t value );
	void Insert( const std::string& value );
	void Insert( unsigned char value );
	void Insert( const CDoubleArray& value );
	void Insert( double* value, int32_t nbValues );

	void Insert( double value, CBratAlgorithmParam::bratAlgoParamTypeVal type );
	void Insert( const std::string& value, CBratAlgorithmParam::bratAlgoParamTypeVal type );

	std::string ToString( const std::string& delim = ",", bool useBracket = true ) const;

	virtual void Dump( std::ostream& fOut = std::cerr );


protected:

	void Init();

protected:


};


#endif // !defined(ALGORITHM_PARAMS_H)
