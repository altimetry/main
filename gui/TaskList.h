
/*
* Copyright (C) 2005 Collecte Localisation Satellites (CLS), France (31)
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

#ifndef __TaskList_H__
#define __TaskList_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "TaskList.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/listctrl.h"


#include "Process.h"

using namespace brathl;



// WDR: class declarations

//-------------------------------------------------------------
//------------------- CTaskList class --------------------
//-------------------------------------------------------------
const int32_t PROCCOL_NAME = 0;
const int32_t PROCCOL_CMD = 1;

class CTaskList: public wxListView
{
public:
  // constructors and destructors
  CTaskList( wxWindow *parent, wxWindowID id = -1,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = 0,
      const wxValidator& validator = wxDefaultValidator,
      const wxString &name = wxListCtrlNameStr);

  virtual ~CTaskList();

  int32_t GetSortedCol() {return m_sortedCol;};
  bool GetSortAsc() {return m_sortAsc;};

  void InsertProcess(CProcess* process);
  void RemoveProcess(CProcess* process);

  static void EvtListCtrl(wxWindow& window,  int32_t eventType, const wxListEventFunction& method,
                        wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);

    // WDR: method declarations for CTaskList
    
private:
  CMapProcess* m_processes;
  int32_t m_sortedCol;
  bool m_sortAsc;
    // WDR: member variable declarations for CTaskList
    
private:
  void CreateColumn();
  void SetColumnImage(int col, int image);
  void ResetColumnImage();

  // WDR: handler declarations for CTaskList
    void OnDeselected( wxListEvent &event );
    void OnSelected( wxListEvent &event );
  void OnColClick( wxListEvent &event );
  
private:
    DECLARE_CLASS(CTaskList)
    DECLARE_EVENT_TABLE()
};


#endif
