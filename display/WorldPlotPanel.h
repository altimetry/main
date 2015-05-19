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

#ifndef __WorldPlotPanel_H__
#define __WorldPlotPanel_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "WorldPlotPanel.h"
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
#include "vtkIdFilter.h"
#include "vtkCellCenters.h"
#include "vtkCell.h"
#include "vtkCellArray.h"
#include "vtkOutlineSource.h"
#include "vtkRendererSource.h"


#include "wxVTKRenderWindowInteractor.h"


#include "vtkInteractorStyleWPlot.h"
#include "vtkInteractorStyle3DWPlot.h"

#include "vtkList.h"

#include "Tools.h"
#include "List.h"
using namespace brathl;

#include "wx/utils.h"
#include "wx/splitter.h"
#include "wx/panel.h"
#include "BratDisplay_wdr.h"

#include "AnimationToolbar.h"
#include "WorldPlotData.h"
#include "WPlotPropertyPanel.h"
#include "LUTPanel.h"
#include "ContourPropFrame.h"

#include <set>
#include "proj_api.h"

class CVtkCharEventCallback;


extern long ID_WPLOTPANEL_VTK_INTERACTOR;
extern long ID_WPLOTPANEL_SPLIT;
extern long ID_WPLOTPANEL_PROPPANEL;
extern long ID_WPLOTPANEL_PROPTAB;
extern long ID_WPLOTPANEL_NOTEBOOK;

class CWorldPlotRenderer;
class CLUTRenderer;

// WDR: class declarations

//----------------------------------------------------------------------------
//-------------------------------------------------------------
//------------------- CWorldPlotPanel class --------------------
//-------------------------------------------------------------

class CWorldPlotPanel: public wxPanel
{
public:
  // constructors and destructors
  CWorldPlotPanel();
  CWorldPlotPanel(wxWindow *parent, wxWindowID id = -1,
                  CWorldPlotProperty* plotProperty = NULL,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = wxTAB_TRAVERSAL | wxNO_BORDER );

  virtual ~CWorldPlotPanel();

  bool Create(wxWindow *parent, wxWindowID id = -1,
              CWorldPlotProperty* plotProperty = NULL,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxTAB_TRAVERSAL | wxNO_BORDER );

  virtual bool Destroy();

  CGeoMap* GetCurrentLayer();
  bool IsNumberOfMapsEquals(int32_t* numberOfMaps = NULL);


  wxVTKRenderWindowInteractor* GetVtkWidget() {return m_vtkWidget;};

  CWPlotPropertyPanel* GetPlotPropertyTab() {return m_plotPropertyTab;};

  CBratLookupTable* GetLUT() {return GetCurrentLayer()->GetLUT();};

  vtkLookupTable* GetLookupTable()
  {
    if (GetLUT() == NULL)
    {
      return NULL;
    }
    return GetLUT()->GetLookupTable();
  };


  CWorldPlotRenderer* GetWorldPlotRenderer() {return m_plotRenderer;};

  void Update2D();

  bool Is3D() {return m_is3D;};
  bool Is2D() {return !m_is3D;};


  void ShowAnimationToolbar(bool showIt);
  void ShowColorBar(bool showIt);
  void ShowPropertyPanel(bool showIt);

  void SetCenterLatitude(double c);
  void SetCenterLongitude(double c);

  void AddData(CWorldPlotData* pdata, wxProgressDialog* dlg = NULL);

  void LutChanged( CLutChangedEvent &event );
  void OnContourPropChanged( CContourPropChangedEvent &event );

  static void EvtSize(wxWindow& window, const wxSizeEventFunction& method, wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);

  void UpdateValues();
  void UpdateFactor(int32_t factorPercent);
  void UpdateRadius(int32_t radiusPercent);
  void UpdateFactor(double factor);
  void UpdateRadius(double radius);

  void RestoreState();
  void RestoreState(int32_t index);
  void SaveState();
  void ClearStates();

  void UpdateViewStateCtrl();

  CAnimationToolbar* GetAnimationToolbar() {return m_animationToolbar;};

  void SetViewParams();

  void SetProjection();

  void DisplayContour(bool show);
  void DisplaySolidColor(bool show);
  void DisplayGridLabels(bool show);

  void PrintPoints(ostream& os);

  void ZoomInit(double scale);


  // WDR: method declarations for CWorldPlotPanel

private:

  void Init();

  bool CreateControls();
  void CreatePropertyPanel();
  void CreateLayout();
  void InstallEventListeners();

  void SetProjection(int32_t proj);
  void SetProjectionByName(const string& projName);


  void ZoomTo(double lon[2], double lat[2]);
  void ZoomTo(double lat1, double lon1,  double lat2, double lon2);


