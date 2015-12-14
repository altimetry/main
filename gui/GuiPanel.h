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

#ifndef __GuiPanel_H__
#define __GuiPanel_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "BratGui_wdr.h"
class COperationPanel;
class CDisplayPanel;

#include "OperationPanel.h"
#include "DatasetPanel.h"
#include "DisplayPanel.h"
#include "LogPanel.h"

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_NEW_DATASET, 7777) // 7777 is ignored, just for compatibility with v2.2
    DECLARE_EVENT_TYPE(wxEVT_RENAME_DATASET, 7777) // 7777 is ignored, just for compatibility with v2.2
    DECLARE_EVENT_TYPE(wxEVT_DELETE_DATASET, 7777) // 7777 is ignored, just for compatibility with v2.2
    DECLARE_EVENT_TYPE(wxEVT_NEW_OPERATION, 7777) // 7777 is ignored, just for compatibility with v2.2
    DECLARE_EVENT_TYPE(wxEVT_RENAME_OPERATION, 7777) // 7777 is ignored, just for compatibility with v2.2
    DECLARE_EVENT_TYPE(wxEVT_DELETE_OPERATION, 7777) // 7777 is ignored, just for compatibility with v2.2
    DECLARE_EVENT_TYPE(wxEVT_NEW_DATAEXPR, 7777) // 7777 is ignored, just for compatibility with v2.2
    DECLARE_EVENT_TYPE(wxEVT_DELETE_DATAEXPR, 7777) // 7777 is ignored, just for compatibility with v2.2
    DECLARE_EVENT_TYPE(wxEVT_DATASET_FILES_CHANGED, 7777) // 7777 is ignored, just for compatibility with v2.2
END_DECLARE_EVENT_TYPES()


//-------------------------------------------------------------
//------------------- CDeleteDataExprEvent class --------------------
//-------------------------------------------------------------

class CDeleteDataExprEvent : public wxCommandEvent
{
public:
  CDeleteDataExprEvent(wxWindowID id, const wxString formulaName = "" )
    : wxCommandEvent(wxEVT_DELETE_DATAEXPR, id)
  {
    m_formulaName = formulaName;
    m_id = id;
  };

  CDeleteDataExprEvent(const CDeleteDataExprEvent& event)
    : wxCommandEvent(wxEVT_DELETE_DATAEXPR, event.m_id)
  {
    m_formulaName = event.m_formulaName;
    m_id = event.m_id;
  };
  
  virtual wxEvent *Clone() const 
    {
      return new CDeleteDataExprEvent(*this); 
    };

  wxWindowID m_id;
  wxString m_formulaName;

};
typedef void (wxEvtHandler::*CDeleteDataExprEventFunction)(CDeleteDataExprEvent&);

//-------------------------------------------------------------
//------------------- CNewDataExprEvent class --------------------
//-------------------------------------------------------------

class CNewDataExprEvent : public wxCommandEvent
{
public:
  CNewDataExprEvent(wxWindowID id, const wxString formulaName )
    : wxCommandEvent(wxEVT_NEW_DATAEXPR, id)
  {
    m_formulaName = formulaName;
    m_id = id;
  };

  CNewDataExprEvent(const CNewDataExprEvent& event)
    : wxCommandEvent(wxEVT_NEW_DATAEXPR, event.m_id)
  {
    m_formulaName = event.m_formulaName;
    m_id = event.m_id;
  };
  
  virtual wxEvent *Clone() const 
    {
      return new CNewDataExprEvent(*this); 
    };

  wxWindowID m_id;
  wxString m_formulaName;

};
typedef void (wxEvtHandler::*CNewDataExprEventFunction)(CNewDataExprEvent&);


//-------------------------------------------------------------
//------------------- CDeleteOperationEvent class --------------------
//-------------------------------------------------------------

class CDeleteOperationEvent : public wxCommandEvent
{
public:
  CDeleteOperationEvent(wxWindowID id, const wxString operationName )
    : wxCommandEvent(wxEVT_DELETE_OPERATION, id)
  {
    m_operationName = operationName;
    m_id = id;
  };

  CDeleteOperationEvent(const CDeleteOperationEvent& event)
    : wxCommandEvent(wxEVT_DELETE_OPERATION, event.m_id)
  {
    m_operationName = event.m_operationName;
    m_id = event.m_id;
  };
  
  virtual wxEvent *Clone() const 
    {
      return new CDeleteOperationEvent(*this); 
    };

  wxWindowID m_id;
  wxString m_operationName;

};
typedef void (wxEvtHandler::*CDeleteOperationEventFunction)(CDeleteOperationEvent&);

//-------------------------------------------------------------
//------------------- CNewOperationEvent class --------------------
//-------------------------------------------------------------

