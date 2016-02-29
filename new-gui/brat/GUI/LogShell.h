#ifndef GUI_LOG_SHELL_H
#define GUI_LOG_SHELL_H

#include <QFrame>
#include <QTimer>
#include <QHash>
#include <QMutex>

#include <osg/Notify>
#include <qgsmessagelog.h>


QT_BEGIN_NAMESPACE
class CTextWidget;
class QAction;
class QComboBox;
class QLineEdit;
class QCheckBox;
QT_END_NAMESPACE


class CLogShell : public QFrame
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

	QHash< osg::NotifySeverity, QColor >	mSeverityToColorTable;
	QHash< osg::NotifySeverity, QString >	mSeverityToPromptTable;

	// construction / destruction

	QGroupBox* CreateDebugWidgets( const std::string *names, size_t names_size );

public:

	CLogShell( QWidget *parent = nullptr );

	virtual ~CLogShell()
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
