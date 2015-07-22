/*
* Copyright (C) 2005-2010 Collecte Localisation Satellites (CLS), France (31)
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

/*
* This source code is based on source from  jeyoung (at) priscimon.com (see http://wiki.wxwidgets.org/MyTimerCtrl)
*/

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "TimeCtrl.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "Tools.h"
#include "Exception.h"

using namespace brathl;

#include "TimeCtrl.h"

// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

//----------------------------------------------------------------------------
// CTimeChangeEvent
//----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(CTimeChangeEvent, wxNotifyEvent)
#if defined(WIN32) || defined(_WIN32)
DEFINE_EVENT_TYPE(wxEVT_TIME_CHANGED)
#endif
//----------------------------------------
CTimeChangeEvent::CTimeChangeEvent()
	: wxNotifyEvent()
{
}

//----------------------------------------
CTimeChangeEvent::CTimeChangeEvent(wxEventType type, wxWindowID id, const wxString& value)
	: wxNotifyEvent(type, id)
{
	SetValue(value);
}


//----------------------------------------
CTimeChangeEvent::CTimeChangeEvent(const CTimeChangeEvent& event) 
	: wxNotifyEvent(event)
{
	m_value = event.m_value;
}

//----------------------------------------
wxEvent *CTimeChangeEvent::Clone()
{
	return new CTimeChangeEvent(*this);
}

//----------------------------------------
void CTimeChangeEvent::SetValue(const wxString& value) 
{
	m_value = value;
}

//----------------------------------------
wxString CTimeChangeEvent::GetValue() const
{
	return m_value;
}

//----------------------------------------------------------------------------
// CTimeChangeTextCtrl
//----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(CTimeChangeTextCtrl, wxTextCtrl)

//----------------------------------------
CTimeChangeTextCtrl::CTimeChangeTextCtrl()
	: wxTextCtrl()
{
}

//----------------------------------------
CTimeChangeTextCtrl::CTimeChangeTextCtrl(CTimeCtrl *timectrl, const wxString& value)
: wxTextCtrl(timectrl->GetParent(), wxID_ANY, value, wxDefaultPosition, wxDefaultSize)
{
	m_timeCtrl = timectrl;
	m_buffer = wxT("");
  m_pos = 0;
  CTools::SetDefaultValue(m_min);
  CTools::SetDefaultValue(m_max);
}

//----------------------------------------
CTimeChangeTextCtrl::~CTimeChangeTextCtrl()
{

}
//----------------------------------------
bool CTimeChangeTextCtrl::HasFocus()
{
  wxWindow* ctrl = wxWindow::FindFocus();
  if (ctrl != NULL)
  {
    if (ctrl == this)
    {
      return true;
    }
  }
  return false;
}
//----------------------------------------
void CTimeChangeTextCtrl::SetFocus()
{
  if (this->HasFocus())
  {
    return;
  }

  wxTextCtrl::SetFocus();

  this->SetInsertionPoint(m_pos);
}
//----------------------------------------
void CTimeChangeTextCtrl::Increment(IncrementType direction)
{
	/**
	 * Format 00:00:00 xM
	 *
	 *        01234567891
	 *                  0
	 */
	 
  if (m_pos > m_timeCtrl->GetPosAmPm())
	{
		if (m_buffer.IsSameAs(wxT("AM"), FALSE) )
		{
			m_buffer = wxT("PM");
		}
		else
		{
			m_buffer = wxT("AM");
		}
	}
	else
	{
		long value;
		m_buffer.ToLong(&value);

		switch	(direction)
		{
			case POSITIVE:
				value++;
				break;

			case NEGATIVE:
				value--;
				break;
		}
		
		FixValue(&value);
	
		m_buffer = wxString::Format(wxT("%.2d"), (int) value);
	}
}

//----------------------------------------
void CTimeChangeTextCtrl::ApplyIncrement(IncrementType type)
{
	UpdatePosition();
	SelectPart();	
	UpdateBuffer();
	Increment(type);
	FlushBuffer();
	SelectPart();
}

