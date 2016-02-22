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


#ifndef __DatasetPanel_H__
#define __DatasetPanel_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "DatasetPanel.h"
#endif


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

//#include "wx/vlbox.h"

#include "brathl.h"

#include "BratDisplay_wdr.h"

#include "new-gui/brat/DataModels/PlotData/XYPlotData.h"

//-------------------------------------------------------------
//------------------- CDatasetSelectedEvent class --------------------
//-------------------------------------------------------------
BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_DATASET_SELECTED, 7777) // 7777 is ignored, just for compatibility with v2.2
END_DECLARE_EVENT_TYPES()

class CDatasetSelectedEvent : public wxCommandEvent
{
public:
  CDatasetSelectedEvent(wxWindowID id, int32_t index, bool selected)
    : wxCommandEvent(wxEVT_DATASET_SELECTED, id)
  {
    m_selected = selected;
    m_index = index;
    m_id = id;
  };

  CDatasetSelectedEvent(const CDatasetSelectedEvent& event)
    : wxCommandEvent(wxEVT_DATASET_SELECTED, event.m_id)
  {
    m_selected = event.m_selected;
    m_index = event.m_index;
    m_id = event.m_id;
  };
  
  virtual wxEvent *Clone() const 
    {
      return new CDatasetSelectedEvent(*this); 
    };

  wxWindowID m_id;
  bool m_selected;
  int32_t m_index;
};
typedef void (wxEvtHandler::*CDatasetSelectedEventFunction)(CDatasetSelectedEvent&);

//-------------------------------------------------------------
//------------------- CDatasetAttrChangedEvent class --------------------
//-------------------------------------------------------------
BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_DATASET_ATTR_CHANGED, 7777) // 7777 is ignored, just for compatibility with v2.2
END_DECLARE_EVENT_TYPES()

class CDatasetAttrChangedEvent : public wxCommandEvent
{
public:
  CDatasetAttrChangedEvent(wxWindowID id)
    : wxCommandEvent(wxEVT_DATASET_ATTR_CHANGED, id)
  {
    m_id = id;
  };

  CDatasetAttrChangedEvent(const CDatasetAttrChangedEvent& event)
    : wxCommandEvent(wxEVT_DATASET_ATTR_CHANGED, event.m_id)
  {
    m_id = event.m_id;
  };
  
  virtual wxEvent *Clone() const 
    {
      return new CDatasetAttrChangedEvent(*this); 
    };

  wxWindowID m_id;
};
typedef void (wxEvtHandler::*CDatasetAttrChangedEventFunction)(CDatasetAttrChangedEvent&);


// WDR: class declarations

//-------------------------------------------------------------
//------------------- CAttributeListPanel class --------------------
//-------------------------------------------------------------

class CAttributeListPanel: public wxPanel
{
public:
    // constructors and destructors
    CAttributeListPanel();
    
    CAttributeListPanel( wxWindow *parent, CXYPlotData* plotData, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER );

    virtual ~CAttributeListPanel();

    bool Create( wxWindow *parent, CXYPlotData* plotData, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER );

    
    // WDR: method declarations for CAttributeListPanel
    
private:
  CXYPlotData* m_plotData;
    // WDR: member variable declarations for CAttributeListPanel
  
  wxStaticText* m_label;
  wxButton* m_colorBtn;
  
  wxStaticBox* m_box1;
  wxStaticBox* m_box2;
  wxStaticBox* m_box3;

  wxBoxSizer* m_hSizer;
  wxBoxSizer* m_hSizer1;
  wxBoxSizer* m_hSizer2;
  wxBoxSizer* m_hSizer3;
  wxBoxSizer* m_hSizer4;
  wxBoxSizer* m_hSizer5;
  wxBoxSizer* m_sizer;
  wxBoxSizer* m_mainSizer;

  
  wxStaticBoxSizer * m_sizer1;
  wxStaticBoxSizer * m_sizer2;
  wxStaticBoxSizer * m_sizer3;

  wxCheckBox* m_hidePlot;

  wxCheckBox* m_drawLines;
  wxCheckBox* m_drawPoints;

  wxStaticText* m_stipplePatternLabel;
  wxChoice* m_stipplePatternChoice;

