#include "stdafx.h"

#include "brat-lab.h"
#include "QbrtMainWindow.h"


/*
 * Windows environment, 32 bit. With the appropriate changes (Win32->x64 and 32->64) the 64 bit environment is obtained
 * PATH=$(PATH);L:\lib\Graphics\QGIS\2.8.3\bin\Win32\Debug\bin;L:\lib\GUI\Qt\4.8.6\build\Win32\bin;;L:\project\dev\bin\Win32\Debug\bin;L:\lib\Graphics\QGIS\OSGeo4W\32\bin;L:\lib\GUI\Qt\qwt\5.2.3\bin\Win32\lib
 */
int main(int argc, char *argv[])
{
    QbrtApplication  a(argc, argv, true);

    QbrtMainWindow w;
    w.show();

    return a.exec();
}