  void AdjustSplitPanelSashPosition();

  static void SetFocusVtkWidget(vtkObject* obj, unsigned long, void*, void*);

  void UpdateRender();

  void OnSetPreferredRatio( wxSizeEvent &event );
  void OnSetPreferredRatio2(wxSizeEvent *event = NULL);

  void OnProjectionChanged(CProjectionChangedEvent& event);
  void OnCenterPointChanged(CCenterPointChangedEvent& event);
  void OnGridLabelsChanged(CGridLabelsChangedEvent& event);
  void OnRangeChanged(CRangeChangedEvent& event);
  void RangeChanged(CRangeChangedEvent& event);
  void OnNumlabelsChanged(CNumLabelsChangedEvent& event);
  void NumlabelsChanged(CNumLabelsChangedEvent& event);
  void OnSplitPanelSize(wxSizeEvent& event);
  void OnPropertyPanelSize(wxSizeEvent& event);
  void OnPlotPropertyTabSize(wxSizeEvent& event);
  void OnKeyframeChanged(CKeyframeEvent& event);
  void OnViewChanged(CViewStateChangedEvent& event);
  void OnViewCommand(CViewStateCommandEvent& event);
  void OnUpdateRange(CUpdateRangeEvent& event);
  void OnZoomChanged(CZoomChangedEvent& event);
  void OnRadiusChanged(CRadiusChangedEvent& event);
  void OnFactorChanged(CFactorChangedEvent& event);
  void OnLutFrameClose( CLutFrameCloseEvent &event );
  void OnDisplayDataAttrChanged(CDisplayDataAttrChangedEvent& event);
  void OnDisplayVectorScale(CVectorScaleChangedEvent& event);
  void OnContourPropFrameClose( CContourPropFrameCloseEvent &event );

  // WDR: handler declarations for CWorldPlotPanel


public:
  bool m_finished;
  //string m_hasProj;
  bool m_colorBarShowing;
  bool m_is3D;

  CWorldPlotProperty m_plotProperty;

  static double m_percentSizeScalarBar;

  //vtkCellPicker* m_picker;

private:

  void ZoomTo2D(double lon[2], double lat[2]);
  void ZoomTo2D(double lat1, double lon1, double lat2, double lon2);

  void SaveInitialState();
  void SaveInitialState2D();
  void SaveInitialState3D();

  // WDR: member variable declarations for CWorldPlotPanel
  wxSplitterWindow* m_splitPanel;

  wxWindow* m_propertyPanel;
  wxNotebook* m_propertyNotebook;

  wxBoxSizer* m_propertyPanelSizer;
  wxBoxSizer* m_splitPanelSizer;
  wxBoxSizer* m_vSizer;

  CWPlotPropertyPanel* m_plotPropertyTab;

  CWorldPlotRenderer* m_plotRenderer;

  vtkInteractorStyleWPlot* m_style2D;
  // a more flexible interactor from VISAN, that helps us preserve center and zoom
  vtkInteractorStyle3DWPlot* m_style3D;

  // Projection center
  // KAVOK: to keep track of this stuff?
  double projCenterLatitude;
  double projCenterLongitude;
  double viewCenterLatitude;
  double viewCenterLongitude;


  wxVTKRenderWindowInteractor* m_vtkWidget;

  CWorldPlotData* m_plotData;

  CVtkCharEventCallback* m_vtkCharEventCallback;

  CAnimationToolbar* m_animationToolbar;


private:
    DECLARE_EVENT_TABLE()
};

//-------------------------------------------------------------
//------------------- Lat/Lon Labels class --------------------
//-------------------------------------------------------------

// this structure and the == operator are just
// to ease label positioning
struct LabelPoint {
        double x, y, z;
        GeoGridLineInfo gInfo;
};

bool operator==(LabelPoint, LabelPoint);
bool operator<(LabelPoint, LabelPoint);
set<LabelPoint>::iterator find_label(set<LabelPoint> &, double, int);
set<LabelPoint>::iterator find_near(set<LabelPoint> &labels, LabelPoint label, double distance);


class LatLonLabels {

public:

    LatLonLabels ( CGeoGrid *geogrid, vtkRenderer *renderer );
    virtual ~LatLonLabels (  );
    void Update2D(double, double, double);
    void Update3D(double, double, double);
    vtkActor2DCollection *GetTextLabels();
    void SetLabelFormat(char *);
    void SetLabelSize(int);
    void AddToRenderer();
    void RemoveFromRenderer();

protected:

    void DeleteAllLabels();

    CGeoGrid *m_geoGrid;
    vtkActor2DCollection *m_labelCollection;

    char *m_format;
    int m_fontSize;
    vtkRenderer *m_renderer;

};



