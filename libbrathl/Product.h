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
#if !defined(_Product_h_)
#define _Product_h_

#include "new-gui/Common/tools/brathl_error.h" 
#include "brathl.h" 

#include "coda.h" 

#include <string>

#include "List.h"
#include "BratObject.h"
#include "TreeField.h"
#include "Date.h" 
#include "CriteriaInfo.h" 
#include "Criteria.h" 
#include "CriteriaLatLon.h" 
#include "CriteriaDatetime.h" 
#include "CriteriaPass.h" 
#include "CriteriaCycle.h" 
#include "File.h" 

#include "ExternalFiles.h"
#include "AliasesDictionary.h"

using namespace brathl;

namespace brathl
{

//-------------------------------------------------------------
//------------------- CProductList class --------------------
//-------------------------------------------------------------


/** \addtogroup product Products
  @{ */

/** 
  Product file list management class.


 \version 1.0
*/


class CProductList : public CStringList
{

public:
    
  /// Empty CProductList ctor
  CProductList();

  /** Creates new CProductList object from another one
    \param p [in] : productList object to be connected */
  CProductList(const CProductList& p);

  /** Creates new CProductList object
    \param fileName [in] : file name to be connected */
  CProductList(const std::string& fileName);
  
  /** Creates new CProduct object
    \param fileNameList [in] : list of file to be connected */
  CProductList(const CStringList& fileNameList);

  /** Creates new CProduct object
    \param fileNameArray [in] : array of file to be connected */
  CProductList(const CStringArray& fileNameArray);

  /** Creates new CProductList object from another one
    \param p [in] : productList object to be connected */
  void Set(const CProductList& lst);

  const std::string GetMessage() {return m_message;}

  /// Destructor
  virtual ~CProductList();

  bool CheckFiles(bool onlyFirstFile = false);
  bool CheckFilesNetCdf();
  
  bool IsHdfOrNetcdfCodaFormat();
  static bool IsHdfOrNetcdfCodaFormat(coda_format format);
  
  bool IsYFX();
  bool IsZFXY();

  bool IsGenericNetCdf();
  
  bool IsJason2();
  bool IsATP();

  bool IsNetCdfProduct() {return (m_productClass.compare(NETCDF_PRODUCT_CLASS) == 0); }
  bool IsNetCdfCFProduct() {return (m_productClass.compare(NETCDF_CF_PRODUCT_CLASS) == 0); }
  bool IsNetCdfOrNetCdfCFProduct() {return (IsNetCdfCFProduct() || IsNetCdfProduct()); }

  bool IsSameProduct(const std::string& productClass, const std::string& productType);

  const CProductList& operator= (const CProductList& lst);

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);


protected:

  bool CheckFileList();

public:

  std::string m_productClass;		
  std::string m_productType;
  coda_format m_productFormat;  
  
  std::string m_message;

  


};



//-------------------------------------------------------------
//------------------- CProduct class --------------------
//-------------------------------------------------------------


/** \addtogroup product Products
  @{ */

/** 
  Product management class.


 \version 1.0
*/




const long DEFAULT_DIM[] = {1};

class CProduct : public CBratObject
{

public:
    
  /// Empty CProduct ctor
  CProduct();

  
  /** Creates new CProduct object
    \param fileName [in] : file name to be connected */
  CProduct(const std::string& fileName);
  
  /** Creates new CProduct object
    \param fileNameList [in] : list of file to be connected */
  CProduct(const CStringList& fileNameList);
  
  /// Destructor
  virtual ~CProduct();

  virtual bool GetDateMinMax(CDatePeriod& datePeriodMinMax);
  virtual bool GetDateMinMax(CDate& dateMin, CDate& dateMax);

  virtual bool GetLatLonMinMax(double& latMin, double& lonMin, double& latMax, double& lonMax);
  virtual bool GetLatLonMinMax(CLatLonRect& latlonRectMinMax);

  virtual bool GetValueMinMax(CExpression& expr, const std::string& recordName, 
                              double& valueMin, double& valueMax, const CUnit& unit);

  void AddCriteria(bool force = false);
  void AddCriteria(CCriteria* criteria, bool erase = true);
  void AddCriteria(CProduct* product);

