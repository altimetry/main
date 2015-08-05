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
    #pragma implementation "TreeCtrl.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/treectrl.h"
#include "wx/image.h"
#include "wx/imaglist.h"
#include "wx/treectrl.h"
#include "wx/math.h"

#ifdef __WIN32__
    // this is not supported by native control
    #define NO_VARIABLE_HEIGHT
#endif


#include "icon1.xpm"
#include "icon2.xpm"
#include "icon3.xpm"
#include "icon4.xpm"
#include "icon5.xpm"

#include "brathl.h"

#include "Stl.h"

#include "Trace.h"
using namespace brathl;

#include "TreeCtrl.h"


// When debugging changes all calls to “new” to be calls to “DEBUG_NEW” allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

// WDR: event table for CTreeCtrl

#if USE_GENERIC_TREECTRL
BEGIN_EVENT_TABLE(CTreeCtrl, wxGenericTreeCtrl)
#else
BEGIN_EVENT_TABLE(CTreeCtrl, wxTreeCtrl)
#endif
/*
    EVT_TREE_BEGIN_DRAG(TreeTest_Ctrl, MyTreeCtrl::OnBeginDrag)
    EVT_TREE_BEGIN_RDRAG(TreeTest_Ctrl, MyTreeCtrl::OnBeginRDrag)
    EVT_TREE_END_DRAG(TreeTest_Ctrl, MyTreeCtrl::OnEndDrag)
    EVT_TREE_BEGIN_LABEL_EDIT(TreeTest_Ctrl, MyTreeCtrl::OnBeginLabelEdit)
    EVT_TREE_END_LABEL_EDIT(TreeTest_Ctrl, MyTreeCtrl::OnEndLabelEdit)
    EVT_TREE_DELETE_ITEM(TreeTest_Ctrl, MyTreeCtrl::OnDeleteItem)
#if 0       // there are so many of those that logging them causes flicker
    EVT_TREE_GET_INFO(TreeTest_Ctrl, MyTreeCtrl::OnGetInfo)
#endif
    EVT_TREE_SET_INFO(TreeTest_Ctrl, MyTreeCtrl::OnSetInfo)
    EVT_TREE_ITEM_EXPANDED(TreeTest_Ctrl, MyTreeCtrl::OnItemExpanded)
    EVT_TREE_ITEM_EXPANDING(TreeTest_Ctrl, MyTreeCtrl::OnItemExpanding)
    EVT_TREE_ITEM_COLLAPSED(TreeTest_Ctrl, MyTreeCtrl::OnItemCollapsed)
    EVT_TREE_ITEM_COLLAPSING(TreeTest_Ctrl, MyTreeCtrl::OnItemCollapsing)

    EVT_TREE_SEL_CHANGED(TreeTest_Ctrl, MyTreeCtrl::OnSelChanged)
    EVT_TREE_SEL_CHANGING(TreeTest_Ctrl, MyTreeCtrl::OnSelChanging)
    EVT_TREE_KEY_DOWN(TreeTest_Ctrl, MyTreeCtrl::OnTreeKeyDown)
    EVT_TREE_ITEM_ACTIVATED(TreeTest_Ctrl, MyTreeCtrl::OnItemActivated)

    // so many differents ways to handle right mouse button clicks...
    EVT_CONTEXT_MENU(MyTreeCtrl::OnContextMenu)
    // EVT_TREE_ITEM_MENU is the preferred event for creating context menus
    // on a tree control, because it includes the point of the click or item,
    // meaning that no additional placement calculations are required.
    EVT_TREE_ITEM_MENU(TreeTest_Ctrl, MyTreeCtrl::OnItemMenu)
    EVT_TREE_ITEM_RIGHT_CLICK(TreeTest_Ctrl, MyTreeCtrl::OnItemRClick)

    EVT_RIGHT_DOWN(MyTreeCtrl::OnRMouseDown)
    EVT_RIGHT_UP(MyTreeCtrl::OnRMouseUp)
    EVT_RIGHT_DCLICK(MyTreeCtrl::OnRMouseDClick)
    */
    EVT_CONTEXT_MENU(CTreeCtrl::OnContextMenu)
END_EVENT_TABLE()

// WDR: class implementations
//----------------------------------------------------------------------------
// CTreeTextDropTarget
//----------------------------------------------------------------------------
/*
bool CTreeTextDropTarget::OnDropText(wxCoord, wxCoord, const wxString& text)
{
  //m_pOwner->Append(text);
  return true;
}
*/
//----------------------------------------------------------------------------
// CTreeCtrl
//----------------------------------------------------------------------------

