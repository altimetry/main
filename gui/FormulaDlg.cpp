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
    #pragma implementation "FormulaDlg.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "BratGui.h"
#include "Validators.h"
#include "Workspace.h"
#include "FormulaDlg.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// CFormulasDlg
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(CFormulasDlg,wxDialog)

// WDR: event table for CFormulasDlg

BEGIN_EVENT_TABLE(CFormulasDlg,wxDialog)
    EVT_BUTTON( wxID_OK, CFormulasDlg::OnOk )
    EVT_BUTTON( wxID_CANCEL, CFormulasDlg::OnCancel )
    EVT_BUTTON( ID_FORMULA_REMOVE, CFormulasDlg::OnRemove )
    EVT_LISTBOX( ID_FORMULA_LIST, CFormulasDlg::OnFormulaList )
END_EVENT_TABLE()
//----------------------------------------

CFormulasDlg::CFormulasDlg( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxDialog( parent, id, title, position, size, style )
{
    // WDR: dialog function FormulasDlg for CFormulasDlg
    //--------------------------------
    FormulasDlg( this, TRUE ); 
    //--------------------------------

    FillFormulaList();

    GetFormulaUnit()->SetLabel("");
    GetFormulaRemove()->Enable(false);

    m_formula = NULL;

}
//----------------------------------------

CFormulasDlg::~CFormulasDlg()
{
}

// WDR: handler implementations for CFormulasDlg
//----------------------------------------

void CFormulasDlg::OnFormulaList( wxCommandEvent &event )
{
  int32_t sel = event.GetInt();

  GetFormulaUnit()->SetLabel("");
  GetFormulaValue()->SetValue("");
  
  CWorkspaceFormula* wks = wxGetApp().GetCurrentWorkspaceFormula();
  if (wks == NULL)
  {
    return;
  }

  if (sel < 0)
  {
    return;
  }

  wxString name = GetFormulaList()->GetString(static_cast<uint32_t>(sel));

  m_formula = wks->GetFormula(name);
  
  wxString value = wks->GetDescFormula(name, false);
 // value = CTools::SlashesDecode(value.c_str()).c_str();

  GetFormulaValue()->SetValue(value);
  if (m_formula != NULL)
  {
    GetFormulaUnit()->SetLabel(m_formula->GetUnitAsText());
    GetFormulaRemove()->Enable(!(m_formula->IsPredefined()));
  }
  else
  {
    GetFormulaUnit()->SetLabel("");
    GetFormulaRemove()->Enable(false);
  }


}
//----------------------------------------

void CFormulasDlg::OnRemove( wxCommandEvent &event )
{
  CWorkspaceFormula* wks = wxGetApp().GetCurrentWorkspaceFormula();
  if (wks == NULL)
  {
    return;
  }

  if (GetFormulaList()->GetSelection() < 0)
  {
    return;
  }

  wxString name = GetFormulaList()->GetStringSelection();

  CFormula* formula = wks->GetFormula(name);
  if (formula == NULL)
  {
    return;
  }

  if (formula->IsPredefined())
  {
    wxMessageBox("Sorry, you are not allowed to remove 'predefined' formulas",
                "Warning",
                wxOK | wxICON_EXCLAMATION);
    return;
  }


  int32_t result = wxMessageBox(wxString::Format("Do you to really want to remove formula '%s'\n"
                                                 "WARNING:If you remove it, be sure that it's not use anymore in the current workspace\n"
                                                 "Otherwise, an error will occurs while executing operations that use it.",
                                                  name.c_str()),
                               "Warning",
                                wxYES_NO | wxCENTRE | wxICON_QUESTION);

  if (result != wxYES)
  {
    return;
  }

  wks->RemoveFormula(name);
  m_formula = NULL;

  FillFormulaList();

  GetFormulaList()->SetSelection(-1);
  GetFormulaValue()->SetValue("");

}
//----------------------------------------

void CFormulasDlg::OnOk(wxCommandEvent &event)
{
    event.Skip();
}
//----------------------------------------

void CFormulasDlg::OnCancel(wxCommandEvent &event)
{
    event.Skip();
}
//----------------------------------------
void CFormulasDlg::FillFormulaList()
{
  bool bOk = true;
  
  GetFormulaList()->Clear();

  CWorkspaceFormula* wks = wxGetApp().GetCurrentWorkspaceFormula();
  if (wks == NULL)
  {
    return;
  }
  
  if (wks->GetFormulaCount() <= 0)
  {
    return;
  }
  
  wks->GetFormulaNames(*GetFormulaList());

}


//----------------------------------------------------------------------------
// CFormulaDlg
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(CFormulaDlg,wxDialog)

// WDR: event table for CFormulaDlg

BEGIN_EVENT_TABLE(CFormulaDlg,wxDialog)
    EVT_BUTTON( wxID_OK, CFormulaDlg::OnOk )
    EVT_BUTTON( wxID_CANCEL, CFormulaDlg::OnCancel )
END_EVENT_TABLE()

//----------------------------------------
CFormulaDlg::CFormulaDlg(wxWindow *parent, wxWindowID id, const wxString &title,
                         CFormula* formula,
                         const wxPoint &position, const wxSize& size, long style ) 
    :  wxDialog( parent, id, title, position, size, style )
{
  m_formula = formula;
  // WDR: dialog function FormulaDlg for CFormulaDlg
  //----------------------------
  FormulaDlg( this, TRUE ); 
  //----------------------------

  CTextLightValidator textLightValidator;
  GetFormulaName()->SetValidator(textLightValidator);

  if (m_formula != NULL)
  {
    GetFormulaName()->SetValue(m_formula->GetName());
    GetFormulaValue()->SetValue(m_formula->GetDescription(false));
    GetFormulaComment()->SetValue(m_formula->GetComment(false));
    GetFormulaUnit()->SetValue(m_formula->GetUnitAsText());
    GetFormulaTitle()->SetValue(m_formula->GetTitle());
  }

}

//----------------------------------------
CFormulaDlg::~CFormulaDlg()
{
}

//----------------------------------------
bool CFormulaDlg::ValidateData()
{
  bool bOk = true;
  bOk &= VerifyName();
  m_formula->SetName(GetFormulaName()->GetValue());
  m_formula->SetComment(GetFormulaComment()->GetValue());
  m_formula->SetTitle(GetFormulaTitle()->GetValue());
  m_formula->SetUnit(GetFormulaUnit()->GetValue());
  return bOk;

}

//----------------------------------------
bool CFormulaDlg::VerifyName()
{
  GetFormulaName()->GetValue().Trim(true);
  GetFormulaName()->GetValue().Trim(false);
  if (GetFormulaName()->GetValue().IsEmpty())
  {
    wxMessageBox("Formula has no name\nPlease set a name for the formula",
                 "Warning",
                  wxOK | wxCENTRE | wxICON_EXCLAMATION);
    return false;
  }

  return true;
}

//----------------------------------------

// WDR: handler implementations for CFormulaDlg


//----------------------------------------
void CFormulaDlg::OnOk(wxCommandEvent &event)
{
  bool bOk = ValidateData();
  
  if (bOk)
  {  
    event.Skip();
  }

}
//----------------------------------------
void CFormulaDlg::OnCancel(wxCommandEvent &event)
{
  event.Skip();
}
//----------------------------------------




