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


#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "MapTypeDisp.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "brathl.h"

/*
#include "Trace.h" 
#include "Tools.h" 
#include "Exception.h" 
using namespace brathl;

#include "BratProcess.h"
using namespace processes;

#include "BratGui.h"
#include "Workspace.h"
 */
#include "MapTypeDisp.h"


//-------------------------------------------------------------
//------------------- CMapTypeDisp class --------------------
//-------------------------------------------------------------

CMapTypeDisp::CMapTypeDisp()
{
  Insert("Y=F(X)", typeDispYFX);
  Insert("Z=F(Lon,Lat)", typeDispZFLatLon);
  Insert("Z=F(X,Y)", typeDispZFXY);
}

//----------------------------------------
CMapTypeDisp::~CMapTypeDisp()
{

}

//----------------------------------------
CMapTypeDisp& CMapTypeDisp::GetInstance()
{
 static CMapTypeDisp instance;

 return instance;
}

//----------------------------------------
bool CMapTypeDisp::ValidName(const string& name)
{   
  return ValidName(name.c_str());
}
//----------------------------------------
bool CMapTypeDisp::ValidName(const char* name)
{   
  uint32_t value = Exists(name);
  return (!CTools::IsDefaultValue(value));
}

//----------------------------------------

wxString CMapTypeDisp::IdToName(uint32_t id)
{
  CMapTypeDisp::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    uint32_t value = it->second;
    if (value == id)
    {
      return (it->first).c_str();
    }
  }

  return "";
}


//----------------------------------------
uint32_t CMapTypeDisp::NameToId(const wxString& name)
{
  return Exists((const char *)(name));
}

//----------------------------------------
void CMapTypeDisp::NamesToArrayString(wxArrayString& array)
{
  CMapTypeDisp::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    uint32_t value = it->second;
    if (!CTools::IsDefaultValue(value))
    {
      array.Add( (it->first).c_str());
    }
  }

}
//----------------------------------------
void CMapTypeDisp::NamesToComboBox(wxComboBox& combo)
{
  CMapTypeDisp::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    uint32_t value = it->second;
    if (!CTools::IsDefaultValue(value))
    {
      //combo.Insert((it->first).c_str(), value);
      combo.Append( (it->first).c_str());
    }
  }

}
//----------------------------------------

wxString CMapTypeDisp::Enum()
{
  CMapTypeDisp::iterator it;
  
  wxString result;

  for (it = begin() ; it != end() ; it++)
  {
    result.Append(wxString::Format("%s ==> %d  ", (it->first).c_str(), (it->second)));
  }

  return result;
}


