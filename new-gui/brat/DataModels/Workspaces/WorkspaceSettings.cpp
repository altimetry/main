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

#include "libbrathl/ProductNetCdf.h"

#include "../MapTypeDisp.h"
#include "../Filters/BratFilters.h"
#include "Workspace.h"
#include "Dataset.h"
#include "Operation.h"
#include "Display.h"
#include "Function.h"
#include "WorkspaceSettings.h"
#include "BratLogger.h"



//static 
const CApplicationPaths *CWorkspaceSettings::smBratPaths = nullptr;





std::string CWorkspaceSettings::Absolute2PortableDatasetPath( const std::string &path ) const
{
	assert__( smBratPaths );

	if ( smBratPaths->UsePortablePaths() && IsSubDirectory( mDir, path ) )
		return GetRelativePath( mDir, path );

	return smBratPaths->Absolute2PortableDataPath( path );
}


std::string CWorkspaceSettings::Portable2AbsoluteDatasetPath( const std::string &path ) const
{
	assert__( smBratPaths );

	if ( IsPortableDataPath( path ) )
		return smBratPaths->Portable2AbsoluteDataPath( path );

	return GetAbsolutePath( mDir, path );		//returns path, if path is absolute
}



bool CWorkspaceSettings::SaveCommonConfig( const CWorkspace &wks, bool flush )
{
	assert__( this == wks.m_config );

	bool bOk = 
		WriteVersionSignature()
		&&
        WriteSection( GROUP_GENERAL_WRITE,

			k_v( ENTRY_WKSNAME, wks.m_name ),
			k_v( ENTRY_WKSLEVEL, wks.m_level )
		);

	if ( flush )
		Sync();

	return bOk;
}
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



