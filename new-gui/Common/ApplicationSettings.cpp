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
// #include "stdafx.h"

// While we can't include stdafx.h, this pragma must be here
//
#if defined (__APPLE__)
#pragma clang diagnostic ignored "-Wdeprecated-register"
#endif

#include <QStyleFactory>

#include "new-gui/Common/+UtilsIO.h"
#include "new-gui/Common/QtUtils.h"

#include "ApplicationSettings.h"


////////////////////////////////////////////////////////////////////////////////////////////
//
//									CApplicationSettings
//
////////////////////////////////////////////////////////////////////////////////////////////


bool CApplicationSettings::SaveConfig()
{
	return
	WriteVersionSignature() &&
	WriteSection( GROUP_COMMON, 
	 
		k_v( ENTRY_USER_MANUAL,				m_userManual ),
		k_v( ENTRY_USER_MANUAL_VIEWER,		m_userManualViewer ),

		k_v( ENTRY_APP_STYLE,				mAppStyle ),
		k_v( ENTRY_USE_DEFAULT_STYLE,		mUseDefaultStyle )
	);
}


bool CApplicationSettings::LoadConfig()
{
	return
	( mSettings.status() == QSettings::NoError )	//just in case check; LoadConfig is typically called only once and before any other settings use
	&&
	ReadSection( GROUP_COMMON, 

		k_v( ENTRY_USER_MANUAL,				&m_userManual ),
		k_v( ENTRY_USER_MANUAL_VIEWER,		&m_userManualViewer ),

		k_v( ENTRY_APP_STYLE,				&mAppStyle ),
		k_v( ENTRY_USE_DEFAULT_STYLE,		&mUseDefaultStyle )
	);
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


bool CApplicationSettings::setApplicationStyle( QApplication &app, QString default_style )
{
    Q_UNUSED( app );

	//validate options
	 
	if ( getStyleIndex( mAppStyle ) == (size_t)-1 )
		mAppStyle = default_style;

	//use options

	const QString &name = mUseDefaultStyle ? default_style : mAppStyle;					assert__( !name.isEmpty() );
	QStyle *style = QStyleFactory::create( name );										assert__( !style->objectName().toLower().compare( name ) );

	QApplication::setStyle( style );							// assert__( !getCurrentStyleName().compare( name ) );
	mAppStyle = name;											// update options style field with actually used style

	return true;
}
