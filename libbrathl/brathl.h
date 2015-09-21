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
#ifndef _brathl_h_
#define _brathl_h_

#if defined( __cplusplus )
extern "C" {
#endif  /* __cplusplus */

#include <stdio.h>


/*
** includes configuration parameters
*/
#include "brathl_config.h"


    
#if GCC_VERSION < 40600				//4.6.0
#define nullptr NULL
#endif
    


#ifdef WIN32

#include <windows.h>
#ifdef __WXMSW__
#   include "wx/msw/winundef.h"
#endif
#include <direct.h>    

/* include sys/stat.h because we are going to override stat */
#include <sys/stat.h>
    
#ifndef S_ISDIR
#define S_ISDIR(X) (((X) & _S_IFDIR) == _S_IFDIR)
#endif
    
typedef signed char		int8_t;
typedef short int		int16_t;
typedef int			int32_t;
typedef __int64	      	      	int64_t;

typedef unsigned char		uint8_t;
typedef unsigned short int	uint16_t;
typedef unsigned int		uint32_t;
typedef unsigned __int64	uint64_t;

#endif /* ifdef WIN32 */

#if defined(WIN32) && defined(LIBRATHLDLL)
# ifdef LIBRATHLDLL_EXPORTS
#  define LIBRATHL_API __declspec(dllexport)
# else
#  define LIBRATHL_API __declspec(dllimport)
# endif
#else
# define LIBRATHL_API
#endif

#if HAVE_INTTYPES_H
#include <inttypes.h>
#else
# if HAVE_STDINT_H
#  include <stdint.h>
# else
#  if HAVE_SYS_TYPES_H
#   include <sys/types.h>
#  endif
# endif
#endif

#if HAVE_SYS_STAT_H
#  include <sys/stat.h>
#endif    

#if HAVE_LIBGEN_H
#  include <libgen.h>  /* for basename and dirname */
#endif

#ifdef WIN32
#  define BRATHL_PATH_MAX _MAX_PATH
#else
#  define BRATHL_PATH_MAX PATH_MAX
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2         1.57079632679489661923  /* pi/2 */
#endif


#ifndef M_PI_4
#define M_PI_4         0.78539816339744830962  /* pi/4 */
#endif

/** \file
  C/C++ general interface of BRATHL
 */


/*
**
** File open mode
**
*/
typedef enum {
	ReadOnly,	/** file exists, open read-only */
	Write,		/** file exists, open for writing */
        Replace,	/** create new file, even if it already exists */
	New		/** create new file, fail if it already exists */
} brathl_FileMode;



/** Maximum length of date reference string */
#define BRATHL_REF_DATE_USER_LEN 28

/** Maximum length of error message string */
#define BRATHL_MAX_ERRMSG_LEN 255

/** Maximum length of date reference string */
#define BRATHL_CYCLE_LEN 60

/** Global variable to define REFUSER1 date (see #brathl_refDate) */
LIBRATHL_API extern char brathl_refDateUser1[BRATHL_REF_DATE_USER_LEN];
/** Global varaiable to define REFUSER2 date (see #brathl_refDate) */
LIBRATHL_API extern char brathl_refDateUser2[BRATHL_REF_DATE_USER_LEN];

/** date reference enumeration
  Used to give a date a a start reference
  User can defined its own reference by using REFUSER1 and/or REFUSER2
*/
typedef enum {
                REF19500101,   /**< reference to the 1950-01-01 00:00:00:00 */
                REF19580101,   /**< reference to the 1958-01-01 00:00:00:00 */
                REF19850101,   /**< reference to the 1985-01-01 00:00:00:00 */
                REF19900101,   /**< reference to the 1990-01-01 00:00:00:00 */
                REF20000101,   /**< reference to the 2000-01-01 00:00:00:00 */
                REFUSER1,      /**< reference to a user-defined date #brathl_refDateUser1*/
                REFUSER2       /**< reference to a second user-defined date #brathl_refDateUser2*/
      } brathl_refDate;

/** YYYY-MM-DD HH:MN:SS:MS date structure
*/
typedef struct _structDateYMDHMSM
{
	uint32_t year;
	uint32_t month;
	uint32_t day;
	uint32_t hour;
	uint32_t minute;
	uint32_t second;
	uint32_t muSecond;

}brathl_DateYMDHMSM;  	    /**< Creates a type name for _structDateYMDHMSM */

/** Day/seconds/microseconds date structure
*/
typedef struct _structDateDSM
{
	brathl_refDate refDate;  /**< date reference  (see #brathl_refDate) */
	int32_t days; 	      	 /**< numbers of days */
	int32_t seconds;      	 /**< numbers of seconds */
	int32_t muSeconds;    	 /**< numbers of microseconds */

} brathl_DateDSM;     	  /**< Creates a type name for _structDateDSM */

/** Decimal seconds date structure
*/
typedef struct _structDateSecond
{
	brathl_refDate refDate;   /**< date reference  (see #brathl_refDate) */
	double nbSeconds;     	  /**< numbers of seconds/microseconds */

} brathl_DateSecond;  	/**< Creates a type name for _structDateSecond */

/** Decimal julian date structure
*/
typedef struct _structDateJulian
{
	brathl_refDate refDate;   /**< date reference  (see #brathl_refDate) */
	double julian;	      	  /**< decimal julian day */

} brathl_DateJulian;  	/**< Creates a type name for _structDateJulian */



/** Satellite (mission) enumeration
*/
typedef enum {
                TOPEX,	      	/**< Topex/Poseidon */
                JASON2,	      	/**< Jason-2 */
                JASON1,	      	/**< Jason-1 */
                ERS2,	      	/**< ERS2*/
                ENVISAT,	/**< Envisat */
                ERS1_A,	      	/**< ERS1-A */
                ERS1_B,	      	/**< ERS1-B */
		GFO	      	/**< GFO */
      } brathl_mission;


typedef enum
{
    EARTH_ROTATION = 0,
    LIGHT_SPEED,
    EARTH_GRAVITY,
    EARTH_RADIUS,
    ELLIPSOID_PARAM
} brathl_global_constants;

/*
** Used to verify format functions via compiler but only available
** under gnu C compiler
*/
#ifndef __GNUC__
#define __attribute__(x)
#endif


#if defined( __cplusplus )
}
#endif  /* __cplusplus */

#endif /* !defined(_brathl_h_) */
