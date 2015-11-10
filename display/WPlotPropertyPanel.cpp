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
    #pragma implementation "WPlotPropertyPanel.h"
#endif


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "Exception.h"
using namespace brathl;

#include "BratDisplay.h"
#include "LabeledTextCtrl.h"
#include "Validators.h"
#include "WPlotPropertyPanel.h"
#include "ContourPropFrame.h"
#include "PlotData/GeoMap.h"


long ID_WPLOTPROP_PROJECTION;
long ID_WPLOTPROP_LAT;
long ID_WPLOTPROP_LON;
long ID_WPLOTPROP_GRAN;
long ID_WPLOTPROP_NUMLABELS;
long ID_WPLOTPROP_VSCALE;
long ID_WPLOTPROP_RANGE_MIN;
long ID_WPLOTPROP_RANGE_MAX;

DEFINE_EVENT_TYPE(wxEVT_PROJECTION_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_CENTER_POINT_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_GRIDLABELS_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_RANGE_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_NUM_LABELS_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_VECTOR_SCALE_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_VIEW_STATE_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_VIEW_STATE_COMMAND)
DEFINE_EVENT_TYPE(wxEVT_UPDATE_RANGE_COMMAND)
DEFINE_EVENT_TYPE(wxEVT_ZOOM_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_RADIUS_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_FACTOR_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_DISPLAY_DATA_ATTR_CHANGED)



//#include "wx/arrimpl.cpp"
//WX_DEFINE_OBJARRAY(wxArrayBoxSizer);
//WX_DEFINE_OBJARRAY(wxArrayStaticBoxSizer); //don't compile because of DECLARE_NO_COPY_CLASS in class wxStaticBoxSizer


// WDR: class implementations

//----------------------------------------------------------------------------
// CWPlotPropertyPanel
//----------------------------------------------------------------------------

// WDR: event table for CWPlotPropertyPanel

BEGIN_EVENT_TABLE(CWPlotPropertyPanel,wxScrolledWindow)
    //EVT_CHOICE( ID_WPLOTPROP_PROJECTION, CWPlotPropertyPanel::OnProjectionChoice )
    EVT_SPINCTRL( ID_VIEWCTRL, CWPlotPropertyPanel::OnSpinView )
    EVT_TEXT_ENTER( ID_VIEWCTRL, CWPlotPropertyPanel::OnViewCtrl )
    EVT_BUTTON( ID_SAVEVIEW, CWPlotPropertyPanel::OnSaveView )
    EVT_BUTTON( ID_CLEARVIEW, CWPlotPropertyPanel::OnClearView )
    EVT_BUTTON( ID_ZOOM_GO, CWPlotPropertyPanel::OnZoom )
    EVT_BUTTON( ID_FULLVIEW, CWPlotPropertyPanel::OnFullView )
    EVT_SPINCTRL( ID_RADIUSCTRL, CWPlotPropertyPanel::OnSpinRadius )
    EVT_SPINCTRL( ID_FACTORCTRL, CWPlotPropertyPanel::OnSpinFactor )
    EVT_TEXT_ENTER( ID_RADIUSCTRL, CWPlotPropertyPanel::OnRadiusCtrl )
    EVT_TEXT_ENTER( ID_FACTORCTRL, CWPlotPropertyPanel::OnFactorCtrl )
END_EVENT_TABLE()

CWPlotPropertyPanel::CWPlotPropertyPanel()

{
  Init();
}

//----------------------------------------
CWPlotPropertyPanel::CWPlotPropertyPanel(wxWindow *parent, vtkActor2D* plotter, CWorldPlotData* plotData, wxVTKRenderWindowInteractor* vtkWidget,
                                         wxWindowID id,
                                         const wxPoint& pos,
                                         const wxSize& size ,
                                         long style)
              // : wxScrolledWindow( parent, -1, pos, size, style ), --> called in Create

{
  Init();

  bool bOk = Create(parent, plotter, plotData, vtkWidget, id, pos, size, style);
  if (bOk == false)
  {
    CException e("ERROR in CWPlotPropertyPanel ctor - Unenable to create wxWidgets (Create method returns false)", BRATHL_SYSTEM_ERROR);
    throw(e);
  }

}

//----------------------------------------

CWPlotPropertyPanel::~CWPlotPropertyPanel()
{
  //DeleteBoxSizers();
}

//----------------------------------------
void CWPlotPropertyPanel::Init()
{
  m_parent = NULL;
  m_finished = false;
  m_vtkWidget = NULL;
  m_plotter = NULL;
  m_plotData = NULL;
  m_lat = 0;
  m_lon = 0;
  m_gran = 5;
  m_min = 0;
  m_max = 0;
  m_nLabels = 8;
  m_projection = 0;
  m_vScale = 10;

}
//----------------------------------------
void CWPlotPropertyPanel::CreateBoxSizers()
{

  m_hSizer = new wxBoxSizer(wxHORIZONTAL);
  m_hSizer2 = new wxBoxSizer(wxHORIZONTAL);
  m_hSizer3 = new wxBoxSizer(wxHORIZONTAL);
  m_hSizer4 = new wxBoxSizer(wxHORIZONTAL);
  m_hSizer5 = new wxBoxSizer(wxHORIZONTAL);
  m_hSizer6 = new wxBoxSizer(wxHORIZONTAL);

  m_vSizer = new wxStaticBoxSizer(m_vStaticSizer, wxVERTICAL);
  m_vSizer2 = new wxStaticBoxSizer(m_vStaticSizer2, wxVERTICAL);
  m_vSizer3 = new wxStaticBoxSizer(m_vStaticSizer3, wxVERTICAL);

  m_sizer = new wxBoxSizer(wxVERTICAL);
  m_mainSizer = new wxBoxSizer(wxVERTICAL);

}

