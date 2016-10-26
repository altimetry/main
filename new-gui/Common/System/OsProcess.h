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
#if !defined COMMON_SYSTEM_OS_PROCESS_H
#define COMMON_SYSTEM_OS_PROCESS_H


#if defined (Q_OS_WIN)
#include <Windows.h>
#endif


#include <QProcess>



/////////////////////////////////////////////////////////////////////////////////
//							Brat Process Class
/////////////////////////////////////////////////////////////////////////////////


class COsProcess : public QProcess
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

protected:
	using base_t = QProcess;

	typedef long long uid_t;

	using log_file_t = void (COsProcess::*)( const std::string &msg );


	// static members

public:

	static const QString& ProcessErrorMessage( QProcess::ProcessError error );


	// instance data

protected:
	bool mSync = false;
	std::string mId;		//tasks
	std::string mAt;		//tasks
	std::string mName;		//tasks & operations
	std::string mStatus;	//tasks
	std::string mCmdLine;	//tasks & operations
	std::string mLogPath;	//tasks

	void *mUserData = nullptr;

	CLogFile mLog;
	log_file_t mLogPtr = nullptr;


	// construction / destruction 

public:
	COsProcess( bool sync, const std::string &name, QObject *parent, const std::string &cmd, void *user_data = nullptr,
		const std::string &id = "",
		const std::string &at = "",
		const std::string &status = "",
		const std::string &log_file = ""
		);

	virtual ~COsProcess();


	// access

	void* UserData() const { return mUserData; }

	const std::string& CmdLine() const { return mCmdLine; };
	virtual void SetCmd( const std::string& value ) { mCmdLine = value; };

	virtual const std::string& Name() const { return mName; };
	virtual void SetName( const std::string& value ) { mName = value; };

	const std::string& Id() const { return mId; }
	const std::string& At() const { return mAt; }
	const std::string& Status() const { return mStatus; }
	const std::string& LogPath() const { return mLogPath; }

	long long Pid() const
	{
		return
#if defined (Q_OS_WIN)
			pid() ? pid()->dwProcessId : 0;
#else
            pid();
#endif
	}

	std::string PidString() const {	return n2s( Pid() ); }


	// operate

	virtual void Kill();

	virtual void Execute( bool detached = false );


	void Log( const std::string &msg )
	{
		assert__( mLogPtr );

		(this->*mLogPtr)( msg );
	}

protected:
	void RealLog( const std::string &msg )
	{
		mLog.Log( msg );
	}

	void PseudoLog( const std::string & )
	{}
};




#endif	//COMMON_SYSTEM_OS_PROCESS_H
