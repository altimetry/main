#include "new-gui/brat/stdafx.h"

#include "GUI/DisplayWidgets/TextWidget.h"

#include "ExportDialog.h"




CExportDialog::CExportDialog( QWidget *parent )
    : base_t( parent )
{

    CreateWidgets();
}

CExportDialog::~CExportDialog()
{}


void  CExportDialog::CreateWidgets()
{
    //	Create

    //	... Help

    auto help = new CTextWidget;
    help->setHelpProperties(
                "The user can choose the name and location of the exported operation file. "
                "Several formats are vailable: NetCDF, Ascii and GeoTiff"
         , 6, Qt::AlignCenter );
    auto help_group = CreateGroupBox( ELayoutType::Grid, { help }, "", nullptr, 6, 6, 6, 6, 6 );
    help_group->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Maximum );


    //	... Buttons

    mButtonBox = new QDialogButtonBox( this );
    mButtonBox->setObjectName( QString::fromUtf8( "mButtonBox" ) );
    mButtonBox->setOrientation( Qt::Horizontal );
    mButtonBox->setStandardButtons( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
    mButtonBox->button( QDialogButtonBox::Ok )->setDefault( true );


    QBoxLayout *main_l =
            LayoutWidgets( Qt::Vertical,
                            {
                                help_group,
                                mButtonBox

                            }, this, 6, 6, 6, 6, 6 );                       Q_UNUSED( main_l );


    setWindowTitle( "Delay Execution...");

    //	Wrap up dimensions

    adjustSize();
    setMinimumWidth( width() );

//    Wire();
}

///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_ExportDialog.cpp"
