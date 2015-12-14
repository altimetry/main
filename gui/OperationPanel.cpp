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

// Temporary hack to prevent the inclusion of Windows XML 
// headers, which collide with xerces
//
#if defined (WIN32) || defined (_WIN32)
//avoid later inclusion of Microsoft XML stuff, which causes name collisions with xerces
#define DOMDocument MsDOMDocument
#include <msxml.h>
#include <urlmon.h>
#undef DOMDocument
#endif


#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "OperationPanel.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
//#include "wx/wxprec.h"
#include "wx/wx.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <algorithm>

#include <wx/ffile.h>
#include <wx/generic/gridctrl.h>

#include "new-gui/Common/+Utils.h"
#include "new-gui/Common/tools/Exception.h"
#include "Expression.h"
using namespace brathl;

#include "Validators.h"
#include "MapColor.h"
#include "BratGui.h"
#include "Workspace.h"
#include "Function.h"
#include "Process.h"
#include "ResolutionDlg.h"
#include "FormulaDlg.h"
#include "FunctionDlg.h"
#include "ExportDlg.h"
#include "ParametersDictionary.h"
#include "RichTextFrame.h"
#include "AlgorithmDlg.h"
#include "DelayDlg.h"

#include "OperationPanel.h"

// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

// WDR: class implementations
//----------------------------------------------------------------------------
// CAliasInfoDlg
//----------------------------------------------------------------------------
const int32_t CAliasInfoDlg::VALUE_COL = 0;
const int32_t CAliasInfoDlg::SYNTAX_COL = 1;
const int32_t CAliasInfoDlg::DESCR_COL = 2;
const int32_t CAliasInfoDlg::USE_COL = 3;

IMPLEMENT_CLASS(CAliasInfoDlg,wxDialog)

// WDR: event table for AliasInfoDlg

BEGIN_EVENT_TABLE(CAliasInfoDlg,wxDialog)
    EVT_BUTTON( wxID_OK, CAliasInfoDlg::OnOk )
END_EVENT_TABLE()
//----------------------------------------

CAliasInfoDlg::CAliasInfoDlg( wxWindow *parent, wxWindowID id, const wxString &title,
                                        COperation* operation, CFormula* formula,
                                        const wxPoint &position, const wxSize& size, long style ) :
        wxDialog( parent, id, title, position, size, style )
{

      // WDR: dialog function AliasInfoDlg for CAliasInfoDlg
  //--------------------------------
  m_item0 = AliasInfoDlg( this, true, true );
  //--------------------------------

  CProduct* product = operation->GetProduct();
  if (product == NULL)
  {
    return;
  }


  GetAliasinfoHeader()->SetLabel(wxString::Format("%s %s", 
                                                GetAliasinfoHeader()->GetLabel().c_str(),
                                                product->GetProductClassType().c_str()));

  CStringArray aliasesArray;
  product->GetAliasKeys(aliasesArray);

  sort(aliasesArray.begin(), aliasesArray.end(), CTools::StringCompare);
  //GetAliasinfoGrid()->BeginBatch();

  bool hasOpFieldSelected = (formula != NULL);
  
  int numCols = 3;
  if (hasOpFieldSelected)
  {
    numCols++;
  }

  GetAliasinfoGrid()->SetMinSize(GetAliasinfoGrid()->GetBestSize());
  GetAliasinfoGrid()->Fit();

  GetAliasinfoGrid()->EnableEditing(true);
  GetAliasinfoGrid()->CreateGrid( aliasesArray.size(), numCols );
  GetAliasinfoGrid()->SetDefaultCellAlignment(wxALIGN_LEFT, wxALIGN_CENTRE);

  GetAliasinfoGrid()->SetRowLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTRE);

  GetAliasinfoGrid()->SetColLabelValue(CAliasInfoDlg::VALUE_COL, "Value");
  
  GetAliasinfoGrid()->SetColLabelValue(CAliasInfoDlg::SYNTAX_COL, "Syntax");
  GetAliasinfoGrid()->SetColLabelValue(CAliasInfoDlg::DESCR_COL, "Description");

 
  wxGridCellAttr* attrReadOnly = new wxGridCellAttr();
  attrReadOnly->SetReadOnly(true);
  GetAliasinfoGrid()->SetColAttr(CAliasInfoDlg::VALUE_COL,  attrReadOnly);

  GetAliasinfoGrid()->SetColAttr(CAliasInfoDlg::SYNTAX_COL,  attrReadOnly);
  attrReadOnly->IncRef();
  GetAliasinfoGrid()->SetColAttr(CAliasInfoDlg::DESCR_COL,  attrReadOnly);
  attrReadOnly->IncRef();


  GetAliasinfoFooter()->Show(false);

  if (hasOpFieldSelected)
  {
    //GetAliasinfoGrid()->SetColFormatBool(CAliasInfoDlg::USE_COL);
    GetAliasinfoGrid()->SetColLabelValue(CAliasInfoDlg::USE_COL, "Select (*)");
    GetAliasinfoFooter()->Show(true);
  }
  
  //int32_t charwidth, charheight;
  //CBratGuiApp::DetermineCharSize(GetAliasinfoGrid(), charwidth, charheight);
  
  int32_t rowLabelSize = 0;

  for (uint32_t i = 0 ; i < aliasesArray.size() ; i++)
  {
    std::string name = aliasesArray.at(i);
    
    if (rowLabelSize < static_cast<int32_t>(name.size()))
    {
      rowLabelSize = name.size();
      GetAliasinfoGrid()->SetRowLabelSize(GetAliasinfoGrid()->GetCharWidth() * (rowLabelSize + 5));
    }

    GetAliasinfoGrid()->SetRowLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTRE);
    GetAliasinfoGrid()->SetRowLabelValue(i, name.c_str());

    GetAliasinfoGrid()->SetCellValue(i, CAliasInfoDlg::VALUE_COL, product->GetAliasExpandedValue(name).c_str());
    GetAliasinfoGrid()->SetCellRenderer(i , CAliasInfoDlg::VALUE_COL, new wxGridCellAutoWrapStringRenderer);

    GetAliasinfoGrid()->SetCellValue(i, CAliasInfoDlg::SYNTAX_COL, CTools::Format("%%{%s}", name.c_str()).c_str());

    const CProductAlias* productAlias = product->GetAlias(name);
    if (productAlias != NULL)
    {
      GetAliasinfoGrid()->SetCellValue(i, CAliasInfoDlg::DESCR_COL, productAlias->GetDescription().c_str());
      GetAliasinfoGrid()->SetCellRenderer(i , CAliasInfoDlg::DESCR_COL, new wxGridCellAutoWrapStringRenderer);

    }

    if (hasOpFieldSelected)
    {
      // Warning set col value to empty std::string ("") as value 'false'. Don't use "0".
      // Because of in wxGridCellBoolEditor dafault are : "" as false and "1" as true
      // Using others values raises an assertion.
      GetAliasinfoGrid()->SetCellValue(i, CAliasInfoDlg::USE_COL, "");
      GetAliasinfoGrid()->SetCellRenderer(i, CAliasInfoDlg::USE_COL, new wxGridCellBoolRenderer);
      GetAliasinfoGrid()->SetCellEditor(i, CAliasInfoDlg::USE_COL, new wxGridCellBoolEditor);
    }

  }

  GetAliasinfoGrid()->Fit();
  
  this->Layout();

  wxSize s = glbAliasParamSizerH->GetSize();
  m_gridSize.SetWidth(s.GetWidth());

  s = glbAliasParamSizerV->GetSize();
  m_gridSize.SetHeight(s.GetHeight());

  //GetAliasinfoGrid()->ForceRefresh();

  //GetAliasinfoGrid()->EndBatch();

  GetAliasinfoGrid()->SetMinSize(m_gridSize);

  glbAliasParamSizerH->Layout();
  glbAliasParamSizerV->Layout();
  m_item0->Layout();
  m_item0->SetSizeHints( this );



}
//----------------------------------------

CAliasInfoDlg::~CAliasInfoDlg()
{
}
//----------------------------------------

// WDR: handler implementations for CAliasInfoDlg

void CAliasInfoDlg::OnOk(wxCommandEvent &event)
{
    event.Skip();
}

//----------------------------------------

//----------------------------------------------------------------------------
// CExpressionInfoDlg
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(CExpressionInfoDlg,wxDialog)

// WDR: event table for ExpressionInfoDlg

BEGIN_EVENT_TABLE(CExpressionInfoDlg,wxDialog)
    EVT_BUTTON( wxID_OK, CExpressionInfoDlg::OnOk )
END_EVENT_TABLE()
//----------------------------------------

CExpressionInfoDlg::CExpressionInfoDlg( wxWindow *parent, wxWindowID id, const wxString &title,
                                        wxString exprValue, COperation* operation, CFormula* formula,
                                        const wxPoint &position, const wxSize& size, long style ) :
        wxDialog( parent, id, title, position, size, style )
{

      // WDR: dialog function CExpressionInfoDlg for CExpressionInfoDlg
  wxSizer *item0 = ExpressionInfoDlg( this, true, true );


  //int32_t charwidth, charheight;
  //CBratGuiApp::DetermineCharSize(this, charwidth, charheight);

  std::string errorMsg;
  CExpression expr;
  CFormula::SetExpression(exprValue, expr, errorMsg);

  const CStringArray* fieldNames = expr.GetFieldNames();

  //GetExprinfoGrid()->BeginBatch();

  GetExprinfoGrid()->CreateGrid( fieldNames->size(), 2 );
  GetExprinfoGrid()->SetDefaultCellAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);

  GetExprinfoGrid()->SetColLabelValue(0, "Original unit");
  GetExprinfoGrid()->SetColLabelValue(1, "Calculation unit (SI)");

  int32_t rowLabelSize = 0;

  for (uint32_t i = 0 ; i < fieldNames->size() ; i++)
  {
    std::string fieldName = fieldNames->at(i);
    
    if (rowLabelSize < static_cast<int32_t>(fieldName.size()))
    {
      rowLabelSize = fieldName.size();
      GetExprinfoGrid()->SetRowLabelSize(GetExprinfoGrid()->GetCharWidth() * (rowLabelSize + 5));
    }

    GetExprinfoGrid()->SetRowLabelValue(i, fieldName.c_str());

    CProduct* product = operation->GetProduct();

    if (product != NULL)
    {
      CField* field = product->FindFieldByName(fieldName, (const char *)operation->GetRecord().c_str(), false, NULL, true);

      if (field != NULL)
      {
        wxString orignalUnit = (field->GetUnit().empty() ? "count" : field->GetUnit().c_str());
        GetExprinfoGrid()->SetCellValue(i, 0, orignalUnit);
        wxString baseUnit;

        try
        {
          CUnit unit = field->GetUnit();
          if (unit.HasDateRef())
          {
            baseUnit = CUnit::m_DATE_REF_UNIT.c_str();
          }
          else
          {
            baseUnit = unit.BaseUnit().AsString().c_str();
          }
        }
        catch (CException& e)
        {
          e.what();
          baseUnit = "count";
        }

        GetExprinfoGrid()->SetCellValue(i, 1, baseUnit);
      }

    }

  }


  GetExprinfoGrid()->Fit();

  this->Layout();

  wxSize s = glbExprInfoGridSizerH->GetSize();
  wxSize gridSize;
  gridSize.SetWidth(s.GetWidth());

  s = glbExprInfoGridSizerH->GetSize();
  gridSize.SetHeight(s.GetHeight());

  GetExprinfoGrid()->SetMinSize(gridSize);

  if (formula != NULL)
  {
    wxString resultUnit =(operation->IsSelect(formula) ? "boolean" : formula->GetUnit()->AsString(false).c_str());
    GetExprinfoResultUnit()->SetLabel(wxString::Format("Result unit: %s", resultUnit.c_str()));
  }


  glbExprInfoGridSizerH->Layout();
  glbExprInfoGridSizerV->Layout();
  item0->Layout();
  item0->SetSizeHints( this );
  item0->SetSizeHints( this );


}
//----------------------------------------

CExpressionInfoDlg::~CExpressionInfoDlg()
{
}
//----------------------------------------

// WDR: handler implementations for CExpressionInfoDlg

void CExpressionInfoDlg::OnOk(wxCommandEvent &event)
{
    event.Skip();
}

//----------------------------------------


//----------------------------------------------------------------------------
// COperationPanel
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(COperationPanel,wxPanel)

// WDR: event table for COperationPanel

