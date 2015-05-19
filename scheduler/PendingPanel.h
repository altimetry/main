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

#ifndef __PendingPanel_H__
#define __PendingPanel_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "PendingPanel.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "BratScheduler_wdr.h"

#include "TaskListView.h"
#include "SchedulerTimer.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// CPendingPanel
//----------------------------------------------------------------------------

class CPendingPanel: public wxPanel
{
public:
  // constructors and destructors
  CPendingPanel( wxWindow *parent, wxWindowID id = -1,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxTAB_TRAVERSAL | wxNO_BORDER );

  virtual ~CPendingPanel();

  void EnableCtrl();
  void EnableCtrl(bool enable);
  
  void ClearTaskList();
  void LoadTasks();

  // WDR: method declarations for CPendingPanel
  wxButton* GetRemovePending()  { return (wxButton*) FindWindow( ID_REMOVE_PENDING ); }
  wxButton* GetClearPending()  { return (wxButton*) FindWindow( ID_CLEAR_PENDING ); }
  CTaskListView* GetListPending()  { return (CTaskListView*) FindWindow( ID_LIST_PENDING ); }
    
private:
    // WDR: member variable declarations for CPendingPanel
    
private:

  void InstallEventListeners();
  void DeInstallEventListeners();
  void RemoveTasks();

  // WDR: handler declarations for CPendingPanel
  void OnRemove( wxCommandEvent &event );
  void OnClear( wxCommandEvent &event );
  void OnCheckFileChange(CCheckFileChangeEvent& event);
  void OnDestroy( wxWindowDestroyEvent &event );

private:
  DECLARE_CLASS(CPendingPanel)
  DECLARE_EVENT_TABLE()
};




#endif
