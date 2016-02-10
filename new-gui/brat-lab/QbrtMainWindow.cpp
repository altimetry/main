#include "stdafx.h"

#include <cassert>

#include <qtimer.h>

#if QT_VERSION >= 0x050000
#include <QtConcurrent/QtConcurrent>
#endif

#include "new-gui/brat/DataModels/CmdLineProcessor.h"
#include "GUI/SettingsDlg.h"
#include "GUI/QbrtMapEditor.h"

#include "brat-lab.h"
#include "QbrtMainWindow.h"



#ifdef Q_WS_MAC
const QString rsrcPath = ":/images/mac";
#else
const QString rsrcPath = ":/images/win";
#endif

//To add Format toolbar and menu
//
//  - Add the files
//        - QFormatToolBar.h
//        - QFormatToolBar.cpp
//  - Copy the res/images folder to the project folder and add them to the qresources
//  - In the designer
//      - Create a (sub)menu menu_Format
//      - Create a Format_toolBar
//      - Promote it to QFormatToolBar
//  - Add the methods in the format section below
//  - Add the code pertinent for this toolbar in the "toolbars" section of the main
//      window ctor below: SetFormatActions(); and the menu view item
//  - Add the pertinent code in connectAutoUpdateEditActions
//  - Copy the pertinent last lines of LoadCmdLineFiles for initialization of the toolbar widgets state

//To add Font toolbar and menu
//
//  - Add the files
//        - QFontToolBar.h
//        - QFontToolBar.cpp
//  - Copy the res/images folder to the project folder and add them to the qresources
//  - In the designer
//      - Create a Font_toolBar
//      - Promote it to QFontToolBar
//  - Add the methods in the font section below
//  - Add the code pertinent for this toolbar in the "toolbars" section of the main
//      window ctor below: Font_toolBar->SetActions( this ); and the menu view item
//  - Add the pertinent code in connectAutoUpdateEditActions
//  - Copy the pertinent last lines of LoadCmdLineFiles for initialization of the toolbar widgets state
//

#include <qgsfeature.h>
#include <qgsspatialindex.h>


