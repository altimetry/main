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
    #pragma implementation "ZFXYContourPropFrame.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "BratDisplayApp.h"
#include "ZFXYContourPropFrame.h"
#include "PlotData/MapColor.h"

DEFINE_EVENT_TYPE(wxEVT_ZFXY_CONTOUR_PROP_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_ZFXY_CONTOUR_PROP_CLOSE)

// WDR: class implementations

//----------------------------------------------------------------------------
// CZFXYContourPropPanel
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(CZFXYContourPropPanel,wxPanel)

// WDR: event table for CZFXYContourPropPanel

BEGIN_EVENT_TABLE(CZFXYContourPropPanel,wxPanel)
    EVT_BUTTON( ID_CONTOUR_LINE_COLOR, CZFXYContourPropPanel::OnLineColor )
    EVT_BUTTON( ID_CONTOUR_LABEL_COLOR, CZFXYContourPropPanel::OnLabelColor )
    EVT_CHECKBOX( ID_CONTOUR_LABEL_COLOR_SAME, CZFXYContourPropPanel::OnSameColor )
    EVT_BUTTON( ID_CONTOUR_APPLY, CZFXYContourPropPanel::OnApply )
    EVT_BUTTON( ID_CONTOUR_CLOSE, CZFXYContourPropPanel::OnClose )
END_EVENT_TABLE()
//----------------------------------------
CZFXYContourPropPanel::CZFXYContourPropPanel()
{
  Init();
}

//----------------------------------------
CZFXYContourPropPanel::CZFXYContourPropPanel( wxWindow *parent, CZFXYPlotData* geoMap, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) 
    //: wxPanel( parent, id, position, size, style )
{
  bool bOk = Create(parent, geoMap, id, position, size, style);
  if (bOk == false)
  {
    CException e("ERROR in CZFXYContourPropPanel ctor - Unenable to create wxWidgets (Create method returns false)", BRATHL_SYSTEM_ERROR);
    throw(e);
  }

}

//----------------------------------------
CZFXYContourPropPanel::~CZFXYContourPropPanel()
{
}
//----------------------------------------
bool CZFXYContourPropPanel::Create( wxWindow *parent, CZFXYPlotData* geoMap, wxWindowID id,
                const wxPoint &position, const wxSize& size, long style ) 
{

  m_geoMap = geoMap;
  m_plotProperty = m_geoMap->m_plotProperty;

  wxPanel::Create(parent, id, position, size, style);

  // WDR: dialog function ContourProperties for CZFXYContourPropFrame
  ContourPropertiesPanel( this, true); 

  InstallEventListeners();
  InstallToolTips();
  FillProperties();

  return true;

}


//----------------------------------------
void CZFXYContourPropPanel::Init()
{

}

//----------------------------------------
void CZFXYContourPropPanel::InstallEventListeners()
{

}
//----------------------------------------
void CZFXYContourPropPanel::InstallToolTips()
{
  GetContourMin()->SetToolTip("Minimum contour value.");
  GetContourMax()->SetToolTip("Maximum contour value.");
  GetContourNumber()->SetToolTip("Number of contours equally spaced contour values between specified range (min./max.).");
  GetContourLineWidth()->SetToolTip("Width of the lines.");
  GetContourLabelNumber()->SetToolTip("Number of labels per contour");
  GetContourLabelSize()->SetToolTip("Size of labels");
  GetContourLabelFormat()->SetToolTip("Format representation of the labels (C/C++ notation)");

}

//----------------------------------------
void CZFXYContourPropPanel::FillProperties()
{
  GetContourMin()->SetValue(m_plotProperty.m_minContourValue, 0.0);
  GetContourMax()->SetValue(m_plotProperty.m_maxContourValue, 0.0);
  GetContourNumber()->SetValue(m_plotProperty.m_numContour, 5, 1);

  m_lineColor = m_plotProperty.m_contourLineColor;
  GetContourLineColor()->SetBackgroundColour(color_cast(m_lineColor));
  GetContourLineWidth()->SetValue(m_plotProperty.m_contourLineWidth, 1.0, 0.5);

  GetContourLabel()->SetValue(m_plotProperty.m_withContourLabel);
  GetContourLabelNumber()->SetValue(m_plotProperty.m_numContourLabel, 1, 1);

  m_labelColor = m_plotProperty.m_contourLabelColor;
  GetContourLabelColor()->SetBackgroundColour(color_cast(m_labelColor));
  
  GetContourLabelColorSame()->SetValue( m_labelColor == m_lineColor );
 
  GetContourLabelSize()->SetValue(m_plotProperty.m_contourLabelSize, 10, 1);
  GetContourLabelFormat()->SetValue(m_plotProperty.m_contourLabelFormat, "%-#.3g");

}

