/*
* This file is part of BRAT
*
* BRAT is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* BRAT is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#ifndef APPLICATION_SETTINGS_DLG_H
#define APPLICATION_SETTINGS_DLG_H


#include "GUI/StackedWidget.h"



class QtServiceController;



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
    QRadioButton *mUseRasterLayerURI = nullptr;
    QLineEdit *mLayerURLLineEdit = nullptr;
	QRadioButton *mViewsUseVectorLayer = nullptr;
	QRadioButton *mViewsUseRasterLayer = nullptr;
    QRadioButton *mViewsUseRasterLayerURI = nullptr;
	QCheckBox *mVectorSimplifyMethodCheck = nullptr;
	QWidget *mStartupOptionsPage = nullptr;

#if (BRAT_MINOR_VERSION_INT==1)
	QLineEdit *mRadsOutputEdit = nullptr;
	QPushButton *mRadsPathBrowseButton = nullptr;
	QSpinBox *mRadsSpin = nullptr;
	QListWidget *mRadsMissionsList = nullptr;
	QGroupBox *mRadsCommandsBox = nullptr;
	QToolButton *mRadsInstallButton = nullptr;
	QToolButton *mRadsUninstallButton = nullptr;
	QToolButton *mRadsStartButton = nullptr;
	QToolButton *mRadsStopButton = nullptr;
	QToolButton *mRadsPauseButton = nullptr;
	QToolButton *mRadsResumeButton = nullptr;
	QPushButton *mRadsViewLogFile = nullptr;
	QWidget *mRadsOptionsPage = nullptr;
#endif

    QListWidget *mStylesListWidget = nullptr;
    QCheckBox *mDefaultStyleCheckBox = nullptr;
    QWidget *mApplicationStylesPage = nullptr;

	QCheckBox *mDesktopManagerSdiCheckbox = nullptr;

    QDialogButtonBox *mButtonBox = nullptr;

	QtServiceController *mRadsController = nullptr;
	CBratSettings &mSettings;


	//construction / destruction

	void CreateWidgets();
	void Wire();
public:
    explicit CApplicationSettingsDlg( CBratSettings &options, QWidget *parent );
	virtual ~CApplicationSettingsDlg();

	//remaining methods

protected:
    bool ValidateAndAssign();

#if (BRAT_MINOR_VERSION_INT==1)
	void EnableRadsButtons();
	void DisplayRadsError( const std::string &action );
#endif

private slots:
    virtual void accept();

    void HandleBrowseDataDirectory();
    void HandleBrowseProjectsPath();
    void HandleMainLayerTypeChanged( bool toggled );
    void HandleViewsLayerTypeChanged( bool toggled );

#if (BRAT_MINOR_VERSION_INT==1)
	void HandleRadsPathBrowse();
	void HandleRadsInstall();
	void HandleRadsUninstall();
	void HandleRadsStart();
	void HandleRadsStop();
	void HandleRadsPause();
	void HandleRadsResume();
	void HandleViewLogFile();
#endif
};

#endif // APPLICATION_SETTINGS_DLG_H
