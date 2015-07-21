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

#if !defined(_List_h_)
#define _List_h_

#include "brathl_error.h" 
#include "brathl.h" 

#include "Stl.h"

#include "BratObject.h"
#include "Tools.h"
using namespace brathl;



namespace brathl
{

typedef double* DoublePtr; 

/*! Creates a type name for string list */ 
typedef list<string> stringlist; 

/*! Creates a type name for string array */ 
typedef vector<string> stringarray; 

/*! Creates a type name for int list */ 
typedef list<int32_t> intlist; 

/*! Creates a type name for object list */ 
typedef list<CBratObject*> oblist; 

/*! Creates a type name for int32_t array */ 
typedef vector<int32_t> int32array; 
  
/*! Creates a type name for uint32_t array */ 
typedef vector<uint32_t> uint32array; 

/*! Creates a type name for int16_t array */ 
typedef vector<int16_t> int16array; 
  
/*! Creates a type name for int16_t array */ 
typedef vector<uint16_t> uint16array; 

/*! Creates a type name for int8_t array */ 
typedef vector<int8_t> int8array; 
  
/*! Creates a type name for int16_t array */ 
typedef vector<uint8_t> uint8array; 
    
/*! Creates a type name for float array */ 
typedef vector<float> floatarray; 
  
/*! Creates a type name for double array */ 
typedef vector<double> doublearray; 

/*! Creates a type name for double pointer array */ 
typedef vector<DoublePtr> doubleptrarray; 

/*! Creates a type name for object array */ 
typedef vector<CBratObject*> obarray; 

/*! Creates a type name for map object base class */ 
typedef map<string, string> mapstring; 

/*! Creates a type name for map object base class */ 
typedef map<string, CBratObject*> mapobject; 

/*! Creates a type name for map object base class */ 
typedef map<int32_t, CBratObject*> mapintobject; 

/*! Creates a type name for map object base class */ 
typedef map<double, CBratObject*> mapdoubleobject; 

/*! Creates a type name for map pointer base class */ 
typedef map<string, void*> mapptr; 

/*! Creates a type name for map pointer base class */ 
typedef map<double, DoublePtr*> mapdoubledoubleptr; 

/*! Creates a type name for map int base class */ 
typedef map<string, int32_t> mapint; 

/*! Creates a type name for map unsigned int base class */ 
typedef map<string, uint32_t> mapuint; 

/*! Creates a type name for map pointer base class */ 
typedef map<string, double> mapdouble; 

/*! Creates a type name for object stack */ 
typedef stack<CBratObject*> obstack; 


//-------------------------------------------------------------
//------------------- CStringList class --------------------
//-------------------------------------------------------------

class CStringArray;
  
/** \addtogroup tools Tools
  @{ */

/** 
  A list of strings management class.


 \version 1.0
*/


class CStringList : public stringlist
{

public:
    
  /// Empty CStringList ctor
  CStringList();
    
  /** Creates new CStringList object from another CStringList
    \param list [in] : list to be copied */
  CStringList(const CStringList& list);
  CStringList(const stringlist& list);

  CStringList(const CStringArray& vect);
  CStringList(const stringarray& vect);


  /// Destructor
  virtual ~CStringList();

  virtual int32_t FindIndex(const string& str, bool compareNoCase = false) const;

  virtual bool Exists(const string& str) const;
  virtual bool ExistsNoCase(const string& str) const; 

  virtual void Insert(const CStringList& list, bool bEnd = true);  
  virtual void Insert(const string& str, bool bEnd = true);
  virtual void Insert(const CStringArray& vect, bool bEnd = true);
  virtual void Insert(const stringarray& vect, bool bEnd = true);
  virtual void Insert(const stringlist& lst, bool bEnd = true);

  virtual void InsertUnique(const string& str, bool bEnd = true);
  virtual void InsertUnique(const CStringList& lst, bool bEnd = true);
  virtual void InsertUnique(const CStringArray* vect, bool bEnd = true);
  virtual void InsertUnique(const CStringArray& vect, bool bEnd = true);
  virtual void InsertUnique(const stringarray& vect, bool bEnd = true);
  virtual void InsertUnique(const stringlist& lst, bool bEnd = true);

  virtual void ExtractStrings(const string& str, const char delim, bool bRemoveAll = true);
  virtual void ExtractStrings(const string& str, const string& delim, bool bRemoveAll = true);

  virtual void ExtractKeys(const string& str, const string& delim, bool bRemoveAll = true) ;
  
  virtual void Erase(const string& str);
  virtual void Erase(CStringList::iterator it);

  virtual void RemoveAll();

  virtual bool Intersect(const CStringList& array, CStringList& intersect) const;
  virtual bool Complement(const CStringList& array, CStringList& complement) const;

  /** Copy a new CStringList to the object */
  virtual const CStringList& operator= (const CStringList& lst);
  virtual const CStringList& operator= (const CStringArray& vect);
  virtual const CStringList& operator= (const stringarray& vect);
  virtual const CStringList& operator= (const stringlist& lst);

  virtual string ToString(const string& delim = ",", bool useBracket = true) const;

  ///Dump fonction
  virtual void Dump(ostream& fOut = cerr) const;

  


};


//-------------------------------------------------------------
//------------------- CIntList class --------------------
//-------------------------------------------------------------
  
/** \addtogroup tools Tools
  @{ */

/** 
  A list of strings management class.


 \version 1.0
*/


class CIntList : public intlist
{

public:
    
  /// Empty CIntList ctor
  CIntList();
    
  /** Creates new CIntList object from another CStringList
    \param list [in] : list to be copied */
  CIntList(const CIntList& list);

  /// Destructor
  virtual ~CIntList();

  virtual void RemoveAll() { this->clear(); };

  virtual void Insert(const CIntList& list, bool bEnd = true);

  virtual void Insert(const int value, bool bEnd = true);

  /** Copy a new CIntList to the object */
  const CIntList& operator= (const CIntList& lst);

  ///Dump fonction
  virtual void Dump(ostream& fOut = cerr) const;

  


};
//-------------------------------------------------------------
//------------------- CObList class --------------------
//-------------------------------------------------------------

/** 
  A list of CBratObject management class.


 \version 1.0
*/


class CObList : public oblist
{

public:
    
  /// Empty CObList ctor
  CObList(bool bDelete = true);
    
  /** Creates new CObList object from another CStringList
    \param lst [in] : list to be copied */
  CObList(const CObList& lst);

  /// Destructor
  virtual ~CObList();

  virtual void Insert(const CObList& list, bool bEnd = true);

  virtual void Insert(CBratObject* ob, bool bEnd = true);

  /** Remove all elements and clear the list*/
  virtual void RemoveAll();

  /** Delete an element referenced by ob
  * \return true if no error, otherwise false */
  bool Erase(CBratObject* ob);

  /** Delete an element referenced by it
  * \return true if no error, otherwise false */
  virtual bool Erase(CObList::iterator it);

  virtual bool PopBack();

  /** Copy a new CStringList to the object */
  virtual const CObList& operator= (const CObList& lst);

  bool GetDelete() {return m_bDelete;};
  void SetDelete(bool value) {m_bDelete = value;};

  ///Dump fonction
  virtual void Dump(ostream& fOut = cerr) const;

  
protected:

  bool m_bDelete;

};




//-------------------------------------------------------------
//------------------- CStringArray class --------------------
//-------------------------------------------------------------


  
/** \addtogroup tools Tools
  @{ */

/** 
  An array (vector) of strings management class.


 \version 1.0
*/

class CIntArray;
class CUIntArray;

class CStringArray : public stringarray
{

public:
    
  /// Empty CStringArray ctor
  CStringArray();
    
  /** Creates new CStringArray object from another CStringList
    \param list [in] : list to be copied */
  CStringArray(const CStringArray& vect);
  CStringArray(const stringarray& vect);
  
  CStringArray(const CStringList& lst);
  CStringArray(const stringlist& lst);


  /// Destructor
  virtual ~CStringArray();

  virtual bool Exists(const string& str, bool compareNoCase = false) const;

  virtual int32_t FindIndex(const string& str, bool compareNoCase = false) const;
  virtual void FindIndexes(const string& str, CIntArray& indexes, bool compareNoCase = false) const;

  virtual void GetValues(const CIntArray& indexes, CStringArray& values) const;
  virtual void GetValues(const CIntArray& indexes, string& values) const;

  virtual void Insert(const CStringArray& vect, bool bEnd = true);
  virtual void Insert(const string& str);
  virtual void Insert(const stringarray& vect, bool bEnd = true);
  virtual void Insert(const CIntArray& vect);
  virtual void Insert(const CStringList& lst);
  virtual void Insert(const stringlist& lst);
  
  virtual CStringArray::iterator InsertAt(CStringArray::iterator where, const string& str);
  virtual CStringArray::iterator InsertAt(int32_t index, const string& str);

