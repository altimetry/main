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
#if !defined(_Formula_h_)
#define _Formula_h_

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/filename.h"
#include "wx/config.h"  // (to let wxWidgets choose a wxConfig class for your platform)
#include "wx/confbase.h"  // (base config class)
#include "wx/fileconf.h" // (wxFileConfig class)
//#include "wx/msw/regconf.h" // (wxRegConfig class)


#include "new-gui/QtInterface.h"


#include "brathl.h"

#include "List.h"
#include "Unit.h"
#include "Date.h"
using namespace brathl;

//const std::string DEFAULT_UNIT_COUNT = "count";

////const std::string DEFAULT_UNIT_LONGITUDE = "degrees_east";
////const std::string DEFAULT_UNIT_LATITUDE = "degrees_north";
////const std::string DEFAULT_UNIT_SECOND = "second";

//const std::string DEFAULT_STEP_LATLON_ASSTRING = "1/3";
//const std::string DEFAULT_STEP_TIME_ASSTRING = "1"; 
//const std::string DEFAULT_STEP_GENERAL_ASSTRING = "1";

//const std::string FORMAT_FILTER_LABEL = "Filter to apply on '%s': %s";
//const std::string FORMAT_FILTER_LABEL_NONE = "No filter to apply on '%s'";
//const std::string FORMAT_LOESS_CUT_OFF_LABEL = "Loess cut-off: %s";
//const std::string FORMAT_INTERVAL_LABEL = "Number of intervals: %s";
//const std::string FORMAT_XY_RESOLUTION_LABEL = "Min.: %s   Max.: %s   %s   Step: %s   %s";
//

//-------------------------------------------------------------
//------------------- CMapTypeFilter class --------------------
//-------------------------------------------------------------

class CMapTypeFilter : public CUIntMap
{
public:

	enum typeFilter
	{
		filterNone,
		filterLoessSmooth,
		filterLoessExtrapolate,
		filterLoess
	};

public:
	/// CIntMap ctor
	CMapTypeFilter();

	/// CIntMap dtor
	virtual ~CMapTypeFilter();

	static CMapTypeFilter&  GetInstance();


	bool ValidName( const char* name );
	bool ValidName( const std::string& name );

	std::string IdToName( uint32_t id );
	uint32_t NameToId( const std::string& name );

	void NamesToArrayString( wxArrayString& array );
	void NamesToComboBox( wxComboBox& combo );
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
  bool ValidName(const std::string& name);

  std::string IdToName(uint32_t id);
  uint32_t NameToId(const std::string& name);

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
  bool ValidName(const std::string& name);

  std::string IdToName(uint32_t id);
  uint32_t NameToId(const std::string& name);

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
  bool ValidName(const std::string& name);

  std::string IdToName(uint32_t id);
  uint32_t NameToId(const std::string& name);

  void NamesToArrayString(wxArrayString& array);
  void NamesToComboBox(wxComboBox& combo);

  std::string Enum();


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
  bool ValidName(const std::string& name);

  std::string IdToName(uint32_t id);
  uint32_t NameToId(const std::string& name);

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
  bool ValidName(const std::string& name);

  std::string IdToName(uint32_t id);
  uint32_t NameToId(const std::string& name);

  void NamesToArrayString(wxArrayString& array);
  void NamesToComboBox(wxComboBox& combo);

  uint32_t GetDefault();
  std::string GetDefaultAsString();

protected:

public:


private :


};
//-------------------------------------------------------------
//------------------- CFormula class --------------------
//-------------------------------------------------------------

class CFormula : public CBratObject
{
protected:

	std::string m_name;
	std::string m_title;
	std::string m_description;
	std::string m_comment;
	int32_t m_type = CMapTypeField::typeOpAsField;

	CUnit m_unit;

	int32_t m_dataType = -1;

	int32_t m_filter = CMapTypeFilter::filterNone;

