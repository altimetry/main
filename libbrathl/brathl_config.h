#ifndef _brathl_config_h_
#define _brathl_config_h_

#define BRAT_VERSION             "4.0.0-beta"

// -------------------------------
// Result check for various functions...
// -------------------------------

#if defined (WIN32) || defined (_WIN32)

/* #undef HAVE_SNPRINTF */
#define HAVE_USCORE_SNPRINTF       1
#define HAVE_VSNPRINTF             1
#define HAVE_USCORE_VSNPRINTF      1
/* #undef HAVE_STRCASECMP */
#define HAVE_USCORE_STRICMP        1
/* #undef HAVE_ISINF */
#define HAVE_FPCLASS               1
/* #undef HAVE_ISNAN */
#define HAVE_USCORE_ISNAN          1
#define HAVE_FULLPATH              1
/* #undef HAVE_REALPATH */
/* #undef HAVE_DIRNAME */
/* #undef HAVE_BASENAME */
#define HAVE_SPLITPATH             1
#define HAVE_STAT                  1


// ---------------------------------
// Result check for optional include files
// ---------------------------------
/* #undef HAVE_UNISTD_H */
#define HAVE_INTTYPES_H            1
#define HAVE_STDINT_H              1
#define HAVE_SYS_TYPES_H           1
#define HAVE_SYS_STAT_H            1
#define HAVE_INTTYPES_H            1
/* #undef HAVE_LIBGEN_H */



#define PATH_SEPARATOR             "\\"
#ifdef PATH_SEPARATOR
#define PATH_SEPARATOR_CHAR             '\\'
#endif
#define PATH_LIST_SEPARATOR        ";"
#ifdef PATH_LIST_SEPARATOR
#define PATH_LIST_SEPARATOR_CHAR        ';'
#endif
#define BRATHL_ENVVAR              "BRAT_DATA_DIR"
#define BRATHL_DEFDATADIR          "L:/project/dev/bin/x64/Release//data"
#define BRATHL_UNITFILE            "brathl_units.dat"



#else 	// not WIN32

#define HAVE_SNPRINTF              1
/* #undef HAVE_USCORE_SNPRINTF */
#define HAVE_VSNPRINTF             1
/* #undef HAVE_USCORE_VSNPRINTF */
#define HAVE_STRCASECMP            1
/* #undef HAVE_USCORE_STRICMP */
#define HAVE_ISINF                 1
/* #undef HAVE_FPCLASS */
#define HAVE_ISNAN                 1
/* #undef HAVE_USCORE_ISNAN */
/* #undef HAVE_FULLPATH */
#define HAVE_REALPATH              1
#define HAVE_DIRNAME               1
#define HAVE_BASENAME              1
/* #undef HAVE_SPLITPATH */
#define HAVE_STAT                  1


// ---------------------------------
// Result check for optional include files
// ---------------------------------
#define HAVE_UNISTD_H              1
#define HAVE_INTTYPES_H            1
#define HAVE_STDINT_H              1
#define HAVE_SYS_TYPES_H           1
#define HAVE_SYS_STAT_H            1
#define HAVE_INTTYPES_H            1
#define HAVE_LIBGEN_H              1



#define PATH_SEPARATOR             "/"
#ifdef PATH_SEPARATOR
#define PATH_SEPARATOR_CHAR             '/'
#endif
#define PATH_LIST_SEPARATOR        ":"
#ifdef PATH_LIST_SEPARATOR
#define PATH_LIST_SEPARATOR_CHAR        ':'
#endif
#define BRATHL_ENVVAR              "BRAT_DATA_DIR"
#define BRATHL_DEFDATADIR          "/home/brat/s3-altb/project/dev/bin/x86_64/Release//data"


#endif	// not WIN32


#define BRATHL_UNITFILE            "brathl_units.dat"

/* #undef WORDS_BIGENDIAN */



#endif // !defined(_brathl_config_h_)
