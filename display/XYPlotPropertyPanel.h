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


#ifndef __XYPlotPropertyPanel_H__
#define __XYPlotPropertyPanel_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "XYPlotPropertyPanel.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/notebook.h"

//#include "BratDisplay_wdr.h"

#include "wxVTKRenderWindowInteractor.h"
#include "vtkXYPlotActor.h"

#include "PlotData/XYPlotData.h"
#include "AxisPropertyPanel.h"

#include "LabeledTextCtrl.h"

extern long ID_XYPLOTPROP_TITLE;
extern long ID_XYFULLVIEW;

// WDR: class declarations

//----------------------------------------------------------------------------
// CXYPlotPropertyPanel
//----------------------------------------------------------------------------

class CXYPlotPropertyPanel: public wxScrolledWindow
{
public:
  // constructors and destructors
  CXYPlotPropertyPanel();


  CXYPlotPropertyPanel( wxWindow *parent, vtkXYPlotActor* plotter, CXYPlotDataCollection* plotDataCollection, wxVTKRenderWindowInteractor* vtkWidget,
      wxWindowID id = -1,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxScrolledWindowStyle );

  virtual ~CXYPlotPropertyPanel();
  
  bool Create( wxWindow *parent, vtkXYPlotActor* plotter, CXYPlotDataCollection* plotDataCollection, wxVTKRenderWindowInteractor* vtkWidget,
      wxWindowID id = -1,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxScrolledWindowStyle );

  void UpdateValues();

  void XReset();
  void YReset();

  void FullView();

  void OnFrameChange(wxEvent& event);

  // WDR: method declarations for CXYPlotPropertyPanel


public:
    bool m_finished;
  
private:
  wxWindow* m_parent;

  wxVTKRenderWindowInteractor* m_vtkWidget;
  vtkXYPlotActor* m_plotter;

  CXYPlotDataCollection* m_plotDataCollection;

  CLabeledTextCtrl m_titleCtrl;
  wxButton m_resetView;
  wxNotebook m_axisNotebook;

  CAxisXPropertyPanel* m_axisXPanel;
  CAxisYPropertyPanel* m_axisYPanel;

  //wxNotebookSizer* m_axisNotebookSizer;
  wxBoxSizer* m_sizer;
  wxBoxSizer* m_sizer1;

  // WDR: member variable declarations for CXYPlotPropertyPanel
    
private:
  void Init();

  void CreateBoxSizers();

  bool CreateControls();
  void CreateLayout();
  void InstallToolTips();
  void InstallEventListeners();


  // WDR: handler declarations for CXYPlotPropertyPanel
  void OnTitle(CValueChangedEvent& event);
  void OnFullView( wxCommandEvent &event );

private:
    DECLARE_EVENT_TABLE()
};




#endif