//----------------------------------------
bool CWPlotPropertyPanel::Create(wxWindow *parent, vtkActor2D* plotter, CWorldPlotData* plotData, wxVTKRenderWindowInteractor* vtkWidget,
                                         wxWindowID id,
                                         const wxPoint& pos,
                                         const wxSize& size ,
                                         long style)
{
  bool bOk = true;

  wxScrolledWindow::Create( parent, -1, pos, size, style );

  m_parent = parent;
  m_plotter = plotter;
  m_plotData = plotData;
  m_vtkWidget = vtkWidget;

  bOk = CreateControls();
  if (bOk == false)
  {
    return false;
  }

  CreateLayout();
  InstallToolTips();
  InstallEventListeners();


  GetViewctrl()->SetRange(0, 0);
  GetViewctrl()->SetValue(0);



  ////////////////////////////ProjectionInit();



  return true;
}

//----------------------------------------
bool CWPlotPropertyPanel::CreateControls()
{
  bool bOk = true;
  CFloatValidator   FloatValidator;
  CDigitValidator   DigitValidator;

//  self.projection = visanvtk.projections[3][1]

  ID_WPLOTPROP_PROJECTION = wxNewId();
  ID_WPLOTPROP_LAT = wxNewId();
  ID_WPLOTPROP_LON = wxNewId();
  ID_WPLOTPROP_GRAN = wxNewId();
  ID_WPLOTPROP_RANGE_MIN = wxNewId();
  ID_WPLOTPROP_RANGE_MAX = wxNewId();
  ID_WPLOTPROP_NUMLABELS = wxNewId();
  ID_WPLOTPROP_VSCALE = wxNewId();

  int32_t charwidth, charheight;
  CBratDisplayApp::DetermineCharSize(this, charwidth, charheight);

  m_vStaticSizer  = new wxStaticBox(this, -1, "Latitude/Longitude");
  m_vStaticSizer2 = new wxStaticBox(this, -1, "Range");
  m_vStaticSizer3 = new wxStaticBox(this, -1, "Data Layers");

  bOk = m_projectionLabel.Create(this, -1,
                           "Projection:",
                           wxDefaultPosition,
                           wxDefaultSize,
                           wxALIGN_RIGHT);
  if (bOk == false)
  {
    return false;
  }

  wxArrayString array;
  NamesToArrayString(*CMapProjection::GetInstance(), array);
  bOk = m_projectionChoice.Create(this, ID_WPLOTPROP_PROJECTION,
                            wxDefaultPosition,
                            wxDefaultSize,
                            array);
  if (bOk == false)
  {
    return false;
  }

  bOk = m_centerLatCtrl.Create(this, &m_finished, ID_WPLOTPROP_LAT,
                         "Center Lat:", "",
                         FloatValidator,
                         "%-#.3g",
                         wxSize(8*charwidth, -1));
  if (bOk == false)
  {
    return false;
  }

  bOk = m_centerLonCtrl.Create(this, &m_finished, ID_WPLOTPROP_LON,
                         "Center Lon:", "",
                         FloatValidator,
                         "%-#.3g",
                         wxSize(8*charwidth, -1));
  if (bOk == false)
  {
    return false;
  }

  bOk = m_gridLabelsCtrl.Create(this, -1, "Show Grid Labels");

  if (bOk == false)
  {
    return false;
  }


  bOk = m_solidColorCtrl.Create(this, -1, "Show Solid Color");
  if (bOk == false)
  {
    return false;
  }

  bOk = m_editColorTableBtn.Create(this, -1, "Edit", wxDefaultPosition, wxSize(50, -1), wxBU_EXACTFIT);
  if (bOk == false)
  {
    return false;
  }

  bOk = m_contourCtrl.Create(this, -1, "Show Contour");
  if (bOk == false)
  {
    return false;
  }
  bOk = m_editContourPropsBtn.Create(this, -1, "Edit", wxDefaultPosition, wxSize(50, -1), wxBU_EXACTFIT);
  if (bOk == false)
  {
    return false;
  }


  bOk = m_numLabelsCtrl.Create(this, &m_finished, ID_WPLOTPROP_NUMLABELS,
                         "Number of Labels (Color Bar):", "",
                         DigitValidator,
                         "",
                         wxSize(3*charwidth, -1));
  if (bOk == false)
  {
    return false;
  }

  bOk = m_VScaleLabel.Create(this, -1,
                           "Vector Scale:",
                           wxDefaultPosition,
                           wxDefaultSize,
                           wxALIGN_RIGHT);
  if (bOk == false)
  {
    return false;
  }

  /*m_numVScaleCtrl = new wxSpinCtrl( this, ID_VECTORSCALE, wxT("3"),
            wxDefaultPosition, wxSize(50,-1), wxSP_ARROW_KEYS|wxSP_WRAP|wxTE_PROCESS_ENTER|wxTE_RIGHT, 1, 20, 3 );*/

  bOk = m_numVScaleCtrl.Create(this, &m_finished, ID_WPLOTPROP_VSCALE,
                        "Vector Scale:", "",
                        FloatValidator,
                        "%#.5g",
                        wxSize(4*charwidth, -1));
  if (bOk == false)
  {
    return false;
  }

  bOk = m_minRangeCtrl.Create(this, &m_finished, ID_WPLOTPROP_RANGE_MIN,
                        "Min:", "",
                        FloatValidator,
                        "%-#.5g",
                        wxSize(8*charwidth, -1));
  if (bOk == false)
  {
    return false;
  }

  bOk = m_maxRangeCtrl.Create(this, &m_finished, ID_WPLOTPROP_RANGE_MAX,
                        "Max:", "",
                        FloatValidator,
                        "%-#.5g",
                        wxSize(8*charwidth, -1));
  if (bOk == false)
  {
    return false;
  }

  bOk = m_resetBtn.Create(this, -1, "Reset", wxDefaultPosition, wxSize(50,-1), wxBU_EXACTFIT);

  ////////////////////m_projectionChoice.SetSelection(3);

  m_centerLatCtrl.SetValue(m_lat, 0.0, -90.0, 90.0);
  m_centerLonCtrl.SetValue(m_lon, 0.0, -180.0, 180.0);
  m_gridLabelsCtrl.SetValue(true);

  m_solidColorCtrl.SetValue(true);
  m_contourCtrl.SetValue(false);

  m_numLabelsCtrl.SetValue(m_nLabels, 2, 2);
  m_numVScaleCtrl.SetValue(m_vScale, m_vScale, 50.0);
  //m_numVScaleCtrl->SetValue(3);

  m_minRangeCtrl.SetValue(m_min);
  m_maxRangeCtrl.SetValue(m_max);


  bOk = m_layerLabel.Create(this, -1,
                           "Layer:",
                           wxDefaultPosition,
                           wxDefaultSize,
                           wxALIGN_RIGHT);
  if (bOk == false)
  {
    return false;
  }

  bOk = m_layerChoice.Create(this, -1,
                            wxDefaultPosition,
                            wxSize(200, -1),
                            0);
  if (bOk == false)
  {
    return false;
  }


  return true;

}

