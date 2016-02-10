#ifndef BRAT_QT_UTILS_H
#define BRAT_QT_UTILS_H

#if defined (QT_UTILS_H)
#error Wrong QtUtils.h included
#endif

#if QT_VERSION >= 0x050000
	#include <QtWidgets/QApplication>
	#include <QtWidgets/QFileDialog>
	#include <QtWidgets/QMessageBox>
	#include <QtWidgets/QLayout>
	#include <QtWidgets/QSplitter>
	#include <QtWidgets/QToolBar>
	#include <QtWidgets/QMainWindow>
	#include <QtWidgets/QListWidget>
    #include <QtWidgets/QGroupBox>
#else
	#include <QtGui/QApplication>
	#include <QtGui/QFileDialog>
	#include <QMessageBox>
	#include <QLayout>
	#include <QSplitter>
	#include <QToolBar>
	#include <QMainWindow>
	#include <QListWidget>
    #include <QGroupBox>
#endif
#include <QSettings>
#include <QResource>
#include <QElapsedTimer>
#include <QDesktopWidget>


#include "QtUtilsIO.h"	// QtUtilsIO.h => QtStringUtils.h => +Utils.h


//Table of Contents
//                      QSettings Reader/Writer Helpers
//
//                      Wait Cursor MFC Style
//
//                      QString Conversions (in "QtStringUtils.h")
//							q2X conversions
//							X2q conversions
//
//						Simple Message Boxes
//
//						File System Utilities  (in "QtFileUtils.h")
//
//						GUI File System Utilities
//
//						Resources File System Utilities
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

//////////
// generic
//////////

inline void SetObjectName( QObject *w, const std::string &name )
{
	static int index = 0;

    w->setObjectName( QString::fromUtf8( ( name + n2s< std::string >( ++index ) ).c_str() ) );
}


//////////
// space
//////////

inline QSpacerItem* CreateSpace( int w = 0, int h = 0, QSizePolicy::Policy hData = QSizePolicy::Expanding, QSizePolicy::Policy vData = QSizePolicy::Expanding )
{
	return new QSpacerItem( w, h, hData, vData );
}


//////////
//layouts
//////////

const int default_spacing = 6;
const int default_left = 2;
const int default_top = 2;
const int default_right = 2;
const int default_bottom = 2;



template< class LAYOUT >
inline LAYOUT* FinishLayout( LAYOUT *l, int spacing, int left, int top, int right, int bottom )
{
	l->setSpacing( spacing );
    SetObjectName( l, "boxLayout" );
	l->setContentsMargins( left, top, right, bottom );
	return l;
}


inline QBoxLayout* CreateLayout( QWidget *parent, Qt::Orientation orientation, int spacing = 0, int left = 0, int top = 0, int right = 0, int bottom = 0 )
{
	QVBoxLayout *vboxLayout;
	QHBoxLayout *hboxLayout;
	QBoxLayout *l;
	if ( orientation == Qt::Vertical ){
		l = vboxLayout = new QVBoxLayout( parent );
	} else {
		l = hboxLayout = new QHBoxLayout( parent );
	} 
	return FinishLayout< QBoxLayout >( l, spacing, left, top, right, bottom );
}


inline QGridLayout* CreateGridLayout( QWidget *parent, int spacing = 0, int left = 0, int top = 0, int right = 0, int bottom = 0 )
{
	return FinishLayout< QGridLayout >( new QGridLayout( parent ), spacing, left, top, right, bottom );
}


inline QBoxLayout* LayoutWidgets( Qt::Orientation o, const std::vector< QObject* > &v, 
	QWidget *parent, int spacing, int left, int top, int right, int bottom  )			
{
	QBoxLayout *main_l = CreateLayout( parent, o, spacing, left, top, right, bottom );
	for ( auto ob : v )
	{
		if ( !ob )
		{
			main_l->addItem( CreateSpace() );
			continue;
		}

		auto w = qobject_cast<QWidget*>( ob );
		auto l = qobject_cast<QLayout*>( ob );		assert__( w || l );
		if ( w )
			main_l->addWidget( w );
		else
			main_l->addLayout( l );
	}
	return main_l;
}


inline QGridLayout* LayoutWidgets( const std::vector< QObject* > &v, 
	QWidget *parent, int spacing, int left, int top, int right, int bottom )
{
	QGridLayout *main_l = CreateGridLayout( parent, spacing, left, top, right, bottom );
	int line = 0, col = 0;
	for ( auto ob : v )
	{
		if ( !ob )
		{
			++line;
			col = 0;
		}
		else
		{
			auto w = qobject_cast<QWidget*>( ob );
			auto l = qobject_cast<QLayout*>( ob );		assert__( w || l );
			if ( w )
				main_l->addWidget( w, line, col, 1, 1 );
			else
				main_l->addLayout( l, line, col, 1, 1 );

			++col;
		}
	}
	return main_l;
}


