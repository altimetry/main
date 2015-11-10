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


#ifndef __ContourPropFrame_H__
#define __ContourPropFrame_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "ContourPropFrame.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "BratDisplay_wdr.h"
#include "LabeledTextCtrl.h"
#include "PlotData/WorldPlotData.h"

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_CONTOUR_PROP_CHANGED, 7777) // 7777 is ignored, just for compatibility with v2.2
    DECLARE_EVENT_TYPE(wxEVT_CONTOUR_PROP_CLOSE, 7777) // 7777 is ignored, just for compatibility with v2.2
END_DECLARE_EVENT_TYPES()

//-------------------------------------------------------------
//------------------- CContourPropChangedEvent class --------------------
//-------------------------------------------------------------


class CContourPropChangedEvent : public wxCommandEvent
{
public:
  CContourPropChangedEvent(wxWindowID id, CWorldPlotProperty& plotProperty)
    : wxCommandEvent(wxEVT_CONTOUR_PROP_CHANGED, id)
  {
    m_id = id;
    m_plotProperty = plotProperty;
  };

  CContourPropChangedEvent(const CContourPropChangedEvent& event)
    : wxCommandEvent(wxEVT_CONTOUR_PROP_CHANGED, event.m_id)
  {
    m_id = event.m_id;
    m_plotProperty = event.m_plotProperty;
  };
  
  virtual wxEvent *Clone() const 
    {
      return new CContourPropChangedEvent(*this); 
    };

  wxWindowID m_id;
  CWorldPlotProperty m_plotProperty;
};
typedef void (wxEvtHandler::*CContourPropChangedEventFunction)(CContourPropChangedEvent&);

//-------------------------------------------------------------
//------------------- CContourPropFrameCloseEvent class --------------------
//-------------------------------------------------------------


class CContourPropFrameCloseEvent : public wxCommandEvent
{
public:
  CContourPropFrameCloseEvent(wxWindowID id)
    : wxCommandEvent(wxEVT_CONTOUR_PROP_CLOSE, id)
  {
    m_id = id;
  };

  CContourPropFrameCloseEvent(const CContourPropFrameCloseEvent& event)
    : wxCommandEvent(wxEVT_CONTOUR_PROP_CLOSE, event.m_id)
  {
    m_id = event.m_id;
  };
  
  virtual wxEvent *Clone() const 
    {
      return new CContourPropFrameCloseEvent(*this); 
    };

  wxWindowID m_id;
};
typedef void (wxEvtHandler::*CContourPropFrameCloseEventFunction)(CContourPropFrameCloseEvent&);

// WDR: class declarations

//----------------------------------------------------------------------------
// CContourPropPanel
//----------------------------------------------------------------------------

class CContourPropPanel: public wxPanel
{
public:
  // constructors and destructors
  CContourPropPanel();
  
  CContourPropPanel( wxWindow *parent, CGeoMap* geoMap, wxWindowID id = -1,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxTAB_TRAVERSAL | wxNO_BORDER );
  
  virtual ~CContourPropPanel();

  bool Create( wxWindow *parent, CGeoMap* geoMap, wxWindowID id = -1,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxTAB_TRAVERSAL | wxNO_BORDER );
  
  // WDR: method declarations for CContourPropPanel
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

  static void EvtContourPropChanged(wxWindow& window, int32_t winId, const CContourPropChangedEventFunction& method,
                                          wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
  static void EvtContourPropFrameClose(wxWindow& window, int32_t winId, const CContourPropFrameCloseEventFunction& method,
                                          wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
  
private:
  CGeoMap* m_geoMap;

  CWorldPlotProperty m_plotProperty;

  CVtkColor m_lineColor;
  CVtkColor m_labelColor;

    
private:
  void Init();
  void InstallEventListeners();
  void InstallToolTips();
  void FillProperties();
  void GetProperties();

    // WDR: member variable declarations for CContourPropPanel
    
private:
    // WDR: handler declarations for CContourPropPanel
  void OnClose( wxCommandEvent &event );
  void OnApply( wxCommandEvent &event );
  void OnSameColor( wxCommandEvent &event );
  void OnLabelColor( wxCommandEvent &event );
  void OnLineColor( wxCommandEvent &event );

private:
    DECLARE_CLASS(CContourPropPanel)
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// CContourPropFrame
//----------------------------------------------------------------------------

class CContourPropFrame: public wxFrame
{
public:
  // constructors and destructors
  CContourPropFrame();

  CContourPropFrame( wxWindow *parent, CGeoMap* geoMap, wxWindowID id = -1, 
      const wxString &title = "",
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxDEFAULT_FRAME_STYLE );

  virtual ~CContourPropFrame();

  bool Create( wxWindow *parent, CGeoMap* geoMap, wxWindowID id = -1, 
      const wxString &title = "",
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxDEFAULT_FRAME_STYLE );

  CContourPropPanel* GetContourPropPanel() {return m_contourPropPanel;};

  // WDR: method declarations for CContourPropFrame

  bool Destroy();

private:
  CContourPropPanel* m_contourPropPanel;

private:
  void Init();
  void InstallEventListeners();
  void InstallToolTips();
  void UpdateMinSize();
  // WDR: member variable declarations for CContourPropFrame
    
private:
  // WDR: handler declarations for CContourPropFrame
  void OnClose( wxCommandEvent &event );
  void OnCloseWindow( wxCloseEvent &event );

private:
    DECLARE_CLASS(CContourPropFrame)
    DECLARE_EVENT_TABLE()
};




#endif