// CTreeCtrl implementation
#if USE_GENERIC_TREECTRL
IMPLEMENT_DYNAMIC_CLASS(CTreeCtrl, wxGenericTreeCtrl)
#else
IMPLEMENT_DYNAMIC_CLASS(CTreeCtrl, wxTreeCtrl)
#endif
//----------------------------------------

CTreeCtrl::CTreeCtrl()
{
  Init();
}
//----------------------------------------
CTreeCtrl::CTreeCtrl(wxWindow *parent, const wxWindowID id,
                       const wxPoint& pos, const wxSize& size,
                       long style)
          : wxTreeCtrl(parent, id, pos, size, style)
{
  Init();
}
//----------------------------------------
CTreeCtrl::~CTreeCtrl()
{
  RemoveAllItems();
}

//----------------------------------------
void CTreeCtrl::CreateImageList(int size)
{
  if ( size == -1 )
  {
      SetImageList(NULL);
      return;
  }
  if ( size == 0 )
      size = m_imageSize;
  else
      m_imageSize = size;

  // Make an image list containing small icons
  wxImageList *images = new wxImageList(size, size, true);

  // should correspond to TreeCtrlIcon_xxx enum
  wxBusyCursor wait;
  wxIcon icons[5];
  icons[0] = wxIcon(icon1_xpm);
  icons[1] = wxIcon(icon2_xpm);
  icons[2] = wxIcon(icon3_xpm);
  icons[3] = wxIcon(icon4_xpm);
  icons[4] = wxIcon(icon5_xpm);

  int sizeOrig = icons[0].GetWidth();
  for ( size_t i = 0; i < WXSIZEOF(icons); i++ )
  {
      if ( size == sizeOrig )
      {
          images->Add(icons[i]);
      }
      else
      {
          images->Add(wxBitmap(wxBitmap(icons[i]).ConvertToImage().Rescale(size, size)));
      }
  }

  AssignImageList(images);
}
//----------------------------------------
void CTreeCtrl::Init()
{
  m_allItemsExpanded = false;
  m_reverseSort = false;

  CreateImageList();

}

//----------------------------------------
void CTreeCtrl::OnContextMenu(wxContextMenuEvent& event)
{
  wxPoint pt = event.GetPosition();
/*  
  if ( HasFlag(wxTR_MULTIPLE) )
  {
    return;
  }

  wxTreeItemId itemId = GetSelection();


  // check if event was generated by keyboard (MSW-specific?)
  if ( pt.x == -1 && pt.y == -1 ) //(this is how MSW indicates it)
  {
      // attempt to guess where to show the menu
      if ( itemId.IsOk() )
      {
          // if an item was clicked, show menu to the right of it
          wxRect rect;
          GetBoundingRect(itemId, rect, true ); // true: only the label 
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
  */
}

//----------------------------------------

