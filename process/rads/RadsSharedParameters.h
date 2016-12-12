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

#if !defined RADS_SHARED_PARAMETERS_H
#define RADS_SHARED_PARAMETERS_H

#include "common/+UtilsIO.h"
#include "new-gui/Common/FileSettings.h"

//
//	Altimeter	Phase	Time						Cycles		Passes	Records
//
//	GEOSAT		A		31 Mar 1985 - 30 Sep 1986	001 - 025	61152	104395824
//				B		08 Nov 1986 - 30 Dec 1989	026 - 093		
//				D		31 Mar 1985 - 30 Sep 1986	001 - 025		
//	ERS-1		A		01 Aug 1991 - 14 Dec 1991	001 - 046	47890	83644555
//				B		14 Dec 1991 - 25 Mar 1992	047 - 081		
//				C		14 Apr 1992 - 20 Dec 1993	083 - 101		
//				D		24 Dec 1993 - 10 Apr 1994	103 - 138		
//				E		10 Apr 1994 - 28 Sep 1994	139 - 140		
//				F		28 Sep 1994 - 21 Mar 1995	141 - 143		
//				G		24 Mar 1995 - 02 Jun 1996	144 - 156		
//	TOPEX		A		25 Sep 1992 - 11 Aug 2002	001 - 364	111960	264208509
//				B		20 Sep 2002 - 08 Oct 2005	369 - 481		
//				N		11 Aug 2002 - 20 Sep 2002	365 - 368		
//	POSEIDON	A		01 Oct 1992 - 12 Jul 2002	001 - 361	7472	15718917
//	ERS-2		A		29 Apr 1995 - 04 Jul 2011	000 - 169	148130	168866998
//	GFO-1		A		07 Jan 2000 - 17 Sep 2008	037 - 223	82623	150249181
//	JASON-1		A		15 Jan 2002 - 26 Jan 2009	001 - 260	102175	279080066
//				B		10 Feb 2009 - 03 Mar 2012	262 - 374		
//				C		07 May 2012 - 21 Jun 2013	382 - 425		
//	ENVISAT1	B		14 May 2002 - 22 Oct 2010	006 - 094	99633	259990495
//				C		26 Oct 2010 - 08 Apr 2012	095 - 113		
//	JASON-2		A		04 Jul 2008 - 02 Oct 2016	000 - 303	77065	245664298
//				B		13 Oct 2016 - 29 Oct 2016	305 - 307		
//	CRYOSAT2	A		14 Jul 2010 - 29 Oct 2016	004 - 085	65033	171966710
//	SARAL		A		14 Mar 2013 - 04 Jul 2016	001 - 035	37839	106335546
//				B		04 Jul 2016 - 29 Oct 2016	036 - 039		
//
//	Total														840972	1850121099
//


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//											Missions
/////////////////////////////////////////////////////////////////////////////////////////////////////////


//	Altimeter	Abbr.	Nr	Alternatives							References
//	
//	GEOS 3		g3		1	ge3 geos-3 geos3						(not included in RADS)
//	Seasat		ss		2	sea seasat-a							(not included in RADS)
//	Geosat		gs		3	geo geosat
//	ERS-1		e1		4	er1 ers-1 ers1							[Francis, 1990; Francis et al., 1991]
//	TOPEX		tx		5	top topex								[Fu et al., 1994]
//	Poseidon	pn		6	pos poseidon
//	ERS-2		e2		7	er2 ers-2 ers2							[Francis et al., 1995]
//	GFO			g1		8	gfo gfo-1 gfo1
//	Jason-1		j1		9	ja1 jason-1 jason1						[M´enard et al., 2003]
//	Envisat		n1		10	en1 envisat
//	Jason-2		j2		11	ja2 jason-2 jason2						[Lambin et al., 2010]
//	CryoSat-2	c2		12	cs2 cryosat-2 cryosat2					[Wingham et al., 2006]
//	SARAL		sa		13	sa srl saral altika
//	Jason-3		j3		14	ja3 jason-3 jason3						(limited access in RADS)
//	HY-2A		2a		15	h2a hy-2a hy2a							(not included in RADS)
//	Sentinel-3A	3a		16	s3a sentinel-3a sentinel3a sntnl-3a		(in RADS summer 2016)
//	Sentinel-3B	3b		17	s3b sentinel-3b sentinel3b sntnl-3b		(to be launched end 2017)
//
//		Table 3.1 Abbreviation and numbers used for the various altimeter missions
//
//rads4_user_manual.pdf, p.8




struct CRadsMission
{
	std::string mName;
	std::string mAbbr;

	bool operator == ( const CRadsMission &o ) const { return mName == o.mName && mAbbr == o.mAbbr; }
};


// This function should only be used for testing purposes, unless
// hard-codded mission lists are acceptable; otherwise, the missions
// must be read from the rads configuration file (not to be confused 
// with the rads service configuration file).
//
inline const std::vector< CRadsMission >& RadsHardCodedAvailableMissions()
{
	static const CRadsMission missions[] =
	{
		//{ "GEOS 3",			"g3" },
		//{ "Seasat",			"ss" },
		{ "Geosat",			"gs" },
		{ "ERS-1",			"e1" },
		{ "TOPEX",			"tx" },
		{ "Poseidon",		"pn" },
		{ "ERS-2",			"e2" },
		{ "GFO",			"g1" },
		{ "Jason-1",		"j1" },
		{ "Envisat",		"n1" },
		{ "Jason-2",		"j2" },
		{ "CryoSat-2",		"c2" },
		{ "SARAL",			"sa" },
		{ "Jason-3",		"j3" },
		//{ "HY-2A",			"2a" },
		{ "Sentinel-3A",	"3a" },
		{ "Sentinel-3B",	"3b" },
	};

	static const DEFINE_ARRAY_SIZE( missions );

	static const std::vector< CRadsMission > vmissions( &missions[ 0 ], &missions[ missions_size ] );

	return vmissions;
}


