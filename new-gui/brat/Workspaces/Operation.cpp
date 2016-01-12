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

#include "new-gui/Common/tools/Trace.h"
#include "new-gui/Common/tools/Exception.h"

#include "libbrathl/brathl.h"
#include "libbrathl/Tools.h"
#include "process/BratProcess.h"

using namespace brathl;
using namespace processes;

#include "Workspace.h"
#include "Operation.h"



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

	bool BuildCmdFileVerbose()
	{
		WriteLn();
		Comment( "----- LOG -----" );
		WriteLn();
		WriteLn( kwVERBOSE + "=" + n2s<std::string>( mOp.m_verbose ) );

		if ( !mOp.GetLogFile().empty() )
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
			WriteLn( std::string( kwALIAS_NAME.c_str() ) + std::string( "=" ) + std::string( value->GetName().c_str() ) );
			WriteLn( std::string( kwALIAS_VALUE.c_str() ) + std::string( "=" ) + std::string( value->GetDescription( true ) ) );
		}

		return true;
	}

	bool BuildCmdFileDataset()
	{
		WriteLn();
		Comment( "----- DATASET -----" );
		WriteLn();
		WriteLn( kwRECORD + "=" + mOp.GetRecord() );
		WriteLn();
		std::vector< std::string >  array;
		mOp.GetDataset()->GetFiles( array );
		for ( size_t i = 0; i < array.size(); i++ )
		{
			WriteLn( kwFILE + "=" + array[ i ] );
		}
		return true;
	}

	bool BuildCmdFileSpecificUnit()
	{
		const CDataset* dataset = mOp.GetDataset();
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

	bool BuildCmdFileFieldsSpecificZFXY( CFormula* value, std::string &errorMsg )
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
			value->ComputeInterval( errorMsg );

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

	bool BuildCmdFileFields( std::string &errorMsg )
	{
		WriteLn();
		Comment( "----- FIELDS -----" );
		WriteLn();

		const CMapFormula &formulas = *mOp.GetFormulas();
		for ( CMapFormula::const_iterator it = formulas.begin(); it != formulas.end(); it++ )
		{
			CFormula* value = dynamic_cast<CFormula*>( it->second );
			if ( value == nullptr )
				continue;

			WriteLn();
			Comment( value->GetComment( true ) );
			WriteLn( value->GetFieldPrefix() + "=" + value->GetDescription( true ) );
			WriteLn( value->GetFieldPrefix() + "_NAME=" + value->GetName() );
			WriteLn( value->GetFieldPrefix() + "_TYPE=" + value->GetDataTypeAsString() );
			WriteLn( value->GetFieldPrefix() + "_UNIT=" + value->GetUnitAsText() );

			if ( value->IsFieldType() )
				WriteLn( value->GetFieldPrefix() + "_DATA_MODE=" + std::string( value->GetDataModeAsString().c_str() ) );

			std::string valueString = ( value->GetTitle().empty() ) ? value->GetName() : value->GetTitle();
			WriteLn( value->GetFieldPrefix() + "_TITLE=" + valueString );

			valueString = ( value->GetComment().empty() ) ? "DV" : value->GetComment();
			WriteLn( value->GetFieldPrefix() + "_COMMENT=" + valueString );

			BuildCmdFileFieldsSpecificZFXY( value, errorMsg );
		}

		return true;
	}

	bool BuildCmdFileSelect()
	{
		WriteLn();
		Comment( "----- SELECT -----" );
		WriteLn();
		if ( mOp.GetSelect()->GetDescription( true ).empty() == false )
		{
			WriteLn( std::string( kwSELECT.c_str() ) + "=" + mOp.GetSelect()->GetDescription( true ) );
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

	bool BuildExportAsciiCmdFileHeader()
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

			if ( value->GetType() != CMapTypeField::eTypeOpAsField )
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

	static bool BuildCmdFile( const std::string &path, const COperation &Op, CWorkspaceFormula *wks, std::string &errorMsg )
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
			f.BuildCmdFileFields( errorMsg )	&&
			f.BuildCmdFileSelect()				&&
			f.BuildCmdFileOutput()				&&
			f.IsOk();
	}


	static bool BuildExportAsciiCmdFile( const std::string &path, const COperation &Op, CWorkspaceFormula *wks )
	{
		COperationCmdFile f( path, Op );
		//m_exportAsciiCmdFile.Normalize();
		//m_file.Create( m_exportAsciiCmdFile.GetFullPath(), true, wxS_DEFAULT | wxS_IXUSR );

		return
			f.IsOk()																								&&
			f.BuildExportAsciiCmdFileHeader()																		&&
			f.BuildCmdFileVerbose()																					&&
			f.BuildExportAsciiCmdFileGeneralProperties()															&&
			f.BuildCmdFileAlias( wks )																				&&
			( Op.IsExportAsciiNoDataComputation() ? f.BuildCmdFileDataset() : f.BuildCmdFileFromOutputOperation() ) &&
			f.BuildCmdFileSpecificUnit()																			&&
			f.BuildExportAsciiCmdFileFields()																		&&
			// If Export ascii is done from the output operation file, 
			// don't include the select expression in the command line,
			// because the select expression has already been applied 
			// and the fields contained in the select expression are not necessarily in the output file. 
			( !Op.IsExportAsciiNoDataComputation() || f.BuildCmdFileSelect() )										&&
			f.BuildExportAsciiCmdFileOutput()																		&&
			f.IsOk();
	}


	static bool BuildShowStatsCmdFile( const std::string &path, const COperation &Op, CWorkspaceFormula *wks )
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
			f.BuildCmdFileSelect()				&&
			f.BuildShowStatsCmdFileOutput()		&&
			f.IsOk();
	}
};



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




