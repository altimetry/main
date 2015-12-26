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

#ifndef __DisplayPanel_H__
#define __DisplayPanel_H__



#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "BratGui_wdr.h"


#include "LabeledTextCtrl.h"

class CNewOperationEvent;
class CRenameOperationEvent;
class CDeleteOperationEvent;
class CDeleteDisplaySelFieldEvent;

#include "DisplayDataTreeCtrl.h"
#include "BratGui_wdr.h"
#include "Process.h"
#include "new-gui/brat/Workspaces/Display.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// CDisplayPanel
//----------------------------------------------------------------------------

class CDisplayPanel: public wxPanel
{
public:
  // constructors and destructors
  CDisplayPanel( wxWindow *parent, wxWindowID id = -1,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxTAB_TRAVERSAL | wxNO_BORDER );

  virtual ~CDisplayPanel();

  // WDR: method declarations for CDisplayPanel
  wxCheckBox* GetDispInvertXyaxes()  { return (wxCheckBox*) FindWindow( ID_DISP_INVERT_XYAXES ); }
  wxCheckBox* GetDispEastComponent()  { return (wxCheckBox*) FindWindow( ID_DISP_EAST_COMPONENT ); }
  wxCheckBox* GetDispNorthComponent()  { return (wxCheckBox*) FindWindow( ID_DISP_NORTH_COMPONENT ); }
  wxStaticText* GetDispXaxisTextLabel()  { return (wxStaticText*) FindWindow( ID_DISP_XAXIS_TEXT_LABEL ); }
  wxTextCtrl* GetDispXaxisText()  { return (wxTextCtrl*) FindWindow( ID_DISP_XAXIS_TEXT ); }
  wxStaticText* GetDispXaxisLabel()  { return (wxStaticText*) FindWindow( ID_DISP_XAXIS_LABEL ); }
  wxChoice* GetDispXaxis()  { return (wxChoice*) FindWindow( ID_DISP_XAXIS ); }
  wxStaticText* GetDispPaletteLabel()  { return (wxStaticText*) FindWindow( ID_DISP_PALETTE_LABEL ); }
  CLabeledTextCtrl* GetDispzoomLatSouth()  { return (CLabeledTextCtrl*) FindWindow( ID_DISPZOOM_LAT_SOUTH ); }
  CLabeledTextCtrl* GetDispzoomLonEast()  { return (CLabeledTextCtrl*) FindWindow( ID_DISPZOOM_LON_EAST ); }
  CLabeledTextCtrl* GetDispzoomLatNorth()  { return (CLabeledTextCtrl*) FindWindow( ID_DISPZOOM_LAT_NORTH ); }
  CLabeledTextCtrl* GetDispzoomLonWest()  { return (CLabeledTextCtrl*) FindWindow( ID_DISPZOOM_LON_WEST ); }
  CLabeledTextCtrl* GetDispMax()  { return (CLabeledTextCtrl*) FindWindow( ID_DISP_MAX ); }
  CLabeledTextCtrl* GetDispMin()  { return (CLabeledTextCtrl*) FindWindow( ID_DISP_MIN ); }
  CLabeledTextCtrl* GetDispYmax()  { return (CLabeledTextCtrl*) FindWindow( ID_DISP_YMAX ); }
  CLabeledTextCtrl* GetDispYmin()  { return (CLabeledTextCtrl*) FindWindow( ID_DISP_YMIN ); }
  CLabeledTextCtrl* GetDispXmax()  { return (CLabeledTextCtrl*) FindWindow( ID_DISP_XMAX ); }
  CLabeledTextCtrl* GetDispXmin()  { return (CLabeledTextCtrl*) FindWindow( ID_DISP_XMIN ); }
  wxButton* GetDispPalBrowse()  { return (wxButton*) FindWindow( ID_DISP_PAL_BROWSE ); }
  wxComboBox* GetDispPalette()  { return (wxComboBox*) FindWindow( ID_DISP_PALETTE ); }
  wxCheckBox* GetDispContour()  { return (wxCheckBox*) FindWindow( ID_DISP_CONTOUR ); }
  wxCheckBox* GetDispSolidColor()  { return (wxCheckBox*) FindWindow( ID_DISP_SOLID_COLOR ); }
  wxTextCtrl* GetDispFieldName()  { return (wxTextCtrl*) FindWindow( ID_DISP_FIELD_NAME ); }
  wxCheckBox* GetDispgroupfields()  { return (wxCheckBox*) FindWindow( ID_DISPGROUPFIELDS ); }
  wxButton* GetDispRemovefield()  { return (wxButton*) FindWindow( ID_DISP_REMOVEFIELD ); }
  wxButton* GetDispAddfield()  { return (wxButton*) FindWindow( ID_DISP_ADDFIELD ); }
  wxButton* GetDispRefresh()  { return (wxButton*) FindWindow( ID_DISP_REFRESH ); }
  wxComboBox* GetDispProj()  { return (wxComboBox*) FindWindow( ID_DISP_PROJ ); }
  wxCheckBox* GetDispWithAnimation()  { return (wxCheckBox*) FindWindow( ID_DISP_WITH_ANIMATION ); }
  wxTextCtrl* GetDispTitle()  { return (wxTextCtrl*) FindWindow( ID_DISP_TITLE ); }
  wxButton* GetDispexecute()  { return (wxButton*) FindWindow( ID_DISPEXECUTE ); }
  wxComboBox* GetDispNames()  { return (wxComboBox*) FindWindow( ID_DISP_NAMES ); }
  wxButton* GetDispdelete()  { return (wxButton*) FindWindow( ID_DISPDELETE ); }
  wxButton* GetDispnew()  { return (wxButton*) FindWindow( ID_DISPNEW ); }
  wxStaticText* GetDispType()  { return (wxStaticText*) FindWindow( ID_DISP_TYPE ); }
  CDictionaryDisplayList* GetDispDataSel()  { return (CDictionaryDisplayList*) FindWindow( ID_DISP_DATA_SEL ); }
  CDisplayDataTreeCtrl* GetDispavailtreectrl()  { return (CDisplayDataTreeCtrl*) FindWindow( ID_DISPAVAILTREECTRL ); }

