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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "brathlc.h"

#if defined( __cplusplus )
#error "This source file must be compiled with C not C++"
#endif  /* __cplusplus */

#ifdef f2cFortran
#define SECOND_UNDERSCORE(X)  _(X,_)
#else
#define SECOND_UNDERSCORE(X)  X
#endif



/* First prepare for the C compiler. */

#ifndef ANSI_C_preprocessor /* i.e. user can override. */
#ifdef __CF__KnR
#define ANSI_C_preprocessor 0
#else
#ifdef __STDC__
#define ANSI_C_preprocessor 1
#else
#define _cfleft             1
#define _cfright
#define _cfleft_cfright     0
#define ANSI_C_preprocessor _cfleft/**/_cfright
#endif
#endif
#endif

#if ANSI_C_preprocessor
#define _0(A,B)   A##B
#define  _(A,B)   _0(A,B)  /* see cat,xcat of K&R ANSI C p. 231 */
#define _2(A,B)   A##B     /* K&R ANSI C p.230: .. identifier is not replaced */
#define _3(A,B,C) _(A,_(B,C))
#else                      /* if it turns up again during rescanning.         */
#define  _(A,B)   A/**/B
#define _2(A,B)   A/**/B
#define _3(A,B,C) A/**/B/**/C
#endif

#ifdef FORTRAN_TWO_UNDERSCORES
  #define ADD_UNDERSCORE(X)	_(X,__)
#else
  #ifdef FORTRAN_ONE_UNDERSCORE
    #define ADD_UNDERSCORE(X)	_(X,_)
  #else
    #ifdef FORTRAN_NO_UNDERSCORE
      #define ADD_UNDERSCORE(X)	X
    #else
      #error "Either FORTRAN_TWO_UNDERSCORES, FORTRAN_ONE_UNDERSCORE or FORTRAN_NO_UNDERSCORE must be defined"
    #endif
  #endif
#endif

#ifdef FORTRAN_UPPERCASE
  #define FTN_NAME(Low,Up)	ADD_UNDERSCORE(Up)
#else
  #define FTN_NAME(Low,Up)	ADD_UNDERSCORE(Low)
#endif

#define REAL8		double
#define INTEGER4	int32_t


#define MIN(A,B)	((A) < (B) ? (A) : (B))

static int32_t GetFtnStringLen
			(const char	*FtnString,
			 int32_t	FtnLength)
{
  do
  {
    FtnLength--;
  } while ((FtnLength >= 0) &&
	   ((FtnString[FtnLength] == ' ') || (FtnString[FtnLength] == '\0')));
  return FtnLength+1;
}

static char *GetFtnString
			(const char	*FtnString,
			 int32_t	FtnLength)
{
  char	*result;
  result	= calloc(FtnLength+1, 1);
  if (result != NULL)
  {
    memcpy(result, FtnString, GetFtnStringLen(FtnString, FtnLength));
  }
  return result;
}


/*
** Creates an array of ArraySize pointers to CStrings of FtnLength+1 chars max
** It is allocated at one time in order to simplify freeing (only one free
** is needed
*/
static char **GetFtnStringArray
			(const char	*FtnString,
			 int32_t	FtnLength,
			 int32_t	ArraySize)
{
  char		**result;
  char		*Allocated;
  int32_t	Index;
  size_t	ArrayTableSize	= sizeof(char *)*ArraySize;
  size_t	AllocatedSize	= (FtnLength+1)*ArraySize + ArrayTableSize;

  Allocated	= calloc(AllocatedSize, 1);
  if (Allocated == NULL)
    return NULL;

  result	= (char **)Allocated;
  Allocated	+= ArrayTableSize;
  for (Index=0; Index<ArraySize ; Index++, Allocated += FtnLength+1)
  {
    result[Index]	= Allocated;
    memcpy(Allocated,
	   FtnString+(Index*FtnLength),
	   GetFtnStringLen(FtnString+(Index*FtnLength), FtnLength));
  }
  return result;
}


static void PutFtnString
			(char		*FtnString,
			 int32_t	FtnLength,
			 const char	*CString)
{
  int32_t	Length	= strlen(CString);
  Length	= MIN(Length, FtnLength);
  memset(FtnString+Length, ' ', FtnLength-Length);
  memcpy(FtnString, CString, Length);
  return;
}



static void PutFtnStringArray
			(char		*FtnString,
			 int32_t	FtnLength,
			 int32_t	ArraySize,
			 const char	**CStrings)
{
  int32_t	Index;

  for (Index=0; Index<ArraySize; Index++)
  {
    PutFtnString(FtnString+(FtnLength*Index),
		 FtnLength,
		 CStrings[Index]);
  }
  return;
}


/** \file
  Fortran general interface of BRATHL
  no .h file since it is only called from fortran
 */

/** \addtogroup date_conversionFortran Date conversion Fortran APIs
 @{*/

/*
**----------------------------------
*/

/** Initializes the date reference user1 from a string
    See also #brathl_refDate

    \par Fortran specification
    SUBROUTINE brathlf_SetRefUser1(dateRefUser)<BR>
	CHARACTER*(*) dateRefUser

    \param[in] dateRefUser : date string (format: YYYY-MM-DD HH:MN:SS:MS) */
void FTN_NAME(brathlf_setrefuser1, BRATHLF_SETREFUSER1)
			(char		*dateRefUser,
			 int32_t	ftn_Length1)
{
  int	Length	= GetFtnStringLen(dateRefUser, ftn_Length1);
  if (Length > 0)
  {
    memset(brathl_refDateUser1, 0, BRATHL_REF_DATE_USER_LEN);
    memcpy(brathl_refDateUser1, dateRefUser, MIN(BRATHL_REF_DATE_USER_LEN-1, Length));
  }
  return;
}


/*
**----------------------------------
*/

/** Initializes the date reference user2 from a string
    See also #brathl_refDate
    \par Fortran specification
    SUBROUTINE brathlf_SetRefUser2(dateRefUser)<BR>
	CHARACTER*(*) dateRefUser

    \param dateRefUser : date string (format: YYYY-MM-DD HH:MN:SS:MS) */
void FTN_NAME(brathlf_setrefuser2, BRATHLF_SETREFUSER2)
			(char		*dateRefUser,
			 int32_t	ftn_Length1)
{
  int	Length	= GetFtnStringLen(dateRefUser, ftn_Length1);
  if (Length > 0)
  {
    memset(brathl_refDateUser2, 0, BRATHL_REF_DATE_USER_LEN);
    memcpy(brathl_refDateUser2, dateRefUser, MIN(BRATHL_REF_DATE_USER_LEN-1, Length));
  }
  return;
}

/*
**----------------------------------
*/

/** returns #brathl_errno
    \par Fortran specification
    INTEGER*4 FUNCTION brathlf_GetErrno()

    \return Last registered error code
*/
INTEGER4 FTN_NAME(brathlf_geterrno, BRATHLF_GETERRNO)
			(void)
{
    return brathl_errno;
}


