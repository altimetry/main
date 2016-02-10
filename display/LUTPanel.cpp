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
    #pragma implementation "LUTPanel.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "new-gui/Common/tools/Exception.h"


#include "wx/tooltip.h"

#include "wxInterface.h"
#ifdef CursorShape  //collsion Qt X
#undef CursorShape
#endif
#include "LUTPanel.h"
#include "BratDisplayApp.h"

DEFINE_EVENT_TYPE(wxEVT_CLUT_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_CLUT_CLOSE)

long ID_CLUTPANEL_APPLY_BUTTON;

long ID_STDMODIFYPANEL_DEFBOX;

long ID_GRDMODIFYPANEL_LEFT_COLOR_PICKER;
long ID_GRDMODIFYPANEL_RIGHT_COLOR_PICKER;

long ID_CUSTMODIFYPANEL_COLOR_PICKER;
long ID_CUSTMODIFYPANEL_G_LIST;
long ID_CUSTMODIFYPANEL_LABEL;
long ID_CUSTMODIFYPANEL_SLIDER;
long ID_CUSTMODIFYPANEL_INSERT_BUTTON;
long ID_CUSTMODIFYPANEL_REMOVE_BUTTON;
long ID_CUSTMODIFYPANEL_DESELECT_BUTTON;


// WDR: class implementations

//-------------------------------------------------------------
//------------------- CCustomModifyPanel class --------------------
//-------------------------------------------------------------

// WDR: event table for CCustomModifyPanel

BEGIN_EVENT_TABLE(CCustomModifyPanel,wxPanel)
END_EVENT_TABLE()

//----------------------------------------
CCustomModifyPanel::CCustomModifyPanel()
{
  Init();
}

CCustomModifyPanel::CCustomModifyPanel( CLUTPanel *parent, CBratLookupTable* lut,wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) 
    //////: wxPanel( parent, id, position, size, style )
{
  Init();
  if (! Create(parent, lut, id, position, size, style))
  {
    CException e("ERROR in CCustomModifyPanel ctor - Unenable to create wxWidgets (Create method returns false)", BRATHL_SYSTEM_ERROR);
    throw(e);
  }

}

//----------------------------------------
CCustomModifyPanel::~CCustomModifyPanel()
{
    DeleteLUT();
}

//----------------------------------------
void CCustomModifyPanel::DeleteLUT()
{
  if (m_LUT != NULL)
  {
    delete m_LUT;
    m_LUT = NULL;
  }
  
}

//----------------------------------------
void CCustomModifyPanel::Init()
{
  m_LUT = NULL;
  m_parent = NULL;

  m_colorPicker = NULL;
  m_slider = NULL;

  m_butSizer = NULL;
  m_sizer = NULL;
  m_sizer1 = NULL;
  m_sizer2 = NULL;
  m_hSizer = NULL;

  m_insertButton = NULL;
  m_removeButton = NULL;
  m_deselectButton = NULL;

  m_label = NULL;
  m_gList = NULL;

}

//----------------------------------------
bool CCustomModifyPanel::Create( CLUTPanel *parent, CBratLookupTable* lut, wxWindowID id,
                                   const wxPoint &position, const wxSize& size, long style ) 
{
  m_parent = parent;

  wxPanel::Create( parent, id, position, size, style );

  DeleteLUT();
  if (lut == NULL)
  {
    m_LUT = new CBratLookupTable();
    ResetCust();
  }
  else
  {
    m_LUT = new CBratLookupTable(*lut);
  }


  ID_CUSTMODIFYPANEL_COLOR_PICKER = ::wxNewId();
  ID_CUSTMODIFYPANEL_G_LIST = ::wxNewId();
  ID_CUSTMODIFYPANEL_LABEL = ::wxNewId();
  ID_CUSTMODIFYPANEL_SLIDER = ::wxNewId();
  ID_CUSTMODIFYPANEL_INSERT_BUTTON = ::wxNewId();
  ID_CUSTMODIFYPANEL_REMOVE_BUTTON = ::wxNewId();
  ID_CUSTMODIFYPANEL_DESELECT_BUTTON = ::wxNewId();

  m_slider = new wxSlider(this, ID_CUSTMODIFYPANEL_SLIDER, 0, 0, m_LUT->GetLookupTable()->GetNumberOfTableValues());
  m_colorPicker = new CColorPicker(this, "Color:", ID_CUSTMODIFYPANEL_COLOR_PICKER);
  m_insertButton = new wxButton(this, ID_CUSTMODIFYPANEL_INSERT_BUTTON, "Insert Color");
  m_deselectButton = new wxButton(this, ID_CUSTMODIFYPANEL_DESELECT_BUTTON, "Deselect Color");
  m_removeButton = new wxButton(this, ID_CUSTMODIFYPANEL_REMOVE_BUTTON, "Remove Color");

  m_label = new wxStaticText(this, ID_CUSTMODIFYPANEL_LABEL, "X-Values:");
  m_gList = new wxListBox(this, ID_CUSTMODIFYPANEL_G_LIST);
  FillList();

  m_butSizer = new wxBoxSizer(wxVERTICAL);
  m_butSizer->Add(m_insertButton, 0, wxEXPAND|wxALIGN_RIGHT|wxBOTTOM, 5);
  m_butSizer->Add(m_deselectButton, 0, wxEXPAND|wxALIGN_RIGHT|wxBOTTOM, 5);
  m_butSizer->Add(m_removeButton, 0, wxEXPAND|wxALIGN_RIGHT|wxBOTTOM, 5);

  m_sizer1 = new wxBoxSizer(wxVERTICAL);
  m_sizer1->Add(m_colorPicker, 0, wxEXPAND|wxALIGN_RIGHT);
  m_sizer1->Add(new wxStaticLine(this, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL),
                0, wxEXPAND|wxTOP|wxBOTTOM, 10);
  m_sizer1->Add(m_butSizer, 0, wxALIGN_RIGHT|wxBOTTOM, 5);

  m_sizer2 = new wxBoxSizer(wxVERTICAL);
  m_sizer2->Add(m_label, 0);
  m_sizer2->Add(m_gList, 1, wxEXPAND);

  m_hSizer = new wxBoxSizer(wxHORIZONTAL);
  m_hSizer->Add(m_sizer1, 1, wxEXPAND|wxRIGHT, 10);
  m_hSizer->Add(m_sizer2, 1, wxEXPAND);

  m_sizer = new wxBoxSizer(wxVERTICAL);
  m_sizer->Add(m_slider, 0, wxEXPAND|wxBOTTOM, 10);
  m_sizer->Add(m_hSizer, 1, wxEXPAND);
  
  SetSizer(m_sizer);

  if (lut != NULL)
  {
    SetColor();
  }

  m_gList->Connect(wxEVT_COMMAND_LISTBOX_SELECTED,
               (wxObjectEventFunction)
               (wxEventFunction)
               (wxCommandEventFunction)&CCustomModifyPanel::OnGSelect,
               NULL,
               this);

  m_slider->Connect(wxEVT_COMMAND_SLIDER_UPDATED,
               (wxObjectEventFunction)
               (wxEventFunction)
               (wxCommandEventFunction)&CCustomModifyPanel::OnGChange,
               NULL,
               this);

  m_slider->Connect(wxEVT_SCROLL_THUMBTRACK,
               (wxObjectEventFunction)
               (wxEventFunction)
               (wxScrollEventFunction)&CCustomModifyPanel::OnSlider,
               NULL,
               this);

  m_insertButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
               (wxObjectEventFunction)
               (wxEventFunction)
               (wxCommandEventFunction)&CCustomModifyPanel::OnInsert,
               NULL,
               this);

  m_removeButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
               (wxObjectEventFunction)
               (wxEventFunction)
               (wxCommandEventFunction)&CCustomModifyPanel::OnRemove,
               NULL,
               this);

  m_deselectButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
               (wxObjectEventFunction)
               (wxEventFunction)
               (wxCommandEventFunction)&CCustomModifyPanel::OnDeselect,
               NULL,
               this);

  CColorPicker::EvtColorChanged(*this,
                               -1,
                               (CColorChangedEventFunction)&CCustomModifyPanel::OnColorChange);

  return true;
}


