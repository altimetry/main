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


#include <osgDB/Registry>

#include "new-gui/Common/+UtilsIO.h"
#include "new-gui/Common/QtUtils.h"
#include "ApplicationPaths.h"




////////////////////////////////////////////////////////////////////////////////////////////
//
//									CApplicationPaths
//
////////////////////////////////////////////////////////////////////////////////////////////


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
const std::string BRATSCHEDULER_EXE		= setExecExtension( SCHEDULER_APPLICATION_NAME );		//IMPORTANT: this name must match app_name in scheduler main.cpp
const std::string BRATEXPORTGEOTIFF_EXE = setExecExtension( "BratExportGeoTiff" );
const std::string BRATEXPORTASCII_EXE	= setExecExtension( "BratExportAscii" );
const std::string BRATCREATEZFXY_EXE	= setExecExtension( "BratCreateZFXY" );
const std::string BRATCREATEYFX_EXE		= setExecExtension( "BratCreateYFX" );


//static 
const std::string CApplicationPaths::smDefaultURLRasterLayerPath =
		R"raw(&format=image/png&layers=bluemarble&styles=&url=http://disc1.sci.gsfc.nasa.gov/daac-bin/wms_airsnrt?layer%3DAIRS_SO2_A%26)raw";
            //contextualWMSLegend=0&crs=EPSG:4326&dpiMode=all&featureCount=10&format=image/png&layers=bluemarble&styles=&url=http://disc1.sci.gsfc.nasa.gov/daac-bin/wms_airsnrt?layer%3DAIRS_SO2_A%26




CApplicationPaths::CApplicationPaths( const QString &exec_path, const QString &app_name ) :

    // I. NOT user (re)definable paths

	base_t( q2a( exec_path ), q2a( app_name ) )

    , mVectorLayerPath( mInternalDataDir + "/maps/ne_10m_coastline/ne_10m_coastline.shp" )
    , mRasterLayerPath( mInternalDataDir + "/maps/raster-image/world_googlemaps.tif" )
    , mURLRasterLayerPath( smDefaultURLRasterLayerPath )

    //"http://localhost:8080/geoserver/wfs?srsname=EPSG:23030&typename=union&version=1.0.0&request=GetFeature&service=WFS"
    //"url=http://kaart.maaamet.ee/wms/alus&format=image/png&layers=MA-ALUS&styles=&crs=EPSG:AUTO"

    , mOsgPluginsDir( mExecutableDir + "/" + osg_paths_SUBDIR )

    , mQgisPluginsDir( mExecutableDir + "/" + QGIS_PLUGINS_SUBDIR )
    , mGlobeDir( mExecutableDir + "/" + DefaultGlobeSubDir() )

    , mExecYFXName(				EscapePath( mExecutableDir + "/" + BRATCREATEYFX_EXE) )
    , mExecZFXYName(			EscapePath( mExecutableDir + "/" + BRATCREATEZFXY_EXE ) )
    , mExecExportAsciiName(		EscapePath( mExecutableDir + "/" + BRATEXPORTASCII_EXE ) )
    , mExecExportGeoTiffName(	EscapePath( mExecutableDir + "/" + BRATEXPORTGEOTIFF_EXE ) )
    , mExecShowStatsName(		EscapePath( mExecutableDir + "/" + BRATSHOWSTATS_EXE ) )

#if defined(__APPLE__)
    , mExecBratSchedulerName(	EscapePath( mExecutableDir + "/../../../scheduler.app/Contents/MacOS/" + BRATSCHEDULER_EXE ) )
#else
    , mExecBratSchedulerName(	EscapePath( mExecutableDir + "/" + BRATSCHEDULER_EXE ) )
#endif

	, mUserDocumentsDirectory( DefaultUserDocumentsPath() )
{
    // user (RE)DEFINABLE paths
	//
    SetUserPaths();

    if ( !IsValid() || !ValidatePaths() )
    {
        return;
    }

    // Set OSG plug-ins path
	//
    osgDB::FilePathList &osg_paths = osgDB::Registry::instance()->getLibraryFilePathList();
    osg_paths.push_front( mOsgPluginsDir );


#if defined (_DEBUG) || defined (DEBUG)
	for ( auto const &s : osg_paths )
        qDebug() << t2q( s );
#endif
}


