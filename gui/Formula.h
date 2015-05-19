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
#if !defined(_Formula_h_)
#define _Formula_h_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "Formula.h"
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

#include "List.h"
#include "Unit.h"
#include "Date.h"
using namespace brathl;

const wxString DEFAULT_UNIT_COUNT = "count";
//const wxString DEFAULT_UNIT_LONGITUDE = "degrees_east";
//const wxString DEFAULT_UNIT_LATITUDE = "degrees_north";
//const wxString DEFAULT_UNIT_SECOND = "second";

const wxString DEFAULT_STEP_LATLON_ASSTRING = "1/3";
const wxString DEFAULT_STEP_TIME_ASSTRING = "1"; 
const wxString DEFAULT_STEP_GENERAL_ASSTRING = "1";

const wxString FORMAT_FILTER_LABEL = "Filter to apply on '%s': %s";
const wxString FORMAT_FILTER_LABEL_NONE = "No filter to apply on '%s'";
const wxString FORMAT_LOESS_CUT_OFF_LABEL = "Loess cut-off: %s";
const wxString FORMAT_INTERVAL_LABEL = "Number of intervals: %s";
const wxString FORMAT_XY_RESOLUTION_LABEL = "Min.: %s   Max.: %s   %s   Step: %s   %s";


//-------------------------------------------------------------
//------------------- CMapTypeFilter class --------------------
//-------------------------------------------------------------

class CMapTypeFilter: public CUIntMap
{
public:
  /// CIntMap ctor
  CMapTypeFilter();

  /// CIntMap dtor
  virtual ~CMapTypeFilter();

  static CMapTypeFilter&  GetInstance();


  bool ValidName(const char* name);
  bool ValidName(const string& name);

  wxString IdToName(uint32_t id);
  uint32_t NameToId(const wxString& name);

  void NamesToArrayString(wxArrayString& array);
  void NamesToComboBox(wxComboBox& combo);

protected:

public:

  enum typeFilter
  {
    filterNone,
    filterLoessSmooth,
    filterLoessExtrapolate, 
    filterLoess
  };


private :


};

//-------------------------------------------------------------
//------------------- CMapTypeData class --------------------
//-------------------------------------------------------------

class CMapTypeData: public CUIntMap
{
public:
  /// CIntMap ctor
  CMapTypeData();

  /// CIntMap dtor
  virtual ~CMapTypeData();

  static CMapTypeData&  GetInstance();


  bool ValidName(const char* name);
  bool ValidName(const string& name);

  wxString IdToName(uint32_t id);
  uint32_t NameToId(const wxString& name);

  void NamesToArrayString(wxArrayString& array, bool noData = false);
  void NamesToComboBox(wxComboBox& combo, bool noData = false);

protected:

public:
  enum typeData
  {
    typeOpData,
    typeOpLongitude,
    typeOpLatitude,
    typeOpX,
    typeOpY,
    typeOpZ,
    typeOpT
  };



private :


};
//-------------------------------------------------------------
//------------------- CMapTypeOp class --------------------
//-------------------------------------------------------------

class CMapTypeOp: public CUIntMap
{
public:
  CMapTypeOp();

  virtual ~CMapTypeOp();

  static CMapTypeOp&  GetInstance();


  bool ValidName(const char* name);
  bool ValidName(const string& name);

  wxString IdToName(uint32_t id);
  uint32_t NameToId(const wxString& name);

  void NamesToArrayString(wxArrayString& array);
  void NamesToComboBox(wxComboBox& combo);

protected:

public:

  enum typeOp
  {
    typeOpYFX,
    typeOpZFXY
  };



private :


};
/*
//-------------------------------------------------------------
//------------------- CMapTypeDisp class --------------------
//-------------------------------------------------------------

class CMapTypeDisp: public CUIntMap
{
public:
  CMapTypeDisp();

  virtual ~CMapTypeDisp();

  static CMapTypeDisp&  GetInstance();


  bool ValidName(const char* name);
  bool ValidName(const string& name);

  wxString IdToName(uint32_t id);
  uint32_t NameToId(const wxString& name);

  void NamesToArrayString(wxArrayString& array);
  void NamesToComboBox(wxComboBox& combo);

  wxString Enum();


protected:

public:

  enum typeDisp
  {
    typeDispYFX,
    typeDispZFXY,
    typeDispZFLatLon
  };



private :


};
*/
//-------------------------------------------------------------
//------------------- CMapTypeField class --------------------
//-------------------------------------------------------------

