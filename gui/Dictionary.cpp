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
    #pragma implementation "Dictionary.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <typeinfo>

#include "wx/imaglist.h"

#include "TreeField.h"
#include "Unit.h"
#include "new-gui/Common/tools/Exception.h"
#include "new-gui/brat/Workspaces/Operation.h"

using namespace brathl;

#include "Dictionary.h"


#if !wxCHECK_VERSION(2, 9, 0)
// anything before wxWidgets 2.9 will be adjusted
#define wxIntPtr long
#endif



// WDR: class implementations


static int wxCALLBACK DictListCompareFunction(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData)
{

  CDictionaryList* dictList = (CDictionaryList*)(sortData);


  CField* f1 = (CField*)(item1);
  CField* f2 = (CField*)(item2);

  int result = 0;

  switch (dictList->GetSortedCol())
  {
  case DICTCOL_NAME :
    result = f1->GetName().compare(f2->GetName());
    break;
  case DICTCOL_RECORD :
    result = f1->GetRecordName().compare(f2->GetRecordName());
    break;
  case DICTCOL_FULLNAME :
    result = f1->GetFullName().compare(f2->GetFullName());
    break;
  case DICTCOL_UNIT :
    result = f1->GetUnit().compare(f2->GetUnit());
    break;
  case DICTCOL_FORMAT :
    result = dictList->GetFieldFormat(f1).Cmp( dictList->GetFieldFormat(f2) );
    break;
  case DICTCOL_DIM :
    result = dictList->GetFieldDim(f1).Cmp( dictList->GetFieldDim(f2) );
    break;
  default:
    break;
  }

  if (dictList->GetSortAsc() == false)
  {
    if (result > 0)
    {
      result = -1;
    }
    else if (result < 0)
    {
      result = 1;
    }
  }

  return result;
}

//----------------------------------------


static int wxCALLBACK DictDisplayListCompareFunction(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData)
{

  CDictionaryDisplayList* dictList = (CDictionaryDisplayList*)(sortData);


  CDisplayData* d1 = (CDisplayData*)(item1);
  CDisplayData* d2 = (CDisplayData*)(item2);

  int result = 0;

  switch (dictList->GetSortedCol())
  {
  case DICTDISPCOL_OPNAME :
    result = wxString(d1->GetOperation()->GetName()).CmpNoCase(d2->GetOperation()->GetName());
    break;
  case DICTDISPCOL_FILENAME :
    result = wxString(d1->GetOperation()->GetOutputName()).CmpNoCase(d2->GetOperation()->GetOutputName());
    break;
  case DICTDISPCOL_FIELDNAME :
    result = d1->GetField()->GetName().compare(d2->GetField()->GetName());
    break;
  case DICTDISPCOL_UNIT :
    result = d1->GetField()->GetUnit().compare(d2->GetField()->GetUnit());
    break;
  case DICTDISPCOL_DIM1 :
    result = d1->GetX()->GetUnit().compare(d2->GetX()->GetUnit());
    break;
  case DICTDISPCOL_DIM2 :
    result = d1->GetY()->GetUnit().compare(d2->GetY()->GetUnit());
    break;
  case DICTDISPCOL_DIM3 :
    result = d1->GetZ()->GetUnit().compare(d2->GetZ()->GetUnit());
    break;
  case DICTDISPCOL_TITLE :
    result = d1->GetField()->GetDescription().compare(d2->GetField()->GetDescription());
    break;
  default:
    break;
  }

  if (dictList->GetSortAsc() == false)
  {
    if (result > 0)
    {
      result = -1;
    }
    else if (result < 0)
    {
      result = 1;
    }
  }

  return result;
}
//-------------------------------------------------------------
//------------------- CDictionaryList class --------------------
//-------------------------------------------------------------


IMPLEMENT_CLASS(CDictionaryList,wxListView)

// WDR: event table for CDictionaryList

