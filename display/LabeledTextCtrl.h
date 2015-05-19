/*
* Copyright (C) 2005 Collecte Localisation Satellites (CLS), France (31)
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

#ifndef __LabeledTextCtrl_H__
#define __LabeledTextCtrl_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "LabeledTextCtrl.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "brathl.h"

#include "wx/stattext.h"
#include "wx/textctrl.h"
#include "wx/string.h"
#include "wx/validate.h"
#include "wx/event.h"

#include "Date.h"
#include "Tools.h"
using namespace brathl;

//#include "BratDisplay_wdr.h"

//long EVT_VALUE_CHANGED_ID = wxNewId();

//extern long ID_LABELED_TEXT;

// ----------------------------------------------------------------------------
// event constants
// ----------------------------------------------------------------------------

// declare a custom event type
//
// note that in wxWin 2.3+ these macros expand simply into the following code:
//
//  extern const wxEventType wxEVT_MY_CUSTOM_COMMAND;
//
//  const wxEventType wxEVT_MY_CUSTOM_COMMAND = wxNewEventType();
//
// and you may use this code directly if you don't care about 2.2 compatibility

// the number 7777 is not used in wxWindows 2.4.x, it is
// only for compatibility with wxWindows 2.2.x.

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_LABELED_TEXT_VALUE_CHANGED, 7777) // 7777 is ignored, just for compatibility with v2.2
END_DECLARE_EVENT_TYPES()

class CValueChangedEvent : public wxCommandEvent
{
public:
  CValueChangedEvent(wxWindowID id, const wxString& value)
    : wxCommandEvent(wxEVT_LABELED_TEXT_VALUE_CHANGED, id)
  {
    m_value = value;
    m_id = id;
  };

  CValueChangedEvent(const CValueChangedEvent& event)
    : wxCommandEvent(wxEVT_LABELED_TEXT_VALUE_CHANGED, event.m_id)
  {
    m_value = event.m_value;
    m_id = event.m_id;
  };
  
  virtual wxEvent *Clone() const 
    {
      return new CValueChangedEvent(*this); 
    };

  wxWindowID m_id;
  wxString m_value;
};
typedef void (wxEvtHandler::*CValueChangedEventFunction)(CValueChangedEvent&);

// WDR: class declarations

//----------------------------------------------------------------------------
// LabeledTextCtrl
//----------------------------------------------------------------------------

class CLabeledTextCtrl: public wxPanel
{
public:
   // constructors and destructors
  CLabeledTextCtrl();
  CLabeledTextCtrl(wxWindow *parent, 
                   bool* finished, 
                   wxWindowID id, 
                   const wxString& label,
                   const wxString& value, 
                   const wxValidator& validator, 
                   const wxString& formatString, 
                   const wxSize& size, long style = wxTE_PROCESS_ENTER);
  
  ~CLabeledTextCtrl ();

  bool Create(wxWindow *parent, 
              bool* finished, 
              wxWindowID id, 
              const wxString& label,
              const wxString& value, 
              const wxValidator& validator, 
              const wxString& formatString, 
              const wxSize& size, long style = wxTE_PROCESS_ENTER);

  wxTextCtrl* GetTextCtrl() {return m_text;};

  bool SetBackgroundColour(const wxColour& colour);

  void SetToolTip(wxToolTip *tip) {m_text->SetToolTip(tip);};
  void SetToolTip(const wxString &tip) {m_text->SetToolTip(tip);};

  wxString GetStringValue(const wxString& defValue = "");

  void GetValue(int32_t& value, int32_t defValue = 0,
                int32_t min = CTools::m_defaultValueINT32, int32_t max = CTools::m_defaultValueINT32);
  void GetValue(uint32_t& value, uint32_t defValue = 0,
                uint32_t min = CTools::m_defaultValueUINT32, uint32_t max = CTools::m_defaultValueUINT32);
  void GetValue(double& value, double defValue = 0.0, 
                double min = CTools::m_defaultValueDOUBLE, double max = CTools::m_defaultValueDOUBLE);
  void GetValue(bool& value, bool defValue = false);
  void GetValue(CDate& value);
  void GetValue(CDate& value, double defValue,
                double min = CTools::m_defaultValueDOUBLE, double max = CTools::m_defaultValueDOUBLE);
  void GetValueAsDate(double& seconds, double defValue = 0.0,
                double min = CTools::m_defaultValueDOUBLE, double max = CTools::m_defaultValueDOUBLE);
  void GetValue(string& value, const string& defValue = "");
  void GetValue(wxString& value, const wxString& defValue = "");


  void SetValue(const wxString& value, const wxString& defValue = "");
  void SetValue(const string& value, const string& defValue = "");
  void SetValue(const char* value);
  void SetValue(int32_t value, int32_t defValue = 0,
                int32_t min = CTools::m_defaultValueINT32, int32_t max = CTools::m_defaultValueINT32);
  void SetValue(uint32_t value, uint32_t defValue = 0,
                uint32_t min = CTools::m_defaultValueUINT32, uint32_t max = CTools::m_defaultValueUINT32);
  void SetValue(double value, double defValue = 0.0,
                double min = CTools::m_defaultValueDOUBLE, double max = CTools::m_defaultValueDOUBLE);
  void SetValue(bool value);
  void SetValue(CDate& value);
  void SetValue(CDate& value, const wxString& formatString);
  void SetValueAsDate(double seconds, const wxString& formatString = "");

  void Clear() {m_text->Clear();};
  void SetFormat(const wxString& formatString);

  wxString GetFormatString() {return m_formatString;};

  void SetTextValidator( const wxValidator &validator );

  void InstallEventListeners();
  void RemoveEventListeners();

  void SetMaxLength(uint32_t len);
;

  static void EvtValueChanged(wxWindow& window, int32_t winId, const CValueChangedEventFunction& method, wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);

    // WDR: method declarations for LabeledTextCtrl
    
private:
  wxString m_formatString;

  wxStaticText* m_label;
  wxTextCtrl* m_text;
  wxBoxSizer* m_sizer;

  
  bool m_hasFocus;
  bool* m_finished; //Is parent windows destroy ? if yes, don't send event to it;

  long ID_LABELED_TEXT;

    // WDR: member variable declarations for LabeledTextCtrl
    
private:
    void Init();

    // WDR: handler declarations for LabeledTextCtrl
    void OnKillFocus( wxFocusEvent &event );
    void OnSetFocus( wxFocusEvent &event );
    void OnTextEnter( wxCommandEvent &event );

private:
    DECLARE_EVENT_TABLE()
};




#endif
