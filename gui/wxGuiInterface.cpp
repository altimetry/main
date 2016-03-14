
#if !defined(BRAT_V3)		//help IntelliSense
#define BRAT_V3
#endif

#include "BratGui.h"
#include "wxGuiInterface.h"
#include "new-gui/QtInterface.h"
#include "new-gui/Common/ConfigurationKeywords.h"
#include "new-gui/brat/DataModels/Workspaces/Dataset.h"
#include "new-gui/brat/DataModels/MapTypeDisp.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//											Configurations
//////////////////////////////////////////////////////////////////////////////////////////////////////////



CConfiguration::CConfiguration( 
	//const std::string& appName,
	//const std::string& vendorName,
	const std::string& localFilename
	//const std::string& globalFilename
	////, long style = wxCONFIG_USE_LOCAL_FILE | wxCONFIG_USE_GLOBAL_FILE 
	)
	: base_t( "", "", localFilename, "", wxCONFIG_USE_LOCAL_FILE )
{}



const std::string GROUP_GENERAL = "General";



bool StdGetNextEntry( CConfiguration *config, std::string& str, long& lIndex )
{
	wxString wstr( str );
	bool result = config->GetNextEntry( wstr, lIndex );
	str = wstr.ToStdString();
	return result;
}
bool StdRead( CConfiguration *config, const std::string& str, std::string *value )
{
	wxString wvalue( *value );
	bool result = config->Read( str, &wvalue );
	*value = wvalue.ToStdString();
	return result;
}
void CConfiguration::Sync()
{
	base_t::Flush();
}
void CConfiguration::Clear()
{
	base_t::DeleteAll();
}


bool CConfiguration::SaveConfig( const CDataset *d )
{
	//return
	//	SaveConfig( config, ENTRY_FILE ) &&
	//	SaveConfigSpecificUnit( config, ENTRY_UNIT );

	// SaveConfig( CConfiguration *config, const std::string& entry )
	{
		const std::string entry( ENTRY_FILE );

		SetPath( "/" + d->GetName() );

		int index = 0;
		bool bOk = true;
		for ( CProductList::const_iterator it = d->GetProductList()->begin(); it != d->GetProductList()->end(); it++ )
		{
			index++;
			bOk &= Write( entry + n2s<std::string>( index ), it->c_str() );
		}
		if ( !bOk )
			return false;
	}

	//SaveConfigSpecificUnit( CConfiguration *config, const std::string& entry )
	{
		const std::string entry( ENTRY_UNIT );

		SetPath( "/" + d->GetName() );

		bool bOk = true;
		for ( CStringMap::const_iterator itMap = d->GetFieldSpecificUnits()->begin(); itMap != d->GetFieldSpecificUnits()->end(); itMap++ )
		{
			bOk &= Write( entry + "_" + itMap->first, ( itMap->second ).c_str() );
		}

		return bOk;
	}
}

bool CConfiguration::LoadConfig( CDataset *d  )
{
	bool bOk = true;

	SetPath( "/" + d->GetName() );

	GetNumberOfEntries();
	std::string entry;
	std::string valueString;
	long i = 0;

	CStringArray findStrings;
	while ( StdGetNextEntry( this, entry, i ) )
	{
		// Finds ENTRY_FILE entries (dataset files entries)
		findStrings.RemoveAll();
		CTools::Find( entry, ENTRY_FILE_REGEX, findStrings );

		if ( findStrings.size() > 0 )
		{
			valueString = Read( entry );
			d->InsertProduct( valueString );
			continue;
		}

		// Finds specific unit entries
		findStrings.RemoveAll();
		CTools::Find( entry, ENTRY_UNIT_REGEX, findStrings );

		if ( findStrings.size() > 0 )
		{
			valueString = Read( entry );
			d->GetFieldSpecificUnits()->Insert( findStrings.at( 0 ), valueString );			//m_fieldSpecificUnit.Dump(*CTrace::GetDumpContext());
			continue;
		}
	}

	return true;
}



