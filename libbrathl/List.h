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

#if !defined(_List_h_)
#define _List_h_

#include <stack>
#include <list>



#include "new-gui/Common/tools/brathl_error.h" 
#include "brathl.h" 

#include <string>

#include "BratObject.h"
#include "Tools.h"



class CStringArray;



namespace brathl
{

typedef double* DoublePtr; 

/*! Creates a type name for std::string std::list */ 
typedef std::list<std::string> stringlist; 

/*! Creates a type name for std::string array */ 
typedef std::vector<std::string> stringarray; 

/*! Creates a type name for int std::list */ 
typedef std::list<int32_t> intlist; 

/*! Creates a type name for object std::list */ 
typedef std::list<CBratObject*> oblist; 

/*! Creates a type name for double pointer array */ 
typedef std::vector<DoublePtr> doubleptrarray; 

/*! Creates a type name for object array */ 
typedef std::vector<CBratObject*> obarray; 

/*! Creates a type name for std::map object base class */ 
typedef std::map<std::string, std::string> mapstring; 

/*! Creates a type name for std::map object base class */ 
typedef std::map<std::string, CBratObject*> mapobject; 

/*! Creates a type name for std::map object base class */ 
typedef std::map<int32_t, CBratObject*> mapintobject; 

/*! Creates a type name for std::map object base class */ 
typedef std::map<double, CBratObject*> mapdoubleobject; 

/*! Creates a type name for std::map pointer base class */ 
typedef std::map<std::string, void*> mapptr; 

/*! Creates a type name for std::map pointer base class */ 
typedef std::map<double, DoublePtr*> mapdoubledoubleptr; 

/*! Creates a type name for std::map int base class */ 
typedef std::map<std::string, int32_t> mapint; 

/*! Creates a type name for std::map unsigned int base class */ 
typedef std::map<std::string, uint32_t> mapuint; 

/*! Creates a type name for std::map pointer base class */ 
typedef std::map<std::string, double> mapdouble; 

/*! Creates a type name for object std::stack */ 
typedef std::stack<CBratObject*> obstack; 


//-------------------------------------------------------------
//------------------- CStringList class --------------------
//-------------------------------------------------------------

  
/** \addtogroup tools Tools
  @{ */

/** 
  A std::list of strings management class.


 \version 1.0
*/


class CStringList : public stringlist
{

public:
    
  /// Empty CStringList ctor
  CStringList();
    
  /** Creates new CStringList object from another CStringList
    \param std::list [in] : std::list to be copied */
  CStringList(const CStringList& list);
  CStringList(const stringlist& list);

  CStringList(const CStringArray& vect);
  CStringList(const stringarray& vect);


  /// Destructor
  virtual ~CStringList();

  virtual int32_t FindIndex(const std::string& str, bool compareNoCase = false) const;

  virtual bool Exists(const std::string& str) const;
  virtual bool ExistsNoCase(const std::string& str) const; 

  virtual void Insert(const CStringList& list, bool bEnd = true);  
  virtual void Insert(const std::string& str, bool bEnd = true);
  virtual void Insert(const CStringArray& vect, bool bEnd = true);
  virtual void Insert(const stringarray& vect, bool bEnd = true);
  virtual void Insert(const stringlist& lst, bool bEnd = true);

  virtual void InsertUnique(const std::string& str, bool bEnd = true);
  virtual void InsertUnique(const CStringList& lst, bool bEnd = true);
  virtual void InsertUnique(const CStringArray* vect, bool bEnd = true);
  virtual void InsertUnique(const CStringArray& vect, bool bEnd = true);
  virtual void InsertUnique(const stringarray& vect, bool bEnd = true);
  virtual void InsertUnique(const stringlist& lst, bool bEnd = true);

  virtual void ExtractStrings(const std::string& str, const char delim, bool bRemoveAll = true);
  virtual void ExtractStrings(const std::string& str, const std::string& delim, bool bRemoveAll = true);

  virtual void ExtractKeys(const std::string& str, const std::string& delim, bool bRemoveAll = true) ;
  
