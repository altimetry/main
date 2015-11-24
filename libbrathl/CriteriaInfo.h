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
#if !defined(_CriteriaInfo_h_)
#define _CriteriaInfo_h_

#include <string>

#include "new-gui/Common/tools/brathl_error.h" 
#include "brathl.h" 

#include "BratObject.h"
#include "List.h"

using namespace brathl;

namespace brathl
{

//-------------------------------------------------------------
//------------------- CFieldInfo class --------------------
//-------------------------------------------------------------

class CFieldInfo : public CBratObject
{
public:
  
  CFieldInfo();    
  
  CFieldInfo(const std::string& name);    
  
  CFieldInfo(CFieldInfo& f);    

  virtual ~CFieldInfo();

  void SetIsNetCdfVarAttribute(bool value) { m_isNetCdfVarAttribute = value; };
  bool IsNetCdfVarAttribute() { return m_isNetCdfVarAttribute; };

  void SetIsNetCdfGlobalAttribute(bool value) { m_isNetCdfGlobalAttribute = value; };
  bool IsNetCdfGlobalAttribute() { return m_isNetCdfGlobalAttribute; };

  void SetIsNetCdfVarValue(bool value) { m_isNetCdfVarValue = value; };
  bool IsNetCdfVarValue() { return m_isNetCdfVarValue; };

  void SetAttributeVarName(const std::string& value) { m_attributeVarName = value; };
  std::string GetAttributeVarName() { return m_attributeVarName; };

  void SetName(const std::string& value) {m_name = value; };
  const std::string& GetName() {return m_name;};

  //void SetDataType(int32_t value) {m_dataType = value; };
  //int32_t GetDataType() {return m_dataType;};

  
  void Set(CFieldInfo& f);

  const CFieldInfo& operator= (CFieldInfo& lst);

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);


protected:
  
  std::string m_name;
  
  bool m_isNetCdfVarAttribute;
  std::string m_attributeVarName;
  bool m_isNetCdfGlobalAttribute;
  bool m_isNetCdfVarValue;

  /*
   *  The netcdf external data types
   */
  //int32_t m_dataType;

private:

  void Init();


};


//-------------------------------------------------------------
//------------------- CRecordDataMap class --------------------
//-------------------------------------------------------------
class CRecordDataMap : public CObMap
{
public:
  /// Ctor
  CRecordDataMap(bool bDelete = true) : CObMap(bDelete) {} ;  

  /// Dtor
  virtual ~CRecordDataMap() {};

    //----------------------------------------
  CObMap* GetFields(const std::string& key, bool create = true);

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

};

//-------------------------------------------------------------
//------------------- CRecordData class --------------------
//-------------------------------------------------------------
class CRecordData : public CBratObject
{
public:
  CRecordData(const std::string& name) { SetName(name); };
  virtual ~CRecordData() {};

  CObMap* GetFields() { return &m_fields; };
  
  void SetName(const std::string& value) {m_name = value; };
  std::string GetName() {return m_name;};

  static CRecordData* GetRecordData(CBratObject* ob, bool withExcept = true);

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

protected:
 std::string m_name;
 CObMap m_fields;
};


//-------------------------------------------------------------
//------------------- CCriteriaInfo class --------------------
//-------------------------------------------------------------


/** \addtogroup product Criteria information
  @{ */

/** 
  Base class for criteria information.


 \version 1.0
*/



class CCriteriaInfo : public CBratObject
{

public:
    
  /// Empty CCriteriaInfo ctor
  CCriteriaInfo();
    
  /// Destructor
  virtual ~CCriteriaInfo();


  int32_t GetKey() { return m_key; };

  virtual void GetFieldsInfo(CObMap* fieldsInfo) = 0;
  
  virtual void GetFields(CRecordDataMap& listRecord);
  virtual void GetFieldNames(CStringList& fieldNames);
  virtual void GetFieldNames(CStringArray& fieldNames);

  void SetDataRecord(const std::string& value) { m_dataRecord = value; };
  std::string GetDataRecord() { return m_dataRecord; };

