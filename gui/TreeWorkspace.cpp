
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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "TreeWorkspace.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <typeinfo>

#include "brathl.h"

//#include <string>

#include "Trace.h"
#include "Tools.h"
#include "Exception.h"
using namespace brathl;

#include "TreeWorkspace.h"



const wxString CTreeWorkspace::m_keyDelimiter = ".";

//----------------------------------------

CTreeWorkspace::CTreeWorkspace()
{
  Init();
}

//----------------------------------------


CTreeWorkspace::~CTreeWorkspace()
{
}

//----------------------------------------
void CTreeWorkspace::Init()
{
  m_ctrlDatasetFiles = true;
}
//----------------------------------------
void CTreeWorkspace::SetRoot(const wxString& nm, CWorkspace* x, bool goCurrent)
{
  x->SetKey(nm);
  CObjectTree::SetRoot((const char *)nm.c_str(), x, goCurrent);
  x->SetLevel(m_pTreeroot->GetLevel());

}

//----------------------------------------
CWorkspaceFormula * CTreeWorkspace::GetWorkspaceFormula()
{
  CWorkspaceFormula *wks = NULL;

  bool bOk = true;

  if (GetRootData() == NULL)
  {
    return wks;
  }

  SetWalkDownRootPivot();

  do
  {
   wks = GetWorkspaceFormula(GetCurrentData(), false);

    if (wks != NULL)
    {
      break;
    }
  }
  while (this->SubTreeWalkDown());

  return wks;
}
//----------------------------------------
CWorkspaceFormula * CTreeWorkspace::GetWorkspaceFormula(CWorkspace *w, bool withExcept)
{
  CWorkspaceFormula *wks  = dynamic_cast<CWorkspaceFormula*>(w);
  if (withExcept)
  {
    if (wks == NULL)
    {
      CException e("ERROR in CTreeWorkspace::GetWorkspaceFormula() - workspace is not a CWorkspaceFormula object", BRATHL_LOGIC_ERROR);
      throw(e);
    }
  }

  return wks;

}
//----------------------------------------
CWorkspace * CTreeWorkspace::GetCurrentData(bool withExcept)
{
  CWorkspace *wks  = dynamic_cast<CWorkspace*>(this->GetWalkCurrent()->GetData());
  if (withExcept)
  {
    if (wks == NULL)
    {
      CException e("ERROR in CTreeWorkspace::GetCurrentData() - at least one of the tree object is not a CWorkspace object", BRATHL_LOGIC_ERROR);
      throw(e);
    }
  }

  return wks;
}
//----------------------------------------

CWorkspace * CTreeWorkspace::GetParentData(bool withExcept)
{
  CWorkspace *wks  = dynamic_cast<CWorkspace*>(this->GetWalkParent()->GetData());
  if (withExcept)
  {
    if (wks == NULL)
    {
      CException e("ERROR in CTreeWorkspace::GetParentData() - at least one of the tree object is not a CWorkspace object", BRATHL_LOGIC_ERROR);
      throw(e);
    }
  }

  return wks;
}

//----------------------------------------

CWorkspace* CTreeWorkspace::FindParent(CWorkspace* wks)
{
  return dynamic_cast<CWorkspace*>(CObjectTree::FindParentObject((const char *)wks->GetKey().c_str()));

}

//----------------------------------------

CWorkspace* CTreeWorkspace::GetRootData()
{
  if (m_pTreeroot == NULL)
  {
    return NULL;
  }

  return dynamic_cast<CWorkspace*>(m_pTreeroot->GetData());

}


//----------------------------------------

CObjectTreeIterator CTreeWorkspace::AddChild (CObjectTreeNode* parent, const wxString& nm, CWorkspace* x, bool goCurrent)
{

  wxString key = parent->GetKey().c_str();
  key += CTreeWorkspace::m_keyDelimiter + nm;

  x->SetKey(key);
  x->SetLevel(parent->GetLevel() + 1);

  return CObjectTree::AddChild(parent, (const char *)key.c_str(), x, goCurrent);

}
//----------------------------------------

CObjectTreeIterator CTreeWorkspace::AddChild (CObjectTreeIterator& parent, const wxString& nm, CWorkspace* x, bool goCurrent)
{

  wxString key = parent.m_Node->second->GetKey().c_str();
  key += CTreeWorkspace::m_keyDelimiter + nm;

  x->SetKey(key);
  x->SetLevel(parent.m_Node->second->GetLevel() + 1);
  return CObjectTree::AddChild(parent, (const char *)key.c_str(), x, goCurrent);
}

