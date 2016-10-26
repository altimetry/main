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
#ifndef __RADS_SERVICE_H__
#define __RADS_SERVICE_H__

#include <QProcess>


#include "new-gui/Common/GUI/ApplicationUserPaths.h"
#include "new-gui/Common/System/Service/QtService"


//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//						RadsFaemon
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

class CRadsDaemon : public QObject
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

	using base_t = QObject;


	/////////////////////////////
	//	Instance Data
	/////////////////////////////

	const CApplicationStaticPaths &mPaths;
	QTimer mTimer;
	bool mDisabled = false;
	COsProcess *mCurrentProcess = nullptr;


	/////////////////////////////
	//	Construction /Destruction
	/////////////////////////////

public:
	CRadsDaemon( const CApplicationStaticPaths &paths, QObject* parent = nullptr )
		: base_t( parent )
		, mPaths( paths ) 
		, mTimer( this )
		, mDisabled( false )
	{
		connect( &mTimer, SIGNAL( timeout() ), this, SLOT( Synchronize() ) );
		mTimer.start( /*24 * 60 * 60 * */10000 );
	}


	virtual ~CRadsDaemon();


	/////////////////////////////
	//	
	/////////////////////////////

	bool IsListening() const
	{
		return mTimer.isActive();
	}

	void Pause()
	{
		mDisabled = true;
	}

	void Resume()
	{
		mDisabled = false;
	}

	
private slots:

	bool Synchronize();

	void HandleUpdateOutput();
	void HandleProcessFinished( int exit_code, QProcess::ExitStatus exitStatus );

	void RsyncError( QProcess::ProcessError error );
};




//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//						RadsService
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////


class CRadsService : public QtService<QCoreApplication>
{
	/////////////////////////////
	//	Types
	/////////////////////////////

	using base_t = QtService<QCoreApplication>;


	/////////////////////////////
	//	Instance Data
	/////////////////////////////

	const CApplicationStaticPaths &mPaths;
	CRadsDaemon *mDaemon = nullptr;


	/////////////////////////////
	//	Construction /Destruction
	/////////////////////////////

public:
	CRadsService( int argc, char *argv[], const CApplicationStaticPaths &paths )
		: base_t( argc, argv, RADS_SERVICE_NAME )
		, mPaths( paths ) 
	{
		setServiceDescription( "Synchronizes data between RADS sever and local BRAT data repository" );
		setServiceFlags( QtServiceBase::CanBeSuspended );
	}
	
	virtual ~CRadsService()
	{}


	/////////////////////////////
	//	
	/////////////////////////////

protected:
	virtual void start() override;

	virtual void pause() override
	{
		mDaemon->Pause();
	}

	virtual void resume() override
	{
		mDaemon->Resume();
	}
};



#endif		// __RADS_SERVICE_H__
