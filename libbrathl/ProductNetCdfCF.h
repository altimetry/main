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
#if !defined(_ProductNetCdfCF_h_)
#define _ProductNetCdfCF_h_

#include "Product.h"
#include "ProductNetCdf.h"

#include "ExternalFiles.h"
#include "ExternalFilesFactory.h"

using namespace brathl;

namespace brathl
{


/** \addtogroup product Products
  @{ */

/** 
  Netcdf product management class.


 \version 1.0
*/

class CProductNetCdfCF : public CProductNetCdf
{

public:
    
  /// Empty CProductNetCdf ctor
  CProductNetCdfCF();

  
  /** Creates new CProductNetCdf object
    \param fileName [in] : file name to be connected */
  CProductNetCdfCF(const std::string& fileName);
  
  /** Creates new CProductNetCdf object
    \param fileNameList [in] : list of file to be connected */
  CProductNetCdfCF(const CStringList& fileNameList);
    
  /// Destructor
  virtual ~CProductNetCdfCF();

//  virtual void InitDateRef();


  //virtual void GetRecords(CStringArray& array);

  //virtual bool Close();

  //virtual void ReadBratFile(const std::string& fileName, const std::string& dataSetName, const std::string& field);
  //virtual void ReadBratFile(const std::string& fileName, const std::string& dataSetName, CStringList& listField);

  virtual void Rewind ();
  virtual bool NextRecord ();
  virtual bool PrevRecord ();



  //virtual void ReadBratRecord(int32_t iRecord);

  virtual int32_t GetNumberOfRecords(const std::string& dataSetName /*NOT USED*/);
  virtual int32_t GetNumberOfRecords();

  virtual CProduct* Clone();

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

  static bool IsProductNetCdfCF(CBratObject* ob);

  static CProductNetCdfCF* GetProductNetCdfCF(CBratObject* ob, bool withExcept = true);


protected:

  void Init();

  virtual void RewindInit ();
  virtual void RewindProcess ();
  virtual void RewindEnd ();

  //virtual void InitDateRef();

  //virtual bool Open();

  //virtual std::string MakeInternalFieldName(const std::string& field);

  //virtual void LoadFieldsInfo();
  //virtual CFieldNetCdf* CreateField(const std::string& fieldName);

  void InitDimIndexes(uint32_t value);
  bool IsAtBeginning();
  
  bool NextFieldIndex();
  bool PrevFieldIndex();


  void AdjustIndexesFromField(CFieldNetCdf *field, bool next = true);
  
  void AdjustIndexesToMin(bool next = true);
  void AdjustIndexesToMin(CFieldNetCdf *field, bool next = true);

  void SetFieldIndex();
  void SetFieldIndex(CFieldNetCdf *field);

  //bool NextIndex();
  virtual void InitDimsIndexToMax();
  bool CheckEOF();


  //virtual void ReadBratFieldRecord(CField::CListField::iterator it);

public:
//  static std::string m_virtualRecordName;

protected:
  // map index dimensions of the read fields (key : dim name --> current index)
  CUIntMap m_dimIndexes;

  /** Map of the dimension's ids of the read fields (key : dim name --> dim ids)
    */
  CIntMap m_dimIds;

  /** Map of the dimension's values of the read fields (key : dim name --> dim value)
    */
  CUIntMap m_dimValues;

  /** Map of the dimension's ranges of the read fields (key : dim name --> dim range)
    */
  //CUIntMap m_dimRanges;

  /** Array of the dimension count for reading (key : dim name --> count)
    */
  CUIntMap m_dimsCount;


  /** 'At beginning" flag
  */
  bool m_atBeginning;


};

/** @} */

} //end namespace

#endif // !defined(_ProductNetCdfCF_h_)
