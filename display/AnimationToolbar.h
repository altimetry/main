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


#ifndef __AnimationToolbar_H__
#define __AnimationToolbar_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "AnimationToolbar.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "brathl.h"

#include "BratDisplay_wdr.h"

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_KEYFRAME_CHANGED, 7777) // 7777 is ignored, just for compatibility with v2.2
    DECLARE_EVENT_TYPE(wxEVT_AUTO_ZOOM_DATARANGE, 7777) // 7777 is ignored, just for compatibility with v2.2
END_DECLARE_EVENT_TYPES()

//-------------------------------------------------------------
//------------------- CKeyframeEvent class --------------------
//-------------------------------------------------------------

class CKeyframeEvent : public wxEvent
{
public:
  CKeyframeEvent(uint32_t frameNumber, wxEventType eventType)
    : wxEvent()
  {
    SetEventType(eventType);
    m_frameNumber = frameNumber;
  };

  CKeyframeEvent(const CKeyframeEvent& event)
    : wxEvent()
  {
    SetEventType(event.GetEventType());
    m_frameNumber = event.m_frameNumber;
  };
  
  virtual wxEvent *Clone() const 
    {
      return new CKeyframeEvent(*this); 
    };

  uint32_t m_frameNumber;
};
typedef void (wxEvtHandler::*CKeyframeEventFunction)(CKeyframeEvent&);

//-------------------------------------------------------------
//------------------- CAutoZoomDataRangeEvent class --------------------
//-------------------------------------------------------------

class CAutoZoomDataRangeEvent : public wxEvent
{
public:
  CAutoZoomDataRangeEvent(bool autoZoom, wxEventType eventType)
    : wxEvent()
  {
    SetEventType(eventType);
    m_autoZoom = autoZoom;
  };

  CAutoZoomDataRangeEvent(const CAutoZoomDataRangeEvent& event)
    : wxEvent()
  {
    SetEventType(event.GetEventType());
    m_autoZoom = event.m_autoZoom;
  };
  
  virtual wxEvent *Clone() const 
    {
      return new CAutoZoomDataRangeEvent(*this); 
    };

  bool m_autoZoom;
};
typedef void (wxEvtHandler::*CAutoZoomDataRangeEventFunction)(CAutoZoomDataRangeEvent&);


// WDR: class declarations
//-------------------------------------------------------------
//------------------- CAnimationToolbar class --------------------
//-------------------------------------------------------------

class CAnimationToolbar: public wxPanel
{
public:
  // constructors and destructors
  CAnimationToolbar();
  CAnimationToolbar( wxWindow *parent, wxWindowID id = -1,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxTAB_TRAVERSAL|wxSUNKEN_BORDER);

  virtual ~CAnimationToolbar();

  bool Create( wxWindow *parent, wxWindowID id = -1,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxTAB_TRAVERSAL|wxSUNKEN_BORDER);
  
  uint32_t GetFPS();
  void SetFPS( uint32_t fps );

  void SetLoopPlayback(bool loop);
  void SetIntervalMSec(uint32_t ms);
  void SetMaxFrame(uint32_t value);

  static void EvtKeyframeChanged(wxWindow& window, const CKeyframeEventFunction& method, 
                                 wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
  static void EvtAutoZoomDataRange(wxWindow& window, const CAutoZoomDataRangeEventFunction& method, 
                                 wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);

  // WDR: method declarations for CAnimationToolbar
    wxCheckBox* GetAnimZoomto()  { return (wxCheckBox*) FindWindow( ID_ANIM_ZOOMTO ); }
  wxStaticText* GetDescr()  { return (wxStaticText*) FindWindow( ID_DESCR ); }
  wxButton* GetReset()  { return (wxButton*) FindWindow( ID_RESET ); }
  wxCheckBox* GetLoop()  { return (wxCheckBox*) FindWindow( ID_LOOP ); }
  wxSpinCtrl* GetFramectrl()  { return (wxSpinCtrl*) FindWindow( ID_FRAMECTRL ); }
  wxSlider* GetSlider()  { return (wxSlider*) FindWindow( ID_SLIDER ); }
  wxSpinCtrl* GetSpeedfield()  { return (wxSpinCtrl*) FindWindow( ID_SPEEDFIELD ); }
  wxButton* GetPlay()  { return (wxButton*) FindWindow( ID_PLAY ); }

  void Animate();

  void Reset();
  void Pause();
  void Start();

    
private:
  uint32_t m_frames;
  uint32_t m_currentFrame;
  bool m_loop;
  bool m_running;
  uint32_t m_interval;

  wxTimer* m_timer;
  // WDR: member variable declarations for CAnimationToolbar
    
private:
  void Init();

  void PostKeyframeChangedEvent(uint32_t frameNumber);

  void FrameChanged();
  

  void UpdateSlider();



  void OnPause( wxCommandEvent &event );
  void OnStart( wxCommandEvent &event );

  // WDR: handler declarations for CAnimationToolbar
  void OnAutomaticZoom( wxCommandEvent &event );
  void OnSpinSpeed( wxSpinEvent &event );
  void OnSpinFrame( wxSpinEvent &event );
  void OnSpeedCtrl( wxCommandEvent &event );
  void OnLoop( wxCommandEvent &event );
  void OnFrameCtrl( wxCommandEvent &event );
  void OnReset( wxCommandEvent &event );
  void OnScroll( wxScrollEvent &event );
  void OnAnimate( wxCommandEvent &event );
  void OnTimer( wxTimerEvent &event );
  void OnDestroy( wxWindowDestroyEvent &event );

private:
  DECLARE_CLASS(CAnimationToolbar)
  DECLARE_EVENT_TABLE()
};





#endif
