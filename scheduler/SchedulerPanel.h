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

#ifndef __SchedulerPanel_H__
#define __SchedulerPanel_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "SchedulerPanel.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "BratScheduler_wdr.h"

#include "PendingPanel.h"
#include "ProcessingPanel.h"
#include "EndedPanel.h"
#include "LogPanel.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// CSchedulerPanel
//----------------------------------------------------------------------------

class CSchedulerPanel: public wxPanel
{
public:
  // constructors and destructors
  CSchedulerPanel( wxWindow *parent, wxWindowID id = -1,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxTAB_TRAVERSAL | wxNO_BORDER );
  virtual ~CSchedulerPanel();
  
  // WDR: method declarations for CSchedulerPanel
  CEndedPanel* GetEndedPanel()  { return (CEndedPanel*) FindWindow( ID_ENDEDPANEL ); }
  CProcessingPanel* GetProcessingPanel()  { return (CProcessingPanel*) FindWindow( ID_PROCESSINGPANEL ); }
  CPendingPanel* GetPendingPanel()  { return (CPendingPanel*) FindWindow( ID_PENDINGPANEL ); }
  CLogPanel* GetLogPanel()  { return (CLogPanel*) FindWindow( ID_LOGPANEL ); }
  wxNotebook* GetMainnotebook()  { return (wxNotebook*) FindWindow( ID_MAINNOTEBOOK ); }

  wxLog* GetActiveLog() { return m_pLog; };
  wxLog* GetPrevLog() { return m_pLogPrev; };


private:
  wxLog *m_pLog;
  wxLog *m_pLogPrev;

    // WDR: member variable declarations for CSchedulerPanel
    
private:
    // WDR: handler declarations for CSchedulerPanel
    void OnDestroy( wxWindowDestroyEvent &event );

private:
  DECLARE_CLASS(CSchedulerPanel)
  DECLARE_EVENT_TABLE()

};




#endif
