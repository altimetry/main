#ifndef GUI_CONTROL_PANELS_DATASETBROWSERCONTROLS_H
#define GUI_CONTROL_PANELS_DATASETBROWSERCONTROLS_H


#include "GUI/ControlPanels/ControlPanel.h"


class CTextWidget;


// TODO (DELETE): old Brat code at gui/DatasetPanel.cpp //


/////////////////////////////////////////////////////////////////////////////////////
//								Dataset Browser
/////////////////////////////////////////////////////////////////////////////////////

class CDatasetBrowserControls : public CDesktopControlsPanel
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

    using base_t = CDesktopControlsPanel;

    CStackedWidget *mBrowserStakWidget = nullptr;
    QPushButton *m_BrowseFilesButton = nullptr;
    QPushButton *m_BrowseRadsButton = nullptr;

    QTreeWidget *mDatasetTree = nullptr;

    QPushButton *mAddDir = nullptr;
    QPushButton *mAddFiles = nullptr;
    QPushButton *mRemove = nullptr;
    QPushButton *mClear = nullptr;

    CTextWidget *mFileDesc = nullptr;
    QGroupBox   *mFileDescGroup = nullptr;
    QListWidget *mFieldList = nullptr;
    CTextWidget *mFieldDesc = nullptr;

    QPushButton *mNewDataset = nullptr;
    QPushButton *mDeleteDataset = nullptr;
    QPushButton *mRenameDataset = nullptr;

    CWorkspaceDataset *mWDataset = nullptr;



    //construction / destruction

    void Wire();

public:
    explicit CDatasetBrowserControls( CDesktopManagerBase *manager, QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

    virtual ~CDatasetBrowserControls()
    {}

    // access

    // operations
    void AddFiles(QStringList &paths_list);
    void CheckFiles();

    void DatasetChanged(QTreeWidgetItem *tree_item );
    void FileChanged(QTreeWidgetItem *file_item );

    void AddDatasetToTree(const QString &dataset_name);
    void FillFileTree(QTreeWidgetItem *current_dataset_item);
    void FillFieldList(QTreeWidgetItem *current_file_item);
    void ClearFieldList();

signals:
    void CurrentDatasetChanged( CDataset* );

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
    void HandlePageChanged( int index );
    void HandleDatasetExpanded(); // Resizes DatasetTree when datasets are expanded
    void HandleRenameDataset(QTreeWidgetItem *dataset_item, int col);
};






#endif // GUI_CONTROL_PANELS_DATASETBROWSERCONTROLS_H
