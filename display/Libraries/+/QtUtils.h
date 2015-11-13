#ifndef QT_UTILS_H
#define QT_UTILS_H

#include "+Utils.h"

#include <QFile>
#if QT_VERSION >= 0x050000
	#include <QtWidgets/QApplication>
	#include <QtWidgets/QFileDialog>
	#include <QtWidgets/QMessageBox>
	#include <QtWidgets/QLayout>
	#include <QtWidgets/QSplitter>
	#include <QtWidgets/QToolBar>
	#include <QtWidgets/QMainWindow>
	#include <QtWidgets/QListWidget>
#else
	#include <QtGui/QApplication>
	#include <QtGui/QFileDialog>
	#include <QMessageBox>
	#include <QLayout>
	#include <QSplitter>
	#include <QToolBar>
	#include <QMainWindow>
	#include <QListWidget>
#endif
#include <QSettings>
#include <QResource>
#include <QElapsedTimer>

#include <Libraries/+/+Utils.h>

//Table of Contents
//                      QSettings Reader/Writer Helpers
//
//                      Wait Cursor MFC Style
//
//                      QString Conversions
// q2X conversions
// X2q conversions
//						Simple Message Boxes
//
//						File System Utilities
// Resources "file system" utilities
//
//						Timer
//
//						Widget Creation Utilities
//




///////////////////////////////////////////////////////////////////////////
//                      QSettings Reader/Writer Helpers
///////////////////////////////////////////////////////////////////////////


template< typename T >
QSettings& QSettingsWriteArray( QSettings &s, const T &o, const std::string name )
{
    s.beginWriteArray( name.c_str() );
    size_t size = o.size();
    for ( size_t i = 0; i < size; ++i )
    {
        s.setArrayIndex( (int)i );
        s << o[i];
    }
    s.endArray();
    return s;
}

template< typename T >
inline void DefaultItemReader( QSettings &s, T &o, size_t i )  //QSettings cannot be const
{
    o.push_back( typename T::value_type() );
    s >> o[i];
}

template< typename T, typename READER >
const QSettings& QSettingsReadArray( QSettings &s, T &o, const std::string name, READER r = DefaultItemReader )  //QSettings cannot be const
{
    size_t size = s.beginReadArray( name.c_str() );
    for ( size_t i = 0; i < size; ++i )
    {
        s.setArrayIndex( (int)i );
        r( s, o, i );
    }
    s.endArray();
    return s;
}

///////////////////////////////////////////////////////////////////////////
//                      Wait Cursor MFC Style
///////////////////////////////////////////////////////////////////////////

class WaitCursor
{
public:
    WaitCursor()
    {
        QApplication::setOverrideCursor( Qt::WaitCursor );
    }
    ~WaitCursor()
    {
        QApplication::restoreOverrideCursor();
    }
};



///////////////////////////////////////////////////////////////////////////
//                          QString Conversions
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////
/// q2X conversions
///////////////////////////////////////////////

inline std::wstring q2w( const QString qs )
{
#ifdef _MSC_VER
    return std::wstring((const wchar_t *)qs.utf16());
#else
    return qs.toStdWString();
#endif
}

inline std::string q2a( const QString qs )
{
    return qs.toStdString();
}

template< typename STRING >
STRING q2t( const QString &str );

template<>
inline std::string q2t( const QString &str )
{
    return q2a( str );
}
template<>
inline std::wstring q2t( const QString &str )
{
    return q2w( str );
}


///////////////////////////////////////////////
/// X2q conversions
///////////////////////////////////////////////

inline QString t2q( const std::wstring ws )
{
    //return QString::fromWCharArray( ws.c_str(), (int)ws.length() );
#ifdef _MSC_VER
    return QString::fromUtf16((const ushort *)ws.c_str());
#else
    return QString::fromStdWString( ws );
#endif
}

inline QString t2q( const std::string s )
{
    return QString::fromStdString( s );
}




///////////////////////////////////////////////////////////////////
//                  Simple Message Boxes
///////////////////////////////////////////////////////////////////

// DO NOT DELETE: serve as reference for comparison with the implementation 
//					style below	(better, see comment there)
//
inline void SimpleMsgBox2( const QString &msg )
{
    QMessageBox msgBox;
	msgBox.setIcon( QMessageBox::Information );
	msgBox.setText( msg );
    msgBox.exec();
}
template< class STRING >
inline void SimpleMsgBox2( const STRING &msg )
{
    SimpleMsgBox2( t2q( msg ) );
}

