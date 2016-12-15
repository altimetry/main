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

#include "common/tools/Trace.h"
#include "common/tools/Exception.h"

#include "libbrathl/brathl.h"
#include "libbrathl/Tools.h"
#include "process/BratProcess.h"

using namespace brathl;
using namespace processes;

#include "../Filters/BratFilters.h"
#include "Workspace.h"
#include "Operation.h"
#include "RadsDataset.h"
#include "WorkspaceSettings.h"



class COperationCmdFile : CCmdFile
{
	const COperation &mOp;

public:
	COperationCmdFile( const std::string &path, const COperation &Op ) :
		CCmdFile( path ), mOp( Op )
	{}

	virtual ~COperationCmdFile()
	{}


	///////////////////////////////////////////////////

	bool BuildCmdFileHeader()
	{
		// Comment("!/usr/bin/env " + GetSystemCommand()->GetName());
		Comment( "Type:" + CMapTypeOp::GetInstance().IdToName( mOp.GetType() ) );
		return true;
	}

	bool BuildCmdFileVerbose( bool write_log_file = true )
	{
		WriteLn();
		Comment( "----- LOG -----" );
		WriteLn();
		WriteLn( kwVERBOSE + "=" + n2s<std::string>( mOp.m_verbose ) );

		if ( write_log_file && !mOp.GetLogFile().empty() )
		{
			WriteLn( kwLOGFILE + "=" + mOp.GetLogFile() );
		}
		return true;
	}


	bool BuildCmdFileGeneralProperties()
	{
		WriteLn();
		Comment( "----- GENERAL PROPERTIES -----" );
		WriteLn();
		// Data mode is no more for a operation, but for each formula (data expression)
		//WriteLine(std::string::Format("DATA_MODE=%s", CMapDataMode::GetInstance().IdToName(m_dataMode).c_str()));

		return true;
	}

	bool BuildCmdFileAlias( CWorkspaceFormula *wks )
	{
		//CWorkspaceFormula* wks = wxGetApp().GetCurrentWorkspaceFormula();
		if ( wks == nullptr )
			return false;

		WriteLn();
		Comment( "----- ALIAS -----" );

		CMapFormula* formulas = wks->GetFormulas();
		for ( CMapFormula::iterator it = formulas->begin(); it != formulas->end(); it++ )
		{
			CFormula* value = dynamic_cast<CFormula*>( it->second );
			if ( value == nullptr )
				continue;
			WriteLn( kwALIAS_NAME  + "=" + value->GetName() );
			WriteLn( kwALIAS_VALUE + "=" + value->GetDescription( true ) );
		}

		return true;
	}

	bool BuildCmdFileDataset()
	{
		assert__( mOp.mFilteredDataset );

		WriteLn();
		Comment( "----- DATASET -----" );
		WriteLn();
		WriteLn( kwRECORD + "=" + mOp.GetRecord() );
		WriteLn();
		std::vector< std::string > array = mOp.mFilteredDataset->GetFiles( false );
		for ( size_t i = 0; i < array.size(); i++ )
		{
			WriteLn( kwFILE + "=" + array[ i ] );
		}
		return true;
	}

	bool BuildCmdFileSpecificUnit()
	{
		assert__( mOp.mFilteredDataset );

		const CDataset* dataset = mOp.mFilteredDataset;
		if ( dataset == nullptr )
			return false;

		WriteLn();
		Comment( "----- SPECIFIC UNIT -----" );

		const CStringMap* fieldSpecificUnit = dataset->GetFieldSpecificUnits();
		for ( CStringMap::const_iterator itMap = fieldSpecificUnit->begin(); itMap != fieldSpecificUnit->end(); itMap++ )
		{
			WriteLn( kwUNIT_ATTR_NAME + "=" + itMap->first );
			WriteLn( kwUNIT_ATTR_VALUE + "=" + itMap->second );
		}

		return true;
	}

	bool BuildCmdFileFieldsSpecificZFXY( CFormula* value, std::string &error_msg )
	{
		// if operation is not X/Y as Lat/Lon or Lon/Lat, force to default value
		/*
		if (!IsMap())
		{
		value->SetFilterDefault();
		value->SetMinValueDefault();
		value->SetMaxValueDefault();
		value->SetIntervalDefault();
		value->SetLoessCutOffDefault();
		}
		*/
		;

		if ( mOp.GetType() != CMapTypeOp::eTypeOpZFXY )
			return false;

		if ( value == nullptr )
			return false;

		std::string valueString = ( value->GetFilter() == CMapTypeFilter::eFilterNone ) ? "DV" : value->GetFilterAsString();
		WriteLn( value->GetFieldPrefix() + "_FILTER=" + valueString );

		if ( value->IsXYType() )
		{
			value->ComputeInterval( error_msg );

			if ( value->IsTimeDataType() )
			{
				valueString = ( isDefaultValue( value->GetMinValue() ) ) ? "DV" : value->GetMinValueAsDateString();
				WriteLn( value->GetFieldPrefix() + "_MIN=" + valueString );

				valueString = ( isDefaultValue( value->GetMaxValue() ) ) ? "DV" : value->GetMaxValueAsDateString();
				WriteLn( value->GetFieldPrefix() + "_MAX=" + valueString );
			}
			else
			{
				valueString = ( isDefaultValue( value->GetMinValue() ) ) ? "DV" : value->GetMinValueAsText();
				WriteLn( value->GetFieldPrefix() + "_MIN=" + valueString );

				valueString = ( isDefaultValue( value->GetMaxValue() ) ) ? "DV" : value->GetMaxValueAsText();
				WriteLn( value->GetFieldPrefix() + "_MAX=" + valueString );
			}

			valueString = ( isDefaultValue( value->GetInterval() ) ) ? "DV" : value->GetIntervalAsText();
			WriteLn( value->GetFieldPrefix() + "_INTERVALS=" + valueString );

			valueString = ( isDefaultValue( value->GetLoessCutOff() ) ) ? "DV" : value->GetLoessCutOffAsText();
			WriteLn( value->GetFieldPrefix() + "_LOESS_CUTOFF=" + valueString );
		}

		return true;
	}

	bool BuildCmdFileFields( std::string &error_msg )
	{
		WriteLn();
		Comment( "----- FIELDS -----" );
		WriteLn();

		const CMapFormula &formulas = *mOp.GetFormulas();
		for ( CMapFormula::const_iterator it = formulas.begin(); it != formulas.end(); it++ )
		{
			CFormula *value = dynamic_cast<CFormula*>( it->second );
			if ( value == nullptr )
				continue;

			WriteLn();
			Comment( value->GetComment( true ) );
			WriteLn( value->GetFieldPrefix() + "=" + value->GetDescription( true ) );
			WriteLn( value->GetFieldPrefix() + "_NAME=" + value->GetName() );
			WriteLn( value->GetFieldPrefix() + "_TYPE=" + value->GetDataTypeAsString() );
			WriteLn( value->GetFieldPrefix() + "_UNIT=" + value->GetUnitAsText() );

			if ( value->IsFieldType() )
			{
				WriteLn( value->GetFieldPrefix() + DATA_MODE_SUFFIX +				"=" + value->GetDataModeAsString() );
				WriteLn( value->GetFieldPrefix() + DATA_MODE_DI_TIME_NAME_SUFFIX +	"=" + value->DataModeDITimeName() );
				WriteLn( value->GetFieldPrefix() + DATA_MODE_DI_DATE_TIME_SUFFIX +	"=" + value->DataModeDIDateTime().AsString() );
			}

			std::string valueString = ( value->GetTitle().empty() ) ? value->GetName() : value->GetTitle();
			WriteLn( value->GetFieldPrefix() + "_TITLE=" + valueString );

			valueString = ( value->GetComment().empty() ) ? "DV" : value->GetComment();
			WriteLn( value->GetFieldPrefix() + "_COMMENT=" + valueString );

			BuildCmdFileFieldsSpecificZFXY( value, error_msg );
		}

		return true;
	}

