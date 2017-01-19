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
#ifndef _brathlc_h_
#define _brathlc_h_

#if defined( __cplusplus )
extern "C" {
#endif  /* __cplusplus */



/*
** includes configuration parameters
*/
#include "brathl.h"
#include "common/ccore-types.h"
#include "common/tools/brathl_error.h"



/** \file
  C general interface of BRATHL
 */


/** Global variable to save error code  */
LIBRATHL_API extern int brathl_errno;


/** Retrieve a string with the error description

With a few exceptions almost all BRATHL functions return an integer that indicate whether the function
was able to perform its operations successfully.
The return value will be 0 on success and < 0 otherwise. The result is also save in the global variable #brathl_errno
In case you get a negative value.

\param[in] err : error code
\return  string error description
*/
LIBRATHL_API const char* brathl_Errno2String(const int32_t err);


/** \addtogroup date_conversionC Date conversion C APIs
 @{*/

/** Converts seconds into a days-seconds-microseconds date, according to refDate parameter
  \param[in] dateSeconds : date to convert
  \param[in] refDate : date reference conversion
  \param[out] dateDSM : result of the conversion
  \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
LIBRATHL_API int32_t brathl_Seconds2DSM(brathl_DateSecond *dateSeconds, brathl_refDate refDate,
					brathl_DateDSM *dateDSM);
/** Converts a date in days-seconds-microseconds into a seconds, according to refDate parameter
  \param[in] dateDSM : date to convert
  \param[in] refDate : date reference conversion
  \param[out] dateSeconds : result of the conversion
  \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
LIBRATHL_API int32_t brathl_DSM2Seconds(brathl_DateDSM *dateDSM, brathl_refDate refDate,
      	      	      	      	        brathl_DateSecond *dateSeconds);

/** Converts a decimal julian date into a days-seconds-microseconds date, according to refDate parameter
  \param[in] dateJulian : date to convert
  \param[in] refDate : date reference conversion
  \param[out] dateDSM : result of conversion
  \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
LIBRATHL_API int32_t brathl_Julian2DSM(brathl_DateJulian *dateJulian, brathl_refDate refDate,
      	      	      	      	       brathl_DateDSM *dateDSM);

/** Converts a days-seconds-microseconds date into a decimal julian date, according to refDate parameter
  \param[in] dateDSM : date to convert
  \param[in] refDate : date reference conversion
  \param[out] dateJulian : result of the conversion
  \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
LIBRATHL_API int32_t brathl_DSM2Julian(brathl_DateDSM *dateDSM, brathl_refDate refDate,
      	      	      	      	       brathl_DateJulian *dateJulian);

/** Converts a year, month, day, hour, minute, second, microsecond date into a days-seconds-microseconds date,
   according to refDate parameter
  \param[in] dateYMDHMSM : date to convert
  \param[in] refDate : date reference conversion
  \param[out] dateDSM : result of the conversion
  \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
LIBRATHL_API int32_t brathl_YMDHMSM2DSM(brathl_DateYMDHMSM *dateYMDHMSM, brathl_refDate refDate,
      	      	      	      	       brathl_DateDSM *dateDSM);

/** Converts a days-seconds-microseconds date into a year, month, day, hour, minute, second, microsecond date
  \param[in] dateDSM : date to convert
  \param[out] dateYMDHMSM : result of the conversion
  \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
LIBRATHL_API int32_t brathl_DSM2YMDHMSM(brathl_DateDSM *dateDSM,
      	      	      	      	        brathl_DateYMDHMSM *dateYMDHMSM);

/** Converts seconds into a decimal julian date, according to refDate parameter
  \param[in] dateSeconds : date to convert
  \param[in] refDate : date reference conversion
  \param[out] dateJulian : result of the conversion
  \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
LIBRATHL_API int32_t brathl_Seconds2Julian(brathl_DateSecond *dateSeconds, brathl_refDate refDate,
      	      	      	      	           brathl_DateJulian *dateJulian);

/** Converts a decimal julian date into seconds, according to refDate parameter
  \param[in] dateJulian : date to convert
  \param[in] refDate : date reference conversion
  \param[out] dateSeconds : result of the conversion
  \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
LIBRATHL_API int32_t brathl_Julian2Seconds(brathl_DateJulian *dateJulian, brathl_refDate refDate,
      	      	      	      	           brathl_DateSecond *dateSeconds);


/** Converts seconds into a year, month, day, hour, minute, second, microsecond date
  \param[in] dateSeconds : date to convert
  \param[out] dateYMDHMSM : result of the conversion
  \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
LIBRATHL_API int32_t brathl_Seconds2YMDHMSM(brathl_DateSecond *dateSeconds,
      	      	      	      	        brathl_DateYMDHMSM *dateYMDHMSM);

/** Converts a year, month, day, hour, minute, second, microsecond date  into seconds,
   according to refDate parameter
  \param[in] dateYMDHMSM : date to convert
  \param[in] refDate : date reference conversion
  \param[out] dateSeconds : result of the conversion
  \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
LIBRATHL_API int32_t brathl_YMDHMSM2Seconds(brathl_DateYMDHMSM *dateYMDHMSM, brathl_refDate refDate,
      	      	      	      	       brathl_DateSecond *dateSeconds);

/** Converts a decimal julian date into a year, month, day, hour, minute, second, microsecond date
  \param[in] dateJulian : date to convert
  \param[out] dateYMDHMSM : result of the conversion
  \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
LIBRATHL_API int32_t brathl_Julian2YMDHMSM(brathl_DateJulian *dateJulian,
      	      	      	      	        brathl_DateYMDHMSM *dateYMDHMSM);

/** Converts a year, month, day, hour, minute, second, microsecond date  into a decimal julian date,
   according to refDate parameter
  \param[in] dateYMDHMSM : date to convert
  \param[in] refDate : date reference conversion
  \param[out] dateJulian : result of the conversion
  \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
LIBRATHL_API int32_t brathl_YMDHMSM2Julian(brathl_DateYMDHMSM *dateYMDHMSM, brathl_refDate refDate,
      	      	      	      	       brathl_DateJulian *dateJulian);


/** Gets the current date/time,
   \param[out] dateYMDHMSM : current date/time
  \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
LIBRATHL_API int32_t brathl_NowYMDHMSM(brathl_DateYMDHMSM *dateYMDHMSM);


/** Retrieves the day of year of a date
   \param[in] dateYMDHMSM : date
   \param[out] dayOfYear : day of year of the date parameter
  \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
LIBRATHL_API int32_t brathl_DayOfYear(brathl_DateYMDHMSM *dateYMDHMSM, uint32_t *dayOfYear);

/**  Computes the difference between two dates (date1 - date2)
      the result is expressed in a decimal number of seconds
   \param[in] dateYMDHMSM1 : date1
   \param[in] dateYMDHMSM2 : date2
   \param[out] diff : difference in seconds (date1 - date2)
  \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
LIBRATHL_API int32_t brathl_DiffYMDHMSM(brathl_DateYMDHMSM *dateYMDHMSM1, brathl_DateYMDHMSM *dateYMDHMSM2, double *diff);

/**  Computes the difference between two dates (date1 - date2)
      the result is expressed in a decimal number of seconds
   \param[in] dateDSM1 : date1
   \param[in] dateDSM2 : date2
   \param[out] diff : difference in seconds (date1 - date2)
  \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
LIBRATHL_API int32_t brathl_DiffDSM(brathl_DateDSM *dateDSM1, brathl_DateDSM *dateDSM2, double *diff);

/**  Computes the difference between two dates (date1 - date2)
      the result is expressed in a decimal number of seconds
   \param[in] dateJulian1 : date1
   \param[in] dateJulian2 : date2
   \param[out] diff : difference in seconds (date1 - date2)
  \return #BRATHL_SUCCESS or error code (see \ref Date_error_codes) */
LIBRATHL_API int32_t brathl_DiffJulian(brathl_DateJulian *dateJulian1, brathl_DateJulian *dateJulian2, double *diff);

/**@}*/

/*
**---------------- Cycle/Date conversions
*/
/** \addtogroup date_conversionC
 @{*/

/** Converts a cyle/pass into a date
    \param[in] mission : mission type (see #brathl_mission)
    \param[in] cycle : number of cycle to convert
    \param[in] pass : number of pass in the cycle  to convert
    \param[out] dateYMDHMSM : date corresponding to the cycle/pass
    \return #BRATHL_SUCCESS or error code (see \ref Cycle_date_error_codes) */
LIBRATHL_API int32_t brathl_Cycle2YMDHMSM(brathl_mission mission, uint32_t cycle, uint32_t pass,
      	      	      	      	      	  brathl_DateYMDHMSM *dateYMDHMSM);

/** Converts a date into a cycle/pass
    \param[in] mission : mission type (see #brathl_mission)
    \param[in] dateYMDHMSM : date to convert
    \param[out] cycle : number of cycle
    \param[out] pass : number of pass in the cycle
    \return #BRATHL_SUCCESS or error code (see \ref Cycle_date_error_codes) */
LIBRATHL_API int32_t brathl_YMDHMSM2Cycle(brathl_mission mission, brathl_DateYMDHMSM *dateYMDHMSM,
      	      	      	      	          uint_t *cycle, uint_t *pass);

/**@}*/

/** \addtogroup data_readingC C API for reading data
 @{*/

/** Read data from a set of files
    Each measure for a data is a scalar value (a single number)
    \param[in] nbFiles : Number of files in file name list
		This is the usable size of #fileNames
    \param[in] fileNames : File name list
		Must contain at least #nbFiles entries. If an entry is NULL or
		points to an empty string, the entry is ignored.
    \param[in] selection : Expression involving data fields which has to be true to select returned data
		if NULL or empty string no selection is done (all data is
		selected)
    \param[in] nbData : Number of expression used to retreive data
    \param[in] dataExpressions : Expression applyed to data fields to build the wanted value
		Must contain at least #nbData entries. If an entry is NULL or
		points to an empty string, the data returned are always default
		values.
    \param[in] units : Wanted unit for each expression
		Must be NULL or contain at least #nbData entries.
		If NULL, no unit conversion is done.
		If an entry is NULL or points to an empty string, no unit
		conversion is applyed to the data of the corresponding
		expression. When a unit conversion has to be applyed, the result
		of the expression is considered to be the base unit (SI). For
		example if the wanted unit is gram/l, the unit of the expression
		is supposed to be kilogram/m3 (internaly all data are converted
		to base unit of the actual fields unit which is coherent with
		the above assumption).
    \param results [in/out] : Data read
		Must be a vector of at least #nbData pointers (entries) to
		values to read.
		If NULL, nothing is returned in results and sizes MUST be NULL
		(otherwise this is an error).
		An entry can be NULL, see #sizes for the actual behaviour.
    \param sizes [in/out] : Number of allocated values in a #results entry.
		Must be a vector of at least #nbData integers.
		If NULL, results MUST also be NULL (otherwise this is an error).
		If a value is 0, nothing is returned.
		If a value is > 0, the corresponding entry in results must not
		be NULL and must have been allocated to be able to store as much
		float values as indicated.
		If a value is < 0, and the corresponding entry in results is
		NULL, the entry will be allocated with enough space to
		store the result and sizes modified to reflect the size of
		allocated data (may be more than actual used ones).
		If a value is < 0, and the corresponding entry in results is
		not NULL, this is an error.
    \param[out] actualSize : Number of actual data needed to store result.
		It cannot be NULL.
		The actual number of values in the corresponding entry of
		#results are returned in this number (all entries need
		the same amount of result).
		If #result is NULL, the number of values which would be needed
		for each entry is returned.
    \param[in] ignoreOutOfRange : Skip excess data.
		0=false, other = true
		If true, #actualSize can be greater than any positive value
		of #sizes, if there is too much value to store they are ignored.
		If false, it generates an error.
		Has no effect on #sizes entries which are <= 0 (or if it is
		NULL).
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
LIBRATHL_API int32_t brathl_ReadData
		(int32_t	nbFiles,
		 char		**fileNames,
		 const char	*recordName,
		 const char	*selection,
		 int32_t	nbData,
		 char		**dataExpressions,
		 char		**units,
		 double		**results,
		 int32_t	sizes[],
		 size_t		*actualSize,
		 int		ignoreOutOfRange,
		 int		statistics,
		 double		defaultValue
		 );

LIBRATHL_API void brathl_RegisterAlgorithms();

LIBRATHL_API void brathl_LoadAliasesDictionary();

/**@}*/


#if defined( __cplusplus )
}
#endif  /* __cplusplus */

#endif /* !defined(_brathlc_h_) */
