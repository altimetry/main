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

#include "common/tools/Exception.h"
#include "RadsSettings.h"


//entries in rads service ini file (operational)

const std::string ENTRY_RADS_MISSIONS =			"missions";
const std::string ENTRY_RADS_NUMBER_OF_DAYS =	"days";
const std::string ENTRY_RADS_OUTPUT_DIRECTORY =	"out_dir";
const std::string ENTRY_LAT_SYNC_TIME =			"last_sync_time";



////////////////////////////////////////////////////////////////////////////////////////////
//
//									CRadsSettingsBase
//
////////////////////////////////////////////////////////////////////////////////////////////


//static 
unsigned CRadsSettingsBase::smNumberOfDays = 3;


bool CRadsSettingsBase::ReadAvailableRadsMissions()
{
	return ::ReadAvailableRadsMissions( mRadsPaths.mRadsConfigurationFilePath, mAllAvailableMissions );
}


bool CRadsSettingsBase::SaveConfig()
{
	const std::string missions_str = Vector2String( mMissionNames, " " );

	return
		WriteSection( GROUP_COMMON, 

			k_v( ENTRY_RADS_MISSIONS,			missions_str ),
			k_v( ENTRY_RADS_NUMBER_OF_DAYS,		mNumberOfDays ),
			k_v( ENTRY_RADS_OUTPUT_DIRECTORY,	mOutputDirectory )
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
			k_v( ENTRY_RADS_NUMBER_OF_DAYS,		&mNumberOfDays, (decltype(mNumberOfDays))smNumberOfDays ),
			k_v( ENTRY_RADS_OUTPUT_DIRECTORY,	&mOutputDirectory )
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

	return result
		&&
		mSettings.status() == QSettings::NoError;
}




////////////////////////////////////////////////////////////////////////////////////////////
//
//									CRadsSettings
//
////////////////////////////////////////////////////////////////////////////////////////////



CRadsSettings::CRadsSettings( const CApplicationStaticPaths &brat_paths ) :
	//base_t( brat_paths.mRadsServiceIniFilePath )
	base_t( brat_paths, ORGANIZATION_NAME, RADS_SERVICE_NAME )
{}


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
	return
		!mLockedFile
		&&
		base_t::SaveConfig()
		&&
		WriteSection( GROUP_COMMON, 
	 
			k_v( ENTRY_LAT_SYNC_TIME,			mLastSync )
		)
		&&
		mSettings.status() == QSettings::NoError;
}


bool CRadsSettings::LoadConfig()
{
	return 
		!mLockedFile
		&&
		base_t::LoadConfig()
		&&
		ReadSection( GROUP_COMMON,

			k_v( ENTRY_LAT_SYNC_TIME,			&mLastSync )
		)
		&&
		mSettings.status() == QSettings::NoError;
}






////////////////////////////////////////////////////////////////////////////////////////////
//
//									CSharedRadsSettings
//
////////////////////////////////////////////////////////////////////////////////////////////


bool CSharedRadsSettings::SetApplicationParameters( const std::string &missions_str, int ndays, const std::string &output_dir )
{
	mMissionNames = String2Vector( missions_str );
	mNumberOfDays = ndays;
	mOutputDirectory = output_dir;

	bool result = SaveConfig();

	Sync();

	return result && mSettings.status() == QSettings::NoError;
}
