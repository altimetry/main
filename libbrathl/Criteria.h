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
#if !defined(_Criteria_h_)
#define _Criteria_h_

#include <string>

#include "common/tools/brathl_error.h"
#include "brathl.h" 

#include "BratObject.h"
#include "List.h"
#include "Field.h"

using namespace brathl;

namespace brathl
{




//-------------------------------------------------------------
//------------------- CCriteria class --------------------
//-------------------------------------------------------------


/** \addtogroup product Criteria
  @{ */

/** 
  Criteria management class.


 \version 1.0
*/





class CCriteria : public CBratObject
{

public:
    
  /// Empty CCriteria ctor
  CCriteria();
    
  /// Destructor
  virtual ~CCriteria();


  int32_t GetKey() { return m_key; };

  /** Sets internal value to the default value (uninitialized)*/
  virtual void SetDefaultValue() = 0;
  /**
     * Tests whether value have been initialized or not
     *
     * @return true if not initialized
     */
  virtual bool IsDefaultValue() = 0;

  virtual std::string GetAsText(const std::string& delimiter) = 0;
 
  static CCriteria* GetCriteria(CBratObject* ob, bool withExcept = true);
  static void Adjust(CIntArray& array);

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

public:

  /** Kind of criteria enumeration.
  */
  typedef enum {
                  UNKNOWN,	/**< not set */
                  LATLON,	/**< geographical latitude/longitude area */
                  DATETIME,	/**< date/time */
                  PASS,	      	/**< Pass */
                  CYCLE	        /**< Cycle */
        } CriteriaKind;



protected:
private:

  void Init();

public:
    

protected:
  int32_t m_key;

private:
  

};

/** @} */

}

#endif // !defined(_Criteria_h_)