  virtual CStringArray::iterator ReplaceAt(int32_t index, const string& str);
  virtual CStringArray::iterator ReplaceAt(uint32_t index, const string& str);
  virtual CStringArray::iterator ReplaceAt(CStringArray::iterator where, const string& str);
  virtual bool Erase(CStringArray::iterator it);
  virtual bool Erase(int32_t index);
  virtual bool Erase(uint32_t index);

  virtual bool Remove(const string& array, bool compareNoCase = false);
  virtual bool Remove(const CStringArray& array, bool compareNoCase = false);

  /** Remove all elements and clear the list*/
  virtual void RemoveAll();

  virtual void ExtractStrings(const string& str, const char delim, bool bRemoveAll = true, bool insertUnique = false);
  virtual void ExtractStrings(const string& str, const string& delim, bool bRemoveAll = true, bool insertUnique = false);

  virtual void ExtractKeys(const string& str, const string& delim, bool bRemoveAll = true) ;

  virtual bool Intersect(const string& str, CStringArray& intersect, bool compareNoCase = false) const;
  virtual bool Intersect(const CStringArray& array, CStringArray& intersect, bool compareNoCase = false) const;
  virtual bool Intersect(const string& str, CUIntArray& intersect, bool compareNoCase = false) const;
  virtual bool Intersect(const CStringArray& array, CUIntArray& intersect, bool compareNoCase = false) const;
  virtual bool Complement(const CStringArray& array, CStringArray& complement) const;
  
  virtual void Replace(const CStringArray& findString, const string& replaceBy, CStringArray& replaced, bool compareNoCase = false, bool insertUnique = false) const;
  virtual void Replace(const string& findString, const string& replaceBy, CStringArray& replaced, bool compareNoCase = false, bool insertUnique = false) const;

  virtual void InsertUnique(const string& str);
  virtual void InsertUnique(const CStringArray* vect);
  virtual void InsertUnique(const CStringArray& vect);
  virtual void InsertUnique(const CStringList& lst);
  virtual void InsertUnique(const stringarray& vect);
  virtual void InsertUnique(const stringlist& lst);

  virtual string ToString(const string& delim = ",", bool useBracket = true) const;

  /** Equality operator overload
      Array are equal if they have same size and the same element values (at the same position)*/
  virtual bool operator ==(const CStringArray& vect);

  /** Inequality operator overload
      Array are unequal if they are not equal*/
  virtual bool operator !=(const CStringArray& vect) {return !(*this == vect);};

  /** Copy a new CStringArray to the object */
  virtual const CStringArray& operator= (const CStringArray& vect);
  virtual const CStringArray& operator= (const CStringList& lst);
  virtual const CStringArray& operator= (const stringarray& vect);
  virtual const CStringArray& operator= (const stringlist& lst);

  ///Dump fonction
  virtual void Dump(ostream& fOut = cerr) const;

  


};


//-------------------------------------------------------------
//------------------- CIntArray class --------------------
//-------------------------------------------------------------


/** \addtogroup tools Tools
  @{ */

/** 
  An array (vector) of ints management class.


 \version 1.0
*/


class CIntArray : public int32array
{

public:
    
  /// Empty CIntArray ctor
  CIntArray();
    
  /** Creates new CIntArray object from another CStringList
    \param list [in] : list to be copied */
  CIntArray(const CIntArray& vect);

  /// Destructor
  virtual ~CIntArray();

  virtual void Insert(const CIntArray& vect, bool bEnd = true);
  virtual void Insert(const CStringArray& vect);

  virtual void Insert(int32_t* vect, size_t length);

  virtual void Insert(const int32_t value);
  virtual CIntArray::iterator InsertAt(CIntArray::iterator where, const int32_t value);
  virtual CIntArray::iterator InsertAt(int32_t index, const int32_t value);

  virtual CIntArray::iterator ReplaceAt(CIntArray::iterator where, const int32_t value);
  virtual CIntArray::iterator ReplaceAt(int32_t index, const int32_t value);
  
  virtual bool Erase(CIntArray::iterator it);

  virtual bool Intersect(const CIntArray& array, CIntArray& intersect) const;

  virtual void IncrementValue(uint32_t incr = 1);

  virtual string ToString(const string& delim = ",", bool useBracket = true) const;

  virtual int32_t* ToArray();

  virtual void RemoveAll() { this->clear(); };

  /** Equality operator overload
      Array are equal if they have same size and the same element values (at the same position)*/
  virtual bool operator ==(const CIntArray& vect);

  /** Copy a new CIntArray to the object */
  virtual const CIntArray& operator= (const CIntArray& vect);

  ///Dump fonction
  virtual void Dump(ostream& fOut = cerr) const;

  


};

//-------------------------------------------------------------
//------------------- CUIntArray class --------------------
//-------------------------------------------------------------


/** \addtogroup tools Tools
  @{ */

/** 
  An array (vector) of ints management class.


 \version 1.0
*/


class CUIntArray : public uint32array
{

public:
    
  /// Empty CUIntArray ctor
  CUIntArray();
    
  /** Creates new CUIntArray object from another CStringList
    \param list [in] : list to be copied */
  CUIntArray(const CUIntArray& vect);

  /// Destructor
  virtual ~CUIntArray();

  virtual void Insert(CUIntArray* vect, bool bEnd = true);
  virtual void Insert(const CUIntArray& vect, bool bEnd = true);
  virtual void Insert(const vector<uint32_t>& vect, bool bEnd = true);
  virtual void Insert(uint32_t* vect, size_t length);

  virtual void Insert(const uint32_t value);

  virtual CUIntArray::iterator InsertAt(CUIntArray::iterator where, const uint32_t value);
  virtual CUIntArray::iterator InsertAt(int32_t index, const uint32_t value);
  
  virtual CUIntArray::iterator ReplaceAt(CUIntArray::iterator where, const uint32_t value);
  virtual CUIntArray::iterator ReplaceAt(int32_t index, const uint32_t value);

  virtual bool Erase(CUIntArray::iterator it);

  virtual bool Intersect(const CUIntArray& array, CUIntArray& intersect) const; 
  virtual bool Complement(const CUIntArray& array, CUIntArray& complement) const;

  virtual string ToString(const string& delim = ",", bool useBracket = true) const;
  
  virtual uint32_t* ToArray();
  virtual int32_t* ToIntArray();
  virtual size_t* ToSizeTArray();

  virtual void RemoveAll() { this->clear(); };

  uint32_t GetProductValues() const;

  /** Equality operator overload
      Array are equal if they have same size and the same element values (at the same position)*/
  virtual bool operator ==(const CUIntArray& vect);

  /** Inequality operator overload
      Array are unequal if they are not equal*/
  virtual bool operator !=(const CUIntArray& vect) {return !(*this == vect);};

  /** Copy a new CUIntArray to the object */
  virtual const CUIntArray& operator= (const CUIntArray& vect);

  ///Dump fonction
  virtual void Dump(ostream& fOut = cerr) const;

  


};
//-------------------------------------------------------------
//------------------- CInt16Array class --------------------
//-------------------------------------------------------------


/** \addtogroup tools Tools
  @{ */

/** 
  An array (vector) of ints management class.


 \version 1.0
*/


class CInt16Array : public int16array
{

public:
    
  /// Empty CInt16Array ctor
  CInt16Array();
    
  /** Creates new CInt16Array object from another CStringList
    \param list [in] : list to be copied */
  CInt16Array(const CInt16Array& vect);

  /// Destructor
  virtual ~CInt16Array();

  virtual void Insert(const CInt16Array& vect, bool bEnd = true);
  virtual void Insert(const CStringArray& vect);

  virtual void Insert(int16_t* vect, size_t length);

  virtual void Insert(const int16_t value);
  virtual CInt16Array::iterator InsertAt(CInt16Array::iterator where, const int16_t value);
  virtual CInt16Array::iterator InsertAt(int32_t index, const int16_t value);

  virtual CInt16Array::iterator ReplaceAt(CInt16Array::iterator where, const int16_t value);
  virtual CInt16Array::iterator ReplaceAt(int32_t index, const int16_t value);

  virtual bool Erase(CInt16Array::iterator it);

  virtual bool Intersect(const CInt16Array& array, CInt16Array& intersect) const;

  virtual string ToString(const string& delim = ",", bool useBracket = true) const;

  virtual int16_t* ToArray();

  virtual void RemoveAll() { this->clear(); };

  /** Copy a new CInt16Array to the object */
  virtual const CInt16Array& operator= (const CInt16Array& vect);

  ///Dump fonction
  virtual void Dump(ostream& fOut = cerr) const;

  


};

//-------------------------------------------------------------
//------------------- CUInt16Array class --------------------
//-------------------------------------------------------------


/** \addtogroup tools Tools
  @{ */

/** 
  An array (vector) of ints management class.


 \version 1.0
*/


class CUInt16Array : public uint16array
{

public:
    
  /// Empty CUInt16Array ctor
  CUInt16Array();
    
