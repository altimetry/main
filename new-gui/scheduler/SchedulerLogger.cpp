#include "stdafx.h"

#include "new-gui/Common/+Utils.h"
#include "SchedulerLogger.h"


void TraceWrite( const QString& msg, int debuglevel, const char* file, const char* function, int line )	//debuglevel = 1, const char* file = nullptr, const char* function = nullptr, int line = -1 
{
	CApplicationLoggerBase::TraceWrite( msg, debuglevel, file, function, line );
}



//static 
CSchedulerLogger& CSchedulerLogger::Instance()
{
	if ( !smInstance )
		smInstance = new CSchedulerLogger();

	return *dynamic_cast< CSchedulerLogger* >( smInstance );
}


//	Dynamic instance of the class is passed to osg::setNotifyHandler and automatically deleted by OSG
//
CSchedulerLogger::CSchedulerLogger() 
	: base_t()
{
	assert__( smInstance && smInstance == this );		//smInstance not necessarily == this
}

//virtual 
CSchedulerLogger::~CSchedulerLogger()
{}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_SchedulerLogger.cpp"
