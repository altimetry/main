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
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA	 02110-1301, USA.
*/
#ifndef _NetCDFFiles_h_
#define _NetCDFFiles_h_

#include "netcdf.h"
#include "BratObject.h"
#include <string>
#include "List.h"
#include "Expression.h"

namespace brathl
{
  class CFieldNetCdf;

  class CNetCDFAttrByte;
  class CNetCDFAttrDouble;
  class CNetCDFAttrFloat;
  class CNetCDFAttrInt;
  class CNetCDFAttrShort;
  class CNetCDFAttrString;

  class CNetCDFAttrUByte;
  class CNetCDFAttrUShort;
  class CNetCDFAttrUInt;
  class CNetCDFAttrInt64;
  class CNetCDFAttrUInt64;
  class CNetCDFAttrStringArray;


  enum NetCDFVarKind {
	  Unknown,
	  X,
	  Y,
	  Z,
	  T,
	  Latitude,
	  Longitude,
	  Data
  };

}
#include "Field.h"
using namespace brathl;

namespace brathl
{

  const std::string SSHA = "SSHA";
  const std::string GDR = "GDR";
  const std::string SGDR = "SGDR";
  
  const std::string SSHA_TITLE = "reduced dataset";
  const std::string GDR_TITLE = "standard dataset";
  const std::string SGDR_TITLE = "expertise dataset";


/** \addtogroup tools Tools
  @{ */

/**
  NetCDF files access.



 \version 1.0
*/


#define FILL_VALUE_ATTR		"_FillValue"
#define LONG_NAME_ATTR		"long_name"
#define UNITS_ATTR		"units"
#define SCALE_FACTOR_ATTR	"scale_factor"
#define ADD_OFFSET_ATTR		"add_offset"
#define STANDARD_NAME_ATTR	"standard_name"
#define AXIS_ATTR		"axis"
#define FILE_TYPE_ATTR		"FileType"
#define CONVENTIONS_ATTR	"Conventions"
#define FILE_TITLE_ATTR		"title"
#define TITLE_ATTR		"title"
#define PRODUCT_TYPE_ATTR	"product_type"
#define DATA_SET_ATTR	        "data_set"
#define MISSION_NAME_ATTR	"mission_name"
#define COMMENT_ATTR		"comment"
#define VALID_MIN_ATTR		"valid_min"
#define VALID_MAX_ATTR		"valid_max"

const std::string GENERIC_NETCDF_TYPE  = "Generic NetCdf";
const std::string YFX_NETCDF_TYPE  = "Y=F(X)";
const std::string ZFXY_NETCDF_TYPE  = "Z=F(X,Y)";


//-------------------------------------------------------------
//------------------- CNetCDFAttr class --------------------
//-------------------------------------------------------------
class CNetCDFAttr : public CBratObject
{
protected:

  CNetCDFAttr();
  CNetCDFAttr(const std::string& name, bool globalAttr = false);

  CNetCDFAttr(CNetCDFAttr& a);


public:

  virtual ~CNetCDFAttr();
  
  std::string GetName() { return m_name;};
  void SetName(const std::string& value) { m_name = value; };
  
  bool IsGlobalAttr() { return m_globalAttr;};
  void SetGlobalAttr(bool value) { m_globalAttr = value; };
  
  virtual CBratObject* Clone() = 0;
  CNetCDFAttr* CloneThis() { return CNetCDFAttr::GetNetCDFAttr(Clone(), true); };

  void Set(CNetCDFAttr& a);

  virtual bool IsValueDate() { return false;};

  const CNetCDFAttr& operator =(CNetCDFAttr& a);

  static CNetCDFAttr* NewFillValueAttr(nc_type type);

  static CNetCDFAttr* NewAttr(nc_type type, const std::string& name, size_t length = 1);

  nc_type GetType() { return m_type;};

  virtual void WriteAttribute(int32_t fileId, int32_t varId = NC_GLOBAL) = 0;
  
  virtual void ReadAttribute(int32_t fileId, int32_t varId = NC_GLOBAL) = 0;
  
  virtual void DeleteAttribute(int32_t fileId, int32_t varId = NC_GLOBAL);

  virtual void SetLength(size_t value) { m_length = value; };
  
  virtual bool IsTypeDateString();
  virtual bool IsTypeNumeric();
  virtual bool IsTypeNumericOrDateString();


  static CNetCDFAttr* GetNetCDFAttr(CBratObject* ob, bool withExcept = false);
  static CNetCDFAttrByte* GetNetCDFAttrByte(CBratObject* ob, bool withExcept = false);
  static CNetCDFAttrDouble* GetNetCDFAttrDouble(CBratObject* ob, bool withExcept = false);
  static CNetCDFAttrFloat* GetNetCDFAttrFloat(CBratObject* ob, bool withExcept = false);
  static CNetCDFAttrInt* GetNetCDFAttrInt(CBratObject* ob, bool withExcept = false);
  static CNetCDFAttrShort* GetNetCDFAttrShort(CBratObject* ob, bool withExcept = false);
  static CNetCDFAttrString* GetNetCDFAttrString(CBratObject* ob, bool withExcept = false);

  static CNetCDFAttrUByte* GetNetCDFAttrUByte(CBratObject* ob, bool withExcept = false);
  static CNetCDFAttrUShort* GetNetCDFAttrUShort(CBratObject* ob, bool withExcept = false);
  static CNetCDFAttrUInt* GetNetCDFAttrUInt(CBratObject* ob, bool withExcept = false);
  static CNetCDFAttrInt64* GetNetCDFAttrInt64(CBratObject* ob, bool withExcept = false);
  static CNetCDFAttrUInt64* GetNetCDFAttrUInt64(CBratObject* ob, bool withExcept = false);
  static CNetCDFAttrStringArray* GetNetCDFAttrStringArray(CBratObject* ob, bool withExcept = false);

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

protected:
  
  void Init();

protected:
  std::string m_name;
  nc_type m_type;
  bool m_globalAttr;
  
  size_t m_length;

};
//-------------------------------------------------------------
//------------------- CNetCDFAttrString class --------------------
//-------------------------------------------------------------
class CNetCDFAttrString : public CNetCDFAttr
{
public:

  CNetCDFAttrString();
  CNetCDFAttrString(const std::string& name);
  CNetCDFAttrString(const std::string& name, const std::string& value, bool globalAttr = false);
  CNetCDFAttrString(const std::string& name, CUnit& unit, bool globalAttr = false);
  CNetCDFAttrString(CNetCDFAttrString& a);