//----------------------------------------
void CZFXYContourPropPanel::EvtContourPropChanged(wxWindow& window, int32_t winId, const CZFXYContourPropChangedEventFunction& method,
                                wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(winId,
                 wxEVT_ZFXY_CONTOUR_PROP_CHANGED,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}

//----------------------------------------
void CZFXYContourPropPanel::EvtContourPropFrameClose(wxWindow& window, int32_t winId, const CZFXYContourPropFrameCloseEventFunction& method,
                                wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(winId,
                 wxEVT_ZFXY_CONTOUR_PROP_CLOSE,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}


// WDR: handler implementations for CZFXYContourPropPanel

//----------------------------------------
void CZFXYContourPropPanel::OnClose( wxCommandEvent &event )
{    
  GetParent()->Close(true);        
}
//----------------------------------------
void CZFXYContourPropPanel::OnApply( wxCommandEvent &event )
{
  GetProperties();

  if (GetParent() == NULL) // Frame
  {
    return;
  }
  if (GetParent()->GetParent() == NULL) // Parent of the Frame
  {
    return;
  }

  
  CZFXYContourPropChangedEvent  ev(GetId(), m_plotProperty);
  wxPostEvent(GetParent()->GetParent(), ev);
    
}
//----------------------------------------
void CZFXYContourPropPanel::GetProperties()
{
  GetContourMin()->GetValue(m_plotProperty.m_minContourValue, 0.0);
  GetContourMax()->GetValue(m_plotProperty.m_maxContourValue, 0.0);
  GetContourNumber()->GetValue(m_plotProperty.m_numContour, 5, 1);

  m_plotProperty.m_contourLineColor = m_lineColor;
  GetContourLineWidth()->GetValue(m_plotProperty.m_contourLineWidth, 1.0, 0.5);

  m_plotProperty.m_withContourLabel = GetContourLabel()->GetValue();
  GetContourLabelNumber()->GetValue(m_plotProperty.m_numContourLabel, 1, 1);

  m_plotProperty.m_contourLabelColor = m_labelColor;
 
  GetContourLabelSize()->GetValue(m_plotProperty.m_contourLabelSize, 10, 1);
  GetContourLabelFormat()->GetValue(m_plotProperty.m_contourLabelFormat, "%-#.3g");

}

//----------------------------------------
void CZFXYContourPropPanel::OnSameColor( wxCommandEvent &event )
{
  bool sameColor =  GetContourLabelColorSame()->GetValue();
  if (sameColor)
  {
    m_labelColor = m_lineColor;
    GetContourLabelColor()->SetBackgroundColour(color_cast(m_lineColor));
    Layout();
  }
}

//----------------------------------------
void CZFXYContourPropPanel::OnLabelColor( wxCommandEvent &event )
{
  wxColourData colorData = CMapColor::GetInstance().ChooseColor(color_cast(m_geoMap->m_plotProperty.m_contourLabelColor), this);
  m_labelColor = color_cast(colorData);

  GetContourLabelColor()->SetBackgroundColour(color_cast(m_labelColor));

  GetContourLabelColorSame()->SetValue(m_labelColor == m_lineColor);

  Layout();
    

}

//----------------------------------------
void CZFXYContourPropPanel::OnLineColor( wxCommandEvent &event )
{
  wxColourData colorData = CMapColor::GetInstance().ChooseColor(color_cast(m_geoMap->m_plotProperty.m_contourLineColor), this);
  m_lineColor = color_cast(colorData);

  GetContourLineColor()->SetBackgroundColour(color_cast(m_lineColor));

  if ( GetContourLabelColorSame()->GetValue())
  {
    m_labelColor = m_lineColor;
    GetContourLabelColor()->SetBackgroundColour(color_cast(m_labelColor));
  }

  GetContourLabelColorSame()->SetValue(m_labelColor == m_lineColor);

  Layout();
    
}

//----------------------------------------------------------------------------
// CZFXYContourPropFrame
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(CZFXYContourPropFrame,wxFrame)

// WDR: event table for CZFXYContourPropFrame

BEGIN_EVENT_TABLE(CZFXYContourPropFrame,wxFrame)
    EVT_CLOSE( CZFXYContourPropFrame::OnCloseWindow )
END_EVENT_TABLE()

//----------------------------------------
CZFXYContourPropFrame::CZFXYContourPropFrame()
{
  Init();
}

//----------------------------------------
CZFXYContourPropFrame::CZFXYContourPropFrame( wxWindow *parent, CZFXYPlotData* geoMap, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) 
    ///: wxFrame( parent, id, title, position, size, style )
{
  bool bOk = Create(parent, geoMap, id, title, position, size, style);
  if (bOk == false)
  {
    CException e("ERROR in CZFXYContourPropFrame ctor - Unenable to create wxWidgets (Create method returns false)", BRATHL_SYSTEM_ERROR);
    throw(e);
  }

}

//----------------------------------------
CZFXYContourPropFrame::~CZFXYContourPropFrame()
{
}

//----------------------------------------
bool CZFXYContourPropFrame::Create( wxWindow *parent, CZFXYPlotData* geoMap, wxWindowID id, const wxString &title,
                const wxPoint &position, const wxSize& size, long style ) 
{

  wxFrame::Create(parent, id, title, position, size, style);

  Show(false);

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

  m_contourPropPanel = new CZFXYContourPropPanel(this, geoMap);

  wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(m_contourPropPanel, 1, wxEXPAND);
  SetSizer(sizer);
  UpdateMinSize();

  InstallEventListeners();
  InstallToolTips();

  return true;

}


//----------------------------------------
void CZFXYContourPropFrame::Init()
{
  m_contourPropPanel = NULL;
}

//----------------------------------------
void CZFXYContourPropFrame::InstallEventListeners()
{

}
//----------------------------------------
void CZFXYContourPropFrame::InstallToolTips()
{

}
//----------------------------------------
void CZFXYContourPropFrame::UpdateMinSize()
{
  if (m_contourPropPanel == NULL)
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
bool CZFXYContourPropFrame::Destroy()
{
  return wxFrame::Destroy();
}

//----------------------------------------

// WDR: handler implementations for CZFXYContourPropFrame


//----------------------------------------
void CZFXYContourPropFrame::OnCloseWindow( wxCloseEvent &event )
{

  Destroy();
    
}


