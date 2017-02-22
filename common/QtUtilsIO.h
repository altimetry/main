#ifndef BRAT_QT_UTILS_IO_H
#define BRAT_QT_UTILS_IO_H

#if defined (QT_FILE_UTILS_H)
#error Wrong QtFileUtils.h included
#endif

#include <functional>

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QTemporaryFile>
#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#endif
#include <QCoreApplication>

#include "common/ccore-types.h"
#include "common/+UtilsIO.h"		// => +Utils.h
#include "QtStringUtils.h"          // => QtUtils.h => QtUtilsIO.h => QtStringUtils.h => +Utils.h


///////////////////////////////////////////////////////////////////////////
//                      Paths Manipulation
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

inline QString GetBaseFilenameFromPath( const QString &path )
{
	return QFileInfo( t2q( path ) ).baseName();
}

inline std::string GetBaseFilenameFromPath( const std::string &path )
{
	return q2t< std::string >( GetBaseFilenameFromPath( t2q( path ) ) );
}

template< typename STRING >
inline std::string GetFilenameFromPath( const STRING &path )
{
	return q2t< std::string >( QFileInfo( t2q( path ) ).fileName() );
}

inline std::string GetLastExtensionFromPath( const std::string &path )
{
	return q2t< std::string >( QFileInfo( t2q( path ) ).suffix() );
}


template< typename STRING >
inline STRING GetDirectoryFromPath( const STRING &path )
{
	return q2t< STRING >( QFileInfo( t2q( path ) ).absoluteDir().absolutePath() );
}


template< typename STRING >
inline void SetFileExtension( STRING &path, const STRING &extension )
{
	STRING dir = GetDirectoryFromPath( path );
	STRING name = GetBaseFilenameFromPath( path );
	path = dir + "/" + name + "." + extension;
}



// Path normalization:
//
//	- converts to path with "/" separators
//	- eliminates duplicated directory separators (of any kind)
//	- resolves (eliminating) "." and ".." directories
//
inline QString NormalizedPath( const QString &path )
{
	QString qpath = path;
	//
	// Really does what qt5 documentation says it does, but doesn't:
	//
	// "Returns path with directory separators normalized (converted to "/")"
	//
	// In fact, cleanPath only does this in systems like Windows where the
	//	separator is '\\'; in systems with separator '/', the '\\' are not 
	//	replaced. Great...
	//
	return QDir::cleanPath( qpath.replace( QLatin1Char( '\\' ), QLatin1Char( '/' ) ) );
}
inline std::string NormalizedPath( const std::string &path )
{
	return q2t< std::string >( NormalizedPath( t2q( path ) ) );
}
inline QString NormalizedPath( const char *path )
{
	return NormalizedPath( QString( path ) );
}
inline std::string& NormalizePath( std::string &path )
{
	path = NormalizedPath( path );
	return path;
}


template< typename STRING >
inline std::string GetRelativePath( const STRING &ref_path, const STRING &absolute )
{
	QDir dir( t2q( ref_path ) );

	return q2t< STRING >( dir.relativeFilePath( t2q( absolute ) ) );	
}


inline std::string GetRelativePath( const char *ref_path, const char *absolute )
{
	QDir dir( t2q( ref_path ) );

	return q2a( dir.relativeFilePath( t2q( absolute ) ) );	
}


inline std::string GetAbsolutePath( const std::string &ref_dir, const std::string &relative )
{
	QDir dir( t2q( ref_dir ) );

	return NormalizedPath( q2a( dir.absoluteFilePath( t2q( relative ) ) ) );
}


inline bool IsRelative( const std::string &path );

// Assumes that ":" only appears in windows paths as a driver specifier after the drive letter
// Assumes that only one ":" can exist in a windows path
// Assumes that the driver letter is always the first character in a windows absolute path
// All the above apply to paths with wild cards
//
inline std::string win2cygwin( const std::string &win_path )
{
	//C:\WORK\* --> /cygdrive/c/work/*

	std::string path = NormalizedPath( win_path );

#if defined (Q_OS_WIN)
    if ( !IsRelative( win_path ) )
	{
		path = "/cygdrive/" + path;
		path = replace( path, ":", "" );
	}
#endif

    return path;
}





///////////////////////////////////////////////////////////////////////////
//                      Paths Querying
///////////////////////////////////////////////////////////////////////////