//-------------------------------------------------------------
//------------------- CWorldPlotRenderer class --------------------
//-------------------------------------------------------------

class CWorldPlotRenderer
{

public:

  CWorldPlotRenderer(wxWindow *parent, string mode = "low");
  virtual ~CWorldPlotRenderer();

  vtkRenderer* GetVtkRenderer() {return m_vtkRend;};

  vtkTransformCollection* GetVtkTransformCollection(){return m_transformations;};
  CGeoGrid* GetGeoGrid() { return m_geoGrid; };
  CGeoMap* GetGeoMap();

  void UpdateColorBarRender(bool show);
  int32_t CountColorBarRender();

  void Update2D();

  CObList* GetActors() { return &m_actors; };

  bool GetCoords2D(double lon[2], double lat[2], int& x1, int& y1, int& x2, int& y2);

  bool IsNumberOfMapsEquals(int32_t* numberOfMaps = NULL);

  void SetTextActor();
  void ResetTextActor();

  void AddData(CWorldPlotData* pdata);

  void SetCenterLatitude(double c);
  double GetCenterLatitude();
  void SetCenterLongitude(double c);
  double GetCenterLongitude();

  void UpdateLatLonLabels(double, double, double, bool);
  void UpdateLOD(double, double, double, double, double, double, double);

  void SetProjectionByName(const string& projName);
  void SetProjection(int32_t proj);
  void SetGlyphs(bool val);
  void OnKeyframeChanged(CKeyframeEvent& event);

  int32_t GetProjection() {return m_projection;};

  void SaveState();
  void RestoreState();
  void RestoreState(int32_t index);
  void ClearStates();
  bool HasState() {return CountState() > 0;};
  uint32_t CountState() {return m_states.size();};

  void SetVtkWidget(wxVTKRenderWindowInteractor* vtkWidget) {m_vtkWidget = vtkWidget;};

  void AddActorsToRenderer();
  void AddActorsBackgroundToRenderer();
  void AddActorsContourToRenderer();
  void AddActorsSolidColorToRenderer();
  void RemoveActorsFromRenderer();

  void AddActors2DToRenderer();
  void AddActorsBackground2DToRenderer();
  void AddActorsContour2DToRenderer();
  void AddActorsSolidColor2DToRenderer();
  void RemoveActors2DFromRenderer();

  void AddActors(CWorldPlotData* pdata);
  void AddActorsContour(CWorldPlotData* pdata);
  void AddActorsSolidColor(CWorldPlotData* pdata);

  void AddActors2D(CWorldPlotData* pdata);
  void AddActors2DContour(CWorldPlotData* pdata);
  void AddActors2DSolidColor(CWorldPlotData* pdata);

  void RemoveActors(CWorldPlotData* pdata);
  void RemoveActorsSolidColor(CWorldPlotData* pdata);
  void RemoveActorsContour(CWorldPlotData* pdata);

  void RemoveActors2D(CWorldPlotData* pdata);
  void RemoveActors2DSolidColor(CWorldPlotData* pdata);
  void RemoveActors2DContour(CWorldPlotData* pdata);

  void AddActorsContourLabel(CWorldPlotData* pdata);
  void RemoveActorsContourLabel(CWorldPlotData* pdata);

  void ResetActors();

  void AddTextActors();

  void RenderOverlay();

  // void OnContourPropChanged( CContourPropChangedEvent &event );


protected:

  void Init();


  void DeleteTextActor();

  vtkCameraState* GetState(int32_t index);

protected:
  CObList m_actors;

  //bool m_actorAdded;

  wxWindow* m_parent;

  string m_GSHHSFile;

  int32_t m_projection;

  bool m_cameraResetted;

  vtkRenderer* m_vtkRend;
  vtkTransformCollection* m_transformations;
  //vtkTextActor* m_textActor;
  vtkActor2D* m_textActor;

  CGlobe* m_globe;
  CGeoGrid* m_geoGrid;
  CCoastLine* m_coastLine;

  double m_cLong;
  double m_cLati;

  vtkObArray m_states;
  int32_t m_currentState;

  wxVTKRenderWindowInteractor* m_vtkWidget;

  //KAVOK
  vtkActor2DCollection *m_textActors;
  vtkActor2D *lActor;
  CWorldPlotData* m_wData;

  LatLonLabels *m_latlonLabels;


};



//-------------------------------------------------------------
//------------------- CVtkCharEventCallback class --------------------
//-------------------------------------------------------------

class CVtkCharEventCallback : public vtkCommand
{
protected:
  CVtkCharEventCallback(CWorldPlotPanel *wplotPanel)
    {
      m_wplotPanel = wplotPanel;
    }

public:
  static CVtkCharEventCallback *New(CWorldPlotPanel *wplotPanel)
    { return new CVtkCharEventCallback(wplotPanel); }