//----------------------------------------
void CWPlotPropertyPanel::CreateLayout()
{
  CreateBoxSizers();

  m_hSizer->Add(&m_projectionLabel, 0, wxALIGN_CENTER|wxRIGHT, 10);
  m_hSizer->Add(&m_projectionChoice, 0, wxALIGN_CENTER);

  m_hSizer2->Add(10, 0, 0);
  m_hSizer2->Add(&m_centerLatCtrl, 0, wxRIGHT, 10);
  m_hSizer2->Add(&m_centerLonCtrl, 0);

  m_hSizer3->Add(5, 0, 0);
  m_hSizer3->Add(&m_minRangeCtrl, 0, wxRIGHT, 5);
  m_hSizer3->Add(&m_maxRangeCtrl, 0, wxRIGHT, 5);
  m_hSizer3->Add(&m_resetBtn, 0);

  m_hSizer4->Add(&m_layerLabel, 0, wxALIGN_CENTER|wxRIGHT, 10);
  m_hSizer4->Add(&m_layerChoice, 0, wxALIGN_CENTER);

  m_hSizer5->Add(&m_solidColorCtrl, 0, wxALIGN_CENTER|wxRIGHT, 5);
  m_hSizer5->Add(&m_editColorTableBtn, 0, wxALIGN_CENTER|wxRIGHT, 5);
  m_hSizer5->Add(&m_contourCtrl, 0, wxALIGN_CENTER|wxRIGHT, 5);
  m_hSizer5->Add(&m_editContourPropsBtn, 0, wxALIGN_CENTER);

  m_vSizer->Add(0, 5, 0);
  m_vSizer->Add(m_hSizer2, 1, wxLEFT|wxRIGHT, 5);
  m_vSizer->Add(0, 5, 0);
  m_vSizer->Add(&m_gridLabelsCtrl, 1, wxLEFT|wxRIGHT, 10);

  m_vSizer2->Add(0, 5, 0);
  m_vSizer2->Add(m_hSizer3, 1, wxLEFT|wxRIGHT, 5);
  //m_vSizer2->Add(0, 5, 0);
  //m_vSizer2->Add(&m_resetBtn, 0, wxCENTER, 5);
  //m_vSizer2->Add(0, 5, 0);

  m_vSizer3->Add(0, 10, 0);
  m_vSizer3->Add(m_hSizer4, 0, wxLEFT|wxRIGHT, 5);
  m_vSizer3->Add(0, 10, 0);
  m_vSizer3->Add(m_hSizer5, 0, wxLEFT|wxRIGHT, 5);
  m_vSizer3->Add(0, 10, 0);
  m_vSizer3->Add(&m_numLabelsCtrl, 0, wxLEFT|wxRIGHT, 5);
  m_vSizer3->Add(0, 10, 0);

  m_hSizer6->Add(&m_numVScaleCtrl, 0, wxALIGN_CENTER, 5);
  m_hSizer6->Add(0,10,0);
  //m_hSizer6->Add(&m_GlyphCtrl, 0, wxALIGN_CENTER|wxLEFT, 10);
  m_vSizer3->Add(m_hSizer6, 1, wxLEFT|wxRIGHT, 5);

  m_vSizer3->Add(m_vSizer2);
  m_vSizer3->Add(0, 5, 0);


  m_sizer->Add(m_hSizer, 0, wxEXPAND);
  m_sizer->Add(0, 5, 0);
  m_sizer->Add(m_vSizer, 0, wxEXPAND);
  m_sizer->Add(0, 5, 0);
  m_sizer->Add(m_vSizer3, 0, wxEXPAND);
  m_sizer->Add(0, 5, 0);
  wxSizer* plotViewSizer = WPlotView(this, false, false);
  m_sizer->Add(plotViewSizer, 0, wxEXPAND);

  m_mainSizer->Add(m_sizer, 1, wxEXPAND|wxALL, 10);

  SetSizerAndFit(m_mainSizer);

}
//----------------------------------------
void CWPlotPropertyPanel::InstallToolTips()
{
  m_projectionChoice.SetToolTip("Projection to use for mapping the world globe to the 2-dimensional plane.");
  m_centerLatCtrl.SetToolTip("Latitude of the projection's center point.");
  m_centerLonCtrl.SetToolTip("Longitude of the projection's center point.");
  m_numLabelsCtrl.SetToolTip("Nr of labels shown on the plot's color bar.");
  m_numVScaleCtrl.SetToolTip("Scaling Factor for Vectors");
  m_minRangeCtrl.SetToolTip("Minimium color table value to use in plot.");
  m_maxRangeCtrl.SetToolTip("Maximum color table value to use in plot.");
  m_resetBtn.SetToolTip("Set range (min./max) with original values.");
  m_layerChoice.SetToolTip("Layer to show or modify its properties.");
  m_solidColorCtrl.SetToolTip("Display or not data values as solid color.");
  m_contourCtrl.SetToolTip("Display or not data values as contour (isosurfaces/isolines).");
  m_editColorTableBtn.SetToolTip("Show an editor window for the current Color Table.");
  m_editContourPropsBtn.SetToolTip("Show an editor window for the current Contour Properties.");
}