//----------------------------------------
void CCustomModifyPanel::FillList() 
{
  if (m_LUT == NULL)
  {
    return;
  }
  
  m_gList->Clear();

  CObArray::iterator it;
  for (it = m_cust.begin() ; it != m_cust.end() ; it++)
  {
    CCustomColor* c = dynamic_cast<CCustomColor*>(*it);
    if (c == NULL)
    {
      CException e("ERROR in CCustomModifyPanel::SetList - at least one of the array custom color object is not a CCustomColor object", BRATHL_LOGIC_ERROR);
      throw(e);
    }

    m_gList->Append(c->GetStringXValue());
  }

  if (m_gList->GetCount() > 0)
  {
    m_gList->SetSelection(0);
    OnGSelect2();

    m_parent->UpdateRender();
  }


}
//----------------------------------------
void CCustomModifyPanel::SetList() 
{
  FillList();

  m_parent->SetLUT(m_LUT);
  m_parent->UpdateRender();


}

//----------------------------------------
void CCustomModifyPanel::SetFacets()
{
  if ( m_LUT->GetCust()->size() <= 0 )  
  {
    ResetCust();
  }
  else
  {
    m_cust.RemoveAll();
    CBratLookupTable::DupCustMap(*(m_LUT->GetCust()), m_cust);
  }

  ResetSlider();
  
  m_colorPicker->Reset();

  FillList();

}
//----------------------------------------
void CCustomModifyPanel::SetColor()
{
  if (m_LUT == NULL)
  {
    return;
  }
      
  m_cust.RemoveAll();
  CBratLookupTable::DupCustMap(*(m_LUT->GetCust()), m_cust);

  ResetSlider();
  
  m_colorPicker->Reset();

  FillList();

}

//----------------------------------------
void CCustomModifyPanel::SetLUT(CBratLookupTable* lut, bool updateParent)
{
  if (lut == NULL)
  {
    return;
  }

  DeleteLUT();

  m_LUT = new CBratLookupTable(*lut);
      
  if ( m_LUT->GetCust()->size() <= 0 )  
  {
    ResetCust();
    m_LUT->Custom(m_cust);
  }
  else
  {
    m_cust.RemoveAll();
    CBratLookupTable::DupCustMap(*(m_LUT->GetCust()), m_cust);
  }

  ResetSlider();
  
  m_colorPicker->Reset();

  FillList();

  if (updateParent)
  {
    m_parent->SetLUT(m_LUT);
  }
  m_parent->UpdateRender();


}

//----------------------------------------
void CCustomModifyPanel::ResetLUT(CBratLookupTable* lut)
{ 
  if (m_LUT == NULL)
  {
    return;
  }
  
  if (lut == NULL)
  {
    return;
  }
  
 
  *m_LUT = *lut;
  Reset();
}

//----------------------------------------
void CCustomModifyPanel::Reset()
{
  if (m_LUT == NULL)
  {
    return;
  }
  
  m_LUT->Reset();

  ResetCust();
  SetList();
  ResetSlider();
  m_colorPicker->Reset();
}

