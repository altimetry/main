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
#if !defined(_ProductErsWAP_h_)
#define _ProductErsWAP_h_

#include "ProductErs.h"
using namespace brathl;

namespace brathl
{


/** \addtogroup product Products
  @{ */

/** 
  Ers product management class.


 \version 1.0
*/

class CProductErsWAP : public CProductErs
{

public:
    
  /// Empty CProductErsWAP ctor
  CProductErsWAP();

  
  /** Creates new CProductErsWAP object
    \param fileName [in] : file name to be connected */
  CProductErsWAP(const string& fileName);
  
  /** Creates new CProductErsWAP object
    \param fileNameList [in] : list of file to be connected */
  CProductErsWAP(const CStringList& fileNameList);

  /// Destructor
  virtual ~CProductErsWAP();

  virtual void InitCriteriaInfo();

  ///Dump fonction
  virtual void Dump(ostream& fOut = cerr);


protected:

  virtual bool FindParentToRead(CField* fromField, CObList* parentFieldList);

  virtual void InitDateRef();

  /** Determines if a field object is a 'high resolution' array data
       For Jason, to be a 'high resolution' field, all conditions below have to be true :
       - the field object is not an instance of CFieldBasic
       - the field has one dimension and the dimension is 10.
    \param field [in] : field to be tested.
  */
  virtual bool IsHighResolutionField(CField* field);
  virtual bool IsDirectHighResolutionField(CField* field);


  virtual void AddInternalHighResolutionFieldCalculation();

  virtual void ProcessHighResolutionWithoutFieldCalculation();
  void ComputeHighResolutionFields(CDataSet* dataSet);

private:
  void Init();

public:
    

protected:
  
  string m_timeStampDayFieldName;
  string m_timeStampMillisecondFieldName;
  string m_timeStampMicrosecondFieldName;		

private:
  

};

/** @} */

} //end namespace

#endif // !defined(_ProductErsWAP_h_)
