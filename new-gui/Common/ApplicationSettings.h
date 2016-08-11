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
#if !defined COMMON_APPLICATION_SETTINGS_H
#define COMMON_APPLICATION_SETTINGS_H

#include "new-gui/Common/FileSettings.h"


///////////////////////////////////////////////////
//	Extract and insert values in QVariant
///////////////////////////////////////////////////

// Extract: QVariant => Type

//DO NOT DELETE: example
//
//template<>
//inline EApplicationStyleSheets qv2type( const QVariant &v )
//{
//	return static_cast< EApplicationStyleSheets >( v.toInt() );
//}



// Insert: Type => QVariant

// See FileSettings.h




class CApplicationSettings : public CFileSettings
{
    //////////////////////////////////////
    //	types & friends
    //////////////////////////////////////

    using base_t = CFileSettings;


    //////////////////////////////////////
    //		static members
    //////////////////////////////////////


public:
	
	static const std::vector< std::string >& getStyles();

	static size_t getStyleIndex( const QString &qname );

	static std::string getNameOfStyle( const QStyle *s );

	static std::string getNameOfStyle( QStyle *s, bool del );


protected:

    //////////////////////////////////////
    //	data
    //////////////////////////////////////

    std::string mOrgName;
    std::string mExecName;

public:
    std::string m_userManual;
    std::string m_userManualViewer;

	QString mAppStyle;
	bool mUseDefaultStyle = false;


    //////////////////////////////////////
    //	construction / destruction
    //////////////////////////////////////

protected:

    // Settings of type I - For exclusive use of application-wide settings
    //
    // For Application type initialization; static and non-static read/write functions can access (THE) instance type

    CApplicationSettings( const std::string &org_name, const std::string &exec_name, const std::string &domain_name = "" ) :
        base_t( org_name, exec_name, domain_name )
        , mOrgName( org_name )
        , mExecName( exec_name )
    {
        qDebug() << mSettings.format();
        qDebug() << mSettings.fileName();
			//if ( !IsFile( new_path ) )
   //     return write_version_signature();

    }

public:
	// Settings of type II - used by parameter files
	//
	// Module type instance; static read/write functions should not be used with this instance type

    CApplicationSettings( const std::string &path ) :
		base_t( path )
	{}


    virtual ~CApplicationSettings()
    {}

    //////////////////////////////////////
    //	access
    //////////////////////////////////////


protected:
    //////////////////////////////////////
    //	access - privileged
    //////////////////////////////////////

	bool setApplicationStyle( QApplication &app, QString default_style );

public:

    //////////////////////////////////////
    //	operations
    //////////////////////////////////////

    virtual bool LoadConfig();
    virtual bool SaveConfig();

};








///////////////////////////////////////////////////
//	Global (Application scope) Settings Interface 
///////////////////////////////////////////////////


class CAppSection : public CFileSettings::CSection
{
    using base_t = CFileSettings::CSection;

	static QSettings& AppSettings()
	{
		static QSettings s;
		return s;
	}

public:
	CAppSection( const std::string &group )
		: base_t( AppSettings(), group ) 
	{}
	virtual ~CAppSection()
	{}
};



inline QSettings::Status AppSettingsStatus()
{ 
	QSettings settings;
	return settings.status(); 
}


inline void AppSettingsSync()
{ 
	QSettings settings;
	settings.sync(); 
}


inline void AppSettingsClearGroup( CAppSection &section )
{
	section.Settings().remove( QString() );
}



// Generic Read / Write functions

//... for single values

template< class VALUE = std::string >
inline VALUE AppSettingsReadValue( CAppSection &section, const std::string &key )
{
    return CFileSettings::ReadValue< VALUE >( section, key );
}


template< class VALUE >
inline void AppSettingsWriteValue( CAppSection &section, const std::string &key, const VALUE &value )
{
    return CFileSettings::WriteValue( section, key, value );
}


//... for sections with the same value types
//		- assume respective group (section) already open by creating a CAppSection object

template< typename T >
inline bool AppSettingsReadValues( const std::string &group, std::initializer_list< kv_t< T& > > kv_pairs )
{
	QSettings settings;
    return CFileSettings::ReadValues( settings, group, kv_pairs );
}


template< typename T >
inline bool AppSettingsWriteValues( const std::string &group, std::initializer_list< kv_t< T > > kv_pairs )
{
	QSettings settings;
    return CFileSettings::WriteValues( settings, group, kv_pairs );
}

//template< typename T >
//bool AppSettingsApplyToWholeSection( const std::string &group, std::function< bool( const std::string &key, const T &value ) > &f )
template< typename T, typename F >
bool AppSettingsApplyToWholeSection( const std::string &group, const F &f )
{
	QSettings settings;
    return CFileSettings::ApplyToWholeSection( settings, group, f );
}

//... for sections, independently of values types
//		- assume respective group (section) already open by creating a CAppSection object

template< typename... Types >
inline bool AppSettingsReadSection( const std::string &group, Types... kv_pairs )
{
	QSettings settings;
    return CFileSettings::ReadSection( settings, group, kv_pairs... );
}


template< typename... Types >
inline bool AppSettingsWriteSection( const std::string &group, const Types&... kv_pairs )
{
	QSettings settings;
    return CFileSettings::WriteSection( settings, group, kv_pairs... );
}




#endif	//COMMON_APPLICATION_SETTINGS_H
