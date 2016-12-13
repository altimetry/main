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
#include "new-gui/Common/GUI/TextWidget.h"
#include "GUI/ControlPanels/DatasetsRadsBrowserControls.h"

#include "libbrathl/Field.h"



/////////////////////////////////////////////////////////////////////////////////////
//								Dataset Browser
/////////////////////////////////////////////////////////////////////////////////////


//explicit
CRadsBrowserControls::CRadsBrowserControls( CBratApplication &app, CDesktopManagerBase *manager, QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0
    : base_t( app.DataModel(), manager, parent, f )
    , mApp( app )
	, mBratPaths( mModel.BratPaths() )
	, mRadsServiceSettings( app.RadsServiceSettings() )
	, mSharedMemory( RADS_SHARED_MEMORY_KEY.c_str(), this )
{
	if ( !CheckRadsConfigStatus() )
	{
		SimpleErrorBox( "RADS settings will not be available." );
		setEnabled( false );
		return;
	}


    // I. Browse Stack Widget; 2 pages: files and rads

    // - Page Files browser
    //

    // Datasets files buttons group
	mNewDataset = CreateToolButton( "", ":/images/OSGeo/dataset_new.png", "<b>New Dataset</b><br>Insert a new Dataset in current workspace." );
    mDeleteDataset = CreateToolButton( "", ":/images/OSGeo/dataset_delete.png", "<b>Delete...</b><br>Delete selected Dataset." );

    QWidget *buttons_row = CreateButtonRow( false, Qt::Horizontal, { mNewDataset, mDeleteDataset, nullptr } );

	AddTopWidget( buttons_row );

	//datasets tree

    mDatasetTree = new CRadsDatasetsTreeWidget( mRadsServiceSettings.AllAvailableMissions() );
    mDatasetTree->setToolTip("Tree of workspace RADS datasets");

    QGroupBox *page_1 = CreateGroupBox( ELayoutType::Vertical, { mDatasetTree }, "Datasets", nullptr, s, m, m, m, m );


    // II. File Description group
    //
    mFileDesc = new CTextWidget;
    mFileDesc->SetReadOnlyEditor(true);
    mFileDescGroup = CreateGroupBox( ELayoutType::Horizontal, { mFileDesc }, "Files Description", nullptr, s, 4, 4, 4, 4 );


    // III. Variable Description group
    //
    mFieldList = new QListWidget;	   //mFieldList->setMaximumWidth( 120 );
    mFieldList->setSelectionMode(QAbstractItemView::ExtendedSelection); // Multiple items can be selected

    mFieldDesc = new CTextWidget;
    mFieldDesc->SetReadOnlyEditor(true);
	auto *fields_desc_group = CreateGroupBox( ELayoutType::Horizontal, 
	{ 
		CreateSplitter( nullptr, Qt::Horizontal, { mFieldList, mFieldDesc }, false )
    }, "Fields Description", nullptr, s, 4, 4, 4, 4 );


	AddTopSplitter( Qt::Vertical, { page_1, mFileDescGroup, fields_desc_group } );


    // IV. Making connections
    //
    Wire();
}



//virtual 
CRadsBrowserControls::~CRadsBrowserControls()
{
}


bool CRadsBrowserControls::CheckRadsConfigStatus()
{
	std::string rads_error_msg;
	bool result = mRadsServiceSettings.mValidRadsMissions;	//This variable records the status of reading RADS configuration file (not the service settings file)
	if ( !result )
		rads_error_msg = "An error occurred reading missions from rads configuration file.\nPlease check " + mBratPaths.mRadsConfigurationFilePath;
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
				notify( "The RADS service stopped data synchronization with the remote RADS server.\n\
It is recommended to restart BRAT.", false );
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


void CRadsBrowserControls::Wire()
{
	connect( mDatasetTree, SIGNAL( currentIndexChanged( CRadsDatasetsTreeWidgetItem*, int ) ), this, SLOT( HandleCurrentIndexChanged( CRadsDatasetsTreeWidgetItem*,int ) ) );

    connect( mDatasetTree, SIGNAL( itemSelectionChanged() ), this, SLOT( HandleTreeItemChanged() ) );
    connect( mDatasetTree, SIGNAL( itemExpanded(QTreeWidgetItem*) ), this, SLOT( HandleDatasetExpanded() ) );
    connect( mDatasetTree, SIGNAL( itemChanged(QTreeWidgetItem*,int) ), this, SLOT( HandleItemChanged(QTreeWidgetItem*,int) ) );

    connect( mNewDataset, SIGNAL( clicked(bool) ), this, SLOT (HandleNewDataset()) );
    connect( mDeleteDataset, SIGNAL( clicked(bool) ), this, SLOT (HandleDeleteDataset()) );

    connect( mFieldList, SIGNAL( itemSelectionChanged() ), this, SLOT( HandleFieldChanged() ) );

    connect( &mApp, &CBratApplication::RadsNotification, this, &CRadsBrowserControls::HandleRsyncStatusChanged, Qt::QueuedConnection );
}



void CRadsBrowserControls::HandleDatasetExpanded()
{
    mDatasetTree->resizeColumnToContents(0);
}


bool CRadsBrowserControls::RenameDataset( QTreeWidgetItem *dataset_item )
{
	static QRegExp re("[_a-zA-Z0-9]+"); // only alphanumeric letters

										// ToolTip contains the old name of the dataset
	QString old_name( dataset_item->toolTip(0) );
	QString new_name( dataset_item->text(0) );

	if (re.exactMatch( new_name ) ) // Has a Valid Name
	{
		CRadsDataset *current_dataset = mWDataset->GetDataset< CRadsDataset >( old_name.toStdString() );

		if( mWDataset->RenameDataset(current_dataset, new_name.toStdString() ) )
		{
			// Dataset renamed. Update tooltip and notify the change
			dataset_item->setToolTip(0, new_name );					//this triggers another itemChanged
			emit DatasetsChanged( current_dataset );
			return true;
		}
		else // Repeated name
		{
			SimpleWarnBox( QString( "Unable to rename dataset '%1' by '%2'.\nPerhaps dataset already exists.").arg(
				old_name, new_name) );
		}
	}
	else // Invalid name inserted
	{
		SimpleWarnBox( QString( "Unable to rename dataset '%1' by '%2'.\nPlease enter only alphanumeric letters, 'A-Z' or '_a-z' or '0-9'.").arg(
			old_name, new_name) );
	}

	// Setting old name (if new name is repeated or invalid)
	dataset_item->setText( 0, old_name );

	return false;
}



void CRadsBrowserControls::HandleCurrentIndexChanged( CRadsDatasetsTreeWidgetItem *item, int index )			//THIS IS AddFiles
{
    Q_UNUSED( index );

	CWorkspaceOperation *wkso = mModel.Workspace<CWorkspaceOperation>();		assert__( wkso != nullptr );
	const QString dataset_name = item->DatasetName();
	//mDatasetTree->MissionStateChanged( wkso, dataset_name, item );

	assert__( mWDataset && wkso );

	CRadsDataset *current_dataset = mWDataset->GetDataset< CRadsDataset >( q2a( dataset_name ) );				assert__( current_dataset );

	const std::string rads_server_address = ReadRadsServerAddress( mBratPaths.mRadsConfigurationFilePath );
	const std::string local_dir = FormatRadsLocalOutputPath( mBratPaths.UserDataDirectory() );
	const std::string mission_name = q2a( item->CurrentMission() );

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

	std::string errors;
	bool result = current_dataset->SetMission( rads_server_address, local_dir, mission, errors );
	if ( !errors.empty() )
	{

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
			+ "\n\nBe sure field's names used in these operations match the fields of the dataset files"
		);
	}

	// Notify about the change (files added)
	emit DatasetsChanged( current_dataset );
}

void CRadsBrowserControls::HandleItemChanged( QTreeWidgetItem *item, int col )
{
	assert__( col == 0 );		 Q_UNUSED( col );

	assert__( item->parent() == nullptr );

	RenameDataset( item );
}



void CRadsBrowserControls::HandleTreeItemChanged( )
{
    static CRadsDataset *current_dataset = nullptr;

    QString dataset_name;

	CRadsDatasetsTreeWidgetItem *tree_item = dynamic_cast< CRadsDatasetsTreeWidgetItem* >( mDatasetTree->currentItem() );

    // Clear field list and descriptions
    ClearFieldList();

    // No selected Item ------------------------------------------------------------
    if ( tree_item == nullptr )
    {
        // Update buttons status
        mDeleteDataset->setDisabled(true);
        return;
    }

    // Item is a Dataset -----------------------------------------------------------
	assert__( tree_item->parent() == nullptr );

    dataset_name = tree_item->text(0);

    // Update buttons status
    mDeleteDataset->setEnabled(true);

	DatasetChanged( tree_item );

    // notify the world (different dataset)
    CRadsDataset *dataset = mWDataset->GetDataset< CRadsDataset >( dataset_name.toStdString() );
    if( current_dataset != dataset )
    {
        current_dataset = dataset;
        emit CurrentDatasetChanged( current_dataset );
    }
}



void CRadsBrowserControls::HandleWorkspaceChanged( CWorkspaceDataset *wksd )
{
    mWDataset = wksd;

    // Fill DatasetTree with Datasets items
	mDatasetTree->blockSignals( true );
    mDatasetTree->clear();
	mDatasetTree->blockSignals( false );

    HandleTreeItemChanged(); // for updating buttons status
	mDatasetTree->WorkspaceChanged( mWDataset );

    if (wksd)
    {
		auto *datasets = mWDataset->GetDatasets();
        for( auto const it : *datasets )
        {
			if ( dynamic_cast<CRadsDataset*>( it.second ) )
				AddDatasetToTree( t2q( it.first ) );
        }

		//if ( !datasets->empty() )
		//{
		//	QTreeWidgetItemIterator it( mDatasetTree );
		//	if ( *it  )
		//		mDatasetTree->setCurrentItem( *it );
		//}
    }

	LOG_TRACEstd( "RADS Datasets tab finished handling signal to change workspace" );
}



QTreeWidgetItem *CRadsBrowserControls::AddDatasetToTree( const QString &dataset_name )
{
    QIcon dataset_icon = QIcon(":/images/OSGeo/dataset.png");

	QTreeWidgetItem *dataset_item = mDatasetTree->AddDatasetToTree( dataset_name );

    // Fill tree with new dataset products
    //FillFileTree( dataset_item );

    // Returns dataset item (required for automatically select a new dataset)
    // cannot be done here because this method is used in initialization of all datasets.
    return dataset_item;
}



void CRadsBrowserControls::DatasetChanged( QTreeWidgetItem *tree_item )
{
	FileChanged( tree_item );
}



void CRadsBrowserControls::FileChanged( QTreeWidgetItem *tree_item )
{
    WaitCursor wait;

    // Clear field list and descriptions
    ClearFieldList();

    CRadsDatasetsTreeWidgetItem *rads_item = dynamic_cast< CRadsDatasetsTreeWidgetItem* >( tree_item );

	//do not use this before ensuring "there are" datasets
	CRadsDataset *current_dataset  = mWDataset->GetDataset< CRadsDataset >( q2a( rads_item->DatasetName() ) );

	if( current_dataset == nullptr )
		return;

    FillFieldList( current_dataset, q2a( rads_item->CurrentMission() ) );
}



void CRadsBrowserControls::HandleFieldChanged()
{
    WaitCursor wait;

    // Create field description text (for all selected fields)
    QString description;

    foreach(QListWidgetItem *item, mFieldList->selectedItems())
    {
        description += item->data(Qt::AccessibleTextRole).toString() +
                       QString (":\n---------------------------\n") +
                       item->data(Qt::AccessibleDescriptionRole).toString() +
                       QString ("\n\n");
    }

    mFieldDesc->setPlainText( description );
}



void CRadsBrowserControls::HandleNewDataset()
{
	assert__( mWDataset != nullptr );

	auto result = ValidatedInputString( "Dataset Name", mWDataset->GetDatasetNewName( true ), "New Dataset..." );
	if ( !result.first )
		return;

	WaitCursor wait;

    // Get name for the new dataset
    std::string dataset_name = result.second;

    // Insert dataset in workspace and into DatasetTree
    mWDataset->InsertDataset( dataset_name, []( const std::string &name ) { return new CRadsDataset( name ); } );
    QTreeWidgetItem *dataset_item = AddDatasetToTree( dataset_name.c_str() );

    // Selects new dataset
    mDatasetTree->setCurrentItem( dataset_item );

    // Notify about the change (new dataset)
    emit DatasetsChanged( mWDataset->GetDataset( dataset_name ) );

    // -TODO(Delete)-- Old Brat code - mWDataset->GetDatasetNewName() ensures that new dataset does not exists ------
    //else
    //{
    //    SimpleWarnBox( QString("Dataset '%s' already exists.").sprintf( dataset_name.toStdString().c_str() ));
    //}
    // ----------------------------------------------------------------------------------------------------------

    // TODO(delete) - It is already done by HandleTreeItemChanged >> DatasetChanged()
    //ClearFieldList();

    // -TODO -- Old Brat code ----------------------
    //EnableCtrl();
    //
    //CNewDatasetEvent ev(GetId(), dsName);
    //wxPostEvent(GetParent(), ev);
    // ---------------------------------------------

    // FEMM (TBC) - Signal emission is not required. It creates only a new dataset, the dataset selection is catched by HandleTreeItemChanged().
    //emit	( mWDataset->GetDataset( q2a( dataset_name ) ) );
}



void CRadsBrowserControls::HandleDeleteDataset()
{
	// Get selected dataset item
	QTreeWidgetItem *current_dataset_item = mDatasetTree->currentItem();

	if ( mWDataset == nullptr || current_dataset_item == nullptr )
	{
		return;
	}

	if ( current_dataset_item->parent() ) //check if selected item has parent (is a file)
		current_dataset_item = current_dataset_item->parent();

	// Get current dataset
	CRadsDataset *current_dataset = mWDataset->GetDataset< CRadsDataset >( current_dataset_item->text( 0 ).toStdString() );

	CWorkspaceOperation *wks = mModel.Workspace<CWorkspaceOperation>();			assert__( wks != nullptr );
	CStringArray operation_names;
	if ( wks->UseDataset( current_dataset->GetName(), &operation_names ) )
	{
		std::string str = operation_names.ToString( "\n", false );
		SimpleErrorBox(
			"Unable to delete dataset '"
			+ current_dataset->GetName()
			+ "'.\nIt is used by the operations below:\n"
			+ str
			);

		return;
	}

	if ( !SimpleQuestion( QString( "Are you sure to delete dataset '%1'?" ).arg( current_dataset->GetName().c_str() ) ) )
	{
		return;
	}

	if ( !mWDataset->DeleteDataset( current_dataset ) )
	{
		SimpleWarnBox( QString( "Unable to delete dataset '%1'" ).arg(
			current_dataset->GetName().c_str() ) );
		return;
	}

	// Select dataset, this ensures automatic selection of other dataset after deletion
	mDatasetTree->setCurrentItem( current_dataset_item );

	// Delete dataset item (including children => file items)
	delete current_dataset_item;

	// Notify about the change (dataset deleted)
	emit DatasetsChanged( nullptr );
	emit CurrentDatasetChanged( nullptr );

	// -TODO -- Old Brat code ----------------------
	//EnableCtrl();
	//
	//CNewDatasetEvent ev(GetId(), dsName);
	//wxPostEvent(GetParent(), ev);
	// ---------------------------------------------
}



void CRadsBrowserControls::ClearFieldList()  // the old method was: ClearDict();
{   
    mFileDescGroup->setDisabled(true);
    mFileDesc->setDisabled(true);
    mFileDesc->clear();

    mFieldList->clear();
    mFieldDesc->clear();
}



void CRadsBrowserControls::FillFieldList( CDataset *current_dataset, const std::string &current_file_or_mission )
{
	assert__( current_dataset );
    CRadsDataset *current_rads_dataset = dynamic_cast<CRadsDataset*>( current_dataset );    assert__( current_rads_dataset );
    
    if ( !current_rads_dataset->HasMission( current_file_or_mission ) )
        return;
    
    if ( current_rads_dataset->GetProductList()->empty() )
    {
        LOG_WARN( "No files were found for mission " + current_file_or_mission );
        return;
    }
	else
	{
		LOG_WARN( n2s<std::string >(  current_rads_dataset->GetProductList()->size() ) + " files found for mission " + current_file_or_mission );
		return;
	}
	CProductInfo product( current_dataset, *current_rads_dataset->GetProductList()->begin() );
    if ( product.IsValid() )
    {
        // GetDictlist()->InsertProduct(m_product); //////////////////////////////////////////////

		const std::vector< const CField* > &fields = product.Fields();

        for ( auto const *field : fields )
        {
            // InsertField(field); ===========================================
            auto const &type_id = typeid( *field );

            if ( type_id == typeid( CFieldIndex )
                ||  type_id == typeid( CFieldRecord )
                || ( ( type_id == typeid( CFieldArray ) ) && !field->IsFixedSize() )
                || field->IsVirtual() )
            {
                continue;
            }

            QListWidgetItem *item = new QListWidgetItem;

            item->setData(Qt::AccessibleTextRole,        field->GetName().c_str());
            item->setData(Qt::AccessibleDescriptionRole, field->GetDescription().c_str());
            item->setText( field->GetFullName().c_str() );

            mFieldList->addItem( item );
            // =================================================================

			//if ( startsWith( const_cast<CField*>( field )->GetFullName(), std::string( "lat" ) ) )
			//{
			//	CFieldNetCdf *f = dynamic_cast<CFieldNetCdf*>( const_cast<CField*>( field ) );
			//	if ( f && f->IsFixedSize() )
			//		if ( f->IsFixedSize() )
			//		{
			//			double *data = f->GetValuesAsArray();
			//		}
			//}
        }
        // ////////////////////////////////////////////////////////////////////////////////////

        mFileDescGroup->setEnabled( product.IsNetCdf() );

        mFileDesc->setEnabled( product.IsNetCdf() );
        mFileDesc->setText( QString("Product : ") +
                            product.Class().c_str() +
                            QString(" / ") +
                            product.Type().c_str() +
                            QString("\n------------------------------------------------------\n") +
                            product.Description().c_str() );
    }
    else
    {
        SimpleErrorBox( product.ErrorMessages() );
    }

    // Sorts field list
    mFieldList->sortItems();
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_DatasetsRadsBrowserControls.cpp"
