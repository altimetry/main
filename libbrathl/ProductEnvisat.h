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
#if !defined(_ProductEnvisat_h_)
#define _ProductEnvisat_h_

#include "Product.h"
using namespace brathl;

namespace brathl
{


/** \addtogroup product Products
  @{ */

/** 
  Envisat product management class.


 \version 1.0
*/

class CProductEnvisat : public CProduct
{

public:
    
  /// Empty CProductEnvisat ctor
  CProductEnvisat();

  
  /** Creates new CProductEnvisat object
    \param fileName [in] : file name to be connected */
  CProductEnvisat(const std::string& fileName);
  
  /** Creates new CProductEnvisat object
    \param fileNameList [in] : list of file to be connected */
  CProductEnvisat(const CStringList& fileNameList);

  /// Destructor
  virtual ~CProductEnvisat();

  virtual void InitCriteriaInfo();

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

protected:

  virtual bool FindParentToRead(CField* fromField, CObList* parentFieldList);

  virtual void InitDateRef();

  /** Determines if a field object is a 'high resolution' array data
       For Envisat, to be a 'high resolution' field, all conditions below have to be true :
       - the field object is not an instance of CFieldBasic
       - the field has one dimension and the dimension is 20.
       - the field name is different from the '18 Hz latitude differences from 1 Hz' field (1) and the 
         '18 Hz longitude differences from 1 Hz' field (1) 

          (1) if this field are present in the record. Note that only off-line product 
              (product type RA2_GDR_2P and RA2_MWS_2P have these fields

       - the field name contains 'hz18' or '18hz'
    \param field [in] : field to be tested.
  */
  virtual bool IsHighResolutionField(CField* field);
  bool IsParentHighResolutionField(CField* field);

  /** Get the "High resolution latitude differences" field name */
  virtual std::string GetHighResolutionLatDiffFieldName() {return m_highResolutionLatDiffFieldName;};
  /** Set the "High resolution latitude differences" field name */
  virtual void SetHighResolutionLatDiffFieldName(const std::string& value) {m_highResolutionLatDiffFieldName = value;};

  /** Get the "High resolution longitude differences" field name */
  virtual std::string GetHighResolutionLonDiffFieldName() {return m_highResolutionLonDiffFieldName;};
  /** Set the "High resolution longitude differences" field name */
  virtual void SetHighResolutionLonDiffFieldName(const std::string& value) {m_highResolutionLonDiffFieldName = value;};

  virtual void ProcessHighResolutionWithFieldCalculation();
  virtual void ProcessHighResolutionWithoutFieldCalculation();

  void ComputeHighResolutionFields(CDataSet* dataSet);
  void ComputeHighResolutionFields(CDataSet* dataSet, double deltaLat, double deltaLon);

  virtual bool HasHighResolutionFieldCalculation();
  virtual void AddInternalHighResolutionFieldCalculation();

  // Checks if all the values in the '18 Hz latitude differences from 1 Hz' field and 
  // '18 Hz longitude differences from 1 Hz' field are the same 
  // if true then consider there are no values
  bool HasHighResolutionFieldCalculationValue(CDataSet* dataset);
  bool HasHighResolutionFieldCalculationValue(CDataSet* dataset, CFieldSetArrayDbl* fieldSetArrayDbl);

private:
  void Init();

public:
  

protected:
  std::string m_highResolutionLatDiffFieldName;
  std::string m_highResolutionLonDiffFieldName;
		
  std::string m_timeStampFieldName;
  CStringArray m_arrayTimeStampFieldName;


private:
  

};

/** @} */

} //end namespace

#endif // !defined(_ProductEnvisat_h_)