  /** Creates new CUInt16Array object from another CStringList
    \param list [in] : list to be copied */
  CUInt16Array(const CUInt16Array& vect);

  /// Destructor
  virtual ~CUInt16Array();

  virtual void Insert(CUInt16Array* vect, bool bEnd = true);
  virtual void Insert(const CUInt16Array& vect, bool bEnd = true);
  virtual void Insert(const vector<uint16_t>& vect, bool bEnd = true);

  virtual void Insert(uint16_t* vect, size_t length);

  virtual void Insert(const uint16_t value);

  virtual CUInt16Array::iterator InsertAt(CUInt16Array::iterator where, const uint16_t value);
  virtual CUInt16Array::iterator InsertAt(int32_t index, const uint16_t value);

  virtual CUInt16Array::iterator ReplaceAt(CUInt16Array::iterator where, const uint16_t value);
  virtual CUInt16Array::iterator ReplaceAt(int32_t index, const uint16_t value);

  virtual bool Erase(CUInt16Array::iterator it);

  virtual bool Intersect(const CUInt16Array& array, CUInt16Array& intersect) const; 
  virtual bool Complement(const CUInt16Array& array, CUInt16Array& complement) const;

  virtual string ToString(const string& delim = ",", bool useBracket = true) const;
  
  virtual uint16_t* ToArray();
  virtual int16_t* ToIntArray();

  virtual void RemoveAll() { this->clear(); };

  /** Equality operator overload
      Array are equal if they have same size and the same element values (at the same position)*/
  virtual bool operator ==(const CUInt16Array& vect);

  /** Inequality operator overload
      Array are unequal if they are not equal*/
  virtual bool operator !=(const CUInt16Array& vect) {return !(*this == vect);};

  /** Copy a new CUInt16Array to the object */
  virtual const CUInt16Array& operator= (const CUInt16Array& vect);

  ///Dump fonction
  virtual void Dump(ostream& fOut = cerr) const;

  


};


//-------------------------------------------------------------
//------------------- CInt8Array class --------------------
//-------------------------------------------------------------


/** \addtogroup tools Tools
  @{ */

/** 
  An array (vector) of ints management class.


 \version 1.0
*/


class CInt8Array : public int8array
{

public:
    
  /// Empty CInt8Array ctor
  CInt8Array();
    
  /** Creates new CInt8Array object from another CStringList
    \param list [in] : list to be copied */
  CInt8Array(const CInt8Array& vect);

  /// Destructor
  virtual ~CInt8Array();

  virtual void Insert(const CInt8Array& vect, bool bEnd = true);
  virtual void Insert(const CStringArray& vect);

  virtual void Insert(int8_t* vect, size_t length);

  virtual void Insert(const int8_t value);
  virtual CInt8Array::iterator InsertAt(CInt8Array::iterator where, const int8_t value);
  virtual CInt8Array::iterator InsertAt(int32_t index, const int8_t value);

  virtual CInt8Array::iterator ReplaceAt(CInt8Array::iterator where, const int8_t value);
  virtual CInt8Array::iterator ReplaceAt(int32_t index, const int8_t value);

  virtual bool Erase(CInt8Array::iterator it);

  virtual bool Intersect(const CInt8Array& array, CInt8Array& intersect) const;

  virtual string ToString(const string& delim = ",", bool useBracket = true) const;

  virtual int8_t* ToArray();

  virtual void RemoveAll() { this->clear(); };

  /** Copy a new CInt8Array to the object */
  virtual const CInt8Array& operator= (const CInt8Array& vect);

  ///Dump fonction
  virtual void Dump(ostream& fOut = cerr) const;

  


};

//-------------------------------------------------------------
//------------------- CUInt8Array class --------------------
//-------------------------------------------------------------


/** \addtogroup tools Tools
  @{ */

/** 
  An array (vector) of ints management class.


 \version 1.0
*/


class CUInt8Array : public uint8array
{

public:
    
  /// Empty CUInt8Array ctor
  CUInt8Array();
    
  /** Creates new CUInt8Array object from another CStringList
    \param list [in] : list to be copied */
  CUInt8Array(const CUInt8Array& vect);

  /// Destructor
  virtual ~CUInt8Array();

  virtual void Insert(CUInt8Array* vect, bool bEnd = true);
  virtual void Insert(const CUInt8Array& vect, bool bEnd = true);
  virtual void Insert(const vector<uint8_t>& vect, bool bEnd = true);

  virtual void Insert(uint8_t* vect, size_t length);

  virtual void Insert(const uint8_t value);

  virtual CUInt8Array::iterator InsertAt(CUInt8Array::iterator where, const uint8_t value);
  virtual CUInt8Array::iterator InsertAt(int32_t index, const uint8_t value);

  virtual CUInt8Array::iterator ReplaceAt(CUInt8Array::iterator where, const uint8_t value);
  virtual CUInt8Array::iterator ReplaceAt(int32_t index, const uint8_t value);

  virtual bool Erase(CUInt8Array::iterator it);

  virtual bool Intersect(const CUInt8Array& array, CUInt8Array& intersect) const; 
  virtual bool Complement(const CUInt8Array& array, CUInt8Array& complement) const;

  virtual string ToString(const string& delim = ",", bool useBracket = true) const;
  
  virtual uint8_t* ToArray();
  virtual int8_t* ToIntArray();

  virtual void RemoveAll() { this->clear(); };

  /** Equality operator overload
      Array are equal if they have same size and the same element values (at the same position)*/
  virtual bool operator ==(const CUInt8Array& vect);

  /** Inequality operator overload
      Array are unequal if they are not equal*/
  virtual bool operator !=(const CUInt8Array& vect) {return !(*this == vect);};

  /** Copy a new CUInt8Array to the object */
  virtual const CUInt8Array& operator= (const CUInt8Array& vect);

  ///Dump fonction
  virtual void Dump(ostream& fOut = cerr) const;

  


};

//-------------------------------------------------------------
//------------------- CFloatArray class --------------------
//-------------------------------------------------------------

/** \addtogroup tools Tools
  @{ */

/** 
  An array (vector) of float management class.


 \version 1.0
*/


class CFloatArray : public floatarray
{

public:
    
  /// Empty CFloatArray ctor
  CFloatArray();
    
  /** Creates new CFloatArray object from another CFloatArray
    \param vect [in] : array to be copied */
  CFloatArray(const CFloatArray& vect);

  /// Destructor
  virtual ~CFloatArray();

  /** Inserts an array of float at the end of the array
    \param data [in] : array to be copied 
    \param size [in] : array size to be copied */
  virtual void Insert(float* data, int32_t size);

  /** Inserts a CFloatArray 
    \param vect [in] : array to be copied 
    \param bEnd [in] : insert values  at the end if true, at the beginning if false */
  virtual void Insert(const CFloatArray& vect, bool bEnd = true);

  /** Inserts a partial CFloatArray 
    \param vect [in] : array to be copied 
    \param first [in] : the position of the first element in the range of elements to be copied. 
    \param last [in] : the position of the first element beyond the range of elements to be copied.
    \param bEnd [in] : insert values at the end if true, at the beginning if false */
  virtual void Insert(const CFloatArray& vect, int32_t first, int32_t last, bool bEnd = true);

  virtual void Insert(const float value);
  virtual void Insert(const int32_t value);
  virtual void Insert(const uint32_t value);

  /** Remove all elements and clear the list*/
  virtual void RemoveAll();

  virtual CFloatArray::iterator InsertAt(CFloatArray::iterator where, const float value);
  virtual CFloatArray::iterator InsertAt(int32_t index, const float value);

  virtual CFloatArray::iterator ReplaceAt(CFloatArray::iterator where, const float value);
  virtual CFloatArray::iterator ReplaceAt(int32_t index, const float value);

  virtual bool Erase(CFloatArray::iterator it);

  virtual bool Intersect(const CFloatArray& array, CFloatArray& intersect) const;

  virtual string ToString(const string& delim = ",", bool useBracket = true) const;

  void GetRange(float& min, float& max);

  float* ToArray();

  /** Copy a new CFloatArray to the object */
  virtual const CFloatArray& operator= (const CFloatArray& vect);

  ///Dump fonction
  virtual void Dump(ostream& fOut = cerr) const;

  


};
//-------------------------------------------------------------
//------------------- CDoubleArray class --------------------
//-------------------------------------------------------------

/** \addtogroup tools Tools
  @{ */

/** 
  An array (vector) of double management class.


 \version 1.0
*/


class CDoubleArray : public doublearray
{

public:
    
  /// Empty CDoubleArray ctor
  CDoubleArray();
    
  /** Creates new CDoubleArray object from another CDoubleArray
    \param vect [in] : array to be copied */
  CDoubleArray(const CDoubleArray& vect);

  /// Destructor
  virtual ~CDoubleArray();

  /** Inserts an array of double at the end of the array
    \param data [in] : array to be copied 
    \param size [in] : array size to be copied */
  virtual void Insert(double* data, int32_t size);