//----------------------------------------
void CWPlotPropertyPanel::EvtButtonClicked(wxWindow& window, const wxCommandEventFunction& method, wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_COMMAND_BUTTON_CLICKED,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);

}
//----------------------------------------
void CWPlotPropertyPanel::EvtCheckBox(wxWindow& window, const wxCommandEventFunction& method, wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_COMMAND_CHECKBOX_CLICKED,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);

}
//----------------------------------------
void CWPlotPropertyPanel::EvtDisplayDataAttrChanged(wxWindow& window, const CDisplayDataAttrChangedEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_DISPLAY_DATA_ATTR_CHANGED,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}

//----------------------------------------
void CWPlotPropertyPanel::EvtProjectionChanged(wxWindow& window, const CProjectionChangedEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_PROJECTION_CHANGED,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}

//----------------------------------------
void CWPlotPropertyPanel::EvtCenterPointChanged(wxWindow& window, const CCenterPointChangedEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_CENTER_POINT_CHANGED,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}


//----------------------------------------
void CWPlotPropertyPanel::EvtRangeChanged(wxWindow& window, const CRangeChangedEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_RANGE_CHANGED,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}



//----------------------------------------
void CWPlotPropertyPanel::EvtNumLabelsChanged(wxWindow& window, const CNumLabelsChangedEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_NUM_LABELS_CHANGED,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}

//----------------------------------------
void CWPlotPropertyPanel::EvtVScaleChanged(wxWindow& window, const CVectorScaleChangedEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_VECTOR_SCALE_CHANGED,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}

//----------------------------------------
void CWPlotPropertyPanel::EvtUpdateRangeCommand(wxWindow& window, const CUpdateRangeEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_UPDATE_RANGE_COMMAND,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}
//----------------------------------------
void CWPlotPropertyPanel::EvtViewStateChanged(wxWindow& window, const CViewStateChangedEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_VIEW_STATE_CHANGED,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}

