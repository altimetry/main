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
    #pragma implementation "XYPlotPropertyPanel.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif


#include "wxInterface.h"
#include "new-gui/brat/DataModels/PlotData/BratLookupTable.h"
#include "BratDisplayApp.h"

#include "AxisPropertyPanel.h"
#include "new-gui/brat/DataModels/PlotData/XYPlotData.h"

#include "XYPlotPropertyPanel.h"

long ID_XYPLOTPROP_TITLE;
long ID_XYFULLVIEW;


// WDR: class implementations

//----------------------------------------------------------------------------
// CXYPlotPropertyPanel
//----------------------------------------------------------------------------

// WDR: event table for CXYPlotPropertyPanel

BEGIN_EVENT_TABLE(CXYPlotPropertyPanel,wxWindow)
END_EVENT_TABLE()

CXYPlotPropertyPanel::CXYPlotPropertyPanel()

{
  Init();
}

//----------------------------------------
CXYPlotPropertyPanel::CXYPlotPropertyPanel(wxWindow *parent, vtkXYPlotActor* plotter, CXYPlotDataCollection* plotDataCollection, wxVTKRenderWindowInteractor* vtkWidget,
                                         wxWindowID id,
                                         const wxPoint& pos,
                                         const wxSize& size ,
                                         long style) 
              // : wxScrolledWindow( parent, -1, pos, size, style ), --> called in Create

{
  Init();

  if (! Create(parent, plotter, plotDataCollection, vtkWidget, id, pos, size, style))
  {
    CException e("ERROR in CXYPlotPropertyPanel ctor - Unenable to create wxWidgets (Create method returns false)", BRATHL_SYSTEM_ERROR);
    throw(e);
  }

}

//----------------------------------------

CXYPlotPropertyPanel::~CXYPlotPropertyPanel()
{
  m_plotDataCollection = NULL;

}

//----------------------------------------
void CXYPlotPropertyPanel::Init()
{
  m_parent = NULL;
  m_finished = false;
  m_vtkWidget = NULL;
  m_plotter = NULL;
  m_plotDataCollection = NULL;

  m_axisXPanel = NULL;
  m_axisYPanel = NULL;
  
  //m_axisNotebookSizer = NULL;
  m_sizer = NULL;
  m_sizer1 = NULL;


}
//----------------------------------------
void CXYPlotPropertyPanel::CreateBoxSizers()
{
  //m_axisNotebookSizer = new wxNotebookSizer(&m_axisNotebook);
  m_sizer = new wxBoxSizer(wxVERTICAL);
  m_sizer1 = new wxBoxSizer(wxHORIZONTAL );

}
//----------------------------------------
bool CXYPlotPropertyPanel::Create(wxWindow *parent, vtkXYPlotActor* plotter, CXYPlotDataCollection* plotDataCollection, wxVTKRenderWindowInteractor* vtkWidget,
                                         wxWindowID id,
                                         const wxPoint& pos,
                                         const wxSize& size ,
                                         long style) 
{
  wxScrolledWindow::Create( parent, -1, pos, size, style );
  
  m_parent = parent;
  m_plotter = plotter;
  m_plotDataCollection = plotDataCollection;
  m_vtkWidget = vtkWidget;
  
  if (! CreateControls())
    return false;

  CreateLayout();
  InstallToolTips();
  InstallEventListeners();

  return true;
}

//----------------------------------------
bool CXYPlotPropertyPanel::CreateControls()
{
//  self.projection = visanvtk.projections[3][1]

  ID_XYPLOTPROP_TITLE = wxNewId();
  ID_XYFULLVIEW = wxNewId();

  int32_t charwidth, charheight;
  CBratDisplayApp::DetermineCharSize(this, charwidth, charheight);
  
  m_titleCtrl.Create(this, &m_finished, ID_XYPLOTPROP_TITLE,
                     "Plot Title", m_plotter->GetTitle(),
                     wxDefaultValidator,
                     "",
                     wxSize(10*charwidth, -1),
                     wxTE_PROCESS_ENTER);

  m_resetView.Create( this, ID_XYFULLVIEW, _("Reset View"), wxDefaultPosition, wxDefaultSize, 0 );
  
  m_axisNotebook.Create(this, -1);

  m_axisXPanel = new CAxisXPropertyPanel(&m_axisNotebook, m_plotter, m_plotDataCollection, m_vtkWidget);
  m_axisYPanel = new CAxisYPropertyPanel(&m_axisNotebook, m_plotter, m_plotDataCollection, m_vtkWidget);


  return true;

}

