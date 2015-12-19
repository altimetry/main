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
#if !defined(WORKSPACES_FORMULA_H)
#define WORKSPACES_FORMULA_H

#include "new-gui/QtInterface.h"

#include "brathl.h"

#include "List.h"
#include "Unit.h"
#include "Date.h"


using namespace brathl;


//-------------------------------------------------------------
//------------------- CMapTypeFilter class --------------------
//-------------------------------------------------------------

class CMapTypeFilter : public CUIntMap
{
public:

	enum ETypeFilter
	{
		eFilterNone,
		eFilterLoessSmooth,
		eFilterLoessExtrapolate,
		eFilterLoess,

		ETypeFilter_size
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

	//void NamesToArrayString( wxArrayString& array )
	//{
	//	for ( CMapTypeFilter::iterator it = begin(); it != end(); it++ )
	//	{
	//		uint32_t value = it->second;
	//		if ( !isDefaultValue( value ) )
	//		{
	//			array.Add( ( it->first ).c_str() );
	//		}
	//	}
	//}
};

//-------------------------------------------------------------
//------------------- CMapTypeData class --------------------
//-------------------------------------------------------------

class CMapTypeData : public CUIntMap
{
public:
	enum ETypeData
	{
		eTypeOpData,
		eTypeOpLongitude,
		eTypeOpLatitude,
		eTypeOpX,
		eTypeOpY,
		eTypeOpZ,
		eTypeOpT
	};

public:
	/// CIntMap ctor
	CMapTypeData();

	/// CIntMap dtor
	virtual ~CMapTypeData();

	static CMapTypeData&  GetInstance();


	bool ValidName( const char* name );
	bool ValidName( const std::string& name );

	std::string IdToName( uint32_t id );
	uint32_t NameToId( const std::string& name );

	//void NamesToArrayString( wxArrayString& array, bool noData = false )
	//{
	//	CMapTypeData::iterator it;

	//	for ( it = begin(); it != end(); it++ )
	//	{
	//		uint32_t value = it->second;
	//		if ( ( value == eTypeOpData ) && ( noData ) )
	//		{
	//			continue;
	//		}
	//		if ( !isDefaultValue( value ) )
	//		{
	//			array.Add( ( it->first ).c_str() );
	//		}
	//	}
	//}

};
//-------------------------------------------------------------
//------------------- CMapTypeOp class --------------------
//-------------------------------------------------------------

class CMapTypeOp : public CUIntMap
{
public:

	enum ETypeOp
	{
		eTypeOpYFX,
		eTypeOpZFXY
	};

public:
	CMapTypeOp();

	virtual ~CMapTypeOp();

	static CMapTypeOp&  GetInstance();


	bool ValidName( const char* name );
	bool ValidName( const std::string& name );

	std::string IdToName( uint32_t id );
	uint32_t NameToId( const std::string& name );

	//void NamesToArrayString( wxArrayString& array )
	//{
	//	CMapTypeOp::iterator it;

	//	for ( it = begin(); it != end(); it++ )
	//	{
	//		uint32_t value = it->second;
	//		if ( !isDefaultValue( value ) )
	//		{
	//			array.Add( ( it->first ).c_str() );
	//		}
	//	}
	//}
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

class CMapTypeField : public CUIntMap
{
public:
	enum ETypeField
	{
		eTypeOpAsField,
		eTypeOpAsX,
		eTypeOpAsY,
		eTypeOpAsSelect
	};
public:
	CMapTypeField();

	virtual ~CMapTypeField();

	static CMapTypeField&  GetInstance();


	bool ValidName( const char* name );
	bool ValidName( const std::string& name );

	std::string IdToName( uint32_t id );
	uint32_t NameToId( const std::string& name );

	//void NamesToArrayString( wxArrayString& array )
	//{
	//	CMapTypeField::iterator it;

	//	for ( it = begin(); it != end(); it++ )
	//	{
	//		uint32_t value = it->second;
	//		if ( !isDefaultValue( value ) )
	//		{
	//			array.Add( ( it->first ).c_str() );
	//		}
	//	}
	//}
};

//-------------------------------------------------------------
//------------------- CMapDataMode class --------------------
//-------------------------------------------------------------

class CMapDataMode : public CUIntMap
{
public:
	CMapDataMode();

	virtual ~CMapDataMode();

	static CMapDataMode&  GetInstance();


	bool ValidName( const char* name );
	bool ValidName( const std::string& name );

	std::string IdToName( uint32_t id );
	uint32_t NameToId( const std::string& name );

	//void NamesToArrayString( wxArrayString& array )
	//{
	//	CMapDataMode::iterator it;

	//	for ( it = begin(); it != end(); it++ )
	//	{
	//		uint32_t value = it->second;
	//		if ( !isDefaultValue( value ) )
	//		{
	//			array.Add( ( it->first ).c_str() );
	//		}
	//	}
	//}

