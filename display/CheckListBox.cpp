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
    #pragma implementation "CheckListBox.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif


#include "CheckListBox.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// CCheckListBox
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(CCheckListBox,wxCheckListBox)

// WDR: event table for CCheckListBox

BEGIN_EVENT_TABLE(CCheckListBox,wxCheckListBox)
END_EVENT_TABLE()
//----------------------------------------

CCheckListBox::CCheckListBox( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxCheckListBox( parent, id, position, size, style )
{
}
//----------------------------------------

CCheckListBox::~CCheckListBox()
{
}

//----------------------------------------
void CCheckListBox::CheckAll(bool bCheck /*= true*/)
{
  uint32_t n = GetCount();

  for (uint32_t i = 0 ; i < n ; i++)
  {
    Check(i, bCheck);
  }
}
//----------------------------------------
void CCheckListBox::DeleteAll()
{
  uint32_t n = GetCount();

  for (uint32_t i = n ; i > 0 ; i--)
  {
    Delete(i-1);
  }
}
//----------------------------------------
uint32_t CCheckListBox::CountCheck()
{
  uint32_t nChecked = 0;
  uint32_t n = GetCount();

  for (uint32_t i = 0 ; i < n ; i++)
  {
    if (IsChecked(i))
    {
      nChecked++;
    }
  }

  return nChecked;
}

//----------------------------------------
void CCheckListBox::GetCheckItemStrings(wxArrayString& to, bool removeAll /*= true*/)
{
  if (removeAll)
  {
    to.Clear();
  }
  uint32_t n = GetCount();

  for (uint32_t i = 0 ; i < n ; i++)
  {
    if (IsChecked(i))
    {
      to.Add(GetString(i));
    }
  }

}

//----------------------------------------

// WDR: handler implementations for CCheckListBox




