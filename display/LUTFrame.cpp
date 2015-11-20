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
    #pragma implementation "LUTFrame.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <wx/numdlg.h>
#include "BratDisplayApp.h"
#include "LUTFrame.h"
#include "Trace.h"
#include "Tools.h"
#include "Exception.h"
using namespace brathl;

const wxString GROUP_COLORTABLES = "COLORTABLES";

#define brathlFmtEntryColorTablesMacro(index) \
  wxString::Format(GROUP_COLORTABLES + "/File%d", index)

// WDR: class implementations

//----------------------------------------------------------------------------
// CLUTFrame
//----------------------------------------------------------------------------

// WDR: event table for CLUTFrame

BEGIN_EVENT_TABLE(CLUTFrame,wxFrame)
    EVT_MENU( ID_MENU_EDIT_SETFACET, CLUTFrame::OnSetFacets )
    EVT_MENU( ID_MENU_EDIT_SQRT, CLUTFrame::OnCurveSQRT )
    EVT_MENU( ID_MENU_EDIT_COS, CLUTFrame::OnCurveCosinus )
    EVT_MENU( ID_MENU_EDIT_LIN, CLUTFrame::OnCurveLinear )
    EVT_MENU( ID_MENU_FILE_SAVE, CLUTFrame::OnSave )
    EVT_MENU( ID_MENU_FILE_OPEN, CLUTFrame::OnOpen )
    EVT_MENU( ID_MENU_EDIT_CLEAR, CLUTFrame::OnReset )
    EVT_MENU( ID_MENU_FILE_CLOSE, CLUTFrame::OnClose )
    EVT_MENU( ID_MENU_MODE_STANDARD, CLUTFrame::OnModeStandard )
    EVT_MENU( ID_MENU_MODE_GRADIENT, CLUTFrame::OnModeGradient )
    EVT_MENU( ID_MENU_MODE_CUSTOM, CLUTFrame::OnModeCustom )
    EVT_MENU_RANGE( wxID_FILE1, wxID_FILE9, CLUTFrame::OnFileHistory )
    EVT_CLOSE( CLUTFrame::OnCloseWindow )

    EVT_UPDATE_UI( ID_MENU_MODE_STANDARD, CLUTFrame::OnModeUpdateUI )
    EVT_UPDATE_UI( ID_MENU_MODE_GRADIENT, CLUTFrame::OnModeUpdateUI )
    EVT_UPDATE_UI( ID_MENU_MODE_CUSTOM, CLUTFrame::OnModeUpdateUI )
    EVT_UPDATE_UI( ID_MENU_FILE_SAVE, CLUTFrame::OnUpdateUIFileSave )
END_EVENT_TABLE()

//----------------------------------------
CLUTFrame::CLUTFrame()
{
  Init();

}

//----------------------------------------
CLUTFrame::CLUTFrame( wxWindow *parent, wxWindowID id, const wxString &title, CBratLookupTable* lut, bool apply,
    const wxPoint &position, const wxSize& size, long style ) 
    //: wxFrame( parent, id, title, position, size, style ) --> called in Create
{
  Init();

  bool bOk = Create(parent, id, title, lut, apply, position, size, style);
  if (bOk == false)
  {
    CException e("ERROR in CLUTFrame ctor - Unenable to create wxWidgets (Create method returns false)", BRATHL_SYSTEM_ERROR);
    throw(e);
  }

}

//----------------------------------------
CLUTFrame::~CLUTFrame()
{
}

//----------------------------------------
void CLUTFrame::Init()
{
  m_apply = false;
  m_recentFiles = false;

  //????self.closingdown = False
  m_menuBar = NULL;
  m_lutPanel = NULL;

  m_menuFile = NULL;
  m_menuEdit = NULL;
  m_menuMode = NULL;
  m_menuHistory = NULL;


}

