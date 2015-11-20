#include "stdafx.h"	//RCCC: delete this comment, because it is only to point to you that these header is a special one, like mourinho, and always comes first in *.cpp files, with some rare exceptions (like the moc_*.cpp)

#include "qschedulerapplication.h"
#include "SchedulerDlg.h"

#if _XERCES_VERSION < 30000
#  error Xerces-C++ 2-series is not supported
#endif

struct xerces
{
	xerces()
	{
		::xercesc::XMLPlatformUtils::Initialize();
	}

	~xerces()
	{
		::xercesc::XMLPlatformUtils::Terminate();
	}
};


int main(int argc, char *argv[])
{
	// RCCC: This is critical: eliminate the struct xerces and pass 
	//	the lines in its constructor and destructor, respectively, to the 
	//	constructor and destructor of the application class. Delete also,
	//	of course, this variable x. This struct was created only in 
	//	absence of an application class where to execute critical 
	//	initialization and termination code.
	//
	xerces x;

    QSchedulerApplication a(argc, argv);
	SchedulerDlg::functionWithCodeToMoveToApplicationClassConstructor();

    SchedulerDlg w;
    w.show();

    return a.exec();
}
