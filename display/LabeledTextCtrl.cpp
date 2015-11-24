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

#include <cerrno>

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "LabeledTextCtrl.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "Tools.h"
#include "Expression.h"
#include "new-gui/Common/tools/Exception.h"
using namespace brathl;

#include "wx/gdicmn.h"

#include "LabeledTextCtrl.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// LabeledTextCtrl
//----------------------------------------------------------------------------
//long ID_LABELED_TEXT;

DEFINE_EVENT_TYPE(wxEVT_LABELED_TEXT_VALUE_CHANGED)


// WDR: event table for LabeledTextCtrl

BEGIN_EVENT_TABLE(CLabeledTextCtrl,wxPanel)
    //EVT_TEXT_ENTER( -1, CLabeledTextCtrl::OnTextEnter )
    //EVT_SET_FOCUS( CLabeledTextCtrl::OnSetFocus )
    //EVT_KILL_FOCUS( CLabeledTextCtrl::OnKillFocus )
    //EVT_TEXT_ENTER( ID_LABELED_TEXT, CLabeledTextCtrl::OnTextEnter )
    //EVT_SET_FOCUS( CLabeledTextCtrl::OnSetFocus )
    //EVT_KILL_FOCUS( CLabeledTextCtrl::OnKillFocus )
END_EVENT_TABLE()

CLabeledTextCtrl::CLabeledTextCtrl()
{
  Init();
}
//----------------------------------------
CLabeledTextCtrl::~CLabeledTextCtrl()
{
  RemoveEventListeners();
}

//----------------------------------------
CLabeledTextCtrl::CLabeledTextCtrl(wxWindow *parent, bool* finished, wxWindowID id, const wxString& label,
                                   const wxString& value,
                                   const wxValidator& validator, const wxString& formatString,
                                   const wxSize& size, long style)
                                   // : wxPanel(parent, id), --> called in Create
{
  Init();

  bool bOk = Create(parent, finished, id, label, value, validator, formatString, size, style);
  if (bOk == false)
  {
    CException e("ERROR in CLabeledTextCtrl ctor - Unenable to create wxWidgets (Create method returns false)", BRATHL_SYSTEM_ERROR);
    throw(e);
  }

}

//----------------------------------------
void CLabeledTextCtrl::Init()
{

  m_finished = NULL;
  m_label = NULL;
  m_text = NULL;
  m_sizer = NULL;
}
//----------------------------------------
bool CLabeledTextCtrl::Create(wxWindow *parent, bool* finished, wxWindowID id, const wxString& label, const wxString& value,
                                   const wxValidator& validator, const wxString& formatString,
                                   const wxSize& size, long style)
{
  bool bOk = true;

  m_finished = finished;

  bOk = wxPanel::Create(parent, id);
  if (bOk == false)
  {
    return false;
  }

  m_formatString = formatString;
  wxSize lblSize(-1, -1);

  if (label.IsEmpty() == false)
  {
    m_label = new wxStaticText(this, -1, label, wxDefaultPosition, lblSize);
    if (bOk == false)
    {
      return false;
    }
  }

  ID_LABELED_TEXT = wxNewId();
  m_text = new wxTextCtrl(this, ID_LABELED_TEXT, value, wxDefaultPosition, size, style, validator);
  if (bOk == false)
  {
    return false;
  }

  m_hasFocus = false;


  m_sizer = new wxBoxSizer(wxHORIZONTAL);

  if (label.IsEmpty() == false)
  {
    m_sizer->Add(m_label, 0, wxALIGN_CENTER|wxRIGHT, 3, NULL);
  }

  m_sizer->Add(m_text, 1, wxALIGN_CENTER|wxRIGHT);

  SetAutoLayout(true);
  SetSizerAndFit(m_sizer);

  InstallEventListeners();

  return true;

}
//----------------------------------------
bool CLabeledTextCtrl::SetBackgroundColour(const wxColour& colour)
{
  if (m_text == NULL)
  {
    return false;
  }

  return m_text->SetBackgroundColour(colour);
}
//----------------------------------------
wxString CLabeledTextCtrl::GetStringValue(const wxString& defValue)
{
  if (m_text->GetValue().IsEmpty())
  {
    SetValue(defValue);
    return defValue;
  }

  return m_text->GetValue();
}
//----------------------------------------
void CLabeledTextCtrl::GetValue(int32_t& value, int32_t defValue, int32_t min, int32_t max)
{
  char *stopString = NULL;
  if (CTools::StrCaseCmp(m_text->GetValue().c_str(), "DV") == 0)
  {
    setDefaultValue(value);
    SetValue(value);
    return;
  }
  // don't use this (it doesn't work)
  //value = strtol(m_text->GetValue().c_str(), &stopString, 10);
  // but this works
  std::string str = (const char *)(m_text->GetValue());
  if (str.empty())
  {
    value = defValue;
    SetValue(value);
    return;
  }

  errno	= 0;
  value = strtol(str.c_str(), &stopString, 10);

  if ((*stopString != '\0') || (errno != 0))
  {
    //throw CException(CTools::Format("Invalid integer value for '%s'",
     //                        m_text->GetValue().c_str()),
     //   		      BRATHL_SYNTAX_ERROR);

    value = 0;
    SetValue(value);
    return;
  }

  if (isDefaultValue(min) == false)
  {
    if (value < min)
    {
      value = defValue;
      SetValue(value);
      return;

    }
  }
  if (isDefaultValue(max) == false)
  {
    if (value > max)
    {
      value = defValue;
      SetValue(value);
      return;
    }
  }

}