//----------------------------------------
void CCustomModifyPanel::ResetCust()
{

  CCustomColor *c1 = new CCustomColor(CVtkColor(0, 0, 0, 255), 0);
  CCustomColor *c2 = new CCustomColor(CVtkColor(0, 0, 0, 255), m_LUT->GetLookupTable()->GetNumberOfTableValues());

  m_cust.RemoveAll();
  m_cust.Insert(c1);
  m_cust.Insert(c2);

  CBratLookupTable::DupCustMap(m_cust, *(m_LUT->GetCust()));


}
//----------------------------------------
void CCustomModifyPanel::ResetSlider()
{
  m_slider->SetRange(0, m_LUT->GetLookupTable()->GetNumberOfTableValues());
  m_slider->SetValue(0);
}
//----------------------------------------
int32_t CCustomModifyPanel::InsertGReverse(int32_t gValue) 
{
  CCustomColor* cMin = dynamic_cast<CCustomColor*>(*(m_cust.begin()));
  CCustomColor* cMax = dynamic_cast<CCustomColor*>(*(m_cust.end() - 1));
  if ( (cMin == NULL) || (cMax == NULL) )
  {
    CException e("ERROR in CCustomModifyPanel::InsertGReverse - at least one of the array custom color object is not a CCustomColor object", BRATHL_LOGIC_ERROR);
    throw(e);
  }

  if ( (gValue < cMin->GetXValue()) || (gValue > cMax->GetXValue()) )
  {
    return -1;
  }

  int32_t value = gValue;

  bool duplicate = false;
  CObArray::reverse_iterator revit;


  value = gValue;

  for (revit = m_cust.rbegin() ; revit != m_cust.rend() ; revit++)
  {
    CCustomColor* c = dynamic_cast<CCustomColor*>(*revit);
    if (c == NULL)
    {
      CException e("ERROR in CCustomModifyPanel::InsertGReverse - at least one of the array custom color object is not a CCustomColor object", BRATHL_LOGIC_ERROR);
      throw(e);
    }

    if (c->GetXValue() < value)
    {
      duplicate = false;
      break;
    }
    else if (c->GetXValue() == value)
    {
      duplicate = true;
      value--;
    }
  }


  if (duplicate == true)
  {
    return -1;
  }
  
  CObArray::iterator it(revit.base());


  CCustomColor* cInserted = new CCustomColor(CVtkColor( *(m_colorPicker->GetVtkColor()) ), value );

  CObArray::iterator itInserted = m_cust.InsertAt(it, cInserted);

  CObArray::difference_type diff ;
  diff = distance (m_cust.begin ( ) , itInserted );
  
  return diff;


}
//----------------------------------------
int32_t CCustomModifyPanel::InsertG(int32_t gValue) 
{

  CCustomColor* cMin = dynamic_cast<CCustomColor*>(*(m_cust.begin()));
  CCustomColor* cMax = dynamic_cast<CCustomColor*>(*(m_cust.end() - 1));
  if ( (cMin == NULL) || (cMax == NULL) )
  {
    CException e("ERROR in CCustomModifyPanel::InsertG - at least one of the array custom color object is not a CCustomColor object", BRATHL_LOGIC_ERROR);
    throw(e);
  }


  if ( (gValue < cMin->GetXValue()) || (gValue > cMax->GetXValue()) )
  {
    return -1;
  }

  int32_t value = gValue;

  bool duplicate = false;
  CObArray::iterator it;

  for (it = m_cust.begin() ; it != m_cust.end() ; it++)
  {
    CCustomColor* c = dynamic_cast<CCustomColor*>(*it);
    if (c == NULL)
    {
      CException e("ERROR in CCustomModifyPanel::InsertG - at least one of the array custom color object is not a CCustomColor object", BRATHL_LOGIC_ERROR);
      throw(e);
    }

    if (c->GetXValue() > value)
    {
      duplicate = false;
      break;
    }
    else if (c->GetXValue() == value)
    {
      duplicate = true;
      value++;
    }
  }

  if (duplicate)
  {
    return InsertGReverse(gValue);
  }


  CCustomColor* cInserted = new CCustomColor(CVtkColor( *(m_colorPicker->GetVtkColor()) ), value );

  CObArray::iterator itInserted = m_cust.InsertAt(it, cInserted);

  CObArray::difference_type diff ;
  diff = distance (m_cust.begin ( ) , itInserted );
  
  return diff;


}
//----------------------------------------
void CCustomModifyPanel::OnColorChange( CColorChangedEvent& event )
{ 
  if (m_LUT == NULL)
  {
    return;
  }

  int32_t index = m_gList->GetSelection();
  if (index < 0)
  {
    return;
  }

  CCustomColor*  c = dynamic_cast<CCustomColor*>(m_cust.at(index));
  if (c == NULL)
  {
    CException e("ERROR in OnColorChange::OnColorChange - at least one of the array custom color object is not a CCustomColor object", BRATHL_LOGIC_ERROR);
    throw(e);
  }
  
  c->SetVtkColor(event.m_vtkColor);

  m_LUT->Custom(m_cust);

  m_parent->SetLUT(m_LUT);
  m_parent->UpdateRender();
    
}
//----------------------------------------
void CCustomModifyPanel::OnDeselect(wxCommandEvent& event) 
{
  m_gList->Deselect(m_gList->GetSelection());
}
//----------------------------------------
void CCustomModifyPanel::OnRemove(wxCommandEvent& event) 
{
  if (m_LUT == NULL)
  {
    return;
  }

  int32_t index = m_gList->GetSelection();
  if ( ( index <= 0) ||  (m_gList->GetSelection() >= static_cast<int32_t>(m_gList->GetCount()) - 1) )
  {
    ::wxMessageBox("Removing first or last color is not allowed", "Error");
    return;
  }

  if (index < 0)
  {
    return;
  }
  
  m_cust.Erase(m_cust.begin() + index);
  SetList();

  m_LUT->Custom(m_cust);

  m_parent->SetLUT(m_LUT);
  m_parent->UpdateRender();

}
//----------------------------------------
void CCustomModifyPanel::OnInsert(wxCommandEvent& event) 
{
  if (m_LUT == NULL)
  {
    return;
  }

  int32_t i = -1;

  if (m_gList->GetSelection() == static_cast<int32_t>(m_gList->GetCount()) - 1)
  {
    i = InsertG(m_slider->GetValue() - 1);
  }
  else
  {
    i = InsertG(m_slider->GetValue() + 1);
  }
  
  if (i == -1)
  {
    return;
  }

  SetList();

  m_gList->Select(i);
  
  m_LUT->Custom(m_cust);

  OnGSelect2();

  m_parent->SetLUT(m_LUT);
  m_parent->UpdateRender();

}
//----------------------------------------

