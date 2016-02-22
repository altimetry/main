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
    #pragma implementation "ZFXYPlotPanel.h"
#endif

#ident "$Id: ZFXYPlotPanel.cpp,v 1.57 2006/05/11 12:24:38 dearith Exp $"

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
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

#include "vtkCallbackCommand.h"
#include "vtkLookupTable.h"
#include "PlotData/vtkZFXYPlotFilter.h"

#include "wxInterface.h"
#ifdef CursorShape  //collsion Qt X
#undef CursorShape
#endif
#include "ZFXYPlotPropertyPanel.h"
#include "ZFXYPlotPanel.h"

long ID_ZFXYPLOTPANEL_VTK_INTERACTOR;
long ID_ZFXYPLOTPANEL_SPLIT;
long ID_ZFXYPLOTPANEL_PROPPANEL;
long ID_ZFXYPLOTPANEL_PROPTAB;
//long ID_ZFXYPLOTPANEL_NOTEBOOK;

double CZFXYPlotPanel::m_percentSizeScalarBar = 0.1;

// WDR: class implementations

//----------------------------------------------------------------------------
// CZFXYPlotPanel
//----------------------------------------------------------------------------

// WDR: event table for CZFXYPlotPanel

BEGIN_EVENT_TABLE(CZFXYPlotPanel,wxPanel)
END_EVENT_TABLE()

CZFXYPlotPanel::CZFXYPlotPanel()
{
  Init();
}

//----------------------------------------
CZFXYPlotPanel::CZFXYPlotPanel( wxWindow *parent, wxWindowID id, CZFXYPlotProperties* plotProperty,
                                  const wxPoint &position, const wxSize& size, long style )
                        //: wxPanel( parent, id, position, size, style ), --> called in Create
{

  Init();

  bool bOk = Create(parent, id, plotProperty, position, size, style);
  if (bOk == false)
  {
    CException e("ERROR in CZFXYPlotPanel ctor - Unenable to create wxWidgets (Create method returns false)", BRATHL_SYSTEM_ERROR);
    throw(e);
  }

}
//----------------------------------------

CZFXYPlotPanel::~CZFXYPlotPanel()
{

  if (m_vtkWidget != NULL)
  {
    m_vtkWidget->Delete();
    m_vtkWidget = NULL;
  }

  if ( m_polyDataMapper != NULL )
  {
	  m_polyDataMapper->Delete();
	  m_polyDataMapper = NULL;
  }

  if (m_plotActor != NULL)
  {
    m_plotActor->Delete();
    m_plotActor = NULL;
  }


  if (m_vtkRend != NULL)
  {
    m_vtkRend->Delete();
    m_vtkRend = NULL;
  }
  
  if (m_styleZFXY != NULL)
  {
    m_styleZFXY->Delete();
    m_styleZFXY = NULL;
  }


  
  if (m_plotData != NULL)
  {
    delete m_plotData;
    m_plotData = NULL;
  }

  if (m_vtkCharEventCallback != NULL)
  {
    m_vtkCharEventCallback->Delete();
    m_vtkCharEventCallback = NULL;
  }


}
//----------------------------------------
void CZFXYPlotPanel::Init()
{
  m_splitPanel = NULL;
  m_propertyPanel = NULL;
  m_vtkWidget = NULL;
  m_plotPropertyTab = NULL;
  m_propertyNotebook = NULL;
  m_propertyPanelSizer = NULL;
  m_splitPanelSizer = NULL;
  m_vSizer = NULL;

  m_animationToolbar = NULL;

  m_plotActor = NULL;
  m_vtkRend = NULL;
  m_styleZFXY = NULL;

  m_plotData = NULL;
  m_vtkCharEventCallback = CZFXYVtkCharEventCallback::New(this);
  m_polyDataMapper = NULL;

  m_finished = false;
  m_colorBarShowing = false;

  m_zfxyPlotData.SetDelete(false);


}

//----------------------------------------
bool CZFXYPlotPanel::Create( wxWindow *parent, wxWindowID id, CZFXYPlotProperties* plotProperty,
                              const wxPoint &position, const wxSize& size, long style )
{
  bool bOk = true;

  if (plotProperty != NULL)
  {
    m_plotProperty = *plotProperty;
  }

  wxPanel::Create( parent, id, position, size, style );

  bOk = CreateControls();
  if (bOk == false)
  {
    return false;
  }
  CreateLayout();
  InstallEventListeners();
  return true;

}

