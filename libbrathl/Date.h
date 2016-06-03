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
#if !defined(_Date_h_)
#define _Date_h_

#include <string>
#include <iostream>


namespace brathl
{

/** \addtogroup date_conversionCPP Date conversion classes
  @{ */

/** 
  Date management and conversion class.

 This class allows calendar an date conversion.
 
 \warning Date before 1950/01/01 00:00:00:00 are not accepted 
 
 \version 1.0
*/

class CDate 

{

public:   
  /// Constructs a date with a 1950/01/01 value
  CDate();  
  
  /**
    Constructs a date from a std::string 
    \param strDate : 
      Allowed format are  :
      - YYYY-MM-DD HH:MN:SS.MS std::string
      - a julian std::string (format:positive 'Days Seconds Microseconds' or positive decimal julian day)
  */   
  CDate(const char *strDate);
  /// Constructs a date from another CDate object
  CDate(const CDate& date);
  /// Constructs a date from year, month, day, hour, minute, second, microsecond
  CDate(const uint32_t year, const uint32_t month = 1, const uint32_t day = 1, 
       const uint32_t hour = 0, const uint32_t minute = 0, const uint32_t second = 0,
       const uint32_t muSecond = 0); 

  /// Constructs a date from days, seconds, microseconds
  CDate(const uint32_t days, const uint32_t seconds, const uint32_t muSeconds,
        const brathl_refDate refDate = REF19500101);

  /// Constructs a date from days, seconds, microseconds
  CDate(const double days, const double seconds, const double muSeconds,
        const brathl_refDate refDate = REF19500101);

  /** Constructs a date value from a decimal number of seconds  
     \param dateSeconds [in]: decimal number of seconds
     \param refDate [in]: date reference (default value is REF19500101 - see #brathl_refDate)
   */
  CDate(const double dateSeconds, brathl_refDate refDate = REF19500101);
  
  CDate(brathl_refDate refDate);

  virtual ~CDate();

public:

  /** Initializes a CDate object to 0 */
  void InitDateZero();

  /// Gets the year of the date
  uint32_t GetYear() const;

  /// Gets the month of the date
  uint32_t GetMonth();

  /// Gets the day of the date
  uint32_t GetDay();

  /// Gets the hour of the date
  uint32_t GetHour();

  /// Gets the minutes of the date
  uint32_t GetMinute();

  /// Gets the seconds of the date
  uint32_t GetSecond();

  /// Gets the microseconds of the date 
  uint32_t GetMuSecond();
  
  /// returns the date in a number of seconds since internal reference date, ie 1950)
  double Value() {return GetNbSeconds();}
  
  /// returns the date in a decimal julian day (since internal reference date, ie 1950)
  double ValueJulian();

  /**  Formats a date as std::string.
     \param Format [in] : String controlling how the date
	will be converted into std::string.
	This format std::string consists of zero or more conversion specifications
	and ordinary characters. A conversion specification consists of a 
	'%' (percent) character and one or two terminating conversion
	characters that determine the conversion specification's
	behavior. All ordinary characters are copied unchanged into the
	result.
	Each conversion specification is replaced by appropriate characters as
	described in the following list. The appropriate characters are
	determined by the LC_TIME category of the program's locale.
	%%    Same as %.
	%a    Locale's abbreviated weekday name.
	%A    Locale's full weekday name.
	%b    Locale's abbreviated month name.
	%B    Locale's full month name.
	%c    Locale's appropriate date and time representation.
	%C    Century number (the year divided by 100 and truncated
	      to an integer as a decimal number [1,99]); single
	      digits are preceded by 0; see standards(5).
	%d    Day of month [1,31]; single digits are preceded by 0.
	%H    Hour (24-hour clock) [0,23]; single digits are preceded by 0.
	%I    Hour (12-hour clock) [1,12]; single digits are preceded by 0.
	%j    Day number of year [1,366]; single digits are preceded by 0.
	%m    Month number [1,12]; single digits are preceded by 0.
	%M    Minute [00,59]; leading 0 is permitted but not required.
	%p    Locale's equivalent of either a.m. or p.m.
	%S    Seconds [00,61]; the range of values is [00,61] rather
	      than [00,59] to allow for the occasional leap second
	      and even more occasional double leap second.
	%U    Week number of year as a decimal number [00,53], with
	      Sunday as the first day of week 1.
	%w    Weekday as a decimal number [0,6], with 0 representing Sunday.
	%W    Week number of year as a decimal number [00,53], with
	      Monday as the first day of week 1.
	%x    Locale's appropriate date representation.
	%X    Locale's appropriate time representation.
	%y    Year within century [00,99].
	%Y    Year, including the century (for example 1993).
	%Z    Time zone name or abbreviation, or no bytes if no time
	      zone information exists.
	If the format is an empty std::string it is forced to be "%Y-%m-%d %H:%M:%S"
	(ISO 8601)
        \param withMuSecond [in] : add the microseconds of the date at the end of the std::string
        (format ".%06u")
	\return Formatted std::string */
  std::string AsString(const std::string& format = "",  bool withMuSecond = false);

