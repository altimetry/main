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
    #pragma implementation "XYPlotPanel.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "new-gui/Common/tools/Trace.h"


#include "XYPlotPanel.h"
#include "vtkCallbackCommand.h"

long ID_XYPLOTPANEL_VTK_INTERACTOR;
long ID_XYPLOTPANEL_SPLIT;
long ID_XYPLOTPANEL_PROPPANEL;
long ID_XYPLOTPANEL_PROPTAB;
long ID_XYPLOTPANEL_NOTEBOOK;


// WDR: class implementations

//----------------------------------------------------------------------------
// CXYPlotPanel
//----------------------------------------------------------------------------

// WDR: event table for CXYPlotPanel

BEGIN_EVENT_TABLE(CXYPlotPanel,wxPanel)
END_EVENT_TABLE()


//----------------------------------------
CXYPlotPanel::CXYPlotPanel()
{
  Init();
}

//----------------------------------------
CXYPlotPanel::CXYPlotPanel( wxWindow *parent, wxWindowID id,
                            const wxPoint &position, const wxSize& size, long style ) 
                        //: wxPanel( parent, id, position, size, style ), --> called in Create
{
  Init();

  bool bOk = Create(parent, id, position, size, style);
  if (bOk == false)
  {
    CException e("ERROR in CXYPlotPanel ctor - Unenable to create wxWidgets (Create method returns false)", BRATHL_SYSTEM_ERROR);
    throw(e);
  }
}

