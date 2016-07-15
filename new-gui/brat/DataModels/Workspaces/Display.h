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
#include "DataModels/PlotData/ColorPalleteNames.h"
#include "DataModels/PlotData/FieldData.h"


class COperation;
class CFormula;
class CWorkspaceDisplay;


using namespace brathl;



class CDisplay;


//-------------------------------------------------------------
//------------------- CDisplayData class --------------------
//-------------------------------------------------------------

class CDisplayData : public CFieldData
{
	/////////////////////////////
	//		types
	/////////////////////////////

	using base_t = CFieldData;

	friend class CWorkspaceSettings;
	friend class CDisplayCmdFile;

	enum EAxisIndex
	{
		eX,
		eY,
		eZ,

		EAxisIndex_size
	};

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


	/////////////////////////////
	//		statics
	/////////////////////////////

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

	void CopyFieldUserProperties( CDisplayData &d );		//so far, v3 only
#else
	static std::string MakeKeyV3( const COperation *operation, const std::string &field_name, CMapTypeDisp::ETypeDisp type );

	static std::string MakeKey( const std::string &field_name, CMapTypeDisp::ETypeDisp type );

#endif


	/////////////////////////////
	//		instance data
	/////////////////////////////

private:
	//...type & key & fields

	CMapTypeDisp::ETypeDisp m_type = CMapTypeDisp::Invalid();

	std::vector< CFieldBasic > mDimFields;
	CFieldBasic m_field;

	//...group ( TBC )
	int m_group = 1;

	//...operation & display
	const COperation *mOperation = nullptr;
	const CDisplay *mDisplay = nullptr;

#if defined(BRAT_V3)
	//...axis
	std::string m_xAxis;
	bool m_invertXYAxes = false;
#endif


	/////////////////////////////
	// construction / destruction
	/////////////////////////////

private:

	CDisplayData( const CDisplayData &o ) = delete;

public:

#if defined(BRAT_V3)
	CDisplayData()
		: base_t()
		, mDimFields( EAxisIndex_size )
	{}
#endif

	CDisplayData( const COperation* operation, const CDisplay *display, CMapTypeDisp::ETypeDisp type )
		: base_t()
		, mDimFields( EAxisIndex_size )
		, mOperation( operation )
		, mDisplay( display )
		, m_type( type )
	{
#if !defined(BRAT_V3)
		assert__( operation && display && type != CMapTypeDisp::Invalid() );
#endif
	}


	CDisplayData( const CDisplayData &o, const CWorkspaceOperation *wkso, const CWorkspaceDisplay *wksd );

	//CDisplayData& operator = ( const CDisplayData &o) = delete;
	CDisplayData& operator = ( const CDisplayData &o )
	{
		static_cast< base_t& >( *this ) = o;
		return *this;
	}

	~CDisplayData()
	{}



	/////////////////////////////
	//	identity / field
	/////////////////////////////

	//... key

	std::string GetDataKey();
	std::string GetDataKey( CMapTypeDisp::ETypeDisp type );

public:

	//...data field

	const CFieldBasic* GetField() const { return &m_field; }


	virtual void SetFieldName( const std::string &name ) override
	{ 
		m_field.SetName( name ); 
		base_t::SetFieldName( name );
	}

	void SetFieldDescription( const std::string &desc ) 
	{ 
		m_field.SetDescription( desc );
		if ( !desc.empty() )
			base_t::SetUserName( desc );
	}

	virtual void SetUserName( const std::string &name ) override { SetFieldDescription( name );	}

	void SetFieldUnit( const std::string &unit ) { m_field.SetUnit( unit ); }


	//...type

	bool IsYFXType();
	bool IsZYFXType();
	bool IsZLatLonType();

	CMapTypeDisp::ETypeDisp GetType() const { return m_type; }

#if defined(BRAT_V3)
	void SetType( int32_t value ){ m_type = (CMapTypeDisp::ETypeDisp)value; }
#endif


	/////////////////////////////
	//	operation & display
	/////////////////////////////

	const CDisplay* Display() const { return mDisplay; }
	const COperation* Operation() { return mOperation; }
	const COperation* Operation() const { return const_cast<CDisplayData*>( this )->Operation(); }


