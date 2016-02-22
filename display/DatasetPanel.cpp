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
    #pragma implementation "DatasetPanel.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "Validators.h"

#include "BratDisplayApp.h"
#include "XYPlotPanel.h"
#include "new-gui/brat/DataModels/PlotData/XYPlotData.h"
#include "new-gui/brat/DataModels/PlotData/MapColor.h"
#include "DatasetPanel.h"
#include "wxInterface.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// CAttributeListPanel
//----------------------------------------------------------------------------
DEFINE_EVENT_TYPE(wxEVT_DATASET_ATTR_CHANGED)

IMPLEMENT_CLASS(CAttributeListPanel,wxPanel)

// WDR: event table for CAttributeListPanel

BEGIN_EVENT_TABLE(CAttributeListPanel,wxPanel)
END_EVENT_TABLE()

//----------------------------------------
CAttributeListPanel::CAttributeListPanel()
{
  Init();

}
//----------------------------------------
CAttributeListPanel::CAttributeListPanel( wxWindow *parent,CXYPlotData* plotData,  wxWindowID id,
                                          const wxPoint &position, const wxSize& size, long style ) 
                                          //: wxPanel( parent, id, position, size, style )
{
  Init();
  bool bOk = Create(parent, plotData, id, position, size, style);
  if (bOk == false)
  {
    CException e("ERROR in CAttributeListPanel ctor - Unenable to create wxWidgets (Create method returns false)", BRATHL_SYSTEM_ERROR);
    throw(e);
  }


}


//----------------------------------------
CAttributeListPanel::~CAttributeListPanel()
{

}

//----------------------------------------
bool CAttributeListPanel::Create(wxWindow *parent, CXYPlotData* plotData,
                                 wxWindowID id,
                                 const wxPoint& pos,
                                 const wxSize& size ,
                                 long style) 
{
  bool bOk = true;

  wxPanel::Create( parent, id, pos, size, style );
  
  m_plotData = plotData;
  
  bOk = CreateControls();
  if (bOk == false)
  {
    return false;
  }

  CreateLayout();
  InstallEventListeners();
  InstallToolTips();
  return true;
}

