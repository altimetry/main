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
    #pragma implementation "WorldPlotFrame.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "new-gui/Common/tools/Trace.h"
#include "Tools.h"
#include "new-gui/Common/tools/Exception.h"
using namespace brathl;


#include "vtkWindowToImageFilter.h"

#include "BratDisplayApp.h"
#include "TypedSaveFileDialog.h"
#include "MapImageType.h"

#include "WorldPlotFrame.h"

DEFINE_EVENT_TYPE(wxEVT_UPDATE_MINSIZE)

// WDR: class implementations

//----------------------------------------------------------------------------
// CProgressDialog
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(CProgressDialog,wxProgressDialog)

// WDR: event table for CProgressDialog

BEGIN_EVENT_TABLE(CProgressDialog,wxProgressDialog)
    EVT_TIMER(wxID_ANY, CProgressDialog::OnProgress)
    //EVT_WINDOW_DESTROY(CProgressDialog::OnDestroy)
END_EVENT_TABLE()

CProgressDialog::CProgressDialog(const wxString &title, wxString const &message,
                                int maximum,
                                wxWindow *parent,
                                int style)
                 : wxProgressDialog(title, message, maximum, parent, style)
{

  Connect(wxEVT_DESTROY,
           (wxObjectEventFunction)
           (wxEventFunction)
           (wxWindowDestroyEventFunction)&CProgressDialog::OnDestroy,
           NULL,
           this);

  m_count = 0;
  m_timer = new wxTimer(this);
  m_timer->Start(1000);
}
//----------------------------------------
CProgressDialog:: ~CProgressDialog()
{
}

//----------------------------------------
void CProgressDialog::OnProgress(wxTimerEvent& event)
{
  m_count++;
  this->Update(m_count);
}
//----------------------------------------
void CProgressDialog::OnDestroy( wxWindowDestroyEvent &event )
{
  m_timer->Stop();
  delete m_timer;
  m_timer = NULL;
}



// WDR: handler implementations for CProgressDialog


//----------------------------------------------------------------------------
// CWorldPlotFrame
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(CWorldPlotFrame,wxFrame)

// WDR: event table for CWorldPlotFrame

BEGIN_EVENT_TABLE(CWorldPlotFrame,wxFrame)
    EVT_MENU( ID_MENU_FILE_CLOSE, CWorldPlotFrame::OnClose )
    EVT_MENU( ID_MENU_FILE_SAVE, CWorldPlotFrame::OnSave )
    EVT_MENU( ID_MENU_VIEW_SLIDER, CWorldPlotFrame::OnViewSlider )
    EVT_MENU( ID_MENU_VIEW_PROPS, CWorldPlotFrame::OnViewProps )
    EVT_MENU( ID_MENU_VIEW_COLORBAR, CWorldPlotFrame::OnViewColorbar )
    EVT_MENU( ID_MENU_VIEW_CLUTEDIT, CWorldPlotFrame::OnShowColorTableEditor )
    EVT_CLOSE( CWorldPlotFrame::OnCloseWindow )
    EVT_UPDATE_UI( ID_MENU_VIEW_SLIDER, CWorldPlotFrame::OnUpdateUIAnimationToolbar )
    EVT_MENU( ID_MENU_VIEW_CONTOUR_PROPS, CWorldPlotFrame::OnShowContourProps )
END_EVENT_TABLE()

CWorldPlotFrame::CWorldPlotFrame()
{
  Init();
}

//----------------------------------------
CWorldPlotFrame::CWorldPlotFrame( wxWindow *parent, wxWindowID id, const wxString &title, CWorldPlotProperties* plotProperty,
	const wxPoint &position, const wxSize& size, long style )
	//: wxFrame(parent, id, title, position, size, style), --> called in Create
{
	Init();
	if ( !Create( parent, id, title, plotProperty, position, size, style ) )
	{
		CException e( "ERROR in CWorldPlotFrame ctor - Unable to create wxWidgets (Create method returns false)", BRATHL_SYSTEM_ERROR );
		throw( e );
	}
}

//----------------------------------------
CWorldPlotFrame::~CWorldPlotFrame()
{


}
//----------------------------------------
bool CWorldPlotFrame::Destroy()
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
void CWorldPlotFrame::Init()
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
bool CWorldPlotFrame::Create( wxWindow *parent, wxWindowID id, const wxString &title, CWorldPlotProperties* plotProperty,
	const wxPoint &position, const wxSize& size, long style )

