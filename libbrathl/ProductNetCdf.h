/*
* Copyright (C) 2005 Collecte Localisation Satellites (CLS), France (31)
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
#if !defined(_ProductNetCdf_h_)
#define _ProductNetCdf_h_

#include "Product.h"

#include "ExternalFiles.h"
#include "ExternalFilesNetCDF.h"
#include "ExternalFilesFactory.h"
#include "ExternalFilesATP.h"

using namespace brathl;

namespace brathl
{


/** \addtogroup product Products
  @{ */

/** 
  Netcdf product management class.


 \version 1.0
*/

class CProductNetCdf : public CProduct
{

public:
    
  /// Empty CProductNetCdf ctor
  CProductNetCdf();

  
  /** Creates new CProductNetCdf object
    \param fileName [in] : file name to be connected */
  CProductNetCdf(const string& fileName);
  
  /** Creates new CProductNetCdf object
    \param fileNameList [in] : list of file to be connected */
  CProductNetCdf(const CStringList& fileNameList);
    
  /// Destructor
  virtual ~CProductNetCdf();

  virtual void InitDateRef();

  virtual bool GetDateMinMax(CDatePeriod& datePeriodMinMax);
  virtual bool GetLatLonMinMax(CLatLonRect& latlonRectMinMax);

  virtual bool HasCriteriaInfo() { return true; };

  virtual void ApplyCriteria(CStringList& filteredFileList, const string& logFileName = "");

  virtual bool ApplyCriteriaLatLon(CCriteriaInfo* criteriaInfo);
  virtual bool ApplyCriteriaDatetime(CCriteriaInfo* criteriaInfo);
  virtual bool ApplyCriteriaPass(CCriteriaInfo* criteriaInfo);
  virtual bool ApplyCriteriaPassInt(CCriteriaInfo* criteriaInfo);
  virtual bool ApplyCriteriaPassString(CCriteriaInfo* criteriaInfo);
  virtual bool ApplyCriteriaCycle(CCriteriaInfo* criteriaInfo);

  CFieldNetCdf* ReadDateCriteriaValue(CFieldInfo& fieldInfo, CDate& date, bool wantMin = true);
  CFieldNetCdf* ReadDoubleCriteriaValue(CFieldInfo& fieldInfo, double& value, bool wantMin = true);

  virtual bool IsOpened();
  virtual void CheckFileOpened();
  virtual bool IsOpened(const string& fileName);

  virtual void GetRecords(CStringArray& array);

  virtual bool Close();

  virtual bool Open(const string& fileName, const string& dataSetName, CStringList& listFieldToRead);
  virtual bool Open(const string& fileName, const string& dataSetName);
  virtual bool Open(const string& fileName);

  virtual void Rewind ();
  virtual bool NextRecord ();
  virtual bool PrevRecord ();

  void InitDataset();

  //virtual void ReadBratFile(const string& fileName, const string& dataSetName, const string& field);
  //virtual void ReadBratFile(const string& fileName, const string& dataSetName, CStringList& listField);

  virtual void ReadBratRecord(int32_t iRecord);

  virtual int32_t GetNumberOfRecords(const string& dataSetName /*NOT USED*/);
  virtual int32_t GetNumberOfRecords();

  //virtual CField* GetFieldRead(const string& fieldName);

  void InitLatLonFieldName();

  virtual void InitCriteriaInfo();

  const CStringArray* GetAxisDims() { return &m_axisDims; };
  void SetAxisDims(const CStringArray& value) { m_axisDims = value; };

  CStringArray* GetComplementDims() { return &m_complementDims; };
  void SetComplementDims(const CStringArray& value) { m_complementDims = value; };
  
  CStringArray* GetDimsToReadOneByOne() { return &m_dimsToReadOneByOne; };
  void SetDimsToReadOneByOne(const CStringArray& value) { m_dimsToReadOneByOne = value; };
  void AddDimsToReadOneByOne(const CStringArray& value) { m_dimsToReadOneByOne.InsertUnique(value); };
  
  void GetNetCdfDimensions(const vector<CExpression>& expressions, CStringArray& commonDimNames);
  void GetNetCdfDimensions(const CExpression& expr, CStringArray& commonDimNames);
  void GetNetCdfDimensions(const CStringArray& fields, CStringArray& commonDimNames);
  void GetNetCdfDimensions(const vector<CExpression>& expressions, CStringArray& commonDimNames, const string& recordName);
  void GetNetCdfDimensions(const CExpression& expr, CStringArray& commonDimNames, const string& recordName);
  void GetNetCdfDimensions(const CStringArray& fields, CStringArray& commonDimNames, const string& recordName);

