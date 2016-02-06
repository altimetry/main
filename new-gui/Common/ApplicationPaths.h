#if !defined COMMON_APPLICATION_PATHS_H
#define COMMON_APPLICATION_PATHS_H


#include <QString>

#include "ConsoleApplicationPaths.h"



///////////////////////////////////////////////////////////////////////////
//							Standard Paths
///////////////////////////////////////////////////////////////////////////



class CApplicationPaths : public CConsoleApplicationPaths
{
    ////////////////////////////////////////////
    //	types & friends
    ////////////////////////////////////////////

	using base_t = CConsoleApplicationPaths;


    ////////////////////////////////////////////
    //	static members
    ////////////////////////////////////////////

public:

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


public:

    ////////////////////////////////////////////
    //	instance data
    ////////////////////////////////////////////

    // RIGID (not user definable), definitions
    //	based on executable properties, runtime
    //	and static. So, origin and deployment as
    //	executable products.

    const std::string mVectorLayerPath;			//origin: version control, lives in mInternalDataDir

    const std::string mOsgPluginsDir;			//origin: OSG deployable (libraries)

    const std::string mQtPluginsDir;			//origin: Qt deployable (libraries)
    const std::string mQgisPluginsDir;			//origin: QGIS deployable (libraries)
    const std::string mGlobeDir;				//origin: QGIS deployable (like libraries)

	//TODO: change these names according to our convention, for instance: m_execYFXName -> mExecYFXName. The same in COperation

    const std::string mExecYFXName;
    const std::string mExecZFXYName;
    const std::string mExecExportAsciiName;
    const std::string mExecExportGeoTiffName;
    const std::string mExecShowStatsName;
    const std::string mExecBratSchedulerName;

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

    const std::string& UserBasePath() const { return mUserBasePath; }

    const std::string& ExternalDataDir() const { return mExternalDataDir; }

    const std::string& RasterLayerPath() const { return mRasterLayerPath; }

    const std::string& WorkspacesPath() const { return mWorkspacesDir; }

    bool UniqueUserBasePath() const { return mUniqueUserBasePath; }


    ////////////////////////////////////////////
    //	operations
    ////////////////////////////////////////////


    virtual bool ValidatePaths() const override;

    bool SetUserPaths();

    std::string ToString() const;

public:

    // Operates only over user changeable paths, trying to
    //	ensure their existence in the expected locations
    //



    // if "unique" is true, path must exist
    //

    bool SetUserBasePath( bool unique, const std::string &path = "" );

    // When mUniqueUserBasePath is true, SetUniqueUserBasePath
    //	is the only setter accepted. Otherwise, these 2 setters
    //	can be used to make individual changes to paths.

    bool SetExternalDataDirectory( const std::string &path );

    bool SetWorkspacesDirectory( const std::string &path );


protected:

	bool CopyRasterLayerFile();
};







#endif	//COMMON_APPLICATION_PATHS_H
