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
  CProductNetCdf(const std::string& fileName);

  /** Creates new CProductNetCdf object
    \param fileNameList [in] : list of file to be connected */
  CProductNetCdf( const CStringList& fileNameList, bool check_only_first_files );

  /// Destructor
  virtual ~CProductNetCdf();

  virtual void InitDateRef() override;

  virtual bool GetDateMinMax(CDatePeriod& datePeriodMinMax, CProgressInterface *pi = nullptr ) override;
  virtual bool GetLatLonMinMax(CLatLonRect& latlonRectMinMax, CProgressInterface *pi = nullptr ) override;

  virtual bool HasCriteriaInfo()  override { return true; }

  virtual bool ApplyCriteria( CStringList& filteredFileList, CProgressInterface *pi, const std::string& log_file = "" ) override;

  virtual bool ApplyCriteriaLatLon(CCriteriaInfo* criteriaInfo) override;
  virtual bool ApplyCriteriaDatetime(CCriteriaInfo* criteriaInfo) override;
  virtual bool ApplyCriteriaPass(CCriteriaInfo* criteriaInfo) override;
  virtual bool ApplyCriteriaPassInt(CCriteriaInfo* criteriaInfo) override;
  virtual bool ApplyCriteriaPassString(CCriteriaInfo* criteriaInfo) override;
  virtual bool ApplyCriteriaCycle(CCriteriaInfo* criteriaInfo) override;

  CFieldNetCdf* ReadDateCriteriaValue(CFieldInfo& fieldInfo, CDate& date, bool wantMin = true);
  CFieldNetCdf* ReadDoubleCriteriaValue(CFieldInfo& fieldInfo, double& value, bool wantMin = true);

  virtual bool IsOpened() override;
  virtual void CheckFileOpened() override;
  virtual bool IsOpened(const std::string& fileName) override;

  virtual void GetRecords(CStringArray& array) override;

  virtual bool Close() override;

  virtual bool Open(const std::string& fileName, const std::string& dataSetName, CStringList& listFieldToRead);
  virtual bool Open(const std::string& fileName, const std::string& dataSetName) override;
  virtual bool Open(const std::string& fileName) override;

  virtual void Rewind () override;
  virtual bool NextRecord ();
  virtual bool PrevRecord ();

  void InitDataset();

  //virtual void ReadBratFile(const std::string& fileName, const std::string& dataSetName, const std::string& field);
  //virtual void ReadBratFile(const std::string& fileName, const std::string& dataSetName, CStringList& listField);

  virtual void ReadBratRecord(int32_t iRecord) override;

  virtual int32_t GetNumberOfRecords(const std::string& dataSetName /*NOT USED*/) override;
  virtual int32_t GetNumberOfRecords() override;

  //virtual CField* GetFieldRead(const std::string& fieldName);

  void InitLatLonFieldName();

  virtual void InitCriteriaInfo() override;

  const CStringArray* GetAxisDims() { return &m_axisDims; }
  void SetAxisDims(const CStringArray& value) { m_axisDims = value; }

  CStringArray* GetComplementDims() { return &m_complementDims; }
  void SetComplementDims(const CStringArray& value) { m_complementDims = value; }

  CStringArray* GetDimsToReadOneByOne() { return &m_dimsToReadOneByOne; }
  void SetDimsToReadOneByOne(const CStringArray& value) { m_dimsToReadOneByOne = value; }
  void AddDimsToReadOneByOne(const CStringArray& value) { m_dimsToReadOneByOne.InsertUnique(value); }

  void GetNetCdfDimensions(const std::vector<CExpression>& expressions, CStringArray& commonDimNames);
  void GetNetCdfDimensions(const CExpression& expr, CStringArray& commonDimNames);
  void GetNetCdfDimensions(const CStringArray& fields, CStringArray& commonDimNames);
  void GetNetCdfDimensions(const std::vector<CExpression>& expressions, CStringArray& commonDimNames, const std::string& recordName);
  void GetNetCdfDimensions(const CExpression& expr, CStringArray& commonDimNames, const std::string& recordName);
  void GetNetCdfDimensions(const CStringArray& fields, CStringArray& commonDimNames, const std::string& recordName);

  void GetNetCdfDimensionsWithoutAlgo(const std::vector<CExpression>& expressions, CStringArray& commonDimNames, const std::string& recordName);
  void GetNetCdfDimensionsWithoutAlgo(const CExpression& expr, CStringArray& commonDimNames, const std::string& recordName);

  virtual void SetForceReadDataOneByOne(bool value) override { m_forceReadDataOneByOne = value; }
  virtual bool GetForceReadDataOneByOne() override { return m_forceReadDataOneByOne; }
  //virtual void SetForceReadDataComplementDimsOneByOne(bool value) { m_forceReadDataComplementDimsOneByOne = value; };

  // throws an exception if file is not opened
  void MustBeOpened();

  virtual CProduct* Clone() override;

  virtual void NetCdfProductInitialization(CProduct* from);

  bool IsLatField(CFieldNetCdf* field);
  bool IsLonField(CFieldNetCdf* field);

  bool IsApplyNetcdfProductInitialisation() {return m_applyNetcdfProductInitialisation;}
  void SetApplyNetcdfProductInitialisation(bool value) {m_applyNetcdfProductInitialisation = value;}

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr) override;

  static bool IsProductNetCdf(CBratObject* ob);

  static CProductNetCdf* GetProductNetCdf(CBratObject* ob, bool withExcept = true);

  CExternalFilesNetCDF* GetExternalFile() { return m_externalFile; }

  virtual void SetOffset(double value) override;
  virtual void AddOffset(double value, CField* field = NULL) override;


