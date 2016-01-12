#include "stdafx.h"

#include "BratApplication.h"
#include "BratMainWindow.h"

#include "GUI/norwegianwoodstyle.h"


int main( int argc, char *argv[] )
{
    CBratApplication a( argc, argv, true );
    //QApplication::setStyle(new NorwegianWoodStyle);		//joke

	CBratMainWindow w( a.Settings() );
	QObject::connect( &w, SIGNAL( SettingsUpdated() ), &a, SLOT( updateSettings() ) );		//UGGLY

	w.show();

    return a.exec();
}
