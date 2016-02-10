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

#ifndef __DisplayDataTreeCtrl_H__
#define __DisplayDataTreeCtrl_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

class CDisplayDataTreeCtrl;

//#include "BratGui.h"
#include "BratGui_wdr.h"

#include "TreeCtrl.h"
#include "new-gui/brat/DataModels/Workspaces/Display.h"



//----------------------------------------------------------------------------
// CDndDisplayDataObject
//----------------------------------------------------------------------------


static const wxChar* displayDataFormatId = _T("CDndDisplayDataObject");


class CDndDisplayDataObject : public wxDataObjectSimple
{
	CDndDisplayData* m_dndDisplayData;
public:

	CDndDisplayDataObject( CDndDisplayData* dndDisplayData = NULL );
	virtual ~CDndDisplayDataObject() { delete m_dndDisplayData; }

	virtual size_t GetDataSize() const;
	virtual bool GetDataHere( void* buf ) const;
	virtual bool SetData( size_t len, const void* buf );

	CDndDisplayData* GetDisplayData() { return m_dndDisplayData; }
};



// WDR: class declarations
//----------------------------------------------------------------------------
// CDisplayDataTreeItemData
//----------------------------------------------------------------------------
class CDisplayDataTreeItemData : public wxTreeItemData
{
public:
  CDisplayDataTreeItemData(bool allowLabelChange = false);
  CDisplayDataTreeItemData(CDisplayData* displayData, bool allowLabelChange = false);

  void SetAllowLabelChange(bool value) { m_allowLabelChange = value; }
  bool GetAllowLabelChange() { return m_allowLabelChange; }

  CDisplayData* GetDisplayData() { return m_displayData; }
  void SetDisplayData(CDisplayData* value);
  /*
  wxTreeItemId GetParentId() { return m_parentId; }
  void SetParentId(const wxTreeItemId& value) { m_parentId = value; }
  */
  int32_t GetType() {return m_type;};
  void SetType(int32_t value) {m_type = value;}; 

  wxString GetSplittedFieldDescr() { return m_splittedFieldDescr; }
  
  void SetSplittedFieldDescr();

private:
    void Init();

private:

  bool m_allowLabelChange;
  CDisplayData* m_displayData;

  int32_t m_type;

  wxString m_splittedFieldDescr;

  //wxTreeItemId m_parentId;

};

//----------------------------------------------------------------------------
// CDisplayDataTreeCtrl
//----------------------------------------------------------------------------

class CDisplayDataTreeCtrl: public CTreeCtrl
{
public:
  // constructors and destructors
  CDisplayDataTreeCtrl( wxWindow *parent, wxWindowID id = -1,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = 0 );
  virtual ~CDisplayDataTreeCtrl();
  
  // WDR: method declarations for CDisplayDataTreeCtrl

  void InsertData(CMapDisplayData* data);

  void ExpandRootData();

  wxTreeItemId FindParentItem(const wxTreeItemId& idParent, int32_t type, wxTreeItemIdValue cookie = 0);
  wxTreeItemId FindParentItem(int32_t type, const wxTreeItemId& fromId = 0);
  //wxTreeItemId FindParentItem(const wxTreeItemId& parentId, const wxString& name, wxTreeItemIdValue cookie = 0);
  
  CDisplayDataTreeItemData* GetCurrentItemDataValue();
  CDisplayData* GetCurrentDisplayData();
  CDisplayDataTreeItemData* GetItemDataValue(const wxTreeItemId& id);
  CDisplayData* GetDisplayData(const wxTreeItemId& id);
  
  CMapDisplayData* GetMapDisplayData() { return m_data; };

  void GetAllDisplayData(CObArray& data, const wxTreeItemId& idSelected);


  int32_t CountData();
  int32_t HasData() {return (CountData() > 0);};

  void DeInstallEventListeners();

  virtual void ShowMenu(wxTreeItemId id, const wxPoint& pt);

  virtual void CollapseItemsRecurse();
    

protected:
    // WDR: member variable declarations for CDisplayDataTreeCtrl
  CMapDisplayData* m_data;

private:
  
  void Init();
  
  // WDR: handler declarations for CDisplayDataTreeCtrl
  void OnBeginDrag( wxTreeEvent &event );

  void OnGetToolTip( wxTreeEvent &event );


  void InitItemsToTree();
  void InsertField(CDisplayData* displayData, CMapDisplayData* data);
  
  void GetItemsDataRecursively(CObArray& data, const wxTreeItemId& idParent, wxTreeItemIdValue cookie =0);

private:
    DECLARE_EVENT_TABLE()
};




#endif
