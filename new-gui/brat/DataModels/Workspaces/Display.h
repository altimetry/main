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
#if !defined(WORKSPACES_DISPLAY_H)
#define WORKSPACES_DISPLAY_H

#include "libbrathl/brathl.h"

#include "libbrathl/BratObject.h"
#include "libbrathl/Field.h"
#include "libbrathl/LatLonRect.h"
#include "libbrathl/InternalFiles.h"

#include "Operation.h"
#include "DataModels/MapTypeDisp.h"
#include "DataModels/PlotData/BratLookupTable.h"
#include "DataModels/PlotData/ColorPalleteNames.h"
#include "DataModels/PlotData/FieldData.h"


class COperation;
class CFormula;
class CWorkspaceDisplay;


using namespace brathl;



//-------------------------------------------------------------
//------------------- CDisplayData class --------------------
//-------------------------------------------------------------

class CDisplayData : public CFieldData
{
	////////
	// types
	////////

	using base_t = CFieldData;

	friend class CWorkspaceSettings;

public:

	enum EImageExportType
	{
		eBmp,
		eJpg,
		ePng,
		ePnm,
		eTif,
		ePS,

		EImageExportType_size
	};


	/////////
	//statics
	/////////

	//... export image types

	static const std::string* ImageTypeStrings()
	{
		static const std::string names[ EImageExportType_size ] =
		{
			"bmp",
			"jpg",
			"png",
			"pnm",
			"tif",
			"ps"
		};
		static const DEFINE_ARRAY_SIZE( names );

		assert__( names_size == EImageExportType_size );

		return names;
	}


	static std::string ImageType2String( EImageExportType type )
	{
		static const std::string *names = ImageTypeStrings();

		assert__( type >= 0 && type < EImageExportType_size );

		return names[ type ];
	}


    // returns EImageExportType_size on failure
    //
    static EImageExportType String2ImageType( const std::string &str_type )
    {
        static const std::string *names = ImageTypeStrings();

        size_t i = 0;
        for ( ; i < EImageExportType_size; ++i )
            if ( names[i] == str_type )
                break;

        return (EImageExportType) i;
    }


public:
	static std::string GetValueAsText( double value );

	static const char* FMT_FLOAT_XY;

#if defined(BRAT_V3)
	static std::string MakeKey( const COperation *operation, const std::string &field_name, CMapTypeDisp::ETypeDisp type );
#else
	static std::string MakeKeyV3( const COperation *operation, const std::string &field_name, CMapTypeDisp::ETypeDisp type );

	static std::string MakeKey( const std::string &field_name, CMapTypeDisp::ETypeDisp type );
#endif

    static const unsigned smDefaultNumberOfBins;


	///////////////
	//instance data
	///////////////

private:
	//...type & key & fields

	CMapTypeDisp::ETypeDisp m_type = CMapTypeDisp::Invalid();

	CObArray m_dimFields;

	CFieldBasic m_field;
	CFieldBasic m_x;
	CFieldBasic m_y;
	CFieldBasic m_z;

	//0...group ( TBC )
	int m_group = 1;

	//1...operation
	const COperation* m_operation = nullptr;

	//2...vector
	bool mEastComponent = false;
	bool mNorthComponent = false;

	//3...contour
	bool m_withContour = false;

	//4...solid color
	bool m_withSolidColor = true;

	//5...color table && color table range
	CBratLookupTable mLUT;
	double mAbsoluteMinValue = defaultValue<double>();
	double mAbsoluteMaxValue = defaultValue<double>();
	double mCurrentMinValue = defaultValue<double>();
	double mCurrentMaxValue = defaultValue<double>();
	std::string m_colorPalette = PALETTE_AEROSOL;

	//6...axis
	std::string m_xAxis;
	bool m_invertXYAxes = false;

	//7...histogram

	unsigned mNumberOfBins = smDefaultNumberOfBins;


	/////////////////////////////
	// construction / destruction
	/////////////////////////////

private:
	void Init()
	{
		m_dimFields.SetDelete( false );
		m_dimFields.Insert( GetX() );
		m_dimFields.Insert( GetY() );
		m_dimFields.Insert( GetZ() );

		mLUT.ExecMethodDefaultColorTable();
	}

	CDisplayData( const CDisplayData &o ) = delete;
	CDisplayData& operator = ( const CDisplayData &o) = delete;

	CDisplayData()	//for persistence
	{
		Init();
	}

public:
	
	CDisplayData( const COperation* operation, CMapTypeDisp::ETypeDisp type )
		: base_t()
		, m_operation( operation )
		, m_type( type )
	{
		Init();
	}
	
	CDisplayData( const CDisplayData &o, const CWorkspaceOperation *wkso );

	~CDisplayData()
	{}


