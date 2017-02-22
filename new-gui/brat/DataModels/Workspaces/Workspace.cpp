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
#include "BitSet32.h"

#include "Operation.h"
#include "Display.h"
#include "RadsDataset.h"
#include "Workspace.h"
#include "WorkspaceSettings.h"


#if defined WORKSPACES_TREE_WORKSPACE_H
#error This module must be independent of CTreeWorkspace
#endif


//static 
const std::string CWorkspaceDataset::NAME = "Datasets";
//static 
const std::string CWorkspaceFormula::NAME = "Formulas";
//static 
const std::string CWorkspaceOperation::NAME = "Operations";
//static 
const std::string CWorkspaceDisplay::NAME = "Displays";



const std::string CWorkspace::m_configName = "setup.ini";

const std::string CWorkspace::m_keyDelimiter = ".";


//-------------------------------------------------------------
//------------------- CWorkspace class --------------------
//-------------------------------------------------------------

//virtual 
CWorkspace::~CWorkspace()
{
	delete m_config;
}


//----------------------------------------
std::string CWorkspace::GetRootKey()
{
	CStringArray array;
	array.ExtractStrings( m_key, m_keyDelimiter );

	std::string result;
	if ( array.size() >= 1 )
		result = array[ 0 ];

	return result;
}
//----------------------------------------
bool CWorkspace::DeleteConfigFile()
{
	m_config->Clear();
	InitConfig();
	return true;
}
//----------------------------------------
void CWorkspace::InitConfig()
{
	assert__( m_configFileName == NormalizedPath( m_path + "/" + CWorkspace::m_configName ) );

	delete m_config;
	m_config = new CWorkspaceSettings( m_configFileName );
}
//----------------------------------------
bool CWorkspace::SaveConfig( std::string &errorMsg, CWorkspaceOperation *wkso, CWorkspaceDisplay *wksd, bool flush )
{
	UNUSED( errorMsg );		UNUSED( wkso );		UNUSED( wksd );		assert__( m_config != nullptr );

	bool bOk = DeleteConfigFile() && SaveCommonConfig();

	if ( flush )
		m_config->Sync();

	return bOk;
}
//----------------------------------------
bool CWorkspace::SaveCommonConfig( bool flush )
{
	assert__( m_config != nullptr );

	return m_config->SaveCommonConfig( *this, flush );
}
//----------------------------------------
bool CWorkspace::LoadConfig( std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso )
{
    UNUSED( errorMsg );			UNUSED( wks );     UNUSED( wksd );     UNUSED( wkso );

	return LoadCommonConfig();
}
//----------------------------------------
bool CWorkspace::LoadCommonConfig()
{
	assert__( m_config != nullptr );

	return m_config->LoadCommonConfig( *this );
}

bool CWorkspace::SetPath( const std::string& path, bool createPath )
{
	if ( path.empty() )
		return false;

	m_path = NormalizedPath( path );

	if ( IsDir( m_path ) || ( createPath && MakeDirectory( m_path ) ) )
	{
		m_configFileName = NormalizedPath( m_path + "/" + CWorkspace::m_configName );
		if ( IsFile( m_configFileName) )
			InitConfig();

		return true;
	}

	return false;
}
//----------------------------------------
bool CWorkspace::Rmdir()
{
	return DeleteDirectory( m_path );
	//RmdirRecurse();
	//return m_path.Rmdir();
}
//----------------------------------------
void CWorkspace::Dump( std::ostream& fOut /* = std::cerr */ )
{
	if ( !CTrace::IsTrace() )
		return;

	fOut << "==> Dump a CWorkspace Object at " << this << std::endl;
	fOut << "m_name = " << m_name << std::endl;
	fOut << "m_path object at = " << &m_path << std::endl;
	fOut << "m_path = " << m_path << std::endl;
	fOut << "m_configFileName object at = " << &m_configFileName << std::endl;
	fOut << "m_configFileName = " << m_configFileName << std::endl;
	fOut << "m_config object at = " << m_config << std::endl;

	fOut << "==> END Dump a CWorkspace Object at " << this << std::endl;

	fOut << std::endl;
}


//-------------------------------------------------------------
//------------------- CWorkspaceDataset class --------------------
//-------------------------------------------------------------

bool CWorkspaceDataset::IsRadsDataset( const std::string &name ) const
{
	return GetDataset< CRadsDataset>( name ) != nullptr;

}


bool CWorkspaceDataset::HasRadsDataset() const 
{ 
	if ( HasDataset() )
		for ( auto const &dataset_pair : *this )
		{
			if ( dynamic_cast< const CRadsDataset* >( dataset_pair.second ) )
				return true;
		}

	return false;
}


