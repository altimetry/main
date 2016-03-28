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
#if !defined(_Field_h_)
#define _Field_h_

#include <string>

#include "new-gui/Common/tools/brathl_error.h" 
#include "brathl.h" 

#include "coda.h"

#include "List.h"
#include "BratObject.h"


namespace brathl
{
  class CNetCDFFiles;
  class CNetCDFVarDef;
  class CNetCDFAttr;
  class CFieldNetCdfCFAttr;
  class CFieldNetCdfIndexData;
}

#include "NetCDFFiles.h"
#include "Expression.h"

using namespace brathl;


namespace brathl
{

#ifndef MAX
#define MAX(X,Y)	((X) >= (Y) ? (X) : (Y))
#endif


class CFieldSet;
class CFieldSetDbl;
class CFieldSetArrayDbl;

class CFieldRecord;
class CFieldIndex;

//-------------------------------------------------------------
//------------------- CField class --------------------
//-------------------------------------------------------------
//const int32_t MAX_NUM_DIMS = MAX(BRAT_MAX_NUM_DIMS, NC_MAX_DIMS);
const int32_t MAX_NUM_DIMS = CODA_MAX_NUM_DIMS;

/** \addtogroup product Products
  @{ */

/** 
  Field management base classe.

 
 \version 1.0
*/

class CField : public CBratObject
{
 

public:   
  /// Ctor
  CField();  
  
  CField(const std::string& name, const std::string& description = "", const std::string& unit = "");  
  CField(CField& f);  
  
  /// Dtor
  virtual ~CField();

  const CField& operator= (CField& f);

  void Set(CField& f);



  
//-------------------------------------------------------------
//------------------- CListField class --------------------
//-------------------------------------------------------------
  /** 
    A list of CField object management class

   \version 1.0
  */
  class CListField : public CObList
  {
  public:
    CListField(bool bDelete) : CObList(bDelete) {};
    virtual ~CListField() {};
    CField* Back(bool withExcept = true);
    CField* Front(bool withExcept = true);

    virtual void InsertField(CField* field, bool hasDataset = true, bool bEnd = true);
    void RemoveAll();

    int32_t m_nbFieldSetDims;
    CUIntArray m_fieldSetDim;
    
  };

//-------------------------------------------------------------

  virtual std::string GetNativeTypeName();
  virtual std::string GetSpecialTypeName();

  void SetKey(const std::string& key) { m_key = key; };
  const std::string& GetKey() { return m_key; };

  void SetName(const std::string& name) { m_name = name; };
  const std::string& GetName() const { return m_name; };

  virtual std::string GetFullName();
  virtual std::string GetRecordName();			//v4 note: cannot be const; changes field state
  virtual std::string GetFullNameWithRecord();

  void SetDescription(const std::string& description) { m_description = description; };
  const std::string& GetDescription() const { return m_description; };

  virtual void SetUnit(const std::string& unit) { m_unit = CTools::RemoveCharSurroundingNumber(unit); };
  const std::string& GetUnit() const { return m_unit; };
  
  void SetTypeClass(coda_type_class typeClass) { m_typeClass = typeClass; };
  coda_type_class GetTypeClass() { return m_typeClass; };

  void SetDateRef(brathl_refDate refDate);

  void SetDateRef(const CDate& value);
  
  const CDate& GetDateRef() { return m_dateRef; };

  void SetSpecialType(coda_special_type specialType) { m_specialType = specialType; };
  coda_special_type GetSpecialType() { return m_specialType; };
  virtual bool IsSpecialType() { return m_specialType != coda_special_no_data; };

  void SetNativeType(coda_native_type nativeType) { m_nativeType = nativeType; };
  coda_native_type GetNativeType() { return m_nativeType; };

  void SetMetaData(bool metaData) { m_metaData = metaData; };
  bool IsMetaData() { return m_metaData; };

  void SetIndex(int32_t index) { m_index = index; };
  int32_t GetIndex() { return m_index; };

  void SetUnion(int32_t value) { m_isUnion = value; };
  int32_t GetUnion() { return m_isUnion; };
  bool IsUnion() { return m_isUnion == 1; };

  void SetToBeRemoved(bool value) { m_toBeRemoved = value; };
  bool IsToBeRemoved() { return m_toBeRemoved; };



  void SetDim(int32_t nbDims, const long dim[]);
  void SetDim(int32_t nbDims, const CUIntArray& dim);
  void SetDim(const CUIntArray& dim);
  void SetDim(const CUIntArray* dim);
  void SetDim(int32_t nbElts);

  bool TransposeDim();
  bool TransposeValues(double* data, int32_t size);

  void ConvertDefaultValueInt8(double* data, int32_t size);
  void ConvertDefaultValueUInt8(double* data, int32_t size);
  void ConvertDefaultValueInt16(double* data, int32_t size);
  void ConvertDefaultValueUInt16(double* data, int32_t size);
  void ConvertDefaultValueInt32(double* data, int32_t size);
  void ConvertDefaultValueUInt32(double* data, int32_t size);
  void ConvertDefaultValueInt64(double* data, int32_t size);
  void ConvertDefaultValueUInt64(double* data, int32_t size);
  void ConvertDefaultValueFloat(double* data, int32_t size);