//----------------------------------------
void CLabeledTextCtrl::GetValue(uint32_t& value, uint32_t defValue, uint32_t min, uint32_t max)
{
  char *stopString = NULL;
  if (CTools::StrCaseCmp(m_text->GetValue().c_str(), "DV") == 0)
  {
    setDefaultValue(value);
    SetValue(static_cast<int32_t>(value));
    return;
  }
  errno	= 0;
  // don't use this (it doesn't work)
  //value = strtoul(m_text->GetValue().c_str(), &stopString, 10);
  // but this works
  std::string str = (const char *)(m_text->GetValue());
  if (str.empty())
  {
    value = defValue;
    SetValue(static_cast<int32_t>(value));
    return;
  }

  value = strtoul(str.c_str(), &stopString, 10);
  if (*stopString || (errno != 0))
  {
    //throw CException(CTools::Format("Invalid unsigned integer value for '%s'",
    //	          	      m_text->GetValue().c_str()),
    //    		      BRATHL_SYNTAX_ERROR);

    value = defValue;
    SetValue(static_cast<int32_t>(value));
    return;
  }

  if (isDefaultValue(min) == false)
  {
    if (value < min)
    {
      value = defValue;
      SetValue(static_cast<int32_t>(value));
      return;

    }
  }
  if (isDefaultValue(max) == false)
  {
    if (value > max)
    {
      value = defValue;
      SetValue(static_cast<int32_t>(value));
      return;

    }
  }

}



//----------------------------------------
void CLabeledTextCtrl::GetValue(double& value, double defValue, double min, double max)
{
  CExpression expression;
  CExpressionValue exprValue;
  try
  {
    wxString str = GetStringValue();
    if (str.IsEmpty())
    {
      value = defValue;
      SetValue(value);
      return;
    }

    if (str.Last() == '.')
    {
      str.Append("0");
      //SetValue(str);
    }


    expression.SetExpression((const char *)(str));
    if (expression.GetFieldNames()->size() != 0)
    {
      throw CException("Fieldnames are not allowed for float parameter value",
	      	      	        BRATHL_SYNTAX_ERROR);
    }

    exprValue = expression.Execute();
    value = *(exprValue.GetValues());

    if (isDefaultValue(min) == false)
    {
      if (value < min)
      {
        value = defValue;
        SetValue(value);
        return;

      }
    }
    if (isDefaultValue(max) == false)
    {
      if (value > max)
      {
        value = defValue;
        SetValue(value);
        return;

      }
    }
  }
  catch (CException e)
  {
    //throw CException(CTools::Format("Invalid double value for '%s'",
    //	          	      m_text->GetValue().c_str()),
    //    		      BRATHL_SYNTAX_ERROR);
    value = defValue;
    SetValue(value);
  }
}


