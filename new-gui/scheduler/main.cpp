#include "stdafx.h"	//RCCC: delete this comment, because it is only to point to you that this header is a special one, like mourinho, and always comes first in *.cpp files, with some rare exceptions (like the moc_*.cpp)


#include "SchedulerApplication.h"
#include "SchedulerDlg.h"


int main(int argc, char *argv[])
{
	int result = -1;

	{
		QSchedulerApplication a( argc, argv );
		
		SchedulerDlg w;

		w.show();

		result = a.exec();
	}

	QSchedulerApplication::dumpMemoryStatistics();

	return result;
}
