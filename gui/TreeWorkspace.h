/*
* 
*
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
#if !defined(_TreeWorkspace_h_)
#define _TreeWorkspace_h_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "TreeWorkspace.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "brathl.h" 

#include <string>

#include "List.h"
#include "ObjectTree.h"
using namespace brathl;

#include "Workspace.h"

/** 
  Tree of workspace elements management class.


 \version 1.0
*/

const int32_t IMPORT_DATASET_INDEX = 0;
const int32_t IMPORT_FORMULA_INDEX = 1;
const int32_t IMPORT_OPERATION_INDEX = 2;
const int32_t IMPORT_DISPLAY_INDEX = 3;

class CTreeWorkspace : public CObjectTree
{

public:
    
  /// Empty CTreeWorkspace ctor
  CTreeWorkspace();

  /// Destructor
  virtual ~CTreeWorkspace();

  virtual void SetRoot(const wxString& nm, CWorkspace* x, bool goCurrent = false);

  virtual CObjectTreeIterator AddChild (CObjectTreeNode* parent, const wxString& nm, CWorkspace* x, bool goCurrent = false);

  virtual CObjectTreeIterator AddChild (CObjectTreeIterator& parent, const wxString& nm, CWorkspace* x, bool goCurrent = false);

  virtual CObjectTreeIterator AddChild (const wxString& nm, CWorkspace* x, bool goCurrent = false); 
  
  bool SaveConfig(bool flush = true);
  bool LoadConfig();

  CWorkspaceFormula* LoadConfigFormula();

  bool Import(CTreeWorkspace* treeSrc);

  /// Dump function
  virtual void Dump(std::ostream& fOut = std::cerr);

  CWorkspaceFormula* GetWorkspaceFormula();
  static CWorkspaceFormula * GetWorkspaceFormula(CWorkspace *w, bool withExcept  = true);

  CWorkspace * GetCurrentData(bool withExcept = true);
  CWorkspace * GetParentData(bool withExcept  = true);

  CWorkspace* FindParent(CWorkspace* wks);

  CWorkspace* GetRootData();
  CBitSet32* GetImportBitSet() { return &m_importBitSet; };

  bool GetCtrlDatasetFiles () { return m_ctrlDatasetFiles;};
  void SetCtrlDatasetFiles (bool value) { m_ctrlDatasetFiles = value;};

protected:
  void Init();

protected:
  CBitSet32 m_importBitSet;
  
  bool m_ctrlDatasetFiles;

private:
  

public:
  static const wxString m_keyDelimiter;

protected:
		
		
private:
  

};


#endif // !defined(_TreeWorkspace_h_)
