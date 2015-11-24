/////////////////////////////////////////////////////////////////////////////
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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "WorldPlotPanel.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <stdio.h>
#ifdef WIN32
#define snprintf _snprintf
#endif

#include "new-gui/Common/tools/Trace.h"
#include "new-gui/Common/tools/Exception.h"


#include "vtkActor2DCollection.h"
#include "vtkObjectFactory.h"
#include "vtkCamera.h"
#include "vtkCommand.h"
#include "vtkCellData.h"
#include "vtkPointData.h"
#include "vtkRendererCollection.h"
#include "vtkArrowSource.h"

#include "vtkCallbackCommand.h"
#include "vtkLookupTable.h"
#include "PlotData/vtkGeoMapFilter.h"

#include "WPlotPropertyPanel.h"
#include "WorldPlotPanel.h"

long ID_WPLOTPANEL_VTK_INTERACTOR;
long ID_WPLOTPANEL_SPLIT;
long ID_WPLOTPANEL_PROPPANEL;
long ID_WPLOTPANEL_PROPTAB;
long ID_WPLOTPANEL_NOTEBOOK;

double CWorldPlotPanel::m_percentSizeScalarBar = 0.1;

// WDR: class implementations

//----------------------------------------------------------------------------
// CWordlPlotPanel
//----------------------------------------------------------------------------

// WDR: event table for CWorldPlotPanel

BEGIN_EVENT_TABLE(CWorldPlotPanel,wxPanel)
END_EVENT_TABLE()

CWorldPlotPanel::CWorldPlotPanel()
{
  Init();
}

//----------------------------------------
CWorldPlotPanel::CWorldPlotPanel( wxWindow *parent, wxWindowID id, CWorldPlotProperty* plotProperty,
	const wxPoint &position, const wxSize& size, long style )
	//: wxPanel( parent, id, position, size, style ), --> called in Create
{
	Init();
	if ( !Create( parent, id, plotProperty, position, size, style ) )
	{
		CException e( "ERROR in CWorldPlotPanel ctor - Unable to create wxWidgets (Create method returns false)", BRATHL_SYSTEM_ERROR );
		throw( e );
	}
}
//----------------------------------------

CWorldPlotPanel::~CWorldPlotPanel()
{

  if (m_vtkWidget != NULL)
  {
    m_vtkWidget->Delete();
    m_vtkWidget = NULL;
  }


  //DeleteLUT();

  if (m_plotData != NULL)
  {
    delete m_plotData;
    m_plotData = NULL;
  }

  if (m_style3D != NULL)
  {
    m_style3D->Delete();
    m_style3D = NULL;
  }

  if (m_style2D != NULL)
  {
    m_style2D->Delete();
    m_style2D = NULL;
  }

  if (m_plotRenderer != NULL)
  {
    delete m_plotRenderer;
    m_plotRenderer = NULL;
  }

  if (m_vtkCharEventCallback != NULL)
  {
    m_vtkCharEventCallback->Delete();
    m_vtkCharEventCallback = NULL;
  }


}
//----------------------------------------
void CWorldPlotPanel::Init()
{

  m_splitPanel = NULL;
  m_propertyPanel = NULL;
  m_plotRenderer = NULL;
  m_vtkWidget = NULL;
  m_plotPropertyTab = NULL;
  m_propertyNotebook = NULL;
  m_propertyPanelSizer = NULL;
  m_splitPanelSizer = NULL;
  m_vSizer = NULL;

  m_animationToolbar = NULL;

  m_style2D = NULL;
  m_style3D = NULL;

  m_plotData = NULL;
  m_vtkCharEventCallback = CVtkCharEventCallback::New(this);

  //m_picker = vtkCellPicker::New();
  //m_picker->SetTolerance(0.001);

  m_finished = false;
  //m_hasProj = "";
  m_colorBarShowing = false;
  m_is3D = true;

}

//----------------------------------------
bool CWorldPlotPanel::Create( wxWindow *parent, wxWindowID id, CWorldPlotProperty* plotProperty,
	const wxPoint &position, const wxSize& size, long style )
{
	if ( plotProperty != NULL )
		m_plotProperty = *plotProperty;

	wxPanel::Create( parent, id, position, size, style );

	ID_WPLOTPANEL_VTK_INTERACTOR = ::wxNewId();
	ID_WPLOTPANEL_SPLIT = ::wxNewId();

	m_splitPanel = new wxSplitterWindow( this, ID_WPLOTPANEL_SPLIT, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE );
	m_splitPanel->SetMinimumPaneSize( 50 );

	m_plotRenderer = new CWorldPlotRenderer( this, m_plotProperty.m_coastResolution );
	m_plotRenderer->GetVtkRenderer()->InteractiveOn();


	m_style2D = vtkInteractorStyleWPlot::New();
	m_style2D->SetTransformCollection( m_plotRenderer->GetVtkTransformCollection() );

	// KAVOK: a more flexible interactor, from VISAN
	m_style3D = vtkInteractorStyle3DWPlot::New();

	//femm: This is a window as well as an inter-actor
	m_vtkWidget = new wxVTKRenderWindowInteractor( m_splitPanel, ID_WPLOTPANEL_VTK_INTERACTOR );
	m_vtkWidget->SetRenderWhenDisabled( 0 );
	m_vtkWidget->Disable();
	m_vtkWidget->UseCaptureMouseOn();
	m_vtkWidget->GetRenderWindow()->AddRenderer( m_plotRenderer->GetVtkRenderer() );
	m_vtkWidget->SetInteractorStyle( m_style2D );

	m_plotRenderer->SetVtkWidget( m_vtkWidget );

	CreatePropertyPanel();

	m_splitPanel->Initialize( m_vtkWidget );

	m_animationToolbar = new CAnimationToolbar( this );
	//if ( !CreateControls() )
	//	return false;

	CreateLayout();
	InstallEventListeners();

	return true;
}

//----------------------------------------
//bool CWorldPlotPanel::CreateControls()
//{
//
//  ID_WPLOTPANEL_VTK_INTERACTOR = ::wxNewId();
//  ID_WPLOTPANEL_SPLIT = ::wxNewId();
//
//  m_splitPanel = new wxSplitterWindow(this, ID_WPLOTPANEL_SPLIT, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE);
//  m_splitPanel->SetMinimumPaneSize(50);
//
//  m_plotRenderer = new CWorldPlotRenderer(this, m_plotProperty.m_coastResolution);
//  m_plotRenderer->GetVtkRenderer()->InteractiveOn();
//
//
//  m_style2D = vtkInteractorStyleWPlot::New();
//  m_style2D->SetTransformCollection(m_plotRenderer->GetVtkTransformCollection());
//
//  // KAVOK: a more felixible interactor, from VISAN
//  m_style3D = vtkInteractorStyle3DWPlot::New();
//
//  m_vtkWidget = new wxVTKRenderWindowInteractor(m_splitPanel, ID_WPLOTPANEL_VTK_INTERACTOR);
//  m_vtkWidget->SetRenderWhenDisabled(0);
//  m_vtkWidget->Disable();
//  m_vtkWidget->UseCaptureMouseOn();
//  m_vtkWidget->GetRenderWindow()->AddRenderer(m_plotRenderer->GetVtkRenderer());
//  m_vtkWidget->SetInteractorStyle(m_style2D);
//
//  m_plotRenderer->SetVtkWidget(m_vtkWidget);
//
//  CreatePropertyPanel();
//
//  m_splitPanel->Initialize(m_vtkWidget);
//
//  m_animationToolbar = new CAnimationToolbar(this);
//
//  return true;
//}

//----------------------------------------
void CWorldPlotPanel::CreatePropertyPanel()
{
  ID_WPLOTPANEL_PROPPANEL = ::wxNewId();
  ID_WPLOTPANEL_NOTEBOOK = ::wxNewId();

  // We create a parent wxWindow for the notebook to be able to give a
  // proper background colour for the area behind the tabs
  m_propertyPanel = new wxWindow(m_splitPanel, ID_WPLOTPANEL_PROPPANEL);
  // We must std::set the background 'explicitly' to have it stick
  ////m_propertyPanel->SetBackgroundColour(m_propertyPanel->GetBackgroundColour());
  m_propertyNotebook = new wxNotebook(m_propertyPanel, ID_WPLOTPANEL_NOTEBOOK);

  m_plotPropertyTab = new CWPlotPropertyPanel(m_propertyNotebook, NULL, m_plotData, m_vtkWidget, ID_WPLOTPANEL_PROPTAB);
  m_propertyNotebook->AddPage(m_plotPropertyTab, "Properties");
//  m_propertyPanel->Hide();

}

//----------------------------------------
void CWorldPlotPanel::CreateLayout()
{
  m_propertyPanelSizer = new wxBoxSizer(wxHORIZONTAL);
  m_propertyPanelSizer->Add(m_propertyNotebook, 1, wxEXPAND);

  m_propertyPanel->SetSizerAndFit(m_propertyPanelSizer);

  m_splitPanelSizer = new wxBoxSizer(wxHORIZONTAL);
  m_splitPanelSizer->Add(m_vtkWidget, 1, wxEXPAND);
  m_splitPanelSizer->Add(m_splitPanel->GetSashSize(), -1, 0, wxEXPAND);
  m_splitPanelSizer->Add(m_propertyPanel, 0, wxEXPAND);
  m_splitPanelSizer->Show(1, false);
  m_splitPanelSizer->Show(2, false);
  m_splitPanel->SetSizer(m_splitPanelSizer);

  m_vtkWidget->SetMinSize(wxSize(80,50));

  m_vSizer = new wxBoxSizer(wxVERTICAL);
  m_vSizer->Add(m_splitPanel, 1, wxEXPAND);
  m_vSizer->Add(m_animationToolbar, 0, wxEXPAND);

  //Initially, hide the animationtoolbar
  m_vSizer->Hide(m_animationToolbar);

  SetSizer(m_vSizer);


}
//----------------------------------------
bool CWorldPlotPanel::Destroy()
{
  m_finished = true;
  m_plotPropertyTab->m_finished = true;

  m_animationToolbar->Pause();

  return wxPanel::Destroy();
}

//----------------------------------------
void CWorldPlotPanel::ShowAnimationToolbar(bool showIt)
{
  m_vSizer->Show(m_animationToolbar, showIt);
  Layout();
}
//----------------------------------------
void CWorldPlotPanel::ShowPropertyPanel(bool showIt)
{
  if (showIt)
  {
    m_splitPanel->SplitVertically(m_vtkWidget, m_propertyPanel);
    m_splitPanelSizer->Show(1, true);
    m_splitPanelSizer->Show(2, true);
    m_propertyPanel->Show();
    m_propertyPanel->Fit();
  }
  else
  {
    m_splitPanel->Unsplit(m_propertyPanel);
    m_splitPanelSizer->Show(1, false);
    m_splitPanelSizer->Show(2, false);
  }

  Layout();
}
//----------------------------------------
void CWorldPlotPanel::ShowColorBar(bool showIt)
{
  m_colorBarShowing = showIt;
  OnSetPreferredRatio2();

}
//----------------------------------------
void CWorldPlotPanel::UpdateRender()
{
  m_plotRenderer->ResetTextActor();
  m_plotRenderer->UpdateColorBarRender(m_colorBarShowing);

  if (m_colorBarShowing)
  {
    double relLength = CWorldPlotPanel::m_percentSizeScalarBar * m_plotRenderer->CountColorBarRender();
    m_plotRenderer->GetVtkRenderer()->SetViewport(0, relLength, 1, 1);
  }
  else
  {
    m_plotRenderer->GetVtkRenderer()->SetViewport(0, 0, 1, 1);
  }

  double xm, ym, zoom;
   if ( Is2D() )
   {
      m_style2D->GetViewMidPoint(xm, ym);
      zoom = m_style2D->GetZoomScale();
   }
   else
   {
      ym = m_style3D->GetViewCenterLongitude();
      xm = m_style3D->GetViewCenterLatitude();
      zoom = m_style3D->GetZoom();
   }


   m_plotRenderer->UpdateLatLonLabels(xm, ym, zoom, m_plotProperty.m_gridLabel);
   m_plotRenderer->Update2D();

  // changed the position of this rendering
  Layout();

  m_vtkWidget->Refresh();

  Refresh();
}
//----------------------------------------
void CWorldPlotPanel::Update2D()
{
  if (Is2D())
  {
    m_plotRenderer->Update2D();
    UpdateRender();
  } else
      UpdateRender();
}

