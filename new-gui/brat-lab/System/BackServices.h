#ifndef QtOutputServer_H
#define QtOutputServer_H

#include <QtGui>

#include "../display/Libraries/+/QtUtils.h"


////////////////////////////////////////////////////////////////
//
//                      Background Services
//
////////////////////////////////////////////////////////////////


class TBackServices
{

    static std::string m_SettingsPath;
    static std::string m_DataPath;

public:
    TBackServices()
    {
        RestoreSettingsFile();
        QSettings::setPath( QSettings::IniFormat, QSettings::UserScope, GetSettingsPath() );
        ReadSettings();
    }

    ~TBackServices()
    {
        WriteSettings();
        BackupSettingsFile();
    }

    //////////////////////////////
    //  Application Settings
    //////////////////////////////

    static const char* GetAppName()
    {
        static const char *AppName = "AppMDI";
        return AppName;
    }

    static const char* GetOrgName()
    {
        static const char *OrgName = "UNIX_Inc.";
        return OrgName;
    }

    static const char* DefaultSettingsPath()
    {
#if defined(_WIN32)
        static const char *SettingsPath = "P:/data/common/include/Sheets/QtAppTemplate/AppMDI/System";
#else
        static const char *SettingsPath = "/home/femm/.AppMDI";
#endif
        return SettingsPath;
    }

    static const char* DefaultDataPath()
    {
#if defined(_WIN32)
        static const char *DataPath = "P:/dev/common/include/Sheets/QtAppTemplate/AppMDI";
#else
        static const char *DataPath = "/home/femm/dev/share/dev/common/include/Sheets/QtAppTemplate/AppMDI";
#endif
        return DataPath;
    }

    static const char* GetSettingsPath()
    {
        if ( m_SettingsPath.empty() )
            SetSettingsPath();
        return m_SettingsPath.c_str();
    }

    static const char* GetDataPath()
    {
        if ( m_DataPath.empty() )
            SetDataPath();
        return m_DataPath.c_str();
    }

    static bool SetSettingsPath( const char* NewSettingsPath = NULL )
    {
        if ( !NewSettingsPath || !IsDir( NewSettingsPath ) )
        {
            if ( m_SettingsPath.empty() || !IsDir( m_SettingsPath.c_str() ) )    //preserve if good, try the default if not
                m_SettingsPath = DefaultSettingsPath();
            return false;   //assignment request refused
        }
        else {
            m_SettingsPath = NewSettingsPath;
            return true;
        }
    }

    static bool SetDataPath( const char* NewDataPath = NULL )
    {
        if ( !NewDataPath || !IsDir( NewDataPath ) )
        {
            if ( m_DataPath.empty() || !IsDir( m_DataPath.c_str() ) )    //preserve if good, try the default if not
                m_DataPath = DefaultDataPath();
            return false;   //assignment request refused
        }
        else {
            m_DataPath = NewDataPath;
            return true;
        }
    }

    static const char* GetBackupSettingsPath()
    {
#if defined(_WIN32)
        static const char *SettingsDir = "P:/data/common/include/Sheets/QtAppTemplate/AppMDI/res";
#else
        static const char *SettingsDir = "/home/femm/dev/share/dev/common/include/Sheets/QtAppTemplate/AppMDI/res";
#endif
        return SettingsDir;
    }

    static const std::string& GetAppSettingsFile( bool backup )
    {
        static const std::string settings_path = std::string( GetSettingsPath() ) + "/" + GetOrgName() + "/" + GetAppName() + ".ini";
        static const std::string backup_settings_path = std::string( GetBackupSettingsPath() ) + "/" + GetOrgName() + "/" + GetAppName() + ".ini";
        return backup ? backup_settings_path : settings_path;
    }

protected:
    bool RestoreSettingsFile();

    bool BackupSettingsFile();


public:
    QSettings& GetAppSettings()
    {
        static QSettings settings( QSettings::IniFormat, QSettings::UserScope, GetOrgName(), GetAppName() );
        return settings;
    }


    void WriteSettings();

    void ReadSettings();
};



TBackServices& GetBackServices();


#endif // QtOutputServer_H
