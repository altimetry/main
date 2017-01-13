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

#include "new-gui/brat/stdafx.h"

#include "libbrathl/ExternalFilesNetCDF.h"
using namespace brathl;


#include "WorkspaceSettings.h"
#include "RadsDataset.h"


bool CRadsDataset::AddMissionFiles( const std::vector< std::string > &paths, std::string &warnings )
{
	for ( auto const& path : paths )
	{
		std::string normalized_path = NormalizedPath( path );

		GetProductList()->Insert( normalized_path );
	}

	try
	{
		CheckFiles( true, true );		// Check only first file
				
		if ( !CExternalFilesRads::IsTypeOf( m_files.m_productType ) )
		{
			std::string msg = "Warning - Please make sure that all the files in the list belong to RADS,"
				"\notherwise results may be ill-defined or BRAT may return a reading error.";
			warnings += ( "\n" + msg );
		}
	}
	catch ( CException& e )
	{
		warnings += ( std::string( "\nUnable to process files. Reason: " ) + e.what() );
	}

	return warnings.empty();
}


bool CRadsDataset::AddMission( const std::string &rads_server_address, const std::string &local_dir, const CRadsMission &mission, std::string &warnings )
{
	static const std::string filter( "*.nc" );

	if ( std::find( mMissions.begin(), mMissions.end(), mission ) != mMissions.end() )
		return false;

	mMissions.push_back( mission );

	//add files
	const std::string input_path = MissionPath( rads_server_address, local_dir, mission.mAbbr );
	std::vector< std::string >paths;
    TraverseDirectory<std::string>( input_path, empty_string(), { filter }, [&paths]( const std::string &s1, const std::string & ) -> bool
	{
		paths.push_back( s1 );
		return true;

	} );

	//$RADS_ROOT/data/<sat>/<phase>/cycle/<sat>p<pass>c<cycle>.nc

	return AddMissionFiles( paths, warnings );
}


bool CRadsDataset::SetMission( const std::string &rads_server_address, const std::string &local_rads_dir, const CRadsMission &mission, std::string &warnings )
{
	return
        ( Missions().empty() || RemoveAllMissions() )
		&&
		AddMission( rads_server_address, local_rads_dir, mission, warnings );
}


bool CRadsDataset::RemoveAllMissions()
{
	mMissions.clear();

	GetProductList()->RemoveAll();

	return true;
}


bool CRadsDataset::HasMission( const std::string &mission_name ) const
{
    return 
            std::find_if( mMissions.begin(), mMissions.end(), 
                          [&mission_name]( const CRadsMission &mission )
                          {
                                return mission.mName == mission_name;
                          } )
            != mMissions.end();
}


//virtual 
bool CRadsDataset::SaveConfig( CWorkspaceSettings *config ) const
{
	assert__( config );

	return config->SaveConfig( this );
}
//virtual 
bool CRadsDataset::LoadConfig( CWorkspaceSettings *config )
{
	assert__( config );

	return config->LoadConfig( this );
}