	double m_minValue = defaultValue< double >();
	double m_maxValue = defaultValue< double >();
	int32_t m_interval = defaultValue< int32_t >();
	std::string m_step;
	int32_t m_loessCutOff = defaultValue< int32_t >();

	bool m_predefined = false;

	int32_t m_dataMode = CMapDataMode::GetInstance().GetDefault();

public:
	CFormula()
	{}

	CFormula( const std::string& name, bool predefined, const std::string& description = "", const std::string& comment = "" ) :
		m_name( name )
		, m_description( description )
		, m_comment( comment )
		, m_predefined( predefined )
	{}

	CFormula( const CFormula &o )
	{
		*this = o;
	}

    CFormula& operator = ( const CFormula &o )
	{
		if ( this != &o )
		{
			m_name = o.m_name;
			m_description = o.m_description;
			m_comment = o.m_comment;
			m_predefined = o.m_predefined;
			m_type = o.m_type;
			m_dataType = o.m_dataType;
			m_filter = o.m_filter;
			m_interval = o.m_interval;
			m_loessCutOff = o.m_loessCutOff;
			m_minValue = o.m_minValue;
			m_maxValue = o.m_maxValue;
			m_step = o.m_step;
			m_title = o.m_title;
			m_unit = o.m_unit;
			m_dataMode = o.m_dataMode;
		}
		return *this;
	}

	/// Destructor
	virtual ~CFormula()
	{}

	std::string GetName() { return m_name; };
	void SetName( const std::string& value ) { m_name = value; };

	std::string GetDescription( bool removeCRLF = false, const CStringMap* formulaAliases = NULL, const CStringMap* fieldAliases = NULL );
	void SetDescription( const std::string& value ) { m_description = value; };
	void SetDescription( const char* value ) { m_description = value; };

	std::string GetAlias();

	std::string GetComment( bool removeCRLF = false );
	void SetComment( const std::string& value ) { m_comment = value; };

	bool IsPredefined() { return m_predefined; };
	void SetPredefined( bool value ) { m_predefined = value; };

	bool LoadConfig( wxFileConfig* config );
	bool LoadConfig( wxFileConfig* config, const std::string& pathSuff = "" );
	bool LoadConfigDesc( wxFileConfig* config, const std::string& path );

	bool SaveConfig( wxFileConfig* config );
	bool SaveConfig( CConfiguration *config, const std::string& pathSuff = "" );

	bool SaveConfigDesc( CConfiguration *config, const std::string& path );

	bool SaveConfigPredefined( CConfiguration *config, const std::string& pathSuff );

	std::string GetFilterAsString() { return CMapTypeFilter::GetInstance().IdToName( m_filter ); };
	int32_t GetFilter() { return m_filter; };
	void SetFilter( int32_t value ) { m_filter = value; };

	void SetFilterDefault() { SetFilter( CMapTypeFilter::filterNone ); };

	std::string GetMinValueAsText();
	std::string GetMinAsString();
	double GetMinValue() { return m_minValue; };
	void SetMinValue( double value ) { m_minValue = value; };
	void SetMinValue( CDate& d );
	void SetMinValueFromDateString( const std::string& value );

	void SetMinValueDefault() { setDefaultValue( m_minValue ); };

	std::string GetMinValueAsDateString() { return GetAsDateString( m_minValue); }
	std::string GetMaxValueAsDateString() {  return GetAsDateString( m_maxValue); }

	std::string GetValueAsString( double value );
	std::string GetFormatString();

	std::string GetMaxValueAsText();
	std::string GetMaxAsString();
	double GetMaxValue() { return m_maxValue; };
	void SetMaxValue( double value ) { m_maxValue = value; };
	void SetMaxValue( CDate& d );
	void SetMaxValueFromDateString( const std::string& value );

	void SetMaxValueDefault() { setDefaultValue( m_maxValue ); };