void CCustomModifyPanel::OnSlider(wxScrollEvent& event)
{
  int32_t index = m_gList->GetSelection();
  if ( (index == 0) || (index == static_cast<int32_t>(m_gList->GetCount()) - 1) )
  {
    return;
  }

  int32_t min = atoi(m_gList->GetString(index - 1).c_str());
  int32_t max = atoi(m_gList->GetString(index + 1).c_str());

  if (event.GetPosition() <= min)
  {
    m_slider->SetValue(min + 1);
  }
  else if (event.GetPosition() >= max)
  {
    m_slider->SetValue(max - 1);
  }

}
//----------------------------------------
void CCustomModifyPanel::OnGChange(wxCommandEvent& event) 
{
  if (m_LUT == NULL)
  {
    return;
  }

  CCustomColor* c = NULL;

  int32_t index = m_gList->GetSelection();
  
  
  if ( (index == 0) || (index == static_cast<int32_t>(m_gList->GetCount()) - 1) )
  {
    ::wxMessageBox("Moving first or last color is not allowed", "Error");
    c = dynamic_cast<CCustomColor*>(m_cust.at(index));
    if (c == NULL)
    {
      CException e("ERROR in CCustomModifyPanel::OnGChange - at least one of the array custom color object is not a CCustomColor object", BRATHL_LOGIC_ERROR);
      throw(e);
    }
    m_slider->SetValue(c->GetXValue());
  
    return;
  }

  if (index > 0 )
  {
    c = dynamic_cast<CCustomColor*>(m_cust.at(index));
    if (c == NULL)
    {
      CException e("ERROR in CCustomModifyPanel::OnGChange - at least one of the array custom color object is not a CCustomColor object", BRATHL_LOGIC_ERROR);
      throw(e);
    }
    c->SetXValue(m_slider->GetValue());
    m_gList->SetString(index, c->GetStringXValue());
    m_LUT->Custom(m_cust);
  }
  else
  {
    /*
    double* color = m_LUT->GetLookupTable()->GetTableValue(m_slider->GetValue());
    CVtkColor vtkColor(color[0],
                       color[1],
                       color[2], 
                       color[3]); 
    */
    m_colorPicker->SetColor(*(c->GetVtkColor()));
  }

  m_parent->SetLUT(m_LUT);
  m_parent->UpdateRender();
}
//----------------------------------------
void CCustomModifyPanel::OnGSelect(wxCommandEvent& event) 
{
  if (m_LUT == NULL)
  {
    return;
  }
  
  OnGSelect2();

  m_parent->SetLUT(m_LUT);
  m_parent->UpdateRender();

}
//----------------------------------------
void CCustomModifyPanel::OnGSelect2() 
{
  if (m_LUT == NULL)
  {
    return;
  }

  int32_t index = m_gList->GetSelection();

  if ( ( index <= 0) ||  (m_gList->GetSelection() >= static_cast<int32_t>(m_gList->GetCount()) - 1) )
  {
    m_removeButton->Enable(false);
  }
  else
  {
    m_removeButton->Enable(true);
  }


  if (index < 0)
  {
    return;
  }

  CCustomColor* c = dynamic_cast<CCustomColor*>(m_cust.at(index));
  if (c == NULL)
  {
    CException e("ERROR in CCustomModifyPanel::OnGSelect - at least one of the array custom color object is not a CCustomColor object", BRATHL_LOGIC_ERROR);
    throw(e);
  }

  m_slider->SetValue(c->GetXValue());

  //m_parent->SetLUT(m_LUT);
  //m_parent->UpdateRender();
  
  /*
  double* color = m_LUT->GetLookupTable()->GetTableValue(m_slider->GetValue());
  CVtkColor vtkColor(color[0],
                     color[1],
                     color[2], 
                    color[3]); 
*/
  m_colorPicker->SetColor(*(c->GetVtkColor()));



}

// WDR: handler implementations for CCustomModifyPanel


//-------------------------------------------------------------
//------------------- CGradientModifyPanel class --------------------
//-------------------------------------------------------------

// WDR: event table for CGradientModifyPanel

BEGIN_EVENT_TABLE(CGradientModifyPanel,wxPanel)
END_EVENT_TABLE()
//----------------------------------------
CGradientModifyPanel::CGradientModifyPanel()
{
  Init();
}

CGradientModifyPanel::CGradientModifyPanel( CLUTPanel *parent, CBratLookupTable* lut, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style )
    //: wxPanel( parent, id, position, size, style )
{

  Init();
  if (! Create(parent, lut, id, position, size, style))
  {
    CException e("ERROR in CGradientModifyPanel ctor - Unenable to create wxWidgets (Create method returns false)", BRATHL_SYSTEM_ERROR);
    throw(e);
  }

}

