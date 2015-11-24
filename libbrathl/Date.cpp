
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

#include <cstdio>

//#include <sys/types.h>
//#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <ctime>

#include "new-gui/Common/tools/brathl_error.h"
#include "brathl.h"

#include "new-gui/Common/tools/Exception.h"
#include "new-gui/Common/tools/TraceLog.h"
#include "Tools.h"
#include "Date.h"
using namespace brathl;

extern "C" {

LIBRATHL_API char brathl_refDateUser1[BRATHL_REF_DATE_USER_LEN];
LIBRATHL_API char brathl_refDateUser2[BRATHL_REF_DATE_USER_LEN];

}


namespace brathl
{

const double CDate::m_secInDay = 86400.0;
const double CDate::m_secInHour = 3600.0;
const double CDate::m_secInMinute = 60.0;
const double CDate::m_minutesInDay = 1440.0;
const double CDate::m_minutesInHour = 60.0;
const uint32_t CDate::m_internalRefYear = 1950;

const char* CDate::m_DEFAULT_UNIT_SECOND = "second";

/* Days of year of the beagining of each month for leap and not leap years */
const uint32_t CDate::m_daysOfYear[2][12]	=
	{
		{0,31,60,91,121,152,182,213,244,274,305,335},
		{0,31,59,90,120,151,181,212,243,273,304,334}
	};



/* Number of days in each month for leap and not leap years */
const uint32_t CDate::m_daysInMonth[2][12]	=
	{
		{31,29,31,30,31,30,31,31,30,31,30,31},
		{31,28,31,30,31,30,31,31,30,31,30,31}
	};



//================================================================

//ctor
CDate::CDate()
{
  InitDateZero();

}

//----------------------------------------
CDate::CDate(const char *strDate)
{
  SetDate(strDate);
  //DecodeDateYMDHMSM(strDate);
}

//----------------------------------------
CDate::CDate(const CDate& date)
{

  //m_nbSeconds = date.m_nbSeconds;
  m_minutes = date.m_minutes;
  m_muSeconds = date.m_muSeconds;


}

//----------------------------------------
CDate::CDate(const uint32_t year, const uint32_t month /*= 1*/, const uint32_t day /*= 1*/,
      	      	       const uint32_t hour /*= 0*/, const uint32_t minute /*= 0*/, const uint32_t second /*= 0*/,
      	      	       const uint32_t muSecond /*= 0*/)
{
  SetDate(year, month, day, hour, minute, second, muSecond);
}

//----------------------------------------
CDate::CDate(const uint32_t days, const uint32_t seconds, const uint32_t muSeconds,
      	      	      const brathl_refDate refDate /* = REF19500101*/)
{
  SetDate(days,
	        seconds,
	        muSeconds,
      	  refDate);
}
//----------------------------------------
CDate::CDate(const double days, const double seconds, const double muSeconds,
      	      	      const brathl_refDate refDate /* = REF19500101*/)
{
  SetDate(days,
	        seconds,
	        muSeconds,
      	  refDate);
}

//----------------------------------------
CDate::CDate(const double dateSeconds, brathl_refDate refDate /*= REF19500101*/)
{
  SetDate(dateSeconds,
	        refDate);
}
//----------------------------------------
CDate::CDate(brathl_refDate refDate)
{
  SetDate(0,
	        refDate);
}
//----------------------------------------
//dtor
CDate::~CDate()
{

}

//----------------------------------------
void CDate::InitDateZero()
{
  m_minutes = 0;
  m_muSeconds = 0;
}

//----------------------------------------
double CDate::GetNbSeconds()
{
  double value = 0.0;
  if (IsDefaultValue())
  {
    setDefaultValue(value);
  }
  else
  {
    value = (static_cast<double>(m_minutes) * CDate::m_secInMinute)
      	+  (static_cast<double>(m_muSeconds) / 1000000.0);
  }

 return value;
}

//----------------------------------------
const CDate& CDate::operator =(const brathl_refDate refDate)
{

  SetDate(0, refDate);

  return *this;


}

//----------------------------------------
const CDate& CDate::operator =(const CDate& date)
{

  //m_nbSeconds = date.m_nbSeconds;
  m_minutes = date.m_minutes;
  m_muSeconds = date.m_muSeconds;

  return *this;


}

//----------------------------------------
const CDate& CDate::operator =(const char *strDate)
{

  SetDate(strDate);
  //DecodeDateYMDHMSM(strDate);

  return *this;
}
//----------------------------------------
const CDate& CDate::operator =(double seconds)
{

  SetDate(seconds);
  //DecodeDateYMDHMSM(strDate);

  return *this;
}

//----------------------------------------
double CDate::ValueJulian()
{
  double julian;
  Convert2DecimalJulian(julian);
  return julian;
}

//----------------------------------------
std::string CDate::AsString(const std::string& format /*= ""*/, bool withMuSecond /* = false */)
{
  size_t	strSize	= format.length()*10;
  std::string	result = "";

  if (strSize == 0)
  {
    strSize	= 50;
  }

  char* formatted	= new char[strSize];
  memset(formatted, '\0', strSize);

  try
  {
    struct tm	Time;
    uint32_t	muSecond = 0;
    uint32_t    year = 0;
    uint32_t  	month = 0;
    uint32_t  	day = 0;
    uint32_t  	hour = 0;
    uint32_t  	minute = 0;
    uint32_t  	second = 0;

    size_t	nbWrittenChars;
    /*
    Convert2YMDHMSM(uint32_t(Time.tm_year),
		    uint32_t(Time.tm_mon),
		    uint32_t(Time.tm_mday),
		    uint32_t(Time.tm_hour),
		    uint32_t(Time.tm_min),
		    uint32_t(Time.tm_sec),
		    muSecond);
		  */

    Convert2YMDHMSM(year, month, day, hour, minute, second, muSecond);

    if (IsDefaultValue())
    {
      delete []formatted;
      return result;
    }

    Time.tm_year = year;
    Time.tm_mon = month;
    Time.tm_mday = day;
    Time.tm_hour = hour;
    Time.tm_min = minute;
    Time.tm_sec = second;

    Time.tm_year	-= 1900;
    Time.tm_mon--;

    nbWrittenChars = strftime(formatted,
				                    strSize,
				                    (format == "" ? "%Y-%m-%d %H:%M:%S" : format.c_str()),
				                    &Time);

    if (nbWrittenChars == 0)
    {
      *formatted	= '\0';
    }

    if (withMuSecond)
    {
      result = CTools::Format(strSize + 20, "%s.%06u", formatted, muSecond);
    }
    else
    {
      result = formatted;
    }
  }
  catch (...)
  {
    delete []formatted;
    throw;
  }

  delete []formatted;
  return result;
}


//----------------------------------------

bool CDate::IsLeapYear(const uint32_t year)
{

  return ((((year % 4) == 0) && ((year % 100) != 0)) || (year % 400) == 0);

}


bool CDate::IsLeapYear()
{

  return CDate::IsLeapYear(this->GetYear());

}


//----------------------------------------

int32_t CDate::LeapYearIndex(const uint32_t year)
{
  int32_t index = 1;

  if (CDate::IsLeapYear(year) == true)
  {
    index = 0;
  }

  return index;

}

//----------------------------------------
int32_t CDate::LeapYearIndex()
{
  return CDate::LeapYearIndex(this->GetYear());
}

//----------------------------------------

uint32_t CDate::DayOfYear(uint32_t year, uint32_t month, uint32_t day)
{
  if (CDate::CheckYear(year) != BRATHL_SUCCESS)
  {
    year = CDate::m_internalRefYear;
  }
  if (CDate::CheckMonth(month) != BRATHL_SUCCESS)
  {
    month = 1;
  }
  return (CDate::m_daysOfYear[CDate::LeapYearIndex(year)][month - 1] + day);

}


//----------------------------------------

uint32_t CDate::DayOfYear(CDate& date)
{

  return (CDate::m_daysOfYear[date.LeapYearIndex()][date.GetMonth() - 1] + date.GetDay());

}


//----------------------------------------

uint32_t CDate::DayOfYear()
{
  return CDate::DayOfYear(*this);
}

//----------------------------------------

int32_t CDate::SetDateNow()
{
  int32_t result = BRATHL_SUCCESS;

  time_t now;

  time(&now);

  struct tm *l_time;
  l_time = localtime(&now);

  uint32_t year = l_time->tm_year + 1900;
  uint32_t month = l_time->tm_mon + 1;
  uint32_t day = l_time->tm_mday;
  uint32_t hour = l_time->tm_hour;
  uint32_t minute = l_time->tm_min;
  uint32_t second =  l_time->tm_sec;
  uint32_t muSecond = 0;

  result = YMDHMSM2Second(year, month, day, hour, minute, second, muSecond);


  return result;

}


//----------------------------------------

double CDate::CvDate(const char *strDate)
{
  CDate date;
  int32_t result = date.SetDate(strDate);

  if (result !=  BRATHL_SUCCESS)
  {
    std::string msg = CTools::Format("ERROR - CDate::CvDate - Invalid date '%s' ",
                                 strDate);
    CException e(msg, BRATHL_INCONSISTENCY_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);
  }

  return date.Value();
}

//----------------------------------------

bool CDate::IsCharDate(const char *strDate)
{

  return isDefaultValue( CTools::StrToDouble(strDate) ) ;
}

//----------------------------------------

int32_t CDate::SetDate(const char *strDate)
{
  int32_t result = BRATHL_SUCCESS;

  if (CTools::IsEmpty(strDate))
  {
    SetDefaultValue();
    return result;
  }
  if (str_icmp(strDate, "dv"))
  {
    SetDefaultValue();
    return result;
  }

  int32_t nbFields = 0;

  uint32_t year = 0;
  uint32_t month = 0;
  uint32_t day = 0;
  uint32_t hour = 0;
  uint32_t minute = 0;
  uint32_t second = 0;
  uint32_t muSecond = 0;

  bool isYMDHMSM = false;
  bool isDSM = false;

  const char *current = strDate;
  while (*current)
  {
    switch(*current)
    {
    case '-' :
    case ':' :
      isYMDHMSM = true;
      break;
    case '@' :
      isDSM = true;
      break;
    case '.' :
      isDSM = true;
      isYMDHMSM = true;
      break;
    default: break;
    }


    // next char.
    current++;

  }

  // if YMDHMSM and DSM char. found
  if ((isYMDHMSM == true) && (isDSM == true)) // both are true if '.' char is found.
  {
    // if not a valid  YMDHMSM, try if a YMDHMSM without '-' and ':' and '.' char.
    nbFields = sscanf(strDate, "%d%*c%d%*c%d%*c"
				       "%d%*c%d%*c%d%*c%d",
			   &year, &month, &day,
      	      	      	   &hour, &minute, &second, &muSecond);

    if (nbFields == 7) //it's a complete YMDHMSM date
    {
      isDSM = false;
    }
    // Is it a valid decimal julian date (xxx.yyy)
    else if (nbFields > 2) //we found a '.' char, but it's not a valid decimal julian date
    {
      return BRATHL_ERROR_INVALID_DATE;
    }
    else
    {
      isYMDHMSM = false;
    }

  }

  if (isDSM == true)
  {
    result = DecodeDateDSM(strDate);
    return result;
  }

  if (isYMDHMSM == true)
  {
    result = DecodeDateYMDHMSM(strDate);
    return result;
  }

   // if not a valid  YMDHMSM, try if a YMDHMSM without '-' and ':' and '.' char.
  nbFields = sscanf(strDate, "%d%*c%d%*c%d%*c"
				     "%d%*c%d%*c%d%*c%d",
			 &year, &month, &day,
      	      	      	 &hour, &minute, &second, &muSecond);

  if (nbFields >= 4)
  {
    result = DecodeDateYMDHMSM(strDate);
    return result;
  }


  // if not a YMDHMSM, try if a julian date
  result = DecodeDateDSM(strDate);


  return result;

}


//----------------------------------------
int32_t CDate::SetDate(const brathl_DateYMDHMSM &date)
{
  int32_t result = BRATHL_SUCCESS;

  result = SetDate(date.year,
		   date.month,
		   date.day,
		   date.hour,
		   date.minute,
		   date.second,
		   date.muSecond);

  return result;
}

//----------------------------------------
int32_t CDate::SetDate(const brathl_DateDSM &date)
{
  int32_t result = BRATHL_SUCCESS;

  if ( (date.days < 0) ||
       (date.seconds < 0) ||
       (date.muSeconds < 0))
  {
    return BRATHL_ERROR_INVALID_DSM;
  }
  result = DSM2Second(date.days,
		   date.seconds,
		   date.muSeconds,
      	      	   date.refDate);

  return result;
}
//----------------------------------------
int32_t CDate::SetDate(const uint32_t days, const uint32_t seconds, const uint32_t muSeconds,
      	      	      const brathl_refDate refDate /* = REF19500101*/)
{
  return DSM2Second(days,
		   seconds,
		   muSeconds,
      	      	   refDate);
}

//----------------------------------------
int32_t CDate::SetDate(const double days, const double seconds, const double muSeconds,
      	      	      const brathl_refDate refDate /* = REF19500101*/)
{
  return SetDate(static_cast<uint32_t>(days),
	               static_cast<uint32_t>(seconds),
 	               static_cast<uint32_t>(muSeconds),
      	      	 refDate);
}

//----------------------------------------
int32_t CDate::SetDate(const brathl_DateSecond &date)
{
  int32_t result = SetDate(date.nbSeconds, date.refDate);
  return result;
}


//----------------------------------------

int32_t CDate::SetDate(const double dateSeconds, brathl_refDate refDate /*= REF19500101*/)
{

  int32_t result = BRATHL_SUCCESS;

  if (isDefaultValue(dateSeconds))
  {
    SetDefaultValue();
    return result;
  }

  CDate dateRef;

  result = GetDateRef(refDate, dateRef);
  if (result !=  BRATHL_SUCCESS)
  {
    return result;
  }

  if (dateSeconds < 0)
  {
    double nbSeconds = dateRef.GetNbSeconds() + dateSeconds;
    if (nbSeconds < 0)
    {
      return BRATHL_ERROR_INVALID_YEAR;
    }
    else
    {
      return this->SetDate(nbSeconds);
    }
  }


  m_minutes = static_cast<uint32_t>(dateSeconds / CDate::m_secInMinute);
  double muSecondsTmp = CTools::Round((dateSeconds - (m_minutes * CDate::m_secInMinute)) * 1.0E+6);
  m_muSeconds = static_cast<uint32_t>(muSecondsTmp);

  Add(dateRef);

  return result;
}



//----------------------------------------

int32_t CDate::SetDate(const brathl_DateJulian &date)
{
  int32_t result = SetDateJulian(date.julian, date.refDate);

  return result;
}


//----------------------------------------
int32_t CDate::SetDateJulian(const double dateJulian, brathl_refDate refDate/*= REF19500101*/)
{
  int32_t result = BRATHL_SUCCESS;


  if (isDefaultValue(dateJulian) == true)
  {
    return BRATHL_SUCCESS;
  }

  SetDefaultValue();

  CDate dateRef;
  result = GetDateRef(refDate, dateRef);
  if (result !=  BRATHL_SUCCESS)
  {
    return result;
  }


  if (dateJulian < 0)
  {
    double julianTmp = dateRef.ValueJulian() + dateJulian;
    if (julianTmp < 0)
    {
      return BRATHL_ERROR_INVALID_YEAR;
    }
    else
    {
      return this->SetDateJulian(julianTmp);
    }
  }

  m_minutes += static_cast<uint32_t>(dateJulian * CDate::m_minutesInDay);
  double muSecondsTmp = CTools::Round(((dateJulian * CDate::m_minutesInDay) - m_minutes) * CDate::m_secInMinute * 1.0E+6);
  m_muSeconds = static_cast<uint32_t>(muSecondsTmp);

  Add(dateRef);
  //m_minutes += dateRef.m_minutes;
  //m_muSeconds += dateRef.m_muSeconds;

  return result;

}


//----------------------------------------
int32_t CDate::SetDate(const uint32_t year, const uint32_t month /*= 1*/, const uint32_t day /*= 1*/,
      	      	       const uint32_t hour /*= 0*/, const uint32_t minute /*= 0*/, const uint32_t second /*= 0*/,
      	      	       const uint32_t muSecond /*= 0*/)
{

  int32_t result = BRATHL_SUCCESS;

  result = YMDHMSM2Second(year, month, day, hour, minute, second, muSecond);

  return result;

}

//----------------------------------------
int32_t CDate::ConstructDate(const brathl_refDate refDate)
{

  int32_t result = BRATHL_SUCCESS;

  switch (refDate)
  {
    case REF19500101 :
    {
      break;
    }
    case REF19580101 :
    {
      this->SetDate(1958U);
      break;
    }
    case REF19850101 :
    {
      this->SetDate(1985U);
      break;
    }
    case REF19900101 :
    {
      this->SetDate(1990U);
      break;
    }
    case REF20000101 :
    {
      this->SetDate(2000U);
      break;
    }
    case REFUSER1 :
    {
      result = this->SetDate(brathl_refDateUser1);
      if (result !=  BRATHL_SUCCESS)
      {
      	return BRATHL_ERROR_INVALID_DATE_REF;
      }
      break;
    }
    case REFUSER2 :
    {
      result = this->SetDate(brathl_refDateUser2);
      if (result !=  BRATHL_SUCCESS)
      {
      	return BRATHL_ERROR_INVALID_DATE_REF;
      }
      break;
    }
    default :
    {
      result = BRATHL_ERROR_INVALID_DATE_REF;
    }
  }

  return result;
}


//----------------------------------------
int32_t CDate::GetDateRef(CDate& date, brathl_refDate& refDate)
{
  int32_t result = BRATHL_SUCCESS;

  switch (date.GetYear())
  {
    case 1950 :
    {
      refDate =  REF19500101;
      break;
    }
    case 1958 :
    {
      refDate =  REF19580101;
      break;
    }
    case 1985 :
    {
      refDate =  REF19850101;
      break;
    }
    case 1990 :
    {
      refDate =  REF19900101;
      break;
    }
    case 2000 :
    {
      refDate =  REF20000101;
      break;
    }
    default :
    {
      result = BRATHL_ERROR_INVALID_DATE_REF;
    }
  }

  return result;
}
//----------------------------------------
int32_t CDate::GetDateRef(const brathl_refDate refDate, CDate& date)
{

  int32_t result = BRATHL_SUCCESS;

  result = date.ConstructDate(refDate);

  if (result != BRATHL_SUCCESS)
  {
    return BRATHL_ERROR_INVALID_DATE_REF;
  }

  // Test if date can be converted to the reference date (refDate)
/*  if (this->Value() < date.Value())
  {
    return BRATHL_ERROR_INVALID_DATE_REF_CONV;
  }
*/
  return result;
}
//----------------------------------------

uint32_t CDate::HowManyLeapYear(const uint32_t year)
{
  uint32_t nbLeapYear = 0;

//  for (uint32_t i = year - 1 ; i > m_dateRef.GetYear() ; i--)
  for (uint32_t i = year - 1 ; i > CDate::m_internalRefYear ; i--)
  {
    if (CDate::IsLeapYear(i) == true)
    {
      nbLeapYear++;
    }
  }


  return nbLeapYear;
}

//----------------------------------------

int32_t CDate::CheckYear(uint32_t year)
{
  if (year < CDate::m_internalRefYear)
  {
    return BRATHL_ERROR_INVALID_YEAR;
  }

  return BRATHL_SUCCESS;

}

//----------------------------------------

int32_t CDate::CheckMonth(uint32_t month)
{

  if (month < 1 || month > 12)
  {
    return BRATHL_ERROR_INVALID_MONTH;
  }
  return BRATHL_SUCCESS;
}


//----------------------------------------

int32_t CDate::CheckDay(uint32_t day, uint32_t month, uint32_t year)
{

  int32_t result = CheckMonth(month);

  if (result != BRATHL_SUCCESS)
  {
    return result;
  }

  uint32_t nbDaysInMonth = 0;

  result = GetDaysInMonth(month, year, nbDaysInMonth);

  if (result != BRATHL_SUCCESS)
  {
    return result;
  }

  if ((day <= 0)
      || (day > nbDaysInMonth))
  {
    return BRATHL_ERROR_INVALID_DAY;
  }

  return BRATHL_SUCCESS;
}

//----------------------------------------

int32_t CDate::CheckHour(uint32_t hour)
{

  if (hour < 0 || hour > 23)
  {
    return BRATHL_ERROR_INVALID_HOUR;
  }
  return BRATHL_SUCCESS;
}

//----------------------------------------

int32_t CDate::CheckMinute(uint32_t minute)
{

  if (minute < 0 || minute > 59)
  {
    return BRATHL_ERROR_INVALID_MINUTE;
  }
  return BRATHL_SUCCESS;
}

//----------------------------------------

int32_t CDate::CheckSecond(uint32_t second)
{

  if (second < 0 || second > 59)
  {
    return BRATHL_ERROR_INVALID_SECOND;
  }
  return BRATHL_SUCCESS;
}

//----------------------------------------

int32_t CDate::CheckMuSecond(uint32_t muSecond)
{

  if (muSecond < 0 || muSecond > 999999)
  {
    return BRATHL_ERROR_INVALID_MUSECOND;
  }
  return BRATHL_SUCCESS;
}

//----------------------------------------

void CDate::SetDefaultValue()
{

  setDefaultValue(m_minutes);
  setDefaultValue(m_muSeconds);

}


bool CDate::IsDefaultValue()
{
  if ((isDefaultValue(m_minutes) == true) ||
      (isDefaultValue(m_muSeconds) == true))
  {
    return true;
  }

  return false;

}

bool CDate::IsDefaultValueYMDHMSM(const uint32_t year, const uint32_t month /*= 1*/, const uint32_t day /*= 1*/,
      	      	         const uint32_t hour /*= 0*/, const uint32_t minute /*= 0*/, const uint32_t second /*= 0*/,
      	      	         const uint32_t muSecond /*= 0*/)
{
  if ((isDefaultValue(year) == true) ||
      (isDefaultValue(month) == true) ||
      (isDefaultValue(day) == true) ||
      (isDefaultValue(hour) == true) ||
      (isDefaultValue(minute) == true) ||
      (isDefaultValue(second) == true) ||
      (isDefaultValue(muSecond) == true))
  {
    return true;
  }

  return false;

}


//----------------------------------------

int32_t CDate::CheckDate(const uint32_t year, const uint32_t month /*= 1*/, const uint32_t day /*= 1*/,
      	      	         const uint32_t hour /*= 0*/, const uint32_t minute /*= 0*/, const uint32_t second /*= 0*/,
      	      	         const uint32_t muSecond /*= 0*/)
{
  int32_t result = BRATHL_SUCCESS;

  result = CheckYear(year);
  if (result != BRATHL_SUCCESS)
  {
    return result;
  }

  result = CheckMonth(month);
  if (result != BRATHL_SUCCESS)
  {
    return result;
  }

  result = CheckDay(day, month, year);
  if (result != BRATHL_SUCCESS)
  {
    return result;
  }

  result = CheckHour(hour);
  if (result != BRATHL_SUCCESS)
  {
    return result;
  }

  result = CheckMinute(minute);
  if (result != BRATHL_SUCCESS)
  {
    return result;
  }

  result = CheckSecond(second);
  if (result != BRATHL_SUCCESS)
  {
    return result;
  }

  result = CheckMuSecond(muSecond);
  if (result != BRATHL_SUCCESS)
  {
    return result;
  }

  return BRATHL_SUCCESS;


}

//----------------------------------------
//----------------------------------------

int32_t CDate::GetDaysInMonth(const uint32_t month, const uint32_t year, uint32_t &nbDaysInMonth)
{
  int32_t indexLeapYear = LeapYearIndex(year);

  nbDaysInMonth = 0;

  int32_t result = CheckMonth(month);

  if (result != BRATHL_SUCCESS)
  {
    return result;
  }

  nbDaysInMonth = m_daysInMonth[indexLeapYear][month - 1];


  return BRATHL_SUCCESS;
}



//----------------------------------------

int32_t CDate::AddDays(uint32_t days)
{
  int32_t result = BRATHL_SUCCESS;

  m_minutes += days * static_cast<uint32_t>(CDate::m_minutesInDay);

  return result;

}



//----------------------------------------

int32_t CDate::SubtractDays(uint32_t days)
{
  int32_t result = BRATHL_SUCCESS;

  m_minutes -= days * static_cast<uint32_t>(CDate::m_minutesInDay);

  return result;
}

//----------------------------------------

int32_t CDate::Add(const CDate& d)
{
  int32_t result = BRATHL_SUCCESS;

  m_minutes += d.m_minutes;
  m_muSeconds += d.m_muSeconds;

  Adjust();

  return result;
}

//----------------------------------------


void CDate::Adjust()
{

  uint32_t minutesTmp = static_cast<uint32_t>(m_muSeconds / (1.0E+6 * CDate::m_secInMinute));

  m_minutes += minutesTmp;

  m_muSeconds -= static_cast<uint32_t>(minutesTmp  * 1.0E+6 * CDate::m_secInMinute);

}


//----------------------------------------

int32_t CDate::DecodeDateYMDHMSM(const char *strDate)
{
  int32_t result = BRATHL_SUCCESS;
  uint32_t year = 0;
  uint32_t month = 0;
  uint32_t day = 0;
  uint32_t hour = 0;
  uint32_t minute = 0;
  uint32_t second = 0;
  uint32_t muSecond = 0;

  int32_t nbFields = sscanf(strDate, "%4d%*c%2d%*c%2d%*c"
				     "%2d%*c%2d%*c%2d%*c%6d",
			 &year, &month, &day,
      	      	      	 &hour, &minute, &second, &muSecond);

  if ( (nbFields == EOF) || (nbFields < 3) )
  {
    return BRATHL_ERROR_INVALID_DATE;
  }

  result = YMDHMSM2Second(year, month, day, hour, minute, second, muSecond);

  return result;

}

//----------------------------------------
int32_t CDate::Convert2SM(double& seconds, double& muSeconds,
      	      	      	   const brathl_refDate refDate /*= REF19500101*/)
{
  int32_t iseconds = 0;
  int32_t imuseconds = 0;

  int32_t result = Convert2SM(iseconds, imuseconds, refDate);

  if (result !=  BRATHL_SUCCESS)
  {
    setDefaultValue(seconds);
    setDefaultValue(muSeconds);
  }
  else
  {
    seconds = static_cast<double>(iseconds);
    muSeconds = static_cast<double>(imuseconds);
  }

  return result;
}
//----------------------------------------
int32_t CDate::Convert2SM(int32_t& seconds, int32_t& muSeconds,
      	      	      	   const brathl_refDate refDate /*= REF19500101*/)
{
  int32_t result = BRATHL_SUCCESS;

  CDate dateRef;
  result = GetDateRef(refDate, dateRef);

  if (result !=  BRATHL_SUCCESS)
  {
    return result;
  }

  if (IsDefaultValue() == true)
  {
    setDefaultValue(seconds);
    setDefaultValue(muSeconds);
    return result;
  }

  int32_t minutesTmp = m_minutes - dateRef.m_minutes;
  int32_t muSecondsTmp = m_muSeconds - dateRef.m_muSeconds;

  if (muSecondsTmp < 0 )
  {
    minutesTmp--;
    muSecondsTmp += static_cast<int32_t>(CDate::m_secInMinute * 1.0E+6);
  }

  seconds = static_cast<int32_t>(minutesTmp * CDate::m_secInMinute)
      	    + (static_cast<int32_t>(muSecondsTmp / 1.0E+6));


  muSeconds = muSecondsTmp
	      - static_cast<int32_t>((static_cast<int32_t>(muSecondsTmp / 1.0E+6) * 1.0E+6));

  return result;

}

//----------------------------------------
int32_t CDate::Convert2DMM(double& days, double& milliSeconds, double& muSeconds,
      	      	      	   const brathl_refDate refDate /*= REF19500101*/)
{
  int32_t idays = 0;
  int32_t imilliseconds = 0;
  int32_t imuseconds = 0;

  int32_t result = Convert2DMM(idays, imilliseconds, imuseconds, refDate);

  if (result !=  BRATHL_SUCCESS)
  {
    setDefaultValue(days);
    setDefaultValue(milliSeconds);
    setDefaultValue(muSeconds);
  }
  else
  {
    days = static_cast<double>(idays);
    milliSeconds = static_cast<double>(imilliseconds);
    muSeconds = static_cast<double>(imuseconds);
  }

  return result;
}
//----------------------------------------
int32_t CDate::Convert2DMM(int32_t& days, int32_t& milliSeconds, int32_t& muSeconds,
      	      	      	   const brathl_refDate refDate /*= REF19500101*/)
{

  int32_t result = BRATHL_SUCCESS;

  CDate dateRef;
  result = GetDateRef(refDate, dateRef);

  if (result !=  BRATHL_SUCCESS)
  {
    return result;
  }

  if (IsDefaultValue() == true)
  {
    setDefaultValue(days);
    setDefaultValue(milliSeconds);
    setDefaultValue(muSeconds);
    return result;
  }

  int32_t minutesTmp = m_minutes - dateRef.m_minutes;
  int32_t muSecondsTmp = m_muSeconds - dateRef.m_muSeconds;

  if (muSecondsTmp < 0 )
  {
    minutesTmp--;
    muSecondsTmp += static_cast<int32_t>(CDate::m_secInMinute * 1.0E+6);
  }

  days = static_cast<int32_t>(minutesTmp / CDate::m_minutesInDay);
  milliSeconds = static_cast<int32_t>((minutesTmp - (days * CDate::m_minutesInDay)) * CDate::m_secInMinute * 1000)
      	    + (static_cast<int32_t>(muSecondsTmp / 1000));

  if (milliSeconds < 0 )
  {
    days--;
    milliSeconds += static_cast<int32_t>(CDate::m_secInDay * 1.0E+3);
  }

  muSeconds = muSecondsTmp
              - static_cast<int32_t>((static_cast<int32_t>(muSecondsTmp / 1.0E+3) * 1.0E+3));

  return result;



}
//----------------------------------------
int32_t CDate::Convert2DSM(int32_t& days, int32_t& seconds, int32_t& muSeconds,
      	      	      	   const brathl_refDate refDate /*= REF19500101*/)
{
  int32_t result = BRATHL_SUCCESS;

  CDate dateRef;
  result = GetDateRef(refDate, dateRef);

  if (result !=  BRATHL_SUCCESS)
  {
    return result;
  }

  if (IsDefaultValue() == true)
  {
    setDefaultValue(days);
    setDefaultValue(seconds);
    setDefaultValue(muSeconds);
    return result;
  }

  int32_t minutesTmp = m_minutes - dateRef.m_minutes;
  int32_t muSecondsTmp = m_muSeconds - dateRef.m_muSeconds;

  if (muSecondsTmp < 0 )
  {
    minutesTmp--;
    muSecondsTmp += static_cast<int32_t>(CDate::m_secInMinute * 1.0E+6);
  }

  days = static_cast<int32_t>(minutesTmp / CDate::m_minutesInDay);
  seconds = static_cast<int32_t>((minutesTmp - (days * CDate::m_minutesInDay)) * CDate::m_secInMinute)
      	    + (static_cast<int32_t>(muSecondsTmp / 1.0E+6));

  if (seconds < 0 )
  {
    days--;
    seconds += static_cast<int32_t>(CDate::m_secInDay);
  }

  muSeconds = muSecondsTmp
	      - static_cast<int32_t>((static_cast<int32_t>(muSecondsTmp / 1.0E+6) * 1.0E+6));

  return result;


}

//----------------------------------------
int32_t CDate::Convert2DSM(double& days, double& seconds, double& muSeconds,
      	      	      	   const brathl_refDate refDate /*= REF19500101*/)
{
  int32_t idays = 0;
  int32_t iseconds = 0;
  int32_t imuseconds = 0;

  int32_t result = Convert2DSM(idays, iseconds, imuseconds, refDate);

  if (result !=  BRATHL_SUCCESS)
  {
    setDefaultValue(days);
    setDefaultValue(seconds);
    setDefaultValue(muSeconds);
  }
  else
  {
    days = static_cast<double>(idays);
    seconds = static_cast<double>(iseconds);
    muSeconds = static_cast<double>(imuseconds);
  }

  return result;

}
//----------------------------------------

int32_t CDate::Convert2DecimalJulian(double& julian, const brathl_refDate refDate /*= REF19500101*/)
{
  int32_t result = BRATHL_SUCCESS;
  int32_t days = 0;
  int32_t seconds = 0;
  int32_t muSeconds = 0;

  if (IsDefaultValue())
  {
    return BRATHL_SUCCESS;
  }

  setDefaultValue(julian);

  result = Convert2DSM(days, seconds, muSeconds, refDate);

  if (result != BRATHL_SUCCESS)
  {
    return result;
  }

  double tmp = ((static_cast<double>(seconds)) +
           ((static_cast<double>(muSeconds))/(static_cast<double>(1.0E+6)))) / CDate::m_secInDay;
  julian = (static_cast<double>(days)) + tmp;

  return result;
}

//----------------------------------------

int32_t CDate::Convert2Second(double& seconds, const brathl_refDate refDate /*= REF19500101*/)
{
  int32_t result = BRATHL_SUCCESS;

  CDate dateRef;
  result = GetDateRef(refDate, dateRef);

  if (result !=  BRATHL_SUCCESS)
  {
    return result;
  }

  seconds = Value() - dateRef.Value();
  return BRATHL_SUCCESS;
}

//----------------------------------------
int32_t CDate::Convert2YMDHMSM(uint32_t& year, uint32_t& month, uint32_t& day,
      	      	      	  uint32_t& hour, uint32_t& minute, uint32_t& second,
      	      	      	  uint32_t& muSecond)
{
  int32_t result = BRATHL_SUCCESS;

  uint32_t nbDays = 0;
  uint32_t nbYears = 0;
  uint32_t nbLeapYears = 0;
  int32_t leapYearIndex = 0;
  //double tmp = 0.0;
  int32_t tmp = 0;

  setDefaultValue(year);
  setDefaultValue(month);
  setDefaultValue(day);
  setDefaultValue(hour);
  setDefaultValue(minute);
  setDefaultValue(second);
  setDefaultValue(muSecond);

  if (IsDefaultValue())
  {
    return BRATHL_SUCCESS;
  }


  //uint32_t nbMinutes = m_minutes;
 // uint32_t nbMuSeconds = m_muSeconds;

  /////tmp = static_cast<uint32_t>(m_muSeconds / (1.0E+6 * CDate::m_secInMinute));

  ////nbMinutes += tmp;

  second = static_cast<uint32_t>(m_muSeconds / 1.0E+6);

  muSecond = m_muSeconds - static_cast<uint32_t>(second * 1.0E+6);

  nbDays = static_cast<uint32_t>(m_minutes / CDate::m_minutesInDay);

  tmp = m_minutes - static_cast<uint32_t>(nbDays * CDate::m_minutesInDay);

  hour = static_cast<uint32_t>(tmp / CDate::m_minutesInHour);

  tmp -= hour * static_cast<int32_t>(CDate::m_minutesInHour);

  minute = tmp;



  // Compute numner of years since reference date

  nbYears = static_cast<uint32_t>( (nbDays + 0.5) / 365.25);

  //year = nbYears + dateRef.GetYear();
  year = nbYears + CDate::m_internalRefYear;

  nbLeapYears = HowManyLeapYear(year);

  leapYearIndex =  LeapYearIndex(year);

  // Compute number of days since the beginning of the year
  nbDays = nbDays - (nbYears * 365) - nbLeapYears + 1;

  // Compute  month number
  month = 1;
  while ((month < 12) &&
         (CDate::m_daysOfYear[leapYearIndex][month] < nbDays))
  {
    month++;
  }

  // Compute day number
  day = nbDays - CDate::m_daysOfYear[leapYearIndex][month - 1];

  return result;

}

/*
*******************************************************************************
*******************************************************************************
/// <FUNCTION	NAME  = SUDT_JMAHMSM_STU50
///		TITLE = "Conversion date calendaire -> secondes ecoulees depuis 1950"
/// >
///
///
/// <GROUP NAME="02-Conversion"/>
/// <DESCRIPTION>
///	Convertit une date exprimee sous la forme  Jour/Mois/Annee
///     Heure/Minute/Seconde/µSecondes en nombre de secondes ecoulees depuis
///     le 01-Jan-1950 00:00:00.
/// </DESCRIPTION>
///
/// <PARAM NAME=Ijv_Jour	  UNIT="1-31"	TITLE="Jour dans le mois" ></PARAM>
/// <PARAM NAME=Ijv_Mois	  UNIT="1-12"	TITLE="Mois dans l'annee" ></PARAM>
/// <PARAM NAME=Ijv_Annee	  UNIT=""	TITLE="Annee (4 digits)" ></PARAM>
/// <PARAM NAME=Ijv_Heures	  UNIT="0-23"	TITLE="Heure dans le jour" ></PARAM>
/// <PARAM NAME=Ijv_Minutes	  UNIT="0-59"	TITLE="Minutes" ></PARAM>
/// <PARAM NAME=Ijv_Secondes	  UNIT="0-59"	TITLE="Secondes" ></PARAM>
/// <PARAM NAME=Ijv_muSecondes	  UNIT="0-999999"	TITLE="µSecondes" ></PARAM>
/// <PARAM NAME=Odv_DateSec50	  UNIT="s"	TITLE="Nombre de secondes ecoulees depuis 1950" ></PARAM>
/// <RESULT	UNIT=""	TITLE="Code retour"></RESULT>
/// <PROTOTYPE>
*/

int32_t CDate::YMDHMSM2Second(const uint32_t year, const uint32_t month /*= 1*/, const uint32_t day /*= 1*/,
      	      	         const uint32_t hour /*= 0*/, const uint32_t minute /*= 0*/, const uint32_t second /*= 0*/,
      	      	         const uint32_t muSecond /*= 0*/)
{
  int32_t nbDays = 0;	/* Nombre de jours ecoules depuis date de reference	*/
  int32_t nbLeapYears = 0;	/* Nombre d'annees bissextiles		*/
  int32_t result = BRATHL_SUCCESS;


  SetDefaultValue();

  if (IsDefaultValueYMDHMSM(year, month, day, hour, minute, second, muSecond) == true)
  {
    return BRATHL_SUCCESS;
  }

  result = CheckDate(year, month, day, hour, minute, second, muSecond);

  if (result != BRATHL_SUCCESS)
  {
    return result;
  }


  nbLeapYears = HowManyLeapYear(year);

  nbDays = ((year - CDate::m_internalRefYear) * 365) +
	     nbLeapYears +
      	     DayOfYear(year, month, day) - 1;

  m_minutes = static_cast<uint32_t>( (nbDays * CDate::m_minutesInDay) +
	      	      	      	     (hour * CDate::m_minutesInHour) +
	      	      	      	      minute );

  m_muSeconds =  static_cast<uint32_t>((second * 1.0E+6)) + muSecond;

  Adjust();

  return BRATHL_SUCCESS;
}






//----------------------------------------

int32_t CDate::DSM2Second(const uint32_t days, const uint32_t seconds, const uint32_t muSeconds,
      	      	      	  const brathl_refDate refDate /* = REF19500101*/)
{
  int32_t result = BRATHL_SUCCESS;

  if ((isDefaultValue(days) == true) ||
      (isDefaultValue(seconds) == true) ||
      (isDefaultValue(muSeconds) == true))
  {
    SetDefaultValue();
    return result;
   }


  CDate dateRef;
  result = GetDateRef(refDate, dateRef);

  if (result !=  BRATHL_SUCCESS)
  {
    return result;
  }

  uint32_t minutesTmp = static_cast<uint32_t>(seconds / CDate::m_secInMinute);
  uint32_t secondsTmp = static_cast<uint32_t>(seconds - (minutesTmp * CDate::m_secInMinute));

  m_minutes = static_cast<uint32_t>(days * CDate::m_minutesInDay)
	      + minutesTmp;

  m_muSeconds = static_cast<uint32_t>((static_cast<double>(secondsTmp) * 1.0E+6)) + muSeconds;

  Add(dateRef);
  //m_minutes += dateRef.m_minutes;
  //m_muSeconds += dateRef.m_muSeconds;

  return result;

}

//----------------------------------------

uint32_t CDate::GetYear()
{
  int32_t result = BRATHL_SUCCESS;

  uint32_t year = 0;
  uint32_t month = 0;
  uint32_t day = 0;
  uint32_t hour = 0;
  uint32_t minute = 0;
  uint32_t second = 0;
  uint32_t muSecond = 0;

  result = Convert2YMDHMSM(year, month, day, hour, minute, second, muSecond);
  return year;
}

//----------------------------------------

uint32_t CDate::GetMonth()
{
  int32_t result = BRATHL_SUCCESS;

  uint32_t year = 0;
  uint32_t month = 0;
  uint32_t day = 0;
  uint32_t hour = 0;
  uint32_t minute = 0;
  uint32_t second = 0;
  uint32_t muSecond = 0;

  result = Convert2YMDHMSM(year, month, day, hour, minute, second, muSecond);
  return month;
}

//----------------------------------------

uint32_t CDate::GetDay()
{
  int32_t result = BRATHL_SUCCESS;

  uint32_t year = 0;
  uint32_t month = 0;
  uint32_t day = 0;
  uint32_t hour = 0;
  uint32_t minute = 0;
  uint32_t second = 0;
  uint32_t muSecond = 0;

  result = Convert2YMDHMSM(year, month, day, hour, minute, second, muSecond);
  return day;
}

//----------------------------------------

uint32_t CDate::GetHour()
{
  int32_t result = BRATHL_SUCCESS;

  uint32_t year = 0;
  uint32_t month = 0;
  uint32_t day = 0;
  uint32_t hour = 0;
  uint32_t minute = 0;
  uint32_t second = 0;
  uint32_t muSecond = 0;

  result = Convert2YMDHMSM(year, month, day, hour, minute, second, muSecond);
  return hour;
}
//----------------------------------------

uint32_t CDate::GetMinute()
{
  int32_t result = BRATHL_SUCCESS;

  uint32_t year = 0;
  uint32_t month = 0;
  uint32_t day = 0;
  uint32_t hour = 0;
  uint32_t minute = 0;
  uint32_t second = 0;
  uint32_t muSecond = 0;

  result = Convert2YMDHMSM(year, month, day, hour, minute, second, muSecond);
  return minute;
}

//----------------------------------------

uint32_t CDate::GetSecond()
{
  int32_t result = BRATHL_SUCCESS;

  uint32_t year = 0;
  uint32_t month = 0;
  uint32_t day = 0;
  uint32_t hour = 0;
  uint32_t minute = 0;
  uint32_t second = 0;
  uint32_t muSecond = 0;

  result = Convert2YMDHMSM(year, month, day, hour, minute, second, muSecond);
  return second;
}
//----------------------------------------

uint32_t CDate::GetMuSecond()
{
  int32_t result = BRATHL_SUCCESS;

  uint32_t year = 0;
  uint32_t month = 0;
  uint32_t day = 0;
  uint32_t hour = 0;
  uint32_t minute = 0;
  uint32_t second = 0;
  uint32_t muSecond = 0;

  result = Convert2YMDHMSM(year, month, day, hour, minute, second, muSecond);
  return muSecond;
}
//----------------------------------------


int32_t CDate::ExtractRef(char *strDate, CDate& dateRef, bool& hasRef)
{
  int32_t result = BRATHL_SUCCESS;

  hasRef = false;

  char * refStr = strchr(strDate, '@');
  if (refStr == NULL)
  {
    return BRATHL_SUCCESS;
  }

  hasRef = true;

  //Separates date and date reference
  *refStr = '\0';

  refStr = CTools::Trim(refStr + 1); //refStr + 1 --> first char after '@'

  uint32_t year = 0;
  sscanf(refStr, "%d",
			    &year);

  result = dateRef.YMDHMSM2Second(year);

  return result;

}

//----------------------------------------

int32_t CDate::DecodeDateDSM(const char *strDateDSM)
{
  int32_t result = BRATHL_SUCCESS;

  std::string stringDate = strDateDSM;

  if (stringDate.empty())
  {
      result = BRATHL_ERROR_INVALID_DATE;
  }

  char lastChar = stringDate[stringDate.size() - 1];

  while (((lastChar == ' ') || (lastChar == '\t') ||  (lastChar == '\n')) && ( stringDate.size() > 1))
  {
    stringDate.resize(stringDate.size() - 1);
    lastChar = stringDate[stringDate.size() - 1];
  }

  //-------------

  char		*current= NULL;	// current char
  int32_t	tabResult[3];	// result
  int32_t	value = 0;		// temporary value
  bool		isNumber = false;	// to test if a number is read
  int32_t	nbValues = 0;	// nb value in the std::string
  double        floatDate = 0.0;
  char		*endPtr = NULL;

  brathl_DateDSM dateDSM;

  brathl_refDate refDate = REF19500101;

  char * strDate = new char[stringDate.size() + 1];
  strcpy(strDate, stringDate.c_str());

  // Checks if strdate contains a date reference
  // if no, 1950 will be default date reference
  CDate dateRef(static_cast<uint32_t>(1950));
  bool hasRef = false;

  result = ExtractRef(strDate, dateRef, hasRef);

  if (result != BRATHL_SUCCESS)
  {
    delete []strDate;
    strDate = NULL;
    return result;
  }

  result = GetDateRef(dateRef, refDate);

  if (result != BRATHL_SUCCESS)
  {
    delete []strDate;
    strDate = NULL;
    return result;
  }

  memset(tabResult, '\0', sizeof(tabResult));

  //---------------
  if (CTools::StrCaseCmp(strDate,"DV") == 0)
  {
    SetDefaultValue();

    delete []strDate;
    strDate = NULL;

    return BRATHL_SUCCESS;
  }

  current	= strDate;
  nbValues	= 1;

  while (*current)
  {
    switch(*current)
    {
    case '0' :
    case '1' :
    case '2' :
    case '3' :
    case '4' :
    case '5' :
    case '6' :
    case '7' :
    case '8' :
    case '9' :
      break;

    case ' '  :
    case '\t' :
	    nbValues++;
      break;

    default:
      break;
    }

    // next char.
    current++;
  }




  //---------------
  if ((strchr(strDate, '.') != NULL) || (nbValues == 1))
  {// std::string has '.' so it's a decimal julian day
    errno	= 0;
    floatDate	= strtod(strDate, &endPtr);

    if (*endPtr || (errno != 0))
    {
      result = BRATHL_ERROR_INVALID_DATE;
    }
    else
    {
      result = SetDateJulian(floatDate, refDate);
    }

    delete []strDate;
    strDate = NULL;

    return result;
  }

  //---------------

  // decodes list */
  current	= strDate;
  value	= 0;
  isNumber	= false;
  nbValues	= 0;

  while (*current)
  {
    switch(*current)
    {
    case '0' :
    case '1' :
    case '2' :
    case '3' :
    case '4' :
    case '5' :
    case '6' :
    case '7' :
    case '8' :
    case '9' :
      if (nbValues >= 3)
      {
	      return BRATHL_ERROR_INVALID_DATE;
      }
      value	= value*10 + (*current & 0xF);
      isNumber = true;
      break;

    case ' '  :
    case '\t' :
      if (isNumber)
      {
	      tabResult[nbValues++] = value;
	      isNumber = false;
	      value = 0;
      }
      break;

    default:
      delete []strDate;
      strDate = NULL;
	    return BRATHL_ERROR_INVALID_DATE;
    }

    // next char.
    current++;
  }


  delete []strDate;
  strDate = NULL;


  if (isNumber)
  {
    tabResult[nbValues++] = value;
    isNumber = false;
  }

  if (isDefaultValue(tabResult[0]) == true)
  {
    return BRATHL_ERROR_INVALID_DAY;
  }

  if (tabResult[1] >= CDate::m_secInDay)
  {
    return BRATHL_ERROR_INVALID_SECOND;
  }

  if (tabResult[2] >=  1.0E+6)
  {
    return BRATHL_ERROR_INVALID_MUSECOND;
  }


  dateDSM.refDate = refDate;
  dateDSM.days = static_cast<int32_t>(tabResult[0]);
  dateDSM.seconds =  static_cast<int32_t>(tabResult[1]);
  dateDSM.muSeconds = static_cast<int32_t>(tabResult[2]);

  result = SetDate(dateDSM);

  return result;
}

//----------------------------------------

void CDate::Dump(std::ostream& fOut /* = std::cerr */)
{
   if (CTrace::IsTrace() == false)
   {
     return;
   }

   fOut << "==> Dump a CDate Object at "<< this << std::endl;

   fOut << "m_minutes = " << m_minutes << std::endl;
   fOut << "m_muSeconds = " << m_muSeconds << std::endl;

   fOut << "==> END Dump a CDate Object at "<< this << std::endl;

   fOut << std::endl;

}

}
