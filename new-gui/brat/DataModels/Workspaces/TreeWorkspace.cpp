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

#include <typeinfo>

#include "libbrathl/brathl.h"
#include "libbrathl/Tools.h"

#include "common/tools/Trace.h"
#include "common/tools/Exception.h"

#include "Workspace.h"
#include "TreeWorkspace.h"


using namespace brathl;



std::string CTreeWorkspace::ChildKey( const std::string &name ) const 
{
	return m_pTreeroot->GetKey() + CWorkspace::m_keyDelimiter + name;
}


CWorkspace* CTreeWorkspace::Reset( const std::string &internal_data_path, const std::string& base_path, std::string &error_msg, const std::string& root_name )
{
	Clear();

	if ( root_name.empty() && !IsDir( base_path ) )
	{
		error_msg += "The required workspace does not exist.";
		return nullptr;
	}

	CWorkspace *root= root_name.empty() ? new CWorkspace( base_path ) : new CWorkspace( root_name, base_path );
    root->InitConfig();

	// Set tree root
	SetRoot( root, true );

	//WARNING : the sequence of workspaces object creation is significant, because of the interdependence of them

	std::string path = root->GetPath();

	//FIRST - Create "Datasets" branch
	CWorkspaceDataset* wksDataSet = new CWorkspaceDataset( path );
    wksDataSet->InitConfig();
	AddChild( wksDataSet->GetName(), wksDataSet );

	//SECOND - Create "Formulas" branch
	CWorkspaceFormula* wksFormula = new CWorkspaceFormula( internal_data_path, error_msg, path );
    wksFormula->InitConfig();
    AddChild( wksFormula->GetName(), wksFormula );

	//THIRD - Create "Operations" branch
	CWorkspaceOperation* wksOperation = new CWorkspaceOperation( path );
    wksOperation->InitConfig();
    AddChild( wksOperation->GetName(), wksOperation );

	//FOURTH - Create "Displays" branch
	CWorkspaceDisplay* wksDisplay = new CWorkspaceDisplay( path );
    wksDisplay->InitConfig();
    AddChild( wksDisplay->GetName(), wksDisplay );

	return root;
}



// Default arguments: 	pe = nullptr, const std::string &name = "" , dest_wks = nullptr 
//static 
bool CTreeWorkspace::Validate( EValidationTask task, const std::string &path, EValidationError *pe,	
	const std::string &name, const CWorkspace *dest_wks )
{
	EValidationError e = eNoError;

	// lambdas 

	auto validate_name = [ &e ]( const std::string &name )
	{
		if ( name.empty() )
			e = eError_EmptyName;

		return e == eNoError;
	};


	auto validate_path_name = [ &e ]( const std::string &path )
	{
		if ( path.empty() )
			e = eError_EmptyPathName;

		return e == eNoError;
	};


	auto validate_path = [ &e, &validate_path_name ]( const std::string &path )
	{
		if ( validate_path_name( path ) && !IsDir( path ) )
			e = eError_NotExistingPath;

		return e == eNoError;
	};


	auto validate_workspace_configuration = [ &e, &validate_path ]( const std::string &path )
	{
		if ( validate_path( path ) )
		{
			CWorkspace wks( path );						//wks.SetPath( path, false );
			std::string error_msg;
			if (
				! wks.IsConfigFile() ||
				! wks.LoadConfig( error_msg, nullptr, nullptr, nullptr
					//wxGetApp().GetCurrentWorkspaceDataset(), 
					//wxGetApp().GetCurrentWorkspaceDisplay(), 
					//wxGetApp().GetCurrentWorkspaceOperation() 
				) ||
				! wks.IsRoot()
				)
				e = eError_InvalidConfig;
		}

		return e == eNoError;
	};


	auto validate_not_workspace_path = [ &e, &validate_workspace_configuration ]( const std::string &name, const std::string &path )
	{
		if ( validate_workspace_configuration( CWorkspace::MakeNewWorkspacePath( name, path ) ) )
			e = eError_WorkspaceAlreadyExists;
		else
			e = eNoError;	//clear error set by validate_workspace_configuration, which means success in this case

		return e == eNoError;
	};


	auto validate_not_self_importing = [ &e, &dest_wks ]( const std::string &path )
	{
		assert__(dest_wks);

		if ( str_icmp( path, dest_wks->GetPath() ) )	// even in case sensitive systems, forbid 
			e = eError_SelfImporting;

		return e == eNoError;
	};


	// body

	switch ( task )
	{
		case eNew:
			validate_name( name ) && 
			validate_path_name( path ) &&
			validate_not_workspace_path( name, path );
			break;

		case eImport:
			if ( !validate_not_self_importing( path ) )
				break;
		case eOpen:
		case eRename:
			validate_workspace_configuration( path );
			break;

		case eDelete:
			validate_path_name( path ) &&
			validate_workspace_configuration( path );
			break;

		default:
			assert__( false );
	}

	if ( pe )
		*pe = e;

	return e == eNoError;
}