//----------------------------------------
void CTimeChangeTextCtrl::OnChar(wxKeyEvent& event)
{
	UpdatePosition();
	SelectPart();
	UpdateBuffer();
		
	int keycode = event.GetKeyCode();
	
	if (keycode >= 48 && keycode <= 57 && m_pos < m_timeCtrl->GetPosAmPm())
	{
		char ch = keycode;
		wxString temp = m_buffer;
    temp.Append(ch);

		long value;
		temp.Right(2).ToLong(&value);
		
		if (value < m_min || value > m_max)
		{
			m_buffer = "0";
      m_buffer.Append(ch);
		}
		else 
		{
			m_buffer = wxString::Format(wxT("%.2d"), (int) value);
		}
		FlushBuffer();		
	}
	
	switch (keycode)
	{
		case WXK_TAB:
			FlushBuffer();
			
			if (!event.ShiftDown() )
			{
				m_pos += 3;
			}
			else
			{
				m_pos -= 3;
			}
			
			#ifndef __WXMAC__
			if (m_pos < 0 || m_pos > 11)
			{
				event.Skip();
			}
			#endif // __WXMAC__
			break;

		case WXK_LEFT:
			FlushBuffer();

			m_pos -= 3;
			if (m_pos < 0) 
			{
				m_pos = 0;
			}
			break;
			
		case WXK_RIGHT:
			FlushBuffer();

			m_pos += 3;
			if (m_pos > 11) 
			{
				m_pos = 11;
			}
			break;
						
		case WXK_UP:
			Increment(POSITIVE);
			FlushBuffer();
			break;
			
		case WXK_DOWN:
			Increment(NEGATIVE);
			FlushBuffer();
			break;
		
		case 65:
		case 97:
			if (m_pos > m_timeCtrl->GetPosAmPm())
			{
				m_buffer = wxT("AM");
			}
			FlushBuffer();
			break;
			
		case 80:
		case 112:
			if (m_pos > m_timeCtrl->GetPosAmPm())
			{
				m_buffer = wxT("PM");
			}
			FlushBuffer();
		 	break;
	}
	SelectPart();
}

//----------------------------------------
void CTimeChangeTextCtrl::UpdatePosition()
{
	m_pos = GetInsertionPoint();
}

//----------------------------------------
void CTimeChangeTextCtrl::SelectPart() 
{
	if (m_pos <= 2) 
	{
		m_min = 0;   
    m_max = 23;
    if (m_timeCtrl->IsShowAmPm())
    {
		  m_max = 12;
    }
	}
	
	if (m_pos > 2 && m_pos <= m_timeCtrl->GetPosAmPm())
	{
		m_min = 0;
		m_max = 59;
	}

	if (m_pos <= 2)
	{
		SetSelection(0, 2);
	}
	
	if (m_pos > 2 && m_pos <= 5)
	{
		SetSelection(3, 5);
	}
	
	if (m_pos > 5 && m_pos <= 8) 
	{
		SetSelection(6, 8);
	}
	
	if (m_pos > 8) 
	{
		SetSelection(9, 11);
	}
}

//----------------------------------------
void CTimeChangeTextCtrl::UpdateBuffer()
{
	m_buffer = GetStringSelection();
}


//----------------------------------------
void CTimeChangeTextCtrl::FlushBuffer(bool clear)
{
	long start, end;
	GetSelection(&start, &end);
	Replace(start, end, m_buffer);
	
	// trigger a CTimeChangeEvent
	CTimeChangeEvent timechanged(wxEVT_TIME_CHANGED, m_timeCtrl->GetId() );
	timechanged.SetEventObject(m_timeCtrl);
	timechanged.SetValue(GetValue() );
	GetEventHandler()->ProcessEvent(timechanged);
}

//----------------------------------------
void CTimeChangeTextCtrl::FixValue(long *value)
{
	if (*value < m_min) 
	{
		*value = m_max;
	}
	
	if (*value > m_max)
	{
		*value = m_min;
	}
}

//----------------------------------------
BEGIN_EVENT_TABLE(CTimeChangeTextCtrl, wxTextCtrl)
	EVT_CHAR(CTimeChangeTextCtrl::OnChar)
END_EVENT_TABLE()	

//----------------------------------------------------------------------------
// CTimeChangeSpinButton 
//----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(CTimeChangeSpinButton, wxSpinButton)

//----------------------------------------
CTimeChangeSpinButton::CTimeChangeSpinButton()
	: wxSpinButton()
{	
  throw CUnImplementException("Default CTimeChangeSpinButton constructor should not be called !");
}

//----------------------------------------
CTimeChangeSpinButton::CTimeChangeSpinButton(CTimeCtrl *timectrl)
	: wxSpinButton(timectrl->GetParent(), -1, wxDefaultPosition, wxDefaultSize)
{
  // Under Linux, set value to a value diiferent from m_min, otherwise 
  // the spin DOWN is disabled
  SetValue(m_min + 1);

	m_timeCtrl = timectrl;
}

//----------------------------------------
CTimeChangeSpinButton::~CTimeChangeSpinButton()
{

}

//----------------------------------------
void CTimeChangeSpinButton::OnSpinUp(wxSpinEvent& event)
{
	m_timeCtrl->UpdateTextCtrl(POSITIVE);
  // Under Linux, we have to call Veto() to avoid the spin UP to be disabled if max is reached
  event.Veto();
}

//----------------------------------------
void CTimeChangeSpinButton::OnSpinDown(wxSpinEvent& event)
{
	m_timeCtrl->UpdateTextCtrl(NEGATIVE);
  // Under Linux, we have to call Veto() to avoid the spin DOWN to be disabled if min is reached
  event.Veto();
}

