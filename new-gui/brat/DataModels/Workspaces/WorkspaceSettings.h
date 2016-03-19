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

#ifndef WORKSPACES_WORKSPACE_SETTINGS_H
#define WORKSPACES_WORKSPACE_SETTINGS_H

#include "new-gui/Common/ApplicationSettings.h"


class CWorkspace;
class CDataset;
class CWorkspaceDataset;
class CMapFormula;
class CFormula;
class CWorkspaceOperation;
class COperation;
class CWorkspaceDisplay;
class CDisplay;
class CMapDisplayData;
class CDisplayData;

class CMapFunction;

class CApplicationPaths;



class CWorkspaceSettings : public CFileSettings
{
	//types

    using base_t = CFileSettings;

	//static members

	static const CApplicationPaths *smBratPaths;


	//construction / destruction

public:

	static void SetApplicationPaths( const CApplicationPaths &paths )
	{
		smBratPaths = &paths;
	}

public:
	CWorkspaceSettings( const std::string &path ) :
		base_t(path)
	{
		assert__( smBratPaths );
	}

	virtual ~CWorkspaceSettings()
	{}


	// Load/Save methods

	bool SaveCommonConfig( const CWorkspace &wks, bool flush );
	bool LoadCommonConfig( CWorkspace &wks );


	bool SaveConfig( const CDataset *d );
	bool LoadConfig( CDataset *d );
	bool SaveConfigDataset( const CWorkspaceDataset &data, std::string &errorMsg );
	bool LoadConfigDataset( CWorkspaceDataset &data, std::string &errorMsg );

	bool SaveConfig( const COperation &op, const CWorkspaceOperation *wks );
	bool LoadConfig( COperation &op, std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceOperation *wkso );
	bool SaveConfigOperation( const CWorkspaceOperation &op, std::string &errorMsg );
	bool LoadConfigOperation( CWorkspaceOperation &op, std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceOperation *wkso );

	bool SaveConfig( const CMapFormula &mapf, bool predefined, const std::string& pathSuff );
	bool LoadConfig( CMapFormula &mapf, std::string &errorMsg, bool predefined, const std::string& pathSuff );

	bool SaveConfigPredefined( const CFormula &f, const std::string& pathSuff );
	bool SaveConfig( const CFormula &f, const std::string& pathSuff );
	bool LoadConfig( CFormula &f, std::string &errorMsg, const std::string& pathSuff );
	bool SaveConfigDesc( const CFormula &f, const std::string& path );
	bool LoadConfigDesc( CFormula &f, const std::string& path );

	bool SaveConfigDisplay( const CWorkspaceDisplay &disp, std::string &errorMsg, CWorkspaceDisplay *wksd );
	bool LoadConfigDisplay( CWorkspaceDisplay &disp, std::string &errorMsg, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso );
	bool SaveConfig( const CDisplay &d, CWorkspaceDisplay *wksd );
	bool LoadConfig( CDisplay &d, std::string &errorMsg, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso );
	bool SaveConfig( const CMapDisplayData &data, CWorkspaceDisplay *wks, const std::string& pathSuff );
	bool LoadConfig( CMapDisplayData &data, std::string &errorMsg, CWorkspaceDisplay *wks, CWorkspaceOperation *wkso, const std::string& pathSuff );
	bool SaveConfig( CDisplayData &data, const std::string& pathSuff, CWorkspaceDisplay *wksd );
	bool LoadConfig( CDisplayData &data, const std::string& path, CWorkspaceDisplay *wks, CWorkspaceOperation *wkso );


	void SaveFunctionDescrTemplate( const std::string &internal_data_path, bool flush );
	std::string GetFunctionExtraDescr( const std::string& path );


protected:

	std::string Absolute2PortableDataPath( const std::string &path ) const;

	std::string Portable2AbsoluteDataPath( const std::string &path ) const;
};


#endif		// WORKSPACES_WORKSPACE_SETTINGS_H
