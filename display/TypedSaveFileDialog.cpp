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
    #pragma implementation "TypedSaveFileDialog.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "brathl.h"

#include "Tools.h"
#include "List.h"
#include "Exception.h"
using namespace brathl;

#include "TypedSaveFileDialog.h"

#include "Config.h"


const char BYEXTENSION[] = "By Extension";

// WDR: class implementations

//----------------------------------------------------------------------------
// CTypedSaveFileDialog
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(CTypedSaveFileDialog,wxDialog)

// WDR: event table for CTypedSaveFileDialog

BEGIN_EVENT_TABLE(CTypedSaveFileDialog,wxDialog)
END_EVENT_TABLE()

CTypedSaveFileDialog::CTypedSaveFileDialog()
{
  Init();
}
//----------------------------------------
CTypedSaveFileDialog::CTypedSaveFileDialog( wxWindow *parent, const wxString& initialDir, const CStringMap& info,
                                            wxWindowID id, const wxString &title,
                                            const wxPoint &position, const wxSize& size, long style )
                  //: wxDialog( parent, id, title, position, size, style ) --> called in Create
{
  Init();

  bool bOk = Create(parent, initialDir, info, id, title, position, size, style);
  if (bOk == false)
  {
    CException e("ERROR in CTypedSaveFileDialog ctor - Unenable to create wxWidgets (Create method returns false)", BRATHL_SYSTEM_ERROR);
    throw(e);
  }
}
//----------------------------------------
CTypedSaveFileDialog::~CTypedSaveFileDialog()
{
}
//----------------------------------------
bool CTypedSaveFileDialog::Create( wxWindow *parent, const wxString& initialDir, const CStringMap& info,
                                  wxWindowID id, const wxString &title,
                                  const wxPoint &position, const wxSize& size, long style )
{
  bool bOk = true;

  wxDialog::Create(parent, id, title, position, size, style);

  m_currentDir.Assign(initialDir);
  m_currentDir.Normalize();
  m_currentDir.SetName("");

  m_info.Insert(info);
  m_info.Insert("", BYEXTENSION);

  bOk = CreateControls();
  if (bOk == false)
  {
    return false;
  }

  CreateLayout();
  InstallToolTips();
  InstallEventListeners();

  return true;
}

//----------------------------------------
void CTypedSaveFileDialog::Init()
{
  m_locLabel = NULL;
  m_locField = NULL;
  m_browseButton = NULL;
  m_typeLabel = NULL;
  m_typeChoice = NULL;
  m_cancelButton = NULL;
  m_saveButton = NULL;
  m_hSizer1 = NULL;
  m_hSizer2 = NULL;
  m_hSizer3 = NULL;
  m_vSizer = NULL;

  //m_menuView = NULL;
}
//----------------------------------------
void CTypedSaveFileDialog::InfoToArray(wxArrayString& array)
{
  CStringMap::iterator it;

  for (it = m_info.begin() ; it != m_info.end() ; it++)
  {
    array.Add(it->second.c_str());
  }

}
//----------------------------------------
bool CTypedSaveFileDialog::CreateControls()
{
  m_ID_LOCFIELD = ::wxNewId();
  m_ID_BROWSE = ::wxNewId();
  m_ID_TYPECHOICE = ::wxNewId();

  m_locLabel = new wxStaticText(this, -1, "&Location",
                                wxDefaultPosition,
                                this->ConvertDialogToPixels(wxSize(-1, -1)),
                                wxALIGN_RIGHT);

  m_locField = new wxTextCtrl(this, m_ID_LOCFIELD,
                              "",
                              wxDefaultPosition,
                              this->ConvertDialogToPixels(wxSize(150, -1)),
                              wxTE_PROCESS_ENTER);

  m_browseButton = new wxButton(this, m_ID_BROWSE, "Browse...");

  m_typeLabel = new wxStaticText(this, -1, "Determine File Type:",
                                wxDefaultPosition,
                                this->ConvertDialogToPixels(wxSize(-1, -1)),
                                wxALIGN_RIGHT);

  wxArrayString array;
  InfoToArray(array);

  m_typeChoice = new wxChoice(this, m_ID_TYPECHOICE,
                              wxDefaultPosition,
                              wxDefaultSize,
                              array);

  m_cancelButton = new wxButton(this, wxID_CANCEL, "Cancel");
  m_saveButton = new wxButton(this, wxID_OK, "Save");

  m_saveButton->Enable(false);
  m_typeChoice->Enable(m_info.size() > 0);

  wxString locDefault;
  CConfig *config = CConfig::GetInstance();
  if ( !config->GetEntry(CONFIG_SAVE_LOCATION, &locDefault) )
  {
     locDefault = m_currentDir.GetFullPath();
      if ( (locDefault.IsEmpty() == false) && (locDefault[locDefault.Length() - 1] != wxFileName::GetPathSeparator()) )
      {
        locDefault.Append(wxFileName::GetPathSeparator());
      }
  }
  config->SetEntry(CONFIG_SAVE_LOCATION, locDefault);


  m_locField->SetValue(locDefault);
  m_locField->SetInsertionPointEnd();
  SetDefaultItem(m_locField);
  m_typeChoice->SetSelection(0);
  m_locField->SetFocus();

  return true;
}

