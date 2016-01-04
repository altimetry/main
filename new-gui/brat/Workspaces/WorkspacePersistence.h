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

#ifndef WORKSPACES_WORKSPACE_PERSISTENCE_H
#define WORKSPACES_WORKSPACE_PERSISTENCE_H

#include "new-gui/brat/ApplicationSettings.h"


class CWorkspace;
class CDataset;
class CWorkspaceDataset;
class CMapFormula;
class CFormula;


class CWorkspaceSettings : public CApplicationSettings
{
	using base_t = CApplicationSettings;

public:
	CWorkspaceSettings( const std::string &path ) :
		base_t( path )
	{}

	virtual ~CWorkspaceSettings()
	{}

	bool LoadCommonConfig( CWorkspace &wks );

	bool LoadConfig( CDataset *d );
	bool LoadConfigDataset( CWorkspaceDataset &data, std::string &errorMsg );

	bool LoadConfig( CMapFormula &mapf, std::string &errorMsg, bool predefined, const std::string& pathSuff );
	bool LoadConfig( CFormula &f, std::string &errorMsg, const std::string& pathSuff );

};


#endif		// WORKSPACES_WORKSPACE_PERSISTENCE_H