inline bool IsRelative( const std::string &path )
{
	return QFileInfo( t2q( path ) ).isRelative();
}


// only valid for absolute paths
//
inline bool IsSubDirectory( const std::string &path, const std::string &sub_path )
{
	return NormalizedPath( sub_path ).find( NormalizedPath( path ) ) != std::string::npos;
}


inline bool IsFile( QString path )
{
	QFileInfo file_info( path );

    return file_info.exists() && file_info.isFile();
}
inline bool QIsFile( const char *path )
{
    return IsFile( QString( path ) );
}
inline bool QIsFile( const std::string &path )
{
    return QIsFile( path.c_str() );
}

inline bool IsDir( const QString &dir_path )
{
    QDir d( dir_path );
    return !dir_path.isEmpty() && d.exists();
}
inline bool IsDir( const std::string &dir_path )
{
    return IsDir( t2q( dir_path ) );
}
inline bool IsDir( const char *dir_path )
{
    return IsDir( QString( dir_path ) );
}


inline std::string EscapePath( const std::string &s )
{
	return q2a( QDir::toNativeSeparators( s.c_str() ) );
}




///////////////////////////////////////////////////////////////////////////
//                      Paths Physical Operations
///////////////////////////////////////////////////////////////////////////

// On files...

inline bool NewFile( const std::string &NewName, bool close = false )			  				//CAUTION: There is a CreateFile in windows
{
	QFile f( NewName.c_str() );
	if ( !f.open( QIODevice::ReadWrite ) )
		return false;
	if ( close )
		f.close();

    return true;
}

inline void CloseFile( const std::string &NewName )
{
	QFile f( NewName.c_str() );
	f.close();
}

inline bool RenameFile( const std::string &OldName, const std::string &NewName )
{
    return QFile::rename( OldName.c_str(), NewName.c_str() );
}

inline bool DuplicateFile( const std::string &SourcePath, const std::string &DestinationPath )	//CAUTION: There is a CopyFile in windows
{
    return QFile::copy( SourcePath.c_str(), DestinationPath.c_str() );
}



inline bool RemoveFile( const QString &Path )													//CAUTION: There is a DeleteFile in windows
{
    return QFile::remove( Path );
}
inline bool RemoveFile( const std::string &Path )
{
    return RemoveFile( t2q( Path ) );
}

//	Creates a new file with unique path. The file has 0 bytes. It
//	will be left on disk (closed) if remove is false.
//
template< typename STRING >
inline 
STRING CreateUniqueFile_( const STRING &base_path, bool remove = false )
{
	STRING new_path;
	
	{
		QTemporaryFile f( t2q( base_path ) );
		f.setAutoRemove( remove );
		if ( f.open() )
			new_path = q2t<STRING >( f.fileName() );
	}

	assert__( !remove || !IsFile( new_path ) );

	return new_path;
}

template< typename STRING >
inline 
STRING CreateUniqueFileName( const STRING &base_path )
{
	return CreateUniqueFile_( base_path, true );
}



// On directories...

inline bool MakeDirectory( const std::string &path )
{
    QDir d;
    return d.mkpath( path.c_str() );
}


// Assumes normalized paths (which also means without trailing path separators)
//
inline bool DeleteDirectory( const QString &path )										//CAUTION: There is a RemoveDirectory in windows
{
	QFileInfo fileInfo( path );
	if ( fileInfo.isDir() )
	{
		QDir dir( path );
		QStringList fileList = dir.entryList( QDir::AllEntries | QDir::NoDotAndDotDot | QDir::System );
		for ( int i = 0; i < fileList.count(); ++i )
		{
			qDebug() << fileList.at( i );
			if ( !DeleteDirectory( path + "/" + fileList.at( i ) ) )
				return false;
		}
		if ( !dir.rmdir( path ) )
			return false;
	}
	else
	{
		return RemoveFile( path );
	}

	return true;
}
inline bool DeleteDirectory( const std::string &path )
{
	return DeleteDirectory( t2q( NormalizedPath( path ) ) );
}


//not tested
//AKA copy root: this is exactly the same thing we implemented in our python script.
inline bool DuplicateDirectory( const QString &sourceFolder, const QString &destFolder )
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
        QString src_path = sourceFolder + "/" + files[i];
        QString dest_path = destFolder + "/" + files[i];
        QFile::copy( src_path, dest_path );
    }
    files.clear();
    files = sourceDir.entryList( QDir::AllDirs | QDir::NoDotAndDotDot );
    for(int i = 0; i< files.count(); i++)
    {
        QString src_path = sourceFolder + "/" + files[i];
        QString dest_path = destFolder + "/" + files[i];
        if ( !DuplicateDirectory( src_path, dest_path ) )
            return false;
    }
    return true;
}


