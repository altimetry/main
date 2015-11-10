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
#if !defined(_Operation_h_)
#define _Operation_h_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "Operation.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/filename.h"
#include "wx/config.h"  // (to let wxWidgets choose a wxConfig class for your platform)
#include "wx/confbase.h"  // (base config class)
#include "wx/fileconf.h" // (wxFileConfig class)
//#include "wx/msw/regconf.h" // (wxRegConfig class)


#include "brathl.h"

#include "Dataset.h"
#include "Formula.h"
#include "Product.h" 

const wxString LOGFILE_EXTENSION = "log";
const wxString COMMANDFILE_EXTENSION = "par";
const wxString EXPORTASCII_COMMANDFILE_EXTENSION = "export.par";
const wxString EXPORTGEOTIFF_COMMANDFILE_EXTENSION = "geotiff_export.par";
const wxString SHOWSTAT_COMMANDFILE_EXTENSION = "stats.par";

class COperation : public CBratObject
{

public:


  /// Empty COperation ctor
  COperation(const wxString name);

  COperation(COperation& o);

  /// Destructor
  virtual ~COperation();

  wxString GetName() {return m_name;};
  void SetName(const wxString& value) {m_name = value;};

  bool HasFormula() {return GetFormulaCount() > 0;};

  bool HasFilters();
  
  int32_t GetFormulaCount() {return m_formulas.size();};
  int32_t GetFormulaCountDataFields();

  bool RenameFormula(CFormula* formula, const wxString& newName);

  void GetFormulaNames(wxComboBox& combo);

  bool SaveConfig(wxFileConfig* config);
  bool LoadConfig(wxFileConfig* config);

  CDataset* FindDataset(const wxString& datasetName);

  wxString GetRecord() {return m_record;};
  void SetRecord(const wxString& value) {m_record = value;};
  
  int32_t GetType() {return m_type;};
  void SetType(int32_t value) {m_type = value;};
  
  wxString GetDataModeAsString() {return CMapDataMode::GetInstance().IdToName(m_dataMode);};
  int32_t GetDataMode() {return m_dataMode;};
  void SetDataMode(int32_t value) {m_dataMode = value;};
  
  CDataset* GetDataset() {return m_dataset;};
  void SetDataset(CDataset* value) {m_dataset = value;};
  wxString GetDatasetName();

  CProduct* GetProduct() {return m_product;};
  void SetProduct(CProduct* value) {m_product = value;};

  CMapFormula* GetFormulas() { return &m_formulas; };

  CFormula* GetFormula(const wxString& name);
  CFormula* GetFormula(int32_t type);
  CFormula* GetFormula(CMapFormula::iterator it);
  
  CFormula* NewUserFormula(CField* field, int32_t typeField, bool addToMap = true, CProduct* product = NULL);
  CFormula* NewUserFormula( const wxString& name = "", int32_t typeField = CMapTypeField::typeOpAsField, const wxString& strUnit = "", bool addToMap = true, CProduct* product = NULL);

  bool AddFormula(CFormula& value);
  bool DeleteFormula(const wxString& name);

  wxString GetDescFormula(const wxString& name, bool alias = false);
  //void SetDescFormula(const wxString& name, const wxString& value);
  
  wxFileName* GetOutput() {return &m_output;};
  wxString GetOutputName() {return m_output.GetFullPath();};
  wxString GetOutputNameRelativeToWks();
  
  void SetOutput(const wxFileName& value);
  void SetOutput(const wxString& value);
  
  wxFileName* GetExportAsciiOutput() {return &m_exportAsciiOutput;};
  wxString GetExportAsciiOutputName() {return m_exportAsciiOutput.GetFullPath();};
  wxString GetExportAsciiOutputNameRelativeToWks();

  void SetExportAsciiOutput(const wxFileName& value);
  void SetExportAsciiOutput(const wxString& value);


  wxFileName* GetShowStatsOutput() {return &m_showStatsOutput;};
  wxString GetShowStatsOutputName() {return m_showStatsOutput.GetFullPath();};
  wxString GetShowStatsOutputNameRelativeToWks();
  
  void SetShowStatsOutput(const wxFileName& value);
  void SetShowStatsOutput(const wxString& value);

  wxString GetTaskName();
  wxFileName* GetCmdFile() {return &m_cmdFile;};
  wxString GetCmdFileFullName() {return m_cmdFile.GetFullPath();};
  void SetCmdFile();

  wxString GetExportAsciiTaskName();
  wxFileName* GetExportAsciiCmdFile() {return &m_exportAsciiCmdFile;};
  wxString GetExportAsciiCmdFileFullName() {return m_exportAsciiCmdFile.GetFullPath();};
  void SetExportAsciiCmdFile();
  
  wxString GetShowStatsTaskName();
  wxFileName* GetShowStatsCmdFile() {return &m_showStatsCmdFile;};
  wxString GetShowStatsCmdFileFullName() {return m_showStatsCmdFile.GetFullPath();};
  void SetShowStatsCmdFile();


  wxString GetCommentFormula(const wxString& name);
  void SetCommentFormula(const wxString& name, const wxString& value);

  CFormula* GetSelect() {return m_select;};
  void SetSelect(CFormula* value);
  
  bool IsYFX();
  bool IsZFXY();
  bool IsMap();

  bool IsSelect(CFormula* value);
  static bool IsSelect(const wxString& name);

