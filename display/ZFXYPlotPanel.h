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

#ifndef __ZFXYPlotPanel_H__
#define __ZFXYPlotPanel_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "ZFXYPlotPanel.h"
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "vtkTransformCollection.h"
#include "vtkTextProperty.h"
#include "vtkTextActor.h"
#include "vtkTextMapper.h"
#include "vtkRenderer.h"
#include "vtkScalarBarActor.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkRenderWindow.h"
#include "vtkGenericRenderWindowInteractor.h"
#include "vtkCommand.h"
#include "vtkPointPicker.h"
#include "vtkCellPicker.h"
#include "vtkPoints.h"
#include "vtkOutputWindow.h"
#include "vtkCameraState.h"


#include "wxVTKRenderWindowInteractor.h"

#include "vtkInteractorStyleZFXYPlot.h"

#include "PlotData/vtkList.h"

#include "Tools.h"
#include "List.h"
using namespace brathl;

#include "wx/utils.h"
#include "wx/splitter.h"
#include "wx/panel.h"
#include "BratDisplay_wdr.h"


#include "vtkZFXYPlotActor.h"
#include "vtkRenderer.h"
#include "vtkInteractorStyleZFXYPlot.h"
#include "vtkPolyDataMapper2D.h"

#include "AnimationToolbar.h"
#include "ZFXYPlotData.h"
#include "ZFXYPlotPropertyPanel.h"
#include "LUTPanel.h"
#include "ZFXYContourPropFrame.h"


class CZFXYVtkCharEventCallback;


extern long ID_ZFXYPLOTPANEL_VTK_INTERACTOR;
extern long ID_ZFXYPLOTPANEL_SPLIT;
extern long ID_ZFXYPLOTPANEL_PROPPANEL;
extern long ID_ZFXYPLOTPANEL_PROPTAB;
extern long ID_ZFXYPLOTPANEL_NOTEBOOK;

//class CZFXYPlotRenderer;
class CLUTZFXYRenderer;

// WDR: class declarations

//----------------------------------------------------------------------------
//-------------------------------------------------------------
//------------------- CZFXYPlotPanel class --------------------
//-------------------------------------------------------------

class CZFXYPlotPanel: public wxPanel
{
public:
  // constructors and destructors
  CZFXYPlotPanel();
  CZFXYPlotPanel(wxWindow *parent, wxWindowID id = -1,
                  CZFXYPlotProperty* plotProperty = NULL,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = wxTAB_TRAVERSAL | wxNO_BORDER );

  virtual ~CZFXYPlotPanel();
  

  // WDR: method declarations for CZFXYPlotPropertyPanel
//  CLabeledTextCtrl* GetZfxyrangeymax()  { return (CLabeledTextCtrl*) FindWindow( ID_ZFXYRANGEYMAX ); }
//  CLabeledTextCtrl* GetZfxyrangeymin()  { return (CLabeledTextCtrl*) FindWindow( ID_ZFXYRANGEYMIN ); }
//  CLabeledTextCtrl* GetZfxynumticksy()  { return (CLabeledTextCtrl*) FindWindow( ID_ZFXYNUMTICKSY ); }
//  wxTextCtrl* GetZfxytitley()  { return (wxTextCtrl*) FindWindow( ID_ZFXYTITLEY ); }
//  wxNotebook* GetZfxyaxes()  { return (wxNotebook*) FindWindow( ID_ZFXYAXES ); }
//  CLabeledTextCtrl* GetZfxyrangexmax()  { return (CLabeledTextCtrl*) FindWindow( ID_ZFXYRANGEXMAX ); }
//  CLabeledTextCtrl* GetZfxyrangexmin()  { return (CLabeledTextCtrl*) FindWindow( ID_ZFXYRANGEXMIN ); }
//  CLabeledTextCtrl* GetZfxynumticksx()  { return (CLabeledTextCtrl*) FindWindow( ID_ZFXYNUMTICKSX ); }
//  wxTextCtrl* GetZfxytitlex()  { return (wxTextCtrl*) FindWindow( ID_ZFXYTITLEX ); }
//  CLabeledTextCtrl* GetZfxyrangemax()  { return (CLabeledTextCtrl*) FindWindow( ID_ZFXYRANGEMAX ); }
//  CLabeledTextCtrl* GetZfxyrangemin()  { return (CLabeledTextCtrl*) FindWindow( ID_ZFXYRANGEMIN ); }
//  CLabeledTextCtrl* GetZfxynumlabels()  { return (CLabeledTextCtrl*) FindWindow( ID_ZFXYNUMLABELS ); }
//  wxCheckBox* GetZfxyshowcontour()  { return (wxCheckBox*) FindWindow( ID_ZFXYSHOWCONTOUR ); }
//  wxCheckBox* GetZfxyshowsolidcolor()  { return (wxCheckBox*) FindWindow( ID_ZFXYSHOWSOLIDCOLOR ); }
  wxChoice* GetZfxylayerchoice()  { return (wxChoice*) FindWindow( ID_ZFXYLAYERCHOICE ); }
//  CLabeledTextCtrl* GetZfxyplotpropTitle()  { return (CLabeledTextCtrl*) FindWindow( ID_ZFXYPLOTPROP_TITLE ); }



