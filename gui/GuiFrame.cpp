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
    #pragma implementation "GuiFrame.h"
#endif

// For compilers that support precompilation
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//#include "wx/mimetype.h"

// Include private headers
#include "BratGui.h"
#include "GuiFrame.h"
#include "DirTraverser.h"
#include "WorkspaceDlg.h"

// WDR: class implementations

//------------------------------------------------------------------------------
// CGuiFrame
//------------------------------------------------------------------------------

// WDR: event table for CGuiFrame

BEGIN_EVENT_TABLE(CGuiFrame,wxFrame)
    EVT_MENU(wxID_ABOUT, CGuiFrame::OnAbout)
    EVT_MENU(wxID_EXIT, CGuiFrame::OnQuit)
    EVT_CLOSE(CGuiFrame::OnCloseWindow)
//    EVT_SIZE(CGuiFrame::OnSize)
//    EVT_UPDATE_UI(-1,CGuiFrame::OnUpdateUI)
    EVT_MENU( ID_WSNEW, CGuiFrame::OnNewWorkspace )
    EVT_MENU( ID_WSOPEN, CGuiFrame::OnOpenWorkspace )
    EVT_MENU( ID_WSIMPORT, CGuiFrame::OnImportWorkspace )
    EVT_MENU( ID_WSRENAME, CGuiFrame::OnRenameWorkspace )
    EVT_MENU( ID_WSDELETE, CGuiFrame::OnDeleteWorkspace )
    EVT_UPDATE_UI( ID_WSRENAME, CGuiFrame::OnRenameWorkspaceUpdateUI )
    EVT_UPDATE_UI( ID_WSDELETE, CGuiFrame::OnDeleteWorkspaceUpdateUI )
    EVT_MENU( ID_WSSAVE, CGuiFrame::OnSaveWorkspace )
    EVT_UPDATE_UI( ID_WSSAVE, CGuiFrame::OnSaveWorkspaceUpdateUI )
    EVT_UPDATE_UI( ID_WSIMPORT, CGuiFrame::OnImportWorkspaceUpdateUI )
    EVT_MENU( ID_DSNEW, CGuiFrame::OnNewDataset )
    EVT_UPDATE_UI( ID_DSNEW, CGuiFrame::OnNewDatasetUpdateUI )
    EVT_MENU( ID_DSDELETE, CGuiFrame::OnDeleteDataset )
    EVT_UPDATE_UI( ID_DSDELETE, CGuiFrame::OnDeleteDatasetUpdateUI )
    EVT_MENU( ID_OPNEW, CGuiFrame::OnNewOperation )
    EVT_UPDATE_UI( ID_OPNEW, CGuiFrame::OnNewOperationUpdateUI )
    EVT_MENU( ID_OPDELETE, CGuiFrame::OnDeleteOperation )
    EVT_UPDATE_UI( ID_OPDELETE, CGuiFrame::OnDeleteOperationUpdateUI )
    EVT_MENU_RANGE( wxID_FILE1, wxID_FILE9, CGuiFrame::OnFileHistory )
    EVT_MENU( ID_VINEW, CGuiFrame::OnNewView )
    EVT_UPDATE_UI( ID_VINEW, CGuiFrame::OnNewViewUpdateUI )
    EVT_MENU( ID_VIDELETE, CGuiFrame::OnDeleteView )
    EVT_UPDATE_UI( ID_VIDELETE, CGuiFrame::OnDeleteViewUpdateUI )
    EVT_MENU( ID_OPDUPLICATE, CGuiFrame::OnDuplicateOperation )
    EVT_UPDATE_UI( ID_OPDUPLICATE, CGuiFrame::OnDuplicateOperationUpdateUI )
    EVT_MENU( ID_HELP_USER_MANUEL, CGuiFrame::OnUserManual )
    EVT_MENU( ID_FIELDSSORTMENU, CGuiFrame::OnSortFieldCtrl )
    EVT_MENU( ID_FIELDSSORTREVMENU, CGuiFrame::OnSortRevFieldCtrl )
    EVT_MENU( ID_OPSORTMENU, CGuiFrame::OnSortOpCtrl )
    EVT_MENU( ID_OPSORTREVMENU, CGuiFrame::OnSortRevOpCtrl )
    EVT_MENU( ID_DISPSORTMENU, CGuiFrame::OnSortDispTreeCtrl )
    EVT_MENU( ID_DISPSORTREVMENU, CGuiFrame::OnSortRevDispTreeCtrl )
    EVT_MENU( ID_ASXMENU, CGuiFrame::OnSetAsX )
    EVT_MENU( ID_ASYMENU, CGuiFrame::OnSetAsY )
    EVT_MENU( ID_ASDATAMENU, CGuiFrame::OnSetAsData )
    EVT_MENU( ID_ASSELECTMENU, CGuiFrame::OnInsertIntoSelect )
    EVT_MENU( ID_INSERTEXPRMENU, CGuiFrame::OnInsertEmptyExpression )
    EVT_MENU( ID_INSERTFIELDMENU, CGuiFrame::OnInsertField )
    EVT_MENU( ID_INSERTFCTMENU, CGuiFrame::OnInsertFunction )
    EVT_MENU( ID_INSERTFORMULAMENU, CGuiFrame::OnInsertFormula )
    EVT_MENU( ID_SAVEASFORMULAMENU, CGuiFrame::OnSaveAsFormula )
    EVT_MENU( ID_DELETEEXPRMENU, CGuiFrame::OnDeleteExpression )
    EVT_MENU( ID_DISPADDTOSEL, CGuiFrame::OnAddToSelected )
    EVT_MENU( ID_DISPEXPANDMENU, CGuiFrame::OnExpandAvailabledispDataTree )
    EVT_MENU( ID_RENAMEEXPRMENU, CGuiFrame::OnRenameExpression )
    EVT_MENU( ID_DISPCOLLAPSEMENU, CGuiFrame::OnCollapseAvailabledispDataTree )
    EVT_MENU( ID_FIELD_ATTR_CHANGE_UNIT, CGuiFrame::OnFieldAttrChangeUnit )