  virtual void Insert(int32_t* data, int32_t size);
  virtual void Insert(uint32_t* data, int32_t size);

  /** Inserts a CDoubleArray 
    \param vect [in] : array to be copied 
    \param bEnd [in] : insert values  at the end if true, at the beginning if false */
  virtual void Insert(const CDoubleArray& vect, bool bEnd = true);

  /** Inserts a partial CDoubleArray 
    \param vect [in] : array to be copied 
    \param first [in] : the position of the first element in the range of elements to be copied. 
    \param last [in] : the position of the first element beyond the range of elements to be copied.
    \param bEnd [in] : insert values at the end if true, at the beginning if false */
  virtual void Insert(const CDoubleArray& vect, int32_t first, int32_t last, bool bEnd = true);

  virtual void Insert(const CUInt8Array& vect, bool bEnd = true);
  virtual void Insert(const CInt8Array& vect, bool bEnd = true);
  virtual void Insert(const CInt16Array& vect, bool bEnd = true);
  virtual void Insert(const CIntArray& vect, bool bEnd = true);

  virtual void Insert(const CFloatArray& vect, bool bEnd = true);

  virtual void Insert(const CStringArray& vect, bool bEnd = true);
  virtual void Insert(const string& vect, const string& delim = "," , bool bEnd = true);

  virtual void Insert(const double value);
  virtual void Insert(const int32_t value);
  virtual void Insert(const uint32_t value);
  virtual void Insert(const int16_t value);
  virtual void Insert(const uint16_t value);
  virtual void Insert(const int8_t value);
  virtual void Insert(const uint8_t value);

  /** Remove all elements and clear the list*/
  virtual void RemoveAll();

  virtual CDoubleArray::iterator InsertAt(CDoubleArray::iterator where, const double value);
  virtual CDoubleArray::iterator InsertAt(int32_t index, const double value);

  virtual CDoubleArray::iterator ReplaceAt(CDoubleArray::iterator where, const double value);
  virtual CDoubleArray::iterator ReplaceAt(int32_t index, const double value);

  virtual bool Erase(CDoubleArray::iterator it);

  virtual bool Intersect(const CDoubleArray& array, CDoubleArray& intersect) const;
  
  virtual int32_t FindIndex(double value) const;

  virtual string ToString(const string& delim = ",", bool useBracket = true) const;

  void GetRange(double& min, double& max);

  double* ToArray();

  /** Copy a new CDoubleArray to the object */
  virtual const CDoubleArray& operator= (const CDoubleArray& vect);

  /** Equality operator overload
      Array are equal if they have same size and the same element values (at the same position)*/
  virtual bool operator ==(const CDoubleArray& vect);

  /** Inequality operator overload
      Array are unequal if they are not equal*/
  virtual bool operator !=(const CDoubleArray& vect) {return !(*this == vect);};

  ///Dump fonction
  virtual void Dump(ostream& fOut = cerr) const;

  // Explicitly override data() as it is not a standard C++ stdlib method
  const double *data() const
  {
      return &front();
  }

};

//-------------------------------------------------------------
//------------------- CDoublePtrArray class --------------------
//-------------------------------------------------------------

/** 
  An array (vector) of duble pointer management class.


 \version 1.0
*/

class CDoublePtrArray : public doubleptrarray
{

public:
    
  /// Empty CDoublePtrArray ctor
  CDoublePtrArray(bool bDelete = true);
    
  /// Destructor
  virtual ~CDoublePtrArray();

  virtual void Insert(DoublePtr ob);

  virtual CDoublePtrArray::iterator InsertAt(CDoublePtrArray::iterator where, DoublePtr ob);

  virtual CDoublePtrArray::iterator ReplaceAt(CDoublePtrArray::iterator where, DoublePtr ob);

   /** Remove all elements and clear the list*/
   virtual void RemoveAll();

   /** Delete an element referenced by it
   * \return true if no error, otherwise false */
   virtual bool Erase(CDoublePtrArray::iterator it);

   /** Delete an element referenced by index
   * \return true if no error, otherwise false */
   virtual bool Erase(int32_t index);

   virtual bool PopBack();


  bool GetDelete() {return m_bDelete;};
  void SetDelete(bool value) {m_bDelete = value;};

  CUIntArray* GetMatrixDims() { return &m_matrixDims; };
  void SetMatrixDims(const CUIntArray& matrixDims) { m_matrixDims = matrixDims; };

  uint32_t GetMatrixDim(uint32_t row);

  uint32_t GetMatrixNumberOfDims() { return m_matrixDims.size(); };

  DoublePtr NewMatrix(double initialValue = CTools::m_defaultValueDOUBLE);

  ///Dump fonction
  virtual void Dump(ostream& fOut = cerr) const;

protected:

  void Delete(DoublePtr matrix);

protected:

  bool m_bDelete;

  CUIntArray m_matrixDims;


  
};


//-------------------------------------------------------------
//------------------- CArrayDoublePtrArray class --------------------
//-------------------------------------------------------------

/** \addtogroup tools Tools
  @{ */

/** 
  An array (vector) of vector of double pointer


 \version 1.0
*/

/*! Creates a type name for array of DoublePtr array */ 
typedef vector<doubleptrarray> arraydoubleptrarray; 
  

class CArrayDoublePtrArray : public arraydoubleptrarray
{

public:
    
  /// Empty CDoubleArray ctor
  CArrayDoublePtrArray(bool bDelete = true);
  CArrayDoublePtrArray(const CArrayDoublePtrArray& a);
    
  /// Destructor
  virtual ~CArrayDoublePtrArray();

  void Set(const CArrayDoublePtrArray& m);

  /** Remove all elements and clear the list*/
  virtual void RemoveAll();

  virtual void Remove(doubleptrarray& vect);

  CUIntArray* GetMatrixDims() { return &m_matrixDims; };
  void SetMatrixDims(const CUIntArray& matrixDims) { m_matrixDims = matrixDims; };

  uint32_t GetMatrixDim(uint32_t row);

  uint32_t GetMatrixNumberOfDims() { return m_matrixDims.size(); };

  DoublePtr NewMatrix(double initialValue = CTools::m_defaultValueDOUBLE);
  void InitMatrix(double initialValue = CTools::m_defaultValueDOUBLE);

  void InitMatrixData(double initialValue = CTools::m_defaultValueDOUBLE);

  bool GetDelete() {return m_bDelete;};
  void SetDelete(bool value) {m_bDelete = value;};

  void ResizeRC(uint32_t nrows, uint32_t ncols);

  virtual const CArrayDoublePtrArray& operator= (const CArrayDoublePtrArray& m);

  /*
  double GetMinValue() { return m_minValue; };
  double GetMaxValue() { return m_maxValue; };
  */
  void GetMinMaxValues(double& min, double& max, bool recalc = true);


  ///Dump fonction
  virtual void Dump(ostream& fOut = cerr) const;
protected:

  void Delete(DoublePtr matrix);

  void Init();

  void AdjustValidMinMax(double value);

protected:

  bool m_bDelete;

  CUIntArray m_matrixDims;

  double m_minValue;
  double m_maxValue;
  

};

//-------------------------------------------------------------
//------------------- CArrayDoubleArray class --------------------
//-------------------------------------------------------------

/** \addtogroup tools Tools
  @{ */

/** 
  An array (vector) of vector of double


 \version 1.0
*/

/*! Creates a type name for array of double array */ 
typedef vector<doublearray> arraydoublearray; 
  

class CArrayDoubleArray : public arraydoublearray
{

public:
    
  /// Empty CDoubleArray ctor
  CArrayDoubleArray();
  CArrayDoubleArray(const CArrayDoubleArray& a);
    
  /// Destructor
  virtual ~CArrayDoubleArray();

  void Set(const CArrayDoubleArray& m);

  /** Remove all elements and clear the list*/
  virtual void RemoveAll();

  /*
  CUIntArray* GetMatrixDims() { return &m_matrixDims; };
  void SetMatrixDims(const CUIntArray& matrixDims) { m_matrixDims = matrixDims; };

  uint32_t GetMatrixDim(uint32_t row);

  uint32_t GetMatrixNumberOfDims() { return m_matrixDims.size(); };

  DoublePtr NewMatrix(double initialValue = CTools::m_defaultValueDOUBLE);
  */
  void InitMatrix(double initialValue = CTools::m_defaultValueDOUBLE);

  //void InitMatrixData(double initialValue = CTools::m_defaultValueDOUBLE);

  void ResizeRC(uint32_t nrows, uint32_t ncols);

  virtual const CArrayDoubleArray& operator= (const CArrayDoubleArray& m);

  /*
  double GetMinValue() { return m_minValue; };
  double GetMaxValue() { return m_maxValue; };
  */
  void GetMinMaxValues(double& min, double& max, bool recalc = true);


  ///Dump fonction
  virtual void Dump(ostream& fOut = cerr) const;
protected:

  void Init();

  void AdjustValidMinMax(double value);

protected:

