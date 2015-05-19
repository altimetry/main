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
#ifndef __MapProjection_H__
#define __MapProjection_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "MapProjection.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/arrstr.h"

#include "brathl.h"

#include "List.h"
using namespace brathl;

class CSmartCleanerMapProj;

class CMapProjection: public CUIntMap
{
public:
  /// CIntMap ctor
  CMapProjection();

  /// CIntMap dtor
  virtual ~CMapProjection();

  static CMapProjection* GetInstance();

  bool ValidName(const char* name);
  bool ValidName(const string& name);
  
  string IdToName(uint32_t id);

  uint32_t NameToId(const char* name);
  uint32_t NameToId(const string& name);

  void NamesToArrayString(wxArrayString& array);
  void NamesToComboBox(wxComboBox& combo);

  string NameToLabeledName(const string& name);

  virtual uint32_t Exists(const string& key);
  virtual uint32_t operator[](const string& key);

protected:

public:


   /**
   * unique instance of the class
   */
   static CMapProjection* m_instance;

//  CStringArray m_idToName;
/*
  static uint32_t VTK_PROJ2D_LAMBERT_CYLINDRICAL;
  static uint32_t VTK_PROJ2D_PLATE_CAREE;
  static uint32_t VTK_PROJ2D_MOLLWEIDE;
  static uint32_t VTK_PROJ2D_ROBINSON;
  static uint32_t VTK_PROJ2D_LAMBERT_AZIMUTHAL;
  static uint32_t VTK_PROJ2D_AZIMUTHAL_EQUIDISTANT;
  static uint32_t VTK_PROJ2D_3D;

*/

private :
  static CSmartCleanerMapProj	m_SmartCleaner;


};

class CSmartCleanerMapProj
{
    public :
        CSmartCleanerMapProj( CMapProjection * pObject = NULL ) : m_pObject( pObject ) { }
        virtual ~CSmartCleanerMapProj()
        {
            if( m_pObject != NULL )
            {
                delete m_pObject;
            }
        }
        void SetObject( CMapProjection * pObject )
        {
            m_pObject = pObject;
        }
        CMapProjection * GetObject()
        {
            return m_pObject;
        }
    private :
        CMapProjection * m_pObject;
};



#endif
