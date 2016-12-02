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

#include "new-gui/Common/QtUtilsIO.h"
#include "BratLogger.h"
#include "BratSettings.h"
#include "Model.h"
#include "ApplicationPaths.h"
#include "Workspaces/Workspace.h"
#include "Workspaces/Display.h"


//static 
CModel *CModel::smInstance = nullptr;

//static 
CBratFilters *CModel::smBratFilters = nullptr;

//static 
bool CModel::smValidFilters = false;




//static 
CModel& CModel::CreateInstance( const CBratSettings &settings )
{
	assert__( !smInstance );

	smInstance = new CModel( settings );
	smBratFilters = CBratFilters::CreateInstance( settings.BratPaths().WorkspacesDirectory() );
	smValidFilters = smBratFilters->Load();
	if ( !smValidFilters )
	{
		delete smBratFilters;
		smBratFilters = nullptr;
	}
	return *smInstance;
}


//explicit 
CModel::CModel( const CBratSettings &settings ) 
	: mSettings( settings )
{}

//virtual 
CModel::~CModel()
{
	mTree.Clear();

	assert__( smInstance );
	delete smInstance;
	smInstance = nullptr;		//just in case
	delete smBratFilters;
	smBratFilters = nullptr;	//idem
}	
						

const CApplicationPaths& CModel::BratPaths() const 
{ 
	return mSettings.BratPaths(); 
}


CBratFilters& CModel::BratFilters() 
{ 
	if ( !BratFiltersValid() )
	{
		const std::string msg = "Trying to use invalid filter data structures.";
		LOG_WARN( msg );
		throw CException( msg );
	}

	assert__( smBratFilters );

	return *smBratFilters; 
}


//static 
CWorkspace* CModel::RootWorkspace( CTreeWorkspace &tree )
{
	return tree.GetRootData();
}

CWorkspace* CModel::RootWorkspace()
{
	return RootWorkspace( mTree );
}


//static 
template< class WKSPC >
WKSPC* CModel::Workspace( CTreeWorkspace &tree )
{
	CWorkspace* wks = RootWorkspace( tree );
	if ( !wks )								   	//admissible when no workspace loaded
		return nullptr;

	std::string workspaceKey = tree.ChildKey( WKSPC::NAME );

	return dynamic_cast< WKSPC* >( tree.FindWorkspace( workspaceKey ) );
}

template< class WKSPC >
const WKSPC* CModel::Workspace( const CTreeWorkspace &tree )
{
	return Workspace< WKSPC >( const_cast< CTreeWorkspace& >( tree ) );
}


CWorkspace* CModel::CreateWorkspace( const std::string& name, const std::string& path, std::string &error_msg )
{
    return mTree.CreateReset( mSettings.BratPaths().mInternalDataDir, name, path, error_msg );
}

CWorkspace* CModel::CreateTree( CTreeWorkspace &tree, const std::string& path, std::string &error_msg )
{
    return tree.LoadReset( smInstance->mSettings.BratPaths().mInternalDataDir, path, error_msg );
}



//static 
bool CModel::SaveWorkspace( CTreeWorkspace &tree, std::string &error_msg )
{
	return smBratFilters->Save() && tree.SaveConfig(
		error_msg,
		Workspace< CWorkspaceOperation >( tree ),
		Workspace< CWorkspaceDisplay >( tree ) );
}


//static 
CWorkspace* CModel::LoadWorkspace( CTreeWorkspace &tree, const std::string& path, std::string &error_msg )
{
	CWorkspace *wks = CreateTree( tree, path, error_msg );
	if ( wks )
	{
		CWorkspace *failed_wks = nullptr;
		if ( !tree.LoadConfig(
			failed_wks,
			Workspace< CWorkspaceDataset >( tree ),
			Workspace< CWorkspaceOperation >( tree ),
			Workspace< CWorkspaceDisplay >( tree ),
			error_msg
			) )
		{
			error_msg += ( "\nUnable to load sub-workspace '" + ( failed_wks ? failed_wks->GetName() : "" ) + "'." );
			return nullptr;
		}
	}

	return wks;
}


CWorkspace* CModel::LoadWorkspace( const std::string& path, std::string &error_msg )
{
	return LoadWorkspace( mTree, path, error_msg );
}


void CModel::Reset()
{
	mTree.Clear();
}



bool CModel::LoadImportFormulas( const std::string& path, std::vector< std::string > &v, bool predefined, bool user, std::string &error_msg )
{
	CTreeWorkspace import_tree;
	CWorkspace* wks = CreateTree( import_tree, path, error_msg );
	if ( !wks )
	{
		assert__( !error_msg.empty() );
		return false;
	}

	const CWorkspaceFormula* wksFormula =  import_tree.LoadConfigFormula(
		Workspace< CWorkspaceDataset >( import_tree ),
		Workspace< CWorkspaceOperation >( import_tree ),
		Workspace< CWorkspaceDisplay >( import_tree ), 
		error_msg 
		);

	if ( !wksFormula )
	{
		assert__( !error_msg.empty() );
		return false;
	}

	wksFormula->GetFormulaNames( v, predefined, user );

	import_tree.Clear();

	return true;
}


