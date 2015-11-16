#include <QApplication>

#include "qschedulerapplication.h"
#include "schedulerdlg.h"


int main(int argc, char *argv[])
{
    QSchedulerApplication a(argc, argv);
    SchedulerDlg w;
    w.show();

    return a.exec();
}
