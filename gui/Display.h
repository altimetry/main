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
#if !defined(_Display_h_)
#define _Display_h_

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/filename.h"
#include "wx/config.h"  // (to let wxWidgets choose a wxConfig class for your platform)
#include "wx/confbase.h"  // (base config class)
#include "wx/fileconf.h" // (wxFileConfig class)
//#include "wx/msw/regconf.h" // (wxRegConfig class)
#include "wx/dnd.h"

#include "brathl.h"

#include "BratObject.h"
#include "Field.h"
#include "LatLonRect.h"
using namespace brathl;


//#include "Dataset.h"
#include "Formula.h"
#include "Operation.h"

static const wxChar* displayDataFormatId = _T("CDndDisplayDataObject");


//-------------------------------------------------------------
//------------------- CDisplayData class --------------------
//-------------------------------------------------------------

class CDisplayData : public CBratObject
{
public:
  CDisplayData();
  CDisplayData(COperation* operation);
  CDisplayData(CDisplayData& d);
  ~CDisplayData();

  COperation* GetOperation() {return m_operation; };
  void SetOperation(COperation* value) {m_operation = value;};

  wxString GetGroupAsText() {return wxString::Format("%d", m_group);};
  int32_t GetGroup() {return m_group;};
  void SetGroup(int32_t value) {m_group = value;};

  wxString GetContourAsText() {return (m_withContour ? "Y" : "N");};
  bool IsContour() {return GetContour();};
  bool GetContour() {return m_withContour;};
  void SetContour(bool value) {m_withContour = value;};

  wxString GetSolidColorAsText() {return (m_withSolidColor ? "Y" : "N");};
  bool IsSolidColor() {return GetSolidColor();};
  bool GetSolidColor() {return m_withSolidColor;};
  void SetSolidColor(bool value) {m_withSolidColor = value;};

  wxString GetMinValueAsText() {return wxString::Format(CDisplayData::FMT_FLOAT_XY, m_minValue);};
  double GetMinValue() {return m_minValue;};
  void SetMinValue(double value) {m_minValue = value;};

  wxString GetMaxValueAsText() {return wxString::Format(CDisplayData::FMT_FLOAT_XY, m_maxValue);};
  double GetMaxValue() {return m_maxValue;};
  void SetMaxValue(double value) {m_maxValue = value;};

  wxString GetColorPalette() {return m_colorPalette;};
  void SetColorPalette(const wxString& value) {m_colorPalette = value;};

  CFieldBasic* GetField() {return &m_field;};
  CFieldBasic* GetX() {return &m_x;};
  CFieldBasic* GetY() {return &m_y;};
  CFieldBasic* GetZ() {return &m_z;};

  wxString GetXAxis() {return m_xAxis;};
  void SetXAxis(const wxString& value) {m_xAxis = value;};

  wxString GetXAxisText(const std::string& name);
  wxString GetXAxisText(uint32_t index);
  void SetXAxisText(uint32_t index, const wxString& value);

  bool HasXComplement();
  void GetXComplement(CStringArray& complement);

  void GetAvailableAxes(wxArrayString& names);
  void GetAvailableAxes(CStringArray& names);

  void GetAvailableDisplayTypes(CUIntArray& displayTypes);

  CObArray* GetDimFields() { return &m_dimFields; };

  bool IsYFXType();
  bool IsZYFXType();
  bool IsZLatLonType();

  bool IsInvertXYAxes() { return m_invertXYAxes;};
  void SetInvertXYAxes(bool value) { m_invertXYAxes = value;};

  bool IsEastComponent() { return m_eastcomponent;};
  void SetEastComponent(bool value) { m_eastcomponent = value;};

  bool IsNorthComponent() { return m_northcomponent;};
  void SetNorthComponent(bool value) { m_northcomponent = value;};


  CDisplayData& operator=(CDisplayData& d);

  wxString GetDataKey();
  wxString GetDataKey(int32_t type);

  int32_t GetType() {return m_type;};
  void SetType(int32_t value) {m_type = value;};


  bool LoadConfig(wxFileConfig* config, const wxString& path);
  bool SaveConfig(wxFileConfig* config, const wxString& pathSuff);

  void CopyFieldUserProperties(CDisplayData& d);

public:
  static const char* FMT_FLOAT_XY;
  //static const uint32_t NB_DIMS;


protected:
  void Copy(CDisplayData& d);

  void Init();

private:
  int32_t m_type;

  CFieldBasic m_field;
  CFieldBasic m_x;
  CFieldBasic m_y;
  CFieldBasic m_z;
  COperation* m_operation;
  int32_t m_group;
  bool m_withContour;
  bool m_withSolidColor;

  double m_minValue;
  double m_maxValue;

  wxString m_colorPalette;

