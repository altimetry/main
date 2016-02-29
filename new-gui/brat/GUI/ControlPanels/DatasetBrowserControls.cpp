#include "new-gui/brat/stdafx.h"

#include "DataModels/Workspaces/Workspace.h"
#include "GUI/DisplayWidgets/TextWidget.h"
#include "GUI/ControlPanels/DatasetBrowserControls.h"

#include "libbrathl/Field.h"



/////////////////////////////////////////////////////////////////////////////////////
//								Dataset Browser
/////////////////////////////////////////////////////////////////////////////////////

void CDatasetBrowserControls::PageChanged( int index )
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
    mFilesList = new QListWidget;

    mAddFiles = new QPushButton( "Add Files..." );
    mAddDir = new QPushButton( "Add Dir..." );
    mRemove = new QPushButton( "Remove" );
    mClear = new QPushButton( "Clear" );

    auto mUp = new QPushButton( "Up" );
    auto mDown = new QPushButton( "Down" );			//QPushButton *mSort = new QPushButton( "Sort", page_1 );	by QListWidget ?
    auto mCheckFiles = new QPushButton( "Check" );

    mUp->setDisabled(true);
    mDown->setDisabled(true);
    mCheckFiles->setDisabled(true);

    QBoxLayout *buttons_vl = LayoutWidgets( Qt::Vertical, { mAddFiles, mAddDir, mUp, mDown, /*mSort, */mRemove, mClear, mCheckFiles } );

    QGroupBox *page_1 = CreateGroupBox( ELayoutType::Horizontal, { mFilesList, buttons_vl }, "", nullptr, 0, 2, 2, 2, 2 );

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


    // III. Dataset name and buttons group
    //
    mDatasetsCombo = new QComboBox;
    mDatasetsCombo->setToolTip( "List of current workspace datasets" );
    mNewDataset = new QPushButton( "New..." );
    mDeleteDataset = new QPushButton( "Delete..." );
    mRenameDataset = new QPushButton( "Rename..." );
    auto mSaveDataset = new QPushButton( "Save..." );	// TODO confirm that save will only be done in the scope of whole workspace

    mRenameDataset->setDisabled(true);
    mSaveDataset->setDisabled(true);

    AddTopGroupBox(
        ELayoutType::Grid, {
            mDatasetsCombo, mNewDataset, mDeleteDataset, mRenameDataset, nullptr, 
			mSaveDataset
        },
        "", 0, 0, 0, 0, 0
        );


    Wire();
}



void CDatasetBrowserControls::Wire()
{
	connect( mDatasetsCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( DatasetChanged( int ) ) );
	connect( mFilesList, SIGNAL( currentRowChanged( int ) ), this, SLOT( FileChanged( int ) ) );


    connect( mAddFiles, SIGNAL( clicked(bool) ), this, SLOT( OnAddFiles() ) );
    connect( mAddDir, SIGNAL( clicked(bool) ), this, SLOT( OnAddDir() ) );
    connect( mRemove, SIGNAL( clicked(bool) ), this, SLOT( OnRemoveFile() ) );
    connect( mClear, SIGNAL( clicked(bool) ), this, SLOT( OnClearFiles() ) );

    connect( mFieldList, SIGNAL( itemSelectionChanged() ), this, SLOT( FieldChanged() ) );

    connect( mNewDataset, SIGNAL( clicked(bool) ), this, SLOT (OnNewDataset()) );
    connect( mDeleteDataset, SIGNAL( clicked(bool) ), this, SLOT (OnDeleteDataset()) );
}



void CDatasetBrowserControls::WorkspaceChanged( CWorkspaceDataset *wksd )
{
    mWks = wksd;

    // Fill ComboBox with Datasets list
    mDatasetsCombo->clear();
    if (wksd)
    {
        FillCombo( mDatasetsCombo, *mWks->GetDatasets(),

            []( const CObMap::value_type &i ) -> const char*
            {
                return i.first.c_str();
            },
            0, true
        );

    }
}


void CDatasetBrowserControls::DatasetChanged( int current_index )
{
    // Assert index of selected Dataset
    assert__( current_index == mDatasetsCombo->currentIndex() );

    WaitCursor wait;

    // Clear list of files
    mFilesList->clear();

    // If not empty or a Dataset is selected
    if ( current_index < 0 || !mWks->HasDataset() )
    {
        ClearFieldList();
    }
    else
    {
        FillFileList();
    }
}



void CDatasetBrowserControls::FileChanged( int file_index )
{
    // Assert index of selected file
    assert__( file_index == mFilesList->currentRow() );

    WaitCursor wait;

    // Clear field list and descriptions
    ClearFieldList();

    if ( file_index >= 0 )
    {
        FillFieldList();
    }

	QString path;
	if ( file_index >= 0 )
		path = mFilesList->currentItem()->text();

	emit FileChanged( path );
}



void CDatasetBrowserControls::FieldChanged()
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



void CDatasetBrowserControls::OnNewDataset()
{
    if( mWks == nullptr )
    {
        return;
    }

    WaitCursor wait;

    mDatasetsCombo->setEnabled(true);

    // Get name for the new dataset
    QString dataset_name ( mWks->GetDatasetNewName().c_str() );

    // Insert dataset in workspace and into DatasetCombo
    mWks->InsertDataset( dataset_name.toStdString() );

    mDatasetsCombo->addItem( dataset_name );
    mDatasetsCombo->setCurrentIndex( mDatasetsCombo->findText( dataset_name ) );

    // -TODO(Delete)-- Old Brat code - mWks->GetDatasetNewName() ensures that new dataset does not exists ------
    //else
    //{
    //    SimpleWarnBox( QString("Dataset '%s' already exists.").sprintf( dataset_name.toStdString().c_str() ));
    //}
    // ----------------------------------------------------------------------------------------------------------

    // Clear list of files and fields list
    mFilesList->clear();
    ClearFieldList();

    // -TODO -- Old Brat code ----------------------
    //EnableCtrl();
    //
    //CNewDatasetEvent ev(GetId(), dsName);
    //wxPostEvent(GetParent(), ev);
    // ---------------------------------------------
}



