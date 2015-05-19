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

#ifndef __DatasetTreeCtrl_H__
#define __DatasetTreeCtrl_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "DatasetTreeCtrl.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "BratGui_wdr.h"
#include "TreeCtrl.h"

#include "Product.h"
#include "Dataset.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// CDatasetTreeCtrl
//----------------------------------------------------------------------------

class CDatasetTreeItemData : public wxTreeItemData
{
public:
    CDatasetTreeItemData(CDataset* dataset);

    virtual ~CDatasetTreeItemData() { };

    CDataset* GetDataset() { return m_dataset; }

private:
    CDataset* m_dataset;
};

//----------------------------------------------------------------------------
// CDatasetTreeCtrl
//----------------------------------------------------------------------------

class CDatasetTreeCtrl: public CTreeCtrl
{
public:
  // constructors and destructors
  CDatasetTreeCtrl( wxWindow *parent, wxWindowID id = -1,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = 0 );
  
  // WDR: method declarations for CDatasetTreeCtrl
    virtual ~CDatasetTreeCtrl() { };

  void DoToggleIcon(const wxTreeItemId& item);

  void AddItemsToTree();
  wxTreeItemId AddItemToTree(const wxString& datasetName);

  CDataset* GetDataset(const wxTreeItemId& id);
  CDatasetTreeItemData* GetItemDataValue(const wxTreeItemId& id);
  
  CDatasetTreeItemData* GetCurrentItemDataValue();
  CDataset* GetCurrentDataset();


private:
    // WDR: member variable declarations for CDatasetTreeCtrl
  CProduct* m_product;
    
private:

  // WDR: handler declarations for CDatasetTreeCtrl
    void OnDatasetSelChanged( wxTreeEvent &event );

private:
    DECLARE_EVENT_TABLE()
};




#endif
