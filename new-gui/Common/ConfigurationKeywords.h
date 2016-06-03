#if !defined COMMON_CONFIGURATION_KEYWORDS_H
#define COMMON_CONFIGURATION_KEYWORDS_H

#include <string>

#include "new-gui/Common/+Utils.h"


const std::string GROUP_SETTINGS_VERSION = "SettingsVersion";
const std::string KEY_SETTINGS_VERSION = "version";

const std::string GROUP_COMMON = "Common";
const std::string ENTRY_USER_MANUAL = "UserManual";
const std::string ENTRY_USER_MANUAL_VIEWER = "UserManualViewer";
const std::string ENTRY_LAST_DATA_PATH = "LastDataPath";
const std::string ENTRY_LAST_PAGE_REACHED = "LastPage";
const std::string ENTRY_ADVANCED_OPERATIONS = "AdvancedOperations";

const std::string GROUP_WKS = "Workspaces";
const std::string GROUP_COLORTABLE = "ColorTable";
const std::string ENTRY_LAST = "Last";

const std::string GROUP_SEL_CRITERIA = "SelectionCriteria";
const std::string ENTRY_LAT_LON = "LatLon";
const std::string ENTRY_DATETIME = "DateTime";
const std::string ENTRY_CYCLE = "Cycle";
const std::string ENTRY_PASS_NUMBER = "Pass(Number)";
const std::string ENTRY_PASS_STRING = "Pass(String)";

const std::string GROUP_WKS_RECENT = "RecentWorkspaces";
const std::string KEY_WKS_RECENT = "Recent";				//v4


// Workspaces module
//
//const std::string GROUP_GENERAL = "WorkspaceGeneral";		//General -> WorkspaceGeneral; moved from Constants.h
//const std::string GROUP_GENERAL_V3 = "General";				//GROUP_GENERAL -> GROUP_GENERAL_V3

const std::string GROUP_GENERAL_READ = "";				//GROUP_GENERAL -> GROUP_GENERAL_READ; "General" -> ""; moved from Constants.h
const std::string GROUP_GENERAL_WRITE = "";				//GROUP_GENERAL -> GROUP_GENERAL_WRITE				  ; moved from Constants.h


//v4


const std::string GROUP_MAIN_WINDOW = "MainWindow";
const std::string KEY_MAIN_WINDOW_GEOMETRY = "geometry";
const std::string KEY_MAIN_WINDOW_STATE = "state";
const std::string KEY_MAIN_WINDOW_MAXIMIZED = "maximized";

const std::string ENTRY_LOAD_WKSPC_AT_STARTUP = "load_wkspc_at_startup";
const std::string ENTRY_MAIN_LAYER_BASE_TYPE = "main_layer_base_type";
const std::string ENTRY_VIEWS_LAYER_BASE_TYPE = "views_layer_base_type";
const std::string ENTRY_APP_STYLE = "application_style";
const std::string ENTRY_USE_DEFAULT_STYLE = "use_default_style";


// Workspaces module
//
const std::string VALUE_OPERATION_TypeYFX = "Y=F(X)";				// Created from CMapTypeOp ctor
const std::string VALUE_OPERATION_TypeZFXY = "Z=F(X,Y)";			// Created from CMapTypeOp ctor
const std::string VALUE_DISPLAY_eTypeZFLatLon = "Z=F(Lon,Lat)";		// Created from CMapTypeDisp ctor
const std::string OLD_VALUE_DISPLAY_eTypeZFLatLon = "Z=F(Lat,Lon)";	// Created from CDisplayData::LoadConfig






inline std::string BuildKey( const std::string &s )
{
	return replace( s, " ", "" );
}

inline std::string BuildComposedKey( std::initializer_list< const std::string > l )
{
	std::string key;
	for ( auto const &s : l )
		key += BuildKey( s );

	return key;
}



#endif	//COMMON_CONFIGURATION_KEYWORDS_H