	bool BuildCmdFileSelect( const CProductInfo &pi )
	{
		WriteLn();
		Comment( "----- SELECT -----" );
		WriteLn();

        ///// RCCC TODO /////////////////////////////////////////////////
        std::string CritExpression; // Selection criteria expression

        // Add user expression
        CritExpression += mOp.GetSelect()->GetDescription( true );

        // Add filter expression
        if ( mOp.Filter() )
        {
            CritExpression.empty() ? CritExpression += "" : CritExpression += " && ";
            CritExpression += mOp.Filter()->GetSelectionCriteriaExpression( pi.Label() );
        }
        /////////////////////////////////////////////////////////////////

        if ( !CritExpression.empty() ) /// RCCC TODO: // ( !mOp.GetSelect()->GetDescription( true ).empty() )
		{
            WriteLn( std::string( kwSELECT.c_str() ) + "=" + CritExpression ); /// RCCC TODO: // + mOp.GetSelect()->GetDescription( true )
		}
		return true;
	}

	bool BuildCmdFileOutput()
	{
		WriteLn();
		Comment( "----- OUTPUT -----" );
		WriteLn();
		//if ( m_output.GetFullPath().IsEmpty() )
		//{
		//	InitOutput();
		//}
		WriteLn( kwOUTPUT + "=" + mOp.GetOutputPath() );

		return true;
	}

	bool BuildExportCmdFileHeader()
	{
		// Comment("!/usr/bin/env " + GetExportAsciiSystemCommand()->GetName());
		return true;
	}

	bool BuildExportAsciiCmdFileGeneralProperties()
	{
		WriteLn();
		Comment( "----- GENERAL PROPERTIES -----" );
		WriteLn();

		std::string valueString = ( mOp.IsExportAsciiDateAsPeriod() ? "Yes" : "No" );

		WriteLn( std::string( kwDATE_AS_PERIOD.c_str() ) + "=" + valueString );

		valueString = ( mOp.IsExportAsciiExpandArray() ? "Yes" : "No" );

		WriteLn( std::string( kwEXPAND_ARRAY.c_str() ) + "=" + valueString );

		return true;
	}

	bool BuildExportGeoTIFFProperties( const CProductInfo &pi )
	{
		assert__( pi.IsValid() );

		WriteLn();
		Comment( "----- INPUT -----" );
		WriteLn();
		WriteLn( kwFILE + "=" + mOp.GetOutputPath() );
		WriteLn();
		Comment( "----- COLORTABLE -----" );
		WriteLn();
		WriteLn( kwDISPLAY_COLORTABLE + "=" + mOp.mColorTable );

#if defined(BRAT_V3)
		if ( !isDefaultValue( mOp.mGeoTIFFRangeMin ) )
		{
			WriteLn( kwDISPLAY_MINVALUE + "=" + n2s<std::string>( mOp.mGeoTIFFRangeMin ) );
		}
		if ( !isDefaultValue( mOp.mGeoTIFFRangeMax ) )
		{
			WriteLn( kwDISPLAY_MAXVALUE + "=" + n2s<std::string>( mOp.mGeoTIFFRangeMax ) );
		}
#endif
		if ( mOp.mCreateKMLFieldsData || mOp.mCreateKMLTrackData )
		{
			std::string kmlOutputFile = mOp.GetExportGeoTIFFOutputPath();
			SetFileExtension( kmlOutputFile, std::string( "kml" ) );

			WriteLn();
			Comment( "----- GOOGLE EARTH -----" );
			WriteLn();
			WriteLn( kwOUTPUT_KML + "=" + kmlOutputFile );
			WriteLn( kwDISPLAY_LOGO_URL + "=" + mOp.mBratLogoPath );
			//v4: the following does not seem to be written in v3 when delaying
			WriteLn( kwFILETYPE + "=" + pi.Class() + " / " + pi.Type() );
			WriteLn( kwPRODUCT_LIST + "=" + mOp.OriginalDataset()->ToString( ", " ) );

			if ( mOp.mCreateKMLTrackData )
				WriteLn( kwEXPORT_KML_TRACKS + "=Y" );

			if ( mOp.mCreateKMLFieldsData )
				WriteLn( kwEXPORT_KML_FIELDS + "=Y" );
		}

		return true;
	}

	bool BuildCmdFileFromOutputOperation()
	{
		WriteLn();
		Comment( "----- DATASET -----" );
		WriteLn();
		WriteLn( std::string( kwRECORD.c_str() ) + "=" + CProductNetCdf::m_virtualRecordName.c_str() );
		WriteLn();
		WriteLn( std::string( kwFILE.c_str() ) + "=" + mOp.GetOutputPath() );

		return true;
	}

	bool BuildExportAsciiCmdFileFields()
	{
		WriteLn();
		Comment( "----- FIELDS -----" );
		WriteLn();

		auto formulas = mOp.GetFormulas();
		for ( CMapFormula::const_iterator it = formulas->begin(); it != formulas->end(); it++ )
		{
			CFormula* value = dynamic_cast<CFormula*>( it->second );
			if ( value == nullptr )
			{
				continue;
			}
			WriteLn();
			Comment( value->GetComment( true ) );

			// If export operation  is not a dump export,
			// Field name in not the field name of the source data file
			// but the field name of the operation result file (intermediate netcdf file).
			if ( mOp.IsExportAsciiNoDataComputation() )
			{
				// Just a dump --> get the name of the source file
				WriteLn( value->GetExportAsciiFieldPrefix() + "=" + value->GetDescription( true ) );
			}
			else
			{
				// Not a dump --> get the name of operation file (intermediate netcdf file).
				WriteLn( value->GetExportAsciiFieldPrefix() + "=" + value->GetName() );
			}

			WriteLn( value->GetExportAsciiFieldPrefix() + "_NAME=" + value->GetName() );
			WriteLn( value->GetExportAsciiFieldPrefix() + "_UNIT=" + value->GetUnitAsText() );

			if ( ! isDefaultValue( mOp.GetExportAsciiNumberPrecision() ) )
			{
				WriteLn( value->GetExportAsciiFieldPrefix() + "_FORMAT=" + n2s<std::string>( mOp.GetExportAsciiNumberPrecision() ) );
			}
		}

		return true;
	}

	bool BuildExportAsciiCmdFileOutput()
	{
		WriteLn();
		Comment( "----- OUTPUT -----" );
		WriteLn();
		//if ( mOp.GetExportAsciiOutput()->GetFullPath().IsEmpty() )
		//{
		//	InitExportAsciiOutput();
		//}
		WriteLn( kwOUTPUT + "=" + mOp.GetExportAsciiOutputPath() );

		return true;

	}

	bool BuildExportGeoTIFFCmdFileOutput()
	{
		if ( !mOp.mCreateGeoTIFFs )
			return true;

		WriteLn();
		Comment( "----- OUTPUT -----" );
		WriteLn();
		WriteLn( kwOUTPUT + "=" + mOp.GetExportGeoTIFFOutputPath() );

		return true;
	}

	bool BuildShowStatsCmdFileHeader()
	{
		//  WriteComment("!/usr/bin/env " + GetShowStatsSystemCommand()->GetName());
		return true;
	}

	bool BuildShowStatsCmdFileFields()
	{
		WriteLn();
		Comment( "----- FIELDS -----" );
		WriteLn();

		auto formulas = mOp.GetFormulas();
		for ( CMapFormula::const_iterator it = formulas->begin(); it != formulas->end(); it++ )
		{
			CFormula* value = dynamic_cast<CFormula*>( it->second );
			if ( value == nullptr )
				continue;

			if ( value->GetFieldType() != CMapTypeField::eTypeOpAsField )
				continue;

			WriteLn();
			Comment( value->GetComment( true ) );
			WriteLn( value->GetExportAsciiFieldPrefix() + "=" + value->GetDescription( true ) );
			//WriteLine(value->GetExportAsciiFieldPrefix()  + "_NAME=" + value->GetName());
			WriteLn( value->GetExportAsciiFieldPrefix() + "_UNIT=" + value->GetUnitAsText() );
		}

		return true;
	}

	bool BuildShowStatsCmdFileOutput()
	{
		WriteLn();
		Comment( "----- OUTPUT -----" );
		WriteLn();
		//if ( m_showStatsOutput.GetFullPath().IsEmpty() )
		//{
		//	InitShowStatsOutput();
		//}
		WriteLn( kwOUTPUT + "=" + mOp.GetShowStatsOutputPath() );

		return true;
	}


	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////


public:

