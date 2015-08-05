
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

#include "brathl_error.h" 
#include "brathl.h" 

#include "Stl.h" 

#include "TraceLog.h" 
#include "Tools.h" 
#include "Exception.h" 
#include "TreeField.h" 

using namespace brathl;


namespace brathl
{


const string CTreeField::m_keyDelimiter = ".";

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

CObjectTreeIterator CTreeField::AddChild (CObjectTreeNode* parent, const string& nm, CField* x, bool goCurrent)
{

  string key = parent->GetKey() + CTreeField::m_keyDelimiter + nm;

  x->SetKey(key);
  
  return CObjectTree::AddChild(parent, key, x, goCurrent);

} 
//----------------------------------------

CObjectTreeIterator CTreeField::AddChild (CObjectTreeIterator& parent, const string& nm, CField* x, bool goCurrent)
{

  string key = parent.m_Node->second->GetKey() + CTreeField::m_keyDelimiter + nm;

  x->SetKey(key);
  
  return CObjectTree::AddChild(parent, key, x, goCurrent);
}

//----------------------------------------
CObjectTreeIterator CTreeField::AddChild (const string& nm, CField* x, bool goCurrent) 
{
  string key = m_pTreeroot->GetKey() + CTreeField::m_keyDelimiter + nm;

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
void CTreeField::DumpDictionary(const string& outputFileName)
{
  ostream* fOut = NULL;
  ofstream* fStream = NULL;

  if (outputFileName.compare("cout") == 0)
  {
    fOut = &cout;
  }
  else if (outputFileName.compare("cerr") == 0)
  {
    fOut = &cerr;
  }
  else
  {

    fStream = new ofstream;
    fStream->open(outputFileName.c_str(), ios::out | ios::trunc);
    if (fStream->good() != true)
    {
      cerr << "CTreeField::DumpDictionary - Open file failed - file name  " << outputFileName << 
                " error " << fStream->rdstate() << endl;
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

void CTreeField::DumpDictionary(ostream& fOut /* = cout */)
{
  if (m_pTreeroot == NULL)
  {
    fOut << "==> Nothing to dump !!!"<< endl;
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
  fOut << endl;

  this->SetWalkDownRootPivot();    

  do
  {
    CField *field  = dynamic_cast<CField*>(this->m_WalkCurrent->GetData()); 
    if (field == NULL)
    {
      CException e("ERROR in CTreeField::DumpDictionary - at least one of the tree object is not a CField object", BRATHL_LOGIC_ERROR);
      throw(e);
    }
    string indent(this->m_WalkCurrent->GetLevel(), '-');

    fOut << this->m_WalkCurrent->GetLevel() << "\t"
         << typeid(*field).name() << "\t";
    field->DumpFieldDictionary(fOut);
    fOut << endl;

  }while (this->SubTreeWalkDown());


}

//----------------------------------------
void CTreeField::Dump(ostream& fOut /* = cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }


  fOut << "==> Dump a CTreeField Object at "<< this << endl;
  if (m_pTreeroot == NULL)
  {
    fOut << "==> END Dump a CTreeField Object at "<< this << endl;
    return;
  }

  fOut << "==> Dump Tree only "<< this << endl;

  this->SetWalkDownRootPivot();    

  do
  {
    CField *field  = dynamic_cast<CField*>(this->m_WalkCurrent->GetData()); 
    if (field == NULL)
    {
      CException e("ERROR in CTreeField::Dump - at least one of the tree object is not a CField object", BRATHL_LOGIC_ERROR);
      throw(e);
    }
    string indent(this->m_WalkCurrent->GetLevel(), '-');

    fOut << indent << field->GetName() << "\t\t\t\t\t\tKey=" << this->m_WalkCurrent->GetKey() << endl;

  }while (this->SubTreeWalkDown());

  fOut << "==> END Dump Tree only "<< this << endl;

  fOut << "==> Dump Tree  and CField  "<< this << endl;
  CObjectTree::Dump(fOut);
  fOut << "==> END Dump Tree  and CField  "<< this << endl;
  
  fOut << "==> END Dump a CTreeField Object at "<< this << endl;

  fOut << endl;

}




}