const std::string& CWorkspaceDataset::FindFirstStandardDataset() const
{
	if ( HasDataset() )
		for ( auto const &dataset_pair : *this )
		{
			if ( dynamic_cast< const CRadsDataset* >( dataset_pair.second ) )
				continue;

			const CDataset *d = dynamic_cast<const CDataset*>( dataset_pair.second );		assert__( d );
			return d->GetName();
		}

	return empty_string();
}


//----------------------------------------
bool CWorkspaceDataset::Import( CWorkspace* wksi, std::string &errorMsg, CWorkspaceDataset *wks_data, CWorkspaceDisplay *wks_disp, CWorkspaceOperation *wks_op )
{
	UNUSED( wks_data );		UNUSED( wks_disp );		UNUSED( wks_op );

	if ( wksi == nullptr )
		return true;


	CWorkspaceDataset* wksToImport = dynamic_cast<CWorkspaceDataset*>( wksi );
	if ( wksToImport == nullptr )
		return true;

	if ( m_importBitSet == nullptr || !m_importBitSet->m_bitSet.test( IMPORT_DATASET_INDEX ) )
		return true;

	for ( CObMap::const_iterator it = wksToImport->GetDatasets()->begin(); it != wksToImport->GetDatasets()->end(); it++ )
	{
		CDataset* datasetImport = dynamic_cast<CDataset*>( it->second );
		if ( datasetImport == nullptr )
		{
			errorMsg += "ERROR in  CWorkspaceDataset::Import\ndynamic_cast<CDataset*>(it->second) returns nullptr pointer"
				"\nList seems to contain objects other than those of the class CDataset";

			//wxMessageBox( ,				"Error",				wxOK | wxCENTRE | wxICON_ERROR );

			return false;
		}

		CDataset* dataset = GetDataset( datasetImport->GetName() );
		if ( dataset != nullptr )
		{
			errorMsg +=
				"Dataset to import '"
				+ datasetImport->GetName()
				+ "':\nUnable to process, a dataset with the same name already exists\n"
				+ "Import cancelled";

			//wxMessageBox( 				"Warning",				wxOK | wxICON_EXCLAMATION );

			return false;
		}

		m_datasets.Insert( datasetImport->GetName(), new CDataset( *datasetImport ) );

	}
	return true;
}

//----------------------------------------
// v4 note: Apparently not used...
//
//bool CWorkspaceDataset::CheckFiles( std::string &errorMsg )
//{
//	bool bOk = true;
//
//	for ( CObMap::iterator it = m_datasets.begin(); it != m_datasets.end(); it++ )
//	{
//		CDataset* dataset = dynamic_cast<CDataset*>( it->second );
//		if ( dataset == nullptr )
//		{
//			errorMsg +=
//				"ERROR in  CWorkspaceDataset::CheckFiles\ndynamic_cast<CDataset*>(it->second) returns nullptr pointer"
//				"\nList seems to contain objects other than those of the class CDataset";
//
//			return false;
//		}
//
//		try
//		{
//			dataset->CheckFiles();
//		}
//		catch ( CException& e )
//		{
//			bOk = false;
//			errorMsg +=
//				"Dataset '"
//				+ dataset->GetName()
//				+ "':\nUnable to process files. Please apply correction\n\nReason:\n"
//				+ e.what()
//				+ "\n";
//		}
//	}
//
//	return bOk;
//}
//----------------------------------------
bool CWorkspaceDataset::SaveConfig( std::string &errorMsg, CWorkspaceOperation *wkso, CWorkspaceDisplay *wksd, bool flush )
{
    UNUSED( wksd );     UNUSED( wkso );		assert__( m_config != nullptr );

	bool bOk = DeleteConfigFile() && SaveCommonConfig() && SaveConfigDataset( errorMsg );

	if ( flush )
		m_config->Sync();

	return bOk;
}
//----------------------------------------
bool CWorkspaceDataset::SaveConfigDataset( std::string &errorMsg )
{
	assert__( m_config != nullptr );

	return m_config->SaveConfigDataset( *this, errorMsg );
}

//----------------------------------------
bool CWorkspaceDataset::LoadConfig( std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso )
{
    UNUSED( wks );        UNUSED( wksd );          UNUSED( wkso );			assert__( m_config != nullptr );

	return LoadCommonConfig() && LoadConfigDataset( errorMsg );
}

//----------------------------------------
bool CWorkspaceDataset::LoadConfigDataset( std::string &errorMsg )
{
	assert__( m_config != nullptr );

	return m_config->LoadConfigDataset( *this, errorMsg );
}
//----------------------------------------

bool CWorkspaceDataset::RenameDataset( CDataset *dataset, const std::string& new_name )
{
	if ( dataset->GetName() != new_name )
	{
		if ( !m_datasets.RenameKey( dataset->GetName(), new_name ) )
			return false;

		dataset->SetName( new_name );
	}

	return true;
}