//----------------------------------------
CObjectTreeIterator CTreeWorkspace::AddChild (const wxString& nm, CWorkspace* x, bool goCurrent)
{
  wxString key = m_pTreeroot->GetKey().c_str();
  key += CTreeWorkspace::m_keyDelimiter + nm;

  x->SetKey(key);
  x->SetLevel(m_pTreeroot->GetLevel() + 1);

  return CObjectTree::AddChild((const char *)key.c_str(), x, goCurrent);
}

//----------------------------------------
bool CTreeWorkspace::SaveConfig(bool flush)
{
  bool bOk = true;

  if (GetRootData() == NULL)
  {
    return true;
  }

  SetWalkDownRootPivot();

  do
  {
    bOk &= GetCurrentData()->SaveConfig(flush);
  }
  while (this->SubTreeWalkDown());

  return bOk;

}

//----------------------------------------
bool CTreeWorkspace::LoadConfig()
{
  bool bOk = true;

  if (GetRootData() == NULL)
  {
    return true;
  }

  SetWalkDownRootPivot();

  do
  {
    CWorkspace* wks = GetCurrentData();

    wks->SetCtrlDatasetFiles(m_ctrlDatasetFiles);

    bOk &= wks->LoadConfig();
    if (bOk == false)
    {
      wxMessageBox("Unable to load workspace.",
                   "Warning",
                    wxOK | wxICON_EXCLAMATION);
      return false;
    }
  }
  while (this->SubTreeWalkDown());

  return true;
}
//----------------------------------------
CWorkspaceFormula* CTreeWorkspace::LoadConfigFormula()
{
  bool bOk = false;

  CWorkspaceFormula* wks = GetWorkspaceFormula();

  if (wks != NULL)
  {
    bOk = wks->LoadConfig();
  }

  return wks;
}

//----------------------------------------
bool CTreeWorkspace::Import(CTreeWorkspace* treeSrc)
{
  bool bOk = true;

  if (GetRootData() == NULL)
  {
    wxMessageBox("Unable to import workspace. No root found",
                 "Warning",
                  wxOK | wxCENTRE | wxICON_EXCLAMATION);
    return false;
  }

  SetWalkDownRootPivot();
  wxString keyToFind;

  do
  {
    if (typeid(*GetCurrentData()) == typeid(CWorkspace))
    {
      keyToFind = treeSrc->GetRootData()->GetKey();
    }
    else
    {
      keyToFind = treeSrc->GetRootData()->GetKey() +
                  GetCurrentData()->GetKey().Right( GetCurrentData()->GetKey().Length() -
                                                    GetRootData()->GetKey().Length());
    }


    CWorkspace* wksImport = dynamic_cast<CWorkspace*>(treeSrc->FindObject((const char *)keyToFind.c_str()));
    if (wksImport == NULL)
    {
      wxMessageBox(wxString::Format("Unable to import workspace. Workspace '%s' doesn't contain '%s'",
                                    treeSrc->GetRootData()->GetName().c_str(),
                                    GetCurrentData()->GetKey().c_str()),
                   "Warning",
                    wxOK | wxCENTRE | wxICON_EXCLAMATION);
      return false;
    }

    CWorkspace* currentWorkspace = GetCurrentData();
    if (currentWorkspace == NULL)
    {
      continue;
    }

    currentWorkspace->SetImportBitSet(&m_importBitSet);

    bOk = currentWorkspace->Import(wksImport);
    if (bOk == false)
    {
      break;
    }
  }
  while (this->SubTreeWalkDown());

  return bOk;
}

//----------------------------------------
void CTreeWorkspace::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }


  fOut << "==> Dump a CTreeWorkspace Object at "<< this << std::endl;
  if (m_pTreeroot == NULL)
  {
    fOut << "==> END Dump a CTreeWorkspace Object at "<< this << std::endl;
    return;
  }

  fOut << "==> Dump Tree only "<< this << std::endl;

  this->SetWalkDownRootPivot();

  do
  {
    CWorkspace *wks  = dynamic_cast<CWorkspace*>(this->m_WalkCurrent->GetData());
    if (wks == NULL)
    {
      CException e("ERROR in CTreeWorkspace::Dump - at least one of the tree object is not a CWorkspace object", BRATHL_LOGIC_ERROR);
      throw(e);
    }
    wxString indent(this->m_WalkCurrent->GetLevel(), '-');

    fOut << indent << wks->GetName() << "\t\t\t\t\t\tKey=" << this->m_WalkCurrent->GetKey() << std::endl;

  }while (this->SubTreeWalkDown());

  fOut << "==> END Dump Tree only "<< this << std::endl;

  fOut << "==> Dump Tree  and CWorkspace  "<< this << std::endl;
  CObjectTree::Dump(fOut);
  fOut << "==> END Dump Tree  and CWorkspace  "<< this << std::endl;

  fOut << "==> END Dump a CTreeWorkspace Object at "<< this << std::endl;

  fOut << std::endl;

}

