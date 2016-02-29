#include "new-gui/brat/stdafx.h"

#include "QtInterface.h"

#include "Common/QtUtils.h"

#include "new-gui/brat/DataModels/Workspaces/Dataset.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//											File Names
//////////////////////////////////////////////////////////////////////////////////////////////////////////


std::string GetFileName( const std::string &path )
{
	return GetBaseFilenameFromPath( path );
}

void SetFileExtension( std::string &path, const std::string &extension )
{
	std::string dir = GetDirectoryFromPath( path );
	std::string name = GetFileName( path );
	path = dir + "/" + name + "." + extension;
}

std::string NormalizedAbsolutePath( const std::string &path, const std::string &dir )
{
	std::string d = NormalizedPath( dir );
	std::string f = NormalizedPath( path );
	if ( QDir::isAbsolutePath( t2q( f ) ) )
		return f;
	else
		return dir + "/" + path;
}
