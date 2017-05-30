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
#ifndef GUI_LOG_SHELL_H
#define GUI_LOG_SHELL_H

#include <QFrame>
#include <QTimer>
#include <QHash>
#include <QMutex>


QT_BEGIN_NAMESPACE
class CTextWidget;
class QAction;
class QComboBox;
class QLineEdit;
class QCheckBox;
QT_END_NAMESPACE


#include "new-gui/Common/ApplicationLoggerInterface.h"




class CBasicLogShell : public QFrame
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

	Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif


	// types & friends

	using base_t = QFrame;

	// static members

    static const size_t smDefaultMaxMsgsPerSec = 100;
    static size_t smMaxMsgsPerSec;

	// instance data

	QCheckBox	*mEnabledCheckBox = nullptr;
	CTextWidget	*mEditor = nullptr;
	QComboBox	*mLogLevelCombo = nullptr;
    QLineEdit   *mMaxMsgs = nullptr;
	bool		mEnabled = true;
    QTimer      mTimer;
    size_t      mMsgCounter = 0;

	ApplicationLoggerInterface &mLogger;
	const QHash< int, QColor >	&mSeverityToColorTable;
	const QHash< int, QString >	&mSeverityToPromptTable;


	// construction / destruction

	QGroupBox* CreateDebugWidgets( const std::vector<std::string> &level_names );

public:

	CBasicLogShell( ApplicationLoggerInterface &logger, QWidget *parent = nullptr );

	virtual ~CBasicLogShell()
	{}

	// remaining operations

	void Deactivate( bool yes );

protected:
	void Append( QString str, QColor color, bool removeNewlineChar );

public slots:

	// Both slots expect OSG severity levels, Qt level must be
	//	translated by the caller
	//
	void OsgNotifySlot( int severity, QString message );

    void QtNotifySlot( int severity, QString message );

private slots:
	void SetOsgLogLevel( int index );

	void Enable( bool yes );
	void UpdateOsgLogLevel( int index );
    void UpdateMsgsPerSecCounter();
    void UpdateMaxMsgsPerSecValue();
};




#endif		//GUI_LOG_SHELL_H