	void CopyFieldUserProperties( CDisplayData &d );		//so far, v3 only


	/////////////////////////////
	//	identity / fields
	/////////////////////////////

	//...type & key

	bool IsYFXType();
	bool IsZYFXType();
	bool IsZLatLonType();

	std::string GetDataKey();
	std::string GetDataKey( CMapTypeDisp::ETypeDisp type );

	const std::string& FieldName() const { return m_field.GetName(); }

	virtual const std::string& Name() const override
	{ 
		return FieldName(); 
	}


	CMapTypeDisp::ETypeDisp GetType() const { return m_type; }

#if defined(BRAT_V3)
	void SetType( int32_t value ){ m_type = (CMapTypeDisp::ETypeDisp)value; }
#endif

	//...field(s)

	const CFieldBasic* GetField() const { return &m_field; }
	CFieldBasic* GetField() { return &m_field; }
	CFieldBasic* GetX() { return &m_x; }
	CFieldBasic* GetY() { return &m_y; }
	CFieldBasic* GetZ() { return &m_z; }




	/////////////////////////////
	//		properties
	/////////////////////////////

	//0...group ( TBC )
	std::string GetGroupAsText() const { return n2s< std::string >( m_group ); }
	int32_t GetGroup() { return m_group; }
	void SetGroup( int32_t value ) { m_group = value; }


	//1...operation

	const COperation* GetOperation() { return m_operation; }
	const COperation* GetOperation() const { return const_cast<CDisplayData*>( this )->GetOperation(); }


	//2...vector

	bool IsEastComponent() const { return mNorthComponent; }
	void SetEastComponent( bool value ) { mNorthComponent = value; }

	bool IsNorthComponent() const { return mEastComponent; }
	void SetNorthComponent( bool value ) { mEastComponent = value; }


	//3...contour

	std::string GetContourAsText() const { return ( m_withContour ? "Y" : "N" ); }
	bool IsContour() const { return GetContour(); }
	bool GetContour() const { return m_withContour; }
	void SetContour( bool value ) { m_withContour = value; }


	//4...solid color

	std::string GetSolidColorAsText() const { return ( m_withSolidColor ? "Y" : "N" ); }
	bool IsSolidColor() const { return GetSolidColor(); }
	bool GetSolidColor() const { return m_withSolidColor; }
	void SetSolidColor( bool value ) { m_withSolidColor = value; }


	//5...color table && color table range

	std::string GetAbsoluteMinValueAsText() const { return GetValueAsText( mAbsoluteMinValue ); }
	std::string GetAbsoluteMaxValueAsText() const { return GetValueAsText( mAbsoluteMaxValue ); }
	double GetAbsoluteMinValue() const { return mAbsoluteMinValue; }
	double GetAbsoluteMaxValue() const { return mAbsoluteMaxValue; }
	void SetAbsoluteRangeValues( double m, double M ) 
	{ 
		if ( m > M )
			return;

		mAbsoluteMinValue = m; 
		mAbsoluteMaxValue = M; 

		if ( isDefaultValue( mCurrentMinValue ) )
			mCurrentMinValue = m;
		else
			SetCurrentMinValue( mCurrentMinValue );

		if ( isDefaultValue( mCurrentMaxValue ) )
			mCurrentMaxValue = M;
		else
			SetCurrentMaxValue( mCurrentMaxValue );
	}


	std::string GetCurrentMinValueAsText() const { return GetValueAsText( mCurrentMinValue ); }
	double GetCurrentMinValue() const { return mCurrentMinValue; }
	void SetCurrentMinValue( double value ) 
	{ 
		mCurrentMinValue = std::min( std::max( value, mAbsoluteMinValue ), mAbsoluteMaxValue ); 
	}

	std::string GetCurrentMaxValueAsText() const { return GetValueAsText( mCurrentMaxValue ); }
	double GetCurrentMaxValue() const { return mCurrentMaxValue; }
	void SetCurrentMaxValue( double value ) 
	{ 
		mCurrentMaxValue = std::max( std::min( value, mAbsoluteMaxValue ), mAbsoluteMinValue ); 
	}

	const std::string& GetColorPalette() const { return m_colorPalette; }
	void SetColorPalette( const std::string& value ) { m_colorPalette = value; }


	//6...axis

	std::string GetXAxis() const { return m_xAxis; }
	void SetXAxis( const std::string& value ) { m_xAxis = value; }

	std::string GetXAxisText( const std::string& name );
	std::string GetXAxisText( unsigned index );
	void SetXAxisText( unsigned index, const std::string& value );

	bool HasXComplement();
	void GetXComplement( CStringArray& complement );

	void GetAvailableAxes( CStringArray& names );

	bool IsInvertXYAxes() const { return m_invertXYAxes; }
	void SetInvertXYAxes( bool value ) { m_invertXYAxes = value; }