//----------------------------------------
void CWPlotPropertyPanel::EvtViewStateCommand(wxWindow& window, const CViewStateCommandEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_VIEW_STATE_COMMAND,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}
//----------------------------------------
void CWPlotPropertyPanel::EvtZoomChangedCommand(wxWindow& window, const CZoomChangedEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_ZOOM_CHANGED,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}
//----------------------------------------
void CWPlotPropertyPanel::EvtFactorChangedCommand(wxWindow& window, const CFactorChangedEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_FACTOR_CHANGED,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}
//----------------------------------------
void CWPlotPropertyPanel::EvtRadiusChangedCommand(wxWindow& window, const CRadiusChangedEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_RADIUS_CHANGED,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}
//----------------------------------------
void CWPlotPropertyPanel::EvtChoice(wxWindow& window, const wxCommandEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_COMMAND_CHOICE_SELECTED,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}
//----------------------------------------
void CWPlotPropertyPanel::InstallEventListeners()
{
  CLabeledTextCtrl::EvtValueChanged(*this,
                                     ID_WPLOTPROP_LAT,
                                     (CValueChangedEventFunction)&CWPlotPropertyPanel::OnCenterLat);

  CLabeledTextCtrl::EvtValueChanged(*this,
                                     ID_WPLOTPROP_LON,
                                     (CValueChangedEventFunction)&CWPlotPropertyPanel::OnCenterLon);


  CLabeledTextCtrl::EvtValueChanged(*this,
                                     ID_WPLOTPROP_RANGE_MIN,
                                     (CValueChangedEventFunction)&CWPlotPropertyPanel::OnMinRange);

  CLabeledTextCtrl::EvtValueChanged(*this,
                                     ID_WPLOTPROP_RANGE_MAX,
                                     (CValueChangedEventFunction)&CWPlotPropertyPanel::OnMaxRange);

  CLabeledTextCtrl::EvtValueChanged(*this,
                                     ID_WPLOTPROP_NUMLABELS,
                                     (CValueChangedEventFunction)&CWPlotPropertyPanel::OnNumLabels);

  CLabeledTextCtrl::EvtValueChanged(*this,
                                     ID_WPLOTPROP_VSCALE,
                                     (CValueChangedEventFunction)&CWPlotPropertyPanel::OnVScale);

  CWPlotPropertyPanel::EvtCenterPointChanged(*this,
                                             (CCenterPointChangedEventFunction)&CWPlotPropertyPanel::OnCenterPointChanged);

  CWPlotPropertyPanel::EvtCheckBox(m_gridLabelsCtrl,
                            (wxCommandEventFunction)&CWPlotPropertyPanel::OnGridLabels, NULL, this);

  CWPlotPropertyPanel::EvtProjectionChanged(*this,
                                            (CProjectionChangedEventFunction)&CWPlotPropertyPanel::OnProjectionChanged);

  CWPlotPropertyPanel::EvtRangeChanged(*this,
                                            (CRangeChangedEventFunction)&CWPlotPropertyPanel::OnRangeChanged);

  CWPlotPropertyPanel::EvtNumLabelsChanged(*this,
                                            (CNumLabelsChangedEventFunction)&CWPlotPropertyPanel::OnNumLabelsChanged);

  CWPlotPropertyPanel::EvtChoice(m_projectionChoice,
                                            (wxCommandEventFunction)&CWPlotPropertyPanel::OnProjectionChoice,
                                            NULL,
                                            this);

  CWPlotPropertyPanel::EvtChoice(m_layerChoice,
                                        (wxCommandEventFunction)&CWPlotPropertyPanel::OnLayerChoice,
                                        NULL,
                                        this);

  CWPlotPropertyPanel::EvtButtonClicked(m_resetBtn,
                            (wxCommandEventFunction)&CWPlotPropertyPanel::OnReset, NULL, this);

  CWPlotPropertyPanel::EvtCheckBox(m_solidColorCtrl,
                            (wxCommandEventFunction)&CWPlotPropertyPanel::OnSolidColor, NULL, this);

  CWPlotPropertyPanel::EvtCheckBox(m_contourCtrl,
                            (wxCommandEventFunction)&CWPlotPropertyPanel::OnContour, NULL, this);

  CWPlotPropertyPanel::EvtButtonClicked(m_editColorTableBtn,
                            (wxCommandEventFunction)&CWPlotPropertyPanel::OnEditColorTable, NULL, this);

  CWPlotPropertyPanel::EvtButtonClicked(m_editContourPropsBtn,
                            (wxCommandEventFunction)&CWPlotPropertyPanel::OnEditContourProperties, NULL, this);

}

//----------------------------------------
void CWPlotPropertyPanel::OnGridLabels(wxCommandEvent& event)
{

  GetCurrentLayer()->m_plotProperty.m_gridLabel = m_gridLabelsCtrl.GetValue();

  CDisplayDataAttrChangedEvent ev(GetId(),
                                  CDisplayDataAttrChangedEvent::displayGridLabels,
                                  m_gridLabelsCtrl.GetValue());
  wxPostEvent(GetParent(), ev);
}

//----------------------------------------
void CWPlotPropertyPanel::OnSolidColor(wxCommandEvent& event)
{

  GetCurrentLayer()->m_plotProperty.m_solidColor = m_solidColorCtrl.GetValue();
  GetCurrentLayer()->m_plotProperty.m_withContour = m_contourCtrl.GetValue();

  CDisplayDataAttrChangedEvent ev(GetId(),
                                  CDisplayDataAttrChangedEvent::displaySolidColor,
                                  m_solidColorCtrl.GetValue());
  wxPostEvent(GetParent(), ev);
}

//----------------------------------------
void CWPlotPropertyPanel::OnContour(wxCommandEvent& event)
{

  GetCurrentLayer()->m_plotProperty.m_withContour = m_contourCtrl.GetValue();

  CDisplayDataAttrChangedEvent ev(GetId(),
                                  CDisplayDataAttrChangedEvent::displayContour,
                                  m_contourCtrl.GetValue());
  wxPostEvent(GetParent(), ev);
}
//----------------------------------------
void CWPlotPropertyPanel::OnReset(wxCommandEvent& event)
{
  UpdateValues();
}
//----------------------------------------
void CWPlotPropertyPanel::OnEditContourProperties(wxCommandEvent& event)
{
  wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, ID_MENU_VIEW_CONTOUR_PROPS);
  wxPostEvent(GetParent()->GetParent(), evt);

}
//----------------------------------------
void CWPlotPropertyPanel::OnEditColorTable(wxCommandEvent& event)
{
  wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, ID_MENU_VIEW_CLUTEDIT);
  wxPostEvent(GetParent()->GetParent(), evt);
}
//----------------------------------------
void CWPlotPropertyPanel::OnCenterLat(CValueChangedEvent& event)
{
    //wxString msg = event.m_value;
    //wxLogMessage(msg);
  double nlat = 0;

  try
  {
    m_centerLatCtrl.GetValue(nlat, 0.0, -90.0, 90.0);
  }
  catch (...)
  {
    wxBell();
    m_centerLatCtrl.SetValue(m_lat, 0.0, -90.0, 90.0);
    return;
  }

  if (nlat != m_lat)
  {
    m_lat = nlat;
    CCenterPointChangedEvent ev(GetId(),
                m_lat,
                m_lon);
    wxPostEvent(m_parent, ev);
  }
}

//----------------------------------------
void CWPlotPropertyPanel::OnCenterLon(CValueChangedEvent& event)
{
    //wxString msg = event.m_value;
    //wxLogMessage(msg);
  double nlon = 0;

  try
  {
    m_centerLonCtrl.GetValue(nlon, 0.0, -180.0, 180.0);
  }
  catch (...)
  {
    wxBell();
    m_centerLonCtrl.SetValue(m_lon, 0.0, -180.0, 180.0);
    return;
  }

  if (nlon != m_lon)
  {
    m_lon = nlon;
    CCenterPointChangedEvent ev(GetId(),
                m_lat,
                m_lon);
    wxPostEvent(m_parent, ev);
  }
}

