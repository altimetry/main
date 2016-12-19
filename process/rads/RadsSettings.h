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


public:

	static const unsigned smDefaultNumberOfDays;
	static const unsigned smMinNumberOfDays;
	static const unsigned smMaxNumberOfDays;


	inline static const char* RsyncDateFormat()
	{
		const char *date_s = "yyyy-MM-dd";		//const char *date_time_s = "yyyy-MM-dd hh:mm:ss.zzz";

		return date_s;
	}

	
	static std::string FormatRadsLocalOutputFolder( const std::string &user_data_dir );


	static std::string FormatRadsLogFilePath( const std::string &user_data_dir );


protected:

    //////////////////////////////////////
    //	instance data
    //////////////////////////////////////

	const CApplicationStaticPaths &mRadsPaths;

	// These 2 parameters must be explicitly specified by BRAT 
	// for the service to operate
	//
	std::vector< std::string > mMissionNames;
	std::string mUserDataDir;							//data folder and log path dynamically formed based on this

	int mNumberOfDays = smDefaultNumberOfDays;			//signed: we must be able to do arithmetic with negative mNumberOfDays

	std::vector< CRadsMission > mAllAvailableMissions;	//initialized by CommonConstruct


	// This parameter is for rads client private use
	//
	QDateTime mLastSync;


public:
	// This variable records the status of reading RADS configuration file (missions, not to be confused with service settings file)
	// It should be inspected before using the class instances
	//
	const bool mValidRadsMissions = false;


    //////////////////////////////////////
    //	construction / destruction
    //////////////////////////////////////

	bool CommonConstruct();

protected:

	// for rads application settings derived class

	CRadsSettingsBase( const CApplicationStaticPaths &brat_paths, const std::string &org_name, const std::string &exec_name );

	// for rads clients shared settings access derived class

	CRadsSettingsBase( const CApplicationStaticPaths &brat_paths, const std::string &path );

public:

    virtual ~CRadsSettingsBase()
    {}

	
	//////////////////////////////////////
	//	Log
	//////////////////////////////////////

	friend std::ostream& operator << ( std::ostream &os, const CRadsSettingsBase &o );


	//////////////////////////////////////
    //	access
    //////////////////////////////////////

	const CApplicationStaticPaths& RadsPaths() const { return mRadsPaths; }

	const std::vector< std::string >& MissionNames() const { return mMissionNames; }

	std::string OutputDirectory() const { return FormatRadsLocalOutputFolder( mUserDataDir ); }

	std::string LogFilePath() const { return FormatRadsLogFilePath( mUserDataDir ); }


	//... time related

	int NumberOfDays() const { return mNumberOfDays; }

	// Given the instance last synchronization time and an arbitrary period of ndays,
	//	returns the next synchronization date. If last synchronization is so old that
	//	even after a period of ndays it is in the past, returns the current date
	//
	QDate NextSyncDateForPeriodWithDays( int ndays );


protected:

	// Synchronization time computations

	// Function effectively used to trigger a synchronization
	//
	QDate NextSyncDate();

	// Next 2 functions return true if configuration in file needs to be updated

	// Corrects invalid numeric values or (over/out)dated reference times:
	//	(mLastSync + period < today || mLastSync > today).
	// To be used when values could externally be modified (so, in LoadConfig); 
	//	called as runtime precaution in time checks before triggering a synchronization
	//
	bool CorrectLastSyncTime();

	// to be used when a synchronization is requested immediately
	//
	bool SetNextSyncDateToday();

public:

    //////////////////////////////////////
    //	Persistence
    //////////////////////////////////////

	virtual bool LoadConfig();

	virtual bool SaveConfig();
};





//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//					RadsService class application settings
//			(do NOT instantiate or inherit outside the RADS service)
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

	static const unsigned smDefaultPeriodicCheckInMinutes;

protected:

	//////////////////////////////////////
	//	instance data
	//////////////////////////////////////

	bool mLockedFile = false;
	unsigned mPeriodicCheckInMinutes = smDefaultPeriodicCheckInMinutes;

public:


	//////////////////////////////////////
	//	construction / destruction
	//////////////////////////////////////

public:

	CRadsSettings( const CApplicationStaticPaths &brat_paths );

	virtual ~CRadsSettings()
	{}


	//////////////////////////////////////
	//	Log
	//////////////////////////////////////

	friend std::ostream& operator << ( std::ostream &os, const CRadsSettings &o );


	//////////////////////////////////////
	//	access
	//////////////////////////////////////


	bool HasFileLocked() const { return mLockedFile; }

	bool LockFile( bool lock );

	unsigned PeriodicCheckInMinutes() const { return mPeriodicCheckInMinutes; }

public:

	//////////////////////////////////////
	//	operations
	//////////////////////////////////////

	
	using base_t::NextSyncDate;
	using base_t::CorrectLastSyncTime;
	using base_t::SetNextSyncDateToday;


	// Returns true if configuration in file needs to be updated
	// To be used when a synchronization just finished
	//
	bool SetLastSyncTimeNow();


	virtual bool LoadConfig() override;

	virtual bool SaveConfig() override;
};







//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//			Foreign access to RadsService settings by external application
//				(do NOT instantiate or inherit in the RADS service)
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
	bool SetApplicationParameterValues( const std::string &missions_str, int ndays, const std::string &user_data_dir );


	virtual bool SaveConfig() override;
};





#endif	//RADS_SETTINGS_H
