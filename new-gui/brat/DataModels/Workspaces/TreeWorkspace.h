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
#if !defined(WORKSPACES_TREE_WORKSPACE_H)
#define WORKSPACES_TREE_WORKSPACE_H

#include "libbrathl/List.h"
#include "libbrathl/ObjectTree.h"
#include "BitSet32.h"


class CWorkspace;
class CWorkspaceFormula;
class CWorkspaceDataset;
class CWorkspaceOperation;
class CWorkspaceDisplay;



/** 
  Tree of workspace elements management class.


 \version 1.0
*/

class CTreeWorkspace : private CObjectTree
{
	// types 

	using base_t = CObjectTree;

public:

	//using const_iterator = std::vector< CObjectTreeNode* >::const_iterator;
	using const_iterator = std::map< std::string, CObjectTreeNode* >::const_iterator;

	//struct const_iterator : std::map< std::string, CObjectTreeNode* >::const_iterator
	//{
	//	typedef std::map< std::string, CObjectTreeNode* >::const_iterator base_t;
	//public:
	//	const_iterator( const const_iterator &o ) :
	//		base_t( o )
	//	{}
	//	const_iterator( const base_t &o ) :
	//		base_t( o )
	//	{}

	//	const CWorkspace* operator * ( ) const;
	//};


	enum EValidationTask
	{
		eNew,
		eOpen,
		eImport,
		eRename,
		eDelete,

		EValidationTasks_size
	};

	enum EValidationError
	{
		eNoError,
		eError_EmptyName,
		eError_EmptyPathName,
		eError_NotExistingPath,
		eError_InvalidConfig,
		eError_WorkspaceAlreadyExists,
		eError_SelfImporting,

		EValidationErrors_size
	};

	// static members

	//v4: new, based on WorkspaceDlg.cpp

	//	- name is only checked for task eNew; it can be passed empty in all other cases
	//	- path must never be empty, but must NOT contain a workspace if task is eNew
	//	- in all other cases, path must physically exist and contain valid workspace configuration files
	//	- if it returns false and "pe" is not null, *pe contains an EValidationError != eNoError
	//	- dest_wks is the destination of an import task and is only used when task==eImport; it must not 
	//		be null in that case
	//
	static bool Validate( EValidationTask task, const std::string &path, EValidationError *pe = nullptr, 
		const std::string &name = "", const CWorkspace *dest_wks = nullptr );


	// instance data

protected:

	CBitSet32 m_importBitSet;
	bool m_ctrlDatasetFiles = true;


	// construction / destruction

public:
	CTreeWorkspace()
	{}

	virtual ~CTreeWorkspace()
	{}

	//v4: new, based on BratGui.cpp

	CWorkspace* CreateReset( const std::string &internal_data_path, const std::string& root_name, const std::string& path, std::string &errorMsg )
	{
		return Reset( internal_data_path, path, errorMsg, root_name );
	}

	CWorkspace* LoadReset( const std::string &internal_data_path, const std::string& root_path, std::string &errorMsg )
	{
		return Reset( internal_data_path, root_path, errorMsg, "" );
	}

	//v4: given private inheritance (to prevent any uses of CObjectTree implementation outside this class) provide these accessors

	CWorkspace* FindWorkspace( const std::string key );

	void Clear() { DeleteTree(); }

	std::string ChildKey( const std::string &name ) const;


	const_iterator begin() const
	{
		//return GetRoot()->begin();
		return m_nodemap.begin();
	}

	const_iterator end() const
	{
		//return GetRoot()->end();
		return m_nodemap.end();
	}

protected:
	CWorkspace* Reset( const std::string &internal_data_path, const std::string& path, std::string &errorMsg, const std::string& root_name );

public:
	// get / set / add

	CWorkspace* GetRootData();
	
	const CWorkspace* GetRootData() const { return const_cast<CTreeWorkspace*>(this)->GetRootData(); }

    //CBitSet32* GetImportBitSet() { return &m_importBitSet; }
	void ResetImportBits()
	{
		m_importBitSet.m_bitSet.reset();
	}
    void SetImportBits( std::initializer_list< std::pair< size_t, bool > > pos_values )
    {
        for ( auto &pos_value : pos_values )
            m_importBitSet.m_bitSet.set( pos_value.first, pos_value.second );
    }
    bool GetImportBit( size_t pos )
	{
        return m_importBitSet.m_bitSet.test( pos );
    }

    bool GetCtrlDatasetFiles() { return m_ctrlDatasetFiles; }
    void SetCtrlDatasetFiles( bool value ) { m_ctrlDatasetFiles = value; }

	CWorkspace * GetCurrentData( bool withExcept = true );
	CWorkspace * GetParentData( bool withExcept  = true );

	CWorkspaceFormula* GetWorkspaceFormula();
	static CWorkspaceFormula * GetWorkspaceFormula( CWorkspace *w, bool withExcept  = true );

	void SetRoot( CBratObject* x, bool goCurrent = false );

	virtual CObjectTreeIterator AddChild( const std::string& nm, CWorkspace* x, bool goCurrent = false );


	// save /load / import

	bool SaveConfig( std::string &errorMsg, CWorkspaceOperation *wkso, CWorkspaceDisplay *wksd, bool flush = true );
	bool LoadConfig( CWorkspace *&wks, CWorkspaceDataset *wksds, CWorkspaceOperation *wkso, CWorkspaceDisplay *wksd, std::string &errorMsg );

	CWorkspaceFormula* LoadConfigFormula( CWorkspaceDataset *wksds, CWorkspaceOperation *wkso, CWorkspaceDisplay *wksd, std::string &errorMsg );

	bool Import( CTreeWorkspace* treeSrc, CWorkspaceDataset *wksds, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso, std::string &keyToFind, std::string &errorMsg );


	/// Dump function
    virtual void Dump( std::ostream& fOut = std::cerr ) override;
};


#endif // !defined(WORKSPACES_TREE_WORKSPACE_H)