//----------------------------------------
bool CLUTFrame::Create( wxWindow *parent, wxWindowID id, const wxString &title, CBratLookupTable* lut, bool apply,
    const wxPoint &position, const wxSize& size, long style )
{
  bool bOk = true;

  wxFrame::Create(parent, id, title, position, size, style);
  Show(false);

  ///???self.prefs = Preferences.Preferences()
  m_apply = apply;

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

  //self.prefs = Preferences.Preferences()


  CreateMenuBar();
  CreateStatusBar();
  bOk = CreateControls(lut);
  if (bOk == false)
  {
    return false;
  }

  CreateLayout();
  InstallEventListeners();

  UpdateMenuMode();

  return true;

}

//----------------------------------------
void CLUTFrame::CreateMenuBar()
{
  m_menuBar = LUTFrameMenuBarFunc();
  
  SetMenuBar(m_menuBar);

  m_menuBar->Enable(ID_MENU_FILE_RECENT, false);
  
  wxMenuItem* menuItemRecent = m_menuBar->FindItem(ID_MENU_FILE_RECENT, &m_menuFile);
  if (menuItemRecent != NULL)
  {
    m_menuHistory = menuItemRecent->GetSubMenu();
  }

  m_menuBar->FindItem(ID_MENU_EDIT_CLEAR, &m_menuEdit);
  m_menuBar->FindItem(ID_MENU_MODE_STANDARD, &m_menuMode);

  /*
  m_menuFile = m_menuBar->GetMenu(0);
  if (m_menuFile != NULL)
  {
    menuItemRecent = m_menuFile->FindItem(ID_MENU_FILE_RECENT);
    if (menuItemRecent != NULL)
    {
      m_menuHistory = menuItemRecent->GetSubMenu();
    }
  }

*/
  if (m_menuHistory != NULL)
  {
    m_fileHistory.UseMenu(m_menuHistory);
    ConfigToFileHistory();

  }

  wxMenuItem* menuItemSave = m_menuBar->FindItem(ID_MENU_FILE_SAVE, &m_menuFile);
  menuItemSave->Enable(false);



}
//----------------------------------------
bool CLUTFrame::CreateControls(CBratLookupTable* lut)
{ 
  m_lutPanel = new CLUTPanel(this, lut, -1, m_apply);

  return true;
}

//----------------------------------------
void CLUTFrame::CreateLayout()
{
  m_sizer = new wxBoxSizer(wxVERTICAL);
  m_sizer->Add(m_lutPanel, 1, wxEXPAND);
  SetSizer(m_sizer);
  UpdateMinSize();

}
//----------------------------------------
void CLUTFrame::InstallEventListeners()
{

}
//----------------------------------------
void CLUTFrame::DeInstallEventListeners()
{
  if (m_lutPanel != NULL)
  {
    m_lutPanel->DeInstallEventListeners();
  }
}



//----------------------------------------
void CLUTFrame::UpdateMinSize()
{
  if (m_lutPanel == NULL)
  {
    return;
  }

  //Prevent redrawing of the window when we call SetClientSize()
  Freeze();

  // Reset the hard coded minimum size
  SetMinSize(wxSize(-1, -1));
  // This uses the sizer to calculate the minimal client size
  wxSize minClientSize = GetBestSize();
  // Turn the minimum client size to a minimum frame size
  wxSize prevSize = GetSize();
  SetClientSize(minClientSize);
  wxSize minSize  = GetSize();
  SetMinSize(minSize);
  Thaw();
  SetSize(prevSize);

}
//----------------------------------------
void CLUTFrame::PostLutChangedEvent(CBratLookupTable* lut)
{
  CLutChangedEvent  ev(GetId(), lut);
  wxPostEvent(GetParent(), ev);
}

//----------------------------------------
bool CLUTFrame::Destroy()
{
  return wxFrame::Destroy();
}

//----------------------------------------

void CLUTFrame::OnCloseWindow( wxCloseEvent &event )
{
  // if ! saved changes -> return
 
    //???    if self.closingdown:
    //???        return
    //???    self.closingdown = True
  DeInstallEventListeners();

  FileHistoryToConfig();

  Destroy();
  ////Hide();
    
}
//----------------------------------------
void CLUTFrame::OnFileHistory( wxCommandEvent &event )
{
  int32_t fileNum = event.GetId() - wxID_FILE1;
  LoadFromFileHistory(fileNum);
    
}