QbrtMainWindow::QbrtMainWindow( CBratSettings &settings ) :
    QMainWindow( nullptr )
	, mSettings( settings )
	, m_bs( GetBackServices() )
    , m_FindDialog( nullptr )
{
    setupUi(this);
    //mdiArea->setViewMode(QMdiArea::SubWindowView);
    setWindowIcon( QIcon(":/butterfly.png") );
    setWindowTitle( tr("%1[*] - %2").arg( windowTitle() ).arg( tr("Rich Text") ) );     //set title sensitive to doc (un)modifications

    CMapWidget::SetQGISDirectories(
                mSettings.BratPaths().mQgisPluginsDir,
                mSettings.BratPaths().mVectorLayerPath,
                mSettings.BratPaths().RasterLayerPath(),
                mSettings.BratPaths().mGlobeDir );

	QImage newBackground(":/images/world.png");
	mdiArea->setBackground( newBackground );

    // Menu File
    //
    menu_File->insertAction( action_Print, action_Close );    //replicate menu item here: it is handy...
    menu_File->insertSeparator( action_Print );

    // Menu File / Most recent files logic
    //
    m_RecentFiles_separatorAction = menu_File->insertSeparator( action_Exit );
    for (int i = 0; i < sm_MaxRecentFiles; ++i) {
        m_recentFileActions[i] = new QAction(this);
        m_recentFileActions[i]->setVisible(false);
        connect( m_recentFileActions[i], SIGNAL(triggered()), this, SLOT(openRecentFile()) );
        menu_File->insertAction( action_Exit, m_recentFileActions[i] );
    }
    m_RecentFiles_separatorAction = menu_File->insertSeparator( action_Exit );

    setupPrintActions();

    // Aplication Settings
    //
    readSettings();


    // MDI sub-windows logic
    //
    SubWindows_separatorAction = menu_Window->addSeparator();
    windowActionGroup = new QActionGroup(this);
    connect( mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(UpdateEditorsActions()) ); //UpdateEditorsActions => UpdateFileActions
    connect( mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(InitialUpdateActions()) );
    //
    //it seems the above line is not enough... well, keep testing...
    //
    connect( menu_File, SIGNAL(aboutToShow()), this, SLOT(UpdateFileActions()) );
    connect( menu_Edit, SIGNAL(aboutToShow()), this, SLOT(UpdateEditorsActions()) );
    connect( menu_Window, SIGNAL(aboutToShow()), this, SLOT(UpdateFileActions()) );
    #ifndef QT_NO_CLIPBOARD
    connect( QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(clipboardDataChanged()) );
    #endif

    // Menu View / Toolbars
    //
    SetFormatActions();
    Font_toolBar->SetActions( this );

    menu_View->addAction( mainToolBar->toggleViewAction() );
    menu_View->addAction( Format_toolBar->toggleViewAction() );
    menu_View->addAction( Font_toolBar->toggleViewAction() );
    menu_View->addAction( Tools_toolBar->toggleViewAction() );

    // Menu View / Docks (see also the toolbars section in this ctor)
    //
    setDockOptions( dockOptions() | VerticalTabs );     //not set in designer

    setCorner( Qt::TopLeftCorner, Qt::LeftDockWidgetArea );
    setCorner( Qt::TopRightCorner, Qt::RightDockWidgetArea );
    setCorner( Qt::BottomRightCorner, Qt::RightDockWidgetArea );
    setCorner( Qt::BottomLeftCorner, Qt::BottomDockWidgetArea );

    Editor *editors[] = { Output_textEdit, Tool_textEdit };
    QDockWidget *docks[] = { mOutput_dockWidget, Tool_dockWidget };
    size_t size = sizeof(editors) / sizeof(*editors);
    menu_View->addSeparator();
    for ( size_t i = 0; i < size; ++i )
    {
        editors[i]->ToolEditor( true );
        connectAutoUpdateEditActions( editors[i] );
        menu_View->addAction( docks[i]->toggleViewAction() );
    }

    // Menu Tools
    //
    connect( menu_Tools, SIGNAL(aboutToShow()), this, SLOT(UpdateToolsActions()) );

    // Status Bar
    //
	createStatusBar();

    // Load CmdLine Files
    //
    QTimer::singleShot( 0, this, SLOT( LoadCmdLineFiles() ) );
	//QApplication::sendEvent(this,new QCloseEvent());

	mConcurrentActions.push_back(mAction_Test);
	//QTimer::singleShot( 0, this, SLOT( doThreadedStuff() ) );

	//QgsRectangle rect;
	//QgsSpatialIndex idx;
	//rect.setXMaximum( 1037.6081200907827 );
	//rect.setYMaximum( 83.634100653000075 );
	//rect.setXMinimum( -1037.6081200907827 );
	//rect.setYMinimum( -85.221937757999910 );
	//QList<QgsFeatureId> l = idx.intersects( rect );
}



struct ProgressEvent : public QEvent
{
	enum { EventId = QEvent::User };

	const size_t thread_done;
	const QString message;

	explicit ProgressEvent( size_t thread_done_, const QString &message_ )
		: QEvent( static_cast<Type>( EventId ) ),
		thread_done( thread_done_ ), message( message_ ) 
	{}
};

void updateUI( void *receiver, size_t thread_done, std::string message )
{
	QObject *qreceiver = static_cast<QObject*>(receiver);

	QApplication::postEvent( qreceiver, new ProgressEvent( thread_done, t2q( message ) ) );
}

bool QbrtMainWindow::event( QEvent *event )
{
	if ( !mStopped && event->type() == static_cast<QEvent::Type>( ProgressEvent::EventId ) ) 
	{
		ProgressEvent *progressEvent = static_cast<ProgressEvent*>( event );
		
		Q_ASSERT( progressEvent );
		
		mDone += progressEvent->thread_done;
		mMessageButton->setText( progressEvent->message + " - " + t2q( n2s<std::string>(mDone) ) );
		mProgressBar->setMaximum( mTotal );
		mProgressBar->setValue( (int)mDone );

		return true;
	}
	return base_t::event( event );
}

void convertImages( QObject *receiver, const volatile bool *stopped, const size_t from, const size_t to, size_t max_notifications )
{
	const size_t size = to - from;
	std::string message;
	size_t count = 0;
	auto notify_interval = size / max_notifications;
	if ( !notify_interval )
		notify_interval = size;
	try{
		for ( size_t i = from; i < to; ++i )
		{
			if ( *stopped )
				break;

			message = "Thread +" + n2s<std::string>( QThread::currentThreadId() );

			count++;
			if ( count == notify_interval ) {
				updateUI( receiver, count, message );
				count = 0;
			}
		}
		updateUI( receiver, count, message );
	}
	catch ( ... )
	{
		//tragic update UI, or this will be lost
	}
}

