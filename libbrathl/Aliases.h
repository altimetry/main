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

  void SetName(const string& value) {m_name = value;};
  string GetName() {return m_name;};

  void AddProductType(const string& value) {m_productType.Insert(value);};
  const CStringArray* GetProductType() {return &m_productType;};

  bool IsValidForAllProductType() {return (m_productType.size() <= 0);};
  
  void GetRecordByProductType(CObMap& recordByProductType);

  virtual void Dump(ostream& fOut = cerr);


protected:
  string m_name;
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

  void SetName(const string& value) {m_name = value;};
  string GetName() const {return m_name;};

  void SetRef(const string& value) {m_ref = value;};
  string GetRef() const {return m_ref;};

  void SetValue(const string& value) {m_value = value;};
  string GetValue() const {return m_value;};

  void SetDescription(const string& value) {m_description = value;};
  string GetDescription() const {return m_description;};

  bool IsSynonym() {return !(m_ref.empty());};
  
  virtual void Dump(ostream& fOut = cerr);

protected:
  string m_name;
  string m_description;
  string m_ref;
  string m_value;

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

  void SetDescription(const string& value) {m_description = value;};
  string GetDescription() const {return m_description;};

  void SetProductType(const string& value) {m_productType = value;};
  string GetProductType() const {return m_productType;};
  
  void SetRecord(const string& value) {m_record = value;};
  string GetRecord() const {return m_record;};
  
  void SetRef(const string& value) {m_ref = value;};
  string GetRef() const {return m_ref;};
  bool HasRef() const {return !(m_ref.empty());};

  void AddAlias(CAlias* value);
  CAlias* GetAlias(const string& key);
  string GetAliasValue(const string& key);

  const CObMap* GetAliases() {return &m_aliases;};

  bool IsValidForAllProductType() {return m_productType.empty();};
  bool IsValidForAllRecord() {return m_record.empty();};

  void GetAliasesByProductType(CObMap& aliasesByProductType);

  void ResolveSynonyms();

  virtual void Dump(ostream& fOut = cerr);

public:
  static const string m_ALL;


protected:
  string m_description;
  string m_productType;
  string m_record;
  string m_ref;

  CObMap m_aliases;

protected:
    void Init();

};

//-------------------------------------------------------------
//------------------- CProductAlias class --------------------
//-------------------------------------------------------------

class CProductAlias : public CBratObject
{

public:
   // constructors and destructors
  CProductAlias();

  CProductAlias(const CAlias& alias);

  virtual ~CProductAlias ();
 
  void SetRecord(const string& value) {m_record = value;};
  string GetRecord() {return m_record;};
 
  void SetName(const string& value) {m_name = value;};
  string GetName() {return m_name;};

  void SetValue(const string& value) {m_value = value;};
  string GetValue() {return m_value;};

  void SetDescription(const string& value) {m_description = value;};
  string GetDescription() const {return m_description;};

  void Set(const CAlias& alias);

  virtual void Dump(ostream& fOut = cerr);


protected:
  string m_name;
  string m_description;
  string m_record;
  string m_value;

protected:
    void Init();

};

//-------------------------------------------------------------
//------------------- CProductAliases class --------------------
//-------------------------------------------------------------

class CProductAliases : public CObMap
{

public:
   // constructors and destructors
  CProductAliases(const string& defaultRecord);

  virtual ~CProductAliases ();
 
  void SetRecord(const string& value) {m_record = value;};
  string GetRecord() {return m_record;};

  void AddAlias(CAliases* value);
  void AddAlias(CObMap* value, const string& record);
  void AddAlias(CAlias* value, const string& record);
  CProductAlias* GetAlias(const string& key);
  string GetAliasValue(const string& key);

  //const CObMap* GetAliases() {return &m_aliases;};

  bool AddRecordNameToField(CStringMap& mapAliasesString, CProduct* product, string& errorMsg);
  bool AddRecordNameToField(const string& in, const string& dataSetName, string& out, CProduct* product, string& errorMsg);
  bool AddRecordNameToField(const string& in, const string& dataSetName, const CStringArray& fieldsIn, bool complexExpression, string& out, CProduct* product, string& errorMsg);

  void ToStringMap(CStringMap& map);

  virtual void Dump(ostream& fOut = cerr);

public:
  static const string m_ALL;


protected:

  string m_record;
  //CObMap m_aliases;

protected:
    void Init();

};

}
#endif