  void Convert(double* data, int32_t size);
  void SetDefaultValue(double* data, int32_t size);

  int32_t GetNbDims() {return m_nbDims;};
  virtual int32_t GetVirtualNbDims();
  virtual uint32_t GetOffsetDim();
  
  bool HasDim() {return m_nbDims > 0;};
  virtual bool HasVirtualNbDims() {return GetVirtualNbDims() > 0; };
  
  inline bool HasXDim() {return m_nbDims >= 1;};
  inline bool HasYDim() {return m_nbDims >= 2;};

  long* GetDim() { return m_dim;};
  long GetDimAt(int32_t index);
  void GetDimAsVector(CUIntArray& dim);
  virtual std::string GetDimAsString();

  bool IsFixedSize() const {return m_isFixedSize;};
  void SetFixedSize(bool isFixedSize) {m_isFixedSize = isFixedSize;};

  int32_t GetCurrentPos() {return m_currentPos;};
  void SetCurrentPos(int32_t currentPos) {m_currentPos = currentPos;};
  void SetCurrentPosToLast() {m_currentPos = m_maxPos;};
  
  void SetGoToAvailableUnionField(bool value) { m_goToAvailableUnionField = value; };
  bool IsGoToAvailableUnionField() { return m_goToAvailableUnionField; };

  virtual void PushPos() = 0;
  virtual void PopCursor() = 0;

  void PopRecordCusor(CObList* parentFieldList);

  coda_Cursor* GetCursor() {return &m_cursor;};
  void SetCursor(coda_Cursor& cursor) {m_cursor = cursor;};

  int GetMaxPos() {return m_maxPos;};
  int GetNbElts() {return m_maxPos + 1;};
  bool End() {return m_currentPos > m_maxPos;};
  bool LastRecord() {return m_currentPos == m_maxPos;};
  
  virtual void ReadParent(CDoubleArray& vect, CFieldRecord* parentField);
  virtual void ReadParent(CDoubleArray& vect, CObList* parentFieldList);

  virtual void Read(CDoubleArray& vect, bool skip = false);
  virtual void Read(double* data, bool skip = false);
  virtual void Read(std::string& value, bool skip = false);

  virtual CFieldSet* CreateFieldSet(const CField::CListField& listFields) = 0;

  virtual bool GetHighResolution() {return m_highResolution;};
  virtual void SetHighResolution(bool value) {m_highResolution = value;};

  virtual bool IsHighResolution() {return GetHighResolution();};

  virtual bool GetHidden() {return m_hidden;};
  virtual void SetHidden(bool value) {m_hidden = value;};

  virtual bool IsHidden() {return GetHidden();};
  
  virtual bool IsVirtual() const {return m_virtualField;};
  virtual void SetVirtual(bool value) {m_virtualField = value;};

  virtual bool IsDimTransposed() {return m_dimsTransposed;};

  void SetValidMin(double value) { m_validMin = value; };
  double GetValidMin() { return m_validMin; };
  
  void SetValidMax(double value) { m_validMax = value; };
  double GetValidMax() { return m_validMax; };

  virtual void AdjustValidMinMax(double* data, int32_t size);
  virtual void AdjustValidMinMax(double value);

  static void AdjustValidMinMax(double* data, int32_t size, double& min, double& max);
  static void AdjustValidMinMax(double value, double& min, double& max);

  void SetConvertDate(bool value) { m_convertDate = value; };
  bool GetConvertDate() { return m_convertDate; };
  
  CObArray* GetFieldIndexes() { return m_fieldIndexes; };
  void AddFieldIndexes(CFieldIndex* value);
  void AddFieldIndexes(CObArray* vect, bool removeAll = true);

  virtual void DumpFieldDictionary(std::ostream& fOut = std::cout);

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

  void DeleteFieldIndexes();

  bool IsExpandArray() {return GetExpandArray();};
  bool GetExpandArray() {return m_expandArray;};
  void SetExpandArray(bool value) {m_expandArray = value;};

  bool HasEqualDims(CField* field);

  void HandleBratError(const std::string& str = "", int32_t errClass = BRATHL_LOGIC_ERROR);

  virtual uint32_t GetNumHighResolutionMeasure() { return m_numHighResolutionMeasure; };
  virtual void SetNumHighResolutionMeasure(uint32_t value) { m_numHighResolutionMeasure = value; };
  
  bool UnitIsDate() {return m_unitIsDate; };


  bool IsFieldNetCdfCFAttr();
  
  bool IsFieldHasDefaultValue() { return m_fieldHasDefaultValue; };
  void SetFieldHasDefaultValue(bool value) { m_fieldHasDefaultValue = value; };

  virtual void SetOffset(double value) { m_offset = value; };
  virtual void AddOffset(double value) { m_offset += value; };
  double GetOffset() { return m_offset; };

  static bool IsFieldNetCdfCFAttr(CBratObject* ob);
  static CFieldNetCdfCFAttr* GetFieldNetCdfCFAttr(CBratObject* ob, bool withExcept = true);
  static CFieldNetCdfIndexData* GetFieldNetCdfIndexData(CBratObject* ob, bool withExcept = true);

public:
  static const std::string m_BRAT_INDEX_DATA_NAME;
  static const std::string m_BRAT_INDEX_DATA_DESC;

protected :
  void Init();

protected:
  std::string m_key;

