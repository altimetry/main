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

#ifndef __ZFXYPlotFrame_H__
#define __ZFXYPlotFrame_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "ZFXYPlotFrame.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/filename.h"

//#include "BratDisplay_wdr.h"

#include "ZFXYPlotPanel.h"
#include "ZFXYPlotData.h"
#include "LUTFrame.h"
#include "ZFXYContourPropFrame.h"
#include "WindowHandler.h"


BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_ZFXY_UPDATE_MINSIZE, 7777) // 7777 is ignored, just for compatibility with v2.2
END_DECLARE_EVENT_TYPES()
//-------------------------------------------------------------
//------------------- CUpdateMinSizeEvent class --------------------
//-------------------------------------------------------------

class CZFXYUpdateMinSizeEvent : public wxCommandEvent
{
public:
  CZFXYUpdateMinSizeEvent(wxWindowID id)
    : wxCommandEvent(wxEVT_ZFXY_UPDATE_MINSIZE, id)
  {
    m_id = id;
  };

  CZFXYUpdateMinSizeEvent(const CZFXYUpdateMinSizeEvent& event)
    : wxCommandEvent(wxEVT_ZFXY_UPDATE_MINSIZE, event.m_id)
  {
    m_id = event.m_id;
  };
  
  virtual wxEvent *Clone() const 
    {
      return new CZFXYUpdateMinSizeEvent(*this); 
    };

  wxWindowID m_id;
};
typedef void (wxEvtHandler::*CZFXYUpdateMinSizeEventFunction)(CZFXYUpdateMinSizeEvent&);




// WDR: class declarations

//----------------------------------------------------------------------------
// CZFXYPlotFrame
//----------------------------------------------------------------------------

class CZFXYPlotFrame: public wxFrame
{
public:
  // constructors and destructors
  CZFXYPlotFrame();
  CZFXYPlotFrame( wxWindow *parent, wxWindowID id, 
                   const wxString &title = "",
                   CZFXYPlotProperty* plotProperty = NULL,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = CWindowHandler::GetDefaultSize(),
                   long style = wxDEFAULT_FRAME_STYLE );

  virtual ~CZFXYPlotFrame();
   
  bool Create( wxWindow *parent, wxWindowID id, 
                   const wxString &title = "",
                   CZFXYPlotProperty* plotProperty = NULL,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = CWindowHandler::GetDefaultSize(),
                   long style = wxDEFAULT_FRAME_STYLE );
  bool Destroy();
 
  CZFXYPlotPanel* GetPlotPlanel() {return m_plotPanel;};

  void AddData(CZFXYPlotData* pdata);

  void UpdateView();

  static void EvtUpdateMinSize(wxWindow& window, int32_t winId, const CZFXYUpdateMinSizeEventFunction& method,
                               wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);


    // WDR: method declarations for CZFXYPlotFrame
    
private:

  CZFXYPlotPanel* m_plotPanel;
  CZFXYPlotProperty m_plotProperty;
  
  bool m_multiFrame;
  bool m_hasClut;

  wxBoxSizer* m_sizer;

  wxMenuBar* m_menuBar;
  
  CLUTFrame* m_lutFrame;
  CZFXYContourPropFrame* m_contourPropFrame;

  //wxMenu* m_menuView;


  // WDR: member variable declarations for CZFXYPlotFrame
    
private:
  void Init();
  void CreateMenuBar();
  bool CreateControls();
  void CreateLayout();
  void UpdateMinSize();
  void InstallEventListeners();
  void DeInstallEventListeners();


  void ShowColorBar(bool showIt);
  void ShowPropertyPanel(bool showIt);
  void ShowAnimationToolbar(bool showIt);

  void WriteImage(const wxFileName& fileName);


  // WDR: handler declarations for CZFXYPlotFrame
  void OnShowContourProps( wxCommandEvent &event );
  void OnUpdateUIAnimationToolbar( wxUpdateUIEvent &event );
  void OnClose( wxCommandEvent &event );
  void OnShowColorTableEditor( wxCommandEvent &event );
  void OnViewColorbar( wxCommandEvent &event );
  void OnViewProps( wxCommandEvent &event );
  void OnViewSlider( wxCommandEvent &event );
  void OnSave( wxCommandEvent &event );
  void OnCloseWindow( wxCloseEvent &event );
  void OnLutChanged( CLutChangedEvent &event );
  void OnDestroyLUT( wxWindowDestroyEvent &event );
  void OnLutFrameClose( CLutFrameCloseEvent &event );
  void OnDestroyContourPropFrame( wxWindowDestroyEvent &event );
  void OnContourPropFrameClose( CZFXYContourPropFrameCloseEvent &event );
  void OnContourPropChanged( CZFXYContourPropChangedEvent &event );
  void OnUpdateMinSize(CZFXYUpdateMinSizeEvent& event);

private:
  DECLARE_CLASS(CZFXYPlotFrame)
  DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// CZFXYProgressDialog
//----------------------------------------------------------------------------

class CZFXYProgressDialog: public wxProgressDialog
{
public:
    // constructors and destructors
   CZFXYProgressDialog(const wxString &title, wxString const &message,
                    int maximum = 100,
                    wxWindow *parent = NULL,
                    int style = wxPD_APP_MODAL | wxPD_AUTO_HIDE);
   virtual ~CZFXYProgressDialog();

   wxTimer* m_timer;
   int32_t m_count;


    // WDR: method declarations for CZFXYProgressDialog
    
private:
    // WDR: member variable declarations for CZFXYProgressDialog
    
private:
    // WDR: handler declarations for CZFXYProgressDialog
  void OnProgress(wxTimerEvent& event);
  void OnDestroy( wxWindowDestroyEvent &event );

private:
    DECLARE_CLASS(CZFXYProgressDialog)
    DECLARE_EVENT_TABLE()
};

#endif
