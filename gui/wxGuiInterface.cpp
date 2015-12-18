
#if !defined(BRAT_V3)		//help IntelliSense
#define BRAT_V3
#endif

#include "BratGui.h"
#include "new-gui/QtInterface.h"
#include "wxGuiInterface.h"


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

bool CConfiguration::Flush( bool bCurrentOnly )		//bCurrentOnly = false
{
	return base_t::Flush( bCurrentOnly );
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



//////////////////////////////////////////////////////////////////////////////////////////////////////////
//											CDataSet (see also CConfiguration)
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "new-gui/brat/Workspaces/Dataset.h"

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