class CNewOperationEvent : public wxCommandEvent
{
public:
  CNewOperationEvent(wxWindowID id, const wxString operationName )
    : wxCommandEvent(wxEVT_NEW_OPERATION, id)
  {
    m_operationName = operationName;
    m_id = id;
  };

  CNewOperationEvent(const CNewOperationEvent& event)
    : wxCommandEvent(wxEVT_NEW_OPERATION, event.m_id)
  {
    m_operationName = event.m_operationName;
    m_id = event.m_id;
  };
  
  virtual wxEvent *Clone() const 
    {
      return new CNewOperationEvent(*this); 
    };

  wxWindowID m_id;
  wxString m_operationName;

};
typedef void (wxEvtHandler::*CNewOperationEventFunction)(CNewOperationEvent&);

//-------------------------------------------------------------
//------------------- CRenameOperationEvent class --------------------
//-------------------------------------------------------------

class CRenameOperationEvent : public wxCommandEvent
{
public:
  CRenameOperationEvent(wxWindowID id, const wxString operationOldName, const wxString operationNewName )
    : wxCommandEvent(wxEVT_RENAME_OPERATION, id)
  {
    m_operationOldName = operationOldName;
    m_operationNewName = operationNewName;
    m_id = id;
  };

  CRenameOperationEvent(const CRenameOperationEvent& event)
    : wxCommandEvent(wxEVT_RENAME_OPERATION, event.m_id)
  {
    m_operationOldName = event.m_operationOldName;
    m_operationNewName = event.m_operationNewName;
    m_id = event.m_id;
  };
  
  virtual wxEvent *Clone() const 
    {
      return new CRenameOperationEvent(*this); 
    };

  wxWindowID m_id;
  wxString m_operationOldName;
  wxString m_operationNewName;

};
typedef void (wxEvtHandler::*CRenameOperationEventFunction)(CRenameOperationEvent&);

//-------------------------------------------------------------
//------------------- CDatasetFilesChangeEvent class --------------------
//-------------------------------------------------------------

class CDatasetFilesChangeEvent : public wxCommandEvent
{
public:
  CDatasetFilesChangeEvent(wxWindowID id, const wxString datasetName, const CStringList& newFiles)
    : wxCommandEvent(wxEVT_DATASET_FILES_CHANGED, id)
  {
    m_datasetName = datasetName;
    m_files.Insert(newFiles);
    m_id = id;
  };

  CDatasetFilesChangeEvent(const CDatasetFilesChangeEvent& event)
    : wxCommandEvent(wxEVT_DATASET_FILES_CHANGED, event.m_id)
  {
    m_datasetName = event.m_datasetName;
    m_files.Insert(event.m_files);
    m_id = event.m_id;
  };
  
  virtual wxEvent *Clone() const 
    {
      return new CDatasetFilesChangeEvent(*this); 
    };

  wxWindowID m_id;
  wxString m_datasetName;
  CProductList m_files;

};
typedef void (wxEvtHandler::*CDatasetFilesChangeEventFunction)(CDatasetFilesChangeEvent&);


//-------------------------------------------------------------
//------------------- CDeleteDatasetEvent class --------------------
//-------------------------------------------------------------

class CDeleteDatasetEvent : public wxCommandEvent
{
public:
  CDeleteDatasetEvent(wxWindowID id, const wxString datasetName )
    : wxCommandEvent(wxEVT_DELETE_DATASET, id)
  {
    m_datasetName = datasetName;
    m_id = id;
  };

  CDeleteDatasetEvent(const CDeleteDatasetEvent& event)
    : wxCommandEvent(wxEVT_DELETE_DATASET, event.m_id)
  {
    m_datasetName = event.m_datasetName;
    m_id = event.m_id;
  };
  
  virtual wxEvent *Clone() const 
    {
      return new CDeleteDatasetEvent(*this); 
    };

  wxWindowID m_id;
  wxString m_datasetName;

};
typedef void (wxEvtHandler::*CDeleteDatasetEventFunction)(CDeleteDatasetEvent&);


//-------------------------------------------------------------
//------------------- CNewDatasetEvent class --------------------
//-------------------------------------------------------------

class CNewDatasetEvent : public wxCommandEvent
{
public:
  CNewDatasetEvent(wxWindowID id, const wxString datasetName )
    : wxCommandEvent(wxEVT_NEW_DATASET, id)
  {
    m_datasetName = datasetName;
    m_id = id;
  };

  CNewDatasetEvent(const CNewDatasetEvent& event)
    : wxCommandEvent(wxEVT_NEW_DATASET, event.m_id)
  {
    m_datasetName = event.m_datasetName;
    m_id = event.m_id;
  };
  
  virtual wxEvent *Clone() const 
    {
      return new CNewDatasetEvent(*this); 
    };

