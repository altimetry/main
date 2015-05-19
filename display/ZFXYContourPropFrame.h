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


#ifndef __ZFXYContourPropFrame_H__
#define __ZFXYContourPropFrame_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "ZFXYContourPropFrame.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "BratDisplay_wdr.h"
#include "LabeledTextCtrl.h"
#include "ZFXYPlotData.h"

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_ZFXY_CONTOUR_PROP_CHANGED, 7777) // 7777 is ignored, just for compatibility with v2.2
    DECLARE_EVENT_TYPE(wxEVT_ZFXY_CONTOUR_PROP_CLOSE, 7777) // 7777 is ignored, just for compatibility with v2.2
END_DECLARE_EVENT_TYPES()

//-------------------------------------------------------------
//------------------- CZFXYContourPropChangedEvent class --------------------
//-------------------------------------------------------------


class CZFXYContourPropChangedEvent : public wxCommandEvent
{
public:
  CZFXYContourPropChangedEvent(wxWindowID id, CZFXYPlotProperty& plotProperty)
    : wxCommandEvent(wxEVT_ZFXY_CONTOUR_PROP_CHANGED, id)
  {
    m_id = id;
    m_plotProperty = plotProperty;
  };

  CZFXYContourPropChangedEvent(const CZFXYContourPropChangedEvent& event)
    : wxCommandEvent(wxEVT_ZFXY_CONTOUR_PROP_CHANGED, event.m_id)
  {
    m_id = event.m_id;
    m_plotProperty = event.m_plotProperty;
  };
  
  virtual wxEvent *Clone() const 
    {
      return new CZFXYContourPropChangedEvent(*this); 
    };

  wxWindowID m_id;
  CZFXYPlotProperty m_plotProperty;
};
typedef void (wxEvtHandler::*CZFXYContourPropChangedEventFunction)(CZFXYContourPropChangedEvent&);

//-------------------------------------------------------------
//------------------- CZFXYContourPropFrameCloseEvent class --------------------
//-------------------------------------------------------------


class CZFXYContourPropFrameCloseEvent : public wxCommandEvent
{
public:
  CZFXYContourPropFrameCloseEvent(wxWindowID id)
    : wxCommandEvent(wxEVT_ZFXY_CONTOUR_PROP_CLOSE, id)
  {
    m_id = id;
  };

  CZFXYContourPropFrameCloseEvent(const CZFXYContourPropFrameCloseEvent& event)
    : wxCommandEvent(wxEVT_ZFXY_CONTOUR_PROP_CLOSE, event.m_id)
  {
    m_id = event.m_id;
  };
  
  virtual wxEvent *Clone() const 
    {
      return new CZFXYContourPropFrameCloseEvent(*this); 
    };

  wxWindowID m_id;
};
typedef void (wxEvtHandler::*CZFXYContourPropFrameCloseEventFunction)(CZFXYContourPropFrameCloseEvent&);

// WDR: class declarations

//----------------------------------------------------------------------------
// CZFXYContourPropPanel
//----------------------------------------------------------------------------

class CZFXYContourPropPanel: public wxPanel
{
public:
  // constructors and destructors
  CZFXYContourPropPanel();
  
  CZFXYContourPropPanel( wxWindow *parent, CZFXYPlotData* geoMap, wxWindowID id = -1,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxTAB_TRAVERSAL | wxNO_BORDER );
  
  virtual ~CZFXYContourPropPanel();

  bool Create( wxWindow *parent, CZFXYPlotData* geoMap, wxWindowID id = -1,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxTAB_TRAVERSAL | wxNO_BORDER );
  
