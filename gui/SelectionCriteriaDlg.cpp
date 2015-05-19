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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "SelectionCriteriaDlg.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <typeinfo>

#include "Product.h"
#include "ProductJason2.h"
#include "ProductNetCdf.h"
using namespace brathl;

#include "BratGui.h"
#include "SelectionCriteriaDlg.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// CSelectionCriteriaDlg
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(CSelectionCriteriaDlg,wxDialog)

// WDR: event table for CSelectionCriteriaDlg

BEGIN_EVENT_TABLE(CSelectionCriteriaDlg,wxDialog)
    EVT_BUTTON( wxID_OK, CSelectionCriteriaDlg::OnOk )
    EVT_BUTTON( wxID_CANCEL, CSelectionCriteriaDlg::OnCancel )
    EVT_LISTBOX( ID_CRIT_PRODUCT_LIST, CSelectionCriteriaDlg::OnProductList )
END_EVENT_TABLE()

CSelectionCriteriaDlg::CSelectionCriteriaDlg( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxDialog( parent, id, title, position, size, style )
{
  // WDR: dialog function SelectionCriteriaDlg for CSelectionCriteriaDlg

      //----------------------------
  SelectionCriteriaDlg( this, TRUE );
  //----------------------------

  Init();
  EnableCtrl();

}


//----------------------------------------
CSelectionCriteriaDlg::~CSelectionCriteriaDlg()
{
}
//----------------------------------------
void CSelectionCriteriaDlg::Init()
{

  m_currentProduct = -1;
  FillProductList();

  CLabeledTextCtrl::EvtValueChanged(*this,
                                    ID_CRIT_LAT_NORTH,
                                    (CValueChangedEventFunction)&CSelectionCriteriaDlg::StoreCriteria);
  CLabeledTextCtrl::EvtValueChanged(*this,
                                    ID_CRIT_LAT_SOUTH,
                                    (CValueChangedEventFunction)&CSelectionCriteriaDlg::StoreCriteria);
  CLabeledTextCtrl::EvtValueChanged(*this,
                                    ID_CRIT_LON_WEST,
                                    (CValueChangedEventFunction)&CSelectionCriteriaDlg::StoreCriteria);
  CLabeledTextCtrl::EvtValueChanged(*this,
                                    ID_CRIT_LON_EAST,
                                    (CValueChangedEventFunction)&CSelectionCriteriaDlg::StoreCriteria);
  CLabeledTextCtrl::EvtValueChanged(*this,
                                    ID_CRIT_DATE_MIN,
                                    (CValueChangedEventFunction)&CSelectionCriteriaDlg::StoreCriteria);
  CLabeledTextCtrl::EvtValueChanged(*this,
                                    ID_CRIT_DATE_MAX,
                                    (CValueChangedEventFunction)&CSelectionCriteriaDlg::StoreCriteria);
  CLabeledTextCtrl::EvtValueChanged(*this,
                                    ID_CRIT_CYCLE_FROM,
                                    (CValueChangedEventFunction)&CSelectionCriteriaDlg::StoreCriteria);
  CLabeledTextCtrl::EvtValueChanged(*this,
                                    ID_CRIT_CYCLE_TO,
                                    (CValueChangedEventFunction)&CSelectionCriteriaDlg::StoreCriteria);
  CLabeledTextCtrl::EvtValueChanged(*this,
                                    ID_CRIT_PASS_FROM,
                                    (CValueChangedEventFunction)&CSelectionCriteriaDlg::StoreCriteria);
  CLabeledTextCtrl::EvtValueChanged(*this,
                                    ID_CRIT_PASS_TO,
                                    (CValueChangedEventFunction)&CSelectionCriteriaDlg::StoreCriteria);
  CLabeledTextCtrl::EvtValueChanged(*this,
                                    ID_CRIT_PASS_LIST,
                                    (CValueChangedEventFunction)&CSelectionCriteriaDlg::StoreCriteria);
}

//----------------------------------------
void CSelectionCriteriaDlg::StoreCriteria(CValueChangedEvent& event)
{

  CProduct* product = GetProduct(m_currentProduct);

  switch (event.m_id)
  {
  case ID_CRIT_LAT_NORTH:
  case ID_CRIT_LAT_SOUTH:
  case ID_CRIT_LON_EAST:
  case ID_CRIT_LON_WEST:
    StoreCriteriaValueLatLon(product, false);
    break;
  case ID_CRIT_DATE_MIN:
  case ID_CRIT_DATE_MAX:
    StoreCriteriaValueDatetime(product, false);
    break;
  case ID_CRIT_CYCLE_FROM:
  case ID_CRIT_CYCLE_TO:
    StoreCriteriaValueCycle(product, false);
    break;
  case ID_CRIT_PASS_FROM:
  case ID_CRIT_PASS_TO:
    StoreCriteriaValuePassInt(product, false);
    break;
  case ID_CRIT_PASS_LIST:
    StoreCriteriaValuePassString(product, false);
    break;
  default:
    break;
  }


}
//----------------------------------------
void CSelectionCriteriaDlg::FillProductList()
{
  CStringArray array;
  CMapProduct::GetInstance().GetProductKeysWithCriteria(array);

  wxArrayString labels;
  CBratGuiApp::CStringArrayToWxArray(array, labels);

  GetCritProductList()->Clear();

  GetCritProductList()->Set(labels);
}

//----------------------------------------
void CSelectionCriteriaDlg::EnableCtrl()
{

  if (GetProduct() != NULL)
  {
    GetCritLatSouth()->Enable(CTools::IsDefaultValue(GetProduct()->GetForceLatMinCriteriaValue()));
    GetCritLatNorth()->Enable(CTools::IsDefaultValue(GetProduct()->GetForceLatMaxCriteriaValue()));
  }

  ShowCriteriaSizer();
}
//----------------------------------------
void CSelectionCriteriaDlg::ShowCriteriaSizer()
{
  int32_t pos = GetCritProductList()->GetSelection();
  bool showCriteria = (pos >= 0);
  glbCritSizer->Show(showCriteria);
  glbSelectionCriteriaDlgSizer->Fit(this);
  glbCritSizer->Layout();
  glbSelectionCriteriaDlgSizer->Layout();

  if (!showCriteria)
  {
    return;
  }


  wxString label = GetCritProductList()->GetStringSelection();
  CProduct* product = dynamic_cast<CProduct*>( CMapProduct::GetInstance().Exists((const char *)label.c_str()) );

  if (product == NULL)
  {
    glbCritSizer->Show(false);
    glbSelectionCriteriaDlgSizer->Fit(this);
    return;
  }

  //this->Freeze();

  glbCritSizer->Show(true);


  bool showDateCrit = product->HasDatetimeCriteriaInfo();
  bool showLatLonCrit = product->HasLatLonCriteriaInfo();
  bool showCycleCrit = product->HasCycleCriteriaInfo();
  bool showPassFromToCrit = product->HasPassIntCriteriaInfo();
  bool showPassListCrit = product->HasPassStringCriteriaInfo();

  glbCritDateSizer->Show(showDateCrit);
  glbCritLatLonSizer->Show(showLatLonCrit);
  glbCritCycleFromToSizer->Show(showCycleCrit);
  glbCritPassFromToSizer->Show(showPassFromToCrit);
  glbCritPassListSizer->Show(showPassListCrit);


  CProductNetCdf* productNetCdf = dynamic_cast<CProductNetCdf*>( CMapProduct::GetInstance().Exists((const char *)label.c_str()) );
  if (productNetCdf != NULL)
  {
    if (typeid(*productNetCdf) != typeid(CProductJason2))
    {
      glbCritDateSizer->Show(true);
      glbCritLatLonSizer->Show(true);
      glbCritCycleFromToSizer->Show(true);
      glbCritPassFromToSizer->Show(true);
      //glbCritPassListSizer->Show(true);
    }
  }


  glbSelectionCriteriaDlgSizer->Fit(this);
  glbCritSizer->Fit(this);
  glbSelectionCriteriaDlgSizer->Layout();
  glbCritSizer->Layout();
  glbSelectionCriteriaDlgSizer->Fit(this);

  //this->Thaw();




}
//----------------------------------------
CProduct* CSelectionCriteriaDlg::GetProduct()
{
  return GetProduct(GetCritProductList()->GetSelection());

}
//----------------------------------------
CProduct* CSelectionCriteriaDlg::GetProduct(int32_t sel)
{
  if (sel < 0)
  {
    return NULL;
  }

  wxString label = GetCritProductList()->GetString(sel);

  CProduct* product = dynamic_cast<CProduct*>( CMapProduct::GetInstance().Exists((const char *)label.c_str()) );

  return product;
}
//----------------------------------------
void CSelectionCriteriaDlg::StoreCriteriaValue(bool checkDefault /* = true */)
{
  StoreCriteriaValue(GetProduct(), checkDefault);
}
//----------------------------------------
void CSelectionCriteriaDlg::StoreCriteriaValueLatLon(CProduct* product, bool checkDefault /* = true */)
{
  if (product == NULL)
  {
    return;
  }

  if (!product->HasLatLonCriteria())
  {
    return;
  }

  CCriteriaLatLon* criteria = product->GetLatLonCriteria();
  criteria->SetDefaultValue();

  double defaultValue;
  CTools::SetDefaultValue(defaultValue);
  double lat;
  double lon;

  GetCritLatSouth()->GetValue(lat, defaultValue, -90, 90);
  GetCritLonWest()->GetValue(lon, defaultValue);
/*
  if (checkDefault)
  {
    if (CTools::IsDefaultValue(lat) && !CTools::IsDefaultValue(lon))
    {
      lat = -90;
    }

    if (!CTools::IsDefaultValue(lat) && CTools::IsDefaultValue(lon))
    {
      lon = -180;
    }
  }
*/
  GetCritLatSouth()->SetValue(lat);
  GetCritLonWest()->SetValue(lon);

  CLatLonPoint left(lat, lon);

  GetCritLatNorth()->GetValue(lat, defaultValue, -90, 90);
  GetCritLonEast()->GetValue(lon, defaultValue);
/*
  if (checkDefault)
  {
    if (CTools::IsDefaultValue(lat) && !CTools::IsDefaultValue(lon))
    {
      lat = 90;
    }

    if (!CTools::IsDefaultValue(lat) && CTools::IsDefaultValue(lon))
    {
      lon = 180;
    }
  }
*/
  GetCritLatNorth()->SetValue(lat);
  GetCritLonEast()->SetValue(lon);

  CLatLonPoint right(lat, lon);

  criteria->Set(left, right);

}
//----------------------------------------
void CSelectionCriteriaDlg::StoreCriteriaValueDatetime(CProduct* product, bool checkDefault /* = true */)
{
  if (product == NULL)
  {
    return;
  }

  if (!product->HasDatetimeCriteria())
  {
    return;
  }

  CCriteriaDatetime* criteria = product->GetDatetimeCriteria();
  criteria->SetDefaultValue();

  double defaultValue;
  CTools::SetDefaultValue(defaultValue);

  CDate dateMin;
  CDate dateMax;
  double min = 0.0;
  double max = 0.0;

  dateMin.InitDateZero();
  dateMax.SetDateNow();

/*
  CDate dateMin;
  CDate dateMax;
  dateMin.SetDefaultValue();
  dateMax.SetDefaultValue();

  GetCritDateMin()->GetValue(dateMin, defaultValue);
  GetCritDateMax()->GetValue(dateMax, defaultValue);
*/
  //GetCritDateMin()->GetValueAsDate(min, defaultValue, dateMin.Value(), dateMax.Value());
  //GetCritDateMax()->GetValueAsDate(max, defaultValue, dateMin.Value(), dateMax.Value());
  GetCritDateMin()->GetValueAsDate(min, defaultValue);
  GetCritDateMax()->GetValueAsDate(max, defaultValue);

  // to set right date string format
  GetCritDateMin()->SetValueAsDate(min, "");
  GetCritDateMax()->SetValueAsDate(max, "");

  criteria->Set(min, max);

}
//----------------------------------------
void CSelectionCriteriaDlg::StoreCriteriaValueCycle(CProduct* product, bool checkDefault /* = true */)
{

  if (product == NULL)
  {
    return;
  }

  if (!product->HasCycleCriteria())
  {
    return;
  }
  int32_t defaultValue;
  CTools::SetDefaultValue(defaultValue);

  int32_t min;
  int32_t max;

  CCriteriaCycle* criteria = product->GetCycleCriteria();
  criteria->SetDefaultValue();

  GetCritCycleFrom()->GetValue(min, defaultValue);
  GetCritCycleTo()->GetValue(max, defaultValue);

  criteria->Set(min, max);

}
//----------------------------------------
void CSelectionCriteriaDlg::StoreCriteriaValuePassInt(CProduct* product, bool checkDefault /* = true */)
{
  if (product == NULL)
  {
    return;
  }

  if (!product->HasPassIntCriteria())
  {
    return;
  }

  int32_t defaultValue;
  CTools::SetDefaultValue(defaultValue);

  int32_t min;
  int32_t max;

  CCriteriaPassInt* criteria = product->GetPassIntCriteria();
  criteria->SetDefaultValue();

  GetCritPassFrom()->GetValue(min, defaultValue);
  GetCritPassTo()->GetValue(max, defaultValue);

  criteria->Set(min, max);

}
//----------------------------------------
void CSelectionCriteriaDlg::StoreCriteriaValuePassString(CProduct* product, bool checkDefault /* = true */)
{
  if (product == NULL)
  {
    return;
  }

  if (!product->HasPassStringCriteria())
  {
    return;
  }
  wxString passes;

  CCriteriaPassString* criteria = product->GetPassStringCriteria();
  criteria->SetDefaultValue();

  GetCritPassList()->GetValue(passes);

  criteria->Set((const char *)passes.c_str());

}
//----------------------------------------
void CSelectionCriteriaDlg::StoreCriteriaValue(CProduct* product, bool checkDefault /* = true */)
{

  if (product == NULL)
  {
    return;
  }

  StoreCriteriaValueLatLon(product, checkDefault);
  StoreCriteriaValueDatetime(product, checkDefault);
  StoreCriteriaValueCycle(product, checkDefault);
  StoreCriteriaValuePassInt(product, checkDefault);
  StoreCriteriaValuePassString(product, checkDefault);

}
//----------------------------------------
void CSelectionCriteriaDlg::ClearCriteria()
{
  GetCritLonWest()->Clear();
  GetCritLonEast()->Clear();
  GetCritLatSouth()->Clear();
  GetCritLatNorth()->Clear();

  GetCritDateMin()->Clear();
  GetCritDateMax()->Clear();

  GetCritCycleFrom()->Clear();
  GetCritCycleTo()->Clear();

  GetCritPassFrom()->Clear();
  GetCritPassTo()->Clear();

  GetCritPassList()->Clear();

}

//----------------------------------------
void CSelectionCriteriaDlg::FillCriteria()
{
  FillCriteria(GetProduct());
}
//----------------------------------------
void CSelectionCriteriaDlg::FillCriteria(CProduct* product)
{
  ClearCriteria();

  if (product == NULL)
  {
    return;
  }

  if (product->IsSetLatLonCriteria())
  {
    CCriteriaLatLon* criteria = product->GetLatLonCriteria();

    GetCritLonWest()->SetValue(criteria->GetLowerLeftLon());
    GetCritLonEast()->SetValue(criteria->GetUpperRightLon());

    GetCritLatSouth()->SetValue(criteria->GetLowerLeftLat());
    GetCritLatNorth()->SetValue(criteria->GetUpperRightLat());

  }

  if (!CTools::IsDefaultValue(product->GetForceLatMinCriteriaValue()))
  {
    GetCritLatSouth()->SetValue(product->GetForceLatMinCriteriaValue());
  }
  if (!CTools::IsDefaultValue(product->GetForceLatMaxCriteriaValue()))
  {
    GetCritLatNorth()->SetValue(product->GetForceLatMaxCriteriaValue());
  }

  if (product->IsSetDatetimeCriteria())
  {
    CCriteriaDatetime* criteria = product->GetDatetimeCriteria();

    GetCritDateMin()->SetValue( *(criteria->GetFrom()) );
    GetCritDateMax()->SetValue( *(criteria->GetTo()) );
  }
  if (product->IsSetCycleCriteria())
  {
    CCriteriaCycle* criteria = product->GetCycleCriteria();

    GetCritCycleFrom()->SetValue(criteria->GetFrom());
    GetCritCycleTo()->SetValue(criteria->GetTo());
  }
  if (product->IsSetPassIntCriteria())
  {
    CCriteriaPassInt* criteria = product->GetPassIntCriteria();

    GetCritPassFrom()->SetValue(criteria->GetFrom());
    GetCritPassTo()->SetValue(criteria->GetTo());
  }
  if (product->IsSetPassStringCriteria())
  {
    CCriteriaPassString* criteria = product->GetPassStringCriteria();

    GetCritPassList()->SetValue(criteria->GetAsText());
  }

}

//----------------------------------------
// WDR: handler implementations for CSelectionCriteriaDlg
//----------------------------------------

//----------------------------------------
void CSelectionCriteriaDlg::OnProductList( wxCommandEvent &event )
{

  if (m_currentProduct >= 0)
  {
    StoreCriteriaValue(GetProduct(m_currentProduct));
  }


  m_currentProduct = GetCritProductList()->GetSelection();

  FillCriteria();

  EnableCtrl();
}


//----------------------------------------
void CSelectionCriteriaDlg::OnOk(wxCommandEvent &event)
{
  if (m_currentProduct >= 0)
  {
    StoreCriteriaValue(GetProduct(m_currentProduct));
  }

  event.Skip();
}

//----------------------------------------
void CSelectionCriteriaDlg::OnCancel(wxCommandEvent &event)
{
  event.Skip();
}

//----------------------------------------