  virtual void Erase(const std::string& str);
  virtual void Erase(CStringList::iterator it);

  virtual void RemoveAll();

  virtual bool Intersect(const CStringList& array, CStringList& intersect) const;
  virtual bool Complement(const CStringList& array, CStringList& complement) const;

  /** Copy a new CStringList to the object */
  virtual const CStringList& operator= (const CStringList& lst);
  virtual const CStringList& operator= (const CStringArray& vect);
  virtual const CStringList& operator= (const stringarray& vect);
  virtual const CStringList& operator= (const stringlist& lst);

  virtual std::string ToString(const std::string& delim = ",", bool useBracket = true) const;

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr) const;

  


};


//-------------------------------------------------------------
//------------------- CIntList class --------------------
//-------------------------------------------------------------
  
/** \addtogroup tools Tools
  @{ */

/** 
  A std::list of strings management class.


 \version 1.0
*/


class CIntList : public intlist
{

public:
    
  /// Empty CIntList ctor
  CIntList();
    
  /** Creates new CIntList object from another CStringList
    \param std::list [in] : std::list to be copied */
  CIntList(const CIntList& list);

  /// Destructor
  virtual ~CIntList();

  virtual void RemoveAll() { this->clear(); };

  virtual void Insert(const CIntList& list, bool bEnd = true);

  virtual void Insert(const int value, bool bEnd = true);

  /** Copy a new CIntList to the object */
  const CIntList& operator= (const CIntList& lst);

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr) const;

  


};
//-------------------------------------------------------------
//------------------- CObList class --------------------
//-------------------------------------------------------------

/** 
  A std::list of CBratObject management class.


 \version 1.0
*/


class CObList : public oblist
{

public:
    
  /// Empty CObList ctor
  CObList(bool bDelete = true);
    
  /** Creates new CObList object from another CStringList
    \param lst [in] : std::list to be copied */
  CObList(const CObList& lst);

  /// Destructor
  virtual ~CObList();

  virtual void Insert(const CObList& list, bool bEnd = true);

  virtual void Insert(CBratObject* ob, bool bEnd = true);

  /** Remove all elements and clear the std::list*/
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
  virtual void Dump(std::ostream& fOut = std::cerr) const;

  
protected:

  bool m_bDelete;

};




//-------------------------------------------------------------
//------------------- CDoublePtrArray class --------------------
//-------------------------------------------------------------

/** 
  An array (std::vector) of duble pointer management class.


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

   /** Remove all elements and clear the std::list*/
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

  size_t GetMatrixNumberOfDims() { return m_matrixDims.size(); };

  DoublePtr NewMatrix(double initialValue = CTools::m_defaultValueDOUBLE);

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr) const;

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
  An array (std::vector) of std::vector of double pointer


 \version 1.0
*/

/*! Creates a type name for array of DoublePtr array */ 
typedef std::vector<doubleptrarray> arraydoubleptrarray; 
  

class CArrayDoublePtrArray : public arraydoubleptrarray
{

public:
    
  /// Empty CDoubleArray ctor
  CArrayDoublePtrArray(bool bDelete = true);
  CArrayDoublePtrArray(const CArrayDoublePtrArray& a);
    
  /// Destructor
  virtual ~CArrayDoublePtrArray();

  void Set(const CArrayDoublePtrArray& m);

  /** Remove all elements and clear the std::list*/
  virtual void RemoveAll();

  virtual void Remove(doubleptrarray& vect);

  CUIntArray* GetMatrixDims() { return &m_matrixDims; };
  void SetMatrixDims(const CUIntArray& matrixDims) { m_matrixDims = matrixDims; };

  uint32_t GetMatrixDim(uint32_t row);

  size_t GetMatrixNumberOfDims() { return m_matrixDims.size(); };

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
  virtual void Dump(std::ostream& fOut = std::cerr) const;
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
  An array (std::vector) of std::vector of double