  double m_minValue;
  double m_maxValue;
  

};
//-------------------------------------------------------------
//------------------- CArrayStringMap class --------------------
//-------------------------------------------------------------
/** 
  a set of array string value management classes.

 
 \version 1.0
*/

/*! Creates a type name for map of string array */ 
typedef map<string, CStringArray> maparraystring; 

class CArrayStringMap : public maparraystring
{
public:
   /// CStringMap ctor
   CArrayStringMap();

   CArrayStringMap(const CArrayStringMap& a);

   /// CStringMap dtor
   virtual ~CArrayStringMap();

/////////////
// Methods
/////////////
public:

   
   /** Inserts a string
   * \param key : map key
   * \param str : string value 
   * \return the inserted string value or existing string value if key exists */
   virtual CStringArray* Insert(const string& key, const CStringArray& str, bool withExcept = true);

   /** Inserts a string map 
   * \param strmap : map to insert
   * \param withExcept : true for exception handling, flse otherwise
   * \return the inserted string value or existing string value if key exists */
   virtual void Set(const CArrayStringMap& a);

   /** Tests if an element identify by 'key' already exists
   * \return a string array  value corresponding to the key; if exists, otherwise empty string */
   virtual const CStringArray* Exists(const string& key) const;

   /** Delete an element referenced by it
   * \return true if no error, otherwise false */
   virtual bool Erase(CArrayStringMap::iterator it);
   
   /** Delete an element by its key
   * \return true if no error, otherwise false */
   virtual bool Erase(const string& key);

   virtual const CArrayStringMap& operator= (const CArrayStringMap& a);

   /** Remove all elements and clear the map*/
   virtual void RemoveAll();

   /// Dump fonction
   virtual void Dump(ostream& fOut = cerr) const;

protected:

  void Init();



 
};



//-------------------------------------------------------------
//------------------- CDoubleArrayOb class --------------------
//-------------------------------------------------------------

class CDoubleArrayOb : public CDoubleArray, public CBratObject
{

public:

  CDoubleArrayOb();
    
  CDoubleArrayOb(const CDoubleArrayOb& vect);

  virtual ~CDoubleArrayOb();
  
  virtual CBratObject* Clone();

  virtual const CDoubleArrayOb& operator= (const CDoubleArrayOb& vect);

  virtual void Dump(ostream& fOut = cerr) const;


};
//-------------------------------------------------------------
//------------------- CObStack class --------------------
//-------------------------------------------------------------

/** 
  An stack of CBratObject management class.


 \version 1.0
*/

class CObStack : public obstack
{

public:
    
  /// Empty CObArray ctor
  CObStack(bool bDelete = true);
    
  /// Destructor
  virtual ~CObStack();

  virtual void Push(CBratObject* ob);

  virtual void Pop();

  virtual CBratObject* Top();

   /** Remove all elements and clear the list*/
   virtual void RemoveAll();

  bool GetDelete() {return m_bDelete;};
  void SetDelete(bool value) {m_bDelete = value;};

  ///Dump fonction
  //virtual void Dump(ostream& fOut = cerr) const;

protected:

  bool m_bDelete;


  
};


//-------------------------------------------------------------
//------------------- CObArray class --------------------
//-------------------------------------------------------------

/** 
  An array (vector) of CBratObject management class.


 \version 1.0
*/

class CObArray : public obarray
{

public:
    
  /// Empty CObArray ctor
  CObArray(bool bDelete = true);
    
  /** Creates new CObArray object from another CObArray
    \param vect [in] : list to be copied */
  CObArray(const CObArray& vect);

  /// Destructor
  virtual ~CObArray();

  virtual void Insert(const CObArray& vect);

  virtual void Insert(CBratObject* ob);

  virtual CObArray::iterator InsertAt(CObArray::iterator where, CBratObject* ob);

  virtual CObArray::iterator ReplaceAt(CObArray::iterator where, CBratObject* ob);

   /** Remove all elements and clear the list*/
   virtual void RemoveAll();

   /** Delete an element referenced by ob
   * \return true if no error, otherwise false */
   bool Erase(CBratObject* ob);

   /** Delete an element referenced by it
   * \return true if no error, otherwise false */
   virtual bool Erase(CObArray::iterator it);

   /** Delete an element referenced by index
   * \return true if no error, otherwise false */
   virtual bool Erase(int32_t index);

   virtual bool PopBack();


  /** Copy a new CObArray to the object */
  virtual const CObArray& operator= (const CObArray& lst);

  bool GetDelete() {return m_bDelete;};
  void SetDelete(bool value) {m_bDelete = value;};

  ///Dump fonction
  virtual void Dump(ostream& fOut = cerr) const;

protected:

  bool m_bDelete;


  
};


//-------------------------------------------------------------
//------------------- CObArrayOb class --------------------
//-------------------------------------------------------------

class CObArrayOb : public CObArray, public CBratObject
{

public:

  CObArrayOb(bool bDelete = true);

  CObArrayOb(const CObArrayOb& vect);
  
  virtual ~CObArrayOb();

  virtual CBratObject* Clone();

  virtual const CObArrayOb& operator= (const CObArrayOb& vect);

  virtual void Dump(ostream& fOut = cerr) const;


};


//-------------------------------------------------------------
//------------------- CStringMap class --------------------
//-------------------------------------------------------------
/** 
  a set of string value management classes.

 
 \version 1.0
*/


class CStringMap : public mapstring
{
public:
   /// CStringMap ctor
   CStringMap();
  
   /// CStringMap dtor
   virtual ~CStringMap();

/////////////
// Methods
/////////////
public:

   
   /** Inserts a string
   * \param key : map key
   * \param str : string value 
   * \return the inserted string value or existing string value if key exists */
   virtual string Insert(const string& key, const string& str, bool withExcept = true);

   /** Inserts a string map 
   * \param strmap : map to insert
   * \param withExcept : true for exception handling, flse otherwise
   * \return the inserted string value or existing string value if key exists */
   virtual void Insert(const CStringMap& strmap, bool withExcept = true);

   /** Tests if an element identify by 'key' already exists
   * \return a string value corresponding to the key; if exists, otherwise empty string */
   virtual string Exists(const string& key) const;

   /** Tests if an element value exists
   * \return a string key corresponding to the value (or the first key found, if some values are the same); if exists, otherwise empty string */
   virtual string IsValue(const string& value);

   /** Delete an element referenced by it
   * \return true if no error, otherwise false */
   virtual bool Erase(CStringMap::iterator it);
   
   /** Delete an element by its key
   * \return true if no error, otherwise false */
   virtual bool Erase(const string& key);

   /** Remove all elements and clear the map*/
   virtual void RemoveAll();

      /** Gets keys of the map 
    * \param keys [out] : the keys of the map
    * \param bRemoveAll [in] : if true, remove keys array element before filling the keys
    */
   virtual void GetKeys(CStringArray& keys, bool bRemoveAll = true ) const;

   /// Dump fonction
   virtual void Dump(ostream& fOut = cerr) const;



 
};



//-------------------------------------------------------------
//------------------- CIntMap class --------------------
//-------------------------------------------------------------
/** 
  a set of integer value management classes.

 
 \version 1.0
*/


class CIntMap : public mapint
{
public:
   /// CIntMap ctor
   CIntMap();
  
   /// CIntMap dtor
   virtual ~CIntMap();

/////////////
// Methods
/////////////
public:

   
   /** Inserts an integer
   * \param key : map key
   * \param value : int value 
   * \return the inserted integer value or existing integer value if key exists */
   virtual int32_t Insert(const string& key, int32_t value, bool withExcept = true);

   /** Inserts a CIntMap
   * \param map [in]: map
    * \param bRemoveAll [in] : if true, remove keys array element before filling the keys
   */
   virtual void Insert(const CIntMap& m, bool bRemoveAll = true, bool withExcept = true);

   virtual void Insert(const CStringArray& keys, const CIntArray& values, bool bRemoveAll = true, bool withExcept = true);

   /** Tests if an element identify by 'key' already exists
   * \return a integer value corresponding to the key; if exists, otherwise default value CTools::m_defaultValueINT32 */
   virtual int32_t Exists(const string& key) const;

   /** Delete an element referenced by it
   * \return true if no error, otherwise false */
   virtual bool Erase(CIntMap::iterator it);
   
   /** Delete an element by its key
   * \return true if no error, otherwise false */
   virtual bool Erase(const string& key);

   /** Remove all elements and clear the map*/
   virtual void RemoveAll();

   /** operator[] redefinition. Searches an integer value identifiy by 'key'.
     \param key : string keyword 
     \return the interger value if found, default value CTools::m_defaultValueINT32 if not found */
   virtual int32_t operator[](const string& key);

   /// Dump fonction
   virtual void Dump(ostream& fOut = cerr) const;



 
};

//-------------------------------------------------------------
//------------------- CUIntMap class --------------------
//-------------------------------------------------------------
/** 
  a set of unsigned integer value management classes.

 
 \version 1.0
*/


class CUIntMap : public mapuint
{
public:
   /// CUIntMap ctor
   CUIntMap();
  
