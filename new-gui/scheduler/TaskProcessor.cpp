#include "stdafx.h"

#include "new-gui/Common/+Utils.h"
#include "new-gui/Common/ScheduledTasksList.hxx"

#include "TaskProcessor.h"
#include "BratTask.h"

/*
class TBratTask
{
public:

	enum Status
	{
		e_BRAT_STATUS_PENDING,
		e_BRAT_STATUS_PROCESSING,
		e_BRAT_STATUS_ENDED,
		e_BRAT_STATUS_ERROR,
		e_BRAT_STATUS_WARNING,

		e_Status_size
	};

protected:
	static const std::string* statusNames()
	{
		static const std::string labels[ e_Status_size ] =
		{
			"pending",
			"in progress",
			"ended",
			"error",
			"warning"
		};

		static_assert( ARRAY_SIZE( labels ) == e_Status_size, "CBratTask: Number of status labels and values does not match." );

		return labels;
	}


public:
	static const std::string& TaskStatusToString( Status s )
	{
		assert__( s < e_Status_size );
		return statusNames()[ s ];
	}
	static Status StringToTaskStatus( const std::string& status )
	{
		static const std::string *labels = statusNames();

		auto it = std::find( labels, &labels[ e_Status_size ], status );
		if ( it != &labels[ e_Status_size ] )
			return static_cast<Status>( it - labels );

		//throw CException( "CBratTask::StringToTaskStatus: unknown status label " + status, BRATHL_INCONSISTENCY_ERROR );
		return Status::e_Status_size;
	}

	// datetime in ISO 8601 format
	static const std::string FormatISODateTime;

protected:
	long long m_uid;
	std::string m_name;
	std::string m_cmd;
	CBratTaskFunction m_function;	//??? also in xml ???
//	wxDateTime m_at;
	std::string m_at;
	Status m_status;

	std::string m_logFile;

//	CVectorBratTask m_subordinateTasks;

public:
	TBratTask( const task &oxml )				//USED ON LOAD
	{
		m_uid = oxml.uid();
		m_name = oxml.name();
		m_cmd = fromOpt( oxml.cmd(), std::string() );
		m_at = oxml.at();
		m_status = enum_cast< Status >( oxml.status() );
		m_logFile = oxml.logFile();
	}

	task& IOcopy( task &oxml ) const			//USED ON STORE
	{
		oxml.uid( m_uid );
		oxml.name( m_name );
		oxml.cmd( m_cmd );
		oxml.at( m_at );
		oxml.status( enum_reverse_cast< task::status_type >( m_status ) );
		oxml.logFile( m_logFile );

		return oxml;
	}

	bool operator == ( const TBratTask &o ) const 
	{
		return 
			m_uid == o.m_uid &&
			m_name == o.m_name &&
			m_cmd == o.m_cmd &&
			//m_function;
			m_at == o.m_at &&
			m_status == o.m_status &&
			m_logFile == o.m_logFile
			;
	}

	bool operator != ( const TBratTask &o ) const
	{
		return !( *this == o );
	}
};
*/

class SchedulerTasks
{
	typedef std::vector<CBratTask*> tasks_set_t;

	tasks_set_t mPendingTasks;
	tasks_set_t mProcessingTasks;
	tasks_set_t mEndedTasks;

public:

	virtual ~SchedulerTasks()
	{
		destroyPointersAndContainer( mPendingTasks );
		destroyPointersAndContainer( mProcessingTasks );
		destroyPointersAndContainer( mEndedTasks );
	}

	bool operator == ( const SchedulerTasks &o ) const 
	{
		auto equal_vetors = []( const tasks_set_t &v1, const tasks_set_t &v2 )
		{
			const size_t size = v1.size();
			if ( size != v2.size() )
				return false;

			for ( size_t i = 0; i < size; ++i )
				if ( *v1[ i ] != *v2[ i ] )
					return false;

			return true;
		};

		return 
			equal_vetors( mPendingTasks, o.mPendingTasks ) &&
			equal_vetors( mProcessingTasks, o.mProcessingTasks ) &&
			equal_vetors( mEndedTasks, o.mEndedTasks );
	}


