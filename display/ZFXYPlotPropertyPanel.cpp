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
    #pragma implementation "ZFXYPlotPropertyPanel.h"
#endif


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "Exception.h"
using namespace brathl;

#include "vtkInteractorObserver.h"

#include "BratDisplayApp.h"
#include "LabeledTextCtrl.h"
#include "Validators.h"
#include "ZFXYPlotPropertyPanel.h"
#include "ZFXYContourPropFrame.h"


DEFINE_EVENT_TYPE(wxEVT_ZFXY_PROJECTION_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_ZFXY_CENTER_POINT_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_ZFXY_RANGE_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_ZFXY_NUM_LABELS_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_ZFXY_VIEW_STATE_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_ZFXY_VIEW_STATE_COMMAND)
DEFINE_EVENT_TYPE(wxEVT_ZFXY_UPDATE_RANGE_COMMAND)
DEFINE_EVENT_TYPE(wxEVT_ZFXY_ZOOM_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_ZFXY_RADIUS_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_ZFXY_FACTOR_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_ZFXY_DISPLAY_DATA_ATTR_CHANGED)




// WDR: class implementations

//----------------------------------------------------------------------------
// CZFXYPlotPropertyPanel
//----------------------------------------------------------------------------


const uint32_t CZFXYPlotPropertyPanel::m_DIGITS_DEF = 4;
const uint32_t CZFXYPlotPropertyPanel::m_DIGITS_MIN = 1;
const uint32_t CZFXYPlotPropertyPanel::m_DIGITS_MAX = 20;

// WDR: event table for CZFXYPlotPropertyPanel

BEGIN_EVENT_TABLE(CZFXYPlotPropertyPanel,wxScrolledWindow)
    EVT_CHOICE( ID_ZFXYLAYERCHOICE, CZFXYPlotPropertyPanel::OnLayerChoice )
    EVT_BUTTON( ID_ZFXYRESETRANGE, CZFXYPlotPropertyPanel::OnReset )
    EVT_BUTTON( ID_ZFXYEDITSOLIDCOLOR, CZFXYPlotPropertyPanel::OnEditColorTable )
    EVT_BUTTON( ID_ZFXYEDITCONTOUR, CZFXYPlotPropertyPanel::OnEditContourProperties )
    EVT_CHECKBOX( ID_ZFXYSHOWSOLIDCOLOR, CZFXYPlotPropertyPanel::OnSolidColor )
    EVT_CHECKBOX( ID_ZFXYSHOWCONTOUR, CZFXYPlotPropertyPanel::OnContour )
    EVT_BUTTON( ID_ZFXYRESETXRANGE, CZFXYPlotPropertyPanel::OnXReset )
    EVT_BUTTON( ID_ZFXYRESETYRANGE, CZFXYPlotPropertyPanel::OnYReset )
    EVT_CHECKBOX( ID_ZFXYLOGX, CZFXYPlotPropertyPanel::OnXLog )
    EVT_CHECKBOX( ID_ZFXYLOGY, CZFXYPlotPropertyPanel::OnYLog )
    EVT_BUTTON( ID_ZFXYFULLVIEW, CZFXYPlotPropertyPanel::OnFullView )
END_EVENT_TABLE()

CZFXYPlotPropertyPanel::CZFXYPlotPropertyPanel()

{
  Init();
}

//----------------------------------------
CZFXYPlotPropertyPanel::CZFXYPlotPropertyPanel(wxWindow *parent, vtkZFXYPlotActor* plotter, CZFXYPlotData* plotData, wxVTKRenderWindowInteractor* vtkWidget,
                                         wxWindowID id,
                                         const wxPoint& pos,
                                         const wxSize& size ,
                                         long style) 
                  : wxScrolledWindow( parent, id, pos, size, style )


{
  Init();

  wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

  //----------------------------
  ZFXYDataProperties(this, false, true);
  //----------------------------

  item0->Add( this, 0, wxGROW|wxALL, 5 );

  parent->SetSizer( item0 );


  m_parent = parent;
  m_plotter = plotter;
  m_plotData = plotData;
  m_vtkWidget = vtkWidget;
 
  InstallToolTips();
  InstallEventListeners();


}

//----------------------------------------

CZFXYPlotPropertyPanel::~CZFXYPlotPropertyPanel()
{
  if (m_vtkComputeRangeCallback != NULL)
  {
    m_vtkComputeRangeCallback->Delete();
    m_vtkComputeRangeCallback = NULL;
  }


}

//----------------------------------------
void CZFXYPlotPropertyPanel::Init()
{
  m_parent = NULL;

  m_zoomToDataRange = false;
  m_currentFrame = 0;

  m_vtkComputeRangeCallback = CVtkZFXYComputeRangeCallback::New(this);

  m_vtkWidget = NULL;
  m_plotter = NULL;
  m_plotData = NULL;
  m_min = 0;
  m_max = 0;
  m_nLabels = 8;
  m_numberOfXDigits = CZFXYPlotPropertyPanel::m_DIGITS_DEF;
  m_numberOfYDigits = CZFXYPlotPropertyPanel::m_DIGITS_DEF;


}

//----------------------------------------
bool CZFXYPlotPropertyPanel::Create(wxWindow *parent, vtkZFXYPlotActor* plotter, CZFXYPlotData* plotData, wxVTKRenderWindowInteractor* vtkWidget,
                                         wxWindowID id,
                                         const wxPoint& pos,
                                         const wxSize& size ,
                                         long style) 
{
  wxScrolledWindow::Create( parent, -1, pos, size, style );
  
  m_parent = parent;
  m_plotter = plotter;
  m_plotData = plotData;
  m_vtkWidget = vtkWidget;
  
  InstallToolTips();
  InstallEventListeners();


  return true;
}

