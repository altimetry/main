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

#include "DataModels/Model.h"
#include "DataModels/Workspaces/RadsDataset.h"
#include "DataModels/Workspaces/Workspace.h"
#include "GUI/ControlPanels/DatasetsRadsBrowserControls.h"
#include "GUI/ControlPanels/Dialogs/RadsPhasesDialog.h"

#include "libbrathl/Field.h"



/////////////////////////////////////////////////////////////////////////////////////
//								Dataset Browser
/////////////////////////////////////////////////////////////////////////////////////



void CDatasetsRadsBrowserControls::Wire()
{
	base_t::Wire();
	
	connect( RadsDatasetTree(), SIGNAL( currentIndexChanged( CRadsDatasetsTreeWidgetItem*, int ) ), this, SLOT( HandleCurrentIndexChanged( CRadsDatasetsTreeWidgetItem*,int ) ) );
	connect( &mApp, &CBratApplication::RadsNotification, this, &CDatasetsRadsBrowserControls::HandleRsyncStatusChanged, Qt::QueuedConnection );
}



//explicit
CDatasetsRadsBrowserControls::CDatasetsRadsBrowserControls( CBratApplication &app, CDesktopManagerBase *manager, QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0
    : base_t( app, manager, parent, f )
	, mRadsServiceSettings( app.RadsServiceSettings() )
    //, mSharedMemory( RADS_SHARED_MEMORY_KEY.c_str(), this )
{
	if ( !CheckRadsConfigStatus() )
	{
		SimpleErrorBox( "RADS settings will not be available." );
		setEnabled( false );
		return;
	}

    //datasets browser
    //
	mNewDataset = CreateToolButton( "", ":/images/OSGeo/dataset_new.png", "<b>New Dataset</b><br>Insert a new Dataset in current workspace." );
    mDeleteDataset = CreateToolButton( "", ":/images/OSGeo/dataset_delete.png", "<b>Delete...</b><br>Delete selected Dataset." );

    QWidget *buttons_row = CreateButtonRow( false, Qt::Horizontal, { mNewDataset, mDeleteDataset, nullptr } );

	//datasets tree

    mDatasetTree = new CRadsDatasetsTreeWidget( mRadsServiceSettings.AllMissions() );
	//mDatasetTree->setToolTip("Tree of workspace RADS datasets");

	AddSpecializedWgdets( buttons_row, "Datasets", "Files Description" );


    //connections
    //
    Wire();
}



//virtual 
CDatasetsRadsBrowserControls::~CDatasetsRadsBrowserControls()
{
}


//virtual 
void CDatasetsRadsBrowserControls::UpdatePanelSelectionChange() //override
{
	if ( SelectedPanel() )
	{
		DrawDatasetTracks( nullptr, false );	//draw RADS tracks, that is, nothing
	}
}



CRadsMission CDatasetsRadsBrowserControls::RadsMission( const std::string &mission_name, const std::vector< std::string> &phases ) const
{
	auto const &missions = mRadsServiceSettings.AllMissions();
	CRadsMission mission = { mission_name, FindRadsMissionAbbr( mission_name, missions ), phases };
	if ( mission.mAbbr.empty() )
	{
		SimpleErrorBox( "Invalid RADS mission name." );
	}

	return mission;
}


std::string CDatasetsRadsBrowserControls::RadsServerAddress() const
{ 
	return ReadRadsServerAddress( mBratPaths.mRadsConfigurationFilePath ); 
}



CRadsDatasetsTreeWidget* CDatasetsRadsBrowserControls::RadsDatasetTree()
{ 
	return dynamic_cast< CRadsDatasetsTreeWidget* >( mDatasetTree ); 
}


QString CDatasetsRadsBrowserControls::TreeItemSelectionChanged( QTreeWidgetItem *tree_item ) 
{
    QString dataset_name;

    // No selected Item
	//
    if ( tree_item == nullptr )
    {
        // Update buttons status
		//
        mDeleteDataset->setDisabled( true );
    }
	else
	//
	// With RADS, only Datasets in tree, no child files
	//
	{
		assert__( tree_item->parent() == nullptr );

		dataset_name = tree_item->text( 0 );

		// Update buttons status
		//
		mDeleteDataset->setEnabled( true );

		CRadsDatasetsTreeWidgetItem *rads_tree_item = dynamic_cast< CRadsDatasetsTreeWidgetItem*> ( tree_item );	assert__( rads_tree_item );

		CRadsDataset *current_rads_dataset = mWDataset->GetDataset< CRadsDataset >( q2a( rads_tree_item->DatasetName() ) );
		if ( 
			current_rads_dataset &&
			current_rads_dataset->HasMission( q2a( rads_tree_item->CurrentMission() ) ) &&
			!current_rads_dataset->IsEmpty() )
			{
				FillFieldList( current_rads_dataset, current_rads_dataset->GetFirstFile() );
			}
	}

	return dataset_name;
}



// Called
//	- after assigning new workspace 
//	- after clearing tree
//	- before invoking AddDatasetToTree iteratively to refill tree
//
//virtual 
void CDatasetsRadsBrowserControls::PrepareWorkspaceChange()
{
	RadsDatasetTree()->WorkspaceChanged( mWDataset );
}


//virtual 
QTreeWidgetItem *CDatasetsRadsBrowserControls::AddDatasetToTree( const QString &dataset_name )
{
	QTreeWidgetItem *dataset_item = RadsDatasetTree()->AddDatasetToTree( dataset_name );

    // Returns dataset item (required for automatically select a new dataset)
    // cannot be done here because this method is used in initialization of all datasets.
	//
    return dataset_item;
}



// Changed the mission selection in a dataset
// Has the same purpose of AddFiles in sibling tab for standard datasets
//
void CDatasetsRadsBrowserControls::HandleCurrentIndexChanged( CRadsDatasetsTreeWidgetItem *item, int index )
{
	Q_UNUSED( index );

	const QString dataset_name = item->DatasetName();												assert__( mWDataset );

	CRadsDataset *current_dataset = mWDataset->GetDataset< CRadsDataset >( q2a( dataset_name ) );	assert__( current_dataset );

	const QString qmission_name = item->CurrentMission();
	const std::string mission_name = q2a( qmission_name );
	if ( ( mission_name.empty() && !current_dataset->HasMission() )
		||
		current_dataset->HasMission( mission_name ) )
		return;


	assert__( !mission_name.empty() );

	// Ask for phases

	CRadsMission mission = RadsMission( mission_name, std::vector< std::string>() );
	const std::string mission_path = MissionPath( RadsServerAddress(), mRadsServiceSettings.DownloadDirectory(), mission.mAbbr );
	QDir source_dir( t2q( mission_path ) );
	std::vector< std::string > all_phases;
	if ( source_dir.exists() )
	{
		QStringList dirs;
		dirs = source_dir.entryList( QDir::AllDirs | QDir::NoDotAndDotDot );
		for ( auto const &qs : dirs )
			if ( qs.length() == 1 )
				all_phases.push_back( q2a( qs ) );
	}
    if ( all_phases.empty() )
    {
        SimpleWarnBox("No data was found for mission '" + mission.mName + "'." );
    }
    else
    {
        CRadsPhasesDialog dlg( all_phases, mission.mPhases, mission_name, this );
        if ( dlg.exec() != QDialog::Accepted )
        {
            item->UpdateMission();
            return;
        }
    }

	// Get current files class and type, then assign

	std::string old_product_class = current_dataset->ProductClass();
	std::string old_product_type = current_dataset->ProductType();

	{
		WaitCursor wait;

		std::string errors;
		if ( !current_dataset->SetMission( RadsServerAddress(), mRadsServiceSettings.DownloadDirectory(), mission, errors ) )
		{
			LOG_WARN( errors );
		}
        
        item->SetMissionToolTip();
        item->UpdatePhases();
	}

	CheckNewFilesClassAndType( old_product_class, old_product_type, current_dataset );

	HandleSelectionChanged();	//force files description update

	// Notify about the change (files added)
	emit DatasetsChanged( current_dataset );
}



//////////////////////////////////////////////////////////////////////////////////////////////////////
//						Functions not in common with Standard Datasets tab
//////////////////////////////////////////////////////////////////////////////////////////////////////


bool CDatasetsRadsBrowserControls::CheckRadsConfigStatus()
{
	std::string rads_error_msg;
	bool result = mRadsServiceSettings.mValidRadsMissions;	//This variable records the status of reading RADS configuration file (not the service settings file)
	if ( !result )
		rads_error_msg = "An error occurred reading missions from rads configuration file.\nPlease check " + mBratPaths.mRadsConfigurationFilePath.mPath;
	else
	{
		result = mRadsServiceSettings.Status() == QSettings::Status::NoError;
		if ( !result )
			rads_error_msg = "Error reading RADS Service configuration file '" + mRadsServiceSettings.FilePath() + "'";
		else
		{
			//result = mSharedMemory.create( sizeof( CRadsSharedMemory ) );
			//if ( !result )
			//{
			//	rads_error_msg = "Unable to communicate with RADS service.";
			//}
		}
	}

	if ( !result )
		SimpleErrorBox( rads_error_msg );

	return result;
}


void CDatasetsRadsBrowserControls::HandleRsyncStatusChanged( CBratApplication::ERadsNotification notification )
{
	static const std::string synchronizing_msg = "The RADS service is synchronizing data with the remote RADS server.";
	static const std::string stopped_synchronizing_msg = "The RADS service stopped data synchronization with the remote RADS server.";
	static const int auto_close_time = 5;

    // The service always sends a 1st notification when connected by a client, to inform rsync state.
	// After that, the application only emits a signal when rsync state changes: those changes, not
	//	the 1st notification, should be announced to the user.
	//
	static bool first_rsync_notification = true;
    static bool processing = false;


	//nested lambdas

	auto notify = []( const std::string &msg, bool warn )
	{
		if ( warn )
		{
			LOG_WARN( msg );
            AutoCloseWarnBox( msg, auto_close_time );
		}
		else
		{
			LOG_INFO( msg );
            AutoCloseMsgBox( msg, auto_close_time );
        }
	};


	//function body

    // This function can easliy be re-entered. Ensure sequential processing,
    //  furthermore because displayed dialogs have undefined time (<= auto_close_time)
    //  to close but the thread's event loop keeps running.
    //
    if ( processing )
    {
        QTimer::singleShot( 2000, this,
        [this, notification]()
        {
            HandleRsyncStatusChanged( notification );
        }
        );
        return;
    }

    boolean_locker_t l ( processing );

	bool change_enable_state = true;

	switch ( notification )
	{
		case CBratApplication::eNotificationRsyncRunnig:
		{
			if ( first_rsync_notification )
			{
				first_rsync_notification = false;
				LOG_WARN( synchronizing_msg );
			}
			else
				notify( synchronizing_msg, true );
		}
		break;

		case CBratApplication::eNotificationRsyncStopped:
		{
			if ( first_rsync_notification )
				first_rsync_notification = false;
			else
			{
                notify( stopped_synchronizing_msg, true );
			}
		}
		break;

		case CBratApplication::eNotificationUnknown:
		{
			if ( first_rsync_notification )
				first_rsync_notification = false;
			else
                notify( "Cannot detect if the RADS service is synchronizing data with the remote RADS server.", true );
		}
		break;

		case CBratApplication::eNotificationConfigSaved:
		{
			change_enable_state = false;

			//do nothing besides validation check; so far, only AllMissions is used from mRadsServiceSettings,
			//and it is only affected by service configuration changes if a file reading error occurs.

			if ( !CheckRadsConfigStatus() )
			{
				SimpleErrorBox( "RADS settings will not be available." );
				setEnabled( false );
                return;
			}
		}
		break;

		default:
			change_enable_state = false;
			assert__( false );
	}


	if ( change_enable_state )
		setEnabled( notification != CBratApplication::eNotificationRsyncRunnig );
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_DatasetsRadsBrowserControls.cpp"