bool CConfiguration::LoadConfig( CDisplayData &data, const std::string& path, CWorkspaceDisplay *wks, CWorkspaceOperation *wkso )
{
	SetPath( "/" + path );

    std::string valueString = Read( ENTRY_TYPE, CMapTypeDisp::GetInstance().IdToName( data.m_type ) ).ToStdString();
	if ( valueString.empty() )
	{
		data.m_type = -1;
	}
	else
	{
		// Because Z=F(Lat,Lon) have been changed to Z=F(Lon,Lat)
		if ( str_icmp( valueString.c_str(), "Z=F(Lat,Lon)" ) )
		{
			data.m_type = CMapTypeDisp::eTypeDispZFLatLon;
		}
		else
		{
			data.m_type = CMapTypeDisp::GetInstance().NameToId( valueString );
		}
	}


	valueString = Read( ENTRY_FIELD );
	data.m_field.SetName( (const char *)valueString.c_str() );

	valueString = Read( ENTRY_FIELDNAME );
	data.m_field.SetDescription( (const char *)valueString.c_str() );

	valueString = Read( ENTRY_UNIT );
	data.m_field.SetUnit( (const char *)valueString.c_str() );

	valueString = Read( ENTRY_X );
	data.m_x.SetName( (const char *)valueString.c_str() );

	valueString = Read( ENTRY_XDESCR );
	data.m_x.SetDescription( (const char *)valueString.c_str() );

	valueString = Read( ENTRY_XUNIT );
	data.m_x.SetUnit( (const char *)valueString.c_str() );

	valueString = Read( ENTRY_Y );
	data.m_y.SetName( (const char *)valueString.c_str() );

	valueString = Read( ENTRY_YDESCR );
	data.m_y.SetDescription( (const char *)valueString.c_str() );

	valueString = Read( ENTRY_YUNIT );
	data.m_y.SetUnit( (const char *)valueString.c_str() );

	valueString = Read( ENTRY_Z );
	data.m_z.SetName( (const char *)valueString.c_str() );

	valueString = Read( ENTRY_ZDESCR );
	data.m_z.SetDescription( (const char *)valueString.c_str() );

	valueString = Read( ENTRY_ZUNIT );
	data.m_z.SetUnit( (const char *)valueString.c_str() );

	valueString = Read( ENTRY_OPNAME );
	data.m_operation = wkso->GetOperation( valueString );

	Read( ENTRY_GROUP, &data.m_group, 1 );
	Read( ENTRY_CONTOUR, &data.m_withContour, false );
	Read( ENTRY_SOLID_COLOR, &data.m_withSolidColor, true );

	Read( ENTRY_EAST_COMPONENT, &data.m_eastcomponent, false );
	Read( ENTRY_NORTH_COMPONENT, &data.m_northcomponent, false );

	Read( ENTRY_INVERT_XYAXES, &data.m_invertXYAxes, false );

	Read( ENTRY_MINVALUE, &data.m_minValue, CTools::m_defaultValueDOUBLE );
	Read( ENTRY_MAXVALUE, &data.m_maxValue, CTools::m_defaultValueDOUBLE );

	//Read(ENTRY_COLOR_PALETTE, &m_colorPalette, PALETTE_AEROSOL);
	//femm Read( ENTRY_COLOR_PALETTE, &data.m_colorPalette );
	Read( ENTRY_COLOR_PALETTE, data.m_colorPalette );

	//femm Read( ENTRY_X_AXIS, &data.m_xAxis );
	Read( ENTRY_X_AXIS, data.m_xAxis );

	//Read(ENTRY_X_AXIS_TEXT, &m_xAxisText);

	// if color palette is a file (it has an extension)
	// save path in absolute form, based on workspace Display path

	wxFileName relative;
	relative.Assign( data.m_colorPalette );
	std::string paletteToSet = data.m_colorPalette;
	if ( relative.HasExt() )
	{
		//CWorkspaceDisplay* wks = wxGetApp().GetCurrentWorkspaceDisplay();
		if ( wks != nullptr )
		{
			relative.Normalize( wxPATH_NORM_ALL, wks->GetPath() );
			data.m_colorPalette = relative.GetFullPath();
		}
	}
	return true;
}

bool CConfiguration::SaveConfig( CDisplayData &data, const std::string& pathSuff, CWorkspaceDisplay *wksd )
{
	bool bOk = true;

	std::string path = data.GetDataKey();
	if ( pathSuff.empty() == false )
	{
		path += "_" + pathSuff;
	}
	SetPath( "/" + path );

	bOk &= Write( ENTRY_TYPE, CMapTypeDisp::GetInstance().IdToName( data.m_type ).c_str() );

	bOk &= Write( ENTRY_FIELD, data.m_field.GetName().c_str() );
	bOk &= Write( ENTRY_FIELDNAME, data.m_field.GetDescription().c_str() );
	bOk &= Write( ENTRY_UNIT, data.m_field.GetUnit().c_str() );

	bOk &= Write( ENTRY_X, data.m_x.GetName().c_str() );
	bOk &= Write( ENTRY_XDESCR, data.m_x.GetDescription().c_str() );
	bOk &= Write( ENTRY_XUNIT, data.m_x.GetUnit().c_str() );

	bOk &= Write( ENTRY_Y, data.m_y.GetName().c_str() );
	bOk &= Write( ENTRY_YDESCR, data.m_y.GetDescription().c_str() );
	bOk &= Write( ENTRY_YUNIT, data.m_y.GetUnit().c_str() );

	bOk &= Write( ENTRY_Z, data.m_z.GetName().c_str() );
	bOk &= Write( ENTRY_ZDESCR, data.m_z.GetDescription().c_str() );
	bOk &= Write( ENTRY_ZUNIT, data.m_z.GetUnit().c_str() );


	if ( data.m_operation != nullptr )
	{
		bOk &= Write( ENTRY_OPNAME, data.m_operation->GetName().c_str() );
	}

	bOk &= Write( ENTRY_GROUP, data.m_group );
	bOk &= Write( ENTRY_CONTOUR, data.m_withContour );
	bOk &= Write( ENTRY_SOLID_COLOR, data.m_withSolidColor );
	bOk &= Write( ENTRY_EAST_COMPONENT, data.m_eastcomponent );
	bOk &= Write( ENTRY_NORTH_COMPONENT, data.m_northcomponent );

	bOk &= Write( ENTRY_INVERT_XYAXES, data.m_invertXYAxes );

	if ( isDefaultValue( data.m_minValue ) == false )
	{
		bOk &= Write( ENTRY_MINVALUE, CTools::Format( "%.15g", data.m_minValue ).c_str() );
	}
	if ( isDefaultValue( data.m_maxValue ) == false )
	{
		bOk &= Write( ENTRY_MAXVALUE, CTools::Format( "%.15g", data.m_maxValue ).c_str() );
	}
	if ( data.m_colorPalette.empty() == false )
	{
		// if color palette is a file (it has an extension)
		// save path in relative form, based on workspace Display path
		wxFileName relative;
		relative.Assign( data.m_colorPalette );
		std::string paletteToWrite = data.m_colorPalette;
		if ( relative.HasExt() )
		{
			//CWorkspaceDisplay* wks = wxGetApp().GetCurrentWorkspaceDisplay();
			if ( wksd != nullptr )
			{
				relative.MakeRelativeTo( wksd->GetPath() );
				paletteToWrite = relative.GetFullPath();
			}
		}

		bOk &= Write( ENTRY_COLOR_PALETTE, paletteToWrite.c_str() );
	}

	if ( data.m_xAxis.empty() == false )
	{
		Write( ENTRY_X_AXIS, data.m_xAxis.c_str() );
	}

	return true;
}