	static bool BuildCmdFile( const std::string &path, const COperation &Op, CWorkspaceFormula *wks, std::string &error_msg, const CProductInfo &pi )
	{
		COperationCmdFile f( path, Op );
		//m_cmdFile.Normalize();
		//m_file.Create( m_cmdFile.GetFullPath(), true, wxS_DEFAULT | wxS_IXUSR );

		return 
			f.IsOk()							&&
			f.BuildCmdFileHeader()				&&
			f.BuildCmdFileVerbose()				&&
			f.BuildCmdFileGeneralProperties()	&&
			f.BuildCmdFileAlias( wks )			&&
			f.BuildCmdFileDataset()				&&
			f.BuildCmdFileSpecificUnit()		&&
			f.BuildCmdFileFields( error_msg )	&&
			f.BuildCmdFileSelect( pi )			&&
			f.BuildCmdFileOutput()				&&
			f.IsOk();
	}


	static bool BuildExportAsciiCmdFile( const std::string &path, const COperation &Op, CWorkspaceFormula *wks, const CProductInfo &pi )
	{
		COperationCmdFile f( path, Op );
		//m_exportAsciiCmdFile.Normalize();
		//m_file.Create( m_exportAsciiCmdFile.GetFullPath(), true, wxS_DEFAULT | wxS_IXUSR );

		return
			f.IsOk()																								&&
			f.BuildExportCmdFileHeader()																			&&
			f.BuildCmdFileVerbose()																					&&
			f.BuildExportAsciiCmdFileGeneralProperties()															&&
			f.BuildCmdFileAlias( wks )																				&&
			( Op.IsExportAsciiNoDataComputation() ? f.BuildCmdFileDataset() : f.BuildCmdFileFromOutputOperation() )	&&
			f.BuildCmdFileSpecificUnit()																			&&
			f.BuildExportAsciiCmdFileFields()																		&&
			// If Export ascii is done from the output operation file, 
			// don't include the select expression in the command line,
			// because the select expression has already been applied 
			// and the fields contained in the select expression are not necessarily in the output file. 
			( !Op.IsExportAsciiNoDataComputation() || f.BuildCmdFileSelect( pi ) )									&&
			f.BuildExportAsciiCmdFileOutput()																		&&
			f.IsOk();
	}


    static bool BuildExportGeoTIFFCmdFile( const std::string &path, const COperation &Op, const CProductInfo &pi )
	{
		COperationCmdFile f( path, Op );

		return
			f.IsOk() &&
			f.BuildExportCmdFileHeader()			&&
			f.BuildCmdFileVerbose( false )			&&
			f.BuildExportGeoTIFFProperties( pi )	&&
			f.BuildExportGeoTIFFCmdFileOutput()		&&
			f.IsOk();
	}


	static bool BuildShowStatsCmdFile( const std::string &path, const COperation &Op, CWorkspaceFormula *wks, const CProductInfo &pi )
	{
		COperationCmdFile f( path, Op );
		//m_showStatsCmdFile.Normalize();
		//m_file.Create( m_showStatsCmdFile.GetFullPath(), true, wxS_DEFAULT | wxS_IXUSR );

		return
			f.IsOk()							&&
			f.BuildShowStatsCmdFileHeader()		&&
			f.BuildCmdFileVerbose()				&&
			f.BuildCmdFileAlias( wks )			&&
			f.BuildCmdFileDataset()				&&
			f.BuildCmdFileSpecificUnit()		&&
			f.BuildShowStatsCmdFileFields()		&&
			f.BuildCmdFileSelect( pi )			&&
			f.BuildShowStatsCmdFileOutput()		&&
			f.IsOk();
	}
};



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





//static 
std::string COperation::m_execYFXName;
//static 
std::string COperation::m_execZFXYName;
//static 
std::string COperation::m_execExportAsciiName;
//static 
std::string COperation::m_execExportGeoTiffName;
//static 
std::string COperation::m_execShowStatsName;
//static 
std::string COperation::m_execBratSchedulerName;


//static
void COperation::SetExecNames( const CApplicationPaths &app_path )
{
    m_execYFXName = app_path.mExecYFXName;

    m_execZFXYName = app_path.mExecZFXYName;

    m_execExportAsciiName = app_path.mExecExportAsciiName;

    m_execExportGeoTiffName = app_path.mExecExportGeoTiffName;

    m_execShowStatsName = app_path.mExecShowStatsName;

    m_execBratSchedulerName = app_path.mExecBratSchedulerName;

    assert__(
        IsFile( m_execYFXName )				&&
        IsFile( m_execZFXYName )			&&
        IsFile( m_execExportAsciiName )		&&
        IsFile( m_execExportGeoTiffName )	&&
        IsFile( m_execShowStatsName )		&&
        IsFile( m_execBratSchedulerName )
        );
}




//static 
COperation* COperation::Copy( const COperation &o, CWorkspaceOperation *wkso, CWorkspaceDataset *wksd )
{
	COperation *new_op = new COperation( o.m_name );
	
	assert__( new_op->mFilteredDataset == nullptr && new_op->mOriginalDataset == nullptr );

	//assuming original operation is well formed, no reason to check filter and dataset assignment
	std::string error_msg;
	//if ( o.mDataset != nullptr )
	//{
		assert__( o.OriginalDataset() != nullptr && new_op->Filter() == nullptr );

        new_op->SetOriginalDataset( wksd, o.OriginalDatasetName(), error_msg );	//NOTE: this clears formulas and can clear filter, don't assign them before
	//}
	new_op->SetFilter( o.Filter(), error_msg );

	new_op->SetRecord( o.m_record );
	new_op->CopyFilteredDatasetAndProduct( o );	//new_op->SetProduct( o.m_product );


	assert__( new_op->mSelectionCriteria && o.mSelectionCriteria );

	*new_op->mSelectionCriteria = *o.mSelectionCriteria;


	new_op->m_formulas = o.m_formulas;

	new_op->m_type = o.m_type;
	new_op->m_dataMode = o.m_dataMode;
	new_op->m_exportAsciiDateAsPeriod = o.m_exportAsciiDateAsPeriod;

	new_op->InitOutputs( wkso );				//assigns all output and cmd files
	//new_op->InitExportGeoTIFFOutput( wkso );	//assigns m_exportGetTIFFOutput and m_exportGetTIFFCmdFile
	//new_op->InitExportAsciiOutput( wkso );	//assigns m_exportAsciiOutput and m_exportAsciiCmdFile
	new_op->SetCmdFile( wkso );					//assigns m_cmdFile
	new_op->SetExportAsciiCmdFile( wkso );		//assigns m_exportAsciiCmdFile
	new_op->SetExportGeoTIFFCmdFile( wkso );	//assigns m_exportGetTIFFCmdFile
	new_op->SetShowStatsCmdFile( wkso );

	return new_op;

	// TODO not assigned: check why and consequences

	//const int m_verbose = 2;

	//std::string m_logFile;

	//bool m_exportAsciiExpandArray = false;
	//bool m_exportAsciiNoDataComputation = false;
	//bool m_executeAgain = false;

	//int m_exportAsciiNumberPrecision = defaultValue<int>();
}


void COperation::Clear()
{
	//m_product = nullptr;
	RemoveOriginalDataset();		//mOriginalDataset = nullptr && ReomveFormulas && RemoveFilteredDataset	
	RemoveFilter();					//also RemoveFilteredDataset
	m_record.clear();

	CreateSelectionCriteria();

	m_formulas.clear();
	m_type = CMapTypeOp::eTypeOpYFX;
	m_dataMode = CMapDataMode::GetInstance().GetDefault();

	m_output.clear();
	m_exportAsciiOutput.clear();
	m_exportNetcdfOutput.clear();
	m_exportGeoTIFFOutput.clear();
	m_showStatsOutput.clear();

	m_cmdFile.clear();;
	m_exportAsciiCmdFile.clear();
	m_exportGeoTIFFCmdFile.clear();
	m_showStatsCmdFile.clear();

	m_logFile.clear();

	m_exportAsciiDateAsPeriod = false;
	m_exportAsciiExpandArray = false;
	m_exportAsciiNoDataComputation = false;
	m_executeAgain = false;

	m_exportAsciiNumberPrecision = defaultValue<int>();
}


//----------------------------------------
bool COperation::IsSelect( const CFormula* value ) const
{
	// if same pointer --> return
	return mSelectionCriteria == value;
}
//----------------------------------------
bool COperation::IsZFXY() const
{
	return ( m_type == CMapTypeOp::eTypeOpZFXY );
}
//----------------------------------------
bool COperation::IsYFX() const
{
	return ( m_type == CMapTypeOp::eTypeOpYFX );
}
//----------------------------------------
bool COperation::IsMap() const
{
	if ( !IsZFXY() )
	{
		return false;
	}

	const CFormula* xFormula = GetFormula( CMapTypeField::eTypeOpAsX );
	const CFormula* yFormula = GetFormula( CMapTypeField::eTypeOpAsY );

	if ( ( xFormula == nullptr ) || ( yFormula == nullptr ) )
	{
		return false;
	}

	bool isMap = ( xFormula->IsLonDataType() && yFormula->IsLatDataType() ) ||
		( xFormula->IsLatDataType() && yFormula->IsLonDataType() );

	return isMap;
}

