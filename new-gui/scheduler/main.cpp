#include "schedulerdlg.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SchedulerDlg w;
    w.show();

    return a.exec();
}
