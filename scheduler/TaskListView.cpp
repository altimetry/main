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

// Temporary hack to prevent the inclusion of Windows XML 
// headers, which collide with xerces
//
#if defined (WIN32) || defined (_WIN32)
//avoid later inclusion of Microsoft XML stuff, which causes name collisions with xerces
#define DOMDocument MsDOMDocument
#include <msxml.h>
#include <urlmon.h>
#undef DOMDocument
#endif

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "TaskListView.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "new-gui/Common/+Utils.h"

#include "SchedulerTaskConfig.h"
#include "BratSchedulerApp.h"

#include "TaskListView.h"

// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"


#if !wxCHECK_VERSION(2, 9, 0)
// anything before wxWidgets 2.9 will be adjusted
#define wxIntPtr long
#endif

// WDR: class implementations

static int wxCALLBACK TaskListCompareFunction(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData)
{

  CTaskListView* taskList = (CTaskListView*)(sortData);


  CBratTask* d1 = (CBratTask*)(item1);
  CBratTask* d2 = (CBratTask*)(item2);

  int result = 0;

  switch (taskList->GetSortedCol())
  {
  case TASKLISTCOL_UID :
    {
      if (d1->GetUid() > d2->GetUid())
      {
        result = 1;
      } 
      else
      {
        result = -1;
      }
      break;
    }
  case TASKLISTCOL_NAME :
    result = str_icmp( d1->GetName(), d2->GetName() );
    break;
  case TASKLISTCOL_START :
    {
      if (d1->GetAt() > d2->GetAt())
      {
        result = 1;
      } 
      else
      {
        result = -1;
      }
      break;
    }
  case TASKLISTCOL_STATUS :
    result = str_icmp( d1->GetStatusAsString(), d2->GetStatusAsString() );
    break;
  case TASKLISTCOL_CMD :
    result = str_icmp( d1->GetCmd(), d2->GetCmd() );
    break;
  case TASKLISTCOL_LOGFILE :
    result = str_icmp( d1->GetLogFile(), d2->GetLogFile() );
    break;
  default:
    break;
  }

  if (taskList->GetSortAsc() == false)
  {
    if (result > 0)
    {
      result = -1;
    }
    else if (result < 0)
    {
      result = 1;
    }
  }

  return result;
}
//----------------------------------------------------------------------------
// CTaskListView
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(CTaskListView,wxListView)

// WDR: event table for CTaskListView

BEGIN_EVENT_TABLE(CTaskListView,wxListView)
    EVT_KEY_UP( CTaskListView::OnKeyUp )
END_EVENT_TABLE()