  void LoadDisplay();
  void NewDisplay();
  void DeleteDisplay();

  bool HasDisplay() {return m_currentDisplay >= 0;};

  void EnableCtrl();
  void ClearAll();
  void Reset();

  void ClearAvailableDataList();
  void ClearSelectedDataList();

  bool RefreshSelectedData();

  void LoadAvailableData();
  void LoadSelectedData();
  void SelectAvailableFromSelectedData(long item);
  wxTreeItemId FindAvailableItemFromSelectedData(long item);

  void GetOperations(int32_t type = -1);

  void GetDisplayOutput();
  void InitDisplayOutput();

  bool AddField(const CObArray& data, bool preserveFieldProperties = false);
  bool AddField(CDisplayData* data, bool preserveFieldProperties = false);
  void AddField();

  void FillXAxis();

  void SetXAxisText();

  void SetInvertXYAxes();

  void SetEastComponent();
  void SetNorthComponent();

  void RemoveFields();

  bool Control(wxString& msg);
  bool ControlSolidColor(wxString& msg);
  bool ControlVectorComponents(wxString& msg);

  void Execute();

  bool IsYFXType();
  bool IsZYFXType();
  bool IsZLatLonType();


private:
    // WDR: member variable declarations for CDisplayPanel
  bool m_dispNameDirty;
  int32_t m_currentDisplay;
  CDisplay* m_display;
  CMapDisplayData m_dataList;
  bool m_paletteDirty;

  CDisplayData* m_currentData;

private:

  bool CheckSelectedFields();

  CDisplay* GetCurrentDisplay();
  bool SetCurrentDisplay();

  void TypeDisplayChanged( );

  void DispNameChanged();

  void SetPaletteName();
  void SetValueZoom();
  void SetValueMin();
  void SetValueMax();
  void SetValueFieldGroup();
  void SetContour();
  void SetSolidColor();