  std::string m_name;
  std::string m_description;
  std::string m_unit;
  bool m_unitIsDate;

  CDate m_dateRef;

  bool m_convertDate;


  std::string m_fullName;
  std::string m_recordName;

  // true by default. Have to be set to false is field is in output (asked by user)
  bool m_hidden;

  // true if it's a virtual field (thats doesn't exits in the file - e.g. CFieldIndex object)
  bool m_virtualField;

  
  bool m_dimsTransposed;


  //field index in the the record
  int32_t m_index;

  // If field is 'union', value is 1, otherwise 0
  int32_t m_isUnion;
  
  // Set to true if field is to be removed from data list field (union field case)
  bool m_toBeRemoved;

  coda_type_class m_typeClass;
  coda_special_type m_specialType;
  coda_native_type m_nativeType;

  bool m_metaData;

  int32_t m_nbDims;   /** total number of dimensions */

  long m_dim[MAX_NUM_DIMS];  /** (maximum) dimensions */

  bool m_isFixedSize;

  int32_t m_maxPos;
  int32_t m_currentPos;
  coda_Cursor m_cursor;
  
  bool m_goToAvailableUnionField;

  bool m_highResolution;

  /** Valid min value
  */
  double m_validMin;

  /** Valid max value
  */
  double m_validMax;

  CObArray* m_fieldIndexes;

  bool m_expandArray;

  uint32_t m_numHighResolutionMeasure;

  bool m_fieldHasDefaultValue;
  
  double m_offset;



};                      

//-------------------------------------------------------------
//------------------- CFieldArray class --------------------
//-------------------------------------------------------------


/** 
  Field of type 'array" management classes.

 
 \version 1.0
*/
class CFieldArray : public CField 

{

public:   
  /// Ctor
  CFieldArray();  

  CFieldArray(const std::string& name, const std::string& description = "", const std::string& unit = "");

  CFieldArray(int32_t nbDims, const long dim[], const std::string& name, const std::string& description = "", const std::string& unit = "");
  
  CFieldArray(CFieldArray& f);  
  
  /// Dtor
  virtual ~CFieldArray();

  const CFieldArray& operator= (CFieldArray& f);

  void Set(CFieldArray& f);



  //virtual void ReadParent(CDoubleArray& vect, CFieldRecord* parentField);
  //virtual void ReadParent(CDoubleArray& vect, CObList* parentFieldList);

  virtual void Read(CDoubleArray& vect, bool skip = false) override;
  virtual void Read(double* data, bool skip = false) override;

  virtual int32_t GetVirtualNbDims() override;
  virtual uint32_t GetOffsetDim() override;

  void CreateFieldIndexes(CObArray& vect);

  virtual void PushPos() override;
  virtual void PushPos(int32_t iDim);
  virtual void PopCursor() override;

  virtual CFieldSet* CreateFieldSet(const CField::CListField& listFields) override;

  virtual void DumpFieldDictionary(std::ostream& fOut = std::cout) override;

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr) override;


public:

};                      

//-------------------------------------------------------------
//------------------- CFieldRecord class --------------------
//-------------------------------------------------------------

/** 
  Field of type 'record" management classes.

 
 \version 1.0
*/


class CFieldRecord : public CFieldArray 
{
protected:
  size_t m_nbFields;

public:   
  /// Ctor
  CFieldRecord();  

  CFieldRecord(size_t nbFields, const std::string& name, const std::string& description = "", const std::string& unit = "");
  CFieldRecord(int32_t nbDims, const long dim[], size_t nbFields, const std::string& name, const std::string& description = "", const std::string& unit = "");
  CFieldRecord(CFieldRecord& f);  

  /// Dtor
  virtual ~CFieldRecord();
  
  const CFieldRecord& operator= (CFieldRecord& f);

  void Set(CFieldRecord& f);

  void SetNbFields(size_t value) { m_nbFields = value; };
  size_t GetNbFields() { return m_nbFields; };

  virtual void PushPos();
  virtual void PushPos(int32_t iDim);  
  virtual void PopCursor();

  virtual CFieldSet* CreateFieldSet(const CField::CListField& listFields);

  //virtual void ReadParent(CDoubleArray& vect, CFieldRecord* parentField);
  //virtual void ReadParent(CDoubleArray& vect, CObList* parentFieldList);

  virtual int32_t GetVirtualNbDims();

  virtual void Read(CDoubleArray& vect, bool skip = false);
  virtual void Read(double* data, bool skip = false);

  virtual void DumpFieldDictionary(std::ostream& fOut = std::cout);

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);
};                      

//-------------------------------------------------------------
//------------------- CFieldBasic class --------------------
//-------------------------------------------------------------

/** 
  Field of type 'basic" management classes.

 
 \version 1.0
*/
class CFieldBasic : public CField 

{

public:   
  /// Ctor
  CFieldBasic();  

  CFieldBasic(long length, const std::string& name, const std::string& description, const std::string& unit);

  CFieldBasic(CFieldBasic& f);  