{
	//Enable(false);

	if ( plotProperty != NULL )
	{
		m_plotProperty = *plotProperty;
	}

	wxFrame::Create( parent, id, title, position, size, style );

	try
	{
		if ( !wxGetApp().GetIconFile().IsEmpty() )
		{
			SetIcon( wxIcon( wxGetApp().GetIconFile(), ::wxGetApp().GetIconType() ) );
		}
		else
		{
			::wxMessageBox( wxString::Format( "WARNING: Unable to find Brat icon file %s",
				::wxGetApp().GetIconFileName().c_str() )
				, "BRAT WARNING" );
		}
	}
	catch ( ... )
	{
		// Do nothing
	}

	//self.prefs = Preferences.Preferences()


	CreateMenuBar();

	m_plotPanel = new CWorldPlotPanel(this, -1, &m_plotProperty);
	//if ( !CreateControls() )
	//	return false;

	CreateLayout();
	InstallEventListeners();

	ShowPropertyPanel( m_plotProperty.m_showPropertyPanel );
	ShowAnimationToolbar( m_plotProperty.m_showAnimationToolbar );
	ShowColorBar( m_plotProperty.m_showColorBar );

	SetCenterLongitude( m_plotProperty.m_centerLongitude );
	SetCenterLatitude( m_plotProperty.m_centerLatitude );

	// Already in ShowPropertyPanel below
	//CUpdateMinSizeEvent evMinSize(GetId());
	//wxPostEvent(this, evMinSize);

	CCenterPointChangedEvent evt( GetId(),
		m_plotProperty.m_centerLatitude,
		m_plotProperty.m_centerLongitude );
	wxPostEvent( m_plotPanel->GetPlotPropertyTab(),
		evt );

	return true;
}
//----------------------------------------
void CWorldPlotFrame::CreateMenuBar()
{
  m_menuBar = WorldPlotFrameMenuBarFunc();

  SetMenuBar(m_menuBar);

  m_menuBar->Enable(ID_MENU_VIEW_SLIDER, false);
  m_menuBar->Enable(ID_MENU_VIEW_COLORBAR, false);
  m_menuBar->Enable(ID_MENU_VIEW_CLUTEDIT, false);

  //m_menuBar->FindItem(ID_MENU_VIEW_SLIDER, &m_menuView);

}
//----------------------------------------
//bool CWorldPlotFrame::CreateControls()
//{
//  m_plotPanel = new CWorldPlotPanel(this, -1, &m_plotProperty);
//  return true;
//}

//----------------------------------------
void CWorldPlotFrame::CreateLayout()
{
  m_sizer = new wxBoxSizer(wxVERTICAL);

  m_sizer->Add(m_plotPanel, 1, wxEXPAND);
  SetSizer(m_sizer);

  //UpdateMinSize();

}

//----------------------------------------
void CWorldPlotFrame::OnUpdateMinSize(CUpdateMinSizeEvent& event)
{
  UpdateMinSize();
}

//----------------------------------------
void CWorldPlotFrame::UpdateMinSize()
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
void CWorldPlotFrame::InstallEventListeners()
{
  CLUTPanel::EvtLutChanged(*this,
                           -1,
                           (CLutChangedEventFunction)&CWorldPlotFrame::OnLutChanged);

  CLUTPanel::EvtLutFrameClose(*this,
                              -1,
                              (CLutFrameCloseEventFunction)&CWorldPlotFrame::OnLutFrameClose);

  CContourPropPanel::EvtContourPropChanged(*this,
                           -1,
                           (CContourPropChangedEventFunction)&CWorldPlotFrame::OnContourPropChanged);

  CContourPropPanel::EvtContourPropFrameClose(*this,
                              -1,
                              (CContourPropFrameCloseEventFunction)&CWorldPlotFrame::OnContourPropFrameClose);

  CWorldPlotFrame::EvtUpdateMinSize(*this,
                                    -1,
                                    (CUpdateMinSizeEventFunction)& CWorldPlotFrame::OnUpdateMinSize);


}