//----------------------------------------
void CWorldPlotPanel::AddData(CWorldPlotData* pdata, wxProgressDialog* dlg)
{
  wxString szMsg = wxString::Format("Displaying data: %s ...", pdata->GetDataTitle().c_str());
  if (dlg != NULL)
  {
    dlg->Update(0, szMsg);
  }

  if (pdata->GetColorBarRenderer() != NULL)
  {
    m_vtkWidget->GetRenderWindow()->AddRenderer(pdata->GetColorBarRenderer()->GetVtkRenderer());
  }

  m_plotRenderer->AddData(pdata);


  CGeoMap* geoMap = dynamic_cast<CGeoMap*>(pdata);
  if (geoMap != NULL)
  {
    wxString textLayer = wxString::Format("%s",
                                          geoMap->GetDataName().c_str());

    m_plotPropertyTab->GetLayerChoice()->Append(textLayer, static_cast<void*>(geoMap));
    m_plotPropertyTab->SetCurrentLayer(0);
  }



  if (dlg != NULL)
  {
    dlg->Update(1);
  }


  int32_t nFrames = 1;

  if (geoMap != NULL)
  {
    nFrames = geoMap->GetNrMaps();
  }

  m_animationToolbar->SetMaxFrame(nFrames);
  m_animationToolbar->SetBackgroundColour( wxColor( 0, 0, 0 ) );

  if (dlg != NULL)
  {
    dlg->Update(3);
  }

    // Fix for Windows refusing to draw initial screen:
#ifdef WIN32
  //Layout();
#endif


}

//----------------------------------------
CGeoMap* CWorldPlotPanel::GetCurrentLayer()
{
  return m_plotPropertyTab->GetCurrentLayer();
}
//----------------------------------------
bool CWorldPlotPanel::IsNumberOfMapsEquals(int32_t* numberOfMaps)
{
  return m_plotRenderer->IsNumberOfMapsEquals(numberOfMaps);
}

//----------------------------------------
void CWorldPlotPanel::SetProjectionByName(const std::string& projName)
{
  int32_t nprojection = CMapProjection::GetInstance()->NameToId(projName);

  if (nprojection <= 0)
  {
    CException e(CTools::Format("ERROR in CWorldPlotPanel::SetProjectionByName - Invalid projection name '%s' - id %d",
                                projName.c_str(), nprojection),
                 BRATHL_LOGIC_ERROR);
    throw(e);
  }

  SetProjection(nprojection);

}

//----------------------------------------
void CWorldPlotPanel::SetViewParams()
{
  CGeoMap* geoMap = GetCurrentLayer();
  if (geoMap == NULL)
  {
    return;
  }

  m_plotPropertyTab->GetZoomLon1()->SetValue(geoMap->m_plotProperty.m_zoomLon1, -180.0, -180.0, 180.0);
  m_plotPropertyTab->GetZoomLon2()->SetValue(geoMap->m_plotProperty.m_zoomLon2, 180.0, -180.0, 180.0);
  m_plotPropertyTab->GetZoomLat1()->SetValue(geoMap->m_plotProperty.m_zoomLat1, -90.0, -90.0, 90.0);
  m_plotPropertyTab->GetZoomLat2()->SetValue(geoMap->m_plotProperty.m_zoomLat2, 90.0, -90.0, 90.0);

  int32_t radiusPercent = static_cast<int32_t>(geoMap->m_plotProperty.m_deltaRadius * 100.0);
  m_plotPropertyTab->GetRadiusctrl()->SetValue(radiusPercent);
  int32_t factorPercent = static_cast<int32_t>(geoMap->m_plotProperty.m_heightFactor * 100.0);
  m_plotPropertyTab->GetFactorctrl()->SetValue(factorPercent);

  bool allLongitudes =  (CTools::IsLongitudeCircular(geoMap->m_plotProperty.m_zoomLon1,
                                                     geoMap->m_plotProperty.m_zoomLon2,
                                                     2.0));

  if ( ( geoMap->m_plotProperty.m_zoom == true ) && ( ! allLongitudes ) )
  {
    CZoomChangedEvent ev(GetId(),
                         geoMap->m_plotProperty.m_zoomLat1,
                         geoMap->m_plotProperty.m_zoomLon1,
                         geoMap->m_plotProperty.m_zoomLat2,
                         geoMap->m_plotProperty.m_zoomLon2);
    wxPostEvent(this, ev);

    if (m_is3D == false)
    {
      CViewStateCommandEvent evView(GetId(), CViewStateCommandEvent::viewSave);
      wxPostEvent(this, evView);
    }

  }

}
//----------------------------------------
void CWorldPlotPanel::SetProjection()
{
  CGeoMap* geoMap = GetCurrentLayer();
  if (geoMap == NULL)
  {
    return;
  }
  if (geoMap->m_plotProperty.m_projection.empty())
  {
    geoMap->m_plotProperty.m_projection = CMapProjection::GetInstance()->IdToName(VTK_PROJ2D_3D);
  }

  int32_t projection = CMapProjection::GetInstance()->NameToId(geoMap->m_plotProperty.m_projection);

  //SetProjection(projection);

  CProjectionChangedEvent evProj(GetId(),
                                 projection);
  wxPostEvent(m_plotPropertyTab, evProj);
  wxPostEvent(this, evProj);

}

//----------------------------------------
void CWorldPlotPanel::SetProjection(int32_t proj)
{

  double x,y,zoom =1, viewLat, viewLon;

  this->SetCursor(*wxHOURGLASS_CURSOR);

  m_vtkWidget->Freeze();
  this->Freeze();

  //m_style2D->ClearStates();
  int32_t oldProj = m_plotRenderer->GetProjection();

  if (oldProj != 0)
  {
    //RestoreState(0);
  }

  if ( oldProj == VTK_PROJ2D_3D ) {

      viewLat = m_style3D->GetViewCenterLatitude();
      viewLon = m_style3D->GetViewCenterLongitude();
      zoom = m_style3D->GetViewZoom();

      // now we go to 2D
      m_vtkWidget->SetInteractorStyle(m_style2D);
      m_is3D = false;

  }
  else if (proj == VTK_PROJ2D_3D){

      m_style2D->GetViewMidPoint(x, y);
      vtkProj2DFilter::normalizedDeprojection2D(oldProj,
                                                    this->projCenterLatitude, this->projCenterLongitude,
                                                    x, y, viewLat, viewLon);
      zoom = this->m_style2D->GetZoomScale();

       m_vtkWidget->SetInteractorStyle(m_style3D);
       m_is3D = true;

  } else if (proj > 0) {
      //2D to 2D

      m_style2D->GetViewMidPoint(x, y);
      vtkProj2DFilter::normalizedDeprojection2D(oldProj,
                                                    this->projCenterLatitude, this->projCenterLongitude,
                                                    x, y, viewLat, viewLon);

      zoom = this->m_style2D->GetZoomScale();

      m_vtkWidget->SetInteractorStyle(m_style2D);
      m_is3D = false;

  }


  // correction --
  // if the projection has been panned in such a way that
  // there is no point of it in the center of the screen,
  // we have no other resort but to center it again.
  if ( viewLat == HUGE_VAL || std::isnan(viewLat) || std::isinf(viewLat) )
      {
            viewLat = this->projCenterLatitude;
            zoom = 1.0;
      }


      if ( viewLon == HUGE_VAL || std::isnan(viewLon) || std::isinf(viewLon) )
      {
           viewLon = this->projCenterLongitude;
           zoom = 1.0;
      }


  if ( proj == VTK_PROJ2D_LAMBERT_CYLINDRICAL || proj == VTK_PROJ2D_PLATE_CAREE || proj == VTK_PROJ2D_MERCATOR )
  {
      m_plotPropertyTab->GetViewNotebook()->GetPage(1)->Enable(true);
  }
  else
  {
      m_plotPropertyTab->GetViewNotebook()->GetPage(1)->Enable(false);
  }

  m_plotPropertyTab->GetViewNotebook()->GetPage(2)->Enable(m_is3D);

  global_wplotViewSizer->Layout();

  m_plotRenderer->SetProjection(proj);

  OnSetPreferredRatio2();

    if ( m_is3D ) {

       m_style3D->SetViewParameters(viewLat, viewLon, 0.0, zoom);

   } else {

        // calculate a new center lat/lon?
        // some projections only show half of the world, so it might be necessary to recenter them
        vtkProj2DFilter::calcCenterLatLon(proj, this->projCenterLatitude, this->projCenterLongitude,
                                               viewLat, viewLon, this->projCenterLatitude, this->projCenterLongitude);
        CWPlotPropertyPanel *prop_panel = GetPlotPropertyTab();
        prop_panel->SetCenterPoint(this->projCenterLatitude, this->projCenterLongitude);
        SetCenterLatitude(this->projCenterLatitude);
        SetCenterLongitude(this->projCenterLongitude);

        vtkProj2DFilter::normalizedProjection2D(proj, this->projCenterLatitude, this->projCenterLongitude,
                                                 viewLat, viewLon, x, y);


        m_style2D->SetViewMidPoint(x, y);
        m_style2D->SetZoomScale(zoom);

   }

  //SaveInitialState();

  m_vtkWidget->Thaw();
  this->Thaw();

  this->SetCursor(wxNullCursor);
  Update2D();

}

//----------------------------------------
void CWorldPlotPanel::SetCenterLatitude(double c)
{
  projCenterLatitude = c;
  m_plotRenderer->SetCenterLatitude(c);
}
//----------------------------------------
void CWorldPlotPanel::SetCenterLongitude(double c)
{
  projCenterLongitude = c;
  m_plotRenderer->SetCenterLongitude(c);
}

//----------------------------------------
void CWorldPlotPanel::AdjustSplitPanelSashPosition()
{
  // Update sash position in splitter window
  // Try to keep the size of the property pane the same
  //  sashposition = - (self.propertypanel.GetSize()[0] + self.splitpanel.GetSashSize())
  wxSize size = m_propertyPanel->GetSize();
  int32_t sashPosition = - (size.GetWidth() + m_splitPanel->GetSashSize());
  m_splitPanel->SetSashPosition(sashPosition);
}
//----------------------------------------
void CWorldPlotPanel::EvtSize(wxWindow& window, const wxSizeEventFunction& method, wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_SIZE,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);

}

//----------------------------------------
void CWorldPlotPanel::InstallEventListeners()
{
  m_vtkWidget->AddObserver(vtkCommand::CharEvent, m_vtkCharEventCallback);


  CVtkUpdate2DCallback* vtkUpdate2DCallback = CVtkUpdate2DCallback::New(this, m_style2D, m_style3D);
  m_style2D->AddObserver(vtkInteractorStyleWPlot::EndZoomEvent, vtkUpdate2DCallback);
  m_style2D->AddObserver(vtkInteractorStyleWPlot::EndPanEvent, vtkUpdate2DCallback);
  m_style2D->AddObserver(vtkInteractorStyleWPlot::EndRestoreStateEvent, vtkUpdate2DCallback);
  vtkUpdate2DCallback->Delete();

  vtkCallbackCommand* callback = vtkCallbackCommand::New();
  callback->SetCallback(&CWorldPlotPanel::SetFocusVtkWidget);
  m_vtkWidget->AddObserver(vtkCommand::MouseMoveEvent, callback);
  callback->Delete();

  //KAVOK:  add a listener for adaptive Lat/Lon
  CVtkUpdateLatLonCallback* vtkUpdate2DLatLonCallback = CVtkUpdateLatLonCallback::New( m_plotRenderer, m_style2D, m_style3D );
  m_style2D->AddObserver(vtkInteractorStyleWPlot::EndZoomEvent, vtkUpdate2DLatLonCallback);
  m_style3D->AddObserver("WorldViewChanged", /*vtkUpdate2DLatLonCallback*/vtkUpdate2DCallback);
  vtkUpdate2DLatLonCallback->Delete();

  CWorldPlotPanel::EvtSize(*m_vtkWidget,
                           (wxSizeEventFunction)&CWorldPlotPanel::OnSetPreferredRatio, NULL, this);

  CWorldPlotPanel::EvtSize(*m_splitPanel,
                           (wxSizeEventFunction)&CWorldPlotPanel::OnSplitPanelSize, NULL, this);

  CWorldPlotPanel::EvtSize(*m_propertyPanel,
                           (wxSizeEventFunction)&CWorldPlotPanel::OnPropertyPanelSize, NULL, this);

  CWorldPlotPanel::EvtSize(*m_plotPropertyTab,
                           (wxSizeEventFunction)&CWorldPlotPanel::OnPlotPropertyTabSize, NULL, this);

  CWPlotPropertyPanel::EvtProjectionChanged(*this,
                                            (CProjectionChangedEventFunction)&CWorldPlotPanel::OnProjectionChanged, NULL, this);

  CWPlotPropertyPanel::EvtCenterPointChanged(*this,
                                             (CCenterPointChangedEventFunction)&CWorldPlotPanel::OnCenterPointChanged, NULL, this);


  CWPlotPropertyPanel::EvtRangeChanged(*this,
                                       (CRangeChangedEventFunction)&CWorldPlotPanel::OnRangeChanged, NULL, this);

  CWPlotPropertyPanel::EvtNumLabelsChanged(*this,
                                           (CNumLabelsChangedEventFunction)&CWorldPlotPanel::OnNumlabelsChanged, NULL, this);


  CAnimationToolbar::EvtKeyframeChanged(*this,
                                        (CKeyframeEventFunction)&CWorldPlotPanel::OnKeyframeChanged, NULL, this);

  CWPlotPropertyPanel::EvtViewStateChanged(*this,
                                          (CViewStateChangedEventFunction)&CWorldPlotPanel::OnViewChanged, NULL, this);

  CWPlotPropertyPanel::EvtViewStateCommand(*this,
                                          (CViewStateCommandEventFunction)&CWorldPlotPanel::OnViewCommand, NULL, this);

  CWPlotPropertyPanel::EvtUpdateRangeCommand(*this,
                                          (CUpdateRangeEventFunction)&CWorldPlotPanel::OnUpdateRange, NULL, this);

  CWPlotPropertyPanel::EvtZoomChangedCommand(*this,
                                          (CZoomChangedEventFunction)&CWorldPlotPanel::OnZoomChanged, NULL, this);

  CWPlotPropertyPanel::EvtRadiusChangedCommand(*this,
                                          (CRadiusChangedEventFunction)&CWorldPlotPanel::OnRadiusChanged, NULL, this);

  CWPlotPropertyPanel::EvtFactorChangedCommand(*this,
                                          (CFactorChangedEventFunction)&CWorldPlotPanel::OnFactorChanged, NULL, this);
  CLUTPanel::EvtLutFrameClose(*this,
                              -1,
                              (CLutFrameCloseEventFunction)&CWorldPlotPanel::OnLutFrameClose);

  CWPlotPropertyPanel::EvtDisplayDataAttrChanged(*this,
                              (CDisplayDataAttrChangedEventFunction)&CWorldPlotPanel::OnDisplayDataAttrChanged, NULL, this);

  CWPlotPropertyPanel::EvtVScaleChanged(*this,
                              (CVectorScaleChangedEventFunction)&CWorldPlotPanel::OnDisplayVectorScale, NULL, this);


  CContourPropPanel::EvtContourPropFrameClose(*this,
                              -1,
                              (CContourPropFrameCloseEventFunction)&CWorldPlotPanel::OnContourPropFrameClose);
}
//----------------------------------------
void CWorldPlotPanel::UpdateViewStateCtrl()
{

  int32_t maxRange = -1;
  if (m_is3D)
  {
    maxRange = m_plotRenderer->CountState() - 1;
  }
  else
  {
    maxRange = m_style2D->CountState(m_plotRenderer->GetProjection()) - 1;
  }

  GetPlotPropertyTab()->GetViewctrl()->SetRange(0, maxRange);
  GetPlotPropertyTab()->GetViewctrl()->SetValue(maxRange);
}