  virtual void ApplyCriteria(CStringList& filteredFileList, const std::string& logFileName = "");

  virtual bool ApplyCriteriaLatLon(CCriteriaInfo* criteriaInfo);
  virtual bool ApplyCriteriaDatetime(CCriteriaInfo* criteriaInfo);
  virtual bool ApplyCriteriaPass(CCriteriaInfo* criteriaInfo);
  virtual bool ApplyCriteriaPassInt(CCriteriaInfo* criteriaInfo);
  virtual bool ApplyCriteriaPassString(CCriteriaInfo* criteriaInfo);
  virtual bool ApplyCriteriaCycle(CCriteriaInfo* criteriaInfo);

  CCriteria* GetCriteria(CCriteriaInfo* criteriaInfo);

  void RemoveCriteria();

  void AddFile(const std::string& fileName, bool bEnd = true, bool checkFiles = true);
  void AddFile(const CStringList& fileNameList, bool bEnd = true, bool checkFiles = true);
  
  bool CheckFiles();

  void SetPerformConversions(bool performConversions);
  int32_t GetPerformConversions() {return m_performConversions;}
  void SetPerformBoundaryChecks(bool performBoundaryChecks);
  int32_t GetPerformBoundaryChecks() {return m_performBoundaryChecks;}
  

  std::string GetProductClass() {return m_fileList.m_productClass;}
  std::string GetProductType() {return m_fileList.m_productType;}

  bool IsNetCdfProduct() {return (m_fileList.m_productClass.compare(NETCDF_PRODUCT_CLASS) == 0); }
  bool IsNetCdfCFProduct() {return (m_fileList.m_productClass.compare(NETCDF_CF_PRODUCT_CLASS) == 0); }
  bool IsNetCdfOrNetCdfCFProduct() {return (IsNetCdfCFProduct() || IsNetCdfProduct()); }

  std::string GetProductClassType();

  virtual void GetRecords(CStringArray& array);

  virtual bool IsOpened();
  virtual void CheckFileOpened();
  virtual bool IsOpened(const std::string& fileName);
  
  void SetListFieldToRead(CStringList& listFieldToRead,  bool convertDate = false);

  virtual bool Open(const std::string& fileName, const std::string& dataSetName, CStringList& listFieldToRead, bool convertDate = false);
  virtual bool Open(const std::string& fileName, const std::string& dataSetName);
  virtual bool Open(const std::string& fileName);

  virtual void Rewind ();

  virtual bool Close();

  //virtual void ReadBratFile(const std::string& fileName, const std::string& dataSetName, const std::string& field);
  //virtual void ReadBratFile(const std::string& fileName, const std::string& dataSetName, CStringList& listField);
  
  virtual void ReadBratRecord(const std::string& dataSetName, const std::string& field, int32_t iRecord);
  virtual void ReadBratRecord(const std::string& dataSetName, CStringList& listField, int32_t iRecord);
  virtual void ReadBratRecord(int32_t iRecord);

  virtual int32_t GetNumberOfRecords();
  virtual int32_t GetNumberOfRecords(const std::string& dataSetName);
  virtual void GetNumberOfRecords(const CStringList& datasetNames, CIntMap& datasetRecordsNumber);

  virtual void GetMinMaxNumberOfRecords(int32_t& min, int32_t& max, 
                                        CIntMap* datasetRecordsNumber = NULL, int32_t minThreshold =  -1);
  
  virtual void ExtractDatasetNamesFromFields(const CStringList& listFields, CStringList& datasetNames);

  std::string DatasetRecordsNumberToString(const CIntMap& datasetRecordsNumber);

  bool HasEqualsNumberOfRecord(const CIntMap& datasetRecordsNumber);

  //virtual CField* GetFieldRead(const std::string& fieldName);

  //virtual double GetDeltaTHighResolution() {return m_deltaTHighResolution;};
  //virtual void SetDeltaTHighResolution(double value) {m_deltaTHighResolution = value;};

  /** Determines if a field object is a 'high resolution' array data
      see classes derived from CProduct.
  */
  virtual bool IsHighResolutionField(CField *) { return false; }
  virtual void SetHighResolution(CField* field);