BEGIN_EVENT_TABLE(CTimeChangeSpinButton, wxSpinButton)
	EVT_SPIN_UP(wxID_ANY, CTimeChangeSpinButton::OnSpinUp)
	EVT_SPIN_DOWN(wxID_ANY, CTimeChangeSpinButton::OnSpinDown)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
// CTimeCtrl
//----------------------------------------------------------------------------

const int CTimeCtrl::m_TEXT_MAX_LENGTH = 11;

IMPLEMENT_DYNAMIC_CLASS(CTimeCtrl, wxControl)

//----------------------------------------
CTimeCtrl::CTimeCtrl(wxWindow *parent, wxWindowID id, 
	const wxString& value, bool showSecond, bool showAmPm,
  const wxPoint& pos, const wxSize& size)
	: wxControl(parent, id, pos, size)
{
  Init();

  m_showSecond = showSecond;
  m_showAmPm = showAmPm;

  wxString dt = value;
  if (dt.IsEmpty())
  {
    dt = this->GetCurrentTime();
  }

	m_textControl = new CTimeChangeTextCtrl(this, dt);
	m_spinButton = new CTimeChangeSpinButton(this);
	
	m_textControl->SetWindowStyle(wxTE_PROCESS_TAB);
  m_textControl->SetMaxLength(this->GetTextMaxLength());
	
	wxSize bestsize = DoGetBestSize();
	DoMoveWindow(pos.x, pos.y, bestsize.x, bestsize.y);
	
	// prevent the time picker from intercepting events
	wxControl::Enable(FALSE);
	
	Show(true);
}
//----------------------------------------
CTimeCtrl::~CTimeCtrl()
{
}
//----------------------------------------
void CTimeCtrl::Init()
{
  m_showSecond = true;
  m_showAmPm = true;
}
//----------------------------------------
int CTimeCtrl::GetTextMaxLength()
{
  int value = CTimeCtrl::m_TEXT_MAX_LENGTH;
  m_posAmPm = CTimeCtrl::m_TEXT_MAX_LENGTH - 3;

  if (!m_showSecond)
  {
    value -=3;
    m_posAmPm -= 3;
  }
  if (!m_showAmPm)
  {
    value -=3;
  }
  return value;
}
//----------------------------------------
bool CTimeCtrl::Show(bool show)
{
	wxControl::Show(show);
	
	m_textControl->Show(show);
	m_spinButton->Show(show);
	
	return TRUE;
}

//----------------------------------------
bool CTimeCtrl::Enable(bool enable)
{
	wxControl::Enable(enable);
	
	m_textControl->Enable(enable);
	m_spinButton->Enable(enable);
	
	return TRUE;
}

//----------------------------------------
wxSize CTimeCtrl::DoGetBestSize() const
{
	wxSize textsize = m_textControl->GetBestSize();
  wxSize buttonsize = m_spinButton->GetBestSize();
	return wxSize(textsize.x - buttonsize.x + SPACING + 10, textsize.y);
}

//----------------------------------------
void CTimeCtrl::DoMoveWindow(int x, int y, int width, int height)
{
	wxControl::DoMoveWindow(x, y, width, height);
	
	wxSize buttonsize = m_spinButton->GetBestSize();
	int textwidth = width - (buttonsize.x + SPACING);
	
	m_textControl->SetSize(x, y, textwidth, height);
	m_spinButton->SetSize(x + textwidth + SPACING, y, -1, height);
}

//----------------------------------------
void CTimeCtrl::UpdateTextCtrl(IncrementType direction)
{
	// Don't call SetFocus() : tricky behaviour un der Linux
  // But needed under Windows
	#ifdef WIN32
    m_textControl->SetFocus();
  #endif
	m_textControl->ApplyIncrement(direction);
}

//----------------------------------------
wxString CTimeCtrl::GetValue() const
{
	return m_textControl->GetValue();
}

//----------------------------------------
wxString CTimeCtrl::GetCurrentTime()
{
	// get the current time
	wxDateTime ct = wxDateTime::Now();
	
	// format current time appropriately for the time control
	int hour = ct.GetHour();
	int minute = ct.GetMinute();
	int second = ct.GetSecond();
	
	wxString ampm(wxT("AM") );
	
	if (hour == 24)
	{
		hour = 0;
	}

	if ((hour >= 12) && (m_showAmPm))
	{
		ampm = wxT("PM");
		hour %= 12;
	}

  wxString result = wxString::Format("%.2d:%.2d", hour, minute);
  wxString format = "%.2d:%.2d";
  
  if (m_showSecond)
  {
    result.Append(wxString::Format(":%.2d", second));
  }

  if (m_showAmPm)
  {
    result.Append(" ");
    result.Append(ampm);
  }

	//wxString result = wxString::Format(wxT("%.2d"), hour) + wxT(":") + wxString::Format(wxT("%.2d"), minute) + wxT(":") +
	//	wxString::Format(wxT("%.2d"), second) + wxT(" ") + ampm;
		
	return result;
}