//----------------------------------------
void CWorldPlotPanel::ZoomTo(double lon[2], double lat[2])
{
  if (m_is3D)
  {
    return;
  }

  ZoomTo(lat[0], lon[0], lat[1], lon[1]);
}

//----------------------------------------
void CWorldPlotPanel::ZoomTo(double lat1, double lon1,  double lat2, double lon2)
{
  if (m_is3D)
  {
    return;
  }
  ZoomTo2D(lat1, lon1, lat2, lon2);
}

//----------------------------------------
void CWorldPlotPanel::ZoomInit(double scale)
{
    if (m_is3D)
    {
        m_style3D->SetViewParameters(m_style3D->GetViewCenterLatitude(),
                                     m_style3D->GetViewCenterLongitude(),
                                     0.0, scale);
    }
    else
    {
        m_style2D->SetZoomScale(scale);
    }
}

//----------------------------------------
void CWorldPlotPanel::ZoomTo2D(double lat1, double lon1, double lat2, double lon2)
{
  double lon[2];
  double lat[2];

  lon[0] = lon1,
  lat[0] = lat1;
  lon[1] = lon2;
  lat[1] = lat2;

  ZoomTo2D(lon, lat);

}
//----------------------------------------
void CWorldPlotPanel::ZoomTo2D(double lon[2], double lat[2])
{
  if (m_is3D)
  {
    return;
  }

  if (lon[0] < -179.0)
  {
    lon[0] += 1.0;
  }
  if (lon[1] > -179.0)
  {
    lon[1] -= 1.0;
  }
  int x1, y1, x2, y2;
  x1 = y1 = x2 = y2 = 0;

  bool bOk = m_plotRenderer->GetCoords2D(lon, lat, x1, y1, x2, y2);

  if (bOk)
  {
    int proj = m_plotRenderer->GetProjection();
    double xcenter, ycenter;
    double latCenter, longCenter, deltaLong, deltaLat;
    double zoomFactorLat = 1.0, zoomFactorLong = 1.0;
    double ratio[2];

    // calculate the midpoint
    latCenter = (lat[0] + lat[1])/2;
    longCenter = (lon[0] + lon[1])/2;

    vtkProj2DFilter::normalizedProjection2D(proj, this->projCenterLatitude, this->projCenterLongitude, latCenter, longCenter, xcenter, ycenter);
    m_style2D->SetViewMidPoint(xcenter, ycenter);

    // calculate the zoom factor
    m_style2D->GetRatioVector(ratio);
     deltaLat = fabs(lat[1] - lat[0]) * ratio[1];
    deltaLong = fabs(lon[1] - lon[0]) * ratio[0];

    zoomFactorLat = 180.0 / deltaLat;
    zoomFactorLong = 360.0 / deltaLong;

    // we will chose the one that let's us see everything, so the lesser one
    m_style2D->SetZoomScale( zoomFactorLat < zoomFactorLong ? zoomFactorLat : zoomFactorLong);

  }
}
//----------------------------------------
void CWorldPlotPanel::SaveInitialState()
{
  if (m_is3D)
  {
    SaveInitialState3D();
  }
  else
  {
    SaveInitialState2D();
  }

  UpdateViewStateCtrl();

}
//----------------------------------------
void CWorldPlotPanel::SaveInitialState2D()
{
  if (m_style2D->HasState(m_plotRenderer->GetProjection()) == false)
  {
    SaveState();
  }

}
//----------------------------------------
void CWorldPlotPanel::SaveInitialState3D()
{
  if (m_plotRenderer->HasState() == false)
  {
    SaveState();
  }

}
//----------------------------------------
void CWorldPlotPanel::RestoreState()
{
  if (m_is3D)
  {
    m_plotRenderer->RestoreState();
  }
  else
  {
    m_style2D->RestoreState(m_plotRenderer->GetProjection());
  }

  UpdateRender();
}

//----------------------------------------
void CWorldPlotPanel::RestoreState(int32_t index)
{
  if (m_is3D)
  {
    m_plotRenderer->RestoreState(index);
  }
  else
  {
    m_style2D->RestoreState(m_plotRenderer->GetProjection(), index);
  }

  UpdateRender();
}
//----------------------------------------
void CWorldPlotPanel::SaveState()
{
  if (m_is3D)
  {
    m_plotRenderer->SaveState();
  }
  else
  {
    m_style2D->SaveState(m_plotRenderer->GetProjection());
  }

  UpdateViewStateCtrl();

}
//----------------------------------------
void CWorldPlotPanel::ClearStates()
{
  if (m_is3D)
  {
    m_plotRenderer->ClearStates();
  }
  else
  {
    m_style2D->ClearStates(m_plotRenderer->GetProjection());
  }

  UpdateViewStateCtrl();
}
//----------------------------------------
void CWorldPlotPanel::OnLutFrameClose( CLutFrameCloseEvent &event )
{
  wxPostEvent(GetParent(), event);
}
//----------------------------------------
void CWorldPlotPanel::OnContourPropFrameClose( CContourPropFrameCloseEvent &event )
{
  wxPostEvent(GetParent(), event);
}

//----------------------------------------
void CWorldPlotPanel::OnSetPreferredRatio( wxSizeEvent &event )
{
  OnSetPreferredRatio2(&event);
}
//----------------------------------------
void CWorldPlotPanel::OnSetPreferredRatio2(wxSizeEvent *event)
{
  double ratio = 0;
  double xratio = 0;
  double yratio = 0;

  wxSize size;

  if (event != NULL)
  {
    size = event->GetSize();
  }
  else
  {
    size = dynamic_cast<wxWindow *>(m_vtkWidget)->GetSize();
  }

  if (m_colorBarShowing)
    size.SetHeight(size.GetHeight() - 50);


  if ( (m_is3D == false) &&  (size.GetHeight() > 0) && (size.GetWidth() > 0) )
  {
    ratio = m_plotRenderer->GetGeoGrid()->GetVtkFilter()->GetXYRatio();
    if ( (ratio * size.GetHeight()) > size.GetWidth() )
    {
      // max width
      xratio = 1.0;
      yratio = size.GetWidth() / (ratio * size.GetHeight());
    }
    else
    {
      // max height
      xratio = (ratio * size.GetHeight()) / size.GetWidth();
      yratio = 1.0;
    }


    m_style2D->SetViewportSize(size.GetWidth(), size.GetHeight());
    m_style2D->SetRatio( xratio, yratio);

  }

  Update2D();
  UpdateRender();

}
//----------------------------------------
void CWorldPlotPanel::PrintPoints(std::ostream& os)
{

  vtkPolyData *outputSource = (vtkPolyData *) m_plotRenderer->GetGeoGrid()->GetVtkGeoGridSource()->GetOutput();
  vtkPolyData *outputFilter = (vtkPolyData *) m_plotRenderer->GetGeoGrid()->GetVtkFilter()->GetOutput();
  vtkPolyData *outputTransform = (vtkPolyData *) m_plotRenderer->GetGeoGrid()->GetVtkTransform()->GetOutput();

  if (m_plotRenderer->GetGeoGrid()->GetVtkMapper2D() == NULL)
  {
    return;
  }

  vtkCoordinate* c = m_plotRenderer->GetGeoGrid()->GetVtkMapper2D()->GetTransformCoordinate();
  const char *whichCoord = c->GetCoordinateSystemAsString();
  double* value;
  int* ivalue;
  vtkFloatingPointType* xyz;
  vtkIdType p;

  os <<  "Point dump" << std::endl;
  for (int j = 0 ; j < outputSource->GetNumberOfPoints() ; j++)
  {
    xyz = outputSource->GetPoint(j);
    p = outputSource->FindPoint(xyz);
    os << " PointSource " << j
      << " x: " << xyz[0]
      << " y: " << xyz[1]
      << " z: " << xyz[2]
      << " find point : " << p
      << std::endl;

    xyz = outputFilter->GetPoint(j);
    p = outputFilter->FindPoint(xyz);
    os << " PointFilter " << j
      << " x: " << xyz[0]
      << " y: " << xyz[1]
      << " z: " << xyz[2]
      << " find point : " << p
      << std::endl;

    xyz = outputTransform->GetPoint(j);
    p = outputTransform->FindPoint(xyz);
    os << " PointTransform " << j
      << " x: " << xyz[0]
      << " y: " << xyz[1]
      << " z: " << xyz[2]
      << " find point : " << p
      << std::endl;

    c->SetValue(xyz);
    ivalue = c->GetComputedDisplayValue (m_plotRenderer->GetVtkRenderer());
    os << whichCoord << "(" << xyz[0] << ", " << xyz[1] << ", " << xyz[2]
     << ") -> Display(" << ivalue[0] << ", " << ivalue[1] << ")" << std::endl;
    ivalue = c->GetComputedLocalDisplayValue (m_plotRenderer->GetVtkRenderer());
    os << whichCoord << "(" << xyz[0] << ", " << xyz[1] << ", " << xyz[2]
     << ") -> LocalDisplay(" << ivalue[0] << ", " << ivalue[1] << ")" << std::endl;
    ivalue = c->GetComputedViewportValue (m_plotRenderer->GetVtkRenderer());
    os << whichCoord << "(" << xyz[0] << ", " << xyz[1] << ", " << xyz[2]
     << ") -> ViewPort(" << ivalue[0] << ", " << ivalue[1] << ")" << std::endl;
    value = c->GetComputedWorldValue (m_plotRenderer->GetVtkRenderer());
    os << whichCoord << "(" << xyz[0] << ", " << xyz[1] << ", " << xyz[2]
     << ") -> World(" << value[0] << ", " << value[1] << ", " << value[2] << ")" << std::endl;

  }
}

//----------------------------------------
void CWorldPlotPanel::OnProjectionChanged(CProjectionChangedEvent& event)
{
  SetProjection(event.m_projection);
  //UpdateRender();
}

//----------------------------------------
void CWorldPlotPanel::OnCenterPointChanged(CCenterPointChangedEvent& event)
{
  SetCenterLatitude(event.m_lat);
  SetCenterLongitude(event.m_lon);
  UpdateRender();
}


//----------------------------------------
void CWorldPlotPanel::OnRangeChanged(CRangeChangedEvent& event)
{
  RangeChanged(event);
  UpdateRender();
}
//----------------------------------------
void CWorldPlotPanel::RangeChanged(CRangeChangedEvent& event)
{

}
//----------------------------------------
void CWorldPlotPanel::OnNumlabelsChanged(CNumLabelsChangedEvent& event)
{
  NumlabelsChanged(event);
  UpdateRender();
}
//----------------------------------------
void CWorldPlotPanel::NumlabelsChanged(CNumLabelsChangedEvent& event)
{
}
//----------------------------------------
void CWorldPlotPanel::OnSplitPanelSize(wxSizeEvent& event)
{
  if (m_splitPanel->IsSplit())
  {
    AdjustSplitPanelSashPosition();
  }

  event.Skip();
}
//----------------------------------------
void CWorldPlotPanel::OnPropertyPanelSize(wxSizeEvent& event)
{
  m_propertyPanel->Layout();
  event.Skip();
}

