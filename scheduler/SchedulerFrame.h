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

#ifndef __SchedulerFrame_H__
#define __SchedulerFrame_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "SchedulerFrame.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/docview.h"

#include "BratScheduler_wdr.h"

#include "SchedulerTimer.h"
#include "SchedulerPanel.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// CSchedulerFrame
//----------------------------------------------------------------------------

class CSchedulerFrame: public wxFrame
{
public:
  // constructors and destructors
  CSchedulerFrame( wxWindow *parent, wxWindowID id, const wxString &title,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxDEFAULT_FRAME_STYLE );
  virtual ~CSchedulerFrame();
  
  // WDR: method declarations for CSchedulerFrame

  CSchedulerPanel* GetSchedulerPanel() {return m_schedulerPanel;};
  
  CLogPanel* GetLogPanel()  { return  m_schedulerPanel->GetLogPanel(); };
  CEndedPanel* GetEndedPanel()  { return  m_schedulerPanel->GetEndedPanel(); };
  CPendingPanel* GetPendingPanel()  { return  m_schedulerPanel->GetPendingPanel(); };
  CProcessingPanel* GetProcessingPanel()  { return  m_schedulerPanel->GetProcessingPanel(); };

  CTaskListView* GetListPending()  { return  GetPendingPanel()->GetListPending(); };
  CTaskListView* GetListProcessing()  { return  GetProcessingPanel()->GetListProcessing(); };
  CTaskListView* GetListEnded()  { return  GetEndedPanel()->GetListEnded(); };

  wxNotebook* GetMainnotebook()  { return (wxNotebook*) FindWindow( ID_MAINNOTEBOOK ); }

  void SetTitle();
  
  //bool LoadTasks();

private:

  //CSchedulerTimer m_schedulerTimer;
  //wxTimer m_checkConfigFileTimer;

  //long m_schedulerTimerId;
  //long m_checkConfigFileTimerId;


  // WDR: member variable declarations for CSchedulerFrame
  CSchedulerPanel* m_schedulerPanel;

  wxMenuBar* m_menuBar;


private:
  bool AskToExit();

    
private:
  // WDR: handler declarations for CSchedulerFrame
    void OnViewConfiguration( wxCommandEvent &event );
    void OnUserManual( wxCommandEvent &event );
    void OnAbout( wxCommandEvent &event );
    void OnQuit( wxCommandEvent &event );
    void OnCloseWindow( wxCloseEvent &event );

  void CreateMainMenuBar();

private:
    DECLARE_CLASS(CSchedulerFrame)
    DECLARE_EVENT_TABLE()
};




#endif
