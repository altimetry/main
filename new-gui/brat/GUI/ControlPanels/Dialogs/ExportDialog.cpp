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

    // I. Output file
    //
    QLineEdit *OutputPath = new QLineEdit();
    QToolButton *BrowseButton = CreateToolButton( "Browse...", "", "<b>Browse...</b><br>Select a output file." );

    QBoxLayout *lOutputFile = LayoutWidgets( Qt::Horizontal, { new QLabel( "Output File: "), OutputPath, BrowseButton }, nullptr, 6, 6, 6, 6, 6 );

    // II. Export format
    //

    // III. Export options
    //


    // IV. Help
    //
    auto help = new CTextWidget;
    help->setHelpProperties(
                "The user can choose the name and location of the exported operation file. "
                "Several formats are vailable:\n"
                " - NetCDF (default format, it can be used as source data in a new dataset,\n"
                " opened using Brat in display mode, or used with any other tool reading netCDF)\n"
                " - Ascii (once saved, can be seen through a built-in text viewer 'Edit Ascii Export')\n"
                " - GeoTiff (only available if the axis of the operation are longitude and latitude,"
                " also provides a Google Earth KML export format)"
         , 6, Qt::AlignJustify );
    auto help_group = CreateGroupBox( ELayoutType::Grid, { help }, "", nullptr, 6, 6, 6, 6, 6 );
    help_group->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Maximum );


    //	V. Buttons (Execute, Delay Execution, Cancel)
    //
    mButtonBox = new QDialogButtonBox();
    mButtonBox->setObjectName( QString::fromUtf8( "mButtonBox" ) );
    mButtonBox->setOrientation( Qt::Horizontal );
    mButtonBox->setStandardButtons( QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel );
    //mButtonBox->button( QDialogButtonBox::Ok )->setDefault( true );
    mButtonBox->button( QDialogButtonBox::Ok )->setText( "Delay execution..." );
    mButtonBox->button( QDialogButtonBox::Apply )->setText( "Execute" );


    QBoxLayout *main_l =
            LayoutWidgets( Qt::Vertical,
                            {
                                lOutputFile,
                                help_group,
                                mButtonBox

                            }, this, 6, 6, 6, 6, 6 );                       Q_UNUSED( main_l );


    setWindowTitle( "Export...");

    //	Wrap up dimensions

    adjustSize();
    setMinimumWidth( width() );

    Wire();
}


void  CExportDialog::Wire()
{
    //	Setup things

    connect( mButtonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( mButtonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );
}



//virtual
void  CExportDialog::accept()
{

    base_t::accept();
}


////////////////////////////////////////////////////
// CEditExportAsciiDialog
////////////////////////////////////////////////////
CEditExportAsciiDialog::CEditExportAsciiDialog( QWidget *parent )
    : base_t( parent )
{

    CreateWidgets();
}

CEditExportAsciiDialog::~CEditExportAsciiDialog()
{}


void  CEditExportAsciiDialog::CreateWidgets()
{
    //	Create

    //	... Help

    auto help = new CTextWidget;
    help->setHelpProperties(
                "It can be used to visualize and edit the exported Ascii files."
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


    setWindowTitle( "Edit Export Ascii...");

    //	Wrap up dimensions

    adjustSize();
    setMinimumWidth( width() );

   Wire();
}


void  CEditExportAsciiDialog::Wire()
{
    //	Setup things

    connect( mButtonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( mButtonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );
}



//virtual
void  CEditExportAsciiDialog::accept()
{

    base_t::accept();
}


///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_ExportDialog.cpp"
