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

#ifndef __ColorPicker_H__
#define __ColorPicker_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "ColorPicker.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "Tools.h"
using namespace brathl;

//#include "BratDisplay_wdr.h"
#include "BratDisplayApp.h"

#include "vtkColor.h"
#include "LabeledTextCtrl.h"
#include "wxInterface.h"

extern long ID_COLOR_SLIDER;
extern long ID_COLOR_SLIDER_TEXT;

extern long ID_COLOR_PICKER_LABEL;
extern long ID_COLOR_PICKER_BUTTON;
extern long ID_COLOR_PICKER_R_SLIDER;
extern long ID_COLOR_PICKER_G_SLIDER;
extern long ID_COLOR_PICKER_B_SLIDER;
extern long ID_COLOR_PICKER_A_SLIDER;



//-------------------------------------------------------------
//------------------- CColorChangedEvent class --------------------
//-------------------------------------------------------------

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_COLOR_PICKER_CHANGED, 7777) // 7777 is ignored, just for compatibility with v2.2
END_DECLARE_EVENT_TYPES()

class CColorChangedEvent : public wxCommandEvent
{
public:
  CColorChangedEvent(wxWindowID id, const CPlotColor& vtkColor)
    : wxCommandEvent(wxEVT_COLOR_PICKER_CHANGED, id)
  {
    m_vtkColor = vtkColor;
    m_id = id;
  };

  CColorChangedEvent(const CColorChangedEvent& event)
    : wxCommandEvent(wxEVT_COLOR_PICKER_CHANGED, event.m_id)
  {
    m_vtkColor = event.m_vtkColor;
    m_id = event.m_id;
  };
  
  virtual wxEvent *Clone() const 
    {
      return new CColorChangedEvent(*this); 
    };

  wxWindowID m_id;
  CPlotColor m_vtkColor;
};
typedef void (wxEvtHandler::*CColorChangedEventFunction)(CColorChangedEvent&);

// WDR: class declarations

//-------------------------------------------------------------
//------------------- CColorSlider class --------------------
//-------------------------------------------------------------

class CColorSlider: public wxPanel
{
public:
  // constructors and destructors
  CColorSlider();
  CColorSlider( wxWindow *parent, const wxString& label, wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL | wxNO_BORDER );
 
  virtual ~CColorSlider();

  bool Create( wxWindow *parent, const wxString& label, wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL | wxNO_BORDER );
  
  void GetValue(wxString& value);
  void GetValue(int32_t& value);

  void SetValue(const wxString& value);
  void SetValue(int32_t value);

  // WDR: method declarations for CColorSlider

public:
  bool m_finished;
    
    
private:
  bool m_hasFocus;

  CLabeledTextCtrl* m_colorText;

  wxSlider* m_colorSlider;
  wxBoxSizer* m_sizer;
    // WDR: member variable declarations for CColorSlider
    
private:
  void Init();
  void OnText(CValueChangedEvent& event);
  void PostValueChangedEvent();
  void OnSlider(wxCommandEvent& event);


    // WDR: handler declarations for CColorSlider

private:
    DECLARE_EVENT_TABLE()
};

//-------------------------------------------------------------
//------------------- CColorPicker class --------------------
//-------------------------------------------------------------

class CColorPicker: public wxPanel
{
public:
    // constructors and destructors
    CColorPicker();
    CColorPicker( wxWindow *parent, const wxString& title, wxWindowID id = -1,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = wxTAB_TRAVERSAL | wxNO_BORDER );
    virtual ~CColorPicker();

    bool Create( wxWindow *parent, const wxString& title, wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL | wxNO_BORDER );

    const CPlotColor* GetVtkColor() {return &m_vtkColor;};
    wxColour GetColor() {return color_cast(m_vtkColor);}

    void SetColor(const CPlotColor& vtkColor);
    
    void Reset();

    static void EvtColorChanged(wxWindow& window, int32_t winId, const CColorChangedEventFunction& method,
                                              wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);


    // WDR: method declarations for CColorPicker
    
private:
    CColorSlider* m_rSlider;
    CColorSlider* m_gSlider;
    CColorSlider* m_bSlider;
    CColorSlider* m_aSlider;

    wxStaticText* m_label;
    wxButton* m_colorBtn;

    wxBoxSizer* m_hSizer;
    wxBoxSizer* m_sizer;

    CPlotColor m_vtkColor;

    // WDR: member variable declarations for CColorPicker
    
private:
  void Init();
  void ColorChanged();
  void OnValueChanged(CValueChangedEvent& event);
  void PostColorChangedEvent();
  void OnColorChange( CColorChangedEvent& event );

    // WDR: handler declarations for CColorPicker

private:
    DECLARE_EVENT_TABLE()
};




#endif
