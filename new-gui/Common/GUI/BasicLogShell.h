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


class CApplicationLoggerBase;




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

	const QHash< int, QColor >	mSeverityToColorTable;
	const QHash< int, QString >	mSeverityToPromptTable;

	CApplicationLoggerBase &mLogger;

	// construction / destruction

	QGroupBox* CreateDebugWidgets( const std::vector<std::string> &level_names );

public:

	CBasicLogShell( CApplicationLoggerBase &logger, const std::vector<std::string> &level_names, 
		const QHash< int, QColor > &severity2color_table, const QHash< int, QString > &severity2prompt_table, QWidget *parent = nullptr );

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