void QbrtMainWindow::checkIfStuffDone()
{
	static const int PollTimeout = 100;

	if ( QThreadPool::globalInstance()->activeThreadCount() )
		QTimer::singleShot( PollTimeout, this, SLOT( checkIfStuffDone() ) );
	else {
		qApp->processEvents();

		QString message;
		if ( mDone == mTotal )
			message = tr( "All %n image(s) converted", "", (int)mDone );
		else
			message = tr( "Converted %n/%1 image(s)", "", (int)mDone )
			.arg( mTotal );

		mMessageButton->setText( message );
		mProgressBar->setMaximum( 1 );
		mProgressBar->reset();
		mProgressBar->hide();
		mThreadsButton->hide();

		mStopped = true;
		for ( auto it : mConcurrentActions )
			it->setEnabled( true );
	}
}
void QbrtMainWindow::cancelStuff( bool cancel )
{
	if ( !cancel )
		return;

	mStopped = true;
	mThreadsButton->setText( "Canceling..." );
	checkIfStuffDone();
}

void QbrtMainWindow::doThreadedStuff()
{
	if ( !mStopped ) {
		SimpleErrorBox( "You are a dumb idiot. A background process is already running" );
		return;
	}

	for ( auto it : mConcurrentActions )
		it->setEnabled( false );

	mTotal = 10000000;
	mDone = 0;
	mStopped = false;

	mProgressBar->show();
	mProgressBar->setMaximum( mTotal );
	mProgressBar->setValue( 0 );
	mThreadsButton->show();
	mThreadsButton->setChecked( false );
	mThreadsButton->setText( "Cancel" );

	int nProcs = std::max( 1, QThread::idealThreadCount() / 2 );
	int chunk = mTotal / nProcs;
	if ( !chunk )
		chunk = mTotal;
	int chunks = mTotal / chunk;
	size_t remaining = mTotal % chunk;

	for ( size_t i = 0; i < chunks; ++i ) 
	{
		size_t from = i*chunk;
		size_t to = from + chunk;
		QtConcurrent::run( convertImages, this, &mStopped, from, to, 10 );
	}
	if ( remaining )
		QtConcurrent::run( convertImages, this, &mStopped, mTotal - remaining, mTotal, 10 );
	checkIfStuffDone();
}

void QbrtMainWindow::on_mAction_Test_triggered()
{
	doThreadedStuff();
}

