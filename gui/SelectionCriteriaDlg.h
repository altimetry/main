/*
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

#ifndef __SelectionCriteriaDlg_H__
#define __SelectionCriteriaDlg_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "BratGui_wdr.h"

#include "LabeledTextCtrl.h"


// WDR: class declarations

//----------------------------------------------------------------------------
// CSelectionCriteriaDlg
//----------------------------------------------------------------------------

class CSelectionCriteriaDlg: public wxDialog
{
public:
  // constructors and destructors
  CSelectionCriteriaDlg( wxWindow *parent, wxWindowID id, const wxString &title,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

  virtual ~CSelectionCriteriaDlg();
  
  // WDR: method declarations for CSelectionCriteriaDlg
  CLabeledTextCtrl* GetCritPassList()  { return (CLabeledTextCtrl*) FindWindow( ID_CRIT_PASS_LIST ); }
  CLabeledTextCtrl* GetCritPassTo()  { return (CLabeledTextCtrl*) FindWindow( ID_CRIT_PASS_TO ); }
  CLabeledTextCtrl* GetCritPassFrom()  { return (CLabeledTextCtrl*) FindWindow( ID_CRIT_PASS_FROM ); }
  CLabeledTextCtrl* GetCritCycleTo()  { return (CLabeledTextCtrl*) FindWindow( ID_CRIT_CYCLE_TO ); }
  CLabeledTextCtrl* GetCritCycleFrom()  { return (CLabeledTextCtrl*) FindWindow( ID_CRIT_CYCLE_FROM ); }
  CLabeledTextCtrl* GetCritLonEast()  { return (CLabeledTextCtrl*) FindWindow( ID_CRIT_LON_EAST ); }
  CLabeledTextCtrl* GetCritLatSouth()  { return (CLabeledTextCtrl*) FindWindow( ID_CRIT_LAT_SOUTH ); }
  CLabeledTextCtrl* GetCritLatNorth()  { return (CLabeledTextCtrl*) FindWindow( ID_CRIT_LAT_NORTH ); }
  CLabeledTextCtrl* GetCritLonWest()  { return (CLabeledTextCtrl*) FindWindow( ID_CRIT_LON_WEST ); }
  CLabeledTextCtrl* GetCritDateMax()  { return (CLabeledTextCtrl*) FindWindow( ID_CRIT_DATE_MAX ); }
  CLabeledTextCtrl* GetCritDateMin()  { return (CLabeledTextCtrl*) FindWindow( ID_CRIT_DATE_MIN ); }
  wxListBox* GetCritProductList()  { return (wxListBox*) FindWindow( ID_CRIT_PRODUCT_LIST ); }
    
private:
  // WDR: member variable declarations for CSelectionCriteriaDlg
  int32_t m_currentProduct;

private:

  void Init();
  void FillProductList();
  void EnableCtrl();
  void ShowCriteriaSizer();

  CProduct* GetProduct();
  CProduct* GetProduct(int32_t sel);
  
  void ClearCriteria();

  void FillCriteria();
  void FillCriteria(CProduct* product);

  void StoreCriteriaValue(bool checkDefault = true);
  void StoreCriteriaValue(CProduct* product, bool checkDefault = true);

  void StoreCriteriaValueLatLon(CProduct* product, bool checkDefault = true);
  void StoreCriteriaValueDatetime(CProduct* product, bool checkDefault = true);
  void StoreCriteriaValueCycle(CProduct* product, bool checkDefault = true);
  void StoreCriteriaValuePassInt(CProduct* product, bool checkDefault = true);
  void StoreCriteriaValuePassString(CProduct* product, bool checkDefault = true);

  // WDR: handler declarations for CSelectionCriteriaDlg
  void StoreCriteria(CValueChangedEvent& event);
  void OnProductList( wxCommandEvent &event );
  void OnOk( wxCommandEvent &event );
  void OnCancel( wxCommandEvent &event );

private:
    DECLARE_CLASS(CSelectionCriteriaDlg)
    DECLARE_EVENT_TABLE()
};




#endif