// WDR: handler implementations for CLUTFrame
//----------------------------------------

void CLUTFrame::OnUpdateUIFileSave( wxUpdateUIEvent &event )
{
  if ( m_lutPanel->IsStandardModifyPanelVisible() )
  {
    m_menuFile->Enable(ID_MENU_FILE_SAVE, false);
  }
  else
  {
    m_menuFile->Enable(ID_MENU_FILE_SAVE, true);
  }

}

void CLUTFrame::OnModeUpdateUI( wxUpdateUIEvent &event )
{
  //int which = (event.GetId() - ID_MENU_MODE_STANDARD + 1);

  if ( (m_lutPanel->IsStandardModifyPanelVisible()) && (event.GetId() == ID_MENU_MODE_STANDARD) )
  {
    event.Check(true);
  }
  else if ( (m_lutPanel->IsGradientModifyPanelVisible()) && (event.GetId() == ID_MENU_MODE_GRADIENT) )
  {
    event.Check(true);
  }
  else if ( (m_lutPanel->IsCustomModifyPanelVisible()) && (event.GetId() == ID_MENU_MODE_CUSTOM) )
  {
    event.Check(true);
  }

}

//----------------------------------------
void CLUTFrame::OnModeCustom( wxCommandEvent &event )
{

  int32_t answer = ::wxMessageBox("If you proceed the current lookup table will be cleared.\nDo you want to Proceed?",
                               "Proceed", wxYES_NO);
  if (answer == wxYES)
  {
    m_lutPanel->SetModeToCustom();
  }  
    
}

//----------------------------------------
void CLUTFrame::OnModeGradient( wxCommandEvent &event )
{
  int32_t answer = ::wxMessageBox("If you proceed the current lookup table will be cleared.\nDo you want to Proceed?",
                               "Proceed", wxYES_NO);
  if (answer == wxYES)
  {
    m_lutPanel->SetModeToGradient();
  }
        
}

//----------------------------------------
void CLUTFrame::OnModeStandard( wxCommandEvent &event )
{
  int32_t answer = ::wxMessageBox("If you proceed the current lookup table will be cleared.\nDo you want to Proceed?",
                               "Proceed", wxYES_NO);
  if (answer == wxYES)
  {
    m_lutPanel->SetModeToStandard();
  }
    
}

//----------------------------------------
void CLUTFrame::OnClose( wxCommandEvent &event )
{
  Close(true);        
}

//----------------------------------------
void CLUTFrame::OnReset( wxCommandEvent &event )
{
  m_lutPanel->ResetLUT();
    
}

//----------------------------------------
void CLUTFrame::OnOpen( wxCommandEvent &event )
{
  int32_t answer = ::wxMessageBox("If you load a color table, the current lookup table will be cleared.\nDo you want to proceed?",
                               "Load", wxYES_NO);
  if (answer != wxYES)
  {
   return;
  }
  m_fileName.Assign( wxFileSelector("Open Color Table File...",
                                   ".",
                                   m_fileName.GetFullPath(),
                                   "ct",
                                   "Color Tables|*.ct|All Files|*.*",
                                   wxFD_OPEN,
                                   this));
  
  Open();
}