void QbrtMainWindow::createStatusBar()
{
	//remove borders from children under Windows
	//statusBar()->setStyleSheet( "QStatusBar::item {border: none;}" );

	// Add a panel to the status bar for the scale, coords and progress
	// And also rendering suppression checkbox
	mProgressBar = new QProgressBar( statusBar() );
	mProgressBar->setObjectName( "mProgressBar" );
	mProgressBar->setMaximumWidth( 100 );
	//mProgressBar->hide();
	mProgressBar->setWhatsThis( tr( "Progress bar that displays the status of time-intensive operations" ) );
	statusBar()->addPermanentWidget( mProgressBar, 1 );

	//connect( mMapCanvas, SIGNAL( renderStarting() ), this, SLOT( canvasRefreshStarted() ) );
	//connect( mMapCanvas, SIGNAL( mapCanvasRefreshed() ), this, SLOT( canvasRefreshFinished() ) );

	//// Bumped the font up one point size since 8 was too
	//// small on some platforms. A point size of 9 still provides
	//// plenty of display space on 1024x768 resolutions
	//QFont myFont( "Arial", 9 );

	//statusBar()->setFont( myFont );
	////toggle to switch between mouse pos and extents display in status bar widget
	//mToggleExtentsViewButton = new QToolButton( statusBar() );
	//mToggleExtentsViewButton->setObjectName( "mToggleExtentsViewButton" );
	//mToggleExtentsViewButton->setMaximumWidth( 20 );
	//mToggleExtentsViewButton->setMaximumHeight( 20 );
	//mToggleExtentsViewButton->setIcon( QgsApplication::getThemeIcon( "tracking.png" ) );
	//mToggleExtentsViewButton->setToolTip( tr( "Toggle extents and mouse position display" ) );
	//mToggleExtentsViewButton->setCheckable( true );
	//connect( mToggleExtentsViewButton, SIGNAL( toggled( bool ) ), this, SLOT( extentsViewToggled( bool ) ) );
	//statusBar()->addPermanentWidget( mToggleExtentsViewButton, 0 );

	//// add a label to show current scale
	//mCoordsLabel = new QLabel( QString(), statusBar() );
	//mCoordsLabel->setObjectName( "mCoordsLabel" );
	//mCoordsLabel->setFont( myFont );
	//mCoordsLabel->setMinimumWidth( 10 );
	//mCoordsLabel->setMaximumHeight( 20 );
	//mCoordsLabel->setMargin( 3 );
	//mCoordsLabel->setAlignment( Qt::AlignCenter );
	//mCoordsLabel->setFrameStyle( QFrame::NoFrame );
	//mCoordsLabel->setText( tr( "Coordinate:" ) );
	//mCoordsLabel->setToolTip( tr( "Current map coordinate" ) );
	//statusBar()->addPermanentWidget( mCoordsLabel, 0 );

	////coords status bar widget
	//mCoordsEdit = new QLineEdit( QString(), statusBar() );
	//mCoordsEdit->setObjectName( "mCoordsEdit" );
	//mCoordsEdit->setFont( myFont );
	//mCoordsEdit->setMinimumWidth( 10 );
	//mCoordsEdit->setMaximumWidth( 300 );
	//mCoordsEdit->setMaximumHeight( 20 );
	//mCoordsEdit->setContentsMargins( 0, 0, 0, 0 );
	//mCoordsEdit->setAlignment( Qt::AlignCenter );
	//QRegExp coordValidator( "[+-]?\\d+\\.?\\d*\\s*,\\s*[+-]?\\d+\\.?\\d*" );
	//mCoordsEditValidator = new QRegExpValidator( coordValidator, mCoordsEdit );
	//mCoordsEdit->setWhatsThis( tr( "Shows the map coordinates at the "
	//	"current cursor position. The display is continuously updated "
	//	"as the mouse is moved. It also allows editing to set the canvas "
	//	"center to a given position. The format is lat,lon or east,north" ) );
	//mCoordsEdit->setToolTip( tr( "Current map coordinate (lat,lon or east,north)" ) );
	//statusBar()->addPermanentWidget( mCoordsEdit, 0 );
	//connect( mCoordsEdit, SIGNAL( returnPressed() ), this, SLOT( userCenter() ) );
	//mDizzyTimer = new QTimer( this );
	//connect( mDizzyTimer, SIGNAL( timeout() ), this, SLOT( dizzy() ) );

	//// add a label to show current scale
	//mScaleLabel = new QLabel( QString(), statusBar() );
	//mScaleLabel->setObjectName( "mScaleLable" );
	//mScaleLabel->setFont( myFont );
	//mScaleLabel->setMinimumWidth( 10 );
	//mScaleLabel->setMaximumHeight( 20 );
	//mScaleLabel->setMargin( 3 );
	//mScaleLabel->setAlignment( Qt::AlignCenter );
	//mScaleLabel->setFrameStyle( QFrame::NoFrame );
	//mScaleLabel->setText( tr( "Scale " ) );
	//mScaleLabel->setToolTip( tr( "Current map scale" ) );
	//statusBar()->addPermanentWidget( mScaleLabel, 0 );

	//mScaleEdit = new QgsScaleComboBox( statusBar() );
	//mScaleEdit->setObjectName( "mScaleEdit" );
	//mScaleEdit->setFont( myFont );
	//// seems setFont() change font only for popup not for line edit,
	//// so we need to set font for it separately
	//mScaleEdit->lineEdit()->setFont( myFont );
	//mScaleEdit->setMinimumWidth( 10 );
	//mScaleEdit->setMaximumWidth( 100 );
	//mScaleEdit->setMaximumHeight( 20 );
	//mScaleEdit->setContentsMargins( 0, 0, 0, 0 );
	//mScaleEdit->setWhatsThis( tr( "Displays the current map scale" ) );
	//mScaleEdit->setToolTip( tr( "Current map scale (formatted as x:y)" ) );

	//statusBar()->addPermanentWidget( mScaleEdit, 0 );
	//connect( mScaleEdit, SIGNAL( scaleChanged() ), this, SLOT( userScale() ) );

	//if ( QgsMapCanvas::rotationEnabled() )
	//{
	//	// add a widget to show/set current rotation
	//	mRotationLabel = new QLabel( QString(), statusBar() );
	//	mRotationLabel->setObjectName( "mRotationLabel" );
	//	mRotationLabel->setFont( myFont );
	//	mRotationLabel->setMinimumWidth( 10 );
	//	mRotationLabel->setMaximumHeight( 20 );
	//	mRotationLabel->setMargin( 3 );
	//	mRotationLabel->setAlignment( Qt::AlignCenter );
	//	mRotationLabel->setFrameStyle( QFrame::NoFrame );
	//	mRotationLabel->setText( tr( "Rotation:" ) );
	//	mRotationLabel->setToolTip( tr( "Current clockwise map rotation in degrees" ) );
	//	statusBar()->addPermanentWidget( mRotationLabel, 0 );

	//	mRotationEdit = new QgsDoubleSpinBox( statusBar() );
	//	mRotationEdit->setObjectName( "mRotationEdit" );
	//	mRotationEdit->setClearValue( 0.0 );
	//	mRotationEdit->setKeyboardTracking( false );
	//	mRotationEdit->setMaximumWidth( 120 );
	//	mRotationEdit->setDecimals( 1 );
	//	mRotationEdit->setRange( -180.0, 180.0 );
	//	mRotationEdit->setWrapping( true );
	//	mRotationEdit->setSingleStep( 5.0 );
	//	mRotationEdit->setFont( myFont );
	//	mRotationEdit->setWhatsThis( tr( "Shows the current map clockwise rotation "
	//		"in degrees. It also allows editing to set "
	//		"the rotation" ) );
	//	mRotationEdit->setToolTip( tr( "Current clockwise map rotation in degrees" ) );
	//	statusBar()->addPermanentWidget( mRotationEdit, 0 );
	//	connect( mRotationEdit, SIGNAL( valueChanged( double ) ), this, SLOT( userRotation() ) );

	//	showRotation();
	//}

	//// render suppression status bar widget
	//mRenderSuppressionCBox = new QCheckBox( tr( "Render" ), statusBar() );
	//mRenderSuppressionCBox->setObjectName( "mRenderSuppressionCBox" );
	//mRenderSuppressionCBox->setChecked( true );
	//mRenderSuppressionCBox->setFont( myFont );
	//mRenderSuppressionCBox->setWhatsThis( tr( "When checked, the map layers "
	//	"are rendered in response to map navigation commands and other "
	//	"events. When not checked, no rendering is done. This allows you "
	//	"to add a large number of layers and symbolize them before rendering." ) );
	//mRenderSuppressionCBox->setToolTip( tr( "Toggle map rendering" ) );
	//statusBar()->addPermanentWidget( mRenderSuppressionCBox, 0 );
	//// On the fly projection status bar icon
	//// Changed this to a tool button since a QPushButton is
	//// sculpted on OS X and the icon is never displayed [gsherman]
	//mOnTheFlyProjectionStatusButton = new QToolButton( statusBar() );
	//mOnTheFlyProjectionStatusButton->setAutoRaise( true );
	//mOnTheFlyProjectionStatusButton->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
	//mOnTheFlyProjectionStatusButton->setObjectName( "mOntheFlyProjectionStatusButton" );
	//// Maintain uniform widget height in status bar by setting button height same as labels
	//// For Qt/Mac 3.3, the default toolbutton height is 30 and labels were expanding to match
	//mOnTheFlyProjectionStatusButton->setMaximumHeight( mScaleLabel->height() );
	//mOnTheFlyProjectionStatusButton->setIcon( QgsApplication::getThemeIcon( "mIconProjectionEnabled.png" ) );
	//mOnTheFlyProjectionStatusButton->setWhatsThis( tr( "This icon shows whether "
	//	"on the fly coordinate reference system transformation is enabled or not. "
	//	"Click the icon to bring up "
	//	"the project properties dialog to alter this behaviour." ) );
	//mOnTheFlyProjectionStatusButton->setToolTip( tr( "CRS status - Click "
	//	"to open coordinate reference system dialog" ) );
	//connect( mOnTheFlyProjectionStatusButton, SIGNAL( clicked() ),
	//	this, SLOT( projectPropertiesProjections() ) );//bring up the project props dialog when clicked
	//statusBar()->addPermanentWidget( mOnTheFlyProjectionStatusButton, 0 );
	//statusBar()->showMessage( tr( "Ready" ) );

	mThreadsButton = new QToolButton( statusBar() );
	mThreadsButton->setAutoRaise( true );
	mThreadsButton->setIcon( QbrtApplication::getThemeIcon( "mActionCancelEdits.svg" ) );
	mThreadsButton->setToolTip( tr( "Background Tasks" ) );
	mThreadsButton->setWhatsThis( tr( "BackgroundTasks" ) );
	mThreadsButton->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
	mThreadsButton->setObjectName( "mThreadsButton" );
	mThreadsButton->setCheckable( true );
	mThreadsButton->hide();
	statusBar()->addPermanentWidget( mThreadsButton, 0 );

	connect( mThreadsButton, SIGNAL( toggled( bool ) ), this, SLOT( cancelStuff( bool ) ) );

	mMessageButton = new QToolButton( statusBar() );
	mMessageButton->setAutoRaise( true );
	mMessageButton->setIcon( QbrtApplication::getThemeIcon( "bubble.svg" ) );
	mMessageButton->setToolTip( tr( "Messages" ) );
	mMessageButton->setWhatsThis( tr( "Messages" ) );
	mMessageButton->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
	mMessageButton->setObjectName( "mMessageButton" );
	//mMessageButton->setMaximumHeight( mScaleLabel->height() );
	mMessageButton->setCheckable( true );
	statusBar()->addPermanentWidget( mMessageButton, 0 );

	connect( mMessageButton, SIGNAL( toggled( bool ) ), mOutput_dockWidget, SLOT( setVisible( bool ) ) );
	connect( mOutput_dockWidget, SIGNAL( visibilityChanged( bool ) ), mMessageButton, SLOT( setChecked( bool ) ) );	
}