  /**  Convert a date std::string to a number of seconds since internal reference year (ie 1950)
    Allowed format are  :
      - YYYY-MM-DD HH:MN:SS.MS std::string
      - a julian std::string (format:positive 'Days Seconds Microseconds' or positive decimal julian day)
    For julian std::string,  it can contain its date reference at the end by specifying @YYYY where YYYY the reference year.
    If no date reference is specified the default date reference is used.
     \param strDate : date std::string 
     \return number of seconds since internal reference year (ie 1950) */
  static double CvDate(const char *strDate);

  /**  Sets date value from a std::string 
    Allowed format are  :
      - YYYY-MM-DD HH:MN:SS.MS std::string
      - a julian std::string (format:positive 'Days Seconds Microseconds' or positive decimal julian day)
    For julian std::string,  it can contain its date reference at the end by specifying @YYYY where YYYY the reference year.
    If no date reference is specified the default date reference is used.
     \param strDate : date std::string 
     \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
  int32_t SetDate(const char *strDate);

  /** Sets date value from a #brathl_DateYMDHMSM structure 
     \param date [in]: #brathl_DateYMDHMSM structure date
     \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
  int32_t SetDate(const brathl_DateYMDHMSM &date);

  /** Sets date value from a #brathl_DateDSM structure 
     \param date [in]: #brathl_DateDSM structure date
     \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
  int32_t SetDate(const brathl_DateDSM &date);  

  /** Sets date value from year, month, day, hour, minute, second, microsecond
     \param days [in]: number of days
     \param seconds [in]: number of seconds
     \param muSeconds [in]: number of microseconds
     \param refDate [in]: date reference (default value is REF19500101 - see #brathl_refDate)
     \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
  int32_t SetDate(const uint32_t days, const uint32_t seconds, const uint32_t muSeconds,
      	      	      const brathl_refDate refDate = REF19500101);

  int32_t SetDate(const double days, const double seconds, const double muSeconds,
      	      	      const brathl_refDate refDate = REF19500101);

  /** Sets date value from a #brathl_DateSecond structure 
     \param date [in]: #brathl_DateSecond structure date
     \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
  int32_t SetDate(const brathl_DateSecond &date);  

  /** Sets date value from a #brathl_DateJulian structure 
     \param date [in]: #brathl_DateJulian structure date
     \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
  int32_t SetDate(const brathl_DateJulian &date);  

  /** Sets date value from year, month, day, hour, minute, second, microsecond
     \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
  int32_t SetDate(const uint32_t year, const uint32_t month = 1, const uint32_t day = 1, 
      	      	  const uint32_t hour = 0, const uint32_t minute = 0, const uint32_t second = 0,
      	      	  const uint32_t muSecond = 0); 

  /** Sets date value from a decimal number of seconds  
     \param dateSeconds [in]: decimal number of seconds
     \param refDate [in]: date reference (default value is REF19500101 - see #brathl_refDate)
     \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
  int32_t SetDate(const double dateSeconds, brathl_refDate refDate = REF19500101); 
  
  /** Sets date value from a decimal julian day 
     \param dateJulian [in]: decimal julian day
     \param refDate [in]: date reference (default value is REF19500101 - see #brathl_refDate)
     \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
  int32_t SetDateJulian(const double dateJulian, brathl_refDate refDate = REF19500101);
  
  /** Sets internal value to the default value */
  void SetDefaultValue();

  /** Tests the internal value to the default value 
     \return true if default value, otherwise false */
  bool IsDefaultValue() const;


