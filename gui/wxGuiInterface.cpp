
#if !defined(BRAT_V3)		//help IntelliSense
#define BRAT_V3
#endif

#include "BratGui.h"
#include "new-gui/QtInterface.h"
#include "wxGuiInterface.h"
#include "new-gui/brat/Workspaces/Dataset.h"
#include "new-gui/brat/Display/MapTypeDisp.h"


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

const std::string CConfiguration::GetConfigPath() const
{
	return base_t::GetPath().ToStdString();
}
void CConfiguration::SetPath( const std::string& strPath )
{
	base_t::SetPath( strPath );
}
bool CConfiguration::Write( const std::string& key, const std::string& value )
{
	return base_t::Write( key, wxString( value ) );
}
bool CConfiguration::Write( const std::string& key, const int& value )
{
	return base_t::Write( key, value );
}
size_t CConfiguration::GetNumberOfEntries()
{
	return base_t::GetNumberOfEntries( false );
}
bool CConfiguration::GetNextEntry( std::string& str, long& lIndex )
{
	wxString wstr( str );
	bool result = base_t::GetNextEntry( wstr, lIndex );
	str = wstr.ToStdString();
	return result;
}
std::string CConfiguration::Read( const std::string& str ) const
{
	return base_t::Read( str ).ToStdString();
}
bool CConfiguration::Read( const std::string& str, std::string *value ) const
{
	wxString wvalue( *value );
	bool result = base_t::Read( str, &wvalue );
	*value = wvalue.ToStdString();
	return result;
}
std::string CConfiguration::Read( const std::string& str, const std::string &value ) const
{
	wxString wvalue( value );
	wxString wstr( str );
	auto result = base_t::Read( wstr, wvalue );
	return result.ToStdString();
}
bool CConfiguration::Read( const std::string& str, int *value ) const
{
	wxString wstr( str );
	return base_t::Read( wstr, value );
}

bool CConfiguration::Read( const std::string& str, double *value, double defValue ) const
{
	wxString wstr( str );
	return base_t::Read( wstr, value, defValue );
}

bool CConfiguration::Read( const std::string& str, int32_t *value, double defValue ) const
{
	wxString wstr( str );
	return base_t::Read( wstr, value, defValue );
}

bool CConfiguration::Read( const std::string& str, bool *value, bool defValue ) const
{
	wxString wstr( str );
	return base_t::Read( wstr, value, defValue );
}

bool CConfiguration::Flush( bool bCurrentOnly )		//bCurrentOnly = false
{
	return base_t::Flush( bCurrentOnly );
}

bool CConfiguration::DeleteAll()
{
	return base_t::DeleteAll();
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
	while ( GetNextEntry( entry, i ) )
	{
		// Finds ENTRY_FILE entries (dataset files entries)
		findStrings.RemoveAll();
		CTools::Find( entry, ENTRY_FILE_REGEX, findStrings );

		if ( findStrings.size() > 0 )
		{
			valueString = Read( entry );
			d->GetProductList()->Insert( valueString );
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

	std::string valueString = Read( ENTRY_TYPE, CMapTypeDisp::GetInstance().IdToName( data.m_type ) );
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
	data.m_operation = CDisplay::FindOperation( valueString, wkso );

	Read( ENTRY_GROUP, &data.m_group, 1 );
	Read( ENTRY_CONTOUR, &data.m_withContour, false );
	Read( ENTRY_SOLID_COLOR, &data.m_withSolidColor, true );

	Read( ENTRY_EAST_COMPONENT, &data.m_eastcomponent, false );
	Read( ENTRY_NORTH_COMPONENT, &data.m_northcomponent, false );

	Read( ENTRY_INVERT_XYAXES, &data.m_invertXYAxes, false );

	Read( ENTRY_MINVALUE, &data.m_minValue, CTools::m_defaultValueDOUBLE );
	Read( ENTRY_MAXVALUE, &data.m_maxValue, CTools::m_defaultValueDOUBLE );

	//config->Read(ENTRY_COLOR_PALETTE, &m_colorPalette, PALETTE_AEROSOL);
	Read( ENTRY_COLOR_PALETTE, &data.m_colorPalette );

	Read( ENTRY_X_AXIS, &data.m_xAxis );

	//config->Read(ENTRY_X_AXIS_TEXT, &m_xAxisText);

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

	bOk &= Write( ENTRY_TYPE,
		CMapTypeDisp::GetInstance().IdToName( data.m_type ) );


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

		bOk &= Write( ENTRY_COLOR_PALETTE, paletteToWrite );
	}

	if ( data.m_xAxis.empty() == false )
	{
		Write( ENTRY_X_AXIS, data.m_xAxis );
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
		bOk = GetNextEntry( entry, i );
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

			Write( ENTRY_FORMULA + n2s<std::string>( index ), formulaName );

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
	bOk &= Write( ENTRY_WKSNAME, wks.m_name );
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
		Read( ENTRY_WKSNAME, &wks.m_name ) &&
		Read( ENTRY_WKSLEVEL, &wks.m_level );
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

	while ( GetNextEntry( entry, i ) )
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
					dataset->GetProductList()->CheckFiles( true );
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

		bOk &= Write( ENTRY_OPNAME + n2s< std::string >( index ), operation->GetName() );

		operation->SaveConfig( this );
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
		bOk = GetNextEntry( entry, i );
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

	std::string entry;
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

		bOk &= Write( ENTRY_DISPLAYNAME + n2s< std::string >( index ), display->GetName() );

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
		bOk = disp.m_config->GetNextEntry( entry, i );
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

















//////////////////////////////////////////////////////////////////////////////////////////////////////////
//											CDataSet (see also CConfiguration)
//////////////////////////////////////////////////////////////////////////////////////////////////////////


void GetFiles( const CDataset &d, wxArrayString& array )
{
	CBratGuiApp::CStringListToWxArray( *d.GetProductList(), array );
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

std::string NormalizedPath( const std::string &path, const std::string &dir )	//1. simply adding path to dir; 2. adding file from path (absolute or relative) to dir
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