//virtual 
QbrtMainWindow::~QbrtMainWindow()
{
	delete mCmdLineProcessor;
}


void QbrtMainWindow::readSettings()
{
    static QSettings& settings = m_bs.GetAppSettings();

    settings.beginGroup( "MainWindow" );

    restoreGeometry( settings.value("geometry").toByteArray() );
    restoreState( settings.value("state").toByteArray() );

    m_recentFiles = settings.value("recentFiles").toStringList();
    updateRecentFileActions();

//    bool showGrid = settings.value("showGrid", true).toBool();
//    showGridAction->setChecked(showGrid);

//    bool autoRecalc = settings.value("autoRecalc", true).toBool();
//    autoRecalcAction->setChecked(autoRecalc);

    settings.endGroup();
}


void QbrtMainWindow::writeSettings()
{
    static QSettings& settings = m_bs.GetAppSettings();

    settings.beginGroup( "MainWindow" );
    settings.setValue( "geometry", saveGeometry() );
    settings.setValue( "state", saveState() );

    settings.setValue( "recentFiles", m_recentFiles );

//    settings.setValue("showGrid", showGridAction->isChecked());
//    settings.setValue("autoRecalc", autoRecalcAction->isChecked());
    settings.endGroup();
}


EditorBase *QbrtMainWindow::activeMDIEditor()
{
    QMdiSubWindow *subWindow = mdiArea->activeSubWindow();
    if (subWindow)
        return dynamic_cast< EditorBase* >(subWindow->widget());
    return 0;
}

