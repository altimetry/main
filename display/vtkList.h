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
#if !defined(_vtkList_h_)
#define _vtkList_h_

#include "brathl.h"
#include "Stl.h"

#include "vtkObject.h"


/*! Creates a type name for vtkObject list */ 
typedef list<vtkObject*> listvtkobject; 

/*! Creates a type name for vtkObject array */ 
typedef vector<vtkObject*> arrayvtkobject; 
 
/*! Creates a type name for map vtkObject base class */ 
typedef map<string, vtkObject*> mapvtkobject; 




//-------------------------------------------------------------
//------------------- vtkObList class --------------------
//-------------------------------------------------------------

/** 
  A list of vtkObject management class.


 \version 1.0
*/


class vtkObList : public listvtkobject
{

public:
    
  /// Empty vtkObList ctor
  vtkObList(bool bDelete = true);
    
//  /** Creates new vtkObList object from another CStringList
//    \param list [in] : list to be copied */
//  vtkObList(const vtkObList& list);

  /// Destructor
  virtual ~vtkObList();

  //virtual void Insert(const vtkObList& list, bool bEnd = true);

  virtual void Insert(vtkObject* ob, bool bEnd = true);

  /** Remove all elements and clear the list*/
  virtual void RemoveAll();

  /** Delete an element referenced by iteratorMnemo
  * \return true if no error, otherwise false */
  virtual bool Erase(vtkObList::iterator it);

  virtual bool PopBack();

  /** Copy a new CStringList to the object */
  //const vtkObList& operator= (const vtkObList& lst);

  bool GetDelete() {return m_bDelete;};
  void SetDelete(bool value) {m_bDelete = value;};

  ///Dump fonction
  virtual void Dump(ostream& fOut = cerr);

  
protected:

  bool m_bDelete;

};



//-------------------------------------------------------------
//------------------- vtkObArray class --------------------
//-------------------------------------------------------------

/** 
  An array (vector) of vtkObject management class.


 \version 1.0
*/


class vtkObArray : public arrayvtkobject
{

public:
    
  /// Empty vtkObArray ctor
  vtkObArray(bool bDelete = true);
    
//  /** Creates new vtkObArray object from another vtkObArray
//    \param list [in] : list to be copied */
//  vtkObArray(const vtkObArray& list);

  /// Destructor
  virtual ~vtkObArray();

  //virtual void Insert(const vtkObArray& vect);

  virtual void Insert(vtkObject* ob);

   /** Remove all elements and clear the list*/
   virtual void RemoveAll();

  virtual vtkObArray::iterator InsertAt(vtkObArray::iterator where, vtkObject* ob);

  virtual vtkObArray::iterator ReplaceAt(vtkObArray::iterator where, vtkObject* ob);


  /** Delete an element referenced by it
  * \return true if no error, otherwise false */
  virtual bool Erase(vtkObArray::iterator it);

  /** Delete an element referenced by it
  * \return true if no error, otherwise false */
  virtual bool Erase(int32_t index);

  virtual bool PopBack();

  /** Copy a new vtkObArray to the object */
  //virtual const vtkObArray& operator= (const vtkObArray& lst);

  bool GetDelete() {return m_bDelete;};
  void SetDelete(bool value) {m_bDelete = value;};

  ///Dump fonction
  virtual void Dump(ostream& fOut = cerr);

protected:

  bool m_bDelete;


  
};




//-------------------------------------------------------------
//------------------- vtkObMap class --------------------
//-------------------------------------------------------------
/** 
  a set of object management classes.

 
 \version 1.0
*/


class vtkObMap : public mapvtkobject
{
public:
   /// vtkObMap ctor
   vtkObMap(bool bDelete = true);
  
   /// vtkObMap dtor
   virtual ~vtkObMap();

/////////////
// Methods
/////////////
public:

   
   /** Inserts a vtkObject object
   * \param key : vtkObject name (map key)
   * \param ob : vtkObject value 
   * \param withExcept : true for exception handling, flse otherwise
   * \return vtkObject object or NULL if error */
   vtkObject* Insert(const string& key, vtkObject* ob, bool withExcept = true);

   /** Inserts a vtkObMap 
   * \param obMap : vtkObMap to insert
   * \param withExcept : true for exception handling, flse otherwise
    */
   //void Insert(const vtkObMap& obMap, bool withExcept = true);

   
   /** Tests if an element identify by 'key' already exists
   * \return a vtkObject pointer if exists, otherwise NULL */
   vtkObject* Exists(const string& key);

   /** Delete an element referenced by it
   * \return true if no error, otherwise false */
   bool Erase(vtkObMap::iterator it);
   
   /** Delete an element by its key
   * \return true if no error, otherwise false */
   bool Erase(const string& key);

   /** Remove all elements and clear the map*/
   void RemoveAll();

   /** operator[] redefinition. Searches a vtkObject object identifiy by 'key'.
     \param key : vtkObject keyword 
     \return a pointer to the vtkObject object if found, NULL  if not found */
   vtkObject* operator[](const string& key);

   bool GetDelete() {return m_bDelete;};
   void SetDelete(bool value) {m_bDelete = value;};

  /// Dump fonction
   virtual void Dump(ostream& fOut = cerr);


protected:

  bool m_bDelete;

 
};




/** @} */


#endif // !defined(_List_h_)
