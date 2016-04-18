#if !defined COMMON_CONSOLE_APPLICATION_PATHS_H
#define COMMON_CONSOLE_APPLICATION_PATHS_H

#include "new-gui/Common/+Utils.h"


///////////////////////////////////////////////////////////////////////////
//							Standard Paths
///////////////////////////////////////////////////////////////////////////


class CConsoleApplicationPaths : public non_copyable
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


protected:

    static std::string ComputeInternalDataDirectory( const std::string &ExecutableDir );


	static bool ValidPath( std::string &error_msg, const std::string &path, bool is_file, const std::string path_title );


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
    const std::string mDeploymentRootDir;		//origin: runtime binary; executable's parent directory

    // mInternalDataDir: allowed to change for
    //	compatibility reasons only. Modifiable
    //	exclusively through environment variable.
    //	Requires restart.
    //
    const std::string mInternalDataDir;			//origin: version control, deployed to bin as internal resources


protected:

    mutable bool mValid = false;
    mutable std::string mErrorMsg;

    ////////////////////////////////////////////
    //	construction / destruction
    ////////////////////////////////////////////

public:
    CConsoleApplicationPaths( const std::string &exec_path );


    virtual ~CConsoleApplicationPaths()
    {}

public:

    ////////////////////////////////////////////
    //	access (getters/setters/testers)
    ////////////////////////////////////////////

    std::string GetErrorMsg() const { return mErrorMsg; }

	bool IsValid() const { return mValid; }


    ////////////////////////////////////////////
    //	remaining member functions
    ////////////////////////////////////////////

    virtual bool ValidatePaths() const;
};






#endif	//COMMON_CONSOLE_APPLICATION_PATHS_H
