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

#if !defined(_ProductJason2_h_)
#define _ProductJason2_h_


#include "brathl.h"
#include "ProductNetCdfCF.h"
using namespace brathl;

namespace brathl
{

//-------------------------------------------------------------
//------------------- CProductJason2 class --------------------
//-------------------------------------------------------------

/** \addtogroup product Products
  @{ */

/** 
  Mapping products management class.


 \version 1.0
*/



class CProductJason2: public CProductNetCdfCF
{
public:
  /// CIntMap ctor
  CProductJason2();

  /** Creates new CProductNetCdf object
    \param fileName [in] : file name to be connected */
  CProductJason2(const string& fileName);
  
  /** Creates new CProductNetCdf object
    \param fileNameList [in] : list of file to be connected */
  CProductJason2(const CStringList& fileNameList);

  virtual ~CProductJason2();

  virtual bool HasCriteriaInfo() { return CProduct::HasCriteriaInfo(); };

  virtual void InitCriteriaInfo();

  virtual void InitDateRef();

  virtual void Dump(ostream& fOut  = cerr); 



protected:


protected:

  void Init();


};

/** @} */

}

#endif 