	static double GetStepAsDouble( const std::string& step );
	double GetStepAsDouble();
	std::string GetStep() { return m_step; };
	void SetStep( const std::string& value ) { m_step = value; };

	std::string GetIntervalAsText() { return n2s< std::string >( m_interval ); };
	int32_t GetInterval() { return m_interval; };
	void SetInterval( int32_t value ) { m_interval = value; };

	void SetIntervalDefault() { setDefaultValue( m_interval ); };

	std::string GetLoessCutOffAsText() { return n2s< std::string >( m_loessCutOff ); };
	int32_t GetLoessCutOff() { return m_loessCutOff; };
	void SetLoessCutOff( int32_t value ) { m_loessCutOff = value; };

	void SetLoessCutOffDefault() { setDefaultValue( m_loessCutOff ); };

	std::string GetTypeAsString() { return CMapTypeField::GetInstance().IdToName( m_type ); };
	int32_t GetType() { return m_type; };
	void SetType( int32_t value ) { m_type = value; };

	std::string GetDataTypeAsString() { return CMapTypeData::GetInstance().IdToName( m_dataType ); };
	int32_t GetDataType() { return m_dataType; };
	void SetDataType( int32_t value ) { m_dataType = value; };
	void SetDataType( int32_t typeField, const CUnit& unit, CProduct* product = NULL );


	void SetDefaultMinMax();

	std::string GetFilterAsLabel();
	std::string GetResolutionAsLabel( bool hasFilter );

	std::string GetTitle() { return m_title; };
	void SetTitle( const std::string& value ) { m_title = value; };

	CUnit* GetUnit() { return &m_unit; };
	std::string GetUnitAsText() { return m_unit.GetText().c_str(); };
	void SetUnit( const std::string& value, const std::string& defaultValue = "", bool withMsg = true, bool convertMinMax = false );
	void SetUnit( const CUnit& value, const std::string& defaultValue = "", bool withMsg = true, bool convertMinMax = false );

	std::string GetDefaultUnit();
	static std::string GetDefaultUnit( int32_t dataType );

	void SetDefaultUnit();
	bool IsDefaultUnit();
	static bool IsDefaultUnit( const char * value, int32_t dataType );


	static bool IsUnitCompatible( int32_t dataType, const std::string& unitText );
	static bool IsUnitCompatible( int32_t dataType, const CUnit& unit );
	bool IsUnitCompatible( const CUnit& unit );
	bool IsUnitCompatible( const std::string& unitText );
	bool IsUnitCompatible();

	static bool IsDefaultUnitData( const CUnit& unit );
	static bool IsDefaultUnitData( const std::string& unitText );
	bool IsDefaultUnitData();

	bool ControlUnitConsistency( std::string& errorMsg );

	static bool CheckFieldNames( const CExpression& expr, const std::string& record, CProduct* product, CStringArray& fieldNamesNotFound );
	static bool CheckFieldNames( const CExpression& expr, const std::string& record, CProduct* product, CExpression& exprOut, std::string& errorMsg );
	static bool CheckFieldNames( const std::string& expr, const std::string& record, CProduct* product, std::string& exprOut, std::string& errorMsg );

	static bool CheckExpression( const std::string& value, const std::string& record, std::string& errorMsg, std::string* strUnitExpr = NULL,
		const CStringMap* aliases = NULL, CProduct* product = NULL, std::string* valueOut = NULL );
	bool CheckExpression( std::string& errorMsg, const std::string& record,
		const CStringMap* aliases = NULL, CProduct* product = NULL, std::string* valueOut = NULL );

	static bool CheckExpressionUnits( const std::string& exprStr, const std::string& record, const std::string& strUnitExpr, CProduct* product, std::string& errorMsg );

	bool GetFields( CStringArray& fields, std::string& errorMsg, const CStringMap* aliases = NULL, const CStringMap* fieldAliases = NULL );