bool CWorkspaceSettings::SaveConfigDataset( const CWorkspaceDataset &data, std::string &error_msg )
{
	assert__( this == data.m_config );

	int index = 0;
	{
		for ( CObMap::const_iterator it = data.m_datasets.begin(); it != data.m_datasets.end(); it++ )
		{
			index++;
			CDataset* dataset = dynamic_cast< CDataset* >( it->second );
			if ( dataset == nullptr )
			{
				error_msg += "ERROR in  CWorkspaceDataset::SaveConfigDataset\ndynamic_cast<CDataset*>(it->second) returns nullptr pointer"
					"\nList seems to contain objects other than those of the class CDataset";
				return false;
			}

			WriteSection( GROUP_DATASETS, 
				
				k_v( ENTRY_DSNAME + n2s<std::string>( index ), dataset->GetName() )
			);

			if ( !dataset->SaveConfig( this ) )
				return false;
		}
	}

	return Status() == QSettings::NoError;
}
bool CWorkspaceSettings::LoadConfigDataset( CWorkspaceDataset &data, std::string &error_msg )
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
		error_msg += "Error reading datasets from configuration file " + mFilePath;
		return false;
	}

	for ( CObMap::iterator it = data.m_datasets.begin(); it != data.m_datasets.end(); it++ )
	{
		CDataset* dataset = dynamic_cast< CDataset* >( it->second );		assert__( dataset != nullptr );	//v3: an error msg box here

		dataset->LoadConfig( this );
		if ( data.m_ctrlDatasetFiles )
		{
			std::vector< std::string > v;
			if ( !dataset->CtrlFiles(v) )
			{
				std::string s = Vector2String( v, "\n" );
				error_msg += "Dataset '" + data.m_name + "':\n contains file '" + s + "' that doesn't exist\n";
				return false;
			}
			else {
				try
				{
					dataset->CheckFiles( true );	//Just to initialize 'product class' and 'product type'
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


bool CWorkspaceSettings::SaveConfig( const CDataset *d )
{
	{
		// SaveConfig( CConfiguration *config, const std::string& entry )

		CSection section( mSettings, d->GetName() );
		int index = 0;
		for ( CProductList::const_iterator it = d->GetProductList()->begin(); it != d->GetProductList()->end(); it++ )
		{
			index++;
			WriteValue( section, ENTRY_FILE + n2s<std::string>( index ), Absolute2PortableDatasetPath( *it ) );		//v4 Absolute2PortableDataPath( *it )
		}

		//SaveConfigSpecificUnit( CConfiguration *config, const std::string& entry )

		for ( CStringMap::const_iterator itMap = d->GetFieldSpecificUnits()->begin(); itMap != d->GetFieldSpecificUnits()->end(); itMap++ )
		{
			WriteValue( section, ENTRY_UNIT + "_" + itMap->first, itMap->second );
		}
	}
	return Status() == QSettings::NoError;
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
			std::string value_string = ReadValue( section, entry );			
			value_string = Portable2AbsoluteDatasetPath( value_string );

			// Find ENTRY_FILE entries (dataset files entries)
			findStrings.RemoveAll();
			CTools::Find( entry, ENTRY_FILE_REGEX, findStrings );
			if ( findStrings.size() > 0 )
			{
				d->InsertProduct( value_string );
				continue;
			}

			// Find specific unit entries
			findStrings.RemoveAll();
			CTools::Find( entry, ENTRY_UNIT_REGEX, findStrings );
			if ( findStrings.size() > 0 )
			{
				d->GetFieldSpecificUnits()->Insert( findStrings.at( 0 ), value_string );			//m_fieldSpecificUnit.Dump(*CTrace::GetDumpContext());
				continue;
			}
		}
	}

	return Status() == QSettings::NoError;
}


bool CWorkspaceSettings::SaveConfig( const CMapFormula &mapf, bool predefined, const std::string& pathSuff )
{
	std::string path = GROUP_FORMULAS;
	if ( !pathSuff.empty() )
		path += "_" + pathSuff;

	{
		int index = 0;
		for ( CMapFormula::const_iterator it = mapf.begin(); it != mapf.end(); it++ )
		{
			const CFormula* formula = dynamic_cast< const CFormula*>( it->second );
			if ( formula != nullptr )
			{
				if ( formula->IsPredefined() != predefined )
					continue;

				index++;
				std::string formulaName = formula->GetName();
				if ( !pathSuff.empty() )
					formulaName += "_" + pathSuff;

				WriteSection( path, 
					
					k_v( ENTRY_FORMULA + n2s<std::string>( index ), formulaName )
				);

				if ( !formula->SaveConfig( this, pathSuff ) )
					return false;
			}
		}
	}
	return Status() == QSettings::NoError;
}

bool CWorkspaceSettings::LoadConfig( CMapFormula &mapf, std::string &error_msg, bool predefined, const std::string& pathSuff )
{
	std::string path = GROUP_FORMULAS;
	if ( !pathSuff.empty() )
		path += "_" + pathSuff;

	{
		CSection section( mSettings, path );
		auto const keys = section.Keys();
		for ( auto const &key : keys )
		{
			std::string value_string = ReadValue( section, q2t< std::string >( key ) );
			std::string formulaName = 
				pathSuff.empty() ? 
				value_string :
				value_string.substr( 0, value_string.length() - pathSuff.length() - 1 );	//formulaName = value_string.Left(value_string.Length() - pathSuff.Length() - 1);

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
			error_msg +=
				"ERROR in  CMapFormula::LoadConfig\ndynamic_cast<CFormula*>(it->second) returns nullptr pointer"
				"\nList seems to contain objects other than those of the class CFormula.\n";
			
			return false;
		}

		if ( !formula->LoadConfig( this, error_msg, pathSuff ) )
			return false;
	}

	return true;
}


bool CWorkspaceSettings::SaveConfigPredefined( const CFormula &f, const std::string& pathSuff )
{
	std::string path = f.m_name;
	if ( !pathSuff.empty() )
		path += "_" + pathSuff;

    WriteSection( path,

		k_v( ENTRY_DEFINE, f.GetDescription( true ) ),
		k_v( ENTRY_COMMENT, f.GetComment( true ) ),
		k_v( ENTRY_UNIT, f.m_unit.GetText() )
	);

	return Status() == QSettings::NoError;
}
bool CWorkspaceSettings::SaveConfig( const CFormula &f, const std::string& pathSuff )
{
	std::string path = f.m_name;
	if ( !pathSuff.empty() )
		path += "_" + pathSuff;

    WriteSection( path,

		k_v( ENTRY_DEFINE,			f.GetDescription( true ) ),
		k_v( ENTRY_COMMENT,			f.GetComment( true ) ),
		k_v( ENTRY_UNIT,			f.m_unit.GetText() ),
		k_v( ENTRY_FIELDTYPE,		f.GetTypeAsString() ),
		k_v( ENTRY_DATATYPE,		f.GetDataTypeAsString() ),
		k_v( ENTRY_TITLE,			f.m_title ),
		k_v( ENTRY_FILTER,			f.GetFilterAsString() ),
		k_v( ENTRY_NORTH_COMPONENT,	f.IsNorthComponent() ),
		k_v( ENTRY_EAST_COMPONENT,	f.IsEastComponent() )
	);

	{
		CSection section( mSettings, path );

		if ( !isDefaultValue( f.m_minValue ) )
		{
			WriteValue( section, ENTRY_MINVALUE, 
				f.IsTimeDataType() ? 
				f.GetMinValueAsDateString() : 
				CTools::Format( "%.15g", f.m_minValue ) );
		}

		if ( !isDefaultValue( f.m_maxValue ) )
		{
			WriteValue( section, ENTRY_MAXVALUE, 
				f.IsTimeDataType() ?
				f.GetMaxValueAsDateString() :
				CTools::Format( "%.15g", f.m_maxValue ) );
		}
		if ( !isDefaultValue( f.m_interval ) )
		{
			WriteValue( section, ENTRY_INTERVAL, f.m_interval );
		}
		if ( !f.m_step.empty() )
		{
			WriteValue( section, ENTRY_STEP, f.m_step );
		}
		if ( !isDefaultValue( f.m_loessCutOff ) )
		{
			WriteValue( section, ENTRY_LOESSCUTOFF, f.m_loessCutOff );
		}
		WriteValue( section, ENTRY_DATA_MODE, f.GetDataModeAsString() );
	}

	return Status() == QSettings::NoError;
}
bool CWorkspaceSettings::LoadConfig( CFormula &f, std::string &error_msg, const std::string& pathSuff )
{			
	std::string path = f.m_name;
	if ( !pathSuff.empty() )
		path += "_" + pathSuff;

	std::string unit, type, data_type, filter;

	ReadSection( path,

		k_v( ENTRY_DEFINE,			&f.m_description	),
		k_v( ENTRY_COMMENT,			&f.m_comment		),
		k_v( ENTRY_UNIT,			&unit,				f.m_unit.GetText() ),
		k_v( ENTRY_FIELDTYPE,		&type,				f.GetTypeAsString() ),
		k_v( ENTRY_DATATYPE,		&data_type,			f.GetDataTypeAsString() ),
		k_v( ENTRY_TITLE,			&f.m_title			),
		k_v( ENTRY_FILTER,			&filter,			f.GetFilterAsString() ),
		k_v( ENTRY_NORTH_COMPONENT,	&f.mNorthComponent, false ),
		k_v( ENTRY_EAST_COMPONENT,	&f.mEastComponent,	false )
	);
				
	f.SetUnit( unit, error_msg, "" );
	f.m_type = type.empty() ? CMapTypeField::eTypeOpAsField : CMapTypeField::GetInstance().NameToId( type );
	f.m_dataType = data_type.empty()  ? CMapTypeData::eTypeOpData :CMapTypeData::GetInstance().NameToId( data_type );
	f.m_filter = filter.empty() ? CMapTypeFilter::eFilterNone : CMapTypeFilter::GetInstance().NameToId( filter );
	
	if ( f.IsTimeDataType() )
	{
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
		ReadSection( path,

			k_v( ENTRY_MINVALUE,	&f.m_minValue, defaultValue< double >() ),
			k_v( ENTRY_MAXVALUE,	&f.m_maxValue, defaultValue< double >() )
		);
	}

	//Read( ENTRY_INTERVAL, &f.m_interval, defaultValue< int32_t >() );
	//f.m_step = Read( ENTRY_STEP, f.DEFAULT_STEP_GENERAL_ASSTRING );
	//Read( ENTRY_LOESSCUTOFF, &f.m_loessCutOff, defaultValue< int32_t >() );

	//value_string = Read( ENTRY_DATA_MODE );

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


bool CWorkspaceSettings::SaveConfigOperation( const CWorkspaceOperation &op, std::string &error_msg )
{
	assert__( this == op.m_config );

	{
		int32_t index = 0;
		for ( CObMap::const_iterator it = op.m_operations.begin(); it != op.m_operations.end(); it++ )
		{
			index++;
			const COperation* operation = dynamic_cast< const COperation* >( it->second );
			if ( operation == nullptr )
			{
				error_msg += "ERROR in  CWorkspaceOperation::SaveConfigOperation\ndynamic_cast<COperation*>(it->second) returns nullptr pointer"
					"\nList seems to contain objects other than those of the class COperation\n";

				return false;
			}

			WriteSection( GROUP_OPERATIONS,

				k_v( ENTRY_OPNAME + n2s< std::string >( index ), operation->GetName() )
			);

			if ( !operation->SaveConfig( this, &op ) )
				return false;
		}
	}
	return Status() == QSettings::NoError;
}
bool CWorkspaceSettings::LoadConfigOperation( CWorkspaceOperation &op, std::string &error_msg, CWorkspaceDataset *wks, CWorkspaceOperation *wkso )
{
	assert__( this == op.m_config );

	//assuming filters are already loaded

	ApplyToWholeSection< std::string >( GROUP_OPERATIONS,

		[&op]( const std::string &key, const std::string &value ) -> bool
		{
			return op.InsertOperation( value );		Q_UNUSED( key );
		}
	);

	for ( CObMap::iterator it = op.m_operations.begin(); it != op.m_operations.end(); it++ )
	{
		COperation* operation = dynamic_cast<COperation*>( it->second );
		if ( operation == nullptr )
		{
			error_msg += "ERROR in  CWorkspaceOperation::LoadConfigOperation\ndynamic_cast<COperation*>(it->second) returns nullptr pointer"
				"\nList seems to contain objects other than those of the class COperation";

			return false;
		}

		if ( !operation->LoadConfig( this, error_msg, wks, wkso ) )
			return false;
	}

	return Status() == QSettings::NoError;
}


bool CWorkspaceSettings::SaveConfig( const COperation &op, const CWorkspaceOperation *wks )
{
	std::string group = op.m_name;

	if ( op.OriginalDataset() != nullptr )
	{
		WriteSection( group,

			k_v( ENTRY_DSNAME,		op.OriginalDatasetName() ),
			k_v( ENTRY_RECORDNAME,	op.GetRecord() )
		);
	}

	WriteSection( group,

		k_v( ENTRY_TYPE,				CMapTypeOp::GetInstance().IdToName( op.m_type ) )
	);

	//v3 note
	// Data mode is no more for a operation, but for each formula (data expression)
	//bOk &= Write(ENTRY_DATA_MODE, CMapDataMode::GetInstance().IdToName(m_dataMode));

	// See note close to LoadConfigDesc call in LoadConfig( COperation &op, ...)
	//
	if (
		!op.mSelectionCriteria->SaveConfigDesc( this, group )
		||
		!op.m_formulas.SaveConfig( this, false, op.GetName() )	//v3 note:  Warning after formulas Load config conig path has changed
		)
		return false;

	WriteSection( group,

		//v4 k_v( ENTRY_OUTPUT,					Absolute2PortableDataPath( op.m_output ) ),					//op.GetOutputPathRelativeToWks( wks )
		//v4 k_v( ENTRY_EXPORT_ASCII_OUTPUT,	Absolute2PortableDataPath( op.m_exportAsciiOutput ) )		//op.GetExportAsciiOutputPathRelativeToWks( wks )	)
		//v4 k_v( ENTRY_EXPORT_GEOTIFF_OUTPUT,	Absolute2PortableDataPath( op.m_exportGeoTIFFOutput ) )		//op.GetExportGeoTIFFOutputPathRelativeToWks( wks )	)
		k_v( ENTRY_OUTPUT,					op.GetOutputPathRelativeToWks( wks ) ),
		k_v( ENTRY_EXPORT_ASCII_OUTPUT,		op.GetExportAsciiOutputPathRelativeToWks( wks )	),
		k_v( ENTRY_EXPORT_GEOTIFF_OUTPUT,	op.GetExportGeoTIFFOutputPathRelativeToWks( wks )	),
        k_v( ENTRY_OPERATION_FILTER,		op.FilterName() )
	);

	return Status() == QSettings::NoError;
}
bool CWorkspaceSettings::LoadConfig( COperation &op, std::string &error_msg, CWorkspaceDataset *wks, CWorkspaceOperation *wkso )
{
	std::string group = op.m_name;
	std::string dsname, type, data_mode, output, ascii_export_output, geo_tiff_export_output, v4filter;

	ReadSection( group,

		k_v( ENTRY_DSNAME,					&dsname					),
		k_v( ENTRY_TYPE,					&type,					CMapTypeOp::GetInstance().IdToName( op.m_type ) ),
		k_v( ENTRY_DATA_MODE,				&data_mode,				CMapDataMode::GetInstance().IdToName( op.m_dataMode ) ),
		k_v( ENTRY_RECORDNAME,				&op.m_record			),
		k_v( ENTRY_OUTPUT,					&output					),
		k_v( ENTRY_EXPORT_ASCII_OUTPUT,		&ascii_export_output	),
		k_v( ENTRY_EXPORT_GEOTIFF_OUTPUT,	&geo_tiff_export_output	),
		k_v( ENTRY_OPERATION_FILTER,		&v4filter				)

	);

	const CBratFilter *filter = CBratFilters::GetInstance().Find( v4filter );
	if ( !filter && !v4filter.empty() )
	{
		const std::string msg = "Operation '" + op.GetName() + "' referenced non-existing filter '" + v4filter + "'. The reference was removed.";
		LOG_WARN( msg );
	}
	op.SetFilter( filter );
	op.SetDataset( op.FindDataset( dsname, wks ) );
	op.m_type = type.empty() ? CMapTypeOp::eTypeOpYFX : CMapTypeOp::GetInstance().NameToId( type );
	op.m_dataMode = data_mode.empty() ? CMapDataMode::GetInstance().GetDefault() : CMapDataMode::GetInstance().NameToId( data_mode );
	if ( op.m_record.empty() )
	{
		op.m_record = CProductNetCdf::m_virtualRecordName;
	}
	if ( !output.empty() )
	{
		// Old Comment: Note that if the path to the output is in relative form,
		// SetOutput make it in absolute form based on workspace Operation path.		
		op.SetOutput( output, wkso );							//v4 op.SetOutput( PortableData2AbsolutePath( output ), wkso );
	}
	if ( !ascii_export_output.empty() )
	{
		// Old Comment: Note that if the path to the output is in relative form,
		// SetOutput make it in absolute form based on workspace Operation path.
		op.SetExportAsciiOutput( ascii_export_output, wkso );	//v4 op.SetExportAsciiOutput( PortableData2AbsolutePath( ascii_export_output ), wkso );
	}
	else
	{
		op.InitExportAsciiOutput( wkso );
	}

	if ( !geo_tiff_export_output.empty() )
	{
		op.SetExportGeoTIFFOutput( geo_tiff_export_output, wkso );	//v4 op.SetExportGeoTIFFOutput( PortableData2AbsolutePath( geo_tiff_export_output ), wkso );
	}
	else
	{
		op.InitExportGeoTIFFOutput( wkso );
	}

	// We don't use Group(), like v3 with the equivalent GetPath(), because, 
	//	given the open/close policy of QSttings, the current group is indeed 
	//	the parent of what we want. The "current" group in v3 was opened at 
	//	the beginning of this function.	!!! TODO !!!: test this and consider 
	//	the v3 note below
	//
	if ( 
		!op.mSelectionCriteria->LoadConfigDesc( this, group ) 
		||
		!op.m_formulas.LoadConfig( this, error_msg, false, op.GetName() )	// Warning after formulas Load config conig path has changed	(v3 note)
		)
		return false;

	op.m_formulas.InitFormulaDataMode( op.m_dataMode );

	return Status() == QSettings::NoError;
}


bool CWorkspaceSettings::SaveConfigDesc( const CFormula &f, const std::string& path )
{
	WriteSection( path,

		k_v( f.m_name,	f.GetDescription( true ) )
	);

	return Status() == QSettings::NoError;
}
bool CWorkspaceSettings::LoadConfigDesc( CFormula &f, const std::string& path )
{
	ReadSection( path,

		k_v( f.m_name,	&f.m_description,	f.m_description )
	);

	return Status() == QSettings::NoError;
}


bool CWorkspaceSettings::SaveConfigDisplay( const CWorkspaceDisplay &disp, std::string &error_msg, CWorkspaceDisplay *wksd )
{
	assert__( this == disp.m_config );

	{
		int32_t index = 0;
		for ( CObMap::const_iterator it = disp.m_displays.begin(); it != disp.m_displays.end(); it++ )
		{
			index++;
			CDisplay* display = dynamic_cast<CDisplay*>( it->second );
			if ( display == nullptr )
			{
				error_msg +=
					"ERROR in  CWorkspaceDisplay::m_displays\ndynamic_cast<CDisplay*>(it->second) returns nullptr pointer"
					"\nList seems to contain objects other than those of the class CDisplay";

				return false;
			}

			WriteSection( GROUP_DISPLAY,

				k_v( ENTRY_DISPLAYNAME + n2s< std::string >( index ), display->GetName() )
			);

			if ( !display->SaveConfig( this, wksd ) )
				return false;
		}
	}

	return Status() == QSettings::NoError;
}
bool CWorkspaceSettings::LoadConfigDisplay( CWorkspaceDisplay &disp, std::string &error_msg, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso )
{
	assert__( this == disp.m_config );

	ApplyToWholeSection< std::string >( GROUP_DISPLAY,

		[&disp]( const std::string &key, const std::string &value ) -> bool
		{
			return disp.InsertDisplay( value );		Q_UNUSED( key );
		}
	);

	for ( CObMap::const_iterator it = disp.m_displays.begin(); it != disp.m_displays.end(); it++ )
	{
		CDisplay* display = dynamic_cast< CDisplay* >( it->second );
		if ( display == nullptr )
		{
			error_msg +=
				"ERROR in  CWorkspaceDisplay::LoadConfigOperation\ndynamic_cast<CDisplay*>(it->second) returns nullptr pointer"
				"\nList seems to contain objects other than those of the class CDisplay\n";

			return false;
		}

		if ( !display->LoadConfig( this, error_msg, wksd, wkso ) )
			return false;
	}

	return true;
}


bool CWorkspaceSettings::SaveConfig( const CDisplay &d, CWorkspaceDisplay *wksd )
{
	WriteSection( d.m_name,

		k_v( ENTRY_TYPE,					CMapTypeDisp::GetInstance().IdToName( d.m_type ) ),
		k_v( ENTRY_TITLE,					d.Title() ),
		k_v( ENTRY_DISPLAY_DATA_X_LABEL,	d.TitleX() ),
		k_v( ENTRY_DISPLAY_DATA_Y_LABEL,	d.TitleY() ),
		k_v( ENTRY_DISPLAY_DATA_Z_LABEL,	d.TitleValue() ),

		k_v( ENTRY_DISPLAY_DATA_X_NUM_TICKS,	d.Xticks() ),
		k_v( ENTRY_DISPLAY_DATA_Y_NUM_TICKS,	d.Yticks() ),
		k_v( ENTRY_DISPLAY_DATA_Z_NUM_TICKS,	d.Zticks() ),

		k_v( ENTRY_DISPLAY_DATA_X_NUM_DIGITS,	d.Xdigits() ),
		k_v( ENTRY_DISPLAY_DATA_Y_NUM_DIGITS,	d.Ydigits() ),
		k_v( ENTRY_DISPLAY_DATA_Z_NUM_DIGITS,	d.Zdigits() ),

		k_v( ENTRY_ANIMATION,				d.GetWithAnimation() ),
		k_v( ENTRY_PROJECTION,				d.GetProjection() )
		);

	{
		CSection section( mSettings, d.m_name );

#if defined (BRAT_V3)
		if ( !isDefaultValue( d.m_minXValue ) )
		{
			WriteValue( section, ENTRY_MINXVALUE, d.m_minXValue );
		}
		if ( !isDefaultValue( d.m_maxXValue ) )
		{
			WriteValue( section, ENTRY_MAXXVALUE, d.m_maxXValue );
		}

		if ( !isDefaultValue( d.m_minYValue ) )
		{
			WriteValue( section, ENTRY_MINYVALUE, d.m_minYValue );
		}
		if ( !isDefaultValue( d.m_maxYValue ) )
		{
			WriteValue( section, ENTRY_MAXYVALUE, d.m_maxYValue );
		}
#endif

		WriteValue( section, ENTRY_ZOOM, d.m_zoom.GetAsText( CDisplay::m_zoomDelimiter ) );

		// the entry ENTRY_OUTPUT  is not used any more
		//bOk &= Write(ENTRY_OUTPUT, GetOutputName());
	}

	// Warning after formulas Load config conig path has changed
	if ( !SaveConfig( d.m_data, wksd, d.GetName() ) )
		return false;

	return Status() == QSettings::NoError;
}
bool CWorkspaceSettings::LoadConfig( CDisplay &d, std::string &error_msg, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso )
{
	std::string type, zoom;

	ReadSection( d.m_name,

		k_v( ENTRY_TYPE,					&type,				CMapTypeDisp::GetInstance().IdToName( d.m_type ) ),
		k_v( ENTRY_TITLE,					&d.mTitle			),
		k_v( ENTRY_DISPLAY_DATA_X_LABEL,	&d.mTitleX ),
		k_v( ENTRY_DISPLAY_DATA_Y_LABEL,	&d.mTitleY ),
		k_v( ENTRY_DISPLAY_DATA_Z_LABEL,	&d.mTitleValue ),

		k_v( ENTRY_DISPLAY_DATA_X_NUM_TICKS,	&d.m_xTicks, CDisplay::smDefaultNumberOfTicks ),
		k_v( ENTRY_DISPLAY_DATA_Y_NUM_TICKS,	&d.m_yTicks, CDisplay::smDefaultNumberOfTicks ),
		k_v( ENTRY_DISPLAY_DATA_Z_NUM_TICKS,	&d.m_zTicks, CDisplay::smDefaultNumberOfTicks ),

		k_v( ENTRY_DISPLAY_DATA_X_NUM_DIGITS,	&d.m_xDigits, CDisplay::smDefaultNumberOfDigits ),
		k_v( ENTRY_DISPLAY_DATA_Y_NUM_DIGITS,	&d.m_yDigits, CDisplay::smDefaultNumberOfDigits ),
		k_v( ENTRY_DISPLAY_DATA_Z_NUM_DIGITS,	&d.m_zDigits, CDisplay::smDefaultNumberOfDigits ),

		k_v( ENTRY_ANIMATION,				&d.m_withAnimation,	false ),
		k_v( ENTRY_PROJECTION,				&d.m_projection,	PROJECTION_3D_VALUE ),

#if defined (BRAT_V3)
		k_v( ENTRY_MINXVALUE,	&d.m_minXValue,		defaultValue< double >() ),
		k_v( ENTRY_MAXXVALUE,	&d.m_maxXValue,		defaultValue< double >() ),
		k_v( ENTRY_MINYVALUE,	&d.m_minYValue,		defaultValue< double >() ),
		k_v( ENTRY_MAXYVALUE,	&d.m_maxYValue,		defaultValue< double >() ),
#endif

		k_v( ENTRY_ZOOM,		&zoom				)
	);

	d.SetName( d.GetName() );		//force title if empty
	d.m_type = type.empty() ? CMapTypeDisp::Invalid() : CMapTypeDisp::GetInstance().NameToId( type );
	d.m_zoom.Set( zoom, CDisplay::m_zoomDelimiter );

	// the entry ENTRY_OUTPUT  is not used any more
	//  value_string = Read(ENTRY_OUTPUT);
	//  if (value_string.empty() == false)
	//  {
	//    SetOutput(value_string);
	//  }

	d.InitOutput( wksd );

	return LoadConfig( d.m_data, &d, error_msg, wksd, wkso, d.GetName() );
}

bool CWorkspaceSettings::SaveConfig( const CMapDisplayData &data, CWorkspaceDisplay *wks, const std::string& pathSuff )
{
	std::string path = GROUP_DISPLAY + "_" + pathSuff;			assert__( !pathSuff.empty() );

	{
		int index = 0;
		for ( CMapDisplayData::const_iterator it = data.begin(); it != data.end(); it++ )
		{
			CDisplayData* displayData = dynamic_cast<CDisplayData*>( it->second );
			if ( displayData != nullptr )
			{
				index++;
				std::string key = displayData->GetDataKey() + "_" + pathSuff;

				WriteSection( path,

					k_v( ENTRY_DISPLAY_DATA + n2s<std::string>( index ), key )
				);

				if ( !SaveConfig( *displayData, pathSuff, wks ) )
					return false;
			}
		}
	}
	return Status() == QSettings::NoError;
}
bool CWorkspaceSettings::LoadConfig( CMapDisplayData &data, const CDisplay *parent, std::string &error_msg, CWorkspaceDisplay *wks, 
	CWorkspaceOperation *wkso, const std::string& pathSuff )
{
    std::string path = GROUP_DISPLAY + "_" + pathSuff;			assert__( !pathSuff.empty() );      Q_UNUSED( error_msg );

	std::vector< std::string> data_display_keys;

	ApplyToWholeSection< std::string >( path,

		[ &data, &pathSuff, &data_display_keys ]( const std::string &key, const std::string &value ) -> bool
		{
			std::string displayDataName = value.substr( 0, value.length() - pathSuff.length() - 1 );

			if ( dynamic_cast< CDisplayData* >( data.Exists( displayDataName ) ) != nullptr )
				data.Erase( displayDataName );
                                                                                              Q_UNUSED( key )
            data_display_keys.push_back( displayDataName );
            //data.Insert( displayDataName, new CDisplayData() );

			return true;	//continue iteration
		}
	);

	CStringMap renameKeyMap;
	//for ( CMapDisplayData::iterator it = data.begin(); it != data.end(); it++ )
	for ( auto const &key : data_display_keys )
	{
		CDisplayData *displayData = nullptr;
		//CDisplayData *displayData = dynamic_cast< CDisplayData* >( it->second );
		//if ( displayData == nullptr )
		//{
		//	error_msg +=
		//		"ERROR in  CMapDisplayData::LoadConfig\ndynamic_cast<CDisplayData*>(it->second) returns nullptr pointer"
		//		"\nList seems to contain objects other than those of the class CDisplayData";

		//	return false;		// v3: GUI error message displayed here
		//}

		//if ( !LoadConfig( *displayData, it->first + "_" + pathSuff, wks, wkso ) )
		if ( !LoadConfig( displayData, parent, key + "_" + pathSuff, wks, wkso ) )
			return false;

        data.Insert( key, displayData );

		// To maintain compatibility with Brat v1.x (display name doesn't contain 'display type' in v1.x)
		const std::string displayDataKey = displayData->GetDataKey();
		//if ( it->first != displayDataKey )
		if ( key != displayDataKey )
		{
			renameKeyMap.Insert( key, displayDataKey );
		}
	}


	for ( CStringMap::const_iterator itStringMap = renameKeyMap.begin(); itStringMap != renameKeyMap.end(); itStringMap++ )
	{
		data.RenameKey( itStringMap->first, itStringMap->second );
	}

	return true;
}

bool CWorkspaceSettings::SaveConfig( CDisplayData &data, const std::string& pathSuff, CWorkspaceDisplay *wksd )
{
	std::string path = data.GetDataKey();
	if ( !pathSuff.empty() )
		path += "_" + pathSuff;


	//inherited CFieldData

	WriteSection( path,

		k_v( ENTRY_FIELD_DATA_NAME, data.mFieldName ),
		k_v( ENTRY_FIELD_DESC_NAME, data.mUserName ),

		k_v( ENTRY_FIELD_DATA_OPACITY, data.mOpacity ),

		k_v( ENTRY_FIELD_DATA_X_LOG,	data.mXlogarithmic ),
		k_v( ENTRY_FIELD_DATA_Y_LOG,	data.mYlogarithmic ),

		k_v( ENTRY_FIELD_DATA_WITHCONTOUR,				data.mWithContour ),
		k_v( ENTRY_FIELD_DATA_NUMCONTOUR,				data.mNumContours ),
		k_v( ENTRY_FIELD_DATA_CONTOURPRECISIONGRID1,	data.mContourPrecisionGrid1 ),
		k_v( ENTRY_FIELD_DATA_CONTOURPRECISIONGRID2,	data.mContourPrecisionGrid2 ),

		k_v( ENTRY_FIELD_DATA_CONTOURLINEWIDTH,	data.mContourLineWidth ),
		k_v( ENTRY_FIELD_DATA_CONTOURLINECOLOR,	data.mContourLineColor.GetQColor() ),

		k_v( ENTRY_FIELD_DATA_WITHSOLIDCOLOR,	data.mWithSolidColor ),

		k_v( ENTRY_EAST_COMPONENT,				data.IsEastComponent() ),
		k_v( ENTRY_NORTH_COMPONENT,				data.IsNorthComponent() ),

		k_v( ENTRY_FIELD_DATA_SHOW_LINES,	data.mLines ),
		k_v( ENTRY_FIELD_DATA_SHOW_POINTS,	data.mPoints ),

		k_v( ENTRY_FIELD_DATA_POINTSIZE,	data.mPointSize ),
		k_v( ENTRY_FIELD_DATA_GLYPHTYPE,	data.mPointGlyph ),
		k_v( ENTRY_FIELD_DATA_FILLEDPOINT,	data.mFilledPoint ),

		k_v( ENTRY_FIELD_DATA_LINE_COLOR,		data.mLineColor.GetQColor() ),
		k_v( ENTRY_FIELD_DATA_LINE_WIDTH,		data.mLineWidth ),
		k_v( ENTRY_FIELD_DATA_STIPPLE_PATTERN,	data.mStipplePattern ),
		k_v( ENTRY_FIELD_DATA_POINT_COLOR,		data.mPointColor.GetQColor() ),

		k_v( ENTRY_NUMBER_OF_BINS, data.mNumberOfBins )
	);

	if ( !data.ColorPalette().empty() )
	{
		// v3 note
		// if color palette is a file (it has an extension)
		// save path in relative form, based on workspace Display path
		std::string paletteToWrite = data.ColorPalette();
		// TODO this is v3: !!! test !!! this if ever reused
		if ( !GetLastExtensionFromPath( paletteToWrite ).empty() )
		{
			if ( wksd != nullptr )
			{
				paletteToWrite = GetRelativePath( wksd->GetPath(), data.ColorPalette() );
			}
		}

		WriteSection( path,

			k_v( ENTRY_COLOR_PALETTE,		paletteToWrite )
		);
	}

	if ( !data.ColorCurve().empty() )
	{
		WriteSection( path,

			k_v( ENTRY_COLOR_TABLE_CURVE,	data.ColorCurve() )
		);
	}


	double current_min = data.CurrentMinValue();
	double current_max = data.CurrentMaxValue();
	if ( !isDefaultValue( data.mMinHeightValue ) && !isDefaultValue( data.mMaxHeightValue ) && 
		( current_min != CBratLookupTable::smDefaultRangeValues ) && (current_max != CBratLookupTable::smDefaultRangeValues ) )
	{
		WriteSection( path,

			k_v( ENTRY_FIELD_DATA_MIN_HEIGHT_VALUE,				CTools::Format( "%.15g", data.mMinHeightValue ) ),
			k_v( ENTRY_FIELD_DATA_MAX_HEIGHT_VALUE,				CTools::Format( "%.15g", data.mMaxHeightValue ) ),
			k_v( ENTRY_FIELD_DATA_MIN_CURRENT_HEIGHT_VALUE,		CTools::Format( "%.15g", current_min ) ),
			k_v( ENTRY_FIELD_DATA_MAX_CURRENT_HEIGHT_VALUE,		CTools::Format( "%.15g", current_max ) )
		);
	}
	else
	{
		ClearKeys( path,

		{ ENTRY_FIELD_DATA_MIN_HEIGHT_VALUE, ENTRY_FIELD_DATA_MAX_HEIGHT_VALUE, 
		ENTRY_FIELD_DATA_MIN_CURRENT_HEIGHT_VALUE, ENTRY_FIELD_DATA_MAX_CURRENT_HEIGHT_VALUE }

		);
	}


	if ( !isDefaultValue( data.mMinContourValue ) && !isDefaultValue( data.mMaxContourValue ) )
	{
		WriteSection( path,

			k_v( ENTRY_FIELD_DATA_MIN_CONTOURVALUE, CTools::Format( "%.15g", data.mMinContourValue ) ),
			k_v( ENTRY_FIELD_DATA_MAX_CONTOURVALUE, CTools::Format( "%.15g", data.mMaxContourValue ) )
		);
	}
	else
	{
		ClearKeys( path,

		{ ENTRY_FIELD_DATA_MIN_CONTOURVALUE, ENTRY_FIELD_DATA_MAX_CONTOURVALUE }

		);
	}

	if ( !isDefaultValue( data.MagnitudeFactor() ) )
	{
		WriteSection( path,

			k_v( ENTRY_FIELD_DATA_MAGNITUDE_FACTOR, CTools::Format( "%.15g", data.MagnitudeFactor() ) )
		);
	}
	else
	{
		ClearKeys( path,

		{ ENTRY_FIELD_DATA_MAGNITUDE_FACTOR, ENTRY_FIELD_DATA_MAX_CONTOURVALUE }

		);
	}


	//CDisplayData

	//type & fields

	WriteSection( path,

		k_v( ENTRY_TYPE, CMapTypeDisp::GetInstance().IdToName( data.m_type ) ),

		k_v( ENTRY_FIELD, data.m_field.GetName() ),
		k_v( ENTRY_FIELDNAME, data.m_field.GetDescription()	),
		k_v( ENTRY_UNIT, data.m_field.GetUnit() ),

		k_v( ENTRY_X, data.GetX()->GetName() ),
		k_v( ENTRY_XDESCR, data.GetX()->GetDescription() ),
		k_v( ENTRY_XUNIT, data.GetX()->GetUnit() ),

		k_v( ENTRY_Y, data.GetY()->GetName() ),
		k_v( ENTRY_YDESCR, data.GetY()->GetDescription() ),
		k_v( ENTRY_YUNIT, data.GetY()->GetUnit() )

		//k_v( ENTRY_Z, data.GetZ()->GetName() ),
		//k_v( ENTRY_ZDESCR, data.GetZ()->GetDescription() ),
		//k_v( ENTRY_ZUNIT, data.GetZ()->GetUnit() )
	);

	//1. operation...
	if ( data.mOperation != nullptr )
	{
		WriteSection( path,

			k_v( ENTRY_OPNAME, data.mOperation->GetName() )
		);
	}

	WriteSection( path,

		//0. group ( TBC )
		k_v( ENTRY_GROUP, data.m_group )

#if defined(BRAT_V3)
		,
		//3. axis
		k_v( ENTRY_INVERT_XYAXES, data.m_invertXYAxes )
#endif

	);


#if defined(BRAT_V3)

	if ( !data.m_xAxis.empty() )
	{
		WriteSection( path,

			k_v( ENTRY_X_AXIS, data.m_xAxis )
		);
	}
#endif


	return Status() == QSettings::NoError;
}
bool CWorkspaceSettings::LoadConfig( CDisplayData *&pdata, const CDisplay *parent, const std::string& path, CWorkspaceDisplay *wks, CWorkspaceOperation *wkso )
{
	//type & operation

	std::string stype, op_name;

	ReadSection( path,

		k_v( ENTRY_TYPE,	&stype,		CMapTypeDisp::GetInstance().IdToName( CMapTypeDisp::Invalid() ) ),
		k_v( ENTRY_OPNAME,	&op_name	)
	);

	CMapTypeDisp::ETypeDisp type = CMapTypeDisp::Invalid();
	if ( !stype.empty() )	
	{
		// Because Z=F(Lat,Lon) have been changed to Z=F(Lon,Lat)	(v3.3.0 note)
		//
		if ( str_icmp( stype, OLD_VALUE_DISPLAY_eTypeZFLatLon ) )
			type = CMapTypeDisp::eTypeDispZFLatLon;
		else
			type = CMapTypeDisp::GetInstance().NameToId( stype );
	}

	const COperation *operation = wkso->GetOperation( op_name );


	//create data item

	pdata = new CDisplayData( operation, parent, type );
	CDisplayData &data = *pdata;


	//fill display data item

	std::string 
		field_name, field_description, unit, 
		entry_x, entry_x_desc, entry_x_unit,
		entry_y, entry_y_desc, entry_y_unit,		//entry_z, entry_z_desc, entry_z_unit,
		palette, curve;


	//inherited CFieldData

	QColor line_color, point_color, contour_line_color;
	double current_min = CBratLookupTable::smDefaultRangeValues;
	double current_max = CBratLookupTable::smDefaultRangeValues;
	ReadSection( path,

		k_v( ENTRY_FIELD_DATA_NAME,		&data.mFieldName ),
		k_v( ENTRY_FIELD_DESC_NAME,		&data.mUserName ),

		k_v( ENTRY_FIELD_DATA_OPACITY,	&data.mOpacity ),

		k_v( ENTRY_FIELD_DATA_X_LOG,	&data.mXlogarithmic, false ),
		k_v( ENTRY_FIELD_DATA_Y_LOG,	&data.mYlogarithmic, false ),

		k_v( ENTRY_COLOR_PALETTE,			&palette,				data.ColorPalette()	),
		k_v( ENTRY_COLOR_TABLE_CURVE,		&curve,					data.ColorCurve() ),

		k_v( ENTRY_FIELD_DATA_WITHCONTOUR,		&data.mWithContour,			false ),
		k_v( ENTRY_FIELD_DATA_NUMCONTOUR,		&data.mNumContours,			CFieldData::smDefaultNumContour ),

		k_v( ENTRY_FIELD_DATA_CONTOURPRECISIONGRID1,	&data.mContourPrecisionGrid1, CFieldData::smContourPrecisionGrid1 ),
		k_v( ENTRY_FIELD_DATA_CONTOURPRECISIONGRID2,	&data.mContourPrecisionGrid2, CFieldData::smContourPrecisionGrid2 ),

		k_v( ENTRY_FIELD_DATA_CONTOURLINEWIDTH,	&data.mContourLineWidth,	CFieldData::smDefaultContourLineWidth ),
		k_v( ENTRY_FIELD_DATA_CONTOURLINECOLOR,	&contour_line_color,		CFieldData::smDefaultContourLineColor.GetQColor() ),
		k_v( ENTRY_FIELD_DATA_MIN_CONTOURVALUE,	&data.mMinContourValue,		defaultValue< double >() ),
		k_v( ENTRY_FIELD_DATA_MAX_CONTOURVALUE,	&data.mMaxContourValue,		defaultValue< double >() ),

		k_v( ENTRY_FIELD_DATA_WITHSOLIDCOLOR,	&data.mWithSolidColor,		true ),

		k_v( ENTRY_EAST_COMPONENT,				&data.mEastComponent,		false  ),
		k_v( ENTRY_NORTH_COMPONENT,				&data.mNorthComponent,		false  ),
		k_v( ENTRY_FIELD_DATA_MAGNITUDE_FACTOR,	&data.mMagnitudeFactor,		defaultValue< double >() ),

		k_v( ENTRY_FIELD_DATA_SHOW_LINES,	&data.mLines,	CFieldData::smDefaultShowLines ),
		k_v( ENTRY_FIELD_DATA_SHOW_POINTS,	&data.mPoints,	CFieldData::smDefaultShowPoints ),

		k_v( ENTRY_FIELD_DATA_POINTSIZE,	&data.mPointSize,			CFieldData::smDefaultPointSize ),
		k_v( ENTRY_FIELD_DATA_GLYPHTYPE,	(int*)&data.mPointGlyph,	(int)CFieldData::smDefaultGlyphType ),
		k_v( ENTRY_FIELD_DATA_FILLEDPOINT,	&data.mFilledPoint,			CFieldData::smDefaultFilledPoint ),

		k_v( ENTRY_FIELD_DATA_LINE_COLOR,		&line_color,					CMapColor::GetInstance().NextPrimaryColors().GetQColor() ),
		k_v( ENTRY_FIELD_DATA_LINE_WIDTH,		&data.mLineWidth,				CFieldData::smDefaultLineWidth ),
		k_v( ENTRY_FIELD_DATA_STIPPLE_PATTERN,	(int*)&data.mStipplePattern,	(int)CFieldData::smDefaultStipplePattern ),
		k_v( ENTRY_FIELD_DATA_POINT_COLOR,		&point_color,					CFieldData::smDefaultPointColor.GetQColor() ),

		k_v( ENTRY_FIELD_DATA_MIN_HEIGHT_VALUE,				&data.mMinHeightValue,	defaultValue< double >() ),
		k_v( ENTRY_FIELD_DATA_MAX_HEIGHT_VALUE,				&data.mMaxHeightValue,	defaultValue< double >() ),
		k_v( ENTRY_FIELD_DATA_MIN_CURRENT_HEIGHT_VALUE,		&current_min,			CBratLookupTable::smDefaultRangeValues ),
		k_v( ENTRY_FIELD_DATA_MAX_CURRENT_HEIGHT_VALUE,		&current_max,			CBratLookupTable::smDefaultRangeValues ),

		k_v( ENTRY_NUMBER_OF_BINS,		&data.mNumberOfBins,		CFieldData::smDefaultNumberOfBins )
	);

	data.SetLineColor( line_color );
	data.SetPointColor( point_color );
	data.SetContourLineColor( contour_line_color );
	data.SetCurrentMinValue( current_min );
	data.SetCurrentMaxValue( current_max );

	// if color palette is a file (it has an extension)
	// save path in absolute form, based on workspace Display path

	//wxFileName relative;
	//relative.Assign( data.m_colorPalette );
	//std::string paletteToSet = data.m_colorPalette;		//not used
	//if ( relative.HasExt() )
	//{
	//	//CWorkspaceDisplay* wks = wxGetApp().GetCurrentWorkspaceDisplay();
	//	if ( wks != nullptr )
	//	{
	//		relative.Normalize( wxPATH_NORM_ALL, wks->GetPath() );
	//		data.m_colorPalette = relative.GetFullPath();
	//	}
	//}

	// TODO this is v3 !!! test !!! this if ever reused
	if ( !GetLastExtensionFromPath( palette ).empty() )
	{
		if ( wks != nullptr )
			palette = NormalizedAbsolutePath( palette, wks->GetPath() );		// TODO !!! test !!! this
	}
	data.SetColorPalette( palette );
	data.SetColorCurve( curve );


	//CDisplayData

	ReadSection( path,

		//fields...

		k_v( ENTRY_FIELD,				&field_name					),
		k_v( ENTRY_FIELDNAME,			&field_description			),		// v4: yes, not a mistake, the key retrieves a value for setDescription
		k_v( ENTRY_UNIT,				&unit						),

		k_v( ENTRY_X,					&entry_x					),
		k_v( ENTRY_XDESCR,				&entry_x_desc				),
		k_v( ENTRY_XUNIT,				&entry_x_unit				),

		k_v( ENTRY_Y,					&entry_y					),
		k_v( ENTRY_YDESCR,				&entry_y_desc				),
		k_v( ENTRY_YUNIT,				&entry_y_unit				),

		//k_v( ENTRY_Z,					&entry_z					),
		//k_v( ENTRY_ZDESCR,				&entry_z_desc				),
		//k_v( ENTRY_ZUNIT,				&entry_z_unit				),

		//1. operation...
		k_v( ENTRY_OPNAME,				&op_name					),

		//0. group ( TBC )
		k_v( ENTRY_GROUP,				&data.m_group,				1 )

#if defined(BRAT_V3)
		//3. axis

		,
		k_v( ENTRY_X_AXIS,				&data.m_xAxis				),
		k_v( ENTRY_INVERT_XYAXES,		&data.m_invertXYAxes,		false  )
#endif
	);


	// key & fields

	data.SetFieldName( field_name );
	data.SetFieldDescription( field_description );
	data.SetFieldUnit( unit );

	data.SetDimension( CDisplayData::eX, entry_x, entry_x_desc, entry_x_unit );
	data.SetDimension( CDisplayData::eY, entry_y, entry_y_desc, entry_y_unit );
	//data.SetDimension( 2, entry_z, entry_z_desc, entry_z_unit );

	return true;
}




void CWorkspaceSettings::SaveFunctionDescrTemplate( const std::string &internal_data_path, bool flush )
{
    Q_UNUSED( internal_data_path );

	//if ( config == nullptr )
	//{
	//	std::string filePathName = internal_data_path;
	//	filePathName.append( PATH_SEPARATOR );
	//	filePathName.append( CMapFunction::m_configFilename.c_str() );
	//	//config = new wxFileConfig( wxEmptyString, wxEmptyString, filePathName.c_str(), wxEmptyString, wxCONFIG_USE_LOCAL_FILE );
	//	config = new CWorkspaceSettings( filePathName );
	//}

	for ( CMapFunction::iterator it = CMapFunction::GetInstance().begin(); it != CMapFunction::GetInstance().end(); it++ )
	{
		CFunction* value = dynamic_cast<CFunction*>( it->second );
		if ( value == nullptr )
		{
			continue;
		}

		//TODO translate all the following commented out lines to Qt settings 

		//SetPath( "/" + value->GetName() );

		//std::string str;
		//for ( int32_t i = 0; i < value->GetNbparams(); i++ )
		//{
		//	str += ( "param" + n2s<std::string>( i + 1 ) + ": \n" );
		//}

		//Write( ENTRY_COMMENT, str.c_str() );
	}

	if ( flush )
	{
		Sync();
	}
}
std::string CWorkspaceSettings::GetFunctionExtraDescr( const std::string& path )
{
	//if ( config == nullptr )
	//{
	//	return "";
	//}

	std::string value;

	ReadSection( path,

		k_v( ENTRY_COMMENT, &value )

		);

	//SetPath( "/" + pathSuff );
	//std::string valueString;
	//valueString = Read( ENTRY_COMMENT );

	return value;
}
