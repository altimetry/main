#include "new-gui/brat/stdafx.h"

#include "DataModels/Model.h"
#include "DataModels/Workspaces/Workspace.h"
#include "GUI/DisplayWidgets/TextWidget.h"
#include "GUI/ControlPanels/DatasetBrowserControls.h"

#include "libbrathl/Field.h"



/////////////////////////////////////////////////////////////////////////////////////
//								Dataset Browser
/////////////////////////////////////////////////////////////////////////////////////


//explicit
CDatasetBrowserControls::CDatasetBrowserControls( CModel &model, CDesktopManagerBase *manager, QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0
    : base_t( model, manager, parent, f )
	, mBratPaths( mModel.BratPaths() )
{
    // I. Browse Stack Widget; 2 pages: files and rads

    // - Page Files browser
    //

    // Datasets files buttons group
	mNewDataset = CreateToolButton( "", ":/images/OSGeo/dataset_new.png", "<b>New Dataset</b><br>Insert a new Dataset in current workspace." );
	mRemoveDataset = CreateToolButton( "", ":/images/OSGeo/dataset_remove.png", "<b>Delete...</b><br>Delete selected Dataset." );
    mAddFiles = CreateToolButton( "", ":/images/OSGeo/add_files_dataset.png", "<b>Add Files...</b><br>Add files to selected dataset.");
    mAddDir = CreateToolButton( "", ":/images/OSGeo/add_dir_dataset.png", "<b>Add Dir...</b><br>Add directory contents to selected dataset.");
    mRemove = CreateToolButton( "", ":/images/OSGeo/file_remove.png", "<b>Remove...</b><br>Remove selected file." );
    mClear = CreateToolButton( "", ":/images/OSGeo/file_delete.png", "<b>Clear...</b><br>Clear all files of selected dataset.");

    QWidget *buttons_row = CreateButtonRow( false, Qt::Horizontal, {
        mNewDataset, mRemoveDataset, nullptr, mAddFiles, mAddDir, mRemove, mClear } );

	AddTopWidget( buttons_row );

	//datasets tree

    mDatasetTree = new QTreeWidget();
    mDatasetTree->setToolTip("Tree of current workspace datasets");
    mDatasetTree->setHeaderHidden(true);

    QGroupBox *page_1 = CreateGroupBox( ELayoutType::Vertical, { mDatasetTree }, "File Datasets", nullptr, s, m, m, m, m );



    // - Page RADS browser
    //
    QListWidget *mRadsList = CreateBooleanList( nullptr, { { "ERS1", true }, { "Jason1" }, { "CryoSat" } } );	//TODO, obviously

    QGroupBox *page_2 = CreateGroupBox( ELayoutType::Vertical, { mRadsList }, "Missions", nullptr, s, m, m, m, m );


    // Group Stack Widget Buttons & add Stack Widget itself
    //
    mBrowserStakWidget = new CStackedWidget( nullptr, { 
		{ page_1, "Files", "", ":/images/OSGeo/file.png", true }, 
		{ page_2, "RADS", "", "://images/rads.gif", true } 
	} );

    m_BrowseFilesButton = qobject_cast<stack_button_type*>( mBrowserStakWidget->Button( 0 ) );
    m_BrowseRadsButton = qobject_cast<stack_button_type*>( mBrowserStakWidget->Button( 1 ) );
    auto *browse_buttons = CreateButtonRow( false, Qt::Vertical, { m_BrowseFilesButton, m_BrowseRadsButton } );

    auto *browse_group = CreateGroupBox( ELayoutType::Horizontal, { mBrowserStakWidget, browse_buttons }, "", nullptr, s, 4, 4, 4, 4 );


    // II. File Description group
    //
    mFileDesc = new CTextWidget;
    mFileDesc->SetReadOnlyEditor(true);
    mFileDescGroup = CreateGroupBox( ELayoutType::Horizontal, { mFileDesc }, "File Description", nullptr, s, 4, 4, 4, 4 );


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

	AddTopSplitter( Qt::Vertical, { browse_group, mFileDescGroup, fields_desc_group } );


    // IV. Making connections
    //
    Wire();
}



void CDatasetBrowserControls::Wire()
{
    connect( mDatasetTree, SIGNAL( itemSelectionChanged() ), this, SLOT( HandleTreeItemChanged() ) );
    connect( mDatasetTree, SIGNAL( itemExpanded(QTreeWidgetItem*) ), this, SLOT( HandleDatasetExpanded() ) );
    connect( mDatasetTree, SIGNAL( itemChanged(QTreeWidgetItem*,int) ), this, SLOT( HandleRenameDataset(QTreeWidgetItem*,int) ) );

    connect( mNewDataset, SIGNAL( clicked(bool) ), this, SLOT (HandleNewDataset()) );
    connect( mRemoveDataset, SIGNAL( clicked(bool) ), this, SLOT (HandleDeleteDataset()) );

    connect( mAddFiles, SIGNAL( clicked(bool) ), this, SLOT( HandleAddFiles() ) );
    connect( mAddDir, SIGNAL( clicked(bool) ), this, SLOT( HandleAddDir() ) );
    connect( mRemove, SIGNAL( clicked(bool) ), this, SLOT( HandleRemoveFile() ) );
    connect( mClear, SIGNAL( clicked(bool) ), this, SLOT( HandleClearFiles() ) );

    connect( mFieldList, SIGNAL( itemSelectionChanged() ), this, SLOT( HandleFieldChanged() ) );
}



void CDatasetBrowserControls::HandleDatasetExpanded()
{
    mDatasetTree->resizeColumnToContents(0);
}



void CDatasetBrowserControls::HandleRenameDataset(QTreeWidgetItem *dataset_item, int col)
{
    Q_UNUSED( col );

    static QRegExp re("[_a-zA-Z0-9]+"); // only alphanumeric letters

    // ToolTip contains the old name of the dataset
    QString old_name( dataset_item->toolTip(0) );
    QString new_name( dataset_item->text(0) );

    if (re.exactMatch( new_name ) ) // Has a Valid Name
    {
        CDataset *current_dataset = mWDataset->GetDataset( old_name.toStdString() );

        if( mWDataset->RenameDataset(current_dataset, new_name.toStdString() ) == true )
        {
            // Dataset renamed. Update tooltip and notify the change
            dataset_item->setToolTip(0, new_name );
            emit DatasetsChanged( current_dataset );
            return;
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
    dataset_item->setText(0, old_name );
}



void CDatasetBrowserControls::HandleTreeItemChanged( )
{
    static CDataset *current_dataset = nullptr;
    QString dataset_name;

    QTreeWidgetItem *tree_item = mDatasetTree->currentItem();

    // No selected Item ------------------------------------------------------------
    if ( tree_item == nullptr )
    {
        // Update buttons status
        mRemoveDataset->setDisabled(true);
        mAddFiles->setDisabled(true);
        mAddDir->setDisabled(true);
        mRemove->setDisabled(true);
        mClear->setDisabled(true);
        return;
    }

    // Item is a Dataset -----------------------------------------------------------
    if (tree_item->parent() == nullptr ) // top level item has no parent() = nullptr
    {
        dataset_name = tree_item->text(0);
        DatasetChanged( tree_item );

        // Update buttons status
        mRemoveDataset->setEnabled(true);
        mAddFiles->setEnabled(true);
        mAddDir->setEnabled(true);
        mRemove->setDisabled(true);
        mClear->setEnabled(true);
    }
    else // Item is a File --------------------------------------------------------
    {
        dataset_name = tree_item->parent()->text(0);
        FileChanged( tree_item );

        // Update buttons status
        mRemoveDataset->setEnabled(true);
        mAddFiles->setEnabled(true);
        mAddDir->setEnabled(true);
        mRemove->setEnabled(true);
        mClear->setEnabled(true);
    }

    // notify the world (different dataset)
    CDataset *dataset = mWDataset->GetDataset( dataset_name.toStdString() );
    if( current_dataset != dataset )
    {
        current_dataset = dataset;
        emit CurrentDatasetChanged( current_dataset );
    }
}



void CDatasetBrowserControls::HandleWorkspaceChanged( CWorkspaceDataset *wksd )
{
    mWDataset = wksd;

    // Fill DatasetTree with Datasets items
    mDatasetTree->clear();
    HandleTreeItemChanged(); // for updating buttons status

    if (wksd)
    {
		auto *datasets = mWDataset->GetDatasets();
        for( auto const it : *datasets )
        {
            AddDatasetToTree( t2q( it.first ) );
        }

		//if ( !datasets->empty() )
		//{
		//	QTreeWidgetItemIterator it( mDatasetTree );
		//	if ( *it  )
		//		mDatasetTree->setCurrentItem( *it );
		//}
    }
}



QTreeWidgetItem *CDatasetBrowserControls::AddDatasetToTree( const QString &dataset_name )
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
    return dataset_item;
}



void CDatasetBrowserControls::DatasetChanged( QTreeWidgetItem *tree_item )
{
    Q_UNUSED( tree_item );

    WaitCursor wait;

    ClearFieldList();
}



void CDatasetBrowserControls::FileChanged( QTreeWidgetItem *file_item )
{
    WaitCursor wait;

    // Clear field list and descriptions
    ClearFieldList();

    FillFieldList( file_item );
}



void CDatasetBrowserControls::HandleFieldChanged()
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



void CDatasetBrowserControls::HandleNewDataset()
{
	assert__( mWDataset != nullptr );

    WaitCursor wait;

    // Get name for the new dataset
    std::string dataset_name = mWDataset->GetDatasetNewName();

    // Insert dataset in workspace and into DatasetTree
    mWDataset->InsertDataset( dataset_name );
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
    //emit CurrentDatasetChanged( mWDataset->GetDataset( q2a( dataset_name ) ) );
}



void CDatasetBrowserControls::HandleDeleteDataset()
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
	CDataset *current_dataset = mWDataset->GetDataset( current_dataset_item->text( 0 ).toStdString() );

	CWorkspaceOperation *wks = mModel.Workspace<CWorkspaceOperation>();			assert__( wks != nullptr );
	std::string error_msg;
	CStringArray operation_names;
	if ( wks->UseDataset( current_dataset->GetName(), error_msg, &operation_names ) )
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
	emit DatasetsChanged( current_dataset );

	// -TODO -- Old Brat code ----------------------
	//EnableCtrl();
	//
	//CNewDatasetEvent ev(GetId(), dsName);
	//wxPostEvent(GetParent(), ev);
	// ---------------------------------------------

	// // FEMM (TBC) - Signal emission is not required. Previous dataset is automatically selected and catched by HandleTreeItemChanged()
	//emit CurrentDatasetChanged( nullptr );
}



void CDatasetBrowserControls::HandleAddFiles()
{
	static std::string last_path = mBratPaths.mExternalDataDir;

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


void CDatasetBrowserControls::HandleAddDir()
{
	static std::string last_path = mBratPaths.mExternalDataDir;

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



void CDatasetBrowserControls::HandleRemoveFile()
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
    emit CurrentDatasetChanged( current_dataset );
}



void CDatasetBrowserControls::HandleClearFiles()
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

    // Notify to redraw tracks. Item selection may not change, thus, is not catched by HandleTreeItemChanged()
    emit CurrentDatasetChanged( current_dataset );

    // Notify about the change (files removed) TODO: try to merge with previous signal??
    emit DatasetsChanged( current_dataset );
}


void CDatasetBrowserControls::AddFiles( QStringList &paths_list )
{
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

	// Get current files class and type
	std::string old_product_class = current_dataset->GetProductList()->m_productClass;
	std::string old_product_type = current_dataset->GetProductList()->m_productType;

	// -TODO-- Old Brat code for applying select criteria -----
	//    bool applySelectionCriteria = GetDsapplycrit()->GetValue();

	//    if (applySelectionCriteria)
	//    {
	//      CStringList filesOut;
	//      ApplySelectionCriteria(fileList, filesOut);

	//      fileList.clear();
	//      fileList.Insert(filesOut);
	//    }

	foreach( QString file_path, paths_list )
	{
		std::string normalized_path = NormalizedPath( q2a( file_path ) );
		try
		{
			// Insert file path into current Dataset
			current_dataset->GetProductList()->InsertUnique( normalized_path );

			// Check each file (as they are added to current dataset)
			current_dataset->CheckFiles();
			std::string msg = current_dataset->GetProductList()->GetMessage();
			if ( !msg.empty() )
			{
				SimpleWarnBox( msg );
			}
		}
		catch ( CException& e )
		{
			SimpleWarnBox( QString( "Unable to process files. Please apply correction.\n\nReason:\n%1" ).arg( e.what() ) );

			// Delete product from dataset
			current_dataset->EraseProduct( normalized_path );	//use the exact same path string, otherwise it is not recognized
		}
	}

	// Check new files class and type
	const bool is_same_product_class_type = 
		str_icmp( old_product_class, current_dataset->GetProductList()->m_productClass ) &&
		str_icmp( old_product_type, current_dataset->GetProductList()->m_productType );

	CWorkspaceOperation *wks = mModel.Workspace<CWorkspaceOperation>();		assert__( wks != nullptr );
	std::string error_msg;
	CStringArray operation_names;
	bool used_by_operations = wks->UseDataset( current_dataset->GetName(), error_msg, &operation_names );
	if ( !is_same_product_class_type && used_by_operations )
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
			+ current_dataset->GetProductList()->m_productClass
			+ "/"
			+ current_dataset->GetProductList()->m_productType
			+ "' product class/type.\n\nThis dataset is used by the operations below:\n"
			+ str
			+ "\n\nBe sure field's names used in these operations match the fields of the dataset files"
			);
	}

	// Clear all files and fill with new list
	qDeleteAll( current_dataset_item->takeChildren() );
	FillFileTree( current_dataset_item );

	// Notify to redraw tracks. Item selection may not change, thus, is not catched by HandleTreeItemChanged()
	emit CurrentDatasetChanged( current_dataset );

	// Notify about the change (files added) TODO: try to merge with previous signal??
	emit DatasetsChanged( current_dataset );
}