//----------------------------------------
CXYPlotPanel::~CXYPlotPanel()
{
  if (m_vtkCharEventCallback != NULL)
  {
    m_vtkCharEventCallback->Delete();
    m_vtkCharEventCallback = NULL;
  }

  if (m_vtkWidget != NULL)
  {
    m_vtkWidget->Delete();
    m_vtkWidget = NULL;
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
  
  if (m_styleXY != NULL)
  {
    m_styleXY->Delete();
    m_styleXY = NULL;
  }
  
  if (m_plotDataCollection != NULL)
  {
    delete m_plotDataCollection;
    m_plotDataCollection = NULL;
  }
  
}
//----------------------------------------
void CXYPlotPanel::Init()
{

  m_splitPanel = NULL;
  m_propertyPanel = NULL;
  m_plotPropertyTab = NULL;
  m_propertyNotebook = NULL;
  //m_propertyNotebookSizer = NULL;
  m_propertyPanelSizer = NULL;
  m_splitPanelSizer = NULL;
  m_vSizer = NULL;

  m_animationToolbar = NULL;

  m_vtkWidget = NULL;
  m_plotActor = NULL;
  m_vtkRend = NULL;
  m_styleXY = NULL;

  m_plotDataCollection = NULL;
  m_vtkCharEventCallback = CVtkXYPlotCharEventCallback::New(this);


  m_finished = false;
  m_multiFrame = false;
  
  setDefaultValue(m_currentXMin);
  setDefaultValue(m_currentXMax);
  setDefaultValue(m_currentYMin);
  setDefaultValue(m_currentYMax);

}
//----------------------------------------
bool CXYPlotPanel::Create( wxWindow *parent, wxWindowID id,
                              const wxPoint &position, const wxSize& size, long style )
{
  bool bOk = true;

  wxPanel::Create( parent, id, position, size, style );

  m_plotDataCollection = new CXYPlotDataCollection();

  bOk = CreateControls();
  if (bOk == false)
  {
    return false;
  }
  CreateLayout();
  InstallEventListeners();

  m_vtkWidget->SetFocus();

  return true;

}

//----------------------------------------
bool CXYPlotPanel::CreateControls()
{

  ID_XYPLOTPANEL_VTK_INTERACTOR = ::wxNewId();
  ID_XYPLOTPANEL_SPLIT = ::wxNewId();

  m_splitPanel = new wxSplitterWindow(this, ID_XYPLOTPANEL_SPLIT, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE);
  m_splitPanel->SetMinimumPaneSize(50);


  m_plotActor = vtkXYPlotActor::New();
  m_plotActor->GetProperty()->SetColor(0.0, 0.0, 0.0);

  m_vtkRend = vtkRenderer::New();
  m_vtkRend->SetBackground(1, 1, 1);
  m_vtkRend->AddActor2D(m_plotActor);

  m_styleXY = vtkInteractorStyleXYPlot::New();


  m_vtkWidget = new wxVTKRenderWindowInteractor(m_splitPanel, ID_XYPLOTPANEL_VTK_INTERACTOR);
  m_vtkWidget->UseCaptureMouseOn();
  m_vtkWidget->GetRenderWindow()->SetPointSmoothing(true);
  m_vtkWidget->GetRenderWindow()->SetLineSmoothing(true);
  m_vtkWidget->GetRenderWindow()->AddRenderer(m_vtkRend);
  m_vtkWidget->SetInteractorStyle(m_styleXY);

  CreatePropertyPanel();
  
  m_splitPanel->Initialize(m_vtkWidget);

  //???self.animationtoolbar = AnimationToolbar.AnimationToolbar(self, -1, 0)
  m_animationToolbar = new CAnimationToolbar(this);


  return true;
}

//----------------------------------------
void CXYPlotPanel::CreatePropertyPanel()
{
  ID_XYPLOTPANEL_PROPPANEL = ::wxNewId();
  ID_XYPLOTPANEL_NOTEBOOK = ::wxNewId(); 

  // We create a parent wxWindow for the notebook to be able to give a
  // proper background colour for the area behind the tabs
  m_propertyPanel = new wxWindow(m_splitPanel, ID_XYPLOTPANEL_PROPPANEL);
  // We must set the background 'explicitly' to have it stick
  m_propertyPanel->SetBackgroundColour(m_propertyPanel->GetBackgroundColour());
  m_propertyNotebook = new wxNotebook(m_propertyPanel, ID_XYPLOTPANEL_NOTEBOOK);

  m_datasetPanelTab = new CDatasetPanel(m_propertyNotebook, m_plotDataCollection);
  
  m_plotPropertyTab = new CXYPlotPropertyPanel(m_propertyNotebook, m_plotActor, m_plotDataCollection, m_vtkWidget, ID_XYPLOTPANEL_PROPTAB);
  
  m_propertyNotebook->AddPage(m_datasetPanelTab, "Datasets");
  m_propertyNotebook->AddPage(m_plotPropertyTab, "Properties");

  m_propertyPanel->Hide();

}

//----------------------------------------
void CXYPlotPanel::CreateLayout()
{

  //m_propertyNotebookSizer = new wxNotebookSizer(m_propertyNotebook);

  //m_propertyNotebook->SetSizerAndFit(m_propertyNotebookSizer);

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
  m_vSizer->Add(m_splitPanel, 6, wxEXPAND);
  ////?????self.vsizer.Add(self.animationtoolbar, 0, wx.EXPAND)
  m_vSizer->Add(m_animationToolbar, 1, wxEXPAND);

  //Initially, hide the animationtoolbar
  ////????self.vsizer.Hide(self.animationtoolbar)
  m_vSizer->Hide(m_animationToolbar);

  SetSizer(m_vSizer);


}

//----------------------------------------
void CXYPlotPanel::InstallEventListeners()
{
  m_vtkWidget->AddObserver(vtkCommand::CharEvent, m_vtkCharEventCallback);

  vtkCallbackCommand* callback = vtkCallbackCommand::New();
  callback->SetCallback(&CXYPlotPanel::SetFocusVtkWidget);
  m_vtkWidget->AddObserver(vtkCommand::MouseMoveEvent, callback);
  callback->Delete();

  CXYPlotPanel::EvtSize(*m_vtkWidget,
                           (wxSizeEventFunction)&CXYPlotPanel::OnVtkWidgetSize, NULL, this);
    

  
  CXYPlotPanel::EvtSize(*m_splitPanel,
                           (wxSizeEventFunction)&CXYPlotPanel::OnSplitPanelSize, NULL, this);

  CXYPlotPanel::EvtSize(*m_propertyPanel,
                           (wxSizeEventFunction)&CXYPlotPanel::OnPropertyPanelSize, NULL, this);


  CXYPlotPanel::EvtSize(*m_plotPropertyTab,
                           (wxSizeEventFunction)&CXYPlotPanel::OnPlotPropertyTabSize, NULL, this);
  

  // DON'T re-layout the propertynotebook (this doesn't work on Windows)


  CXYPlotPanel::EvtSize(*m_datasetPanelTab,
                           (wxSizeEventFunction)&CXYPlotPanel::OnDataSetTabSize, NULL, this);

  CDatasetPanel::EvtDatasetSelected(*m_datasetPanelTab, 
                                   (CDatasetSelectedEventFunction)&CXYPlotPanel::OnDatasetSelected,
                                    NULL,
                                    this);

  CAnimationToolbar::EvtKeyframeChanged(*this, 
                                        (CKeyframeEventFunction)&CXYPlotPanel::OnKeyframeChanged, NULL, this);
  
  CAnimationToolbar::EvtAutoZoomDataRange(*this, 
                                        (CAutoZoomDataRangeEventFunction)&CXYPlotPanel::OnAutoZoomDataRange, NULL, this);

}

//----------------------------------------
bool CXYPlotPanel::Destroy()
{
  m_finished = true;
  ///????self.animationtoolbar.Destroy()
  m_plotPropertyTab->m_finished = true;
  m_animationToolbar->Pause();

  return wxPanel::Destroy();
}
//----------------------------------------
void CXYPlotPanel::SetXRange(double min, double max)
{
  m_currentXMin = min;
  m_currentXMax = max;
  m_plotActor->SetXRange(min, max);
}
//----------------------------------------
void CXYPlotPanel::SetYRange(double min, double max)
{
  m_currentYMin = min;
  m_currentYMax = max;
  m_plotActor->SetYRange(min, max);
}
//----------------------------------------
void CXYPlotPanel::SetBaseX(double value)
{
  if (value != 0)
  {
    m_plotActor->SetBaseX(value);
  }
}
//----------------------------------------
void CXYPlotPanel::SetBaseY(double value)
{
  if (value != 0)
  {
    m_plotActor->SetBaseY(value);
  }
}
//----------------------------------------
void CXYPlotPanel::SetLogX(bool value)
{
  m_plotActor->SetLogX(value);
}
//----------------------------------------
void CXYPlotPanel::SetLogY(bool value)
{
  m_plotActor->SetLogY(value);
}
//----------------------------------------
void CXYPlotPanel::SetXTitle(const wxString& value)
{
  std::string str = CTools::SlashesDecode((const char *)(value));
  m_plotActor->SetXTitle(str.c_str());
}
//----------------------------------------
void CXYPlotPanel::SetYTitle(const wxString& value)
{
  std::string str = CTools::SlashesDecode((const char *)(value));
  m_plotActor->SetYTitle(str.c_str());
}
//----------------------------------------
void CXYPlotPanel::SetTitle(const wxString& value)
{
  std::string str = CTools::SlashesDecode((const char *)(value));
  m_plotActor->SetTitle(str.c_str());
}
//----------------------------------------
void CXYPlotPanel::SetFPS(int32_t value)
{
  //????self.animationtoolbar.SetFPS(fps)
}

//----------------------------------------
CXYPlotProperty* CXYPlotPanel::GetPlotProperty(int32_t index)
{
  if (m_plotDataCollection == NULL)
  {
    return NULL;
  }
  return m_plotDataCollection->GetPlotProperty(index);
}
//----------------------------------------
void CXYPlotPanel::AddData(CXYPlotData* pdata)
{
  
  if (m_plotDataCollection == NULL)
  {
    return;
  }

  CXYPlotProperty* props = pdata->GetPlotProperty();

  m_plotDataCollection->Insert(pdata);


  m_plotActor->AddInput(pdata->GetVtkDataArrayPlotData(), props->GetVtkProperty2D());
  
  double xrMin = 0;
  double xrMax = 0;
  double yrMin = 0;
  double yrMax = 0;

  if ( (isDefaultValue(props->GetXMin())) ||
       (isDefaultValue(props->GetXMax())) )
  {
    m_plotDataCollection->GetXRange(xrMin, xrMax);
  }

  if ( (isDefaultValue(props->GetYMin())) ||
       (isDefaultValue(props->GetYMax())) )
  {
    m_plotDataCollection->GetYRange(yrMin, yrMax);
  }

  if (isDefaultValue(props->GetXMin()) == false)
  {
    xrMin = props->GetXMin();
  }
  if (isDefaultValue(props->GetXMax()) == false)
  {
    xrMax = props->GetXMax();
  }

  if (isDefaultValue(props->GetYMin()) == false)
  {
    yrMin = props->GetYMin();
  }
  if (isDefaultValue(props->GetYMax()) == false)
  {
    yrMax = props->GetYMax();
  }

  SetXRange(xrMin, xrMax);
  SetYRange(yrMin, yrMax);

  SetBaseX(props->GetXBase());
  SetBaseY(props->GetYBase());

  SetLogX(props->GetXLog());
  SetLogY(props->GetYLog());

  SetXTitle(props->GetXLabel());
  SetYTitle(props->GetYLabel());

  SetTitle(props->GetTitle());

  if (isDefaultValue(props->GetXNumTicks()) == false)
  {
    m_plotActor->SetNumberOfXLabels(props->GetXNumTicks());
  }

  if (isDefaultValue(props->GetYNumTicks()) == false)
  {
    m_plotActor->SetNumberOfYLabels(props->GetYNumTicks());
  }

  int32_t nFrames = pdata->GetNumberOfFrames();
  m_multiFrame |= (nFrames > 1);

  
  //????self.animationtoolbar.SetMaxFrames(nframes)
  m_animationToolbar->SetMaxFrame(nFrames);

  m_datasetPanelTab->Reset();
  m_plotPropertyTab->UpdateValues();

  SetAnimationDescr(0);

  // Fix for Windows refusing to draw initial screen:
#ifdef WIN32
  Layout();
#endif


}



//----------------------------------------
void CXYPlotPanel::SetAnimationDescr(uint32_t indexValue)
{
  if (m_plotDataCollection == NULL)
  {
    return;
  }

 // glbSizerAnimationDescr->Hide(m_animationToolbar->GetDescr());
  m_animationToolbar->GetDescr()->SetLabel("");
  if (m_plotDataCollection->size() <= 0)
  {
    return;
  }

  wxString descr;

  uint32_t nData = m_plotDataCollection->size();

  for (uint32_t i = 0 ; i < nData ; i++)
  {

    CXYPlotData* data = m_plotDataCollection->Get(i);

    if (data->GetVarComplement().GetNbValues() <= 0)
    {
      continue;
    }

    CFieldNetCdf& field = data->GetFieldComplement();
    std::string fieldDescr = field.GetMostExplicitName();
    std::string strUnit = field.GetUnit();

    CUnit* unit = field.GetNetCdfUnit();
  
    uint32_t index = indexValue;
    if (index >= data->GetVarComplement().GetNbValues())
    {
      index = data->GetVarComplement().GetNbValues() - 1;
    }
    if (unit->IsDate())
    {
      CDate d1950_01_01 = unit->ConvertToBaseUnit(data->GetVarComplement().GetValues()[index]);
    
      descr.Append(wxString::Format("'%s' value for above plot: %s (field '%s')\n", 
                               fieldDescr.c_str(), 
                               d1950_01_01.AsString("", true).c_str(),
                               data->GetName().c_str()));

    }
    else
    {
      descr.Append(wxString::Format("'%s' value for above plot: %f %s (field '%s')\n", 
                               fieldDescr.c_str(), 
                               data->GetVarComplement().GetValues()[index],
                               strUnit.c_str(),
                               data->GetName().c_str()));
    }


    uint32_t iOtherFields = 0;

    const CObArray& otherFields = data->GetOtherFields();
    //const std::vector<CExpressionValue>& otherData = data->GetOtherVars();
    const CObArray& otherData = data->GetOtherVars();
    
    //otherFields.Dump(*(CTrace::GetInstance()->GetDumpContext()));

    for (iOtherFields = 0 ; iOtherFields < otherFields.size() ; iOtherFields++)
    {
      CFieldNetCdf* field = dynamic_cast<CFieldNetCdf*>(otherFields.at(iOtherFields));
      if (field == NULL)
      {
        continue;
      }

      std::string fieldDescr = field->GetMostExplicitName();
      std::string strUnit = field->GetUnit();

      CUnit* unit = field->GetNetCdfUnit();

      uint32_t index = indexValue;
      CExpressionValue* exprValue = CExpressionValue::GetExpressionValue(otherData.at(iOtherFields));

      if (index >= exprValue->GetNbValues())
      {
        index = exprValue->GetNbValues() - 1;
      }

      if (unit->IsDate())
      {
        CDate d1950_01_01 = unit->ConvertToBaseUnit(exprValue->GetValues()[index]);
  
        descr.Append(wxString::Format("'%s' value for above plot: %s (field '%s')\n", 
                                 fieldDescr.c_str(), 
                                 d1950_01_01.AsString("", true).c_str(),
                                 data->GetName().c_str()));

      }
      else
      {
        double value = exprValue->GetValues()[index];
        wxString valueStr;

        if (isDefaultValue(value))
        {
          valueStr = "No value (default value)";
        }
        else
        {
          valueStr = wxString::Format("%f %s", value, strUnit.c_str());
        }

        descr.Append(wxString::Format("'%s' value for above plot: %s (field '%s')\n", 
                                 fieldDescr.c_str(), 
                                 valueStr.c_str(),
                                 data->GetName().c_str()));
      }

    }
  }


  m_animationToolbar->GetDescr()->SetLabel(descr);

}

//----------------------------------------
void CXYPlotPanel::UpdateRender()
{
  m_vtkWidget->Refresh();

  Refresh();  
}

//----------------------------------------
void CXYPlotPanel::ShowAnimationToolbar(bool showIt)
{
  ///???m_vSizer.Show(self.animationtoolbar, showIt);
  m_vSizer->Show(m_animationToolbar, showIt);
  Layout();
}
//----------------------------------------
void CXYPlotPanel::ShowPropertyPanel(bool showIt)
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
void CXYPlotPanel::AdjustSplitPanelSashPosition()
{
  // Update sash position in splitter window
  // Try to keep the size of the property pane the same
  //  sashposition = - (self.propertypanel.GetSize()[0] + self.splitpanel.GetSashSize())
  wxSize size = m_propertyPanel->GetSize();
  int32_t sashPosition = - (size.GetWidth() + m_splitPanel->GetSashSize());
  m_splitPanel->SetSashPosition(sashPosition);
}

//----------------------------------------
void CXYPlotPanel::EvtSize(wxWindow& window, const wxSizeEventFunction& method, wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_SIZE,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);

}
//----------------------------------------
void CXYPlotPanel::EvtRadioButton(wxWindow& window, const wxCommandEventFunction& method, wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);

}
//----------------------------------------
void CXYPlotPanel::EvtCheckBox(wxWindow& window, const wxCommandEventFunction& method, wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_COMMAND_CHECKBOX_CLICKED,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);

}
/*

    def ExportToGnuplotFile(self, filename):
        return self.plotactor.ExportToGnuplotFile(filename)
*/