//----------------------------------------
CTaskListView::CTaskListView( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxListView( parent, id, position, size, style )
{
  m_sortedCol = -1;
  m_sortAsc = true;
  m_data = NULL;
  m_bratTask = NULL;

  m_allowSuppr = true;

  m_imageListSmall = new wxImageList(16, 16, true);
  m_imageListSmall->Add( BitmapsList(0) );
  m_imageListSmall->Add( BitmapsList(1) );

  SetImageList(m_imageListSmall, wxIMAGE_LIST_SMALL);
  CreateColumn();

  CTaskListView::EvtListCtrl(*this, wxEVT_COMMAND_LIST_ITEM_SELECTED,
                               (wxListEventFunction)&CTaskListView::OnSelected,
                               NULL,
                               this);
  CTaskListView::EvtListCtrl(*this, wxEVT_COMMAND_LIST_ITEM_DESELECTED,
                               (wxListEventFunction)&CTaskListView::OnDeselected,
                               NULL,
                               this);
  CTaskListView::EvtListCtrl(*this, wxEVT_COMMAND_LIST_COL_CLICK,
                               (wxListEventFunction)&CTaskListView::OnColClick,
                               NULL,
                               this);
  CTaskListView::EvtListCtrl(*this, wxEVT_COMMAND_LIST_INSERT_ITEM,
                               (wxListEventFunction)&CTaskListView::OnInsertItem,
                               NULL,
                               this);
  wxSize bestsize = GetBestSize();
  bestsize.x = 100;
  SetMinSize(bestsize);
  CacheBestSize(bestsize);
}
//----------------------------------------
CTaskListView::~CTaskListView()
{
  if (m_imageListSmall != NULL)
  {
    delete m_imageListSmall;
    m_imageListSmall = NULL;
  }

}
//----------------------------------------
void CTaskListView::EvtListCtrl(wxWindow& window,  int32_t eventType, const wxListEventFunction& method,
                                wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(eventType,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}
//----------------------------------------
// WDR: handler implementations for CTaskListView
//----------------------------------------
//----------------------------------------
void CTaskListView::CreateColumn()
{ 
  wxListItem itemCol;
  itemCol.SetText(_T("Uid"));
  itemCol.SetWidth(130);

  //itemCol.SetImage(-1);
  //itemCol.SetMask(wxLIST_MASK_TEXT);

  itemCol.SetAlign(wxLIST_FORMAT_LEFT);
  this->InsertColumn(TASKLISTCOL_UID, itemCol);

  itemCol.SetText(_T("Name"));
  //don't use SetWidth(0), since this breaks on Mac OS X!
  itemCol.SetWidth(160);
  itemCol.SetAlign(wxLIST_FORMAT_CENTER);
  this->InsertColumn(TASKLISTCOL_NAME, itemCol);

  itemCol.SetText(_T("Start"));
  itemCol.SetWidth(150);
  itemCol.SetAlign(wxLIST_FORMAT_CENTER);
  this->InsertColumn(TASKLISTCOL_START, itemCol);

  itemCol.SetText(_T("Status"));
  itemCol.SetWidth(100);
  itemCol.SetAlign(wxLIST_FORMAT_CENTER);
  this->InsertColumn(TASKLISTCOL_STATUS, itemCol);

  itemCol.SetText(_T("Command line"));
  itemCol.SetWidth(700);
  itemCol.SetAlign(wxLIST_FORMAT_CENTER);
  this->InsertColumn(TASKLISTCOL_CMD, itemCol);

  itemCol.SetText(_T("Log file"));
  itemCol.SetWidth(700);
  itemCol.SetAlign(wxLIST_FORMAT_CENTER);
  this->InsertColumn(TASKLISTCOL_LOGFILE, itemCol);

}
//----------------------------------------
void CTaskListView::SetColumnImage(int col, int image)
{
#if !defined(__WXMAC__)
    wxListItem item;
    item.SetMask(wxLIST_MASK_IMAGE);
    item.SetImage(image);
    SetColumn(col, item);
#endif
}
//----------------------------------------
void CTaskListView::ResetColumnImage()
{
  for (int32_t i = 0 ; i < GetColumnCount() ; i ++)
  {
    SetColumnImage(i, -1);
  }

}
//----------------------------------------

void CTaskListView::OnInsertItem( wxListEvent &event )
{

  //CBratTask* bratTask =
    (CBratTask*)(event.GetData());

//  event.Veto();
}

//----------------------------------------
void CTaskListView::OnDeselected( wxListEvent &event )
{
  event.Skip();
}

//----------------------------------------
void CTaskListView::OnSelected( wxListEvent &event )
{
  event.Skip();
}

//----------------------------------------
void CTaskListView::OnColClick( wxListEvent &event )
{
  int32_t col = event.GetColumn();

  if (m_sortedCol != col)
  {
    ResetColumnImage();
    m_sortedCol = col;
    m_sortAsc = true;
  }
  else
  {
    m_sortAsc = !m_sortAsc;
  }

  SetColumnImage(col, m_sortAsc ? 1 : 0);

  SortItems(TaskListCompareFunction, (long)this);

  event.Skip();

}

//----------------------------------------
void CTaskListView::OnKeyUp( wxKeyEvent &event )
{
  long keycode = event.GetKeyCode();

  switch ( keycode )
  {
    case WXK_DELETE:
    case WXK_NUMPAD_DELETE:
    {
      if (!event.HasModifiers())
      {
      }
      break;
    }

    default:
    {
      break;
    }
  }

}
//----------------------------------------
void CTaskListView::AddTasks(const CMapBratTask* data)
{
  if (data == NULL)
  {
    return;
  }

  CMapBratTask::const_iterator it;

  for (it = data->begin(); it != data->end(); it++)
  {
    CBratTask* bratTask = it->second;
    InsertTask(bratTask);
  }

  SortItems(TaskListCompareFunction, (long)this);

}
//----------------------------------------
void CTaskListView::InsertTasks(CMapBratTask* data)
{
  m_data = data;

  DeleteAllItems();

  if (m_data == NULL)
  {
    return;
  }

  CMapBratTask::iterator it;

  for (it = m_data->begin(); it != m_data->end(); it++)
  {
    CBratTask* bratTask = it->second;
    InsertTask(bratTask);
  }
}
//----------------------------------------
long CTaskListView::InsertTask(CBratTask* bratTask)
{
  int32_t index = GetItemCount();

  if (bratTask == NULL)
  {
    return -1;
  }


  wxListItem itemRow;
  itemRow.SetText(bratTask->GetUidAsString());
  itemRow.SetMask(wxLIST_MASK_TEXT);
  itemRow.SetImage(-1);
  itemRow.SetData(bratTask);
  itemRow.SetId(index);

  long indexInserted = InsertItem(itemRow);


  SetItem(index, TASKLISTCOL_NAME, bratTask->GetName());
  SetItem(index, TASKLISTCOL_START, bratTask->GetAtAsString());
  SetItem(index, TASKLISTCOL_STATUS, bratTask->GetStatusAsString());
  SetItem(index, TASKLISTCOL_CMD, bratTask->GetCmd());
  SetItem(index, TASKLISTCOL_LOGFILE, bratTask->GetLogFile());

  return indexInserted;

}
//----------------------------------------
bool CTaskListView::GetSelectedItems(CVectorBratTask& selectedTasks)
{
  selectedTasks.SetDelete(false);

  long item = GetNextItem(-1, wxLIST_NEXT_ALL,
                              wxLIST_STATE_SELECTED);
  while ( item >= 0 )
  {
    CBratTask* data = (CBratTask*)(GetItemData(item));
    if (data != NULL)
    {
      selectedTasks.Insert(data);
    }


    item = GetNextItem(item, wxLIST_NEXT_ALL,
                              wxLIST_STATE_SELECTED);

  }

  return (selectedTasks.size() > 0);
}

//----------------------------------------
bool CTaskListView::RemoveTasks(bool ask /* = true */)
{

  if (!m_allowSuppr)
  {
    return true;
  }

  if (ask)
  {
    bool cancel = AskToRemove();
    if (cancel)
    {
      return true;
    }
  }


  int32_t selCount = GetSelectedItemCount();

  std::vector<long> itemToDelete;
  std::vector<wxLongLong_t> dataKeyToDelete;

  long item = GetNextItem(-1, wxLIST_NEXT_ALL,
                              wxLIST_STATE_SELECTED);
  while ( selCount != 0 )
  {
    CBratTask* data = (CBratTask*)(GetItemData(item));
    if (data != NULL)
    {
      //We just inspect the items to delete.
      //They will be deleted after the file is sucessfully saved.
      Select(item, false);
      dataKeyToDelete.push_back(data->GetUid());
      itemToDelete.push_back(item);
    }

    selCount = GetSelectedItemCount();

    item = GetNextItem(-1, wxLIST_NEXT_ALL,
                              wxLIST_STATE_SELECTED);

  }

  // Nothing to delete ==> return
  if (dataKeyToDelete.size() <= 0)
  {
    return true;
  }

  bool saved = false;

  try
  {
    //---------------------
    wxGetApp().StopCheckConfigFileTimer();
    //---------------------

    bool loaded = CSchedulerTaskConfig::LoadAllSchedulerTaskConfig();

    if (!loaded)
    {    
      //---------------------
      wxGetApp().StartCheckConfigFileTimer();
      //---------------------
      return true;
    }

    std::vector<wxLongLong_t>::const_iterator itData;
    for (itData = dataKeyToDelete.begin(); itData != dataKeyToDelete.end() ; itData++)
    {
      try
      {
        CSchedulerTaskConfig::GetInstance()->RemoveTask(*itData);
      }
      catch(CException& e)
      {
        wxMessageBox(e.GetMessage().c_str(),
		                 "Warning",
		                  wxOK | wxCENTRE | wxICON_EXCLAMATION);
      }
    }
 
    saved = CSchedulerTaskConfig::SaveSchedulerTaskConfig();

    wxGetApp().ResetConfigFileChange();    

  }
  catch(CException& e)
  {
    //wxGetApp().UnLockConfigFile();
    wxMessageBox(e.GetMessage().c_str(),
		              "Warning",
		              wxOK | wxCENTRE | wxICON_EXCLAMATION);

  }  

  //---------------------
  wxGetApp().StartCheckConfigFileTimer();
  //---------------------


  // Now really delete items from list (start from the end)
  int32_t size = static_cast<int32_t>(itemToDelete.size());

  for (int32_t i = size - 1; i >= 0 ; i--)
  {
    DeleteItem(itemToDelete.at(i));
  }

  return saved;

}

//----------------------------------------
bool CTaskListView::RemoveTasksFromList(wxLongLong_t id)
{
  bool bOk = false;

  long item = GetNextItem(-1, wxLIST_NEXT_ALL,
                              wxLIST_STATE_DONTCARE);
  while ( item >= 0 )
  {
    CBratTask* data = (CBratTask*)(GetItemData(item));
    if (data != NULL)
    {
      if (data->GetUid() == id)
      {
        bOk = DeleteItem(item);
        break;
      }

    }

    item = GetNextItem(item, wxLIST_NEXT_ALL,
                              wxLIST_STATE_DONTCARE);

  }

  return bOk;
}
//----------------------------------------
void CTaskListView::DeselectAll()
{

  int32_t selCount = GetSelectedItemCount();

  long item = GetNextItem(-1, wxLIST_NEXT_ALL,
                              wxLIST_STATE_SELECTED);
  while ( selCount != 0 )
  {
    Select(item, false);

    selCount = GetSelectedItemCount();

    item = GetNextItem(-1, wxLIST_NEXT_ALL,
                              wxLIST_STATE_SELECTED);

  }

}
//----------------------------------------
void CTaskListView::SelectAll()
{

  long item = GetNextItem(-1, wxLIST_NEXT_ALL,
                              wxLIST_STATE_DONTCARE);
  while ( item >= 0 )
  {
    Select(item, true);
    item = GetNextItem(item, wxLIST_NEXT_ALL,
                              wxLIST_STATE_DONTCARE);

  }

}

//----------------------------------------
bool CTaskListView::AskToRemove()
{
  bool cancel = false;

  int32_t result = wxMessageBox("You are about to delete tasks. Are you sure ?\n"
                                 "Click Yes to process\n"
                                 "Click No to cancel\n",
                               "Question",
                                wxYES_NO | wxCENTRE | wxICON_QUESTION);

  cancel = (result == wxNO);

  return cancel;
}
//----------------------------------------