inline bool CopyFileProc( const QString &src_path, const QString &dest_path )
{
	return QFile::copy( src_path, dest_path );

}



#if !defined(PRE_CPP11)

// How to set filters:
//
inline bool TraverseDirectory_( const QString &sourceFolder, const QString &destFolder, const QStringList &filters, 
	const std::function< bool( const QString &, const QString & ) > &f = CopyFileProc )
{
    QDir sourceDir( sourceFolder );
    if( !sourceDir.exists() )
        return false;

    QDir destDir( destFolder );
    if( !destDir.exists() )
		destDir.mkdir( destFolder );

	{
		QStringList files = sourceDir.entryList( filters, QDir::Files );
		for ( int i = 0; i < files.count(); i++ )
		{
			QString src_path = sourceFolder + "/" + files[ i ];
			QString dest_path = destFolder + "/" + files[ i ];
			if ( !f( src_path, dest_path ) )
				return false;
		}
	}
    QStringList dirs;
    dirs = sourceDir.entryList( QDir::AllDirs | QDir::NoDotAndDotDot );
    for(int i = 0; i< dirs.count(); i++)
    {
        QString src_path = sourceFolder + "/" + dirs[i];
        QString dest_path = destFolder + "/" + dirs[i];
        if ( !TraverseDirectory_( src_path, dest_path, filters, f ) )
            return false;
    }
    return true;
}


inline bool TraverseDirectory_( const QString &sourceFolder, const QString &destFolder, std::initializer_list< QString > filters,
	const std::function< bool( const QString &, const QString & ) > &f = CopyFileProc )
{
	QStringList qfilters;
	for ( auto const &filter : filters )
		qfilters << filter;

	return TraverseDirectory_( sourceFolder, destFolder, qfilters, f );
}




template< class STRING >
inline bool TraverseDirectory( const STRING &sourceFolder, const STRING &destFolder, const QStringList &filters, 
	std::function< bool( const STRING &, const STRING & ) > f = CopyFileProc )
{
	const QString qsource_folder = t2q( sourceFolder );
	const QString qdest_folder = t2q( destFolder );

	QDir sourceDir( qsource_folder );
	if( !sourceDir.exists() )
		return false;

	if ( !qdest_folder.isEmpty() )
	{
		QDir destDir( qdest_folder );
		if( !destDir.exists() )
			destDir.mkdir( qdest_folder );
	}

	{
		QStringList files = sourceDir.entryList( filters, QDir::Files );
		for ( int i = 0; i < files.count(); i++ )
		{
			STRING src_path = sourceFolder + "/" + q2t<STRING>( files[ i ] );
			STRING dest_path = qdest_folder.isEmpty() ? x2x<STRING>( empty_string() ) : destFolder + "/" + q2t<STRING>( files[ i ] );
			if ( !f( src_path, dest_path ) )
				return false;
		}
	}
	QStringList dirs;
	dirs = sourceDir.entryList( QDir::AllDirs | QDir::NoDotAndDotDot );
	for(int i = 0; i< dirs.count(); i++)
	{
		STRING src_path = sourceFolder + "/" + q2t<STRING>( dirs[i] );
		STRING dest_path = qdest_folder.isEmpty() ? x2x<STRING>( empty_string() ) : destFolder + "/" + q2t<STRING>( dirs[ i ] );
		if ( !TraverseDirectory( src_path, dest_path, filters, f ) )
			return false;
	}
	return true;
}


template< class STRING >
inline bool TraverseDirectory( const STRING &sourceFolder, const STRING &destFolder, std::initializer_list< STRING > filters,
	std::function< bool( const STRING &, const STRING & ) > f = CopyFileProc )
{
	QStringList qfilters;
	for ( auto const &filter : filters )
		qfilters << t2q( filter );

	return TraverseDirectory( sourceFolder, destFolder, qfilters, f );
}







/*
 * Attempts to safely copy one file from one path to another: If the destination path directory does not exist
 * it will attempt to create it (if it fails will return false). If the source path does not contain a valid
 * file, it will return false. If nothing of this fails then we return DuplicateFile.
 */
