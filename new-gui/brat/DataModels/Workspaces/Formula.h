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

#include "libbrathl/brathl.h"

#include "libbrathl/List.h"
#include "libbrathl/Unit.h"
#include "libbrathl/Date.h"

#include "ProductInfo.h"

namespace brathl
{ 
	class CProduct;
	class CExpression;
}


using namespace brathl;


class CWorkspaceSettings;


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

	std::string IdToName( ETypeData id );
	ETypeData NameToId( const std::string& name );
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
		eTypeOpZFXY,

		ETypeOp_size
	};

public:
	CMapTypeOp();

	virtual ~CMapTypeOp();

	static CMapTypeOp&  GetInstance();


	bool ValidName( const char* name );
	bool ValidName( const std::string& name );

	std::string IdToName( uint32_t id );
	CMapTypeOp::ETypeOp NameToId( const std::string& name );
};

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
		eTypeOpAsSelect,

		ETypeField_size
	};

public:
	CMapTypeField();

	virtual ~CMapTypeField();

	static CMapTypeField&  GetInstance();


	bool ValidName( const char* name );
	bool ValidName( const std::string& name );

	std::string IdToName( uint32_t id );
	ETypeField NameToId( const std::string& name );

	int GetMaxAllowed( ETypeField type )
	{
		int max = 0;
		switch ( type )
		{
			case CMapTypeField::eTypeOpAsX:
				max = 1;
				break;
			case CMapTypeField::eTypeOpAsY:
				max = 1;
				break;
			case CMapTypeField::eTypeOpAsField:
				max = -1;
				break;
			case CMapTypeField::eTypeOpAsSelect:
				max = 1;
				break;

            default:
                assert__( false );
		}

		return max;
	}
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

	std::string IdToName( uint32_t id );
	uint32_t NameToId( const std::string& name );

	uint32_t GetDefault();
	std::string GetDefaultAsString();
};




//-------------------------------------------------------------
//------------------- CFormula class --------------------
//-------------------------------------------------------------



class CWorkspaceFormula;



class CFormula : public CBratObject
{
	friend class CWorkspaceSettings;
	
	static const std::string DEFAULT_STEP_LATLON_ASSTRING;
	static const std::string DEFAULT_STEP_TIME_ASSTRING; 
	static const std::string DEFAULT_STEP_GENERAL_ASSTRING;

protected:

	std::string m_name;
	std::string m_title;
	std::string m_description;
	std::string m_comment;
	CMapTypeField::ETypeField m_type = CMapTypeField::eTypeOpAsField;

	CUnit m_unit;

	CMapTypeData::ETypeData m_dataType = (CMapTypeData::ETypeData)-1;

	int m_filter = CMapTypeFilter::eFilterNone;

	double m_minValue = defaultValue< double >();
	double m_maxValue = defaultValue< double >();
	int32_t m_interval = defaultValue< int32_t >();
	std::string m_step;
	int32_t m_loessCutOff = defaultValue< int32_t >();

	bool m_predefined = false;

	int32_t m_dataMode = CMapDataMode::GetInstance().GetDefault();
	std::string mDataModeDITimeName;
	CDate mDataModeDIDateTime;
	double mDataModeDIDistanceWeightingParameter = 0.;
	double mDataModeDITimeWeightingParameter = 0.;


    bool mNorthComponent = false;
    bool mEastComponent = false;


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
            mDataModeDITimeName = o.mDataModeDITimeName;
            mDataModeDIDateTime = o.mDataModeDIDateTime;
            mDataModeDIDistanceWeightingParameter = o.mDataModeDIDistanceWeightingParameter;
            mDataModeDITimeWeightingParameter = o.mDataModeDITimeWeightingParameter;
            
