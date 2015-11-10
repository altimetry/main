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


#include "Aliases.h"
#include "AliasesDictionary.h"
using namespace brathl;

// When debugging changes all calls to “new” to be calls to “DEBUG_NEW” allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

namespace brathl
{

const std::string CAliasesDictionary::m_ALIASES_DICT_FILE_NAME = "aliases.xml";

CAliasesDictionary* CAliasesDictionary::m_instance = NULL;

const std::string CAliasesDictionary::m_ROOT_ELT = "aliasesDefinition";

const std::string CAliasesDictionary::m_ALIAS_ELT = "alias";
const std::string CAliasesDictionary::m_ALIASES_ELT = "aliases";
const std::string CAliasesDictionary::m_DEFAULT_RECORD_ELT = "defaultRecord";
const std::string CAliasesDictionary::m_PRODUCT_ELT = "product";
const std::string CAliasesDictionary::m_PRODUCT_TYPE_ELT = "productType";

const std::string CAliasesDictionary::m_DESCR_ATTR = "description";
const std::string CAliasesDictionary::m_CLASS_ATTR = "class";
const std::string CAliasesDictionary::m_NAME_ATTR = "name";
const std::string CAliasesDictionary::m_PRODUCT_TYPE_ATTR = "productType";
const std::string CAliasesDictionary::m_RECORD_ATTR = "record";
const std::string CAliasesDictionary::m_REF_ATTR = "ref";


//-------------------------------------------------------------
//------------------- CAliasesDictionary class --------------------
//-------------------------------------------------------------

CAliasesDictionary::CAliasesDictionary()
{
  Init();
}

//----------------------------------------
CAliasesDictionary::CAliasesDictionary(const CXmlDocument& doc)
                      : CXmlDocument(doc)
{
  Init();
}

//----------------------------------------
CAliasesDictionary::CAliasesDictionary(const CAliasesDictionary& o)                     
{
  Init();
  Set(o);
}
//----------------------------------------
CAliasesDictionary::CAliasesDictionary(const std::string& filename, const std::string &encoding)
{

  // Don't call CXmlDocument(filename, encoding) because overriden 'Load' will not be called

  Init();
  m_fullFileName = filename;

  try
  {
    Load(filename, encoding);
  }
  catch(CException& e)
  {
    DeleteRoot();
    throw e;
  }
  catch(std::exception& e)
  {
    DeleteRoot();
    throw e;
  }
  catch(...)
  {
    DeleteRoot();
    throw CException("Error in CAliasesDictionary ctor - Unexpected error while loading  the aliases dictionary - No Context and no message have been set for this error", BRATHL_ERROR);
  }


}
//----------------------------------------
CAliasesDictionary::CAliasesDictionary(std::istream& stream, const std::string &encoding)
                      : CXmlDocument(stream, encoding)
{
  Init();
}

//----------------------------------------
CAliasesDictionary::~CAliasesDictionary()
{
}

//----------------------------------------
void CAliasesDictionary::Init()
{
}
//----------------------------------------
void CAliasesDictionary::Set(const CAliasesDictionary& o)                     
{
  CXmlDocument::operator=(o);

}
//----------------------------------------
CAliasesDictionary& CAliasesDictionary::operator=(const CAliasesDictionary& o)                     
{
  if (this == &o)
  {
    return *this;
  }

  Set(o);
  return *this;

}

//----------------------------------------
std::string CAliasesDictionary::GetAliasesDictFilePath()
{
  return CTools::Format("%s/%s", CTools::GetDataDir().c_str(), CAliasesDictionary::m_ALIASES_DICT_FILE_NAME.c_str());
}

//----------------------------------------------------
void CAliasesDictionary::PrepareSmartCleaner()
{
  static CSmartCleaner object(&CAliasesDictionary::m_instance);
}
//----------------------------------------
CAliasesDictionary* CAliasesDictionary::GetInstance(const std::string& encoding)
{
  std::string str = GetAliasesDictFilePath();
  return CAliasesDictionary::GetInstance(&str, encoding);
}
//----------------------------------------------------
CAliasesDictionary* CAliasesDictionary::GetInstance(const std::string* fileName, const std::string& encoding)
{

   //std::cout << "Enter GetInstance" << std::endl;
  if (CAliasesDictionary::m_instance != NULL)
  {
    return CAliasesDictionary::m_instance;
  }
    
  //std::cout << "GetInstance" << std::endl;
  if (fileName == NULL)
  {     
    throw CException("Error in CAliasesDictionary::GetInstance: Unable to load the aliases dictionary because fileName argument is NULL", BRATHL_ERROR);
  }


  bool bOk = true;

  std::string errorMsg;
  try
  {
    CAliasesDictionary::m_instance = new CAliasesDictionary(*fileName, encoding);
    bOk = CAliasesDictionary::m_instance->IsOk();
  }
  catch(CException& e)
  {
    bOk = false;
    errorMsg = e.GetMessage().c_str();
  }

  PrepareSmartCleaner();
      
  if (!bOk)
  {
    std::string msg;
    if (errorMsg.empty())
    {
      msg = CTools::Format("Unable to load the aliases dictionary file '%s' for unknown reason - Please, check XML syntax.", fileName->c_str());
    }
    else
    {
      msg = errorMsg;
    }

        
    throw CException(msg.c_str(), BRATHL_ERROR);
  }
      
   //std::cout << "Exit GetInstance" << std::endl;

  return CAliasesDictionary::m_instance;
}
//----------------------------------------------------
void CAliasesDictionary::DeleteRootNode()
{
  CXmlNode* root = DetachRoot();
  if (root != NULL) 
  {
    delete root;
    root = NULL;
  }
}

//----------------------------------------------------
bool CAliasesDictionary::GetAliases(CProduct* product)
{
  if (product == NULL)
  {
    return false;
  }
  CXmlNode* productNode = this->FindProductNode(product->GetProductClass());

  if (productNode == NULL)
  {
    return false;
  }

  CXmlNode* child = productNode->GetChildren();
  
  while (child != NULL) 
  {
    if (!CTools::CompareNoCase(child->GetName(), CAliasesDictionary::m_PRODUCT_ELT))
    {
      child = child->GetNext();
      continue;
    }

    child = child->GetNext();
  }

  return true;

}

//----------------------------------------------------
void CAliasesDictionary::Load(const std::string& fileName, const std::string& encoding, int flags)
{
  try
  {
    DeleteRootNode();    
    CXmlDocument::Load(fileName, encoding, flags);
  }
  catch(CXMLParseException& e)
  {
    std::string msg = CTools::Format("Unable to load the aliases dictionary file '%s' - Parsing errors found -  Native error: '%s'",
                                fileName.c_str(), e.GetMessage().c_str());
      
    DeleteRootNode();

    throw CXMLParseException(msg, e.error());
  }
  catch(CXMLException& e)
  {
    std::string msg = CTools::Format("Unable to load the aliases dictionary file '%s' - Native error: '%s'",
                                fileName.c_str(), e.GetMessage().c_str());
      
    DeleteRootNode();

    throw CXMLException(msg, e.error());
  }
  catch(CException& e)
  {
    DeleteRootNode();
    throw e;
  }
  catch(std::exception& e)
  {
    DeleteRootNode();
    throw e;
  }
  catch(...)
  {
    throw CException("Unexpected error while loading  the aliases dictionary - No Context and no message have been set for this error", BRATHL_ERROR);
  }

}
//----------------------------------------------------
void CAliasesDictionary::Load(const std::string& encoding, int flags)
{
  Load(m_fullFileName, encoding, flags);
}

//----------------------------------------------------
void CAliasesDictionary::Delete(CAliasesDictionary*& AliasesDictionary)
{
  if (AliasesDictionary != NULL)
  {
    delete AliasesDictionary;
    AliasesDictionary = NULL;
  }
}
//----------------------------------------------------
void CAliasesDictionary::GetDefaultRecords(CXmlNode* parent, CObArray& arrayDefaultRecord)
{
  if (parent == NULL)
  {
    throw CXMLException("Error in CAliasesDictionary::GetDefaultRecordNodes - Unable to get node because parent argument is NULL", BRATHL_LOGIC_ERROR);
  }
  
  CObArray arrayNodes(false);
  
  FindAllNodesByName(CAliasesDictionary::m_DEFAULT_RECORD_ELT, parent, arrayNodes, true);

  CObArray::const_iterator it;

  for(it = arrayNodes.begin() ; it != arrayNodes.end() ; it++)
  {
    CXmlNode* node = dynamic_cast<CXmlNode*>(*it);
    CDefaultRecord* defaultRecord = CreateDefaultRecord(node);
    if (defaultRecord != NULL)
    {
      arrayDefaultRecord.Insert(defaultRecord);
    }
  }

}


//----------------------------------------
CDefaultRecord* CAliasesDictionary::CreateDefaultRecord(CXmlNode* defaultRecordNode)
{
  if (defaultRecordNode == NULL)
  {
    return NULL;
  }

  CDefaultRecord* defaultRecord = new CDefaultRecord();

  std::string value;

  bool bOk = defaultRecordNode->GetPropVal(CAliasesDictionary::m_NAME_ATTR, &value);
  if (!bOk)
  {
    std::string msg = CTools::Format("Unable to create default record - Default record name is empty - "
      "Please check all '%s' attributes of the '%s' elements in the aliases dictionary '%s'.",
      CAliasesDictionary::m_NAME_ATTR.c_str(),
      CAliasesDictionary::m_DEFAULT_RECORD_ELT.c_str(),
      m_fullFileName.c_str());

    delete defaultRecord;
    defaultRecord = NULL;

    throw CXMLException(msg, BRATHL_ERROR);
  }

  defaultRecord->SetName(value);

  CObArray arrayNodes(false);
  
  FindAllNodesByName(CAliasesDictionary::m_PRODUCT_TYPE_ELT, defaultRecordNode, arrayNodes, false);

  CObArray::const_iterator it;

  for(it = arrayNodes.begin() ; it != arrayNodes.end() ; it++)
  {
    CXmlNode* node = dynamic_cast<CXmlNode*>(*it);

    if (node == NULL)
    {
      continue;
    }

    CXmlNode* textNode = node->GetChildren();

    if (textNode == NULL)
    {
      continue;
    }

    std::string productType = textNode->GetContent().c_str();
    if (productType.empty())
    {
      continue;
    }
    defaultRecord->AddProductType(productType);
  }

  return defaultRecord;
}

//----------------------------------------------------
void CAliasesDictionary::GetAliases(CXmlNode* parent, CObArray& arrayAliases)
{
  if (parent == NULL)
  {
    throw CXMLException("Error in CAliasesDictionary::arrayAliases - Unable to get node because parent argument is NULL", BRATHL_LOGIC_ERROR);
  }
  
  CObArray arrayNodes(false);
  
  FindAllNodesByName(CAliasesDictionary::m_ALIASES_ELT, parent, arrayNodes, true);

  CObArray::const_iterator it;

  for(it = arrayNodes.begin() ; it != arrayNodes.end() ; it++)
  {
    CXmlNode* node = dynamic_cast<CXmlNode*>(*it);
    CAliases* aliases = CreateAliases(node);
    if (aliases != NULL)
    {
      arrayAliases.Insert(aliases);
    }
  }

}


//----------------------------------------
CAliases* CAliasesDictionary::CreateAliases(CXmlNode* aliasesNode)
{
  if (aliasesNode == NULL)
  {
    return NULL;
  }

  CAliases* aliases = new CAliases();

  std::string value;

  bool bOk = aliasesNode->GetPropVal(CAliasesDictionary::m_DESCR_ATTR, &value);
  aliases->SetDescription(value);
  
  value.clear();
  bOk = aliasesNode->GetPropVal(CAliasesDictionary::m_PRODUCT_TYPE_ATTR, &value);
  aliases->SetProductType(value);

  value.clear();
  bOk = aliasesNode->GetPropVal(CAliasesDictionary::m_REF_ATTR, &value);
  aliases->SetRef(value);

  value.clear();
  bOk = aliasesNode->GetPropVal(CAliasesDictionary::m_RECORD_ATTR, &value);
  aliases->SetRecord(value);

  CObArray arrayNodes(false);
  
  FindAllNodesByName(CAliasesDictionary::m_ALIAS_ELT, aliasesNode, arrayNodes, false);

  CObArray::const_iterator it;

  for(it = arrayNodes.begin() ; it != arrayNodes.end() ; it++)
  {
    CXmlNode* node = dynamic_cast<CXmlNode*>(*it);

    if (node == NULL)
    {
      continue;
    }

    CAlias* alias = CreateAlias(node);

    if (alias == NULL)
    {
      continue;
    }

    aliases->AddAlias(alias);
  }

  return aliases;
}

//----------------------------------------
CAlias* CAliasesDictionary::CreateAlias(CXmlNode* aliasNode)
{
  if (aliasNode == NULL)
  {
    return NULL;
  }

  CAlias* alias = new CAlias();

  std::string value;

  bool bOk = aliasNode->GetPropVal(CAliasesDictionary::m_NAME_ATTR, &value);
  if (!bOk)
  {
    std::string msg = CTools::Format("Unable to create alias - Alias name is empty - "
      "Please check all '%s' attributes of the '%s' elements in the aliases dictionary '%s'.",
      CAliasesDictionary::m_NAME_ATTR.c_str(),
      CAliasesDictionary::m_ALIAS_ELT.c_str(),
      m_fullFileName.c_str());

    delete alias;
    alias = NULL;

    throw CXMLException(msg, BRATHL_ERROR);
  }

  alias->SetName(value);
  
  value.clear();
  bOk = aliasNode->GetPropVal(CAliasesDictionary::m_REF_ATTR, &value);
  alias->SetRef(value);
  bOk = aliasNode->GetPropVal(CAliasesDictionary::m_DESCR_ATTR, &value);
  alias->SetDescription(value);

  CXmlNode* textNode = aliasNode->GetChildren();

  std::string aliasValue;

  if (textNode != NULL) 
  {
    aliasValue = textNode->GetContent().c_str();
  }

  if ((aliasValue.empty()) && (!alias->IsSynonym()))
  {
    std::string msg = CTools::Format("Unable to create alias '%s' - Alias value is empty and alias ref. attribute is empty. "
      "One of two values has to be filled (but not both of them)."
      "Please check all '%s' attributes of the '%s' elements in the aliases dictionary '%s'.",
      alias->GetName().c_str(),
      CAliasesDictionary::m_NAME_ATTR.c_str(),
      CAliasesDictionary::m_ALIAS_ELT.c_str(),
      m_fullFileName.c_str());

    delete alias;
    alias = NULL;

    throw CXMLException(msg, BRATHL_ERROR);

  }

  if ((!aliasValue.empty()) && (alias->IsSynonym()))
  {
    std::string msg = CTools::Format("Unable to create alias '%s' - Alias value is not empty ('%s') and alias ref. attribute is not empty ('%s'). "
      "Only one of two values has to be filled, not both of them."
      "Please check all '%s' attributes of the '%s' elements in the aliases dictionary '%s'.",
      alias->GetName().c_str(),
      aliasValue.c_str(),
      alias->GetRef().c_str(),
      CAliasesDictionary::m_NAME_ATTR.c_str(),
      CAliasesDictionary::m_ALIAS_ELT.c_str(),
      m_fullFileName.c_str());

    delete alias;
    alias = NULL;

    throw CXMLException(msg, BRATHL_ERROR);
  }

  if (alias->IsSynonym())
  {
    CXmlNode* aliasRefNode = FindAliasNode(alias->GetRef(), aliasNode->GetParent());
    if (aliasRefNode == NULL) 
    {
      std::string msg = CTools::Format("Unable to create alias '%s' - Alias is as synonym, but the referenced alias ('%s') is not found. "
        "Please check all '%s' attributes of the '%s' elements in the aliases dictionary '%s'.",
        alias->GetName().c_str(),
        alias->GetRef().c_str(),
        CAliasesDictionary::m_NAME_ATTR.c_str(),
        CAliasesDictionary::m_ALIAS_ELT.c_str(),
        m_fullFileName.c_str());

      delete alias;
      alias = NULL;

      throw CXMLException(msg, BRATHL_ERROR);
    }

    alias->SetDescription(CTools::Format("%s (Synonym of %s).", alias->GetDescription().c_str(), alias->GetRef().c_str()));

    textNode = aliasRefNode->GetChildren();

    if (textNode == NULL) 
    {
      std::string msg = CTools::Format("Unable to create alias '%s' - Alias is as synonym, but the referenced alias ('%s') value is empty. "
        "Please check all '%s' attributes of the '%s' elements in the aliases dictionary '%s'.",
        alias->GetName().c_str(),
        alias->GetRef().c_str(),
        CAliasesDictionary::m_NAME_ATTR.c_str(),
        CAliasesDictionary::m_ALIAS_ELT.c_str(),
        m_fullFileName.c_str());

      delete alias;
      alias = NULL;

      throw CXMLException(msg, BRATHL_ERROR);
    }

    aliasValue = textNode->GetContent().c_str();

  }

  alias->SetValue(aliasValue);

  return alias;
}

//----------------------------------------------------
CXmlNode* CAliasesDictionary::FindProductNode(const std::string& clazz, bool allDepths /* = false */)
{
  return FindProductNode(clazz, this->GetRoot(), allDepths);
}
//----------------------------------------------------
CXmlNode* CAliasesDictionary::FindProductNode(const std::string& clazz, CXmlNode* parent, bool allDepths /* = false */)
{
  if (parent == NULL)
  {
    return NULL;
  }

  CXmlNode* child =  parent->GetChildren();
  std::string value;

  while (child != NULL)
  {
    if (CTools::CompareNoCase(child->GetName(), CAliasesDictionary::m_PRODUCT_ELT))
    {
      bool bOk = child->GetPropVal(CAliasesDictionary::m_CLASS_ATTR, &value);
      if (bOk)
      {
        if (CTools::CompareNoCase(clazz, value))
        {
          break;
        }
      }
    }

    if ((allDepths) && (child->GetChildren() != NULL))
    {
      CXmlNode* newChild = FindProductNode(clazz, child, allDepths);
      
      if (newChild != NULL)
      {
        child = newChild;
        break;
      }
    }

    child = child->GetNext();
  }

  return child;

}
//----------------------------------------------------
CXmlNode* CAliasesDictionary::FindAliasNode(const std::string& name, CXmlNode* parent, bool allDepths /* = false */)
{
  if (parent == NULL)
  {
    return NULL;
  }

  CXmlNode* child =  parent->GetChildren();
  std::string value;

  while (child != NULL)
  {
    if (CTools::CompareNoCase(child->GetName(), CAliasesDictionary::m_ALIAS_ELT))
    {
      bool bOk = child->GetPropVal(CAliasesDictionary::m_NAME_ATTR, &value);
      if (bOk)
      {
        if (CTools::CompareNoCase(name, value))
        {
          break;
        }
      }
    }

    if ((allDepths) && (child->GetChildren() != NULL))
    {
      CXmlNode* newChild = FindAliasNode(name, child, allDepths);
      
      if (newChild != NULL)
      {
        child = newChild;
        break;
      }
    }

    child = child->GetNext();
  }

  return child;

}

//----------------------------------------
CAliases* CAliasesDictionary::ResolveRef(const CAliases* aliases, const CObArray* aliasesArray)
{
  if (aliases == NULL)
  {
    return NULL;
  }
  if (!aliases->HasRef())
  {
    return NULL;
  }
  if (aliasesArray == NULL)
  {
    return NULL;
  }

  std::string productType = aliases->GetProductType();
  std::string ref = aliases->GetRef();
  
  if ((!productType.empty()) && (!ref.empty()) && (productType.compare(ref) == 0))
  {
    std::string msg = CTools::Format("Unable to load the aliases dictionary because the '%s' and '%s' attributes of the '%s' element have the same value (reference to itself). "
      "Please check the XML aliases dictionary and fix the error.",
      CAliasesDictionary::m_PRODUCT_TYPE_ATTR.c_str(),
      CAliasesDictionary::m_REF_ATTR.c_str(),
      CAliasesDictionary::m_ALIASES_ELT.c_str());

    throw CXMLException(msg, BRATHL_ERROR);
  }

  CAliases* refAliases = NULL;
  bool found = false;

  CObArray::const_iterator it;
  for (it = aliasesArray->begin() ; it != aliasesArray->end() ; it++) 
  {
    refAliases = dynamic_cast<CAliases*>(*it);
    if (refAliases == NULL)
    {
      continue;
    }

    std::string productType = refAliases->GetProductType();

    if (productType.compare(ref) == 0)
    {
      found = true;
      break;
    }
  }

  if (!found)
  {
    refAliases = NULL;
    return refAliases;
  }

  return refAliases;
}

//----------------------------------------
CAliasesDictionary* CAliasesDictionary::LoadAliasesDictionary(std::string* errorMsg, bool withExcept)
{
  CAliasesDictionary* aliasesDictionary = NULL;
  
  try
  {
    aliasesDictionary = CAliasesDictionary::GetInstance();
  }
  catch (CException& e)
  {
    // Create an 'empty' instance
    CAliasesDictionary::m_instance = new CAliasesDictionary();
    PrepareSmartCleaner();
    
    std::string msg = CTools::Format("The aliases dictionary can't be loaded properly - This does not prevent Brat to continue to work properly but without the use of aliases - Native Error is '%s'", e.what());

    if (errorMsg != NULL)
    {
      *errorMsg = msg;
    }

    if (withExcept)
    {
      throw CLoadAliasesException(CTools::Format("The aliases dictionary can't be loaded properly - This does not prevent Brat to continue to work properly but without the use of aliases - Native Error is '%s'", e.what()), BRATHL_LOGIC_ERROR);
    }
  }

  return aliasesDictionary;
}

//----------------------------------------
bool CAliasesDictionary::SetPropVal(CXmlNode* node, const std::string& propName, const std::string& value, bool allDepths /* = true */)
{
  bool bOk = false;

  if (node == NULL)
  {
    return bOk;
  }

  CXmlProperty *prop = node->GetProperties();

  while (prop)
  {
      if (prop->GetName() == propName)
      {
          prop->SetValue(value);
          bOk = true;
          break;
      }

      prop = prop->GetNext();
  }

  if (allDepths)
  {
    this->SetPropVal(node->GetChildren(), propName, value, allDepths);
  }

  return bOk;

}

//----------------------------------------

} // end namespace