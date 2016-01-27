#include "stdafx.h"

#include "new-gui/Common/ApplicationPaths.h"

#include "brat-lab.h"
#include "QbrtMainWindow.h"
//#include "../GUI/QbrtMapEditor.h"


int main(int argc, char *argv[])
{
    static CApplicationPaths brat_paths( argv[0] ); // (*)

    QbrtApplication  a( brat_paths, argc, argv, true );

    //QbrtMapCanvas w;
    QbrtMainWindow w( a.Settings() );
    w.show();

    return a.exec();
}