  CDataSet* GetDataSet() {return &m_dataSet;}

  brathl_refDate GetRefDate() {return m_refDate;}
  CDate GetRefDateAsDate() {return CDate(0.0, m_refDate);}

  CTreeField* GetTreeField() {return &m_tree;}

  uint32_t GetSkippedRecordCount() {return m_nSkippedRecord;}

  bool GetExpandArray() {return m_expandArray;}
  void SetExpandArray(bool value) {m_expandArray = value;}
  
  bool GetCreateVirtualField() {return m_createVirtualField;}
  void SetCreateVirtualField(bool value) {m_createVirtualField = value;}

  
  static CProduct* Construct(CStringArray& fileNameArray);
  static CProduct* Construct(CStringList& fileNameList);
  static CProduct* Construct(CProductList& fileNameList);
  static CProduct* Construct(const std::string& fileName);

  bool IsNetCdf();

  const std::string& GetDescription() {return m_description;}
  void SetDescription(const std::string& value) {m_description = value;}

  static void CodaInit();
  static void CodaRelease();

  //static void SetCodaReleaseWhenDestroy(bool value) {m_codaReleaseWhenDestroy = value;};

  virtual CProduct* Clone();

  void DumpDictionary(std::ostream& fOut = std::cout);
  void DumpDictionary(const std::string& outputFileName);

  virtual bool HasCriteriaInfo() { return (m_criteriaInfoMap.size() > 0); }

  bool HasLatLonCriteriaInfo() { return (GetLatLonCriteriaInfo() != NULL); }
  bool HasDatetimeCriteriaInfo() { return (GetDatetimeCriteriaInfo() != NULL); }
  bool HasPassCriteriaInfo() { return (GetPassCriteriaInfo() != NULL); }
  bool HasPassIntCriteriaInfo() { return (GetPassIntCriteriaInfo() != NULL); }
  bool HasPassStringCriteriaInfo() { return (GetPassStringCriteriaInfo() != NULL); }
  bool HasCycleCriteriaInfo() { return (GetCycleCriteriaInfo() != NULL); }
  
  CCriteriaLatLonInfo* GetLatLonCriteriaInfo() { return CCriteriaLatLonInfo::GetCriteriaInfo(m_criteriaInfoMap.Exists(CCriteria::LATLON), false); }
  CCriteriaDatetimeInfo* GetDatetimeCriteriaInfo() { return CCriteriaDatetimeInfo::GetCriteriaInfo(m_criteriaInfoMap.Exists(CCriteria::DATETIME), false); }
  CCriteriaPassInfo* GetPassCriteriaInfo() { return CCriteriaPassInfo::GetCriteriaInfo(m_criteriaInfoMap.Exists(CCriteria::PASS), false); }
  CCriteriaPassIntInfo* GetPassIntCriteriaInfo() { return CCriteriaPassIntInfo::GetCriteriaInfo(m_criteriaInfoMap.Exists(CCriteria::PASS), false); }
  CCriteriaPassStringInfo* GetPassStringCriteriaInfo() { return CCriteriaPassStringInfo::GetCriteriaInfo(m_criteriaInfoMap.Exists(CCriteria::PASS), false); }
  CCriteriaCycleInfo* GetCycleCriteriaInfo() { return CCriteriaCycleInfo::GetCriteriaInfo(m_criteriaInfoMap.Exists(CCriteria::CYCLE), false); }


  bool HasLatLonCriteria() { return (GetLatLonCriteria() != NULL); }
  bool HasDatetimeCriteria() { return (GetDatetimeCriteria() != NULL); }
  bool HasPassCriteria() { return (GetPassCriteria() != NULL); }
  bool HasPassIntCriteria() { return (GetPassIntCriteria() != NULL); }
  bool HasPassStringCriteria() { return (GetPassStringCriteria() != NULL); }
  bool HasCycleCriteria() { return (GetCycleCriteria() != NULL); }

