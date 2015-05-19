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

#ifndef __OperationTreeCtrl_H__
#define __OperationTreeCtrl_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "OperationTreeCtrl.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/dnd.h"

#include "Field.h"
using namespace brathl;


#include "BratGui_wdr.h"

#include "Dnd.h"
#include "TreeCtrl.h"
#include "Formula.h"

class COperationTreeCtrl;

// WDR: class declarations

//----------------------------------------------------------------------------
// CSelectRecord
//----------------------------------------------------------------------------

class CSelectRecord: public wxDialog
{
public:
    // constructors and destructors
    CSelectRecord( wxWindow *parent,  CProduct* product, wxWindowID id, const wxString &title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE );
    virtual ~CSelectRecord();
    
    // WDR: method declarations for CSelectRecord
    wxListBox* GetRecordList()  { return (wxListBox*) FindWindow( ID_RECORD_LIST ); }
    
    wxString GetRecordName() { return m_recordName; };

private:
    // WDR: member variable declarations for CSelectRecord
    wxString m_recordName;  
private:
    // WDR: handler declarations for CSelectRecord
    void OnRecordListDblClick( wxCommandEvent &event );
    void OnCancel( wxCommandEvent &event );
    void OnOk( wxCommandEvent &event );

private:
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// CFieldTextCtrlDropTarget
//----------------------------------------------------------------------------
class CFieldTextCtrlDropTarget : public wxDropTarget
{
public:

//  CFieldTextCtrlDropTarget(wxTextCtrl* pOwner) 
//         : wxDropTarget(new CDndFieldDataObject())
//         {
//          m_pOwner = pOwner;
//         };

  CFieldTextCtrlDropTarget(COperationPanel* pOwner) 
         : wxDropTarget(new CDndFieldDataObject())
         {
          m_pOwner = pOwner;
         };
  
  virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult def);
  virtual wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult def);

private:
    //wxTextCtrl* m_pOwner;
    COperationPanel*m_pOwner; 
};


//----------------------------------------------------------------------------
// CFieldDropTarget
//----------------------------------------------------------------------------
class CFieldDropTarget : public wxDropTarget
{
public:

  CFieldDropTarget(COperationTreeCtrl* pOwner) 
         : wxDropTarget(new CDndFieldDataObject())
         {
          m_pOwner = pOwner;
         };
  
  virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult def);
  virtual wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult def);

private:
    COperationTreeCtrl* m_pOwner;
};



//----------------------------------------------------------------------------
// COperationTreeItemData
//----------------------------------------------------------------------------
class COperationTreeItemData : public wxTreeItemData
{
public:
  COperationTreeItemData(bool allowLabelChange = true);
  COperationTreeItemData(CFormula* formula, bool allowLabelChange = true);

  void SetAllowLabelChange(bool value) { m_allowLabelChange = value; }
  bool GetAllowLabelChange() { return m_allowLabelChange; }

  CFormula* GetFormula() { return m_formula; }
  void SetFormula(CFormula* value) { m_formula = value; }
  /*
  wxTreeItemId GetParentId() { return m_parentId; }
  void SetParentId(const wxTreeItemId& value) { m_parentId = value; }
  */
  int32_t GetType() {return m_type;};
  void SetType(int32_t value) {m_type = value;}; 
  
  int32_t GetMaxAllowed();

private:
    void Init();

private:
  bool m_canDelete;
  bool m_allowLabelChange;
  CFormula* m_formula;
  
  int32_t m_type;

  //wxTreeItemId m_parentId;

};


//----------------------------------------------------------------------------
// COperationTreeCtrl
//----------------------------------------------------------------------------

class COperationTreeCtrl: public CTreeCtrl
{
public:
  // constructors and destructors
  COperationTreeCtrl( wxWindow *parent, wxWindowID id = -1,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = 0 );

  virtual ~COperationTreeCtrl();
    
  // WDR: method declarations for COperationTreeCtrl

  void Add(const wxTreeItemId& parentId);
  void Add(const wxTreeItemId& parentId, CField* field);
  void Add(const wxTreeItemId& parentId, CFormula* formula);

  bool SelectRecord(CProduct* product);
  bool SelectRecord();

  CFieldsTreeCtrl* GetFieldstreectrl();
  virtual void ShowMenu(wxTreeItemId id, const wxPoint& pt);

  COperationTreeItemData* GetCurrentItemDataValue();
  CFormula* GetCurrentFormula();
  int32_t GetCurrentType();
 
  COperationTreeItemData* GetItemDataValue(const wxTreeItemId& id);
  bool IsRootType(const wxTreeItemId& id);

  CFormula* GetFormula(const wxTreeItemId& id);
  void SetFormula(const wxTreeItemId& id, CFormula* formula);

  wxString GetFormulaName(const wxTreeItemId& id);

  void DeleteFormulaAll(const wxTreeItemId& idParent, wxTreeItemIdValue cookie = 0);
  void DeleteFormula(const wxTreeItemId& id);
  void DeleteCurrentFormula();


  int32_t GetType(const wxTreeItemId& id);
  
  int32_t GetTypeOperation();

  int32_t GetMaxAllowed(const wxTreeItemId& id);

  wxTreeItemId FindItem(int32_t type, const wxString& text = "", bool bCaseSensitive = true, bool bExactMatch = true);
  wxTreeItemId FindItem(const wxTreeItemId& from, int32_t type, const wxString& text = "", bool bCaseSensitive = true, bool bExactMatch = true);

  wxTreeItemId FindParentItem(const wxTreeItemId& idParent, int32_t type, wxTreeItemIdValue cookie = 0);
  wxTreeItemId FindParentItem(int32_t type, const wxTreeItemId& fromId = 0);

  wxTreeItemId FindParentRootTypeItem(const wxTreeItemId& fromId);

  virtual void RemoveAllItems(bool bUnselect = true);


  wxTreeItemId GetXRootId();
  wxTreeItemId GetYRootId();
  wxTreeItemId GetDataRootId();
  wxTreeItemId GetSelectRootId();

  wxWindow* GetOwner() { return m_owner; };
  void SetOwner(wxWindow* value) { m_owner = value; };

  void Insert(COperation* operation);

  void OnFormulaChanged(CFormula* formula);

  void DeInstallEventListeners();
  void ConnectToolTipEvent();


private:
    // WDR: member variable declarations for COperationTreeCtrl
    COperation* m_operation;
    wxWindow* m_owner;
private:

  void Init();

    // WDR: handler declarations for COperationTreeCtrl
  void OnGetToolTip( wxTreeEvent &event );
  void OnEndLabelEdit( wxTreeEvent &event );
  void OnKeyUp( wxKeyEvent &event );
  void OnBeginLabelEdit( wxTreeEvent &event );
  void InitItemsToTree();

private:
    DECLARE_EVENT_TABLE()
};




#endif
