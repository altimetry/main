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
#include "stdafx.h"


#include "new-gui/Common/QtUtils.h"

#include "OsProcess.h"


/////////////////////////////////////////////////////////////////////////////////
//							Brat Process Class
/////////////////////////////////////////////////////////////////////////////////

//static 
const QString& COsProcess::ProcessErrorMessage( QProcess::ProcessError error )
{
	static const QString msgs[] =
	{
		"Failed To Start",
		"Crashed",
		"Timed out",
		"Read Error",
		"Write Error",
		"Unknown Error"
	};
	static const DEFINE_ARRAY_SIZE( msgs );

	static_assert( ( QProcess::ProcessError::UnknownError + 1 ) == msgs_size, "ProcessError enumerated values differ in size from their respective messages array." );

	assert__( error < (int)msgs_size );

	return msgs[ error ];
}



COsProcess::COsProcess( bool sync, const std::string& name, QObject *parent, const std::string& cmd, void *user_data,
		const std::string &id,
		const std::string &at,
		const std::string &status,
		const std::string &log_file	)		//user_data = nullptr, etc.

	: QProcess( parent )
	, mSync( sync )
	, mName( name )
	, mCmdLine( cmd )
	, mUserData( user_data )
	, mId( id )
	, mAt( at )
	, mStatus( status )
	, mLogPath( log_file )
	, mLog( true, log_file )
{
	mLogPtr = mLog.IsOk() ? &COsProcess::RealLog : &COsProcess::PseudoLog;
}


//virtual 
COsProcess::~COsProcess()
{
	if ( state() != NotRunning )
		Kill();
}


//virtual 
void COsProcess::Kill()
{ 
	kill();
}

//virtual 
void COsProcess::Execute( bool detached )	//detached = false 
{
    if ( detached )
        startDetached( mCmdLine.c_str() );
    else
        start( mCmdLine.c_str() );
}





///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_OsProcess.cpp"
