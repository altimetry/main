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
    #pragma implementation "ZFXYPlotFrame.h"
#endif


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "Trace.h"
#include "Tools.h"
#include "Exception.h"
using namespace brathl;


#include "vtkWindowToImageFilter.h"

#include "BratDisplay.h"
#include "TypedSaveFileDialog.h"
#include "MapImageType.h"

#include "ZFXYPlotFrame.h"

DEFINE_EVENT_TYPE(wxEVT_ZFXY_UPDATE_MINSIZE)

// WDR: class implementations

//----------------------------------------------------------------------------
// CZFXYProgressDialog
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(CZFXYProgressDialog,wxProgressDialog)

// WDR: event table for CZFXYProgressDialog

BEGIN_EVENT_TABLE(CZFXYProgressDialog,wxProgressDialog)
    EVT_TIMER(wxID_ANY, CZFXYProgressDialog::OnProgress)
    //EVT_WINDOW_DESTROY(CZFXYProgressDialog::OnDestroy)
END_EVENT_TABLE()

CZFXYProgressDialog::CZFXYProgressDialog(const wxString &title, wxString const &message,
                                int maximum,
                                wxWindow *parent,
                                int style)
                 : wxProgressDialog(title, message, maximum, parent, style)
{

  Connect(wxEVT_DESTROY,
           (wxObjectEventFunction)
           (wxEventFunction)
           (wxWindowDestroyEventFunction)&CZFXYProgressDialog::OnDestroy,
           NULL,
           this);

  m_count = 0;
  m_timer = new wxTimer(this);
  m_timer->Start(1000);
}
//----------------------------------------
CZFXYProgressDialog:: ~CZFXYProgressDialog()
{
}

//----------------------------------------
void CZFXYProgressDialog::OnProgress(wxTimerEvent& event)
{
  m_count++;
  this->Update(m_count);
}
//----------------------------------------
void CZFXYProgressDialog::OnDestroy( wxWindowDestroyEvent &event )
{
  m_timer->Stop();
  delete m_timer;
  m_timer = NULL;
}



// WDR: handler implementations for CZFXYProgressDialog


//----------------------------------------------------------------------------
// CZFXYPlotFrame
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(CZFXYPlotFrame,wxFrame)

// WDR: event table for CZFXYPlotFrame

BEGIN_EVENT_TABLE(CZFXYPlotFrame,wxFrame)
    EVT_MENU( ID_MENU_FILE_CLOSE, CZFXYPlotFrame::OnClose )
    EVT_MENU( ID_MENU_FILE_SAVE, CZFXYPlotFrame::OnSave )
    EVT_MENU( ID_MENU_VIEW_SLIDER, CZFXYPlotFrame::OnViewSlider )
    EVT_MENU( ID_MENU_VIEW_PROPS, CZFXYPlotFrame::OnViewProps )
    EVT_MENU( ID_MENU_VIEW_COLORBAR, CZFXYPlotFrame::OnViewColorbar )
    EVT_MENU( ID_MENU_VIEW_CLUTEDIT, CZFXYPlotFrame::OnShowColorTableEditor )
    EVT_CLOSE( CZFXYPlotFrame::OnCloseWindow )
    EVT_UPDATE_UI( ID_MENU_VIEW_SLIDER, CZFXYPlotFrame::OnUpdateUIAnimationToolbar )
    EVT_MENU( ID_MENU_VIEW_CONTOUR_PROPS, CZFXYPlotFrame::OnShowContourProps )
END_EVENT_TABLE()

CZFXYPlotFrame::CZFXYPlotFrame()
{
  Init();
}

//----------------------------------------
CZFXYPlotFrame::CZFXYPlotFrame(wxWindow *parent, wxWindowID id, const wxString &title, CZFXYPlotProperty* plotProperty,
                                 const wxPoint &position, const wxSize& size, long style )
    //: wxFrame(parent, id, title, position, size, style), --> called in Create
{
  Init();

  bool bOk = Create(parent, id, title, plotProperty, position, size, style);
  if (bOk == false)
  {
    CException e("ERROR in CZFXYPlotFrame ctor - Unenable to create wxWidgets (Create method returns false)", BRATHL_SYSTEM_ERROR);
    throw(e);
  }

}