//----------------------------------------
void CAttributeListPanel::Init()
{
  m_plotData = NULL;
  m_colorBtn = NULL;
  m_label = NULL;

  m_sizer = NULL;
  m_hSizer = NULL;
  m_hSizer1 = NULL;
  m_hSizer2 = NULL;
  m_hSizer3 = NULL;
  m_hSizer4 = NULL;
  m_hSizer5 = NULL;

  m_sizer1 = NULL;
  m_sizer2 = NULL;
  m_sizer3 = NULL;

  m_drawLines = NULL;
  m_drawPoints = NULL;

  m_stipplePatternLabel = NULL;
  m_stipplePatternChoice = NULL;
  m_glyphLabel = NULL;
  m_glyphChoice = NULL;
  m_lineWidthCtrl = NULL;
  m_pointSizeCtrl = NULL;
  m_filledPoints = NULL;
  
}
//----------------------------------------
bool CAttributeListPanel::CreateControls()
{
  m_ID_LINEWIDTH = wxNewId();
  m_ID_POINTSIZE = wxNewId();
  m_ID_OPACITY = wxNewId();

  CFloatValidator floatValidator;

  int32_t charwidth, charheight;
  CBratDisplayApp::DetermineCharSize(this, charwidth, charheight);

  m_box1 = new wxStaticBox(this, -1, "Lines attributes");
  m_box2 = new wxStaticBox(this, -1, "Points attributes");
  m_box3 = new wxStaticBox(this, -1, "Plot");

  CXYPlotProperties* props = m_plotData->GetPlotProperties();

  m_hidePlot = new wxCheckBox(this, -1, "Hide");
  m_hidePlot->SetValue(props->GetHide());

  m_label = new wxStaticText(this, -1, "Plot color", wxDefaultPosition, wxDefaultSize, wxCENTRE);

  m_colorBtn = new wxButton(this, -1, "", wxDefaultPosition, wxSize(3*charwidth, -1));
  m_colorBtn->SetBackgroundColour(color_cast(props->GetColor()));


  m_opacityCtrl = new CLabeledTextCtrl(this, NULL, m_ID_OPACITY,
                                         "Opacity:", "",
                                         floatValidator,
                                         "%-#.2f",
                                         wxSize(5*charwidth, -1),
                                         wxTE_PROCESS_ENTER);

  m_opacityCtrl->SetValue(props->GetOpacity(), 0.6, 0.1, 1.0);



  m_drawLines = new wxCheckBox(this, -1, "Draw");
  m_drawLines->SetValue(props->GetLines());

  m_drawPoints = new wxCheckBox(this, -1, "Draw");
  m_drawPoints->SetValue(props->GetPoints());

  std::vector<std::string> v;
  CMapStipplePattern::GetInstance().NamesToArrayString(v);
  wxArrayString array;
  for ( auto &s : v )
	  array.Add( s.c_str() );
  m_stipplePatternChoice = new wxChoice(this, -1,
                                       wxDefaultPosition, 
                                       wxDefaultSize,
                                       array);

  wxString strSelect = CMapStipplePattern::GetInstance().StippleToName(props->GetLineStipple());

  m_stipplePatternChoice->SetStringSelection(strSelect);
  m_stipplePatternChoice->Enable(props->GetLines());

  m_stipplePatternLabel = new wxStaticText(this, -1, "Stipple pattern",
                                           wxDefaultPosition, 
                                           wxDefaultSize,
                                           wxALIGN_RIGHT);


  m_lineWidthCtrl = new CLabeledTextCtrl(this, NULL, m_ID_LINEWIDTH,
                                         "Line width:", "",
                                         floatValidator,
                                         "%-#.2f",
                                         wxSize(5*charwidth, -1),
                                         wxTE_PROCESS_ENTER);

  m_lineWidthCtrl->SetValue(props->GetLineWidth() / props->GetLineWidthFactor(), 1.0, 0.1);
  m_lineWidthCtrl->Enable(props->GetLines());


  v.clear();
  array.Clear();
  CMapPointGlyph::GetInstance().NamesToArrayString(v);
  for ( auto &s : v )
	  array.Add( s.c_str() );
  m_glyphChoice = new wxChoice(this, -1,
                               wxDefaultPosition, 
                               wxDefaultSize,
                               array);

  m_glyphLabel = new wxStaticText(this, -1, "Point glyph",
                                           wxDefaultPosition, 
                                           wxDefaultSize,
                                           wxALIGN_RIGHT);

  strSelect = CMapPointGlyph::GetInstance().GlyphToName(props->GetPointGlyph());

  m_glyphChoice->SetStringSelection(strSelect);
  m_glyphChoice->Enable(props->GetPoints());

  m_pointSizeCtrl = new CLabeledTextCtrl(this, NULL, m_ID_POINTSIZE,
                                       "Point size:", "",
                                       floatValidator,
                                       "%-#.2f",
                                       wxSize(5*charwidth, -1),
                                       wxTE_PROCESS_ENTER);

  m_pointSizeCtrl->SetValue(props->GetPointSize(), 1.0, 0.1);
  m_pointSizeCtrl->Enable(props->GetPoints());

  m_filledPoints = new wxCheckBox(this, -1, "Fill point");
  m_filledPoints->SetValue(props->GetFilledPoint());
  m_filledPoints->Enable(props->GetPoints());


  return true;

}

