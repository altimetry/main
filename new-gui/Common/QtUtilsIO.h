#ifndef BRAT_QT_UTILS_IO_H
#define BRAT_QT_UTILS_IO_H

#if defined (QT_FILE_UTILS_H)
#error Wrong QtFileUtils.h included
#endif

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QTemporaryFile>

#include "QtUtils.h"	// => QtUtils.h => QtStringUtils.h => +Utils.h


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

inline std::string GetBaseFilenameFromPath( const std::string &path )
{
	return q2t< std::string >( QFileInfo( t2q( path ) ).baseName() );
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
inline std::string GetDirectoryFromPath( const STRING &path )
{
	return q2t< std::string >( QFileInfo( t2q( path ) ).absoluteDir().absolutePath() );
}


inline QString NormalizedPath( const QString &path )
{
	QDir d( path );
	return d.cleanPath( path );
}
inline std::string NormalizedPath( const std::string &path )
{
	return q2t< std::string >( NormalizedPath( t2q( path ) ) );
}
inline std::string& NormalizePath( std::string &path )
{
	path = NormalizedPath( path );
	return path;
}


template< typename STRING >
inline std::string GetRelativePath( const STRING &ref_path, const STRING &path )
{
	QDir dir( t2q( ref_path ) );
	return q2t< STRING >( dir.relativeFilePath( t2q( path ) ) );	
}




///////////////////////////////////////////////////////////////////////////
//                      Paths Querying
///////////////////////////////////////////////////////////////////////////



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

inline bool MakeDirectory( const std::string &Path )
{
    QDir d;
    return d.mkpath( Path.c_str() );
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
//
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
        if ( !DuplicateDirectory( srcName, destName ) )
            return false;
    }
    return true;
}





///////////////////////////////////////////////////////////////////////////
//                      File System GUI Utilities
///////////////////////////////////////////////////////////////////////////


inline QString BrowseDirectory( QWidget *parent, const char *title, QString InitialDir )
{
    static QString lastDir = InitialDir;

    if ( InitialDir.isEmpty() )
        InitialDir = lastDir;
    QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
    //if (!native->isChecked())
    options |= QFileDialog::DontUseNativeDialog;
    QString dir = QFileDialog::getExistingDirectory( parent, QObject::tr( title ), InitialDir, options );
    if ( !dir.isEmpty() )
        lastDir = dir;
    return dir;
}

inline QStringList getOpenFileNames( QWidget * parent = 0, const QString & caption = QString(),
                              const QString & dir = QString(), const QString & filter = QString(),
                              QString * selectedFilter = 0, QFileDialog::Options options = 0 )
{
    return QFileDialog::getOpenFileNames( parent, caption, dir, filter, selectedFilter, options );
}

inline QStringList getOpenFileName( QWidget * parent = 0, const QString & caption = QString(),
                             const QString & dir = QString(), const QString & filter = QString(),
                             QString * selectedFilter = 0, QFileDialog::Options options = 0 )
{
    QStringList result;
    result.append( QFileDialog::getOpenFileName( parent, caption, dir, filter, selectedFilter, options ) );
    return result;
}

template< typename F >
QStringList tBrowseFile( F f, QWidget *parent, const char *title, QString Initial )
{
    QFileDialog::Options options;
    //if (!native->isChecked())
    //    options |= QFileDialog::DontUseNativeDialog;
    QString selectedFilter;
    return f( parent, QObject::tr( title ), Initial, QObject::tr("All Files (*);;Text Files (*.txt)"), &selectedFilter, options);
}

inline QString BrowseFile( QWidget *parent, const char *title, QString Initial )
{
    return tBrowseFile( getOpenFileName, parent, title, Initial )[0];
}

inline QStringList BrowseFiles( QWidget *parent, const char *title, QString Initial )
{
    return tBrowseFile( getOpenFileNames, parent, title, Initial );
}



///////////////////////////////////////////////////////////////////////////
// Resources "file system" utilities
///////////////////////////////////////////////////////////////////////////


inline bool readFileFromResource( const QString &rpath, QString &dest, bool unicode = false )
{
    QResource r( rpath );
    if ( !r.isValid() )
        return false;
    QByteArray ba( reinterpret_cast< const char* >( r.data() ), (int)r.size() );
    QByteArray data;
    if ( r.isCompressed() )
        data = qUncompress( ba );
    else
        data = ba;

	if ( unicode )
		dest.setUtf16( (const ushort*)(const char*)data, data.size() / sizeof(ushort) );
	else
		dest = data;

    return true;
}

template< typename STRING >
inline bool readFileFromResource( const QString &rpath, STRING &dest, bool unicode = false )
{
    QString str;
    if ( !readFileFromResource( rpath, str, unicode ) )
        return false;
    dest = q2t< STRING >( str );
    return true;
}

inline bool readUnicodeFileFromResource( const QString &rpath, std::wstring &dest )
{
	return readFileFromResource( rpath, dest, true );
}