BEGIN_EVENT_TABLE(COperationPanel,wxPanel)
    EVT_TREE_SEL_CHANGED( ID_DATASETTREECTRL, COperationPanel::OnDatasetSelChanged )
    EVT_TREE_SEL_CHANGED( ID_OPERATIONTREECTRL, COperationPanel::OnOperationSelChanged )
    EVT_TREE_SEL_CHANGING( ID_DATASETTREECTRL, COperationPanel::OnDatasetSelChanging )
    EVT_BUTTON( ID_OPNEW, COperationPanel::OnNewOperation )
    EVT_BUTTON( ID_OPDUP, COperationPanel::OnDuplicateOperation )
    EVT_BUTTON( ID_OPDELETE, COperationPanel::OnDeleteOperation )
    EVT_BUTTON( ID_OPEXPORT, COperationPanel::OnExportOperation )
    EVT_BUTTON( ID_OPEXPORTASCIIEDIT, COperationPanel::OnEditAsciiExportOperation )
    EVT_BUTTON( ID_OPINSERTEXPR, COperationPanel::OnInsertExpression )
    EVT_BUTTON( ID_OPINSERTFIELD, COperationPanel::OnInsertField )
    EVT_BUTTON( ID_OPINSERTFCT, COperationPanel::OnInsertFunction )
    EVT_BUTTON( ID_OPINSERTFORMULA, COperationPanel::OnInsertFormula )
    EVT_BUTTON( ID_OPSAVEASFORMULA, COperationPanel::OnSaveAsFormula )
    EVT_BUTTON( ID_OPDELETEEXPR, COperationPanel::OnDeleteExpression )
    EVT_BUTTON( ID_OPCHECKDATAF, COperationPanel::OnCheckSyntax )
    EVT_BUTTON( ID_OPTITLE, COperationPanel::OnTitle )
    EVT_BUTTON( ID_OPRESOLUTION, COperationPanel::OnResolution )
    EVT_COMBOBOX( ID_OP_DATA_MODE, COperationPanel::OnComboDataMode )
    EVT_COMBOBOX( ID_OPNAMES, COperationPanel::OnComboOperation )
    EVT_TEXT_ENTER( ID_OPNAMES, COperationPanel::OnComboOperationTextEnter )
    EVT_BUTTON( ID_OPEXECUTE, COperationPanel::OnExecute )
    EVT_TEXT( ID_OPTEXTFORM, COperationPanel::OnTextFormula )
    EVT_TEXT_ENTER( ID_OPTEXTFORM, COperationPanel::OnTextFormulaEnter )
    EVT_TEXT( ID_OPUNIT, COperationPanel::OnUnitText )
    EVT_TEXT_ENTER( ID_OPUNIT, COperationPanel::OnUnitTextEnter )
    EVT_COMBOBOX( ID_OPTYPE, COperationPanel::OnComboDataType )
    EVT_TREE_SEL_CHANGED( ID_FIELDSTREECTRL, COperationPanel::OnFieldSelChanged )
    EVT_SPLITTER_SASH_POS_CHANGING( -1, COperationPanel::OnPositionChanging )
    EVT_BUTTON( ID_OPEXPRINFO, COperationPanel::OnShowExprInfo )
    EVT_BUTTON( ID_OPSHOWSTATS, COperationPanel::OnShowStats )
    EVT_BUTTON( ID_OPCHANGERECORD, COperationPanel::OnChangeDefaultRecord )
    EVT_BUTTON( ID_OPINSERTALGORITHM, COperationPanel::OnInsertAlgorithm )
    EVT_BUTTON( ID_OPDELAY, COperationPanel::OnDelay )
    EVT_BUTTON( ID_LAUNCH_SCHEDULER, COperationPanel::OnLaunchScheduler )
    EVT_BUTTON( ID_OPALIASINFO, COperationPanel::OnShowAliases )
END_EVENT_TABLE()