//----------------------------------------
void CAttributeListPanel::CreateLayout()
{

  CXYPlotProperties* props = m_plotData->GetPlotProperties();

  m_mainSizer = new wxBoxSizer(wxVERTICAL);

  m_sizer = new wxBoxSizer(wxVERTICAL);

  m_hSizer = new wxBoxSizer(wxHORIZONTAL);
  m_hSizer1 = new wxBoxSizer(wxHORIZONTAL);
  m_hSizer2 = new wxBoxSizer(wxHORIZONTAL);
  m_hSizer3 = new wxBoxSizer(wxHORIZONTAL);
  m_hSizer4 = new wxBoxSizer(wxHORIZONTAL);
  m_hSizer5 = new wxBoxSizer(wxHORIZONTAL);

  m_sizer1 = new wxStaticBoxSizer(m_box1, wxVERTICAL);
  m_sizer2 = new wxStaticBoxSizer(m_box2, wxVERTICAL);
  m_sizer3 = new wxStaticBoxSizer(m_box3, wxVERTICAL);


  m_hSizer->Add(m_label, 0, wxALIGN_CENTRE, 10);
  m_hSizer->Add(5, 0, 1);
  m_hSizer->Add(m_colorBtn, 0, wxALIGN_CENTRE, 10);
  m_hSizer->Add(5, 0, 1);
  m_hSizer->Add(m_opacityCtrl, 0, wxALIGN_CENTRE, 10);

  m_hSizer5->Add(m_hidePlot, 0, wxALIGN_CENTER|wxRIGHT, 10);

  m_sizer3->Add(m_hSizer5, 0);
  m_hSizer1->Add(5, 0, 1);
  m_sizer3->Add(m_hSizer, 0);


  m_hSizer1->Add(m_drawLines, 0, wxALIGN_CENTER|wxRIGHT, 10);
  m_hSizer1->Add(m_stipplePatternLabel, 0, wxALIGN_CENTER, 10);
  m_hSizer1->Add(5, 0, 1);
  m_hSizer1->Add(m_stipplePatternChoice, 0, wxALIGN_CENTER|wxEXPAND, 10);

  //m_hSizer3->Add(5, 0, 1);
  m_hSizer3->Add(m_lineWidthCtrl, 0, wxRIGHT);

  m_hSizer2->Add(m_drawPoints, 0, wxALIGN_CENTER|wxRIGHT, 10);
  m_hSizer2->Add(m_glyphLabel, 0, wxALIGN_CENTER, 10);
  m_hSizer2->Add(5, 0, 1);
  m_hSizer2->Add(m_glyphChoice, 0, wxALIGN_CENTER|wxEXPAND, 10);

  //m_hSizer4->Add(5, 0, 1);
  m_hSizer4->Add(m_pointSizeCtrl, 0, wxRIGHT, 10);
  m_hSizer4->Add(m_filledPoints, 0, wxALIGN_CENTER, 10);

  m_sizer1->Add(m_hSizer1, 0);
  m_sizer1->Add(0, 5, 1);
  m_sizer1->Add(m_hSizer3, 0);

  m_sizer2->Add(m_hSizer2, 0);
  m_sizer2->Add(0, 5, 1);
  m_sizer2->Add(m_hSizer4, 0);

  m_sizer->Add(m_sizer3, 0, wxEXPAND);
  m_sizer->Add(0, 15, 0);
  m_sizer->Add(m_sizer1, 0, wxEXPAND);
  m_sizer->Add(0, 15, 0);
  m_sizer->Add(m_sizer2, 0, wxEXPAND);

  m_mainSizer->Add(m_sizer, 1, wxEXPAND|wxALL, 10);


  SetAutoLayout(true);
  SetSizerAndFit(m_mainSizer);
  
  m_sizer1->Show(!props->GetHide());
  m_sizer2->Show(!props->GetHide());

}

