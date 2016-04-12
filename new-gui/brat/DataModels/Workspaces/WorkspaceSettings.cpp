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

#include "../Filters/BratFilters.h"
#include "Workspace.h"
#include "Dataset.h"
#include "Operation.h"
#include "Function.h"
#include "WorkspaceSettings.h"
#include "Display.h"
#include "DataModels/MapTypeDisp.h"



//static 
const CApplicationPaths *CWorkspaceSettings::smBratPaths = nullptr;


std::string CWorkspaceSettings::Absolute2PortableDataPath( const std::string &path ) const
{
	return smBratPaths ?
		smBratPaths->Absolute2PortableDataPath( path ) :
		path;
}


std::string CWorkspaceSettings::Portable2AbsoluteDataPath( const std::string &path ) const
{
	return smBratPaths ?
		smBratPaths->Portable2AbsoluteDataPath( path ) :
		path;
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



bool CWorkspaceSettings::SaveConfigDataset( const CWorkspaceDataset &data, std::string &errorMsg )
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
				errorMsg += "ERROR in  CWorkspaceDataset::SaveConfigDataset\ndynamic_cast<CDataset*>(it->second) returns nullptr pointer"
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
			WriteValue( section, ENTRY_FILE + n2s<std::string>( index ), Absolute2PortableDataPath( *it ) );		//v4 Absolute2PortableDataPath( *it )
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
			value_string = Portable2AbsoluteDataPath( value_string );

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
			errorMsg +=
				"ERROR in  CMapFormula::LoadConfig\ndynamic_cast<CFormula*>(it->second) returns nullptr pointer"
				"\nList seems to contain objects other than those of the class CFormula.\n";
			
			return false;
		}

		if ( !formula->LoadConfig( this, errorMsg, pathSuff ) )
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

		k_v( ENTRY_DEFINE,		f.GetDescription( true ) ),
		k_v( ENTRY_COMMENT,		f.GetComment( true ) ),
		k_v( ENTRY_UNIT,		f.m_unit.GetText() ),
		k_v( ENTRY_FIELDTYPE,	f.GetTypeAsString() ),
		k_v( ENTRY_DATATYPE,	f.GetDataTypeAsString() ),
		k_v( ENTRY_TITLE,		f.m_title ),
		k_v( ENTRY_FILTER,		f.GetFilterAsString() )
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
bool CWorkspaceSettings::LoadConfig( CFormula &f, std::string &errorMsg, const std::string& pathSuff )
{			
	std::string path = f.m_name;
	if ( !pathSuff.empty() )
		path += "_" + pathSuff;

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
				
	f.SetUnit( unit, errorMsg, "" );
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


bool CWorkspaceSettings::SaveConfigOperation( const CWorkspaceOperation &op, std::string &errorMsg )
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
				errorMsg += "ERROR in  CWorkspaceOperation::SaveConfigOperation\ndynamic_cast<COperation*>(it->second) returns nullptr pointer"
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
bool CWorkspaceSettings::LoadConfigOperation( CWorkspaceOperation &op, std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceOperation *wkso )
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
			errorMsg += "ERROR in  CWorkspaceOperation::LoadConfigOperation\ndynamic_cast<COperation*>(it->second) returns nullptr pointer"
				"\nList seems to contain objects other than those of the class COperation";

			return false;
		}

		if ( !operation->LoadConfig( this, errorMsg, wks, wkso ) )
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
		!op.m_select->SaveConfigDesc( this, group )
		||
		!op.m_formulas.SaveConfig( this, false, op.GetName() )	//v3 note:  Warning after formulas Load config conig path has changed
		)
		return false;

	WriteSection( group,

		//v4 k_v( ENTRY_OUTPUT,				Absolute2PortableDataPath( op.m_output ) ),					//op.GetOutputPathRelativeToWks( wks )
		//v4 k_v( ENTRY_EXPORT_ASCII_OUTPUT, Absolute2PortableDataPath( op.m_exportAsciiOutput ) )		//op.GetExportAsciiOutputPathRelativeToWks( wks )	)
		k_v( ENTRY_OUTPUT,				op.GetOutputPathRelativeToWks( wks ) ),
		k_v( ENTRY_EXPORT_ASCII_OUTPUT, op.GetExportAsciiOutputPathRelativeToWks( wks )	),
        k_v( ENTRY_OPERATION_FILTER,	op.FilterName() )
	);

	return Status() == QSettings::NoError;
}
bool CWorkspaceSettings::LoadConfig( COperation &op, std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceOperation *wkso )
{
	std::string group = op.m_name;
	std::string dsname, type, data_mode, output, ascii_export_output, v4filter;

	ReadSection( group,

		k_v( ENTRY_DSNAME,				&dsname					),
		k_v( ENTRY_TYPE,				&type,					CMapTypeOp::GetInstance().IdToName( op.m_type ) ),
		k_v( ENTRY_DATA_MODE,			&data_mode,				CMapDataMode::GetInstance().IdToName( op.m_dataMode ) ),
		k_v( ENTRY_RECORDNAME,			&op.m_record			),
		k_v( ENTRY_OUTPUT,				&output					),
		k_v( ENTRY_EXPORT_ASCII_OUTPUT,	&ascii_export_output	),
		k_v( ENTRY_OPERATION_FILTER,	&v4filter				)

	);

	const CBratFilter *filter = CBratFilters::GetInstance().Find( v4filter );
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

	// We don't use Group(), like v3 with the equivalent GetPath(), because, 
	//	given the open/close policy of QSttings, the current group is indeed 
	//	the parent of what we want. The "current" group in v3 was opened at 
	//	the beginning of this function.	!!! TODO !!!: test this and consider 
	//	the v3 note below
	//
	if ( 
		!op.m_select->LoadConfigDesc( this, group ) 
		||
		!op.m_formulas.LoadConfig( this, errorMsg, false, op.GetName() )	// Warning after formulas Load config conig path has changed	(v3 note)
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


bool CWorkspaceSettings::SaveConfigDisplay( const CWorkspaceDisplay &disp, std::string &errorMsg, CWorkspaceDisplay *wksd )
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
				errorMsg +=
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
bool CWorkspaceSettings::LoadConfigDisplay( CWorkspaceDisplay &disp, std::string &errorMsg, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso )
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
			errorMsg +=
				"ERROR in  CWorkspaceDisplay::LoadConfigOperation\ndynamic_cast<CDisplay*>(it->second) returns nullptr pointer"
				"\nList seems to contain objects other than those of the class CDisplay\n";

			return false;
		}

		if ( !display->LoadConfig( this, errorMsg, wksd, wkso ) )
			return false;
	}

	return true;
}