//----------------------------------------
void CLUTFrame::Open()
{
  m_fileName.Normalize();
  
  if (m_fileName.GetName().IsEmpty() == true)
  {
    return;
  }

  bool bOk = false;
  
  try
  {
    bOk = m_lutPanel->GetLUT()->LoadFromFile(m_fileName.GetFullPath().ToStdString());
  }
  catch(CException& e)
  {
    ::wxMessageBox(e.what(), "Load error");
    
  }
  catch(...)
  {

    ::wxMessageBox(wxString::Format("Can't load %s", 
                                    m_fileName.GetFullPath().c_str()),
                   "Load error");
  }

  UpdateMode(!bOk);

  if (bOk)
  {
    AddFileToHistory(m_fileName.GetFullPath());
  }
  /*
          filename = wx.FileSelector("Open Color Table File",
                                   default_extension="lt",
                                   default_path=self.prefs.clutsavedir,
                                   wildcard="VISAN Color Table Files|*.lt|All Files|*.*",
                                   flags=wx.OPEN,
                                   parent=self)
        if (filename):
	    self.filename=os.path.basename(filename)
            self._Open(filename)
            self.prefs.clutsavedir = os.path.dirname(filename)


    def _Open(self, filename):
        lut = VisanLookupTable()
        try:
            lut.FromFile(filename)
        except Exception, e:
            wx.MessageDialog(parent=self,
                             message="Error loading Color Table from file '%s':\n\n%s"
                             % (filename, str(e)),
                             caption="Load Error",
                             style=wx.OK|wx.ICON_ERROR|wx.STAY_ON_TOP)
        else:
            self.UpdateMode(lut)

            self._AddFileToHistory(filename)

*/
}
//----------------------------------------
void CLUTFrame::OnSave( wxCommandEvent &event )
{
  //wxFileName fileName;
  m_fileName.Assign( wxFileSelector("Save Color Table...",
                                   ".",
                                   m_fileName.GetFullPath(),
                                   "ct",
                                   "Color Tables|*.ct|All Files|*.*",
                                   wxFD_SAVE | wxFD_OVERWRITE_PROMPT,
                                   this));
  
  m_fileName.Normalize();
  
  if (m_fileName.GetName().IsEmpty() == true)
  {
    return;
  }

  try
  {
    m_lutPanel->GetLUT()->SaveToFile(m_fileName.GetFullPath().ToStdString());
    AddFileToHistory(m_fileName.GetFullPath());
  }
  catch(...)
  {
    // do Nothing
  }

  /*
    filename = wx.FileSelector("Save Color Table",
                                   default_path=self.prefs.clutsavedir,
                                   default_filename=self.filename,
                                   default_extension="lt",
                                   wildcard="Color Tables|*.lt|All Files|*.*",
                                   flags=wx.SAVE | wx.OVERWRITE_PROMPT,
                                   parent=self)

        if (filename):
            try:
                self.clutPanel.GetLUT().SaveToFile(filename)
            except Exception, e:
                wx.MessageBox(parent=self,
                              message="Error saving Color Table to file '%s':\n\n%s"
                              % (filename, str(e)),
                              caption="Save Error",
                              style=wx.OK|wx.ICON_ERROR|wx.STAY_ON_TOP)
                return
        self.filename=os.path.basename(filename)
            self.prefs.clutsavedir = os.path.dirname(filename)
            self._AddFileToHistory(filename)

*/        
}

//----------------------------------------
void CLUTFrame::OnCurveLinear( wxCommandEvent &event )
{
  m_lutPanel->GetLUT()->SetCurveLinear();
  m_lutPanel->UpdateForCurveLinear();
  m_lutPanel->UpdateRender();
    
}

//----------------------------------------
void CLUTFrame::OnCurveCosinus( wxCommandEvent &event )
{
  m_lutPanel->GetLUT()->SetCurveCosinus();
  m_lutPanel->UpdateForCurveCosinus();
  m_lutPanel->UpdateRender();
    
}

//----------------------------------------
void CLUTFrame::OnCurveSQRT( wxCommandEvent &event )
{
  m_lutPanel->GetLUT()->SetCurveSQRT();
  m_lutPanel->UpdateForCurveSQRT();
  m_lutPanel->UpdateRender();
    
}

//----------------------------------------
void CLUTFrame::OnSetFacets( wxCommandEvent &event )
{
  int32_t n = ::wxGetNumberFromUser("Set the number of table values for this Color Table",
                           "Enter an integer:",
                           "Number of Table Values",
                           m_lutPanel->GetLookupTable()->GetNumberOfTableValues(),
                           1,
                           65536,
                           this);
  if (n < 0)
  {
    return;
  }

  m_lutPanel->GetLUT()->SetFacets(n);
  //m_lutPanel->SetLUT(m_lutPanel->GetLUT());
  m_lutPanel->UpdateForFacets(n);
  m_lutPanel->UpdateRender();
    
}
/*
//----------------------------------------
void CLUTFrame::SetLUT(CBratLookupTable* lut )
{
  UpdateMode(lut);    
}
*/


