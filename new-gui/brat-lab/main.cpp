#include "stdafx.h"

#include "brat-lab.h"
#include "QbrtMainWindow.h"
//#include "../GUI/QbrtMapEditor.h"


int main(int argc, char *argv[])
{
    QbrtApplication  a(argc, argv, true);

    //QbrtMapCanvas w;
    QbrtMainWindow w;
    w.show();

    return a.exec();
}
