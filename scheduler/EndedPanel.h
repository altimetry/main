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

#ifndef __EndedPanel_H__
#define __EndedPanel_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "EndedPanel.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "BratScheduler_wdr.h"

#include "TaskListView.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// CEndedPanel
//----------------------------------------------------------------------------

class CEndedPanel: public wxPanel
{
public:
  // constructors and destructors
  CEndedPanel( wxWindow *parent, wxWindowID id = -1,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxTAB_TRAVERSAL | wxNO_BORDER );

  virtual ~CEndedPanel();
  
  void EnableCtrl();
  void EnableCtrl(bool enable);

  // WDR: method declarations for CEndedPanel
  wxButton* GetRemoveEnded()  { return (wxButton*) FindWindow( ID_REMOVE_ENDED ); }
  wxButton* GetClearEnded()  { return (wxButton*) FindWindow( ID_CLEAR_ENDED ); }
  wxButton* GetShowLog()  { return (wxButton*) FindWindow( ID_SHOW_LOG ); }
  CTaskListView* GetListEnded()  { return (CTaskListView*) FindWindow( ID_LIST_ENDED ); }
    
private:
    // WDR: member variable declarations for CEndedPanel
    
private:
  void ClearTaskList();
  void LoadTasks();

  // WDR: handler declarations for CEndedPanel
  void OnShowLog( wxCommandEvent &event );
  void OnClear( wxCommandEvent &event );
  void OnRemove( wxCommandEvent &event );
  void OnDestroy( wxWindowDestroyEvent &event );

private:
    DECLARE_CLASS(CEndedPanel)
    DECLARE_EVENT_TABLE()
};




#endif
