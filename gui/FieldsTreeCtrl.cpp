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

#include "new-gui/brat/stdafx.h"

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <typeinfo>

#include "wx/dnd.h"
#include "wx/tokenzr.h"

//#include "wx/dataobj.h"
//#include "wx/clipbrd.h"


#include "BratGui.h"
#include "Dnd.h"
#include "FieldsTreeCtrl.h"


// WDR: class implementations

//----------------------------------------------------------------------------
// CUnitFieldDlg
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(CUnitFieldDlg,wxDialog)

// WDR: event table for CUnitFieldDlg

BEGIN_EVENT_TABLE(CUnitFieldDlg,wxDialog)
    EVT_BUTTON( wxID_OK, CUnitFieldDlg::OnOk )
    EVT_BUTTON( wxID_CANCEL, CUnitFieldDlg::OnCancel )
END_EVENT_TABLE()
//----------------------------------------

CUnitFieldDlg::CUnitFieldDlg( wxWindow *parent, wxWindowID id, const wxString &title,
    CField* field,
    const wxPoint &position, const wxSize& size, long style ) :
    wxDialog( parent, id, title, position, size, style )
{

  m_field = field;

  // WDR: dialog function UnitFieldDlg for CUnitFieldDlg
  //----------------------------
  UnitFieldDlg( this, TRUE );
  //----------------------------

  Init();

}
//----------------------------------------

CUnitFieldDlg::~CUnitFieldDlg()
{
}


//----------------------------------------
void CUnitFieldDlg::Init()
{
  bool isFieldNetCdfCFAttr = (CFieldNetCdfCFAttr::GetFieldNetCdfCFAttr(m_field, false) != NULL);

  if (!isFieldNetCdfCFAttr)
  {
    EnableCtrl();
    return;
  }

  GetUnitfieldField()->SetValue(m_field->GetName().c_str());
  GetUnitfieldActualunit()->SetValue(m_field->GetUnit().c_str());

  EnableCtrl();

}

//----------------------------------------
void CUnitFieldDlg::EnableCtrl()
{
  bool isFieldNetCdfCFAttr = (CFieldNetCdfCFAttr::GetFieldNetCdfCFAttr(m_field, false) != NULL);

  GetUnitfieldField()->Enable(false);
  GetUnitfieldActualunit()->Enable(false);
  GetUnitfieldNewunit()->Enable(isFieldNetCdfCFAttr);
}
// WDR: handler implementations for CUnitFieldDlg
//----------------------------------------

void CUnitFieldDlg::OnOk(wxCommandEvent &event)
{
  m_newUnit = GetUnitfieldNewunit()->GetValue();

  bool bOk = true;

  try
  {
    CUnit unit = m_newUnit;
  }
  catch (CException& e)
  {
    bOk = false;
    wxMessageBox(wxString::Format("'%s' is an invalid unit\nReason\n'%s'",
                                  m_newUnit.c_str(),
                                  e.what()),
                "Warning",
                wxOK | wxICON_EXCLAMATION);

  }

  if (bOk)
  {
    event.Skip();
  }

}
//----------------------------------------

void CUnitFieldDlg::OnCancel(wxCommandEvent &event)
{
    m_newUnit = GetUnitfieldActualunit()->GetValue();

    event.Skip();
}

//----------------------------------------------------------------------------
// CFieldsTreeItemData
//----------------------------------------------------------------------------
CFieldsTreeItemData::CFieldsTreeItemData(CField* field)
{
  m_field = field;

  SetSplittedFieldDescr();

}
//----------------------------------------
void CFieldsTreeItemData::SetSplittedFieldDescr()
{
  SetSplittedFieldDescr(m_field);
}
//----------------------------------------
void CFieldsTreeItemData::SetSplittedFieldDescr(CField* field)
{
  if (field == NULL)
  {
    return;
  }

  m_splittedFieldDescr.Clear();

  m_splittedFieldDescr.Append(field->GetName().c_str());
  m_splittedFieldDescr.Append(":\n");

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

  std::string fieldFullName = field->GetFullName();

  if (fieldFullName.empty() == false)
  {
    m_splittedFieldDescr.Append("\nShort name is: ");
    m_splittedFieldDescr.Append(field->GetFullName().c_str());
    m_splittedFieldDescr.Append("\n");
  }

  std::string fieldFullNameWithRecord = field->GetFullNameWithRecord();

  if (fieldFullNameWithRecord.empty() == false)
  {
    m_splittedFieldDescr.Append("\nFull name is: ");
    m_splittedFieldDescr.Append(field->GetFullNameWithRecord().c_str());
    m_splittedFieldDescr.Append("\n");
  }

  if (field->HasVirtualNbDims())
  {
    m_splittedFieldDescr.Append("\nDimension: ");
    m_splittedFieldDescr.Append(field->GetDimAsString().c_str());
    m_splittedFieldDescr.Append("\n");
  }

  if (!field->GetUnit().empty())
  {
    m_splittedFieldDescr.Append("\nUnit: ");
    m_splittedFieldDescr.Append(field->GetUnit().c_str());
    m_splittedFieldDescr.Append("\n");
  }
}
//----------------------------------------------------------------------------
// CFieldsTreeCtrl
//----------------------------------------------------------------------------