	/////////////////////////////
	//		axis field(s)
	/////////////////////////////

protected:
	CFieldBasic* GetX() { return &mDimFields[ eX ]; }
	CFieldBasic* GetY() { return &mDimFields[ eY ]; }
	CFieldBasic* GetZ() { return &mDimFields[ eZ ]; }

public:
	const CFieldBasic* GetX() const { return const_cast<CDisplayData*>( this )->GetX(); }
	const CFieldBasic* GetY() const { return const_cast<CDisplayData*>( this )->GetY(); }
	const CFieldBasic* GetZ() const { return const_cast<CDisplayData*>( this )->GetZ(); }


	void SetDimension( size_t index, const std::string &name, const std::string &description, const std::string &unit );

	const CField* FindDimension( const std::string &name ) const;


	/////////////////////////////
	//		properties
	/////////////////////////////

	//...group ( TBC )

	std::string GetGroupAsText() const { return n2s< std::string >( m_group ); }
	int GetGroup() const { return m_group; }
	void SetGroup( int value ) { m_group = value; }

	//...color table && color table range

	std::string AbsoluteMinValueAsText() const { return GetValueAsText( AbsoluteMinValue() ); }
	std::string AbsoluteMaxValueAsText() const { return GetValueAsText( AbsoluteMaxValue() ); }
	std::string CurrentMinValueAsText() const { return GetValueAsText( CurrentMinValue() ); }
	std::string CurrentMaxValueAsText() const { return GetValueAsText( CurrentMaxValue() ); }

	std::string GetSolidColorAsText() const { return ( WithSolidColor() ? "Y" : "N" ); }

#if defined(BRAT_V3)

	//...axis

	std::string GetXAxis() const { return m_xAxis; }
	void SetXAxis( const std::string& value ) { m_xAxis = value; }

public:
	const std::string& GetXAxisText( unsigned index ) const;
	void SetXAxisText( unsigned index, const std::string& value );

	bool HasXComplement();
	void GetXComplement( CStringArray& complement );

	void GetAvailableAxes( CStringArray& names ) const;

	bool IsInvertXYAxes() const { return m_invertXYAxes; }
	void SetInvertXYAxes( bool value ) { m_invertXYAxes = value; }

#endif

};



//-------------------------------------------------------------
//------------------- CMapDisplayData class --------------------
//-------------------------------------------------------------

class CMapDisplayData : public CObMap
{
	friend class CConfiguration;

	CMapDisplayData( const CMapDisplayData &o ) = delete;
	CMapDisplayData& operator = ( const CMapDisplayData &o ) = delete;

public:
	CMapDisplayData()
	{}

	virtual ~CMapDisplayData()
	{}
};





//-------------------------------------------------------------
//------------------- CDisplay class --------------------
//-------------------------------------------------------------


class CDisplay : public CBratObject
{
	//types

	using base_t = CBratObject;

	friend class CDisplayCmdFile;
	friend class CWorkspaceSettings;

	//statics

	//v4: move to COperation
	//v3: static void GetDisplayType( const COperation* operation, CUIntArray& displayTypes, CInternalFiles** pf = nullptr );

protected:
	static const std::string m_zoomDelimiter;

	//instance data

public:
	const int32_t m_verbose = 2;

protected:

	CMapTypeDisp::ETypeDisp m_type = CMapTypeDisp::Invalid();
	std::string m_cmdFile;

	std::string m_name;
	std::string m_title;

	CMapDisplayData m_data;
	bool m_withAnimation = false;

	std::string m_projection = PROJECTION_3D_VALUE;

	CLatLonRect m_zoom;

#if defined (BRAT_V3)
	double m_minXValue = defaultValue<double>();
	double m_maxXValue = defaultValue<double>();
	double m_minYValue = defaultValue<double>();
	double m_maxYValue = defaultValue<double>();
#endif


	//construction / destruction

	CDisplay( const CDisplay &o ) = delete;
	CDisplay& operator = ( const CDisplay &o ) = delete;

	void CloneDisplayData( const CMapDisplayData &o, const CWorkspaceDisplay *wksd, const CWorkspaceOperation *wkso );
public:

	CDisplay( const std::string &name )
		: base_t()
		, m_name( name )
	{}