bool CConfiguration::LoadConfig( CMapFormula &mapf, std::string &errorMsg, bool predefined, const std::string& pathSuff )
{
	bool bOk = true;

	std::string path = GROUP_FORMULAS;
	if ( pathSuff.empty() == false )
	{
		path += "_" + pathSuff;
	}
	SetPath( "/" + path );

	std::string entry;
	std::string valueString;
	std::string formulaName;
	long i = 0;

	do
	{
		bOk = StdGetNextEntry( this, entry, i );
		if ( bOk )
		{
			valueString = Read( entry );

			if ( !pathSuff.empty() )
			{
				//formulaName = valueString.Left(valueString.Length() - pathSuff.Length() - 1);
				formulaName = valueString.substr( 0, valueString.length() - pathSuff.length() - 1 );
			}
			else
			{
				formulaName = valueString;
			}
			CFormula* value = dynamic_cast<CFormula*>( mapf.Exists( formulaName ) );
			if ( value != nullptr )
			{
				mapf.Erase( formulaName );
			}

			mapf.Insert( formulaName, new CFormula( formulaName, predefined ) );
		}
	} while ( bOk );

	//int32_t index = 0;

	for ( CMapFormula::iterator it = mapf.begin(); it != mapf.end(); it++ )
	{
		//index++;
		CFormula* formula = dynamic_cast<CFormula*>( it->second );
		if ( formula == nullptr )
		{
			errorMsg +=
				"ERROR in  CMapFormula::LoadConfig\ndynamic_cast<CFormula*>(it->second) returns nullptr pointer"
				"\nList seems to contain objects other than those of the class CFormula.\n";

			//wxMessageBox(,                   "Error",                    wxOK | wxCENTRE | wxICON_ERROR);

			return false;
		}

		formula->LoadConfig( this, errorMsg, pathSuff );
	}

	return true;
}

bool CConfiguration::SaveConfig( const CMapFormula &mapf, bool predefined, const std::string& pathSuff )
{
	bool bOk = true;
	int index = 0;
	for ( CMapFormula::const_iterator it = mapf.begin(); it != mapf.end(); it++ )
	{
		std::string path = GROUP_FORMULAS;
		if ( pathSuff.empty() == false )
		{
			path += "_" + pathSuff;
		}
		SetPath( "/" + path );

		CFormula* formula = dynamic_cast<CFormula*>( it->second );
		if ( formula != nullptr )
		{
			if ( formula->IsPredefined() != predefined )
				continue;

			index++;
			std::string formulaName = formula->GetName();
			if ( pathSuff.empty() == false )
			{
				formulaName += "_" + pathSuff;
			}

			Write( ENTRY_FORMULA + n2s<std::string>( index ), formulaName.c_str() );

			formula->SaveConfig( this, pathSuff );
		}
	}
	return true;
}


bool CConfiguration::SaveCommonConfig( const CWorkspace &wks, bool flush )
{
	assert__( this == wks.m_config );

	bool bOk = true;

	SetPath( "/" + GROUP_GENERAL );
	bOk &= Write( ENTRY_WKSNAME, wks.m_name.c_str() );
	SetPath( "/" + GROUP_GENERAL );
	bOk &= Write( ENTRY_WKSLEVEL, wks.m_level );

	if ( flush )
		Flush();

	return bOk;
}


bool CConfiguration::LoadCommonConfig( CWorkspace &wks )
{
	assert__( this == wks.m_config );

	SetPath( "/" + GROUP_GENERAL );
	wks.m_name = "";

	return 
		StdRead( this, ENTRY_WKSNAME, &wks.m_name ) &&
		Read( ENTRY_WKSLEVEL.c_str(), &wks.m_level );
}


bool CConfiguration::SaveConfigDataset( const CWorkspaceDataset &data, std::string &errorMsg )
{
	assert__( this == data.m_config );

	bool bOk = true;
	std::string entry;
	int index = 0;
	for ( CObMap::const_iterator it = data.m_datasets.begin(); it != data.m_datasets.end(); it++ )
	{
		index++;
		CDataset* dataset = dynamic_cast<CDataset*>( it->second );
		if ( dataset == nullptr )
		{
			errorMsg += "ERROR in  CWorkspaceDataset::SaveConfigDataset\ndynamic_cast<CDataset*>(it->second) returns nullptr pointer"
				"\nList seems to contain objects other than those of the class CDataset";
			return false;
		}

		SetPath( "/" + GROUP_DATASETS );

		bOk &= Write( ENTRY_DSNAME + n2s<std::string >( index ), dataset->GetName().c_str() );

		dataset->SaveConfig( this );
	}

	return bOk;
}


bool CConfiguration::LoadConfigDataset( CWorkspaceDataset &data, std::string &errorMsg )
{
	assert__( this == data.m_config );

	SetPath( "/" + GROUP_DATASETS );

	std::string entry;
	std::string valueString;
	long i = 0;

	while ( StdGetNextEntry( this, entry, i ) )
	{
		valueString = Read( entry );
		data.InsertDataset( valueString );
	}

	int index = 0;
	for ( CObMap::iterator it = data.m_datasets.begin(); it != data.m_datasets.end(); it++ )
	{
		index++;
		CDataset* dataset = dynamic_cast< CDataset* >( it->second );
		assert__( dataset != nullptr );
		//{
		//	wxMessageBox( "ERROR in  CWorkspaceDataset::LoadConfigDataset\ndynamic_cast<CDataset*>(it->second) returns nullptr pointer"
		//		"\nList seems to contain objects other than those of the class CDataset",
		//		"Error",
		//		wxOK | wxCENTRE | wxICON_ERROR );
		//	return false;
		//}

		dataset->LoadConfig( this );
		if ( data.m_ctrlDatasetFiles )
		{
			std::vector< std::string > v;
			if ( !dataset->CtrlFiles(v) )
			{
				std::string s = Vector2String( v, "\n" );
				errorMsg += "Dataset '" + data.m_name + "':\n contains file '" + s + "' that doesn't exist\n";
				return false;
			}
			else
			{
				try
				{
					//Just to initialize 'product class' and 'product type'
					dataset->CheckFiles( true );
				}
				catch ( CException& e )
				{
					UNUSED( e );
				}
			}
		}
	}

	return true;
}


