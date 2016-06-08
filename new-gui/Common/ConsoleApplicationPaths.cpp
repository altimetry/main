// #include "stdafx.h"

// While we can't include stdafx.h, this pragma must be here
//
#if defined (__APPLE__)
#pragma clang diagnostic ignored "-Wdeprecated-register"
#endif


#include "+UtilsIO.h"
#include "QtUtilsIO.h"
#include "BratVersion.h"
#include "ConsoleApplicationPaths.h"

#include "libbrathl/Tools.h"



////////////////////////////////////////////////////////////////////////////////////////////
//
//									CConsoleApplicationPaths
//
////////////////////////////////////////////////////////////////////////////////////////////



const std::string BRATHL_ENVVAR = "BRAT_DATA_DIR";		//v3.1.0 legacy



const std::string PLATFORM_SUBDIR =

#if defined(BRAT_ARCHITECTURE_64)
    #if defined(_WIN32)
        "x64"
    #else
        "x86_64"
    #endif
#else
    #if !defined(BRAT_ARCHITECTURE_32)
    #error One of BRAT_ARCHITECTURE_32 BRAT_ARCHITECTURE_64 must be defined
    #endif
    #if defined(_WIN32)
        "Win32"
    #else
        "i386"
    #endif
#endif
;


const std::string CONFIG_SUBDIR =

#if defined(_DEBUG) || defined(DEBUG)
    "Debug"
#else
    "Release"
#endif
;


//static
std::string CConsoleApplicationPaths::ComputeInternalDataDirectory( const std::string &ExecutableDir )
{
    std::string InternalDataDir;
    auto s3data = getenv( BRATHL_ENVVAR.c_str() );
    if ( s3data )
        InternalDataDir = s3data;
    else
    {
        InternalDataDir = ExecutableDir;    //stripped from first parent directory (MacOS in mac)
        InternalDataDir += "/" + DefaultInternalDataSubDir();
    }
    return InternalDataDir;
}


//static
bool CConsoleApplicationPaths::ValidPath( std::string &error_msg, const std::string &path, bool is_file, const std::string path_title )
{
	if ( is_file ? !IsFile( path ) : !IsDir( path ) )
	{
		error_msg = 
			( is_file ? std::string( "File: ") : std::string("Directory: ") ) 
			+ path_title + " " 
			+ path 
			+ std::string(" is not valid.");

		return false;
	}
	return true;
}




CConsoleApplicationPaths::CConsoleApplicationPaths( const std::string &exec_path ) :

    // I. NOT user (re)definable paths

      mPlatform( PLATFORM_SUBDIR )
    , mConfiguration( CONFIG_SUBDIR )

    , mExecutablePath( exec_path )
    , mExecutableDir( GetDirectoryFromPath( mExecutablePath ) )			// (*)
    , mDeploymentRootDir( GetDirectoryFromPath( mExecutableDir ) )
	, mUserManualPath( mDeploymentRootDir + "/doc/brat_user_manual_" + BRAT_VERSION + ".pdf" )

    , mInternalDataDir( ComputeInternalDataDirectory( mExecutableDir ) )
{
    ValidatePaths();
}
// (*) this achieves the same (but, among other problems, needs
//	an instance and is not Qt 5 portable): qApp->argv()[ 0 ]


// Test here mandatory paths only. User changeable paths are not tested, because
//	by definition they are not critical
//
bool CConsoleApplicationPaths::ValidatePaths() const
{
	mValid = ValidPath( mErrorMsg, mInternalDataDir, false, "BRAT resources directory" );
    return mValid;
}