COperationPanel::COperationPanel( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxPanel( parent, id, position, size, style )
{


  m_product = NULL;
  m_leftPanel = NULL;
  m_rightPanel = NULL;
  m_operation = NULL;

  Reset();

  wxBoxSizer *item0 = new wxBoxSizer( wxHORIZONTAL );
  wxBoxSizer *item1 = new wxBoxSizer( wxVERTICAL );

  wxPanel* top_Panel = new wxPanel( this, -1 );

  //----------------------------
  OperationPanelHeader( top_Panel, false, true );
  //----------------------------

  wxSplitterWindow* splitterVert = new wxSplitterWindow( this, -1, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE|wxSP_3DSASH|wxCLIP_CHILDREN );

  m_leftPanel = new wxPanel( splitterVert, -1 );
  //----------------------------
  DataStruct( m_leftPanel, false, true );
  //----------------------------
  m_leftPanel->SetMinSize(m_leftPanel->GetBestSize());
  m_leftPanel->SetSize(m_leftPanel->GetBestSize());


  m_rightPanel = new wxPanel( splitterVert, -1 );
  //----------------------------
  OperationInfoPanel( m_rightPanel, false, true );
  //----------------------------
  m_rightPanel->SetMinSize(m_rightPanel->GetBestSize());
  m_rightPanel->SetSize(m_rightPanel->GetBestSize());

  wxStaticLine *line = new wxStaticLine( this, -1 );

  item1->Add(top_Panel, 0, wxGROW|wxALL, 5);
  item1->Add(line, 0, wxGROW|wxALL, 5);
  item1->Add(splitterVert, 1, wxGROW|wxALL, 5);

  SetSizer(item1);

  splitterVert->SetMinimumPaneSize( 250 );

  splitterVert->SplitVertically( m_leftPanel, m_rightPanel, m_leftPanel->GetBestSize().x );

  item0->Add( this, 1, wxGROW|wxALL, 5 );

  parent->SetSizer( item0 );

  CTextLightValidator textLightValidator;

  GetOperationtreectrl()->SetDropTarget(new CFieldDropTarget(GetOperationtreectrl()));
  GetOperationtreectrl()->SetOwner(this);

  GetOpnames()->SetValidator(textLightValidator);

  //GetOptextform()->SetDropTarget(new CFieldTextCtrlDropTarget(GetOptextform()));
  GetOptextform()->SetDropTarget(new CFieldTextCtrlDropTarget(this));

  GetOpchangerecord()->SetBitmapDisabled(ButtonBitmapsFunc( 7 ));

  SetDataSetAndRecordLabel();

  CGuiPanel::EvtNewDatasetCommand(*this,
                                 (CNewDatasetEventFunction)&COperationPanel::OnNewDataset, NULL, this);

  CGuiPanel::EvtRenameDatasetCommand(*this,
                                 (CRenameDatasetEventFunction)&COperationPanel::OnRenameDataset, NULL, this);

  CGuiPanel::EvtDeleteDatasetCommand(*this,
                                 (CDeleteDatasetEventFunction)&COperationPanel::OnDeleteDataset, NULL, this);

  CBratGuiApp::EvtFocus(*GetOpnames(), wxEVT_KILL_FOCUS,
                          (wxFocusEventFunction)&COperationPanel::OnComboOperationKillFocus,
                          NULL,
                          this);

  CBratGuiApp::EvtChar(*GetOpnames(), wxEVT_CHAR,
                          (wxCharEventFunction)&COperationPanel::OnComboOperationChar,
                          NULL,
                          this);

  CBratGuiApp::EvtChar(*GetOpunit(), wxEVT_CHAR,
                          (wxCharEventFunction)&COperationPanel::OnUnitChar,
                          NULL,
                          this);

  CBratGuiApp::EvtFocus(*GetOpunit(), wxEVT_KILL_FOCUS,
                          (wxFocusEventFunction)&COperationPanel::OnUnitKillFocus,
                          NULL,
                          this);

  CBratGuiApp::EvtFocus(*GetOptextform(), wxEVT_KILL_FOCUS,
                          (wxFocusEventFunction)&COperationPanel::OnTextFormulaKillFocus,
                          NULL,
                          this);

  CGuiPanel::EvtNewDataExprCommand(*this,
                                 (CNewDataExprEventFunction)&COperationPanel::OnDataExprNew, NULL, this);

  CGuiPanel::EvtDeleteDataExprCommand(*this,
                                 (CDeleteDataExprEventFunction)&COperationPanel::OnDataExprDeleted, NULL, this);

  CProcess::EvtProcessTermCommand(*this,
                                 (CProcessTermEventFunction)&COperationPanel::OnProcessTerm, NULL, this);

  CGuiPanel::EvtDatasetFilesChangedCommand(*this,
                                 (CDatasetFilesChangeEventFunction)&COperationPanel::OnDatasetFilesChanged, NULL, this);
}
//----------------------------------------
COperationPanel::~COperationPanel()
{
  DeleteProduct();
}

//----------------------------------------
void COperationPanel::SetUnitText()
{
  if (m_operation == NULL)
  {
    return;
  }
  if (m_userFormula == NULL)
  {
    return;
  }

  wxString unitValue = GetOpunit()->GetValue();
  std::string defaultValue;

  wxString formulaValue = GetOptextform()->GetValue();
  formulaValue.Trim(false);
  formulaValue.Trim(true);

  wxTreeItemId item = GetFieldstreectrl()->FindItem(formulaValue, true, false);

  if (item.IsOk())
  {
    defaultValue = GetFieldstreectrl()->GetFieldUnit(item);
  }

  if (unitValue.IsEmpty())
  {

    GetOpunit()->SetValue(defaultValue);
  }


  m_userFormula->SetUnit(GetOpunit()->GetValue().ToStdString(), defaultValue, false, true);
  GetOpunit()->SetValue(m_userFormula->GetUnitAsText());

  std::string msg;
  bool bOk = m_userFormula->ControlUnitConsistency(msg);
  if (!bOk)
  {
    wxMessageBox(msg,
                "Warning",
                wxOK | wxICON_WARNING);
  }

//  ComputeInterval(m_userFormula);

  SetResolutionLabels();
  ShowResolutionAndFilterSizer(m_operation->IsZFXY());


}
//----------------------------------------
void COperationPanel::OnNewDataset(CNewDatasetEvent& event)
{
  GetDatasettreectrl()->AddItemToTree(event.m_datasetName);
}
//----------------------------------------
void COperationPanel::OnRenameDataset(CRenameDatasetEvent& event)
{
  wxTreeItemId id = GetDatasettreectrl()->FindItem(event.m_datasetOldName);
  if (!id)
  {
    wxMessageBox(wxString::Format("ERROR in COperationPanel::OnRenameDataset : Unable to find dataset '%s'",
                                  event.m_datasetOldName.c_str()),
                "ERROR",
                wxOK | wxICON_ERROR);
    return;
  }

  GetDatasettreectrl()->SetItemText(id, event.m_datasetNewName);
  return;

}
//----------------------------------------
void COperationPanel::OnDeleteDataset(CDeleteDatasetEvent& event)
{
  wxTreeItemId id = GetDatasettreectrl()->FindItem(event.m_datasetName);
  if (!id)
  {
      wxMessageBox(wxString::Format("ERROR in COperationPanel::OnDeleteDataset : Unable to delete dataset '%s'."
                                    "\nIt's used by an operation",
                                    event.m_datasetName.c_str()),
                  "ERROR",
                  wxOK | wxICON_ERROR);
      return;
  }

  wxTreeItemId idSel = GetDatasettreectrl()->GetSelection();
  if (id == idSel)
  {
      wxMessageBox(wxString::Format("ERROR in COperationPanel::OnDeleteDataset : Unable to delete dataset '%s'."
                                    "\nIt's used by an operation",
                                    event.m_datasetName.c_str()),
                  "ERROR",
                  wxOK | wxICON_ERROR);
      return;
  }

  GetDatasettreectrl()->Delete(id);
}

//----------------------------------------
void COperationPanel::OnUnitChar(wxKeyEvent &event )
{
  event.Skip();
}
//----------------------------------------
void COperationPanel::OnUnitKillFocus( wxFocusEvent &event )
{
 SetUnitText();
 event.Skip();
}
//----------------------------------------
void COperationPanel::OnUnitTextEnter( wxCommandEvent &event )
{
 SetUnitText();
 event.Skip();
}
//----------------------------------------
void COperationPanel::OnUnitText( wxCommandEvent &event )
{
// SetUnitText();
 event.Skip();
}

//----------------------------------------
void COperationPanel::OnComboOperationChar(wxKeyEvent &event )
{
  /*
  if (event.GetKeyCode() == ' ')
  {
  ::wxBell();
    return;
  }
  */
  m_opNameDirty = true;
  event.Skip();
}

//----------------------------------------
void COperationPanel::OnComboOperationKillFocus( wxFocusEvent &event )
{
 OpNameChanged();
 event.Skip();
}


//----------------------------------------
void COperationPanel::OpNameChanged()
{
  if (m_opNameDirty == false)
  {
    return;
  }

  if (m_currentOperation < 0)
  {
    return;
  }

  CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();
  if (wks == NULL)
  {
    return;
  }

  //COperation* operation = GetCurrentOperation();

  //if (operation == NULL)
  //{
  //  return;
  //}

  if (m_operation == NULL)
  {
    return;
  }


  wxString opName = GetOpnames()->GetValue();

  if (opName.IsEmpty())
  {
    opName = m_operation->GetName();
    GetOpnames()->SetStringSelection(opName);
    //GetDsnames()->SetValue(dsName);
    m_opNameDirty = false;
    return;
  }

  wxString operationOldName = m_operation->GetName();

  if (wks->RenameOperation(m_operation, opName.ToStdString()) == false)
  {
    m_opNameDirty = false; // must be here, to avoid loop, because  on wxMessageBox, combo kill focus event is process again

    wxMessageBox(wxString::Format("Unable to rename operation '%s' by '%s'\nPerhaps operation already exists, ",
                                  operationOldName.c_str(),
                                  opName.c_str()),
                "Warning",
                wxOK | wxICON_EXCLAMATION);
    GetOpnames()->SetSelection(m_currentOperation);
    return;
  }

  GetOpnames()->Delete(m_currentOperation);

  m_currentOperation = GetOpnames()->Append(opName);
  GetOpnames()->SetSelection(m_currentOperation);

  CRenameOperationEvent ev(GetId(), operationOldName, opName);
  wxPostEvent(GetParent(), ev);

  wxString oldOutput = m_operation->GetOutputName();

  InitOperationOutputs();

  RenameOutput(oldOutput);

  GetOperationOutputs();

  m_opNameDirty = false;

}
//----------------------------------------
void COperationPanel::InitOperationExportAsciiOutput()
{
  if (m_operation != NULL)
  {
    m_operation->InitExportAsciiOutput();
  }

}
//----------------------------------------
void COperationPanel::InitOperationOutput()
{
  if (m_operation != NULL)
  {
    m_operation->InitOutput();
  }

}
//----------------------------------------
void COperationPanel::InitOperationOutputs()
{
  InitOperationOutput();
  InitOperationExportAsciiOutput();
}
//----------------------------------------
void COperationPanel::GetOperationOutput()
{
  if (m_operation != NULL)
  {
    if (m_operation->GetOutputName().IsEmpty())
    {
      InitOperationOutput();
    }
  }
}

//----------------------------------------
void COperationPanel::GetOperationOutputs()
{
  GetOperationOutput();
  GetOperationExportAsciiOutput();
}

//----------------------------------------
void COperationPanel::GetOperationExportAsciiOutput()
{
  if (m_operation != NULL)
  {
    if (m_operation->GetExportAsciiOutputName().IsEmpty())
    {
      InitOperationExportAsciiOutput();
    }
  }
}
//----------------------------------------
void COperationPanel::RemoveOutput()
{
  if (m_operation == NULL)
  {
    return;
  }

  bool bOk = wxFileExists(m_operation->GetOutputName());
  if (bOk == false)
  {
    return;
  }


  int32_t result = wxMessageBox(wxString::Format("Do you want to delete output file\n'%s'\nlinked to this operation ?",
                                                  m_operation->GetOutputName().c_str()),
                               "Warning",
                                wxYES_NO | wxCENTRE | wxICON_QUESTION);

  if (result != wxYES)
  {
    return;
  }

  bOk = m_operation->RemoveOutput();
  if (bOk == false)
  {
    wxMessageBox(wxString::Format("Unable to delete file '%s' \nYou have to delete it by yourself",
                                  m_operation->GetOutputName().c_str()),
                "Warning",
                wxOK | wxICON_EXCLAMATION);
  }

}

//----------------------------------------
void COperationPanel::RenameOutput(const wxString& oldName)
{
  if (m_operation == NULL)
  {
    return;
  }

  bool outputExists = wxFileExists(oldName);

  bool bOk = m_operation->RenameOutput(oldName);

  if (bOk == false)
  {
    wxMessageBox(wxString::Format("Unable to rename file '%s' by '%s'\nYou have to rename it by yourself",
                                  oldName.c_str(),
                                  m_operation->GetOutputName().c_str()),
                "Warning",
                wxOK | wxICON_EXCLAMATION);
  }

  if (outputExists) // old file has been renamed, so we ha ve to save ALL workspaces to keep consistency
  {
    wxGetApp().m_tree.SaveConfig();
  }


}
//----------------------------------------
void COperationPanel::ClearFieldsInfo()
{
  if (m_operation != NULL)
  {
    if (m_operation->GetDataset() != NULL)
    {
      DeleteProduct();
    }
  }
  GetFieldstreectrl()->InsertProduct(NULL);

}
//----------------------------------------
void COperationPanel::EnableCtrl()
{
  bool hasOperation = false;
  bool hasDataset = false;
  bool hasUserFormula = false;

  bool enable = false;
  bool enableField  = false;
  bool enableDataType  = false;
  bool enableAddExpression = false;
  bool enableDeleteExpression = false;
  bool enableExecute = false;

  bool enableSaveAs = false;

  bool hasOpFieldSelected = false;

  bool isZFXY = false;
  bool isMap = false;

  bool enableExportAsciiEdit = false;

  CWorkspaceOperation* wksOp = wxGetApp().GetCurrentWorkspaceOperation();
  if (wksOp != NULL)
  {
    hasOperation = wksOp->HasOperation();
  }
  CWorkspaceDataset* wksDs = wxGetApp().GetCurrentWorkspaceDataset();
  if (wksDs != NULL)
  {
    hasDataset = wksDs->HasDataset();
  }
  if (m_operation != NULL)
  {
    hasUserFormula = m_operation->HasFormula();
    isZFXY = (m_operation->IsZFXY());
    isMap = (m_operation->IsMap());
    enableExportAsciiEdit = wxFile::Exists(m_operation->GetExportAsciiOutputName());

  }

  CFormula* formula = GetOperationtreectrl()->GetCurrentFormula();

  enable = hasOperation && hasDataset;
  enableField = (GetDatasettreectrl()->GetSelection().IsOk() && (GetFieldstreectrl()->GetCount() > 0));
  enableAddExpression = enableField && (GetOperationtreectrl()->GetSelection().IsOk());
  hasOpFieldSelected = (formula != NULL);
  enableDeleteExpression = (formula != NULL);
  enableExecute = enable && enableField && hasUserFormula;

  enableDataType = ((formula != NULL) && !IsFormulaDataField() && !IsFormulaSelectField());

  GetOpnames()->Enable(enable);
  GetDatasettreectrl()->Enable(enable);
  GetFieldstreectrl()->Enable(enableField);

  GetOperationtreectrl()->Enable(enableField);

  GetOpinsertexpr()->Enable(enableAddExpression);

  wxTreeItemId idSelected = GetFieldstreectrl()->GetSelection();
  bool fieldItemhasNoChild = (idSelected.IsOk()) ? !GetFieldstreectrl()->ItemHasChildren(idSelected) : false;
  //GetOpinsertfield()->Enable(hasOpFieldSelected && (fieldItemhasNoChild));
  GetOpinsertfield()->Enable(fieldItemhasNoChild && GetOperationtreectrl()->GetSelection().IsOk());

  GetOpinsertfct()->Enable(hasOpFieldSelected);
  GetOpinsertformula()->Enable(hasOpFieldSelected);
  GetOpinsertalgorithm()->Enable(hasOpFieldSelected);

  wxString formulaText = GetOptextform()->GetValue().Trim();
  enableSaveAs = hasOpFieldSelected && (formulaText.IsEmpty() == false);
  GetOpsaveasformula()->Enable(enableSaveAs);

  GetOpdeleteexpr()->Enable(hasOpFieldSelected);

  GetOpunit()->Enable(hasOpFieldSelected);
  GetOptype()->Enable(enableDataType);

  GetOpDataMode()->Enable( hasOpFieldSelected && IsFormulaDataField() && !IsFormulaSelectField());

  GetOptextform()->Enable(hasOpFieldSelected);
  GetOptitle()->Enable(hasOpFieldSelected);
  GetOpcheckdataf()->Enable(hasOpFieldSelected);
  GetOpexprinfo()->Enable(hasOpFieldSelected);

  bool showFilter = hasOpFieldSelected && IsFormulaDataField() && !IsFormulaSelectField() && isZFXY;
  GetOpFilterlabel()->Show(showFilter);
  GetOpresolution()->Show(isZFXY);
  ShowResolutionAndFilterSizer(isZFXY);

  GetOpnew()->Enable( wxGetApp().IsNewOperationEnable() );
  GetOpdelete()->Enable( wxGetApp().IsDeleteOperationEnable() );
  GetOpdup()->Enable( enable );

  GetOpexport()->Enable( enableExecute );

  GetOpexportasciiedit()->Enable( enableExportAsciiEdit );

  GetOpexecute()->Enable(enableExecute);
  GetOpshowstats()->Enable(enableExecute);
  GetOpdelay()->Enable(enableExecute);
  GetOpaliasinfo()->Enable(enable);

}

//----------------------------------------
void COperationPanel::ShowResolutionAndFilterSizer(bool showIt)
{
  glb_OpPanelRightPane->Show(glb_OpPanelResolutionAndFilterSizer, showIt);
  glb_OpPanelRightPane->Layout();

  if (!showIt)
  {
    return;
  }

  CFormula* xFormula = m_operation->GetFormula(CMapTypeField::typeOpAsX);
  CFormula* yFormula = m_operation->GetFormula(CMapTypeField::typeOpAsY);

  if (xFormula != NULL)
  {
    wxString label = wxString::Format(XRESOLUTION_LABELFORMAT, xFormula->GetName().c_str(), xFormula->GetUnitAsText().c_str());
    (static_cast<wxStaticBoxSizer*>(glb_OpXResolutionSizer))->GetStaticBox()->SetLabel(label);
  }
  if (yFormula != NULL)
  {
    wxString label = wxString::Format(YRESOLUTION_LABELFORMAT, yFormula->GetName().c_str(), yFormula->GetUnitAsText().c_str());
    (static_cast<wxStaticBoxSizer*>(glb_OpYResolutionSizer))->GetStaticBox()->SetLabel(label);
  }

}

//----------------------------------------
bool COperationPanel::IsFormulaDataField()
{
  bool enable = false;
  if (m_operation == NULL)
  {
    return enable;
  }
  if (m_userFormula == NULL)
  {
    return enable;
  }

  return (m_userFormula->GetType() == CMapTypeField::typeOpAsField);
}
//----------------------------------------
bool COperationPanel::IsFormulaSelectField()
{
  bool enable = false;
  if (m_operation == NULL)
  {
    return enable;
  }
  if (m_userFormula == NULL)
  {
    return enable;
  }

  return m_operation->IsSelect(m_userFormula);
}

//----------------------------------------
void COperationPanel::ClearAll()
{
  GetOpnames()->Clear();

  GetDatasettreectrl()->RemoveAllItems();
  ClearFieldsInfo();

  GetOperationtreectrl()->Insert(NULL);

  GetOptextform()->Clear();
  GetOpunit()->Clear();
  GetOptype()->SetSelection(-1);

  Reset();

  SetDataSetAndRecordLabel();

  EnableCtrl();
}

//----------------------------------------
void COperationPanel::LoadDataset()
{
  ClearAll();

  bool bOk = true;

  GetDatasettreectrl()->RemoveAllItems();

  CWorkspaceDataset* wks = wxGetApp().GetCurrentWorkspaceDataset();
  if (wks == NULL)
  {
    return;
  }

  if (wks->GetDatasetCount() <= 0)
  {
    return;
  }

  GetDatasettreectrl()->AddItemsToTree();

  if (m_currentDataset.IsOk())
  {
    m_currentDataset.Unset();
  }



  //GetDatasettreectrl()->Unselect();

  /*
  SetCurrentDataset();

  wxTreeItemIdValue cookie;
  wxTreeItemId id = GetDatasettreectrl()->GetFirstChild(GetDatasettreectrl()->GetRootItem(), cookie);

  DatasetSelChanged(id);
*/
  EnableCtrl();


}
//----------------------------------------
bool COperationPanel::SetCurrentDataset()
{
  if (m_operation == NULL)
  {
    return false;
  }

  m_operation->SetDataset(GetCurrentDataset());
  m_operation->SetProduct(m_product);

  return (m_operation->GetDataset() != NULL);
}
//----------------------------------------
CDataset* COperationPanel::GetCurrentDataset()
{
  CDataset* dataset = NULL;
  CWorkspaceDataset* wks = wxGetApp().GetCurrentWorkspaceDataset();
  if (wks == NULL)
  {
    return NULL;
  }

  if (m_currentDataset.IsOk())
  {
    dataset = wks->GetDataset(GetDatasettreectrl()->GetItemText(m_currentDataset).ToStdString());
  }
  return dataset;
}
//----------------------------------------
void COperationPanel::FillDataTypeList()
{
  GetOptype()->Clear();
  CMapTypeData::GetInstance().NamesToComboBox(*GetOptype(), true);
}
//----------------------------------------
void COperationPanel::FillDataModeList()
{
  GetOpDataMode()->Clear();
  CMapDataMode::GetInstance().NamesToComboBox(*GetOpDataMode());
}

//----------------------------------------
void COperationPanel::LoadOperation()
{
  ClearAll();

  LoadDataset();

  FillFormulaList();

  FillDataTypeList();
  FillDataModeList();

  bool bOk = true;

  CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();
  if (wks == NULL)
  {
    return;
  }

  if (wks->GetOperationCount() <= 0)
  {
    return;
  }

  this->SetCursor(*wxHOURGLASS_CURSOR);

  wks->GetOperationNames(*GetOpnames());

  m_currentOperation = 0;
  GetOpnames()->SetSelection(m_currentOperation);

  SetCurrentOperation();

  GetDataMode();

  GetOperationOutputs();

  GetOperationDataset();

  SetCurrentFormula();

  GetOperationtreectrl()->Unselect();

  EnableCtrl();

  this->SetCursor(wxNullCursor);


}
//----------------------------------------
CFormula* COperationPanel::GetCurrentFormula()
{
  if (m_operation == NULL)
  {
    return NULL;
  }

  return GetOperationtreectrl()->GetCurrentFormula();
}
//----------------------------------------
bool COperationPanel::SetCurrentFormula()
{
  m_userFormula = GetCurrentFormula();
  GetUserFormulaText();
  GetOperationUnit();
  GetOperationDataType();
  GetDataMode();

  SetResolutionLabels();

  return (m_userFormula != NULL);
}

//----------------------------------------
void COperationPanel::SetResolutionLabels()
{
  if (m_operation == NULL)
  {
    return;
  }

  if (m_operation->IsZFXY())
  {
    CFormula* xFormula = m_operation->GetFormula(CMapTypeField::typeOpAsX);
    CFormula* yFormula = m_operation->GetFormula(CMapTypeField::typeOpAsY);


    if (m_userFormula != NULL)
    {
      if (IsFormulaDataField() && !IsFormulaSelectField())
      {
        GetOpFilterlabel()->SetLabel(m_userFormula->GetFilterAsLabel());
      }
      else
      {
        GetOpFilterlabel()->SetLabel("");
      }
    }
    else
    {
      GetOpFilterlabel()->SetLabel("");
    }


    bool hasFilter = m_operation->HasFilters();

    if (xFormula != NULL)
    {
      GetOpXresolutionlabel()->SetLabel(xFormula->GetResolutionAsLabel(hasFilter));
    }
    else
    {
      GetOpXresolutionlabel()->SetLabel("");
    }

    if (yFormula != NULL)
    {
      GetOpYresolutionlabel()->SetLabel(yFormula->GetResolutionAsLabel(hasFilter).c_str());
    }
    else
    {
      GetOpYresolutionlabel()->SetLabel("");
    }
  }

}
//----------------------------------------
void COperationPanel::GetUserFormulaText()
{
  if (m_userFormula == NULL)
  {
    GetOptextform()->Clear();
  }
  else
  {
    GetOptextform()->SetValue(m_userFormula->GetDescription());
    ////////////////GetOpdatatype()->SetSelection(m_userFormula->GetType());
  }



}

//----------------------------------------
void COperationPanel::GetOperationUnit()
{
  GetOpunit()->Clear();

  if (m_operation != NULL)
  {
    if (m_userFormula != NULL)
    {
      GetOpunit()->SetValue(m_userFormula->GetUnitAsText());
    }
  }
}
//----------------------------------------
void COperationPanel::GetOperationDataType()
{
  GetOptype()->SetSelection(-1);
  if (m_operation != NULL)
  {
    if (m_userFormula != NULL)
    {
      GetOptype()->SetStringSelection(m_userFormula->GetDataTypeAsString());
    }
  }
}

//----------------------------------------
wxTreeItemId COperationPanel::GetOperationDataset(bool select /*= true*/)
{
  if (m_operation == NULL)
  {
    if (select)
    {
      if (m_currentDataset.IsOk())
      {
        m_currentDataset.Unset();
      }

      GetDatasettreectrl()->Unselect();
    }
    return m_currentDataset;
  }

  wxTreeItemId id =  GetDatasettreectrl()->FindItem(m_operation->GetDatasetName());

  if (select)
  {
    if (id.IsOk())
    {
      //m_currentDataset = id;
      GetDatasettreectrl()->SelectItem(id);
#if defined(__WXMAC__) || defined (wxUSE_UNIX)
      wxTreeEvent ev(wxEVT_COMMAND_TREE_SEL_CHANGED, GetDatasettreectrl(), id);
      wxPostEvent(this, ev);
#endif
    }
  }

  //SetCurrentDataset();
  return id;

}

//----------------------------------------
void COperationPanel::GetDataMode()
{
  if (m_operation == NULL)
  {
    return;
  }

  CFormula* formula = GetCurrentFormula();
  if (formula == NULL)
  {
    return;
  }

  // if same pointer
  if (IsCriteriaSelection(formula))
  {
    return;
  }

  GetOpDataMode()->SetStringSelection(formula->GetDataModeAsString());
}
//----------------------------------------
bool COperationPanel::IsCurrentCriteriaSelection()
{
  return IsCriteriaSelection(GetCurrentFormula());
}
//----------------------------------------
bool COperationPanel::IsCriteriaSelection(CFormula* formula)
{
  if (m_operation == NULL)
  {
    return false;
  }

  if (formula == NULL)
  {
    return false;
  }

  // pointer comparison
  return (m_operation->GetSelect() == formula);

}

//----------------------------------------
COperation* COperationPanel::GetCurrentOperation()
{
  CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();
  if (wks == NULL)
  {
    return NULL;
  }

  return wks->GetOperation(GetOpnames()->GetString(m_currentOperation).ToStdString());
}
//----------------------------------------
bool COperationPanel::SetCurrentOperation()
{
  m_operation = GetCurrentOperation();

  GetOperationtreectrl()->Insert(m_operation);

  return (m_operation!= NULL);
}
//----------------------------------------
void COperationPanel::Reset()
{
//  m_currentRecord = -1;
  if (m_currentDataset.IsOk())
  {
    m_currentDataset.Unset();
  }

  if (m_currentRecord.IsOk())
  {
    m_currentRecord.Unset();
  }

  m_currentOperation = -1;
//  m_currentUserForm = -1;
  DeleteProduct();
  m_operation = NULL;  // must be after DeleteProduct
  m_userFormula = NULL;
  m_opNameDirty = false;
//  m_opUserFormDirty = false;


}
//----------------------------------------
void COperationPanel::DeleteProduct()
{
  if (m_product != NULL)
  {
    delete m_product;
    m_product = NULL;
  }

  m_productClass.Empty();
  m_productType.Empty();


}
//----------------------------------------
void COperationPanel::DatasetSelChanged( const wxTreeItemId& id )
{
  ClearFieldsInfo();

  CDataset* dataset = GetDatasettreectrl()->GetDataset(id);
  if (dataset == NULL)
  {
    return;
  }

  try
  {
    m_product = dataset->SetProduct();

    if (m_product != NULL)
    {
      m_productClass = m_product->GetProductClass().c_str();
      m_productType = m_product->GetProductType().c_str();
    }

  }
  catch (CException& e)
  {
    wxMessageBox(wxString::Format("Unable to process files.\nReason:\n%s",
                                  e.what()),
                "Warning",
                wxOK | wxICON_EXCLAMATION);

    m_product = NULL;

  }

  if (m_product == NULL)
  {
    wxMessageBox("Unable to set Product\nPerhaps dataset file list is empty or or product file doesn't exist()",
                "Warning",
                wxOK | wxICON_EXCLAMATION);

    DeleteProduct();

    this->SetCursor(wxNullCursor);
    return;
  }


  GetFieldstreectrl()->InsertProduct(m_product);

  wxTreeItemId rootIdFields = GetFieldstreectrl()->GetRootItem();


  int32_t nRecords = (rootIdFields.IsOk()) ? GetFieldstreectrl()->GetChildrenCount(rootIdFields, false) : 0;

  if ( (nRecords > 0) && (nRecords <= 2))
  {
    wxTreeItemIdValue cookie;
    wxTreeItemId idChild = GetFieldstreectrl()->GetFirstChild(rootIdFields, cookie);
    if (nRecords == 2)
    {
      idChild = GetFieldstreectrl()->GetNextChild(rootIdFields, cookie);
    }
    GetFieldstreectrl()->Expand(idChild);
  }


}
//----------------------------------------
bool COperationPanel::CtrlOperation(std::string& msg, bool basicControl /* = false */, const CStringMap* aliases /* = NULL*/)
{
  if (m_operation == NULL)
  {
    return false;
  }

  bool bOk = m_operation->Control(msg, basicControl, aliases);

  if (bOk)
  {
    if (m_userFormula != NULL)
    {
      GetOptextform()->SetValue(m_userFormula->GetDescription());
    }
  }

  return bOk;

}

//----------------------------------------
bool COperationPanel::BuildCmdFile()
{
  if (m_operation == NULL)
  {
    return false;
  }

  return m_operation->BuildCmdFile();

}
//----------------------------------------
bool COperationPanel::BuildExportAsciiCmdFile()
{
  if (m_operation == NULL)
  {
    return false;
  }

  return m_operation->BuildExportAsciiCmdFile();

}
//----------------------------------------
bool COperationPanel::BuildShowStatsCmdFile()
{
  if (m_operation == NULL)
  {
    return false;
  }

  return m_operation->BuildShowStatsCmdFile();

}
//----------------------------------------
void COperationPanel::SaveCurrentFormulaAs()
{
  CFormula* formula = GetOperationtreectrl()->GetCurrentFormula();
  SaveFormulaAs(formula);
}
//----------------------------------------
void COperationPanel::SaveFormulaAs(CFormula* formula)
{
  if (formula == NULL)
  {
    return;
  }

  CFormula newFormula(*formula);

  wxString title;
  if (COperation::IsSelect(formula->GetName()))
  {
    title = "Save 'select' as...";
  }
  else
  {
    title = "Save 'formula' as...";
  }

  CFormulaDlg dlg (this, -1, title,
                   &newFormula);

  int32_t result = dlg.ShowModal();
  if (result != wxID_OK)
  {
    return;
  }

  AddFormula(newFormula);

  FillFormulaList();

}
//----------------------------------------
void COperationPanel::AddFormulaComment(CFormula* formula)
{

  wxString title = wxString::Format("Set a title for '%s'...", formula->GetName().c_str());

  CFormulaDlg dlg (this, -1, title,
                   formula);

  dlg.GetFormulaName()->Enable(false);
  dlg.GetFormulaValue()->Enable(false);
  dlg.GetFormulaUnit()->Enable(false);
  int32_t result = dlg.ShowModal();
  if (result != wxID_OK)
  {
    return;
  }

  formula->SetTitle(dlg.GetFormulaTitle()->GetValue().ToStdString());
  formula->SetComment(dlg.GetFormulaComment()->GetValue().ToStdString());

}

//----------------------------------------
void COperationPanel::AddFormula(CFormula& formula)
{
  CWorkspaceFormula* wks = wxGetApp().GetCurrentWorkspaceFormula();
  if (wks == NULL)
  {
    return;
  }

  wks->AddFormula(formula);
}

//----------------------------------------
// WDR: handler implementations for COperationPanel

//----------------------------------------
void COperationPanel::OnShowAliases( wxCommandEvent &event )
{
  if (m_operation == NULL)
  {
    return;
  }

  if (m_product == NULL)
  {
    return;
  }

  wxString title = wxString::Format("Show '%s' aliases...", m_product->GetProductClassType().c_str());

  CFormula* formula = GetOperationtreectrl()->GetCurrentFormula();
  bool hasOpFieldSelected = (formula != NULL);

  CAliasInfoDlg dlg (this, -1, title, m_operation, formula);

  dlg.ShowModal();

  
  if (hasOpFieldSelected)
  {
    wxGrid* aliasesGrid = dlg.GetAliasinfoGrid();
    wxString text;
    int32_t numRows = aliasesGrid->GetNumberRows();
    
    bool hasSelectedItems = false;

    for (int32_t i = 0 ; i < numRows ; i++)
    {
      wxString isSeleted = aliasesGrid->GetCellValue(i, CAliasInfoDlg::USE_COL);
      if (isSeleted.compare("1") == 0)
      {
        hasSelectedItems = true;
        text.Append(aliasesGrid->GetCellValue(i, CAliasInfoDlg::SYNTAX_COL));
        text.Append(" ");
      }

    }
    if (hasSelectedItems)
    {
      GetOptextform()->WriteText(text);
      SetTextFormula();
    }
  }

}
//----------------------------------------
void COperationPanel::OnLaunchScheduler( wxCommandEvent &event )
{
  ::wxExecute(wxGetApp().GetExecBratSchedulerName()->GetFullPath());        
}
//----------------------------------------
void COperationPanel::OnDelay( wxCommandEvent &event )
{
  try
  {
    Delay();
  }
  catch(CException& e)
  {
    wxMessageBox(e.GetMessage().c_str(),
                   "Warning",
                    wxOK | wxCENTRE | wxICON_EXCLAMATION);    
  }
    
}
//----------------------------------------

void COperationPanel::OnInsertAlgorithm( wxCommandEvent &event )
{
  InsertAlgorithm();
}

//----------------------------------------
void COperationPanel::OnChangeDefaultRecord( wxCommandEvent &event )
{
  if (m_product == NULL)
  {
    return;
  }

  CSelectRecord recordDlg (this, m_product, -1, "Select a record...");

  wxString recordName;
  int32_t result = wxID_CANCEL;
  do
  {
    result = recordDlg.ShowModal();

    recordName = recordDlg.GetRecordName();

  }
  while ((result == wxID_OK) && (recordName.IsEmpty()));

  if (result != wxID_OK)
  {
    return;
  }

  m_operation->SetRecord(recordName);

  SetDataSetAndRecordLabel();

}
//----------------------------------------
void COperationPanel::OnShowStats( wxCommandEvent &event )
{
  ShowStats();

}
//----------------------------------------

void COperationPanel::OnShowExprInfo( wxCommandEvent &event )
{
  if (m_operation == NULL)
  {
    return;
  }

  if (m_product == NULL)
  {
    return;
  }


  if (! GetOptextform()->GetValue().IsEmpty())
  {
    CheckExpression(GetOptextform());
  }

  const CStringMap* fieldAliases = m_product->GetAliasesAsString();
  std::string errorString;

  wxString exprValue = CTools::ExpandVariables((const char *)GetOptextform()->GetValue().c_str(), &m_mapFormulaString, fieldAliases, true, '%', NULL, false, &errorString).c_str();
  
  if (!errorString.empty())
    {
    wxString msg = wxString::Format("Unable to expand aliases (formulas and fields) from:\n%s\n.\nReason\n'%s'",
                                    GetOptextform()->GetValue().c_str(),
                                    errorString.c_str());    
    wxMessageBox(msg,
                 "Warning",
                 wxOK | wxCENTRE | wxICON_EXCLAMATION);    

      return;
    }


  wxString title = wxString::Format("Show '%s' information...", m_userFormula->GetName().c_str());

  CExpressionInfoDlg dlg (this, -1, title, exprValue, m_operation, m_userFormula);

  dlg.ShowModal();

}
//----------------------------------------
void COperationPanel::OnPositionChanging( wxSplitterEvent &event )
{
/*  if (event.GetSashPosition() > GetMinimumPaneSize())
  {
    event.Veto();
  }
*/
}
//----------------------------------------
void COperationPanel::OnComboDataType( wxCommandEvent &event )
{
  if (m_operation == NULL)
  {
    return;
  }
  if (m_userFormula == NULL)
  {
    return;
  }
  m_userFormula->SetDataType( CMapTypeData::GetInstance().NameToId(GetOptype()->GetStringSelection().ToStdString()));
  m_userFormula->SetDefaultMinMax();
  m_userFormula->SetStepToDefault();
  GetOpunit()->SetValue(m_userFormula->GetDefaultUnit());


  //m_userFormula->SetDefaultUnit();
  SetUnitText();

/*
  if (   (m_userFormula->IsXYType())
      && (m_operation->GetType() == CMapTypeOp::typeOpZFXY) )
  {
    wxCommandEvent ev(wxEVT_COMMAND_BUTTON_CLICKED, ID_OPOPTIONF);
    wxPostEvent(this, ev);
  }

*/
}

//----------------------------------------
void COperationPanel::OnExecute( wxCommandEvent &event )
{
  Execute();
}

//----------------------------------------
void COperationPanel::OnComboOperationTextEnter( wxCommandEvent &event )
{
  OpNameChanged();
}

//----------------------------------------
void COperationPanel::OnComboOperation( wxCommandEvent &event )
{
  ComboOperation(event.GetInt());
}

//----------------------------------------
void COperationPanel::ComboOperation( int32_t currentOperation )
{
  m_currentOperation = currentOperation;
  ComboOperation();
}

//----------------------------------------
void COperationPanel::ComboOperation()
{
  Freeze();

  SetCurrentOperation();

  GetDataMode();

  GetOperationOutputs();

  GetOperationDataset();

  //GetOperationRecord();

  SetCurrentFormula();

  GetOperationtreectrl()->Unselect();

  GetOpnames()->SetFocus();

  EnableCtrl();

  Thaw();

}


//----------------------------------------
void COperationPanel::OnComboDataMode( wxCommandEvent &event )
{
  if (m_operation == NULL)
  {
    return;
  }


  CFormula* formula = GetCurrentFormula();
  if (formula == NULL)
  {
    return;
  }

  // if same pointer
  if (IsCriteriaSelection(formula))
  {
    return;
  }

  uint32_t id   = CMapDataMode::GetInstance().NameToId(GetOpDataMode()->GetValue().ToStdString());
  formula->SetDataMode(id);

}

//----------------------------------------
void COperationPanel::OnTitle( wxCommandEvent &event )
{
  if (m_userFormula == NULL)
  {
    return;
  }

  AddFormulaComment(m_userFormula);
}

//----------------------------------------
void COperationPanel::OnCheckSyntax( wxCommandEvent &event )
{
  CheckExpression(GetOptextform());
}

//----------------------------------------
void COperationPanel::OnDeleteExpression( wxCommandEvent &event )
{
  DeleteExpression();
}

//----------------------------------------
void COperationPanel::OnSaveAsFormula( wxCommandEvent &event )
{
  SaveFormulaAs(m_userFormula);
}

//----------------------------------------
void COperationPanel::OnInsertFormula( wxCommandEvent &event )
{
  InsertFormula();
}

//----------------------------------------
void COperationPanel::OnInsertFunction( wxCommandEvent &event )
{
  InsertFunction();
}

//----------------------------------------
void COperationPanel::OnInsertField( wxCommandEvent &event )
{
  InsertField();
}

//----------------------------------------
void COperationPanel::OnInsertExpression( wxCommandEvent &event )
{
  InsertEmptyExpression();
}

//----------------------------------------
void COperationPanel::OnEditAsciiExportOperation( wxCommandEvent &event )
{
  wxString title = wxString::Format("%s", m_operation->GetExportAsciiOutputName().c_str());
  CRichTextFrame* frame = new CRichTextFrame(this, title);

  wxFFile wxffile(m_operation->GetExportAsciiOutputName());

  wxString content;
  wxffile.ReadAll(&content);
  frame->GetTextCtrl()->SetValue(content);
  //frame->GetTextCtrl()->LoadFile(m_operation->GetExportAsciiOutputName());

  frame->Show(true);
  frame->GetTextCtrl()->SetInsertionPoint(0);

  EnableCtrl();
}

//----------------------------------------
void COperationPanel::OnExportOperation( wxCommandEvent &event )
{
  try
  {
    ExportOperation();
  }
  catch(CException& e)
  {
    wxMessageBox(e.GetMessage().c_str(),
                   "Warning",
                    wxOK | wxCENTRE | wxICON_EXCLAMATION);    
  }
}

//----------------------------------------
void COperationPanel::OnDeleteOperation( wxCommandEvent &event )
{
  DeleteOperation();
}

//----------------------------------------
void COperationPanel::OnDuplicateOperation( wxCommandEvent &event )
{
  DuplicateOperation();
}

//----------------------------------------
void COperationPanel::OnNewOperation( wxCommandEvent &event )
{
  NewOperation();
}
//----------------------------------------
void COperationPanel::OnOperationSelChanged( wxTreeEvent &event )
{
  SetCurrentFormula();
  EnableCtrl();
}
//----------------------------------------
void COperationPanel::OnFieldSelChanged( wxTreeEvent &event )
{
  EnableCtrl();

}

//----------------------------------------
void COperationPanel::OnDatasetSelChanging( wxTreeEvent &event )
{
  wxTreeItemId id = event.GetItem();

  if (!id)
  {
    event.Veto();
    return;
  }
  // Apparently some versions of the MS common controls does not send the events
  // when a tree item is selected programatically, so wxWidgets sends it.  Other
  // versions of the common controls do send the events and so you get it twice.
  if (m_currentDataset == id)
  {
    if (GetFieldstreectrl()->GetRecordCount() > 0)
    {
      SetCurrentDataset();
      event.Veto();
      return;
    }
  }

  if (m_operation == NULL)
  {
    event.Veto();
    return;
  }

  CDataset* dataset = GetDatasettreectrl()->GetDataset(id);
  if (dataset == NULL)
  {
    event.Veto();
    return;
  }


  wxString datasetNameOperation =  m_operation->GetDatasetName();
  wxString datasetName = dataset->GetName();
  bool resetDataExpression = m_operation->HasFormula() && (datasetNameOperation.CmpNoCase(datasetName) != 0);

  if (resetDataExpression)
  {
    int32_t result =  wxMessageBox(wxString::Format("Some data expressions are built from '%s' dataset.\n"
                                              "If you want to use another dataset, make sure that field names in data expressions are the same, or don't forget to change them.\n"
                                              "Change to '%s' dataset ?",
                                                    m_operation->GetDatasetName().c_str(),
                                                    GetDatasettreectrl()->GetItemText(id).c_str()),
                                 "Changing operation dataset",
                                  wxYES_NO | wxCENTRE | wxICON_QUESTION);

    if (result != wxYES)
    {
      event.Veto();
      return;
    }

    m_operation->SetRecord("");

    CProduct* product = NULL;

    try
    {
      product = dataset->SetProduct();
    }
    catch (CException& e)
    {
      wxMessageBox(wxString::Format("Unable to process files.\nReason:\n%s",
                                    e.what()),
                  "Warning",
                  wxOK | wxICON_EXCLAMATION);

      product = NULL;
    }

    if (product == NULL)
    {
      wxMessageBox("Unable to set Product\nPerhaps dataset file list is empty or or product file doesn't exist()",
                  "Warning",
                  wxOK | wxICON_EXCLAMATION);

      event.Veto();
      return;
    }

    bool bOk = GetOperationtreectrl()->SelectRecord(product);

    delete product;
    product = NULL;

    if (!bOk)
    {
      wxMessageBox("You have not selected a record name.\n"
                   "Dataset has not been changed.\n",
                   "Changing operation dataset is cancelled",
                    wxOK | wxCENTRE | wxICON_WARNING);

      event.Veto();
      return;
    }

  }

}

//----------------------------------------
void COperationPanel::OnDatasetSelChanged( wxTreeEvent &event )
{

  wxTreeItemId id = event.GetItem();

  // Apparently some versions of the MS common controls does not send the events
  // when a tree item is selected programatically, so wxWidgets sends it.  Other
  // versions of the common controls do send the events and so you get it twice.
  //if (m_currentDataset == id)
  //{
  //  return;
  //}



  DatasetSelChanged(id);

  m_currentDataset = id;
  SetCurrentDataset();

  // If there is only one record in the dataset ==> select it
  GetOperationRecord();
  SetCurrentRecord();

  SetDataSetAndRecordLabel();


  EnableCtrl();

}


//----------------------------------------
void COperationPanel::SetDataSetAndRecordLabel()
{
  if (m_product == NULL)
  {
    GetOperationtreectrllabel()->SetLabel("");
    GetOpchangerecord()->Enable(false);
    return;
  }

  if (m_operation == NULL)
  {
    GetOperationtreectrllabel()->SetLabel("");
    GetOpchangerecord()->Enable(false);
    return;
  }


  wxString strFormat = "Product: %s/%s\nDataset: %s\nDefault record: %s";

  if (CProductNetCdf::IsProductNetCdf(m_product))
  {
    strFormat = "Product: %s/%s\nDataset: %s";
  }

  wxString str;

  wxString datasetName = "?";
  wxString recordName = "?";

  if (!m_operation->GetDatasetName().empty())
  {
    datasetName =  m_operation->GetDatasetName();
  }
  if (!m_operation->GetRecord().empty())
  {
    recordName =  m_operation->GetRecord();
  }

  if (CProductNetCdf::IsProductNetCdf(m_product))
  {
    GetOpchangerecord()->Enable(false);
    str.Printf(strFormat, m_productClass.c_str(), m_productType.c_str(), datasetName.c_str());
  }
  else
  {
    GetOpchangerecord()->Enable(true);
    str.Printf(strFormat, m_productClass.c_str(), m_productType.c_str(), datasetName.c_str(), recordName.c_str());
  }

  GetOperationtreectrllabel()->SetLabel(str);
}

//----------------------------------------
bool COperationPanel::SetCurrentRecord()
{

  if (m_operation == NULL)
  {
    return false;
  }


  m_operation->SetRecord(GetFieldstreectrl()->GetCurrentRecord());

  /*
  InitDefaultFormulaValue("longitude", CMapTypeField::typeOpAsX);
  InitDefaultFormulaValue("lon_tra", CMapTypeField::typeOpAsX);
  InitDefaultFormulaValue("lon", CMapTypeField::typeOpAsX);

  InitDefaultFormulaValue("latitude", CMapTypeField::typeOpAsY);
  InitDefaultFormulaValue("lat_tra", CMapTypeField::typeOpAsY);
  InitDefaultFormulaValue("lat", CMapTypeField::typeOpAsY);
*/
  EnableCtrl();

  return (m_operation->GetRecord().IsEmpty() == false);

}

//----------------------------------------
wxTreeItemId COperationPanel::GetOperationRecord(bool select)
{
  wxTreeItemId id;

  if (m_operation == NULL)
  {
    return id;
  }
  id = GetFieldstreectrl()->FindItem(m_operation->GetRecord());


  if (select)
  {
    m_currentRecord = id;
    // if no selection (current record < 0) and only one record in the list, select it
    if ( (!m_currentRecord) && (GetFieldstreectrl()->GetRecordCount() == 1) )
    {
      m_currentRecord = GetFieldstreectrl()->GetFirstRecordId() ;
    }

    GetFieldstreectrl()->SelectItem(m_currentRecord);
    GetFieldstreectrl()->Expand(m_currentRecord);
  }

  return id;

}

//----------------------------------------

void COperationPanel::OnResolution( wxCommandEvent &event )
{
  Resolution();
}
//----------------------------------------
void COperationPanel::Resolution()
{
  if (m_operation == NULL)
  {
    return;
  }

  wxString title = "Set X/Y resolution...";

  if (m_userFormula != NULL)
  {
    if (m_userFormula->GetType() == CMapTypeField::typeOpAsField)
    {
      title = wxString::Format("Set filter for '%s' and X/Y resolution...", m_userFormula->GetName().c_str());
    }
  }

  CResolutionDlg dlg (this, -1, title,
                      m_operation,
                      m_userFormula,
                      m_mapFormulaString);


  int32_t result = dlg.ShowModal();

  if (result != wxID_OK)
  {
    return;
  }
  /*
  if (m_userFormula != NULL)
  {
    m_userFormula->SetFilter(dlg.GetFormulaoptFilter()->GetSelection());
  }


  CFormula* xFormula = m_operation->GetFormula(CMapTypeField::typeOpAsX);

  if (xFormula != NULL)
  {
    double doubleValue;
    int32_t intValue;
    wxString stringValue;

    if (xFormula->IsTimeDataType())
    {
      dlg.GetFormulaoptXmin()->GetValueAsDate(doubleValue, 0.0, 0.0);
      xFormula->SetMinValue(doubleValue);

      dlg.GetFormulaoptXmax()->GetValueAsDate(doubleValue, CDate::m_secInDay);
      xFormula->SetMaxValue(doubleValue);
    }
    else
    {
      dlg.GetFormulaoptXmin()->GetValue(doubleValue, CTools::m_defaultValueDOUBLE);
      xFormula->SetMinValue(doubleValue);

      dlg.GetFormulaoptXmax()->GetValue(doubleValue, CTools::m_defaultValueDOUBLE);
      xFormula->SetMaxValue(doubleValue);
    }

    dlg.GetFormulaoptXstep()->GetValue(stringValue, xFormula->GetDefaultStep());
    xFormula->SetStep(stringValue);

    dlg.GetFormulaoptXinterval()->GetValue(intValue, CTools::m_defaultValueINT32);
    xFormula->SetInterval(intValue);

    dlg.GetFormulaoptXloesscut()->GetValue(intValue, CTools::m_defaultValueINT32);
    xFormula->SetLoessCutOff(intValue);

  }

  CFormula* yFormula = m_operation->GetFormula(CMapTypeField::typeOpAsY);

  if (yFormula != NULL)
  {
    double doubleValue;
    int32_t intValue;
    wxString stringValue;

    if (yFormula->IsTimeDataType())
    {
      dlg.GetFormulaoptYmin()->GetValueAsDate(doubleValue, 0.0, 0.0);
      yFormula->SetMinValue(doubleValue);

      dlg.GetFormulaoptYmax()->GetValueAsDate(doubleValue, CDate::m_secInDay);
      yFormula->SetMaxValue(doubleValue);
    }
    else
    {
      dlg.GetFormulaoptYmin()->GetValue(doubleValue, CTools::m_defaultValueDOUBLE);
      yFormula->SetMinValue(doubleValue);

      dlg.GetFormulaoptYmax()->GetValue(doubleValue, CTools::m_defaultValueDOUBLE);
      yFormula->SetMaxValue(doubleValue);
    }

    dlg.GetFormulaoptYstep()->GetValue(stringValue, yFormula->GetDefaultStep());
    yFormula->SetStep(stringValue);

    dlg.GetFormulaoptYinterval()->GetValue(intValue, CTools::m_defaultValueINT32);
    yFormula->SetInterval(intValue);

    dlg.GetFormulaoptYloesscut()->GetValue(intValue, CTools::m_defaultValueINT32);
    yFormula->SetLoessCutOff(intValue);

  }
*/
  SetResolutionLabels();

}

//----------------------------------------
void COperationPanel::OnTextFormulaKillFocus( wxFocusEvent &event )
{
  SetTextFormula();
  event.Skip();
}
//----------------------------------------
void COperationPanel::OnTextFormulaEnter( wxCommandEvent &event )
{
  SetTextFormula();
  event.Skip();

}
//----------------------------------------

void COperationPanel::OnTextFormula( wxCommandEvent &event )
{
  SetTextFormula();

}

//----------------------------------------
void COperationPanel::SetTextFormula()
{
  if (m_operation == NULL)
  {
    return;
  }

  if (m_userFormula == NULL)
  {
    return;
  }

  m_userFormula->SetDescription(GetOptextform()->GetValue());

  OnFormulaChanged(m_userFormula);

  EnableCtrl();
}
//----------------------------------------
void COperationPanel::OnFormulaChanged(CFormula* formula)
{
  if (m_operation == NULL)
  {
    return;
  }

  if (formula == NULL)
  {
    return;
  }

  GetOperationtreectrl()->OnFormulaChanged( formula );
}
//----------------------------------------
void COperationPanel::NewOperation()
{

  bool bOk = true;

  CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();
  if (wks == NULL)
  {
    return;
  }
  this->SetCursor(*wxHOURGLASS_CURSOR);

  FillFormulaList();

  FillDataTypeList();
  FillDataModeList();

  GetOpnames()->Enable(true);

  wxString opName = wks->GetOperationNewName();

  bOk = wks->InsertOperation(opName.ToStdString());

  if (bOk == false)
  {
    wxMessageBox(wxString::Format("Operation '%s' already exists", opName.c_str()),
                "Warning",
                wxOK | wxICON_EXCLAMATION);

    GetOpnames()->SetStringSelection(opName);
    m_currentOperation = GetOpnames()->GetSelection();
  }
  else
  {
    m_currentOperation = GetOpnames()->Append(opName);
    GetOpnames()->SetSelection(m_currentOperation);
  }

  SetCurrentOperation();

  GetDataMode();

  InitOperationOutputs();
  GetOperationOutputs();

  GetOperationDataset();

  GetOperationtreectrl()->SelectItem(GetOperationtreectrl()->GetXRootId());

  SetCurrentFormula();

  SetCurrentDataset();

  // If there is only one record in the dataset ==> select it
  GetOperationRecord();
  SetCurrentRecord();

  SetDataSetAndRecordLabel();

  EnableCtrl();

  wxGetApp().GetDisplayPanel()->EnableCtrl();

  //CNewOperationEvent ev(GetId(), opName);
  //wxPostEvent(GetParent(), ev);
  this->SetCursor(wxNullCursor);


}

//----------------------------------------
void COperationPanel::DuplicateOperation()
{

  bool bOk = true;


  CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();
  if (wks == NULL)
  {
    return;
  }

  if (m_currentOperation < 0)
  {
    return;
  }

 // COperation* operation = GetCurrentOperation();

  if (m_operation == NULL)
  {
    return;
  }


  wxString opName = wks->GetOperationCopyName(m_operation->GetName().ToStdString());

  bOk = wks->InsertOperation(opName.ToStdString(), m_operation);

  if (bOk == false)
  {
    wxMessageBox(wxString::Format("Operation '%s' already exists", opName.c_str()),
                "Warning",
                wxOK | wxICON_EXCLAMATION);

    GetOpnames()->SetStringSelection(m_operation->GetName());
    m_currentOperation = GetOpnames()->GetSelection();
  }
  else
  {
    m_currentOperation = GetOpnames()->Append(opName);
    GetOpnames()->SetSelection(m_currentOperation);
  }

  ComboOperation();

}
//----------------------------------------
void COperationPanel::DeleteOperation()
{

  bool bOk = true;


  CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();
  if (wks == NULL)
  {
    return;
  }

  if (m_currentOperation < 0)
  {
    return;
  }

 // COperation* operation = GetCurrentOperation();

  if (m_operation == NULL)
  {
    return;
  }

  int32_t result = wxMessageBox(wxString::Format("Are you sure to delete operation '%s' ?\nContinue ?",
                                                  m_operation->GetName().c_str()),
                               "Warning",
                                wxYES_NO | wxCENTRE | wxICON_QUESTION);

  if (result != wxYES)
  {
    return;
  }

  CStringArray displayNames;
  bOk = wxGetApp().CanDeleteOperation(m_operation->GetName(), &displayNames);

  if (bOk == false)
  {
    std::string str = displayNames.ToString("\n", false);
    wxMessageBox(wxString::Format("Unable to delete operation '%s'.\nIt is used by the views below:\n%s\n",
                                  m_operation->GetName().c_str(),
                                  str.c_str()),
                "Warning",
                wxOK | wxICON_EXCLAMATION);
    return;

  }

  wxString opName = m_operation->GetName();

  RemoveOutput();

  bOk = wks->DeleteOperation(m_operation);

  if (bOk == false)
  {
    wxMessageBox(wxString::Format("Unable to delete operation '%s'", m_operation->GetName().c_str()),
                "Warning",
                wxOK | wxICON_EXCLAMATION);
    return;
  }

  m_operation = NULL;
  m_userFormula = NULL;


  GetOpnames()->Delete(m_currentOperation);

  m_currentOperation = (GetOpnames()->GetCount() > 0) ? 0 : -1;

  GetOpnames()->SetSelection(m_currentOperation);

  SetCurrentOperation();

  GetDataMode();

  InitOperationOutputs();
  GetOperationOutputs();

  GetOperationDataset();

  GetOperationtreectrl()->SelectItem(GetOperationtreectrl()->GetXRootId());

  SetCurrentFormula();

  SetCurrentDataset();


  GetOperationRecord();
  SetCurrentRecord();

  SetDataSetAndRecordLabel();

  EnableCtrl();

  CDeleteOperationEvent ev(GetId(), opName);
  wxPostEvent(GetParent(), ev);

  EnableCtrl();
}

//----------------------------------------
void COperationPanel::ComputeInterval()
{
  if (!m_operation->IsZFXY())
  {
    return;
  }

  CFormula* xFormula = m_operation->GetFormula(CMapTypeField::typeOpAsX);
  CFormula* yFormula = m_operation->GetFormula(CMapTypeField::typeOpAsY);

  ComputeInterval(xFormula);
  ComputeInterval(yFormula);

}
//----------------------------------------
void COperationPanel::ComputeInterval(CFormula* formula)
{
  if (!m_operation->IsZFXY())
  {
    return;
  }

  m_operation->ComputeInterval(formula);
  SetResolutionLabels();
}
//----------------------------------------
void COperationPanel::ComputeInterval(const wxString& formulaName)
{
  if (!m_operation->IsZFXY())
  {
    return;
  }

  m_operation->ComputeInterval(formulaName);
  SetResolutionLabels();
}

//----------------------------------------
void COperationPanel::OnDatasetFilesChanged(CDatasetFilesChangeEvent& event)
{
  if (m_product == NULL)
  {
    return;
  }

  CDataset* dataset = GetCurrentDataset();
  if (dataset == NULL)
  {
    return;
  }

  wxString eventDatasetName = event.m_datasetName;
  if ( !str_icmp( dataset->GetName(), event.m_datasetName.ToStdString() ) )
    return;

  wxString errorMsg;
  bool bOk = true;

  try
  {
    m_product->SetProductList(event.m_files, false);
    m_productClass = event.m_files.m_productClass.c_str();
    m_productType = event.m_files.m_productType.c_str();

  }
  catch (CException &e)
  {
    errorMsg = e.what();
    bOk = false;
  }
  catch (std::exception &e)
  {
    errorMsg = e.what();
    bOk = false;
  }
  catch (...)
  {
    errorMsg = " Unexpected error while getting product (runtime error).";
    bOk = false;
  }


  if (!bOk)
  {
    wxMessageBox(wxString::Format("A error occurred while setting dataset file in the current operation - Reason:\n%s",
                                  errorMsg.c_str()),
                "ERROR",
                wxOK | wxICON_ERROR);
    return;
  }


}

//----------------------------------------
void COperationPanel::OnDataExprNew(CNewDataExprEvent& event)
{
  SetDataSetAndRecordLabel();

  //ComputeInterval(event.m_formulaName);
  ComputeInterval();

  EnableCtrl();
}
//----------------------------------------
void COperationPanel::OnDataExprDeleted(CDeleteDataExprEvent& event)
{
  SetCurrentFormula();
  SetDataSetAndRecordLabel();
  EnableCtrl();
}

//----------------------------------------
void COperationPanel::InsertEmptyExpression()
{
  if (m_operation == NULL)
  {
    return;
  }


  bool bOk = true;

  wxTreeItemId id = GetOperationtreectrl()->GetSelection();

  GetOperationtreectrl()->Add(id);

  SetCurrentFormula();

  EnableCtrl();

}
//----------------------------------------
void COperationPanel::InsertFormula()
{
  if (m_operation == NULL)
  {
    return;
  }


  GetFormulaSyntax(GetOptextform(), GetOpunit());
  SetTextFormula();
  SetUnitText();
}
//----------------------------------------
void COperationPanel::InsertAlgorithm()
{
   if (m_operation == NULL)
  {
    return;
  }


  GetAlgorithmSyntax(GetOptextform(), GetOpunit());
  SetTextFormula();
  SetUnitText();
    
}

//----------------------------------------
void COperationPanel::InsertFunction()
{
  if (m_operation == NULL)
  {
    return;
  }


  GetOptextform()->WriteText(GetFunctionSyntax());
  SetTextFormula();

}
//----------------------------------------
void COperationPanel::InsertField()
{
  if (m_operation == NULL)
  {
    return;
  }

  CField* field = GetFieldstreectrl()->GetCurrentField();

  InsertField(field);

}
//----------------------------------------
void COperationPanel::InsertField(CField* field)
{
  if (m_operation == NULL)
  {
    return;
  }
  if (field == NULL)
  {
    return;
  }

  CFormula* formula = GetOperationtreectrl()->GetCurrentFormula();

  if (formula == NULL)
  {
    GetOperationtreectrl()->Add(GetOperationtreectrl()->GetSelection(), field);
  }
  else
  {
    if (m_operation->GetRecord().compare(field->GetRecordName().c_str()) != 0)
    {
      GetOptextform()->WriteText(field->GetFullNameWithRecord().c_str());
    }
    else
    {
      GetOptextform()->WriteText(field->GetFullName().c_str());
    }

    SetTextFormula();
  }

  EnableCtrl();
}

//----------------------------------------
void COperationPanel::DeleteExpression()
{
  if (m_operation == NULL)
  {
    return;
  }


  GetOperationtreectrl()->DeleteCurrentFormula();

  //SetCurrentFormula();	[FEMM: commented out assuming GetOperationtreectrl()->DeleteCurrentFormula() 
  //always posts the event that OnDataExprDeleted processes, calling SetCurrentFormula()]

  EnableCtrl();
}
//----------------------------------------
void COperationPanel::RenameExpression()
{
  if (m_operation == NULL)
  {
    return;
  }

  wxTreeItemId id = GetOperationtreectrl()->GetSelection();
  GetOperationtreectrl()->EditLabel(id);

  EnableCtrl();
}


//----------------------------------------
void COperationPanel::FieldAttrChangeUnit()
{
  if (m_operation == NULL)
  {
    return;
  }

  CDataset* dataset = GetCurrentDataset();
  if (dataset == NULL)
  {
    return;
  }

  CField* field = GetFieldstreectrl()->GetCurrentField();
  if (field == NULL)
  {
    return;
  }
  wxString title = wxString::Format("Set/change unit of field '%s'...", field->GetName().c_str());


  CUnitFieldDlg dlg (this, -1, title,
                      field);


  int32_t result = dlg.ShowModal();

  if (result != wxID_OK)
  {
    return;
  }

  field->SetUnit(dlg.GetUnit());

  GetFieldstreectrl()->UpdateFieldDescription();

  dataset->SetFieldSpecificUnit(field->GetName(), field->GetUnit());



}
//----------------------------------------
bool COperationPanel::CheckExpression(wxTextCtrl* ctrl)
{
  bool bOk = true;

  if (m_operation == NULL)
  {
    return bOk;
  }


  if (ctrl == NULL)
  {
    return bOk;
  }


  std::string value = ctrl->GetValue().ToStdString();
  std::string valueOut;

  if (IsFormulaSelectField())
  {
    bOk = CheckExpression(value, NULL, &valueOut);
  }
  else
  {
    std::string unit = GetOpunit()->GetValue().ToStdString();
    bOk = CheckExpression(value, &unit, &valueOut);
  }

  if (! valueOut.empty())
  {
    if ( !str_cmp( value, valueOut) )
    {
      ctrl->SetValue(valueOut);
    }
  }

  if (bOk == false)
  {
    ctrl->SetFocus();
  }

  return bOk;

}
//----------------------------------------
bool COperationPanel::CheckExpression(const std::string &value, std::string *strExprUnit /*= NULL*/, std::string *valueOut /*= NULL*/)
{
  if (m_operation == NULL)
  {
    return true;
  }

  std::string msg;
  bool bOk = CFormula::CheckExpression(value, m_operation->GetRecord().ToStdString(), msg, strExprUnit, &m_mapFormulaString, m_product, valueOut);

  if (!bOk)
  {
    wxMessageBox(msg,
                "Warning",
                wxOK | wxICON_EXCLAMATION);
  }

  return bOk;
}
//----------------------------------------
void COperationPanel::FillFormulaList()
{
  bool bOk = true;

  CWorkspaceFormula* wks = wxGetApp().GetCurrentWorkspaceFormula();
  if (wks == NULL)
  {
    return;
  }

  if (wks->GetFormulaCount() <= 0)
  {
    return;
  }

  wks->GetFormulaNames(m_mapFormulaString);
}
/*
//----------------------------------------
void COperationPanel::InitDefaultFormulaValue(const wxString& fieldName, int32_t type)
{
  if (m_operation == NULL)
  {
    return;
  }

  CFormula* formula = m_operation->GetFormula(type);

  if (formula == NULL)
  {
    return;
  }

  wxString text = formula->GetDescription(true).Trim();

  if (text.IsEmpty() == false)
  {
    return;
  }


  //long index = GetOpfieldlist()->FindItem(-1, fieldName);

  //if (index < 0)
  //{
  //  return;
  //}


  wxTreeItemId id = GetFieldstreectrl()->FindItem(fieldName, false, false);
  if (!id)
  {
    return;
  }

  CField* field = GetFieldstreectrl()->GetField(id);

  if (field == NULL)
  {
    return;
  }

  formula->SetDescription(field->GetName().c_str());
}

*/
//----------------------------------------
wxString COperationPanel::GetAlgorithmSyntax(wxTextCtrl* textCtrl, wxTextCtrl* unitCtrl)
{

 wxString title = "Insert an algorithm ...";

  CAlgorithmDlg dlg (this, -1, title);
  
  dlg.CentreOnParent();

  int32_t result = dlg.ShowModal();
  if (result != wxID_OK)
  {
    return "";
  }

  if (dlg.m_algo == NULL)
  {
    return  "";
  }

  //wxString name = dlg.GetAlgoList()->GetStringSelection();
  //CMapBratAlgorithm* algorithms = CBratAlgorithmBaseRegistry::GetAlgorithms();

  //wxString syntax = algorithms->GetSyntaxAlgo(name.c_str()).c_str();
  
  wxString syntax = dlg.m_algo->GetSyntax().c_str();

  if (textCtrl != NULL)
  {
    textCtrl->WriteText(syntax);
  }
  if (unitCtrl != NULL)
  {
    try 
    {
      // Algo. unit can be a descriptive label and not necessary a unit label, 
      // e.g on algo. whose unit depends on algo. input param/expression .
      // So, don't erase the current unit if the algo. has no specified unit.
      CUnit algoUnit = dlg.m_algo->GetOutputUnit();
      if (!algoUnit.HasNoUnit())
      {
        unitCtrl->SetValue(dlg.m_algo->GetOutputUnit().c_str());
      }
    }
    catch (CException& e)
    {
      // Do nothing
      e.what(); // To avoid compiler complaint
    }

  }

  return syntax;
}
//----------------------------------------
wxString COperationPanel::GetFormulaSyntax(wxTextCtrl* textCtrl, wxTextCtrl* unitCtrl)
{

  CWorkspaceFormula* wks = wxGetApp().GetCurrentWorkspaceFormula();
  if (wks == NULL)
  {
    return "";
  }

  wxString title = "Insert formula ...";

  CFormulasDlg dlg (this, -1, title);

  int32_t result = dlg.ShowModal();
  if (result != wxID_OK)
  {
    return "";
  }

  wxString name = dlg.GetFormulaList()->GetStringSelection();
  wxString syntax = wks->GetDescFormula(name.ToStdString(), dlg.GetFormulaAlias()->IsChecked());

  if (textCtrl != NULL)
  {
    textCtrl->WriteText(syntax);
  }
  if ((unitCtrl != NULL) && (dlg.m_formula != NULL))
  {
    unitCtrl->SetValue(dlg.m_formula->GetUnitAsText());
  }

  return syntax;
}
//----------------------------------------
wxString COperationPanel::GetFunctionSyntax()
{


  wxString title = "Insert function ...";

  CFunctionsDlg dlg (this, -1, title);

  int32_t result = dlg.ShowModal();
  if (result != wxID_OK)
  {
    return "";
  }
  wxString name = dlg.GetFunctionList()->GetStringSelection();
  return CMapFunction::GetInstance().GetSyntaxFunc(name);

}


//----------------------------------------
void COperationPanel::Execute(bool wait /*= false*/)
{

  if (m_operation == NULL)
  {
    return;
  }

  std::string msg;
  bool operationOk = CtrlOperation(msg, false, &m_mapFormulaString);
  if (operationOk == false)
  {
    wxMessageBox(wxString::Format("Operation '%s' has some errors and can't be execute:\n%s",
                                                    m_operation->GetName().c_str(), msg.c_str()),
                                 "Warning",
                                  wxOK | wxCENTRE | wxICON_EXCLAMATION);
    return;
  }

  /*

  int32_t result = wxMessageBox(wxString::Format("Do you to really want to execute this operation ?\n(it can take a while yo build '%s', depending on input data)",
                                                  m_operation->GetOutputName().c_str()),
                               "Warning",
                                wxYES_NO | wxCENTRE | wxICON_QUESTION);

  if (result != wxYES)
  {
    return;
  }
*/
/*
  bool outputExists = wxFileExists(m_operation->GetOutputName());

  if (outputExists)
  {
    result = wxMessageBox(wxString::Format("File '%s' already exists. Replace it ?",
                                                    m_operation->GetOutputName().c_str()),
                                 "Warning",
                                  wxYES_NO | wxCENTRE | wxICON_QUESTION);

    if (result != wxYES)
    {
      wxMessageBox("Process cancelled by user",
                   "Information",
                   wxOK | wxICON_INFORMATION);
      return;
    }
  }
*/
  wxGetApp().GotoLogPage();

  if (wait)
  {
    m_operation->SetLogFile();
  }
  else
  {
    m_operation->ClearLogFile();
  }

  BuildCmdFile();


  CPipedProcess* process = new CPipedProcess(m_operation->GetTaskName(),
                                           wxGetApp().GetLogPanel(),
                                           m_operation->GetFullCmd(),
                                           wxGetApp().GetLogPanel()->GetLogMess(),
                                           m_operation->GetOutput(),
                                           m_operation->GetType());


  if (wait)
  {
    process->SetExecuteFlags(wxEXEC_SYNC | wxEXEC_NODISABLE);
  }

  bool bOk = wxGetApp().GetLogPanel()->AddProcess(process);
  if (bOk == false)
  {
    delete process;
    process = NULL;
  }

  if (wait)
  {
    wxGetApp().GetLogPanel()->LogFile( m_operation->GetLogFile() );
  }


  return;
}
//----------------------------------------
bool COperationPanel::LoadSchedulerTaskConfig()
{
	if ( CSchedulerTaskConfig::IsLoaded() )
		CSchedulerTaskConfig::GetInstance()->setReloadAll( true );

	return CSchedulerTaskConfig::LoadAllSchedulerTaskConfig();
}
//----------------------------------------
bool COperationPanel::RemoveTaskFromSchedulerTaskConfig(wxXmlNode* taskNode)
{

  bool bOk = true;
  try
  {
    CSchedulerTaskConfig::GetInstance()->RemoveTask(taskNode);
  }
  catch(CException& e)
  {
    bOk = false;
    wxMessageBox(e.GetMessage().c_str(),
                   "Warning",
                    wxOK | wxCENTRE | wxICON_EXCLAMATION);      
  }

  return bOk;
}
//----------------------------------------
TaskRet COperationPanel::Delay()
{
	TaskRet r;
	if ( m_operation )
		return r;

	CDelayDlg delayDlg( this, -1, "Delay execution..." );

	if ( delayDlg.ShowModal() != wxID_OK )
		return r;

	return Delay( delayDlg );
}
//----------------------------------------
TaskRet COperationPanel::Delay( CDelayDlg& delayDlg )
{
	TaskRet r;
	if ( !m_operation )
		return r;

	std::string msg;
	bool operationOk = CtrlOperation( msg, false, &m_mapFormulaString );
	if ( !operationOk )
	{
		wxMessageBox( wxString::Format( "Operation '%s' has some errors and can't be execute:\n%s",
			m_operation->GetName().c_str(), msg.c_str() ),
			"Warning",
			wxOK | wxCENTRE | wxICON_EXCLAMATION );
		return r;
	}

	this->SetCursor( *wxHOURGLASS_CURSOR );

	BuildCmdFile();

	wxString taskLabel = delayDlg.GetTaskName()->GetValue();
	if ( taskLabel.IsEmpty() )
		taskLabel = m_operation->GetTaskName().c_str();

	wxDateTime at;
	delayDlg.GetDateTime( at );

	if ( !LoadSchedulerTaskConfig() )
	{
		this->SetCursor( wxNullCursor );
		return r;
	}

    r = CSchedulerTaskConfig::GetInstance()->AddTaskAsPending( m_operation->GetFullCmd().ToStdString(), at, taskLabel.ToStdString() );

	if ( !CSchedulerTaskConfig::SaveSchedulerTaskConfig() )
		RemoveTaskFromSchedulerTaskConfig( r.n );


	this->SetCursor( wxNullCursor );

	return r;
}
//----------------------------------------
void COperationPanel::ShowStats()
{

  if (m_operation == NULL)
  {
    return;
  }

  std::string msg;
  bool operationOk = CtrlOperation(msg, true, &m_mapFormulaString);
  if (operationOk == false)
  {
    wxMessageBox(wxString::Format("Operation '%s' has some errors and process can't be achieved:\n%s",
                                                    m_operation->GetName().c_str(), msg.c_str()),
                                 "Warning",
                                  wxOK | wxCENTRE | wxICON_EXCLAMATION);
    return;
  }


  wxGetApp().GotoLogPage();

  m_operation->ClearLogFile();

  m_operation->InitShowStatsOutput();

  BuildShowStatsCmdFile();

  CPipedProcess* process = new CPipedProcess(m_operation->GetShowStatsTaskName(),
                                           wxGetApp().GetLogPanel(),
                                           m_operation->GetShowStatsFullCmd(),
                                           wxGetApp().GetLogPanel()->GetLogMess(),
                                           m_operation->GetShowStatsOutput(),
                                           -1);

  bool bOk = wxGetApp().GetLogPanel()->AddProcess(process);
  if (bOk == false)
  {
    delete process;
    process = NULL;
  }


  EnableCtrl();
}


//----------------------------------------
void COperationPanel::ExportOperation()
{

  if (m_operation == NULL)
  {
    return;
  }

  std::string msg;
  bool operationOk = CtrlOperation(msg, true, &m_mapFormulaString);
  if (operationOk == false)
  {
    wxMessageBox(wxString::Format("Operation '%s' has some errors and can't be execute:\n%s",
                                                    m_operation->GetName().c_str(), msg.c_str()),
                                 "Warning",
                                  wxOK | wxCENTRE | wxICON_EXCLAMATION);
    return;
  }

  CExportDlg dlg (this, -1, "Export...", m_operation, &m_mapFormulaString);

  int32_t result = dlg.ShowModal();
  if (result != wxID_OK)
  {
    return;
  }

  //------------------
  // Remember last export filename and path
  //------------------
  wxFileName filenameToSave = dlg.m_currentName;
  filenameToSave.SetExt(CExportDlg::m_defaultExtensionAscii);
  m_operation->SetExportAsciiOutput(filenameToSave);

  m_operation->SetExecuteAgain(dlg.m_executeAgain);
//  m_operation->SetDelayExecution(dlg.m_delayExecution);


  //------------------
  // Export as Ascii
  //------------------
  if (dlg.AsAscii())
  {
    m_operation->SetExportAsciiDateAsPeriod(dlg.m_dateAsPeriod);
    m_operation->SetExportAsciiExpandArray(dlg.m_expandArray);
    m_operation->SetExportAsciiNoDataComputation(dlg.m_noDataComputation);
    m_operation->SetExportAsciiNumberPrecision(dlg.m_asciiNumberPrecision);

    if (dlg.m_delayExecution)
    {
      DelayExportOperationAsAscii(dlg.m_delayDlg);
    }
    else
    {
      ExportOperationAsAscii();
    }

    return;
  }

  //------------------
  // Export as NetCdf
  //------------------
  if (dlg.AsNetCdf())
  {
    bool bExecute = m_operation->IsExecuteAgain() || ( ! wxFileExists(m_operation->GetOutputName()) );
    if ((!bExecute) && (dlg.m_delayExecution))
    {
      wxMessageBox(wxString::Format("There is no need to delay the execution because you haven't check '%s' "
                                    "and the file '%s' already exists",
                                    dlg.GetExecagain()->GetLabelText().c_str(),
                                    m_operation->GetOutput()->GetFullPath().c_str()),
                                   "Warning",
                                    wxOK | wxCENTRE | wxICON_EXCLAMATION);
      return;

    }

    if (bExecute)
    {
      if (dlg.m_delayExecution)
      {
        DelayExportOperationAsNetCdf(dlg);
        return;
      }
      else
      {
        Execute(true);
      }
    }

    if (wxFileExists(m_operation->GetOutput()->GetFullPath()) == false)
    {
      wxMessageBox(wxString::Format("File'%s' doesn't exist - Please, look at the messages in the log panel"
                                    " and check if the operation have been correcly processed" ,
                                    m_operation->GetOutput()->GetFullPath().c_str()),
                                   "Warning",
                                    wxOK | wxCENTRE | wxICON_EXCLAMATION);
    }
    else if (wxCopyFile(m_operation->GetOutput()->GetFullPath(), dlg.m_currentName.GetFullPath()) == false)
    {
      wxMessageBox(wxString::Format("Unable to copy file '%s' to '%s'" ,
                                    m_operation->GetOutput()->GetFullPath().c_str(),
                                    dlg.m_currentName.GetFullPath().c_str()),
                   "Warning",
                    wxOK | wxCENTRE | wxICON_EXCLAMATION);

    }
    else
    {
      wxMessageBox(wxString::Format("File has been exported to '%s' " ,
                                    dlg.m_currentName.GetFullPath().c_str()),
                   "Warning",
                    wxOK | wxCENTRE | wxICON_INFORMATION);

    }

    return;

  }

  //------------------
  // Export as GeoTiff
  //------------------
  if (dlg.AsGeoTiff())
  {
    if (dlg.m_delayExecution)
    {
      DelayExportOperationAsGeoTiff(dlg);
    }
    else
    {
      // Always execute again
      Execute(true);
      ExportGeoTiff(m_operation->GetOutput()->GetFullPath(), dlg.m_currentName.GetFullPath(),
                    dlg.m_createKML, dlg.m_colorTable, dlg.m_colorRangeMin, dlg.m_colorRangeMax);
    }
  }

  EnableCtrl();
}

//----------------------------------------

#if defined(_MSC_VER)
#pragma warning( disable : 4996 )
#endif


void COperationPanel::DelayExportOperationAsGeoTiff( CExportDlg& exportDlg )
{
	if ( !m_operation )
		return;

	TaskRet r = Delay( exportDlg.m_delayDlg );
	if ( !r.n )
		return;

	CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();
	wxFileName exportGeoTiffCmdFile = exportDlg.m_currentName;
	exportGeoTiffCmdFile.SetExt( EXPORTGEOTIFF_COMMANDFILE_EXTENSION );

	if ( wks == NULL )
	{
		exportGeoTiffCmdFile.Normalize();
	}
	else
	{
		exportGeoTiffCmdFile.Normalize( wxPATH_NORM_ALL, wks->GetPath() );
	}

	wxFile file;
	file.Create( exportGeoTiffCmdFile.GetFullPath(), true, wxS_DEFAULT | wxS_IXUSR );
	//  file.Write("#!/usr/bin/env " + wxGetApp().GetExecExportGeoTiffName()->GetName() + "\n");
	file.Write( "\n#----- LOG -----\n\n" );
	file.Write( wxString::Format( "%s=2\n", kwVERBOSE.c_str() ) );
	file.Write( "\n#----- INPUT -----\n\n" );
	file.Write( kwFILE.c_str() );
	file.Write( "=" );
	file.Write( m_operation->GetOutput()->GetFullPath() );
	file.Write( "\n" );
	file.Write( "\n#----- COLORTABLE -----\n\n" );
	file.Write( kwDISPLAY_COLORTABLE.c_str() );
	file.Write( "=" );
	file.Write( exportDlg.m_colorTable );
	file.Write( "\n" );
	if ( !isDefaultValue( exportDlg.m_colorRangeMin ) )
	{
		file.Write( wxString::Format( "%s=%lf\n", kwDISPLAY_MINVALUE.c_str(), exportDlg.m_colorRangeMin ) );
	}
	if ( !isDefaultValue( exportDlg.m_colorRangeMax ) )
	{
		file.Write( wxString::Format( "%s=%lf\n", kwDISPLAY_MAXVALUE.c_str(), exportDlg.m_colorRangeMax ) );
	}
	if ( exportDlg.m_createKML )
	{
		wxFileName kmlOutputFile = exportDlg.m_currentName;
		kmlOutputFile.SetExt( "kml" );
		file.Write( "\n#----- GOOGLE EARTH -----\n\n" );
		file.Write( kwOUTPUT_KML.c_str() );
		file.Write( "=" );
		file.Write( kmlOutputFile.GetFullPath() );
		file.Write( "\n" );
		file.Write( kwDISPLAY_LOGO_URL.c_str() );
		file.Write( "=" );
		file.Write( CTools::GetDataDir().c_str() );
		file.Write( "/BratLogo.png" );
		file.Write( "\n" );
	}
	file.Write( "\n#----- OUTPUT -----\n\n" );
	file.Write( kwOUTPUT.c_str() );
	file.Write( "=" );
	file.Write( exportDlg.m_currentName.GetFullPath() );
	file.Write( "\n" );

	file.Close();

	wxString fullCommand = wxString::Format( "\"%s\" \"%s\"",
		wxGetApp().GetExecExportGeoTiffName()->GetFullPath().c_str(),
		exportGeoTiffCmdFile.GetFullPath().c_str() );


	wxString taskLabel = exportDlg.m_delayDlg.GetTaskName()->GetValue();
	if ( taskLabel.IsEmpty() )
		taskLabel = exportGeoTiffCmdFile.GetName().c_str();

	wxDateTime at;
	exportDlg.m_delayDlg.GetDateTime( at );

	// Save tasks uid because LoadSchedulerTaskConfig can reload all the file
	// and 'mainTaskNode' will be invalid
    std::string uidSaved;
    wxString uidSaved2;
    r.n->GetPropVal( CSchedulerTaskConfig::m_TASK_ID_ATTR, &uidSaved2 );
    uidSaved = r.t->GetUidAsString();		assert__( uidSaved2.ToStdString() == uidSaved );
	r.n = NULL;

	if ( !LoadSchedulerTaskConfig() )
		return;

    r.n = CSchedulerTaskConfig::GetInstance()->FindTaskNode_xml( uidSaved, true );

    r = CSchedulerTaskConfig::GetInstance()->AddTaskAsPending( r, fullCommand.ToStdString(), at, taskLabel.ToStdString() );

	if ( !CSchedulerTaskConfig::SaveSchedulerTaskConfig() )
		RemoveTaskFromSchedulerTaskConfig( r.n );

	EnableCtrl();
}
//----------------------------------------
void COperationPanel::DelayExportOperationAsNetCdf( CExportDlg& exportDlg )
{
	if ( !m_operation )
		return;

	TaskRet r = Delay( exportDlg.m_delayDlg );
	if ( !r.n )
		return;

	// Save tasks uid because LoadSchedulerTaskConfig can reload all the file
	// and 'mainTaskNode' will be invalid
	wxString uidSaved;
	r.n->GetPropVal( CSchedulerTaskConfig::m_TASK_ID_ATTR, &uidSaved );		assert__( uidSaved == r.t->GetUidAsString() );
	r.n = NULL;

	if ( !LoadSchedulerTaskConfig() )
		return;

    r.n = CSchedulerTaskConfig::GetInstance()->FindTaskNode_xml( uidSaved.ToStdString(), true );

	wxString taskLabel = exportDlg.m_delayDlg.GetTaskName()->GetValue();
	if ( taskLabel.IsEmpty() )
		taskLabel = m_operation->GetTaskName().c_str();

	wxDateTime at;
	exportDlg.m_delayDlg.GetDateTime( at );

	CVectorBratAlgorithmParam params;
	params.Insert( m_operation->GetOutput()->GetFullPath().ToStdString() );
	params.Insert( exportDlg.m_currentName.GetFullPath().ToStdString() );

	r = CSchedulerTaskConfig::GetInstance()->AddFunctionTaskAsPending( r,		//femm: AddTaskAsPending -> AddFunctionTaskAsPending
		CBratTaskFunction::m_TASK_FUNC_COPYFILE,
		params,
		at,
        taskLabel.ToStdString() );

	if ( !CSchedulerTaskConfig::SaveSchedulerTaskConfig() )
		RemoveTaskFromSchedulerTaskConfig( r.n );

	EnableCtrl();
}
//----------------------------------------
void COperationPanel::DelayExportOperationAsAscii( CDelayDlg& delayDlg )
{
	if ( !m_operation )
		return;

	TaskRet r;

	// Exports operation with data computation, otherwise only dumps expressions
	if ( ! m_operation->IsExportAsciiNoDataComputation() )
	{
		bool bExecute = m_operation->IsExecuteAgain() || ( ! wxFileExists( m_operation->GetOutputName() ) );
		if ( bExecute )
		{
			r = Delay( delayDlg );
			if ( !r.n )
				return;
		}
	}

	DelayExportOperationAsAsciiDump( delayDlg, r );

	EnableCtrl();
}
//----------------------------------------
void COperationPanel::DelayExportOperationAsAsciiDump( CDelayDlg& delayDlg, TaskRet r )
{
	if ( !m_operation )
		return;

	BuildExportAsciiCmdFile();

	wxString taskLabel = delayDlg.GetTaskName()->GetValue();
	if ( taskLabel.IsEmpty() )
		taskLabel = m_operation->GetExportAsciiTaskName().c_str();

	wxDateTime at;
	delayDlg.GetDateTime( at );

	// Save tasks uid because LoadSchedulerTaskConfig can reload all the file
	// and 'parent' will be invalid
	wxString uidSaved;

	if ( r.n != NULL )
	{
		r.n->GetPropVal( CSchedulerTaskConfig::m_TASK_ID_ATTR, &uidSaved );
		r.n = NULL;
	}

	if ( !LoadSchedulerTaskConfig() )
		return;

	if ( r.n == NULL )
	{
        r.n = CSchedulerTaskConfig::GetInstance()->FindTaskNode_xml( uidSaved.ToStdString(), true );
	}

    r = CSchedulerTaskConfig::GetInstance()->AddTaskAsPending( r, m_operation->GetExportAsciiFullCmd().ToStdString(), at, taskLabel.ToStdString() );

	if ( !CSchedulerTaskConfig::SaveSchedulerTaskConfig() )
		RemoveTaskFromSchedulerTaskConfig( r.n );

	EnableCtrl();
}


#if defined(_MSC_VER)
#pragma warning( default : 4996 )
#endif

//----------------------------------------
void COperationPanel::ExportOperationAsAscii()
{
	if (!m_operation)
		return;

	wxGetApp().GotoLogPage();

	// Exports operation with data computation, otherwise only dumps expressions
	if ( ! m_operation->IsExportAsciiNoDataComputation() )
	{
		// [FEMM] The output file is not updated as it should when the values in the formula tree change; 
		//so, inspecting IsExecuteAgain and output file existence is not enough to know when execution 
		//is necessary. Hence, here, we are forced to always execute, because apparently there is no other
		//mechanism that we could inspect to know if the output (netcdf) file is updated.
		//
		bool bExecute = true; // m_operation->IsExecuteAgain() || ( ! wxFileExists(m_operation->GetOutputName()) );

		if ( bExecute )
		{
			Execute( true );

			if ( wxFileExists( m_operation->GetOutput()->GetFullPath() ) == false )
			{
				wxMessageBox( wxString::Format( "File'%s' doesn't exist - Please, look at the messages in the log panel"
					" and check if the operation has been correctly processed",
					m_operation->GetOutput()->GetFullPath().c_str() ),
					"Warning",
					wxOK | wxCENTRE | wxICON_EXCLAMATION );
				EnableCtrl();
				return;
			}
		}
	}

	ExportOperationAsAsciiDump();

	EnableCtrl();

}

//----------------------------------------
void COperationPanel::ExportOperationAsAsciiDump()
{
  if (m_operation == NULL)
  {
    return;
  }

  wxGetApp().GotoLogPage();

  m_operation->ClearLogFile();

  BuildExportAsciiCmdFile();

  CPipedProcess* process = new CPipedProcess(m_operation->GetExportAsciiTaskName(),
                                           wxGetApp().GetLogPanel(),
                                           m_operation->GetExportAsciiFullCmd(),
                                           wxGetApp().GetLogPanel()->GetLogMess(),
                                           m_operation->GetExportAsciiOutput(),
                                           m_operation->GetType());

  bool bOk = wxGetApp().GetLogPanel()->AddProcess(process);
  if (bOk == false)
  {
    delete process;
    process = NULL;
  }

  EnableCtrl();

}
//----------------------------------------
void COperationPanel::ExportGeoTiff(wxFileName inputFile, wxFileName outputFile, bool createKML,
                                    wxString colorTable, double rangeMin, double rangeMax)
{
  CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();
  wxFileName exportGeoTiffCmdFile;
  wxFile file;

  wxGetApp().GotoLogPage();

  exportGeoTiffCmdFile.AssignDir(wks->GetPath());
  exportGeoTiffCmdFile.SetFullName(outputFile.GetFullName());
  exportGeoTiffCmdFile.SetExt(EXPORTGEOTIFF_COMMANDFILE_EXTENSION);
  if (wks == NULL)
  {
    exportGeoTiffCmdFile.Normalize();
  }
  else
  {
    exportGeoTiffCmdFile.Normalize(wxPATH_NORM_ALL, wks->GetPath());
  }

  file.Create(exportGeoTiffCmdFile.GetFullPath(), true, wxS_DEFAULT | wxS_IXUSR);
//  file.Write("#!/usr/bin/env " + wxGetApp().GetExecExportGeoTiffName()->GetName() + "\n");
  file.Write("\n#----- LOG -----\n\n");
  file.Write(wxString::Format("%s=2\n", kwVERBOSE.c_str()));
  file.Write("\n#----- INPUT -----\n\n");
  file.Write(kwFILE.c_str());
  file.Write("=");
  file.Write(inputFile.GetFullPath());
  file.Write("\n");
  file.Write("\n#----- COLORTABLE -----\n\n");
  file.Write(kwDISPLAY_COLORTABLE.c_str());
  file.Write("=");
  file.Write(colorTable);
  file.Write("\n");
  if (!isDefaultValue(rangeMin))
  {
    file.Write(wxString::Format("%s=%lf\n", kwDISPLAY_MINVALUE.c_str(), rangeMin));
  }
  if (!isDefaultValue(rangeMax))
  {
    file.Write(wxString::Format("%s=%lf\n", kwDISPLAY_MAXVALUE.c_str(), rangeMax));
  }
  if (createKML)
  {
    wxFileName kmlOutputFile = outputFile;
    kmlOutputFile.SetExt("kml");
    file.Write("\n#----- GOOGLE EARTH -----\n\n");
    file.Write(kwOUTPUT_KML.c_str());
    file.Write("=");
    file.Write(kmlOutputFile.GetFullPath());
    file.Write("\n");
    file.Write(kwDISPLAY_LOGO_URL.c_str());
    file.Write("=");
    file.Write(CTools::GetDataDir().c_str());
    file.Write("/BratLogo.png");
    file.Write("\n");
    file.Write(kwFILETYPE.c_str());
    file.Write("=");
    file.Write(m_product->GetProductClass().c_str());
    file.Write( " / ");
	file.Write(m_product->GetProductType().c_str());
    file.Write("\n");
    file.Write(kwPRODUCT_LIST.c_str());
    file.Write("=");
    file.Write(m_operation->GetDataset()->GetProductList()->ToString(", ", true));
    file.Write("\n");
  }
  file.Write("\n#----- OUTPUT -----\n\n");
  file.Write(kwOUTPUT.c_str());
  file.Write("=");
  file.Write(outputFile.GetFullPath());
  file.Write("\n");

  file.Close();

  wxString fullCommand = wxString::Format("\"%s\" \"%s\"",
                                          wxGetApp().GetExecExportGeoTiffName()->GetFullPath().c_str(),
                                          exportGeoTiffCmdFile.GetFullPath().c_str());

  CPipedProcess* process = new CPipedProcess(exportGeoTiffCmdFile.GetName(),
                                             wxGetApp().GetLogPanel(),
                                             fullCommand,
                                             wxGetApp().GetLogPanel()->GetLogMess(),
                                             &outputFile,
                                             m_operation->GetType());

  bool bOk = wxGetApp().GetLogPanel()->AddProcess(process);
  if (bOk == false)
  {
    delete process;
    process = NULL;
  }

  EnableCtrl();
}

//----------------------------------------
void COperationPanel::OnProcessTerm(CProcessTermEvent& event)
{

  EnableCtrl();

}