  virtual ~CNetCDFAttrString();

  const CDate& GetValueAsDate() { return m_date;};
  double GetValueDateAsDouble();
  virtual bool IsValueDate() { return !m_date.IsDefaultValue();};

  std::string GetValue() { return m_value;};
  void SetValue(const std::string& value);
  void SetValue(const char* value);
  void SetValue(const char* value, size_t length);
  
  virtual CBratObject* Clone();
  CNetCDFAttrString* CloneThis() { return CNetCDFAttr::GetNetCDFAttrString(Clone(), true); };

  void Set(CNetCDFAttrString& a);
  
  const CNetCDFAttrString& operator =(CNetCDFAttrString& a);

  virtual void WriteAttribute(int32_t fileId, int32_t varId = NC_GLOBAL);
  virtual void ReadAttribute(int32_t fileId, int32_t varId);

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

protected:
  
  void Init();

public:

  static const std::string m_DATE_UNIT;


protected:
  std::string m_value;

  CDate m_date;

};


//-------------------------------------------------------------
//------------------- CNetCDFAttrUByte class --------------------
//-------------------------------------------------------------
class CNetCDFAttrUByte : public CNetCDFAttr
{
public:

  CNetCDFAttrUByte();
  CNetCDFAttrUByte(const std::string& name);
  CNetCDFAttrUByte(const std::string& name, uint8_t value, bool globalAttr = false);
  CNetCDFAttrUByte(CNetCDFAttrUByte& a);

  virtual ~CNetCDFAttrUByte();

  CUInt8Array* GetValue() { return &m_value;};
  void SetValue(const CUInt8Array& value);
  void SetValue(uint8_t* values, size_t length);
  void SetValue(uint8_t value);

  virtual void SetLength(size_t value) { CNetCDFAttr::SetLength(value); m_value.resize(m_length); };
  
  virtual CBratObject* Clone();
  CNetCDFAttrUByte* CloneThis() { return CNetCDFAttr::GetNetCDFAttrUByte(Clone(), true); };

  void Set(CNetCDFAttrUByte& a);
  
  const CNetCDFAttrUByte& operator =(CNetCDFAttrUByte& a);

  virtual void WriteAttribute(int32_t fileId, int32_t varId = NC_GLOBAL);
  virtual void ReadAttribute(int32_t fileId, int32_t varId);
  
  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

protected:
  
  void Init();

protected:
  CUInt8Array m_value;

};

//-------------------------------------------------------------
//------------------- CNetCDFAttrDouble class --------------------
//-------------------------------------------------------------
class CNetCDFAttrDouble : public CNetCDFAttr
{
public:

  CNetCDFAttrDouble();
  CNetCDFAttrDouble(const std::string& name);
  CNetCDFAttrDouble(const std::string& name, double value, bool globalAttr = false);
  CNetCDFAttrDouble(CNetCDFAttrDouble& a);

  virtual ~CNetCDFAttrDouble();

  CDoubleArray* GetValue() { return &m_value;};
  void SetValue(const CDoubleArray& value);
  void SetValue(double* values, size_t length);
  void SetValue(double value);

  virtual void SetLength(size_t value) { CNetCDFAttr::SetLength(value); m_value.resize(m_length); };

  
  virtual CBratObject* Clone();
  CNetCDFAttrDouble* CloneThis() { return CNetCDFAttr::GetNetCDFAttrDouble(Clone(), true); };

  void Set(CNetCDFAttrDouble& a);
  
  const CNetCDFAttrDouble& operator =(CNetCDFAttrDouble& a);

  virtual void WriteAttribute(int32_t fileId, int32_t varId = NC_GLOBAL);
  virtual void ReadAttribute(int32_t fileId, int32_t varId);
  
  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

protected:
  
  void Init();

protected:

  CDoubleArray m_value;

};

//-------------------------------------------------------------
//------------------- CNetCDFAttrByte class --------------------
//-------------------------------------------------------------
class CNetCDFAttrByte : public CNetCDFAttr
{
public:

  CNetCDFAttrByte();
  CNetCDFAttrByte(const std::string& name);
  CNetCDFAttrByte(const std::string& name, int8_t value, bool globalAttr = false);
  CNetCDFAttrByte(CNetCDFAttrByte& a);

  virtual ~CNetCDFAttrByte();

  CInt8Array* GetValue() { return &m_value;};
  void SetValue(const CInt8Array& value);
  void SetValue(int8_t* values, size_t length);
  void SetValue(int8_t value);

  virtual void SetLength(size_t value) { CNetCDFAttr::SetLength(value); m_value.resize(m_length); };
  
  virtual CBratObject* Clone();
  CNetCDFAttrByte* CloneThis() { return CNetCDFAttr::GetNetCDFAttrByte(Clone(), true); };

  void Set(CNetCDFAttrByte& a);
  
  const CNetCDFAttrByte& operator =(CNetCDFAttrByte& a);

  virtual void WriteAttribute(int32_t fileId, int32_t varId = NC_GLOBAL);
  virtual void ReadAttribute(int32_t fileId, int32_t varId);
  
  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

protected:
  
  void Init();

protected:

  CInt8Array m_value;

};

//-------------------------------------------------------------
//------------------- CNetCDFAttrShort class --------------------
//-------------------------------------------------------------
class CNetCDFAttrShort : public CNetCDFAttr
{
public:

  CNetCDFAttrShort();
  CNetCDFAttrShort(const std::string& name);
  CNetCDFAttrShort(const std::string& name, int16_t value, bool globalAttr = false);
  CNetCDFAttrShort(CNetCDFAttrShort& a);

  virtual ~CNetCDFAttrShort();

  CInt16Array* GetValue() { return &m_value;};
  void SetValue(const CInt16Array& value);
  void SetValue(int16_t* values, size_t length);
  void SetValue(int16_t value);

  virtual void SetLength(size_t value) { CNetCDFAttr::SetLength(value); m_value.resize(m_length); };

  virtual CBratObject* Clone();
  CNetCDFAttrShort* CloneThis() { return CNetCDFAttr::GetNetCDFAttrShort(Clone(), true); };
  
  void Set(CNetCDFAttrShort& a);
  
  const CNetCDFAttrShort& operator =(CNetCDFAttrShort& a);

  virtual void WriteAttribute(int32_t fileId, int32_t varId = NC_GLOBAL);
  virtual void ReadAttribute(int32_t fileId, int32_t varId);
  
  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

protected:
  