//----------------------------------------
void CZFXYPlotPropertyPanel::InstallToolTips()
{
  GetZfxyplotpropTitle()->SetToolTip("The plot title will be displayed centered at the top of the plot panel.");
  GetZfxynumlabels()->SetToolTip("Number of labels shown on the plot's color bar.");
  GetZfxyrangemin()->SetToolTip("Minimium color table value to use in plot.");
  GetZfxyrangemin()->SetToolTip("Maximum color table value to use in plot.");

  GetZfxytitlex()->SetToolTip("The axis title will be displayed centered on the outside of the axis in the plot panel.");
  GetZfxylogx()->SetToolTip("Use a logarithmic axis. Disabled if the current range of axis values contains the number 0.");
  GetZfxynumticksx()->SetToolTip("The target number of ticks to display on the axis. The actual number of ticks is calculated; this control's value is what the calculation will try to aim for.");
  GetZfxybasex()->SetToolTip("The base for the linear and logarithmic tick calculation for the axis. This value can be fractional but must be greater than 1.");
  GetZfxyrangexmin()->SetToolTip("The minimum fallback range value.");
  GetZfxyrangexmax()->SetToolTip("The maximum fallback range value.");
  GetZfxydigitsx()->SetToolTip("The number of decimal digits to display on the X axis labels.");

  GetZfxytitley()->SetToolTip("The axis title will be displayed centered on the outside of the axis in the plot panel.");
  GetZfxylogy()->SetToolTip("Use a logarithmic axis. Disabled if the current range of axis values contains the number 0.");
  GetZfxynumticksy()->SetToolTip("The target number of ticks to display on the axis. The actual number of ticks is calculated; this control's value is what the calculation will try to aim for.");
  GetZfxybasey()->SetToolTip("The base for the linear and logarithmic tick calculation for the axis. This value can be fractional but must be greater than 1.");
  GetZfxyrangeymin()->SetToolTip("The minimum fallback range value.");
  GetZfxyrangeymax()->SetToolTip("The maximum fallback range value.");
  //GetZfxydigitsy()->SetToolTip("The number of decimal digits to display on the Y axis labels.");

}

//----------------------------------------
void CZFXYPlotPropertyPanel::EvtButtonClicked(wxWindow& window, const wxCommandEventFunction& method, wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_COMMAND_BUTTON_CLICKED,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);

}
//----------------------------------------
void CZFXYPlotPropertyPanel::EvtCheckBox(wxWindow& window, const wxCommandEventFunction& method, wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_COMMAND_CHECKBOX_CLICKED,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);

}
//----------------------------------------
void CZFXYPlotPropertyPanel::EvtDisplayDataAttrChanged(wxWindow& window, const CZFXYDisplayDataAttrChangedEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_ZFXY_DISPLAY_DATA_ATTR_CHANGED,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}

//----------------------------------------
void CZFXYPlotPropertyPanel::EvtProjectionChanged(wxWindow& window, const CZFXYProjectionChangedEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_ZFXY_PROJECTION_CHANGED,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}

//----------------------------------------
void CZFXYPlotPropertyPanel::EvtCenterPointChanged(wxWindow& window, const CZFXYCenterPointChangedEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_ZFXY_CENTER_POINT_CHANGED,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}

//----------------------------------------
void CZFXYPlotPropertyPanel::EvtRangeChanged(wxWindow& window, const CZFXYRangeChangedEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_ZFXY_RANGE_CHANGED,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}

//----------------------------------------
void CZFXYPlotPropertyPanel::EvtNumLabelsChanged(wxWindow& window, const CZFXYNumLabelsChangedEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_ZFXY_NUM_LABELS_CHANGED,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}
//----------------------------------------
void CZFXYPlotPropertyPanel::EvtUpdateRangeCommand(wxWindow& window, const CZFXYUpdateRangeEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_ZFXY_UPDATE_RANGE_COMMAND,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}
//----------------------------------------
void CZFXYPlotPropertyPanel::EvtViewStateChanged(wxWindow& window, const CZFXYViewStateChangedEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_ZFXY_VIEW_STATE_CHANGED,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}