inline bool SafeDuplicateFile( const std::string &SourcePath, const std::string &DestinationPath )
{
    std::string dest_dir = GetDirectoryFromPath(DestinationPath);
    if (!QIsFile(SourcePath))
    {
        return false;
    }
    if (!IsDir(dest_dir))
    {
        if (!MakeDirectory(dest_dir))
        {
            return false;
        }
    }

    return DuplicateFile(SourcePath, DestinationPath);
}

#endif





//////////////////////////////////////////////////////////////////
//						OS Identification
//////////////////////////////////////////////////////////////////

inline bool IsWindowXP()
{
	return QSysInfo::windowsVersion() == QSysInfo::WV_XP || QSysInfo::windowsVersion() == QSysInfo::WV_2003;
}



//////////////////////////////////////////////////////////////////
//						OS Standard Paths
//////////////////////////////////////////////////////////////////
//
//For Qt4, this section should be in QtUtils and not here because 
//	QDesktopServices include GUI


inline const std::string& SystemUserDocumentsPath()
{
#if QT_VERSION >= 0x050000
	static const std::string docs = q2a( QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation ) );
#else
	static const std::string docs = q2a( QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ) );
#endif

	assert__( IsDir( docs ) );

	return docs;
}

// If applicationName and/or organizationName are set, are used by the framework as sub-directories.
// If parameter root is false (the default), this function always removes the application 
//  sub-directory, if it exists; so, for different applications of the same "organization" 
//	(or of "no organization") the returned path is the same in the same machine.
//
inline std::string ComputeSystemUserSettingsPath( bool root = false )
{
#if QT_VERSION >= 0x050000

	std::string data = q2a( QStandardPaths::writableLocation( QStandardPaths::GenericDataLocation ) );
	if ( !root && !QCoreApplication::organizationName().isEmpty() )
		data += ( "/" + q2a( QCoreApplication::organizationName() ) );

#else
	std::string data = q2a( QDesktopServices::storageLocation( QDesktopServices::DataLocation ) );

	if ( !QCoreApplication::applicationName().isEmpty() )
		data = GetDirectoryFromPath( data );
	if ( root && !QCoreApplication::organizationName().isEmpty() && !QCoreApplication::applicationName().isEmpty() )
		data = GetDirectoryFromPath( data );

#endif

	return data;
}

inline const std::string& SystemUserSettingsPath()
{
	static const std::string data = ComputeSystemUserSettingsPath();		assert__( IsDir( GetDirectoryFromPath( data ) ) );
	return data;
}





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                              QtCore not IO specific
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////
//						Linux Desktop
//////////////////////////////////////////////////////////////////

inline bool IsLinuxDesktop( const std::string &desktop )
{
//Tested only in debian 7
//DESKTOP_SESSION=kde-plasma
//DESKTOP_SESSION=gnome

#if defined (Q_OS_LINUX)
    const char *DESKTOP_SESSION = getenv( "DESKTOP_SESSION" );
    return DESKTOP_SESSION && ( std::string( DESKTOP_SESSION ).find( desktop ) != std::string::npos );
#else

    UNUSED( desktop )

    return false;

#endif
}

inline bool IsGnomeDesktop()
{
    return IsLinuxDesktop( "gnome" );
}

inline bool IsKDEDesktop()
{
    return IsLinuxDesktop( "kde" );
}



#if defined (Q_OS_LINUX)

//////////////////////////////////////////////////////////////////
//				Linux Execute Command As Root
//////////////////////////////////////////////////////////////////

inline bool ExecuteCommand( bool root, const std::string &script, const std::string &arguments = empty_string() )
{
    static const std::string full_kdesu = "/usr/lib/kde4/libexec/kdesu";
    static const bool is_full_kdesu = IsFile( full_kdesu );
    static const std::string root_cmd = is_full_kdesu ? full_kdesu : ( IsGnomeDesktop() ? "gksu" : "kdesu" );
    static const std::string root_args = IsGnomeDesktop() ? "-u root" : "-u root -c";

    // If root, script is treated as an arg
    //
    const std::string cmd = root ? root_cmd : script;
    std::string args;
    if ( root )
        args = ( root_args + " " + script );
    if ( !arguments.empty() )
        args += ( " "  + arguments );

    return system( ( cmd + " " + args ).c_str() ) == 0;
}

#endif






#endif		//BRAT_QT_UTILS_IO_H