  wxString GetSelectName();
  wxString GetSelectDescription();
//  void SetSelectDesc(const wxString& value);

  wxString GetFormulaNewName();
  wxString GetFormulaNewName(const wxString& prefix);

  wxFileName* GetSystemCommand();
  wxString GetFullCmd();
  
  wxFileName* GetExportAsciiSystemCommand();
  wxString GetExportAsciiFullCmd();

  wxFileName* GetShowStatsSystemCommand();
  wxString GetShowStatsFullCmd();


  bool IsExportAsciiDateAsPeriod() {return m_exportAsciiDateAsPeriod;};
  void SetExportAsciiDateAsPeriod(bool value) {m_exportAsciiDateAsPeriod = value;};

  bool IsExportAsciiExpandArray() {return m_exportAsciiExpandArray;};
  void SetExportAsciiExpandArray(bool value) {m_exportAsciiExpandArray = value;};

  bool IsExportAsciiNoDataComputation() {return m_exportAsciiNoDataComputation;};
  void SetExportAsciiNoDataComputation(bool value) {m_exportAsciiNoDataComputation = value;};
  
  int32_t GetExportAsciiNumberPrecision() {return m_exportAsciiNumberPrecision;};
  void SetExportAsciiNumberPrecision(int32_t value) {m_exportAsciiNumberPrecision = value;};

  bool IsExecuteAgain() {return m_executeAgain;};
  void SetExecuteAgain(bool value) {m_executeAgain = value;};

  //bool IsDelayExecution() {return m_delayExecution;};
  //void SetDelayExecution(bool value) {m_delayExecution = value;};

  bool CtrlLoessCutOff(wxString& msg);

  bool UseDataset(const wxString& name);

  bool RemoveOutput();
  bool RenameOutput(const wxString& oldName);

  bool BuildShowStatsCmdFile();
  bool BuildShowStatsCmdFileHeader();
  bool BuildShowStatsCmdFileOutput();
  bool BuildShowStatsCmdFileFields();
    
  bool BuildExportAsciiCmdFile();
  bool BuildExportAsciiCmdFileHeader();
  bool BuildExportAsciiCmdFileGeneralProperties();
  bool BuildExportAsciiCmdFileAlias();
  bool BuildExportAsciiCmdFileDataset();
  bool BuildExportAsciiCmdFileFields();
  bool BuildExportAsciiCmdFileOutput();
  bool BuildExportAsciiCmdFileSelect();
  bool BuildExportAsciiCmdFileVerbose();

  bool BuildCmdFile();
  bool BuildCmdFileHeader();
  bool BuildCmdFileGeneralProperties();
  bool BuildCmdFileAlias();
  bool BuildCmdFileSpecificUnit();
  bool BuildCmdFileDataset();
  bool BuildCmdFileFromOutputOperation();
  bool BuildCmdFileFields();
  bool BuildCmdFileFieldsSpecificZFXY(CFormula* value);
  bool BuildCmdFileOutput();
  bool BuildCmdFileSelect();
  bool BuildCmdFileVerbose();


  bool WriteComment(const wxString& text);
  bool WriteLine(const wxString& text);
  bool WriteEmptyLine();

  void InitOutput();
  void InitExportAsciiOutput();
  void InitShowStatsOutput();

  bool ComputeInterval(const wxString& formulaName, bool showMsg = false);
  bool ComputeInterval(CFormula* f, bool showMsg = false);

  bool ControlDimensions(CFormula* formula, wxString& errorMsg, const CStringMap* aliases = NULL);
  bool ControlResolution(wxString& errorMsg);
  bool Control(wxString& msg, bool basicControl = false, const CStringMap* aliases = NULL);

  bool GetXExpression(CExpression& expr, wxString& errorMsg, const CStringMap* aliases = NULL);
  bool GetYExpression(CExpression& expr, wxString& errorMsg, const CStringMap* aliases = NULL);

  bool ControlXYDataFields(wxString& errorMsg, const CStringMap* aliases = NULL);

  void DeleteSelect();

  void ClearLogFile();

  void SetLogFile(const wxString& value);
  void SetLogFile();

  wxFileName* GetLogFile() {return &m_logFile;};

  COperation& operator=(COperation& o);

   ///Dump fonction
   virtual void Dump(std::ostream& fOut = std::cerr);

protected:
  void Init();
  void Copy(COperation& o);


protected:
  
  CProduct* m_product;
  CDataset* m_dataset;
  wxString m_record;
  //CFormula* m_formula;
  CFormula* m_select;
  CMapFormula m_formulas;
  int32_t m_type;
  int32_t m_dataMode;
  
  wxFileName m_output;
  wxFileName m_exportAsciiOutput;
  wxFileName m_showStatsOutput;
  
  wxFileName m_cmdFile;
  wxFileName m_exportAsciiCmdFile;
  wxFileName m_showStatsCmdFile;

  wxFile m_file;
  int32_t m_verbose;

  wxFileName m_logFile;

  bool m_exportAsciiDateAsPeriod;
  bool m_exportAsciiExpandArray;
  bool m_exportAsciiNoDataComputation;
  bool m_executeAgain;
  //bool m_delayExecution;

  int32_t m_exportAsciiNumberPrecision;

private:
  wxString m_name;



};

/** @} */


#endif // !defined(_Parameter_h_)