//----------------------------------------
void CZFXYPlotPropertyPanel::EvtViewStateCommand(wxWindow& window, const CZFXYViewStateCommandEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_ZFXY_VIEW_STATE_COMMAND,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}
//----------------------------------------
void CZFXYPlotPropertyPanel::EvtZoomChangedCommand(wxWindow& window, const CZFXYZoomChangedEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_ZFXY_ZOOM_CHANGED,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}
//----------------------------------------
void CZFXYPlotPropertyPanel::EvtFactorChangedCommand(wxWindow& window, const CZFXYFactorChangedEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_ZFXY_FACTOR_CHANGED,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}
//----------------------------------------
void CZFXYPlotPropertyPanel::EvtRadiusChangedCommand(wxWindow& window, const CZFXYRadiusChangedEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_ZFXY_RADIUS_CHANGED,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}
//----------------------------------------
void CZFXYPlotPropertyPanel::EvtChoice(wxWindow& window, const wxCommandEventFunction& method,
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
void CZFXYPlotPropertyPanel::InstallEventListeners()
{
  
  CLabeledTextCtrl::EvtValueChanged(*this, 
                                     ID_ZFXYRANGEMIN, 
                                     (CValueChangedEventFunction)&CZFXYPlotPropertyPanel::OnMinRange);

  CLabeledTextCtrl::EvtValueChanged(*this, 
                                     ID_ZFXYRANGEMAX, 
                                     (CValueChangedEventFunction)&CZFXYPlotPropertyPanel::OnMaxRange);
  
  CLabeledTextCtrl::EvtValueChanged(*this, 
                                     ID_ZFXYNUMLABELS, 
                                     (CValueChangedEventFunction)&CZFXYPlotPropertyPanel::OnNumLabels);
  
  
  CZFXYPlotPropertyPanel::EvtRangeChanged(*this, 
                                            (CZFXYRangeChangedEventFunction)&CZFXYPlotPropertyPanel::OnRangeChanged);
  
  CZFXYPlotPropertyPanel::EvtNumLabelsChanged(*this, 
                                            (CZFXYNumLabelsChangedEventFunction)&CZFXYPlotPropertyPanel::OnNumLabelsChanged);


  CLabeledTextCtrl::EvtValueChanged(*this, 
                                     ID_ZFXYPLOTPROP_TITLE, 
                                     (CValueChangedEventFunction)&CZFXYPlotPropertyPanel::OnTitle);

  CAnimationToolbar::EvtKeyframeChanged(*this, 
                                        (CKeyframeEventFunction)&CZFXYPlotPropertyPanel::OnKeyframeChanged, NULL, this);

  
  //---- X Axis

  CLabeledTextCtrl::EvtValueChanged(*this, 
                                     ID_ZFXYTITLEX, 
                                     (CValueChangedEventFunction)&CZFXYPlotPropertyPanel::OnXTitle);
  
  CLabeledTextCtrl::EvtValueChanged(*this, 
                                     ID_ZFXYNUMTICKSX, 
                                     (CValueChangedEventFunction)&CZFXYPlotPropertyPanel::OnXNTicks);
  
  CLabeledTextCtrl::EvtValueChanged(*this, 
                                     ID_ZFXYDIGITSX, 
                                     (CValueChangedEventFunction)&CZFXYPlotPropertyPanel::OnXDigits);  

  CLabeledTextCtrl::EvtValueChanged(*this, 
                                     ID_ZFXYRANGEXMIN, 
                                     (CValueChangedEventFunction)&CZFXYPlotPropertyPanel::OnXMin);

  CLabeledTextCtrl::EvtValueChanged(*this, 
                                     ID_ZFXYRANGEXMAX, 
                                     (CValueChangedEventFunction)&CZFXYPlotPropertyPanel::OnXMax);

  CLabeledTextCtrl::EvtValueChanged(*this, 
                                     ID_ZFXYBASEX, 
                                     (CValueChangedEventFunction)&CZFXYPlotPropertyPanel::OnXBase);
  //---- Y Axis

  CLabeledTextCtrl::EvtValueChanged(*this, 
                                     ID_ZFXYTITLEY, 
                                     (CValueChangedEventFunction)&CZFXYPlotPropertyPanel::OnYTitle);
  
  CLabeledTextCtrl::EvtValueChanged(*this, 
                                     ID_ZFXYNUMTICKSY, 
                                     (CValueChangedEventFunction)&CZFXYPlotPropertyPanel::OnYNTicks);
    
  CLabeledTextCtrl::EvtValueChanged(*this, 
                                     ID_ZFXYDIGITSY, 
                                     (CValueChangedEventFunction)&CZFXYPlotPropertyPanel::OnYDigits);

  CLabeledTextCtrl::EvtValueChanged(*this, 
                                     ID_ZFXYRANGEYMIN, 
                                     (CValueChangedEventFunction)&CZFXYPlotPropertyPanel::OnYMin);

  CLabeledTextCtrl::EvtValueChanged(*this, 
                                     ID_ZFXYRANGEYMAX, 
                                     (CValueChangedEventFunction)&CZFXYPlotPropertyPanel::OnYMax);

  CLabeledTextCtrl::EvtValueChanged(*this, 
                                     ID_ZFXYBASEY, 
                                     (CValueChangedEventFunction)&CZFXYPlotPropertyPanel::OnYBase);

  

  m_plotter->AddObserver(vtkCommand::UserEvent, m_vtkComputeRangeCallback);
 

}

//----------------------------------------
void CZFXYPlotPropertyPanel::UpdateXMaxFromControls()
{
  double curMin = m_plotter->GetComputedXRange()[0];
  double curMax = m_plotter->GetComputedXRange()[1];
  
  double newMax;
  GetZfxyrangexmax()->GetValue(newMax);
 
  if (curMax == newMax)
  {
    return;
  }
  
  GetZfxylogx()->Enable(curMin > 0);

  m_plotter->ZoomToXRange(curMin, newMax);

  m_vtkWidget->Refresh();

  UpdateXAxisControls();
}
//----------------------------------------
void CZFXYPlotPropertyPanel::UpdateXMinFromControls()
{
  double curMin = m_plotter->GetComputedXRange()[0];
  double curMax = m_plotter->GetComputedXRange()[1];
  
  double newMin;
  GetZfxyrangexmin()->GetValue(newMin);
 
  if (curMin == newMin)
  {
    return;
  }

  GetZfxylogx()->Enable(newMin > 0);

  m_plotter->ZoomToXRange(newMin, curMax);
  
  m_vtkWidget->Refresh();

  UpdateXAxisControls();
}

//----------------------------------------
void CZFXYPlotPropertyPanel::UpdateYMaxFromControls()
{
  double curMin = m_plotter->GetComputedYRange()[0];
  double curMax = m_plotter->GetComputedYRange()[1];
  
  double newMax;
  GetZfxyrangeymax()->GetValue(newMax);
 
  if (curMax == newMax)
  {
    return;
  }
  
  GetZfxylogx()->Enable(curMin > 0);

  m_plotter->ZoomToYRange(curMin, newMax);

  m_vtkWidget->Refresh();

  UpdateXAxisControls();
}
//----------------------------------------
void CZFXYPlotPropertyPanel::UpdateYMinFromControls()
{
  double curMin = m_plotter->GetComputedYRange()[0];
  double curMax = m_plotter->GetComputedYRange()[1];
  
  double newMin;
  GetZfxyrangeymin()->GetValue(newMin);
 
  if (curMin == newMin)
  {
    return;
  }
  
  GetZfxylogx()->Enable(newMin > 0);

  m_plotter->ZoomToYRange(newMin, curMax);

  m_vtkWidget->Refresh();

  UpdateXAxisControls();
}

//----------------------------------------
void CZFXYPlotPropertyPanel::UpdateXNTicksFromControls()
{
  uint32_t curNTicks = m_plotter->GetNumberOfXLabels();
  
  uint32_t newNTicks;
  GetZfxynumticksx()->GetValue(newNTicks);
 
  if (curNTicks == newNTicks)
  {
    return;
  }
  
  m_plotter->SetNumberOfXLabels(newNTicks);
  wxString format = "%-#.";
  format.Append(wxString::Format("%d", newNTicks));
  format.Append("g");
  m_plotter->SetLabelYFormat(format.c_str());
  
  m_vtkWidget->Refresh();

  UpdateXAxisControls();
}
//----------------------------------------
void CZFXYPlotPropertyPanel::UpdateYNTicksFromControls()
{
  uint32_t curNTicks = m_plotter->GetNumberOfYLabels();
  
  uint32_t newNTicks;
  GetZfxynumticksy()->GetValue(newNTicks);
 
  if (curNTicks == newNTicks)
  {
    return;
  }
  
  m_plotter->SetNumberOfYLabels(newNTicks);
  
  m_vtkWidget->Refresh();

  UpdateXAxisControls();
}
//----------------------------------------
void CZFXYPlotPropertyPanel::UpdateXDigitsFromControls()
{
  uint32_t newNumberOfXDigits;
  GetZfxydigitsx()->GetValue(newNumberOfXDigits, CZFXYPlotPropertyPanel::m_DIGITS_DEF, CZFXYPlotPropertyPanel::m_DIGITS_MIN, CZFXYPlotPropertyPanel::m_DIGITS_MAX);
  GetZfxydigitsx()->SetValue(newNumberOfXDigits);

  if (m_numberOfXDigits == newNumberOfXDigits)
  {
    return;
  }
/*  
  if (newNumberOfXDigits <= 0)
  {
    newNumberOfXDigits = 1;
    GetZfxydigitsx()->SetValue(newNumberOfXDigits);
  }
*/
  m_numberOfXDigits = newNumberOfXDigits;
  wxString format = "%-#.";
  format.Append(wxString::Format("%d", newNumberOfXDigits));
  format.Append("g");
  m_plotter->SetLabelXFormat(format.c_str());
  
  m_vtkWidget->Refresh();

  UpdateXAxisControls();
}
//----------------------------------------
void CZFXYPlotPropertyPanel::UpdateYDigitsFromControls()
{
  uint32_t newNumberOfYDigits;
  GetZfxydigitsy()->GetValue(newNumberOfYDigits, CZFXYPlotPropertyPanel::m_DIGITS_DEF, CZFXYPlotPropertyPanel::m_DIGITS_MIN, CZFXYPlotPropertyPanel::m_DIGITS_MAX);
  GetZfxydigitsy()->SetValue(newNumberOfYDigits);
 
  if (m_numberOfYDigits == newNumberOfYDigits)
  {
    return;
  }
  
  /*
  if (newNumberOfYDigits <= 0)
  {
    newNumberOfYDigits = 1;
    GetZfxydigitsy()->SetValue(newNumberOfYDigits);
  }
*/
  m_numberOfYDigits = newNumberOfYDigits;
  wxString format = "%-#.";
  format.Append(wxString::Format("%d", newNumberOfYDigits));
  format.Append("g");
  m_plotter->SetLabelYFormat(format.c_str());
  
  m_vtkWidget->Refresh();

  UpdateYAxisControls();
}
//----------------------------------------
void CZFXYPlotPropertyPanel::UpdateXTitleFromControls()
{
  wxString str = m_plotter->GetXTitle();
  
  wxString curTitle = wxString(CTools::SlashesEncode((const char *)(str)).c_str());
  
  wxString newTitle = GetZfxytitlex()->GetStringValue();
  
  if (curTitle == newTitle)
  {
    return;
  }
  
  m_plotter->SetXTitle(CTools::SlashesDecode((const char *)(newTitle)).c_str());
  
  m_vtkWidget->Refresh();

  UpdateXAxisControls();
}
//----------------------------------------
void CZFXYPlotPropertyPanel::UpdateYTitleFromControls()
{
  wxString str = m_plotter->GetYTitle();
  
  wxString curTitle = wxString(CTools::SlashesEncode((const char *)(str)).c_str());
  
  wxString newTitle = GetZfxytitley()->GetStringValue();
  
  if (curTitle == newTitle)
  {
    return;
  }
  
  m_plotter->SetYTitle(CTools::SlashesDecode((const char *)(newTitle)).c_str());
  
  m_vtkWidget->Refresh();

  UpdateXAxisControls();
}

//----------------------------------------
void CZFXYPlotPropertyPanel::UpdateLogXAxisFromControls()
{
  bool curLog = (m_plotter->GetLogX() != 0);
  bool nLog = GetZfxylogx()->IsChecked();
  if (curLog == nLog)
  {
    return;
  }

  m_plotter->SetLogX(nLog);
  
  m_vtkWidget->Refresh();
  
  UpdateXAxisControls();
}


//----------------------------------------
void CZFXYPlotPropertyPanel::UpdateLogYAxisFromControls()
{
  bool curLog = (m_plotter->GetLogY() != 0);
  bool nLog = GetZfxylogy()->IsChecked();
  if (curLog == nLog)
  {
    return;
  }
 
  m_plotter->SetLogY(nLog);
  
  m_vtkWidget->Refresh();
  
  UpdateYAxisControls();
 
}

//----------------------------------------
void CZFXYPlotPropertyPanel::UpdateXBaseFromControls()
{
  double nBase = 0;
  double curBase = m_plotter->GetBaseX();
  try
  {
    GetZfxybasex()->GetValue(nBase);
  }
  catch (...)
  {
    ::wxBell();
    GetZfxybasex()->SetValue(curBase);
    return;
  }

  if (nBase == curBase)
  {
    return;
  }

  if (nBase > 1)
  {
     m_plotter->SetBaseX(nBase);
    m_vtkWidget->Refresh();
    UpdateXAxisControls();
  }
  else
  {
    ::wxBell();
    GetZfxybasex()->SetValue(curBase);
  }

}

//----------------------------------------
void CZFXYPlotPropertyPanel::UpdateYBaseFromControls()
{
  double nBase = 0;
  double curBase = m_plotter->GetBaseY();
  try
  {
    GetZfxybasey()->GetValue(nBase);
  }
  catch (...)
  {
    ::wxBell();
    GetZfxybasey()->SetValue(curBase);
    return;
  }


  if (nBase == curBase)
  {
    return;
  }

  if (nBase > 1)
  {
     m_plotter->SetBaseY(nBase);
    m_vtkWidget->Refresh();
    UpdateXAxisControls();
  }
  else
  {
    ::wxBell();
    GetZfxybasey()->SetValue(curBase);
  }

}
//----------------------------------------
void CZFXYPlotPropertyPanel::OnXTitle(CValueChangedEvent& event)
{
  UpdateXTitleFromControls();
}
//----------------------------------------
void CZFXYPlotPropertyPanel::OnXNTicks(CValueChangedEvent& event)
{
  UpdateXNTicksFromControls();
}
//----------------------------------------
void CZFXYPlotPropertyPanel::OnXDigits(CValueChangedEvent& event)
{
  UpdateXDigitsFromControls();
}
//----------------------------------------
void CZFXYPlotPropertyPanel::OnXMin(CValueChangedEvent& event)
{
  UpdateXMinFromControls();
}
//----------------------------------------
void CZFXYPlotPropertyPanel::OnXMax(CValueChangedEvent& event)
{
  UpdateXMaxFromControls();

}
//----------------------------------------
void CZFXYPlotPropertyPanel::OnXLog(wxCommandEvent& event)
{
  UpdateLogXAxisFromControls();
}
//----------------------------------------
void CZFXYPlotPropertyPanel::OnXBase(CValueChangedEvent& event)
{
  UpdateXBaseFromControls();
}

//----------------------------------------
void CZFXYPlotPropertyPanel::OnYTitle(CValueChangedEvent& event)
{
  UpdateYTitleFromControls();
}

//----------------------------------------
void CZFXYPlotPropertyPanel::OnYNTicks(CValueChangedEvent& event)
{
  UpdateYNTicksFromControls();
}
//----------------------------------------
void CZFXYPlotPropertyPanel::OnYDigits(CValueChangedEvent& event)
{
  UpdateYDigitsFromControls();
}
//----------------------------------------
void CZFXYPlotPropertyPanel::OnYMin(CValueChangedEvent& event)
{
  UpdateYMinFromControls();

}
//----------------------------------------
void CZFXYPlotPropertyPanel::OnYMax(CValueChangedEvent& event)
{
  UpdateYMaxFromControls();

}
//----------------------------------------
void CZFXYPlotPropertyPanel::OnYLog(wxCommandEvent& event)
{
  UpdateLogYAxisFromControls();
}
//----------------------------------------
void CZFXYPlotPropertyPanel::OnYBase(CValueChangedEvent& event)
{
  UpdateYBaseFromControls();
}

//----------------------------------------
void CZFXYPlotPropertyPanel::OnTitle(CValueChangedEvent& event)
{

  wxString curTitle = m_plotter->GetTitle();
  wxString newTitle = CTools::SlashesDecode((const char *)(event.m_value)).c_str();
  if (curTitle == newTitle)
  {
    return;
  }

  m_plotter->SetTitle(newTitle);
  m_vtkWidget->Refresh();
}

//----------------------------------------
void CZFXYPlotPropertyPanel::OnSolidColor(wxCommandEvent& event)
{
  CZFXYPlotData* pdata = GetCurrentLayer();
  if (pdata == NULL)
  {
    return;
  }

  bool show = GetZfxyshowsolidcolor()->GetValue();
  pdata->m_plotProperty.m_solidColor = show;

  m_plotter->RemoveActors();
  m_plotter->AddActors();

/*
  if (show)
  {
    //m_plotter->AddInput(pdata);
    m_plotter->AddActor(pdata->GetVtkActor2D());
  }
  else
  {
    m_plotter->RemoveActor(pdata->GetVtkActor2D());
  }
*/
  m_vtkWidget->Refresh();

}


//----------------------------------------
void CZFXYPlotPropertyPanel::ContourPropChanged( CZFXYContourPropChangedEvent &event )
{
  
  bool contourNeedGenerateValues = false;

  CZFXYPlotData* pdata = GetCurrentLayer();
  if (pdata == NULL)
  {
    return;
  }

  if ((event.m_plotProperty.m_numContour != pdata->m_plotProperty.m_numContour) ||
      (event.m_plotProperty.m_minContourValue != pdata->m_plotProperty.m_minContourValue) ||
      (event.m_plotProperty.m_maxContourValue != pdata->m_plotProperty.m_maxContourValue))
  {
    contourNeedGenerateValues = true;
  }

  if ((event.m_plotProperty.m_numContourLabel != pdata->m_plotProperty.m_numContourLabel))
  {
    pdata->SetContourLabelNeedUpdatePositionOnContour(true);
  }
  

  //---------------------------
  /////////////pdata->m_plotProperty = event.m_plotProperty;
  pdata->m_plotProperty.m_numContour = event.m_plotProperty.m_numContour;
  pdata->m_plotProperty.m_minContourValue = event.m_plotProperty.m_minContourValue;
  pdata->m_plotProperty.m_maxContourValue = event.m_plotProperty.m_maxContourValue;

  pdata->m_plotProperty.m_contourLineColor = event.m_plotProperty.m_contourLineColor;
  pdata->m_plotProperty.m_contourLineWidth = event.m_plotProperty.m_contourLineWidth;

  pdata->m_plotProperty.m_withContourLabel = event.m_plotProperty.m_withContourLabel;
  pdata->m_plotProperty.m_numContourLabel = event.m_plotProperty.m_numContourLabel;

  pdata->m_plotProperty.m_contourLabelColor = event.m_plotProperty.m_contourLabelColor;

  pdata->m_plotProperty.m_contourLabelSize = event.m_plotProperty.m_contourLabelSize;
  pdata->m_plotProperty.m_contourLabelFormat = event.m_plotProperty.m_contourLabelFormat;
  //---------------------------

  if (contourNeedGenerateValues)
  {
    pdata->ContourGenerateValues();
  }

  pdata->SetContour2DProperties();
  pdata->SetContourLabelProperties();


  if (pdata->m_plotProperty.m_withContourLabel)
  {
    /*
    if (pdata->GetContourLabelNeedUpdateOnWindow())
    {
      pdata->UpdateContourLabels2D(); 
    }
    else
    {
      if (pdata->GetContourLabelNeedUpdatePositionOnContour())
      {
        pdata->SetContourLabels2DPosition(); 
        pdata->SetContourLabelNeedUpdatePositionOnContour(false);
      }
    }
    */
    pdata->UpdateContourLabels2D(); 

    m_plotter->RemoveActors();
    m_plotter->AddActors();
  }
  else
  {
    m_plotter->RemoveActorsContourLabel(pdata);
  }

  m_vtkWidget->Refresh();

}
//----------------------------------------
void CZFXYPlotPropertyPanel::OnContour(wxCommandEvent& event)
{

  CZFXYPlotData* pdata = GetCurrentLayer();
  if (pdata == NULL)
  {
    return;
  }

  bool show = GetZfxyshowcontour()->GetValue();
  pdata->m_plotProperty.m_withContour = show;
  
  m_plotter->RemoveActors();


  if (show)
  {
    if (pdata->GetContourLabelNeedUpdateOnWindow())
    {
      pdata->UpdateContourLabels2D();
    }

    if (pdata->GetVtkContourActor2D() == NULL)
    {
      pdata->SetContour2D();
      pdata->UpdateContourLabels2D(); // because sometimes, on the first time refresh is not perfect

    }
  }
  
  m_plotter->AddActors();

  m_vtkWidget->Refresh();
}
//----------------------------------------
void CZFXYPlotPropertyPanel::OnReset(wxCommandEvent& event) 
{
  UpdateLayerControls();
}
//----------------------------------------
void CZFXYPlotPropertyPanel::OnEditContourProperties(wxCommandEvent& event) 
{
  wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, ID_MENU_VIEW_CONTOUR_PROPS);
  wxPostEvent(GetParent()->GetParent(), evt); 

}
//----------------------------------------
void CZFXYPlotPropertyPanel::OnEditColorTable(wxCommandEvent& event) 
{
  wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, ID_MENU_VIEW_CLUTEDIT);
  wxPostEvent(GetParent()->GetParent(), evt); 
}