  bool Create(wxWindow *parent, wxWindowID id = -1,
              CZFXYPlotProperty* plotProperty = NULL,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxTAB_TRAVERSAL | wxNO_BORDER );

  virtual bool Destroy();

  CZFXYPlotData* GetCurrentLayer();
  bool IsNumberOfMapsEquals(int32_t* numberOfMaps = NULL);


  wxVTKRenderWindowInteractor* GetVtkWidget() {return m_vtkWidget;};

  CZFXYPlotPropertyPanel* GetPlotPropertyTab() {return m_plotPropertyTab;};

  CBratLookupTable* GetLUT() {return GetCurrentLayer()->GetLUT();};

  vtkLookupTable* GetLookupTable() 
  { 
    if (GetLUT() == NULL)
    {
      return NULL;
    }
    return GetLUT()->GetLookupTable();
  };


  //CZFXYPlotRenderer* GetZFXYPlotRenderer() {return m_plotRenderer;};

  void Update2D();
  
//  bool Is3D() {return m_is3D;};
//  bool Is2D() {return !m_is3D;};


  void ShowAnimationToolbar(bool showIt);
  void ShowColorBar(bool showIt);
  void ShowPropertyPanel(bool showIt);

  //void SetCenterLatitude(double c);
  //void SetCenterLongitude(double c);

  void AddData(CZFXYPlotData* pdata, wxProgressDialog* dlg = NULL);

  void LutChanged( CLutChangedEvent &event );
  void OnContourPropChanged( CZFXYContourPropChangedEvent &event );

