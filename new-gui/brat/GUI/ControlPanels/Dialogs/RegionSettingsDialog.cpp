#include "new-gui/brat/stdafx.h"

#include "GUI/DisplayWidgets/TextWidget.h"

#include "RegionSettingsDialog.h"





CRegionSettingsDialog::CRegionSettingsDialog( QWidget *parent, CBratRegions &BratRegions, CBratAreas &BratAreas )
    : base_t( parent ), mBratRegions(BratRegions), mBratAreas(BratAreas)
{

    CreateWidgets();

    FillRegionsCombo();
    FillAreasList();
}

CRegionSettingsDialog::~CRegionSettingsDialog()
{}


void  CRegionSettingsDialog::CreateWidgets()
{
    //	Create

    // I. Region buttons
    //
    mNewRegion = CreateToolButton( "", ":/images/OSGeo/region_new.png", "<b>Create region</b><br>Create a new region in selected filter" );
    mRenameRegion = CreateToolButton( "", ":/images/OSGeo/region_edit.png", "<b>Rename region</b><br>Change name of selected region" );
    mDeleteRegion = CreateToolButton( "", ":/images/OSGeo/region_delete.png", "<b>Delete region</b><br>Delete the selected region" );
    //mSaveRegion = CreateToolButton( "", ":/images/OSGeo/region_save.png", "<b>Save regions</b><br>Save region configuration" );

    mRegionsCombo = new QComboBox;
    mRegionsCombo->setToolTip( "List of regions" );

    QWidget *buttons_regions = CreateButtonRow( false, Qt::Horizontal, { mNewRegion, mRenameRegion, mDeleteRegion, /*mSaveRegion,*/ nullptr, new QLabel("Selected Region"), mRegionsCombo } );


    // II. List of areas
    //
    mAreasListWidget = new QListWidget( this );
    mAreasListWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    mAreasListWidget->setSelectionMode(QAbstractItemView::MultiSelection); // Multiple items can be selected

    QGroupBox *areas_box = CreateGroupBox( ELayoutType::Vertical, { mAreasListWidget }, "All Areas", this );


    // III. Help
    //
    auto help = new CTextWidget;
    help->SetHelpProperties(
                "The user can create, edit, delete and save regions.\n"
                "The list of areas can be used to select which of them are included on each region."
         ,0 , 6, Qt::AlignJustify );
    auto help_group = CreateGroupBox( ELayoutType::Grid, { help }, "", nullptr, 6, 6, 6, 6, 6 );
    help_group->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Maximum );


    //	IV. Buttons (Execute, Delay Execution, Cancel)
    //
    mButtonBox = new QDialogButtonBox();
    mButtonBox->setObjectName( QString::fromUtf8( "mButtonBox" ) );
    mButtonBox->setOrientation( Qt::Horizontal );
    mButtonBox->setStandardButtons( QDialogButtonBox::Close );
    mButtonBox->button( QDialogButtonBox::Close )->setDefault( true );

    // V. Add widgets to dialog
    QBoxLayout *main_l =
            LayoutWidgets( Qt::Vertical,
                            {
                                buttons_regions,
                                areas_box,
                                help_group,
                                mButtonBox

                            }, this, 6, 6, 6, 6, 6 );                       Q_UNUSED( main_l );


    setWindowTitle( "Region settings...");

    //	Wrap up dimensions
    adjustSize();
    setMinimumWidth( width() );

    Wire();
}


void  CRegionSettingsDialog::Wire()
{
    //	Setup things

    //connect( mButtonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( mButtonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );

    connect( mRegionsCombo, SIGNAL( currentIndexChanged(int) ), this, SLOT( HandleRegionsCurrentIndexChanged(int) ) );

    connect( mNewRegion, SIGNAL( clicked() ), this, SLOT( HandleNewRegion() ) );
    connect( mRenameRegion, SIGNAL( clicked() ), this, SLOT( HandleRenameRegion() ) );
    connect( mDeleteRegion, SIGNAL( clicked() ), this, SLOT( HandleDeleteRegion() ) );
    //connect( mSaveRegion, SIGNAL( clicked() ), this, SLOT( HandleSaveRegion() ) );

    //connect( mAreasListWidget, SIGNAL( itemChanged(QListWidgetItem*) ), this, SLOT( HandleAreaChecked(QListWidgetItem*) ) );
    connect( mAreasListWidget, SIGNAL( itemClicked(QListWidgetItem*) ), this, SLOT( HandleAreaClicked(QListWidgetItem*) ) );
}


void CRegionSettingsDialog::FillRegionsCombo()
{
    mRegionsCombo->clear();
    HandleRegionsCurrentIndexChanged( -1 );  // for updating buttons status

    auto &rmap = mBratRegions.RegionsMap();
    for ( auto &region_entry : rmap )
    {
        mRegionsCombo->addItem( region_entry.first.c_str() );
    }
}


