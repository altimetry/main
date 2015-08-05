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
#ifndef __ZFXYPlotPropertyPanel_H__
#define __ZFXYPlotPropertyPanel_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "ZFXYPlotPropertyPanel.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "brathl.h"

#include "wx/stattext.h"
#include "wx/choice.h"
#include "wx/dynarray.h"

#include "wxVTKRenderWindowInteractor.h"

#include "vtkZFXYPlotActor.h"
#include "vtkCommand.h"

#include "BratDisplay_wdr.h"

#include "ZFXYPlotData.h"
#include "MapProjection.h"
#include "LabeledTextCtrl.h"
#include "LUTPanel.h"
#include "ZFXYContourPropFrame.h"
#include "AnimationToolbar.h"

class CVtkZFXYComputeRangeCallback;



BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_ZFXY_PROJECTION_CHANGED, 7777) // 7777 is ignored, just for compatibility with v2.2
    DECLARE_EVENT_TYPE(wxEVT_ZFXY_CENTER_POINT_CHANGED, 7777) // 7777 is ignored, just for compatibility with v2.2
    DECLARE_EVENT_TYPE(wxEVT_ZFXY_RANGE_CHANGED, 7777) // 7777 is ignored, just for compatibility with v2.2
    DECLARE_EVENT_TYPE(wxEVT_ZFXY_NUM_LABELS_CHANGED, 7777) // 7777 is ignored, just for compatibility with v2.2
    DECLARE_EVENT_TYPE(wxEVT_ZFXY_VIEW_STATE_CHANGED, 7777) // 7777 is ignored, just for compatibility with v2.2
    DECLARE_EVENT_TYPE(wxEVT_ZFXY_VIEW_STATE_COMMAND, 7777) // 7777 is ignored, just for compatibility with v2.2
    DECLARE_EVENT_TYPE(wxEVT_ZFXY_UPDATE_RANGE_COMMAND, 7777) // 7777 is ignored, just for compatibility with v2.2
    DECLARE_EVENT_TYPE(wxEVT_ZFXY_ZOOM_CHANGED, 7777) // 7777 is ignored, just for compatibility with v2.2
    DECLARE_EVENT_TYPE(wxEVT_ZFXY_FACTOR_CHANGED, 7777) // 7777 is ignored, just for compatibility with v2.2
    DECLARE_EVENT_TYPE(wxEVT_ZFXY_RADIUS_CHANGED, 7777) // 7777 is ignored, just for compatibility with v2.2
    DECLARE_EVENT_TYPE(wxEVT_ZFXY_DISPLAY_DATA_ATTR_CHANGED, 7777) // 7777 is ignored, just for compatibility with v2.2
END_DECLARE_EVENT_TYPES()
//-------------------------------------------------------------
//------------------- CZFXYUpdateRangeEvent class --------------------
//-------------------------------------------------------------

class CZFXYUpdateRangeEvent : public wxCommandEvent
{
public:
  CZFXYUpdateRangeEvent(wxWindowID id)
    : wxCommandEvent(wxEVT_ZFXY_UPDATE_RANGE_COMMAND, id)
  {
    m_id = id;
  };

  CZFXYUpdateRangeEvent(const CZFXYUpdateRangeEvent& event)
    : wxCommandEvent(wxEVT_ZFXY_UPDATE_RANGE_COMMAND, event.m_id)
  {
    m_id = event.m_id;
  };
  
  virtual wxEvent *Clone() const 
    {
      return new CZFXYUpdateRangeEvent(*this); 
    };

  wxWindowID m_id;
};
typedef void (wxEvtHandler::*CZFXYUpdateRangeEventFunction)(CZFXYUpdateRangeEvent&);




//-------------------------------------------------------------
//------------------- CZFXYNumLabelsChangedEvent class --------------------
//-------------------------------------------------------------

class CZFXYNumLabelsChangedEvent : public wxCommandEvent
{
public:
  CZFXYNumLabelsChangedEvent(wxWindowID id, int32_t nLabels)
    : wxCommandEvent(wxEVT_ZFXY_NUM_LABELS_CHANGED, id)
  {
    m_nLabels = nLabels;
    m_id = id;
  };

  CZFXYNumLabelsChangedEvent(const CZFXYNumLabelsChangedEvent& event)
    : wxCommandEvent(wxEVT_ZFXY_NUM_LABELS_CHANGED, event.m_id)
  {
    m_nLabels = event.m_nLabels;
    m_id = event.m_id;
  };
  