//----------------------------------------
void CLabeledTextCtrl::GetValue(bool& value, bool defValue)
{
  std::string What	= CTools::StringToUpper((const char *)(m_text->GetValue()));
  if ((What == "YES")	||
      (What == "Y")	||
      (What == "TRUE")	||
      (What == "T")	||
      (What == "OUI")	||
      (What == "O")	||
      (What == "VRAI")	||
      (What == "V")	||
      (What == "1"))
  {
    value = true;
    return;
  }

  if ((What == "NO")	||
      (What == "N")	||
      (What == "FALSE")	||
      (What == "F")	||
      (What == "NON")	||
      (What == "FAUX")	||
      (What == "0"))
  {
    value = false;
    return;
  }

  value = defValue;
  SetValue(value);
  return;


  //throw CException(CTools::Format("Invalid double value for '%s' (Allowed=YES,Y,TRUE,T,1,NO,N,FALSE,F,0)",
  //	          	    m_text->GetValue().c_str()),
  //      		    BRATHL_SYNTAX_ERROR);
}


//----------------------------------------
void CLabeledTextCtrl::GetValue(CDate& value)
{
  int32_t result = value.SetDate(m_text->GetValue().c_str());
  if (result != BRATHL_SUCCESS)
  {
    value.SetDefaultValue();
    //throw CException(CTools::Format("Invalid date value for '%s'",
    //	          	      m_text->GetValue().c_str()),
    //    		      BRATHL_SYNTAX_ERROR);
  }

}
//----------------------------------------
void CLabeledTextCtrl::GetValue(CDate& value, double defValue, double min, double max)
{
  try
  {
    wxString str = GetStringValue();
    if (str.IsEmpty())
    {
      if (isDefaultValue(defValue))
      {
        value.SetDefaultValue();
      }
      else
      {
        value = defValue;
        SetValue(value);
      }
      return;
    }

    GetValue(value);

    if (value.IsDefaultValue())
    {
      if (!isDefaultValue(defValue))
      {
        value = defValue;
        SetValue(value);
      }
      return;
    }

    if (isDefaultValue(min) == false)
    {
      if (value < min)
      {
        if (isDefaultValue(defValue))
        {
          value.SetDefaultValue();
        }
        else
        {
          value = defValue;
          SetValue(value);
        }
        return;

      }
    }
    if (isDefaultValue(max) == false)
    {
      if (value > max)
      {
        if (isDefaultValue(defValue))
        {
          value.SetDefaultValue();
        }
        else
        {
          value = defValue;
          SetValue(value);
        }
        return;

      }
    }
  }
  catch (CException e)
  {
    //throw CException(CTools::Format("Invalid double value for '%s'",
    //	          	      m_text->GetValue().c_str()),
    //    		      BRATHL_SYNTAX_ERROR);
    value = defValue;
    SetValue(value);
  }

}
//----------------------------------------
void CLabeledTextCtrl::GetValueAsDate(double& seconds, double defValue, double min, double max)
{
  CDate dateTmp;
  GetValue(dateTmp, defValue, min, max);

  seconds = dateTmp.Value();
}
//----------------------------------------
void CLabeledTextCtrl::GetValue(std::string& value, const std::string& defValue)
{
  value	= GetStringValue(defValue.c_str()).c_str();
}

//----------------------------------------
void CLabeledTextCtrl::GetValue(wxString& value, const wxString& defValue)
{
  value	= GetStringValue(defValue);
}

