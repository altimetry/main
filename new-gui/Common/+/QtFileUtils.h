#ifndef BRAT_QT_FILE_UTILS_H
#define BRAT_QT_FILE_UTILS_H

#if defined (QT_FILE_UTILS_H)
#error Wrong QtFileUtils.h included
#endif

#include "QtStringUtils.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>


///////////////////////////////////////////////////////////////////////////
//                      File System Utilities
///////////////////////////////////////////////////////////////////////////


inline QString& switchExtension( QString &fullPath, const QString &ext_a, const QString &ext_b )	  //extensions without dot
{
    QFileInfo info( fullPath );
    QString new_path = info.absolutePath() + "/" + info.baseName() + ".";
    if ( info.suffix() == ext_a )
        new_path += ext_b;
    else
    if ( info.suffix() == ext_b )
        new_path += ext_a;
    else
        new_path += info.suffix();	//the idea is to restore the original

    fullPath = new_path;
    return fullPath;
}

inline std::string getFilenameFromPath( const std::string &path )
{
	return q2t< std::string >( QFileInfo( t2q( path ) ).fileName() );
}

inline std::string getDirectoryFromPath( const std::string &path )
{
	return q2t< std::string >( QFileInfo( t2q( path ) ).absoluteDir().absolutePath() );
}

inline bool IsFile( const QString &FileName )
{
    return QFile::exists( FileName );
}

inline bool IsFile( const char *FileName )
{
    return QFile::exists( FileName );
}

inline bool IsFile( const std::string &FileName )
{
    return IsFile( FileName.c_str() );
}

inline bool IsDir( const std::string &DirName )
{
    QDir d( DirName.c_str() );
    return d.exists();
}

inline bool MakeDirectory( const std::string &Path )
{
    QDir d;
    return d.mkpath( Path.c_str() );
}

//not tested
//
inline bool CopyDirectory( const QString &sourceFolder, const QString &destFolder )
{
    QDir sourceDir( sourceFolder );
    if( !sourceDir.exists() )
        return false;

    QDir destDir( destFolder );
    if( !destDir.exists() )
        destDir.mkdir( destFolder );

    QStringList files = sourceDir.entryList( QDir::Files );
    for(int i = 0; i< files.count(); i++)
    {
        QString srcName = sourceFolder + "/" + files[i];
        QString destName = destFolder + "/" + files[i];
        QFile::copy( srcName, destName );
    }
    files.clear();
    files = sourceDir.entryList( QDir::AllDirs | QDir::NoDotAndDotDot );
    for(int i = 0; i< files.count(); i++)
    {
        QString srcName = sourceFolder + "/" + files[i];
        QString destName = destFolder + "/" + files[i];
        if ( !CopyDirectory( srcName, destName ) )
            return false;
    }
    return true;
}

inline bool RenameFile( const std::string &OldName, const std::string &NewName )
{
    return QFile::rename( OldName.c_str(), NewName.c_str() );
}

inline bool copyFile( std::string &SourcePath, std::string &DestinationPath )	//CAUTION: There is a CopyFile in windows
{
    return QFile::copy( SourcePath.c_str(), DestinationPath.c_str() );
}

inline bool deleteFile( std::string &Path )										//CAUTION: There is a DeleteFile in windows
{
    return QFile::remove( Path.c_str() );
}






#endif		//BRAT_QT_FILE_UTILS_H
