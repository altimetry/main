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

#include "ProcessesTable.h"


/////////////////////////////////////////////////////////////////////////////////
//						Processes Table Widget
/////////////////////////////////////////////////////////////////////////////////


//static 
const QString CProcessesTable::smSeparator = " : \n";
//static 
const QString CProcessesTable::smTitleBegin = "===> ";
//static 
const QString CProcessesTable::smTitleEnd = " <===";


////////////////////////////
// Construction/Destruction
////////////////////////////



void CProcessesTable::CreateWidgets()
{
	mProcessesList = new QTableWidget;
	mProcessesList->setColumnCount( EProcessColumn_size );

	for ( int i = 0; i < EProcessColumn_size; ++i )
        mProcessesList->setHorizontalHeaderItem( i, new QTableWidgetItem() );

    mProcessesList->horizontalHeaderItem( eTaskUid )->setText( "Uid" );
    mProcessesList->horizontalHeaderItem( eProcessName )->setText( "Name" );
    mProcessesList->horizontalHeaderItem( eTaskStart )->setText( "Start" );
    mProcessesList->horizontalHeaderItem( eTaskStatus )->setText( "Status" );
    mProcessesList->horizontalHeaderItem( eProcessCmdLine )->setText( "Command Line" );
    mProcessesList->horizontalHeaderItem( eTaskLogFile )->setText( "Log file" );

	mProcessesList->setSortingEnabled( false );
	mProcessesList->setSelectionBehavior( QAbstractItemView::SelectRows );
	mProcessesList->setSelectionMode( QAbstractItemView::SingleSelection );	
	mProcessesList->setAlternatingRowColors( true );
    mProcessesList->setEditTriggers(QAbstractItemView::NoEditTriggers);
	mProcessesList->setShowGrid( false );

	mProcessesList->horizontalHeader()->setStretchLastSection( true );

	//mProcessesList->setMinimumWidth( ComputeTableWidth( mProcessesList ) );   TODO: too wide for small screens: 1280 x 800


	mKillButton = new QPushButton( "Kill" );
	mKillButton->setEnabled( false );

	AddTopLayout( ELayoutType::Horizontal, { mProcessesList, mKillButton } );

	Wire();
}
void CProcessesTable::Wire()
{
	connect( mProcessesList, SIGNAL( itemSelectionChanged() ), this, SLOT( HandleSelectedProcessChanged() ) );
	connect( mKillButton, SIGNAL( clicked() ), this, SLOT( HandleKill() ) );
}


CProcessesTable::CProcessesTable( QWidget *parent )		//parent = nullptr 
	: base_t( parent )
{
	CreateWidgets();
}


void CProcessesTable::KillAll()
{
	while ( mProcessesList->rowCount() > 0 )
	{
		COsProcess *item_process = GetItemProcessData( 0 );		assert__( item_process );
		item_process->Kill();					//triggers error && finished
		item_process->waitForFinished();		//qApp->processEvents();
	}
}


//virtual 
CProcessesTable::~CProcessesTable()
{
	KillAll();
	DestroyPointersAndContainer( mProcesses );
}


////////////////////////////
// List manipulation helpers
////////////////////////////



Q_DECLARE_METATYPE( COsProcess* )

void CProcessesTable::SetItemProcessData( int index, COsProcess *process )
{
	QTableWidgetItem *widgetItem = mProcessesList->item( index, eProcessCmdLine ); 
	widgetItem->setData( Qt::UserRole, QVariant::fromValue( process ) );
}

COsProcess* CProcessesTable::GetItemProcessData( int index ) const
{
	QTableWidgetItem *widgetItem = mProcessesList->item( index, eProcessCmdLine );
	return widgetItem->data( Qt::UserRole ).value< COsProcess* >();
}

int CProcessesTable::FindProcess( const std::string &name ) const
{
    const int size = mProcessesList->rowCount();
	for ( int index = 0; index < size; ++index )
	{
		COsProcess *item_process = GetItemProcessData( index );		assert__( item_process );
		if ( item_process->Name() == name )
			return index;
	}
	return -1;
}



void CProcessesTable::HideColumns( std::initializer_list< EProcessColumn > columns, bool hide )		//bool hide = true 
{
	for ( auto column : columns )
	{
		mProcessesList->setColumnHidden( column, hide );
	}
}


