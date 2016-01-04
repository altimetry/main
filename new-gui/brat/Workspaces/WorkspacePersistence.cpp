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
#include "new-gui/brat/stdafx.h"

#include "Workspace.h"
#include "Dataset.h"
#include "WorkspacePersistence.h"




bool CWorkspaceSettings::LoadCommonConfig( CWorkspace &wks )
{
	assert__( this == wks.m_config );

	return
		( mSettings.status() == QSettings::NoError )	//just in case check; LoadCommonConfig is typically called before any other workspaces persistence related
		&&
        ReadSection( GROUP_GENERAL_READ,

			k_v( ENTRY_WKSNAME, &wks.m_name ),
			k_v( ENTRY_WKSLEVEL, &wks.m_level )
		);
}



bool CWorkspaceSettings::LoadConfigDataset( CWorkspaceDataset &data, std::string &errorMsg )
{
	assert__( this == data.m_config );

	{
		CSection section( mSettings, GROUP_DATASETS );
		auto const keys = section.Keys();
		for ( auto const &key : keys )
		{
			auto value = ReadValue( section, q2t< std::string >( key ) );
			data.InsertDataset( value );
		};
	}
	if ( Status() != QSettings::NoError )
	{
		errorMsg += "Error reading datasets from configuration file " + mFilePath;
		return false;
	}

	for ( CObMap::iterator it = data.m_datasets.begin(); it != data.m_datasets.end(); it++ )
	{
		CDataset* dataset = dynamic_cast< CDataset* >( it->second );		assert__( dataset != nullptr );	//v3: an error msg box here
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
			else {
				try
				{
					dataset->GetProductList()->CheckFiles( true );	//Just to initialize 'product class' and 'product type'
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



bool CWorkspaceSettings::LoadConfig( CDataset *d )
{
	{
		CSection section( mSettings, d->GetName() );
		auto const keys = section.Keys();
		CStringArray findStrings;
		for ( auto const &key : keys )
		{
			std::string entry = q2a( key );
			std::string valueString = ReadValue( section, entry );

			// Find ENTRY_FILE entries (dataset files entries)
			findStrings.RemoveAll();
			CTools::Find( entry, ENTRY_FILE_REGEX, findStrings );
			if ( findStrings.size() > 0 )
			{
				d->GetProductList()->Insert( valueString );
				continue;
			}

			// Find specific unit entries
			findStrings.RemoveAll();
			CTools::Find( entry, ENTRY_UNIT_REGEX, findStrings );
			if ( findStrings.size() > 0 )
			{
				d->GetFieldSpecificUnits()->Insert( findStrings.at( 0 ), valueString );			//m_fieldSpecificUnit.Dump(*CTrace::GetDumpContext());
				continue;
			}
		}
	}

	return Status() == QSettings::NoError;
}


bool CWorkspaceSettings::LoadConfig( CMapFormula &mapf, std::string &errorMsg, bool predefined, const std::string& pathSuff )
{
	std::string path = GROUP_FORMULAS;
	if ( !pathSuff.empty() )
		path += "_" + pathSuff;

	{
		CSection section( mSettings, path );
		auto const keys = section.Keys();
		for ( auto const &key : keys )
		{
			std::string valueString = ReadValue( section, q2t< std::string >( key ) );
			std::string formulaName = 
				pathSuff.empty() ? 
				valueString :
				valueString.substr( 0, valueString.length() - pathSuff.length() - 1 );	//formulaName = valueString.Left(valueString.Length() - pathSuff.Length() - 1);

			CFormula* value = dynamic_cast< CFormula* >( mapf.Exists( formulaName ) );
			if ( value != nullptr )
				mapf.Erase( formulaName );

			mapf.Insert( formulaName, new CFormula( formulaName, predefined ) );
		}
	}


	for ( CMapFormula::iterator it = mapf.begin(); it != mapf.end(); it++ )
	{
		CFormula* formula = dynamic_cast< CFormula* >( it->second );
		if ( formula == nullptr )
		{
			//an error msg box was displayed here
			errorMsg +=
				"ERROR in  CMapFormula::LoadConfig\ndynamic_cast<CFormula*>(it->second) returns nullptr pointer"
				"\nList seems to contain objects other than those of the class CFormula.\n";
			
			return false;
		}

		formula->LoadConfig( this, errorMsg, pathSuff );
	}

	return true;
}

bool CWorkspaceSettings::LoadConfig( CFormula &f, std::string &errorMsg, const std::string& pathSuff )
{			
	std::string path = f.m_name;
	if ( !pathSuff.empty() )
		path += "_" + pathSuff;

	//SetPath( "/" + path );
	//f.m_description = Read( ENTRY_DEFINE, f.m_description );
	//f.m_comment = Read( ENTRY_COMMENT, f.m_comment );
	//valueString = Read( ENTRY_UNIT, f.m_unit.GetText().c_str() );
	//valueString = Read( ENTRY_FIELDTYPE, f.GetTypeAsString() );
	//valueString = Read( ENTRY_DATATYPE, f.GetDataTypeAsString() );
	//f.m_title = Read( ENTRY_TITLE, f.m_title );
	//valueString = Read( ENTRY_FILTER, f.GetFilterAsString() );

	std::string unit, type, data_type, filter;

	ReadSection( path,

		k_v( ENTRY_DEFINE,		&f.m_description	),
		k_v( ENTRY_COMMENT,		&f.m_comment		),
		k_v( ENTRY_UNIT,		&unit,				f.m_unit.GetText() ),
		k_v( ENTRY_FIELDTYPE,	&type,				f.GetTypeAsString() ),
		k_v( ENTRY_DATATYPE,	&data_type,			f.GetDataTypeAsString() ),
		k_v( ENTRY_TITLE,		&f.m_title			),
		k_v( ENTRY_FILTER,		&filter,			f.GetFilterAsString() )
	);
				
	//std::string valueString;

	f.SetUnit( unit, errorMsg, "" );
	f.m_type = type.empty() ? CMapTypeField::eTypeOpAsField : CMapTypeField::GetInstance().NameToId( type );
	f.m_dataType = data_type.empty()  ? CMapTypeData::eTypeOpData :CMapTypeData::GetInstance().NameToId( data_type );
	f.m_filter = filter.empty() ? CMapTypeFilter::eFilterNone : CMapTypeFilter::GetInstance().NameToId( filter );
	
	if ( f.IsTimeDataType() )
	{
		//valueString = Read( ENTRY_MINVALUE );
		//f.SetMinValueFromDateString( valueString );
		//valueString = Read( ENTRY_MAXVALUE );
		//f.SetMaxValueFromDateString( valueString );

		std::string min_value, max_value;

		ReadSection( path,

			k_v( ENTRY_MINVALUE,	&min_value			 ),
			k_v( ENTRY_MAXVALUE,	&max_value			 )
		);

		f.SetMinValueFromDateString( min_value );
		f.SetMaxValueFromDateString( max_value );
	}
	else
	{
		//Read( ENTRY_MINVALUE, &f.m_minValue, defaultValue<double>() );
		//Read( ENTRY_MAXVALUE, &f.m_maxValue, defaultValue<double>() );
		ReadSection( path,

			k_v( ENTRY_MINVALUE,	&f.m_minValue, defaultValue< double >() ),
			k_v( ENTRY_MAXVALUE,	&f.m_maxValue, defaultValue< double >() )
		);
	}

	//Read( ENTRY_INTERVAL, &f.m_interval, defaultValue< int32_t >() );
	//f.m_step = Read( ENTRY_STEP, f.DEFAULT_STEP_GENERAL_ASSTRING );
	//Read( ENTRY_LOESSCUTOFF, &f.m_loessCutOff, defaultValue< int32_t >() );

	//valueString = Read( ENTRY_DATA_MODE );

	// 3.3.1 note: wxWidgets asserts if value >= INT_MAX, but CTools::m_defaultValueINT32 is INT_MAX, so,
	// if value is not read and the default is used, we have a failed assertion. This was not changed, to
	// avoid unknown implications.

	std::string data_mode;

	ReadSection( path,

		k_v( ENTRY_INTERVAL,	&f.m_interval,		defaultValue< int32_t >() ),
		k_v( ENTRY_STEP,		&f.m_step,			f.DEFAULT_STEP_GENERAL_ASSTRING ),
		k_v( ENTRY_LOESSCUTOFF, &f.m_loessCutOff,	defaultValue< int32_t >() ),
		k_v( ENTRY_DATA_MODE,	&data_mode			)
	);

	if ( f.m_step.empty() )
		f.m_step = f.DEFAULT_STEP_GENERAL_ASSTRING;

	f.m_dataMode = data_mode.empty() ? -1 : CMapDataMode::GetInstance().NameToId( data_mode );

	return Status() == QSettings::NoError;
}
