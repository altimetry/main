#include "stdafx.h"

#include "libbrathl/Product.h"
#include "new-gui/Common/+UtilsIO.h"
#include "new-gui/Common/QtUtilsIO.h"
#include "DataModels/Workspaces/Workspace.h"

#include "BratSettings.h"


const std::string GROUP_PATHS =					"ApplicationPaths";

const std::string ENTRY_USER_BASE_PATH =		"user_base_path";
const std::string ENTRY_EXTERNAL_DATA_DIR =		"external_data_dir";
const std::string ENTRY_RASTER_LAYER_PATH =		"raster_layer_path";
const std::string ENTRY_URL_RASTER_LAYER_PATH =	"url_raster_layer_path";
const std::string ENTRY_WORKSPACES_DIR =		"workspaces_dir";
const std::string ENTRY_PORTABLE_PATHS =		"portable_paths";
const std::string ENTRY_DESKTOP_MANAGER_SDI =	"desktop_manager_sdi";



////////////////////////////////////////////////////////////////////////////////////////////
//
//									CBratSettings
//
////////////////////////////////////////////////////////////////////////////////////////////


//static 
const CMapWidget::ELayerBaseType CBratSettings::smDefaultLayerBaseType = CMapWidget::ELayerBaseType::eRasterLayer;



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


bool CBratSettings::LoadPaths()
{
	return
		ReadSection( GROUP_PATHS,

            k_v( ENTRY_USER_BASE_PATH,			&mBratPaths.mPortableBasePath ),
            k_v( ENTRY_URL_RASTER_LAYER_PATH,   &mBratPaths.mURLRasterLayerPath, mBratPaths.smDefaultURLRasterLayerPath ),
            k_v( ENTRY_WORKSPACES_DIR,			&mBratPaths.mWorkspacesDir ),

            k_v( ENTRY_PORTABLE_PATHS,	&mBratPaths.mUsePortablePaths )
		)
		&&
        mBratPaths.SetUserPaths();
}

bool CBratSettings::SavePaths()
{
	return 
		WriteSection( GROUP_PATHS,

            k_v( ENTRY_USER_BASE_PATH,          mBratPaths.mPortableBasePath ),
            k_v( ENTRY_URL_RASTER_LAYER_PATH,	mBratPaths.mURLRasterLayerPath ),
            k_v( ENTRY_WORKSPACES_DIR,          mBratPaths.mWorkspacesDir ),

            k_v( ENTRY_PORTABLE_PATHS,	mBratPaths.mUsePortablePaths )
		);
}


bool CBratSettings::SaveConfig()
{
	return
	base_t::SaveConfig() &&
	WriteSection( GROUP_COMMON, 
	 
		k_v( ENTRY_LAST_DATA_PATH,			m_lastDataPath ),
		k_v( ENTRY_LAST_PAGE_REACHED,		m_lastPageReached ),
		k_v( ENTRY_ADVANCED_OPERATIONS,		mAdvancedOperations ),

		k_v( ENTRY_LOAD_WKSPC_AT_STARTUP,	mLoadLastWorkspaceAtStartUp ),
        k_v( ENTRY_LAYER_BASE_TYPE,         (int)mLayerBaseType ),
		k_v( ENTRY_DESKTOP_MANAGER_SDI,		mDesktopManagerSdi )
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
	&&
	SaveConfigSelectionCriteria()
	&&
	SavePaths();
}


bool CBratSettings::LoadConfig()
{
	return
	base_t::LoadConfig()
	&&
	ReadSection( GROUP_COMMON, 

		k_v( ENTRY_LAST_DATA_PATH,			&m_lastDataPath ),
		k_v( ENTRY_LAST_PAGE_REACHED,		&m_lastPageReached ),
		k_v( ENTRY_ADVANCED_OPERATIONS,		&mAdvancedOperations, false ),

		k_v( ENTRY_LOAD_WKSPC_AT_STARTUP,	&mLoadLastWorkspaceAtStartUp ),
        k_v( ENTRY_LAYER_BASE_TYPE,         (int*)&mLayerBaseType, (int)smDefaultLayerBaseType ),
		k_v( ENTRY_DESKTOP_MANAGER_SDI,		&mDesktopManagerSdi, true )
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




//////////////////////////////////////////////////////////////////////////////////////////
//	TODO: delete: only for brat v3 support to v4 development
//////////////////////////////////////////////////////////////////////////////////////////

template< class CONTAINER >
bool CBratSettings::SaveRecentFilesGeneric( const CONTAINER &paths )
{
	auto brathlFmtEntryRecentWksMacro = []( size_t index ) -> std::string
	{
		return KEY_WKS_RECENT + n2s<std::string>( index );
	};

	//

	{
		CSection section( mSettings, GROUP_WKS_RECENT );
		const size_t size = paths.size();
		for ( size_t i = 0; i < size; ++i )
			WriteValue( section, brathlFmtEntryRecentWksMacro( i ), paths[ (typename CONTAINER::size_type) i ] );
	}

	return mSettings.status() == QSettings::NoError;
}


bool CBratSettings::SaveRecentFiles( const std::vector<std::string> &paths )
{
	return SaveRecentFilesGeneric( paths );
}
bool CBratSettings::SaveRecentFiles( const QStringList &paths )
{
	return SaveRecentFilesGeneric( paths );
}


template< class CONTAINER >
bool CBratSettings::LoadRecentFilesGeneric( CONTAINER &paths )
{
	long maxEntries = 0;

	{
		CSection section( mSettings, GROUP_WKS_RECENT );
		auto const &keys = mSettings.childKeys();
        maxEntries = keys.size();
		for ( auto const &key : keys )
		{
			auto value = ReadValue< typename CONTAINER::value_type >( section, q2t< std::string >( key ) );
			paths.push_back( value );
		};
	}

	assert__( paths.size() == maxEntries );			Q_UNUSED( maxEntries );		//for release builds

	return mSettings.status() == QSettings::NoError;
}

bool CBratSettings::LoadRecentFiles( std::vector<std::string> &paths )
{
	return LoadRecentFilesGeneric( paths );
}
bool CBratSettings::LoadRecentFiles( QStringList &paths )
{
	return LoadRecentFilesGeneric( paths );
}