  void FillProjList();
  void FillPaletteList();

  void SetTitle();
  void SetFieldName();

  void ShowFieldProperties();
  void ClearFieldProperties();

  void ShowGeneralProperties();
  void ClearGeneralProperties();

  void EnableSpecific();
  void EnableSpecificXY();
  void EnableSpecificZXY();

  wxString GetDataKey(CDisplayData& d);

  CDisplayData* GetFirstSelectedField();

  bool BuildCmdFile();

  void GetType();
  void GetTitle();
  void XAxisChanged();

  // WDR: handler declarations for CDisplayPanel
    void OnInvertXYAxes( wxCommandEvent &event );
    void OnEastComponent( wxCommandEvent &event );
    void OnNorthComponent( wxCommandEvent &event );
  void OnXAxisTextEnter( wxCommandEvent &event );
  void OnXAxisTextKillFocus( wxFocusEvent &event );
  void OnXAxis( wxCommandEvent &event );
  void OnDispSelFieldDeleted(CDeleteDisplaySelFieldEvent& event);
  void OnFieldGroup( wxCommandEvent &event );
  void OnDataAvailableSelChanged( wxTreeEvent &event );
  void OnFieldNameTextEnter( wxCommandEvent &event );
  void OnFieldNameKillFocus( wxFocusEvent &event );
  void OnValueChangeXMin(CValueChangedEvent& event);
  void OnValueChangeXMax(CValueChangedEvent& event);
  void OnValueChangeYMin(CValueChangedEvent& event);
  void OnValueChangeYMax(CValueChangedEvent& event);
  void OnValueChangeMin(CValueChangedEvent& event);
  void OnValueChangeMax(CValueChangedEvent& event);
  void OnValueChangeZoom(CValueChangedEvent& event);
  void OnValueChangeFieldGroup(CValueChangedEvent& event);
  void OnNewOperation(CNewOperationEvent& event);
  void OnRenameOperation(CRenameOperationEvent& event);
  void OnDeleteOperation(CDeleteOperationEvent& event);
  void OnRefresh( wxCommandEvent &event );
  void OnComboPaletteTextEnter( wxCommandEvent &event );
  void OnComboPalette( wxCommandEvent &event );
  void OnBrowsePalette( wxCommandEvent &event );
  void OnContour( wxCommandEvent &event );
  void OnSolidColor( wxCommandEvent &event );
  void OnWithAnimation( wxCommandEvent &event );
  void OnTitleTextEnter( wxCommandEvent &event );
  void OnComboDisplayTextEnter( wxCommandEvent &event );
  void OnComboDisplay( wxCommandEvent &event );
  void OnDataSelDeselected( wxListEvent &event );
  void OnDataSelSelected( wxListEvent &event );
  void OnRemoveField( wxCommandEvent &event );
  void OnAddField( wxCommandEvent &event );
  void OnComboProj( wxCommandEvent &event );
  void OnExecute( wxCommandEvent &event );
  void OnDeleteView( wxCommandEvent &event );
  void OnNewView( wxCommandEvent &event );
  void OnComboDisplayChar(wxKeyEvent &event );
  void OnComboDisplayKillFocus( wxFocusEvent &event );
  void OnComboPaletteKillFocus( wxFocusEvent &event );
  void OnComboPaletteSetFocus( wxFocusEvent &event );
  void OnComboPaletteChar(wxKeyEvent &event );
  void OnTitleKillFocus( wxFocusEvent &event );
  void OnProcessTerm(CProcessTermEvent& event);

private:
    DECLARE_CLASS(CDisplayPanel)
    DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------------------
// CDisplayDataDropTarget
//----------------------------------------------------------------------------
class CDisplayDataDropTarget : public wxDropTarget
{
public:

  CDisplayDataDropTarget(CDisplayPanel* pOwner)
         : wxDropTarget(new CDndDisplayDataObject())
         {
          m_pOwner = pOwner;
         };

  virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult def);
  virtual wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult def);

private:
    CDisplayPanel* m_pOwner;
};



#endif
