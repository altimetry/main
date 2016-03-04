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

#ifndef __Aliases_H__
#define __Aliases_H__


#include "brathl.h"

#include "List.h"

using namespace brathl;

namespace brathl
{

  class CProduct;

//-------------------------------------------------------------
//------------------- CDefaultRecord class --------------------
//-------------------------------------------------------------

class CDefaultRecord : public CBratObject
{

public:
   // constructors and destructors
  CDefaultRecord();

  virtual ~CDefaultRecord ();

  void SetName(const std::string& value) {m_name = value; }
  std::string GetName() {return m_name; }

  void AddProductType(const std::string& value) {m_productType.Insert(value); }
  const CStringArray* GetProductType() {return &m_productType; }

  bool IsValidForAllProductType() {return (m_productType.size() <= 0); }
  
  void GetRecordByProductType(CObMap& recordByProductType);

  virtual void Dump(std::ostream& fOut = std::cerr);


protected:
  std::string m_name;
  CStringArray m_productType;


protected:
    void Init();

};

//-------------------------------------------------------------
//------------------- CAlias class --------------------
//-------------------------------------------------------------

class CAlias : public CBratObject
{

public:
   // constructors and destructors
  CAlias();

  virtual ~CAlias ();

  void SetName(const std::string& value) {m_name = value; }
  std::string GetName() const {return m_name; }

  void SetRef(const std::string& value) {m_ref = value; }
  std::string GetRef() const {return m_ref; }

  void SetValue(const std::string& value) {m_value = value; }
  std::string GetValue() const {return m_value; }

  void SetDescription(const std::string& value) {m_description = value; }
  std::string GetDescription() const {return m_description; }

  bool IsSynonym() {return !(m_ref.empty()); }
  
  virtual void Dump(std::ostream& fOut = std::cerr);

protected:
  std::string m_name;
  std::string m_description;
  std::string m_ref;
  std::string m_value;

protected:
    void Init();

};

//-------------------------------------------------------------
//------------------- CAliases class --------------------
//-------------------------------------------------------------

class CAliases : public CBratObject
{

public:
   // constructors and destructors
  CAliases();

  virtual ~CAliases ();

  void SetDescription(const std::string& value) {m_description = value; }
  std::string GetDescription() const {return m_description; }

  void SetProductType(const std::string& value) {m_productType = value; }
  std::string GetProductType() const {return m_productType; }
  
  void SetRecord(const std::string& value) {m_record = value; }
  std::string GetRecord() const {return m_record; }
  
  void SetRef(const std::string& value) {m_ref = value; }
  std::string GetRef() const {return m_ref; }
  bool HasRef() const {return !(m_ref.empty()); }

  void AddAlias(CAlias* value);
  CAlias* GetAlias(const std::string& key);
  std::string GetAliasValue(const std::string& key);

  const CObMap* GetAliases() {return &m_aliases; }

  bool IsValidForAllProductType() {return m_productType.empty(); }
  bool IsValidForAllRecord() {return m_record.empty(); }

  void GetAliasesByProductType(CObMap& aliasesByProductType);

  void ResolveSynonyms();

  virtual void Dump(std::ostream& fOut = std::cerr);

public:
  static const std::string m_ALL;


protected:
  std::string m_description;
  std::string m_productType;
  std::string m_record;
  std::string m_ref;

  CObMap m_aliases;

protected:
    void Init();

};

//-------------------------------------------------------------
//------------------- CProductAlias class --------------------
//-------------------------------------------------------------

class CProductAlias : public CBratObject
{

  std::string m_name;
  std::string m_description;
  std::string m_record;
  std::string m_value;

public:
   // constructors and destructors
  CProductAlias()
  {}

  CProductAlias(const CAlias& alias)
  {
    Set(alias);
  }

  virtual ~CProductAlias ()
  {}
 
  void SetRecord(const std::string& value) { m_record = value; }
  const std::string& GetRecord() const { return m_record; }
 
  void SetName(const std::string& value) { m_name = value; }
  const std::string& GetName() const { return m_name; }

  void SetValue(const std::string& value) { m_value = value; }
  const std::string& GetValue() const { return m_value; }

  void SetDescription(const std::string& value) { m_description = value; }
  const std::string& GetDescription() const { return m_description; }

  void Set(const CAlias& alias)
  {
    m_name = alias.GetName();
    m_value = alias.GetValue();
    m_description = alias.GetDescription();
  }


  virtual void Dump(std::ostream& fOut = std::cerr);
};

//-------------------------------------------------------------
//------------------- CProductAliases class --------------------
//-------------------------------------------------------------

class CProductAliases : public CObMap
{

public:
   // constructors and destructors
  CProductAliases(const std::string& defaultRecord);

  virtual ~CProductAliases ();
 
  void SetRecord(const std::string& value) {m_record = value; }
  const std::string& GetRecord() const {return m_record; }

  void AddAlias(CAliases* value);
  void AddAlias(CObMap* value, const std::string& record);
  void AddAlias(CAlias* value, const std::string& record);
  CProductAlias* GetAlias(const std::string& key);
  std::string GetAliasValue(const std::string& key);

  //const CObMap* GetAliases() {return &m_aliases; }

  bool AddRecordNameToField(CStringMap& mapAliasesString, CProduct* product, std::string& errorMsg);
  bool AddRecordNameToField(const std::string& in, const std::string& dataSetName, std::string& out, CProduct* product, std::string& errorMsg);
  bool AddRecordNameToField(const std::string& in, const std::string& dataSetName, const CStringArray& fieldsIn, bool complexExpression, std::string& out, CProduct* product, std::string& errorMsg);

  void ToStringMap(CStringMap& map);

  virtual void Dump(std::ostream& fOut = std::cerr);

public:
  static const std::string m_ALL;


protected:

  std::string m_record;
  //CObMap m_aliases;

protected:
    void Init();

};

}
#endif
