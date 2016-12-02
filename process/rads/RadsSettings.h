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
#if !defined RADS_SETTINGS_H
#define RADS_SETTINGS_H

#include "common/ApplicationStaticPaths.h"
#include "new-gui/Common/FileSettings.h"
#include "RadsSharedParameters.h"



class CRadsSettingsBase : public CFileSettings
{
    //////////////////////////////////////
    //	types & friends
    //////////////////////////////////////

    using base_t = CFileSettings;


    //////////////////////////////////////
    //	static members
    //////////////////////////////////////

	static unsigned smNumberOfDays;

protected:

    //////////////////////////////////////
    //	data
    //////////////////////////////////////

	const CApplicationStaticPaths &mRadsPaths;

	// These 2 parameters must be explicitly specified by BRAT 
	// for the service to operate
	//
	std::vector< std::string > mMissionNames;
	std::string mOutputDirectory;

	int mNumberOfDays = smNumberOfDays;		//signed: we must be able to do arithmetic with negative mNumberOfDays

	std::vector< CRadsMission > mAllAvailableMissions;

public:
	// This variable records the status of reading brads configuration file (missions, not to be confused with service settings file)
	// It should be inspected before using the class instances
	//
	const bool mValidRadsMissions = false;


    //////////////////////////////////////
    //	construction / destruction
    //////////////////////////////////////

	bool ReadAvailableRadsMissions();

protected:

	// for rads application settings derived class

	CRadsSettingsBase( const CApplicationStaticPaths &brat_paths, const std::string &org_name, const std::string &exec_name ) 
		: base_t( org_name, exec_name, "" )
		, mRadsPaths( brat_paths )
		, mValidRadsMissions( ReadAvailableRadsMissions() )
	{}

	// for rads clients shared settings access derived class

	CRadsSettingsBase( const CApplicationStaticPaths &brat_paths, const std::string &path ) 
		: base_t( path )
		, mRadsPaths( brat_paths )
		, mValidRadsMissions( ReadAvailableRadsMissions() )
	{}

public:

    virtual ~CRadsSettingsBase()
    {}

    //////////////////////////////////////
    //	access
    //////////////////////////////////////

	const CApplicationStaticPaths& RadsPaths() const { return mRadsPaths; }

	const std::vector< std::string >& MissionNames() const { return mMissionNames; }

	const std::string& OutputDirectory() const { return mOutputDirectory; }

	int NumberOfDays() const { return mNumberOfDays; }


public:

    //////////////////////////////////////
    //	operations
    //////////////////////////////////////

	virtual bool LoadConfig();

	virtual bool SaveConfig();
};





//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//				RadsService class application settings
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////


class CRadsSettings : public CRadsSettingsBase
{
	//////////////////////////////////////
	//	types & friends
	//////////////////////////////////////

	using base_t = CRadsSettingsBase;


	//////////////////////////////////////
	//	static members
	//////////////////////////////////////


protected:

	//////////////////////////////////////
	//	data
	//////////////////////////////////////

	bool mLockedFile = false;

public:

	// This parameter is for daemon private use
	//
	QDateTime mLastSync;


	//////////////////////////////////////
	//	construction / destruction
	//////////////////////////////////////

public:

	CRadsSettings( const CApplicationStaticPaths &brat_paths );

	virtual ~CRadsSettings()
	{}

	//////////////////////////////////////
	//	access
	//////////////////////////////////////


	bool HasFileLocked() const { return mLockedFile; }

	bool LockFile( bool lock );


public:

	//////////////////////////////////////
	//	operations
	//////////////////////////////////////

	virtual bool LoadConfig() override;

	virtual bool SaveConfig() override;
};







//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//			Foreign access to RadsService settings by external application
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////


struct CSharedRadsSettings : public CRadsSettingsBase
{
	using base_t = CRadsSettingsBase;

	static std::string RadsSettingsFilePath()
	{
		QSettings settings( QSettings::IniFormat, QSettings::UserScope, ORGANIZATION_NAME, RADS_SERVICE_NAME );
		return q2a( settings.fileName() );
	}

public:
	CSharedRadsSettings( const CApplicationStaticPaths &brat_paths )
		: base_t( brat_paths, RadsSettingsFilePath() )
	{}
	virtual ~CSharedRadsSettings()
	{}


	//////////////////////////////////////
	//	access
	//////////////////////////////////////


	const std::vector< CRadsMission >& AllAvailableMissions() const { return mAllAvailableMissions; }


	// Calls to this function should be enveloped in lock/unlock command calls to the rads service
	//
	bool SetApplicationParameters( const std::string &missions_str, int ndays, const std::string &output_dir );
};





#endif	//RADS_SETTINGS_H
