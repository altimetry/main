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

    QListWidget *mFilesList = nullptr;

    QPushButton *mAddDir = nullptr;
    QPushButton *mAddFiles = nullptr;
    QPushButton *mRemove = nullptr;
    QPushButton *mClear = nullptr;

    QComboBox *mDatasetsCombo = nullptr;

    CTextWidget *mFileDesc = nullptr;
    QGroupBox   *mFileDescGroup = nullptr;
    QListWidget *mFieldList = nullptr;
    CTextWidget *mFieldDesc = nullptr;

    QPushButton *mNewDataset = nullptr;
    QPushButton *mDeleteDataset = nullptr;
    QPushButton *mRenameDataset = nullptr;

    CWorkspaceDataset *mWks = nullptr;



    //construction / destruction

    void Wire();

public:
    explicit CDatasetBrowserControls( CDesktopManagerBase *manager, QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

    virtual ~CDatasetBrowserControls()
    {}

    // access

    // operations
	//    void DeleteDataset();

    void AddFiles(QStringList &paths_list);
    void FillFileList();
    void CheckFiles();

    void FillFieldList();
    void ClearFieldList();


signals:
    void FileChanged( QString path );

public slots:
    void WorkspaceChanged( CWorkspaceDataset *wksd );
    void DatasetChanged( int dataset_index );
    void FileChanged( int file_index );
    void FieldChanged();
    void OnNewDataset();
    void OnDeleteDataset();
    void OnAddFiles();
    void OnAddDir();
    void OnRemoveFile();
    void OnClearFiles();

protected slots:
    void PageChanged( int index );
};






#endif // GUI_CONTROL_PANELS_DATASETBROWSERCONTROLS_H