void CProcessesTable::FillList()
{
	// lambdas

    auto resize_columns = []( std::initializer_list< QTableWidget* > tables )
	{
        static const EProcessColumn resizable_columns[] = { eProcessName, eProcessCmdLine };

        for ( auto *t : tables )
		{
			for ( auto i : resizable_columns )
				t->resizeColumnToContents( i );
		}
	};


	// function body

	mProcessesList->setRowCount( 0 );
	mProcessesList->setRowCount( (int)mProcesses.size() );
	int index = 0;
	for ( auto *process : mProcesses )
	{
		auto *cmd_item = new QTableWidgetItem( process->CmdLine().c_str() );
		cmd_item->setToolTip( cmd_item->text() );							//for long command lines

		mProcessesList->setItem( index, eProcessName, new QTableWidgetItem( process->Name().c_str() ) );
		mProcessesList->setItem( index, eProcessCmdLine, cmd_item );

		mProcessesList->setItem( index, eTaskUid, new QTableWidgetItem( process->Id().c_str() ) );
		mProcessesList->setItem( index, eTaskStart, new QTableWidgetItem( process->At().c_str() ) );
		mProcessesList->setItem( index, eTaskStatus, new QTableWidgetItem( process->Status().c_str() ) );
		mProcessesList->setItem( index, eTaskLogFile, new QTableWidgetItem( process->LogPath().c_str() ) );

		SetItemProcessData( index, process );
		index++;
	}

	resize_columns( { mProcessesList } );
}


////////////////////////////
// Process I/O Helpers
////////////////////////////


QString CProcessesTable::ReadStdError( COsProcess *process )
{ 
	return FormatOutputMessage( QString(), process->readAllStandardError() );
}
QString CProcessesTable::ReadStdOut( COsProcess *process )
{ 
	return FormatOutputMessage( QString(), process->readAllStandardOutput() );
}


void CProcessesTable::LogInfo( COsProcess *process, const std::string &text )
{
	LOG_INFO( FormatOutputMessage( process->Name(), text ) );
	process->Log( text );
}
void CProcessesTable::LogWarn( COsProcess *process, const std::string &text )
{
	LOG_WARN( FormatOutputMessage( process->Name(), text ) );
	process->Log( text );
}



					
////////////////////////////
// Kill
////////////////////////////

//slot
void CProcessesTable::HandleSelectedProcessChanged()
{
	mKillButton->setEnabled( mProcessesList->currentRow() >= 0 );
}

//slot
void CProcessesTable::HandleKill()
{
	int index = mProcessesList->currentRow();					assert__( index >= 0 );	
	COsProcess *item_process = GetItemProcessData( index );		assert__( item_process );
	item_process->Kill();	//triggers error && finished
	mUserKillRequestPending = true;
}




///////////////////////////////////////
//
//			Add / Remove
//
//	Add is called by the client
//	Remove is called by signal handling
//
///////////////////////////////////////


std::string CProcessesTable::MakeProcessNewName( const std::string &original_name ) const
{
	size_t cardinal = 0;
	std::string name = original_name;
	
	while ( FindProcess( name ) >= 0 )
	{
		name = original_name + "_#" + n2s<std::string>( ++cardinal );
	}
	return name;
}


bool CProcessesTable::Add( COsProcess *process, bool with_gui, bool sync, bool allow_multiple )
{
	const std::string &original_name = process->Name();

	const int index = FindProcess( original_name );
	if ( index >= 0 )
	{
		if ( allow_multiple )
		{
			process->SetName( MakeProcessNewName( original_name ) );
		}
		else
		{
			std::string msg = "Unable to process task '" + original_name + "'. A similar task is already running.";
			LOG_WARN( "\n==> " + msg + "<===\n" );		//v3 also logged besides displaying message box
			if ( with_gui )
				SimpleWarnBox( msg );

			delete process;
			return false;
		}
	}

	//COsProcess *process = new COsProcess( sync, name, this, cmd, user_data, id, at, status, log_file );
	if ( !process->parent() )
		process->setParent( this );
	mProcesses.push_back( process );
	FillList();

    connect( process, SIGNAL( readyReadStandardOutput() ),				this, SLOT( HandleUpdateOutput() ) );
    connect( process, SIGNAL( readyReadStandardError() ),				this, SLOT( HandleUpdateOutput() ) );
    connect( process, SIGNAL( finished( int, QProcess::ExitStatus ) ),	this, 
		sync ?
		SLOT( HandleSyncProcessFinished( int, QProcess::ExitStatus ) ) 
		:
		SLOT( HandleAsyncProcessFinished( int, QProcess::ExitStatus ) ) 
		);
    connect( process, SIGNAL( error( QProcess::ProcessError ) ),		this, SLOT( HandleProcessError( QProcess::ProcessError ) ) );

	if ( sync )
	{
		std::string msg = 
			"\n" + q2a( smTitleBegin ) + "Synchronous Task '"
			+ process->Name()
			+ "' started with command line below:\n\n'"
			+ process->CmdLine()
			+ "'\n\nPlease wait.... A report will display at the end of the task" + q2a( smTitleEnd ) + "\n";

		LogInfo( process, msg );
	}

	//In case any abnormal termination is not captured by the finish
	// slots, which are also responsible for clearing the flag
	// 
	mUserKillRequestPending = false;

	///////////////////
	process->Execute();
	///////////////////

	if ( !sync )
	{
		std::string msg = 
			"\n" + q2a( smTitleBegin ) + "Asynchronous Task '"
			+ process->Name()
			+ "' (pid "
			+ process->PidString()
			+ ") started with command line below:\n'"
			+ process->CmdLine()
			+ "'\n" + q2a( smTitleEnd ) + "\n";

		LogInfo( process, msg );
	}

	//if ( sync )
	//{
	//  while ( !process->waitForFinished( 250 ) )	//calling this from thread or GUI risks to block if called process never returns
	//  {
	//		qApp->processEvents();
	//	}
	//}

	return process->waitForStarted( 5000 ) ;	//calling this from thread or GUI risks to block if called process never returns
}