/*
**----------------------------------
*/

/** Retrieve a string with the error description

With a few exceptions almost all BRATHL functions return an integer that indicate whether the function
was able to perform its operations successfully.
The return value will be 0 on success and < 0 otherwise. The result is also save in the global variable #brat_errno
In case you get a negative value.

    \par Fortran specification
    SUBROUTINE brathlf_errno2string(err, str)<BR>
	INTEGER*4 err
	CHARACTER*(*) str

\param[in] err : error code
\param[out] str : string error description
*/
void FTN_NAME(brathlf_errno2string, BRATHLF_ERRNO2STRING)
			(INTEGER4	*err,
			 char		*str,
			 INTEGER4	ftn_Length1)
{
  PutFtnString(str, ftn_Length1, brathl_Errno2String(*err));
  return;
}


/*
**----------------------------------
*/

/** Converts seconds into a days-seconds-microseconds date, according to refDate parameter

    \par Fortran specification
    INTEGER*4 FUNCTION brathlf_Seconds2DSM(iRefDateSrc,iSeconds,iRefDateDest,oDays,oSeconds,oMuSeconds)<BR>
	INTEGER*4 iRefDateSrc
	REAL*8    iSeconds
	INTEGER*4 iRefDateDest
	INTEGER*4 oDays
	INTEGER*4 oSeconds
	INTEGER*4 oMuSeconds

  \param[in] iRefDateSrc : source date reference (see #brathl_refDate)
  \param[in] iSeconds : date to convert
  \param[in] iRefDateDest : date reference conversion (see #brathl_refDate)
  \param[out] oDays : numbers of days
  \param[out] oSeconds : number of seconds
  \param[out] oMuSeconds : numbers of microseconds
  \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
INTEGER4 FTN_NAME(brathlf_seconds2dsm, BRATHLF_SECONDS2DSM)
			(INTEGER4	*iRefDateSrc,
			 REAL8		*iSeconds,
			 INTEGER4	*iRefDateDest,
      		      	 INTEGER4	*oDays,
			 INTEGER4	*oSeconds,
			 INTEGER4	*oMuSeconds)
{
  int32_t result = BRATHL_SUCCESS;

  brathl_DateSecond dateSeconds;
  brathl_refDate refDate;
  brathl_DateDSM dateDSM;

  dateSeconds.refDate	= *iRefDateSrc;
  dateSeconds.nbSeconds	= *iSeconds;

  refDate	= *iRefDateDest;
  result	= brathl_Seconds2DSM(&dateSeconds, refDate, &dateDSM);
  *oDays	= dateDSM.days;
  *oSeconds	= dateDSM.seconds;
  *oMuSeconds	= dateDSM.muSeconds;
  return result;
}


/*
**----------------------------------
*/

/** Converts a date in days-seconds-microseconds into a seconds, according to refDate parameter

    \par Fortran specification
    INTEGER*4 FUNCTION brathlf_DSM2Seconds(iRefDateSrc,iDays,iSeconds,iMuSeconds,iRefDateDest,oSeconds)<BR>
	INTEGER*4 iRefDateSrc
	INTEGER*4 iDays
	INTEGER*4 iSeconds
	INTEGER*4 iMuSeconds
	INTEGER*4 iRefDateDest
	REAL*8    oSeconds

  \param[in] iRefDateSrc : source date reference (see #brathl_refDate)
  \param[in] iDays : numbers of days
  \param[in] iSeconds : number of seconds
  \param[in] iMuSeconds : numbers of microseconds
  \param[in] iRefDateDest : date reference conversion (see #brathl_refDate)
  \param[out] oSeconds : date to convert
  \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
INTEGER4 FTN_NAME(brathlf_dsm2seconds, BRATHLF_DSM2SECONDS)
			(INTEGER4	*iRefDateSrc,
			 INTEGER4	*iDays,
			 INTEGER4	*iSeconds,
			 INTEGER4	*iMuSeconds,
			 INTEGER4	*iRefDateDest,
			 REAL8		*oSeconds)
{
  int32_t result = BRATHL_SUCCESS;

  brathl_DateSecond dateSeconds;
  brathl_refDate refDate;
  brathl_DateDSM dateDSM;

  dateDSM.refDate	= *iRefDateSrc;
  dateDSM.days		= *iDays;
  dateDSM.seconds	= *iSeconds;
  dateDSM.muSeconds	= *iMuSeconds;


  refDate	= *iRefDateDest;
  result	= brathl_DSM2Seconds(&dateDSM, refDate, &dateSeconds);
  *oSeconds	= dateSeconds.nbSeconds;

  return result;
}



/*
**----------------------------------
*/

/** Converts a decimal julian date into a days-seconds-microseconds date, according to refDate parameter

    \par Fortran specification
    INTEGER*4 FUNCTION brathlf_Julian2DSM(iRefDateSrc,iJulian,iRefDateDest,oDays,oSeconds,oMuSeconds)<BR>
	INTEGER*4 iRefDateSrc
	REAL*8    iJulian
	INTEGER*4 iRefDateDest
	INTEGER*4 oDays
	INTEGER*4 oSeconds
	INTEGER*4 oMuSeconds

  \param[in] iRefDateSrc : source date reference (see #brathl_refDate)
  \param[in] iJulian : decimal julian date
  \param[in] iRefDateDest : date reference conversion (see #brathl_refDate)
  \param[out] oDays : numbers of days
  \param[out] oSeconds : number of seconds
  \param[out] oMuSeconds : numbers of microseconds
  \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
INTEGER4 FTN_NAME(brathlf_julian2dsm, BRATHLF_JULIAN2DSM)
			(INTEGER4	*iRefDateSrc,
			 REAL8		*iJulian,
			 INTEGER4	*iRefDateDest,
			 INTEGER4	*oDays,
			 INTEGER4	*oSeconds,
			 INTEGER4	*oMuSeconds)
{
  int32_t result = BRATHL_SUCCESS;

  brathl_DateJulian dateJulian;
  brathl_refDate refDate;
  brathl_DateDSM dateDSM;

  dateJulian.refDate	= *iRefDateSrc;
  dateJulian.julian	= *iJulian;

  refDate	= *iRefDateDest;
  result	= brathl_Julian2DSM(&dateJulian, refDate, &dateDSM);

  *oDays	= dateDSM.days;
  *oSeconds	= dateDSM.seconds;
  *oMuSeconds	= dateDSM.muSeconds;
  return result;
}


/*
**----------------------------------
*/