BEGIN_EVENT_TABLE(CDictionaryList,wxListView)
    //EVT_LIST_COL_CLICK( ID_DICTLIST, CDictionaryList::OnColClick )
    //EVT_LIST_ITEM_SELECTED( ID_DICTLIST, CDictionaryList::OnSelected )
    //EVT_LIST_ITEM_DESELECTED( ID_DICTLIST, CDictionaryList::OnDeselected )
END_EVENT_TABLE()

//----------------------------------------
CDictionaryList::CDictionaryList( wxWindow *parent, wxWindowID id,
                                  const wxPoint &position, const wxSize& size, long style,
                                  const wxValidator& validator,
                                  const wxString &name)
      : wxListView( parent, id, position, size, style, validator, name)
{

  m_sortedCol = -1;
  m_sortAsc = true;
  m_product = NULL;
  m_descCtrl = NULL;

  m_imageListSmall = new wxImageList(16, 16, true);
  m_imageListSmall->Add( BitmapsList(0) );
  m_imageListSmall->Add( BitmapsList(1) );

  SetImageList(m_imageListSmall, wxIMAGE_LIST_SMALL);
  CreateColumn();

  CDictionaryList::EvtListCtrl(*this, wxEVT_COMMAND_LIST_ITEM_SELECTED,
                               (wxListEventFunction)&CDictionaryList::OnSelected,
                               NULL,
                               this);
  CDictionaryList::EvtListCtrl(*this, wxEVT_COMMAND_LIST_ITEM_DESELECTED,
                               (wxListEventFunction)&CDictionaryList::OnDeselected,
                               NULL,
                               this);
  CDictionaryList::EvtListCtrl(*this, wxEVT_COMMAND_LIST_COL_CLICK,
                               (wxListEventFunction)&CDictionaryList::OnColClick,
                               NULL,
                               this);
  wxSize bestsize = GetBestSize();
  bestsize.x = 100;
  SetMinSize(bestsize);
  CacheBestSize(bestsize);
}