  bool IsSetLatLonCriteria() { return (HasLatLonCriteria() ? !GetLatLonCriteria()->IsDefaultValue() : false); }
  bool IsSetDatetimeCriteria() { return (HasDatetimeCriteria() ? !GetDatetimeCriteria()->IsDefaultValue() : false); }
  bool IsSetPassCriteria() { return (HasPassCriteria() ? !GetPassCriteria()->IsDefaultValue() : false); }
  bool IsSetPassIntCriteria() { return (HasPassIntCriteria() ? !GetPassIntCriteria()->IsDefaultValue() : false); }
  bool IsSetPassStringCriteria() { return (HasPassStringCriteria() ? !GetPassStringCriteria()->IsDefaultValue() : false); }
  bool IsSetCycleCriteria() { return (HasCycleCriteria() ? !GetCycleCriteria()->IsDefaultValue() : false); }

  CCriteriaLatLon* GetLatLonCriteria() { return CCriteriaLatLon::GetCriteria(m_criteriaMap.Exists(CCriteria::LATLON), false); }
  CCriteriaDatetime* GetDatetimeCriteria() { return CCriteriaDatetime::GetCriteria(m_criteriaMap.Exists(CCriteria::DATETIME), false); }
  CCriteriaPass* GetPassCriteria() { return CCriteriaPass::GetCriteria(m_criteriaMap.Exists(CCriteria::PASS), false); }
  CCriteriaPassInt* GetPassIntCriteria() { return CCriteriaPassInt::GetCriteria(m_criteriaMap.Exists(CCriteria::PASS), false); }
  CCriteriaPassString* GetPassStringCriteria() { return CCriteriaPassString::GetCriteria(m_criteriaMap.Exists(CCriteria::PASS), false); }
  CCriteriaCycle* GetCycleCriteria() { return CCriteriaCycle::GetCriteria(m_criteriaMap.Exists(CCriteria::CYCLE), false); }


  CProductList& GetProductList() { return m_fileList; }
  void SetProductList(const std::string& fileName, bool checkFiles = true);
  void SetProductList(const CStringList& fileList, bool checkFiles = true);

  bool IsSameProduct(const CProductList fileList);
  bool IsSameProduct(const std::string& productClass, const std::string& productType);

  virtual std::string GetLongitudeFieldName() { return m_longitudeFieldName; }
  virtual std::string GetLatitudeFieldName() { return m_latitudeFieldName; }
  
  virtual bool IsLongitudeFieldName(const std::string& name) { return (m_longitudeFieldName.compare(name) == 0); }
  virtual bool IsLatitudeFieldName(const std::string& name) { return (m_latitudeFieldName.compare(name) == 0); }

  virtual std::string GetLabel() { return m_label; }
  virtual void SetLabel(const std::string& value) { m_label = value; }

  std::string GetDataSetNameToRead() { return m_dataSetNameToRead; }
  void SetDataSetNameToRead(const std::string& value) { m_dataSetNameToRead = value; }

  virtual void InitCriteriaInfo();
  virtual void LoadAliases();
  virtual bool HasAliases() {return (m_productAliases != NULL);}
  
  static void GroupAliases(const CProduct* product, const CStringMap* formulaAliases, CStringMap& allAliases);

  static bool CheckAliases(const std::string& fileName, CStringArray& errors);
  bool CheckAliases(CStringArray& errors);

  const CProductAliases* GetAliases() {return m_productAliases;}
  const CStringMap* GetAliasesAsString() const {return &m_mapStringAliases;}
  static const CStringMap* GetAliasesAsString(const CProduct* product);

  const CProductAlias* GetAlias(const std::string& key);
  void GetAliasKeys(CStringArray& keys);
  std::string GetAliasExpandedValue(const std::string& key);

  int32_t GetIndexProcessedFile() { return m_indexProcessedFile; }

  bool CheckFieldNames(const CExpression& expr, const std::string& dataSetName, CStringArray& fieldNamesNotFound);
  bool CheckFieldNames(const CExpression& expr, CStringArray& fieldNamesNotFound);
  bool CheckFieldNames(const CStringArray* fieldNames, const std::string& dataSetName, CStringArray& fieldNamesNotFound);
  //bool CheckFieldNames(const CStringArray* fieldNames, CStringArray& fieldNamesNotFound);

