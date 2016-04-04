#include "new-gui/brat/stdafx.h"

#include "GUI/DisplayWidgets/TextWidget.h"

#include "RegionSettingsDialog.h"





CRegionSettingsDialog::CRegionSettingsDialog( QWidget *parent, CBratRegions &BratRegions, CBratAreas &BratAreas )
    : base_t( parent ), mBratRegions(BratRegions), mBratAreas(BratAreas)
{

    CreateWidgets();
}

CRegionSettingsDialog::~CRegionSettingsDialog()
{}


void  CRegionSettingsDialog::CreateWidgets()
{
    //	Create

    // I. Region layout
    //
    QLineEdit *RegionName = new QLineEdit();
    QBoxLayout *Region_layout = LayoutWidgets( Qt::Horizontal, { new QLabel( "Region name: "), RegionName }, nullptr, 6, 6, 6, 6, 6 );


    // II. Region buttons
    //
    mNewRegion = CreateToolButton( "", ":/images/OSGeo/region_new.png", "<b>Create region</b><br>Create a new region in selected filter" );
    mRenameRegion = CreateToolButton( "", ":/images/OSGeo/region_edit.png", "<b>Rename region</b><br>Change name of selected region" );
    mDeleteRegion = CreateToolButton( "", ":/images/OSGeo/region_delete.png", "<b>Delete region</b><br>Delete the selected region" );
    mSaveRegion = CreateToolButton( "", ":/images/OSGeo/region_save.png", "<b>Save region</b><br>Save values in selected region" );

    QWidget *buttons_regions = CreateButtonRow( false, Qt::Horizontal, { mNewRegion, mRenameRegion, mDeleteRegion, mSaveRegion } );
    //QBoxLayout *regions_layout = LayoutWidgets( Qt::Horizontal, { buttons_regions, nullptr, new QLabel( "Selected Region"), mRegionsCombo });

    // III. Help
    //
    auto help = new CTextWidget;
    help->SetHelpProperties(
                "The user can create, edit, delete and save regions.\n"
                "The list of areas can be used to select which of them are included on each region."
         ,0 , 6, Qt::AlignJustify );
    auto help_group = CreateGroupBox( ELayoutType::Grid, { help }, "", nullptr, 6, 6, 6, 6, 6 );
    help_group->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Maximum );


    //	VI. Buttons (Execute, Delay Execution, Cancel)
    //
    mButtonBox = new QDialogButtonBox();
    mButtonBox->setObjectName( QString::fromUtf8( "mButtonBox" ) );
    mButtonBox->setOrientation( Qt::Horizontal );
    mButtonBox->setStandardButtons( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
    mButtonBox->button( QDialogButtonBox::Ok )->setDefault( true );

    // V. Add widgets to dialog
    QBoxLayout *main_l =
            LayoutWidgets( Qt::Vertical,
                            {
                                Region_layout,
                                buttons_regions,
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

    connect( mButtonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( mButtonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );
}



/*
void CDatasetFilterControls::HandleNewRegion()
{
    auto result = SimpleInputString( "Region Name", mBratRegions.MakeNewName(), "New Region..." );
    if ( !result.first )
        return;

    if ( !mBratRegions.AddRegion( CRegion(result.second) ) )
        SimpleMsgBox( "A area with same name already exists." );
    else
    {
        FillRegionsCombo();
        mRegionsCombo->setCurrentIndex( mRegionsCombo->findText( result.second.c_str() ) );

        // TODO Refresh all other pertinent widgets
    }
}

void CDatasetFilterControls::HandleRenameRegion()
{
    std::string region_name = mRegionsCombo->currentText().toStdString();

    auto result = SimpleInputString( "Region Name", region_name, "Rename Region..." );
    if ( !result.first )
        return;

    if ( !mBratRegions.RenameRegion( region_name, result.second ) )
        SimpleMsgBox( "A region with same name already exists." );
    else
    {
        FillRegionsCombo();
        mFiltersCombo->setCurrentIndex( mRegionsCombo->findText( result.second.c_str() ));

        // TODO Refresh all other pertinent widgets
    }
}


void CDatasetFilterControls::HandleDeleteRegion()
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
        }
    }
}

void CDatasetFilterControls::HandleSaveRegion()
{
    std::string region_name = mRegionsCombo->currentText().toStdString();

    CRegion *region = mBratRegions.Find( region_name );

    Q_UNUSED( region );

    // TODO
    // Add selected areas to region

    if ( !mBratRegions.Save() )
        SimpleWarnBox( "There was a problem saving region to '" + mBratRegions.FilePath() + "'. Some information could be lost or damaged." );
}
*/



//virtual
void  CRegionSettingsDialog::accept()
{

    base_t::accept();
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_RegionSettingsDialog.cpp"