//----------------------------------------
bool CWorkspaceDataset::DeleteDataset( CDataset* dataset )
{
	return m_datasets.Erase( dataset->GetName() );
}

//----------------------------------------
std::string CWorkspaceDataset::GetDatasetNewName( bool RADS )
{
	size_t i = m_datasets.size();
	std::string key;

	do
	{
		key = ( RADS ? "RADS_" : "" ) + NAME + "_" + n2s< std::string >( i + 1 );
		i++;
	} 
	while ( m_datasets.Exists( key ) != nullptr );

	return key;
}
//----------------------------------------
void CWorkspaceDataset::GetDatasetNames( std::vector< std::string >& array ) const
{
	for ( CObMap::const_iterator it = m_datasets.begin(); it != m_datasets.end(); it++ )
	{
		array.push_back( it->first );
	}
}
//----------------------------------------
bool CWorkspaceDataset::InsertDataset( const std::string& name, std::function< CDataset*(const std::string &) > factory )
{
    if ( m_datasets.Exists(name) )
    {
        return false;
    }

    m_datasets.Insert(name, factory(name) );

    return true;
}
//----------------------------------------
void CWorkspaceDataset::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  CWorkspace::Dump(fOut);

  fOut << "==> Dump a CWorkspaceDataset Object at "<< this << std::endl;

  fOut << "==> END Dump a CWorkspaceDataset Object at "<< this << std::endl;

  fOut << std::endl;

}

//-------------------------------------------------------------
//------------------- CWorkspaceFormula class --------------------
//-------------------------------------------------------------

//----------------------------------------
void CWorkspaceFormula::AmendFormulas(const CStringArray& keys, const CProductInfo &pi, const std::string& record)
{
  m_formulas.Amend(keys, pi, record);
  //SaveConfigPredefinedFormula();			//commented out v3.1.0
  SaveConfigFormula();

}

//----------------------------------------
void CWorkspaceFormula::SetFormulasToImport(const std::vector< std::string >& array)
{
  m_formulasToImport.clear();
  m_formulasToImport = array;
}

//----------------------------------------
bool CWorkspaceFormula::IsFormulaToImport( const std::string& name )
{
	const size_t n = m_formulasToImport.size();
	for ( size_t i = 0; i < n; i++ )
		if ( str_icmp( name, m_formulasToImport[ i ] ) )
			return true;

	return false;
}
//----------------------------------------
bool CWorkspaceFormula::Import( CWorkspace* wksi, std::string &errorMsg, CWorkspaceDataset *wks_data, CWorkspaceDisplay *wks_disp, CWorkspaceOperation *wks_op )
{
    UNUSED( wks_data );		UNUSED( wks_op );   UNUSED( wks_disp );

	if ( wksi == nullptr )
		return true;

	CWorkspaceFormula* wksToImport = dynamic_cast<CWorkspaceFormula*>( wksi );
	if ( wksToImport == nullptr )
		return true;

	if ( m_importBitSet == nullptr )
		return true;

	if ( m_importBitSet->m_bitSet.test( IMPORT_FORMULA_INDEX ) == false )
		return true;

	size_t countImport = m_formulasToImport.size();

	for ( CObMap::iterator it = wksToImport->GetFormulas()->begin(); it != wksToImport->GetFormulas()->end(); it++ )
	{
		CFormula* formulaImport = dynamic_cast<CFormula*>( it->second );
		assert__( formulaImport != nullptr );
		//{
		//  wxMessageBox("ERROR in  CWorkspaceFormula::Import\ndynamic_cast<CFormula*>(it->second) returns nullptr pointer"
		//               "\nList seems to contain objects other than those of the class CFormula",
		//               "Error",
		//                wxOK | wxCENTRE | wxICON_ERROR);
		//  return false;
		//}
		if ( formulaImport->IsPredefined() )
			continue;

		std::string formulaNewName = formulaImport->GetName();

		// if import array is not empty : test if import or not
		// else import all formulas
		if ( countImport > 0 && !IsFormulaToImport( formulaNewName ) )
			continue;

		CFormula* formula = GetFormula( formulaImport->GetName() );
		if ( formula != nullptr )
		{
			CFormula* formulaTmp = nullptr;
			do
			{
				formulaNewName += "_from_";
				formulaNewName += wksToImport->GetRootKey();
				formulaTmp = GetFormula( formulaNewName );

			} while ( formulaTmp != nullptr );

			errorMsg +=
				"Import formula '"
				+ formulaImport->GetName()
				+ "':\n\nA formula with the same name already exists.\n\nImported formula has been renamed to:\n\n '"
				+ formulaNewName
				+ "'\n";
		}

		CFormula* newFormula = new CFormula( *formulaImport );
		newFormula->SetName( formulaNewName );

		m_formulas.Insert( newFormula->GetName(), newFormula, false );
	}

	return true;
}