  /** Converts the date value into a number of seconds, microseconds
     \param seconds [out]: number of milliseconds (can be < 0)
     \param muSeconds [out]: number of microseconds 
     \param refDate [in]: date reference (default value is REF19500101 - see #brathl_refDate)
     \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
  int32_t Convert2SM(int32_t& seconds, int32_t& muSeconds, const brathl_refDate refDate = REF19500101);
  
  /** Converts the date value into a number of seconds, microseconds
     \param seconds [out]: number of milliseconds (can be < 0)
     \param muSeconds [out]: number of microseconds 
     \param refDate [in]: date reference (default value is REF19500101 - see #brathl_refDate)
     \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
  int32_t Convert2SM(double& seconds, double& muSeconds, const brathl_refDate refDate = REF19500101);
  

  /** Converts the date value into a number of days, milliseconds, microseconds
     \param days [out]: number of days (can be < 0)
     \param milliSeconds [out]: number of milliseconds
     \param muSeconds [out]: number of microseconds
     \param refDate [in]: date reference (default value is REF19500101 - see #brathl_refDate)
     \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
  int32_t Convert2DMM(int32_t& days, int32_t& milliSeconds, int32_t& muSeconds, const brathl_refDate refDate = REF19500101);
  
  /** Converts the date value into a number of days, milliseconds, microseconds
     \param days [out]: number of days (can be < 0)
     \param milliSeconds [out]: number of milliseconds
     \param muSeconds [out]: number of microseconds
     \param refDate [in]: date reference (default value is REF19500101 - see #brathl_refDate)
     \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
  int32_t Convert2DMM(double& days, double& milliSeconds, double& muSeconds, const brathl_refDate refDate = REF19500101);


  /** Converts the date value into a number of days, seconds, microseconds
     \param days [out]: number of days (can be < 0)
     \param seconds [out]: number of seconds
     \param muSeconds [out]: number of microseconds
     \param refDate [in]: date reference (default value is REF19500101 - see #brathl_refDate)
     \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
  int32_t Convert2DSM(int32_t& days, int32_t& seconds, int32_t& muSeconds, const brathl_refDate refDate = REF19500101);

  /** Converts the date value into a number of days, seconds, microseconds
     \param days [out]: number of days (can be < 0)
     \param seconds [out]: number of seconds
     \param muSeconds [out]: number of microseconds
     \param refDate [in]: date reference (default value is REF19500101 - see #brathl_refDate)
     \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
  int32_t Convert2DSM(double& days, double& seconds, double& muSeconds, const brathl_refDate refDate = REF19500101);
  
  
  /** Converts the date value into a decimal julian day
     \param julian [out]: decimal julian day (can be < 0)
     \param refDate [in]: date reference (default value is REF19500101 - see #brathl_refDate)
     \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
  int32_t Convert2DecimalJulian(double& julian, const brathl_refDate refDate = REF19500101);

  /** Converts the date value into a decimal number of seconds
     \param seconds [out]: decimal number of seconds day (can be < 0)
     \param refDate [in]: date reference (default value is REF19500101 - see #brathl_refDate)
     \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
  int32_t Convert2Second(double& seconds, const brathl_refDate refDate = REF19500101);
  
  /** Converts the date value into year, month, day, hour, minute, second, microsecond
     \param year [out]: year
     \param month [out]: month
     \param day [out]: day
     \param hour [out]: hour
     \param minute [out]: minute
     \param second [out]: second
     \param muSecond [out]: microsecond
     \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
  int32_t Convert2YMDHMSM(uint32_t& year, uint32_t& month, uint32_t& day, 
      	      	      	  uint32_t& hour, uint32_t& minute, uint32_t& second,
                          uint32_t& muSecond) const;

  /** Converts a date whose value corresponds to the date reference enumeration
     \param refDate [in]: date reference - see #brathl_refDate)
     \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
  int32_t ConstructDate(const brathl_refDate refDate);  
  
  /** Testd if the year is a leap year
     \param year [in]: year to test
     \return true if the year is a leap year, otherwise false */
  static bool IsLeapYear(const uint32_t year);
  /** Tests if the year of the date object is a leap year
     \return true if the year of the date object is a leap year, otherwise false */
  bool IsLeapYear();
  
  /** Retrieves the index of the #m_daysOfYear or #m_daysInMonth arrays in accordance with the year (leap year or not)
     \param year [in]: year to test
     \return 0 if year is a leap year, otherwise 1  */
  static int32_t LeapYearIndex(const uint32_t year);

  /** Retrieve sthe index of the daysOfYear or daysInMonth arrays in accordance with the year of the date object (leap year or not)
     \return 0 if year of the date object is a leap year, otherwise 1 */
  int32_t LeapYearIndex();

  /** Retrieves the day of a year
      if year is not valid, methods force the value to the internal reference year (1950)
      if month is not valid, methods force the value to 1
      day value is not check
     \param year [in]: year
     \param month [in]: month of year
     \param day [in]: day of the month
     \return the day of year  */
  static uint32_t DayOfYear(uint32_t year, uint32_t month, uint32_t day);
  
