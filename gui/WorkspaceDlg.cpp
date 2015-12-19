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

#include "new-gui/brat/stdafx.h"

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "brathl.h"

#include "Tools.h"
#include "new-gui/Common/tools/Exception.h"
using namespace brathl;

#include "BratGui.h"
#include "Validators.h"

#include "WorkspaceDlg.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// CWorkspaceDlg
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(CWorkspaceDlg,wxDialog)

// WDR: event table for CWorkspaceDlg

BEGIN_EVENT_TABLE(CWorkspaceDlg,wxDialog)
    EVT_BUTTON( ID_WKS_BROWSE, CWorkspaceDlg::OnBrowse )
    EVT_BUTTON( wxID_OK, CWorkspaceDlg::OnOk )
    EVT_CHECKBOX( ID_IMPORT_DATASET, CWorkspaceDlg::OnCheckDataset )
    EVT_CHECKBOX( ID_IMPORT_FORMULAS, CWorkspaceDlg::OnCheckFormula )
    EVT_CHECKBOX( ID_IMPORT_OPERATIONS, CWorkspaceDlg::OnCheckOperation )
    EVT_CHECKBOX( ID_IMPORT_VIEWS, CWorkspaceDlg::OnCheckView )
    EVT_TOGGLEBUTTON( ID_IMPORT_FORMULAS_SELECT, CWorkspaceDlg::OnSelectFormulas )
    EVT_BUTTON( ID_IMPORT_FORMULAS_SELECT_ALL, CWorkspaceDlg::OnSelectAllFormulas )
    EVT_BUTTON( ID_IMPORT_FORMULAS_DESELECT_ALL, CWorkspaceDlg::OnDeselectAllFormulas )
    EVT_TEXT( ID_WKS_LOC, CWorkspaceDlg::OnLocation )
    EVT_TEXT_ENTER( ID_WKS_LOC, CWorkspaceDlg::OnLocationEnter )
    EVT_CHECKLISTBOX( ID_IMPORT_FORMULAS_LIST, CWorkspaceDlg::OnCheckFormulaList )
END_EVENT_TABLE()

//----------------------------------------
CWorkspaceDlg::CWorkspaceDlg( wxWindow *parent, wxWindowID id, const wxString &title,
                              actionFlags action, CWorkspace* wks,
                              const wxString& initialDir,
                              const wxPoint &position, const wxSize& size, long style ) :
                              wxDialog( parent, id, title, position, size, style )
{
//  m_formulasCheckListBox = NULL;

  //m_currentDir.SetName("");

  // WDR: dialog function WorkspaceDlg for CWorkspaceDlg

  //----------------------------
  WorkspaceDlg( this, TRUE );
  //----------------------------

  CTextLightValidator textLightValidator;
  GetWksName()->SetValidator(textLightValidator);

  CTextDirValidator textDirValidator;
  GetWksLoc()->SetValidator(textDirValidator);

  brathlGuiImportOptionsSizer->Show(false);
  glb_FormulaListToImportSizer->Show(false);

  if (wks != NULL)
  {
    GetWksName()->SetValue(wks->GetName());
    m_currentDir.Assign(wks->GetPath());
  }
  else
  {
    m_currentDir.AssignDir(initialDir);
  }

  m_currentDir.Normalize();

  wxArrayString dirs = m_currentDir.GetDirs();


  m_action = action;

  switch (m_action)
  {
    case wksNew :
    {
      GetWksName()->Enable(true);
      GetWksName()->Clear();
      GetWksLoc()->Enable(true);
      GetOk()->SetLabel("Create");
      if (dirs.GetCount() > 0)
      {
        m_currentDir.RemoveLastDir();
      }
      break;
    }
    case wksOpen:
      GetWksName()->Enable(false);
      GetWksName()->Clear();
      GetWksLoc()->Enable(true);
      GetOk()->SetLabel("Open");
      break;
    case wksImport :
      GetWksName()->Enable(false);
      GetWksName()->Clear();
      GetWksLoc()->Enable(true);
      GetOk()->SetLabel("Import");
      if (dirs.GetCount() > 0)
      {
        m_currentDir.RemoveLastDir();
      }

      brathlGuiImportOptionsSizer->Show(true);

      break;
    case wksRename :
      GetWksName()->Enable(true);
      GetWksLoc()->Enable(false);
      GetOk()->SetLabel("Rename");
      break;
    case wksDelete :
      GetWksName()->Enable(false);
      GetWksLoc()->Enable(false);
      GetOk()->SetLabel("Delete");
      break;
    default:
      std::string msg = CTools::Format("ERROR - CWorkspaceDlg ctor : unexpected action flags %d",
                                  m_action);
      CUnImplementException e(msg, BRATHL_UNIMPLEMENT_ERROR);
      throw(e);
      break;
  }

  GetWksLoc()->SetValue(m_currentDir.GetPath());
  GetWksBrowse()->Enable(GetWksLoc()->IsEnabled());

  CBratGuiApp::EvtChar(*GetWksLoc(), wxEVT_CHAR,
                          (wxCharEventFunction)&CWorkspaceDlg::OnLocationChar,
                          NULL,
                          this);

  CBratGuiApp::EvtFocus(*GetWksLoc(), wxEVT_KILL_FOCUS,
                          (wxFocusEventFunction)&CWorkspaceDlg::OnLocationKillFocus,
                          NULL,
                          this);

}

