// #include "stdafx.h"

// While we can't include stdafx.h, this pragma must be here
//
#if defined (__APPLE__)
#pragma clang diagnostic ignored "-Wdeprecated-register"
#endif


#include <osgDB/Registry>

#include "+UtilsIO.h"
#include "QtUtils.h"
#include "ApplicationPaths.h"




////////////////////////////////////////////////////////////////////////////////////////////
//
//									CApplicationPaths
//
////////////////////////////////////////////////////////////////////////////////////////////


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




const std::string BRATSHOWSTATS_EXE		= setExecExtension( "BratStats" );
const std::string BRATSCHEDULER_EXE		= setExecExtension( "scheduler" );
const std::string BRATEXPORTGEOTIFF_EXE = setExecExtension( "BratExportGeoTiff" );
const std::string BRATEXPORTASCII_EXE	= setExecExtension( "BratExportAscii" );
const std::string BRATCREATEZFXY_EXE	= setExecExtension( "BratCreateZFXY" );
const std::string BRATCREATEYFX_EXE		= setExecExtension( "BratCreateYFX" );




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



CApplicationPaths::CApplicationPaths( const QString &exec_path ) :

    // I. NOT user (re)definable paths

	base_t( q2a( exec_path ) )

    , mVectorLayerPath( mInternalDataDir + "/maps/ne_10m_coastline/ne_10m_coastline.shp" )

    , mOsgPluginsDir( mExecutableDir + "/" + osg_paths_SUBDIR )

    , mQtPluginsDir( mExecutableDir + "/" + QT_PLUGINS_SUBDIR )
    , mQgisPluginsDir( mExecutableDir + "/" + QGIS_PLUGINS_SUBDIR )
    , mGlobeDir( mExecutableDir + "/" + DefaultGlobeSubDir() )

    , m_execYFXName( mExecutableDir + "/" + BRATCREATEYFX_EXE)
    , m_execZFXYName( mExecutableDir + "/" + BRATCREATEZFXY_EXE )
    , m_execExportAsciiName( mExecutableDir + "/" + BRATEXPORTASCII_EXE )
    , m_execExportGeoTiffName( mExecutableDir + "/" + BRATEXPORTGEOTIFF_EXE )
    , m_execShowStatsName( mExecutableDir + "/" + BRATSHOWSTATS_EXE )

#if defined(__APPLE__)
    , m_execBratSchedulerName( mExecutableDir + "/../../../scheduler.app/Contents/MacOS/" + BRATSCHEDULER_EXE )
#else
    , m_execBratSchedulerName( mExecutableDir + "/" + BRATSCHEDULER_EXE )
#endif

{
    if ( IsValid() || !ValidatePaths() )
    {
        return;
    }

    // Set Qt plug-ins path
	//
	//	- Use QCoreApplication::libraryPaths(); to inspect Qt library (plug-ins) directories

        auto list = QCoreApplication::libraryPaths();
        for ( auto const&d : list)
            qDebug() << d;

        //qDebug() << QLibraryInfo::location(QLibraryInfo::PluginsPath);
        //QCoreApplication::addLibraryPath( mQtPluginsDir.c_str() );
    QCoreApplication::setLibraryPaths( QStringList() << mQtPluginsDir.c_str() );

    // Set OSG plug-ins path
	//
    osgDB::FilePathList &osg_paths = osgDB::Registry::instance()->getLibraryFilePathList();
    osg_paths.push_front( mOsgPluginsDir );

        for ( auto const &s : osg_paths )
            qDebug() << t2q( s );


    // II. user (RE)DEFINABLE paths

    SetUserPaths();
}


bool CApplicationPaths::operator == ( const CApplicationPaths &o ) const
{
	//from base class

    assert__( mPlatform == o.mPlatform );
    assert__( mConfiguration == o.mConfiguration );

    assert__( mExecutablePath == o.mExecutablePath );
    assert__( mExecutableDir == o.mExecutableDir );
    assert__( mDeploymentRootDir == o.mDeploymentRootDir );

    assert__( mInternalDataDir == o.mInternalDataDir );

	// from this

    assert__( mVectorLayerPath == o.mVectorLayerPath );

    assert__( mOsgPluginsDir == o.mOsgPluginsDir );
    assert__( mQtPluginsDir == o.mQtPluginsDir );
    assert__( mQgisPluginsDir == o.mQgisPluginsDir );
    assert__( mGlobeDir == o.mGlobeDir );

	//TODO acrescentar asserts para as variaveis novas adicionadas.

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

	//TODO acrescentar as novas variaveis.

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


// TODO Change path title (ValidPath parameter #4) to user friendly name, e.g., mVectorLayerPath to "Vector layer path"

bool CApplicationPaths::ValidatePaths() const
{
	mValid =
		base_t::ValidatePaths() &&											// tests mInternalDataDir
		ValidPath( mErrorMsg, mVectorLayerPath, false, "mVectorLayerPath" ) &&
		ValidPath( mErrorMsg, mOsgPluginsDir, true, "mOsgPluginsDir" ) &&
		ValidPath( mErrorMsg, mQtPluginsDir, true, "mQtPluginsDir" ) &&
		ValidPath( mErrorMsg, mQgisPluginsDir, true, "mQgisPluginsDir" ) &&
		ValidPath( mErrorMsg, mGlobeDir, true, "mGlobeDir" );

    return mValid;
}



// If "unique" is true, path must exist
//
/*
to read
*/
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


bool CApplicationPaths::SetUserPaths()
{
    /*
     *brat: (deployment_root_dir)
     *  +bin
     *  +data
     *  +...
     */
    if ( !IsDir( mUserBasePath ) )
        mUserBasePath = ComputeDefaultUserBasePath( mDeploymentRootDir );

    if ( mUniqueUserBasePath ) //os dados estao em default na data
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


// mRasterLayerPath setter
//	- assumes mExternalDataDir correctly assigned
//	- copies around a distributed raster layer file if the user changes
//		the external data directory location
//
bool CApplicationPaths::CopyRasterLayerFile()
{
    const std::string raster_path = mExternalDataDir + "/" + RasterLayerSubPath();

    if ( raster_path == mRasterLayerPath )
        return true;

    if ( IsFile( raster_path ) || ( IsFile( mRasterLayerPath ) && DuplicateFile( mRasterLayerPath, raster_path ) ) )
        mRasterLayerPath = raster_path;
    else
        return false;	//deserves at most a warning; so far this file exists only to test raster layers

    return true;
}