	///////////////
	//serialization

    SchedulerTasks& operator = ( const bratSchedulerConfig &o );			//USED ON LOAD

    bratSchedulerConfig& IOcopy( bratSchedulerConfig &oxml ) const;			//USED ON STORE


	bool store( const std::string &path )
	{
		return ::store( *this, path );
	}

	bool load( const std::string &path )
	{
		return ::load( *this, path );
	}
};

SchedulerTasks& SchedulerTasks::operator = ( const bratSchedulerConfig &oxml )			//USED ON LOAD
{
	mPendingTasks.clear();
	mProcessingTasks.clear();
	mEndedTasks.clear();

	auto const &pending = oxml.pendingTasks().task();
	auto const &processing = oxml.processingTasks().task();
	auto const &ended = oxml.endedTasks().task();

	for ( auto ii = pending.begin(); ii != pending.end(); ++ii )
	{
		mPendingTasks.push_back( new CBratTask(*ii) );
	}
	for ( auto ii = processing.begin(); ii != processing.end(); ++ii )
	{
		mProcessingTasks.push_back( new CBratTask(*ii) );
	}
	for ( auto ii = ended.begin(); ii != ended.end(); ++ii )
	{
		mEndedTasks.push_back( new CBratTask(*ii) );
	}
	return *this;
}

bratSchedulerConfig& SchedulerTasks::IOcopy( bratSchedulerConfig &oxml ) const			//USED ON STORE
{
	oxml.pendingTasks( bratSchedulerConfig::pendingTasks_type() );
	oxml.processingTasks( bratSchedulerConfig::processingTasks_type() );
	oxml.endedTasks( bratSchedulerConfig::endedTasks_type() );

	auto &pending = oxml.pendingTasks().task();
	auto &processing = oxml.processingTasks().task();
	auto &ended = oxml.endedTasks().task();

	for ( auto ii = mPendingTasks.begin(); ii != mPendingTasks.end(); ++ii )
	{
		task t;
		pending.push_back( (*ii)->IOcopy( t ) );
	}

	for ( auto ii = mProcessingTasks.begin(); ii != mProcessingTasks.end(); ++ii )
	{
		task t;
		processing.push_back( (*ii)->IOcopy( t ) );
	}

	for ( auto ii = mEndedTasks.begin(); ii != mEndedTasks.end(); ++ii )
	{
		task t;
		ended.push_back( (*ii)->IOcopy( t ) );
	}

	return oxml;
}



template<>
struct serializer_traits< SchedulerTasks >
{
	typedef bratSchedulerConfig converter_t;

	static void
	serialize(::xercesc::XMLFormatTarget& ft,
						 const converter_t& x, 
						 const ::xml_schema::namespace_infomap& m = ::xml_schema::namespace_infomap (),
						 const ::std::string& e = "UTF-8",
						 ::xml_schema::flags f = 0)
	{
		::bratSchedulerConfig_( ft, x, m, e, f );
	}

	static ::std::auto_ptr< converter_t >
	deserialize( const ::std::string& u, ::xml_schema::flags f, const ::xml_schema::properties& p = ::xml_schema::properties() )
	{
		return ::bratSchedulerConfig_( u, f, p );
	}
};

bool readTasks( const std::string &path )
{
	SchedulerTasks tasks;

	bool result = tasks.load( path );

#if defined(DEBUG)

	SchedulerTasks debug_tasks;
	if ( result )
	{
		std::string debug_path = std::string( getenv( "S3ALTB_ROOT") ) + "/project/dev/source/new-gui/scheduler/BratSchedulerTasksConfig_backup.xml";
		result = tasks.store( debug_path );
		if ( result )
			result = debug_tasks.load( debug_path );		
	}
	assert__( result && debug_tasks == tasks );

#endif

	return result;
}