//----------------------------------------
CGradientModifyPanel::~CGradientModifyPanel()
{
  DeleteLUT();
}

//----------------------------------------
void CGradientModifyPanel::DeleteLUT()
{
  if (m_LUT != NULL)
  {
    delete m_LUT;
    m_LUT = NULL;
  }
  
}

//----------------------------------------
void CGradientModifyPanel::Init()
{
  m_LUT = NULL;
  m_parent = NULL;
  m_leftColorPicker = NULL;
  m_rightColorPicker = NULL;
  m_sizer = NULL;

}

//----------------------------------------
bool CGradientModifyPanel::Create( CLUTPanel *parent, CBratLookupTable* lut, wxWindowID id,
                                   const wxPoint &position, const wxSize& size, long style ) 
{
  m_parent = parent;

  wxPanel::Create( parent, id, position, size, style );

  DeleteLUT();
  if (lut == NULL)
  {
    m_LUT = new CBratLookupTable();
  }
  else
  {
    m_LUT = new CBratLookupTable(*lut);
  }

  ID_GRDMODIFYPANEL_LEFT_COLOR_PICKER = ::wxNewId();
  ID_GRDMODIFYPANEL_RIGHT_COLOR_PICKER = ::wxNewId();

  m_leftColorPicker = new CColorPicker(this, "First Color", ID_GRDMODIFYPANEL_LEFT_COLOR_PICKER);
  m_rightColorPicker = new CColorPicker(this, "Last Color", ID_GRDMODIFYPANEL_RIGHT_COLOR_PICKER);

  SetColor();

  m_sizer = new wxBoxSizer(wxHORIZONTAL);
  m_sizer->Add(m_leftColorPicker, 1);
  m_sizer->Add(new wxStaticLine(this, -1, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL), 0, wxEXPAND|wxLEFT|wxRIGHT, 10);
  m_sizer->Add(m_rightColorPicker, 1);
  SetSizer(m_sizer);

  CColorPicker::EvtColorChanged(*this,
                               -1,
                               (CColorChangedEventFunction)&CGradientModifyPanel::OnColorChange);

  return true;
}
//----------------------------------------
void CGradientModifyPanel::OnColorChange( CColorChangedEvent& event )
{
  if (m_LUT == NULL)
  {
    return;
  }

  m_LUT->Gradient(*(m_leftColorPicker->GetVtkColor()), *(m_rightColorPicker->GetVtkColor()));

  m_parent->SetLUT(m_LUT);

  m_parent->UpdateRender();
    
}
//----------------------------------------
void CGradientModifyPanel::SetColor()
{
  if (m_LUT == NULL)
  {
    return;
  }
    
  if ( m_LUT->GetGrad()->size() >= 2 )  
  {
    CVtkColor* c1 = dynamic_cast<CVtkColor*>(m_LUT->GetGrad()->at(0));
    CVtkColor* c2 = dynamic_cast<CVtkColor*>(m_LUT->GetGrad()->at(1));
    if ( (c1 != NULL) && (c2!= NULL) )
    {
      m_leftColorPicker->SetColor(*c1);
      m_rightColorPicker->SetColor(*c2);
    }
  }

}

//----------------------------------------
void CGradientModifyPanel::SetLUT(CBratLookupTable* lut, bool updateParent)
{
  if (lut == NULL)
  {
    return;
  }

  DeleteLUT();

  m_LUT = new CBratLookupTable(*lut);
    
  if ( m_LUT->GetGrad()->size() >= 2 )  
  {
    CVtkColor* c1 = dynamic_cast<CVtkColor*>(m_LUT->GetGrad()->at(0));
    CVtkColor* c2 = dynamic_cast<CVtkColor*>(m_LUT->GetGrad()->at(1));
    if ( (c1 != NULL) && (c2!= NULL) )
    {
      m_leftColorPicker->SetColor(*c1);
      m_rightColorPicker->SetColor(*c2);
    }
    else
    {
      Reset();
    }
  }
  else
  {
    Reset();
  }

  if (updateParent)
  {
    m_parent->SetLUT(m_LUT);
  }

  m_parent->UpdateRender();

}

//----------------------------------------
void CGradientModifyPanel::Reset()
{
  if (m_LUT == NULL)
  {
    return;
  }
  
  m_LUT->Reset();
  m_leftColorPicker->Reset();
  m_rightColorPicker->Reset();
}

//----------------------------------------
void CGradientModifyPanel::ResetLUT(CBratLookupTable* lut)
{
  if (m_LUT == NULL)
  {
    return;
  }

  if (lut == NULL)
  {
    return;
  }
  
  
  *m_LUT = *lut;
  Reset();
}

// WDR: handler implementations for CGradientModifyPanel


//-------------------------------------------------------------
//------------------- CStandardModifyPanel class --------------------
//-------------------------------------------------------------

// WDR: event table for CStandardModifyPanel

BEGIN_EVENT_TABLE(CStandardModifyPanel,wxPanel)
END_EVENT_TABLE()

CStandardModifyPanel::CStandardModifyPanel()
{
  Init();
}

//----------------------------------------
CStandardModifyPanel::CStandardModifyPanel( CLUTPanel *parent, CBratLookupTable* lut, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) 
    //: wxPanel( parent, id, position, size, style )
{
  Init();
  if (! Create(parent, lut, id, position, size, style))
  {
    CException e("ERROR in CStandardModifyPanel ctor - Unenable to create wxWidgets (Create method returns false)", BRATHL_SYSTEM_ERROR);
    throw(e);
  }

}

