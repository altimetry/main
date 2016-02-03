#ifndef APPLICATION_SETTINGS_DLG_H
#define APPLICATION_SETTINGS_DLG_H

#include "ui_ApplicationSettingsDlg.h"




class CApplicationSettingsDlg : public QDialog, private Ui::CApplicationSettingsDlg
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

	Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	QCheckBox *mDesktopManagerSdiCheckbox = nullptr;

	CBratSettings &mSettings;

	//ctor helpers

	void createIcons();
public:
    explicit CApplicationSettingsDlg( CBratSettings &options, QWidget *parent );

protected:
    bool ValidateAndAssign();

private slots:
    virtual void accept();

	void changePage( QListWidgetItem *current, QListWidgetItem *previous );

	void UpdateDirectoriesActions( const QString & text );

    void on_BrowseParseDataDirectory_pushButton_clicked();
    void on_Browse_ProjectsPath_pushButton_clicked();
    void on_Browse_ExternalDataPath_pushButton_clicked();
    void on_AutoRelativePaths_checkBox_clicked(bool checked);
};

#endif // APPLICATION_SETTINGS_DLG_H
