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

#ifndef __XYPlotPanel_H__
#define __XYPlotPanel_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "XYPlotPanel.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "vtkTransformCollection.h"
#include "vtkTextProperty.h"
#include "vtkTextActor.h"
#include "vtkTextMapper.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkGenericRenderWindowInteractor.h"
#include "vtkCommand.h"

#include "wxVTKRenderWindowInteractor.h"


#include "vtkInteractorStyleXYPlot.h"

#include "Tools.h"
#include "List.h"
using namespace brathl;

//#include "BratDisplay_wdr.h"
#include "AnimationToolbar.h"
#include "XYPlotData.h"
#include "XYPlotPropertyPanel.h"
#include "DatasetPanel.h"


extern long ID_XYPLOTPANEL_VTK_INTERACTOR;
extern long ID_XYPLOTPANEL_SPLIT;
extern long ID_XYPLOTPANEL_PROPPANEL;
extern long ID_XYPLOTPANEL_PROPTAB;
extern long ID_XYPLOTPANEL_NOTEBOOK;

class CVtkXYPlotCharEventCallback;


// WDR: class declarations

//----------------------------------------------------------------------------
// CXYPlotPanel
//----------------------------------------------------------------------------

class CXYPlotPanel: public wxPanel
{
public:
  // constructors and destructors
  CXYPlotPanel();
  
  CXYPlotPanel( wxWindow *parent, wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL | wxNO_BORDER );
  
  virtual ~CXYPlotPanel();
  
  bool Create( wxWindow *parent, wxWindowID id = -1,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxTAB_TRAVERSAL | wxNO_BORDER );

  virtual bool Destroy();

  CXYPlotPropertyPanel* GetPlotPropertyTab() {return m_plotPropertyTab;};

  wxVTKRenderWindowInteractor* GetVtkWidget() {return m_vtkWidget;};

  void SetXRange(double min, double max);
  void SetYRange(double min, double max);

  void SetBaseX(double value);
  void SetBaseY(double value);

  void SetLogX(bool value);
  void SetLogY(bool value);

  void SetXTitle(const wxString& value);
  void SetYTitle(const wxString& value);

  void SetTitle(const wxString& value);

  void SetFPS(int32_t value);

  vtkXYPlotActor* GetPlotActor() {return m_plotActor;};


  void UpdateRender();

  void SetAnimationDescr(uint32_t indexValue);

  void AddData(CXYPlotData* pdata);

  CXYPlotProperty* GetPlotProperty(int32_t index);

  CXYPlotDataCollection* GetPlotDataCollection() {return m_plotDataCollection;};

  void ShowAnimationToolbar(bool showIt);
  void ShowPropertyPanel(bool showIt);

  CAnimationToolbar* GetAnimationToolbar() {return m_animationToolbar;};

  static void EvtRadioButton(wxWindow& window, const wxCommandEventFunction& method, wxObject* userData  = NULL, wxEvtHandler* eventSink = NULL);
  static void EvtCheckBox(wxWindow& window, const wxCommandEventFunction& method, wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
  static void EvtSize(wxWindow& window, const wxSizeEventFunction& method, wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);

  // WDR: method declarations for CXYPlotPanel
    
public:
  bool m_finished;
  bool m_multiFrame;

private:
  // WDR: member variable declarations for CXYPlotPanel
  wxSplitterWindow* m_splitPanel;

  wxWindow* m_propertyPanel;
  wxNotebook* m_propertyNotebook;

  //wxNotebookSizer* m_propertyNotebookSizer;
  wxBoxSizer* m_propertyPanelSizer;
  wxBoxSizer* m_splitPanelSizer;
  wxBoxSizer* m_vSizer;

  CXYPlotPropertyPanel* m_plotPropertyTab;

  CDatasetPanel* m_datasetPanelTab;

  vtkXYPlotActor* m_plotActor;
  vtkRenderer* m_vtkRend;
  vtkInteractorStyleXYPlot* m_styleXY;

  wxVTKRenderWindowInteractor* m_vtkWidget;

  CXYPlotDataCollection* m_plotDataCollection;

  CVtkXYPlotCharEventCallback* m_vtkCharEventCallback;

  double m_currentXMin;
  double m_currentXMax;
  double m_currentYMin;
  double m_currentYMax;

  CAnimationToolbar* m_animationToolbar;
    
private:
  void Init();

  bool CreateControls();
  void CreatePropertyPanel();
  void CreateLayout();
  void InstallEventListeners();

  void AdjustSplitPanelSashPosition();

  static void SetFocusVtkWidget(vtkObject* obj, unsigned long, void*, void*);

  void OnVtkWidgetSize(wxSizeEvent& event);

  // WDR: handler declarations for CXYPlotPanel
  void OnSplitPanelSize(wxSizeEvent& event);
  void OnPropertyPanelSize(wxSizeEvent& event);
  void OnDataSetTabSize(wxSizeEvent& event);
  void OnPlotPropertyTabSize(wxSizeEvent& event);
  void OnDatasetSelected(CDatasetSelectedEvent& event);
  void OnKeyframeChanged(CKeyframeEvent& event);
  void OnAutoZoomDataRange(CAutoZoomDataRangeEvent& event);

private:
    DECLARE_EVENT_TABLE()
};


//-------------------------------------------------------------
//------------------- CVtkXYPlotCharEventCallback class --------------------
//-------------------------------------------------------------
 
class CVtkXYPlotCharEventCallback : public vtkCommand
{
protected:
  CVtkXYPlotCharEventCallback(CXYPlotPanel *xyplotPanel)
    {
      m_xyplotPanel = xyplotPanel;
    }

public:  
  static CVtkXYPlotCharEventCallback *New(CXYPlotPanel *xyplotPanel) 
    { return new CVtkXYPlotCharEventCallback(xyplotPanel); }
  
  virtual void Execute(vtkObject *caller, unsigned long, void*)
    {
      //vtkGenericRenderWindowInteractor *Interactor = vtkGenericRenderWindowInteractor::SafeDownCast(caller);
      wxVTKRenderWindowInteractor *Interactor = dynamic_cast<wxVTKRenderWindowInteractor*>(caller);
      if (Interactor == NULL)
      {
        return;
      }
/*
      if (m_xyplotPanel->GetVtkWidget() == NULL)
      {
        return;
      }
*/
//      if (wxWindow::FindFocus()->GetId() == m_xyplotPanel->GetVtkWidget()->GetId())
      if (wxWindow::FindFocus()->GetId() == Interactor->GetId())
      {
        int c = Interactor->GetKeyCode();
        if (c == ' ')
        {
          ////???self.animationtoolbar.OnAnimate(None);
        }
        else if (c == 'r')
        {
          if (m_xyplotPanel->GetPlotPropertyTab() != NULL)
          {
            m_xyplotPanel->GetPlotPropertyTab()->UpdateValues();
          }
        }
      }
    }
protected:

  CXYPlotPanel* m_xyplotPanel;
};



#endif
