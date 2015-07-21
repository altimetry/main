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
#ifndef __WPlotPropertyPanel_H__
#define __WPlotPropertyPanel_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA) && !defined(__clang__)
    #pragma interface "WPlotPropertyPanel.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "brathl.h"

#include "wx/stattext.h"
#include "wx/choice.h"
#include "wx/dynarray.h"

#include "wxVTKRenderWindowInteractor.h"

#include "vtkActor2D.h"

#include "BratDisplay_wdr.h"

#include "WorldPlotData.h"
#include "MapProjection.h"
#include "LabeledTextCtrl.h"
#include "LUTPanel.h"


BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_PROJECTION_CHANGED, 7777) // 7777 is ignored, just for compatibility with v2.2
    DECLARE_EVENT_TYPE(wxEVT_CENTER_POINT_CHANGED, 7777) // 7777 is ignored, just for compatibility with v2.2
    DECLARE_EVENT_TYPE(wxEVT_GRIDLABELS_CHANGED, 7777) // 7777 is ignored, just for compatibility with v2.2
    DECLARE_EVENT_TYPE(wxEVT_RANGE_CHANGED, 7777) // 7777 is ignored, just for compatibility with v2.2
    DECLARE_EVENT_TYPE(wxEVT_NUM_LABELS_CHANGED, 7777) // 7777 is ignored, just for compatibility with v2.2
    DECLARE_EVENT_TYPE(wxEVT_VIEW_STATE_CHANGED, 7777) // 7777 is ignored, just for compatibility with v2.2
    DECLARE_EVENT_TYPE(wxEVT_VIEW_STATE_COMMAND, 7777) // 7777 is ignored, just for compatibility with v2.2
    DECLARE_EVENT_TYPE(wxEVT_UPDATE_RANGE_COMMAND, 7777) // 7777 is ignored, just for compatibility with v2.2
    DECLARE_EVENT_TYPE(wxEVT_ZOOM_CHANGED, 7777) // 7777 is ignored, just for compatibility with v2.2
    DECLARE_EVENT_TYPE(wxEVT_FACTOR_CHANGED, 7777) // 7777 is ignored, just for compatibility with v2.2
    DECLARE_EVENT_TYPE(wxEVT_RADIUS_CHANGED, 7777) // 7777 is ignored, just for compatibility with v2.2
    DECLARE_EVENT_TYPE(wxEVT_DISPLAY_DATA_ATTR_CHANGED, 7777) // 7777 is ignored, just for compatibility with v2.2
    DECLARE_EVENT_TYPE(wxEVT_VECTOR_SCALE_CHANGED, 7777) // 7777 is ignored, just for compatibility with v2.2
END_DECLARE_EVENT_TYPES()
//-------------------------------------------------------------
//------------------- CUpdateRangeEvent class --------------------
//-------------------------------------------------------------

class CUpdateRangeEvent : public wxCommandEvent
{
public:
  CUpdateRangeEvent(wxWindowID id)
    : wxCommandEvent(wxEVT_UPDATE_RANGE_COMMAND, id)
  {
    m_id = id;
  };

  CUpdateRangeEvent(const CUpdateRangeEvent& event)
    : wxCommandEvent(wxEVT_UPDATE_RANGE_COMMAND, event.m_id)
  {
    m_id = event.m_id;
  };

  virtual wxEvent *Clone() const
    {
      return new CUpdateRangeEvent(*this);
    };

  wxWindowID m_id;
};
typedef void (wxEvtHandler::*CUpdateRangeEventFunction)(CUpdateRangeEvent&);




//-------------------------------------------------------------
//------------------- CNumlabelsChangedEvent class --------------------
//-------------------------------------------------------------

class CNumLabelsChangedEvent : public wxCommandEvent
{
public:
  CNumLabelsChangedEvent(wxWindowID id, int32_t nLabels)
    : wxCommandEvent(wxEVT_NUM_LABELS_CHANGED, id)
  {
    m_nLabels = nLabels;
    m_id = id;
  };

  CNumLabelsChangedEvent(const CNumLabelsChangedEvent& event)
    : wxCommandEvent(wxEVT_NUM_LABELS_CHANGED, event.m_id)
  {
    m_nLabels = event.m_nLabels;
    m_id = event.m_id;
  };

