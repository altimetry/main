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
#if !defined(_DatePeriod_h_)
#define _DatePeriod_h_

#include <string>

#include "new-gui/Common/tools/brathl_error.h" 
#include "brathl.h" 

#include "List.h"
#include "Date.h"
using namespace brathl;

namespace brathl
{


//-------------------------------------------------------------
//------------------- CDatePeriod class --------------------
//-------------------------------------------------------------


/** \addtogroup date_conversionCPP Date conversion classes
  @{ */

/** 
  Date interval management class.


 \version 1.0
*/





class CDatePeriod : public CBratObject
{

public:
    
  /// Empty CDatePeriod ctor
  CDatePeriod();

   /**
     * Copy constructor.
     * 
     * @param datePeriod period to set
    */
  CDatePeriod(CDatePeriod& datePeriod);

   /**
     * Constructor.
     * 
     * @param from start date
     * @param to end date
    */
  CDatePeriod(CDate& from, CDate& to); 
  
   /**
     * Constructor.
     * 
     * @param from start date
     * @param to end date
    */
  CDatePeriod(const std::string& from, const std::string& to);

   /**
     * Constructor.
     * 
     * @param from start date (number of seconds since 1950-01-01)
     * @param to end date (number of seconds since 1950-01-01)
    */
  CDatePeriod(double from, double to);

  /**
     * Constructor from a array that contains start date as std::string, end date as std::string
     * 
     * @param array start and end dates
     */
  CDatePeriod(const CStringArray& array);


  ///Destructor
  virtual ~CDatePeriod();


   /**
     * Gets end date
     * 
     * @return end date
    */
  CDate& GetTo() { return m_to; };
   /**
     * Gets start date
     * 
     * @return start date
    */
  CDate& GetFrom() { return m_from; };

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
    /**
     * Sets date period from another one
     * 
     * @param datePeriod period to set
    */
  void Set(CDatePeriod& datePeriod);

  /** Sets internal value to the default value (uninitialized)*/
  void SetDefaultValue();

  /**
     * Tests whether date period have been initialized or not
     *
     * @return true if not initialized
     */
  bool IsDefaultValue();

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
  bool Intersect(CDate& otherFrom, CDate& otherTo, CDatePeriod& intersect);


  bool Union(CDatePeriod& datePeriod);
  bool Union(CDate& otherFrom, CDate& otherTo);
  bool Union(CDatePeriod& datePeriod, CDatePeriod& unionDate);
  bool Union(CDate& otherFrom, CDate& otherTo, CDatePeriod& unionDate);


  /** Assigns a new value to the CDatePeriod object, with a CDatePeriod object */
  const CDatePeriod& operator= (CDatePeriod& datePeriod);


  std::string AsString(const std::string& format = "",  bool withMuSecond = false);

  std::string GetAsText(const std::string& delimiter = CDatePeriod::m_delimiter);
  std::string GetFromAsText();
  std::string GetToAsText();

  bool GetWithMuSecond() { return m_withMuSecond; };
  void SetWithMuSecond(bool value) { m_withMuSecond = value; };

  std::string GetFormat() { return m_format; };
  void SetFormat(const std::string& value) { m_format = value; };

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

protected:

  void Init();
  void Adjust();

public:
    
  static const std::string m_delimiter;

protected:


  /**
     * Start date
     */
  CDate m_from;
  /**
     * End date
     */
  CDate m_to;

  bool m_withMuSecond;
  std::string m_format;


private:
  

};

/** @} */

}

#endif // !defined(_DatePeriod_h_)
