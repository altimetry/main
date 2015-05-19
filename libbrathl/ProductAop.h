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
#if !defined(_ProductAop_h_)
#define _ProductAop_h_

#include "Product.h"
using namespace brathl;

namespace brathl
{


/** \addtogroup product Products
  @{ */

/** 
  Aop product management class.


 \version 1.0
*/

class CProductAop : public CProduct
{

public:
    
  /// Empty CProductAop ctor
  CProductAop();

  
  /** Creates new CProductAop object
    \param fileName [in] : file name to be connected */
  CProductAop(const string& fileName);
  
  /** Creates new CProductAop object
    \param fileNameList [in] : list of file to be connected */
  CProductAop(const CStringList& fileNameList);

  /// Destructor
  virtual ~CProductAop();

  virtual void InitCriteriaInfo();

  ///Dump fonction
  virtual void Dump(ostream& fOut = cerr);

protected:


  virtual void InitDateRef();

  

private:
  void Init();

public:
    

protected:
		

private:
  

};

/** @} */

} //end namespace

#endif // !defined(_ProductAop_h_)
