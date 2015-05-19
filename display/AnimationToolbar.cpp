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


#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "AnimationToolbar.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "Exception.h"
using namespace brathl;

#include "AnimationToolbar.h"

DEFINE_EVENT_TYPE(wxEVT_KEYFRAME_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_AUTO_ZOOM_DATARANGE)

// WDR: class implementations

//----------------------------------------------------------------------------
// CAnimationToolbar
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(CAnimationToolbar,wxPanel)

// WDR: event table for CAnimationToolbar

BEGIN_EVENT_TABLE(CAnimationToolbar,wxPanel)
    EVT_WINDOW_DESTROY(CAnimationToolbar::OnDestroy )
    EVT_BUTTON( ID_PLAY, CAnimationToolbar::OnAnimate )
    EVT_COMMAND_SCROLL( ID_SLIDER, CAnimationToolbar::OnScroll )
    EVT_BUTTON( ID_RESET, CAnimationToolbar::OnReset )
    EVT_TEXT_ENTER( ID_FRAMECTRL, CAnimationToolbar::OnFrameCtrl )
    EVT_CHECKBOX( ID_LOOP, CAnimationToolbar::OnLoop )
    EVT_TEXT_ENTER( ID_SPEEDFIELD, CAnimationToolbar::OnSpeedCtrl )
    EVT_SPINCTRL( ID_FRAMECTRL, CAnimationToolbar::OnSpinFrame )
    EVT_SPINCTRL( ID_SPEEDFIELD, CAnimationToolbar::OnSpinSpeed )
    EVT_TIMER(wxID_ANY, CAnimationToolbar::OnTimer)
    EVT_CHECKBOX( ID_ANIM_ZOOMTO, CAnimationToolbar::OnAutomaticZoom )
END_EVENT_TABLE()

//----------------------------------------
CAnimationToolbar::CAnimationToolbar()
{
  Init();
}

//----------------------------------------
CAnimationToolbar::CAnimationToolbar( wxWindow *parent, wxWindowID id,
                                      const wxPoint &position, const wxSize& size, long style ) 
    ///: wxPanel( parent, id, position, size, style )
{
  Init();

  bool bOk = Create(parent, id, position, size, style);
  if (bOk == false)
  {
    CException e("ERROR in CAnimationToolbar ctor - Unenable to create wxWidgets (Create method returns false)", BRATHL_SYSTEM_ERROR);
    throw(e);
  }

}

//----------------------------------------
CAnimationToolbar::~CAnimationToolbar()
{
}


//----------------------------------------
void CAnimationToolbar::Init()
{
  m_frames = 0;
  m_currentFrame = 0;
  m_loop = false;
  m_running = false;
  m_interval = 1000;

  m_timer = NULL;

}
//----------------------------------------
bool CAnimationToolbar::Create( wxWindow *parent, wxWindowID id,
                                const wxPoint &position, const wxSize& size, long style ) 
{

#ifdef WIN32
  // We need to add the full repaint on resize property on Windows to force proper drawing of the controls
  style = style|wxFULL_REPAINT_ON_RESIZE;
#endif

  wxPanel::Create( parent, id, position, size, style );

  // WDR: dialog function AnimationToolBar for CAnimationToolbar
  AnimationToolBar( this, true, true ); 

  GetPlay()->SetFocus();

  m_timer = new wxTimer(this);

  /*
  this->Connect(wxEVT_TIMER,
                   (wxObjectEventFunction)
                   (wxEventFunction)
                   (wxTimerEventFunction)&CAnimationToolbar::OnTimer,
                   NULL,
                   this);
                   */

  SetFPS(20);
  return true;

}

//----------------------------------------
void CAnimationToolbar::SetFPS( uint32_t fps ) 
{

  if (fps == 0)
  {
    fps = 1;
  }
  //Set Timer interval in FPS. Restarts timer if it is currently running
  m_interval = static_cast<uint32_t>((1.0/fps) * 1000);
  GetSpeedfield()->SetValue(fps);

  if (m_running)
  {
    m_timer->Start(m_interval);
  }

}

//----------------------------------------
uint32_t CAnimationToolbar::GetFPS() 
{
  //Return Timer interval in FPS 
  return static_cast<uint32_t>((1.0/m_interval) * 1000);

}

//----------------------------------------
void CAnimationToolbar::SetIntervalMSec(uint32_t ms) 
{
  //Set interval to ms (msec)
  m_interval = ms;
  if (m_running)
  {
    m_timer->Stop();
    m_timer->Start(m_interval);
  }
}
//----------------------------------------
void CAnimationToolbar::SetLoopPlayback(bool loop) 
{
  //Set whether the slider should loop on end """
  m_loop = loop;
}

//----------------------------------------
void CAnimationToolbar::SetMaxFrame(uint32_t value) 
{
  m_frames = (m_frames > value) ? m_frames : value;
  UpdateSlider();

}
//----------------------------------------
void CAnimationToolbar::UpdateSlider() 
{
  wxWindow* focusedWindow = wxWindow::FindFocus();
  GetFramectrl()->SetRange(1, m_frames);
  GetSlider()->SetRange(1, m_frames);
  GetFramectrl()->SetValue(1);
  GetSlider()->SetValue(1);

  if (focusedWindow != NULL)
  {
    focusedWindow->SetFocus();
  }
/*
  else
  {
    GetDefaultItem()->SetFocus();
  }
*/

  Update();
}



