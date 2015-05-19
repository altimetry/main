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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "EndedPanel.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "SchedulerTaskConfig.h"
#include "BratScheduler.h"
#include "EndedPanel.h"


// When debugging changes all calls to “new” to be calls to “DEBUG_NEW” allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// CEndedPanel
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(CEndedPanel,wxPanel)

// WDR: event table for CEndedPanel

BEGIN_EVENT_TABLE(CEndedPanel,wxPanel)
    EVT_WINDOW_DESTROY(CEndedPanel::OnDestroy )
    EVT_BUTTON( ID_REMOVE_ENDED, CEndedPanel::OnRemove )
    EVT_BUTTON( ID_CLEAR_ENDED, CEndedPanel::OnClear )
    EVT_BUTTON( ID_SHOW_LOG, CEndedPanel::OnShowLog )
END_EVENT_TABLE()
//----------------------------------------
CEndedPanel::CEndedPanel( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxPanel( parent, id, position, size, style )
{
  wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

  //----------------------------
  wxSizer* sizer = EndedListPanel( this, false, true );
  //----------------------------
  
  item0->Add( this, 1, wxGROW|wxALL, 5 );

  parent->SetSizer( item0 );

  LoadTasks();

}
//----------------------------------------
CEndedPanel::~CEndedPanel()
{
}
//----------------------------------------
void CEndedPanel::OnDestroy( wxWindowDestroyEvent &event )
{
}
//----------------------------------------
void CEndedPanel::ClearTaskList()
{
  GetListEnded()->InsertTasks(NULL);
}
//----------------------------------------
void CEndedPanel::LoadTasks()
{
  wxLogInfo("Loading ended tasks...");
  
  ClearTaskList();
  
  GetListEnded()->InsertTasks(CSchedulerTaskConfig::GetInstance()->GetMapEndedBratTask());
  
  wxLogInfo("%d ended task(s) loaded.",  GetListEnded()->GetItemCount());


  EnableCtrl();

}
//----------------------------------------
void CEndedPanel::EnableCtrl()
{

}
//----------------------------------------
void CEndedPanel::EnableCtrl(bool enable)
{
  GetShowLog()->Enable(enable);
  GetRemoveEnded()->Enable(enable);
  GetClearEnded()->Enable(enable);
  GetListEnded()->Enable(enable);
}  

//----------------------------------------
// WDR: handler implementations for CEndedPanel
//----------------------------------------
void CEndedPanel::OnShowLog( wxCommandEvent &event )
{
  CVectorBratTask selectedTasks(false);
  
  bool bOk = GetListEnded()->GetSelectedItems(selectedTasks);
  if (!bOk)
  {
    return;
  }

  CVectorBratTask::const_iterator it;
  for (it = selectedTasks.begin() ; it != selectedTasks.end() ; it++)
  {
    CBratTask* bratTask = *it;
    if (bratTask == NULL)
    {
      continue;
    }

    wxString logFile = bratTask->GetLogFileAsString();
    CBratSchedulerApp::ViewFileGeneric(logFile, this);
  }

    
}
//----------------------------------------
void CEndedPanel::OnClear( wxCommandEvent &event )
{
  GetListEnded()->SelectAll();    
  bool saved = GetListEnded()->RemoveTasks();    
  if (!saved)
  {
    wxGetApp().AskUserToRestartApplication();
  }
    
}
//----------------------------------------
void CEndedPanel::OnRemove( wxCommandEvent &event )
{
  bool saved = GetListEnded()->RemoveTasks();    
  if (!saved)
  {
    wxGetApp().AskUserToRestartApplication();
  }
}
//----------------------------------------




