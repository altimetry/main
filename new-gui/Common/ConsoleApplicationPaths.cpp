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
// #include "stdafx.h"

// While we can't include stdafx.h, this pragma must be here
//
#if defined (__APPLE__)
#pragma clang diagnostic ignored "-Wdeprecated-register"
#endif

#include <QCoreApplication>

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


const std::string QT_PLUGINS_SUBDIR =

#if defined(_WIN32)
    "plugins"
#elif defined (__APPLE__)
    "../PlugIns"
#else
    "plugins"
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




CConsoleApplicationPaths::CConsoleApplicationPaths( const std::string &exec_path, const std::string &user_docs_dir ) :

    // I. NOT user (re)definable paths

      mPlatform( PLATFORM_SUBDIR )
    , mConfiguration( CONFIG_SUBDIR )

    , mExecutablePath( exec_path )
    , mExecutableDir( GetDirectoryFromPath( mExecutablePath ) )			// (*)
    , mDeploymentRootDir( GetDirectoryFromPath( mExecutableDir ) )
    , mQtPluginsDir( mExecutableDir + "/" + QT_PLUGINS_SUBDIR )
	, mUserManualPath( mDeploymentRootDir + "/doc/brat_user_manual_" + BRAT_VERSION + ".pdf" )

    , mInternalDataDir( ComputeInternalDataDirectory( mExecutableDir ) )
	, mUserDocumentsDirectory( user_docs_dir )
{
    // Set Qt plug-ins path before anything else
	//
	//	- Use QCoreApplication::libraryPaths(); to inspect Qt library (plug-ins) directories

    QCoreApplication::setLibraryPaths( QStringList() << mQtPluginsDir.c_str() );	


    // user (RE)DEFINABLE paths
	//
    SetUserPaths();

    ValidatePaths();
}
// (*) this achieves the same (but, among other problems, needs
//	an instance and is not Qt 5 portable): qApp->argv()[ 0 ]


//virtual
std::string CConsoleApplicationPaths::ToString() const
{
    std::string s = std::string( "\n*** ApplicationPaths ***\nValidation Result: " ) + ( mValid ? "Ok" : "Failed" );

	if ( !mErrorMsg.empty() ) s+= ( "\nValidation Messages: " + mErrorMsg + "\n\nData:\n\n" );

	//base

    s += ( "\nPlatform == " + mPlatform );
    s += ( "\nConfiguration == " + mConfiguration );

    s += ( "\nExecutable Path == " + mExecutablePath );
    s += ( "\nExecutable Dir == " + mExecutableDir );
    s += ( "\nDeployment Root Dir == " + mDeploymentRootDir );

    s += ( "\nQt Plugins Dir == " + mQtPluginsDir );
	s += ( "\nUser Manual Path == " + mUserManualPath );
    s += ( "\nInternalData Dir == " + mInternalDataDir );

    s += ( std::string( "\nUse Portable Paths == " ) + ( UsePortablePaths() ? "true" : "false" ) );

	s += ( "\nUser Documents Directory == " + mUserDocumentsDirectory );
    s += ( "\nmUser Data Directory == " + mUserDataDirectory );
    s += ( "\nWorkspaces Directory == " + mWorkspacesDirectory );

    return s;
}


// Test here mandatory paths only. User changeable paths are not tested, because
//	by definition they are not critical
//
bool CConsoleApplicationPaths::ValidatePaths() const
{
	mValid = 
        ValidPath( mErrorMsg, mQtPluginsDir, false, "Qt Plugins directory" ) &&
		ValidPath( mErrorMsg, mInternalDataDir, false, "BRAT resources directory" );


    return mValid;
}


bool CConsoleApplicationPaths::SetUserDataDirectory( bool portable, const std::string &path )
{
    if ( !IsDir( path ) && !MakeDirectory( path ) )
        return false;

    mUsePortablePaths = portable;

    mUserDataDirectory = path;

    return true;
}


bool CConsoleApplicationPaths::SetWorkspacesDirectory( const std::string &path )
{
    if ( !IsDir( path ) && !MakeDirectory( path ) )
        return false;

    mWorkspacesDirectory = path;

    return true;
}


bool CConsoleApplicationPaths::SetUserPaths()
{
	if ( mUserDocumentsDirectory.empty() )	//we are in a console application or the client does not need user directories
		return true;

    const std::string wkspaces = mUserDocumentsDirectory + "/workspaces";
    const std::string data = mUserDocumentsDirectory + "/user-data";

    if ( !IsDir( mUserDocumentsDirectory ) && !MakeDirectory( mUserDocumentsDirectory ) )
        return false;

	//set defaults taking "mUserDocumentsDirectory" as root if existing values don't exist

    return 
		( IsDir( mUserDataDirectory ) || SetUserDataDirectory( mUsePortablePaths, data ) ) &&
		( IsDir( mWorkspacesDirectory ) || SetWorkspacesDirectory( wkspaces ) );
}