 \version 1.0
*/

/*! Creates a type name for array of double array */ 
typedef std::vector<double> doublearray; 
typedef std::vector<doublearray> arraydoublearray; 
  

class CArrayDoubleArray : public arraydoublearray
{

public:
    
  /// Empty CDoubleArray ctor
  CArrayDoubleArray();
  CArrayDoubleArray(const CArrayDoubleArray& a);
    
  /// Destructor
  virtual ~CArrayDoubleArray();

  void Set(const CArrayDoubleArray& m);

  /** Remove all elements and clear the std::list*/
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
  virtual void Dump(std::ostream& fOut = std::cerr) const;
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
  a set of array std::string value management classes.

 
 \version 1.0
*/

/*! Creates a type name for std::map of std::string array */ 
typedef std::map<std::string, CStringArray> maparraystring; 

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

   
   /** Inserts a std::string
   * \param key : std::map key
   * \param str : std::string value 
   * \return the inserted std::string value or existing std::string value if key exists */
   virtual CStringArray* Insert(const std::string& key, const CStringArray& str, bool withExcept = true);

   /** Inserts a std::string std::map 
   * \param strmap : std::map to insert
   * \param withExcept : true for exception handling, flse otherwise
   * \return the inserted std::string value or existing std::string value if key exists */
   virtual void Set(const CArrayStringMap& a);

   /** Tests if an element identify by 'key' already exists
   * \return a std::string array  value corresponding to the key; if exists, otherwise empty std::string */
   virtual const CStringArray* Exists(const std::string& key) const;

   /** Delete an element referenced by it
   * \return true if no error, otherwise false */
   virtual bool Erase(CArrayStringMap::iterator it);
   
   /** Delete an element by its key
   * \return true if no error, otherwise false */
   virtual bool Erase(const std::string& key);

   virtual const CArrayStringMap& operator= (const CArrayStringMap& a);

   /** Remove all elements and clear the std::map*/
   virtual void RemoveAll();

   /// Dump fonction
   virtual void Dump(std::ostream& fOut = std::cerr) const;

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

  virtual void Dump(std::ostream& fOut = std::cerr) const;


};
//-------------------------------------------------------------
//------------------- CObStack class --------------------
//-------------------------------------------------------------

/** 
  An std::stack of CBratObject management class.


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

   /** Remove all elements and clear the std::list*/
   virtual void RemoveAll();

  bool GetDelete() {return m_bDelete;};
  void SetDelete(bool value) {m_bDelete = value;};

  ///Dump fonction
  //virtual void Dump(std::ostream& fOut = std::cerr) const;

protected:

  bool m_bDelete;


  
};


//-------------------------------------------------------------
//------------------- CObArray class --------------------
//-------------------------------------------------------------

/** 
  An array (std::vector) of CBratObject management class.


 \version 1.0
*/

class CObArray : public obarray
{

public:
    
  /// Empty CObArray ctor
  CObArray(bool bDelete = true);
    
  /** Creates new CObArray object from another CObArray
    \param vect [in] : std::list to be copied */
  CObArray(const CObArray& vect);

  /// Destructor
  virtual ~CObArray();

  virtual void Insert(const CObArray& vect);

  virtual void Insert(CBratObject* ob);

  virtual CObArray::iterator InsertAt(CObArray::iterator where, CBratObject* ob);

  virtual CObArray::iterator ReplaceAt(CObArray::iterator where, CBratObject* ob);

   /** Remove all elements and clear the std::list*/
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
  virtual void Dump(std::ostream& fOut = std::cerr) const;

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

  virtual void Dump(std::ostream& fOut = std::cerr) const;


};