template< class EDITOR >
EDITOR* QbrtMainWindow::activeEditor()
{
    QWidget *pw = QApplication::focusWidget();
    if ( pw == Output_textEdit || pw == Tool_textEdit )
        return qobject_cast<EDITOR*>( pw );

    //default to MDI subWindows
    //
    return dynamic_cast< EDITOR* >( activeMDIEditor() );
}
Editor *QbrtMainWindow::activeTextEditor()
{
	return activeEditor<Editor>();
    //QWidget *pw = QApplication::focusWidget();
    //if ( pw == Output_textEdit || pw == Tool_textEdit )
    //    return qobject_cast<Editor *>( pw );

    ////default to MDI subWindows
    ////
    //return dynamic_cast< Editor* >( activeMDIEditor() );
}
QbrtMapEditor *QbrtMainWindow::activeMapEditor()
{
	return activeEditor<QbrtMapEditor>();
    //QWidget *pw = QApplication::focusWidget();
    //if ( pw == Output_textEdit || pw == Tool_textEdit )
    //    return qobject_cast<QbrtMapEditor *>( pw );

    ////default to MDI subWindows
    ////
    //return dynamic_cast< QbrtMapEditor* >( activeMDIEditor() );
}


void QbrtMainWindow::connectAutoUpdateEditActions( Editor* pe )
{
    connect( pe, SIGNAL(toolWindowActivated( Editor* )), this, SLOT(UpdateEditorsActions()) ); //UpdateEditorsActions => UpdateFileActions
    connect( pe, SIGNAL(toolWindowActivated( Editor* )), this, SLOT(InitialUpdateActions()) );

    pe->connectSaveAvailableAction( action_Save );

    pe->connectUndoAvailableAction( action_Undo );
    pe->connectRedoAvailableAction( action_Redo );
    pe->connectCutAvailableAction( action_Cut );
    pe->connectCopyAvailableAction( action_Copy );
    pe->connectDeleteSelAvailableAction( action_Delete );

    connect( pe, SIGNAL( currentCharFormatChanged(QTextCharFormat)), this, SLOT( currentCharFormatChanged(QTextCharFormat)) );
    connect( pe, SIGNAL( cursorPositionChanged()), this, SLOT( cursorPositionChanged()) );
    connect( pe->document(), SIGNAL(modificationChanged(bool)), this, SLOT(setWindowModified(bool)));
}



