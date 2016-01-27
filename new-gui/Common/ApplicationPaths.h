#if !defined COMMON_APPLICATION_PATHS_H
#define COMMON_APPLICATION_PATHS_H


#include "QtUtilsIO.h"


///////////////////////////////////////////////////////////////////////////
//							Standard Paths
///////////////////////////////////////////////////////////////////////////


class CApplicationPaths : public non_copyable
{
    ////////////////////////////////////////////
    //	types & friends
    ////////////////////////////////////////////



    ////////////////////////////////////////////
    //	static members
    ////////////////////////////////////////////

public:

    static std::string DefaultInternalDataSubDir()
    {
        static const std::string s = "data";
        return s;
    }
    static std::string DefaultExternalDataSubDir()
    {
        static const std::string s = "data";
        return s;
    }
    static std::string DefaultProjectsSubDir()
    {
        static const std::string s = "workspaces";
        return s;
    }
    static std::string DefaultGlobeSubDir()
    {
        static const std::string s = "globe/gui";
        return s;
    }
    static std::string RasterLayerSubPath()
    {
        static const std::string s = "maps/NE1_HR_LC_SR_W_DR/NE1_HR_LC_SR_W_DR.tif";
        return s;
    }



protected:

    static std::string ComputeDefaultUserBasePath( const std::string &DeploymentRootDir);


    static std::string ComputeInternalDataDirectory( const std::string &ExecutableDir );


public:

    ////////////////////////////////////////////
    //	instance data
    ////////////////////////////////////////////

    // RIGID (not user definable), definitions
    //	based on executable properties, runtime
    //	and static. So, origin and deployment as
    //	executable products.

    const std::string mPlatform;				//origin: runtime binary
    const std::string mConfiguration;			//origin: runtime binary

    const std::string mExecutablePath;          //origin: runtime binary
    const std::string mExecutableDir;			//origin: runtime binary
    const std::string mDeploymentRootDir;		//origin: runtime binary

    // mInternalDataDir: allowed to change for
    //	compatibility reasons only. Modifiable
    //	exclusively through environment variable.
    //	Requires restart.
    //
    const std::string mInternalDataDir;			//origin: version control, deployed to bin as internal resources
    const std::string mVectorLayerPath;			//origin: version control, lives in mInternalDataDir

    const std::string mOsgPluginsDir;			//origin: OSG deployable (libraries)

    const std::string mQtPluginsDir;			//origin: Qt deployable (libraries)
    const std::string mQgisPluginsDir;			//origin: QGIS deployable (libraries)
    const std::string mGlobeDir;				//origin: QGIS deployable (like libraries)


    // NOT rigid (user definable), so, definitions
    //	are based on defaults. Origin: lib/data,
    //	deployment like COTS libraries

    std::string mUserBasePath;					// defaults to executable grand-parent directory (deployed) or S3ALTB_ROOT/lib (development)
    std::string mExternalDataDir;				// defaults mUserBasePath/data (i.e., to executable's sub-directory sibling when deployed)
    std::string mRasterLayerPath;				// deployed as external data sample (lives in mExternalDataDir)

    std::string mWorkspacesDir;					// samples? if so, deployed as external data sample (like mRasterLayerPath)

    bool mUniqueUserBasePath = true;			// re-assigned in ctor


    ////////////////////////////////////////////
    //	construction / destruction
    ////////////////////////////////////////////

public:
    CApplicationPaths( const QString &exec_path );


    virtual ~CApplicationPaths()
    {}

public:

    ////////////////////////////////////////////
    //	access (getters/setters/testers)
    ////////////////////////////////////////////

    bool operator == ( const CApplicationPaths &o ) const;


    bool operator != ( const CApplicationPaths &o ) const
    {
        return !( *this == o );
    }

    // User changeable paths are not tested, because
    //	by definition they are not critical
    // TODO refuse to run if this is false, allowing at most
    //	a dialog to open and correct the situation until
    //	this is true; false only if deployment error or user
    //	messing with installation outputs
    //
    bool valid() const;


    const std::string& UserBasePath() const { return mUserBasePath; }

    const std::string& ExternalDataDir() const { return mExternalDataDir; }

    const std::string& RasterLayerPath() const { return mRasterLayerPath; }

    const std::string& WorkspacesPath() const { return mWorkspacesDir; }

    bool UniqueUserBasePath() const { return mUniqueUserBasePath; }


    ////////////////////////////////////////////
    //	operations
    ////////////////////////////////////////////

    std::string ToString() const;

public:

    // Operates only over user changeable paths, trying to
    //	ensure their existence in the expected locations
    //
    bool validate();


    // if "unique" is true, path must exist
    //
    bool SetUniqueUserBasePath( bool unique, const std::string &path = "" );

    // When mUniqueUserBasePath is true, SetUniqueUserBasePath
    //	is the only setter accepted. Otherwise, these 2 setters
    //	can be used to make individual changes to paths.

    bool SetExternalDataDirectory( const std::string &path );

    bool SetWorkspacesDirectory( const std::string &path );


protected:

    // mRasterLayerPath setter
    //	- assumes mExternalDataDir correctly assigned
    //	- copies around a distributed raster layer file if the user changes
    //		the external data directory location
    //
    bool CopyRasterLayerFile()
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
};






#endif	//COMMON_APPLICATION_PATHS_H