//----------------------------------------
CWorkspaceDlg::~CWorkspaceDlg()
{
}


//----------------------------------------
bool CWorkspaceDlg::VerifyName()
{
  GetWksName()->GetValue().Trim(true);
  GetWksName()->GetValue().Trim(false);
  if (GetWksName()->GetValue().IsEmpty())
  {
    wxMessageBox("Workspace has no name\nPlease set a name for the workspace",
                 "Warning",
                  wxOK | wxCENTRE | wxICON_EXCLAMATION);
    return false;
  }

  return true;
}
//----------------------------------------
bool CWorkspaceDlg::VerifyPathName()
{
  GetWksLoc()->GetValue().Trim(true);
  GetWksLoc()->GetValue().Trim(false);
  //wxString name = GetWksName()->GetValue();
  wxFileName path;
  path.AssignDir(GetWksLoc()->GetValue());

  if ( (path.IsDir() == false) || (GetWksLoc()->GetValue().IsEmpty()) )
  {
      wxMessageBox(wxString::Format("Invalid pathname %s\nPlease set a valid pathname\n"
                                    "(if the pathname doesn't exist, it will be created",
                                    GetWksLoc()->GetValue().c_str()),
                   "Warning",
                    wxOK | wxCENTRE | wxICON_EXCLAMATION);
    return false;
  }

  return true;
}
//----------------------------------------
bool CWorkspaceDlg::VerifyPath(bool withMsg)
{
  if (VerifyPathName() == false)
  {
    return false;
  }

  wxFileName path;
  path.AssignDir(GetWksLoc()->GetValue());

  if (path.DirExists() == false)
  {
    if (withMsg)
    {
      wxMessageBox(wxString::Format("Pathname %s doesn't exist \nPlease set a correct pathname\n",
                                    path.GetPath().c_str()),
                   "Warning",
                    wxOK | wxCENTRE | wxICON_EXCLAMATION);
    }
    return false;
  }

  return true;
}
//----------------------------------------
bool CWorkspaceDlg::VerifyConfig( bool withMsg )
{
	if ( !VerifyPath( withMsg ) )
		return false;

	CWorkspace wks;
	wks.SetPath( GetWksLoc()->GetValue().ToStdString(), false );
	bool bOk = wks.IsConfigFile();
	if ( bOk )
	{
		std::string errorMsg;
		bOk &= wks.LoadConfig( errorMsg, wxGetApp().GetCurrentWorkspaceDataset(), wxGetApp().GetCurrentWorkspaceOperation() );
		if ( !errorMsg.empty() )
			wxMessageBox( errorMsg, "Warning", wxOK | wxCENTRE | wxICON_INFORMATION );

		bOk &= wks.IsRoot();
	}
	if ( bOk == false )
	{
		if ( withMsg )
		{
			wxMessageBox( wxString::Format( "Path %s doesn't contain any workspace informations or is not the root folder\nPlease set a correct pathname\n",
				GetWksLoc()->GetValue().c_str() ),
				"Warning",
				wxOK | wxCENTRE | wxICON_EXCLAMATION );
		}
	}
	else
	{
		GetWksName()->SetValue( wks.GetName() );
	}
	return bOk;
}

//----------------------------------------
bool CWorkspaceDlg::VerifyNotConfig(bool withMsg)
{
  bool bOk = true;

  if (VerifyConfig(false))
  {
    bOk = false;
    if (withMsg)
    {
      wxMessageBox(wxString::Format("Path %s seems to already contains a workspace\nPlease set a correct pathname\n",
                                    GetWksLoc()->GetValue().c_str()),
                   "Warning",
                    wxOK | wxCENTRE | wxICON_EXCLAMATION);
    }
  }

  return bOk;
}