//----------------------------------------
void CWPlotPropertyPanel::OnMinRange(CValueChangedEvent& event)
{
    //wxString msg = event.m_value;
    //wxLogMessage(msg);
  double nmin = 0;

  try
  {
    m_minRangeCtrl.GetValue(nmin);
  }
  catch (...)
  {
    wxBell();
    m_minRangeCtrl.SetValue(m_min);

    return;
  }

  CGeoVelocityMap *gMap = dynamic_cast<CGeoVelocityMap *>(GetCurrentLayer());
  if ( gMap )
  {
    m_minRangeCtrl.SetValue(0);
    m_min = 0;
    return;
  }

  if (nmin != m_min)
  {
    if (nmin <= m_max)
    {
      m_min = nmin;

    }
    else
    {
      wxBell();
      double tmp = m_max;
      m_max = nmin;
      m_min = tmp;
      m_minRangeCtrl.SetValue(m_min);
      m_maxRangeCtrl.SetValue(m_max);
    }
    GetCurrentLayer()->GetLookupTable()->SetTableRange(m_min, m_max);
    GetCurrentLayer()->GetColorBarRenderer()->GetLookupTable()->SetTableRange(m_min, m_max);

    CRangeChangedEvent ev(GetId(),
                          m_min,
                          m_max);

    wxPostEvent(m_parent, ev);

  }
}

//----------------------------------------
void CWPlotPropertyPanel::OnMaxRange(CValueChangedEvent& event)
{
    //wxString msg = event.m_value;
    //wxLogMessage(msg);
  double nmax = 0;

  try
  {
    m_maxRangeCtrl.GetValue(nmax);
  }
  catch (...)
  {
    wxBell();
    m_maxRangeCtrl.SetValue(m_max);
    return;
  }

    CGeoVelocityMap *gMap = dynamic_cast<CGeoVelocityMap *>(GetCurrentLayer());
    if ( gMap )
    {
      if ( nmax < 0  )
      {
          wxBell();
          m_maxRangeCtrl.SetValue(m_max);
          return;
      }
      else
      {
          gMap->GetGlyphFilter()->SetMaxV(nmax);
      }
    }


  if (nmax != m_max)
  {
    if (nmax >= m_min)
    {
      m_max = nmax;
    }
    else
    {
      wxBell();
      double tmp = m_min;
      m_min = nmax;
      m_max = tmp;

      m_minRangeCtrl.SetValue(m_min);
      m_maxRangeCtrl.SetValue(m_max);

    }

    GetCurrentLayer()->GetLookupTable()->SetTableRange(m_min, m_max);
    GetCurrentLayer()->GetColorBarRenderer()->GetLookupTable()->SetTableRange(m_min, m_max);

    CRangeChangedEvent ev(GetId(),
                          m_min,
                          m_max);
    wxPostEvent(m_parent, ev);

  }
}

//----------------------------------------
void CWPlotPropertyPanel::OnNumLabels(CValueChangedEvent& event)
{
    //wxString msg = event.m_value;
    //wxLogMessage(msg);
  int32_t nLabels = 0;

  try
  {
    m_numLabelsCtrl.GetValue(nLabels, 2, 2);
  }
  catch (...)
  {
    wxBell();
    m_numLabelsCtrl.SetValue(m_nLabels, 2, 2);
    GetCurrentLayer()->GetColorBarRenderer()->SetNumberOfLabels(m_nLabels);

    return;
  }

  if (nLabels != m_nLabels)
  {
    m_nLabels = nLabels;

    GetCurrentLayer()->GetColorBarRenderer()->SetNumberOfLabels(m_nLabels);

    CNumLabelsChangedEvent ev(GetId(),
                              m_nLabels);
    wxPostEvent(m_parent, ev);

  }
}


//----------------------------------------
void CWPlotPropertyPanel::OnVScale(CValueChangedEvent& event)
{
    //wxString msg = event.m_value;
    //wxLogMessage(msg);
  double vScale = 0;


  try
  {
    m_numVScaleCtrl.GetValue(vScale, 10.0, 0.1, 50.0);
  }
  catch (...)
  {
    wxBell();
    m_vScale = 10;
    m_numVScaleCtrl.SetValue(m_vScale, m_vScale, 0.1, 50.0);

    return;
  }

  if (vScale != m_vScale)
  {
    m_vScale = vScale;

    CGeoVelocityMap *vmap = dynamic_cast<CGeoVelocityMap *> (GetCurrentLayer());
    if ( vmap != NULL ) {

            vtkVelocityGlyphFilter *vFilter = vmap->GetGlyphFilter();
            if ( vFilter ) {

                vFilter->SetOffsetLatitude(m_vScale);
                vFilter->SetOffsetLongitude(m_vScale);

                GetCurrentLayer()->Update();

                CVectorScaleChangedEvent ev(GetId(),
                                          m_vScale);
                wxPostEvent(m_parent, ev);
            }
    }

  }
}

//----------------------------------------
void CWPlotPropertyPanel::ProjectionInit()
{
  int32_t n = m_projectionChoice.GetSelection();
  wxString wxstr = m_projectionChoice.GetString(n);
  m_projection = CMapProjection::GetInstance()->NameToId(wxstr.c_str());

  CProjectionChangedEvent ev(GetId(),
                 m_projection);
  wxPostEvent(m_parent, ev);

}
//----------------------------------------
// WDR: handler implementations for CWPlotPropertyPanel