END_EVENT_TABLE()
//----------------------------------------

CGuiFrame::CGuiFrame( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxFrame( parent, id, title, position, size, style )
{
  m_menuBar = NULL;
  m_menuWorkspace = NULL;
  m_menuHistory = NULL;

  m_menuBarPopup = NULL;
  m_menuFieldsTreeCtrl = NULL;
  m_menuOperationTreeCtrl = NULL;
  m_menuDisplayTreeCtrl = NULL;

  CreateMainMenuBar();
  
  CreatePopupMenuBar();
  
  //CreateMainToolBar();
  
  CreateStatusBar(1);
    
  m_guiPanel = new CGuiPanel(this, -1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER );
  wxBoxSizer * sizer = new wxBoxSizer(wxVERTICAL);

  sizer->Add(m_guiPanel, 1, wxEXPAND);
  SetSizer(sizer);

  // insert main window here
  //MainFrameContentFunc(this);

  SetMinSize(GetBestSize());
  SetSize(GetBestSize());

  try
  {
    if (!wxGetApp().GetIconFile().IsEmpty())
    {
      SetIcon(wxIcon(wxGetApp().GetIconFile(), ::wxGetApp().GetIconType()));
    }
    else
    {
      ::wxMessageBox(wxString::Format("WARNING: Unable to find Brat icon file %s",
                                      ::wxGetApp().GetIconFileName().c_str())
                     , "BRAT WARNING");
    }
  }
  catch(...)
  {
    // Do nothing
  }

}

//----------------------------------------
CGuiFrame::~CGuiFrame()
{
}

//----------------------------------------
void CGuiFrame::CreateMainMenuBar()
{
  m_menuBar = MainMenuBarFunc();

  SetMenuBar( m_menuBar );

  m_menuBar->Enable(ID_MENU_FILE_RECENT, false);

  wxMenuItem* menuItemRecent = m_menuBar->FindItem(ID_MENU_FILE_RECENT, &m_menuWorkspace);
  if (menuItemRecent != NULL)
  {
    m_menuHistory = menuItemRecent->GetSubMenu();
  }

  if (m_menuHistory != NULL)
  {
    m_wksHistory.UseMenu(m_menuHistory);
    ConfigToFileHistory();

  }



}
//----------------------------------------
void CGuiFrame::CreatePopupMenuBar()
{
  m_menuBarPopup = PopupMenuFunc();
  m_menuFieldsTreeCtrl = m_menuBarPopup->GetMenu(0);
  
  m_menuOperationTreeCtrl = m_menuBarPopup->GetMenu(1);
  
  m_menuDisplayTreeCtrl = m_menuBarPopup->GetMenu(2);
}

//----------------------------------------
void CGuiFrame::CreateMainToolBar()
{
    wxToolBar *tb = CreateToolBar( wxTB_HORIZONTAL|wxNO_BORDER /* |wxTB_FLAT */ );
    
    MainToolBarFunc( tb );
}
//----------------------------------------
bool CGuiFrame::CreateWorkspace()
{

  bool bOk = false;

  bool cancel = AskToSave();

  if (cancel)
  {
    return true;
  }

  CWorkspaceDlg wksDlg (this, -1, "Create a new workspace...", 
                        CWorkspace::wksNew, wxGetApp().GetCurrentWorkspace(), ".");
  
  int32_t result = wksDlg.ShowModal();
  if (result != wxID_OK)
  {
    return true;
  }
  
  wxFileName path;
  wxString name = wksDlg.m_name;
  path.Assign(wksDlg.m_currentDir);
  path.AppendDir(wksDlg.m_name);

  
  m_guiPanel->GetDatasetPanel()->Reset();
  //m_guiPanel->GetOperationPanel()->Reset();
  m_guiPanel->GetOperationPanel()->Reset();
  //m_guiPanel->GetDisplayPanel()->Reset();
  m_guiPanel->GetDisplayPanel()->Reset();

  m_guiPanel->GetDatasetPanel()->ClearAll();
  //m_guiPanel->GetOperationPanel()->ClearAll();
  m_guiPanel->GetOperationPanel()->ClearAll();
  //m_guiPanel->GetDisplayPanel()->ClearAll();
  m_guiPanel->GetDisplayPanel()->ClearAll();
  
  CWorkspace* wks = new CWorkspace(name, path);
  
  wxGetApp().CreateTree(wks);
  wxGetApp().m_tree.SaveConfig();
  
  m_guiPanel->GetDatasetPanel()->EnableCtrl();
  //m_guiPanel->GetOperationPanel()->EnableCtrl();
  m_guiPanel->GetOperationPanel()->EnableCtrl();
  //m_guiPanel->GetDisplayPanel()->EnableCtrl();
  m_guiPanel->GetDisplayPanel()->EnableCtrl();

  if (wks != NULL)
  {
    AddWorkspaceToHistory(wks->GetPathName());
  }

  SetTitle();

  return true;
}

//----------------------------------------
void CGuiFrame::ConfigToFileHistory()
{
  if (wxGetApp().GetConfig() == NULL)
  {
    return;
  }
  
  wxFileConfig* config = wxGetApp().GetConfig();

  config->SetPath("/" + GROUP_WKS_RECENT);

  long maxEntries = config->GetNumberOfEntries();
  bool bOk = false;
  wxString entry;
  wxString valueString;
  long i = 0;

  do
  {
    bOk = config->GetNextEntry(entry, i);
    if (bOk)
    {
      valueString = config->Read(entry);
      AddWorkspaceToHistory(valueString);
    }
  }
  while (bOk);

  m_menuBar->Enable(ID_MENU_FILE_RECENT, maxEntries > 0);

}
//----------------------------------------
void CGuiFrame::AddWorkspaceToHistory(const wxString& name)
{
  m_menuBar->Enable(ID_MENU_FILE_RECENT, true);
  m_wksHistory.AddFileToHistory(name);
}

//----------------------------------------
void CGuiFrame::RemoveWorkspaceFromHistory(const wxString& name)
{
  for (uint32_t i = 0 ; i < m_wksHistory.GetCount() ; i++)
  {
    if (m_wksHistory.GetHistoryFile(i).CmpNoCase(name) == 0)
    {
      m_wksHistory.RemoveFileFromHistory(i);
      break;
    }
  }
}

//----------------------------------------
void CGuiFrame::LoadFromWorkspaceHistory(int32_t fileNum)
{
  bool bOk = OpenWorkspace(m_wksHistory.GetHistoryFile(fileNum));

  if (bOk == false)
  {
    m_wksHistory.RemoveFileFromHistory(fileNum);
  }
}
//----------------------------------------
void CGuiFrame::FileHistoryToConfig()
{
  if (wxGetApp().GetConfig() == NULL)
  {
    return;
  }
  
  wxFileConfig* config = wxGetApp().GetConfig();

  config->SetPath("/");
  if (m_menuBar->IsEnabled(ID_MENU_FILE_RECENT) == false)
  {
    return;
  }

  for (uint32_t i =0 ; i < m_wksHistory.GetCount() ; i++)
  {
    wxFileName fileName;
    fileName.AssignDir(m_wksHistory.GetHistoryFile(i));
    fileName.Normalize();

    config->Write(brathlFmtEntryRecentWksMacro(i), fileName.GetPath() );
  }
  
  config->Flush();

}
//----------------------------------------
void CGuiFrame::OnFileHistory( wxCommandEvent &event )
{
  int32_t fileNum = event.GetId() - wxID_FILE1;
  LoadFromWorkspaceHistory(fileNum);
    
}

//----------------------------------------
bool CGuiFrame::SaveWorkspace()
{
  bool bOk = false;
  wxGetApp().m_tree.SaveConfig();

  CWorkspace* wks = wxGetApp().GetCurrentWorkspace();

  if (wks != NULL)
  {
    AddWorkspaceToHistory(wks->GetPathName());
  }


  return true;
}
//----------------------------------------
bool CGuiFrame::OpenWorkspace()
{

  bool bOk = false;

  bool cancel = AskToSave();

  if (cancel)
  {
    return true;
  }

  CWorkspaceDlg wksDlg (this, -1, "Open a workspace...",
                        CWorkspace::wksOpen, wxGetApp().GetCurrentWorkspace(), ".");
  
  int32_t result = wksDlg.ShowModal();
  if (result != wxID_OK)
  {
    return true;
  }

  CWorkspace* wks = new CWorkspace(wksDlg.GetWksName()->GetValue(), wksDlg.GetWksLoc()->GetValue());

  wxGetApp().CreateTree(wks);

  bOk = wxGetApp().m_tree.LoadConfig();

  if (bOk == false)
  {
    DoEmptyWorkspace();
    return false;
  }

  ResetWorkspace();

  LoadWorkspace();  

  EnableCtrlWorkspace();


  if (wks != NULL)
  {
    AddWorkspaceToHistory(wks->GetPathName());
  }

  SetTitle();

  return true;
}

//----------------------------------------
bool CGuiFrame::OpenWorkspace(const wxString& path)
{
  if (path.IsEmpty())
  {
    DoEmptyWorkspace();
    return true;
  }


  bool cancel = AskToSave();

  if (cancel)
  {
    return true;
  }

  CWorkspaceDlg wksDlg (this, -1, "Open a workspace...",
                        CWorkspace::wksOpen, wxGetApp().GetCurrentWorkspace(), path);
  
  wksDlg.GetWksLoc()->SetValue(path);

  bool bOk = wksDlg.ValidateData();
  if (bOk == false)
  {
    return bOk;
  }

  CWorkspace* wks = new CWorkspace(wksDlg.GetWksName()->GetValue(), wksDlg.GetWksLoc()->GetValue());
 
  wxGetApp().CreateTree(wks);
  bOk = wxGetApp().m_tree.LoadConfig();
  
  if (bOk == false)
  {
    DoEmptyWorkspace();
    return false;
  }

  ResetWorkspace();

  LoadWorkspace();  

  EnableCtrlWorkspace();

  if (wks != NULL)
  {
    AddWorkspaceToHistory(wks->GetPathName());
  }
 
  SetTitle();

  return true;

}

//----------------------------------------
void CGuiFrame::LoadWorkspace()
{
  m_guiPanel->GetDatasetPanel()->LoadDataset();
  //m_guiPanel->GetOperationPanel()->LoadOperation();
  m_guiPanel->GetOperationPanel()->LoadOperation();
  //m_guiPanel->GetDisplayPanel()->LoadDisplay();
  m_guiPanel->GetDisplayPanel()->LoadDisplay();

}
//----------------------------------------
void CGuiFrame::EnableCtrlWorkspace()
{
  m_guiPanel->GetDatasetPanel()->EnableCtrl();
  //m_guiPanel->GetOperationPanel()->EnableCtrl();
  m_guiPanel->GetOperationPanel()->EnableCtrl();
  //m_guiPanel->GetDisplayPanel()->EnableCtrl();
  m_guiPanel->GetDisplayPanel()->EnableCtrl();

}
//----------------------------------------
void CGuiFrame::ResetWorkspace()
{
  m_guiPanel->GetDatasetPanel()->Reset();
  //m_guiPanel->GetOperationPanel()->Reset();
  m_guiPanel->GetOperationPanel()->Reset();
  //m_guiPanel->GetDisplayPanel()->Reset();
  m_guiPanel->GetDisplayPanel()->Reset();

  m_guiPanel->GetDatasetPanel()->ClearAll();
  //m_guiPanel->GetOperationPanel()->ClearAll();
  m_guiPanel->GetOperationPanel()->ClearAll();
  //m_guiPanel->GetDisplayPanel()->ClearAll();
  m_guiPanel->GetDisplayPanel()->ClearAll();

}
//----------------------------------------
void CGuiFrame::DoEmptyWorkspace()
{
  wxGetApp().m_tree.DeleteTree();

  ResetWorkspace();
  
  EnableCtrlWorkspace();

  SetTitle();
    
}
//----------------------------------------
bool CGuiFrame::ImportWorkspace()
{

  bool bOk = false;

  if (wxGetApp().GetCurrentWorkspace() == NULL)
  {
    return false;
  }

  wxGetApp().m_treeImport.DeleteTree();


  wxString wksPath = wxGetApp().GetCurrentWorkspace()->GetPathName();
  wxFileName currentWksPath;
  currentWksPath.AssignDir( wksPath );
  currentWksPath.MakeAbsolute();


  SaveWorkspace();    

  CWorkspaceDlg wksDlg (this, -1, "Import a workspace...",
                        CWorkspace::wksImport, wxGetApp().GetCurrentWorkspace(), ".");
  
  wxGetApp().m_tree.GetImportBitSet()->m_bitSet.reset();

  wxString wksPathToImport;
  wxArrayString formulasToImport;

  while (true)
  {
    int32_t result = wksDlg.ShowModal();
    if (result != wxID_OK)
    {
      bOk = false;
      break;
    }

    wksPathToImport = wksDlg.GetWksLoc()->GetValue();
    wxFileName importWksPath;
    importWksPath.AssignDir( wksPathToImport );
    importWksPath.MakeAbsolute();
  
    if (currentWksPath.GetPath().CmpNoCase(importWksPath.GetPath()) == 0)
    {
      wxMessageBox("You can't import a workspace from itself.",
                   "Warning",
                    wxOK | wxCENTRE | wxICON_EXCLAMATION);
      bOk = false;
    }
    else
    {

      bOk = true;
      break;
    }
  }

  if (!bOk)
  {
    return bOk;
  }

  wxGetApp().m_tree.GetImportBitSet()->m_bitSet.set(IMPORT_DATASET_INDEX, wksDlg.GetImportDataset()->GetValue());  
  wxGetApp().m_tree.GetImportBitSet()->m_bitSet.set(IMPORT_FORMULA_INDEX, wksDlg.GetImportFormulas()->GetValue());  
  wxGetApp().m_tree.GetImportBitSet()->m_bitSet.set(IMPORT_OPERATION_INDEX, wksDlg.GetImportOperations()->GetValue());  
  wxGetApp().m_tree.GetImportBitSet()->m_bitSet.set(IMPORT_DISPLAY_INDEX, wksDlg.GetImportViews()->GetValue());  


  CWorkspace* wks = new CWorkspace(wksDlg.GetWksName()->GetValue(), wksPathToImport, false);
  
  wxGetApp().m_treeImport.SetCtrlDatasetFiles( wxGetApp().m_tree.GetImportBitSet()->m_bitSet.test(IMPORT_DATASET_INDEX) );

  wxGetApp().CreateTree(wks, wxGetApp().m_treeImport);

  //-----------------------------------------
  wxGetApp().SetCurrentTree(&(wxGetApp().m_treeImport));
  //-----------------------------------------
  
  bOk = wxGetApp().m_treeImport.LoadConfig();
  
  if (bOk == false)
  {
    wxGetApp().m_treeImport.DeleteTree();
    wxGetApp().m_tree.DeleteTree();

    bool oldValue = wxGetApp().m_tree.GetCtrlDatasetFiles();
    wxGetApp().m_tree.SetCtrlDatasetFiles(false);

    OpenWorkspace(wksPath);

    wxGetApp().m_tree.SetCtrlDatasetFiles(oldValue);
    
    return bOk;
  }

  CWorkspaceOperation* wksOpImport = wxGetApp().GetCurrentWorkspaceOperation();

  //-----------------------------------------
  wxGetApp().SetCurrentTree(&(wxGetApp().m_tree));
  //-----------------------------------------
  
  // Retrieve formula to Import
  CWorkspaceFormula* wksFormula = wxGetApp().GetCurrentWorkspaceFormula();
  
  wksDlg.GetCheckedFormulas(wksFormula->GetFormulasToImport());

  
  bOk = wxGetApp().m_tree.Import(&(wxGetApp().m_treeImport));
  if (bOk)
  {    
    SaveWorkspace(); 
 
    CWorkspaceOperation* wksOp = wxGetApp().GetCurrentWorkspaceOperation();
    if ( (wksOp != NULL) && (wksOpImport != NULL) )
    {
      CDirTraverserCopyFile traverserCopyFile(wksOp->GetPath(), "nc");
      //path name given to wxDir is locked until wxDir object is deleted
      wxDir dir;

      dir.Open(wksOpImport->GetPathName());
      dir.Traverse(traverserCopyFile);
    }
 
  }

  wxGetApp().m_treeImport.DeleteTree();
  wxGetApp().m_tree.DeleteTree();

  bool oldValue = wxGetApp().m_tree.GetCtrlDatasetFiles();
  wxGetApp().m_tree.SetCtrlDatasetFiles(false);

  bOk = OpenWorkspace(wksPath);

  wxGetApp().m_tree.SetCtrlDatasetFiles(oldValue);

  return bOk;
}
//----------------------------------------
bool CGuiFrame::RenameWorkspace()
{
  bool bOk = true;
  CWorkspace* wks = wxGetApp().GetCurrentWorkspace();

  if (wks == NULL)
  {
    wxMessageBox("There is no current workspace opened",
                 "Warning",
                  wxOK | wxCENTRE | wxICON_EXCLAMATION);
    return true;
  }

  CWorkspaceDlg wksDlg (this, -1, "Rename a workspace...",
                        CWorkspace::wksRename, wks, ".");
  
  int32_t result = wksDlg.ShowModal();
  if (result != wxID_OK)
  {
    return true;
  }
  
  wks->SetName(wksDlg.GetWksName()->GetValue());
  wks->SaveConfig();

  return bOk;
}
//----------------------------------------
bool CGuiFrame::DeleteWorkspace()
{  
  bool bOk = true;

  CWorkspace* wks = wxGetApp().GetCurrentWorkspace();

  if (wks == NULL)
  {
    wxMessageBox("There is no current workspace opened",
                 "Warning",
                  wxOK | wxCENTRE | wxICON_EXCLAMATION);
    return true;
  }

  wxString msg = wxString::Format("Delete workspace '%s' ...", wks->GetName().c_str());

  CWorkspaceDlg wksDlg (this, -1, msg,
                        CWorkspace::wksDelete, wks, ".");
  
  int32_t result = wksDlg.ShowModal();
  if (result != wxID_OK)
  {
    return true;
  }
  msg = wxString::Format("WARNING - All files under '%s' will be deleted\nAre you sure ?", wks->GetPathName().c_str());
  result = wxMessageBox(msg,
                        "Warning",
                        wxYES_NO | wxCENTRE | wxICON_QUESTION);

  if (result != wxYES)
  {
    return true;
  }

  bOk = wks->Rmdir();

  RemoveWorkspaceFromHistory(wks->GetPathName());
  
  DoEmptyWorkspace();

  return bOk;
}
//----------------------------------------
bool CGuiFrame::AskToSave()
{
  CWorkspace* wks = wxGetApp().GetCurrentWorkspace();
  bool cancel = false;

  if (wks != NULL)
  {
    int32_t result = wxMessageBox("Do you want to save the current workspace ?\n"
                                   "Click Yes to save and continue\n"
                                   "Click No to continue without saving\n"
                                   "Click Cancel to cancel operation",
                                 "Question",
                                  wxYES_NO | wxCANCEL | wxCENTRE | wxICON_QUESTION);
    if (result == wxYES)
    {
      SaveWorkspace();    
    }

    cancel = (result == wxCANCEL);
  }

  return cancel;
}
//----------------------------------------
void CGuiFrame::DoSortFieldCtrl(bool reverse)
{
    wxTreeItemId item = GetFieldstreectrl()->GetSelection();

    if (!item)
    {
      return;
    }
    //CHECK_ITEM( item );

    if (!GetFieldstreectrl()->ItemHasChildren(item))
    {
      item = GetFieldstreectrl()->GetItemParent(item);
    }
    GetFieldstreectrl()->DoSortChildren(item, reverse);
}
//----------------------------------------
void CGuiFrame::DoSortDispTreeCtrl(bool reverse)
{
    wxTreeItemId item = GetDispavailtreectrl()->GetSelection();

    if (!item)
    {
      return;
    }
    //CHECK_ITEM( item );

    if (!GetDispavailtreectrl()->ItemHasChildren(item))
    {
      item = GetDispavailtreectrl()->GetItemParent(item);
    }
    GetDispavailtreectrl()->DoSortChildren(item, reverse);
}

//----------------------------------------
void CGuiFrame::DoSortOpCtrl(bool reverse)
{
    wxTreeItemId item = GetOperationtreectrl()->GetSelection();

    if (!item)
    {
      return;
    }
    //CHECK_ITEM( item );

    if (!GetOperationtreectrl()->ItemHasChildren(item))
    {
      item = GetOperationtreectrl()->GetItemParent(item);
    }
    GetOperationtreectrl()->DoSortChildren(item, reverse);

}
//----------------------------------------
// WDR: handler implementations for CGuiFrame

//----------------------------------------
void CGuiFrame::OnFieldAttrChangeUnit( wxCommandEvent &event )
{
  m_guiPanel->GetOperationPanel()->FieldAttrChangeUnit();            
}
//----------------------------------------
void CGuiFrame::OnRenameExpression( wxCommandEvent &event )
{
  m_guiPanel->GetOperationPanel()->RenameExpression();            
}

//----------------------------------------
void CGuiFrame::OnCollapseAvailabledispDataTree( wxCommandEvent &event )
{
  m_guiPanel->GetDisplayPanel()->GetDispavailtreectrl()->CollapseItemsRecurse();            
    
}
//----------------------------------------
void CGuiFrame::OnExpandAvailabledispDataTree( wxCommandEvent &event )
{
  m_guiPanel->GetDisplayPanel()->GetDispavailtreectrl()->ExpandItemsRecurse();            
}
//----------------------------------------
void CGuiFrame::OnAddToSelected( wxCommandEvent &event )
{
  m_guiPanel->GetDisplayPanel()->AddField(); 
}

//----------------------------------------
void CGuiFrame::OnDeleteExpression( wxCommandEvent &event )
{
  m_guiPanel->GetOperationPanel()->DeleteExpression();        
}

//----------------------------------------
void CGuiFrame::OnSaveAsFormula( wxCommandEvent &event )
{

  m_guiPanel->GetOperationPanel()->SaveCurrentFormulaAs();
    
}

//----------------------------------------
void CGuiFrame::OnInsertFormula( wxCommandEvent &event )
{
  m_guiPanel->GetOperationPanel()->InsertFormula();            
}

//----------------------------------------
void CGuiFrame::OnInsertFunction( wxCommandEvent &event )
{
  m_guiPanel->GetOperationPanel()->InsertFunction();                
}

//----------------------------------------
void CGuiFrame::OnInsertField( wxCommandEvent &event )
{
  m_guiPanel->GetOperationPanel()->InsertField();    

}

//----------------------------------------
void CGuiFrame::OnInsertEmptyExpression( wxCommandEvent &event )
{
  m_guiPanel->GetOperationPanel()->InsertEmptyExpression();       
}
//----------------------------------------

void CGuiFrame::OnInsertIntoSelect( wxCommandEvent &event )
{    
  CField* field = GetFieldstreectrl()->GetCurrentField();    
  if (field == NULL)
  {
    return;
  }

  GetOperationtreectrl()->Add(GetOperationtreectrl()->GetSelectRootId(), field);  
}
//----------------------------------------

void CGuiFrame::OnSetAsData( wxCommandEvent &event )
{    
  CField* field = GetFieldstreectrl()->GetCurrentField();    
  if (field == NULL)
  {
    return;
  }

  GetOperationtreectrl()->Add(GetOperationtreectrl()->GetDataRootId(), field);  
}
//----------------------------------------

void CGuiFrame::OnSetAsY( wxCommandEvent &event )
{
  CField* field = GetFieldstreectrl()->GetCurrentField();    
  if (field == NULL)
  {
    return;
  }

  GetOperationtreectrl()->Add(GetOperationtreectrl()->GetYRootId(), field);  
    
}
//----------------------------------------

void CGuiFrame::OnSetAsX( wxCommandEvent &event )
{
  CField* field = GetFieldstreectrl()->GetCurrentField();    
  if (field == NULL)
  {
    return;
  }

  GetOperationtreectrl()->Add(GetOperationtreectrl()->GetXRootId(), field);
  
}
//----------------------------------------

void CGuiFrame::OnSortRevDispTreeCtrl( wxCommandEvent &event )
{
  DoSortDispTreeCtrl(true);            
    
}
//----------------------------------------

void CGuiFrame::OnSortDispTreeCtrl( wxCommandEvent &event )
{
  DoSortDispTreeCtrl();            
}

//----------------------------------------
void CGuiFrame::OnSortRevOpCtrl( wxCommandEvent &event )
{
  DoSortOpCtrl(true);    
}

//----------------------------------------
void CGuiFrame::OnSortOpCtrl( wxCommandEvent &event )
{
  DoSortOpCtrl();        
}

//----------------------------------------
void CGuiFrame::OnSortRevFieldCtrl( wxCommandEvent &event )
{
  DoSortFieldCtrl(true);
}

//----------------------------------------
void CGuiFrame::OnSortFieldCtrl( wxCommandEvent &event )
{
  DoSortFieldCtrl();    
}

//----------------------------------------
void CGuiFrame::OnUserManual( wxCommandEvent &event )
{

  wxGetApp().ViewUserManual();

 }

//----------------------------------------
void CGuiFrame::OnDuplicateOperationUpdateUI( wxUpdateUIEvent &event )
{
  /*
  bool enable = wxGetApp().GetCurrentWorkspace() 
                && m_guiPanel->GetOperationPanel()->HasOperation(); 
*/
  bool enable = wxGetApp().GetCurrentWorkspace() 
                && m_guiPanel->GetOperationPanel()->HasOperation(); 

  event.Enable(enable);    
    
}

//----------------------------------------
void CGuiFrame::OnDuplicateOperation( wxCommandEvent &event )
{
  wxGetApp().GotoOperationPage();
  //m_guiPanel->GetOperationPanel()->DuplicateOperation();         
  m_guiPanel->GetOperationPanel()->DuplicateOperation();         
}

//----------------------------------------
void CGuiFrame::OnDeleteViewUpdateUI( wxUpdateUIEvent &event )
{
  event.Enable(wxGetApp().IsDeleteViewEnable());    
    
}

//----------------------------------------
void CGuiFrame::OnDeleteView( wxCommandEvent &event )
{
  wxGetApp().GotoDisplayPage();
  //m_guiPanel->GetDisplayPanel()->DeleteDisplay();     
  m_guiPanel->GetDisplayPanel()->DeleteDisplay();     
    
}

//----------------------------------------
void CGuiFrame::OnNewViewUpdateUI( wxUpdateUIEvent &event )
{
  event.Enable(wxGetApp().IsNewViewEnable() );            
}

//----------------------------------------
void CGuiFrame::OnNewView( wxCommandEvent &event )
{
  wxGetApp().GotoDisplayPage();
  //m_guiPanel->GetDisplayPanel()->NewDisplay();     
  m_guiPanel->GetDisplayPanel()->NewDisplay();     
}

//----------------------------------------
void CGuiFrame::OnDeleteOperationUpdateUI( wxUpdateUIEvent &event )
{
  event.Enable(wxGetApp().IsDeleteOperationEnable());    
}

//----------------------------------------
void CGuiFrame::OnDeleteOperation( wxCommandEvent &event )
{
  wxGetApp().GotoOperationPage();
  //m_guiPanel->GetOperationPanel()->DeleteOperation();     
  m_guiPanel->GetOperationPanel()->DeleteOperation();     
    
}

//----------------------------------------
void CGuiFrame::OnNewOperationUpdateUI( wxUpdateUIEvent &event )
{
  event.Enable( wxGetApp().IsNewOperationEnable() );        
}
//----------------------------------------
void CGuiFrame::OnNewOperation( wxCommandEvent &event )
{
  wxGetApp().GotoOperationPage();
  //m_guiPanel->GetOperationPanel()->NewOperation();     
  m_guiPanel->GetOperationPanel()->NewOperation();     
    
}

//----------------------------------------
void CGuiFrame::OnDeleteDatasetUpdateUI( wxUpdateUIEvent &event )
{
  event.Enable(wxGetApp().IsDeleteDatasetEnable());    
}

//----------------------------------------
void CGuiFrame::OnDeleteDataset( wxCommandEvent &event )
{
  wxGetApp().GotoDatasetPage();
  m_guiPanel->GetDatasetPanel()->DeleteDataset();         
}

//----------------------------------------
void CGuiFrame::OnNewDatasetUpdateUI( wxUpdateUIEvent &event )
{
  event.Enable(wxGetApp().IsNewDatasetEnable());          
}

//----------------------------------------
void CGuiFrame::OnNewDataset( wxCommandEvent &event )
{
  wxGetApp().GotoDatasetPage();
  m_guiPanel->GetDatasetPanel()->NewDataset();     
}
//----------------------------------------

void CGuiFrame::OnImportWorkspaceUpdateUI( wxUpdateUIEvent &event )
{
  event.Enable(wxGetApp().GetCurrentWorkspace() != NULL);        
}

//----------------------------------------
void CGuiFrame::OnSaveWorkspaceUpdateUI( wxUpdateUIEvent &event )
{
  event.Enable(wxGetApp().GetCurrentWorkspace() != NULL);        
}

//----------------------------------------
void CGuiFrame::OnDeleteWorkspaceUpdateUI( wxUpdateUIEvent &event )
{
  event.Enable(wxGetApp().GetCurrentWorkspace() != NULL);        
}

//----------------------------------------
void CGuiFrame::OnRenameWorkspaceUpdateUI( wxUpdateUIEvent &event )
{
  event.Enable(wxGetApp().GetCurrentWorkspace() != NULL);        
}

//----------------------------------------
void CGuiFrame::OnSaveWorkspace( wxCommandEvent &event )
{
  SaveWorkspace();    
}

//----------------------------------------
void CGuiFrame::OnDeleteWorkspace( wxCommandEvent &event )
{
  DeleteWorkspace(); 
}

//----------------------------------------
void CGuiFrame::OnRenameWorkspace( wxCommandEvent &event )
{
  RenameWorkspace(); 
}

//----------------------------------------
void CGuiFrame::OnImportWorkspace( wxCommandEvent &event )
{
  ImportWorkspace();
}

//----------------------------------------
void CGuiFrame::OnOpenWorkspace( wxCommandEvent &event )
{
  OpenWorkspace();     
}

//----------------------------------------
void CGuiFrame::OnNewWorkspace( wxCommandEvent &event )
{
  CreateWorkspace();    
}

//----------------------------------------
void CGuiFrame::OnAbout( wxCommandEvent &event )
{
    wxMessageDialog dialog( this, wxT("Welcome to BRAT ") wxT(BRAT_VERSION) wxT("\n(C)opyright CNES/ESA"),
        wxT("About BRAT"), wxOK|wxICON_INFORMATION );
    dialog.ShowModal();
}

//----------------------------------------
void CGuiFrame::OnQuit( wxCommandEvent &event )
{
     Close();
}

//----------------------------------------
void CGuiFrame::OnCloseWindow( wxCloseEvent &event )
{
  GetFieldstreectrl()->DeInstallEventListeners();
  
  bool cancel = AskToSave();

  if (cancel)
  {
    event.Veto();
    return;
  }

  FileHistoryToConfig();
  
  if (m_menuBarPopup != NULL)
  {
    delete m_menuBarPopup;
    m_menuBarPopup = NULL;
  }
  // if ! saved changes -> return
  Destroy();
}

//----------------------------------------
void CGuiFrame::OnSize( wxSizeEvent &event )
{
    event.Skip( TRUE );
}

//----------------------------------------
void CGuiFrame::OnUpdateUI( wxUpdateUIEvent &event )
{
    event.Skip( TRUE );
}

//----------------------------------------
void CGuiFrame::SetTitle()
{
  wxString title = BRATGUI_TITLE;
  CWorkspace* wks =wxGetApp().GetCurrentWorkspace();
  if (wks != NULL)
  {
    title += " - " + wks->GetName();
  }

  wxTopLevelWindow::SetTitle(title);
}
