#include "stdafx.h"

#include "new-gui/Common/ApplicationPaths.h"

#include "BratApplication.h"
#include "BratMainWindow.h"

#include "GUI/norwegianwoodstyle.h"




int main( int argc, char *argv[] )
{
    static CApplicationPaths brat_paths( argv[0] ); // (*)

    //TODO check brat_paths validity, issue informative error message
    //  if that is the case, and return value != 0. Or proceeed to application
    //  construction passing a valid paths set.

    CBratApplication a( brat_paths, argc, argv, true );
    //QApplication::setStyle(new NorwegianWoodStyle);		//joke

	CBratMainWindow w( a.Settings() );
	QObject::connect( &w, SIGNAL( SettingsUpdated() ), &a, SLOT( updateSettings() ) );		//UGGLY

	w.show();

    return a.exec();
}

//(*) As a last resort, this would solve the problem of double version loading:
//
//      setenv( "QT_PLUGIN_PATH", nullptr, 1 );
//