            mNorthComponent = o.mNorthComponent;
            mEastComponent = o.mEastComponent;
        }
		return *this;
	}

	/// Destructor
	virtual ~CFormula()
	{}

	const std::string& GetName() const { return m_name; }
	void SetName( const std::string& value ) { m_name = value; }

	std::string GetDescription( bool removeCRLF = false, const CStringMap* formulaAliases = nullptr, const CStringMap* fieldAliases = nullptr ) const;
	void SetDescription( const std::string& value ) { m_description = rtrim( ltrim( value ) ); }
	void SetDescription( const char* value ) { m_description = rtrim( ltrim( std::string( value ) ) ); }

	std::string GetAlias();

	std::string GetComment( bool removeCRLF = false ) const ;
	void SetComment( const std::string& value ) { m_comment = value; }

	bool IsPredefined() const { return m_predefined; }
	void SetPredefined( bool value ) { m_predefined = value; }

    bool IsNorthComponent() const { return mNorthComponent; }
    bool IsEastComponent() const { return mEastComponent; }
    void SetNorthComponent( bool north ) { mNorthComponent = north; }
    void SetEastComponent( bool east ) { mEastComponent = east; }


	//bool LoadConfig( CWorkspaceSettings *config );
	bool LoadConfig( CWorkspaceSettings *config, std::string &errorMsg, const std::string& pathSuff );
	bool LoadConfigDesc( CWorkspaceSettings *config, const std::string& path );

	//bool SaveConfig( CWorkspaceSettings *config );
	bool SaveConfig( CWorkspaceSettings *config, const std::string& pathSuff = "" ) const;
	bool SaveConfigDesc( CWorkspaceSettings *config, const std::string& path );
	bool SaveConfigPredefined( CWorkspaceSettings *config, const std::string& pathSuff ) const;

	std::string GetFilterAsString() const { return CMapTypeFilter::GetInstance().IdToName( m_filter ); }
	int32_t GetFilter() { return m_filter; }
	void SetFilter( int32_t value ) { m_filter = value; }

	void SetFilterDefault() { SetFilter( CMapTypeFilter::eFilterNone ); }

	std::string GetMinValueAsText();
	std::string GetMinAsString();
	double GetMinValue() { return m_minValue; }
	void SetMinValue( double value ) { m_minValue = value; }
	void SetMinValue( CDate& d );
	void SetMinValueFromDateString( const std::string& value );

	void SetMinValueDefault() { setDefaultValue( m_minValue ); }

	std::string GetMinValueAsDateString() const { return GetAsDateString( m_minValue); }
	std::string GetMaxValueAsDateString() const {  return GetAsDateString( m_maxValue); }

	std::string GetValueAsString( double value );
	std::string GetFormatString();

	std::string GetMaxValueAsText();
	std::string GetMaxAsString();
	double GetMaxValue() { return m_maxValue; }
	void SetMaxValue( double value ) { m_maxValue = value; }
	void SetMaxValue( CDate& d );
	void SetMaxValueFromDateString( const std::string& value );

	void SetMaxValueDefault() { setDefaultValue( m_maxValue ); }

	static double GetStepAsDouble( const std::string& step, std::string errorMsg );
	double GetStepAsDouble( std::string &errorMsg );
	std::string GetStep() { return m_step; }
	void SetStep( const std::string& value ) { m_step = value; }

	std::string GetIntervalAsText() const;
	int32_t GetInterval() { return m_interval; }
	void SetInterval( int32_t value ) { m_interval = value; }

	void SetIntervalDefault() { setDefaultValue( m_interval ); }

	std::string GetLoessCutOffAsText() const;
	int32_t GetLoessCutOff() { return m_loessCutOff; }
	void SetLoessCutOff( int32_t value ) { m_loessCutOff = value; }

	void SetLoessCutOffDefault() { setDefaultValue( m_loessCutOff ); }

	std::string GetTypeAsString() const { return CMapTypeField::GetInstance().IdToName( m_type ); }
	CMapTypeField::ETypeField GetFieldType() const { return m_type; }	
	void SetFieldType( CMapTypeField::ETypeField type ) { m_type = type; }
	void SetFieldType( int32_t value )
	{ 
		SetFieldType( (CMapTypeField::ETypeField)value ); 
	}

	std::string GetDataTypeAsString() const { return CMapTypeData::GetInstance().IdToName( m_dataType ); }
	CMapTypeData::ETypeData GetDataType() const { return m_dataType; }
	void SetDataType( CMapTypeData::ETypeData value ) { m_dataType = value; }
	void SetDataType( CMapTypeField::ETypeField typeField, const CUnit& unit, const CProductInfo &pi );


	void SetDefaultMinMax();

	std::string GetFilterAsLabel();
	std::string GetResolutionAsLabel( bool hasFilter );

	std::string GetTitle() { return m_title; }
	void SetTitle( const std::string& value ) { m_title = value; }

	const CUnit* GetUnit() const { return &m_unit; }
	std::string GetUnitAsText() const { return m_unit.GetText(); }
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

	static bool SetExpression( const std::string& value, CExpression& expr, std::string& errorMsg );
	static bool SetExpression( const char* value, CExpression& expr, std::string& errorMsg );

	static bool CheckExpression( CWorkspaceFormula *wks, const std::string& value, const std::string& record, std::string& errorMsg, std::string* strUnitExpr = nullptr,
		const CStringMap* aliases = nullptr, const CProductInfo &pi = CProductInfo::smInvalidProduct, std::string* valueOut = nullptr );

	bool CheckExpression( CWorkspaceFormula *wks, std::string& errorMsg, const std::string& record,
		const CStringMap* aliases = nullptr, const CProductInfo &pi = CProductInfo::smInvalidProduct, std::string* valueOut = nullptr );

