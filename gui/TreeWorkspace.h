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

#include "List.h"
#include "ObjectTree.h"
#include "BitSet32.h"


class CWorkspace;
class CWorkspaceFormula;




/** 
  Tree of workspace elements management class.


 \version 1.0
*/

class CTreeWorkspace : public CObjectTree
{
	typedef CObjectTree base_t;

protected:
	CBitSet32 m_importBitSet;

	bool m_ctrlDatasetFiles = true;

public:
	/// Empty CTreeWorkspace ctor
	CTreeWorkspace()
	{}

	/// Destructor
	virtual ~CTreeWorkspace()
	{}

	virtual void SetRoot( const std::string& nm, CBratObject* x, bool goCurrent = false ) override;

	virtual CObjectTreeIterator AddChild( CObjectTreeNode* parent, const std::string& nm, CWorkspace* x, bool goCurrent = false );

	virtual CObjectTreeIterator AddChild( CObjectTreeIterator& parent, const std::string& nm, CWorkspace* x, bool goCurrent = false );

	virtual CObjectTreeIterator AddChild( const std::string& nm, CWorkspace* x, bool goCurrent = false );

	bool SaveConfig( bool flush = true );
	bool LoadConfig( CWorkspace *&wks );

	CWorkspaceFormula* LoadConfigFormula();

	bool Import( CTreeWorkspace* treeSrc, std::string &keyToFind );

	/// Dump function
	virtual void Dump( std::ostream& fOut = std::cerr );

	CWorkspaceFormula* GetWorkspaceFormula();
	static CWorkspaceFormula * GetWorkspaceFormula( CWorkspace *w, bool withExcept  = true );

	CWorkspace * GetCurrentData( bool withExcept = true );
	CWorkspace * GetParentData( bool withExcept  = true );

	CWorkspace* FindParent( CWorkspace* wks );

	CWorkspace* GetRootData();
	CBitSet32* GetImportBitSet() { return &m_importBitSet; };

	bool GetCtrlDatasetFiles() { return m_ctrlDatasetFiles; };
	void SetCtrlDatasetFiles( bool value ) { m_ctrlDatasetFiles = value; };
};


#endif // !defined(WORKSPACES_TREE_WORKSPACE_H)