bool CWorkspaceSettings::SaveConfig( const CDisplay &d, CWorkspaceDisplay *wksd )
{
	WriteSection( d.m_name,

		k_v( ENTRY_TYPE,		CMapTypeDisp::GetInstance().IdToName( d.m_type ) ),
		k_v( ENTRY_TITLE,		d.GetTitle() ),
		k_v( ENTRY_ANIMATION,	d.GetWithAnimation() ),
		k_v( ENTRY_PROJECTION,	d.GetProjection() )
		);

	{
		CSection section( mSettings, d.m_name );

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

		WriteValue( section, ENTRY_ZOOM, d.m_zoom.GetAsText( CDisplay::m_zoomDelimiter ) );

		// the entry ENTRY_OUTPUT  is not used any more
		//bOk &= Write(ENTRY_OUTPUT, GetOutputName());
	}

	// Warning after formulas Load config conig path has changed
	if ( !d.m_data.SaveConfig( this, wksd, d.GetName() ) )
		return false;

	return Status() == QSettings::NoError;
}
bool CWorkspaceSettings::LoadConfig( CDisplay &d, std::string &errorMsg, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso )
{
	std::string type, zoom;

	ReadSection( d.m_name,

		k_v( ENTRY_TYPE,		&type,				CMapTypeDisp::GetInstance().IdToName( d.m_type ) ),
		k_v( ENTRY_TITLE,		&d.m_title			),
		k_v( ENTRY_ANIMATION,	&d.m_withAnimation,	false ),
		k_v( ENTRY_PROJECTION,	&d.m_projection,	PROJECTION_3D_VALUE ),
		k_v( ENTRY_MINXVALUE,	&d.m_minXValue,		defaultValue< double >() ),
		k_v( ENTRY_MAXXVALUE,	&d.m_maxXValue,		defaultValue< double >() ),
		k_v( ENTRY_MINYVALUE,	&d.m_minYValue,		defaultValue< double >() ),
		k_v( ENTRY_MAXYVALUE,	&d.m_maxYValue,		defaultValue< double >() ),
		k_v( ENTRY_ZOOM,		&zoom				)
	);

	d.m_type = type.empty() ? CMapTypeDisp::Invalid() : CMapTypeDisp::GetInstance().NameToId( type );
	d.m_zoom.Set( zoom, CDisplay::m_zoomDelimiter );

	// the entry ENTRY_OUTPUT  is not used any more
	//  value_string = Read(ENTRY_OUTPUT);
	//  if (value_string.empty() == false)
	//  {
	//    SetOutput(value_string);
	//  }

	d.InitOutput( wksd );

	return d.m_data.LoadConfig( this, errorMsg, wksd, wkso, d.GetName() );
}

