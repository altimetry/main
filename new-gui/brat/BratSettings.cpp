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
#include "stdafx.h"

#include "libbrathl/Product.h"
#include "common/+UtilsIO.h"
#include "common/QtUtilsIO.h"
#include "DataModels/Workspaces/Workspace.h"

#include "BratSettings.h"

// Making group keys static ensures that they are private to the class and groups
//	can safely be cleared here before saving
//
static const std::string GROUP_WKS =								"Workspaces";
static const std::string GROUP_COLORTABLE =							"ColorTable";
static const std::string GROUP_PATHS =								"ApplicationPaths";
static const std::string GROUP_SEL_CRITERIA =						"SelectionCriteria";

static const std::string ENTRY_LAST_DATA_PATH =						"LastDataPath";
static const std::string ENTRY_LAST_PAGE_REACHED =					"LastPage";
static const std::string ENTRY_ADVANCED_OPERATIONS =				"AdvancedOperations";
static const std::string ENTRY_LAST =								"Last";

static const std::string ENTRY_LAT_LON =							"LatLon";
static const std::string ENTRY_DATETIME =							"DateTime";
static const std::string ENTRY_CYCLE =								"Cycle";
static const std::string ENTRY_PASS_NUMBER =						"Pass(Number)";
static const std::string ENTRY_PASS_STRING =						"Pass(String)";


//v4

static const std::string ENTRY_LOAD_WKSPC_AT_STARTUP =				"load_wkspc_at_startup";
static const std::string ENTRY_MAIN_LAYER_BASE_TYPE	=				"main_layer_base_type";
static const std::string ENTRY_VIEWS_LAYER_BASE_TYPE =				"views_layer_base_type";
static const std::string ENTRY_VECTOR_SIMPLIFY_METHOD =				"vector_simplify_method";

static const std::string ENTRY_URL_RASTER_LAYER_PATH =				"url_raster_layer_path";
static const std::string ENTRY_USER_DATA_PATH =						"user_data_path";
static const std::string ENTRY_WORKSPACES_DIR =						"workspaces_dir";
static const std::string ENTRY_PORTABLE_PATHS =						"portable_paths";
static const std::string ENTRY_DESKTOP_MANAGER_SDI =				"desktop_manager_sdi";
static const std::string ENTRY_CHECK_OPENGL =						"check_opengl_capabilities";
static const std::string ENTRY_USE_UNSUPPORTED_FIELDS =				"use_unsupported_fields";
static const std::string ENTRY_DISPLAY_RADS_INSTALL_INFORMATION =	"display_rads_install_information";
static const std::string ENTRY_MINIMUM_FILES_TO_WARN_USER =			"minimum_files_to_warn_user";

static const std::string ENTRY_H_CHILD_RATIO_NUM =					"h_child_ratio_num";
static const std::string ENTRY_H_CHILD_RATIO_DNM =					"h_child_ratio_dnm";
static const std::string ENTRY_PLOTS_FONT_NAME =					"plots_font_name";
static const std::string ENTRY_PLOTS_AXIS_FONT_SIZE = 				"plots_axis_font_size";
static const std::string ENTRY_PLOTS_TITLE_FONT_SIZE =				"plots_title_font_size";




////////////////////////////////////////////////////////////////////////////////////////////
//
//									CBratSettings
//
////////////////////////////////////////////////////////////////////////////////////////////


//static 
const CMapWidget::ELayerBaseType CBratSettings::smDefaultLayerBaseType =
#if defined(BRAT_ARCHITECTURE_64)
    CMapWidget::ELayerBaseType::eVectorLayer;
#else
    CMapWidget::ELayerBaseType::eVectorLayer;
#endif
//static 
const bool CBratSettings::smUseUnsupportedFields = false;

//static 
const size_t CBratSettings::smMinimumFilesToWarnUser = 1000;


//static 
const double CBratSettings::smHchildRatioNum = 2.;
//static 
const double CBratSettings::smHchildRatioDnm = 3.;

//static
const std::string CBratSettings::smPlotsFontName =  "Courier";

//static 
const int CBratSettings::smPlotsAxisFontSize = 10;
//static 
const int CBratSettings::smPlotsTitleFontSize = 12;






#if defined(BRAT_V3)

