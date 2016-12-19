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
#include <QSharedMemory>


#include "new-gui/Common/System/Service/QtService"
#include "RadsSettings.h"


class QLocalServer;



//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//						RadsClient
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

class CRadsClient : public QObject
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

	CRadsSettings &mSettings;
	QTimer mTimer;
	bool mDisabled = false;
	const std::string mRadsServerAddress;
	COsProcess *mCurrentProcess = nullptr;
	QSharedMemory mSharedMemory;
	QLocalServer *mSocketServer = nullptr;
	std::vector< QLocalSocket* > mSocketConnections;
	bool mBroadcasting = false;


	/////////////////////////////
	//	Construction /Destruction
	/////////////////////////////

public:
	CRadsClient( CRadsSettings &settings, QObject* parent = nullptr );

	virtual ~CRadsClient();


	/////////////////////////////
	//	
	/////////////////////////////

	bool IsListening() const
	{
		return mTimer.isActive();
	}


protected:

	void CleanRsyncProcess( bool kill = false );


public slots:

	void Pause();

	void Resume();

	void ForceSynchronize();

	void ForceRsyncEnd();


private slots:

	void DelaySaveConfig();

	bool Synchronize( bool force = false );

	void HandleSynchronize()
	{
		Synchronize();
	}

	void HandleUpdateOutput();
	void HandleProcessFinished( int exit_code, QProcess::ExitStatus exitStatus );

	void HandleRsyncError( QProcess::ProcessError error );

	void HandleNewConnection();
	void HandleSocketDisconnected();
	bool BroadcastRsyncStatusToSocketClients( const char *msg );
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

	using base_t = QtService< QCoreApplication >;


	/////////////////////////////
	//	Instance Data
	/////////////////////////////

	CRadsClient *mRadsClient = nullptr;
	CRadsSettings mSettings;


	/////////////////////////////
	//	Construction /Destruction
	/////////////////////////////

public:
	CRadsService( int argc, char *argv[], const CApplicationStaticPaths &paths, bool auto_start );
	
	virtual ~CRadsService();


	/////////////////////////////
	//	
	/////////////////////////////

	virtual int exec() override;

protected:
	virtual void logMessage(const QString &message, MessageType type = Success,
		int id = 0, uint category = 0, const QByteArray &data = QByteArray() ) override;

	virtual void start() override;

	virtual void stop() override;

	virtual void pause() override;

	virtual void resume() override;

	virtual void processCommand( int code ) override;
};



#endif		// __RADS_SERVICE_H__