   /// CUIntMap dtor
   virtual ~CUIntMap();

/////////////
// Methods
/////////////
public:

   
   /** Inserts an integer
   * \param key : map key
   * \param value : int value 
   * \return the inserted integer value or existing unsigned integer value if key exists */
   virtual uint32_t Insert(const string& key, uint32_t value, bool withExcept = true);

   /** Inserts a CUIntMap
   * \param map [in]: map
    * \param bRemoveAll [in] : if true, remove keys array element before filling the keys
   */
   virtual void Insert(const CUIntMap& m, bool bRemoveAll = true, bool withExcept = true);

   /** Inserts a CStrinArray as keys and initial value
   * \param keys [in]: map keys to insert
   * \param initValue [in]: value of the keys
   * \param bRemoveAll [in] : if true, remove keys array element before filling the keys
   */
   virtual void Insert(const CStringArray& keys, uint32_t initValue, bool bRemoveAll = true, bool withExcept = true);

   /** Inserts a CStrinArray as keys and a CUIntArray as value
   * \param keys [in]: keys to insert
   * \param values [in]: values to insert
   * \param bRemoveAll [in] : if true, remove keys array element before filling the keys
   */
   virtual void Insert(const CStringArray& keys, const CUIntArray& values, bool bRemoveAll = true, bool withExcept = true);

   virtual void Insert(const CStringArray& keys, bool bRemoveAll = true, bool withExcept = true);

   /** Tests if an element identify by 'key' already exists
   * \return a integer value corresponding to the key; if exists, otherwise default value CTools::m_defaultValueUINT32 */
   virtual uint32_t Exists(const string& key) const;

   /** Delete an element referenced by it
   * \return true if no error, otherwise false */
   virtual bool Erase(CUIntMap::iterator it);
   
   /** Delete an element by its key
   * \return true if no error, otherwise false */
   virtual bool Erase(const string& key);

   /** Remove all elements and clear the map*/
   virtual void RemoveAll();

   /** Gets keys of the map 
    * \param keys [out] : the keys of the map
    * \param bRemoveAll [in] : if true, remove keys array element before filling the keys
    */
   virtual void GetKeys(CStringArray& keys, bool bRemoveAll = true );

   /** operator[] redefinition. Searches an integer value identifiy by 'key'.
     \param key : string keyword 
     \return the interger value if found, default value CTools::m_defaultValueUINT32 if not found */
   virtual uint32_t operator[](const string& key);

   /// Dump fonction
   virtual void Dump(ostream& fOut = cerr) const;



 
};
//-------------------------------------------------------------
//------------------- CDoubleMap class --------------------
//-------------------------------------------------------------
/** 
  a set of double value management classes.

 
 \version 1.0
*/


class CDoubleMap : public mapdouble
{
public:
   /// CDoubleMap ctor
   CDoubleMap();
  
   /// CDoubleMap dtor
   virtual ~CDoubleMap();

/////////////
// Methods
/////////////
public:

   
   /** Inserts an double
   * \param key : map key
   * \param value : double value 
   * \return the inserted double value or existing double value if key exists */
   virtual double Insert(const string& key, double value, bool withExcept = true);

   
   /** Tests if an element identify by 'key' already exists
   * \return a double value corresponding to the key; if exists, otherwise default value CTools::m_defaultValueDOUBLE */
   virtual double Exists(const string& key) const;

   /** Delete an element referenced by it
   * \return true if no error, otherwise false */
   virtual bool Erase(CDoubleMap::iterator it);
   
   /** Delete an element by its key
   * \return true if no error, otherwise false */
   virtual bool Erase(const string& key);

   /** Remove all elements and clear the map*/
   virtual void RemoveAll();

   /** operator[] redefinition. Searches an integer value identifiy by 'key'.
     \param key : string keyword 
     \return the double value if found, default value CTools::m_defaultValueDOUBLE if not found */
   virtual double operator[](const string& key);

   /// Dump fonction
   virtual void Dump(ostream& fOut = cerr) const;



 
};

//-------------------------------------------------------------
//------------------- CObMap class --------------------
//-------------------------------------------------------------
/** 
  a set of object management classes.

 
 \version 1.0
*/


class CObMap : public mapobject
{
public:
   /// CObMap ctor
   CObMap(bool bDelete = true);

   CObMap(const CObMap& obMap);
  
   /// CObMap dtor
   virtual ~CObMap();

/////////////
// Methods
/////////////
public:

   
   /** Inserts a CBratObject object
   * \param key : CBratObject name (map key)
   * \param value : CBratObject value 
   * \param withExcept : true for exception handling, flse otherwise
   * \return CBratObject object or NULL if error */
   virtual CBratObject* Insert(const string& key, CBratObject* ob, bool withExcept = true);

   /** Inserts a CObMap 
   * \param obMap : CObMap to insert
   * \param withExcept : true for exception handling, flse otherwise
    */
   virtual void Insert(const CObMap& obMap, bool withExcept = true);

   
   /** Tests if an element identify by 'key' already exists
   * \return a CBratObject pointer if exists, otherwise NULL */
   virtual CBratObject* Exists(const string& key) const;

   /** Delete an element referenced by it
   * \return true if no error, otherwise false */
   virtual bool Erase(CObMap::iterator it);
   
   /** Delete an element by its key
   * \return true if no error, otherwise false */
   virtual bool Erase(const string& key);

   /** Rename a key 
   * \param oldKey : old key 
   * \param newKey : new key
   * \return true if key is renamed, otherwise false*/
   bool RenameKey(const string& oldKey, const string& newKey);

   /** Gets keys of the map 
    * \param keys [out] : the keys of the map
    * \param bRemoveAll [in] : if true, remove keys array element before filling the keys
    */
   virtual void GetKeys(CStringArray& keys, bool bRemoveAll = true, bool bUnique = false);
  
   /** Gets keys of the map 
    * \param keys [out] : the keys of the map
    * \param bRemoveAll [in] : if true, remove keys array element before filling the keys
    */
   virtual void GetKeys(CStringList& keys, bool bRemoveAll = true, bool bUnique = false);

   /** Remove all elements and clear the map*/
   virtual void RemoveAll();

   virtual void ToArray(CObArray& obArray);

   /** operator[] redefinition. Searches a CBratObject object identifiy by 'key'.
       DON'T USE this syntax if you are not sure the key exists, there's a bug in STL,  
       after calling  'record = m_recordSetMap[recordSetName]', if key not existed and the map is empty 
       then the key exists in the map and points to a NULL object
        CBratObject *o = myMap[key] --> use Exists method instead ;
     \param key : CBratObject keyword 
     \return a pointer to the CBratObject object if found, NULL  if not found */
  virtual CBratObject* operator[](const string& key);

  virtual const CObMap& operator =(const CObMap& obMap);


  bool GetDelete() {return m_bDelete;};
  void SetDelete(bool value) {m_bDelete = value;};

   /// Dump fonction
   virtual void Dump(ostream& fOut = cerr) const;


protected:

  bool m_bDelete;

 
};


//-------------------------------------------------------------
//------------------- CObIntMap class --------------------
//-------------------------------------------------------------
/** 
  a set of object management classes.

 
 \version 1.0
*/


class CObIntMap : public mapintobject
{
public:
   /// CObMap ctor
   CObIntMap(bool bDelete = true);
  
   /// CObMap dtor
   virtual ~CObIntMap();

/////////////
// Methods
/////////////
public:

   
   /** Inserts a CBratObject object
   * \param key : CBratObject name (map key)
   * \param value : CBratObject value 
   * \param withExcept : true for exception handling, flse otherwise
   * \return CBratObject object or NULL if error */
   virtual CBratObject* Insert(int32_t key, CBratObject* ob, bool withExcept = true);

   /** Inserts a CObIntMap 
   * \param obMap : CObMap to insert
   * \param withExcept : true for exception handling, flse otherwise
    */
   virtual void Insert(const CObIntMap& obMap, bool withExcept = true);

   
   /** Tests if an element identify by 'key' already exists
   * \return a CBratObject pointer if exists, otherwise NULL */
   virtual CBratObject* Exists(int32_t key) const;

   /** Delete an element referenced by it
   * \return true if no error, otherwise false */
   virtual bool Erase(CObIntMap::iterator it);
   
   /** Delete an element by its key
   * \return true if no error, otherwise false */
   virtual bool Erase(int32_t key);

   /** Rename a key 
   * \param oldKey : old key 
   * \param newKey : new key
   * \return true if key is renamed, otherwise false*/
   bool RenameKey(int32_t oldKey, int32_t newKey);

   /** Gets keys of the map 
    * \param keys [out] : the keys of the map
    * \param bRemoveAll [in] : if true, remove keys array element before filling the keys
    */
   virtual void GetKeys(CIntArray& keys, bool bRemoveAll = true );