//----------------------------------------
void CAttributeListPanel::InstallEventListeners()
{
  m_colorBtn->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
             (wxObjectEventFunction)
             (wxEventFunction)
             (wxCommandEventFunction)&CAttributeListPanel::OnColor,
             NULL,
             this);

  CXYPlotPanel::EvtCheckBox(*m_drawLines,
                            (wxCommandEventFunction)&CAttributeListPanel::OnUpdate, NULL, this);
  
  CXYPlotPanel::EvtCheckBox(*m_drawPoints,
                            (wxCommandEventFunction)&CAttributeListPanel::OnUpdate, NULL, this);

  CXYPlotPanel::EvtCheckBox(*m_filledPoints,
                            (wxCommandEventFunction)&CAttributeListPanel::OnUpdate, NULL, this);

  CXYPlotPanel::EvtCheckBox(*m_hidePlot,
                            (wxCommandEventFunction)&CAttributeListPanel::OnHide, NULL, this);


  CLabeledTextCtrl::EvtValueChanged(*this, 
                                   m_ID_LINEWIDTH,
                                   (CValueChangedEventFunction)&CAttributeListPanel::OnValueChange, NULL, this);

  CLabeledTextCtrl::EvtValueChanged(*this, 
                                   m_ID_POINTSIZE,
                                   (CValueChangedEventFunction)&CAttributeListPanel::OnValueChange, NULL, this);

  CLabeledTextCtrl::EvtValueChanged(*this, 
                                   m_ID_OPACITY,
                                   (CValueChangedEventFunction)&CAttributeListPanel::OnValueChange, NULL, this);


  m_stipplePatternChoice->Connect( wxEVT_COMMAND_CHOICE_SELECTED,
                                   (wxObjectEventFunction)
                                   (wxEventFunction)
                                   (wxCommandEventFunction)&CAttributeListPanel::OnUpdate,
                                   NULL,
                                   this);
  m_glyphChoice->Connect( wxEVT_COMMAND_CHOICE_SELECTED,
                         (wxObjectEventFunction)
                         (wxEventFunction)
                         (wxCommandEventFunction)&CAttributeListPanel::OnUpdate,
                         NULL,
                         this);


}
//----------------------------------------
void CAttributeListPanel::InstallToolTips()
{
  m_colorBtn->SetToolTip("Open a color selector dialog box.");
  m_drawLines->SetToolTip("Draw plot lines.");
  m_drawPoints->SetToolTip("Draw plot points.");
  m_stipplePatternChoice->SetToolTip("Available stipple patterns.");
  m_glyphChoice->SetToolTip("Available glyhs.");

}
// WDR: handler implementations for CAttributeListPanel
//----------------------------------------
void CAttributeListPanel::OnValueChange(CValueChangedEvent& event)
{
  Update();  
}
//----------------------------------------
void CAttributeListPanel::OnHide(wxCommandEvent& event)
{
  CXYPlotProperties* props = m_plotData->GetPlotProperties();
  
  
  bool hide = m_hidePlot->GetValue();
  m_sizer1->Show(!hide);
  m_sizer2->Show(!hide);
  
  props->SetHide(hide);

  CDatasetAttrChangedEvent ev(GetId());
  wxPostEvent(GetParent(), ev);
}

//----------------------------------------
void CAttributeListPanel::OnUpdate(wxCommandEvent& event)
{
  Update();
}
//----------------------------------------
void CAttributeListPanel::Update()
{
  CXYPlotProperties* props = m_plotData->GetPlotProperties();

  
  m_stipplePatternChoice->Enable(m_drawLines->GetValue());
  m_lineWidthCtrl->Enable(m_drawLines->GetValue());

  m_glyphChoice->Enable(m_drawPoints->GetValue());
  m_pointSizeCtrl->Enable(m_drawPoints->GetValue());
  m_filledPoints->Enable(m_drawPoints->GetValue());

  double opacity;
  m_opacityCtrl->GetValue(opacity, 0.6, 0.1, 1.0);
  props->SetOpacity(opacity);

  props->SetFilledPoint(m_filledPoints->GetValue());
  //Reverse draw points otherwise points are not refresh (and 'fill' property doesn't work)
  props->SetPoints(!m_drawPoints->GetValue());

  props->SetLines(m_drawLines->GetValue());
  props->SetPoints(m_drawPoints->GetValue());

  double pointSize;
  m_pointSizeCtrl->GetValue(pointSize, props->GetPointSize(), 0.1);
  props->SetPointSize(pointSize);


  wxString strSelect = m_stipplePatternChoice->GetStringSelection();
  StipplePattern stipple = CMapStipplePattern::GetInstance().NameToStipple(strSelect.ToStdString());
  props->SetLineStipple(stipple);

  double lineWidth;
  m_lineWidthCtrl->GetValue(lineWidth, props->GetLineWidth() / props->GetLineWidthFactor(), 0.1); 
  props->SetLineWidth(lineWidth * props->GetLineWidthFactor());

  strSelect = m_glyphChoice->GetStringSelection();
  PointGlyph glyph = CMapPointGlyph::GetInstance().NameToGlyph(strSelect.ToStdString());
  props->SetPointGlyph(glyph);

  
  //Layout();
  //GetParent()->Layout();

  CDatasetAttrChangedEvent ev(GetId());
  wxPostEvent(GetParent(), ev);

}
//----------------------------------------
void CAttributeListPanel::OnColor(wxCommandEvent& event) 
{
  CXYPlotProperties* props = m_plotData->GetPlotProperties();

  wxColourData colorData = CMapColor::GetInstance().ChooseColor(color_cast(props->GetColor()), this);
  SetColor( *props, colorData );

  //props->Update();

  m_colorBtn->SetBackgroundColour(color_cast(props->GetColor()));

  Layout();
  GetParent()->Layout();

  CDatasetAttrChangedEvent ev(GetId());
  wxPostEvent(GetParent(), ev);
}