bool CConfiguration::SaveConfigOperation( const CWorkspaceOperation &op, std::string &errorMsg )
{
	assert__( this == op.m_config );

	bool bOk = true;
	std::string entry;

	int32_t index = 0;
	for ( CObMap::const_iterator it = op.m_operations.begin(); it != op.m_operations.end(); it++ )
	{
		index++;
		COperation* operation = dynamic_cast<COperation*>( it->second );
		if ( operation == nullptr )
		{
			errorMsg += "ERROR in  CWorkspaceOperation::SaveConfigOperation\ndynamic_cast<COperation*>(it->second) returns nullptr pointer"
				"\nList seems to contain objects other than those of the class COperation\n";

			return false;
		}

		SetPath( "/" + GROUP_OPERATIONS );

		bOk &= Write( ENTRY_OPNAME + n2s< std::string >( index ), operation->GetName().c_str() );

        operation->SaveConfig( this, &op );
	}

	return bOk;
}

bool CConfiguration::LoadConfigOperation( CWorkspaceOperation &op, std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceOperation *wkso )
{
	assert__( this == op.m_config );

	bool bOk = true;

	SetPath( "/" + GROUP_OPERATIONS );

	std::string entry;
	std::string valueString;
	long i = 0;

	do
	{
		bOk = StdGetNextEntry( this, entry, i );
		if ( bOk )
		{
			valueString = Read( entry );
			op.InsertOperation( valueString );
		}
	} while ( bOk );

	CObMap::iterator it;

	int32_t index = 0;

	for ( it = op.m_operations.begin(); it != op.m_operations.end(); it++ )
	{
		index++;
		COperation* operation = dynamic_cast<COperation*>( it->second );
		if ( operation == nullptr )
		{
			errorMsg += "ERROR in  CWorkspaceOperation::LoadConfigOperation\ndynamic_cast<COperation*>(it->second) returns nullptr pointer"
				"\nList seems to contain objects other than those of the class COperation";

			return false;
		}

		operation->LoadConfig( this, errorMsg, wks, wkso );
	}

	return true;
}

bool CConfiguration::SaveConfigDisplay( const CWorkspaceDisplay &disp, std::string &errorMsg, CWorkspaceDisplay *wksd )
{
	assert__( this == disp.m_config );

	bool bOk = true;
	int32_t index = 0;

	for ( CObMap::const_iterator it = disp.m_displays.begin(); it != disp.m_displays.end(); it++ )
	{
		index++;
		CDisplay* display = dynamic_cast<CDisplay*>( it->second );
		if ( display == nullptr )
		{
			errorMsg += 
				"ERROR in  CWorkspaceDisplay::m_displays\ndynamic_cast<CDisplay*>(it->second) returns nullptr pointer"
				"\nList seems to contain objects other than those of the class CDisplay";

			return false;
		}

		SetPath( "/" + GROUP_DISPLAY );

		bOk &= Write( ENTRY_DISPLAYNAME + n2s< std::string >( index ), display->GetName().c_str() );

		display->SaveConfig( this, wksd );
	}

	return bOk;
}

bool CConfiguration::LoadConfigDisplay( CWorkspaceDisplay &disp, std::string &errorMsg, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso )
{
	assert__( this == disp.m_config );

	bool bOk = true;

	SetPath( "/" + GROUP_DISPLAY );

	std::string entry;
	std::string valueString;
	long i = 0;
	do
	{
		bOk = StdGetNextEntry( disp.m_config, entry, i );
		if ( bOk )
		{
			valueString = Read( entry );
			disp.InsertDisplay( valueString );
		}
	} while ( bOk );

	int32_t index = 0;

	for ( CObMap::const_iterator it = disp.m_displays.begin(); it != disp.m_displays.end(); it++ )
	{
		index++;
		CDisplay* display = dynamic_cast<CDisplay*>( it->second );
		if ( display == nullptr )
		{
			errorMsg +=
				"ERROR in  CWorkspaceDisplay::LoadConfigOperation\ndynamic_cast<CDisplay*>(it->second) returns nullptr pointer"
				"\nList seems to contain objects other than those of the class CDisplay\n";

			return false;
		}

		display->LoadConfig( this, errorMsg, wksd, wkso );
	}

	return true;
}

bool CConfiguration::LoadConfigDesc( CFormula &f, const std::string& path )
{
	SetPath( "/" + path );

	f.m_description = Read( f.m_name, f.m_description );

	return true;
}

bool CConfiguration::SaveConfigDesc( const CFormula &f, const std::string& path )
{
	bool bOk = true;

	SetPath( "/" + path );

	bOk &= Write( f.m_name, f.GetDescription( true ).c_str() );	//bOk &= Write(ENTRY_COMMENT, m_comment);

	return true;
}

bool CConfiguration::SaveConfigPredefined( const CFormula &f, const std::string& pathSuff )
{
	bool bOk = true;
	std::string path = f.m_name;
	if ( pathSuff.empty() == false )
	{
		path += "_" + pathSuff;
	}
	SetPath( "/" + path );

	bOk &= Write( ENTRY_DEFINE, f.GetDescription( true ).c_str() );
	bOk &= Write( ENTRY_COMMENT, f.GetComment( true ).c_str() );
	bOk &= Write( ENTRY_UNIT, f.m_unit.GetText().c_str() );

	return true;
}