inline std::string setExecExtension( const std::string &execPath )
{
	static const std::string &ext =

#ifdef WIN32
		".exe"
#else
        ""
#endif
        ;

	return execPath + ext;
}


const std::string BRATSHOWSTATS_EXE		= setExecExtension( "BratStats" );
const std::string BRATSCHEDULER_EXE		= setExecExtension( "BratScheduler" );
const std::string BRATEXPORTGEOTIFF_EXE = setExecExtension( "BratExportGeoTiff" );
const std::string BRATEXPORTASCII_EXE	= setExecExtension( "BratExportAscii" );
const std::string BRATDISPLAY_EXE		= setExecExtension( "BratDisplay" );
const std::string BRATCREATEZFXY_EXE	= setExecExtension( "BratCreateZFXY" );
const std::string BRATCREATEYFX_EXE		= setExecExtension( "BratCreateYFX" );


//static 
std::string COperation::m_execYFXName;
//static 
std::string COperation::m_execZFXYName;
//static 
std::string COperation::m_execDisplayName;
//static 
std::string COperation::m_execExportAsciiName;
//static 
std::string COperation::m_execExportGeoTiffName;
//static 
std::string COperation::m_execShowStatsName;
//static 
std::string COperation::m_execBratSchedulerName;


//static 
void COperation::SetExecNames( const std::string &appPath )
{
	m_execYFXName = appPath + "/" + BRATCREATEYFX_EXE;					//m_execYFXName.MakeAbsolute();

	m_execZFXYName = appPath + "/" + BRATCREATEZFXY_EXE;				//m_execZFXYName.MakeAbsolute();

	m_execDisplayName = appPath + "/" + BRATDISPLAY_EXE;				//m_execDisplayName.MakeAbsolute();

	m_execExportAsciiName = appPath + "/" + BRATEXPORTASCII_EXE;		//m_execExportAsciiName.MakeAbsolute();

	m_execExportGeoTiffName = appPath + "/" + BRATEXPORTGEOTIFF_EXE;	//m_execExportGeoTiffName.MakeAbsolute();

	m_execShowStatsName = appPath + "/" + BRATSHOWSTATS_EXE;			//m_execShowStatsName.MakeAbsolute();

	m_execBratSchedulerName = appPath + "/" + BRATSCHEDULER_EXE;		//m_execBratSchedulerName.MakeAbsolute();


#ifdef __APPLE__

	//m_execName.IsRelative() ? 
	//m_execName.GetPath(flags, format) + "/BratScheduler.app/Contents/MacOS" :
	//m_execName.GetPath(flags, format));
	m_execBratSchedulerName = appPath + "/BratScheduler.app/Contents/MacOS/" + BRATSCHEDULER_EXE;		//m_execBratSchedulerName.MakeAbsolute();
#else

	m_execBratSchedulerName = appPath + "/" + BRATSCHEDULER_EXE;		//m_execBratSchedulerName.MakeAbsolute();
    assert__(
        IsFile( m_execYFXName )				&&
        IsFile( m_execZFXYName )			&&
        IsFile( m_execDisplayName )			&&
        IsFile( m_execExportAsciiName )		&&
        IsFile( m_execExportGeoTiffName )	&&
        IsFile( m_execShowStatsName )		&&
        IsFile( m_execBratSchedulerName )
        );
#endif

//assert__(
//	IsFile( m_execYFXName )				&&
//	IsFile( m_execZFXYName )			&&
//	IsFile( m_execDisplayName )			&&
//	IsFile( m_execExportAsciiName )		&&
//	IsFile( m_execExportGeoTiffName )	&&
//	IsFile( m_execShowStatsName )		&&
//	IsFile( m_execBratSchedulerName )
//	);
}