  wxString m_xAxis;
  //wxString m_xAxisText;

  CObArray m_dimFields;

  bool m_invertXYAxes;

  bool m_eastcomponent;
  bool m_northcomponent;

};

//-------------------------------------------------------------
//------------------- CMapDisplayData class --------------------
//-------------------------------------------------------------
class CDisplay;

class CMapDisplayData: public CObMap
{
public:
  /// CMapDisplayData ctor
  CMapDisplayData();

  CMapDisplayData(CMapDisplayData& m);

  /// CMapDisplayData dtor
  virtual ~CMapDisplayData();

  CDisplayData* GetDisplayData(const std::string& name);
  CDisplayData* GetDisplayData(const wxString& name);
  CDisplayData* GetDisplayData(const char* name);
  CDisplayData* GetDisplayData(CMapDisplayData::const_iterator& it);

  bool AreFieldsGrouped();

  void SetGroups(bool groupFields);
  void GroupFields();
  void SplitFields();

  bool ValidName(const char* name);
  bool ValidName(const std::string& name);

  void NamesToArrayString(wxArrayString& array);
  void NamesToComboBox(wxComboBox& combo);

  void GetFiles(wxArrayString& array);

  void GetDistinctFiles(wxArrayString& array);
  void GetDistinctFiles(CStringMap& array);

  void GetFields(wxArrayString& array);

  void GetDistinctFields(wxArrayString& array);
  void GetDistinctFields(CStringMap& array);

  bool LoadConfig(wxFileConfig* config, const wxString& pathSuff = "");

  bool SaveConfig(wxFileConfig* config, const wxString& pathSuff = "");

  bool CheckFields(wxString& errorMsg, CDisplay* display);

  void SetAllAxis(uint32_t index, const wxString& axisName, const wxString& axisLabel);
  void SetAllInvertXYAxes(bool value);

  CMapDisplayData& operator=(CMapDisplayData& m);


protected:
  void Init();
  void Copy(CMapDisplayData& m);

public:


private :

  //static CSmartCleanerMapImageType	m_SmartCleaner;


};

//-------------------------------------------------------------
//------------------- CDisplay class --------------------
//-------------------------------------------------------------


class CDisplay : public CBratObject
{

public:

  /// Empty CDisplay ctor
  CDisplay(const wxString name);

  CDisplay(CDisplay& d);

  /// Destructor
  virtual ~CDisplay();

  wxString GetName() {return m_name;};
  void SetName(const wxString& value) {m_name = value;};

  bool HasData() {return GetDataCount() > 0;};
  int32_t GetDataCount() {return m_data.size();};

  bool SaveConfig(wxFileConfig* config);
  bool LoadConfig(wxFileConfig* config);

  static COperation* FindOperation(const wxString& name);

  bool UseOperation(const wxString& name);

  int32_t GetType() {return m_type;};
  void SetType(int32_t value) {m_type = value;};

  wxString GetTypeAsString() {return wxString::Format("%d", m_type);};

  CMapDisplayData* GetDataSelected() {return &m_data;};
  wxString FmtCmdParam(const std::string& name);

  bool ExistData(const wxString& key);
  bool InsertData(const wxString& key, CDisplayData* data);
  bool RemoveData(const wxString& key);

  wxString GetTitle() { return m_title; };
  void SetTitle(const wxString& value) { m_title = value; };

  wxString GetProjection() { return m_projection; };
  void SetProjection(const wxString& value) { m_projection = value; };

  wxString GetWithAnimationAsText() {return (m_withAnimation ? "Y" : "N");};
  bool GetWithAnimation() { return m_withAnimation; };
  void SetWithAnimation(bool value) { m_withAnimation = value; };

  wxString GetMinXValueAsText() {return wxString::Format(CDisplayData::FMT_FLOAT_XY, m_minXValue);};
  double GetMinXValue() {return m_minXValue;};
  void SetMinXValue(double value) {m_minXValue = value;};

  wxString GetMaxXValueAsText() {return wxString::Format(CDisplayData::FMT_FLOAT_XY, m_maxXValue);};
  double GetMaxXValue() {return m_maxXValue;};
  void SetMaxXValue(double value) {m_maxXValue = value;};

  wxString GetMinYValueAsText() {return wxString::Format(CDisplayData::FMT_FLOAT_XY, m_minYValue);};
  double GetMinYValue() {return m_minYValue;};
  void SetMinYValue(double value) {m_minYValue = value;};

  wxString GetMaxYValueAsText() {return wxString::Format(CDisplayData::FMT_FLOAT_XY, m_maxYValue);};
  double GetMaxYValue() {return m_maxYValue;};
  void SetMaxYValue(double value) {m_maxYValue = value;};


