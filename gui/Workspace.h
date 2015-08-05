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

#ifndef __Workspace_H__
#define __Workspace_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "Workspace.h"
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


#include "BratObject.h"
#include "List.h"
using namespace brathl;

#include "BitSet32.h"

#include "Dataset.h"
#include "Operation.h"
#include "Formula.h"
#include "Display.h"


const wxString WKS_DATASET_NAME = "Datasets";
const wxString WKS_FORMULA_NAME = "Formulas";
const wxString WKS_EXPRESSION_NAME = "Expression";
const wxString WKS_OPERATION_NAME = "Operations";
const wxString WKS_DISPLAY_NAME = "Displays";


const wxString GROUP_GENERAL = "General";
const wxString GROUP_DATASETS = "Datasets";
const wxString GROUP_OPERATIONS = "Operations";
const wxString GROUP_FORMULAS = "Formulas";
const wxString GROUP_FUNCTIONS = "Functions";
const wxString GROUP_DISPLAY = "Displays";

const wxString ENTRY_WKSNAME = "WorkspaceName";
const wxString ENTRY_WKSLEVEL = "WorkspaceLevel";

const wxString ENTRY_RECORDNAME = "RecordName";
const wxString ENTRY_DSNAME = "DatasetName";
const wxString ENTRY_OPNAME = "OperationName";
const wxString ENTRY_FILE = "File";
const wxString ENTRY_FILE_REGEX = ENTRY_FILE + "\\d+";
const wxString ENTRY_UNIT = "Unit";
const wxString ENTRY_UNIT_REGEX = ENTRY_UNIT + "_(.+)";
const wxString ENTRY_DISPLAYNAME = "DisplayName";

const wxString ENTRY_DISPLAY_DATA = "DisplayData";
const wxString ENTRY_FORMULA = "Formula";
const wxString ENTRY_SELECT = "Select";

const wxString ENTRY_DEFINE = "Define";
const wxString ENTRY_COMMENT = "Comment";
const wxString ENTRY_TYPE = "Type";
const wxString ENTRY_DATA_MODE = "DataMode";
const wxString ENTRY_FIELD = "Field";
const wxString ENTRY_FIELDNAME = "FieldName";
const wxString ENTRY_FIELDTITLE = "FieldTitle";
const wxString ENTRY_FIELDTYPE = "FieldType";
const wxString ENTRY_DATATYPE = "DataType";
const wxString ENTRY_TITLE = "Title";
const wxString ENTRY_INTERVAL = "Interval";
const wxString ENTRY_STEP = "Step";
const wxString ENTRY_FILTER = "Filter";
const wxString ENTRY_MINVALUE = "MinValue";
const wxString ENTRY_MAXVALUE = "MaxValue";
const wxString ENTRY_MINXVALUE = "MinXValue";
const wxString ENTRY_MAXXVALUE = "MaxXValue";
const wxString ENTRY_MINYVALUE = "MinYValue";
const wxString ENTRY_MAXYVALUE = "MaxYValue";
const wxString ENTRY_LOESSCUTOFF = "LoessCutOff";
const wxString ENTRY_ANIMATION = "Animation";
const wxString ENTRY_PROJECTION = "Projection";
const wxString ENTRY_GROUP = "Group";
const wxString ENTRY_CONTOUR = "Contour";
const wxString ENTRY_INVERT_XYAXES = "InvertXYAxes";
const wxString ENTRY_SOLID_COLOR = "SolidColor";
const wxString ENTRY_COLOR_PALETTE = "ColorPalette";
const wxString ENTRY_EAST_COMPONENT = "EastComponent";
const wxString ENTRY_NORTH_COMPONENT = "NorthComponent";

const wxString ENTRY_X = "X";
const wxString ENTRY_XDESCR = "XDescr";
const wxString ENTRY_XUNIT = "XUnit";
const wxString ENTRY_Y = "Y";
const wxString ENTRY_YDESCR = "YDescr";
const wxString ENTRY_YUNIT = "YUnit";
const wxString ENTRY_Z = "Z";
const wxString ENTRY_ZDESCR = "ZDescr";
const wxString ENTRY_ZUNIT = "ZUnit";

const wxString ENTRY_X_AXIS = "XAxis";
//const wxString ENTRY_X_AXIS_TEXT = "XAxisTitle";

const wxString ENTRY_OUTPUT = "Output";
const wxString ENTRY_EXPORT_ASCII_OUTPUT = "ExportAsciiOutput";

const wxString ENTRY_ZOOM = "Zoom";