  /// Dtor
  virtual ~CFieldBasic();
  
  const CFieldBasic& operator= (CFieldBasic& f);

  void Set(CFieldBasic& f);

  //void ReadParent(CDoubleArray& vect, CFieldRecord* parentField);
  
  virtual void Read(CDoubleArray& vect, bool skip = false);
  virtual void Read(double* data, bool skip = false);
  virtual void Read(std::string& data, bool skip = false);

  virtual void PushPos();
  virtual void PopCursor();

  virtual CFieldSet* CreateFieldSet(const CField::CListField& listFields);

  virtual void DumpFieldDictionary(std::ostream& fOut = std::cout);

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

public:

  long m_length;

};                      

//-------------------------------------------------------------
//------------------- CFieldIndexData class --------------------
//-------------------------------------------------------------

/** 
  Field of type 'basic" management classes.

 
 \version 1.0
*/
class CFieldIndexData : public CFieldBasic 

{

public:   
  /// Ctor
  CFieldIndexData();  

  CFieldIndexData(const std::string& name, const std::string& description, const std::string& unit = "");

  CFieldIndexData(CFieldIndexData& f);  

  /// Dtor
  virtual ~CFieldIndexData();
  
  const CFieldIndexData& operator= (CFieldIndexData& f);

  void Set(CFieldIndexData& f);
  
  virtual void Read(CDoubleArray& vect, bool skip = false);
  virtual void Read(double* data, bool skip = false);
  virtual void Read(std::string& data, bool skip = false);
  virtual void Read(double& value);
  virtual double Read();

  virtual void PushPos();
  virtual void PopCursor();

  virtual CFieldSet* CreateFieldSet(const CField::CListField& listFields);

  virtual void DumpFieldDictionary(std::ostream& fOut = std::cout);

  double GetValue();

  //void SetAtBeginning(bool value);

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

protected: 
  void Init();



};                      

//-------------------------------------------------------------
//------------------- CFieldNetCdf class --------------------
//-------------------------------------------------------------


/** 
  Field from a NetCdf file management classes.

 
 \version 1.0
*/

class CFieldNetCdf : public CField 

{

public:   
  /// Ctor
  CFieldNetCdf();  

  CFieldNetCdf(const std::string& name,
               const std::string& description = "",
               const std::string& unit = "",
               int32_t netCdfId = NC_GLOBAL,
               int32_t type = NC_NAT,
               const CUIntArray* dimValues = NULL,
               const CStringArray* dimNames = NULL,
               const CIntArray* dimIds = NULL,
               const CDoubleArray* values = NULL);

  
  CFieldNetCdf(CFieldNetCdf& f);  

  /// Dtor
  virtual ~CFieldNetCdf();
  
  const CFieldNetCdf& operator= (CFieldNetCdf& f);

  virtual CBratObject* Clone();
  CFieldNetCdf* CloneThis() { return dynamic_cast<CFieldNetCdf*>(Clone()); };

  void AdjustValidMinMaxFromValues();

  void Set(CFieldNetCdf& f);

  virtual void PushPos();
  virtual void PopCursor();

  bool IsAtBeginning();
  void SetAtBeginning(bool value) { m_atBeginning = value; };

  virtual void InitDimIndexes (uint32_t value);
  virtual void InitDimsIndexToMax();
  virtual void InitDimsIndexToMax(uint32_t index);


  bool NextIndex();
  bool PrevIndex();

  virtual void SetIndex(const std::string& dimName, uint32_t index, uint32_t count);

  uint32_t* GetDimsIndexArray() { return m_dimsIndexArray; };
  uint32_t* GetDimsCountArray() { return m_dimsCountArray; };

  void SetNativeType(int32_t type) { SetType(type); };
  int32_t GetNativeType() { return GetType(); };
  
  virtual std::string GetNativeTypeName() {return GetTypeName();}

  virtual void SetType(int32_t type) { m_type = type; };
  int32_t GetType() { return m_type; };
  virtual std::string GetTypeName();
  
  int32_t GetSpecialType();
  virtual std::string GetSpecialTypeName();
  
  virtual bool IsSpecialType() { return false; };

  virtual int32_t GetVirtualNbDims();

  void SetNetCdfId(int32_t id) { m_netCdfId = id; };
  int32_t GetNetCdfId() { return m_netCdfId; };

  virtual void SetDimValues(const CUIntMap& dimValues);
  virtual void SetDimValues(const CUIntMap* dimValues);
  const CUIntMap& GetDimValues() { return m_dimValues; };
  
  void GetDimValuesAsArray(CUIntArray& values, bool bRemoveAll = true);
  
  virtual void SetDimNames(const CStringArray& dimNames);
  virtual void SetDimNames(const CStringArray* dimNames);
  const CStringArray& GetDimNames() { return m_dimNames; };

  const CUIntMap& GetDimRanges() { return m_dimRanges; };;
  uint32_t GetDimRange(const std::string& dimName);

  virtual void SetDimInfo(const CStringArray& dimNames, const CIntArray& dimIds, const CUIntArray& dimValues);
  virtual void SetDimInfo(const CStringArray* dimNames, const CIntArray* dimIds, const CUIntArray* dimValues);

