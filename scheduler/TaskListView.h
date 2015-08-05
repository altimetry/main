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


#ifndef __TaskListView_H__
#define __TaskListView_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "TaskListView.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/imaglist.h"

#include "brathl.h"

#include "BratScheduler_wdr.h"

#include "BratTask.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// CTaskListView
//----------------------------------------------------------------------------
const int32_t TASKLISTCOL_UID = 0;
const int32_t TASKLISTCOL_NAME = 1;
const int32_t TASKLISTCOL_START = 2;
const int32_t TASKLISTCOL_STATUS = 3;
const int32_t TASKLISTCOL_CMD = 4;
const int32_t TASKLISTCOL_LOGFILE = 5;

class CTaskListView: public wxListView
{
public:
  // constructors and destructors
  CTaskListView( wxWindow *parent, wxWindowID id = -1,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = 0 );

  virtual ~CTaskListView();

  bool GetSelectedItems(CVectorBratTask& selectedTasks);
  
  CMapBratTask* GetData() {return m_data;};
  void SetData(CMapBratTask* value) {m_data = value;};

  void InsertTasks(CMapBratTask* data);
  long InsertTask(CBratTask* bratTask);
  
  bool RemoveTasks(bool ask = true);
  
  bool RemoveTasksFromList(wxLongLong_t id);

  bool AskToRemove();

  void AddTasks(CMapBratTask* data);

  void SelectAll();
  void DeselectAll();

  int32_t GetSortedCol() {return m_sortedCol;};
  bool GetSortAsc() {return m_sortAsc;};

  bool GetAllowSuppr() { return m_allowSuppr; };
  void SetAllowSuppr(bool value) { m_allowSuppr = value; };

  static void EvtListCtrl(wxWindow& window,  int32_t eventType, const wxListEventFunction& method,
                        wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);

  // WDR: method declarations for CTaskListView
    
private:
  wxImageList *m_imageListSmall;
  CMapBratTask* m_data;

  CBratTask* m_bratTask;

  int32_t m_sortedCol;
  bool m_allowSuppr;

  bool m_sortAsc;
  // WDR: member variable declarations for CTaskListView
    
private:
  void CreateColumn();
  void SetColumnImage(int col, int image);
  void ResetColumnImage();

  // WDR: handler declarations for CTaskListView
  void OnKeyUp( wxKeyEvent &event );
  void OnInsertItem( wxListEvent &event );
  void OnDeselected( wxListEvent &event );
  void OnSelected( wxListEvent &event );
  void OnColClick( wxListEvent &event );

private:
  DECLARE_CLASS(CTaskListView)
  DECLARE_EVENT_TABLE()
};




#endif
