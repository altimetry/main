/*
* This file is part of BRAT
*
* BRAT is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* BRAT is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include "stdafx.h"

#include <cstdio>
#include <iostream>

#include <QTextEdit>
#include <QVBoxLayout>

#include "TextWidget.h"
#include "BasicLogShell.h"


//static
size_t CBasicLogShell::smMaxMsgsPerSec = smDefaultMaxMsgsPerSec;


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


CBasicLogShell::CBasicLogShell( ApplicationLoggerInterface &logger, QWidget *parent ) 
	: base_t( parent )
	, mLogger( logger )
	, mSeverityToColorTable( mLogger.SeverityToColorTable() )
	, mSeverityToPromptTable( mLogger.SeverityToPromptTable() )
{
    assert__( mSeverityToColorTable.size() == mSeverityToPromptTable.size() );

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

	v.push_back( CreateDebugWidgets( mLogger.LevelNames() ) );
#endif

	v.push_back( mEditor );
	LayoutWidgets( Qt::Vertical, v, this, 0, 0, 0, 0, 0 );

	setFocusPolicy( Qt::StrongFocus );
	setFrameStyle( QFrame::StyledPanel );

	const char *QGIS_LOG_FILE = getenv( "QGIS_LOG_FILE" );
	if ( QGIS_LOG_FILE )
		mEditor->readFromFile( QGIS_LOG_FILE );

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
		mLogger.LogMessage( t2q( "Maximum number of log messages per second set to " + n2s<std::string>( smMaxMsgsPerSec ) ), QtDebugMsg );
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