  virtual wxEvent *Clone() const 
    {
      return new CZFXYNumLabelsChangedEvent(*this); 
    };

  wxWindowID m_id;
  int32_t m_nLabels;
};
typedef void (wxEvtHandler::*CZFXYNumLabelsChangedEventFunction)(CZFXYNumLabelsChangedEvent&);


//-------------------------------------------------------------
//------------------- CZFXYRangeChangedEvent class --------------------
//-------------------------------------------------------------

class CZFXYRangeChangedEvent : public wxCommandEvent
{
public:
  CZFXYRangeChangedEvent(wxWindowID id, double min, double max)
    : wxCommandEvent(wxEVT_ZFXY_RANGE_CHANGED, id)
  {
    m_min = min;
    m_max = max;
    m_id = id;
  };

  CZFXYRangeChangedEvent(const CZFXYRangeChangedEvent& event)
    : wxCommandEvent(wxEVT_ZFXY_RANGE_CHANGED, event.m_id)
  {
    m_min = event.m_min;
    m_max = event.m_max;
    m_id = event.m_id;
  };
  
  virtual wxEvent *Clone() const 
    {
      return new CZFXYRangeChangedEvent(*this); 
    };

  wxWindowID m_id;
  double m_min;
  double m_max;
};
typedef void (wxEvtHandler::*CZFXYRangeChangedEventFunction)(CZFXYRangeChangedEvent&);


//-------------------------------------------------------------
//------------------- CZFXYCenterPointChangedEvent class --------------------
//-------------------------------------------------------------

class CZFXYCenterPointChangedEvent : public wxCommandEvent
{
public:
  CZFXYCenterPointChangedEvent(wxWindowID id, double lat, double lon)
    : wxCommandEvent(wxEVT_ZFXY_CENTER_POINT_CHANGED, id)
  {
    m_lat = lat;
    m_lon = lon;
    m_id = id;
  };

  CZFXYCenterPointChangedEvent(const CZFXYCenterPointChangedEvent& event)
    : wxCommandEvent(wxEVT_ZFXY_CENTER_POINT_CHANGED, event.m_id)
  {
    m_lat = event.m_lat;
    m_lon = event.m_lon;
    m_id = event.m_id;
  };
  
  virtual wxEvent *Clone() const 
    {
      return new CZFXYCenterPointChangedEvent(*this); 
    };

  wxWindowID m_id;
  double m_lat;
  double m_lon;
};
typedef void (wxEvtHandler::*CZFXYCenterPointChangedEventFunction)(CZFXYCenterPointChangedEvent&);

//-------------------------------------------------------------
//------------------- CZFXYProjectionChangedEvent class --------------------
//-------------------------------------------------------------

class CZFXYProjectionChangedEvent : public wxCommandEvent
{
public:
  CZFXYProjectionChangedEvent(wxWindowID id, int32_t projection)
    : wxCommandEvent(wxEVT_ZFXY_PROJECTION_CHANGED, id)
  {
    m_projection = projection;
    m_id = id;
  };

  CZFXYProjectionChangedEvent(const CZFXYProjectionChangedEvent& event)
    : wxCommandEvent(wxEVT_ZFXY_PROJECTION_CHANGED, event.m_id)
  {
    m_projection = event.m_projection;
    m_id = event.m_id;
  };
  
  virtual wxEvent *Clone() const 
    {
      return new CZFXYProjectionChangedEvent(*this); 
    };

  wxWindowID m_id;
  int32_t m_projection;
};
typedef void (wxEvtHandler::*CZFXYProjectionChangedEventFunction)(CZFXYProjectionChangedEvent&);

//-------------------------------------------------------------
//------------------- CZFXYRadiusChangedEvent class --------------------
//-------------------------------------------------------------

class CZFXYRadiusChangedEvent : public wxCommandEvent
{
public:
  CZFXYRadiusChangedEvent(wxWindowID id, int32_t index)
    : wxCommandEvent(wxEVT_ZFXY_RADIUS_CHANGED, id)
  {
    m_index = index;
    m_id = id;
  };

  CZFXYRadiusChangedEvent(const CZFXYRadiusChangedEvent& event)
    : wxCommandEvent(wxEVT_ZFXY_RADIUS_CHANGED, event.m_id)
  {
    m_index = event.m_index;
    m_id = event.m_id;
  };
  
  virtual wxEvent *Clone() const 
    {
      return new CZFXYRadiusChangedEvent(*this); 
    };