//----------------------------------------
CDictionaryList::~CDictionaryList()
{
  if (m_imageListSmall != NULL)
  {
    delete m_imageListSmall;
    m_imageListSmall = NULL;
  }
}
//----------------------------------------
void CDictionaryList::CreateColumn()
{

  wxListItem itemCol;

  itemCol.SetText(_T("Full name"));
  itemCol.SetWidth(200);
  itemCol.SetAlign(wxLIST_FORMAT_CENTER);
  this->InsertColumn(DICTCOL_FULLNAME, itemCol);

  itemCol.SetText(_T("Record"));
  itemCol.SetWidth(100);
  itemCol.SetAlign(wxLIST_FORMAT_CENTER);
  this->InsertColumn(DICTCOL_RECORD, itemCol);

  itemCol.SetText(_T("Name"));
  itemCol.SetWidth(150);
  itemCol.SetAlign(wxLIST_FORMAT_CENTER);
  this->InsertColumn(DICTCOL_NAME, itemCol);

  itemCol.SetText(_T("Unit"));
  itemCol.SetWidth(80);
  itemCol.SetAlign(wxLIST_FORMAT_CENTER);
  this->InsertColumn(DICTCOL_UNIT, itemCol);

  itemCol.SetText(_T("Format"));
  itemCol.SetWidth(80);
  itemCol.SetAlign(wxLIST_FORMAT_CENTER);
  this->InsertColumn(DICTCOL_FORMAT, itemCol);

  itemCol.SetText(_T("Dim."));
  itemCol.SetWidth(120);
  itemCol.SetAlign(wxLIST_FORMAT_CENTER);
  this->InsertColumn(DICTCOL_DIM, itemCol);

}
//----------------------------------------
void CDictionaryList::ResetColumnImage()
{
  for (int32_t i = 0 ; i < GetColumnCount() ; i ++)
  {
    SetColumnImage(i, -1);
  }

}
//----------------------------------------
void CDictionaryList::InsertProduct(CProduct* product, wxArrayString* records)
{
  m_sortedCol = -1;
  m_sortAsc = true;
  bool showRow = false;
  ResetColumnImage();

  m_product = product;
  DeleteAllItems();

  if (m_product == NULL)
  {
    if (m_descCtrl != NULL)
    {
      m_descCtrl->Clear();
    }
    return;
  }

  CTreeField* tree = m_product->GetTreeField();

  if (tree->GetRoot() == NULL)
  {
    return;
  }

  tree->SetWalkDownRootPivot();

  do
  {
    CField *field  = dynamic_cast<CField*>(tree->GetWalkCurrent()->GetData());
    if (field == NULL)
    {
      wxMessageBox("ERROR in CDictionaryList::InsertProduct - at least one of the tree object is not a CField object",
                   "Error",
                wxOK | wxICON_ERROR);
      return;
    }

    showRow = true;
    if (records != NULL)
    {
      showRow = (records->Index(field->GetRecordName().c_str()) == -1) ? false : true;
    }

    if (showRow)
    {
      InsertField(field);
    }

  }
  while (tree->SubTreeWalkDown());



/*

  int32_t index = GetItemCount();

  wxString buf;
  buf.Printf(_T("This is item %d"), index);
  long tmp = InsertItem(index, buf, -1);
  SetItemData(tmp, GetItemCount());

  buf.Printf(_T("Col 1, item %d"), index);
  SetItem(index, 1, buf);

  buf.Printf(_T("Item %d in column 2"), index);
  SetItem(index, 2, buf);
  */

}
//----------------------------------------
long CDictionaryList::InsertField(CField* field)
{

  if (typeid(*field) == typeid(CFieldIndex))
  {
    return -1;
  }

  if (typeid(*field) == typeid(CFieldRecord))
  {
    return -1;
  }

  if ( (typeid(*field) == typeid(CFieldArray)) && (field->IsFixedSize() == false) )
  {
    return -1;
  }

  if (field->IsVirtual())
  {
    return -1;
  }

  int32_t index = GetItemCount();

  wxListItem itemRow;
  itemRow.SetText(field->GetName().c_str());
  itemRow.SetMask(wxLIST_MASK_TEXT);
  itemRow.SetImage(-1);
  itemRow.SetData(field);
  itemRow.SetId(index);

  long indexInserted = InsertItem(itemRow);


  //long tmp = InsertItem(index, field->GetName().c_str(),  DICTCOL_NAME);

  SetItem(index, DICTCOL_FULLNAME, field->GetFullName().c_str());
  SetItem(index, DICTCOL_RECORD, field->GetRecordName().c_str());
  SetItem(index, DICTCOL_NAME, field->GetName().c_str());
  SetItem(index, DICTCOL_UNIT, field->GetUnit().c_str());

  wxString format = GetFieldFormat(field);
  SetItem(index, DICTCOL_FORMAT, format);

  wxString dim = GetFieldDim(field);
  SetItem(index, DICTCOL_DIM, dim);

  return indexInserted;


}
//----------------------------------------
wxString CDictionaryList::GetFieldFormat(CField* field)
{
  wxString format;
//  if (typeid(*field) != typeid(CFieldNetCdf))
/*  if (dynamic_cast<CFieldNetCdf*>(field) != NULL)
  {
    format = "Unknown";
    if (field->IsSpecialType())
    {
      format = field->GetSpecialTypeName().c_str();
    }
    else
    {
      format = field->GetNativeTypeName().c_str();
    }
  }
*/
  try
  {
    //format = "Unknown";
    if (field->IsSpecialType())
    {
      format = field->GetSpecialTypeName().c_str();
    }
    else
    {
      format = field->GetNativeTypeName().c_str();
    }
  }
  catch (CException& e)
  {
    e.what(); // To avoid warning compilation
  }

  return format;
}
//----------------------------------------
wxString CDictionaryList::GetFieldDim(CField* field)
{
  wxString dim;
  bool showDims = false;
  showDims |=  (dynamic_cast<CFieldNetCdfCF*>(field) != NULL);
  showDims |=  (dynamic_cast<CFieldArray*>(field) != NULL);

  int32_t nbDims = field->GetNbDims();
//  if (typeid(*field) == typeid(CFieldArray))
  if ((showDims) && (nbDims > 0))
  {
    dim = wxString::Format("Nb:%d - Dims:%s", nbDims, field->GetDimAsString().c_str());
  }

  return dim;

}
//----------------------------------------
void CDictionaryList::SetColumnImage(int col, int image)
{
#if !defined(__WXMAC__)
    wxListItem item;
    item.SetMask(wxLIST_MASK_IMAGE);
    item.SetImage(image);
    SetColumn(col, item);
#endif
}



