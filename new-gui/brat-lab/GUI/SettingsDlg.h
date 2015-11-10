#ifndef SETTINGSDLG_H
#define SETTINGSDLG_H

#include "../tmp/ui_SettingsDlg.h"
#include "../System/BackServices.h"

class SettingsDlg : public QDialog, private Ui::SettingsDlg
{
    Q_OBJECT

    TBackServices &m_bs;

public:
    explicit SettingsDlg( QWidget *parent );

protected:
    bool ValidateAndAssign();

private slots:
    virtual void accept();
};

#endif // SETTINGSDLG_H
