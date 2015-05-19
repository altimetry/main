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
#if !defined(_wxList_h_)
#define _wxList_h_

#include "wx/object.h"

#include "brathl.h"
#include "Stl.h"


/*! Creates a type name for wxObject list */ 
typedef list<wxObject*> listwxobject; 

/*! Creates a type name for wxObject array */ 
typedef vector<wxObject*> arraywxobject; 
 
/*! Creates a type name for map wxObject base class */ 
typedef map<string, wxObject*> mapwxobject; 




//-------------------------------------------------------------
//------------------- wxObList class --------------------
//-------------------------------------------------------------

/** 
  A list of wxObject management class.


 \version 1.0
*/


class wxObList : public listwxobject
{

public:
    
  /// Empty wxObList ctor
  wxObList(bool bDelete = true);
    
  /** Creates new wxObList object from another CStringList
    \param list [in] : list to be copied */
  //wxObList(const wxObList& list);

  /// Destructor
  virtual ~wxObList();

  //virtual void Insert(const wxObList& list, bool bEnd = true);

  virtual void Insert(wxObject* ob, bool bEnd = true);

  /** Remove all elements and clear the list*/
  virtual void RemoveAll();

  /** Delete an element referenced by iteratorMnemo
  * \return true if no error, otherwise false */
  virtual bool Erase(wxObList::iterator it);

  virtual bool PopBack();

  /** Copy a new CStringList to the object */
  //const wxObList& operator= (const wxObList& lst);

  bool GetDelete() {return m_bDelete;};
  void SetDelete(bool value) {m_bDelete = value;};

  ///Dump fonction
  virtual void Dump(ostream& fOut = cerr);

  
protected:

  bool m_bDelete;

};



//-------------------------------------------------------------
//------------------- wxObArray class --------------------
//-------------------------------------------------------------

/** 
  An array (vector) of wxObject management class.


 \version 1.0
*/


class wxObArray : public arraywxobject
{

public:
    
  /// Empty wxObArray ctor
  wxObArray(bool bDelete = true);
    
  /** Creates new wxObArray object from another wxObArray
    \param list [in] : list to be copied */
  //wxObArray(const wxObArray& list);

  /// Destructor
  virtual ~wxObArray();

  //virtual void Insert(const wxObArray& vect);

  virtual void Insert(wxObject* ob);

   /** Remove all elements and clear the list*/
   virtual void RemoveAll();

  virtual wxObArray::iterator InsertAt(wxObArray::iterator where, wxObject* ob);

  virtual wxObArray::iterator ReplaceAt(wxObArray::iterator where, wxObject* ob);


  /** Delete an element referenced by it
  * \return true if no error, otherwise false */
  virtual bool Erase(wxObArray::iterator it);

  /** Delete an element referenced by it
  * \return true if no error, otherwise false */
  virtual bool Erase(int32_t index);

  virtual bool PopBack();

  /** Copy a new wxObArray to the object */
  //virtual const wxObArray& operator= (const wxObArray& lst);

  bool GetDelete() {return m_bDelete;};
  void SetDelete(bool value) {m_bDelete = value;};

  ///Dump fonction
  virtual void Dump(ostream& fOut = cerr);

protected:

  bool m_bDelete;


  
};




//-------------------------------------------------------------
//------------------- wxObMap class --------------------
//-------------------------------------------------------------
/** 
  a set of object management classes.

 
 \version 1.0
*/


class wxObMap : public mapwxobject
{
public:
   /// wxObMap ctor
   wxObMap(bool bDelete = true);
  
   /// wxObMap dtor
   virtual ~wxObMap();

/////////////
// Methods
/////////////
public:

   
   /** Inserts a wxObject object
   * \param key : wxObject name (map key)
   * \param value : wxObject value 
   * \param withExcept : true for exception handling, flse otherwise
   * \return wxObject object or NULL if error */
   wxObject* Insert(const string& key, wxObject* ob, bool withExcept = true);

   /** Inserts a wxObMap 
   * \param obMap : wxObMap to insert
   * \param withExcept : true for exception handling, flse otherwise
    */
   //void Insert(const wxObMap& obMap, bool withExcept = true);

   
   /** Tests if an element identify by 'key' already exists
   * \return a wxObject pointer if exists, otherwise NULL */
   wxObject* Exists(const string& key);

   /** Delete an element referenced by it
   * \return true if no error, otherwise false */
   bool Erase(wxObMap::iterator it);
   
   /** Delete an element by its key
   * \return true if no error, otherwise false */
   bool Erase(const string& key);

   /** Remove all elements and clear the map*/
   void RemoveAll();

   /** operator[] redefinition. Searches a wxObject object identifiy by 'key'.
     \param key : wxObject keyword 
     \return a pointer to the wxObject object if found, NULL  if not found */
   wxObject* operator[](const string& key);

   bool GetDelete() {return m_bDelete;};
   void SetDelete(bool value) {m_bDelete = value;};

   /// Dump fonction
   virtual void Dump(ostream& fOut = cerr);


protected:

  bool m_bDelete;

 
};




/** @} */


#endif // !defined(_List_h_)