// WDR: handler implementations for CDictionaryList

//----------------------------------------
void CDictionaryList::OnDeselected( wxListEvent &event )
{
  ShowFieldDescription();
  event.Skip();
}

//----------------------------------------
void CDictionaryList::OnSelected( wxListEvent &event )
{
  ShowFieldDescription();
  event.Skip();
}

//----------------------------------------
void CDictionaryList::OnColClick( wxListEvent &event )
{
  int32_t col = event.GetColumn();

  if (m_sortedCol != col)
  {
    ResetColumnImage();
    m_sortedCol = col;
    m_sortAsc = true;
  }
  else
  {
    m_sortAsc = !m_sortAsc;
  }

  SetColumnImage(col, m_sortAsc ? 1 : 0);

  SortItems(DictListCompareFunction, (long)this);

  event.Skip();

}
//----------------------------------------
void CDictionaryList::ShowFieldDescription()
{
  if (m_descCtrl == NULL)
  {
    return;
  }

  int32_t selCount = GetSelectedItemCount();

  wxString desc;
  long item = GetNextItem(-1, wxLIST_NEXT_ALL,
                              wxLIST_STATE_SELECTED);
  while ( item != -1 )
  {
    CField* field = (CField*)(GetItemData(item));
    desc += wxString::Format("%s:\n---------------------------\n%s\n\n",
                             field->GetName().c_str(),
                             field->GetDescription().c_str());
    item = GetNextItem(item, wxLIST_NEXT_ALL,
                            wxLIST_STATE_SELECTED);
  }

  m_descCtrl->SetValue(desc);
}
//----------------------------------------
wxString CDictionaryList::GetSelectedFieldNames(const wxString& sep)
{
  wxString text;

  int32_t selCount = GetSelectedItemCount();

  wxString desc;
  long item = GetNextItem(-1, wxLIST_NEXT_ALL,
                              wxLIST_STATE_SELECTED);
  while ( item != -1 )
  {
    CField* field = (CField*)(GetItemData(item));
    text += wxString::Format("%s%s",
                             field->GetName().c_str(),
                             sep.c_str());
    item = GetNextItem(item, wxLIST_NEXT_ALL,
                            wxLIST_STATE_SELECTED);
  }

  return text;
}
//----------------------------------------
wxString CDictionaryList::GetFieldUnit()
{
  CUnit unit;
  int32_t selCount = GetSelectedItemCount();
  if (selCount <= 0)
  {
    return unit.GetText().c_str();
  }

  long item = GetNextItem(-1, wxLIST_NEXT_ALL,
                              wxLIST_STATE_SELECTED);

  if ( item < 0 )
  {
    return unit.GetText().c_str();
  }

  CField* field = (CField*)(GetItemData(item));

  if (field == NULL)
  {
    return unit.GetText().c_str();
  }

  return field->GetUnit().c_str();

}
//----------------------------------------
wxString CDictionaryList::GetFieldUnit(long item)
{
  CUnit unit;

  CField* field = (CField*)(GetItemData(item));

  if (field == NULL)
  {
    return unit.GetText().c_str();
  }

  return field->GetUnit().c_str();
}
//----------------------------------------
wxString CDictionaryList::GetSelectedFieldFullNames(const wxString& sep)
{
  wxString text;

  int32_t selCount = GetSelectedItemCount();

  wxString desc;
  long item = GetNextItem(-1, wxLIST_NEXT_ALL,
                              wxLIST_STATE_SELECTED);
  while ( item != -1 )
  {
    CField* field = (CField*)(GetItemData(item));
    text += wxString::Format("%s%s",
                             field->GetFullName().c_str(),
                             sep.c_str());
    item = GetNextItem(item, wxLIST_NEXT_ALL,
                            wxLIST_STATE_SELECTED);
  }

  return text;
}

