#include "new-gui/brat/stdafx.h"

#include <QAction>

#include "new-gui/Common/ConfigurationKeywords.h"
#include "new-gui/Common/QtUtilsIO.h"

#include "new-gui/brat/ApplicationSettings.h"

#include "ui_BratMainWindow.h"
#include "ActionsTable.h"




//////////////////////////////////////////////////////////////////////////////////
//					Application Global QActions
//////////////////////////////////////////////////////////////////////////////////



CActionInfo ActionsTable[ EActionTags_size ] =
{
	{ eAction_Separator, "", "", "", "", "" },

	{ eAction_RecentFile1, "", "", "", "", "" }, { eAction_RecentFile2, "", "", "", "", "" }, 
	{ eAction_RecentFile3, "", "", "", "", "" }, { eAction_RecentFile4, "", "", "", "", "" }, 
	{ eAction_RecentFile5, "", "", "", "", "" }, { eAction_RecentFile6, "", "", "", "", "" },
	{ eAction_RecentFile7, "", "", "", "", "" }, { eAction_RecentFile8, "", "", "", "", "" }, 
	{ eAction_RecentFile9, "", "", "", "", "" }, { eAction_RecentFile10, "", "", "", "", "" },

	{ eAction_Exit, "&Exit", "Exit brat", ":/images/alpha-numeric/0.png" },

	{ eAction_Open, "&Open...", "Open existing workspace", ":/images/alpha-numeric/1.png", "", "Ctrl+O" },

	{ eAction_New, "&New...", "Create a new workspace", ":/images/alpha-numeric/2.png", "", "Ctrl+N" },

	{ eAction_Save, "&Save","Save the workspace to disk", ":/images/alpha-numeric/3.png", "", "Ctrl+S" },

	{ eAction_Save_As, "Save &As...", "", "" },

	{ eAction_Import_Workspace, "&Import...", "Import existing workspace objects", "" }, 

	{ eAction_Rename_Workspace, "&Rename...", "Rename the loaded workspace\n""The workspace directory will not change", "" }, 

	{ eAction_Delete_Workspace, "&Delete...", "Delete the loaded workspace", "" }, 

	{ eAction_About, "&About...", "", ":/images/BratIcon.png" },

	{ eAction_Close, "&Close", "Exit brat", ":/images/alpha-numeric/4.png" },

	{ eAction_Close_All, "Close &All", "", "" },

	{ eAction_Tile, "&Tile", "", "" },

	{ eAction_Cascade, "Casca&de", "", "" },

	{ eAction_Cut, "Cut", "", "" },

	{ eAction_Copy, "&Copy", "", "" },

	{ eAction_Paste, "&Paste", "", "" },

	{ eAction_Next, "&Next", "", "" },

	{ eAction_Previous, "&Previous", "", "" },

	{ eAction_Options, "&Options...", "Edit application settings", ":/images/alpha-numeric/6.png" },		//icon 5.png is below TODO: delete this comment

	{ eAction_Undo, "&Undo", "", "", "", "Ctrl+Z" },

	{ eAction_Redo, "&Redo", "", "", "", "Ctrl+Y" },

	{ eAction_Delete, "&Delete", "", "" },

	{ eAction_Select_All, "Select &All", "", "" },

	{ eAction_Test, "Test...", "", ":/images/test.png" },

	{ eAction_One_Click, "One-&Click...", "Group actions as a single operation under a single name", ":/images/alpha-numeric/7.png" },

	{ eAction_Launch_Scheduler, "Launch Scheduler", "Launch Scheduler application", ":/images/alpha-numeric/8.png" },

	{ eAction_Workspace_Tree, "Workspace Tree...", "See the objects tree of the loaded workspace", ":/images/alpha-numeric/9.png" },

	{ eAction_Save_Map_Image, "Save Map Image...", "Save map to file", ":/images/alpha-numeric/a.png" },

	{ eAction_Refresh_Map, "Refresh", "Refresh main map", ":/images/alpha-numeric/b.png" },

	{ eAction_Graphic_Settings, "Graphic Settings...", "Adjust graphic settings", ":/images/alpha-numeric/c.png" },

	{ eAction_Full_Screen, "Full Screen", "<html><head/><body><p>Toggle full screen mode.</p><p>Use the <span style=\" font-weight:600;\">F11</span> key to switch modes.</p></body></html>", ":/images/alpha-numeric/d.png", "", "F11" },

	{ eAction_Re_center, "Re-center", "Bring map to original position and size", ":/images/alpha-numeric/e.png", "", "Home" },

	{ eAction_Zoom_In, "Zoom In", "Zoom map in", ":/images/alpha-numeric/5.png", "", "Ctrl++" },

	{ eAction_Zom_Out, "Zom Out", "Zom map out", ":/images/alpha-numeric/f.png", "", "Ctrl+-" },			//last designer icon, so far.  TODO: delete this comment

	{ eAction_Operations, "&Operations", "", "" },

	{ eAction_Dataset, "&Dataset", "", "" },

	{ eAction_Filter, "&Filter", "", "" },	


	// End of Designer actions list


	{ eAction_DisplayEditor2D, "2D", "Display 2D view", "://images/alpha-numeric/__a.png", "://images/alpha-numeric/__a.png" },	

	{ eAction_DisplayEditor3D, "3D", "Display 3D view", "://images/alpha-numeric/__b.png", "://images/alpha-numeric/__b.png" },	

	{ eAction_DisplayEditorLog, "Text", "Display text", "://images/alpha-numeric/__c.png", "://images/alpha-numeric/__c.png" },

	{ eAction_DisplayEditorDock, "", "Show/Hide the view working panel", "://images/alpha-numeric/__d.png" },

	{ eAction_MapEditorDistance, "Distance", "Measure distance between two points", "://images/alpha-numeric/__e.png" },

	{ eAction_MapEditorArea, "Area", "Measure m2 of selected area", "://images/alpha-numeric/__f.png" },

	{ eAction_MapEditorMean, "Mean", "Compute mean of plotted variable in selected area", "://images/alpha-numeric/__g.png" },

	{ eAction_MapEditorStDev, "StDev", "Compute standard deviation of plotted variable in selected area", "://images/alpha-numeric/__h.png" },

	{ eAction_MapEditorLinearRegression, "LG", "Compute linear regression of plotted variable in selected area", "://images/alpha-numeric/__i.png" },


	{ eActionGroup_Projections, "Projections", "Projections menu", "://images/alpha-numeric/__j.png" },

	{ eAction_Projection1, "Projection 0", "Projection 0 tool-tip", "://images/alpha-numeric/__k.png", "://images/alpha-numeric/__k.png" },

	{ eAction_Projection2, "Projection 1", "Projection 1 tool-tip", "://images/alpha-numeric/__l.png", "://images/alpha-numeric/__l.png" },

	{ eAction_Projection3, "Projection 2", "Projection 2 tool-tip", "://images/alpha-numeric/__m.png", "://images/alpha-numeric/__m.png" },


	{ eAction_ApplicationPaths_page, "Paths", "Default application paths selection", "://images/alpha-numeric/__n.png", "://images/alpha-numeric/__n.png" },

	{ eAction_StartupOptions_page, "Startup", "Application start-up behavior", "://images/alpha-numeric/__o.png", "://images/alpha-numeric/__o.png" },

	{ eAction_ApplicationStyles_page, "Styles", "Application visual options", "://images/alpha-numeric/__p.png", "://images/alpha-numeric/__p.png" },

	// TODO images __n, __o and __p are used bu settings dialog
};



