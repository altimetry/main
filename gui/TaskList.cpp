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
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "TaskList.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


#ifdef __BORLANDC__
    #pragma hdrstop
#endif


#include "TaskList.h"



// WDR: class implementations

//-------------------------------------------------------------
//------------------- CTaskList class --------------------
//-------------------------------------------------------------

IMPLEMENT_CLASS(CTaskList,wxListView)

// WDR: event table for CTaskList

BEGIN_EVENT_TABLE(CTaskList,wxListView)
    //EVT_LIST_COL_CLICK( ID_DICTLIST, CTaskList::OnColClick )
    //EVT_LIST_ITEM_SELECTED( ID_DICTLIST, CTaskList::OnSelected )
    //EVT_LIST_ITEM_DESELECTED( ID_DICTLIST, CTaskList::OnDeselected )
END_EVENT_TABLE()

//----------------------------------------
CTaskList::CTaskList( wxWindow *parent, wxWindowID id,
                                  const wxPoint &position, const wxSize& size, long style, 
                                  const wxValidator& validator,
                                  const wxString &name) 
      : wxListView( parent, id, position, size, style, validator, name)
{

  m_sortedCol = -1;
  m_sortAsc = true;

  /*
  m_imageListSmall = new wxImageList(16, 16, true);
  m_imageListSmall->Add( BitmapsList(0) );
  m_imageListSmall->Add( BitmapsList(1) );

  SetImageList(m_imageListSmall, wxIMAGE_LIST_SMALL);
  */
  CreateColumn();

  CTaskList::EvtListCtrl(*this, wxEVT_COMMAND_LIST_ITEM_SELECTED,
                               (wxListEventFunction)&CTaskList::OnSelected,
                               NULL,
                               this);
  CTaskList::EvtListCtrl(*this, wxEVT_COMMAND_LIST_ITEM_DESELECTED,
                               (wxListEventFunction)&CTaskList::OnDeselected,
                               NULL,
                               this);
  CTaskList::EvtListCtrl(*this, wxEVT_COMMAND_LIST_COL_CLICK,
                               (wxListEventFunction)&CTaskList::OnColClick,
                               NULL,
                               this);


}

//----------------------------------------
CTaskList::~CTaskList()
{
}
//----------------------------------------
void CTaskList::CreateColumn()
{

  wxListItem itemCol;
  itemCol.SetText(_T("Name"));
  itemCol.SetWidth(300); 
  itemCol.SetAlign(wxLIST_FORMAT_CENTER);
  this->InsertColumn(PROCCOL_NAME, itemCol);
  
  itemCol.SetText(_T("Command line"));
  itemCol.SetWidth(500);
  itemCol.SetAlign(wxLIST_FORMAT_CENTER);
  this->InsertColumn(PROCCOL_CMD, itemCol);


}
//----------------------------------------
void CTaskList::ResetColumnImage()
{
  for (int32_t i = 0 ; i < GetColumnCount() ; i ++)
  {
    SetColumnImage(i, -1);
  }
  
}
//----------------------------------------
void CTaskList::InsertProcess(CProcess* process)
{

  int32_t index = GetItemCount();

  wxListItem itemRow;
  itemRow.SetText(process->GetName().c_str());
  itemRow.SetMask(wxLIST_MASK_TEXT);
  itemRow.SetImage(-1);
  itemRow.SetData(process);
  itemRow.SetId(index);
  
  long tmp = InsertItem(itemRow);

  SetItem(index, PROCCOL_CMD, process->GetCmd().c_str());

}
//----------------------------------------
void CTaskList::RemoveProcess(CProcess* process)
{
  long item = FindItem(-1, (long)process);
  if (item < 0)
  {
    return;
  }
  DeleteItem(item);
}
//----------------------------------------
void CTaskList::SetColumnImage(int col, int image)
{
    wxListItem item;
    item.SetMask(wxLIST_MASK_IMAGE);
    item.SetImage(image);
    SetColumn(col, item);
}



// WDR: handler implementations for CTaskList

//----------------------------------------
void CTaskList::OnDeselected( wxListEvent &event )
{
  event.Skip();
}

//----------------------------------------
void CTaskList::OnSelected( wxListEvent &event )
{
  event.Skip();
}

//----------------------------------------
void CTaskList::OnColClick( wxListEvent &event )
{

  event.Skip();
    
}

//----------------------------------------
void CTaskList::EvtListCtrl(wxWindow& window,  int32_t eventType, const wxListEventFunction& method,
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