//----------------------------------------
wxString CDictionaryList::GetFieldFullNames(long item)
{
  if (item < 0)
  {
    return "";
  }

  CField* field = (CField*)(GetItemData(item));

  if (field == NULL)
  {
    return "";
  }

  return field->GetFullName().c_str();
}

//----------------------------------------
CField* CDictionaryList::FindFieldFullNames(const wxString fieldName)
{


  wxString text;

  int32_t itemCount = GetItemCount();

  for(int32_t item = 0 ; item < itemCount ; item++)
  {
    CField* field = (CField*)(GetItemData(item));
    if (field == NULL)
    {
      continue;
    }

    if (fieldName.CmpNoCase(field->GetFullName().c_str()) == 0)
    {
      return field;
    }

  }

  return NULL;
}
//----------------------------------------

void CDictionaryList::EvtListCtrl(wxWindow& window,  int32_t eventType, const wxListEventFunction& method,
                                wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(eventType,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}

//----------------------------------------
//-------------------------------------------------------------
//------------------- CDictionaryDisplayList class --------------------
//-------------------------------------------------------------

DEFINE_EVENT_TYPE(wxEVT_DELETE_DISPSELECTED_FIELD)


IMPLEMENT_CLASS(CDictionaryDisplayList,wxListView)

// WDR: event table for CDictionaryDisplayList

BEGIN_EVENT_TABLE(CDictionaryDisplayList,wxListView)
    //EVT_LIST_COL_CLICK( ID_DICTLIST, CDictionaryDisplayList::OnColClick )
    //EVT_LIST_ITEM_SELECTED( ID_DICTLIST, CDictionaryDisplayList::OnSelected )
    //EVT_LIST_ITEM_DESELECTED( ID_DICTLIST, CDictionaryDisplayList::OnDeselected )
    //EVT_LIST_INSERT_ITEM( ID_DISP_DATA_SEL, CDictionaryDisplayList::OnInsertItem )
    EVT_KEY_UP( CDictionaryDisplayList::OnKeyUp )
END_EVENT_TABLE()

//----------------------------------------
CDictionaryDisplayList::CDictionaryDisplayList( wxWindow *parent, wxWindowID id,
                                  const wxPoint &position, const wxSize& size, long style,
                                  const wxValidator& validator,
                                  const wxString &name)
      : wxListView( parent, id, position, size, style, validator, name)
{

  m_sortedCol = -1;
  m_sortAsc = true;
  m_data = NULL;
  m_display = NULL;

  m_allowSuppr = true;

  m_imageListSmall = new wxImageList(16, 16, true);
  m_imageListSmall->Add( BitmapsList(0) );
  m_imageListSmall->Add( BitmapsList(1) );

  SetImageList(m_imageListSmall, wxIMAGE_LIST_SMALL);
  CreateColumn();

  CDictionaryDisplayList::EvtListCtrl(*this, wxEVT_COMMAND_LIST_ITEM_SELECTED,
                               (wxListEventFunction)&CDictionaryDisplayList::OnSelected,
                               NULL,
                               this);
  CDictionaryDisplayList::EvtListCtrl(*this, wxEVT_COMMAND_LIST_ITEM_DESELECTED,
                               (wxListEventFunction)&CDictionaryDisplayList::OnDeselected,
                               NULL,
                               this);
  CDictionaryDisplayList::EvtListCtrl(*this, wxEVT_COMMAND_LIST_COL_CLICK,
                               (wxListEventFunction)&CDictionaryDisplayList::OnColClick,
                               NULL,
                               this);
  CDictionaryDisplayList::EvtListCtrl(*this, wxEVT_COMMAND_LIST_INSERT_ITEM,
                               (wxListEventFunction)&CDictionaryDisplayList::OnInsertItem,
                               NULL,
                               this);
  wxSize bestsize = GetBestSize();
  bestsize.x = 100;
  SetMinSize(bestsize);
  CacheBestSize(bestsize);
}

//----------------------------------------
void CDictionaryDisplayList::EvtDeleteDisplaySelFieldCommand(wxWindow& window, const CDeleteDisplaySelFieldEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(wxEVT_DELETE_DISPSELECTED_FIELD,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}


//----------------------------------------
CDictionaryDisplayList::~CDictionaryDisplayList()
{
  if (m_imageListSmall != NULL)
  {
    delete m_imageListSmall;
    m_imageListSmall = NULL;
  }
}
//----------------------------------------
void CDictionaryDisplayList::CreateColumn()
{

  wxListItem itemCol;
  itemCol.SetText(_T("Operation Name"));
  itemCol.SetWidth(140);

  //itemCol.SetImage(-1);
  //itemCol.SetMask(wxLIST_MASK_TEXT);

  itemCol.SetAlign(wxLIST_FORMAT_CENTER);
  this->InsertColumn(DICTDISPCOL_OPNAME, itemCol);

  itemCol.SetText(_T("File Name"));
  //don't use SetWidth(0), since this breaks on Mac OS X!
  itemCol.SetWidth(1);
  itemCol.SetAlign(wxLIST_FORMAT_CENTER);
  this->InsertColumn(DICTDISPCOL_FILENAME, itemCol);

  itemCol.SetText(_T("Expression Name"));
  itemCol.SetWidth(140);
  itemCol.SetAlign(wxLIST_FORMAT_CENTER);
  this->InsertColumn(DICTDISPCOL_FIELDNAME, itemCol);

  itemCol.SetText(_T("Unit"));
  itemCol.SetWidth(80);
  itemCol.SetAlign(wxLIST_FORMAT_CENTER);
  this->InsertColumn(DICTDISPCOL_UNIT, itemCol);

  itemCol.SetText(_T("1st dim. (unit)"));
  itemCol.SetWidth(100);
  itemCol.SetAlign(wxLIST_FORMAT_CENTER);
  this->InsertColumn(DICTDISPCOL_DIM1, itemCol);

  itemCol.SetText(_T("2nd dim. (unit)"));
  itemCol.SetWidth(100);
  itemCol.SetAlign(wxLIST_FORMAT_CENTER);
  this->InsertColumn(DICTDISPCOL_DIM2, itemCol);

  itemCol.SetText(_T("3rd dim. (unit)"));
  //itemCol.SetWidth(100);
  //don't use SetWidth(0), since this breaks on Mac OS X!
  itemCol.SetWidth(1);
  itemCol.SetAlign(wxLIST_FORMAT_CENTER);
  this->InsertColumn(DICTDISPCOL_DIM3, itemCol);

  itemCol.SetText(_T("Title"));
  itemCol.SetWidth(200);
  itemCol.SetAlign(wxLIST_FORMAT_CENTER);
  this->InsertColumn(DICTDISPCOL_TITLE, itemCol);

}
//----------------------------------------
void CDictionaryDisplayList::ResetColumnImage()
{
  for (int32_t i = 0 ; i < GetColumnCount() ; i ++)
  {
    SetColumnImage(i, -1);
  }

}
//----------------------------------------
void CDictionaryDisplayList::InsertDisplay(CDisplay* display)
{
  m_display = display;
  if (display == NULL)
  {
    InsertData(NULL);
  }
  else
  {
    InsertData(m_display->GetDataSelected());
  }
}
//----------------------------------------
void CDictionaryDisplayList::InsertData(CMapDisplayData* data)
{
  m_sortedCol = -1;
  m_sortAsc = true;
  bool showRow = false;
  ResetColumnImage();

  m_data = data;

  DeleteAllItems();

  if (m_data == NULL)
  {
    return;
  }

  CObMap::iterator it;

  for (it = m_data->begin(); it != m_data->end(); it++)
  {
    CDisplayData* displayData = dynamic_cast<CDisplayData*>(it->second);
    InsertField(displayData);
  }



/*

  int32_t index = GetItemCount();

  wxString buf;
  buf.Printf(_T("This is item %d"), index);
  long tmp = InsertItem(index, buf, -1);
  SetItemData(tmp, GetItemCount());

  buf.Printf(_T("Col 1, item %d"), index);
  SetItem(index, 1, buf);

  buf.Printf(_T("Item %d in column 2"), index);
  SetItem(index, 2, buf);
  */

}

//----------------------------------------
long CDictionaryDisplayList::InsertField(CDisplayData* displayData)
{

  int32_t index = GetItemCount();

  if (displayData->GetOperation() == NULL)
  {
    return -1;
  }

  COperation* operation = displayData->GetOperation();

  wxListItem itemRow;
  itemRow.SetText(operation->GetName());
  itemRow.SetMask(wxLIST_MASK_TEXT);
  itemRow.SetImage(-1);
  itemRow.SetData(displayData);
  itemRow.SetId(index);

  long indexInserted = InsertItem(itemRow);


  //long tmp = InsertItem(index, field->GetName().c_str(),  DICTCOL_NAME);

  SetItem(index, DICTDISPCOL_FILENAME, operation->GetOutputName());
  SetItem(index, DICTDISPCOL_FIELDNAME, displayData->GetField()->GetName().c_str());
  SetItem(index, DICTDISPCOL_TITLE, displayData->GetField()->GetDescription().c_str());
  SetItem(index, DICTDISPCOL_UNIT, displayData->GetField()->GetUnit().c_str());

  wxString label;

  if ( ! displayData->GetX()->GetName().empty() )
  {
    label.Printf("%s (%s)", displayData->GetX()->GetName().c_str(), displayData->GetX()->GetUnit().c_str());
  }

  SetItem(index, DICTDISPCOL_DIM1, label);

  label.Empty();

  if ( ! displayData->GetY()->GetName().empty() )
  {
    label.Printf("%s (%s)", displayData->GetY()->GetName().c_str(), displayData->GetY()->GetUnit().c_str());
  }

  SetItem(index, DICTDISPCOL_DIM2, label);

  label.Empty();

  if ( ! displayData->GetZ()->GetName().empty() )
  {
    label.Printf("%s (%s)", displayData->GetZ()->GetName().c_str(), displayData->GetZ()->GetUnit().c_str());
  }

  SetItem(index, DICTDISPCOL_DIM3, label);

  return indexInserted;

}
//----------------------------------------
void CDictionaryDisplayList::SetColumnImage(int col, int image)
{
#if !defined(__WXMAC__)
    wxListItem item;
    item.SetMask(wxLIST_MASK_IMAGE);
    item.SetImage(image);
    SetColumn(col, item);
#endif
}



// WDR: handler implementations for CDictionaryDisplayList
//----------------------------------------

void CDictionaryDisplayList::OnKeyUp( wxKeyEvent &event )
{
  long keycode = event.GetKeyCode();

  switch ( keycode )
  {
    case WXK_DELETE:
    case WXK_NUMPAD_DELETE:
    {
      if (!event.HasModifiers())
      {
        RemoveFields();
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
void CDictionaryDisplayList::RemoveFields()
{

  if (!m_allowSuppr)
  {
    return;
  }

  int32_t selCount = GetSelectedItemCount();

  CStringArray dataKeyToDelete;

  long item = GetNextItem(-1, wxLIST_NEXT_ALL,
                              wxLIST_STATE_SELECTED);
  while ( selCount != 0 )
  {
    CDisplayData* data = (CDisplayData*)(GetItemData(item));
    if (data != NULL)
    {
      DeleteItem(item);
      dataKeyToDelete.Insert((const char *)data->GetDataKey().c_str());
    }

    selCount = GetSelectedItemCount();

    item = GetNextItem(-1, wxLIST_NEXT_ALL,
                              wxLIST_STATE_SELECTED);

  }

  CStringArray::const_iterator it;

  if (m_display != NULL)
  {
    for (it = dataKeyToDelete.begin(); it != dataKeyToDelete.end() ; it++)
    {
      m_display->RemoveData(it->c_str());
    }
  }


  CDeleteDisplaySelFieldEvent ev(GetId());
  if (GetParent() != NULL)
  {
    wxPostEvent(GetParent(), ev);
  }



}
//----------------------------------------
void CDictionaryDisplayList::DeselectAll()
{


  int32_t selCount = GetSelectedItemCount();

  long item = GetNextItem(-1, wxLIST_NEXT_ALL,
                              wxLIST_STATE_SELECTED);
  while ( selCount != 0 )
  {
    Select(item, false);

    selCount = GetSelectedItemCount();

    item = GetNextItem(-1, wxLIST_NEXT_ALL,
                              wxLIST_STATE_SELECTED);

  }


}
//----------------------------------------

void CDictionaryDisplayList::OnInsertItem( wxListEvent &event )
{

  CDisplayData* displayData = (CDisplayData*)(event.GetData());

//  event.Veto();
}

//----------------------------------------
void CDictionaryDisplayList::OnDeselected( wxListEvent &event )
{
  event.Skip();
}

//----------------------------------------
void CDictionaryDisplayList::OnSelected( wxListEvent &event )
{
  event.Skip();
}

//----------------------------------------
void CDictionaryDisplayList::OnColClick( wxListEvent &event )
{
  int32_t col = event.GetColumn();

  if (m_sortedCol != col)
  {
    ResetColumnImage();
    m_sortedCol = col;
    m_sortAsc = true;
  }
  else
  {
    m_sortAsc = !m_sortAsc;
  }

  SetColumnImage(col, m_sortAsc ? 1 : 0);

  SortItems(DictDisplayListCompareFunction, (long)this);

  event.Skip();

}

//----------------------------------------
wxString CDictionaryDisplayList::GetSelectedFieldNames(const wxString& sep)
{
  wxString text;

  int32_t selCount = GetSelectedItemCount();

  wxString desc;
  long item = GetNextItem(-1, wxLIST_NEXT_ALL,
                              wxLIST_STATE_SELECTED);
  while ( item != -1 )
  {
    CField* field = (CField*)(GetItemData(item));
    text += wxString::Format("%s%s",
                             field->GetName().c_str(),
                             sep.c_str());
    item = GetNextItem(item, wxLIST_NEXT_ALL,
                            wxLIST_STATE_SELECTED);
  }

  return text;
}

//----------------------------------------
wxString CDictionaryDisplayList::GetSelectedFieldFullNames(const wxString& sep)
{
  wxString text;

  int32_t selCount = GetSelectedItemCount();

  wxString desc;
  long item = GetNextItem(-1, wxLIST_NEXT_ALL,
                              wxLIST_STATE_SELECTED);
  while ( item != -1 )
  {
    CField* field = (CField*)(GetItemData(item));
    text += wxString::Format("%s%s",
                             field->GetFullName().c_str(),
                             sep.c_str());
    item = GetNextItem(item, wxLIST_NEXT_ALL,
                            wxLIST_STATE_SELECTED);
  }

  return text;
}

//----------------------------------------

void CDictionaryDisplayList::EvtListCtrl(wxWindow& window,  int32_t eventType, const wxListEventFunction& method,
                                wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(eventType,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}

//----------------------------------------

