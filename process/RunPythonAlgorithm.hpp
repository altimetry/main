#ifndef RUNPYTHONALGORITHM_TOOLS_HPP
#define RUNPYTHONALGORITHM_TOOLS_HPP


#ifndef __cplusplus
#error Must use C++ for RunPythonAlgorithm
#endif


//////////////////////////////////////////////////////////////////
//					DECLARE_ARRAY_SIZE
//////////////////////////////////////////////////////////////////


#define ARRAY_SIZE(a) (sizeof(a) / sizeof(*a))

#define DECLARE_ARRAY_SIZE_DEPRECATED(a) size_t a##_size = ARRAY_SIZE(a)

#define DECLARE_ARRAY_SIZE_(a) size_t a##_size

#define DEFINE_ARRAY_SIZE(a) DECLARE_ARRAY_SIZE_(a) = ARRAY_SIZE(a)



//////////////////////////////////////////////////////////////
//				System (C standard Library)
//////////////////////////////////////////////////////////////


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <algorithm>
#include <fstream>


//////////////////////////////////////////////////////////////
//						File Utilities
//////////////////////////////////////////////////////////////


inline bool IsFile( const char *name )
{
    if (FILE *file = fopen(name, "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
}


struct MatchPathSeparator
{
    bool operator()( char ch ) const
    {
#if defined (WIN32) || defined (_WIN32)
        return ch == '\\' || ch == '/';
#else
        return ch == '/';
#endif
    }
};

inline std::string
removeExtension( std::string const& filename )
{
    std::string::const_reverse_iterator pivot =
            std::find( filename.rbegin(), filename.rend(), '.' );
    return pivot == filename.rend()
        ? filename
        : std::string( filename.begin(), pivot.base() - 1 );
}

inline std::string
filename_from_path( std::string const& path )
{
    return std::string(
        std::find_if( path.rbegin(), path.rend(),
                      MatchPathSeparator() ).base(),
        path.end() );
}

inline std::string
basename_from_path( std::string const& path )
{
    return removeExtension( filename_from_path( path ) );
}

inline std::string
dir_from_filepath( std::string const& path )
{
    std::string filename = filename_from_path( path );

    return path.substr( 0, path.find( filename ) );
}


//////////////////////////////////////////////////////////////
//						String Utilities
//////////////////////////////////////////////////////////////

template< class T >
inline T atoi( const std::string& s )
{
    T v;
    std::stringstream( s ) >> v;
    return v;
}


//////////////////////////////////////////////////////////////
//						Brathl Header
//////////////////////////////////////////////////////////////

#if defined (_MSC_VER)
#pragma warning ( disable : 4267 )
#endif

#include "BratAlgorithmBase.h"

#if defined (_MSC_VER)
#pragma warning ( default : 4267 )
#endif


//////////////////////////////////////////////////////////////
//						Python Headers
//////////////////////////////////////////////////////////////

// Mess caused by including Python.h

// I. avoid redefinition warnings (lots of them): save definitions

#if !defined(WIN32)
#define SAVE_XOPEN_SOURCE   _XOPEN_SOURCE
#define SAVE_POSIX_C_SOURCE _POSIX_C_SOURCE
#undef _XOPEN_SOURCE
#undef _POSIX_C_SOURCE
#endif

#if defined (HAVE_STAT)				// HAVE_STAT is defined by brathl_config.h
#define SAVE_HAVE_STAT HAVE_STAT
#undef HAVE_STAT				
#endif


// II. include Python headers


#include <patchlevel.h>

#if PY_VERSION_HEX < 0x03040300
#if defined(_MSC_VER) && (_MSC_VER >= 1800)
#define HAVE_ROUND
#endif
#endif

//#define Py_NO_ENABLE_SHARED	0

#if defined (_DEBUG)            //avoid automatic inclusion of pythonX_Y.lib (see .....\Python\default\include\pyconfig.h)
#undef _DEBUG
#include <Python.h>				//must come before Qt headers: has a struct with a member named "slots"...
#define _DEBUG
#else
#include <Python.h>				//must come before Qt headers: has a struct with a member named "slots"...
#endif


// III. avoid redefinition warnings: reset definitions

#if !defined(WIN32)
#undef _XOPEN_SOURCE
#undef _POSIX_C_SOURCE
#define _XOPEN_SOURCE   SAVE_XOPEN_SOURCE
#define _POSIX_C_SOURCE SAVE_POSIX_C_SOURCE
#endif

#if !defined (HAVE_STAT)
#define HAVE_STAT SAVE_HAVE_STAT
#endif



#endif //RUNPYTHONALGORITHM_TOOLS_HPP
