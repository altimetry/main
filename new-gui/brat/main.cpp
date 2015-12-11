#include "stdafx.h"

#include <QMainWindow>

#include <qgsapplication.h>


int main(int argc, char *argv[])
{

    QgsApplication a(argc, argv, true);

    QMainWindow w;
    w.show();

    return a.exec();

}



