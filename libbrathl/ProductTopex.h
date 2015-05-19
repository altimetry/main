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
#if !defined(_ProductTopex_h_)
#define _ProductTopex_h_

#include "Product.h"
using namespace brathl;

namespace brathl
{


/** \addtogroup product Products
  @{ */

/** 
  Topex/Poseidon product management class.


 \version 1.0
*/

class CProductTopex : public CProduct
{

public:
    
  /// Empty CProductTopex ctor
  CProductTopex();

  
  /** Creates new CProductTopex object
    \param fileName [in] : file name to be connected */
  CProductTopex(const string& fileName);
  
  /** Creates new CProductTopex object
    \param fileNameList [in] : list of file to be connected */
  CProductTopex(const CStringList& fileNameList);
    
  /// Destructor
  virtual ~CProductTopex();

  virtual void InitCriteriaInfo();

  virtual string GetLabel();

  ///Dump fonction
  virtual void Dump(ostream& fOut = cerr);

public:
  static const string m_PASS_FILE;
  static const string m_XNG_FILE;
  static const string m_SDR_PASS_FILE;

  static const string m_TOPEX_POSEIDON_HEADER;

  static const int32_t m_ALTIMETER_POSEIDON;		
  static const int32_t m_ALTIMETER_TOPEX;		

protected:


  virtual void InitDateRef();

  /** Determines if a field object is a 'high resolution' array data
       For Topex/Poseidon, to be a 'high resolution' field, all conditions below have to be true :
       - the field object is not an instance of CFieldBasic
       - the field has one dimension and the dimension is 10.
    \param field [in] : field to be tested.
  */
  virtual bool IsHighResolutionField(CField* field);

  virtual void SetDeltaTimeHighResolution(int32_t altimeterIndicator);

  virtual void AddInternalHighResolutionFieldCalculation();

  virtual void ProcessHighResolutionWithoutFieldCalculation();
  void ComputeHighResolutionFields(CDataSet* dataSet, double deltaLat, double deltaLon);

private:
  void Init();
public:
    

protected:
  string m_timeStampDayFieldName;
  string m_timeStampMillisecondFieldName;
  string m_timeStampMicrosecondFieldName;
  /** Altimeter Indicator. 
      This element is computed for TOPEX and POSEIDON data. 
      It indicates which altimeter is on at the time of the measurement. 
      Value Definition: 0 = POSEIDON on, 1 = TOPEX on
      */
  string m_altimeterIndicatorFieldName;

private:
  

};

/** @} */

} //end namespace

#endif // !defined(_ProductTopex_h_)