//	The implementations style of these functions is preferable to the one above (SimpleMsgBox2),
//	not only because they are simpler but also because the appropriate icon is automatically set 
//	not only in the dialog body but also the application icon is set in the frame: the one above
//	has the default NULL icon in the frame.
//	If you get bored of having the redundant titles below in the dialogs frame, just pass nullptr 
//	instead of the title string and you'll get the application name as title.
//
inline QMessageBox::StandardButton SimpleMsgBox( const QString &msg )
{
	return QMessageBox::information( qApp->activeWindow(), "Information", msg );
}
template< class STRING >
inline QMessageBox::StandardButton SimpleMsgBox( const STRING &msg )
{
	return SimpleMsgBox( t2q( msg ) );
}
inline void SimpleMsgBox( const char *msg )
{
	SimpleMsgBox( QString( msg ) );
}
											 

inline QMessageBox::StandardButton SimpleErrorBox( const QString &msg )
{
	return QMessageBox::critical( qApp->activeWindow(), "Error", msg );
}
template< class STRING >
inline QMessageBox::StandardButton SimpleErrorBox( const STRING &msg )
{
	return SimpleErrorBox( t2q( msg ) );
}
inline bool SimpleErrorBox( const char *msg )
{
	return SimpleErrorBox( QString( msg ) );
}


inline QMessageBox::StandardButton SimpleWarnBox( const QString &msg )
{
	return QMessageBox::warning( qApp->activeWindow(), "Warning", msg );
}
template< class STRING >
inline QMessageBox::StandardButton SimpleWarnBox( const STRING &msg )
{
	return SimpleWarnBox( t2q( msg ) );
}
inline bool SimpleWarnBox( const char *msg )
{
	return SimpleWarnBox( QString( msg ) );
}


inline bool SimpleQuestion( const QString &msg )
{
	return QMessageBox::warning( qApp->activeWindow(), "Question", msg, QMessageBox::Yes | QMessageBox::No ) == QMessageBox::Yes;
}
template< class STRING >
inline bool SimpleQuestion( const STRING &msg )
{
	return SimpleQuestion( t2q( msg ) );
}
inline bool SimpleQuestion( const char *msg )
{
	return SimpleQuestion( QString( msg ) );
}





///////////////////////////////////////////////////////////////////////////
//                      File System GUI Utilities
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



///////////////////////////////////////////////////////////////////////////
// Resources "file system" utlities
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
//								Timer
///////////////////////////////////////////////////////////////////////////

//Use like this
//		QElapsedTimer timer;
//		timer.start();
//		//do stuff 
//		QString elapsed = ElapsedFormat( timer );
//
inline QString ElapsedFormat( QElapsedTimer &timer )
{
	auto t = timer.elapsed();
	TIME_FIELD TimeFld;
	FormatTimeFields( t * 1e4, &TimeFld );
	QString s = t2q( n2s< std::string >( t ) + " - " );
	std::string tmp;

	tmp = n2s< std::string >( TimeFld.Hours );
	if ( tmp.length() < 2 )
		tmp = "0" + tmp;
	s += t2q( tmp + ":" );

	tmp = n2s< std::string >( TimeFld.Mins );
	if ( tmp.length() < 2 )
		tmp = "0" + tmp;
	s += t2q( tmp + ":" );

	tmp = n2s< std::string >( TimeFld.Secs );
	if ( tmp.length() < 2 )
		tmp = "0" + tmp;
	s += t2q( tmp + "::" );

	tmp = n2s< std::string >( TimeFld.mSecs );
	const size_t zeros = 3 - tmp.length();
	for ( size_t i = 0; i < zeros; ++ i )
		tmp = "0" + tmp;
	s += t2q( tmp );

	return s;
}



///////////////////////////////////////////////////////////////////////////
//						Widget Creation Utilities
///////////////////////////////////////////////////////////////////////////


//generic

inline QWidget* addWidget( QWidget *parent, QWidget *component )	//widget is added to layout
{
	parent->layout()->addWidget( component );
	return parent;
}
inline void setObjectName( QObject *w, const std::string &name )
{
	static int index = 0;

    w->setObjectName( QString::fromUtf8( ( name + n2s< std::string >( ++index ) ).c_str() ) );
}

//layout