   /** Remove all elements and clear the map*/
   virtual void RemoveAll();

   /** operator[] redefinition. Searches a CBratObject object identifiy by 'key'.
       DON'T USE this syntax if you are not sure the key exists, there's a bug in STL,  
       after calling  'record = m_recordSetMap[recordSetName]', if key not existed and the map is empty 
       then the key exists in the map and points to a NULL object
        CBratObject *o = myMap[key] --> use Exists method instead ;
     \param key : CBratObject keyword 
     \return a pointer to the CBratObject object if found, NULL  if not found */
   virtual CBratObject* operator[](int32_t key);

  bool GetDelete() {return m_bDelete;};
  void SetDelete(bool value) {m_bDelete = value;};

  virtual const CObIntMap& operator =(const CObIntMap& obMap);

   /// Dump fonction
   virtual void Dump(ostream& fOut = cerr) const;


protected:

  bool m_bDelete;

 
};


//-------------------------------------------------------------
//------------------- CObDoubleMap class --------------------
//-------------------------------------------------------------
/** 
  a set of object management classes.

 
 \version 1.0
*/


class CObDoubleMap : public mapdoubleobject
{
public:
   /// CObMap ctor
   CObDoubleMap(bool bDelete = true);
  
   /// CObMap dtor
   virtual ~CObDoubleMap();

/////////////
// Methods
/////////////
public:

   
   /** Inserts a CBratObject object
   * \param key : CBratObject name (map key)
   * \param value : CBratObject value 
   * \param withExcept : true for exception handling, flse otherwise
   * \return CBratObject object or NULL if error */
   virtual CBratObject* Insert(double key, CBratObject* ob, bool withExcept = true);

   /** Inserts a CObDoubleMap 
   * \param obMap : CObMap to insert
   * \param withExcept : true for exception handling, flse otherwise
    */
   virtual void Insert(const CObDoubleMap& obMap, bool withExcept = true);

   
   /** Tests if an element identify by 'key' already exists
   * \return a CBratObject pointer if exists, otherwise NULL */
   virtual CBratObject* Exists(double key) const;

   /** Delete an element referenced by it
   * \return true if no error, otherwise false */
   virtual bool Erase(CObDoubleMap::iterator it);
   
   /** Delete an element by its key
   * \return true if no error, otherwise false */
   virtual bool Erase(double key);

   /** Rename a key 
   * \param oldKey : old key 
   * \param newKey : new key
   * \return true if key is renamed, otherwise false*/
   bool RenameKey(double oldKey, double newKey);

   /** Gets keys of the map 
    * \param keys [out] : the keys of the map
    * \param bRemoveAll [in] : if true, remove keys array element before filling the keys
    */
   virtual void GetKeys(CDoubleArray& keys, bool bRemoveAll = true );

   /** Remove all elements and clear the map*/
   virtual void RemoveAll();

   /** operator[] redefinition. Searches a CBratObject object identifiy by 'key'.
       DON'T USE this syntax if you are not sure the key exists, there's a bug in STL,  
       after calling  'record = m_recordSetMap[recordSetName]', if key not existed and the map is empty 
       then the key exists in the map and points to a NULL object
        CBratObject *o = myMap[key] --> use Exists method instead ;
     \param key : CBratObject keyword 
     \return a pointer to the CBratObject object if found, NULL  if not found */
   virtual CBratObject* operator[](double key);

  bool GetDelete() {return m_bDelete;};
  void SetDelete(bool value) {m_bDelete = value;};

  virtual const CObDoubleMap& operator =(const CObDoubleMap& obMap);

   /// Dump fonction
   virtual void Dump(ostream& fOut = cerr) const;


protected:

  bool m_bDelete;

 
};
//-------------------------------------------------------------
//------------------- CObDoubleMap class --------------------
//-------------------------------------------------------------
/** 
  a set of a non rectangular matrix of double management classes.

 
 \version 1.0
*/
class CDoublePtrDoubleMap : public mapdoubledoubleptr
{
public:
   /// CDoublePtrDoubleMap ctor
   CDoublePtrDoubleMap(bool bDelete = true);
   
   CDoublePtrDoubleMap(const CUIntArray& matrixDims, bool bDelete = true);
  
   /// CDoublePtrDoubleMap dtor
   virtual ~CDoublePtrDoubleMap();

/////////////
// Methods
/////////////
public:

   
   /** Inserts a DoublePtr* object
   * \param key : DoublePtr* name (map key)
   * \param value : DoublePtr* value 
   * \param withExcept : true for exception handling, flse otherwise
   * \return DoublePtr* object or NULL if error */
   virtual DoublePtr* Insert(double key, DoublePtr* ob, bool withExcept = true);
   
   virtual DoublePtr* Insert(double key, double initialValue = CTools::m_defaultValueDOUBLE);
   
   /** Tests if an element identify by 'key' already exists
   * \return a CBratObject pointer if exists, otherwise NULL */
   virtual DoublePtr* Exists(double key) const;

   /** Delete an element referenced by it
   * \return true if no error, otherwise false */
   virtual bool Erase(CDoublePtrDoubleMap::iterator it);
   
   /** Delete an element by its key
   * \return true if no error, otherwise false */
   virtual bool Erase(double key);

   /** Rename a key 
   * \param oldKey : old key 
   * \param newKey : new key
   * \return true if key is renamed, otherwise false*/
   bool RenameKey(double oldKey, double newKey);

   /** Gets keys of the map 
    * \param keys [out] : the keys of the map
    * \param bRemoveAll [in] : if true, remove keys array element before filling the keys
    */
   virtual void GetKeys(CDoubleArray& keys, bool bRemoveAll = true );

   /** Remove all elements and clear the map*/
   virtual void RemoveAll();

   /** operator[] redefinition. Searches a CBratObject object identifiy by 'key'.
       DON'T USE this syntax if you are not sure the key exists, there's a bug in STL,  
       after calling  'record = m_recordSetMap[recordSetName]', if key not existed and the map is empty 
       then the key exists in the map and points to a NULL object
        CBratObject *o = myMap[key] --> use Exists method instead ;
     \param key : CBratObject keyword 
     \return a pointer to the CBratObject object if found, NULL  if not found */
   virtual DoublePtr* operator[](double key);

  bool GetDelete() {return m_bDelete;};
  void SetDelete(bool value) {m_bDelete = value;};

  CUIntArray* GetMatrixDims() { return &m_matrixDims; };
  void SetMatrixDims(const CUIntArray& matrixDims) { m_matrixDims = matrixDims; };

  uint32_t GetMatrixColDim(uint32_t row);

  uint32_t GetMatrixNumberOfRows() const { return m_matrixDims.size(); };

  DoublePtr* NewMatrix(double initialValue = CTools::m_defaultValueDOUBLE);

   /// Dump fonction
   virtual void Dump(ostream& fOut = cerr) const;

protected:

  void Delete(DoublePtr* matrix);

protected:

  bool m_bDelete;

  CUIntArray m_matrixDims;
 
};
//-------------------------------------------------------------
//------------------- CPtrMap class --------------------
//-------------------------------------------------------------
/** 
  a set of pointer management classes.

 
 \version 1.0
*/


class CPtrMap : public mapptr
{
public:
   /// CPtrMap ctor
   CPtrMap(bool bDelete = true);
  
   /// CPtrMap dtor
   virtual ~CPtrMap();

/////////////
// Methods
/////////////
public:

   
   /** Inserts a pointer
   * \param key : keymap
   * \param value : pointer value 
   * \param withExcept : true for exception handling, flse otherwise
   * \return pointer or NULL if error */
   virtual void* Insert(const string& key, void* ptr, bool withExcept = true);

   /** Inserts a CPtrMap 
   * \param obMap : CPtrMap to insert
   * \param withExcept : true for exception handling, flse otherwise
    */
   virtual void Insert(const CPtrMap& ptrMap, bool withExcept = true);

   
   /** Tests if an element identify by 'key' already exists
   * \return a pointer if exists, otherwise NULL */
   virtual void* Exists(const string& key) const;

   /** Delete an element referenced by it
   * \return true if no error, otherwise false */
   virtual bool Erase(CPtrMap::iterator it);
   
   /** Delete an element by its key
   * \return true if no error, otherwise false */
   virtual bool Erase(const string& key);

   /** Remove all elements and clear the map*/
   virtual void RemoveAll();

   /** operator[] redefinition. Searches a CBratObject object identifiy by 'key'.
       DON'T USE this syntax if you are not sure the key exists, there's a bug in STL,  
       after calling  'record = m_recordSetMap[recordSetName]', if key not existed and the map is empty 
       then the key exists in the map and points to a NULL object
        void *p = myMap[key] --> use Exists method instead ;
     \param key : CBratObject keyword 
     \return a pointer to the pointer if found, NULL  if not found */
   virtual void* operator[](const string& key);

   /// Dump fonction
   virtual void Dump(ostream& fOut = cerr)  const;


protected:

