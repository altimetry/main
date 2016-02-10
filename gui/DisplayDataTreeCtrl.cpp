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

#include "new-gui/brat/stdafx.h"

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/tokenzr.h"

#include "DisplayDataTreeCtrl.h"
#include "new-gui/brat/Display/MapTypeDisp.h"

#include "PlotData/MapColor.h"
#include "BratGui.h"




const wxString ADD_DISP_TO_SEL_MENU_LABEL = "Add '%s' to selected";
const wxString ADD_EXPAND_CHILDREN_MENU_LABEL = "Expand all '%s' children";


//----------------------------------------------------------------------------
// CDndDisplayDataObject
//----------------------------------------------------------------------------

CDndDisplayDataObject::CDndDisplayDataObject(CDndDisplayData* dndDisplayData)
{
  if (dndDisplayData != nullptr)
  {
    m_dndDisplayData = new CDndDisplayData(*dndDisplayData);
  }
  else
  {
    m_dndDisplayData = nullptr;
  }

  wxDataFormat dataDisplayFormat;
  dataDisplayFormat.SetId(displayDataFormatId);
  SetFormat(dataDisplayFormat);
}
//----------------------------------------

size_t CDndDisplayDataObject::GetDataSize() const
{
  size_t ret = 0;
  //size_t count = 0;

  if (m_dndDisplayData == nullptr)
  {
    return 0;
  }
  ret = sizeof(long);

  return ret;
}
//----------------------------------------

bool CDndDisplayDataObject::GetDataHere(void* buf) const
{
  if (m_dndDisplayData == nullptr)
  {
    return false;
  }
/*
  if (m_dndDisplayData->m_displayData == nullptr)
  {
    return false;
  }
  */
  if (m_dndDisplayData->m_data.size() <= 0)
  {
    return false;
  }

  long ptr = (long)(&(m_dndDisplayData->m_data));
  memset(buf, 0, sizeof(long));
  memcpy(buf, &ptr, sizeof(long));

  return true;
}
//----------------------------------------

bool CDndDisplayDataObject::SetData(size_t len, const void* buf)
{
    UNUSED(len);

  if (m_dndDisplayData == nullptr)
  {
    m_dndDisplayData = new CDndDisplayData();
  }

  long ptr = 0;
  memcpy(&ptr, buf, sizeof(long));
  /*
  CDisplayData* displayData = (CDisplayData*)(ptr);
  m_dndDisplayData->m_displayData = displayData;
  */
  CObArray* data = (CObArray*)(ptr);

  m_dndDisplayData->Set(*data);

  return true;
}




// WDR: class implementations
//----------------------------------------------------------------------------
// CDisplayDataTreeItemData
//----------------------------------------------------------------------------
CDisplayDataTreeItemData::CDisplayDataTreeItemData(bool allowLabelChange /*= true*/)
{
    UNUSED(allowLabelChange);

    Init();
}

//----------------------------------------
CDisplayDataTreeItemData::CDisplayDataTreeItemData(CDisplayData* displayData, bool allowLabelChange /*= true*/)
{
  Init();

  SetDisplayData(displayData);

  m_allowLabelChange = allowLabelChange;


//  m_parentId = parentId;
}

//----------------------------------------
void CDisplayDataTreeItemData::Init()
{
  m_allowLabelChange = false;
  m_displayData = NULL;
}
//----------------------------------------
void CDisplayDataTreeItemData::SetDisplayData(CDisplayData* value)
{
  m_displayData = value;
  SetSplittedFieldDescr();
}

