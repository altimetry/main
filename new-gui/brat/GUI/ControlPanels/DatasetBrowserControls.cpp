#include "new-gui/brat/stdafx.h"

#include "DataModels/Workspaces/Workspace.h"
#include "GUI/DisplayWidgets/TextWidget.h"
#include "GUI/ControlPanels/DatasetBrowserControls.h"

#include "libbrathl/Field.h"



/////////////////////////////////////////////////////////////////////////////////////
//								Dataset Browser
/////////////////////////////////////////////////////////////////////////////////////

void CDatasetBrowserControls::HandlePageChanged( int index )
{
    qDebug() << index;
}


//explicit
CDatasetBrowserControls::CDatasetBrowserControls( CDesktopManagerBase *manager, QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0
    : base_t( manager, parent, f )
{
    // I. Browse Stack Widget; 2 pages: files and rads

    // - Page Files browser
    //
    mDatasetTree = new QTreeWidget();
    mDatasetTree->setToolTip("Tree of current workspace datasets");
    mDatasetTree->setHeaderHidden(true);

    mAddFiles = new QPushButton( "Add Files..." );
    mAddDir = new QPushButton( "Add Dir..." );
    mRemove = new QPushButton( "Remove..." );
    mClear = new QPushButton( "Clear..." );

    auto mUp = new QPushButton( "Up" );
    auto mDown = new QPushButton( "Down" );			//QPushButton *mSort = new QPushButton( "Sort", page_1 );	by QListWidget ?
    auto mCheckFiles = new QPushButton( "Check" );

    mUp->setDisabled(true);
    mDown->setDisabled(true);
    mCheckFiles->setDisabled(true);

    // - Dataset name and buttons group
    //
    mNewDataset = new QPushButton( "New..." );
    mDeleteDataset = new QPushButton( "Delete..." );
    mRenameDataset = new QPushButton( "Rename..." );
    auto mSaveDataset = new QPushButton( "Save..." );	// TODO confirm that save will only be done in the scope of whole workspace

    mRenameDataset->setDisabled(true);
    mSaveDataset->setDisabled(true);

//    AddTopGroupBox(
//        ELayoutType::Grid, {
//            mDatasetsCombo, mNewDataset, mDeleteDataset, mRenameDataset, nullptr,
//			mSaveDataset
//        },
//        "", 0, 0, 0, 0, 0
//        );

    QBoxLayout *buttons_vl = LayoutWidgets( Qt::Vertical, { mAddFiles,
                                                            mAddDir,
                                                            /*mSort, */
                                                            mRemove,
                                                            mClear,
                                                            mUp,
                                                            mDown,
                                                            mCheckFiles,
                                                            mNewDataset,
                                                            mDeleteDataset,
                                                            mRenameDataset,
                                                            mSaveDataset  } );

    QGroupBox *page_1 = CreateGroupBox( ELayoutType::Horizontal, { mDatasetTree, buttons_vl }, "", nullptr, 0, 2, 2, 2, 2 );


    // - Page RADS browser
    //
    auto rads_label = new QLabel( "Missions" );
    QListWidget *mRadsList = CreateBooleanList( nullptr, { { "ERS1", true }, { "Jason1" }, { "CryoSat" } } );	//TODO, obviously
    //QBoxLayout *rads_vl =
    LayoutWidgets( Qt::Vertical, { rads_label, mRadsList }, nullptr, 0, 2, 2, 2, 2 );

    QGroupBox *page_2 = CreateGroupBox( ELayoutType::Vertical, { rads_label, mRadsList }, "", nullptr, 0, 2, 2, 2, 2 );

    // Group Stack Widget Buttons & add Stack Widget itself
    //
    mBrowserStakWidget = new CStackedWidget( nullptr, { { page_1, "Files" }, { page_2, "RADS" } } );

    m_BrowseFilesButton = qobject_cast<QPushButton*>( mBrowserStakWidget->Button( 0 ) );
    m_BrowseRadsButton = qobject_cast<QPushButton*>( mBrowserStakWidget->Button( 1 ) );
    QBoxLayout *browse_buttons_hl = LayoutWidgets( Qt::Horizontal, { m_BrowseFilesButton, m_BrowseRadsButton }, nullptr, 0, 2, 2, 2, 2 );

    AddTopGroupBox( ELayoutType::Vertical, { browse_buttons_hl, mBrowserStakWidget }, "", 4, 4, 4, 4, 4 );


    // II. Variable Description group
    //
    mFileDesc = new CTextWidget;
    mFileDesc->setReadOnly(true);

    mFileDescGroup = AddTopGroupBox( ELayoutType::Horizontal, { mFileDesc }, "File Description", 4, 4, 4, 4, 4 );

    mFieldList = new QListWidget;	   //mFieldList->setMaximumWidth( 120 );
    mFieldList->setSelectionMode(QAbstractItemView::ExtendedSelection); // Multiple items can be selected

    mFieldDesc = new CTextWidget;
    mFieldDesc->setReadOnly(true);

    AddTopGroupBox( ELayoutType::Horizontal, { mFieldList, mFieldDesc }, "Field Description", 4, 4, 4, 4, 4 );

    // IV. Making connections
    //
    Wire();
}



void CDatasetBrowserControls::Wire()
{
    connect( mDatasetTree, SIGNAL( itemSelectionChanged() ), this, SLOT( HandleTreeItemChanged() ) );
    connect( mDatasetTree, SIGNAL( itemExpanded(QTreeWidgetItem*) ), this, SLOT( ResizeDatasetTree() ) );
	//RCCC line above triggers the following message in application log window : WARN] 2016-03-04T02:23:58	2	Object::connect: No such slot CDatasetBrowserControls::ResizeDatasetTree() in ..\..\..\..\source\new-gui\brat\GUI\ControlPanels\DatasetBrowserControls.cpp:125
    connect( mDatasetTree, SIGNAL( itemChanged(QTreeWidgetItem*,int) ), this, SLOT( HandleRenameDataset(QTreeWidgetItem*,int) ) );

    connect( mNewDataset, SIGNAL( clicked(bool) ), this, SLOT (HandleNewDataset()) );
    connect( mDeleteDataset, SIGNAL( clicked(bool) ), this, SLOT (HandleDeleteDataset()) );

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
            dataset_item->setToolTip(0, new_name );
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
    QTreeWidgetItem *tree_item = mDatasetTree->currentItem();

    if ( tree_item == nullptr )
    {
        return;
    }

    // top-level item has no parent() = NULL -> is a dataset
    if (tree_item->parent() == nullptr )
    {
        DatasetChanged( tree_item );

		// notify the world
		CDataset *current_dataset = mWDataset->GetDataset( tree_item->text(0).toStdString() );
		emit CurrentDatasetChanged( current_dataset );
    }
    else // is a file
    {
        FileChanged( tree_item );
    }
}



void CDatasetBrowserControls::HandleWorkspaceChanged( CWorkspaceDataset *wksd )
{
    mWDataset = wksd;

    // Fill DatasetTree with Datasets items
    mDatasetTree->clear();

    if (wksd)
    {
        for( auto const it : *mWDataset->GetDatasets() )
        {
            AddDatasetToTree( t2q( it.first ) );
        }
    }
}



void CDatasetBrowserControls::AddDatasetToTree( const QString &dataset_name )
{
    QIcon dataset_icon = QIcon(":/images/OSGeo/db.png");

    QTreeWidgetItem *dataset_item = new QTreeWidgetItem();
    dataset_item->setText(0, dataset_name );
    dataset_item->setToolTip(0, dataset_name );
    dataset_item->setIcon(0, dataset_icon);
    dataset_item->setFlags( dataset_item->flags() | Qt::ItemIsEditable );
    mDatasetTree->addTopLevelItem( dataset_item );

    FillFileTree( dataset_item );
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
    if( mWDataset == nullptr )
    {
        return;
    }

    WaitCursor wait;

    // Get name for the new dataset
    QString dataset_name ( mWDataset->GetDatasetNewName().c_str() );

    // Insert dataset in workspace and into DatasetTree
    mWDataset->InsertDataset( dataset_name.toStdString() );
    AddDatasetToTree( dataset_name );

    // -TODO(Delete)-- Old Brat code - mWDataset->GetDatasetNewName() ensures that new dataset does not exists ------
    //else
    //{
    //    SimpleWarnBox( QString("Dataset '%s' already exists.").sprintf( dataset_name.toStdString().c_str() ));
    //}
    // ----------------------------------------------------------------------------------------------------------

    // Clear list of files and fields list
    ClearFieldList();

    // -TODO -- Old Brat code ----------------------
    //EnableCtrl();
    //
    //CNewDatasetEvent ev(GetId(), dsName);
    //wxPostEvent(GetParent(), ev);
    // ---------------------------------------------

	emit CurrentDatasetChanged( mWDataset->GetDataset( q2a( dataset_name ) ) );
}



void CDatasetBrowserControls::HandleDeleteDataset()
{
    // Get selected dataset item
    QTreeWidgetItem *current_dataset_item = mDatasetTree->currentItem();

    if( current_dataset_item->parent() ) //check if selected item has parent (is a file)
        current_dataset_item = current_dataset_item->parent();

    if( mWDataset == nullptr || current_dataset_item == nullptr )
    {
        return;
    }

    // Get current dataset
    CDataset *current_dataset = mWDataset->GetDataset( current_dataset_item->text(0).toStdString() );

    if ( !SimpleQuestion( QString("Are you sure to delete dataset '%1'?").arg(current_dataset->GetName().c_str()) ) )
    {
        return;
    }

    // -TODO -- Old Brat code ----------------------------------------------------
    //CStringArray operationNames;
    //bOk = wxGetApp().CanDeleteDataset(m_dataset->GetName(), &operationNames);

    //if (bOk == false)
    //{
    //  std::string str = operationNames.ToString("\n", false);
    //  wxMessageBox(wxString::Format("Unable to delete dataset '%s'.\nIt is used by the operations below:\n%s\n",
    //                                m_dataset->GetName().c_str(),
    //                                str.c_str()),
    //              "Warning",
    //              wxOK | wxICON_EXCLAMATION);
    //  return;
    //}
    // --------------------------------------------------------------------

    if( !mWDataset->DeleteDataset( current_dataset ) )
    {
        SimpleWarnBox( QString( "Unable to delete dataset '%1").arg(
                                current_dataset->GetName().c_str()  ) );
        return;
    }

    delete current_dataset_item;
    ClearFieldList();

    // -TODO -- Old Brat code ----------------------
    //EnableCtrl();
    //
    //CNewDatasetEvent ev(GetId(), dsName);
    //wxPostEvent(GetParent(), ev);
    // ---------------------------------------------

	emit CurrentDatasetChanged( nullptr );
}



void CDatasetBrowserControls::HandleAddFiles()
{
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
    QStringList paths_list = BrowseFiles( this, "Select files...", mWDataset->GetPath().c_str() );

    AddFiles(paths_list);
}


void CDatasetBrowserControls::HandleAddDir()
{
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
    QString dir_path = BrowseDirectory( this, "Select a directory...", mWDataset->GetPath().c_str() );
    if ( dir_path.isEmpty() )
	{
		return;
	}

    // Create list containing all directory files
    QStringList file_paths;
    QDirIterator it( dir_path,
                     QStringList() << "*",
                     QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks
                     /*, QDirIterator::Subdirectories*/  ); //uncomment for listing directory contents recursively.

    while ( it.hasNext() )
    {
        file_paths.append( it.next() );
    }

    AddFiles( file_paths );
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


    if ( !SimpleQuestion( "Are you sure to remove the selected file?" ) )
    {
        return;
    }

    // Delete product from current dataset
    CDataset *current_dataset = mWDataset->GetDataset( q2a( current_dataset_item->text(0) ) );
    current_dataset->EraseProduct( q2a( current_file_item->text(0) ) );

    // Delete product from tree
    delete current_file_item;

    ClearFieldList();

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


    if ( !SimpleQuestion("Are you sure to remove all files of the current dataset?") )
    {
        return;
    }

    // Delete all products from current dataset
    CDataset *current_dataset = mWDataset->GetDataset( q2a( current_dataset_item->text(0) ) );
    current_dataset->ClearProductList();

    // Delete all products in current dataset
    current_dataset_item->takeChildren();

    ClearFieldList();

	emit CurrentDatasetChanged( current_dataset );
}


void CDatasetBrowserControls::AddFiles(QStringList &paths_list)
{
    if (paths_list.empty())
    {
        return;
    }

    paths_list.sort();

    // Get selected dataset item
    QTreeWidgetItem *current_dataset_item = mDatasetTree->currentItem();

    if( current_dataset_item->parent() ) //check if selected item has parent (is a file)
    {
        current_dataset_item = current_dataset_item->parent();
    }


// -TODO-- Old Brat code for saving last data path -----
//    wxFileName currentDir;

//    currentDir.Assign(paths[0]);
//    currentDir.Normalize();
//    wxGetApp().SetLastDataPath(currentDir.GetPath());
// ----------------------------------------------------


// -TODO (delete) - Changed in order to add only valid product files ////
//    CStringList file_list;
//    foreach(QString path, paths_list)
//    {
//        file_list.InsertUnique( path.toStdString() );
//    }
//  //////////////////////////////////////////////////////////////////////

// -TODO-- Old Brat code for applying select criteria -----
//    bool applySelectionCriteria = GetDsapplycrit()->GetValue();

//    if (applySelectionCriteria)
//    {
//      CStringList filesOut;
//      ApplySelectionCriteria(fileList, filesOut);

//      fileList.clear();
//      fileList.Insert(filesOut);
//    }
// --------------------------------------------------------

// -TODO (delete) - Changed in order to add only valid product files /////
//    // Insert file_list into current Dataset
//    CDataset *current_dataset = mWDataset->GetDataset( current_dataset_item->text(0).toStdString() );
//    current_dataset->InsertUniqueProducts(file_list);

//    // Clear all files and fill with new list
//    current_dataset_item->takeChildren();
//    FillFileTree( current_dataset_item );

//    CheckFiles();
//  //////////////////////////////////////////////////////////////////////

    CDataset *current_dataset = mWDataset->GetDataset( current_dataset_item->text(0).toStdString() );

    foreach(QString file_path, paths_list)
    {
        try
        {
            // Insert file path into current Dataset
            current_dataset->GetProductList()->InsertUnique( q2a( file_path ) );

            // Check each file (as they are added to current dataset)
            current_dataset->CheckFiles();
        }
        catch (CException& e)
        {
            SimpleWarnBox( QString( "Unable to process files. Please apply correction.\n\nReason:\n%1").arg(
                                    e.what()) );

            // Delete product from dataset
            current_dataset->EraseProduct( q2a( file_path ) );
        }
    }

    // Clear all files and fill with new list
    current_dataset_item->takeChildren();
    FillFileTree( current_dataset_item );

	emit CurrentDatasetChanged( current_dataset );
}



void CDatasetBrowserControls::FillFileTree( QTreeWidgetItem *current_dataset_item )
{
    // Get current Dataset            (do not use this before ensuring "there are" datasets)
    CDataset *current_dataset  = mWDataset->GetDataset( current_dataset_item->text(0).toStdString() );

    for ( auto const it : *current_dataset->GetProductList() )
    {
        QTreeWidgetItem *file = new QTreeWidgetItem();
        file->setText(0, it.c_str());
        file->setToolTip(0, it.c_str() );	//see complete path when it exceeds list width

        current_dataset_item->addChild(file);
    }
}



void CDatasetBrowserControls::CheckFiles()
{

// -TODO (Delete)-- Old Brat code - Clear product list & insert list of files in mFilesList -------------
//    CStringArray array;

//    wxString oldProductClass = m_dataset->GetProductList()->m_productClass.c_str();
//    wxString oldProductType = m_dataset->GetProductList()->m_productType.c_str();

//    m_dataset->GetProductList()->clear();

//    GetFiles(array);

//    if (array.size() <= 0)
//    {
//      return true;
//    }

//    m_dataset->GetProductList()->Insert(array);
// ------------------------------------------------------------------------------------------------------

    // Get selected dataset item
    QTreeWidgetItem *current_dataset_item = mDatasetTree->currentItem();

    if( current_dataset_item->parent() ) //check if selected item has parent (is a file)
    {
        current_dataset_item = current_dataset_item->parent();
    }

    try
    {
        // Get current Dataset
        CDataset *current_dataset = mWDataset->GetDataset( current_dataset_item->text(0).toStdString() );
        current_dataset->CheckFiles();

        // -TODO (Delete)-- Old Brat code - Does not make sense because msg is always empty!! -------------
        //wxString msg = m_dataset->GetProductList()->GetMessage().c_str();
        //if (!(msg.IsEmpty()))
        //{
        //  wxMessageBox(msg,
        //            "Warning",
        //            wxOK | wxICON_INFORMATION);
        //}
        // ----------------------------------------------------------------------------------------------
    }
    catch (CException& e)
    {
        SimpleWarnBox( QString( "Unable to process files. Please apply correction.\n\nReason:\n%1").arg(
                                e.what()) );
        return;
    }


// -TODO-- Old Brat code - Check if product class&type has been changed and if it is used in any operation  ---
//    if (bOk)
//    {
//      bool isSameProductClassType = ( oldProductClass.CmpNoCase(m_dataset->GetProductList()->m_productClass.c_str()) == 0 );
//      isSameProductClassType &= ( oldProductType.CmpNoCase(m_dataset->GetProductList()->m_productType.c_str()) == 0 );

//      CStringArray operationNames;

//      bool usedByOperation = !(wxGetApp().CanDeleteDataset(m_dataset->GetName(), &operationNames));

//      if ((!isSameProductClassType) && (usedByOperation))
//      {
//        std::string str = operationNames.ToString("\n", false);
//        wxMessageBox(wxString::Format("Warning: Files contained in the dataset '%s' have been changed from '%s/%s' to '%s/%s' product class/type.\n"
//                                      "\nThis dataset is used by the operations below:\n%s\n"
//                                      "\nBe sure field's names used in these operations match the fields of the dataset files",
//                                      m_dataset->GetName().c_str(),
//                                      oldProductClass.c_str(), oldProductType.c_str(),
//                                      m_dataset->GetProductList()->m_productClass.c_str(), m_dataset->GetProductList()->m_productType.c_str(),
//                                      str.c_str()),
//                    "Warning",
//                    wxOK | wxICON_EXCLAMATION);
//      }
//    }
// -----------------------------------------------------------------------------------------------------------

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
        //wxMessageBox(wxString::Format("Unable to process files.\nReason:\n%s",
        //                              e.what()),  "Warning",  wxOK | wxICON_EXCLAMATION);
        //this->SetCursor(wxNullCursor);
    }

    delete product;
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_DatasetBrowserControls.cpp"
