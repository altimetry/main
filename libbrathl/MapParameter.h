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

#if !defined(_MapParameter_h_)
#define _MapParameter_h_

#include "Stl.h"

#include "Parameter.h"
using namespace brathl;


namespace brathl
{

  
/** \file 
  Class definition file
*/
  
  
/*! Creates a type name for map parameter base class */ 
typedef map<string, CParameter*> map_parameter; 

/** \addtogroup parameters Parameters
  @{ */

/** 
  Parameter management class.

  This class provides a map of CParameter objects

 \version 1.0
*/


class CMapParameter : public map_parameter
{
public:
   /// CMapParameter ctor
   CMapParameter();
  
   /// CMapParameter dtor
   virtual ~CMapParameter();

/////////////
// Methods
/////////////
public:

   
   /** Inserts a CParameter object
   * \param key : parameter name (map key)
   * \param value : parameter value 
   * \return CParameter oject or NULL if error */
   CParameter* Insert(const string& key, const string& value);

   
   /** Delete an element referenced by iteratorMnemo
   * \return true if no error, otherwise false */
   bool Erase(CMapParameter::iterator iteratorParameter);

  /** Tests if an element identify by 'key' already exists
   * \return a CParameter pointer if exists, otherwise NULL */
  CParameter* Exists(const string& key);

   /** Delete an element by its key
   * \return true if no error, otherwise false */
   bool Erase(const string& key);

   /** Remove all elements and clear the map*/
   void RemoveAll();

   /** operator[] redefinition. Searches a CParameter object identifiy by 'key'.
      DON'T USE this syntax if you are not sure the key exists, there's a bug in STL,  
       after calling  'record = m_recordSetMap[recordSetName]', if key not existed and the map is empty 
       then the key exists in the map and points to a NULL object
        CParameter *p = m_mapParam[key] --> use Exists method instead ;
     \param key : parameter keyword 
     \return a pointer to th CParameter object if found, NULL  if not found */
   CParameter* operator[](const string key);

   /// Dump fonction
   virtual void Dump(ostream& fOut = cerr);


protected:
   
public:
    
protected:
 
};

}
#endif // !defined(_MapParameter_h_)
