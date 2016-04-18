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

#ifndef DATA_MODELS_PLOT_DATA_BRAT_LOOKUP_TABLE_SETTINGS_H
#define DATA_MODELS_PLOT_DATA_BRAT_LOOKUP_TABLE_SETTINGS_H

#include "new-gui/Common/ApplicationSettings.h"
#include "BratLookupTable.h"


class CBratLookupTableSettings : public CFileSettings
{
	//types

    using base_t = CFileSettings;

	//construction / destruction

public:
	CBratLookupTableSettings( const std::string &path ) :
		base_t(path)
	{}

	virtual ~CBratLookupTableSettings()
	{}


	// Load/Save methods

	void SaveToFile( const CBratLookupTable &table );
	bool LoadFromFile( std::string &error_msg, CBratLookupTable &table );

	void SaveGradToFile( const CBratLookupTable &table );
	void SaveCustToFile( const CBratLookupTable &table );

	bool LoadGradFromFile( std::string &error_msg, CBratLookupTable &table );
	bool LoadCustFromFile( std::string &error_msg, CBratLookupTable &table );
};


#endif		// DATA_MODELS_PLOT_DATA_BRAT_LOOKUP_TABLE_SETTINGS_H
