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
//#include "stdafx.h"

// While we can't include stdafx.h, this pragma must be here
//
#if defined (__APPLE__)
#pragma clang diagnostic ignored "-Wdeprecated-register"
#endif

#include "new-gui/Common/+UtilsIO.h"
#include "new-gui/Common/QtUtils.h"

#include "FileSettings.h"


////////////////////////////////////////////////////////////////////////////////////////////
//
//									CFileSettings
//
////////////////////////////////////////////////////////////////////////////////////////////


bool CFileSettings::CheckVersion()
{	
	// lambdas

	//	- remove spaces from group names

	auto clean_groups = []( const std::string &s ) -> std::string
	{
		const size_t len = s.length();
		std::string new_s;
		for ( size_t i = 0; i < len; ++i )
		{
			new_s += s[ i ];
			if ( s[ i ] == '[' )
			{
				++i;
				for ( size_t j = i + 1; j < len; ++j )
				{
					if ( s[ j ] == ']' )
					{
						std::string group = s.substr( i, j - i + 1 );	//group name and the closing ']'
						new_s += replace( group, "\\ ", "" );
						i = j;
						break;
					}
				}
			}
		}
		return new_s;
	};

	//	- remove spaces from key names
	//	- put "" around predefined constant values containing commas 

	auto clean_keys = []( const std::string &s ) -> std::string
	{
		const size_t len = s.length();
		std::string new_s;
		for ( size_t i = 0; i < len; ++i )
		{
			new_s += s[ i ];
			if ( s[i] == '\n' )
			{
				if ( s[ i + 1 ] == '[' )	//we are at the beginning of a section (group) not of a key
					continue;
				++i;
				for ( size_t j = i + 1; j < len; ++j )
				{
					if ( s[ j ] == '=' )
					{
						std::string key = s.substr( i, j - i + 1 );	//key name and the closing '='
						new_s += replace( key, "\\ ", "" );
						i = j;
						break;
					}
				}
			}
		}
		return replace( replace( new_s, 
			VALUE_OPERATION_TypeZFXY,		"\"" + VALUE_OPERATION_TypeZFXY + "\"" ), 
			VALUE_DISPLAY_eTypeZFLatLon,	"\"" + VALUE_DISPLAY_eTypeZFLatLon + "\"" );
	};


	// -- function body --

	{
		CSection section( mSettings, GROUP_SETTINGS_VERSION );
		if ( mSettings.childKeys().empty() )
			mVersion = UnknownVersionValue();
		else
			mVersion = ReadValue( section, KEY_SETTINGS_VERSION );
	}
	if ( mVersion == VersionValue() )							//"our" format, no format check/update necessary
		return true;

	std::string new_path = q2a( mSettings.fileName() );			//the existing path, to save new contents
	if ( !IsFile( new_path ) )
        return true;

	std::string s;
	if ( !Read2String( s, new_path ) )
		return false;

	std::string new_s = clean_keys( clean_groups( s ) );
	if ( s == new_s )
		return true;

	std::string old_path = CreateUniqueFileName( new_path );			//new path, to save old contents
	if ( old_path.empty() || !DuplicateFile( new_path, old_path ) )		//save "old" file
		return false;

	// Having changed the content, we must not only save to "new" file but also 
	//	WriteVersionSignature, otherwise we risk to damage the content next time,
	//	by "blindly" making the same replacements over replacements already done.
	//
	if ( !Write2File( new_s, new_path ) || !WriteVersionSignature() )	//save "new" file
		return false;

	Sync();

	return true;
}
