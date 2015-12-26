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

#include "brathl.h"

#include "BratObject.h"
#include "Field.h"
#include "LatLonRect.h"
#include "InternalFiles.h"

#include "new-gui/brat/Workspaces/Operation.h"

namespace brathl 
{
	class CInternalFiles;
};

class COperation;
class CFormula;
class CWorkspaceDisplay;


using namespace brathl;



//-------------------------------------------------------------
//------------------- CDisplayData class --------------------
//-------------------------------------------------------------

class CDisplayData : public CBratObject
{
	friend struct CConfiguration;

public:
	static const char* FMT_FLOAT_XY;

private:
	int m_type = -1;

	CFieldBasic m_field;
	CFieldBasic m_x;
	CFieldBasic m_y;
	CFieldBasic m_z;
	COperation* m_operation = nullptr;
	int m_group = 1;
	bool m_withContour = false;
	bool m_withSolidColor = true;

	double m_minValue = defaultValue<double>();
	double m_maxValue = defaultValue<double>();

	std::string m_colorPalette;

	std::string m_xAxis;

	CObArray m_dimFields;

	bool m_invertXYAxes = false;

	bool m_eastcomponent = false;
	bool m_northcomponent = false;

public:
	static std::string GetValueAsText( double value );

private:
	void Init()
	{
		m_dimFields.SetDelete( false );
		m_dimFields.Insert( GetX() );
		m_dimFields.Insert( GetY() );
		m_dimFields.Insert( GetZ() );
	}

	CDisplayData( const CDisplayData &o) = delete;
	CDisplayData& operator = ( const CDisplayData &o) = delete;

public:
	CDisplayData()
	{
		Init();
	}
	
	CDisplayData( COperation* operation )
	{
		Init();
		m_operation = operation;
	}
	
	CDisplayData( const CDisplayData &o, CWorkspaceOperation *wkso );

	~CDisplayData()
	{}


	COperation* GetOperation() { return m_operation; }
	void SetOperation( COperation* value ) { m_operation = value; }

	std::string GetGroupAsText() const { return n2s< std::string >( m_group ); }
	int32_t GetGroup() { return m_group; }
	void SetGroup( int32_t value ) { m_group = value; }

	std::string GetContourAsText() const { return ( m_withContour ? "Y" : "N" ); }
	bool IsContour() const { return GetContour(); }
	bool GetContour() const { return m_withContour; }
	void SetContour( bool value ) { m_withContour = value; }

	std::string GetSolidColorAsText() const { return ( m_withSolidColor ? "Y" : "N" ); }
	bool IsSolidColor() const { return GetSolidColor(); }
	bool GetSolidColor() const { return m_withSolidColor; }
	void SetSolidColor( bool value ) { m_withSolidColor = value; }

	std::string GetMinValueAsText() const { return GetValueAsText( m_minValue ); }
	double GetMinValue() const { return m_minValue; }
	void SetMinValue( double value ) { m_minValue = value; }

	std::string GetMaxValueAsText() const { return GetValueAsText( m_maxValue ); }
	double GetMaxValue() const { return m_maxValue; }
	void SetMaxValue( double value ) { m_maxValue = value; }

	std::string GetColorPalette() const { return m_colorPalette; }
	void SetColorPalette( const std::string& value ) { m_colorPalette = value; }

	const CFieldBasic* GetField() const { return &m_field; }
	CFieldBasic* GetField() { return &m_field; }
	CFieldBasic* GetX() { return &m_x; }
	CFieldBasic* GetY() { return &m_y; }
	CFieldBasic* GetZ() { return &m_z; }

	std::string GetXAxis() const { return m_xAxis; }
	void SetXAxis( const std::string& value ) { m_xAxis = value; }

	std::string GetXAxisText( const std::string& name );
	std::string GetXAxisText( uint32_t index );
	void SetXAxisText( uint32_t index, const std::string& value );

	bool HasXComplement();
	void GetXComplement( CStringArray& complement );

	void GetAvailableAxes( CStringArray& names );