class CMapTypeField: public CUIntMap
{
public:
  CMapTypeField();

  virtual ~CMapTypeField();

  static CMapTypeField&  GetInstance();


  bool ValidName(const char* name);
  bool ValidName(const string& name);

  wxString IdToName(uint32_t id);
  uint32_t NameToId(const wxString& name);

  void NamesToArrayString(wxArrayString& array);
  void NamesToComboBox(wxComboBox& combo);

protected:

public:
  enum typeField
  {
    typeOpAsField,
    typeOpAsX,
    typeOpAsY,
    typeOpAsSelect
  };




private :


};
//-------------------------------------------------------------
//------------------- CMapDataMode class --------------------
//-------------------------------------------------------------

class CMapDataMode: public CUIntMap
{
public:
  CMapDataMode();

  virtual ~CMapDataMode();

  static CMapDataMode&  GetInstance();


  bool ValidName(const char* name);
  bool ValidName(const string& name);

  wxString IdToName(uint32_t id);
  uint32_t NameToId(const wxString& name);

  void NamesToArrayString(wxArrayString& array);
  void NamesToComboBox(wxComboBox& combo);

  uint32_t GetDefault();
  wxString GetDefaultAsString();

protected:

public:


private :


};
//-------------------------------------------------------------
//------------------- CFormula class --------------------
//-------------------------------------------------------------

class CFormula : public CBratObject
{

public:


  CFormula();
  CFormula(const wxString& name, bool predefined, const wxString& description = "", const wxString& comment = "");
  CFormula(const string& name, bool predefined, const string& description = "", const string& comment = "");
  CFormula(const CFormula& formula);


  /// Destructor
  virtual ~CFormula();


  wxString GetName() {return m_name;};
  void SetName(const wxString& value) {m_name = value;};

  wxString GetDescription(bool removeCRLF = false, const CStringMap* formulaAliases = NULL, const CStringMap* fieldAliases = NULL);
  void SetDescription(const wxString& value) {m_description = value;};
  void SetDescription(const string& value) {m_description = value.c_str();};
  void SetDescription(const char* value) {m_description = value;};
  
  wxString GetAlias();

  wxString GetComment(bool removeCRLF = false);
  void SetComment(const wxString& value) {m_comment = value;};
  
  bool IsPredefined() {return m_predefined;};
  void SetPredefined( bool value) {m_predefined = value;};

  bool LoadConfig(wxFileConfig* config);
  bool LoadConfig(wxFileConfig* config, const wxString& pathSuff = "");
  bool LoadConfigDesc(wxFileConfig* config, const wxString& path);

  bool SaveConfig(wxFileConfig* config);
  bool SaveConfig(wxFileConfig* config, const wxString& pathSuff = "");
  
  bool SaveConfigDesc(wxFileConfig* config, const wxString& path);
  
  bool SaveConfigPredefined(wxFileConfig* config, const wxString& pathSuff);

  CFormula& operator=(const CFormula& formula);

  wxString GetFilterAsString() {return CMapTypeFilter::GetInstance().IdToName(m_filter);};
  int32_t GetFilter() {return m_filter;};
  void SetFilter(int32_t value) {m_filter = value;}; 
  
  void SetFilterDefault() {SetFilter(CMapTypeFilter::filterNone);}; 

  wxString GetMinValueAsDateString(const wxString& format = "") {return CFormula::GetAsDateString(m_minValue, format);};
  wxString GetMinValueAsText();
  wxString GetMinAsString();
  double GetMinValue() {return m_minValue;};
  void SetMinValue(double value) {m_minValue = value;}; 
  void SetMinValue(CDate& d); 
  void SetMinValueFromDateString(const wxString& value); 

  void SetMinValueDefault() {CTools::SetDefaultValue(m_minValue);}; 
  

  wxString GetMaxValueAsDateString(const wxString& format = "") {return CFormula::GetAsDateString(m_maxValue, format);};
  wxString GetMaxValueAsText();
  wxString GetMaxAsString();
  double GetMaxValue() {return m_maxValue;};
  void SetMaxValue(double value) {m_maxValue = value;}; 
  void SetMaxValue(CDate& d); 
  void SetMaxValueFromDateString(const wxString& value); 

