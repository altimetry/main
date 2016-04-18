#ifndef APPLICATION_SETTINGS_DLG_H
#define APPLICATION_SETTINGS_DLG_H


#include "GUI/StackedWidget.h"


class CApplicationSettingsDlg : public QDialog
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

	Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	//types

	using base_t = QDialog;


	//instance data

    CStackedWidget *mStackedWidget = nullptr;

	QLineEdit *mDataDirectoryLineEdit = nullptr;
	QPushButton *mBrowseDataDirectoryPushButton = nullptr;
    QLineEdit *mProjectsDirectoryLineEdit = nullptr;
	QPushButton *mBrowseWorkspacesDirectoryPushButton = nullptr;
    QCheckBox *mUsePortablePathsCheckBox = nullptr;
    QWidget *mApplicationPathsPage = nullptr;

    QCheckBox *mLoadLastProjectAtAtartupCheckBox = nullptr;
	QRadioButton *mUseVectorLayer = nullptr;
	QRadioButton *mUseRasterLayer = nullptr;
	QWidget *mStartupOptionsPage = nullptr;

    QListWidget *mStylesListWidget = nullptr;
    QCheckBox *mDefaultStyleCheckBox = nullptr;
    QWidget *mApplicationStylesPage = nullptr;

	QCheckBox *mDesktopManagerSdiCheckbox = nullptr;

    QDialogButtonBox *mButtonBox = nullptr;


	CBratSettings &mSettings;


	//construction / destruction

	void CreateWidgets();
	void Wire();
public:
    explicit CApplicationSettingsDlg( CBratSettings &options, QWidget *parent );

	
	//remaining methods

protected:
    bool ValidateAndAssign();

private slots:
    virtual void accept();

    void HandleBrowseDataDirectory();
    void HandleBrowseProjectsPath();
};

#endif // APPLICATION_SETTINGS_DLG_H
