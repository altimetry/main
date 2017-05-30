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
#include "ApplicationStaticPaths.h"

#include "libbrathl/Tools.h"



////////////////////////////////////////////////////////////////////////////////////////////
//
//									CApplicationStaticPaths
//
////////////////////////////////////////////////////////////////////////////////////////////






const std::string PLATFORM_SUBDIR =

#if defined(ARCHITECTURE_64)
    #if defined(_WIN32)
        "x64"
    #else
        "x86_64"
    #endif
#else
    #if !defined(ARCHITECTURE_32)
    #error One of ARCHITECTURE_32 ARCHITECTURE_64 must be defined
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
const std::string CApplicationStaticPaths::smPythonExecutableName =

#if defined(Q_OS_WIN)

        "python.exe";

#elif defined(Q_OS_LINUX )

        "python3.2mu";

#else
        "python3";

#endif



const std::string RSYNC_EXE	= setExecExtension( "rsync" );


//

CBratPath::CBratPath( const std::string &path, bool must_exist, const char *name )
	: mPath( NormalizedPath( path ) )
	, mMustExist( must_exist )
	, mName( name )
{}


CBratPath::operator const std::string& ( ) const
{
	return mPath;
}

CBratPath::operator std::string ( ) const
{
	return mPath;
}

bool CBratPath::operator == ( const CBratPath &o ) const
{
	return 
		mPath == o.mPath
		&&	mMustExist == o.mMustExist
		&&	mName == o.mName
		;
}

std::string CBratPath::ToString() const
{
	return mName + " == " + mPath + ( Validate() ? "" : " [INVALID]" );
}

bool CBratPath::Valid( std::string &error_msg ) const
{
	if ( !Validate() )
	{
		const bool is_file = dynamic_cast< const CFilePath* >( this );
		error_msg +=
			( is_file ? std::string( "\nFile: " ) : std::string( "\nDirectory: " ) )
			+ mName + " "
			+ mPath
			+ std::string( " is not valid." );

		return false;
	}
	return true;
}





std::string CFolderPath::operator + ( const std::string &str ) const
{
	return mPath + str;
}

bool CFolderPath::Validate() const
{
	return !mMustExist || IsDir( mPath );
}

bool CFilePath::Validate() const
{
	return !mMustExist || IsFile( mPath );
}



//