////////////////////
//layout group-box
////////////////////


// For use with CreateGroupBox
//
enum class ELayoutType
{
	Horizontal = Qt::Horizontal,
	Vertical = Qt::Vertical,
	Grid,

	ELayoutType_size
};


//	spacing and coordinates refer to enclosed layout
//
template< class GROUP_BOX = QGroupBox >
GROUP_BOX* CreateGroupBox( ELayoutType o, const std::vector< QObject* > &v, const QString &title = "", QWidget *parent = nullptr,
	int spacing = default_spacing, int left = default_left, int top = default_top, int right = default_right, int bottom = default_bottom )
{
	auto group = new GROUP_BOX( title, parent );

	for ( auto ob : v )
	{
		if ( !ob )
			continue;

		// QObject and QWidget setParent are not polymorphic
		//
		auto w = qobject_cast<QWidget*>( ob );
		auto l = qobject_cast<QLayout*>( ob );		assert__( w || l );
		if ( w )
			w->setParent( group );
		else
			l->setParent( nullptr );
	}

	switch ( o )
	{
		case ELayoutType::Horizontal:
		case ELayoutType::Vertical:
			LayoutWidgets( static_cast< Qt::Orientation >( o ), v, group, spacing, left, top, right, bottom  );
			break;

		case ELayoutType::Grid:
			LayoutWidgets( v, group, spacing, left, top, right, bottom  );
			break;

		default:
			assert__( false );
	}

	return group;
}


///////////
// position
///////////

inline QWidget* CenterOnParentCenter( QWidget *const w, QPoint pcenter )
{
	w->move( pcenter - w->rect().center() );

    return w;
}

inline QWidget* CenterOnScreen( QWidget *const w )
{
	assert__( qApp );

	return CenterOnParentCenter( w, qApp->desktop()->availableGeometry().center() );
}

inline QWidget* CenterOnScreen2( QWidget *const w )
{
	assert__( qApp );

    w->setGeometry( QStyle::alignedRect( Qt::LeftToRight, Qt::AlignCenter, w->size(), qApp->desktop()->availableGeometry() ) );

    return w;
}

inline QWidget* CenterOnScreen3( QWidget *const w )
{
    QRect screenGeometry = QApplication::desktop()->screenGeometry();
    int x = ( screenGeometry.width() - w->width() ) / 2;
    int y = ( screenGeometry.height() - w->height() ) / 2;
    w->move(x, y);

    return w;
}


///////////
// widgets
///////////

// widget is added to parent's layout, which is created if necessary
//
inline QWidget* AddWidget( QWidget *parent, QWidget *component )
{ 
	QLayout *l = parent->layout();
	if ( !l )
		l = CreateLayout( parent, Qt::Vertical );

	l->addWidget( component );
	return parent;
}


inline QFrame* WidgetLine( QWidget *parent, Qt::Orientation o )
{
    QFrame *line = new QFrame( parent );
    SetObjectName( line, "line" );
    line->setSizePolicy(
        o == Qt::Horizontal ? QSizePolicy::Expanding : QSizePolicy::Minimum,
        o == Qt::Horizontal ? QSizePolicy::Expanding : QSizePolicy::Minimum );
    line->setFrameStyle( ( o == Qt::Horizontal ? QFrame::HLine : QFrame::VLine ) | QFrame::Sunken );

    return line;
}



///////////
//splitter
///////////

inline QSplitter* createSplitter( QWidget *parent, Qt::Orientation orientation )
{
    QSplitter *s = new QSplitter( parent );
    SetObjectName( s, "splitter" );
    s->setOrientation( orientation );
	return s;
}
inline QSplitter* createSplitterIn( QWidget *parent, Qt::Orientation orientation )
{
    QSplitter *s = createSplitter( parent, orientation );
	if ( !parent->layout() )
			CreateLayout( parent, orientation );
    AddWidget( parent, s );
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


///////////
// tool-bar
///////////

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
        l = CreateLayout( w, orientation, 1, 1, 1, 1, 1 );	//(*)
    }
    l->addWidget( toolbar );
}
inline void insertToolBar( QMainWindow *w, QToolBar *toolbar, Qt::ToolBarArea area )
{
    w->addToolBar( area, toolbar );
}


///////////
// actions
///////////

// see ActionsTable.*



///////////
//combo box
///////////

//NOTE: this is a template only for the compiler not to error "use of undefined type" in the poor moc files
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


//////////////////////
//list widget types
//////////////////////

