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
#include "stdafx.h"

#include "common/QtUtilsIO.h"
#include "common/tools/Exception.h"
#include "RadsSettings.h"


//entries in rads service ini file (operational)

const std::string ENTRY_RADS_MISSIONS =				"missions";
const std::string ENTRY_RADS_NUMBER_OF_DAYS =		"days";
const std::string ENTRY_USER_DATA_DIR =				"user_data_dir";
const std::string ENTRY_LAT_SYNC_TIME =				"last_sync_time";
const std::string ENTRY_PERIODIC_CHECK_IN_MINUTES =	"periodic_check_in_minutes";



////////////////////////////////////////////////////////////////////////////////////////////
//
//									CRadsSettingsBase
//
////////////////////////////////////////////////////////////////////////////////////////////


//static 
const unsigned CRadsSettingsBase::smDefaultNumberOfDays = 3;
//static 
const unsigned CRadsSettingsBase::smMinNumberOfDays = 1;
//static 
const unsigned CRadsSettingsBase::smMaxNumberOfDays = 31;


//static 
std::string CRadsSettingsBase::FormatRadsLocalOutputFolder( const std::string &user_data_dir )
{
	return NormalizedPath( user_data_dir + "/rads");
}


//static 
std::string CRadsSettingsBase::FormatRadsLogFilePath( const std::string &user_data_dir )
{
	return NormalizedPath( FormatRadsLocalOutputFolder( user_data_dir ) + "/RadsServiceLog.txt" );
}





bool CRadsSettingsBase::CommonConstruct()
{
	// 
	// do not SaveConfig(), otherwise settings in file are overwritten before read
	//
	mLastSync = QDateTime::currentDateTime();

	return ::ReadAvailableRadsMissions( mRadsPaths.mRadsConfigurationFilePath, mAllAvailableMissions );
}

// for rads application settings derived class

CRadsSettingsBase::CRadsSettingsBase( const CApplicationStaticPaths &brat_paths, const std::string &org_name, const std::string &exec_name ) 
	: base_t( org_name, exec_name, "" )
	, mRadsPaths( brat_paths )
	, mValidRadsMissions( CommonConstruct() )
{}

// for rads clients shared settings access derived class

CRadsSettingsBase::CRadsSettingsBase( const CApplicationStaticPaths &brat_paths, const std::string &path ) 
	: base_t( path )
	, mRadsPaths( brat_paths )
	, mValidRadsMissions( CommonConstruct() )
{}


template< class CONTAINER_ITEM >
std::ostream& operator << ( std::ostream &os, const std::vector< CONTAINER_ITEM > &o )
{
	for ( auto const &item : o )
		os << item << std::endl;
	return os;
}


std::ostream& operator << ( std::ostream &os, const CRadsSettingsBase &o )
{
	return os
		<< "MissionNames == " << o.mMissionNames << std::endl
		<< "UserDataDir == " << o.mUserDataDir << std::endl
		<< "LogFilePath() == " << o.LogFilePath() << std::endl
		<< "OutputDirectory() == " << o.OutputDirectory() << std::endl
		<< "NumberOfDays == " << n2s<std::string>( o.mNumberOfDays ) << std::endl
		<< "AllAvailableMissions == " << o.mAllAvailableMissions << std::endl
		<< "LastSync == " << q2a( o.mLastSync.toString() ) << std::endl
		<< "ValidRadsMissions == " << o.mValidRadsMissions << std::endl
		;
}


QDate CRadsSettingsBase::NextSyncDateForPeriodWithDays( int ndays )
{
	assert__( mLastSync.isValid() && mLastSync <= QDateTime::currentDateTime() );	//the ctor and LoadConfig must ensure this

	//function body

	auto next_sync_date = mLastSync.addDays( ndays ).date();
	if ( next_sync_date < QDate::currentDate() )
	{
		next_sync_date = QDate::currentDate();
	}

	assert__( next_sync_date >= QDate::currentDate() );

	return next_sync_date;
}


QDate CRadsSettingsBase::NextSyncDate()
{
	return NextSyncDateForPeriodWithDays( NumberOfDays() );
}


bool CRadsSettingsBase::SetNextSyncDateToday()
{
	auto now = QDateTime::currentDateTime();
	mLastSync = QDateTime( now.addDays( -NumberOfDays() ).date() );		assert__( NextSyncDate() == QDate::currentDate() );

	return true;
}


bool CRadsSettingsBase::CorrectLastSyncTime()
{
	if ( !mLastSync.isValid() || mLastSync > QDateTime::currentDateTime() )
	{
		return SetNextSyncDateToday();
	}

	assert__( mLastSync <= QDateTime::currentDateTime() && NextSyncDate() >= QDate::currentDate() );

	return false;
}