	void GetAvailableDisplayTypes( CUIntArray& displayTypes );

	CObArray* GetDimFields() { return &m_dimFields; }

	bool IsYFXType();
	bool IsZYFXType();
	bool IsZLatLonType();

	bool IsInvertXYAxes() const { return m_invertXYAxes; }
	void SetInvertXYAxes( bool value ) { m_invertXYAxes = value; }

	bool IsEastComponent() const { return m_eastcomponent; }
	void SetEastComponent( bool value ) { m_eastcomponent = value; }

	bool IsNorthComponent() const { return m_northcomponent; }
	void SetNorthComponent( bool value ) { m_northcomponent = value; }

	std::string GetDataKey();
	std::string GetDataKey( int32_t type );

	int32_t GetType() const { return m_type; }
	void SetType( int32_t value ) { m_type = value; }


	bool LoadConfig( CConfiguration* config, const std::string& path, CWorkspaceDisplay *wks, CWorkspaceOperation *wkso );
	bool SaveConfig( CConfiguration* config, const std::string& pathSuff, CWorkspaceDisplay *wks );

	void CopyFieldUserProperties( CDisplayData& d );
};

//-------------------------------------------------------------
//------------------- CMapDisplayData class --------------------
//-------------------------------------------------------------

class CDisplay;


class CMapDisplayData : public CObMap
{
	CMapDisplayData( const CMapDisplayData &o ) = delete;
	CMapDisplayData& operator = ( const CMapDisplayData &o ) = delete;

public:
	/// CMapDisplayData ctor
	CMapDisplayData()
	{}

	CMapDisplayData( const CMapDisplayData &o, CWorkspaceOperation *wkso );

	/// CMapDisplayData dtor
	virtual ~CMapDisplayData()
	{}

	const CDisplayData* GetDisplayData( const std::string& name ) const;
	const CDisplayData* GetDisplayData( const char* name ) const;
	CDisplayData* GetDisplayData( CMapDisplayData::const_iterator& it );

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

	bool LoadConfig( CConfiguration* config, std::string &errorMsg, CWorkspaceDisplay *wks, CWorkspaceOperation *wkso, const std::string& pathSuff = "" );

	bool SaveConfig( CConfiguration* config, CWorkspaceDisplay *wks, const std::string& pathSuff = "" );

	bool CheckFields( std::string& errorMsg, CDisplay* display );

	void SetAllAxis( uint32_t index, const std::string& axisName, const std::string& axisLabel );
	void SetAllInvertXYAxes( bool value );
};

//-------------------------------------------------------------
//------------------- CDisplay class --------------------
//-------------------------------------------------------------


class CDisplay : public CBratObject
{
	friend class CDisplayCmdFile;

public:
	static void GetDisplayType( const COperation* operation, CUIntArray& displayTypes, CInternalFiles** pf = nullptr );

	static COperation* FindOperation( const std::string& name, CWorkspaceOperation *wkso );

protected:
	static const std::string m_zoomDelimiter;

public:
	const int32_t m_verbose = 2;

protected:
	int32_t m_type;
	std::string m_cmdFile;
	//wxFile m_file;

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

	CDisplay( const CDisplay &o, CWorkspaceDisplay *wks, CWorkspaceOperation *wkso ) : m_data( o.m_data, wkso )
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

	//CDisplay( CDisplay& d );
//----------------------------------------
//CDisplay::CDisplay(CDisplay& d)
//{
//  Init();
//  Copy(d);
//}
	//CDisplay& operator=( CDisplay& d );
//CDisplay& CDisplay::operator=(CDisplay& d)
//{
//  Copy(d);
//  return *this;
//}
//

protected:
	//void Copy( CDisplay& d );
	void Init();

public:
	/// Destructor
	virtual ~CDisplay();

	const std::string& GetName() const { return m_name; }
	void SetName( const std::string& value ) { m_name = value; }

	bool HasData() const { return GetDataCount() > 0; }
	size_t GetDataCount() const { return m_data.size(); }