//----------------------------------------
void CWorkspaceDlg::FillImportFormulas()
{
	if ( m_action != wksImport )
	{
		return;
	}

	bool bOk = false;

	GetImportFormulasList()->DeleteAll();

	m_currentDir.AssignDir( GetWksLoc()->GetValue() );
	m_currentDir.Normalize();
	m_currentDir.MakeAbsolute();

	bOk = VerifyConfig( false );

	if ( !bOk )
	{
		return;
	}

	CWorkspace* wks = new CWorkspace( GetWksName()->GetValue().ToStdString(), m_currentDir.GetFullPath().ToStdString() );

	wxGetApp().CreateTree( wks, wxGetApp().m_treeImport );

	std::string errorMsg;
	CWorkspaceFormula* wksFormula =  wxGetApp().m_treeImport.LoadConfigFormula( wxGetApp().GetCurrentWorkspaceDataset(), wxGetApp().GetCurrentWorkspaceOperation(), errorMsg );
	if ( !errorMsg.empty() )
		wxMessageBox( errorMsg, "Warning", wxOK | wxCENTRE | wxICON_INFORMATION );

	if ( wksFormula != NULL )
	{
		m_userFormulas.Clear();
		std::vector< std::string > userFormulas;
		wksFormula->GetFormulaNames( userFormulas, false, true );
		for ( auto &s : userFormulas )
			m_userFormulas.Add( s );
	}

	wxGetApp().m_treeImport.DeleteTree();

	GetImportFormulasList()->Append( m_userFormulas );
	GetImportFormulasList()->CheckAll( GetImportFormulas()->GetValue() );

}
//----------------------------------------
// WDR: handler implementations for CWorkspaceDlg
//----------------------------------------
//----------------------------------------

void CWorkspaceDlg::OnCheckFormulaList( wxCommandEvent &event )
{
  //uint32_t nItem = event.GetInt();

  GetImportFormulas()->SetValue( (GetImportFormulasList()->CountCheck() != 0) );

}

//----------------------------------------
void CWorkspaceDlg::OnLocationChar(wxKeyEvent &event )
{
  //FillImportFormulas();
  event.Skip();
}
//----------------------------------------
void CWorkspaceDlg::OnLocationKillFocus( wxFocusEvent &event )
{
//  FillImportFormulas();
  event.Skip();
}
//----------------------------------------

void CWorkspaceDlg::OnLocationEnter( wxCommandEvent &event )
{
  FillImportFormulas();
  event.Skip();

}
//----------------------------------------

void CWorkspaceDlg::OnLocation( wxCommandEvent &event )
{
  FillImportFormulas();
  event.Skip();
}
//----------------------------------------

void CWorkspaceDlg::OnDeselectAllFormulas( wxCommandEvent &event )
{
  GetImportFormulasList()->CheckAll(false);
  GetImportFormulas()->SetValue(false);
}
//----------------------------------------

void CWorkspaceDlg::OnSelectAllFormulas( wxCommandEvent &event )
{
  GetImportFormulasList()->CheckAll();
  GetImportFormulas()->SetValue(true);
}
//----------------------------------------

void CWorkspaceDlg::OnSelectFormulas( wxCommandEvent &event )
{
  bool sel = GetImportFormulasSelect()->GetValue();
  if (!sel)
  {
    /*
    if (m_formulasCheckListBox != NULL)
    {
      GetImportFormulasSelect()->SetLabel("Select formulas");
      glbFormulaListToImport->Remove(m_formulasCheckListBox);
      m_formulasCheckListBox->Destroy();
      glbFormulaListToImport->Show(false);

      glb_WorkspaceDlgSizer->Fit(this);
    }
    */

    glb_FormulaListToImportSizer->Show(false);
    glb_WorkspaceDlgSizer->Fit(this);
    GetImportFormulasSelect()->SetLabel("Select formulas");
    return;
  }

  long flags = 0;


  GetImportFormulasSelect()->SetLabel("Hide formulas");

//  glbFormulaListToImport->Add(m_formulasCheckListBox, 0, wxALIGN_CENTER|wxALL, 5);
  glb_WorkspaceDlgSizer->Fit(this);
  glb_FormulaListToImportSizer->Show(true);
  glb_FormulaListToImportSizer->Fit(this);
  glb_WorkspaceDlgSizer->Fit(this);

}

//----------------------------------------
void CWorkspaceDlg::OnCheckView( wxCommandEvent &event )
{
  bool value = GetImportViews()->GetValue();
  if (value)
  {
    GetImportOperations()->SetValue(value);
    GetImportFormulas()->SetValue(value);
    GetImportDataset()->SetValue(value);
  }

}

