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

#ifndef __LogPanel_H__
#define __LogPanel_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "LogPanel.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "brathl.h"

#include "BratScheduler_wdr.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// CLogPanel
//----------------------------------------------------------------------------

class CLogPanel: public wxPanel
{
public:
  // constructors and destructors
  CLogPanel( wxWindow *parent, wxWindowID id = -1,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxTAB_TRAVERSAL | wxNO_BORDER );
    
  virtual ~CLogPanel();
    
  void EnableCtrl();
  
  void PartialClear();

  // WDR: method declarations for CLogPanel
  wxTextCtrl* GetLogMess()  { return (wxTextCtrl*) FindWindow( ID_LOG_MESS ); }
    
private:
  int32_t m_numLinesMax;
  int32_t m_numLines;
  wxTimer m_removeLinesTimer;
  
  int32_t m_lineEndingLength;

    // WDR: member variable declarations for CLogPanel
    
private:
  // WDR: handler declarations for CLogPanel
  void OnTimer(wxTimerEvent& event);
  void OnLogText( wxCommandEvent &event );
  void OnClear( wxCommandEvent &event );
  void OnDestroy( wxWindowDestroyEvent &event );

private:
  DECLARE_CLASS(CLogPanel)
  DECLARE_EVENT_TABLE()
};




#endif
