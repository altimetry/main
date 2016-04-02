#include "stdafx.h"

#include "BratApplication.h"
#include "BratMainWindow.h"

#include "GUI/DisplayWidgets/TextWidget.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBratMainWindow::FillStatusBarExperimental()
{
	//remove borders from children under Windows
	//statusBar()->setStyleSheet( "QStatusBar::item {border: none;}" );

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
	mThreadsButton->setIcon( CBratApplication::getThemeIcon( "mActionCancelEdits.svg" ) );
	mThreadsButton->setToolTip( tr( "Background Tasks" ) );
	mThreadsButton->setWhatsThis( tr( "BackgroundTasks" ) );
	mThreadsButton->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
	mThreadsButton->setObjectName( "mThreadsButton" );
	mThreadsButton->setCheckable( true );
	mThreadsButton->hide();
	statusBar()->addPermanentWidget( mThreadsButton, 0 );

	//connect( mThreadsButton, SIGNAL( toggled( bool ) ), this, SLOT( cancelStuff( bool ) ) );
}


CAbstractDisplayEditor *CBratMainWindow::ActiveMDIEditor()
{
    //QMdiSubWindow *subWindow = mdiArea->activeSubWindow();
    //if (subWindow)
    //    return dynamic_cast< CEditorBase* >(subWindow->widget());
    return nullptr;
}

template< class EDITOR >
EDITOR* CBratMainWindow::ActiveEditor()
{
    //QWidget *pw = QApplication::focusWidget(); TODO return this only if matches a known window

    //default to MDI subWindows
    //
    return dynamic_cast< EDITOR* >( ActiveMDIEditor() );
}



///////////////////////////////////////////////////////////////////////////////////////////
//                                  EDIT ACTIONS
///////////////////////////////////////////////////////////////////////////////////////////


void CBratMainWindow::on_action_Cut_triggered()
{
    BRAT_NOT_IMPLEMENTED
    //if ( CTextWidget *pe = activeTextEditor() )
    //    pe->cut();
}
void CBratMainWindow::on_action_Copy_triggered()
{
    BRAT_NOT_IMPLEMENTED
    //if ( CTextWidget *pe = activeTextEditor() )
    //    pe->copy();
}
void CBratMainWindow::on_action_Paste_triggered()
{
    BRAT_NOT_IMPLEMENTED
    //if ( CTextWidget *pe = activeTextEditor() )
    //    pe->paste();
}
void CBratMainWindow::on_action_Undo_triggered()
{
    BRAT_NOT_IMPLEMENTED
    //if ( CTextWidget *pe = activeTextEditor() )
    //    pe->undo();
}
void CBratMainWindow::on_action_Redo_triggered()
{
    BRAT_NOT_IMPLEMENTED
    //if ( CTextWidget *pe = activeTextEditor() )
    //    pe->redo();
}
void CBratMainWindow::on_action_Delete_triggered()
{
    BRAT_NOT_IMPLEMENTED
    //if ( CTextWidget *pe = activeTextEditor() )
    //    pe->textCursor().removeSelectedText();        //from Qt source code
}
void CBratMainWindow::on_action_Select_All_triggered()
{
    BRAT_NOT_IMPLEMENTED
    //if ( CTextWidget *pe = activeTextEditor() )
    //    pe->selectAll();
}









void CBratMainWindow::cursorPositionChanged()
{
	BRAT_NOT_IMPLEMENTED
    //if ( CTextWidget *editor = activeTextEditor() )
    //    alignmentChanged( editor->alignment() );
}

void CBratMainWindow::clipboardDataChanged()
{
#ifndef QT_NO_CLIPBOARD
    if (const QMimeData *md = QApplication::clipboard()->mimeData())
        action_Paste->setEnabled( md->hasText() );
#endif
}
