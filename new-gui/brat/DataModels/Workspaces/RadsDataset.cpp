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

#include "WorkspaceSettings.h"
#include "RadsDataset.h"


template< class STRING >
inline bool TraverseDirectory_( const STRING &sourceFolder, const STRING &destFolder, const QStringList &filters, 
	std::function< bool( const STRING &, const STRING & ) > f = CopyFileProc )
{
	const QString qsource_folder = t2q( sourceFolder );
	const QString qdest_folder = t2q( destFolder );

	QDir sourceDir( qsource_folder );
	if( !sourceDir.exists() )
		return false;

	if ( !qdest_folder.isEmpty() )
	{
		QDir destDir( qdest_folder );
		if( !destDir.exists() )
			destDir.mkdir( qdest_folder );
	}

	{
		QStringList files = sourceDir.entryList( filters, QDir::Files );
		for ( int i = 0; i < files.count(); i++ )
		{
			STRING src_path = sourceFolder + "/" + q2t<STRING>( files[ i ] );
			STRING dest_path = qdest_folder.isEmpty() ? empty_string() : destFolder + "/" + q2t<STRING>( files[ i ] );
			if ( !f( src_path, dest_path ) )
				return false;
		}
	}
	QStringList dirs;
	dirs = sourceDir.entryList( QDir::AllDirs | QDir::NoDotAndDotDot );
	for(int i = 0; i< dirs.count(); i++)
	{
		STRING src_path = sourceFolder + "/" + q2t<STRING>( dirs[i] );
		STRING dest_path = qdest_folder.isEmpty() ? empty_string() : destFolder + "/" + q2t<STRING>( dirs[i] );
		if ( !TraverseDirectory_( src_path, dest_path, filters, f ) )
			return false;
	}
	return true;
}


template< class STRING >
inline bool TraverseDirectory_( const STRING &sourceFolder, const STRING &destFolder, std::initializer_list< STRING > filters,
	std::function< bool( const STRING &, const STRING & ) > f = CopyFileProc )
{
	QStringList qfilters;
	for ( auto const &filter : filters )
		qfilters << t2q( filter );

	return TraverseDirectory_( sourceFolder, destFolder, qfilters, f );
}



bool CRadsDataset::AddMissionFiles( const std::vector< std::string > &paths, std::string &warnings )
{
	for ( auto const& path : paths )
	{
		std::string normalized_path = NormalizedPath( path );

		GetProductList()->Insert( normalized_path );
	}

	try
	{
		CheckFiles( true, true );                           // Check only first file
				
		if ( ( m_files.IsYFX() || m_files.IsZFXY() || m_files.IsGenericNetCdf() ) && m_files.size() > 1 )//TODO REPLACE THIS BY A CHECK OF RADS CLASS
		{
			std::string msg = "Warning - You have to check that all the files in the list : "
				"\n1) are in the same way (same structure, same fields with same dimension...)"
				"\n2) contain the same kind of data"
				"\n\n otherwise results may be ill-defined and confused or Brat may return a reading error.";
			warnings += ( "\n" + msg );
		}
	}
	catch ( CException& e )
	{
		warnings += ( std::string( "\nUnable to process files. Reason: " ) + e.what() );
		// TODO: Delete all products from dataset ???
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
    TraverseDirectory_<std::string>( input_path, empty_string(), { filter }, [&paths]( const std::string &s1, const std::string & ) -> bool
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
		( Missions().empty() || RemoveAllMissions( rads_server_address, local_rads_dir ) )
		&&
		AddMission( rads_server_address, local_rads_dir, mission, warnings );
}


bool CRadsDataset::RemoveAllMissions( const std::string &rads_server_address, const std::string &local_dir )
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
