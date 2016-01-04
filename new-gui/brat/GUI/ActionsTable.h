#ifndef ACTIONS_TABLE_H
#define ACTIONS_TABLE_H


class CApplicationSettings;



//////////////////////////////////////////////////////////////////////////////////
//					Application Global QActions
//////////////////////////////////////////////////////////////////////////////////


enum EActionTag : int
{	
	eAction_Separator,

	eAction_RecentFile1, eFirstRecentFileAction = eAction_RecentFile1,
	eAction_RecentFile2, eAction_RecentFile3, eAction_RecentFile4, 
	eAction_RecentFile5, eAction_RecentFile6, eAction_RecentFile7, 
	eAction_RecentFile8, eAction_RecentFile9, 
	eAction_RecentFile10, eLastRecentFileAction = eAction_RecentFile10,

	eAction_Exit,
	eAction_Open,
	eAction_New,
	eAction_Save,
	eAction_Save_As,
	eAction_About,
	eAction_Close,
	eAction_Close_All,
	eAction_Tile,
	eAction_Cascade,
	eAction_Cut,
	eAction_Copy,
	eAction_Paste,
	eAction_Next,
	eAction_Previous,
	eAction_Options,
	eAction_Undo,
	eAction_Redo,
	eAction_Delete,
	eAction_Select_All,
	eAction_Test,
	eAction_One_Click,
	eAction_Launch_Scheduler,
	eAction_Workspace_Tree,
	eAction_Save_Map_Image,
	eAction_Refresh_Map,
	eAction_Graphic_Settings,
	eAction_Full_Screen,
	eAction_Re_center,
	eAction_Zoom_In,
	eAction_Zom_Out,
	eAction_Operations,
	eAction_Dataset,
	eAction_Filter, eLastAutomaticAction = eAction_Filter,

	eAction_DisplayEditor2D,
	eAction_DisplayEditor3D,
	eAction_DisplayEditorLog,
	eAction_DisplayEditorDock,

	eAction_MapEditorDistance,
	eAction_MapEditorArea,
	eAction_MapEditorMean,
	eAction_MapEditorStDev,
	eAction_MapEditorLinearRegression,

	eActionGroup_Projections,
	eAction_Projection1,
	eAction_Projection2,
	eAction_Projection3,


	//add new items before this line

	EActionTags_size
};



struct CActionInfo
{
	EActionTag mActionTag;
	std::string mName;
	std::string mTip;
	std::string mIconPath;
	std::string mOnIconPath;
	std::string mShortcut;
	QList<QAction*> mActions;

	CActionInfo( EActionTag ActionTag, const std::string &Name, const std::string &Tip, const std::string &IconPath, 
		const std::string &OnIconPath = "", const std::string &Shortcut = "" ) :
		mActionTag( ActionTag ), mName( Name ), mTip( Tip ), mIconPath( IconPath ), mOnIconPath( OnIconPath ), mShortcut( Shortcut )
	{}


	static const CActionInfo& ActionInfo( EActionTag tag );

private:
    static QAction* SetActionProperties( QAction *a, EActionTag tag );

public:
	static QAction* CreateAction( QObject *parent, EActionTag tag );


	static QAction* SetDockActionProperties( QDockWidget *dock, EActionTag tag );


	static QToolButton* CreateMenuButton( EActionTag button_tag, const QList<QAction*> &actions );


private:	//not used
	static QToolButton* CreateMenuButton( QObject *parent, EActionTag button_tag, const std::vector<EActionTag> &tags );


public:
	static QActionGroup* CreateActionsGroup( QWidget *group_parent, const std::vector<EActionTag> &tags, bool exclusive );
};






//////////////////////////////////////////////////////////////////////////////////
//						Most Recent Files QActions
//////////////////////////////////////////////////////////////////////////////////

class CRecentFilesProcessor : public QObject
{
	Q_OBJECT


	// DATA

	static const size_t smMaxRecentFiles = eLastRecentFileAction - eFirstRecentFileAction + 1;

	QMenu *mRecentFilesMenu = nullptr;
	QAction *mRecentFileActions[ smMaxRecentFiles ];
	QStringList mRecentFiles;
	const QString mSettingsSectionName;

	// CONSTRUCTION / DESTRUCTION

	void SetupMenu( QMainWindow *parent, QAction *ActionAfter );
public:
	CRecentFilesProcessor( QMainWindow *parent, const std::string MenuName, QMenu *MenuParent, QAction *ActionAfter, const QString &SettingsSectionName );

	CRecentFilesProcessor( QMainWindow *parent, QMenu *RecentFiles_menu, QAction *ActionAfter, const QString &SettingsSectionName );

	virtual ~CRecentFilesProcessor()
	{}


	// ACCESS

	size_t size() const							//can be used by LoadLastFileAtStartUp logic
	{
		return (size_t)mRecentFiles.size();
	}
	QString path( const size_t i ) const		//can be used by LoadLastFileAtStartUp logic
	{
		assert__( i < size() );

		return mRecentFileActions[ i ]->data().toString();
	}


	// UPDATE: to be called when file is successfully opened

	void setCurrentFile( const QString &curFile )
	{
		mRecentFiles.removeAll( curFile );
		mRecentFiles.prepend( curFile );
		UpdateRecentFileActions();
	}


	// PERSISTENCE  

	//	... using Qt settings logic

	void ReadSettings( const QSettings &settings );

	void WriteSettings( QSettings &settings );

	// ... using BRAT v3 logic

	virtual bool ReadSettings();

	virtual bool WriteSettings();


protected:
	// IMPLEMENTATION details

	template< class CONTAINER >
	bool LoadRecentFilesGeneric( CONTAINER &paths );

	template< class CONTAINER >
	bool SaveRecentFilesGeneric( const CONTAINER &paths );

	void UpdateRecentFileActions();

signals:

	void triggered( QAction *action );
	void updatedRecentFiles();

private slots:

	void openRecentFile()
	{
		QAction *action = qobject_cast< QAction*>( sender() );
		if ( action )
			emit triggered( action );
	}
};




#endif // ACTIONS_TABLE_H
