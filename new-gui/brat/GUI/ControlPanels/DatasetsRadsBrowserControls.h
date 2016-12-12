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
#ifndef GUI_CONTROL_PANELS_RADS_BROWSERCONTROLS_H
#define GUI_CONTROL_PANELS_RADS_BROWSERCONTROLS_H


#include "DesktopControlPanel.h"
#include "RadsDatasetsTreeWidget.h"
#include "BratApplication.h"


class CTextWidget;




/////////////////////////////////////////////////////////////////////////////////////
//								Dataset Browser
/////////////////////////////////////////////////////////////////////////////////////

class CRadsBrowserControls : public CDesktopControlsPanel
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

    using base_t = CDesktopControlsPanel;


	//instance data

	CRadsDatasetsTreeWidget *mDatasetTree = nullptr;

    QToolButton *mNewDataset = nullptr;
    QToolButton *mDeleteDataset = nullptr;

    CTextWidget *mFileDesc = nullptr;
    QGroupBox   *mFileDescGroup = nullptr;
    QListWidget *mFieldList = nullptr;
    CTextWidget *mFieldDesc = nullptr;


	//...doamin data

    CBratApplication &mApp;
	const CApplicationPaths &mBratPaths;
	const CSharedRadsSettings &mRadsServiceSettings;
	CWorkspaceDataset *mWDataset = nullptr;
	QSharedMemory mSharedMemory;

   

    //construction / destruction

    void Wire();

public:
    explicit CRadsBrowserControls( CBratApplication &app, CDesktopManagerBase *manager, QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~CRadsBrowserControls();

    // access

    // operations
    void AddFiles(QStringList &paths_list);

    void DatasetChanged( QTreeWidgetItem *tree_item );
    virtual void FileChanged( QTreeWidgetItem *tree_item );

    QTreeWidgetItem *AddDatasetToTree(const QString &dataset_name);

    void FillFieldList( CDataset *current_dataset, const std::string &current_file_or_mission );
    void ClearFieldList();

	bool RenameDataset( QTreeWidgetItem *dataset_item );

protected:

	bool CheckRadsConfigStatus();

signals:
    void CurrentDatasetChanged( CDataset* );
    void DatasetsChanged( const CDataset* );

public slots:
    void HandleWorkspaceChanged( CWorkspaceDataset *wksd );

    void HandleFieldChanged();
    void HandleNewDataset();
    void HandleDeleteDataset();
    void HandleTreeItemChanged();

protected slots:

    void HandleDatasetExpanded(); // Resizes DatasetTree when datasets are expanded
    void HandleItemChanged( QTreeWidgetItem *item, int col );
	void HandleCurrentIndexChanged( CRadsDatasetsTreeWidgetItem*, int index );
    
    void HandleRsyncStatusChanged( CBratApplication::ERadsNotification notification );
};






#endif // GUI_CONTROL_PANELS_RADS_BROWSERCONTROLS_H
