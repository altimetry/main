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
#if !defined(_ProductTopexSDR_h_)
#define _ProductTopexSDR_h_

#include "ProductTopex.h"
using namespace brathl;

namespace brathl
{


/** \addtogroup product Products
  @{ */

/** 
  Topex/Poseidon SDR product management class.


 \version 1.0
*/

class CProductTopexSDR : public CProductTopex
{

public:
    
  /// Empty CProductTopexSDR ctor
  CProductTopexSDR();

  
  /** Creates new CProductTopexSDR object
    \param fileName [in] : file name to be connected */
  CProductTopexSDR(const std::string& fileName);
  
  /** Creates new CProductTopexSDR object
    \param fileNameList [in] : list of file to be connected */
  CProductTopexSDR(const CStringList& fileNameList);
    
  /// Destructor
  virtual ~CProductTopexSDR();

  virtual std::string GetLabel();

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

public:


protected:

  /** Determines if a field object is a 'high resolution' array data
       For Topex/Poseidon, to be a 'high resolution' field, all conditions below have to be true :
       - CProductTopex rules (see CProductTopex::IsHighResolutionField)
       - the field has two dimensions and the first dimension is 10 or 5.
    \param field [in] : field to be tested.
  */
  virtual bool IsHighResolutionField(CField* field);
  virtual void SetHighResolution(CField* field);

  virtual void ProcessHighResolutionWithoutFieldCalculation();
  void ComputeHighResolutionFields(CDataSet* dataSet, double deltaLat, double deltaLon);

  virtual void CheckConsistencyHighResolutionField(CFieldSetArrayDbl* fieldSetArrayDbl);

  virtual void PutFlatHighResolution(CDataSet* dataSet, CFieldSetArrayDbl* fieldSetArrayDbl);

private:
  void Init();
public:
    

protected:
  
  uint32_t m_highRateNumHighResolutionMeasure;
  uint32_t m_lowRateNumHighResolutionMeasure;


private:
  

};

/** @} */

} //end namespace

#endif // !defined(_ProductTopexSDR_h_)