//----------------------------------------
void COperation::SetSelect( CFormula* value )
{
	//assert__( value );	//v3 implementation assumed it:
	//	apparently, cleaning a selection criteria did not use this function but instead:
	//		mSelectionCriteria->SetDescription( "" );

	// if same pointer --> return
	if ( IsSelect( value ) )
		return;

	CreateSelectionCriteria( value );
}

//----------------------------------------
void COperation::ClearLogFile()
{
	m_logFile.clear();
}
//----------------------------------------
void COperation::SetLogFile( const std::string& value )
{
	m_logFile = value;
}
//----------------------------------------
void COperation::SetLogFile( CWorkspaceOperation *wks )
{
	//CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();

	m_logFile = m_output;
	SetFileExtension( m_logFile, LOGFILE_EXTENSION );
	if ( wks == nullptr )
	{
		//m_logFile.Normalize();
	}
	else
	{
		m_logFile + wks->GetPath() + "/" + m_logFile;	//m_logFile.Normalize( wxPATH_NORM_ALL, wks->GetPath() );
	}
}

//----------------------------------------
bool COperation::CtrlLoessCutOff( std::string &msg )
{
  if (m_type != CMapTypeOp::eTypeOpZFXY)
  {
    return true;
  }

  CMapFormula::iterator it;

  bool hasCutOffX = false;
  bool hasCutOffY = false;
  bool hasFilter = false;
  std::string fields;

  for (it = m_formulas.begin() ; it != m_formulas.end() ; it++)
  {
    CFormula* value = dynamic_cast<CFormula*>(it->second);
    if (value == nullptr)
    {
      continue;
    }

    if (value->GetFieldType() == CMapTypeField::eTypeOpAsSelect)
    {
      continue;
    }

    if (value->GetFieldType() == CMapTypeField::eTypeOpAsField)
    {
      if ( (isDefaultValue(value->GetFilter()) == false) &&
        (value->GetFilter() != CMapTypeFilter::eFilterNone) )
      {
        hasFilter = true;
        fields += "\t'" + value->GetName() + "'";
      }
    }
    if (value->GetFieldType() == CMapTypeField::eTypeOpAsX)
    {
      if ( (isDefaultValue(value->GetLoessCutOff()) == false) &&
           (value->GetLoessCutOff() > 1) )
      {
        hasCutOffX = true;
      }
    }
    if (value->GetFieldType() == CMapTypeField::eTypeOpAsY)
    {
      if ( (isDefaultValue(value->GetLoessCutOff()) == false) &&
           (value->GetLoessCutOff() > 1) )
      {
        hasCutOffY = true;
      }
    }
  }

  if (hasFilter)
  {
    msg += 
		"At least a filter is defined for operation '"
		+ GetName()
		+ "'\non 'Data field(s)' \n"
		+ fields 
		+ "\n";

    if (hasCutOffX == false)
    {
      msg += "No 'Loess cut-off' is defined for X field (or 'Loess-cut-off value is <= 1)\n";
    }
    if (hasCutOffY == false)
    {
      msg += "No 'Loess cut-off' is defined for Y field (or 'Loess-cut-off value is <= 1)\n";
    }
  }
  /*
  else
  {
    msg += std::string::Format("No filter is defined on 'Data field(s)' for operation '%s'.\n",
                            GetName().c_str());
    if (hasCutOffX)
    {
      msg += "A 'Loess cut-off' is defined for X field.\n";
    }
    if (hasCutOffY)
    {
      msg += "A 'Loess cut-off' is defined for Y field.\n";
    }
  }
*/
  bool bOk = true;

  if (hasFilter)
  {
   bOk = hasCutOffX && hasCutOffY;
  }
  /*
  else
  {
   bOk = (!hasCutOffX) && (!hasCutOffY);
  }
  */

  return bOk;
}
//----------------------------------------
bool COperation::UseDataset( const std::string& name )
{
	if ( mOriginalDataset == nullptr )
		return false;

	return str_icmp( mOriginalDataset->GetName(), name );
}

//----------------------------------------
std::string COperation::GetFormulaNewName() const
{
	size_t i = m_formulas.size();
	std::string key;
	do
	{
		key = WKS_EXPRESSION_NAME + "_" + n2s<std::string>( i + 1 );
		i++;
	} 
	while ( m_formulas.Exists( key ) != nullptr );

	return key;
}
//----------------------------------------
std::string COperation::GetFormulaNewName( const std::string& prefix ) const
{
	if ( m_formulas.Exists( prefix ) == nullptr )
		return prefix;

	int i = 1;
	std::string key;
	do
	{
		key = prefix + "_" + n2s<std::string>( i );
		i++;
	} 
	while ( m_formulas.Exists( key ) != nullptr );

	return key;
}


//----------------------------------------
bool COperation::RenameFormula(CFormula* formula, const std::string &newName)
{
 bool bOk = m_formulas.RenameKey(formula->GetName(), newName);
 if (bOk == false)
 {
   return false;
 }

 formula->SetName(newName);
 return true;
}


bool COperation::ReapplyFilter()
{ 
	return RemoveFilteredDataset();		// v4.0: SetFilteredDataset( error_msg, pi );
}


bool COperation::SetFilter( const CBratFilter *filter, std::string &error_msg )
{ 
	mFilter = filter; 
	return RemoveFilteredDataset();;		// SetFilteredDataset( error_msg, pi );
}

// Always succeeds
//
void COperation::RemoveFilter()
{
	mFilter = nullptr; 
	std::string error_msg;
	RemoveFilteredDataset();	// SetFilteredDataset( error_msg, nullptr );
}


bool COperation::IsRadsDataset() const
{
	return dynamic_cast< const CRadsDataset* >( mOriginalDataset ) != nullptr;
}


// Can return true and null dataset, if original dataset is empty
// Can return false and valid dataset, if filter could not be applied
//
std::pair< bool, const CDataset* > COperation::FilteredDataset( std::string &error_msg, CProgressInterface *progress )
{ 
	bool result = mFilteredDataset ? true : CreateFilteredDataset( error_msg, progress );

	return { result, mFilteredDataset };
}


CDataset* COperation::NewFilteredDataset() const
{
	assert__( mOriginalDataset );

	const bool is_rads_dataset = dynamic_cast< const CRadsDataset* >( mOriginalDataset ) != nullptr;
	auto const filtered_name = OriginalDatasetName() + "_filtered_" + m_name;
	CDataset *filtered_dataset = 
		is_rads_dataset ? 
		new CRadsDataset( filtered_name ) : 
		new CDataset( filtered_name ) ;

	filtered_dataset->GetProductList()->mCodaProductClass = mOriginalDataset->GetProductList()->mCodaProductClass;
	filtered_dataset->GetProductList()->mCodaProductType = mOriginalDataset->GetProductList()->mCodaProductType;
	filtered_dataset->GetProductList()->m_productClass = mOriginalDataset->GetProductList()->m_productClass;
	filtered_dataset->GetProductList()->m_productType = mOriginalDataset->GetProductList()->m_productType;
	filtered_dataset->GetProductList()->m_productFormat = mOriginalDataset->GetProductList()->m_productFormat;

	return filtered_dataset;
}

void COperation::CopyFilteredDatasetAndProduct( const COperation &o )
{
	assert__( mOriginalDataset == o.mOriginalDataset );

	RemoveFilteredDataset();

	if ( o.mFilteredDataset )
	{
		//copy filtered dataset

		mFilteredDataset = NewFilteredDataset();
		*mFilteredDataset->GetProductList() = *o.mFilteredDataset->GetProductList();	//copy and not simply insert files to preserve class and type

		//copy filtered product
		//	- absorb any errors; client code will have to rebuild the product when required 
		//	in a more explicit place to publicize errors (mFilteredProduct is an "on demand" variable,
		//	it is kept, as a cache, only for efficiency reasons)
		//
		if ( o.mFilteredProduct )
		try
		{
			mFilteredProduct = CProduct::Construct( *o.mFilteredDataset->GetProductList() );
		}
		catch ( ... )
		{
			mFilteredProduct = nullptr;
		}
	}
}

