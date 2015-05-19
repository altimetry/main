/*
* Copyright (C) 2008 Collecte Localisation Satellites (CLS), France (31)
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

#ifndef __TreeCtrl_H__
#define __TreeCtrl_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "TreeCtrl.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif



#define USE_GENERIC_TREECTRL 0

#if USE_GENERIC_TREECTRL
#include "wx/generic/treectlg.h"
#ifndef wxTreeCtrl
#define wxTreeCtrl wxGenericTreeCtrl
#define sm_classwxTreeCtrl sm_classwxGenericTreeCtrl
#endif
#endif


// verify that the item is ok and insult the user if it is not
#define CHECK_ITEM( item ) if ( !item.IsOk() ) {                                 \
                             wxMessageBox(wxT("Please select some item first!"), \
                                          wxT("Tree sample error"),              \
                                          wxOK | wxICON_EXCLAMATION,             \
                                          this);                                 \
                             return;                                             \
                           }

#include "wx/dnd.h"


// WDR: class declarations
/*
//----------------------------------------------------------------------------
// CTreeTextDropTarget
//----------------------------------------------------------------------------
class CTreeTextDropTarget : public wxTextDropTarget
{
public:
    CTreeTextDropTarget(CTreeCtrl* pOwner) { m_pOwner = pOwner; }

    virtual bool OnDropText(wxCoord x, wxCoord y, const wxString& text);

private:
    CTreeCtrl* m_pOwner;
};
*/

//----------------------------------------------------------------------------
// CTreeCtrl
//----------------------------------------------------------------------------

class CTreeCtrl : public wxTreeCtrl
{
public:
  
  enum
  {
      TreeCtrlIcon_File,
      TreeCtrlIcon_FileSelected,
      TreeCtrlIcon_Folder,
      TreeCtrlIcon_FolderSelected,
      TreeCtrlIcon_FolderOpened
  };

  CTreeCtrl();
  CTreeCtrl(wxWindow *parent, const wxWindowID id,
             const wxPoint& pos, const wxSize& size,
             long style);

  virtual ~CTreeCtrl();

  virtual void CreateImageList(int size = 16);
  virtual void CreateButtonsImageList(int size = 11);


  void DoSortChildren(const wxTreeItemId& item, bool reverse = false)
      { m_reverseSort = reverse; wxTreeCtrl::SortChildren(item); };
  
  void DoEnsureVisible() { if (m_lastItem.IsOk()) EnsureVisible(m_lastItem); };

  
  int ImageSize(void) const { return m_imageSize; }

  void SetLastItem(wxTreeItemId id) { m_lastItem = id; }

  static wxBitmap TreeIconsDefault( size_t index );

  virtual void ShowMenu(wxTreeItemId id, const wxPoint& pt) { };

  virtual wxTreeItemId FindItem(const wxString& text, bool bCaseSensitive = true, bool bExactMatch = true, bool bFindAtBeginning = false);
  virtual wxTreeItemId FindItem(const wxTreeItemId& from, const wxString& text, bool bCaseSensitive = true, bool bExactMatch = true, bool bFindAtBeginning = false);

  virtual wxTreeItemId FindParentItem(const wxTreeItemId& parentId, const wxString& name, wxTreeItemIdValue cookie = 0);

  virtual void RemoveAllItems(bool bUnselect = true);

  void RemoveItemRecurse(const wxTreeItemId& from);

  virtual void CollapseItemsRecurse();
  virtual void CollapseItemsRecurse(const wxTreeItemId& from);

  virtual void ExpandItemsRecurse();
  virtual void ExpandItemsRecurse(const wxTreeItemId& from);

  bool IsAllItemsExpanded() { return m_allItemsExpanded; };
  void SetAllItemsExpanded(bool value) { m_allItemsExpanded = value; };
  void ReverseAllItemsExpanded() { m_allItemsExpanded = !m_allItemsExpanded; };

/*
    void OnBeginDrag(wxTreeEvent& event);
    void OnBeginRDrag(wxTreeEvent& event);
    void OnEndDrag(wxTreeEvent& event);
    void OnBeginLabelEdit(wxTreeEvent& event);
    void OnEndLabelEdit(wxTreeEvent& event);
    void OnDeleteItem(wxTreeEvent& event);
    void OnContextMenu(wxContextMenuEvent& event);
    void OnItemMenu(wxTreeEvent& event);
    void OnGetInfo(wxTreeEvent& event);
    void OnSetInfo(wxTreeEvent& event);
    void OnItemExpanded(wxTreeEvent& event);
    void OnItemExpanding(wxTreeEvent& event);
    void OnItemCollapsed(wxTreeEvent& event);
    void OnItemCollapsing(wxTreeEvent& event);
    void OnSelChanged(wxTreeEvent& event);
    void OnSelChanging(wxTreeEvent& event);
    void OnTreeKeyDown(wxTreeEvent& event);
    void OnItemActivated(wxTreeEvent& event);
    void OnItemRClick(wxTreeEvent& event);

    void OnRMouseDown(wxMouseEvent& event);
    void OnRMouseUp(wxMouseEvent& event);
    void OnRMouseDClick(wxMouseEvent& event);

    void GetItemsRecursively(const wxTreeItemId& idParent,
                             wxTreeItemIdValue cookie = 0);

    void AddTestItemsToTree(size_t numChildren, size_t depth);


    void DoToggleIcon(const wxTreeItemId& item);

    void ShowMenu(wxTreeItemId id, const wxPoint& pt);
*/
protected:
  /*
    virtual int OnCompareItems(const wxTreeItemId& i1, const wxTreeItemId& i2);

    // is this the test item which we use in several event handlers?
    bool IsTestItem(const wxTreeItemId& item)
    {
        // the test item is the first child folder
        return GetItemParent(item) == GetRootItem() && !GetPrevSibling(item);
    }
*/
/*
    void AddItemsRecursively(const wxTreeItemId& idParent,
                             size_t nChildren,
                             size_t depth,
                             size_t folder);
*/
  void Init();
  virtual void OnContextMenu(wxContextMenuEvent& event);
  virtual void OnItemMenu(wxTreeEvent& event);
  virtual int OnCompareItems(const wxTreeItemId& i1, const wxTreeItemId& i2);

protected:

  bool m_allItemsExpanded;    

  bool m_reverseSort;             // flag for OnCompareItems
  int          m_imageSize;               // current size of images
  wxTreeItemId m_lastItem;                // for OnEnsureVisible()
  wxTreeItemId m_draggedItem;             // item being dragged right now

  // NB: due to an ugly wxMSW hack you _must_ use DECLARE_DYNAMIC_CLASS()
    //     if you want your overloaded OnCompareItems() to be called.
    //     OTOH, if you don't want it you may omit the next line - this will
    //     make default (alphabetical) sorting much faster under wxMSW.
    DECLARE_DYNAMIC_CLASS(CTreeCtrl)
    DECLARE_EVENT_TABLE()

};





#endif
