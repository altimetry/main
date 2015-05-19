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
#ifndef __MapImageType_H__
#define __MapImageType_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "MapImageType.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "brathl.h"

#include "List.h"
using namespace brathl;

#include "vtkImageWriter.h"
#include "vtkBMPWriter.h"
#include "vtkJPEGWriter.h"
#include "vtkPNGWriter.h"
#include "vtkPNMWriter.h"
#include "vtkPostScriptWriter.h"
#include "vtkTIFFWriter.h"
#include "vtkMetaImageWriter.h"
#include "vtkImageWriter.h"

#include "wx/arrstr.h"
//-------------------------------------------------------------
//------------------- CImageType class --------------------
//-------------------------------------------------------------

class CImageType: public CBratObject
{
public:
  /// CImageType ctor
  CImageType(const wxString& name, vtkImageWriter* vtkImageWriter);
  /// CImageType dtor
  virtual ~CImageType();
  
  wxString m_name;
  vtkImageWriter* m_vtkImageWriter;
};


////class CSmartCleanerMapImageType;
//-------------------------------------------------------------
//------------------- CMapImageType class --------------------
//-------------------------------------------------------------

class CMapImageType: public CObMap
{
public:
  /// CIntMap ctor
  CMapImageType();

  /// CIntMap dtor
  virtual ~CMapImageType();

  //static CMapImageType&  GetInstance();

  bool ValidName(const char* name);
  bool ValidName(const string& name);
  
  void NamesToArrayString(wxArrayString& array);
  void NamesToMapString(CStringMap& strMap);


protected:

public:


private :

  //static CSmartCleanerMapImageType	m_SmartCleaner;


};

/*
class CSmartCleanerMapImageType
{
    public :
        CSmartCleanerMapImageType( CMapImageType * pObject = NULL ) : m_pObject( pObject ) { }
        virtual ~CSmartCleanerMapImageType()
        {
            if( m_pObject != NULL )
            {
                delete m_pObject;
            }
        }
        void SetObject( CMapImageType * pObject )
        {
            m_pObject = pObject;
        }
        CMapImageType * GetObject()
        {
            return m_pObject;
        }
    private :
        CMapImageType * m_pObject;
};

*/

#endif