  wxWindowID m_id;
  wxString m_datasetName;

};
typedef void (wxEvtHandler::*CNewDatasetEventFunction)(CNewDatasetEvent&);

//-------------------------------------------------------------
//------------------- CRenameDatasetEvent class --------------------
//-------------------------------------------------------------

class CRenameDatasetEvent : public wxCommandEvent
{
public:
  CRenameDatasetEvent(wxWindowID id, const wxString datasetOldName, const wxString datasetNewName )
    : wxCommandEvent(wxEVT_RENAME_DATASET, id)
  {
    m_datasetOldName = datasetOldName;
    m_datasetNewName = datasetNewName;
    m_id = id;
  };

  CRenameDatasetEvent(const CRenameDatasetEvent& event)
    : wxCommandEvent(wxEVT_RENAME_DATASET, event.m_id)
  {
    m_datasetOldName = event.m_datasetOldName;
    m_datasetNewName = event.m_datasetNewName;
    m_id = event.m_id;
  };
  
  virtual wxEvent *Clone() const 
    {
      return new CRenameDatasetEvent(*this); 
    };

  wxWindowID m_id;
  wxString m_datasetOldName;
  wxString m_datasetNewName;

};
typedef void (wxEvtHandler::*CRenameDatasetEventFunction)(CRenameDatasetEvent&);


// WDR: class declarations

//----------------------------------------------------------------------------
// CGuiPanel
//----------------------------------------------------------------------------

class CGuiPanel: public wxPanel
{
public:
  // constructors and destructors
  CGuiPanel( wxWindow *parent, wxWindowID id = -1,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxTAB_TRAVERSAL | wxNO_BORDER );

  virtual ~CGuiPanel();
  
  // WDR: method declarations for CGuiPanel
  CLogPanel* GetLogPanel()  { return (CLogPanel*) FindWindow( ID_LOGPANEL ); }
  //CDisplayPanel* GetDisplayPanel()  { return (CDisplayPanel*) FindWindow( ID_DISPLAYPANEL ); }
  CDisplayPanel* GetDisplayPanel()  { return (CDisplayPanel*) FindWindow( ID_DISPLAYPANEL ); }
  //COperationPanel* GetOperationPanel()  { return (COperationPanel*) FindWindow( ID_OPERATIONPANEL ); }
  COperationPanel* GetOperationPanel()  { return (COperationPanel*) FindWindow( ID_OPERATIONSPLITTER ); }
  CDatasetPanel* GetDatasetPanel()  { return (CDatasetPanel*) FindWindow( ID_DATASETSPLITTER ); }
  wxNotebook* GetMainnotebook()  { return (wxNotebook*) FindWindow( ID_MAINNOTEBOOK ); }

  
  static void EvtDatasetFilesChangedCommand(wxWindow& window, const CDatasetFilesChangeEventFunction& method,
                        wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
  static void EvtNewDataExprCommand(wxWindow& window, const CNewDataExprEventFunction& method,
                        wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
  static void EvtDeleteDataExprCommand(wxWindow& window, const CDeleteDataExprEventFunction& method,
                        wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
  static void EvtNewOperationCommand(wxWindow& window, const CNewOperationEventFunction& method,
                        wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
  static void EvtRenameOperationCommand(wxWindow& window, const CRenameOperationEventFunction& method,
                        wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
  static void EvtDeleteOperationCommand(wxWindow& window, const CDeleteOperationEventFunction& method,
                        wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
  static void EvtNewDatasetCommand(wxWindow& window, const CNewDatasetEventFunction& method,
                        wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
  static void EvtRenameDatasetCommand(wxWindow& window, const CRenameDatasetEventFunction& method,
                        wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
  static void EvtDeleteDatasetCommand(wxWindow& window, const CDeleteDatasetEventFunction& method,
                        wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
  static void EvtNotebookPageChanged(wxWindow& window, const wxNotebookEventFunction& method,
                        wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
  static void EvtNotebookPageChanging(wxWindow& window, const wxNotebookEventFunction& method,
                        wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);

private:
  // WDR: member variable declarations for CGuiPanel
    
private:
  void OnDatasetFilesChanged(CDatasetFilesChangeEvent& event);
  void OnNewDataset(CNewDatasetEvent& event);
  void OnRenameDataset(CRenameDatasetEvent& event);
  void OnDeleteDataset(CDeleteDatasetEvent& event);
  void OnNewOperation(CNewOperationEvent& event);
  void OnRenameOperation(CRenameOperationEvent& event);
  void OnDeleteOperation(CDeleteOperationEvent& event);



  // WDR: handler declarations for CGuiPanel
  void OnPageChanging(wxNotebookEvent& event);
  void OnPageChanged(wxNotebookEvent& event);

private:
    DECLARE_CLASS(CGuiPanel)
    DECLARE_EVENT_TABLE()
};




#endif
