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
    #pragma implementation "ColorPicker.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "brathl.h"

#include "new-gui/Common/tools/Exception.h"


#include "Validators.h"
#include "ColorPicker.h"

long ID_COLOR_SLIDER;
long ID_COLOR_SLIDER_TEXT;


long ID_COLOR_PICKER_LABEL;
long ID_COLOR_PICKER_BUTTON;
long ID_COLOR_PICKER_R_SLIDER;
long ID_COLOR_PICKER_G_SLIDER;
long ID_COLOR_PICKER_B_SLIDER;
long ID_COLOR_PICKER_A_SLIDER;

DEFINE_EVENT_TYPE(wxEVT_COLOR_PICKER_CHANGED)

// WDR: class implementations


//-------------------------------------------------------------
//------------------- CColorSlider class --------------------
//-------------------------------------------------------------

// WDR: event table for CColorSlider

BEGIN_EVENT_TABLE(CColorSlider,wxPanel)
END_EVENT_TABLE()

//----------------------------------------
CColorSlider::CColorSlider()
{
  Init();
}

//----------------------------------------
CColorSlider::CColorSlider( wxWindow *parent, const wxString& label, wxWindowID id,
                            const wxPoint &position, const wxSize& size, long style ) 
      //: wxPanel( parent, id, position, size, style )
{
  Init();
  bool bOk = Create(parent, label, id, position, size, style);
  if (bOk == false)
  {
    CException e("ERROR in CColorPicker ctor - Unenable to create wxWidgets (Create method returns false)", BRATHL_SYSTEM_ERROR);
    throw(e);
  }

}

//----------------------------------------
CColorSlider::~CColorSlider()
{
}
//----------------------------------------
void CColorSlider::Init()
{
  m_hasFocus = false;
  m_finished = false;

  m_colorSlider = NULL;
  m_colorText = NULL;
  m_sizer = NULL;

}
//----------------------------------------
bool CColorSlider::Create( wxWindow *parent, const wxString& label, wxWindowID id,
                            const wxPoint &position, const wxSize& size, long style )
{
  CDigitValidator	DigitValidator;

  wxPanel::Create( parent, id, position, size, style );

  ID_COLOR_SLIDER = ::wxNewId();
  ID_COLOR_SLIDER_TEXT = ::wxNewId();

  int32_t charwidth, charheight;
  CBratDisplayApp::DetermineCharSize(this, charwidth, charheight);

  m_colorSlider = new wxSlider(this, ID_COLOR_SLIDER, 0, 0, 255, wxDefaultPosition, wxSize(150, -1));
  m_colorText = new CLabeledTextCtrl(this, &m_finished, ID_COLOR_SLIDER_TEXT, 
                                     label+":", "0",
                                     DigitValidator,
                                     "",
                                     wxSize(3*charwidth, -1),
                                     wxTE_PROCESS_ENTER | wxTE_RIGHT);

  m_sizer = new wxBoxSizer(wxHORIZONTAL);
  m_sizer->Add(m_colorSlider, 1, wxALIGN_CENTRE|wxRIGHT, 3);
  m_sizer->Add(m_colorText, 0, wxALIGN_CENTRE);
  SetAutoLayout(true);
  SetSizerAndFit(m_sizer);

  m_colorSlider->Connect(wxEVT_COMMAND_SLIDER_UPDATED,
                         (wxObjectEventFunction)
                         (wxEventFunction)
                         (wxCommandEventFunction)&CColorSlider::OnSlider,
                         NULL,
                         this);

  CLabeledTextCtrl::EvtValueChanged(*this, 
                                    ID_COLOR_SLIDER_TEXT, 
                                    (CValueChangedEventFunction)&CColorSlider::OnText);

  return true;
}
//----------------------------------------
void CColorSlider::OnText(CValueChangedEvent& event)
{
    UNUSED(event);

  int32_t value;
  m_colorText->GetValue(value);
  
  int32_t iMin = (value > 255 ? 255 : value);
  int32_t i = ( iMin < 0 ? 0 : iMin);

  m_colorSlider->SetValue(i);

  PostValueChangedEvent();

}
//----------------------------------------
void CColorSlider::OnSlider(wxCommandEvent& event)
{
    UNUSED(event);

  m_colorText->SetValue(m_colorSlider->GetValue());
  PostValueChangedEvent();
}
//----------------------------------------
void CColorSlider::PostValueChangedEvent()
{
  CValueChangedEvent	ev(GetId(), m_colorText->GetStringValue());
  wxPostEvent(GetParent(), ev);
}