// WDR: event table for CFieldsTreeCtrl

BEGIN_EVENT_TABLE(CFieldsTreeCtrl,CTreeCtrl)
    //EVT_TREE_ITEM_GETTOOLTIP( ID_FIELDSTREECTRL, CFieldsTreeCtrl::OnGetToolTip )
    EVT_TREE_BEGIN_DRAG( ID_FIELDSTREECTRL, CFieldsTreeCtrl::OnBeginDrag )
    EVT_TREE_END_DRAG( ID_FIELDSTREECTRL, CFieldsTreeCtrl::OnEndDrag )
    // EVT_TREE_ITEM_MENU is the preferred event for creating context menus
    // on a tree control, because it includes the point of the click or item,
    // meaning that no additional placement calculations are required.
    EVT_TREE_ITEM_MENU(ID_FIELDSTREECTRL, CFieldsTreeCtrl::OnItemMenu)
    EVT_CONTEXT_MENU(CFieldsTreeCtrl::OnContextMenu)
END_EVENT_TABLE()
//----------------------------------------

CFieldsTreeCtrl::CFieldsTreeCtrl( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    CTreeCtrl( parent, id, position, size, style )
{
  m_product = NULL;



}

//----------------------------------------
void CFieldsTreeCtrl::DeInstallEventListeners()
{
  this->Disconnect(wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP);

}

//----------------------------------------
void CFieldsTreeCtrl::InsertProduct(CProduct* product)
{
  this->Disconnect(wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP);

  m_product = product;
  RemoveAllItems();

  if (m_product == NULL)
  {
    return;
  }

  CTreeField* tree = m_product->GetTreeField();

  if (tree->GetRoot() == NULL)
  {
    return;
  }


  int image = CTreeCtrl::TreeCtrlIcon_Folder;


  wxTreeItemId parentId;

  tree->SetWalkDownRootPivot();

  do
  {
    parentId = InsertField(parentId, tree->GetWalkCurrent());
  }
  while (tree->SubTreeWalkDown());

  this->Connect(wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 (wxTreeEventFunction)&CFieldsTreeCtrl::OnGetToolTip,
                 NULL,
                 this);
}

//----------------------------------------
wxTreeItemId CFieldsTreeCtrl::InsertField(const wxTreeItemId& parentId, CObjectTreeNode* node)
{
  int32_t depth = node->GetLevel();

  long numChildren = node->ChildCount();

  CField *field  = dynamic_cast<CField*>(node->GetData());
  if (field == NULL)
  {
    wxMessageBox("ERROR in CFieldsTreeCtrl::InsertField - at least one of the tree object is not a CField object",
                 "Error",
              wxOK | wxICON_ERROR);
    return parentId;
  }

  if ((typeid(*field) == typeid(CFieldIndex)) && field->IsVirtual())
  {
    return parentId;
  }

  if ( depth <= 1 )
  {

    wxString str;
    str.Printf(wxT("%s"), field->GetName().c_str());

    // here we pass to AppendItem() normal and selected item images (we
    // suppose that selected image follows the normal one in the enum)
    int32_t image = TreeCtrlIcon_Folder;
    int32_t imageSel = image + 1;

    wxTreeItemId id = AddRoot(str, image, image, new CFieldsTreeItemData(field));

    return id;
  }



  bool hasChildren = numChildren > 0;

  wxString str;
  if (field->GetUnit().empty())
  {
    str.Printf(wxT("%s"), field->GetName().c_str());
  }
  else
  {
    str.Printf(wxT("%s (%s)"), field->GetName().c_str(), field->GetUnit().c_str());
  }

  /*
  // at depth 1 elements won't have any more children
  if ( hasChildren )
  {
      str.Printf(wxT("%s"), field->GetName().c_str());
  }
  else
  {
    str.Printf(wxT("%s child %d.%d"), wxT("File"), folder, n + 1);
  }
  */
  // here we pass to AppendItem() normal and selected item images (we
  // suppose that selected image follows the normal one in the enum)
  int32_t image = (hasChildren) ?  TreeCtrlIcon_Folder : TreeCtrlIcon_File;
  int32_t imageSel = image + 1;

  //wxTreeItemId id = AppendItem(idParent, str, image, imageSel,
  //                             new MyTreeItemData(str));

  wxTreeItemId id = AppendItem(parentId, str, image, imageSel,
                               new CFieldsTreeItemData(field));


  //Expand(parentId);

  if    ( (typeid(*field) == typeid(CFieldRecord))
     || ( (typeid(*field) == typeid(CFieldArray)) && (field->IsFixedSize() == false) ) )
  {
    SetItemBold(id, true);
    //SetItemFont(parentId, *wxSWISS_FONT);
    //SetItemFont(rootId, *wxITALIC_FONT);
  }

  // and now we also set the expanded one (only for the folders)
  if ( hasChildren )
  {
      SetItemImage(id, TreeCtrlIcon_FolderOpened,
                   wxTreeItemIcon_Expanded);
  }

  // remember the last child for OnEnsureVisible()
  if ( !hasChildren  )
  {
      m_lastItem = id;
  }

  wxTreeItemId returnId;

  if (hasChildren)
  {
    returnId = id;
  }
  else
  {
    returnId = parentId;
    CObjectTreeNode* parentNode = node->GetParent();
    while (parentNode != NULL)
    {
      if (parentNode->m_current == parentNode->GetChildren().end())
      {
        returnId = GetItemParent(returnId);
        parentNode = parentNode->GetParent();
      }
      else
      {
        break;
      }
    }
  }

  return returnId;

}
//----------------------------------------
void CFieldsTreeCtrl::UpdateFieldDescription()
{
  UpdateFieldDescription(GetSelection());
}
//----------------------------------------
void CFieldsTreeCtrl::UpdateFieldDescription(const wxTreeItemId& id)
{
  if (!id.IsOk())
  {
    return;
  }

  CFieldsTreeItemData* itemData = GetItemDataValue(id);;


  if (itemData == NULL)
  {
    return;
  }


  CField* field = itemData->GetField();

  if (field == NULL)
  {
    return;
  }

  itemData->SetSplittedFieldDescr(field);

  wxString str;
  if (field->GetUnit().empty())
  {
    str.Printf(wxT("%s"), field->GetName().c_str());
  }
  else
  {
    str.Printf(wxT("%s (%s)"), field->GetName().c_str(), field->GetUnit().c_str());
  }

  SetItemText(id, str);

  Refresh();

}
//----------------------------------------
CFieldsTreeItemData* CFieldsTreeCtrl::GetCurrentItemDataValue()
{
  wxTreeItemId id = GetSelection();
  return GetItemDataValue(id);
}
//----------------------------------------
CField* CFieldsTreeCtrl::GetCurrentField()
{
  wxTreeItemId id = GetSelection();
  return GetField(id);

}
//----------------------------------------
wxString CFieldsTreeCtrl::GetCurrentRecord()
{
	wxTreeItemId id = GetSelection();
	if ( !id )
	{
		wxTreeItemIdValue cookie;
		wxTreeItemId rootId = GetRootItem();
		if ( rootId.IsOk() )
		{
			id = GetFirstChild( rootId, cookie );
		}
	}
	return GetRecord( id );
}
//----------------------------------------
int32_t CFieldsTreeCtrl::GetRecordCount()
{
  wxTreeItemId rootId = GetRootItem();
  if (!rootId)
  {
    return 0;
  }

  return GetChildrenCount(rootId, false);
}

//----------------------------------------
wxTreeItemId CFieldsTreeCtrl::GetFirstRecordId()
{
  wxTreeItemId rootId = GetRootItem();
  if (!rootId)
  {
    return rootId;
  }

  wxTreeItemIdValue cookie;
  return GetFirstChild(rootId, cookie);
}
//----------------------------------------
CFieldsTreeItemData* CFieldsTreeCtrl::GetItemDataValue(const wxTreeItemId& id)
{
  CFieldsTreeItemData* itemData = NULL;

  if (id.IsOk())
  {
    itemData = dynamic_cast<CFieldsTreeItemData*>(GetItemData(id));
  }

  return itemData;
}

//----------------------------------------
CField* CFieldsTreeCtrl::GetField(const wxTreeItemId& id)
{
  CField* field = NULL;
  CFieldsTreeItemData* itemData = GetItemDataValue(id);

  if (itemData != NULL)
  {
    field = itemData->GetField();
  }

  return field;

}
//----------------------------------------
wxString CFieldsTreeCtrl::GetRecord(const wxTreeItemId& id)
{
  wxString recordName;

  if (id.IsOk())
  {
    CField* field = GetField(id);

    if (field != NULL)
    {
      recordName = field->GetRecordName().c_str();
    }

    if ((recordName.IsEmpty()) && (m_product->IsNetCdf()))
    {
      recordName = CProductNetCdf::m_virtualRecordName.c_str();
    }
  }


  return recordName;

}

//----------------------------------------

/*
void MyTreeCtrl::AddItemsRecursively(const wxTreeItemId& idParent,
                                     size_t numChildren,
                                     size_t depth,
                                     size_t folder)
{
    if ( depth > 0 )
    {
        bool hasChildren = depth > 1;

        wxString str;
        for ( size_t n = 0; n < numChildren; n++ )
        {
            // at depth 1 elements won't have any more children
            if ( hasChildren )
                str.Printf(wxT("%s child %d"), wxT("Folder"), n + 1);
            else
                str.Printf(wxT("%s child %d.%d"), wxT("File"), folder, n + 1);

            // here we pass to AppendItem() normal and selected item images (we
            // suppose that selected image follows the normal one in the enum)
            int image, imageSel;
            if ( wxGetApp().ShowImages() )
            {
                image = depth == 1 ? TreeCtrlIcon_File : TreeCtrlIcon_Folder;
                imageSel = image + 1;
            }
            else
            {
                image = imageSel = -1;
            }
            wxTreeItemId id = AppendItem(idParent, str, image, imageSel,
                                         new MyTreeItemData(str));

            // and now we also set the expanded one (only for the folders)
            if ( hasChildren && wxGetApp().ShowImages() )
            {
                SetItemImage(id, TreeCtrlIcon_FolderOpened,
                             wxTreeItemIcon_Expanded);
            }

            // remember the last child for OnEnsureVisible()
            if ( !hasChildren && n == numChildren - 1 )
            {
                m_lastItem = id;
            }

            AddItemsRecursively(id, numChildren, depth - 1, n + 1);
        }
    }
    //else: done!
}
*/

// WDR: handler implementations for CFieldsTreeCtrl


//----------------------------------------
void CFieldsTreeCtrl::OnBeginDrag( wxTreeEvent &event )
{

  if (ItemHasChildren(event.GetItem()))
  {
    event.Veto();
    return;
  }

  CFieldsTreeItemData* itemData = dynamic_cast<CFieldsTreeItemData*>(GetItemData(event.GetItem()));
  if (itemData == NULL)
  {
    event.Veto();
    return;
  }

  CField* field = itemData->GetField();
  if (field == NULL)
  {
    event.Veto();
    return;
  }

  //event.Allow();

  // start drag operation
  CDndField* dndField = new CDndField();
//  dndField->AddField(field);
  dndField->m_field  = field;

  CDndFieldDataObject dndFieldDataObject(dndField);

  wxDropSource source(dndFieldDataObject, this, wxDROP_ICON(dnd_copy),
                     wxDROP_ICON(dnd_move), wxDROP_ICON(dnd_none));

  //source.DoDragDrop(wxDrag_DefaultMove); // Effectuer l'operation de D&D
  source.DoDragDrop(); // Effectuer l'opération de D&D

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
void CFieldsTreeCtrl::OnEndDrag( wxTreeEvent &event )
{
  event.Skip();
}
//----------------------------------------
void CFieldsTreeCtrl::OnGetToolTip( wxTreeEvent &event )
{
  if (m_product == NULL)
  {
    event.Veto();
    return;
  }

  wxTreeItemId itemId = event.GetItem();

  CFieldsTreeItemData* itemData = itemId.IsOk() ? dynamic_cast<CFieldsTreeItemData*>(GetItemData(itemId))
                                         : NULL;

  if (itemData == NULL)
  {
    event.Veto();
    return;
  }

  CField* field = itemData->GetField();
  if (field == NULL)
  {
    event.Veto();
    return;
  }
  /*
  wxString str;

  wxStringTokenizer tkz(wxT(field->GetDescription()), wxT("."), wxTOKEN_RET_DELIMS);
  while ( tkz.HasMoreTokens() ){
      str.Append(tkz.GetNextToken());
      if (tkz.HasMoreTokens())
      {
        str.Append("\n");
      }
  }
*/

  SetToolTip(NULL); // This forces to recreate tooltip and to set its length to the max line length of text
  event.SetToolTip(itemData->GetSplittedFieldDescr());


}

//----------------------------------------
wxString CFieldsTreeCtrl::GetFieldUnit(const wxTreeItemId& item)
{
  CUnit unit;

  CFieldsTreeItemData* itemData = item.IsOk() ? dynamic_cast<CFieldsTreeItemData*>(GetItemData(item))
                                         : NULL;

  CField* field = NULL;


  if (itemData != NULL)
  {
    field = itemData->GetField();
  }

  if (field == NULL)
  {
    return unit.GetText().c_str();
  }

  return field->GetUnit().c_str();
}


/*
//----------------------------------------
void CFieldsTreeCtrl::OnItemMenu(wxTreeEvent& event)
{
  wxTreeItemId itemId = event.GetItem();
  CFieldsTreeItemData* itemData = itemId.IsOk() ? dynamic_cast<CFieldsTreeItemData*>(GetItemData(itemId))
                                       : NULL;
  if (itemData == NULL)
  {
    return;
  }

  CField* field = itemData->GetField();
  if (field == NULL)
  {
    return;
  }
   // wxLogMessage(wxT("OnItemMenu for item \"%s\""), field->GetName().c_str());

    event.Skip();
}
*/
/*
//----------------------------------------
void CFieldsTreeCtrl::OnContextMenu(wxContextMenuEvent& event)
{
  wxPoint pt = event.GetPosition();
  if ( HasFlag(wxTR_MULTIPLE) )
  {
    return;
  }

  wxTreeItemId itemId = GetSelection();
  // wxLogMessage(wxT("OnContextMenu at screen coords (%i, %i)"), pt.x, pt.y);

  // check if event was generated by keyboard (MSW-specific?)
  if ( pt.x == -1 && pt.y == -1 ) //(this is how MSW indicates it)
  {
      // attempt to guess where to show the menu
      if ( itemId.IsOk() )
      {
          // if an item was clicked, show menu to the right of it
          wxRect rect;
          GetBoundingRect(itemId, rect, true ); // only the label
          pt = wxPoint(rect.GetRight(), rect.GetTop());
      }
      else
      {
          pt = wxPoint(0, 0);
      }
  }
  else // event was generated by mouse, use supplied coords
  {
      pt = ScreenToClient(pt);
  }

  ShowMenu(itemId, pt);
}
*/
//----------------------------------------
void CFieldsTreeCtrl::ShowMenu(wxTreeItemId id, const wxPoint& pt)
{
  CFieldsTreeItemData* itemData = id.IsOk() ? dynamic_cast<CFieldsTreeItemData*>(GetItemData(id))
                                       : NULL;
  if (itemData == NULL)
  {
    return;
  }

  CField* field = itemData->GetField();
  if (field == NULL)
  {
    return;
  }


  wxString title;
  //title << wxT("Menu for ") << field->GetName().c_str();
  title.Printf(wxT("Menu for '%s'"), field->GetName().c_str());

  wxMenu* menu = wxGetApp().GetMenuFieldsTreeCtrl();
  if (menu == NULL)
  {
    return;
  }

  menu->SetTitle(title);

  wxMenuItem* menuItem = menu->FindItem(ID_ASXMENU);
  if (menuItem != NULL)
  {
    menuItem->Enable(!ItemHasChildren(id));
  }

  menuItem = menu->FindItem(ID_ASYMENU);
  if (menuItem != NULL)
  {
    menuItem->Enable(!ItemHasChildren(id));
  }

  menuItem = menu->FindItem(ID_ASDATAMENU);
  if (menuItem != NULL)
  {
    menuItem->Enable(!ItemHasChildren(id));
  }

  menuItem = menu->FindItem(ID_ASSELECTMENU);
  if (menuItem != NULL)
  {
    menuItem->Enable(!ItemHasChildren(id));
  }

  menuItem = menu->FindItem(ID_FIELD_ATTR_CHANGE_UNIT);
  if (menuItem != NULL)
  {
    menuItem->Enable(CField::GetFieldNetCdfCFAttr(field, false) != NULL);
  }


  PopupMenu(menu, pt);
/*
#if wxUSE_MENUS
    wxMenu menu(title);
    menu.Append(-1, wxT("&Set as X..."));
    menu.Append(-1, wxT("&Set as Y..."));
    menu.Append(-1, wxT("&Set as Data..."));
    menu.Append(-1, wxT("&Set as Selection criteria..."));

#endif // wxUSE_MENUS
*/
}