//----------------------------------------
void CTypedSaveFileDialog::CreateLayout()
{

  m_hSizer1 = new wxBoxSizer(wxHORIZONTAL);
  m_hSizer1->Add(m_locField, 1, wxEXPAND|wxRIGHT, 10);
  m_hSizer1->Add(m_browseButton, 0, wxEXPAND);

  m_hSizer2 = new wxBoxSizer(wxHORIZONTAL);
  m_hSizer2->Add(m_typeLabel, 0, wxALIGN_CENTER|wxRIGHT, 10);
  m_hSizer2->Add(m_typeChoice, 0, wxALIGN_CENTER);

  m_hSizer3 = new wxBoxSizer(wxHORIZONTAL);
  m_hSizer3->Add(0, 0, 1, wxEXPAND);
  m_hSizer3->Add(m_saveButton, 0, wxEXPAND|wxRIGHT, 10);
  m_hSizer3->Add(m_cancelButton, 0, wxEXPAND);

  m_vSizer = new wxBoxSizer(wxVERTICAL);
  m_vSizer->Add(m_locLabel, 0, wxLEFT|wxTOP|wxRIGHT, 10);
  m_vSizer->Add(m_hSizer1, 0, wxEXPAND|wxALL, 10);
  m_vSizer->Add(m_hSizer2, 0, wxEXPAND|wxALL, 10);
  m_vSizer->Add(m_hSizer3, 0, wxEXPAND|wxLEFT|wxBOTTOM|wxRIGHT, 10);
  m_vSizer->Fit(this);

  SetAutoLayout(true);
  SetSizerAndFit(m_vSizer);
  CentreOnParent();

}
//----------------------------------------
void CTypedSaveFileDialog::InstallToolTips()
{
  m_locField->SetToolTip("The filename to save to.");
  m_browseButton->SetToolTip("Open a file selector dialog box.");
  m_typeChoice->SetToolTip("Available formats (this will auto-update filename if appropriate).");

}
//----------------------------------------
void CTypedSaveFileDialog::InstallEventListeners()
{
  m_cancelButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED,
                           (wxObjectEventFunction)
                           (wxEventFunction)
                           (wxCommandEventFunction)&CTypedSaveFileDialog::OnCancel,
                           NULL,
                           this);
  m_saveButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED,
                         (wxObjectEventFunction)
                         (wxEventFunction)
                         (wxCommandEventFunction)&CTypedSaveFileDialog::OnSaveEvent,
                         NULL,
                         this);
  m_browseButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED,
                         (wxObjectEventFunction)
                         (wxEventFunction)
                         (wxCommandEventFunction)&CTypedSaveFileDialog::OnBrowse,
                         NULL,
                         this);
  m_locField->Connect( wxEVT_COMMAND_TEXT_ENTER,
                         (wxObjectEventFunction)
                         (wxEventFunction)
                         (wxCommandEventFunction)&CTypedSaveFileDialog::OnLocFieldEnter,
                         NULL,
                         this);
  m_locField->Connect( wxEVT_COMMAND_TEXT_UPDATED,
                         (wxObjectEventFunction)
                         (wxEventFunction)
                         (wxCommandEventFunction)&CTypedSaveFileDialog::OnLocFieldChar,
                         NULL,
                         this);

  m_typeChoice->Connect( wxEVT_COMMAND_CHOICE_SELECTED,
                         (wxObjectEventFunction)
                         (wxEventFunction)
                         (wxCommandEventFunction)&CTypedSaveFileDialog::OnTypeChoice,
                         NULL,
                         this);
  this->Connect( wxEVT_CLOSE_WINDOW,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 (wxCloseEventFunction)&CTypedSaveFileDialog::OnCloseWindow,
                 NULL,
                 this);

}


// WDR: handler implementations for CTypedSaveFileDialog
//----------------------------------------
void CTypedSaveFileDialog::OnTypeChoice( wxCommandEvent &event )
{
  wxString typeString = m_typeChoice->GetStringSelection();
  if (typeString == BYEXTENSION)
  {
    return;
  }

  m_fileName.Assign(m_locField->GetValue());
  m_fileName.Normalize();

  if (m_fileName.GetName().IsEmpty())
  {
    return;
  }

  m_fileName.ClearExt();
  m_fileName.SetExt(m_info.IsValue((const char *)(typeString)).c_str());

  m_locField->SetValue(m_fileName.GetFullPath());
  m_locField->SetInsertionPointEnd();

}

