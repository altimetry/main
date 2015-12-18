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

#include "brathl.h"

#include "Tools.h"
#include "BratProcess.h"

using namespace brathl;
using namespace processes;

#include "new-gui/brat/Workspaces/Workspace.h"
#include "Operation.h"




class non_copyable
{
protected:
	non_copyable() = default;
	~non_copyable() = default;

	non_copyable( non_copyable const & ) = delete;
	void operator=( non_copyable const &x ) = delete;
};


/////////////////////////////////////////////////////


class CCmdFile : non_copyable
{
	fstream mFile;
	const COperation &mOp;

public:
	CCmdFile( const std::string &path, const COperation &Op ) :
		mFile( path, ios::binary | ios::out | ios::trunc ), mOp( Op )
	{}

	virtual ~CCmdFile()
	{
		mFile.close();		// just in case
	}

	bool IsOk() const { return !!mFile; }

	///////////////////////////////////////////////////

	void WriteLn( const std::string& text = "" )
	{
		assert__( !!mFile );

		mFile << text << "\n";
	}
	void Comment( const std::string& text )
	{
		WriteLn( "#" + text );
	}

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

		if ( mOp.GetLogFile()->IsOk() )
		{
			WriteLn( kwLOGFILE + "=" + mOp.GetLogFile()->GetFullPath().ToStdString() );
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
		WriteLn( kwOUTPUT + "=" + mOp.GetOutput()->GetFullPath().ToStdString() );

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
		WriteLn( std::string( kwFILE.c_str() ) + "=" + mOp.GetOutputName() );

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
		WriteLn( kwOUTPUT + "=" + mOp.GetExportAsciiOutput()->GetFullPath().ToStdString() );

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
		WriteLn( kwOUTPUT + "=" + mOp.GetShowStatsOutput()->GetFullPath().ToStdString() );

		return true;
	}


	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////


public:

