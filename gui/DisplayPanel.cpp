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

#include "new-gui/Common/tools/Trace.h"
#include "Tools.h"
#include "new-gui/Common/tools/Exception.h"
#include "Expression.h"
#include "InternalFilesFactory.h"
#include "InternalFiles.h"
#include "InternalFilesYFX.h"
#include "InternalFilesZFXY.h"
using namespace brathl;

#include "ColorPalette.h"
#include "PlotData/MapProjection.h"
#include "Validators.h"

#include "BratGui.h"
#include "new-gui/brat/DataModels/Workspaces/Workspace.h"
#include "new-gui/brat/Display/MapTypeDisp.h"
#include "Process.h"

#include "DisplayPanel.h"
#include "PlotData/ColorPalleteNames.h"
#include "wxGuiInterface.h"


// WDR: class implementations

//----------------------------------------------------------------------------
// CDisplayPanel
//----------------------------------------------------------------------------
IMPLEMENT_CLASS(CDisplayPanel,wxPanel)

// WDR: event table for CDisplayPanel

BEGIN_EVENT_TABLE(CDisplayPanel,wxPanel)
    EVT_BUTTON( ID_DISPNEW, CDisplayPanel::OnNewView )
    EVT_BUTTON( ID_DISPDELETE, CDisplayPanel::OnDeleteView )
    EVT_BUTTON( ID_DISPEXECUTE, CDisplayPanel::OnExecute )
    EVT_COMBOBOX( ID_DISP_PROJ, CDisplayPanel::OnComboProj )
    EVT_BUTTON( ID_DISP_ADDFIELD, CDisplayPanel::OnAddField )
    EVT_BUTTON( ID_DISP_REMOVEFIELD, CDisplayPanel::OnRemoveField )
    EVT_LIST_ITEM_SELECTED( ID_DISP_DATA_SEL, CDisplayPanel::OnDataSelSelected )
    EVT_LIST_ITEM_DESELECTED( ID_DISP_DATA_SEL, CDisplayPanel::OnDataSelDeselected )
    EVT_COMBOBOX( ID_DISP_NAMES, CDisplayPanel::OnComboDisplay )
    EVT_TEXT_ENTER( ID_DISP_NAMES, CDisplayPanel::OnComboDisplayTextEnter )
    EVT_TEXT_ENTER( ID_DISP_TITLE, CDisplayPanel::OnTitleTextEnter )
    EVT_TEXT_ENTER( ID_DISP_FIELD_NAME, CDisplayPanel::OnFieldNameTextEnter )
    EVT_CHECKBOX( ID_DISP_WITH_ANIMATION, CDisplayPanel::OnWithAnimation )
    EVT_CHECKBOX( ID_DISP_SOLID_COLOR, CDisplayPanel::OnSolidColor )
    EVT_CHECKBOX( ID_DISP_CONTOUR, CDisplayPanel::OnContour )
    EVT_BUTTON( ID_DISP_PAL_BROWSE, CDisplayPanel::OnBrowsePalette )
    EVT_COMBOBOX( ID_DISP_PALETTE, CDisplayPanel::OnComboPalette )
    EVT_TEXT_ENTER( ID_DISP_PALETTE, CDisplayPanel::OnComboPaletteTextEnter )
    EVT_BUTTON( ID_DISP_REFRESH, CDisplayPanel::OnRefresh )
    EVT_TREE_SEL_CHANGED( ID_DISPAVAILTREECTRL, CDisplayPanel::OnDataAvailableSelChanged )
    EVT_CHECKBOX( ID_DISPGROUPFIELDS, CDisplayPanel::OnFieldGroup )
    EVT_CHOICE( ID_DISP_XAXIS, CDisplayPanel::OnXAxis )
    EVT_TEXT_ENTER( ID_DISP_XAXIS_TEXT, CDisplayPanel::OnXAxisTextEnter )
    EVT_CHECKBOX( ID_DISP_INVERT_XYAXES, CDisplayPanel::OnInvertXYAxes )
    EVT_CHECKBOX( ID_DISP_EAST_COMPONENT, CDisplayPanel::OnEastComponent )
    EVT_CHECKBOX( ID_DISP_NORTH_COMPONENT, CDisplayPanel::OnNorthComponent )
END_EVENT_TABLE()
//----------------------------------------

