#ifndef QT_INTERFACE_H
#define QT_INTERFACE_H

#if defined(BRAT_V3)
#include "display/wxInterface.h"
#include "wx/fileconf.h"		// (wxFileConfig class)
#define CONFIG wxFileConfig
#else
#define CONFIG QSettings
#endif



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Common declarations (interfaces) implemented differently in QtInterface.cpp and wxInterface.cpp / wxGuiInterface.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "display/PlotData/BratLookupTable.h"


//
//	CPlotField					=> CBratObject
//	CPlot		=> CPlotBase	=> CBratObject
//	CWPlot		=> CPlotBase
//	CZFXYPlot	=> CPlotBase
//
bool LoadFromFile( CBratLookupTable &lut, const std::string& fileName );

void SaveToFile( CBratLookupTable &lut, const std::string& fileName );



//////////////////////////////////////////////////////////////////////////////////////////////////////////
//											Configurations
//////////////////////////////////////////////////////////////////////////////////////////////////////////

class CDataset;
class CDisplayData;
class CWorkspaceDisplay;
class CWorkspaceOperation;
class CMapFormula;
class CWorkspace;
class CWorkspaceDataset;


struct CConfiguration : public CONFIG
{
	typedef CONFIG base_t;

	CConfiguration( const std::string& localFilename );

	bool SaveConfig( const CDataset *d );
	bool LoadConfig( CDataset *d );

	bool SaveConfig( CDisplayData &data, const std::string& pathSuff, CWorkspaceDisplay *wksd );
	bool LoadConfig( CDisplayData &data, const std::string& path, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso );

	bool SaveConfig( const CMapFormula &mapf, bool predefined, const std::string& pathSuff );
	bool LoadConfig( CMapFormula &mapf, std::string &errorMsg, bool predefined, const std::string& pathSuff );

	bool LoadCommonConfig( CWorkspace &wks );
	bool SaveCommonConfig( const CWorkspace &wks, bool flush );

	bool SaveConfigDataset( const CWorkspaceDataset &data, std::string &errorMsg );
	bool LoadConfigDataset( CWorkspaceDataset &data, std::string &errorMsg );

	bool SaveConfigOperation( const CWorkspaceOperation &op, std::string &errorMsg );
	bool LoadConfigOperation( CWorkspaceOperation &op, std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceOperation *wkso );

	bool SaveConfigDisplay( const CWorkspaceDisplay &disp, std::string &errorMsg, CWorkspaceDisplay *wksd );
	bool LoadConfigDisplay( CWorkspaceDisplay &disp, std::string &errorMsg, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso );

	///

	void SetPath( const std::string& strPath );

	bool Write( const std::string& key, const std::string& value );
	bool Write( const std::string& key, const int& value );

	size_t GetNumberOfEntries();
	bool GetNextEntry( std::string& str, long& lIndex );

	std::string Read( const std::string& str ) const;
	bool Read( const std::string& str, std::string *value ) const;
	std::string Read( const std::string& str, const std::string &value ) const;
	bool Read( const std::string& str, int *value ) const;

	bool Read( const std::string& str, double *value, double defValue ) const;
	bool Read( const std::string& str, int32_t *value, double defValue ) const;

	bool Read( const std::string& str, bool *value, bool defValue ) const;

	bool Flush( bool bCurrentOnly = false );

	const std::string GetConfigPath() const;

	virtual bool DeleteAll();
};



//////////////////////////////////////////////////////////////////////////////////////////////////////////
//											File Names
//////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string GetFileName( const std::string &path );

void SetFileExtension( std::string &path, const std::string &extension );

std::string NormalizedPath( const std::string &path, const std::string &dir );






#endif		//QT_INTERFACE_H