bool CProcessesTable::Add( bool with_gui, void *user_data, bool sync, bool allow_multiple, const std::string &original_name, const std::string &cmd,
		const std::string &id,
		const std::string &at,
		const std::string &status,
		const std::string &log_file	)	//id = "", etc.
{
	return Add( new COsProcess( sync, original_name, this, cmd, user_data, id, at, status, log_file ), with_gui, sync, allow_multiple );
}


void CProcessesTable::Remove( COsProcess *process )
{
	auto it = std::find( mProcesses.begin(), mProcesses.end(), process );			assert__( it != mProcesses.end() );
	mProcesses.erase( it );

	const int size = mProcessesList->rowCount();
	for ( int index = 0; index < size; ++index )
	{
		COsProcess *item_process = GetItemProcessData( index );		assert__( item_process );
		if ( item_process == process )
		{
			mProcessesList->removeRow( index );
			HandleSelectedProcessChanged();			//enable/disable kill button
			process->deleteLater();
			return;
		}
	}

	assert__( false );
}




//////////////////////////////
// COsProcess Signals Handling
//////////////////////////////


//slot
void CProcessesTable::HandleUpdateOutput()
{
	COsProcess *process = qobject_cast<COsProcess*>( sender() );			assert__( process );

    QString text = ReadStdError( process );
	if ( !text.isEmpty() )
		LogWarn( process, q2a( text ) );

    text = ReadStdOut( process );
	if ( !text.isEmpty() )
		LogInfo( process, q2a( text ) );
}


COsProcess* CProcessesTable::ProcessFinished( int exit_code, QProcess::ExitStatus exitStatus )
{
	COsProcess *process = qobject_cast<COsProcess*>( sender() );			assert__( process );

    if (exitStatus == QProcess::CrashExit) 
	{
		if ( mUserKillRequestPending )
			LogInfo( process, q2a( FormatFinishedMessage( "termination requested by the user" ) ) );
		else
			LogWarn( process, q2a( FormatFinishedMessage( "program crash") ) );
	} 
	else if ( exit_code != 0 )	//this is ExitStatus::NormalExit, although error code != 0
	{
        LogWarn( process, q2a( FormatFinishedMessage( ( "exit code " + n2s<std::string>( exit_code ) ).c_str() ) ) );
    } 
	else 
	{
        LogInfo( process, q2a( FormatFinishedMessage( "success" ) ) );
    }

	mUserKillRequestPending = false;

	return process;
}
//slot
void CProcessesTable::HandleAsyncProcessFinished( int exit_code, QProcess::ExitStatus exitStatus )
{
	COsProcess *process = ProcessFinished( exit_code, exitStatus );

	auto *data = process->UserData();

	Remove( process );

	emit ProcessFinished( exit_code, exitStatus, false, data );
}
//slot
void CProcessesTable::HandleSyncProcessFinished( int exit_code, QProcess::ExitStatus exitStatus )
{
	COsProcess *process = ProcessFinished( exit_code, exitStatus );

	auto *data = process->UserData();

	Remove( process );

	emit ProcessFinished( exit_code, exitStatus, true, data );
}


//slot
void CProcessesTable::HandleProcessError( QProcess::ProcessError error )
{
	// Not an error if requested by the user
	// 
	if ( mUserKillRequestPending && error == QProcess::Crashed )
		return;

	COsProcess *process = qobject_cast<COsProcess*>( sender() );			assert__( process );

	LogWarn( process, q2a( FormatErrorMessage( COsProcess::ProcessErrorMessage( error ) ) ) );
}





///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_ProcessesTable.cpp"
