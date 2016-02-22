#include "new-gui/brat/stdafx.h"

#include <QAction>

#include "new-gui/Common/ConfigurationKeywords.h"
#include "new-gui/Common/QtUtilsIO.h"

#include "new-gui/Common/ApplicationSettings.h"

#include "ui_BratMainWindow.h"
#include "ActionsTable.h"




//////////////////////////////////////////////////////////////////////////////////
//				Application Global QActions - The Actions Table
//////////////////////////////////////////////////////////////////////////////////



CActionInfo ActionsTable[ EActionTags_size ] =
{
	{ eAction_Separator, "", "", "", "", "" },

	{ eAction_RecentFile1, "", "", "", "", "" }, { eAction_RecentFile2, "", "", "", "", "" }, 
	{ eAction_RecentFile3, "", "", "", "", "" }, { eAction_RecentFile4, "", "", "", "", "" }, 
	{ eAction_RecentFile5, "", "", "", "", "" }, { eAction_RecentFile6, "", "", "", "", "" },
	{ eAction_RecentFile7, "", "", "", "", "" }, { eAction_RecentFile8, "", "", "", "", "" }, 
	{ eAction_RecentFile9, "", "", "", "", "" }, { eAction_RecentFile10, "", "", "", "", "" },

    { eAction_Exit, "&Exit", "Exit brat", ":/images/OSGeo/exit.png" },

    { eAction_Open, "&Open...", "Open\nOpen existing workspace", ":/images/OSGeo/open.png", "", "Ctrl+O" },

    { eAction_New, "&New...", "New\nCreate a new workspace", ":/images/OSGeo/new.png", "", "Ctrl+N" },

	{ eAction_CloseWorkspace, "&Close", "Close and unload the workspace", "://images/alpha-numeric/__q.png" },

    { eAction_Save, "&Save","Save\nSave the workspace to disk", ":/images/OSGeo/save.png", "", "Ctrl+S" },

    { eAction_Save_As, "Save &As...", "Save As", ":/images/OSGeo/save-as.png" },

    { eAction_Import_Workspace, "&Import...", "Import\nImport existing workspace objects", ":/images/OSGeo/import.png" },

    { eAction_Rename_Workspace, "&Rename...", "Rename\nRename the loaded workspace\n""The workspace directory will not change", "" },

    { eAction_Delete_Workspace, "&Delete...", "Delete\nDelete the loaded workspace", ":/images/OSGeo/workspace-delete.png" },

    { eAction_About, "&About...", "About", ":/images/BratIcon.png" },

    { eAction_Close, "&Close", "Close", ":/images/OSGeo/quit.png" },

	{ eAction_Close_All, "Close &All", "Close All", "" },

	{ eAction_Tile, "&Tile", "Tile", "" },

	{ eAction_Cascade, "Casca&de", "Cascade", "" },

    { eAction_Cut, "Cut", "Cut", ":/images/OSGeo/edit-cut.png" },

    { eAction_Copy, "&Copy", "Copy", ":/images/OSGeo/edit-copy.png" },

    { eAction_Paste, "&Paste", "Paste", ":/images/OSGeo/edit-paste.png" },

	{ eAction_Next, "&Next", "Next", "" },

	{ eAction_Previous, "&Previous", "Previous", "" },

    { eAction_Options, "&Options...", "Options\nEdit application settings", ":/images/OSGeo/settings.png" },		//icon 5.png is below TODO: delete this comment

	{ eAction_Undo, "&Undo", "Undo", "", "", "Ctrl+Z" },

	{ eAction_Redo, "&Redo", "Redo", "", "", "Ctrl+Y" },

	{ eAction_Delete, "&Delete", "Delete", "" },

	{ eAction_Select_All, "Select &All", "Select All", "" },

	{ eAction_Test, "Test...", "Test", ":/images/test.png" },

    { eAction_One_Click, "One-&Click...", "One-Click\nGroup actions as a single operation under a single name", ":/images/alpha-numeric/7.png" },

    { eAction_Launch_Scheduler, "Launch Scheduler", "Brat Scheduler\nLaunch Scheduler application", ":/images/alpha-numeric/8.png" },

    { eAction_Workspace_Tree, "Workspace Tree...", "Workspace Tree\nSee the objects tree of the loaded workspace", ":/images/OSGeo/page-info.png" },

    { eAction_Save_Map_Image, "Save Map Image...", "Save Map Image\nSave map to file", ":/images/OSGeo/map-export.png" },

    { eAction_Refresh_Map, "Refresh", "Refresh\nRefresh main map", ":/images/OSGeo/zoom-refresh.png" },

    { eAction_Graphic_Settings, "Graphic Settings...", "Graphic Settings\nAdjust map graphics", ":images/OSGeo/map-settings.png" },

    { eAction_Full_Screen, "Full Screen", "Full Screen\nToggle full screen mode.\nUse the <span style=\" font-weight:600;\">F11</span> key to switch modes.", ":images/OSGeo/fullscreen.png", "", "F11" },

    { eAction_Re_center, "Re-center", "Re-center\nBring map to original position and size", ":/images/OSGeo/zoom-extent.png", "", "Home" },

    { eAction_Zoom_In, "Zoom In", "Zoom In\nZoom map in", ":/images/OSGeo/zoom-in.png", "", "Ctrl++" },

    { eAction_Zoom_Out, "Zoom Out", "Zoom Out\nZoom map out", ":/images/OSGeo/zoom-out.png", "", "Ctrl+-" },			//last designer icon, so far.  TODO: delete this comment

	{ eAction_Operations, "&Operations", "Operations", "" },

	{ eAction_Dataset, "&Dataset", "Dataset", "" },

	{ eAction_Filter, "&Filter", "Filter", "" },	


	// End of Designer actions list


    { eAction_DisplayEditor2D, "2D", "2D\nDisplay 2D view", "://images/alpha-numeric/__a.png", "://images/alpha-numeric/__a.png" },

    { eAction_DisplayEditor3D, "3D", "3D\nDisplay 3D view", "://images/alpha-numeric/__b.png", "://images/alpha-numeric/__b.png" },

	{ eAction_DisplayEditorLog, "Text", "Display text", "://images/alpha-numeric/__c.png", "://images/alpha-numeric/__c.png" },

	{ eAction_DisplayEditorDock, "", "Show/Hide the view working panel", "://images/alpha-numeric/__d.png" },

	{ eAction_MeasureLine, "Distance", "Distance\nMeasure distance (m) between two points", "://images/OSGeo/measure-length.png", "://images/OSGeo/measure-length.png" },

	{ eAction_MeasureArea, "Area", "Area\nMeasure area (m<sup>2</sup>) of selected region", "://images/OSGeo/measure-area.png", "://images/OSGeo/measure-area.png" },

    { eAction_MapEditorMean, "Mean", "Mean\nCompute mean of plotted variable in selected region", "://images/alpha-numeric/__g.png" },

    { eAction_MapEditorStDev, "StDev", "Standard Deviation\nCompute standard deviation of plotted variable in selected region", "://images/alpha-numeric/__h.png" },

    { eAction_MapEditorLinearRegression, "LG", "Linear Regression\nCompute linear regression of plotted variable in selected area", "://images/alpha-numeric/__i.png" },


    { eActionGroup_Projections, "Projections", "Projections menu", "://images/OSGeo/crs-change.png" },

    { eAction_Projection1, "Projection 0", "Projection 0 tool-tip", "://images/OSGeo/projection.png", "://images/OSGeo/projection.png" },

    { eAction_Projection2, "Projection 1", "Projection 1 tool-tip", "://images/OSGeo/projection.png", "://images/OSGeo/projection.png" },

    { eAction_Projection3, "Projection 2", "Projection 2 tool-tip", "://images/OSGeo/projection.png", "://images/OSGeo/projection.png" },


	{ eAction_ApplicationPaths_page, "Paths", "Default application paths selection", "://images/alpha-numeric/__n.png", "://images/alpha-numeric/__n.png" },

	{ eAction_StartupOptions_page, "Startup", "Application start-up behavior", "://images/alpha-numeric/__o.png", "://images/alpha-numeric/__o.png" },

	{ eAction_ApplicationStyles_page, "Styles", "Application visual options", "://images/alpha-numeric/__p.png", "://images/alpha-numeric/__p.png" },


	{ eAction_SelectFeatures, "Rectangular Selection", "Select Features by area or single click", ":/images/themes/default/mActionSelectRectangle.svg", ":/images/themes/default/mActionSelectRectangle.svg" },

	{ eAction_SelectPolygon, "Polygon Selection", "Select Features by Polygon", ":/images/themes/default/mActionSelectPolygon.svg", ":/images/themes/default/mActionSelectPolygon.svg" },

	{ eAction_DeselectAll, "Deselect All ", "Deselect Features from All Layers", ":/images/themes/default/mActionDeselectAll.svg" },

	{ eAction_DecorationGrid, "Grid", "Creates a scale bar that is displayed on the map canvas", ":/images/themes/default/transformed.png", ":/images/themes/default/transformed.png" },


    // TODO images __n, __o and __p are used by settings dialog
};