//----------------------------------------
void CWorldPlotFrame::DeInstallEventListeners()
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
void CWorldPlotFrame::EvtUpdateMinSize(wxWindow& window, int32_t winId, const CUpdateMinSizeEventFunction& method,
                                wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(winId,
                 wxEVT_UPDATE_MINSIZE,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}

// WDR: handler implementations for CWorldPlotFrame

//----------------------------------------
void CWorldPlotFrame::OnUpdateUIAnimationToolbar( wxUpdateUIEvent &event )
{

}
//----------------------------------------
void CWorldPlotFrame::UpdateView( )
{
  //wxString szMsg = wxString::Format("%s", this->GetTitle().c_str());

  int32_t maxProgbar = 100;
  int32_t nprojection = -1;
  wxProgressDialog* dlg = new wxProgressDialog(GetTitle().c_str(),
                                               "Computing view...",
                                               maxProgbar, this, wxPD_SMOOTH|wxPD_APP_MODAL|wxPD_AUTO_HIDE);

  dlg->SetSize(-1,-1, 400, -1);

  dlg->Update(50);
  m_plotPanel->SetProjection();

  dlg->Update(75);

  // Set the initial Zoom level.  3D and 2D have diffrent scales
  nprojection = CMapProjection::GetInstance()->NameToId(m_plotProperty.m_projection);
  if (nprojection == VTK_PROJ2D_3D || nprojection <= 0)
  {
      m_plotPanel->ZoomInit(2.5);
  }
  else
      m_plotPanel->ZoomInit(1.0);

  m_plotPanel->SetViewParams();


  if (m_plotPanel->IsNumberOfMapsEquals() == false)
  {
    m_menuBar->Enable(ID_MENU_VIEW_SLIDER, false);
    ShowAnimationToolbar(false);
  }

  dlg->Update(100);

  dlg->Destroy();
}

//----------------------------------------
void CWorldPlotFrame::OnLutFrameClose( CLutFrameCloseEvent &event )
{
  if (m_lutFrame != NULL)
  {
    m_lutFrame->Close();
  }
}
//----------------------------------------
void CWorldPlotFrame::OnContourPropFrameClose( CContourPropFrameCloseEvent &event )
{
  if (m_contourPropFrame != NULL)
  {
    m_contourPropFrame->Close();
  }
}

//----------------------------------------
void CWorldPlotFrame::OnDestroyLUT( wxWindowDestroyEvent &event )
{
  m_lutFrame = NULL;
}

//----------------------------------------
void CWorldPlotFrame::OnDestroyContourPropFrame( wxWindowDestroyEvent &event )
{
  m_contourPropFrame = NULL;
}

//----------------------------------------
void CWorldPlotFrame::OnClose( wxCommandEvent &event )
{
  Close(true);
}
//----------------------------------------
void CWorldPlotFrame::OnLutChanged( CLutChangedEvent &event )
{
  m_plotPanel->LutChanged(event);
}
//----------------------------------------
void CWorldPlotFrame::OnContourPropChanged( CContourPropChangedEvent &event )
{
  m_plotPanel->OnContourPropChanged(event);
}


//----------------------------------------
void CWorldPlotFrame::OnShowContourProps( wxCommandEvent &event )
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
  VTK_CWorldPlotData* geoMap = m_plotPanel->GetCurrentLayer();
  if (geoMap == NULL)
  {
    CException e("CWorldPlotFrame::OnShowContourProps - m_plotPanel->GetCurrentLayer() == NULL",
                 BRATHL_LOGIC_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);
  }

  frameTitle = wxString::Format("BRAT Contour Properties for %s",
                                geoMap->GetDataName().c_str());


  m_contourPropFrame = new CContourPropFrame(this, geoMap, -1, frameTitle,
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
                   (wxWindowDestroyEventFunction)&CWorldPlotFrame::OnDestroyContourPropFrame,
                   NULL,
                   this);


}

//----------------------------------------
void CWorldPlotFrame::OnShowColorTableEditor( wxCommandEvent &event )
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
  VTK_CWorldPlotData* geoMap = m_plotPanel->GetCurrentLayer();
  if (geoMap == NULL)
  {
    CException e("CWorldPlotFrame::OnShowColorTableEditor - m_plotPanel->GetCurrentLayer() == NULL",
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
                   (wxWindowDestroyEventFunction)&CWorldPlotFrame::OnDestroyLUT,
                   NULL,
                   this);


}

//----------------------------------------
void CWorldPlotFrame::OnViewColorbar( wxCommandEvent &event )
{
  ShowColorBar(event.IsChecked());
}

//----------------------------------------
void CWorldPlotFrame::OnViewProps( wxCommandEvent &event )
{
  ShowPropertyPanel(event.IsChecked());
}

//----------------------------------------
void CWorldPlotFrame::OnViewSlider( wxCommandEvent &event )
{
  ShowAnimationToolbar(event.IsChecked());
}

//----------------------------------------
void CWorldPlotFrame::OnSave( wxCommandEvent &event )
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
      ///???self.prefs.exportdir = os.path.dirname(dlg.filename)
  }
  /* ??????????
       imagetypemap = [("TIFF"          , "tif", vtk.vtkTIFFWriter),
                       ("Windows Bitmap" , "bmp", vtk.vtkBMPWriter),
                       ("JPEG"           , "jpg", vtk.vtkJPEGWriter),
                       ("PNG"            , "png", vtk.vtkPNGWriter),
                       ("PNM"            , "pnm", vtk.vtkPNMWriter)
                       ]

        imagetypeinfo = [(i[0], i[1]) for i in imagetypemap]
        imagewriters = dict([(i[1], i[2]) for i in imagetypemap])

        dlg = TypedSaveFileDialog(self,
                                  title="Save Image",
                                  initialdir=self.prefs.exportdir,
                                  typeinfo=imagetypeinfo)

        if dlg.ShowModal() == wx.ID_OK:
            self.WriteImage(dlg.filename, imagewriters[dlg.ext])
            self.prefs.exportdir = os.path.dirname(dlg.filename)
*/

}