  CLabeledTextCtrl* m_opacityCtrl;
  CLabeledTextCtrl* m_lineWidthCtrl;
  CLabeledTextCtrl* m_pointSizeCtrl;
  wxCheckBox* m_filledPoints;

  wxStaticText* m_glyphLabel;
  wxChoice* m_glyphChoice;

  long m_ID_LINEWIDTH;
  long m_ID_POINTSIZE;
  long m_ID_OPACITY;

private:
  void Init();

  bool CreateControls();
  void CreateLayout();
  void InstallEventListeners();
  void InstallToolTips();
  
  void Update();
    // WDR: handler declarations for CAttributeListPanel

  void OnHide(wxCommandEvent& event);
  void OnValueChange(CValueChangedEvent& event);
  void OnUpdate(wxCommandEvent& event);
  void OnColor(wxCommandEvent& event);


private:
    DECLARE_CLASS(CAttributeListPanel)
    DECLARE_EVENT_TABLE()
};

//-------------------------------------------------------------
//------------------- CDatasetListPanel class --------------------
//-------------------------------------------------------------

class CDatasetListPanel: public wxPanel
{
public:
    // constructors and destructors
  CDatasetListPanel();
  CDatasetListPanel( wxWindow *parent, CXYPlotDataCollection* plotDataCollection,  wxWindowID id = -1,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxTAB_TRAVERSAL | wxNO_BORDER );

  virtual ~CDatasetListPanel();

  bool Create ( wxWindow *parent, CXYPlotDataCollection* plotDataCollection,  wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL | wxNO_BORDER );

  void UpdateFieldValues();

  void Reset();

    
  // WDR: method declarations for CDatasetListPanel
    
private:
  CXYPlotDataCollection* m_plotDataCollection;
  int32_t m_currentDatasetIndex;
  bool m_datasetSelected;

  long m_ID_DATASETLIST;

  wxListBox* m_datasetListBox;
  wxButton* m_deselectButton;
  wxBoxSizer* m_sizer;

  CAttributeListPanel* m_attributeListPanel;

  // WDR: member variable declarations for CDatasetListPanel
    
private:
  void Init();
  bool CreateControls();
  void CreateBoxSizers();
  void CreateLayout();
  void InstallEventListeners();

  void SelectDataset(int32_t index, bool select);

  // WDR: handler declarations for CDatasetListPanel
  void OnDatasetSelect(wxCommandEvent& event);
  void OnDatasetDeselect(wxUpdateUIEventFunction& event);
  void OnDeselect(wxCommandEvent& event);

  void OnDatasetAttrChanged(CDatasetAttrChangedEvent& event);

private:
    DECLARE_CLASS(CDatasetListPanel)
    DECLARE_EVENT_TABLE()
};

//-------------------------------------------------------------
//------------------- CDatasetPanel class --------------------
//-------------------------------------------------------------

class CDatasetPanel: public wxPanel
{
public:
  // constructors and destructors
  CDatasetPanel();
  CDatasetPanel( wxWindow *parent, CXYPlotDataCollection* plotDataCollection,  wxWindowID id = -1,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxTAB_TRAVERSAL|wxSUNKEN_BORDER );
  
  virtual ~CDatasetPanel();

  bool Create ( wxWindow *parent, CXYPlotDataCollection* plotDataCollection,  wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL|wxSUNKEN_BORDER );

  void OnFrameChange(wxEvent& event);
  void Reset();
  static void EvtDatasetSelected(wxWindow& window, const CDatasetSelectedEventFunction& method,
                                 wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
  
  static void EvtDatasetAttrChanged(wxWindow& window, const CDatasetAttrChangedEventFunction& method,
                                    wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
  // WDR: method declarations for CDatasetPanel
    
private:
  CXYPlotDataCollection* m_plotDataCollection;
  
  CDatasetListPanel* m_datasetListPanel;
  wxBoxSizer* m_sizer;

  long m_ID_DATASETLISTPANEL;

  // WDR: member variable declarations for CDatasetPanel
    
private:
  void Init();

  bool CreateControls();
  void CreateBoxSizers();
  void CreateLayout();
  void InstallEventListeners();


  // WDR: handler declarations for CDatasetPanel

private:
    DECLARE_CLASS(CDatasetPanel)
    DECLARE_EVENT_TABLE()
};




#endif