  void Init();

protected:

  CInt16Array m_value;

};

//-------------------------------------------------------------
//------------------- CNetCDFAttrShort class --------------------
//-------------------------------------------------------------
class CNetCDFAttrUShort : public CNetCDFAttr
{
public:

  CNetCDFAttrUShort();
  CNetCDFAttrUShort(const std::string& name);
  CNetCDFAttrUShort(const std::string& name, uint16_t value, bool globalAttr = false);
  CNetCDFAttrUShort(CNetCDFAttrUShort& a);

  virtual ~CNetCDFAttrUShort();

  CUInt16Array* GetValue() { return &m_value;};
  void SetValue(const CUInt16Array& value);
  void SetValue(uint16_t* values, size_t length);
  void SetValue(uint16_t value);

  virtual void SetLength(size_t value) { CNetCDFAttr::SetLength(value); m_value.resize(m_length); };

  virtual CBratObject* Clone();
  CNetCDFAttrUShort* CloneThis() { return CNetCDFAttr::GetNetCDFAttrUShort(Clone(), true); };
  
  void Set(CNetCDFAttrUShort& a);
  
  const CNetCDFAttrUShort& operator =(CNetCDFAttrUShort& a);

  virtual void WriteAttribute(int32_t fileId, int32_t varId = NC_GLOBAL);
  virtual void ReadAttribute(int32_t fileId, int32_t varId);
  
  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

protected:
  
  void Init();

protected:

  CUInt16Array m_value;

};

//-------------------------------------------------------------
//------------------- CNetCDFAttrInt class --------------------
//-------------------------------------------------------------
class CNetCDFAttrInt : public CNetCDFAttr
{
public:

  CNetCDFAttrInt();
  CNetCDFAttrInt(const std::string& name);
  CNetCDFAttrInt(const std::string& name, int32_t value, bool globalAttr = false);
  CNetCDFAttrInt(CNetCDFAttrInt& a);

  virtual ~CNetCDFAttrInt();

  CIntArray* GetValue() { return &m_value;};
  void SetValue(const CIntArray& value);
  void SetValue(int32_t* values, size_t length);
  void SetValue(int32_t value);

  virtual void SetLength(size_t value) { CNetCDFAttr::SetLength(value); m_value.resize(m_length); };

  virtual CBratObject* Clone();
  CNetCDFAttrInt* CloneThis() { return CNetCDFAttr::GetNetCDFAttrInt(Clone(), true); };
  
  void Set(CNetCDFAttrInt& a);
  
  const CNetCDFAttrInt& operator =(CNetCDFAttrInt& a);

  virtual void WriteAttribute(int32_t fileId, int32_t varId = NC_GLOBAL);
  virtual void ReadAttribute(int32_t fileId, int32_t varId);
  
  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

protected:
  
  void Init();

protected:

  CIntArray m_value;

};

//-------------------------------------------------------------
//------------------- CNetCDFAttrUInt class --------------------
//-------------------------------------------------------------
class CNetCDFAttrUInt : public CNetCDFAttr
{
public:

  CNetCDFAttrUInt();
  CNetCDFAttrUInt(const std::string& name);
  CNetCDFAttrUInt(const std::string& name, uint32_t value, bool globalAttr = false);
  CNetCDFAttrUInt(CNetCDFAttrUInt& a);

  virtual ~CNetCDFAttrUInt();

  CUIntArray* GetValue() { return &m_value;};
  void SetValue(const CUIntArray& value);
  void SetValue(uint32_t* values, size_t length);
  void SetValue(uint32_t value);

  virtual void SetLength(size_t value) { CNetCDFAttr::SetLength(value); m_value.resize(m_length); };

  virtual CBratObject* Clone();
  CNetCDFAttrUInt* CloneThis() { return CNetCDFAttr::GetNetCDFAttrUInt(Clone(), true); };
  
  void Set(CNetCDFAttrUInt& a);
  
  const CNetCDFAttrUInt& operator =(CNetCDFAttrUInt& a);

  virtual void WriteAttribute(int32_t fileId, int32_t varId = NC_GLOBAL);
  virtual void ReadAttribute(int32_t fileId, int32_t varId);
  
  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

protected:
  
  void Init();

protected:

  CUIntArray m_value;

};

//-------------------------------------------------------------
//------------------- CNetCDFAttrInt64 class --------------------
//-------------------------------------------------------------
class CNetCDFAttrInt64 : public CNetCDFAttr
{
public:

  CNetCDFAttrInt64();
  CNetCDFAttrInt64(const std::string& name);
  CNetCDFAttrInt64(const std::string& name, int64_t value, bool globalAttr = false);
  CNetCDFAttrInt64(CNetCDFAttrInt64& a);

  virtual ~CNetCDFAttrInt64();

  CInt64Array* GetValue() { return &m_value;};
  void SetValue(const CInt64Array& value);
  void SetValue(int64_t* values, size_t length);
  void SetValue(int64_t value);

  virtual void SetLength(size_t value) { CNetCDFAttr::SetLength(value); m_value.resize(m_length); };

  virtual CBratObject* Clone();
  CNetCDFAttrInt64* CloneThis() { return CNetCDFAttr::GetNetCDFAttrInt64(Clone(), true); };
  
  void Set(CNetCDFAttrInt64& a);
  
  const CNetCDFAttrInt64& operator =(CNetCDFAttrInt64& a);

  virtual void WriteAttribute(int32_t fileId, int32_t varId = NC_GLOBAL);
  virtual void ReadAttribute(int32_t fileId, int32_t varId);
  
  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

protected:
  
  void Init();

protected:

  CInt64Array m_value;

};

//-------------------------------------------------------------
//------------------- CNetCDFAttrUInt64 class --------------------
//-------------------------------------------------------------
class CNetCDFAttrUInt64 : public CNetCDFAttr
{
public:

  CNetCDFAttrUInt64();
  CNetCDFAttrUInt64(const std::string& name);
  CNetCDFAttrUInt64(const std::string& name, uint64_t value, bool globalAttr = false);
  CNetCDFAttrUInt64(CNetCDFAttrUInt64& a);

  virtual ~CNetCDFAttrUInt64();

  CUInt64Array* GetValue() { return &m_value;};
  void SetValue(const CUInt64Array& value);
  void SetValue(uint64_t* values, size_t length);
  void SetValue(uint64_t value);

