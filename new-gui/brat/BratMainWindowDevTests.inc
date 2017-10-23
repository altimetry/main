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


#if defined (_DEBUG) || defined (DEBUG)|| defined (CORRESYF_WORK)
#include "../support/code/future/NcDumpDialog.h"
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TEST CODE SECTION for a future python console ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined (_DEBUG) || defined (DEBUG)
void CBratMainWindow::PythonConsoleError( QProcess::ProcessError error )
{
    auto message = "An error occurred launching " + mModel.BratPaths().FullPythonExecutablePath() + "\n" + q2a( COsProcess::ProcessErrorMessage( error ) );
	SimpleErrorBox( message );
	LOG_WARN( message );
}
void CBratMainWindow::on_action_Python_Console_triggered()
{
    if (!IsFile( mModel.BratPaths().FullPythonExecutablePath() ) )    {
        SimpleErrorBox( "Could not find\n" + mModel.BratPaths().FullPythonExecutablePath() );
        return;
    }

    COsProcess *process = new COsProcess( false, "", this, "\"" + mModel.BratPaths().FullPythonExecutablePath() + "\"" );
    connect( process, SIGNAL( error( QProcess::ProcessError ) ), this, SLOT( PythonConsoleError( QProcess::ProcessError ) ) );

#if defined(Q_OS_LINUX)

    //process->Execute( true );
    //process->Execute();
    QStringList args;
    //args << "-hold" << "-C" << "-e" << mModel.BratPaths().FullPythonExecutablePath().c_str();
    args << "-hold" << "-e" << mModel.BratPaths().FullPythonExecutablePath().c_str();
    //args << "-e" << mModel.BratPaths().FullPythonExecutablePath().c_str();

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert( "PYTHONPATH", ( mModel.BratPaths().mPythonDir + "/lib/python3.2" ).c_str() );
    env.insert( "PYTHONHOME", ( mModel.BratPaths().mPythonDir + "/lib/python3.2" ).c_str() );
    //env.insert("TERM", "xterm");    
    process->setProcessEnvironment( env );

    //process->setWorkingDirectory( ( mModel.BratPaths().mPythonDir + "" ).c_str() );

    process->start( "xterm", args );    
    //process->startDetached( "xterm", args );  //does not use env(ironemnt)

#elif defined(Q_OS_MAC)

    QStringList args;
    //args << "-hold" << "-C" << "-e" << mModel.BratPaths().FullPythonExecutablePath().c_str();
    //args << "-c" << mModel.BratPaths().FullPythonExecutablePath().c_str();
    //args << "-e" << mModel.BratPaths().FullPythonExecutablePath().c_str();

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert( "DYLD_LIBRARY_PATH", ( mModel.BratPaths().mPythonDir.mPath + ":" + mModel.BratPaths().mPythonDir.mPath + "/lib"  + ":/Users/brat/dev/bin/Qt-4.8.6/brat/x86_64/Debug/brat.app/Contents/MacOS/Python/lib/python3.2/lib-dynload" ).c_str() );
    env.insert( "DYLD_FRAMEWORK_PATH", ( mModel.BratPaths().mPythonDir.mPath + ":/Users/brat/dev/bin/Qt-4.8.6/brat/x86_64/Debug/brat.app/Contents/MacOS/Python/lib/python3.2/lib-dynload" ).c_str() );
    env.insert( "PYTHONPATH", ( mModel.BratPaths().mPythonDir + "/lib/python3.2" ).c_str() );
    env.insert( "PYTHONHOME", ( mModel.BratPaths().mPythonDir.mPath ).c_str() );
    //env.insert("TERM", "xterm");
    process->setProcessEnvironment( env );

    //process->setWorkingDirectory( ( mModel.BratPaths().mPythonDir + "" ).c_str() );

    //args << mModel.BratPaths().FullPythonExecutablePath().c_str();
    args << "/Users/brat/dev/bin/Qt-4.8.6/brat/x86_64/Debug/brat.app/Contents/MacOS/Python/Resources/Python.app/Contents/MacOS/Python";

    process->start( "/Applications/Utilities/Terminal.app/Contents/MacOS/Terminal", args );
    //process->start( "/Users/brat/dev/bin/Qt-4.8.6/brat/x86_64/Debug/brat.app/Contents/MacOS/Python/Resources/Python.app/Contents/MacOS/Python" );

    //process->startDetached( "xterm", args );  //does not use env(ironemnt)
#else

    process->Execute( true );           //true -> detached

#endif
}
#endif
// TEST CODE SECTION for a python console //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TEST CODE SECTION for worker threads ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static bool thread_finished = true;