//----------------------------------------
void CColorSlider::GetValue(int32_t& value)
{
  value = m_colorSlider->GetValue();
}
//----------------------------------------
void CColorSlider::GetValue(wxString& value)
{
  value = m_colorText->GetStringValue();
}
//----------------------------------------
void CColorSlider::SetValue(const wxString& value)
{
  m_colorText->SetValue(value);
  int32_t nValue;
  m_colorText->GetValue(nValue);
  m_colorSlider->SetValue(nValue);
}
//----------------------------------------
void CColorSlider::SetValue(int32_t value)
{
  m_colorText->SetValue(value);
  m_colorSlider->SetValue(value);
}

// WDR: handler implementations for CColorSlider


//-------------------------------------------------------------
//------------------- CColorPicker class --------------------
//-------------------------------------------------------------

// WDR: event table for CColorPicker

BEGIN_EVENT_TABLE(CColorPicker,wxPanel)
END_EVENT_TABLE()

CColorPicker::CColorPicker()
{
}

//----------------------------------------
CColorPicker::CColorPicker( wxWindow *parent, const wxString& title, wxWindowID id,
                            const wxPoint &position, const wxSize& size, long style )
                           // : wxPanel( parent, id, position, size, style )
{
  Init();
  bool bOk = Create(parent, title, id, position, size, style);
  if (bOk == false)
  {
    CException e("ERROR in CColorPicker ctor - Unenable to create wxWidgets (Create method returns false)", BRATHL_SYSTEM_ERROR);
    throw(e);
  }
}

//----------------------------------------
CColorPicker::~CColorPicker()
{
}

//----------------------------------------
void CColorPicker::Init()
{
  m_rSlider = NULL;
  m_gSlider = NULL;
  m_bSlider = NULL;
  m_aSlider = NULL;

  m_label = NULL;
  m_colorBtn= NULL;

  m_hSizer = NULL;
  m_sizer = NULL;

}