bool CRadsSettingsBase::SaveConfig()
{
	const std::string missions_str = Vector2String( mMissionNames, " " );


	qDebug() << "Saved mLastSync==" << mLastSync.toString( CRadsSettingsBase::RsyncDateFormat() );


	return
		WriteSection( GROUP_COMMON, 

			k_v( ENTRY_RADS_MISSIONS,			missions_str ),
			k_v( ENTRY_RADS_NUMBER_OF_DAYS,		mNumberOfDays ),
			k_v( ENTRY_USER_DATA_DIR,			mUserDataDir ),
			k_v( ENTRY_LAT_SYNC_TIME,			mLastSync )
		)
		&&
		mSettings.status() == QSettings::NoError;
}


bool CRadsSettingsBase::LoadConfig()
{
	std::string missions_str;

	bool result =
		ReadSection( GROUP_COMMON,

			k_v( ENTRY_RADS_MISSIONS,			&missions_str ),
			k_v( ENTRY_RADS_NUMBER_OF_DAYS,		&mNumberOfDays, (decltype(mNumberOfDays))smDefaultNumberOfDays ),
			k_v( ENTRY_USER_DATA_DIR,			&mUserDataDir ),
			k_v( ENTRY_LAT_SYNC_TIME,			&mLastSync )
		);

	mMissionNames = String2Vector( missions_str, std::string( " " ) );
	CleanStringVector( mMissionNames );

	//exclude from user missions all those removed from rads configuration available missions

	if ( result )
	{
		for ( int i = 0; i < mMissionNames.size(); )
		{
			const std::string name = FindRadsMissionName( mMissionNames[ i ], mAllAvailableMissions );
			if ( name.empty() )
				mMissionNames.erase( mMissionNames.begin() + i );
			else
				++i;
		}
	}


	qDebug() << "Loaded mLastSync==" << mLastSync.toString( CRadsSettingsBase::RsyncDateFormat() );


	return result
		&&
		mSettings.status() == QSettings::NoError;
}




////////////////////////////////////////////////////////////////////////////////////////////
//
//									CRadsSettings
//
////////////////////////////////////////////////////////////////////////////////////////////


//static 
const unsigned CRadsSettings::smDefaultPeriodicCheckInMinutes =
#if defined(DEBUG) || defined(_DEBUG)
	1;
#else
	60;
#endif




CRadsSettings::CRadsSettings( const CApplicationStaticPaths &brat_paths ) //base_t( brat_paths.mRadsServiceIniFilePath )
	: base_t( brat_paths, ORGANIZATION_NAME, RADS_SERVICE_NAME )
{}



bool CRadsSettings::SetLastSyncTimeNow()
{
	mLastSync = QDateTime::currentDateTime();

	return true;
}


std::ostream& operator << ( std::ostream &os, const CRadsSettings &o )
{
	return os 
		<< static_cast< const CRadsSettings::base_t& >( o )
		<< "PeriodicCheckInMinutes == " << n2s<std::string>( o.mPeriodicCheckInMinutes ) << std::endl
		;

	return os;
}


bool CRadsSettings::LockFile( bool lock ) 
{ 
	mLockedFile = lock; 
	if ( !mLockedFile )
    {
        Sync();
		return LoadConfig();
    }
	return true;
}


bool CRadsSettings::SaveConfig()
{
	bool result =
		!mLockedFile
		&&
		base_t::SaveConfig()
		&&
		WriteSection( GROUP_COMMON, 

			k_v( ENTRY_PERIODIC_CHECK_IN_MINUTES, mPeriodicCheckInMinutes )
		)
		&&
		mSettings.status() == QSettings::NoError;

	Sync();

	return result;
}


bool CRadsSettings::LoadConfig()
{
	bool result = 
		!mLockedFile
		&&
		base_t::LoadConfig()
		&&
		ReadSection( GROUP_COMMON,

			k_v( ENTRY_PERIODIC_CHECK_IN_MINUTES, &mPeriodicCheckInMinutes )
		)
		&&
		mSettings.status() == QSettings::NoError;

	if ( result )
	{
		if ( CorrectLastSyncTime() )
			Sync();
	}

	return result;
}






////////////////////////////////////////////////////////////////////////////////////////////
//
//									CSharedRadsSettings
//
////////////////////////////////////////////////////////////////////////////////////////////


bool CSharedRadsSettings::SetApplicationParameterValues( const std::string &missions_str, int ndays, const std::string &user_data_dir )
{
	if ( ndays < smMinNumberOfDays || ndays > smMaxNumberOfDays )
		return false;

	if ( !IsDir( user_data_dir ) )
		return false;

	if ( !IsDir( user_data_dir ) )
		return false;

	const std::string rads_data_dir = FormatRadsLocalOutputFolder( user_data_dir );
	if ( !IsDir( rads_data_dir ) && !MakeDirectory( rads_data_dir ) )
		return false;

	mUserDataDir = user_data_dir;
	mMissionNames = String2Vector( missions_str, missions_separator );
	mNumberOfDays = ndays;

	return SaveConfig();
}


bool CSharedRadsSettings::SaveConfig()
{
	bool result = base_t::SaveConfig();

	Sync();

	return result;
}