  virtual void SetLength(size_t value) { CNetCDFAttr::SetLength(value); m_value.resize(m_length); };

  virtual CBratObject* Clone();
  CNetCDFAttrUInt64* CloneThis() { return CNetCDFAttr::GetNetCDFAttrUInt64(Clone(), true); };
  
  void Set(CNetCDFAttrUInt64& a);
  
  const CNetCDFAttrUInt64& operator =(CNetCDFAttrUInt64& a);

  virtual void WriteAttribute(int32_t fileId, int32_t varId = NC_GLOBAL);
  virtual void ReadAttribute(int32_t fileId, int32_t varId);
  
  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

protected:
  
  void Init();

protected:

  CUInt64Array m_value;

};

//-------------------------------------------------------------
//------------------- CNetCDFAttrFloat class --------------------
//-------------------------------------------------------------
class CNetCDFAttrFloat : public CNetCDFAttr
{
public:

  CNetCDFAttrFloat();
  CNetCDFAttrFloat(const std::string& name);
  CNetCDFAttrFloat(const std::string& name, float value, bool globalAttr = false);
  CNetCDFAttrFloat(CNetCDFAttrFloat& a);

  virtual ~CNetCDFAttrFloat();

  CFloatArray* GetValue() { return &m_value;};
  void SetValue(const CFloatArray& value);
  void SetValue(float* values, size_t length);
  void SetValue(float value);

  virtual void SetLength(size_t value) { CNetCDFAttr::SetLength(value); m_value.resize(m_length); };

  virtual CBratObject* Clone();
  CNetCDFAttrFloat* CloneThis() { return CNetCDFAttr::GetNetCDFAttrFloat(Clone(), true); };
  
  void Set(CNetCDFAttrFloat& a);
  
  const CNetCDFAttrFloat& operator =(CNetCDFAttrFloat& a);

  virtual void WriteAttribute(int32_t fileId, int32_t varId = NC_GLOBAL);
  virtual void ReadAttribute(int32_t fileId, int32_t varId);
  
  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

protected:
  
  void Init();

protected:

  CFloatArray m_value;

};

class CNetCDFVarDef;
//-------------------------------------------------------------
//------------------- CNetCDFDimension class --------------------
//-------------------------------------------------------------
class CNetCDFDimension : public CBratObject
{

public:

  CNetCDFDimension();
  CNetCDFDimension(const std::string& name, int32_t length, bool isUnlimited = false);
  CNetCDFDimension(const std::string& name, CNetCDFDimension& from);
  CNetCDFDimension(CNetCDFDimension& d);

  virtual ~CNetCDFDimension();
  
  std::string GetName() { return m_name;};
  void SetName(const std::string& value) { m_name = value; };
 
  bool IsUnlimited() { return m_isUnlimited;};
  void SetIsUnlimited(bool value) { m_isUnlimited = value; };
  
  int32_t GetLength() { return m_length;};
  void SetLength(int32_t value) { m_length = value; };
  
  int32_t GetDimId() { return m_dimId; };
  void SetDimId(int32_t value) { m_dimId = value; };

  //CObArray* GetCoordinateVariables() { return &m_coordVars; };
  CStringArray* GetCoordinateVariables() { return &m_coordVars; };

  void AddCoordinateVariable(CNetCDFVarDef& v);
  void AddCoordinateVariable(CNetCDFVarDef* v);
  void AddCoordinateVariable(const std::string& name);

  bool IsCoordinateVariable(const std::string& name);
  bool IsCoordinateVariable(CNetCDFVarDef& cv);
  bool IsCoordinateVariable(CNetCDFVarDef* cv);
  
  //CNetCDFVarDef* GetCoordinateVariable(const std::string& name);

  void Set(CNetCDFDimension& d);

  const CNetCDFDimension& operator= (CNetCDFDimension& d);
  virtual bool operator== ( CNetCDFDimension& d ) { return str_icmp(this->GetName(), d.GetName()); }
  virtual bool operator== ( const std::string& text ) { return str_icmp(this->GetName(), text); }

  virtual bool operator!= ( CNetCDFDimension& d ) { return ! (*this == d); }
  virtual bool operator!= ( const std::string& text ) { return ! (*this == text); }


  virtual bool IsEqual(CNetCDFDimension& d) 
                { return ((*this == d) && (this->m_length == d.m_length)); };
  
  virtual bool IsNotEqual(CNetCDFDimension& d) 
                { return ! (this->IsEqual(d)) ; };

  virtual CBratObject* Clone();
  CNetCDFDimension* CloneThis() { return dynamic_cast<CNetCDFDimension*>(Clone()); };

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);



protected:
  void Init();

protected:

  std::string m_name;
  int32_t m_length;

  int32_t m_dimId;

  //CObArray m_coordVars;
  CStringArray m_coordVars;
  bool m_isUnlimited;


};

//-------------------------------------------------------------
//------------------- CNetCDFVarDef class --------------------
//-------------------------------------------------------------
class CNetCDFVarDef : public CBratObject
{

public:

  CNetCDFVarDef(nc_type type = NC_DOUBLE);
  CNetCDFVarDef(const std::string& name, const CUnit& unit, nc_type type = NC_DOUBLE);
  CNetCDFVarDef(const std::string& name, const std::string& unit, nc_type type = NC_DOUBLE);
  CNetCDFVarDef(const std::string& name, nc_type type = NC_DOUBLE);
  //CNetCDFVarDef(const std::string& name, CNetCDFDimension* dim, nc_type type = NC_DOUBLE);
  CNetCDFVarDef(CNetCDFVarDef& v);

  virtual ~CNetCDFVarDef();
  
  std::string GetName() { return m_name;};
  void SetName(const std::string& value) { m_name = value; };
  
  std::string GetLongName();
  
  CObMap* GetAttributes() { return &m_mapAttributes;};

  void AddAttribute(CNetCDFAttr* attr);
  void AddAttributes(const CStringMap& mapAttributes);

  CNetCDFAttr* GetAttribute(const std::string& name);

  CUnit* GetUnit() { return &m_unit;};
  void SetUnit(const CUnit& value);
  void SetUnit(const std::string& value);

  bool IsDateTime() { return m_unit.HasDateRef();};

  bool HasCompatibleUnit(CNetCDFVarDef* netCDFVarDef);

  nc_type GetType() { return m_type;};
  void SetType(nc_type value);
   
  int32_t GetNbDims();
  long GetNbData();

