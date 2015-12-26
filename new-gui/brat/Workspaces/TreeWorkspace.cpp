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

#include "brathl.h"

#include "new-gui/Common/tools/Trace.h"
#include "new-gui/Common/tools/Exception.h"
#include "Tools.h"

#include "Workspace.h"
#include "TreeWorkspace.h"


using namespace brathl;


//----------------------------------------
void CTreeWorkspace::SetRoot( const std::string& nm, CBratObject* o, bool goCurrent )
{
	CWorkspace *x = dynamic_cast<CWorkspace*>( o );		assert__( x );
	x->SetKey( nm );
	base_t::SetRoot( nm, x, goCurrent );
	x->SetLevel( m_pTreeroot->GetLevel() );
}

//----------------------------------------
CWorkspaceFormula * CTreeWorkspace::GetWorkspaceFormula()
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
CWorkspaceFormula * CTreeWorkspace::GetWorkspaceFormula( CWorkspace *w, bool withExcept )
{
	CWorkspaceFormula *wks  = dynamic_cast<CWorkspaceFormula*>( w );
	if ( withExcept )
	{
		if ( wks == nullptr )
		{
			CException e( "ERROR in CTreeWorkspace::GetWorkspaceFormula() - workspace is not a CWorkspaceFormula object", BRATHL_LOGIC_ERROR );
			throw( e );
		}
	}
	return wks;
}
//----------------------------------------
CWorkspace * CTreeWorkspace::GetCurrentData( bool withExcept )
{
	CWorkspace *wks  = dynamic_cast<CWorkspace*>( this->GetWalkCurrent()->GetData() );
	if ( withExcept )
	{
		if ( wks == nullptr )
		{
			CException e( "ERROR in CTreeWorkspace::GetCurrentData() - at least one of the tree object is not a CWorkspace object", BRATHL_LOGIC_ERROR );
			throw( e );
		}
	}
	return wks;
}
//----------------------------------------
CWorkspace * CTreeWorkspace::GetParentData( bool withExcept )
{
	CWorkspace *wks  = dynamic_cast<CWorkspace*>( this->GetWalkParent()->GetData() );
	if ( withExcept )
	{
		if ( wks == nullptr )
		{
			CException e( "ERROR in CTreeWorkspace::GetParentData() - at least one of the tree object is not a CWorkspace object", BRATHL_LOGIC_ERROR );
			throw( e );
		}
	}
	return wks;
}

//----------------------------------------
CWorkspace* CTreeWorkspace::FindParent( CWorkspace* wks )
{
	return dynamic_cast<CWorkspace*>( CObjectTree::FindParentObject( (const char *)wks->GetKey().c_str() ) );
}

//----------------------------------------
CWorkspace* CTreeWorkspace::GetRootData()
{
	if ( m_pTreeroot == nullptr )
		return nullptr;

	return dynamic_cast<CWorkspace*>( m_pTreeroot->GetData() );
}


//----------------------------------------

CObjectTreeIterator CTreeWorkspace::AddChild (CObjectTreeNode* parent, const std::string& nm, CWorkspace* x, bool goCurrent)
{

  std::string key = parent->GetKey().c_str();
  key += CWorkspace::m_keyDelimiter + nm;

  x->SetKey(key);
  x->SetLevel(parent->GetLevel() + 1);

  return CObjectTree::AddChild(parent, (const char *)key.c_str(), x, goCurrent);

}
//----------------------------------------

CObjectTreeIterator CTreeWorkspace::AddChild (CObjectTreeIterator& parent, const std::string& nm, CWorkspace* x, bool goCurrent)
{

  std::string key = parent.m_Node->second->GetKey().c_str();
  key += CWorkspace::m_keyDelimiter + nm;

  x->SetKey(key);
  x->SetLevel(parent.m_Node->second->GetLevel() + 1);
  return CObjectTree::AddChild(parent, (const char *)key.c_str(), x, goCurrent);
}

//----------------------------------------
CObjectTreeIterator CTreeWorkspace::AddChild (const std::string& nm, CWorkspace* x, bool goCurrent)
{
  std::string key = m_pTreeroot->GetKey().c_str();
  key += CWorkspace::m_keyDelimiter + nm;

  x->SetKey(key);
  x->SetLevel(m_pTreeroot->GetLevel() + 1);

  return CObjectTree::AddChild((const char *)key.c_str(), x, goCurrent);
}

//----------------------------------------
bool CTreeWorkspace::SaveConfig( std::string &errorMsg, CWorkspaceOperation *wkso, CWorkspaceDisplay *wksd, bool flush )		//flush = true
{
	if ( GetRootData() == nullptr )
		return true;

	SetWalkDownRootPivot();

	bool bOk = true;

	do
	{
		bOk &= GetCurrentData()->SaveConfig( errorMsg, wkso, wksd, flush );
	} while ( this->SubTreeWalkDown() );

	return bOk;
}

//----------------------------------------
bool CTreeWorkspace::LoadConfig( CWorkspace *&wks, CWorkspaceDataset *wksds, CWorkspaceOperation *wkso, CWorkspaceDisplay *wksd, std::string &errorMsg )
{
	if ( GetRootData() == nullptr )
		return true;

	SetWalkDownRootPivot();
	do
	{
		wks = GetCurrentData();
		wks->SetCtrlDatasetFiles( m_ctrlDatasetFiles );
		if ( !wks->LoadConfig( errorMsg, wksds, wksd, wkso ) )
			return false;

	} while ( this->SubTreeWalkDown() );

	return true;
}
//----------------------------------------
CWorkspaceFormula* CTreeWorkspace::LoadConfigFormula( CWorkspaceDataset *wksds, CWorkspaceOperation *wkso, CWorkspaceDisplay *wksd, std::string &errorMsg )
{
	CWorkspaceFormula* wks = GetWorkspaceFormula();
	if ( wks != nullptr )
		wks->LoadConfig( errorMsg, wksds, wksd, wkso );

	return wks;
}

//----------------------------------------
bool CTreeWorkspace::Import( CTreeWorkspace* treeSrc, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso, std::string &keyToFind, std::string &errorMsg )
{
	if ( GetRootData() == nullptr )
		return false;

	bool bOk = true;

	SetWalkDownRootPivot();
	do
	{
        //if ( typeid( *GetCurrentData() ) == typeid( CWorkspace ) )
        if ( dynamic_cast< CWorkspace* >( GetCurrentData() ) )
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

		CWorkspace* currentWorkspace = GetCurrentData();
		if ( currentWorkspace == nullptr )
			continue;

		currentWorkspace->SetImportBitSet( &m_importBitSet );

		bOk = currentWorkspace->Import( wksImport, errorMsg, wksd, wkso );
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