//----------------------------------------
void CTreeWorkspace::SetRoot( CBratObject* o, bool goCurrent )
{
	CWorkspace *x = dynamic_cast<CWorkspace*>( o );		assert__( x );
	x->SetKey( x->GetName() );
	base_t::SetRoot( x->GetName(), x, goCurrent );
	x->SetLevel( m_pTreeroot->GetLevel() );
}

//----------------------------------------
CWorkspaceFormula* CTreeWorkspace::GetWorkspaceFormula()
{
	CWorkspaceFormula *wks = nullptr;

	if ( GetRootData() != nullptr )
	{
		SetWalkDownRootPivot();
		do
		{
			wks = GetWorkspaceFormula( GetCurrentData(), false );
			if ( wks != nullptr )
				break;

		} while ( this->SubTreeWalkDown() );
	}
	return wks;
}
//----------------------------------------
CWorkspaceFormula* CTreeWorkspace::GetWorkspaceFormula( CWorkspace *w, bool withExcept )
{
	CWorkspaceFormula *wks  = dynamic_cast<CWorkspaceFormula*>( w );
	if ( wks == nullptr )
	{
		if ( withExcept )
		{
			CException e( "ERROR in CTreeWorkspace::GetWorkspaceFormula() - workspace is not a CWorkspaceFormula object", BRATHL_LOGIC_ERROR );
			throw( e );
		}
	}
	return wks;
}
//----------------------------------------
CWorkspace* CTreeWorkspace::GetCurrentData( bool withExcept )
{
	CWorkspace *wks  = dynamic_cast<CWorkspace*>( this->GetWalkCurrent()->GetData() );
	if ( wks == nullptr )
	{
		if ( withExcept )
		{
			CException e( "ERROR in CTreeWorkspace::GetCurrentData() - at least one of the tree object is not a CWorkspace object", BRATHL_LOGIC_ERROR );
			throw( e );
		}
	}
	return wks;
}
//----------------------------------------
CWorkspace* CTreeWorkspace::GetParentData( bool withExcept )
{
	CWorkspace *wks  = dynamic_cast<CWorkspace*>( this->GetWalkParent()->GetData() );
	if ( wks == nullptr )
	{
		if ( withExcept )
		{
			CException e( "ERROR in CTreeWorkspace::GetParentData() - at least one of the tree object is not a CWorkspace object", BRATHL_LOGIC_ERROR );
			throw( e );
		}
	}
	return wks;
}

//----------------------------------------
CWorkspace* CTreeWorkspace::FindWorkspace( const std::string key )
{
	assert__( !key.empty() );

	std::map< std::string, CObjectTreeNode* >::iterator it = m_nodemap.find( key );

	return dynamic_cast< CWorkspace* >( it->second->GetData() );
}		


//----------------------------------------
CWorkspace* CTreeWorkspace::GetRootData()
{
	if ( m_pTreeroot == nullptr )
		return nullptr;

	return dynamic_cast<CWorkspace*>( m_pTreeroot->GetData() );
}

//----------------------------------------
CObjectTreeIterator CTreeWorkspace::AddChild( const std::string& nm, CWorkspace* x, bool goCurrent )
{
	std::string key = ChildKey( nm );

	x->SetKey( key );

	x->SetLevel( m_pTreeroot->GetLevel() + 1 );

	return CObjectTree::AddChild( key, x, goCurrent );
}

//----------------------------------------
bool CTreeWorkspace::SaveConfig( std::string &error_msg, CWorkspaceOperation *wkso, CWorkspaceDisplay *wksd, bool flush )		//flush = true
{
	if ( GetRootData() == nullptr )
		return true;

	SetWalkDownRootPivot();

	bool bOk = true;

	do
	{
        bOk = bOk && GetCurrentData()->SaveConfig( error_msg, wkso, wksd, flush );
	} while ( this->SubTreeWalkDown() );

	return bOk;
}