  wxWindowID m_id;
  int32_t m_index;
};
typedef void (wxEvtHandler::*CZFXYRadiusChangedEventFunction)(CZFXYRadiusChangedEvent&);

//-------------------------------------------------------------
//------------------- CZFXYFactorChangedEvent class --------------------
//-------------------------------------------------------------

class CZFXYFactorChangedEvent : public wxCommandEvent
{
public:
  CZFXYFactorChangedEvent(wxWindowID id, int32_t index)
    : wxCommandEvent(wxEVT_ZFXY_FACTOR_CHANGED, id)
  {
    m_index = index;
    m_id = id;
  };

  CZFXYFactorChangedEvent(const CZFXYFactorChangedEvent& event)
    : wxCommandEvent(wxEVT_ZFXY_FACTOR_CHANGED, event.m_id)
  {
    m_index = event.m_index;
    m_id = event.m_id;
  };
  
  virtual wxEvent *Clone() const 
    {
      return new CZFXYFactorChangedEvent(*this); 
    };

  wxWindowID m_id;
  int32_t m_index;
};
typedef void (wxEvtHandler::*CZFXYFactorChangedEventFunction)(CZFXYFactorChangedEvent&);


//-------------------------------------------------------------
//------------------- CZFXYViewStateChangedEvent class --------------------
//-------------------------------------------------------------

class CZFXYViewStateChangedEvent : public wxCommandEvent
{
public:
  CZFXYViewStateChangedEvent(wxWindowID id, int32_t index)
    : wxCommandEvent(wxEVT_ZFXY_VIEW_STATE_CHANGED, id)
  {
    m_index = index;
    m_id = id;
  };

  CZFXYViewStateChangedEvent(const CZFXYViewStateChangedEvent& event)
    : wxCommandEvent(wxEVT_ZFXY_VIEW_STATE_CHANGED, event.m_id)
  {
    m_index = event.m_index;
    m_id = event.m_id;
  };
  
  virtual wxEvent *Clone() const 
    {
      return new CZFXYViewStateChangedEvent(*this); 
    };

  wxWindowID m_id;
  int32_t m_index;
};
typedef void (wxEvtHandler::*CZFXYViewStateChangedEventFunction)(CZFXYViewStateChangedEvent&);

//-------------------------------------------------------------
//------------------- CZFXYViewStateChangedEvent class --------------------
//-------------------------------------------------------------

class CZFXYViewStateCommandEvent : public wxCommandEvent
{
public:

  enum actionFlags 
  {
    viewSave,
    viewClear
  };

  CZFXYViewStateCommandEvent(wxWindowID id, actionFlags action)
    : wxCommandEvent(wxEVT_ZFXY_VIEW_STATE_COMMAND, id)
  {
    m_action = action;
    m_id = id;
  };

  CZFXYViewStateCommandEvent(const CZFXYViewStateCommandEvent& event)
    : wxCommandEvent(wxEVT_ZFXY_VIEW_STATE_COMMAND, event.m_id)
  {
    m_action = event.m_action;
    m_id = event.m_id;
  };
  
  virtual wxEvent *Clone() const 
    {
      return new CZFXYViewStateCommandEvent(*this); 
    };

  wxWindowID m_id;
  actionFlags m_action;
};
typedef void (wxEvtHandler::*CZFXYViewStateCommandEventFunction)(CZFXYViewStateCommandEvent&);

//-------------------------------------------------------------
//------------------- CZFXYZoomChangedEvent class --------------------
//-------------------------------------------------------------

class CZFXYZoomChangedEvent : public wxCommandEvent
{
public:
  CZFXYZoomChangedEvent(wxWindowID id, double lat1, double lon1, double lat2, double lon2)
    : wxCommandEvent(wxEVT_ZFXY_ZOOM_CHANGED, id)
  {
    m_lon1 = lon1;
    m_lon2 = lon2;
    m_lat1 = lat1;
    m_lat2 = lat2;
    m_id = id;
  };

  CZFXYZoomChangedEvent(const CZFXYZoomChangedEvent& event)
    : wxCommandEvent(wxEVT_ZFXY_ZOOM_CHANGED, event.m_id)
  {
    m_lon1 = event.m_lon1;
    m_lon2 = event.m_lon2;
    m_lat1 = event.m_lat1;
    m_lat2 = event.m_lat2;
    m_id = event.m_id;
  };
  