CDisplayPanel::CDisplayPanel( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxPanel( parent, id, position, size, style ),
    m_currentData(NULL)
{
    // WDR: dialog function DisplayInfoPanel2 for CDisplayPanel
  Reset();

  wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

  //----------------------------
  DisplayInfoPanel(this, false, true);
  //----------------------------

  item0->Add( this, 0, wxGROW|wxALL, 5 );

  parent->SetSizer( item0 );

  GetDispDataSel()->SetDropTarget(new CDisplayDataDropTarget(this));


  CTextLightValidator textLightValidator;

  GetDispNames()->SetValidator(textLightValidator);


  GetDispzoomLonWest()->SetToolTip("West longitude. At the first display, zoom to the latitude/longitude box.");
  GetDispzoomLonEast()->SetToolTip("East longitude. At the first display, zoom to the latitude/longitude box.");
  GetDispzoomLatNorth()->SetToolTip("North latitude. At the first display, zoom to the latitude/longitude box.");
  GetDispzoomLatSouth()->SetToolTip("South latitude. At the first display, zoom to the latitude/longitude box.");

  GetDispXmin()->SetToolTip("Minimum X coordinate value to use in XY plot.");
  GetDispXmax()->SetToolTip("Maximum X coordinate value to use in XY plot.");
  GetDispYmin()->SetToolTip("Minimum Y coordinate value to use in XY plot.");
  GetDispYmax()->SetToolTip("Maximum Y coordinate value to use in XY plot.");

  GetDispMin()->SetToolTip("Minimum value to use in World plot.");
  GetDispMax()->SetToolTip("Maximum value to use in World plot.");

  CGuiPanel::EvtNewOperationCommand(*this,
                                 (CNewOperationEventFunction)&CDisplayPanel::OnNewOperation, NULL, this);

  CGuiPanel::EvtRenameOperationCommand(*this,
                                 (CRenameOperationEventFunction)&CDisplayPanel::OnRenameOperation, NULL, this);

  CGuiPanel::EvtDeleteOperationCommand(*this,
                                 (CDeleteOperationEventFunction)&CDisplayPanel::OnDeleteOperation, NULL, this);

  CBratGuiApp::EvtChar(*GetDispNames(), wxEVT_CHAR,
                          (wxCharEventFunction)&CDisplayPanel::OnComboDisplayChar,
                          NULL,
                          this);
  CBratGuiApp::EvtFocus(*GetDispNames(), wxEVT_KILL_FOCUS,
                          (wxFocusEventFunction)&CDisplayPanel::OnComboDisplayKillFocus,
                          NULL,
                          this);

  CBratGuiApp::EvtFocus(*GetDispTitle(), wxEVT_KILL_FOCUS,
                          (wxFocusEventFunction)&CDisplayPanel::OnTitleKillFocus,
                          NULL,
                          this);

  CBratGuiApp::EvtFocus(*GetDispFieldName(), wxEVT_KILL_FOCUS,
                          (wxFocusEventFunction)&CDisplayPanel::OnFieldNameKillFocus,
                          NULL,
                          this);

  CBratGuiApp::EvtFocus(*GetDispXaxisText(), wxEVT_KILL_FOCUS,
                          (wxFocusEventFunction)&CDisplayPanel::OnXAxisTextKillFocus,
                          NULL,
                          this);

  CBratGuiApp::EvtFocus(*GetDispPalette(), wxEVT_SET_FOCUS,
                          (wxFocusEventFunction)&CDisplayPanel::OnComboPaletteSetFocus,
                          NULL,
                          this);

  CBratGuiApp::EvtFocus(*GetDispPalette(), wxEVT_KILL_FOCUS,
                          (wxFocusEventFunction)&CDisplayPanel::OnComboPaletteKillFocus,
                          NULL,
                          this);
  CBratGuiApp::EvtChar(*GetDispPalette(), wxEVT_CHAR,
                          (wxCharEventFunction)&CDisplayPanel::OnComboPaletteChar,
                          NULL,
                          this);

  CLabeledTextCtrl::EvtValueChanged(*this,
                                   ID_DISP_XMIN,
                                   (CValueChangedEventFunction)&CDisplayPanel::OnValueChangeXMin, NULL, this);

  CLabeledTextCtrl::EvtValueChanged(*this,
                                   ID_DISP_XMAX,
                                   (CValueChangedEventFunction)&CDisplayPanel::OnValueChangeXMax, NULL, this);

  CLabeledTextCtrl::EvtValueChanged(*this,
                                   ID_DISP_YMIN,
                                   (CValueChangedEventFunction)&CDisplayPanel::OnValueChangeYMin, NULL, this);

  CLabeledTextCtrl::EvtValueChanged(*this,
                                   ID_DISP_YMAX,
                                   (CValueChangedEventFunction)&CDisplayPanel::OnValueChangeYMax, NULL, this);

  CLabeledTextCtrl::EvtValueChanged(*this,
                                   ID_DISP_MIN,
                                   (CValueChangedEventFunction)&CDisplayPanel::OnValueChangeMin, NULL, this);

  CLabeledTextCtrl::EvtValueChanged(*this,
                                   ID_DISP_MAX,
                                   (CValueChangedEventFunction)&CDisplayPanel::OnValueChangeMax, NULL, this);

  CLabeledTextCtrl::EvtValueChanged(*this,
                                   ID_DISPZOOM_LAT_NORTH,
                                   (CValueChangedEventFunction)&CDisplayPanel::OnValueChangeZoom, NULL, this);
  CLabeledTextCtrl::EvtValueChanged(*this,
                                   ID_DISPZOOM_LAT_SOUTH,
                                   (CValueChangedEventFunction)&CDisplayPanel::OnValueChangeZoom, NULL, this);
  CLabeledTextCtrl::EvtValueChanged(*this,
                                   ID_DISPZOOM_LON_WEST,
                                   (CValueChangedEventFunction)&CDisplayPanel::OnValueChangeZoom, NULL, this);
  CLabeledTextCtrl::EvtValueChanged(*this,
                                   ID_DISPZOOM_LON_EAST,
                                   (CValueChangedEventFunction)&CDisplayPanel::OnValueChangeZoom, NULL, this);
  /*
  CLabeledTextCtrl::EvtValueChanged(*this,
                                   ID_DISP_FIELD_GROUP,
                                   (CValueChangedEventFunction)&CDisplayPanel::OnValueChangeFieldGroup, NULL, this);
 */
  CProcess::EvtProcessTermCommand(*this,
                                 (CProcessTermEventFunction)&CDisplayPanel::OnProcessTerm, NULL, this);

  CDictionaryDisplayList::EvtDeleteDisplaySelFieldCommand(*this,
                                 (CDeleteDisplaySelFieldEventFunction)&CDisplayPanel::OnDispSelFieldDeleted, NULL, this);

  // Disable one of the zoom panels, so the minimum size gets correctly calculated
  glbDisplayZoomXY->Show(false);
}

//----------------------------------------

CDisplayPanel::~CDisplayPanel()
{
}
//----------------------------------------
void CDisplayPanel::Reset()
{
  m_currentDisplay = -1;
  m_display = NULL;
  m_dispNameDirty = false;
  m_paletteDirty = false;
  m_currentData = NULL;
}
//----------------------------------------
void CDisplayPanel::DeleteDisplay()
{
  bool bOk = true;


  CWorkspaceDisplay* wks = wxGetApp().GetCurrentWorkspaceDisplay();
  if (wks == NULL)
  {
    return;
  }

  if (m_currentDisplay < 0)
  {
    return;
  }

  if (m_display == NULL)
  {
    return;
  }

  int32_t result = wxMessageBox(wxString::Format("Are you sure to delete display '%s' ?\nContinue ?",
                                                  m_display->GetName().c_str()),
                               "Warning",
                                wxYES_NO | wxCENTRE | wxICON_QUESTION);

  if (result != wxYES)
  {
    return;
  }

  bOk = wxGetApp().CanDeleteDisplay(m_display->GetName());

  if (bOk == false)
  {
    wxMessageBox(wxString::Format("Unable to delete display '%s'.\nIt is used at least in one view", m_display->GetName().c_str()),
                "Warning",
                wxOK | wxICON_EXCLAMATION);
    return;

  }

  wxString dispName = m_display->GetName();
  bOk = wks->DeleteDisplay(m_display);

  if (bOk == false)
  {
    wxMessageBox(wxString::Format("Unable to delete display '%s'", m_display->GetName().c_str()),
                "Warning",
                wxOK | wxICON_EXCLAMATION);
    return;
  }

  m_display = NULL;
  m_currentData = NULL;

  GetDispNames()->Delete(m_currentDisplay);

  m_currentDisplay = (GetDispNames()->GetCount() > 0) ? 0 : -1;

  GetDispNames()->SetSelection(m_currentDisplay);

  SetCurrentDisplay();

  LoadAvailableData();
  LoadSelectedData();

  ShowGeneralProperties();
  ShowFieldProperties();

  GetDisplayOutput();


  EnableCtrl();
}

//----------------------------------------
void CDisplayPanel::NewDisplay()
{
  bool bOk = true;

  CWorkspaceDisplay* wks = wxGetApp().GetCurrentWorkspaceDisplay();
  if (wks == NULL)
  {
    return;
  }
  FillProjList();
  FillPaletteList();

  GetDispNames()->Enable(true);

  wxString dispName = wks->GetDisplayNewName();

  bOk = wks->InsertDisplay(dispName.ToStdString());

  if (bOk == false)
  {
    wxMessageBox(wxString::Format("Display '%s' already exists", dispName.c_str()),
                "Warning",
                wxOK | wxICON_EXCLAMATION);

    GetDispNames()->SetStringSelection(dispName);
    m_currentDisplay = GetDispNames()->GetSelection();
  }
  else
  {
    m_currentDisplay = GetDispNames()->Append(dispName);
    GetDispNames()->SetSelection(m_currentDisplay);
  }

  SetCurrentDisplay();

  InitDisplayOutput();
  GetDisplayOutput();

  GetDispTitle()->Clear();

  ClearSelectedDataList();

  ShowGeneralProperties();
  ShowFieldProperties();

  LoadAvailableData();

  GetDispDataSel()->SetDisplay(m_display);

  EnableCtrl();

}
//----------------------------------------
void CDisplayPanel::LoadDisplay()
{

  ClearAll();

  FillProjList();
  FillPaletteList();

  CWorkspaceDisplay* wks = wxGetApp().GetCurrentWorkspaceDisplay();
  if (wks == NULL)
  {
    return;
  }

  if (wks->GetDisplayCount() <= 0)
  {
    return;
  }

  this->SetCursor(*wxHOURGLASS_CURSOR);

  GetDisplayNames( *wks, *GetDispNames() );

  m_currentDisplay = 0;
  GetDispNames()->SetSelection(m_currentDisplay);

  SetCurrentDisplay();


  ShowGeneralProperties();
  ShowFieldProperties();


  GetDisplayOutput();

  LoadAvailableData();
  LoadSelectedData();

  RefreshSelectedData();

  EnableCtrl();

  this->SetCursor(wxNullCursor);

}

//----------------------------------------
void CDisplayPanel::EnableCtrl()
{
  bool hasDisplay = false;
  bool hasOperation = false;
  bool enable = false;
  bool enableFieldSelected  = false;
  bool enableAddField  = false;
  bool enableData = false;
  bool enableExecute = false;
  bool enableGroupFields = false;

  //bool enableAsY = false;

  CWorkspaceDisplay* wksDisp = wxGetApp().GetCurrentWorkspaceDisplay();
  if (wksDisp != NULL)
  {
    hasDisplay = wksDisp->HasDisplay();
  }

  CWorkspaceOperation* wksOp = wxGetApp().GetCurrentWorkspaceOperation();
  if (wksOp != NULL)
  {
    hasOperation = wksOp->HasOperation();
  }

  enable = hasDisplay && hasOperation;

  enableAddField = enable && (GetDispavailtreectrl()->GetSelection().IsOk());
  enableFieldSelected = enable && (GetDispDataSel()->GetSelectedItemCount() > 0);

  enableData = (m_dataList.size() > 0);

  enableExecute = enable && hasDisplay && (GetDispDataSel()->GetItemCount() > 0);
  enableGroupFields = enable && hasDisplay && (GetDispDataSel()->GetItemCount() > 1);

  GetDispNames()->Enable(enable);
  GetDispTitle()->Enable(enable);
  GetDispType()->Enable(enable);
  GetDispgroupfields()->Enable(enableGroupFields);

  GetDispavailtreectrl()->Enable(enableData);
  GetDispRefresh()->Enable(enableData);
  GetDispDataSel()->Enable(enableData);

  GetDispAddfield()->Enable(enableAddField);
  GetDispRemovefield()->Enable(enableFieldSelected);
  GetDispFieldName()->Enable(enableFieldSelected);

  GetDispexecute()->Enable(enableExecute);

  GetDispnew()->Enable( wxGetApp().IsNewViewEnable() );
  GetDispdelete()->Enable( wxGetApp().IsDeleteViewEnable() );


  EnableSpecific();

  //glbDisplayZoomSizer->Fit(this);
  glbDisplayZoomSizer->Layout();
  //glbDisplayPanelSizer->Fit(this);
  //glbDisplayGeneralPropsSizer->SetSizeHints(this);
  //glbDisplayPanelSizer->SetSizeHints(this);

  glbDisplayFieldProps1->Layout();
  /*
  glbDisplayFieldPropsZFXY1->Layout();
  glbDisplayFieldPropsZFXY2->Layout();
  glbDisplayFieldPropsYFX1->Layout();
  glbDisplayFieldPropsAll->Layout();
  glbDisplayFieldPropsAll2->Layout();
  */
  glbDisplaySelected->Layout();
  this->Layout();
}

//----------------------------------------
void CDisplayPanel::EnableSpecific()
{
  glbDisplayZoomGeo->Show(false);
  glbDisplayZoomXY->Show(false);

  glbDisplayFieldPropsZFXY1->Show(false);
  glbDisplayFieldPropsZFXY2->Show(false);
  glbDisplayFieldPropsYFX1->Show(false);

  GetDispXmin()->Enable(false);
  GetDispXmax()->Enable(false);
  GetDispYmin()->Enable(false);
  GetDispYmax()->Enable(false);

  GetDispzoomLatNorth()->Enable(false);
  GetDispzoomLatSouth()->Enable(false);
  GetDispzoomLonEast()->Enable(false);
  GetDispzoomLonWest()->Enable(false);

  GetDispWithAnimation()->Enable(false);
  GetDispMin()->Enable(false);
  GetDispMax()->Enable(false);
  GetDispProj()->Enable(false);
  GetDispSolidColor()->Enable(false);
  GetDispContour()->Enable(false);
  GetDispPalette()->Enable(false);
  GetDispPaletteLabel()->Enable(false);
  GetDispPalBrowse()->Enable(false);

  GetDispXaxis()->Enable(false);
  GetDispXaxisLabel()->Enable(false);
  GetDispXaxisText()->Enable(false);
  GetDispXaxisTextLabel()->Enable(false);


  GetDispInvertXyaxes()->Enable(false);


  EnableSpecificXY();
  EnableSpecificZXY();

}

//----------------------------------------
void CDisplayPanel::EnableSpecificXY()
{
  if (m_display == NULL)
  {
    return;
  }

  bool enable = (m_display->GetType() == CMapTypeDisp::eTypeDispYFX);
  bool enableZoomYFX = ((m_display->GetType() == CMapTypeDisp::eTypeDispZFXY) || (m_display->GetType() == CMapTypeDisp::eTypeDispYFX));
  //bool enableFieldSelected = enable && (GetDispDataSel()->GetSelectedItemCount() > 0);

  GetDispXmin()->Enable(enableZoomYFX);
  GetDispXmax()->Enable(enableZoomYFX);
  GetDispYmin()->Enable(enableZoomYFX);
  GetDispYmax()->Enable(enableZoomYFX);

  glbDisplayZoomGeo->Show(!enableZoomYFX);


  glbDisplayZoomXY->Show(enableZoomYFX);

  glbDisplayFieldPropsZFXY1->Show(!enable);
  glbDisplayFieldPropsZFXY2->Show(!enable);
  glbDisplayFieldPropsYFX1->Show(enable);

  //GetDispFieldGroup()->Enable(enableFieldSelected);

  bool hasXComplement = (m_currentData != NULL) ? m_currentData->HasXComplement() : false;

  bool enableXAxis = enable && (GetDispDataSel()->GetSelectedItemCount() > 0) && hasXComplement;
  GetDispXaxis()->Enable(enableXAxis);
  GetDispXaxisLabel()->Enable(enableXAxis);
  GetDispXaxisText()->Enable(enableXAxis);
  GetDispXaxisTextLabel()->Enable(enableXAxis);



}
//----------------------------------------
void CDisplayPanel::EnableSpecificZXY()
{
  if (m_display == NULL)
  {
    return;
  }

  bool enable = ((m_display->GetType() == CMapTypeDisp::eTypeDispZFXY) || (m_display->GetType() == CMapTypeDisp::eTypeDispZFLatLon));
  bool enableZFLatLon =  (m_display->GetType() == CMapTypeDisp::eTypeDispZFLatLon);
  bool enableField = enable && (GetDispDataSel()->GetSelectedItemCount() > 0);

  GetDispWithAnimation()->Enable(enable);
  GetDispMin()->Enable(enableField);
  GetDispMax()->Enable(enableField);
  GetDispProj()->Enable(enableZFLatLon);
  GetDispSolidColor()->Enable(enableField);
  GetDispContour()->Enable(enableField);
  GetDispPalette()->Enable(enableField);
  GetDispPaletteLabel()->Enable(enableField);
  GetDispPalBrowse()->Enable(enableField);

  GetDispzoomLatNorth()->Enable(enable);
  GetDispzoomLatSouth()->Enable(enable);
  GetDispzoomLonEast()->Enable(enable);
  GetDispzoomLonWest()->Enable(enable);

  GetDispInvertXyaxes()->Enable(enable && (! enableZFLatLon));

  glbDisplayZoomGeo->Show(enableZFLatLon);
  glbDisplayZoomXY->Show(!enableZFLatLon);

  glbDisplayFieldPropsZFXY1->Show(enable);
  glbDisplayFieldPropsZFXY2->Show(enable);
  glbDisplayFieldPropsYFX1->Show(!enable);


}

//----------------------------------------
void CDisplayPanel::FillXAxis()
{
	GetDispXaxis()->Clear();

	if ( m_currentData == NULL )
		return;

	bool fill = ( m_display->GetType() == CMapTypeDisp::eTypeDispYFX );
	if ( ! fill )
	{
		return;
	}

	CStringArray array;
	m_currentData->GetAvailableAxes( array );
	wxArrayString axes_names;
	CStringArrayToWxArray( array, axes_names );
	GetDispXaxis()->Append( axes_names );

	wxString xAxis = m_currentData->GetXAxis();

	if ( xAxis.IsEmpty() )
	{
		CStringArray complement;
		m_currentData->GetXComplement( complement );
	}


	if ( xAxis.IsEmpty() )
	{
		GetDispXaxis()->SetSelection( -1 );
	}
	else
	{
		GetDispXaxis()->SetStringSelection( xAxis.c_str() );
	}
	XAxisChanged();
}

//----------------------------------------
void CDisplayPanel::ClearAvailableDataList()
{
  m_dataList.RemoveAll();
  GetDispavailtreectrl()->InsertData(NULL);
}
//----------------------------------------
bool CDisplayPanel::RefreshSelectedData()
{

  if (m_display == NULL)
  {
    return false;
  }

  bool forceThaw = false;

  if (!IsFrozen())
  {
    Freeze();
    forceThaw = true;
  }

  CMapDisplayData* availableData = GetDispavailtreectrl()->GetMapDisplayData();
  CMapDisplayData* selectedData =  m_display->GetDataSelected();

  CObArray dispSelNotFound;

  CObArray dispSelToRefresh;

  int32_t oldDisplayType = m_display->GetType();
  int32_t newDisplayType = -1;

  CUIntArray displayTypes;

  const CDisplayData* firstNewSelData = NULL;

  CMapDisplayData::const_iterator itSel;
  for (itSel = selectedData->begin() ; itSel != selectedData->end() ; itSel ++)
  {
    CDisplayData* dataSel = selectedData->GetDisplayData(itSel);
    if (dataSel == NULL)
    {
      continue;
    }

    const CDisplayData* dataAvailable = NULL;

    if (firstNewSelData == NULL)
    {
      dataAvailable = availableData->GetDisplayData(dataSel->GetDataKey());

      if (dataAvailable == NULL)
      {
        dataSel->GetAvailableDisplayTypes(displayTypes);

        CUIntArray::iterator itDispType;

        for (itDispType = displayTypes.begin(); itDispType != displayTypes.end(); itDispType++)
        {
          dataAvailable = availableData->GetDisplayData(dataSel->GetDataKey(*itDispType));
          if (dataAvailable != NULL)
          {
            break;
          }
        }
      }
    }
    else
    {
      dataAvailable = availableData->GetDisplayData(dataSel->GetDataKey(firstNewSelData->GetType()));
    }

	CWorkspaceOperation *wkso = wxGetApp().GetCurrentWorkspaceOperation();
    if (dataAvailable == NULL)
    {
      dispSelNotFound.Insert( new CDisplayData( *dataSel, wkso ));
      continue;
    }

    if (firstNewSelData == NULL)
    {
      firstNewSelData = dataAvailable;
      newDisplayType = dataAvailable->GetType();
    }

    CDisplayData* newDisplayData = new CDisplayData( *dataAvailable, wkso );
    newDisplayData->CopyFieldUserProperties(*dataSel);

    dispSelToRefresh.Insert(newDisplayData);
  }

  GetDispDataSel()->DeselectAll();
  m_currentData = NULL;

  wxString msg;
  msg.Append(wxString::Format("Warnings about view '%s':\n\n", m_display->GetName().c_str()));

  CObArray::const_iterator it;

  bool bOk = true;

  for (it = dispSelNotFound.begin() ; it != dispSelNotFound.end() ; it ++)
  {
    CDisplayData* data = dynamic_cast<CDisplayData*>(*it);

    if (newDisplayType >= 0)
    {
      data->SetType(newDisplayType);
    }
    else
    {
      data->SetType(oldDisplayType);
    }

    bOk = false;

    msg.Append(wxString::Format("In operation '%s', field's name '%s' doesn't exist anymore.\n",
                                      data->GetOperation()->GetName().c_str(),
                                      data->GetField()->GetName().c_str()
                                      ));
  }


  selectedData->RemoveAll();
  GetDispDataSel()->InsertDisplay(NULL);
  GetDispDataSel()->SetDisplay(m_display);

  if (newDisplayType >= 0)
  {
    m_display->SetType(newDisplayType);
  }

  bOk &= AddField(dispSelToRefresh, true);
  bOk &= AddField(dispSelNotFound, true);

  //GetDispDataSel()->InsertDisplay(m_display);

  if (dispSelToRefresh.size() <= 0)
  {
    msg.Append("\nAll the expressions to display don't exist anymore.\n");

  }

  if ( (newDisplayType > 0) && (oldDisplayType != newDisplayType) )
  {
    bOk = false;
    msg.Append(wxString::Format("\nView type has changed from '%s' to '%s'\n\n",
                                CMapTypeDisp::GetInstance().IdToName(oldDisplayType).c_str(),
                                CMapTypeDisp::GetInstance().IdToName(newDisplayType).c_str()
                                ));
  }


  ShowFieldProperties();

  EnableCtrl();

  if (!bOk)
  {
    if (IsFrozen())
    {
      Thaw();
    }

    wxMessageBox(msg, "Warning", wxOK | wxICON_EXCLAMATION);
  }

  if (forceThaw)
  {
    if (IsFrozen())
    {
      Thaw();
    }
  }

  return bOk;

}
//----------------------------------------
void CDisplayPanel::LoadAvailableData()
{
  ClearAvailableDataList();

  if (m_display == NULL)
  {
    return;
  }


  GetOperations();

  GetDispavailtreectrl()->InsertData(&m_dataList);

  EnableCtrl();


}

//----------------------------------------
void CDisplayPanel::GetOperations(int32_t type /*= -1*/)
{
    UNUSED(type);

  m_dataList.RemoveAll();

  CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();
  if (wks == NULL)
  {
    return;
  }

  CObMap* operations = wks->GetOperations();
  if (operations == NULL)
  {
    return;
  }


  CInternalFiles *file = NULL;
  /*
  CInternalFilesYFX *yfx = NULL;
  CInternalFilesZFXY *zfxy = NULL;
  */
  CUIntArray displayTypes;

  CStringArray names;

  CObMap::iterator it;

  for (it = operations->begin() ; it != operations->end() ; it++)
  {

    COperation* operation = dynamic_cast<COperation*>(it->second);
    if (operation == NULL)
    {
      continue;
    }

    /*

    yfx = NULL;
    zfxy = NULL;

    COperation* operation = dynamic_cast<COperation*>(it->second);
    if (operation == NULL)
    {
      continue;
    }

    if (type >= 0)
    {
      if (operation->GetType() != type)
      {
        continue;
      }
    }

    try
    {
      file = BuildExistingInternalFileKind(operation->GetOutputName().c_str());
      file->Open(ReadOnly);

    }
    catch (CException& e)
    {
      e.what();
      if (file != NULL)
      {
        delete file;
        file = NULL;
      }

      continue;
    }

    switch (operation->GetType())
    {
      case CMapTypeOp::typeOpYFX :
        yfx = dynamic_cast<CInternalFilesYFX*>(file);
        break;
      case CMapTypeOp::typeOpZFXY :
        zfxy = dynamic_cast<CInternalFilesZFXY*>(file);
        break;
      default  :
        break;
    }

    if ( (yfx == NULL) && (zfxy == NULL) )
    {
      continue;
    }
    */

    displayTypes.RemoveAll();

    CDisplay::GetDisplayType(operation, displayTypes, &file);

    if (file == NULL)
    {
      continue;
    }

    names.RemoveAll();

    file->GetDataVars(names);

    CUIntArray::iterator itDispType;
    CStringArray::iterator itField;
    CDisplayData* displayData;

    for (itDispType = displayTypes.begin(); itDispType != displayTypes.end(); itDispType++)
    {
      for (itField = names.begin(); itField != names.end(); itField++)
      {
        CStringArray varDimensions;
        file->GetVarDims(*itField,varDimensions);

        uint32_t nbDims = varDimensions.size();

        if (nbDims > 2)
        {
          continue;
        }

        if ( (nbDims != 2) && (    (*itDispType == CMapTypeDisp::eTypeDispZFXY)
                                || (*itDispType == CMapTypeDisp::eTypeDispZFLatLon) ) )
        {
          continue;
        }

        displayData = new CDisplayData(operation);

        displayData->SetType(*itDispType);

        displayData->GetField()->SetName(*itField);

        std::string unit = file->GetUnit(*itField).GetText();
        displayData->GetField()->SetUnit(unit);

        wxString comment = file->GetComment(*itField).c_str();

        wxString description = file->GetTitle(*itField).c_str();

        if (!comment.IsEmpty())
        {
          description += "." + comment;
        }

        displayData->GetField()->SetDescription((const char *)description.c_str());

        if (nbDims >= 1)
        {
          std::string dimName = varDimensions.at(0);
          displayData->GetX()->SetName(varDimensions.at(0));

          std::string unit = file->GetUnit(dimName).GetText();
          displayData->GetX()->SetUnit(unit);

          displayData->GetX()->SetDescription(file->GetTitle(dimName));
        }

        if (nbDims >= 2)
        {
          std::string dimName = varDimensions.at(1);
          displayData->GetY()->SetName(varDimensions.at(1));

          std::string unit = file->GetUnit(dimName).GetText();
          displayData->GetY()->SetUnit(unit);

          displayData->GetY()->SetDescription(file->GetTitle(dimName));
        }

        if (nbDims >= 3)
        {
          std::string dimName = varDimensions.at(2);
          displayData->GetZ()->SetName(varDimensions.at(2));

          std::string unit = file->GetUnit(dimName).GetText();
          displayData->GetZ()->SetUnit(unit);

          displayData->GetZ()->SetDescription(file->GetTitle(dimName));
        }

        m_dataList.Insert((const char *)GetDataKey(*displayData).c_str(), displayData, false);
      }

    }

    delete file;
    file = NULL;
  }


  delete file;
  file = NULL;



}
//----------------------------------------
bool CDisplayPanel::Control(wxString& msg)
{
  bool bOk = ControlSolidColor(msg);
  bOk &= ControlVectorComponents(msg);

  return bOk;
}
//----------------------------------------
bool CDisplayPanel::ControlSolidColor(wxString& msg)
{
    UNUSED(msg);

  if (m_display == NULL)
  {
    return false;
  }

  CMapDisplayData* selectedData =  m_display->GetDataSelected();


  CMapDisplayData::const_iterator itSel;
  for (itSel = selectedData->begin() ; itSel != selectedData->end() ; itSel ++)
  {
    CDisplayData* dataSel = selectedData->GetDisplayData(itSel);
    if (dataSel == NULL)
    {
      continue;
    }

    if (( ! dataSel->IsSolidColor() ) && ( ! dataSel->IsContour() ))
    {
      dataSel->SetSolidColor(true);
    }
  }

  return true;

}

bool CDisplayPanel::ControlVectorComponents(wxString& msg)
{
  bool xcomponent = false;
  bool ycomponent = false;

  if (m_display == NULL)
  {
    return false;
  }

  CMapDisplayData* selectedData =  m_display->GetDataSelected();


  CMapDisplayData::const_iterator itSel;
  for (itSel = selectedData->begin() ; itSel != selectedData->end() ; itSel ++)
  {
    CDisplayData* dataSel = selectedData->GetDisplayData(itSel);
    if (dataSel == NULL)
    {
      continue;
    }

    if ( dataSel->IsEastComponent() ) {
        if ( xcomponent ) {
            msg.Printf("%s",_("More than one field selected as East Component!"));
            return false;
        }
        xcomponent = true;
    }

    if ( dataSel->IsNorthComponent() ) {
        if ( ycomponent ) {
            msg.Printf("%s",_("More than one field selected as North Component!"));
            return false;
        }
        ycomponent = true;
    }
  }

  if ( xcomponent == ycomponent )
    return true;
  else {
    msg.Printf("%s",_("Both East and North fields must be selected for a Vector Plot!"));
    return false;
  }
}


//static 
std::string CDisplayPanel::m_execDisplayName;


std::string CDisplayPanel::GetFullCmd()
{
	std::string cmd;
#ifdef  USE_SYNC_OPTION
	cmd = "\"" + GetExecDisplayName() + "\" --sync \"" + m_display->GetCmdFilePath() + "\"";
#else
	cmd = "\"" + GetExecDisplayName() + "\" \"" + m_display->GetCmdFilePath() + "\"";
#endif
	return cmd;
}

//----------------------------------------
void CDisplayPanel::Execute()
{
  if (m_display == NULL)
  {
    return;
  }

  wxString msg;

  bool bOk = Control(msg);

  if (!bOk)
  {
    wxMessageBox(msg, "Warning", wxOK | wxICON_EXCLAMATION);
    return;
  }

  bOk = RefreshSelectedData();

  if (!bOk)
  {
    return;
  }

  BuildCmdFile();

  wxGetApp().GotoLogPage();


  CProcess* process = new CProcess(m_display->GetTaskName(),
                                             wxGetApp().GetLogPanel(),
                                             GetFullCmd(),
                                             //wxGetApp().GetLogPanel()->GetLogMess(),
                                             NULL);

  bOk = wxGetApp().GetLogPanel()->AddProcess(process, true);

  if (bOk == false)
  {
    delete process;
    process = NULL;
  }

}

//----------------------------------------
wxString CDisplayPanel::GetDataKey(CDisplayData& d)
{
  return d.GetDataKey();
}
//----------------------------------------
bool CDisplayPanel::AddField(const CObArray& data, bool preserveFieldProperties /* = false */)
{
  bool bOk = true;

  CObArray::const_iterator it;
  for (it = data.begin() ; it != data.end() ; it++)
  {
    CDisplayData* data = dynamic_cast<CDisplayData*>(*it);
    if (data == NULL)
    {
      continue;
    }
    bOk &= AddField(data, preserveFieldProperties);
  }

  return bOk;
}
//----------------------------------------
bool CDisplayPanel::AddField(CDisplayData* data, bool preserveFieldProperties /* = false */)
{
  if (data == NULL)
  {
    return false;
  }
  if (m_display == NULL)
  {
    return false;
  }

  int32_t typeDisplay = m_display->GetType();
  int32_t typeDataToAdd = data->GetType();

  if (typeDisplay >= 0)
  {
    if (typeDataToAdd != typeDisplay)
    {
      wxMessageBox(wxString::Format("The actual view type is '%s'. You can't add a '%s' data type,",
                                    CMapTypeDisp::GetInstance().IdToName(typeDisplay).c_str(),
                                    CMapTypeDisp::GetInstance().IdToName(typeDataToAdd).c_str()),
                  "Warning",
                  wxOK | wxICON_HAND);
      return false;
    }
  }

  bool insert = false;
  std::string dataKey;

  dataKey = data->GetDataKey();


  if (m_display->ExistData(dataKey) == true)
  {
    EnableCtrl();
    return true;
  }


  CDisplayData* newdata = new CDisplayData( *data, wxGetApp().GetCurrentWorkspaceOperation() );

  if (! preserveFieldProperties)
  {
    if (wxGetApp().GetLastColorTable().IsEmpty() == false)
    {
      newdata->SetColorPalette(wxGetApp().GetLastColorTable().ToStdString());
    }
    else
    {
      newdata->SetColorPalette(PALETTE_AEROSOL.c_str());
    }
  }

  insert = m_display->InsertData(GetDataKey(*data).ToStdString(), newdata);

  long indexInserted = GetDispDataSel()->InsertField(newdata);

  GetDispType()->SetLabel(CMapTypeDisp::GetInstance().IdToName(m_display->GetType()).c_str());

  CheckSelectedFields();

  SetValueFieldGroup();

  GetDispDataSel()->DeselectAll();
  GetDispDataSel()->Select(indexInserted);

  //FillXAxis();

  EnableCtrl();

  return true;
}

//----------------------------------------
bool CDisplayPanel::IsYFXType()
{
  if (m_display == NULL)
  {
    return false;
  }

  return m_display->IsYFXType();
}
//----------------------------------------
bool CDisplayPanel::IsZYFXType()
{
  if (m_display == NULL)
  {
    return false;
  }

  return m_display->IsZYFXType();
}
//----------------------------------------
bool CDisplayPanel::IsZLatLonType()
{
  if (m_display == NULL)
  {
    return false;
  }

  return m_display->IsZLatLonType();
}

//----------------------------------------
bool CDisplayPanel::CheckSelectedFields()
{
  if (m_display == NULL)
  {
    return false;
  }

  bool isGrouped = GetDispgroupfields()->GetValue();

  bool bOk = true;

  if (!isGrouped)
  {
    return bOk;
  }

  std::string errorMsg;
  bOk = m_display->GetDataSelected()->CheckFields(errorMsg, m_display);

  if ( ! bOk )
  {
    GetDispgroupfields()->SetValue(false);
    SetValueFieldGroup();

    wxString msg = wxString::Format("View '%s':\n\nData expressions can't be displayed on the same plot because of the following reasons:\n"
                                    "%s\n\n"
                                    "'%s' option have been unchecked.",
                                    m_display->GetName().c_str(),
                                    errorMsg.c_str(),
                                    GetDispgroupfields()->GetLabel().c_str());
    /*
    wxString msg = wxString::Format("Data fields can't be displayed on the same plot because of\nat least one of the following reasons:\n"
                                    " - number of dimensions is different\n"
                                    " - names of the dimensions are different\n"
                                    " - units of the dimensions are not in the same way (not compatible)\n"
                                    " - (only Y=F(X)) units of the fields are not in the same way (not compatible)\n\n"
                                    "'%s' option have been unchecked.",
                   GetDispgroupfields()->GetLabel().c_str());
    */
    wxMessageBox(msg,
                "Warning",
                wxOK | wxICON_EXCLAMATION);

  }

  return bOk;
}
//----------------------------------------
CDisplay* CDisplayPanel::GetCurrentDisplay()
{
  CWorkspaceDisplay* wks = wxGetApp().GetCurrentWorkspaceDisplay();
  if (wks == NULL)
  {
    return NULL;
  }

  return wks->GetDisplay(GetDispNames()->GetString(m_currentDisplay).ToStdString());
}
//----------------------------------------
bool CDisplayPanel::SetCurrentDisplay()
{
  m_display = GetCurrentDisplay();
  return (m_display!= NULL);
}
//----------------------------------------
void CDisplayPanel::ShowGeneralProperties()
{
  ClearGeneralProperties();

  if (m_display == NULL)
  {
    return;
  }

  GetDispType()->SetLabel(CMapTypeDisp::GetInstance().IdToName(m_display->GetType()).c_str());

  GetDispTitle()->SetValue(m_display->GetTitle());

  if (isDefaultValue(m_display->GetMinXValue()) == false)
  {
    GetDispXmin()->SetValue(m_display->GetMinXValue());
  }
  if (isDefaultValue(m_display->GetMaxXValue()) == false)
  {
    GetDispXmax()->SetValue(m_display->GetMaxXValue());
  }

  if (isDefaultValue(m_display->GetMinYValue()) == false)
  {
    GetDispYmin()->SetValue(m_display->GetMinYValue());
  }
  if (isDefaultValue(m_display->GetMaxYValue()) == false)
  {
    GetDispYmax()->SetValue(m_display->GetMaxYValue());
  }


  GetDispWithAnimation()->SetValue(m_display->GetWithAnimation());

  GetDispProj()->SetStringSelection(m_display->GetProjection());

  GetDispgroupfields()->SetValue(m_display->AreFieldsGrouped());


  CLatLonRect* latLonRect = m_display->GetZoom();

  GetDispzoomLatSouth()->SetValue(latLonRect->GetLowerLeftPoint().GetLatitude());
  GetDispzoomLonWest()->SetValue(latLonRect->GetLowerLeftPoint().GetLongitude());

  GetDispzoomLatNorth()->SetValue(latLonRect->GetUpperRightPoint().GetLatitude());
  GetDispzoomLonEast()->SetValue(latLonRect->GetUpperRightPoint().GetLongitude());


}
//----------------------------------------
void CDisplayPanel::ClearGeneralProperties()
{
  GetDispTitle()->Clear();
  GetDispXmin()->Clear();
  GetDispXmax()->Clear();
  GetDispYmin()->Clear();
  GetDispYmax()->Clear();

  GetDispWithAnimation()->SetValue(false);

  GetDispProj()->SetSelection(-1);

  EnableCtrl();

}
//----------------------------------------
void CDisplayPanel::SetFieldName()
{
  if (m_display == NULL)
  {
    m_currentData = NULL;
    return;
  }

  if (m_currentData == NULL)
  {
    return;
  }

  m_currentData->GetField()->SetDescription((const char *)GetDispFieldName()->GetValue().c_str());

}
//----------------------------------------
void CDisplayPanel::SetXAxisText()
{
  if (m_display == NULL)
  {
    m_currentData = NULL;
    return;
  }

  if (m_currentData == NULL)
  {
    return;
  }

  m_currentData->SetXAxisText( GetDispXaxis()->GetSelection(), GetDispXaxisText()->GetValue().ToStdString() );

  bool isGrouped = GetDispgroupfields()->GetValue();

  uint32_t index = GetDispXaxis()->GetSelection();

  if ( (index > 0) && (isGrouped) )
  {
    std::string axisName = GetDispXaxis()->GetStringSelection().ToStdString();
    std::string axisLabel = m_currentData->GetXAxisText(index);

    m_display->GetDataSelected()->SetAllAxis(index, axisName, axisLabel);
  }

}

//----------------------------------------
void CDisplayPanel::SetInvertXYAxes()
{
  if (m_display == NULL)
  {
    m_currentData = NULL;
    return;
  }

  if (m_currentData == NULL)
  {
    return;
  }

  bool value = GetDispInvertXyaxes()->GetValue();

  m_currentData->SetInvertXYAxes(value);

  bool isGrouped = GetDispgroupfields()->GetValue();

  if ( (isGrouped) )
  {
    m_display->GetDataSelected()->SetAllInvertXYAxes(value);
  }

}


void CDisplayPanel::SetEastComponent()
{
  if (m_display == NULL)
  {
    m_currentData = NULL;
    return;
  }

  if (m_currentData == NULL)
  {
    return;
  }

  m_currentData->SetEastComponent(GetDispEastComponent()->GetValue());

}

void CDisplayPanel::SetNorthComponent()
{
  if (m_display == NULL)
  {
    m_currentData = NULL;
    return;
  }

  if (m_currentData == NULL)
  {
    return;
  }

  m_currentData->SetNorthComponent(GetDispNorthComponent()->GetValue());

}

//----------------------------------------
void CDisplayPanel::SetSolidColor()
{
  if (m_display == NULL)
  {
    m_currentData = NULL;
    return;
  }

  //CDisplayData* data = GetFirstSelectedField();

  if (m_currentData == NULL)
  {
    return;
  }

  m_currentData->SetSolidColor(GetDispSolidColor()->GetValue());

}
//----------------------------------------
CDisplayData* CDisplayPanel::GetFirstSelectedField()
{
  long item = GetDispDataSel()->GetNextItem(-1, wxLIST_NEXT_ALL,
                              wxLIST_STATE_SELECTED);

  if (item == -1)
  {
    return NULL;
  }

  return (CDisplayData*)(GetDispDataSel()->GetItemData(item));

}
//----------------------------------------
void CDisplayPanel::SetPaletteName()
{
  if (m_display == NULL)
  {
    m_currentData = NULL;
    return;
  }

  if (m_currentData == NULL)
  {
    return;
  }


  m_currentData->SetColorPalette( GetDispPalette()->GetValue().ToStdString() );

}

//----------------------------------------
void CDisplayPanel::ShowFieldProperties()
{
  if (m_currentData != NULL)
  {
    SetFieldName();
    SetPaletteName();
    SetValueMin();
    SetValueMax();
    SetValueFieldGroup();
    SetContour();
    SetSolidColor();
    SetXAxisText();
    SetInvertXYAxes();
    SetEastComponent();
    SetNorthComponent();
  }

  m_currentData = NULL;

  ClearFieldProperties();

  if (m_display == NULL)
  {
    return;
  }

  m_currentData = GetFirstSelectedField();

  if (m_currentData == NULL)
  {
    return;
  }

  GetDispFieldName()->SetValue(m_currentData->GetField()->GetDescription().c_str());
  GetDispSolidColor()->SetValue(m_currentData->GetSolidColor());
  GetDispContour()->SetValue(m_currentData->GetContour());
  GetDispPalette()->SetValue(m_currentData->GetColorPalette());

  GetDispInvertXyaxes()->SetValue(m_currentData->IsInvertXYAxes());
  GetDispEastComponent()->SetValue(m_currentData->IsEastComponent());
  GetDispNorthComponent()->SetValue(m_currentData->IsNorthComponent());


  if (isDefaultValue(m_currentData->GetMinValue()) == false)
  {
    GetDispMin()->SetValue(m_currentData->GetMinValue());
  }
  if (isDefaultValue(m_currentData->GetMaxValue()) == false)
  {
    GetDispMax()->SetValue(m_currentData->GetMaxValue());
  }

  FillXAxis();

  EnableCtrl();

}
//----------------------------------------
void CDisplayPanel::ClearFieldProperties()
{
  GetDispFieldName()->Clear();
  //GetDispgroupfields()->SetValue(true);
  GetDispSolidColor()->SetValue(true);
  GetDispContour()->SetValue(false);

  GetDispMin()->Clear();
  GetDispMax()->Clear();

  GetDispXaxis()->Clear();
  GetDispXaxisText()->Clear();

  GetDispEastComponent()->SetValue(false);
  GetDispNorthComponent()->SetValue(false);


  EnableCtrl();

}

//----------------------------------------
void CDisplayPanel::InitDisplayOutput()
{
  if (m_display != NULL)
  {
	CWorkspaceDisplay *wksd = wxGetApp().GetCurrentWorkspaceDisplay();
    m_display->InitOutput( wksd );
  }

}
//----------------------------------------
void CDisplayPanel::GetDisplayOutput()
{
  if (m_display != NULL)
  {
    if (m_display->GetOutputName().empty())
    {
      InitDisplayOutput();
    }
  }
}
//----------------------------------------
void CDisplayPanel::OnProcessTerm(CProcessTermEvent& event)
{
  if (m_display == NULL)
  {
    return;
  }

  if (event.m_type < 0)
  {
    return;
  }

  int32_t result = wxYES;
  wxWindow* currentPage = wxGetApp().GetMainnotebook()->GetCurrentPage();

  //if ( (idx != wxNOT_FOUND) &&
  //     (wxGetApp().GetMainnotebook()->GetPageText(idx) == DISPLAY_PAGE_NAME) )
  if (currentPage == this)
  {
    result = wxMessageBox("Some new data available - Refresh list ?",
                                 "Information",
                                  wxYES_NO | wxCENTRE | wxICON_QUESTION);

  }


  if (result != wxYES)
  {
    return;
  }

  Freeze();

  LoadAvailableData();

  RefreshSelectedData();

  if (IsFrozen())
  {
    Thaw();
  }


  EnableCtrl();

}
//----------------------------------------
void CDisplayPanel::OnComboDisplayChar(wxKeyEvent &event )
{
  /*
  if (event.GetKeyCode() == ' ')
  {
  ::wxBell();
    return;
  }
  */
  m_dispNameDirty = true;
  event.Skip();
}

//----------------------------------------
void CDisplayPanel::OnComboDisplayKillFocus( wxFocusEvent &event )
{
 DispNameChanged();
 event.Skip();
}
//----------------------------------------
void CDisplayPanel::OnValueChangeXMin(CValueChangedEvent& event)
{
    UNUSED(event);

  if (m_display == NULL)
  {
    return;
  }

  double doubleValue;
  GetDispXmin()->GetValue(doubleValue, CTools::m_defaultValueDOUBLE);
  m_display->SetMinXValue(doubleValue);

  if (isDefaultValue(doubleValue))
  {
    GetDispXmin()->Clear();
  }

}
//----------------------------------------
void CDisplayPanel::OnValueChangeXMax(CValueChangedEvent& event)
{
    UNUSED(event);

  if (m_display == NULL)
  {
    return;
  }

  double doubleValue;
  GetDispXmax()->GetValue(doubleValue, CTools::m_defaultValueDOUBLE);
  m_display->SetMaxXValue(doubleValue);

  if (isDefaultValue(doubleValue))
  {
    GetDispXmax()->Clear();
  }

}
//----------------------------------------
void CDisplayPanel::OnValueChangeYMin(CValueChangedEvent& event)
{
    UNUSED(event);

  if (m_display == NULL)
  {
    return;
  }

  double doubleValue;
  GetDispYmin()->GetValue(doubleValue, CTools::m_defaultValueDOUBLE);
  m_display->SetMinYValue(doubleValue);

  if (isDefaultValue(doubleValue))
  {
    GetDispYmin()->Clear();
  }

}
//----------------------------------------
void CDisplayPanel::OnValueChangeYMax(CValueChangedEvent& event)
{
    UNUSED(event);

  if (m_display == NULL)
  {
    return;
  }

  double doubleValue;
  GetDispYmax()->GetValue(doubleValue, CTools::m_defaultValueDOUBLE);
  m_display->SetMaxYValue(doubleValue);

  if (isDefaultValue(doubleValue))
  {
    GetDispYmax()->Clear();
  }

}
//----------------------------------------
void CDisplayPanel::OnValueChangeMin(CValueChangedEvent& event)
{
    UNUSED(event);

  SetValueMin();
}
//----------------------------------------
void CDisplayPanel::OnValueChangeZoom(CValueChangedEvent& event)
{
    UNUSED(event);

  SetValueZoom();
}
//----------------------------------------
void CDisplayPanel::SetValueZoom()
{
  if (m_display == NULL)
  {
    return;
  }

  double lat;
  double lon;

  GetDispzoomLatSouth()->GetValue(lat, -90, -90, 90);
  GetDispzoomLonWest()->GetValue(lon, -180, -180, 180);

  CLatLonPoint left(lat, lon);

  GetDispzoomLatNorth()->GetValue(lat, 90, -90, 90);
  GetDispzoomLonEast()->GetValue(lon, 180, -180, 180);

  CLatLonPoint right(lat, lon);


  CLatLonRect* latLonRect = m_display->GetZoom();

  latLonRect->Set(left, right);

  GetDispzoomLatSouth()->SetValue(latLonRect->GetLowerLeftPoint().GetLatitude());
  GetDispzoomLonWest()->SetValue(latLonRect->GetLowerLeftPoint().GetLongitude());

  GetDispzoomLatNorth()->SetValue(latLonRect->GetUpperRightPoint().GetLatitude());
  GetDispzoomLonEast()->SetValue(latLonRect->GetUpperRightPoint().GetLongitude());

}

//----------------------------------------
void CDisplayPanel::SetValueMin()
{
  if (m_display == NULL)
  {
    m_currentData = NULL;
    return;
  }

  if (m_currentData == NULL)
  {
    return;
  }

  double doubleValue;
  GetDispMin()->GetValue(doubleValue, CTools::m_defaultValueDOUBLE);


  m_currentData->SetMinValue(doubleValue);

  if (isDefaultValue(doubleValue))
  {
    GetDispMin()->Clear();
  }
}
//----------------------------------------
void CDisplayPanel::OnValueChangeMax(CValueChangedEvent& event)
{
    UNUSED(event);

  SetValueMax();
}
//----------------------------------------
void CDisplayPanel::SetValueMax()
{
  if (m_display == NULL)
  {
    m_currentData = NULL;
    return;
  }

  if (m_currentData == NULL)
  {
    return;
  }

  double doubleValue;
  GetDispMax()->GetValue(doubleValue, CTools::m_defaultValueDOUBLE);

  m_currentData->SetMaxValue(doubleValue);

  if (isDefaultValue(doubleValue))
  {
    GetDispMax()->Clear();
  }

}
/*
//----------------------------------------
void CDisplayPanel::OnValueChangeFieldGroup(CValueChangedEvent& event)
{
  SetValueFieldGroup();
}
*/
//----------------------------------------
bool CDisplayPanel::BuildCmdFile()
{
	return m_display && m_display->BuildCmdFile();
}

//----------------------------------------
void CDisplayPanel::LoadSelectedData()
{
  ClearSelectedDataList();

  if (m_display == NULL)
  {
    return;
  }

  //bool bOk = true;

  GetDispDataSel()->InsertDisplay(m_display);

  GetDispType()->SetLabel(CMapTypeDisp::GetInstance().IdToName(m_display->GetType()).c_str());

  SelectAvailableFromSelectedData(GetDispDataSel()->GetTopItem());

  if (GetDispDataSel()->GetItemCount() > 0)
  {
    GetDispDataSel()->Select(0);
  }


  //FillXAxis();

  EnableCtrl();

  //FillFileList();

}
//----------------------------------------
void CDisplayPanel::SelectAvailableFromSelectedData(long item)
{
  wxTreeItemId id = FindAvailableItemFromSelectedData(item);

  if (! id.IsOk())
  {
    return;
  }

  GetDispavailtreectrl()->EnsureVisible(id);
  GetDispavailtreectrl()->SelectItem(id);

}

//----------------------------------------
wxTreeItemId CDisplayPanel::FindAvailableItemFromSelectedData(long item)
{
  wxTreeItemId id;

  if (m_display == NULL)
  {
    return id;
  }

  if (GetDispDataSel()->GetItemCount() <= 0)
  {
    return id;
  }

  if (item < 0)
  {
    return id;
  }


  wxString operationName = GetDispDataSel()->GetItemText(item);

  CDisplayData* data = (CDisplayData*)(GetDispDataSel()->GetItemData(item));

  wxTreeItemId dispTypeId = GetDispavailtreectrl()->FindItem(CMapTypeDisp::GetInstance().IdToName(m_display->GetType()).c_str());

  if (dispTypeId.IsOk())
  {
    id = dispTypeId;

    wxTreeItemId operationId = GetDispavailtreectrl()->FindItem(dispTypeId, operationName);

    if (operationId.IsOk())
    {
      id = GetDispavailtreectrl()->FindItem(operationId, data->GetField()->GetName().c_str(), true, false, true);
      if (id.IsOk())
      {
        wxTreeItemId itemParent = GetDispavailtreectrl()->GetItemParent(id);
        if (itemParent != operationId)
        {
          id.Unset();
        }
      }

      //CDisplayData* dataItemFound = GetDispavailtreectrl()->GetDisplayData(id);
    }
  }
  else
  {
    id = GetDispavailtreectrl()->FindItem(operationName);
  }

  return id;

}
/*
//----------------------------------------
CDisplayData* CDisplayPanel::FindAvailableDataFromSelectedData(long item)
{

  if (m_display == NULL)
  {
    return NULL;
  }

  if (GetDispDataSel()->GetItemCount() <= 0)
  {
    return NULL;
  }

  if (item < 0)
  {
    return NULL;
  }

  wxTreeItemId id;

  wxString operationName = GetDispDataSel()->GetItemText(item);
  wxTreeItemId dispTypeId = GetDispavailtreectrl()->FindItem(CMapTypeDisp::GetInstance().IdToName(m_display->GetType()).c_str());

  if (dispTypeId.IsOk())
  {
    id = GetDispavailtreectrl()->FindItem(dispTypeId, operationName);
  }
  else
  {
    id = GetDispavailtreectrl()->FindItem(operationName);
  }

  return id;

}
*/
//----------------------------------------
void CDisplayPanel::FillPaletteList()
{
  GetDispPalette()->Clear();
  CListColorPalette::GetInstance().NamesToComboBox(*GetDispPalette());
}

//----------------------------------------
void CDisplayPanel::FillProjList()
{
  GetDispProj()->Clear();
  NamesToComboBox(*CMapProjection::GetInstance(), *GetDispProj());
}

//----------------------------------------
void CDisplayPanel::SetTitle()
{
  if (m_display == NULL)
  {
    return;
  }
  m_display->SetTitle( GetDispTitle()->GetValue().ToStdString() );
}
//----------------------------------------
void CDisplayPanel::DispNameChanged()
{
  if (m_dispNameDirty == false)
  {
    return;
  }

  if (m_currentDisplay < 0)
  {
    return;
  }

  CWorkspaceDisplay* wks = wxGetApp().GetCurrentWorkspaceDisplay();
  if (wks == NULL)
  {
    return;
  }


  if (m_display == NULL)
  {
    return;
  }


  wxString dispName = GetDispNames()->GetValue();

  if (dispName.IsEmpty())
  {
    dispName = m_display->GetName();
    GetDispNames()->SetStringSelection(dispName);
    //GetDsnames()->SetValue(dsName);
    m_dispNameDirty = false;
    return;
  }

  wxString displayOldName = m_display->GetName();
  wxString displayOldOutputName = m_display->GetOutputName();

  if (wks->RenameDisplay(m_display, dispName.ToStdString()) == false)
  {
    m_dispNameDirty = false; // must be here, to avoid loop, because  on wxMessageBox, combo kill focus event is process again

    wxMessageBox(wxString::Format("Unable to rename view '%s' by '%s'\nPerhaps view already exists, ",
                                  displayOldName.c_str(),
                                  dispName.c_str()),
                "Warning",
                wxOK | wxICON_EXCLAMATION);
    GetDispNames()->SetSelection(m_currentDisplay);
    return;
  }

  GetDispNames()->Delete(m_currentDisplay);

  m_currentDisplay = GetDispNames()->Append(dispName);
  GetDispNames()->SetSelection(m_currentDisplay);

  InitDisplayOutput();

  RemoveFile(displayOldOutputName);

  GetDisplayOutput();

  m_dispNameDirty = false;
}


//----------------------------------------
void CDisplayPanel::SetContour()
{

  if (m_display == NULL)
  {
    m_currentData = NULL;
    return;
  }

  //CDisplayData* data = GetFirstSelectedField();

  if (m_currentData == NULL)
  {
    return;
  }

  m_currentData->SetContour(GetDispContour()->GetValue());
}

//----------------------------------------
void CDisplayPanel::SetValueFieldGroup()
{
  if (m_display == NULL)
  {
    m_currentData = NULL;
    return;
  }

  bool isGrouped = GetDispgroupfields()->GetValue();

  m_display->SetGroups(isGrouped);

}

//----------------------------------------
void CDisplayPanel::OnXAxisTextKillFocus( wxFocusEvent &event )
{
 SetXAxisText();
 event.Skip();
}
//----------------------------------------
void CDisplayPanel::OnFieldNameKillFocus( wxFocusEvent &event )
{
 SetFieldName();
 event.Skip();
}
//----------------------------------------
void CDisplayPanel::OnComboPaletteSetFocus( wxFocusEvent &event )
{
  m_paletteDirty = false;
  event.Skip();
}
//----------------------------------------
void CDisplayPanel::OnComboPaletteKillFocus( wxFocusEvent &event )
{
  SetPaletteName();

  if (m_paletteDirty)
  {
   wxGetApp().SetLastColorTable(GetDispPalette()->GetValue());
  }

  m_paletteDirty = false;

  event.Skip();
}
//----------------------------------------
void CDisplayPanel::OnComboPaletteChar(wxKeyEvent &event )
{
  m_paletteDirty = true;
  event.Skip();
}
//----------------------------------------
void CDisplayPanel::OnTitleKillFocus( wxFocusEvent &event )
{
 SetTitle();
 event.Skip();
}

//----------------------------------------
void CDisplayPanel::OnTitleTextEnter( wxCommandEvent &event )
{
 SetTitle();
 event.Skip();
}

//----------------------------------------

// WDR: handler implementations for CDisplayPanel

//----------------------------------------
void CDisplayPanel::OnInvertXYAxes( wxCommandEvent &event )
{
    UNUSED(event);

  SetInvertXYAxes();
}

void CDisplayPanel::OnNorthComponent( wxCommandEvent &event )
{
    UNUSED(event);

   long item = -1;
    for ( ;; )
    {
        item = GetDispDataSel()->GetNextItem(item,
                                     wxLIST_NEXT_ALL,
                                     wxLIST_STATE_DONTCARE);
        if ( item == -1 )
            break;
        // this item is selected - do whatever is needed with it

        CDisplayData* cdata = (CDisplayData*)(GetDispDataSel()->GetItemData(item));

        if ( cdata != m_currentData) {

            if ( cdata->IsNorthComponent() ){
                wxMessageBox( "There is already a field selected as Y Component.", wxT("Error"), wxICON_INFORMATION);
                GetDispNorthComponent()->SetValue(false);
                return;
            }
        }
    }

  GetDispEastComponent()->SetValue(false);
  SetEastComponent();
  SetNorthComponent();
}

void CDisplayPanel::OnEastComponent( wxCommandEvent &event )
{
    UNUSED(event);

    long item = -1;


    for ( ;; )
    {
        item = GetDispDataSel()->GetNextItem(item,
                                     wxLIST_NEXT_ALL,
                                     wxLIST_STATE_DONTCARE);
        if ( item == -1 )
            break;
        // this item is selected - do whatever is needed with it

        CDisplayData* cdata = (CDisplayData*)(GetDispDataSel()->GetItemData(item));

        if ( cdata != m_currentData) {

            if ( cdata->IsEastComponent() ){
                wxMessageBox( "There is already a field selected as East Component.", wxT("Error"), wxICON_INFORMATION);
                GetDispEastComponent()->SetValue(false);
                return;
            }
        }
    }

  GetDispNorthComponent()->SetValue(false);
  SetEastComponent();
  SetNorthComponent();
}

//----------------------------------------
void CDisplayPanel::OnXAxisTextEnter( wxCommandEvent &event )
{
 SetXAxisText();
 event.Skip();

}
//----------------------------------------
void CDisplayPanel::XAxisChanged()
{
  if (m_display == NULL)
  {
    m_currentData = NULL;
    return;
  }

  if (m_currentData == NULL)
  {
    return;
  }

  uint32_t index = GetDispXaxis()->GetSelection();

  if (index < 0)
  {
    m_currentData->SetXAxis("");
  }
  else
  {
    std::string axisName = GetDispXaxis()->GetStringSelection().ToStdString();
    std::string axisLabel = m_currentData->GetXAxisText(index);

    bool isGrouped = GetDispgroupfields()->GetValue();

    if (isGrouped)
    {
      m_display->GetDataSelected()->SetAllAxis(index, axisName, axisLabel);
    }
    else
    {
      m_currentData->SetXAxis(axisName);
    }
    GetDispXaxisText()->SetValue(axisLabel);
  }


}

//----------------------------------------

void CDisplayPanel::OnXAxis( wxCommandEvent &event )
{
    UNUSED(event);

  XAxisChanged();
}
//----------------------------------------

void CDisplayPanel::OnFieldGroup( wxCommandEvent &event )
{
    UNUSED(event);

  bool bOk = CheckSelectedFields();

  if ( bOk )
  {
    SetValueFieldGroup();
  }

  SetValueFieldGroup();

}
//----------------------------------------

void CDisplayPanel::OnDataAvailableSelChanged( wxTreeEvent &event )
{
    UNUSED(event);

  EnableCtrl();
}

//----------------------------------------
void CDisplayPanel::OnRefresh( wxCommandEvent &event )
{
    UNUSED(event);

  Freeze();

  LoadAvailableData();

  RefreshSelectedData();

  EnableCtrl();

  if (IsFrozen())
  {
    Thaw();
  }

}

//----------------------------------------
void CDisplayPanel::OnComboPaletteTextEnter( wxCommandEvent &event )
{
 SetPaletteName();
 wxGetApp().SetLastColorTable(GetDispPalette()->GetValue());
 m_paletteDirty = false;
 event.Skip();

}

//----------------------------------------
void CDisplayPanel::OnComboPalette( wxCommandEvent &event )
{
    UNUSED(event);

 SetPaletteName();
 wxGetApp().SetLastColorTable(GetDispPalette()->GetValue());
 m_paletteDirty = false;

}

//----------------------------------------
void CDisplayPanel::OnBrowsePalette( wxCommandEvent &event )
{
    UNUSED(event);

  if (m_display == NULL)
  {
    m_currentData = NULL;
    return;
  }

  if (m_currentData == NULL)
  {
    return;
  }


  wxFileName currentDir;
  currentDir.AssignDir(".");
  currentDir.Normalize();

  wxString wildcards = wxString::Format(_T("All files (%s)|%s"),
                                        wxFileSelectorDefaultWildcardStr,
                                        wxFileSelectorDefaultWildcardStr
                                        );

  wxFileDialog dlg(this, "Select a color palette file ...", currentDir.GetPath(), wxEmptyString, wildcards, wxFD_CHANGE_DIR|wxFD_FILE_MUST_EXIST);
  if (dlg.ShowModal() != wxID_OK)
  {
    return;
  }

  std::string palette = dlg.GetPath().ToStdString();

  GetDispPalette()->SetValue(palette);

  m_currentData->SetColorPalette(palette);
}

//----------------------------------------
void CDisplayPanel::OnContour( wxCommandEvent &event )
{
    UNUSED(event);

  SetContour();
}

//----------------------------------------
void CDisplayPanel::OnSolidColor( wxCommandEvent &event )
{
    UNUSED(event);

  SetSolidColor();
}

//----------------------------------------
void CDisplayPanel::OnWithAnimation( wxCommandEvent &event )
{
    UNUSED(event);

  if (m_display == NULL)
  {
    return;
  }

  m_display->SetWithAnimation(GetDispWithAnimation()->GetValue());

}

//----------------------------------------
void CDisplayPanel::OnFieldNameTextEnter( wxCommandEvent &event )
{
 SetFieldName();
 event.Skip();

}


//----------------------------------------
void CDisplayPanel::OnComboDisplayTextEnter( wxCommandEvent &event )
{
    UNUSED(event);

  DispNameChanged();
}

//----------------------------------------
void CDisplayPanel::OnComboDisplay( wxCommandEvent &event )
{
  Freeze();

  GetDispDataSel()->DeselectAll();

  m_currentDisplay = event.GetInt();
  SetCurrentDisplay();

  ShowGeneralProperties();
  ShowFieldProperties();

  LoadAvailableData();

  LoadSelectedData();

  RefreshSelectedData();

  EnableCtrl();

  if (IsFrozen())
  {
    Thaw();
  }
}

//----------------------------------------
void CDisplayPanel::OnDataSelDeselected( wxListEvent &event )
{
  ShowFieldProperties();

  EnableCtrl();

  event.Skip();

}

//----------------------------------------
void CDisplayPanel::OnDataSelSelected( wxListEvent &event )
{
  ShowFieldProperties();

  SelectAvailableFromSelectedData(event.GetIndex());

  EnableCtrl();
  event.Skip();

}

//----------------------------------------
void CDisplayPanel::OnRemoveField( wxCommandEvent &event )
{
    UNUSED(event);

  RemoveFields();
}
//----------------------------------------
void CDisplayPanel::OnDispSelFieldDeleted(CDeleteDisplaySelFieldEvent& event)
{
    UNUSED(event);

  GetDispType()->SetLabel(CMapTypeDisp::GetInstance().IdToName(m_display->GetType()).c_str());
  m_currentData = NULL;

  EnableCtrl();
}


//----------------------------------------
void CDisplayPanel::OnAddField( wxCommandEvent &event )
{
    UNUSED(event);

  AddField();
}

//----------------------------------------
void CDisplayPanel::OnComboProj( wxCommandEvent &event )
{
	UNUSED( event );

	if ( m_display == NULL )
		return;

	m_display->SetProjection( GetDispProj()->GetStringSelection().ToStdString() );
}

//----------------------------------------
void CDisplayPanel::OnExecute( wxCommandEvent &event )
{
    UNUSED(event);

  Execute();
}

//----------------------------------------
void CDisplayPanel::OnDeleteView( wxCommandEvent &event )
{
    UNUSED(event);

  DeleteDisplay();
}
//----------------------------------------

void CDisplayPanel::OnNewView( wxCommandEvent &event )
{
    UNUSED(event);

  NewDisplay();
}

//----------------------------------------
void CDisplayPanel::ClearAll()
{
  GetDispNames()->Clear();
  GetDispProj()->Clear();
  GetDispTitle()->Clear();
  ClearAvailableDataList();
  ClearSelectedDataList();
  ClearFieldProperties();

  Reset();

  EnableCtrl();
}

//----------------------------------------
void CDisplayPanel::ClearSelectedDataList()
{
  GetDispDataSel()->InsertDisplay(NULL);
}

//----------------------------------------

void CDisplayPanel::AddField()
{

  if (m_display == NULL)
  {
    return;
  }

  CObArray data(false);
  CDisplayData* displayData = GetDispavailtreectrl()->GetCurrentDisplayData();

  if (displayData != NULL)
  {
    data.Insert(displayData);
  }
  else
  {
    wxTreeItemId idSelected = GetDispavailtreectrl()->GetSelection();
    GetDispavailtreectrl()->GetAllDisplayData(data, idSelected);
  }

  AddField(data);

}
//----------------------------------------
void CDisplayPanel::RemoveFields()
{
  m_currentData = NULL;
  GetDispDataSel()->RemoveFields();
}
//----------------------------------------
void CDisplayPanel::OnNewOperation(CNewOperationEvent& event)
{
    UNUSED(event);

  LoadAvailableData();
  EnableCtrl();
}
//----------------------------------------
void CDisplayPanel::OnRenameOperation(CRenameOperationEvent& event)
{
    UNUSED(event);

  LoadAvailableData();
  LoadSelectedData();
  EnableCtrl();
}
//----------------------------------------
void CDisplayPanel::OnDeleteOperation(CDeleteOperationEvent& event)
{
    UNUSED(event);

  LoadAvailableData();
  EnableCtrl();
}

//----------------------------------------

//----------------------------------------------------------------------------
// CDisplayDataDropTarget
//----------------------------------------------------------------------------

wxDragResult CDisplayDataDropTarget::OnDragOver(wxCoord x, wxCoord y, wxDragResult def)
{
    UNUSED(y);    UNUSED(x);

  return (def == wxDragMove ? wxDragCopy : def);
}

//----------------------------------------
wxDragResult CDisplayDataDropTarget::OnData(wxCoord x, wxCoord y, wxDragResult def)
{
    UNUSED(def);    UNUSED(y);    UNUSED(x);

    if (!GetData())
  {
    return wxDragNone;
  }

  CDndDisplayDataObject* dndDisplayDataObject = dynamic_cast<CDndDisplayDataObject*>(GetDataObject());
  CDndDisplayData* dndDisplayData = dndDisplayDataObject->GetDisplayData();

  CObArray& data = dndDisplayData->m_data;

  m_pOwner->AddField(data);

  return wxDragCopy;
}
