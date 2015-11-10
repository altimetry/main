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
    #pragma implementation "OperationTreeCtrl.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "Product.h" 
using namespace brathl;

#include "BratGui.h"
#include "Validators.h"
#include "Formula.h"
#include "FieldsTreeCtrl.h"
#include "OperationTreeCtrl.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// CSelectRecord
//----------------------------------------------------------------------------

// WDR: event table for CSelectRecord

BEGIN_EVENT_TABLE(CSelectRecord,wxDialog)
    EVT_BUTTON( wxID_OK, CSelectRecord::OnOk )
    EVT_BUTTON( wxID_CANCEL, CSelectRecord::OnCancel )
    EVT_LISTBOX_DCLICK( ID_RECORD_LIST, CSelectRecord::OnRecordListDblClick )
END_EVENT_TABLE()

CSelectRecord::CSelectRecord( wxWindow *parent, CProduct* product, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxDialog( parent, id, title, position, size, style )
{
  // WDR: dialog function RecordDlg for CSelectRecord
  RecordDlg( this, TRUE ); 

  if (product != NULL)
  {
    wxGetApp().GetRecordNames(*GetRecordList(), product);
  }

}

//----------------------------------------
CSelectRecord::~CSelectRecord()
{
}

// WDR: handler implementations for CSelectRecord

//----------------------------------------
void CSelectRecord::OnRecordListDblClick( wxCommandEvent &event )
{
  wxCommandEvent ev(wxEVT_COMMAND_BUTTON_CLICKED, wxID_OK);
  wxPostEvent(this, ev);
}

//----------------------------------------
void CSelectRecord::OnCancel( wxCommandEvent &event )
{
  m_recordName.Empty();    
  event.Skip();
}

//----------------------------------------
void CSelectRecord::OnOk( wxCommandEvent &event )
{
  m_recordName =  GetRecordList()->GetStringSelection();   
  event.Skip();
}
//----------------------------------------


//----------------------------------------------------------------------------
// CFieldTextCtrlDropTarget
//----------------------------------------------------------------------------

wxDragResult CFieldTextCtrlDropTarget::OnDragOver(wxCoord x, wxCoord y, wxDragResult def)
{
  return (def == wxDragMove ? wxDragCopy : def);  
}

//----------------------------------------
wxDragResult CFieldTextCtrlDropTarget::OnData(wxCoord x, wxCoord y, wxDragResult def)
{
  if (!GetData())  
  {
    return wxDragNone;
  }

  CDndFieldDataObject* dndFieldDataObject = dynamic_cast<CDndFieldDataObject*>(GetDataObject());
  CDndField* dndField =  dndFieldDataObject->GetField();
  
  CField* field = dndField->m_field;
  if (field == NULL)
  {
    return wxDragNone;
  }

  if (m_pOwner == NULL)
  {
    return wxDragNone;
  }


  //m_pOwner->WriteText(field->GetFullNameWithRecord().c_str());
  m_pOwner->InsertField(field);


  return wxDragCopy;
}

//----------------------------------------------------------------------------
// CFieldDropTarget
//----------------------------------------------------------------------------

wxDragResult CFieldDropTarget::OnDragOver(wxCoord x, wxCoord y, wxDragResult def)
{
  return (def == wxDragMove ? wxDragCopy : def);  
}

//----------------------------------------
wxDragResult CFieldDropTarget::OnData(wxCoord x, wxCoord y, wxDragResult def)
{
  if (!GetData())  
  {
    return wxDragNone;
  }

  CDndFieldDataObject* dndFieldDataObject = dynamic_cast<CDndFieldDataObject*>(GetDataObject());
  CDndField* dndField =  dndFieldDataObject->GetField();
  
  CField* field = dndField->m_field;

  if (field == NULL)
  {
    return wxDragNone;
  }

  if (m_pOwner == NULL)
  {
    return wxDragNone;
  }


  int32_t flags;
  wxPoint pt(x, y);
  wxTreeItemId parentId =  m_pOwner->HitTest(pt, flags);


  m_pOwner->Add(parentId, field);

  return wxDragCopy;
}

//----------------------------------------------------------------------------
// COperationTreeItemData
//----------------------------------------------------------------------------
COperationTreeItemData::COperationTreeItemData(bool allowLabelChange /*= true*/)
{
  Init();

  m_allowLabelChange = allowLabelChange;
}

//----------------------------------------
COperationTreeItemData::COperationTreeItemData(CFormula* formula, bool allowLabelChange /*= true*/)
{
  Init();

  m_formula = formula;
  m_allowLabelChange = allowLabelChange;
//  m_parentId = parentId;
}

//----------------------------------------
void COperationTreeItemData::Init()
{
  m_canDelete = true;
  m_allowLabelChange = false;
  m_type = -1;
  m_formula = NULL;
}

//----------------------------------------
int32_t COperationTreeItemData::GetMaxAllowed()
{
  int32_t max = 0;
  switch (m_type)
  {
  case CMapTypeField::typeOpAsX: 
    max = 1;
    break;
  case CMapTypeField::typeOpAsY: 
    max = 1;
    break;
  case CMapTypeField::typeOpAsField: 
    max = -1;
    break;
  case CMapTypeField::typeOpAsSelect: 
    max = 1;
    break;
  }

  return max;
}
//----------------------------------------------------------------------------
// COperationTreeCtrl
//----------------------------------------------------------------------------

// WDR: event table for COperationTreeCtrl

BEGIN_EVENT_TABLE(COperationTreeCtrl,CTreeCtrl)
    EVT_TREE_BEGIN_LABEL_EDIT( ID_OPERATIONTREECTRL, COperationTreeCtrl::OnBeginLabelEdit )
    // EVT_TREE_ITEM_MENU is the preferred event for creating context menus
    // on a tree control, because it includes the point of the click or item,
    // meaning that no additional placement calculations are required.
    EVT_TREE_ITEM_MENU(ID_OPERATIONTREECTRL, COperationTreeCtrl::OnItemMenu)
    EVT_CONTEXT_MENU(COperationTreeCtrl::OnContextMenu)
    EVT_KEY_UP( COperationTreeCtrl::OnKeyUp )
    EVT_TREE_END_LABEL_EDIT( ID_OPERATIONTREECTRL, COperationTreeCtrl::OnEndLabelEdit )
END_EVENT_TABLE()
//----------------------------------------

COperationTreeCtrl::COperationTreeCtrl( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    CTreeCtrl( parent, id, position, size, style )
{
  Init();

  InitItemsToTree();
}


//----------------------------------------
COperationTreeCtrl::~COperationTreeCtrl()
{
  CTreeCtrl::RemoveAllItems();

}
//----------------------------------------
void COperationTreeCtrl::Init()
{
  m_operation = NULL;
  m_owner = NULL;
}

//----------------------------------------
void COperationTreeCtrl::RemoveAllItems(bool bUnselect /*= true*/)
{
  CTreeCtrl::RemoveAllItems(bUnselect);

  InitItemsToTree();
}

//----------------------------------------
void COperationTreeCtrl::InitItemsToTree()
{
  //int image = wxGetApp().ShowImages() ? MyTreeCtrl::TreeCtrlIcon_Folder : -1;
  int image = CTreeCtrl::TreeCtrlIcon_Folder;

  /*wxTreeItemId rootId = AddRoot(wxT("Root"),
                                image, image,
                                new MyTreeItemData(wxT("Root item")));
                                */
  wxTreeItemId rootId = AddRoot(wxT("Root"),
                                image, image);


  COperationTreeItemData* itemData =  new COperationTreeItemData(false);
  itemData->SetType(CMapTypeField::typeOpAsX);  

  wxTreeItemId id = AppendItem(rootId, "X", image, image + 1, itemData );
  SetItemBold(id, true);
  SetItemImage(id, CTreeCtrl::TreeCtrlIcon_FolderOpened,
                     wxTreeItemIcon_Expanded);


  itemData =  new COperationTreeItemData(false);
  itemData->SetType(CMapTypeField::typeOpAsY);

  id = AppendItem(rootId, "Y (optional)", image, image + 1, itemData);
  SetItemBold(id, true);
  SetItemImage(id, CTreeCtrl::TreeCtrlIcon_FolderOpened,
                     wxTreeItemIcon_Expanded);

  itemData =  new COperationTreeItemData(false);
  itemData->SetType(CMapTypeField::typeOpAsField);

  id = AppendItem(rootId, "Data ", image, image + 1, itemData);
  SetItemBold(id, true);
  SetItemImage(id, CTreeCtrl::TreeCtrlIcon_FolderOpened,
                     wxTreeItemIcon_Expanded);
  
  itemData =  new COperationTreeItemData(false);
  itemData->SetType(CMapTypeField::typeOpAsSelect);

  id = AppendItem(rootId, "Selection criteria (optional)", CTreeCtrl::TreeCtrlIcon_File, CTreeCtrl::TreeCtrlIcon_File + 1, itemData);
  SetItemBold(id, false);
}

//----------------------------------------
void COperationTreeCtrl::DeInstallEventListeners()
{
  this->Disconnect(wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP);

}
//----------------------------------------

bool COperationTreeCtrl::IsRootType(const wxTreeItemId& id) 
{
  if (id == GetSelectRootId())
  {
    return true;
  }
  
  if (id == GetXRootId())
  {
    return true;
  }

  if (id == GetYRootId())
  {
    return true;
  }

  if (id == GetDataRootId())
  {
    return true;
  }

  return false;
}
//----------------------------------------
wxTreeItemId COperationTreeCtrl::FindParentRootTypeItem(const wxTreeItemId& fromId)
{
  
  wxTreeItemId parentId;
  wxTreeItemId id = fromId;

  if (!id)
  {
    return id;
  }

  if (IsRootType(id))
  {
    return id;
  }

  if (id == GetRootItem())
  {
    return parentId;
  }

  wxString test = GetItemText(id);
  parentId = GetItemParent(id);

  FindParentRootTypeItem(parentId);

  return parentId;
  
}


//----------------------------------------
wxTreeItemId COperationTreeCtrl::FindParentItem(const wxTreeItemId& parentId, int32_t type, wxTreeItemIdValue cookie /*= 0*/)
{
  wxTreeItemId id;

  if ( !cookie )
  {
    id = GetFirstChild(parentId, cookie);
  }
  else
  {
    id = GetNextChild(parentId, cookie);
  }

  if ( !id.IsOk() )
  {
    return id;
  }

  COperationTreeItemData* itemData = id.IsOk() ? dynamic_cast<COperationTreeItemData*>(GetItemData(id))
                                         : NULL;

 
  if (itemData != NULL)
  {
    if (type == itemData->GetType())
    {
      return id;
    }
  }

  if (ItemHasChildren(id))
  {
    id = FindParentItem(id, type);
    if (id.IsOk())
    {
      return id;
    }

  }

  return FindParentItem(parentId, type, cookie);
}

//----------------------------------------
CFieldsTreeCtrl* COperationTreeCtrl::GetFieldstreectrl()
{
  return wxGetApp().GetGuiFrame()->GetFieldstreectrl(); 
}

// WDR: handler implementations for COperationTreeCtrl

//----------------------------------------

void COperationTreeCtrl::OnKeyUp( wxKeyEvent &event )
{

  long keycode = event.GetKeyCode();

  switch ( keycode )
  {
    case WXK_DELETE:
    case WXK_NUMPAD_DELETE:
    {
      if (!event.HasModifiers())
      {
        DeleteCurrentFormula();
      }
      break;
    }

    default:
    {
      break;
    }
  }
  

    
}
//----------------------------------------
void COperationTreeCtrl::OnEndLabelEdit( wxTreeEvent &event )
{

  if (event.IsEditCancelled())
  {
    event.Veto();
    return;
  }

  // Warning : if label has not been changed, Getlabel() returns empty std::string !!!!!
  
  wxTreeItemId id = event.GetItem();
  wxString label = event.GetLabel();

  label = label.Trim(false);
  label = label.Trim(true);

  CTextLightValidator textLightValidator;
  
  if (label.IsEmpty())
  {
//      wxMessageBox("Names of data expressions must not be empty.\n",
//                  "Warning",
//                  wxOK | wxICON_EXCLAMATION);
//
      event.Veto();
      return;
  }

  bool bOk = textLightValidator.IsValid(label);
  if (!bOk)
  {
    wxMessageBox(wxString::Format("Names of data expressions must only contain the following characters:\n"
                              "%s\n",
                                  textLightValidator.GetDigits().c_str()),
                "Warning",
                wxOK | wxICON_EXCLAMATION);

    event.Veto();
    return;
  }

  if (!id)
  {
    event.Veto();
    return;
  }

  CFormula* formula = GetFormula(id);
  if (formula == NULL)
  {
    event.Veto();
    return;
  }
  
  if (m_operation == NULL)
  {
    event.Veto();
    return;
  }


  CFormula* otherFormula = m_operation->GetFormula(label);
  if (otherFormula != NULL)
  {
    // Compare pointer, if same, don't warn
    if (otherFormula != formula)
    {
        wxMessageBox(wxString::Format("Unable to rename.\nAnother data expression is already named '%s'.",
                                      label.c_str()),
                    "Warning",
                    wxOK | wxICON_EXCLAMATION);

        event.Veto();
        return;

    }
  }

  m_operation->RenameFormula(formula, label);
    
}

//----------------------------------------

void COperationTreeCtrl::OnBeginLabelEdit( wxTreeEvent &event )
{

  COperationTreeItemData* itemData = dynamic_cast<COperationTreeItemData*>(GetItemData(event.GetItem()));
  if (itemData == NULL)
  {
    return;
  }

  if (!itemData->GetAllowLabelChange())
  {
    event.Veto();
  }

}


//----------------------------------------
void COperationTreeCtrl::Insert(COperation* operation)
{
 
  DeInstallEventListeners();

  m_operation = operation;

  RemoveAllItems();
  
  if (m_operation == NULL)
  {
    return;
  }

  wxTreeItemId idRootX = GetXRootId();
  wxTreeItemId idRootY = GetYRootId();
  wxTreeItemId idRootData = GetDataRootId();
  wxTreeItemId idRootSelect = GetSelectRootId();

  CMapFormula* formulas = m_operation->GetFormulas();
  CMapFormula::iterator it;

  for (it = formulas->begin() ; it != formulas->end() ; it++)
  {
    CFormula* formula = m_operation->GetFormula(it);
    if (formula == NULL)
    {
      continue;
    }

    switch (formula->GetType())
    {
    case CMapTypeField::typeOpAsX: 
      Add(idRootX, formula);
      break;
    case CMapTypeField::typeOpAsY: 
      Add(idRootY, formula);
      break;
    case CMapTypeField::typeOpAsField: 
      Add(idRootData, formula);
      break;
    }

  }

  Add(idRootSelect, m_operation->GetSelect());


  ConnectToolTipEvent();

}
//----------------------------------------
void COperationTreeCtrl::ConnectToolTipEvent()
{
  this->Connect(wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 (wxTreeEventFunction)&COperationTreeCtrl::OnGetToolTip,
                 NULL, 
                 this);

}
//----------------------------------------
void COperationTreeCtrl::DeleteFormulaAll(const wxTreeItemId& idParent, wxTreeItemIdValue cookie)
{
  wxTreeItemId id;

  if ( !cookie )
  {
    id = GetFirstChild(idParent, cookie);
  }
  else
  {
    id = GetNextChild(idParent, cookie);
  }

  DeleteFormula(id);

  if (ItemHasChildren(id))
  {
    DeleteFormulaAll(id);
  }

  //DeleteFormulaAll(idParent, cookie);
  
}
//----------------------------------------
void COperationTreeCtrl::DeleteFormula(const wxTreeItemId& id)
{
  if (!id)
  {
    return;
  }

  wxString name;

  CFormula* formula = GetFormula(id);
  if (formula != NULL)
  {
    name = formula->GetName();
    
    wxTreeItemId selectRootId = GetSelectRootId();
  
    if (id != selectRootId)
    {
      SetFormula(id, NULL);
    }
    m_operation->DeleteFormula(name);
  }
  
  //Delete(id);

  //m_operation->SetType(GetTypeOperation());

  CDeleteDataExprEvent ev(GetId(), name);
  if (m_owner != NULL)
  {
    wxPostEvent(m_owner, ev);
  }
  
  Refresh();
  
}
//----------------------------------------
void COperationTreeCtrl::DeleteCurrentFormula()
{
  wxTreeItemId id = GetSelection();
  
  DeleteFormula(id);

  wxTreeItemId selectRootId = GetSelectRootId();
  if (id != selectRootId)
  {
    Delete(id);
  }

  m_operation->SetType(GetTypeOperation());

}
//----------------------------------------
bool COperationTreeCtrl::SelectRecord(CProduct* product)
{
  if (m_operation == NULL)  
  {
    return false;
  }
  
  if (product == NULL)  
  {
    return false;
  }

  CSelectRecord recordDlg (this, product, -1, "Select a record...");
  
  wxString recordName;
  int32_t result = wxID_CANCEL;
  do 
  {
    result = recordDlg.ShowModal();

    recordName = recordDlg.GetRecordName();

  }
  while ((result == wxID_OK) && (recordName.IsEmpty()));

  if (result != wxID_OK)
  {
    return false;
  }

  m_operation->SetRecord(recordName);
  return true;
  
}
//----------------------------------------
bool COperationTreeCtrl::SelectRecord()
{
  if (m_operation == NULL)  
  {
    return false;
  }
  
  CProduct* product = GetFieldstreectrl()->GetProduct();
  return SelectRecord(product);

}
//----------------------------------------
void COperationTreeCtrl::Add(const wxTreeItemId& parentId)
{

  if (m_operation == NULL)  
  {
    return;
  }

  if (!parentId)  
  {
    return;
  }

  bool bOk = true;

  wxString operationRecord = m_operation->GetRecord();
  
  if (operationRecord.IsEmpty())
  {
    bOk = SelectRecord();
  }

  if (!bOk)
  {
    return;
  }


  wxTreeItemId theParentId = FindParentRootTypeItem(parentId);
  COperationTreeItemData* itemDataParent = dynamic_cast<COperationTreeItemData*>(GetItemData(theParentId));

  wxTreeItemId selectRootId = GetSelectRootId();
  
  if (itemDataParent != NULL)
  {
    CFormula* formula = m_operation->NewUserFormula(m_operation->GetFormulaNewName(), itemDataParent->GetType(), "", (theParentId != selectRootId));
    Add(theParentId, formula);
  }
  
  return;
}

//----------------------------------------
void COperationTreeCtrl::Add(const wxTreeItemId& parentId, CField* field)
{
  if (field == NULL)  
  {
    return;
  }

  if (m_operation == NULL)  
  {
    return;
  }

  if (!parentId)  
  {
    return;
  }

  wxTreeItemId theParentId = FindParentRootTypeItem(parentId);
  
  //int32_t nbVirtualDims = field->GetVirtualNbDims();
/*
  if (nbVirtualDims > 1)
  {
    wxMessageBox(wxString::Format("Field '%s' is a %d-dimensional field.\n"
                              "It is no allowed as '%s' expression.\n",
                                  field->GetName().c_str(),
                                  nbVirtualDims,
                                  GetItemText(theParentId).c_str()),
                "Warning",
                wxOK | wxICON_HAND);
    return;

  }
 */
  /*
  if (theParentId != GetDataRootId())
  {
    if (field->HasVirtualNbDims() && !field->IsHighResolution())
    {
      wxMessageBox(wxString::Format("Field '%s' is neither a scalar (dimension: '%s') nor a 'high resolution' field.\n"
                                "It is no allowed as '%s' expression.\n",
                                    field->GetName().c_str(),
                                    field->GetDimAsString().c_str(),
                                    GetItemText(theParentId).c_str()),
                  "Warning",
                  wxOK | wxICON_HAND);
      return;

    }
  }
  */
  wxString operationRecord = m_operation->GetRecord();
  wxString fieldRecord = field->GetRecordName().c_str();

  /*
  if (m_operation->HasFormula())
  {
    if (operationRecord.CmpNoCase(fieldRecord) != 0)
    {
      wxMessageBox(wxString::Format("Data expression must have the same record.\n"
                                "The current record of the operation is: '%s'\n"
                                "and you are currently going to add a field from record: '%s' \n",
                                    operationRecord.c_str(),
                                    fieldRecord.c_str()),
                  "Warning",
                  wxOK | wxICON_HAND);
      return;

    }
  }
  else
  {
    m_operation->SetRecord(fieldRecord);
  }
  */

  if (!m_operation->HasFormula())
  {
    m_operation->SetRecord(fieldRecord);
  }

  DeInstallEventListeners();

  COperationTreeItemData* itemDataParent = dynamic_cast<COperationTreeItemData*>(GetItemData(theParentId));

  wxTreeItemId selectRootId = GetSelectRootId();
  
  if (itemDataParent != NULL)
  {
    CFormula* formula = m_operation->NewUserFormula(field, itemDataParent->GetType(), (theParentId != selectRootId), GetFieldstreectrl()->GetProduct());
    Add(theParentId, formula);
  }

  ConnectToolTipEvent();

  return;
}

//----------------------------------------
void COperationTreeCtrl::Add(const wxTreeItemId& parentId, CFormula* formula)
{

  wxTreeItemId theParentId = parentId;

  if (formula == NULL)  
  {
    return;
  }

  if (m_operation == NULL)  
  {
    return;
  }

  if (!parentId)  
  {
    return;
  }

  theParentId = FindParentRootTypeItem(parentId);
  
  if (!theParentId)  
  {
    return;
  }

  DeInstallEventListeners();

  int32_t nChildren = GetChildrenCount(theParentId, false);
  int32_t max = GetMaxAllowed(theParentId);

  wxTreeItemId selectRootId = GetSelectRootId();
  wxTreeItemId xRootId = GetXRootId();
  wxTreeItemId yRootId = GetYRootId();
  
  if (max > 0)
  {
    if (nChildren >= max)
    {
      if (theParentId != selectRootId)
      {
        wxTreeItemId lastChildId = GetLastChild(theParentId);
        //DeleteFormulaAll(theParentId);
        DeleteFormula(lastChildId);
        //DeleteChildren(theParentId);
        Delete(lastChildId);
      }
     
    }
  }
  
  if (theParentId == selectRootId)
  {
    m_operation->SetSelect(formula);
    SetFormula(selectRootId, m_operation->GetSelect());    
    SelectItem(selectRootId);

  }
  else
  {

    int32_t image = TreeCtrlIcon_File;
    int32_t imageSel = image + 1;

    wxString str;
  //  str.Printf(wxT("%s (%s)"), field->GetName().c_str(), field->GetUnit().c_str());
    str.Printf(wxT("%s"), formula->GetName().c_str());

    wxTreeItemId idInserted = AppendItem(theParentId, str, image, imageSel,
                                   new COperationTreeItemData(formula));

    Expand(theParentId);

    m_operation->SetType(GetTypeOperation());
    
    SelectItem(idInserted);
  }

  if ((theParentId == xRootId) || (theParentId == yRootId))
  {
    formula->SetStepToDefaultAsNecessary();
  }
  

  Refresh();


  CNewDataExprEvent ev(GetId(), formula->GetName());
  if (m_owner != NULL)
  {
    wxPostEvent(m_owner, ev);
  }
  
  ConnectToolTipEvent();

  return;
}
//----------------------------------------
void COperationTreeCtrl::OnFormulaChanged(CFormula* formula)
{
  if (m_operation == NULL)
  {
    return;
  }

  if (formula == NULL)
  {
    return;
  }

  if (m_operation->IsSelect(formula))
  {
    wxString value = formula->GetDescription(true).Trim();
    SetItemBold( GetSelectRootId(), !(value.IsEmpty()) );
  }

}
//----------------------------------------
/*
void COperationTreeCtrl::Add(const wxTreeItemId& parentId, CFormula* formula)
{
  wxTreeItemId theParentId = parentId;

  if (formula == NULL)  
  {
    return;
  }

  if (m_operation == NULL)  
  {
    return;
  }

  if (!parentId)  
  {
    return;
  }

  theParentId = FindParentRootTypeItem(parentId);

  wxTreeItemId selectRootId = GetSelectRootId();

  if (theParentId == selectRootId)
  {
    m_operation->SetSelect(formula);
  }

  int32_t image = TreeCtrlIcon_File;
  int32_t imageSel = image + 1;

  wxString str;
//  str.Printf(wxT("%s (%s)"), field->GetName().c_str(), field->GetUnit().c_str());
  str.Printf(wxT("%s"), formula->GetName().c_str());

  wxTreeItemId idInserted = AppendItem(theParentId, str, image, imageSel,
                                 new COperationTreeItemData(formula));

  Expand(theParentId);

  SelectItem(idInserted);
  
  Refresh();
  
  return;
}
*/

//----------------------------------------
int32_t COperationTreeCtrl::GetTypeOperation()
{
  wxTreeItemId idRootX = GetXRootId();
  wxTreeItemId idRootY = GetYRootId();

  bool hasX = ((!idRootX) ?  false : ItemHasChildren(idRootX));
  bool hasY = ((!idRootY) ?  false : ItemHasChildren(idRootY));
  
  int32_t typeOp = -1;

  if ((hasX) && (hasY))
  {
    typeOp =  CMapTypeOp::typeOpZFXY;
  }
  else
  {
    typeOp =  CMapTypeOp::typeOpYFX;
  }
  
  return typeOp;
}
//----------------------------------------
COperationTreeItemData* COperationTreeCtrl::GetCurrentItemDataValue()
{
  wxTreeItemId id = GetSelection();
  return GetItemDataValue(id);
}
//----------------------------------------
int32_t COperationTreeCtrl::GetCurrentType()
{
  wxTreeItemId id = GetSelection();
  return GetType(id);
}

//----------------------------------------
CFormula* COperationTreeCtrl::GetCurrentFormula()
{
  wxTreeItemId id = GetSelection();
  return GetFormula(id);
}

//----------------------------------------
COperationTreeItemData* COperationTreeCtrl::GetItemDataValue(const wxTreeItemId& id)
{
  COperationTreeItemData* itemData = NULL;
  
  if (id.IsOk())
  {
    itemData = dynamic_cast<COperationTreeItemData*>(GetItemData(id));
  }

  return itemData;
}

//----------------------------------------
CFormula* COperationTreeCtrl::GetFormula(const wxTreeItemId& id)
{
  CFormula* formula = NULL;
  COperationTreeItemData* itemData = GetItemDataValue(id);

  if (itemData != NULL)
  {
    formula = itemData->GetFormula();
  }

  return formula;

}

//----------------------------------------
void COperationTreeCtrl::SetFormula(const wxTreeItemId& id, CFormula* formula)
{
  COperationTreeItemData* itemData = GetItemDataValue(id);

  if (itemData != NULL)
  {
    itemData->SetFormula(formula);
  }

}
//----------------------------------------
wxString COperationTreeCtrl::GetFormulaName(const wxTreeItemId& id)
{
  wxString name;

  CFormula* formula = GetFormula(id);
  if (formula != NULL)
  {
    name = formula->GetName();
  }

  return name;
}

//----------------------------------------
int32_t COperationTreeCtrl::GetType(const wxTreeItemId& id)
{
  int32_t type = -1;
  COperationTreeItemData* itemData = GetItemDataValue(id);

  if (itemData != NULL)
  {
    type = itemData->GetType();
  }

  return type;

}
//----------------------------------------
int32_t COperationTreeCtrl::GetMaxAllowed(const wxTreeItemId& id)
{
  int32_t max = 0;
  
  COperationTreeItemData* itemData = GetItemDataValue(id);

  if (itemData != NULL)
  {
    max = itemData->GetMaxAllowed();
  }

  return max;

}

//----------------------------------------
wxTreeItemId COperationTreeCtrl::GetXRootId()
{
  return FindItem(CMapTypeField::typeOpAsX);
}
//----------------------------------------
wxTreeItemId COperationTreeCtrl::GetYRootId()
{
  return FindItem(CMapTypeField::typeOpAsY);
}
//----------------------------------------
wxTreeItemId COperationTreeCtrl::GetDataRootId()
{
  return FindItem(CMapTypeField::typeOpAsField);
}
//----------------------------------------
wxTreeItemId COperationTreeCtrl::GetSelectRootId()
{
  return FindItem(CMapTypeField::typeOpAsSelect);
}

//----------------------------------------
wxTreeItemId COperationTreeCtrl::FindItem(int32_t type, const wxString& text /*= ""*/, bool bCaseSensitive /*= true*/, bool bExactMatch /*= true*/)
{
  return FindItem(GetRootItem(), type, text, bCaseSensitive, bExactMatch);
}
//----------------------------------------
wxTreeItemId COperationTreeCtrl::FindItem(const wxTreeItemId& from, int32_t type, const wxString& text /*= ""*/, bool bCaseSensitive /*= true*/, bool bExactMatch /*= true*/)
{
  CFormula* formula = NULL;
  int32_t typeFound = -1;

  wxTreeItemId item = from;
  wxTreeItemId child;
  wxTreeItemIdValue cookie;

  wxString findText(text);
  wxString itemText;

  bool bFound = false;

  if(!bCaseSensitive) 
  {
    findText.MakeLower();
  }

  while(item.IsOk())
  {
    if (item != GetRootItem())
    {
      itemText = GetItemText(item);
      formula = GetFormula(item);
      typeFound = GetType(item);

      if(!bCaseSensitive)
      {
        itemText.MakeLower();
      }

      if (type == typeFound)
      {
        if (type == CMapTypeField::typeOpAsSelect)
        {
          bFound = true;
        } 
        else if (findText.empty())
        {
          bFound = (formula == NULL);
        }
        else
        {
          bFound = bExactMatch ? (itemText == findText) : itemText.Contains(findText);
          bFound &= (formula != NULL);
        }
      }
    }
  
    if(bFound) 
    {
      return item;
    }

    child = GetFirstChild(item, cookie);

    if(child.IsOk())
    {
      child = FindItem(child, type, text, bCaseSensitive, bExactMatch);
    }
    if(child.IsOk())
    {
      return child;
    }

    // Bug on WIN32 : GetNextSibling crashes if item is Root and Root is Hidden (virtual)
    if (item == GetRootItem())
    {
      item.Unset();
    }
    else
    {
      item = GetNextSibling(item);
    }
  } // while(item.IsOk())

  return item;
}

//----------------------------------------
void COperationTreeCtrl::OnGetToolTip( wxTreeEvent &event )
{

  wxTreeItemId itemId = event.GetItem();

  COperationTreeItemData* itemData = itemId.IsOk() ? dynamic_cast<COperationTreeItemData*>(GetItemData(itemId))
                                       : NULL;

  if (itemData == NULL)
  {
    event.Veto();
    return;
  }

  wxString description;

  CFormula* formula = itemData->GetFormula();
  if (formula == NULL)
  {
    event.Veto();
    return;
  }

  if (!formula->GetTitle().IsEmpty())
  {
    description.Append(formula->GetTitle().c_str());
    description.Append("\n\n");
  }

  description.Append("Value:\n");
  description.Append(formula->GetDescription().c_str());
  description.Append("\n");

  description.Append("\nUnit: ");
  description.Append(formula->GetUnitAsText());
  description.Append("\n");

  if (!formula->GetComment().IsEmpty())
  {
    description.Append("\n");
    description.Append(formula->GetComment().c_str());
  }
  
  description.Append("\n");

  //SetToolTip(NULL); // This forces to recreate tooltip and to set its length to the max line length of text
  event.SetToolTip(description);

    
}


#if defined(_MSC_VER)
#pragma warning( disable : 4996 )
#endif

//----------------------------------------
void COperationTreeCtrl::ShowMenu(wxTreeItemId id, const wxPoint& pt)
{
  COperationTreeItemData* itemData = id.IsOk() ? dynamic_cast<COperationTreeItemData*>(GetItemData(id))
                                       : NULL;
  if (itemData == NULL)
  {
    return;
  }

  wxTreeItemId idFieldTreeCtrl = GetFieldstreectrl()->GetSelection();
  CFieldsTreeItemData* itemDataField = idFieldTreeCtrl.IsOk() ? dynamic_cast<CFieldsTreeItemData*>(GetItemData(idFieldTreeCtrl))
                                       : NULL;
  CField* fieldF = NULL;
  if (itemDataField != NULL)
  {
    fieldF = (!GetFieldstreectrl()->ItemHasChildren(idFieldTreeCtrl)) ? itemDataField->GetField() : NULL;
  }

  wxString name;
  wxString title;
  CFormula* formula = itemData->GetFormula();
  if (formula == NULL)
  {
    name = GetItemText(id);
  }
  else
  {
    name = formula->GetName().c_str();
  }


  title.Printf(wxT("Menu for %s"), name.c_str());

  wxMenu* menu = wxGetApp().GetMenuOperationTreeCtrl();
  if (menu == NULL)
  {
    return;
  }


  menu->SetTitle(title);
  wxMenuItem* menuItem = menu->FindItem(ID_INSERTFIELDMENU);
  if (menuItem != NULL)
  {
    if (fieldF != NULL)
    {
      wxString text;
      if (formula == NULL)
      {
        
        text.Printf(INSERT_FIELD_MENU_LABEL, fieldF->GetName().c_str());
        menuItem->SetText(text);
        menuItem->Enable(true);
      }
      else
      {
        text.Printf(INSERT_FIELD_MENU_LABEL2, fieldF->GetName().c_str());
        menuItem->SetText(text);
        menuItem->Enable(true);
      }
    }
    else
    {
      menuItem->SetText("Insert f&ield");
      menuItem->Enable(false);
    }
  }

  menuItem = menu->FindItem(ID_DELETEEXPRMENU);
  if (menuItem != NULL)
  {
    if (formula != NULL)
    {
      wxString text;      
      text.Printf(DELETE_EXPR_MENU_LABEL, GetItemText(id).c_str());
      menuItem->SetText(text);
      menuItem->Enable(true);
    }
    else
    {
      menuItem->SetText("&Delete expression");
      menuItem->Enable(false);
    }
  }

  menuItem = menu->FindItem(ID_RENAMEEXPRMENU);
  if (menuItem != NULL)
  {
    menuItem->SetText("&Rename expression");
    menuItem->Enable(false);

    if (formula != NULL)
    {
      if (!COperation::IsSelect(formula->GetName()))
      {
        wxString text;      
        text.Printf(RENAME_EXPR_MENU_LABEL, GetItemText(id).c_str());
        menuItem->SetText(text);
        menuItem->Enable(true);
      }
    }
  }

  menuItem = menu->FindItem(ID_SAVEASFORMULAMENU);
  if (menuItem != NULL)
  {
    bool enable = false;

    if (formula != NULL)
    {
      if (!formula->GetDescription().IsEmpty())
      {
        enable = true;
      }
    }
    menuItem->Enable(enable);
  }

  menuItem = menu->FindItem(ID_INSERTFCTMENU);
  if (menuItem != NULL)
  {
    menuItem->Enable((formula != NULL));
  }

  menuItem = menu->FindItem(ID_INSERTFORMULAMENU);
  if (menuItem != NULL)
  {
    menuItem->Enable((formula != NULL));
  }

  PopupMenu(menu, pt);

/*
#if wxUSE_MENUS
    wxMenu menu(title);
    menu.Append(-1, wxT("Insert empty expression..."));
    menu.Append(-1, wxT("Insert selected field..."));
    menu.Append(-1, wxT("Insert function..."));
    menu.Append(-1, wxT("Insert formula..."));
    menu.AppendSeparator();
    menu.Append(-1, wxT("Save as formula..."));
    menu.AppendSeparator();
    menu.Append(-1, wxT("Delete expression..."));
    menu.AppendSeparator();
    menu.Append(-1, wxT("&Set resolution/filter..."));

    PopupMenu(&menu, pt);
#endif // wxUSE_MENUS
    */
}

#if defined(_MSC_VER)
#pragma warning( default : 4996 )
#endif