//----------------------------------------
void CLabeledTextCtrl::SetValue(const wxString& value, const wxString& defValue)
{
  wxString wxValue;

  if (value.IsEmpty())
  {
    wxValue = defValue;
  }
  else
  {
    wxValue = value;
  }

  m_text->SetValue(wxValue);

}
//----------------------------------------
void CLabeledTextCtrl::SetValue(const char* value)
{
  wxString wxValue = value;
  SetValue(wxValue);
}

//----------------------------------------
void CLabeledTextCtrl::SetValue(const std::string& value, const std::string& defValue)
{
  wxString wxValue = value.c_str();
  wxString wxDefValue = defValue.c_str();
  SetValue(wxValue);
}
//----------------------------------------
void CLabeledTextCtrl::SetValue(int32_t value, int32_t defValue,
                                int32_t min, int32_t max)
{
  wxString wxValue;
  if (isDefaultValue(value))
  {
    wxValue = "";
    m_text->SetValue(wxValue);
    return;
  }

  if (isDefaultValue(min) == false)
  {
    if (value < min)
    {
      value = defValue;

    }
  }
  if (isDefaultValue(max) == false)
  {
    if (value > max)
    {
      value = defValue;

    }
  }

  if (isDefaultValue(value))
  {
    wxValue = "";
  }
  else
  {
    if (m_formatString.IsEmpty() == false)
    {
      //wxValue = (CTools::Format(m_formatString.c_str(), value)).c_str();
      wxValue = wxString::Format(m_formatString.c_str(), value);
    }
    else
    {
      //wxValue = (CTools::Format("%d", value)).c_str();
      wxValue = wxString::Format("%d", value);
    }
  }

  m_text->SetValue(wxValue);

}
//----------------------------------------
void CLabeledTextCtrl::SetValue(uint32_t value, uint32_t defValue,
                                uint32_t min, uint32_t max)
{
  wxString wxValue;
  if (isDefaultValue(value))
  {
    wxValue = "";
    m_text->SetValue(wxValue);
    return;
  }

  if (isDefaultValue(min) == false)
  {
    if (value < min)
    {
      value = defValue;

    }
  }
  if (isDefaultValue(max) == false)
  {
    if (value > max)
    {
      value = defValue;
    }
  }

  if (isDefaultValue(value))
  {
    wxValue = "";
  }
  else
  {
    if (m_formatString.IsEmpty() == false)
    {
      //wxValue = (CTools::Format(m_formatString.c_str(), value)).c_str();
      wxValue = wxString::Format(m_formatString.c_str(), value);
    }
    else
    {
      //wxValue = (CTools::Format("%d", value)).c_str();
      wxValue = wxString::Format("%d", value);
    }
  }

  m_text->SetValue(wxValue);

}
//----------------------------------------
void CLabeledTextCtrl::SetValue(bool value)
{
  wxString wxValue = (value ? "Yes" : "No");
  m_text->SetValue(wxValue);

}

//----------------------------------------
void CLabeledTextCtrl::SetValue(double value, double defValue,
                                double min, double max)
{
  wxString wxValue;

  if (isDefaultValue(value))
  {
    wxValue = "";
    m_text->SetValue(wxValue);
    return;
  }

  if (isDefaultValue(min) == false)
  {
    if (value < min)
    {
      value = defValue;
    }
  }

  if (isDefaultValue(max) == false)
  {
    if (value > max)
    {
      value = defValue;
    }
  }

  if (isDefaultValue(value))
  {
    wxValue = "";
  }
  else
  {
    if (m_formatString.IsEmpty() == false)
    {
      //wxValue = (CTools::Format(m_formatString.c_str(), value)).c_str();
      wxValue = wxString::Format(m_formatString.c_str(), value);
    }
    else
    {
      //wxValue = (CTools::Format("%f", value)).c_str();
      wxValue = wxString::Format("%f", value);
    }
  }

  m_text->SetValue(CTools::TrailingZeroesTrim((const char *)(wxValue)).c_str());

}

