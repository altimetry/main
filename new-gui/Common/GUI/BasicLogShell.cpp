#include "stdafx.h"

#include <cstdio>
#include <iostream>

#include <QTextEdit>
#include <QVBoxLayout>

#include "../ApplicationLogger.h"

#include "TextWidget.h"
#include "BasicLogShell.h"


//static
size_t CBasicLogShell::smMaxMsgsPerSec = smDefaultMaxMsgsPerSec;


//enum NotifySeverity 
//{
//    ALWAYS=0,
//    FATAL=1,
//    WARN=2,
//    NOTICE=3,
//    INFO=4,
//    DEBUG_INFO=5,
//    DEBUG_FP=6
//};


QGroupBox* CBasicLogShell::CreateDebugWidgets( const std::vector<std::string> &level_names )
{
	assert__( (int)level_names.size() <= mSeverityToColorTable.size() && (int)level_names.size() <= mSeverityToPromptTable.size() );

	// create widgets

	mEnabledCheckBox = new QCheckBox( "Enabled", this );
	mEnabledCheckBox->setChecked( true );
    mEnabledCheckBox->setFocusPolicy( Qt::NoFocus );

    QLabel *MaxMsgs_label = new QLabel( "Maximum messages per second" );
    mMaxMsgs = new QLineEdit( this );
    mMaxMsgs->setText( n2s<std::string>( smMaxMsgsPerSec ).c_str() );
    mMaxMsgs->setToolTip( "Default: " + t2q( n2s<std::string >( smDefaultMaxMsgsPerSec ) ) + ". A big number can hang the application" );
    MaxMsgs_label->setBuddy( mMaxMsgs );
    QPushButton *set_button = new QPushButton( "Set" );
    set_button->setFocusPolicy( Qt::NoFocus );

	mLogLevelCombo = new QComboBox( this );
	FillCombo( mLogLevelCombo, level_names, mLogger.NotifyLevel(), true );
    mLogLevelCombo->setFocusPolicy( Qt::NoFocus );

	// wire & layout

	connect( mEnabledCheckBox, SIGNAL( toggled( bool ) ), this, SLOT( Enable( bool ) ) );
    connect( set_button, SIGNAL( clicked() ), this, SLOT( UpdateMaxMsgsPerSecValue() ) );
    connect( mLogLevelCombo, SIGNAL( currentIndexChanged ( int ) ), this, SLOT( UpdateOsgLogLevel( int ) ) );

	QGroupBox *box = CreateGroupBox( ELayoutType::Horizontal, 
	{ 
		mEnabledCheckBox, nullptr, MaxMsgs_label, mMaxMsgs, set_button, nullptr, mLogLevelCombo 
	},
	"", nullptr, 2, 2, 2, 2, 2 );

	box->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Maximum );

	return box;
}


CBasicLogShell::CBasicLogShell( CApplicationLoggerBase &logger, const std::vector<std::string> &level_names, const QHash< int, QColor > &severity2color_table, const QHash< int, QString >	&severity2prompt_table, QWidget *parent ) 
	: base_t( parent )
	, mSeverityToColorTable( severity2color_table )
	, mSeverityToPromptTable( severity2prompt_table )
	, mLogger( logger )
{
    assert__( mSeverityToColorTable.size() == mSeverityToPromptTable.size() );          Q_UNUSED( level_names );    //for release builds

	//// log level colors and labels

	//static std::string names[ ENotifySeverity_size ] =
	//{
	//	"ALWAYS",
	//	"FATAL",
	//	"WARN",
	//	"NOTICE",
	//	"INFO",
	//	"DEBUG_INFO",
	//	"DEBUG_FP"
	//};
 //   static const DEFINE_ARRAY_SIZE( names ); Q_UNUSED(names_size);

	//mSeverityToColorTable.reserve( 10 );

	//mSeverityToColorTable.insert( osg::ALWAYS,		QColor( 0,255,0, 127 ) );
	//mSeverityToColorTable.insert( osg::FATAL,		QColor( 255, 0, 0, 127 ) );
	//mSeverityToColorTable.insert( osg::WARN,		QColor( 255, 255, 0, 127 ) );
	//// debug levels not used by brat logger
	//mSeverityToColorTable.insert( osg::NOTICE,		QColor( 255, 255, 255, 127 ) );
	//mSeverityToColorTable.insert( osg::INFO,		QColor( 255, 255, 255, 127 ) );
	//mSeverityToColorTable.insert( osg::DEBUG_INFO,	QColor( 255, 255, 255, 127 ) );
	//mSeverityToColorTable.insert( osg::DEBUG_FP,	QColor( 255, 255, 255, 127 ) );
	//	
	//mSeverityToPromptTable.reserve( 10 );

	//mSeverityToPromptTable.insert( osg::ALWAYS,		"[INFO] " );
	//mSeverityToPromptTable.insert( osg::FATAL,		"[FATAL] " );
	//mSeverityToPromptTable.insert( osg::WARN,		"[WARN] " );
	//mSeverityToPromptTable.insert( osg::NOTICE,		"[OSG_NOTICE] " );
	//mSeverityToPromptTable.insert( osg::INFO,		"[OSG_INFO] " );
	//mSeverityToPromptTable.insert( osg::DEBUG_INFO,	"[OSG_DEBUG_INFO] " );
	//mSeverityToPromptTable.insert( osg::DEBUG_FP,	"[OSG_DEBUG_FP] " );


	// log editor

	mEditor = new CTextWidget( this );
	mEditor->setFrameStyle( QFrame::NoFrame );
    mEditor->setReadOnly( true );
	mEditor->setLineWrapMode( QTextEdit::NoWrap );
	mEditor->setToolEditor( true );
	mEditor->document()->setMaximumBlockCount( 1000 );


#ifdef Q_WS_MAC
	QFont font("Courier", 12);
#else
	QFont font("Monospace", 8);
#endif
	font.setStyleHint( QFont::TypeWriter );
	mEditor->setFont( font );

	// layout

	std::vector<QObject*> v;

#if defined(DEBUG)

	v.push_back( CreateDebugWidgets( level_names ) );
#endif

	v.push_back( mEditor );
	LayoutWidgets( Qt::Vertical, v, this, 0, 0, 0, 0, 0 );

	setFocusPolicy( Qt::StrongFocus );
	setFrameStyle( QFrame::StyledPanel );

    connect( &mTimer, SIGNAL( timeout() ), this, SLOT( UpdateMsgsPerSecCounter() ) );
    mTimer.start( 1000 );      //1 sec
}


