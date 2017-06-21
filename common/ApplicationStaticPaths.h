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

#include "common/+Utils.h"
#include "common/tools/brathl_error.h"




//	Ensure common names across applications

#define ORGANIZATION_NAME "ESA"
#define BRAT_APPLICATION_NAME "BRAT"
#define SCHEDULER_APPLICATION_NAME "SCHEDULER"		//IMPORTANT: this name must match BRATSCHEDULER_EXE in brat ApplicationPaths.cpp
#define RADS_SERVICE_NAME "RadsService"




///////////////////////////////////////////////////////////////////////////
//							Standard Paths
///////////////////////////////////////////////////////////////////////////

struct CBratPath
{
	const std::string mPath;
	const bool mMustExist;
	const std::string mName;

	CBratPath( const std::string &path, bool must_exist, const char *name );

	CBratPath( const CBratPath &o )
		: mPath( o.mPath )
		, mMustExist( o.mMustExist )
		, mName( o.mName )
	{}

	virtual ~CBratPath()
	{}


	operator const std::string& () const;

	operator std::string () const;

	
	bool operator == ( const CBratPath &o ) const;


	std::string ToString() const;

	
	bool Valid( std::string &error_msg ) const;

protected:

	virtual bool Validate() const = 0;
};


struct CFolderPath : public CBratPath
{
    DECLARE_BASE_TYPE( CBratPath )

	CFolderPath( const std::string &path, bool must_exist, const char *name )
		: base_t( path, must_exist, name )
	{}

	CFolderPath( const CBratPath &o )
		: base_t( o )
	{}

	virtual ~CFolderPath()
	{}

	
	std::string operator + ( const std::string &str ) const;


protected:

	virtual bool Validate() const override;
};


struct CFilePath : public CBratPath
{
    DECLARE_BASE_TYPE( CBratPath )

	CFilePath( const std::string &path, bool must_exist, const char *name )
		: base_t( path, must_exist, name )
	{}

	CFilePath( const CBratPath &o )
		: base_t( o )
	{}

	virtual ~CFilePath()
	{}

protected:

	virtual bool Validate() const override;
};






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

    const CFilePath mExecutablePath;			//origin: runtime binary
    const std::string mApplicationName;         //origin: caller
    const CFolderPath mExecutableDir;			//origin: runtime binary
    const CFolderPath mDeploymentRootDir;		//origin: runtime binary; executable's parent directory

    const CFolderPath mQtPluginsDir;			//origin: Qt deployable (libraries)
    const CFolderPath mPythonDir;				//origin: runtime binary

    const CFilePath mUserManualPath;			//origin: runtime binary

    // mInternalDataDir: allowed to change for
    //	compatibility reasons only. Modifiable
    //	exclusively through environment variable.
    //	Requires restart. 
    //
    const CFolderPath mInternalDataDir;		//origin: version control, deployed to bin as internal resources

	const CFilePath mRsyncExecutablePath;
	const CFilePath mRadsConfigurationFilePath;
	const CFilePath mRadsServicePanicLogFilePath;


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

    std::string ErrorMsg() const { return mErrorMsg; }

	bool IsValid() const { return mValid; }


	std::string DefaultUserDocumentsPath() const;


	std::string DefaultUserSettingsPath4Application( const std::string&application_name, bool create = true ) const;


	std::string DefaultUserSettingsPath( bool create = true ) const
	{
		return DefaultUserSettingsPath4Application( mApplicationName, create );
	}


	////////////////////////////////////////////
    //	remaining member functions
    ////////////////////////////////////////////

	virtual std::string ToString() const;

protected:

    virtual bool ValidatePaths() const;
};






#endif	//COMMON_APPLICATION_STATIC_PATHS_H