//----------------------------------------
bool CWorkspaceFormula::AddFormula( CFormula& formula, std::string &errorMsg )
{
	return m_formulas.InsertUserDefined_ReplacePredefinedNotAllowed( formula, errorMsg );
}
//----------------------------------------
void CWorkspaceFormula::RemoveFormula(const std::string& name)
{
  m_formulas.Erase((const char *)name.c_str());
}
//----------------------------------------
std::string CWorkspaceFormula::GetCommentFormula(const std::string& name)
{
  return m_formulas.GetCommentFormula(name);
}
//----------------------------------------
std::string CWorkspaceFormula::GetDescFormula(const std::string& name, bool alias)
{
  return m_formulas.GetDescFormula(name, alias);
}
//----------------------------------------
CFormula* CWorkspaceFormula::GetFormula(const std::string& name)
{
  return dynamic_cast<CFormula*>(m_formulas.Exists((const char *)name.c_str()));
}
//----------------------------------------
const CFormula* CWorkspaceFormula::GetFormula( CMapFormula::const_iterator it ) const
{
	return dynamic_cast<const CFormula*>( it->second );
}
CFormula* CWorkspaceFormula::GetFormula(CMapFormula::iterator it)
{
  return dynamic_cast<CFormula*>(it->second);
}
//----------------------------------------
void CWorkspaceFormula::GetFormulaNames( CStringMap& stringMap, bool getPredefined /* = true*/, bool getUser /* = true*/ )
{
	stringMap.RemoveAll();

	for ( CMapFormula::iterator it = m_formulas.begin(); it != m_formulas.end(); it++ )
	{
		CFormula* formula = GetFormula( it );
		if ( formula != nullptr )
		{
			if ( ( getPredefined && formula->IsPredefined() ) || ( getUser && !formula->IsPredefined() ) )
			{
				stringMap.Insert( formula->GetName(), formula->GetDescription() );
			}
		}
	}
}
//----------------------------------------
void CWorkspaceFormula::GetFormulaNames( std::vector< std::string >& array, bool getPredefined /* = true*/, bool getUser /* = true*/ ) const
{
	for ( CMapFormula::const_iterator it = m_formulas.begin(); it != m_formulas.end(); it++ )
	{
		const CFormula* formula = GetFormula( it );
		if ( formula != nullptr )
		{
			if ( ( getPredefined && formula->IsPredefined() ) || ( getUser && !formula->IsPredefined() ) )
			{
				array.push_back( it->first );
			}
		}
	}
}

//----------------------------------------
bool CWorkspaceFormula::SaveConfig( std::string &errorMsg, CWorkspaceOperation *wkso, CWorkspaceDisplay *wksd, bool flush )
{
    UNUSED( errorMsg );		UNUSED( wkso );          UNUSED( wksd );		assert__( m_config != nullptr );

	bool bOk = DeleteConfigFile() && SaveCommonConfig() && SaveConfigFormula();
	if ( flush )
		m_config->Sync();

	return bOk;
}
//----------------------------------------
bool CWorkspaceFormula::SaveConfigFormula()
{
	assert__( m_config != nullptr );

	return m_formulas.SaveConfig( m_config, false );
}
//----------------------------------------
bool CWorkspaceFormula::LoadConfig( std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso )
{
    UNUSED( wks );        UNUSED( wksd );          UNUSED( wkso );		assert__( m_config != nullptr );

	return LoadCommonConfig() && LoadConfigFormula( errorMsg );
}
//----------------------------------------
bool CWorkspaceFormula::LoadConfigFormula( std::string &errorMsg )
{
	assert__( m_config != nullptr );

	return m_formulas.InsertUserDefined( m_config, errorMsg );
}
//----------------------------------------
void CWorkspaceFormula::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  CWorkspace::Dump(fOut);

  fOut << "==> Dump a CWorkspaceFormula Object at "<< this << std::endl;

  fOut << "==> END Dump a CWorkspaceFormula Object at "<< this << std::endl;

  fOut << std::endl;

}