//----------------------------------------
void CTypedSaveFileDialog::OnCloseWindow( wxCloseEvent &event )
{
  this->EndModal(wxID_CANCEL);
}
//----------------------------------------
void CTypedSaveFileDialog::OnCancel( wxCommandEvent &event )
{
  this->EndModal(wxID_CANCEL);
}
//----------------------------------------
void CTypedSaveFileDialog::OnLocFieldChar( wxCommandEvent &event )
{
  m_saveButton->Enable(m_locField->GetValue() != "");
}
//----------------------------------------
void CTypedSaveFileDialog::OnLocFieldEnter( wxCommandEvent &event )
{
  OnSave();

}
//----------------------------------------
void CTypedSaveFileDialog::OnSaveEvent( wxCommandEvent &event )
{
  OnSave();
}
//----------------------------------------
void CTypedSaveFileDialog::OnSave()
{
  wxString fileType = m_typeChoice->GetStringSelection();

  m_fileName.Assign(m_locField->GetValue());
  m_fileName.Normalize();

  //m_fileName = CTools::ExpandShellVar(m_fileName);
  //m_fileName = CTools::MakeCorrectPath(m_fileName);
  //m_fileName = CTools::AbsolutePath(m_fileName);
  //m_fileName = wxFileName::GetFullPath(m_fileName.c_str);


  bool extOk= true;

  if (fileType != BYEXTENSION)
  {
    m_fileName.SetExt(m_info.IsValue((const char *)(fileType)).c_str());
  }
  else
  {
    extOk = CtrlExt();
  }

  if (extOk == false)
  {
    ::wxMessageBox(CTools::Format("Filename error: '%s' has no extension or an invalid extension",
                                  (const char *)(m_fileName.GetFullPath())).c_str(),
                   "Save Error",
                   wxOK|wxICON_ERROR|wxSTAY_ON_TOP,
                   this);
    return;
  }

  if (m_fileName.FileExists())
  {
    int32_t overwrite = ::wxMessageBox(CTools::Format("File '%s' already exists. Do you want to overwrite it?",
                                                      (const char *)(m_fileName.GetFullPath())).c_str(),
                                       "Confirm Overwrite",
                                        wxOK|wxCANCEL|wxICON_EXCLAMATION|wxSTAY_ON_TOP,
                                        this);

    if (overwrite != wxOK)
    {
      return;
    }
  }

  wxString m_path = m_fileName.GetFullPath().BeforeLast(wxFileName::GetPathSeparator()) + wxFileName::GetPathSeparator();
  CConfig *config = CConfig::GetInstance();
  config->SetEntry(CONFIG_SAVE_LOCATION, m_path);

  config->Flush();


  EndModal(wxID_OK);


}
//----------------------------------------
bool CTypedSaveFileDialog::CtrlExt()
{
//  m_ext = m_fileName.GetExt();
  if ( m_fileName.HasExt() == false )
  {
    return false;
  }

  //m_ext.erase(m_ext.begin());
  if ( m_info.Exists((const char *)(m_fileName.GetExt().MakeLower())).empty() )
  {
    return false;
  }

  return true;
}
//----------------------------------------
void CTypedSaveFileDialog::OnBrowse( wxCommandEvent &event )
{
  m_fileName.Assign(m_locField->GetValue());
  m_fileName.Normalize();

  //std::string dir = CTools::DirName(m_locField->GetValue().c_str());
  if (m_fileName.GetPath().IsEmpty())
  {
    m_fileName = m_currentDir;
  }
/*
  dir = CTools::ExpandShellVar(dir);
  dir = CTools::MakeCorrectPath(dir);
  dir = CTools::AbsolutePath(dir);
*/
  m_fileName.Assign( wxFileSelector("Save As...",
                                     m_fileName.GetPath(),
                                     "",
                                     "",
                                     "*.*",
                                     wxFD_SAVE,
                                     this));

  m_fileName.Normalize();

  wxString typeString = m_typeChoice->GetStringSelection();

  if (typeString != BYEXTENSION)
  {
    m_fileName.ClearExt();
    m_fileName.SetExt(m_info.IsValue((const char *)(typeString)).c_str());
  }


  if (m_fileName.GetFullPath().IsEmpty() == false)
  {
    m_locField->SetValue(m_fileName.GetFullPath());
    m_locField->SetInsertionPointEnd();
    m_saveButton->SetFocus();
    m_currentDir.AssignDir(m_fileName.GetPath());
    m_currentDir.SetName("");
  }


}



