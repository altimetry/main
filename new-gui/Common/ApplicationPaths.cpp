// #include "stdafx.h"

// While we can't include stdafx.h, this pragma must be here
//
#if defined (__APPLE__)
#pragma clang diagnostic ignored "-Wdeprecated-register"
#endif


#include <osgDB/Registry>

#include "ApplicationPaths.h"

#include "brathl_config.h"



////////////////////////////////////////////////////////////////////////////////////////////
//
//									CApplicationPaths
//
////////////////////////////////////////////////////////////////////////////////////////////

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


const std::string QGIS_PLUGINS_SUBDIR =

#if defined(_WIN32)
    "plugins"
#elif defined (__APPLE__)
    "../PlugIns/qgis"           //"QGIS.app/Contents/PlugIns/qgis"
#else
    "plugins"
#endif
;


const std::string osg_paths_SUBDIR = "osgPlugins-3.4.0";





//static
std::string CApplicationPaths::ComputeDefaultUserBasePath( const std::string &DeploymentRootDir )
{
    auto s3root = getenv( "S3ALTB_ROOT" );	//assuming development environment available
    if ( s3root )
    {
        std::string s = s3root + std::string( "/project" );
        if ( IsDir( s ) )
            return s;
    }

    assert__( IsDir( DeploymentRootDir ) );

    return DeploymentRootDir;
}


//static
std::string CApplicationPaths::ComputeInternalDataDirectory( const std::string &ExecutableDir )
{
    std::string InternalDataDir;
    auto s3data = getenv( BRATHL_ENVVAR );	// TODO dependent of brathl_config.h: get rid of this file
    if ( s3data )
        InternalDataDir = s3data;
    else
    {
        InternalDataDir = ExecutableDir;    //stripped from first parent directory (MacOS in mac)
        InternalDataDir += "/" + DefaultInternalDataSubDir();
    }
    return InternalDataDir;
}


CApplicationPaths::CApplicationPaths( const QString &exec_path ) :

    // I. NOT user (re)definable paths

      mPlatform( PLATFORM_SUBDIR )
    , mConfiguration( CONFIG_SUBDIR )

    , mExecutablePath( q2a( exec_path ) )
    , mExecutableDir( GetDirectoryFromPath( mExecutablePath ) )			// (*)
    , mDeploymentRootDir( GetDirectoryFromPath( mExecutableDir ) )

    , mInternalDataDir( ComputeInternalDataDirectory( mExecutableDir ) )
    , mVectorLayerPath( mInternalDataDir + "/maps/ne_10m_coastline/ne_10m_coastline.shp" )

    , mOsgPluginsDir( mExecutableDir + "/" + osg_paths_SUBDIR )

    , mQtPluginsDir( mExecutableDir + "/" + QT_PLUGINS_SUBDIR )
    , mQgisPluginsDir( mExecutableDir + "/" + QGIS_PLUGINS_SUBDIR )
    , mGlobeDir( mExecutableDir + "/" + DefaultGlobeSubDir() )
{
    assert__( valid() );

    // Set Qt plugins

    auto list = QCoreApplication::libraryPaths();
    for ( auto const&d : list)
        qDebug() << d;

    //qDebug() << QLibraryInfo::location(QLibraryInfo::PluginsPath);
    //QCoreApplication::addLibraryPath( mQtPluginsDir.c_str() );
    QCoreApplication::setLibraryPaths( QStringList() << mQtPluginsDir.c_str() );

    auto const &list2 = QCoreApplication::libraryPaths();
    for ( auto const&d : list2)
        qDebug() << d;

    osgDB::FilePathList &osg_paths = osgDB::Registry::instance()->getLibraryFilePathList();
    osg_paths.push_front( mOsgPluginsDir );

    for ( auto const &s : osg_paths )
        qDebug() << t2q( s );


    // II. user (RE)DEFINABLE paths

    validate();
}
// (*) this achieves the same (but, among other problems, needs
//	an instance and is not Qt 5 portable): qApp->argv()[ 0 ]


bool CApplicationPaths::operator == ( const CApplicationPaths &o ) const
{
    assert__( mPlatform == o.mPlatform );
    assert__( mConfiguration == o.mConfiguration );

    assert__( mExecutablePath == o.mExecutablePath );
    assert__( mExecutableDir == o.mExecutableDir );
    assert__( mDeploymentRootDir == o.mDeploymentRootDir );

    assert__( mInternalDataDir == o.mInternalDataDir );
    assert__( mVectorLayerPath == o.mVectorLayerPath );

    assert__( mOsgPluginsDir == o.mOsgPluginsDir );
    assert__( mQtPluginsDir == o.mQtPluginsDir );
    assert__( mQgisPluginsDir == o.mQgisPluginsDir );
    assert__( mGlobeDir == o.mGlobeDir );

    return
        // user re-definable

        mUserBasePath == o.mUserBasePath &&
        mExternalDataDir == o.mExternalDataDir &&
        mRasterLayerPath == o.mRasterLayerPath &&

        mWorkspacesDir == o.mWorkspacesDir
        ;
}