  virtual wxEvent *Clone() const
    {
      return new CNumLabelsChangedEvent(*this);
    };

  wxWindowID m_id;
  int32_t m_nLabels;
};
typedef void (wxEvtHandler::*CNumLabelsChangedEventFunction)(CNumLabelsChangedEvent&);


//-------------------------------------------------------------
//------------------- CVectorScaleChnagedEvent class --------------------
//-------------------------------------------------------------

class CVectorScaleChangedEvent : public wxCommandEvent
{
public:
  CVectorScaleChangedEvent(wxWindowID id, double vScale)
    : wxCommandEvent(wxEVT_VECTOR_SCALE_CHANGED, id)
  {
    m_vScale = vScale;
    m_id = id;
  };

  CVectorScaleChangedEvent(const CVectorScaleChangedEvent& event)
    : wxCommandEvent(wxEVT_VECTOR_SCALE_CHANGED, event.m_id)
  {
    m_vScale = event.m_vScale;
    m_id = event.m_id;
  };

  virtual wxEvent *Clone() const
    {
      return new CVectorScaleChangedEvent(*this);
    };

  wxWindowID m_id;
  double m_vScale;
};
typedef void (wxEvtHandler::*CVectorScaleChangedEventFunction)(CVectorScaleChangedEvent&);

//-------------------------------------------------------------
//------------------- CRangeChangedEvent class --------------------
//-------------------------------------------------------------

class CRangeChangedEvent : public wxCommandEvent
{
public:
  CRangeChangedEvent(wxWindowID id, double min, double max)
    : wxCommandEvent(wxEVT_RANGE_CHANGED, id)
  {
    m_min = min;
    m_max = max;
    m_id = id;
  };

  CRangeChangedEvent(const CRangeChangedEvent& event)
    : wxCommandEvent(wxEVT_RANGE_CHANGED, event.m_id)
  {
    m_min = event.m_min;
    m_max = event.m_max;
    m_id = event.m_id;
  };

  virtual wxEvent *Clone() const
    {
      return new CRangeChangedEvent(*this);
    };

  wxWindowID m_id;
  double m_min;
  double m_max;
};
typedef void (wxEvtHandler::*CRangeChangedEventFunction)(CRangeChangedEvent&);


//-------------------------------------------------------------
//------------------- CCenterPointChangedEvent class --------------------
//-------------------------------------------------------------

class CCenterPointChangedEvent : public wxCommandEvent
{
public:
  CCenterPointChangedEvent(wxWindowID id, double lat, double lon)
    : wxCommandEvent(wxEVT_CENTER_POINT_CHANGED, id)
  {
    m_lat = lat;
    m_lon = lon;
    m_id = id;
  };

  CCenterPointChangedEvent(const CCenterPointChangedEvent& event)
    : wxCommandEvent(wxEVT_CENTER_POINT_CHANGED, event.m_id)
  {
    m_lat = event.m_lat;
    m_lon = event.m_lon;
    m_id = event.m_id;
  };

  virtual wxEvent *Clone() const
    {
      return new CCenterPointChangedEvent(*this);
    };

  wxWindowID m_id;
  double m_lat;
  double m_lon;
};
typedef void (wxEvtHandler::*CCenterPointChangedEventFunction)(CCenterPointChangedEvent&);

//-------------------------------------------------------------
//--------------- CGranFactorChangedEvent class ---------------
//-------------------------------------------------------------

class CGridLabelsChangedEvent : public wxCommandEvent
{
public:
  CGridLabelsChangedEvent(wxWindowID id, double gran)
    : wxCommandEvent(wxEVT_GRIDLABELS_CHANGED, id)
  {
    m_gran = gran;
  };

  CGridLabelsChangedEvent(const CGridLabelsChangedEvent& event)
    : wxCommandEvent(wxEVT_GRIDLABELS_CHANGED, event.m_id)
  {
    m_gran = event.m_gran;
  };

  virtual wxEvent *Clone() const
    {
      return new CGridLabelsChangedEvent(*this);
    };

  wxWindowID m_id;
  double m_gran;
};
typedef void (wxEvtHandler::*CGridLabelsEventFunction)(CGridLabelsChangedEvent&);

