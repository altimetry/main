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
#if !defined(WORKSPACES_RADS_DATASET_H)
#define WORKSPACES_RADS_DATASET_H

#include "Dataset.h"
#include "process/rads/RadsSharedParameters.h"


class CRadsDataset : public CDataset
{
	//types

	using base_t = CDataset;


	//instance data

	// See note in class declaration "access" section
	//
	std::vector< CRadsMission > mMissions;
	std::string mPath;

public:

	// construction

	explicit CRadsDataset( const std::string &name )
		: base_t( name )
	{}

	CRadsDataset( const CRadsDataset &o )
		: base_t( o )
		, mMissions( o.mMissions )
		, mPath( o.mPath )
	{}

	CRadsDataset& operator= ( const CRadsDataset &o )
	{
		if ( this != &o )
		{
			static_cast< base_t& >( *this ) = o;
			mMissions = o.mMissions;
			mPath = o.mPath;
		}
		return *this;
	}


	// destructor

	virtual ~CRadsDataset()
	{}


	//persistence

	virtual bool SaveConfig( CWorkspaceSettings *config ) const override;
	virtual bool LoadConfig( CWorkspaceSettings *config ) override;


	//access

	//NOTE: the function names reflect the initial concept of multiple missions per dataset.
	//	This naming concept was preserved, but in fact only one mission per dataset is 
	//	implemented (rads missions have different product types). So, we have a RemoveAllMissions 
	//	(which simply clears all), but not a RemoveMission (which would be more selective and time 
	//	consuming, removing only a specific mission's files).	

	const std::vector< CRadsMission >& Missions() const { return mMissions; }

	bool SetMission( const std::string &rads_server_address, const std::string &local_rads_dir, const CRadsMission &mission, std::string &warnings );

    bool HasMission( const std::string &mission_name ) const;
    
protected:
	bool AddMission( const std::string &rads_server_address, const std::string &local_rads_dir, const CRadsMission &mission, std::string &warnings );
    bool RemoveAllMissions();
	bool AddMissionFiles( const std::vector< std::string > &paths, std::string &warnings );
	bool SetMissionFiles( const std::vector< std::string > &paths, std::string &warnings );
};

/** @} */


#endif // !defined(WORKSPACES_RADS_DATASET_H)