//----------------------------------------
void CWorldPlotPanel::OnPlotPropertyTabSize(wxSizeEvent& event)
{
  m_plotPropertyTab->Layout();
  event.Skip();
}
//----------------------------------------
void CWorldPlotPanel::OnContourPropChanged( CContourPropChangedEvent &event )
{
  bool contourNeedGenerateValues = false;

  CGeoMap* geoMap = GetCurrentLayer();
  if (geoMap == NULL)
  {
    return;
  }

  if ((event.m_plotProperty.m_numContour != geoMap->m_plotProperty.m_numContour) ||
      (event.m_plotProperty.m_minContourValue != geoMap->m_plotProperty.m_minContourValue) ||
      (event.m_plotProperty.m_maxContourValue != geoMap->m_plotProperty.m_maxContourValue))
  {
    contourNeedGenerateValues = true;
  }

  if ((event.m_plotProperty.m_numContourLabel != geoMap->m_plotProperty.m_numContourLabel))
  {
    geoMap->SetContourLabelNeedUpdatePositionOnContour(true);
  }


  //---------------------------
  /////////////geoMap->m_plotProperty = event.m_plotProperty;
  geoMap->m_plotProperty.m_numContour = event.m_plotProperty.m_numContour;
  geoMap->m_plotProperty.m_minContourValue = event.m_plotProperty.m_minContourValue;
  geoMap->m_plotProperty.m_maxContourValue = event.m_plotProperty.m_maxContourValue;

  geoMap->m_plotProperty.m_contourLineColor = event.m_plotProperty.m_contourLineColor;
  geoMap->m_plotProperty.m_contourLineWidth = event.m_plotProperty.m_contourLineWidth;

  geoMap->m_plotProperty.m_withContourLabel = event.m_plotProperty.m_withContourLabel;
  geoMap->m_plotProperty.m_numContourLabel = event.m_plotProperty.m_numContourLabel;

  geoMap->m_plotProperty.m_contourLabelColor = event.m_plotProperty.m_contourLabelColor;

  geoMap->m_plotProperty.m_contourLabelSize = event.m_plotProperty.m_contourLabelSize;
  geoMap->m_plotProperty.m_contourLabelFormat = event.m_plotProperty.m_contourLabelFormat;
  //---------------------------

  if (contourNeedGenerateValues)
  {
    geoMap->ContourGenerateValues();
  }

  geoMap->SetContour3DProperties();
  geoMap->SetContour2DProperties();
  geoMap->SetContourLabelProperties();

  if (Is3D())
  {
    m_plotRenderer->RemoveActorsContourLabel(geoMap);

    if (geoMap->m_plotProperty.m_withContourLabel)
    {
      if (geoMap->GetContourLabelNeedUpdateOnWindow())
      {
        geoMap->UpdateContourLabels();
      }
      m_plotRenderer->AddActorsContourLabel(geoMap);
    }
    else
    {
      //m_plotRenderer->RemoveActorsContourLabel(geoMap);
    }
  }
  else // 2D
  {
    if (geoMap->m_plotProperty.m_withContourLabel)
    {
      if (geoMap->GetContourLabelNeedUpdateOnWindow())
      {
        geoMap->UpdateContourLabels2D();
      }
      else
      {
        if (geoMap->GetContourLabelNeedUpdatePositionOnContour())
        {
          geoMap->SetContourLabels2DPosition();
          geoMap->SetContourLabelNeedUpdatePositionOnContour(false);
        }
      }
      m_plotRenderer->RemoveActors2DFromRenderer();
      m_plotRenderer->AddActors2DToRenderer();
    }
    else
    {
      m_plotRenderer->RemoveActorsContourLabel(geoMap);
    }

  }

  if (geoMap->m_plotProperty.m_withContour == true)
  {
    UpdateRender();
  }


}
//----------------------------------------
void CWorldPlotPanel::LutChanged( CLutChangedEvent &event )
{
  m_plotPropertyTab->LutChanged(event);
  UpdateRender();
}
//----------------------------------------
void CWorldPlotPanel::SetFocusVtkWidget(vtkObject* obj, unsigned long, void*, void*)
{
  wxVTKRenderWindowInteractor *Interactor = dynamic_cast<wxVTKRenderWindowInteractor*>(obj);
  Interactor->SetFocus();
}
//----------------------------------------
void CWorldPlotPanel::UpdateValues()
{

  CGeoMap* geoMap = GetCurrentLayer();
  if (geoMap == NULL)
  {
    return;
  }
  vtkGeoMapFilter* geoMapFilter = NULL;

  double minhv = geoMap->GetComputedRangeMin();
  double maxhv = geoMap->GetComputedRangeMax();
  int32_t nrMaps = geoMap->GetNrMaps();

  for (int32_t iMap = 0 ; iMap < nrMaps; iMap++)
  {
    geoMapFilter = dynamic_cast<vtkGeoMapFilter*>((*geoMap->GetGeoMapFilterList())[iMap]);
    geoMapFilter->SetMinMappedValue(minhv);
    geoMapFilter->SetMaxMappedValue(maxhv);
  }

  CRangeChangedEvent ev(GetId(),
                        minhv,
                        maxhv);
  wxPostEvent(m_plotPropertyTab, ev);
  wxPostEvent(this, ev);


}
//----------------------------------------
void CWorldPlotPanel::UpdateRadius(int32_t radiusPercent)
{
  double radiusTmp = static_cast<double>(radiusPercent / 100.0);

  UpdateRadius(radiusTmp);
}
//----------------------------------------
void CWorldPlotPanel::UpdateFactor(int32_t factorPercent)
{
  double factorTmp = static_cast<double>(factorPercent / 100.0);

  UpdateFactor(factorTmp);
}
//----------------------------------------
void CWorldPlotPanel::UpdateRadius(double radius)
{
  CGeoMap* geoMap = GetCurrentLayer();
  if (geoMap == NULL)
  {
    return;
  }

  vtkGeoMapFilter* geoMapFilter = NULL;

  int32_t nrMaps = geoMap->GetNrMaps();

  for (int32_t iMap = 0 ; iMap < nrMaps; iMap++)
  {
    geoMapFilter = dynamic_cast<vtkGeoMapFilter*>((*geoMap->GetGeoMapFilterList())[iMap]);
    geoMapFilter->SetRadius(1.0 + radius);
  }

  UpdateRender();
}

//----------------------------------------
void CWorldPlotPanel::UpdateFactor(double factor)
{
  CGeoMap* geoMap = GetCurrentLayer();
  if (geoMap == NULL)
  {
    return;
  }
  vtkGeoMapFilter* geoMapFilter = NULL;

  int32_t nrMaps = geoMap->GetNrMaps();

  for (int32_t iMap = 0 ; iMap < nrMaps; iMap++)
  {
    geoMapFilter = dynamic_cast<vtkGeoMapFilter*>((*geoMap->GetGeoMapFilterList())[iMap]);
    geoMapFilter->SetFactor(factor);
  }

  UpdateRender();
}

//----------------------------------------
void CWorldPlotPanel::OnKeyframeChanged(CKeyframeEvent& event)
{
  if (m_finished)
  {
    return;
  }
  m_plotRenderer->OnKeyframeChanged(event);
  UpdateRender();
}
//----------------------------------------
void CWorldPlotPanel::DisplayContour(bool show)
{
  CGeoMap* geoMap = GetCurrentLayer();
  if (geoMap == NULL)
  {
    return;
  }

  if (m_is3D)
  {
    if (show)
    {
      if (geoMap->GetContourLabelNeedUpdateOnWindow())
      {
        geoMap->UpdateContourLabels();
      }

      if (geoMap->GetVtkContourActor() == NULL)
      {
        geoMap->SetContour3D();
      }

      m_plotRenderer->AddActorsContour(geoMap);
    }
    else
    {
      m_plotRenderer->RemoveActorsContour(geoMap);
    }
  }
  else // 2D
  {
    if (show)
    {
      if (geoMap->GetContourLabelNeedUpdateOnWindow())
      {
        geoMap->UpdateContourLabels2D();
      }

      if (geoMap->GetVtkContourActor2D() == NULL)
      {
        geoMap->SetContour2D();
        geoMap->UpdateContourLabels2D(); // because sometimes, on the first time refresh is not perfect

      }
      m_plotRenderer->RemoveActors2DFromRenderer();

      m_plotRenderer->AddActors2DToRenderer();
    }
    else
    {
      m_plotRenderer->RemoveActors2DContour(geoMap);
    }
  }
}
//----------------------------------------
void CWorldPlotPanel::DisplayGridLabels(bool show)
{
    this->m_plotProperty.m_gridLabel = show;
    m_plotRenderer->Update2D();

}
//----------------------------------------
void CWorldPlotPanel::DisplaySolidColor(bool show)
{
  CGeoMap* geoMap = GetCurrentLayer();
  if (geoMap == NULL)
  {
    return;
  }

  if (m_is3D)
  {
    if (show)
    {
      if (geoMap->GetVtkActor() == NULL)
      {
        geoMap->SetSolidColor3D();
      }
      m_plotRenderer->AddActorsSolidColor(geoMap);
    }
    else
    {
      m_plotRenderer->RemoveActorsSolidColor(geoMap);
    }
  }
  else // 2D
  {
    if (show)
    {
      if (geoMap->GetVtkActor2D() == NULL)
      {
        geoMap->SetSolidColor2D();
      }
      m_plotRenderer->RemoveActors2DFromRenderer();
      m_plotRenderer->AddActors2DToRenderer();
    }
    else
    {
      m_plotRenderer->RemoveActors2DSolidColor(geoMap);
    }
  }

}
//----------------------------------------
void CWorldPlotPanel::OnDisplayDataAttrChanged(CDisplayDataAttrChangedEvent& event)
{
  this->SetCursor(*wxHOURGLASS_CURSOR);

  switch (event.m_action)
  {
  case CDisplayDataAttrChangedEvent::displayContour :
    {
      DisplayContour(event.m_value);
      break;
    }
  case CDisplayDataAttrChangedEvent::displaySolidColor :
    {
      DisplaySolidColor(event.m_value);
      break;
    }
  case CDisplayDataAttrChangedEvent::displayGridLabels :
   {
      DisplayGridLabels(event.m_value);
      break;
    }
  default :
    {
      break;
    }
  }

  UpdateRender();
  this->SetCursor(wxNullCursor);

}

//----------------------------------------
void CWorldPlotPanel::OnDisplayVectorScale(CVectorScaleChangedEvent& event)
{
  this->SetCursor(*wxHOURGLASS_CURSOR);

  CGeoVelocityMap * vmap = dynamic_cast<CGeoVelocityMap *> (GetCurrentLayer());
  if ( vmap != NULL ) {

      /*vtkVelocityGlyphFilter *gFilter = vmap->GetGlyphFilter();
      if ( gFilter != NULL ) {
          //gFilter->Execute();
          printf("CWorldPlotPanel::OnDisplayVectorScale\n");
      }*/

  }

//  GetWorldPlotRenderer()->Update();
  UpdateRender();
  //GetVtkWidget()->Render();


  this->SetCursor(wxNullCursor);

}

//----------------------------------------
void CWorldPlotPanel::OnFactorChanged(CFactorChangedEvent& event)
{
  UpdateFactor(event.m_index);
}
//----------------------------------------
void CWorldPlotPanel::OnRadiusChanged(CRadiusChangedEvent& event)
{
  UpdateRadius(event.m_index);
}
//----------------------------------------
void CWorldPlotPanel::OnZoomChanged(CZoomChangedEvent& event)
{
  ZoomTo(event.m_lat1, event.m_lon1, event.m_lat2, event.m_lon2);
}
//----------------------------------------
void CWorldPlotPanel::OnUpdateRange(CUpdateRangeEvent& event)
{
  UpdateValues();
}
//----------------------------------------
void CWorldPlotPanel::OnViewCommand(CViewStateCommandEvent& event)
{
  if (event.m_action == CViewStateCommandEvent::viewClear)
  {
    ClearStates();
  }
  else if (event.m_action == CViewStateCommandEvent::viewSave)
  {
    SaveState();
  }
}
//----------------------------------------
void CWorldPlotPanel::OnViewChanged(CViewStateChangedEvent& event)
{
   // removed by me
  //RestoreState(event.m_index);
}