	//7...histogram

	unsigned GetNumberOfBins() const { return mNumberOfBins; }
	void SetNumberOfBins( unsigned bins ) { mNumberOfBins = bins; }
};




//-------------------------------------------------------------
//------------------- CMapDisplayData class --------------------
//-------------------------------------------------------------

class CDisplay;


class CMapDisplayData : public CObMap
{
	friend class CConfiguration;


	CMapDisplayData( const CMapDisplayData &o ) = delete;
	CMapDisplayData& operator = ( const CMapDisplayData &o ) = delete;

public:
	/// CMapDisplayData ctor
	CMapDisplayData()
	{}

	CMapDisplayData( const CMapDisplayData &o, const CWorkspaceOperation *wkso );

	/// CMapDisplayData dtor
	virtual ~CMapDisplayData()
	{}

	const CDisplayData* GetDisplayData( const std::string& name ) const	{ return GetDisplayData( name.c_str() ); }
	const CDisplayData* GetDisplayData( const char* name ) const;

	bool AreFieldsGrouped() const;

	void SetGroups( bool groupFields );
	void GroupFields();
	void SplitFields();

	bool ValidName( const char* name );
	bool ValidName( const std::string& name );

	//void NamesToArrayString( wxArrayString& array );

	//void GetFiles( wxArrayString& array );

	//void GetDistinctFiles( wxArrayString& array );
	void GetDistinctFiles( CStringMap& array );

	//void GetFields( wxArrayString& array );

	//void GetDistinctFields( wxArrayString& array );
	void GetDistinctFields( CStringMap& array );

	bool LoadConfig( CWorkspaceSettings *config, std::string &errorMsg, CWorkspaceDisplay *wks, CWorkspaceOperation *wkso, const std::string& pathSuff = "" );
	bool SaveConfig( CWorkspaceSettings *config, CWorkspaceDisplay *wks, const std::string& pathSuff = "" ) const;

	bool CheckFields( std::string& errorMsg, CDisplay* display );

	void SetAllAxis( unsigned index, const std::string& axisName, const std::string& axisLabel );
	void SetAllInvertXYAxes( bool value );
};

//-------------------------------------------------------------
//------------------- CDisplay class --------------------
//-------------------------------------------------------------


class CDisplay : public CBratObject
{
	friend class CDisplayCmdFile;
	friend class CWorkspaceSettings;

	//v4: move to COperation
	//v3: static void GetDisplayType( const COperation* operation, CUIntArray& displayTypes, CInternalFiles** pf = nullptr );

protected:
	static const std::string m_zoomDelimiter;

public:
	const int32_t m_verbose = 2;

protected:
	CMapTypeDisp::ETypeDisp m_type;
	std::string m_cmdFile;

	CMapDisplayData m_data;
	std::string m_title;
	bool m_withAnimation;

	double m_minXValue;
	double m_maxXValue;

	double m_minYValue;
	double m_maxYValue;

	std::string m_projection;

	std::string m_name;

	CLatLonRect m_zoom;

	CDisplay( const CDisplay &o ) = delete;
	CDisplay& operator = ( const CDisplay &o ) = delete;

public:

	/// Empty CDisplay ctor
	CDisplay( const std::string name );

	CDisplay( const CDisplay &o, const CWorkspaceDisplay *wks, const CWorkspaceOperation *wkso ) 
		: m_data( o.m_data, wkso )
	{
		m_name = o.m_name;
		m_withAnimation = o.m_withAnimation;

		m_maxXValue = o.m_maxXValue;
		m_minXValue = o.m_minXValue;

		m_maxYValue = o.m_maxYValue;
		m_minYValue = o.m_minYValue;

		m_projection = o.m_projection;

		m_title = o.m_title;

		m_type = o.m_type;

		m_zoom = const_cast<CLatLonRect&>( o.m_zoom );

		InitOutput( wks );
	}

	//v4 new

	CDisplay* Clone( const std::string &name,  const CWorkspaceDisplay *wks, const CWorkspaceOperation *wkso ) const
	{
		CDisplay *d = new CDisplay( *this, wks, wkso );
		d->m_name = name;
		d->InitOutput( wks );
		return d;
	}

	//bool AssignOperation( const COperation *operation, bool update = false );
	void UpdateDisplayData( const CMapDisplayData *data_list, const CWorkspaceOperation *wkso );

	//

protected:

	void Init();

public:
	/// Destructor
	virtual ~CDisplay();

	const std::string& GetName() const { return m_name; }
	void SetName( const std::string& value ) { m_name = value; }

	bool HasData() const { return GetDataCount() > 0; }
	size_t GetDataCount() const { return m_data.size(); }