//----------------------------------------
void CLUTFrame::UpdateMode(bool updateLUTPanel)
{
  UpdateMenuMode();

  if (m_lutPanel->GetLUT()->GetCust()->size() > 0)  //is custom
  {
    m_lutPanel->SetModeToCustom(updateLUTPanel);
  }
  else if (m_lutPanel->GetLUT()->GetGrad()->size() > 0) 
  {
    m_lutPanel->SetModeToGradient(updateLUTPanel);
  }
  else if (m_lutPanel->GetLUT()->GetCurrentFunction().empty() == false)
  {
    m_lutPanel->SetModeToStandard(updateLUTPanel);
  }
  else
  {
    CException e("ERROR in CLUTFrame::UpdateModeFromPanel - Unexpected LUT mode encountered)", BRATHL_LOGIC_ERROR);
    throw(e);
  }


}
//----------------------------------------
void CLUTFrame::UpdateMenuMode()
{
  //m_lutPanel->SetLUT(lut);

  if (m_lutPanel->GetLUT()->GetCust()->size() > 0)  //is custom
  {
    m_menuMode->Check(ID_MENU_MODE_CUSTOM, true);
    //m_lutPanel->SetModeToCustom();
  }
  else if (m_lutPanel->GetLUT()->GetGrad()->size() > 0) 
  {
    m_menuMode->Check(ID_MENU_MODE_GRADIENT, true);
    //m_lutPanel->SetModeToGradient();
  }
  else if (m_lutPanel->GetLUT()->GetCurrentFunction().empty() == false)
  {
    m_menuMode->Check(ID_MENU_MODE_STANDARD, true);
    //m_lutPanel->SetModeToStandard();
  }
  else
  {
    CException e("ERROR in CLUTFrame::UpdateMenuMode - Unexpected LUT mode encountered)", BRATHL_LOGIC_ERROR);
    throw(e);
  }

  if (m_lutPanel->GetLUT()->GetCurve() == "Linear")
  {
    m_menuEdit->Check(ID_MENU_EDIT_LIN, true);
  }
  else if (m_lutPanel->GetLUT()->GetCurve() == "SQRT")
  {
    m_menuEdit->Check(ID_MENU_EDIT_SQRT, true);
  }
  else if (m_lutPanel->GetLUT()->GetCurve() == "Cosinus")
  {
    m_menuEdit->Check(ID_MENU_EDIT_COS, true);
  }
  else
  {
    std::string szMsg = CTools::Format("ERROR in CLUTFrame::UpdateMenuMode - Unexpected LUT curve encountered : '%s'",
                                  m_lutPanel->GetLUT()->GetCurve().c_str());
    CException e(szMsg, BRATHL_LOGIC_ERROR);
    throw(e);
  }


}
//----------------------------------------
void CLUTFrame::AddFileToHistory(const wxString& fileName)
{
  m_menuBar->Enable(ID_MENU_FILE_RECENT, true);
  m_fileHistory.AddFileToHistory(fileName);
}

//----------------------------------------
void CLUTFrame::LoadFromFileHistory(int32_t fileNum)
{
  m_fileName.Assign( m_fileHistory.GetHistoryFile(fileNum) );
  Open();
}
//----------------------------------------
void CLUTFrame::FileHistoryToConfig()
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

  for (uint32_t i =0 ; i < m_fileHistory.GetCount() ; i++)
  {
    wxFileName fileName;
    fileName.Assign(m_fileHistory.GetHistoryFile(i));
    fileName.Normalize();

    config->Write(brathlFmtEntryColorTablesMacro(i), fileName.GetFullPath() );
  }
  
  config->Flush();

}
//----------------------------------------
void CLUTFrame::ConfigToFileHistory()
{
  if (wxGetApp().GetConfig() == NULL)
  {
    return;
  }
  
  wxFileConfig* config = wxGetApp().GetConfig();

  config->SetPath("/" + GROUP_COLORTABLES);

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
      AddFileToHistory(valueString);
    }
  }
  while (bOk);

  m_menuBar->Enable(ID_MENU_FILE_RECENT, maxEntries > 0);

}