#include <iostream>

void QbrtMainWindow::InitialUpdateActions()
{
    Editor *pe = activeTextEditor();    //has the focus
    EditorBase *pmdi = activeMDIEditor();   //does not necessarily have the focus

    bool hasTextEditor = ( pe != 0 );
    bool hasMDIEditor = ( pmdi != 0 );
    bool isMDITextEditor = ( hasMDIEditor && pmdi == pe );

    setWindowModified( isMDITextEditor && pe->document()->isModified() );
    action_Save->setEnabled( isMDITextEditor && pe->document()->isModified() );

    action_Undo->setEnabled( hasTextEditor && pe->document()->isUndoAvailable());
    action_Redo->setEnabled( hasTextEditor && pe->document()->isRedoAvailable());

    if ( hasTextEditor )
    {
        fontChanged( pe->font() );
        colorChanged( pe->textColor() );
        alignmentChanged( pe->alignment() );
    }

    std::cout << "InitialUpdateActions::isMDITextEditor" << isMDITextEditor << std::endl;
}

void QbrtMainWindow::UpdateEditorsActions()
{
    Editor *pe = activeTextEditor();

    bool hasEditor = ( pe != 0 );
    bool hasSelection = hasEditor && pe->textCursor().hasSelection();
    bool isReadOnly = hasEditor && pe->isReadOnly();

    UpdateFileActions();

    //the automatic behaviour does nto seem to be enough...
    //a->setEnabled(d->doc->isUndoAvailable());
    //action_Undo->setEnabled( hasEditor && pe->document()->isUndoAvailable() );
    //a->setEnabled(d->doc->isRedoAvailable());
    //action_Redo->setEnabled( hasEditor && pe->document()->isRedoAvailable() );

    //a = menu->addAction(tr("Cu&t") + ACCEL_KEY(QKeySequence::Cut), this, SLOT(cut()));
    //a->setEnabled(d->cursor.hasSelection());
    action_Cut->setEnabled( hasSelection );

    //a = menu->addAction(tr("&Copy") + ACCEL_KEY(QKeySequence::Copy), this, SLOT(copy()));
    //a->setEnabled(d->cursor.hasSelection());
    action_Copy->setEnabled( hasSelection );

    //a = menu->addAction(tr("&Paste") + ACCEL_KEY(QKeySequence::Paste), this, SLOT(paste()));
    //a->setEnabled(canPaste());
    action_Paste->setEnabled( hasEditor && !isReadOnly && pe->canPaste() );

    //a = menu->addAction(tr("Delete"), this, SLOT(_q_deleteSelected()));
    //a->setEnabled(d->cursor.hasSelection());
    action_Delete->setEnabled( hasSelection );

    //a = menu->addAction(tr("Select All") + ACCEL_KEY(QKeySequence::SelectAll), this, SLOT(selectAll()));
    //a->setEnabled(!d->doc->isEmpty());
    action_Select_All->setEnabled( hasEditor && !pe->document()->isEmpty() );

    action_Read_Only->setChecked( isReadOnly );
    std::cout << "UpdateEditorsActions::hasEditor" << hasEditor << std::endl;
}



///////////////////////////////////////////////////////////////////////////////////////////
//                                  EDIT ACTIONS
///////////////////////////////////////////////////////////////////////////////////////////


void QbrtMainWindow::on_action_Undo_triggered()
{
    if ( Editor *pe = activeTextEditor() )
        pe->undo();
}

void QbrtMainWindow::on_action_Redo_triggered()
{
    if ( Editor *pe = activeTextEditor() )
        pe->redo();
}


