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
    #pragma implementation "PendingPanel.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "SchedulerTaskConfig.h"
#include "BratScheduler.h"
#include "PendingPanel.h"

// When debugging changes all calls to “new” to be calls to “DEBUG_NEW” allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// CPendingPanel
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(CPendingPanel,wxPanel)

// WDR: event table for CPendingPanel

BEGIN_EVENT_TABLE(CPendingPanel,wxPanel)
    EVT_WINDOW_DESTROY(CPendingPanel::OnDestroy )
    EVT_BUTTON( ID_CLEAR_PENDING, CPendingPanel::OnClear )
    EVT_BUTTON( ID_REMOVE_PENDING, CPendingPanel::OnRemove )
END_EVENT_TABLE()

CPendingPanel::CPendingPanel( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxPanel( parent, id, position, size, style )
{

  wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

  //----------------------------
  wxSizer* sizer = PendingListPanel( this, false, true );
  //----------------------------

  item0->Add( this, 1, wxGROW|wxALL, 5 );

  parent->SetSizer( item0 );

  LoadTasks();

  InstallEventListeners();

}
//----------------------------------------
CPendingPanel::~CPendingPanel()
{
}
//----------------------------------------
void CPendingPanel::ClearTaskList()
{
  GetListPending()->InsertTasks(NULL);
}

//----------------------------------------
void CPendingPanel::LoadTasks()
{
  wxLogInfo("Loading pending tasks...");

  ClearTaskList();

  GetListPending()->InsertTasks(CSchedulerTaskConfig::GetInstance()->GetMapPendingBratTask());

  wxLogInfo("%d pending task(s) loaded.",  GetListPending()->GetItemCount());


  EnableCtrl();

}
//----------------------------------------
void CPendingPanel::EnableCtrl()
{

}//----------------------------------------
void CPendingPanel::EnableCtrl(bool enable)
{
  GetClearPending()->Enable(enable);
  GetRemovePending()->Enable(enable);
  GetListPending()->Enable(enable);
}

//----------------------------------------
void CPendingPanel::RemoveTasks()
{
  //---------------------
  wxGetApp().StopSchedulerTimer();
  wxGetApp().StopCheckConfigFileTimer();
  //---------------------
  bool saved = GetListPending()->RemoveTasks();
  if (!saved)
  {
    wxGetApp().AskUserToRestartApplication();
  }
  //---------------------
  wxGetApp().StartSchedulerTimer();
  wxGetApp().StartCheckConfigFileTimer();
  //---------------------
}

//----------------------------------------

// WDR: handler implementations for CPendingPanel
//----------------------------------------
void CPendingPanel::OnDestroy( wxWindowDestroyEvent &event )
{
  DeInstallEventListeners();
}
//----------------------------------------
void CPendingPanel::OnRemove( wxCommandEvent &event )
{
  RemoveTasks();
}
//----------------------------------------
void CPendingPanel::OnClear( wxCommandEvent &event )
{
  GetListPending()->SelectAll();
  RemoveTasks();
}
//----------------------------------------
void CPendingPanel::OnCheckFileChange(CCheckFileChangeEvent& event)
{
  try
  {
    EnableCtrl(false);

    if (GetListPending()->GetData() == NULL)
    {
      GetListPending()->SetData(CSchedulerTaskConfig::GetInstance()->GetMapPendingBratTask());
    }

    GetListPending()->AddTasks(CSchedulerTaskConfig::GetInstance()->GetMapNewBratTask());
  }
  catch(CException& e)
  {
    wxLogError("%s",e.what());

  }

  EnableCtrl(true);

}
//----------------------------------------
void CPendingPanel::InstallEventListeners()
{
  CCheckFileChange::EvtCheckFileChangeCommand(*this,
                                (CCheckFileChangeEventFunction)&CPendingPanel::OnCheckFileChange, NULL, this);
}
//----------------------------------------
void CPendingPanel::DeInstallEventListeners()
{
  CCheckFileChange::DisconnectEvtCheckFileChangeCommand(*this);
}
//----------------------------------------




