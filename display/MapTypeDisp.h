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
#if !defined(_MapTypeDisp_h_)
#define _MapTypeDisp_h_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "MapTypeDisp.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "List.h"
using namespace brathl;


//-------------------------------------------------------------
//------------------- CMapTypeDisp class --------------------
//-------------------------------------------------------------

class CMapTypeDisp: public CUIntMap
{
public:
  CMapTypeDisp();

  virtual ~CMapTypeDisp();

  static CMapTypeDisp&  GetInstance();


  bool ValidName(const char* name);
  bool ValidName(const std::string& name);

  wxString IdToName(uint32_t id);
  uint32_t NameToId(const wxString& name);

  void NamesToArrayString(wxArrayString& array);
  void NamesToComboBox(wxComboBox& combo);

  wxString Enum();


protected:

public:

  enum typeDisp
  {
    typeDispYFX,
    typeDispZFXY,
    typeDispZFLatLon
  };



private :


};



#endif 