//static
std::string CApplicationStaticPaths::ComputeInternalDataDirectory( const std::string &ExecutableDir )
{
    std::string InternalDataDir;
    auto s3data = getenv( BRATHL_ENVVAR );
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
bool CApplicationStaticPaths::ValidPath( std::string &error_msg, const std::string &path, bool is_file, const std::string path_title )
{
	return is_file ? CFilePath( path, true, path_title.c_str() ).Valid( error_msg ) : CFolderPath( path, true, path_title.c_str() ).Valid( error_msg );
}






CApplicationStaticPaths::CApplicationStaticPaths( const std::string &exec_path, const std::string &app_name )
    : mValid( false )


    // I. NOT user (re)definable paths

    , mPlatform( PLATFORM_SUBDIR )
    , mConfiguration( CONFIG_SUBDIR )

    , mExecutablePath		( exec_path , true, "Executable File" )
	, mApplicationName		( app_name )
    , mExecutableDir		( GetDirectoryFromPath( mExecutablePath.mPath ), true, "Executable Folder" )			// (*)
    , mDeploymentRootDir	( GetDirectoryFromPath( mExecutableDir.mPath ), true, "Deployment Root Folder" )
    , mQtPluginsDir			( mExecutableDir + "/" + QT_PLUGINS_SUBDIR, true, "Qt Plug-ins Folder" )
	, mPythonDir			( mExecutableDir + "/Python", mApplicationName == BRAT_APPLICATION_NAME, "Python Folder" )
    , mUserManualPath		( mDeploymentRootDir + "/doc/brat_user_manual_" + BRAT_VERSION_STRING + ".pdf", true, "User Manual" )

    , mInternalDataDir		( ComputeInternalDataDirectory( mExecutableDir ), true, "Private Data Folder" )

#if defined(Q_OS_WIN)
    , mRsyncExecutablePath	(	EscapePath( mExecutableDir + "/rsync/" + RSYNC_EXE ), 
#else
    , mRsyncExecutablePath(	"/usr/bin/rsync", 
#endif
	true, "Rsync Executable" )

	, mRadsConfigurationFilePath	( mInternalDataDir + "/rads.ini", true, "RADS Configuration File" )
	, mRadsServicePanicLogFilePath	( DefaultUserDocumentsPath() + "/brat/RadsServiceCriticalLog.txt", false, "RADS Service Panic Log File" )

{
	// Set Qt plug-ins path before anything else
	//
	//	- Use QCoreApplication::libraryPaths(); to inspect Qt library (plug-ins) directories

    QCoreApplication::setLibraryPaths( QStringList() << mQtPluginsDir.mPath.c_str() );


    ValidatePaths();
}
// (*) the following achieves the same (but, among other problems,
//	needs an instance and is not Qt 5 portable): qApp->argv()[ 0 ]



std::string CApplicationStaticPaths::DefaultUserDocumentsPath() const
{
	return ::SystemUserDocumentsPath() + "/" + mApplicationName;
}


std::string CApplicationStaticPaths::DefaultUserSettingsPath4Application( const std::string&application_name, bool create ) const        //create = true
{
	std::string data = ::SystemUserSettingsPath();  //"/Users/brat/Library/Application Support/ESA"
	if ( !EndsWith( data, application_name ) )
	{
		if ( EndsWith( data, mApplicationName ) )
			data = GetDirectoryFromPath( data );

		data += ( "/" + application_name );
	}

	if ( create && !IsDir( data ) && !MakeDirectory( data ) )
	{
		mValid = false;
		mErrorMsg += ( "\nCould not create " +  data );
	}

	return data;
}


//virtual
std::string CApplicationStaticPaths::ToString() const
{
    std::string s = std::string( "\n*** ApplicationPaths ***\nValidation Result: " ) + ( mValid ? "Ok" : "Failed" );

	if ( !mErrorMsg.empty() ) s+= ( "\nValidation Messages: " + mErrorMsg + "\n\nData:\n\n" );

	//base

    s += ( "\nPlatform == " + mPlatform );
    s += ( "\nConfiguration == " + mConfiguration );

    s += ( "\n" + mExecutablePath.ToString() );
    s += ( "\n" + mExecutableDir.ToString() );
    s += ( "\n" + mDeploymentRootDir.ToString() );

    s += ( "\n" + mQtPluginsDir.ToString() );
    s += ( "\n" + mPythonDir.ToString() );
	s += ( "\n" + mUserManualPath.ToString() );
    s += ( "\n" + mInternalDataDir.ToString() );

	s += ( "\n" + mRsyncExecutablePath.ToString() );

    s += ( "\n" + mRadsConfigurationFilePath.ToString() );
	s += ( "\n" + mRadsServicePanicLogFilePath.ToString() );

    return s;
}

// Test here mandatory paths only. User changeable paths are not tested, because
//	by definition they are not critical
//
bool CApplicationStaticPaths::ValidatePaths() const
{
	mErrorMsg.clear();

	mValid =
			mExecutablePath.Valid( mErrorMsg )
		&&	mExecutableDir.Valid( mErrorMsg )
		&&	mDeploymentRootDir.Valid( mErrorMsg )

		&&	mQtPluginsDir.Valid( mErrorMsg )

		&&	mPythonDir.Valid( mErrorMsg )

		&&	mUserManualPath.Valid( mErrorMsg )

		&&	mInternalDataDir.Valid( mErrorMsg )

		&&	mRsyncExecutablePath.Valid( mErrorMsg )
		&&	mRadsServicePanicLogFilePath.Valid( mErrorMsg )
		&&	mRadsConfigurationFilePath.Valid( mErrorMsg )
            
		//	ValidPath( mErrorMsg, mQtPluginsDir, false, "Qt Plugins directory" ) 
		//&&	ValidPath( mErrorMsg, mInternalDataDir, false, "BRAT resources directory" ) 
		//&&	ValidPath( mErrorMsg, mRsyncExecutablePath, true, "rsync executable path" )
		//&&	ValidPath( mErrorMsg, mRadsConfigurationFilePath, true, "RADS configuration file" )
		;

    return mValid;
}