//----------------------------------------
void CZFXYPlotPropertyPanel::OnMinRange(CValueChangedEvent& event)
{
  double nmin = 0;

  try
  {
    GetZfxyrangemin()->GetValue(nmin);
  }
  catch (...)
  {
    wxBell();
    GetZfxyrangemin()->SetValue(m_min);

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
      GetZfxyrangemin()->SetValue(m_min);
      GetZfxyrangemax()->SetValue(m_max);
    }
    GetCurrentLayer()->GetLookupTable()->SetTableRange(m_min, m_max);
    GetCurrentLayer()->GetColorBarRenderer()->GetLookupTable()->SetTableRange(m_min, m_max);

    CZFXYRangeChangedEvent ev(GetId(),
                          m_min,
                          m_max);

    wxPostEvent(m_parent, ev);

  }
}

//----------------------------------------
void CZFXYPlotPropertyPanel::OnMaxRange(CValueChangedEvent& event)
{
  double nmax = 0;

  try
  {
    GetZfxyrangemax()->GetValue(nmax);
  }
  catch (...)
  {
    wxBell();
    GetZfxyrangemax()->SetValue(m_max);
    return;
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
      GetZfxyrangemin()->SetValue(m_min);
      GetZfxyrangemax()->SetValue(m_max);

    }
    GetCurrentLayer()->GetLookupTable()->SetTableRange(m_min, m_max);
    GetCurrentLayer()->GetColorBarRenderer()->GetLookupTable()->SetTableRange(m_min, m_max);

    CZFXYRangeChangedEvent ev(GetId(),
                          m_min,
                          m_max);
    wxPostEvent(m_parent, ev);

  }
}