bool CApplicationPaths::operator == ( const CApplicationPaths &o ) const
{
	//from base class

    assert__( mPlatform == o.mPlatform );
    assert__( mConfiguration == o.mConfiguration );

    assert__( mExecutablePath == o.mExecutablePath );
    assert__( mExecutableDir == o.mExecutableDir );
    assert__( mDeploymentRootDir == o.mDeploymentRootDir );

    assert__( mQtPluginsDir == o.mQtPluginsDir );
    assert__( mPythonDir == o.mPythonDir );
	assert__( mUserManualPath == o.mUserManualPath );
    assert__( mInternalDataDir == o.mInternalDataDir );

	// from this

	assert__( mUserDocumentsDirectory == o.mUserDocumentsDirectory );
	assert__( mUserDataDirectory == o.mUserDataDirectory );
	assert__( mWorkspacesDirectory == o.mWorkspacesDirectory );

    assert__( mVectorLayerPath == o.mVectorLayerPath );

    assert__( mOsgPluginsDir == o.mOsgPluginsDir );
    assert__( mQgisPluginsDir == o.mQgisPluginsDir );
    assert__( mGlobeDir == o.mGlobeDir );

    assert__( mExecYFXName == o.mExecYFXName );
    assert__( mExecZFXYName == o.mExecZFXYName );
    assert__( mExecExportAsciiName == o.mExecExportAsciiName );
    assert__( mExecExportGeoTiffName == o.mExecExportGeoTiffName );
    assert__( mExecShowStatsName == o.mExecShowStatsName );
    assert__( mExecBratSchedulerName == o.mExecBratSchedulerName );

    return
        // user re-definable

        mRasterLayerPath == o.mRasterLayerPath &&
		mURLRasterLayerPath == o.mRasterLayerPath;
        ;
}

//virtual
std::string CApplicationPaths::ToString() const
{
	std::string s = base_t::ToString();

    s += ( std::string( "\nUse Portable Paths == " ) + ( UsePortablePaths() ? "true" : "false" ) );

	s += ( "\nUser Documents Directory == " + mUserDocumentsDirectory );
    s += ( "\nmUser Data Directory == " + mUserDataDirectory );
    s += ( "\nWorkspaces Directory == " + mWorkspacesDirectory );

    s += ( "\nVectorLayer Path == " + mVectorLayerPath );

    s += ( "\nOsgPlugins Dir == " + mOsgPluginsDir );
    s += ( "\nQgisPlugins Dir == " + mQgisPluginsDir );
    s += ( "\nGlobe Dir == " + mGlobeDir );

    s += ("\nmExecYFXName == " + mExecYFXName);
    s += ("\nmExecZFXYName == " + mExecZFXYName);
    s += ("\nmExecExportAsciiName == " + mExecExportAsciiName);
    s += ("\nmExecExportGeoTiffName == " + mExecExportGeoTiffName);
    s += ("\nmExecShowStatsName == " + mExecShowStatsName);
    s += ("\nmExecBratSchedulerName == " + mExecBratSchedulerName);

    s += ( "\nRasterLayer Path == " + mRasterLayerPath );
    s += ( "\nmURLRasterLayer Path == " + mURLRasterLayerPath );

    return s;
}


bool CApplicationPaths::ValidatePaths() const
{
	mValid =
		base_t::ValidatePaths() &&											// tests mInternalDataDir
        ValidPath( mErrorMsg, mVectorLayerPath, true, "Vector Layer path" ) &&
        ValidPath( mErrorMsg, mRasterLayerPath, true, "Raster Layer path" ) &&
        ValidPath( mErrorMsg, mOsgPluginsDir, false, "OSG Plugins diretory" ) &&
        ValidPath( mErrorMsg, mQgisPluginsDir, false, "Qgis Plugins directory" ) &&
        ValidPath( mErrorMsg, mGlobeDir, false, "Globe files directory" );

    return mValid;
}


bool CApplicationPaths::SetUserDataDirectory( bool portable, const std::string &path )
{
    if ( !IsDir( path ) && !MakeDirectory( path ) )
        return false;

    mUsePortablePaths = portable;

    mUserDataDirectory = path;

    return true;
}


bool CApplicationPaths::SetWorkspacesDirectory( const std::string &path )
{
    if ( !IsDir( path ) && !MakeDirectory( path ) )
        return false;

    mWorkspacesDirectory = path;

    return true;
}


bool CApplicationPaths::SetUserPaths()
{
	assert__( !mUserDocumentsDirectory.empty() );

    const std::string wkspaces = mUserDocumentsDirectory + "/workspaces";
    const std::string data = mUserDocumentsDirectory + "/user-data";

    if ( !IsDir( mUserDocumentsDirectory ) && !MakeDirectory( mUserDocumentsDirectory ) )
        return false;

	//set defaults taking "mUserDocumentsDirectory" as root if existing values don't exist

    return 
		( IsDir( mUserDataDirectory ) || SetUserDataDirectory( mUsePortablePaths, data ) ) &&
		( IsDir( mWorkspacesDirectory ) || SetWorkspacesDirectory( wkspaces ) );
}