bool CBratSettings::SaveConfigSelectionCriteria()
{
	CMapProduct& mapProductInstance = CMapProduct::GetInstance();
	for ( CObMap::iterator it = mapProductInstance.begin(); it != mapProductInstance.end(); it++ )
	{
		CProduct* product = dynamic_cast<CProduct*>( it->second );
		if ( product == nullptr || !product->HasCriteriaInfo() )
			continue;

		std::string config_path = BuildComposedKey( { GROUP_SEL_CRITERIA, product->GetLabel() } );		
		{
			CSection section( mSettings, config_path );
			if ( product->IsSetLatLonCriteria() )
				WriteValue( section, ENTRY_LAT_LON, product->GetLatLonCriteria()->GetAsText() );

			if ( product->IsSetDatetimeCriteria() )
				WriteValue( section, ENTRY_DATETIME, product->GetDatetimeCriteria()->GetAsText() );

			if ( product->IsSetCycleCriteria() )
				WriteValue( section, ENTRY_CYCLE, product->GetCycleCriteria()->GetAsText() );

			if ( product->IsSetPassIntCriteria() )
				WriteValue( section, ENTRY_PASS_NUMBER, product->GetPassIntCriteria()->GetAsText() );

			if ( product->IsSetPassStringCriteria() )
				WriteValue( section, ENTRY_PASS_STRING, product->GetPassStringCriteria()->GetAsText() );
		}

		Sync();
	}

	return mSettings.status() == QSettings::NoError;
}

bool CBratSettings::LoadConfigSelectionCriteria()
{
	QStringList keys = mSettings.childGroups();
	if ( keys.empty() )
		return true;		qDebug() << keys;

	try {
		CMapProduct& mapProductInstance = CMapProduct::GetInstance();
		mapProductInstance.AddCriteriaToProducts();

		for ( CObMap::iterator it = mapProductInstance.begin(); it != mapProductInstance.end(); it++ )
		{
			CProduct* product = dynamic_cast<CProduct*>( it->second );

			if ( product == nullptr || !product->HasCriteriaInfo() )
				continue;

            std::string config_path = BuildComposedKey( { GROUP_SEL_CRITERIA, product->GetLabel() } );
			{
				CSection section( mSettings, config_path );
				QStringList keys = mSettings.childKeys();
				if ( keys.empty() )
					continue;

				std::string val;
				if ( product->HasLatLonCriteria() )
				{
					val = ReadValue( section, ENTRY_LAT_LON );
					if ( !val.empty() )
						product->GetLatLonCriteria()->Set( val );
				}
				if ( product->HasDatetimeCriteria() )
				{
					val = ReadValue( section, ENTRY_DATETIME );
					if ( !val.empty() )
						product->GetDatetimeCriteria()->SetFromText( val );
				}
				if ( product->HasCycleCriteria() )
				{
					val = ReadValue( section, ENTRY_CYCLE );
					if ( !val.empty() )
						product->GetCycleCriteria()->SetFromText( val );
				}
				if ( product->HasPassIntCriteria() )
				{
					val = ReadValue( section, ENTRY_PASS_NUMBER );
					if ( !val.empty() )
						product->GetPassIntCriteria()->SetFromText( val );
				}
				if ( product->HasPassStringCriteria() )
				{
					val = ReadValue( section, ENTRY_PASS_STRING );
					if ( !val.empty() )
						product->GetPassStringCriteria()->Set( val );
				}
			}
		}
	}
	catch ( const CException &e ) 
	{
		std::string msg( "An error occurred while loading criteria selection configuration (CBratGuiApp::LoadConfigSelectionCriteria)\nNative exception: " );
		msg += e.what();
		throw CException( msg, e.error() );
	}

	return mSettings.status() == QSettings::NoError;
}
#endif    //BRAT_V3


bool CBratSettings::LoadPaths()
{
	return
		ReadSection( GROUP_PATHS,

			k_v( ENTRY_USER_DATA_PATH,			&mBratPaths.mUserDataDirectory ),
            k_v( ENTRY_WORKSPACES_DIR,			&mBratPaths.mWorkspacesDirectory ),
            k_v( ENTRY_URL_RASTER_LAYER_PATH,   &mBratPaths.mURLRasterLayerPath, mBratPaths.smDefaultURLRasterLayerPath ),

            k_v( ENTRY_PORTABLE_PATHS,			&mBratPaths.mUsePortablePaths )
		)
		&&
        mBratPaths.SetUserPaths();
}

bool CBratSettings::SavePaths()
{
	ClearGroup( GROUP_PATHS );	

	return 
		WriteSection( GROUP_PATHS,

			k_v( ENTRY_USER_DATA_PATH,			mBratPaths.UserDataDirectory() ),
            k_v( ENTRY_WORKSPACES_DIR,          mBratPaths.WorkspacesDirectory() ),
            k_v( ENTRY_URL_RASTER_LAYER_PATH,	mBratPaths.URLRasterLayerPath() ),

            k_v( ENTRY_PORTABLE_PATHS,			mBratPaths.UsePortablePaths() )
		);
}


