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

#ifndef __TimeCtrl_H__
#define __TimeCtrl_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "TimeCtrl.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <wx/spinbutt.h>

const int SPACING = 2;

enum IncrementType
{
	POSITIVE = 1,
	NEGATIVE = 2
};

class CTimeCtrl;

//----------------------------------------------------------------------------
// CTimeChangeEvent
//----------------------------------------------------------------------------

/**
 * This custom time change event triggers whenever the value in the text
 * control changes.
 */
class CTimeChangeEvent : public wxNotifyEvent
{
public:
	/**
	 * Default constructor
	 */
	CTimeChangeEvent();
	
	/**
	 * Normal constructor
	 */
	CTimeChangeEvent(wxEventType type, wxWindowID id = -1, const wxString& value = wxT("") );
	
	/**
	 * To cater for Clone() function
	 * 
	 * @see @ref #Clone()
	 */
	CTimeChangeEvent(const CTimeChangeEvent& event);
	
	/**
	 * Set value
	 */
	void SetValue(const wxString& value);
	
	/**
	 * Get value
	 */
	wxString GetValue() const;
	
	/**
	 * Clone 
	 */
	virtual wxEvent *Clone();
	
private:
	wxString m_value;

	DECLARE_DYNAMIC_CLASS(CTimeChangeEvent)
};

typedef void (wxEvtHandler::*CTimeChangeEventFunction) (CTimeChangeEvent&);

BEGIN_DECLARE_EVENT_TYPES()
  DECLARE_EVENT_TYPE(wxEVT_TIME_CHANGED, 7777) // 7777 is ignored, just for compatibility with v2.2
END_DECLARE_EVENT_TYPES()

#define EVT_TIME_CHANGED(id, func)  \
	DECLARE_EVENT_TABLE_ENTRY(wxEVT_TIME_CHANGED, id, -1, (wxObjectEventFunction) (wxEventFunction) (CTimeChangeEventFunction) & func, (wxObject *) NULL ),

//----------------------------------------------------------------------------
// CTimeChangeTextCtrl
//----------------------------------------------------------------------------

/**
 * This control is the text part of the time-picker.
 */
class CTimeChangeTextCtrl : public wxTextCtrl
{
public:
	/**
	 * Default constructor
	 */
	CTimeChangeTextCtrl();
	
	/**
	 * Normal constructor
	 */
	CTimeChangeTextCtrl(CTimeCtrl *timectrl, const wxString& value = wxT("") );
	  
	/**
	 * Destructor
	 */
	~CTimeChangeTextCtrl();
	
	/**
	 * This function is called from the spin control to 
	 * increment the value
	 *
	 * @param IncrementType type
	 */
	void ApplyIncrement(IncrementType type);

  bool HasFocus();

  virtual void SetFocus();

private:
	CTimeCtrl* m_timeCtrl;
	wxString m_buffer;
	int m_pos;
  int m_min;
  int m_max;

	/**
	 * This event handler is called when a key is pressed in the text ctrl
	 *
	 * @param event
	 */
	void OnChar(wxKeyEvent& event);

	/**
	 * This funciton updates the caret position
	 */
	void UpdatePosition();
	
	/**
	 * This function sets the selection to the appropriate time part
	 */
	void SelectPart();
	
	/**
	 * This function updates the buffer
	 */
	void UpdateBuffer();
	
	/**
	 * This function updates the current time part (hour, minute, second or
	 * am/pm) based on the type of the change
	 *
	 * @param type
	 */
	void Increment(IncrementType type);

	/**
	 * Flush the buffer
	 */
	void FlushBuffer(bool clear = TRUE);
	 
	/**
	 * This function checks whether the valuefalls within the limit
	 *
	 * @param value
	 */
	void FixValue(long *value);
	
	
	DECLARE_DYNAMIC_CLASS(CTimeChangeTextCtrl)
	DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// CTimeChangeSpinButton
//----------------------------------------------------------------------------

/**
 * This control is the spin button of the time picker.
 */
class CTimeChangeSpinButton : public wxSpinButton
{
public:
 	/** 
 	 * Default constructor
 	 */
 	CTimeChangeSpinButton();
 	 
 	/**
 	 * Normal constructor
 	 */
 	CTimeChangeSpinButton(CTimeCtrl *timectrl);
 	
 	/**
 	 * Destructor
 	 */
 	~CTimeChangeSpinButton();
 	
 	/**
 	 * These functions are called when the spin button is pressed
 	 *
 	 * @param wxSpinEvent&
 	 */
 	void OnSpinUp(wxSpinEvent& event);
 	
 	/**
 	 * @see @ref #OnSpinUp(wxSpinEvent& event)
 	 */
 	void OnSpinDown(wxSpinEvent& event);
 	
 private:
 	CTimeCtrl* m_timeCtrl;	

	DECLARE_DYNAMIC_CLASS(CTimeChangeSpinButton)
	DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// CTimeCtrl
//----------------------------------------------------------------------------
class CTimeCtrl : public wxControl
{
public:
	/**
	 * Normal constructor
	 */
	CTimeCtrl(wxWindow *parent = NULL, wxWindowID id = -1,
		const wxString& value = wxT(""), bool showSecond = true, bool showAmPm = true,
    const wxPoint& pos = wxDefaultPosition, 
		const wxSize& size = wxDefaultSize);
			
	/**
	 * Destructor
	 */
	~CTimeCtrl();
	
	/**
	 * This function triggers an update on the text control part, indicating
	 * an increment or a decrement with the parameter @p type.
	 *
	 * @param type
	 */
	void UpdateTextCtrl(IncrementType type);
	
	/**
	 * This function returns the value of the time control.
	 *
	 * @return Time, const wxString&
	 */
	wxString GetValue() const;
	
	/**
	 * Get current time as expected by the control
	 */
	wxString GetCurrentTime();
		
	/**
	 * This function shows or hides the control.
	 *
	 * @param show (TRUE to show; FALSE to hide)
	 * @return bool
	 */
	virtual bool Show(bool show = TRUE);

	/**
	 * This function enables or disables the control
	 *
	 * @param true or false
	 */
	virtual bool Enable(bool enable = TRUE);
	

  bool IsShowSecond() { return m_showSecond; };
  bool IsShowAmPm() { return m_showAmPm; };
  int GetPosAmPm() { return m_posAmPm; };

protected:

  void Init();
  
  int GetTextMaxLength();

	/**
	 * This function overrides the wxControl::DoMoveWindow() function so
	 * that the two parts of the control are synchronised.
	 *
	 * @see wxControl::DoMoveWindow(int x, int y, int width, int height)
	 */
	void DoMoveWindow(int x, int y, int width, int height);
	
	/**
	 * This function overrides the wxControl::DoGetBestSize() function so
	 * that the correct "best size" of the combined controls is returned.
	 *
	 * @see wxControl::DoGetBestSize()
	 */
	wxSize DoGetBestSize() const;
public:
  static const int m_TEXT_MAX_LENGTH;

private:
	CTimeChangeTextCtrl* m_textControl;
	CTimeChangeSpinButton* m_spinButton;
  bool m_showSecond;
  bool m_showAmPm;
  int m_posAmPm;


	DECLARE_DYNAMIC_CLASS(CTimeCtrl)
};

#endif // __TimeCtrl_H__
