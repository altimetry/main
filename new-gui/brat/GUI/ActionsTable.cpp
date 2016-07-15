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
#include "new-gui/brat/stdafx.h"

#include <QAction>

#include "new-gui/Common/ConfigurationKeywords.h"
#include "new-gui/Common/QtUtils.h"

#include "new-gui/Common/ApplicationSettings.h"

#include "DataModels/PlotData/MapProjection.h"

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

    { eAction_Exit, "&Exit", CActionInfo::FormatTip("Exit brat"), ":/images/OSGeo/exit.png" },

    { eAction_Open, "&Open...", CActionInfo::FormatTip("Open\nOpen existing workspace"), ":/images/OSGeo/open.png", "", "Ctrl+O" },

    { eAction_New, "&New...", CActionInfo::FormatTip("New\nCreate a new workspace"), ":/images/OSGeo/new.png", "", "Ctrl+N" },

    { eAction_CloseWorkspace, "&Close", CActionInfo::FormatTip("Close\nClose and unload the workspace"), ":/images/OSGeo/quit.png" },

    { eAction_Save, "&Save", CActionInfo::FormatTip("Save\nSave the workspace to disk"), ":/images/OSGeo/save.png", "", "Ctrl+S" },

    { eAction_Import_Workspace, "&Import...", CActionInfo::FormatTip("Import\nImport existing workspace objects"), ":/images/OSGeo/import.png" },

    { eAction_Rename_Workspace, "&Rename...", CActionInfo::FormatTip("Rename\nRename the loaded workspace\n""The workspace directory will not change"), "" },

    { eAction_Delete_Workspace, "&Delete...", CActionInfo::FormatTip("Delete\nDelete the loaded workspace"), ":/images/OSGeo/workspace-delete.png" },

    { eAction_About, "&About...", CActionInfo::FormatTip("About"), ":/images/BratIcon.png" },

    { eAction_Views_List, "&List...", CActionInfo::FormatTip("List\nList all open windows and select or close views"), ":/images/themes/default/propertyicons/diagram.png" },

    { eAction_Close_All, "Close &All", CActionInfo::FormatTip("Close All"), "" },

    { eAction_Open_View, "&Open...", CActionInfo::FormatTip("Open view\nSelect a view from the list of all workspace views"), ":/images/OSGeo/open.png" },

    { eAction_Options, "&Options...", CActionInfo::FormatTip("Options\nEdit application settings"), ":/images/OSGeo/settings.png" },		//icon 5.png is below TODO: delete this comment

	{ eAction_Test, "Test...", "Test", ":/images/test.png" },

    { eAction_One_Click, "One-&Click...", CActionInfo::FormatTip("One-Click\nGroup actions as a single operation under a single name"), ":/images/OSGeo/pointer_oneclick.png" },

    { eAction_Launch_Scheduler, "Launch Scheduler", CActionInfo::FormatTip("Brat Scheduler\nLaunch Scheduler application"), ":/images/brat_scheduler.png" },

    { eAction_Workspace_Tree, "Workspace Tree...", CActionInfo::FormatTip("Workspace Tree\nSee the objects tree of the loaded workspace"), ":/images/OSGeo/page-info.png" },

    { eAction_Operation_Views, "Operation Views...", CActionInfo::FormatTip("List the views of the selected operation"), "" },

    { eAction_Save_Map_Image, "Save Map Image...", CActionInfo::FormatTip("Save Map Image\nSave map to file"), ":/images/OSGeo/map-export.png" },

    { eAction_Refresh_Map, "Refresh", CActionInfo::FormatTip("Refresh\nRefresh main map"), ":/images/OSGeo/zoom-refresh.png" },

    { action_Satellite_Tracks, "Satellite Tracks", CActionInfo::FormatTip("Satellite track\nEnable/Disable automatic satellite track drawing when selected dataset changes"), ":images/OSGeo/satellite_tracks.png", ":images/OSGeo/satellite_tracks.png" },

    { eAction_Full_Screen, "Full Screen", CActionInfo::FormatTip("Full Screen\nToggle full screen mode.\nUse the <span style=\" font-weight:600;\">F11</span> key to switch modes."), ":images/OSGeo/fullscreen.png", "", "F11" },

    { eAction_Re_center, "Re-center", CActionInfo::FormatTip("Zoom Full\nZoom to full extent"), ":/images/OSGeo/zoom-extent.png", "", "Home" },

    { eAction_Zoom_In, "Zoom In", CActionInfo::FormatTip("Zoom In\nZoom map in"), ":/images/OSGeo/zoom-in.png", "", "Ctrl++" },

    { eAction_Zoom_Out, "Zoom Out", CActionInfo::FormatTip("Zoom Out\nZoom map out"), ":/images/OSGeo/zoom-out.png", "", "Ctrl+-" },			//last designer icon, so far.  TODO: delete this comment



	// End of Designer actions list


    { eAction_DisplayEditor2D, "2D", CActionInfo::FormatTip("2D\nDisplay 2D view"), "://images/OSGeo/2d-view.png", "://images/OSGeo/2d-view.png" },

    { eAction_DisplayEditor3D, "3D", CActionInfo::FormatTip("3D\nDisplay 3D view"), "://images/OSGeo/3d-view.png", "://images/OSGeo/3d-view.png" },

    { eAction_DisplayEditorDock, "", CActionInfo::FormatTip("Show/Hide the view working panel"), "://images/alpha-numeric/__d.png" },

    { eAction_MeasureLine, "Distance", CActionInfo::FormatTip("Distance\nMeasure distance (m) between two points"), "://images/OSGeo/measure-length.png", "://images/OSGeo/measure-length.png" },

    { eAction_MeasureArea, "Area", CActionInfo::FormatTip("Area\nMeasure area (m<sup>2</sup>) of selected region"), "://images/OSGeo/measure-area.png", "://images/OSGeo/measure-area.png" },

    { eAction_MapEditorMean, "Mean", CActionInfo::FormatTip("Mean\nCompute mean of plotted variable in selected region"), "://images/OSGeo/stats.png" },

    { eAction_MapEditorStDev, "StDev", CActionInfo::FormatTip("Standard Deviation\nCompute standard deviation of plotted variable in selected region"), "://images/OSGeo/stats_std.png" },

    { eAction_MapEditorLinearRegression, "LG", CActionInfo::FormatTip("Linear Regression\nCompute linear regression of plotted variable in selected area"), "://images/OSGeo/regression.png" },


    { eActionGroup_Projections, "", CActionInfo::FormatTip("Projections menu"), "://images/OSGeo/crs-change.png" },

	{ eAction_Projection_LAMBERT_CYLINDRICAL, CMapProjection::GetInstance()->IdToName( PROJ2D_LAMBERT_CYLINDRICAL ), "Projection Lambert Cylindrical", "://images/OSGeo/projection.png", "://images/OSGeo/projection.png" },

	{ eAction_Projection_PLATE_CAREE, CMapProjection::GetInstance()->IdToName( PROJ2D_PLATE_CAREE ), "Projection Plate Caree", "://images/OSGeo/projection.png", "://images/OSGeo/projection.png" },

	{ eAction_Projection_MOLLWEIDE, CMapProjection::GetInstance()->IdToName( PROJ2D_MOLLWEIDE ), "Projection Mollweide", "://images/OSGeo/projection.png", "://images/OSGeo/projection.png" },

	{ eAction_Projection_ROBINSON, CMapProjection::GetInstance()->IdToName( PROJ2D_ROBINSON ), "Projection Robinson", "://images/OSGeo/projection.png", "://images/OSGeo/projection.png" },

	{ eAction_Projection_LAMBERT_AZIMUTHAL, CMapProjection::GetInstance()->IdToName( PROJ2D_LAMBERT_AZIMUTHAL ), "Projection Lambert Azimuthal", "://images/OSGeo/projection.png", "://images/OSGeo/projection.png" },

	{ eAction_Projection_AZIMUTHAL_EQUIDISTANT, CMapProjection::GetInstance()->IdToName( PROJ2D_AZIMUTHAL_EQUIDISTANT ), "Projection Azimuthal Equidistant", "://images/OSGeo/projection.png", "://images/OSGeo/projection.png" },

	{ eAction_Projection_MERCATOR, CMapProjection::GetInstance()->IdToName( PROJ2D_MERCATOR ), "Projection Mercator", "://images/OSGeo/projection.png", "://images/OSGeo/projection.png" },

	{ eAction_Projection_ORTHO, CMapProjection::GetInstance()->IdToName( PROJ2D_ORTHO ), "Projection Ortho", "://images/OSGeo/projection.png", "://images/OSGeo/projection.png" },

	{ eAction_Projection_NEAR_SIGHTED, CMapProjection::GetInstance()->IdToName( PROJ2D_NEAR_SIGHTED ), "Projection Near Sighted", "://images/OSGeo/projection.png", "://images/OSGeo/projection.png" },

	{ eAction_Projection_STEREOGRAPHIC, CMapProjection::GetInstance()->IdToName( PROJ2D_STEREOGRAPHIC ), "Projection Stereographic", "://images/OSGeo/projection.png", "://images/OSGeo/projection.png" },

	// { eAction_Projection_TMERCATOR, CMapProjection::GetInstance()->IdToName( PROJ2D_LAMBERT_CYLINDRICAL ), "Projection 0 tool-tip", "://images/OSGeo/projection.png", "://images/OSGeo/projection.png" },

	{ eAction_Projection_Default, "Default", "Original projection", "://images/OSGeo/projection.png", "://images/OSGeo/projection.png" },


    { eAction_ApplicationPaths_page, "Paths", CActionInfo::FormatTip("Paths\nDefault application paths selection"), "://images/alpha-numeric/__n.png", "://images/alpha-numeric/__n.png" },

    { eAction_StartupOptions_page, "Startup", CActionInfo::FormatTip("Startup\nApplication start-up behavior"), "://images/alpha-numeric/__o.png", "://images/alpha-numeric/__o.png" },

    { eAction_ApplicationStyles_page, "Styles", CActionInfo::FormatTip("Styles\nApplication visual options"), "://images/alpha-numeric/__p.png", "://images/alpha-numeric/__p.png" },


    { eAction_SelectFeatures, "Rectangular Selection", CActionInfo::FormatTip("Selection\nSelect area"), ":/images/themes/default/mActionSelectRectangle.svg", ":/images/themes/default/mActionSelectRectangle.svg" },

    { eAction_SelectPolygon, "Polygon Selection", CActionInfo::FormatTip("Polygon Selection\nSelect Features by Polygon"), ":/images/themes/default/mActionSelectPolygon.svg", ":/images/themes/default/mActionSelectPolygon.svg" },

    { eAction_DeselectAll, "Deselect All", CActionInfo::FormatTip("Deselect\nRemove features selection"), ":/images/themes/default/mActionDeselectAll.svg" },

    { eAction_DecorationGrid, "Grid", CActionInfo::FormatTip("Grid\nCreates a scale bar that is displayed on the map canvas"), ":/images/themes/default/transformed.png", ":/images/themes/default/transformed.png" },

    { eAction_MapTips, "Map Tips", CActionInfo::FormatTip("Map Tips\nShow layer data values when mouse moves over map canvas"), ":/images/OSGeo/maptips.png", ":/images/OSGeo/maptips.png" },	

    { eAction_MouseTrackingeCoordinatesFormat, "", CActionInfo::FormatTip("Format\nCoordinates format"), ":/images/OSGeo/coordinate_capture.png" },


	{ eActionGroup_Filters_Quick, "", "Quick operation filters", "://images/OSGeo/filter.png", "://images/OSGeo/filter.png" },

	{ eActionGroup_Filters_Advanced, "", "Advanced operation filters", "://images/OSGeo/filter.png", "://images/OSGeo/filter.png" },

	{ eAction_Item_Filters, "", "Click to apply filter", "://images/OSGeo/filter.png" },

	{ eAction_AssignExpression, "", "Assign to the selected tree item", ":/images/themes/default/mIconDataDefineExpressionOn.svg" },

	{ eAction_ExportView, "", "Save view as image file under selected format", "://images/OSGeo/map-export.png" },

    { eAction_DataDisplayProperties, "", CActionInfo::FormatTip("View Properties\nEdit display properties of selected field"), ":images/OSGeo/map-settings.png" },


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

	return ::CreateMenuButton( button_ai.mName, button_ai.mIconPath, button_ai.mTip, actions );
}