bool CConfiguration::LoadConfig( CFormula &f, std::string &errorMsg, const std::string& pathSuff )
{
	std::string valueString;

	std::string path = f.m_name;
	if ( pathSuff.empty() == false )
	{
		path += "_" + pathSuff;
	}

	SetPath( "/" + path );

	f.m_description = Read( ENTRY_DEFINE, f.m_description );
	f.m_comment = Read( ENTRY_COMMENT, f.m_comment );

	valueString = Read( ENTRY_UNIT, f.m_unit.GetText().c_str() );
	f.SetUnit( valueString, errorMsg, "" );

	valueString = Read( ENTRY_FIELDTYPE, f.GetTypeAsString() );
	if ( valueString.empty() )
	{
		f.m_type = CMapTypeField::eTypeOpAsField;
	}
	else
	{
		f.m_type = CMapTypeField::GetInstance().NameToId( valueString );
	}

	valueString = Read( ENTRY_DATATYPE, f.GetDataTypeAsString() );
	if ( valueString.empty() )
	{
		f.m_dataType = CMapTypeData::eTypeOpData;
	}
	else
	{
		f.m_dataType = CMapTypeData::GetInstance().NameToId( valueString );
	}

	f.m_title = Read( ENTRY_TITLE, f.m_title );

	valueString = Read( ENTRY_FILTER, f.GetFilterAsString() );
	if ( valueString.empty() )
	{
		f.m_filter = CMapTypeFilter::eFilterNone;
	}
	else
	{
		f.m_filter = CMapTypeFilter::GetInstance().NameToId( valueString );
	}
	if ( f.IsTimeDataType() )
	{
		valueString = Read( ENTRY_MINVALUE );
		f.SetMinValueFromDateString( valueString );

		valueString = Read( ENTRY_MAXVALUE );
		f.SetMaxValueFromDateString( valueString );
	}
	else
	{
		Read( ENTRY_MINVALUE, &f.m_minValue, CTools::m_defaultValueDOUBLE );
		Read( ENTRY_MAXVALUE, &f.m_maxValue, CTools::m_defaultValueDOUBLE );
	}

	// 3.3.1 note: wxWidgets asserts if value >= INT_MAX, but CTools::m_defaultValueINT32 is INT_MAX, so,
	// if value is not read and the default is used, we have a failed assertion. This was not changed, to
	// avoid unknown implications.

	Read( ENTRY_INTERVAL, &f.m_interval, CTools::m_defaultValueINT32 );
	f.m_step = Read( ENTRY_STEP, f.DEFAULT_STEP_GENERAL_ASSTRING );
	if ( f.m_step.empty() )
	{
		f.m_step = f.DEFAULT_STEP_GENERAL_ASSTRING;
	}

	Read( ENTRY_LOESSCUTOFF, &f.m_loessCutOff, CTools::m_defaultValueINT32 );

	valueString = Read( ENTRY_DATA_MODE );
	if ( valueString.empty() )
	{
		f.m_dataMode = -1;
	}
	else
	{
		f.m_dataMode = CMapDataMode::GetInstance().NameToId( valueString );
	}

	return true;
}

bool CConfiguration::SaveConfig( const CFormula &f, const std::string& pathSuff )
{
	bool bOk = true;
	std::string path = f.m_name;
	if ( pathSuff.empty() == false )
	{
		path += "_" + pathSuff;
	}
	SetPath( "/" + path );

	bOk &= Write( ENTRY_DEFINE, f.GetDescription( true ).c_str() );
	bOk &= Write( ENTRY_COMMENT, f.GetComment( true ).c_str() );
	bOk &= Write( ENTRY_UNIT, f.m_unit.GetText().c_str() );
	bOk &= Write( ENTRY_FIELDTYPE, f.GetTypeAsString().c_str() );
	bOk &= Write( ENTRY_DATATYPE, f.GetDataTypeAsString().c_str() );
	bOk &= Write( ENTRY_TITLE, f.m_title.c_str() );
	bOk &= Write( ENTRY_FILTER, f.GetFilterAsString().c_str() );
	if ( isDefaultValue( f.m_minValue ) == false )
	{
		if ( f.IsTimeDataType() )
		{
			bOk &= Write( ENTRY_MINVALUE, f.GetMinValueAsDateString().c_str() );
		}
		else
		{
			bOk &= Write( ENTRY_MINVALUE, CTools::Format( "%.15g", f.m_minValue ).c_str() );
		}
	}
	if ( isDefaultValue( f.m_maxValue ) == false )
	{
		if ( f.IsTimeDataType() )
		{
			bOk &= Write( ENTRY_MAXVALUE, f.GetMaxValueAsDateString().c_str() );
		}
		else
		{
			bOk &= Write( ENTRY_MAXVALUE, CTools::Format( "%.15g", f.m_maxValue ).c_str() );
		}
	}
	if ( isDefaultValue( f.m_interval ) == false )
	{
		bOk &= Write( ENTRY_INTERVAL, f.m_interval );
	}
	if ( f.m_step.empty() == false )
	{
		bOk &= Write( ENTRY_STEP, f.m_step.c_str() );
	}
	if ( isDefaultValue( f.m_loessCutOff ) == false )
	{
		bOk &= Write( ENTRY_LOESSCUTOFF, f.m_loessCutOff );
	}
	bOk &= Write( ENTRY_DATA_MODE, f.GetDataModeAsString().c_str() );

	return true;
}