  static void EvtSize(wxWindow& window, const wxSizeEventFunction& method, wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
  
  void UpdateValues();

  //void RestoreState();
  //void RestoreState(int32_t index);
  //void SaveState();
  //void ClearStates();

//  void UpdateViewStateCtrl();

  CAnimationToolbar* GetAnimationToolbar() {return m_animationToolbar;};

  //void SetViewParams();

  //void SetProjection();
  
  void DisplayContour(bool show);
  void DisplaySolidColor(bool show);

  vtkZFXYPlotActor* GetPlotActor() {return m_plotActor;};

  void UpdateColorBarRender(bool show);
  int32_t CountColorBarRender();

  void UpdateRender();


  //void PrintPoints(std::ostream& os);


  // WDR: method declarations for CZFXYPlotPanel

private:
  
  void Init();

  bool CreateControls();
  void CreatePropertyPanel();
  void CreateLayout();
  void InstallEventListeners();

  //void SetProjection(int32_t proj);
  //void SetProjectionByName(const std::string& projName);


  //void ZoomTo(double lon[2], double lat[2]);
  //void ZoomTo(double lat1, double lon1,  double lat2, double lon2);


  void AdjustSplitPanelSashPosition();

  static void SetFocusVtkWidget(vtkObject* obj, unsigned long, void*, void*);

  void OnVtkWidgetSize(wxSizeEvent& event);


  //void OnSetPreferredRatio( wxSizeEvent &event );
  //void OnSetPreferredRatio2(wxSizeEvent *event = NULL);

  void OnRangeChanged(CZFXYRangeChangedEvent& event);
  void RangeChanged(CZFXYRangeChangedEvent& event);
  void OnNumlabelsChanged(CZFXYNumLabelsChangedEvent& event);
  void NumlabelsChanged(CZFXYNumLabelsChangedEvent& event);
  void OnSplitPanelSize(wxSizeEvent& event);
  void OnPropertyPanelSize(wxSizeEvent& event);
  void OnPlotPropertyTabSize(wxSizeEvent& event);
  void OnKeyframeChanged(CKeyframeEvent& event);
  void OnUpdateRange(CZFXYUpdateRangeEvent& event);
  void OnLutFrameClose( CLutFrameCloseEvent &event );
  void OnContourPropFrameClose( CZFXYContourPropFrameCloseEvent &event );
  void OnAutoZoomDataRange(CAutoZoomDataRangeEvent& event);

  // WDR: handler declarations for CZFXYPlotPanel


public:
  bool m_finished;
  //std::string m_hasProj;
  bool m_colorBarShowing;
  //bool m_is3D;

  CObList m_zfxyPlotData;

  CZFXYPlotProperty m_plotProperty;

  static double m_percentSizeScalarBar;

  //vtkCellPicker* m_picker;

private:

  //void ZoomTo2D(double lon[2], double lat[2]);
  //void ZoomTo2D(double lat1, double lon1, double lat2, double lon2);
/*
  void SaveInitialState();
  void SaveInitialState2D();
  void SaveInitialState3D();
*/
  // WDR: member variable declarations for CZFXYPlotPanel
  wxSplitterWindow* m_splitPanel;

  wxWindow* m_propertyPanel;
  wxNotebook* m_propertyNotebook;

  wxBoxSizer* m_propertyPanelSizer;
  wxBoxSizer* m_splitPanelSizer;
  wxBoxSizer* m_vSizer;

  CZFXYPlotPropertyPanel* m_plotPropertyTab;

  //CZFXYPlotRenderer* m_plotRenderer;

  CZFXYPlotData* m_plotData;

  //vtkInteractorStyleZFXYPlot* m_style2D;
  //vtkInteractorStyle* m_style2D;
  //vtkInteractorStyleTrackballCamera* m_style3D;

  wxVTKRenderWindowInteractor* m_vtkWidget;


  vtkZFXYPlotActor* m_plotActor;
  vtkRenderer* m_vtkRend;
  vtkInteractorStyleZFXYPlot* m_styleZFXY;
  vtkPolyDataMapper2D *m_polyDataMapper;

  CZFXYVtkCharEventCallback* m_vtkCharEventCallback;

  CAnimationToolbar* m_animationToolbar;


private:
    DECLARE_EVENT_TABLE()
};

/*
//-------------------------------------------------------------
//------------------- vtkCameraState class --------------------
//-------------------------------------------------------------
class VTK_EXPORT vtkCameraState : public vtkObject
{
public:
  static vtkCameraState *New();
  vtkTypeMacro(vtkCameraState, vtkObject);
  void PrintSelf(std::ostream& os, vtkIndent indent);

  vtkSetVector3Macro(ViewUp,double);
  vtkGetVector3Macro(ViewUp,double);

  vtkSetVector3Macro(FocalPoint,double);
  vtkGetVector3Macro(FocalPoint,double);

  vtkSetVector3Macro(Position,double);
  vtkGetVector3Macro(Position,double);

  vtkSetVector2Macro(ClippingRange, double);
  vtkGetVector2Macro(ClippingRange, double);

  vtkSetMacro(ViewAngle, double);
  vtkGetMacro(ViewAngle, double);

  vtkSetMacro(ParallelScale, double);
  vtkGetMacro(ParallelScale, double);

protected:

  vtkCameraState();
  virtual ~vtkCameraState();

protected:
  double ViewUp[3];
  double FocalPoint[3];
  double Position[3];
  double ClippingRange[2];
  double ViewAngle;
  double ParallelScale;

};

*/


//-------------------------------------------------------------
//------------------- CZFXYVtkCharEventCallback class --------------------
//-------------------------------------------------------------
 
class CZFXYVtkCharEventCallback : public vtkCommand
{
protected:
  CZFXYVtkCharEventCallback(CZFXYPlotPanel *zfxyPlotPanel)
    {
      m_zfxyPlotPanel = zfxyPlotPanel;
    }

public:  
  static CZFXYVtkCharEventCallback *New(CZFXYPlotPanel *zfxyPlotPanel) 
    { return new CZFXYVtkCharEventCallback(zfxyPlotPanel); }
  