bool CBratSettings::SaveConfig()
{
	//GROUP_COMMON already cleared by base class, clearing here
	//	would delete base entries
	//
	ClearGroup( GROUP_WKS );	
	ClearGroup( GROUP_COLORTABLE );

	return
	base_t::SaveConfig() &&
	WriteSection( GROUP_COMMON, 
	 
		k_v( ENTRY_LAST_DATA_PATH,				m_lastDataPath ),
		k_v( ENTRY_LAST_PAGE_REACHED,			m_lastPageReached ),
		k_v( ENTRY_ADVANCED_OPERATIONS,			mAdvancedOperations ),

		k_v( ENTRY_LOAD_WKSPC_AT_STARTUP,				mLoadLastWorkspaceAtStartUp ),
        k_v( ENTRY_MAIN_LAYER_BASE_TYPE,				(int)mMainLayerBaseType ),
        k_v( ENTRY_VIEWS_LAYER_BASE_TYPE,				(int)mViewsLayerBaseType ),
		k_v( ENTRY_VECTOR_SIMPLIFY_METHOD,				mVectorSimplifyMethod ),
		k_v( ENTRY_DESKTOP_MANAGER_SDI,					mDesktopManagerSdi ),
		k_v( ENTRY_CHECK_OPENGL,						mCheckOpenGL ),
		k_v( ENTRY_USE_UNSUPPORTED_FIELDS,				mUseUnsupportedFields ),
		k_v( ENTRY_DISPLAY_RADS_INSTALL_INFORMATION,	mDisplayRadsInstallInformation ),
		k_v( ENTRY_MINIMUM_FILES_TO_WARN_USER,			mMinimumFilesToWarnUser ),
		k_v( ENTRY_H_CHILD_RATIO_NUM,					mHchildRatioNum ),
		k_v( ENTRY_H_CHILD_RATIO_DNM,					mHchildRatioDnm ),
		k_v( ENTRY_PLOTS_FONT_NAME,						mPlotsFontName ),
		k_v( ENTRY_PLOTS_AXIS_FONT_SIZE,				mPlotsAxisFontSize ),
		k_v( ENTRY_PLOTS_TITLE_FONT_SIZE,				mPlotsTitleFontSize )
	)
	&&
	WriteValues( GROUP_WKS, 
	{ 
		{ ENTRY_LAST, m_lastWksPath },
	} 
	)
	&&
	WriteValues( GROUP_COLORTABLE, 
	{ 
		{ ENTRY_LAST, m_lastColorTable },
	} 
	)
#if defined(BRAT_V3)
	&&
	SaveConfigSelectionCriteria()
#endif
	&&
	SavePaths();
}


bool CBratSettings::LoadConfig()
{
	return
	base_t::LoadConfig()
	&&
	ReadSection( GROUP_COMMON, 

		k_v( ENTRY_LAST_DATA_PATH,				&m_lastDataPath ),
		k_v( ENTRY_LAST_PAGE_REACHED,			&m_lastPageReached ),
		k_v( ENTRY_ADVANCED_OPERATIONS,			&mAdvancedOperations, false ),

		k_v( ENTRY_LOAD_WKSPC_AT_STARTUP,				&mLoadLastWorkspaceAtStartUp ),
        k_v( ENTRY_MAIN_LAYER_BASE_TYPE,				(int*)&mMainLayerBaseType, (int)smDefaultLayerBaseType ),
        k_v( ENTRY_VIEWS_LAYER_BASE_TYPE,				(int*)&mViewsLayerBaseType, (int)smDefaultLayerBaseType ),
		k_v( ENTRY_VECTOR_SIMPLIFY_METHOD,				&mVectorSimplifyMethod, true ),
		k_v( ENTRY_DESKTOP_MANAGER_SDI,					&mDesktopManagerSdi, true ),
		k_v( ENTRY_CHECK_OPENGL,						&mCheckOpenGL, true ),
		k_v( ENTRY_USE_UNSUPPORTED_FIELDS,				&mUseUnsupportedFields, smUseUnsupportedFields ),
		k_v( ENTRY_DISPLAY_RADS_INSTALL_INFORMATION,	&mDisplayRadsInstallInformation ),
		k_v( ENTRY_MINIMUM_FILES_TO_WARN_USER,			&mMinimumFilesToWarnUser, smMinimumFilesToWarnUser ),
		k_v( ENTRY_H_CHILD_RATIO_NUM,					&mHchildRatioNum, smHchildRatioNum ),
		k_v( ENTRY_H_CHILD_RATIO_DNM,					&mHchildRatioDnm, smHchildRatioDnm ),
		k_v( ENTRY_PLOTS_FONT_NAME,						&mPlotsFontName, smPlotsFontName ),
		k_v( ENTRY_PLOTS_AXIS_FONT_SIZE,				&mPlotsAxisFontSize, smPlotsAxisFontSize ),
		k_v( ENTRY_PLOTS_TITLE_FONT_SIZE,				&mPlotsTitleFontSize, smPlotsTitleFontSize )
	)
	&&
	ReadValues( GROUP_WKS, 
	{ 
		{ ENTRY_LAST, m_lastWksPath },
	} 
	)
	&&
	ReadValues( GROUP_COLORTABLE, 
	{ 
		{ ENTRY_LAST, m_lastColorTable },
	}
	)
	&&
	LoadPaths();
}