  virtual void SetValues(double values);
  virtual void SetValues(double* values, size_t length);

  virtual void SetValues(const CDoubleArray& values);
  virtual void SetValues(const CDoubleArray* values);
  
  virtual void SetValues(const CInt16Array& values);
  virtual void SetValues(const CInt16Array* values);
  
  virtual void SetValues(const CInt8Array& values);
  virtual void SetValues(const CInt8Array* values);

  virtual void SetValues(const CIntArray& values);
  virtual void SetValues(const CIntArray* values);
  
  virtual void SetValues(const CUInt8Array& values);
  virtual void SetValues(const CUInt8Array* values);
  
  virtual void SetValues(const CFloatArray& values);
  virtual void SetValues(const CFloatArray* values);

  virtual void SetValues(const std::string& values);

  virtual CDoubleArray& GetValues() { return m_values; };
  virtual CDoubleArray& GetValuesWithUnitConversion(const std::string& wantedUnit);

  void EmptyValues();

  double* GetValuesAsArray() { return m_valuesAsArray; };
  void SetValuesAsArray();
  void SetValuesAsArray(const CDoubleArray& values);
  void SetValuesAsArray(const CDoubleArray* values);


  void SetDimIds(const CIntMap& dimIds);
  void SetDimIds(const CIntMap* dimIds);
  const CIntMap& GetDimIds() { return m_dimIds; };

  void GetDimIdsAsArray(CIntArray& values, bool bRemoveAll = true);

  virtual void SetAttributes(const CStringMap& mapAttributes);
  virtual void SetAttributes(const CStringMap* mapAttributes);
  const CStringMap& GetAttributes() { return m_mapAttributes; };

  virtual std::string GetAttribute(const std::string attrName);

  virtual std::string GetFullName();
  virtual std::string GetRecordName();
  virtual std::string GetFullNameWithRecord();

  NetCDFVarKind SearchDimKind();

  virtual void Read(CDoubleArray& vect, bool skip = false);
  virtual void Read(CExpressionValue& value, bool skip = false);

  virtual CFieldSet* CreateFieldSet(const CField::CListField& listFields);
  //virtual CFieldSet* CreateFieldSet(const CUIntArray& dims);
  virtual CFieldSet* CreateFieldSet();

  virtual void SetUnit(const std::string& unit);
  virtual void SetUnit(const CUnit& unit);
  CUnit* GetNetCdfUnit() { return &m_netCdfUnit; };
  
  void SetScaleFactor(double value) { m_scaleFactor = value;};;
  double GetScaleFactor() { return m_scaleFactor; };
 
  void SetAddOffset(double value) { m_addOffset = value;};;
  double GetAddOffset() { return m_addOffset; };

  void SetFillValue(double value) { m_fillValue = value;};
  double GetFillValue() { return m_fillValue; };

  virtual std::string GetMostExplicitName();

  uint32_t* NewDimIndexArray(CFieldNetCdf* fromField = NULL);

  int32_t GetPosFromDimIndexArray();
  int32_t GetCounFromDimCountArray();

  virtual void DumpFieldDictionary(std::ostream& fOut = std::cout);

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);


protected:

  void DeleteValuesAsArray();

protected:
  /** The netcdf external data types
    */
  int32_t m_type;

  /** The netcdf unit
    */
  CUnit m_netCdfUnit;

  /** data scale factor
    */
  double m_scaleFactor;

  /** data add offset
    */
  double m_addOffset;

  /** data default value (fill value)
    */
  double m_fillValue;

  /** The netcdf external id
    */
  int32_t m_netCdfId;

  /** Array of the dimension's names of the field (index is dim. range)
    */
  CStringArray m_dimNames;

  /** Map of the dimension's values of the field (key is dim. name)
    */
  CUIntMap m_dimValues;

  /** Map of the dimension's ids of the field (key is dim. name)
    */
  CIntMap m_dimIds;
  /** Map of the dimension's range of the field (key is dim. name)
    */
  CUIntMap m_dimRanges;
  
  CDoubleArray m_valuesWithUnitConversion;	// For data as needed but may be empty if only a read has to be done

  CDoubleArray m_values;	// For data as needed but may be empty if only a read has to be done
  // For data as needed but may be NULL (use in the case we don't want use a CDoubleArray object
  // and don't want to create a new array from a CDoubleArray object
  double* m_valuesAsArray;

  //CExternalFiles* m_externalFile;


  /** Map of the netcdf attributes (as std::string representation).
    */
  CStringMap m_mapAttributes;
  

  /** Array of the dimension's index 
    */
  uint32_t* m_dimsIndexArray;

  /** Array of the dimension count for reading
    */
  uint32_t* m_dimsCountArray;


  /** 'At beginning" flag
  */
  bool m_atBeginning;

protected:
  void Init();
  void DeleteDimIndexArray();
  
  //void IsFileOpened();

public:


};                      


//-------------------------------------------------------------
//------------------- CFieldNetCdfCF class --------------------
//-------------------------------------------------------------


/** 
  Field from a NetCdf file management classes.

 
 \version 1.0
*/
class CFieldNetCdfCF : public CFieldNetCdf 

{

public:   
  /// Ctor
  CFieldNetCdfCF();  