///////////////////////////////////////////////////////////////////////////
//							Standard Paths
///////////////////////////////////////////////////////////////////////////


#if defined(_WIN32)
	#if defined(_WIN64)
		#define PLATFORM_SUBDIR "x64"
	#else defined(_WIN32)
		#define PLATFORM_SUBDIR "Win32"
	#endif
#else
	#if defined(__LP64__) || defined(__x86_64__)
		#define PLATFORM_SUBDIR "x86_64"
	#else
		#define PLATFORM_SUBDIR "i386"
	#endif
#endif

#if defined(_DEBUG) || defined(DEBUG)
	#define CONFIG_SUBDIR "Debug"
#else
	#define CONFIG_SUBDIR "Release"
#endif

#if defined(_WIN32)
	#define QGIS_PLUGINS_SUBDIR "plugins"
#elif defined (__APPLE__)
    #define QGIS_PLUGINS_SUBDIR "QGIS.app/Contents/PlugIns/qgis"
#else
	#define QGIS_PLUGINS_SUBDIR "lib/qgis/plugins"
#endif

struct ApplicationDirectories
{
	std::string mBasePath;
	std::string mPlatform;
	std::string mConfiguration;

	std::string mQgisDir;
	std::string mQgisPluginsDir;

	std::string mExecutableFileName;
	std::string mExecutableDir;
	std::string mInternalDataDir;
	std::string mExternalDataDir;
	std::string mWorkspacesPath;

	std::string mRasterLayerPath;
	std::string mVectorLayerPath;

	std::string mGlobeDir;

	static std::string DefaultExternalDataSubDir()
	{
		static const std::string s = "lib/data";
		return s;
	}
    static std::string DefaultProjectsSubDir()
	{
		static const std::string s = "workspaces";
		return s;
	}

	static std::string computeBaseDirectory()
	{
		auto s3root = getenv( "S3ALTB_ROOT" );
		if ( s3root )
			return s3root;

		return std::string();
	}
	static std::string computeInternalDataDirectory( const std::string &ExecutableDir )
	{
		std::string InternalDataDir;
		auto s3data = getenv( "BRAT_DATA_DIR" );
		if ( s3data )
			InternalDataDir = s3data;
        else
        {
            InternalDataDir = GetDirectoryFromPath( ExecutableDir );    //strip first parent directory (MacOS in mac)
        #if defined (__APPLE__)
            InternalDataDir = GetDirectoryFromPath( InternalDataDir );  //strip Contents
            InternalDataDir = GetDirectoryFromPath( InternalDataDir );  //strip brat.app
            InternalDataDir = GetDirectoryFromPath( InternalDataDir );  //strip wherever brat.app is
        #endif
            InternalDataDir += "/data";
        }
        return InternalDataDir;
	}
private:
	ApplicationDirectories() :
		  mBasePath( computeBaseDirectory() )
		, mPlatform( PLATFORM_SUBDIR )
		, mConfiguration( CONFIG_SUBDIR )

		, mQgisDir( mBasePath + "/lib/Graphics/QGIS/default/bin/" + mPlatform + "/" + mConfiguration )
		, mQgisPluginsDir( mQgisDir + "/" + QGIS_PLUGINS_SUBDIR )

		, mExecutableFileName( q2a( QCoreApplication::applicationFilePath() ) )
		, mExecutableDir( GetDirectoryFromPath( mExecutableFileName /*qApp->argv()[ 0 ]*/ ) )
		, mInternalDataDir( computeInternalDataDirectory( mExecutableDir ) )
		, mExternalDataDir( mBasePath + "/" + DefaultExternalDataSubDir() )
		, mWorkspacesPath( mBasePath + "/" + DefaultProjectsSubDir() )

		, mRasterLayerPath( mExternalDataDir + "/maps/NE1_HR_LC_SR_W_DR/NE1_HR_LC_SR_W_DR.tif" )
		, mVectorLayerPath( mExternalDataDir + "/maps/ne_10m_coastline/ne_10m_coastline.shp" )
	{
		//mGlobeDir = QDir::cleanPath( QgsApplication::pkgDataPath() + "/globe/gui" ).toStdString();
		//if ( QgsApplication::isRunningFromBuildDir() )
		//{
		//	mGlobeDir = QDir::cleanPath( QgsApplication::buildSourcePath() + "/src/plugins/globe/images/gui" ).toStdString();
		//}
		mGlobeDir = mExternalDataDir + "/globe/gui";
	}

public:
	static const ApplicationDirectories& instance()
	{
		static const ApplicationDirectories ad;
		return ad;
	}

	bool valid() const 
	{ 
		return 
			!mBasePath.empty() && !mInternalDataDir.empty() &&
			IsDir( mBasePath ) && IsDir( mInternalDataDir ) &&
			IsFile( mRasterLayerPath) && IsFile( mVectorLayerPath ); 
	}
};


#endif		//BRAT_QT_UTILS_IO_H