//////////////////////////////////////////////////////////////////////////////////
//						CActionInfo Implementation
//						(Actions Table Management)
//////////////////////////////////////////////////////////////////////////////////


//static 
bool CActionInfo::CheckActionsTableIntegrity()
{
	// nested lambdas section

	auto has_equal_entry = []( const CActionInfo &ai ) -> bool
	{
		for ( int i = ai.mTag + 1; i < EActionTags_size; ++i )
		{
			EActionTag tag_i = static_cast< EActionTag >( i );
			const CActionInfo& ai_i = ActionInfo( tag_i );
			if ( ai_i == ai )
				return true;
		}
		return false;
	};


	// function body

#if defined (DEBUG) || defined (_DEBUG)

	for ( int i = eLastRecentFileAction + 1; i < EActionTags_size; ++i )
	{
		EActionTag tag_i = static_cast<EActionTag >( i );
		const CActionInfo& ai = ActionInfo( tag_i );
		if ( has_equal_entry( ai ) )
			return false;
	}

#endif

	return true;
}

//static 
bool CActionInfo::ActionsTableIntegrityChecked = CheckActionsTableIntegrity();



//static 
const CActionInfo& CActionInfo::ActionInfo( EActionTag tag )
{
	assert__( tag < EActionTags_size );

	return ActionsTable[ tag ];
}