  CFieldNetCdfCF(const std::string& name,
                 const std::string& description = "",
                 const std::string& unit = "",
                 int32_t netCdfId = NC_GLOBAL,
                 int32_t type = NC_NAT,
                 const CUIntArray* dimValues = NULL,
                 const CStringArray* dimNames = NULL,
                 const CIntArray* dimIds = NULL,
                 const CDoubleArray* values = NULL);

  CFieldNetCdfCF(CFieldNetCdfCF& f);  

  /// Dtor
  virtual ~CFieldNetCdfCF();
  
  const CFieldNetCdfCF& operator= (CFieldNetCdfCF& f);

  virtual CBratObject* Clone();

  void Set(CFieldNetCdfCF& f);

  virtual std::string GetDimAsString();
  std::string GetDimAsStringWithIndexes();
  std::string GetDimAsStringWithNames();


  virtual void DumpFieldDictionary(std::ostream& fOut = std::cout);

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

protected:
  void Init();

protected:


public:


};                      

//-------------------------------------------------------------
//------------------- CFieldNetCdfIndexData class --------------------
//-------------------------------------------------------------


class CFieldNetCdfIndexData : public CFieldNetCdfCF 

{

public:   
  /// Ctor
  CFieldNetCdfIndexData();  

  CFieldNetCdfIndexData(const std::string& name,
                 const std::string& description = "",
                 const std::string& unit = "",
                 int32_t netCdfId = NC_GLOBAL,
                 int32_t type = NC_NAT,
                 const CUIntArray* dimValues = NULL,
                 const CStringArray* dimNames = NULL,
                 const CIntArray* dimIds = NULL,
                 const CDoubleArray* values = NULL);

  CFieldNetCdfIndexData(CFieldNetCdfIndexData& f);  

  CFieldNetCdfIndexData(CNetCDFVarDef& netCdfVarDef);  

  /// Dtor
  virtual ~CFieldNetCdfIndexData();
  
  const CFieldNetCdfIndexData& operator= (CFieldNetCdfIndexData& f);
  const CFieldNetCdfIndexData& operator= (CNetCDFVarDef& netCdfVarDef);

  bool operator== ( const CFieldNetCdfIndexData& f ) const;

  virtual CBratObject* Clone();

  void Set(CFieldNetCdfIndexData& f);

  void Set(CNetCDFVarDef& netCdfVarDef);

  virtual void SetOffset(double value, bool force = false);
  virtual void AddOffset(double value, bool force = false);

  virtual void DumpFieldDictionary(std::ostream& fOut = std::cout);

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

protected:
  void Init();
  void SetValuesWithOffset(double newOffset, bool force = false);

protected:

public:


};                      

//-------------------------------------------------------------
//------------------- CFieldNetCdfCFAttr class --------------------
//-------------------------------------------------------------


/** 
  Field from a NetCdf Attribute file management classes.

 
 \version 1.0
*/

class CFieldNetCdfCFAttr : public CFieldNetCdfCF 

{

public:   
  /// Ctor
  CFieldNetCdfCFAttr();  


  CFieldNetCdfCFAttr(CNetCDFVarDef* netCDFVarDef, CNetCDFAttr* netCDFAttr);

  CFieldNetCdfCFAttr(CNetCDFAttr* netCDFAttr);

  
  CFieldNetCdfCFAttr(CFieldNetCdfCFAttr& f);  

  /// Dtor
  virtual ~CFieldNetCdfCFAttr();
  
  const CFieldNetCdfCFAttr& operator= (CFieldNetCdfCFAttr& f);

  virtual CBratObject* Clone();
  CFieldNetCdfCFAttr* CloneThis() { return dynamic_cast<CFieldNetCdfCFAttr*>(Clone()); };


  void Set(CFieldNetCdfCFAttr& f);

  void SetInfoFromAttr(CNetCDFVarDef* netCDFVarDef = NULL);
  void SetInfoFromAttr(CNetCDFAttr* netCDFAttr, CNetCDFVarDef* netCDFVarDef = NULL);

  void SetValuesFromAttr();
  void SetValuesFromAttr(CNetCDFAttr* netCDFAttr);


  virtual void SetType(int32_t type) { m_type = type; };

  CNetCDFAttr* GetNetCDFAttr() {return m_netCDFAttr;};
  void SetNetCDFAttr(CNetCDFAttr* value);

  void DeleteNetCDFAttr();

  const std::string& GetRelatedVarName() {return m_relatedVarName;};
  void SetRelatedVarName(const std::string& value) {m_relatedVarName = value;};

  virtual void SetAttributes(const CStringMap& mapAttributes);
  virtual void SetAttributes(const CStringMap* mapAttributes);

  virtual std::string GetMostExplicitName();

  bool IsFieldNetCdfCFAttrGlobal();
  bool IsFieldNetCdfCFAttrVariable();

  static bool IsFieldNetCdfCFAttrGlobal(CBratObject* ob);
  static bool IsFieldNetCdfCFAttrVariable(CBratObject* ob);


  virtual void DumpFieldDictionary(std::ostream& fOut = std::cout);


  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);


protected:

  CNetCDFAttr* m_netCDFAttr;

  std::string m_relatedVarName;