/*
    def OnKeyFrameChanged(self, event):
        if self.finished:
            return
        key = event.Keyframe
        self.plotdata.OnFrameChange(key)
        self.datasettab.OnFrameChange(key)
        self.UpdateRender()
*/


//----------------------------------------
void CXYPlotPanel::OnAutoZoomDataRange(CAutoZoomDataRangeEvent& event)
{
  if (m_plotDataCollection == NULL)
  {
    return;
  }
  m_plotDataCollection->SetZoomToDataRange(event.m_autoZoom);
  m_plotPropertyTab->FullView();
}

//----------------------------------------
void CXYPlotPanel::OnKeyframeChanged(CKeyframeEvent& event)
{
  if (m_finished)
  {
    return;
  }
  if (m_plotDataCollection == NULL)
  {
    return;
  }

  SetAnimationDescr(event.m_frameNumber);

  m_plotDataCollection->OnFrameChange(event.m_frameNumber);
  UpdateRender();
  m_datasetPanelTab->OnFrameChange(event);
  m_plotPropertyTab->OnFrameChange(event);

  if (m_plotDataCollection->IsZoomToDataRange())
  {
      m_plotPropertyTab->FullView();
  }

}
//----------------------------------------
void CXYPlotPanel::OnVtkWidgetSize(wxSizeEvent& event)
{

  UpdateRender();
  event.Skip();

}