  double GetScaleFactor() { return m_scaleFactor;};
  void SetScaleFactor(double value) { m_scaleFactor = value; };
   
  double GetAddOffset() { return m_addOffset;};
  void SetAddOffset(double value) { m_addOffset = value; };
   
  double GetFillValue() { return m_fillValue;};
  //void SetFillValue(char value);
  void SetFillValue(int8_t value);
  void SetFillValue(uint8_t value);
  void SetFillValue(int16_t value);
  void SetFillValue(uint16_t value);
  void SetFillValue(int32_t value);
  void SetFillValue(uint32_t value);
  void SetFillValue(float value);
  void SetFillValue(double value);
   
  void Set(CNetCDFVarDef& v);

  const CNetCDFVarDef& operator =(CNetCDFVarDef& v);

  virtual CBratObject* Clone();
  CNetCDFVarDef* CloneThis() { return dynamic_cast<CNetCDFVarDef*>(Clone()); };

  CObArray* GetNetCDFDims() {return &m_netCDFdims; };

  CUIntArray* GetDims() { return &m_dims;};

  void AddNetCDFDims(CObArray& netCDFdims);

  CNetCDFDimension* AddNetCDFDim(CNetCDFDimension& dim);
  //CNetCDFDimension* AddNetCDFDim(CNetCDFDimension* dim);

  CNetCDFDimension* SetNetCDFDim(CNetCDFDimension& dim, uint32_t index);
  void SetNetCDFDims(const CStringArray* dimNames, const CUIntArray* dimValues, const CIntArray* dimIds = NULL);
  //void SetNetCDFDims(const CUIntMap& dimsMap);


  CNetCDFDimension* InsertNetCDFDim(CNetCDFDimension& dim, uint32_t index);
  CNetCDFDimension* ReplaceNetCDFDim(CNetCDFDimension& dim);

  
  CNetCDFDimension* GetNetCDFDim(const std::string& name);
  CNetCDFDimension* GetNetCDFDim(uint32_t index);

  uint32_t GetNetCDFDimRange(const std::string& name);

  void GetNetCdfDimNames(CStringArray& dimNames) const;
  
  void GetNetCdfDimIds(CIntArray& dimId) const;

  bool HasCommonDims(const std::string& dimName);
  bool HasCommonDims(const CStringArray& dimNames);

  void GetCommomDims(const std::string& dimName, CStringArray& intersect);
  void GetCommomDims(const CStringArray& dimNames, CStringArray& intersect);

  void GetComplementDims(const std::string& dimName, CStringArray& complement);
  void GetComplementDims(const CStringArray& dimNames, CStringArray& complement);

  bool HaveEqualDimSizes(CNetCDFVarDef* netCDFVarDef, std::string* msg = NULL);

  bool HaveEqualDimNames(CNetCDFVarDef* netCDFVarDef,std::string* msg = NULL);

  bool HaveEqualDims(CNetCDFVarDef* netCDFVarDef, CNetCDFFiles* netCDFFiles1 = NULL, CNetCDFFiles* netCDFFiles2 = NULL, std::string* msg = NULL);
  
  bool HaveCompatibleDimUnits(CNetCDFVarDef* netCDFVarDef, CNetCDFFiles* netCDFFiles1, CNetCDFFiles* netCDFFiles2, std::string* msg = NULL);

  virtual bool operator== ( CNetCDFVarDef& d ) { return str_icmp(this->GetName(), d.GetName()); }
  virtual bool operator== ( const std::string& text ) { return str_icmp(this->GetName(), text); }
  
  virtual bool operator!= ( CNetCDFVarDef& d ) { return ! (*this == d); }
  virtual bool operator!= ( const std::string& text ) { return ! (*this == text); }

  double GetValidMin() { return m_validMin; };
  double GetValidMax() { return m_validMax; };
  
  void SetValidMin (double value)
        { m_validMin = value; 
          AddAttribute(new CNetCDFAttrDouble(VALID_MIN_ATTR, m_validMin));
        };

  void SetValidMax (double value)
        { m_validMax = value; 
          AddAttribute(new CNetCDFAttrDouble(VALID_MAX_ATTR, m_validMax));
        };

  void SetValidMinMax (double min, double max) {SetValidMin(min); SetValidMax(max);};

  void AddFillValueAttr();

  int32_t GetVarId() { return m_varId; };
  void SetVarId(int32_t value) { m_varId = value; };


  virtual void WriteAttributes(int32_t fileId);

  void WriteData(int32_t fileId, CExpressionValue* values);
  void WriteData(int32_t fileId, CMatrix* matrix);
  void WriteData(int32_t fileId, CMatrixDouble* matrix);
  void WriteData(int32_t fileId, CMatrixDoublePtr* matrix);

  bool IsCoordinateAxis();
  static bool IsCoordinateAxis(CNetCDFVarDef* var);

  static CNetCDFVarDef* GetNetCDFVarDef(CBratObject* ob, bool withExcept = false);

  static bool NetcdfVarCompareByName(CBratObject* o1, CBratObject* o2);

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);


protected:
  
  void Init();

  void AddDim(uint32_t dim);
  void SetDim(uint32_t dim, uint32_t index);
  void InsertDim(uint32_t dim, uint32_t index);

  void SetDims(const CUIntArray& dims) ;

protected:
  std::string m_name;
  
  CObMap m_mapAttributes;
 
  nc_type m_type;

  CUIntArray m_dims;
  CObArray m_netCDFdims;
  
  double m_validMin;
  double m_validMax;

  int32_t m_varId;

  CUnit m_unit;

   double m_scaleFactor;
   double m_addOffset;

   double m_fillValue;


};


//-------------------------------------------------------------
//------------------- CNetCDFCoordinateAxis class --------------------
//-------------------------------------------------------------
class CNetCDFCoordinateAxis : public CNetCDFVarDef
{
public:

  CNetCDFCoordinateAxis(NetCDFVarKind dimKind = Unknown, nc_type type = NC_DOUBLE);
  CNetCDFCoordinateAxis(const std::string& name, NetCDFVarKind dimKind = Unknown, nc_type type = NC_DOUBLE);
  //CNetCDFCoordinateAxis(const std::string& name, CNetCDFDimension* dim, NetCDFVarKind dimKind = Unknown, nc_type type = NC_DOUBLE);
  CNetCDFCoordinateAxis(const std::string& name, const CUnit& unit, NetCDFVarKind dimKind = Unknown, nc_type type = NC_DOUBLE);
  CNetCDFCoordinateAxis(const std::string& name, const std::string& unit, NetCDFVarKind dimKind = Unknown, nc_type type = NC_DOUBLE);
  CNetCDFCoordinateAxis(CNetCDFCoordinateAxis& v);
  CNetCDFCoordinateAxis(CNetCDFVarDef& v);

