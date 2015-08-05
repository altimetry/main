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
    #pragma implementation "AxisPropertyPanel.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "BratDisplay.h"
#include "Validators.h"
#include "XYPlotPanel.h"

#include "AxisPropertyPanel.h"

// WDR: class implementations

//-------------------------------------------------------------
//------------------- CAxisPropertyPanel class --------------------
//-------------------------------------------------------------
const uint32_t CAxisPropertyPanel::m_DIGITS_DEF = 4;
const uint32_t CAxisPropertyPanel::m_DIGITS_MIN = 1;
const uint32_t CAxisPropertyPanel::m_DIGITS_MAX = 20;


// WDR: event table for CAxisPropertyPanel

BEGIN_EVENT_TABLE(CAxisPropertyPanel,wxPanel)
END_EVENT_TABLE()

CAxisPropertyPanel::CAxisPropertyPanel()
{
   Init();
}
//----------------------------------------

CAxisPropertyPanel::CAxisPropertyPanel( wxWindow *parent, vtkXYPlotActor* plotter, CXYPlotDataCollection* plotDataCollection, wxVTKRenderWindowInteractor* vtkWidget,
                                        wxWindowID id,
                                        const wxPoint &position, const wxSize& size, long style )
                                        //: wxPanel( parent, id, position, size, style ) --> called in Create
{

  Init();

  bool bOk = Create(parent, plotter, plotDataCollection, vtkWidget, id, position, size, style);
  if (bOk == false)
  {
    CException e("ERROR in CAxisPropertyPanel ctor - Unenable to create wxWidgets (Create method returns false)", BRATHL_SYSTEM_ERROR);
    throw(e);
  }

}

//----------------------------------------

CAxisPropertyPanel::~CAxisPropertyPanel()
{
  if (m_vtkComputeRangeCallback != NULL)
  {
    m_vtkComputeRangeCallback->Delete();
    m_vtkComputeRangeCallback = NULL;
  }


  m_plotDataCollection = NULL;


}
//----------------------------------------
void CAxisPropertyPanel::Init()
{
  m_finished = false;
  m_vtkWidget = NULL;

  m_vtkComputeRangeCallback= CVtkComputeRangeCallback::New(this);

  m_plotter = NULL;
  m_plotDataCollection = NULL;

  m_maxTicks = 50;
  m_numberOfDigits = CAxisPropertyPanel::m_DIGITS_DEF;

  m_rSizer1 = NULL;
  m_rSizer2 = NULL;
  m_rSizer3 = NULL;

  m_sizer1 = NULL;
  m_sizer2 = NULL;

  m_sizer = NULL;
  m_mainSizer = NULL;


}
//----------------------------------------