//----------------------------------------
void CXYPlotPanel::OnSplitPanelSize(wxSizeEvent& event)
{
  if (m_splitPanel->IsSplit())
  {
    AdjustSplitPanelSashPosition();
  }

  event.Skip();
}
//----------------------------------------
void CXYPlotPanel::OnPropertyPanelSize(wxSizeEvent& event)
{
  m_propertyPanel->Layout();
  event.Skip();
}


//----------------------------------------
void CXYPlotPanel::OnPlotPropertyTabSize(wxSizeEvent& event)
{
  m_plotPropertyTab->Layout();
  event.Skip();
}
//----------------------------------------
void CXYPlotPanel::OnDataSetTabSize(wxSizeEvent& event)
{
  m_datasetPanelTab->Layout();
  event.Skip();
}

//----------------------------------------
void CXYPlotPanel::OnDatasetSelected(CDatasetSelectedEvent& event)
{
  UpdateRender();
}

//----------------------------------------
void CXYPlotPanel::SetFocusVtkWidget(vtkObject* obj, unsigned long, void*, void*)
{
  wxVTKRenderWindowInteractor *Interactor = dynamic_cast<wxVTKRenderWindowInteractor*>(obj);
  Interactor->SetFocus();
}


// WDR: handler implementations for CXYPlotPanel