//----------------------------------------
bool COperation::IsSelect(CFormula* value)
{
  // if same pointer --> return
  return (m_select == value);
}
//----------------------------------------
bool COperation::IsZFXY()
{
  return (m_type == CMapTypeOp::eTypeOpZFXY);
}
//----------------------------------------
bool COperation::IsYFX()
{
  return (m_type == CMapTypeOp::eTypeOpYFX);
}
//----------------------------------------
bool COperation::IsMap()
{
  if (!IsZFXY())
  {
    return false;
  }

  CFormula* xFormula = GetFormula(CMapTypeField::eTypeOpAsX);
  CFormula* yFormula = GetFormula(CMapTypeField::eTypeOpAsY);

  if ((xFormula == nullptr)  || (yFormula == nullptr))
  {
    return false;
  }

  bool isMap = (xFormula->IsLonDataType() && yFormula->IsLatDataType()) ||
               (xFormula->IsLatDataType() && yFormula->IsLonDataType());

  return isMap;
}

//----------------------------------------
bool COperation::IsSelect( const std::string& name )
{
	  return str_icmp( name, ENTRY_SELECT );
}

//----------------------------------------
void COperation::SetSelect( CFormula* value )
{
	// if same pointer --> return
	if ( IsSelect( value ) )
		return;

	delete m_select;
	m_select = value;
	m_select->SetName( ENTRY_SELECT );
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

    if (value->GetType() == CMapTypeField::eTypeOpAsSelect)
    {
      continue;
    }

    if (value->GetType() == CMapTypeField::eTypeOpAsField)
    {
      if ( (isDefaultValue(value->GetFilter()) == false) &&
        (value->GetFilter() != CMapTypeFilter::eFilterNone) )
      {
        hasFilter = true;
        fields += "\t'" + value->GetName() + "'";
      }
    }
    if (value->GetType() == CMapTypeField::eTypeOpAsX)
    {
      if ( (isDefaultValue(value->GetLoessCutOff()) == false) &&
           (value->GetLoessCutOff() > 1) )
      {
        hasCutOffX = true;
      }
    }
    if (value->GetType() == CMapTypeField::eTypeOpAsY)
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
	if ( m_dataset == nullptr )
		return false;

	return str_icmp( m_dataset->GetName(), name );
}

//----------------------------------------
std::string COperation::GetFormulaNewName()
{

	size_t i = m_formulas.size();
	std::string key;

	do
	{
		key = WKS_EXPRESSION_NAME + "_" + n2s<std::string>( i + 1 );
		i++;
	} while ( m_formulas.Exists( (const char *)key.c_str() ) != nullptr );

	return key;
}
//----------------------------------------
std::string COperation::GetFormulaNewName( const std::string& prefix )
{
	if ( m_formulas.Exists( prefix ) == nullptr )
		return prefix;

	int i = 1;
	std::string key;
	do
	{
		key = prefix + "_" + n2s<std::string>( i );
		i++;
	} while ( m_formulas.Exists( key ) != nullptr );

	return key;
}


//----------------------------------------
bool COperation::RenameFormula(CFormula* formula, const std::string &newName)
{
 bool bOk = m_formulas.RenameKey((const char *)formula->GetName().c_str(), (const char *)newName.c_str());
 if (bOk == false)
 {
   return false;
 }

 formula->SetName(newName);
 return true;
}