//-------------------------------------------------------------
//------------------- CStringMap class --------------------
//-------------------------------------------------------------
/** 
  a set of std::string value management classes.

 
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

   
   /** Inserts a std::string
   * \param key : std::map key
   * \param str : std::string value 
   * \return the inserted std::string value or existing std::string value if key exists */
   virtual std::string Insert(const std::string& key, const std::string& str, bool withExcept = true);

   /** Inserts a std::string std::map 
   * \param strmap : std::map to insert
   * \param withExcept : true for exception handling, flse otherwise
   * \return the inserted std::string value or existing std::string value if key exists */
   virtual void Insert(const CStringMap& strmap, bool withExcept = true);

   /** Tests if an element identify by 'key' already exists
   * \return a std::string value corresponding to the key; if exists, otherwise empty std::string */
   virtual std::string Exists(const std::string& key) const;

   /** Tests if an element value exists
   * \return a std::string key corresponding to the value (or the first key found, if some values are the same); if exists, otherwise empty std::string */
   virtual std::string IsValue(const std::string& value);

   /** Delete an element referenced by it
   * \return true if no error, otherwise false */
   virtual bool Erase(CStringMap::iterator it);
   
   /** Delete an element by its key
   * \return true if no error, otherwise false */
   virtual bool Erase(const std::string& key);

   /** Remove all elements and clear the std::map*/
   virtual void RemoveAll();

      /** Gets keys of the std::map 
    * \param keys [out] : the keys of the std::map
    * \param bRemoveAll [in] : if true, remove keys array element before filling the keys
    */
   virtual void GetKeys(CStringArray& keys, bool bRemoveAll = true ) const;

   /// Dump fonction
   virtual void Dump(std::ostream& fOut = std::cerr) const;



 
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
   * \param key : std::map key
   * \param value : int value 
   * \return the inserted integer value or existing integer value if key exists */
   virtual int32_t Insert(const std::string& key, int32_t value, bool withExcept = true);

   /** Inserts a CIntMap
   * \param std::map [in]: std::map
    * \param bRemoveAll [in] : if true, remove keys array element before filling the keys
   */
   virtual void Insert(const CIntMap& m, bool bRemoveAll = true, bool withExcept = true);

   virtual void Insert(const CStringArray& keys, const CIntArray& values, bool bRemoveAll = true, bool withExcept = true);

   /** Tests if an element identify by 'key' already exists
   * \return a integer value corresponding to the key; if exists, otherwise default value CTools::m_defaultValueINT32 */
   virtual int32_t Exists(const std::string& key) const;

   /** Delete an element referenced by it
   * \return true if no error, otherwise false */
   virtual bool Erase(CIntMap::iterator it);
   
   /** Delete an element by its key
   * \return true if no error, otherwise false */
   virtual bool Erase(const std::string& key);

   /** Remove all elements and clear the std::map*/
   virtual void RemoveAll();

   /** operator[] redefinition. Searches an integer value identifiy by 'key'.
     \param key : std::string keyword 
     \return the interger value if found, default value CTools::m_defaultValueINT32 if not found */
   virtual int32_t operator[](const std::string& key);

   /// Dump fonction
   virtual void Dump(std::ostream& fOut = std::cerr) const;



 
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
   * \param key : std::map key
   * \param value : int value 
   * \return the inserted integer value or existing unsigned integer value if key exists */
   virtual uint32_t Insert(const std::string& key, uint32_t value, bool withExcept = true);

   /** Inserts a CUIntMap
   * \param std::map [in]: std::map
    * \param bRemoveAll [in] : if true, remove keys array element before filling the keys
   */
   virtual void Insert(const CUIntMap& m, bool bRemoveAll = true, bool withExcept = true);

   /** Inserts a CStrinArray as keys and initial value
   * \param keys [in]: std::map keys to insert
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
   virtual uint32_t Exists(const std::string& key) const;

   /** Delete an element referenced by it
   * \return true if no error, otherwise false */
   virtual bool Erase(CUIntMap::iterator it);
   
   /** Delete an element by its key
   * \return true if no error, otherwise false */
   virtual bool Erase(const std::string& key);

   /** Remove all elements and clear the std::map*/
   virtual void RemoveAll();

   /** Gets keys of the std::map 
    * \param keys [out] : the keys of the std::map
    * \param bRemoveAll [in] : if true, remove keys array element before filling the keys
    */
   virtual void GetKeys(CStringArray& keys, bool bRemoveAll = true );

   /** operator[] redefinition. Searches an integer value identifiy by 'key'.
     \param key : std::string keyword 
     \return the interger value if found, default value CTools::m_defaultValueUINT32 if not found */
   virtual uint32_t operator[](const std::string& key);

   /// Dump fonction
   virtual void Dump(std::ostream& fOut = std::cerr) const;



 
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
   * \param key : std::map key
   * \param value : double value 
   * \return the inserted double value or existing double value if key exists */
   virtual double Insert(const std::string& key, double value, bool withExcept = true);

   
   /** Tests if an element identify by 'key' already exists
   * \return a double value corresponding to the key; if exists, otherwise default value CTools::m_defaultValueDOUBLE */
   virtual double Exists(const std::string& key) const;

   /** Delete an element referenced by it
   * \return true if no error, otherwise false */
   virtual bool Erase(CDoubleMap::iterator it);
   
   /** Delete an element by its key
   * \return true if no error, otherwise false */
   virtual bool Erase(const std::string& key);

   /** Remove all elements and clear the std::map*/
   virtual void RemoveAll();

   /** operator[] redefinition. Searches an integer value identifiy by 'key'.
     \param key : std::string keyword 
     \return the double value if found, default value CTools::m_defaultValueDOUBLE if not found */
   virtual double operator[](const std::string& key);

   /// Dump fonction
   virtual void Dump(std::ostream& fOut = std::cerr) const;



 
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
   * \param key : CBratObject name (std::map key)
   * \param value : CBratObject value 
   * \param withExcept : true for exception handling, flse otherwise
   * \return CBratObject object or NULL if error */
   virtual CBratObject* Insert(const std::string& key, CBratObject* ob, bool withExcept = true);

   /** Inserts a CObMap 
   * \param obMap : CObMap to insert
   * \param withExcept : true for exception handling, flse otherwise
    */
   virtual void Insert(const CObMap& obMap, bool withExcept = true);

   
   /** Tests if an element identify by 'key' already exists
   * \return a CBratObject pointer if exists, otherwise NULL */
   virtual CBratObject* Exists(const std::string& key) const;

   /** Delete an element referenced by it
   * \return true if no error, otherwise false */
   virtual bool Erase(CObMap::iterator it);
   
   /** Delete an element by its key
   * \return true if no error, otherwise false */
   virtual bool Erase(const std::string& key);

   /** Rename a key 
   * \param oldKey : old key 
   * \param newKey : new key
   * \return true if key is renamed, otherwise false*/
   bool RenameKey(const std::string& oldKey, const std::string& newKey);

   /** Gets keys of the std::map 
    * \param keys [out] : the keys of the std::map
    * \param bRemoveAll [in] : if true, remove keys array element before filling the keys
    */
   virtual void GetKeys(CStringArray& keys, bool bRemoveAll = true, bool bUnique = false);
  
   /** Gets keys of the std::map 
    * \param keys [out] : the keys of the std::map
    * \param bRemoveAll [in] : if true, remove keys array element before filling the keys
    */
   virtual void GetKeys(CStringList& keys, bool bRemoveAll = true, bool bUnique = false);

   /** Remove all elements and clear the std::map*/
   virtual void RemoveAll();

   virtual void ToArray(CObArray& obArray);

   /** operator[] redefinition. Searches a CBratObject object identifiy by 'key'.
       DON'T USE this syntax if you are not sure the key exists, there's a bug in STL,  
       after calling  'record = m_recordSetMap[recordSetName]', if key not existed and the std::map is empty 
       then the key exists in the std::map and points to a NULL object
        CBratObject *o = myMap[key] --> use Exists method instead ;
     \param key : CBratObject keyword 
     \return a pointer to the CBratObject object if found, NULL  if not found */
  virtual CBratObject* operator[](const std::string& key);

  virtual const CObMap& operator =(const CObMap& obMap);


  bool GetDelete() {return m_bDelete;};
  void SetDelete(bool value) {m_bDelete = value;};

   /// Dump fonction
   virtual void Dump(std::ostream& fOut = std::cerr) const;


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
   * \param key : CBratObject name (std::map key)
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

   /** Gets keys of the std::map 
    * \param keys [out] : the keys of the std::map
    * \param bRemoveAll [in] : if true, remove keys array element before filling the keys
    */
   virtual void GetKeys(CIntArray& keys, bool bRemoveAll = true );

   /** Remove all elements and clear the std::map*/
   virtual void RemoveAll();

   /** operator[] redefinition. Searches a CBratObject object identifiy by 'key'.
       DON'T USE this syntax if you are not sure the key exists, there's a bug in STL,  
       after calling  'record = m_recordSetMap[recordSetName]', if key not existed and the std::map is empty 
       then the key exists in the std::map and points to a NULL object
        CBratObject *o = myMap[key] --> use Exists method instead ;
     \param key : CBratObject keyword 
     \return a pointer to the CBratObject object if found, NULL  if not found */
   virtual CBratObject* operator[](int32_t key);

  bool GetDelete() {return m_bDelete;};
  void SetDelete(bool value) {m_bDelete = value;};

  virtual const CObIntMap& operator =(const CObIntMap& obMap);

   /// Dump fonction
   virtual void Dump(std::ostream& fOut = std::cerr) const;


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
   * \param key : CBratObject name (std::map key)
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

   /** Gets keys of the std::map 
    * \param keys [out] : the keys of the std::map
    * \param bRemoveAll [in] : if true, remove keys array element before filling the keys
    */
   virtual void GetKeys(CDoubleArray& keys, bool bRemoveAll = true );

   /** Remove all elements and clear the std::map*/
   virtual void RemoveAll();

   /** operator[] redefinition. Searches a CBratObject object identifiy by 'key'.
       DON'T USE this syntax if you are not sure the key exists, there's a bug in STL,  
       after calling  'record = m_recordSetMap[recordSetName]', if key not existed and the std::map is empty 
       then the key exists in the std::map and points to a NULL object
        CBratObject *o = myMap[key] --> use Exists method instead ;
     \param key : CBratObject keyword 
     \return a pointer to the CBratObject object if found, NULL  if not found */
   virtual CBratObject* operator[](double key);

  bool GetDelete() {return m_bDelete;};
  void SetDelete(bool value) {m_bDelete = value;};

  virtual const CObDoubleMap& operator =(const CObDoubleMap& obMap);

   /// Dump fonction
   virtual void Dump(std::ostream& fOut = std::cerr) const;


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
   * \param key : DoublePtr* name (std::map key)
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

   /** Gets keys of the std::map 
    * \param keys [out] : the keys of the std::map
    * \param bRemoveAll [in] : if true, remove keys array element before filling the keys
    */
   virtual void GetKeys(CDoubleArray& keys, bool bRemoveAll = true );

   /** Remove all elements and clear the std::map*/
   virtual void RemoveAll();

   /** operator[] redefinition. Searches a CBratObject object identifiy by 'key'.
       DON'T USE this syntax if you are not sure the key exists, there's a bug in STL,  
       after calling  'record = m_recordSetMap[recordSetName]', if key not existed and the std::map is empty 
       then the key exists in the std::map and points to a NULL object
        CBratObject *o = myMap[key] --> use Exists method instead ;
     \param key : CBratObject keyword 
     \return a pointer to the CBratObject object if found, NULL  if not found */
   virtual DoublePtr* operator[](double key);

  bool GetDelete() {return m_bDelete;};
  void SetDelete(bool value) {m_bDelete = value;};

  CUIntArray* GetMatrixDims() { return &m_matrixDims; };
  void SetMatrixDims(const CUIntArray& matrixDims) { m_matrixDims = matrixDims; };

  uint32_t GetMatrixColDim(uint32_t row);

  size_t GetMatrixNumberOfRows() const { return m_matrixDims.size(); };

  DoublePtr* NewMatrix(double initialValue = CTools::m_defaultValueDOUBLE);

   /// Dump fonction
   virtual void Dump(std::ostream& fOut = std::cerr) const;

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
   virtual void* Insert(const std::string& key, void* ptr, bool withExcept = true);

   /** Inserts a CPtrMap 
   * \param obMap : CPtrMap to insert
   * \param withExcept : true for exception handling, flse otherwise
    */
   virtual void Insert(const CPtrMap& ptrMap, bool withExcept = true);

   
   /** Tests if an element identify by 'key' already exists
   * \return a pointer if exists, otherwise NULL */
   virtual void* Exists(const std::string& key) const;

   /** Delete an element referenced by it
   * \return true if no error, otherwise false */
   virtual bool Erase(CPtrMap::iterator it);
   
   /** Delete an element by its key
   * \return true if no error, otherwise false */
   virtual bool Erase(const std::string& key);

   /** Remove all elements and clear the std::map*/
   virtual void RemoveAll();

   /** operator[] redefinition. Searches a CBratObject object identifiy by 'key'.
       DON'T USE this syntax if you are not sure the key exists, there's a bug in STL,  
       after calling  'record = m_recordSetMap[recordSetName]', if key not existed and the std::map is empty 
       then the key exists in the std::map and points to a NULL object
        void *p = myMap[key] --> use Exists method instead ;
     \param key : CBratObject keyword 
     \return a pointer to the pointer if found, NULL  if not found */
   virtual void* operator[](const std::string& key);

   /// Dump fonction
   virtual void Dump(std::ostream& fOut = std::cerr)  const;


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
  virtual size_t GetNumberOfRows() const = 0;
  // #columns in this matrix
  virtual size_t GetNumberOfCols() const = 0;
  
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
  
  virtual size_t GetNumberOfValues() = 0;

  /*
  DoublePtr NewMatrixData(double initialValue = CTools::m_defaultValueDOUBLE) { return m_data.NewMatrix(initialValue); };
  */
  virtual void InitMatrix(double initialValue = CTools::m_defaultValueDOUBLE) = 0;

  std::string GetName() { return m_name; };
  void SetName(const std::string& value) { m_name = value; };

  std::string GetXName() { return m_xName; };
  void SetXName(const std::string& value) { m_xName = value; };

  std::string GetYName() { return m_yName; };
  void SetYName(const std::string& value) { m_yName = value; };

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr) const;