  // WDR: method declarations for CZFXYContourPropPanel
  CLabeledTextCtrl* GetContourLabelNumber()  { return (CLabeledTextCtrl*) FindWindow( ID_CONTOUR_LABEL_NUMBER ); }
  wxButton* GetContourApply()  { return (wxButton*) FindWindow( ID_CONTOUR_APPLY ); }
  wxButton* GetContourClose()  { return (wxButton*) FindWindow( ID_CONTOUR_CLOSE ); }
  CLabeledTextCtrl* GetContourLabelFormat()  { return (CLabeledTextCtrl*) FindWindow( ID_CONTOUR_LABEL_FORMAT ); }
  CLabeledTextCtrl* GetContourLabelSize()  { return (CLabeledTextCtrl*) FindWindow( ID_CONTOUR_LABEL_SIZE ); }
  wxCheckBox* GetContourLabelColorSame()  { return (wxCheckBox*) FindWindow( ID_CONTOUR_LABEL_COLOR_SAME ); }
  wxButton* GetContourLabelColor()  { return (wxButton*) FindWindow( ID_CONTOUR_LABEL_COLOR ); }
  wxCheckBox* GetContourLabel()  { return (wxCheckBox*) FindWindow( ID_CONTOUR_LABEL ); }
  CLabeledTextCtrl* GetContourLineWidth()  { return (CLabeledTextCtrl*) FindWindow( ID_CONTOUR_LINE_WIDTH ); }
  wxButton* GetContourLineColor()  { return (wxButton*) FindWindow( ID_CONTOUR_LINE_COLOR ); }
  CLabeledTextCtrl* GetContourNumber()  { return (CLabeledTextCtrl*) FindWindow( ID_CONTOUR_NUMBER ); }
  CLabeledTextCtrl* GetContourMax()  { return (CLabeledTextCtrl*) FindWindow( ID_CONTOUR_MAX ); }
  CLabeledTextCtrl* GetContourMin()  { return (CLabeledTextCtrl*) FindWindow( ID_CONTOUR_MIN ); }

  static void EvtContourPropChanged(wxWindow& window, int32_t winId, const CZFXYContourPropChangedEventFunction& method,
                                          wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
  static void EvtContourPropFrameClose(wxWindow& window, int32_t winId, const CZFXYContourPropFrameCloseEventFunction& method,
                                          wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
  
private:
  CZFXYPlotData* m_geoMap;

  CZFXYPlotProperty m_plotProperty;

  CVtkColor m_lineColor;
  CVtkColor m_labelColor;

    
private:
  void Init();
  void InstallEventListeners();
  void InstallToolTips();
  void FillProperties();
  void GetProperties();

    // WDR: member variable declarations for CZFXYContourPropPanel
    
private:
    // WDR: handler declarations for CZFXYContourPropPanel
  void OnClose( wxCommandEvent &event );
  void OnApply( wxCommandEvent &event );
  void OnSameColor( wxCommandEvent &event );
  void OnLabelColor( wxCommandEvent &event );
  void OnLineColor( wxCommandEvent &event );

private:
    DECLARE_CLASS(CZFXYContourPropPanel)
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// CZFXYContourPropFrame
//----------------------------------------------------------------------------

class CZFXYContourPropFrame: public wxFrame
{
public:
  // constructors and destructors
  CZFXYContourPropFrame();

  CZFXYContourPropFrame( wxWindow *parent, CZFXYPlotData* geoMap, wxWindowID id = -1, 
      const wxString &title = "",
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxDEFAULT_FRAME_STYLE );

  virtual ~CZFXYContourPropFrame();

  bool Create( wxWindow *parent, CZFXYPlotData* geoMap, wxWindowID id = -1, 
      const wxString &title = "",
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxDEFAULT_FRAME_STYLE );

  CZFXYContourPropPanel* GetContourPropPanel() {return m_contourPropPanel;};

  // WDR: method declarations for CZFXYContourPropFrame

  bool Destroy();

private:
  CZFXYContourPropPanel* m_contourPropPanel;

private:
  void Init();
  void InstallEventListeners();
  void InstallToolTips();
  void UpdateMinSize();
  // WDR: member variable declarations for CZFXYContourPropFrame
    
private:
  // WDR: handler declarations for CZFXYContourPropFrame
  void OnCloseWindow( wxCloseEvent &event );

private:
    DECLARE_CLASS(CZFXYContourPropFrame)
    DECLARE_EVENT_TABLE()
};




#endif