void QbrtMainWindow::on_action_Cut_triggered()
{
    if ( Editor *pe = activeTextEditor() )
        pe->cut();
}

void QbrtMainWindow::on_action_Copy_triggered()
{
    if ( Editor *pe = activeTextEditor() )
        pe->copy();
}

void QbrtMainWindow::on_action_Paste_triggered()
{
    if ( Editor *pe = activeTextEditor() )
        pe->paste();
}

void QbrtMainWindow::clipboardDataChanged()
{
#ifndef QT_NO_CLIPBOARD
    if (const QMimeData *md = QApplication::clipboard()->mimeData())
        action_Paste->setEnabled( md->hasText() );
#endif
}

void QbrtMainWindow::on_action_Delete_triggered()
{
    if ( Editor *pe = activeTextEditor() )
        pe->textCursor().removeSelectedText();        //from Qt source code
}

void QbrtMainWindow::on_action_Select_All_triggered()
{
    if ( Editor *pe = activeTextEditor() )
        pe->selectAll();
}

void QbrtMainWindow::on_action_Read_Only_triggered()
{
    if ( Editor *pe = activeTextEditor() ) {
        QAction *action = qobject_cast<QAction *>(sender());
        pe->setReadOnly( action->isChecked() );
        action_Paste->setEnabled( !action->isChecked() );
    }
}



///////////////////////////////////////////////////////////////////////////////////////////
//                                  TOOLS->OPTIONS ACTIONS
///////////////////////////////////////////////////////////////////////////////////////////

void QbrtMainWindow::UpdateToolsActions() {   /* business logic */  }

void QbrtMainWindow::on_action_Options_triggered()
{
    SettingsDlg dlg( this );
    dlg.exec();
}


///////////////////////////////////////////////////////////////////////////////////////////
//                                  HELP ACTIONS
///////////////////////////////////////////////////////////////////////////////////////////


void QbrtMainWindow::on_action_About_triggered()
{
    static QSettings& settings = GetBackServices().GetAppSettings();
    QMessageBox::about(this, tr("About Development Editor"),
            tr("<h2>Editor 1.1</h2>"
               "<p>Copyright &copy; 2011 Software Inc."
               "<p>Development Editor settings "
               "file: <p>") + settings.fileName() );
}



void QbrtMainWindow::on_action_Find_triggered()
{
    if (!m_FindDialog) {
        m_FindDialog = new FindDialog(this);
        connect( m_FindDialog, SIGNAL(findNext(const QString &, Qt::CaseSensitivity)), this, SLOT( findNext(const QString &, Qt::CaseSensitivity)) );
        connect( m_FindDialog, SIGNAL(findPrevious(const QString &, Qt::CaseSensitivity)), this, SLOT( findPrevious(const QString &, Qt::CaseSensitivity)) );
    }
    m_FindDialog->show();
    m_FindDialog->raise();
    m_FindDialog->activateWindow();
}

//enum FindFlag
//{
//    FindBackward        = 0x00001,
//    FindCaseSensitively = 0x00002,
//    FindWholeWords      = 0x00004
//};

void QbrtMainWindow::findNext( const QString &str, Qt::CaseSensitivity cs )
{
    if ( Editor *pe = activeTextEditor() )
        if ( !pe->find( str, cs == Qt::CaseSensitive ? QTextDocument::FindCaseSensitively : (QTextDocument::FindFlags)0 ) )
        {
            QApplication::beep();
            SimpleMsgBox( QString("String \"") + str + "\" not found." );
        }
}

void QbrtMainWindow::findPrevious( const QString &str, Qt::CaseSensitivity cs )
{
    if ( Editor *pe = activeTextEditor() )
        if ( !pe->find( str, QTextDocument::FindBackward | ( cs == Qt::CaseSensitive ? QTextDocument::FindCaseSensitively : (QTextDocument::FindFlags)0  ) ) )
        {
            QApplication::beep();
            SimpleMsgBox( QString("String \"") + str + "\" not found." );
        }
}

void QbrtMainWindow::on_action_Find_Next_triggered()
{
    findNext( m_FindDialog->FindText(), m_FindDialog->CaseSensitive() ? Qt::CaseSensitive : Qt::CaseInsensitive );
}

void QbrtMainWindow::on_action_Find_Previous_triggered()
{
    findPrevious( m_FindDialog->FindText(), m_FindDialog->CaseSensitive() ? Qt::CaseSensitive : Qt::CaseInsensitive  );
}




///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_QbrtMainWindow.cpp"