  /** Retrieves the day of year of a CDate object 
     \param date [in]: date
     \return the day of year  */
  static uint32_t DayOfYear(CDate& date);
  
  /** Retrieves the day of year of the date object 
     \return the day of year  */
  uint32_t DayOfYear();

  /** Sets the date object to the current time
     \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
  int32_t SetDateNow();
  
  /** Computes the number of leap years since a year 
     \param year [in]: year
     \return number of leap years  */
  uint32_t HowManyLeapYear(const uint32_t year) const;
  
  /** Retrieves the number of days in a month, according to a year and a month
    \param month [in] : month
    \param year [in] : year
    \param nbDaysInMonth[out] : number of days in the month
     \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
  static int32_t GetDaysInMonth(const uint32_t month, const uint32_t year, uint32_t &nbDaysInMonth);  

  /** Check if a date value (year, month, day, hour, minute, second, microsecond ) is valid
     \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
  static int32_t CheckDate(const uint32_t year, const uint32_t month = 1, const uint32_t day = 1, 
      	      	    const uint32_t hour = 0, const uint32_t minute = 0, const uint32_t second = 0,
      	      	    const uint32_t muSecond = 0);

  /** Checks if a year value is valid
      year have to be >= internal reference year (1950)
     \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
  static int32_t CheckYear(uint32_t year);
  
  /** Checks if a month value is valid
     \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
  static int32_t CheckMonth(uint32_t month);
  
  /** Checks if a day value is valid, according to a month an a year
     \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
  static int32_t CheckDay(uint32_t day, uint32_t month, uint32_t year);
  
  /** Checks if an hour value is valid
     \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
  static int32_t CheckHour(uint32_t hour);
  
  /** Checks if a minute is valid
     \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
  static int32_t CheckMinute(uint32_t minute);
  
  /** Checks if a second value is valid
     \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
  static int32_t CheckSecond(uint32_t second);
  
  /** Checks if a month value is valid
     \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
  static int32_t CheckMuSecond(uint32_t muSecond);
  
  /** Adds a number of day to the date object
     \param days [in]: number of days to add (if < 0, a subtract operation is performed)
     \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
  int32_t AddDays(uint32_t days);
  
  /** Subtracts a number of day from the date object
     \param days [in]: number of days to subtract (if < 0, a addition operation is performed)
     \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
  int32_t SubtractDays(uint32_t days);
  
  /** Adds a date to the date object
     \param d [in]: a CDate object to add
     \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
  int32_t Add(const CDate& d);
  

  static bool IsCharDate(const char *strDate);

  /** Minus operator redefinition
      Computes the difference between two dates,
      the result is expressed in a decimal number of seconds */
  double operator- ( CDate &d ) { return this->Value() - d.Value(); }
  
  /** Plus operator redefinition
      Computes the addition of two dates,
      the result is expressed in a decimal number of seconds */
  double operator+ ( CDate &d ) { return this->Value() + d.Value(); }

  /** Assigns a new value to the CDate object, with a CDate object */
  const CDate& operator= (const CDate& date);

  /** Assigns a new value to the CDate object, with a date std::string (format: YYYY-MM-DD HH:MN:SS.MS) */
  const CDate& operator= (const char *strDate);

  /** Assigns a new value to the CDate object, with a number of seconds since  1950-01-01 */
  const CDate& operator =(double seconds);
  
  /** Assigns a new value to the CDate object, with a reference date */
  const CDate& operator =(const brathl_refDate refDate);

  //@{
  /** Comparison operators */
  bool operator< ( CDate & d )  { return this->Value() < d.Value(); }
  bool operator< ( double d )  { return this->Value() < d; }
  bool operator> ( CDate & d )  { return this->Value() > d.Value(); }
  bool operator> ( double d )  { return this->Value() > d; }
  bool operator== ( CDate & d ) { return this->Value() == d.Value(); }
  bool operator== ( double d ) { return this->Value() == d; }
  bool operator<= ( CDate & d ) { return ! (*this > d); }
  bool operator<= ( double d ) { return ! (*this > d); }
  bool operator>= ( CDate & d ) { return ! (*this < d); }
  bool operator>= ( double d ) { return ! (*this < d); }
  bool operator!= ( CDate & d ) { return ! (*this == d); }
  bool operator!= ( double d ) { return ! (*this == d); }
  //@}


  
  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

private:
	
