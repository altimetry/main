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
#if !defined BRAT_APPLICATION_PATHS_H
#define BRAT_APPLICATION_PATHS_H


#include <QString>

#include "new-gui/Common/QtUtilsIO.h"
#include "new-gui/Common/GUI/ApplicationUserPaths.h"



///////////////////////////////////////////////////////////////////////////
//							Standard Paths
///////////////////////////////////////////////////////////////////////////



class CApplicationPaths : public CApplicationUserPaths
{
    ////////////////////////////////////////////
    //	types & friends
    ////////////////////////////////////////////

	using base_t = CApplicationUserPaths;

	friend class CBratSettings;

    ////////////////////////////////////////////
    //	static members
    ////////////////////////////////////////////

public:

	static const std::string smDefaultURLRasterLayerPath;


    static std::string DefaultGlobeSubDir()
    {
        static const std::string s = "globe/gui";
        return s;
    }


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
    const std::string mQgisPluginsDir;			//origin: QGIS deployable (libraries)
    const std::string mGlobeDir;				//origin: QGIS deployable (like libraries)

	//TODO: change these names according to our convention, for instance: m_execYFXName -> mExecYFXName. The same in COperation

    const std::string mExecYFXName;
    const std::string mExecZFXYName;
    const std::string mExecExportAsciiName;
    const std::string mExecExportGeoTiffName;
    const std::string mExecShowStatsName;
    const std::string mExecBratSchedulerName;

#if (BRAT_MINOR_VERSION_INT==1)
	const std::string mRadsServicePath;
#endif


	// NOT rigid (user definable), so, definitions
    //	are based on defaults.

private:
	bool mUsePortablePaths = true;			// re-assigned in ctor

protected:

	std::string mUserDocumentsDirectory;

	std::string mUserDataDirectory;

    std::string mWorkspacesDirectory;	


protected:
	std::string mRasterLayerPath;
    std::string mURLRasterLayerPath;


    ////////////////////////////////////////////
    //	construction / destruction
    ////////////////////////////////////////////

private:
	bool SetUserPaths();

public:
    explicit CApplicationPaths( const QString &exec_path, const QString &app_name );


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


    bool UsePortablePaths() const { return mUsePortablePaths; }

    const std::string& UserDataDirectory() const { return mUserDataDirectory; }

    bool SetUserDataDirectory( bool portable, const std::string &path );


    const std::string& WorkspacesDirectory() const { return mWorkspacesDirectory; }

    bool SetWorkspacesDirectory( const std::string &path );



    const std::string& RasterLayerPath() const { return mRasterLayerPath; }

    const std::string& URLRasterLayerPath() const { return mURLRasterLayerPath; }

    void SetURLRasterLayerPath( const std::string &path ) { mURLRasterLayerPath = path; }


    const std::string& FullPythonExecutablePath() const
    {
        static const std::string s = mPythonDir + "/" + smPythonExecutableName;
        return s;
    }

    //

	std::string Absolute2PortableDataPath( const std::string &path ) const
	{
		if ( !IsRelative( path ) && UsePortablePaths() )
			return ::Absolute2PortableDataPath( path, mUserDataDirectory );

		return path;
	}


	std::string Portable2AbsoluteDataPath( const std::string &path ) const
	{
		// Portable2AbsoluteDataPath does not change an already absolute path,
		//	but a portable path always needs to be converted to an absolute 
		//	path even if mUsePortablePaths is false
		//
		return ::Portable2AbsoluteDataPath( path, mUserDataDirectory );
	}



    ////////////////////////////////////////////
    //	operations
    ////////////////////////////////////////////


    virtual std::string ToString() const override;

    virtual bool ValidatePaths() const override;
};







#endif	//BRAT_APPLICATION_PATHS_H