void CVtkUpdate2DCallback::Execute(vtkObject *caller, unsigned long, void*)
{
       double minLat, minLong, maxLat, maxLong;
       double midx, midy;
       double zoom =0;

       CWorldPlotRenderer* wplotRenderer = m_wplotPanel->GetWorldPlotRenderer();

        //m_wplotPanel->Update2D();
        //wplotRenderer->Update2D();

       CGeoGrid* geogrid = wplotRenderer->GetGeoGrid();
       vtkGeoGridSource* gsource = geogrid->GetVtkGeoGridSource();

       // get the style info
       if ( geogrid->Is2D() )
       {

            m_style2D->GetViewMidPoint(midx, midy);
            zoom = m_style2D->GetZoomScale();

            // let's calculate our REAL center lat/lon
            // not all projections change it.
            vtkProj2DFilter::normalizedDeprojection2D(wplotRenderer->GetProjection(),
                                                    wplotRenderer->GetCenterLatitude(), wplotRenderer->GetCenterLongitude(),
                                                    0.5, 0.5, centerLat, centerLong);

            // we first calculate the mid latitude and longitude of our viewport
            vtkProj2DFilter::normalizedDeprojection2D(wplotRenderer->GetProjection(),
                                                    wplotRenderer->GetCenterLatitude(), wplotRenderer->GetCenterLongitude(),
                                                    midx, midy, midLat, midLong);


            // if the projection has been panned in such a way that
            // there is no point of it in the center of the screen,
            // we have no other resort but to center it again.
            if ( midLat == HUGE_VAL || std::isnan(midLat) || std::isinf(midLat) )
            {
                midLat = wplotRenderer->GetCenterLatitude();
                zoom = 1.0;
            }


            if ( midLong == HUGE_VAL || std::isnan(midLong) || std::isinf(midLong) )
            {
                midLong = wplotRenderer->GetCenterLongitude();
                zoom = 1.0;
            }


            gsource->SetMidLat(midLat);
            gsource->SetMidLong(midLong);

       }
       else
       {
           zoom = m_style3D->GetViewZoom();
           midLat = m_style3D->GetViewCenterLatitude();
           midLong = m_style3D->GetViewCenterLongitude();
           gsource->SetMidLat(midLat);
           gsource->SetMidLong(midLong);
       }

       double latgrat = GetLatExtent(minLat, maxLat);
       double longgrat = GetLongExtent(minLong, maxLong);

       // tell the geogrid we are only interested in these:
       gsource->SetInitLat(minLat);
       gsource->SetEndLat(maxLat);
       gsource->SetInitLong(minLong);
       gsource->SetEndLong(maxLong);

       gsource->SetLongGraticule( longgrat );
       gsource->SetLatGraticule(latgrat);

       // update the stuff
       wplotRenderer->UpdateLOD(midLat, midLong, minLat, maxLat, minLong, maxLong, zoom);
       m_wplotPanel->Update2D();
       wplotRenderer->Update2D();

    }

double CVtkUpdate2DCallback::GetLongExtent(double &minLong, double &maxLong)
{
    double extt = 0;
    double zoom = 1.0;
    double graticule = 30;

    int proj = m_wplotPanel->GetWorldPlotRenderer()->GetProjection();

    switch ( proj )
    {
        case VTK_PROJ2D_LAMBERT_CYLINDRICAL:
        case VTK_PROJ2D_PLATE_CAREE:
        case VTK_PROJ2D_MERCATOR:
          /*
            zoom = m_style2D->GetZoomScale();
            if ( zoom < 1.5 )
            {
                //we see everything..
                minLong = -180;
                maxLong = 180;
                extt = 180;
            }
            else
            {

                extt = 180/zoom;

                minLong = midLong - extt;
                maxLong = midLong + extt;

                minLong = minLong == HUGE_VAL ? centerLong-180 : minLong;
                maxLong = maxLong == HUGE_VAL ? centerLong+180 : maxLong;
            }

        break; */
        case VTK_PROJ2D_MOLLWEIDE:
        case VTK_PROJ2D_ROBINSON:

            zoom = m_style2D->GetZoomScale();
            if ( zoom < 1.5 )
            {
                //we see everything..
                minLong = -180;
                maxLong = 180;
                extt = 180;

            }
            else
            {
                extt = 90;
                extt = 2.5*extt/(zoom*cos(fabs(midLat)*DEG_TO_RAD));
                extt = extt > 180 ? 180 : extt;

                minLong = midLong - extt;
                maxLong = midLong + extt;

                minLong = minLong == HUGE_VAL ? centerLong-180 : minLong;
                maxLong = maxLong == HUGE_VAL ? centerLong+180 : maxLong;
            }

        break;

        case VTK_PROJ2D_LAMBERT_AZIMUTHAL:
        case VTK_PROJ2D_AZIMUTHAL_EQUIDISTANT:
        case VTK_PROJ2D_STEREOGRAPHIC:
        case VTK_PROJ2D_TMERCATOR:
        case VTK_PROJ2D_ORTHO:
        case VTK_PROJ2D_NEAR_SIGHTED:


            zoom = m_style2D->GetZoomScale();
            extt = 90+90*sin(fabs(midLat)*DEG_TO_RAD);
            extt = 2.5*extt/(zoom*cos(fabs(midLat)*DEG_TO_RAD));
            extt = extt > 180 ? 180 : extt;

            minLong = midLong - extt;
            maxLong = midLong + extt;

            if ( minLong < -180 )
            {
                // add the remaining to the maxLong
                maxLong += (-180 - minLong);
                minLong = -180;
                extt = 180;
            }

            if ( maxLong > 180 )
            {
                minLong -= (maxLong-180);
                maxLong = 180;
            }

            minLong = minLong == HUGE_VAL ? -180 : minLong;
            maxLong = maxLong == HUGE_VAL ? 180 : maxLong;

        break;

        case VTK_PROJ2D_3D:


           zoom = m_style3D->GetZoom();
           // how may longitudes do we see?? Depends on the latitude
           // this procedure is an approximation, better solutions possible
           extt = 90+90*sin(fabs(midLat)*DEG_TO_RAD);
           extt = 2.5*extt/(zoom*cos(fabs(midLat)*DEG_TO_RAD));
           extt = extt > 180 ? 180 : extt;


           minLong = midLong - extt;
           maxLong = midLong + extt;
           minLong = minLong < -180.0 ? minLong+360.0 : minLong;
           maxLong = maxLong > 180.0 ? maxLong-360.0 : maxLong;
        break;

        default:
            //vtkErrorMacro(<<"unknown projection");
        return 30;  // default
    }

    // now calculate the graticule for longitude
    graticule = fabs(2*extt);
    graticule = pow(2, ceil(log(graticule)/log(2.0))-2);

    // special, fixed graticule values.
    // after resolution of 2.5 ... do descending powers of 2
    switch ( int(graticule) )
    {
           case 512:
           case 256:
           case 128:
           case 64:
             graticule = 30.0;
           break;
           case 32:
             graticule = 15.0;
           break;
           case 16:
            graticule = 10.0;
           break;
           case 8:
            graticule = 5.0;
           break;
           case 4:
            graticule = 2.5;
    }

    return graticule;
}


double CVtkUpdate2DCallback::GetLatExtent(double &minLat, double &maxLat)
{
    double size[2];
    double midx, midy;
    double zoom;
    double graticule = 30;
    double lmax, gmax;

    CGeoGrid* geogrid = m_wplotPanel->GetWorldPlotRenderer()->GetGeoGrid();
    minLat = -90;
    maxLat = 90;

    if ( geogrid->Is2D() )
    {

        m_style2D->GetViewMidPoint(midx, midy);
        m_style2D->GetSize(size);
        zoom = m_style2D->GetZoomScale();

        if ( zoom > 3.5 )
        {
            minLat = 90;
            maxLat = -90;

            // search for minimum and maximum latitudes
            lmax = midy + 0.6/size[1];
            gmax = midx + 0.6/size[0];
            for ( int i =0 ; i< 3; i++)
            {
                gmax = midx + 0.6/size[0];

                for ( int k =0; k <3; k++ )
                {
                    double lat, lon;
                    vtkProj2DFilter::normalizedDeprojection2D(m_wplotPanel->GetWorldPlotRenderer()->GetProjection(),
                                                            centerLat, centerLong,
                                                            gmax, lmax, lat, lon);
                    if ( lat > maxLat )
                    {
                        maxLat = lat;
                    }
                    if ( lat < minLat )
                    {
                        minLat = lat;
                    }
                    gmax -= 0.6/size[0];
                }
                lmax -= 0.6/size[1];
            }
        }
    }
    else
    {
        zoom = m_style3D->GetZoom();
        maxLat = midLat + 180.0/zoom;
        maxLat = maxLat > 90.0 ?  90.0 : maxLat;
        minLat = midLat - 180.0/zoom;
        minLat = minLat < -90.0 ? -89.99 : minLat;
    }

    graticule = fabs(maxLat - minLat);
    graticule = pow(2, ceil(log(graticule)/log(2.0))-2);

     // special, fixed graticule values.
    // after resolution of 2.5 ... do descending powers of 2
    switch ( int(graticule) )
    {
           case 512:
           case 256:
           case 128:
           case 64:
             graticule = 30.0;
           break;
           case 32:
             graticule = 15.0;
           break;
           case 16:
            graticule = 10.0;
           break;
           case 8:
            graticule = 5.0;
           break;
           case 4:
            graticule = 2.5;
    }

    return graticule;
}


//-------------------------------------------------------------
//------------------- Lat/Lon Labels class --------------------
//-------------------------------------------------------------

bool operator==(LabelPoint l1, LabelPoint l2)
{
   return (l1.x == l2.x ) && (l1.y == l2.y);
}

bool operator<(LabelPoint l1, LabelPoint l2)
{
    double d1, d2;
    d1 = sqrt(l1.x*l1.x + l1.y*l1.y);
    d2 = sqrt(l2.x*l2.x + l2.y*l2.y);
    return d1<d2;
}

std::set<LabelPoint>::iterator find_label(std::set<LabelPoint> &labels, GeoGridLineInfo gInfo)
{
    std::set<LabelPoint>::iterator it;
    for ( it = labels.begin(); it != labels.end(); it++ )
    {
        LabelPoint il = *it;
        if ( il.gInfo.val == gInfo.val && il.gInfo.lineType == gInfo.lineType )
            break;
    }

    return it;
}

std::set<LabelPoint>::iterator find_near(std::set<LabelPoint> &labels, LabelPoint label, double distance)
{
    double dx, dy, dz;
    double d;

    std::set<LabelPoint>::iterator it;
    for ( it = labels.begin(); it != labels.end(); it++ )
    {
        LabelPoint il = *it;

        dx = il.x - label.x;
        dy = il.y - label.y;
        dz = il.z - label.z;

        d = sqrt(dx*dx + dy*dy + dz*dz);

        if ( d <= distance )
        {
                return it;
        }
    }
    return it;
}


LatLonLabels::LatLonLabels ( CGeoGrid *g, vtkRenderer *renderer ){
    m_geoGrid = g;
    m_format = "%g %s";
    m_fontSize = 10;
    m_renderer = renderer;

    m_labelCollection = vtkActor2DCollection::New();

}

void LatLonLabels::DeleteAllLabels(){
     vtkActor2D *actor2D = NULL;
     m_labelCollection->InitTraversal();
     while ( (actor2D = m_labelCollection->GetNextItem() ) != NULL )
          actor2D->Delete();

      m_labelCollection->RemoveAllItems();
}

LatLonLabels::~LatLonLabels(){
    DeleteAllLabels();
    m_labelCollection->Delete();
}

vtkActor2DCollection *LatLonLabels::GetTextLabels(){
    m_labelCollection->InitTraversal();
    return m_labelCollection;
}

// checks to see if a point is inside a box
inline bool inside(double xUp, double yUp, double xDown, double yDown, double pointx, double pointy){
    return ( pointx < xUp && pointx > xDown && pointy < yUp && pointy > yDown );
}