//----------------------------------------
void CWorldPlotFrame::OnCloseWindow( wxCloseEvent &event )
{
  // if ! saved changes -> return

    //???    if self.closingdown:
    //???        return
    //???    self.closingdown = True

  DeInstallEventListeners();

  //m_plotPanel->Destroy();
  Destroy();

}

//----------------------------------------
void CWorldPlotFrame::ShowColorBar(bool showIt)
{
  m_menuBar->Check(ID_MENU_VIEW_COLORBAR, showIt);
  m_plotPanel->ShowColorBar(showIt);

}

//----------------------------------------
void CWorldPlotFrame::ShowPropertyPanel(bool showIt)
{
  m_menuBar->Check(ID_MENU_VIEW_PROPS, showIt);
  m_plotPanel->Freeze();
  m_plotPanel->ShowPropertyPanel(showIt);

  //UpdateMinSize();
  CUpdateMinSizeEvent evMinSize(GetId());
  wxPostEvent(this, evMinSize);

  m_plotPanel->Thaw();
  m_plotPanel->Refresh();


}

//----------------------------------------
void CWorldPlotFrame::ShowAnimationToolbar(bool showIt)
{
  if (m_menuBar->IsEnabled(ID_MENU_VIEW_SLIDER) == false)
  {
    return;
  }

  m_menuBar->Check(ID_MENU_VIEW_SLIDER, showIt);
  m_plotPanel->Freeze();
  m_plotPanel->ShowAnimationToolbar(showIt);

  //UpdateMinSize();
  CUpdateMinSizeEvent evMinSize(GetId());
  wxPostEvent(this, evMinSize);

  m_plotPanel->Thaw();
  m_plotPanel->Refresh();


}
//----------------------------------------
void CWorldPlotFrame::AddData(VTK_CWorldPlotCommonData* data)
{
  wxString szMsg = wxString::Format("Rendering %s ...", this->GetTitle().c_str());
  int32_t maxProgbar = 4;
  wxProgressDialog* dlg = new wxProgressDialog(szMsg,
                                               "Adding data to plot...",
                                               maxProgbar, this, wxPD_SMOOTH|wxPD_APP_MODAL|wxPD_AUTO_HIDE);

  dlg->SetSize(-1,-1, 350, -1);
  int32_t nFrames = 1;

  VTK_CWorldPlotData* geoMap = dynamic_cast<VTK_CWorldPlotData*>(data);
  if (geoMap != NULL)
  {
    nFrames = geoMap->GetNrMaps();
  }

  // Only pop-up the animationtoolbar if this is the first
  // multiframe dataset and the user has not specified any
  // explicit option so far.
  if ( (m_multiFrame == false) && (nFrames > 1) )
  {
    m_multiFrame = true;
    m_menuBar->Enable(ID_MENU_VIEW_SLIDER, true);

    ShowAnimationToolbar(data->m_plotProperty.m_showAnimationToolbar);
  }

  if ( ( m_hasClut == false) && (data->GetLookupTable() != NULL) )
  {
    m_hasClut = true;
    m_menuBar->Enable(ID_MENU_VIEW_COLORBAR, m_hasClut);
    m_menuBar->Enable(ID_MENU_VIEW_CLUTEDIT, m_hasClut);
    //ShowColorBar(data->m_plotProperty.m_showColorBar);
  }

  m_plotPanel->AddData(data, dlg);
  //dlg->Update(maxProgbar);
  dlg->Destroy();
}
//----------------------------------------
void CWorldPlotFrame::SetCenterLatitude(double c)
{
  m_plotPanel->SetCenterLatitude(c);
}
//----------------------------------------
void CWorldPlotFrame::SetCenterLongitude(double c)
{
  m_plotPanel->SetCenterLongitude(c);
}

//----------------------------------------
void CWorldPlotFrame::WriteImage(const wxFileName& fileName)
{
  vtkWindowToImageFilter* w2i = vtkWindowToImageFilter::New();

  CMapImageType mapImageType;

  CImageType* imageType = dynamic_cast<CImageType*>(mapImageType[(const char *)(fileName.GetExt())]);
  if (imageType == NULL)
  {
    CException e(CTools::Format("CWorldPlotFrame::WriteImage - extension:'%s' - dynamic_cast<CImageType*>"
                                "(CMapImageType::GetInstance()[ext]) returns NULL pointer CMapImageType map seems to "
                                "contain objects other than those of the class CImageType",
                                (const char *)(fileName.GetExt())),
                 BRATHL_LOGIC_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);
  }
  if (imageType->m_vtkImageWriter == NULL)
  {
    CException e(CTools::Format("CWorldPlotFrame::WriteImage - m_vtkImageWriter == NULL - type name:'%s' (extenstion "
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