void CDatasetBrowserControls::OnDeleteDataset()
{
    if( mWks == nullptr || mDatasetsCombo->currentIndex() < 0 )
    {
        return;
    }

    // Get current dataset
    CDataset *current_dataset = mWks->GetDataset( mDatasetsCombo->currentText().toStdString() );

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

    if( !mWks->DeleteDataset( current_dataset ) )
    {
        SimpleWarnBox( QString( "Unable to delete dataset '%s1").arg(
                                current_dataset->GetName().c_str()  ) );
        return;
    }

    mDatasetsCombo->removeItem( mDatasetsCombo->currentIndex() );
    mDatasetsCombo->setCurrentIndex( mDatasetsCombo->count() - 1 );

    // -TODO -- Old Brat code ----------------------
    //EnableCtrl();
    //
    //CNewDatasetEvent ev(GetId(), dsName);
    //wxPostEvent(GetParent(), ev);
    // ---------------------------------------------
}



void CDatasetBrowserControls::OnAddFiles()
{
    // If empty or no Dataset is selected
    if ( mDatasetsCombo->currentIndex() < 0 || !mWks->HasDataset() )
    {
        return;
    }

    // TODO: Change mWks->GetPath() by the last data path ??
    QStringList paths_list = BrowseFiles( this, "Select files...", mWks->GetPath().c_str() );

    AddFiles(paths_list);
}


void CDatasetBrowserControls::OnAddDir()
{
    // If empty or no Dataset is selected
    if ( mDatasetsCombo->currentIndex() < 0 || !mWks->HasDataset() )
    {
        return;
    }

    // TODO: Change mWks->GetPath() by the last data path ??
    QString dir_path = BrowseDirectory( this, "Select a directory...", mWks->GetPath().c_str() );
	if ( dir_path.isEmpty() )			//TODO RCCC - add if necessary what to do if the user cancels
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



void CDatasetBrowserControls::OnRemoveFile()
{
    if ( mDatasetsCombo->currentIndex() < 0 || mFilesList->currentRow() < 0 )
    {
        return;
    }

    // Delete product from current dataset
    CDataset *current_dataset = mWks->GetDataset( q2a( mDatasetsCombo->currentText() ) );
    current_dataset->EraseProduct( q2a( mFilesList->currentItem()->text() ) );

    // Delete product in Files list
    qDeleteAll ( mFilesList->selectedItems() );

    mFilesList->setCurrentRow( -1 );
    ClearFieldList();
}



void CDatasetBrowserControls::OnClearFiles()
{
    if ( mDatasetsCombo->currentIndex() < 0 || mFilesList->count() == 0 )
    {
        return;
    }

    if ( !SimpleQuestion("Are you sure to remove all files of the current dataset?") )
    {
        return;
    }

    // Delete all products from current dataset
    CDataset *current_dataset = mWks->GetDataset( q2a( mDatasetsCombo->currentText() ) );
    current_dataset->ClearProductList();

    // Delete all products in Files list
    mFilesList->clear();

    ClearFieldList();
}


void CDatasetBrowserControls::AddFiles(QStringList &paths_list)
{
    if (paths_list.empty())
    {
        return;
    }

    paths_list.sort();

// -TODO-- Old Brat code for saving last data path -----
//    wxFileName currentDir;

//    currentDir.Assign(paths[0]);
//    currentDir.Normalize();
//    wxGetApp().SetLastDataPath(currentDir.GetPath());
// ----------------------------------------------------

    CStringList file_list;
    foreach(QString path, paths_list)
    {
        file_list.InsertUnique( path.toStdString() );
    }

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

    // Insert file_list into current Dataset
    CDataset *current_dataset = mWks->GetDataset( mDatasetsCombo->currentText().toStdString() );
    current_dataset->InsertUniqueProducts(file_list);

    // Clear list of files and fill with new list
    mFilesList->clear();
    FillFileList();

    CheckFiles();
}



void CDatasetBrowserControls::FillFileList()
{
    // Get current Dataset            (do not use this before ensuring "there are" datasets)
    CDataset *current_dataset  = mWks->GetDataset( mDatasetsCombo->currentText().toStdString() );

    // Fill FileList with files of current Dataset
    QIcon fileIcon = QIcon(":/images/OSGeo/db.png");

    for ( auto const it : *current_dataset->GetProductList() )
    {
        QListWidgetItem *file = new QListWidgetItem( it.c_str() );
        file->setToolTip( it.c_str() );			//see complete path when it exceeds list width
        file->setIcon( fileIcon );

        mFilesList->addItem( file );
    }
    mFilesList->setCurrentRow( 0 );
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

    try
    {
        // Get current Dataset
        CDataset *current_dataset = mWks->GetDataset( mDatasetsCombo->currentText().toStdString() );
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



void CDatasetBrowserControls::FillFieldList()
{
    if ( mDatasetsCombo->currentIndex() < 0 || !mWks->HasDataset() )
        return;

    CDataset *current_dataset  = mWks->GetDataset( mDatasetsCombo->currentText().toStdString() );	//do not use this before ensuring "there are" datasets

    CProduct *product = nullptr;
    try
    {
        product = current_dataset->SetProduct( q2t<std::string>( mFilesList->currentItem()->text() ) );

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