//-------------------------------------------------------------
//------------------- CWorkspaceOperation class --------------------
//-------------------------------------------------------------
//----------------------------------------
//bool CWorkspaceOperation::Import( CWorkspace* wksi, std::string &errorMsg, CWorkspaceDataset *wks_data, CWorkspaceDisplay *wks_disp, CWorkspaceOperation *wks_op )
//{
//    UNUSED( wks_disp );
//
//	if ( wksi == nullptr )
//		return true;
//
//	CWorkspaceOperation* wksToImport = dynamic_cast<CWorkspaceOperation*>( wksi );
//	if ( wksToImport == nullptr )
//	{
//		return true;
//	}
//
//	if ( m_importBitSet == nullptr )
//	{
//		return true;
//	}
//
//	if ( m_importBitSet->m_bitSet.test( IMPORT_OPERATION_INDEX ) == false )
//	{
//		return true;
//	}
//
//
//	CObMap::iterator it;
//	for ( it = wksToImport->GetOperations()->begin(); it != wksToImport->GetOperations()->end(); it++ )
//	{
//		COperation* operationImport = dynamic_cast<COperation*>( it->second );
//		if ( operationImport == nullptr )
//		{
//			errorMsg += "ERROR in  CWorkspaceOperation::Import\ndynamic_cast<COperation*>(it->second) returns nullptr pointer"
//				"\nList seems to contain objects other than those of the class COperation\n";
//
//			return false;
//		}
//
//		COperation* operation = GetOperation( operationImport->GetName() );
//		if ( operation != nullptr )
//		{
//			errorMsg += 
//				"Operation to import '"
//				+ operationImport->GetName()
//				+ "':\nUnable to process, an operation with the same name already exists\n"
//				+ "Import cancelled\n";
//
//			return false;
//		}
//
//		m_operations.Insert( operationImport->GetName(), COperation::Copy( *operationImport, wks_op, wks_data ) );
//	}
//
//	return true;
//}

//----------------------------------------
bool CWorkspaceOperation::RenameOperation( COperation* operation, const std::string& new_name )
{
	if ( str_cmp( QuickOperationName(), new_name ) )
		return false;

	if ( str_cmp( operation->GetName(), new_name ) )
		return true;

	if ( !m_operations.RenameKey( operation->GetName(), new_name ) )
		return false;

	operation->SetName( new_name );

	return true;
}
//----------------------------------------
bool CWorkspaceOperation::SaveConfig( std::string &errorMsg, CWorkspaceOperation *wkso, CWorkspaceDisplay *wksd, bool flush )		//flush = true
{
	UNUSED( wkso );		UNUSED( wksd );			assert__( m_config != nullptr );

	bool bOk = DeleteConfigFile() && SaveCommonConfig() && SaveConfigOperation( errorMsg );
	if ( flush )
		m_config->Sync();

	return bOk;
}

//----------------------------------------
bool CWorkspaceOperation::SaveConfigOperation( std::string &errorMsg )
{
	assert__( m_config != nullptr );

	return m_config->SaveConfigOperation( *this, errorMsg );
}

//----------------------------------------
bool CWorkspaceOperation::LoadConfig( std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso )
{
	UNUSED( wksd );			assert__( m_config != nullptr );

	return LoadCommonConfig() && LoadConfigOperation( errorMsg, wks, wkso );
}

//----------------------------------------
bool CWorkspaceOperation::LoadConfigOperation( std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceOperation *wkso )
{
	assert__( m_config != nullptr );

	return m_config->LoadConfigOperation( *this, errorMsg, wks, wkso );
}
//----------------------------------------
std::string CWorkspaceOperation::GetOperationNewName()
{
	size_t i = m_operations.size();
	std::string key;

	do
	{
		key = NAME + "_" + n2s< std::string >( i + 1 );		//the suffix ensures also that the name does not clash with QuickOperationName()
		i++;
	} while ( m_operations.Exists( key ) != nullptr );

	return key;
}

COperation* CWorkspaceOperation::GetOperation( const std::string& name )
{
	return dynamic_cast<COperation*>( m_operations.Exists( name ) );
}
const COperation* CWorkspaceOperation::GetOperation( const std::string& name ) const
{
	return const_cast<CWorkspaceOperation*>( this )->GetOperation( name );
}


//----------------------------------------
std::string CWorkspaceOperation::GetOperationCopyName( const std::string& baseName )
{
	int32_t i = 0;
	std::string key;
	do
	{
		if ( i == 0 )
		{
			key = baseName + "_copy";
		}
		else
		{
			key = baseName + "_copy" + n2s<std::string>( i );
		}
		
		i++;

	} while ( m_operations.Exists( key ) != nullptr );

	return key;
}

