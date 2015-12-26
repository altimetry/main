#include "new-gui/brat/stdafx.h"

#include "QtInterface.h"

#include "Common/QtUtils.h"
#include "Common/QtFileUtils.h"



//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//										CBratLookupTable	
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////////////////////////////
//										CBratLookupTable - Load
//////////////////////////////////////////////////////////////////////////////////////////////////////////


bool LoadFromFile( CBratLookupTable &lut, const std::string& fileName )
{
	Q_UNUSED(lut);
	Q_UNUSED(fileName);

	SimpleWarnBox("Development message: LoadFromFile( CBratLookupTable &lut, const std::string& fileName ) called");
	return true;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////
//										CBratLookupTable - Store
//////////////////////////////////////////////////////////////////////////////////////////////////////////


void SaveToFile( CBratLookupTable &lut, const std::string& fileName )
{
	Q_UNUSED(lut);
	Q_UNUSED(fileName);

	SimpleWarnBox("Development message: SaveToFile( CBratLookupTable &lut, const std::string& fileName ) called");
}




//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//										CConfiguration
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined (WIN32) || defined(_WIN32)
#pragma warning ( disable: 4100 )           //unreferenced formal parameter
#endif


#include "new-gui/brat/Workspaces/Dataset.h"

CConfiguration::CConfiguration( const std::string& fileName )
	//const QString & fileName, Format format, QObject * parent = 0)	
	: base_t( t2q( fileName), QSettings::IniFormat )
{}

const std::string CConfiguration::GetConfigPath() const
{
	return "";
}
void CConfiguration::SetPath( const std::string& strPath )
{
}
bool CConfiguration::Write( const std::string& key, const std::string& value )
{
	return true;
}
bool CConfiguration::Write( const std::string& key, const int& value )
{
	return true;
}
size_t CConfiguration::GetNumberOfEntries()
{
	return 0;
}
bool CConfiguration::GetNextEntry( std::string& str, long& lIndex )
{
	return true;
}
std::string CConfiguration::Read( const std::string& str ) const
{
	return "";
}
bool CConfiguration::Read( const std::string& str, std::string *value ) const
{
	return true;
}
std::string CConfiguration::Read( const std::string& str, const std::string &value ) const
{
	return "";
}
bool CConfiguration::Read( const std::string& str, int *value ) const
{
	return true;
}

bool CConfiguration::Read( const std::string& str, double *value, double defValue ) const
{
	return true;
}

bool CConfiguration::Read( const std::string& str, int32_t *value, double defValue ) const
{
	return true;
}

bool CConfiguration::Read( const std::string& str, bool *value, bool defValue ) const
{
	return true;
}

bool CConfiguration::Flush( bool bCurrentOnly )		//bCurrentOnly = false
{
	return true;
}

bool CConfiguration::DeleteAll()
{
	return true;
}

#if defined (WIN32) || defined(_WIN32)
#pragma warning ( default: 4100 )           //unreferenced formal parameter
#endif




bool CConfiguration::SaveConfig( const CDataset *d )
{
	////return
	////	SaveConfig( config, ENTRY_FILE ) &&
	////	SaveConfigSpecificUnit( config, ENTRY_UNIT );

	//// SaveConfig( CConfiguration *config, const std::string& entry )
	//{
	//	const std::string entry( ENTRY_FILE );

	//	SetPath( "/" + d->GetName() );

	//	int index = 0;
	//	bool bOk = true;
	//	for ( CProductList::const_iterator it = d->GetProductList()->begin(); it != d->GetProductList()->end(); it++ )
	//	{
	//		index++;
	//		bOk &= Write( entry + n2s<std::string>( index ), it->c_str() );
	//	}
	//	if ( !bOk )
	//		return false;
	//}

	////SaveConfigSpecificUnit( CConfiguration *config, const std::string& entry )
	//{
	//	const std::string entry( ENTRY_UNIT );

	//	SetPath( "/" + d->GetName() );

	//	bool bOk = true;
	//	for ( CStringMap::const_iterator itMap = d->GetFieldSpecificUnits()->begin(); itMap != d->GetFieldSpecificUnits()->end(); itMap++ )
	//	{
	//		bOk &= Write( entry + "_" + itMap->first, ( itMap->second ).c_str() );
	//	}

	//	return bOk;
	//}

	return true;
}

bool CConfiguration::LoadConfig( CDataset *d  )
{
	//bool bOk = true;

	//in workspace

	beginGroup( t2q( GROUP_DATASETS ) );
	QStringList dkeys = allKeys();
	QStringList dvalues;
	for ( auto dkey : dkeys )
	{
		SimpleMsgBox( dkey );
		auto dvalue = value( dkey, QString() ).toString();
		SimpleMsgBox( dvalue );
		dvalues << dvalue;
	}
	endGroup();

	//here

	for ( auto dvalue : dvalues )
	{
		beginGroup( dvalue /*t2q( d->GetName() )*/ );

		QStringList fkeys = allKeys();
		for ( auto fkey : fkeys )
			SimpleMsgBox( fkey );

		endGroup();
	}


	//SetPath( "/" + d->GetName() );

	//GetNumberOfEntries();
	//std::string entry;
	//std::string valueString;
	//long i = 0;

	//CStringArray findStrings;
	//while ( GetNextEntry( entry, i ) )
	//{
	//	// Finds ENTRY_FILE entries (dataset files entries)
	//	findStrings.RemoveAll();
	//	CTools::Find( entry, ENTRY_FILE_REGEX, findStrings );

	//	if ( findStrings.size() > 0 )
	//	{
	//		valueString = Read( entry );
	//		d->GetProductList()->Insert( valueString );
	//		continue;
	//	}

	//	// Finds specific unit entries
	//	findStrings.RemoveAll();
	//	CTools::Find( entry, ENTRY_UNIT_REGEX, findStrings );

	//	if ( findStrings.size() > 0 )
	//	{
	//		valueString = Read( entry );
	//		d->GetFieldSpecificUnits()->Insert( findStrings.at( 0 ), valueString );			//m_fieldSpecificUnit.Dump(*CTrace::GetDumpContext());
	//		continue;
	//	}
	//}

	return true;
}

bool CConfiguration::LoadConfig( CDisplayData &data, const std::string& path, CWorkspaceDisplay *wks, CWorkspaceOperation *wkso )
{
	//SetPath( "/" + path );

	//std::string valueString = Read( ENTRY_TYPE, CMapTypeDisp::GetInstance().IdToName( data.m_type ) );
	//if ( valueString.empty() )
	//{
	//	data.m_type = -1;
	//}
	//else
	//{
	//	// Because Z=F(Lat,Lon) have been changed to Z=F(Lon,Lat)
	//	if ( str_icmp( valueString.c_str(), "Z=F(Lat,Lon)" ) )
	//	{
	//		data.m_type = CMapTypeDisp::eTypeDispZFLatLon;
	//	}
	//	else
	//	{
	//		data.m_type = CMapTypeDisp::GetInstance().NameToId( valueString );
	//	}
	//}


	//valueString = Read( ENTRY_FIELD );
	//data.m_field.SetName( (const char *)valueString.c_str() );

	//valueString = Read( ENTRY_FIELDNAME );
	//data.m_field.SetDescription( (const char *)valueString.c_str() );

	//valueString = Read( ENTRY_UNIT );
	//data.m_field.SetUnit( (const char *)valueString.c_str() );

	//valueString = Read( ENTRY_X );
	//data.m_x.SetName( (const char *)valueString.c_str() );

	//valueString = Read( ENTRY_XDESCR );
	//data.m_x.SetDescription( (const char *)valueString.c_str() );

	//valueString = Read( ENTRY_XUNIT );
	//data.m_x.SetUnit( (const char *)valueString.c_str() );

	//valueString = Read( ENTRY_Y );
	//data.m_y.SetName( (const char *)valueString.c_str() );

	//valueString = Read( ENTRY_YDESCR );
	//data.m_y.SetDescription( (const char *)valueString.c_str() );

	//valueString = Read( ENTRY_YUNIT );
	//data.m_y.SetUnit( (const char *)valueString.c_str() );

	//valueString = Read( ENTRY_Z );
	//data.m_z.SetName( (const char *)valueString.c_str() );

	//valueString = Read( ENTRY_ZDESCR );
	//data.m_z.SetDescription( (const char *)valueString.c_str() );

	//valueString = Read( ENTRY_ZUNIT );
	//data.m_z.SetUnit( (const char *)valueString.c_str() );

	//valueString = Read( ENTRY_OPNAME );
	//data.m_operation = CDisplay::FindOperation( valueString, wkso );

	//Read( ENTRY_GROUP, &data.m_group, 1 );
	//Read( ENTRY_CONTOUR, &data.m_withContour, false );
	//Read( ENTRY_SOLID_COLOR, &data.m_withSolidColor, true );

	//Read( ENTRY_EAST_COMPONENT, &data.m_eastcomponent, false );
	//Read( ENTRY_NORTH_COMPONENT, &data.m_northcomponent, false );

	//Read( ENTRY_INVERT_XYAXES, &data.m_invertXYAxes, false );

	//Read( ENTRY_MINVALUE, &data.m_minValue, CTools::m_defaultValueDOUBLE );
	//Read( ENTRY_MAXVALUE, &data.m_maxValue, CTools::m_defaultValueDOUBLE );

	////config->Read(ENTRY_COLOR_PALETTE, &m_colorPalette, PALETTE_AEROSOL);
	//Read( ENTRY_COLOR_PALETTE, &data.m_colorPalette );

	//Read( ENTRY_X_AXIS, &data.m_xAxis );

	////config->Read(ENTRY_X_AXIS_TEXT, &m_xAxisText);

	//// if color palette is a file (it has an extension)
	//// save path in absolute form, based on workspace Display path

	//wxFileName relative;
	//relative.Assign( data.m_colorPalette );
	//std::string paletteToSet = data.m_colorPalette;
	//if ( relative.HasExt() )
	//{
	//	//CWorkspaceDisplay* wks = wxGetApp().GetCurrentWorkspaceDisplay();
	//	if ( wks != nullptr )
	//	{
	//		relative.Normalize( wxPATH_NORM_ALL, wks->GetPath() );
	//		data.m_colorPalette = relative.GetFullPath();
	//	}
	//}
	return true;
}

bool CConfiguration::SaveConfig( CDisplayData &data, const std::string& pathSuff, CWorkspaceDisplay *wksd )
{
	//bool bOk = true;

	//std::string path = data.GetDataKey();
	//if ( pathSuff.empty() == false )
	//{
	//	path += "_" + pathSuff;
	//}
	//SetPath( "/" + path );

	//bOk &= Write( ENTRY_TYPE,
	//	CMapTypeDisp::GetInstance().IdToName( data.m_type ) );


	//bOk &= Write( ENTRY_FIELD, data.m_field.GetName().c_str() );
	//bOk &= Write( ENTRY_FIELDNAME, data.m_field.GetDescription().c_str() );
	//bOk &= Write( ENTRY_UNIT, data.m_field.GetUnit().c_str() );

	//bOk &= Write( ENTRY_X, data.m_x.GetName().c_str() );
	//bOk &= Write( ENTRY_XDESCR, data.m_x.GetDescription().c_str() );
	//bOk &= Write( ENTRY_XUNIT, data.m_x.GetUnit().c_str() );

	//bOk &= Write( ENTRY_Y, data.m_y.GetName().c_str() );
	//bOk &= Write( ENTRY_YDESCR, data.m_y.GetDescription().c_str() );
	//bOk &= Write( ENTRY_YUNIT, data.m_y.GetUnit().c_str() );

	//bOk &= Write( ENTRY_Z, data.m_z.GetName().c_str() );
	//bOk &= Write( ENTRY_ZDESCR, data.m_z.GetDescription().c_str() );
	//bOk &= Write( ENTRY_ZUNIT, data.m_z.GetUnit().c_str() );


	//if ( data.m_operation != nullptr )
	//{
	//	bOk &= Write( ENTRY_OPNAME, data.m_operation->GetName().c_str() );
	//}

	//bOk &= Write( ENTRY_GROUP, data.m_group );
	//bOk &= Write( ENTRY_CONTOUR, data.m_withContour );
	//bOk &= Write( ENTRY_SOLID_COLOR, data.m_withSolidColor );
	//bOk &= Write( ENTRY_EAST_COMPONENT, data.m_eastcomponent );
	//bOk &= Write( ENTRY_NORTH_COMPONENT, data.m_northcomponent );

	//bOk &= Write( ENTRY_INVERT_XYAXES, data.m_invertXYAxes );

	//if ( isDefaultValue( data.m_minValue ) == false )
	//{
	//	bOk &= Write( ENTRY_MINVALUE, CTools::Format( "%.15g", data.m_minValue ).c_str() );
	//}
	//if ( isDefaultValue( data.m_maxValue ) == false )
	//{
	//	bOk &= Write( ENTRY_MAXVALUE, CTools::Format( "%.15g", data.m_maxValue ).c_str() );
	//}
	//if ( data.m_colorPalette.empty() == false )
	//{
	//	// if color palette is a file (it has an extension)
	//	// save path in relative form, based on workspace Display path
	//	wxFileName relative;
	//	relative.Assign( data.m_colorPalette );
	//	std::string paletteToWrite = data.m_colorPalette;
	//	if ( relative.HasExt() )
	//	{
	//		//CWorkspaceDisplay* wks = wxGetApp().GetCurrentWorkspaceDisplay();
	//		if ( wksd != nullptr )
	//		{
	//			relative.MakeRelativeTo( wksd->GetPath() );
	//			paletteToWrite = relative.GetFullPath();
	//		}
	//	}

	//	bOk &= Write( ENTRY_COLOR_PALETTE, paletteToWrite );
	//}

	//if ( data.m_xAxis.empty() == false )
	//{
	//	Write( ENTRY_X_AXIS, data.m_xAxis );
	//}

	return true;
}

bool CConfiguration::SaveConfig( const CMapFormula &mapf, bool predefined, const std::string& pathSuff )
{
	return true;
}
bool CConfiguration::LoadConfig( CMapFormula &mapf, std::string &errorMsg, bool predefined, const std::string& pathSuff )
{
	return true;
}
bool CConfiguration::SaveCommonConfig( const CWorkspace &wks, bool flush )
{
	return true;
}
bool CConfiguration::LoadCommonConfig( CWorkspace &wks )
{
	return true;
}
bool CConfiguration::SaveConfigDataset( const CWorkspaceDataset &data, std::string &errorMsg )
{
	return true;
}
bool CConfiguration::LoadConfigDataset( CWorkspaceDataset &data, std::string &errorMsg )
{
	return true;
}
bool CConfiguration::LoadConfigOperation( CWorkspaceOperation &op, std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceOperation *wkso )
{
	return true;
}
bool CConfiguration::SaveConfigOperation( const CWorkspaceOperation &op, std::string &errorMsg )
{
	return true;
}
bool CConfiguration::SaveConfigDisplay( const CWorkspaceDisplay &disp, std::string &errorMsg, CWorkspaceDisplay *wksd )
{
	return true;
}
bool CConfiguration::LoadConfigDisplay( CWorkspaceDisplay &disp, std::string &errorMsg, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso )
{
	return true;
}




//////////////////////////////////////////////////////////////////////////////////////////////////////////
//											File Names
//////////////////////////////////////////////////////////////////////////////////////////////////////////


std::string GetFileName( const std::string &path )
{
	return GetBaeFilenameFromPath( path );
}

void SetFileExtension( std::string &path, const std::string &extension )
{
	std::string dir = GetDirectoryFromPath( path );
	std::string name = GetFilenameFromPath( path );
	path = path + "/" + name + "." + extension;
}

std::string NormalizedPath( const std::string &path, const std::string &dir )
{
	std::string d = NormalizedPath( dir );
	std::string f = NormalizedPath( path );
	if ( QDir::isAbsolutePath( t2q( f ) ) )
		return f;
	else
		return dir + "/" + path;
}
