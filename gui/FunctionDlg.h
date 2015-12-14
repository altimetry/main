/*
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

#ifndef __FunctionDlg_H__
#define __FunctionDlg_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "BratGui_wdr.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// CFunctionsDlg
//----------------------------------------------------------------------------

class CFunctionsDlg: public wxDialog
{
public:
    // constructors and destructors
    CFunctionsDlg( wxWindow *parent, wxWindowID id, const wxString &title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER );
    virtual ~CFunctionsDlg();
    
    // WDR: method declarations for CFunctionsDlg
    wxTextCtrl* GetFunctionComment()  { return (wxTextCtrl*) FindWindow( ID_FUNCTION_COMMENT ); }
    wxListBox* GetFunctionList()  { return (wxListBox*) FindWindow( ID_FUNCTION_LIST ); }
    wxChoice* GetFunctionCat()  { return (wxChoice*) FindWindow( ID_FUNCTION_CAT ); }
    
private:
    // WDR: member variable declarations for CFunctionsDlg
  void FillCategoryList();
  void FillFunctionList(int32_t category);

  void FunctionCat(int32_t sel);
  void FunctionList(int32_t sel);

  
private:
    // WDR: handler declarations for CFunctionsDlg
    void OnFunctionList( wxCommandEvent &event );
    void OnFunctionCat( wxCommandEvent &event );
    void OnOk( wxCommandEvent &event );
    void OnCancel( wxCommandEvent &event );

private:
    DECLARE_CLASS(CFunctionsDlg)
    DECLARE_EVENT_TABLE()
};




#endif