//-------------------------------------------------------------
//------------------- CWorkspace class --------------------
//-------------------------------------------------------------

class CWorkspace: public CBratObject
{
public:
  // constructors and destructors
  CWorkspace();
  CWorkspace(const wxString& name, const wxFileName& path, bool createPath = true);
  CWorkspace(const wxString& name, const wxString& path, bool createPath = true);

  virtual ~CWorkspace();

  void SetName(const wxString& name) {m_name = name;};
  const wxString& GetName() {return m_name;};

  bool SetPath(const wxFileName& path, bool createPath = true) ;
  bool SetPath(const wxString& path,bool createPath = true);

  void SetLevel(int32_t value) {m_level = value;};

  bool Rmdir();

  const wxFileName& GetPath() {return m_path;};
  wxString GetPathName(int32_t flags = wxPATH_GET_VOLUME, wxPathFormat format = wxPATH_NATIVE)
                        {return m_path.GetPath(flags, format);};

  //void AppendDir(const wxString& dir) {m_path.AppendDir(dir);};
  //void AppendDir() {m_path.AppendDir(m_name);};

  wxFileConfig* GetConfig() {return m_config;};

  void SetKey(const wxString& key) { m_key = key; };
  const wxString& GetKey() { return m_key; };
  wxString GetRootKey();

  bool HasName() {return m_name.IsEmpty() == false;};
  bool HasPath() {return m_path.IsDir() && (m_path.GetPath().IsEmpty() == false);};
  bool IsPath() {return m_path.DirExists();};
  bool IsConfigFile() {return m_configFileName.FileExists();};
  bool IsRoot() {return m_level == 1;};

  virtual void InitConfig();

  virtual bool SaveConfig(bool flush = true);
  virtual bool LoadConfig();
  virtual bool Import(CWorkspace* wks);

  void SetImportBitSet(CBitSet32* value) { m_importBitSet = value; };

  bool GetCtrlDatasetFiles () { return m_ctrlDatasetFiles;};
  void SetCtrlDatasetFiles (bool value) { m_ctrlDatasetFiles = value;};

  virtual void Dump(ostream& fOut = cerr);


public:
  enum actionFlags
  {
    wksNew,
    wksOpen,
    wksImport,
    wksRename,
    wksDelete
  };

protected:
  bool SaveCommonConfig(bool flush = true);
  bool LoadCommonConfig();
  bool RmdirRecurse();
  bool DeleteConfigFile();

protected:
  wxFileConfig* m_config;
  wxFileName m_configFileName;

  wxString m_key;

  wxString m_name;
  wxFileName m_path;

  int32_t m_level;

  CBitSet32* m_importBitSet;

  static const wxString m_configName;

  bool m_ctrlDatasetFiles;

private:
  void Init();
  void DeleteConfigObj();

private:


};

//-------------------------------------------------------------
//------------------- CWorkspaceDataset class --------------------
//-------------------------------------------------------------

class CWorkspaceDataset: public CWorkspace
{
public:
  // constructors and destructors
  CWorkspaceDataset();
  CWorkspaceDataset(const wxString& name, const wxFileName& path, bool createPath = true);
  CWorkspaceDataset(const wxString& name, const wxString& path, bool createPath = true);

  virtual ~CWorkspaceDataset();

  void Close();

  bool HasDataset() {return GetDatasetCount() > 0;};

  bool CheckFiles();


  virtual bool SaveConfig(bool flush = true);
  virtual bool LoadConfig();
  virtual bool Import(CWorkspace* wks);

  bool RenameDataset(CDataset* dataset, const wxString& newName);
  bool DeleteDataset(CDataset* dataset);

  wxString GetDatasetNewName();

  int32_t GetDatasetCount();
  CDataset* GetDataset(const wxString& name);
  CObMap* GetDatasets() {return &m_datasets;};

  void GetDatasetNames(wxArrayString& array);
  void GetDatasetNames(wxComboBox& combo);

  bool InsertDataset(wxString& name);



  virtual void Dump(ostream& fOut = cerr);

protected:
  bool SaveConfigDataset();
  bool LoadConfigDataset();


protected:
  CObMap m_datasets;

private:
  void Init();

private:


};
//-------------------------------------------------------------
//------------------- CWorkspaceFormula class --------------------
//-------------------------------------------------------------

class CWorkspaceFormula: public CWorkspace
{
public:
  // constructors and destructors
  CWorkspaceFormula();
  CWorkspaceFormula(const wxString& name, const wxFileName& path, bool createPath = true);
  CWorkspaceFormula(const wxString& name, const wxString& path, bool createPath = true);

