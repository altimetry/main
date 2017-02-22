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
#include "GUI/ControlPanels/DatasetsBrowserControls.h"

#include "libbrathl/Field.h"



/////////////////////////////////////////////////////////////////////////////////////
//								Dataset Browser
/////////////////////////////////////////////////////////////////////////////////////


void CDatasetsBrowserControls::Wire()
{
	base_t::Wire();
	
	connect( mAddFiles, SIGNAL( clicked(bool) ), this, SLOT( HandleAddFiles() ) );
	connect( mAddDir, SIGNAL( clicked(bool) ), this, SLOT( HandleAddDir() ) );
	connect( mRemove, SIGNAL( clicked(bool) ), this, SLOT( HandleRemoveFile() ) );
	connect( mClear, SIGNAL( clicked(bool) ), this, SLOT( HandleClearFiles() ) );
}



//explicit
CDatasetsBrowserControls::CDatasetsBrowserControls( CBratApplication &app, CDesktopManagerBase *manager, QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0
    : base_t( app, manager, parent, f )
{
    //datasets browser
    //
	mNewDataset = CreateToolButton( "", ":/images/OSGeo/dataset_new.png", "<b>New Dataset</b><br>Insert a new Dataset in current workspace." );
    mDeleteDataset = CreateToolButton( "", ":/images/OSGeo/dataset_delete.png", "<b>Delete...</b><br>Delete selected Dataset." );
    mAddFiles = CreateToolButton( "", ":/images/OSGeo/add_files_dataset.png", "<b>Add Files...</b><br>Add files to selected dataset.");
    mAddDir = CreateToolButton( "", ":/images/OSGeo/add_dir_dataset.png", "<b>Add Dir...</b><br>Add directory contents to selected dataset.");
    mRemove = CreateToolButton( "", ":/images/OSGeo/file_remove.png", "<b>Remove...</b><br>Remove selected file." );
    mClear = CreateToolButton( "", ":/images/OSGeo/file_delete.png", "<b>Clear...</b><br>Clear all files of selected dataset.");

    QWidget *buttons_row = CreateButtonRow( false, Qt::Horizontal, { mNewDataset, mDeleteDataset, nullptr, mAddFiles, mAddDir, mRemove, mClear } );

	//datasets tree

	mDatasetTree = new QTreeWidget();
	mDatasetTree->setToolTip("Tree of current workspace datasets");
	mDatasetTree->setHeaderHidden(true);

	AddSpecializedWgdets( buttons_row, "File Datasets", "File Description" );


    //connections
    //
    Wire();
}



//virtual 
CDatasetsBrowserControls::~CDatasetsBrowserControls()
{
}


//virtual 
void CDatasetsBrowserControls::UpdatePanelSelectionChange()
{
	CDataset *current_dataset = nullptr;
	if ( SelectedPanel() )
	{
		QList<QTreeWidgetItem*> l = mDatasetTree->selectedItems();				assert__( l.size() <= 1 );
		QTreeWidgetItem *current_dataset_item = l.empty() ? nullptr : l[0];		//mDatasetTree->currentItem(); can be inaccurate

		if ( current_dataset_item && current_dataset_item->parent() )			//check if selected item has parent (is a file)
		{
			current_dataset_item = current_dataset_item->parent();
		}
		current_dataset = current_dataset_item ? mWDataset->GetDataset( current_dataset_item->text( 0 ).toStdString() ) : nullptr;

		DrawDatasetTracks( current_dataset, true );
	}
}


QString CDatasetsBrowserControls::TreeItemSelectionChanged( QTreeWidgetItem *tree_item ) 
{
    QString dataset_name;

    // No selected Item ------------------------------------------------------------
	//
    if ( tree_item == nullptr )
    {
        // Update buttons status
		//
        mDeleteDataset->setDisabled(true);
        mAddFiles->setDisabled(true);
        mAddDir->setDisabled(true);
        mRemove->setDisabled(true);
        mClear->setDisabled(true);
    }
	else 
	//
	// Item is a Dataset -----------------------------------------------------------
	//
    if ( tree_item->parent() == nullptr ) // top level item has no parent() = nullptr
    {
        dataset_name = tree_item->text(0);

        // The field list was already cleared by base class

        // Update buttons status
		//
        mDeleteDataset->setEnabled(true);
        mAddFiles->setEnabled(true);
        mAddDir->setEnabled(true);
        mRemove->setDisabled(true);
        mClear->setEnabled(true);
    }
    else 
	//
	// Item is a File --------------------------------------------------------
    {
        dataset_name = tree_item->parent()->text(0);

		WaitCursor wait;

		// The field list was already cleared by base class

		CDataset *current_dataset  = mWDataset->GetDataset< CDataset >( tree_item->parent()->text(0).toStdString() );
		if ( current_dataset )
			FillFieldList( current_dataset, q2t<std::string>( tree_item->text( 0 ) ) );

        // Update buttons status
		//
        mDeleteDataset->setEnabled(true);
        mAddFiles->setEnabled(true);
        mAddDir->setEnabled(true);
        mRemove->setEnabled(true);
        mClear->setEnabled(true);
    }

	return dataset_name;
}



// Called
//	- after assigning new workspace 
//	- after clearing tree
//	- before invoking AddDatasetToTree iteratively to refill tree
//
//virtual 
void CDatasetsBrowserControls::PrepareWorkspaceChange()
{
}


//virtual 
void CDatasetsBrowserControls::FillFieldList( CDataset *current_dataset, const std::string &current_file )
{
	base_t::FillFieldList( current_dataset, current_file );
}



//virtual 
QTreeWidgetItem* CDatasetsBrowserControls::AddDatasetToTree( const QString &dataset_name )
{
    QIcon dataset_icon = QIcon(":/images/OSGeo/dataset.png");

    QTreeWidgetItem *dataset_item = new QTreeWidgetItem();
    dataset_item->setText(0, dataset_name );
    dataset_item->setToolTip(0, dataset_name );
    dataset_item->setIcon(0, dataset_icon);
    dataset_item->setFlags( dataset_item->flags() | Qt::ItemIsEditable );
    mDatasetTree->addTopLevelItem( dataset_item );

    // Fill tree with new dataset products
    FillFileTree( dataset_item );

    // Returns dataset item (required for automatically select a new dataset)
    // cannot be done here because this method is used in initialization of all datasets.
	//
    return dataset_item;
}



// Has the same purpose of HandleCurrentIndexChanged in sibling tab for RADS datasets
//
void CDatasetsBrowserControls::AddFiles( QStringList &paths_list )
{
	WaitCursor wait;


	if ( paths_list.empty() )
	{
		return;
	}
	paths_list.sort();

	// Get selected dataset item
	QTreeWidgetItem *current_dataset_item = mDatasetTree->currentItem();
	if ( current_dataset_item->parent() ) //check if selected item has parent (is a file)
	{
		current_dataset_item = current_dataset_item->parent();
	}
	CDataset *current_dataset = mWDataset->GetDataset( current_dataset_item->text( 0 ).toStdString() );

	// Get current files class and type, then assign

	std::string old_product_class = current_dataset->ProductClass();
	std::string old_product_type = current_dataset->ProductType();

	foreach( QString file_path, paths_list )
	{
		std::string normalized_path = NormalizedPath( q2a( file_path ) );
		try
		{
			// Insert file path into current Dataset; Check each file (as they are added to current dataset)

			current_dataset->AddProduct( normalized_path );
		}
		catch ( CException& e )
		{
			LOG_WARN( QString( "Unable to process file " ) + file_path + ".\nReason: " + e.what() );

			// Delete product from dataset
			current_dataset->EraseProduct( normalized_path );	//use the exact same path string, otherwise it is not recognized
		}
	}


	auto const &files = *const_cast< const CDataset* >( current_dataset )->GetProductList();
	if ( ( files.IsYFX() || files.IsZFXY() || files.IsGenericNetCdf() ) && files.size() > 1 )
	{
		std::string msg = "Warning - You have to check that all the files in the list : "
			"\n1) are in the same way (same structure, same fields with same dimension...)"
			"\n2) contain the same kind of data"
			"\n\n otherwise results may be ill-defined and confused or Brat may return a reading error.";
		LOG_WARN( msg );
	}


	CheckNewFilesClassAndType( old_product_class, old_product_type, current_dataset );


	// Clear all files and fill with new list
	qDeleteAll( current_dataset_item->takeChildren() );
	FillFileTree( current_dataset_item );

	// Redraw tracks. Item selection may not change, thus, is not catched by HandleTreeItemChanged()
	DrawDatasetTracks( current_dataset, true );

	// Notify about the change (files added)
	emit DatasetsChanged( current_dataset );
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//						Functions not in common with Rads Datasets tab
//////////////////////////////////////////////////////////////////////////////////////////////////////

void CDatasetsBrowserControls::HandleAddFiles()
{
	static std::string last_path = mBratPaths.UserDataDirectory();

    // Get selected dataset item
    QTreeWidgetItem *current_dataset_item = mDatasetTree->currentItem();

    if( current_dataset_item->parent() ) //check if selected item has parent (is a file)
        current_dataset_item = current_dataset_item->parent();

    // If empty or no Dataset is selected
    if ( current_dataset_item == nullptr  || !mWDataset->HasDataset() )
    {
        return;
    }

    // TODO: Change mWDataset->GetPath() by the last data path ??
    QStringList paths_list = BrowseFiles( this, "Select files...", last_path.c_str() );
	if ( !paths_list.empty() )
		last_path = GetDirectoryFromPath( q2a( paths_list[ 0 ] ) );

    AddFiles( paths_list );
}


void CDatasetsBrowserControls::HandleAddDir()
{
	static std::string last_path = mBratPaths.UserDataDirectory();

    // Get selected dataset item
    QTreeWidgetItem *current_dataset_item = mDatasetTree->currentItem();

    if( current_dataset_item->parent() ) //check if selected item has parent (is a file)
        current_dataset_item = current_dataset_item->parent();

    // If empty or no Dataset is selected
    if ( current_dataset_item == nullptr  || !mWDataset->HasDataset() )
    {
        return;
    }

    // TODO: Change mWDataset->GetPath() by the last data path ??
    QString dir_path = BrowseDirectory( this, "Select a directory...", last_path.c_str() );
    if ( dir_path.isEmpty() )
		return;

	last_path = q2a( dir_path );

    // Create list containing all directory files
    QStringList paths_list;
    QDirIterator it( dir_path,
                     QStringList() << "*",
                     QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks
                     /*, QDirIterator::Subdirectories*/  ); //uncomment for listing directory contents recursively.

    while ( it.hasNext() )
    {
        paths_list.append( it.next() );
    }

    AddFiles( paths_list );
}



void CDatasetsBrowserControls::HandleRemoveFile()
{
    // Get selected dataset item
    QTreeWidgetItem *current_dataset_item = nullptr;
    QTreeWidgetItem *current_file_item = mDatasetTree->currentItem();

    if( current_file_item->parent() ) //check if selected item has parent (is a file)
    {
        current_dataset_item = current_file_item->parent();
    }
    else
    {
        return;
    }

    if ( !mWDataset->HasDataset() )
    {
        return;
    }

    if ( !SimpleQuestion( QString("Are you sure to remove file '%1'?").arg(
                                  current_file_item->text(0))               ) )
    {
        return;   
    }

    // Delete product from current dataset
    CDataset *current_dataset = mWDataset->GetDataset( q2a( current_dataset_item->text(0) ) );
    current_dataset->EraseProduct( q2a( current_file_item->text(0) ) );

    // Delete product from tree
    delete current_file_item;

    ClearFieldList();

    // Notify about the change (file removed)
    emit DatasetsChanged( current_dataset );
	DrawDatasetTracks( current_dataset, true );
}



void CDatasetsBrowserControls::HandleClearFiles()
{
    // Get selected dataset item
    QTreeWidgetItem *current_dataset_item = mDatasetTree->currentItem();

    if( current_dataset_item->parent() ) //check if selected item has parent (is a file)
    {
        current_dataset_item = current_dataset_item->parent();
    }

    if( current_dataset_item->childCount() == 0 )
        return;


    if ( !SimpleQuestion( QString("Are you sure to remove all files of dataset '%1'?").arg(
                                   current_dataset_item->text(0)) ) )
    {
        return;
    }

    // Delete all products from current dataset
    CDataset *current_dataset = mWDataset->GetDataset( q2a( current_dataset_item->text(0) ) );
    current_dataset->ClearProductList();

    // Delete all products in current dataset
    current_dataset_item->takeChildren();

    ClearFieldList();

    // Redraw tracks
	DrawDatasetTracks( current_dataset, true );

    // Notify about the change (files removed) TODO: try to merge with previous signal??
    emit DatasetsChanged( current_dataset );
}



void CDatasetsBrowserControls::FillFileTree( QTreeWidgetItem *current_dataset_item )
{
    // Get current Dataset
	const CDataset *current_dataset = mWDataset->GetDataset( current_dataset_item->text( 0 ).toStdString() );		assert__( current_dataset );

    QIcon file_icon = QIcon( ":/images/OSGeo/file.png" );

	std::vector< std::string > v = current_dataset->GetFiles();
    for ( auto const &file : v )
    {
        QTreeWidgetItem *file_item = new QTreeWidgetItem();
		file_item->setText( 0, file.c_str() );
		file_item->setToolTip( 0, file.c_str() );	//see complete path when it exceeds list width
		file_item->setIcon(0, file_icon );

        current_dataset_item->addChild( file_item );
    }
}






///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_DatasetsBrowserControls.cpp"
