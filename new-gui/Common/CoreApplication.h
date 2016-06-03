#ifndef COMMON_CORE_APPLICATION_H
#define COMMON_CORE_APPLICATION_H


#if QT_VERSION >= 0x050000
#include <QtWidgets/QApplication>
#else
#include <QtGui/QApplication>
#endif

#include "new-gui/Common/QtStringUtils.h"


template< class APPLICATION, class SETTINGS >
class CCoreApplication : public APPLICATION
{
	//////////////////////////////////////
	//	types & friends
	//////////////////////////////////////

	typedef APPLICATION base_t;


	//////////////////////////////////////
	//	static members
	//////////////////////////////////////


	static bool smPrologueCalled;					//initialized in Prologue

	static CApplicationPaths *smApplicationPaths;	//initialized in Prologue

	static const PythonEngine *sm_pe;


	static void CheckOpenGL( bool extended = false );

public:

	// Calling this in main is mandatory
	//	
	static void Prologue( int argc, char *argv[] );


    static int OffGuiErrorDialog( int error_type, char const *error_msg );


protected:
	//	This is accurate only if(when) no style sheet was also assigned.
	//	External clients should not rely on this, and use the name in options.
    //	Internal code can only rely on this before assigning a style sheet.
	// 
	static QString getCurrentStyleName()
	{
		return style()->objectName().toLower();
	}


	//////////////////////////////////////
    //	instance data
	//////////////////////////////////////

    SETTINGS mSettings;

	bool mLeakExceptions = false;

	QSplashScreen *mSplash = nullptr;



	//////////////////////////////////////
	//	construction / destruction 
	//////////////////////////////////////

	QString mDefaultAppStyle;

	void CheckRunMode();
	void CreateSplash();
public:
    CCoreApplication( int &argc, char **argv, bool GUIenabled, QString customConfigPath = QString() );

	virtual ~CCoreApplication();


	//////////////////////////////////////
	//	access
	//////////////////////////////////////

    SETTINGS& Settings() { return  mSettings; }

    const SETTINGS& Settings() const { return  mSettings; }


	void LeakExceptions( bool leak )
	{
		mLeakExceptions = leak;
	}


	bool OperatingInDisplayMode() const { return mOperatingInDisplayMode; }

	void ShowSplash( const std::string &msg, bool disable_events = false ) const;
	void EndSplash( QWidget *w );
	bool SplashAvailable() const { return mSplash != nullptr; }


	//////////////////////////////////////
	//	operations
	//////////////////////////////////////

	virtual bool notify( QObject * receiver, QEvent * event ) override
	{
		if ( mLeakExceptions )
		{
			return QApplication::notify( receiver, event );
		}
		return base_t::notify( receiver, event );
	}


	//not tested enough
	//
	void Restart()
	{
		quit();
		QProcess::startDetached( arguments()[ 0 ], arguments() );
	}
};



#endif // COMMON_CORE_APPLICATION_H
