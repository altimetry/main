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
#if !defined(_ProductGfo_h_)
#define _ProductGfo_h_

#include "Product.h"
using namespace brathl;

namespace brathl
{


/** \addtogroup product Products
  @{ */

/** 
  Ers product management class.


 \version 1.0
*/

class CProductGfo : public CProduct
{

public:
    
  /// Empty CProductGfo ctor
  CProductGfo();

  
  /** Creates new CProductGfo object
    \param fileName [in] : file name to be connected */
  CProductGfo(const std::string& fileName);
  
  /** Creates new CProductGfo object
    \param fileNameList [in] : list of file to be connected */
  CProductGfo( const CStringList &fileNameList, bool check_only_first_file );

  /// Destructor
  virtual ~CProductGfo();

  virtual void InitCriteriaInfo();

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);


protected:

  virtual void InitDateRef();

  /** Determines if a field object is a 'high resolution' array data
       For Jason, to be a 'high resolution' field, all conditions below have to be true :
       - the field object is not an instance of CFieldBasic
       - the field has one dimension and the dimension is 10.
    \param field [in] : field to be tested.
  */
  virtual bool IsHighResolutionField(CField* field);


  virtual void AddInternalHighResolutionFieldCalculation();

  virtual void ProcessHighResolutionWithoutFieldCalculation();
  void ComputeHighResolutionFields(CDataSet* dataSet, double deltaLat, double deltaLon);

private:
  void Init();
public:
    

protected:  
  std::string m_timeStampSecondFieldName;
  std::string m_timeStampMicrosecondFieldName;
		

private:
  

};

/** @} */

} //end namespace

#endif // !defined(_ProductGfo_h_)
