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

	friend class CBratSettings;


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

    const std::string mPlatform;
    const std::string mConfiguration;

    const std::string mExecutablePath;          //origin: runtime binary
    const std::string mExecutableDir;			//origin: runtime binary
    const std::string mDeploymentRootDir;		//origin: runtime binary; executable's parent directory

    const std::string mQtPluginsDir;			//origin: Qt deployable (libraries)

    const std::string mUserManualPath;			//origin: runtime binary

    // mInternalDataDir: allowed to change for
    //	compatibility reasons only. Modifiable
    //	exclusively through environment variable.
    //	Requires restart. 
    //
    const std::string mInternalDataDir;		//origin: version control, deployed to bin as internal resources

	//user directories

private:
	bool mUsePortablePaths = true;			// re-assigned in ctor

protected:

	std::string mUserDocumentsDirectory;

	std::string mUserDataDirectory;

    std::string mWorkspacesDirectory;	


protected:

    mutable bool mValid = false;
    mutable std::string mErrorMsg;

    ////////////////////////////////////////////
    //	construction / destruction
    ////////////////////////////////////////////

private:
	bool SetUserPaths();

public:
    explicit CConsoleApplicationPaths( const std::string &exec_path, const std::string &user_docs_dir );


    virtual ~CConsoleApplicationPaths()
    {}

public:

    ////////////////////////////////////////////
    //	access (getters/setters/testers)
    ////////////////////////////////////////////

    std::string GetErrorMsg() const { return mErrorMsg; }

	bool IsValid() const { return mValid; }


    bool UsePortablePaths() const { return mUsePortablePaths; }

    const std::string& UserDataDirectory() const { return mUserDataDirectory; }

    bool SetUserDataDirectory( bool portable, const std::string &path );


    const std::string& WorkspacesDirectory() const { return mWorkspacesDirectory; }

    bool SetWorkspacesDirectory( const std::string &path );


    ////////////////////////////////////////////
    //	remaining member functions
    ////////////////////////////////////////////

	virtual std::string ToString() const;

protected:

    virtual bool ValidatePaths() const;
};






#endif	//COMMON_CONSOLE_APPLICATION_PATHS_H