  virtual void Execute(vtkObject *caller, unsigned long, void*)
    {
      //vtkGenericRenderWindowInteractor *Interactor = vtkGenericRenderWindowInteractor::SafeDownCast(caller);
      wxVTKRenderWindowInteractor *vtkWidget = dynamic_cast<wxVTKRenderWindowInteractor*>(caller);
      if (vtkWidget == NULL)
      {
        return;
      }

/*      if (m_zfxyPlotPanel->GetVtkWidget() == NULL)
      {
        return;
      }
*/
//      if (wxWindow::FindFocus()->GetId() == m_ZFXYPLOTPANEL->GetVtkWidget()->GetId())
      if (wxWindow::FindFocus()->GetId() == vtkWidget->GetId())
      {
        int c = vtkWidget->GetKeyCode();
        if (c == ' ')
        {
          m_zfxyPlotPanel->GetAnimationToolbar()->Animate();
        }
        else if (c == 'z')
        {
          if (m_zfxyPlotPanel != NULL)
          {
            //m_ZFXYPLOTPANEL->RestoreState();             
            //m_ZFXYPLOTPANEL->UpdateValues();
          }
        }
        else if (c == 's')
        {
          if (m_zfxyPlotPanel != NULL)
          {
            //m_ZFXYPLOTPANEL->SaveState();
          }
        }
      }
    }
protected:
  CZFXYPlotPanel* m_zfxyPlotPanel;
};


//-------------------------------------------------------------
//------------------- CZFXYVtkRenderEventCallback class --------------------
//-------------------------------------------------------------
 
class CZFXYVtkRenderEventCallback : public vtkCommand
{
protected:
  CZFXYVtkRenderEventCallback(CZFXYPlotPanel *zfxyPlotPanel)
    {
      m_zfxyPlotPanel = zfxyPlotPanel;
    }

public:  
  static CZFXYVtkRenderEventCallback *New(CZFXYPlotPanel *zfxyPlotPanel) 
    { return new CZFXYVtkRenderEventCallback(zfxyPlotPanel); }
  
  virtual void Execute(vtkObject *caller, unsigned long, void*)
    {
      //vtkGenericRenderWindowInteractor *Interactor = vtkGenericRenderWindowInteractor::SafeDownCast(caller);
      wxVTKRenderWindowInteractor *vtkWidget = dynamic_cast<wxVTKRenderWindowInteractor*>(caller);
      if (vtkWidget == NULL)
      {
        return;
      }

      m_zfxyPlotPanel->Update2D();
    }
protected:
  CZFXYPlotPanel* m_zfxyPlotPanel;
};
//-------------------------------------------------------------
//------------------- CZFXYVtkUpdate2DCallback class --------------------
//-------------------------------------------------------------
 
class CZFXYVtkUpdate2DCallback : public vtkCommand
{
protected:
  CZFXYVtkUpdate2DCallback(CZFXYPlotPanel *zfxyPlotPanel)
    {
      m_zfxyPlotPanel = zfxyPlotPanel;
    }

public:  
  static CZFXYVtkUpdate2DCallback *New(CZFXYPlotPanel *zfxyPlotPanel) 
    { return new CZFXYVtkUpdate2DCallback(zfxyPlotPanel); }
  
  virtual void Execute(vtkObject *caller, unsigned long, void*)
    {
      m_zfxyPlotPanel->Update2D();
    }
protected:
  CZFXYPlotPanel* m_zfxyPlotPanel;
};


#endif