void CRegionSettingsDialog::FillAreasList()
{
    mAreasListWidget->clear();

    // Fill all areas
    auto &amap = mBratAreas.AreasMap();
    for ( auto &area_entry : amap )
    {
        auto &area = area_entry.second;
        //QListWidgetItem* item = new QListWidgetItem;
        //item->setFlags( item->flags() | Qt::ItemIsUserCheckable );
        QListWidgetItem* item = new QListWidgetItem(mAreasListWidget);
        item->setText( t2q(area.Name())  );

        if ( mCurrentRegion == nullptr )
        {
            //item->setCheckState( Qt::Unchecked );
            item->setSelected( false );
        }
        else
        {
            // TODO include in a new method in CRegion?
            bool isOnRegion = std::find( mCurrentRegion->begin(), mCurrentRegion->end(), area.Name() ) != mCurrentRegion->end();
            //item->setCheckState( isOnRegion ? Qt::Checked : Qt::Unchecked );
            item->setSelected( isOnRegion ? true : false );
        }

        //mAreasListWidget->addItem( item );
    }

    // Sort items (ascending order)
    mAreasListWidget->sortItems();
}



void CRegionSettingsDialog::HandleNewRegion()
{
    auto result = ValidatedInputString( "Region Name", mBratRegions.MakeNewName(), "New Region..." );
    if ( !result.first )
        return;

    if ( !mBratRegions.AddRegion( CRegion(result.second) ) )
        SimpleMsgBox( "A area with same name already exists." );
    else
    {
        FillRegionsCombo();
        mRegionsCombo->setCurrentIndex( mRegionsCombo->findText( result.second.c_str() ) );

        // Save all regions
        SaveAllRegions();
    }

}

void CRegionSettingsDialog::HandleRenameRegion()
{
    std::string region_name = mRegionsCombo->currentText().toStdString();

    auto result = ValidatedInputString( "Region Name", region_name, "Rename Region..." );
    if ( !result.first )
        return;

    if ( !mBratRegions.RenameRegion( region_name, result.second ) )
        SimpleMsgBox( "A region with same name already exists." );
    else
    {
        FillRegionsCombo();
        mRegionsCombo->setCurrentIndex( mRegionsCombo->findText( result.second.c_str() ));

        // Save all regions
        SaveAllRegions();
    }
}


void CRegionSettingsDialog::HandleDeleteRegion()
{
    std::string region_name = mRegionsCombo->currentText().toStdString();

    if ( SimpleQuestion( "Are you sure you want to delete region '" + region_name + "' ?" ) )
    {
        if ( !mBratRegions.DeleteRegion( region_name ) )
            SimpleErrorBox( "Region '" + region_name + "' was not found!" );
        else
        {
            FillRegionsCombo();
            mRegionsCombo->setCurrentIndex( 0 );

            // Save all regions
            SaveAllRegions();
        }
    }
}

void CRegionSettingsDialog::SaveAllRegions()
{
    //std::string region_name = mRegionsCombo->currentText().toStdString();
    //CRegion *region = mBratRegions.Find( region_name );

    if ( !mBratRegions.Save() )
        SimpleWarnBox( "There was a problem saving region to '" + mBratRegions.FilePath() + "'. Some information could be lost or damaged." );
}


void CRegionSettingsDialog::HandleRegionsCurrentIndexChanged(int region_index)
{
    mRenameRegion->setDisabled( region_index < 0 );
    mDeleteRegion->setDisabled( region_index < 0 );

    if ( region_index < 0 )
    {
        mCurrentRegion = nullptr;
    }
    else
    {
        mCurrentRegion = mBratRegions.Find( mRegionsCombo->currentText().toStdString() );
    }

    FillAreasList();
}


void CRegionSettingsDialog::HandleAreaClicked(QListWidgetItem *area_item)
{
    if ( mCurrentRegion == nullptr )
    {
        return;
    }

    std::string name_region = q2a( mRegionsCombo->currentText() );
    CRegion *region = mBratRegions.Find( name_region );

    if ( area_item->isSelected() )
    {
        region->push_back( area_item->text().toStdString()  );
    }
    else
    {
        auto it_region = std::find( region->begin(), region->end(), area_item->text().toStdString() );
        if ( it_region != region->end() )
        {
            region->erase( it_region );
        }
    }

    // Save all regions
    SaveAllRegions();
}


//void CRegionSettingsDialog::HandleAreaChecked(QListWidgetItem *area_item)
//{
//    std::string name_region = q2a( mRegionsCombo->currentText() );
//    CRegion *region = mBratRegions.Find( name_region );

//    if ( area_item->checkState() == Qt::Checked )
//    {
//        region->push_back( area_item->text().toStdString()  );
//    }
//    else
//    {
//        auto it_region = std::find( region->begin(), region->end(), area_item->text().toStdString() );
//        if ( it_region != region->end() )
//        {
//            region->erase( it_region );
//        }
//    }

//    // Save all regions
//    SaveAllRegions();
//}


//virtual
void  CRegionSettingsDialog::accept()
{

    base_t::accept();
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_RegionSettingsDialog.cpp"