//----------------------------------------
void CAnimationToolbar::PostKeyframeChangedEvent(uint32_t frameNumber) 
{
  CKeyframeEvent ev(frameNumber, wxEVT_KEYFRAME_CHANGED);
  wxPostEvent(GetParent(), ev);
}

//----------------------------------------
void CAnimationToolbar::EvtKeyframeChanged(wxWindow& window, const CKeyframeEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_KEYFRAME_CHANGED,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}

//----------------------------------------
void CAnimationToolbar::EvtAutoZoomDataRange(wxWindow& window, const CAutoZoomDataRangeEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_AUTO_ZOOM_DATARANGE,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}

//----------------------------------------

// WDR: handler implementations for CAnimationToolbar

//----------------------------------------
void CAnimationToolbar::OnAutomaticZoom( wxCommandEvent &event )
{

  CAutoZoomDataRangeEvent ev(GetAnimZoomto()->GetValue(), wxEVT_AUTO_ZOOM_DATARANGE);
  wxPostEvent(GetParent(), ev);
    
}

//----------------------------------------
void CAnimationToolbar::OnTimer( wxTimerEvent &event ) 
{
  //Discard timer events still arriving after animation has
  //been stopped.
  if (m_running == false)
  {
    return;
  }

  //Event Handler for Timer Event
  if (m_currentFrame < (m_frames - 1))
  {
    //advance 1
    m_currentFrame++;
    GetSlider()->SetValue(m_currentFrame + 1);
    GetFramectrl()->SetValue(m_currentFrame + 1);
    PostKeyframeChangedEvent(m_currentFrame);
  }
  else
  {
    //auto stop
    if (m_loop)
    {
      Reset();
    }
    else
    {
      Animate();
    }
  }

}

//----------------------------------------
void CAnimationToolbar::OnSpinSpeed( wxSpinEvent &event )
{
  SetFPS(GetSpeedfield()->GetValue());      
}
//----------------------------------------
void CAnimationToolbar::OnSpinFrame( wxSpinEvent &event )
{
  FrameChanged();    
}

//----------------------------------------
void CAnimationToolbar::OnSpeedCtrl( wxCommandEvent &event )
{
  SetFPS(GetSpeedfield()->GetValue());          
}

//----------------------------------------
void CAnimationToolbar::OnFrameCtrl( wxCommandEvent &event )
{
  FrameChanged();

}
//----------------------------------------
void CAnimationToolbar::FrameChanged()
{
  int32_t value =  GetFramectrl()->GetValue();
  if (value <= 0)
  {
    value = 1;
  }
    GetFramectrl()->SetValue(value);
  // Event Handler for TextCtrl->Press_Enter
  m_currentFrame = value - 1;
  GetSlider()->SetValue(m_currentFrame + 1);
  PostKeyframeChangedEvent(m_currentFrame);
}

//----------------------------------------
void CAnimationToolbar::OnLoop( wxCommandEvent &event )
{
  SetLoopPlayback(event.IsChecked());  
}

//----------------------------------------
void CAnimationToolbar::OnReset( wxCommandEvent &event )
{
  Reset();
    
}
//----------------------------------------
void CAnimationToolbar::Reset()
{
  //Event Handler Reset Button
  m_currentFrame = 0;
  GetSlider()->SetValue(m_currentFrame + 1);
  GetFramectrl()->SetValue(m_currentFrame + 1 );
  PostKeyframeChangedEvent(m_currentFrame);
    
}

//----------------------------------------
void CAnimationToolbar::OnScroll( wxScrollEvent &event )
{
  //Event Handler for Slider-Scroll
  m_currentFrame = GetSlider()->GetValue() - 1;
  GetFramectrl()->SetValue(m_currentFrame + 1);
  PostKeyframeChangedEvent(m_currentFrame);
    
}
//----------------------------------------
void CAnimationToolbar::OnAnimate( wxCommandEvent &event )
{
  Animate();
}

//----------------------------------------
void CAnimationToolbar::Animate()
{
  //Event Handler for Animate/Pause button """
  if (m_frames <= 1)
  {
      return;
  }
  if (m_running)
  {
    Pause();
  }
  else
  {
    if (m_currentFrame == (m_frames - 1))
    {
      Reset();
    }
    Start();
  }

}
//----------------------------------------
void CAnimationToolbar::OnPause( wxCommandEvent &event )
{
  Pause();
    
}
//----------------------------------------
void CAnimationToolbar::Pause()
{
  m_running = false;
  m_timer->Stop();
  GetPlay()->SetLabel("Animate");
  GetFramectrl()->Enable(true);
    
}
//----------------------------------------
void CAnimationToolbar::OnStart( wxCommandEvent &event )
{
  Start();
    
}
//----------------------------------------
void CAnimationToolbar::Start()
{
  m_running = true;
  m_timer->Start(m_interval);
  GetPlay()->SetLabel("Pause");
  GetFramectrl()->Enable(false);
    
}

//----------------------------------------
void CAnimationToolbar::OnDestroy( wxWindowDestroyEvent &event )
{
  Pause();
}