//----------------------------------------
bool CColorPicker::Create( wxWindow *parent, const wxString& title, wxWindowID id,
                            const wxPoint &position, const wxSize& size, long style )
{
  wxPanel::Create( parent, id, position, size, style );

  ID_COLOR_PICKER_LABEL = ::wxNewId();
  ID_COLOR_PICKER_BUTTON = ::wxNewId();

  ID_COLOR_PICKER_R_SLIDER = ::wxNewId();
  ID_COLOR_PICKER_G_SLIDER = ::wxNewId();
  ID_COLOR_PICKER_B_SLIDER = ::wxNewId();
  ID_COLOR_PICKER_A_SLIDER = ::wxNewId();

  m_rSlider = new CColorSlider(this, "R", ID_COLOR_PICKER_R_SLIDER);
  m_gSlider = new CColorSlider(this, "G", ID_COLOR_PICKER_G_SLIDER);
  m_bSlider = new CColorSlider(this, "B", ID_COLOR_PICKER_B_SLIDER);
  m_aSlider = new CColorSlider(this, "A", ID_COLOR_PICKER_A_SLIDER);
  m_aSlider->SetValue(255);
  
  int32_t charwidth, charheight;
  CBratDisplayApp::DetermineCharSize(this, charwidth, charheight);

  m_label = new wxStaticText(this, ID_COLOR_PICKER_LABEL, title, wxDefaultPosition, wxDefaultSize, wxCENTRE);
  m_colorBtn = new wxButton(this, ID_COLOR_PICKER_BUTTON, "", wxDefaultPosition, wxSize(3*charwidth, -1));
  m_colorBtn->SetBackgroundColour(GetColor());
  
  m_hSizer = new wxBoxSizer(wxHORIZONTAL);
  m_hSizer->Add(m_label, 0, wxALIGN_CENTRE);
  m_hSizer->Add(0, 0, 1);
  m_hSizer->Add(m_colorBtn, 0, wxALIGN_CENTRE);

  m_sizer = new wxBoxSizer(wxVERTICAL);
  m_sizer->Add(m_hSizer, 0, wxEXPAND|wxBOTTOM, 5);
  m_sizer->Add(m_rSlider, 0, wxEXPAND|wxBOTTOM, 5);
  m_sizer->Add(m_gSlider, 0, wxEXPAND|wxBOTTOM, 5);
  m_sizer->Add(m_bSlider, 0, wxEXPAND|wxBOTTOM, 10);
  m_sizer->Add(m_aSlider, 0, wxEXPAND);

  SetAutoLayout(true);
  SetSizerAndFit(m_sizer);

  CLabeledTextCtrl::EvtValueChanged(*this, 
                                    ID_COLOR_PICKER_R_SLIDER, 
                                    (CValueChangedEventFunction)&CColorPicker::OnValueChanged);
  CLabeledTextCtrl::EvtValueChanged(*this, 
                                    ID_COLOR_PICKER_G_SLIDER, 
                                    (CValueChangedEventFunction)&CColorPicker::OnValueChanged);
  CLabeledTextCtrl::EvtValueChanged(*this, 
                                    ID_COLOR_PICKER_B_SLIDER, 
                                    (CValueChangedEventFunction)&CColorPicker::OnValueChanged);
  CLabeledTextCtrl::EvtValueChanged(*this, 
                                    ID_COLOR_PICKER_A_SLIDER, 
                                    (CValueChangedEventFunction)&CColorPicker::OnValueChanged);

  return true;
}

//----------------------------------------
void CColorPicker::OnValueChanged(CValueChangedEvent& event)
{
    UNUSED(event);

  ColorChanged();
  m_colorBtn->SetBackgroundColour(GetColor());
  
  PostColorChangedEvent();


}
//----------------------------------------
void CColorPicker::ColorChanged()
{
  int32_t r = 0, g = 0, b = 0, a = 0;

  m_rSlider->GetValue(r);
  m_gSlider->GetValue(g);
  m_bSlider->GetValue(b);
  m_aSlider->GetValue(a);

  m_vtkColor.Set(r, g, b, a);
}

//----------------------------------------
void CColorPicker::PostColorChangedEvent()
{
  CColorChangedEvent	ev(GetId(), m_vtkColor);
  wxPostEvent(GetParent(), ev);

}
//----------------------------------------
void CColorPicker::Reset()
{
  m_vtkColor.Reset();
  SetColor(m_vtkColor);
}

//----------------------------------------
void CColorPicker::SetColor(const CVtkColor& vtkColor)
{
  int32_t r, g, b, a;
  
  m_vtkColor = vtkColor;

  m_vtkColor.Get(r, g, b, a);

  m_rSlider->SetValue(r);
  m_gSlider->SetValue(g);
  m_bSlider->SetValue(b);
  m_aSlider->SetValue(a);

  m_colorBtn->SetBackgroundColour(GetColor());

}
//----------------------------------------
void CColorPicker::EvtColorChanged(wxWindow& window, int32_t winId, const CColorChangedEventFunction& method,
                                       wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(winId,
                 wxEVT_COLOR_PICKER_CHANGED,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}


// WDR: handler implementations for CColorPicker




