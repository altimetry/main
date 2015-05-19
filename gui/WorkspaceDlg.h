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

#ifndef __WorkspaceDlg_H__
#define __WorkspaceDlg_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "WorkspaceDlg.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/filename.h"
#include  "wx/checklst.h"

#include "BratGui_wdr.h"
#include "CheckListBox.h"
#include "Workspace.h"

//#include "GuiPanel.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// CWorkspaceDlg
//----------------------------------------------------------------------------

class CWorkspaceDlg: public wxDialog
{
public:
  // constructors and destructors
  CWorkspaceDlg( wxWindow *parent, wxWindowID id, const wxString &title,
                  CWorkspace::actionFlags action, CWorkspace* wks,
                  const wxString& initialDir, 
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = wxDEFAULT_DIALOG_STYLE );
  virtual ~CWorkspaceDlg();
  
  // WDR: method declarations for CWorkspaceDlg
  wxButton* GetImportFormulasDeselectAll()  { return (wxButton*) FindWindow( ID_IMPORT_FORMULAS_DESELECT_ALL ); }
  wxButton* GetImportFormulasSelectAll()  { return (wxButton*) FindWindow( ID_IMPORT_FORMULAS_SELECT_ALL ); }
  CCheckListBox* GetImportFormulasList()  { return (CCheckListBox*) FindWindow( ID_IMPORT_FORMULAS_LIST ); }
  wxToggleButton* GetImportFormulasSelect()  { return (wxToggleButton*) FindWindow( ID_IMPORT_FORMULAS_SELECT ); }
  wxCheckBox* GetImportViews()  { return (wxCheckBox*) FindWindow( ID_IMPORT_VIEWS ); }
  wxCheckBox* GetImportOperations()  { return (wxCheckBox*) FindWindow( ID_IMPORT_OPERATIONS ); }
  wxCheckBox* GetImportFormulas()  { return (wxCheckBox*) FindWindow( ID_IMPORT_FORMULAS ); }
  wxCheckBox* GetImportDataset()  { return (wxCheckBox*) FindWindow( ID_IMPORT_DATASET ); }
  wxButton* GetWksBrowse()  { return (wxButton*) FindWindow( ID_WKS_BROWSE ); }
  wxButton* GetOk()  { return (wxButton*) FindWindow( wxID_OK ); }
  wxTextCtrl* GetWksLoc()  { return (wxTextCtrl*) FindWindow( ID_WKS_LOC ); }
  wxTextCtrl* GetWksName()  { return (wxTextCtrl*) FindWindow( ID_WKS_NAME ); }

  void GetCheckedFormulas();
  void GetCheckedFormulas(wxArrayString& to);

  bool ValidateData();

public:
  wxFileName m_currentDir;
  wxString m_name;  

  wxArrayString m_userFormulas;

//  wxCheckListBox* m_formulasCheckListBox;

private:
    // WDR: member variable declarations for CWorkspaceDlg
  CWorkspace::actionFlags m_action;

private:

  void FillImportFormulas();

  bool VerifyName();
  bool VerifyPathName();
  bool VerifyPath(bool withMsg = true);
  bool VerifyConfig(bool withMsg = true);
  bool VerifyNotConfig(bool withMsg = true);

  // WDR: handler declarations for CWorkspaceDlg
    void OnCheckFormulaList( wxCommandEvent &event );
  void OnLocationEnter( wxCommandEvent &event );
  void OnLocation( wxCommandEvent &event );
  void OnLocationChar( wxKeyEvent &event );
  void OnLocationKillFocus( wxFocusEvent &event );
  void OnDeselectAllFormulas( wxCommandEvent &event );
  void OnSelectAllFormulas( wxCommandEvent &event );
  void OnSelectFormulas( wxCommandEvent &event );
  void OnCheckView( wxCommandEvent &event );
  void OnCheckOperation( wxCommandEvent &event );
  void OnCheckFormula( wxCommandEvent &event );
  void OnCheckDataset( wxCommandEvent &event );
  void OnOk( wxCommandEvent &event );
  void OnBrowse( wxCommandEvent &event );

private:
  DECLARE_CLASS(CWorkspaceDlg)
  DECLARE_EVENT_TABLE()
};




#endif