void CBasicLogShell::UpdateMaxMsgsPerSecValue()
{
    QString s = mMaxMsgs->text();
    bool ok = true;
    size_t n = s.toULong(&ok);
    if ( !ok )
        SimpleMsgBox( "Wrong value for maximum number of log messages per second." );
    else
    if ( smMaxMsgsPerSec != n )
	{
        smMaxMsgsPerSec = (size_t)n;
        CApplicationLoggerBase::LogMsg( t2q( "Maximum number of log messages per second set to " + n2s<std::string>( smMaxMsgsPerSec ) ), QtDebugMsg );
    }
}


void CBasicLogShell::UpdateMsgsPerSecCounter()
{
    mMsgCounter = 0;
}


//	- directly answers to combo signal
//
void CBasicLogShell::UpdateOsgLogLevel( int index )
{
	mLogger.SetNotifyLevel( index );
}


//	- answers to logger signal
//
void CBasicLogShell::SetOsgLogLevel( int index )
{
	if ( mLogLevelCombo )
		mLogLevelCombo->setCurrentIndex( index );
}


void CBasicLogShell::Enable( bool yes )
{
	static const auto sheet = mEditor->styleSheet();

	mLogger.SetEnabled( yes );

	mEnabled = yes;

	mEditor->setStyleSheet( !mEnabled ? ( "color: dark-gray; background-color: gray; " + sheet ) : sheet );	
}
void CBasicLogShell::Deactivate( bool yes )
{
	if ( mEnabledCheckBox )
	{
		mEnabledCheckBox->setChecked( !yes );
		//Enable( !yes );	not needed: the line above triggers the signal
		mEnabledCheckBox->setDisabled( yes );
	}
	else
		Enable( !yes );	//needed: no check box to trigger signal
}


void CBasicLogShell::Append( QString str, QColor color, bool removeNewlineChar )
{
	if ( !mEnabled )
		return;

	mEditor->setTextColor( color );
	mEditor->append( removeNewlineChar ? str.remove('\n') : str );

	mEditor->MoveToEnd( false );
	mEditor->horizontalScrollBar()->setValue( 0 );
}


void CBasicLogShell::OsgNotifySlot( int severity, QString message )
{
	if ( ++mMsgCounter > smMaxMsgsPerSec )
		return;

	Append( 
		mSeverityToPromptTable[ severity ] + message, 
		mSeverityToColorTable[ severity ], 
		false );
}


void CBasicLogShell::QtNotifySlot( int severity, QString message )
{
	if ( severity > mLogger.NotifyLevel() )
		return;

	QString 
		msg = message, 

		prefix = QString( "%1\t" )
		.arg( QDateTime::currentDateTime().toString( Qt::ISODate ) );

	OsgNotifySlot( severity, msg.prepend( prefix ).replace( "\n", "\n\t\t\t" ) );
}




///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_BasicLogShell.cpp"
