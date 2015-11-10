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
#if !defined(_ProductCryosat_h_)
#define _ProductCryosat_h_

#include "Product.h"
using namespace brathl;

namespace brathl
{


/** \addtogroup product Products
  @{ */

/** 
  Cryosat product management class.


 \version 1.0
*/

class CProductCryosat : public CProduct
{

public:
    
  /// Empty CProductCryosat ctor
  CProductCryosat();

  
  /** Creates new CProductCryosat object
    \param fileName [in] : file name to be connected */
  CProductCryosat(const std::string& fileName);
  
  /** Creates new CProductCryosat object
    \param fileNameList [in] : list of file to be connected */
  CProductCryosat(const CStringList& fileNameList);

  /// Destructor
  virtual ~CProductCryosat();

  virtual void InitCriteriaInfo();

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);


protected:
  virtual bool FindParentToRead(CField* fromField, CObList* parentFieldList);

  virtual void InitDateRef();

  virtual bool IsHighResolutionField(CField* field);
  virtual void ProcessHighResolutionWithoutFieldCalculation();


private:
  void Init();

public:
    

protected:
		

private:
  

};

/** @} */

} //end namespace

#endif // !defined(_ProductCryosat_h_)