  bool HasEqualDims(const std::string& value, std::string& msg);
  bool HasEqualDims(const std::string& value, const std::string& dataSetName, std::string& msg);
  bool HasEqualDims(const CExpression& expr, std::string& msg);
  bool HasEqualDims(const CExpression& expr, const std::string& dataSetName, std::string& msg);
  bool HasEqualDims(const CStringArray* fieldNames, std::string& msg);
  bool HasEqualDims(const CStringArray* fieldNames, const std::string& dataSetName,  std::string& msg);
  
  virtual bool HasCompatibleDims(const std::string& value, std::string& msg, bool useVirtualDims, CUIntArray* commonDimensions = NULL);
  virtual bool HasCompatibleDims(const std::string& value, const std::string& dataSetName, std::string& msg, bool useVirtualDims, CUIntArray* commonDimensions = NULL);
  virtual bool HasCompatibleDims(const CExpression& expr, std::string& msg, bool useVirtualDims, CUIntArray* commonDimensions = NULL);
  virtual bool HasCompatibleDims(const CExpression& expr, const std::string& dataSetName, std::string& msg, bool useVirtualDims, CUIntArray* commonDimensions = NULL);
  virtual bool HasCompatibleDims(const CStringArray* fieldNames, std::string& msg, bool useVirtualDims, CUIntArray* commonDimensions = NULL);
  virtual bool HasCompatibleDims(const CStringArray* fieldNames, const std::string& dataSetName, std::string& msg, bool useVirtualDims, CUIntArray* commonDimensions = NULL);


  CField* FindFieldByName(const std::string& fieldName, const std::string& dataSetName, bool withExcept = true, std::string* errorMsg = NULL, bool showTrace = true);
  CField* FindFieldByName(const std::string& fieldName, bool withExcept = true, std::string* errorMsg = NULL, bool showTrace = true);
  CField* FindFieldByInternalName(const std::string& internalFieldName, bool withExcept = true);


  CStringArray* GetFieldToTranspose() { return &m_fieldsToTranspose; }

  bool LoadTransposeFieldsValue(CStringArray& fieldsToTranspose);

  virtual void SetForceReadDataOneByOne(bool) {  }
  virtual bool GetForceReadDataOneByOne() { return false; }

  void ReplaceNamesCaseSensitive(const CExpression& exprIn, const CStringArray& fieldsIn, CExpression& exprOut, bool forceReload = false);
  void ReplaceNamesCaseSensitive(const std::string& in, const CStringArray& fieldsIn, std::string& out, bool forceReload = false);
  void ReplaceNamesCaseSensitive(const std::string& in, std::string& out, bool forceReload = false);
  void ReplaceNamesCaseSensitive(const CExpression& exprIn, std::string& out, bool forceReload = false);


  void GetNamesCaseSensitive(const CStringArray& fieldsIn, CStringArray& fieldsOutNoCaseSensitive, CStringArray& fieldsOutCaseSensitive, bool forceReload = false);

  
  bool AddRecordNameToField(const CExpression& expr, const std::string& dataSetName, CExpression& exprOut, std::string& errorMsg);
  bool AddRecordNameToField(const std::string& in, const std::string& dataSetName, std::string& out, std::string& errorMsg);
  bool AddRecordNameToField(const std::string& in, const std::string& dataSetName, const CStringArray& fieldsIn, std::string& out, std::string& errorMsg);
  bool AddRecordNameToField(CProductAliases* productAliases, std::string& errorMsg);

  CStringArray* GetDataDictionaryFieldNames(bool forceReload = false);
  CStringArray* GetDataDictionaryFieldNamesWithDatasetName(bool forceReload = false);

  double GetForceLatMinCriteriaValue() { return m_forceLatMinCriteriaValue; }
  double GetForceLatMaxCriteriaValue() { return m_forceLatMaxCriteriaValue; }

  CStringMap* GetFieldSpecificUnits() { return &m_fieldSpecificUnit; }
  void SetFieldSpecificUnits(const CStringMap& fieldSpecificUnit);
  
  std::string GetFieldSpecificUnit(const std::string& key);
  void SetFieldSpecificUnit(const std::string& key, const std::string& value);

  virtual void SetFieldSpecificUnit(CField* field);
  