//----------------------------------------
void CDisplayDataTreeItemData::SetSplittedFieldDescr()
{
  CDisplayData* data = GetDisplayData();
  if (data == NULL)
  {
    return;
  }
  const CField* field = data->GetField();
  if (field == NULL)
  {
    return;
  }

//  m_splittedFieldDescr.Append(field->GetName().c_str());
//  m_splittedFieldDescr.Append(":\n");

  wxStringTokenizer tkz;

  tkz.SetString(field->GetDescription(), wxT("\t\r\n"));

  while ( tkz.HasMoreTokens() )
  {
    m_splittedFieldDescr.Append(tkz.GetNextToken());

    if (m_splittedFieldDescr.Last() != '.')
    {
      m_splittedFieldDescr.Append('.');
    }
  }

  if (m_splittedFieldDescr.IsEmpty())
  {
    m_splittedFieldDescr = "No Description for this field.";
  }

  tkz.SetString(m_splittedFieldDescr, wxT("."), wxTOKEN_RET_DELIMS);

  m_splittedFieldDescr.Empty();

  while ( tkz.HasMoreTokens() )
  {
    wxString str = CTools::StringTrim((const char *)tkz.GetNextToken().c_str()).c_str();

    m_splittedFieldDescr.Append(str);

    if (m_splittedFieldDescr.Last() != '\n')
    {
      m_splittedFieldDescr.Append('\n');
    }
  }

}

//----------------------------------------------------------------------------
// CDisplayDataTreeCtrl
//----------------------------------------------------------------------------

// WDR: event table for CDisplayDataTreeCtrl

BEGIN_EVENT_TABLE(CDisplayDataTreeCtrl,CTreeCtrl)
//    EVT_TREE_ITEM_GETTOOLTIP( ID_DISPAVAILTREECTRL, CDisplayDataTreeCtrl::OnGetToolTip )
    EVT_TREE_BEGIN_DRAG( ID_DISPAVAILTREECTRL, CDisplayDataTreeCtrl::OnBeginDrag )
    EVT_TREE_ITEM_MENU(ID_DISPAVAILTREECTRL, CDisplayDataTreeCtrl::OnItemMenu)
    EVT_CONTEXT_MENU(CDisplayDataTreeCtrl::OnContextMenu)
END_EVENT_TABLE()