	bool SaveConfig( CConfiguration* config, CWorkspaceDisplay *wksd );
	bool LoadConfig( CConfiguration* config, std::string &errorMsg, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso );

	bool UseOperation( const std::string& name, std::string& errorMsg );

	int32_t GetType() const { return m_type; }
	void SetType( int32_t value ) { m_type = value; }

	std::string GetTypeAsString() const { return n2s< std::string >( m_type ); }

	CMapDisplayData* GetDataSelected() { return &m_data; }
	const CMapDisplayData& GetDataSelected() const { return m_data; }
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


	CDisplayData* GetDisplayData( const std::string& name );
	CDisplayData* GetDisplayData( CMapDisplayData::iterator it );

	std::string GetOutputFilename() const { return GetCmdFilename(); }
	std::string GetOutputName() const { return GetCmdFilePath(); }
	void SetOutput( const std::string& value ) { SetCmdFile( value ); }

	bool AreFieldsGrouped() const { return m_data.AreFieldsGrouped(); }

	void SetGroups( bool groupFields );

	std::string GetTaskName();
	std::string GetCmdFilename() const { return GetFileName( m_cmdFile ); }
	const std::string& GetCmdFilePath() const { return m_cmdFile; }
	void SetCmdFile( const std::string& value ) { m_cmdFile = value; clean_path( m_cmdFile ); }

	void SetAllAxis( uint32_t index, const std::string& axisName, const std::string& axisLabel ) { m_data.SetAllAxis( index, axisName, axisLabel ); }
	void SetAllInvertXYAxes( bool value ) { m_data.SetAllInvertXYAxes( value ); }

	CLatLonRect* GetZoom() { return &m_zoom; }
	void SetZoom( CLatLonRect& value ) { m_zoom = value; }

	std::string GetFullCmd();


	bool BuildCmdFile();
private:
	//bool BuildCmdFileHeader();
	//bool BuildCmdFileGeneralProperties();
	//bool BuildCmdFileGeneralPropertiesXY();
	//bool BuildCmdFileGeneralPropertiesZXY();
	//bool BuildCmdFileGeneralPropertiesZLatLon();
	//bool BuildCmdFileFields();
	//bool BuildCmdFileFieldsZFXY();
	//bool BuildCmdFileFieldsYFX();
	//bool BuildCmdFileFieldProperties( const std::string& dataKey );
	//bool BuildCmdFileFieldPropertiesXY( const CDisplayData* value );
	//bool BuildCmdFileFieldPropertiesZXY( const CDisplayData* value );

	//bool BuildCmdFileVerbose();

	//bool WriteComment( const std::string& text );
	//bool WriteLine( const std::string& text );
	//bool WriteEmptyLine();

public:
	void InitOutput( CWorkspaceDisplay *wks );

	bool IsYFXType();
	bool IsZYFXType();
	bool IsZLatLonType();

	///Dump fonction
	virtual void Dump( std::ostream& fOut = std::cerr );
};

//----------------------------------------------------------------------------
// CDndDisplayData
//----------------------------------------------------------------------------

struct CDndDisplayData
{
	CObArray m_data;

	CDndDisplayData() : m_data( false ) 
	{}
	CDndDisplayData( CDisplayData* displayData ) { m_data.Insert( displayData ); }
	CDndDisplayData( const CObArray& data ) { Set( data ); }
	CDndDisplayData( const CObArray* data ) { Set( data ); }

	CDndDisplayData( CDndDisplayData& dndDisplayData )
		: m_data( false )
	{
		Set( dndDisplayData );
	}

	virtual ~CDndDisplayData()
	{}

	void Set( CDndDisplayData& dndDisplayData )
	{
		Set( dndDisplayData.m_data );
	}

	void Set( const CObArray& data )
	{
		m_data.RemoveAll();
		for ( CObArray::const_iterator it = data.begin(); it != data.end(); it++ )
			m_data.Insert( *it );
	}

	void Set( const CObArray* data )
	{
		Set( *data );
	}
};


/** @} */



#endif // !defined(WORKSPACES_DISPLAY_H)