//----------------------------------------
CZFXYPlotFrame::~CZFXYPlotFrame()
{


}
//----------------------------------------
bool CZFXYPlotFrame::Destroy()
{
/*
  if (m_sizer != NULL)
  {
    delete m_sizer;
    m_sizer = NULL;
  }
*/

  if (m_plotPanel != NULL)
  {
    m_plotPanel->Destroy();
    m_plotPanel = NULL;
  }

  return wxFrame::Destroy();
}

//----------------------------------------
void CZFXYPlotFrame::Init()
{

  m_plotPanel = NULL;
  m_multiFrame = false;
  m_hasClut = false;
  m_menuBar = NULL;
  m_lutFrame = NULL;
  m_contourPropFrame = NULL;
  //m_menuView = NULL;

}

//----------------------------------------
bool CZFXYPlotFrame::Create(wxWindow *parent, wxWindowID id, const wxString &title, CZFXYPlotProperty* plotProperty,
                                 const wxPoint &position, const wxSize& size, long style )

{
  //Enable(false);
  bool bOk = true;

  if (plotProperty != NULL)
  {
    m_plotProperty = *plotProperty;
  }

  wxFrame::Create(parent, id, title, position, size, style);

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

  bOk = CreateControls();
  if (bOk == false)
  {
    return false;
  }

  CreateLayout();
  InstallEventListeners();

  ShowPropertyPanel(m_plotProperty.m_showPropertyPanel);
  ShowAnimationToolbar(m_plotProperty.m_showAnimationToolbar);
  ShowColorBar(m_plotProperty.m_showColorBar);

  // Already in ShowPropertyPanel below
  //CUpdateMinSizeEvent evMinSize(GetId());
  //wxPostEvent(this, evMinSize);



  return true;
}
//----------------------------------------
void CZFXYPlotFrame::CreateMenuBar()
{
  m_menuBar = ZFXYPlotFrameMenuBarFunc();

  SetMenuBar(m_menuBar);

  m_menuBar->Enable(ID_MENU_VIEW_SLIDER, false);
  m_menuBar->Enable(ID_MENU_VIEW_COLORBAR, false);
  m_menuBar->Enable(ID_MENU_VIEW_CLUTEDIT, false);

  //m_menuBar->FindItem(ID_MENU_VIEW_SLIDER, &m_menuView);

}
//----------------------------------------
bool CZFXYPlotFrame::CreateControls()
{
  m_plotPanel = new CZFXYPlotPanel(this, -1, &m_plotProperty);
  return true;
}

//----------------------------------------
void CZFXYPlotFrame::CreateLayout()
{
  m_sizer = new wxBoxSizer(wxVERTICAL);

  m_sizer->Add(m_plotPanel, 1, wxEXPAND);
  SetSizer(m_sizer);

  //UpdateMinSize();

}

//----------------------------------------
void CZFXYPlotFrame::OnUpdateMinSize(CZFXYUpdateMinSizeEvent& event)
{
  UpdateMinSize();
}

//----------------------------------------
void CZFXYPlotFrame::UpdateMinSize()
{
  //Prevent redrawing of the window when we call SetClientSize()
  Freeze();
  //Reset the hard coded minimum size
  SetMinSize(wxSize(-1, -1));
  //This uses the sizer to calculate the minimal client size
  wxSize minClientSize = GetBestSize();
  //Turn the minimum client size to a minimum frame size
  wxSize prevSize = GetSize();
  SetClientSize(minClientSize);
  wxSize minSize = GetSize();
  SetMinSize(minSize);
  Thaw();
  SetSize(prevSize);
}

//----------------------------------------
void CZFXYPlotFrame::InstallEventListeners()
{
  CLUTPanel::EvtLutChanged(*this,
                           -1,
                           (CLutChangedEventFunction)&CZFXYPlotFrame::OnLutChanged);

  CLUTPanel::EvtLutFrameClose(*this,
                              -1,
                              (CLutFrameCloseEventFunction)&CZFXYPlotFrame::OnLutFrameClose);

  CZFXYContourPropPanel::EvtContourPropChanged(*this,
                           -1,
                           (CZFXYContourPropChangedEventFunction)&CZFXYPlotFrame::OnContourPropChanged);

  CZFXYContourPropPanel::EvtContourPropFrameClose(*this,
                              -1,
                              (CZFXYContourPropFrameCloseEventFunction)&CZFXYPlotFrame::OnContourPropFrameClose);

  CZFXYPlotFrame::EvtUpdateMinSize(*this,
                                    -1,
                                    (CZFXYUpdateMinSizeEventFunction)& CZFXYPlotFrame::OnUpdateMinSize);


}

