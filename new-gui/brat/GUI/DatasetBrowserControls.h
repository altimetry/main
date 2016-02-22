#ifndef GUI_DATASETBROWSERCONTROLS_H
#define GUI_DATASETBROWSERCONTROLS_H


#include "new-gui/brat/GUI/ControlsPanel.h"


// File Index:
//
//	- Dataset Control Panel
//




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
    QComboBox *mDatasetsCombo = nullptr;

    CTextWidget *mFileDesc = nullptr;
    QGroupBox   *mFileDescGroup = nullptr;
    QListWidget *mFieldList = nullptr;
    CTextWidget *mFieldDesc = nullptr;

    CWorkspaceDataset *mWks = nullptr;


    QString mCurrentFilename;


    //construction / destruction

    void Wire();

public:
    explicit CDatasetBrowserControls( CDesktopManagerBase *manager, QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

    virtual ~CDatasetBrowserControls()
    {}

    // access

    // operations
    void ClearVarList();
    void FillFieldList();
    void SetFieldDesc();


signals:
    void FileChanged( QString path );

public slots:
    void WorkspaceChanged( CWorkspaceDataset *wksd );
    void DatasetChanged( int dataset_index );
    void FileChanged( int file_index );
    void FieldChanged( int field_index );

protected slots:
    void PageChanged( int index );
};






#endif // GUI_DATASETBROWSERCONTROLS_H
