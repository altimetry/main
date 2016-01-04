#include "new-gui/brat/stdafx.h"

#include "QtInterface.h"

#include "Common/QtUtilsIO.h"

#include "new-gui/brat/Workspaces/Dataset.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//										CBratLookupTable	
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////////////////////////////
//										CBratLookupTable - Load
//////////////////////////////////////////////////////////////////////////////////////////////////////////


bool LoadFromFile( CBratLookupTable &lut, const std::string& fileName )
{
	Q_UNUSED(lut);
	Q_UNUSED(fileName);

	SimpleWarnBox("Development message: LoadFromFile( CBratLookupTable &lut, const std::string& fileName ) called");
	return true;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////
//										CBratLookupTable - Store
//////////////////////////////////////////////////////////////////////////////////////////////////////////


void SaveToFile( CBratLookupTable &lut, const std::string& fileName )
{
	Q_UNUSED(lut);
	Q_UNUSED(fileName);

	SimpleWarnBox("Development message: SaveToFile( CBratLookupTable &lut, const std::string& fileName ) called");
}




//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//										CConfiguration
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////


// Workspaces related Configuration




#if defined (WIN32) || defined(_WIN32)
#pragma warning ( disable: 4100 )           //unreferenced formal parameter
#endif



CConfiguration::CConfiguration( const std::string& fileName )
	: base_t( fileName )
{}

void CConfiguration::Sync()
{
	base_t::Sync();
}

void CConfiguration::Clear()
{
	base_t::Clear();			//!!! TODO !!! : test equivalence with wxFileConfig DeleteAll
}

bool CConfiguration::LoadCommonConfig( CWorkspace &wks )
{
	return base_t::LoadCommonConfig( wks );
}
bool CConfiguration::LoadConfigDataset( CWorkspaceDataset &data, std::string &errorMsg )
{
	return base_t::LoadConfigDataset( data, errorMsg );
}
bool CConfiguration::LoadConfig( CDataset *d  )
{
	return base_t::LoadConfig( d );
}



#if defined (WIN32) || defined(_WIN32)
#pragma warning ( default: 4100 )           //unreferenced formal parameter
#endif



bool CConfiguration::LoadConfig( CMapFormula &mapf, std::string &errorMsg, bool predefined, const std::string& pathSuff )
{
	return base_t::LoadConfig( mapf, errorMsg, predefined, pathSuff );
}
bool CConfiguration::LoadConfig( CFormula &f, std::string &errorMsg, const std::string& pathSuff )
{
	return base_t::LoadConfig( f, errorMsg, pathSuff );
}



bool CConfiguration::SaveConfig( const CDataset *d )
{
	return true;
}
bool CConfiguration::LoadConfig( CDisplayData &data, const std::string& path, CWorkspaceDisplay *wks, CWorkspaceOperation *wkso )
{
	return true;
}
bool CConfiguration::SaveConfig( CDisplayData &data, const std::string& pathSuff, CWorkspaceDisplay *wksd )
{
	return true;
}
bool CConfiguration::SaveConfig( const CMapFormula &mapf, bool predefined, const std::string& pathSuff )
{
	return true;
}
bool CConfiguration::SaveCommonConfig( const CWorkspace &wks, bool flush )
{
	return true;
}
bool CConfiguration::SaveConfigDataset( const CWorkspaceDataset &data, std::string &errorMsg )
{
	return true;
}
bool CConfiguration::LoadConfigOperation( CWorkspaceOperation &op, std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceOperation *wkso )
{
	return true;
}
bool CConfiguration::SaveConfigOperation( const CWorkspaceOperation &op, std::string &errorMsg )
{
	return true;
}
bool CConfiguration::SaveConfigDisplay( const CWorkspaceDisplay &disp, std::string &errorMsg, CWorkspaceDisplay *wksd )
{
	return true;
}
bool CConfiguration::LoadConfigDisplay( CWorkspaceDisplay &disp, std::string &errorMsg, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso )
{
	return true;
}
bool CConfiguration::LoadConfigDesc( CFormula &f, const std::string& path )
{
	return true;
}
bool CConfiguration::SaveConfigDesc( const CFormula &f, const std::string& path)
{
	return true;
}
bool CConfiguration::SaveConfigPredefined( const CFormula &f, const std::string& pathSuff)
{
	return true;
}
bool CConfiguration::SaveConfig( const CFormula &f, const std::string& pathSuff )
{
	return true;
}
bool CConfiguration::SaveConfig( const COperation &op )
{
	return true;
}
bool CConfiguration::LoadConfig( CMapDisplayData &data, std::string &errorMsg, CWorkspaceDisplay *wks, CWorkspaceOperation *wkso, const std::string& pathSuff )
{
	return true;
}
bool CConfiguration::SaveConfig( const CMapDisplayData &data, CWorkspaceDisplay *wks, const std::string& pathSuff )
{
	return true;
}
bool CConfiguration::LoadConfig( CDisplay &d, std::string &errorMsg, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso )
{
	return true;
}
bool CConfiguration::SaveConfig( const CDisplay &d, CWorkspaceDisplay *wksd )
{
	return true;
}
bool CConfiguration::LoadConfig( COperation &op, std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceOperation *wkso )
{
	return true;
}






//////////////////////////////////////////////////////////////////////////////////////////////////////////
//											File Names
//////////////////////////////////////////////////////////////////////////////////////////////////////////


std::string GetFileName( const std::string &path )
{
	return GetBaeFilenameFromPath( path );
}

void SetFileExtension( std::string &path, const std::string &extension )
{
	std::string dir = GetDirectoryFromPath( path );
	std::string name = GetFilenameFromPath( path );
	path = path + "/" + name + "." + extension;
}

std::string NormalizedPath( const std::string &path, const std::string &dir )
{
	std::string d = NormalizedPath( dir );
	std::string f = NormalizedPath( path );
	if ( QDir::isAbsolutePath( t2q( f ) ) )
		return f;
	else
		return dir + "/" + path;
}
