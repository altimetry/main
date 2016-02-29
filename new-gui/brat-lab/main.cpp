#include "stdafx.h"

#include "new-gui/Common/ApplicationPaths.h"

#include "brat-lab.h"
#include "QbrtMainWindow.h"


#include <qthread.h>

class I : public QThread
{
public:
	static void sleep( unsigned long secs ) 
	{
		QThread::sleep( secs );
	}

	static void msleep( unsigned long msecs ) 
	{
		QThread::msleep( msecs );
	}

	static void usleep( unsigned long usecs ) 
	{
		QThread::usleep( usecs );
	}
};



int main(int argc, char *argv[])
{
	static const int messages_start_index = 2;

	if ( argc < 3 )
	{
		std::string msg = "Wrong number of arguments";
		std::cout << msg << std::endl;
		throw CException( msg );
	}

	const int times = s2n< int >( argv[1] );
	for ( int i = 0; i < times; ++i )
	{

		I::sleep( 2 );
		for ( int j = messages_start_index; j < argc; ++j )
			std::cout << argv[ j ] << std::endl;
	}

	return 0;

	/*
    static CApplicationPaths brat_paths( argv[0] ); // (*)

    QbrtApplication  a( brat_paths, argc, argv, true );

    //QbrtMapCanvas w;
    QbrtMainWindow w( a.Settings() );
    w.show();

    return a.exec();
	*/
}
