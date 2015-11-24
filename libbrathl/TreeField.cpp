
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


#include <cstdlib>
#include <cstdio>
#include <cstring> 
#include <typeinfo> 
#include <fstream> 
#include <string> 

#include "new-gui/Common/tools/brathl_error.h" 
#include "brathl.h" 

#include "new-gui/Common/tools/TraceLog.h" 
#include "Tools.h" 
#include "new-gui/Common/tools/Exception.h" 
#include "TreeField.h" 

using namespace brathl;


namespace brathl
{


const std::string CTreeField::m_keyDelimiter = ".";

//----------------------------------------

CTreeField::CTreeField()
{
  
}

//----------------------------------------


CTreeField::~CTreeField()
{
}
//----------------------------------------

CField * CTreeField::GetDataAsFieldObject(CObjectTreeNode* node, bool withExcept)
{
  CField *field  = dynamic_cast<CField*>(node->GetData()); 
  if (withExcept)
  {
    if (field == NULL)
    {
      CException e("ERROR in CTreeField::GetData() - at least one of the tree object is not a CField object", BRATHL_LOGIC_ERROR);
      throw(e);
    }
  }

  return field;
}
//----------------------------------------

CFieldRecord * CTreeField::GetDataAsFieldRecordObject(CObjectTreeNode* node, bool withExcept)
{
  CFieldRecord *field  = dynamic_cast<CFieldRecord*>(node->GetData()); 
  if (withExcept)
  {
    if (field == NULL)
    {
      CException e("ERROR in CTreeField::GetData() - at least one of the tree object is not a CFieldRecord object", BRATHL_LOGIC_ERROR);
      throw(e);
    }
  }

  return field;
}
//----------------------------------------

CField * CTreeField::GetCurrentData(bool withExcept)
{
  CField *field  = dynamic_cast<CField*>(this->GetWalkCurrent()->GetData()); 
  if (withExcept)
  {
    if (field == NULL)
    {
      CException e("ERROR in CTreeField::GetCurrentData() - at least one of the tree object is not a CField object", BRATHL_LOGIC_ERROR);
      throw(e);
    }
  }

  return field;
}
//----------------------------------------

CField * CTreeField::GetParentData(bool withExcept)
{
  CField *field  = dynamic_cast<CField*>(this->GetWalkParent()->GetData()); 
  if (withExcept)
  {
    if (field == NULL)
    {
      CException e("ERROR in CTreeField::GetParentData() - at least one of the tree object is not a CField object", BRATHL_LOGIC_ERROR);
      throw(e);
    }
  }

  return field;
}

//----------------------------------------

CField* CTreeField::FindParent(CField* field) 
{ 
  return dynamic_cast<CField*>(CObjectTree::FindParentObject(field->GetKey()));

}

//----------------------------------------

CField* CTreeField::GetRootData() 
{
  if (m_pTreeroot == NULL)
  {
    return NULL;
  }
  return dynamic_cast<CField*>(m_pTreeroot->GetData());

}


//----------------------------------------

CObjectTreeIterator CTreeField::AddChild (CObjectTreeNode* parent, const std::string& nm, CField* x, bool goCurrent)
{

  std::string key = parent->GetKey() + CTreeField::m_keyDelimiter + nm;

  x->SetKey(key);
  
  return CObjectTree::AddChild(parent, key, x, goCurrent);

} 
//----------------------------------------

CObjectTreeIterator CTreeField::AddChild (CObjectTreeIterator& parent, const std::string& nm, CField* x, bool goCurrent)
{

  std::string key = parent.m_Node->second->GetKey() + CTreeField::m_keyDelimiter + nm;

  x->SetKey(key);
  
  return CObjectTree::AddChild(parent, key, x, goCurrent);
}

//----------------------------------------
CObjectTreeIterator CTreeField::AddChild (const std::string& nm, CField* x, bool goCurrent) 
{
  std::string key = m_pTreeroot->GetKey() + CTreeField::m_keyDelimiter + nm;

  x->SetKey(key);
  
  return CObjectTree::AddChild(key, x, goCurrent);
}
//----------------------------------------
void CTreeField::ResetHiddenFlag () 
{
  if (m_pTreeroot == NULL)
  {
    return;
  }


  this->SetWalkDownRootPivot();    

  do
  {
    CField *field  = dynamic_cast<CField*>(this->m_WalkCurrent->GetData()); 
    if (field != NULL)
    {
      field->SetHidden(true);
    }

  }while (this->SubTreeWalkDown());
}

//----------------------------------------
void CTreeField::DumpDictionary(const std::string& outputFileName)
{
  std::ostream* fOut = NULL;
  std::ofstream* fStream = NULL;

  if (outputFileName.compare("std::cout") == 0)
  {
    fOut = &std::cout;
  }
  else if (outputFileName.compare("std::cerr") == 0)
  {
    fOut = &std::cerr;
  }
  else
  {

    fStream = new std::ofstream;
    fStream->open(outputFileName.c_str(), std::ios::out | std::ios::trunc);
    if (fStream->good() != true)
    {
      std::cerr << "CTreeField::DumpDictionary - Open file failed - file name  " << outputFileName << 
                " error " << fStream->rdstate() << std::endl;
      delete fStream;
      fStream = NULL;
      return;
    }
    fOut = fStream;
  }

  DumpDictionary(*fOut);

  if (fStream != NULL)
  {
    fStream->close();
    delete fStream;
    fStream = NULL;    
  }

}
//----------------------------------------

void CTreeField::DumpDictionary(std::ostream& fOut /* = std::cout */)
{
  if (m_pTreeroot == NULL)
  {
    fOut << "==> Nothing to dump !!!"<< std::endl;
    return;
  }

  fOut << "Level" << "\t"
       << "Class" << "\t"
       << "Name" << "\t"
       << "Unit" << "\t"
       << "Brat type class" << "\t"
       << "Brat special type" << "\t"
       << "Brat native type" << "\t"
       << "Metadata" << "\t"
       << "Index" << "\t"
       << "Description" << "\t"
       << "Fixed size ?" << "\t"
       << "Number dims" << "\t"
       << "Dimensions" << "\t"
       << "Number fields";
  fOut << std::endl;

  this->SetWalkDownRootPivot();    

  do
  {
    CField *field  = dynamic_cast<CField*>(this->m_WalkCurrent->GetData()); 
    if (field == NULL)
    {
      CException e("ERROR in CTreeField::DumpDictionary - at least one of the tree object is not a CField object", BRATHL_LOGIC_ERROR);
      throw(e);
    }
    std::string indent(this->m_WalkCurrent->GetLevel(), '-');

    fOut << this->m_WalkCurrent->GetLevel() << "\t"
         << typeid(*field).name() << "\t";
    field->DumpFieldDictionary(fOut);
    fOut << std::endl;

  }while (this->SubTreeWalkDown());


}

//----------------------------------------
void CTreeField::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }


  fOut << "==> Dump a CTreeField Object at "<< this << std::endl;
  if (m_pTreeroot == NULL)
  {
    fOut << "==> END Dump a CTreeField Object at "<< this << std::endl;
    return;
  }

  fOut << "==> Dump Tree only "<< this << std::endl;

  this->SetWalkDownRootPivot();    

  do
  {
    CField *field  = dynamic_cast<CField*>(this->m_WalkCurrent->GetData()); 
    if (field == NULL)
    {
      CException e("ERROR in CTreeField::Dump - at least one of the tree object is not a CField object", BRATHL_LOGIC_ERROR);
      throw(e);
    }
    std::string indent(this->m_WalkCurrent->GetLevel(), '-');

    fOut << indent << field->GetName() << "\t\t\t\t\t\tKey=" << this->m_WalkCurrent->GetKey() << std::endl;

  }while (this->SubTreeWalkDown());

  fOut << "==> END Dump Tree only "<< this << std::endl;

  fOut << "==> Dump Tree  and CField  "<< this << std::endl;
  CObjectTree::Dump(fOut);
  fOut << "==> END Dump Tree  and CField  "<< this << std::endl;
  
  fOut << "==> END Dump a CTreeField Object at "<< this << std::endl;

  fOut << std::endl;

}




}