//----------------------------------------
CStandardModifyPanel::~CStandardModifyPanel()
{
  DeleteLUT();
}
//----------------------------------------
void CStandardModifyPanel::DeleteLUT()
{
  if (m_LUT != NULL)
  {
    delete m_LUT;
    m_LUT = NULL;
  }
  
}

//----------------------------------------
bool CStandardModifyPanel::Create( CLUTPanel *parent, CBratLookupTable* lut, wxWindowID id,
                                   const wxPoint &position, const wxSize& size, long style ) 
{
  m_parent = parent;

  wxPanel::Create( parent, id, position, size, style );
  
  DeleteLUT();
  if (lut == NULL)
  {
    m_LUT = new CBratLookupTable();
  }
  else
  {
    m_LUT = new CBratLookupTable(*lut);
  }

  ID_STDMODIFYPANEL_DEFBOX = ::wxNewId();

  int32_t charwidth, charheight;
  CBratDisplayApp::DetermineCharSize(this, charwidth, charheight);

  m_label = new wxStaticText(this, -1, "Predefined Color Tables:");
  m_defBox = new wxListBox(this, ID_STDMODIFYPANEL_DEFBOX, wxDefaultPosition, wxSize(-1, (m_LUT->GetColorTableList()->size()+1)*charheight));
  for ( auto ii = m_LUT->GetColorTableList()->begin(); ii != m_LUT->GetColorTableList()->end(); ++ii )
	  m_defBox->AppendString(*ii);
  //femm m_defBox->Set( *(m_LUT->GetColorTableList()) );

  SetDefaultLUT();

  m_sizer = new wxBoxSizer(wxVERTICAL);
  m_sizer->Add(m_label, 0, wxBOTTOM, 3);
  m_sizer->Add(m_defBox, 1, wxEXPAND);
  SetSizer(m_sizer);

  
  m_defBox->Connect(wxEVT_COMMAND_LISTBOX_SELECTED,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 (wxCommandEventFunction)&CStandardModifyPanel::OnDefSelect,
                 NULL,
                 this);

  return true;
}
//----------------------------------------
void CStandardModifyPanel::DeInstallEventListeners()
{
  if (m_defBox != NULL)
  {
    m_defBox->Disconnect(wxEVT_COMMAND_LISTBOX_SELECTED);
  }
}

//----------------------------------------
void CStandardModifyPanel::OnDefSelect( wxCommandEvent &event )
{
  
  if (m_parent == NULL)
  {
    return;
  }
  
  if (m_LUT == NULL)
  {
    return;
  }
  
  std::string sel = event.GetString().ToStdString();
  if (sel.empty())
  {
    return;
  }
  
  m_LUT->ExecMethod(sel);

  m_parent->SetLUT(m_LUT);

  UpdateRender();

}
//----------------------------------------
void CStandardModifyPanel::Init()
{
  m_LUT = NULL;
  m_parent = NULL;
}

//----------------------------------------
void CStandardModifyPanel::SetDefaultLUT()
{
  if (m_LUT == NULL)
  {
    return;
  }

  
  if (m_LUT->IsCurrentReset())
  {
    m_defBox->SetStringSelection(m_LUT->GetDefaultColorTable());
    //m_defBox->SetStringSelection(m_LUT->GetCurrentFunction().c_str());
    m_LUT->ExecMethodDefaultColorTable();
    //m_LUT->ExecMethod(m_LUT->GetCurrentFunction());
  }
  else
  {
    m_defBox->SetStringSelection(m_LUT->GetCurrentFunction().c_str());
    m_LUT->ExecMethod(m_LUT->GetCurrentFunction());
  }

  UpdateRender();
}
//----------------------------------------
void CStandardModifyPanel::UpdateRender()
{
  Update();
  m_parent->UpdateRender();
}

//----------------------------------------
void CStandardModifyPanel::SetLUT(CBratLookupTable* lut, bool updateParent)
{
  if (lut == NULL)
  {
    return;
  }
 
  DeleteLUT();
  m_LUT = new CBratLookupTable(*lut);

  wxString currentFct = m_LUT->GetCurrentFunction().c_str();

  if ( (currentFct.IsEmpty() == false) && (currentFct != "Reset") )
  {
    m_defBox->SetStringSelection(currentFct);
  }
  else
  {
    Reset();
  }

  if (updateParent)
  {
    m_parent->SetLUT(m_LUT);
  }

  m_parent->UpdateRender();

}

//----------------------------------------
void CStandardModifyPanel::Reset()
{
  if (m_LUT == NULL)
  {
    return;
  }

  
  m_LUT->Reset();
  SetDefaultLUT();
}

//----------------------------------------
void CStandardModifyPanel::ResetLUT(CBratLookupTable* lut)
{
  if (m_LUT == NULL)
  {
    return;
  }

  if (lut == NULL)
  {
    return;
  }
  
  *m_LUT = *lut;
  Reset();
}
// WDR: handler implementations for CStandardModifyPanel


//-------------------------------------------------------------
//------------------- CLUTPanel class --------------------
//-------------------------------------------------------------


// WDR: event table for CLUTPanel

BEGIN_EVENT_TABLE(CLUTPanel,wxPanel)
END_EVENT_TABLE()
CLUTPanel::CLUTPanel()
{
  Init();
}

//----------------------------------------
CLUTPanel::CLUTPanel( CLUTFrame *parent, CBratLookupTable* lut, wxWindowID id, bool applyButton,
                      const wxPoint &position, const wxSize& size, long style ) 
    //: wxPanel( parent, id, position, size, style ) --> called in Create
{
  Init();
  if (! Create(parent, lut, id, applyButton, position, size, style))
  {
    CException e("ERROR in CLUTPanel ctor - Unenable to create wxWidgets (Create method returns false)", BRATHL_SYSTEM_ERROR);
    throw(e);
  }

}