	static bool BuildCmdFile( const std::string &path, const COperation &Op, CWorkspaceFormula *wks, std::string &errorMsg )
	{
		CCmdFile f( path, Op );
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
		CCmdFile f( path, Op );
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
		CCmdFile f( path, Op );
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





//----------------------------------------

COperation::COperation(std::string name)
      : m_formulas(false)
{
  Init();
  m_name = name;

}
//----------------------------------------
COperation::COperation(COperation& o, CWorkspaceDataset *wks, CWorkspaceOperation *wkso )
      : m_formulas(false)
{
  Init();
  Copy( o, wks, wkso );
}
//----------------------------------------

COperation::~COperation()
{
  DeleteSelect();
}
//----------------------------------------
//COperation& COperation::operator = ( COperation& o )
//{
//
//  Copy(o);
//  return *this;
//}
//----------------------------------------
void COperation::Init()
{
  m_product = nullptr;
  m_dataset = nullptr;

  m_select = new CFormula(ENTRY_SELECT, false);
  m_select->SetType(CMapTypeField::eTypeOpAsSelect);

  m_type = CMapTypeOp::eTypeOpYFX;
  m_dataMode = CMapDataMode::GetInstance().GetDefault();
  //m_verbose = 2;
  m_exportAsciiDateAsPeriod = false;
  m_exportAsciiExpandArray = false;
  m_exportAsciiNoDataComputation = false;
  m_executeAgain = false;

  setDefaultValue(m_exportAsciiNumberPrecision);
}
//----------------------------------------
void COperation::DeleteSelect()
{

  if (m_select != nullptr)
  {
    delete m_select;
    m_select = nullptr;
  }
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
void COperation::SetSelect(CFormula* value)
{
  // if same pointer --> return
  if (IsSelect(value))
  {
    return;
  }

  DeleteSelect();

  m_select = value;
  m_select->SetName(ENTRY_SELECT);
}

//----------------------------------------
void COperation::Copy( COperation& o, CWorkspaceDataset *wks, CWorkspaceOperation *wkso )
{
	m_name = o.m_name;

	m_dataset = nullptr;
	if ( o.m_dataset != nullptr )
	{
		m_dataset = FindDataset( o.m_dataset->GetName(), wks );
	}

	m_record = o.m_record;
	m_product = o.m_product;

	if ( o.m_select != nullptr )
	{
		if ( m_select != nullptr )
		{
			*m_select = *( o.m_select );
		}
		else
		{
			m_select = new CFormula( *( o.m_select ) );
		}
	}

	m_formulas = o.m_formulas;

	m_type = o.m_type;
	m_dataMode = o.m_dataMode;
	m_exportAsciiDateAsPeriod = o.m_exportAsciiDateAsPeriod;

	InitOutput( wkso );
	SetCmdFile( wkso );

	InitExportAsciiOutput( wkso );
	SetExportAsciiCmdFile( wkso );
}

//----------------------------------------
void COperation::ClearLogFile()
{
  m_logFile.Clear();
}
//----------------------------------------
void COperation::SetLogFile(const std::string& value)
{
  m_logFile.Assign(value);
  m_logFile.Normalize();
}
//----------------------------------------
void COperation::SetLogFile( CWorkspaceOperation *wks )
{
  //CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();

  m_logFile = m_output;
  m_logFile.SetExt(LOGFILE_EXTENSION);
  if (wks == nullptr)
  {
    m_logFile.Normalize();
  }
  else
  {
    m_logFile.Normalize(wxPATH_NORM_ALL, wks->GetPath());
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

  int32_t i = m_formulas.size();
  std::string key;

  do
  {
    key = WKS_EXPRESSION_NAME + "_" + n2s<std::string>( i + 1 );
    i++;
  }
  while (m_formulas.Exists((const char *)key.c_str()) != nullptr);

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
CFormula* COperation::NewUserFormula(CField* field, int32_t typeField, bool addToMap, CProduct* product)
{
  if (field == nullptr)
  {
    return nullptr;

  }

  CFormula* formula =  NewUserFormula(field->GetName().c_str(), typeField, field->GetUnit().c_str(), addToMap, product);

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
CFormula* COperation::NewUserFormula( const std::string& name, int32_t typeField, const std::string& strUnit, bool addToMap, CProduct* product )
{
	bool bOk = true;

	CFormula* formulaToReturn = nullptr;

	std::string formulaName = name.empty() ? GetFormulaNewName() : GetFormulaNewName( name );

	CFormula formula( formulaName, false );
	formula.SetType( typeField );
	std::string errorMsg;
	formula.SetUnit( strUnit, errorMsg, "", false );
	if ( !errorMsg.empty() )
		wxMessageBox( errorMsg, "Warning", wxOK | wxCENTRE | wxICON_INFORMATION );

	CUnit* unit = formula.GetUnit();

	formula.SetDataType( typeField, *unit, product );

	if ( strUnit.empty() )
	{
		formula.SetDefaultUnit();
	}

	formula.SetDefaultMinMax();

	if ( addToMap )
	{
		bOk = AddFormula( formula );
		formulaToReturn = GetFormula( formulaName );
	}
	else
	{
		formulaToReturn = new CFormula( formula );
	}

	return formulaToReturn;
}
//----------------------------------------
bool COperation::AddFormula( CFormula& formula )
{
	CFormula* value = dynamic_cast<CFormula*>( m_formulas.Exists( (const char *)formula.GetName().c_str() ) );
	if ( value != nullptr )
	{
		return false;
	}

	std::string errorMsg;
	m_formulas.InsertUserDefined_ReplacePredefinedNotAllowed( formula, errorMsg );
	if ( !errorMsg.empty() )
		wxMessageBox( errorMsg, "Warning", wxOK | wxCENTRE | wxICON_INFORMATION );

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
std::string COperation::GetCommentFormula(const std::string& name)
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
/*
//----------------------------------------
void COperation::SetDescFormula(const std::string& name, const std::string& value)
{
  m_formulas.SetDescFormula(name, value);
}
*/
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
int32_t COperation::GetFormulaCountDataFields()
{
  return m_formulas.CountDataFields();
}
//----------------------------------------
bool COperation::HasFilters()
{
  return m_formulas.HasFilters();
}
/*
//----------------------------------------
std::string COperation::GetFormulaName()
{
  if (m_formula == nullptr)
  {
    return "";
  }

  return m_formula->GetName();
}

//----------------------------------------
std::string COperation::GetFormulaDescription()
{
  if (m_formula == nullptr)
  {
    return "";
  }

  return m_formula->GetDescription();
}
//----------------------------------------
void COperation::SetFormulaDesc(const std::string& value)
{
  if (m_formula == nullptr)
  {
    return;
  }

  m_formula->SetDescription(value);
}
*/
//----------------------------------------
/*
void COperation::SetSelectDesc(const std::string& value)
{
  if (m_select == nullptr)
  {
    return;
  }

  m_select->SetDescription(value);

}
*/
//----------------------------------------
std::string COperation::GetSelectName()
{
  if (m_select == nullptr)
  {
    return "";
  }

  return m_select->GetName();
}
//----------------------------------------
std::string COperation::GetSelectDescription()
{
  if (m_select == nullptr)
  {
    return "";
  }

  return m_select->GetDescription();
}
//----------------------------------------
bool COperation::SaveConfig( CConfiguration *config, CWorkspaceOperation *wkso )
{
	if ( config == nullptr )
		return true;

	config->SetPath( "/" + m_name );

	bool bOk = true;
	if ( m_dataset != nullptr )
		bOk &= config->Write( ENTRY_DSNAME, m_dataset->GetName().c_str() ) && config->Write( ENTRY_RECORDNAME, GetRecord() );

	bOk &= config->Write( ENTRY_TYPE, CMapTypeOp::GetInstance().IdToName( m_type ) );

	// Data mode is no more for a operation, but for each formula (data expression)
	//bOk &= config->Write(ENTRY_DATA_MODE,
	//                     CMapDataMode::GetInstance().IdToName(m_dataMode));

	m_select->SaveConfigDesc( config, config->GetPath().ToStdString() );

	// Save relative path based on path operation workspace for output file
	//bOk &= config->Write(ENTRY_OUTPUT, GetOutputName());
	bOk &= config->Write( ENTRY_OUTPUT, GetOutputNameRelativeToWks(wkso) );

	bOk &= config->Write( ENTRY_EXPORT_ASCII_OUTPUT, GetExportAsciiOutputNameRelativeToWks(wkso) );

	// Warning after formulas Load config conig path has changed
	m_formulas.SaveConfig( config, false, GetName() );

	return bOk;
}
//----------------------------------------
bool COperation::LoadConfig( CConfiguration *config, std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceOperation *wkso )
{
	bool bOk = true;
	if ( config == nullptr )
	{
		return true;
	}

	config->SetPath( "/" + m_name );

	std::string valueString;
	valueString = config->Read( ENTRY_DSNAME );
	m_dataset = FindDataset( valueString, wks );

	valueString = config->Read( ENTRY_TYPE, CMapTypeOp::GetInstance().IdToName( m_type ) );
	if ( valueString.empty() )
	{
		m_type = CMapTypeOp::eTypeOpYFX;
	}
	else
	{
		m_type = CMapTypeOp::GetInstance().NameToId( valueString );
	}

	valueString = config->Read( ENTRY_DATA_MODE, CMapDataMode::GetInstance().IdToName( m_dataMode ) );
	if ( valueString.empty() )
	{
		m_dataMode = CMapDataMode::GetInstance().GetDefault();
	}
	else
	{
		m_dataMode = CMapDataMode::GetInstance().NameToId( valueString );
	}


	m_record = config->Read( ENTRY_RECORDNAME );

	if ( m_record.empty() )
	{
		m_record = CProductNetCdf::m_virtualRecordName.c_str();
	}
	valueString = config->Read( ENTRY_OUTPUT );
	if ( valueString.empty() == false )
	{
		// Note that if the path to the output is in relative form,
		// SetOutput make it in absolute form based on workspace Operation path.
		SetOutput( valueString, wkso );
	}

	valueString = config->Read( ENTRY_EXPORT_ASCII_OUTPUT );
	if ( valueString.empty() == false )
	{
		// Note that if the path to the output is in relative form,
		// SetOutput make it in absolute form based on workspace Operation path.
		SetExportAsciiOutput( valueString, wkso );
	}
	else
	{
		InitExportAsciiOutput();
	}

	m_select->LoadConfigDesc( config, config->GetPath().ToStdString() );

	// Warning after formulas Load config conig path has changed
	m_formulas.LoadConfig( config, errorMsg, false, GetName() );

	m_formulas.InitFormulaDataMode( m_dataMode );

	return true;
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
wxFileName* COperation::GetSystemCommand()
{
	switch ( m_type )
	{
		case CMapTypeOp::eTypeOpYFX:
			return wxGetApp().GetExecYFXName();
			break;
		case CMapTypeOp::eTypeOpZFXY:
			return wxGetApp().GetExecZFXYName();
			break;
		default:
			return &m_cmdFile;
			break;
	}
}
//----------------------------------------
wxFileName* COperation::GetShowStatsSystemCommand()
{
  return wxGetApp().GetExecShowStatsName();
}
//----------------------------------------
wxFileName* COperation::GetExportAsciiSystemCommand()
{
  return wxGetApp().GetExecExportAsciiName();
}
//----------------------------------------
std::string COperation::GetFullCmd()
{
	return "\"" + GetSystemCommand()->GetFullPath().ToStdString() + "\" \"" + GetCmdFileFullName() + "\"";
}
//----------------------------------------
std::string COperation::GetExportAsciiFullCmd()
{
	return "\"" + GetExportAsciiSystemCommand()->GetFullPath().ToStdString() + "\" \"" + GetExportAsciiCmdFileFullName() + "\"";
}
//----------------------------------------
std::string COperation::GetShowStatsFullCmd()
{
	return "\"" + GetShowStatsSystemCommand()->GetFullPath().ToStdString() + "\" \"" + GetShowStatsCmdFileFullName() + "\"";
}


//----------------------------------------
std::string COperation::GetTaskName()
{
  //std::string value = m_cmdFile.GetName();
  if (m_cmdFile.GetName().IsEmpty())
  {
    return "NoName";
  }
  /*
  if (value.Last() == '.')
  {
    value.RemoveLast();
  }
  return value;
  */
  return m_cmdFile.GetName().ToStdString();
}
//----------------------------------------
std::string COperation::GetExportAsciiTaskName()
{
  //std::string value = m_cmdFile.GetName();
  if (m_exportAsciiCmdFile.GetName().IsEmpty())
  {
    return "NoName";
  }
  /*
  if (value.Last() == '.')
  {
    value.RemoveLast();
  }
  return value;
  */
  return m_exportAsciiCmdFile.GetName().ToStdString();
}

//----------------------------------------
std::string COperation::GetShowStatsTaskName()
{
  //std::string value = m_cmdFile.GetName();
  if (m_showStatsCmdFile.GetName().IsEmpty())
  {
    return "NoName";
  }
  /*
  if (value.Last() == '.')
  {
    value.RemoveLast();
  }
  return value;
  */
  return m_showStatsCmdFile.GetName().ToStdString();
}

//----------------------------------------
bool COperation::BuildCmdFile( CWorkspaceFormula *wks, CWorkspaceOperation *wkso, std::string &errorMsg )
{
#ifndef OLD_CREATE_FILES

	if ( GetOutput()->GetFullPath().IsEmpty() )
		InitOutput( wkso );
	
	return CCmdFile::BuildCmdFile( m_cmdFile.GetFullPath().ToStdString(), *this, wks, errorMsg );

#else

	m_cmdFile.Normalize();

	m_file.Create( m_cmdFile.GetFullPath(), true, wxS_DEFAULT | wxS_IXUSR );

	BuildCmdFileHeader();
	BuildCmdFileVerbose();
	BuildCmdFileGeneralProperties();
	BuildCmdFileAlias();
	BuildCmdFileDataset();
	BuildCmdFileSpecificUnit();
	BuildCmdFileFields();
	BuildCmdFileSelect();
	BuildCmdFileOutput();

	m_file.Close();
	return true;

#endif
}
//----------------------------------------
bool COperation::BuildExportAsciiCmdFile( CWorkspaceFormula *wks, CWorkspaceOperation *wkso )
{
#ifndef OLD_CREATE_FILES

	if ( GetExportAsciiOutput()->GetFullPath().IsEmpty() )
		InitExportAsciiOutput( wkso );

	return CCmdFile::BuildExportAsciiCmdFile( m_exportAsciiCmdFile.GetFullPath().ToStdString(), *this, wks );

#else

	m_exportAsciiCmdFile.Normalize();

	m_file.Create( m_exportAsciiCmdFile.GetFullPath(), true, wxS_DEFAULT | wxS_IXUSR );

	BuildExportAsciiCmdFileHeader();
	BuildCmdFileVerbose();
	BuildExportAsciiCmdFileGeneralProperties();
	BuildCmdFileAlias();

	if ( m_exportAsciiNoDataComputation )
	{
		BuildCmdFileDataset();
	}
	else
	{
		BuildCmdFileFromOutputOperation();
	}

	BuildCmdFileSpecificUnit();
	BuildExportAsciiCmdFileFields();

	// If Export ascii is done from the output operation file, 
	// don't include the select expression in the command line,
	// because the select expression has already been applied 
	// and the fields contained in the select expression are not necessarily in the output file. 
	if ( m_exportAsciiNoDataComputation )
	{
		BuildCmdFileSelect();
	}

	BuildExportAsciiCmdFileOutput();

	m_file.Close();
	return true;

#endif
}
//----------------------------------------
bool COperation::BuildShowStatsCmdFile( CWorkspaceFormula *wks, CWorkspaceOperation *wkso )
{
#ifndef OLD_CREATE_FILES

	if ( m_showStatsOutput.GetFullPath().IsEmpty() )
		InitShowStatsOutput( wkso );

	return CCmdFile::BuildShowStatsCmdFile( m_showStatsCmdFile.GetFullPath().ToStdString(), *this, wks );

#else

	m_showStatsCmdFile.Normalize();

	m_file.Create( m_showStatsCmdFile.GetFullPath(), true, wxS_DEFAULT | wxS_IXUSR );

	BuildShowStatsCmdFileHeader();
	BuildCmdFileVerbose();
	BuildCmdFileAlias();
	BuildCmdFileDataset();
	BuildCmdFileSpecificUnit();
	BuildShowStatsCmdFileFields();
	BuildCmdFileSelect();
	BuildShowStatsCmdFileOutput();

	m_file.Close();
	return true;

#endif
}
//----------------------------------------
//bool COperation::BuildShowStatsCmdFileHeader()
//{
////  WriteComment("!/usr/bin/env " + GetShowStatsSystemCommand()->GetName());
//  return true;
//
//}
//
////----------------------------------------
//bool COperation::BuildExportAsciiCmdFileHeader()
//{
////  WriteComment("!/usr/bin/env " + GetExportAsciiSystemCommand()->GetName());
//  return true;
//
//}
//
////----------------------------------------
//bool COperation::BuildCmdFileHeader()
//{
////  WriteComment("!/usr/bin/env " + GetSystemCommand()->GetName());
//  WriteComment("Type:" + CMapTypeOp::GetInstance().IdToName(m_type));
//  return true;
//
//}
////----------------------------------------
//bool COperation::BuildExportAsciiCmdFileGeneralProperties()
//{
//
//  WriteEmptyLine();
//  WriteComment("----- GENERAL PROPERTIES -----");
//  WriteEmptyLine();
//
//  std::string valueString = (m_exportAsciiDateAsPeriod ? "Yes" : "No");
//
//  WriteLine(std::string(kwDATE_AS_PERIOD.c_str()) + "=" + valueString);
//
//  valueString = (m_exportAsciiExpandArray ? "Yes" : "No");
//
//  WriteLine(std::string(kwEXPAND_ARRAY.c_str()) + "=" + valueString);
//
//  return true;
//
//}
////----------------------------------------
//bool COperation::BuildCmdFileGeneralProperties()
//{
//
//  WriteEmptyLine();
//  WriteComment("----- GENERAL PROPERTIES -----");
//  WriteEmptyLine();
//  // Data mode is no more for a operation, but for each formula (data expression)
//  //WriteLine(std::string::Format("DATA_MODE=%s", CMapDataMode::GetInstance().IdToName(m_dataMode).c_str()));
//
//  return true;
//
//}
//
////----------------------------------------
//bool COperation::BuildCmdFileAlias()
//{
//  CMapFormula::iterator it;
//
//  CWorkspaceFormula* wks = wxGetApp().GetCurrentWorkspaceFormula();
//  if (wks == nullptr)
//  {
//    return false;
//  }
//
//  CMapFormula* formulas = wks->GetFormulas();
//
//  WriteEmptyLine();
//  WriteComment("----- ALIAS -----");
//
//  for (it = formulas->begin() ; it != formulas->end() ; it++)
//  {
//    CFormula* value = dynamic_cast<CFormula*>(it->second);
//    if (value == nullptr)
//    {
//      continue;
//    }
//    WriteLine(std::string(kwALIAS_NAME.c_str()) + std::string("=") + std::string(value->GetName().c_str()));
//    WriteLine(std::string(kwALIAS_VALUE.c_str()) + std::string("=") + std::string(value->GetDescription(true)));
//  }
//
//  return true;
//
//}
////----------------------------------------
//bool COperation::BuildCmdFileSpecificUnit()
//{
//	const CDataset* dataset = GetDataset();
//	if ( dataset == nullptr )
//		return false;
//
//
//	WriteEmptyLine();
//	WriteComment( "----- SPECIFIC UNIT -----" );
//
//	const CStringMap* fieldSpecificUnit = dataset->GetFieldSpecificUnits();
//
//	for ( CStringMap::const_iterator itMap = fieldSpecificUnit->begin(); itMap != fieldSpecificUnit->end(); itMap++ )
//	{
//		WriteLine( kwUNIT_ATTR_NAME + "=" + itMap->first );
//		WriteLine( kwUNIT_ATTR_VALUE + "=" + itMap->second );
//	}
//
//	return true;
//}
////----------------------------------------
//bool COperation::BuildCmdFileFromOutputOperation()
//{
//
//  WriteEmptyLine();
//  WriteComment("----- DATASET -----");
//  WriteEmptyLine();
//  WriteLine(std::string(kwRECORD.c_str()) + "=" + CProductNetCdf::m_virtualRecordName.c_str());
//  WriteEmptyLine();
//  WriteLine(std::string(kwFILE.c_str()) + "=" + this->GetOutputName());
//
//  return true;
//
//}
////----------------------------------------
//bool COperation::BuildCmdFileDataset()
//{
//	WriteEmptyLine();
//	WriteComment( "----- DATASET -----" );
//	WriteEmptyLine();
//	WriteLine( kwRECORD + "=" + m_record );
//	WriteEmptyLine();
//	std::vector< std::string >  array;
//	m_dataset->GetFiles( array );
//	for ( size_t i = 0; i < array.size(); i++ )
//	{
//		WriteLine( kwFILE + "=" + array[ i ] );
//	}
//	return true;
//}
////----------------------------------------
//bool COperation::BuildCmdFileFields()
//{
//  std::string valueString;
//
//  WriteEmptyLine();
//  WriteComment("----- FIELDS -----");
//  WriteEmptyLine();
//
//  CMapFormula::iterator it;
//
//  for (it = m_formulas.begin() ; it != m_formulas.end() ; it++)
//  {
//    CFormula* value = dynamic_cast<CFormula*>(it->second);
//    if (value == nullptr)
//    {
//      continue;
//    }
//    WriteEmptyLine();
//    WriteComment(value->GetComment(true));
//    WriteLine(value->GetFieldPrefix()  + "=" + value->GetDescription(true));
//    WriteLine(value->GetFieldPrefix()  + "_NAME=" + value->GetName());
//    WriteLine(value->GetFieldPrefix()  + "_TYPE=" + value->GetDataTypeAsString());
//    WriteLine(value->GetFieldPrefix()  + "_UNIT=" + value->GetUnitAsText());
//
//    if (value->IsFieldType())
//    {
//      WriteLine(value->GetFieldPrefix()  + "_DATA_MODE=" + std::string(value->GetDataModeAsString().c_str()));
//    }
//
//    valueString = (value->GetTitle().empty()) ? value->GetName() : value->GetTitle();
//    WriteLine(value->GetFieldPrefix()  + "_TITLE=" + valueString);
//
//    valueString = (value->GetComment().empty()) ? "DV" : value->GetComment();
//    WriteLine(value->GetFieldPrefix()  + "_COMMENT=" + valueString);
//
//    BuildCmdFileFieldsSpecificZFXY(value);
//
//  }
//
//  return true;
//
//}
////----------------------------------------
//bool COperation::BuildExportAsciiCmdFileFields()
//{
//	std::string valueString;
//
//	WriteEmptyLine();
//	WriteComment( "----- FIELDS -----" );
//	WriteEmptyLine();
//
//	CMapFormula::iterator it;
//
//	for ( it = m_formulas.begin(); it != m_formulas.end(); it++ )
//	{
//		CFormula* value = dynamic_cast<CFormula*>( it->second );
//		if ( value == nullptr )
//		{
//			continue;
//		}
//		WriteEmptyLine();
//		WriteComment( value->GetComment( true ) );
//
//		// If export operation  is not a dump export,
//		// Field name in not the field name of the source data file
//		// but the field name of the operation result file (intermediate netcdf file).
//		if ( m_exportAsciiNoDataComputation )
//		{
//			// Just a dump --> get the name of the source file
//			WriteLine( value->GetExportAsciiFieldPrefix() + "=" + value->GetDescription( true ) );
//		}
//		else
//		{
//			// Not a dump --> get the name of operation file (intermediate netcdf file).
//			WriteLine( value->GetExportAsciiFieldPrefix() + "=" + value->GetName() );
//		}
//
//		WriteLine( value->GetExportAsciiFieldPrefix() + "_NAME=" + value->GetName() );
//		WriteLine( value->GetExportAsciiFieldPrefix() + "_UNIT=" + value->GetUnitAsText() );
//
//		if ( ! isDefaultValue( m_exportAsciiNumberPrecision ) )
//		{
//			WriteLine( value->GetExportAsciiFieldPrefix() + "_FORMAT=" + n2s<std::string>( m_exportAsciiNumberPrecision ) );
//		}
//	}
//
//	return true;
//}
////----------------------------------------
//bool COperation::BuildShowStatsCmdFileFields()
//{
//  std::string valueString;
//
//  WriteEmptyLine();
//  WriteComment("----- FIELDS -----");
//  WriteEmptyLine();
//
//  CMapFormula::iterator it;
//
//  for (it = m_formulas.begin() ; it != m_formulas.end() ; it++)
//  {
//    CFormula* value = dynamic_cast<CFormula*>(it->second);
//    if (value == nullptr)
//    {
//      continue;
//    }
//    if (value->GetType() != CMapTypeField::eTypeOpAsField)
//    {
//      continue;
//    }
//    WriteEmptyLine();
//    WriteComment(value->GetComment(true));
//    WriteLine(value->GetExportAsciiFieldPrefix()  + "=" + value->GetDescription(true));
//    //WriteLine(value->GetExportAsciiFieldPrefix()  + "_NAME=" + value->GetName());
//    WriteLine(value->GetExportAsciiFieldPrefix()  + "_UNIT=" + value->GetUnitAsText());
//  }
//
//  return true;
//
//}
//
////----------------------------------------
//bool COperation::BuildCmdFileFieldsSpecificZFXY( CFormula* value )
//{
//
//	// if operation is not X/Y as Lat/Lon or Lon/Lat, force to default value
//	/*
//	if (!IsMap())
//	{
//	value->SetFilterDefault();
//	value->SetMinValueDefault();
//	value->SetMaxValueDefault();
//	value->SetIntervalDefault();
//	value->SetLoessCutOffDefault();
//	}
//	*/
//	std::string valueString;
//
//	if ( m_type != CMapTypeOp::eTypeOpZFXY )
//	{
//		return false;
//	}
//
//	if ( value == nullptr )
//	{
//		return false;
//	}
//
//	valueString = ( value->GetFilter() == CMapTypeFilter::eFilterNone ) ? "DV" : value->GetFilterAsString();
//	WriteLine( value->GetFieldPrefix() + "_FILTER=" + valueString );
//
//	if ( value->IsXYType() )
//	{
//		std::string errorMsg;
//		value->ComputeInterval( errorMsg );
//		if ( !errorMsg.empty() )
//			wxMessageBox( errorMsg, "Warning", wxOK | wxCENTRE | wxICON_INFORMATION );
//
//		if ( value->IsTimeDataType() )
//		{
//			valueString = ( isDefaultValue( value->GetMinValue() ) ) ? "DV" : value->GetMinValueAsDateString();
//			WriteLine( value->GetFieldPrefix() + "_MIN=" + valueString );
//
//			valueString = ( isDefaultValue( value->GetMaxValue() ) ) ? "DV" : value->GetMaxValueAsDateString();
//			WriteLine( value->GetFieldPrefix() + "_MAX=" + valueString );
//		}
//		else
//		{
//			valueString = ( isDefaultValue( value->GetMinValue() ) ) ? "DV" : value->GetMinValueAsText();
//			WriteLine( value->GetFieldPrefix() + "_MIN=" + valueString );
//
//			valueString = ( isDefaultValue( value->GetMaxValue() ) ) ? "DV" : value->GetMaxValueAsText();
//			WriteLine( value->GetFieldPrefix() + "_MAX=" + valueString );
//		}
//
//		valueString = ( isDefaultValue( value->GetInterval() ) ) ? "DV" : value->GetIntervalAsText();
//		WriteLine( value->GetFieldPrefix() + "_INTERVALS=" + valueString );
//
//		valueString = ( isDefaultValue( value->GetLoessCutOff() ) ) ? "DV" : value->GetLoessCutOffAsText();
//		WriteLine( value->GetFieldPrefix() + "_LOESS_CUTOFF=" + valueString );
//	}
//
//	return true;
//}
////----------------------------------------
//bool COperation::BuildCmdFileSelect()
//{
//
//  WriteEmptyLine();
//  WriteComment("----- SELECT -----");
//  WriteEmptyLine();
//  if (m_select->GetDescription(true).empty() == false)
//  {
//    WriteLine(std::string(kwSELECT.c_str()) + "=" + m_select->GetDescription(true));
//  }
//
//  return true;
//
//}
////----------------------------------------
//bool COperation::BuildCmdFileOutput()
//{
//
//  WriteEmptyLine();
//  WriteComment("----- OUTPUT -----");
//  WriteEmptyLine();
//  if (m_output.GetFullPath().IsEmpty())
//  {
//    InitOutput();
//  }
//  WriteLine( kwOUTPUT + "=" + m_output.GetFullPath().ToStdString() );
//
//  return true;
//
//}
////----------------------------------------
//bool COperation::BuildExportAsciiCmdFileOutput()
//{
//
//  WriteEmptyLine();
//  WriteComment("----- OUTPUT -----");
//  WriteEmptyLine();
//  if (m_exportAsciiOutput.GetFullPath().IsEmpty())
//  {
//    InitExportAsciiOutput();
//  }
//  WriteLine( kwOUTPUT + "=" + m_exportAsciiOutput.GetFullPath().ToStdString() );
//
//  return true;
//
//}
////----------------------------------------
//bool COperation::BuildShowStatsCmdFileOutput()
//{
//
//  WriteEmptyLine();
//  WriteComment("----- OUTPUT -----");
//  WriteEmptyLine();
//  if (m_showStatsOutput.GetFullPath().IsEmpty())
//  {
//    InitShowStatsOutput();
//  }
//  WriteLine( kwOUTPUT + "=" + m_showStatsOutput.GetFullPath().ToStdString() );
//
//  return true;
//
//}
//
//
////----------------------------------------
//bool COperation::BuildCmdFileVerbose()
//{
//
//  WriteEmptyLine();
//  WriteComment("----- LOG -----");
//  WriteEmptyLine();
//  WriteLine( kwVERBOSE + "=" + n2s<std::string>( m_verbose ) );
//
//  if (m_logFile.IsOk())
//  {
//    WriteLine( kwLOGFILE + "=" + m_logFile.GetFullPath().ToStdString() );
//  }
//
//  return true;
//
//}
//
////----------------------------------------
//bool COperation::WriteComment(const std::string& text)
//{
//  WriteLine("#" + text);
//  return true;
//}
////----------------------------------------
//bool COperation::WriteLine(const std::string& text)
//{
//  if (m_file.IsOpened() == false)
//  {
//    return false;
//  }
//  m_file.Write(text + "\n");
//  return true;
//}
////----------------------------------------
//bool COperation::WriteEmptyLine()
//{
//  if (m_file.IsOpened() == false)
//  {
//    return false;
//  }
//  m_file.Write("\n");
//  return true;
//}

//----------------------------------------
void COperation::SetShowStatsOutput(const wxFileName& value, CWorkspaceOperation* wks )
{
  //CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();

  m_showStatsOutput = value;

  if (wks == nullptr)
  {
    m_showStatsOutput.Normalize();
  }
  else
  {
    m_showStatsOutput.Normalize(wxPATH_NORM_ALL, wks->GetPath());
  }

  SetShowStatsCmdFile( wks );
}
//----------------------------------------
void COperation::SetShowStatsOutput(const std::string& value, CWorkspaceOperation* wks )
{
  //CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();

  m_showStatsOutput.Assign(value);

  if (wks == nullptr)
  {
    m_showStatsOutput.Normalize();
  }
  else
  {
    m_showStatsOutput.Normalize(wxPATH_NORM_ALL, wks->GetPath());
  }

  SetShowStatsCmdFile( wks );
}
//----------------------------------------
void COperation::SetExportAsciiOutput(const wxFileName& value, CWorkspaceOperation* wks )
{
  //CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();

  m_exportAsciiOutput = value;

  if (wks == nullptr)
  {
    m_exportAsciiOutput.Normalize();
  }
  else
  {
    m_exportAsciiOutput.Normalize(wxPATH_NORM_ALL, wks->GetPath());
  }

  SetExportAsciiCmdFile( wks );
}
//----------------------------------------
void COperation::SetExportAsciiOutput(const std::string& value, CWorkspaceOperation* wks )
{
  //CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();

  m_exportAsciiOutput.Assign(value);

  if (wks == nullptr)
  {
    m_exportAsciiOutput.Normalize();
  }
  else
  {
    m_exportAsciiOutput.Normalize(wxPATH_NORM_ALL, wks->GetPath());
  }

  SetExportAsciiCmdFile( wks );
}

//----------------------------------------
void COperation::SetOutput(const wxFileName& value, CWorkspaceOperation* wks )
{
  //CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();

  m_output = value;

  if (wks == nullptr)
  {
    m_output.Normalize();
  }
  else
  {
    m_output.Normalize(wxPATH_NORM_ALL, wks->GetPath());
  }

  SetCmdFile( wks );
}
//----------------------------------------
void COperation::SetOutput( const std::string& value, CWorkspaceOperation* wks )
{
	//CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();

	m_output.Assign( value );

	if ( wks == nullptr )
		m_output.Normalize();
	else
		m_output.Normalize( wxPATH_NORM_ALL, wks->GetPath() );

	SetCmdFile( wks );
}

//----------------------------------------
void COperation::SetCmdFile( CWorkspaceOperation* wks )
{
  //CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();

  m_cmdFile = m_output;
  m_cmdFile.SetExt(COMMANDFILE_EXTENSION);
  if (wks == nullptr)
  {
    m_cmdFile.Normalize();
  }
  else
  {
    m_cmdFile.Normalize(wxPATH_NORM_ALL, wks->GetPath());
  }

}
//----------------------------------------
void COperation::SetExportAsciiCmdFile( CWorkspaceOperation *wks )
{
  //CWorkspaceOperation *wks = wxGetApp().GetCurrentWorkspaceOperation();

  m_exportAsciiCmdFile.AssignDir(wks->GetPath());
  m_exportAsciiCmdFile.SetFullName(m_exportAsciiOutput.GetFullName());
  m_exportAsciiCmdFile.SetExt(EXPORTASCII_COMMANDFILE_EXTENSION);
  if (wks == nullptr)
  {
    m_exportAsciiCmdFile.Normalize();
  }
  else
  {
    m_exportAsciiCmdFile.Normalize(wxPATH_NORM_ALL, wks->GetPath());
  }

}
//----------------------------------------
void COperation::SetShowStatsCmdFile( CWorkspaceOperation *wks )
{
  //CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();

  m_showStatsCmdFile.AssignDir(wks->GetPath());
  m_showStatsCmdFile.SetFullName(m_showStatsOutput.GetFullName());
  m_showStatsCmdFile.SetExt(SHOWSTAT_COMMANDFILE_EXTENSION);
  if (wks == nullptr)
  {
    m_showStatsCmdFile.Normalize();
  }
  else
  {
    m_showStatsCmdFile.Normalize(wxPATH_NORM_ALL, wks->GetPath());
  }

}

//----------------------------------------
std::string COperation::GetOutputNameRelativeToWks( CWorkspaceOperation *wks )
{
  //CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();
  if (wks == nullptr)
  {
    return GetOutputName();
  }

  wxFileName relative = m_output;
  relative.MakeRelativeTo(wks->GetPath());
  return relative.GetFullPath().ToStdString();
}
//----------------------------------------
std::string COperation::GetExportAsciiOutputNameRelativeToWks( CWorkspaceOperation *wks )
{
  //CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();
  if (wks == nullptr)
  {
    return GetExportAsciiOutputName();
  }

  wxFileName relative = m_exportAsciiOutput;
  relative.MakeRelativeTo(wks->GetPath());
  return relative.GetFullPath().ToStdString();

}
//----------------------------------------
std::string COperation::GetShowStatsOutputNameRelativeToWks( CWorkspaceOperation *wks )
{
  //CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();
  if (wks == nullptr)
  {
    return GetShowStatsOutputName();
  }

  wxFileName relative = m_showStatsOutput;
  relative.MakeRelativeTo(wks->GetPath());
  return relative.GetFullPath().ToStdString();

}
//----------------------------------------
void COperation::InitOutput( CWorkspaceOperation *wks )
{
	//CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();
	if ( wks == nullptr )
	{
		return;
	}

	std::string output = wks->GetPath()		//wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR				!!! femm attention here !!!
		+ "/Create"
		+ GetName() + ".nc";

	SetOutput( output, wks );
}
//----------------------------------------
void COperation::InitShowStatsOutput( CWorkspaceOperation *wks )
{
  //CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();
  if (wks == nullptr)
  {
    return;
  }

  std::string output = wks->GetPath()				//wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR		!!! femm attention here !!!
                                     + "/Stats"
                                     + GetName() + ".txt";
  SetShowStatsOutput(output, wks );

}
//----------------------------------------
void COperation::InitExportAsciiOutput( CWorkspaceOperation *wks )
{
  //CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();
  if (wks == nullptr)
  {
    return;
  }

  std::string output = wks->GetPath()						//wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR !!! femm attention here !!!
                                     + "/ExportAscii"
                                     + GetName() + ".txt";
  SetExportAsciiOutput(output, wks );

}
//----------------------------------------
bool COperation::RemoveOutput()
{
  bool bOk = RemoveFile(GetOutputName());

  RemoveFile(GetCmdFileFullName());

  return bOk;
}
//----------------------------------------
bool COperation::RenameOutput(const std::string& oldName)
{
  bool bOk = RenameFile( oldName, GetOutputName() );

  wxFileName oldCmdFile;
  oldCmdFile.Assign(oldName);
  oldCmdFile.SetExt(COMMANDFILE_EXTENSION);

  //RenameFile(oldCmdFile.GetFullPath(), GetCmdFile());
  RemoveFile( oldCmdFile.GetFullPath().ToStdString() );
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

