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
#include "new-gui/brat/stdafx.h"


#include "new-gui/Common/QtUtils.h"

#include "LoginDialog.h"



void LoginDialog::Create()
{
	// set up the layout
	QGridLayout *formGridLayout = new QGridLayout( this );

	mUserNameCombo = new QComboBox( this );
	mUserNameCombo->setEditable( true );

	// don't echo passwords
	mPasswordEdit = new QLineEdit( this );
	mPasswordEdit->setEchoMode( QLineEdit::Password );

	mUserNameLabel = new QLabel( this );
	mPasswordLabel = new QLabel( this );
	mUserNameLabel->setText( tr( "Username" ) );
	mUserNameLabel->setBuddy( mUserNameCombo );
	mPasswordLabel->setText( tr( "Password" ) );
	mPasswordLabel->setBuddy( mPasswordEdit );

	mButtons = new QDialogButtonBox( this );
	mButtons->addButton( QDialogButtonBox::Ok );
	mButtons->addButton( QDialogButtonBox::Cancel );
	mButtons->button( QDialogButtonBox::Ok )->setText( tr("Install") );
	mButtons->button( QDialogButtonBox::Cancel )->setText( tr("Abort") );

	//layout

	formGridLayout->addWidget( mUserNameLabel, 0, 0 );
	formGridLayout->addWidget( mUserNameCombo, 0, 1 );
	formGridLayout->addWidget( mPasswordLabel, 1, 0 );
	formGridLayout->addWidget( mPasswordEdit, 1, 1 );
	formGridLayout->addWidget( mButtons, 2, 0, 1, 2 );

	setLayout( formGridLayout );

	Wire();
}


void LoginDialog::Wire()
{
	connect( mButtons->button( QDialogButtonBox::Cancel ), SIGNAL ( clicked() ), this, SLOT ( close() ) );
	connect( mButtons->button( QDialogButtonBox::Ok ), SIGNAL ( clicked() ), this, SLOT ( AcceptLoginHandler() ) );
}


LoginDialog::LoginDialog( QString title, QWidget *parent ) //parent = nullptr 
	: base_t( parent )
{
	Create();
	setWindowTitle( title );
	setModal( true );
}


void LoginDialog::SetUsername( const QString &username )
{
	bool found = false;
	for( int i = 0; i < mUserNameCombo->count() && ! found ; i++ )
		if( mUserNameCombo->itemText( i ) == username )
		{
			mUserNameCombo->setCurrentIndex( i );
			found = true;
		}

	if( !found )
	{
		int index = mUserNameCombo->count();			qDebug() << "Select username " << index;
		mUserNameCombo->addItem( username );
		mUserNameCombo->setCurrentIndex( index );
	}

	// place the focus on the password field
	mPasswordEdit->setFocus();
}


void LoginDialog::SetPassword( const QString &password )
{
	mPasswordEdit->setText( password );
}


void LoginDialog::AcceptLoginHandler()
{
	mUsername = mUserNameCombo->currentText();
	mPassword = mPasswordEdit->text();

	accept();
}


void LoginDialog::SetUsernamesList( const QStringList &usernames )
{
	mUserNameCombo->addItems( usernames );
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_LoginDialog.cpp"