class DebugThread : public QThread
{
    //Q_OBJECT
private:
	void run()
	{
		for ( ;; )
		{
			LOG_TRACEstd( "DEBUG hello from worker thread " + n2s( thread()->currentThreadId() ) );
			msleep( 1000 );
			if ( thread_finished )
				return;
		}
	}
};
class LogThread : public QThread
{
    //Q_OBJECT
private:
	void run()
	{
		int i = 0;
		for ( ;; )
		{
			switch ( ++i % 3 )
			{
				case 0:
					LOG_INFO( "LOG hello from worker thread " + n2s( thread()->currentThreadId() ) );
					break;
				case 1:
					LOG_WARN( "LOG hello from worker thread " + n2s( thread()->currentThreadId() ) );
					break;
				case 2:
					LOG_FATAL( "LOG hello from worker thread " + n2s( thread()->currentThreadId() ) );
					break;
			}
			msleep( 10 );
			if ( thread_finished )
				return;
		}
	}
};


static DebugThread thread_d;
static LogThread thread_l1, thread_l2;

void TestWorkerThreads()
{
	if ( thread_finished )
	{
		thread_finished = false;
		thread_d.start();
		thread_l1.start();
		thread_l2.start();
	}
	else
	{
		thread_finished = true;
		thread_d.terminate();
		thread_l1.terminate();
		thread_l2.terminate();

		thread_d.wait();
		thread_l1.wait();
		thread_l2.wait();
	}
	//for ( ;; )
	//{
	//	qDebug() << "hello from GUI thread " << qApp->thread()->currentThreadId();
	//	qApp->processEvents();
	//	//QTest::qSleep( 1000 );
	//}
}

// TEST CODE SECTION for worker threads ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void TestPyAlgo()
{
	static const char *argstr = "L:\\project\\dev\\source\\process\\python\\BratAlgorithmExample1.py PyAlgoExample1 10 20 abc 30 40";   Q_UNUSED(argstr);
	static const char *argv1 = "L:\\project\\dev\\source\\process\\python\\BratAlgorithmExample1.py";
	static const char *argv2 = "PyAlgoExample1";
	static const char *argv[] =
	{
		"10", "20", "abc", "30", "40"
	};
	static const DEFINE_ARRAY_SIZE( argv );



	EMessageCode error_code = e_OK;
	try {

		// I. create algorithm object

		PyAlgo algo( argv1, argv2 );

		// II. output algorithm properties

		LOG_INFO( "------------------------------------------------" );
		LOG_INFO( "Algorithm Name:        " + algo.GetName()        );
		LOG_INFO( "------------------------------------------------" );

		LOG_INFO( "Algorithm Description: " + algo.GetDescription() );
		LOG_INFO( "------------------------------------------------" );

		int size = algo.GetNumInputParam();
		LOG_INFO( "Nb. Input Parameters:  " + n2s( size )                  );

		for ( int i = 0; i < size; ++i )
		{
			LOG_INFO( "  " +  n2s( i+1 ) + ":" );;
			LOG_INFO( "    Description: " +            algo.GetInputParamDesc(i)     );
			LOG_INFO( "    Format:      " + std::string( typeName( algo.GetInputParamFormat( i ) ) ) );
			LOG_INFO( "    Unit:        " +            algo.GetInputParamUnit(i)     );
		}
		LOG_INFO( "------------------------------------------------" );

		LOG_INFO( "Algorithm Output Unit: " + algo.GetOutputUnit()  );
		LOG_INFO( "------------------------------------------------" );

		// III. use algorithm to compute something

		CVectorBratAlgorithmParam args;
		algo.CreateAlgorithmParamVector( args, (char**)argv, argv_size );

		LOG_INFO( "Running Algorithm... "                        );
		double result = algo.Run( args );
		LOG_INFO( "The result is: " + n2s( result ) );
		LOG_INFO( "------------------------------------------------" );
	}
	catch( EMessageCode msg )
	{
		error_code = msg;
	}
	catch( ... )
	{
		error_code = e_unspecified_error;
	}

	if ( error_code != e_OK )
	{
		LOG_INFO( py_error_message( error_code ) );
		return;
	}
}



void TestSettingsPaths( const CBratSettings &settings )
{
	std::string
	msg = "Main application settings file: " + settings.FilePath();
	msg += "\nCurrent Working Directory: " + q2a( QDir::currentPath() );
	msg += "\n\n";
	msg += settings.BratPaths().ToString();
	SimpleMsgBox( msg );
}


void TestNcDump( CBratMainWindow *w, const CBratSettings &settings )
{
	static const std::string ncdump = settings.BratPaths().mExecutableDir + "/ncdump";
	static QString netcdf_path = settings.BratPaths().UserDataDirectory().c_str();

	auto path = BrowseFile( w, "Open netcdf file", netcdf_path );
	if ( !path.isEmpty() )
	{
		netcdf_path = path;

#if defined (_DEBUG) || defined (DEBUG)|| defined (CORRESYF_WORK)
		CNcDumpDialog *pdlg = new CNcDumpDialog( false, ncdump, netcdf_path, w );
		pdlg->show();
#endif

	}
}
