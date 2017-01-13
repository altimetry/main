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

#include "libbrathl/Field.h"



/////////////////////////////////////////////////////////////////////////////////////
//								Dataset Browser
/////////////////////////////////////////////////////////////////////////////////////


void CRadsBrowserControls::Wire()
{
	base_t::Wire();
	
	connect( RadsDatasetTree(), SIGNAL( currentIndexChanged( CRadsDatasetsTreeWidgetItem*, int ) ), this, SLOT( HandleCurrentIndexChanged( CRadsDatasetsTreeWidgetItem*,int ) ) );
	connect( &mApp, &CBratApplication::RadsNotification, this, &CRadsBrowserControls::HandleRsyncStatusChanged, Qt::QueuedConnection );
}



//explicit
CRadsBrowserControls::CRadsBrowserControls( CBratApplication &app, CDesktopManagerBase *manager, QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0
    : base_t( app, manager, parent, f )
	, mRadsServiceSettings( app.RadsServiceSettings() )
	, mSharedMemory( RADS_SHARED_MEMORY_KEY.c_str(), this )
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

    mDatasetTree = new CRadsDatasetsTreeWidget( mRadsServiceSettings.AllAvailableMissions() );
	//mDatasetTree->setToolTip("Tree of workspace RADS datasets");

	AddSpecializedWgdets( buttons_row, "Datasets", "Files Description" );


    //connections
    //
    Wire();
}



//virtual 
CRadsBrowserControls::~CRadsBrowserControls()
{
}



CRadsDatasetsTreeWidget* CRadsBrowserControls::RadsDatasetTree()
{ 
	return dynamic_cast< CRadsDatasetsTreeWidget* >( mDatasetTree ); 
}


QString CRadsBrowserControls::TreeItemSelectionChanged( QTreeWidgetItem *tree_item ) 
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
void CRadsBrowserControls::PrepareWorkspaceChange()
{
	RadsDatasetTree()->WorkspaceChanged( mWDataset );
}


//virtual 
QTreeWidgetItem *CRadsBrowserControls::AddDatasetToTree( const QString &dataset_name )
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
void CRadsBrowserControls::HandleCurrentIndexChanged( CRadsDatasetsTreeWidgetItem *item, int index )
{
	Q_UNUSED( index );

	CWorkspaceOperation *wkso = mModel.Workspace<CWorkspaceOperation>();		assert__( wkso != nullptr );
	const QString dataset_name = item->DatasetName();

	assert__( mWDataset && wkso );

	CRadsDataset *current_dataset = mWDataset->GetDataset< CRadsDataset >( q2a( dataset_name ) );				assert__( current_dataset );

	const std::string rads_server_address = ReadRadsServerAddress( mBratPaths.mRadsConfigurationFilePath );
	const std::string local_dir = CRadsSettings::FormatRadsLocalOutputFolder( mBratPaths.UserDataDirectory() );
	const QString qmission_name = item->CurrentMission();
	const std::string mission_name = q2a( qmission_name );

	auto const &missions = mRadsServiceSettings.AllAvailableMissions();
	CRadsMission mission = { mission_name, FindRadsMissionAbbr( mission_name, missions ) };
	if ( mission.mAbbr.empty() )
	{
		SimpleErrorBox( "Invalid RADS mission name." );
		return;
	}

	// Get current files class and type
	std::string old_product_class = current_dataset->ProductClass();
	std::string old_product_type = current_dataset->ProductType();

	{
		WaitCursor wait;

		std::string errors;
		if ( !current_dataset->SetMission( rads_server_address, local_dir, mission, errors ) )
		{
			LOG_WARN( errors );
		}
		else
		{
			item->SetMissionToolTip();
		}
	}

	// Check new files class and type
	const bool is_same_product_class_and_type = 
		str_icmp( old_product_class, current_dataset->ProductClass() ) &&
		str_icmp( old_product_type, current_dataset->ProductType() );

	CStringArray operation_names;
	bool used_by_operations = wkso->UseDataset( current_dataset->GetName(), &operation_names );
	if ( !is_same_product_class_and_type && used_by_operations )
	{
		std::string str = operation_names.ToString( "\n", false );
		SimpleWarnBox( 
			"Warning: Files contained in the dataset '"
			+ current_dataset->GetName()
			+ "' have been changed from '"
			+ old_product_class
			+ "/"
			+ old_product_type
			+ "' to '"
			+ current_dataset->ProductClass()
			+ "/"
			+ current_dataset->ProductType()
			+ "' product class/type.\n\nThis dataset is used by the operations below:\n"
			+ str
			+ "\n\nPlease review the fields used in these operations."
		);
	}

	HandleSelectionChanged();	//force files description update

	// Notify about the change (files added)
	emit DatasetsChanged( current_dataset );
}



//////////////////////////////////////////////////////////////////////////////////////////////////////
//						Functions not in common with Standard Datasets tab
//////////////////////////////////////////////////////////////////////////////////////////////////////


bool CRadsBrowserControls::CheckRadsConfigStatus()
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


void CRadsBrowserControls::HandleRsyncStatusChanged( CBratApplication::ERadsNotification notification )
{
	static const std::string common_msg = "Any activity involving RADS Datasets can break the application stability or cause silent computation errors.\n\
It is recommended not to use BRAT until synchronization is finished.";

	// The service always sends a 1st notification when connected by a client, to inform rsync state.
	// After that, the application only emits a signal when rsync state changes: those changes, not
	//	the 1st notification, should be announced to the user.
	//
	static bool first_rsync_notification = true;

	//nested lambdas

	auto notify = []( const std::string &msg, bool warn )
	{
		//log before displaying dialogs: another notification can come 
		//and the user closes the last dialog first, altering the log order
		//
		if ( warn )
		{
			LOG_WARN( msg );
			AutoCloseWarnBox( msg, 10 );
		}
		else
		{
			LOG_INFO( msg );
			AutoCloseMsgBox( msg, 10 );
		}
	};

	//function body

	switch ( notification )
	{
		case CBratApplication::eNotificationRsyncRunnig:
		{
			if ( first_rsync_notification )
				first_rsync_notification = false;
			else
				notify( "The RADS service is synchronizing data with the remote RADS server.\n" + common_msg, true );
		}
		break;

		case CBratApplication::eNotificationRsyncStopped:
		{
			if ( first_rsync_notification )
				first_rsync_notification = false;
			else
			{
				const std::string recommendation = ( mWDataset && mWDataset->HasRadsDataset() ) ? "\nIn a workspace with RADS datasets, it is recommended to restart BRAT." : "";
				notify( "The RADS service stopped data synchronization with the remote RADS server." + recommendation, false );
			}
		}
		break;

		case CBratApplication::eNotificationUnknown:
		{
			if ( first_rsync_notification )
				first_rsync_notification = false;
			else
				notify( "Cannot detect if the RADS service is synchronizing data with the remote RADS server.\n" + common_msg, true );
		}
		break;

		case CBratApplication::eNotificationConfigSaved:
		{
			//do nothing besides validation check; so far, only AllAvailableMissions is used from mRadsServiceSettings,
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
			assert__( false );
	}
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_DatasetsRadsBrowserControls.cpp"
