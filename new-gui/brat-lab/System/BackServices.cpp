#include "../stdafx.h"

#include <cassert>

#include "BackServices.h"


////////////////////////////////////////////////////////////////
//
//                      Background Services
//
////////////////////////////////////////////////////////////////

std::string TBackServices::m_SettingsPath;
std::string TBackServices::m_DataPath;


bool TBackServices::RestoreSettingsFile()
{
    std::string backup_path = GetAppSettingsFile( true );
    std::string path = GetAppSettingsFile( false );
    if ( IsFile( backup_path.c_str() ) )
    {
        if ( IsFile( path.c_str() ) )
            RenameFile( path, path + ".bak" );
        else
            MakeDirectory( std::string( GetSettingsPath() ) + "/" + GetOrgName() );
        return copyFile( backup_path, path );
    }
    return true;
}

bool TBackServices::BackupSettingsFile()
{
    GetAppSettings().sync();    //essential! Because the settings are static, that is, destroyed after the call to this function in the destructor
    std::string backup_path = GetAppSettingsFile( true );
    std::string path = GetAppSettingsFile( false );
    if ( IsFile( path.c_str() ) )
    {
        if ( IsFile( backup_path.c_str() ) )
            RenameFile( backup_path, backup_path + ".bak" );
        return copyFile( path, backup_path );
    }
    return false;
}

void TBackServices::WriteSettings()
{
    QSettings& settings = GetAppSettings();

//    TCommandsTable::StoreCommands( settings );
//    TEnvironmentsTable::StoreEnvironments( settings );
    settings.setValue( "SettingsPath", m_SettingsPath.c_str() );
}

void TBackServices::ReadSettings()
{
    QSettings& settings = GetAppSettings();

//    TCommandsTable::LoadCommands( settings );
//    TEnvironmentsTable::LoadEnvironments( settings );
//    m_start_output_server = settings.value( "start_output_server", true ).toBool();
    m_SettingsPath = settings.value( "SettingsPath", QString( DefaultSettingsPath() ) ).toString().toStdString();
}


TBackServices& GetBackServices()
{
    static TBackServices BS;
    return BS;
}