bool COperation::RemoveFilteredDataset()
{
	delete mFilteredDataset;
	mFilteredDataset = nullptr;
	delete mFilteredProduct; 
	mFilteredProduct = nullptr; 

	return true;
}

bool COperation::CreateFilteredDataset( std::string &error_msg, CProgressInterface *progress )
{
	delete mFilteredDataset;
	if ( !mOriginalDataset )
	{
		mFilteredDataset = nullptr;
		return true;
	}

	mFilteredDataset = NewFilteredDataset();

	bool result = mFilteredDataset->ApplyFilter( mFilter, mOriginalDataset, error_msg, progress );
	if ( !result )
	{
		error_msg = "Filter '" + mFilter->Name() + "' could not be applied and was removed from operation '" + GetName() + "'.\nReason: " + error_msg;
		mFilter = nullptr;
	}

	return result;
}


bool COperation::SetOriginalDataset( const CWorkspaceDataset *wks, const std::string dataset_name, std::string &error_msg ) 
{ 
	const CDataset *dataset = wks ? wks->GetDataset( dataset_name ) : nullptr;			assert__( !wks || dataset );
	RemoveFormulas();
	mOriginalDataset = dataset;
	return RemoveFilteredDataset();	// SetFilteredDataset( error_msg, pi );
}


void COperation::RemoveOriginalDataset()
{
	std::string error_msg;
	SetOriginalDataset( nullptr, "", error_msg );
}


void COperation::RemoveFormulas()
{
	while ( GetFormulaCount() > 0 )
		DeleteFormula( GetFormulas()->begin()->first );
}


//----------------------------------------
std::string COperation::FilterName() const
{
	return mFilter == nullptr ? "" : mFilter->Name();
}

std::string COperation::OriginalDatasetName() const
{
	return mOriginalDataset == nullptr ? "" : mOriginalDataset->GetName();
}

//----------------------------------------
CFormula* COperation::NewUserFormula( std::string &error_msg, CField* field, CMapTypeField::ETypeField typeField, bool addToMap, const CProductInfo &pi )	//pi = CProductInfo::smInvalidProduct
{
  if (field == nullptr)
  {
    return nullptr;

  }

  CFormula* formula =  NewUserFormula( error_msg, field->GetName(), typeField, field->GetUnit(), addToMap, pi );

  //formula->SetUnit(field->GetUnit().c_str());
  if (m_record.compare(field->GetRecordName().c_str()) != 0)
  {
    formula->SetDescription(field->GetFullNameWithRecord());
  }
  else
  {
    formula->SetDescription(field->GetFullName());
  }

  return formula;
}
//----------------------------------------
CFormula* COperation::NewUserFormula( std::string &error_msg, const std::string& name, CMapTypeField::ETypeField typeField, const std::string& strUnit, 
	bool addToMap, const CProductInfo &pi )	//name = "", typeField = CMapTypeField::eTypeOpAsField, strUnit = "", addToMap = true, pi = CProductInfo::smInvalidProduct 
{
	bool bOk = true;

	CFormula* formulaToReturn = nullptr;

	std::string formulaName = name.empty() ? GetFormulaNewName() : GetFormulaNewName( name );

	CFormula formula( formulaName, false );
	formula.SetFieldType( typeField );
	formula.SetUnit( strUnit, error_msg, "", false );

	const CUnit* unit = formula.GetUnit();

	formula.SetDataType( typeField, *unit, pi );

	if ( strUnit.empty() )
	{
		formula.SetDefaultUnit();
	}

	formula.SetDefaultMinMax();

	if ( addToMap )
	{
		bOk = AddFormula( formula, error_msg );
		formulaToReturn = GetFormula( formulaName );
	}
	else
	{
		formulaToReturn = new CFormula( formula );
	}

	return formulaToReturn;
}
//----------------------------------------
bool COperation::AddFormula( CFormula& formula, std::string &error_msg )
{
	CFormula* value = dynamic_cast<CFormula*>( m_formulas.Exists( formula.GetName() ) );
	if ( value != nullptr )
	{
		return false;
	}

	m_formulas.InsertUserDefined_ReplacePredefinedNotAllowed( formula, error_msg );

	return true;
}

//----------------------------------------
bool COperation::DeleteFormula( const std::string& name )
{
	bool bOk = false;

	CFormula* formula = GetFormula( name );
	if ( formula != nullptr )
	{
		if ( IsSelect( formula ) )
		{
			formula->SetDescription( "" );
			bOk = true;
		}
		else
		{
			bOk = m_formulas.Erase( name );
		}
	}
	else if ( COperation::IsSelect( name ) )
	{
		mSelectionCriteria->SetDescription( "" );
		bOk = true;
	}

	return bOk;
}
//----------------------------------------
std::string COperation::GetCommentFormula(const std::string& name) const 
{
  return m_formulas.GetCommentFormula(name);
}
//----------------------------------------
void COperation::SetCommentFormula(const std::string &name, const std::string &value)
{
  m_formulas.SetCommentFormula(name, value);
}
//----------------------------------------
std::string COperation::GetDescFormula( const std::string& name, bool alias )
{
	return m_formulas.GetDescFormula( name, alias );
}
//----------------------------------------
CFormula* COperation::GetFormula( const std::string& name ) const
{
	return dynamic_cast<CFormula*>( m_formulas.Exists( name ) );
}
//----------------------------------------
CFormula* COperation::GetFormula( int type )
{
	return m_formulas.GetFormula( type );
}
//----------------------------------------
CFormula* COperation::GetFormula(CMapFormula::iterator it)
{
  return dynamic_cast<CFormula*>(it->second);
}
const CFormula* COperation::GetFormula( CMapFormula::const_iterator it ) const
{
  return dynamic_cast<const CFormula*>(it->second);
}

//----------------------------------------
size_t COperation::GetFormulaCountDataFields()
{
	return m_formulas.CountDataFields();
}
//----------------------------------------
std::string COperation::GetSelectName() const
{
	assert__( mSelectionCriteria );

	return mSelectionCriteria->GetName();
}
//----------------------------------------
std::string COperation::GetSelectDescription() const
{
	assert__( mSelectionCriteria );

	return mSelectionCriteria->GetDescription();
}

//v4: formerly static void CDisplay::GetDisplayType( const COperation* operation, CUIntArray& displayTypes, CInternalFiles** pf = nullptr );
//
std::vector< CMapTypeDisp::ETypeDisp > COperation::GetDisplayTypes( CInternalFiles** pf ) const		//CInternalFiles** pf = nullptr 
{
	std::vector< CMapTypeDisp::ETypeDisp > types;

	CInternalFiles* f = CInternalFiles::Create( GetOutputPath(), true, false );

	if ( CInternalFiles::IsYFXFile( f ) )
	{
		types.push_back( CMapTypeDisp::eTypeDispYFX );
	}

	if ( CInternalFiles::IsZFLatLonFile( f ) )
	{
		types.push_back( CMapTypeDisp::eTypeDispZFLatLon );
	}

	if ( CInternalFiles::IsZFXYFile( f ) )
	{
		types.push_back( CMapTypeDisp::eTypeDispZFXY );
	}

	if ( pf == NULL )
	{
		delete f;
		f = NULL;
	}
	else
	{
		*pf = f;
	}

	return types;
}


//----------------------------------------
bool COperation::SaveConfig( CWorkspaceSettings *config, const CWorkspaceOperation *wks ) const
{
	assert__( config );
	//return !config || config->SaveConfig( *this );

	return config->SaveConfig( *this, wks );
}
//----------------------------------------
bool COperation::LoadConfig( CWorkspaceSettings *config, std::string &error_msg, CWorkspaceDataset *wks, CWorkspaceOperation *wkso )
{
	return !config || config->LoadConfig( *this, error_msg, wks, wkso );
}

//----------------------------------------
CDataset* COperation::FindDataset(const std::string& datasetName, CWorkspaceDataset *wks )
{
  //CWorkspaceDataset* wks = wxGetApp().GetCurrentWorkspaceDataset();
  if (wks == nullptr)
  {
    return nullptr;
  }

  return wks->GetDataset(datasetName);
}
//----------------------------------------
const std::string& COperation::GetSystemCommand() const
{
	switch ( m_type )
	{
		case CMapTypeOp::eTypeOpYFX:
			return GetExecYFXName();
			break;
		case CMapTypeOp::eTypeOpZFXY:
			return GetExecZFXYName();
			break;
		default:
			return m_cmdFile;
			break;
	}
}
//----------------------------------------
const std::string& COperation::GetShowStatsSystemCommand() const
{
  return GetExecShowStatsName();
}
//----------------------------------------
const std::string& COperation::GetExportAsciiSystemCommand() const
{
  return GetExecExportAsciiName();
}
//----------------------------------------


