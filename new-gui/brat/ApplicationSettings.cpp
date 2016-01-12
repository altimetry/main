#include "stdafx.h"

#include "libbrathl/Product.h"
#include "new-gui/Common/+UtilsIO.h"
#include "new-gui/Common/QtUtilsIO.h"
#include "new-gui/brat/Workspaces/Workspace.h"

#include "ApplicationSettings.h"


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





////////////////////////////////////////////////////////////////////////////////////////////
//
//									CApplicationSettings
//
////////////////////////////////////////////////////////////////////////////////////////////


bool CApplicationSettings::SaveConfigSelectionCriteria()
{
	CMapProduct& mapProductInstance = CMapProduct::GetInstance();
	for ( CObMap::iterator it = mapProductInstance.begin(); it != mapProductInstance.end(); it++ )
	{
		CProduct* product = dynamic_cast<CProduct*>( it->second );
		if ( product == nullptr || !product->HasCriteriaInfo() )
			continue;

		std::string configPath = BuildComposedKey( { GROUP_SEL_CRITERIA, product->GetLabel() } );		
		{
			CSection section( mSettings, configPath );
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



bool CApplicationSettings::LoadConfigSelectionCriteria()
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

            std::string configPath = BuildComposedKey( { GROUP_SEL_CRITERIA, product->GetLabel() } );
			{
				CSection section( mSettings, configPath );
				QStringList keys = mSettings.childKeys();
				if ( keys.empty() )
					continue;			qDebug() << keys;

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
		std::string msg( "An error occured while loading criteria selection configuration (CBratGuiApp::LoadConfigSelectionCriteria)\nNative exception: " );
		msg += e.what();
		throw CException( msg, e.error() );
	}

	return mSettings.status() == QSettings::NoError;
}



bool CApplicationSettings::SaveConfig()
{
	return
	WriteVersionSignature() &&
	WriteSection( GROUP_COMMON, 
	 
		k_v( ENTRY_USER_MANUAL,				m_userManual ),
		k_v( ENTRY_USER_MANUAL_VIEWER,		m_userManualViewer ),
		k_v( ENTRY_LAST_DATA_PATH,			m_lastDataPath ),
		k_v( ENTRY_LAST_PAGE_REACHED,		m_lastPageReached ),
		k_v( ENTRY_ADVANCED_OPERATIONS,		mAdvancedOperations ),

		k_v( ENTRY_LOAD_WKSPC_AT_STARTUP,	mLoadLastWorkspaceAtStartUp ),
		k_v( ENTRY_APP_STYLE,				mAppStyle ),
		k_v( ENTRY_APPLICATION_STYLESHEETS, mCustomAppStyleSheet ),
		k_v( ENTRY_USE_DEFAULT_STYLE,		mUseDefaultStyle ),
		k_v( ENTRY_CUSTOM_STYLESHEET,		mCustomAppStyleSheet ),
		k_v( ENTRY_NO_STYLESHEET,			mNoStyleSheet )
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
	SaveConfigSelectionCriteria();
}


bool CApplicationSettings::LoadConfig()
{
	return
	( mSettings.status() == QSettings::NoError )	//just in case check; LoadConfig is typically called only once and before any other settings use
	&&
	ReadSection( GROUP_COMMON, 

		k_v( ENTRY_USER_MANUAL,				&m_userManual ),
		k_v( ENTRY_USER_MANUAL_VIEWER,		&m_userManualViewer ),
		k_v( ENTRY_LAST_DATA_PATH,			&m_lastDataPath ),
		k_v( ENTRY_LAST_PAGE_REACHED,		&m_lastPageReached ),
		k_v( ENTRY_ADVANCED_OPERATIONS,		&mAdvancedOperations ),

		k_v( ENTRY_LOAD_WKSPC_AT_STARTUP,	&mLoadLastWorkspaceAtStartUp ),
		k_v( ENTRY_APP_STYLE,				&mAppStyle ),
		k_v( ENTRY_APPLICATION_STYLESHEETS, &mCustomAppStyleSheet ),
		k_v( ENTRY_USE_DEFAULT_STYLE,		&mUseDefaultStyle ),
		k_v( ENTRY_CUSTOM_STYLESHEET,		&mCustomAppStyleSheet ),
		k_v( ENTRY_NO_STYLESHEET,			&mNoStyleSheet )
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
	LoadConfigSelectionCriteria();
}





//////////////////////////////////////////////////////////////////////////////////////////
//								Application Styles
//////////////////////////////////////////////////////////////////////////////////////////


//static
std::string CApplicationSettings::getNameOfStyle( const QStyle *s )
{
    return q2t<std::string>( s->objectName().toLower() );
}

//static
std::string CApplicationSettings::getNameOfStyle( QStyle *s, bool del )
{
    std::string result = getNameOfStyle( s );
    if ( del )
        delete s;
    return result;
}




static const std::vector< std::string >& buildStyleNamesList()
{
	static std::vector< std::string > styles;
	if ( styles.empty() )
	{
		QStringList l = QStyleFactory::keys();
		for ( int i = 0; i < l.size(); ++i )
		{
			const std::string key = q2t<std::string>( QString( l.at( i ) ).toLower() );
			styles.push_back( key );
			if ( 0 )
			{
                assert__( CApplicationSettings::getNameOfStyle( QStyleFactory::create( key.c_str() ), true ) == key );
			}
		}
	}
	return styles;
}

//static 
const std::vector< std::string >& CApplicationSettings::getStyles()
{
	static std::vector< std::string > styles = buildStyleNamesList();
	return styles;
}


//static 
size_t CApplicationSettings::getStyleIndex( const QString &qname )
{
	const std::vector< std::string > &v = getStyles();
	const size_t size = v.size();
	const std::string name = q2t<std::string>( qname );
	for ( size_t i = 0; i < size; ++i )
		if ( name == v[i] )
			return i;

	return (size_t)-1;
}


//static 
const std::vector< std::string >& CApplicationSettings::getStyleSheets( bool resources )
{
	static const std::vector< std::string > style_sheets =
	{
		"DarkStyle",
		"DarkOrangeStyle",
		"Dark-2015",
	};

	static const std::vector< std::string > res =
	{
		":/StyleSheets/StyleSheets/QTDark.stylesheet",						//with recommended style QCleanlooksStyle
		":/StyleSheets/StyleSheets/DarkOrange/darkorange.stylesheet",		//with recommended style QPlastiqueStyle
		":/StyleSheets/StyleSheets/Dark-2015/Dark-2015.stylesheet",		//with recommended style QPlastiqueStyle ?
	};

	assert__( EApplicationStylesSheets_size == style_sheets.size() );
	assert__( EApplicationStylesSheets_size == res.size() );

	return resources ? res : style_sheets;
}

#include "new-gui/brat/BratApplication.h"

bool CApplicationSettings::setApplicationStyle( CBratApplication &app, QString default_style )
{
	//validate options
	 
	if ( getStyleIndex( mAppStyle ) == (size_t)-1 )
		mAppStyle = default_style;

	//if ( mCustomAppStyleSheet < 0 || mCustomAppStyleSheet >= getStyleSheets( true ).size() )
	//	mCustomAppStyleSheet = smDefaultCustomAppStyleSheet;

	//use options

	const QString &name = mUseDefaultStyle ? default_style : mAppStyle;					assert__( !name.isEmpty() );
	QStyle *style = QStyleFactory::create( name );										assert__( !style->objectName().toLower().compare( name ) );
	CBratApplication::setStyle( style );												//assert__( !getCurrentStyleName().compare( name ) );
	mAppStyle = name;					//update options style field with actually used style

	//if ( mNoStyleSheet )
	//	app.setStyleSheet( nullptr );	//this is necessary because without this the "old" sheet continues to be used and the style assignment above is not enough
	//else
	//{
 //       QString resource = t2q( getStyleSheets( true )[mCustomAppStyleSheet] );
 //       QFile styleFile( resource );
 //       if ( !styleFile.open( QFile::ReadOnly ) )
 //       {
 //           if ( &options == &getAppOptions() )
 //           {
 //               setStyleSheet( nullptr );
 //               mNoStyleSheet = true;
 //           }
 //           else
 //               mCustomAppStyleSheet = getAppOptions().m_CustomAppStyleSheet;
 //           return false;
 //       }
 //       QString sheet( styleFile.readAll() );
 //       app.setStyleSheet( sheet );		//apparently this changes the style name for an empty string
	//}
	return true;
}




//////////////////////////////////////////////////////////////////////////////////////////
//	TODO: delete: only for brat v3 support to v4 development
//////////////////////////////////////////////////////////////////////////////////////////

template< class CONTAINER >
bool CApplicationSettings::SaveRecentFilesGeneric( const CONTAINER &paths )
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


bool CApplicationSettings::SaveRecentFiles( const std::vector<std::string> &paths )
{
	return SaveRecentFilesGeneric( paths );
}
bool CApplicationSettings::SaveRecentFiles( const QStringList &paths )
{
	return SaveRecentFilesGeneric( paths );
}


template< class CONTAINER >
bool CApplicationSettings::LoadRecentFilesGeneric( CONTAINER &paths )
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

bool CApplicationSettings::LoadRecentFiles( std::vector<std::string> &paths )
{
	return LoadRecentFilesGeneric( paths );
}
bool CApplicationSettings::LoadRecentFiles( QStringList &paths )
{
	return LoadRecentFilesGeneric( paths );
}