//----------------------------------------
bool CWorkspaceOperation::InsertOperation(const std::string &name)
{
    if (m_operations.Exists(name))
    {
        return false;
    }

    m_operations.Insert(name, new COperation(name));
    return true;
}
//----------------------------------------
bool CWorkspaceOperation::InsertOperation( const std::string &name, COperation* operationToCopy, CWorkspaceDataset *wksds )
{
    if (m_operations.Exists(name))
    {
        return false;
    }

    // Create and copy the operation
    COperation* newOperation = COperation::Copy( *operationToCopy, this, wksds );

    // Set the correct names
    newOperation->SetName(name);
    newOperation->InitOutputs( this );    //newOperation->InitExportAsciiOutput( this );

    m_operations.Insert( name, newOperation );
    return true;
}
//----------------------------------------
void CWorkspaceOperation::GetOperationNames( std::vector< std::string >& array )
{
	for ( CObMap::iterator it = m_operations.begin(); it != m_operations.end(); it++ )
		array.push_back( it->first );
}
//----------------------------------------
bool CWorkspaceOperation::UseDataset( const std::string& name, CStringArray *operation_names )	//operation_names = nullptr 
{
	bool use = false;
	for ( CObMap::iterator it = m_operations.begin(); it != m_operations.end(); it++ )
	{
		COperation* operation = dynamic_cast<COperation*>( it->second );		assert__( operation != nullptr );

		bool useIt = operation->UseDataset( name );
		use = use | useIt;

		if ( operation_names != nullptr )
		{
			if ( useIt )
			{
				operation_names->Insert( operation->GetName() );
			}

		}
		else
		{
			if ( use )
			{
				break;
			}
		}
	}

	return use;
}
bool CWorkspaceOperation::UseFilter( const std::string& name, CStringArray* operation_names )		//operation_names = nullptr 
{
	bool use = false;
	for ( CObMap::iterator it = m_operations.begin(); it != m_operations.end(); it++ )
	{
		COperation* operation = dynamic_cast<COperation*>( it->second );		assert__( operation != nullptr );

		bool useIt = operation->FilterName() == name;
		use |= useIt;

		if ( operation_names != nullptr )
		{
			if ( useIt )
			{
				operation_names->Insert( operation->GetName() );
			}
		}
		else
		{
			if ( use )
			{
				break;
			}
		}
	}

	return use;
}

//----------------------------------------
bool CWorkspaceOperation::DeleteOperation( COperation* operation )
{
	return m_operations.Erase( operation->GetName() );
}

//----------------------------------------
void CWorkspaceOperation::Dump( std::ostream& fOut /* = std::cerr */ )
{
	if ( !CTrace::IsTrace() )
		return;

	CWorkspace::Dump( fOut );

	fOut << "==> Dump a CWorkspaceOperation Object at " << this << std::endl;

	fOut << "==> END Dump a CWorkspaceOperation Object at " << this << std::endl;

	fOut << std::endl;
}

//-------------------------------------------------------------
//------------------- CWorkspaceDisplay class --------------------
//-------------------------------------------------------------

bool CWorkspaceDisplay::Import( CWorkspace* wksi, std::string &errorMsg, CWorkspaceDataset *wks_data, CWorkspaceDisplay *wks_disp, CWorkspaceOperation *wks_op )
{
	UNUSED( wks_data );

	if ( wksi == nullptr )
		return true;

	CWorkspaceDisplay* wksToImport = dynamic_cast<CWorkspaceDisplay*>( wksi );
	if ( wksToImport == nullptr )
	{
		return true;
	}

	if ( m_importBitSet == nullptr )
	{
		return true;
	}

	if ( m_importBitSet->m_bitSet.test( IMPORT_DISPLAY_INDEX ) == false )
	{
		return true;
	}

	for ( CObMap::iterator it = wksToImport->GetDisplays()->begin(); it != wksToImport->GetDisplays()->end(); it++ )
	{
		CDisplay* displayImport = dynamic_cast<CDisplay*>( it->second );
		if ( displayImport == nullptr )
		{
			errorMsg += 
				"ERROR in  CWorkspaceDisplay::Import\ndynamic_cast<CDisplay*>(it->second) returns nullptr pointer"
				"\nList seems to contain objects other than those of the class CDisplay";

			return false;
		}

		CDisplay* display = GetDisplay( displayImport->GetName() );
		if ( display != nullptr )
		{
			errorMsg += 
				"Operation to import '"
				+ displayImport->GetName()
				+ "':\nUnable to process, a display with the same name already exists\n"
				+ "Import canceled\n";

			return false;
		}

		m_displays.Insert( displayImport->GetName(), new CDisplay( *displayImport, wks_disp, wks_op ) );
	}

	return true;
}
//----------------------------------------
bool CWorkspaceDisplay::UseOperation( const std::string& name, std::string &errorMsg, CStringArray* displayNames /*= nullptr*/ )
{
	bool useOperation = false;

    for ( CObMap::iterator it = m_displays.begin(); it != m_displays.end(); it++ )
	{
		CDisplay* display = dynamic_cast<CDisplay*>( it->second );
		if ( display == nullptr )
		{
			errorMsg += 
				"ERROR in  CWorkspaceDisplay::UseOperation\ndynamic_cast<CDisplay*>(it->second) returns nullptr pointer"
				"\nList seems to contain objects other than those of the class CDisplay\n";

			return false;
		}

		bool useIt = display->UsesOperation( name );
		useOperation |= useIt;

		if ( displayNames != nullptr )
		{
			if ( useIt )
			{
                displayNames->Insert( display->GetName() );
			}
		}
		else
		{
			if ( useOperation )
			{
				break;
			}
		}
	}

	return useOperation;
}
//----------------------------------------
bool CWorkspaceDisplay::SaveConfig( std::string &errorMsg, CWorkspaceOperation *wkso, CWorkspaceDisplay *wksd, bool flush )
{
     UNUSED( wkso );			assert__( m_config != nullptr );

	bool bOk = DeleteConfigFile() && SaveCommonConfig() && SaveConfigDisplay( errorMsg, wksd );

	if ( flush )
		m_config->Sync();

	return bOk;
}
//----------------------------------------
bool CWorkspaceDisplay::SaveConfigDisplay( std::string &errorMsg, CWorkspaceDisplay *wksd )
{
	assert__( m_config != nullptr );

	return m_config->SaveConfigDisplay( *this, errorMsg, wksd );
}