inline QLayout* createLayout( QWidget *parent, Qt::Orientation orientation, int spacing = 0, int left = 0, int top = 0, int right = 0, int bottom = 0 )
{
	QVBoxLayout *vboxLayout;
	QHBoxLayout *hboxLayout;
	QLayout *l;
	if ( orientation == Qt::Vertical ){
		vboxLayout = new QVBoxLayout( parent );
		l = vboxLayout;
	} else {
		hboxLayout = new QHBoxLayout( parent );
		l = hboxLayout;
	} 
	l->setSpacing( spacing );
    setObjectName( l, "boxLayout" );
	l->setContentsMargins( left, top, right, bottom );
	return l;
}

//splitter

inline QSplitter* createSplitter( QWidget *parent, Qt::Orientation orientation )
{
    QSplitter *s = new QSplitter( parent );
    setObjectName( s, "splitter" );
    s->setOrientation( orientation );
	return s;
}
inline QSplitter* createSplitterIn( QWidget *parent, Qt::Orientation orientation )
{
    QSplitter *s = createSplitter( parent, orientation );
	if ( !parent->layout() )
			createLayout( parent, orientation );
    addWidget( parent, s );
	return s;
}
inline QSplitter* createSplitterIn( QSplitter *parent, Qt::Orientation orientation )
{
    QSplitter *s = createSplitter( parent, orientation );
    parent->addWidget( s );
	return s;
}
inline QSplitter* createSplitterIn( QMainWindow *parent, Qt::Orientation orientation )
{
	QSplitter *s = createSplitter( parent, orientation );
	parent->setCentralWidget( s );
	return s;
}

// toolbar

//	Destroys any existing layout, so it must be called before any other
//	widgets/layouts are inserted in w
//
//	(*) See IMPORTANT note in	OSGGraphicEditor constructor or build
//
inline void insertToolBar( QWidget *w, QToolBar *toolbar, Qt::ToolBarArea area )
{
    Qt::Orientation	orientation = Qt::Horizontal;   //initializarion to silent compiler warnings in release mode
    switch ( area )
    {
        case Qt::LeftToolBarArea:
        case Qt::RightToolBarArea:
        {
            orientation = Qt::Horizontal;
            toolbar->setOrientation( Qt::Vertical );
            break;
        }
        case Qt::TopToolBarArea:
        case Qt::BottomToolBarArea:
            orientation = Qt::Vertical;
            toolbar->setOrientation( Qt::Horizontal );
            break;
        default:
            assert__( false );
    }

    QLayout *l = w->layout();
    {//(*)
        if ( l )
            delete l;
        l = createLayout( w, orientation, 1, 1, 1, 1, 1 );	//(*)
    }
    l->addWidget( toolbar );
}
inline void insertToolBar( QMainWindow *w, QToolBar *toolbar, Qt::ToolBarArea area )
{
    w->addToolBar( area, toolbar );
}



//combo box

//NOTE: this is a template only for the compiler not to error use of undefined type in the shitish moc files
//
template< typename COMBO >
inline void fillCombo( COMBO *c, const std::string *names, size_t size, int selected, bool enabled )
{
	for ( size_t i = 0; i < size; ++i ) {
		c->addItem( names[i].c_str() );
	}
	c->setCurrentIndex( selected );
	c->setEnabled( enabled );
}

//list widget types

template<  typename LIST_TYPE, typename ENUM_VALUE >
inline LIST_TYPE* fillList_t( LIST_TYPE *c, const std::string *names, size_t size, const std::vector< ENUM_VALUE > &selected, bool enabled )
{
	for ( size_t i = 0; i < size; ++i ) {
		c->addItem( names[i].c_str() );
	}

	const size_t sel_size = selected.size();
	for ( size_t i = 0; i < sel_size; ++i )
		if ( selected[i] >= 0 )
		{
			c->item( selected[i] )->setSelected( true );
			c->setCurrentRow( selected[i] );			//yes, it is stupid, but setCurrentRow must be called for all selected items; if called once, deselects all others
		}
		else
			c->setCurrentRow( -1 );

	c->setEnabled( enabled );
	return c;
}

inline QListWidget* fillList( QListWidget *c, const std::string *names, size_t size, int selected, bool enabled )
{
	std::vector< int > v; v.push_back( selected );
	return fillList_t< QListWidget >( c, names, size, v, enabled );
}



#endif