  virtual ~CNetCDFCoordinateAxis();
  
  
  NetCDFVarKind GetDimKind() { return m_dimKind;};
  void SetDimKind(NetCDFVarKind value);
  
  std::string GetDimType() { return m_dimType;};
  void SetDimType(const std::string& value) { m_dimType = value;};

  std::string GetAxis() { return m_axis;};
  void SetAxis(const std::string& value) { m_axis = value;};
  
  void Set(CNetCDFCoordinateAxis& v);
  void Set(CNetCDFVarDef& v);

  bool IsTimeAxis() { return (m_dimKind == T); };

  const CNetCDFCoordinateAxis& operator =(CNetCDFCoordinateAxis& v);
  const CNetCDFCoordinateAxis& operator =(CNetCDFVarDef& v);

  virtual CBratObject* Clone();
  CNetCDFCoordinateAxis* CloneThis() { return dynamic_cast<CNetCDFCoordinateAxis*>(Clone()); };

  //static void GetDimType(NetCDFVarKind dimKind, std::string& dimType, std::string& axis);
  virtual void WriteAttributes(int32_t fileId);

  static CNetCDFCoordinateAxis* GetNetCDFCoordAxis(CBratObject* ob, bool withExcept = false);

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

protected:
  
  void Init();

  void WriteMinMaxGobalAttr(int32_t fileId);
  void WriteMinMaxGobalAttr(int32_t fileId, bool wantMin);


protected:
      
  NetCDFVarKind m_dimKind;
  
  std::string m_dimType;
  std::string m_axis;
};

//-------------------------------------------------------------
//------------------- CNetCDFFiles class --------------------
//-------------------------------------------------------------
class CNetCDFFiles : public CBratObject
{
public:

  CNetCDFFiles ();
 
  CNetCDFFiles (const std::string& name,
	        brathl_FileMode	mode = ReadOnly);
 
  ~CNetCDFFiles();

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

/*
** File management
*/

  void SetName(const std::string& name);
  std::string GetName() const {return m_fileName;};

  void SetMode(brathl_FileMode mode);

  void Open(brathl_FileMode mode);
  void Open();

  void Close();

  bool IsOpened() const;

  // throws an exception if file is not opened
  void MustBeOpened();

  // True if opened with mode New or Create
  bool IsCreating();


  static std::string IdentifyExistingFile
		(const std::string		&name,
		 bool			checkConvention	= false,
		 bool			noError		= false);
  
  std::string IdentifyExistingFile
		(bool			checkConvention	= false,
                bool			noError		= false);

  void WriteFileTitle
		(const std::string	&Title);


/*
** Dimensions
*/
  uint32_t GetMaxFieldNumberOfDims(const CStringArray* fieldNames = NULL);
  

  bool HaveEqualDimNames(const std::string& v1, const CStringArray& v2, std::string* msg = NULL);
  bool HaveEqualDimNames(const CStringArray& v1, const CStringArray& v2, std::string* msg = NULL);
  bool HaveEqualDimNames(const std::string& v1, const std::string& v2, std::string* msg = NULL);
  bool HaveEqualDimNames(CNetCDFVarDef* v1, CNetCDFVarDef* v2, std::string* msg = NULL);
  
  void GetNetCDFDimsNames(CStringArray& names) {m_mapNetCDFDims.GetKeys(names); };

  CObMap* GetNetCDFDims() {return &m_mapNetCDFDims; };
  CNetCDFDimension* AddNetCDFDim(CNetCDFDimension& dim, bool forceReplace = false);

  void SetNetCDFDims(const CStringArray* dimNames, const CUIntArray* dimValues, const CIntArray* dimIds = NULL);

  void GetNetCDFDims(const std::string& varName, CObArray* dims);
  CNetCDFDimension* GetNetCDFDim(const std::string& name);
  
  void GetNetCDFVarDefNames(CStringArray& names) {m_mapNetCDFVarDefs.GetKeys(names); };

  CObMap* GetNetCDFVarDefs() {return &m_mapNetCDFVarDefs; };
  CNetCDFVarDef* AddNetCDFVarDef(CNetCDFVarDef& var, bool forceReplace = false);

  CNetCDFVarDef* GetNetCDFVarDef(const std::string& name, bool withExcept = false);

  void ReplaceNetCDFDim(CNetCDFDimension& dim);

  void GetNetCDFCoordAxes(const std::string& varName, CObMap* coordAxes);
  void GetNetCDFCoordAxes(const std::string& varName, CObArray* coordAxes);
  void GetNetCDFCoordAxes(const std::string& varName, CStringArray* coordAxes, bool bRemoveAll = true, bool bUnique = false);
  void GetNetCDFCoordAxes(const CStringArray& varNames, CStringArray* coordAxes, bool bRemoveAll = true, bool bUnique = false);

  void GetNetCDFCoordAxes(CObMap* coordAxes);
  void GetNetCDFCoordAxes(CObArray* coordAxes);
  void GetNetCDFCoordAxes(CStringArray* coordAxesNames, bool bRemoveAll = true, bool bUnique = false);
  
  void GetNetCDFVarDefsTime(CObArray* netCDFVarDefs);
  void GetNetCDFVarDefsTime(CObMap* netCDFVarDefs);
  
  void GetNetCDFVarDefsTimeByDim(CObArray* netCDFVarDefs, bool sortedArray = false);
  void GetNetCDFVarDefsTimeByDim(CObMap* netCDFVarDefs);

  void GetNetCDFCoordAxesTime(CObArray* coordAxes);
  void GetNetCDFCoordAxesTime(CObMap* coordAxes);

  void AddCoordinateVariable(const CStringArray& dimNames, const std::string& varName);
  void AddCoordinateVariable(const CStringArray* dimNames, const std::string& varName);

  CObMap* GetNetCDFGlobalAttributes() { return &m_mapNetCDFGlobalAttrs; };

  CNetCDFAttr* GetNetCDFGlobalAttribute(const std::string& name, bool withExcept = false);

