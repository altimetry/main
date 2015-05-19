/////////////////////////////////////////////////////////////////////////////
// Name:        WindowHandler.cpp
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "WindowHandler.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#include "wx/settings.h"

#include "WindowHandler.h"

int32_t CWindowHandler::m_offset = 25;


// WDR: class implementations

CWindowHandler::CWindowHandler()
{

}

//----------------------------------------
CWindowHandler::~CWindowHandler()
{


}

//----------------------------------------
wxSize CWindowHandler::GetDefaultSize()
{
  return wxSize(800, 494);
}


//----------------------------------------
void CWindowHandler::GetSizeAndPosition(wxSize& size, wxPoint& pos)
{
  if (size == wxDefaultSize)
  {
    size = GetDefaultSize();
  }

  if (pos == wxDefaultPosition)
  {
    return;
  }
  int32_t xmax = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
  int32_t ymax = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
  pos.x += m_offset;
  pos.y += m_offset;
  //test if win stays within window
  if ( ((size.GetWidth() + pos.x) > xmax - m_offset) || ((size.GetHeight() + pos.y) > ymax - m_offset) )
  {
    pos = wxDefaultPosition;
  }
}