std::string COperation::GetFullCmd() const
{
	//return "\"" + GetSystemCommand() + "\" \"" + GetCmdFile() + "\"";
	return "\"" + GetSystemCommand() + "\" " + std::string( IsRadsDataset() ? CHECKED_DATASET_CMD_LINE_PARAMETER + " " : "" ) + "\"" + GetCmdFile() + "\"";
}

std::string COperation::GetExportAsciiFullCmd() const
{
	return "\"" + GetExportAsciiSystemCommand() + "\" \"" + GetExportAsciiCmdFile() + "\"";
}

std::string COperation::GetExportGeoTIFFFullCmd() const
{
	return "\"" + GetExecExportGeoTiffName() + "\" \"" + GetExportGeoTIFFCmdFile() + "\"";
}

std::string COperation::GetShowStatsFullCmd() const
{
	return "\"" + GetShowStatsSystemCommand() + "\" \"" + GetShowStatsCmdFile() + "\"";
}




static const std::string NoName = "NoName";

//----------------------------------------
std::string COperation::GetTaskName() const
{
	return m_cmdFile.empty() ? NoName : GetFileName( m_cmdFile );
}
//----------------------------------------
std::string COperation::GetExportAsciiTaskName() const
{
	return m_exportAsciiCmdFile.empty() ? NoName : GetFileName( m_exportAsciiCmdFile );
}
std::string COperation::GetExportNetcdfTaskName() const
{
	return "NetcdfExport" + GetTaskName();
}
std::string COperation::GetExportGeoTIFFTaskName() const
{
	return m_exportGeoTIFFCmdFile.empty() ? NoName : GetFileName( m_exportGeoTIFFCmdFile );
}

//----------------------------------------
std::string COperation::GetShowStatsTaskName() const
{
	return m_showStatsCmdFile.empty() ? NoName : GetFileName( m_showStatsCmdFile );
}

//----------------------------------------
bool COperation::BuildCmdFile( CWorkspaceFormula *wks, CWorkspaceOperation *wkso, std::string &error_msg, CProgressInterface *progress )
{
	auto filtered_result = FilteredDataset( error_msg, progress );
	if ( !filtered_result.first )
		return false;

	assert__( mFilteredDataset );

	if ( m_output.empty() )
		InitOperationOutput( wkso );

	const CProductInfo pi( mFilteredDataset );
	if ( !pi.IsValid() )
	{
		error_msg = pi.ErrorMessages();
		return false;
	}

	return COperationCmdFile::BuildCmdFile( m_cmdFile, *this, wks, error_msg, pi );
}
//----------------------------------------
bool COperation::BuildExportAsciiCmdFile( CWorkspaceFormula *wks, CWorkspaceOperation *wkso, std::string &error_msg )
{
	if ( m_exportAsciiOutput.empty() )
		InitExportAsciiOutput( wkso );

	const CProductInfo pi( OriginalDataset() );
	if ( !pi.IsValid() )
	{
		error_msg = pi.ErrorMessages();
		return false;
	}

	if ( !COperationCmdFile::BuildExportAsciiCmdFile( m_exportAsciiCmdFile, *this, wks, pi ) )
	{
		error_msg = "There was an error composing the command file.\nOperation cannot be exported.";
		return false;
	}

	return true;
}
//----------------------------------------
bool COperation::BuildShowStatsCmdFile( CWorkspaceFormula *wks, CWorkspaceOperation *wkso, std::string &error_msg )
{
	if ( m_showStatsOutput.empty() )
		InitShowStatsOutput( wkso );

	const CProductInfo pi( OriginalDataset() );
	if ( !pi.IsValid() )
	{
		error_msg = pi.ErrorMessages();
		return false;
	}

	if ( !COperationCmdFile::BuildShowStatsCmdFile( m_showStatsCmdFile, *this, wks, pi ) )
	{
		error_msg = "There was an error composing the command file.\nStatistics cannot be computed.";
		return false;
	}

	return true;
}


bool COperation::BuildExportGeoTIFFCmdFile( CWorkspaceFormula *wks, CWorkspaceOperation *wkso, std::string &error_msg )
{
    UNUSED( wks );

	if ( m_exportGeoTIFFOutput.empty() )
		InitExportGeoTIFFOutput( wkso );

	const CProductInfo pi( OriginalDataset() );
	if ( !pi.IsValid() )
	{
		error_msg = pi.ErrorMessages();
		return false;
	}

    if ( !COperationCmdFile::BuildExportGeoTIFFCmdFile( m_exportGeoTIFFCmdFile, *this, pi ) )
	{
		error_msg = "There was an error composing the command file.\nOperation cannot be exported.";
		return false;
	}

	return true;
}



//----------------------------------------
void COperation::SetShowStatsOutput( const std::string& value, CWorkspaceOperation* wks )
{
	//CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();

	m_showStatsOutput = value;

	if ( wks == nullptr )
	{
		clean_path( m_showStatsOutput );
	}
	else
	{
		//m_showStatsOutput.Normalize( wxPATH_NORM_ALL, wks->GetPath() );
		normalize( m_showStatsOutput, wks->GetPath() );
	}

	SetShowStatsCmdFile( wks );
}
//----------------------------------------
void COperation::SetExportAsciiOutput( const std::string& value, CWorkspaceOperation* wks )
{
	//CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();

	m_exportAsciiOutput = value;

	if ( wks == nullptr )
	{
		clean_path( m_exportAsciiOutput );
	}
	else
	{
		//m_exportAsciiOutput.Normalize( wxPATH_NORM_ALL, wks->GetPath() );
		normalize( m_exportAsciiOutput, wks->GetPath() );
	}

	SetExportAsciiCmdFile( wks );
}

void COperation::SetExportNetcdfOutput( const std::string& value, CWorkspaceOperation* wks )
{
	m_exportNetcdfOutput = value;

	if ( wks == nullptr )
	{
		clean_path( m_exportNetcdfOutput );
	}
	else
	{
		normalize( m_exportNetcdfOutput, wks->GetPath() );
	}
}

void COperation::SetExportGeoTIFFOutput( const std::string& value, CWorkspaceOperation* wks )
{
	m_exportGeoTIFFOutput = value;

	if ( wks == nullptr )
	{
		clean_path( m_exportGeoTIFFOutput );
	}
	else
	{
		//m_exportAsciiOutput.Normalize( wxPATH_NORM_ALL, wks->GetPath() );
		normalize( m_exportGeoTIFFOutput, wks->GetPath() );
	}

	SetExportGeoTIFFCmdFile( wks );
}

//----------------------------------------
void COperation::SetOutput( const std::string& value, CWorkspaceOperation* wks )
{
	//CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();

	m_output = value;

	if ( wks == nullptr )
	{
		clean_path( m_output );
	}
	else
	{
		//m_output.Normalize( wxPATH_NORM_ALL, wks->GetPath() );
		normalize( m_output, wks->GetPath() );
	}

	SetCmdFile( wks );
}
//----------------------------------------
std::string COperation::GetOutputPathRelativeToWks( const CWorkspaceOperation *wks ) const
{
	if ( wks == nullptr )
		return GetOutputPath();

	return GetRelativePath( wks->GetPath(), m_output );
}
//----------------------------------------
std::string COperation::GetExportAsciiOutputPathRelativeToWks( const CWorkspaceOperation *wks ) const
{
	if ( wks == nullptr )
		return GetExportAsciiOutputPath();

	return GetRelativePath( wks->GetPath(), m_exportAsciiOutput );
}
std::string COperation::GetExportNetcdfOutputPathRelativeToWks( const CWorkspaceOperation *wks ) const
{
	if ( wks == nullptr )
		return GetExportNetcdfOutputPath();

	return GetRelativePath( wks->GetPath(), m_exportNetcdfOutput );
}
std::string COperation::GetExportGeoTIFFOutputPathRelativeToWks( const CWorkspaceOperation *wks ) const
{
	if ( wks == nullptr )
		return GetExportGeoTIFFOutputPath();

	return GetRelativePath( wks->GetPath(), m_exportGeoTIFFOutput );
}
std::string COperation::GetShowStatsOutputPathRelativeToWks( const CWorkspaceOperation *wks ) const
{
	if ( wks == nullptr )
		return GetShowStatsOutputPath();

	return GetRelativePath( wks->GetPath(), m_showStatsOutput );
}

