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


#ifndef __Dictionary_H__
#define __Dictionary_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "Dictionary.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/dnd.h"

#include "List.h"
#include "Product.h"
#include "Field.h"
using namespace brathl;

#include "BratGui_wdr.h"
#include "new-gui/brat/DataModels/Workspaces/Display.h"

const int32_t DICTCOL_NAME = 2;
const int32_t DICTCOL_RECORD = 1;
const int32_t DICTCOL_FULLNAME = 0;
const int32_t DICTCOL_UNIT = 3;
const int32_t DICTCOL_FORMAT = 4;
const int32_t DICTCOL_DIM = 5;


const int32_t DICTDISPCOL_OPNAME = 0;
const int32_t DICTDISPCOL_FILENAME = 1;
const int32_t DICTDISPCOL_FIELDNAME = 2;
const int32_t DICTDISPCOL_UNIT = 3;
const int32_t DICTDISPCOL_DIM1 = 4;
const int32_t DICTDISPCOL_DIM2 = 5;
const int32_t DICTDISPCOL_DIM3 = 6;
const int32_t DICTDISPCOL_TITLE = 7;
//const int32_t DICTDISPCOL_DESC = 5;

// WDR: class declarations

//-------------------------------------------------------------
//------------------- CDictionaryList class --------------------
//-------------------------------------------------------------

class CDictionaryList: public wxListView
{
public:
  // constructors and destructors
  CDictionaryList( wxWindow *parent, wxWindowID id = -1,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = 0,
      const wxValidator& validator = wxDefaultValidator,
      const wxString &name = wxListCtrlNameStr);

  virtual ~CDictionaryList();

  void ShowFieldDescription();
  wxString GetFieldUnit();
  wxString GetFieldUnit(long item);

  wxString GetSelectedFieldNames(const wxString& sep = " ");
  wxString GetSelectedFieldFullNames(const wxString& sep = " ");
  wxString GetFieldFullNames(long item);

  CField* FindFieldFullNames(const wxString fieldName);

  void InsertProduct(CProduct* product/*, wxArrayString* records = NULL*/);

  int32_t GetSortedCol() {return m_sortedCol;};
  bool GetSortAsc() {return m_sortAsc;};

  wxString GetFieldFormat(CField* field);
  wxString GetFieldDim(CField* field);

  wxTextCtrl* GetDescCtrl() {return m_descCtrl;};
  void SetDescCtrl(wxTextCtrl* value) {m_descCtrl = value;};

  static void EvtListCtrl(wxWindow& window,  int32_t eventType, const wxListEventFunction& method,
                        wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);

    // WDR: method declarations for CDictionaryList
    
private:
  wxImageList *m_imageListSmall;
  CProduct* m_product;

  wxTextCtrl* m_descCtrl;

  int32_t m_sortedCol;
  bool m_sortAsc;
    // WDR: member variable declarations for CDictionaryList
    
private:
  long InsertField(CField* field);
  void CreateColumn();
  void SetColumnImage(int col, int image);
  void ResetColumnImage();

  // WDR: handler declarations for CDictionaryList
    void OnDeselected( wxListEvent &event );
    void OnSelected( wxListEvent &event );
  void OnColClick( wxListEvent &event );
  
private:
    DECLARE_CLASS(CDictionaryList)
    DECLARE_EVENT_TABLE()
};




BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_DELETE_DISPSELECTED_FIELD, 7777) // 7777 is ignored, just for compatibility with v2.2
END_DECLARE_EVENT_TYPES()

//-------------------------------------------------------------
//------------------- CDeleteDisplaySelectedFieldEvent class --------------------
//-------------------------------------------------------------

class CDeleteDisplaySelFieldEvent : public wxCommandEvent
{
public:
  CDeleteDisplaySelFieldEvent(wxWindowID id)
    : wxCommandEvent(wxEVT_DELETE_DISPSELECTED_FIELD, id)
  {
    m_id = id;
  };

  CDeleteDisplaySelFieldEvent(const CDeleteDisplaySelFieldEvent& event)
    : wxCommandEvent(wxEVT_DELETE_DISPSELECTED_FIELD, event.m_id)
  {
    m_id = event.m_id;
  };
  
  virtual wxEvent *Clone() const 
    {
      return new CDeleteDisplaySelFieldEvent(*this); 
    };

  wxWindowID m_id;

};
typedef void (wxEvtHandler::*CDeleteDisplaySelFieldEventFunction)(CDeleteDisplaySelFieldEvent&);


//-------------------------------------------------------------
//------------------- CDictionaryDisplayList class --------------------
//-------------------------------------------------------------

class CDictionaryDisplayList: public wxListView
{
public:
  // constructors and destructors
  CDictionaryDisplayList( wxWindow *parent, wxWindowID id = -1,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = 0,
      const wxValidator& validator = wxDefaultValidator,
      const wxString &name = wxListCtrlNameStr);

  virtual ~CDictionaryDisplayList();

  void ShowFieldDescription();
  
  wxString GetSelectedFieldNames(const wxString& sep = " ");
  wxString GetSelectedFieldFullNames(const wxString& sep = " ");


  void InsertData(CMapDisplayData* data);
  void InsertDisplay(CDisplay* data);

  CDisplay* GetDisplay() { return m_display; };
  void SetDisplay(CDisplay* value) { m_display = value; };

  long InsertField(CDisplayData* displayData);

  int32_t GetSortedCol() {return m_sortedCol;};
  bool GetSortAsc() {return m_sortAsc;};

  wxString GetFieldFormat(CField* field);
  wxString GetFieldDim(CField* field);

  void RemoveFields();
  bool GetAllowSuppr() { return m_allowSuppr; };
  void SetAllowSuppr(bool value) { m_allowSuppr = value; };

  CMapDisplayData* GetMapDisplayData() { return m_data; };

 // deselect all items
 void DeselectAll();

  static void EvtDeleteDisplaySelFieldCommand(wxWindow& window, const CDeleteDisplaySelFieldEventFunction& method,
                        wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);


  static void EvtListCtrl(wxWindow& window,  int32_t eventType, const wxListEventFunction& method,
                        wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);

    // WDR: method declarations for CDictionaryDisplayList
    
private:
  wxImageList *m_imageListSmall;
  CMapDisplayData* m_data;

  CDisplay* m_display;

  int32_t m_sortedCol;
  bool m_allowSuppr;

  bool m_sortAsc;
    // WDR: member variable declarations for CDictionaryDisplayList
    
private:
  void GetOperation(int32_t type);

  void CreateColumn();
  void SetColumnImage(int col, int image);
  void ResetColumnImage();


  // WDR: handler declarations for CDictionaryDisplayList
    void OnKeyUp( wxKeyEvent &event );
    void OnInsertItem( wxListEvent &event );
    void OnDeselected( wxListEvent &event );
    void OnSelected( wxListEvent &event );
  void OnColClick( wxListEvent &event );
  
private:
    DECLARE_CLASS(CDictionaryDisplayList)
    DECLARE_EVENT_TABLE()
};


#endif