  void SetMaxValueDefault() {CTools::SetDefaultValue(m_maxValue);}; 

  static double GetStepAsDouble(const wxString& step);
  double GetStepAsDouble();
  wxString GetStep() {return m_step;};
  void SetStep(const wxString& value) {m_step = value;}; 

  wxString GetIntervalAsText() {return wxString::Format("%d", m_interval);};
  int32_t GetInterval() {return m_interval;};
  void SetInterval(int32_t value) {m_interval = value;}; 

  void SetIntervalDefault() {CTools::SetDefaultValue(m_interval);}; 

  wxString GetLoessCutOffAsText() {return wxString::Format("%d", m_loessCutOff);};
  int32_t GetLoessCutOff() {return m_loessCutOff;};
  void SetLoessCutOff(int32_t value) {m_loessCutOff = value;}; 

  void SetLoessCutOffDefault() {CTools::SetDefaultValue(m_loessCutOff);}; 

  wxString GetTypeAsString() {return CMapTypeField::GetInstance().IdToName(m_type);};
  int32_t GetType() {return m_type;};
  void SetType(int32_t value) {m_type = value;}; 

  wxString GetDataTypeAsString() {return CMapTypeData::GetInstance().IdToName(m_dataType);};
  int32_t GetDataType() {return m_dataType;};
  void SetDataType(int32_t value) {m_dataType = value;}; 
  void SetDataType(int32_t typeField, const CUnit& unit, CProduct* product = NULL);


  void SetDefaultMinMax();

  wxString GetFormatString();

  wxString GetFilterAsLabel();
  wxString GetResolutionAsLabel(bool hasFilter);

  wxString GetTitle() {return m_title;};
  void SetTitle(const wxString& value) {m_title = value;};
  
  CUnit* GetUnit() {return &m_unit;};
  wxString GetUnitAsText() {return m_unit.GetText().c_str();};
  void SetUnit(const wxString& value, const wxString& defaultValue = "", bool withMsg = true, bool convertMinMax = false);
  void SetUnit(const CUnit& value, const wxString& defaultValue = "", bool withMsg = true, bool convertMinMax = false);

  wxString GetDefaultUnit();
  static wxString GetDefaultUnit(int32_t dataType);

  void SetDefaultUnit();
  bool IsDefaultUnit();
  static bool IsDefaultUnit(const char * value, int32_t dataType);


  static bool IsUnitCompatible(int32_t dataType, const wxString& unitText);
  static bool IsUnitCompatible(int32_t dataType, const CUnit& unit);
  bool IsUnitCompatible(const CUnit& unit);
  bool IsUnitCompatible(const wxString& unitText);
  bool IsUnitCompatible();

  static bool IsDefaultUnitData(const CUnit& unit);
  static bool IsDefaultUnitData(const wxString& unitText);
  bool IsDefaultUnitData();

  bool ControlUnitConsistency(wxString& errorMsg);

  static bool CheckFieldNames(const CExpression& expr, const wxString& record, CProduct* product, CStringArray& fieldNamesNotFound);
  static bool CheckFieldNames(const CExpression& expr, const wxString& record, CProduct* product, CExpression& exprOut, wxString& errorMsg);
  static bool CheckFieldNames(const wxString& expr, const wxString& record, CProduct* product, wxString& exprOut, wxString& errorMsg);

  static bool CheckExpression(const wxString& value, const wxString& record, wxString& errorMsg, wxString* strUnitExpr = NULL,
                              const CStringMap* aliases = NULL, CProduct* product = NULL, wxString* valueOut = NULL);
  bool CheckExpression( wxString& errorMsg, const wxString& record,
                        const CStringMap* aliases = NULL, CProduct* product = NULL, wxString* valueOut = NULL);

  static bool CheckExpressionUnits(const wxString& exprStr, const wxString& record, const wxString& strUnitExpr, CProduct* product, wxString& errorMsg);

  bool GetFields(CStringArray& fields, wxString& errorMsg, const CStringMap* aliases = NULL, const CStringMap* fieldAliases = NULL);