// These are the only properties that allow us to
//	identify an action, so actions must be unique
//	with respect to them (mName, etc).
//
bool CActionInfo::operator == ( const CActionInfo &o ) const
{
    return
        mName == o.mName &&                 //mTip == o.mTip &&
        //QKeySequence(t2q(mWShortcut)).toString() == QKeySequence(t2q(o.mWShortcut)).toString();
        QKeySequence(mQShortcut).toString() == QKeySequence(o.mQShortcut).toString();
}



//static 
EActionTag CActionInfo::FindEntry( const QAction *a )
{
	if ( a->isSeparator() )
		return eAction_Separator;

	CActionInfo ai( a );
	for ( int i = eLastRecentFileAction + 1; i < EActionTags_size; ++i )
	{
		EActionTag tag_i = static_cast< EActionTag >( i );
		const CActionInfo& ai_i = ActionInfo( tag_i );
		if ( ai_i == ai )
			return tag_i;
	}
	return EActionTags_size;
}


//static 
EActionTag CActionInfo::UpdateActionProperties( QAction *a )
{
	EActionTag tag = FindEntry( a );

	if ( tag != eAction_Separator && tag < EActionTags_size )
		SetActionProperties( a, tag );

	return tag;
}


//static 
QAction* CActionInfo::SetActionProperties( QAction *a, EActionTag tag )
{
	assert__( tag < EActionTags_size );

	CActionInfo  &ai = ActionsTable[ tag ];

	if ( !ai.mTip.empty() )
		a->setToolTip( ai.mTip.c_str() );

	if ( !ai.mIconPath.empty() )
	{
		QIcon icon;
		icon.addFile( QString::fromUtf8( ai.mIconPath.c_str() ), QSize(), QIcon::Normal, QIcon::Off );
		if ( !ai.mOnIconPath.empty() )
		{
			icon.addFile( QString::fromUtf8( ai.mOnIconPath.c_str() ), QSize(), QIcon::Normal, QIcon::On );
			a->setCheckable( true );
		}
		a->setIcon( icon );
	}

	ai.mActions.push_back( a );
	return a;
}