//----------------------------------------
bool CZFXYPlotPanel::CreateControls()
{

  ID_ZFXYPLOTPANEL_VTK_INTERACTOR = ::wxNewId();
  ID_ZFXYPLOTPANEL_SPLIT = ::wxNewId();

  m_splitPanel = new wxSplitterWindow(this, ID_ZFXYPLOTPANEL_SPLIT, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE);
  m_splitPanel->SetMinimumPaneSize(50);

  m_polyDataMapper = vtkPolyDataMapper2D::New();

  m_plotActor = vtkZFXYPlotActor::New();
  m_plotActor->GetProperty()->SetColor(0.0, 0.0, 0.0);
  m_plotActor->SetMapper(m_polyDataMapper);

  m_vtkRend = vtkRenderer::New();
  m_vtkRend->InteractiveOn();
  m_vtkRend->SetBackground(1, 1, 1);
  m_vtkRend->AddActor2D(m_plotActor);

  m_styleZFXY = vtkInteractorStyleZFXYPlot::New();


  m_vtkWidget = new wxVTKRenderWindowInteractor(m_splitPanel, ID_ZFXYPLOTPANEL_VTK_INTERACTOR);
  m_vtkWidget->UseCaptureMouseOn();
  m_vtkWidget->GetRenderWindow()->SetPointSmoothing(true);
  m_vtkWidget->GetRenderWindow()->SetLineSmoothing(true);
  m_vtkWidget->GetRenderWindow()->SetPolygonSmoothing(true);
  m_vtkWidget->GetRenderWindow()->AddRenderer(m_vtkRend);
  m_vtkWidget->SetInteractorStyle(m_styleZFXY);


  CreatePropertyPanel();
  
  m_splitPanel->Initialize(m_vtkWidget);

  m_animationToolbar = new CAnimationToolbar(this);

  return true;
}

//----------------------------------------
void CZFXYPlotPanel::CreatePropertyPanel()
{
  ID_ZFXYPLOTPANEL_PROPPANEL = ::wxNewId();

  // We create a parent wxWindow for the notebook to be able to give a
  // proper background colour for the area behind the tabs
  m_propertyPanel = new wxWindow(m_splitPanel, ID_ZFXYPLOTPANEL_PROPPANEL);
  m_propertyNotebook = new wxNotebook(m_propertyPanel, ID_ZFXYMAINNOTEBOOK, wxDefaultPosition, wxDefaultSize, 0 );

  m_plotPropertyTab = new CZFXYPlotPropertyPanel(m_propertyNotebook, m_plotActor, m_plotData, m_vtkWidget, ID_ZFXYPLOTPANEL_PROPTAB);
  m_propertyNotebook->AddPage(m_plotPropertyTab, "Properties");

}

//----------------------------------------
void CZFXYPlotPanel::CreateLayout()
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
bool CZFXYPlotPanel::Destroy()
{
  m_finished = true;
  m_animationToolbar->Pause();

  return wxPanel::Destroy();
}

