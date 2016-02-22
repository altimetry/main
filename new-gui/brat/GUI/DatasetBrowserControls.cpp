#include "new-gui/brat/stdafx.h"

//#include "new-gui/brat/ApplicationLogger.h" ////

#include "new-gui/brat/DataModels/Workspaces/Workspace.h"

#include "new-gui/brat/Views/TextWidget.h"
//#include "new-gui/brat/Views/2DPlotWidget.h" ///
//#include "new-gui/brat/Views/3DPlotWidget.h" ///

#include "new-gui/brat/GUI/DatasetBrowserControls.h"

//#include "libbrathl/TreeField.h"
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

    auto mAddFiles = new QPushButton( "Add Files" );
    auto mAddDir = new QPushButton( "Add Dir" );
    auto mUp = new QPushButton( "Up" );
    auto mDown = new QPushButton( "Down" );			//QPushButton *mSort = new QPushButton( "Sort", page_1 );	by QListWidget ?
    auto mRemove = new QPushButton( "Remove" );
    auto mClear = new QPushButton( "Clear" );
    auto mCheckFiles = new QPushButton( "Check" );
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
    mFieldDesc = new CTextWidget;
    mFieldDesc->setReadOnly(true);
    mFieldDesc->setPlainText("bla bla bla with the full....");

    AddTopGroupBox( ELayoutType::Horizontal, { mFieldList, mFieldDesc }, "Field Description", 4, 4, 4, 4, 4 );


    // III. Dataset name and buttons group
    //
    mDatasetsCombo = new QComboBox;
    mDatasetsCombo->setToolTip( "List of current workspace datasets" );
    auto mNewDataset = new QPushButton( "New..." );
    auto mDeleteDataset = new QPushButton( "Delete..." );
    auto mRenameDataset = new QPushButton( "Rename..." );

    auto mSaveDataset = new QPushButton( "Save..." );	// TODO confirm that save will only be done in the scope of whole workspace

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
	connect( mFieldList, SIGNAL( currentRowChanged( int ) ), this, SLOT( FieldChanged( int ) ) );
}



const QString& qidentity( const QString &s ){  return s; }

template <
    typename COMBO, typename CONTAINER, typename FUNC = decltype( qidentity )
>
inline void FillCombo( COMBO *c, const CONTAINER &items, const FUNC &f, int selected = 0, bool enabled = true )
{
    for ( auto i : items )
    {
        c->addItem( f( i ) );
    }
    c->setCurrentIndex( selected );
    c->setEnabled( enabled );
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
        ClearVarList();
    }
    else
    {
        // Get current Dataset
        CDataset *current_dataset  = mWks->GetDataset( current_index );	//do not use this before ensuring "there are" datasets
        //CObMap::const_iterator it = rDatasets.begin();
        //std::advance(it, current_index);
        //dynamic_cast< CDataset* >( it->second );   assert__(current_dataset);

        // Fill FileList with files of current Dataset
        std::vector<std::string> file_names;
        current_dataset->GetFiles( file_names );

        QIcon fileIcon = QIcon(":/images/OSGeo/db.png");
        int index = 0;
        for ( auto const &it : file_names )
        {
            QListWidgetItem *file = new QListWidgetItem( it.c_str() );
            file->setToolTip( it.c_str() );						//see complete path when it exceeds list width
            file->setIcon( fileIcon );

            mFilesList->insertItem(index, file);
            index++;
        }
        mFilesList->setCurrentRow( 0 );
    }
}


void CDatasetBrowserControls::FileChanged( int file_index )
{
    // Assert index of selected file
    assert__( file_index == mFilesList->currentRow() );

    WaitCursor wait;

    ClearVarList();

    if ( file_index >= 0 )
    {
        FillFieldList();
    }

	QString path;
	if ( file_index >= 0 )
		path = mFilesList->currentItem()->text();

	emit FileChanged( path );
}

void CDatasetBrowserControls::FieldChanged(int fieldIndex)
{
    WaitCursor wait;

    if ( fieldIndex >= 0 )
    {
        SetFieldDesc();
    }
}


void CDatasetBrowserControls::ClearVarList()  // the old method was: ClearDict();
{
//    if (m_dataset != NULL)
//    {
//      //m_dataset->DeleteProduct();
//      DeleteProduct();
//    }
//    GetProductLabel()->SetLabel("");
//    //GetDictlist()->DeleteAllItems();
//    GetDictlist()->InsertProduct(NULL);
//    //GetFielddesc()->SetValue("");

    mFieldList->clear();

    mFileDescGroup->setDisabled(true);
    mFileDesc->setDisabled(true);
    mFileDesc->clear();

}




void CDatasetBrowserControls::FillFieldList()
{

    int current_index = mDatasetsCombo->currentIndex();
    if ( current_index < 0 || !mWks->HasDataset() )
        return;

    CDataset *current_dataset  = mWks->GetDataset( current_index );	//do not use this before ensuring "there are" datasets

    CProduct *product = nullptr;
    try
    {
        product = current_dataset->SetProduct( q2t<std::string>( mFilesList->currentItem()->text() ) );

        //GetDictlist()->InsertProduct(m_product); //////////////////////////////////////////////
        CTreeField* tree = product->GetTreeField();

        for ( auto const *object : *tree )
        {
            const CField *field  = static_cast< const CField* >( object );			assert__( dynamic_cast< const CField*>( object ) != nullptr );

            //InsertField(field); ///////////////////////////////
            auto const &type_id = typeid( *field );

            if ( type_id == typeid( CFieldIndex )
                ||  type_id == typeid( CFieldRecord )
                || ( ( type_id == typeid( CFieldArray ) ) && ( field->IsFixedSize() == false ) )
                || field->IsVirtual() )
            {
                continue;
            }

            QListWidgetItem *item = new QListWidgetItem;
            QVariant fieldVariant ( QString( field->GetDescription().c_str() ));


            item->setData(Qt::UserRole, fieldVariant);
            item->setText( const_cast<CField*>( field )->GetFullName().c_str() );

            //QListWidgetItem *item = new QListWidgetItem( const_cast<CField*>( field )->GetFullName().c_str() );
            //item->setData( Qt::UserRole, field );

            mFieldList->addItem( item );
            // //////////////////////////////////////////////////

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
            //GetProductLabel()->SetLabel(wxString::Format("Product : %s / %s",
            //                                             m_product->GetProductClass().c_str(),
            //                                             m_product->GetProductType().c_str()));
            mFileDescGroup->setEnabled(product->IsNetCdf());

            mFileDesc->setEnabled( product->IsNetCdf() );
            mFileDesc->setText( product->GetDescription().c_str() );
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


void CDatasetBrowserControls::SetFieldDesc()
{
    mFieldDesc->clear();

//            void *pvnow = item->data( 0 ).value< void* >();
//            CField *now = (CField*)pvnow;

//            CField *future = item->data( 0 ).value< CField* >();

    QVariant fieldVariant = mFieldList->currentItem()->data(Qt::UserRole);

    //void *pfield = fieldVariant.value<void*>();
//    void *pfield =  (void*)mFieldList->currentItem()->data(Qt::UserRole).value<unsigned long long>();
//    CField *field = (CField*)pfield;

    qDebug() << mFieldList->currentItem()->text();

    //const CField *field = static_cast< const CField* > (pfield);

    //std::string description;
//    if (field == nullptr)
//    {
//        qDebug() << "NULL";
//        return;
//    }

    //field->GetDescription() );

    QString separator (":\n---------------------------\n");

    mFieldDesc->setPlainText( separator + fieldVariant.toString()  );





}


///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_DatasetBrowserControls.cpp"