  virtual int32_t GetCurrentRecordNumber() { return m_currentRecord; }
  virtual std::string GetCurrentFileName() { return m_currFileName; }

  void SetDisableTrace(bool value) { m_disableTrace = value; }
  bool GetDisableTrace() { return m_disableTrace; }

  CStringList* GetListFieldOrigin(){ return &m_listFieldOrigin; }
  void SetListFieldOrigin(const CStringList& listFieldOrigin);
  
  virtual void SetOffset(double value) { m_offset = value; }
  virtual void AddOffset(double value, CField* field = NULL);
  virtual double GetOffset() { return m_offset; }


  static int32_t ReadData
		(int32_t	nbFiles,
		 char		**fileNames,
		 const char	*recordName,
		 const char	*selection,
		 int32_t	nbData,
		 char		**dataExpressions,
		 char		**units,
		 double		**results,
		 int32_t	sizes[],
         size_t	*actualSize,
		 int		ignoreOutOfRange,
		 int		statistics,
		 double		defaultValue,
     CStringMap* fieldSpecificUnit = NULL
		 );

  static void ReadDataForOneMeasure
		(CDataSet			*dataSet,
		 const std::string			&recordName,
		 CExpression			&Select,
		 std::vector<CExpression>		&Expressions,
		 const std::vector<CUnit>		&WantedUnits,
		 double				**results,
		 int32_t			*sizes,
         size_t			*actualSize,
		 int				ignoreOutOfRange,
		 int				statistics,
     CProduct* product = NULL);


  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

public:

//-------------------------------------------------------------
//------------------- CInfo class --------------------
//-------------------------------------------------------------

  /** 
    A class to traverse Brat files

   \version 1.0
  */
  class CInfo : public CBratObject
  {
  public:
    CInfo();
   virtual ~CInfo();
    coda_Type * m_type;
    coda_type_class m_type_class;
    int32_t m_isUnion;
    std::string m_fieldName;

    int32_t m_index;
   };

//---------------------- End CInfo class --------------------

//-------------------------------------------------------------
//------------------- CListInfo class --------------------
//-------------------------------------------------------------

  /** 
    A list of CInfo object management class

   \version 1.0
  */
  class CListInfo : public CObList
  {
  public:
    CListInfo() {}
   virtual ~CListInfo() {}
   CInfo* AddNew();
   CInfo* Back(bool withExcept = true);
   CInfo* Front(bool withExcept = true);
   CInfo* PrevBack(bool withExcept = true);

   };
//---------------------- End CListInfo class --------------------
protected:

protected:

  virtual bool Open();

  virtual void RewindInit ();
  virtual void RewindProcess ();
  virtual void RewindEnd ();

  virtual bool HasHighResolutionFieldCalculation() {return false;}
  virtual void AddInternalHighResolutionFieldCalculation() {}

  virtual void AddSameFieldName(const std::string& fieldNameToSearch, CStringArray& arrayFieldsAdded);

  virtual void CheckConsistencyHighResolutionField(CFieldSetArrayDbl* fieldSetArrayDbl);


  void CreateFieldIndexes(CFieldArray* field);
  void CreateFieldIndexData();

  void CheckFields(bool convertDate = false);

  bool GetInfoRecord(int32_t nbDims = 1, const long dim[] = DEFAULT_DIM);
  bool GetInfoArray();
  bool GetInfoSpecial(int32_t nbDims = 1, const long dim[] = DEFAULT_DIM);

  bool TraverseData();
  bool TraverseRecord(int32_t indexFields);

  void GetRootType();


  void HandleBratError(const std::string& str = "", int32_t errClass = BRATHL_LOGIC_ERROR);


  std::string GetTypeName();
  std::string GetTypeDesc();
  std::string GetTypeDesc(coda_Type *type);
  std::string GetTypeUnit();
  std::string GetRecordFieldName();

  void SetTypeClass(CField* field);
  void SetNativeType(CField* field);
  void SetSpecialType(CField* field);
  void SetIndex(CField* field);
  void SetUnion(CField* field);

  virtual void InitDateRef() = 0;


