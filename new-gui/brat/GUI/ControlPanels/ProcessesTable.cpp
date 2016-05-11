#include "new-gui/brat/stdafx.h"


#include "new-gui/Common/QtUtils.h"
#include "ApplicationLogger.h"

#include "DataModels/Workspaces/Operation.h"

#include "ProcessesTable.h"


/////////////////////////////////////////////////////////////////////////////////
//							Brat Process Class
/////////////////////////////////////////////////////////////////////////////////



COsProcess::COsProcess( const COperation *operation, bool sync, const std::string& name, QWidget *parent, const std::string& cmd, const std::string *output, int type )		//output = nullptr, int type = -1 
	: QProcess( parent )
	, mSync( sync )
	, mName( name )
	, mCmdLine( cmd )
	, mOutput( output ? *output : "" )
	, mType( type )
	, mOperation( operation )
{}


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
void COsProcess::Execute()
{
	start( mCmdLine.c_str() );
}









/////////////////////////////////////////////////////////////////////////////////
//						Processes Table Widget
/////////////////////////////////////////////////////////////////////////////////


//static 
const QString CProcessesTable::smSeparator = " : \n";


////////////////////////////
// Construction/Destruction
////////////////////////////



void CProcessesTable::CreateWidgets()
{
	mProcessesList = new QTableWidget;
	mProcessesList->setColumnCount( EProcessColumn_size );

	mProcessesList->setSortingEnabled( false );
	mProcessesList->setSelectionBehavior( QAbstractItemView::SelectRows );
	mProcessesList->setSelectionMode( QAbstractItemView::SingleSelection );	
	mProcessesList->setAlternatingRowColors( true );
    mProcessesList->setEditTriggers(QAbstractItemView::NoEditTriggers);
	mProcessesList->setShowGrid( false );

	mProcessesList->setMinimumWidth( ComputeTableWidth( mProcessesList ) );


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
	QTableWidgetItem *widgetItem = mProcessesList->item( index, 0 ); 
	widgetItem->setData( Qt::UserRole, QVariant::fromValue( process ) );
}

COsProcess* CProcessesTable::GetItemProcessData( int index ) const
{
	QTableWidgetItem *widgetItem = mProcessesList->item( index, 0 );
	return widgetItem->data( Qt::UserRole ).value< COsProcess* >();
}