bool CWorkspaceSettings::SaveConfig( const CMapDisplayData &data, CWorkspaceDisplay *wks, const std::string& pathSuff )
{
	std::string path = GROUP_DISPLAY;
	if ( !pathSuff.empty() )
		path += "_" + pathSuff;

	{
		int index = 0;
		for ( CMapDisplayData::const_iterator it = data.begin(); it != data.end(); it++ )
		{
			CDisplayData* displayData = dynamic_cast<CDisplayData*>( it->second );
			if ( displayData != nullptr )
			{
				index++;
				std::string key = displayData->GetDataKey();
				if ( !pathSuff.empty() )
					key += "_" + pathSuff;

				WriteSection( path,

					k_v( ENTRY_DISPLAY_DATA + n2s<std::string>( index ), key )
				);

				if ( !displayData->SaveConfig( this, pathSuff, wks ) )
					return false;
			}
		}
	}
	return Status() == QSettings::NoError;
}
bool CWorkspaceSettings::LoadConfig( CMapDisplayData &data, std::string &errorMsg, CWorkspaceDisplay *wks, CWorkspaceOperation *wkso, const std::string& pathSuff )
{
	std::string path = GROUP_DISPLAY;
	if ( !pathSuff.empty() )
		path += "_" + pathSuff;

	ApplyToWholeSection< std::string >( path,

		[ &data, &pathSuff ]( const std::string &key, const std::string &value ) -> bool
		{
			std::string displayDataName = 
				!pathSuff.empty() ? 
				value.substr( 0, value.length() - pathSuff.length() - 1 ) :
				value;

			if ( dynamic_cast< CDisplayData* >( data.Exists( displayDataName ) ) != nullptr )
				data.Erase( displayDataName );

            data.Insert( displayDataName, new CDisplayData() );         Q_UNUSED( key )

			return true;	//continue iteration
		}
	);

	CStringMap renameKeyMap;
	for ( CMapDisplayData::iterator it = data.begin(); it != data.end(); it++ )
	{
		CDisplayData* displayData = dynamic_cast< CDisplayData* >( it->second );
		if ( displayData == nullptr )
		{
			errorMsg +=
				"ERROR in  CMapDisplayData::LoadConfig\ndynamic_cast<CDisplayData*>(it->second) returns nullptr pointer"
				"\nList seems to contain objects other than those of the class CDisplayData";

			return false;		// v3: GUI error message displayed here
		}

		if ( !displayData->LoadConfig( this, it->first + "_" + pathSuff, wks, wkso ) )
			return false;

		// To maintain compatibility with Brat v1.x (display name doesn't contain 'display type' in v1.x)
		const std::string displayDataKey = displayData->GetDataKey();

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

bool CWorkspaceSettings::SaveConfig( CDisplayData &data, const std::string& pathSuff, CWorkspaceDisplay *wksd )
{
	std::string path = data.GetDataKey();
	if ( !pathSuff.empty() )
		path += "_" + pathSuff;

	WriteSection( path,

		k_v( ENTRY_TYPE, CMapTypeDisp::GetInstance().IdToName( data.m_type ) ),
		k_v( ENTRY_FIELD, data.m_field.GetName() ),
		k_v( ENTRY_FIELDNAME, data.m_field.GetDescription()	),
		k_v( ENTRY_UNIT, data.m_field.GetUnit() ),

		k_v( ENTRY_X, data.m_x.GetName() ),
		k_v( ENTRY_XDESCR, data.m_x.GetDescription() ),
		k_v( ENTRY_XUNIT, data.m_x.GetUnit() ),

		k_v( ENTRY_Y, data.m_y.GetName() ),
		k_v( ENTRY_YDESCR, data.m_y.GetDescription() ),
		k_v( ENTRY_YUNIT, data.m_y.GetUnit() ),

		k_v( ENTRY_Z, data.m_z.GetName() ),
		k_v( ENTRY_ZDESCR, data.m_z.GetDescription() ),
		k_v( ENTRY_ZUNIT, data.m_z.GetUnit() )
	);

	if ( data.m_operation != nullptr )
	{
		WriteSection( path,

			k_v( ENTRY_OPNAME, data.m_operation->GetName() )
		);
	}

	WriteSection( path,

		k_v( ENTRY_GROUP, data.m_group ),
		k_v( ENTRY_CONTOUR, data.m_withContour ),
		k_v( ENTRY_SOLID_COLOR, data.m_withSolidColor ),
		k_v( ENTRY_EAST_COMPONENT, data.m_eastcomponent ),
		k_v( ENTRY_NORTH_COMPONENT, data.m_northcomponent ),
		k_v( ENTRY_INVERT_XYAXES, data.m_invertXYAxes )
	);

	if ( !isDefaultValue( data.m_minValue ) )
	{
		WriteSection( path,

			k_v( ENTRY_MINVALUE, CTools::Format( "%.15g", data.m_minValue ) )
		);
	}
	if ( !isDefaultValue( data.m_maxValue ) )
	{
		WriteSection( path,

			k_v( ENTRY_MAXVALUE, CTools::Format( "%.15g", data.m_maxValue ) )
		);
	}
	if ( !data.m_colorPalette.empty() )
	{
		// v3 note
		// if color palette is a file (it has an extension)
		// save path in relative form, based on workspace Display path
		std::string paletteToWrite = data.m_colorPalette;
		// TODO !!! test !!! this
		if ( !GetLastExtensionFromPath( data.m_colorPalette ).empty() )
		{
			if ( wksd != nullptr )
			{
				paletteToWrite = GetRelativePath( wksd->GetPath(), data.m_colorPalette );
			}
		}

		WriteSection( path,

			k_v( ENTRY_COLOR_PALETTE, paletteToWrite )
		);
	}

	if ( !data.m_xAxis.empty() )
	{
		WriteSection( path,

			k_v( ENTRY_X_AXIS, data.m_xAxis )
		);
	}

	return Status() == QSettings::NoError;
}
bool CWorkspaceSettings::LoadConfig( CDisplayData &data, const std::string& path, CWorkspaceDisplay *wks, CWorkspaceOperation *wkso )
{
	std::string 
		type, field_name, field_description, unit, 
		entry_x, entry_x_desc, entry_x_unit,
		entry_y, entry_y_desc, entry_y_unit,
		entry_z, entry_z_desc, entry_z_unit,
		op_name;

	ReadSection( path,

		k_v( ENTRY_TYPE,			&type,						CMapTypeDisp::GetInstance().IdToName( data.m_type ) ),
		k_v( ENTRY_FIELD,			&field_name					),
		k_v( ENTRY_FIELDNAME,		&field_description			),		// v4: yes, not a mistake, the key retrieves a value for setDescription
		k_v( ENTRY_UNIT,			&unit						),

		k_v( ENTRY_X,				&entry_x					),
		k_v( ENTRY_XDESCR,			&entry_x_desc				),
		k_v( ENTRY_XUNIT,			&entry_x_unit				),

		k_v( ENTRY_Y,				&entry_y					),
		k_v( ENTRY_YDESCR,			&entry_y_desc				),
		k_v( ENTRY_YUNIT,			&entry_y_unit				),

		k_v( ENTRY_Z,				&entry_z					),
		k_v( ENTRY_ZDESCR,			&entry_z_desc				),
		k_v( ENTRY_ZUNIT,			&entry_z_unit				),

		k_v( ENTRY_OPNAME,			&op_name					),

		k_v( ENTRY_GROUP,			&data.m_group,				1 ),
		k_v( ENTRY_CONTOUR,			&data.m_withContour,		false  ),
		k_v( ENTRY_SOLID_COLOR,		&data.m_withSolidColor,		true  ),

		k_v( ENTRY_EAST_COMPONENT,	&data.m_eastcomponent,		false  ),
		k_v( ENTRY_NORTH_COMPONENT, &data.m_northcomponent,		false  ),

		k_v( ENTRY_INVERT_XYAXES,	&data.m_invertXYAxes,		false  ),

		k_v( ENTRY_MINVALUE,		&data.m_minValue,			defaultValue<double>()  ),
		k_v( ENTRY_MAXVALUE,		&data.m_maxValue,			defaultValue<double>()  ),

		k_v( ENTRY_COLOR_PALETTE,	&data.m_colorPalette		),

		k_v( ENTRY_X_AXIS,			&data.m_xAxis				)
	);

	if ( type.empty() )	{
		data.m_type = CMapTypeDisp::Invalid();
	}
	else {
		// Because Z=F(Lat,Lon) have been changed to Z=F(Lon,Lat)	(v3.3.0 note)
		//
		if ( str_icmp( type, OLD_VALUE_DISPLAY_eTypeZFLatLon ) )
			data.m_type = CMapTypeDisp::eTypeDispZFLatLon;
		else
			data.m_type = CMapTypeDisp::GetInstance().NameToId( type );
	}

	data.m_field.SetName( field_name );
	data.m_field.SetDescription( field_description );
	data.m_field.SetUnit( unit );

	data.m_x.SetName( entry_x );
	data.m_x.SetDescription( entry_x_desc );
	data.m_x.SetUnit( entry_x_unit );

	data.m_y.SetName( entry_y );
	data.m_y.SetDescription( entry_y_desc );
	data.m_y.SetUnit( entry_y_unit );

	data.m_z.SetName( entry_z );
	data.m_z.SetDescription( entry_z_desc );
	data.m_z.SetUnit( entry_z_unit );
	
	data.m_operation = wkso->GetOperation( op_name );

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

	// TODO !!! test !!! this
	if ( !GetLastExtensionFromPath( data.m_colorPalette ).empty() )
	{
		if ( wks != nullptr )
			data.m_colorPalette = NormalizedAbsolutePath( data.m_colorPalette, wks->GetPath() );		// TODO !!! test !!! this
	}

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
