#ifndef SCHEDULERDLG_H
#define SCHEDULERDLG_H

#include "ui_schedulerdlg.h"

class SchedulerDlg : public QDialog, private Ui::SchedulerDlg
{
    Q_OBJECT

public:
    explicit SchedulerDlg(QWidget *parent = 0);
};

#endif // SCHEDULERDLG_H
