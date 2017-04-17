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
#ifndef BRAT_MAIN_WINDOW_H
#define BRAT_MAIN_WINDOW_H

#include "DataModels/Workspaces/TreeWorkspace.h"

#include "ui_BratMainWindow.h"
#include "GUI/DesktopManager.h"
#include "GUI/LogShell.h"
#include "BratSettings.h"
#include "DataModels/Model.h"
#include "DataModels/Workspaces/Display.h"
#include "DataModels/Filters/BratFilters.h"


class CTabbedDock;
class CTextWidget;
class CRecentFilesProcessor;

class CControlPanel;
class CDatasetsBrowserControls;
class CDatasetsRadsBrowserControls;
class CBratFilterControls;
class COperationControls;
class CProcessesTable;
class CAbstractDisplayEditor;

class CWorkspace;
class CBratFilters;





//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//									Brat Main Window
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////


//net stop uxsms
//net start uxsms

class CBratMainWindow : public QMainWindow, public non_copyable, private Ui::CBratMainWindow
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

    // Types & Friends

	using base_t = QMainWindow;


public:
	enum ETabName
	{
		eDataset,
		eRADS,
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
	static QString MakeWindowTitle( const QString &title = QString() );


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
	QToolButton *mCoordinatesFormat = nullptr;
	QLineEdit *mCoordsEdit = nullptr;
	QCheckBox *mRenderSuppressionCBox = nullptr;
	QAction *mActionDecorationGrid = nullptr;
    QAction *mActionMapTips = nullptr;
	QAction *mActionSelectFeatures = nullptr;
#if defined(ENABLE_POLYGON_SELECTION)
	QAction *mActionSelectPolygon = nullptr;
#endif
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

	// Operate in display only mode
	//
	bool StartDisplayMode();

    // Operate in instant save mode
    //
    void StartInInstantPlotSaveMode( const std::string &display_name, const std::string &path, bool is_3d, CDisplayData::EImageExportType type);

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
		using panels_factory_t = std::tuple< CDatasetsBrowserControls, CDatasetsRadsBrowserControls, CBratFilterControls, COperationControls >;

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

	QWidget* OpenDisplay( CDisplay *display, bool maps_as_plots );


protected slots:

    bool CheckInstantPlotSave( const QStringList &args, const QString &wkspc_dir,
                               std::string &display_name, std::string &path, bool &is_3d, CDisplayData::EImageExportType &type );
    void LoadCmdLineFiles();

    // Update State
    //
    void WorkspaceChangedUpdateUI();
	void HandleSyncProcessExecution( bool executing );
	void HandleAsyncProcessExecution( bool executing );
	void EnableMapSelectionActions( bool enable );		//more a slot helper than a slot

    void UpdateToolsMenu();
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
	void openRecentWorkspace_triggered( QAction *action );
    void on_action_Close_Workspace_triggered();

    void on_action_Import_Workspace_triggered();
    void on_action_Rename_Workspace_triggered();
    void on_action_Delete_Workspace_triggered();
    

	////////////////
	//Menu View

    void on_action_Zoom_In_triggered();
    void on_action_Zoom_Out_triggered();
    void on_action_Re_center_triggered();
    void on_action_Refresh_Map_triggered();
	void on_action_Satellite_Tracks_toggled( bool checked );
    void on_action_Save_Map_Image_triggered();
    void on_action_Full_Screen_triggered();


	////////////////
	//Menu Tools

    void on_action_One_Click_triggered();
    void on_action_Workspace_Tree_triggered();
    void on_action_Launch_Scheduler_triggered();
    void on_action_Options_triggered();
    void on_action_Operation_Views_triggered();
#if defined (_DEBUG) || defined (DEBUG)
	void PythonConsoleError( QProcess::ProcessError error );
	void on_action_Python_Console_triggered();
#endif

	////////////////
	//Menu Window

    void on_action_Views_List_triggered();
    void on_action_Open_View_triggered();


	////////////////
	//Menu Help

    void on_action_User_s_Manual_triggered();
    void on_action_Youtube_Video_Tutorials_triggered();
    void on_action_About_triggered();
    void on_action_Test_triggered();



	///////////////////////////////
	// Status-bar slots
	///////////////////////////////


	void OutputDockVisibilityChanged( bool );


	///////////////////////////////
	// Main dock slots
	///////////////////////////////


	void TabSelected( int index );


	///////////////////////////////
	// Display Mode slots
	///////////////////////////////

	void StopDisplayMode();
};






QT_BEGIN_NAMESPACE
class QSslError;
QT_END_NAMESPACE

QT_USE_NAMESPACE


class RemoteCounter: public QObject
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

	Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

    // Types

	using base_t = QObject;
    friend class CBratMainWindow;
    
    // statics 
    
    static RemoteCounter *smRemoteCounter;

    // instance data    
    
	QNetworkAccessManager mManager;
	QList<QNetworkReply *> mCurrentReplies;

public:
    // construction / destruction
    
	RemoteCounter();
    virtual ~RemoteCounter();
    
    
    //
    
	void Count();

public slots:
	void downloadFinished(QNetworkReply *reply);
	void sslErrors(const QList<QSslError> &errors);
};





#if defined(USE_WEB_ENGINE)

//////////////////////////////////////////////////////////////////////////////////////////////
//									Remote Counter
//////////////////////////////////////////////////////////////////////////////////////////////

class RemoteCounterPage : public QWebEnginePage
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

	Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	// Types

	using base_t = QWebEnginePage;

	// Data

	std::string mUser;
	std::string mPass;

public:
	RemoteCounterPage( const std::string &user, const std::string &pass, QWebEngineProfile *profile, QObject *parent = nullptr );
	virtual ~RemoteCounterPage();

protected:
	virtual bool certificateError(const QWebEngineCertificateError &error) override;
	void FillAuth( QAuthenticator *auth );

	private slots:
	void handleAuthenticationRequired( const QUrl &requestUrl, QAuthenticator *auth );
	void handleProxyAuthenticationRequired( const QUrl &requestUrl, QAuthenticator *auth, const QString &proxyHost );
};


class RemoteCounterView : public QWebEngineView
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

	Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	// Types & Friends

	using base_t = QWebEngineView;

	friend class CBratMainWindow;

	// Statics

	static RemoteCounterView *smRemoteCounterView;

public:
	RemoteCounterView();
	virtual ~RemoteCounterView();

	void RemoteCount();
};

#endif



#endif // BRAT_MAIN_WINDOW_H