//----------------------------------------
void CZFXYPlotPropertyPanel::OnNumLabels(CValueChangedEvent& event)
{
  int32_t nLabels = 0;

  try
  {
    GetZfxynumlabels()->GetValue(nLabels, 2, 2);
  }
  catch (...)
  {
    wxBell();
    GetZfxynumlabels()->SetValue(m_nLabels, 2, 2);
    GetCurrentLayer()->GetColorBarRenderer()->SetNumberOfLabels(m_nLabels);

    return;
  } 

  if (nLabels != m_nLabels)
  {
    m_nLabels = nLabels;

    GetCurrentLayer()->GetColorBarRenderer()->SetNumberOfLabels(m_nLabels);
    
    CZFXYNumLabelsChangedEvent ev(GetId(),
                              m_nLabels);
    wxPostEvent(m_parent, ev);

  }
}
//----------------------------------------

void CZFXYPlotPropertyPanel::YReset()
{
  double range[2];

  if (IsZoomToDataRange())
  {
    m_plotter->GetYPlotDataRange(range[0], range[1], m_currentFrame);
  }
  else
  {
    m_plotter->GetBaseYRange(range);
  }

  m_plotter->ZoomToYRange(range);
  m_vtkWidget->Refresh();
}
//----------------------------------------

void CZFXYPlotPropertyPanel::XReset()
{
  double range[2];

  if (IsZoomToDataRange())
  {
    m_plotter->GetXPlotDataRange(range[0], range[1], m_currentFrame);
  }
  else
  {
    m_plotter->GetBaseXRange(range);
  }

  m_plotter->ZoomToXRange(range);
  m_vtkWidget->Refresh();
    
}
//----------------------------------------
void CZFXYPlotPropertyPanel::FullView()
{
  XReset();
  YReset();
}

