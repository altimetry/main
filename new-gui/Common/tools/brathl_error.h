/*
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
#ifndef BRAT_ERROR_H
#define BRAT_ERROR_H

#if defined (Q_UNUSED)
    #define UNUSED Q_UNUSED
#else
    #if defined(Q_CC_INTEL) && !defined(Q_OS_WIN) || defined(Q_CC_RVCT)
    template <typename T>
    inline void bratUnused(T &x) { (void)x; }
    #  define UNUSED(x) bratUnused(x);
    #else
    #  define UNUSED(x) (void)x;
    #endif
#endif

/*
** Used to verify format functions via compiler but only available
** under gnu C compiler
*/
#ifndef __GNUC__
#define __attribute__(x)
#endif


#if defined( __cplusplus )
extern "C" {
#endif  /* __cplusplus */

/** \file 
  BRATHL error codes
 */

/** \defgroup Error_codes Error codes 
  @{ */
/** Success - no error */
#define BRATHL_SUCCESS	      	      	      		0
/// warning
#define BRATHL_WARNING	      	      	      		2
/// General error
#define BRATHL_ERROR					-1
/// Syntax error
#define BRATHL_SYNTAX_ERROR	      	      	      	-2
/// Unit error
#define BRATHL_UNIT_ERROR	      	      	      	-3
/// Count error
#define BRATHL_COUNT_ERROR	      	      	      	-4
/// Range error
#define BRATHL_RANGE_ERROR	      	      	      	-5
/// Limit error
#define BRATHL_LIMIT_ERROR	      	      	      	-6
/// I/O error
#define BRATHL_IO_ERROR		      	      	      	-7
/// Memory error
#define BRATHL_MEMORY_ERROR		      	      	-8
/// System error
#define BRATHL_SYSTEM_ERROR	      	      	      	-9
/// Logic error (program error)
#define BRATHL_LOGIC_ERROR	      	      	      	-10
/// Inconsistency error
#define BRATHL_INCONSISTENCY_ERROR	      	      	-11
/// error for non non implement code
#define BRATHL_UNIMPLEMENT_ERROR                        -12


/** \defgroup Date_error_codes Date error codes 
  @{ */
/// Invalid date 
#define BRATHL_ERROR_INVALID_DATE  			-101
/// Invalid reference date 
#define BRATHL_ERROR_INVALID_DATE_REF  			-102
/// Invalid reference date conversion
#define BRATHL_ERROR_INVALID_DATE_REF_CONV    	      	-103
/// Invalid days or seconds or museonds values (must be > 0)
#define BRATHL_ERROR_INVALID_DSM      	      	        -104
/// Invalid year value (must be >= 1950)
#define BRATHL_ERROR_INVALID_YEAR     	      	        -105
/// Invalid month value (must be >= 1 and <= 12)
#define BRATHL_ERROR_INVALID_MONTH     	      	        -106
/// Invalid day value
#define BRATHL_ERROR_INVALID_DAY     	      	        -107
/// Invalid hour value (must be >= 0 and <= 23)
#define BRATHL_ERROR_INVALID_HOUR   	      	        -108
/// Invalid minute value (must be >= 0 and <= 59)
#define BRATHL_ERROR_INVALID_MINUTE    	      	        -109
/// Invalid second value (must be >= 0 and <= 59)
#define BRATHL_ERROR_INVALID_SECOND     	      	-110
/// Invalid musecond value (must be >= 0 and <= 999999)
#define BRATHL_ERROR_INVALID_MUSECOND     	      	-111
/// Invalid date (date must be > 0)
#define BRATHL_ERROR_INVALID_DATE_NEGATIVE     	      	-112
/** @} */

/** \defgroup Cycle_date_error_codes Cycle/date conversion error codes
  @{ */
/// Invalid nb pass (must be > 0)
#define BRATHL_ERROR_INVALID_NB_PASS     	      	-201
/// Invalid repetition (must be > 0)
#define BRATHL_ERROR_INVALID_REPETITION  	      	-202
/// Unknown mission value
#define BRATHL_ERROR_INVALID_MISSION  	      	      	-203
/// WARNING - Unable to open reference mission file
#define BRATHL_WARNING_OPEN_FILE_REF_FILE         	-204
/// WARNING - Invalid reference mission file format
#define BRATHL_WARNING_INVALID_REF_FILE_FIELD         	-205
/// WARNING - Invalid reference mission date
#define BRATHL_WARNING_INVALID_REF_FILE_FIELDDATE     	-206
/// WARNING - Unable to open alias mission file
#define BRATHL_WARNING_OPEN_FILE_ALIAS_MISSION         	-207

/** @} */

/** @} */


#if defined( __cplusplus )
}
#endif  /* __cplusplus */


#endif // !defined(BRAT_ERROR_H)
