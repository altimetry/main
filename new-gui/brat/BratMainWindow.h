#ifndef BRAT_MAIN_WINDOW_H
#define BRAT_MAIN_WINDOW_H

#include "DataModels/Workspaces/TreeWorkspace.h"

#include "ui_BratMainWindow.h"
#include "GUI/DesktopManager.h"
#include "GUI/LogShell.h"
#include "BratSettings.h"
#include "DataModels/Model.h"
#include "DataModels/Filters/BratFilters.h"


class CTabbedDock;
class CTextWidget;
class CRecentFilesProcessor;

class CControlPanel;
class CDatasetBrowserControls;
class CDatasetFilterControls;
class COperationControls;
class CProcessesTable;
class CAbstractDisplayEditor;

class CWorkspace;
class CBratFilters;


//net stop uxsms
//net start uxsms

class CBratMainWindow : public QMainWindow, public non_copyable, private Ui::CBratMainWindow
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

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


	// instance data

	CBratApplication &mApp;


    // MDI sub-windows logic
    //
	CDesktopManagerBase *mDesktopManager = nullptr;

    // Most recent files logic
    //
	QAction *mRecentFilesSeparatorAction = nullptr;
	CRecentFilesProcessor *mRecentFilesProcessor = nullptr;

    // Widgets
	//

	// Main Working Dock
	CTabbedDock *mMainWorkingDock = nullptr;

	CTabbedDock *mOutputDock = nullptr;
    CLogShell *mLogFrame = nullptr;
	int mLogFrameIndex = -1;
	CProcessesTable *mProcessesTable = nullptr;
	int mProcessesTableIndex = -1;

	// map related actions/widgets
	QProgressBar *mProgressBar = nullptr;
	QCheckBox *mRenderSuppressionCBox = nullptr;
	QAction *mActionDecorationGrid = nullptr;
	QAction *mActionSelectFeatures = nullptr;
	QAction *mActionSelectPolygon = nullptr;
	QAction *mActionDeselectAll = nullptr;
	QToolButton *mSelectionButton = nullptr;

    // Threading Lab
	QToolButton *mMessageButton = nullptr;
	QToolButton *mThreadsButton = nullptr;

    // Persistence
    //
	CBratSettings &mSettings;


    // Domain
    //
	CModel &mModel;


	/////////////////////////////
    // Construction / Destruction
    //

    virtual bool ReadSettings( bool &has_gui_settings, bool &wasMaximized, QByteArray &geom, QByteArray &state );
    virtual bool WriteSettings();

	// assume mManager created
	//
	CControlPanel *MakeWorkingPanel( ETabName tab );
	void CreateWorkingDock();
	void CreateOutputDock();
	void CreateDocks();

	// assume docks created
	//
	void ProcessMenu();
	void FillStatusBar();
	void FillStatusBarExperimental();

	// Operate in display only mode
	//
	bool StartDisplayMode();

public:
    explicit CBratMainWindow( CBratApplication &a );
	
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
		using panels_factory_t = std::tuple< CDatasetBrowserControls, CDatasetFilterControls, COperationControls >;

		using type = typename std::tuple_element< INDEX, panels_factory_t >::type;
	};

	template< ETabName INDEX >
	using TabType = typename ControlsPanelType< INDEX >::type;


	template< ETabName INDEX >
	TabType< INDEX >* WorkingPanel();

	template< class EDITOR >
	EDITOR* ActiveEditor();

    CAbstractDisplayEditor *ActiveMDIEditor();

    //
    //			Access
	/////////////////////////////



	/////////////////////////////////////////////////////
	// Business Logic
	// ............................ Workspace Management

private:
	void SetCurrentWorkspace( const CWorkspace *wks );
	bool DoNoWorkspace();
    bool SaveWorkspace();

	// .................................................
	
	//
	//
	/////////////////////////////////////////////////////


	void EmitWorkspaceChanged();
	
signals:

	////////////////////////////////////
	//			Signals
	////////////////////////////////////

	void WorkspaceChanged();
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
    virtual void closeEvent( QCloseEvent *event ) override;


protected slots:

    void LoadCmdLineFiles();

    // Update State
    //
    void WorkspaceChangedUpdateUI();
	void HandleSyncProcessExecution( bool executing );
	void EnableMapSelectionActions( bool enable );		//more a slot helper than a slot

    void UpdateWindowMenu();


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
	//Menu View

    void on_action_Zoom_In_triggered();
    void on_action_Zoom_Out_triggered();
    void on_action_Re_center_triggered();
    void on_action_Refresh_Map_triggered();
    void on_action_Graphic_Settings_triggered();
	void on_action_Satellite_Tracks_toggled( bool checked );
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

    void on_action_Views_List_triggered();


	////////////////
	//Menu Help

    void on_action_About_triggered();
    void on_action_Test_triggered();

    void on_action_Import_Workspace_triggered();
    void on_action_Rename_Workspace_triggered();
    void on_action_Delete_Workspace_triggered();


	///////////////////////////////
	// Status-bar slots
	///////////////////////////////



	///////////////////////////////
	// Display Mode slots
	///////////////////////////////

	void StopDisplayMode();
    void on_action_Close_Workspace_triggered();
    void on_action_Open_View_triggered();
};



#endif // BRAT_MAIN_WINDOW_H