std::string CApplicationPaths::ToString() const
{
    std::string s;

    s += ( "\nPlatform == " + mPlatform );
    s += ( "\nConfiguration == " + mConfiguration );

    s += ( "\nExecutable Path == " + mExecutablePath );
    s += ( "\nExecutable Dir == " + mExecutableDir );
    s += ( "\nDeploymentRoot Dir == " + mDeploymentRootDir );

    s += ( "\nInternalData Dir == " + mInternalDataDir );
    s += ( "\nVectorLayer Path == " + mVectorLayerPath );

    s += ( "\nOsgPlugins Dir == " + mOsgPluginsDir );
    s += ( "\nQtPlugins Dir == " + mQtPluginsDir );
    s += ( "\nQgisPlugins Dir == " + mQgisPluginsDir );
    s += ( "\nGlobe Dir == " + mGlobeDir );

    s += ( "\nUserBase Path == " + mUserBasePath );
    s += ( "\nExternalData Dir == " + mExternalDataDir );
    s += ( "\nRasterLayer Path == " + mRasterLayerPath );

    s += ( "\nWorkspaces Dir == " + mWorkspacesDir );

    s += ( std::string( "\nUnique UserBasePath == " ) + ( mUniqueUserBasePath ? "true" : "false" ) );

    return s;
}



bool CApplicationPaths::valid() const
{
    return
        IsDir( mInternalDataDir ) && IsFile( mVectorLayerPath ) &&
        IsDir( mOsgPluginsDir ) && IsDir( mQtPluginsDir ) && IsDir( mQgisPluginsDir ) &&
        IsDir( mGlobeDir );
}


// If "unique" is true, path must exist
//
bool CApplicationPaths::SetUniqueUserBasePath( bool unique, const std::string &path )	//path = ""
{
    if ( unique == mUniqueUserBasePath )
        return true;

    if ( !unique )
    {
        mUniqueUserBasePath = false;
        return true;
    }
    else
    if ( !IsDir( path ) )
        return false;

    std::string external = path + "/" + DefaultExternalDataSubDir();
    std::string wkspaces = path + "/" + DefaultProjectsSubDir();

    if ( !MakeDirectory( external ) || !MakeDirectory( wkspaces ) )
        return false;

    mUserBasePath = path;
    mExternalDataDir = external;
    mWorkspacesDir = wkspaces;

    CopyRasterLayerFile();

    mUniqueUserBasePath = true;
    return true;
}


// When mUniqueUserBasePath is true, SetUniqueUserBasePath
//	is the only setter accepted. Otherwise, these setters
//	can be used to make individual changes to paths.

bool CApplicationPaths::SetExternalDataDirectory( const std::string &path )
{
    if ( UniqueUserBasePath() )
        return false;

    if ( !IsDir( path ) && !MakeDirectory( path ) )
        return false;

    mExternalDataDir = path;

    CopyRasterLayerFile();

    return true;
}


bool CApplicationPaths::SetWorkspacesDirectory( const std::string &path )
{
    if ( UniqueUserBasePath() )
        return false;

    if ( !IsDir( path ) && !MakeDirectory( path ) )
        return false;

    mWorkspacesDir = path;
    return true;
}


bool CApplicationPaths::validate()
{
    if ( !IsDir( mUserBasePath ) )
        mUserBasePath = ComputeDefaultUserBasePath( mDeploymentRootDir );

    if ( mUniqueUserBasePath )
    {
        mUniqueUserBasePath = false;	//force validation & assignment
        return SetUniqueUserBasePath( true, mUserBasePath );
    }
    else
    {
        std::string external = mUserBasePath + "/" + DefaultExternalDataSubDir();
        std::string wkspaces = mUserBasePath + "/" + DefaultProjectsSubDir();

        if ( !IsDir( mExternalDataDir ) && !SetExternalDataDirectory( external ) )
            return false;

        if ( !IsDir( mWorkspacesDir ) && !SetWorkspacesDirectory( wkspaces ) )
            return false;
    }

    return true;
}