//-------------------------------------------------------------
//------------------- CDatasetListPanel class --------------------
//-------------------------------------------------------------
DEFINE_EVENT_TYPE(wxEVT_DATASET_SELECTED)

IMPLEMENT_CLASS(CDatasetListPanel,wxPanel)

// WDR: event table for CDatasetListPanel

BEGIN_EVENT_TABLE(CDatasetListPanel,wxPanel)
END_EVENT_TABLE()
//----------------------------------------

CDatasetListPanel::CDatasetListPanel()
{
}
//----------------------------------------

CDatasetListPanel::CDatasetListPanel( wxWindow *parent, CXYPlotDataCollection* plotDataCollection,  wxWindowID id,
                              const wxPoint &position, const wxSize& size, long style ) 
                              //: wxPanel( parent, id, position, size, style ) --> called in Create
{
  Init();
  bool bOk = Create(parent, plotDataCollection, id, position, size, style);
  if (bOk == false)
  {
    CException e("ERROR in CDatasetListPanel ctor - Unenable to create wxWidgets (Create method returns false)", BRATHL_SYSTEM_ERROR);
    throw(e);
  }

}

//----------------------------------------

CDatasetListPanel::~CDatasetListPanel()
{
  m_plotDataCollection = NULL;

}
//----------------------------------------
void CDatasetListPanel::Init()
{
  m_plotDataCollection = NULL;
  
  m_currentDatasetIndex = -1;
  m_datasetSelected = false;

  m_datasetListBox = NULL;
  m_sizer = NULL;
  
  m_attributeListPanel = NULL;

}

//----------------------------------------
bool CDatasetListPanel::Create(wxWindow *parent, CXYPlotDataCollection* plotDataCollection,
                               wxWindowID id,
                               const wxPoint& pos,
                               const wxSize& size ,
                               long style) 
{
  bool bOk = true;

  wxPanel::Create( parent, id, pos, size, style );
  
  m_plotDataCollection = plotDataCollection;
  
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
bool CDatasetListPanel::CreateControls()
{

  m_ID_DATASETLIST = ::wxNewId();

  int32_t charwidth, charheight;
  CBratDisplayApp::DetermineCharSize(this, charwidth, charheight);

  m_datasetListBox = new wxListBox(this, m_ID_DATASETLIST, wxDefaultPosition, wxSize(120, 6 * charheight),
                                   0, NULL, wxLB_HSCROLL );
  m_deselectButton = new wxButton(this, -1, "Deselect");

  Reset();

  return true;
}
//----------------------------------------
void CDatasetListPanel::CreateBoxSizers()
{
  m_sizer = new wxBoxSizer(wxVERTICAL);

}
//----------------------------------------
void CDatasetListPanel::CreateLayout()
{
  CreateBoxSizers();

  m_sizer->Add(m_datasetListBox, 1, wxEXPAND|wxALIGN_CENTER);
  m_sizer->Add(m_deselectButton, 0, wxEXPAND|wxALIGN_CENTER|wxBOTTOM, 5);

  SetSizer(m_sizer);

}
//----------------------------------------
void CDatasetListPanel::InstallEventListeners()
{
  m_datasetListBox->Connect(wxEVT_COMMAND_LISTBOX_SELECTED,
                           (wxObjectEventFunction)
                           (wxEventFunction)
                           (wxCommandEventFunction)&CDatasetListPanel::OnDatasetSelect,
                           NULL,
                           this);

  m_datasetListBox->Connect(wxEVT_UPDATE_UI,
                           (wxObjectEventFunction)
                           (wxEventFunction)
                           (wxUpdateUIEventFunction)&CDatasetListPanel::OnDatasetDeselect,
                           NULL,
                           this);
  
  m_deselectButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
               (wxObjectEventFunction)
               (wxEventFunction)
               (wxCommandEventFunction)&CDatasetListPanel::OnDeselect,
               NULL,
               this);

  CDatasetPanel::EvtDatasetAttrChanged(*this, 
                                       (CDatasetAttrChangedEventFunction)&CDatasetListPanel::OnDatasetAttrChanged,
                                       NULL,
                                       this);

}
//----------------------------------------
void CDatasetListPanel::OnDatasetAttrChanged(CDatasetAttrChangedEvent& event)
{
  CDatasetSelectedEvent ev(GetId(),
                           m_datasetListBox->GetSelection(),
                           false);
  
   wxPostEvent(GetParent(), ev);
}