//----------------------------------------
void CXYPlotPropertyPanel::CreateLayout()
{
  CreateBoxSizers();

  m_axisNotebook.AddPage(m_axisXPanel, "X-Axis");
  m_axisNotebook.AddPage(m_axisYPanel, "Y-Axis");
  
//  m_axisNotebook.SetSizerAndFit(m_axisNotebookSizer);

  m_sizer1->Add( &m_titleCtrl, 1, wxALIGN_CENTER|wxALL, 5 );
  m_sizer1->Add( &m_resetView, 0, wxALIGN_CENTER|wxALL, 0 );

  //m_sizer->Add(&m_titleCtrl, 0, wxEXPAND|wxALL, 10);
  m_sizer->Add( m_sizer1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

  m_sizer->Add(&m_axisNotebook, 1, wxEXPAND|wxLEFT|wxRIGHT|wxBOTTOM, 2);


  SetSizerAndFit(m_sizer);

}
//----------------------------------------
void CXYPlotPropertyPanel::InstallToolTips()
{
  m_titleCtrl.SetToolTip("The plot title will be displayed centered at the top of the plot panel.");
  m_resetView.SetToolTip("Set axes min./max with original values.");

}

//----------------------------------------
void CXYPlotPropertyPanel::InstallEventListeners()
{

  CLabeledTextCtrl::EvtValueChanged(*this, 
                                     ID_XYPLOTPROP_TITLE, 
                                     (CValueChangedEventFunction)&CXYPlotPropertyPanel::OnTitle);

  m_resetView.Connect(wxEVT_COMMAND_BUTTON_CLICKED,
               (wxObjectEventFunction)
               (wxEventFunction)
               (wxCommandEventFunction)&CXYPlotPropertyPanel::OnFullView,
               NULL,
               this);

}
//----------------------------------------

void CXYPlotPropertyPanel::YReset()
{
  m_axisYPanel->AxisResetRange();
}
//----------------------------------------

void CXYPlotPropertyPanel::XReset()
{
  m_axisXPanel->AxisResetRange();     
}

//----------------------------------------
void CXYPlotPropertyPanel::FullView()
{
  XReset();
  YReset();
}
//----------------------------------------
void CXYPlotPropertyPanel::OnFullView( wxCommandEvent &event )
{
  FullView();
}

//----------------------------------------
void CXYPlotPropertyPanel::OnTitle(CValueChangedEvent& event)
{

  wxString curTitle = m_plotter->GetTitle();
  //??newtitle = codecs.escape_decode(event.value)[0]
  wxString newTitle = CTools::SlashesDecode((const char *)(event.m_value)).c_str();
  if (curTitle == newTitle)
  {
    return;
  }

  m_plotter->SetTitle(newTitle);
  m_vtkWidget->Refresh();
}
//----------------------------------------
void CXYPlotPropertyPanel::UpdateValues()
{
        //???self.titleCtrl.SetValue(codecs.escape_encode(self.plotter.GetTitle())[0])
  wxString str = m_plotter->GetTitle();
  m_titleCtrl.SetValue(CTools::SlashesEncode((const char *)(str)));
  m_axisXPanel->UpdateControls();
  m_axisYPanel->UpdateControls();

}


//----------------------------------------
void CXYPlotPropertyPanel::OnFrameChange(wxEvent& event)
{
  UpdateValues();
}

// WDR: handler implementations for CXYPlotPropertyPanel