//----------------------------------------
CLUTPanel::~CLUTPanel()
{
  if (m_vtkRend != NULL)
  {
    m_vtkRend->Delete();
    m_vtkRend = NULL;
  }

  if (m_scalarBarActor != NULL)
  {
    m_scalarBarActor->Delete();
    m_scalarBarActor = NULL;
  }

  if (m_vtkWidget != NULL)
  {
    m_vtkWidget->Delete();
    m_vtkWidget = NULL;
  }

  DeleteLUT();

}

//----------------------------------------
void CLUTPanel::DeleteLUT()
{
  if (m_LUT != NULL)
  {
    delete m_LUT;
    m_LUT = NULL;
  }
  
}

//----------------------------------------
bool CLUTPanel::Destroy()
{
  m_finished = true;
  return wxPanel::Destroy();
}
//----------------------------------------
void CLUTPanel::Init()
{
  m_apply = false;
  m_finished = false;

  m_LUT = NULL;

  m_scalarBarActor = NULL;
  m_vtkRend = NULL;
  m_vtkWidget = NULL;

  m_standardModifyPanel = NULL;
  m_gradientModifyPanel = NULL;
  m_customModifyPanel = NULL;

  m_applyButton = NULL;
  m_sizer = NULL;
  m_vSizer = NULL;

}

