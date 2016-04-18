#if !defined BRAT_SETTINGS_H
#define BRAT_SETTINGS_H

#include "new-gui/Common/ApplicationPaths.h"
#include "new-gui/Common/ApplicationSettings.h"
#include "new-gui/Common/QtUtilsIO.h"


class CWorkspace;
class CBratApplication;






class CBratSettings : public CApplicationSettings
{
    //////////////////////////////////////
    //	types & friends
    //////////////////////////////////////

    using base_t = CApplicationSettings;

    friend class CBratApplication;
	friend class QbrtApplication;		//TODO delete when brat-lab is discontinued


    //////////////////////////////////////
    //	static members
    //////////////////////////////////////


protected:

    //////////////////////////////////////
    //	data
    //////////////////////////////////////

    CApplicationPaths &mBratPaths;

public:
    std::string m_lastDataPath;
    std::string m_lastPageReached;
    bool mAdvancedOperations = false;
    std::string m_lastWksPath;
    std::string m_lastColorTable;

	bool mLoadLastWorkspaceAtStartUp = true;
	bool mUseRasterLayer = false;
	bool mDesktopManagerSdi = true;


    //////////////////////////////////////
    //	construction / destruction
    //////////////////////////////////////

protected:

    // Settings of type I - For exclusive use of application-wide settings
    //
    // For Application type initialization; static and non-static read/write functions can access (THE) instance type

    CBratSettings( CApplicationPaths &brat_paths, const std::string &org_name, const std::string &exec_name, const std::string &domain_name = "" ) :
        base_t( org_name, exec_name, domain_name )
      , mBratPaths( brat_paths )
    {}

public:
	// TODO REMOVE this constructor type (from this class only) when compatibility with old gui is not a concern
	//
	// Settings of type II - used by parameter files
	//
	// Module type instance; static read/write functions should not be used with this instance type

    CBratSettings( CApplicationPaths &brat_paths, const std::string &path ) :
		base_t( path )
      , mBratPaths( brat_paths )
	{}

    virtual ~CBratSettings()
    {}

    //////////////////////////////////////
    //	access
    //////////////////////////////////////


    const CApplicationPaths& BratPaths() const { return mBratPaths; }


	bool SetUserBasePath( bool unique, const std::string &path )
	{
		return mBratPaths.SetUserBasePath( unique, path );
	}


	bool SetWorkspacesDirectory( const std::string &path )
	{
		return mBratPaths.SetWorkspacesDirectory( path );
	}


	bool UsingRasterLayer() const { return mUseRasterLayer; }


public:

    //////////////////////////////////////
    //	operations
    //////////////////////////////////////

    virtual bool LoadConfig() override;
    virtual bool SaveConfig() override;

protected:

    bool LoadConfigSelectionCriteria();
    bool SaveConfigSelectionCriteria();

	bool LoadPaths();
	bool SavePaths();
	

public:
    ////////////////////////////////////////////////////
    //	TODO: DELETE
    //		- only for brat v3 support to v4 development
    ////////////////////////////////////////////////////

    template< class CONTAINER >
    bool LoadRecentFilesGeneric( CONTAINER &paths );

    bool LoadRecentFiles( std::vector<std::string> &paths );
    bool LoadRecentFiles( QStringList &paths );

    template< class CONTAINER >
    bool SaveRecentFilesGeneric( const CONTAINER &paths );

    bool SaveRecentFiles( const std::vector<std::string> &paths );
    bool SaveRecentFiles( const QStringList &paths );
};



#endif	//BRAT_SETTINGS_H
