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
#include "display/wxInterface.h"

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "BratGui.h"
#include "new-gui/brat/DataModels/Workspaces/Workspace.h"

#include "DatasetTreeCtrl.h"


//----------------------------------------------------------------------------
// CDatasetTreeItemData
//----------------------------------------------------------------------------

CDatasetTreeItemData::CDatasetTreeItemData(CDataset* dataset)
{
  m_dataset = dataset;
}

// WDR: class implementations

//----------------------------------------------------------------------------
// CDatasetTreeCtrl
//----------------------------------------------------------------------------

// WDR: event table for CDatasetTreeCtrl

BEGIN_EVENT_TABLE(CDatasetTreeCtrl,CTreeCtrl)
    EVT_TREE_SEL_CHANGED( ID_DATASETTREECTRL, CDatasetTreeCtrl::OnDatasetSelChanged )
END_EVENT_TABLE()

CDatasetTreeCtrl::CDatasetTreeCtrl( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    CTreeCtrl( parent, id, position, size, style )
{
  m_product = NULL;
}
//----------------------------------------
void CDatasetTreeCtrl::OnDatasetSelChanged( wxTreeEvent &event )
{
  event.Skip();
}
//----------------------------------------
wxTreeItemId CDatasetTreeCtrl::AddItemToTree(const wxString& datasetName)
{
  wxTreeItemId id;

  CWorkspaceDataset* wks = wxGetApp().GetCurrentWorkspaceDataset();
  if (wks == NULL)
  {
    return id;
  }
  
  if (wks->GetDatasetCount() <= 0)
  {
    return id;
  }

  CDataset* dataset = wks->GetDataset(datasetName.ToStdString());
  
  int image = CTreeCtrl::TreeCtrlIcon_File;

  wxTreeItemId rootId = GetRootItem();
  
  if (!rootId)
  {

    rootId = AddRoot("Root", CTreeCtrl::TreeCtrlIcon_Folder, CTreeCtrl::TreeCtrlIcon_Folder);
  }

  id = AppendItem(rootId, datasetName.c_str(), image, image + 1, new CDatasetTreeItemData(dataset));

  return id;
}
//----------------------------------------
void CDatasetTreeCtrl::AddItemsToTree()
{
  //int image = wxGetApp().ShowImages() ? MyTreeCtrl::TreeCtrlIcon_Folder : -1;
  int image = CTreeCtrl::TreeCtrlIcon_Folder;

  /*wxTreeItemId rootId = AddRoot(wxT("Root"),
                                image, image,
                                new MyTreeItemData(wxT("Root item")));
                                */
  wxTreeItemId rootId = AddRoot(wxT("Root"),
                                image, image);

  CWorkspaceDataset* wks = wxGetApp().GetCurrentWorkspaceDataset();
  if (wks == NULL)
  {
    return;
  }
  
  if (wks->GetDatasetCount() <= 0)
  {
    return;
  }

  const CObMap* datasets = wks->GetDatasets();
  for (CObMap::const_iterator it = datasets->begin() ; it != datasets->end() ; it++)
  {
    CDataset* dataset = dynamic_cast<CDataset*>(it->second);

    //std::string value = it->first;
    image = CTreeCtrl::TreeCtrlIcon_File;
    wxTreeItemId id = AppendItem(rootId, (it->first).c_str(), image, image + 1, new CDatasetTreeItemData(dataset));
  }
}
//----------------------------------------

void CDatasetTreeCtrl::DoToggleIcon(const wxTreeItemId& item)
{
    int image = GetItemImage(item) == TreeCtrlIcon_Folder ? TreeCtrlIcon_File
                                                          : TreeCtrlIcon_Folder;

    SetItemImage(item, image);
}
//----------------------------------------
CDatasetTreeItemData* CDatasetTreeCtrl::GetCurrentItemDataValue()
{
  wxTreeItemId id = GetSelection();
  return GetItemDataValue(id);
}
//----------------------------------------
CDataset* CDatasetTreeCtrl::GetCurrentDataset()
{
  wxTreeItemId id = GetSelection();
  return GetDataset(id);

}
//----------------------------------------
CDatasetTreeItemData* CDatasetTreeCtrl::GetItemDataValue(const wxTreeItemId& id)
{
  CDatasetTreeItemData* itemData = NULL;
  
  if (id.IsOk())
  {
    itemData = dynamic_cast<CDatasetTreeItemData*>(GetItemData(id));
  }

  return itemData;
}

//----------------------------------------
CDataset* CDatasetTreeCtrl::GetDataset(const wxTreeItemId& id)
{
  CDataset* dataset = NULL;

  CDatasetTreeItemData* itemData = GetItemDataValue(id);
  if (itemData != NULL)
  {
    dataset = itemData->GetDataset();
  }

  return dataset;      
}

// WDR: handler implementations for CDatasetTreeCtrl