/** Converts a days-seconds-microseconds date into a decimal julian date, according to refDate parameter

    \par Fortran specification
    INTEGER*4 FUNCTION brathlf_DSM2Julian(iRefDateSrc,iDays,iSeconds,iMuSeconds,iRefDateDest,oJulian)<BR>
	INTEGER*4 iRefDateSrc
	INTEGER*4 iDays
	INTEGER*4 iSeconds
	INTEGER*4 iMuSeconds
	INTEGER*4 iRefDateDest
	REAL*8    oJulian
 
  \param[in] iRefDateSrc : source date reference (see #brathl_refDate)
  \param[in] iDays : numbers of days
  \param[in] iSeconds : number of seconds
  \param[in] iMuSeconds : numbers of microseconds
  \param[in] iRefDateDest : date reference conversion (see #brathl_refDate)
  \param[out] oJulian : date to convert
  \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
INTEGER4 FTN_NAME(brathlf_dsm2julian, BRATHLF_DSM2JULIAN)
			(INTEGER4	*iRefDateSrc,
			 INTEGER4	*iDays,
			 INTEGER4	*iSeconds,
			 INTEGER4	*iMuSeconds,
			 INTEGER4	*iRefDateDest,
			 REAL8		*oJulian)
{
  int32_t result = BRATHL_SUCCESS;

  brathl_DateJulian dateJulian;
  brathl_refDate refDate;
  brathl_DateDSM dateDSM;

  dateDSM.refDate	= *iRefDateSrc;
  dateDSM.days		= *iDays;
  dateDSM.seconds	= *iSeconds;
  dateDSM.muSeconds	= *iMuSeconds;


  refDate	= *iRefDateDest;
  result	= brathl_DSM2Julian(&dateDSM, refDate, &dateJulian);
  *oJulian	= dateJulian.julian;

  return result;
}

/*
**----------------------------------
*/
/** Converts a year, month, day, hour, minute, second, microsecond date into a days-seconds-microseconds date,
   according to refDate parameter
    \par Fortran specification
    INTEGER*4 FUNCTION brathlf_YMDHMSM2DSM(iYear,iMonth,iDay,iHour,iMinute,iSecond,iMuSecond,iRefDateDest,oDays,oSeconds,oMuSeconds)

       INTEGER*4 iYear,
       INTEGER*4 iMonth,
       INTEGER*4 iDay,
       INTEGER*4 iHour,
       INTEGER*4 iMinute,
       INTEGER*4 iSecond,
       INTEGER*4 iMuSecond,
       INTEGER*4 iRefDateDest,
       INTEGER*4 oDays,
       INTEGER*4 oSeconds,
       INTEGER*4 oMuSeconds

  \param[in] iYear : year
  \param[in] iMonth : month
  \param[in] iDay : day 
  \param[in] iHour : hour 
  \param[in] iMinute : minute 
  \param[in] iSecond : second
  \param[in] iMuSecond : micro-second 
  \param[in] iRefDateDest : date reference conversion (see #brathl_refDate)
  \param[out] oDays : numbers of days
  \param[out] oSeconds : number of seconds
  \param[out] oMuSeconds : numbers of microseconds
  \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */

INTEGER4 FTN_NAME(brathlf_ymdhmsm2dsm, BRATHLF_YMDHMSM2DSM)
			(INTEGER4	*iYear,
			 INTEGER4	*iMonth,
			 INTEGER4	*iDay,
			 INTEGER4	*iHour,
			 INTEGER4	*iMinute,
			 INTEGER4	*iSecond,
			 INTEGER4	*iMuSecond,
			 INTEGER4	*iRefDateDest,
			 INTEGER4	*oDays,
			 INTEGER4	*oSeconds,
			 INTEGER4	*oMuSeconds)
{
  int32_t result = BRATHL_SUCCESS;

  brathl_DateYMDHMSM dateYMDHMSM;
  brathl_refDate refDate;
  brathl_DateDSM dateDSM;

  dateYMDHMSM.year	= *iYear;
  dateYMDHMSM.month	= *iMonth;
  dateYMDHMSM.day	= *iDay;
  dateYMDHMSM.hour	= *iHour;
  dateYMDHMSM.minute	= *iMinute;
  dateYMDHMSM.second	= *iSecond;
  dateYMDHMSM.muSecond	= *iMuSecond;

  refDate	= *iRefDateDest;
  result	= brathl_YMDHMSM2DSM(&dateYMDHMSM, refDate, &dateDSM);

  *oDays	= dateDSM.days;
  *oSeconds	= dateDSM.seconds;
  *oMuSeconds	= dateDSM.muSeconds;
  return result;
}

/*
**----------------------------------
*/
/** Converts a days-seconds-microseconds date into a year, month, day, hour, minute, second, microsecond date
   according to refDate parameter
    \par Fortran specification
    INTEGER*4 FUNCTION brathlf_DSM2MDHMSM(iRefDateSrc,iDays,iSeconds,iMuSeconds,oYear,oMonth,oDay,oHour,oMinute,oSecond,oMuSecond)

       INTEGER*4 iRefDateSrc,
       INTEGER*4 iDays,
       INTEGER*4 iSeconds,
       INTEGER*4 iMuSeconds
       INTEGER*4 oYear,
       INTEGER*4 oMonth,
       INTEGER*4 oDay,
       INTEGER*4 oHour,
       INTEGER*4 oMinute,
       INTEGER*4 oSecond,
       INTEGER*4 oMuSecond,

  \param[in] iRefDateSrc : source date reference (see #brathl_refDate)
  \param[in] iDays : numbers of days
  \param[in] iSeconds : number of seconds
  \param[in] iMuSeconds : numbers of microseconds
  \param[out] oYear : year 
  \param[out] oMonth : month 
  \param[out] oDay : day 
  \param[out] oHour : hour convert
  \param[out] oMinute : minute to convert
  \param[out] oSecond : second to convert
  \param[out] oMuSecond : micro-second to convert
  \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */

INTEGER4 FTN_NAME(brathlf_dsm2ymdhmsm, BRATHLF_DSM2YMDHMSM)
			(INTEGER4	*iRefDateSrc,
			 INTEGER4	*iDays,
			 INTEGER4	*iSeconds,
			 INTEGER4	*iMuSeconds,
			 INTEGER4	*oYear,
			 INTEGER4	*oMonth,
			 INTEGER4	*oDay,
			 INTEGER4	*oHour,
			 INTEGER4	*oMinute,
			 INTEGER4	*oSecond,
			 INTEGER4	*oMuSecond)
{
  int32_t result = BRATHL_SUCCESS;

  brathl_DateYMDHMSM dateYMDHMSM;
  brathl_DateDSM dateDSM;

  dateDSM.refDate	= *iRefDateSrc;
  dateDSM.days		= *iDays;
  dateDSM.seconds	= *iSeconds;
  dateDSM.muSeconds	= *iMuSeconds;

  result	= brathl_DSM2YMDHMSM(&dateDSM, &dateYMDHMSM);

  *oYear	= dateYMDHMSM.year;
  *oMonth	= dateYMDHMSM.month;
  *oDay		= dateYMDHMSM.day;
  *oHour	= dateYMDHMSM.hour;
  *oMinute	= dateYMDHMSM.minute;
  *oSecond	= dateYMDHMSM.second;
  *oMuSecond	= dateYMDHMSM.muSecond;

  return result;
}

