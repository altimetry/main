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

#ifndef __FormulaDlg_H__
#define __FormulaDlg_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "BratGui_wdr.h"

#include "new-gui/brat/DataModels/Workspaces/Formula.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// CFormulasDlg
//----------------------------------------------------------------------------

class CFormulasDlg: public wxDialog
{
public:
    // constructors and destructors
    CFormulasDlg( wxWindow *parent, wxWindowID id, const wxString &title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    virtual ~CFormulasDlg();
    
    // WDR: method declarations for CFormulasDlg
    wxButton* GetFormulaRemove()  { return (wxButton*) FindWindow( ID_FORMULA_REMOVE ); }
    wxStaticText* GetFormulaUnit()  { return (wxStaticText*) FindWindow( ID_FORMULA_UNIT ); }
    wxTextCtrl* GetFormulaValue()  { return (wxTextCtrl*) FindWindow( ID_FORMULA_VALUE ); }
    wxCheckBox* GetFormulaAlias()  { return (wxCheckBox*) FindWindow( ID_FORMULA_ALIAS ); }
    wxListBox* GetFormulaList()  { return (wxListBox*) FindWindow( ID_FORMULA_LIST ); }
public:
  CFormula* m_formula;
private:
    // WDR: member variable declarations for CFormulasDlg
    void FillFormulaList();

private:
    // WDR: handler declarations for CFormulasDlg
    void OnFormulaList( wxCommandEvent &event );
    void OnRemove( wxCommandEvent &event );
    void OnOk( wxCommandEvent &event );
    void OnCancel( wxCommandEvent &event );

private:
    DECLARE_CLASS(CFormulasDlg)
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// CFormulaDlg
//----------------------------------------------------------------------------

class CFormulaDlg: public wxDialog
{
public:
    // constructors and destructors
  CFormulaDlg(wxWindow *parent, wxWindowID id, const wxString &title,
              CFormula* formula,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxDEFAULT_DIALOG_STYLE );
  virtual ~CFormulaDlg();
    
  // WDR: method declarations for CFormulaDlg
    wxTextCtrl* GetFormulaTitle()  { return (wxTextCtrl*) FindWindow( ID_FORMULA_TITLE ); }
    wxTextCtrl* GetFormulaUnit()  { return (wxTextCtrl*) FindWindow( ID_FORMULA_UNIT ); }
  wxTextCtrl* GetFormulaValue()  { return (wxTextCtrl*) FindWindow( ID_FORMULA_VALUE ); }
  wxTextCtrl* GetFormulaComment()  { return (wxTextCtrl*) FindWindow( ID_FORMULA_COMMENT ); }
  wxTextCtrl* GetFormulaName()  { return (wxTextCtrl*) FindWindow( ID_FORMULA_NAME ); }
    
private:
  CFormula* m_formula;
  // WDR: member variable declarations for CFormulaDlg
    
private:
  bool ValidateData();
  bool VerifyName();

  // WDR: handler declarations for CFormulaDlg
  void OnOk( wxCommandEvent &event );
  void OnCancel( wxCommandEvent &event );

private:
  DECLARE_CLASS(CFormulaDlg)
  DECLARE_EVENT_TABLE()
};



#endif