void LatLonLabels::Update3D(double xmpoint, double ympoint, double zoom){

    // store the positions of the labels
  std::set<LabelPoint> lpset;

   double center[3];

   RemoveFromRenderer();

  vtkInteractorStyle3DWPlot::latlon2position(xmpoint, ympoint, center);


  vtkGeoGridSource*  grid_source = m_geoGrid->GetVtkGeoGridSource();
  vtkPolyData *tdata = (vtkPolyData *)  grid_source->GetOutput();

  // Get the lines of the transformed data (done by the projection filter)
  vtkCellArray *lines = tdata->GetLines();
  int lnum = lines->GetNumberOfCells();

  // Get the cell data, that is, the data associated to each line (long/lat)
  // see the code of vtkGeoGridSource for InsertTuple1() calls
  // the scalar associated to each line is the index of the GeoGridLineInfo
  // structure in the vtkGeoGridSource
  vtkCellData *cellData = tdata->GetCellData();
  vtkDataArray *scalars = cellData->GetScalars();

  // We iterate through each line
  for ( int i =0; i < lnum; i++ ){
      double labelpos[3];
      vtkCell *cell = tdata->GetCell(i);

      // get the GeoGridLineInfo structure from the source
      double cellId = scalars->GetTuple1(i);
      GeoGridLineInfo gInfo = grid_source->GetGridLineInfo(cellId);

      LabelPoint worldcoord;
      worldcoord.x = 100;
      worldcoord.y = 100;
      worldcoord.z = 0;
      worldcoord.gInfo = gInfo;

      // We need to examine all the points of the line
      vtkPoints *points = cell->GetPoints();
      int nIds = points->GetNumberOfPoints();

      // this is to check the distance to middle point
      // this minimum distance depends on the latitude
      // so that poles don't seem too crowded
      double distance = 1000.0;

      // this is the numerical value as a std::string
      char degtext[50];

      // find if the line is within the viewing bounds
      for ( int j =0; j < nIds; j++ )
      {
          double pcoords[3];
          double tdistance = 0, difx, dify;
          points->GetPoint(j, pcoords);

          // the label for this line will be placed
          // in the point closest to the center

          difx = ympoint-pcoords[0];
          dify = xmpoint-pcoords[1];
          tdistance = sqrt(difx*difx + dify*dify);

          if ( tdistance < distance)
          {
                worldcoord.x = pcoords[0];
                worldcoord.y = pcoords[1];
                worldcoord.z = pcoords[2];
                //worldcoord[2] = pcoords[2];
                distance = tdistance;
           }
      }

      // offset each label a little
      double midgrat = 0;
      if ( gInfo.lineType == GRIDLINE_LONG )
      {
              midgrat =grid_source->GetLongGraticule()/4;
              worldcoord.y +=  xmpoint > 0 ? -midgrat : midgrat;
      }
      else
      {
              midgrat =grid_source->GetLatGraticule()/4;
              worldcoord.x +=  midgrat;
      }

      // convert coordinate value to std::string
      if ( gInfo.lineType == GRIDLINE_LAT )
      {
          // convert coordinate value to std::string
          if ( gInfo.val < 0 )
          {
             snprintf(degtext, 49, m_format, fabs(gInfo.val), "S");
          }
          else
          {
              snprintf(degtext, 49, m_format, gInfo.val, "N");
          }
      }
      else
      {
          // convert coordinate value to std::string
          if ( gInfo.val < 0 )
          {
             snprintf(degtext, 49, m_format, fabs(gInfo.val), "W");
          }
          else
          {
              snprintf(degtext, 49, m_format, gInfo.val, "E");
          }
      }


      // convert to 3D world
      vtkInteractorStyle3DWPlot::latlon2position(worldcoord.y, worldcoord.x, labelpos);

      // labels that cannot be seen are not rendered at all (distances > 1)
      double cdiffx = labelpos[0]-center[0];
      double cdiffy = labelpos[1]-center[1];
      double cdiffz = labelpos[2]-center[2];
      double cdist = sqrt(cdiffx*cdiffx + cdiffy*cdiffy + cdiffz*cdiffz);
      if ( cdist > 1 )
        continue;

      // create the label for this line
      vtkActor2D *label = vtkActor2D::New();
      vtkTextMapper *labelMapper = vtkTextMapper::New();
      labelMapper->GetTextProperty()->SetFontFamilyToArial();
      labelMapper->GetTextProperty()->SetFontSize(m_fontSize);
      labelMapper->GetTextProperty()->SetColor(0,0,0);
      labelMapper->GetTextProperty()->SetJustificationToCentered();
      labelMapper->SetInput(degtext);
      label->SetMapper(labelMapper);
      labelMapper->Delete();


      label->GetPositionCoordinate()->SetViewport(m_renderer);
      // std::set the coordinates in world position
      label->GetPositionCoordinate()->SetValue(labelpos);
      label->GetPositionCoordinate()->SetCoordinateSystemToWorld();
      label->GetPositionCoordinate()->GetValue(worldcoord.x, worldcoord.y, worldcoord.z);

      // if there is already a label near, do not render this one
      if ( find_near(lpset, worldcoord, 0.05/zoom) != lpset.end())
      {
          continue;
      }

      lpset.insert(worldcoord);

      // add it to the collection
      m_labelCollection->AddItem(label);

  }
}

void LatLonLabels::Update2D(double xmpoint, double ympoint, double zoom){

  // store the positions of the labels
  std::set<LabelPoint> lpset;

  vtkGeoGridSource*  grid_source = m_geoGrid->GetVtkGeoGridSource();

  // The transformed data (after transformed by projection filter, etc)
  vtkPolyData *tdata = (vtkPolyData *)  m_geoGrid->GetVtkTransform()->GetOutput();

  // We need to transform the center as manipulated by the interactor
  // to the center in Normalized Viewport coordinate system (as done in the transformation)
  double center[3] = {xmpoint, ympoint, 0.0};
  m_geoGrid->GetVtkTransform()->GetTransform()->GetInverse()->TransformPoint(center, center);


  // Get the lines of the transformed data (done by the projection filter)
  vtkCellArray *lines = tdata->GetLines();
  int lnum = lines->GetNumberOfCells();

  // Get the cell data, that is, the data associated to each line (long/lat)
  // see the code of vtkGeoGridSource for InsertTuple1() calls
  // the scalar associated to each line is the index of the GeoGridLineInfo
  // structure in the vtkGeoGridSource
  vtkCellData *cellData = tdata->GetCellData();
  vtkDataArray *scalars = cellData->GetScalars();

  // We iterate through each line
  for ( int i =0; i < lnum; i++ ){
      vtkCell *cell = tdata->GetCell(i);

      // get the GeoGridLineInfo structure from the source
      double cellId = scalars->GetTuple1(i);
      GeoGridLineInfo gInfo = grid_source->GetGridLineInfo(cellId);

        // the label aready exists, so skip
      if ( find_label(lpset, gInfo) != lpset.end() )
        continue;


      // We need to get all the points of the line
      // to see if it falls within the viewing range
      vtkPoints *points = cell->GetPoints();
      int nIds = points->GetNumberOfPoints();

      // this will be the position of the label
      LabelPoint labelPos;
      labelPos.x = 100;
      labelPos.y = 100;
      labelPos.z = 100;
      labelPos.gInfo = gInfo;

      double distance = 0.6;

      // this is the numerical value as a std::string
      char degtext[50];

      // find if the line is within the viewing bounds
      for ( int j =0; j < nIds; j++ )
      {
          double coords[3];
          double tdistance = 0, difx, dify;
          points->GetPoint(j, coords);

          // the label for this line will be placed
          // in the point closest to the center
          difx = center[0]-coords[0];
          dify = center[1]-coords[1];
          tdistance = sqrt(difx*difx + dify*dify);

          if ( tdistance < distance )
          {
              labelPos.x = coords[0];
              labelPos.y = coords[1];
              labelPos.z = coords[2];
              distance = tdistance;
          }
      }

      // check to see if this point is already used by another label
      // if so, forget it, makes it cleaner
      if ( find_near(lpset, labelPos, 0.1/zoom) != lpset.end() )
      {
          continue;
      }

      if ( gInfo.lineType == GRIDLINE_LAT )
      {
          // convert coordinate value to std::string
          if ( gInfo.val < 0 )
          {
             snprintf(degtext, 49, m_format, fabs(gInfo.val), "S");
          }
          else
          {
              snprintf(degtext, 49, m_format, gInfo.val, "N");
          }
      }
      else
      {
          // convert coordinate value to std::string
          if ( gInfo.val < 0 )
          {
             snprintf(degtext, 49, m_format, fabs(gInfo.val), "W");
          }
          else
          {
              snprintf(degtext, 49, m_format, gInfo.val, "E");
          }
      }

      // save the point in the std::list, so no other label
      // can be std::set here
      lpset.insert(labelPos);


      // create the label for this line
      vtkActor2D *label = vtkActor2D::New();
      vtkTextMapper *labelMapper = vtkTextMapper::New();
      labelMapper->GetTextProperty()->SetFontFamilyToArial();
      labelMapper->GetTextProperty()->SetFontSize(m_fontSize);
      labelMapper->GetTextProperty()->SetColor(0,0,0);
      labelMapper->GetTextProperty()->SetJustificationToCentered();
      labelMapper->SetInput(degtext);
      label->SetMapper(labelMapper);
      labelMapper->Delete();

      label->GetPositionCoordinate()->SetViewport(m_renderer);
      label->SetPosition(labelPos.x, labelPos.y);
      label->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();


      // add it to the collection
      m_labelCollection->AddItem(label);
  }
}


void LatLonLabels::AddToRenderer(){

   vtkActor2D *actor2D = NULL;
   m_labelCollection->InitTraversal();
   actor2D = m_labelCollection->GetNextItem();

    while ( actor2D != NULL ) {
            m_renderer->AddActor2D(actor2D);
            actor2D = m_labelCollection->GetNextItem();
    }
}

void LatLonLabels::RemoveFromRenderer(){

   vtkActor2D *actor2D = NULL;
   m_labelCollection->InitTraversal();
   while ( (actor2D = m_labelCollection->GetNextItem()) != NULL )
    m_renderer->RemoveActor2D(actor2D);

   DeleteAllLabels();
}

void LatLonLabels::SetLabelFormat(char *f){
    m_format = f;
}

void LatLonLabels::SetLabelSize(int s){
    m_fontSize = s;
}


//-------------------------------------------------------------
//------------------- CWorldPlotRenderer class --------------------
//-------------------------------------------------------------
CWorldPlotRenderer::CWorldPlotRenderer( wxWindow *parent, std::string mode ) : m_actors( false )
{
	Init();

	m_parent = parent;

	if ( ( mode == "crude" ) ||	( mode == "intermediate" ) || ( mode == "high" ) || ( mode == "full" ) )
		m_GSHHSFile = mode[ 0 ];
	else
		m_GSHHSFile = "l";  // Low

	m_GSHHSFile   = CTools::GetDataDir() + PATH_SEPARATOR + "gshhs_" + m_GSHHSFile + ".b";

	m_vtkRend = vtkRenderer::New();
	//KAVOK: added some default values
	vtkCamera* camera = m_vtkRend->GetActiveCamera();
	camera->SetRoll( -90 );
	camera->Elevation( -60 );
	camera->Zoom( 1.5 );

	m_transformations = vtkTransformCollection::New();

	m_globe = new CGlobe();
	m_geoGrid = new CGeoGrid();
	m_coastLine = new CCoastLine( m_GSHHSFile );

	SetTextActor();

	m_latlonLabels = new LatLonLabels( m_geoGrid, m_vtkRend );

	AddData( m_globe );
	AddData( m_geoGrid );
	AddData( m_coastLine );

	m_cLong = 0;
	m_cLati = 0;

	SetCenterLatitude( m_cLati );
	SetCenterLongitude( m_cLong );
}

//----------------------------------------
CWorldPlotRenderer::~CWorldPlotRenderer()
{
	DeleteTextActor();

	if ( m_vtkRend != NULL ){
		m_vtkRend->Delete();
		m_vtkRend = NULL;
	}

	if ( m_transformations != NULL ){
		m_transformations->Delete();
		m_transformations = NULL;
	}

	delete m_globe;
	m_globe = NULL;

	delete m_geoGrid;
	m_geoGrid = NULL;

	delete m_coastLine;
	m_coastLine = NULL;

	delete m_latlonLabels;
	m_latlonLabels = NULL;
}
//----------------------------------------
void CWorldPlotRenderer::DeleteTextActor()
{
	if ( m_textActor != NULL ) {
		m_textActor->Delete();
		m_textActor = NULL;
	}
}
//----------------------------------------
void CWorldPlotRenderer::Init()
{
  m_projection = 0;
  m_cLong = 0.0;
  m_cLati = 0.0;

  m_cameraResetted = true;

  m_parent = NULL;

  m_vtkRend = NULL;
  m_transformations = NULL;
  m_textActor = NULL;
  m_globe = NULL;
  m_geoGrid = NULL;
  m_coastLine = NULL;

  m_currentState = m_states.size() - 1;

  m_vtkWidget = NULL;

}
//----------------------------------------
bool CWorldPlotRenderer::IsNumberOfMapsEquals(int32_t* numberOfMaps)
{
  CGeoMap* geoMap = NULL;
  CObList::iterator it;
  if (numberOfMaps != NULL)
  {
    *numberOfMaps = -1;
  }

  if (m_actors.size() == 0)
  {
    return false;
  }

  int32_t nMaps = -1;

  for (it = m_actors.begin() ; it != m_actors.end() ; it++)
  {
    geoMap = dynamic_cast<CGeoMap*>(*it);
    if (geoMap != NULL)
    {
      if (nMaps < 0)
      {
        nMaps = geoMap->GetNrMaps();
      }
      if (nMaps != geoMap->GetNrMaps())
      {
        return false;
      }
    }
  }

  if (numberOfMaps != NULL)
  {
    *numberOfMaps = nMaps;
  }

  return true;
}

//----------------------------------------
bool CWorldPlotRenderer::GetCoords2D(double lon[2], double lat[2], int& x1, int& y1, int& x2, int& y2)
{
  bool bOk = true;

  double lonlat[2];
  x1 = y1 = x2 = y2 = 0;

  lonlat[0] = lon[0];
  lonlat[1] = lat[0];
  bOk &= m_geoGrid->FindDataCoords2D(m_vtkRend, lonlat, x1, y1);

  lonlat[0] = lon[1];
  lonlat[1] = lat[1];
  bOk &= m_geoGrid->FindDataCoords2D(m_vtkRend, lonlat, x2, y2);

  return bOk;
}

