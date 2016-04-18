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

#ifndef __LUTFrame_H__
#define __LUTFrame_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "LUTFrame.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/docview.h"
#include "wx/filename.h"


#include "new-gui/brat/DataModels/PlotData/BratLookupTable.h"
#include "BratDisplay_wdr.h"
#include "LUTPanel.h"

class CLUTPanel;

// WDR: class declarations

//----------------------------------------------------------------------------
// CLUTFrame
//----------------------------------------------------------------------------

class CLUTFrame: public wxFrame
{
public:
    // constructors and destructors
  CLUTFrame();

  CLUTFrame( wxWindow *parent, wxWindowID id, const wxString &title, CBratLookupTable* lut, bool apply = false,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxDEFAULT_FRAME_STYLE );

  virtual ~CLUTFrame();

  bool Create( wxWindow *parent, wxWindowID id, const wxString &title, CBratLookupTable* lut, bool apply = false,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxDEFAULT_FRAME_STYLE );

  void UpdateMinSize();

  void PostLutChangedEvent(CBratLookupTable* lut);
  
  bool Destroy();
  
  //void SetLUT(CBratLookupTable* lut);
  void UpdateMode(bool updateLUTPanel = false);
  void UpdateMenuMode();

  CLUTPanel* GetLUTPanel() {return m_lutPanel;};


    // WDR: method declarations for CLUTFrame
    
private:
  
  wxFileName m_fileName;
  
  bool m_apply;
  bool m_recentFiles;

  wxMenuBar* m_menuBar;
  wxFileHistory m_fileHistory;

  wxMenu* m_menuFile;
  wxMenu* m_menuEdit;
  wxMenu* m_menuMode;
  wxMenu* m_menuHistory;

  CLUTPanel* m_lutPanel;
  wxBoxSizer* m_sizer;
    // WDR: member variable declarations for CLUTFrame
    
private:
  void Init();
  void CreateMenuBar();
  bool CreateControls(CBratLookupTable* lut);
  void CreateLayout();
  void InstallEventListeners();
  void DeInstallEventListeners();

  void Open();

  void AddFileToHistory(const wxString& fileName);
  void LoadFromFileHistory(int32_t fileNum);

  void FileHistoryToConfig();
  void ConfigToFileHistory();


  // WDR: handler declarations for CLUTFrame
  void OnUpdateUIFileSave( wxUpdateUIEvent &event );
  void OnModeUpdateUI( wxUpdateUIEvent &event );
  void OnModeCustom( wxCommandEvent &event );
  void OnModeGradient( wxCommandEvent &event );
  void OnModeStandard( wxCommandEvent &event );
  void OnClose( wxCommandEvent &event );
  void OnReset( wxCommandEvent &event );
  void OnOpen( wxCommandEvent &event );
  void OnSave( wxCommandEvent &event );
  void OnCurveLinear( wxCommandEvent &event );
  void OnCurveCosinus( wxCommandEvent &event );
  void OnCurveSQRT( wxCommandEvent &event );
  void OnSetFacets( wxCommandEvent &event );
  void OnCloseWindow( wxCloseEvent &event );
  void OnFileHistory( wxCommandEvent &event );

private:
    DECLARE_EVENT_TABLE()
};




#endif