//----------------------------------------
void COperation::SetCmdFile( CWorkspaceOperation* wks )
{
	//CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();

	m_cmdFile = m_output;
	SetFileExtension( m_cmdFile, COMMANDFILE_EXTENSION );
	if ( wks == nullptr )
	{
		clean_path( m_cmdFile );
	}
	else
	{
		//m_cmdFile.Normalize( wxPATH_NORM_ALL, wks->GetPath() );
		normalize( m_cmdFile, wks->GetPath() );
	}

	if ( mScheduledTaskName.empty() )
		mScheduledTaskName = GetTaskName();
}
//----------------------------------------
void COperation::SetExportAsciiCmdFile( CWorkspaceOperation *wks )
{
	//CWorkspaceOperation *wks = wxGetApp().GetCurrentWorkspaceOperation();

	m_exportAsciiCmdFile = wks->GetPath() + "/" + GetFileName( m_exportAsciiOutput );
	SetFileExtension( m_exportAsciiCmdFile, EXPORTASCII_COMMANDFILE_EXTENSION );
	if ( wks == nullptr )
	{
		clean_path( m_exportAsciiCmdFile );
	}
	else
	{
		//m_exportAsciiCmdFile.Normalize( wxPATH_NORM_ALL, wks->GetPath() );
		normalize( m_exportAsciiCmdFile, wks->GetPath() );
	}

	if ( mScheduledExportAsciiTaskName.empty() )
		mScheduledExportAsciiTaskName = GetExportAsciiTaskName();
}
//----------------------------------------
void COperation::SetShowStatsCmdFile( CWorkspaceOperation *wks )
{
	//CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();

	m_showStatsCmdFile = wks->GetPath() + "/" + GetFileName( m_showStatsOutput );
	SetFileExtension( m_showStatsCmdFile, SHOWSTAT_COMMANDFILE_EXTENSION );
	if ( wks == nullptr )
	{
		clean_path( m_showStatsCmdFile );
	}
	else
	{
		//m_showStatsCmdFile.Normalize( wxPATH_NORM_ALL, wks->GetPath() );
		normalize( m_showStatsCmdFile, wks->GetPath() );
	}
}
void COperation::SetExportGeoTIFFCmdFile( CWorkspaceOperation *wks )
{
	m_exportGeoTIFFCmdFile = wks->GetPath() + "/" + GetFileName( m_exportGeoTIFFOutput );
	SetFileExtension( m_exportGeoTIFFCmdFile, EXPORTGEOTIFF_COMMANDFILE_EXTENSION );
	if ( wks == nullptr )
	{
		clean_path( m_exportGeoTIFFCmdFile );
	}
	else
	{
		//m_exportAsciiCmdFile.Normalize( wxPATH_NORM_ALL, wks->GetPath() );
		normalize( m_exportGeoTIFFCmdFile, wks->GetPath() );
	}	

	if ( mScheduledExportGeoTIFFTaskName.empty() )
		mScheduledExportGeoTIFFTaskName = GetExportGeoTIFFTaskName();
}

//----------------------------------------
void COperation::InitOutputs( CWorkspaceOperation *wks )
{
	InitOperationOutput( wks );
	InitShowStatsOutput( wks );
	InitExportAsciiOutput( wks );
	InitExportNetcdfOutput( wks );
	InitExportGeoTIFFOutput( wks );
}
void COperation::InitOperationOutput( CWorkspaceOperation *wks )
{
	if ( wks == nullptr )
		return;

	SetOutput( wks->GetPath() + "/Create_" + GetName() + ".nc", wks );
}
void COperation::InitShowStatsOutput( CWorkspaceOperation *wks )
{
	if ( wks == nullptr )
		return;

	SetShowStatsOutput( wks->GetPath() + "/Stats_" + GetName() + ".txt", wks );
}
//----------------------------------------
void COperation::InitExportAsciiOutput( CWorkspaceOperation *wks )
{
	if ( wks == nullptr )
		return;

	SetExportAsciiOutput( wks->GetPath() + "/ExportAscii_" + GetName() + ".txt", wks );
}
void COperation::InitExportNetcdfOutput( CWorkspaceOperation *wks )
{
	if ( wks == nullptr )
		return;

	SetExportNetcdfOutput( wks->GetPath() + "/ExportNetcdf_" + GetName() + ".nc", wks );
}
void COperation::InitExportGeoTIFFOutput( CWorkspaceOperation *wks )
{
	if ( wks == nullptr )
		return;

	SetExportGeoTIFFOutput( wks->GetPath() + "/ExportGeoTIFF_" + GetName() + ".tif", wks );
}

//----------------------------------------
bool COperation::RemoveOutput()
{
	bool bOk = RemoveFile( GetOutputPath() );

	RemoveFile( GetCmdFile() );

	return bOk;
}
//----------------------------------------
bool COperation::RenameOutput( const std::string &old_path )
{
	const bool is_old_file = IsFile( old_path );
	bool bOk = !is_old_file || RenameFile( old_path, GetOutputPath() );

	if ( is_old_file )
	{
		std::string oldCmdFile = old_path;
		SetFileExtension( oldCmdFile, COMMANDFILE_EXTENSION );

		//RenameFile(oldCmdFile.GetFullPath(), GetCmdFile());
		RemoveFile( oldCmdFile );
	}

	return bOk;
}

//----------------------------------------
//bool COperation::ComputeInterval( const std::string& formulaName, std::string &error_msg )
//{
//	CFormula* f = dynamic_cast<CFormula*>( m_formulas.Exists( (const char *)formulaName.c_str() ) );
//
//	return ComputeInterval( f, error_msg );
//}
//----------------------------------------
bool COperation::ComputeInterval( std::string &error_msg )
{
	if ( !IsZFXY() )
	{
		return true;
	}

	bool result = true;
	std::vector< CFormula* > v = { GetFormula( CMapTypeField::eTypeOpAsX ), GetFormula( CMapTypeField::eTypeOpAsY ) };
	for ( auto *formula : v )
	{
		if ( !formula )
			continue;

		if ( !ComputeInterval( formula, error_msg ) )
			result = false;
	}
	return result;
}
bool COperation::ComputeInterval( CFormula* f, std::string &error_msg )
{
	if ( f == nullptr )
		return false;

	if ( !IsZFXY() || !f->IsXYType() )
		return true;

	return f->ComputeInterval( error_msg );
}

//----------------------------------------
bool COperation::GetXExpression_NOT_USED( const CProductInfo &pi, CExpression& expr, std::string& error_msg, const CStringMap* aliases ) const		//aliases = nullptr
{
	const CFormula* formula = GetFormula( CMapTypeField::eTypeOpAsX );
	if ( formula != nullptr )
	{
		const CStringMap* fieldAliases = nullptr;
		if ( pi.IsValid() )
		{
			fieldAliases = pi.AliasesAsString();
		}

		if ( !CFormula::SetExpression( formula->GetDescription( true, aliases, fieldAliases ), expr, error_msg ) )
			return false;
	}

	return true;
}

//----------------------------------------
bool COperation::GetYExpression_NOT_USED( const CProductInfo &pi, CExpression& expr, std::string& error_msg, const CStringMap* aliases ) const		//aliases = nullptr
{
	const CFormula* formula = GetFormula( CMapTypeField::eTypeOpAsY );

	if ( formula != nullptr )
	{
		const CStringMap* fieldAliases = nullptr;
		if ( pi.IsValid() )
		{
			fieldAliases = pi.AliasesAsString();
		}

		if ( !CFormula::SetExpression( formula->GetDescription( true, aliases, fieldAliases ), expr, error_msg ) )
			return false;
	}

	return true;
}

