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
#if !defined COMMON_APPLICATION_STATIC_PATHS_H
#define COMMON_APPLICATION_STATIC_PATHS_H

#include "new-gui/Common/+Utils.h"




//	Ensure common name across applications

#define ORGANIZATION_NAME "ESA"
#define SCHEDULER_APPLICATION_NAME "scheduler"
#define RADS_SERVICE_NAME "RadsService"




///////////////////////////////////////////////////////////////////////////
//							Standard Paths
///////////////////////////////////////////////////////////////////////////




class CApplicationStaticPaths : public non_copyable
{
    ////////////////////////////////////////////
    //	types & friends
    ////////////////////////////////////////////

	friend class CBratSettings;


    ////////////////////////////////////////////
    //	static members
    ////////////////////////////////////////////

public:
    static const std::string smPythonExecutableName;


    static std::string DefaultInternalDataSubDir()
    {
        static const std::string s = "data";
        return s;
    }

protected:

    static std::string ComputeInternalDataDirectory( const std::string &ExecutableDir );

	static bool ValidPath( std::string &error_msg, const std::string &path, bool is_file, const std::string path_title );


    ////////////////////////////////////////////
    //	instance data
    ////////////////////////////////////////////

protected:

    mutable bool mValid;    // = false; do not initialize here: code used by pre-c++11 compilers
    mutable std::string mErrorMsg;

public:

    // RIGID (not user definable), definitions
    //	based on executable properties, runtime
    //	and static. So, origin and deployment as
    //	executable products.

    const std::string mPlatform;
    const std::string mConfiguration;

    const std::string mExecutablePath;          //origin: runtime binary
    const std::string mApplicationName;         //origin: caller
    const std::string mExecutableDir;			//origin: runtime binary
    const std::string mDeploymentRootDir;		//origin: runtime binary; executable's parent directory

    const std::string mQtPluginsDir;			//origin: Qt deployable (libraries)
    const std::string mPythonDir;				//origin: runtime binary

    const std::string mUserManualPath;			//origin: runtime binary

    // mInternalDataDir: allowed to change for
    //	compatibility reasons only. Modifiable
    //	exclusively through environment variable.
    //	Requires restart. 
    //
    const std::string mInternalDataDir;		//origin: version control, deployed to bin as internal resources

#if (BRAT_MINOR_VERSION_INT==1)
	const std::string mRsyncExecutablePath;
#endif


    ////////////////////////////////////////////
    //	construction / destruction
    ////////////////////////////////////////////

public:
    explicit CApplicationStaticPaths( const std::string &exec_path, const std::string &app_name );


    virtual ~CApplicationStaticPaths()
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

	virtual std::string ToString() const;

protected:

    virtual bool ValidatePaths() const;
};






#endif	//COMMON_APPLICATION_STATIC_PATHS_H