void CTreeCtrl::OnItemMenu(wxTreeEvent& event)
{
    wxTreeItemId itemId = event.GetItem();
    wxPoint clientpt = event.GetPoint();
    wxPoint screenpt = ClientToScreen(clientpt);
    SelectItem(itemId);
    ShowMenu(itemId, clientpt);
    event.Skip();
}
//----------------------------------------
int CTreeCtrl::OnCompareItems(const wxTreeItemId& item1,
                               const wxTreeItemId& item2)
{
    if ( m_reverseSort )
    {
        // just exchange 1st and 2nd items
        return wxTreeCtrl::OnCompareItems(item2, item1);
    }
    else
    {
        return wxTreeCtrl::OnCompareItems(item1, item2);
    }
}
//----------------------------------------
wxTreeItemId CTreeCtrl::FindItem(const wxString& text, bool bCaseSensitive /*= true*/, bool bExactMatch /*= true*/, bool bFindAtBeginning /*= false*/)
{
  return FindItem(GetRootItem(), text, bCaseSensitive, bExactMatch, bFindAtBeginning);
}
//----------------------------------------
wxTreeItemId CTreeCtrl::FindItem(const wxTreeItemId& from, const wxString& text, bool bCaseSensitive /*= true*/, bool bExactMatch /*= true*/, bool bFindAtBeginning /*= false*/)
{
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

      if(!bCaseSensitive)
      {
        itemText.MakeLower();
      }

      if (bExactMatch)
      {
        bFound = (itemText == findText);
      }
      else
      {
        if (bFindAtBeginning)
        {
          bFound = (itemText.Find(findText) == 0);
        }
        else
        {
          bFound = itemText.Contains(findText);
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
      child = FindItem(child, text, bCaseSensitive, bExactMatch, bFindAtBeginning);
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
wxTreeItemId CTreeCtrl::FindParentItem(const wxTreeItemId& parentId, const wxString& name, wxTreeItemIdValue cookie /*= 0*/)
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
//----------------------------------------
void CTreeCtrl::RemoveAllItems(bool bUnselect /*= true*/)
{
  if (bUnselect)
  {
    Unselect();
  }

  // WARNING -------
  // It seems thats DeleteAllItems sometimes crash under Windows.
  // So don't use it and delete item one by one recursively.
   //DeleteAllItems();

  wxTreeItemId rootId = GetRootItem();
  if (!rootId)
  {
    return;
  }

  RemoveItemRecurse(rootId);

  Delete(rootId);

  // Now call DeleteAllItems() to be sure that the 'm_pVirtualRoot' is deleted - Win32 ONLY
#ifdef __WIN32__
  DeleteAllItems();
#endif

}
//----------------------------------------
void CTreeCtrl::RemoveItemRecurse(const wxTreeItemId& from)
{

  stack<wxTreeItemId> items;

  wxTreeItemIdValue cookie;
  wxTreeItemId item  = from;

  if( !item ) 
  {
    return;
  }

  wxTreeItemId child = GetFirstChild(item, cookie);

  while(child.IsOk())
  {
    items.push(child);

    child = GetNextChild(item, cookie);

  }

  cookie = 0;

  child = GetFirstChild(item, cookie);

  while(child.IsOk())
  {
    RemoveItemRecurse(child); 
    child = GetNextChild(item, cookie);
  }

  //wxString itemtext = (item == GetRootItem() ? "Root" : GetItemText(item));

  //CTrace::Tracer("%s Delete tree item %s - child count %ld - items stack %ld" , typeid(*this).name(), itemtext.c_str(),
  //                                                                  (long)GetChildrenCount(item), items.size());

	while (!items.empty())
	{
    wxTreeItemId next = items.top();
		items.pop();
    
    //CTrace::Tracer("%s Delete tree item %s" , typeid(this).name(), GetItemText(next).c_str());
    wxTreeItemData* itemData = GetItemData(next);
    if (itemData != NULL)
    {
      delete itemData;
      SetItemData(next, NULL);
    }
    Delete(next);

  }

  //CTrace::Tracer("=============================================");

  //Delete(item);
}

//----------------------------------------
void CTreeCtrl::CollapseItemsRecurse()
{
  CollapseItemsRecurse(GetRootItem());
  
}
//----------------------------------------
void CTreeCtrl::CollapseItemsRecurse(const wxTreeItemId& from)
{
  wxTreeItemId item = from;
  wxTreeItemId child;
  wxTreeItemIdValue cookie;

  while(item.IsOk())
  {
    if (item != GetRootItem())
    {
      Collapse(item);

    }

    child = GetFirstChild(item, cookie);

    if(child.IsOk())
    {
      CollapseItemsRecurse(child);
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

  m_allItemsExpanded = false;

}

//----------------------------------------
void CTreeCtrl::ExpandItemsRecurse()
{
  ExpandItemsRecurse(GetRootItem());
  
  m_allItemsExpanded = true;

}
//----------------------------------------
void CTreeCtrl::ExpandItemsRecurse(const wxTreeItemId& from)
{
  wxTreeItemId item = from;
  wxTreeItemId child;
  wxTreeItemIdValue cookie;

  while(item.IsOk())
  {
    if (item != GetRootItem())
    {
      Expand(item);

    }

    child = GetFirstChild(item, cookie);

    if(child.IsOk())
    {
      ExpandItemsRecurse(child);
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

}

//----------------------------------------

#if USE_GENERIC_TREECTRL || !defined(__WXMSW__)
void CTreeCtrl::CreateButtonsImageList(int size)
{
    if ( size == -1 )
    {
        SetButtonsImageList(NULL);
        return;
    }

    // Make an image list containing small icons
    wxImageList *images = new wxImageList(size, size, true);

    // should correspond to TreeCtrlIcon_xxx enum
    wxBusyCursor wait;
    wxIcon icons[4];
    icons[0] = wxIcon(icon3_xpm);   // closed
    icons[1] = wxIcon(icon3_xpm);   // closed, selected
    icons[2] = wxIcon(icon5_xpm);   // open
    icons[3] = wxIcon(icon5_xpm);   // open, selected

    for ( size_t i = 0; i < WXSIZEOF(icons); i++ )
    {
        int sizeOrig = icons[i].GetWidth();
        if ( size == sizeOrig )
        {
            images->Add(icons[i]);
        }
        else
        {
            images->Add(wxBitmap(wxBitmap(icons[i]).ConvertToImage().Rescale(size, size)));
        }
    }

    AssignButtonsImageList(images);

#else
void CTreeCtrl::CreateButtonsImageList(int WXUNUSED(size))
{
#endif
}

