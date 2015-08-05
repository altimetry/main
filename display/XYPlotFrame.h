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

#ifndef __XYPlotFrame_H__
#define __XYPlotFrame_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "XYPlotFrame.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/filename.h"

#include "BratDisplay_wdr.h"

#include "XYPlotPanel.h"
#include "XYPlotData.h"
#include "WindowHandler.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// CXYPlotFrame
//----------------------------------------------------------------------------

class CXYPlotFrame: public wxFrame
{
public:
    // constructors and destructors
  CXYPlotFrame();

  CXYPlotFrame( wxWindow *parent, wxWindowID id, 
                   const wxString &title = "",
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = CWindowHandler::GetDefaultSize(),
                   long style = wxDEFAULT_FRAME_STYLE );

  virtual ~CXYPlotFrame();
   
  bool Create( wxWindow *parent, wxWindowID id, 
                   const wxString &title = "",
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = CWindowHandler::GetDefaultSize(),
                   long style = wxDEFAULT_FRAME_STYLE );
  bool Destroy();

  void AddData(CXYPlotData* data);

  CXYPlotPanel* GetPlotPlanel() {return m_plotPanel;};
  CXYPlotProperty* GetPlotProperty(int32_t index = 0);

  void ShowPropertyPanel();
  void ShowPropertyPanel(bool showIt);
  void ShowAnimationToolbar(bool showIt);
    
    // WDR: method declarations for CXYPlotFrame
    
private:

  CXYPlotPanel* m_plotPanel;

  bool m_multiFrame;

  wxBoxSizer* m_sizer;

  wxMenuBar* m_menuBar;
  

  // WDR: member variable declarations for CXYPlotFrame
    
private:
  void Init();
  void CreateMenuBar();
  bool CreateControls();
  void CreateLayout();
  void UpdateMinSize();
  void InstallEventListeners();
  void DeInstallEventListeners();

  void WriteImage(const wxFileName& fileName);


  // WDR: handler declarations for CXYPlotFrame
  void OnExport( wxCommandEvent &event );
  void OnClose( wxCommandEvent &event );
  void OnViewProps( wxCommandEvent &event );
  void OnViewSlider( wxCommandEvent &event );
  void OnSave( wxCommandEvent &event );
  void OnCloseWindow( wxCloseEvent &event );

private:
  DECLARE_CLASS(CXYPlotFrame)
  DECLARE_EVENT_TABLE()
};




#endif
