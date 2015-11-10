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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "ColorPalette.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif



#include "PlotData/ColorPalleteNames.h"
#include "ColorPalette.h"


//-------------------------------------------------------------
//------------------- CListColorPalette class --------------------
//-------------------------------------------------------------

CListColorPalette::CListColorPalette()
{

  Insert(PALETTE_AEROSOL);
  Insert(PALETTE_AEROSOL2);
  Insert(PALETTE_BLACKTOWHITE);
  Insert(PALETTE_WHITETOBLACK);
  Insert(PALETTE_REDTOGREEN);
  Insert(PALETTE_GREENTORED);
  Insert(PALETTE_CLOUD);
  Insert(PALETTE_RAINBOW);
  Insert(PALETTE_RAINBOW2);
  Insert(PALETTE_OZONE);
  Insert(PALETTE_BLACKBODY);
}


//----------------------------------------
CListColorPalette::~CListColorPalette()
{

}

//----------------------------------------
CListColorPalette& CListColorPalette::GetInstance()
{
 static CListColorPalette instance;

 return instance;
}

//----------------------------------------
void CListColorPalette::NamesToArrayString(wxArrayString& array)
{
  CListColorPalette::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    array.Add((*it).c_str());
  }

}
//----------------------------------------
void CListColorPalette::NamesToComboBox(wxComboBox& combo)
{
  CListColorPalette::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    combo.Append((*it).c_str());
  }

}


