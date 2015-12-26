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
#include "display/wxInterface.h"

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#include <wx/ffile.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/filename.h"
#include "wx/dir.h"

#include "List.h"
#include "new-gui/Common/tools/Exception.h"
using namespace brathl;

#include "Validators.h"

#include "BratGui.h"
#include "new-gui/brat/Workspaces/Workspace.h"
#include "GuiPanel.h"
#include "DatasetPanel.h"
#include "SelectionCriteriaDlg.h"
#include "RichTextFrame.h"


//----------------------------------------------------------------------------
// CDatasetDropTarget
//----------------------------------------------------------------------------

CDatasetDropTarget::CDatasetDropTarget(CDatasetPanel *pOwner)
{
  m_pOwner = pOwner;
}

//----------------------------------------

bool CDatasetDropTarget::OnDropFiles(wxCoord, wxCoord, const wxArrayString& fileNames)
{
  /*
    size_t nFiles = filenames.GetCount();
    wxString str;
    str.Printf( _T("%d files dropped"), (int)nFiles);
    m_pOwner->Append(str);
    for ( size_t n = 0; n < nFiles; n++ )
    {
        m_pOwner->Append(filenames[n]);
        if (wxFile::Exists(filenames[n]))
            m_pOwner->Append(wxT("  This file exists.") );
        else
            m_pOwner->Append(wxT("  This file doesn't exist.") );

    }
*/
  if (m_pOwner == NULL)
  {
    return true;
  }

  CWorkspaceDataset* wks = wxGetApp().GetCurrentWorkspaceDataset();

  if (wks == NULL)
  {
    return true;
  }

  if (!wks->HasDataset())
  {
    wxMessageBox("You have to create an empty dataset before adding files\n",
                   "Warning",
                    wxOK | wxICON_EXCLAMATION);
    return true;
  }


  wxArrayString dirs;
  wxArrayString paths;

  wxFileName file;

  int32_t count = fileNames.GetCount();

  for ( int32_t n = 0; n < count; n++ )
  {
    bool isDir = wxDirExists(fileNames[n]);

    if (isDir)
    {
      dirs.Add(fileNames[n]);
    }
    else
    {
      paths.Add(fileNames[n]);
    }

  }


  m_pOwner->AddDir(dirs);
  m_pOwner->AddFiles(paths);

  return true;
}


// WDR: class implementations
//----------------------------------------------------------------------------
// CDatasetPanel
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(CDatasetPanel,wxSplitterWindow)

// WDR: event table for CDatasetPanel

BEGIN_EVENT_TABLE(CDatasetPanel,wxSplitterWindow)
    EVT_BUTTON( ID_DSADDFILES, CDatasetPanel::OnAddFiles )
    EVT_COMBOBOX( ID_DSNAMES, CDatasetPanel::OnComboBox )
    EVT_TEXT( ID_DSNAMES, CDatasetPanel::OnComboText )
    EVT_TEXT_ENTER( ID_DSNAMES, CDatasetPanel::OnComboTextEnter )
    EVT_WINDOW_CREATE(CDatasetPanel::OnWindowCreate)
    EVT_BUTTON( ID_DSUP, CDatasetPanel::OnFileUp )
    EVT_BUTTON( ID_DSDOWN, CDatasetPanel::OnFileDown )
    EVT_BUTTON( ID_DSSORT, CDatasetPanel::OnFileSort )
    EVT_BUTTON( ID_DSCLEAR, CDatasetPanel::OnFileClear )
    EVT_BUTTON( ID_DSCHECK, CDatasetPanel::OnCheckFiles )
    EVT_LISTBOX( ID_DSFILELIST, CDatasetPanel::OnListBox )
    //EVT_LIST_ITEM_SELECTED( ID_DICTLIST, CDatasetPanel::OnFieldSelected )
    //EVT_LIST_ITEM_DESELECTED( ID_DICTLIST, CDatasetPanel::OnFieldDeselected )
    EVT_BUTTON( ID_DSADDDIR, CDatasetPanel::OnAddDir )
    EVT_BUTTON( ID_DSREMOVE, CDatasetPanel::OnFileRemove )
    EVT_BUTTON( ID_DSNEW, CDatasetPanel::OnNewDataset )
    EVT_BUTTON( ID_DSDELETE, CDatasetPanel::OnDeleteDataset )
    EVT_CHECKBOX( ID_DSAPPLYCRIT, CDatasetPanel::OnApplyCriteria )
    EVT_BUTTON( ID_DSDEFINECRIT, CDatasetPanel::OnDefineCriteria )
    EVT_BUTTON( ID_DSSHOWREPORT, CDatasetPanel::OnShowSelectionReport )
