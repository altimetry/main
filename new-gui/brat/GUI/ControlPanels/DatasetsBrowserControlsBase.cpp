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
#include "GUI/ControlPanels/DatasetsBrowserControlsBase.h"

#include "libbrathl/Field.h"



/////////////////////////////////////////////////////////////////////////////////////
//								Dataset Browser
/////////////////////////////////////////////////////////////////////////////////////


void CDatasetsBrowserControlsBase::Wire()
{
	connect( mFieldList, SIGNAL( itemSelectionChanged() ), this, SLOT( HandleFieldChanged() ) );

	connect( mNewDataset, SIGNAL( clicked(bool) ), this, SLOT (HandleNewDataset()) );
	connect( mDeleteDataset, SIGNAL( clicked(bool) ), this, SLOT (HandleDeleteDataset()) );

	connect( mDatasetTree, SIGNAL( itemExpanded(QTreeWidgetItem*) ), this, SLOT( HandleDatasetExpanded() ) );
	connect( mDatasetTree, SIGNAL( itemChanged(QTreeWidgetItem*,int) ), this, SLOT( HandleItemChanged(QTreeWidgetItem*,int) ) );	//to rename dataset
	connect( mDatasetTree, SIGNAL( itemSelectionChanged() ), this, SLOT( HandleSelectionChanged() ) );
}



void CDatasetsBrowserControlsBase::AddSpecializedWgdets( QWidget *buttons_row, const QString &datasets_tree_title, const QString &files_description_title )
{
	assert__( mDatasetTree && mNewDataset && mDeleteDataset );

	AddTopWidget( buttons_row );

	//datasets tree
	
	QGroupBox *datasets_tree_group = CreateGroupBox( ELayoutType::Vertical, { mDatasetTree }, datasets_tree_title, nullptr, s, m, m, m, m );


	// II. File Description group
	//
	mFileDesc = new CTextWidget;
	mFileDesc->SetReadOnlyEditor(true);
	mFileDescGroup = CreateGroupBox( ELayoutType::Horizontal, { mFileDesc }, files_description_title, nullptr, s, 4, 4, 4, 4 );


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


	AddTopSplitter( Qt::Vertical, { datasets_tree_group, mFileDescGroup, fields_desc_group } );
}

