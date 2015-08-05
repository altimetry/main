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

#ifndef __CheckListBox_H__
#define __CheckListBox_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "CheckListBox.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "brathl.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// CCheckListBox
//----------------------------------------------------------------------------

class CCheckListBox: public wxCheckListBox
{
public:
  // constructors and destructors
  CCheckListBox( wxWindow *parent, wxWindowID id = -1,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = 0 );
  virtual ~CCheckListBox();
  
  // WDR: method declarations for CCheckListBox

  virtual void CheckAll(bool bCheck = true);
  virtual void DeleteAll();
  virtual uint32_t CountCheck();
  virtual void GetCheckItemStrings(wxArrayString& to, bool removeAll = true);
    
private:
    // WDR: member variable declarations for CCheckListBox
    
private:
    // WDR: handler declarations for CCheckListBox

private:
    DECLARE_CLASS(CCheckListBox)
    DECLARE_EVENT_TABLE()
};




#endif
