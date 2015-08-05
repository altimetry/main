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
    #pragma implementation "FunctionDlg.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "BratGui.h"

#include "Function.h"

#include "FunctionDlg.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// CFunctionsDlg
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(CFunctionsDlg,wxDialog)

// WDR: event table for CFunctionsDlg

BEGIN_EVENT_TABLE(CFunctionsDlg,wxDialog)
    EVT_BUTTON( wxID_OK, CFunctionsDlg::OnOk )
    EVT_BUTTON( wxID_CANCEL, CFunctionsDlg::OnCancel )
    EVT_CHOICE( ID_FUNCTION_CAT, CFunctionsDlg::OnFunctionCat )
    EVT_LISTBOX( ID_FUNCTION_LIST, CFunctionsDlg::OnFunctionList )
END_EVENT_TABLE()
//----------------------------------------

CFunctionsDlg::CFunctionsDlg( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxDialog( parent, id, title, position, size, style )
{
    // WDR: dialog function FunctionsDlg for CFunctionsDlg
    //----------------------------------------
    FunctionsDlg( this, TRUE ); 
    //----------------------------------------

    FillCategoryList();
}

//----------------------------------------
CFunctionsDlg::~CFunctionsDlg()
{
}

//----------------------------------------
void CFunctionsDlg::FillCategoryList()
{
  GetFunctionCat()->Clear();
  CMapFunction::GetInstance().GetCategory(*GetFunctionCat());

  int32_t sel = 0;
  GetFunctionCat()->SetSelection(0);
  FunctionCat(sel);

}
//----------------------------------------
void CFunctionsDlg::FillFunctionList(int32_t category)
{
  GetFunctionList()->Clear();
  CMapFunction::GetInstance().NamesToListBox(*GetFunctionList(), category);

  int32_t sel = 0;
  GetFunctionList()->SetSelection(sel);
  FunctionList(sel);

}
//----------------------------------------

// WDR: handler implementations for CFunctionsDlg
//----------------------------------------

//----------------------------------------
void CFunctionsDlg::OnFunctionList( wxCommandEvent &event )
{
  FunctionList(event.GetInt());
}
//----------------------------------------
void CFunctionsDlg::FunctionList(int32_t sel)
{
  GetFunctionComment()->SetValue("");

  if (sel < 0)
  {
    return;
  }

  wxString funcName = GetFunctionList()->GetString(sel);
  GetFunctionComment()->SetValue(CMapFunction::GetInstance().GetDescFunc(funcName));
}
//----------------------------------------

void CFunctionsDlg::OnFunctionCat( wxCommandEvent &event )
{
  FunctionCat(event.GetInt());
    
}
//----------------------------------------

void CFunctionsDlg::FunctionCat(int32_t sel)
{
  GetFunctionList()->Clear();
  
  if (sel < 0)
  {
    return;
  }
  
  int32_t* category = static_cast<int32_t*>(GetFunctionCat()->GetClientData(sel));
  //int32_t* category = static_cast<int32_t*>(event.GetClientData());

  if (category != NULL)
  {
    FillFunctionList(*category);
  }
    
}

//----------------------------------------
void CFunctionsDlg::OnOk(wxCommandEvent &event)
{
    event.Skip();
}

//----------------------------------------
void CFunctionsDlg::OnCancel(wxCommandEvent &event)
{
    event.Skip();
}