//----------------------------------------
void CDatasetListPanel::OnDeselect(wxCommandEvent& event) 
{
  m_datasetListBox->Deselect(m_datasetListBox->GetSelection());
}

//----------------------------------------
void CDatasetListPanel::OnDatasetSelect(wxCommandEvent& event) 
{

  if (m_plotDataCollection == NULL)
  {
    return;
  }
  
  if (m_datasetSelected)
  {
    SelectDataset(m_currentDatasetIndex, false);
  }

  m_currentDatasetIndex = m_datasetListBox->GetSelection();
  if (m_currentDatasetIndex < 0)
  {
    return;
  }
  m_datasetSelected = true;

  
  SelectDataset(m_currentDatasetIndex, true);

  Layout();
  GetParent()->Layout();

  CDatasetSelectedEvent ev(GetId(),
                           m_datasetListBox->GetSelection(),
                           true);
  wxPostEvent(GetParent(), ev);
}

//----------------------------------------
void CDatasetListPanel::OnDatasetDeselect(wxUpdateUIEventFunction& event) 
{
  if (m_plotDataCollection == NULL)
  {
    return;
  }
  
  // Listboxes do not fire deselect events for some reason, so
  // we determine whether such an event occurred the hard way,
  // by looking at *all* events.
  
  wxWindow* win = wxGetTopLevelParent(this);
  if (win == NULL)
  {
    return;
  }

  wxWindow* myWin = win->FindWindowById(GetId());
  if (myWin == NULL)
  {
    return;
  }

  if ( (m_currentDatasetIndex >= 0) && (m_datasetListBox->GetSelection() < 0) )
  {
    SelectDataset(m_currentDatasetIndex, false);
    m_currentDatasetIndex = -1;
    m_datasetSelected = false;
    Layout();
    GetParent()->Layout();
    CDatasetSelectedEvent ev(GetId(),
                 m_datasetListBox->GetSelection(),
                             false);
    
     wxPostEvent(GetParent(), ev);
  }

}
//----------------------------------------
void CDatasetListPanel::SelectDataset(int32_t index, bool select)
{
  if (index < 0)
  {
    return;
  }

  if (m_plotDataCollection == NULL)
  {
    return;
  }

  Freeze();

  CXYPlotData* data = m_plotDataCollection->Get(index);
  data->GetPlotProperties()->SetFocus(select);


  if (select)
  {
    m_attributeListPanel = new CAttributeListPanel(this, data);
    m_sizer->Add(m_attributeListPanel, 0, wxADJUST_MINSIZE|wxEXPAND|wxTOP, 10);
  }
  else
  {
    if (m_attributeListPanel != NULL) 
    {
      //m_sizer->Remove(m_attributeListPanel);
      m_sizer->Detach(m_attributeListPanel);
      m_attributeListPanel->Destroy();
      m_attributeListPanel = NULL;
    }

  }

  Thaw();

}
//----------------------------------------
void CDatasetListPanel::UpdateFieldValues()
{
      // ??? TODO

///        try:
///            self.attrPanel.UpdateFieldValues()
///        except AttributeError:
///            pass
}
//----------------------------------------
void CDatasetListPanel::Reset()
{
  if (m_plotDataCollection == NULL)
  {
    return;
  }

  std::vector<std::string> v;
  wxArrayString names;
  m_plotDataCollection->GetNames(v);
  for ( auto &s : v )
	  names.Add( s );


  if (names.IsEmpty() == false)
  {
    m_datasetListBox->Set(names);

    m_currentDatasetIndex = 0;
    m_datasetListBox->SetSelection(m_currentDatasetIndex);
    
    wxCommandEvent evt(wxEVT_COMMAND_LISTBOX_SELECTED);
    wxPostEvent(m_datasetListBox, evt);
  }
  else
  {
    m_datasetListBox->Clear();
    m_currentDatasetIndex = -1;
    m_datasetSelected = false;
  }


  Layout();

  GetParent()->Layout();

}
// WDR: handler implementations for CDatasetListPanel