  /** Tests the internal value to the default value 
     \return true if default value, otherwise false */
  bool IsDefaultValueYMDHMSM(const uint32_t year, const uint32_t month = 1, const uint32_t day = 1, 
      	      	             const uint32_t hour = 0, const uint32_t minute = 0, const uint32_t second = 0,
      	      	             const uint32_t muSecond = 0);


  /** Decodes a date from a std::string (format: YYYY-MM-DD HH:MN:SS.MS)
    \param strDate [in] : date to decode
     \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
  int32_t DecodeDateYMDHMSM(const char *strDate);
  
  /** Decodes a julian date from a std::string (format: 'Days Seconds Microseconds' or decimal julian day.) 
    The std::string can contain its date reference at the en by specifying @YYYY where YYYY the reference year.
    If no date reference is specified the default date reference is used.
    \param strDate [in] : date to decode
     \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
  int32_t DecodeDateDSM(const char *strDate);

  /** Extract a date reference from a std::string
     Date reference is specified by std::string '@YYYY' 
     where YYYY the reference year (so value of date reference is the first day of the year) 
    \param strDate [in/out] : date std::string with its reference, after calling methode date is altered
     : date reference is eliminate from date std::string
    \param dateRef [out]: date reference
    \param hasRef [out]: indcates a date reference has benne found in strDate (true), otherwise hasRef is false 
    \return #BRATHL_SUCCESS if date reference is valid or date std::string has no reference, 
    otherwise returns error code (see \ref Date_error_codes) */
  int32_t ExtractRef(char *strDate, CDate& dateRef, bool& hasRef);

  /** Converts a date a date (year, month, day, hour, minute, second, microsecond) to the internal format
    \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
  int32_t YMDHMSM2Second(const uint32_t year, const uint32_t month = 1, const uint32_t day = 1, 
      	      	         const uint32_t hour = 0, const uint32_t minute = 0, const uint32_t second = 0,
      	      	         const uint32_t muSecond = 0);
   
  /** Converts a date (numbers of days, seconds, microseconds) to the internal format
    \param days [in] : number of days
    \param seconds [in] : number of seconds
    \param muSeconds [in] : number of muSeconds
    \param refDate [in]: date reference  (default value is REF19500101 - see #brathl_refDate)
    \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
  int32_t DSM2Second(const uint32_t days, const uint32_t seconds, const uint32_t muSeconds,
      	      	     const brathl_refDate refDate = REF19500101);

  /** Gets a CDate object according to a date reference enumeration
    \param refDate [in]: date reference (default value is REF19500101 - see #brathl_refDate)
    \param date [out] : CDate object whose value corresponds to the refDate parameter
    \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
  int32_t GetDateRef(const brathl_refDate refDate, CDate& date);

public:
  /** Construct a date reference enumeration according to a CDate object 
    Only date according to #brathl_refDate enumeration are valid, 
    furthermore REFUSER1 and REFUSER2 are not allowed.
    \param date [int] : CDate object whose value corresponds to the refDate parameter
    \param refDate [out]: date reference enumeration value (see #brathl_refDate)
    \return #BRATHL_SUCCESS if CDate object is according to #brathl_refDate enumeration
     except REFUSER1 and REFUSER2. Otherwise returns a erro code (see \ref Date_error_codes) */
  static int32_t GetDateRef(const CDate& date, brathl_refDate& refDate);

private:
  /** Retrieves the date value in a decimal number of seconds (since internal reference date, ie 1950) */
  double GetNbSeconds(); 
  
  /** Adjusts the internal format units
      by converting the minutes contained in the seconds  */
  void Adjust();


public :
  
  /** Number of seconds in a day */
  static const double m_secInDay;
  /** Number of seconds in an hour */
  static const double m_secInHour;
  /** Number of seconds in a minute */
  static const double m_secInMinute;
  /** Number of minutes in a day */
  static const double m_minutesInDay;
  /** Number of minutes in an hour */
  static const double m_minutesInHour;
  /** Array[i,j] of day of year 
    i : 0 corresponds to a leap year, 1 corresponds to a non-leap year
    j : index of the month */
  static const uint32_t m_daysOfYear[2][12];
  /** Array[i,j] of number of days in month 
    i : 0 corresponds to a leap year, 1 corresponds to a non-leap year
    j : index of the month */
  static const uint32_t m_daysInMonth[2][12];
  /** Internal reference year  (1950) */
  static const uint32_t m_internalRefYear;

  static const char* m_DEFAULT_UNIT_SECOND;

private :

  
  /** Number of minutes of the date */
  uint32_t m_minutes;
  /** Number of microsesonds of the date */
  uint32_t m_muSeconds;



};                      

/** @} */

}
#endif // !defined(_Date_h_)