const CActionInfo& CActionInfo::ActionInfo( EActionTag tag )
{
	assert__( tag < EActionTags_size );

	return ActionsTable[ tag ];
}


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


QAction* CActionInfo::CreateAction( QObject *parent, EActionTag tag )
{
	assert__( tag < EActionTags_size );

	CActionInfo  &ai = ActionsTable[ tag ];

	QAction *a = new QAction( ai.mName.c_str(), parent );
	a->setSeparator( tag == eAction_Separator );
	a->setObjectName( QString::fromUtf8( replace( ai.mName, " ", "_" ).c_str() ) );

	return SetActionProperties( a, tag );
}


QAction* CActionInfo::SetDockActionProperties( QDockWidget *dock, EActionTag tag )
{
	return SetActionProperties( dock->toggleViewAction(), tag );
}


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


QToolButton* CActionInfo::CreateMenuButton( QObject *parent, EActionTag button_tag, const std::vector<EActionTag> &tags )
{
	QList<QAction*> ActionsList;
	for ( auto tag : tags )
		ActionsList << CreateAction( parent, tag );
	
	return CreateMenuButton( button_tag, ActionsList );
}


QActionGroup* CActionInfo::CreateActionsGroup( QWidget *group_parent, const std::vector<EActionTag> &tags, bool exclusive )
{
	QActionGroup *group = new QActionGroup( group_parent );
	group->setExclusive( exclusive );

	QList<QAction*> ActionsList;
	for ( auto tag : tags )
		ActionsList << CreateAction( group, tag );

	return group;
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