//----------------------------------------
bool CWorkspaceDisplay::LoadConfig( std::string &errorMsg, CWorkspaceDataset *wksds, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso )
{
	UNUSED( wksds );				assert__( m_config != nullptr );

	return LoadCommonConfig() && LoadConfigDisplay( errorMsg, wksd, wkso );
}
//----------------------------------------
bool CWorkspaceDisplay::LoadConfigDisplay( std::string &errorMsg, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso )
{
	assert__( m_config != nullptr );

	return m_config->LoadConfigDisplay( *this, errorMsg, wksd, wkso );
}
//----------------------------------------
bool CWorkspaceDisplay::RenameDisplay( CDisplay* display, const std::string& new_name )
{
	if ( str_cmp( display->GetName(), new_name ) )
		return true;

	if ( !m_displays.RenameKey( (const char *)display->GetName().c_str(), (const char *)new_name.c_str() ) )
		return false;

	display->SetName( new_name );

	return true;
}



CDisplay* CWorkspaceDisplay::GetDisplay( const std::string& name )
{
	return dynamic_cast<CDisplay*>( m_displays.Exists( name ) );
}


//----------------------------------------
std::string CWorkspaceDisplay::GetDisplayNewName( const COperation *operation )
{
#if !defined (BRAT_V3)
	assert__( operation );
#endif

	size_t i = m_displays.size();
	std::string key;
	const std::string op_name = operation ? operation->GetName() : "";

	do
	{
		key = op_name + "_" + NAME + "_" + n2s< std::string >( i + 1 );
		i++;
	} 
	while ( m_displays.Exists( key ) != nullptr );

	return key;
}
//----------------------------------------
CDisplay* CWorkspaceDisplay::CloneDisplay( const CDisplay *display, const COperation *operation, const CWorkspaceOperation *wkso )
{
	std::string display_name = GetDisplayNewName( operation );		assert__( !m_displays.Exists( display_name ) );
	CDisplay *d = display->Clone( display_name, this, wkso );
    m_displays.Insert( display_name, d );
    return d;
}
bool CWorkspaceDisplay::InsertDisplay(const std::string &name)
{
    if (m_displays.Exists(name))
    {
        return false;
    }

    m_displays.Insert(name, new CDisplay(name));
    return true;
}
//----------------------------------------
void CWorkspaceDisplay::GetDisplayNames( std::vector< std::string >& array )
{
	for ( CObMap::iterator it = m_displays.begin(); it != m_displays.end(); it++ )
		array.push_back( it->first );
}
//----------------------------------------
bool CWorkspaceDisplay::DeleteDisplay( CDisplay* display )
{
	return m_displays.Erase( display->GetName() );
}