  CDisplayData* GetDisplayData(const wxString& name);
  CDisplayData* GetDisplayData(CMapDisplayData::iterator it);

  wxFileName* GetOutput() {return GetCmdFile();};
  wxString GetOutputName() {return GetCmdFileFullName();};
  void SetOutput(const wxFileName& value) {SetCmdFile(value);};
  void SetOutput(const wxString& value) {SetCmdFile(value);};

  bool AreFieldsGrouped();

  void SetGroups(bool groupFields);

  wxString GetTaskName();
  wxFileName* GetCmdFile() {return &m_cmdFile;};
  wxString GetCmdFileFullName() {return m_cmdFile.GetFullPath();};
  void SetCmdFile(const wxFileName& value) {m_cmdFile = value; m_cmdFile.Normalize();};
  void SetCmdFile(const wxString& value) {m_cmdFile.Assign(value); m_cmdFile.Normalize();};

  void SetAllAxis(uint32_t index, const wxString& axisName, const wxString& axisLabel) { m_data.SetAllAxis(index, axisName, axisLabel); };
  void SetAllInvertXYAxes(bool value) { m_data.SetAllInvertXYAxes(value); };

  CLatLonRect* GetZoom() { return &m_zoom; };
  void SetZoom(CLatLonRect& value) { m_zoom = value; };

  wxString GetFullCmd();

  bool BuildCmdFile();
  bool BuildCmdFileHeader();
  bool BuildCmdFileGeneralProperties();
  bool BuildCmdFileGeneralPropertiesXY();
  bool BuildCmdFileGeneralPropertiesZXY();
  bool BuildCmdFileGeneralPropertiesZLatLon();
  bool BuildCmdFileFields();
  bool BuildCmdFileFieldsZFXY();
  bool BuildCmdFileFieldsYFX();
  bool BuildCmdFileFieldProperties(const wxString& dataKey);
  bool BuildCmdFileFieldPropertiesXY(CDisplayData* value);
  bool BuildCmdFileFieldPropertiesZXY(CDisplayData* value);

  bool BuildCmdFileVerbose();

  bool WriteComment(const wxString& text);
  bool WriteLine(const wxString& text);
  bool WriteEmptyLine();

  void InitOutput();

  bool IsYFXType();
  bool IsZYFXType();
  bool IsZLatLonType();

  CDisplay& operator=(CDisplay& d);


   ///Dump fonction
   virtual void Dump(std::ostream& fOut = std::cerr);

protected:
  void Copy(CDisplay& d);
  void Init();
protected:

  int32_t m_type;
  //wxFileName m_output;
  wxFileName m_cmdFile;
  wxFile m_file;
  int32_t m_verbose;
  CMapDisplayData m_data;
  wxString m_title;
  bool m_withAnimation;

  double m_minXValue;
  double m_maxXValue;

  double m_minYValue;
  double m_maxYValue;

  wxString m_projection;

  wxString m_name;

  CLatLonRect m_zoom;

  static const std::string m_zoomDelimiter;

};

//----------------------------------------------------------------------------
// CDndDisplayData
//----------------------------------------------------------------------------
//class CDisplayData;

class CDndDisplayData
{

public:

  CDndDisplayData() : m_data(false) {  };
  //CDndDisplayData()  { m_displayData = NULL; };
  CDndDisplayData(CDisplayData* displayData) { m_data.Insert(displayData); };
  CDndDisplayData(const CObArray& data) { Set(data); };
  CDndDisplayData(const CObArray* data) { Set(data); };

  CDndDisplayData(CDndDisplayData& dndDisplayData);

  ~CDndDisplayData() { };

  //void AddField(CField* field);
  //void AddField(CObArray& fields);

  //void GetField(CObArray& fields);

  //CField* Item(int32_t nIndex) const {return dynamic_cast<CField*>(m_fields.at(nIndex));}
  //size_t GetCount() const {return m_fields.size();}

  //CDisplayData* m_displayData;

  CObArray m_data;

  void Set(CDndDisplayData& dndDisplayData);
  void Set(const CObArray& data);
  void Set(const CObArray* data);

};

//----------------------------------------------------------------------------
// CDndDisplayDataObject
//----------------------------------------------------------------------------

class CDndDisplayDataObject : public wxDataObjectSimple
{
   public:

      CDndDisplayDataObject(CDndDisplayData* dndDisplayData = NULL);
      virtual ~CDndDisplayDataObject() {delete m_dndDisplayData;}

      virtual size_t GetDataSize() const;
      virtual bool GetDataHere(void* buf) const;
      virtual bool SetData(size_t len, const void* buf);

      CDndDisplayData* GetDisplayData() {return m_dndDisplayData;}

   private:
      CDndDisplayData* m_dndDisplayData;
};

/** @} */


#endif // !defined(_Parameter_h_)