  virtual void LoadFieldsInfo();
  void SetDynInfo();
  void RemoveUnusedFields();

  void SetCursor(CField* field, bool& skipRecord);

  //virtual void ReadBratField(const std::string& key);
  //virtual void ReadBratField(CField::CListField::iterator it);

  virtual void ReadBratFieldRecord(const std::string& key, int32_t iRecord);
  virtual void ReadBratFieldRecord(CField::CListField::iterator it);
  virtual void ReadBratFieldRecord(CField::CListField::iterator it, bool& skipRecord);

  virtual bool FindParentToRead(CField* fromField, CObList* parentFieldList);

  void FillListFields(const std::string& key);

  virtual std::string MakeInternalNameByAddingRoot(const std::string& name);

  virtual std::string MakeInternalFieldName(const std::string& dataSetName, const std::string& field);

  virtual std::string MakeInternalFieldName(const std::string& field);
  
  virtual std::string MakeInternalDataSetName(const std::string& dataSetName);

  virtual void InitInternalFieldName(const std::string& dataSetName, CStringList& listField, bool convertDate = false);
  virtual void InitInternalFieldName(CStringList& listField, bool convertDate = false);
  virtual void InitInternalFieldName(const std::string& field, bool convertDate = false);

  void ProcessHighResolution();
  void ExpandArray();
  void ExpandFieldsArray();
  virtual void ProcessHighResolutionWithFieldCalculation();
  virtual void ProcessHighResolutionWithoutFieldCalculation();

  virtual void PutFlat(CDataSet* dataSet, CFieldSetArrayDbl* fieldSetArrayDbl, uint32_t insertRecordAt = 0);
  
  virtual void PutFlatHighResolution(CDataSet* dataSet, CFieldSetArrayDbl* fieldSetArrayDbl);
  
  virtual void Put(CDataSet* dataSet, CFieldSetDbl* fieldSetDbl, uint32_t repeat, uint32_t insertRecordAt = 0);
  virtual void Put(CDataSet* dataSet, CFieldSetArrayDbl* fieldSetArrayDbl, uint32_t repeat, uint32_t insertRecordAt = 0);
  virtual void Put(CDataSet* dataSet, CFieldSetDbl* fieldSetDbl);

  void InsertRecord(int32_t pos);
  void InsertRecord(CDataSet& dataSet, int32_t pos);

  void ConvertDate(CDoubleArray& vect);
  
  virtual void FillDescription();

  void BuildCriteriaFieldsToRead(CRecordDataMap& listRecord);

  void CreateLogFile(const std::string& logFileName, uint32_t mode = CFile::modeWrite|CFile::typeText);
  void DeleteLogFile();

  template< typename T >
  void Log( const T n, bool bCrLf )
  {
	  Log(n2s<std::string>(n).c_str(), bCrLf);
  }

  void Log( const char *str, bool bCrLf )
  {
	  if ( !m_logFile || !m_logFile->IsOpen() )
		  return;

	  try
	  {
		  m_logFile->Write( str );
		  if ( bCrLf )
			  m_logFile->Write( '\n' );
	  }
	  catch ( ... )
	  {}
  }

  void Log( const std::string& str, bool bCrLf )
  {
	  Log( str.c_str(), bCrLf );
  }

  void Log( const bool n, bool bCrLf )
  {
	  std::string str = ( n ? "yes" : "no" );
	  Log( str, bCrLf );
  }

  // template<>
  void Log( const CStringList& l, bool bCrLf )
  {
	  for ( auto it = l.begin(); it != l.end(); it++ )
		  Log( it->c_str(), bCrLf );
  }


  void LogSelectionResult(const std::string& fileName, bool result);

  virtual void InitApplyCriteriaStats();
  virtual void EndApplyCriteriaStats(const CStringList& filteredFileList);

private:

  void Init();
  void Release();
  void DeleteProductAliases();

  void InitBratOptions();

public:
    
  static coda_array_ordering  m_arrayOrdering;

  static const std::string m_treeRootName;

  static const char* m_transposeFieldValuesFileName;

