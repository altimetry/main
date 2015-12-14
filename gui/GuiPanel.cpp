/*
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

#include "new-gui/brat/stdafx.h"

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif


#include "BratGui.h"
#include "GuiPanel.h"

DEFINE_EVENT_TYPE(wxEVT_NEW_DATASET)
DEFINE_EVENT_TYPE(wxEVT_RENAME_DATASET)
DEFINE_EVENT_TYPE(wxEVT_DELETE_DATASET)
DEFINE_EVENT_TYPE(wxEVT_NEW_OPERATION)
DEFINE_EVENT_TYPE(wxEVT_RENAME_OPERATION)
DEFINE_EVENT_TYPE(wxEVT_DELETE_OPERATION)
DEFINE_EVENT_TYPE(wxEVT_NEW_DATAEXPR)
DEFINE_EVENT_TYPE(wxEVT_DELETE_DATAEXPR)
DEFINE_EVENT_TYPE(wxEVT_DATASET_FILES_CHANGED)



// WDR: class implementations

//----------------------------------------------------------------------------
// CGuiPanel
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(CGuiPanel,wxPanel)

// WDR: event table for CGuiPanel

BEGIN_EVENT_TABLE(CGuiPanel,wxPanel)
    EVT_NOTEBOOK_PAGE_CHANGING(wxID_ANY, CGuiPanel::OnPageChanging)
    EVT_NOTEBOOK_PAGE_CHANGED(wxID_ANY, CGuiPanel::OnPageChanged)
END_EVENT_TABLE()

//----------------------------------------
CGuiPanel::CGuiPanel( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxPanel( parent, id, position, size, style )
{
  //m_propertyNotebook = new wxNotebook(this, ID_MAINNOTEBOOK);
  //wxSizer *item0 = MainNotebook(this);

  wxString lastPageReached = wxGetApp().GetLastPageReached();

  wxBoxSizer *boxSizer = new wxBoxSizer( wxHORIZONTAL );

  wxNotebook *mainNoteBook = new wxNotebook( this, ID_MAINNOTEBOOK, wxDefaultPosition, wxDefaultSize, 0 );

  CDatasetPanel *datasetPanel = new CDatasetPanel( mainNoteBook, ID_DATASETSPLITTER, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE|wxSP_3DSASH|wxCLIP_CHILDREN );
  mainNoteBook->AddPage(datasetPanel, DATASETS_PAGE_NAME);

  //COperationPanel *operationPanel = new COperationPanel( mainNoteBook, ID_OPERATIONPANEL, wxDefaultPosition, wxDefaultSize, 0 );
  //mainNoteBook->AddPage(operationPanel, OPERATIONS_PAGE_NAME);

  COperationPanel *operationPanel = new COperationPanel( mainNoteBook, ID_OPERATIONSPLITTER, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE|wxCLIP_CHILDREN  );
  mainNoteBook->AddPage(operationPanel, OPERATIONS_PAGE_NAME);
  
  //CDisplayPanel *displayPanel = new CDisplayPanel( mainNoteBook, ID_DISPLAYPANEL, wxDefaultPosition, wxDefaultSize, 0 );
  //mainNoteBook->AddPage(displayPanel, DISPLAY_PAGE_NAME);

  CDisplayPanel *displayPanel = new CDisplayPanel( mainNoteBook, ID_DISPLAYPANEL, wxDefaultPosition, wxDefaultSize, 0 );
  mainNoteBook->AddPage(displayPanel, DISPLAY_PAGE_NAME);

  CLogPanel *logPanel = new CLogPanel( mainNoteBook, ID_LOGPANEL, wxDefaultPosition, wxDefaultSize, 0 );
  mainNoteBook->AddPage(logPanel, LOG_PAGE_NAME);

  boxSizer->Add( mainNoteBook, 1, wxGROW|wxALL, 5 );

  wxGetApp().SetLastPageReached(lastPageReached);

  SetSizer( boxSizer );

  CGuiPanel::EvtNewDatasetCommand(*this, 
                                 (CNewDatasetEventFunction)&CGuiPanel::OnNewDataset, NULL, this);
  CGuiPanel::EvtRenameDatasetCommand(*this, 
                                 (CRenameDatasetEventFunction)&CGuiPanel::OnRenameDataset, NULL, this);
  CGuiPanel::EvtDeleteDatasetCommand(*this, 
                                 (CDeleteDatasetEventFunction)&CGuiPanel::OnDeleteDataset, NULL, this);
  CGuiPanel::EvtNewOperationCommand(*this, 
                                 (CNewOperationEventFunction)&CGuiPanel::OnNewOperation, NULL, this);
  CGuiPanel::EvtRenameOperationCommand(*this, 
                                 (CRenameOperationEventFunction)&CGuiPanel::OnRenameOperation, NULL, this);
  CGuiPanel::EvtDeleteOperationCommand(*this, 
                                 (CDeleteOperationEventFunction)&CGuiPanel::OnDeleteOperation, NULL, this);

  CGuiPanel::EvtDatasetFilesChangedCommand(*this, 
                                 (CDatasetFilesChangeEventFunction)&CGuiPanel::OnDatasetFilesChanged, NULL, this);
}

//----------------------------------------
CGuiPanel::~CGuiPanel()
{
}

//----------------------------------------
void CGuiPanel::EvtNewDatasetCommand(wxWindow& window, const CNewDatasetEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_NEW_DATASET,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}
//----------------------------------------
void CGuiPanel::EvtRenameDatasetCommand(wxWindow& window, const CRenameDatasetEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_RENAME_DATASET,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}
//----------------------------------------
void CGuiPanel::EvtDeleteDatasetCommand(wxWindow& window, const CDeleteDatasetEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_DELETE_DATASET,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}

//----------------------------------------
void CGuiPanel::EvtNewDataExprCommand(wxWindow& window, const CNewDataExprEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_NEW_DATAEXPR,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}
//----------------------------------------
void CGuiPanel::EvtDeleteDataExprCommand(wxWindow& window, const CDeleteDataExprEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_DELETE_DATAEXPR,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}
//----------------------------------------
void CGuiPanel::EvtDatasetFilesChangedCommand(wxWindow& window, const CDatasetFilesChangeEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_DATASET_FILES_CHANGED,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}

//----------------------------------------
void CGuiPanel::EvtNewOperationCommand(wxWindow& window, const CNewOperationEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_NEW_OPERATION,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}
//----------------------------------------
void CGuiPanel::EvtRenameOperationCommand(wxWindow& window, const CRenameOperationEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_RENAME_OPERATION,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}
//----------------------------------------
void CGuiPanel::EvtDeleteOperationCommand(wxWindow& window, const CDeleteOperationEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_DELETE_OPERATION,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}
//----------------------------------------
void CGuiPanel::EvtNotebookPageChanged(wxWindow& window, const wxNotebookEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}
//----------------------------------------
void CGuiPanel::EvtNotebookPageChanging(wxWindow& window, const wxNotebookEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}

//----------------------------------------
void CGuiPanel::OnNewDataset(CNewDatasetEvent& event)
{
  //wxPostEvent(GetOperationPanel(), event);
  wxPostEvent(GetOperationPanel(), event);
}

//----------------------------------------
void CGuiPanel::OnRenameDataset(CRenameDatasetEvent& event)
{
  //wxPostEvent(GetOperationPanel(), event);
  wxPostEvent(GetOperationPanel(), event);
}
//----------------------------------------
void CGuiPanel::OnDeleteDataset(CDeleteDatasetEvent& event)
{
  //wxPostEvent(GetOperationPanel(), event);
  wxPostEvent(GetOperationPanel(), event);
}
//----------------------------------------
void CGuiPanel::OnNewOperation(CNewOperationEvent& event)
{
  //wxPostEvent(GetDisplayPanel(), event);
  wxPostEvent(GetDisplayPanel(), event);
}
//----------------------------------------
void CGuiPanel::OnDatasetFilesChanged(CDatasetFilesChangeEvent& event)
{
  //wxPostEvent(GetOperationPanel(), event);
  wxPostEvent(GetOperationPanel(), event);
}

//----------------------------------------
void CGuiPanel::OnRenameOperation(CRenameOperationEvent& event)
{
  //wxPostEvent(GetDisplayPanel(), event);
  wxPostEvent(GetDisplayPanel(), event);
}
//----------------------------------------
void CGuiPanel::OnDeleteOperation(CDeleteOperationEvent& event)
{
  //wxPostEvent(GetDisplayPanel(), event);
  wxPostEvent(GetDisplayPanel(), event);
}

// WDR: handler implementations for CGuiPanel

//----------------------------------------
void CGuiPanel::OnPageChanging(wxNotebookEvent& event)
{
  /*
  int32_t idx = event.GetOldSelection();
  wxBookCtrlBase *book = (wxBookCtrlBase *)event.GetEventObject();
  if ( idx != wxNOT_FOUND && book && book->GetPageText(idx) == DATASETS_PAGE_NAME )
  {
    CWorkspaceDataset* wks = wxGetApp().GetCurrentWorkspaceDataset();
    if (wks != NULL)
    {
      if (wks->CheckFiles() == false)
      {
        event.Veto();
        return;
      }
    }

  }
*/
  event.Skip();
}
//----------------------------------------
void CGuiPanel::OnPageChanged(wxNotebookEvent& event)
{
  //int32_t idxOld = event.GetOldSelection();
  int32_t idx = event.GetSelection();
  wxBookCtrlBase *book = (wxBookCtrlBase *)event.GetEventObject();
  if ( (idx != wxNOT_FOUND) && book)
  {
   if (book->GetPageText(idx) != LOG_PAGE_NAME)
   {
    wxGetApp().SetLastPageReached(book->GetPageText(idx));
   }
  }
  event.Skip();
}

//----------------------------------------


