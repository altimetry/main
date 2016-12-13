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
#ifndef GUI_CONTROL_PANELS_DATASETBROWSERCONTROLS_H
#define GUI_CONTROL_PANELS_DATASETBROWSERCONTROLS_H


#include "DesktopControlPanel.h"


class CTextWidget;




/////////////////////////////////////////////////////////////////////////////////////
//								Dataset Browser
/////////////////////////////////////////////////////////////////////////////////////

class CDatasetBrowserControls : public CDesktopControlsPanel
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

    QTreeWidget *mDatasetTree = nullptr;

    QToolButton *mNewDataset = nullptr;
    QToolButton *mDeleteDataset = nullptr;
    QToolButton *mAddDir = nullptr;
    QToolButton *mAddFiles = nullptr;
    QToolButton *mRemove = nullptr;
    QToolButton *mClear = nullptr;

    CTextWidget *mFileDesc = nullptr;
    QGroupBox   *mFileDescGroup = nullptr;
    QListWidget *mFieldList = nullptr;
    CTextWidget *mFieldDesc = nullptr;


	//...doamin data

	const CApplicationPaths &mBratPaths;
    CWorkspaceDataset *mWDataset = nullptr;


    //construction / destruction

    void Wire();

public:
    explicit CDatasetBrowserControls( CModel &model, CDesktopManagerBase *manager, QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

    virtual ~CDatasetBrowserControls()
    {}

    // access

    // operations
    void AddFiles(QStringList &paths_list);

    void DatasetChanged( QTreeWidgetItem *tree_item );
    virtual void FileChanged( QTreeWidgetItem *file_or_mission_item );

    QTreeWidgetItem *AddDatasetToTree(const QString &dataset_name);
    void FillFileTree( QTreeWidgetItem *current_dataset_item);
    void FillFieldList( CDataset *current_dataset, const std::string &current_file );
    void ClearFieldList();

	bool RenameDataset( QTreeWidgetItem *dataset_item );

signals:
    void CurrentDatasetChanged( CDataset* );
    void DatasetsChanged( const CDataset* );

public slots:
    void HandleWorkspaceChanged( CWorkspaceDataset *wksd );

    void HandleFieldChanged();
    void HandleNewDataset();
    void HandleDeleteDataset();
    void HandleAddFiles();
    void HandleAddDir();
    void HandleRemoveFile();
    void HandleClearFiles();
    void HandleTreeItemChanged();

protected slots:

    void HandleDatasetExpanded(); // Resizes DatasetTree when datasets are expanded
    void HandleItemChanged( QTreeWidgetItem *item, int col );
};






#endif // GUI_CONTROL_PANELS_DATASETBROWSERCONTROLS_H