bool CConfiguration::LoadConfig( COperation &op, std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceOperation *wkso )
{
	SetPath( "/" + op.m_name );

	std::string valueString;
	valueString = Read( ENTRY_DSNAME );
	op.m_dataset = op.FindDataset( valueString, wks );

	valueString = Read( ENTRY_TYPE, CMapTypeOp::GetInstance().IdToName( op.m_type ) );
	if ( valueString.empty() )
	{
		op.m_type = CMapTypeOp::eTypeOpYFX;
	}
	else
	{
		op.m_type = CMapTypeOp::GetInstance().NameToId( valueString );
	}

	valueString = Read( ENTRY_DATA_MODE, CMapDataMode::GetInstance().IdToName( op.m_dataMode ) );
	if ( valueString.empty() )
	{
		op.m_dataMode = CMapDataMode::GetInstance().GetDefault();
	}
	else
	{
		op.m_dataMode = CMapDataMode::GetInstance().NameToId( valueString );
	}


	op.m_record = Read( ENTRY_RECORDNAME );
	if ( op.m_record.empty() )
	{
		op.m_record = CProductNetCdf::m_virtualRecordName.c_str();
	}
	valueString = Read( ENTRY_OUTPUT );
	if ( valueString.empty() == false )
	{
		// Old Comment: Note that if the path to the output is in relative form,
		// SetOutput make it in absolute form based on workspace Operation path.
		op.SetOutput( valueString, wkso );
	}

	valueString = Read( ENTRY_EXPORT_ASCII_OUTPUT );
	if ( valueString.empty() == false )
	{
		// Old Comment: Note that if the path to the output is in relative form,
		// SetOutput make it in absolute form based on workspace Operation path.
		op.SetExportAsciiOutput( valueString, wkso );
	}
	else
	{
		op.InitExportAsciiOutput( wkso );
	}

	op.m_select->LoadConfigDesc( this, base_t::GetPath().ToStdString() );

	// Warning after formulas Load config conig path has changed
	op.m_formulas.LoadConfig( this, errorMsg, false, op.GetName() );

	op.m_formulas.InitFormulaDataMode( op.m_dataMode );

	return true;
}

bool CConfiguration::SaveConfig( const COperation &op, const CWorkspaceOperation *wkso )
{
	SetPath( "/" + op.m_name );

	bool bOk = true;
	if ( op.m_dataset != nullptr )
		bOk &= Write( ENTRY_DSNAME, op.m_dataset->GetName().c_str() ) && Write( ENTRY_RECORDNAME, op.GetRecord().c_str() );

	bOk &= Write( ENTRY_TYPE, CMapTypeOp::GetInstance().IdToName( op.m_type ).c_str() );

	// Data mode is no more for a operation, but for each formula (data expression)
	//bOk &= Write(ENTRY_DATA_MODE,
	//                     CMapDataMode::GetInstance().IdToName(m_dataMode));

	op.m_select->SaveConfigDesc( this, base_t::GetPath().ToStdString() );

    bOk &= Write( ENTRY_OUTPUT, op.GetOutputPathRelativeToWks( wkso ).c_str() );
    bOk &= Write( ENTRY_EXPORT_ASCII_OUTPUT, op.GetExportAsciiOutputPathRelativeToWks( wkso ).c_str() );

	// Warning after formulas Load config conig path has changed
	op.m_formulas.SaveConfig( this, false, op.GetName() );

	return bOk;
}

bool CConfiguration::LoadConfig( CMapDisplayData &data, std::string &errorMsg, CWorkspaceDisplay *wks, CWorkspaceOperation *wkso, const std::string& pathSuff )
{
	bool bOk = true;
	std::string path = GROUP_DISPLAY;
	if ( pathSuff.empty() == false )
	{
		path += "_" + pathSuff;
	}
	SetPath( "/" + path );

	std::string entry;
	std::string valueString;
	std::string displayDataName;
	long i = 0;
	do
	{
		bOk = StdGetNextEntry( this, entry, i );
		if ( bOk )
		{
			valueString = Read( entry );

			if ( pathSuff.empty() == false )
			{
				displayDataName = valueString.substr( 0, valueString.length() - pathSuff.length() - 1 );
				//displayDataName = valueString.Left( valueString.Length() - pathSuff.Length() - 1 );
			}
			else
			{
				displayDataName = valueString;
			}

			if ( dynamic_cast< CDisplayData* >( data.Exists( (const char *)displayDataName.c_str() ) ) != nullptr )
			{
				data.Erase( (const char *)displayDataName.c_str() );
			}
			data.Insert( (const char *)displayDataName.c_str(), new CDisplayData() );
		}
	} while ( bOk );


	int32_t index = 0;

	CStringMap renameKeyMap;
	for ( CMapDisplayData::iterator it = data.begin(); it != data.end(); it++ )
	{
		index++;
		CDisplayData* displayData = dynamic_cast<CDisplayData*>( it->second );
		if ( displayData == nullptr )
		{
			errorMsg +=
				"ERROR in  CMapDisplayData::LoadConfig\ndynamic_cast<CDisplayData*>(it->second) returns nullptr pointer"
				"\nList seems to contain objects other than those of the class CDisplayData";

			//wxMessageBox( "Error",
			//wxOK | wxCENTRE | wxICON_ERROR );
			return false;
		}

		displayData->LoadConfig( this, it->first + "_" + pathSuff, wks, wkso );

		// To maintain compatibility with Brat v1.x (display name doesn't contain 'display type' in v1.x)
		std::string displayDataKey = (const char *)displayData->GetDataKey().c_str();

		if ( it->first != displayDataKey )
		{
			renameKeyMap.Insert( it->first, displayDataKey );
		}
	}

	for ( CStringMap::const_iterator itStringMap = renameKeyMap.begin(); itStringMap != renameKeyMap.end(); itStringMap++ )
	{
		data.RenameKey( itStringMap->first, itStringMap->second );
	}

	return true;
}

