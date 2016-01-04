#include "stdafx.h"


#include "new-gui/QtInterface.h"
#include "new-gui/Common/+UtilsIO.h"
#include "new-gui/Common/QtUtilsIO.h"
#include "new-gui/brat/Workspaces/Operation.h"

#include "BratApplication.h"





CBratApplication::CBratApplication(int &argc, char **argv, bool GUIenabled, QString customConfigPath ) :	//customConfigPath = QString()
	base_t( argc, argv, GUIenabled, customConfigPath )
{
	qDebug() << showSettings();
	qDebug() << qgisSettingsDirPath();


	//femm
//#ifdef WIN32
//#ifdef _DEBUG
//	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
//#endif
//#endif

	// Register Brat algorithms
	CBratAlgorithmBase::RegisterAlgorithms();

	// To be sure that number have always a decimal point (and not a comma
	// or something else)
	setlocale( LC_NUMERIC, "C" );

	//// m_execName.Assign( wxGetApp().argv[ 0 ] );
	std::string m_execName = ApplicationDirectories::instance().mExecutableFileName.c_str();

	std::string appPath = ApplicationDirectories::instance().mExecutableDir;

	qDebug() << ApplicationDirectories::instance().mExecutableFileName.c_str(); //// 

	if ( getenv( BRATHL_ENVVAR ) == nullptr )
	{
		// Note that this won't work on Mac OS X when you use './BratGui' from within the Contents/MacOS directory of
		// you .app bundle. The problem is that in that case Mac OS X will change the current working directory to the
		// location of the .app bundle and thus the calculation of absolute paths will break
        CTools::SetDataDirForExecutable( ApplicationDirectories::instance().mExecutableFileName.c_str() /*this->argv()[ 0 ]*/ );
	}

	assert__( !appPath.empty() );

	COperation::SetExecNames( appPath );
	if ( getenv( BRATHL_ENVVAR ) == nullptr )
	{
        CTools::SetDataDirForExecutable( ApplicationDirectories::instance().mExecutableFileName.c_str() /*this->argv()[ 0 ]*/ );
	}

	if ( !CTools::DirectoryExists( CTools::GetDataDir() ) )
	{
		std::cerr << "WARNING: " << CTools::GetDataDir() << " is not a valid directory" << std::endl;
		////Warning??? or error?
		SimpleWarnBox( QString( "WARNING: " ) + CTools::GetDataDir().c_str() + " is not a valid directory -- BRAT cannot continue. \n\nAre you sure your " + BRATHL_ENVVAR + " environment variable is set correctly?" );
		throw false;  //TODO
	}

	std::string errorMsg;
	if ( !CTools::LoadAndCheckUdUnitsSystem( errorMsg ) )
	{
		std::cerr << errorMsg << std::endl;
		SimpleErrorBox( errorMsg.c_str() );
		throw false;
	}

	// Load aliases dictionary
	//
	CAliasesDictionary::LoadAliasesDictionary( &errorMsg, false );
	if ( !errorMsg.empty() )
	{
		std::cerr << "WARNING: " << errorMsg << std::endl;
		SimpleWarnBox( errorMsg );
	}

	CProduct::CodaInit();
	//CProduct::SetCodaReleaseWhenDestroy(false);

	//femm: remaining initialization in charge of the main window
}


//virtual 
CBratApplication::~CBratApplication()
{}




////////////////////////////////////////////////
//
////////////////////////////////////////////////

#include "moc_BratApplication.cpp"