template< typename LIST_TYPE, typename CONTAINER, typename ENUM_VALUE >
inline LIST_TYPE* fillList_t( LIST_TYPE *c, CONTAINER names, std::initializer_list< ENUM_VALUE > selection, bool disable )
{
	for ( auto const &name : names ) 
	{
		c->addItem( name.c_str() );
	}

	for ( auto selected : selection )
		if ( selected >= 0 )
		{
			c->item( selected )->setSelected( true );
			c->setCurrentRow( selected );			 //setCurrentRow must be called for all selected items; if called once, deselects all others
		}
		else
			c->setCurrentRow( -1 );

	if ( disable )
		c->setEnabled( false );

	return c;
}

template< typename CONTAINER >
inline QListWidget* fillList( QListWidget *c, CONTAINER &names, int selection, bool disable )
{
	return fillList_t< QListWidget, CONTAINER, int >( c, names, { selection }, disable );
}



//////////////////////
//table widget
//////////////////////


// This is not always accurate.	resizeColumnsToContents should be
//	called if the returned width is used to resize the table and 
//	all width must be visible; but this is not desirable for big 
//	data.
//	To be improved with empiric experience.
//
template< class TABLE >
inline int ComputeTableWidth( TABLE *t )
{
	//t->resizeColumnsToContents();
	t->resize( 1, 1 );
	auto const vmode = t->verticalScrollMode();
	auto const hmode = t->horizontalScrollMode();
	t->setVerticalScrollMode( QAbstractItemView::ScrollPerPixel );		//vertical must be done first...
	t->setHorizontalScrollMode( QAbstractItemView::ScrollPerPixel );	//...and horizontal must also be done, even if only vertical scrollBar size matters

	int width = t->contentsMargins().left() + t->contentsMargins().right() + t->verticalScrollBar()->width();
	if ( t->verticalHeader()->isVisible() )
		width += t->verticalHeader()->width();
    const int columns = t->columnCount();
	for ( auto i = 0; i < columns; ++i )
		width += t->columnWidth( i );

	t->setVerticalScrollMode( vmode );
	t->setHorizontalScrollMode( hmode );
	return width;
}


// This was based on ComputeTableWidth but is (even more) less tested.
//	To be improved also with empiric experience.
//
template< class TABLE >
inline int ComputeTableHeight( TABLE *t )
{
	//t->resizeRowsToContents();
	t->resize( 1, 1 );
	auto const vmode = t->verticalScrollMode();
	auto const hmode = t->horizontalScrollMode();
	t->setVerticalScrollMode( QAbstractItemView::ScrollPerPixel );
	t->setHorizontalScrollMode( QAbstractItemView::ScrollPerPixel );

	int height = t->contentsMargins().top() + t->contentsMargins().bottom() + t->horizontalScrollBar()->height();
	if ( t->horizontalHeader()->isVisible() )
		height += t->horizontalHeader()->height();
    const int rows = t->rowCount();
	for ( auto i = 0; i < rows; ++i )
		height += t->rowHeight( i );

	t->setVerticalScrollMode( vmode );
	t->setHorizontalScrollMode( hmode );
	return height;
}




//////////////////////
//	dialog box
//////////////////////

inline void SetMaximizableDialog( QDialog *d )
{
#if defined (_WIN32) || defined (WIN32)
	// Show maximize button in windows
	// If this is set in linux, it will not center the dialog over parent
	d->setWindowFlags( ( d->windowFlags() & ~Qt::Dialog ) | Qt::Window | Qt::WindowMaximizeButtonHint );
#elif defined (Q_OS_MAC)
	// Qt::WindowStaysOnTopHint also works (too weel: stays on top of other apps also). Without this, we have the mac MDI mess...
	d->setWindowFlags( ( d->windowFlags() & ~Qt::Dialog ) | Qt::Tool );
#endif
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



//////////////////////////////////////////////////////////////////
//					Dimensions - BRAT Specific
//////////////////////////////////////////////////////////////////

inline QSize DefaultSizeHint( const QWidget *w )
{
	return QSize( 72 * w->fontMetrics().width( 'x' ), 25 * w->fontMetrics().lineSpacing() );
}


const int icon_size = 24;


const int min_main_window_width = 1024;
const int min_main_window_height = 640;

const auto child_ratio = 1. / 3.;

const int min_globe_widget_width =  min_main_window_width * child_ratio;
const int min_globe_widget_height = min_globe_widget_width / 2 * 3;

const int min_plot_widget_width = min_globe_widget_width;
const int min_plot_widget_height = min_globe_widget_height;


const int min_editor_dock_width = 200;
const int min_editor_dock_height = 10;





//////////////////////////////////////////////////////////////////
//	Development only
//////////////////////////////////////////////////////////////////

inline void NotImplemented( const char *msg = nullptr )
{
	SimpleMsgBox( std::string( "Not implemented. " ) + ( msg ? msg : "" ) );
}

#define NOT_IMPLEMENTED  NotImplemented( __func__ );
#define FNOT_IMPLEMENTED( x )  NotImplemented( std::string( x ).c_str() );




#endif		// BRAT_QT_UTILS_H