  bool m_bDelete;

 
};




//-------------------------------------------------------------
//------------------- CMatrix class --------------------
//-------------------------------------------------------------
class CMatrix : public CBratObject
{

protected:

  CMatrix();

  CMatrix(const CMatrix& m);

  ~CMatrix();

  virtual void Set(const CMatrix& m);

  const CMatrix& operator= (const CMatrix& m);

public:

  // Access methods to get the (i,j) element:
  virtual DoublePtr operator() (uint32_t i, uint32_t j) = 0;        
  virtual DoublePtr operator() (uint32_t i, uint32_t j) const = 0;

  
  DoublePtr At(uint32_t i, uint32_t j) { return (*this)(i, j); };        

  virtual void ScaleDownData(double scaleFactor, double addOffset, double defaultValue = CTools::m_defaultValueDOUBLE) = 0;
  virtual void ScaleUpData(double scaleFactor, double addOffset, double defaultValue = CTools::m_defaultValueDOUBLE) = 0;

  // #rows in this matrix
  virtual uint32_t GetNumberOfRows() const = 0;
  // #columns in this matrix
  virtual uint32_t GetNumberOfCols() const = 0;
  
  virtual void Set(uint32_t& row, uint32_t& col, DoublePtr x) = 0;

  virtual void GetMinMaxValues(double& min, double& max) = 0; 

  /*
  CUIntArray* GetMatrixDimsData() { return m_data.GetMatrixDims(); };
  void SetMatrixDimsData(const CUIntArray& matrixDims) { m_data.SetMatrixDims(matrixDims); };
  void SetMatrixDimsData(uint32_t nbValues);
  
  void InitMatrixDimsData(const CUIntArray& matrixDims, double initialValue = CTools::m_defaultValueDOUBLE);
*/
  virtual bool IsMatrixDataSet() { return true; };

  /*
  uint32_t GetMatrixDimData(uint32_t row) { return m_data.GetMatrixDim(row); };

  uint32_t GetMatrixNumberOfDimsData() { return m_data.GetMatrixNumberOfDims(); };
  */

  virtual uint32_t GetMatrixNumberOfValuesData() { return 1; };
  
  virtual uint32_t GetNumberOfValues() = 0;

  /*
  DoublePtr NewMatrixData(double initialValue = CTools::m_defaultValueDOUBLE) { return m_data.NewMatrix(initialValue); };
  */
  virtual void InitMatrix(double initialValue = CTools::m_defaultValueDOUBLE) = 0;

  string GetName() { return m_name; };
  void SetName(const string& value) { m_name = value; };

  string GetXName() { return m_xName; };
  void SetXName(const string& value) { m_xName = value; };

  string GetYName() { return m_yName; };
  void SetYName(const string& value) { m_yName = value; };

  ///Dump fonction
  virtual void Dump(ostream& fOut = cerr) const;

protected : 


private:
 
  string m_name;
  string m_xName;
  string m_yName;

};

//-------------------------------------------------------------
//------------------- CMatrixDoublePtr class --------------------
//-------------------------------------------------------------

class CMatrixDoublePtr : public CMatrix
{

public:

  CMatrixDoublePtr(uint32_t nrows, uint32_t ncols);

  CMatrixDoublePtr(const CMatrixDoublePtr& m);

  virtual ~CMatrixDoublePtr();


  // Access methods to get the (i,j) element:
  virtual DoublePtr operator() (uint32_t i, uint32_t j) ;        
  virtual DoublePtr operator() (uint32_t i, uint32_t j) const;

  virtual doubleptrarray& operator[](const uint32_t& i);
  virtual const doubleptrarray& operator[](const uint32_t& i) const;
  
  const CMatrixDoublePtr& operator= (const CMatrixDoublePtr& m);
  
  const CArrayDoublePtrArray& GetData() { return m_data; };

  virtual void ScaleDownData(double scaleFactor, double addOffset, double defaultValue = CTools::m_defaultValueDOUBLE);
  virtual void ScaleUpData(double scaleFactor, double addOffset, double defaultValue = CTools::m_defaultValueDOUBLE);

  // #rows in this matrix
  virtual uint32_t GetNumberOfRows() const { return m_data.size(); };  
  // #columns in this matrix
  virtual uint32_t GetNumberOfCols() const { return m_data[0].size(); };  
  
  void Set(uint32_t& row, uint32_t& col, DoublePtr x);
  void Set(const CMatrixDoublePtr& m);

  virtual void GetMinMaxValues(double& min, double& max);

  CUIntArray* GetMatrixDimsData() { return m_data.GetMatrixDims(); };
  void SetMatrixDimsData(const CUIntArray& matrixDims) { m_data.SetMatrixDims(matrixDims); };
  void SetMatrixDimsData(uint32_t nbValues);

  void InitMatrixDimsData(const CUIntArray& matrixDims, double initialValue = CTools::m_defaultValueDOUBLE);

  CStringArray* GetMatrixDataDimIndexes() { return &m_matrixDataDimIndexes; };
  void SetMatrixDataDimIndexes(const CStringArray& m) { m_matrixDataDimIndexes.Insert(m); };

  bool IsMatrixDataSet();

  uint32_t GetMatrixDimData(uint32_t row) { return m_data.GetMatrixDim(row); };

  uint32_t GetMatrixNumberOfDimsData() { return m_data.GetMatrixNumberOfDims(); };
  uint32_t GetMatrixNumberOfValuesData() { return GetMatrixDimsData()->GetProductValues(); };

  virtual uint32_t GetNumberOfValues() { return ( GetNumberOfRows() * GetNumberOfCols() * GetMatrixNumberOfValuesData() ); };

  DoublePtr NewMatrixData(double initialValue = CTools::m_defaultValueDOUBLE) { return m_data.NewMatrix(initialValue); };
  void InitMatrix(double initialValue = CTools::m_defaultValueDOUBLE) { m_data.InitMatrix(initialValue); };

  ///Dump fonction
  virtual void Dump(ostream& fOut = cerr) const;

protected : 

  CStringArray m_matrixDataDimIndexes;

  CArrayDoublePtrArray m_data;
};
 
//-------------------------------------------------------------
//------------------- CMatrixDouble class --------------------
//-------------------------------------------------------------

class CMatrixDouble : public CMatrix
{

public:

  CMatrixDouble(uint32_t nrows, uint32_t ncols);

  CMatrixDouble(const CMatrixDouble& m);

  virtual ~CMatrixDouble();


  // Access methods to get the (i,j) element:
  virtual DoublePtr operator() (uint32_t i, uint32_t j) ;        
  virtual DoublePtr operator() (uint32_t i, uint32_t j) const;
  
  virtual doublearray& operator[](const uint32_t& i);
  virtual const doublearray& operator[](const uint32_t& i) const;

  const CMatrixDouble& operator= (const CMatrixDouble& m);

  const CArrayDoubleArray& GetData() { return m_data; };
  
  virtual void ScaleDownData(double scaleFactor, double addOffset, double defaultValue = CTools::m_defaultValueDOUBLE);
  virtual void ScaleUpData(double scaleFactor, double addOffset, double defaultValue = CTools::m_defaultValueDOUBLE);

  // #rows in this matrix
  virtual uint32_t GetNumberOfRows() const { return m_data.size(); };  
  // #columns in this matrix
  virtual uint32_t GetNumberOfCols() const { return m_data[0].size(); };  
  
  void Set(uint32_t& row, uint32_t& col, DoublePtr x);
  void Set(const CMatrixDouble& m);

  virtual void GetMinMaxValues(double& min, double& max);

  /*
  CUIntArray* GetMatrixDimsData() { return m_data.GetMatrixDims(); };
  void SetMatrixDimsData(const CUIntArray& matrixDims) { m_data.SetMatrixDims(matrixDims); };
  void SetMatrixDimsData(uint32_t nbValues);
  
  void InitMatrixDimsData(const CUIntArray& matrixDims, double initialValue = CTools::m_defaultValueDOUBLE);
*/
  /*
  uint32_t GetMatrixDimData(uint32_t row) { return m_data.GetMatrixDim(row); };
  uint32_t GetMatrixNumberOfDimsData() { return m_data.GetMatrixNumberOfDims(); };
  */
  uint32_t GetMatrixNumberOfValuesData() { return 1; };

  virtual uint32_t GetNumberOfValues() { return ( GetNumberOfRows() * GetNumberOfCols() ); };

  /*
  DoublePtr NewMatrixData(double initialValue = CTools::m_defaultValueDOUBLE) { return m_data.NewMatrix(initialValue); };
  */
  void InitMatrix(double initialValue = CTools::m_defaultValueDOUBLE) { m_data.InitMatrix(initialValue); };


  ///Dump fonction
  virtual void Dump(ostream& fOut = cerr) const;

protected : 


private:
 
  CArrayDoubleArray m_data;
};
 




/** @} */

}

#endif // !defined(_List_h_)
