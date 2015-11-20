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
    #pragma implementation "XYPlotFrame.h"
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

#include "BratDisplayApp.h"
#include "TypedSaveFileDialog.h"
#include "MapImageType.h"

#include "XYPlotFrame.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// CXYPlotFrame
//----------------------------------------------------------------------------
IMPLEMENT_CLASS(CXYPlotFrame,wxFrame)

// WDR: event table for CXYPlotFrame

BEGIN_EVENT_TABLE(CXYPlotFrame,wxFrame)
    EVT_MENU( ID_MENU_FILE_CLOSE, CXYPlotFrame::OnClose )
    EVT_MENU( ID_MENU_FILE_SAVE, CXYPlotFrame::OnSave )
    EVT_MENU( ID_MENU_FILE_EXPORT, CXYPlotFrame::OnExport )
    EVT_MENU( ID_MENU_VIEW_SLIDER, CXYPlotFrame::OnViewSlider )
    EVT_MENU( ID_MENU_VIEW_PROPS, CXYPlotFrame::OnViewProps )
    EVT_CLOSE( CXYPlotFrame::OnCloseWindow )
END_EVENT_TABLE()

CXYPlotFrame::CXYPlotFrame()
{
  Init();
}

//----------------------------------------
CXYPlotFrame::CXYPlotFrame(wxWindow *parent, wxWindowID id, const wxString &title,
                                 const wxPoint &position, const wxSize& size, long style )
    //: wxFrame(parent, id, title, position, size, style), --> called in Create
{
  Init();

  bool bOk = Create(parent, id, title, position, size, style);
  if (bOk == false)
  {
    CException e("ERROR in CXYPlotFrame ctor - Unenable to create wxWidgets (Create method returns false)", BRATHL_SYSTEM_ERROR);
    throw(e);
  }

}

//----------------------------------------
CXYPlotFrame::~CXYPlotFrame()
{


}
//----------------------------------------
bool CXYPlotFrame::Destroy()
{

  if (m_plotPanel != NULL)
  {
    m_plotPanel->Destroy();
    m_plotPanel = NULL;
  }

  return wxFrame::Destroy();
}

//----------------------------------------
void CXYPlotFrame::Init()
{
  m_sizer = NULL;
  m_plotPanel = NULL;
  m_multiFrame = false;
  m_menuBar = NULL;
}

//----------------------------------------
bool CXYPlotFrame::Create(wxWindow *parent, wxWindowID id, const wxString &title,
                                 const wxPoint &position, const wxSize& size, long style )
{
  bool bOk = true;

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


  return true;
}
//----------------------------------------
void CXYPlotFrame::CreateMenuBar()
{
  m_menuBar = XYPlotFrameMenuBarFunc();

  SetMenuBar(m_menuBar);

  m_menuBar->Enable(ID_MENU_VIEW_SLIDER, false);
}
//----------------------------------------
bool CXYPlotFrame::CreateControls()
{
  m_plotPanel = new CXYPlotPanel(this);
  return true;
}

//----------------------------------------
void CXYPlotFrame::CreateLayout()
{
  m_sizer = new wxBoxSizer(wxVERTICAL);

  m_sizer->Add(m_plotPanel, 1, wxEXPAND);
  SetSizer(m_sizer);
  UpdateMinSize();
}
//----------------------------------------
void CXYPlotFrame::UpdateMinSize()
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
void CXYPlotFrame::InstallEventListeners()
{

}

//----------------------------------------
void CXYPlotFrame::DeInstallEventListeners()
{

}
//----------------------------------------
CXYPlotProperty* CXYPlotFrame::GetPlotProperty(int32_t index)
{
  return m_plotPanel->GetPlotProperty(index);
}


// WDR: handler implementations for CXYPlotFrame
//----------------------------------------
void CXYPlotFrame::OnClose( wxCommandEvent &event )
{
  Close(true);
}


//----------------------------------------
void CXYPlotFrame::OnViewProps( wxCommandEvent &event )
{
  ShowPropertyPanel(event.IsChecked());
}