//----------------------------------------
// WDR: handler implementations for CZFXYPlotPropertyPanel

void CZFXYPlotPropertyPanel::OnFullView( wxCommandEvent &event )
{
  FullView();
}

//----------------------------------------

void CZFXYPlotPropertyPanel::OnYReset( wxCommandEvent &event )
{
  YReset();  
}
//----------------------------------------

void CZFXYPlotPropertyPanel::OnXReset( wxCommandEvent &event )
{
  XReset();  
}

//----------------------------------------
CZFXYPlotData* CZFXYPlotPropertyPanel::GetCurrentLayer()
{
  int32_t n = GetZfxylayerchoice()->GetSelection();
  if (n < 0)
  {
    return NULL;
  }
  return static_cast<CZFXYPlotData*>(GetZfxylayerchoice()->GetClientData(n));
}
//----------------------------------------
void CZFXYPlotPropertyPanel::SetCurrentLayer(int32_t index)
{
  if (GetZfxylayerchoice()->GetCount() <= 0)
  {
    return;
  }

  GetZfxylayerchoice()->SetSelection(index);

  CZFXYPlotData* geoMap =  static_cast<CZFXYPlotData*>(GetZfxylayerchoice()->GetClientData(index));
  SetCurrentLayer(geoMap);

}
//----------------------------------------
void CZFXYPlotPropertyPanel::SetCurrentLayer( CZFXYPlotData* geoMap )
{
  if (geoMap == NULL)
  {
      throw CException("ERROR in  CZFXYPlotPropertyPanel::SetCurrentLayer : static_cast<CZFXYPlotData*>(event.GetClientData()) returns NULL pointer - ",
                       BRATHL_LOGIC_ERROR);
  }

  m_nLabels = GetCurrentLayer()->GetColorBarRenderer()->GetNumberOfLabels();

  GetZfxynumlabels()->SetValue(m_nLabels, 2, 2);

  vtkLookupTable* vtkLUT = geoMap->GetLookupTable();
  if (vtkLUT == NULL)
  {
    throw CException("ERROR in  CZFXYPlotPropertyPanel::SetCurrentLayer : geoMap->GetLookupTable() returns NULL pointer - ",
                     BRATHL_LOGIC_ERROR);
   }


  m_min = vtkLUT->GetTableRange()[0];
  m_max = vtkLUT->GetTableRange()[1];
  GetZfxyrangemin()->SetValue(m_min);
  GetZfxyrangemax()->SetValue(m_max);

  GetZfxyshowsolidcolor()->SetValue(GetCurrentLayer()->m_plotProperty.m_solidColor);
  GetZfxyshowcontour()->SetValue(GetCurrentLayer()->m_plotProperty.m_withContour);

}
//----------------------------------------
void CZFXYPlotPropertyPanel::OnLayerChoice( wxCommandEvent &event )
{
  
  // Close lutFrame if opened
  CLutFrameCloseEvent evtLut(GetId());
  wxPostEvent(GetParent(), evtLut);
  
  // Close contour properties frame  if opened
  CZFXYContourPropFrameCloseEvent evtContour(GetId());
  wxPostEvent(GetParent(), evtContour);

  CZFXYPlotData* geoMap = static_cast<CZFXYPlotData*>(event.GetClientData());
  if (geoMap == NULL)
  {
      throw CException("ERROR in  CZFXYPlotPropertyPanel::OnLayerChoice : static_cast<CZFXYPlotData*>(event.GetClientData()) returns NULL pointer - ",
                       BRATHL_LOGIC_ERROR);
  }

  SetCurrentLayer(geoMap);

}