//----------------------------------------
bool CLUTPanel::Create( CLUTFrame *parent, CBratLookupTable* lut, wxWindowID id, bool applyButton,
    const wxPoint &position, const wxSize& size, long style ) 
    //: wxPanel( parent, id, position, size, style ) --> called in Create
{
  DeleteLUT();
  if (lut == NULL)
  {
    m_LUT = new CBratLookupTable();
  }
  else
  {
    m_LUT = new CBratLookupTable(*lut);
  }
  //m_LUT->Reset();

  m_parent = parent;

  wxPanel::Create( reinterpret_cast<wxWindow*>(parent), id, position, size, style );

  m_apply = applyButton;

  if (! CreateControls())
    return false;
  CreateLayout();
  InstallToolTips();
  InstallEventListeners();

  if (m_LUT->GetCust()->size() > 0)
  {
    SetModeToCustom();
  }
  else  if (m_LUT->GetGrad()->size() > 0)
  {
    SetModeToGradient();
  }
  else
  {
    SetModeToStandard();
  }

  UpdateRender();
  
  return true;
}
//----------------------------------------
bool CLUTPanel::CreateControls()
{
  ID_CLUTPANEL_APPLY_BUTTON = ::wxNewId();


  m_scalarBarActor = vtkScalarBarActor::New();
  m_scalarBarActor->SetLookupTable(m_LUT->GetLookupTable());
  m_scalarBarActor->SetOrientationToHorizontal();
  m_scalarBarActor->SetPosition(0, 0);
  m_scalarBarActor->SetPosition2(1, 6);
  m_scalarBarActor->SetNumberOfLabels(-1);

  m_vtkRend = vtkRenderer::New();
  m_vtkRend->SetBackground(1, 1, 1);
  m_vtkRend->AddActor(m_scalarBarActor);

  m_vtkWidget = new wxVTKRenderWindowInteractor(this, -1);
  m_vtkWidget->UseCaptureMouseOn();
  m_vtkWidget->GetRenderWindow()->AddRenderer(m_vtkRend);

  CBratLookupTable* lutStd = NULL;
  CBratLookupTable* lutGrad = NULL;
  CBratLookupTable* lutCust = NULL;
  
  if (m_LUT->GetCust()->size() > 0)
  {
    lutCust = m_LUT;
  }
  else  if (m_LUT->GetGrad()->size() > 0)
  {
    lutGrad = m_LUT;
  }
  else
  {
    lutStd = m_LUT;
  }
  m_standardModifyPanel = new CStandardModifyPanel(this, lutStd, -1);
  m_gradientModifyPanel = new CGradientModifyPanel(this, lutGrad, -1);
  m_customModifyPanel = new CCustomModifyPanel(this, lutCust, -1);

  if (m_apply)
  {
    m_applyButton = new wxButton(this, ID_CLUTPANEL_APPLY_BUTTON, "Apply");
  }


  return true;
}
//----------------------------------------
void CLUTPanel::CreateLayout()
{
  m_vSizer = new wxBoxSizer(wxVERTICAL);
  m_vSizer->Add(m_vtkWidget, 0, wxEXPAND|wxBOTTOM, 30);
  m_vSizer->Add(m_standardModifyPanel, 1, wxEXPAND);
  m_vSizer->Add(m_gradientModifyPanel, 1, wxEXPAND);
  m_vSizer->Add(m_customModifyPanel, 1, wxEXPAND);

  m_vtkWidget->SetMinSize(wxSize(100, 50));

  if (m_applyButton != NULL)
  {
    m_vSizer->Add(new wxStaticLine(this, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 
                  0, wxEXPAND|wxTOP|wxBOTTOM, 10);
    m_vSizer->Add(m_applyButton, 0, wxALIGN_RIGHT);
  }

  m_vSizer->Show(m_standardModifyPanel, false);
  m_vSizer->Show(m_gradientModifyPanel, false);
  m_vSizer->Show(m_customModifyPanel, false);

  m_sizer = new wxBoxSizer(wxVERTICAL);
  m_sizer->Add(m_vSizer, 1, wxEXPAND|wxALL, 10);
  SetSizer(m_vSizer);

  //SetModeToStandard();

}
//----------------------------------------
void CLUTPanel::InstallEventListeners()
{
  if (m_applyButton != NULL)
  {
    Connect(ID_CLUTPANEL_APPLY_BUTTON,
            wxEVT_COMMAND_BUTTON_CLICKED,
            (wxObjectEventFunction)
            (wxEventFunction)
            (wxCommandEventFunction)&CLUTPanel::OnApply);
  }

}

//----------------------------------------
void CLUTPanel::DeInstallEventListeners()
{
  if (m_applyButton != NULL)
  {
    Disconnect(ID_CLUTPANEL_APPLY_BUTTON, wxEVT_COMMAND_BUTTON_CLICKED);
  }

  if (m_standardModifyPanel != NULL)
  {
    m_standardModifyPanel->DeInstallEventListeners();
  }
}

//----------------------------------------
void CLUTPanel::InstallToolTips()
{
  if (m_applyButton != NULL)
  {
    m_applyButton->SetToolTip(new wxToolTip("Apply the current color table to the plot."));
  }

}
//----------------------------------------
void CLUTPanel::UpdateRender()
{
  Update();
  m_vtkWidget->Refresh();
}
//----------------------------------------
void CLUTPanel::SetModeToStandard(bool updateLUTPanel)
{
  m_vSizer->Show(m_gradientModifyPanel, false);
  m_vSizer->Show(m_customModifyPanel, false);
  //////////////////m_standardModifyPanel->SetLUT(m_LUT);
  m_vSizer->Show(m_standardModifyPanel, true);

  if (updateLUTPanel)
  {
    SetLUT(m_standardModifyPanel->GetLUT());
  }
  else
  {
    m_standardModifyPanel->SetLUT(m_LUT);
  }

  m_parent->UpdateMinSize();
}
//----------------------------------------
void CLUTPanel::SetModeToGradient(bool updateLUTPanel)
{
  m_vSizer->Show(m_standardModifyPanel, false);
  m_vSizer->Show(m_customModifyPanel, false);
  //////////////////////////////////m_gradientModifyPanel->SetLUT(m_LUT);
  m_vSizer->Show(m_gradientModifyPanel, true);

  if (updateLUTPanel)
  {
    SetLUT(m_gradientModifyPanel->GetLUT());
  }
  else
  {
    m_gradientModifyPanel->SetLUT(m_LUT);
  }
  m_parent->UpdateMinSize();
}
//----------------------------------------
void CLUTPanel::SetModeToCustom(bool updateLUTPanel)
{
  m_vSizer->Show(m_standardModifyPanel, false);
  m_vSizer->Show(m_gradientModifyPanel, false);
  //////////////////////////////////////////???m_customModifyPanel->SetLUT(m_LUT);
  m_vSizer->Show(m_customModifyPanel, true);
  
  if (updateLUTPanel)
  {
    SetLUT(m_customModifyPanel->GetLUT());
  }
  else
  {
    m_customModifyPanel->SetLUT(m_LUT);
  }
  m_parent->UpdateMinSize();
}
//----------------------------------------
void CLUTPanel::SetLUT(CBratLookupTable* lut)
{
  if (lut == NULL)
  {
    return;
  }

  DeleteLUT();
  m_LUT = new CBratLookupTable(*lut);  

  m_scalarBarActor->SetLookupTable(m_LUT->GetLookupTable());


  UpdateRender();
}

//----------------------------------------
void CLUTPanel::ResetLUT()
{
  if (m_LUT == NULL)
  {
    return;
  }
  
  m_LUT->Reset();
  m_standardModifyPanel->Reset();
  m_gradientModifyPanel->Reset();
  m_customModifyPanel->Reset();
  
  UpdateRender();
}
//----------------------------------------
void CLUTPanel::UpdateForFacets(int32_t n)
{
  m_standardModifyPanel->GetLUT()->SetFacets(n);
  m_gradientModifyPanel->GetLUT()->SetFacets(n);
  m_customModifyPanel->GetLUT()->SetFacets(n);
  m_customModifyPanel->SetFacets();
  //m_customModifyPanel->SetLUT(m_LUT);
}

//----------------------------------------
void CLUTPanel::UpdateForCurveLinear()
{
  m_standardModifyPanel->GetLUT()->SetCurveLinear();
  m_gradientModifyPanel->GetLUT()->SetCurveLinear();
  m_customModifyPanel->GetLUT()->SetCurveLinear();
  ///???m_customModifyPanel->SetLUT(m_LUT)
}

//----------------------------------------
void CLUTPanel::UpdateForCurveCosinus()
{
  m_standardModifyPanel->GetLUT()->SetCurveCosinus();
  m_gradientModifyPanel->GetLUT()->SetCurveCosinus();
  m_customModifyPanel->GetLUT()->SetCurveCosinus();
  ///???m_customModifyPanel->SetLUT(m_LUT)
}

//----------------------------------------
void CLUTPanel::UpdateForCurveSQRT()
{
  m_standardModifyPanel->GetLUT()->SetCurveSQRT();
  m_gradientModifyPanel->GetLUT()->SetCurveSQRT();
  m_customModifyPanel->GetLUT()->SetCurveSQRT();
  ///???m_customModifyPanel->SetLUT(m_LUT)
}

//----------------------------------------
void CLUTPanel::EvtLutChanged(wxWindow& window, int32_t winId, const CLutChangedEventFunction& method,
                                wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(winId,
                 wxEVT_CLUT_CHANGED,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}

//----------------------------------------
void CLUTPanel::EvtLutFrameClose(wxWindow& window, int32_t winId, const CLutFrameCloseEventFunction& method,
                                wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(winId,
                 wxEVT_CLUT_CLOSE,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}

//----------------------------------------
void CLUTPanel::OnApply(wxCommandEvent& event)
{
  if (m_finished)
  {
    return;
  }
  m_parent->PostLutChangedEvent(m_LUT);

}

// WDR: handler implementations for CLUTPanel