//----------------------------------------
void CWPlotPropertyPanel::OnFactorCtrl( wxCommandEvent &event )
{
  GetFactorctrl()->SetValue(GetFactorctrl()->GetValue());

  FactorChanged();

}

//----------------------------------------
void CWPlotPropertyPanel::OnRadiusCtrl( wxCommandEvent &event )
{
  GetRadiusctrl()->SetValue(GetRadiusctrl()->GetValue());

  RadiusChanged();

}

//----------------------------------------
void CWPlotPropertyPanel::OnSpinFactor( wxSpinEvent &event )
{
  FactorChanged();
}

//----------------------------------------
void CWPlotPropertyPanel::OnSpinRadius( wxSpinEvent &event )
{
  RadiusChanged();
}
//----------------------------------------
void CWPlotPropertyPanel::FactorChanged()
{
  CFactorChangedEvent ev(GetId(), GetFactorctrl()->GetValue());
  wxPostEvent(GetParent(), ev);
}

//----------------------------------------
void CWPlotPropertyPanel::RadiusChanged()
{
  CRadiusChangedEvent ev(GetId(), GetRadiusctrl()->GetValue());
  wxPostEvent(GetParent(), ev);
}

//----------------------------------------
void CWPlotPropertyPanel::OnFullView( wxCommandEvent &event )
{
  FullView();
}
//----------------------------------------
void CWPlotPropertyPanel::FullView()
{
  GetViewctrl()->SetValue(0);

  ViewChanged();

}
//----------------------------------------

void CWPlotPropertyPanel::OnZoom( wxCommandEvent &event )
{
  double lon1, lon2, lat1, lat2;

  GetZoomLon1()->GetValue(lon1, -180.0, -180.0, 180.0);
  GetZoomLon2()->GetValue(lon2, 180.0, -180.0, 180.0);
  GetZoomLat1()->GetValue(lat1, -90.0, -90.0, 90.0);
  GetZoomLat2()->GetValue(lat2, 90.0, -90.0, 90.0);

  if (CTools::IsLongitudeCircular(lon1, lon2, 2.0))
  {
    FullView();
    return;
  }

  CZoomChangedEvent ev(GetId(),
                       lat1,
                       lon1,
                       lat2,
                       lon2);

  wxPostEvent(GetParent(), ev);



}
//----------------------------------------

void CWPlotPropertyPanel::OnClearView( wxCommandEvent &event )
{
  CViewStateCommandEvent ev(GetId(), CViewStateCommandEvent::viewClear);
  wxPostEvent(GetParent(), ev);

}
//----------------------------------------

void CWPlotPropertyPanel::OnSaveView( wxCommandEvent &event )
{
  CViewStateCommandEvent ev(GetId(), CViewStateCommandEvent::viewSave);
  wxPostEvent(GetParent(), ev);

}
//----------------------------------------

void CWPlotPropertyPanel::OnViewCtrl( wxCommandEvent &event )
{
  GetViewctrl()->SetValue(GetViewctrl()->GetValue());

  ViewChanged();
}
//----------------------------------------

void CWPlotPropertyPanel::OnSpinView( wxSpinEvent &event )
{
  ViewChanged();
}
//----------------------------------------
void CWPlotPropertyPanel::ViewChanged()
{
  CViewStateChangedEvent ev(GetId(), GetViewctrl()->GetValue());
  wxPostEvent(GetParent(), ev);
}
//----------------------------------------
CGeoMap* CWPlotPropertyPanel::GetCurrentLayer()
{
  int32_t n = m_layerChoice.GetSelection();
  if (n < 0)
  {
    return NULL;
  }
  return static_cast<CGeoMap*>(m_layerChoice.GetClientData(n));
}
//----------------------------------------
void CWPlotPropertyPanel::SetCurrentLayer(int32_t index)
{
  if (m_layerChoice.GetCount() <= 0)
  {
    return;
  }

  m_layerChoice.SetSelection(index);

  CGeoMap* geoMap =  static_cast<CGeoMap*>(m_layerChoice.GetClientData(index));
  SetCurrentLayer(geoMap);

}
//----------------------------------------
void CWPlotPropertyPanel::SetCurrentLayer( CGeoMap* geoMap )
{
  if (geoMap == NULL)
  {
      throw CException("ERROR in  CWPlotPropertyPanel::SetCurrentLayer : static_cast<CGeoMap*>(event.GetClientData()) returns NULL pointer - ",
                       BRATHL_LOGIC_ERROR);
  }

   CGeoVelocityMap *vMap = dynamic_cast<CGeoVelocityMap*>(geoMap);
   if ( vMap == NULL ) {
        m_numVScaleCtrl.Enable(false);
   }
   else {
       m_numVScaleCtrl.Enable(true);
   }


  m_nLabels = GetCurrentLayer()->GetColorBarRenderer()->GetNumberOfLabels();

  m_numLabelsCtrl.SetValue(m_nLabels, 2, 2);

  vtkLookupTable* vtkLUT = geoMap->GetLookupTable();
  if (vtkLUT == NULL)
  {
    throw CException("ERROR in  CWPlotPropertyPanel::OnLayerChoice : geoMap->GetLookupTable() returns NULL pointer - ",
                     BRATHL_LOGIC_ERROR);
   }


  m_min = vtkLUT->GetTableRange()[0];
  m_max = vtkLUT->GetTableRange()[1];
  m_minRangeCtrl.SetValue(m_min);
  m_maxRangeCtrl.SetValue(m_max);

  m_solidColorCtrl.SetValue(GetCurrentLayer()->m_plotProperty.m_solidColor);
  m_contourCtrl.SetValue(GetCurrentLayer()->m_plotProperty.m_withContour);

}

