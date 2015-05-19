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

#ifndef __FieldsTreeCtrl_H__
#define __FieldsTreeCtrl_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "FieldsTreeCtrl.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "List.h"
#include "Product.h"
#include "ProductNetCdf.h"
#include "Field.h"
using namespace brathl;

#include "BratGui_wdr.h"

#include "TreeCtrl.h"


// WDR: class declarations

//----------------------------------------------------------------------------
// CUnitFieldDlg
//----------------------------------------------------------------------------

class CUnitFieldDlg: public wxDialog
{
public:
  // constructors and destructors
  CUnitFieldDlg( wxWindow *parent, wxWindowID id, const wxString &title,
      CField* field,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxDEFAULT_DIALOG_STYLE );

  virtual ~CUnitFieldDlg();

  // WDR: method declarations for CUnitFieldDlg
  wxTextCtrl* GetUnitfieldNewunit()  { return (wxTextCtrl*) FindWindow( ID_UNITFIELD_NEWUNIT ); }
  wxTextCtrl* GetUnitfieldActualunit()  { return (wxTextCtrl*) FindWindow( ID_UNITFIELD_ACTUALUNIT ); }
  wxTextCtrl* GetUnitfieldField()  { return (wxTextCtrl*) FindWindow( ID_UNITFIELD_FIELD ); }
    
  const string& GetUnit() {return m_newUnit;};

private:
  void Init();
  void EnableCtrl();

private:
    // WDR: member variable declarations for CUnitFieldDlg
    CField* m_field;
    string m_newUnit;

private:
    // WDR: handler declarations for CUnitFieldDlg
    void OnOk( wxCommandEvent &event );
    void OnCancel( wxCommandEvent &event );

private:
    DECLARE_CLASS(CUnitFieldDlg)
    DECLARE_EVENT_TABLE()
};
//----------------------------------------------------------------------------
// CFieldsTreeItemData
//----------------------------------------------------------------------------
class CFieldsTreeItemData : public wxTreeItemData
{
public:
  CFieldsTreeItemData(CField* field);

  CField* GetField() { return m_field; }
  
  wxString GetSplittedFieldDescr() { return m_splittedFieldDescr; }
  
  void SetSplittedFieldDescr();
  void SetSplittedFieldDescr(CField* field);

private:

private:
  CField* m_field;
  wxString m_splittedFieldDescr;
};


//----------------------------------------------------------------------------
// CFieldsTreeCtrl
//----------------------------------------------------------------------------

class CFieldsTreeCtrl: public CTreeCtrl
{
public:
  // constructors and destructors
  CFieldsTreeCtrl( wxWindow *parent, wxWindowID id = -1,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = 0 );
  
  // WDR: method declarations for CFieldsTreeCtrl
  void InsertProduct(CProduct* product);
  wxTreeItemId InsertField(const wxTreeItemId& parentId, CObjectTreeNode* node);

  void DeInstallEventListeners();

  virtual void ShowMenu(wxTreeItemId id, const wxPoint& pt);

  wxString GetFieldUnit(const wxTreeItemId& item);

  CFieldsTreeItemData* GetCurrentItemDataValue();
  CField* GetCurrentField();
  wxString GetCurrentRecord();

  CFieldsTreeItemData* GetItemDataValue(const wxTreeItemId& id);
  CField* GetField(const wxTreeItemId& id);

  wxString GetRecord(const wxTreeItemId& id);

  int32_t GetRecordCount();
  wxTreeItemId GetFirstRecordId();

  void UpdateFieldDescription();
  void UpdateFieldDescription(const wxTreeItemId& id);


  CProduct* GetProduct() { return m_product; };
    
protected:
  // WDR: member variable declarations for CFieldsTreeCtrl
  CProduct* m_product;
    
protected:
  // WDR: handler declarations for CFieldsTreeCtrl
    //void OnSetAsX( wxCommandEvent &event );
  void OnBeginDrag( wxTreeEvent &event );
  void OnEndDrag( wxTreeEvent &event );
  void OnGetToolTip( wxTreeEvent &event );
  //virtual void OnContextMenu(wxContextMenuEvent& event);
  //virtual void OnItemMenu(wxTreeEvent& event);

private:
    DECLARE_EVENT_TABLE()
};




#endif