//explicit
CDatasetsBrowserControlsBase::CDatasetsBrowserControlsBase( CBratApplication &app, CDesktopManagerBase *manager, QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0
    : base_t( app.DataModel(), manager, parent, f )
    , mApp( app )
	, mBratPaths( mModel.BratPaths() )
{
}



//virtual 
CDatasetsBrowserControlsBase::~CDatasetsBrowserControlsBase()
{
}




CDataset* CDatasetsBrowserControlsBase::WorkspaceDataset( const QString &qname )
{
	const std::string name = q2a( qname );

	// All datasets are CDataset; here we only want to return the type that matches 
	//	the respective tab: CRadsDataset or CDataset-not-CRadsDataset
	//
	CDataset *dataset = nullptr;
	CRadsDataset *rads_dataset = mWDataset->GetDataset< CRadsDataset >( name );
	if ( !rads_dataset )
		dataset = mWDataset->GetDataset< CDataset >( name );

	switch ( DatasetType() )
	{
		case eStandard:
			return dataset;
			break;

		case eRADS:
			return rads_dataset;
			break;

		default:
			assert__( false );
			break;
	}

	return nullptr;
}



void CDatasetsBrowserControlsBase::HandleDatasetExpanded()
{
	mDatasetTree->resizeColumnToContents( 0 );
}


void CDatasetsBrowserControlsBase::HandleWorkspaceChanged( CWorkspaceDataset *wksd )
{
	const std::string tab_name = DatasetType() == eStandard ? "Datasets" : "RADS Datasets";

	LOG_TRACEstd( tab_name + " tab started handling signal to change workspace" );

	mWDataset = wksd;

	mDatasetTree->blockSignals( true );
	mDatasetTree->clear();
	mDatasetTree->blockSignals( false );

	HandleSelectionChanged(); // for updating buttons status

	PrepareWorkspaceChange();

	// Fill DatasetTree with Datasets items
	//
	if (wksd)
	{
		auto *datasets = mWDataset->GetDatasets();
		for( auto const &it : *datasets )
		{
			const QString name = t2q( it.first );
			if ( WorkspaceDataset( name ) )
				AddDatasetToTree( name );
		}
	}

	LOG_TRACEstd( tab_name + " tab finished handling signal to change workspace" );
}



//virtual
void CDatasetsBrowserControlsBase::ClearFieldList()  // the old method was: ClearDict();
{   
	mFileDescGroup->setDisabled(true);
	mFileDesc->setDisabled(true);
	mFileDesc->clear();

	mFieldList->clear();
	mFieldDesc->clear();
}


//virtual
void CDatasetsBrowserControlsBase::FillFieldList( CDataset *current_dataset, const std::string &current_file )
{
	assert__( current_dataset );

	WaitCursor wait;

	CProductInfo product( current_dataset, current_file );
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


void CDatasetsBrowserControlsBase::HandleFieldChanged()
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



bool CDatasetsBrowserControlsBase::RenameDataset( QTreeWidgetItem *dataset_item )
{
	static QRegExp re("[_a-zA-Z0-9]+"); // only alphanumeric letters

	// ToolTip contains the old name of the dataset
	//
	QString old_name = dataset_item->toolTip( 0 );
	QString new_name = dataset_item->text( 0 );

	if ( re.exactMatch( new_name ) ) // Has a Valid Name
	{
		CDataset *current_dataset = WorkspaceDataset( old_name );				assert__( current_dataset );
		if( mWDataset->RenameDataset( current_dataset, q2a( new_name ) ) )
		{
			// Dataset renamed. Update tool-tip and notify the change
			//
			dataset_item->setToolTip( 0, new_name );	//this triggers another itemChanged
			emit DatasetsChanged( current_dataset );
			return true;
		}
		else // Repeated name
		{
			SimpleWarnBox( QString( "Unable to rename dataset '%1' with '%2'.\nPerhaps name already exists.").arg( old_name, new_name) );
		}
	}
	else // Invalid name inserted
	{
		SimpleWarnBox( QString( "Unable to rename dataset '%1' with '%2'.\nPlease enter only alphanumeric letters, 'A-Z' or '_a-z' or '0-9'.").arg(
			old_name, new_name) );
	}

	// Setting old name (if new name is repeated or invalid)
	//
	dataset_item->setText( 0, old_name );

	return false;
}



void CDatasetsBrowserControlsBase::HandleItemChanged( QTreeWidgetItem *item, int col )
{
	if ( col != 0 )	//col 1 can triggers this caused by seTooltip
		return;

	assert__( item->parent() == nullptr );

	RenameDataset( item );
}



void CDatasetsBrowserControlsBase::HandleSelectionChanged( )
{
	// Note that this variable stores datasets from both tabs, which is not a problem,
	//	but it is not also the same as if defined in a derived class function.
	//
	static CDataset *current_dataset = nullptr;


	// Clear field list and descriptions
	//
	ClearFieldList();

	QString dataset_name = TreeItemSelectionChanged( mDatasetTree->currentItem() );
	if ( dataset_name.isEmpty() )
		return;

	// notify the world (different dataset)
	//
	CDataset *dataset = WorkspaceDataset( dataset_name );		assert__( dataset );
	if( current_dataset != dataset )
	{
		current_dataset = dataset;
		emit CurrentDatasetChanged( current_dataset );
	}
}



void CDatasetsBrowserControlsBase::HandleNewDataset()
{
	assert__( mWDataset != nullptr );

	const bool is_rads = DatasetType() == eRADS;
	auto result = ValidatedInputString( "Dataset Name", mWDataset->GetDatasetNewName( is_rads ), "New Dataset..." );
	if ( !result.first )
		return;

	WaitCursor wait;

	// Get name for the new dataset
	std::string dataset_name = result.second;

	// Insert dataset in workspace and into DatasetTree
	mWDataset->InsertDataset( dataset_name, [&is_rads]( const std::string &name ) { return is_rads ? new CRadsDataset( name ) : new CDataset( name ); } );
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

	// TODO(delete) - It is already done by HandleSelectionChanged >> DatasetChanged()
	//ClearFieldList();

	// -TODO -- Old Brat code ----------------------
	//EnableCtrl();
	//
	//CNewDatasetEvent ev(GetId(), dsName);
	//wxPostEvent(GetParent(), ev);
	// ---------------------------------------------

	// FEMM (TBC) - Signal emission is not required. It creates only a new dataset, the dataset selection is catched by HandleSelectionChanged().
	//emit	( mWDataset->GetDataset( q2a( dataset_name ) ) );
}



void CDatasetsBrowserControlsBase::HandleDeleteDataset()
{
	// Get selected dataset item
	//
	QTreeWidgetItem *current_dataset_item = mDatasetTree->currentItem();

	if ( mWDataset == nullptr || current_dataset_item == nullptr )
	{
		return;
	}

	if ( current_dataset_item->parent() ) //check if selected item has parent (is a file)
		current_dataset_item = current_dataset_item->parent();

	// Get current dataset
	//
	CDataset *current_dataset = WorkspaceDataset( current_dataset_item->text( 0 ) );

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
	//
	mDatasetTree->setCurrentItem( current_dataset_item );

	// Delete dataset item (including children => file items)
	//
	delete current_dataset_item;

	// Notify about the change ( dataset deleted )
	//
	emit DatasetsChanged( nullptr );
	emit CurrentDatasetChanged( nullptr );

	// -TODO -- Old Brat code ----------------------
	//EnableCtrl();
	//
	//CNewDatasetEvent ev(GetId(), dsName);
	//wxPostEvent(GetParent(), ev);
	// ---------------------------------------------
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_DatasetsBrowserControlsBase.cpp"