  virtual wxEvent *Clone() const 
    {
      return new CZFXYZoomChangedEvent(*this); 
    };

  wxWindowID m_id;
  double m_lon1;
  double m_lon2;
  double m_lat1;
  double m_lat2;

};
typedef void (wxEvtHandler::*CZFXYZoomChangedEventFunction)(CZFXYZoomChangedEvent&);

//-------------------------------------------------------------
//------------------- CZFXYDisplayDataAttrChangedEvent class --------------------
//-------------------------------------------------------------

class CZFXYDisplayDataAttrChangedEvent : public wxCommandEvent
{
public:

  enum displayAttrFlags 
  {
    displayContour,
    displaySolidColor
  };

  CZFXYDisplayDataAttrChangedEvent(wxWindowID id, displayAttrFlags action, bool value)
    : wxCommandEvent(wxEVT_ZFXY_DISPLAY_DATA_ATTR_CHANGED, id)
  {
    m_action = action;
    m_value = value;
    m_id = id;
  };

  CZFXYDisplayDataAttrChangedEvent(const CZFXYDisplayDataAttrChangedEvent& event)
    : wxCommandEvent(wxEVT_ZFXY_DISPLAY_DATA_ATTR_CHANGED, event.m_id)
  {
    m_action = event.m_action;
    m_value = event.m_value;
    m_id = event.m_id;
  };
  
  virtual wxEvent *Clone() const 
    {
      return new CZFXYDisplayDataAttrChangedEvent(*this); 
    };

  wxWindowID m_id;
  displayAttrFlags m_action;
  bool m_value;

};
typedef void (wxEvtHandler::*CZFXYDisplayDataAttrChangedEventFunction)(CZFXYDisplayDataAttrChangedEvent&);
// WDR: class declarations

//-------------------------------------------------------------
//------------------- CZFXYPlotPropertyPanel class --------------------
//-------------------------------------------------------------

//WX_DECLARE_OBJARRAY(wxBoxSizer, wxArrayBoxSizer);
// WX_DECLARE_OBJARRAY(wxStaticBoxSizer, wxArrayStaticBoxSizer); //don't compile because of DECLARE_NO_COPY_CLASS in class wxStaticBoxSizer


class CZFXYPlotPropertyPanel: public wxScrolledWindow
{
public:
  // constructors and destructors
  CZFXYPlotPropertyPanel();
  
  CZFXYPlotPropertyPanel( wxWindow *parent, vtkZFXYPlotActor* plotter, CZFXYPlotData* plotData, wxVTKRenderWindowInteractor* vtkWidget,
      wxWindowID id = -1,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxScrolledWindowStyle );
  
  virtual ~CZFXYPlotPropertyPanel();

  bool Create( wxWindow *parent, vtkZFXYPlotActor* plotter, CZFXYPlotData* plotData, wxVTKRenderWindowInteractor* vtkWidget,
      wxWindowID id = -1,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxScrolledWindowStyle );
 
  void GetRange(double& min, double& max);
  double GetRangeMin();
  double GetRangeMax();

//    wxChoice* GetLayerChoice() {return &m_layerChoice;};
  
  CZFXYPlotData* GetCurrentLayer();
  
  void SetCurrentLayer(int32_t index);
  void SetCurrentLayer(CZFXYPlotData* geoMap);

  void UpdateValues();
  
  void UpdateTitleControls();
  void UpdateLayerControls();

  void UpdateAxesControls();

  void UpdateXAxisControls();
  void UpdateYAxisControls();

  void UpdateXTitleFromControls();
  void UpdateYTitleFromControls();

  void UpdateLogXAxisFromControls();
  void UpdateLogYAxisFromControls();

  void UpdateXBaseFromControls();
  void UpdateYBaseFromControls();

  void UpdateXNTicksFromControls();
  void UpdateYNTicksFromControls();

  void UpdateXMaxFromControls();
  void UpdateXMinFromControls();

  void UpdateYMaxFromControls();
  void UpdateYMinFromControls();

  void UpdateXDigitsFromControls();
  void UpdateYDigitsFromControls();


  bool IsZoomToDataRange() { return m_zoomToDataRange; };
  void SetZoomToDataRange(bool value) { m_zoomToDataRange = value; };

  int32_t GetCurrentFrame() { return m_currentFrame; };
  void SetCurrentFrame(int32_t value) { m_currentFrame = value; };

  void XReset();
  void YReset();
  void FullView();

  void LutChanged( CLutChangedEvent &event );