/*
**----------------------------------
*/

/** Converts seconds into a decimal julian date, according to refDate parameter
    INTEGER*4 FUNCTION brathlf_Seconds2Julian(iRefDateSrc,iSeconds,iRefDateDest,oJulian)
    \par Fortran specification

       INTEGER*4 iRefDateSrc,
       INTEGER*4 iSeconds,
       INTEGER*4 iRefDateDest
       REAL*8 oJulian,

  \param[in] iRefDateSrc : source date reference (see #brathl_refDate)
  \param[in] iSeconds : number of seconds
  \param[in] iRefDateDest : date reference conversion (see #brathl_refDate)
  \param[out] oJulian : julian date
  \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */

INTEGER4 FTN_NAME(brathlf_seconds2julian, BRATHLF_SECONDS2JULIAN)
			(INTEGER4	*iRefDateSrc,
			 REAL8		*iSeconds,
			 INTEGER4	*iRefDateDest,
			 REAL8		*oJulian)
{
  int32_t result = BRATHL_SUCCESS;

  brathl_DateSecond dateSeconds;
  brathl_refDate refDate;
  brathl_DateJulian dateJulian;

  dateSeconds.refDate	= *iRefDateSrc;
  dateSeconds.nbSeconds	= *iSeconds;

  refDate	= *iRefDateDest;
  result	= brathl_Seconds2Julian(&dateSeconds, refDate, &dateJulian);
  *oJulian	= dateJulian.julian;
  return result;
}

/*
**----------------------------------
*/
/** Converts a decimal julian date into seconds, according to refDate parameter
    INTEGER*4 FUNCTION brathlf_Seconds2Julian(iRefDateSrc,iJulian,iRefDateDest,oSeconds)
    \par Fortran specification

       INTEGER*4 iRefDateSrc,
       REAL*8 iJulian,
       INTEGER*4 iRefDateDest
       INTEGER*4 oSeconds,

  \param[in] iRefDateSrc : source date reference (see #brathl_refDate)
  \param[in] iJulian : julian date
  \param[in] iRefDateDest : date reference conversion (see #brathl_refDate)
  \param[out] oSeconds : number of seconds
  \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */

INTEGER4 FTN_NAME(brathlf_julian2seconds, BRATHLF_JULIAN2SECONDS)
			(INTEGER4	*iRefDateSrc,
			 REAL8		*iJulian,
			 INTEGER4	*iRefDateDest,
      	      	         REAL8		*oSeconds)
{
  int32_t result = BRATHL_SUCCESS;

  brathl_DateSecond dateSeconds;
  brathl_refDate refDate;
  brathl_DateJulian dateJulian;

  dateJulian.refDate	= *iRefDateSrc;
  dateJulian.julian	= *iJulian;

  refDate	= *iRefDateDest;
  result	= brathl_Julian2Seconds(&dateJulian, refDate, &dateSeconds);
  *oSeconds	= dateSeconds.nbSeconds;
  return result;
}


/*
**----------------------------------
*/

/** Converts seconds into into a year, month, day, hour, minute, second, microsecond date, according to refDate parameter
    INTEGER*4 FUNCTION brathlf_Seconds2YMDHMSM(iRefDateSrc,iSeconds,oYear,oMonth,oDay,oHour,oMinute,oSecond,oMuSecond)
    \par Fortran specification

       INTEGER*4 iRefDateSrc,
       INTEGER*4 iSeconds,
       INTEGER*4 iRefDateDest
       INTEGER*4 oYear,
       INTEGER*4 oMonth,
       INTEGER*4 oDay,
       INTEGER*4 oHour,
       INTEGER*4 oMinute,
       INTEGER*4 oSecond,
       INTEGER*4 oMuSecond,

  \param[in] iRefDateSrc : source date reference (see #brathl_refDate)
  \param[in] iSeconds : number of seconds
  \param[out] oYear : year 
  \param[out] oMonth : month 
  \param[out] oDay : day 
  \param[out] oHour : hour convert
  \param[out] oMinute : minute to convert
  \param[out] oSecond : second to convert
  \param[out] oMuSecond : micro-second to convert
  \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */

INTEGER4 FTN_NAME(brathlf_seconds2ymdhmsm, BRATHLF_SECONDS2YMDHMSM)
			(INTEGER4	*iRefDateSrc,
			 REAL8		*iSeconds,
			 INTEGER4	*oYear,
			 INTEGER4	*oMonth,
			 INTEGER4	*oDay,
			 INTEGER4	*oHour,
			 INTEGER4	*oMinute,
			 INTEGER4	*oSecond,
			 INTEGER4	*oMuSecond)
{
  int32_t result = BRATHL_SUCCESS;

  brathl_DateYMDHMSM dateYMDHMSM;
  brathl_DateSecond dateSeconds;

  dateSeconds.refDate	= *iRefDateSrc;
  dateSeconds.nbSeconds	= *iSeconds;

  result	= brathl_Seconds2YMDHMSM(&dateSeconds, &dateYMDHMSM);

  *oYear	= dateYMDHMSM.year;
  *oMonth	= dateYMDHMSM.month;
  *oDay		= dateYMDHMSM.day;
  *oHour	= dateYMDHMSM.hour;
  *oMinute	= dateYMDHMSM.minute;
  *oSecond	= dateYMDHMSM.second;
  *oMuSecond	= dateYMDHMSM.muSecond;

  return result;
}



/*
**----------------------------------
*/
/** Converts a year, month, day, hour, minute, second, microsecond date  into seconds, according to refDate parameter
      \par Fortran specification
    INTEGER*4 FUNCTION brathlf_YMDHMSM2DSM(iYear,iMonth,iDay,iHour,iMinute,iSecond,iMuSecond,iRefDateDest,oSeconds)

       INTEGER*4 iYear,
       INTEGER*4 iMonth,
       INTEGER*4 iDay,
       INTEGER*4 iHour,
       INTEGER*4 iMinute,
       INTEGER*4 iSecond,
       INTEGER*4 iMuSecond,
       INTEGER*4 iRefDateDest,
       INTEGER*4 oSeconds,

  \param[in] iYear : year
  \param[in] iMonth : month
  \param[in] iDay : day 
  \param[in] iHour : hour 
  \param[in] iMinute : minute 
  \param[in] iSecond : second
  \param[in] iMuSecond : micro-second 
  \param[in] iRefDateDest : date reference conversion (see #brathl_refDate)
  \param[out] oSeconds : number of seconds
  \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */

INTEGER4 FTN_NAME(brathlf_ymdhmsm2seconds, BRATHLF_YMDHMSM2SECONDS)
			(INTEGER4	*iYear,
			 INTEGER4	*iMonth,
			 INTEGER4	*iDay,
			 INTEGER4	*iHour,
			 INTEGER4	*iMinute,
			 INTEGER4	*iSecond,
			 INTEGER4	*iMuSecond,
			 INTEGER4	*iRefDateDest,
			 REAL8		*oSeconds)
{
  int32_t result = BRATHL_SUCCESS;

  brathl_DateYMDHMSM dateYMDHMSM;
  brathl_refDate refDate;
  brathl_DateSecond dateSeconds;

  dateYMDHMSM.year	= *iYear;
  dateYMDHMSM.month	= *iMonth;
  dateYMDHMSM.day	= *iDay;
  dateYMDHMSM.hour	= *iHour;
  dateYMDHMSM.minute	= *iMinute;
  dateYMDHMSM.second	= *iSecond;
  dateYMDHMSM.muSecond	= *iMuSecond;

  refDate	= *iRefDateDest;
  result	= brathl_YMDHMSM2Seconds(&dateYMDHMSM, refDate, &dateSeconds);
  *oSeconds	= dateSeconds.nbSeconds;
  return result;
}

/*
**----------------------------------
*/

/** Converts julian date into into a year, month, day, hour, minute, second, microsecond date, according to refDate parameter
    INTEGER*4 FUNCTION brathl_Julian2YMDHMSM(iRefDateSrc,iJulian,oYear,oMonth,oDay,oHour,oMinute,oSecond,oMuSecond)
    \par Fortran specification

       INTEGER*4 iRefDateSrc,
       REAL*8 iJulian,
       INTEGER*4 oYear,
       INTEGER*4 oMonth,
       INTEGER*4 oDay,
       INTEGER*4 oHour,
       INTEGER*4 oMinute,
       INTEGER*4 oSecond,
       INTEGER*4 oMuSecond,

  \param[in] iRefDateSrc : source date reference (see #brathl_refDate)
  \param[in] iJulian : julian date
  \param[out] oYear : year 
  \param[out] oMonth : month 
  \param[out] oDay : day 
  \param[out] oHour : hour convert
  \param[out] oMinute : minute to convert
  \param[out] oSecond : second to convert
  \param[out] oMuSecond : micro-second to convert
  \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */

INTEGER4 FTN_NAME(brathlf_julian2ymdhmsm, BRATHLF_JULIAN2YMDHMSM)
			(INTEGER4	*iRefDateSrc,
			 REAL8		*iJulian,
			 INTEGER4	*oYear,
			 INTEGER4	*oMonth,
			 INTEGER4	*oDay,
			 INTEGER4	*oHour,
			 INTEGER4	*oMinute,
			 INTEGER4	*oSecond,
			 INTEGER4	*oMuSecond)
{
  int32_t result = BRATHL_SUCCESS;

  brathl_DateYMDHMSM dateYMDHMSM;
  brathl_DateJulian dateJulian;

  dateJulian.refDate	= *iRefDateSrc;
  dateJulian.julian	= *iJulian;

  result	= brathl_Julian2YMDHMSM(&dateJulian, &dateYMDHMSM);

  *oYear	= dateYMDHMSM.year;
  *oMonth	= dateYMDHMSM.month;
  *oDay		= dateYMDHMSM.day;
  *oHour	= dateYMDHMSM.hour;
  *oMinute	= dateYMDHMSM.minute;
  *oSecond	= dateYMDHMSM.second;
  *oMuSecond	= dateYMDHMSM.muSecond;

  return result;
}

/*
**----------------------------------
*/
/** Converts a year, month, day, hour, minute, second, microsecond date  into a decimal julian date,  according to refDate parameter
      \par Fortran specification
    INTEGER*4 FUNCTION brathlf_YMDHMSM2Julian(iYear,iMonth,iDay,iHour,iMinute,iSecond,iMuSecond,iRefDateDest,oJulian)

       INTEGER*4 iYear,
       INTEGER*4 iMonth,
       INTEGER*4 iDay,
       INTEGER*4 iHour,
       INTEGER*4 iMinute,
       INTEGER*4 iSecond,
       INTEGER*4 iMuSecond,
       INTEGER*4 iRefDateDest,
       REAL*8 oJulian,

  \param[in] iYear : year
  \param[in] iMonth : month
  \param[in] iDay : day 
  \param[in] iHour : hour 
  \param[in] iMinute : minute 
  \param[in] iSecond : second
  \param[in] iMuSecond : micro-second 
  \param[in] iRefDateDest : date reference conversion (see #brathl_refDate)
  \param[out] oJulian : julian date
  \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */

INTEGER4 FTN_NAME(brathlf_ymdhmsm2julian, BRATHLF_YMDHMSM2JULIAN)
			(INTEGER4	*iYear,
			 INTEGER4	*iMonth,
			 INTEGER4	*iDay,
			 INTEGER4	*iHour,
			 INTEGER4	*iMinute,
			 INTEGER4	*iSecond,
			 INTEGER4	*iMuSecond,
			 INTEGER4	*iRefDateDest,
			 REAL8		*oJulian)
{
  int32_t result = BRATHL_SUCCESS;

  brathl_DateYMDHMSM dateYMDHMSM;
  brathl_refDate refDate;
  brathl_DateJulian dateJulian;

  dateYMDHMSM.year	= *iYear;
  dateYMDHMSM.month	= *iMonth;
  dateYMDHMSM.day	= *iDay;
  dateYMDHMSM.hour	= *iHour;
  dateYMDHMSM.minute	= *iMinute;
  dateYMDHMSM.second	= *iSecond;
  dateYMDHMSM.muSecond	= *iMuSecond;

  refDate	= *iRefDateDest;
  result	= brathl_YMDHMSM2Julian(&dateYMDHMSM, refDate, &dateJulian);
  *oJulian	= dateJulian.julian;
  return result;
}

/*
**----------------------------------
*/
/** Gets the current date/time,
     \par Fortran specification
    INTEGER*4 FUNCTION brathlf_NowYMDHMSM(oYear,oMonth,oDay,oHour,oMinute,oSecond,oMuSecond)
       
       INTEGER*4 oYear,
       INTEGER*4 oMonth,
       INTEGER*4 oDay,
       INTEGER*4 oHour,
       INTEGER*4 oMinute,
       INTEGER*4 oSecond,
       INTEGER*4 oMuSecond,

  \param[out] oYear : year 
  \param[out] oMonth : month 
  \param[out] oDay : day 
  \param[out] oHour : hour convert
  \param[out] oMinute : minute to convert
  \param[out] oSecond : second to convert
  \param[out] oMuSecond : micro-second to convert
  \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */

INTEGER4 FTN_NAME(brathlf_nowymdhmsm, BRATHLF_NOWYMDHMSM)
			(INTEGER4	*oYear,
			 INTEGER4	*oMonth,
			 INTEGER4	*oDay,
			 INTEGER4	*oHour,
			 INTEGER4	*oMinute,
			 INTEGER4	*oSecond,
			 INTEGER4	*oMuSecond)
{
  int32_t result = BRATHL_SUCCESS;

  brathl_DateYMDHMSM dateYMDHMSM;

  result	= brathl_NowYMDHMSM(&dateYMDHMSM);
  *oYear	= dateYMDHMSM.year;
  *oMonth	= dateYMDHMSM.month;
  *oDay		= dateYMDHMSM.day;
  *oHour	= dateYMDHMSM.hour;
  *oMinute	= dateYMDHMSM.minute;
  *oSecond	= dateYMDHMSM.second;
  *oMuSecond	= dateYMDHMSM.muSecond;

  return result;
}