protected : 


private:
 
  std::string m_name;
  std::string m_xName;
  std::string m_yName;

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
  virtual size_t GetNumberOfRows() const { return m_data.size(); };  
  // #columns in this matrix
  virtual size_t GetNumberOfCols() const { return m_data[0].size(); };  
  
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

  size_t GetMatrixNumberOfDimsData() { return m_data.GetMatrixNumberOfDims(); };
  uint32_t GetMatrixNumberOfValuesData() { return GetMatrixDimsData()->GetProductValues(); };

  virtual size_t GetNumberOfValues() { return ( GetNumberOfRows() * GetNumberOfCols() * GetMatrixNumberOfValuesData() ); };

  DoublePtr NewMatrixData(double initialValue = CTools::m_defaultValueDOUBLE) { return m_data.NewMatrix(initialValue); };
  void InitMatrix(double initialValue = CTools::m_defaultValueDOUBLE) { m_data.InitMatrix(initialValue); };

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr) const;

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
  virtual size_t GetNumberOfRows() const { return m_data.size(); };  
  // #columns in this matrix
  virtual size_t GetNumberOfCols() const { return m_data[0].size(); };  
  
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

  virtual size_t GetNumberOfValues() { return ( GetNumberOfRows() * GetNumberOfCols() ); };

  /*
  DoublePtr NewMatrixData(double initialValue = CTools::m_defaultValueDOUBLE) { return m_data.NewMatrix(initialValue); };
  */
  void InitMatrix(double initialValue = CTools::m_defaultValueDOUBLE) { m_data.InitMatrix(initialValue); };


  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr) const;

protected : 


private:
 
  CArrayDoubleArray m_data;
};
 




/** @} */

}

#endif // !defined(_List_h_)