//----------------------------------------
void CZFXYPlotPropertyPanel::OnRangeChanged(CZFXYRangeChangedEvent& event)
{
  m_min = event.m_min;
  m_max = event.m_max;
  GetZfxyrangemin()->SetValue(m_min);
  GetZfxyrangemax()->SetValue(m_max);

  GetCurrentLayer()->GetLookupTable()->SetTableRange(m_min, m_max);
  GetCurrentLayer()->GetColorBarRenderer()->GetLookupTable()->SetTableRange(m_min, m_max);

}

//----------------------------------------
void CZFXYPlotPropertyPanel::OnNumLabelsChanged(CZFXYNumLabelsChangedEvent& event)
{
  GetZfxynumlabels()->SetValue(event.m_nLabels, 2, 2);

  GetZfxynumlabels()->GetValue(m_nLabels);
  
  GetCurrentLayer()->GetColorBarRenderer()->SetNumberOfLabels(m_nLabels);

}
//----------------------------------------
void CZFXYPlotPropertyPanel::LutChanged( CLutChangedEvent &event )
{

  GetCurrentLayer()->SetLUT(event.m_LUT); // make a copy of lut

  GetCurrentLayer()->GetLookupTable()->SetTableRange(m_min,
                                                     m_max);

  GetCurrentLayer()->GetColorBarRenderer()->SetLUT(event.m_LUT); // make a copy of lut
  GetCurrentLayer()->GetColorBarRenderer()->GetLookupTable()->SetTableRange(m_min, m_max);

  m_plotter->LutChanged();

}