protected:

  void Init();

  virtual void RewindInit () override;
  virtual void RewindProcess () override;
  virtual void RewindEnd () override;


//  virtual void InitCriteriaInfoJason2();
//  virtual void InitCriteriaInfoGeneric();

  CFieldNetCdf* FindTimeField();
  CFieldNetCdf* FindLatField();
  CFieldNetCdf* FindLonField();
  CFieldNetCdf* FindPassField();
  CFieldNetCdf* FindCycleField();


  void DeleteExternalFile();
  void DeleteFieldsToReadMap();

  virtual void InitInternalFieldName(const std::string& dataSetName, CStringList& listField, bool convertDate = false) override;
  virtual void InitInternalFieldName(CStringList& listField, bool convertDate = false) override;

  virtual bool Open() override;

  virtual std::string MakeInternalFieldName(const std::string& dataSetName, const std::string& field) override;
  virtual std::string MakeInternalFieldName(const std::string& field) override;

  virtual void LoadFieldsInfo() override;
  //virtual CFieldNetCdf* CreateField(const std::string& fieldName);
  virtual void CreateFieldSets();

  virtual void ReadBratFieldRecord(const std::string& key);
  virtual void ReadBratFieldRecord(CField::CListField::iterator it) override;


  struct CAdjustValidMinMax
  {
      virtual void operator()( CFieldNetCdf *field ) const
      {
          field->AdjustValidMinMaxFromValues();
      }
  };


  virtual CFieldNetCdf* Read(CFieldInfo& fieldInfo, double& value, bool wantMin = true, const CAdjustValidMinMax &adjust_algo = CAdjustValidMinMax() );
  virtual void Read(CFieldInfo& fieldInfo, std::string& value);
  virtual void Read(CFieldNetCdf* field, double& value);
  virtual void Read(CFieldNetCdf* field, CDoubleArray& vect);
  virtual void Read(CFieldNetCdf* field, CExpressionValue& value );
  virtual void ReadAll(CFieldNetCdf* field, const CAdjustValidMinMax &adjust_algo = CAdjustValidMinMax() );
  virtual void ReadAll(CFieldNetCdf* field, CExpressionValue& value);

  virtual void FillDescription() override;



public:
  static const std::string m_virtualRecordName;


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
  static const std::string m_TIME_NAMES[];

  static const int32_t m_LAT_NAMES_SIZE;
  static const std::string m_LAT_NAMES[];

  static const int32_t m_LON_NAMES_SIZE;
  static const std::string m_LON_NAMES[];

  static const int32_t m_CYCLE_NAMES_SIZE;
  static const std::string m_CYCLE_NAMES[];

  static const int32_t m_PASS_NAMES_SIZE;
  static const std::string m_PASS_NAMES[];
*/




};


/** @} */

} //end namespace

#endif // !defined(_ProductNetCdf_h_)