  //CNetCDFAttr* AddNetCDFGlobalAttributes(CNetCDFAttr& attr);
  CNetCDFAttr* AddNetCDFGlobalAttributes(CNetCDFAttr* attr);

  void WriteDimensions();
  void WriteDimension(CNetCDFDimension* netCDFDim);

  void GetInfoStructure(int32_t& nbDims, int32_t& nbVars, int32_t& nbGlobalAttr, int32_t& unlimitedDimId);

  int32_t GetNbDimensions();

  int GetDimId(const std::string& name, bool withExcept = true);

  bool DimExists(const std::string& name);

  std::string GetDimName(int			DimId);

  void GetDimensions(std::vector<std::string>		&Names);

  uint32_t GetDimension
		(const std::string		&Name);

  size_t GetDimension(int DimId);

  int SetDimension
		(NetCDFVarKind		DimKind,
		 const std::string		&DimName,
		 int32_t		Value,
		 const CExpressionValue	&DimValue,
		 const std::string		&Units,
		 const std::string		&LongName,
                 const std::string           &Comment = "",
      	         double	                ValidMin = CTools::m_defaultValueDOUBLE,
		 double	                ValidMax = CTools::m_defaultValueDOUBLE);

//  void SetDimension(CNetCDFCoordinateAxis* dim, CDoubleArray* values);

     
/*
** Variables
*/
  void WriteVariables();
  void WriteVariable(CNetCDFVarDef* var);

  void WriteData(CNetCDFVarDef* varDef, CExpressionValue* values);
  void WriteData(CNetCDFVarDef* varDef, CMatrix* matrix);

  int32_t GetNbVariables();

  int GetVarId(const std::string	&name, bool	global = false, bool withExcept = true);

  std::string GetVarName(int		VarId);

  int GetVarNbDims(const std::string			&Name);
  int GetVarNbDims(int				VarId);

  void GetVarDims(const std::string &Name, ExpressionValueDimensions	&Dimensions);
  void GetVarDims(int				VarId, ExpressionValueDimensions	&Dimensions);

  void GetVarDims(const std::string &Name, std::vector<std::string>			&Dimensions);
  void GetVarDims(int	VarId, std::vector<std::string>			&Dimensions);

  void GetVarDimIds(const std::string	&name, CIntArray& dimIds);

  void GetVarDimIds(int		varId, CIntArray& dimIds);
  
  bool IsAxisVar(const std::string		&Name);
  bool IsAxisVar(int			VarId);

  void GetVariables(std::vector<std::string>		&VarNames);

  void GetAxisVars(std::vector<std::string>		&VarNames);

  void GetAxisVars(std::vector<int>		&VarIds);

  void GetDataVars(std::vector<std::string>		&VarNames);

  void GetDataVars(std::vector<int>		&VarIds);

  bool VarExists(const std::string		&Name);


  int AddVar
		(const std::string	&VarName,
		       nc_type	VarType,
		 const std::string	&Units,
		 const std::string	&LongName,
		 const std::string	&Comment  = "",
		       int	Dim1Id	= -1,
		       int	Dim2Id	= -1,
		       int	Dim3Id	= -1,
		       int	Dim4Id	= -1,
                       double	ValidMin = CTools::m_defaultValueDOUBLE,
		       double	ValidMax = CTools::m_defaultValueDOUBLE);

  void WriteVar(const std::string		&Name, const CExpressionValue	&Value);

  void ReadVar(const std::string		&name, CExpressionValue	&exprValue, const std::string		&wantedUnit);

  void ReadVar(const std::string& name, CDoubleArray& value, const std::string& wantedUnit);

                 // Low level access. Should be used with extreme care: no control is done on size of data
  // Nbdimensions: Number of dimensions od data
  // Start: start index for each dimension
  // Count: number of daa in each dimension
  // Value: should be able to contain X double data where X is the product of the 'NbDimensions'
  //        values of 'Count' or 1 if 'NbDimensions' is 0.
  void ReadVar(const std::string	&Name, uint32_t	NbDimensions, uint32_t	*Start, uint32_t	*Count, double	*Value, const std::string	&WantedUnit);

  void ReadVar(CFieldNetCdf* field, double& value, const std::string& wantedUnit);
  void ReadVar(CFieldNetCdf* field, CExpressionValue& exprValue, const std::string& wantedUnit);
  void ReadVar(CFieldNetCdf* field, CDoubleArray& values, const std::string& wantedUnit);
  void ReadVar(CFieldNetCdf* field, const std::string& wantedUnit);

  void ReadEntireVar(CNetCDFVarDef* netCDFVarDef, CDoubleArray& array, const std::string& wantedUnit);
  void ReadEntireVar(CNetCDFDimension* netCDFDimension, CDoubleArray& array, const std::string& wantedUnit);

  static bool IsVarNameValid(const std::string	&Name);

  static NetCDFVarKind StringToVarKind(const std::string	&Name);

  static std::string VarKindToString(NetCDFVarKind	VarKind);


  NetCDFVarKind GetVarKind(const std::string		&Name);

  NetCDFVarKind GetVarKind(int			VarId);

  static NetCDFVarKind GetVarKindFromStringValue(const std::string& value);

  static bool IsCandidateForLatitude(const std::string& name);
  static bool IsCandidateForLongitude(const std::string& name);
  static bool IsCandidateForTime(const std::string& name);

  bool HasVar(NetCDFVarKind	VarKind);
  
  // Returns var id of axis var corresponding to var kind
  // or a negative value if not found.
  int WhichAxisVar(NetCDFVarKind	VarKind);

  // Returns coordinate axis corresponding to var kind
  CNetCDFCoordinateAxis* WhichCoordAxis(NetCDFVarKind	varKind);

  // Returns coordinate axis corresponding to var kind and a variable name
  CNetCDFCoordinateAxis* WhichCoordAxis(NetCDFVarKind	varKind, const std::string& varName);

  // Returns coordinate axis corresponding to var kind and a coordiante axes list
  static CNetCDFCoordinateAxis* WhichCoordAxis(NetCDFVarKind varKind, CObMap* coordAxes);


/*
** Attributes
*/
  void WriteAttributes(CNetCDFVarDef* var);

  // Unit of a variable or 'count' if not set
  CUnit GetUnit(const std::string			&Name);

  CUnit GetUnit(int				VarId);

  // If name is empty, it is the title of the file which is returned
  // (global attribute 'title' instead of 'long_name' of the variable)
  std::string GetTitle(const std::string			&Name);