//----------------------------------------
void CWorkspaceDlg::OnCheckOperation( wxCommandEvent &event )
{
  bool value = GetImportOperations()->GetValue();
  if (value)
  {
    GetImportFormulas()->SetValue(value);
    GetImportDataset()->SetValue(value);
  }
  else
  {
    GetImportViews()->SetValue(value);
  }

}

//----------------------------------------
void CWorkspaceDlg::OnCheckFormula( wxCommandEvent &event )
{
  bool value = GetImportFormulas()->GetValue();

  GetImportFormulasList()->CheckAll( value );

  if (value)
  {
    //GetImportDataset()->SetValue(value);
  }
  else
  {
    GetImportOperations()->SetValue(value);
    GetImportViews()->SetValue(value);
  }


}

//----------------------------------------
void CWorkspaceDlg::OnCheckDataset( wxCommandEvent &event )
{

  bool value = GetImportDataset()->GetValue();
  if (value == false)
  {
    GetImportFormulas()->SetValue(value);
    GetImportOperations()->SetValue(value);
    GetImportViews()->SetValue(value);
  }
}

//----------------------------------------
void CWorkspaceDlg::OnOk( wxCommandEvent &event )
{
  bool bOk = ValidateData();

  if (bOk)
  {
    m_currentDir.AssignDir(GetWksLoc()->GetValue());
    m_currentDir.Normalize();
    m_name = GetWksName()->GetValue();
    event.Skip();
  }

}

//----------------------------------------
void CWorkspaceDlg::GetCheckedFormulas()
{
  GetImportFormulasList()->GetCheckItemStrings(m_userFormulas);

}
//----------------------------------------
void CWorkspaceDlg::GetCheckedFormulas( std::vector<std::string>& to )
{
	wxArrayString wto;
	GetImportFormulasList()->GetCheckItemStrings( wto );
	for ( size_t i = 0; i < wto.GetCount(); ++i )
		to.push_back( wto[ i ].ToStdString() );
}
//----------------------------------------
bool CWorkspaceDlg::ValidateData()
{
  bool bOk= true;

  switch (m_action)
  {
    case wksNew :
      bOk &= VerifyName();
      bOk &= VerifyPathName();
      bOk &= VerifyNotConfig();
      break;
    case wksOpen :
      bOk &= VerifyConfig();
      break;
    case wksImport :
      bOk &= VerifyConfig();
      break;
    case wksRename :
      bOk &= VerifyName();
      break;
    case wksDelete :
      bOk &= VerifyPathName();
      bOk &= VerifyConfig();
      break;
    default:
      std::string msg = CTools::Format("ERROR - CWorkspaceDlg::Validate : unexpected action flags %d",
                                  m_action);
      CUnImplementException e(msg, BRATHL_UNIMPLEMENT_ERROR);
      throw(e);
      break;
  }

  return bOk;

}
//----------------------------------------
void CWorkspaceDlg::OnBrowse( wxCommandEvent &event )
{
	m_currentDir.AssignDir( GetWksLoc()->GetValue() );
	m_currentDir.Normalize();

	int32_t style = wxDD_DEFAULT_STYLE;

	if ( m_action == wksNew )
	{
		style |= wxDD_NEW_DIR_BUTTON;
	}
	else
	{
		style &= ~wxDD_NEW_DIR_BUTTON;
	}

	wxDirDialog dirDlg( this, "Select a directory for the workspace", m_currentDir.GetPath(), style );

	if ( dirDlg.ShowModal() != wxID_OK )
	{
		return;
	}

	m_currentDir.AssignDir( dirDlg.GetPath() );

	m_currentDir.Normalize();


	if ( m_currentDir.GetPath().IsEmpty() == false )
	{
		GetWksLoc()->SetValue( m_currentDir.GetPath() );
		GetWksLoc()->SetInsertionPointEnd();
	}

	bool bOk = true;
	switch ( m_action )
	{
		case wksOpen:
		case wksImport:
			bOk &= VerifyConfig();
			if ( bOk )
			{
				CWorkspace wks;
				wks.SetPath( GetWksLoc()->GetValue().ToStdString() );

				std::string errorMsg;
				wks.LoadConfig( errorMsg, wxGetApp().GetCurrentWorkspaceDataset(), wxGetApp().GetCurrentWorkspaceOperation() );
				if ( !errorMsg.empty() )
					wxMessageBox( errorMsg, "Warning", wxOK | wxCENTRE | wxICON_INFORMATION );

				GetWksName()->SetValue( wks.GetName() );
			}
			break;
		case wksNew:
		case wksRename:
		case wksDelete:
			break;
		default:
			break;
	}
}