  /*
  void SetIsNetCdfVarAttribute(bool value) { m_fieldInfo.SetIsNetCdfVarAttribute(value); };
  bool IsNetCdfVarAttribute() { return m_fieldInfo.IsNetCdfVarAttribute(); };

  void SetIsNetCdfGlobalAttribute(bool value) { m_fieldInfo.SetIsNetCdfGlobalAttribute(value); };
  bool IsNetCdfGlobalAttribute() { return m_fieldInfo.IsNetCdfGlobalAttribute(); };

  void SetIsNetCdfVarValue(bool value) { m_fieldInfo.SetIsNetCdfVarValue(value); };
  bool IsNetCdfVarValue() { return m_fieldInfo.IsNetCdfVarValue; };
*/
  static CCriteriaInfo* GetCriteriaInfo(CBratObject* ob, bool withExcept = true);

  //static CStringList* GetRecordDataFields(const std::string& key, CObMap& listRecord, bool create = true);

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

public:


protected:
private:

  void Init();

public:
    

protected:
  int32_t m_key;
  std::string m_dataRecord;
  
private:
  

};

//-------------------------------------------------------------
//------------------- CCriteriaDatetimeInfo class --------------------
//-------------------------------------------------------------


/** 
  Date/Time criteria information management class.


 \version 1.0
*/





class CCriteriaDatetimeInfo : public CCriteriaInfo
{

public:
    
  /// Empty CCriteriaDatetimeInfo ctor
  CCriteriaDatetimeInfo();
    
  /// Destructor
  virtual ~CCriteriaDatetimeInfo();

  virtual void GetFieldsInfo(CObMap* fieldsInfo);

  void SetStartDateField(const std::string& value) { m_startDateField.SetName(value); };
  void SetStartDateField(CFieldInfo& value) { m_startDateField.Set(value); };
  
  const std::string& GetStartDateFieldName() { return m_startDateField.GetName(); };
  CFieldInfo* GetStartDateField() { return &m_startDateField; };
  
  void SetEndDateField(const std::string& value) { m_endDateField.SetName(value); };
  void SetEndDateField(CFieldInfo& value) { m_endDateField.Set(value); };
  
  const std::string& GetEndDateFieldName() { return  m_endDateField.GetName(); };
  CFieldInfo* GetEndDateField() { return &m_endDateField; };

  void SetRefDate(brathl_refDate value) { m_refDate = value; };
  brathl_refDate GetRefDate() { return m_refDate; };

  static CCriteriaDatetimeInfo* GetCriteriaInfo(CBratObject* ob, bool withExcept = true);

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

public:


protected:
private:

  void Init();

public:
    

protected:
  
  CFieldInfo m_startDateField;
  CFieldInfo m_endDateField;

  brathl_refDate m_refDate;

private:
  

};

//-------------------------------------------------------------
//------------------- CCriteriaLatLonInfo class --------------------
//-------------------------------------------------------------


/** 
  Lat/Lon criteria information management class.


 \version 1.0
*/





class CCriteriaLatLonInfo : public CCriteriaInfo
{

public:
    
  /// Empty CCriteriaLatLonInfo ctor
  CCriteriaLatLonInfo();
    
  /// Destructor
  virtual ~CCriteriaLatLonInfo();

  virtual void GetFieldsInfo(CObMap* fieldsInfo);

  
  void SetStartLatField(const std::string& value) { m_startLatField.SetName(value); };
  void SetStartLatField(CFieldInfo& value) { m_startLatField.Set(value); };
  
  const std::string& GetStartLatFieldName() { return m_startLatField.GetName(); };
  CFieldInfo* GetStartLatField() { return &m_startLatField; };
  
  void SetEndLatField(const std::string& value) { m_endLatField.SetName(value); };
  void SetEndLatField(CFieldInfo& value) { m_endLatField.Set(value); };
  
  const std::string& GetEndLatFieldName() { return m_endLatField.GetName(); };
  CFieldInfo* GetEndLatField() { return &m_endLatField; };

  void SetStartLonField(const std::string& value) { m_startLonField.SetName(value); };
  void SetStartLonField(CFieldInfo& value) { m_startLonField.Set(value); };
  const std::string& GetStartLonFieldName() { return m_startLonField.GetName(); };
  CFieldInfo* GetStartLonField() { return &m_startLonField; };

  void SetEndLonField(const std::string& value) { m_endLonField.SetName(value);};
  void SetEndLonField(CFieldInfo& value) { m_endLonField.Set(value);};
  const std::string& GetEndLonFieldName() { return m_endLonField.GetName(); };
  CFieldInfo* GetEndLonField() { return &m_endLonField; };

  static CCriteriaLatLonInfo* GetCriteriaInfo(CBratObject* ob, bool withExcept = true);

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

public:


protected:
private:

  void Init();

public:
    

protected:
  