protected:
  void Init();

public:


};                      



//-------------------------------------------------------------
//------------------- CFieldIndex class --------------------
//-------------------------------------------------------------


class CFieldIndex : public CField 

{

public:   
  /// Ctor
  CFieldIndex();  

  CFieldIndex(const std::string& name, int32_t nbElts, const std::string& description = "", const std::string& unit = "");
  
  CFieldIndex(CFieldIndex& f);  
  
  /// Dtor
  virtual ~CFieldIndex();

  const CFieldIndex& operator= (CFieldIndex& f);

  void Set(CFieldIndex& f);

  void SetDim(int32_t nbElts);

  virtual CFieldSet* CreateFieldSet(const CField::CListField& listFields);
  virtual CFieldSet* CreateFieldSet();


  virtual void Read(CDoubleArray& vect, bool skip = false);

  virtual int32_t GetVirtualNbDims() {return GetNbDims();};

  virtual void PushPos();
  virtual void PopCursor();

  //bool IsAtBeginning();
  void SetAtBeginning(bool value);

  virtual void DumpFieldDictionary(std::ostream& fOut = std::cout);

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);


public:

protected:
  void Init();

  void ReadOneValue(CDoubleArray& vect);
  void ReadAllValues(CDoubleArray& vect);

protected:
  
  CUIntArray m_values;

  /** 'At beginning" flag
  */
  bool m_atBeginning;

};                      


//-------------------------------------------------------------
//------------------- CFieldSet class --------------------
//-------------------------------------------------------------

/** 
  a base class for set of field value.

 
 \version 1.0
*/
class CFieldSet : public CBratObject 

{

public:   
  /// Ctor
  CFieldSet(const std::string& name = "");  

  CFieldSet(CFieldSet& f);

  /// Dtor
  virtual ~CFieldSet();

  virtual void Insert(const CDoubleArray& vect, bool bRemove = false) = 0;
  virtual void Insert(double value, bool bRemove = false) = 0;
  virtual void Insert(const std::string& value, bool bRemove = false) = 0;

  const std::string& GetName() { return m_name; };

  virtual CField* GetField() {return m_field;};
  virtual void SetField(CField* value) {m_field = value;};

  CFieldSet& operator=(CFieldSet& o);

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

protected:
  void Copy(CFieldSet& f);

private:
  void Init();

protected:

  std::string m_name;
  CField* m_field;

};                      

//-------------------------------------------------------------
//------------------- CFieldSetString class --------------------
//-------------------------------------------------------------

/** 
  a set of std::string field value.

 
 \version 1.0
*/
class CFieldSetString : public CFieldSet 

{

public:   
  /// Ctor
  CFieldSetString(const std::string& name = "");  
  
  CFieldSetString(CFieldSetString& f);

  /// Dtor
  virtual ~CFieldSetString();

  virtual void Insert(const CDoubleArray& vect, bool bRemove = false);
  virtual void Insert(double value, bool bRemove = false);
  virtual void Insert(const std::string& value, bool bRemove = false);
  

  CFieldSetString& operator=(CFieldSetString& o);

  std::string& GetDataRef() {return m_value;}; 
  std::string GetData() {return m_value;}; 
  void SetData(const std::string& value) {m_value = value;}; 

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

protected:
  void Copy(CFieldSetString& f);

private:
  void Init();

public:

  std::string m_value;

};                      


//-------------------------------------------------------------
//------------------- CFieldSetDbl class --------------------
//-------------------------------------------------------------

/** 
  a set of double field value.

 
 \version 1.0
*/
class CFieldSetDbl : public CFieldSet 

{

public:   
  /// Ctor
  CFieldSetDbl(const std::string& name = "");  
  
  CFieldSetDbl(CFieldSetDbl& f);

  /// Dtor
  virtual ~CFieldSetDbl();

  virtual void Insert(const CDoubleArray& vect, bool bRemove = false);
  virtual void Insert(double value, bool bRemove = false);
  virtual void Insert(const std::string& value, bool bRemove = false);

  int32_t AsInt32() {return static_cast<int32_t>(m_value);};
  int32_t AsUInt32() {return static_cast<uint32_t>(m_value);};

  CFieldSetDbl& operator=(CFieldSetDbl& o);
  //CFieldSetDbl& operator=(uint32_t value);
  //CFieldSetDbl& operator=(int32_t value);

  double& GetDataRef() {return m_value;}; 
  double GetData() {return m_value;}; 
  void SetData(double value) {m_value = value;}; 

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

protected:
  void Copy(CFieldSetDbl& f);

private:
  void Init();

public:

  double m_value;

};                      

//-------------------------------------------------------------
//------------------- CFieldSetArrayDbl class --------------------
//-------------------------------------------------------------

/** 
  a set of double array field value.

 
 \version 1.0
*/
class CFieldSetArrayDbl : public CFieldSet 

{

public:   
  /// Ctor
  CFieldSetArrayDbl(const std::string& name = "");  
  
  CFieldSetArrayDbl(CFieldSetArrayDbl& f);

  /// Dtor
  virtual ~CFieldSetArrayDbl();

  virtual void Insert(const CDoubleArray& vect, bool bRemove = false);
  virtual void Insert(double value, bool bRemove = false);
  virtual void Insert(const std::string& value, bool bRemove = false);