bool CModel::ImportWorkspace( const std::string& path, 
	bool datasets, bool formulas, bool operations, bool views, const std::vector< std::string > &vformulas, std::string &error_msg )
{
	bool bOk = false;               Q_UNUSED( bOk );

	CWorkspace *wks = RootWorkspace();			assert__( wks != nullptr );

	//wxGetApp().m_tree.GetImportBitSet()->m_bitSet.reset();

	std::string wks_path = wks->GetPath();	//wxGetApp().GetCurrentWorkspace()->GetPathName();
	//wxFileName currentWksPath;
	//currentWksPath.AssignDir( wks_path );
	//currentWksPath.MakeAbsolute();

	if ( !SaveWorkspace( error_msg ) )	// TODO SaveWorkspace failures not verified in v3
		return false;

	mTree.ResetImportBits();
	mTree.SetImportBits(
	{
		{ IMPORT_DATASET_INDEX, datasets },
		{ IMPORT_FORMULA_INDEX, formulas },
		{ IMPORT_OPERATION_INDEX, operations },
		{ IMPORT_DISPLAY_INDEX, views }
	} );

	//CWorkspace* import_wks = new CWorkspace( dlg.mPath );		//CWorkspace* wks = new CWorkspace( dlg.GetWksName()->GetValue(), dlg.mPath, false );

	//wxGetApp().CreateTree( wks, wxGetApp().m_treeImport );
	CTreeWorkspace import_tree;	 			//import_tree.Clear();		// TODO see if this is necessary on a new tree
	import_tree.SetCtrlDatasetFiles( mTree.GetImportBit( IMPORT_DATASET_INDEX ) );
	//wxGetApp().m_treeImport.SetCtrlDatasetFiles( wxGetApp().m_tree.GetImportBitSet()->m_bitSet.test( IMPORT_DATASET_INDEX ) );

	//-----------------------------------------
	//wxGetApp().SetCurrentTree( &( wxGetApp().m_treeImport ) );
	//-----------------------------------------

	//bOk = wxGetApp().m_treeImport.LoadConfig();
	CWorkspace* import_wks = LoadWorkspace( import_tree, path, error_msg );

	if ( !import_wks || !error_msg.empty() )			//TODO TEST THIS ERROR RECOVERY
	{
		//wxGetApp().m_treeImport.DeleteTree();
		mTree.Clear();

		bool oldValue = mTree.GetCtrlDatasetFiles();
		mTree.SetCtrlDatasetFiles( false );

		LoadWorkspace( wks_path, error_msg );

		mTree.SetCtrlDatasetFiles( oldValue );

		return false;
	}

	//-----------------------------------------
	//wxGetApp().SetCurrentTree( &( wxGetApp().m_tree ) );
	//-----------------------------------------

	// Retrieve formula to Import
	CWorkspaceFormula* wksFormula = Workspace< CWorkspaceFormula >( mTree );
	//CWorkspaceFormula* wksFormula = wxGetApp().GetCurrentWorkspaceFormula();

	//dlg.GetCheckedFormulas( wksFormula->GetFormulasToImport() );
	wksFormula->SetFormulasToImport( vformulas );

	std::string keyToFind;
	bool result = mTree.Import( 
		&import_tree, 
		Workspace< CWorkspaceDataset >( mTree ), 
		Workspace< CWorkspaceDisplay >( mTree ), 
		Workspace< CWorkspaceOperation >( mTree ), 
		keyToFind, 
		error_msg );

	if ( result )
	{
		SaveWorkspace( error_msg );

		CWorkspaceOperation* wksOpImport = Workspace< CWorkspaceOperation >( import_tree );
		CWorkspaceOperation* wksOp = Workspace< CWorkspaceOperation >( mTree );
		//CWorkspaceOperation* wksOp = wxGetApp().GetCurrentWorkspaceOperation();
		if ( wksOp != nullptr && wksOpImport != nullptr )
		{
			//CDirTraverserCopyFile traverserCopyFile( wksOp->GetPath(), "nc" );
			////path name given to wxDir is locked until wxDir object is deleted
			//wxDir dir;
			//dir.Open( wksOpImport->GetPathName() );
			//dir.Traverse( traverserCopyFile );
			TraverseDirectory( t2q( wksOpImport->GetPath() ), t2q( wksOp->GetPath() ), { "*.nc" } );
		}
	}

	//wxGetApp().m_treeImport.DeleteTree();
	import_tree.Clear();
	mTree.Clear();

	bool oldValue = mTree.GetCtrlDatasetFiles();
	mTree.SetCtrlDatasetFiles( false );

	result = LoadWorkspace( wks_path, error_msg );		//checks filters

	mTree.SetCtrlDatasetFiles( oldValue );

	return result;
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

template< class DISPLAY >
std::vector< DISPLAY* > CModel::OperationDisplays( const std::string &name )
{
	std::vector<DISPLAY*> v;
	const CWorkspaceDisplay *wkspcd = Workspace< CWorkspaceDisplay >( mTree );
	auto const *displays = wkspcd->GetDisplays();
	for ( auto &pair : *displays )
	{
		auto *display = dynamic_cast< DISPLAY* >( pair.second );		assert__( display );
		if ( display->UsesOperation( name ) )
			v.push_back( display );
	}

	return v;
}

std::vector<const CDisplay*> CModel::OperationDisplays( const std::string &name ) const
{
	return const_cast<CModel*>( this )->OperationDisplays< const CDisplay >( name );
}


std::vector<CDisplay*> CModel::OperationDisplays( const std::string &name )
{
	return OperationDisplays< CDisplay >( name );
}


bool CModel::DeleteOperationDisplays( const std::string &name )
{
	std::vector<CDisplay*> v = OperationDisplays( name );
	CWorkspaceDisplay *wkspcd = Workspace< CWorkspaceDisplay >( mTree );

	while ( v.begin() != v.end() )
	{
		if ( !wkspcd->DeleteDisplay( *v.begin() ) ) 
			return false;

		v.erase( v.begin() );
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_Model.cpp"