  void GetNetCdfDimensionsWithoutAlgo(const vector<CExpression>& expressions, CStringArray& commonDimNames, const string& recordName);
  void GetNetCdfDimensionsWithoutAlgo(const CExpression& expr, CStringArray& commonDimNames, const string& recordName);

  virtual void SetForceReadDataOneByOne(bool value) { m_forceReadDataOneByOne = value; };
  virtual bool GetForceReadDataOneByOne() { return m_forceReadDataOneByOne; };
  //virtual void SetForceReadDataComplementDimsOneByOne(bool value) { m_forceReadDataComplementDimsOneByOne = value; };

  // throws an exception if file is not opened
  void MustBeOpened();

  virtual CProduct* Clone();

  virtual void NetCdfProductInitialization(CProduct* from);

  bool IsLatField(CFieldNetCdf* field);
  bool IsLonField(CFieldNetCdf* field);

  bool IsApplyNetcdfProductInitialisation() {return m_applyNetcdfProductInitialisation;};
  void SetApplyNetcdfProductInitialisation(bool value) {m_applyNetcdfProductInitialisation = value;};

  ///Dump fonction
  virtual void Dump(ostream& fOut = cerr);

  static bool IsProductNetCdf(CBratObject* ob);
  
  static CProductNetCdf* GetProductNetCdf(CBratObject* ob, bool withExcept = true);

  CExternalFilesNetCDF* GetExternalFile() { return m_externalFile; };

  virtual void SetOffset(double value);
  virtual void AddOffset(double value, CField* field = NULL);


protected:

  void Init();

  virtual void RewindInit ();
  virtual void RewindProcess ();
  virtual void RewindEnd ();


//  virtual void InitCriteriaInfoJason2();
//  virtual void InitCriteriaInfoGeneric();

  CFieldNetCdf* FindTimeField();
  CFieldNetCdf* FindLatField();
  CFieldNetCdf* FindLonField();
  CFieldNetCdf* FindPassField();
  CFieldNetCdf* FindCycleField();


  void DeleteExternalFile();
  void DeleteFieldsToReadMap();

  virtual void InitInternalFieldName(const string& dataSetName, CStringList& listField, bool convertDate = false);
  virtual void InitInternalFieldName(CStringList& listField, bool convertDate = false);

  virtual bool Open();

  virtual string MakeInternalFieldName(const string& dataSetName, const string& field);
  virtual string MakeInternalFieldName(const string& field);

  virtual void LoadFieldsInfo();
  //virtual CFieldNetCdf* CreateField(const string& fieldName);
  virtual void CreateFieldSets();

  virtual void ReadBratFieldRecord(const string& key);
  virtual void ReadBratFieldRecord(CField::CListField::iterator it);
  
  virtual CFieldNetCdf* Read(CFieldInfo& fieldInfo, double& value, bool wantMin = true);
  virtual void Read(CFieldInfo& fieldInfo, string& value);
  virtual void Read(CFieldNetCdf* field, double& value);
  virtual void Read(CFieldNetCdf* field, CDoubleArray& vect);
  virtual void Read(CFieldNetCdf* field, CExpressionValue& value);
  virtual void ReadAll(CFieldNetCdf* field);
  virtual void ReadAll(CFieldNetCdf* field, CExpressionValue& value);

  virtual void FillDescription();



public:
  static const string m_virtualRecordName;


protected:
  CExternalFilesNetCDF* m_externalFile;
  //bool m_oneRecordsInMemory;
  bool m_forceReadDataOneByOne;
  //bool m_forceReadDataComplementDimsOneByOne;

  /** Map of the fields to read  (key : var name --> CFieldNetCdf object)
  * NB : CFieldNetCdf objects stored in this map have not to be delete (they are not a copy !!!)
    */
  CObMap* m_fieldsToRead;
  
  CStringArray m_axisDims;
  CStringArray m_complementDims;
  CStringArray m_dimsToReadOneByOne;

  bool m_applyNetcdfProductInitialisation;

  /*
  static const int32_t m_TIME_NAMES_SIZE;
  static const string m_TIME_NAMES[];

  static const int32_t m_LAT_NAMES_SIZE;
  static const string m_LAT_NAMES[];

  static const int32_t m_LON_NAMES_SIZE;
  static const string m_LON_NAMES[];

  static const int32_t m_CYCLE_NAMES_SIZE;
  static const string m_CYCLE_NAMES[];

  static const int32_t m_PASS_NAMES_SIZE;
  static const string m_PASS_NAMES[];
*/




};


/** @} */

} //end namespace

#endif // !defined(_ProductNetCdf_h_)
