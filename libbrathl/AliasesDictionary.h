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

#ifndef __AliasesDictionary_H__
#define __AliasesDictionary_H__


#include "brathl.h"

#include "Xml.h" // (wxXmlDocument, wxXmlNode, wxXmlProperty classes)
#include "Tools.h"
#include "Product.h"
#include "Aliases.h"

using namespace brathl;

namespace brathl
{

//-------------------------------------------------------------
//------------------- CAliasesDictionary class --------------------
//-------------------------------------------------------------

class CAliasesDictionary: public CXmlDocument
{

protected:
   // constructors and destructors
  CAliasesDictionary();

  CAliasesDictionary(const CXmlDocument& doc);


  CAliasesDictionary(const std::string& filename, const std::string& encoding = CXmlDocument::m_DEFAULT_ENCODING);

 	CAliasesDictionary(std::istream& stream, const std::string& encoding = CXmlDocument::m_DEFAULT_ENCODING);

public:
  
  CAliasesDictionary(const CAliasesDictionary& doc);

  virtual ~CAliasesDictionary ();

  CAliasesDictionary& operator=(const CAliasesDictionary& doc);

  const std::string& GetFullFileName() { return m_fullFileName; };

  void Load(const std::string& fileName, const std::string& encoding = CXmlDocument::m_DEFAULT_ENCODING, int flags = CXmlDocument::XMLDOC_NONE);
  void Load(const std::string& encoding = CXmlDocument::m_DEFAULT_ENCODING, int flags = CXmlDocument::XMLDOC_NONE);

  static bool IsLoaded() { return (CAliasesDictionary::m_instance != NULL); };
  
  CXmlNode* FindProductNode(const std::string& clazz, bool allDepths = false);
  CXmlNode* FindProductNode(const std::string& clazz, CXmlNode* parent, bool allDepths = false);
  
  CXmlNode* FindAliasNode(const std::string& name, CXmlNode* parent, bool allDepths = false);

 
  
  bool GetAliases(CProduct* product);

  void GetDefaultRecords(CXmlNode* parentNode, CObArray& mapDefaultRecord);
  void GetAliases(CXmlNode* parent, CObArray& arrayAliases);

  CDefaultRecord* CreateDefaultRecord(CXmlNode* defaultRecordNode);
  CAliases* CreateAliases(CXmlNode* aliasesNode);
  CAlias* CreateAlias(CXmlNode* aliasNode);
  
  bool SetPropVal(CXmlNode* node, const std::string& propName, const std::string& value, bool allDepths = true);

  static CAliasesDictionary* LoadAliasesDictionary(std::string* errorMsg, bool withExcept);
  
  static std::string GetAliasesDictFilePath();

  static CAliasesDictionary* GetInstance(const std::string& encoding = CXmlDocument::m_DEFAULT_ENCODING);
  static CAliasesDictionary* GetInstance(const std::string* fileName, const std::string& encoding = CXmlDocument::m_DEFAULT_ENCODING);

  static CAliases* ResolveRef(const CAliases* aliases, const CObArray* aliasesArray);


public:

  static const std::string m_ROOT_ELT;

  static const std::string m_ALIAS_ELT;
  static const std::string m_ALIASES_ELT;
  static const std::string m_DEFAULT_RECORD_ELT;
  static const std::string m_PRODUCT_ELT;
  static const std::string m_PRODUCT_TYPE_ELT;

  static const std::string m_DESCR_ATTR;
  static const std::string m_CLASS_ATTR;
  static const std::string m_NAME_ATTR;
  static const std::string m_PRODUCT_TYPE_ATTR;
  static const std::string m_RECORD_ATTR;
  static const std::string m_REF_ATTR;

protected:
  //----------------------------------------------------------------------------
  // CSmartCleaner
  //----------------------------------------------------------------------------
  
  class CSmartCleaner
  {
    // the cleaner stores the address of the instance pointer
    // for a singleton object. The CSmartCleaner MUST be
    // instanciated as a static object in a function, which
    // ensures it is destroyed after exit, but is safe if
    // the singleton object IS explicitly deleted.

    protected :
      CAliasesDictionary **m_pAddressOfAliasesDictionaryPtr;


    public :
      CSmartCleaner( CAliasesDictionary **addressOfAliasesDictionaryPtr) 
        : m_pAddressOfAliasesDictionaryPtr( addressOfAliasesDictionaryPtr )
      {
      }
      
      virtual ~CSmartCleaner()
      {
        //cout << "Enter ~CSmartCleaner" << endl;

        if( *m_pAddressOfAliasesDictionaryPtr != NULL )
        {
          delete *m_pAddressOfAliasesDictionaryPtr;
          *m_pAddressOfAliasesDictionaryPtr = NULL;

        }
      }

      CAliasesDictionary * GetObject() {return *m_pAddressOfAliasesDictionaryPtr;}
        
  };

protected:

  std::string m_fullFileName;

  static const std::string m_ALIASES_DICT_FILE_NAME;


  static CAliasesDictionary* m_instance;
    
protected:
    void Init();
    void Set(const CAliasesDictionary& o);
    
    void DeleteRootNode();

    void Delete(CAliasesDictionary*& AliasesDictionary);

    static void PrepareSmartCleaner();

};


}

#endif
