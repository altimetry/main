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
#if !defined GUI_LOGIN_DIALOG_H
#define GUI_LOGIN_DIALOG_H


#include <QDialog>


/*!
'''Makes class LoginDialog a child to its parent, QDialog
*/
class LoginDialog : public QDialog
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

	Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif


	/////////////////////////////
	//	Types
	/////////////////////////////

	using base_t = QDialog;


	/////////////////////////////
	//	Instance Data
	/////////////////////////////

	QLabel *mUserNameLabel = nullptr;
	QLabel *mPasswordLabel = nullptr;
	QComboBox *mUserNameCombo = nullptr;
	QLineEdit *mPasswordEdit = nullptr;

	QDialogButtonBox *mButtons = nullptr;

	QString mUsername;
	QString mPassword;


	/////////////////////////////
	//	Construction /Destruction
	/////////////////////////////

	void Create();
	void Wire();

public:
	explicit LoginDialog( QString title, QWidget *parent = nullptr );

	virtual ~LoginDialog()
	{}


	/////////////////////////////
	//	
	/////////////////////////////

	const QString& Username() const { return mUsername; }
	void SetUsername( const QString& username, bool disable_user = false );

	const QString& Password() const { return mPassword; };
	void SetPassword( const QString& password );

	void SetUsernamesList( const QStringList& usernames );


public slots:

	void AcceptLoginHandler();
};




#endif	//GUI_LOGIN_DIALOG_H