//----------------------------------------
void CLabeledTextCtrl::SetValue(CDate& value)
{
  SetValue(value, m_formatString);

}
//----------------------------------------
void CLabeledTextCtrl::SetValue(CDate& value, const wxString& formatString)
{
  wxString wxValue = "";

  if (!value.IsDefaultValue())
  {
    wxValue = value.AsString((const char *)(formatString)).c_str();
  }

  m_text->SetValue(wxValue);

}

//----------------------------------------
void CLabeledTextCtrl::SetValueAsDate(double seconds, const wxString& formatString)
{
  CDate dateTmp(seconds);
  SetValue(dateTmp, formatString);

}

//----------------------------------------
void CLabeledTextCtrl::SetFormat(const wxString& formatString)
{
  m_formatString = formatString;
}

//----------------------------------------
void CLabeledTextCtrl::SetTextValidator( const wxValidator &validator )
{
  m_text->SetValidator(validator);
}

//----------------------------------------
void CLabeledTextCtrl::SetMaxLength( uint32_t len )
{
  if (m_text == NULL)
  {
    return;
  }

  m_text->SetMaxLength(len);
}

//----------------------------------------

// WDR: handler implementations for LabeledTextCtrl

void CLabeledTextCtrl::OnKillFocus( wxFocusEvent &event )
{
  event.Skip();
  m_text->SetSelection(0,0);

  bool finished = false;

  if (m_finished != NULL)
  {
    finished = *m_finished;
  }

  if ((m_text->IsModified()) && (finished == false) && (m_hasFocus))
  {
    CValueChangedEvent ev(GetId(), m_text->GetValue());
    wxPostEvent(m_parent, ev);
  }

  m_hasFocus = false;

}

//----------------------------------------
void CLabeledTextCtrl::OnSetFocus( wxFocusEvent &event )
{
  m_hasFocus = true;
  m_text->SetSelection(-1, -1);
  event.Skip();
}

//----------------------------------------
void CLabeledTextCtrl::OnTextEnter( wxCommandEvent &event )
{
  m_text->SetSelection(-1, -1);
  CValueChangedEvent ev(GetId(), event.GetString());
  wxPostEvent(GetParent(), ev);
}



//----------------------------------------
void CLabeledTextCtrl::InstallEventListeners()
{
  m_text->Connect(wxEVT_COMMAND_TEXT_ENTER,
                  wxCommandEventHandler(CLabeledTextCtrl::OnTextEnter),
                  NULL,
                  this);

  m_text->Connect(wxEVT_KILL_FOCUS,
                  wxFocusEventHandler(CLabeledTextCtrl::OnKillFocus),
                  NULL,
                  this);

  m_text->Connect(wxEVT_SET_FOCUS,
                  wxFocusEventHandler(CLabeledTextCtrl::OnSetFocus),
                  NULL,
                  this);
}

//----------------------------------------
void CLabeledTextCtrl::RemoveEventListeners()
{
  m_text->Disconnect(wxEVT_COMMAND_TEXT_ENTER,
                     wxCommandEventHandler(CLabeledTextCtrl::OnTextEnter),
                     NULL,
                     this);

  m_text->Disconnect(wxEVT_KILL_FOCUS,
                     wxFocusEventHandler(CLabeledTextCtrl::OnKillFocus),
                     NULL,
                     this);

  m_text->Disconnect(wxEVT_SET_FOCUS,
                     wxFocusEventHandler(CLabeledTextCtrl::OnSetFocus),
                     NULL,
                     this);
}

//----------------------------------------
void CLabeledTextCtrl::EvtValueChanged(wxWindow& window, int32_t winId, const CValueChangedEventFunction& method,
                                       wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(winId,
                 wxEVT_LABELED_TEXT_VALUE_CHANGED,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}