	bool SaveConfig( CWorkspaceSettings* config, CWorkspaceDisplay *wksd ) const;
	bool LoadConfig( CWorkspaceSettings* config, std::string &errorMsg, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso );

	bool UsesOperation( const std::string& name ) const;
	std::vector< const COperation* > GetOperations() const;


	CMapTypeDisp::ETypeDisp GetPlotType( bool map_as_3dplot ) const 
	{ 
		CMapTypeDisp::ETypeDisp type = m_type;
		if ( map_as_3dplot && type == CMapTypeDisp::eTypeDispZFLatLon )
			type = CMapTypeDisp::eTypeDispZFXY;

		return type; 
	}

	CMapTypeDisp::ETypeDisp GetType() const { return m_type; }
	void SetType( CMapTypeDisp::ETypeDisp value ) { m_type = value; }
	void SetType( int32_t value ) { SetType( (CMapTypeDisp::ETypeDisp)value ); }

	std::string GetTypeAsString( bool map_as_3dplot ) const 
	{ 
		return n2s< std::string >( GetPlotType( map_as_3dplot ) ); 
	}

	CMapDisplayData* GetData() { return &m_data; }
	const CMapDisplayData& GetData() const { return m_data; }
	std::string FmtCmdParam( const std::string& name );

	bool ExistData( const std::string& key );
	bool InsertData( const std::string& key, CDisplayData* data );
	bool RemoveData( const std::string& key );

	std::string GetTitle() const { return m_title; }
	void SetTitle( const std::string& value ) { m_title = value; }

	std::string GetProjection() const { return m_projection; }
	void SetProjection( const std::string& value ) { m_projection = value; }

	std::string GetWithAnimationAsText() const { return ( m_withAnimation ? "Y" : "N" ); }
	bool GetWithAnimation() const { return m_withAnimation; }
	void SetWithAnimation( bool value ) { m_withAnimation = value; }

	std::string GetMinXValueAsText() const { return CDisplayData::GetValueAsText( m_minXValue ); }
	double GetMinXValue() const { return m_minXValue; }
	void SetMinXValue( double value ) { m_minXValue = value; }

	std::string GetMaxXValueAsText() const { return CDisplayData::GetValueAsText( m_maxXValue ); }
	double GetMaxXValue() const { return m_maxXValue; }
	void SetMaxXValue( double value ) { m_maxXValue = value; }

	std::string GetMinYValueAsText() const { return CDisplayData::GetValueAsText( m_minYValue ); }
	double GetMinYValue() const { return m_minYValue; }
	void SetMinYValue( double value ) { m_minYValue = value; }

	std::string GetMaxYValueAsText() const { return CDisplayData::GetValueAsText( m_maxYValue ); }
	double GetMaxYValue() const { return m_maxYValue; }
	void SetMaxYValue( double value ) { m_maxYValue = value; }

#if defined(BRAT_V3)
	CDisplayData* GetDisplayData( const COperation *operation, const std::string &field_name )
	{
		std::string key = CDisplayData::MakeKey( operation, field_name, GetType() );
        return const_cast<CDisplayData*>( m_data.GetDisplayData( key ) );            //hack
	}
#else
    CDisplayData* GetFieldDisplayDataV3( const COperation *operation, const std::string &field_name );
    CDisplayData* GetFieldDisplayData( const std::string &field_name );
#endif


	std::string GetOutputFilename() const { return GetCmdFilename(); }
	std::string GetOutputName() const { return GetCmdFilePath(); }
	void SetOutput( const std::string& value ) { SetCmdFile( value ); }

	bool AreFieldsGrouped() const { return m_data.AreFieldsGrouped(); }

	void SetGroups( bool groupFields );

	std::string GetTaskName();
	std::string GetCmdFilename() const { return GetFileName( m_cmdFile ); }
	const std::string& GetCmdFilePath() const { return m_cmdFile; }
	void SetCmdFile( const std::string& value ) { m_cmdFile = value; clean_path( m_cmdFile ); }

	void SetAllAxis( unsigned index, const std::string& axisName, const std::string& axisLabel ) { m_data.SetAllAxis( index, axisName, axisLabel ); }
	void SetAllInvertXYAxes( bool value ) { m_data.SetAllInvertXYAxes( value ); }

	CLatLonRect* GetZoom() { return &m_zoom; }
	void SetZoom( CLatLonRect& value ) { m_zoom = value; }

	//std::string GetFullCmd();


	bool BuildCmdFile( std::string &error_msg, bool map_as_3dplot = false );

public:
	void InitOutput( const CWorkspaceDisplay *wks );

	bool IsYFXType() const;
	bool IsZYFXType() const;
	bool IsZLatLonType() const;

	///Dump fonction
	virtual void Dump( std::ostream& fOut = std::cerr );
};

/** @} */



#endif // !defined(WORKSPACES_DISPLAY_H)