inline const std::string& FindRadsMissionAbbr( const std::string &name, const std::vector< CRadsMission > &missions )
{
	auto it = std::find_if( missions.begin(), missions.end(), 
		[&name]( const CRadsMission &mission ) 
		{
			return name == mission.mName;
		} 
	);

	if ( it != missions.end() )
		return it->mAbbr;

	return empty_string<>();
}


inline const std::string& FindRadsMissionName( const std::string &abbr, const std::vector< CRadsMission > &missions )
{
	auto it = std::find_if( missions.begin(), missions.end(), 
		[&abbr]( const CRadsMission &mission ) 
	{
		return abbr == mission.mAbbr;
	} 
	);

	if ( it != missions.end() )
		return it->mName;

	return empty_string<>();
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////
//										Service Command Codes
/////////////////////////////////////////////////////////////////////////////////////////////////////////

enum ERadsServiceCmds
{
	eRadsService_ExecNow,
	eRadsService_LockConfig,
	eRadsService_UnlockConfig,

	ERadsServiceCmds_size,
};



/////////////////////////////////////////////////////////////////////////////////////////////////////////
//										Service Public Keys
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//entries in rads configuration file

const std::string ENTRY_RADS_ADDRESS =				"address";
const std::string GROUP_RADS_ALL_MISSIONS =			"AllMissions";
const std::string GROUP_RADS_AVAILABLE_MISSIONS =	"AvailableMissions";


//for IPC with client apps such as brat

const std::string RADS_SHARED_MEMORY_KEY =			"RADS_SHARED_MEMORY_KEY";

const std::string RSYNC_RUNNING_SIGN = "RUNNING";
const std::string RSYNC_STOPPED_SIGN = "STOPPED";
const std::string CONFIG_UPDATED_SIGN = "CONFIG_UPDATED";

struct CRadsSharedMemory
{
	char mSignature[ 256 ];
	bool mRsyncRunning = false;

	CRadsSharedMemory()
	{
		memset( mSignature, 0, sizeof(mSignature) );
	}
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//									RADS Configuration File Readers
/////////////////////////////////////////////////////////////////////////////////////////////////////////


//Separator to be used when converting missions vector to/from string
//	in RadsSettings variables
//
static const std::string missions_separator = " ";


inline bool ReadAvailableRadsMissions( const std::string &ini_path, std::vector< CRadsMission > &vavailable_missions )
{
	CFileSettings settings( ini_path.c_str() );
	vavailable_missions.clear();
	long maxEntries = 0;
	{
		CFileSettings::CSection section( settings, GROUP_RADS_AVAILABLE_MISSIONS );
		auto const &qkeys = section.Keys();
		maxEntries = qkeys.size();
		for ( auto const &qkey : qkeys )
		{
			auto key = q2a( qkey );
			auto value = settings.ReadValue( section, key );
			vavailable_missions.push_back( { value, key } );
		};
	}
	assert__( vavailable_missions.size() == maxEntries );			Q_UNUSED( maxEntries );		//for release builds

	return settings.Status() == QSettings::NoError;
}

// Format:
//
//	[USER@]HOST::SRC
//
// From rsync docs:
//
//	"The '::' usages connect to an rsync daemon, and require SRC to start with a module name."
//
// This means that SRC begins with a "module name" and brat will compose the remaining of SRC 
// according to user selected mission.
// Example:
//	If, to retrieve Jason2 data, we must use radsuser@corads.tudelft.nl::rads/data/j2
// then 
//		SRC=rads/data 
//	so the address entry (key=value) in the rads.ini file must be:
//		address=radsuser@corads.tudelft.nl::rads/data
//
// This entry belongs to the [Common] group
//
inline std::string ReadRadsServerAddress( const std::string &ini_path )
{
	CFileSettings settings( ini_path.c_str() );
	std::string address;
	settings.ReadSection( GROUP_COMMON,

		k_v( ENTRY_RADS_ADDRESS,			&address )
	);

	if ( settings.Status() != QSettings::NoError )
		return empty_string();

	return address;
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////
//										Resolve Rads Local Paths
/////////////////////////////////////////////////////////////////////////////////////////////////////////


inline std::string FormatRadsLocalOutputPath( const std::string &user_data_dir )
{
	return user_data_dir + "/rads";
}


// Assumes 
//	- rads_server_address was obtained by ReadRadsServerAddress
//	- local_dir was obtained by FormatRadsLocalOutputPath
//
inline std::string MissionPath( const std::string &rads_server_address, const std::string &local_dir, const std::string &mission )
{
	std::string local_subdir = filename_from_path( rads_server_address );
	if ( !local_subdir.empty() )
		local_subdir = "/" + local_subdir + "/";

	return local_dir + local_subdir + mission;
}





#endif	//RADS_SHARED_PARAMETERS_H
