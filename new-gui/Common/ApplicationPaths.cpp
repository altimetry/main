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
	std::string s;
    auto s3root = getenv( "S3ALTB_ROOT" );	//assuming development environment available
    if ( s3root )
    {
        s = s3root + std::string( "/project" );
        if ( !IsDir( s ) )
            s = DeploymentRootDir;
    }

    assert__( IsDir( s ) );

    return s + "/" + DefaultExternalDataSubDir();
}



CApplicationPaths::CApplicationPaths( const QString &exec_path ) :

    // I. NOT user (re)definable paths

	base_t( q2a( exec_path ) )

    , mVectorLayerPath( mInternalDataDir + "/maps/ne_10m_coastline/ne_10m_coastline.shp" )
    , mRasterLayerPath( mExecutableDir + "/" + RasterLayerSubPath() )

    , mOsgPluginsDir( mExecutableDir + "/" + osg_paths_SUBDIR )

    , mQtPluginsDir( mExecutableDir + "/" + QT_PLUGINS_SUBDIR )
    , mQgisPluginsDir( mExecutableDir + "/" + QGIS_PLUGINS_SUBDIR )
    , mGlobeDir( mExecutableDir + "/" + DefaultGlobeSubDir() )

    , mExecYFXName( mExecutableDir + "/" + BRATCREATEYFX_EXE)
    , mExecZFXYName( mExecutableDir + "/" + BRATCREATEZFXY_EXE )
    , mExecExportAsciiName( mExecutableDir + "/" + BRATEXPORTASCII_EXE )
    , mExecExportGeoTiffName( mExecutableDir + "/" + BRATEXPORTGEOTIFF_EXE )
    , mExecShowStatsName( mExecutableDir + "/" + BRATSHOWSTATS_EXE )

#if defined(__APPLE__)
    , mExecBratSchedulerName( mExecutableDir + "/../../../scheduler.app/Contents/MacOS/" + BRATSCHEDULER_EXE )
#else
    , mExecBratSchedulerName( mExecutableDir + "/" + BRATSCHEDULER_EXE )
#endif

{
    //fixed (return if parent class dirs are not valid or subclass path are not valid as well)
    if ( !IsValid() || !ValidatePaths() )
    {
        return;
    }

    // Set Qt plug-ins path
	//
	//	- Use QCoreApplication::libraryPaths(); to inspect Qt library (plug-ins) directories

    QCoreApplication::setLibraryPaths( QStringList() << mQtPluginsDir.c_str() );

    // Set OSG plug-ins path
	//
    osgDB::FilePathList &osg_paths = osgDB::Registry::instance()->getLibraryFilePathList();
    osg_paths.push_front( mOsgPluginsDir );


#if defined (_DEBUG) || defined (DEBUG)
	for ( auto const &s : osg_paths )
        qDebug() << t2q( s );
#endif


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

    assert__(mExecYFXName == o.mExecYFXName);
    assert__(mExecZFXYName == o.mExecZFXYName);
    assert__(mExecExportAsciiName == o.mExecExportAsciiName);
    assert__(mExecExportGeoTiffName == o.mExecExportGeoTiffName);
    assert__(mExecShowStatsName == o.mExecShowStatsName);
    assert__(mExecBratSchedulerName == o.mExecBratSchedulerName);

    return
        // user re-definable

        mPortableBasePath == o.mPortableBasePath &&
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

    s += ("\nmExecYFXName == " + mExecYFXName);
    s += ("\nmExecZFXYName == " + mExecZFXYName);
    s += ("\nmExecExportAsciiName == " + mExecExportAsciiName);
    s += ("\nmExecExportGeoTiffName == " + mExecExportGeoTiffName);
    s += ("\nmExecShowStatsName == " + mExecShowStatsName);
    s += ("\nmExecBratSchedulerName == " + mExecBratSchedulerName);

    s += ( "\nInternalData Dir == " + mInternalDataDir );
    s += ( "\nVectorLayer Path == " + mVectorLayerPath );

    s += ( "\nOsgPlugins Dir == " + mOsgPluginsDir );
    s += ( "\nQtPlugins Dir == " + mQtPluginsDir );
    s += ( "\nQgisPlugins Dir == " + mQgisPluginsDir );
    s += ( "\nGlobe Dir == " + mGlobeDir );

    s += ( "\nUserBase Path == " + mPortableBasePath );
    s += ( "\nRasterLayer Path == " + mRasterLayerPath );

    s += ( "\nWorkspaces Dir == " + mWorkspacesDir );

    s += ( std::string( "\nUnique UserBasePath == " ) + ( mUsePortablePaths ? "true" : "false" ) );

    return s;
}


bool CApplicationPaths::ValidatePaths() const
{
	mValid =
		base_t::ValidatePaths() &&											// tests mInternalDataDir
        ValidPath( mErrorMsg, mVectorLayerPath, true, "Vector Layer path" ) &&
        ValidPath( mErrorMsg, mRasterLayerPath, true, "Raster Layer path" ) &&
        ValidPath( mErrorMsg, mOsgPluginsDir, false, "OSG Plugins diretory" ) &&
        ValidPath( mErrorMsg, mQtPluginsDir, false, "Qt Plugins directory" ) &&
        ValidPath( mErrorMsg, mQgisPluginsDir, false, "Qgis Plugins directory" ) &&
        ValidPath( mErrorMsg, mGlobeDir, false, "Globe files directory" );

    return mValid;
}


bool CApplicationPaths::SetUserBasePath( bool portable, const std::string &path )	//path = ""
{
    if ( !MakeDirectory( path ) )
        return false;

    mPortableBasePath = path;

    mUsePortablePaths = portable;

    return true;
}


bool CApplicationPaths::SetWorkspacesDirectory( const std::string &path )
{
    if ( !IsDir( path ) && !MakeDirectory( path ) )
        return false;

    mWorkspacesDir = path;
    return true;
}


bool CApplicationPaths::SetUserPaths()
{
    if ( !IsDir( mPortableBasePath ) )
        mPortableBasePath = ComputeDefaultUserBasePath( mDeploymentRootDir );

    std::string wkspaces = GetDirectoryFromPath( mPortableBasePath ) + "/" + DefaultProjectsSubDir();

    if ( !IsDir( mPortableBasePath ) && !SetUserBasePath( mUsePortablePaths, mPortableBasePath ) )
        return false;

    return IsDir( mWorkspacesDir ) || SetWorkspacesDirectory( wkspaces );
}