  void ContourPropChanged( CZFXYContourPropChangedEvent &event );

  static void EvtCheckBox(wxWindow& window, const wxCommandEventFunction& method, wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
  static void EvtButtonClicked(wxWindow& window, const wxCommandEventFunction& method, wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);

  static void EvtDisplayDataAttrChanged(wxWindow& window, const CZFXYDisplayDataAttrChangedEventFunction& method, wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
  static void EvtProjectionChanged(wxWindow& window, const CZFXYProjectionChangedEventFunction& method, wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
  static void EvtCenterPointChanged(wxWindow& window, const CZFXYCenterPointChangedEventFunction& method, wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
  static void EvtRangeChanged(wxWindow& window, const CZFXYRangeChangedEventFunction& method, wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
  static void EvtNumLabelsChanged(wxWindow& window, const CZFXYNumLabelsChangedEventFunction& method, wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
  static void EvtChoice(wxWindow& window, const wxCommandEventFunction& method, wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
  static void EvtViewStateChanged(wxWindow& window, const CZFXYViewStateChangedEventFunction& method, wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
  static void EvtViewStateCommand(wxWindow& window, const CZFXYViewStateCommandEventFunction& method, wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
  static void EvtUpdateRangeCommand(wxWindow& window, const CZFXYUpdateRangeEventFunction& method, wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
  static void EvtZoomChangedCommand(wxWindow& window, const CZFXYZoomChangedEventFunction& method, wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
  static void EvtRadiusChangedCommand(wxWindow& window, const CZFXYRadiusChangedEventFunction& method, wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
  static void EvtFactorChangedCommand(wxWindow& window, const CZFXYFactorChangedEventFunction& method, wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);


  // WDR: method declarations for CZFXYPlotPropertyPanel
  CLabeledTextCtrl* GetZfxydigitsy()  { return (CLabeledTextCtrl*) FindWindow( ID_ZFXYDIGITSY ); }
  CLabeledTextCtrl* GetZfxydigitsx()  { return (CLabeledTextCtrl*) FindWindow( ID_ZFXYDIGITSX ); }
  wxButton* GetZfxyfullview()  { return (wxButton*) FindWindow( ID_ZFXYFULLVIEW ); }
  wxCheckBox* GetZfxylogy()  { return (wxCheckBox*) FindWindow( ID_ZFXYLOGY ); }
  CLabeledTextCtrl* GetZfxybasey()  { return (CLabeledTextCtrl*) FindWindow( ID_ZFXYBASEY ); }
  CLabeledTextCtrl* GetZfxybasex()  { return (CLabeledTextCtrl*) FindWindow( ID_ZFXYBASEX ); }
  wxCheckBox* GetZfxylogx()  { return (wxCheckBox*) FindWindow( ID_ZFXYLOGX ); }
  CLabeledTextCtrl* GetZfxyrangeymax()  { return (CLabeledTextCtrl*) FindWindow( ID_ZFXYRANGEYMAX ); }
  CLabeledTextCtrl* GetZfxyrangeymin()  { return (CLabeledTextCtrl*) FindWindow( ID_ZFXYRANGEYMIN ); }
  CLabeledTextCtrl* GetZfxynumticksy()  { return (CLabeledTextCtrl*) FindWindow( ID_ZFXYNUMTICKSY ); }
  CLabeledTextCtrl* GetZfxytitley()  { return (CLabeledTextCtrl*) FindWindow( ID_ZFXYTITLEY ); }
  wxNotebook* GetZfxyaxes()  { return (wxNotebook*) FindWindow( ID_ZFXYAXES ); }
  CLabeledTextCtrl* GetZfxyrangexmax()  { return (CLabeledTextCtrl*) FindWindow( ID_ZFXYRANGEXMAX ); }
  CLabeledTextCtrl* GetZfxyrangexmin()  { return (CLabeledTextCtrl*) FindWindow( ID_ZFXYRANGEXMIN ); }
  CLabeledTextCtrl* GetZfxynumticksx()  { return (CLabeledTextCtrl*) FindWindow( ID_ZFXYNUMTICKSX ); }
  CLabeledTextCtrl* GetZfxytitlex()  { return (CLabeledTextCtrl*) FindWindow( ID_ZFXYTITLEX ); }
  CLabeledTextCtrl* GetZfxyrangemax()  { return (CLabeledTextCtrl*) FindWindow( ID_ZFXYRANGEMAX ); }
  CLabeledTextCtrl* GetZfxyrangemin()  { return (CLabeledTextCtrl*) FindWindow( ID_ZFXYRANGEMIN ); }
  CLabeledTextCtrl* GetZfxynumlabels()  { return (CLabeledTextCtrl*) FindWindow( ID_ZFXYNUMLABELS ); }
  wxCheckBox* GetZfxyshowcontour()  { return (wxCheckBox*) FindWindow( ID_ZFXYSHOWCONTOUR ); }
  wxCheckBox* GetZfxyshowsolidcolor()  { return (wxCheckBox*) FindWindow( ID_ZFXYSHOWSOLIDCOLOR ); }
  wxChoice* GetZfxylayerchoice()  { return (wxChoice*) FindWindow( ID_ZFXYLAYERCHOICE ); }
  CLabeledTextCtrl* GetZfxyplotpropTitle()  { return (CLabeledTextCtrl*) FindWindow( ID_ZFXYPLOTPROP_TITLE ); }


public:
  
  static const uint32_t m_DIGITS_DEF;
  static const uint32_t m_DIGITS_MIN;
  static const uint32_t m_DIGITS_MAX;

private:

  CVtkZFXYComputeRangeCallback* m_vtkComputeRangeCallback;

    // WDR: member variable declarations for CZFXYPlotPropertyPanel
  wxWindow* m_parent;


  wxVTKRenderWindowInteractor* m_vtkWidget;
  vtkZFXYPlotActor* m_plotter;

  CZFXYPlotData* m_plotData;

  double m_min;
  double m_max;
  int32_t m_nLabels;

  uint32_t m_numberOfXDigits;
  uint32_t m_numberOfYDigits;

  bool m_zoomToDataRange;
  int32_t m_currentFrame;

private:

  void Init();

  void InstallToolTips();
  void InstallEventListeners();


  // WDR: handler declarations for CZFXYPlotPropertyPanel
  void OnKeyframeChanged(CKeyframeEvent& event);
  void OnFullView( wxCommandEvent &event );
  void OnYLog( wxCommandEvent &event );
  void OnXLog( wxCommandEvent &event );
  void OnYReset( wxCommandEvent &event );
  void OnXReset( wxCommandEvent &event );
  void OnReset(wxCommandEvent& event);
  void OnEditColorTable(wxCommandEvent& event);
  void OnEditContourProperties(wxCommandEvent& event);
  
  void OnMinRange(CValueChangedEvent& event);
  void OnMaxRange(CValueChangedEvent& event);
  void OnNumLabels(CValueChangedEvent& event);

  
  void OnSolidColor(wxCommandEvent& event);
  void OnContour(wxCommandEvent& event);

  void OnLayerChoice(wxCommandEvent &event);

  void OnRangeChanged(CZFXYRangeChangedEvent& event);
  void OnNumLabelsChanged(CZFXYNumLabelsChangedEvent& event);
  

  void OnTitle(CValueChangedEvent& event);


  void OnXTitle(CValueChangedEvent& event);
  void OnXNTicks(CValueChangedEvent& event);
  void OnXMin(CValueChangedEvent& event);
  void OnXMax(CValueChangedEvent& event);
  void OnXBase(CValueChangedEvent& event);
  void OnXDigits(CValueChangedEvent& event);


  void OnYTitle(CValueChangedEvent& event);
  void OnYNTicks(CValueChangedEvent& event);
  void OnYMin(CValueChangedEvent& event);
  void OnYMax(CValueChangedEvent& event);
  void OnYBase(CValueChangedEvent& event);
  void OnYDigits(CValueChangedEvent& event);


private:
    DECLARE_EVENT_TABLE()
};

//-------------------------------------------------------------
//------------------- CVtkZFXYComputeRangeCallback class --------------------
//-------------------------------------------------------------

class CVtkZFXYComputeRangeCallback : public vtkCommand
{
protected:
  CVtkZFXYComputeRangeCallback(CZFXYPlotPropertyPanel *panel)
    {
      m_panel = panel;
    }

public:  
  static CVtkZFXYComputeRangeCallback *New(CZFXYPlotPropertyPanel *panel) 
    { return new CVtkZFXYComputeRangeCallback(panel); }
  
  virtual void Execute(vtkObject *caller, unsigned long, void*)
    {
      if (m_panel == NULL)
      {
        return;
      }
      m_panel->UpdateAxesControls();

    }
protected:

  CZFXYPlotPropertyPanel* m_panel;
};




#endif
