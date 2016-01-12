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

bool CConfiguration::SaveCommonConfig( const CWorkspace &wks, bool flush )
{
	return base_t::SaveCommonConfig( wks, flush );
}
bool CConfiguration::LoadCommonConfig( CWorkspace &wks )
{
	return base_t::LoadCommonConfig( wks );
}

bool CConfiguration::SaveConfigDataset( const CWorkspaceDataset &data, std::string &errorMsg )
{
	return base_t::SaveConfigDataset( data, errorMsg );
}
bool CConfiguration::LoadConfigDataset( CWorkspaceDataset &data, std::string &errorMsg )
{
	return base_t::LoadConfigDataset( data, errorMsg );
}

bool CConfiguration::SaveConfig( const CDataset *d )
{
	throw;	// return base_t::SaveConfig( d );
}
bool CConfiguration::LoadConfig( CDataset *d  )
{
	throw;
}

bool CConfiguration::SaveConfig( const CMapFormula &mapf, bool predefined, const std::string& pathSuff )
{
	throw;
}
bool CConfiguration::LoadConfig( CMapFormula &mapf, std::string &errorMsg, bool predefined, const std::string& pathSuff )	//1	//throw
{
	throw;
}

bool CConfiguration::SaveConfigPredefined( const CFormula &f, const std::string& pathSuff)
{
	throw;
}
bool CConfiguration::SaveConfig( const CFormula &f, const std::string& pathSuff )
{
	throw;
}
bool CConfiguration::LoadConfig( CFormula &f, std::string &errorMsg, const std::string& pathSuff )	//2	//throw
{
	throw;
}

bool CConfiguration::SaveConfigOperation( const CWorkspaceOperation &op, std::string &errorMsg )
{
	return base_t::SaveConfigOperation( op, errorMsg );
}
bool CConfiguration::LoadConfigOperation( CWorkspaceOperation &op, std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceOperation *wkso )
{
	return base_t::LoadConfigOperation( op, errorMsg, wks, wkso );
}

bool CConfiguration::SaveConfig(const COperation &op , const CWorkspaceOperation *wkso)
{
	throw;
}
bool CConfiguration::LoadConfig( COperation &op, std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceOperation *wkso )
{
	throw;
}

bool CConfiguration::SaveConfigDesc( const CFormula &f, const std::string& path)
{
	throw;
}
bool CConfiguration::LoadConfigDesc( CFormula &f, const std::string& path )
{
	throw;
}

bool CConfiguration::SaveConfigDisplay( const CWorkspaceDisplay &disp, std::string &errorMsg, CWorkspaceDisplay *wksd )
{
	return base_t::SaveConfigDisplay( disp, errorMsg, wksd );
}
bool CConfiguration::LoadConfigDisplay( CWorkspaceDisplay &disp, std::string &errorMsg, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso )
{
	return base_t::LoadConfigDisplay( disp, errorMsg, wksd, wkso );
}

//bool CConfiguration::SaveConfig( const CDisplay &d, CWorkspaceDisplay *wksd )
//{
//	throw;
//}
bool CConfiguration::LoadConfig( CDisplay &d, std::string &errorMsg, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso )
{
	throw;
}

bool CConfiguration::SaveConfig( const CMapDisplayData &data, CWorkspaceDisplay *wks, const std::string& pathSuff )
{
	throw;
}
bool CConfiguration::LoadConfig( CMapDisplayData &data, std::string &errorMsg, CWorkspaceDisplay *wks, CWorkspaceOperation *wkso, const std::string& pathSuff )
{
	throw;
}

bool CConfiguration::SaveConfig( CDisplayData &data, const std::string& pathSuff, CWorkspaceDisplay *wksd )
{
	throw;
}
bool CConfiguration::LoadConfig( CDisplayData &data, const std::string& path, CWorkspaceDisplay *wks, CWorkspaceOperation *wkso )
{
	throw;
}




#if defined (WIN32) || defined(_WIN32)
#pragma warning ( default: 4100 )           //unreferenced formal parameter
#endif








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