	CDisplay( const CDisplay &o, const CWorkspaceDisplay *wksd, const CWorkspaceOperation *wkso ) 
		: base_t( o )
	{
#if defined (BRAT_V3)
		m_maxXValue = o.m_maxXValue;
		m_minXValue = o.m_minXValue;
		m_maxYValue = o.m_maxYValue;
		m_minYValue = o.m_minYValue;
#endif
		CloneDisplayData( o.m_data, wksd, wkso );

		m_name = o.m_name;
		m_withAnimation = o.m_withAnimation;
		m_projection = o.m_projection;
		m_title = o.m_title;
		m_type = o.m_type;

		m_zoom = const_cast<CLatLonRect&>( o.m_zoom );

		InitOutput( wksd );

	}

	//v4 new
	CDisplay* Clone( const std::string &name,  const CWorkspaceDisplay *wks, const CWorkspaceOperation *wkso ) const
	{
		CDisplay *d = new CDisplay( *this, wks, wkso );
		d->m_name = name;
		d->InitOutput( wks );
		return d;
	}

	virtual ~CDisplay();

private:
	void UpdateDisplayData( const CMapDisplayData *data_list, const CWorkspaceDisplay *wksd, const CWorkspaceOperation *wkso );

public:
	//serialization

	bool SaveConfig( CWorkspaceSettings* config, CWorkspaceDisplay *wksd ) const;
	bool LoadConfig( CWorkspaceSettings* config, std::string &errorMsg, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso );


	//properties

	const std::string& GetName() const { return m_name; }
	void SetName( const std::string& value )
	{ 
		m_name = value; 
		if ( m_title.empty() )
			m_title = m_name;
	}

	const std::string& GetTitle() const { return m_title; }
	void SetTitle( const std::string& value ) { m_title = value; }


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

	bool IsYFXType() const;
	bool IsZYFXType() const;
	bool IsZLatLonType() const;

	std::string GetProjection() const { return m_projection; }
	void SetProjection( const std::string& value ) { m_projection = value; }

	std::string GetWithAnimationAsText() const { return ( m_withAnimation ? "Y" : "N" ); }
	bool GetWithAnimation() const { return m_withAnimation; }
	void SetWithAnimation( bool value ) { m_withAnimation = value; }

	CLatLonRect* GetZoom() { return &m_zoom; }
	void SetZoom( CLatLonRect& value ) { m_zoom = value; }

	void InitOutput( const CWorkspaceDisplay *wks );

	std::string GetOutputFilename() const { return GetCmdFilename(); }
	std::string GetOutputName() const { return GetCmdFilePath(); }
	void SetOutput( const std::string& value ) { SetCmdFile( value ); }
	bool BuildCmdFile( std::string &error_msg, bool map_as_3dplot = false );

	std::string GetTaskName();
	std::string GetCmdFilename() const { return GetFileName( m_cmdFile ); }
	const std::string& GetCmdFilePath() const { return m_cmdFile; }
	void SetCmdFile( const std::string& value ) { m_cmdFile = value; clean_path( m_cmdFile ); }


#if defined(BRAT_V3)

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

	CDisplayData* GetDisplayData( const COperation *operation, const std::string &field_name )
	{
		std::string key = CDisplayData::MakeKey( operation, field_name, GetType() );
        return dynamic_cast< CDisplayData* >( m_data.Exists( key ) );            //hack
	}

	void SetAllAxis( unsigned index, const std::string& axisName, const std::string& axisLabel );
	void SetAllInvertXYAxes( bool value );
#else

    CDisplayData* GetFieldDisplayDataV3( const COperation *operation, const std::string &field_name );
    CDisplayData* GetFieldDisplayData( const std::string &field_name );
#endif

	void SetGroups( bool groupFields ) { groupFields ? GroupFields() : SplitFields(); }
protected:
	void GroupFields();
	void SplitFields();

public:
	bool AreFieldsGrouped() const;
	bool CheckFields( std::string &error_msg );

	bool ExistData( const std::string& key );
	bool InsertData( const std::string& key, CDisplayData* data );
	bool RemoveData( const std::string& key );

	CMapDisplayData* GetData() { return &m_data; }
	const CMapDisplayData& GetData() const { return m_data; }
	bool HasData() const { return GetDataCount() > 0; }
	size_t GetDataCount() const { return m_data.size(); }


	///Dump fonction
	virtual void Dump( std::ostream& fOut = std::cerr );
};

/** @} */



#endif // !defined(WORKSPACES_DISPLAY_H)
