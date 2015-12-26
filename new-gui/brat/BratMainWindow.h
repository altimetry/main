#ifndef BRAT_MAIN_WINDOW_H
#define BRAT_MAIN_WINDOW_H

#include "Workspaces/TreeWorkspace.h"

#include "ui_BratMainWindow.h"
#include "new-gui/brat/GUI/AbstractEditor.h"
#include "new-gui/brat/GUI/DesktopManager.h"

class CTabbedDock;
class CTextWidget;

class CWorkspace;



//net stop uxsms
//net start uxsms

class CBratMainWindow : public QMainWindow, private Ui::CBratMainWindow
{
	Q_OBJECT

	// Types & Friends

	using base_t = QMainWindow ;

#if defined(Q_OS_MACX)
    using desktop_manager_t = CDesktopManager;
#else
    using desktop_manager_t = CDesktopManagerMDI;
#endif

	friend int main( int argc, char *argv[] );


    // Statics
    //

	//returns the application name if title is empty
	//
	static QString makeWindowTitle( const QString &title = QString() );

    // MDI sub-windows logic
    //
	desktop_manager_t *mManager = nullptr;

    // Most recent files logic
    //

    // Widgets
	//

	//Main Working Dock
	CTabbedDock *mMainWorkingDock = nullptr;
	CTextWidget *mToolTextWidget = nullptr;

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

	/////////////////////////////
    // Construction / Destruction
    //
	void CreateDocks();
	void CreateWorkingDock();
	void CreateOutputDock();
	void FillStatusBar();
	void FillStatusBarExperimental();

public:
    explicit CBratMainWindow( QWidget *parent = nullptr );
	
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

	CWorkspace* GetCurrentRootWorkspace();

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

public slots:

    bool OpenWorkspace( const QString &qpath );

protected:
    void closeEvent(QCloseEvent *event);

    void readSettings();
    void writeSettings();

	template< class EDITOR >
	EDITOR* activeEditor();
    //CTextEditor *activeTextEditor();
    CEditorBase *activeMDIEditor();
    //void addEditor(CTextEditor *editor);


    //not slots: manually called
    //
    void updateRecentFileActions();
    //void connectAutoUpdateEditActions( CTextEditor* pe );

    // file print
    //
    void setupPrintActions();

private slots:
    void LoadCmdLineFiles();

    void on_action_New_triggered();
    void on_action_Open_triggered();
    void on_action_Save_triggered();
    void on_action_Save_As_triggered();
    void on_action_Undo_triggered();
    void on_action_Redo_triggered();
    void on_action_Cut_triggered();
    void on_action_Copy_triggered();
    void on_action_Paste_triggered();
    void on_action_Delete_triggered();
    void on_action_Select_All_triggered();
    void on_action_Read_Only_triggered();
    void on_action_Options_triggered();
    void on_action_About_triggered();

    void clipboardDataChanged();

    // font / format
    //
    void cursorPositionChanged();

    // file print
    //
    void filePrint();
    void filePrintPreview();
    void filePrintPdf();

    // threaded tests
    //
};



#define NOT_IMPLEMENTED SimpleMsgBox( "Not implemented." );


#endif // BRAT_MAIN_WINDOW_H