//----------------------------------------------------------------------------
void CWorldPlotRenderer::SaveState()
{
  vtkCameraState* state = vtkCameraState::New();

  vtkCamera* camera = m_vtkRend->GetActiveCamera();

  state->SetViewUp(camera->GetViewUp());
  state->SetFocalPoint(camera->GetFocalPoint());
  state->SetClippingRange(camera->GetClippingRange());
  state->SetPosition(camera->GetPosition());
  state->SetViewAngle(camera->GetViewAngle());
  state->SetParallelScale(camera->GetParallelScale());

  m_states.Insert(state);

  m_currentState = m_states.size() - 1;

}
//----------------------------------------------------------------------------
void CWorldPlotRenderer::RestoreState()
{
  //removed by me
  //RestoreState(m_currentState);

}
//----------------------------------------------------------------------------
void CWorldPlotRenderer::RestoreState(int32_t index)
{
  if (index < 0)
  {
    return;
  }

  vtkCameraState* state = GetState(index);
  vtkCamera* camera = m_vtkRend->GetActiveCamera();

  camera->SetViewUp(state->GetViewUp());
  camera->SetFocalPoint(state->GetFocalPoint());
  camera->SetClippingRange(state->GetClippingRange());
  camera->SetPosition(state->GetPosition());
  camera->SetViewAngle(state->GetViewAngle());
  camera->SetParallelScale(state->GetParallelScale());

}

//----------------------------------------------------------------------------
void CWorldPlotRenderer::ClearStates()
{
  if (m_states.size() <= 0)
  {
    return;
  }

  // In this loop, delete all array elements except first element
  vtkObArray::reverse_iterator rev_it;
  for (rev_it = m_states.rbegin() + 1; rev_it != m_states.rend() ; rev_it++)
  {
    m_states.Erase(rev_it.base());
  }

  m_currentState = m_states.size() - 1;
}
//----------------------------------------------------------------------------
vtkCameraState* CWorldPlotRenderer::GetState(int32_t index)
{
  if ((index < 0) || (index >= static_cast<int32_t>(m_states.size())))
  {
    throw CException(CTools::Format("ERROR in  CWorldPlotRenderer::GetState : Invalid index %d - Max. index %ld",
                                    index, (long)m_states.size() - 1),
                     BRATHL_LOGIC_ERROR);

  }
  vtkCameraState* state = dynamic_cast<vtkCameraState*>(m_states.at(index));
  if (state == NULL)
  {
    throw CException(CTools::Format("ERROR in  CWorldPlotRenderer::GetState : dynamic_cast<vtkCameraState*>(m_states.at(%d) returns NULL pointer - "
                                    "state array seems to contain objects other than those of the class vtkCameraState",
                                    index),
                     BRATHL_LOGIC_ERROR);

  }

  return state;
}

//----------------------------------------
void CWorldPlotRenderer::SetTextActor()
{
  DeleteTextActor();
  return;

  m_textActor = vtkTextActor::New();

  m_textActor->SetPosition(10, 10);
  m_textActor->SetPosition2(50, 10);

  // adding a mapper, required to avoid the messsage:
  // vtkOpenGLTexture: No scalar values found for texture input!
  // in VTK 5.6
  vtkTextMapper *tMapper = vtkTextMapper::New();
  tMapper->GetTextProperty()->SetFontFamilyToArial();
  tMapper->GetTextProperty()->SetFontSize(12);
  //tMapper->GetTextProperty()->SetColor(0,0,0);
  tMapper->GetTextProperty()->SetJustificationToCentered();
  //tMapper->SetInput(degtext);
  m_textActor->SetMapper(tMapper);
  tMapper->Delete();

}

//----------------------------------------
void CWorldPlotRenderer::AddData( CWorldPlotData* pdata )
{
	pdata->SetRenderer( m_vtkRend );

	m_actors.Insert( pdata );
	m_transformations->AddItem( pdata->GetTransform() );
	pdata->GetVtkTransform()->SetTransform( pdata->GetTransform() );

	ResetTextActor();
}
//----------------------------------------
void CWorldPlotRenderer::ResetTextActor()
{

  CGeoMap* geoMap = NULL;
  std::string text;
  CObList::iterator it;

  for (it = m_actors.begin() ; it != m_actors.end() ; it++)
  {
    geoMap = dynamic_cast<CGeoMap*>(*it);
    if (geoMap != NULL)
    {
      text = geoMap->m_plotProperty.m_title;
    }

  }

  if (m_textActor != NULL)
  {
    //m_textActor->SetInput(text.c_str());
    vtkTextMapper *tMapper = dynamic_cast<vtkTextMapper *>(m_textActor->GetMapper());
    if ( tMapper != NULL )
    {
        tMapper->SetInput(text.c_str());
    }

  }

}
//----------------------------------------
int32_t CWorldPlotRenderer::CountColorBarRender()
{
  int32_t count = 0;

  CObList::iterator it;

  for (it = m_actors.begin() ; it != m_actors.end() ; it++)
  {
    CWorldPlotData* pdata = dynamic_cast<CWorldPlotData*>(*it);
    if (pdata == NULL)
    {
      continue;
    }

    if (pdata->GetColorBarRenderer() != NULL)
    {
      count++;
    }

  }
  return count;
}
//----------------------------------------
void CWorldPlotRenderer::UpdateColorBarRender(bool show)
{
  if (m_vtkWidget == NULL)
  {
    return;
  }

  double r = m_vtkRend->GetBackground()[0];
  double g = m_vtkRend->GetBackground()[1];
  double b = m_vtkRend->GetBackground()[2];

  double yMin = 0;
  double yMax = 0;

  //CObList::iterator it;
  CObList::reverse_iterator it;
  int32_t index = 0;

  for (it = m_actors.rbegin() ; it != m_actors.rend() ; it++)
  //for (it = m_actors.begin() ; it != m_actors.end() ; it++)
  {
    CGeoMap* geoMap = dynamic_cast<CGeoMap*>(*it);
    if (geoMap == NULL)
    {
      continue;
    }

    if (geoMap->GetColorBarRenderer() != NULL)
    {
      geoMap->GetColorBarRenderer()->SetBackground(r, g, b);
      geoMap->GetColorBarRenderer()->ResetTextActor(geoMap);
      if (show)
      {
        yMax =  CWorldPlotPanel::m_percentSizeScalarBar * (index + 1);
        geoMap->GetColorBarRenderer()->GetVtkRenderer()->SetViewport(0, yMin, 1, yMax);
      }
      else
      {
        geoMap->GetColorBarRenderer()->GetVtkRenderer()->SetViewport(0, 0, 0, 0);
      }

      //geoMap->GetColorBarRenderer()->GetVtkRenderer()->Print(*CTrace::GetDumpContext());

    }
    yMin = yMax;
    index++;

  }

}
//----------------------------------------
void CWorldPlotRenderer::Update2D()
{
  CGeoMap* geoMap = NULL;

  CObList::iterator it;

  for (it = m_actors.begin() ; it != m_actors.end() ; it++)
  {
    geoMap = dynamic_cast<CGeoMap*>(*it);
    if (geoMap != NULL)
    {
      if (geoMap->Is2D())
      {
        geoMap->Update2D();
      }
    }
  }
}

void CWorldPlotRenderer::UpdateLOD(double midlat, double midlon, double minlat, double maxlat,
                                                double minlon, double maxlon, double zoom)
{
  CGeoVelocityMap* geoMap = NULL;

  CObList::iterator it;

  for (it = m_actors.begin() ; it != m_actors.end() ; it++)
  {
    geoMap = dynamic_cast<CGeoVelocityMap*>(*it);
    if (geoMap != NULL)
    {
        double lod = 5 - log(zoom);
        geoMap->GetGlyphFilter()->SetInitLat(minlat);
        geoMap->GetGlyphFilter()->SetEndLat(maxlat);
        geoMap->GetGlyphFilter()->SetInitLon(minlon);
        geoMap->GetGlyphFilter()->SetEndLon(maxlon);

        geoMap->GetGlyphFilter()->SetMidLon(midlon);
        geoMap->GetGlyphFilter()->SetMidLat(midlat);

        geoMap->GetGlyphFilter()->SetGroupRatio((int)lod);
    }
  }
}
//----------------------------------------

void CWorldPlotRenderer::UpdateLatLonLabels(double xmpoint, double ympoint, double zoom, bool show){


    m_latlonLabels->RemoveFromRenderer();

    if ( show )
    {

        if ( GetGeoMap() && GetGeoMap()->Is2D() )
            m_latlonLabels->Update2D(xmpoint, ympoint, zoom);
        else
        m_latlonLabels->Update3D(xmpoint, ympoint, zoom);

        m_latlonLabels->AddToRenderer();
    }
}

//----------------------------------------
CGeoMap* CWorldPlotRenderer::GetGeoMap()
{
  CGeoMap* geoMap = NULL;

  CObList::iterator it;

  for (it = m_actors.begin() ; it != m_actors.end() ; it++)
  {
    geoMap = dynamic_cast<CGeoMap*>(*it);
    if (geoMap != NULL)
    {
      break;
    }

  }

  return geoMap;

}
//----------------------------------------
void CWorldPlotRenderer::OnKeyframeChanged(CKeyframeEvent& event)
{
  CObList::iterator it;

  for (it = m_actors.begin() ; it != m_actors.end() ; it++)
  {
    /*
    CWorldPlotData* pdata = dynamic_cast<CWorldPlotData*>(*it);
    if (pdata == NULL)
    {
      throw CException("ERROR in  CWorldPlotRenderer::OnKeyframeChanged : dynamic_cast<CWorldPlotData*>(*it) returns NULL pointer - "
                   "actors std::list seems to contain objects other than those of the class CWorldPlotData", BRATHL_LOGIC_ERROR);
    }
    */
    CGeoMap* pdata = dynamic_cast<CGeoMap*>(*it);
    if (pdata == NULL)
    {
      continue;
    }

    pdata->OnKeyframeChanged(event.m_frameNumber);

    ResetTextActor();
  }


}
//---------------------------------------- KAVOK
double CWorldPlotRenderer::GetCenterLatitude()
{
    return m_cLati;
}
//----------------------------------------
void CWorldPlotRenderer::SetCenterLatitude(double c)
{
  m_cLati = c;
  CObList::iterator it;

  for (it = m_actors.begin() ; it != m_actors.end() ; it++)
  {
    CWorldPlotData* pdata = dynamic_cast<CWorldPlotData*>(*it);
    if (pdata == NULL)
    {
      throw CException("ERROR in  CWorldPlotRenderer::SetCenterLatitude : dynamic_cast<CWorldPlotData*>(pdata) returns NULL pointer - "
                   "actors std::list seems to contain objects other than those of the class CWorldPlotData", BRATHL_LOGIC_ERROR);
    }
    pdata->SetCenterLatitude(m_cLati);
  }

}
//----------------------------------------
void CWorldPlotRenderer::RemoveActors(CWorldPlotData* pdata)
{
  RemoveActorsSolidColor(pdata);
  RemoveActorsContour(pdata);
}
//----------------------------------------
void CWorldPlotRenderer::RemoveActorsSolidColor(CWorldPlotData* pdata)
{
  if (pdata->GetVtkActor() != NULL)
  {
    m_vtkRend->RemoveActor(pdata->GetVtkActor());
  }
}
//----------------------------------------
void CWorldPlotRenderer::RemoveActorsContour(CWorldPlotData* pdata)
{
  if (pdata->GetVtkContourActor() != NULL)
  {
    m_vtkRend->RemoveActor(pdata->GetVtkContourActor());
  }
  if (pdata->m_plotProperty.m_withContourLabel)
  {
    RemoveActorsContourLabel(pdata);
  }
}
//----------------------------------------
void CWorldPlotRenderer::RemoveActorsContourLabel(CWorldPlotData* pdata)
{
  if (pdata->GetVtkContourLabelActor() != NULL)
  {
    m_vtkRend->RemoveActor(pdata->GetVtkContourLabelActor());
  }

}
//----------------------------------------
void CWorldPlotRenderer::AddActors(CWorldPlotData* pdata)
{
  AddActorsSolidColor(pdata);
  AddActorsContour(pdata);

}
//----------------------------------------
void CWorldPlotRenderer::AddActorsContour(CWorldPlotData* pdata)
{
  if (pdata->GetVtkContourActor() != NULL)
  {
    m_vtkRend->AddActor(pdata->GetVtkContourActor());
  }

  if (pdata->m_plotProperty.m_withContourLabel)
  {
    AddActorsContourLabel(pdata);
  }

}
//----------------------------------------
void CWorldPlotRenderer::AddActorsContourLabel(CWorldPlotData* pdata)
{
  if (pdata->GetVtkContourLabelActor() != NULL)
  {
    m_vtkRend->AddActor(pdata->GetVtkContourLabelActor());
  }

}
//----------------------------------------
void CWorldPlotRenderer::AddActorsSolidColor(CWorldPlotData* pdata)
{
  if (pdata->GetVtkActor() != NULL)
  {
    m_vtkRend->AddActor(pdata->GetVtkActor());
  }

}
//----------------------------------------
void CWorldPlotRenderer::RemoveActors2D(CWorldPlotData* pdata)
{
  RemoveActors2DSolidColor(pdata);
  RemoveActors2DContour(pdata);
}
//----------------------------------------
void CWorldPlotRenderer::RemoveActors2DSolidColor(CWorldPlotData* pdata)
{
  if (pdata->GetVtkActor2D() != NULL)
  {
    m_vtkRend->RemoveActor2D(pdata->GetVtkActor2D());
  }

}
//----------------------------------------
void CWorldPlotRenderer::RemoveActors2DContour(CWorldPlotData* pdata)
{
  if (pdata->GetVtkContourActor2D() != NULL)
  {
    m_vtkRend->RemoveActor2D(pdata->GetVtkContourActor2D());
  }
  if (pdata->m_plotProperty.m_withContourLabel)
  {
    RemoveActorsContourLabel(pdata);
  }

}
//----------------------------------------
void CWorldPlotRenderer::AddActors2D(CWorldPlotData* pdata)
{
  AddActors2DSolidColor(pdata);
  AddActors2DContour(pdata);
}
//----------------------------------------
void CWorldPlotRenderer::AddActors2DContour(CWorldPlotData* pdata)
{
  if (pdata->GetVtkContourActor2D() != NULL)
  {
    m_vtkRend->AddActor2D(pdata->GetVtkContourActor2D());
  }

  if (pdata->m_plotProperty.m_withContourLabel)
  {
    AddActorsContourLabel(pdata);
  }

}
//----------------------------------------
void CWorldPlotRenderer::AddActors2DSolidColor(CWorldPlotData* pdata)
{
  if (pdata->GetVtkActor2D() != NULL)
  {
    m_vtkRend->AddActor2D(pdata->GetVtkActor2D());
  }

}