/*
**----------------------------------
*/
/** Retrieves the day of year of a date
      \par Fortran specification
    INTEGER*4 FUNCTION brathlf_DayOfYear(iYear,iMonth,iDay,iHour,iMinute,iSecond,iMuSecond,iRefDateDest,oSeconds)

       INTEGER*4 iYear,
       INTEGER*4 iMonth,
       INTEGER*4 iDay,
       INTEGER*4 iHour,
       INTEGER*4 iMinute,
       INTEGER*4 iSecond,
       INTEGER*4 iMuSecond,
       INTEGER*4 oQuant,

  \param[in] iYear : year
  \param[in] iMonth : month
  \param[in] iDay : day 
  \param[in] iHour : hour 
  \param[in] iMinute : minute 
  \param[in] iSecond : second
  \param[in] iMuSecond : micro-second 
  \param[out] oQuant : day of year 
  \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */

INTEGER4 FTN_NAME(brathlf_dayofyear, BRATHLF_DAYOFYEAR)
			(INTEGER4	*iYear,
			 INTEGER4	*iMonth,
			 INTEGER4	*iDay,
			 INTEGER4	*iHour,
			 INTEGER4	*iMinute,
			 INTEGER4	*iSecond,
			 INTEGER4	*iMuSecond,
			 INTEGER4	*oQuant)
{
  int32_t result = BRATHL_SUCCESS;

  brathl_DateYMDHMSM dateYMDHMSM;

  dateYMDHMSM.year	= *iYear;
  dateYMDHMSM.month	= *iMonth;
  dateYMDHMSM.day	= *iDay;
  dateYMDHMSM.hour	= *iHour;
  dateYMDHMSM.minute	= *iMinute;
  dateYMDHMSM.second	= *iSecond;
  dateYMDHMSM.muSecond	= *iMuSecond;
  result		= brathl_DayOfYear(&dateYMDHMSM, (uint32_t *)oQuant);
  return result;
}


/*
**----------------------------------
*/
/**  Computes the difference between two dates (date1 - date2). The result is expressed in a decimal number of seconds
    \par Fortran specification
    INTEGER*4 FUNCTION brathl_DiffYMDHMSM(iYear1,iMonth1,iDay1,iHour1,iMinute1,iSecond1,iMuSecond1,iYear2,iMonth2,iDay2,iHour2,iMinute2,iSecond2,iMuSecond2)

       INTEGER*4 iYear1,
       INTEGER*4 iMonth1,
       INTEGER*4 iDay1,
       INTEGER*4 iHour1,
       INTEGER*4 iMinute1,
       INTEGER*4 iSecond1,
       INTEGER*4 iMuSecond1,
       INTEGER*4 iYear2,
       INTEGER*4 iMonth2,
       INTEGER*4 iDay2,
       INTEGER*4 iHour2,
       INTEGER*4 iMinute2,
       INTEGER*4 iSecond2,
       INTEGER*4 iMuSecond2,
       REAL*8 diff

  \param[in] iYear1 : year
  \param[in] iMonth1 : month
  \param[in] iDay1 : day 
  \param[in] iHour1 : hour 
  \param[in] iMinute1 : minute 
  \param[in] iSecond1 : second
  \param[in] iMuSecond1 : micro-second 
  \param[in] iYear2 : year
  \param[in] iMonth2 : month
  \param[in] iDay2 : day 
  \param[in] iHour2 : hour 
  \param[in] iMinute2 : minute 
  \param[in] iSecond2 : second
  \param[in] iMuSecond2 : micro-second 
  \param[out] diff : difference in seconds (date1 - date2)
  \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */

INTEGER4 FTN_NAME(brathlf_diffymdhmsm, BRATHLF_DIFFYMDHMSM)
			(INTEGER4	*iYear1,
			 INTEGER4	*iMonth1,
			 INTEGER4	*iDay1,
			 INTEGER4	*iHour1,
			 INTEGER4	*iMinute1,
			 INTEGER4	*iSecond1,
			 INTEGER4	*iMuSecond1,
			 INTEGER4	*iYear2,
			 INTEGER4	*iMonth2,
			 INTEGER4	*iDay2,
			 INTEGER4	*iHour2,
			 INTEGER4	*iMinute2,
			 INTEGER4	*iSecond2,
			 INTEGER4	*iMuSecond2,
			 REAL8		*diff)
{
  int32_t result = BRATHL_SUCCESS;

  brathl_DateYMDHMSM d1;
  brathl_DateYMDHMSM d2;

  d1.year	= *iYear1;
  d1.month	= *iMonth1;
  d1.day	= *iDay1;
  d1.hour	= *iHour1;
  d1.minute	= *iMinute1;
  d1.second	= *iSecond1;
  d1.muSecond	= *iMuSecond1;

  d2.year	= *iYear2;
  d2.month	= *iMonth2;
  d2.day	= *iDay2;
  d2.hour	= *iHour2;
  d2.minute	= *iMinute2;
  d2.second	= *iSecond2;
  d2.muSecond	= *iMuSecond2;

  result	= brathl_DiffYMDHMSM(&d1, &d2, diff);
  return result;
}

/*
**----------------------------------
*/
/**  Computes the difference between two dates (date1 - date2). The result is expressed in a decimal number of seconds
    \par Fortran specification
    INTEGER*4 FUNCTION brathl_DiffDSM(iRefDate1,iDays1,iSeconds1,iMuSeconds1,iRefDate2,iDays2,iSeconds2,iMuSeconds2,diff)

       INTEGER*4 iRefDate1,
       INTEGER*4 iDays1,
       INTEGER*4 iSeconds1,
       INTEGER*4 iMuSeconds1,
       INTEGER*4 iRefDate2,
       INTEGER*4 iDays2,
       INTEGER*4 iSeconds2,
       INTEGER*4 iMuSeconds2
       REAL*8 diff

  \param[in] iRefDate1 : source date reference (see #brathl_refDate)
  \param[in] iDays1 : numbers of days
  \param[in] iSeconds1 : number of seconds
  \param[in] iMuSeconds1 : numbers of microseconds
  \param[in] iRefDate2 : source date reference (see #brathl_refDate)
  \param[in] iDays2 : numbers of days
  \param[in] iSeconds2 : number of seconds
  \param[in] iMuSeconds2 : numbers of microseconds
  \param[out] diff : difference in seconds (date1 - date2)
  \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */

INTEGER4 FTN_NAME(brathlf_diffdsm, BRATHLF_DIFFDSM)
			(INTEGER4	*iRefDate1,
			 INTEGER4	*iDays1,
			 INTEGER4	*iSeconds1,
			 INTEGER4	*iMuSeconds1,
			 INTEGER4	*iRefDate2,
			 INTEGER4	*iDays2,
			 INTEGER4	*iSeconds2,
			 INTEGER4	*iMuSeconds2,
			 REAL8		*diff)
{
  int32_t result = BRATHL_SUCCESS;

  brathl_DateDSM d1;
  brathl_DateDSM d2;

  d1.refDate	= *iRefDate1;
  d1.days	= *iDays1;
  d1.seconds	= *iSeconds1;
  d1.muSeconds	= *iMuSeconds1;

  d2.refDate	= *iRefDate2;
  d2.days	= *iDays2;
  d2.seconds	= *iSeconds2;
  d2.muSeconds	= *iMuSeconds2;

  result	= brathl_DiffDSM(&d1, &d2, diff);
  return result;
}

/*
**----------------------------------
*/
/**  Computes the difference between two dates (date1 - date2). The result is expressed in a decimal number of seconds
    \par Fortran specification
    INTEGER*4 FUNCTION brathl_DiffJulian(iRefDate1,iJulian1,iRefDate2,iJulian2,diff)

       INTEGER*4 iRefDate1,
       REAL*8 iJulian1,
       INTEGER*4 iRefDate2,
       REAL*8 iJulian2,
       REAL*8 diff

  \param[in] iRefDate1 : source date reference (see #brathl_refDate)
  \param[in] iJulian1 : julian date
  \param[in] iRefDate2 : source date reference (see #brathl_refDate)
  \param[in] iJulian2 : julian date
  \param[out] diff : difference in seconds (date1 - date2)
  \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */

INTEGER4 FTN_NAME(brathlf_diffjulian, BRATHLF_DIFFJULIAN)
			(INTEGER4	*iRefDate1,
			 REAL8		*iJulian1,
			 INTEGER4	*iRefDate2,
			 REAL8		*iJulian2,
			 REAL8		*diff)
{
  int32_t result = BRATHL_SUCCESS;

  brathl_DateJulian d1;
  brathl_DateJulian d2;

  d1.refDate	= *iRefDate1;
  d1.julian	= *iJulian1;

  d2.refDate	= *iRefDate2;
  d2.julian	= *iJulian2;

  result	= brathl_DiffJulian(&d1, &d2, diff);
  return result;
}

/*
**----------------------------------
*/
/** Converts a cyle/pass into a date
    
    \par Fortran specification
    INTEGER*4 FUNCTION brathl_Cycle2YMDHMSM(iMission,iCycle,iPass,oYear,oMonth,oDay,oHour,oMinute,oSecond,oMuSecond)

       INTEGER*4 iMission,
       INTEGER*4 iCycle,
       INTEGER*4 iPass,
       INTEGER*4 oYear,
       INTEGER*4 oMonth,
       INTEGER*4 oDay,
       INTEGER*4 oHour,
       INTEGER*4 oMinute,
       INTEGER*4 oSecond,
       INTEGER*4 oMuSecond,

  \param[in] iMission : mission type (see #brathl_mission)
  \param[in] iCycle : number of cycle to convert
  \param[in] iPass : number of pass in the cycle  to convert
  \param[out] oYear : year
  \param[out] oMonth : month
  \param[out] oDay : day 
  \param[out] oHour : hour 
  \param[out] oMinute : minute 
  \param[out] oSecond : second
  \param[out] oMuSecond : micro-second 

  \return #BRATHL_SUCCESS or error code (see \ref Cycle_date_error_codes) */

INTEGER4 FTN_NAME(brathlf_cycle2ymdhmsm, BRATHLF_CYCLE2YMDHMSM)
			(INTEGER4	*iMission,
			 INTEGER4	*iCycle,
			 INTEGER4	*iPass,
			 INTEGER4	*oYear,
			 INTEGER4	*oMonth,
			 INTEGER4	*oDay,
			 INTEGER4	*oHour,
			 INTEGER4	*oMinute,
			 INTEGER4	*oSecond,
			 INTEGER4	*oMuSecond)
{
  int32_t result = BRATHL_SUCCESS;

  brathl_DateYMDHMSM dateYMDHMSM;

  result	= brathl_Cycle2YMDHMSM((brathl_mission)*iMission, *iCycle, *iPass,  &dateYMDHMSM);
  *oYear	= dateYMDHMSM.year;
  *oMonth	= dateYMDHMSM.month;
  *oDay		= dateYMDHMSM.day;
  *oHour	= dateYMDHMSM.hour;
  *oMinute	= dateYMDHMSM.minute;
  *oSecond	= dateYMDHMSM.second;
  *oMuSecond	= dateYMDHMSM.muSecond;

  return result;
}

/*
**----------------------------------
*/

/** Converts a date into a cycle/pass
    \par Fortran specification
    INTEGER*4 FUNCTION brathl_YMDHMSM2Cycle(iMission,iYear,iMonth,iDay,iHour,iMinute,iSecond,iMuSecond,oCycle,oPass)

       INTEGER*4 iMission,
       INTEGER*4 iYear,
       INTEGER*4 iMonth,
       INTEGER*4 iDay,
       INTEGER*4 iHour,
       INTEGER*4 iMinute,
       INTEGER*4 iSecond,
       INTEGER*4 iMuSecond,
       INTEGER*4 oCycle,
       INTEGER*4 oPass,

  \param[in] iMission : mission type (see #brathl_mission)
  \param[in] iYear : year
  \param[in] iMonth : month
  \param[in] iDay : day 
  \param[in] iHour : hour 
  \param[in] iMinute : minute 
  \param[in] iSecond : second
  \param[in] iMuSecond : micro-second 
  \param[out] oCycle : number of cycle to convert
  \param[out] oPass : number of pass in the cycle  to convert

  \return #BRATHL_SUCCESS or error code (see \ref Cycle_date_error_codes) */

INTEGER4 FTN_NAME(brathlf_ymdhmsm2cycle, BRATHLF_YMDHMSM2CYCLE)
			(INTEGER4	*iMission,
			 INTEGER4	*iYear,
			 INTEGER4	*iMonth,
			 INTEGER4	*iDay,
			 INTEGER4	*iHour,
			 INTEGER4	*iMinute,
			 INTEGER4	*iSecond,
			 INTEGER4	*iMuSecond,
			 INTEGER4	*oCycle,
			 INTEGER4	*oPass)
{
  int32_t result = BRATHL_SUCCESS;

  brathl_DateYMDHMSM dateYMDHMSM;
  uint32_t cycleTmp	= 0;
  uint32_t passTmp	= 0;

  dateYMDHMSM.year	= *iYear;
  dateYMDHMSM.month	= *iMonth;
  dateYMDHMSM.day	= *iDay;
  dateYMDHMSM.hour	= *iHour;
  dateYMDHMSM.minute	= *iMinute;
  dateYMDHMSM.second	= *iSecond;
  dateYMDHMSM.muSecond	= *iMuSecond;

  result	= brathl_YMDHMSM2Cycle((brathl_mission)*iMission,
				       &dateYMDHMSM,
      		      	      	       &cycleTmp,
				       &passTmp);

  *oCycle	= cycleTmp;
  *oPass	= passTmp;
  return result;
}