//----------------------------------------
void CZFXYPlotPropertyPanel::GetRange(double& min, double& max)
{
  min = GetRangeMin();
  max = GetRangeMax();
  
}

//----------------------------------------
double CZFXYPlotPropertyPanel::GetRangeMin()
{
  double min = 0.0;
  GetZfxyrangemin()->GetValue(min);
  return min;  
}

//----------------------------------------
double CZFXYPlotPropertyPanel::GetRangeMax()
{
  double max = 0.0;
  GetZfxyrangemax()->GetValue(max);
  return max;  
}



//----------------------------------------
void CZFXYPlotPropertyPanel::UpdateValues()
{
  UpdateLayerControls();
  
  UpdateAxesControls();

  CZFXYUpdateRangeEvent ev(GetId());
  
  wxPostEvent(GetParent(), ev);

}
//----------------------------------------
void CZFXYPlotPropertyPanel::UpdateTitleControls()
{
  wxString str = m_plotter->GetTitle();
  GetZfxyplotpropTitle()->SetValue(CTools::SlashesEncode((const char *)(str)));
}
//----------------------------------------
void CZFXYPlotPropertyPanel::UpdateLayerControls()
{
  UpdateTitleControls();

  CZFXYUpdateRangeEvent ev(GetId());
  
  wxPostEvent(GetParent(), ev);

}

//----------------------------------------
void CZFXYPlotPropertyPanel::UpdateAxesControls()
{
  UpdateXAxisControls();
  UpdateYAxisControls();
}

//----------------------------------------
void CZFXYPlotPropertyPanel::UpdateXAxisControls()
{
  wxString str = m_plotter->GetXTitle();
  GetZfxytitlex()->SetValue( wxString(CTools::SlashesEncode((const char *)(str)).c_str()) );
  
  GetZfxynumticksx()->SetValue(m_plotter->GetNumberOfXLabels());
  GetZfxydigitsx()->SetValue(m_numberOfXDigits, CZFXYPlotPropertyPanel::m_DIGITS_DEF, CZFXYPlotPropertyPanel::m_DIGITS_MIN, CZFXYPlotPropertyPanel::m_DIGITS_MAX);
  GetZfxylogx()->SetValue( (m_plotter->GetLogX() != 0) );
  GetZfxybasex()->SetValue(m_plotter->GetBaseX());

  double min = m_plotter->GetComputedXRange()[0];
  double max = m_plotter->GetComputedXRange()[1];

  GetZfxyrangexmin()->SetValue(min);
  GetZfxyrangexmax()->SetValue(max);

  GetZfxylogx()->Enable(min > 0);


}

//----------------------------------------
void CZFXYPlotPropertyPanel::UpdateYAxisControls()
{
  wxString str = m_plotter->GetYTitle();
  GetZfxytitley()->SetValue( wxString(CTools::SlashesEncode((const char *)(str)).c_str()) );
  
  GetZfxynumticksy()->SetValue(m_plotter->GetNumberOfYLabels());
  GetZfxydigitsy()->SetValue(m_numberOfYDigits);
  GetZfxylogy()->SetValue( (m_plotter->GetLogY() != 0) );
  GetZfxybasey()->SetValue(m_plotter->GetBaseY());

  double min = m_plotter->GetComputedYRange()[0];
  double max = m_plotter->GetComputedYRange()[1];

  GetZfxyrangeymin()->SetValue(min);
  GetZfxyrangeymax()->SetValue(max);

  GetZfxylogy()->Enable(min > 0);


}
//----------------------------------------

void CZFXYPlotPropertyPanel::OnKeyframeChanged(CKeyframeEvent& event)
{
  ///////m_plotter->RemoveInput();


}


