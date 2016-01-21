#include "stdafx.h"


#include "SchedulerApplication.h"
#include "SchedulerDlg.h"



int main( int argc, char *argv[] )
{
	int result = -1;
	{
		QSchedulerApplication a( argc, argv );

		CSchedulerDlg w;

		w.show();

		result = a.exec();
	}

	return result;
}