//----------------------------------------
void CZFXYPlotFrame::DeInstallEventListeners()
{
  if (m_lutFrame != NULL)
  {
    m_lutFrame->Disconnect(wxEVT_DESTROY);
  }

  if (m_contourPropFrame != NULL)
  {
    m_contourPropFrame->Disconnect(wxEVT_DESTROY);
  }

}
//----------------------------------------
void CZFXYPlotFrame::EvtUpdateMinSize(wxWindow& window, int32_t winId, const CZFXYUpdateMinSizeEventFunction& method,
                                wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(winId,
                 wxEVT_ZFXY_UPDATE_MINSIZE,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}

// WDR: handler implementations for CZFXYPlotFrame

//----------------------------------------
void CZFXYPlotFrame::OnUpdateUIAnimationToolbar( wxUpdateUIEvent &event )
{

}
//----------------------------------------
void CZFXYPlotFrame::UpdateView( )
{
  //wxString szMsg = wxString::Format("%s", this->GetTitle().c_str());

  int32_t maxProgbar = 100;
  wxProgressDialog* dlg = new wxProgressDialog(GetTitle().c_str(),
                                               "Computing view...",
                                               maxProgbar, this, wxPD_SMOOTH|wxPD_APP_MODAL|wxPD_AUTO_HIDE);

  dlg->SetSize(-1,-1, 400, -1);

  dlg->Update(50);

  dlg->Update(75);


  if (m_plotPanel->IsNumberOfMapsEquals() == false)
  {
    m_menuBar->Enable(ID_MENU_VIEW_SLIDER, false);
    ShowAnimationToolbar(false);
  }

  dlg->Update(100);

  dlg->Destroy();
}

//----------------------------------------
void CZFXYPlotFrame::OnLutFrameClose( CLutFrameCloseEvent &event )
{
  if (m_lutFrame != NULL)
  {
    m_lutFrame->Close();
  }
}
//----------------------------------------
void CZFXYPlotFrame::OnContourPropFrameClose( CZFXYContourPropFrameCloseEvent &event )
{
  if (m_contourPropFrame != NULL)
  {
    m_contourPropFrame->Close();
  }
}

//----------------------------------------
void CZFXYPlotFrame::OnDestroyLUT( wxWindowDestroyEvent &event )
{
  m_lutFrame = NULL;
}

//----------------------------------------
void CZFXYPlotFrame::OnDestroyContourPropFrame( wxWindowDestroyEvent &event )
{
  m_contourPropFrame = NULL;
}

//----------------------------------------
void CZFXYPlotFrame::OnClose( wxCommandEvent &event )
{
  Close(true);
}
//----------------------------------------
void CZFXYPlotFrame::OnLutChanged( CLutChangedEvent &event )
{
  m_plotPanel->LutChanged(event);
}
//----------------------------------------
void CZFXYPlotFrame::OnContourPropChanged( CZFXYContourPropChangedEvent &event )
{
  m_plotPanel->OnContourPropChanged(event);
}


//----------------------------------------
void CZFXYPlotFrame::OnShowContourProps( wxCommandEvent &event )
{
  wxString frameTitle;

  if (m_contourPropFrame != NULL)
  {
    m_contourPropFrame->Raise();
    m_contourPropFrame->Show( true );
    // mandatory under WIN32, otherwise window stay hide
    m_contourPropFrame->Layout();

    return;
  }
  CZFXYPlotData* geoMap = m_plotPanel->GetCurrentLayer();
  if (geoMap == NULL)
  {
    CException e("CZFXYPlotFrame::OnShowContourProps - m_plotPanel->GetCurrentLayer() == NULL",
                 BRATHL_LOGIC_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);
  }

  frameTitle = wxString::Format("BRAT Contour Properties for %s",
                                geoMap->GetDataName().c_str());


  m_contourPropFrame = new CZFXYContourPropFrame(this, geoMap, -1, frameTitle,
                                            wxDefaultPosition,
                                            wxDefaultSize,
                                            wxDEFAULT_FRAME_STYLE | wxFRAME_FLOAT_ON_PARENT);

  m_contourPropFrame->GetContourPropPanel()->Layout();
  m_contourPropFrame->Layout();
  /*
  wxPoint pos = GetPosition();
  wxSize size = m_contourPropFrame->GetSize();
  int32_t newx = ((pos.x - size.x) > 0 ) ? (pos.x - size.x) : 0;
  int32_t newy = ((pos.y - size.y) > 0 ) ? (pos.y - size.y) : 0;
  m_contourPropFrame->SetPosition(wxPoint(newx, newy));
  */
  m_contourPropFrame->Raise();
  m_contourPropFrame->Show( TRUE );

  m_contourPropFrame->Connect(wxEVT_DESTROY,
                   (wxObjectEventFunction)
                   (wxEventFunction)
                   (wxWindowDestroyEventFunction)&CZFXYPlotFrame::OnDestroyContourPropFrame,
                   NULL,
                   this);


}

//----------------------------------------
void CZFXYPlotFrame::OnShowColorTableEditor( wxCommandEvent &event )
{
  wxString frameTitle;

  if (m_lutFrame != NULL)
  {
    m_lutFrame->Raise();
    m_lutFrame->Show( true );
    // mandatory under WIN32, otherwise window stay hide
    m_lutFrame->Layout();

    return;
  }
  CZFXYPlotData* geoMap = m_plotPanel->GetCurrentLayer();
  if (geoMap == NULL)
  {
    CException e("CZFXYPlotFrame::OnShowColorTableEditor - m_plotPanel->GetCurrentLayer() == NULL",
                 BRATHL_LOGIC_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);
  }

  frameTitle = wxString::Format("BRAT Color Table for %s",
                                geoMap->GetDataName().c_str());


  m_lutFrame = new CLUTFrame(this, -1, frameTitle, m_plotPanel->GetLUT(), true,
                              wxDefaultPosition,
                              wxDefaultSize,
                              wxDEFAULT_FRAME_STYLE | wxFRAME_FLOAT_ON_PARENT);
  //m_lutFrame->UpdateMode(m_plotPanel->GetLUT());
  m_lutFrame->GetLUTPanel()->Layout();
  m_lutFrame->Layout();
  /*
  wxPoint pos = GetPosition();
  wxSize size = m_lutFrame->GetSize();
  int32_t newx = ((pos.x - size.x) > 0 ) ? (pos.x - size.x) : 0;
  int32_t newy = ((pos.y - size.y) > 0 ) ? (pos.y - size.y) : 0;
  m_lutFrame->SetPosition(wxPoint(newx, newy));
  */
  m_lutFrame->Raise();
  m_lutFrame->Show( TRUE );

  m_lutFrame->Connect(wxEVT_DESTROY,
                   (wxObjectEventFunction)
                   (wxEventFunction)
                   (wxWindowDestroyEventFunction)&CZFXYPlotFrame::OnDestroyLUT,
                   NULL,
                   this);


}

//----------------------------------------
void CZFXYPlotFrame::OnViewColorbar( wxCommandEvent &event )
{
  ShowColorBar(event.IsChecked());
}

//----------------------------------------
void CZFXYPlotFrame::OnViewProps( wxCommandEvent &event )
{
  ShowPropertyPanel(event.IsChecked());
}

//----------------------------------------
void CZFXYPlotFrame::OnViewSlider( wxCommandEvent &event )
{
  ShowAnimationToolbar(event.IsChecked());
}

//----------------------------------------
void CZFXYPlotFrame::OnSave( wxCommandEvent &event )
{

  CStringMap infoImage;
  CMapImageType mapImageType;

  mapImageType.NamesToMapString(infoImage);

  CTypedSaveFileDialog* dlg = new CTypedSaveFileDialog(this,
                                                       ".",
                                                       infoImage,
                                                       -1,
                                                       "Save Image");
  if (dlg->ShowModal() == wxID_OK)
  {
    WriteImage(dlg->m_fileName);
  }

}

//----------------------------------------
void CZFXYPlotFrame::OnCloseWindow( wxCloseEvent &event )
{
  DeInstallEventListeners();
  Destroy();
}

//----------------------------------------
void CZFXYPlotFrame::ShowColorBar(bool showIt)
{
  m_menuBar->Check(ID_MENU_VIEW_COLORBAR, showIt);
  m_plotPanel->ShowColorBar(showIt);

}

//----------------------------------------
void CZFXYPlotFrame::ShowPropertyPanel(bool showIt)
{
  m_menuBar->Check(ID_MENU_VIEW_PROPS, showIt);
  m_plotPanel->Freeze();
  m_plotPanel->ShowPropertyPanel(showIt);

  //UpdateMinSize();
  CZFXYUpdateMinSizeEvent evMinSize(GetId());
  wxPostEvent(this, evMinSize);

  m_plotPanel->Thaw();
  m_plotPanel->Refresh();


}

//----------------------------------------
void CZFXYPlotFrame::ShowAnimationToolbar(bool showIt)
{
  if (m_menuBar->IsEnabled(ID_MENU_VIEW_SLIDER) == false)
  {
    return;
  }

  m_menuBar->Check(ID_MENU_VIEW_SLIDER, showIt);
  m_plotPanel->Freeze();
  m_plotPanel->ShowAnimationToolbar(showIt);

  //UpdateMinSize();
  CZFXYUpdateMinSizeEvent evMinSize(GetId());
  wxPostEvent(this, evMinSize);

  m_plotPanel->Thaw();
  m_plotPanel->Refresh();


}
//----------------------------------------
void CZFXYPlotFrame::AddData(CZFXYPlotData* pdata)
{
  if (pdata == NULL)
  {
    return;
  }

  wxString szMsg = wxString::Format("Rendering %s ...", this->GetTitle().c_str());
  int32_t maxProgbar = 4;
  wxProgressDialog* dlg = new wxProgressDialog(szMsg,
                                               "Adding data to plot...",
                                               maxProgbar, this, wxPD_SMOOTH|wxPD_APP_MODAL|wxPD_AUTO_HIDE);

  dlg->SetSize(-1,-1, 350, -1);
  int32_t nFrames = pdata->GetNrMaps();

  // Only pop-up the animationtoolbar if this is the first
  // multiframe dataset and the user has not specified any
  // explicit option so far.
  if ( (m_multiFrame == false) && (nFrames > 1) )
  {
    m_multiFrame = true;
    m_menuBar->Enable(ID_MENU_VIEW_SLIDER, true);

    //ShowAnimationToolbar(pdata->m_plotProperty.m_showAnimationToolbar);
    ShowAnimationToolbar(true);

  }

  if ( ( m_hasClut == false) && (pdata->GetLookupTable() != NULL) )
  {
    m_hasClut = true;
    m_menuBar->Enable(ID_MENU_VIEW_COLORBAR, m_hasClut);
    m_menuBar->Enable(ID_MENU_VIEW_CLUTEDIT, m_hasClut);
    //ShowColorBar(data->m_plotProperty.m_showColorBar);
  }

  m_plotPanel->AddData(pdata, dlg);
  //dlg->Update(maxProgbar);
  dlg->Destroy();
}

//----------------------------------------
void CZFXYPlotFrame::WriteImage(const wxFileName& fileName)
{
  vtkWindowToImageFilter* w2i = vtkWindowToImageFilter::New();

  CMapImageType mapImageType;

  CImageType* imageType = dynamic_cast<CImageType*>(mapImageType[(const char *)(fileName.GetExt())]);
  if (imageType == NULL)
  {
    CException e(CTools::Format("CZFXYPlotFrame::WriteImage - extension:'%s' - dynamic_cast<CImageType*>"
                                "(CMapImageType::GetInstance()[ext]) returns NULL pointer CMapImageType map seems to "
                                "contain objects other than those of the class CImageType",
                                (const char *)(fileName.GetExt())),
                 BRATHL_LOGIC_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);
  }
  if (imageType->m_vtkImageWriter == NULL)
  {
    CException e(CTools::Format("CZFXYPlotFrame::WriteImage - m_vtkImageWriter == NULL - type name:'%s' (extenstion "
                                "'%s')", (const char *)(imageType->m_name), (const char *)(fileName.GetExt())),
                 BRATHL_LOGIC_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);
  }

  try
  {
    ::wxRemoveFile(fileName.GetFullPath());
  }
  catch (...)
  {
    //do nothing
  }


  vtkRenderWindow* renderWindow = m_plotPanel->GetVtkWidget()->GetRenderWindow();

  renderWindow->OffScreenRenderingOn();

  w2i->SetInput(renderWindow);

  imageType->m_vtkImageWriter->SetInput(w2i->GetOutput());
  imageType->m_vtkImageWriter->SetFileName(fileName.GetFullPath().c_str());
  renderWindow->Render();

  imageType->m_vtkImageWriter->Write();

  w2i->Delete();

  renderWindow->OffScreenRenderingOff();

}