  CFieldInfo m_startLatField;
  CFieldInfo m_endLatField;
  CFieldInfo m_startLonField;
  CFieldInfo m_endLonField;

private:
  

};


//-------------------------------------------------------------
//------------------- CCriteriaPassInfo class --------------------
//-------------------------------------------------------------


/** 
  Pass criteria information management class.


 \version 1.0
*/





class CCriteriaPassInfo : public CCriteriaInfo
{

public:
    
  /// Empty CCriteriaPassInfo ctor
  CCriteriaPassInfo();
    
  /// Destructor
  virtual ~CCriteriaPassInfo();

  virtual void GetFieldsInfo(CObMap* fieldsInfo);

  void SetStartPassField(const std::string& value) { m_startPassField.SetName(value); };
  void SetStartPassField(CFieldInfo& value) { m_startPassField.Set(value); };

  const std::string& GetStartPassFieldName() { return m_startPassField.GetName(); };
  CFieldInfo* GetStartPassField() { return &m_startPassField; };
  
  void SetEndPassField(const std::string& value) { m_endPassField.SetName(value); };
  void SetEndPassField(CFieldInfo& value) { m_endPassField.Set(value); };

  const std::string& GetEndPassFieldName() { return m_endPassField.GetName(); };
  CFieldInfo* GetEndPassField() { return &m_endPassField; };
  
  static CCriteriaPassInfo* GetCriteriaInfo(CBratObject* ob, bool withExcept = true);


  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

public:


protected:
private:

  void Init();

public:
    

protected:
  
  CFieldInfo m_startPassField;
  CFieldInfo m_endPassField;

private:
  

};
//-------------------------------------------------------------
//------------------- CCriteriaPassStringInfo class --------------------
//-------------------------------------------------------------


/** 
  String Pass criteria information management class.


 \version 1.0
*/


class CCriteriaPassStringInfo : public CCriteriaPassInfo
{

public:
    
  CCriteriaPassStringInfo() {};
    
  virtual ~CCriteriaPassStringInfo() {};

  static CCriteriaPassStringInfo* GetCriteriaInfo(CBratObject* ob, bool withExcept = true);
  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

public:


protected:
private:

  void Init();

public:
    

protected:
  
private:
  

};
//-------------------------------------------------------------
//------------------- CCriteriaPassIntInfo class --------------------
//-------------------------------------------------------------


/** 
  Integer Pass criteria information management class.


 \version 1.0
*/


class CCriteriaPassIntInfo : public CCriteriaPassInfo
{

public:
    
  CCriteriaPassIntInfo() {};
    
  virtual ~CCriteriaPassIntInfo() {};

  static CCriteriaPassIntInfo* GetCriteriaInfo(CBratObject* ob, bool withExcept = true);

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

public:


protected:
private:

  void Init();

public:
    

protected:
  
private:
  

};
//-------------------------------------------------------------
//------------------- CCriteriaCycleInfo class --------------------
//-------------------------------------------------------------


/** 
  Cycle criteria information management class.


 \version 1.0
*/





class CCriteriaCycleInfo : public CCriteriaInfo
{

public:
    
  /// Empty CCriteriaCycleInfo ctor
  CCriteriaCycleInfo();
    
  /// Destructor
  virtual ~CCriteriaCycleInfo();

  virtual void GetFieldsInfo(CObMap* fieldsInfo);

  void SetStartCycleField(const std::string& value) { m_startCycleField.SetName(value); };
  void SetStartCycleField(CFieldInfo& value) { m_startCycleField.Set(value); };
  
  const std::string GetStartCycleFieldName() { return m_startCycleField.GetName(); };
  CFieldInfo* GetStartCycleField() { return &m_startCycleField; };
  
  void SetEndCycleField(const std::string& value) { m_endCycleField.SetName(value); };
  void SetEndCycleField(CFieldInfo& value) { m_endCycleField.Set(value); };
  
  const std::string& GetEndCycleFieldName() { return m_endCycleField.GetName(); };
  CFieldInfo* GetEndCycleField() { return &m_endCycleField; };
  
  static CCriteriaCycleInfo* GetCriteriaInfo(CBratObject* ob, bool withExcept = true);

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

public:


protected:
private:

  void Init();

public:
    

protected:
  
  CFieldInfo m_startCycleField;
  CFieldInfo m_endCycleField;

private:
  

};


/** @} */

}

#endif // !defined(_CriteriaInfo_h_)