  static const int32_t NUMBER_OF_STATISTICS;
  static const uint32_t COUNT_INDEX;
  static const uint32_t MEAN_INDEX;
  static const uint32_t STDDEV_INDEX;
  static const uint32_t MIN_INDEX;
  static const uint32_t MAX_INDEX;


protected:

  double m_forceLatMinCriteriaValue;
  double m_forceLatMaxCriteriaValue;

  std::string m_label;

  std::string m_latitudeFieldName;
  std::string m_longitudeFieldName;

  CStringArray m_arrayLongitudeFieldName;
  CStringArray m_arrayLatitudeFieldName;

  CStringArray m_dataDictionaryFieldNames;
  CStringArray m_dataDictionaryFieldNamesWithDatasetName;

  CTreeField m_tree;
		
  CDataSet m_dataSet;

  CProductList m_fileList;
  
  CListInfo m_listInfo;

  coda_ProductFile* m_currFile;	
  coda_Cursor m_cursor;

  std::string m_currFileName;		

  brathl_refDate m_refDate;

  CStringList m_fieldsToProcess;
  CField::CListField m_listFields;


  size_t m_recordCount;
  int32_t m_currentRecord;

  
  bool m_hasHighResolutionFieldToProcess;

  // delta time in seconds
  double m_deltaTimeHighResolution;
  // Reference point (index) of high resolution measures array
  int32_t m_refPoint;

  uint32_t m_numHighResolutionMeasure;


  // Field names --> internal field names equivalence
  CStringMap m_fieldNameEquivalence;

  std::string m_dataSetNameToRead;
  CStringList m_listInternalFieldName;
  CStringList m_listFieldOrigin;

  // A list fields whose array have to be expanded.
  CStringList m_listFieldExpandArray;
  
  double m_previousLatitude;
  double m_previousLongitude;
  double m_previousTimeStamp;  

  uint32_t m_nSkippedRecord;

  bool m_expandArray;

  bool m_createVirtualField;

  //static bool m_codaReleaseWhenDestroy;


  uint32_t m_countForTrace;
  uint32_t m_traceProcessRecordRatio;

  /** Number of records to read
  */
  int32_t m_nbRecords;

  std::string m_description;

  CObIntMap m_criteriaInfoMap;

  CObIntMap m_criteriaMap;

  CFile* m_logFile;
  CDate m_dateProcessBegin;
  CDate m_dateProcessEnd;
  int32_t m_indexProcessedFile;

  CStringArray m_fieldsToTranspose;

  CStringMap m_fieldSpecificUnit;

  bool m_fieldsHaveDefaultValue;

  bool m_disableTrace;

  CProductAliases* m_productAliases;
  CStringMap m_mapStringAliases;

  double m_offset;

private:
  
  static int32_t m_codaRefCount;

  int32_t m_performConversions;
  int32_t m_performBoundaryChecks;

};

//-------------------------------------------------------------
class CProductGeneric : public CProduct
{

public:
    
  /// Empty CProductGeneric ctor
  CProductGeneric();

  
  /** Creates new CProdCProductGenericuct object
    \param fileName [in] : file name to be connected */
  CProductGeneric(const std::string& fileName) : CProduct(fileName) {}
  
  /** Creates new CProductGeneric object
    \param fileNameList [in] : list of file to be connected */
  CProductGeneric(const CStringList& fileNameList) : CProduct(fileNameList) {}
  
  /// Destructor
  virtual ~CProductGeneric() { }
  //----------------------------------------
  virtual void InitDateRef() { m_refDate = REF20000101; }

};
//-------------------------------------------------------------
//------------------- CMapProduct class --------------------
//-------------------------------------------------------------

/** \addtogroup product Products
  @{ */

/** 
  Mapping products management class.


 \version 1.0
*/



class CMapProduct: public CObMap
{
public:
  /// CIntMap ctor
  CMapProduct();

  /// CIntMap dtor
  virtual ~CMapProduct();

  static CMapProduct&  GetInstance();

  void GetProductKeysWithCriteria(CStringArray& keys);

  void AddCriteriaToProducts();
  void RemoveCriteriaFromProducts();

  virtual void Dump(std::ostream& fOut  = std::cerr); 



protected:



protected:

  void Init();



};

/** @} */

}

#endif // !defined(_Product_h_)