//-------------------------------------------------------------
//------------------- CDatasetPanel class --------------------
//-------------------------------------------------------------

IMPLEMENT_CLASS(CDatasetPanel,wxPanel)

// WDR: event table for CDatasetPanel

BEGIN_EVENT_TABLE(CDatasetPanel,wxPanel)
END_EVENT_TABLE()

//----------------------------------------
CDatasetPanel::CDatasetPanel()
{
  Init();
}

//----------------------------------------
CDatasetPanel::CDatasetPanel( wxWindow *parent, CXYPlotDataCollection* plotDataCollection,  wxWindowID id,
                              const wxPoint &position, const wxSize& size, long style ) 
                              //: wxPanel( parent, id, position, size, style ) --> called in Create
{
  Init();
  if (! Create(parent, plotDataCollection, id, position, size, style))
  {
    CException e("ERROR in CDatasetPanel ctor - Unenable to create wxWidgets (Create method returns false)", BRATHL_SYSTEM_ERROR);
    throw(e);
  }

}


//----------------------------------------
CDatasetPanel::~CDatasetPanel()
{
  m_plotDataCollection = NULL;
}

//----------------------------------------
void CDatasetPanel::Init()
{
  m_plotDataCollection = NULL;

  m_datasetListPanel = NULL;
  
  m_sizer = NULL;


}
//----------------------------------------
void CDatasetPanel::CreateBoxSizers()
{
  m_sizer = new wxBoxSizer(wxVERTICAL);

}
//----------------------------------------
bool CDatasetPanel::Create(wxWindow *parent, CXYPlotDataCollection* plotDataCollection,
                           wxWindowID id,
                           const wxPoint& pos,
                           const wxSize& size ,
                           long style) 
{
  wxPanel::Create( parent, id, pos, size, style );
  
  m_plotDataCollection = plotDataCollection;
  
  if (! CreateControls())
    return false;

  CreateLayout();
  InstallEventListeners();

  return true;
}

//----------------------------------------
bool CDatasetPanel::CreateControls()
{
  m_ID_DATASETLISTPANEL = wxNewId();


  m_datasetListPanel = new CDatasetListPanel(this, m_plotDataCollection, m_ID_DATASETLISTPANEL);


  return true;

}

//----------------------------------------
void CDatasetPanel::CreateLayout()
{
  CreateBoxSizers();


  m_sizer->Add(m_datasetListPanel, 0, wxEXPAND|wxALL, 10);

  SetSizer(m_sizer);

}

//----------------------------------------
void CDatasetPanel::InstallEventListeners()
{
}
//----------------------------------------
void CDatasetPanel::OnFrameChange(wxEvent& event)
{
  m_datasetListPanel->UpdateFieldValues();
}
//----------------------------------------
void CDatasetPanel::Reset()
{
  m_datasetListPanel->Reset();
}
//----------------------------------------
void CDatasetPanel::EvtDatasetSelected(wxWindow& window, const CDatasetSelectedEventFunction& method,
                                       wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_DATASET_SELECTED,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}
//----------------------------------------
void CDatasetPanel::EvtDatasetAttrChanged(wxWindow& window, const CDatasetAttrChangedEventFunction& method,
                                       wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_DATASET_ATTR_CHANGED,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}

// WDR: handler implementations for CDatasetPanel