//-------------------------------------------------------------
//------------------- CProjectionChangedEvent class --------------------
//-------------------------------------------------------------

class CProjectionChangedEvent : public wxCommandEvent
{
public:
  CProjectionChangedEvent(wxWindowID id, int32_t projection)
    : wxCommandEvent(wxEVT_PROJECTION_CHANGED, id)
  {
    m_projection = projection;
    m_id = id;
  };

  CProjectionChangedEvent(const CProjectionChangedEvent& event)
    : wxCommandEvent(wxEVT_PROJECTION_CHANGED, event.m_id)
  {
    m_projection = event.m_projection;
    m_id = event.m_id;
  };

  virtual wxEvent *Clone() const
    {
      return new CProjectionChangedEvent(*this);
    }

  wxWindowID m_id;
  int32_t m_projection;
};
typedef void (wxEvtHandler::*CProjectionChangedEventFunction)(CProjectionChangedEvent&);

//-------------------------------------------------------------
//------------------- CRadiusChangedEvent class --------------------
//-------------------------------------------------------------

class CRadiusChangedEvent : public wxCommandEvent
{
public:
  CRadiusChangedEvent(wxWindowID id, int32_t index)
    : wxCommandEvent(wxEVT_RADIUS_CHANGED, id)
  {
    m_index = index;
    m_id = id;
  };

  CRadiusChangedEvent(const CRadiusChangedEvent& event)
    : wxCommandEvent(wxEVT_RADIUS_CHANGED, event.m_id)
  {
    m_index = event.m_index;
    m_id = event.m_id;
  };

  virtual wxEvent *Clone() const
    {
      return new CRadiusChangedEvent(*this);
    };

  wxWindowID m_id;
  int32_t m_index;
};
typedef void (wxEvtHandler::*CRadiusChangedEventFunction)(CRadiusChangedEvent&);

//-------------------------------------------------------------
//------------------- CFactorChangedEvent class --------------------
//-------------------------------------------------------------

class CFactorChangedEvent : public wxCommandEvent
{
public:
  CFactorChangedEvent(wxWindowID id, int32_t index)
    : wxCommandEvent(wxEVT_FACTOR_CHANGED, id)
  {
    m_index = index;
    m_id = id;
  };

  CFactorChangedEvent(const CFactorChangedEvent& event)
    : wxCommandEvent(wxEVT_FACTOR_CHANGED, event.m_id)
  {
    m_index = event.m_index;
    m_id = event.m_id;
  };

  virtual wxEvent *Clone() const
    {
      return new CFactorChangedEvent(*this);
    };

  wxWindowID m_id;
  int32_t m_index;
};
typedef void (wxEvtHandler::*CFactorChangedEventFunction)(CFactorChangedEvent&);


//-------------------------------------------------------------
//------------------- CViewStateChangedEvent class --------------------
//-------------------------------------------------------------

class CViewStateChangedEvent : public wxCommandEvent
{
public:
  CViewStateChangedEvent(wxWindowID id, int32_t index)
    : wxCommandEvent(wxEVT_VIEW_STATE_CHANGED, id)
  {
    m_index = index;
    m_id = id;
  };

  CViewStateChangedEvent(const CViewStateChangedEvent& event)
    : wxCommandEvent(wxEVT_VIEW_STATE_CHANGED, event.m_id)
  {
    m_index = event.m_index;
    m_id = event.m_id;
  };

  virtual wxEvent *Clone() const
    {
      return new CViewStateChangedEvent(*this);
    };

  wxWindowID m_id;
  int32_t m_index;
};
typedef void (wxEvtHandler::*CViewStateChangedEventFunction)(CViewStateChangedEvent&);

//-------------------------------------------------------------
//------------------- CViewStateChangedEvent class --------------------
//-------------------------------------------------------------

class CViewStateCommandEvent : public wxCommandEvent
{
public:

  enum actionFlags
  {
    viewSave,
    viewClear
  };

  CViewStateCommandEvent(wxWindowID id, actionFlags action)
    : wxCommandEvent(wxEVT_VIEW_STATE_COMMAND, id)
  {
    m_action = action;
    m_id = id;
  };