//static 
QToolButton* CActionInfo::CreatePopupButton( EActionTag button_tag, const QList<QAction*> &actions, QAction *default_action )		//default_action = nullptr 
{
	assert__( button_tag < EActionTags_size );

	CActionInfo  &button_ai = ActionsTable[ button_tag ];

	return ::CreatePopupButton( button_ai.mName, button_ai.mIconPath, button_ai.mTip, actions, default_action );
}


//static 
QToolButton* CActionInfo::CreateToolButton( EActionTag button_tag, bool auto_raise )	//, bool auto_raise = false 
{
	assert__( button_tag < EActionTags_size );

	CActionInfo  &button_ai = ActionsTable[ button_tag ];

	return ::CreateToolButton( button_ai.mName, button_ai.mIconPath, button_ai.mTip, auto_raise );
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
QActionGroup* CActionInfo::CreateActionGroup( QWidget *group_parent, const std::vector<EActionTag> &tags, bool exclusive )
{
	QActionGroup *group = ::CreateActionGroup( group_parent, exclusive );
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
	mRecentFilesMenu = new CRecentFilesMenu( MenuName.c_str(), MenuParent );
	MenuParent->insertMenu( ActionAfter, mRecentFilesMenu );
	SetupMenu( parent, ActionAfter );
}


CRecentFilesProcessor::CRecentFilesProcessor( QMainWindow *parent, QMenu *RecentFiles_menu, QAction *ActionAfter, const QString &SettingsSectionName ) :
	QObject( parent ), mRecentFilesMenu( RecentFiles_menu ), mSettingsSectionName( SettingsSectionName )
{
	SetupMenu( parent, ActionAfter );
}


void CRecentFilesProcessor::SetEnabled( bool enable )
{
	mRecentFilesMenu->setEnabled( enable );
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
			mRecentFileActions[j]->setToolTip( mRecentFiles[j] );
		}
		else
			mRecentFileActions[j]->setVisible( false );
	}
	//m_RecentFiles_separatorAction->setVisible(!m_recentFiles.isEmpty() );
	mRecentFilesMenu->setEnabled( !mRecentFiles.isEmpty() );

	emit updatedRecentFiles();
}


void CRecentFilesProcessor::DeleteEntry( const QString &cur_file )
{
	mRecentFiles.removeAll( cur_file );
	UpdateRecentFileActions();
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