void CWPlotPropertyPanel::SetCenterPoint(double lat, double lon){
    m_lat = lat;
    m_lon = lon;

   m_centerLatCtrl.SetValue(m_lat, 0.0, -90.0, 90.0);
   m_centerLonCtrl.SetValue(m_lon, 0.0, -180.0, 180.0);
}

//----------------------------------------
void CWPlotPropertyPanel::OnLayerChoice( wxCommandEvent &event )
{

  // Close lutFrame if opened
  CLutFrameCloseEvent evtLut(GetId());
  wxPostEvent(GetParent(), evtLut);

  // Close contour properties frame  if opened
  CContourPropFrameCloseEvent evtContour(GetId());
  wxPostEvent(GetParent(), evtContour);

  CGeoMap* geoMap = static_cast<CGeoMap*>(event.GetClientData());
  if (geoMap == NULL)
  {
      throw CException("ERROR in  CWPlotPropertyPanel::OnLayerChoice : static_cast<CGeoMap*>(event.GetClientData()) returns NULL pointer - ",
                       BRATHL_LOGIC_ERROR);
  }

  SetCurrentLayer(geoMap);

}

//----------------------------------------
void CWPlotPropertyPanel::OnProjectionChoice( wxCommandEvent &event )
{
  int32_t nprojection = CMapProjection::GetInstance()->NameToId(event.GetString().c_str());

  if (nprojection != m_projection)
  {
    m_projection = nprojection;
    CProjectionChangedEvent ev(GetId(),
                   m_projection);
    wxPostEvent(m_parent, ev);
  }

}

//----------------------------------------
void CWPlotPropertyPanel::OnCenterPointChanged(CCenterPointChangedEvent& event)
{
  m_lat = event.m_lat;
  m_lon = event.m_lon;
  m_centerLatCtrl.SetValue(m_lat, 0.0, -90.0, 90.0);
  m_centerLonCtrl.SetValue(m_lon, 0.0, -180.0, 180.0);
}

//----------------------------------------
void CWPlotPropertyPanel::OnProjectionChanged(CProjectionChangedEvent& event)
{
  m_projection = event.m_projection;
  // Next fout lines are workaround for weird, weird, GTK
  // problem in which the default choice selection remains
  // grayed out when another one is set through this method.
  // Only manually rebuilding the entire damn choicebox std::list
  // seems to work...
  m_projectionChoice.Clear();
  wxArrayString array;
  NamesToArrayString(*CMapProjection::GetInstance(), array);
  m_projectionChoice.Append(array);
  //---------------------------------

  m_projectionChoice.SetStringSelection(CMapProjection::GetInstance()->IdToName(m_projection).c_str());

}

//----------------------------------------
void CWPlotPropertyPanel::OnRangeChanged(CRangeChangedEvent& event)
{
  m_min = event.m_min;
  m_max = event.m_max;
  m_maxRangeCtrl.SetValue(m_max);
  m_minRangeCtrl.SetValue(m_min);

  CGeoVelocityMap *gMap = dynamic_cast<CGeoVelocityMap *>(GetCurrentLayer());
  if ( gMap )
  {

      if ( m_min < 0 || m_max < 0 )
      {
          UpdateValues();
          return;
      }

     gMap->GetGlyphFilter()->SetMaxV(m_max);
     printf("VelocityMap...range changed\n"); fflush(NULL);
  }

  printf("normal..\n"); fflush(NULL);

  GetCurrentLayer()->GetLookupTable()->SetTableRange(m_min, m_max);
  GetCurrentLayer()->GetColorBarRenderer()->GetLookupTable()->SetTableRange(m_min, m_max);

}

//----------------------------------------
void CWPlotPropertyPanel::OnNumLabelsChanged(CNumLabelsChangedEvent& event)
{
  m_numLabelsCtrl.SetValue(event.m_nLabels, 2, 2);

  m_numLabelsCtrl.GetValue(m_nLabels);

  GetCurrentLayer()->GetColorBarRenderer()->SetNumberOfLabels(m_nLabels);

}
//----------------------------------------
void CWPlotPropertyPanel::LutChanged( CLutChangedEvent &event )
{

  GetCurrentLayer()->SetLUT(event.m_LUT); // make a copy of lut

  GetCurrentLayer()->GetLookupTable()->SetTableRange(m_min,
                                                     m_max);

  GetCurrentLayer()->GetColorBarRenderer()->SetLUT(event.m_LUT); // make a copy of lut
  GetCurrentLayer()->GetColorBarRenderer()->GetLookupTable()->SetTableRange(m_min, m_max);

}

//----------------------------------------
void CWPlotPropertyPanel::GetRange(double& min, double& max)
{
  min = GetRangeMin();
  max = GetRangeMax();

}

//----------------------------------------
double CWPlotPropertyPanel::GetRangeMin()
{
  double min = 0.0;
  m_minRangeCtrl.GetValue(min);
  return min;
}

//----------------------------------------
double CWPlotPropertyPanel::GetRangeMax()
{
  double max = 0.0;
  m_maxRangeCtrl.GetValue(max);
  return max;
}

//----------------------------------------
void CWPlotPropertyPanel::UpdateValues()
{
  CUpdateRangeEvent ev(GetId());

  wxPostEvent(GetParent(), ev);

//m_wplotPanel->UpdateValues();
}


