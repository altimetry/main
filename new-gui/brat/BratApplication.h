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
#ifndef BRAT_APPLICATION_H
#define BRAT_APPLICATION_H


#if QT_VERSION >= 0x050000
#include <QtWidgets/QApplication>
#else
#include <QtGui/QApplication>
#endif

#include <QLocalSocket>

#include <qgsapplication.h>

#include "libbrathl/List.h"
#include "libbrathl/FileParams.h"

#include "common/QtStringUtils.h"
#include "process/rads/RadsSettings.h"
#include "new-gui/Common/System/Service/qtservice.h"

#include "BratSettings.h"



class PythonEngine;
class CModel;


using namespace brathl;



class CBratApplication : public QgsApplication
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif


	//////////////////////////////////////
	//	types & friends
	//////////////////////////////////////

	typedef QgsApplication base_t;

	friend class CApplicationSettingsDlg;

    
	enum ERsyncStatus
	{
		eRsyncUnknown,
		eRsyncRunnig,
		eRsyncStopped,
	};


public:
    
	enum ERadsNotification
    {
		eNotificationUnknown = eRsyncUnknown, eFirstRsyncNotification = eNotificationUnknown,
        eNotificationRsyncRunnig = eRsyncRunnig,
        eNotificationRsyncStopped = eRsyncStopped, eLastRsyncNotification = eNotificationRsyncStopped,
		eNotificationConfigSaved,
	};
    


private:
    
    //////////////////////////////////////
	//	static members
	//////////////////////////////////////


	static bool smPrologueCalled;					//initialized in Prologue

	static CApplicationPaths *smApplicationPaths;	//initialized in Prologue


	static void RegisterAsQtTypes();	//called in Prologue; see respective Q_DECLARE_METATYPE after class declaration

	static void CheckOpenGL( bool extended = false );


	static ERadsNotification Convert( ERsyncStatus status )
	{
		//As long as ERadsNotification is a superset of ERsyncStatus,
		//	where common elements have common definitions, this is pacific
		//
		return (ERadsNotification)status;	
	}

	static ERsyncStatus Convert( ERadsNotification notification )
	{
		if ( notification >= eFirstRsyncNotification && notification <= eLastRsyncNotification )
			return (ERsyncStatus)notification;

		assert__( false );

		return eRsyncUnknown;
	}

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
	static QString CurrentStyleName()
	{
		return style()->objectName().toLower();
	}


	//////////////////////////////////////
    //	instance data
	//////////////////////////////////////

    CBratSettings mSettings;
	CModel *mModel = nullptr;

	bool mLeakExceptions = false;

	bool mOperatingInDisplayMode = false;
    bool mOperatingInInstantPlotSaveMode = false;
	QtServiceController mServiceController;
    QLocalSocket *mSocket = nullptr;
	ERsyncStatus mRsyncStatus = eRsyncUnknown;
	CSharedRadsSettings mRadsServiceSettings;

	QSplashScreen *mSplash = nullptr;



	//////////////////////////////////////
	//	construction / destruction 
	//////////////////////////////////////

	QString mDefaultAppStyle;

	void CheckRunMode();
	void CreateSplash();
public:
    CBratApplication( int &argc, char **argv, bool GUIenabled, QString customConfigPath = QString() );

	virtual ~CBratApplication();


	//////////////////////////////////////
	//	access
	//////////////////////////////////////

    CBratSettings& Settings() { return  mSettings; }

    const CBratSettings& Settings() const { return  mSettings; }

	
	const CModel& DataModel() const 
	{ 
		return const_cast<CBratApplication*>( this )->DataModel();
	}

	CModel& DataModel()
	{ 
		assert__( mModel );

		return *mModel; 
	}


	void LeakExceptions( bool leak )
	{
		mLeakExceptions = leak;
	}


	bool OperatingInDisplayMode() const { return mOperatingInDisplayMode; }
    bool OperatingInInstantPlotSaveMode() const { return mOperatingInInstantPlotSaveMode; }

	void ShowSplash( const std::string &msg, bool disable_events = false ) const;
	void EndSplash( QWidget *w );
	bool SplashAvailable() const { return mSplash != nullptr; }

	bool InstallRadsService( QWidget *parent );
	bool UninstallRadsService();

	bool StartRadsService();
	bool StopRadsService();

	bool PauseRadsService();
	bool ResumeRadsService();

	bool SendRadsServiceCommand( int code );

	bool RsyncIsActive() const { return mRsyncStatus == eRsyncRunnig; }
	bool ResetRadsSocketConnection();
    
	const QtServiceController& RadsServiceController() const { return mServiceController; }

	const CSharedRadsSettings& RadsServiceSettings() const { return mRadsServiceSettings; }



	//////////////////////////////////////
	//	operations
	//////////////////////////////////////

protected:
    
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


    bool RegisterAlgorithms();


signals:
    void RadsNotification( ERadsNotification status );
    

public slots:
	void UpdateSettings();

    
protected slots:
    void HandleSocketReadyRead();
	void HandleLocalSocketError( QLocalSocket::LocalSocketError error );
};






Q_DECLARE_METATYPE( CBratApplication::ERadsNotification )




#endif // BRAT_APPLICATION_H