bool CConfiguration::SaveConfig( const CMapDisplayData &data, CWorkspaceDisplay *wks, const std::string& pathSuff )
{
	int index = 0;
	for ( CMapDisplayData::const_iterator it = data.begin(); it != data.end(); it++ )
	{
		std::string path = GROUP_DISPLAY;
		if ( pathSuff.empty() == false )
		{
			path += "_" + pathSuff;
		}
		SetPath( "/" + path );

		CDisplayData* displayData = dynamic_cast<CDisplayData*>( it->second );
		if ( displayData != nullptr )
		{
			index++;
			std::string key = displayData->GetDataKey();
			if ( pathSuff.empty() == false )
			{
				key += "_" + pathSuff;
			}

			Write( ENTRY_DISPLAY_DATA + n2s<std::string>( index ), key.c_str() );

			displayData->SaveConfig( this, pathSuff, wks );
		}
	}
	return true;
}

bool CConfiguration::LoadConfig( CDisplay &d, std::string &errorMsg, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso )
{
	SetPath( "/" + d.m_name );

	std::string
    valueString = Read( ENTRY_TYPE, CMapTypeDisp::GetInstance().IdToName( d.m_type ) ).ToStdString();
	if ( valueString.empty() )
	{
		d.m_type = -1;
	}
	else
	{
		d.m_type = CMapTypeDisp::GetInstance().NameToId( valueString );
	}

	d.m_title = Read( ENTRY_TITLE );
	Read( ENTRY_ANIMATION, &d.m_withAnimation, false );
	d.m_projection = Read( ENTRY_PROJECTION, "3D" );

	Read( ENTRY_MINXVALUE, &d.m_minXValue, CTools::m_defaultValueDOUBLE );
	Read( ENTRY_MAXXVALUE, &d.m_maxXValue, CTools::m_defaultValueDOUBLE );

	Read( ENTRY_MINYVALUE, &d.m_minYValue, CTools::m_defaultValueDOUBLE );
	Read( ENTRY_MAXYVALUE, &d.m_maxYValue, CTools::m_defaultValueDOUBLE );

	valueString = Read( ENTRY_ZOOM );
	d.m_zoom.Set( valueString, CDisplay::m_zoomDelimiter );

	// the entry ENTRY_OUTPUT  is not used any more
	//  valueString = Read(ENTRY_OUTPUT);
	//  if (valueString.empty() == false)
	//  {
	//    SetOutput(valueString);
	//  }

	d.InitOutput( wksd );

	d.m_data.LoadConfig( this, errorMsg, wksd, wkso, d.GetName() );

	return true;
}



bool CConfiguration::SaveConfig( const CDisplay &d, CWorkspaceDisplay *wksd )
{
    bool bOk = true;

    SetPath( "/" + d.m_name );

    bOk &= Write( ENTRY_TYPE, CMapTypeDisp::GetInstance().IdToName( d.m_type ).c_str() );


    bOk &= Write( ENTRY_TITLE, d.GetTitle().c_str() );
    bOk &= Write( ENTRY_ANIMATION, d.GetWithAnimation() );
    bOk &= Write( ENTRY_PROJECTION, d.GetProjection().c_str() );

    if ( isDefaultValue( d.m_minXValue ) == false )
    {
        bOk &= Write( ENTRY_MINXVALUE, d.m_minXValue );
    }
    if ( isDefaultValue( d.m_maxXValue ) == false )
    {
        bOk &= Write( ENTRY_MAXXVALUE, d.m_maxXValue );
    }

    if ( isDefaultValue( d.m_minYValue ) == false )
    {
        bOk &= Write( ENTRY_MINYVALUE, d.m_minYValue );
    }
    if ( isDefaultValue( d.m_maxYValue ) == false )
    {
        bOk &= Write( ENTRY_MAXYVALUE, d.m_maxYValue );
    }

    std::string valueString = d.m_zoom.GetAsText( CDisplay::m_zoomDelimiter ).c_str();
    bOk &= Write( ENTRY_ZOOM, valueString.c_str() );

    // the entry ENTRY_OUTPUT  is not used any more
    //bOk &= Write(ENTRY_OUTPUT, GetOutputName());


    // Warning after formulas Load config conig path has changed
    d.m_data.SaveConfig( this, wksd, d.GetName() );

    return bOk;
}









//////////////////////////////////////////////////////////////////////////////////////////////////////////
//											CDataSet (see also CConfiguration)
//////////////////////////////////////////////////////////////////////////////////////////////////////////


void GetFiles( const CDataset &d, wxArrayString& array )
{
	CStringListToWxArray( *d.GetProductList(), array );
}
void GetFiles( const CDataset &d, wxListBox& array )
{
	for ( CProductList::const_iterator it = d.GetProductList()->begin(); it != d.GetProductList()->end(); it++ )
	{
		array.Append( it->c_str() );
	}
}

