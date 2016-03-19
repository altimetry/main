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


#ifndef __OperationPanel_H__
#define __OperationPanel_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <wx/xml/xml.h> // (wxXmlDocument, wxXmlNode, wxXmlProperty classes)

#include "BratGui_wdr.h"
class CNewDataExprEvent;
class CRenameDatasetEvent;
class CNewDatasetEvent;
class CDeleteDatasetEvent;
class CDeleteDataExprEvent;
class CDatasetFilesChangeEvent;


#include "GuiPanel.h"

#include "LabeledTextCtrl.h"
#include "DatasetTreeCtrl.h"
#include "FieldsTreeCtrl.h"
#include "OperationTreeCtrl.h"

#include "Dictionary.h"
#include "new-gui/brat/DataModels/Workspaces/Operation.h"
#include "Process.h"
#include "DelayDlg.h"
#include "ExportDlg.h"
#include "SchedulerTaskConfig.h"


class CDataset;


// WDR: class declarations

//----------------------------------------------------------------------------
// CAliasInfoDlg
//----------------------------------------------------------------------------

class CAliasInfoDlg: public wxDialog
{
public:
  // constructors and destructors
  CAliasInfoDlg( wxWindow *parent, wxWindowID id, const wxString &title,
                      COperation* operation, CFormula* formula,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

  virtual ~CAliasInfoDlg();
    
  // WDR: method declarations for CAliasInfoDlg
  wxStaticText* GetAliasinfoFooter()  { return (wxStaticText*) FindWindow( ID_ALIASINFO_FOOTER ); }
  wxStaticText* GetAliasinfoHeader()  { return (wxStaticText*) FindWindow( ID_ALIASINFO_HEADER ); }
  wxGrid* GetAliasinfoGrid()  { return (wxGrid*) FindWindow( ID_ALIASINFO_GRID ); }

public:

  static const int32_t VALUE_COL;
  static const int32_t SYNTAX_COL;
  static const int32_t DESCR_COL;
  static const int32_t USE_COL;
    
private:
    // WDR: member variable declarations for CAliasInfoDlg
  wxSizer* m_item0;
  wxSize m_gridSize;

private:
  // WDR: handler declarations for CAliasInfoDlg
  void OnOk( wxCommandEvent &event );

private:
  DECLARE_CLASS(CAliasInfoDlg)
  DECLARE_EVENT_TABLE()
};
//----------------------------------------------------------------------------
// CExpressionInfoDlg
//----------------------------------------------------------------------------

class CExpressionInfoDlg: public wxDialog
{
public:
    // constructors and destructors
    CExpressionInfoDlg( wxWindow *parent, wxWindowID id, const wxString &title,
                        wxString exprValue, COperation* operation, CFormula* formula,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

    virtual ~CExpressionInfoDlg();
    
    // WDR: method declarations for CExpressionInfoDlg
    wxStaticText* GetExprinfoResultUnit()  { return (wxStaticText*) FindWindow( ID_EXPRINFO_RESULT_UNIT ); }
    wxGrid* GetExprinfoGrid()  { return (wxGrid*) FindWindow( ID_EXPRINFO_GRID ); }
    
private:
    // WDR: member variable declarations for CExpressionInfoDlg
    
private:
    // WDR: handler declarations for CExpressionInfoDlg
    void OnOk( wxCommandEvent &event );

private:
    DECLARE_CLASS(CExpressionInfoDlg)
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// COperationPanel
//----------------------------------------------------------------------------

class COperationPanel: public wxPanel
{
	const CApplicationPaths &mBratPaths;
public:
  // constructors and destructors
  COperationPanel( const CApplicationPaths &brat_paths, wxWindow *parent, wxWindowID id = -1,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxTAB_TRAVERSAL | wxNO_BORDER );

  virtual ~COperationPanel();

  // WDR: method declarations for COperationPanel
  wxButton* GetOpaliasinfo()  { return (wxButton*) FindWindow( ID_OPALIASINFO ); }
  wxButton* GetOpdelay()  { return (wxButton*) FindWindow( ID_OPDELAY ); }
  wxButton* GetOpinsertalgorithm()  { return (wxButton*) FindWindow( ID_OPINSERTALGORITHM ); }
  wxBitmapButton* GetOpchangerecord()  { return (wxBitmapButton*) FindWindow( ID_OPCHANGERECORD ); }
  wxButton* GetOpshowstats()  { return (wxButton*) FindWindow( ID_OPSHOWSTATS ); }
  wxButton* GetOpexprinfo()  { return (wxButton*) FindWindow( ID_OPEXPRINFO ); }
  wxStaticText* GetOpYresolutionlabel()  { return (wxStaticText*) FindWindow( ID_OP_YRESOLUTIONLABEL ); }
  wxStaticText* GetOpXresolutionlabel()  { return (wxStaticText*) FindWindow( ID_OP_XRESOLUTIONLABEL ); }
  wxStaticText* GetOpFilterlabel()  { return (wxStaticText*) FindWindow( ID_OP_FILTERLABEL ); }
  wxStaticText* GetOperationtreectrllabel()  { return (wxStaticText*) FindWindow( ID_OPERATIONTREECTRLLABEL ); }
  wxButton* GetOpexecute()  { return (wxButton*) FindWindow( ID_OPEXECUTE ); }
  wxButton* GetOpexportasciiedit()  { return (wxButton*) FindWindow( ID_OPEXPORTASCIIEDIT ); }
  wxButton* GetOpexport()  { return (wxButton*) FindWindow( ID_OPEXPORT ); }
  wxButton* GetOpdelete()  { return (wxButton*) FindWindow( ID_OPDELETE ); }
  wxButton* GetOpdup()  { return (wxButton*) FindWindow( ID_OPDUP ); }
  wxButton* GetOpnew()  { return (wxButton*) FindWindow( ID_OPNEW ); }
  wxComboBox* GetOpDataMode()  { return (wxComboBox*) FindWindow( ID_OP_DATA_MODE ); }
  wxButton* GetOpresolution()  { return (wxButton*) FindWindow( ID_OPRESOLUTION ); }
  wxButton* GetOptitle()  { return (wxButton*) FindWindow( ID_OPTITLE ); }
  wxButton* GetOpcheckdataf()  { return (wxButton*) FindWindow( ID_OPCHECKDATAF ); }
  wxButton* GetOpdeleteexpr()  { return (wxButton*) FindWindow( ID_OPDELETEEXPR ); }
  wxButton* GetOpsaveasformula()  { return (wxButton*) FindWindow( ID_OPSAVEASFORMULA ); }
  wxButton* GetOpinsertformula()  { return (wxButton*) FindWindow( ID_OPINSERTFORMULA ); }
  wxButton* GetOpinsertfct()  { return (wxButton*) FindWindow( ID_OPINSERTFCT ); }
  wxButton* GetOpinsertfield()  { return (wxButton*) FindWindow( ID_OPINSERTFIELD ); }
  wxButton* GetOpinsertexpr()  { return (wxButton*) FindWindow( ID_OPINSERTEXPR ); }
  wxComboBox* GetOptype()  { return (wxComboBox*) FindWindow( ID_OPTYPE ); }
  wxTextCtrl* GetOpunit()  { return (wxTextCtrl*) FindWindow( ID_OPUNIT ); }
  wxTextCtrl* GetOptextform()  { return (wxTextCtrl*) FindWindow( ID_OPTEXTFORM ); }
  wxComboBox* GetOpnames()  { return (wxComboBox*) FindWindow( ID_OPNAMES ); }
  COperationTreeCtrl* GetOperationtreectrl()  { return (COperationTreeCtrl*) FindWindow( ID_OPERATIONTREECTRL ); }
  CFieldsTreeCtrl* GetFieldstreectrl()  { return (CFieldsTreeCtrl*) FindWindow( ID_FIELDSTREECTRL ); }
  CDatasetTreeCtrl* GetDatasettreectrl()  { return (CDatasetTreeCtrl*) FindWindow( ID_DATASETTREECTRL ); }

  bool HasOperation() {return m_currentOperationIndex >= 0;};

  bool CtrlOperation(CWorkspaceFormula *wks, std::string &msg, bool basicControl = false, const CStringMap* aliases = NULL);
  bool BuildCmdFile();

  bool BuildExportAsciiCmdFile();
  bool BuildShowStatsCmdFile();

  void LoadDataset();
  void LoadOperation();
  void NewOperation();
  void DeleteOperation();       //cannot be private, used by frame
  void DuplicateOperation();

  void GetOperationOutput();
  void GetOperationExportAsciiOutput();

  void GetOperationOutputs();

  void InitOperationOutput();
  void InitOperationExportAsciiOutput();
  
  void InitOperationOutputs();

  void ShowResolutionAndFilterSizer(bool showIt);

  void EnableCtrl();
  void ClearAll();
  void ClearFieldsInfo();
  bool IsFormulaDataField();
  bool IsFormulaSelectField();
  
private:
	void FillDataTypeList();
public:
  void FillDataModeList();

  void GetDataMode();
  void GetOperationUnit();
  void GetOperationDataType();
  void GetUserFormulaText();

  wxTreeItemId GetOperationDataset(bool select = true);
  wxTreeItemId GetOperationRecord(bool select = true);
  
  bool SetCurrentRecord();

  void SetDataSetAndRecordLabel();
  
  bool IsCurrentCriteriaSelection();
  bool IsCriteriaSelection(CFormula* formula);

  void InsertEmptyExpression();
  void InsertField();
  void InsertField(CField* field);
  void InsertAlgorithm();
  void InsertFormula();
  void InsertFunction();
  void DeleteExpression();
  void RenameExpression();
  void FieldAttrChangeUnit();

  void SaveCurrentFormulaAs();
  void SaveFormulaAs(CFormula* formula);

  bool CheckExpression(wxTextCtrl* ctrl);
  bool CheckExpression( const std::string& value, std::string *strExprUnit = NULL, std::string *valueOut = NULL);

  void FillFormulaList();
//  void InitDefaultFormulaValue(const wxString& fieldName, int32_t type);

  void Reset();

  void SetResolutionLabels();



private:
    // WDR: member variable declarations for COperationPanel
  bool m_opNameDirty;


  int32_t m_currentOperationIndex;
  wxTreeItemId m_currentDataset;
  wxTreeItemId m_currentRecord;

  int32_t m_currentFile;
  wxString m_currentFilename;

  COperation* m_operation;
  CFormula* m_userFormula;

  //CDataset* m_dataset;
  bool m_revertSortFile;

  CProduct* m_product;

  wxPanel* m_leftPanel;
  wxPanel* m_rightPanel;

  CStringMap m_mapFormulaString;

  wxString m_productClass;
  wxString m_productType;

    
private:

  wxString GetAlgorithmSyntax(wxTextCtrl* textCtrl = NULL, wxTextCtrl* unitCtrl = NULL);
  wxString GetFormulaSyntax(wxTextCtrl* textCtrl = NULL, wxTextCtrl* unitCtrl = NULL);
  wxString GetFunctionSyntax();

  void AddFormula(CFormula& formula);

  void AddFormulaComment(CFormula* formula);

  CDataset* GetCurrentDataset();
  bool SetCurrentDataset();
  
  COperation* GetCurrentOperation();
  bool SetCurrentOperation();
  
  CFormula* GetCurrentFormula();
  bool SetCurrentFormula();

  void SetUnitText();
  void OpNameChanged();

  void RemoveOutput();
  void RenameOutput(const wxString& oldName);

  void DeleteProduct();

  void SetTextFormula();
  void OnFormulaChanged(CFormula* formula);

  //void DatasetSelChanged( const wxTreeItemId& id ); v4 out
  
  TaskRet Delay();
  TaskRet Delay(CDelayDlg& delayDlg);

  void Execute(bool wait = false);
  void ExportOperation();
  void ExportOperationAsAscii();
  void ExportOperationAsAsciiDump();
  void ExportGeoTiff(wxFileName inputFile, wxFileName outputFile, bool createKML,
                     wxString colorTable, double rangeMin, double rangeMax);

  void DelayExportOperationAsAscii(CDelayDlg& delayDlg);
  void DelayExportOperationAsAsciiDump(CDelayDlg& delayDlg, TaskRet r);

  void DelayExportOperationAsNetCdf(CExportDlg& exportDlg);

  void DelayExportOperationAsGeoTiff(CExportDlg& exportDlg);

  void ShowStats();

  void Resolution( );

  //void ComputeInterval(const wxString& formulaName);
  //void ComputeInterval(CFormula* formula);
  void ComputeInterval();

private:
	bool LoadSchedulerTaskConfig();
	bool RemoveTaskFromSchedulerTaskConfig(wxXmlNode* taskNode);



private:

  void OnDataExprNew(CNewDataExprEvent& event);
  void OnDataExprDeleted(CDeleteDataExprEvent& event);

  // WDR: handler declarations for COperationPanel
    void OnShowAliases( wxCommandEvent &event );
    void OnLaunchScheduler( wxCommandEvent &event );
    void OnDelay( wxCommandEvent &event );
    void OnInsertAlgorithm( wxCommandEvent &event );
  void OnChangeDefaultRecord( wxCommandEvent &event );
  void OnShowStats( wxCommandEvent &event );
  void OnShowExprInfo( wxCommandEvent &event );
  void OnPositionChanging( wxSplitterEvent &event );
  void OnProcessTerm(CProcessTermEvent& event);
  void OnFieldSelChanged( wxTreeEvent &event );
  void OnComboDataType( wxCommandEvent &event );
  void OnTextFormulaEnter( wxCommandEvent &event );
  void OnTextFormula( wxCommandEvent &event );
  void OnExecute( wxCommandEvent &event );
  void OnComboOperationTextEnter( wxCommandEvent &event );
  void OnComboOperation( wxCommandEvent &event );
  void OnComboDataMode( wxCommandEvent &event );
  void OnTitle( wxCommandEvent &event );
  void OnCheckSyntax( wxCommandEvent &event );
  void OnDeleteExpression( wxCommandEvent &event );
  void OnSaveAsFormula( wxCommandEvent &event );
  void OnInsertFormula( wxCommandEvent &event );
  void OnInsertFunction( wxCommandEvent &event );
  void OnInsertField( wxCommandEvent &event );
  void OnInsertExpression( wxCommandEvent &event );
  void OnEditAsciiExportOperation( wxCommandEvent &event );
  void OnExportOperation( wxCommandEvent &event );
  void OnDeleteOperation( wxCommandEvent &event );
  void OnDuplicateOperation( wxCommandEvent &event );
  void OnNewOperation( wxCommandEvent &event );
  void OnDatasetSelChanging( wxTreeEvent &event );
  void OnOperationSelChanged( wxTreeEvent &event );
  void OnDatasetSelChanged( wxTreeEvent &event );
  
  void OnResolution( wxCommandEvent &event );
  
  void OnTextFormulaKillFocus( wxFocusEvent &event );

  void OnNewDataset(CNewDatasetEvent& event);
  void OnRenameDataset(CRenameDatasetEvent& event);
  void OnDeleteDataset(CDeleteDatasetEvent& event);

  void OnDatasetFilesChanged(CDatasetFilesChangeEvent& event);

  void OnUnitKillFocus( wxFocusEvent &event );
  void OnUnitChar( wxKeyEvent &event );
  void OnUnitTextEnter( wxCommandEvent &event );
  void OnUnitText( wxCommandEvent &event );

  void OnComboOperationChar( wxKeyEvent &event );
  void OnComboOperationKillFocus( wxFocusEvent &event );

  void ComboOperation( int32_t currentOperation );
  void ComboOperation();



private:
    DECLARE_CLASS(COperationPanel)
    DECLARE_EVENT_TABLE()
};




#endif