//----------------------------------------
//KAVOK
double CWorldPlotRenderer::GetCenterLongitude()
{
    return m_cLong;
}

//----------------------------------------
void CWorldPlotRenderer::SetCenterLongitude(double c)
{
  m_cLong = c;
  CObList::iterator it;

  for (it = m_actors.begin() ; it != m_actors.end() ; it++)
  {
    CWorldPlotData* pdata = dynamic_cast<CWorldPlotData*>(*it);
    if (pdata == NULL)
    {
      throw CException("ERROR in  CWorldPlotRenderer::SetCenterLongitude : dynamic_cast<CWorldPlotData*>(pdata) returns NULL pointer - "
                   "actors std::list seems to contain objects other than those of the class CWorldPlotData", BRATHL_LOGIC_ERROR);
    }
    pdata->SetCenterLongitude(m_cLong);
  }

}
//----------------------------------------
void CWorldPlotRenderer::SetProjectionByName(const std::string& projName)
{
  int32_t nprojection = CMapProjection::GetInstance()->NameToId(projName);

  if (nprojection <= 0)
  {
    CException e(CTools::Format("ERROR in CWorldPlotRenderer::SetProjectionByName - Invalid projection name '%s' - id %d",
                                projName.c_str(), nprojection),
                 BRATHL_LOGIC_ERROR);
    throw(e);
  }

  SetProjection(nprojection);
}

//
void CWorldPlotRenderer::SetGlyphs(bool val)
{
   bool cleaned = false;

  CObList::iterator it;

  if (m_projection == VTK_PROJ2D_3D)
    {
      RemoveActorsFromRenderer();
    }
    else
    {
      RemoveActors2DFromRenderer();
    }

    cleaned = true;


  // Set Projection for all data
  for (it = m_actors.begin() ; it != m_actors.end() ; it++)
  {
    CGeoVelocityMap* pdata = dynamic_cast<CGeoVelocityMap*>(*it);
    if (pdata != NULL)
    {
        pdata->SetIsGlyph(val);
    }
  }


  if (cleaned)
  {
    if (m_projection == VTK_PROJ2D_3D)
    {
      AddActorsToRenderer();
    }
    else
    {
      AddActors2DToRenderer();
    }
  }

  // TextActor on top
  AddTextActors();


}

//----------------------------------------
void CWorldPlotRenderer::ResetActors()
{
    if (m_projection == VTK_PROJ2D_3D)
    {
      RemoveActorsFromRenderer();
    }
    else
    {
      RemoveActors2DFromRenderer();
    }

    if (m_projection == VTK_PROJ2D_3D)
    {
      AddActorsToRenderer();
    }
    else
    {
      AddActors2DToRenderer();
    }

    AddTextActors();

}
//----------------------------------------
void CWorldPlotRenderer::SetProjection(int32_t proj)
{
  bool cleaned = false;

  CObList::iterator it;

  if ( (proj == VTK_PROJ2D_3D) ||  (m_projection == VTK_PROJ2D_3D) || (m_projection == 0) )
  {

    if (m_projection == VTK_PROJ2D_3D)
    {
      RemoveActorsFromRenderer();
    }
    else
    {
      RemoveActors2DFromRenderer();
    }

    cleaned = true;
  }

  // Set Projection for all data
  for (it = m_actors.begin() ; it != m_actors.end() ; it++)
  {
    CWorldPlotData* pdata = dynamic_cast<CWorldPlotData*>(*it);
    if (pdata == NULL)
    {
      throw CException("ERROR when std::set projection in CWorldPlotRenderer::SetProjection : dynamic_cast<CWorldPlotData*>(pdata) returns NULL pointer - "
                   "actors std::list seems to contain objects other than those of the class CWorldPlotData", BRATHL_LOGIC_ERROR);
    }
    pdata->SetProjection(proj);
  }


  if (cleaned)
  {
    if (proj == VTK_PROJ2D_3D)
    {
      AddActorsToRenderer();
    }
    else
    {
      AddActors2DToRenderer();
    }
  }

  // TextActor on top
  AddTextActors();

  if (proj == VTK_PROJ2D_3D)
  {
      m_vtkRend->SetBackground(0, 0, 0);

      if (m_textActor != NULL)
      {
        vtkTextMapper *tMapper  = dynamic_cast<vtkTextMapper *>(m_textActor->GetMapper());
        if ( tMapper != NULL )
        {
            tMapper->GetTextProperty()->SetColor(1, 1, 1);
        }
      }
      if (m_cameraResetted)
      {
        m_vtkRend->ResetCamera();
        vtkCamera* camera = m_vtkRend->GetActiveCamera();

        camera->SetRoll(-90);
        camera->Elevation(-30);
        camera->Zoom(1.5);
        //SaveState();
        m_cameraResetted = false;
      }
  }
  else
  {
    m_vtkRend->SetBackground(1, 1, 1);
    if (m_textActor != NULL)
    {
      vtkTextMapper *tMapper = dynamic_cast<vtkTextMapper *>(m_textActor->GetMapper());
      if ( tMapper != NULL )
      {
        tMapper->GetTextProperty()->SetColor(0, 0, 0);
      }
    }
  }

  m_projection = proj;
  SetCenterLatitude(m_cLati);
  SetCenterLongitude(m_cLong);


}

//----------------------------------------
void CWorldPlotRenderer::AddActorsToRenderer()
{
  AddActorsBackgroundToRenderer();
  AddActorsSolidColorToRenderer();
  AddActorsContourToRenderer();
  AddTextActors();

}
//----------------------------------------
void CWorldPlotRenderer::AddActorsBackgroundToRenderer()
{
  CObList::iterator it;
  for (it = m_actors.begin() ; it != m_actors.end() ; it++)
  {
    CWorldPlotData* pdata = dynamic_cast<CWorldPlotData*>(*it);
    if (pdata == NULL)
    {
      throw CException("ERROR in CWorldPlotRenderer::AddActorsBackgroundToRenderer : dynamic_cast<CWorldPlotData*>(*it) returns NULL pointer - "
                   "actors std::list seems to contain objects other than those of the class CWorldPlotData", BRATHL_LOGIC_ERROR);
    }

    CGeoMap* geoMap = dynamic_cast<CGeoMap*>(pdata);
    if (geoMap == NULL)
    {
      AddActorsSolidColor(pdata);
    }
  }
}
//----------------------------------------
void CWorldPlotRenderer::AddActorsSolidColorToRenderer()
{
  CObList::iterator it;
  for (it = m_actors.begin() ; it != m_actors.end() ; it++)
  {
    CGeoMap* geoMap = dynamic_cast<CGeoMap*>(*it);
    if (geoMap == NULL)
    {
      continue;
    }
    if (geoMap->m_plotProperty.m_solidColor)
    {
      AddActorsSolidColor(geoMap);
    }
  }
}//----------------------------------------
void CWorldPlotRenderer::AddActorsContourToRenderer()
{
  CObList::iterator it;
  for (it = m_actors.begin() ; it != m_actors.end() ; it++)
  {
    CGeoMap* geoMap = dynamic_cast<CGeoMap*>(*it);
    if (geoMap == NULL)
    {
      continue;
    }
    if (geoMap->m_plotProperty.m_withContour)
    {
      AddActorsContour(geoMap);
    }
  }
}

//----------------------------------------
void CWorldPlotRenderer::RemoveActorsFromRenderer()
{
  CObList::iterator it;
  for (it = m_actors.begin() ; it != m_actors.end() ; it++)
  {
    CWorldPlotData* pdata = dynamic_cast<CWorldPlotData*>(*it);
    if (pdata == NULL)
    {
      throw CException("ERROR when cleaning in CWorldPlotRenderer::RemoveActorsFromRenderer : dynamic_cast<CWorldPlotData*>(pdata) returns NULL pointer - "
                   "actors std::list seems to contain objects other than those of the class CWorldPlotData", BRATHL_LOGIC_ERROR);
    }
    RemoveActors(pdata);
  }

}
//----------------------------------------
void CWorldPlotRenderer::AddActors2DToRenderer()
{
  // Add Solid color actors first
  // Add contour actors after solid color otherwise solid color overlay contour
  // NB: it seems that methods SetLayerNumber (of vtkActor2D class) and RenderOverlay (of vtkAActor2DCollection class)
  // doesn't work

  AddActorsBackground2DToRenderer();
  AddActorsSolidColor2DToRenderer();
  AddActorsContour2DToRenderer();
  AddTextActors();
}
//----------------------------------------
void CWorldPlotRenderer::AddActorsBackground2DToRenderer()
{
  CObList::iterator it;
  for (it = m_actors.begin() ; it != m_actors.end() ; it++)
  {
    CWorldPlotData* pdata = dynamic_cast<CWorldPlotData*>(*it);
    if (pdata == NULL)
    {
      throw CException("ERROR in CWorldPlotRenderer::AddActors2DToRenderer : dynamic_cast<CWorldPlotData*>(*it) returns NULL pointer - "
                   "actors std::list seems to contain objects other than those of the class CWorldPlotData", BRATHL_LOGIC_ERROR);
    }

    CGeoMap* geoMap = dynamic_cast<CGeoMap*>(pdata);
    if (geoMap == NULL)
    {
      AddActors2DSolidColor(pdata);
    }
  }
}
//----------------------------------------
void CWorldPlotRenderer::AddActorsSolidColor2DToRenderer()
{
  CObList::iterator it;
  for (it = m_actors.begin() ; it != m_actors.end() ; it++)
  {
    CGeoMap* geoMap = dynamic_cast<CGeoMap*>(*it);
    if (geoMap == NULL)
    {
      continue;
    }
    if (geoMap->m_plotProperty.m_solidColor)
    {
      AddActors2DSolidColor(geoMap);
    }
  }
}
//----------------------------------------
void CWorldPlotRenderer::AddActorsContour2DToRenderer()
{
  CObList::iterator it;
  for (it = m_actors.begin() ; it != m_actors.end() ; it++)
  {
    CGeoMap* geoMap = dynamic_cast<CGeoMap*>(*it);
    if (geoMap == NULL)
    {
      continue;
    }
    if (geoMap->m_plotProperty.m_withContour)
    {
      AddActors2DContour(geoMap);
    }
  }

}
//----------------------------------------
void CWorldPlotRenderer::RemoveActors2DFromRenderer()
{
  CObList::iterator it;
  for (it = m_actors.begin() ; it != m_actors.end() ; it++)
  {
    CWorldPlotData* pdata = dynamic_cast<CWorldPlotData*>(*it);
    if (pdata == NULL)
    {
      throw CException("ERROR when cleaning in CWorldPlotRenderer::RemoveActors2DFromRenderer : dynamic_cast<CWorldPlotData*>(pdata) returns NULL pointer - "
                   "actors std::list seems to contain objects other than those of the class CWorldPlotData", BRATHL_LOGIC_ERROR);
    }
    RemoveActors2D(pdata);
  }
  m_latlonLabels->RemoveFromRenderer();

}
//----------------------------------------
void CWorldPlotRenderer::AddTextActors()
{
  // TextActor on top
  if (m_textActor != NULL)
  {
    m_vtkRend->RemoveActor2D(m_textActor);
    m_vtkRend->AddActor2D(m_textActor);
  }

  // add the Lat/Lon labels to renderer
  m_latlonLabels->AddToRenderer();
}
//----------------------------------------
void CWorldPlotRenderer::RenderOverlay()
{

  if (m_projection != VTK_PROJ2D_3D)
  {
    vtkActor2DCollection* actor2DCollection = m_vtkRend->GetActors2D();
    actor2DCollection->RenderOverlay(m_vtkRend);

  }

}