	uint32_t GetDefault();
	std::string GetDefaultAsString();
};




//-------------------------------------------------------------
//------------------- CFormula class --------------------
//-------------------------------------------------------------



class CWorkspaceFormula;



class CFormula : public CBratObject
{
protected:

	std::string m_name;
	std::string m_title;
	std::string m_description;
	std::string m_comment;
	int32_t m_type = CMapTypeField::eTypeOpAsField;

	CUnit m_unit;

	int32_t m_dataType = -1;

	int m_filter = CMapTypeFilter::eFilterNone;

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

	std::string GetDescription( bool removeCRLF = false, const CStringMap* formulaAliases = NULL, const CStringMap* fieldAliases = NULL ) const;
	void SetDescription( const std::string& value ) { m_description = value; };
	void SetDescription( const char* value ) { m_description = value; };

	std::string GetAlias();

	std::string GetComment( bool removeCRLF = false );
	void SetComment( const std::string& value ) { m_comment = value; };

	bool IsPredefined() const { return m_predefined; };
	void SetPredefined( bool value ) { m_predefined = value; };

	bool LoadConfig( CConfiguration *config );
	bool LoadConfig( CConfiguration *config, std::string &errorMsg, const std::string& pathSuff );
	bool LoadConfigDesc( CConfiguration *config, const std::string& path );

	bool SaveConfig( CConfiguration *config );
	bool SaveConfig( CConfiguration *config, const std::string& pathSuff = "" );

	bool SaveConfigDesc( CConfiguration *config, const std::string& path );

	bool SaveConfigPredefined( CConfiguration *config, const std::string& pathSuff );

	std::string GetFilterAsString() { return CMapTypeFilter::GetInstance().IdToName( m_filter ); };
	int32_t GetFilter() { return m_filter; };
	void SetFilter( int32_t value ) { m_filter = value; };

	void SetFilterDefault() { SetFilter( CMapTypeFilter::eFilterNone ); };

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

	static double GetStepAsDouble( const std::string& step, std::string errorMsg );
	double GetStepAsDouble( std::string &errorMsg );
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
	int32_t GetType() const { return m_type; };
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
	void SetUnit( const std::string& value, std::string &errorMsg, const std::string& defaultValue, bool convertMinMax = false );
	void SetUnit( const CUnit& value, std::string &errorMsg, const std::string& defaultValue, bool convertMinMax = false );

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

	static bool CheckExpression( CWorkspaceFormula *wks, const std::string& value, const std::string& record, std::string& errorMsg, std::string* strUnitExpr = NULL,
		const CStringMap* aliases = NULL, CProduct* product = NULL, std::string* valueOut = NULL );
	bool CheckExpression( CWorkspaceFormula *wks, std::string& errorMsg, const std::string& record,
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


	void ConvertToMinMaxFormulaUnit( std::string &errorMsg );
	void ConvertToMinMaxFormulaBaseUnit( std::string &errorMsg );

	double ConvertToFormulaBaseUnit( double in );
	bool ConvertToFormulaBaseUnit( double in, double& out );

	double ConvertToFormulaUnit( double in );
	bool ConvertToFormulaUnit( double in, double& out );

	bool ComputeInterval( std::string &errorMsg );
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
	CMapFormula( bool withPredefined )
	{}

	CMapFormula( std::string &errorMsg, bool withPredefined )
	{
		if ( withPredefined )
			InsertPredefined( errorMsg );
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

	std::string GetCommentFormula( const std::string& name ) const;
	bool SetCommentFormula( const std::string& name, const std::string& comment );

	bool ValidName( const char* name );
	bool ValidName( const std::string& name );

	//void NamesToArrayString( wxArrayString& array )
	//{
	//	CMapFormula::iterator it;

	//	for ( it = begin(); it != end(); it++ )
	//	{
	//		CFormula* value = dynamic_cast<CFormula*>( it->second );
	//		if ( value != nullptr )
	//		{
	//			array.Add( value->GetName() );
	//		}
	//	}
	//}

	int32_t CountDataFields();

	bool HasFilters();

	void Amend( const CStringArray& keys, CProduct* product, const std::string& record );

	bool InsertPredefined( std::string &errorMsg );
	//bool InsertUserDefined( const wxFileName& fileName );
	bool InsertUserDefined( CConfiguration *config, std::string &errorMsg );
	bool InsertUserDefined( CFormula* formula, std::string &errorMsg );
	bool InsertUserDefined_ReplacePredefinedNotAllowed( CFormula& formula, std::string &errorMsg );

	bool LoadConfig( std::string &errorMsg, bool predefined );
	bool LoadConfig( CConfiguration *config, std::string &errorMsg, bool predefined, const std::string& pathSuff = "" );

	bool SaveConfig( CConfiguration *config, bool predefined, const std::string& pathSuff = "" );
};


/** @} */


#endif			//	WORKSPACES_FORMULA_H