  static bool SetExpression(const wxString& value, CExpression& expr, wxString& errorMsg);
  static bool SetExpression(const string& value, CExpression& expr, wxString& errorMsg);
  static bool SetExpression(const char* value, CExpression& expr, wxString& errorMsg);

  double LonNormal360(double lon);

  bool ControlResolution(wxString& errorMsg);

  bool CtrlMinMaxValue(wxString& errorMsg);

  bool IsFieldType();
  bool IsXType();
  bool IsYType();
  bool IsXYType();

  bool IsDataTypeSet();
  bool IsLonDataType();
  bool IsLatDataType();
  bool IsLatLonDataType();

  bool IsXYLatLon();
  bool IsTimeDataType();
  bool IsXYTime();

  wxString GetFieldPrefix();
  wxString GetFieldPrefix(int32_t type);
  wxString GetExportAsciiFieldPrefix();


  void ConvertToMinMaxFormulaUnit();
  void ConvertToMinMaxFormulaBaseUnit();
  
  double ConvertToFormulaBaseUnit(double in);
  bool ConvertToFormulaBaseUnit(double in, double& out);
  
  double ConvertToFormulaUnit(double in);
  bool ConvertToFormulaUnit(double in, double& out);

  bool ComputeInterval(bool showMsg = false);
  //bool ComputeIntervalAsDouble(bool showMsg = false);
  //bool ComputeIntervalAsDate(bool showMsg = false);

  void SetStepToDefaultAsNecessary();
  void SetStepToDefault();
  wxString GetDefaultStep();

  static wxString GetAsDateString(double seconds, const wxString& format = "");

  wxString GetDataModeAsString() {return CMapDataMode::GetInstance().IdToName(m_dataMode);};
  int32_t GetDataMode() {return m_dataMode;};
  void SetDataMode(int32_t value) {m_dataMode = value;};

   ///Dump fonction
   virtual void Dump(ostream& fOut = cerr);

public:
  static const char* FMT_FLOAT_XY;
  static const char* FMT_FLOAT_LATLON;

protected:
  void Init();
  void Copy(const CFormula& formula);

protected:

  wxString m_name;
  wxString m_description;
  wxString m_comment;
  int32_t m_type;

  CUnit m_unit;

  int32_t m_dataType; 

  wxString m_title;

  int32_t m_filter;
  
  double m_minValue;
  double m_maxValue;
  int32_t m_interval;
  wxString m_step;
  int32_t m_loessCutOff;

  bool m_predefined;

  int32_t m_dataMode;

};


//-------------------------------------------------------------
//------------------- CMapFormula class --------------------
//-------------------------------------------------------------

class CMapFormula: public CObMap
{
public:
  /// CIntMap ctor
  CMapFormula(bool withPredefined = true);

  /// CIntMap dtor
  virtual ~CMapFormula();

  CMapFormula(CMapFormula& m);

  //static CMapFormula&  GetInstance();

  wxString GetDescFormula(const wxString& name, bool alias = false);
//  bool SetDescFormula(const wxString& name, const wxString& description);

  CFormula*  GetFormula(int32_t type);
  
  void InitFormulaDataMode(int32_t dataMode);


  wxString GetCommentFormula(const wxString& name);
  bool SetCommentFormula(const wxString& name, const wxString& comment);

  bool ValidName(const char* name);
  bool ValidName(const string& name);
  
  void NamesToArrayString(wxArrayString& array);
  void NamesToComboBox(wxComboBox& combo);

  int32_t CountDataFields();

  bool HasFilters();
  
  void Amend(const CStringArray& keys, CProduct* product, const wxString& record);

  void InsertPredefined();
  void InsertUserDefined(const wxFileName& fileName);
  void InsertUserDefined(wxFileConfig* config);
  void InsertUserDefined(CFormula* formula);
  void InsertUserDefined(CFormula& formula);

  bool LoadConfig(bool predefined);
  bool LoadConfig(wxFileConfig* config, bool predefined, const wxString& pathSuff = "");

  bool SaveConfig(wxFileConfig* config, bool predefined, const wxString& pathSuff = "");
  
  CMapFormula& operator=(CMapFormula& m);


protected:

  void DeleteConfig();
  void Init();
  void Copy(CMapFormula& m);


protected:

  wxFileConfig* m_config;

public:

  static wxString m_predefFormulaFile;

private :



};


/** @} */


#endif 