	static bool SetExpression( const std::string& value, CExpression& expr, std::string& errorMsg );
	static bool SetExpression( const char* value, CExpression& expr, std::string& errorMsg );

	double LonNormal360( double lon );

	bool ControlResolution( std::string& errorMsg );

	bool CtrlMinMaxValue( std::string& errorMsg );

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

	std::string GetFieldPrefix();
	std::string GetFieldPrefix( int32_t type );
	std::string GetExportAsciiFieldPrefix();


	void ConvertToMinMaxFormulaUnit();
	void ConvertToMinMaxFormulaBaseUnit();

	double ConvertToFormulaBaseUnit( double in );
	bool ConvertToFormulaBaseUnit( double in, double& out );

	double ConvertToFormulaUnit( double in );
	bool ConvertToFormulaUnit( double in, double& out );

	bool ComputeInterval( bool showMsg = false );
	//bool ComputeIntervalAsDouble(bool showMsg = false);
	//bool ComputeIntervalAsDate(bool showMsg = false);

	void SetStepToDefaultAsNecessary();
	void SetStepToDefault();
	std::string GetDefaultStep();

	static std::string GetAsDateString( double seconds );

	std::string GetDataModeAsString() { return CMapDataMode::GetInstance().IdToName( m_dataMode ); };
	int32_t GetDataMode() { return m_dataMode; };
	void SetDataMode( int32_t value ) { m_dataMode = value; };

	///Dump fonction
	virtual void Dump( std::ostream& fOut = std::cerr );
//
//protected:
//	static const char* FMT_FLOAT_XY;
//	static const char* FMT_FLOAT_LATLON;
};


//-------------------------------------------------------------
//------------------- CMapFormula class --------------------
//-------------------------------------------------------------

class CMapFormula : public CObMap
{
protected:
	CConfiguration *m_config = nullptr;

public:
	static std::string m_predefFormulaFile;

public:
	/// CIntMap ctor
	CMapFormula( bool withPredefined = true )
	{
		if ( withPredefined )
			InsertPredefined();
	}

	CMapFormula( CMapFormula &o )
	{
		*this = o;
	}

	CMapFormula& operator = ( CMapFormula &o )
	{
		if ( this != &o )
		{
			RemoveAll();
			for ( CMapFormula::iterator it = o.begin(); it != o.end(); it++ )
			{
				CFormula* value = dynamic_cast<CFormula*>( it->second );
				if ( value == nullptr )
					continue;

				Insert( value->GetName(), new CFormula( *value ) );
			}
		}
		return *this;
	}

	/// CIntMap dtor
	virtual ~CMapFormula()
	{
		delete m_config;
	}


	//static CMapFormula&  GetInstance();

	std::string GetDescFormula( const std::string& name, bool alias = false );
	//  bool SetDescFormula(const std::string& name, const std::string& description);

	CFormula*  GetFormula( int32_t type );

	void InitFormulaDataMode( int32_t dataMode );

	std::string GetCommentFormula( const std::string& name );
	bool SetCommentFormula( const std::string& name, const std::string& comment );

	bool ValidName( const char* name );
	bool ValidName( const std::string& name );

	void NamesToArrayString( wxArrayString& array );
	void NamesToComboBox( wxComboBox& combo );

	int32_t CountDataFields();

	bool HasFilters();

	void Amend( const CStringArray& keys, CProduct* product, const std::string& record );

	bool InsertPredefined();
	bool InsertUserDefined( const wxFileName& fileName );
	bool InsertUserDefined( CConfiguration *config );
	bool InsertUserDefined( CFormula* formula );
	bool InsertUserDefined( CFormula& formula );

	bool LoadConfig( bool predefined );
	bool LoadConfig( CConfiguration *config, bool predefined, const std::string& pathSuff = "" );

	bool SaveConfig( CConfiguration *config, bool predefined, const std::string& pathSuff = "" );
};


/** @} */


#endif 
