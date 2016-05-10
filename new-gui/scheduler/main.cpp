#include "stdafx.h"

#include "SchedulerApplication.h"
#include "SchedulerDlg.h"


class CConsoleApplicationPaths;


int main( int argc, char *argv[] )
try {
	int result = -1;
	{
		QSchedulerApplication a( argc, argv );

        CSchedulerDlg w( a.ApplicationPaths(), nullptr );

		w.show();

		result = a.exec();
	}

	return result;
}
catch ( const CException &e )
{
    //LOG_FATAL( e.Message() );
    return QSchedulerApplication::OffGuiErrorDialog( -1, e.Message().c_str() );
}
catch ( const std::exception &e )
{
    //LOG_FATAL( e.what() );
    return QSchedulerApplication::OffGuiErrorDialog( -2, e.what() );
}
catch ( ... )
{
    static const std::string msg( "Unknown exception caught in main. Not possible to retrieve error information." );
    //LOG_FATAL( msg );
    return QSchedulerApplication::OffGuiErrorDialog( -3, msg.c_str() );
}