//----------------------------------------
void CXYPlotFrame::OnViewSlider( wxCommandEvent &event )
{
  ShowAnimationToolbar(event.IsChecked());
}

//----------------------------------------
void CXYPlotFrame::OnSave( wxCommandEvent &event )
{
  // ::wxMessageBox("Sorry but this function is not yet avalaible");
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
void CXYPlotFrame::OnCloseWindow( wxCloseEvent &event )
{
  // if ! saved changes -> return

  DeInstallEventListeners();
  Destroy();
}

//----------------------------------------

void CXYPlotFrame::OnExport( wxCommandEvent &event )
{
//    ::wxMessageBox("Sorry but this function is not yet avalaible");
  wxFileName fileName;
  fileName.Assign( wxFileSelector("Export to Gnuplot...",
                                   ".",
                                   "",
                                   "gp",
                                   "Gnuplot files|*.gp|All Files|*.*",
                                   wxFD_SAVE | wxFD_OVERWRITE_PROMPT,
                                   this));

  fileName.Normalize();

  if (fileName.GetName().IsEmpty() == false)
  {
    m_plotPanel->GetPlotActor()->ExportToGnuplotFile(fileName.GetFullPath().c_str());
//??    int saved = m_plotPanel->GetPlotActor()->ExportToGnuplotFile(fileName.GetFullPath().c_str());
  }
//??            if saved != -1:
//??                self.filename=os.path.basename(filename)
//??                self.prefs.exportdir = os.path.dirname(filename)



  /*?????
        filename = wx.FileSelector("Export to Gnuplot",
                                default_path=self.prefs.exportdir,
                                default_filename=self.filename,
                                default_extension="gp",
                                wildcard="Gnuplot files|*.gp|All Files|*.*",
                                flags=wx.SAVE | wx.OVERWRITE_PROMPT,
                                parent=self
                                )

        if (filename):
            saved = self.plotpanel.ExportToGnuplotFile(filename)
            if saved != -1:
                self.filename=os.path.basename(filename)
                self.prefs.exportdir = os.path.dirname(filename)
*/

}

//----------------------------------------
void CXYPlotFrame::ShowPropertyPanel()
{
  ShowPropertyPanel(m_plotPanel->GetPlotDataCollection()->ShowPropertyMenu());
}


//----------------------------------------
void CXYPlotFrame::ShowPropertyPanel(bool showIt)
{
  GetMenuBar()->Check(ID_MENU_VIEW_PROPS, showIt);
  m_plotPanel->Freeze();
  m_plotPanel->ShowPropertyPanel(showIt);
  UpdateMinSize();
  m_plotPanel->Thaw();
  m_plotPanel->Refresh();


}

//----------------------------------------
void CXYPlotFrame::ShowAnimationToolbar(bool showIt)
{
  GetMenuBar()->Check(ID_MENU_VIEW_SLIDER, showIt);
  m_plotPanel->Freeze();
  m_plotPanel->ShowAnimationToolbar(showIt);
  UpdateMinSize();
  m_plotPanel->Thaw();
  m_plotPanel->Refresh();


}
//----------------------------------------
void CXYPlotFrame::AddData(CXYPlotData* data)
{
/*
        uniquified_name = self.usg.uniquify(data.GetName())
        if not uniquified_name.endswith(" 1"):
            data.SetName(uniquified_name)
*/
  // Only pop-up the animationtoolbar if this is the first
  // multiframe dataset and the user has not specified any
  // explicit option so far.
  if (m_multiFrame == false)
  {
    int32_t nFrames = data->GetNumberOfFrames();
    if (nFrames > 1)
    {
      //CXYPlotProperty* props = data->GetPlotProperty();
      m_multiFrame = true;
      m_menuBar->Enable(ID_MENU_VIEW_SLIDER, true);
      //ShowAnimationToolbar(props->GetShowAnimationToolbar());
      ShowAnimationToolbar(true);
    }
  }
  m_plotPanel->AddData(data);
}

//----------------------------------------
void CXYPlotFrame::WriteImage(const wxFileName& fileName)
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