  CViewStateCommandEvent(const CViewStateCommandEvent& event)
    : wxCommandEvent(wxEVT_VIEW_STATE_COMMAND, event.m_id)
  {
    m_action = event.m_action;
    m_id = event.m_id;
  };

  virtual wxEvent *Clone() const
    {
      return new CViewStateCommandEvent(*this);
    };

  wxWindowID m_id;
  actionFlags m_action;
};
typedef void (wxEvtHandler::*CViewStateCommandEventFunction)(CViewStateCommandEvent&);

//-------------------------------------------------------------
//------------------- CZoomChangedEvent class --------------------
//-------------------------------------------------------------

class CZoomChangedEvent : public wxCommandEvent
{
public:
  CZoomChangedEvent(wxWindowID id, double lat1, double lon1, double lat2, double lon2)
    : wxCommandEvent(wxEVT_ZOOM_CHANGED, id)
  {
    m_lon1 = lon1;
    m_lon2 = lon2;
    m_lat1 = lat1;
    m_lat2 = lat2;
    m_id = id;
  };

  CZoomChangedEvent(const CZoomChangedEvent& event)
    : wxCommandEvent(wxEVT_ZOOM_CHANGED, event.m_id)
  {
    m_lon1 = event.m_lon1;
    m_lon2 = event.m_lon2;
    m_lat1 = event.m_lat1;
    m_lat2 = event.m_lat2;
    m_id = event.m_id;
  };

  virtual wxEvent *Clone() const
    {
      return new CZoomChangedEvent(*this);
    };

  wxWindowID m_id;
  double m_lon1;
  double m_lon2;
  double m_lat1;
  double m_lat2;

};
typedef void (wxEvtHandler::*CZoomChangedEventFunction)(CZoomChangedEvent&);

//-------------------------------------------------------------
//------------------- CDisplayDataAttrChangedEvent class --------------------
//-------------------------------------------------------------

class CDisplayDataAttrChangedEvent : public wxCommandEvent
{
public:

  enum displayAttrFlags
  {
    displayContour,
    displaySolidColor,
    displayGridLabels
  };

  CDisplayDataAttrChangedEvent(wxWindowID id, displayAttrFlags action, bool value)
    : wxCommandEvent(wxEVT_DISPLAY_DATA_ATTR_CHANGED, id)
  {
    m_action = action;
    m_value = value;
    m_id = id;
  };

  CDisplayDataAttrChangedEvent(const CDisplayDataAttrChangedEvent& event)
    : wxCommandEvent(wxEVT_DISPLAY_DATA_ATTR_CHANGED, event.m_id)
  {
    m_action = event.m_action;
    m_value = event.m_value;
    m_id = event.m_id;
  };

  virtual wxEvent *Clone() const
    {
      return new CDisplayDataAttrChangedEvent(*this);
    };

  wxWindowID m_id;
  displayAttrFlags m_action;
  bool m_value;

};
typedef void (wxEvtHandler::*CDisplayDataAttrChangedEventFunction)(CDisplayDataAttrChangedEvent&);
// WDR: class declarations

//-------------------------------------------------------------
//------------------- CWPlotPropertyPanel class --------------------
//-------------------------------------------------------------
extern long ID_WPLOTPROP_PROJECTION;
extern long ID_WPLOTPROP_LAT;
extern long ID_WPLOTPROP_LON;
extern long ID_WPLOTPROP_GRAN;
extern long ID_WPLOTPROP_NUMLABELS;
extern long ID_WPLOTPROP_RANGE_MIN;
extern long ID_WPLOTPROP_RANGE_MAX;


//WX_DECLARE_OBJARRAY(wxBoxSizer, wxArrayBoxSizer);
// WX_DECLARE_OBJARRAY(wxStaticBoxSizer, wxArrayStaticBoxSizer); //don't compile because of DECLARE_NO_COPY_CLASS in class wxStaticBoxSizer



class CWPlotPropertyPanel: public wxScrolledWindow
{
public:
    // constructors and destructors
    CWPlotPropertyPanel();

    CWPlotPropertyPanel( wxWindow *parent, vtkActor2D* plotter, CWorldPlotData* plotData, wxVTKRenderWindowInteractor* vtkWidget,
        wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxScrolledWindowStyle );

    virtual ~CWPlotPropertyPanel();

    bool Create( wxWindow *parent, vtkActor2D* plotter, CWorldPlotData* plotData, wxVTKRenderWindowInteractor* vtkWidget,
        wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxScrolledWindowStyle );

    void GetRange(double& min, double& max);
    double GetRangeMin();
    double GetRangeMax();

    wxChoice* GetLayerChoice() {return &m_layerChoice;}

    CGeoMap* GetCurrentLayer();

    void SetCurrentLayer(int32_t index);
    void SetCurrentLayer(CGeoMap* geoMap);
    void SetCenterPoint(double lat, double lon);

    void UpdateValues();

    void LutChanged( CLutChangedEvent &event );

    static void EvtCheckBox(wxWindow& window, const wxCommandEventFunction& method, wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
    static void EvtButtonClicked(wxWindow& window, const wxCommandEventFunction& method, wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);

    static void EvtDisplayDataAttrChanged(wxWindow& window, const CDisplayDataAttrChangedEventFunction& method, wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
    static void EvtProjectionChanged(wxWindow& window, const CProjectionChangedEventFunction& method, wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
    static void EvtCenterPointChanged(wxWindow& window, const CCenterPointChangedEventFunction& method, wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
    static void EvtGridLabelsChanged(wxWindow& window, const CGridLabelsEventFunction& method, wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
    static void EvtRangeChanged(wxWindow& window, const CRangeChangedEventFunction& method, wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
    static void EvtNumLabelsChanged(wxWindow& window, const CNumLabelsChangedEventFunction& method, wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
    static void EvtVScaleChanged(wxWindow& window, const CVectorScaleChangedEventFunction& method, wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
    //static void EvtGlyphChanged(wxWindow& window, const CGlyphChangedEventFunction& method, wxObject* userData = NULL, wxEvtHandler* eventSink = NULL); take out
    static void EvtChoice(wxWindow& window, const wxCommandEventFunction& method, wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
    static void EvtViewStateChanged(wxWindow& window, const CViewStateChangedEventFunction& method, wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
    static void EvtViewStateCommand(wxWindow& window, const CViewStateCommandEventFunction& method, wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
    static void EvtUpdateRangeCommand(wxWindow& window, const CUpdateRangeEventFunction& method, wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
    static void EvtZoomChangedCommand(wxWindow& window, const CZoomChangedEventFunction& method, wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
    static void EvtRadiusChangedCommand(wxWindow& window, const CRadiusChangedEventFunction& method, wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
    static void EvtFactorChangedCommand(wxWindow& window, const CFactorChangedEventFunction& method, wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);


    // WDR: method declarations for CWPlotPropertyPanel
    wxNotebook* GetViewNotebook()  { return (wxNotebook*) FindWindow( ID_VIEW_NOTEBOOK ); }
    wxSpinCtrl* GetFactorctrl()  { return (wxSpinCtrl*) FindWindow( ID_FACTORCTRL ); }
    wxSpinCtrl* GetRadiusctrl()  { return (wxSpinCtrl*) FindWindow( ID_RADIUSCTRL ); }
    CLabeledTextCtrl* GetZoomLat1()  { return (CLabeledTextCtrl*) FindWindow( ID_ZOOM_LAT1 ); }
    CLabeledTextCtrl* GetZoomLon2()  { return (CLabeledTextCtrl*) FindWindow( ID_ZOOM_LON2 ); }
    CLabeledTextCtrl* GetZoomLat2()  { return (CLabeledTextCtrl*) FindWindow( ID_ZOOM_LAT2 ); }
    CLabeledTextCtrl* GetZoomLon1()  { return (CLabeledTextCtrl*) FindWindow( ID_ZOOM_LON1 ); }
    wxButton* GetClearview()  { return (wxButton*) FindWindow( ID_CLEARVIEW ); }
    wxButton* GetSaveview()  { return (wxButton*) FindWindow( ID_SAVEVIEW ); }
    wxSpinCtrl* GetViewctrl()  { return (wxSpinCtrl*) FindWindow( ID_VIEWCTRL ); }

public:
    bool m_finished;

private:
    // WDR: member variable declarations for CWPlotPropertyPanel
  wxWindow* m_parent;


  wxVTKRenderWindowInteractor* m_vtkWidget;
  vtkActor2D* m_plotter;

  CWorldPlotData* m_plotData;
  //CMapProjection m_projections;

  wxStaticText m_projectionLabel;
  wxChoice m_projectionChoice;
  CLabeledTextCtrl m_centerLatCtrl;
  CLabeledTextCtrl m_centerLonCtrl;

  wxCheckBox m_gridLabelsCtrl;

  CLabeledTextCtrl m_numLabelsCtrl;
  wxStaticText m_VScaleLabel;
  CLabeledTextCtrl m_numVScaleCtrl;
  CLabeledTextCtrl m_minRangeCtrl;
  CLabeledTextCtrl m_maxRangeCtrl;

  wxStaticText m_layerLabel;
  wxChoice m_layerChoice;

  wxButton m_resetBtn;
  wxButton m_editColorTableBtn;
  wxButton m_editContourPropsBtn;

  wxCheckBox m_solidColorCtrl;
  wxCheckBox m_contourCtrl;


  //wxArrayBoxSizer m_hSizers;

  wxBoxSizer* m_hSizer;
  wxBoxSizer* m_hSizer2;
  wxBoxSizer* m_hSizer3;
  wxBoxSizer* m_hSizer4;
  wxBoxSizer* m_hSizer5;
  wxBoxSizer* m_hSizer6;

  wxStaticBoxSizer* m_vSizer;
  wxStaticBoxSizer* m_vSizer2;
  wxStaticBoxSizer* m_vSizer3;

  wxStaticBox* m_vStaticSizer;
  wxStaticBox* m_vStaticSizer2;
  wxStaticBox* m_vStaticSizer3;

  wxBoxSizer* m_sizer;
  wxBoxSizer* m_mainSizer;

  double m_lat;
  double m_lon;
  int32_t m_gran;
  double m_min;
  double m_max;
  int32_t m_nLabels;
  double m_vScale;

  int32_t m_projection;



private:

    void Init();

    void CreateBoxSizers();
//    void DeleteBoxSizers();

    bool CreateControls();
    void CreateLayout();
    void InstallToolTips();
    void InstallEventListeners();

    void FactorChanged();
    void RadiusChanged();
    void ViewChanged();

    void FullView();

    // WDR: handler declarations for CWPlotPropertyPanel
    void OnFactorCtrl( wxCommandEvent &event );
    void OnRadiusCtrl( wxCommandEvent &event );
    void OnSpinFactor( wxSpinEvent &event );
    void OnSpinRadius( wxSpinEvent &event );
    void OnFullView( wxCommandEvent &event );
    void OnZoom( wxCommandEvent &event );
    void OnReset(wxCommandEvent& event);
    void OnEditColorTable(wxCommandEvent& event);
    void OnEditContourProperties(wxCommandEvent& event);
    void OnClearView( wxCommandEvent &event );
    void OnSaveView( wxCommandEvent &event );
    void OnViewCtrl( wxCommandEvent &event );
    void OnSpinView( wxSpinEvent &event );

    void OnCenterLat(CValueChangedEvent& event);
    void OnCenterLon(CValueChangedEvent& event);

    void OnMinRange(CValueChangedEvent& event);
    void OnMaxRange(CValueChangedEvent& event);
    void OnNumLabels(CValueChangedEvent& event);
    void OnVScale(CValueChangedEvent& event);

    void ProjectionInit();
    void OnProjectionChoice(wxCommandEvent &event);

    void OnSolidColor(wxCommandEvent& event);
    void OnGridLabels(wxCommandEvent& event);
    void OnContour(wxCommandEvent& event);

    void OnLayerChoice(wxCommandEvent &event);

    void OnCenterPointChanged(CCenterPointChangedEvent& event);
    void OnGridLabelsChanged(wxCommandEvent& event);
    void OnProjectionChanged(CProjectionChangedEvent& event);
    void OnRangeChanged(CRangeChangedEvent& event);
    void OnNumLabelsChanged(CNumLabelsChangedEvent& event);


private:
    DECLARE_EVENT_TABLE()
};




#endif
