#ifndef BRAT_MAIN_WINDOW_H
#define BRAT_MAIN_WINDOW_H

#include "Workspaces/TreeWorkspace.h"

#include "ui_BratMainWindow.h"
#include "new-gui/brat/GUI/AbstractEditor.h"
#include "new-gui/brat/GUI/DesktopManager.h"
#include "ApplicationSettings.h"


class CTabbedDock;
class CTextWidget;
class CRecentFilesProcessor;

class CWorkspace;



//net stop uxsms
//net start uxsms

class CBratMainWindow : public QMainWindow, public non_copyable, private Ui::CBratMainWindow
{
	Q_OBJECT

	// Types & Friends

	using base_t = QMainWindow;


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
	CTreeWorkspace mTree;
	CTreeWorkspace *mCurrentTree = nullptr;


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
	void CreateDocks();
	void CreateWorkingDock();
	void CreateOutputDock();
	void FillStatusBar();
	void FillStatusBarExperimental();

	void finishOldInitialization();

public:
    explicit CBratMainWindow( CApplicationSettings &settings );
	
	virtual ~CBratMainWindow();
    //
    // Construction / Destruction
	/////////////////////////////


	/////////////////////////////////////////////////////
	// Business Logic
	// ............................ Workspace Management

private:
	template< class WKSPC >
	WKSPC* GetCurrentWorkspace();

public:
	CWorkspace* GetCurrentRootWorkspace();
	const CWorkspace* GetCurrentRootWorkspace() const
	{
		return const_cast< CBratMainWindow* >( this )->GetCurrentRootWorkspace();
	}

private:
	void DoEmptyWorkspace();
	void CreateTree( CWorkspace *root, CTreeWorkspace &tree );
	void CreateTree( CWorkspace *root )
	{
		CreateTree( root, mTree );
	}

	// GUI Management
	void AddWorkspaceToHistory( const std::string& name );
	void SetTitle( CWorkspace *wks = nullptr );
	void EnableCtrlWorkspace();
	void LoadWorkspace();
	void ResetWorkspace();

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


	////////////////////////////////////
	// Not triggered by Menu / Tool-bars
	////////////////////////////////////

public slots:

    bool OpenWorkspace( const QString &qpath );

protected:
	void LoadCmdLineFiles();
    void clipboardDataChanged();

    // font / format
    //
    void cursorPositionChanged();

	// Update Actions
    //
	void UpdateWindowMenu();


    void closeEvent(QCloseEvent *event);

    virtual bool ReadSettings( bool &has_gui_settings );
    virtual bool WriteSettings();

	template< class EDITOR >
	EDITOR* activeEditor();
    //CTextEditor *activeTextEditor();
    CEditorBase *activeMDIEditor();
    //void addEditor(CTextEditor *editor);


    //not slots: manually called
    //
    void updateRecentFileActions();
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
    void on_action_Zom_Out_triggered();
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

};



#endif // BRAT_MAIN_WINDOW_H