void CDatasetBrowserControls::FillFileTree( QTreeWidgetItem *current_dataset_item )
{
    // Get current Dataset            (do not use this before ensuring "there are" datasets)
    CDataset *current_dataset  = mWDataset->GetDataset( current_dataset_item->text(0).toStdString() );

    QIcon file_icon = QIcon(":/images/OSGeo/file.png");

    for ( auto const it : *current_dataset->GetProductList() )
    {
        QTreeWidgetItem *file = new QTreeWidgetItem();
        file->setText(0, it.c_str());
        file->setToolTip(0, it.c_str() );	//see complete path when it exceeds list width
        file->setIcon(0, file_icon );

        current_dataset_item->addChild(file);
    }
}



void CDatasetBrowserControls::ClearFieldList()  // the old method was: ClearDict();
{   
    mFileDescGroup->setDisabled(true);
    mFileDesc->setDisabled(true);
    mFileDesc->clear();

    mFieldList->clear();
    mFieldDesc->clear();
}



void CDatasetBrowserControls::FillFieldList( QTreeWidgetItem *current_file_item )
{
    //do not use this before ensuring "there are" datasets
    CDataset *current_dataset  = mWDataset->GetDataset( current_file_item->parent()->text(0).toStdString() );

    if( current_dataset == nullptr )
        return;

    CProduct *product = nullptr;
    try
    {
        product = current_dataset->SetProduct( q2t<std::string>( current_file_item->text(0) ) );

        // GetDictlist()->InsertProduct(m_product); //////////////////////////////////////////////
        CTreeField* tree = product->GetTreeField();

        for ( auto const *object : *tree )
        {
            const CField *field  = static_cast< const CField* >( object );			assert__( dynamic_cast< const CField*>( object ) != nullptr );

            // InsertField(field); ===========================================
            auto const &type_id = typeid( *field );

            if ( type_id == typeid( CFieldIndex )
                ||  type_id == typeid( CFieldRecord )
                || ( ( type_id == typeid( CFieldArray ) ) && ( field->IsFixedSize() == false ) )
                || field->IsVirtual() )
            {
                continue;
            }

            QListWidgetItem *item = new QListWidgetItem;

            item->setData(Qt::AccessibleTextRole,        field->GetName().c_str());
            item->setData(Qt::AccessibleDescriptionRole, field->GetDescription().c_str());
            item->setText( const_cast<CField*>( field )->GetFullName().c_str() );

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

        if ( product != nullptr )
        {
            mFileDescGroup->setEnabled(product->IsNetCdf());

            mFileDesc->setEnabled( product->IsNetCdf() );
            mFileDesc->setText( QString("Product : ") +
                                product->GetProductClass().c_str() +
                                QString(" / ") +
                                product->GetProductType().c_str() +
                                QString("\n------------------------------------------------------\n") +
                                product->GetDescription().c_str() );
        }
    }
    catch ( CException& e )
    {
        //UNUSED( e );
        SimpleErrorBox(e.Message());
        //wxMessageBox(std::string::Format("Unable to process files.\nReason:\n%s",
        //                              e.what()),  "Warning",  wxOK | wxICON_EXCLAMATION);
        //this->SetCursor(wxNullCursor);
    }

    // Sorts field list
    mFieldList->sortItems();

    delete product;
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_DatasetBrowserControls.cpp"
