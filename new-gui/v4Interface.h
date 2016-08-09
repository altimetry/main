#ifndef QT_INTERFACE_H
#define QT_INTERFACE_H

#if defined(BRAT_V3)
#include "../support/code/legacy/display/wxInterface.h"
#include "wx/fileconf.h"		// (wxFileConfig class)
#define CONFIG wxFileConfig
#define CWorkspaceSettings CConfiguration
#define WORKSPACES_WORKSPACE_SETTINGS_H
//#include "new-gui/brat/DataModels/PlotData/BratLookupTable.h"
//#else
//#include "new-gui/brat/DataModels/Workspaces/WorkspaceSettings.h"
//#define CONFIG CWorkspaceSettings
#if !defined (LOG_INFO)
#define LOG_INFO( msg ) std::cout << msg << std::endl
#define LOG_WARN( msg ) std::cerr << msg << std::endl
#endif
#endif




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Common declarations (interfaces) implemented differently in QtInterface.cpp and wxInterface.cpp / wxGuiInterface.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//
//	CPlotField					=> CBratObject
//	CPlot		=> CPlotBase	=> CBratObject
//	CWPlot		=> CPlotBase
//	CZFXYPlot	=> CPlotBase
//
//bool LoadFromFile( CBratLookupTable &lut, const std::string& fileName );

//void SaveToFile( CBratLookupTable &lut, const std::string& fileName );



//////////////////////////////////////////////////////////////////////////////////////////////////////////
//											Configurations
//////////////////////////////////////////////////////////////////////////////////////////////////////////


// Workspaces related Configuration



class CDataset;
class CDisplay;
class CDisplayData;
class CMapDisplayData;
class CWorkspaceDisplay;
class CWorkspaceOperation;
class CFormula;
class CMapFormula;
class CWorkspace;
class CWorkspaceDataset;
class COperation;

class CMapFunction;


class CConfiguration : public CONFIG
{
	typedef CONFIG base_t;

public:
	CConfiguration( const std::string& localFilename );

	virtual void Sync();
	virtual void Clear();

	bool LoadCommonConfig( CWorkspace &wks );
	bool SaveCommonConfig( const CWorkspace &wks, bool flush );

	//
	
	bool SaveConfig( const CDataset *d );
	bool LoadConfig( CDataset *d );

	bool SaveConfig( CDisplayData &data, const std::string& pathSuff, CWorkspaceDisplay *wksd );
	bool LoadConfig( CDisplayData &data, const std::string& path, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso );

	bool SaveConfig( const CMapFormula &mapf, bool predefined, const std::string& pathSuff );
	bool LoadConfig( CMapFormula &mapf, std::string &errorMsg, bool predefined, const std::string& pathSuff );

	bool SaveConfigDataset( const CWorkspaceDataset &data, std::string &errorMsg );
	bool LoadConfigDataset( CWorkspaceDataset &data, std::string &errorMsg );

	bool SaveConfigOperation( const CWorkspaceOperation &op, std::string &errorMsg );
	bool LoadConfigOperation( CWorkspaceOperation &op, std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceOperation *wkso );

	bool SaveConfigDisplay( const CWorkspaceDisplay &disp, std::string &errorMsg, CWorkspaceDisplay *wksd );
	bool LoadConfigDisplay( CWorkspaceDisplay &disp, std::string &errorMsg, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso );

	bool LoadConfig( CFormula &f, std::string &errorMsg, const std::string& pathSuff );
	bool SaveConfig( const CFormula &f, const std::string& pathSuff );
	bool LoadConfigDesc( CFormula &f, const std::string& path );
	bool SaveConfigDesc( const CFormula &f, const std::string& path );
	bool SaveConfigPredefined( const CFormula &f, const std::string& pathSuff );

    bool SaveConfig( const COperation &op, const CWorkspaceOperation *wkso );
	bool LoadConfig( COperation &op, std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceOperation *wkso );

	bool LoadConfig( CMapDisplayData &data, std::string &errorMsg, CWorkspaceDisplay *wks, CWorkspaceOperation *wkso, const std::string& pathSuff );
	bool SaveConfig( const CMapDisplayData &data, CWorkspaceDisplay *wks, const std::string& pathSuff );

    bool SaveConfig( const CDisplay &d, CWorkspaceDisplay *wksd );
	bool LoadConfig( CDisplay &d, std::string &errorMsg, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso );

	void SaveFunctionDescrTemplate( const std::string &internal_data_path, bool flush );
	std::string GetFunctionExtraDescr( const std::string& pathSuff );
};



//////////////////////////////////////////////////////////////////////////////////////////////////////////
//											File Names
//////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string GetFileName( const std::string &path );

void SetFileExtension( std::string &path, const std::string &extension );

std::string NormalizedAbsolutePath( const std::string &path, const std::string &dir );






#endif		//QT_INTERFACE_H