//----------------------------------------
bool CTreeWorkspace::LoadConfig( CWorkspace *&wks, CWorkspaceDataset *wksds, CWorkspaceOperation *wkso, CWorkspaceDisplay *wksd, std::string &error_msg )
{
	if ( GetRootData() == nullptr )
		return true;

	SetWalkDownRootPivot();
	do
	{
		wks = GetCurrentData();
		wks->SetCtrlDatasetFiles( m_ctrlDatasetFiles );
		if ( !wks->LoadConfig( error_msg, wksds, wksd, wkso ) )
			return false;

	} while ( this->SubTreeWalkDown() );

	return true;
}
//----------------------------------------
CWorkspaceFormula* CTreeWorkspace::LoadConfigFormula( CWorkspaceDataset *wksds, CWorkspaceOperation *wkso, CWorkspaceDisplay *wksd, std::string &error_msg )
{
	CWorkspaceFormula* wks = GetWorkspaceFormula();
	if ( wks != nullptr )
		wks->LoadConfig( error_msg, wksds, wksd, wkso );

	return wks;
}

//----------------------------------------
bool CTreeWorkspace::Import( CTreeWorkspace* treeSrc, CWorkspaceDataset *wksds, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso, std::string &keyToFind, std::string &error_msg )
{
	assert__( GetRootData() != nullptr );		//v4: in v3 returned false


	bool bOk = true;

	SetWalkDownRootPivot();
	do
	{
		CWorkspace* currentWorkspace = GetCurrentData();
		if ( currentWorkspace == nullptr )
			continue;
        //if ( typeid( *GetCurrentData() ) == typeid( CWorkspace ) )
        //if ( dynamic_cast< CWorkspace* >( GetCurrentData() ) )
        if ( currentWorkspace->GetKey() != ChildKey( currentWorkspace->GetName() ) )
        {
			keyToFind = treeSrc->GetRootData()->GetKey();
		}
		else
		{
			auto cur_data_key = GetCurrentData()->GetKey();					  //!!! femm attention to substr !!!
			//wxString::Right returns the last count characters
			//keyToFind = treeSrc->GetRootData()->GetKey() + cur_data_key.Right( cur_data_key.length() -	GetRootData()->GetKey().length() );
			keyToFind = treeSrc->GetRootData()->GetKey() + cur_data_key.substr( GetRootData()->GetKey().length() );
		}

		CWorkspace* wksImport = dynamic_cast<CWorkspace*>( treeSrc->FindObject( keyToFind ) );
		if ( wksImport == nullptr )
			return false;

		//CWorkspace* currentWorkspace = GetCurrentData();
		//if ( currentWorkspace == nullptr )
		//	continue;

		currentWorkspace->SetImportBitSet( &m_importBitSet );

		bOk = currentWorkspace->Import( wksImport, error_msg, wksds, wksd, wkso );
		if ( !bOk )
			break;

	} while ( this->SubTreeWalkDown() );

	return bOk;
}

//----------------------------------------
void CTreeWorkspace::Dump( std::ostream& fOut /* = std::cerr */ )
{
	if ( !CTrace::IsTrace() )
		return;

	fOut << "==> Dump a CTreeWorkspace Object at " << this << std::endl;
	if ( m_pTreeroot == nullptr )
	{
		fOut << "==> END Dump a CTreeWorkspace Object at " << this << std::endl;
		return;
	}

	fOut << "==> Dump Tree only " << this << std::endl;

	this->SetWalkDownRootPivot();

	do
	{
		CWorkspace *wks  = dynamic_cast<CWorkspace*>( this->m_WalkCurrent->GetData() );
		if ( wks == nullptr )
		{
			CException e( "ERROR in CTreeWorkspace::Dump - at least one of the tree object is not a CWorkspace object", BRATHL_LOGIC_ERROR );
			throw( e );
		}
		std::string indent( this->m_WalkCurrent->GetLevel(), '-' );

		fOut << indent << wks->GetName() << "\t\t\t\t\t\tKey=" << this->m_WalkCurrent->GetKey() << std::endl;

	} while ( this->SubTreeWalkDown() );

	fOut << "==> END Dump Tree only " << this << std::endl;

	fOut << "==> Dump Tree  and CWorkspace  " << this << std::endl;
	CObjectTree::Dump( fOut );
	fOut << "==> END Dump Tree  and CWorkspace  " << this << std::endl;

	fOut << "==> END Dump a CTreeWorkspace Object at " << this << std::endl;

	fOut << std::endl;
}