  virtual ~CWorkspaceFormula();

  virtual bool SaveConfig(bool flush = true);
  virtual bool LoadConfig();
  virtual bool Import(CWorkspace* wks);


  void AddFormula(CFormula& formula);
  void RemoveFormula(const wxString& name);

  wxString GetDescFormula(const wxString& name, bool alias = false);
  wxString GetCommentFormula(const wxString& name);
  CFormula* GetFormula(const wxString& name);
  CFormula* GetFormula(CMapFormula::iterator it);

  int32_t GetFormulaCount() {return m_formulas.size();};
  CMapFormula* GetFormulas() {return &m_formulas;};

  void GetFormulaNames(CStringMap& stringMap, bool getPredefined = true, bool getUser = true);
  void GetFormulaNames(wxArrayString& array, bool getPredefined = true, bool getUser = true);
  void GetFormulaNames(wxComboBox& combo, bool getPredefined = true, bool getUser = true);
  void GetFormulaNames(wxListBox& lb, bool getPredefined = true, bool getUser = true);

  wxArrayString& GetFormulasToImport() {return m_formulasToImport;};
  void SetFormulasToImport(const wxArrayString& array);

  bool IsFormulaToImport(const wxString& name);

  void AmendFormulas(const CStringArray& key, CProduct* product, const wxString& record);

  bool SaveConfigPredefinedFormula();

  virtual void Dump(ostream& fOut = cerr);

protected:

protected:
  CMapFormula m_formulas;

  wxArrayString m_formulasToImport;

private:
  void Init();
  bool LoadConfigFormula();
  bool SaveConfigFormula();


private:


};

//-------------------------------------------------------------
//------------------- CWorkspaceOperation class --------------------
//-------------------------------------------------------------

class CWorkspaceOperation: public CWorkspace
{
public:
  // constructors and destructors
  CWorkspaceOperation();
  CWorkspaceOperation(const wxString& name, const wxFileName& path, bool createPath = true);
  CWorkspaceOperation(const wxString& name, const wxString& path, bool createPath = true);

  virtual ~CWorkspaceOperation();

  virtual bool SaveConfig(bool flush = true);
  virtual bool LoadConfig();
  virtual bool Import(CWorkspace* wks);

  void GetOperationNames(wxArrayString& array);
  void GetOperationNames(wxComboBox& combo);

  bool HasOperation() {return GetOperationCount() > 0;};

  wxString GetOperationNewName();
  wxString GetOperationCopyName(const wxString& baseName);

  COperation* GetOperation(const wxString& name);

  CObMap* GetOperations() {return &m_operations;};

  int32_t GetOperationCount();

  bool InsertOperation(wxString& name);
  bool InsertOperation(wxString& name, COperation* operationToCopy);

  bool RenameOperation(COperation* operation, const wxString& newName);

  bool UseDataset(const wxString& name, CStringArray* operationNames = NULL);

  bool DeleteOperation(COperation* operation);


  virtual void Dump(ostream& fOut = cerr);

protected:

protected:
  bool SaveConfigOperation();
  bool LoadConfigOperation();

private:
  void Init();

private:
  CObMap m_operations;


};

//-------------------------------------------------------------
//------------------- CWorkspaceDisplay class --------------------
//-------------------------------------------------------------

class CWorkspaceDisplay: public CWorkspace
{
public:
  // constructors and destructors
  CWorkspaceDisplay();
  CWorkspaceDisplay(const wxString& name, const wxFileName& path, bool createPath = true);
  CWorkspaceDisplay(const wxString& name, const wxString& path, bool createPath = true);

  virtual ~CWorkspaceDisplay();

  virtual bool SaveConfig(bool flush = true);
  virtual bool LoadConfig();
  virtual bool Import(CWorkspace* wks);

  bool UseOperation(const wxString& name, CStringArray* displayNames = NULL);

  void GetDisplayNames(wxArrayString& array);
  void GetDisplayNames(wxComboBox& combo);

  bool HasDisplay() {return GetDisplayCount() > 0;};

  wxString GetDisplayNewName();

  CDisplay* GetDisplay(const wxString& name);

  int32_t GetDisplayCount();

  CObMap* GetDisplays() {return &m_displays;};

  bool InsertDisplay(wxString& name);

  bool DeleteDisplay(CDisplay* display);

  bool RenameDisplay(CDisplay* display, const wxString& newName);

  virtual void Dump(ostream& fOut = cerr);

protected:

protected:

private:
  void Init();
  bool SaveConfigDisplay();
  bool LoadConfigDisplay();


private:
  CObMap m_displays;


};


#endif
