#ifndef BRAT_MAIN_WINDOW_H
#define BRAT_MAIN_WINDOW_H

#include "Workspaces/TreeWorkspace.h"

#include "ui_BratMainWindow.h"
#include "new-gui/brat/GUI/AbstractEditor.h"
#include "new-gui/brat/GUI/DesktopManager.h"
#include "ApplicationSettings.h"
#include "Model.h"


class CTabbedDock;
class CTextWidget;
class CRecentFilesProcessor;

class CDatasetBrowserControls;
class CDatasetFilterControls;
class COperationsControls;


class CWorkspace;



//net stop uxsms
//net start uxsms

class CBratMainWindow : public QMainWindow, public non_copyable, private Ui::CBratMainWindow
{
	Q_OBJECT

	// Types & Friends

	using base_t = QMainWindow;


public:
	enum ETabName
	{
		eDataset,
		eFilter,
		eOperations,

		ETabNames_size
	};


    // Statics
    //

	static CBratMainWindow *smInstance;

public:
    static const CBratMainWindow& instance()
	{
		assert__( smInstance );

		return *smInstance;
	}

    static bool destroyed()
	{
		return smInstance == nullptr;
	}


private:

	//returns the application name if title is empty
	//
	static QString makeWindowTitle( const QString &title = QString() );

    // MDI sub-windows logic
    //
	desktop_manager_t *mManager = nullptr;

    // Most recent files logic
    //
	QAction *mRecentFilesSeparatorAction = nullptr;
	CRecentFilesProcessor *mRecentFilesProcessor = nullptr;

    // Widgets
	//

	//Main Working Dock
	CTabbedDock *mMainWorkingDock = nullptr;

	CTabbedDock *mOutputDock = nullptr;
	CTextWidget *mOutputTextWidget = nullptr;

    // Business logic
    //
	CModel mModel;


    // Threading Lab
    //
	QProgressBar *mProgressBar = nullptr;
	QToolButton *mMessageButton = nullptr;
	QToolButton *mThreadsButton = nullptr;

    // Persistence
    //
	CApplicationSettings &mSettings;


	/////////////////////////////
    // Construction / Destruction
    //

	// assume mManager created
	//
	CControlsPanel* MakeWorkingPanel( ETabName tab );
	void CreateWorkingDock();
	void CreateOutputDock();
	void CreateDocks();

	// assume docks created
	//
	void ProcessMenu();
	void FillStatusBar();
	void FillStatusBarExperimental();

public:
    explicit CBratMainWindow( CApplicationSettings &settings );
	
	virtual ~CBratMainWindow();
    //
    // Construction / Destruction
	/////////////////////////////



	/////////////////////////////
    //		GUI MANAGEMENT
    //
	
	template< CBratMainWindow::ETabName INDEX >
	struct ControlsPanelType
	{
		using panels_factory_t = std::tuple< CDatasetBrowserControls, CDatasetFilterControls, COperationsControls >;

		using type = typename std::tuple_element< INDEX, panels_factory_t >::type;
	};

	template< ETabName INDEX >
	using TabType = typename ControlsPanelType< INDEX >::type;




	template< ETabName INDEX >
	TabType< INDEX >* WorkingPanel();

    //
    //			Access
	/////////////////////////////



	/////////////////////////////////////////////////////
	// Business Logic
	// ............................ Workspace Management

private:
	void SetCurrentWorkspace( const CWorkspace *wks );

	bool DoEmptyWorkspace();

	// GUI Management
	void EnableCtrlWorkspace();
	void LoadWorkspace();
	void ResetWorkspace();
    bool SaveWorkspace();

    // Business Logic
    //
	//void CreateWPlot( CWPlot* wplot, QSize& size, QPoint& pos );
	//void WorldPlot();
	void XYPlot();
	void ZFXYPlot();

	// .................................................
	
	//
	//
	/////////////////////////////////////////////////////


	void EmitWorkspaceChanged();
	
signals:

	////////////////////////////////////
	//			Signals
	////////////////////////////////////

	void WorkspaceChanged( const CModel *model );
	void WorkspaceChanged( CWorkspaceDataset *wksd );
	void WorkspaceChanged( CWorkspaceOperation *wkso );
	void WorkspaceChanged( CWorkspaceDisplay *wksd );

	void SettingsUpdated();

	////////////////////////////////////
	//			Slots
	////////////////////////////////////
	// Not triggered by Menu / Tool-bars
	////////////////////////////////////

public slots:

    bool OpenWorkspace( const std::string &path );

protected:

	bool OkToContinue();
    void closeEvent(QCloseEvent *event);

    virtual bool ReadSettings( bool &has_gui_settings );
    virtual bool WriteSettings();

	template< class EDITOR >
	EDITOR* activeEditor();
    //CTextEditor *activeTextEditor();
    CEditorBase *activeMDIEditor();
    //void addEditor(CTextEditor *editor);


protected slots:

    void LoadCmdLineFiles();
    void clipboardDataChanged();

    // font / format
    //
    void cursorPositionChanged();

    // Update Actions
    //
    void UpdateWorkspaceUI( const CModel *model );

    void UpdateWindowMenu();


    // do far, not slots: manually called
    //
    //void updateRecentFileActions();
    //void connectAutoUpdateEditActions( CTextEditor* pe );

private slots:

	///////////////////////////////
	// Menu / Tool-bars actions
	///////////////////////////////

	////////////////
	//Menu Workspace

    void on_action_New_triggered();
    void on_action_Open_triggered();
    void on_action_Save_triggered();
    void on_action_Save_As_triggered();
	void openRecentWorkspace_triggered( QAction *action );


	////////////////
	//Menu Edit			(experimental)

    void on_action_Undo_triggered();
    void on_action_Redo_triggered();
    void on_action_Cut_triggered();
    void on_action_Copy_triggered();
    void on_action_Paste_triggered();
    void on_action_Delete_triggered();
    void on_action_Select_All_triggered();


	////////////////
	//Menu View

    void on_action_Zoom_In_triggered();
    void on_action_Zoom_Out_triggered();
    void on_action_Re_center_triggered();
    void on_action_Refresh_Map_triggered();
    void on_action_Graphic_Settings_triggered();
    void on_action_Save_Map_Image_triggered();
    void on_action_Full_Screen_triggered();


	////////////////
	//Menu Tools

    void on_action_One_Click_triggered();
    void on_action_Workspace_Tree_triggered();
    void on_action_Launch_Scheduler_triggered();
    void on_action_Options_triggered();


	////////////////
	//Menu Window

    void on_action_Close_triggered();
	//
    void on_action_Tile_triggered();
    void on_action_Cascade_triggered();
    void on_action_Next_triggered();
    void on_action_Previous_triggered();


	////////////////
	//Menu Help

    void on_action_About_triggered();
    void on_action_Test_triggered();

    void on_action_Import_Workspace_triggered();
    void on_action_Rename_Workspace_triggered();
    void on_action_Delete_Workspace_triggered();
};



#endif // BRAT_MAIN_WINDOW_H
