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

#include <string>

#include "Exception.h"
#include "ObjectTree.h"
using namespace brathl;



namespace brathl
{





//-------------------------------------------------------------
//------------------- CObjectTreeNode class --------------------
//-------------------------------------------------------------


//----------------------------------------

CObjectTreeNode::CObjectTreeNode(CObjectTree* tr) : m_parent(NULL) 
{

  m_LinkedTree = tr;
  m_parent = NULL;
}
	
//----------------------------------------
CObjectTreeNode::CObjectTreeNode(CObjectTree* tr, const std::string& key, CBratObject* x)
{
  m_key = key;
  m_data = x;
  m_LinkedTree = tr;

  m_parent = NULL;
}

//----------------------------------------
CObjectTreeNode::~CObjectTreeNode()
{
  if (m_data != NULL)
  {
    delete m_data;
    m_data = NULL;
  }
}
//----------------------------------------
void CObjectTreeNode::Delete(const std::string& key)
{
  vectorTreeNode::iterator pchild;
  mapTreeNode::iterator itrmap;
  for ( pchild = m_children.begin(); pchild != m_children.end(); pchild++)
  {
    if ((*pchild)->m_key.compare(key) != 0)
    {
      continue;
    }
    (*pchild)->DeleteAllChildren();
    itrmap = m_LinkedTree->m_nodemap.find((*pchild)->m_key);
    m_LinkedTree->m_nodemap.erase(itrmap);
    delete *pchild;
    m_children.erase(pchild);
    
    break;
    //after removing the node, iterater is advanced
  }

}
//----------------------------------------
void CObjectTreeNode::DeleteAllChildren()
{
	vectorTreeNode::iterator pchild;
	while ( m_children.size() )
	{
		pchild = m_children.begin();
		( *pchild )->DeleteAllChildren();
		mapTreeNode::iterator itrmap = m_LinkedTree->m_nodemap.find( ( *pchild )->m_key );
		m_LinkedTree->m_nodemap.erase( itrmap );
		delete *pchild;
		m_children.erase( pchild );
		//after removing the node, iterater is advanced
	}
	//m_children.clear();
}
/*
void CObjectTreeNode::DeleteAllChildren()
{

  vectorTreeNode::iterator pchild;
  mapTreeNode::iterator itrmap;
  for ( pchild = m_children.begin(); pchild != m_children.end(); )
  {
    (*pchild)->DeleteAllChildren();
    itrmap = m_LinkedTree->m_nodemap.find((*pchild)->m_key);
    m_LinkedTree->m_nodemap.erase(itrmap);
    delete *pchild;
    m_children.erase(pchild);
    //after removing the node, iterater is advanced
  }
}
*/
//----------------------------------------
void CObjectTreeNode::AddChild (CObjectTreeNode* child)
{
  child->m_parent = this;
  child->m_level = this->m_level + 1;
  m_children.push_back(child);
};




//----------------------------------------



//-------------------------------------------------------------
//------------------- CObjectTree class --------------------
//-------------------------------------------------------------
CObjectTree::CObjectTree()
{
  m_pTreeroot = NULL;
  m_WalkPivot = NULL;
  m_WalkCurrent = NULL;
  m_WalkParent = NULL;


};

//----------------------------------------

CObjectTree::CObjectTree(const std::string& nm, CBratObject* x)
{
  m_pTreeroot = NULL;
  m_WalkPivot = NULL;
  m_WalkCurrent = NULL;
  m_WalkParent = NULL;

  SetRoot(nm, x);
}

//----------------------------------------

CObjectTree::~CObjectTree()
{
  DeleteTree();
}
//----------------------------------------

void CObjectTree::DeleteAllChildren(CObjectTreeIterator& itr) 
{ 
  itr.m_Node->second->DeleteAllChildren(); 
}
//----------------------------------------
void CObjectTree::DeleteTree() 
{ 

  if (m_pTreeroot != NULL) 
  {
    m_pTreeroot->DeleteAllChildren();
    delete m_pTreeroot;
  }

  m_pTreeroot = NULL;
  m_WalkCurrent = NULL;
  m_WalkParent = NULL;
  m_WalkPivot = NULL;

  //Just clear the map, do not delete CObjectTreeNode objects stored in the map 
  // because thy are not a copy, but just a reference pointer.
  m_nodemap.clear();

}


//----------------------------------------

void CObjectTree::SetRoot(const std::string& nm, CBratObject* x, bool goCurrent)
{
  DeleteTree();

  CObjectTreeNode* pNode;

  pNode = new CObjectTreeNode(this, nm, x);
  m_nodemap.insert(mapTreeNode::value_type(nm, pNode));
  m_pTreeroot = pNode;
  pNode->SetLevel(1);
  
 if (goCurrent)
  {
    m_WalkPivot = m_pTreeroot;
    m_WalkCurrent = m_WalkPivot;
    m_WalkCurrent->m_current = m_WalkCurrent->GetChildren().begin();
    m_WalkParent = NULL;
  }

}

//----------------------------------------

CObjectTreeNode* CObjectTree::FindParentNode(const std::string& key) 
{ 
  CObjectTree::iterator it;
  
  it = find(key);

  if (it == this->end())
  {
    return NULL;
  }

  if (it.m_Node->second == NULL)
  {
    return NULL;
  }

  return it.m_Node->second->GetParent();


}
//----------------------------------------

CBratObject* CObjectTree::FindParentObject(const std::string& key) 
{ 
  CObjectTreeNode *treeNodeParent = FindParentNode(key) ;
  
  if (treeNodeParent == NULL)
  {
    return NULL;
  }

  return treeNodeParent->GetData();

}

//----------------------------------------

CObjectTreeNode* CObjectTree::FindNode(const std::string& key) 
{ 
  CObjectTree::iterator it;
  
  it = find(key);

  if (it == this->end())
  {
    return NULL;
  }

  return it.m_Node->second;

}

//----------------------------------------

CBratObject* CObjectTree::FindObject(const std::string& key) 
{ 
  CObjectTreeNode *treeNode = FindNode(key) ;
  
  if (treeNode == NULL)
  {
    return NULL;
  }

  return treeNode->GetData();

}


//----------------------------------------

CObjectTreeIterator CObjectTree::AddChild (CObjectTreeNode* parent, const std::string& nm, CBratObject* x, bool goCurrent)
{

  if (parent == NULL)
  {
    CException e("CObjectTree::AddChild - parent is NULL", BRATHL_LOGIC_ERROR);
    throw(e);
  }

  CObjectTreeNode* pNew;
  pNew = new CObjectTreeNode(this, nm, x);

  parent->AddChild(pNew);
  
  if (goCurrent)
  {
    m_WalkPivot = pNew;
    m_WalkCurrent = m_WalkPivot;
    m_WalkCurrent->m_current = m_WalkCurrent->GetChildren().begin();
    m_WalkParent = parent;
  }

  std::pair<mapTreeNode::iterator, bool> pt = m_nodemap.insert(mapTreeNode::value_type(nm, pNew));

  CObjectTreeIterator _tmp;
  _tmp.m_Node = pt.first;
  return _tmp;


}
//----------------------------------------

CObjectTreeIterator CObjectTree::AddChild (CObjectTreeIterator& parent, const std::string& nm, CBratObject* x, bool goCurrent)
{
  CObjectTreeNode* pNew;

  pNew = new CObjectTreeNode(this, nm, x);

  parent.m_Node->second->AddChild(pNew);

  if (goCurrent)
  {
    m_WalkPivot = parent.m_Node->second;
    m_WalkCurrent = m_WalkPivot;
    m_WalkCurrent->m_current = m_WalkCurrent->GetChildren().begin();
    m_WalkParent = parent.m_Node->second;
  }

  std::pair<mapTreeNode::iterator, bool> pt = m_nodemap.insert(mapTreeNode::value_type(nm, pNew));

  CObjectTreeIterator _tmp;
  _tmp.m_Node = pt.first;
  return _tmp;
}

//----------------------------------------
CObjectTreeIterator CObjectTree::AddChild (const std::string& nm, CBratObject* x, bool goCurrent) 
{
  CObjectTreeNode* pNew;

  pNew = new CObjectTreeNode(this, nm, x);

  m_pTreeroot->AddChild(pNew);

  if (goCurrent)
  {
    m_WalkPivot = m_pTreeroot;
    m_WalkCurrent = m_WalkPivot;
    m_WalkCurrent->m_current = m_WalkCurrent->GetChildren().begin();
    m_WalkParent = NULL;
  }

  std::pair<mapTreeNode::iterator, bool> pt = m_nodemap.insert(mapTreeNode::value_type(nm, pNew));

  CObjectTreeIterator _tmp;
  _tmp.m_Node = pt.first;
  return _tmp;
}


//----------------------------------------
void CObjectTree::SetPostOrderSubTreePivot(CObjectTreeIterator& it) 
{
  m_WalkPivot = it.m_Node->second;
  m_WalkCurrent = m_WalkPivot;
  m_WalkCurrent->m_current = m_WalkCurrent->GetChildren().begin();

  while (m_WalkCurrent->GetChildren().size() != 0) 
  {
    m_WalkCurrent->m_current = m_WalkCurrent->GetChildren().begin();
    m_WalkCurrent = *(m_WalkCurrent->GetChildren().begin());
  }

  if (m_WalkCurrent != m_WalkPivot) 
  {
    m_WalkParent = m_WalkCurrent->GetParent();
  }
  else 
  {
    m_WalkParent = m_WalkCurrent;	//for the case no child in pivot
  }

}


//----------------------------------------
void CObjectTree::SetPostOrderRootPivot() 
{
  m_WalkPivot = m_pTreeroot;
  m_WalkCurrent = m_WalkPivot;
  m_WalkCurrent->m_current = m_WalkCurrent->GetChildren().begin();

  while (m_WalkCurrent->GetChildren().size() != 0) 
  {
    m_WalkCurrent->m_current = m_WalkCurrent->GetChildren().begin();
    m_WalkCurrent = *(m_WalkCurrent->GetChildren().begin());
  }

  if (m_WalkCurrent != m_WalkPivot) 
  {
    m_WalkParent = m_WalkCurrent->GetParent();
  }
  else 
  {
    m_WalkParent = m_WalkCurrent;	//for the case no child in pivot
  }

}

//----------------------------------------
void CObjectTree::SetWalkDownRootPivot()
{
  m_WalkPivot = m_pTreeroot;
  m_WalkCurrent = m_WalkPivot;
  m_WalkCurrent->m_current = m_WalkCurrent->GetChildren().begin();
  m_WalkParent = NULL;

}

//----------------------------------------

bool CObjectTree::SubTreePostOrderWalk() 
{

  if (m_WalkCurrent == m_WalkPivot)
  {
    return false;
  }

  //if not the parent's last child, advance one node in paraent's child
  //if the advanced child contains sub node, go in depth to the leftmost one
  if (++m_WalkParent->m_current != m_WalkParent->GetChildren().end()) 
  {
    m_WalkCurrent = *(m_WalkParent->m_current);
    while (m_WalkCurrent->GetChildren().size() != 0) 
    {
      //go down
      m_WalkCurrent->m_current = m_WalkCurrent->GetChildren().begin();
      m_WalkParent = m_WalkCurrent;
      m_WalkCurrent = *(m_WalkCurrent->m_current);
    }
  }
  else 
  {
    //if it's the last child of parent, we go up
    m_WalkCurrent = m_WalkParent;
    m_WalkParent = m_WalkCurrent->GetParent();
  }

  m_WalkParent = m_WalkCurrent->GetParent();

  return true;

}

//----------------------------------------

bool CObjectTree::SubTreeWalkDown() 
{

  //if it has children, we go down to children
  if (m_WalkCurrent->m_current != m_WalkCurrent->GetChildren().end()) 
  {
    m_WalkCurrent = *(m_WalkCurrent->m_current);
    m_WalkParent = m_WalkCurrent->GetParent();
    m_WalkParent->m_current++;	//advance to next child for next iteration
    m_WalkCurrent->m_current = m_WalkCurrent->GetChildren().begin();	//initialize to first child
    return true;

  }
  //if it's the last child of parent, we go up to the level
  //which we still need processing by recurively call ourself

  if (m_WalkCurrent == m_WalkPivot)
  {
    return false;		//no more child
  }
  m_WalkCurrent = m_WalkCurrent->GetParent();
  m_WalkParent = m_WalkCurrent->GetParent();

  return SubTreeWalkDown();
  
}


//----------------------------------------

bool CObjectTree::GoLevelDown(bool firstChild /*= true*/) 
{
  if (m_WalkCurrent == NULL)
  {
    return false;
  }
  
  if (m_WalkCurrent->GetChildren().size() == 0)
  {
    return false;
  }

  if (*(m_WalkCurrent->m_current) == NULL)
  {
    return false;
  }



  m_WalkParent = m_WalkCurrent;
  m_WalkPivot = *m_WalkCurrent->GetChildren().begin();
  m_WalkCurrent = m_WalkPivot;
  if (firstChild)
  {
    m_WalkCurrent->m_current = m_WalkCurrent->GetChildren().begin();
  }

  return true;


}

//----------------------------------------
bool CObjectTree::GoLevelUp(bool firstChild /*= true*/) 
{
  if (m_pTreeroot == NULL)
  {
    return false;
  }

  if (m_WalkCurrent == NULL)
  {
    return false;
  }

  if (m_WalkParent == NULL)
  {
    return false;
  }


  m_WalkPivot = m_WalkParent;
  m_WalkCurrent = m_WalkPivot;
  if (firstChild) 
  {
    m_WalkCurrent->m_current = m_WalkCurrent->GetChildren().begin();
  }
  m_WalkParent = m_WalkCurrent->GetParent();

  return true;


}

//----------------------------------------

void CObjectTree::GoToParent(const std::string& key) 
{
  Go( FindParentNode(key) );

}


//----------------------------------------

void CObjectTree::Go(CObjectTreeIterator child) 
{
  Go(child.m_Node->second);

}
//----------------------------------------

void CObjectTree::Go(CObjectTreeNode* child) 
{

  if (child == NULL)
  {
    return;
  }

  m_WalkPivot = child;
  m_WalkCurrent = m_WalkPivot;
  m_WalkCurrent->m_current = m_WalkCurrent->GetChildren().begin();
  m_WalkParent = m_WalkCurrent->GetParent();


}

//----------------------------------------
void CObjectTree::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }


  fOut << "==> Dump a CObjectTree Object at "<< this << std::endl;

  if (m_pTreeroot == NULL)
  {
    fOut << "==> END Dump a CObjectTree Object at "<< this << std::endl;
    return;
  }

  this->SetWalkDownRootPivot();    

  do
  {
    std::string indent(this->m_WalkCurrent->GetLevel(), '-');

    fOut << indent << this->m_WalkCurrent->GetKey()<< std::endl;
    this->SubTreeGetData()->Dump(fOut);

  }while (this->SubTreeWalkDown());

  
  fOut << "==> END Dump a CObjectTree Object at "<< this << std::endl;

  fOut << std::endl;

}



} // end namespace

