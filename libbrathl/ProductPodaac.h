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
#if !defined(_ProductPodaac_h_)
#define _ProductPodaac_h_

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

class CProductPodaac : public CProduct
{

public:
    
  /// Empty CProductPodaac ctor
  CProductPodaac();

  
  /** Creates new CProductPodaac object
    \param fileName [in] : file name to be connected */
  CProductPodaac(const std::string& fileName);
  
  /** Creates new CProductPodaac object
    \param fileNameList [in] : list of file to be connected */
  CProductPodaac( const CStringList &fileNameList, bool check_only_first_file );

  /// Destructor
  virtual ~CProductPodaac();

  virtual const std::string& GetLabel() const override;

  virtual void InitCriteriaInfo();

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);
public:

  static const std::string m_J1SSHA_PASS_FILE;
  static const std::string m_J1SSHA_ATG_FILE;
  static const std::string m_TPSSHA_PASS_FILE;
  static const std::string m_TPSSHA_ATG_FILE;


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

#endif // !defined(_ProductPodaac_h_)