int CProcessesTable::FindProcess( const std::string &name ) const
{
    const int size = mProcessesList->rowCount();
	for ( int index = 0; index < size; ++index )
	{
		COsProcess *item_process = GetItemProcessData( index );		assert__( item_process );
		if ( item_process->GetName() == name )
			return index;
	}
	return -1;
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

	mProcessesList->clear();
	mProcessesList->setRowCount( (int)mProcesses.size() );
	int index = 0;
	for ( auto *process : mProcesses )
	{
		auto *cmd_item = new QTableWidgetItem( process->GetCmd().c_str() );
		cmd_item->setToolTip( cmd_item->text() );							//for long command lines
		mProcessesList->setItem( index, eProcessName, new QTableWidgetItem( process->GetName().c_str() ) );
		mProcessesList->setItem( index, eProcessCmdLine, cmd_item );

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
	return FormatOutputMessage( process->GetName().c_str(), process->readAllStandardError() );
}
QString CProcessesTable::ReadStdOut( COsProcess *process )
{ 
	return FormatOutputMessage( process->GetName().c_str(), process->readAllStandardOutput() );
}


					
////////////////////////////
// Kill
////////////////////////////


void CProcessesTable::HandleSelectedProcessChanged()
{
	mKillButton->setEnabled( mProcessesList->currentRow() >= 0 );
}

void CProcessesTable::HandleKill()
{
	int index = mProcessesList->currentRow();					assert__( index >= 0 );	
	COsProcess *item_process = GetItemProcessData( index );		assert__( item_process );
	item_process->Kill();	//triggers error && finished
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


bool CProcessesTable::Add( bool sync, bool allow_multiple, const COperation *operation )
{
	return Add( sync, allow_multiple, operation->GetTaskName(), operation->GetFullCmd(), operation );
}

bool CProcessesTable::Add4Statistics( bool allow_multiple, const COperation *operation )
{
	return Add( false, allow_multiple, operation->GetShowStatsTaskName(), operation->GetShowStatsFullCmd(), operation );
}

bool CProcessesTable::Add( bool sync, bool allow_multiple, const std::string &original_name, const std::string &cmd, const COperation *operation )		//operation==nullptr
{
	std::string name = original_name;

	const int index = FindProcess( original_name );
	if ( index >= 0 )
	{
		if ( allow_multiple )
		{
			name = MakeProcessNewName( original_name );
		}
		else
		{
			std::string msg =
				"Unable to process task '"
				+ original_name
				+ "'. A similar task is already running.";

			LOG_WARN( "\n==> " + msg + "<===\n" );		//v3 also logged besides displaying message box
			SimpleWarnBox( msg );
			return false;
		}
	}


	COsProcess *process = new COsProcess( operation, sync, name, this, cmd );
	mProcesses.push_back( process );
	FillList();

    connect( process, SIGNAL( readyReadStandardOutput() ),				this, SLOT( UpdateOutput() ) );
    connect( process, SIGNAL( readyReadStandardError() ),				this, SLOT( UpdateOutput() ) );
    connect( process, SIGNAL( finished( int, QProcess::ExitStatus ) ),	this, 
		sync ?
		SLOT( SyncProcessFinished( int, QProcess::ExitStatus ) ) 
		:
		SLOT( AsyncProcessFinished( int, QProcess::ExitStatus ) ) 
		);
    connect( process, SIGNAL( error( QProcess::ProcessError ) ),		this, SLOT( ProcessError( QProcess::ProcessError ) ) );

	if ( sync )
	{
		std::string msg = 
			"\n\n===> Synchronous Task '"
			+ process->GetName()
			+ "' started with command line below:<===\n'"
			+ process->GetCmd()
			+ "'\n\n\n ==========> Please wait.... A report will display at the end of the task <==========\n\n";

		LOG_INFO( msg );
	}

	///////////////////
	process->Execute();
	///////////////////

	if ( !sync )
	{
		std::string msg = 
			"\n\n===> Asynchronous Task '"
			+ process->GetName()
			+ "' (pid "
			+ process->PidString()
			+ ") started with command line below:<===\n'"
			+ process->GetCmd()
			+ "'\n\n";

		LOG_INFO( msg );
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


void CProcessesTable::UpdateOutput()
{
	COsProcess *process = qobject_cast<COsProcess*>( sender() );			assert__( process );

    QString text = ReadStdError( process );
	if ( !text.isEmpty() )
		LOG_WARN( text );

    text = ReadStdOut( process );
	if ( !text.isEmpty() )
		LOG_INFO( text );
}


COsProcess* CProcessesTable::ProcessFinished( int exit_code, QProcess::ExitStatus exitStatus )
{
	COsProcess *process = qobject_cast<COsProcess*>( sender() );			assert__( process );

    if (exitStatus == QProcess::CrashExit) 
	{
        LOG_WARN( FormatFinishedMessage( process, "program crash") );
    } 
	else if ( exit_code != 0 )	//this is ExitStatus::NormalExit, although error code != 0
	{
        LOG_WARN( FormatFinishedMessage( process, ( "exit code " + n2s<std::string>( exit_code ) ).c_str() ) );
    } 
	else 
	{
        LOG_INFO( FormatFinishedMessage( process, "success" ) );
    }

	return process;
}
void CProcessesTable::AsyncProcessFinished( int exit_code, QProcess::ExitStatus exitStatus )
{
	COsProcess *process = ProcessFinished( exit_code, exitStatus );

	emit ProcessFinished( exit_code, exitStatus, process->Operation(), false );

	Remove( process );
}
void CProcessesTable::SyncProcessFinished( int exit_code, QProcess::ExitStatus exitStatus )
{
	COsProcess *process = ProcessFinished( exit_code, exitStatus );

	emit ProcessFinished( exit_code, exitStatus, process->Operation(), true );

	Remove( process );
}


//static 
const QString& CProcessesTable::ProcessErrorMessage( QProcess::ProcessError error )
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

	assert__( error < msgs_size );

	return msgs[ error ];
}



void CProcessesTable::ProcessError( QProcess::ProcessError error )
{
	COsProcess *process = qobject_cast<COsProcess*>( sender() );			assert__( process );

	LOG_WARN( FormatErrorMessage( process, ProcessErrorMessage( error ) ) );
}





///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_ProcessesTable.cpp"