//static 
QAction* CActionInfo::CreateAction( QObject *parent, EActionTag tag )
{
	assert__( tag < EActionTags_size );

	CActionInfo  &ai = ActionsTable[ tag ];

	QAction *a = new QAction( ai.mName.c_str(), parent );
	a->setSeparator( tag == eAction_Separator );
	a->setObjectName( QString::fromUtf8( replace( ai.mName, " ", "_" ).c_str() ) );

	return SetActionProperties( a, tag );
}


//static 
QAction* CActionInfo::SetDockActionProperties( QDockWidget *dock, EActionTag tag )
{
	return SetActionProperties( dock->toggleViewAction(), tag );
}


//static 
QToolButton* CActionInfo::CreateMenuButton( EActionTag button_tag, const QList<QAction*> &actions )
{
	assert__( button_tag < EActionTags_size );

	CActionInfo  &button_ai = ActionsTable[ button_tag ];

	QMenu *menu = new QMenu();
	for ( auto a : actions )
		menu->addAction( a );

	QToolButton *toolButton = new QToolButton(); 	assert__( toolButton );
	toolButton->setMenu( menu );

	if ( !button_ai.mName.empty() )
		toolButton->setText( button_ai.mName.c_str() );
	if ( !button_ai.mTip.empty() )
		toolButton->setToolTip( button_ai.mTip.c_str() );
	if ( !button_ai.mIconPath.empty() )
		toolButton->setIcon( QPixmap( button_ai.mIconPath.c_str() ) );

	toolButton->setPopupMode( QToolButton::InstantPopup );

	return toolButton;
}


//static 
QToolButton* CActionInfo::CreateMenuButton( QObject *parent, EActionTag button_tag, const std::vector<EActionTag> &tags )
{
	QList<QAction*> ActionsList;
	for ( auto tag : tags )
		ActionsList << CreateAction( parent, tag );
	
	return CreateMenuButton( button_tag, ActionsList );
}


//static 
QActionGroup* CActionInfo::CreateActionsGroup( QWidget *group_parent, const std::vector<EActionTag> &tags, bool exclusive )
{
	QActionGroup *group = new QActionGroup( group_parent );
	group->setExclusive( exclusive );

	for ( auto tag : tags )
		CreateAction( group, tag );

	return group;
}



//static 
void CActionInfo::UpdateActionsState( std::initializer_list< EActionTag > tags, bool enable )
{
	for ( auto &tag : tags )
	{
		const CActionInfo& ai = ActionInfo( tag );	assert__( ai.IsAutomatic() && ai.mActions.size() > 0 );
		for ( auto *a : ai.mActions )
			a->setEnabled( enable );
	}
}


//static 
void CActionInfo::TriggerAction( EActionTag tag )
{
	const CActionInfo& ai = ActionInfo( tag );		assert__( ai.IsAutomatic() && ai.mActions.size() > 0 );
	for ( auto *a : ai.mActions )
		a->trigger();
}






//////////////////////////////////////////////////////////////////////////////////
//						Most Recent Files QActions
//////////////////////////////////////////////////////////////////////////////////


//
// - After construction, connect this class's triggered signal to client openRecentfile handler slot
// - Also after construction, and only if some supplementary action is needed at runtime (for instance,
//		menu separator suppression if recent files list became empty), connect this class's 
//		updatedRecentFiles signal to client slot
//
void CRecentFilesProcessor::SetupMenu( QMainWindow *parent, QAction *ActionAfter )
{
	for ( size_t i = 0; i < smMaxRecentFiles; ++i ) 
	{
		mRecentFileActions[i] = new QAction( parent );
		mRecentFileActions[i]->setVisible( false );
		connect( mRecentFileActions[i], SIGNAL( triggered() ), this, SLOT( openRecentFile() ) );
		mRecentFilesMenu->insertAction( ActionAfter, mRecentFileActions[i] );
	}
}


CRecentFilesProcessor::CRecentFilesProcessor( QMainWindow *parent, const std::string MenuName, QMenu *MenuParent, QAction *ActionAfter, const QString &SettingsSectionName ) : 
	QObject( parent ), mSettingsSectionName( SettingsSectionName  )
{
	mRecentFilesMenu = new QMenu( MenuName.c_str(), MenuParent );
	MenuParent->insertMenu( ActionAfter, mRecentFilesMenu );
	SetupMenu( parent, ActionAfter );
}