//----------------------------------------
bool COperation::ControlXYDataFields_NOT_USED(std::string &error_msg, const CStringMap* aliases /* = nullptr*/)
{

	const CProductInfo pi( OriginalDataset() );

  bool bOk = true;

  CExpression xExpr;
  CExpression yExpr;
  CExpression dataExpr;

  /*
  CFormula* xFormula = GetFormula(CMapTypeField::typeOpAsX);

  if (xFormula != nullptr)
  {
    bOk = bOk && CFormula::SetExpression(xFormula->GetDescription(true, aliases), xExpr, error_msg);
  }

  CFormula* yFormula = GetFormula(CMapTypeField::typeOpAsY);

  if (yFormula != nullptr)
  {
    bOk = bOk && CFormula::SetExpression(yFormula->GetDescription(true, aliases), yExpr, error_msg);
  }
  */
  const CStringMap* fieldAliases = nullptr;
  if ( pi.IsValid() )
  {
    fieldAliases = pi.AliasesAsString();
  }

  bOk = bOk && GetXExpression_NOT_USED(pi, xExpr, error_msg, aliases);
  bOk = bOk && GetYExpression_NOT_USED(pi, yExpr, error_msg, aliases);

  CMapFormula::iterator it;

  for (it = m_formulas.begin() ; it != m_formulas.end() ; it++)
  {
    CFormula* value = dynamic_cast<CFormula*>(it->second);
    if (value == nullptr)
    {
      continue;
    }

    switch (value->GetFieldType())
    {

    //-----------------------------
    case CMapTypeField::eTypeOpAsX:
    case CMapTypeField::eTypeOpAsY:
    //-----------------------------
      continue;
      break;
    //-----------------------------
    case CMapTypeField::eTypeOpAsField:
    {
    //-----------------------------
      bOk = bOk && CFormula::SetExpression(value->GetDescription(true, aliases, fieldAliases), dataExpr, error_msg);

      CStringArray intersect;

      bool bIntersect = xExpr.GetFieldNames()->Intersect(*(dataExpr.GetFieldNames()), intersect, true);

      if (bIntersect)
      {
        error_msg.append("\nCommon fields between X expression and data expressions are not allowed : common field(s) is (are) :\n");
        error_msg.append(intersect.ToString("\n", false).c_str());
        bOk = false;
      }

      intersect.RemoveAll();

      bIntersect = yExpr.GetFieldNames()->Intersect(*(dataExpr.GetFieldNames()), intersect, true);

      if (bIntersect)
      {
        error_msg.append("\nCommon fields between Y expression and data expressions are not allowed : common field(s) is (are) :\n");
        error_msg.append(intersect.ToString("\n", false).c_str());
        bOk = false;
      }
    }
    break;

    default:
        assert__( false );
    }

  }

  return bOk;
}
//----------------------------------------
bool COperation::ControlResolution( std::string& error_msg )
{
	bool bOk = true;

	if ( this->GetType() != CMapTypeOp::eTypeOpZFXY )
	{
		return bOk;
	}


	const CFormula* xFormula = GetFormula( CMapTypeField::eTypeOpAsX );

	if ( xFormula != nullptr )
	{
		bOk = bOk && xFormula->ControlResolution( error_msg );
	}

	const CFormula* yFormula = GetFormula( CMapTypeField::eTypeOpAsY );

	if ( yFormula != nullptr )
	{
		bOk = bOk && yFormula->ControlResolution( error_msg );
	}

	return bOk;
}
//----------------------------------------
bool COperation::ControlDimensions( const CProductInfo &pi, CFormula* formula, std::string &error_msg, const CStringMap* aliases )	//aliases = nullptr
{
	if ( formula == nullptr )
		return true;

	if ( !pi.IsValid() )
		return true;

	std::string msg;

	std::string stringExpr = formula->GetDescription( true, aliases, pi.AliasesAsString() );

	if ( stringExpr.empty() )
	{
		return true;
	}


	CUIntArray commonDimensions;
	bool bOk = pi.HasCompatibleDims( stringExpr, m_record, msg, true, &commonDimensions );

	error_msg += msg;

	return bOk;

	if ( formula->GetFieldType() != CMapTypeField::eTypeOpAsSelect )
	{
		return bOk;
	}

	if ( commonDimensions.size() <= 0 )
	{
		return bOk;
	}

	CStringArray axesFields;

	CStringArray fields;

	const CFormula* xFormula = GetFormula( CMapTypeField::eTypeOpAsX );

	if ( xFormula != nullptr )
	{
		fields.RemoveAll();

		bOk = bOk && xFormula->GetFields( fields, error_msg, aliases, pi.AliasesAsString() );

		axesFields.Insert( fields );
	}

	const CFormula* yFormula = GetFormula( CMapTypeField::eTypeOpAsY );

	if ( yFormula != nullptr )
	{
		fields.RemoveAll();

		bOk = bOk && yFormula->GetFields( fields, error_msg, aliases, pi.AliasesAsString() );

		axesFields.Insert( fields );
	}


	fields.RemoveAll();

	bOk = bOk && formula->GetFields( fields, error_msg, aliases, pi.AliasesAsString() );

	CStringArray complement;
	axesFields.Complement( fields, complement );
	if ( complement.size() <= 0 )
	{
		return bOk;
	}

	error_msg += (
		std::string( "\nFields as array are not allowed in selection criteria expression unless they are present in X and/or Y axes.\nWrong field(s) is (are):\n" )
		+ complement.ToString( "\n", false )
		+ "\n"
		);

	return false;

}

//(*) If any of the CProduct member functions used in this validation needs the filtered dataset,
//	then add a CProgressIntereface parameter for filtering and invoke CreateFilteredDataset; this 
//	will also tell the client code that it must be prepared for a possibly heavy computation.
//
//----------------------------------------
bool COperation::Control( CWorkspaceFormula *wks, std::string &msg, const CStringMap *aliases )	//aliases = nullptr
{
	const CProductInfo pi( mOriginalDataset );	//(*)
	if ( !pi.IsValid() )
	{
		msg = pi.ErrorMessages();
		return false;
	}

	const bool basicControl = false;
	int xCount = 0;
	int yCount = 0;
	int fieldCount = 0;
	int errorCount = 0;
	bool bOk = true;

	for ( CMapFormula::iterator it = m_formulas.begin(); it != m_formulas.end(); it++ )
	{
		CFormula* value = dynamic_cast<CFormula*>( it->second );
		if ( value == nullptr )
		{
			continue;
		}

		switch ( value->GetFieldType() )
		{
			case CMapTypeField::eTypeOpAsX:
				xCount++;
				break;
			case CMapTypeField::eTypeOpAsY:
				yCount++;
				break;
			case CMapTypeField::eTypeOpAsField:
				fieldCount++;
				break;

			default:
				assert__( false );
		}
		bOk = value->CheckExpression( wks, msg, m_record, aliases, pi );
		if ( !bOk )
		{
			errorCount++;
		}
		else
		{
			bOk = ControlDimensions( pi, value, msg, aliases );
			if ( !bOk )
			{
				errorCount++;
			}
		}

		if ( !basicControl )
		{
			bOk = value->ControlUnitConsistency( msg );
			if ( !bOk )
			{
				errorCount++;
			}
		}
	}


	bOk = mSelectionCriteria->CheckExpression( wks, msg, m_record, aliases, pi );
	if ( !bOk )
	{
		errorCount++;
	}
	else
	{
		bOk = ControlDimensions( pi, mSelectionCriteria, msg, aliases );
		if ( !bOk )
		{
			errorCount++;
		}
	}


	if ( !basicControl )
	{
		if ( xCount == 0 )
		{
			msg += ( std::string( "\nThere is no 'X field' for operation '" ) + GetName() + "'." );
			errorCount++;
		}

		if ( ( yCount == 0 ) && ( this->GetType() == CMapTypeOp::eTypeOpZFXY ) )
		{
			msg += ( std::string( "\nThere is no 'Y field' for operation '" ) + GetName() + "'." );
			errorCount++;
		}
		if ( fieldCount == 0 )
		{
			msg += ( std::string( "\nThere is no 'Data field' for operation '" ) + GetName() + "'." );
			errorCount++;
		}

		if ( CtrlLoessCutOff( msg ) == false )
		{
			errorCount++;
		}


		if ( ControlResolution( msg ) == false )
		{
			errorCount++;
		}

		//if (ControlXYDataFields(msg, aliases) == false)
		//{
		//  errorCount++;
		//}
	}

	return ( errorCount == 0 );
}




#if defined(BRAT_V3)

void COperation::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }


  fOut << "==> Dump a COperation Object at "<< this << std::endl;

  mDataset->Dump(fOut);
  fOut << "==> END Dump a COperation Object at "<< this << std::endl;

  fOut << std::endl;

}

#endif