void GetDatasetNames( const CWorkspaceDataset *d, wxComboBox& combo )
{
	for ( CObMap::const_iterator it = d->GetDatasets()->begin(); it != d->GetDatasets()->end(); it++ )
	{
		//std::string value = it->first;
		combo.Append( ( it->first ).c_str() );
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//											CFormula and related
//////////////////////////////////////////////////////////////////////////////////////////////////////////


void NamesToComboBox( const CMapTypeFilter &map, wxComboBox& combo )
{
	for ( CMapTypeFilter::const_iterator it = map.begin(); it != map.end(); it++ )
	{
		uint32_t value = it->second;
		if ( !isDefaultValue( value ) )
		{
			//combo.Insert((it->first).c_str(), value);
			combo.Append( ( it->first ).c_str() );
		}
	}
}

void NamesToComboBox( const CMapTypeData &map, wxComboBox& combo, bool noData )	//noData = false 
{
	for ( CMapTypeData::const_iterator it = map.begin(); it != map.end(); it++ )
	{
		uint32_t value = it->second;
		if ( ( value == CMapTypeData::eTypeOpData ) && ( noData ) )
		{
			continue;
		}
		if ( !isDefaultValue( value ) )
		{
			//combo.Insert((it->first).c_str(), value);
			combo.Append( ( it->first ).c_str() );
		}
	}
}

void NamesToComboBox( const CMapTypeOp &map, wxComboBox& combo )
{
	for ( CMapTypeOp::const_iterator it = map.begin(); it != map.end(); it++ )
	{
		uint32_t value = it->second;
		if ( !isDefaultValue( value ) )
		{
			//combo.Insert((it->first).c_str(), value);
			combo.Append( ( it->first ).c_str() );
		}
	}
}

void NamesToComboBox( const CMapDataMode &map, wxComboBox& combo )
{
	for ( CMapDataMode::const_iterator it = map.begin(); it != map.end(); it++ )
	{
		uint32_t value = it->second;
		if ( !isDefaultValue( value ) )
		{
			//combo.Insert((it->first).c_str(), value);
			combo.Append( ( it->first ).c_str() );
		}
	}
}

void NamesToComboBox( const CMapTypeField &map, wxComboBox& combo )
{
	for ( CMapTypeField::const_iterator it = map.begin(); it != map.end(); it++ )
	{
		uint32_t value = it->second;
		if ( !isDefaultValue( value ) )
		{
			//combo.Insert((it->first).c_str(), value);
			combo.Append( ( it->first ).c_str() );
		}
	}
}

void NamesToComboBox( const CMapFormula &map, wxComboBox& combo )
{
	for ( CMapFormula::const_iterator it = map.begin(); it != map.end(); it++ )
	{
		CFormula* value = dynamic_cast<CFormula*>( it->second );
		if ( value != nullptr )
		{
			combo.Append( value->GetName() );
		}
	}
}




//////////////////////////////////////////////////////////////////////////////////////////////////////////
//											CWorkspace and related
//////////////////////////////////////////////////////////////////////////////////////////////////////////


void GetFormulaNames( const CWorkspaceFormula &wks, wxComboBox& combo, bool getPredefined, bool getUser )	//bool getPredefined = true, bool getUser = true 
{
	for ( CMapFormula::const_iterator it = wks.GetFormulas()->begin(); it != wks.GetFormulas()->end(); it++ )
	{
		const CFormula* formula = wks.GetFormula( it );
		if ( formula != nullptr )
		{
			if ( ( getPredefined && formula->IsPredefined() ) || ( getUser && !formula->IsPredefined() ) )
			{
				combo.Append( ( it->first ).c_str() );
			}
		}
	}
}
void GetFormulaNames( const CWorkspaceFormula &wks, wxListBox& lb, bool getPredefined, bool getUser )		//bool getPredefined = true, bool getUser = true
{
	for ( CMapFormula::const_iterator it = wks.GetFormulas()->begin(); it != wks.GetFormulas()->end(); it++ )
	{
		const CFormula* formula = wks.GetFormula( it );
		if ( formula != nullptr )
		{
			if ( ( getPredefined && formula->IsPredefined() ) || ( getUser && !formula->IsPredefined() ) )
			{
				lb.Append( ( it->first ).c_str() );
			}
		}
	}
}

void GetOperationNames( const CWorkspaceOperation &wks, wxComboBox& combo )
{
	for ( CObMap::const_iterator it = wks.GetOperations()->begin(); it != wks.GetOperations()->end(); it++ )
	{
		//std::string value = it->first;
		combo.Append( ( it->first ).c_str() );
	}
}

void GetDisplayNames( const CWorkspaceDisplay &wks, wxComboBox& combo )
{
	for ( CObMap::const_iterator it = wks.GetDisplays()->begin(); it != wks.GetDisplays()->end(); it++ )
	{
		combo.Append( it->first.c_str() );
	}
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////
//											COperation and related
//////////////////////////////////////////////////////////////////////////////////////////////////////////



void GetFormulaNames( const COperation &op, wxComboBox& combo )
{
	for ( CMapFormula::const_iterator it = op.GetFormulas()->begin(); it != op.GetFormulas()->end(); it++ )
	{
		combo.Append( ( it->first ).c_str() );
	}
}







//////////////////////////////////////////////////////////////////////////////////////////////////////////
//											Extract File Name
//////////////////////////////////////////////////////////////////////////////////////////////////////////


std::string GetFileName( const std::string &path )
{
	return wxFileName( path ).GetName().ToStdString();
}

void SetFileExtension( std::string &path, const std::string &extension )
{
	wxFileName file( path );
	file.SetExt( extension );
	path = file.GetFullPath().ToStdString();
}

std::string NormalizedAbsolutePath( const std::string &path, const std::string &dir )	//1. simply adding path to dir; 2. adding file from path (absolute or relative) to dir
{
	wxFileName file( path );
	file.Normalize( wxPATH_NORM_ALL, dir );
	std::string new_path = file.GetFullPath().ToStdString();
	return new_path;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////
//											CDisplay related
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void NamesToComboBox( const CMapTypeDisp &disp, wxComboBox& combo )
{
	for ( CMapTypeDisp::const_iterator it = disp.begin(); it != disp.end(); it++ )
	{
		uint32_t value = it->second;
		if ( !isDefaultValue( value ) )
		{
			//combo.Insert((it->first).c_str(), value);
			combo.Append( ( it->first ).c_str() );
		}
	}
}

void NamesToComboBox( const CMapDisplayData &data, wxComboBox& combo )
{
	for ( CMapDisplayData::const_iterator it = data.begin(); it != data.end(); it++ )
	{
		CDisplayData* value = dynamic_cast<CDisplayData*>( it->second );
		if ( value != nullptr )
		{
			combo.Append( value->GetDataKey() );
		}
	}
}
