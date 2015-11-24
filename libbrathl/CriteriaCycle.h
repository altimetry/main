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
#if !defined(_CriteriaCycle_h_)
#define _CriteriaCycle_h_

#include <string>

#include "new-gui/Common/tools/brathl_error.h" 
#include "brathl.h" 

#include "BratObject.h"
#include "List.h"
#include "Criteria.h"

using namespace brathl;

namespace brathl
{


//-------------------------------------------------------------
//------------------- CCriteriaCycle class --------------------
//-------------------------------------------------------------


/** \addtogroup product Pass number Criteria
  @{ */

/** 
  Pass number (from/to) Criteria management class.


 \version 1.0
*/

class CCriteriaCycle : public CCriteria
{

public:
    
  /// Empty CCriteriaCycle ctor
  CCriteriaCycle();

  CCriteriaCycle(CCriteriaCycle& c);

  CCriteriaCycle(CCriteriaCycle* c);

   /**
     * Constructor.
     * 
     * @param from start pass
     * @param to end pass
    */
  CCriteriaCycle(int32_t from, int32_t to); 
  
   /**
     * Constructor.
     * 
     * @param from start pass
     * @param to end pass
    */
  CCriteriaCycle(const std::string& from, const std::string& to);

  /**
     * Constructor from a array that contains start pass as std::string, end pass as std::string
     * 
     * @param array start and end dates
     */
  CCriteriaCycle(const CStringArray& array);

  void Set(CCriteriaCycle& c);

  const CCriteriaCycle& operator=(CCriteriaCycle& c);



  ///Destructor
  virtual ~CCriteriaCycle();

  int32_t GetFrom() { return  m_from; };
  int32_t GetTo() { return  m_to; };


   /**
     * Sets end pass
     * 
     * @param to end pass
    */
  void SetTo(int32_t to);

   /**
     * Sets end pass
     * 
     * @param to end pass
    */
  void SetTo(const std::string& to);
  
   /**
     * Sets start pass
     * 
     * @param to start pass
    */
  void SetFrom(int32_t from);
   /**
     * Sets start pass
     * 
     * @param to start pass
    */
  void SetFrom(const std::string& from);

   /**
     * Sets date period from start and end pass
     * 
     * @param from start pass
     * @param to end pass
    */
  void Set(int32_t from, int32_t to);
   /**
     * Sets date period from start and end pass
     * 
     * @param from start pass
     * @param to end pass
    */
  void Set(const std::string& from, const std::string& to);

  void SetFromText(const std::string& values, const std::string& delimiter = CCriteriaCycle::m_delimiter);

  /**
     * Sets a date period from a array that contains start pass as std::string, end pass as std::string
     * 
     * @param array start and end dates
    */
  void Set(const CStringArray& array);
  
  /**
   * Create the intersection of this date period with the given one
   * @param array that contains start pass as std::string, end pass as std::string
   * @param intersect intersection period
   * @return true, or false if there is no intersection
   */
  bool Intersect(CStringArray& array, CStringArray& intersect);
  /**
   * Create the intersection of this date period with the given one
   * @param array that contains start pass as std::string, end pass as std::string
   * @param intersect intersection period
   * @return true, or false if there is no intersection
   */
  bool Intersect(CStringArray& array, CIntArray& intersect);
  /**
   * Create the intersection of this date period with the given one
   * @param array that contains start pass as std::string, end pass as std::string
   * @param intersect intersection period
   * @return true, or false if there is no intersection
   */
  bool Intersect(CIntArray& array, CStringArray& intersect);
  /**
   * Create the intersection of this date period with the given one
   * @param array that contains start pass as std::string, end pass as std::string
   * @param intersect intersection period
   * @return true, or false if there is no intersection
   */
  bool Intersect(CIntArray& array, CIntArray& intersect);

  bool Intersect(int32_t from, int32_t to, CStringArray& intersect);
  bool Intersect(int32_t from, int32_t to, CIntArray& intersect);
  
  bool Intersect(const std::string& from, const std::string& to, CIntArray& intersect);

  bool Intersect(double otherFrom, double otherTo, CIntArray& intersect);

  bool Intersect(const std::string& from, const std::string& to, CStringArray& intersect);

  /** Sets internal value to the default value (uninitialized)*/
  void SetDefaultValue();
  /**
     * Tests whether the pass have been initialized or not
     *
     * @return true if not initialized
     */
  bool IsDefaultValue();

  std::string GetAsText(const std::string& delimiter = CCriteriaCycle::m_delimiter);

  static CCriteriaCycle* GetCriteria(CBratObject* ob, bool withExcept = true);


  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

protected:

  void Init();

  void Adjust();

public:
  static const std::string m_delimiter;  
    

protected:


  /**
     * start pass
     */
  int32_t m_from;
  /**
     * end pass
     */
  int32_t m_to;

private:
  

};
/** @} */

}

#endif // !defined(_CriteriaCycle_h_)