protected:
	static bool CheckFieldNames( const CExpression& expr, const std::string& record, const CProductInfo &pi, CStringArray& fieldNamesNotFound );
	static bool CheckFieldNames( const CExpression& expr, const std::string& record, const CProductInfo &pi, CExpression& exprOut, std::string& errorMsg );
	static bool CheckFieldNames( const std::string& expr, const std::string& record, const CProductInfo &pi, std::string& exprOut, std::string& errorMsg );

	static bool CheckExpressionUnits( const std::string& exprStr, const std::string& record, const std::string& strUnitExpr, const CProductInfo &pi, std::string& errorMsg );

protected:

public:
	bool GetFields( CStringArray& fields, std::string& errorMsg, const CStringMap* aliases = nullptr, const CStringMap* fieldAliases = nullptr ) const;

	double LonNormal360( double lon ) const;

	bool ControlResolution( std::string& errorMsg ) const;
	bool CtrlMinMaxValue( std::string& errorMsg ) const;

	bool IsFieldType() const;
	bool IsXType() const;
	bool IsYType() const;
	bool IsXYType() const;

	bool IsDataTypeSet() const;
	bool IsLonDataType() const;
	bool IsLatDataType() const;
	bool IsLatLonDataType() const;

	bool IsXYLatLon() const;
	bool IsTimeDataType() const;
	bool IsXYTime() const;

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

	std::string GetDataModeAsString() const { return CMapDataMode::GetInstance().IdToName( m_dataMode ); }
	int32_t GetDataMode() const { return m_dataMode; }
	void SetDataMode( int32_t value ) { m_dataMode = value; }

	const std::string& DataModeDITimeName() const { return mDataModeDITimeName; }
	void SetDataModeDITimeName( const std::string &name ) { mDataModeDITimeName = name; }

	const CDate& DataModeDIDateTime() const { return mDataModeDIDateTime; }
	void SetDataModeDIDateTime( const CDate &date_time ) { mDataModeDIDateTime = date_time; }

	double DataModeDIDistanceWeightingParameter() const { return mDataModeDIDistanceWeightingParameter; }
	void SetDataModeDIDistanceWeightingParameter( double v ) 
	{ 
		assert__( v >= 0. );

		mDataModeDIDistanceWeightingParameter = v; 
	}

	double DataModeDITimeWeightingParameter() const { return mDataModeDITimeWeightingParameter; }
	void SetDataModeDITimeWeightingParameter( double v ) 
	{ 
		assert__( v >= 0. );

		mDataModeDITimeWeightingParameter = v; 
	}


	///Dump fonction
	virtual void Dump( std::ostream& fOut = std::cerr );
};


//-------------------------------------------------------------
//------------------- CMapFormula class --------------------
//-------------------------------------------------------------

class CMapFormula : public CObMap
{
protected:
	CWorkspaceSettings *m_config = nullptr;

public:
	static std::string m_predefFormulaFile;

public:
	CMapFormula()
    {}

	CMapFormula( const std::string &internal_data_path, std::string &errorMsg, bool withPredefined )
	{
		if ( withPredefined )
			InsertPredefined( internal_data_path, errorMsg );
	}

	CMapFormula( const CMapFormula &o )
	{
		*this = o;
	}

	CMapFormula& operator = ( const CMapFormula &o )
	{
		if ( this != &o )
		{
			RemoveAll();
			for ( CMapFormula::const_iterator it = o.begin(); it != o.end(); it++ )
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
	virtual ~CMapFormula();


	//static CMapFormula&  GetInstance();

	std::string GetDescFormula( const std::string& name, bool alias = false );
	//  bool SetDescFormula(const std::string& name, const std::string& description);

	CFormula* GetFormula( int32_t type );
	const CFormula* GetFormula( int32_t type ) const
	{
		return const_cast<CMapFormula*>( this )->GetFormula( type );
	}

	std::string GetCommentFormula( const std::string& name ) const;
	bool SetCommentFormula( const std::string& name, const std::string& comment );

	bool ValidName( const char* name );
	bool ValidName( const std::string& name );

	size_t CountDataFields();

	bool HasFilters() const;

	void Amend( const CStringArray& keys, const CProductInfo &pi, const std::string& record );

	bool InsertPredefined( const std::string &internal_data_path, std::string &errorMsg );
	//bool InsertUserDefined( const wxFileName& fileName );
	bool InsertUserDefined( CWorkspaceSettings *config, std::string &errorMsg );
	bool InsertUserDefined( CFormula* formula, std::string &errorMsg );
	bool InsertUserDefined_ReplacePredefinedNotAllowed( CFormula& formula, std::string &errorMsg );

	bool LoadConfig( std::string &errorMsg, bool predefined );
	bool LoadConfig( CWorkspaceSettings *config, std::string &errorMsg, bool predefined, const std::string& pathSuff = "" );
	bool SaveConfig( CWorkspaceSettings *config, bool predefined, const std::string& pathSuff = "" ) const;
};


/** @} */


#endif			//	WORKSPACES_FORMULA_H
