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
#if !defined(_CriteriaDatetime_h_)
#define _CriteriaDatetime_h_

#include <string>

#include "new-gui/Common/tools/brathl_error.h" 
#include "brathl.h" 

#include "BratObject.h"
#include "List.h"
#include "DatePeriod.h"
#include "Criteria.h"

using namespace brathl;

namespace brathl
{




//-------------------------------------------------------------
//------------------- CCriteriaDatetime class --------------------
//-------------------------------------------------------------


/** \addtogroup product Datetime Criteria
  @{ */

/** 
  Datetime Criteria management class.


 \version 1.0
*/





class CCriteriaDatetime : public CCriteria
{

public:
    
  /// Empty CCriteriaDatetime ctor
  CCriteriaDatetime();

  CCriteriaDatetime(CCriteriaDatetime& c);

  CCriteriaDatetime(CCriteriaDatetime* c);

   /**
     * Constructor.
     * 
     * @param datePeriod period to set
    */
  CCriteriaDatetime(CDatePeriod& datePeriod);

   /**
     * Constructor.
     * 
     * @param from start date
     * @param to end date
    */
  CCriteriaDatetime(CDate& from, CDate& to); 
  
   /**
     * Constructor.
     * 
     * @param from start date
     * @param to end date
    */
  CCriteriaDatetime(const std::string& from, const std::string& to);

   /**
     * Constructor.
     * 
     * @param from start date (number of seconds since 1950-01-01)
     * @param to end date (number of seconds since 1950-01-01)
    */
  CCriteriaDatetime(double from, double to);

  /**
     * Constructor from a array that contains start date as std::string, end date as std::string
     * 
     * @param array start and end dates
     */
  CCriteriaDatetime(const CStringArray& array);


  ///Destructor
  virtual ~CCriteriaDatetime();

  CDatePeriod* GetDatePeriod() { return  &m_datePeriod; };
  
  CDate* GetFrom() { return  &(m_datePeriod.GetFrom()); };
  CDate* GetTo() { return  &(m_datePeriod.GetTo()); };

  std::string GetFromAsText() { return m_datePeriod.GetFromAsText(); };
  std::string GetToAsText() { return m_datePeriod.GetToAsText(); };

  /**
     * Sets date period from another one
     * 
     * @param datePeriod period to set
    */
  void Set(CDatePeriod& datePeriod);
   /**
     * Sets end date
     * 
     * @param to end date
    */
  void SetTo(CDate& to);

   /**
     * Sets end date
     * 
     * @param to end date
    */
  void SetTo(const std::string& strDate);
  
   /**
     * Sets start date
     * 
     * @param to start date
    */
  void SetFrom(CDate& from);
   /**
     * Sets start date
     * 
     * @param to start date
    */
  void SetFrom(const std::string& strDate);

   /**
     * Sets date period from start and end date
     * 
     * @param from start date
     * @param to end date
    */
  void Set(CDate& from, CDate& to);
   /**
     * Sets date period from start and end date
     * 
     * @param from start date
     * @param to end date
    */
  void Set(const std::string& from, const std::string& to);

  void SetFromText(const std::string& values, const std::string& delimiter = CDatePeriod::m_delimiter);
    
    /*
     * Sets date period from start and end date
     * 
     * @param from start date (number of seconds since 1950-01-01)
     * @param to end date (number of seconds since 1950-01-01)
    */
  void Set(double from, double to);

  /**
     * Sets a date period from a array that contains start date as std::string, end date as std::string
     * 
     * @param array start and end dates
    */
  void Set(const CStringArray& array);
  

  void Set(CCriteriaDatetime& c);

  /**
   * Create the intersection of this date period with the given one
   * @param datePeriod intersect with this
   * @param intersect intersection period
   * @return true, or false if there is no intersection
   */
  bool Intersect(CDatePeriod& datePeriod, CDatePeriod& intersect);
  /**
   * Create the intersection of this date period with the given one
   * @param otherFrom start date intersect with this
   * @param otherTo end date intersect with this
   * @param intersect intersection period
   * @return true, or false if there is no intersection
   */
  bool Intersect(double otherFrom, double otherTo, CDatePeriod& intersect);

  /*
   * Test the intersection of this date period with the given one
   * @param otherFrom start date intersect with this
   * @param otherTo end date intersect with this
   * @return true, or false if there is no intersection
   */
  bool Intersect(double otherFrom, double otherTo);

  /** Sets internal value to the default value (uninitialized)*/
  void SetDefaultValue();
  /**
     * Tests whether date period have been initialized or not
     *
     * @return true if not initialized
     */
  bool IsDefaultValue();

  std::string GetAsText(const std::string& delimiter = CDatePeriod::m_delimiter);


  static CCriteriaDatetime* GetCriteria(CBratObject* ob, bool withExcept = true);

  const CCriteriaDatetime& operator=(CCriteriaDatetime& c);

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

protected:

  void Init();

public:
    

protected:


  /**
     * Date period
     */
  CDatePeriod m_datePeriod;

private:
  

};

/** @} */

}

#endif // !defined(_CriteriaDatetime_h_)