/**@}*/





/** \addtogroup data_readingFortran Fortran API for reading data
 @{*/



/*
**----------------------------------
*/

/** Read data from a set of files
    Each measure for a data is a scalar value (a single number)

    \par Fortran specification
    INTEGER*4 FUNCTION brathlf_ReadData(nbFiles,<br>
    $                                   fileNames,<br>
    $                                   recordName,<br>
    $                                   selection,<br>
    $                                   nbData,<br>
    $                                   dataExpressions,<br>
    $                                   units,<br>
    $                                   results,<br>
    $                                   size,<br>
    $                                   actualSize,<br>
    $                                   ignoreOutOfRange,<br>
    $                                   statistics,<br>
    $                                   defaultValue,<br>
	INTEGER*4	nbFiles<br>
	CHARACTER*(*)	fileNames(nbFiles)
	CHARACTER*(*)	recordName
	CHARACTER*(*)	selection
	INTEGER*4	nbData
	CHARACTER*(*)	dataExpressions(nbData)
	CHARACTER*(*)	units(nbData)
	REAL*8		results(size,nbData)
	INTEGER*4	size
	INTEGER*4	actualSize
	INTEGER*4	ignoreOutOfRange
	INTEGER*4	statistics
	REAL*8		defaultValue

    \param[in] nbFiles : Number of files in file name list
		This is the usable size of #fileNames
    \param[in] fileNames : File name list
		Must contain at least #nbFiles entries. If an entry is an
		empty string, the entry is ignored.
    \param[in] selection : Expression involving data fields which has to be true to select returned data
		if it is an empty string no selection is done (all data is
		selected)
    \param[in] nbData : Number of expressions used to retreive data
    \param[in] dataExpressions : Expression applyed to data fields to build the wanted value
		Must contain at least #nbData entries. If an entry is an empty
		string, the data returned are always default values.
    \param[in] units : Wanted unit for each expression
		Must contain at least #nbData entries.
		If an entry is an empty string, no unit conversion is applyed
		to the data of the corresponding expression.
		When a unit conversion has to be applyed, the result of the
		expression is considered to be the base unit (SI). For example
		if the wanted unit is gram/l, the unit of the expression is
		supposed to be kilogram/m3 (internaly all data are converted
		to base unit of the actual fields unit which is coherent with
		the above assumption).
    \param[out] results : Data read
		Must be a matrix of at least #nbData entries of size values.
		Must be declared as real*8 results[size, N] where N >= #nbData
    \param[in] size : Number of data in each #results entry.
		Must be >=0. If 0, nothing is returned in results and results
		can be anything.
    \param[out] actualSize : Number of actual data needed to store result.
		The actual number of values in the corresponding entry of
		#results are returned in this number.
		If size if 0 this the number of values that would have been
		returned.
    \param[in] ignoreOutOfRange : Skip excess data.
		0=false, other = true
		If true, #actualSize can be greater than any positive value of
		#sizes, if there is too much value to store they are ignored.
		If false, it generates an error.
    \param[in] statistics : returns statistics on data instead of data themselves
		0=false, other = true
		If statistics is true, ignoreOutOfRange must be false.
		And sizes must be <=0 or >=5. The returned values for each
		expression are:
		- <B>Count</B> of <I>valid</I> data taken into account.
			Invalid data are those which are equal to the
			default/missing value
		- <B>Mean</B> of the valid data.
		- <B>Standard deviation</B> of the valid data
		- <B>Minimum</B> value of the valid data
		- <B>Maximum</B> value of the valid data
		.
		In this case actualSize always returns 5
    \param[in] defaultValue : value to use for default/missing values
		This is the value you want to indicate that a value
		is missing or invalid.
    \return #BRATHL_SUCCESS or error code */
INTEGER4 FTN_NAME(brathlf_readdata, BRATHLF_READDATA)
		(INTEGER4	*nbFiles,
		 char		*fileNames,
		 char		*recordName,
		 char		*selection,
		 INTEGER4	*nbData,
		 char		*dataExpressions,
		 char		*units,
		 REAL8		*results,
		 INTEGER4	*size,
		 INTEGER4	*actualSize,
		 INTEGER4	*ignoreOutOfRange,
		 INTEGER4	*statistics,
		 REAL8		*defaultValue,
		 INTEGER4	ftn_Length1,
		 INTEGER4	ftn_Length2,
		 INTEGER4	ftn_Length3,
		 INTEGER4	ftn_Length4,
		 INTEGER4	ftn_Length5
		 )
{
  int32_t	result			= BRATHL_SUCCESS;
  int32_t	Index;
  char		**CFileNames		= GetFtnStringArray(fileNames, ftn_Length1, *nbFiles);
  char		*CRecordName		= GetFtnString(recordName, ftn_Length2);
  char		*CSelection		= GetFtnString(selection, ftn_Length3);
  char		**CDataExpressions	= GetFtnStringArray(dataExpressions, ftn_Length4, *nbData);
  char		**CUnits		= GetFtnStringArray(units, ftn_Length5, *nbData);
  int32_t	*CSizes			= malloc(sizeof(*CSizes)*(*nbData));
  double	**CResults		= malloc(sizeof(*CResults)*(*nbData));

  if (
	(CFileNames		== NULL) ||
	(CRecordName		== NULL) ||
	(CSelection		== NULL) ||
	(CDataExpressions	== NULL) ||
	(CUnits			== NULL) ||
	(CSizes			== NULL) ||
	(CResults		== NULL)
      )
  {
    result	= BRATHL_MEMORY_ERROR;
  }
  else
  {

    if (*size > 0)
    {
      for (Index=0; Index < *nbData; Index++)
      {
	CSizes[Index]	= *size;
	CResults[Index]	= results + (*size * Index);
      }
    }
    else
    {
      /*
      ** Normally it should not happen from fortran (nonsense) but it is
      ** consistent with C calls
      */
      free(CSizes);
      free(CResults);
      CSizes	= NULL;
      CResults	= NULL;
    }

    result	= brathl_ReadData(*nbFiles,
				  CFileNames,
				  CRecordName,
				  CSelection,
				  *nbData,
				  CDataExpressions,
				  CUnits,
				  CResults,
				  CSizes,
				  actualSize,
				  *ignoreOutOfRange,
				  *statistics,
				  *defaultValue);
  }


  free(CFileNames);
  free(CRecordName);
  free(CSelection);
  free(CDataExpressions);
  free(CUnits);
  free(CSizes);
  free(CResults);
  return result;
}

/*
**----------------------------------
*/


/**@}*/