END_EVENT_TABLE()
//----------------------------------------

CDatasetPanel::CDatasetPanel( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxSplitterWindow( parent, id, position, size, style )
{

  m_product = NULL;
  m_leftPanel = NULL;
  m_rightPanel = NULL;

  m_logFileName.AssignDir(wxGetApp().GetExecPathName());
  m_logFileName.SetFullName(DATASET_SELECTION_LOG_FILENAME);
  m_logFileName.Normalize();

  Reset();

  wxBoxSizer *item0 = new wxBoxSizer( wxHORIZONTAL );


  m_leftPanel = new wxPanel( this, -1 );
  //----------------------------
  wxSizer* sizer = DatasetListPanel( m_leftPanel, false, true );
  //----------------------------
  m_leftPanel->SetMinSize(m_leftPanel->GetBestSize());
  m_leftPanel->SetSize(m_leftPanel->GetBestSize());

  m_rightPanel = new wxPanel( this, -1 );
  //----------------------------
  sizer = DatasetDescCtrl( m_rightPanel, false, true );
  //----------------------------
  m_rightPanel->SetMinSize(m_rightPanel->GetBestSize());
  m_rightPanel->SetSize(m_rightPanel->GetBestSize());

  SplitVertically( m_leftPanel, m_rightPanel, m_leftPanel->GetBestSize().x);
  item0->Add( this, 1, wxGROW|wxALL, 5 );

  parent->SetSizer( item0 );

  CTextLightValidator textLightValidator;

  GetDsnames()->SetValidator(textLightValidator);

  CBratGuiApp::EvtFocus(*GetDsnames(), wxEVT_KILL_FOCUS,
                          (wxFocusEventFunction)&CDatasetPanel::OnComboKillFocus,
                          NULL,
                          this);

  CBratGuiApp::EvtChar(*GetDsnames(), wxEVT_CHAR,
                          (wxCharEventFunction)&CDatasetPanel::OnComboChar,
                          NULL,
                          this);
  CBratGuiApp::EvtChar(*GetDsfilelist(), wxEVT_CHAR,
                          (wxCharEventFunction)&CDatasetPanel::OnListChar,
                          NULL,
                          this);


  SetMinimumPaneSize( 150 );

  GetDictlist()->SetDescCtrl(GetFielddesc());

#if defined(__WXMAC__)
  GetDsfilelist()->SetDropTarget(new CDatasetDropTarget(this));
#else
  this->SetDropTarget(new CDatasetDropTarget(this));
#endif


  //SetSashPosition(300);
  EnableCtrl();
}
//----------------------------------------

CDatasetPanel::~CDatasetPanel()
{
  DeleteProduct();
}
//----------------------------------------
void CDatasetPanel::Reset()
{
  m_currentDataset = -1;
  m_currentFile = -1;
  m_dataset = NULL;
  DeleteProduct();
  m_revertSortFile = false;
  m_dsNameDirty = false;

}
//----------------------------------------
void CDatasetPanel::DeleteProduct()
{
  if (m_product != NULL)
  {
    delete m_product;
    m_product = NULL;
  }

}

//----------------------------------------
void CDatasetPanel::OnWindowCreate(wxWindowCreateEvent& evt)
{
    UNUSED(evt);
}
//----------------------------------------
CDataset* CDatasetPanel::GetCurrentDataset()
{
  CWorkspaceDataset* wks = wxGetApp().GetCurrentWorkspaceDataset();
  if (wks == NULL)
  {
    return NULL;
  }

  return wks->GetDataset(GetDsnames()->GetString(m_currentDataset).ToStdString());
}
//----------------------------------------
bool CDatasetPanel::SetCurrentDataset()
{
  m_dataset = GetCurrentDataset();
  return (m_dataset!= NULL);
}
//----------------------------------------
void CDatasetPanel::EnableCtrl()
{
  bool enable = false;
  bool enableNew = false;
  bool enableDelete = false;

  CWorkspaceDataset* wks = wxGetApp().GetCurrentWorkspaceDataset();
  if (wks != NULL)
  {
    enable = wks->HasDataset();
  }

  enableNew = wxGetApp().IsNewDatasetEnable();
  enableDelete = wxGetApp().IsDeleteDatasetEnable();

  GetDsnew()->Enable(enableNew);
  GetDsdelete()->Enable(enableDelete);

  GetDsnames()->Enable(enable);
  GetDsfilelist()->Enable(enable);
  GetDsaddfiles()->Enable(enable);
  GetDsadddir()->Enable(enable);
  GetDscheck()->Enable(enable);
  GetDsup()->Enable(enable);
  GetDsdown()->Enable(enable);
  GetDssort()->Enable(enable);
  GetDsclear()->Enable(enable);



}
//----------------------------------------
void CDatasetPanel::ClearDict()
{
  if (m_dataset != NULL)
  {
    //m_dataset->DeleteProduct();
    DeleteProduct();
  }
  GetProductLabel()->SetLabel("");
  //GetDictlist()->DeleteAllItems();
  GetDictlist()->InsertProduct(NULL);
  //GetFielddesc()->SetValue("");

  GetFiledescLabel()->Enable(false);
  GetFiledesc()->Enable(false);
  GetFiledesc()->SetValue("");
}
//----------------------------------------
void CDatasetPanel::FillDict()
{
  CDataset* dataset = GetCurrentDataset();
  //CProduct* product = NULL;

  if (dataset == NULL)
  {
    return;
  }

  this->SetCursor(*wxHOURGLASS_CURSOR);

  ClearDict();

  try
  {
    DeleteProduct();
    m_product = dataset->SetProduct(m_currentFilename.ToStdString());
    GetDictlist()->InsertProduct(m_product);
  }
  catch (CException& e)
  {
    wxMessageBox(wxString::Format("Unable to process files.\nReason:\n%s",
                                  e.what()),
                "Warning",
                wxOK | wxICON_EXCLAMATION);

    this->SetCursor(wxNullCursor);

    return;
  }
  if (m_product != NULL)
  {
    GetProductLabel()->SetLabel(wxString::Format("Product : %s / %s",
                                                 m_product->GetProductClass().c_str(),
                                                 m_product->GetProductType().c_str()));
    GetFiledescLabel()->Enable(m_product->IsNetCdf());
    GetFiledesc()->Enable(m_product->IsNetCdf());
    GetFiledesc()->SetValue(m_product->GetDescription().c_str());
  }

  //dataset->CloseProduct();
  m_product->Close();
  this->SetCursor(wxNullCursor);

}

//----------------------------------------
void CDatasetPanel::DsNameChanged()
{
  if (m_dsNameDirty == false)
  {
    return;
  }

  if (m_currentDataset < 0)
  {
    return;
  }

  CWorkspaceDataset* wks = wxGetApp().GetCurrentWorkspaceDataset();
  if (wks == NULL)
  {
    return;
  }

  CDataset* dataset = GetCurrentDataset();

  if (dataset == NULL)
  {
    return;
  }


  wxString dsName = GetDsnames()->GetValue();

  if (dsName.IsEmpty())
  {
    dsName = dataset->GetName();
    GetDsnames()->SetStringSelection(dsName);
    //GetDsnames()->SetValue(dsName);
    m_dsNameDirty = false;
    return;
  }

  wxString datasetOldName = dataset->GetName();

  if (wks->RenameDataset(dataset, dsName.ToStdString()) == false)
  {
    m_dsNameDirty = false; // must be here, to avoid loop, because  on wxMessageBox, combo kill focus event is process again

    wxMessageBox(wxString::Format("Unable to rename dataset '%s' by'%s'\nPerhaps dataset already exists, ",
                                  datasetOldName.c_str(),
                                  dsName.c_str()),
                "Warning",
                wxOK | wxICON_EXCLAMATION);
    GetDsnames()->SetSelection(m_currentDataset);
    return;
  }

  GetDsnames()->Delete(m_currentDataset);

  m_currentDataset = GetDsnames()->Append(dsName);
  GetDsnames()->SetSelection(m_currentDataset);

  CRenameDatasetEvent ev(GetId(), datasetOldName, dsName);
  wxPostEvent(GetParent(), ev);

  m_dsNameDirty = false;
}
//----------------------------------------
void CDatasetPanel::RemoveFileFromList(int32_t pos)
{
  if (pos < 0)
  {
    return;
  }

  /*
  if (GetDsfilelist()->GetCount() == 1)
  {
    bool bOk = CheckForEmptyDataset();
    if (!bOk)
    {
      return;
    }
  }
  */

  GetDsfilelist()->Delete(pos);
  m_currentFile = -1;
  m_currentFilename.Empty();

  ClearDict();

  CheckFiles();

  PostDatasetFilesChangeEvent();

}

/*
//----------------------------------------
void CDatasetPanel::ShowFieldDescription()
{
  int32_t selCount = GetDictlist()->GetSelectedItemCount();

  wxString desc;
  long item = GetDictlist()->GetNextItem(-1, wxLIST_NEXT_ALL,
                              wxLIST_STATE_SELECTED);
  while ( item != -1 )
  {
    CField* field = (CField*)(GetDictlist()->GetItemData(item));
    desc += wxString::Format("%s:\n---------------------------\n%s\n\n",
                             field->GetName().c_str(),
                             field->GetDescription().c_str());
    item = GetDictlist()->GetNextItem(item, wxLIST_NEXT_ALL,
                            wxLIST_STATE_SELECTED);
  }

  GetFielddesc()->SetValue(desc);
}

  */
//----------------------------------------
// WDR: handler implementations for CDatasetPanel
//----------------------------------------

//----------------------------------------
void CDatasetPanel::OnShowSelectionReport( wxCommandEvent &event )
{
    UNUSED(event);

  if (!m_logFileName.FileExists())
  {
    wxMessageBox("There is no report to show.",
                "Warning",
                wxOK | wxICON_EXCLAMATION);
    return;
  }
  wxString title = wxString::Format("%s", m_logFileName.GetFullPath().c_str());
  CRichTextFrame* frame = new CRichTextFrame(this, title);

  //frame->GetTextCtrl()->LoadFile(m_logFileName.GetFullPath());

  wxFFile wxffile(m_logFileName.GetFullPath());

  wxString content;
  wxffile.ReadAll(&content);
  frame->GetTextCtrl()->SetValue(content);

  frame->Show(true);
  frame->GetTextCtrl()->SetInsertionPoint(0);
  frame->GetTextCtrl()->ShowPosition(0);


}

//----------------------------------------
void CDatasetPanel::OnDefineCriteria( wxCommandEvent &event )
{
    UNUSED(event);

  wxString title = "Define selection criteria...";


  CSelectionCriteriaDlg dlg (this, -1, title);


  int32_t result = dlg.ShowModal();

  if (result != wxID_OK)
  {
    return;
  }

}

//----------------------------------------
void CDatasetPanel::OnApplyCriteria( wxCommandEvent &event )
{
    UNUSED(event);
}
//----------------------------------------

void CDatasetPanel::OnDeleteDataset( wxCommandEvent &event )
{
    UNUSED(event);

  DeleteDataset();
}
//----------------------------------------

void CDatasetPanel::OnNewDataset( wxCommandEvent &event )
{
    UNUSED(event);

  NewDataset();
}

//----------------------------------------
void CDatasetPanel::OnFileRemove( wxCommandEvent &event )
{
    UNUSED(event);

  int32_t pos = GetDsfilelist()->GetSelection();
  RemoveFileFromList(pos);

}


//----------------------------------------
/*
void CDatasetPanel::OnFieldDeselected( wxListEvent &event )
{
  ShowFieldDescription();
}
*/
//----------------------------------------
/*
void CDatasetPanel::OnFieldSelected( wxListEvent &event )
{
  ShowFieldDescription();
}
*/
//----------------------------------------
void CDatasetPanel::OnListBox( wxCommandEvent &event )
{
  m_currentFile = event.GetInt();

  if (m_currentFile < 0)
  {
    m_currentFilename.Empty();
    ClearDict();
  }
  else
  {
    m_currentFilename = GetDsfilelist()->GetString(m_currentFile);
    FillDict();
  }

}

//----------------------------------------
void CDatasetPanel::OnCheckFiles( wxCommandEvent &event )
{
    UNUSED(event);

    CheckFiles();
}

//----------------------------------------
void CDatasetPanel::OnFileClear( wxCommandEvent &event )
{
    UNUSED(event);

  if (m_dataset == NULL)
  {
    return;
  }
  int32_t result = wxMessageBox("Are you sure to remove all items of the list ?\nContinue ?",
                               "Warning",
                                wxYES_NO | wxCENTRE | wxICON_QUESTION);

  if (result != wxYES)
  {
    return;
  }


  /*
  bool bOk = CheckForEmptyDataset();

  if (!bOk)
  {
    return;
  }
  */

  m_currentFile = -1;
  m_currentFilename.Empty();

  GetDsfilelist()->Clear();
  ClearDict();

  //m_dataset->GetProductList()->RemoveAll();
  m_dataset->GetProductList()->clear();

  PostDatasetFilesChangeEvent();

  //CheckFiles();

}

//----------------------------------------
void CDatasetPanel::OnFileSort( wxCommandEvent &event )
{
    UNUSED(event);

  m_currentFilename = GetDsfilelist()->GetStringSelection();

  wxArrayString array;
  GetFiles(array);
  array.Sort(m_revertSortFile);

  GetDsfilelist()->Clear();
  GetDsfilelist()->Set(array);

  if (m_currentFilename.IsEmpty() == false)
  {
    GetDsfilelist()->SetStringSelection(m_currentFilename);
    m_currentFile = GetDsfilelist()->GetSelection();
  }
  else
  {
    m_currentFile = -1;
  }

  CheckFiles();
  m_revertSortFile = !m_revertSortFile;
}

//----------------------------------------
void CDatasetPanel::OnFileDown( wxCommandEvent &event )
{
    UNUSED(event);

  m_currentFile = GetDsfilelist()->GetSelection();

  if ((m_currentFile < 0) || (m_currentFile >= static_cast<int32_t>(GetDsfilelist()->GetCount()) - 1))
  {
    return;
  }

  m_currentFilename = GetDsfilelist()->GetStringSelection();

  GetDsfilelist()->Delete(m_currentFile);
  m_currentFile++;
  GetDsfilelist()->Insert(m_currentFilename, m_currentFile );
  GetDsfilelist()->SetSelection(m_currentFile );
  m_currentFilename = GetDsfilelist()->GetStringSelection();

  CheckFiles();

}

//----------------------------------------
void CDatasetPanel::OnFileUp( wxCommandEvent &event )
{
    UNUSED(event);

  int32_t m_currentFile = GetDsfilelist()->GetSelection();

  if (m_currentFile <= 0)
  {
    return;
  }

  m_currentFilename = GetDsfilelist()->GetStringSelection();

  GetDsfilelist()->Delete(m_currentFile);
  m_currentFile--;
  GetDsfilelist()->Insert(m_currentFilename, m_currentFile);
  GetDsfilelist()->SetSelection(m_currentFile);

  CheckFiles();

}
//----------------------------------------
void CDatasetPanel::OnListChar(wxKeyEvent &event )
{
  int32_t pos = GetDsfilelist()->GetSelection();

  if ((pos < 0) || (pos >= static_cast<int32_t>(GetDsfilelist()->GetCount())))
  {
    return;
  }

  int32_t code = event.GetKeyCode();

  if (( code == WXK_DELETE ) || ( code == WXK_NUMPAD_DELETE )  || ( code == WXK_NUMPAD_DECIMAL ))
  {
    RemoveFileFromList(pos);
  }

}

//----------------------------------------
void CDatasetPanel::OnComboChar(wxKeyEvent &event )
{
  /*
  if (event.GetKeyCode() == ' ')
  {
  ::wxBell();
    return;
  }
  */
  m_dsNameDirty = true;
  event.Skip();
}

//----------------------------------------
void CDatasetPanel::OnComboKillFocus( wxFocusEvent &event )
{
 DsNameChanged();
 event.Skip();

}

//----------------------------------------
void CDatasetPanel::OnComboText( wxCommandEvent &event )
{
    UNUSED(event);

  //m_dsNameDirty = true;
}
//----------------------------------------
void CDatasetPanel::OnComboTextEnter( wxCommandEvent &event )
{
    UNUSED(event);

  DsNameChanged();
}
//----------------------------------------

void CDatasetPanel::OnComboBox( wxCommandEvent &event )
{
  m_currentDataset = event.GetInt();
  SetCurrentDataset();
  FillFileList();
}
//----------------------------------------
void CDatasetPanel::OnAddDir( wxCommandEvent &event )
{
    UNUSED(event);

  int32_t style = wxDD_DEFAULT_STYLE;
  style &= ~wxDD_NEW_DIR_BUTTON;

//  wxDirDialog dirDlg(this, "Select a directory...", '.', style);
  wxDirDialog dirDlg(this, "Select a directory...", wxGetApp().GetLastDataPath(), style);

  if (dirDlg.ShowModal() != wxID_OK)
  {
    return;
  }

  wxString dirName = dirDlg.GetPath();
  AddDir(dirName);
/*
  wxFileName fileDir;
  fileDir.AssignDir( dirDlg.GetPath());
  fileDir.Normalize();

  wxGetApp().SetLastDataPath(fileDir.GetPath());



  if (fileDir.GetPath().IsEmpty())
  {
    return;
  }

  wxArrayString paths;

  wxDir dir;
  //dir.Open(fileDir);
  dir.GetAllFiles(fileDir.GetPath(), &paths);

  int32_t count = paths.GetCount();

  CStringList fileList;
  for ( int32_t n = 0; n < count; n++ )
  {
    fileList.InsertUnique(paths[n].c_str());
  }

  bool applySelectionCriteria = GetDsapplycrit()->GetValue();

  if (applySelectionCriteria)
  {
    CStringList filesOut;
    ApplySelectionCriteria(fileList, filesOut);

    fileList.clear();
    fileList.Insert(filesOut);
  }

  m_dataset->GetProductList()->InsertUnique(fileList);

  GetDsfilelist()->Clear();

  m_dataset->GetFiles(*GetDsfilelist());

  CheckFiles();

  PostDatasetFilesChangeEvent();
  */

}
//----------------------------------------
void CDatasetPanel::OnAddFiles( wxCommandEvent &event )
{
    UNUSED(event);

  //bool bOk = true;

  if (m_dataset == NULL)
  {
    return;
  }

  wxFileName currentDir;
  currentDir.AssignDir(wxGetApp().GetLastDataPath());
  currentDir.Normalize();

  wxString wildcards = wxString::Format(_T("All files (%s)|%s"),
                                        wxFileSelectorDefaultWildcardStr,
                                        wxFileSelectorDefaultWildcardStr
                                        );

  wxFileDialog dlg(this, "Select files ...", currentDir.GetPath(), wxEmptyString, wildcards, wxFD_MULTIPLE|wxFD_CHANGE_DIR|wxFD_FILE_MUST_EXIST);
  if (dlg.ShowModal() != wxID_OK)
  {
    return;
  }

  CWorkspaceDataset* wks = wxGetApp().GetCurrentWorkspaceDataset();
  if (wks == NULL)
  {
    return;
  }

  wxArrayString paths;
  dlg.GetPaths(paths);

  AddFiles(paths);

}
//----------------------------------------
void CDatasetPanel::AddDir( wxArrayString& dirNames )
{
  int32_t count = dirNames.GetCount();

  for ( int32_t n = 0; n < count; n++ )
  {
    AddDir(dirNames[n]);
  }

}
//----------------------------------------
void CDatasetPanel::AddDir( wxString& dirName )
{

  wxFileName fileDir;
  fileDir.AssignDir(dirName);
  fileDir.Normalize();

  wxGetApp().SetLastDataPath(fileDir.GetPath());



  if (fileDir.GetPath().IsEmpty())
  {
    return;
  }

  wxArrayString paths;

  wxDir dir;
  //dir.Open(fileDir);
  dir.GetAllFiles(fileDir.GetPath(), &paths);

  int32_t count = paths.GetCount();

  CStringList fileList;
  for ( int32_t n = 0; n < count; n++ )
  {
    fileList.InsertUnique((const char *)paths[n].c_str());
  }

  bool applySelectionCriteria = GetDsapplycrit()->GetValue();

  if (applySelectionCriteria)
  {
    CStringList filesOut;
    ApplySelectionCriteria(fileList, filesOut);

    fileList.clear();
    fileList.Insert(filesOut);
  }

  m_dataset->GetProductList()->InsertUnique(fileList);

  GetDsfilelist()->Clear();

  ::GetFiles(*m_dataset, *GetDsfilelist());

  CheckFiles();

  PostDatasetFilesChangeEvent();

}
//----------------------------------------
void CDatasetPanel::AddFiles( wxArrayString& paths )
{
  int32_t count = paths.GetCount();

  if (count <= 0)
  {
    return;
  }

  paths.Sort();

  wxFileName currentDir;

  currentDir.Assign(paths[0]);
  currentDir.Normalize();
  wxGetApp().SetLastDataPath(currentDir.GetPath());

  CStringList fileList;
  for ( int32_t n = 0; n < count; n++ )
  {
    fileList.InsertUnique((const char *)paths[n].c_str());

  }


  bool applySelectionCriteria = GetDsapplycrit()->GetValue();

  if (applySelectionCriteria)
  {
    CStringList filesOut;
    ApplySelectionCriteria(fileList, filesOut);

    fileList.clear();
    fileList.Insert(filesOut);
  }


  m_dataset->GetProductList()->InsertUnique(fileList);

  GetDsfilelist()->Clear();

  ::GetFiles( *m_dataset, *GetDsfilelist());

  CheckFiles();

  PostDatasetFilesChangeEvent();

  /*
  wxString dsName = GetDsnames()->GetValue();
  if (dsName.IsEmpty())
  {
    dsName = wks->GetDatasetNewName();
    //GetDsnames()->SetValue(dsName);
  }

  int32_t pos = -1;

  bOk = GetDsnames()->SetStringSelection(dsName);
  if (bOk == false)
  {
    pos = GetDsnames()->Append(dsName);
    GetDsnames()->SetSelection(pos);
  }
*/



}

//----------------------------------------
void CDatasetPanel::ApplySelectionCriteria(CStringList& filesIn, CStringList& filesOut)
{

  CProduct* p = NULL;
  bool bOk = true;

  try
  {
    p = CProduct::Construct(*(filesIn.begin()));
  }
  catch (CException e)
  {
    bOk = false;
  }
  catch (...)
  {
    bOk = false;
  }

  if (p == NULL)
  {
    bOk = false;
  }

  if (!bOk)
  {
    filesOut.Insert(filesIn);
    if (p != NULL)
    {
      delete p;
    }
    return;
  }

  this->SetCursor(*wxHOURGLASS_CURSOR);

  p->GetProductList().clear();
  p->GetProductList().Insert(filesIn);


  std::string label = p->GetLabel();
  CProduct* productRef = dynamic_cast<CProduct*>( CMapProduct::GetInstance().Exists(label) );

  if (productRef == NULL)
  {
    filesOut.Insert(filesIn);
    delete p;
    return;
  }


  p->AddCriteria(productRef);

  wxFileName logFileName;
  logFileName.AssignDir(wxGetApp().GetExecPathName());
  logFileName.SetFullName(DATASET_SELECTION_LOG_FILENAME);
  logFileName.Normalize();

  p->ApplyCriteria(filesOut,(const char *) m_logFileName.GetFullPath().c_str());

  delete p;
  p = NULL;

  this->SetCursor(wxNullCursor);


}
//----------------------------------------
void CDatasetPanel::LoadDataset()
{
  ClearAll();

  //bool bOk = true;

  const CWorkspaceDataset* wks = wxGetApp().GetCurrentWorkspaceDataset();
  if (wks == NULL)
  {
    return;
  }

  if (wks->GetDatasetCount() <= 0)
  {
    return;
  }

  GetDatasetNames(wks, *GetDsnames());

  m_currentDataset = 0;
  GetDsnames()->SetSelection(m_currentDataset);

  SetCurrentDataset();

  EnableCtrl();

  FillFileList();

}
//----------------------------------------
void CDatasetPanel::DeleteDataset()
{
  bool bOk = true;


  CWorkspaceDataset* wks = wxGetApp().GetCurrentWorkspaceDataset();
  if (wks == NULL)
  {
    return;
  }

  if (m_currentDataset < 0)
  {
    return;
  }

  //CDataset* dataset = GetCurrentDataset();

  if (m_dataset == NULL)
  {
    return;
  }

  int32_t result = wxMessageBox(wxString::Format("Are you sure to delete dataset '%s' ?\nContinue ?",
                                                  m_dataset->GetName().c_str()),
                               "Warning",
                                wxYES_NO | wxCENTRE | wxICON_QUESTION);

  if (result != wxYES)
  {
    return;
  }

  CStringArray operationNames;

  bOk = wxGetApp().CanDeleteDataset(m_dataset->GetName(), &operationNames);

  if (bOk == false)
  {
    std::string str = operationNames.ToString("\n", false);
    wxMessageBox(wxString::Format("Unable to delete dataset '%s'.\nIt is used by the operations below:\n%s\n",
                                  m_dataset->GetName().c_str(),
                                  str.c_str()),
                "Warning",
                wxOK | wxICON_EXCLAMATION);
    return;

  }

  wxString dsName = m_dataset->GetName();
  bOk = wks->DeleteDataset(m_dataset);

  if (bOk == false)
  {
    wxMessageBox(wxString::Format("Unable to delete dataset '%s'", m_dataset->GetName().c_str()),
                "Warning",
                wxOK | wxICON_EXCLAMATION);
    return;
  }

  m_dataset = NULL;

  GetDsnames()->Delete(m_currentDataset);

  m_currentDataset = (GetDsnames()->GetCount() > 0) ? 0 : -1;

  GetDsnames()->SetSelection(m_currentDataset);

  SetCurrentDataset();
  FillFileList();

  EnableCtrl();

  CDeleteDatasetEvent ev(GetId(), dsName);
  wxPostEvent(GetParent(), ev);


}
//----------------------------------------
void CDatasetPanel::NewDataset()
{
  bool bOk = true;

  CWorkspaceDataset* wks = wxGetApp().GetCurrentWorkspaceDataset();
  if (wks == NULL)
  {
    return;
  }

  GetDsnames()->Enable(true);
  //m_currentDataset = GetDsnames()->Append(dsName);
  ////GetDsnames()->Clear();
  ////////wks->GetDatasetNames(*GetDsnames());
  //int32_t i =    GetDsnames()->GetCount();

  wxString dsName = wks->GetDatasetNewName();

  bOk = wks->InsertDataset(dsName.ToStdString());

  if (bOk == false)
  {
    wxMessageBox(wxString::Format("Dataset '%s' already exists", dsName.c_str()),
                "Warning",
                wxOK | wxICON_EXCLAMATION);

    GetDsnames()->SetStringSelection(dsName);
    m_currentDataset = GetDsnames()->GetSelection();
  }
  else
  {
    m_currentDataset = GetDsnames()->Append(dsName);
    GetDsnames()->SetSelection(m_currentDataset);
  }

  SetCurrentDataset();
  GetDsfilelist()->Clear();

  ClearDict();


  EnableCtrl();

  CNewDatasetEvent ev(GetId(), dsName);
  wxPostEvent(GetParent(), ev);


}
//----------------------------------------
bool CDatasetPanel::CheckFiles()
{
  bool bOk = true;
  CStringArray array;

  wxString oldProductClass = m_dataset->GetProductList()->m_productClass.c_str();
  wxString oldProductType = m_dataset->GetProductList()->m_productType.c_str();

  m_dataset->GetProductList()->clear();

  GetFiles(array);

  if (array.size() <= 0)
  {
    return true;
  }

  m_dataset->GetProductList()->Insert(array);

  try
  {
    m_dataset->GetProductList()->CheckFiles();
    wxString msg = m_dataset->GetProductList()->GetMessage().c_str();
    if (!(msg.IsEmpty()))
    {
      wxMessageBox(msg,
                "Warning",
                wxOK | wxICON_INFORMATION);
    }

  }
  catch (CException& e)
  {
    bOk = false;
    wxMessageBox(wxString::Format("Unable to process files. Please apply correction\n\nReason:\n%s",
                                  e.what()),
                "Warning",
                wxOK | wxICON_EXCLAMATION);

  }

  if (bOk)
  {
    bool isSameProductClassType = ( oldProductClass.CmpNoCase(m_dataset->GetProductList()->m_productClass.c_str()) == 0 );
    isSameProductClassType &= ( oldProductType.CmpNoCase(m_dataset->GetProductList()->m_productType.c_str()) == 0 );

    CStringArray operationNames;

    bool usedByOperation = !(wxGetApp().CanDeleteDataset(m_dataset->GetName(), &operationNames));

    if ((!isSameProductClassType) && (usedByOperation))
    {
      std::string str = operationNames.ToString("\n", false);
      wxMessageBox(wxString::Format("Warning: Files contained in the dataset '%s' have been changed from '%s/%s' to '%s/%s' product class/type.\n"
                                    "\nThis dataset is used by the operations below:\n%s\n"
                                    "\nBe sure field's names used in these operations match the fields of the dataset files",
                                    m_dataset->GetName().c_str(),
                                    oldProductClass.c_str(), oldProductType.c_str(),
                                    m_dataset->GetProductList()->m_productClass.c_str(), m_dataset->GetProductList()->m_productType.c_str(),
                                    str.c_str()),
                  "Warning",
                  wxOK | wxICON_EXCLAMATION);
    }

  }


  return bOk;
}
//----------------------------------------
void CDatasetPanel::GetFiles( CStringArray& array )
{
  int32_t count = GetDsfilelist()->GetCount();

  for ( int32_t n = 0; n < count; n++ )
  {
    array.Insert((const char *)GetDsfilelist()->GetString(n).c_str());
  }

}
//----------------------------------------
void CDatasetPanel::GetFiles( wxArrayString& array )
{
  int32_t count = GetDsfilelist()->GetCount();
  for ( int32_t n = 0; n < count; n++ )
  {
    array.Add(GetDsfilelist()->GetString(n).c_str());
  }

}
//----------------------------------------
void CDatasetPanel::FillFileList()
{
	GetDsfilelist()->Clear();
	ClearDict();

	if ( m_dataset == NULL )
		return;

	wxArrayString array;

	::GetFiles( *m_dataset, array );
	GetDsfilelist()->Set( array );
}
//----------------------------------------
void CDatasetPanel::ClearAll()
{
  ClearDict();
  GetDsfilelist()->Clear();
  GetDsnames()->Clear();

  Reset();

  EnableCtrl();
}
//----------------------------------------
void CDatasetPanel::PostDatasetFilesChangeEvent()
{
  if (m_dataset == NULL)
  {
    return;
  }

  CDatasetFilesChangeEvent ev(GetId(), m_dataset->GetName(), *(m_dataset->GetProductList()));
  wxPostEvent(GetParent(), ev);
}

//----------------------------------------
bool CDatasetPanel::CheckForEmptyDataset()
{
  CStringArray operationNames;

  bool bOk = wxGetApp().CanDeleteDataset(m_dataset->GetName(), &operationNames);

  if (bOk == false)
  {
    std::string str = operationNames.ToString("\n", false);
    wxMessageBox(wxString::Format("Unable to remove all items of the list.\n"
                                  "\nThis dataset is used by the operations below:\n%s\n"
                                  "The dataset must contain at least one file.",
                                  m_dataset->GetName().c_str(),
                                  str.c_str()),
                "Warning",
                wxOK | wxICON_EXCLAMATION);
  }

  return bOk;

}
//----------------------------------------