//----------------------------------------
void CZFXYPlotPanel::ShowAnimationToolbar(bool showIt)
{
  m_vSizer->Show(m_animationToolbar, showIt);
  Layout();
}
//----------------------------------------
void CZFXYPlotPanel::ShowPropertyPanel(bool showIt)
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
void CZFXYPlotPanel::ShowColorBar(bool showIt)
{
  m_colorBarShowing = showIt;
  UpdateRender();

}
//----------------------------------------
int32_t CZFXYPlotPanel::CountColorBarRender()
{
  int32_t count = 0;  

  CObList::iterator it;

  for (it = m_zfxyPlotData.begin() ; it != m_zfxyPlotData.end() ; it++)
  {
    VTK_CZFXYPlotData* pdata = dynamic_cast<VTK_CZFXYPlotData*>(*it);
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
void CZFXYPlotPanel::UpdateColorBarRender(bool show)
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

  CObList::reverse_iterator it;
  int32_t index = 0;  

  for (it = m_zfxyPlotData.rbegin() ; it != m_zfxyPlotData.rend() ; it++)
  {
    VTK_CZFXYPlotData* pdata = dynamic_cast<VTK_CZFXYPlotData*>(*it);
    if (pdata == NULL)
    {
      continue;
    }

    if (pdata->GetColorBarRenderer() != NULL)
    {
      pdata->GetColorBarRenderer()->SetBackground(r, g, b);
      pdata->GetColorBarRenderer()->ResetTextActor(pdata);
      if (show)
      {
        yMax =  CZFXYPlotPanel::m_percentSizeScalarBar * (index + 1);
        pdata->GetColorBarRenderer()->GetVtkRenderer()->SetViewport(0, yMin, 1, yMax);
      }
      else
      {
        pdata->GetColorBarRenderer()->GetVtkRenderer()->SetViewport(0, 0, 0, 0);
      }
      
      //pdata->GetColorBarRenderer()->GetVtkRenderer()->Print(*CTrace::GetDumpContext());

    }
    yMin = yMax;
    index++;

  }

}
//----------------------------------------
void CZFXYPlotPanel::UpdateRender()
{

  UpdateColorBarRender(m_colorBarShowing);

  if (m_colorBarShowing)
  {
    double relLength = CZFXYPlotPanel::m_percentSizeScalarBar * CountColorBarRender();
    m_vtkRend->SetViewport(0, relLength, 1, 1);
  }
  else
  {
    m_vtkRend->SetViewport(0, 0, 1, 1);
  }

  m_vtkWidget->Refresh();
  
  Refresh();
}

//----------------------------------------
void CZFXYPlotPanel::Update2D()
{
  m_plotActor->Update();
  UpdateRender();
}

//----------------------------------------
void CZFXYPlotPanel::AddData(VTK_CZFXYPlotData* pdata, wxProgressDialog* dlg)
{
  
  if (pdata == NULL)
  {
    return;
  }

  wxString szMsg = wxString::Format("Displaying data: %s ...", pdata->GetDataTitle().c_str());
    
  if (dlg != NULL)
  {
    dlg->Update(0, szMsg);
  }

  pdata->SetRenderer(m_vtkRend);
  m_zfxyPlotData.Insert(pdata);

  m_plotActor->AddInput(pdata);


  if (pdata->GetColorBarRenderer() != NULL)
  {
    m_vtkWidget->GetRenderWindow()->AddRenderer(pdata->GetColorBarRenderer()->GetVtkRenderer());
  }

  wxString textLayer = wxString::Format("%s", pdata->GetDataName().c_str());

  GetZfxylayerchoice()->Append(textLayer, static_cast<void*>(pdata));
  m_plotPropertyTab->SetCurrentLayer(0);


  if (dlg != NULL)
  {
    dlg->Update(1);
  }

  int32_t nFrames = pdata->GetNrMaps();

  m_animationToolbar->SetMaxFrame(nFrames);


  if (dlg != NULL)
  {
    dlg->Update(3);
  }

  

  m_plotPropertyTab->UpdateTitleControls();


  UpdateRender();

    // Fix for Windows refusing to draw initial screen:
#ifdef WIN32
  //Layout();
#endif


}

//----------------------------------------
VTK_CZFXYPlotData* CZFXYPlotPanel::GetCurrentLayer()
{
  return m_plotPropertyTab->GetCurrentLayer();
}
//----------------------------------------
bool CZFXYPlotPanel::IsNumberOfMapsEquals(int32_t* numberOfMaps)
{
  VTK_CZFXYPlotData* pdata = NULL;
  CObList::iterator it;
  if (numberOfMaps != NULL)
  {
    *numberOfMaps = -1;
  }

  if (m_zfxyPlotData.size() == 0)
  {
    return false;
  }
  
  int32_t nMaps = -1;

  for (it = m_zfxyPlotData.begin() ; it != m_zfxyPlotData.end() ; it++)
  {
    pdata = dynamic_cast<VTK_CZFXYPlotData*>(*it);
    if (pdata != NULL)
    {
      if (nMaps < 0)
      {
        nMaps = pdata->GetNrMaps();
      }
      if (nMaps != pdata->GetNrMaps())
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
void CZFXYPlotPanel::AdjustSplitPanelSashPosition()
{
  // Update sash position in splitter window
  // Try to keep the size of the property pane the same
  //  sashposition = - (self.propertypanel.GetSize()[0] + self.splitpanel.GetSashSize())
  wxSize size = m_propertyPanel->GetSize();
  int32_t sashPosition = - (size.GetWidth() + m_splitPanel->GetSashSize());
  m_splitPanel->SetSashPosition(sashPosition);
}
//----------------------------------------
void CZFXYPlotPanel::EvtSize(wxWindow& window, const wxSizeEventFunction& method, wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_SIZE,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);

}

//----------------------------------------
void CZFXYPlotPanel::InstallEventListeners()
{
  m_vtkWidget->AddObserver(vtkCommand::CharEvent, m_vtkCharEventCallback);

  //CZFXYVtkRenderEventCallback* vtkRenderEventCallback = CZFXYVtkRenderEventCallback::New(this);
  //m_vtkWidget->AddObserver(vtkCommand::RenderEvent, vtkRenderEventCallback);
  //vtkRenderEventCallback->Delete();

  /*
  CZFXYVtkUpdate2DCallback* vtkUpdate2DCallback = CZFXYVtkUpdate2DCallback::New(this);
  m_styleZFXY->AddObserver(vtkInteractorStyleZFXYPlot::EndZoomEvent, vtkUpdate2DCallback);
  m_styleZFXY->AddObserver(vtkInteractorStyleZFXYPlot::EndPanEvent, vtkUpdate2DCallback);
  vtkUpdate2DCallback->Delete();
  */

  vtkCallbackCommand* callback = vtkCallbackCommand::New();
  callback->SetCallback(&CZFXYPlotPanel::SetFocusVtkWidget);
  m_vtkWidget->AddObserver(vtkCommand::MouseMoveEvent, callback);
  callback->Delete();

  /*
  vtkCellPickerEventCallback* pickerCallback = vtkCellPickerEventCallback::New();
  m_picker->AddObserver(vtkCommand::EndPickEvent, pickerCallback);
  pickerCallback->Delete();
  
  vtkMousePickerEventCallback* mousePickerCallback = vtkMousePickerEventCallback::New(m_picker);
  m_vtkWidget->AddObserver(vtkCommand::LeftButtonPressEvent , mousePickerCallback);
  
  m_vtkWidget->SetPicker(m_picker);
  */
  CZFXYPlotPanel::EvtSize(*m_vtkWidget,
                           (wxSizeEventFunction)&CZFXYPlotPanel::OnVtkWidgetSize, NULL, this);
    
  CZFXYPlotPanel::EvtSize(*m_splitPanel,
                           (wxSizeEventFunction)&CZFXYPlotPanel::OnSplitPanelSize, NULL, this);

  CZFXYPlotPanel::EvtSize(*m_propertyPanel,
                           (wxSizeEventFunction)&CZFXYPlotPanel::OnPropertyPanelSize, NULL, this);

  CZFXYPlotPanel::EvtSize(*m_plotPropertyTab,
                           (wxSizeEventFunction)&CZFXYPlotPanel::OnPlotPropertyTabSize, NULL, this);
  
  CZFXYPlotPropertyPanel::EvtRangeChanged(*this, 
                                       (CZFXYRangeChangedEventFunction)&CZFXYPlotPanel::OnRangeChanged, NULL, this);

  CZFXYPlotPropertyPanel::EvtNumLabelsChanged(*this, 
                                           (CZFXYNumLabelsChangedEventFunction)&CZFXYPlotPanel::OnNumlabelsChanged, NULL, this);


  CAnimationToolbar::EvtKeyframeChanged(*this, 
                                        (CKeyframeEventFunction)&CZFXYPlotPanel::OnKeyframeChanged, NULL, this);

  CAnimationToolbar::EvtAutoZoomDataRange(*this, 
                                        (CAutoZoomDataRangeEventFunction)&CZFXYPlotPanel::OnAutoZoomDataRange, NULL, this);

  CZFXYPlotPropertyPanel::EvtUpdateRangeCommand(*this, 
                                          (CZFXYUpdateRangeEventFunction)&CZFXYPlotPanel::OnUpdateRange, NULL, this);

  CLUTPanel::EvtLutFrameClose(*this,
                              -1,
                              (CLutFrameCloseEventFunction)&CZFXYPlotPanel::OnLutFrameClose);


  CZFXYContourPropPanel::EvtContourPropFrameClose(*this,
                              -1,
                              (CZFXYContourPropFrameCloseEventFunction)&CZFXYPlotPanel::OnContourPropFrameClose);

}

//----------------------------------------
void CZFXYPlotPanel::OnLutFrameClose( CLutFrameCloseEvent &event )
{
  wxPostEvent(GetParent(), event);
}
//----------------------------------------
void CZFXYPlotPanel::OnContourPropFrameClose( CZFXYContourPropFrameCloseEvent &event )
{
  wxPostEvent(GetParent(), event);
}
//----------------------------------------
void CZFXYPlotPanel::OnVtkWidgetSize(wxSizeEvent& event)
{

  UpdateRender();
  event.Skip();

}

//----------------------------------------
void CZFXYPlotPanel::OnAutoZoomDataRange(CAutoZoomDataRangeEvent& event)
{
  m_plotPropertyTab->SetZoomToDataRange(event.m_autoZoom);
  m_plotPropertyTab->FullView();
}


/*
//----------------------------------------
void CZFXYPlotPanel::PrintPoints(std::ostream& os)
{
  if (m_plotRenderer->GetZFXYPlotData() == NULL)
  {
    return;
  }

  
  vtkPolyData *outputSource = (vtkPolyData *) m_plotRenderer->GetZFXYPlotData()->GetVtkFilter()->GetInput();
  vtkPolyData *outputFilter = (vtkPolyData *) m_plotRenderer->GetZFXYPlotData()->GetVtkFilter()->GetOutput();
  vtkPolyData *outputTransform = (vtkPolyData *) m_plotRenderer->GetZFXYPlotData()->GetVtkTransform()->GetOutput();
  
  //outputSource = m_plotRenderer->GetZFXYPlotData()->GetVtkMapper2D()->GetInput();

  outputFilter = outputSource;
  outputTransform = outputSource;

  if (m_plotRenderer->GetZFXYPlotData()->GetVtkMapper2D() == NULL)
  {
    return;
  }

  vtkCoordinate* c = m_plotRenderer->GetZFXYPlotData()->GetVtkMapper2D()->GetTransformCoordinate();
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
*/

//----------------------------------------
void CZFXYPlotPanel::OnRangeChanged(CZFXYRangeChangedEvent& event)
{
  RangeChanged(event);
  UpdateRender();
}
//----------------------------------------
void CZFXYPlotPanel::RangeChanged(CZFXYRangeChangedEvent& event)
{

}
//----------------------------------------
void CZFXYPlotPanel::OnNumlabelsChanged(CZFXYNumLabelsChangedEvent& event)
{
  NumlabelsChanged(event);
  UpdateRender();
}
//----------------------------------------
void CZFXYPlotPanel::NumlabelsChanged(CZFXYNumLabelsChangedEvent& event)
{
}
//----------------------------------------
void CZFXYPlotPanel::OnSplitPanelSize(wxSizeEvent& event)
{
  if (m_splitPanel->IsSplit())
  {
    AdjustSplitPanelSashPosition();
  }

  event.Skip();
}
//----------------------------------------
void CZFXYPlotPanel::OnPropertyPanelSize(wxSizeEvent& event)
{
  m_propertyPanel->Layout();
  event.Skip();
}

//----------------------------------------
void CZFXYPlotPanel::OnPlotPropertyTabSize(wxSizeEvent& event)
{
  m_plotPropertyTab->Layout();
  event.Skip();
}
//----------------------------------------
void CZFXYPlotPanel::OnContourPropChanged( CZFXYContourPropChangedEvent &event )
{
  m_plotPropertyTab->ContourPropChanged(event);
}
//----------------------------------------
void CZFXYPlotPanel::LutChanged( CLutChangedEvent &event )
{
  m_plotPropertyTab->LutChanged(event);
  UpdateRender();
}
//----------------------------------------
void CZFXYPlotPanel::SetFocusVtkWidget(vtkObject* obj, unsigned long, void*, void*)
{
  wxVTKRenderWindowInteractor *Interactor = dynamic_cast<wxVTKRenderWindowInteractor*>(obj);
  Interactor->SetFocus();
}
//----------------------------------------
void CZFXYPlotPanel::UpdateValues()
{

  VTK_CZFXYPlotData* pdata = GetCurrentLayer();
  if (pdata == NULL)
  {
    return;
  }
  vtkZFXYPlotFilter* pdataFilter = NULL;

  double minhv = pdata->GetComputedRangeMin();
  double maxhv = pdata->GetComputedRangeMax();

  int32_t nrMaps = pdata->GetNrMaps();

  for (int32_t iMap = 0 ; iMap < nrMaps; iMap++)
  {
    pdataFilter = dynamic_cast<vtkZFXYPlotFilter*>((*pdata->GetZFXYPlotFilterList())[iMap]);
    pdataFilter->SetMinMappedValue(minhv);
    pdataFilter->SetMaxMappedValue(maxhv);
  }

  CZFXYRangeChangedEvent ev(GetId(),
                        minhv,
                        maxhv);
  wxPostEvent(m_plotPropertyTab, ev);
  wxPostEvent(this, ev);


}

//----------------------------------------
void CZFXYPlotPanel::OnKeyframeChanged(CKeyframeEvent& event)
{
 
  if (m_finished)
  {
    return;
  }


  m_plotActor->ReplaceInput(event.m_frameNumber);
  m_plotPropertyTab->SetCurrentFrame(event.m_frameNumber);

  if (m_plotPropertyTab->IsZoomToDataRange())
  {
      m_plotPropertyTab->FullView();
  }


  UpdateRender();

  //wxPostEvent(m_plotPropertyTab, event);

}

//----------------------------------------
void CZFXYPlotPanel::OnUpdateRange(CZFXYUpdateRangeEvent& event)
{
  UpdateValues();
}

// WDR: handler implementations for CZFXYPlotPanel




//-------------------------------------------------------------
//------------------- vtkPointPickerEventCallback class --------------------
//-------------------------------------------------------------
/*
void vtkPointPickerEventCallback::Execute(vtkObject *caller, unsigned long eventid, void* calldata)
{
  vtkPointPicker *picker = vtkPointPicker::SafeDownCast(caller);

  if (eventid == vtkCommand::EndPickEvent)
  {
    vtkIdType pointId = picker->GetPointId();

    if (pointId < 0)
    {
      return;
    }

    double selPoint[3];
    double pickPos[3];
    double mapperPos[3];
    //double pCoords[3];
    picker->GetSelectionPoint(selPoint);
    picker->GetPickPosition(pickPos);
    picker->GetMapperPosition(mapperPos);
    //picker->GetPCoords(pCoords);
    vtkPoints* points = picker->GetPickedPositions();
    vtkDataSet* dataSet = picker->GetDataSet();
    vtkPointData* pointData = dataSet->GetPointData();
    vtkDataArray* dataArray = pointData->GetScalars();
    double value = dataArray->GetTuple1(pointId);
    vtkActor2D* actor2D = picker->GetActor2D();
    vtkOutputWindow::GetInstance()->DisplayText((const char*) calldata);
  }

}
*/
/*
//-------------------------------------------------------------
//------------------- vtkCellPickerEventCallback class --------------------
//-------------------------------------------------------------

void vtkCellPickerEventCallback::Execute(vtkObject *caller, unsigned long eventid, void* calldata)
{
  vtkCellPicker *picker = vtkCellPicker::SafeDownCast(caller);

  if (eventid == vtkCommand::EndPickEvent)
  {
    vtkIdType cellId = picker->GetCellId();

    if (cellId < 0)
    {
      return;
    }

    double selPoint[3];
    double pickPos[3];
    double mapperPos[3];
    double pCoords[3];
    picker->GetSelectionPoint(selPoint);
    picker->GetPickPosition(pickPos);
    picker->GetMapperPosition(mapperPos);
    picker->GetPCoords(pCoords);
    vtkPoints* points = picker->GetPickedPositions();
    vtkDataSet* dataSet = picker->GetDataSet();
    vtkCellData* cellData = dataSet->GetCellData();
    vtkDataArray* dataArray = cellData->GetScalars();
    double value = dataArray->GetTuple1(cellId);
    
    //for (vtkIdType ptId = 0; ptId < points->GetNumberOfPoints(); ptId++)
    //{
    //  double xyz[3];
    //  points->GetPoint(ptId, xyz);
    //  int ptType = points->GetDataType();
    //  vtkDataArray* array = points->GetData();
    //  vtkIdType nOfT = array->GetNumberOfTuples();
    //  int nOfC = array->GetNumberOfComponents();

    //  for(int i = 0 ; i < nOfT ; i++)
    //  {
    //    double* d = array->GetTuple(i);
    //    for (int j = 0 ; j < nOfC ; j++)
    //    {
    //      double dj = d[j];
    //    }
    //  }

    //} 
    vtkActor2D* actor2D = picker->GetActor2D();
    vtkOutputWindow::GetInstance()->DisplayText((const char*) calldata);
  }

}
*/

