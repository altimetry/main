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

	std::string old_path = CreateUniqueFileName( new_path );		//new path, to save old contents
	if ( old_path.empty() || !DuplicateFile( new_path, old_path ) )	//save "old" file
		return false;

	if ( !Write2File( new_s, new_path ) )						   	//save "new" file
		return false;

	Sync();

	return true;
}
