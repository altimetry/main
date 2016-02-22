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

#ifndef __WorldPlotFrame_H__
#define __WorldPlotFrame_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "WorldPlotFrame.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/filename.h"

//#include "BratDisplay_wdr.h"

#include "WorldPlotPanel.h"
#include "new-gui/brat/DataModels/PlotData/WorldPlotData.h"
#include "LUTFrame.h"
#include "ContourPropFrame.h"
#include "WindowHandler.h"


BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_UPDATE_MINSIZE, 7777) // 7777 is ignored, just for compatibility with v2.2
END_DECLARE_EVENT_TYPES()
//-------------------------------------------------------------
//------------------- CUpdateMinSizeEvent class --------------------
//-------------------------------------------------------------

class CUpdateMinSizeEvent : public wxCommandEvent
{
public:
  CUpdateMinSizeEvent(wxWindowID id)
    : wxCommandEvent(wxEVT_UPDATE_MINSIZE, id)
  {
    m_id = id;
  };

  CUpdateMinSizeEvent(const CUpdateMinSizeEvent& event)
    : wxCommandEvent(wxEVT_UPDATE_MINSIZE, event.m_id)
  {
    m_id = event.m_id;
  };
  
  virtual wxEvent *Clone() const 
    {
      return new CUpdateMinSizeEvent(*this); 
    };

  wxWindowID m_id;
};
typedef void (wxEvtHandler::*CUpdateMinSizeEventFunction)(CUpdateMinSizeEvent&);




// WDR: class declarations

//----------------------------------------------------------------------------
// CWorldPlotFrame
//----------------------------------------------------------------------------

class CWorldPlotFrame: public wxFrame
{
public:
  // constructors and destructors
  CWorldPlotFrame();
  CWorldPlotFrame( wxWindow *parent, wxWindowID id, 
                   const wxString &title = "",
                   CWorldPlotProperties* plotProperty = NULL,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = CWindowHandler::GetDefaultSize(),
                   long style = wxDEFAULT_FRAME_STYLE );

  virtual ~CWorldPlotFrame();
   
  bool Create( wxWindow *parent, wxWindowID id, 
                   const wxString &title = "",
                   CWorldPlotProperties* plotProperty = NULL,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = CWindowHandler::GetDefaultSize(),
                   long style = wxDEFAULT_FRAME_STYLE );
  bool Destroy();
 
  CWorldPlotPanel* GetPlotPlanel() {return m_plotPanel;};

  void AddData(VTK_CWorldPlotCommonData* data);

  void SetCenterLatitude(double c);
  void SetCenterLongitude(double c);

  void UpdateView();

  static void EvtUpdateMinSize(wxWindow& window, int32_t winId, const CUpdateMinSizeEventFunction& method,
                               wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);


    // WDR: method declarations for CWorldPlotFrame
    
private:

  CWorldPlotPanel* m_plotPanel;
  CWorldPlotProperties m_plotProperty;
  
  bool m_multiFrame;
  bool m_hasClut;

  wxBoxSizer* m_sizer;

  wxMenuBar* m_menuBar;
  
  CLUTFrame* m_lutFrame;
  CContourPropFrame* m_contourPropFrame;

  //wxMenu* m_menuView;


  // WDR: member variable declarations for CWorldPlotFrame
    
private:
  void Init();
  void CreateMenuBar();
  //bool CreateControls();
  void CreateLayout();
  void UpdateMinSize();
  void InstallEventListeners();
  void DeInstallEventListeners();


  void ShowColorBar(bool showIt);
  void ShowPropertyPanel(bool showIt);
  void ShowAnimationToolbar(bool showIt);

  void WriteImage(const wxFileName& fileName);


  // WDR: handler declarations for CWorldPlotFrame
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
  void OnContourPropFrameClose( CContourPropFrameCloseEvent &event );
  void OnContourPropChanged( CContourPropChangedEvent &event );
  void OnUpdateMinSize(CUpdateMinSizeEvent& event);

private:
  DECLARE_CLASS(CWorldPlotFrame)
  DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// CProgressDialog
//----------------------------------------------------------------------------

class CProgressDialog: public wxProgressDialog
{
public:
    // constructors and destructors
   CProgressDialog(const wxString &title, wxString const &message,
                    int maximum = 100,
                    wxWindow *parent = NULL,
                    int style = wxPD_APP_MODAL | wxPD_AUTO_HIDE);
   virtual ~CProgressDialog();

   wxTimer* m_timer;
   int32_t m_count;


    // WDR: method declarations for CProgressDialog
    
private:
    // WDR: member variable declarations for CProgressDialog
    
private:
    // WDR: handler declarations for CProgressDialog
  void OnProgress(wxTimerEvent& event);
  void OnDestroy( wxWindowDestroyEvent &event );

private:
    DECLARE_CLASS(CProgressDialog)
    DECLARE_EVENT_TABLE()
};

#endif