// After executing an operation, the following calls were made (in views tab)
//
//	CDisplayPanel::GetOperations(); - inserted all operations data in member m_dataList
//	which then was loaded in available data tree widget:
//
//	CDisplayPanel::GetDispavailtreectrl()->InsertData( &m_dataList );
//
//  When a field was dragged from this tree to the selected data list
//	 a succession of calls CDisplayPanel::AddField resulted in
//
//		m_display->InsertData( GetDataKey( *data ).ToStdString(), newdata );
//
//	This function is based on all that.
//
//	The parameter update is an attempt to maintain display consistency when the 
//	associated operation is executed again: if update is true, existing fields 
//	not in operation are removed
//
std::vector< CDisplay* > CWorkspaceDisplay::CreateDisplays4Operation( const COperation *operation, CMapDisplayData *dataList, bool split_plots )
{
	std::vector< CDisplay*> v;

	CInternalFiles *file = nullptr;
	std::vector< CMapTypeDisp::ETypeDisp > displayTypes = operation->GetDisplayTypes( &file );	//must close returned files, if any
	if ( file == nullptr )
	{
		return v;
	}

	//CMapDisplayData *dataList = new CMapDisplayData;		//v3: dataList, data member

	CStringArray names;
	names.RemoveAll();
	file->GetDataVars( names );

	for ( auto disp_type : displayTypes )
	{
		CDisplay *display = nullptr;
		CDisplayData *north = nullptr;
		CDisplayData *east = nullptr;

		for ( CStringArray::iterator itField = names.begin(); itField != names.end(); itField++ )
		{
			if ( !display )
			{
				std::string display_name = GetDisplayNewName( operation );
				InsertDisplay( display_name );
				display = GetDisplay( display_name );
				display->InitOutput( this );							assert__( display->GetDataCount() == 0 );
				v.push_back( display );
			}

			CStringArray varDimensions;
			file->GetVarDims( *itField, varDimensions );

			size_t nbDims = varDimensions.size();
			if ( nbDims > 2 )						// (*) see below
			{
				continue;
			}
			if ( ( nbDims != 2 ) && ( disp_type == CMapTypeDisp::eTypeDispZFXY || disp_type == CMapTypeDisp::eTypeDispZFLatLon ) )
			{
				continue;
			}

			//create with field name and unit

			CDisplayData *display_data = new CDisplayData( operation, display, disp_type, *itField, file->GetUnit( *itField ).GetText() );

			//description
			std::string comment = file->GetComment( *itField );
			std::string description = file->GetTitle( *itField );
			if ( !comment.empty() )
			{
				description += ( "." + comment );
			}
			display_data->SetFieldDescription( description );

            CFormula *f = operation->GetFormula( *itField );
            if ( f )
            {
				if ( f->IsNorthComponent() )
					north = display_data;

				if ( f->IsEastComponent() )
					east = display_data;

                display_data->SetNorthComponent( f->IsNorthComponent() );
                display_data->SetEastComponent( f->IsEastComponent() );
            }

			//dims / axis
			size_t supported_dims = std::min( nbDims, (size_t)3 );		assert__( supported_dims < 3 );//given (*) above, 3 is never reached, but let v3 be...
			for ( size_t idim = 0; idim < supported_dims; ++idim )
			{
				const std::string &dimName = varDimensions.at( idim );
				display_data->SetDimension( idim, dimName, file->GetTitle( dimName ), file->GetUnit( dimName ).GetText() );
			}

			dataList->Insert( display_data->GetDataKey(), display_data, false );
			display->InsertData( display_data->GetDataKey(), display_data );
			if ( split_plots )
				display = nullptr;
		}

		// TODO : check vector and relation with split plots
		//
		//if ( north && east )
		//{
  //          north->SetAsNorthComponent( east );
  //          east->SetAsEastComponent( north );
		//}
	}

	delete file;	//file->Close();		//critical

	return v;
}
  //wxString errorMsg;
  //bOk = m_display->GetDataSelected()->CheckFields(errorMsg, m_display);

  //if ( ! bOk )
  //{
  //  GetDispgroupfields()->SetValue(false);
  //  SetValueFieldGroup();

  //  wxString msg = wxString::Format("View '%s':\n\nData expressions can't be displayed on the same plot because of the following reasons:\n"
  //                                  "%s\n\n"
  //                                  "'%s' option have been unchecked.",
  //                                  m_display->GetName().c_str(),
  //                                  errorMsg.c_str(),
  //                                  GetDispgroupfields()->GetLabel().c_str());
  //  /*
  //  wxString msg = wxString::Format("Data fields can't be displayed on the same plot because of\nat least one of the following reasons:\n"
  //                                  " - number of dimensions is different\n"
  //                                  " - names of the dimensions are different\n"
  //                                  " - units of the dimensions are not in the same way (not compatible)\n"
  //                                  " - (only Y=F(X)) units of the fields are not in the same way (not compatible)\n\n"
  //                                  "'%s' option have been unchecked.",
  //                 GetDispgroupfields()->GetLabel().c_str());
  //  */
  //  wxMessageBox(msg,
  //              "Warning",
  //              wxOK | wxICON_EXCLAMATION);

  //}






//----------------------------------------
void CWorkspaceDisplay::Dump( std::ostream& fOut /* = std::cerr */ )
{
	if ( !CTrace::IsTrace() )
		return;

	CWorkspace::Dump( fOut );

	fOut << "==> Dump a CWorkspaceDisplay Object at " << this << std::endl;

	fOut << "==> END Dump a CWorkspaceDisplay Object at " << this << std::endl;

	fOut << std::endl;
}