CRecentFilesProcessor::CRecentFilesProcessor( QMainWindow *parent, QMenu *RecentFiles_menu, QAction *ActionAfter, const QString &SettingsSectionName ) :
	QObject( parent ), mRecentFilesMenu( RecentFiles_menu ), mSettingsSectionName( SettingsSectionName )
{
	SetupMenu( parent, ActionAfter );
}


void CRecentFilesProcessor::UpdateRecentFileActions()
{
	QMutableStringListIterator i( mRecentFiles );
	while ( i.hasNext() ) 
	{
		if ( !QFile::exists( i.next() ) )
			i.remove();
	}

    for ( int j = 0; j < smMaxRecentFiles; ++j ) 
	{
        if ( j < mRecentFiles.count() ) 
		{
			mRecentFiles[ j ] = NormalizedPath( mRecentFiles[ j ] );
			QString text = QObject::tr( "&%1 %2" ).arg( j + 1 ).arg( GetFilenameFromPath( mRecentFiles[ j ] ).c_str() );
			mRecentFileActions[j]->setText( text );
			mRecentFileActions[j]->setData( mRecentFiles[j] );
			mRecentFileActions[j]->setVisible( true );
		}
		else
			mRecentFileActions[j]->setVisible( false );
	}
	//m_RecentFiles_separatorAction->setVisible(!m_recentFiles.isEmpty() );
	mRecentFilesMenu->setEnabled( !mRecentFiles.isEmpty() );

	emit updatedRecentFiles();
}



// PERSISTENCE using Qt settings logic (save and load as QStringList)

void CRecentFilesProcessor::ReadSettings( const QSettings &settings )
{
	mRecentFiles = settings.value( mSettingsSectionName ).toStringList();
	UpdateRecentFileActions();
}

void CRecentFilesProcessor::WriteSettings( QSettings &settings )
{
	settings.setValue( mSettingsSectionName, mRecentFiles );
}


// PERSISTENCE using BRAT v3 logic (save and load iteratively)

template< class CONTAINER >
bool CRecentFilesProcessor::SaveRecentFilesGeneric( const CONTAINER &paths )
{
	// lambdas

	auto brathlFmtEntryRecentWksMacro = []( size_t index ) -> std::string
	{
		return KEY_WKS_RECENT + n2s< std::string >( index );
	};

	// function body

	{
		// We write the values one by one for compatibility with old BRAT, 
		//	where the paths are stored one by one, with numbered keys, and 
		//	not as  list. We could decide to write in a simpler way, as a 
		//	list, but then we would have to maintain 2 reading procedures
		//	it we want to keep the ability to read BRAT v3.
		//
		CAppSection section( GROUP_WKS_RECENT );
		AppSettingsClearGroup( section );
		const size_t size = paths.size();
		for ( size_t i = 0; i < size; ++i )
			AppSettingsWriteValue( section, brathlFmtEntryRecentWksMacro( i ), paths[ (typename CONTAINER::size_type) i ] );
	}

	return AppSettingsStatus() == QSettings::NoError;
}


template< class CONTAINER >
bool CRecentFilesProcessor::LoadRecentFilesGeneric( CONTAINER &paths )
{
	long maxEntries = 0;

	{
		// See note in SaveRecentFilesGeneric
		//
		CAppSection section( GROUP_WKS_RECENT );
		auto const keys = section.Keys();
        maxEntries = keys.size();
		for ( auto const &key : keys )
		{
			auto value = AppSettingsReadValue< typename CONTAINER::value_type >( section, q2t< std::string >( key ) );
			paths.push_back( value );
		};
	}

	assert__( paths.size() == maxEntries );			Q_UNUSED( maxEntries );		//for release builds

	return AppSettingsStatus() == QSettings::NoError;
}


bool CRecentFilesProcessor::ReadSettings()
{
	if ( LoadRecentFilesGeneric( mRecentFiles ) )
		UpdateRecentFileActions();
	else
		return false;

	return true;
}
	
bool CRecentFilesProcessor::WriteSettings()
{
	return SaveRecentFilesGeneric( mRecentFiles );
}




///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_ActionsTable.cpp"