//----------------------------------------
std::string COperation::GetDatasetName()
{
  if (m_dataset == nullptr)
  {
    return "";
  }

  return m_dataset->GetName();
}

//----------------------------------------
CFormula* COperation::NewUserFormula( std::string &errorMsg, CField* field, int32_t typeField, bool addToMap, CProduct* product )
{
  if (field == nullptr)
  {
    return nullptr;

  }

  CFormula* formula =  NewUserFormula( errorMsg, field->GetName().c_str(), typeField, field->GetUnit().c_str(), addToMap, product);

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
CFormula* COperation::NewUserFormula( std::string &errorMsg, const std::string& name, int32_t typeField, const std::string& strUnit, bool addToMap, CProduct* product )
{
	bool bOk = true;

	CFormula* formulaToReturn = nullptr;

	std::string formulaName = name.empty() ? GetFormulaNewName() : GetFormulaNewName( name );

	CFormula formula( formulaName, false );
	formula.SetType( typeField );
	formula.SetUnit( strUnit, errorMsg, "", false );

	CUnit* unit = formula.GetUnit();

	formula.SetDataType( typeField, *unit, product );

	if ( strUnit.empty() )
	{
		formula.SetDefaultUnit();
	}

	formula.SetDefaultMinMax();

	if ( addToMap )
	{
		bOk = AddFormula( formula, errorMsg );
		formulaToReturn = GetFormula( formulaName );
	}
	else
	{
		formulaToReturn = new CFormula( formula );
	}

	return formulaToReturn;
}
//----------------------------------------
bool COperation::AddFormula( CFormula& formula, std::string &errorMsg )
{
	CFormula* value = dynamic_cast<CFormula*>( m_formulas.Exists( (const char *)formula.GetName().c_str() ) );
	if ( value != nullptr )
	{
		return false;
	}

	m_formulas.InsertUserDefined_ReplacePredefinedNotAllowed( formula, errorMsg );

	return true;
}

//----------------------------------------
bool COperation::DeleteFormula(const std::string& name)
{

  bool bOk = false;

  CFormula* formula = GetFormula(name);

  if (formula != nullptr)
  {
    if (IsSelect(formula))
    {
      formula->SetDescription("");
      bOk = true;
    }
    else
    {
      bOk = m_formulas.Erase((const char *)name.c_str());
    }
  }
  else if (COperation::IsSelect(name))
  {
    m_select->SetDescription("");
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
CFormula* COperation::GetFormula(const std::string& name)
{
  return dynamic_cast<CFormula*>(m_formulas.Exists((const char *)name.c_str()));
}
//----------------------------------------
CFormula* COperation::GetFormula(int32_t type)
{
  return m_formulas.GetFormula(type);
}
//----------------------------------------
CFormula* COperation::GetFormula(CMapFormula::iterator it)
{
  return dynamic_cast<CFormula*>(it->second);
}
const CFormula* COperation::GetFormula(CMapFormula::const_iterator it)
{
  return dynamic_cast<const CFormula*>(it->second);
}

//----------------------------------------
size_t COperation::GetFormulaCountDataFields()
{
	return m_formulas.CountDataFields();
}
//----------------------------------------
std::string COperation::GetSelectName()
{
	return m_select ? m_select->GetName() : "";
}
//----------------------------------------
std::string COperation::GetSelectDescription()
{
	return m_select ? m_select->GetDescription() : "";
}
//----------------------------------------
bool COperation::SaveConfig( CWorkspaceSettings *config, const CWorkspaceOperation *wks ) const
{
	assert__( config );
	//return !config || config->SaveConfig( *this );

	return config->SaveConfig( *this, wks );
}
//----------------------------------------
bool COperation::LoadConfig( CWorkspaceSettings *config, std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceOperation *wkso )
{
	return !config || config->LoadConfig( *this, errorMsg, wks, wkso );
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
std::string COperation::GetFullCmd()
{
	return "\"" + GetSystemCommand() + "\" \"" + GetCmdFile() + "\"";
}
//----------------------------------------
std::string COperation::GetExportAsciiFullCmd()
{
	return "\"" + GetExportAsciiSystemCommand() + "\" \"" + GetExportAsciiCmdFile() + "\"";
}
//----------------------------------------
std::string COperation::GetShowStatsFullCmd()
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
//----------------------------------------
std::string COperation::GetShowStatsTaskName() const
{
	return m_showStatsCmdFile.empty() ? NoName : GetFileName( m_showStatsCmdFile );
}

//----------------------------------------
bool COperation::BuildCmdFile( CWorkspaceFormula *wks, CWorkspaceOperation *wkso, std::string &errorMsg )
{
	if ( m_output.empty() )
		InitOutput( wkso );
	
	return COperationCmdFile::BuildCmdFile( m_cmdFile, *this, wks, errorMsg );
}
//----------------------------------------
bool COperation::BuildExportAsciiCmdFile( CWorkspaceFormula *wks, CWorkspaceOperation *wkso )
{
	if ( m_exportAsciiOutput.empty() )
		InitExportAsciiOutput( wkso );

	return COperationCmdFile::BuildExportAsciiCmdFile( m_exportAsciiCmdFile, *this, wks );
}
//----------------------------------------
bool COperation::BuildShowStatsCmdFile( CWorkspaceFormula *wks, CWorkspaceOperation *wkso )
{
	if ( m_showStatsOutput.empty() )
		InitShowStatsOutput( wkso );

	return COperationCmdFile::BuildShowStatsCmdFile( m_showStatsCmdFile, *this, wks );
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

//----------------------------------------
void COperation::InitOutput( CWorkspaceOperation *wks )
{
	if ( wks == nullptr )
		return;

	//wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR				!!! femm attention here !!!

	SetOutput( wks->GetPath() + "/Create" + GetName() + ".nc", wks );
}
//----------------------------------------
void COperation::InitShowStatsOutput( CWorkspaceOperation *wks )
{
	if ( wks == nullptr )
		return;

	//wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR		!!! femm attention here !!!

	SetShowStatsOutput( wks->GetPath() + "/Stats" + GetName() + ".txt", wks );
}
//----------------------------------------
void COperation::InitExportAsciiOutput( CWorkspaceOperation *wks )
{
	//CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();
	if ( wks == nullptr )
		return;

	//wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR !!! femm attention here !!!

	SetExportAsciiOutput( wks->GetPath() + "/ExportAscii" + GetName() + ".txt", wks );
}
//----------------------------------------
bool COperation::RemoveOutput()
{
	bool bOk = RemoveFile( GetOutputPath() );

	RemoveFile( GetCmdFile() );

	return bOk;
}
//----------------------------------------
bool COperation::RenameOutput( const std::string& oldPath )
{
	bool bOk = RenameFile( oldPath, GetOutputPath() );

	std::string oldCmdFile = oldPath;
	SetFileExtension( oldCmdFile, COMMANDFILE_EXTENSION );

	//RenameFile(oldCmdFile.GetFullPath(), GetCmdFile());
	RemoveFile( oldCmdFile );
	return bOk;
}

//----------------------------------------
bool COperation::ComputeInterval( const std::string& formulaName, std::string &errorMsg )
{
	CFormula* f = dynamic_cast<CFormula*>( m_formulas.Exists( (const char *)formulaName.c_str() ) );

	return ComputeInterval( f, errorMsg );
}
//----------------------------------------
bool COperation::ComputeInterval( CFormula* f, std::string &errorMsg )
{
	if ( f == nullptr )
		return false;

	if ( !IsZFXY() || !f->IsXYType() )
		return true;

	return f->ComputeInterval( errorMsg );
}

//----------------------------------------
bool COperation::GetXExpression(CExpression& expr, std::string& errorMsg, const CStringMap* aliases /* = nullptr*/)
{
  bool bOk = true;

  CFormula* formula = GetFormula(CMapTypeField::eTypeOpAsX);

  if (formula != nullptr)
  {
    const CStringMap* fieldAliases = nullptr;
    if (m_product != nullptr)
    {
      fieldAliases = m_product->GetAliasesAsString();
    }

    bOk &= CFormula::SetExpression(formula->GetDescription(true, aliases, fieldAliases), expr, errorMsg);
  }

  return bOk;

}

//----------------------------------------
bool COperation::GetYExpression(CExpression& expr, std::string& errorMsg, const CStringMap* aliases /* = nullptr*/)
{
  bool bOk = true;

  CFormula* formula = GetFormula(CMapTypeField::eTypeOpAsY);

  if (formula != nullptr)
  {
    const CStringMap* fieldAliases = nullptr;
    if (m_product != nullptr)
    {
      fieldAliases = m_product->GetAliasesAsString();
    }

    bOk &= CFormula::SetExpression(formula->GetDescription(true, aliases, fieldAliases), expr, errorMsg);
  }

  return bOk;

}

//----------------------------------------
bool COperation::ControlXYDataFields(std::string &errorMsg, const CStringMap* aliases /* = nullptr*/)
{
  bool bOk = true;

  CExpression xExpr;
  CExpression yExpr;
  CExpression dataExpr;

  /*
  CFormula* xFormula = GetFormula(CMapTypeField::typeOpAsX);

  if (xFormula != nullptr)
  {
    bOk &= CFormula::SetExpression(xFormula->GetDescription(true, aliases), xExpr, errorMsg);
  }

  CFormula* yFormula = GetFormula(CMapTypeField::typeOpAsY);

  if (yFormula != nullptr)
  {
    bOk &= CFormula::SetExpression(yFormula->GetDescription(true, aliases), yExpr, errorMsg);
  }
  */
  const CStringMap* fieldAliases = nullptr;
  if (m_product != nullptr)
  {
    fieldAliases = m_product->GetAliasesAsString();
  }

  bOk &= GetXExpression(xExpr, errorMsg, aliases);
  bOk &= GetYExpression(yExpr, errorMsg, aliases);

  CMapFormula::iterator it;

  for (it = m_formulas.begin() ; it != m_formulas.end() ; it++)
  {
    CFormula* value = dynamic_cast<CFormula*>(it->second);
    if (value == nullptr)
    {
      continue;
    }

    switch (value->GetType())
    {

    //-----------------------------
    case CMapTypeField::eTypeOpAsX:
    case CMapTypeField::eTypeOpAsY:
    //-----------------------------
      continue;
      break;
    //-----------------------------
    case CMapTypeField::eTypeOpAsField:
    //-----------------------------
      bOk &= CFormula::SetExpression(value->GetDescription(true, aliases, fieldAliases), dataExpr, errorMsg);

      CStringArray intersect;

      bool bIntersect = xExpr.GetFieldNames()->Intersect(*(dataExpr.GetFieldNames()), intersect, true);

      if (bIntersect)
      {
        errorMsg.append("\nCommon fields between X expression and data expressions are not allowed : common field(s) is (are) :\n");
        errorMsg.append(intersect.ToString("\n", false).c_str());
        bOk = false;
      }

      intersect.RemoveAll();

      bIntersect = yExpr.GetFieldNames()->Intersect(*(dataExpr.GetFieldNames()), intersect, true);

      if (bIntersect)
      {
        errorMsg.append("\nCommon fields between Y expression and data expressions are not allowed : common field(s) is (are) :\n");
        errorMsg.append(intersect.ToString("\n", false).c_str());
        bOk = false;
      }

      break;
    }

  }

  return bOk;
}
//----------------------------------------
bool COperation::ControlResolution( std::string& errorMsg)
{
  bool bOk = true;

  if (this->GetType() != CMapTypeOp::eTypeOpZFXY)
  {
    return bOk;
  }


  CFormula* xFormula = GetFormula(CMapTypeField::eTypeOpAsX);

  if (xFormula != nullptr)
  {
    bOk &= xFormula->ControlResolution(errorMsg);
  }

  CFormula* yFormula = GetFormula(CMapTypeField::eTypeOpAsY);

  if (yFormula != nullptr)
  {
    bOk &= yFormula->ControlResolution(errorMsg);
  }

  return bOk;
}
//----------------------------------------
bool COperation::ControlDimensions(CFormula* formula, std::string &errorMsg,  const CStringMap* aliases /* = nullptr*/)
{
  if (formula == nullptr)
  {
    return true;
  }

  /*
  if ((!formula->IsXType()) && (!formula->IsYType()))
  {
    return true;
  }
*/
  if (m_product == nullptr)
  {
    return true;
  }

  std::string msg;

  std::string stringExpr = formula->GetDescription(true, aliases, m_product->GetAliasesAsString());

  if (stringExpr.empty())
  {
    return true;
  }


  CUIntArray commonDimensions;
  bool bOk = m_product->HasCompatibleDims( (const char *)stringExpr.c_str(), (const char *)m_record.c_str(), msg, true, &commonDimensions);

  errorMsg += msg;

  return bOk;

  if (formula->GetType() != CMapTypeField::eTypeOpAsSelect)
  {
    return bOk;
  }

  if (commonDimensions.size() <= 0)
  {
    return bOk;
  }

  CStringArray axesFields;

  CStringArray fields;

  CFormula* xFormula = GetFormula(CMapTypeField::eTypeOpAsX);

  if (xFormula != nullptr)
  {
    fields.RemoveAll();

    bOk &= xFormula->GetFields(fields, errorMsg, aliases, m_product->GetAliasesAsString());

    axesFields.Insert(fields);
  }

  CFormula* yFormula = GetFormula(CMapTypeField::eTypeOpAsY);

  if (yFormula != nullptr)
  {
    fields.RemoveAll();

    bOk &= yFormula->GetFields(fields, errorMsg, aliases, m_product->GetAliasesAsString());

    axesFields.Insert(fields);
  }


  fields.RemoveAll();

  bOk &= formula->GetFields(fields, errorMsg, aliases, m_product->GetAliasesAsString());

  CStringArray complement;

  axesFields.Complement(fields, complement);

  if (complement.size() <= 0)
  {
    return bOk;
  }

  errorMsg += (
	  std::string("\nFields as array are not allowed in selection criteria expression unless they are present in X and/or Y axes.\nWrong field(s) is (are):\n" )
	  + complement.ToString("\n", false)
	  + "\n"
	  );

  bOk = false;

  return bOk;

}

//----------------------------------------
bool COperation::Control(CWorkspaceFormula *wks, std::string& msg, bool basicControl /* = false */, const CStringMap* aliases /* = nullptr*/)
{
  CMapFormula::iterator it;
  int32_t xCount = 0;
  int32_t yCount = 0;
  int32_t fieldCount = 0;
  int32_t errorCount = 0;
  bool bOk = true;

  for (it = m_formulas.begin() ; it != m_formulas.end() ; it++)
  {
    CFormula* value = dynamic_cast<CFormula*>(it->second);
    if (value == nullptr)
    {
      continue;
    }

    switch (value->GetType())
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
    }
    bOk = value->CheckExpression(wks, msg, m_record, aliases, m_product);
    if (!bOk)
    {
      errorCount++;
    }
    else
    {
      bOk = ControlDimensions(value, msg, aliases);
      if (!bOk)
      {
        errorCount++;
      }
    }

    if (!basicControl)
    {
      bOk = value->ControlUnitConsistency(msg);
      if (!bOk)
      {
        errorCount++;
      }
    }
  }


  bOk = m_select->CheckExpression(wks, msg, m_record, aliases, m_product);
  if (!bOk)
  {
    errorCount++;
  }
  else
  {
    bOk = ControlDimensions(m_select, msg, aliases);
    if (!bOk)
    {
      errorCount++;
    }
  }


  if (!basicControl)
  {
    if (xCount == 0)
    {
      msg += ( std::string("\nThere is no 'X field' for operation '" ) + GetName() + "'." );
      errorCount++;
    }

    if ( (yCount == 0) && (this->GetType() == CMapTypeOp::eTypeOpZFXY) )
    {
      msg += ( std::string("\nThere is no 'Y field' for operation '" ) + GetName() + "'." );
      errorCount++;
    }
    if (fieldCount == 0)
    {
      msg += ( std::string( "\nThere is no 'Data field' for operation '" ) + GetName() + "'." );
      errorCount++;
    }

    if (CtrlLoessCutOff(msg) == false)
    {
      errorCount++;
    }


    if (ControlResolution(msg) == false)
    {
      errorCount++;
    }

    //if (ControlXYDataFields(msg, aliases) == false)
    //{
    //  errorCount++;
    //}
  }

  return (errorCount == 0);
}


//----------------------------------------
void COperation::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }


  fOut << "==> Dump a COperation Object at "<< this << std::endl;

  m_dataset->Dump(fOut);
  fOut << "==> END Dump a COperation Object at "<< this << std::endl;

  fOut << std::endl;

}