  int GetAtt(const std::string	&VarName,
		 const std::string	&AttName,
		       std::string	&AttValue,
		       bool	MustExist	= true,
		 const std::string	Default		= "");

  
  int GetAtt(int		VarId,
		 const std::string	&AttName,
		       std::string	&AttValue,
		       bool	MustExist	= true,
		 const std::string	Default		= "");

  int GetAtt(const std::string	&VarName,
		 const std::string	&AttName,
		       double	&AttValue,
		       bool	MustExist	= true,
		       double   Default		= CTools::m_defaultValueDOUBLE);

  int GetAtt(int		VarId,
		 const std::string	&AttName,
		       double	&AttValue,
		       bool	MustExist	= true,
		       double   Default		= CTools::m_defaultValueDOUBLE);

  int GetGlobalAttributeAsDouble(const std::string	&attName,
		       double	&attValue,
		       bool	mustExist	= true,
		       double   defaultValue	= CTools::m_defaultValueDOUBLE);

  int GetGlobalAttributeAsString(const std::string	&attName,
		       std::string	&attValue,
		       bool	mustExist	= true,
		       std::string   defaultValue	= "");
  
  int GetAttributeAsDouble(const std::string	&varName,
		 const std::string	&attName,
		       double	&attValue,
		       bool	mustExist	= true,
		       double   defaultValue		= CTools::m_defaultValueDOUBLE);

  int GetAttributeAsDouble(int		varId,
		 const std::string	&attName,
		       double	&attValue,
		       bool	mustExist	= true,
		       double   defaultValue		= CTools::m_defaultValueDOUBLE);

  int GetAttributeAsString(int		varId,
		 const std::string&	attName,
		       std::string&	attValue,
		       bool	mustExist	= true,
		 const std::string&	defaultValue	= "");

  int GetAttributeAsString(const std::string	&varName,
		 const std::string&	attName,
		       std::string&	attValue,
		       bool	mustExist	= true,
		 const std::string&	defaultValue	= "");


  int GetAttributeInfo(const std::string	&varName,
		 const std::string&  attName,
		 nc_type&	type,
		 size_t&	length,
                 bool	        mustExist	= true);

  int GetAttributeInfo(int		varId,
		 const std::string&  attName,
		 nc_type&	type,
		 size_t&	length,
                 bool	        mustExist	= true);

  void GetAttributes(const std::string&  varName,
		   CStringMap&	mapAttributes);

  void GetAttributes(const std::string&  varName,
		   CDoubleMap&	mapAttributes);

  void GetAttributes(int		varId,
		   CStringMap&	mapAttributes);

  void GetAttributes(int		varId,
		   CDoubleMap&	mapAttributes);


  void DelAtt(const std::string	&VarName,
		 const std::string	&AttName);
  void DelAtt(int		VarId,
		 const std::string	&AttName);

  void PutAtt(const std::string	&VarName,
		 const std::string	&AttName,
		 const std::string	&AttValue);

  void PutAtt(int		VarId,
		 const std::string	&AttName,
		 const std::string	&AttValue);

  void PutAtt(const std::string	&VarName,
		 const std::string	&AttName,
		       double	AttValue,
		       nc_type	AttType	= NC_DOUBLE);

  void PutAtt(int		VarId,
		 const std::string	&AttName,
		       double	AttValue,
		       nc_type	AttType	= NC_DOUBLE);

  // Retrieve the default value of a variable (contained in _FillValue
  // attribute or by the library, if the attribute is not defined)
  double GetNetcdfVarDefValue(int	varId);

  double GetNetcdfVarDefValue(int	  varId, nc_type  varType);

  double GetNetcdfVarScaleFactor(int32_t  varId);
  double GetNetcdfVarAddOffset(int32_t  varId);

  // Retreive the default value to put in _FillValue according to
  // a data type
  double GetDefValue(nc_type	VarType);

  nc_type GetVarType(const std::string &name);
  nc_type GetVarType(int varId);
  
  virtual std::string GetVarTypeName(const std::string &name);
  virtual std::string GetVarTypeName(int varId);
  
  nc_type GetAttributeType(const std::string& attName);
  nc_type GetAttributeType(const std::string& varName, const std::string& attName);
  nc_type GetAttributeType(int varId, const std::string& attName);

  static std::string GetTypeAsString(int varType);

  bool LoadDimensions();
  bool LoadVariables();
  void LoadAttributes(CNetCDFVarDef* netCDFVarDef = NULL);

  bool IsGeographic();

  static void HandleNetcdfError(int	status, const std::string& message = "PROGRAM ERROR: error should not happen");
  
public:

  static const int32_t m_TIME_NAMES_SIZE;
  static const std::string m_TIME_NAMES[];

  static const int32_t m_LAT_NAMES_SIZE;
  static const std::string m_LAT_NAMES[];

  static const int32_t m_LON_NAMES_SIZE;
  static const std::string m_LON_NAMES[];

  static const int32_t m_CYCLE_NAMES_SIZE;
  static const std::string m_CYCLE_NAMES[];

  static const int32_t m_PASS_NAMES_SIZE;
  static const std::string m_PASS_NAMES[];
  
  static const int32_t m_LAT_UNITS_SIZE;
  static const std::string m_LAT_UNITS[];
  
  static const int32_t m_LON_UNITS_SIZE;
  static const std::string m_LON_UNITS[];


protected:

  std::string			m_fileName;
  brathl_FileMode		m_fileMode;
  
  CObMap m_mapNetCDFDims;
  CObMap m_mapNetCDFVarDefs;
  CObMap m_mapNetCDFGlobalAttrs;

  int32_t m_nDims;
  int32_t m_nVars;
  int32_t m_nGlobalAttrs;
  int32_t m_unlimitedDimId;


protected:

  void Init();

  /*
  ** Structure
  */

  bool LoadStructure();


  void SetDefineMode
		();
  void SetDataMode
		();

  void CheckNetcdfError(int	Error, const std::string	&Message = "PROGRAM ERROR: error should not happen");

private:
  int				m_file;
  bool				m_changingStructure;
  bool				m_creating;

  void ApplyUnitAndFactorsToDataRead
		(double		*Data,
		 uint32_t	NbData,
		 int		VarId,
		 const std::string	&WantedUnit);

/*
  void ApplyUnitAndFactorsToDataRead
		(CDoubleArray& array,
		 int		varId,
		 const std::string& wantedUnit);
*/
};

/** @} */

}

#endif // Already included .h