  virtual void Execute(vtkObject *caller, unsigned long, void*)
    {
      //vtkGenericRenderWindowInteractor *Interactor = vtkGenericRenderWindowInteractor::SafeDownCast(caller);
      wxVTKRenderWindowInteractor *vtkWidget = dynamic_cast<wxVTKRenderWindowInteractor*>(caller);
      if (vtkWidget == NULL)
      {
        return;
      }

      if (wxWindow::FindFocus()->GetId() == vtkWidget->GetId())
      {
        int c = vtkWidget->GetKeyCode();
        if (c == ' ')
        {
          m_wplotPanel->GetAnimationToolbar()->Animate();
        }
        else if (c == 'z')
        {
          if (m_wplotPanel != NULL)
          {
            //m_wplotPanel->RestoreState();
            //m_wplotPanel->UpdateValues();
          }
        }
        else if (c == 's')
        {
          if (m_wplotPanel != NULL)
          {
            //m_wplotPanel->SaveState();
          }
        }
      }
    }
protected:
  CWorldPlotPanel* m_wplotPanel;
};


//-------------------------------------------------------------
//------------------- CVtkRenderEventCallback class --------------------
//-------------------------------------------------------------

class CVtkRenderEventCallback : public vtkCommand
{
protected:
  CVtkRenderEventCallback(CWorldPlotPanel *wplotPanel)
    {
      m_wplotPanel = wplotPanel;
    }

public:
  static CVtkRenderEventCallback *New(CWorldPlotPanel *wplotPanel)
    { return new CVtkRenderEventCallback(wplotPanel); }

  virtual void Execute(vtkObject *caller, unsigned long, void*)
    {
      //vtkGenericRenderWindowInteractor *Interactor = vtkGenericRenderWindowInteractor::SafeDownCast(caller);
      wxVTKRenderWindowInteractor *vtkWidget = dynamic_cast<wxVTKRenderWindowInteractor*>(caller);
      if (vtkWidget == NULL)
      {
        return;
      }

      m_wplotPanel->Update2D();
    }
protected:
  CWorldPlotPanel* m_wplotPanel;
};
//-------------------------------------------------------------
//------------------- CVtkUpdate2DCallback class --------------------
//-------------------------------------------------------------

class CVtkUpdate2DCallback : public vtkCommand
{
protected:
  CVtkUpdate2DCallback(CWorldPlotPanel *wplotPanel, vtkInteractorStyleWPlot *style2D,
                        vtkInteractorStyle3DWPlot *style3D)
    {
      m_wplotPanel = wplotPanel;
      m_style2D = style2D;
      m_style3D = style3D;
      zoomLog = 0;
    }

public:
  static CVtkUpdate2DCallback *New(CWorldPlotPanel *wplotPanel, vtkInteractorStyleWPlot *style2D,
                                    vtkInteractorStyle3DWPlot *style3D)
    { return new CVtkUpdate2DCallback(wplotPanel, style2D, style3D); }


  double GetLongExtent(double &minLong, double &maxLong);
  double GetLatExtent(double &minLong, double &maxLong);
  virtual void Execute(vtkObject *caller, unsigned long, void*);

protected:
  CWorldPlotPanel* m_wplotPanel;
  vtkInteractorStyleWPlot *m_style2D;
  vtkInteractorStyle3DWPlot *m_style3D;

  double midLat, midLong;
  double centerLong, centerLat;
  double zoomLog;
};

//-------------------------------------------------------------
//------------------- CVtkUpdateLatLonCallback --------------------
//-------------------------------------------------------------

class CVtkUpdateLatLonCallback : public vtkCommand
{
protected:
  CVtkUpdateLatLonCallback(CWorldPlotRenderer *wplotRenderer, vtkInteractorStyleWPlot *style2D,
                                 vtkInteractorStyle3DWPlot *style3D)
    {
      m_wplotRenderer = wplotRenderer;
      m_style2D = style2D;
      m_style3D = style3D;
    }

public:
  static CVtkUpdateLatLonCallback *New(CWorldPlotRenderer* wplotRenderer, vtkInteractorStyleWPlot *style2D,
                                                        vtkInteractorStyle3DWPlot *style3D)
    { return new CVtkUpdateLatLonCallback(wplotRenderer, style2D, style3D); }

  virtual void Execute(vtkObject *caller, unsigned long eventId, void* data)
    {
      // nothing

    }
protected:
  CWorldPlotRenderer* m_wplotRenderer;
  vtkInteractorStyleWPlot *m_style2D;
  vtkInteractorStyle3DWPlot *m_style3D;
};


#endif