//----------------------------------------
CDisplayDataTreeCtrl::CDisplayDataTreeCtrl( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    CTreeCtrl( parent, id, position, size, style )
{
  Init();
}

//----------------------------------------
CDisplayDataTreeCtrl::~CDisplayDataTreeCtrl()
{
}

//----------------------------------------
void CDisplayDataTreeCtrl::Init()
{
  m_data = NULL;

}
//----------------------------------------
void CDisplayDataTreeCtrl::InitItemsToTree()
{
  //int image = wxGetApp().ShowImages() ? MyTreeCtrl::TreeCtrlIcon_Folder : -1;
  int image = CTreeCtrl::TreeCtrlIcon_Folder;

  /*wxTreeItemId rootId = AddRoot(wxT("Root"),
                                image, image,
                                new MyTreeItemData(wxT("Root item")));
                                */
  wxTreeItemId rootId = AddRoot(wxT("Root"),
                                image, image);


  /*
  CDisplayDataTreeItemData* itemData =  new CDisplayDataTreeItemData(false);
  itemData->SetType(CMapTypeField::typeOpAsX);

  wxTreeItemId id = AppendItem(rootId, "X", image, image + 1, itemData );
  SetItemBold(id, true);
  SetItemImage(id, TreeCtrlIcon_FolderOpened,
                     wxTreeItemIcon_Expanded);


  itemData =  new CDisplayDataTreeItemData(false);
  itemData->SetType(CMapTypeField::typeOpAsY);
  id = AppendItem(rootId, "Y (optional)", image, image + 1, itemData);
  SetItemBold(id, true);
  SetItemImage(id, TreeCtrlIcon_FolderOpened,
                     wxTreeItemIcon_Expanded);

  itemData =  new CDisplayDataTreeItemData(false);
  itemData->SetType(CMapTypeField::typeOpAsField);

  id = AppendItem(rootId, "Data ", image, image + 1, itemData);
  SetItemBold(id, true);
  SetItemImage(id, TreeCtrlIcon_FolderOpened,
                     wxTreeItemIcon_Expanded);

  itemData =  new CDisplayDataTreeItemData(false);
  itemData->SetType(-1);

  id = AppendItem(rootId, "Selection criteria (optional)", image, image + 1, itemData);
  SetItemBold(id, true);
  SetItemImage(id, TreeCtrlIcon_FolderOpened,
                     wxTreeItemIcon_Expanded);

  */

}
//----------------------------------------
void CDisplayDataTreeCtrl::CollapseItemsRecurse()
{
  CTreeCtrl::CollapseItemsRecurse(GetRootItem());
  ExpandRootData();

}
//----------------------------------------
void CDisplayDataTreeCtrl::DeInstallEventListeners()
{
  this->Disconnect(wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP);

}

//----------------------------------------
void CDisplayDataTreeCtrl::InsertData(CMapDisplayData* data)
{
  this->Disconnect(wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP);

  m_data = data;

  RemoveAllItems();

  if (m_data == NULL)
  {
    return;
  }

  CObMap::iterator it;

  for (it = m_data->begin(); it != m_data->end(); it++)
  {
    CDisplayData* displayData = dynamic_cast<CDisplayData*>(it->second);
    InsertField(displayData, data);
  }


  ExpandRootData();

  this->Connect(wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 (wxTreeEventFunction)&CDisplayDataTreeCtrl::OnGetToolTip,
                 NULL,
                 this);

  if (m_allItemsExpanded)
  {
    ExpandItemsRecurse();
  }

  wxTreeItemIdValue cookie = 0;


  wxTreeItemId rootId = GetRootItem();

  if (!rootId)
  {
    return;
  }

  wxTreeItemId id = GetFirstChild(rootId, cookie);
  if (id.IsOk())
  {
    EnsureVisible(id);
  }


}
//----------------------------------------
void CDisplayDataTreeCtrl::ExpandRootData()
{
  wxTreeItemId rootId = GetRootItem();

  if (!rootId)
  {
    return;
  }

  wxTreeItemId child;
  wxTreeItemIdValue cookie = 0;

  do
  {
    if ( !cookie )
    {
      child = GetFirstChild(rootId, cookie);
    }
    else
    {
      child = GetNextChild(rootId, cookie);
    }
    if (child.IsOk())
    {
      Expand(child);
    }
  }
  while (child.IsOk());


}
//----------------------------------------
/*
wxTreeItemId CDisplayDataTreeCtrl::FindParentItem(int32_t type)
{
  wxTreeItemId fromId;
  return FindParentItem(type, fromId);

}
*/
//----------------------------------------
int32_t CDisplayDataTreeCtrl::CountData()
{
  if (m_data == NULL)
  {
    return -1;
  }

  return m_data->size();
}

//----------------------------------------
wxTreeItemId CDisplayDataTreeCtrl::FindParentItem(int32_t type, const wxTreeItemId& fromId)
{

  wxTreeItemId parentId;
  wxTreeItemId rootId = fromId;

  if (type == -1)
  {
    return parentId;
  }

  if (!rootId)
  {
    rootId = GetRootItem();
  }

  if (rootId.IsOk())
  {
    parentId = FindParentItem(rootId, type);
  }

  return parentId;

}

//----------------------------------------
wxTreeItemId CDisplayDataTreeCtrl::FindParentItem(const wxTreeItemId& parentId, int32_t type, wxTreeItemIdValue cookie /*= 0*/)
{
  wxTreeItemId id;

  if (!parentId)
  {
    return id;
  }

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

  CDisplayDataTreeItemData* itemData = id.IsOk() ? dynamic_cast<CDisplayDataTreeItemData*>(GetItemData(id))
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
//wxTreeItemId CDisplayDataTreeCtrl::FindParentItem(const wxTreeItemId& parentId, const wxString& name, wxTreeItemIdValue cookie /*= 0*/)
/*
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

  wxString itemText = GetItemText(id);

  if (name.CmpNoCase(itemText) == 0)
  {
    return id;
  }

  if (ItemHasChildren(id))
  {
    id = FindParentItem(id, name);
    if (id.IsOk())
    {
      return id;
    }

  }

  return FindParentItem(parentId, name, cookie);
}
*/
//----------------------------------------
void CDisplayDataTreeCtrl::InsertField(CDisplayData* displayData, CMapDisplayData* data)
{

  int image = CTreeCtrl::TreeCtrlIcon_Folder;

  if (data == NULL)
  {
    return;
  }
  if (displayData == NULL)
  {
    return;
  }

  COperation* operation = displayData->GetOperation();

  if (operation == NULL)
  {
    return;
  }


  if (operation == NULL)
  {
    return;
  }

  int32_t type = displayData->GetType();



  wxTreeItemId rootId = GetRootItem();

  if (!rootId)
  {
    rootId = AddRoot("Root", image, image);
  }

  wxTreeItemId idType = FindParentItem(type, rootId);

  CDisplayDataTreeItemData* itemData = NULL;

  if (!idType)
  {
    itemData =  new CDisplayDataTreeItemData(false);
    itemData->SetType(type);

    idType = AppendItem(rootId, CMapTypeDisp::GetInstance().IdToName(type).c_str(), image, image + 1, itemData);

    SetItemBold(idType, true);
    SetItemImage(idType, TreeCtrlIcon_FolderOpened,
                       wxTreeItemIcon_Expanded);
  }


  wxTreeItemId idOp = CTreeCtrl::FindParentItem(idType, operation->GetName().c_str());

  itemData = NULL;

  if (!idOp)
  {
    itemData =  new CDisplayDataTreeItemData(false);
    itemData->SetType(type);

    idOp = AppendItem(idType, operation->GetName().c_str(), image, image + 1, itemData);

    SetItemImage(idOp, TreeCtrlIcon_FolderOpened,
                       wxTreeItemIcon_Expanded);
    SetItemBold(idOp, true);
    SetItemTextColour(idOp, CMapColor::GetInstance().NameToColour("NAVY"));

  }

  itemData =  new CDisplayDataTreeItemData(false);
  itemData->SetType(type);
  itemData->SetDisplayData(displayData);

  wxString fieldLabel;
  wxString dimLabel;

  const uint32_t nbDims = 3;
  CField* dims[nbDims];
  dims[0] = NULL;
  dims[1] = NULL;
  dims[2] = NULL;

  CField *field = NULL;
  /*
  CField *X = NULL;
  CField *Y = NULL;
  CField *Z = NULL;
*/
  if (displayData != NULL)
  {
    field = displayData->GetField();
    if (field != NULL)
    {
      fieldLabel.Printf(wxT("%s (%s)"), field->GetName().c_str(), field->GetUnit().c_str());
    }
/*
    X = displayData->GetX();
    Y = displayData->GetY();
    Z = displayData->GetZ();
*/
    dims[0] = displayData->GetX();
    dims[1] = displayData->GetY();
    dims[2] = displayData->GetZ();

  }


  image = TreeCtrlIcon_File;

  wxTreeItemId idData = AppendItem(idOp, fieldLabel, image, image + 1, itemData);
  SetItemTextColour(idData, *wxBLUE);


  itemData =  new CDisplayDataTreeItemData(false);
  itemData->SetType(type);


  for (uint32_t i = 0 ; i < nbDims ; i++)
  {
    if (dims[i] == NULL)
    {
      continue;
    }
    wxString unit = dims[i]->GetUnit().c_str();

    if (unit.IsEmpty())
    {
      continue;
    }

    if ( ! dimLabel.IsEmpty() )
    {
      dimLabel.Append(" / ");
    }


    dimLabel.Append(dims[i]->GetName().c_str());
    dimLabel.Append(" unit: ");
    dimLabel.Append(unit);

  }

  /*
  if ( (X != NULL) && (Y != NULL) )
  {
    if ( (!X->GetUnit().empty()) && (!Y->GetUnit().empty()) )
    {
      dimLabel.Printf(wxT("%s unit: %s / %s unit: %s"), X->GetName().c_str(), X->GetUnit().c_str(), Y->GetName().c_str(), Y->GetUnit().c_str());

    }
    else if (!X->GetUnit().empty())
    {
      dimLabel.Printf(wxT("%s unit: %s"), X->GetName().c_str(), X->GetUnit().c_str());
    }
    else if (!Y->GetUnit().empty())
    {
      dimLabel.Printf(wxT("%s unit: %s"), Y->GetName().c_str(), Y->GetUnit().c_str());
    }
  }
  else if ( X != NULL)
  {
    if (!X->GetUnit().empty())
    {
      dimLabel.Printf(wxT("%s unit: %s"), X->GetName().c_str(), X->GetUnit().c_str());
    }
  }
  else if ( Y != NULL)
  {
    if (!Y->GetUnit().empty())
    {
      dimLabel.Printf(wxT("%s unit: %s"), Y->GetName().c_str(), Y->GetUnit().c_str());
    }
  }
*/
  if (dimLabel.IsEmpty())
  {
    dimLabel = "dimension informations not available";
  }

  wxTreeItemId idSubData = AppendItem(idData, dimLabel, -1, -1, itemData);
  //SetItemTextColour(idSubData, *wxLIGHT_GREY);





  /*

  wxListItem itemRow;
  itemRow.SetText(operation->GetName());
  itemRow.SetMask(wxLIST_MASK_TEXT);
  itemRow.SetImage(-1);
  itemRow.SetData(displayData);
  itemRow.SetId(index);

  long tmp = InsertItem(itemRow);


  //long tmp = InsertItem(index, field->GetName().c_str(),  DICTCOL_NAME);

  SetItem(index, DICTDISPCOL_FILENAME, operation->GetOutputName());
  SetItem(index, DICTDISPCOL_FIELDNAME, displayData->GetField()->GetName().c_str());
  SetItem(index, DICTDISPCOL_TITLE, displayData->GetField()->GetDescription().c_str());
  SetItem(index, DICTDISPCOL_UNIT, displayData->GetField()->GetUnit().c_str());
*/
}
//----------------------------------------
CDisplayDataTreeItemData* CDisplayDataTreeCtrl::GetCurrentItemDataValue()
{
  wxTreeItemId id = GetSelection();
  return GetItemDataValue(id);
}
//----------------------------------------
CDisplayData* CDisplayDataTreeCtrl::GetCurrentDisplayData()
{
  wxTreeItemId id = GetSelection();
  return GetDisplayData(id);

}
//----------------------------------------
CDisplayDataTreeItemData* CDisplayDataTreeCtrl::GetItemDataValue(const wxTreeItemId& id)
{
  CDisplayDataTreeItemData* itemData = NULL;

  if (id.IsOk())
  {
    itemData = dynamic_cast<CDisplayDataTreeItemData*>(GetItemData(id));
  }

  return itemData;
}

//----------------------------------------
CDisplayData* CDisplayDataTreeCtrl::GetDisplayData(const wxTreeItemId& id)
{
  CDisplayData* displayData = NULL;
  CDisplayDataTreeItemData* itemData = GetItemDataValue(id);

  if (itemData != NULL)
  {
    displayData = itemData->GetDisplayData();
  }

  return displayData;

}
//----------------------------------------
void CDisplayDataTreeCtrl::GetAllDisplayData(CObArray& data, const wxTreeItemId& idSelected)
{
  if (!idSelected)
  {
    return;
  }

  wxTreeItemId idRoot = GetRootItem();

  if (!idRoot)
  {
    return;
  }

  wxTreeItemId idParent;
  wxTreeItemId id;

  CDisplayData* displayData = NULL;

  CDisplayDataTreeItemData* itemData = dynamic_cast<CDisplayDataTreeItemData*>(GetItemData(idSelected));
  if (itemData != NULL)
  {
    displayData = itemData->GetDisplayData();
  }
  //---------------------------------------------------
  if ((displayData == NULL) && (ItemHasChildren(idSelected)))
  {
    GetItemsDataRecursively(data, idSelected);
  }
  //---------------------------------------------------
  else if ((displayData == NULL) && (!ItemHasChildren(idSelected)))
  {
    id = idSelected;
    do
    {
      idParent = GetItemParent(id);
      if (idParent.IsOk())
      {
        itemData = dynamic_cast<CDisplayDataTreeItemData*>(GetItemData(idParent));
        if (itemData != NULL)
        {
          displayData = itemData->GetDisplayData();
          if (displayData != NULL)
          {
            data.Insert(displayData);
          }
        }
        id = idParent;
      }

    }
    while ((displayData == NULL) && (idParent != idRoot) && (idParent.IsOk()));

  }
  //---------------------------------------------------
  else if (displayData != NULL)
  {
    data.Insert(displayData);
  }


}
//----------------------------------------

// WDR: handler implementations for CDisplayDataTreeCtrl

//----------------------------------------
void CDisplayDataTreeCtrl::OnBeginDrag( wxTreeEvent &event )
{
  wxTreeItemId idRoot = GetRootItem();

  if (!idRoot)
  {
    event.Veto();
    return;
  }

  CObArray data(false);

  wxTreeItemId idSelected = event.GetItem();

  GetAllDisplayData(data, idSelected);
/*
  wxTreeItemId idParent;
  wxTreeItemId id;
  CDisplayData* displayData = NULL;

  wxTreeItemId idSelected = event.GetItem();

  CDisplayDataTreeItemData* itemData = dynamic_cast<CDisplayDataTreeItemData*>(GetItemData(idSelected));
  if (itemData != NULL)
  {
    displayData = itemData->GetDisplayData();
  }
  //---------------------------------------------------
  if ((displayData == NULL) && (ItemHasChildren(idSelected)))
  {
    GetItemsDataRecursively(data, idSelected);
  }
  //---------------------------------------------------
  else if ((displayData == NULL) && (!ItemHasChildren(idSelected)))
  {
    id = idSelected;
    do
    {
      idParent = GetItemParent(id);
      if (idParent.IsOk())
      {
        itemData = dynamic_cast<CDisplayDataTreeItemData*>(GetItemData(idParent));
        if (itemData != NULL)
        {
          displayData = itemData->GetDisplayData();
          if (displayData != NULL)
          {
            data.Insert(displayData);
          }
        }
        id = idParent;
      }

    }
    while ((displayData == NULL) && (idParent != idRoot) && (idParent.IsOk()));

  }
  //---------------------------------------------------
  else if (displayData != NULL)
  {
    data.Insert(displayData);
  }
*/
  if (data.size() < 0)
  {
    event.Veto();
    return;
  }

  //event.Allow();

  // start drag operation
  CDndDisplayData* dndDisplayData = new CDndDisplayData(data);

  CDndDisplayDataObject dndDisplayDataObject(dndDisplayData);

  wxDropSource source(dndDisplayDataObject, this, wxDROP_ICON(dnd_copy),
                     wxDROP_ICON(dnd_move), wxDROP_ICON(dnd_none));

  source.DoDragDrop(wxDrag_DefaultMove);
  event.Skip();


/*
  wxDropSource source(textData, this,
                      wxDROP_ICON(dnd_copy),
                      wxDROP_ICON(dnd_move),
                      wxDROP_ICON(dnd_none));
                      */
  /*
  wxDropSource source(textData, this);

  //int flags = 0;

  wxDragResult result = source.DoDragDrop();
  */

}
//----------------------------------------
void CDisplayDataTreeCtrl::GetItemsDataRecursively(CObArray& data, const wxTreeItemId& idParent,
                                     wxTreeItemIdValue cookie /*=0*/)
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

  if ( !id.IsOk() )
  {
    return;
  }

  CDisplayData* displayData = NULL;

  CDisplayDataTreeItemData* itemData = dynamic_cast<CDisplayDataTreeItemData*>(GetItemData(id));
  if (itemData != NULL)
  {
    displayData = itemData->GetDisplayData();
    if (displayData != NULL)
    {
      data.Insert(displayData);
    }
  }

  if (ItemHasChildren(id))
  {
    GetItemsDataRecursively(data, id);
  }

  GetItemsDataRecursively(data, idParent, cookie);
}


//----------------------------------------
void CDisplayDataTreeCtrl::OnGetToolTip( wxTreeEvent &event )
{

  wxTreeItemId itemId = event.GetItem();

  CDisplayDataTreeItemData* itemData = itemId.IsOk() ? dynamic_cast<CDisplayDataTreeItemData*>(GetItemData(itemId))
                                         : NULL;

  if (itemData == NULL)
  {
    event.Veto();
    return;
  }

  SetToolTip(NULL); // This forces to recreate tooltip and to set its length to the max line length of text
  event.SetToolTip(itemData->GetSplittedFieldDescr());


}
//----------------------------------------
void CDisplayDataTreeCtrl::ShowMenu(wxTreeItemId id, const wxPoint& pt)
{
  CDisplayDataTreeItemData* itemData = id.IsOk() ? dynamic_cast<CDisplayDataTreeItemData*>(GetItemData(id))
                                       : NULL;
  if (itemData == NULL)
  {
    return;
  }

  wxTreeItemId idRoot = GetRootItem();
  if (!idRoot)
  {
    return;
  }

  CDisplayData* displayData = itemData->GetDisplayData();

  wxTreeItemId idPrev = id;
  wxTreeItemId idParent = id;

  wxString name = GetItemText(id);

  if ((displayData == NULL) && (!ItemHasChildren(id)))
  {
    idPrev = id;
    do
    {
      idParent = GetItemParent(idPrev);
      if (idParent.IsOk())
      {
        itemData = dynamic_cast<CDisplayDataTreeItemData*>(GetItemData(idParent));
        if (itemData != NULL)
        {
          displayData = itemData->GetDisplayData();
          if (displayData != NULL)
          {
            name = GetItemText(idParent);
            break;
          }
        }
        idPrev = idParent;
      }

    }
    while ((displayData == NULL) && (idParent != idRoot) && (idParent.IsOk()));

  }

  if ( (idParent == idRoot) || (!idParent) )
  {
    return;
  }

  wxString title;

  title.Printf(wxT("Menu for '%s'"), name.c_str());

  wxMenu* menu = wxGetApp().GetMenuDisplayTreeCtrl();
  if (menu == NULL)
  {
    return;
  }

  menu->SetTitle(title);
  /*
  wxMenuItem* menuItem = menu->FindItem(ID_DISPADDTOSEL);
  if (menuItem != NULL)
  {
    wxString text;
    text.Printf(ADD_DISP_TO_SEL_MENU_LABEL, name.c_str());
    menuItem->SetText(text);
    menuItem->Enable(true);
  }

  menuItem = menu->FindItem(ID_DISPEXPANDMENU);
  if (menuItem != NULL)
  {
    if (ItemHasChildren(id))
    {
      wxString text;
      text.Printf(ADD_EXPAND_CHILDREN_MENU_LABEL, GetItemText(id).c_str());
      menuItem->SetText(text);
      menuItem->Enable(true);
    }
    else
    {
      menuItem->SetText("&Expand all children");
      menuItem->Enable(false);
    }
  }
*/
  wxMenuItem* menuItem = menu->FindItem(ID_DISPEXPANDMENU);
  if (menuItem != NULL)
  {
    menuItem->Enable(ItemHasChildren(id));
  }

  menuItem = menu->FindItem(ID_DISPCOLLAPSEMENU);
  if (menuItem != NULL)
  {
    menuItem->Enable(ItemHasChildren(id));
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
//----------------------------------------