bool CAxisPropertyPanel::Create(wxWindow *parent, vtkXYPlotActor* plotter, CXYPlotDataCollection* plotDataCollection, wxVTKRenderWindowInteractor* vtkWidget,
                                         wxWindowID id,
                                         const wxPoint& pos,
                                         const wxSize& size ,
                                         long style) 
{
  bool bOk = true;

  wxPanel::Create( parent, -1, pos, size, style );
  
  m_plotter = plotter;
  m_plotDataCollection = plotDataCollection;
  m_vtkWidget = vtkWidget;

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
bool CAxisPropertyPanel::CreateControls()
{
  CDigitValidator	DigitValidator;
  CFloatValidator	FloatValidator;

  m_ID_TITLE = wxNewId();
  m_ID_NTICKS = wxNewId();
  m_ID_DERIVE = wxNewId();
  m_ID_MANUAL = wxNewId();
  m_ID_LOG = wxNewId();
  m_ID_BASE = wxNewId();
  m_ID_MIN = wxNewId();
  m_ID_MAX = wxNewId();
  m_ID_DYN_MIN = wxNewId();
  m_ID_DYN_MAX = wxNewId();
  m_ID_RESET_AXIS_RANGE = wxNewId();
  m_ID_DIGITSAXIS = wxNewId();

  int32_t charwidth, charheight;
  CBratDisplayApp::DetermineCharSize(this, charwidth, charheight);
  
  m_box1 = new wxStaticBox(this, -1, "Fallback Range");
  m_box2 = new wxStaticBox(this, -1, "Current Range");

  m_titleCtrl.Create(this, &m_finished, m_ID_TITLE,
                     "Label:", GetTitle(),
                     wxDefaultValidator,
                     "",
                     wxSize(8*charwidth, -1),
                     wxTE_PROCESS_ENTER);
  
  m_logAxisCtrl.Create(this, m_ID_LOG, "Logarithmic Scale");
  m_logAxisCtrl.SetValue(GetLog());

  m_nTicksCtrl.Create(this, &m_finished, m_ID_NTICKS,
                     "Number of Ticks:", "",
                     DigitValidator,
                     "",
                     wxSize(3*charwidth, -1),
                     wxTE_PROCESS_ENTER);
  m_nTicksCtrl.SetValue(GetNumberOfLabels());


  m_baseCtrl.Create(this, &m_finished, m_ID_BASE,
                     "Base:", "",
                     FloatValidator,
                     "%-#.3g",
                     wxSize(3*charwidth, -1),
                     wxTE_PROCESS_ENTER);
  m_baseCtrl.SetValue(GetBase());

  m_digitsCtrl.Create(this, &m_finished, m_ID_DIGITSAXIS,
                      "Number of Digits:", "", 
                      DigitValidator, "", 
                      wxSize(3*charwidth, -1),
                      wxTE_PROCESS_ENTER);
  m_digitsCtrl.SetValue(CAxisPropertyPanel::m_DIGITS_DEF);
  

  // Cannot use the wx.RB_GROUP style for RadioButtons,
  // because this, in combination with checkboxes and
  // subpanels and possibly ScrolledWindows triggers a bug
  // under Windows that causes the entire app to freeze if
  // you try to click on the radio buttons. So we use single
  // radio buttons and handle the switching ourselves.
  m_deriveRadioButton.Create(this, m_ID_DERIVE,
                                      "Derive from Data", 
                                      wxDefaultPosition,
                                      wxDefaultSize,
                                      wxRB_SINGLE);

  m_manualRadioButton.Create(this, m_ID_MANUAL,
                                   "User-specified", 
                                    wxDefaultPosition,
                                    wxDefaultSize,
                                    wxRB_SINGLE);
  m_deriveRadioButton.SetValue(true);
  m_manualRadioButton.SetValue(false);

  double low = 0;
  double high = 0;
  
  GetBaseRange(low, high);

  m_minCtrl.Create(this, &m_finished, m_ID_MIN,
                     "Min:", "",
                     FloatValidator,
                     "%-#.5g",
                     wxSize(8*charwidth, -1),
                     wxTE_PROCESS_ENTER);
  
  m_minCtrl.SetValue(low);
  m_minCtrl.Enable(false);

  m_maxCtrl.Create(this, &m_finished, m_ID_MAX,
                     "Max:", "",
                     FloatValidator,
                     "%-#.5g",
                     wxSize(8*charwidth, -1),
                     wxTE_PROCESS_ENTER);
  
  m_maxCtrl.SetValue(high);
  m_maxCtrl.Enable(false);

  low = 0;
  high = 0;   
  
  GetComputedRange(low, high);

  m_dynMinCtrl.Create(this, &m_finished, m_ID_DYN_MIN,
                     "Min:", "",
                     FloatValidator,
                     "%-#.5g",
                     wxSize(8*charwidth, -1),
                     wxTE_PROCESS_ENTER);
  
  m_dynMinCtrl.SetValue(low);

  m_dynMaxCtrl.Create(this, &m_finished, m_ID_DYN_MAX,
                     "Max:", "",
                     FloatValidator,
                     "%-#.5g",
                     wxSize(8*charwidth, -1),
                     wxTE_PROCESS_ENTER);
  
  m_dynMaxCtrl.SetValue(high);


  m_logAxisCtrl.Enable(low > 0);

  m_resetAxisRange.Create(this, m_ID_RESET_AXIS_RANGE, "Reset", wxDefaultPosition, wxSize(60,-1), 0 );

  return true;

}

//----------------------------------------
void CAxisPropertyPanel::CreateBoxSizers()
{
  m_rSizer1 = new wxBoxSizer(wxHORIZONTAL);
  m_rSizer2 = new wxBoxSizer(wxHORIZONTAL);
  m_rSizer3 = new wxBoxSizer(wxHORIZONTAL);

  m_sizer1 = new wxStaticBoxSizer(m_box1, wxVERTICAL);
  m_sizer2 = new wxStaticBoxSizer(m_box2, wxVERTICAL);

  m_sizer = new wxBoxSizer(wxVERTICAL);
  m_mainSizer = new wxBoxSizer(wxVERTICAL);
  
}
//----------------------------------------
void CAxisPropertyPanel::CreateLayout()
{
  CreateBoxSizers();

  m_rSizer3->Add(&m_nTicksCtrl, 0, wxRIGHT, 10);
  m_rSizer3->Add(&m_baseCtrl, 0, wxRIGHT, 10);
  m_rSizer3->Add(&m_digitsCtrl, 0);

  m_rSizer1->Add(15, 0, 0);
  m_rSizer1->Add(&m_minCtrl, 0, wxRIGHT, 10);
  m_rSizer1->Add(&m_maxCtrl, 0);

  //Spacing of radiobuttons is completely screwed-up under Windows & Mac, so
  // we have to special-case that here.
#ifdef __WXGTK__
  m_sizer1->Add(&m_deriveRadioButton, 0);
  m_sizer1->Add(&m_manualRadioButton, 0, wxBOTTOM, 3);
#else
  m_sizer1->Add(&m_deriveRadioButton, 0, wxTOP, 5);
  m_sizer1->Add(0, 10, 0);
  m_sizer1->Add(&m_manualRadioButton, 0, wxBOTTOM, 6);
#endif

  m_sizer1->Add(m_rSizer1, 1, wxLEFT|wxRIGHT, 5);
  m_sizer1->Add(0, 10, 0);
  
  m_rSizer2->Add(15, 0, 0);
  m_rSizer2->Add(&m_dynMinCtrl, 0, wxRIGHT, 10);
  m_rSizer2->Add(&m_dynMaxCtrl, 0, wxRIGHT, 10);
  m_rSizer2->Add(&m_resetAxisRange, 0);


  m_sizer2->Add(0, 10, 0);
  m_sizer2->Add(m_rSizer2, 1, wxLEFT|wxRIGHT, 5);
  m_sizer2->Add(0, 10, 0);

  m_sizer->Add(&m_titleCtrl, 0, wxEXPAND);
  m_sizer->Add(0, 10, 0);
  m_sizer->Add(&m_logAxisCtrl, 0);
  m_sizer->Add(0, 10, 0);
  m_sizer->Add(m_rSizer3, 0, wxEXPAND);
  m_sizer->Add(0, 15, 0);
  m_sizer->Add(m_sizer1, 0, wxEXPAND);
  m_sizer->Add(0, 15, 0);
  m_sizer->Add(m_sizer2, 0, wxEXPAND);

  m_mainSizer->Add(m_sizer, 1, wxEXPAND|wxALL, 10);

  SetSizerAndFit(m_mainSizer);



}
//----------------------------------------
void CAxisPropertyPanel::InstallToolTips()
{
  m_titleCtrl.SetToolTip("The axis title will be displayed centered on the outside of the axis in the plot panel.");
  m_logAxisCtrl.SetToolTip("Use a logarithmic axis. Disabled if the current range of axis values contains the number 0.");
  m_nTicksCtrl.SetToolTip("The target number of ticks to display on the axis. The actual number of ticks is calculated; this control's value is what the calculation will try to aim for.");
  m_baseCtrl.SetToolTip("The base for the linear and logarithmic tick calculation for the axis. This value can be fractional but must be greater than 1.");
  m_deriveRadioButton.SetToolTip("Calculate minimal fallback range values for this axis. All the data in the plot will be minimally enclosed by this fallback range. Use the 'r' key in the plot panel to set the plot to this range.");
  m_manualRadioButton.SetToolTip("Manually set the fallback range values for this axis.");
  m_minCtrl.SetToolTip("The minimum fallback range value.");
  m_maxCtrl.SetToolTip("The maximum fallback range value.");
  m_dynMinCtrl.SetToolTip("The currently displayed minimum range value.");
  m_dynMaxCtrl.SetToolTip("The currently displayed maximum range value.");
  m_resetAxisRange.SetToolTip("Set axis min./max with original values.");
  m_digitsCtrl.SetToolTip("The number of decimal digits to display on the axis labels.");

}

//----------------------------------------
void CAxisPropertyPanel::InstallEventListeners()
{


  CLabeledTextCtrl::EvtValueChanged(*this, 
                                     m_ID_TITLE, 
                                     (CValueChangedEventFunction)&CAxisPropertyPanel::OnTitle);
  
  CLabeledTextCtrl::EvtValueChanged(*this, 
                                     m_ID_NTICKS, 
                                     (CValueChangedEventFunction)&CAxisPropertyPanel::OnNTicks);
  
  CLabeledTextCtrl::EvtValueChanged(*this, 
                                     m_ID_BASE, 
                                     (CValueChangedEventFunction)&CAxisPropertyPanel::OnBase);

  CLabeledTextCtrl::EvtValueChanged(*this, 
                                     m_ID_DIGITSAXIS, 
                                     (CValueChangedEventFunction)&CAxisPropertyPanel::OnDigits);  

  CLabeledTextCtrl::EvtValueChanged(*this, 
                                     m_ID_MIN, 
                                     (CValueChangedEventFunction)&CAxisPropertyPanel::OnMin);

  CLabeledTextCtrl::EvtValueChanged(*this, 
                                     m_ID_MAX, 
                                     (CValueChangedEventFunction)&CAxisPropertyPanel::OnMax);

  CLabeledTextCtrl::EvtValueChanged(*this, 
                                     m_ID_DYN_MIN, 
                                     (CValueChangedEventFunction)&CAxisPropertyPanel::OnDynMin);

  CLabeledTextCtrl::EvtValueChanged(*this, 
                                     m_ID_DYN_MAX, 
                                     (CValueChangedEventFunction)&CAxisPropertyPanel::OnDynMax);
  
  CXYPlotPanel::EvtRadioButton(m_deriveRadioButton,
                               (wxCommandEventFunction)&CAxisPropertyPanel::OnDerive, NULL, this);

  CXYPlotPanel::EvtRadioButton(m_manualRadioButton,
                               (wxCommandEventFunction)&CAxisPropertyPanel::OnManual, NULL, this);

  CXYPlotPanel::EvtCheckBox(m_logAxisCtrl,
                            (wxCommandEventFunction)&CAxisPropertyPanel::OnLog, NULL, this);
  
  m_resetAxisRange.Connect(wxEVT_COMMAND_BUTTON_CLICKED,
               (wxObjectEventFunction)
               (wxEventFunction)
               (wxCommandEventFunction)&CAxisPropertyPanel::OnAxisResetRange,
               NULL,
               this);

  
  m_plotter->AddObserver(vtkCommand::UserEvent, m_vtkComputeRangeCallback);

 }


//----------------------------------------
void CAxisPropertyPanel::UpdateControls()
{
  m_titleCtrl.SetValue(GetTitle());
  m_nTicksCtrl.SetValue(GetNumberOfLabels());
  m_logAxisCtrl.SetValue(GetLog());
  m_baseCtrl.SetValue(GetBase());
  
  UpdateFallBackRangeFromPlot();
  
  UpdateCurrentRangeFromPlot();

}

//----------------------------------------
void CAxisPropertyPanel::UpdateFallBackRangeFromPlot()
{
  double low = 0;
  double high = 0;
  
  GetRange(low, high);
  m_minCtrl.SetValue(low);
  m_maxCtrl.SetValue(high);

}
//----------------------------------------
void CAxisPropertyPanel::UpdateCurrentRangeFromPlot()
{
  double low = 0;
  double high = 0;
  
  GetComputedRange(low, high);
  m_dynMinCtrl.SetValue(low);
  m_dynMaxCtrl.SetValue(high);

  m_logAxisCtrl.Enable(low > 0);


}

//----------------------------------------
void CAxisPropertyPanel::UpdateLogAxisFromControls()
{
  bool curLog = GetLog();
  bool nLog = m_logAxisCtrl.IsChecked();
  if (curLog != nLog)
  {
    SetLog(nLog);
    m_vtkWidget->Refresh();
    UpdateControls();
  }
}

//----------------------------------------
void CAxisPropertyPanel::UpdateBaseFromControls()
{
  double nBase = 0;
  double curBase = GetBase();
  try
  {
    m_baseCtrl.GetValue(nBase);
  }
  catch (...)
  {
    ::wxBell();
    m_baseCtrl.SetValue(curBase);
    return;
  }


  if (nBase != curBase)
  {
    if (nBase > 1)
    {
      SetBase(nBase);
      m_vtkWidget->Refresh();
      UpdateControls();
    }
    else
    {
      ::wxBell();
      m_baseCtrl.SetValue(curBase);
    }
  }

}
//----------------------------------------
void CAxisPropertyPanel::OnLog(wxCommandEvent& event)
{
  UpdateLogAxisFromControls();
}
//----------------------------------------
void CAxisPropertyPanel::OnBase(CValueChangedEvent& event)
{
  UpdateBaseFromControls();
}
//----------------------------------------
void CAxisPropertyPanel::OnDerive(wxCommandEvent& event)
{
  double low = 0;
  double high = 0;

  m_manualRadioButton.SetValue(false);
  m_minCtrl.Enable(false);
  m_maxCtrl.Enable(false);
  
  GetDataRange(low, high);

  SetRange(low, high);
  m_vtkWidget->Refresh();
  UpdateFallBackRangeFromPlot();
}
//----------------------------------------
void CAxisPropertyPanel::OnManual(wxCommandEvent& event)
{
  m_deriveRadioButton.SetValue(false);
  m_minCtrl.Enable(true);
  m_maxCtrl.Enable(true);
}

//----------------------------------------
void CAxisPropertyPanel::OnTitle(CValueChangedEvent& event)
{
  wxString curTitle = GetTitle();
  wxString newTitle = m_titleCtrl.GetStringValue();
  if (curTitle == newTitle)
  {
    return;
  }
  SetTitle(newTitle);
  m_vtkWidget->Refresh();
}
//----------------------------------------
void CAxisPropertyPanel::OnDigits(CValueChangedEvent& event)
{
  DigitsChanged();

}
//----------------------------------------
void CAxisPropertyPanel::OnNTicks(CValueChangedEvent& event)
{
  int32_t nTicks = 0;
  int32_t curTicks = GetNumberOfLabels();
  try
  {
    m_nTicksCtrl.GetValue(nTicks);
  }
  catch (...)
  {
    ::wxBell();
    m_nTicksCtrl.SetValue(curTicks);
    return;
  }


  if (nTicks != curTicks)
  {
    if ( (0 < nTicks) && ( nTicks <= m_maxTicks) )
    {
      SetNumberOfLabels(nTicks);
    }
    else
    {
      ::wxBell();
      if (nTicks > m_maxTicks)
      {
        m_nTicksCtrl.SetValue(m_maxTicks);
      }
      else
      {
        m_nTicksCtrl.SetValue(1);
      }
    }
  }

  m_vtkWidget->Refresh();
}
//----------------------------------------
void CAxisPropertyPanel::OnMin(CValueChangedEvent& event)
{
  double low = 0;
  double high = 0;
  double nLow = 0;

  GetRange(low, high);

  try
  {
    m_minCtrl.GetValue(nLow);
  }
  catch (...)
  {
    ::wxBell();
    m_minCtrl.SetValue(low);
    return;
  }

  if (nLow != low)
  {
    SetRange(nLow, high);
    m_vtkWidget->Refresh();
    UpdateControls();
  }

}

//----------------------------------------
void CAxisPropertyPanel::OnMax(CValueChangedEvent& event)
{
  double low = 0;
  double high = 0;
  double nHigh = 0;

  GetRange(low, high);

  try
  {
    m_maxCtrl.GetValue(nHigh);
  }
  catch (...)
  {
    ::wxBell();
    m_maxCtrl.SetValue(high);
    return;
  }

  if (nHigh != high)
  {
    SetRange(low, nHigh);
    m_vtkWidget->Refresh();
    UpdateControls();
  }

}
//----------------------------------------
void CAxisPropertyPanel::OnDynMin(CValueChangedEvent& event)
{
  double low = 0;
  double high = 0;
  double nLow = 0;

  GetComputedRange(low, high);

  try
  {
    m_dynMinCtrl.GetValue(nLow);
  }
  catch (...)
  {
    ::wxBell();
    m_dynMinCtrl.SetValue(low);
    return;
  }

  if (nLow != low)
  {
    ZoomToRange(nLow, high);
    m_logAxisCtrl.Enable(low > 0);
    m_vtkWidget->Refresh();
    UpdateControls();
  }

}

//----------------------------------------
void CAxisPropertyPanel::OnDynMax(CValueChangedEvent& event)
{
  double low = 0;
  double high = 0;
  double nHigh = 0;

  GetComputedRange(low, high);

  try
  {
    m_dynMaxCtrl.GetValue(nHigh);
  }
  catch (...)
  {
    ::wxBell();
    m_dynMaxCtrl.SetValue(high);
    return;
  }

  if (nHigh != high)
  {
    ZoomToRange(low, nHigh);
    m_logAxisCtrl.Enable(low > 0);
    m_vtkWidget->Refresh();
    UpdateControls();
  }

}
//----------------------------------------
void CAxisPropertyPanel::OnAxisResetRange( wxCommandEvent &event )
{
  AxisResetRange();
}

// WDR: handler implementations for CAxisPropertyPanel





//-------------------------------------------------------------
//------------------- CAxisXPropertyPanel class --------------------
//-------------------------------------------------------------


// WDR: event table for CAxisXPropertyPanel

BEGIN_EVENT_TABLE(CAxisXPropertyPanel,CAxisPropertyPanel)
END_EVENT_TABLE()

CAxisXPropertyPanel::CAxisXPropertyPanel()
{
   Init();
}
//----------------------------------------
CAxisXPropertyPanel::CAxisXPropertyPanel( wxWindow *parent, vtkXYPlotActor* plotter, CXYPlotDataCollection* plotDataCollection, wxVTKRenderWindowInteractor* vtkWidget,
                                        wxWindowID id,
                                        const wxPoint &position, const wxSize& size, long style )
             ///: CAxisPropertyPanel( parent, plotter, plotData, vtkWidget, id, position, size, style )
{
  Init();

  bool bOk = Create(parent, plotter, plotDataCollection, vtkWidget, id, position, size, style);
  if (bOk == false)
  {
    CException e("ERROR in CAxisXPropertyPanel ctor - Unenable to create wxWidgets (Create method returns false)", BRATHL_SYSTEM_ERROR);
    throw(e);
  }


}

//----------------------------------------

CAxisXPropertyPanel::~CAxisXPropertyPanel()
{

}
//----------------------------------------
void CAxisXPropertyPanel::DigitsChanged()
{
  uint32_t newNumberOfDigits;
  m_digitsCtrl.GetValue(newNumberOfDigits, CAxisPropertyPanel::m_DIGITS_DEF, CAxisPropertyPanel::m_DIGITS_MIN, CAxisPropertyPanel::m_DIGITS_MAX);
  m_digitsCtrl.SetValue(newNumberOfDigits);

  if (m_numberOfDigits == newNumberOfDigits)
  {
    return;
  }
  m_numberOfDigits = newNumberOfDigits;
  wxString format = "%-#.";
  format.Append(wxString::Format("%d", newNumberOfDigits));
  format.Append("g");

  m_plotter->SetLabelXFormat(format.c_str());

  m_vtkWidget->Refresh();
}
//----------------------------------------
void CAxisXPropertyPanel::AxisResetRange()
{
  if (m_plotDataCollection == NULL)
  {
    return;
  }

  double range[2];

  if (m_plotDataCollection->IsZoomToDataRange())
  {
    m_plotDataCollection->GetXRange(range[0], range[1], m_plotDataCollection->GetCurrentFrame());
  }
  else
  {
    m_plotter->GetBaseXRange(range);
  }
  m_plotter->ZoomToXRange(range);
  m_vtkWidget->Refresh();
    
}

//----------------------------------------
wxString CAxisXPropertyPanel::GetTitle()
{
  wxString str = m_plotter->GetXTitle();
  return wxString(CTools::SlashesEncode((const char *)(str)).c_str());
}
//----------------------------------------
void CAxisXPropertyPanel::SetTitle(const wxString& value)
{
  string str = CTools::SlashesDecode((const char *)(value));
  m_plotter->SetXTitle(str.c_str());
}
//----------------------------------------
int32_t CAxisXPropertyPanel::GetNumberOfLabels()
{
  return m_plotter->GetNumberOfXLabels();
}
//----------------------------------------
void CAxisXPropertyPanel::SetNumberOfLabels(int32_t value)
{
  m_plotter->SetNumberOfXLabels(value);
}
//----------------------------------------
bool CAxisXPropertyPanel::GetLog()
{
  return (m_plotter->GetLogX() != 0);
}
//----------------------------------------
void CAxisXPropertyPanel::SetLog(bool value)
{
  m_plotter->SetLogX(value);
}
//----------------------------------------
double CAxisXPropertyPanel::GetBase()
{
  return m_plotter->GetBaseX();
}
//----------------------------------------
void CAxisXPropertyPanel::SetBase(double value)
{
  m_plotter->SetBaseX(value);
}

//----------------------------------------
void CAxisXPropertyPanel::GetRange(double& min, double& max)
{
  min = m_plotter->GetXRange()[0];
  max = m_plotter->GetXRange()[1];
}
//----------------------------------------
void CAxisXPropertyPanel::SetRange(double min, double max)
{
  m_plotter->SetXRange(min, max);
}
//----------------------------------------
void CAxisXPropertyPanel::GetBaseRange(double& min, double& max)
{
  min = m_plotter->GetBaseXRange()[0];
  max = m_plotter->GetBaseXRange()[1];
}

//----------------------------------------
void CAxisXPropertyPanel::GetComputedRange(double& min, double& max)
{
  min = m_plotter->GetComputedXRange()[0];
  max = m_plotter->GetComputedXRange()[1];
}
//----------------------------------------
void CAxisXPropertyPanel::GetDataRange(double& min, double& max)
{
  if (m_plotDataCollection == NULL)
  {
    return;
  }

  m_plotDataCollection->GetXRange(min, max);
}
//----------------------------------------
void CAxisXPropertyPanel::GetDataRange(double& min, double& max, uint32_t frame)
{
  if (m_plotDataCollection == NULL)
  {
    return;
  }
  m_plotDataCollection->GetXRange(min, max, frame);
}
//----------------------------------------
void CAxisXPropertyPanel::ZoomToRange(double min, double max)
{
  double v[2];
  v[0] = min;
  v[1] = max;
  m_plotter->ZoomToXRange(v);
}


//-------------------------------------------------------------
//------------------- CAxisYPropertyPanel class --------------------
//-------------------------------------------------------------


// WDR: event table for CAxisYPropertyPanel

BEGIN_EVENT_TABLE(CAxisYPropertyPanel,CAxisPropertyPanel)
END_EVENT_TABLE()

CAxisYPropertyPanel::CAxisYPropertyPanel()
{
   Init();
}
//----------------------------------------
CAxisYPropertyPanel::CAxisYPropertyPanel( wxWindow *parent, vtkXYPlotActor* plotter, CXYPlotDataCollection* plotDataCollection, wxVTKRenderWindowInteractor* vtkWidget,
                                        wxWindowID id,
                                        const wxPoint &position, const wxSize& size, long style )
             //: CAxisPropertyPanel( parent, plotter, plotData, vtkWidget, id, position, size, style )
{
  Init();

  bool bOk = Create(parent, plotter, plotDataCollection, vtkWidget, id, position, size, style);
  if (bOk == false)
  {
    CException e("ERROR in CAxisYPropertyPanel ctor - Unenable to create wxWidgets (Create method returns false)", BRATHL_SYSTEM_ERROR);
    throw(e);
  }

}
//----------------------------------------

CAxisYPropertyPanel::~CAxisYPropertyPanel()
{

}
//----------------------------------------
void CAxisYPropertyPanel::DigitsChanged()
{
  uint32_t newNumberOfDigits;
  m_digitsCtrl.GetValue(newNumberOfDigits, CAxisPropertyPanel::m_DIGITS_DEF, CAxisPropertyPanel::m_DIGITS_MIN, CAxisPropertyPanel::m_DIGITS_MAX);
  m_digitsCtrl.SetValue(newNumberOfDigits);

  if (m_numberOfDigits == newNumberOfDigits)
  {
    return;
  }
  m_numberOfDigits = newNumberOfDigits;
  wxString format = "%-#.";
  format.Append(wxString::Format("%d", newNumberOfDigits));
  format.Append("g");

  m_plotter->SetLabelYFormat(format.c_str());

  m_vtkWidget->Refresh();
}

//----------------------------------------
void CAxisYPropertyPanel::AxisResetRange()
{
  if (m_plotDataCollection == NULL)
  {
    return;
  }

  double range[2];
  if (m_plotDataCollection->IsZoomToDataRange())
  {
    m_plotDataCollection->GetYRange(range[0], range[1], m_plotDataCollection->GetCurrentFrame());
  }
  else
  {
    m_plotter->GetBaseYRange(range);
  }

  m_plotter->ZoomToYRange(range);
  m_vtkWidget->Refresh();
    
}
//----------------------------------------
wxString CAxisYPropertyPanel::GetTitle()
{
  wxString str = m_plotter->GetYTitle();
  return wxString(CTools::SlashesEncode((const char *)(str)).c_str());
}
//----------------------------------------
void CAxisYPropertyPanel::SetTitle(const wxString& value)
{
  string str = CTools::SlashesDecode((const char *)(value));
  m_plotter->SetYTitle(str.c_str());
}
//----------------------------------------
int32_t CAxisYPropertyPanel::GetNumberOfLabels()
{
  return m_plotter->GetNumberOfYLabels();
}
//----------------------------------------
void CAxisYPropertyPanel::SetNumberOfLabels(int32_t value)
{
  m_plotter->SetNumberOfYLabels(value);
}
//----------------------------------------
bool CAxisYPropertyPanel::GetLog()
{
  return (m_plotter->GetLogY() != 0);
}
//----------------------------------------
void CAxisYPropertyPanel::SetLog(bool value)
{
  m_plotter->SetLogY(value);
}
//----------------------------------------
double CAxisYPropertyPanel::GetBase()
{
  return m_plotter->GetBaseY();
}
//----------------------------------------
void CAxisYPropertyPanel::SetBase(double value)
{
  m_plotter->SetBaseY(value);
}

//----------------------------------------
void CAxisYPropertyPanel::GetRange(double& min, double& max)
{
  min = m_plotter->GetYRange()[0];
  max = m_plotter->GetYRange()[1];
}
//----------------------------------------
void CAxisYPropertyPanel::SetRange(double min, double max)
{
  m_plotter->SetYRange(min, max);
}
//----------------------------------------
void CAxisYPropertyPanel::GetBaseRange(double& min, double& max)
{
  min = m_plotter->GetBaseYRange()[0];
  max = m_plotter->GetBaseYRange()[1];
}

//----------------------------------------
void CAxisYPropertyPanel::GetComputedRange(double& min, double& max)
{
  min = m_plotter->GetComputedYRange()[0];
  max = m_plotter->GetComputedYRange()[1];
}
//----------------------------------------
void CAxisYPropertyPanel::GetDataRange(double& min, double& max, uint32_t frame)
{
  if (m_plotDataCollection == NULL)
  {
    return;
  }

  m_plotDataCollection->GetYRange(min, max, frame);
}
//----------------------------------------
void CAxisYPropertyPanel::GetDataRange(double& min, double& max)
{
  if (m_plotDataCollection == NULL)
  {
    return;
  }

  m_plotDataCollection->GetYRange(min, max);
}
//----------------------------------------
void CAxisYPropertyPanel::ZoomToRange(double min, double max)
{
  double v[2];
  v[0] = min;
  v[1] = max;
  m_plotter->ZoomToYRange(v);
}