  CDoubleArray& GetDataVector() {return m_vector; };

  CFieldSetArrayDbl& operator=(CFieldSetArrayDbl& o);

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

protected:
  void Copy(CFieldSetArrayDbl& f);

private:
  void Init();

public:

  int32_t m_nbDims;
  CUIntArray m_dim;
  CDoubleArray m_vector;

};                      



//-------------------------------------------------------------
//------------------- CRecordSet class --------------------
//-------------------------------------------------------------

/** 
  a set of record fields value management classes.

 
 \version 1.0
*/
class CRecordSet : public CObMap
{

public:   
  /// Ctor
  CRecordSet(const std::string& name = "", bool bDelete = true);  

  /// Dtor
  virtual ~CRecordSet();

  CFieldSet* ExistsFieldSet(const std::string& key);

  CFieldSet* GetFieldSet(CRecordSet::iterator it);
  CFieldSet* GetFieldSet(const std::string& dataSetName, const std::string& fieldName);

  CField* GetField(CRecordSet::iterator it);

  bool IsFieldHasToBeExpanded(CRecordSet::iterator it, const CStringList& listFieldExpandArray);
  bool IsFieldHasToBeExpanded(CFieldSet* fieldSet, const CStringList& listFieldExpandArray);

  
  void ExecuteExpression(CExpression &expr, const std::string &recordName, CExpressionValue &exprValue, CProduct* product = NULL);


 
  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

public:

  std::string m_name;

};                      

//-------------------------------------------------------------
//------------------- CRecord class --------------------
//-------------------------------------------------------------

/** 
  a set of record  management classes.

 
 \version 1.0
*/
class CRecord : public CBratObject
{

public:   
  /// Ctor
  CRecord(CRecordSet* recordSet = NULL);  

  /// Dtor
  virtual ~CRecord();

  const std::string& GetName();

  CRecordSet* GetRecordSet() {return m_recordSet;};

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

protected:

  CRecordSet* m_recordSet;

};                      



//-------------------------------------------------------------
//------------------- CDataSet class --------------------
//-------------------------------------------------------------

/** 
  a set of recordset  management classes.

 
 \version 1.0
*/
class CDataSet : public CObArray
{

public:   
  /// Ctor
  CDataSet(const std::string& name = "", bool bDelete = true);  

  /// Dtor
  virtual ~CDataSet();

  const std::string& GetName() { return m_name; };
  void SetName(const std::string& name) { m_name = name; };

  CObMap* GetRecordSetMap() { return &m_recordSetMap; };

  CRecordSet* Back(bool withExcept = true);

  CRecordSet* FindRecord(const std::string& recordSetName);

  void InsertDataset(CDataSet* dataSet, bool setAsCurrent = true);
  CRecordSet* InsertRecord(const std::string& recordSetName, bool setAsCurrent = true);
  
  void SetCurrentRecordSet(int32_t index);
  void SetCurrentRecordSet(CDataSet::iterator itDataSet);
  void SetCurrentRecordSet(const std::string& recordSetName); 
  void SetCurrentRecordSet(CRecordSet* recordSet); 

  CRecordSet* GetCurrentRecordSet() {return m_currentRecordSet;}; 

  CRecordSet* GetFirstRecordSet();

  CRecordSet* GetRecordSet(CDataSet::iterator itDataSet);
  CRecordSet* GetRecordSet(int32_t index);

  CRecord* GetRecord(const std::string& recordSetName);
  CRecord* GetRecord(CRecordSet* recordSet);

  /** Gets the fieldset object (identify by its name) of the current recordset
    \param fieldSetKey [in] : fieldset key to be searched
    \return a pointer to the fieldset object if found, otherwise NULL */
  CFieldSet* GetFieldSet(const std::string& fieldSetKey);

  CFieldSetArrayDbl* GetFieldSetAsArrayDbl(const std::string& fieldSetKey);
  CFieldSetDbl* GetFieldSetAsDbl(const std::string& fieldSetKey);
  CFieldSetString* GetFieldSetAsString(const std::string& fieldSetKey);

  double GetFieldSetAsDblValue(const std::string& fieldSetKey);
  std::string GetFieldSetAsStringValue(const std::string& fieldSetKey);

  /** Inserts a fieldset object (identify by its name)  into the current recordset
    \param fieldSetKey [in] : fieldset key 
    \param fieldSet [in] : fieldset object to be inserted*/
  void InsertFieldSet(const std::string& fieldSetKey, CFieldSet* fieldSet);

  /** remove a fieldset object (identify by its name)  from the current recordset
    \param fieldSetKey [in] : fieldset key */
  void EraseFieldSet(const std::string& fieldSetKey);

  virtual void RemoveAll();
  virtual bool Erase(CRecordSet* recordSet);
  bool EraseCurrentRecordSet();

  /** Copy a new CDataSet to the object */
  //virtual const CDataSet& operator= (const CDataSet& lst);

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

protected:
  
  std::string m_name;

  CRecordSet* m_currentRecordSet;

  CObMap m_recordSetMap;


};                      








/** @} */

}
#endif // !defined(_Field_h_)
